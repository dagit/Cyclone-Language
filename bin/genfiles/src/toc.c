#include "cyc_include.h"

 struct _tuple0{ void* f1; void* f2; } ; struct _tuple1{ void* f1; struct
_tagged_string* f2; } ; struct _tuple2{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual*
f2; void* f3; } ; struct _tuple3{ struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt*
f2; } ; struct _tuple4{ struct _tagged_string* f1; struct Cyc_List_List* f2; } ;
struct _tuple5{ struct Cyc_Absyn_Tqual* f1; void* f2; } ; struct _tuple6{ struct
_tagged_string* f1; void* f2; } ; struct _tuple7{ struct Cyc_List_List* f1;
struct Cyc_Absyn_Exp* f2; } ; struct _tuple8{ struct _tagged_string* f1; struct
Cyc_List_List* f2; struct Cyc_Dict_Dict* f3; } ; struct _tuple9{ struct Cyc_List_List*
f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3; struct Cyc_List_List* f4;
char f5; int f6; } ; struct _tuple10{ struct _tuple1* f1; void* f2; struct Cyc_Absyn_Exp*
f3; } ; struct _tuple11{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Exp* f2; } ;
struct _tuple12{ struct Cyc_Absyn_Structfield* f1; struct Cyc_Absyn_Exp* f2; } ;
struct _tuple13{ struct Cyc_Toc_Env* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Stmt*
f3; } ; struct _tuple14{ struct _tuple1* f1; void* f2; } ; struct _tuple15{
struct Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ; struct _tuple16{ struct
_tagged_string* f1; struct _tagged_string* f2; struct Cyc_Absyn_Switch_clause*
f3; } ; struct _tuple17{ struct Cyc_Toc_Env* f1; struct Cyc_List_List* f2; } ;
struct _tuple18{ struct _tagged_string* f1; struct Cyc_Absyn_Tqual* f2; void* f3;
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
Cstring_to_string( char*); extern int system( char*); struct Cyc_Stdio___sFILE;
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; extern struct Cyc_Stdio___sFILE*
Cyc_Stdio_stderr; typedef int Cyc_Stdio_fpos_t; extern int Cyc_Stdio_fflush(
struct Cyc_Stdio___sFILE*); extern char Cyc_Stdio_FileOpenError_tag[ 14u];
struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Stdio_FileCloseError_tag[ 15u]; struct Cyc_Stdio_FileCloseError_struct{
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
Cyc_List_imp_append( struct Cyc_List_List* x, struct Cyc_List_List* y); extern
struct Cyc_List_List* Cyc_List_flatten( struct Cyc_List_List* x); extern char
Cyc_List_Nth_tag[ 4u]; struct Cyc_List_Nth_struct{ char* tag; } ; extern int Cyc_String_zstrptrcmp(
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
char* tag; } ; extern int Cyc_Absyn_qvar_cmp( struct _tuple1*, struct _tuple1*);
extern struct Cyc_Absyn_Tqual* Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref*
Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_conref_val(
struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_uchar_t; extern void* Cyc_Absyn_uint_t;
extern void* Cyc_Absyn_sint_t; extern void* Cyc_Absyn_exn_typ; extern void* Cyc_Absyn_star_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual* tq); extern void* Cyc_Absyn_cstar_typ(
void* t, struct Cyc_Absyn_Tqual* tq); extern void* Cyc_Absyn_void_star_typ();
extern void* Cyc_Absyn_strct( struct _tagged_string* name); extern void* Cyc_Absyn_strctq(
struct _tuple1* name); extern void* Cyc_Absyn_unionq_typ( struct _tuple1* name);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp( void*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_copy_exp( struct Cyc_Absyn_Exp*); extern
struct Cyc_Absyn_Exp* Cyc_Absyn_int_exp( void*, int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_signed_int_exp( int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp( unsigned int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_char_exp( char c, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_float_exp( struct _tagged_string f,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_string_exp(
struct _tagged_string s, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_var_exp( struct _tuple1*, struct Cyc_Position_Segment*); extern struct
Cyc_Absyn_Exp* Cyc_Absyn_varb_exp( struct _tuple1*, void*, struct Cyc_Position_Segment*);
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
struct _tuple1*, void*, struct Cyc_Absyn_Exp* init, struct Cyc_Absyn_Stmt*,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_label_stmt(
struct _tagged_string* v, struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_goto_stmt( struct _tagged_string*
lab, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_assign_stmt(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Decl* Cyc_Absyn_new_decl( void* r, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Vardecl* Cyc_Absyn_new_vardecl( struct _tuple1* x,
void* t, struct Cyc_Absyn_Exp* init); extern struct Cyc_Absyn_Vardecl* Cyc_Absyn_static_vardecl(
struct _tuple1* x, void* t, struct Cyc_Absyn_Exp* init); extern int Cyc_Absyn_is_lvalue(
struct Cyc_Absyn_Exp*); struct Cyc_PP_Ppstate; typedef struct Cyc_PP_Ppstate*
Cyc_PP_ppstate_t; struct Cyc_PP_Out; typedef struct Cyc_PP_Out* Cyc_PP_out_t;
struct Cyc_PP_Doc; typedef struct Cyc_PP_Doc* Cyc_PP_doc_t; extern struct
_tagged_string Cyc_Absynpp_typ2string( void*); extern struct _tagged_string Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp*); extern struct _tagged_string Cyc_Absynpp_stmt2string(
struct Cyc_Absyn_Stmt*); extern struct _tagged_string Cyc_Absynpp_qvar2string(
struct _tuple1*); extern struct _tagged_string Cyc_Absynpp_prim2string( void* p);
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
struct Cyc_Position_Segment* loc, struct Cyc_List_List* des, struct Cyc_Absyn_Structdecl*
sd); extern int Cyc_Tcutil_is_tagged_pointer_typ( void*); extern unsigned int
Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp* e); extern struct Cyc_Core_Opt*
Cyc_Formatstr_parse_conversionspecification( struct _tagged_string s, int i);
extern struct Cyc_List_List* Cyc_Toc_toc( struct Cyc_List_List* ds); struct Cyc_Absyn_Abs_n_struct
Cyc_Toc_abs_ns_value={ 1u, 0}; void* Cyc_Toc_abs_ns=( void*)& Cyc_Toc_abs_ns_value;
struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq_v=( struct Cyc_Absyn_Tqual){.q_const= 0,.q_volatile=
0,.q_restrict= 0}; struct Cyc_Absyn_Tqual* Cyc_Toc_mt_tq=& Cyc_Toc_mt_tq_v; char
Cyc_Toc_Toc_Unimplemented_tag[ 18u]="Toc_Unimplemented"; struct Cyc_Toc_Toc_Unimplemented_struct{
char* tag; } ; char Cyc_Toc_Toc_Impossible_tag[ 15u]="Toc_Impossible"; struct
Cyc_Toc_Toc_Impossible_struct{ char* tag; } ; static void* Cyc_Toc_unimp( struct
_tagged_string s){({ struct _tagged_string _temp1= s; fprintf( Cyc_Stdio_stderr,"%.*s\n",
_temp1.last_plus_one - _temp1.curr, _temp1.curr);}); Cyc_Stdio_fflush( Cyc_Stdio_stderr);(
void) _throw(( struct _xtunion_struct*)({ struct Cyc_Toc_Toc_Unimplemented_struct*
_temp2=( struct Cyc_Toc_Toc_Unimplemented_struct*) GC_malloc( sizeof( struct Cyc_Toc_Toc_Unimplemented_struct));
_temp2[ 0]=({ struct Cyc_Toc_Toc_Unimplemented_struct _temp3; _temp3.tag= Cyc_Toc_Toc_Unimplemented_tag;
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
struct _tagged_string){ _temp48, _temp48, _temp48 + 14u}; static struct _tuple1
Cyc_Toc__npop_handler_pr=( struct _tuple1){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=&
Cyc_Toc__npop_handler_string}; static struct _tuple1* Cyc_Toc__npop_handler_qv=&
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
_tagged_string* Cyc_Toc__new_region_sp=& Cyc_Toc__new_region_string; static int
Cyc_Toc_is_nullable( void* t){ void* _temp67= Cyc_Tcutil_compress( t); struct
Cyc_Absyn_PtrInfo _temp73; struct Cyc_Absyn_Conref* _temp75; struct Cyc_Absyn_Tqual*
_temp77; struct Cyc_Absyn_Conref* _temp79; void* _temp81; void* _temp83; _LL69:
if(( unsigned int) _temp67 > 4u?(( struct _tunion_struct*) _temp67)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL74: _temp73=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp67)->f1; _LL84: _temp83=( void*) _temp73.elt_typ; goto _LL82; _LL82:
_temp81=( void*) _temp73.rgn_typ; goto _LL80; _LL80: _temp79=( struct Cyc_Absyn_Conref*)
_temp73.nullable; goto _LL78; _LL78: _temp77=( struct Cyc_Absyn_Tqual*) _temp73.tq;
goto _LL76; _LL76: _temp75=( struct Cyc_Absyn_Conref*) _temp73.bounds; goto
_LL70;} else{ goto _LL71;} _LL71: goto _LL72; _LL70: { void* _temp85=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp79))->v; int _temp91; _LL87: if(( unsigned int) _temp85 > 1u?(( struct
_tunion_struct*) _temp85)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL92: _temp91=(
int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp85)->f1; goto _LL88;} else{ goto
_LL89;} _LL89: goto _LL90; _LL88: return _temp91; _LL90: return 0; _LL86:;}
_LL72:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp93=( char*)"is_nullable"; struct _tagged_string
_temp94; _temp94.curr= _temp93; _temp94.base= _temp93; _temp94.last_plus_one=
_temp93 + 12; _temp94;})); return 0; _LL68:;} static struct _tuple1* Cyc_Toc_collapse_qvar_tag(
struct _tuple1* x, struct _tagged_string tag){ return({ struct _tuple1* _temp95=(
struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp95->f1=(* x).f1;
_temp95->f2=({ struct _tagged_string* _temp96=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp96[ 0]= Cyc_String_strconcat(*(*
x).f2, tag); _temp96;}); _temp95;});} static struct Cyc_List_List* Cyc_Toc_added_decls=
0; static void Cyc_Toc_add_tuple_decl( struct _tagged_string* x, struct Cyc_List_List*
ts){ struct Cyc_List_List* fs= 0;{ int i= 1; for( 0; ts != 0; ts=({ struct Cyc_List_List*
_temp97= ts; if( _temp97 == 0){ _throw( Null_Exception);} _temp97->tl;}), i ++){
fs=({ struct Cyc_List_List* _temp98=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp98->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp99=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp99->name=({ struct _tagged_string* _temp101=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp101[ 0]= xprintf("f%d", i);
_temp101;}); _temp99->tq= Cyc_Toc_mt_tq; _temp99->type=( void*)(( void*)({
struct Cyc_List_List* _temp100= ts; if( _temp100 == 0){ _throw( Null_Exception);}
_temp100->hd;})); _temp99->width= 0; _temp99->attributes= 0; _temp99;}); _temp98->tl=
fs; _temp98;});}} fs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fs);{ struct Cyc_Absyn_Structdecl* sd=({ struct Cyc_Absyn_Structdecl* _temp105=(
struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp105->sc=( void*)(( void*) Cyc_Absyn_Public); _temp105->name=({ struct Cyc_Core_Opt*
_temp107=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp107->v=( void*)({ struct _tuple1* _temp108=( struct _tuple1*) GC_malloc(
sizeof( struct _tuple1)); _temp108->f1= Cyc_Toc_abs_ns; _temp108->f2= x;
_temp108;}); _temp107;}); _temp105->tvs= 0; _temp105->fields=({ struct Cyc_Core_Opt*
_temp106=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp106->v=( void*) fs; _temp106;}); _temp105->attributes= 0; _temp105;}); Cyc_Toc_added_decls=({
struct Cyc_List_List* _temp102=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp102->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Struct_d_struct* _temp103=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp103[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp104; _temp104.tag= Cyc_Absyn_Struct_d_tag;
_temp104.f1= sd; _temp104;}); _temp103;}), 0); _temp102->tl= Cyc_Toc_added_decls;
_temp102;});}} static void Cyc_Toc_add_tagged_ptr_decl( struct _tagged_string* x,
void* t){ void* tptr= Cyc_Absyn_star_typ( t,( void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual());
struct Cyc_Absyn_Structfield* f1=({ struct Cyc_Absyn_Structfield* _temp121=(
struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp121->name= Cyc_Toc_curr_sp; _temp121->tq= Cyc_Toc_mt_tq; _temp121->type=(
void*) tptr; _temp121->width= 0; _temp121->attributes= 0; _temp121;}); struct
Cyc_Absyn_Structfield* f2=({ struct Cyc_Absyn_Structfield* _temp120=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp120->name= Cyc_Toc_base_sp;
_temp120->tq= Cyc_Toc_mt_tq; _temp120->type=( void*) tptr; _temp120->width= 0;
_temp120->attributes= 0; _temp120;}); struct Cyc_Absyn_Structfield* f3=({ struct
Cyc_Absyn_Structfield* _temp119=( struct Cyc_Absyn_Structfield*) GC_malloc(
sizeof( struct Cyc_Absyn_Structfield)); _temp119->name= Cyc_Toc_last_plus_one_sp;
_temp119->tq= Cyc_Toc_mt_tq; _temp119->type=( void*) tptr; _temp119->width= 0;
_temp119->attributes= 0; _temp119;}); struct Cyc_List_List* fs=({ struct Cyc_List_List*
_temp116=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp116->hd=( void*) f1; _temp116->tl=({ struct Cyc_List_List* _temp117=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp117->hd=(
void*) f2; _temp117->tl=({ struct Cyc_List_List* _temp118=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp118->hd=( void*) f3; _temp118->tl=
0; _temp118;}); _temp117;}); _temp116;}); struct Cyc_Absyn_Structdecl* sd=({
struct Cyc_Absyn_Structdecl* _temp112=( struct Cyc_Absyn_Structdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp112->sc=( void*)(( void*) Cyc_Absyn_Public);
_temp112->name=({ struct Cyc_Core_Opt* _temp114=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp114->v=( void*)({ struct _tuple1*
_temp115=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp115->f1=
Cyc_Toc_abs_ns; _temp115->f2= x; _temp115;}); _temp114;}); _temp112->tvs= 0;
_temp112->fields=({ struct Cyc_Core_Opt* _temp113=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp113->v=( void*)(( struct Cyc_List_List*)
fs); _temp113;}); _temp112->attributes= 0; _temp112;}); Cyc_Toc_added_decls=({
struct Cyc_List_List* _temp109=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp109->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Struct_d_struct* _temp110=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp110[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp111; _temp111.tag= Cyc_Absyn_Struct_d_tag;
_temp111.f1= sd; _temp111;}); _temp110;}), 0); _temp109->tl= Cyc_Toc_added_decls;
_temp109;});} static struct Cyc_List_List* Cyc_Toc_tuple_types= 0; static int
Cyc_Toc_tuple_type_counter= 0; static struct _tagged_string* Cyc_Toc_add_tuple_type(
struct Cyc_List_List* tqs0){{ struct Cyc_List_List* tts= Cyc_Toc_tuple_types;
for( 0; tts != 0; tts=({ struct Cyc_List_List* _temp122= tts; if( _temp122 == 0){
_throw( Null_Exception);} _temp122->tl;})){ struct _tuple4 _temp126; struct Cyc_List_List*
_temp127; struct _tagged_string* _temp129; struct _tuple4* _temp124=( struct
_tuple4*)({ struct Cyc_List_List* _temp123= tts; if( _temp123 == 0){ _throw(
Null_Exception);} _temp123->hd;}); _temp126=* _temp124; _LL130: _temp129=
_temp126.f1; goto _LL128; _LL128: _temp127= _temp126.f2; goto _LL125; _LL125: {
int okay= 1; struct Cyc_List_List* tqs= tqs0; for( 0; tqs != 0? _temp127 != 0: 0;
tqs=({ struct Cyc_List_List* _temp131= tqs; if( _temp131 == 0){ _throw(
Null_Exception);} _temp131->tl;}), _temp127=({ struct Cyc_List_List* _temp132=
_temp127; if( _temp132 == 0){ _throw( Null_Exception);} _temp132->tl;})){ if( !
Cyc_Tcutil_unify((*(( struct _tuple5*)({ struct Cyc_List_List* _temp133= tqs;
if( _temp133 == 0){ _throw( Null_Exception);} _temp133->hd;}))).f2,( void*)({
struct Cyc_List_List* _temp134= _temp127; if( _temp134 == 0){ _throw(
Null_Exception);} _temp134->hd;}))){ okay= 0; break;}} if(( ! okay? 1: tqs != 0)?
1: _temp127 != 0){ continue;} return _temp129;}}}{ struct _tagged_string* x=({
struct _tagged_string* _temp137=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp137[ 0]= xprintf("_tuple%d",( Cyc_Toc_tuple_type_counter
++)); _temp137;}); struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)( void*(*
f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct
_tuple5*)) Cyc_Core_snd, tqs0); Cyc_Toc_add_tuple_decl( x, ts); Cyc_Toc_tuple_types=({
struct Cyc_List_List* _temp135=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp135->hd=( void*)({ struct _tuple4* _temp136=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp136->f1= x; _temp136->f2=
ts; _temp136;}); _temp135->tl= Cyc_Toc_tuple_types; _temp135;}); return x;}}
static struct Cyc_List_List* Cyc_Toc_tagged_ptr_types= 0; static int Cyc_Toc_tagged_ptr_counter=
0; static struct _tagged_string* Cyc_Toc_add_tagged_ptr_type( void* t, struct
Cyc_Absyn_Tqual* tq){{ void* _temp138= t; void* _temp144; void* _temp146; _LL140:
if(( unsigned int) _temp138 > 4u?(( struct _tunion_struct*) _temp138)->tag ==
Cyc_Absyn_IntType_tag: 0){ _LL147: _temp146=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp138)->f1; if(( int) _temp146 == Cyc_Absyn_Unsigned){ goto _LL145;} else{
goto _LL142;} _LL145: _temp144=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp138)->f2; if(( int) _temp144 == Cyc_Absyn_B1){ goto _LL141;} else{ goto
_LL142;}} else{ goto _LL142;} _LL142: goto _LL143; _LL141: return Cyc_Toc__tagged_string_sp;
_LL143: goto _LL139; _LL139:;}{ struct Cyc_List_List* tts= Cyc_Toc_tagged_ptr_types;
for( 0; tts != 0; tts=({ struct Cyc_List_List* _temp148= tts; if( _temp148 == 0){
_throw( Null_Exception);} _temp148->tl;})){ struct _tuple6 _temp152; void*
_temp153; struct _tagged_string* _temp155; struct _tuple6* _temp150=( struct
_tuple6*)({ struct Cyc_List_List* _temp149= tts; if( _temp149 == 0){ _throw(
Null_Exception);} _temp149->hd;}); _temp152=* _temp150; _LL156: _temp155=
_temp152.f1; goto _LL154; _LL154: _temp153= _temp152.f2; goto _LL151; _LL151:
if( ! Cyc_Tcutil_unify( t, _temp153)){ continue;} return _temp155;}}{ struct
_tagged_string* x=({ struct _tagged_string* _temp159=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp159[ 0]= xprintf("_tagged_ptr%d",(
Cyc_Toc_tagged_ptr_counter ++)); _temp159;}); Cyc_Toc_add_tagged_ptr_decl( x, t);
Cyc_Toc_tagged_ptr_types=({ struct Cyc_List_List* _temp157=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp157->hd=( void*)({ struct
_tuple6* _temp158=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6));
_temp158->f1= x; _temp158->f2= t; _temp158;}); _temp157->tl= Cyc_Toc_tagged_ptr_types;
_temp157;}); return x;}} static int Cyc_Toc_temp_var_counter= 0; static struct
_tuple1* Cyc_Toc_temp_var(){ return({ struct _tuple1* _temp160=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp160->f1= Cyc_Toc_abs_ns; _temp160->f2=({
struct _tagged_string* _temp161=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp161[ 0]= xprintf("_temp%d",( Cyc_Toc_temp_var_counter
++)); _temp161;}); _temp160;});} static int Cyc_Toc_fresh_label_counter= 0;
static struct _tagged_string* Cyc_Toc_fresh_label(){ return({ struct
_tagged_string* _temp162=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp162[ 0]= xprintf("_LL%d",( Cyc_Toc_temp_var_counter
++)); _temp162;});} static struct Cyc_List_List* Cyc_Toc_temp_topdecls= 0;
static void* Cyc_Toc_typ_to_c( void* t); static struct _tuple2* Cyc_Toc_arg_to_c(
struct _tuple2* a){ void* _temp165; struct Cyc_Absyn_Tqual* _temp167; struct Cyc_Core_Opt*
_temp169; struct _tuple2 _temp163=* a; _LL170: _temp169= _temp163.f1; goto
_LL168; _LL168: _temp167= _temp163.f2; goto _LL166; _LL166: _temp165= _temp163.f3;
goto _LL164; _LL164: return({ struct _tuple2* _temp171=( struct _tuple2*)
GC_malloc( sizeof( struct _tuple2)); _temp171->f1= _temp169; _temp171->f2=
_temp167; _temp171->f3= Cyc_Toc_typ_to_c( _temp165); _temp171;});} static struct
_tuple5* Cyc_Toc_typ_to_c_f( struct _tuple5* x){ void* _temp174; struct Cyc_Absyn_Tqual*
_temp176; struct _tuple5 _temp172=* x; _LL177: _temp176= _temp172.f1; goto
_LL175; _LL175: _temp174= _temp172.f2; goto _LL173; _LL173: return({ struct
_tuple5* _temp178=( struct _tuple5*) GC_malloc( sizeof( struct _tuple5));
_temp178->f1= _temp176; _temp178->f2= Cyc_Toc_typ_to_c( _temp174); _temp178;});}
static void* Cyc_Toc_typ_to_c_array( void* t){ void* _temp179= t; struct Cyc_Absyn_Exp*
_temp187; struct Cyc_Absyn_Tqual* _temp189; void* _temp191; int _temp193; struct
Cyc_Core_Opt* _temp195; struct Cyc_Core_Opt _temp197; void* _temp198; void*
_temp200; _LL181: if(( unsigned int) _temp179 > 4u?(( struct _tunion_struct*)
_temp179)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL192: _temp191=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp179)->f1; goto _LL190; _LL190: _temp189=(
struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*) _temp179)->f2;
goto _LL188; _LL188: _temp187=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp179)->f3; goto _LL182;} else{ goto _LL183;} _LL183: if(( unsigned int)
_temp179 > 4u?(( struct _tunion_struct*) _temp179)->tag == Cyc_Absyn_Evar_tag: 0){
_LL201: _temp200=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp179)->f1; goto
_LL196; _LL196: _temp195=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp179)->f2; if( _temp195 == 0){ goto _LL185;} else{ _temp197=* _temp195;
_LL199: _temp198=( void*) _temp197.v; goto _LL194;} _LL194: _temp193=( int)((
struct Cyc_Absyn_Evar_struct*) _temp179)->f3; goto _LL184;} else{ goto _LL185;}
_LL185: goto _LL186; _LL182: return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp202=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp202[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp203; _temp203.tag= Cyc_Absyn_ArrayType_tag;
_temp203.f1=( void*) Cyc_Toc_typ_to_c_array( _temp191); _temp203.f2= _temp189;
_temp203.f3= _temp187; _temp203;}); _temp202;}); _LL184: return Cyc_Toc_typ_to_c_array(
_temp198); _LL186: return Cyc_Toc_typ_to_c( t); _LL180:;} static void* Cyc_Toc_typ_to_c(
void* t){ void* _temp204= t; int _temp252; struct Cyc_Core_Opt* _temp254; void*
_temp256; int _temp258; struct Cyc_Core_Opt* _temp260; struct Cyc_Core_Opt
_temp262; void* _temp263; void* _temp265; struct Cyc_Absyn_Tvar* _temp267;
struct Cyc_Absyn_TunionInfo _temp269; struct Cyc_Absyn_Tuniondecl* _temp271;
void* _temp273; struct Cyc_List_List* _temp275; struct _tuple1* _temp277; struct
Cyc_Absyn_XTunionInfo _temp279; struct Cyc_Absyn_XTuniondecl* _temp281; void*
_temp283; struct _tuple1* _temp285; struct Cyc_Absyn_TunionFieldInfo _temp287;
struct Cyc_Absyn_Tunionfield* _temp289; struct Cyc_Absyn_Tuniondecl* _temp291;
struct _tuple1* _temp293; struct Cyc_List_List* _temp295; struct _tuple1*
_temp297; struct Cyc_Absyn_XTunionFieldInfo _temp299; struct Cyc_Absyn_Tunionfield*
_temp301; struct Cyc_Absyn_XTuniondecl* _temp303; struct _tuple1* _temp305;
struct _tuple1* _temp307; struct Cyc_Absyn_PtrInfo _temp309; struct Cyc_Absyn_Conref*
_temp311; struct Cyc_Absyn_Tqual* _temp313; struct Cyc_Absyn_Conref* _temp315;
void* _temp317; void* _temp319; void* _temp321; void* _temp323; struct Cyc_Absyn_Exp*
_temp325; struct Cyc_Absyn_Tqual* _temp327; void* _temp329; struct Cyc_Absyn_FnInfo
_temp331; struct Cyc_List_List* _temp333; int _temp335; struct Cyc_List_List*
_temp337; void* _temp339; struct Cyc_Core_Opt* _temp341; struct Cyc_List_List*
_temp343; struct Cyc_List_List* _temp345; struct Cyc_Absyn_Uniondecl** _temp347;
struct Cyc_List_List* _temp349; struct _tuple1* _temp351; struct Cyc_Absyn_Structdecl**
_temp353; struct Cyc_List_List* _temp355; struct _tuple1* _temp357; struct Cyc_Absyn_Enumdecl*
_temp359; struct _tuple1* _temp361; struct Cyc_Core_Opt* _temp363; struct Cyc_List_List*
_temp365; struct _tuple1* _temp367; void* _temp369; void* _temp371; struct Cyc_List_List*
_temp373; _LL206: if(( int) _temp204 == Cyc_Absyn_VoidType){ goto _LL207;} else{
goto _LL208;} _LL208: if(( unsigned int) _temp204 > 4u?(( struct _tunion_struct*)
_temp204)->tag == Cyc_Absyn_Evar_tag: 0){ _LL257: _temp256=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp204)->f1; goto _LL255; _LL255: _temp254=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Evar_struct*) _temp204)->f2; if( _temp254 == 0){ goto _LL253;} else{
goto _LL210;} _LL253: _temp252=( int)(( struct Cyc_Absyn_Evar_struct*) _temp204)->f3;
goto _LL209;} else{ goto _LL210;} _LL210: if(( unsigned int) _temp204 > 4u?((
struct _tunion_struct*) _temp204)->tag == Cyc_Absyn_Evar_tag: 0){ _LL266:
_temp265=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp204)->f1; goto _LL261;
_LL261: _temp260=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp204)->f2; if( _temp260 == 0){ goto _LL212;} else{ _temp262=* _temp260;
_LL264: _temp263=( void*) _temp262.v; goto _LL259;} _LL259: _temp258=( int)((
struct Cyc_Absyn_Evar_struct*) _temp204)->f3; goto _LL211;} else{ goto _LL212;}
_LL212: if(( unsigned int) _temp204 > 4u?(( struct _tunion_struct*) _temp204)->tag
== Cyc_Absyn_VarType_tag: 0){ _LL268: _temp267=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_VarType_struct*) _temp204)->f1; goto _LL213;} else{ goto _LL214;}
_LL214: if(( unsigned int) _temp204 > 4u?(( struct _tunion_struct*) _temp204)->tag
== Cyc_Absyn_TunionType_tag: 0){ _LL270: _temp269=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp204)->f1; _LL278: _temp277=( struct
_tuple1*) _temp269.name; goto _LL276; _LL276: _temp275=( struct Cyc_List_List*)
_temp269.targs; goto _LL274; _LL274: _temp273=( void*) _temp269.rgn; goto _LL272;
_LL272: _temp271=( struct Cyc_Absyn_Tuniondecl*) _temp269.tud; goto _LL215;}
else{ goto _LL216;} _LL216: if(( unsigned int) _temp204 > 4u?(( struct
_tunion_struct*) _temp204)->tag == Cyc_Absyn_XTunionType_tag: 0){ _LL280:
_temp279=( struct Cyc_Absyn_XTunionInfo)(( struct Cyc_Absyn_XTunionType_struct*)
_temp204)->f1; _LL286: _temp285=( struct _tuple1*) _temp279.name; goto _LL284;
_LL284: _temp283=( void*) _temp279.rgn; goto _LL282; _LL282: _temp281=( struct
Cyc_Absyn_XTuniondecl*) _temp279.xtud; goto _LL217;} else{ goto _LL218;} _LL218:
if(( unsigned int) _temp204 > 4u?(( struct _tunion_struct*) _temp204)->tag ==
Cyc_Absyn_TunionFieldType_tag: 0){ _LL288: _temp287=( struct Cyc_Absyn_TunionFieldInfo)((
struct Cyc_Absyn_TunionFieldType_struct*) _temp204)->f1; _LL298: _temp297=(
struct _tuple1*) _temp287.name; goto _LL296; _LL296: _temp295=( struct Cyc_List_List*)
_temp287.targs; goto _LL294; _LL294: _temp293=( struct _tuple1*) _temp287.fname;
goto _LL292; _LL292: _temp291=( struct Cyc_Absyn_Tuniondecl*) _temp287.tud; goto
_LL290; _LL290: _temp289=( struct Cyc_Absyn_Tunionfield*) _temp287.tufd; goto
_LL219;} else{ goto _LL220;} _LL220: if(( unsigned int) _temp204 > 4u?(( struct
_tunion_struct*) _temp204)->tag == Cyc_Absyn_XTunionFieldType_tag: 0){ _LL300:
_temp299=( struct Cyc_Absyn_XTunionFieldInfo)(( struct Cyc_Absyn_XTunionFieldType_struct*)
_temp204)->f1; _LL308: _temp307=( struct _tuple1*) _temp299.name; goto _LL306;
_LL306: _temp305=( struct _tuple1*) _temp299.fname; goto _LL304; _LL304:
_temp303=( struct Cyc_Absyn_XTuniondecl*) _temp299.xtud; goto _LL302; _LL302:
_temp301=( struct Cyc_Absyn_Tunionfield*) _temp299.xtufd; goto _LL221;} else{
goto _LL222;} _LL222: if(( unsigned int) _temp204 > 4u?(( struct _tunion_struct*)
_temp204)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL310: _temp309=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp204)->f1; _LL320: _temp319=( void*)
_temp309.elt_typ; goto _LL318; _LL318: _temp317=( void*) _temp309.rgn_typ; goto
_LL316; _LL316: _temp315=( struct Cyc_Absyn_Conref*) _temp309.nullable; goto
_LL314; _LL314: _temp313=( struct Cyc_Absyn_Tqual*) _temp309.tq; goto _LL312;
_LL312: _temp311=( struct Cyc_Absyn_Conref*) _temp309.bounds; goto _LL223;}
else{ goto _LL224;} _LL224: if(( unsigned int) _temp204 > 4u?(( struct
_tunion_struct*) _temp204)->tag == Cyc_Absyn_IntType_tag: 0){ _LL324: _temp323=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp204)->f1; goto _LL322; _LL322:
_temp321=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp204)->f2; goto _LL225;}
else{ goto _LL226;} _LL226: if(( int) _temp204 == Cyc_Absyn_FloatType){ goto
_LL227;} else{ goto _LL228;} _LL228: if(( int) _temp204 == Cyc_Absyn_DoubleType){
goto _LL229;} else{ goto _LL230;} _LL230: if(( unsigned int) _temp204 > 4u?((
struct _tunion_struct*) _temp204)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL330:
_temp329=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp204)->f1; goto
_LL328; _LL328: _temp327=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp204)->f2; goto _LL326; _LL326: _temp325=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp204)->f3; goto _LL231;} else{ goto _LL232;}
_LL232: if(( unsigned int) _temp204 > 4u?(( struct _tunion_struct*) _temp204)->tag
== Cyc_Absyn_FnType_tag: 0){ _LL332: _temp331=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp204)->f1; _LL344: _temp343=( struct Cyc_List_List*)
_temp331.tvars; goto _LL342; _LL342: _temp341=( struct Cyc_Core_Opt*) _temp331.effect;
goto _LL340; _LL340: _temp339=( void*) _temp331.ret_typ; goto _LL338; _LL338:
_temp337=( struct Cyc_List_List*) _temp331.args; goto _LL336; _LL336: _temp335=(
int) _temp331.varargs; goto _LL334; _LL334: _temp333=( struct Cyc_List_List*)
_temp331.attributes; goto _LL233;} else{ goto _LL234;} _LL234: if(( unsigned int)
_temp204 > 4u?(( struct _tunion_struct*) _temp204)->tag == Cyc_Absyn_TupleType_tag:
0){ _LL346: _temp345=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp204)->f1; goto _LL235;} else{ goto _LL236;} _LL236: if(( unsigned int)
_temp204 > 4u?(( struct _tunion_struct*) _temp204)->tag == Cyc_Absyn_UnionType_tag:
0){ _LL352: _temp351=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp204)->f1; goto _LL350; _LL350: _temp349=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp204)->f2; goto _LL348; _LL348: _temp347=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp204)->f3;
goto _LL237;} else{ goto _LL238;} _LL238: if(( unsigned int) _temp204 > 4u?((
struct _tunion_struct*) _temp204)->tag == Cyc_Absyn_StructType_tag: 0){ _LL358:
_temp357=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp204)->f1;
goto _LL356; _LL356: _temp355=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp204)->f2; goto _LL354; _LL354: _temp353=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp204)->f3; goto _LL239;} else{ goto
_LL240;} _LL240: if(( unsigned int) _temp204 > 4u?(( struct _tunion_struct*)
_temp204)->tag == Cyc_Absyn_EnumType_tag: 0){ _LL362: _temp361=( struct _tuple1*)((
struct Cyc_Absyn_EnumType_struct*) _temp204)->f1; goto _LL360; _LL360: _temp359=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*) _temp204)->f2;
goto _LL241;} else{ goto _LL242;} _LL242: if(( unsigned int) _temp204 > 4u?((
struct _tunion_struct*) _temp204)->tag == Cyc_Absyn_TypedefType_tag: 0){ _LL368:
_temp367=( struct _tuple1*)(( struct Cyc_Absyn_TypedefType_struct*) _temp204)->f1;
goto _LL366; _LL366: _temp365=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp204)->f2; goto _LL364; _LL364: _temp363=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_TypedefType_struct*) _temp204)->f3; goto _LL243;} else{ goto _LL244;}
_LL244: if(( unsigned int) _temp204 > 4u?(( struct _tunion_struct*) _temp204)->tag
== Cyc_Absyn_RgnHandleType_tag: 0){ _LL370: _temp369=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp204)->f1; goto _LL245;} else{ goto _LL246;} _LL246: if(( int) _temp204 ==
Cyc_Absyn_HeapRgn){ goto _LL247;} else{ goto _LL248;} _LL248: if(( unsigned int)
_temp204 > 4u?(( struct _tunion_struct*) _temp204)->tag == Cyc_Absyn_AccessEff_tag:
0){ _LL372: _temp371=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp204)->f1;
goto _LL249;} else{ goto _LL250;} _LL250: if(( unsigned int) _temp204 > 4u?((
struct _tunion_struct*) _temp204)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL374:
_temp373=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp204)->f1;
goto _LL251;} else{ goto _LL205;} _LL207: return t; _LL209: return( void*) Cyc_Absyn_VoidType;
_LL211: return Cyc_Toc_typ_to_c( _temp263); _LL213: return Cyc_Absyn_void_star_typ();
_LL215: return( void*)({ struct Cyc_Absyn_TypedefType_struct* _temp375=( struct
Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp375[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp376; _temp376.tag= Cyc_Absyn_TypedefType_tag;
_temp376.f1=( struct _tuple1*) _temp277; _temp376.f2= 0; _temp376.f3=({ struct
Cyc_Core_Opt* _temp377=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp377->v=( void*) Cyc_Absyn_void_star_typ(); _temp377;}); _temp376;});
_temp375;}); _LL217: return( void*)({ struct Cyc_Absyn_TypedefType_struct*
_temp378=( struct Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp378[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp379; _temp379.tag= Cyc_Absyn_TypedefType_tag;
_temp379.f1= _temp285; _temp379.f2= 0; _temp379.f3=({ struct Cyc_Core_Opt*
_temp380=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp380->v=( void*) Cyc_Absyn_star_typ( Cyc_Absyn_strct( Cyc_Toc__xtunion_struct_sp),(
void*) Cyc_Absyn_HeapRgn, Cyc_Toc_mt_tq); _temp380;}); _temp379;}); _temp378;});
_LL219: if(({ struct Cyc_Absyn_Tunionfield* _temp381= _temp289; if( _temp381 ==
0){ _throw( Null_Exception);} _temp381->typs;}) == 0){ return Cyc_Absyn_uint_t;}
else{ return Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp293,( struct
_tagged_string)({ char* _temp382=( char*)"_struct"; struct _tagged_string
_temp383; _temp383.curr= _temp382; _temp383.base= _temp382; _temp383.last_plus_one=
_temp382 + 8; _temp383;})));} _LL221: return Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag(
_temp305,( struct _tagged_string)({ char* _temp384=( char*)"_struct"; struct
_tagged_string _temp385; _temp385.curr= _temp384; _temp385.base= _temp384;
_temp385.last_plus_one= _temp384 + 8; _temp385;}))); _LL223: _temp319= Cyc_Toc_typ_to_c_array(
_temp319);{ void* _temp386=( void*) _temp311->v; void* _temp392; _LL388: if((
unsigned int) _temp386 > 1u?(( struct _tunion_struct*) _temp386)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL393: _temp392=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp386)->f1;
if(( int) _temp392 == Cyc_Absyn_Unknown_b){ goto _LL389;} else{ goto _LL390;}}
else{ goto _LL390;} _LL390: goto _LL391; _LL389: { struct _tagged_string* n= Cyc_Toc_add_tagged_ptr_type(
_temp319, _temp313); return Cyc_Absyn_strct( n);} _LL391: return Cyc_Absyn_star_typ(
_temp319,( void*) Cyc_Absyn_HeapRgn, _temp313); _LL387:;} _LL225: return t;
_LL227: return t; _LL229: return t; _LL231: return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp394=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp394[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp395; _temp395.tag= Cyc_Absyn_ArrayType_tag;
_temp395.f1=( void*) Cyc_Toc_typ_to_c_array( _temp329); _temp395.f2= _temp327;
_temp395.f3= _temp325; _temp395;}); _temp394;}); _LL233: { struct Cyc_List_List*
new_atts= 0; for( 0; _temp333 != 0; _temp333=({ struct Cyc_List_List* _temp396=
_temp333; if( _temp396 == 0){ _throw( Null_Exception);} _temp396->tl;})){ void*
_temp398=( void*)({ struct Cyc_List_List* _temp397= _temp333; if( _temp397 == 0){
_throw( Null_Exception);} _temp397->hd;}); _LL400: if(( int) _temp398 == Cyc_Absyn_Cdecl_att){
goto _LL401;} else{ goto _LL402;} _LL402: if(( int) _temp398 == Cyc_Absyn_Noreturn_att){
goto _LL403;} else{ goto _LL404;} _LL404: if(( int) _temp398 == Cyc_Absyn_Const_att){
goto _LL405;} else{ goto _LL406;} _LL406: goto _LL407; _LL401: goto _LL403;
_LL403: goto _LL405; _LL405: continue; _LL407: new_atts=({ struct Cyc_List_List*
_temp408=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp408->hd=( void*)(( void*)({ struct Cyc_List_List* _temp409= _temp333; if(
_temp409 == 0){ _throw( Null_Exception);} _temp409->hd;})); _temp408->tl=
new_atts; _temp408;}); goto _LL399; _LL399:;} return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp410=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp410[ 0]=({ struct Cyc_Absyn_FnType_struct _temp411; _temp411.tag= Cyc_Absyn_FnType_tag;
_temp411.f1=({ struct Cyc_Absyn_FnInfo _temp412; _temp412.tvars= 0; _temp412.effect=
0; _temp412.ret_typ=( void*) Cyc_Toc_typ_to_c( _temp339); _temp412.args=((
struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct _tuple2*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Toc_arg_to_c, _temp337); _temp412.varargs= _temp335;
_temp412.attributes= new_atts; _temp412;}); _temp411;}); _temp410;});} _LL235:
_temp345=(( struct Cyc_List_List*(*)( struct _tuple5*(* f)( struct _tuple5*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_typ_to_c_f, _temp345);{ struct
_tagged_string* n= Cyc_Toc_add_tuple_type( _temp345); return Cyc_Absyn_strct( n);}
_LL237: if( _temp351 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ char* _temp413=( char*)"anonymous union"; struct
_tagged_string _temp414; _temp414.curr= _temp413; _temp414.base= _temp413;
_temp414.last_plus_one= _temp413 + 16; _temp414;}));} return Cyc_Absyn_unionq_typ((
struct _tuple1*)({ struct _tuple1* _temp415= _temp351; if( _temp415 == 0){
_throw( Null_Exception);} _temp415;})); _LL239: if( _temp357 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char*
_temp416=( char*)"anonymous struct"; struct _tagged_string _temp417; _temp417.curr=
_temp416; _temp417.base= _temp416; _temp417.last_plus_one= _temp416 + 17;
_temp417;}));} return Cyc_Absyn_strctq(( struct _tuple1*)({ struct _tuple1*
_temp418= _temp357; if( _temp418 == 0){ _throw( Null_Exception);} _temp418;}));
_LL241: return t; _LL243: if( _temp363 == 0){ return(( void*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp419=(
char*)"Toc::typ_to_c: unresolved TypedefType"; struct _tagged_string _temp420;
_temp420.curr= _temp419; _temp420.base= _temp419; _temp420.last_plus_one=
_temp419 + 38; _temp420;}));} return( void*)({ struct Cyc_Absyn_TypedefType_struct*
_temp421=( struct Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp421[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp422; _temp422.tag= Cyc_Absyn_TypedefType_tag;
_temp422.f1= _temp367; _temp422.f2= 0; _temp422.f3=({ struct Cyc_Core_Opt*
_temp423=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp423->v=( void*) Cyc_Toc_typ_to_c_array(( void*)({ struct Cyc_Core_Opt*
_temp424= _temp363; if( _temp424 == 0){ _throw( Null_Exception);} _temp424->v;}));
_temp423;}); _temp422;}); _temp421;}); _LL245: return Cyc_Absyn_cstar_typ( Cyc_Absyn_strct(
Cyc_Toc__region_handle_sp), Cyc_Toc_mt_tq); _LL247: return(( void*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp425=(
char*)"Toc::typ_to_c: type translation passed the heap region"; struct
_tagged_string _temp426; _temp426.curr= _temp425; _temp426.base= _temp425;
_temp426.last_plus_one= _temp425 + 55; _temp426;})); _LL249: return(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp427=( char*)"Toc::typ_to_c: type translation passed the access eff"; struct
_tagged_string _temp428; _temp428.curr= _temp427; _temp428.base= _temp427;
_temp428.last_plus_one= _temp427 + 54; _temp428;})); _LL251: return(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp429=( char*)"Toc::typ_to_c: type translation passed the join eff"; struct
_tagged_string _temp430; _temp430.curr= _temp429; _temp430.base= _temp429;
_temp430.last_plus_one= _temp429 + 52; _temp430;})); _LL205:;} static struct Cyc_Absyn_Exp*
Cyc_Toc_array_to_ptr( void* t, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
l){ void* _temp431= t; struct Cyc_Absyn_Exp* _temp437; struct Cyc_Absyn_Tqual*
_temp439; void* _temp441; _LL433: if(( unsigned int) _temp431 > 4u?(( struct
_tunion_struct*) _temp431)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL442: _temp441=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp431)->f1; goto _LL440; _LL440:
_temp439=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp431)->f2; goto _LL438; _LL438: _temp437=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp431)->f3; goto _LL434;} else{ goto _LL435;}
_LL435: goto _LL436; _LL434: return Cyc_Absyn_cast_exp( Cyc_Absyn_star_typ(
_temp441,( void*) Cyc_Absyn_HeapRgn, _temp439), e, l); _LL436: return e; _LL432:;}
static struct Cyc_Absyn_Exp* Cyc_Toc_array_to_ptr_cast( void* t, struct Cyc_Absyn_Exp*
e, struct Cyc_Position_Segment* l){ void* _temp443= t; struct Cyc_Absyn_Exp*
_temp449; struct Cyc_Absyn_Tqual* _temp451; void* _temp453; _LL445: if((
unsigned int) _temp443 > 4u?(( struct _tunion_struct*) _temp443)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL454: _temp453=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp443)->f1;
goto _LL452; _LL452: _temp451=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp443)->f2; goto _LL450; _LL450: _temp449=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp443)->f3; goto _LL446;} else{ goto _LL447;}
_LL447: goto _LL448; _LL446: return Cyc_Absyn_cast_exp( Cyc_Absyn_star_typ(
_temp453,( void*) Cyc_Absyn_HeapRgn, _temp451), e, l); _LL448: return Cyc_Absyn_cast_exp(
t, e, l); _LL444:;} static int Cyc_Toc_atomic_typ( void* t){ void* _temp455= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_Tvar* _temp491; void* _temp493; void* _temp495; struct Cyc_Absyn_Exp*
_temp497; struct Cyc_Absyn_Tqual* _temp499; void* _temp501; struct Cyc_Absyn_FnInfo
_temp503; struct Cyc_List_List* _temp505; struct Cyc_Absyn_Structdecl** _temp507;
struct Cyc_List_List* _temp509; struct _tuple1* _temp511; struct Cyc_Absyn_TunionInfo
_temp513; struct Cyc_Absyn_XTunionInfo _temp515; struct Cyc_Absyn_TunionFieldInfo
_temp517; struct Cyc_Absyn_Tunionfield* _temp519; struct Cyc_Absyn_Tuniondecl*
_temp521; struct _tuple1* _temp523; struct Cyc_List_List* _temp525; struct
_tuple1* _temp527; struct Cyc_Absyn_XTunionFieldInfo _temp529; struct Cyc_Absyn_Uniondecl**
_temp531; struct Cyc_List_List* _temp533; struct _tuple1* _temp535; struct Cyc_Absyn_PtrInfo
_temp537; void* _temp539; _LL457: if(( int) _temp455 == Cyc_Absyn_VoidType){
goto _LL458;} else{ goto _LL459;} _LL459: if(( unsigned int) _temp455 > 4u?((
struct _tunion_struct*) _temp455)->tag == Cyc_Absyn_VarType_tag: 0){ _LL492:
_temp491=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*) _temp455)->f1;
goto _LL460;} else{ goto _LL461;} _LL461: if(( unsigned int) _temp455 > 4u?((
struct _tunion_struct*) _temp455)->tag == Cyc_Absyn_IntType_tag: 0){ _LL496:
_temp495=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp455)->f1; goto _LL494;
_LL494: _temp493=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp455)->f2;
goto _LL462;} else{ goto _LL463;} _LL463: if(( int) _temp455 == Cyc_Absyn_FloatType){
goto _LL464;} else{ goto _LL465;} _LL465: if(( int) _temp455 == Cyc_Absyn_DoubleType){
goto _LL466;} else{ goto _LL467;} _LL467: if(( unsigned int) _temp455 > 4u?((
struct _tunion_struct*) _temp455)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL502:
_temp501=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp455)->f1; goto
_LL500; _LL500: _temp499=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp455)->f2; goto _LL498; _LL498: _temp497=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp455)->f3; goto _LL468;} else{ goto _LL469;}
_LL469: if(( unsigned int) _temp455 > 4u?(( struct _tunion_struct*) _temp455)->tag
== Cyc_Absyn_FnType_tag: 0){ _LL504: _temp503=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp455)->f1; goto _LL470;} else{ goto _LL471;}
_LL471: if(( unsigned int) _temp455 > 4u?(( struct _tunion_struct*) _temp455)->tag
== Cyc_Absyn_TupleType_tag: 0){ _LL506: _temp505=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp455)->f1; goto _LL472;} else{ goto
_LL473;} _LL473: if(( unsigned int) _temp455 > 4u?(( struct _tunion_struct*)
_temp455)->tag == Cyc_Absyn_StructType_tag: 0){ _LL512: _temp511=( struct
_tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp455)->f1; goto _LL510;
_LL510: _temp509=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp455)->f2; goto _LL508; _LL508: _temp507=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp455)->f3; goto _LL474;} else{ goto
_LL475;} _LL475: if(( unsigned int) _temp455 > 4u?(( struct _tunion_struct*)
_temp455)->tag == Cyc_Absyn_TunionType_tag: 0){ _LL514: _temp513=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp455)->f1; goto _LL476;} else{ goto
_LL477;} _LL477: if(( unsigned int) _temp455 > 4u?(( struct _tunion_struct*)
_temp455)->tag == Cyc_Absyn_XTunionType_tag: 0){ _LL516: _temp515=( struct Cyc_Absyn_XTunionInfo)((
struct Cyc_Absyn_XTunionType_struct*) _temp455)->f1; goto _LL478;} else{ goto
_LL479;} _LL479: if(( unsigned int) _temp455 > 4u?(( struct _tunion_struct*)
_temp455)->tag == Cyc_Absyn_TunionFieldType_tag: 0){ _LL518: _temp517=( struct
Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*) _temp455)->f1;
_LL528: _temp527=( struct _tuple1*) _temp517.name; goto _LL526; _LL526: _temp525=(
struct Cyc_List_List*) _temp517.targs; goto _LL524; _LL524: _temp523=( struct
_tuple1*) _temp517.fname; goto _LL522; _LL522: _temp521=( struct Cyc_Absyn_Tuniondecl*)
_temp517.tud; goto _LL520; _LL520: _temp519=( struct Cyc_Absyn_Tunionfield*)
_temp517.tufd; goto _LL480;} else{ goto _LL481;} _LL481: if(( unsigned int)
_temp455 > 4u?(( struct _tunion_struct*) _temp455)->tag == Cyc_Absyn_XTunionFieldType_tag:
0){ _LL530: _temp529=( struct Cyc_Absyn_XTunionFieldInfo)(( struct Cyc_Absyn_XTunionFieldType_struct*)
_temp455)->f1; goto _LL482;} else{ goto _LL483;} _LL483: if(( unsigned int)
_temp455 > 4u?(( struct _tunion_struct*) _temp455)->tag == Cyc_Absyn_UnionType_tag:
0){ _LL536: _temp535=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp455)->f1; goto _LL534; _LL534: _temp533=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp455)->f2; goto _LL532; _LL532: _temp531=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp455)->f3;
goto _LL484;} else{ goto _LL485;} _LL485: if(( unsigned int) _temp455 > 4u?((
struct _tunion_struct*) _temp455)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL538:
_temp537=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp455)->f1; goto _LL486;} else{ goto _LL487;} _LL487: if(( unsigned int)
_temp455 > 4u?(( struct _tunion_struct*) _temp455)->tag == Cyc_Absyn_RgnHandleType_tag:
0){ _LL540: _temp539=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp455)->f1;
goto _LL488;} else{ goto _LL489;} _LL489: goto _LL490; _LL458: return 1; _LL460:
return 0; _LL462: return 1; _LL464: return 1; _LL466: return 0; _LL468: return
Cyc_Toc_atomic_typ( _temp501); _LL470: return 1; _LL472: for( 0; _temp505 != 0;
_temp505=({ struct Cyc_List_List* _temp541= _temp505; if( _temp541 == 0){ _throw(
Null_Exception);} _temp541->tl;})){ if( ! Cyc_Toc_atomic_typ((*(( struct _tuple5*)({
struct Cyc_List_List* _temp542= _temp505; if( _temp542 == 0){ _throw(
Null_Exception);} _temp542->hd;}))).f2)){ return 0;}} return 1; _LL474: return 0;
_LL476: return 0; _LL478: return 0; _LL480: return({ struct Cyc_Absyn_Tunionfield*
_temp543= _temp519; if( _temp543 == 0){ _throw( Null_Exception);} _temp543->typs;})
== 0; _LL482: return 0; _LL484: return 0; _LL486: return 0; _LL488: return 0;
_LL490:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct
_tagged_string _temp544= Cyc_Absynpp_typ2string( t); xprintf("atomic_typ:  bad type %.*s",
_temp544.last_plus_one - _temp544.curr, _temp544.curr);})); return 0; _LL456:;}
static int Cyc_Toc_is_void_star( void* t){ void* _temp545= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_PtrInfo _temp551; struct Cyc_Absyn_Conref* _temp553; struct
Cyc_Absyn_Tqual* _temp555; struct Cyc_Absyn_Conref* _temp557; void* _temp559;
void* _temp561; _LL547: if(( unsigned int) _temp545 > 4u?(( struct
_tunion_struct*) _temp545)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL552:
_temp551=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp545)->f1; _LL562: _temp561=( void*) _temp551.elt_typ; goto _LL560; _LL560:
_temp559=( void*) _temp551.rgn_typ; goto _LL558; _LL558: _temp557=( struct Cyc_Absyn_Conref*)
_temp551.nullable; goto _LL556; _LL556: _temp555=( struct Cyc_Absyn_Tqual*)
_temp551.tq; goto _LL554; _LL554: _temp553=( struct Cyc_Absyn_Conref*) _temp551.bounds;
goto _LL548;} else{ goto _LL549;} _LL549: goto _LL550; _LL548: { void* _temp563=
Cyc_Tcutil_compress( _temp561); _LL565: if(( int) _temp563 == Cyc_Absyn_VoidType){
goto _LL566;} else{ goto _LL567;} _LL567: goto _LL568; _LL566: return 1; _LL568:
return 0; _LL564:;} _LL550: return 0; _LL546:;} static int Cyc_Toc_is_poly_field(
void* t, struct _tagged_string* f){ void* _temp569= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Structdecl** _temp581; struct Cyc_Absyn_Structdecl* _temp583;
struct Cyc_List_List* _temp584; struct _tuple1* _temp586; struct Cyc_Absyn_Structdecl**
_temp588; struct Cyc_List_List* _temp590; struct _tuple1* _temp592; struct Cyc_Absyn_Uniondecl**
_temp594; struct Cyc_Absyn_Uniondecl* _temp596; struct Cyc_List_List* _temp597;
struct _tuple1* _temp599; struct Cyc_Absyn_Uniondecl** _temp601; struct Cyc_List_List*
_temp603; struct _tuple1* _temp605; _LL571: if(( unsigned int) _temp569 > 4u?((
struct _tunion_struct*) _temp569)->tag == Cyc_Absyn_StructType_tag: 0){ _LL587:
_temp586=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp569)->f1;
goto _LL585; _LL585: _temp584=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp569)->f2; goto _LL582; _LL582: _temp581=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp569)->f3; if( _temp581 == 0){ goto
_LL573;} else{ _temp583=* _temp581; goto _LL572;}} else{ goto _LL573;} _LL573:
if(( unsigned int) _temp569 > 4u?(( struct _tunion_struct*) _temp569)->tag ==
Cyc_Absyn_StructType_tag: 0){ _LL593: _temp592=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp569)->f1; goto _LL591; _LL591: _temp590=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp569)->f2; goto _LL589; _LL589: _temp588=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp569)->f3;
if( _temp588 == 0){ goto _LL574;} else{ goto _LL575;}} else{ goto _LL575;}
_LL575: if(( unsigned int) _temp569 > 4u?(( struct _tunion_struct*) _temp569)->tag
== Cyc_Absyn_UnionType_tag: 0){ _LL600: _temp599=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp569)->f1; goto _LL598; _LL598: _temp597=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp569)->f2; goto _LL595; _LL595: _temp594=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp569)->f3;
if( _temp594 == 0){ goto _LL577;} else{ _temp596=* _temp594; goto _LL576;}}
else{ goto _LL577;} _LL577: if(( unsigned int) _temp569 > 4u?(( struct
_tunion_struct*) _temp569)->tag == Cyc_Absyn_UnionType_tag: 0){ _LL606: _temp605=(
struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*) _temp569)->f1; goto
_LL604; _LL604: _temp603=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp569)->f2; goto _LL602; _LL602: _temp601=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp569)->f3; if( _temp601 == 0){ goto
_LL578;} else{ goto _LL579;}} else{ goto _LL579;} _LL579: goto _LL580; _LL572:
if( _temp586 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp607=( char*)"StructType name not properly set";
struct _tagged_string _temp608; _temp608.curr= _temp607; _temp608.base= _temp607;
_temp608.last_plus_one= _temp607 + 33; _temp608;}));} if( _temp583->fields == 0){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string
_temp610= Cyc_Absynpp_qvar2string(( struct _tuple1*)({ struct _tuple1* _temp609=
_temp586; if( _temp609 == 0){ _throw( Null_Exception);} _temp609;})); xprintf("is_poly_field:  struct %.*s missing fields",
_temp610.last_plus_one - _temp610.curr, _temp610.curr);}));}{ struct Cyc_List_List*
fields=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp617= _temp583->fields;
if( _temp617 == 0){ _throw( Null_Exception);} _temp617->v;}); for( 0; fields !=
0; fields=({ struct Cyc_List_List* _temp611= fields; if( _temp611 == 0){ _throw(
Null_Exception);} _temp611->tl;})){ if( Cyc_String_zstrptrcmp( f,(( struct Cyc_Absyn_Structfield*)({
struct Cyc_List_List* _temp612= fields; if( _temp612 == 0){ _throw(
Null_Exception);} _temp612->hd;}))->name) == 0){ return Cyc_Toc_is_void_star((
void*)(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp613= fields;
if( _temp613 == 0){ _throw( Null_Exception);} _temp613->hd;}))->type);}}(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp615=
Cyc_Absynpp_qvar2string(( struct _tuple1*)({ struct _tuple1* _temp614= _temp586;
if( _temp614 == 0){ _throw( Null_Exception);} _temp614;})); struct
_tagged_string _temp616=* f; xprintf("is_poly_field:  struct %.*s has no %.*s field",
_temp615.last_plus_one - _temp615.curr, _temp615.curr, _temp616.last_plus_one -
_temp616.curr, _temp616.curr);})); return 0;} _LL574: if( _temp592 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp618=( char*)"StructType name not properly set"; struct _tagged_string
_temp619; _temp619.curr= _temp618; _temp619.base= _temp618; _temp619.last_plus_one=
_temp618 + 33; _temp619;}));}(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp621= Cyc_Absynpp_qvar2string(( struct _tuple1*)({
struct _tuple1* _temp620= _temp592; if( _temp620 == 0){ _throw( Null_Exception);}
_temp620;})); xprintf("is_poly_field:  struct %.*s missing structdecl", _temp621.last_plus_one
- _temp621.curr, _temp621.curr);})); return 0; _LL576: if( _temp599 == 0){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
char* _temp622=( char*)"UnionType name not properly set"; struct _tagged_string
_temp623; _temp623.curr= _temp622; _temp623.base= _temp622; _temp623.last_plus_one=
_temp622 + 32; _temp623;}));} if( _temp596->fields == 0){(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp625= Cyc_Absynpp_qvar2string((
struct _tuple1*)({ struct _tuple1* _temp624= _temp599; if( _temp624 == 0){
_throw( Null_Exception);} _temp624;})); xprintf("is_poly_field:  union %.*s missing fields",
_temp625.last_plus_one - _temp625.curr, _temp625.curr);}));}{ struct Cyc_List_List*
fields=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp632= _temp596->fields;
if( _temp632 == 0){ _throw( Null_Exception);} _temp632->v;}); for( 0; fields !=
0; fields=({ struct Cyc_List_List* _temp626= fields; if( _temp626 == 0){ _throw(
Null_Exception);} _temp626->tl;})){ if( Cyc_String_zstrptrcmp( f,(( struct Cyc_Absyn_Structfield*)({
struct Cyc_List_List* _temp627= fields; if( _temp627 == 0){ _throw(
Null_Exception);} _temp627->hd;}))->name) == 0){ return Cyc_Toc_is_void_star((
void*)(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp628= fields;
if( _temp628 == 0){ _throw( Null_Exception);} _temp628->hd;}))->type);}}(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp630=
Cyc_Absynpp_qvar2string(( struct _tuple1*)({ struct _tuple1* _temp629= _temp599;
if( _temp629 == 0){ _throw( Null_Exception);} _temp629;})); struct
_tagged_string _temp631=* f; xprintf("is_poly_field:  union %.*s has no %.*s field",
_temp630.last_plus_one - _temp630.curr, _temp630.curr, _temp631.last_plus_one -
_temp631.curr, _temp631.curr);})); return 0;} _LL578: if( _temp605 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp633=( char*)"UnionType name not properly set"; struct _tagged_string
_temp634; _temp634.curr= _temp633; _temp634.base= _temp633; _temp634.last_plus_one=
_temp633 + 32; _temp634;}));}(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp636= Cyc_Absynpp_qvar2string(( struct _tuple1*)({
struct _tuple1* _temp635= _temp605; if( _temp635 == 0){ _throw( Null_Exception);}
_temp635;})); xprintf("is_poly_field:  union %.*s missing structdecl", _temp636.last_plus_one
- _temp636.curr, _temp636.curr);})); return 0; _LL580:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp637= Cyc_Absynpp_typ2string(
t); xprintf("is_poly_field:  bad type %.*s", _temp637.last_plus_one - _temp637.curr,
_temp637.curr);})); return 0; _LL570:;} static int Cyc_Toc_is_poly_project(
struct Cyc_Absyn_Exp* e){ void* _temp638=( void*) e->r; struct _tagged_string*
_temp646; struct Cyc_Absyn_Exp* _temp648; struct _tagged_string* _temp650;
struct Cyc_Absyn_Exp* _temp652; _LL640: if((( struct _tunion_struct*) _temp638)->tag
== Cyc_Absyn_StructMember_e_tag){ _LL649: _temp648=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp638)->f1; goto _LL647; _LL647:
_temp646=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp638)->f2; goto _LL641;} else{ goto _LL642;} _LL642: if((( struct
_tunion_struct*) _temp638)->tag == Cyc_Absyn_StructArrow_e_tag){ _LL653:
_temp652=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp638)->f1; goto _LL651; _LL651: _temp650=( struct _tagged_string*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp638)->f2; goto _LL643;} else{ goto _LL644;}
_LL644: goto _LL645; _LL641: return Cyc_Toc_is_poly_field(( void*)({ struct Cyc_Core_Opt*
_temp654= _temp648->topt; if( _temp654 == 0){ _throw( Null_Exception);} _temp654->v;}),
_temp646); _LL643: { void* _temp656= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp655= _temp652->topt; if( _temp655 == 0){ _throw( Null_Exception);} _temp655->v;}));
struct Cyc_Absyn_PtrInfo _temp662; struct Cyc_Absyn_Conref* _temp664; struct Cyc_Absyn_Tqual*
_temp666; struct Cyc_Absyn_Conref* _temp668; void* _temp670; void* _temp672;
_LL658: if(( unsigned int) _temp656 > 4u?(( struct _tunion_struct*) _temp656)->tag
== Cyc_Absyn_PointerType_tag: 0){ _LL663: _temp662=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp656)->f1; _LL673: _temp672=( void*)
_temp662.elt_typ; goto _LL671; _LL671: _temp670=( void*) _temp662.rgn_typ; goto
_LL669; _LL669: _temp668=( struct Cyc_Absyn_Conref*) _temp662.nullable; goto
_LL667; _LL667: _temp666=( struct Cyc_Absyn_Tqual*) _temp662.tq; goto _LL665;
_LL665: _temp664=( struct Cyc_Absyn_Conref*) _temp662.bounds; goto _LL659;}
else{ goto _LL660;} _LL660: goto _LL661; _LL659: return Cyc_Toc_is_poly_field(
_temp672, _temp650); _LL661:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp675= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp674= _temp652->topt; if( _temp674 == 0){ _throw( Null_Exception);} _temp674->v;}));
xprintf("is_poly_project:  bad type %.*s", _temp675.last_plus_one - _temp675.curr,
_temp675.curr);})); return 0; _LL657:;} _LL645: return 0; _LL639:;} static char
_temp678[ 10u]="GC_malloc"; static struct _tagged_string Cyc_Toc_gc_malloc_string=(
struct _tagged_string){ _temp678, _temp678, _temp678 + 10u}; static struct
_tuple1 Cyc_Toc_gc_malloc_var=( struct _tuple1){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=&
Cyc_Toc_gc_malloc_string}; static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_ptr(
struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(& Cyc_Toc_gc_malloc_var,
0),({ struct Cyc_List_List* _temp679=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp679->hd=( void*) s; _temp679->tl= 0; _temp679;}), 0);}
static char _temp682[ 17u]="GC_malloc_atomic"; static struct _tagged_string Cyc_Toc_gc_malloc_atomic_string=(
struct _tagged_string){ _temp682, _temp682, _temp682 + 17u}; static struct
_tuple1 Cyc_Toc_gc_malloc_atomic_var=( struct _tuple1){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=&
Cyc_Toc_gc_malloc_atomic_string}; static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_atomic(
struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(& Cyc_Toc_gc_malloc_atomic_var,
0),({ struct Cyc_List_List* _temp683=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp683->hd=( void*) s; _temp683->tl= 0; _temp683;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_exp( void* t, struct Cyc_Absyn_Exp*
s){ if( Cyc_Toc_atomic_typ( t)){ return Cyc_Toc_malloc_atomic( s);} return Cyc_Toc_malloc_ptr(
s);} static char _temp686[ 15u]="_region_malloc"; static struct _tagged_string
Cyc_Toc__region_malloc_string=( struct _tagged_string){ _temp686, _temp686,
_temp686 + 15u}; static struct _tuple1 Cyc_Toc_region_malloc_var=( struct
_tuple1){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=& Cyc_Toc__region_malloc_string};
static struct Cyc_Absyn_Exp* Cyc_Toc_rmalloc_exp( struct Cyc_Absyn_Exp* rgn,
struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(& Cyc_Toc_region_malloc_var,
0),({ struct Cyc_List_List* _temp687=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp687->hd=( void*) rgn; _temp687->tl=({ struct Cyc_List_List*
_temp688=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp688->hd=( void*) s; _temp688->tl= 0; _temp688;}); _temp687;}), 0);} typedef
void* Cyc_Toc_Conv; static const int Cyc_Toc_NullCheck_tag= 0; struct Cyc_Toc_NullCheck_struct{
int tag; void* f1; } ; static const int Cyc_Toc_UntagPtr_tag= 1; struct Cyc_Toc_UntagPtr_struct{
int tag; void* f1; struct Cyc_Absyn_Exp* f2; int f3; } ; static const int Cyc_Toc_TagPtr_tag=
2; struct Cyc_Toc_TagPtr_struct{ int tag; void* f1; struct Cyc_Absyn_Exp* f2; }
; typedef void* Cyc_Toc_conv; static struct Cyc_List_List* Cyc_Toc_conversion(
void* t1, void* t2){ t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{
struct _tuple0 _temp690=({ struct _tuple0 _temp689; _temp689.f1= t1; _temp689.f2=
t2; _temp689;}); void* _temp696; struct Cyc_Absyn_PtrInfo _temp698; void*
_temp700; struct Cyc_Absyn_PtrInfo _temp702; _LL692: _LL701: _temp700= _temp690.f1;
if(( unsigned int) _temp700 > 4u?(( struct _tunion_struct*) _temp700)->tag ==
Cyc_Absyn_PointerType_tag: 0){ _LL703: _temp702=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp700)->f1; goto _LL697;} else{ goto
_LL694;} _LL697: _temp696= _temp690.f2; if(( unsigned int) _temp696 > 4u?((
struct _tunion_struct*) _temp696)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL699:
_temp698=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp696)->f1; goto _LL693;} else{ goto _LL694;} _LL694: goto _LL695; _LL693: {
int n1=(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp702.nullable);
int n2=(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp698.nullable);
void* b1=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp702.bounds); void* b2=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp698.bounds); struct Cyc_List_List* convs= 0;{ struct _tuple0 _temp705=({
struct _tuple0 _temp704; _temp704.f1= b1; _temp704.f2= b2; _temp704;}); void*
_temp715; struct Cyc_Absyn_Exp* _temp717; void* _temp719; struct Cyc_Absyn_Exp*
_temp721; void* _temp723; void* _temp725; void* _temp727; void* _temp729; struct
Cyc_Absyn_Exp* _temp731; void* _temp733; struct Cyc_Absyn_Exp* _temp735; void*
_temp737; _LL707: _LL720: _temp719= _temp705.f1; if(( unsigned int) _temp719 > 1u?((
struct _tunion_struct*) _temp719)->tag == Cyc_Absyn_Upper_b_tag: 0){ _LL722:
_temp721=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp719)->f1;
goto _LL716;} else{ goto _LL709;} _LL716: _temp715= _temp705.f2; if((
unsigned int) _temp715 > 1u?(( struct _tunion_struct*) _temp715)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL718: _temp717=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp715)->f1; goto _LL708;} else{ goto _LL709;} _LL709: _LL726: _temp725=
_temp705.f1; if(( int) _temp725 == Cyc_Absyn_Unknown_b){ goto _LL724;} else{
goto _LL711;} _LL724: _temp723= _temp705.f2; if(( int) _temp723 == Cyc_Absyn_Unknown_b){
goto _LL710;} else{ goto _LL711;} _LL711: _LL730: _temp729= _temp705.f1; if((
unsigned int) _temp729 > 1u?(( struct _tunion_struct*) _temp729)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL732: _temp731=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp729)->f1; goto _LL728;} else{ goto _LL713;} _LL728: _temp727= _temp705.f2;
if(( int) _temp727 == Cyc_Absyn_Unknown_b){ goto _LL712;} else{ goto _LL713;}
_LL713: _LL738: _temp737= _temp705.f1; if(( int) _temp737 == Cyc_Absyn_Unknown_b){
goto _LL734;} else{ goto _LL706;} _LL734: _temp733= _temp705.f2; if((
unsigned int) _temp733 > 1u?(( struct _tunion_struct*) _temp733)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL736: _temp735=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp733)->f1; goto _LL714;} else{ goto _LL706;} _LL708: if( n1? ! n2: 0){ convs=({
struct Cyc_List_List* _temp739=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp739->hd=( void*)(( void*)({ struct Cyc_Toc_NullCheck_struct*
_temp740=( struct Cyc_Toc_NullCheck_struct*) GC_malloc( sizeof( struct Cyc_Toc_NullCheck_struct));
_temp740[ 0]=({ struct Cyc_Toc_NullCheck_struct _temp741; _temp741.tag= Cyc_Toc_NullCheck_tag;
_temp741.f1=( void*) t2; _temp741;}); _temp740;})); _temp739->tl= convs;
_temp739;});} goto _LL706; _LL710: goto _LL706; _LL712: convs=({ struct Cyc_List_List*
_temp742=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp742->hd=( void*)(( void*)({ struct Cyc_Toc_TagPtr_struct* _temp743=( struct
Cyc_Toc_TagPtr_struct*) GC_malloc( sizeof( struct Cyc_Toc_TagPtr_struct));
_temp743[ 0]=({ struct Cyc_Toc_TagPtr_struct _temp744; _temp744.tag= Cyc_Toc_TagPtr_tag;
_temp744.f1=( void*) Cyc_Toc_typ_to_c( t1); _temp744.f2= _temp731; _temp744;});
_temp743;})); _temp742->tl= convs; _temp742;}); goto _LL706; _LL714: convs=({
struct Cyc_List_List* _temp745=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp745->hd=( void*)(( void*)({ struct Cyc_Toc_UntagPtr_struct*
_temp746=( struct Cyc_Toc_UntagPtr_struct*) GC_malloc( sizeof( struct Cyc_Toc_UntagPtr_struct));
_temp746[ 0]=({ struct Cyc_Toc_UntagPtr_struct _temp747; _temp747.tag= Cyc_Toc_UntagPtr_tag;
_temp747.f1=( void*) Cyc_Toc_typ_to_c( t1); _temp747.f2= _temp735; _temp747.f3=
n2; _temp747;}); _temp746;})); _temp745->tl= convs; _temp745;}); goto _LL706;
_LL706:;} return convs;} _LL695: return 0; _LL691:;}} static char _temp750[ 7u]="_throw";
static struct _tagged_string Cyc_Toc__throw_string=( struct _tagged_string){
_temp750, _temp750, _temp750 + 7u}; static struct _tuple1 Cyc_Toc__throw_var=(
struct _tuple1){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=& Cyc_Toc__throw_string};
static struct Cyc_Absyn_Exp* Cyc_Toc_newthrow_exp( struct Cyc_Absyn_Exp* e){
return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(& Cyc_Toc__throw_var, 0),({
struct Cyc_List_List* _temp751=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp751->hd=( void*) e; _temp751->tl= 0; _temp751;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_null_check_conv( int toplevel, void* t,
struct Cyc_Absyn_Exp* e){ void* new_typ= Cyc_Toc_typ_to_c( t); if( toplevel){((
void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({
char* _temp752=( char*)"can't do null-check conversion at top-level"; struct
_tagged_string _temp753; _temp753.curr= _temp752; _temp753.base= _temp752;
_temp753.last_plus_one= _temp752 + 44; _temp753;}));}{ struct _tuple1* x= Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt* s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp(
Cyc_Absyn_null_pointer_exn_exp( 0)), 0), Cyc_Absyn_skip_stmt( 0), 0); s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( x, 0), 0), 0); return Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( x, new_typ,( struct Cyc_Absyn_Exp*) e, s, 0), 0);}}
static struct Cyc_Absyn_Exp* Cyc_Toc_tag_ptr_conv( int toplevel, void* t, struct
Cyc_Absyn_Exp* sz, struct Cyc_Absyn_Exp* e){ struct _tuple1* a= Cyc_Toc_temp_var();
struct _tuple1* b= Cyc_Toc_temp_var(); int szvalue=( int) Cyc_Evexp_eval_const_uint_exp(
sz); void* strt_typ; void* elt_typ;{ void* _temp754= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_PtrInfo _temp760; struct Cyc_Absyn_Conref* _temp762; struct Cyc_Absyn_Tqual*
_temp764; struct Cyc_Absyn_Conref* _temp766; void* _temp768; void* _temp770;
_LL756: if(( unsigned int) _temp754 > 4u?(( struct _tunion_struct*) _temp754)->tag
== Cyc_Absyn_PointerType_tag: 0){ _LL761: _temp760=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp754)->f1; _LL771: _temp770=( void*)
_temp760.elt_typ; goto _LL769; _LL769: _temp768=( void*) _temp760.rgn_typ; goto
_LL767; _LL767: _temp766=( struct Cyc_Absyn_Conref*) _temp760.nullable; goto
_LL765; _LL765: _temp764=( struct Cyc_Absyn_Tqual*) _temp760.tq; goto _LL763;
_LL763: _temp762=( struct Cyc_Absyn_Conref*) _temp760.bounds; goto _LL757;}
else{ goto _LL758;} _LL758: goto _LL759; _LL757: elt_typ= _temp770;{ struct
_tagged_string* n= Cyc_Toc_add_tagged_ptr_type( _temp770, Cyc_Toc_mt_tq);
strt_typ= Cyc_Absyn_strct( n); goto _LL755;} _LL759: return(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp772=( char*)"tag_ptr_conv: not a pointer"; struct _tagged_string _temp773;
_temp773.curr= _temp772; _temp773.base= _temp772; _temp773.last_plus_one=
_temp772 + 28; _temp773;})); _LL755:;} if( toplevel){ int is_string= 0;{ void*
_temp774=( void*) e->r; void* _temp780; struct _tagged_string _temp782; _LL776:
if((( struct _tunion_struct*) _temp774)->tag == Cyc_Absyn_Const_e_tag){ _LL781:
_temp780=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp774)->f1; if((
unsigned int) _temp780 > 1u?(( struct _tunion_struct*) _temp780)->tag == Cyc_Absyn_String_c_tag:
0){ _LL783: _temp782=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp780)->f1; goto _LL777;} else{ goto _LL778;}} else{ goto _LL778;} _LL778:
goto _LL779; _LL777: is_string= 1; goto _LL775; _LL779: goto _LL775; _LL775:;}{
struct Cyc_Absyn_Exp* xexp; if( is_string){ struct _tuple1* x= Cyc_Toc_temp_var();
void* vd_typ=( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp787=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp787[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp788; _temp788.tag= Cyc_Absyn_ArrayType_tag;
_temp788.f1=( void*) elt_typ; _temp788.f2= Cyc_Toc_mt_tq; _temp788.f3=( struct
Cyc_Absyn_Exp*) sz; _temp788;}); _temp787;}); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl(
x, vd_typ,( struct Cyc_Absyn_Exp*) e); Cyc_Toc_temp_topdecls=({ struct Cyc_List_List*
_temp784=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp784->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp785=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp785[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp786; _temp786.tag= Cyc_Absyn_Var_d_tag;
_temp786.f1= vd; _temp786;}); _temp785;}), 0); _temp784->tl= Cyc_Toc_temp_topdecls;
_temp784;}); xexp= Cyc_Absyn_var_exp( x, 0);} else{ xexp= e;}{ struct Cyc_Absyn_Exp*
xplussz= Cyc_Absyn_add_exp( xexp, sz, 0); struct Cyc_Absyn_Exp* urm_exp= Cyc_Absyn_unresolvedmem_exp(
0,({ struct Cyc_List_List* _temp789=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp789->hd=( void*)({ struct _tuple7* _temp794=(
struct _tuple7*) GC_malloc( sizeof( struct _tuple7)); _temp794->f1= 0; _temp794->f2=
xexp; _temp794;}); _temp789->tl=({ struct Cyc_List_List* _temp790=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp790->hd=( void*)({ struct
_tuple7* _temp793=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7));
_temp793->f1= 0; _temp793->f2= xexp; _temp793;}); _temp790->tl=({ struct Cyc_List_List*
_temp791=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp791->hd=( void*)({ struct _tuple7* _temp792=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp792->f1= 0; _temp792->f2= xplussz; _temp792;});
_temp791->tl= 0; _temp791;}); _temp790;}); _temp789;}), 0); return urm_exp;}}}
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
_tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char* _temp795=(
char*)"can't coerce t? to t* or t@ at the top-level"; struct _tagged_string
_temp796; _temp796.curr= _temp795; _temp796.base= _temp795; _temp796.last_plus_one=
_temp795 + 45; _temp796;}));}{ struct _tuple1* x= Cyc_Toc_temp_var(); int
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
_temp797= cs; if( _temp797 == 0){ _throw( Null_Exception);} _temp797->tl;})){
void* _temp799=( void*)({ struct Cyc_List_List* _temp798= cs; if( _temp798 == 0){
_throw( Null_Exception);} _temp798->hd;}); void* _temp807; struct Cyc_Absyn_Exp*
_temp809; void* _temp811; int _temp813; struct Cyc_Absyn_Exp* _temp815; void*
_temp817; _LL801: if((( struct _tunion_struct*) _temp799)->tag == Cyc_Toc_NullCheck_tag){
_LL808: _temp807=( void*)(( struct Cyc_Toc_NullCheck_struct*) _temp799)->f1;
goto _LL802;} else{ goto _LL803;} _LL803: if((( struct _tunion_struct*) _temp799)->tag
== Cyc_Toc_TagPtr_tag){ _LL812: _temp811=( void*)(( struct Cyc_Toc_TagPtr_struct*)
_temp799)->f1; goto _LL810; _LL810: _temp809=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Toc_TagPtr_struct*) _temp799)->f2; goto _LL804;} else{ goto _LL805;} _LL805:
if((( struct _tunion_struct*) _temp799)->tag == Cyc_Toc_UntagPtr_tag){ _LL818:
_temp817=( void*)(( struct Cyc_Toc_UntagPtr_struct*) _temp799)->f1; goto _LL816;
_LL816: _temp815=( struct Cyc_Absyn_Exp*)(( struct Cyc_Toc_UntagPtr_struct*)
_temp799)->f2; goto _LL814; _LL814: _temp813=( int)(( struct Cyc_Toc_UntagPtr_struct*)
_temp799)->f3; goto _LL806;} else{ goto _LL800;} _LL802: e= Cyc_Toc_null_check_conv(
toplevel, _temp807, e); goto _LL800; _LL804: e= Cyc_Toc_tag_ptr_conv( toplevel,
_temp811, _temp809, e); goto _LL800; _LL806: e= Cyc_Toc_untag_ptr_conv( toplevel,
_temp817, _temp815, _temp813, e); goto _LL800; _LL800:;}} return e;} struct Cyc_Toc_Env{
struct Cyc_Core_Opt* break_lab; struct Cyc_Core_Opt* continue_lab; struct Cyc_Core_Opt*
fallthru_info; struct Cyc_Dict_Dict* varmap; int toplevel; } ; typedef struct
Cyc_Toc_Env* Cyc_Toc_env_t; static struct Cyc_Toc_Env* Cyc_Toc_empty_env(){
return({ struct Cyc_Toc_Env* _temp819=( struct Cyc_Toc_Env*) GC_malloc( sizeof(
struct Cyc_Toc_Env)); _temp819->break_lab= 0; _temp819->continue_lab= 0;
_temp819->fallthru_info= 0; _temp819->varmap=(( struct Cyc_Dict_Dict*(*)( int(*
comp)( struct _tuple1*, struct _tuple1*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp);
_temp819->toplevel= 1; _temp819;});} static struct Cyc_Toc_Env* Cyc_Toc_copy_env(
struct Cyc_Toc_Env* e){ return({ struct Cyc_Toc_Env* _temp820=( struct Cyc_Toc_Env*)
GC_malloc( sizeof( struct Cyc_Toc_Env)); _temp820->break_lab= e->break_lab;
_temp820->continue_lab= e->continue_lab; _temp820->fallthru_info= e->fallthru_info;
_temp820->varmap= e->varmap; _temp820->toplevel= e->toplevel; _temp820;});}
static struct Cyc_Toc_Env* Cyc_Toc_clear_toplevel( struct Cyc_Toc_Env* e){
struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->toplevel= 0; return ans;}
static struct Cyc_Toc_Env* Cyc_Toc_add_varmap( struct Cyc_Toc_Env* e, struct
_tuple1* x, struct Cyc_Absyn_Exp* y){{ void* _temp821=(* x).f1; struct Cyc_List_List*
_temp827; _LL823: if(( unsigned int) _temp821 > 1u?(( struct _tunion_struct*)
_temp821)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL828: _temp827=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp821)->f1; goto _LL824;} else{ goto _LL825;}
_LL825: goto _LL826; _LL824:( void) _throw(( struct _xtunion_struct*)({ struct
Cyc_Core_Impossible_struct* _temp829=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp829[ 0]=({ struct
Cyc_Core_Impossible_struct _temp830; _temp830.tag= Cyc_Core_Impossible_tag;
_temp830.f1=({ struct _tagged_string _temp831= Cyc_Absynpp_qvar2string( x);
xprintf("Toc::add_varmap on Rel_n: %.*s\n", _temp831.last_plus_one - _temp831.curr,
_temp831.curr);}); _temp830;}); _temp829;})); _LL826: goto _LL822; _LL822:;}{
struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->varmap=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tuple1* key, struct Cyc_Absyn_Exp* data)) Cyc_Dict_insert)(
ans->varmap, x, y); return ans;}} static struct Cyc_Toc_Env* Cyc_Toc_loop_env(
struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab=
0; ans->continue_lab= 0; return ans;} static struct Cyc_Toc_Env* Cyc_Toc_non_last_switchclause_env(
struct Cyc_Toc_Env* e, struct _tagged_string* break_l, struct _tagged_string*
fallthru_l, struct Cyc_List_List* fallthru_binders, struct Cyc_Toc_Env*
next_case_env){ struct Cyc_List_List* fallthru_vars= 0; for( 0; fallthru_binders
!= 0; fallthru_binders=({ struct Cyc_List_List* _temp832= fallthru_binders; if(
_temp832 == 0){ _throw( Null_Exception);} _temp832->tl;})){ fallthru_vars=({
struct Cyc_List_List* _temp833=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp833->hd=( void*)(( struct Cyc_Absyn_Vardecl*)({
struct Cyc_List_List* _temp834= fallthru_binders; if( _temp834 == 0){ _throw(
Null_Exception);} _temp834->hd;}))->name; _temp833->tl= fallthru_vars; _temp833;});}
fallthru_vars=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fallthru_vars);{ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab=({
struct Cyc_Core_Opt* _temp835=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp835->v=( void*) break_l; _temp835;}); ans->fallthru_info=({
struct Cyc_Core_Opt* _temp836=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp836->v=( void*)({ struct _tuple8* _temp837=( struct _tuple8*)
GC_malloc( sizeof( struct _tuple8)); _temp837->f1= fallthru_l; _temp837->f2=
fallthru_vars; _temp837->f3= next_case_env->varmap; _temp837;}); _temp836;});
return ans;}} static struct Cyc_Toc_Env* Cyc_Toc_last_switchclause_env( struct
Cyc_Toc_Env* e, struct _tagged_string* break_l){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env(
e); ans->break_lab=({ struct Cyc_Core_Opt* _temp838=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp838->v=( void*) break_l; _temp838;});
ans->fallthru_info= 0; return ans;} static struct Cyc_Toc_Env* Cyc_Toc_switch_as_switch_env(
struct Cyc_Toc_Env* e, struct _tagged_string* next_l){ struct Cyc_Toc_Env* ans=
Cyc_Toc_copy_env( e); ans->break_lab= 0; ans->fallthru_info=({ struct Cyc_Core_Opt*
_temp839=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp839->v=( void*)({ struct _tuple8* _temp840=( struct _tuple8*) GC_malloc(
sizeof( struct _tuple8)); _temp840->f1= next_l; _temp840->f2= 0; _temp840->f3=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple1*, struct _tuple1*))) Cyc_Dict_empty)(
Cyc_Absyn_qvar_cmp); _temp840;}); _temp839;}); return ans;} static struct Cyc_Absyn_Exp*
Cyc_Toc_printf_to_c( void* p, struct Cyc_Core_Opt* aopt, struct Cyc_List_List*
args, struct Cyc_List_List* typs){ if( args == 0){ return(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp841=( char*)"expecting a literal format string"; struct _tagged_string
_temp842; _temp842.curr= _temp841; _temp842.base= _temp841; _temp842.last_plus_one=
_temp841 + 34; _temp842;}));}{ struct _tagged_string fmt_str;{ void* _temp844=(
void*)(( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp843= args; if(
_temp843 == 0){ _throw( Null_Exception);} _temp843->hd;}))->r; void* _temp850;
struct _tagged_string _temp852; _LL846: if((( struct _tunion_struct*) _temp844)->tag
== Cyc_Absyn_Const_e_tag){ _LL851: _temp850=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp844)->f1; if(( unsigned int) _temp850 > 1u?(( struct _tunion_struct*)
_temp850)->tag == Cyc_Absyn_String_c_tag: 0){ _LL853: _temp852=( struct
_tagged_string)(( struct Cyc_Absyn_String_c_struct*) _temp850)->f1; goto _LL847;}
else{ goto _LL848;}} else{ goto _LL848;} _LL848: goto _LL849; _LL847: fmt_str=
_temp852; goto _LL845; _LL849: return(( struct Cyc_Absyn_Exp*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp854=(
char*)"expecting a literal format string"; struct _tagged_string _temp855;
_temp855.curr= _temp854; _temp855.base= _temp854; _temp855.last_plus_one=
_temp854 + 34; _temp855;})); _LL845:;}{ int len=( int)({ struct _tagged_string
_temp962= fmt_str;( unsigned int)( _temp962.last_plus_one - _temp962.curr);});
int i= 0; for( i= 0; i < len; i ++){ if(({ struct _tagged_string _temp856=
fmt_str; char* _temp858= _temp856.curr + i; if( _temp856.base == 0? 1:( _temp858
< _temp856.base? 1: _temp858 >= _temp856.last_plus_one)){ _throw( Null_Exception);}*
_temp858;}) =='%'){ i ++; if( i < len?({ struct _tagged_string _temp859= fmt_str;
char* _temp861= _temp859.curr + i; if( _temp859.base == 0? 1:( _temp861 <
_temp859.base? 1: _temp861 >= _temp859.last_plus_one)){ _throw( Null_Exception);}*
_temp861;}) =='s': 0){ break;}}} if( i == len){ if( aopt != 0){ args=({ struct
Cyc_List_List* _temp862=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp862->hd=( void*)(( struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp863=
aopt; if( _temp863 == 0){ _throw( Null_Exception);} _temp863->v;})); _temp862->tl=
args; _temp862;});} return Cyc_Absyn_primop_exp( p, args, 0);} args=({ struct
Cyc_List_List* _temp864= args; if( _temp864 == 0){ _throw( Null_Exception);}
_temp864->tl;}); typs=({ struct Cyc_List_List* _temp865= typs; if( _temp865 == 0){
_throw( Null_Exception);} _temp865->tl;});{ struct Cyc_List_List* rev_fmt= 0;
struct Cyc_List_List* rev_result= 0; struct Cyc_List_List* rev_temps= 0; for( i=
0; i < len; i ++){ char c=({ struct _tagged_string _temp952= fmt_str; char*
_temp954= _temp952.curr + i; if( _temp952.base == 0? 1:( _temp954 < _temp952.base?
1: _temp954 >= _temp952.last_plus_one)){ _throw( Null_Exception);}* _temp954;});
rev_fmt=({ struct Cyc_List_List* _temp866=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp866->hd=( void*)(( int) c); _temp866->tl=
rev_fmt; _temp866;}); if( c !='%'){ continue;}{ struct Cyc_Core_Opt* x= Cyc_Formatstr_parse_conversionspecification(
fmt_str, i + 1); if( x == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp867=( char*)"bad format string"; struct
_tagged_string _temp868; _temp868.curr= _temp867; _temp868.base= _temp867;
_temp868.last_plus_one= _temp867 + 18; _temp868;}));}{ struct _tuple9 _temp872;
int _temp873; char _temp875; struct Cyc_List_List* _temp877; struct Cyc_List_List*
_temp879; struct Cyc_List_List* _temp881; struct Cyc_List_List* _temp883; struct
_tuple9* _temp870=( struct _tuple9*)({ struct Cyc_Core_Opt* _temp869= x; if(
_temp869 == 0){ _throw( Null_Exception);} _temp869->v;}); _temp872=* _temp870;
_LL884: _temp883= _temp872.f1; goto _LL882; _LL882: _temp881= _temp872.f2; goto
_LL880; _LL880: _temp879= _temp872.f3; goto _LL878; _LL878: _temp877= _temp872.f4;
goto _LL876; _LL876: _temp875= _temp872.f5; goto _LL874; _LL874: _temp873=
_temp872.f6; goto _LL871; _LL871: i= _temp873 - 1; if( _temp875 !='s'){ rev_fmt=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_revappend)(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)(({ struct
Cyc_List_List* _temp885=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp885->hd=( void*) _temp883; _temp885->tl=({ struct Cyc_List_List* _temp886=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp886->hd=(
void*) _temp881; _temp886->tl=({ struct Cyc_List_List* _temp887=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp887->hd=( void*) _temp879;
_temp887->tl=({ struct Cyc_List_List* _temp888=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp888->hd=( void*) _temp877;
_temp888->tl= 0; _temp888;}); _temp887;}); _temp886;}); _temp885;})), rev_fmt);
rev_fmt=({ struct Cyc_List_List* _temp889=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp889->hd=( void*)(( int) _temp875); _temp889->tl=
rev_fmt; _temp889;});{ struct Cyc_List_List* _temp890= _temp881; struct Cyc_List_List
_temp897; struct Cyc_List_List* _temp898; int _temp900; _LL892: if( _temp890 ==
0){ goto _LL894;} else{ _temp897=* _temp890; _LL901: _temp900=( int) _temp897.hd;
goto _LL899; _LL899: _temp898=( struct Cyc_List_List*) _temp897.tl; if( _temp898
== 0){ goto _LL896;} else{ goto _LL894;}} _LL896: if( _temp900 =='*'){ goto
_LL893;} else{ goto _LL894;} _LL894: goto _LL895; _LL893: { struct _tuple1* temp=
Cyc_Toc_temp_var(); rev_temps=({ struct Cyc_List_List* _temp902=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp902->hd=( void*)({ struct
_tuple10* _temp903=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10));
_temp903->f1= temp; _temp903->f2=( void*)({ struct Cyc_List_List* _temp905= typs;
if( _temp905 == 0){ _throw( Null_Exception);} _temp905->hd;}); _temp903->f3=(
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp904= args; if( _temp904 == 0){
_throw( Null_Exception);} _temp904->hd;}); _temp903;}); _temp902->tl= rev_temps;
_temp902;}); rev_result=({ struct Cyc_List_List* _temp906=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp906->hd=( void*) Cyc_Absyn_var_exp(
temp, 0); _temp906->tl= rev_result; _temp906;}); args=({ struct Cyc_List_List*
_temp907= args; if( _temp907 == 0){ _throw( Null_Exception);} _temp907->tl;});
typs=({ struct Cyc_List_List* _temp908= typs; if( _temp908 == 0){ _throw(
Null_Exception);} _temp908->tl;}); goto _LL891;} _LL895: goto _LL891; _LL891:;}{
struct Cyc_List_List* _temp909= _temp879; struct Cyc_List_List _temp916; struct
Cyc_List_List* _temp917; struct Cyc_List_List _temp919; struct Cyc_List_List*
_temp920; int _temp922; int _temp924; _LL911: if( _temp909 == 0){ goto _LL913;}
else{ _temp916=* _temp909; _LL925: _temp924=( int) _temp916.hd; goto _LL918;
_LL918: _temp917=( struct Cyc_List_List*) _temp916.tl; if( _temp917 == 0){ goto
_LL913;} else{ _temp919=* _temp917; _LL923: _temp922=( int) _temp919.hd; goto
_LL921; _LL921: _temp920=( struct Cyc_List_List*) _temp919.tl; if( _temp920 == 0){
goto _LL915;} else{ goto _LL913;}}} _LL915: if( _temp924 =='.'? _temp922 =='*':
0){ goto _LL912;} else{ goto _LL913;} _LL913: goto _LL914; _LL912: { struct
_tuple1* temp= Cyc_Toc_temp_var(); rev_temps=({ struct Cyc_List_List* _temp926=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp926->hd=(
void*)({ struct _tuple10* _temp927=( struct _tuple10*) GC_malloc( sizeof( struct
_tuple10)); _temp927->f1= temp; _temp927->f2=( void*)({ struct Cyc_List_List*
_temp929= typs; if( _temp929 == 0){ _throw( Null_Exception);} _temp929->hd;});
_temp927->f3=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp928= args;
if( _temp928 == 0){ _throw( Null_Exception);} _temp928->hd;}); _temp927;});
_temp926->tl= rev_temps; _temp926;}); rev_result=({ struct Cyc_List_List*
_temp930=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp930->hd=( void*) Cyc_Absyn_var_exp( temp, 0); _temp930->tl= rev_result;
_temp930;}); args=({ struct Cyc_List_List* _temp931= args; if( _temp931 == 0){
_throw( Null_Exception);} _temp931->tl;}); typs=({ struct Cyc_List_List*
_temp932= typs; if( _temp932 == 0){ _throw( Null_Exception);} _temp932->tl;});
goto _LL910;} _LL914: goto _LL910; _LL910:;} if( _temp875 !='%'){ if( args == 0?
1: typs == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct
_tagged_string _temp933= Cyc_Absynpp_prim2string( p); xprintf("missing arg to %.*s",
_temp933.last_plus_one - _temp933.curr, _temp933.curr);}));}{ struct _tuple1*
temp= Cyc_Toc_temp_var(); rev_temps=({ struct Cyc_List_List* _temp934=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp934->hd=( void*)({
struct _tuple10* _temp935=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10));
_temp935->f1= temp; _temp935->f2=( void*)({ struct Cyc_List_List* _temp937= typs;
if( _temp937 == 0){ _throw( Null_Exception);} _temp937->hd;}); _temp935->f3=(
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp936= args; if( _temp936 == 0){
_throw( Null_Exception);} _temp936->hd;}); _temp935;}); _temp934->tl= rev_temps;
_temp934;}); rev_result=({ struct Cyc_List_List* _temp938=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp938->hd=( void*) Cyc_Absyn_var_exp(
temp, 0); _temp938->tl= rev_result; _temp938;}); args=({ struct Cyc_List_List*
_temp939= args; if( _temp939 == 0){ _throw( Null_Exception);} _temp939->tl;});
typs=({ struct Cyc_List_List* _temp940= typs; if( _temp940 == 0){ _throw(
Null_Exception);} _temp940->tl;});}}} else{ rev_fmt=({ struct Cyc_List_List*
_temp941=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp941->hd=( void*)(( int)'.'); _temp941->tl= rev_fmt; _temp941;}); rev_fmt=({
struct Cyc_List_List* _temp942=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp942->hd=( void*)(( int)'*'); _temp942->tl= rev_fmt;
_temp942;}); rev_fmt=({ struct Cyc_List_List* _temp943=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp943->hd=( void*)(( int)'s');
_temp943->tl= rev_fmt; _temp943;});{ struct _tuple1* temp= Cyc_Toc_temp_var();
rev_temps=({ struct Cyc_List_List* _temp944=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp944->hd=( void*)({ struct _tuple10*
_temp945=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp945->f1=
temp; _temp945->f2=( void*)({ struct Cyc_List_List* _temp947= typs; if( _temp947
== 0){ _throw( Null_Exception);} _temp947->hd;}); _temp945->f3=( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp946= args; if( _temp946 == 0){ _throw( Null_Exception);}
_temp946->hd;}); _temp945;}); _temp944->tl= rev_temps; _temp944;});{ struct Cyc_Absyn_Exp*
size_exp= Cyc_Absyn_subtract_exp( Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
temp, 0), Cyc_Toc_last_plus_one_sp, 0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
temp, 0), Cyc_Toc_curr_sp, 0), 0); struct Cyc_Absyn_Exp* cstring_exp= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0); rev_result=({ struct Cyc_List_List*
_temp948=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp948->hd=( void*) size_exp; _temp948->tl= rev_result; _temp948;});
rev_result=({ struct Cyc_List_List* _temp949=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp949->hd=( void*) cstring_exp; _temp949->tl=
rev_result; _temp949;}); args=({ struct Cyc_List_List* _temp950= args; if(
_temp950 == 0){ _throw( Null_Exception);} _temp950->tl;}); typs=({ struct Cyc_List_List*
_temp951= typs; if( _temp951 == 0){ _throw( Null_Exception);} _temp951->tl;});}}}}}}{
struct Cyc_List_List* es=({ struct Cyc_List_List* _temp961=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp961->hd=( void*) Cyc_Absyn_string_exp(
Cyc_String_implode((( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
rev_fmt)), 0); _temp961->tl=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_rev)( rev_result); _temp961;}); if( aopt != 0){ es=({ struct Cyc_List_List*
_temp955=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp955->hd=( void*)(( struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp956=
aopt; if( _temp956 == 0){ _throw( Null_Exception);} _temp956->v;})); _temp955->tl=
es; _temp955;});}{ struct Cyc_Absyn_Exp* e= Cyc_Absyn_primop_exp( p, es, 0);
struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( e, 0); for( 0; rev_temps != 0;
rev_temps=({ struct Cyc_List_List* _temp957= rev_temps; if( _temp957 == 0){
_throw( Null_Exception);} _temp957->tl;})){ s= Cyc_Absyn_declare_stmt((*((
struct _tuple10*)({ struct Cyc_List_List* _temp958= rev_temps; if( _temp958 == 0){
_throw( Null_Exception);} _temp958->hd;}))).f1,(*(( struct _tuple10*)({ struct
Cyc_List_List* _temp959= rev_temps; if( _temp959 == 0){ _throw( Null_Exception);}
_temp959->hd;}))).f2,( struct Cyc_Absyn_Exp*)(*(( struct _tuple10*)({ struct Cyc_List_List*
_temp960= rev_temps; if( _temp960 == 0){ _throw( Null_Exception);} _temp960->hd;}))).f3,
s, 0);} return Cyc_Absyn_stmt_exp( s, 0);}}}}}} static void* Cyc_Toc_get_c_typ(
struct Cyc_Absyn_Exp* e){ if( e->topt == 0){(( void(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp963=( char*)"Missing type in primop ";
struct _tagged_string _temp964; _temp964.curr= _temp963; _temp964.base= _temp963;
_temp964.last_plus_one= _temp963 + 24; _temp964;}));} return Cyc_Toc_typ_to_c((
void*)({ struct Cyc_Core_Opt* _temp965= e->topt; if( _temp965 == 0){ _throw(
Null_Exception);} _temp965->v;}));} static void* Cyc_Toc_get_cyc_typ( struct Cyc_Absyn_Exp*
e){ if( e->topt == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp966=( char*)"Missing type in primop ";
struct _tagged_string _temp967; _temp967.curr= _temp966; _temp967.base= _temp966;
_temp967.last_plus_one= _temp966 + 24; _temp967;}));} return( void*)({ struct
Cyc_Core_Opt* _temp968= e->topt; if( _temp968 == 0){ _throw( Null_Exception);}
_temp968->v;});} static struct _tuple5* Cyc_Toc_tup_to_c( struct Cyc_Absyn_Exp*
e){ return({ struct _tuple5* _temp969=( struct _tuple5*) GC_malloc( sizeof(
struct _tuple5)); _temp969->f1= Cyc_Toc_mt_tq; _temp969->f2= Cyc_Toc_typ_to_c((
void*)({ struct Cyc_Core_Opt* _temp970= e->topt; if( _temp970 == 0){ _throw(
Null_Exception);} _temp970->v;})); _temp969;});} static void Cyc_Toc_exp_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* e); static struct _tuple7* Cyc_Toc_add_designator(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* e){ Cyc_Toc_exp_to_c( nv, e);
return({ struct _tuple7* _temp971=( struct _tuple7*) GC_malloc( sizeof( struct
_tuple7) * 1); _temp971[ 0]=({ struct _tuple7 _temp972; _temp972.f1= 0; _temp972.f2=
e; _temp972;}); _temp971;});} static struct Cyc_Absyn_Exp* Cyc_Toc_make_struct(
struct Cyc_Toc_Env* nv, struct _tuple1* x, void* struct_typ, struct Cyc_Absyn_Stmt*
s, int pointer, struct Cyc_Absyn_Exp* rgnopt, int is_atomic){ struct Cyc_Absyn_Exp*
eo; void* t; if( pointer){ t= Cyc_Absyn_cstar_typ( struct_typ, Cyc_Toc_mt_tq);{
struct Cyc_Absyn_Exp* se= Cyc_Absyn_sizeoftyp_exp( struct_typ, 0); if( rgnopt ==
0){ eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( t, is_atomic? Cyc_Toc_malloc_atomic(
se): Cyc_Toc_malloc_ptr( se), 0);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp973= rgnopt; if( _temp973 == 0){ _throw(
Null_Exception);} _temp973;}); Cyc_Toc_exp_to_c( nv, r); eo=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_cast_exp( t, Cyc_Toc_rmalloc_exp( r, se), 0);}}} else{ t= struct_typ;
eo= 0;} return Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, t, eo, s, 0), 0);}
static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s, int
e1_already_translated); static struct Cyc_Absyn_Stmt* Cyc_Toc_init_array( struct
Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* lhs, struct Cyc_List_List* dles0, struct
Cyc_Absyn_Stmt* s){ int count=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
dles0) - 1;{ struct Cyc_List_List* dles=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( dles0); for( 0; dles != 0; dles=({ struct Cyc_List_List*
_temp974= dles; if( _temp974 == 0){ _throw( Null_Exception);} _temp974->tl;})){
struct _tuple7 _temp978; struct Cyc_Absyn_Exp* _temp979; struct Cyc_List_List*
_temp981; struct _tuple7* _temp976=( struct _tuple7*)({ struct Cyc_List_List*
_temp975= dles; if( _temp975 == 0){ _throw( Null_Exception);} _temp975->hd;});
_temp978=* _temp976; _LL982: _temp981= _temp978.f1; goto _LL980; _LL980:
_temp979= _temp978.f2; goto _LL977; _LL977: { struct Cyc_Absyn_Exp* e_index; if(
_temp981 == 0){ e_index= Cyc_Absyn_signed_int_exp(( count --), 0);} else{ if(({
struct Cyc_List_List* _temp983= _temp981; if( _temp983 == 0){ _throw(
Null_Exception);} _temp983->tl;}) != 0){(( void(*)( struct _tagged_string s))
Cyc_Toc_unimp)(( struct _tagged_string)({ char* _temp984=( char*)"multiple designators in array";
struct _tagged_string _temp985; _temp985.curr= _temp984; _temp985.base= _temp984;
_temp985.last_plus_one= _temp984 + 30; _temp985;}));}{ void* d=( void*)({ struct
Cyc_List_List* _temp998= _temp981; if( _temp998 == 0){ _throw( Null_Exception);}
_temp998->hd;}); void* _temp986= d; struct Cyc_Absyn_Exp* _temp992; struct
_tagged_string* _temp994; _LL988: if((( struct _tunion_struct*) _temp986)->tag
== Cyc_Absyn_ArrayElement_tag){ _LL993: _temp992=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayElement_struct*) _temp986)->f1; goto _LL989;} else{ goto
_LL990;} _LL990: if((( struct _tunion_struct*) _temp986)->tag == Cyc_Absyn_FieldName_tag){
_LL995: _temp994=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp986)->f1; goto _LL991;} else{ goto _LL987;} _LL989: Cyc_Toc_exp_to_c( nv,
_temp992); e_index= _temp992; goto _LL987; _LL991: e_index=(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char*
_temp996=( char*)"field name designators in array"; struct _tagged_string
_temp997; _temp997.curr= _temp996; _temp997.base= _temp996; _temp997.last_plus_one=
_temp996 + 32; _temp997;})); goto _LL987; _LL987:;}}{ struct Cyc_Absyn_Exp* lval=
Cyc_Absyn_subscript_exp( lhs, e_index, 0); void* _temp999=( void*) _temp979->r;
struct Cyc_List_List* _temp1007; struct Cyc_Absyn_Exp* _temp1009; struct Cyc_Absyn_Exp*
_temp1011; struct Cyc_Absyn_Vardecl* _temp1013; _LL1001: if((( struct
_tunion_struct*) _temp999)->tag == Cyc_Absyn_Array_e_tag){ _LL1008: _temp1007=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp999)->f1; goto
_LL1002;} else{ goto _LL1003;} _LL1003: if((( struct _tunion_struct*) _temp999)->tag
== Cyc_Absyn_Comprehension_e_tag){ _LL1014: _temp1013=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp999)->f1; goto _LL1012; _LL1012:
_temp1011=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp999)->f2; goto _LL1010; _LL1010: _temp1009=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp999)->f3; goto _LL1004;} else{
goto _LL1005;} _LL1005: goto _LL1006; _LL1002: s= Cyc_Toc_init_array( nv, lval,
_temp1007, s); goto _LL1000; _LL1004: s= Cyc_Toc_init_comprehension( nv, lval,
_temp1013, _temp1011, _temp1009, s, 0); goto _LL1000; _LL1006: Cyc_Toc_exp_to_c(
nv, _temp979); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
lhs, e_index, 0), _temp979, 0), s, 0); goto _LL1000; _LL1000:;}}}} return s;}
static struct Cyc_Absyn_Stmt* Cyc_Toc_init_comprehension( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Exp* lhs, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s, int
e1_already_translated){ struct _tuple1* x1= vd->name; if( !
e1_already_translated){ Cyc_Toc_exp_to_c( nv, e1);}{ struct Cyc_Toc_Env* nv2=
Cyc_Toc_add_varmap( nv, x1, Cyc_Absyn_varb_exp( x1,( void*)({ struct Cyc_Absyn_Local_b_struct*
_temp1031=( struct Cyc_Absyn_Local_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp1031[ 0]=({ struct Cyc_Absyn_Local_b_struct _temp1032; _temp1032.tag= Cyc_Absyn_Local_b_tag;
_temp1032.f1= vd; _temp1032;}); _temp1031;}), 0)); struct _tuple1* max= Cyc_Toc_temp_var();
struct _tuple1* a= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* ea= Cyc_Absyn_assign_exp(
Cyc_Absyn_var_exp( x1, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp*
eb= Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp( x1, 0), Cyc_Absyn_var_exp( max, 0), 0);
struct Cyc_Absyn_Exp* ec= Cyc_Absyn_post_inc_exp( Cyc_Absyn_var_exp( x1, 0), 0);
struct Cyc_Absyn_Exp* lval= Cyc_Absyn_subscript_exp( lhs, Cyc_Absyn_var_exp( x1,
0), 0); struct Cyc_Absyn_Stmt* body;{ void* _temp1015=( void*) e2->r; struct Cyc_List_List*
_temp1023; struct Cyc_Absyn_Exp* _temp1025; struct Cyc_Absyn_Exp* _temp1027;
struct Cyc_Absyn_Vardecl* _temp1029; _LL1017: if((( struct _tunion_struct*)
_temp1015)->tag == Cyc_Absyn_Array_e_tag){ _LL1024: _temp1023=( struct Cyc_List_List*)((
struct Cyc_Absyn_Array_e_struct*) _temp1015)->f1; goto _LL1018;} else{ goto
_LL1019;} _LL1019: if((( struct _tunion_struct*) _temp1015)->tag == Cyc_Absyn_Comprehension_e_tag){
_LL1030: _temp1029=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1015)->f1; goto _LL1028; _LL1028: _temp1027=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1015)->f2; goto _LL1026; _LL1026:
_temp1025=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1015)->f3; goto _LL1020;} else{ goto _LL1021;} _LL1021: goto _LL1022;
_LL1018: body= Cyc_Toc_init_array( nv2, lval, _temp1023, Cyc_Absyn_skip_stmt( 0));
goto _LL1016; _LL1020: body= Cyc_Toc_init_comprehension( nv2, lval, _temp1029,
_temp1027, _temp1025, Cyc_Absyn_skip_stmt( 0), 0); goto _LL1016; _LL1022: Cyc_Toc_exp_to_c(
nv2, e2); body= Cyc_Absyn_assign_stmt( lval, e2, 0); goto _LL1016; _LL1016:;}{
struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_for_stmt( ea, eb, ec, body, 0); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_declare_stmt( max, Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*) e1, Cyc_Absyn_declare_stmt(
x1, Cyc_Absyn_uint_t, 0, s2, 0), 0), s, 0); return s;}}} static struct Cyc_Absyn_Exp*
Cyc_Toc_init_tuple( struct Cyc_Toc_Env* nv, int pointer, struct Cyc_Absyn_Exp*
rgnopt, struct Cyc_List_List* es){ struct Cyc_List_List* tqs=(( struct Cyc_List_List*(*)(
struct _tuple5*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Toc_tup_to_c, es); struct _tagged_string* n= Cyc_Toc_add_tuple_type( tqs);
struct _tuple1* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp(
x, 0); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( xexp, 0); struct Cyc_Absyn_Exp*(*
lval_f)( struct Cyc_Absyn_Exp*, struct _tagged_string*, struct Cyc_Position_Segment*)=
pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp; int is_atomic= 1;
es=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( es);{
int i=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es); for( 0; es !=
0; es=({ struct Cyc_List_List* _temp1033= es; if( _temp1033 == 0){ _throw(
Null_Exception);} _temp1033->tl;}), -- i){ struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1053= es; if( _temp1053 == 0){ _throw( Null_Exception);}
_temp1053->hd;}); struct Cyc_Absyn_Exp* lval= lval_f( xexp,({ struct
_tagged_string* _temp1052=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp1052[ 0]= xprintf("f%d", i); _temp1052;}), 0);
is_atomic= is_atomic? Cyc_Toc_atomic_typ(( void*)({ struct Cyc_Core_Opt*
_temp1034= e->topt; if( _temp1034 == 0){ _throw( Null_Exception);} _temp1034->v;})):
0;{ void* _temp1035=( void*) e->r; struct Cyc_List_List* _temp1043; struct Cyc_Absyn_Exp*
_temp1045; struct Cyc_Absyn_Exp* _temp1047; struct Cyc_Absyn_Vardecl* _temp1049;
_LL1037: if((( struct _tunion_struct*) _temp1035)->tag == Cyc_Absyn_Array_e_tag){
_LL1044: _temp1043=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp1035)->f1; goto _LL1038;} else{ goto _LL1039;} _LL1039: if((( struct
_tunion_struct*) _temp1035)->tag == Cyc_Absyn_Comprehension_e_tag){ _LL1050:
_temp1049=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1035)->f1; goto _LL1048; _LL1048: _temp1047=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1035)->f2; goto _LL1046; _LL1046:
_temp1045=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1035)->f3; goto _LL1040;} else{ goto _LL1041;} _LL1041: goto _LL1042;
_LL1038: s= Cyc_Toc_init_array( nv, lval, _temp1043, s); goto _LL1036; _LL1040:
s= Cyc_Toc_init_comprehension( nv, lval, _temp1049, _temp1047, _temp1045, s, 0);
goto _LL1036; _LL1042: Cyc_Toc_exp_to_c( nv, e); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval_f( xexp,({ struct _tagged_string* _temp1051=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp1051[ 0]=
xprintf("f%d", i); _temp1051;}), 0), e, 0), 0), s, 0); goto _LL1036; _LL1036:;}}}
return Cyc_Toc_make_struct( nv, x, Cyc_Absyn_strct( n), s, pointer, rgnopt,
is_atomic);} static struct Cyc_Absyn_Exp* Cyc_Toc_init_struct( struct Cyc_Toc_Env*
nv, void* struct_type, int pointer, struct Cyc_Absyn_Exp* rgnopt, struct Cyc_List_List*
dles, struct _tuple1* tdn){ struct _tuple1* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
xexp= Cyc_Absyn_var_exp( x, 0); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt(
xexp, 0); struct Cyc_Absyn_Exp*(* lval_f)( struct Cyc_Absyn_Exp*, struct
_tagged_string*, struct Cyc_Position_Segment*)= pointer? Cyc_Absyn_structarrow_exp:
Cyc_Absyn_structmember_exp; int is_atomic= 1;{ struct Cyc_List_List* es=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( dles); for( 0;
es != 0; es=({ struct Cyc_List_List* _temp1054= es; if( _temp1054 == 0){ _throw(
Null_Exception);} _temp1054->tl;})){ struct _tuple7 _temp1058; struct Cyc_Absyn_Exp*
_temp1059; struct Cyc_List_List* _temp1061; struct _tuple7* _temp1056=( struct
_tuple7*)({ struct Cyc_List_List* _temp1055= es; if( _temp1055 == 0){ _throw(
Null_Exception);} _temp1055->hd;}); _temp1058=* _temp1056; _LL1062: _temp1061=
_temp1058.f1; goto _LL1060; _LL1060: _temp1059= _temp1058.f2; goto _LL1057;
_LL1057: is_atomic= is_atomic? Cyc_Toc_atomic_typ(( void*)({ struct Cyc_Core_Opt*
_temp1063= _temp1059->topt; if( _temp1063 == 0){ _throw( Null_Exception);}
_temp1063->v;})): 0; if( _temp1061 == 0){(( void(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1064=( char*)"empty designator list";
struct _tagged_string _temp1065; _temp1065.curr= _temp1064; _temp1065.base=
_temp1064; _temp1065.last_plus_one= _temp1064 + 22; _temp1065;}));} if(({ struct
Cyc_List_List* _temp1066= _temp1061; if( _temp1066 == 0){ _throw( Null_Exception);}
_temp1066->tl;}) != 0){ struct _tuple1* y= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
yexp= Cyc_Absyn_var_exp( y, 0); for( 0; _temp1061 != 0; _temp1061=({ struct Cyc_List_List*
_temp1067= _temp1061; if( _temp1067 == 0){ _throw( Null_Exception);} _temp1067->tl;})){
void* _temp1069=( void*)({ struct Cyc_List_List* _temp1068= _temp1061; if(
_temp1068 == 0){ _throw( Null_Exception);} _temp1068->hd;}); struct
_tagged_string* _temp1075; _LL1071: if((( struct _tunion_struct*) _temp1069)->tag
== Cyc_Absyn_FieldName_tag){ _LL1076: _temp1075=( struct _tagged_string*)((
struct Cyc_Absyn_FieldName_struct*) _temp1069)->f1; goto _LL1072;} else{ goto
_LL1073;} _LL1073: goto _LL1074; _LL1072: if( Cyc_Toc_is_poly_field( struct_type,
_temp1075)){ yexp= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), yexp, 0);} s=
Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( lval_f( xexp,
_temp1075, 0), yexp, 0), 0), s, 0); goto _LL1070; _LL1074:( void) _throw(((
struct _xtunion_struct*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1077=( char*)"array designator in struct";
struct _tagged_string _temp1078; _temp1078.curr= _temp1077; _temp1078.base=
_temp1077; _temp1078.last_plus_one= _temp1077 + 27; _temp1078;}))); _LL1070:;}
Cyc_Toc_exp_to_c( nv, _temp1059); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
yexp, _temp1059, 0), 0), s, 0);} else{ void* _temp1080=( void*)({ struct Cyc_List_List*
_temp1079= _temp1061; if( _temp1079 == 0){ _throw( Null_Exception);} _temp1079->hd;});
struct _tagged_string* _temp1086; _LL1082: if((( struct _tunion_struct*)
_temp1080)->tag == Cyc_Absyn_FieldName_tag){ _LL1087: _temp1086=( struct
_tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp1080)->f1; goto
_LL1083;} else{ goto _LL1084;} _LL1084: goto _LL1085; _LL1083: { struct Cyc_Absyn_Exp*
lval= lval_f( xexp, _temp1086, 0);{ void* _temp1088=( void*) _temp1059->r;
struct Cyc_List_List* _temp1096; struct Cyc_Absyn_Exp* _temp1098; struct Cyc_Absyn_Exp*
_temp1100; struct Cyc_Absyn_Vardecl* _temp1102; _LL1090: if((( struct
_tunion_struct*) _temp1088)->tag == Cyc_Absyn_Array_e_tag){ _LL1097: _temp1096=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1088)->f1; goto
_LL1091;} else{ goto _LL1092;} _LL1092: if((( struct _tunion_struct*) _temp1088)->tag
== Cyc_Absyn_Comprehension_e_tag){ _LL1103: _temp1102=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1088)->f1; goto _LL1101; _LL1101:
_temp1100=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1088)->f2; goto _LL1099; _LL1099: _temp1098=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1088)->f3; goto _LL1093;} else{
goto _LL1094;} _LL1094: goto _LL1095; _LL1091: s= Cyc_Toc_init_array( nv, lval,
_temp1096, s); goto _LL1089; _LL1093: s= Cyc_Toc_init_comprehension( nv, lval,
_temp1102, _temp1100, _temp1098, s, 0); goto _LL1089; _LL1095: Cyc_Toc_exp_to_c(
nv, _temp1059); if( Cyc_Toc_is_poly_field( struct_type, _temp1086)){ _temp1059=
Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), _temp1059, 0);} s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( lval, _temp1059, 0), 0), s, 0); goto
_LL1089; _LL1089:;} goto _LL1081;} _LL1085:( void) _throw((( struct
_xtunion_struct*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp1104=( char*)"array designator in struct"; struct
_tagged_string _temp1105; _temp1105.curr= _temp1104; _temp1105.base= _temp1104;
_temp1105.last_plus_one= _temp1104 + 27; _temp1105;}))); _LL1081:;}}} return Cyc_Toc_make_struct(
nv, x, Cyc_Absyn_strctq( tdn), s, pointer, rgnopt, is_atomic);} static struct
Cyc_Absyn_Exp* Cyc_Toc_assignop_lvalue( struct Cyc_Absyn_Exp* el, struct
_tuple11* pr){ return Cyc_Absyn_assignop_exp( el,(* pr).f1,(* pr).f2, 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_address_lvalue( struct Cyc_Absyn_Exp* e1,
int ignore){ return Cyc_Absyn_address_exp( e1, 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_incr_lvalue( struct Cyc_Absyn_Exp* e1, void* incr){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Increment_e_struct* _temp1106=( struct Cyc_Absyn_Increment_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Increment_e_struct)); _temp1106[ 0]=({
struct Cyc_Absyn_Increment_e_struct _temp1107; _temp1107.tag= Cyc_Absyn_Increment_e_tag;
_temp1107.f1= e1; _temp1107.f2=( void*) incr; _temp1107;}); _temp1106;}), 0);}
static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env);
static void Cyc_Toc_lvalue_assign( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env){
void* _temp1108=( void*) e1->r; struct Cyc_Absyn_Stmt* _temp1118; struct Cyc_Absyn_Exp*
_temp1120; void* _temp1122; struct _tagged_string* _temp1124; struct Cyc_Absyn_Exp*
_temp1126; _LL1110: if((( struct _tunion_struct*) _temp1108)->tag == Cyc_Absyn_StmtExp_e_tag){
_LL1119: _temp1118=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1108)->f1; goto _LL1111;} else{ goto _LL1112;} _LL1112: if((( struct
_tunion_struct*) _temp1108)->tag == Cyc_Absyn_Cast_e_tag){ _LL1123: _temp1122=(
void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp1108)->f1; goto _LL1121; _LL1121:
_temp1120=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*) _temp1108)->f2;
goto _LL1113;} else{ goto _LL1114;} _LL1114: if((( struct _tunion_struct*)
_temp1108)->tag == Cyc_Absyn_StructMember_e_tag){ _LL1127: _temp1126=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*) _temp1108)->f1; goto
_LL1125; _LL1125: _temp1124=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1108)->f2; goto _LL1115;} else{ goto _LL1116;} _LL1116: goto _LL1117;
_LL1111:(( void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs, struct
Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp1118, fs, f, f_env); goto _LL1109; _LL1113:(( void(*)( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1120, fs, f, f_env); goto
_LL1109; _LL1115:( void*)( e1->r=( void*)(( void*) _temp1126->r));(( void(*)(
struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)( e1,({
struct Cyc_List_List* _temp1128=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1128->hd=( void*) _temp1124; _temp1128->tl= fs;
_temp1128;}), f, f_env); goto _LL1109; _LL1117: if( ! Cyc_Absyn_is_lvalue( e1)){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string
_temp1129= Cyc_Absynpp_exp2string( e1); xprintf("lvalue_assign: could not find lvalue in `%.*s'",
_temp1129.last_plus_one - _temp1129.curr, _temp1129.curr);}));}{ struct Cyc_Absyn_Exp*
e1_copy= Cyc_Absyn_copy_exp( e1); for( 0; fs != 0; fs=({ struct Cyc_List_List*
_temp1130= fs; if( _temp1130 == 0){ _throw( Null_Exception);} _temp1130->tl;})){
e1_copy= Cyc_Absyn_structmember_exp( e1_copy,( struct _tagged_string*)({ struct
Cyc_List_List* _temp1131= fs; if( _temp1131 == 0){ _throw( Null_Exception);}
_temp1131->hd;}), e1_copy->loc);}( void*)( e1->r=( void*)(( void*)( f( e1_copy,
f_env))->r)); goto _LL1109;} _LL1109:;} static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, void*), void* f_env){ void* _temp1132=( void*) s->r;
struct Cyc_Absyn_Exp* _temp1142; struct Cyc_Absyn_Stmt* _temp1144; struct Cyc_Absyn_Decl*
_temp1146; struct Cyc_Absyn_Stmt* _temp1148; struct Cyc_Absyn_Stmt* _temp1150;
_LL1134: if(( unsigned int) _temp1132 > 1u?(( struct _tunion_struct*) _temp1132)->tag
== Cyc_Absyn_Exp_s_tag: 0){ _LL1143: _temp1142=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Exp_s_struct*) _temp1132)->f1; goto _LL1135;} else{ goto _LL1136;}
_LL1136: if(( unsigned int) _temp1132 > 1u?(( struct _tunion_struct*) _temp1132)->tag
== Cyc_Absyn_Decl_s_tag: 0){ _LL1147: _temp1146=( struct Cyc_Absyn_Decl*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1132)->f1; goto _LL1145; _LL1145:
_temp1144=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1132)->f2;
goto _LL1137;} else{ goto _LL1138;} _LL1138: if(( unsigned int) _temp1132 > 1u?((
struct _tunion_struct*) _temp1132)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL1151:
_temp1150=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1132)->f1;
goto _LL1149; _LL1149: _temp1148=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp1132)->f2; goto _LL1139;} else{ goto _LL1140;} _LL1140: goto _LL1141;
_LL1135:(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct
Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)(
_temp1142, fs, f, f_env); goto _LL1133; _LL1137:(( void(*)( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)( _temp1144, fs, f, f_env);
goto _LL1133; _LL1139:(( void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp1148, fs, f, f_env); goto _LL1133; _LL1141:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1152= Cyc_Absynpp_stmt2string(
s); xprintf("lvalue_assign_stmt: %.*s", _temp1152.last_plus_one - _temp1152.curr,
_temp1152.curr);})); goto _LL1133; _LL1133:;} static struct Cyc_Absyn_Exp* Cyc_Toc_known_size_subscript(
struct Cyc_Absyn_Exp* arr, struct Cyc_Absyn_Exp* ind, int possibly_null, struct
Cyc_Absyn_Exp* esz, void* elt_typ, struct Cyc_Absyn_Tqual* tq, int take_address){
unsigned int sz= Cyc_Evexp_eval_const_uint_exp( esz); struct _tuple1* a= Cyc_Toc_temp_var();
struct _tuple1* i= Cyc_Toc_temp_var(); void* ta= Cyc_Absyn_cstar_typ( Cyc_Toc_typ_to_c(
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
struct Cyc_Absyn_Stmt* s); static void Cyc_Toc_exp_to_c( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Exp* e){ void* r=( void*) e->r; if( e->topt == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1153=
Cyc_Absynpp_exp2string( e); xprintf("exp_to_c: no type for %.*s", _temp1153.last_plus_one
- _temp1153.curr, _temp1153.curr);})); return;}{ void* old_typ=( void*)({ struct
Cyc_Core_Opt* _temp1857= e->topt; if( _temp1857 == 0){ _throw( Null_Exception);}
_temp1857->v;}); void* _temp1154= r; void* _temp1230; void* _temp1232; void*
_temp1234; struct _tuple1* _temp1236; struct _tuple1* _temp1238; struct Cyc_List_List*
_temp1240; void* _temp1242; void* _temp1244; struct Cyc_Absyn_Exp* _temp1246;
struct Cyc_Absyn_Exp* _temp1248; struct Cyc_Core_Opt* _temp1250; struct Cyc_Absyn_Exp*
_temp1252; struct Cyc_Absyn_Exp* _temp1254; struct Cyc_Absyn_Exp* _temp1256;
struct Cyc_Absyn_Exp* _temp1258; struct Cyc_Absyn_Exp* _temp1260; struct Cyc_Absyn_Exp*
_temp1262; struct Cyc_List_List* _temp1264; struct Cyc_Absyn_Exp* _temp1266;
struct Cyc_List_List* _temp1268; struct Cyc_Absyn_Exp* _temp1270; struct Cyc_Absyn_Exp*
_temp1272; struct Cyc_Absyn_Exp* _temp1274; struct Cyc_List_List* _temp1276;
struct Cyc_Absyn_Exp* _temp1278; struct Cyc_Absyn_Exp* _temp1280; struct Cyc_Absyn_Exp**
_temp1282; void* _temp1283; void** _temp1285; struct Cyc_Absyn_Exp* _temp1286;
struct Cyc_Absyn_Exp* _temp1288; struct Cyc_Absyn_Exp* _temp1290; void*
_temp1292; struct Cyc_Absyn_Exp* _temp1294; struct Cyc_Absyn_Exp* _temp1296;
struct _tagged_string* _temp1298; struct Cyc_Absyn_Exp* _temp1300; struct
_tagged_string* _temp1302; struct Cyc_Absyn_Exp* _temp1304; struct Cyc_Absyn_Exp*
_temp1306; struct Cyc_Absyn_Exp* _temp1308; struct Cyc_List_List* _temp1310;
struct Cyc_List_List* _temp1312; struct _tuple2* _temp1314; struct Cyc_List_List*
_temp1316; struct Cyc_Absyn_Exp* _temp1318; struct Cyc_Absyn_Exp* _temp1320;
struct Cyc_Absyn_Vardecl* _temp1322; struct Cyc_Absyn_Structdecl* _temp1324;
struct Cyc_List_List* _temp1326; struct Cyc_Core_Opt* _temp1328; struct _tuple1*
_temp1330; struct Cyc_Absyn_Tunionfield* _temp1332; struct Cyc_Absyn_Tuniondecl*
_temp1334; struct Cyc_List_List* _temp1336; struct Cyc_Core_Opt* _temp1338;
struct Cyc_Core_Opt* _temp1340; struct Cyc_Absyn_Tunionfield* _temp1342; struct
Cyc_Absyn_Tuniondecl* _temp1344; struct Cyc_List_List* _temp1346; struct Cyc_Core_Opt*
_temp1348; struct Cyc_Core_Opt* _temp1350; struct Cyc_Absyn_Tunionfield*
_temp1352; struct Cyc_Absyn_XTuniondecl* _temp1354; struct Cyc_List_List*
_temp1356; struct Cyc_Core_Opt* _temp1358; struct Cyc_Absyn_Enumfield* _temp1360;
struct Cyc_Absyn_Enumdecl* _temp1362; struct _tuple1* _temp1364; void* _temp1366;
struct Cyc_Absyn_Exp* _temp1368; struct Cyc_Absyn_Stmt* _temp1370; struct Cyc_List_List*
_temp1372; struct Cyc_Core_Opt* _temp1374; struct Cyc_Absyn_Fndecl* _temp1376;
struct Cyc_Absyn_Exp* _temp1378; _LL1156: if((( struct _tunion_struct*)
_temp1154)->tag == Cyc_Absyn_Const_e_tag){ _LL1231: _temp1230=( void*)(( struct
Cyc_Absyn_Const_e_struct*) _temp1154)->f1; if(( int) _temp1230 == Cyc_Absyn_Null_c){
goto _LL1157;} else{ goto _LL1158;}} else{ goto _LL1158;} _LL1158: if((( struct
_tunion_struct*) _temp1154)->tag == Cyc_Absyn_Const_e_tag){ _LL1233: _temp1232=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1154)->f1; goto _LL1159;} else{
goto _LL1160;} _LL1160: if((( struct _tunion_struct*) _temp1154)->tag == Cyc_Absyn_Var_e_tag){
_LL1237: _temp1236=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1154)->f1; goto _LL1235; _LL1235: _temp1234=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1154)->f2; goto _LL1161;} else{ goto _LL1162;} _LL1162: if((( struct
_tunion_struct*) _temp1154)->tag == Cyc_Absyn_UnknownId_e_tag){ _LL1239:
_temp1238=( struct _tuple1*)(( struct Cyc_Absyn_UnknownId_e_struct*) _temp1154)->f1;
goto _LL1163;} else{ goto _LL1164;} _LL1164: if((( struct _tunion_struct*)
_temp1154)->tag == Cyc_Absyn_Primop_e_tag){ _LL1243: _temp1242=( void*)(( struct
Cyc_Absyn_Primop_e_struct*) _temp1154)->f1; goto _LL1241; _LL1241: _temp1240=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1154)->f2; goto
_LL1165;} else{ goto _LL1166;} _LL1166: if((( struct _tunion_struct*) _temp1154)->tag
== Cyc_Absyn_Increment_e_tag){ _LL1247: _temp1246=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Increment_e_struct*) _temp1154)->f1; goto _LL1245; _LL1245:
_temp1244=( void*)(( struct Cyc_Absyn_Increment_e_struct*) _temp1154)->f2; goto
_LL1167;} else{ goto _LL1168;} _LL1168: if((( struct _tunion_struct*) _temp1154)->tag
== Cyc_Absyn_AssignOp_e_tag){ _LL1253: _temp1252=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1154)->f1; goto _LL1251; _LL1251:
_temp1250=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1154)->f2; goto _LL1249; _LL1249: _temp1248=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1154)->f3; goto _LL1169;} else{ goto
_LL1170;} _LL1170: if((( struct _tunion_struct*) _temp1154)->tag == Cyc_Absyn_Conditional_e_tag){
_LL1259: _temp1258=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1154)->f1; goto _LL1257; _LL1257: _temp1256=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp1154)->f2; goto _LL1255; _LL1255:
_temp1254=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1154)->f3; goto _LL1171;} else{ goto _LL1172;} _LL1172: if((( struct
_tunion_struct*) _temp1154)->tag == Cyc_Absyn_SeqExp_e_tag){ _LL1263: _temp1262=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*) _temp1154)->f1; goto
_LL1261; _LL1261: _temp1260=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1154)->f2; goto _LL1173;} else{ goto _LL1174;} _LL1174: if((( struct
_tunion_struct*) _temp1154)->tag == Cyc_Absyn_UnknownCall_e_tag){ _LL1267:
_temp1266=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1154)->f1; goto _LL1265; _LL1265: _temp1264=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_e_struct*) _temp1154)->f2; goto _LL1175;} else{
goto _LL1176;} _LL1176: if((( struct _tunion_struct*) _temp1154)->tag == Cyc_Absyn_FnCall_e_tag){
_LL1271: _temp1270=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1154)->f1; goto _LL1269; _LL1269: _temp1268=( struct Cyc_List_List*)((
struct Cyc_Absyn_FnCall_e_struct*) _temp1154)->f2; goto _LL1177;} else{ goto
_LL1178;} _LL1178: if((( struct _tunion_struct*) _temp1154)->tag == Cyc_Absyn_Throw_e_tag){
_LL1273: _temp1272=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*)
_temp1154)->f1; goto _LL1179;} else{ goto _LL1180;} _LL1180: if((( struct
_tunion_struct*) _temp1154)->tag == Cyc_Absyn_NoInstantiate_e_tag){ _LL1275:
_temp1274=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1154)->f1; goto _LL1181;} else{ goto _LL1182;} _LL1182: if((( struct
_tunion_struct*) _temp1154)->tag == Cyc_Absyn_Instantiate_e_tag){ _LL1279:
_temp1278=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1154)->f1; goto _LL1277; _LL1277: _temp1276=( struct Cyc_List_List*)((
struct Cyc_Absyn_Instantiate_e_struct*) _temp1154)->f2; goto _LL1183;} else{
goto _LL1184;} _LL1184: if((( struct _tunion_struct*) _temp1154)->tag == Cyc_Absyn_Cast_e_tag){
_LL1284: _temp1283=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp1154)->f1;
_temp1285=&(( struct Cyc_Absyn_Cast_e_struct*) _temp1154)->f1; goto _LL1281;
_LL1281: _temp1280=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1154)->f2; _temp1282=&(( struct Cyc_Absyn_Cast_e_struct*) _temp1154)->f2;
goto _LL1185;} else{ goto _LL1186;} _LL1186: if((( struct _tunion_struct*)
_temp1154)->tag == Cyc_Absyn_Address_e_tag){ _LL1287: _temp1286=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Address_e_struct*) _temp1154)->f1; goto _LL1187;} else{ goto
_LL1188;} _LL1188: if((( struct _tunion_struct*) _temp1154)->tag == Cyc_Absyn_New_e_tag){
_LL1291: _temp1290=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*)
_temp1154)->f1; goto _LL1289; _LL1289: _temp1288=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_New_e_struct*) _temp1154)->f2; goto _LL1189;} else{ goto
_LL1190;} _LL1190: if((( struct _tunion_struct*) _temp1154)->tag == Cyc_Absyn_Sizeoftyp_e_tag){
_LL1293: _temp1292=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp1154)->f1;
goto _LL1191;} else{ goto _LL1192;} _LL1192: if((( struct _tunion_struct*)
_temp1154)->tag == Cyc_Absyn_Sizeofexp_e_tag){ _LL1295: _temp1294=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Sizeofexp_e_struct*) _temp1154)->f1; goto _LL1193;} else{ goto
_LL1194;} _LL1194: if((( struct _tunion_struct*) _temp1154)->tag == Cyc_Absyn_Deref_e_tag){
_LL1297: _temp1296=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp1154)->f1; goto _LL1195;} else{ goto _LL1196;} _LL1196: if((( struct
_tunion_struct*) _temp1154)->tag == Cyc_Absyn_StructMember_e_tag){ _LL1301:
_temp1300=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1154)->f1; goto _LL1299; _LL1299: _temp1298=( struct _tagged_string*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp1154)->f2; goto _LL1197;} else{
goto _LL1198;} _LL1198: if((( struct _tunion_struct*) _temp1154)->tag == Cyc_Absyn_StructArrow_e_tag){
_LL1305: _temp1304=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1154)->f1; goto _LL1303; _LL1303: _temp1302=( struct _tagged_string*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp1154)->f2; goto _LL1199;} else{
goto _LL1200;} _LL1200: if((( struct _tunion_struct*) _temp1154)->tag == Cyc_Absyn_Subscript_e_tag){
_LL1309: _temp1308=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1154)->f1; goto _LL1307; _LL1307: _temp1306=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp1154)->f2; goto _LL1201;} else{ goto
_LL1202;} _LL1202: if((( struct _tunion_struct*) _temp1154)->tag == Cyc_Absyn_Tuple_e_tag){
_LL1311: _temp1310=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1154)->f1; goto _LL1203;} else{ goto _LL1204;} _LL1204: if((( struct
_tunion_struct*) _temp1154)->tag == Cyc_Absyn_CompoundLit_e_tag){ _LL1315:
_temp1314=( struct _tuple2*)(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp1154)->f1;
goto _LL1313; _LL1313: _temp1312=( struct Cyc_List_List*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1154)->f2; goto _LL1205;} else{ goto _LL1206;} _LL1206: if((( struct
_tunion_struct*) _temp1154)->tag == Cyc_Absyn_Array_e_tag){ _LL1317: _temp1316=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1154)->f1; goto
_LL1207;} else{ goto _LL1208;} _LL1208: if((( struct _tunion_struct*) _temp1154)->tag
== Cyc_Absyn_Comprehension_e_tag){ _LL1323: _temp1322=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1154)->f1; goto _LL1321; _LL1321:
_temp1320=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1154)->f2; goto _LL1319; _LL1319: _temp1318=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1154)->f3; goto _LL1209;} else{
goto _LL1210;} _LL1210: if((( struct _tunion_struct*) _temp1154)->tag == Cyc_Absyn_Struct_e_tag){
_LL1331: _temp1330=( struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1154)->f1; goto _LL1329; _LL1329: _temp1328=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1154)->f2; goto _LL1327; _LL1327:
_temp1326=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1154)->f3; goto _LL1325; _LL1325: _temp1324=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1154)->f4; goto _LL1211;} else{ goto
_LL1212;} _LL1212: if((( struct _tunion_struct*) _temp1154)->tag == Cyc_Absyn_Tunion_e_tag){
_LL1341: _temp1340=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1154)->f1; goto _LL1339; _LL1339: _temp1338=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1154)->f2; goto _LL1337; _LL1337:
_temp1336=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1154)->f3; if( _temp1336 == 0){ goto _LL1335;} else{ goto _LL1214;} _LL1335:
_temp1334=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1154)->f4; goto _LL1333; _LL1333: _temp1332=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1154)->f5; goto _LL1213;} else{ goto
_LL1214;} _LL1214: if((( struct _tunion_struct*) _temp1154)->tag == Cyc_Absyn_Tunion_e_tag){
_LL1351: _temp1350=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1154)->f1; goto _LL1349; _LL1349: _temp1348=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1154)->f2; goto _LL1347; _LL1347:
_temp1346=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1154)->f3; goto _LL1345; _LL1345: _temp1344=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1154)->f4; goto _LL1343; _LL1343:
_temp1342=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1154)->f5; goto _LL1215;} else{ goto _LL1216;} _LL1216: if((( struct
_tunion_struct*) _temp1154)->tag == Cyc_Absyn_XTunion_e_tag){ _LL1359: _temp1358=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_XTunion_e_struct*) _temp1154)->f1; goto
_LL1357; _LL1357: _temp1356=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_e_struct*)
_temp1154)->f2; goto _LL1355; _LL1355: _temp1354=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_e_struct*) _temp1154)->f3; goto _LL1353; _LL1353:
_temp1352=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_e_struct*)
_temp1154)->f4; goto _LL1217;} else{ goto _LL1218;} _LL1218: if((( struct
_tunion_struct*) _temp1154)->tag == Cyc_Absyn_Enum_e_tag){ _LL1365: _temp1364=(
struct _tuple1*)(( struct Cyc_Absyn_Enum_e_struct*) _temp1154)->f1; goto _LL1363;
_LL1363: _temp1362=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp1154)->f2; goto _LL1361; _LL1361: _temp1360=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_e_struct*) _temp1154)->f3; goto _LL1219;} else{ goto
_LL1220;} _LL1220: if((( struct _tunion_struct*) _temp1154)->tag == Cyc_Absyn_Malloc_e_tag){
_LL1369: _temp1368=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1154)->f1; goto _LL1367; _LL1367: _temp1366=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1154)->f2; goto _LL1221;} else{ goto _LL1222;} _LL1222: if((( struct
_tunion_struct*) _temp1154)->tag == Cyc_Absyn_StmtExp_e_tag){ _LL1371: _temp1370=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*) _temp1154)->f1;
goto _LL1223;} else{ goto _LL1224;} _LL1224: if((( struct _tunion_struct*)
_temp1154)->tag == Cyc_Absyn_UnresolvedMem_e_tag){ _LL1375: _temp1374=( struct
Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp1154)->f1; goto
_LL1373; _LL1373: _temp1372=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1154)->f2; goto _LL1225;} else{ goto _LL1226;} _LL1226: if((( struct
_tunion_struct*) _temp1154)->tag == Cyc_Absyn_Codegen_e_tag){ _LL1377: _temp1376=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*) _temp1154)->f1;
goto _LL1227;} else{ goto _LL1228;} _LL1228: if((( struct _tunion_struct*)
_temp1154)->tag == Cyc_Absyn_Fill_e_tag){ _LL1379: _temp1378=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Fill_e_struct*) _temp1154)->f1; goto _LL1229;} else{ goto
_LL1155;} _LL1157:{ void* _temp1380= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1386; struct Cyc_Absyn_Conref* _temp1388; struct Cyc_Absyn_Tqual* _temp1390;
struct Cyc_Absyn_Conref* _temp1392; void* _temp1394; void* _temp1396; _LL1382:
if(( unsigned int) _temp1380 > 4u?(( struct _tunion_struct*) _temp1380)->tag ==
Cyc_Absyn_PointerType_tag: 0){ _LL1387: _temp1386=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1380)->f1; _LL1397: _temp1396=( void*)
_temp1386.elt_typ; goto _LL1395; _LL1395: _temp1394=( void*) _temp1386.rgn_typ;
goto _LL1393; _LL1393: _temp1392=( struct Cyc_Absyn_Conref*) _temp1386.nullable;
goto _LL1391; _LL1391: _temp1390=( struct Cyc_Absyn_Tqual*) _temp1386.tq; goto
_LL1389; _LL1389: _temp1388=( struct Cyc_Absyn_Conref*) _temp1386.bounds; goto
_LL1383;} else{ goto _LL1384;} _LL1384: goto _LL1385; _LL1383: if(( void*)
_temp1388->v ==( void*) Cyc_Absyn_No_constr){( void*)( e->r=( void*)(( void*)(
Cyc_Absyn_signed_int_exp( 0, 0))->r)); goto _LL1381;}{ void* _temp1398=(( void*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp1388); struct Cyc_Absyn_Exp*
_temp1404; _LL1400: if(( int) _temp1398 == Cyc_Absyn_Unknown_b){ goto _LL1401;}
else{ goto _LL1402;} _LL1402: if(( unsigned int) _temp1398 > 1u?(( struct
_tunion_struct*) _temp1398)->tag == Cyc_Absyn_Upper_b_tag: 0){ _LL1405:
_temp1404=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp1398)->f1;
goto _LL1403;} else{ goto _LL1399;} _LL1401: { struct _tuple7* zero=({ struct
_tuple7* _temp1409=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7));
_temp1409->f1= 0; _temp1409->f2= Cyc_Absyn_signed_int_exp( 0, 0); _temp1409;});
struct Cyc_Absyn_Exp* um= Cyc_Absyn_unresolvedmem_exp( 0,({ struct Cyc_List_List*
_temp1406=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1406->hd=( void*) zero; _temp1406->tl=({ struct Cyc_List_List* _temp1407=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1407->hd=(
void*) zero; _temp1407->tl=({ struct Cyc_List_List* _temp1408=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1408->hd=( void*) zero;
_temp1408->tl= 0; _temp1408;}); _temp1407;}); _temp1406;}), 0); if( nv->toplevel){(
void*)( e->r=( void*)(( void*) um->r));} else{ void* t= Cyc_Absyn_strct( Cyc_Toc_add_tagged_ptr_type(
Cyc_Toc_typ_to_c( _temp1396), Cyc_Toc_mt_tq)); struct _tuple1* x= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp( x, 0); struct Cyc_Absyn_Stmt* s=
Cyc_Absyn_declare_stmt( x, t,( struct Cyc_Absyn_Exp*) um, Cyc_Absyn_exp_stmt(
xexp, 0), 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r));}
goto _LL1399;} _LL1403:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp(
0, 0))->r)); goto _LL1399; _LL1399:;} goto _LL1381; _LL1385:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1410= Cyc_Absynpp_typ2string(
old_typ); xprintf("exp_to_c: null given type %.*s", _temp1410.last_plus_one -
_temp1410.curr, _temp1410.curr);})); goto _LL1381; _LL1381:;} goto _LL1155;
_LL1159: goto _LL1155; _LL1161:{ struct _handler_cons _temp1411; _push_handler(&
_temp1411);{ struct _xtunion_struct* _temp1412=( struct _xtunion_struct*) setjmp(
_temp1411.handler); if( ! _temp1412){( void*)( e->r=( void*)(( void*)((( struct
Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict* d, struct _tuple1* key)) Cyc_Dict_lookup)(
nv->varmap, _temp1236))->r));; _pop_handler();} else{ struct _xtunion_struct*
_temp1414= _temp1412; _LL1416: if( _temp1414->tag == Cyc_Dict_Absent_tag){ goto
_LL1417;} else{ goto _LL1418;} _LL1418: goto _LL1419; _LL1417:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1420= Cyc_Absynpp_qvar2string(
_temp1236); xprintf("Can't find %.*s in exp_to_c, Var\n", _temp1420.last_plus_one
- _temp1420.curr, _temp1420.curr);})); return; _LL1419:( void) _throw( _temp1414);
_LL1415:;}}} goto _LL1155; _LL1163:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1421=( char*)"unknownid"; struct
_tagged_string _temp1422; _temp1422.curr= _temp1421; _temp1422.base= _temp1421;
_temp1422.last_plus_one= _temp1421 + 10; _temp1422;})); goto _LL1155; _LL1165: {
struct Cyc_List_List* old_typs=(( struct Cyc_List_List*(*)( void*(* f)( struct
Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_cyc_typ,
_temp1240); struct Cyc_List_List* typs=(( struct Cyc_List_List*(*)( void*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_c_typ,
_temp1240);(( void(*)( void(* f)( struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*),
struct Cyc_Toc_Env* env, struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c,
nv, _temp1240);{ void* _temp1423= _temp1242; _LL1425: if(( int) _temp1423 == Cyc_Absyn_Size){
goto _LL1426;} else{ goto _LL1427;} _LL1427: if(( int) _temp1423 == Cyc_Absyn_Printf){
goto _LL1428;} else{ goto _LL1429;} _LL1429: if(( int) _temp1423 == Cyc_Absyn_Xprintf){
goto _LL1430;} else{ goto _LL1431;} _LL1431: if(( int) _temp1423 == Cyc_Absyn_Fprintf){
goto _LL1432;} else{ goto _LL1433;} _LL1433: if(( int) _temp1423 == Cyc_Absyn_Sscanf){
goto _LL1434;} else{ goto _LL1435;} _LL1435: if(( int) _temp1423 == Cyc_Absyn_Plus){
goto _LL1436;} else{ goto _LL1437;} _LL1437: if(( int) _temp1423 == Cyc_Absyn_Minus){
goto _LL1438;} else{ goto _LL1439;} _LL1439: if(( int) _temp1423 == Cyc_Absyn_Eq){
goto _LL1440;} else{ goto _LL1441;} _LL1441: if(( int) _temp1423 == Cyc_Absyn_Neq){
goto _LL1442;} else{ goto _LL1443;} _LL1443: if(( int) _temp1423 == Cyc_Absyn_Gt){
goto _LL1444;} else{ goto _LL1445;} _LL1445: if(( int) _temp1423 == Cyc_Absyn_Gte){
goto _LL1446;} else{ goto _LL1447;} _LL1447: if(( int) _temp1423 == Cyc_Absyn_Lt){
goto _LL1448;} else{ goto _LL1449;} _LL1449: if(( int) _temp1423 == Cyc_Absyn_Lte){
goto _LL1450;} else{ goto _LL1451;} _LL1451: goto _LL1452; _LL1426: { struct Cyc_Absyn_Exp*
arg=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1486= _temp1240; if(
_temp1486 == 0){ _throw( Null_Exception);} _temp1486->hd;});{ void* _temp1454=(
void*)({ struct Cyc_Core_Opt* _temp1453= arg->topt; if( _temp1453 == 0){ _throw(
Null_Exception);} _temp1453->v;}); struct Cyc_Absyn_Exp* _temp1462; struct Cyc_Absyn_Tqual*
_temp1464; void* _temp1466; struct Cyc_Absyn_PtrInfo _temp1468; struct Cyc_Absyn_Conref*
_temp1470; struct Cyc_Absyn_Tqual* _temp1472; struct Cyc_Absyn_Conref* _temp1474;
void* _temp1476; void* _temp1478; _LL1456: if(( unsigned int) _temp1454 > 4u?((
struct _tunion_struct*) _temp1454)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL1467:
_temp1466=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1454)->f1; goto
_LL1465; _LL1465: _temp1464=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1454)->f2; goto _LL1463; _LL1463: _temp1462=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1454)->f3; goto _LL1457;} else{ goto
_LL1458;} _LL1458: if(( unsigned int) _temp1454 > 4u?(( struct _tunion_struct*)
_temp1454)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1469: _temp1468=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp1454)->f1;
_LL1479: _temp1478=( void*) _temp1468.elt_typ; goto _LL1477; _LL1477: _temp1476=(
void*) _temp1468.rgn_typ; goto _LL1475; _LL1475: _temp1474=( struct Cyc_Absyn_Conref*)
_temp1468.nullable; goto _LL1473; _LL1473: _temp1472=( struct Cyc_Absyn_Tqual*)
_temp1468.tq; goto _LL1471; _LL1471: _temp1470=( struct Cyc_Absyn_Conref*)
_temp1468.bounds; goto _LL1459;} else{ goto _LL1460;} _LL1460: goto _LL1461;
_LL1457:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Exp* _temp1480=
_temp1462; if( _temp1480 == 0){ _throw( Null_Exception);} _temp1480->r;})));
goto _LL1455; _LL1459: { struct _tuple1* temp= Cyc_Toc_temp_var(); void*
temp_typ= Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Core_Opt* _temp1481= arg->topt;
if( _temp1481 == 0){ _throw( Null_Exception);} _temp1481->v;})); struct Cyc_Absyn_Exp*
e2= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_last_plus_one_sp,
0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
temp, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Exp* e4= Cyc_Absyn_cast_exp( Cyc_Absyn_uint_t,
Cyc_Absyn_subtract_exp( e2, e3, 0), 0); struct Cyc_Absyn_Stmt* result= Cyc_Absyn_exp_stmt(
e4, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
temp, temp_typ,( struct Cyc_Absyn_Exp*) arg, result, 0), 0))->r)); goto _LL1455;}
_LL1461:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct
_tagged_string _temp1484= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_List_List*
_temp1482= typs; if( _temp1482 == 0){ _throw( Null_Exception);} _temp1482->hd;}));
struct _tagged_string _temp1485= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp1483= arg->topt; if( _temp1483 == 0){ _throw( Null_Exception);} _temp1483->v;}));
xprintf("size primop applied to non-array %.*s (%.*s)", _temp1484.last_plus_one
- _temp1484.curr, _temp1484.curr, _temp1485.last_plus_one - _temp1485.curr,
_temp1485.curr);})); return; _LL1455:;} goto _LL1424;} _LL1428:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_printf_to_c( _temp1242, 0, _temp1240, typs))->r)); goto
_LL1424; _LL1430:( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp1242, 0, _temp1240, typs))->r)); goto _LL1424; _LL1432: if( _temp1240 == 0){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
char* _temp1487=( char*)"fprintf without arguments"; struct _tagged_string
_temp1488; _temp1488.curr= _temp1487; _temp1488.base= _temp1487; _temp1488.last_plus_one=
_temp1487 + 26; _temp1488;}));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp1242,({ struct Cyc_Core_Opt* _temp1489=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1489->v=( void*)(( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1490= _temp1240; if( _temp1490 == 0){ _throw(
Null_Exception);} _temp1490->hd;})); _temp1489;}),({ struct Cyc_List_List*
_temp1491= _temp1240; if( _temp1491 == 0){ _throw( Null_Exception);} _temp1491->tl;}),({
struct Cyc_List_List* _temp1492= typs; if( _temp1492 == 0){ _throw(
Null_Exception);} _temp1492->tl;})))->r)); goto _LL1424; _LL1434: if( _temp1240
== 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp1493=( char*)"sscanf without arguments"; struct
_tagged_string _temp1494; _temp1494.curr= _temp1493; _temp1494.base= _temp1493;
_temp1494.last_plus_one= _temp1493 + 25; _temp1494;}));}( void*)((( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1495= _temp1240; if( _temp1495 == 0){ _throw(
Null_Exception);} _temp1495->hd;}))->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*)(( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1496= _temp1240; if( _temp1496 == 0){ _throw( Null_Exception);} _temp1496->hd;}))->r,((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1497= _temp1240; if(
_temp1497 == 0){ _throw( Null_Exception);} _temp1497->hd;}))->loc), Cyc_Toc_curr_sp,
0))->r)); goto _LL1424; _LL1436: if( Cyc_Tcutil_is_tagged_pointer_typ( old_typ)){
struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1501= _temp1240; if( _temp1501 == 0){ _throw( Null_Exception);} _temp1501->hd;});
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1500=({ struct Cyc_List_List* _temp1499= _temp1240; if( _temp1499 == 0){
_throw( Null_Exception);} _temp1499->tl;}); if( _temp1500 == 0){ _throw(
Null_Exception);} _temp1500->hd;}); struct _tuple1* temp= Cyc_Toc_temp_var();
void* temp_typ= Cyc_Toc_typ_to_c( old_typ); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp( temp, 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Exp* e4= Cyc_Absyn_assignop_exp(
e3,({ struct Cyc_Core_Opt* _temp1498=( struct Cyc_Core_Opt*) GC_malloc( sizeof(
struct Cyc_Core_Opt)); _temp1498->v=( void*)(( void*) Cyc_Absyn_Plus); _temp1498;}),
e2, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( e4, 0), s, 0); s= Cyc_Absyn_declare_stmt(
temp, temp_typ,( struct Cyc_Absyn_Exp*) e1, s, 0);( void*)( e->r=( void*)(( void*)(
Cyc_Absyn_stmt_exp( s, 0))->r));} goto _LL1424; _LL1438: if( Cyc_Tcutil_is_tagged_pointer_typ((
void*)({ struct Cyc_List_List* _temp1502= old_typs; if( _temp1502 == 0){ _throw(
Null_Exception);} _temp1502->hd;}))){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1509= _temp1240; if( _temp1509 == 0){ _throw(
Null_Exception);} _temp1509->hd;}); struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1508=({ struct Cyc_List_List* _temp1507= _temp1240;
if( _temp1507 == 0){ _throw( Null_Exception);} _temp1507->tl;}); if( _temp1508
== 0){ _throw( Null_Exception);} _temp1508->hd;}); void* old_typ_e2=( void*)({
struct Cyc_List_List* _temp1506=({ struct Cyc_List_List* _temp1505= old_typs;
if( _temp1505 == 0){ _throw( Null_Exception);} _temp1505->tl;}); if( _temp1506
== 0){ _throw( Null_Exception);} _temp1506->hd;}); if( Cyc_Tcutil_is_tagged_pointer_typ(
old_typ_e2)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));( void*)( e2->r=( void*)(( void*)(
Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) e2->r, 0), Cyc_Toc_curr_sp,
0))->r));} else{ struct _tuple1* temp= Cyc_Toc_temp_var(); void* temp_typ= Cyc_Toc_typ_to_c((
void*)({ struct Cyc_List_List* _temp1504= old_typs; if( _temp1504 == 0){ _throw(
Null_Exception);} _temp1504->hd;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp( temp, 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Exp* e4= Cyc_Absyn_assignop_exp(
e3,({ struct Cyc_Core_Opt* _temp1503=( struct Cyc_Core_Opt*) GC_malloc( sizeof(
struct Cyc_Core_Opt)); _temp1503->v=( void*)(( void*) Cyc_Absyn_Minus);
_temp1503;}), e2, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( e4, 0), s, 0);
s= Cyc_Absyn_declare_stmt( temp, temp_typ,( struct Cyc_Absyn_Exp*) e1, s, 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r));}} goto _LL1424;
_LL1440: goto _LL1442; _LL1442: goto _LL1444; _LL1444: goto _LL1446; _LL1446:
goto _LL1448; _LL1448: goto _LL1450; _LL1450: { struct Cyc_Absyn_Exp* e1=(
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1515= _temp1240; if(
_temp1515 == 0){ _throw( Null_Exception);} _temp1515->hd;}); struct Cyc_Absyn_Exp*
e2=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1514=({ struct Cyc_List_List*
_temp1513= _temp1240; if( _temp1513 == 0){ _throw( Null_Exception);} _temp1513->tl;});
if( _temp1514 == 0){ _throw( Null_Exception);} _temp1514->hd;}); void* t1=( void*)({
struct Cyc_List_List* _temp1512= old_typs; if( _temp1512 == 0){ _throw(
Null_Exception);} _temp1512->hd;}); void* t2=( void*)({ struct Cyc_List_List*
_temp1511=({ struct Cyc_List_List* _temp1510= old_typs; if( _temp1510 == 0){
_throw( Null_Exception);} _temp1510->tl;}); if( _temp1511 == 0){ _throw(
Null_Exception);} _temp1511->hd;}); if( Cyc_Tcutil_is_tagged_pointer_typ( t1)){(
void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));} goto _LL1424;} _LL1452: goto
_LL1424; _LL1424:;} goto _LL1155;} _LL1167: { void* e2_cyc_typ=( void*)({ struct
Cyc_Core_Opt* _temp1516= _temp1246->topt; if( _temp1516 == 0){ _throw(
Null_Exception);} _temp1516->v;}); Cyc_Toc_exp_to_c( nv, _temp1246); if( Cyc_Tcutil_is_tagged_pointer_typ(
e2_cyc_typ)){ if( _temp1244 ==( void*) Cyc_Absyn_PreInc? 1: _temp1244 ==( void*)
Cyc_Absyn_PreDec){ struct _tuple1* temp= Cyc_Toc_temp_var(); void* temp_typ= Cyc_Absyn_star_typ(
Cyc_Toc_typ_to_c( e2_cyc_typ),( void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual());
struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp(
temp, 0), 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_increment_exp( Cyc_Absyn_structarrow_exp(
Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0), _temp1244, 0); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( e3, 0), s, 0); s= Cyc_Absyn_declare_stmt( temp, temp_typ,(
struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( _temp1246, 0), s, 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r));} else{ struct _tuple1* temp=
Cyc_Toc_temp_var(); struct _tuple1* temp2= Cyc_Toc_temp_var(); void* temp2_typ=
Cyc_Toc_typ_to_c( e2_cyc_typ); void* temp_typ= Cyc_Absyn_star_typ( Cyc_Toc_typ_to_c(
e2_cyc_typ),( void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual()); struct Cyc_Absyn_Stmt*
s= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( temp2, 0), 0); struct Cyc_Absyn_Exp*
e3= Cyc_Absyn_increment_exp( Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( temp,
0), Cyc_Toc_curr_sp, 0), _temp1244, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
e3, 0), s, 0); s= Cyc_Absyn_declare_stmt( temp2, temp2_typ,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( temp, 0), 0), s, 0); s= Cyc_Absyn_declare_stmt(
temp, temp_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( _temp1246, 0), s,
0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r));}} else{
if( ! Cyc_Absyn_is_lvalue( _temp1246)){(( void(*)( struct Cyc_Absyn_Exp* e1,
struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1246, 0, Cyc_Toc_incr_lvalue,
_temp1244);( void*)( e->r=( void*)(( void*) _temp1246->r));}} goto _LL1155;}
_LL1169: { int e1_poly= Cyc_Toc_is_poly_project( _temp1252); void* e1_old_typ=(
void*)({ struct Cyc_Core_Opt* _temp1522= _temp1252->topt; if( _temp1522 == 0){
_throw( Null_Exception);} _temp1522->v;}); void* e2_old_typ=( void*)({ struct
Cyc_Core_Opt* _temp1521= _temp1248->topt; if( _temp1521 == 0){ _throw(
Null_Exception);} _temp1521->v;}); Cyc_Toc_exp_to_c( nv, _temp1252); Cyc_Toc_exp_to_c(
nv, _temp1248); if( _temp1250 != 0?( void*)({ struct Cyc_Core_Opt* _temp1517=
_temp1250; if( _temp1517 == 0){ _throw( Null_Exception);} _temp1517->v;}) ==(
void*) Cyc_Absyn_Plus? 1:( void*)({ struct Cyc_Core_Opt* _temp1518= _temp1250;
if( _temp1518 == 0){ _throw( Null_Exception);} _temp1518->v;}) ==( void*) Cyc_Absyn_Minus:
0){ if( Cyc_Tcutil_is_tagged_pointer_typ( e1_old_typ)){( void*)( _temp1252->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*)
_temp1252->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
e2_old_typ)){( void*)( _temp1248->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*) _temp1248->r, 0), Cyc_Toc_curr_sp, 0))->r));}} if(
e1_poly){( void*)( _temp1248->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1248->r, 0), 0))->r));} if( ! Cyc_Absyn_is_lvalue(
_temp1252)){(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, struct _tuple11*), struct
_tuple11* f_env)) Cyc_Toc_lvalue_assign)( _temp1252, 0, Cyc_Toc_assignop_lvalue,({
struct _tuple11* _temp1519=( struct _tuple11*) GC_malloc( sizeof( struct
_tuple11) * 1); _temp1519[ 0]=({ struct _tuple11 _temp1520; _temp1520.f1=
_temp1250; _temp1520.f2= _temp1248; _temp1520;}); _temp1519;}));( void*)( e->r=(
void*)(( void*) _temp1252->r));} goto _LL1155;} _LL1171: Cyc_Toc_exp_to_c( nv,
_temp1258); Cyc_Toc_exp_to_c( nv, _temp1256); Cyc_Toc_exp_to_c( nv, _temp1254);
goto _LL1155; _LL1173: Cyc_Toc_exp_to_c( nv, _temp1262); Cyc_Toc_exp_to_c( nv,
_temp1260); goto _LL1155; _LL1175: _temp1270= _temp1266; _temp1268= _temp1264;
goto _LL1177; _LL1177: Cyc_Toc_exp_to_c( nv, _temp1270);(( void(*)( void(* f)(
struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List*
x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp1268); goto _LL1155; _LL1179:
Cyc_Toc_exp_to_c( nv, _temp1272);( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c( old_typ), Cyc_Toc_newthrow_exp( _temp1272), 0))->r)); goto
_LL1155; _LL1181: Cyc_Toc_exp_to_c( nv, _temp1274); goto _LL1155; _LL1183: Cyc_Toc_exp_to_c(
nv, _temp1278);{ void* t_inst= Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Core_Opt*
_temp1525= e->topt; if( _temp1525 == 0){ _throw( Null_Exception);} _temp1525->v;}));
for( 0; _temp1276 != 0; _temp1276=({ struct Cyc_List_List* _temp1523= _temp1276;
if( _temp1523 == 0){ _throw( Null_Exception);} _temp1523->tl;})){ void* k= Cyc_Tcutil_typ_kind((
void*)({ struct Cyc_List_List* _temp1524= _temp1276; if( _temp1524 == 0){ _throw(
Null_Exception);} _temp1524->hd;})); if( k !=( void*) Cyc_Absyn_EffKind? k !=(
void*) Cyc_Absyn_RgnKind: 0){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
t_inst, _temp1278, 0))->r)); break;}} goto _LL1155;} _LL1185: { void* old_t2=(
void*)({ struct Cyc_Core_Opt* _temp1526=(* _temp1282)->topt; if( _temp1526 == 0){
_throw( Null_Exception);} _temp1526->v;}); void* new_typ=* _temp1285;* _temp1285=
Cyc_Toc_typ_to_c( new_typ); Cyc_Toc_exp_to_c( nv,* _temp1282);* _temp1282= Cyc_Toc_convert_exp(
nv->toplevel, old_t2, new_typ,* _temp1282); goto _LL1155;} _LL1187:{ void*
_temp1527=( void*) _temp1286->r; struct Cyc_Absyn_Structdecl* _temp1535; struct
Cyc_List_List* _temp1537; struct Cyc_Core_Opt* _temp1539; struct _tuple1*
_temp1541; struct Cyc_List_List* _temp1543; _LL1529: if((( struct _tunion_struct*)
_temp1527)->tag == Cyc_Absyn_Struct_e_tag){ _LL1542: _temp1541=( struct _tuple1*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1527)->f1; goto _LL1540; _LL1540:
_temp1539=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*) _temp1527)->f2;
goto _LL1538; _LL1538: _temp1537=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1527)->f3; goto _LL1536; _LL1536: _temp1535=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1527)->f4; goto _LL1530;} else{ goto
_LL1531;} _LL1531: if((( struct _tunion_struct*) _temp1527)->tag == Cyc_Absyn_Tuple_e_tag){
_LL1544: _temp1543=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1527)->f1; goto _LL1532;} else{ goto _LL1533;} _LL1533: goto _LL1534;
_LL1530: if( nv->toplevel){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({
struct _tagged_string _temp1545= Cyc_Position_string_of_segment( _temp1286->loc);
xprintf("%.*s: & on non-identifiers at the top-level", _temp1545.last_plus_one -
_temp1545.curr, _temp1545.curr);}));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_struct(
nv,( void*)({ struct Cyc_Core_Opt* _temp1546= _temp1286->topt; if( _temp1546 ==
0){ _throw( Null_Exception);} _temp1546->v;}), 1, 0, _temp1537, _temp1541))->r));
goto _LL1528; _LL1532: if( nv->toplevel){(( void(*)( struct _tagged_string s))
Cyc_Toc_unimp)(({ struct _tagged_string _temp1547= Cyc_Position_string_of_segment(
_temp1286->loc); xprintf("%.*s: & on non-identifiers at the top-level",
_temp1547.last_plus_one - _temp1547.curr, _temp1547.curr);}));}( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_tuple( nv, 1, 0, _temp1543))->r)); goto _LL1528;
_LL1534: Cyc_Toc_exp_to_c( nv, _temp1286); if( ! Cyc_Absyn_is_lvalue( _temp1286)){((
void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(*
f)( struct Cyc_Absyn_Exp*, int), int f_env)) Cyc_Toc_lvalue_assign)( _temp1286,
0, Cyc_Toc_address_lvalue, 1);( void*)( e->r=( void*)(( void*) _temp1286->r));}
goto _LL1528; _LL1528:;} goto _LL1155; _LL1189: if( nv->toplevel){(( void(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(({ struct _tagged_string _temp1548= Cyc_Position_string_of_segment(
_temp1288->loc); xprintf("%.*s: new at top-level", _temp1548.last_plus_one -
_temp1548.curr, _temp1548.curr);}));}{ void* _temp1549=( void*) _temp1288->r;
struct Cyc_List_List* _temp1563; void* _temp1565; struct _tagged_string
_temp1567; struct Cyc_Absyn_Exp* _temp1569; struct Cyc_Absyn_Exp* _temp1571;
struct Cyc_Absyn_Vardecl* _temp1573; struct Cyc_Absyn_Structdecl* _temp1575;
struct Cyc_List_List* _temp1577; struct Cyc_Core_Opt* _temp1579; struct _tuple1*
_temp1581; struct Cyc_List_List* _temp1583; _LL1551: if((( struct _tunion_struct*)
_temp1549)->tag == Cyc_Absyn_Array_e_tag){ _LL1564: _temp1563=( struct Cyc_List_List*)((
struct Cyc_Absyn_Array_e_struct*) _temp1549)->f1; goto _LL1552;} else{ goto
_LL1553;} _LL1553: if((( struct _tunion_struct*) _temp1549)->tag == Cyc_Absyn_Const_e_tag){
_LL1566: _temp1565=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1549)->f1;
if(( unsigned int) _temp1565 > 1u?(( struct _tunion_struct*) _temp1565)->tag ==
Cyc_Absyn_String_c_tag: 0){ _LL1568: _temp1567=( struct _tagged_string)(( struct
Cyc_Absyn_String_c_struct*) _temp1565)->f1; goto _LL1554;} else{ goto _LL1555;}}
else{ goto _LL1555;} _LL1555: if((( struct _tunion_struct*) _temp1549)->tag ==
Cyc_Absyn_Comprehension_e_tag){ _LL1574: _temp1573=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1549)->f1; goto _LL1572; _LL1572:
_temp1571=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1549)->f2; goto _LL1570; _LL1570: _temp1569=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1549)->f3; goto _LL1556;} else{
goto _LL1557;} _LL1557: if((( struct _tunion_struct*) _temp1549)->tag == Cyc_Absyn_Struct_e_tag){
_LL1582: _temp1581=( struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1549)->f1; goto _LL1580; _LL1580: _temp1579=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1549)->f2; goto _LL1578; _LL1578:
_temp1577=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1549)->f3; goto _LL1576; _LL1576: _temp1575=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1549)->f4; goto _LL1558;} else{ goto
_LL1559;} _LL1559: if((( struct _tunion_struct*) _temp1549)->tag == Cyc_Absyn_Tuple_e_tag){
_LL1584: _temp1583=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1549)->f1; goto _LL1560;} else{ goto _LL1561;} _LL1561: goto _LL1562;
_LL1552: { struct _tuple1* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* xexp=
Cyc_Absyn_var_exp( x, 0); struct Cyc_Absyn_Stmt* s= Cyc_Toc_init_array( nv, xexp,
_temp1563, Cyc_Absyn_exp_stmt( xexp, 0)); void* old_elt_typ;{ void* _temp1585=
Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo _temp1591; struct Cyc_Absyn_Conref*
_temp1593; struct Cyc_Absyn_Tqual* _temp1595; struct Cyc_Absyn_Conref* _temp1597;
void* _temp1599; void* _temp1601; _LL1587: if(( unsigned int) _temp1585 > 4u?((
struct _tunion_struct*) _temp1585)->tag == Cyc_Absyn_PointerType_tag: 0){
_LL1592: _temp1591=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1585)->f1; _LL1602: _temp1601=( void*) _temp1591.elt_typ; goto _LL1600;
_LL1600: _temp1599=( void*) _temp1591.rgn_typ; goto _LL1598; _LL1598: _temp1597=(
struct Cyc_Absyn_Conref*) _temp1591.nullable; goto _LL1596; _LL1596: _temp1595=(
struct Cyc_Absyn_Tqual*) _temp1591.tq; goto _LL1594; _LL1594: _temp1593=( struct
Cyc_Absyn_Conref*) _temp1591.bounds; goto _LL1588;} else{ goto _LL1589;} _LL1589:
goto _LL1590; _LL1588: old_elt_typ= _temp1601; goto _LL1586; _LL1590:
old_elt_typ=(( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp1603=( char*)"exp_to_c:new array expression doesn't have ptr type";
struct _tagged_string _temp1604; _temp1604.curr= _temp1603; _temp1604.base=
_temp1603; _temp1604.last_plus_one= _temp1603 + 52; _temp1604;})); goto _LL1586;
_LL1586:;}{ void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp(
elt_typ, 0), Cyc_Absyn_signed_int_exp((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp1563), 0), 0); struct Cyc_Absyn_Exp* e1; if( _temp1290 == 0){ e1= Cyc_Toc_malloc_exp(
old_elt_typ, e2);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1605= _temp1290; if( _temp1605 == 0){ _throw(
Null_Exception);} _temp1605;}); Cyc_Toc_exp_to_c( nv, r); e1= Cyc_Toc_rmalloc_exp(
r, e2);}{ struct Cyc_Absyn_Exp* e0= Cyc_Absyn_cast_exp( ptr_typ, e1, 0);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, ptr_typ,(
struct Cyc_Absyn_Exp*) e0, s, 0), 0))->r)); goto _LL1550;}}} _LL1554: if(
_temp1290 == 0){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(({
struct _tuple1* _temp1606=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp1606->f1= Cyc_Toc_abs_ns; _temp1606->f2= Cyc_Toc_Cyc_new_string_sp;
_temp1606;}), 0),({ struct Cyc_List_List* _temp1607=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1607->hd=( void*) Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp1608=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp1608[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp1609; _temp1609.tag= Cyc_Absyn_Const_e_tag;
_temp1609.f1=( void*)(( void*)({ struct Cyc_Absyn_String_c_struct* _temp1610=(
struct Cyc_Absyn_String_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_String_c_struct));
_temp1610[ 0]=({ struct Cyc_Absyn_String_c_struct _temp1611; _temp1611.tag= Cyc_Absyn_String_c_tag;
_temp1611.f1= _temp1567; _temp1611;}); _temp1610;})); _temp1609;}); _temp1608;}),
0); _temp1607->tl= 0; _temp1607;}), 0))->r));} else{ struct Cyc_Absyn_Exp* r=(
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1618= _temp1290; if(
_temp1618 == 0){ _throw( Null_Exception);} _temp1618;}); Cyc_Toc_exp_to_c( nv, r);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(({
struct _tuple1* _temp1612=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp1612->f1= Cyc_Toc_abs_ns; _temp1612->f2= Cyc_Toc_Cyc_rnew_string_sp;
_temp1612;}), 0),({ struct Cyc_List_List* _temp1613=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1613->hd=( void*) Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp1614=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp1614[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp1615; _temp1615.tag= Cyc_Absyn_Const_e_tag;
_temp1615.f1=( void*)(( void*)({ struct Cyc_Absyn_String_c_struct* _temp1616=(
struct Cyc_Absyn_String_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_String_c_struct));
_temp1616[ 0]=({ struct Cyc_Absyn_String_c_struct _temp1617; _temp1617.tag= Cyc_Absyn_String_c_tag;
_temp1617.f1= _temp1567; _temp1617;}); _temp1616;})); _temp1615;}); _temp1614;}),
0); _temp1613->tl= 0; _temp1613;}), 0))->r));} goto _LL1550; _LL1556: { int
is_tagged_ptr= 0;{ void* _temp1619= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1625; struct Cyc_Absyn_Conref* _temp1627; struct Cyc_Absyn_Tqual* _temp1629;
struct Cyc_Absyn_Conref* _temp1631; void* _temp1633; void* _temp1635; _LL1621:
if(( unsigned int) _temp1619 > 4u?(( struct _tunion_struct*) _temp1619)->tag ==
Cyc_Absyn_PointerType_tag: 0){ _LL1626: _temp1625=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1619)->f1; _LL1636: _temp1635=( void*)
_temp1625.elt_typ; goto _LL1634; _LL1634: _temp1633=( void*) _temp1625.rgn_typ;
goto _LL1632; _LL1632: _temp1631=( struct Cyc_Absyn_Conref*) _temp1625.nullable;
goto _LL1630; _LL1630: _temp1629=( struct Cyc_Absyn_Tqual*) _temp1625.tq; goto
_LL1628; _LL1628: _temp1627=( struct Cyc_Absyn_Conref*) _temp1625.bounds; goto
_LL1622;} else{ goto _LL1623;} _LL1623: goto _LL1624; _LL1622:{ void* _temp1637=(
void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp1627))->v; void* _temp1643; _LL1639: if(( unsigned int) _temp1637 > 1u?((
struct _tunion_struct*) _temp1637)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL1644:
_temp1643=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1637)->f1; if((
int) _temp1643 == Cyc_Absyn_Unknown_b){ goto _LL1640;} else{ goto _LL1641;}}
else{ goto _LL1641;} _LL1641: goto _LL1642; _LL1640: is_tagged_ptr= 1; goto
_LL1638; _LL1642: goto _LL1638; _LL1638:;} goto _LL1620; _LL1624:(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp1645=( char*)"exp_to_c: comprehension not an array type"; struct
_tagged_string _temp1646; _temp1646.curr= _temp1645; _temp1646.base= _temp1645;
_temp1646.last_plus_one= _temp1645 + 42; _temp1646;})); goto _LL1620; _LL1620:;}{
struct _tuple1* max= Cyc_Toc_temp_var(); struct _tuple1* a= Cyc_Toc_temp_var();
void* old_elt_typ=( void*)({ struct Cyc_Core_Opt* _temp1670= _temp1569->topt;
if( _temp1670 == 0){ _throw( Null_Exception);} _temp1670->v;}); void* elt_typ=
Cyc_Toc_typ_to_c( old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq);
Cyc_Toc_exp_to_c( nv, _temp1571);{ struct Cyc_Absyn_Stmt* s= Cyc_Toc_init_comprehension(
nv, Cyc_Absyn_var_exp( a, 0), _temp1573, Cyc_Absyn_var_exp( max, 0), _temp1569,
Cyc_Absyn_skip_stmt( 0), 1); struct Cyc_List_List* decls=({ struct Cyc_List_List*
_temp1668=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1668->hd=( void*)({ struct _tuple10* _temp1669=( struct _tuple10*)
GC_malloc( sizeof( struct _tuple10)); _temp1669->f1= max; _temp1669->f2= Cyc_Absyn_uint_t;
_temp1669->f3=( struct Cyc_Absyn_Exp*) _temp1571; _temp1669;}); _temp1668->tl= 0;
_temp1668;}); struct Cyc_Absyn_Exp* ai; if( _temp1290 == 0){ ai= Cyc_Toc_malloc_exp(
old_elt_typ, Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_var_exp(
max, 0), 0));} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)({ struct
Cyc_Absyn_Exp* _temp1647= _temp1290; if( _temp1647 == 0){ _throw( Null_Exception);}
_temp1647;}); Cyc_Toc_exp_to_c( nv, r); ai= Cyc_Toc_rmalloc_exp( r, Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_var_exp( max, 0), 0));}{ struct
Cyc_Absyn_Exp* ainit= Cyc_Absyn_cast_exp( ptr_typ, ai, 0); decls=({ struct Cyc_List_List*
_temp1648=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1648->hd=( void*)({ struct _tuple10* _temp1649=( struct _tuple10*)
GC_malloc( sizeof( struct _tuple10)); _temp1649->f1= a; _temp1649->f2= ptr_typ;
_temp1649->f3=( struct Cyc_Absyn_Exp*) ainit; _temp1649;}); _temp1648->tl= decls;
_temp1648;}); if( is_tagged_ptr){ struct _tuple1* b= Cyc_Toc_temp_var(); void* t=
Cyc_Toc_typ_to_c( old_typ); struct Cyc_Absyn_Exp* binit= Cyc_Absyn_unresolvedmem_exp(
0,({ struct Cyc_List_List* _temp1652=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1652->hd=( void*)({ struct _tuple7* _temp1657=(
struct _tuple7*) GC_malloc( sizeof( struct _tuple7)); _temp1657->f1= 0;
_temp1657->f2= Cyc_Absyn_var_exp( a, 0); _temp1657;}); _temp1652->tl=({ struct
Cyc_List_List* _temp1653=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1653->hd=( void*)({ struct _tuple7* _temp1656=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp1656->f1= 0; _temp1656->f2= Cyc_Absyn_var_exp( a,
0); _temp1656;}); _temp1653->tl=({ struct Cyc_List_List* _temp1654=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1654->hd=( void*)({ struct
_tuple7* _temp1655=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7));
_temp1655->f1= 0; _temp1655->f2= Cyc_Absyn_add_exp( Cyc_Absyn_var_exp( a, 0),
Cyc_Absyn_var_exp( max, 0), 0); _temp1655;}); _temp1654->tl= 0; _temp1654;});
_temp1653;}); _temp1652;}), 0); decls=({ struct Cyc_List_List* _temp1650=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1650->hd=(
void*)({ struct _tuple10* _temp1651=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp1651->f1= b; _temp1651->f2= t; _temp1651->f3=( struct
Cyc_Absyn_Exp*) binit; _temp1651;}); _temp1650->tl= decls; _temp1650;}); s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( b, 0), 0), 0);} else{ s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( a, 0), 0), 0);}{ struct Cyc_List_List*
ds= decls; for( 0; ds != 0; ds=({ struct Cyc_List_List* _temp1658= ds; if(
_temp1658 == 0){ _throw( Null_Exception);} _temp1658->tl;})){ struct Cyc_Absyn_Exp*
_temp1662; void* _temp1664; struct _tuple1* _temp1666; struct _tuple10 _temp1660=*((
struct _tuple10*)({ struct Cyc_List_List* _temp1659= ds; if( _temp1659 == 0){
_throw( Null_Exception);} _temp1659->hd;})); _LL1667: _temp1666= _temp1660.f1;
goto _LL1665; _LL1665: _temp1664= _temp1660.f2; goto _LL1663; _LL1663: _temp1662=
_temp1660.f3; goto _LL1661; _LL1661: s= Cyc_Absyn_declare_stmt( _temp1666,
_temp1664, _temp1662, s, 0);}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
s, 0))->r)); goto _LL1550;}}}} _LL1558:( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_struct(
nv,( void*)({ struct Cyc_Core_Opt* _temp1671= _temp1288->topt; if( _temp1671 ==
0){ _throw( Null_Exception);} _temp1671->v;}), 1, _temp1290, _temp1577,
_temp1581))->r)); goto _LL1550; _LL1560:( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple(
nv, 1, _temp1290, _temp1583))->r)); goto _LL1550; _LL1562: { void* old_elt_typ=(
void*)({ struct Cyc_Core_Opt* _temp1673= _temp1288->topt; if( _temp1673 == 0){
_throw( Null_Exception);} _temp1673->v;}); struct _tuple1* x= Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( x, 0), 0);
struct Cyc_Absyn_Exp* e_index= Cyc_Absyn_signed_int_exp( 0, 0); Cyc_Toc_exp_to_c(
nv, _temp1288); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
Cyc_Absyn_var_exp( x, 0), e_index, 0), _temp1288, 0), s, 0);{ void* elt_typ= Cyc_Toc_typ_to_c(
old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp* e2= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0); struct Cyc_Absyn_Exp*
e1; if( _temp1290 == 0){ e1= Cyc_Toc_malloc_exp( old_elt_typ, e2);} else{ struct
Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1672=
_temp1290; if( _temp1672 == 0){ _throw( Null_Exception);} _temp1672;}); Cyc_Toc_exp_to_c(
nv, r); e1= Cyc_Toc_rmalloc_exp( r, e2);}{ struct Cyc_Absyn_Exp* e0= Cyc_Absyn_cast_exp(
ptr_typ, e1, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
x, ptr_typ,( struct Cyc_Absyn_Exp*) e0, s, 0), 0))->r)); goto _LL1550;}}}
_LL1550:;} goto _LL1155; _LL1191:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Sizeoftyp_e_struct*
_temp1674=( struct Cyc_Absyn_Sizeoftyp_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Sizeoftyp_e_struct));
_temp1674[ 0]=({ struct Cyc_Absyn_Sizeoftyp_e_struct _temp1675; _temp1675.tag=
Cyc_Absyn_Sizeoftyp_e_tag; _temp1675.f1=( void*) Cyc_Toc_typ_to_c_array(
_temp1292); _temp1675;}); _temp1674;}))); goto _LL1155; _LL1193: Cyc_Toc_exp_to_c(
nv, _temp1294); goto _LL1155; _LL1195: { void* e1_typ= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp1705= _temp1296->topt; if( _temp1705 == 0){
_throw( Null_Exception);} _temp1705->v;})); Cyc_Toc_exp_to_c( nv, _temp1296);{
void* _temp1676= e1_typ; struct Cyc_Absyn_PtrInfo _temp1682; struct Cyc_Absyn_Conref*
_temp1684; struct Cyc_Absyn_Tqual* _temp1686; struct Cyc_Absyn_Conref* _temp1688;
void* _temp1690; void* _temp1692; _LL1678: if(( unsigned int) _temp1676 > 4u?((
struct _tunion_struct*) _temp1676)->tag == Cyc_Absyn_PointerType_tag: 0){
_LL1683: _temp1682=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1676)->f1; _LL1693: _temp1692=( void*) _temp1682.elt_typ; goto _LL1691;
_LL1691: _temp1690=( void*) _temp1682.rgn_typ; goto _LL1689; _LL1689: _temp1688=(
struct Cyc_Absyn_Conref*) _temp1682.nullable; goto _LL1687; _LL1687: _temp1686=(
struct Cyc_Absyn_Tqual*) _temp1682.tq; goto _LL1685; _LL1685: _temp1684=( struct
Cyc_Absyn_Conref*) _temp1682.bounds; goto _LL1679;} else{ goto _LL1680;} _LL1680:
goto _LL1681; _LL1679:{ void* _temp1694=(( void*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_val)( _temp1684); struct Cyc_Absyn_Exp* _temp1700; _LL1696: if((
int) _temp1694 == Cyc_Absyn_Unknown_b){ goto _LL1697;} else{ goto _LL1698;}
_LL1698: if(( unsigned int) _temp1694 > 1u?(( struct _tunion_struct*) _temp1694)->tag
== Cyc_Absyn_Upper_b_tag: 0){ _LL1701: _temp1700=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Upper_b_struct*) _temp1694)->f1; goto _LL1699;} else{ goto
_LL1695;} _LL1697: { struct _tuple1* x= Cyc_Toc_temp_var(); struct _tuple1* a=
Cyc_Toc_temp_var(); void* ta1= Cyc_Toc_typ_to_c_array( _temp1692); struct
_tagged_string* n= Cyc_Toc_add_tagged_ptr_type( ta1, _temp1686); void* tx= Cyc_Absyn_strct(
n); void* ta2= Cyc_Absyn_cstar_typ( ta1, _temp1686); struct Cyc_Absyn_Exp*
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
Cyc_Absyn_declare_stmt( x, tx,( struct Cyc_Absyn_Exp*) _temp1296, s1, 0), 0))->r));
goto _LL1695;} _LL1699: if( Cyc_Toc_is_nullable( e1_typ)){ struct _tuple1* x=
Cyc_Toc_temp_var(); void* tx= Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Core_Opt*
_temp1702= _temp1296->topt; if( _temp1702 == 0){ _throw( Null_Exception);}
_temp1702->v;})); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp(
Cyc_Absyn_null_pointer_exn_exp( 0)); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_ifthenelse_stmt(
e2, Cyc_Absyn_exp_stmt( e3, 0), Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( x, 0), 0), 0);
struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( s1, s2, 0);( void*)( e->r=( void*)((
void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, tx,( struct Cyc_Absyn_Exp*)
_temp1296, s3, 0), 0))->r));} goto _LL1695; _LL1695:;} goto _LL1677; _LL1681:((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
char* _temp1703=( char*)"exp_to_c: Deref: non-pointer"; struct _tagged_string
_temp1704; _temp1704.curr= _temp1703; _temp1704.base= _temp1703; _temp1704.last_plus_one=
_temp1703 + 29; _temp1704;})); goto _LL1677; _LL1677:;} goto _LL1155;} _LL1197:
Cyc_Toc_exp_to_c( nv, _temp1300); if( Cyc_Toc_is_poly_project( e)){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)({ struct
Cyc_Core_Opt* _temp1706= e->topt; if( _temp1706 == 0){ _throw( Null_Exception);}
_temp1706->v;})), Cyc_Absyn_new_exp(( void*) e->r, 0), 0))->r));} goto _LL1155;
_LL1199: Cyc_Toc_exp_to_c( nv, _temp1304);{ int is_poly= Cyc_Toc_is_poly_project(
e); if( Cyc_Toc_is_nullable(( void*)({ struct Cyc_Core_Opt* _temp1707= _temp1304->topt;
if( _temp1707 == 0){ _throw( Null_Exception);} _temp1707->v;}))){ struct _tuple1*
x= Cyc_Toc_temp_var(); void* tx= Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Core_Opt*
_temp1708= _temp1304->topt; if( _temp1708 == 0){ _throw( Null_Exception);}
_temp1708->v;})); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp(
Cyc_Absyn_null_pointer_exn_exp( 0)); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_ifthenelse_stmt(
e2, Cyc_Absyn_exp_stmt( e3, 0), Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_exp_stmt( Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( x, 0),
_temp1302, 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( s1, s2, 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x,
tx,( struct Cyc_Absyn_Exp*) _temp1304, s3, 0), 0))->r));} if( is_poly){( void*)(
e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)({
struct Cyc_Core_Opt* _temp1709= e->topt; if( _temp1709 == 0){ _throw(
Null_Exception);} _temp1709->v;})), Cyc_Absyn_new_exp(( void*) e->r, 0), 0))->r));}
goto _LL1155;} _LL1201: { void* e1_typ= Cyc_Tcutil_compress(( void*)({ struct
Cyc_Core_Opt* _temp1762= _temp1308->topt; if( _temp1762 == 0){ _throw(
Null_Exception);} _temp1762->v;})); Cyc_Toc_exp_to_c( nv, _temp1308); Cyc_Toc_exp_to_c(
nv, _temp1306);( void*)( _temp1308->r=( void*)(( void*)( Cyc_Toc_array_to_ptr(
Cyc_Toc_typ_to_c( e1_typ), Cyc_Absyn_new_exp(( void*) _temp1308->r, 0), 0))->r));{
void* _temp1710= e1_typ; struct Cyc_List_List* _temp1720; struct Cyc_Absyn_Exp*
_temp1722; struct Cyc_Absyn_Tqual* _temp1724; void* _temp1726; struct Cyc_Absyn_PtrInfo
_temp1728; struct Cyc_Absyn_Conref* _temp1730; struct Cyc_Absyn_Tqual* _temp1732;
struct Cyc_Absyn_Conref* _temp1734; void* _temp1736; void* _temp1738; _LL1712:
if(( unsigned int) _temp1710 > 4u?(( struct _tunion_struct*) _temp1710)->tag ==
Cyc_Absyn_TupleType_tag: 0){ _LL1721: _temp1720=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp1710)->f1; goto _LL1713;} else{ goto
_LL1714;} _LL1714: if(( unsigned int) _temp1710 > 4u?(( struct _tunion_struct*)
_temp1710)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL1727: _temp1726=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1710)->f1; goto _LL1725; _LL1725:
_temp1724=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1710)->f2; goto _LL1723; _LL1723: _temp1722=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1710)->f3; goto _LL1715;} else{ goto
_LL1716;} _LL1716: if(( unsigned int) _temp1710 > 4u?(( struct _tunion_struct*)
_temp1710)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1729: _temp1728=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp1710)->f1;
_LL1739: _temp1738=( void*) _temp1728.elt_typ; goto _LL1737; _LL1737: _temp1736=(
void*) _temp1728.rgn_typ; goto _LL1735; _LL1735: _temp1734=( struct Cyc_Absyn_Conref*)
_temp1728.nullable; goto _LL1733; _LL1733: _temp1732=( struct Cyc_Absyn_Tqual*)
_temp1728.tq; goto _LL1731; _LL1731: _temp1730=( struct Cyc_Absyn_Conref*)
_temp1728.bounds; goto _LL1717;} else{ goto _LL1718;} _LL1718: goto _LL1719;
_LL1713: { unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp1306) + 1;( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( _temp1308,({ struct
_tagged_string* _temp1740=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp1740[ 0]= xprintf("f%d",( int) i); _temp1740;}), 0))->r));
goto _LL1711;} _LL1715: if( _temp1722 == 0){(( void(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1741=( char*)"exp_to_c: Subscript_e array type bound null";
struct _tagged_string _temp1742; _temp1742.curr= _temp1741; _temp1742.base=
_temp1741; _temp1742.last_plus_one= _temp1741 + 44; _temp1742;}));}( void*)( e->r=(
void*)(( void*)( Cyc_Toc_known_size_subscript( _temp1308, _temp1306, 0,( struct
Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1743= _temp1722; if( _temp1743 == 0){
_throw( Null_Exception);} _temp1743;}), _temp1726, _temp1724, 0))->r)); goto
_LL1711; _LL1717:{ void* _temp1744=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1730); struct Cyc_Absyn_Exp* _temp1750; _LL1746: if(( unsigned int)
_temp1744 > 1u?(( struct _tunion_struct*) _temp1744)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL1751: _temp1750=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp1744)->f1; goto _LL1747;} else{ goto _LL1748;} _LL1748: if(( int) _temp1744
== Cyc_Absyn_Unknown_b){ goto _LL1749;} else{ goto _LL1745;} _LL1747: { int
possibly_null= 1;{ void* _temp1752=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp1734))->v; int
_temp1758; _LL1754: if(( unsigned int) _temp1752 > 1u?(( struct _tunion_struct*)
_temp1752)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL1759: _temp1758=( int)((
struct Cyc_Absyn_Eq_constr_struct*) _temp1752)->f1; goto _LL1755;} else{ goto
_LL1756;} _LL1756: goto _LL1757; _LL1755: possibly_null= _temp1758; goto _LL1753;
_LL1757: possibly_null= 0; goto _LL1753; _LL1753:;}( void*)( e->r=( void*)((
void*)( Cyc_Toc_known_size_subscript( _temp1308, _temp1306, possibly_null,
_temp1750, _temp1738, _temp1732, 0))->r)); goto _LL1745;} _LL1749: { struct
_tuple1* x= Cyc_Toc_temp_var(); struct _tuple1* i= Cyc_Toc_temp_var(); struct
_tuple1* a= Cyc_Toc_temp_var(); void* ta1= Cyc_Toc_typ_to_c_array( _temp1738);
struct _tagged_string* n= Cyc_Toc_add_tagged_ptr_type( ta1, _temp1732); void* tx=
Cyc_Absyn_strct( n); void* ta2= Cyc_Absyn_cstar_typ( ta1, _temp1732); struct Cyc_Absyn_Exp*
test_exp= Cyc_Absyn_or_exp( Cyc_Absyn_eq_exp( Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Toc_base_sp, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_or_exp(
Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Toc_base_sp, 0), 0), Cyc_Absyn_gte_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_last_plus_one_sp, 0), 0), 0), 0); struct Cyc_Absyn_Exp*
a_init= Cyc_Absyn_add_exp( Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0),
Cyc_Toc_curr_sp, 0), _temp1306, 0); struct Cyc_Absyn_Stmt* s5= Cyc_Absyn_exp_stmt(
Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( a, 0), 0), 0); struct Cyc_Absyn_Stmt* s4=
Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp( 0)), 0);
struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_ifthenelse_stmt( test_exp, s4, Cyc_Absyn_skip_stmt(
0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s3, s5, 0); struct Cyc_Absyn_Stmt*
s1= Cyc_Absyn_declare_stmt( a, ta2,( struct Cyc_Absyn_Exp*) a_init, s2, 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x,
tx,( struct Cyc_Absyn_Exp*) _temp1308, s1, 0), 0))->r)); goto _LL1745;} _LL1745:;}
goto _LL1711; _LL1719:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1760=( char*)"exp_to_c: Subscript on non-tuple/array/tuple ptr";
struct _tagged_string _temp1761; _temp1761.curr= _temp1760; _temp1761.base=
_temp1760; _temp1761.last_plus_one= _temp1760 + 49; _temp1761;})); goto _LL1711;
_LL1711:;} goto _LL1155;} _LL1203: if( ! nv->toplevel){( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_tuple( nv, 0, 0, _temp1310))->r));} else{ struct Cyc_List_List*
tqs=(( struct Cyc_List_List*(*)( struct _tuple5*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_tup_to_c, _temp1310); struct
_tagged_string* n= Cyc_Toc_add_tuple_type( tqs); struct Cyc_List_List* dles= 0;{
int i= 1; for( 0; _temp1310 != 0; _temp1310=({ struct Cyc_List_List* _temp1763=
_temp1310; if( _temp1763 == 0){ _throw( Null_Exception);} _temp1763->tl;}), i ++){
Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1764=
_temp1310; if( _temp1764 == 0){ _throw( Null_Exception);} _temp1764->hd;}));{
struct Cyc_List_List* des=({ struct Cyc_List_List* _temp1768=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1768->hd=( void*)(( void*)({
struct Cyc_Absyn_FieldName_struct* _temp1769=( struct Cyc_Absyn_FieldName_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct)); _temp1769[ 0]=({ struct
Cyc_Absyn_FieldName_struct _temp1770; _temp1770.tag= Cyc_Absyn_FieldName_tag;
_temp1770.f1=({ struct _tagged_string* _temp1771=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp1771[ 0]= xprintf("f%d", i);
_temp1771;}); _temp1770;}); _temp1769;})); _temp1768->tl= 0; _temp1768;}); dles=({
struct Cyc_List_List* _temp1765=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1765->hd=( void*)({ struct _tuple7* _temp1766=(
struct _tuple7*) GC_malloc( sizeof( struct _tuple7)); _temp1766->f1=( struct Cyc_List_List*)
des; _temp1766->f2=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1767=
_temp1310; if( _temp1767 == 0){ _throw( Null_Exception);} _temp1767->hd;});
_temp1766;}); _temp1765->tl= dles; _temp1765;});}}} dles=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles);( void*)( e->r=( void*)((
void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_strct( n), Cyc_Absyn_unresolvedmem_exp( 0,
dles, 0), 0))->r));} goto _LL1155; _LL1205:(( void(*)( struct _tagged_string s))
Cyc_Toc_unimp)(( struct _tagged_string)({ char* _temp1772=( char*)"compoundlit";
struct _tagged_string _temp1773; _temp1773.curr= _temp1772; _temp1773.base=
_temp1772; _temp1773.last_plus_one= _temp1772 + 12; _temp1773;})); goto _LL1155;
_LL1207:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0,
_temp1316, 0))->r));{ struct Cyc_List_List* dles= _temp1316; for( 0; dles != 0;
dles=({ struct Cyc_List_List* _temp1774= dles; if( _temp1774 == 0){ _throw(
Null_Exception);} _temp1774->tl;})){ struct _tuple7 _temp1778; struct Cyc_Absyn_Exp*
_temp1779; struct Cyc_List_List* _temp1781; struct _tuple7* _temp1776=( struct
_tuple7*)({ struct Cyc_List_List* _temp1775= dles; if( _temp1775 == 0){ _throw(
Null_Exception);} _temp1775->hd;}); _temp1778=* _temp1776; _LL1782: _temp1781=
_temp1778.f1; goto _LL1780; _LL1780: _temp1779= _temp1778.f2; goto _LL1777;
_LL1777: Cyc_Toc_exp_to_c( nv, _temp1779);}} goto _LL1155; _LL1209: {
unsigned int sz= Cyc_Evexp_eval_const_uint_exp( _temp1320); Cyc_Toc_exp_to_c( nv,
_temp1318);{ struct Cyc_List_List* es= 0;{ unsigned int i= 0; for( 0; i < sz; i
++){ es=({ struct Cyc_List_List* _temp1783=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1783->hd=( void*)({ struct _tuple7*
_temp1784=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7)); _temp1784->f1=
0; _temp1784->f2= _temp1318; _temp1784;}); _temp1783->tl= es; _temp1783;});}}(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0, es, 0))->r));
goto _LL1155;}} _LL1211: if( ! nv->toplevel){( void*)( e->r=( void*)(( void*)(
Cyc_Toc_init_struct( nv, old_typ, 0, 0, _temp1326, _temp1330))->r));} else{
struct _tuple1* n= _temp1330; struct Cyc_Absyn_Structdecl* sd2=( struct Cyc_Absyn_Structdecl*)({
struct Cyc_Absyn_Structdecl* _temp1795= _temp1324 == 0?(( struct Cyc_Absyn_Structdecl*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1794=
Cyc_Absynpp_qvar2string( _temp1330); xprintf("no structdecl %.*s", _temp1794.last_plus_one
- _temp1794.curr, _temp1794.curr);})): _temp1324; if( _temp1795 == 0){ _throw(
Null_Exception);} _temp1795;}); struct Cyc_List_List* fields=(( struct Cyc_List_List*(*)(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* des, struct Cyc_Absyn_Structdecl*
sd)) Cyc_Tcutil_resolve_struct_designators)( e->loc, _temp1326,( struct Cyc_Absyn_Structdecl*)
sd2); for( 0; fields != 0; fields=({ struct Cyc_List_List* _temp1785= fields;
if( _temp1785 == 0){ _throw( Null_Exception);} _temp1785->tl;})){ struct
_tuple12 _temp1789; struct Cyc_Absyn_Exp* _temp1790; struct Cyc_Absyn_Structfield*
_temp1792; struct _tuple12* _temp1787=( struct _tuple12*)({ struct Cyc_List_List*
_temp1786= fields; if( _temp1786 == 0){ _throw( Null_Exception);} _temp1786->hd;});
_temp1789=* _temp1787; _LL1793: _temp1792= _temp1789.f1; goto _LL1791; _LL1791:
_temp1790= _temp1789.f2; goto _LL1788; _LL1788: { void* fieldtyp=( void*)
_temp1792->type; Cyc_Toc_exp_to_c( nv, _temp1790); if( Cyc_Toc_is_void_star(
fieldtyp)){( void*)( _temp1790->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1790->r, 0), 0))->r));}}}( void*)( e->r=( void*)((
void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_strctq( n), Cyc_Absyn_unresolvedmem_exp( 0,
_temp1326, 0), 0))->r));} goto _LL1155; _LL1213: { struct _tuple1* qv= _temp1332->name;
if( ! nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_var_exp( qv, 0))->r));}
else{ int tag_count= 0; struct Cyc_List_List* fields= _temp1334->fields == 0? 0:(
struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp1799= _temp1334->fields; if(
_temp1799 == 0){ _throw( Null_Exception);} _temp1799->v;}); while( Cyc_Absyn_qvar_cmp(
qv,(( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp1796= fields;
if( _temp1796 == 0){ _throw( Null_Exception);} _temp1796->hd;}))->name) != 0) {
if((( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp1797= fields;
if( _temp1797 == 0){ _throw( Null_Exception);} _temp1797->hd;}))->typs == 0){
tag_count ++;} fields=({ struct Cyc_List_List* _temp1798= fields; if( _temp1798
== 0){ _throw( Null_Exception);} _temp1798->tl;});}( void*)( e->r=( void*)((
void*)( Cyc_Absyn_uint_exp(( unsigned int) tag_count, 0))->r));} goto _LL1155;}
_LL1215: { struct _tuple1* qv= _temp1342->name; struct Cyc_List_List* tqts=
_temp1342->typs; struct _tuple1* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
xexp= Cyc_Absyn_var_exp( x, 0); void* strct_typ= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag(
qv,( struct _tagged_string)({ char* _temp1829=( char*)"_struct"; struct
_tagged_string _temp1830; _temp1830.curr= _temp1829; _temp1830.base= _temp1829;
_temp1830.last_plus_one= _temp1829 + 8; _temp1830;}))); if( nv->toplevel){ int
tag_count= 0; struct Cyc_List_List* fields= _temp1344->fields == 0? 0:( struct
Cyc_List_List*)({ struct Cyc_Core_Opt* _temp1818= _temp1344->fields; if(
_temp1818 == 0){ _throw( Null_Exception);} _temp1818->v;}); while( Cyc_Absyn_qvar_cmp(
qv,(( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp1800= fields;
if( _temp1800 == 0){ _throw( Null_Exception);} _temp1800->hd;}))->name) != 0) {
if((( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp1801= fields;
if( _temp1801 == 0){ _throw( Null_Exception);} _temp1801->hd;}))->typs != 0){
tag_count ++;} fields=({ struct Cyc_List_List* _temp1802= fields; if( _temp1802
== 0){ _throw( Null_Exception);} _temp1802->tl;});}{ struct Cyc_List_List* dles=
0; for( 0; _temp1346 != 0; _temp1346=({ struct Cyc_List_List* _temp1803=
_temp1346; if( _temp1803 == 0){ _throw( Null_Exception);} _temp1803->tl;}), tqts=({
struct Cyc_List_List* _temp1804= tqts; if( _temp1804 == 0){ _throw(
Null_Exception);} _temp1804->tl;})){ struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1810= _temp1346; if( _temp1810 == 0){ _throw(
Null_Exception);} _temp1810->hd;}); void* field_typ= Cyc_Toc_typ_to_c((*((
struct _tuple5*)({ struct Cyc_List_List* _temp1809= tqts; if( _temp1809 == 0){
_throw( Null_Exception);} _temp1809->hd;}))).f2); Cyc_Toc_exp_to_c( nv, cur_e);
if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp( field_typ,
cur_e, 0);} dles=({ struct Cyc_List_List* _temp1805=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp1805[ 0]=({ struct Cyc_List_List
_temp1806; _temp1806.hd=( void*)({ struct _tuple7* _temp1807=( struct _tuple7*)
GC_malloc( sizeof( struct _tuple7) * 1); _temp1807[ 0]=({ struct _tuple7
_temp1808; _temp1808.f1= 0; _temp1808.f2= cur_e; _temp1808;}); _temp1807;});
_temp1806.tl= dles; _temp1806;}); _temp1805;});}{ struct Cyc_Absyn_Exp* tag_exp=
Cyc_Absyn_uint_exp(( unsigned int) tag_count, 0); dles=({ struct Cyc_List_List*
_temp1811=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp1811[ 0]=({ struct Cyc_List_List _temp1812; _temp1812.hd=( void*)({ struct
_tuple7* _temp1813=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7) * 1);
_temp1813[ 0]=({ struct _tuple7 _temp1814; _temp1814.f1= 0; _temp1814.f2=
tag_exp; _temp1814;}); _temp1813;}); _temp1812.tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles); _temp1812;}); _temp1811;});(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(({ struct Cyc_Core_Opt*
_temp1815=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1815->v=( void*) Cyc_Toc_collapse_qvar_tag( qv,( struct _tagged_string)({
char* _temp1816=( char*)"_struct"; struct _tagged_string _temp1817; _temp1817.curr=
_temp1816; _temp1817.base= _temp1816; _temp1817.last_plus_one= _temp1816 + 8;
_temp1817;})); _temp1815;}), dles, 0))->r));}}} else{ struct Cyc_List_List*
rev_assign=({ struct Cyc_List_List* _temp1826=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1826->hd=( void*) Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( xexp, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp( Cyc_Toc_collapse_qvar_tag(
qv,( struct _tagged_string)({ char* _temp1827=( char*)"_tag"; struct
_tagged_string _temp1828; _temp1828.curr= _temp1827; _temp1828.base= _temp1827;
_temp1828.last_plus_one= _temp1827 + 5; _temp1828;})), 0), 0); _temp1826->tl= 0;
_temp1826;});{ int i= 1; for( 0; _temp1346 != 0;( _temp1346=({ struct Cyc_List_List*
_temp1819= _temp1346; if( _temp1819 == 0){ _throw( Null_Exception);} _temp1819->tl;}),
i ++), tqts=({ struct Cyc_List_List* _temp1820= tqts; if( _temp1820 == 0){
_throw( Null_Exception);} _temp1820->tl;})){ struct Cyc_Absyn_Exp* cur_e=(
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1824= _temp1346; if(
_temp1824 == 0){ _throw( Null_Exception);} _temp1824->hd;}); void* field_typ=
Cyc_Toc_typ_to_c((*(( struct _tuple5*)({ struct Cyc_List_List* _temp1823= tqts;
if( _temp1823 == 0){ _throw( Null_Exception);} _temp1823->hd;}))).f2); Cyc_Toc_exp_to_c(
nv, cur_e); if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp(
field_typ, cur_e, 0);}{ struct Cyc_Absyn_Stmt* a= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
xexp,({ struct _tagged_string* _temp1822=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp1822[ 0]= xprintf("f%d", i); _temp1822;}),
0), cur_e, 0); rev_assign=({ struct Cyc_List_List* _temp1821=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1821->hd=( void*) a; _temp1821->tl=
rev_assign; _temp1821;});}}}{ struct Cyc_Absyn_Stmt* final_stmt= Cyc_Absyn_exp_stmt(
xexp, 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmts((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)(({ struct Cyc_List_List* _temp1825=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1825->hd=(
void*) final_stmt; _temp1825->tl= rev_assign; _temp1825;})), 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, strct_typ, 0, s2,
0), 0))->r));}} goto _LL1155;} _LL1217: { struct _tuple1* qv= _temp1352->name;
struct Cyc_List_List* tqts= _temp1352->typs; struct Cyc_Absyn_Exp* tag_exp= Cyc_Absyn_var_exp(
Cyc_Toc_collapse_qvar_tag( qv,( struct _tagged_string)({ char* _temp1848=( char*)"_tag";
struct _tagged_string _temp1849; _temp1849.curr= _temp1848; _temp1849.base=
_temp1848; _temp1849.last_plus_one= _temp1848 + 5; _temp1849;})), 0); if( nv->toplevel){
struct Cyc_List_List* dles=(( struct Cyc_List_List*(*)( struct _tuple7*(* f)(
struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Toc_add_designator, nv, _temp1356); dles=({ struct Cyc_List_List*
_temp1831=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1831->hd=( void*)({ struct _tuple7* _temp1832=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp1832->f1= 0; _temp1832->f2= tag_exp; _temp1832;});
_temp1831->tl= dles; _temp1831;});( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(({
struct Cyc_Core_Opt* _temp1833=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1833->v=( void*) Cyc_Toc_collapse_qvar_tag( qv,( struct
_tagged_string)({ char* _temp1834=( char*)"_struct"; struct _tagged_string
_temp1835; _temp1835.curr= _temp1834; _temp1835.base= _temp1834; _temp1835.last_plus_one=
_temp1834 + 8; _temp1835;})); _temp1833;}), dles, 0))->r));} else{ struct
_tuple1* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp( x,
0); void* strct_typ= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( qv,( struct
_tagged_string)({ char* _temp1846=( char*)"_struct"; struct _tagged_string
_temp1847; _temp1847.curr= _temp1846; _temp1847.base= _temp1846; _temp1847.last_plus_one=
_temp1846 + 8; _temp1847;}))); struct Cyc_List_List* rev_assign=({ struct Cyc_List_List*
_temp1843=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1843->hd=( void*) Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( xexp,
Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp( Cyc_Toc_collapse_qvar_tag( qv,( struct
_tagged_string)({ char* _temp1844=( char*)"_tag"; struct _tagged_string
_temp1845; _temp1845.curr= _temp1844; _temp1845.base= _temp1844; _temp1845.last_plus_one=
_temp1844 + 5; _temp1845;})), 0), 0); _temp1843->tl= 0; _temp1843;});{ int i= 1;
for( 0; _temp1356 != 0;( _temp1356=({ struct Cyc_List_List* _temp1836= _temp1356;
if( _temp1836 == 0){ _throw( Null_Exception);} _temp1836->tl;}), i ++), tqts=({
struct Cyc_List_List* _temp1837= tqts; if( _temp1837 == 0){ _throw(
Null_Exception);} _temp1837->tl;})){ struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1841= _temp1356; if( _temp1841 == 0){ _throw(
Null_Exception);} _temp1841->hd;}); void* field_typ= Cyc_Toc_typ_to_c((*((
struct _tuple5*)({ struct Cyc_List_List* _temp1840= tqts; if( _temp1840 == 0){
_throw( Null_Exception);} _temp1840->hd;}))).f2); Cyc_Toc_exp_to_c( nv, cur_e);
if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp( field_typ,
cur_e, 0);}{ struct Cyc_Absyn_Stmt* a= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
xexp,({ struct _tagged_string* _temp1839=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp1839[ 0]= xprintf("f%d", i); _temp1839;}),
0), cur_e, 0); rev_assign=({ struct Cyc_List_List* _temp1838=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1838->hd=( void*) a; _temp1838->tl=
rev_assign; _temp1838;});}}}{ struct Cyc_Absyn_Stmt* final_stmt= Cyc_Absyn_exp_stmt(
xexp, 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmts((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)(({ struct Cyc_List_List* _temp1842=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1842->hd=(
void*) final_stmt; _temp1842->tl= rev_assign; _temp1842;})), 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, strct_typ, 0, s2,
0), 0))->r));}} goto _LL1155;} _LL1219: goto _LL1155; _LL1221: { void* t_c= Cyc_Toc_typ_to_c(
_temp1366); if( _temp1368 != 0){ struct Cyc_Absyn_Exp* rgn=( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1850= _temp1368; if( _temp1850 == 0){ _throw(
Null_Exception);} _temp1850;}); Cyc_Toc_exp_to_c( nv, rgn);( void*)( e->r=( void*)((
void*)( Cyc_Toc_rmalloc_exp( rgn, Cyc_Absyn_sizeoftyp_exp( t_c, 0)))->r));}
else{( void*)( e->r=( void*)(( void*)( Cyc_Toc_malloc_exp( _temp1366, Cyc_Absyn_sizeoftyp_exp(
t_c, 0)))->r));} goto _LL1155;} _LL1223: Cyc_Toc_stmt_to_c( nv, _temp1370); goto
_LL1155; _LL1225:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1851=( char*)"UnresolvedMem"; struct
_tagged_string _temp1852; _temp1852.curr= _temp1851; _temp1852.base= _temp1851;
_temp1852.last_plus_one= _temp1851 + 14; _temp1852;})); goto _LL1155; _LL1227:((
void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({
char* _temp1853=( char*)"codegen"; struct _tagged_string _temp1854; _temp1854.curr=
_temp1853; _temp1854.base= _temp1853; _temp1854.last_plus_one= _temp1853 + 8;
_temp1854;})); goto _LL1155; _LL1229:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ char* _temp1855=( char*)"fill"; struct _tagged_string
_temp1856; _temp1856.curr= _temp1855; _temp1856.base= _temp1855; _temp1856.last_plus_one=
_temp1855 + 5; _temp1856;})); goto _LL1155; _LL1155:;}} static struct Cyc_Absyn_Stmt*
Cyc_Toc_if_eq_goto_stmt( struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2,
struct _tagged_string* succ_lab, struct _tagged_string* fail_lab){ return Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( e1, e2, 0), Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0);} static struct _tuple13 Cyc_Toc_xlate_pat( struct Cyc_Toc_Env*
nv, void* t, struct Cyc_Absyn_Exp* r, struct Cyc_Absyn_Exp* path, struct Cyc_Absyn_Pat*
p, struct _tagged_string* succ_lab, struct _tagged_string* fail_lab, struct Cyc_List_List*
decls){ struct Cyc_Absyn_Stmt* s;{ void* _temp1858=( void*) p->r; struct Cyc_Absyn_Vardecl*
_temp1898; struct Cyc_Absyn_Vardecl _temp1900; struct Cyc_List_List* _temp1901;
struct Cyc_Core_Opt* _temp1903; int _temp1905; struct Cyc_Absyn_Exp* _temp1907;
void* _temp1909; struct Cyc_Absyn_Tqual* _temp1911; struct _tuple1* _temp1913;
void* _temp1915; struct Cyc_Absyn_Vardecl* _temp1917; int _temp1919; void*
_temp1921; char _temp1923; struct _tagged_string _temp1925; struct Cyc_Absyn_Enumfield*
_temp1927; struct Cyc_Absyn_Enumdecl* _temp1929; struct _tuple1* _temp1931;
struct Cyc_Absyn_Tunionfield* _temp1933; struct Cyc_Absyn_Tuniondecl* _temp1935;
struct Cyc_List_List* _temp1937; struct Cyc_List_List* _temp1939; struct Cyc_Core_Opt*
_temp1941; struct _tuple1* _temp1943; struct Cyc_Absyn_Pat* _temp1945; struct
Cyc_Absyn_Pat _temp1947; struct Cyc_Position_Segment* _temp1948; struct Cyc_Core_Opt*
_temp1950; void* _temp1952; struct Cyc_Absyn_Tunionfield* _temp1954; struct Cyc_Absyn_Tuniondecl*
_temp1956; struct Cyc_List_List* _temp1958; struct Cyc_List_List* _temp1960;
struct Cyc_Core_Opt* _temp1962; struct _tuple1* _temp1964; struct Cyc_Absyn_Pat*
_temp1966; struct Cyc_Absyn_Pat _temp1968; struct Cyc_Position_Segment*
_temp1969; struct Cyc_Core_Opt* _temp1971; void* _temp1973; struct Cyc_Absyn_Tunionfield*
_temp1975; struct Cyc_Absyn_XTuniondecl* _temp1977; struct Cyc_List_List*
_temp1979; struct Cyc_List_List* _temp1981; struct _tuple1* _temp1983; struct
Cyc_List_List* _temp1985; struct Cyc_List_List* _temp1987; struct Cyc_List_List*
_temp1989; struct Cyc_Core_Opt* _temp1991; struct Cyc_Absyn_Structdecl*
_temp1993; struct Cyc_Absyn_Pat* _temp1995; struct _tuple1* _temp1997; struct
Cyc_List_List* _temp1999; struct Cyc_List_List* _temp2001; struct _tuple1*
_temp2003; struct Cyc_List_List* _temp2005; struct Cyc_List_List* _temp2007;
struct _tuple1* _temp2009; struct Cyc_Absyn_Tunionfield* _temp2011; struct Cyc_Absyn_Tuniondecl*
_temp2013; struct Cyc_List_List* _temp2015; struct Cyc_List_List* _temp2017;
struct Cyc_Core_Opt* _temp2019; struct _tuple1* _temp2021; struct Cyc_Absyn_Tunionfield*
_temp2023; struct Cyc_Absyn_XTuniondecl* _temp2025; struct Cyc_List_List*
_temp2027; struct Cyc_List_List* _temp2029; struct _tuple1* _temp2031; _LL1860:
if(( int) _temp1858 == Cyc_Absyn_Wild_p){ goto _LL1861;} else{ goto _LL1862;}
_LL1862: if(( unsigned int) _temp1858 > 2u?(( struct _tunion_struct*) _temp1858)->tag
== Cyc_Absyn_Var_p_tag: 0){ _LL1899: _temp1898=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_p_struct*) _temp1858)->f1; _temp1900=* _temp1898; _LL1916:
_temp1915=( void*) _temp1900.sc; goto _LL1914; _LL1914: _temp1913=( struct
_tuple1*) _temp1900.name; goto _LL1912; _LL1912: _temp1911=( struct Cyc_Absyn_Tqual*)
_temp1900.tq; goto _LL1910; _LL1910: _temp1909=( void*) _temp1900.type; goto
_LL1908; _LL1908: _temp1907=( struct Cyc_Absyn_Exp*) _temp1900.initializer; goto
_LL1906; _LL1906: _temp1905=( int) _temp1900.shadow; goto _LL1904; _LL1904:
_temp1903=( struct Cyc_Core_Opt*) _temp1900.rgn; goto _LL1902; _LL1902:
_temp1901=( struct Cyc_List_List*) _temp1900.attributes; goto _LL1863;} else{
goto _LL1864;} _LL1864: if(( unsigned int) _temp1858 > 2u?(( struct
_tunion_struct*) _temp1858)->tag == Cyc_Absyn_Reference_p_tag: 0){ _LL1918:
_temp1917=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Reference_p_struct*)
_temp1858)->f1; goto _LL1865;} else{ goto _LL1866;} _LL1866: if(( int) _temp1858
== Cyc_Absyn_Null_p){ goto _LL1867;} else{ goto _LL1868;} _LL1868: if((
unsigned int) _temp1858 > 2u?(( struct _tunion_struct*) _temp1858)->tag == Cyc_Absyn_Int_p_tag:
0){ _LL1922: _temp1921=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp1858)->f1;
goto _LL1920; _LL1920: _temp1919=( int)(( struct Cyc_Absyn_Int_p_struct*)
_temp1858)->f2; goto _LL1869;} else{ goto _LL1870;} _LL1870: if(( unsigned int)
_temp1858 > 2u?(( struct _tunion_struct*) _temp1858)->tag == Cyc_Absyn_Char_p_tag:
0){ _LL1924: _temp1923=( char)(( struct Cyc_Absyn_Char_p_struct*) _temp1858)->f1;
goto _LL1871;} else{ goto _LL1872;} _LL1872: if(( unsigned int) _temp1858 > 2u?((
struct _tunion_struct*) _temp1858)->tag == Cyc_Absyn_Float_p_tag: 0){ _LL1926:
_temp1925=( struct _tagged_string)(( struct Cyc_Absyn_Float_p_struct*) _temp1858)->f1;
goto _LL1873;} else{ goto _LL1874;} _LL1874: if(( unsigned int) _temp1858 > 2u?((
struct _tunion_struct*) _temp1858)->tag == Cyc_Absyn_Enum_p_tag: 0){ _LL1932:
_temp1931=( struct _tuple1*)(( struct Cyc_Absyn_Enum_p_struct*) _temp1858)->f1;
goto _LL1930; _LL1930: _temp1929=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp1858)->f2; goto _LL1928; _LL1928: _temp1927=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_p_struct*) _temp1858)->f3; goto _LL1875;} else{ goto
_LL1876;} _LL1876: if(( unsigned int) _temp1858 > 2u?(( struct _tunion_struct*)
_temp1858)->tag == Cyc_Absyn_Tunion_p_tag: 0){ _LL1944: _temp1943=( struct
_tuple1*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp1858)->f1; goto _LL1942;
_LL1942: _temp1941=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1858)->f2; goto _LL1940; _LL1940: _temp1939=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1858)->f3; goto _LL1938; _LL1938:
_temp1937=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1858)->f4; if( _temp1937 == 0){ goto _LL1936;} else{ goto _LL1878;} _LL1936:
_temp1935=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1858)->f5; goto _LL1934; _LL1934: _temp1933=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1858)->f6; goto _LL1877;} else{ goto
_LL1878;} _LL1878: if(( unsigned int) _temp1858 > 2u?(( struct _tunion_struct*)
_temp1858)->tag == Cyc_Absyn_Pointer_p_tag: 0){ _LL1946: _temp1945=( struct Cyc_Absyn_Pat*)((
struct Cyc_Absyn_Pointer_p_struct*) _temp1858)->f1; _temp1947=* _temp1945;
_LL1953: _temp1952=( void*) _temp1947.r; if(( unsigned int) _temp1952 > 2u?((
struct _tunion_struct*) _temp1952)->tag == Cyc_Absyn_Tunion_p_tag: 0){ _LL1965:
_temp1964=( struct _tuple1*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp1952)->f1;
goto _LL1963; _LL1963: _temp1962=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1952)->f2; goto _LL1961; _LL1961: _temp1960=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1952)->f3; goto _LL1959; _LL1959:
_temp1958=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1952)->f4; goto _LL1957; _LL1957: _temp1956=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1952)->f5; goto _LL1955; _LL1955:
_temp1954=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1952)->f6; goto _LL1951;} else{ goto _LL1880;} _LL1951: _temp1950=( struct
Cyc_Core_Opt*) _temp1947.topt; goto _LL1949; _LL1949: _temp1948=( struct Cyc_Position_Segment*)
_temp1947.loc; goto _LL1879;} else{ goto _LL1880;} _LL1880: if(( unsigned int)
_temp1858 > 2u?(( struct _tunion_struct*) _temp1858)->tag == Cyc_Absyn_Pointer_p_tag:
0){ _LL1967: _temp1966=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1858)->f1; _temp1968=* _temp1966; _LL1974: _temp1973=( void*) _temp1968.r;
if(( unsigned int) _temp1973 > 2u?(( struct _tunion_struct*) _temp1973)->tag ==
Cyc_Absyn_XTunion_p_tag: 0){ _LL1984: _temp1983=( struct _tuple1*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp1973)->f1; goto _LL1982; _LL1982: _temp1981=( struct Cyc_List_List*)((
struct Cyc_Absyn_XTunion_p_struct*) _temp1973)->f2; goto _LL1980; _LL1980:
_temp1979=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp1973)->f3; goto _LL1978; _LL1978: _temp1977=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_p_struct*) _temp1973)->f4; goto _LL1976; _LL1976:
_temp1975=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp1973)->f5; goto _LL1972;} else{ goto _LL1882;} _LL1972: _temp1971=( struct
Cyc_Core_Opt*) _temp1968.topt; goto _LL1970; _LL1970: _temp1969=( struct Cyc_Position_Segment*)
_temp1968.loc; goto _LL1881;} else{ goto _LL1882;} _LL1882: if(( unsigned int)
_temp1858 > 2u?(( struct _tunion_struct*) _temp1858)->tag == Cyc_Absyn_Tuple_p_tag:
0){ _LL1986: _temp1985=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*)
_temp1858)->f1; goto _LL1883;} else{ goto _LL1884;} _LL1884: if(( unsigned int)
_temp1858 > 2u?(( struct _tunion_struct*) _temp1858)->tag == Cyc_Absyn_Struct_p_tag:
0){ _LL1994: _temp1993=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1858)->f1; goto _LL1992; _LL1992: _temp1991=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1858)->f2; goto _LL1990; _LL1990:
_temp1989=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1858)->f3; goto _LL1988; _LL1988: _temp1987=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1858)->f4; goto _LL1885;} else{ goto
_LL1886;} _LL1886: if(( unsigned int) _temp1858 > 2u?(( struct _tunion_struct*)
_temp1858)->tag == Cyc_Absyn_Pointer_p_tag: 0){ _LL1996: _temp1995=( struct Cyc_Absyn_Pat*)((
struct Cyc_Absyn_Pointer_p_struct*) _temp1858)->f1; goto _LL1887;} else{ goto
_LL1888;} _LL1888: if(( unsigned int) _temp1858 > 2u?(( struct _tunion_struct*)
_temp1858)->tag == Cyc_Absyn_UnknownId_p_tag: 0){ _LL1998: _temp1997=( struct
_tuple1*)(( struct Cyc_Absyn_UnknownId_p_struct*) _temp1858)->f1; goto _LL1889;}
else{ goto _LL1890;} _LL1890: if(( unsigned int) _temp1858 > 2u?(( struct
_tunion_struct*) _temp1858)->tag == Cyc_Absyn_UnknownCall_p_tag: 0){ _LL2004:
_temp2003=( struct _tuple1*)(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp1858)->f1;
goto _LL2002; _LL2002: _temp2001=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1858)->f2; goto _LL2000; _LL2000: _temp1999=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_p_struct*) _temp1858)->f3; goto _LL1891;} else{
goto _LL1892;} _LL1892: if(( unsigned int) _temp1858 > 2u?(( struct
_tunion_struct*) _temp1858)->tag == Cyc_Absyn_UnknownFields_p_tag: 0){ _LL2010:
_temp2009=( struct _tuple1*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1858)->f1; goto _LL2008; _LL2008: _temp2007=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownFields_p_struct*) _temp1858)->f2; goto _LL2006; _LL2006:
_temp2005=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1858)->f3; goto _LL1893;} else{ goto _LL1894;} _LL1894: if(( unsigned int)
_temp1858 > 2u?(( struct _tunion_struct*) _temp1858)->tag == Cyc_Absyn_Tunion_p_tag:
0){ _LL2022: _temp2021=( struct _tuple1*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1858)->f1; goto _LL2020; _LL2020: _temp2019=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1858)->f2; goto _LL2018; _LL2018:
_temp2017=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1858)->f3; goto _LL2016; _LL2016: _temp2015=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1858)->f4; goto _LL2014; _LL2014:
_temp2013=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1858)->f5; goto _LL2012; _LL2012: _temp2011=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1858)->f6; goto _LL1895;} else{ goto
_LL1896;} _LL1896: if(( unsigned int) _temp1858 > 2u?(( struct _tunion_struct*)
_temp1858)->tag == Cyc_Absyn_XTunion_p_tag: 0){ _LL2032: _temp2031=( struct
_tuple1*)(( struct Cyc_Absyn_XTunion_p_struct*) _temp1858)->f1; goto _LL2030;
_LL2030: _temp2029=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp1858)->f2; goto _LL2028; _LL2028: _temp2027=( struct Cyc_List_List*)((
struct Cyc_Absyn_XTunion_p_struct*) _temp1858)->f3; goto _LL2026; _LL2026:
_temp2025=( struct Cyc_Absyn_XTuniondecl*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp1858)->f4; goto _LL2024; _LL2024: _temp2023=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_XTunion_p_struct*) _temp1858)->f5; goto _LL1897;} else{ goto
_LL1859;} _LL1861: s= Cyc_Absyn_goto_stmt( succ_lab, 0); goto _LL1859; _LL1863:
nv= Cyc_Toc_add_varmap( nv, _temp1913, r); s= Cyc_Absyn_goto_stmt( succ_lab, 0);
goto _LL1859; _LL1865: { struct _tuple1* v= Cyc_Toc_temp_var(); decls=({ struct
Cyc_List_List* _temp2033=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2033->hd=( void*)({ struct _tuple14* _temp2034=( struct _tuple14*)
GC_malloc( sizeof( struct _tuple14)); _temp2034->f1= v; _temp2034->f2= Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c( t), Cyc_Toc_mt_tq); _temp2034;}); _temp2033->tl= decls;
_temp2033;}); nv= Cyc_Toc_add_varmap( nv, _temp1917->name, Cyc_Absyn_var_exp( v,
0)); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_address_exp(
path, 0), 0), Cyc_Absyn_goto_stmt( succ_lab, 0), 0); goto _LL1859;} _LL1867: s=
Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_signed_int_exp( 0, 0), succ_lab, fail_lab);
goto _LL1859; _LL1869: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_int_exp(
_temp1921, _temp1919, 0), succ_lab, fail_lab); goto _LL1859; _LL1871: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_char_exp( _temp1923, 0), succ_lab, fail_lab); goto _LL1859; _LL1873:
s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_float_exp( _temp1925, 0), succ_lab,
fail_lab); goto _LL1859; _LL1875: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Enum_e_struct* _temp2035=( struct Cyc_Absyn_Enum_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_e_struct)); _temp2035[ 0]=({ struct Cyc_Absyn_Enum_e_struct
_temp2036; _temp2036.tag= Cyc_Absyn_Enum_e_tag; _temp2036.f1= _temp1931;
_temp2036.f2=( struct Cyc_Absyn_Enumdecl*) _temp1929; _temp2036.f3=( struct Cyc_Absyn_Enumfield*)
_temp1927; _temp2036;}); _temp2035;}), 0), succ_lab, fail_lab); goto _LL1859;
_LL1877: s= Cyc_Toc_if_eq_goto_stmt( Cyc_Absyn_cast_exp( Cyc_Absyn_sint_t, r, 0),
Cyc_Absyn_var_exp( _temp1943, 0), succ_lab, fail_lab); goto _LL1859; _LL1879: {
struct Cyc_List_List* ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp1958); struct _tuple1* efstrct= Cyc_Toc_collapse_qvar_tag( _temp1954->name,(
struct _tagged_string)({ char* _temp2050=( char*)"_struct"; struct
_tagged_string _temp2051; _temp2051.curr= _temp2050; _temp2051.base= _temp2050;
_temp2051.last_plus_one= _temp2050 + 8; _temp2051;})); struct Cyc_Absyn_Exp*
rcast= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_strctq( efstrct), Cyc_Toc_mt_tq),
r, 0);{ struct Cyc_List_List* rps=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( _temp1958); for( 0; rps != 0; rps=({ struct Cyc_List_List*
_temp2037= rps; if( _temp2037 == 0){ _throw( Null_Exception);} _temp2037->tl;}),
cnt --){ struct Cyc_Absyn_Pat* p2=( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List*
_temp2044= rps; if( _temp2044 == 0){ _throw( Null_Exception);} _temp2044->hd;});
struct _tuple1* v= Cyc_Toc_temp_var(); void* t2=( void*)({ struct Cyc_Core_Opt*
_temp2043= p2->topt; if( _temp2043 == 0){ _throw( Null_Exception);} _temp2043->v;});
void* t2c= Cyc_Toc_typ_to_c( t2); struct _tagged_string* lab= Cyc_Toc_fresh_label();
decls=({ struct Cyc_List_List* _temp2038=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2038->hd=( void*)({ struct _tuple14*
_temp2039=( struct _tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp2039->f1=
v; _temp2039->f2= t2c; _temp2039;}); _temp2038->tl= decls; _temp2038;});{ struct
_tuple13 result= Cyc_Toc_xlate_pat( nv, t2, Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structarrow_exp(
rcast,({ struct _tagged_string* _temp2042=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp2042[ 0]= xprintf("f%d", cnt);
_temp2042;}), 0), p2, succ_lab, fail_lab, decls); nv= result.f1; decls= result.f2;{
struct Cyc_Absyn_Stmt* s3= result.f3; struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_cast_exp( t2c, Cyc_Absyn_structarrow_exp(
rcast,({ struct _tagged_string* _temp2041=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp2041[ 0]= xprintf("f%d", cnt);
_temp2041;}), 0), 0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s1, s3,
0); ss=({ struct Cyc_List_List* _temp2040=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2040->hd=( void*) Cyc_Absyn_label_stmt( lab,
s2, 0); _temp2040->tl= ss; _temp2040;}); succ_lab= lab;}}}}{ unsigned int
max_tag= 0;{ struct Cyc_List_List* fs=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp2047= _temp1956->fields; if( _temp2047 == 0){ _throw( Null_Exception);}
_temp2047->v;}); for( 0; fs != 0; fs=({ struct Cyc_List_List* _temp2045= fs; if(
_temp2045 == 0){ _throw( Null_Exception);} _temp2045->tl;})){ struct Cyc_Absyn_Tunionfield*
f=( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp2046= fs; if(
_temp2046 == 0){ _throw( Null_Exception);} _temp2046->hd;}); if( f->typs == 0){
max_tag ++;}}}{ struct Cyc_Absyn_Exp* max_tag_exp= Cyc_Absyn_uint_exp( max_tag,
0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_strct(
Cyc_Toc__tunion_struct_sp), Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_structarrow_exp(
e3, Cyc_Toc_tag_sp, 0); struct Cyc_Absyn_Exp* e5= Cyc_Absyn_gt_exp( Cyc_Absyn_cast_exp(
Cyc_Absyn_uint_t, r, 0), max_tag_exp, 0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp(
Cyc_Toc_collapse_qvar_tag( _temp1964,( struct _tagged_string)({ char* _temp2048=(
char*)"_tag"; struct _tagged_string _temp2049; _temp2049.curr= _temp2048;
_temp2049.base= _temp2048; _temp2049.last_plus_one= _temp2048 + 5; _temp2049;})),
0); struct Cyc_Absyn_Exp* test_exp= max_tag > 0? Cyc_Absyn_and_exp( e5, Cyc_Absyn_eq_exp(
e1, e, 0), 0): Cyc_Absyn_eq_exp( e1, e, 0); s= Cyc_Absyn_ifthenelse_stmt(
test_exp, Cyc_Absyn_seq_stmts( ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);
goto _LL1859;}}} _LL1881: { struct Cyc_List_List* ss= 0; int cnt=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp1979); struct _tuple1* xfstrct=
Cyc_Toc_collapse_qvar_tag( _temp1975->name,( struct _tagged_string)({ char*
_temp2064=( char*)"_struct"; struct _tagged_string _temp2065; _temp2065.curr=
_temp2064; _temp2065.base= _temp2064; _temp2065.last_plus_one= _temp2064 + 8;
_temp2065;})); struct Cyc_Absyn_Exp* rcast= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ(
Cyc_Absyn_strctq( xfstrct), Cyc_Toc_mt_tq), r, 0);{ struct Cyc_List_List* rps=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( _temp1979);
for( 0; rps != 0; rps=({ struct Cyc_List_List* _temp2052= rps; if( _temp2052 ==
0){ _throw( Null_Exception);} _temp2052->tl;}), cnt --){ struct Cyc_Absyn_Pat*
p2=( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List* _temp2059= rps; if(
_temp2059 == 0){ _throw( Null_Exception);} _temp2059->hd;}); struct _tuple1* v=
Cyc_Toc_temp_var(); void* t2=( void*)({ struct Cyc_Core_Opt* _temp2058= p2->topt;
if( _temp2058 == 0){ _throw( Null_Exception);} _temp2058->v;}); struct
_tagged_string* lab= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp2053=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2053->hd=( void*)({ struct _tuple14* _temp2054=( struct _tuple14*)
GC_malloc( sizeof( struct _tuple14)); _temp2054->f1= v; _temp2054->f2= Cyc_Toc_typ_to_c(
t2); _temp2054;}); _temp2053->tl= decls; _temp2053;});{ struct _tuple13 result=
Cyc_Toc_xlate_pat( nv, t2, Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structarrow_exp(
rcast,({ struct _tagged_string* _temp2057=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp2057[ 0]= xprintf("f%d", cnt);
_temp2057;}), 0), p2, succ_lab, fail_lab, decls); nv= result.f1; decls= result.f2;{
struct Cyc_Absyn_Stmt* s3= result.f3; struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structarrow_exp( rcast,({ struct
_tagged_string* _temp2056=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp2056[ 0]= xprintf("f%d", cnt); _temp2056;}), 0), 0);
struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s1, s3, 0); ss=({ struct Cyc_List_List*
_temp2055=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2055->hd=( void*) Cyc_Absyn_label_stmt( lab, s2, 0); _temp2055->tl= ss;
_temp2055;}); succ_lab= lab;}}}} if( _temp1979 == 0){ s= Cyc_Toc_if_eq_goto_stmt(
Cyc_Absyn_structarrow_exp( r, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp( Cyc_Toc_collapse_qvar_tag(
_temp1983,( struct _tagged_string)({ char* _temp2060=( char*)"_tag"; struct
_tagged_string _temp2061; _temp2061.curr= _temp2060; _temp2061.base= _temp2060;
_temp2061.last_plus_one= _temp2060 + 5; _temp2061;})), 0), succ_lab, fail_lab);}
else{ struct Cyc_Absyn_Exp* e3= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_strct(
Cyc_Toc__xtunion_struct_sp), Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp* e2=
Cyc_Absyn_deref_exp( e3, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_structmember_exp(
e2, Cyc_Toc_tag_sp, 0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp( Cyc_Toc_collapse_qvar_tag(
_temp1983,( struct _tagged_string)({ char* _temp2062=( char*)"_tag"; struct
_tagged_string _temp2063; _temp2063.curr= _temp2062; _temp2063.base= _temp2062;
_temp2063.last_plus_one= _temp2062 + 5; _temp2063;})), 0); s= Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( e1, e, 0), Cyc_Absyn_seq_stmts( ss, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0);} goto _LL1859;} _LL1883: { struct Cyc_List_List* ss= 0; int
cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1985);{ struct
Cyc_List_List* rps=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp1985); for( 0; rps != 0; rps=({ struct Cyc_List_List* _temp2066= rps; if(
_temp2066 == 0){ _throw( Null_Exception);} _temp2066->tl;}), cnt --){ struct Cyc_Absyn_Pat*
p2=( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List* _temp2073= rps; if(
_temp2073 == 0){ _throw( Null_Exception);} _temp2073->hd;}); struct _tuple1* v=
Cyc_Toc_temp_var(); void* t2=( void*)({ struct Cyc_Core_Opt* _temp2072= p2->topt;
if( _temp2072 == 0){ _throw( Null_Exception);} _temp2072->v;}); struct
_tagged_string* lab= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp2067=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2067->hd=( void*)({ struct _tuple14* _temp2068=( struct _tuple14*)
GC_malloc( sizeof( struct _tuple14)); _temp2068->f1= v; _temp2068->f2= Cyc_Toc_typ_to_c(
t2); _temp2068;}); _temp2067->tl= decls; _temp2067;});{ struct _tuple13 result=
Cyc_Toc_xlate_pat( nv, t2, Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structmember_exp(
path,({ struct _tagged_string* _temp2071=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp2071[ 0]= xprintf("f%d", cnt);
_temp2071;}), 0), p2, succ_lab, fail_lab, decls); nv= result.f1; decls= result.f2;{
struct Cyc_Absyn_Stmt* s2= result.f3; struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structmember_exp( r,({
struct _tagged_string* _temp2070=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp2070[ 0]= xprintf("f%d", cnt); _temp2070;}), 0),
0), s2, 0); ss=({ struct Cyc_List_List* _temp2069=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2069->hd=( void*) Cyc_Absyn_label_stmt(
lab, s3, 0); _temp2069->tl= ss; _temp2069;}); succ_lab= lab;}}}} s= Cyc_Absyn_seq_stmts(
ss, 0); goto _LL1859;} _LL1885: { struct Cyc_List_List* ss= 0;{ struct Cyc_List_List*
dlps=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp1987); for( 0; dlps != 0; dlps=({ struct Cyc_List_List* _temp2074= dlps;
if( _temp2074 == 0){ _throw( Null_Exception);} _temp2074->tl;})){ struct
_tuple15* tup=( struct _tuple15*)({ struct Cyc_List_List* _temp2090= dlps; if(
_temp2090 == 0){ _throw( Null_Exception);} _temp2090->hd;}); struct Cyc_Absyn_Pat*
p2=(* tup).f2; struct _tagged_string* f;{ void* _temp2076=( void*)({ struct Cyc_List_List*
_temp2075=(* tup).f1; if( _temp2075 == 0){ _throw( Null_Exception);} _temp2075->hd;});
struct _tagged_string* _temp2082; _LL2078: if((( struct _tunion_struct*)
_temp2076)->tag == Cyc_Absyn_FieldName_tag){ _LL2083: _temp2082=( struct
_tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp2076)->f1; goto
_LL2079;} else{ goto _LL2080;} _LL2080: goto _LL2081; _LL2079: f= _temp2082;
goto _LL2077; _LL2081:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Toc_Match_error_struct*
_temp2084=( struct Cyc_Toc_Match_error_struct*) GC_malloc( sizeof( struct Cyc_Toc_Match_error_struct));
_temp2084[ 0]=({ struct Cyc_Toc_Match_error_struct _temp2085; _temp2085.tag= Cyc_Toc_Match_error_tag;
_temp2085;}); _temp2084;})); _LL2077:;}{ struct _tuple1* v= Cyc_Toc_temp_var();
void* t2=( void*)({ struct Cyc_Core_Opt* _temp2089= p2->topt; if( _temp2089 == 0){
_throw( Null_Exception);} _temp2089->v;}); void* t2c= Cyc_Toc_typ_to_c( t2);
struct _tagged_string* lab= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp2086=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2086->hd=( void*)({ struct _tuple14* _temp2087=( struct _tuple14*)
GC_malloc( sizeof( struct _tuple14)); _temp2087->f1= v; _temp2087->f2= t2c;
_temp2087;}); _temp2086->tl= decls; _temp2086;});{ struct _tuple13 result= Cyc_Toc_xlate_pat(
nv, t2, Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structmember_exp( path, f, 0), p2,
succ_lab, fail_lab, decls); nv= result.f1; decls= result.f2;{ struct Cyc_Absyn_Stmt*
s2= result.f3; struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_cast_exp( t2c, Cyc_Absyn_structmember_exp( r,
f, 0), 0), 0), s2, 0); ss=({ struct Cyc_List_List* _temp2088=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2088->hd=( void*) Cyc_Absyn_label_stmt(
lab, s3, 0); _temp2088->tl= ss; _temp2088;}); succ_lab= lab;}}}}} s= Cyc_Absyn_seq_stmts(
ss, 0); goto _LL1859;} _LL1887: { struct _tuple1* v= Cyc_Toc_temp_var(); void*
t2=( void*)({ struct Cyc_Core_Opt* _temp2093= _temp1995->topt; if( _temp2093 ==
0){ _throw( Null_Exception);} _temp2093->v;}); decls=({ struct Cyc_List_List*
_temp2091=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2091->hd=( void*)({ struct _tuple14* _temp2092=( struct _tuple14*)
GC_malloc( sizeof( struct _tuple14)); _temp2092->f1= v; _temp2092->f2= Cyc_Toc_typ_to_c(
t2); _temp2092;}); _temp2091->tl= decls; _temp2091;});{ struct _tuple13 result=
Cyc_Toc_xlate_pat( nv, t2, Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_deref_exp( path,
0), _temp1995, succ_lab, fail_lab, decls); nv= result.f1; decls= result.f2;{
struct Cyc_Absyn_Stmt* s2= result.f3; struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_deref_exp( r, 0), 0),
s2, 0); if( Cyc_Toc_is_nullable( t)){ s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp(
r, Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_goto_stmt( fail_lab, 0), s3, 0);}
else{ s= s3;} goto _LL1859;}}} _LL1889: s=(( struct Cyc_Absyn_Stmt*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp2094=(
char*)"unknownid"; struct _tagged_string _temp2095; _temp2095.curr= _temp2094;
_temp2095.base= _temp2094; _temp2095.last_plus_one= _temp2094 + 10; _temp2095;}));
goto _LL1859; _LL1891: s=(( struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp2096=( char*)"unknowncall";
struct _tagged_string _temp2097; _temp2097.curr= _temp2096; _temp2097.base=
_temp2096; _temp2097.last_plus_one= _temp2096 + 12; _temp2097;})); goto _LL1859;
_LL1893: s=(( struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp2098=( char*)"unknownfields"; struct
_tagged_string _temp2099; _temp2099.curr= _temp2098; _temp2099.base= _temp2098;
_temp2099.last_plus_one= _temp2098 + 14; _temp2099;})); goto _LL1859; _LL1895:
goto _LL1897; _LL1897: s=(( struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp2100=( char*)"non-null [x]tunion pattern not under &";
struct _tagged_string _temp2101; _temp2101.curr= _temp2100; _temp2101.base=
_temp2100; _temp2101.last_plus_one= _temp2100 + 39; _temp2101;})); goto _LL1859;
_LL1859:;} return({ struct _tuple13 _temp2102; _temp2102.f1= nv; _temp2102.f2=
decls; _temp2102.f3= s; _temp2102;});} static struct _tuple16* Cyc_Toc_gen_label(
struct Cyc_Absyn_Switch_clause* sc){ return({ struct _tuple16* _temp2103=(
struct _tuple16*) GC_malloc( sizeof( struct _tuple16)); _temp2103->f1= Cyc_Toc_fresh_label();
_temp2103->f2= Cyc_Toc_fresh_label(); _temp2103->f3= sc; _temp2103;});} static
void Cyc_Toc_xlate_switch( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Stmt*
whole_s, struct Cyc_Absyn_Exp* e, struct Cyc_List_List* scs){ Cyc_Toc_exp_to_c(
nv, e);{ void* t=( void*)({ struct Cyc_Core_Opt* _temp2159= e->topt; if(
_temp2159 == 0){ _throw( Null_Exception);} _temp2159->v;}); int leave_as_switch;{
void* _temp2104= Cyc_Tcutil_compress( t); void* _temp2110; void* _temp2112;
_LL2106: if(( unsigned int) _temp2104 > 4u?(( struct _tunion_struct*) _temp2104)->tag
== Cyc_Absyn_IntType_tag: 0){ _LL2113: _temp2112=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2104)->f1; goto _LL2111; _LL2111: _temp2110=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2104)->f2; goto _LL2107;} else{ goto _LL2108;} _LL2108: goto _LL2109;
_LL2107: leave_as_switch= 1; goto _LL2105; _LL2109: leave_as_switch= 0; goto
_LL2105; _LL2105:;}{ struct Cyc_List_List* scs1= scs; for( 0; scs1 != 0; scs1=({
struct Cyc_List_List* _temp2114= scs1; if( _temp2114 == 0){ _throw(
Null_Exception);} _temp2114->tl;})){ if(( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp2116=(( struct Cyc_Absyn_Switch_clause*)({ struct Cyc_List_List* _temp2115=
scs1; if( _temp2115 == 0){ _throw( Null_Exception);} _temp2115->hd;}))->pat_vars;
if( _temp2116 == 0){ _throw( Null_Exception);} _temp2116->v;}) != 0){
leave_as_switch= 0; break;}}} if( leave_as_switch){ struct _tagged_string*
next_l= Cyc_Toc_fresh_label();{ struct Cyc_List_List* scs1= scs; for( 0; scs1 !=
0; scs1=({ struct Cyc_List_List* _temp2117= scs1; if( _temp2117 == 0){ _throw(
Null_Exception);} _temp2117->tl;})){ struct Cyc_Absyn_Stmt* s=(( struct Cyc_Absyn_Switch_clause*)({
struct Cyc_List_List* _temp2119= scs1; if( _temp2119 == 0){ _throw(
Null_Exception);} _temp2119->hd;}))->body;(( struct Cyc_Absyn_Switch_clause*)({
struct Cyc_List_List* _temp2118= scs1; if( _temp2118 == 0){ _throw(
Null_Exception);} _temp2118->hd;}))->body= Cyc_Absyn_label_stmt( next_l, s, 0);
next_l= Cyc_Toc_fresh_label(); Cyc_Toc_stmt_to_c( Cyc_Toc_switch_as_switch_env(
nv, next_l), s);}} return;}{ struct _tuple1* v= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
r= Cyc_Absyn_var_exp( v, 0); struct Cyc_Absyn_Exp* path= Cyc_Absyn_var_exp( v, 0);
struct _tagged_string* end_l= Cyc_Toc_fresh_label(); struct Cyc_List_List* lscs=((
struct Cyc_List_List*(*)( struct _tuple16*(* f)( struct Cyc_Absyn_Switch_clause*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_gen_label, scs); struct Cyc_List_List*
test_stmts= 0; struct Cyc_List_List* nvs= 0; struct Cyc_List_List* decls= 0;{
struct Cyc_List_List* lscs2= lscs; for( 0; lscs2 != 0; lscs2=({ struct Cyc_List_List*
_temp2120= lscs2; if( _temp2120 == 0){ _throw( Null_Exception);} _temp2120->tl;})){
struct Cyc_Absyn_Switch_clause* sc=(*(( struct _tuple16*)({ struct Cyc_List_List*
_temp2132= lscs2; if( _temp2132 == 0){ _throw( Null_Exception);} _temp2132->hd;}))).f3;
struct _tagged_string* fail_lab=({ struct Cyc_List_List* _temp2129= lscs2; if(
_temp2129 == 0){ _throw( Null_Exception);} _temp2129->tl;}) == 0? end_l:(*((
struct _tuple16*)({ struct Cyc_List_List* _temp2131=({ struct Cyc_List_List*
_temp2130= lscs2; if( _temp2130 == 0){ _throw( Null_Exception);} _temp2130->tl;});
if( _temp2131 == 0){ _throw( Null_Exception);} _temp2131->hd;}))).f1; struct
_tagged_string* succ_lab=(*(( struct _tuple16*)({ struct Cyc_List_List*
_temp2128= lscs2; if( _temp2128 == 0){ _throw( Null_Exception);} _temp2128->hd;}))).f2;
if( sc->where_clause == 0){ struct _tuple13 res= Cyc_Toc_xlate_pat( nv, t, r,
path, sc->pattern, succ_lab, fail_lab, decls); nvs=({ struct Cyc_List_List*
_temp2121=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2121->hd=( void*) res.f1; _temp2121->tl= nvs; _temp2121;}); decls= res.f2;
test_stmts=({ struct Cyc_List_List* _temp2122=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2122->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple16*)({ struct Cyc_List_List* _temp2123= lscs2; if( _temp2123 == 0){
_throw( Null_Exception);} _temp2123->hd;}))).f1, res.f3, 0); _temp2122->tl=
test_stmts; _temp2122;});} else{ struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp2127= sc->where_clause; if( _temp2127 == 0){ _throw(
Null_Exception);} _temp2127;}); struct _tagged_string* intermed_lab= Cyc_Toc_fresh_label();
struct _tuple13 res= Cyc_Toc_xlate_pat( nv, t, r, path, sc->pattern,
intermed_lab, fail_lab, decls); Cyc_Toc_exp_to_c( res.f1, e); nvs=({ struct Cyc_List_List*
_temp2124=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2124->hd=( void*) res.f1; _temp2124->tl= nvs; _temp2124;}); decls= res.f2;{
struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_ifthenelse_stmt( e, Cyc_Absyn_goto_stmt(
succ_lab, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0); struct Cyc_Absyn_Stmt* s3=
Cyc_Absyn_label_stmt( intermed_lab, s2, 0); test_stmts=({ struct Cyc_List_List*
_temp2125=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2125->hd=( void*) Cyc_Absyn_label_stmt((*(( struct _tuple16*)({ struct Cyc_List_List*
_temp2126= lscs2; if( _temp2126 == 0){ _throw( Null_Exception);} _temp2126->hd;}))).f1,
Cyc_Absyn_seq_stmt( res.f3, s3, 0), 0); _temp2125->tl= test_stmts; _temp2125;});}}}}{
struct Cyc_Absyn_Stmt* test_stmt= Cyc_Absyn_seq_stmts((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( test_stmts), 0); nvs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( nvs);{ struct Cyc_Absyn_Stmt*
clauses= Cyc_Absyn_skip_stmt( 0); if( lscs != 0){ for( 0;({ struct Cyc_List_List*
_temp2133= lscs; if( _temp2133 == 0){ _throw( Null_Exception);} _temp2133->tl;})
!= 0; lscs=({ struct Cyc_List_List* _temp2134= lscs; if( _temp2134 == 0){ _throw(
Null_Exception);} _temp2134->tl;}), nvs=({ struct Cyc_List_List* _temp2135= nvs;
if( _temp2135 == 0){ _throw( Null_Exception);} _temp2135->tl;})){ struct Cyc_Toc_Env*
e=( struct Cyc_Toc_Env*)({ struct Cyc_List_List* _temp2145= nvs; if( _temp2145
== 0){ _throw( Null_Exception);} _temp2145->hd;}); struct Cyc_Absyn_Stmt* s=((*((
struct _tuple16*)({ struct Cyc_List_List* _temp2144= lscs; if( _temp2144 == 0){
_throw( Null_Exception);} _temp2144->hd;}))).f3)->body; Cyc_Toc_stmt_to_c( Cyc_Toc_non_last_switchclause_env(
e, end_l,(*(( struct _tuple16*)({ struct Cyc_List_List* _temp2137=({ struct Cyc_List_List*
_temp2136= lscs; if( _temp2136 == 0){ _throw( Null_Exception);} _temp2136->tl;});
if( _temp2137 == 0){ _throw( Null_Exception);} _temp2137->hd;}))).f2,( struct
Cyc_List_List*)({ struct Cyc_Core_Opt* _temp2140=((*(( struct _tuple16*)({
struct Cyc_List_List* _temp2139=({ struct Cyc_List_List* _temp2138= lscs; if(
_temp2138 == 0){ _throw( Null_Exception);} _temp2138->tl;}); if( _temp2139 == 0){
_throw( Null_Exception);} _temp2139->hd;}))).f3)->pat_vars; if( _temp2140 == 0){
_throw( Null_Exception);} _temp2140->v;}),( struct Cyc_Toc_Env*)({ struct Cyc_List_List*
_temp2142=({ struct Cyc_List_List* _temp2141= nvs; if( _temp2141 == 0){ _throw(
Null_Exception);} _temp2141->tl;}); if( _temp2142 == 0){ _throw( Null_Exception);}
_temp2142->hd;})), s); clauses= Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*((
struct _tuple16*)({ struct Cyc_List_List* _temp2143= lscs; if( _temp2143 == 0){
_throw( Null_Exception);} _temp2143->hd;}))).f2, s, 0), 0);}{ struct Cyc_Toc_Env*
e=( struct Cyc_Toc_Env*)({ struct Cyc_List_List* _temp2148= nvs; if( _temp2148
== 0){ _throw( Null_Exception);} _temp2148->hd;}); struct Cyc_Absyn_Stmt* s=((*((
struct _tuple16*)({ struct Cyc_List_List* _temp2147= lscs; if( _temp2147 == 0){
_throw( Null_Exception);} _temp2147->hd;}))).f3)->body; Cyc_Toc_stmt_to_c( Cyc_Toc_last_switchclause_env(
e, end_l), s); clauses= Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*((
struct _tuple16*)({ struct Cyc_List_List* _temp2146= lscs; if( _temp2146 == 0){
_throw( Null_Exception);} _temp2146->hd;}))).f2, s, 0), 0);}}{ struct Cyc_Absyn_Stmt*
res= Cyc_Absyn_seq_stmt( test_stmt, Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt(
end_l, Cyc_Absyn_skip_stmt( 0), 0), 0), 0); for( decls; decls != 0; decls=({
struct Cyc_List_List* _temp2149= decls; if( _temp2149 == 0){ _throw(
Null_Exception);} _temp2149->tl;})){ struct _tuple14 _temp2153; void* _temp2154;
struct _tuple1* _temp2156; struct _tuple14* _temp2151=( struct _tuple14*)({
struct Cyc_List_List* _temp2150= decls; if( _temp2150 == 0){ _throw(
Null_Exception);} _temp2150->hd;}); _temp2153=* _temp2151; _LL2157: _temp2156=
_temp2153.f1; goto _LL2155; _LL2155: _temp2154= _temp2153.f2; goto _LL2152;
_LL2152: res= Cyc_Absyn_declare_stmt( _temp2156, _temp2154, 0, res, 0);}( void*)(
whole_s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt( v, Cyc_Toc_typ_to_c(( void*)({
struct Cyc_Core_Opt* _temp2158= e->topt; if( _temp2158 == 0){ _throw(
Null_Exception);} _temp2158->v;})),( struct Cyc_Absyn_Exp*) e, res, 0))->r));}}}}}}
static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e, int
exhaust, struct Cyc_Absyn_Stmt* s); static struct _tuple17 Cyc_Toc_decls_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_List_List* ds, int top); struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler( int n){ return Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__npop_handler_qv, 0),({ struct Cyc_List_List*
_temp2160=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp2160[ 0]=({ struct Cyc_List_List _temp2161; _temp2161.hd=( void*) Cyc_Absyn_uint_exp((
unsigned int)( n - 1), 0); _temp2161.tl= 0; _temp2161;}); _temp2160;}), 0), 0);}
void Cyc_Toc_do_npop_before( int n, struct Cyc_Absyn_Stmt* s){ if( n > 0){( void*)(
s->r=( void*)(( void*)( Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( n), Cyc_Absyn_new_stmt((
void*) s->r, 0), 0))->r));}} static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Stmt* s){ while( 1) { void* _temp2162=( void*) s->r; struct
Cyc_Absyn_Exp* _temp2202; struct Cyc_Absyn_Stmt* _temp2204; struct Cyc_Absyn_Stmt*
_temp2206; struct Cyc_Absyn_Exp* _temp2208; struct Cyc_Absyn_Stmt* _temp2210;
struct Cyc_Absyn_Stmt* _temp2212; struct Cyc_Absyn_Exp* _temp2214; struct Cyc_Absyn_Stmt*
_temp2216; struct _tuple3 _temp2218; struct Cyc_Absyn_Stmt* _temp2220; struct
Cyc_Absyn_Exp* _temp2222; struct Cyc_Absyn_Stmt* _temp2224; struct Cyc_Absyn_Stmt*
_temp2226; struct Cyc_Absyn_Stmt* _temp2228; struct _tagged_string* _temp2230;
struct Cyc_Absyn_Stmt* _temp2232; struct _tuple3 _temp2234; struct Cyc_Absyn_Stmt*
_temp2236; struct Cyc_Absyn_Exp* _temp2238; struct _tuple3 _temp2240; struct Cyc_Absyn_Stmt*
_temp2242; struct Cyc_Absyn_Exp* _temp2244; struct Cyc_Absyn_Exp* _temp2246;
struct Cyc_List_List* _temp2248; struct Cyc_Absyn_Exp* _temp2250; struct Cyc_Absyn_Switch_clause**
_temp2252; struct Cyc_List_List* _temp2254; struct Cyc_Absyn_Stmt* _temp2256;
struct Cyc_Absyn_Decl* _temp2258; struct Cyc_Absyn_Stmt* _temp2260; struct Cyc_Absyn_Stmt*
_temp2262; struct Cyc_Absyn_Stmt* _temp2264; struct _tagged_string* _temp2266;
struct _tuple3 _temp2268; struct Cyc_Absyn_Stmt* _temp2270; struct Cyc_Absyn_Exp*
_temp2272; struct Cyc_Absyn_Stmt* _temp2274; struct Cyc_List_List* _temp2276;
struct Cyc_Absyn_Stmt* _temp2278; struct Cyc_Absyn_Stmt* _temp2280; struct Cyc_Absyn_Vardecl*
_temp2282; struct Cyc_Absyn_Tvar* _temp2284; _LL2164: if(( int) _temp2162 == Cyc_Absyn_Skip_s){
goto _LL2165;} else{ goto _LL2166;} _LL2166: if(( unsigned int) _temp2162 > 1u?((
struct _tunion_struct*) _temp2162)->tag == Cyc_Absyn_Exp_s_tag: 0){ _LL2203:
_temp2202=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*) _temp2162)->f1;
goto _LL2167;} else{ goto _LL2168;} _LL2168: if(( unsigned int) _temp2162 > 1u?((
struct _tunion_struct*) _temp2162)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL2207:
_temp2206=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp2162)->f1;
goto _LL2205; _LL2205: _temp2204=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp2162)->f2; goto _LL2169;} else{ goto _LL2170;} _LL2170: if(( unsigned int)
_temp2162 > 1u?(( struct _tunion_struct*) _temp2162)->tag == Cyc_Absyn_Return_s_tag:
0){ _LL2209: _temp2208=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp2162)->f1; goto _LL2171;} else{ goto _LL2172;} _LL2172: if(( unsigned int)
_temp2162 > 1u?(( struct _tunion_struct*) _temp2162)->tag == Cyc_Absyn_IfThenElse_s_tag:
0){ _LL2215: _temp2214=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2162)->f1; goto _LL2213; _LL2213: _temp2212=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp2162)->f2; goto _LL2211; _LL2211:
_temp2210=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2162)->f3; goto _LL2173;} else{ goto _LL2174;} _LL2174: if(( unsigned int)
_temp2162 > 1u?(( struct _tunion_struct*) _temp2162)->tag == Cyc_Absyn_While_s_tag:
0){ _LL2219: _temp2218=( struct _tuple3)(( struct Cyc_Absyn_While_s_struct*)
_temp2162)->f1; _LL2223: _temp2222= _temp2218.f1; goto _LL2221; _LL2221:
_temp2220= _temp2218.f2; goto _LL2217; _LL2217: _temp2216=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_While_s_struct*) _temp2162)->f2; goto _LL2175;} else{ goto
_LL2176;} _LL2176: if(( unsigned int) _temp2162 > 1u?(( struct _tunion_struct*)
_temp2162)->tag == Cyc_Absyn_Break_s_tag: 0){ _LL2225: _temp2224=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Break_s_struct*) _temp2162)->f1; goto _LL2177;} else{ goto
_LL2178;} _LL2178: if(( unsigned int) _temp2162 > 1u?(( struct _tunion_struct*)
_temp2162)->tag == Cyc_Absyn_Continue_s_tag: 0){ _LL2227: _temp2226=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Continue_s_struct*) _temp2162)->f1; goto _LL2179;} else{ goto
_LL2180;} _LL2180: if(( unsigned int) _temp2162 > 1u?(( struct _tunion_struct*)
_temp2162)->tag == Cyc_Absyn_Goto_s_tag: 0){ _LL2231: _temp2230=( struct
_tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp2162)->f1; goto _LL2229;
_LL2229: _temp2228=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp2162)->f2; goto _LL2181;} else{ goto _LL2182;} _LL2182: if(( unsigned int)
_temp2162 > 1u?(( struct _tunion_struct*) _temp2162)->tag == Cyc_Absyn_For_s_tag:
0){ _LL2247: _temp2246=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp2162)->f1; goto _LL2241; _LL2241: _temp2240=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp2162)->f2; _LL2245: _temp2244= _temp2240.f1; goto _LL2243; _LL2243:
_temp2242= _temp2240.f2; goto _LL2235; _LL2235: _temp2234=( struct _tuple3)((
struct Cyc_Absyn_For_s_struct*) _temp2162)->f3; _LL2239: _temp2238= _temp2234.f1;
goto _LL2237; _LL2237: _temp2236= _temp2234.f2; goto _LL2233; _LL2233: _temp2232=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp2162)->f4; goto
_LL2183;} else{ goto _LL2184;} _LL2184: if(( unsigned int) _temp2162 > 1u?((
struct _tunion_struct*) _temp2162)->tag == Cyc_Absyn_Switch_s_tag: 0){ _LL2251:
_temp2250=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp2162)->f1; goto _LL2249; _LL2249: _temp2248=( struct Cyc_List_List*)((
struct Cyc_Absyn_Switch_s_struct*) _temp2162)->f2; goto _LL2185;} else{ goto
_LL2186;} _LL2186: if(( unsigned int) _temp2162 > 1u?(( struct _tunion_struct*)
_temp2162)->tag == Cyc_Absyn_Fallthru_s_tag: 0){ _LL2255: _temp2254=( struct Cyc_List_List*)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp2162)->f1; goto _LL2253; _LL2253:
_temp2252=( struct Cyc_Absyn_Switch_clause**)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp2162)->f2; goto _LL2187;} else{ goto _LL2188;} _LL2188: if(( unsigned int)
_temp2162 > 1u?(( struct _tunion_struct*) _temp2162)->tag == Cyc_Absyn_Decl_s_tag:
0){ _LL2259: _temp2258=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp2162)->f1; goto _LL2257; _LL2257: _temp2256=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Decl_s_struct*) _temp2162)->f2; goto _LL2189;} else{ goto
_LL2190;} _LL2190: if(( unsigned int) _temp2162 > 1u?(( struct _tunion_struct*)
_temp2162)->tag == Cyc_Absyn_Cut_s_tag: 0){ _LL2261: _temp2260=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Cut_s_struct*) _temp2162)->f1; goto _LL2191;} else{ goto
_LL2192;} _LL2192: if(( unsigned int) _temp2162 > 1u?(( struct _tunion_struct*)
_temp2162)->tag == Cyc_Absyn_Splice_s_tag: 0){ _LL2263: _temp2262=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Splice_s_struct*) _temp2162)->f1; goto _LL2193;} else{ goto
_LL2194;} _LL2194: if(( unsigned int) _temp2162 > 1u?(( struct _tunion_struct*)
_temp2162)->tag == Cyc_Absyn_Label_s_tag: 0){ _LL2267: _temp2266=( struct
_tagged_string*)(( struct Cyc_Absyn_Label_s_struct*) _temp2162)->f1; goto
_LL2265; _LL2265: _temp2264=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp2162)->f2; goto _LL2195;} else{ goto _LL2196;} _LL2196: if(( unsigned int)
_temp2162 > 1u?(( struct _tunion_struct*) _temp2162)->tag == Cyc_Absyn_Do_s_tag:
0){ _LL2275: _temp2274=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*)
_temp2162)->f1; goto _LL2269; _LL2269: _temp2268=( struct _tuple3)(( struct Cyc_Absyn_Do_s_struct*)
_temp2162)->f2; _LL2273: _temp2272= _temp2268.f1; goto _LL2271; _LL2271:
_temp2270= _temp2268.f2; goto _LL2197;} else{ goto _LL2198;} _LL2198: if((
unsigned int) _temp2162 > 1u?(( struct _tunion_struct*) _temp2162)->tag == Cyc_Absyn_TryCatch_s_tag:
0){ _LL2279: _temp2278=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp2162)->f1; goto _LL2277; _LL2277: _temp2276=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp2162)->f2; goto _LL2199;} else{ goto
_LL2200;} _LL2200: if(( unsigned int) _temp2162 > 1u?(( struct _tunion_struct*)
_temp2162)->tag == Cyc_Absyn_Region_s_tag: 0){ _LL2285: _temp2284=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_Region_s_struct*) _temp2162)->f1; goto _LL2283; _LL2283:
_temp2282=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Region_s_struct*)
_temp2162)->f2; goto _LL2281; _LL2281: _temp2280=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Region_s_struct*) _temp2162)->f3; goto _LL2201;} else{ goto
_LL2163;} _LL2165: return; _LL2167: Cyc_Toc_exp_to_c( nv, _temp2202); return;
_LL2169: Cyc_Toc_stmt_to_c( nv, _temp2206); s= _temp2204; continue; _LL2171: {
struct Cyc_Core_Opt* topt= 0; if( _temp2208 != 0){ topt=({ struct Cyc_Core_Opt*
_temp2286=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2286->v=( void*) Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Core_Opt* _temp2288=({
struct Cyc_Absyn_Exp* _temp2287= _temp2208; if( _temp2287 == 0){ _throw(
Null_Exception);} _temp2287->topt;}); if( _temp2288 == 0){ _throw(
Null_Exception);} _temp2288->v;})); _temp2286;}); Cyc_Toc_exp_to_c( nv,( struct
Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2289= _temp2208; if( _temp2289 == 0){
_throw( Null_Exception);} _temp2289;}));} if( s->try_depth > 0){ if( topt != 0){
struct _tuple1* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt* retn_stmt= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_Absyn_var_exp( x, 0), 0);( void*)( s->r=( void*)((
void*)( Cyc_Absyn_declare_stmt( x,( void*)({ struct Cyc_Core_Opt* _temp2290=
topt; if( _temp2290 == 0){ _throw( Null_Exception);} _temp2290->v;}), _temp2208,
Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( s->try_depth), retn_stmt, 0), 0))->r));}
else{ Cyc_Toc_do_npop_before( s->try_depth, s);}} return;} _LL2173: Cyc_Toc_exp_to_c(
nv, _temp2214); Cyc_Toc_stmt_to_c( nv, _temp2212); s= _temp2210; continue;
_LL2175: Cyc_Toc_exp_to_c( nv, _temp2222); Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env(
nv), _temp2216); return; _LL2177: if( nv->break_lab != 0){( void*)( s->r=( void*)((
void*)( Cyc_Absyn_goto_stmt(( struct _tagged_string*)({ struct Cyc_Core_Opt*
_temp2291= nv->break_lab; if( _temp2291 == 0){ _throw( Null_Exception);}
_temp2291->v;}), 0))->r));}{ int dest_depth= _temp2224 == 0? 0:({ struct Cyc_Absyn_Stmt*
_temp2292= _temp2224; if( _temp2292 == 0){ _throw( Null_Exception);} _temp2292->try_depth;});
Cyc_Toc_do_npop_before( s->try_depth - dest_depth, s); return;} _LL2179: if( nv->continue_lab
!= 0){( void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct
_tagged_string*)({ struct Cyc_Core_Opt* _temp2293= nv->continue_lab; if(
_temp2293 == 0){ _throw( Null_Exception);} _temp2293->v;}), 0))->r));} Cyc_Toc_do_npop_before(
s->try_depth -({ struct Cyc_Absyn_Stmt* _temp2294= _temp2226; if( _temp2294 == 0){
_throw( Null_Exception);} _temp2294->try_depth;}), s); return; _LL2181: Cyc_Toc_do_npop_before(
s->try_depth -({ struct Cyc_Absyn_Stmt* _temp2295= _temp2228; if( _temp2295 == 0){
_throw( Null_Exception);} _temp2295->try_depth;}), s); return; _LL2183: Cyc_Toc_exp_to_c(
nv, _temp2246); Cyc_Toc_exp_to_c( nv, _temp2244); Cyc_Toc_exp_to_c( nv,
_temp2238); Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2232); return;
_LL2185: Cyc_Toc_xlate_switch( nv, s, _temp2250, _temp2248); return; _LL2187:
if( nv->fallthru_info == 0){( void) _throw((( struct _xtunion_struct*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp2296=(
char*)"fallthru in unexpected place"; struct _tagged_string _temp2297; _temp2297.curr=
_temp2296; _temp2297.base= _temp2296; _temp2297.last_plus_one= _temp2296 + 29;
_temp2297;})));}{ struct _tuple8 _temp2301; struct Cyc_Dict_Dict* _temp2302;
struct Cyc_List_List* _temp2304; struct _tagged_string* _temp2306; struct
_tuple8* _temp2299=( struct _tuple8*)({ struct Cyc_Core_Opt* _temp2298= nv->fallthru_info;
if( _temp2298 == 0){ _throw( Null_Exception);} _temp2298->v;}); _temp2301=*
_temp2299; _LL2307: _temp2306= _temp2301.f1; goto _LL2305; _LL2305: _temp2304=
_temp2301.f2; goto _LL2303; _LL2303: _temp2302= _temp2301.f3; goto _LL2300;
_LL2300: { struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_goto_stmt( _temp2306, 0); Cyc_Toc_do_npop_before(
s->try_depth -(({ struct Cyc_Absyn_Switch_clause** _temp2308= _temp2252; if(
_temp2308 == 0){ _throw( Null_Exception);}* _temp2308;})->body)->try_depth, s2);{
struct Cyc_List_List* vs2=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_rev)( _temp2304); struct Cyc_List_List* es2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( _temp2254); for( 0; vs2 != 0; vs2=({
struct Cyc_List_List* _temp2309= vs2; if( _temp2309 == 0){ _throw(
Null_Exception);} _temp2309->tl;}), es2=({ struct Cyc_List_List* _temp2310= es2;
if( _temp2310 == 0){ _throw( Null_Exception);} _temp2310->tl;})){ Cyc_Toc_exp_to_c(
nv,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp2311= es2; if(
_temp2311 == 0){ _throw( Null_Exception);} _temp2311->hd;})); s2= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt((( struct Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict* d,
struct _tuple1* key)) Cyc_Dict_lookup)( _temp2302,( struct _tuple1*)({ struct
Cyc_List_List* _temp2312= vs2; if( _temp2312 == 0){ _throw( Null_Exception);}
_temp2312->hd;})),( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp2313=
es2; if( _temp2313 == 0){ _throw( Null_Exception);} _temp2313->hd;}), 0), s2, 0);}(
void*)( s->r=( void*)(( void*) s2->r)); return;}}} _LL2189:{ void* _temp2314=(
void*) _temp2258->r; int _temp2322; struct Cyc_Absyn_Exp* _temp2324; struct Cyc_Core_Opt*
_temp2326; struct Cyc_Core_Opt* _temp2328; struct Cyc_Absyn_Pat* _temp2330;
struct Cyc_Absyn_Vardecl* _temp2332; _LL2316: if((( struct _tunion_struct*)
_temp2314)->tag == Cyc_Absyn_Let_d_tag){ _LL2331: _temp2330=( struct Cyc_Absyn_Pat*)((
struct Cyc_Absyn_Let_d_struct*) _temp2314)->f1; goto _LL2329; _LL2329: _temp2328=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp2314)->f2; goto
_LL2327; _LL2327: _temp2326=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp2314)->f3; goto _LL2325; _LL2325: _temp2324=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Let_d_struct*) _temp2314)->f4; goto _LL2323; _LL2323: _temp2322=(
int)(( struct Cyc_Absyn_Let_d_struct*) _temp2314)->f5; goto _LL2317;} else{ goto
_LL2318;} _LL2318: if((( struct _tunion_struct*) _temp2314)->tag == Cyc_Absyn_Var_d_tag){
_LL2333: _temp2332=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp2314)->f1; goto _LL2319;} else{ goto _LL2320;} _LL2320: goto _LL2321;
_LL2317: { int var_case= 0; struct Cyc_Absyn_Vardecl* vdopt= 0;{ void* _temp2334=(
void*) _temp2330->r; struct Cyc_Absyn_Vardecl* _temp2340; _LL2336: if((
unsigned int) _temp2334 > 2u?(( struct _tunion_struct*) _temp2334)->tag == Cyc_Absyn_Var_p_tag:
0){ _LL2341: _temp2340=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_p_struct*)
_temp2334)->f1; goto _LL2337;} else{ goto _LL2338;} _LL2338: goto _LL2339;
_LL2337: var_case= 1; vdopt=( struct Cyc_Absyn_Vardecl*) _temp2340; _temp2340->initializer=(
struct Cyc_Absyn_Exp*) _temp2324;( void*)( _temp2258->r=( void*)(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2342=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2342[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2343; _temp2343.tag= Cyc_Absyn_Var_d_tag; _temp2343.f1= _temp2340;
_temp2343;}); _temp2342;}))); goto _LL2335; _LL2339: goto _LL2335; _LL2335:;}
if( var_case){ _temp2332=( struct Cyc_Absyn_Vardecl*)({ struct Cyc_Absyn_Vardecl*
_temp2344= vdopt; if( _temp2344 == 0){ _throw( Null_Exception);} _temp2344;});
goto _LL2319;}( void*)( s->r=( void*)(( void*)( Cyc_Toc_letdecl_to_c( nv,
_temp2330, _temp2326, _temp2324, _temp2322, _temp2256))->r)); goto _LL2315;}
_LL2319: { struct _tuple1* c_name= _temp2332->name;( void*)( _temp2332->type=(
void*) Cyc_Toc_typ_to_c_array(( void*) _temp2332->type));{ struct Cyc_Toc_Env*
nv2= Cyc_Toc_add_varmap( nv, c_name, Cyc_Absyn_varb_exp( c_name,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp2358=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp2358[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp2359; _temp2359.tag= Cyc_Absyn_Local_b_tag;
_temp2359.f1= _temp2332; _temp2359;}); _temp2358;}), 0)); Cyc_Toc_stmt_to_c( nv2,
_temp2256); if( _temp2332->initializer != 0){ struct Cyc_Absyn_Exp* init=(
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2357= _temp2332->initializer;
if( _temp2357 == 0){ _throw( Null_Exception);} _temp2357;}); void* _temp2345=(
void*) init->r; struct Cyc_Absyn_Exp* _temp2351; struct Cyc_Absyn_Exp* _temp2353;
struct Cyc_Absyn_Vardecl* _temp2355; _LL2347: if((( struct _tunion_struct*)
_temp2345)->tag == Cyc_Absyn_Comprehension_e_tag){ _LL2356: _temp2355=( struct
Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*) _temp2345)->f1;
goto _LL2354; _LL2354: _temp2353=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp2345)->f2; goto _LL2352; _LL2352: _temp2351=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp2345)->f3; goto _LL2348;} else{
goto _LL2349;} _LL2349: goto _LL2350; _LL2348: _temp2332->initializer= 0;
_temp2256= Cyc_Toc_init_comprehension( nv, Cyc_Absyn_var_exp( c_name, 0),
_temp2355, _temp2353, _temp2351, Cyc_Absyn_new_stmt(( void*) _temp2256->r, 0), 0);
goto _LL2346; _LL2350: Cyc_Toc_exp_to_c( nv, init); goto _LL2346; _LL2346:;}
goto _LL2315;}} _LL2321:(( void(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp2360=( char*)"bad nested declaration within function";
struct _tagged_string _temp2361; _temp2361.curr= _temp2360; _temp2361.base=
_temp2360; _temp2361.last_plus_one= _temp2360 + 39; _temp2361;})); goto _LL2315;
_LL2315:;} return; _LL2191:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ char* _temp2362=( char*)"cut"; struct _tagged_string
_temp2363; _temp2363.curr= _temp2362; _temp2363.base= _temp2362; _temp2363.last_plus_one=
_temp2362 + 4; _temp2363;})); return; _LL2193:(( void(*)( struct _tagged_string
s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char* _temp2364=( char*)"splice";
struct _tagged_string _temp2365; _temp2365.curr= _temp2364; _temp2365.base=
_temp2364; _temp2365.last_plus_one= _temp2364 + 7; _temp2365;})); return;
_LL2195: s= _temp2264; continue; _LL2197: Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env(
nv), _temp2274); Cyc_Toc_exp_to_c( nv, _temp2272); return; _LL2199: { struct
_tuple1* h_var= Cyc_Toc_temp_var(); struct _tuple1* e_var= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* h_exp= Cyc_Absyn_var_exp( h_var, 0); struct Cyc_Absyn_Exp*
e_exp= Cyc_Absyn_var_exp( e_var, 0); void* h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp);
void* e_typ= Cyc_Toc_typ_to_c( Cyc_Absyn_exn_typ); struct Cyc_Absyn_Exp*
pushhandler_exp= Cyc_Absyn_var_exp(({ struct _tuple1* _temp2381=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp2381->f1= Cyc_Toc_abs_ns; _temp2381->f2=
Cyc_Toc__push_handler_sp; _temp2381;}), 0); struct Cyc_Absyn_Exp* setjmp_exp=
Cyc_Absyn_var_exp(({ struct _tuple1* _temp2380=( struct _tuple1*) GC_malloc(
sizeof( struct _tuple1)); _temp2380->f1= Cyc_Toc_abs_ns; _temp2380->f2= Cyc_Toc_setjmp_sp;
_temp2380;}), 0); struct Cyc_Absyn_Exp* pophandler_exp= Cyc_Absyn_var_exp(({
struct _tuple1* _temp2379=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp2379->f1= Cyc_Toc_abs_ns; _temp2379->f2= Cyc_Toc__pop_handler_sp; _temp2379;}),
0); e_exp->topt=({ struct Cyc_Core_Opt* _temp2366=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2366->v=( void*) e_typ; _temp2366;});
nv= Cyc_Toc_add_varmap( nv, e_var, e_exp); Cyc_Toc_stmt_to_c( nv, _temp2278);{
struct Cyc_Absyn_Stmt* tryandpop_stmt= Cyc_Absyn_seq_stmt( _temp2278, Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp( pophandler_exp, 0, 0), 0), 0); struct _tuple1* x= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp( x, 0); struct Cyc_Absyn_Vardecl*
x_vd= Cyc_Absyn_new_vardecl( x, Cyc_Absyn_exn_typ, 0); xexp->topt=({ struct Cyc_Core_Opt*
_temp2367=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2367->v=( void*) Cyc_Absyn_exn_typ; _temp2367;});{ struct Cyc_Absyn_Pat* p=({
struct Cyc_Absyn_Pat* _temp2375=( struct Cyc_Absyn_Pat*) GC_malloc( sizeof(
struct Cyc_Absyn_Pat)); _temp2375->r=( void*)(( void*)({ struct Cyc_Absyn_Var_p_struct*
_temp2377=( struct Cyc_Absyn_Var_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct));
_temp2377[ 0]=({ struct Cyc_Absyn_Var_p_struct _temp2378; _temp2378.tag= Cyc_Absyn_Var_p_tag;
_temp2378.f1= x_vd; _temp2378;}); _temp2377;})); _temp2375->topt=({ struct Cyc_Core_Opt*
_temp2376=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2376->v=( void*) Cyc_Absyn_exn_typ; _temp2376;}); _temp2375->loc= 0;
_temp2375;}); struct Cyc_Absyn_Exp* te= Cyc_Absyn_throw_exp( xexp, 0); te->topt=({
struct Cyc_Core_Opt* _temp2368=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2368->v=( void*)(( void*) Cyc_Absyn_VoidType); _temp2368;});{
struct Cyc_Absyn_Stmt* rs= Cyc_Absyn_exp_stmt( te, 0); struct Cyc_Absyn_Switch_clause*
default_case=({ struct Cyc_Absyn_Switch_clause* _temp2372=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2372->pattern= p;
_temp2372->pat_vars=({ struct Cyc_Core_Opt* _temp2373=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2373->v=( void*)({ struct Cyc_List_List*
_temp2374=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2374->hd=( void*) x_vd; _temp2374->tl= 0; _temp2374;}); _temp2373;});
_temp2372->where_clause= 0; _temp2372->body= rs; _temp2372->loc= 0; _temp2372;});
struct Cyc_Absyn_Stmt* handler_stmt= Cyc_Absyn_switch_stmt( e_exp,(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp2276,({
struct Cyc_List_List* _temp2371=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2371->hd=( void*) default_case; _temp2371->tl= 0;
_temp2371;})), 0); Cyc_Toc_stmt_to_c( nv, handler_stmt);{ struct Cyc_Absyn_Exp*
setjmp_call= Cyc_Absyn_cast_exp( e_typ, Cyc_Absyn_fncall_exp( setjmp_exp,({
struct Cyc_List_List* _temp2370=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2370->hd=( void*) Cyc_Absyn_structmember_exp( h_exp,
Cyc_Toc_handler_sp, 0); _temp2370->tl= 0; _temp2370;}), 0), 0);( void*)( s->r=(
void*)(( void*)( Cyc_Absyn_declare_stmt( h_var, h_typ, 0, Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( pushhandler_exp,({ struct Cyc_List_List*
_temp2369=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2369->hd=( void*) Cyc_Absyn_address_exp( h_exp, 0); _temp2369->tl= 0;
_temp2369;}), 0), 0), Cyc_Absyn_declare_stmt( e_var, e_typ,( struct Cyc_Absyn_Exp*)
setjmp_call, Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_prim1_exp(( void*) Cyc_Absyn_Not,
e_exp, 0), tryandpop_stmt, handler_stmt, 0), 0), 0), 0))->r)); return;}}}}}
_LL2201: { void* rh_struct_typ= Cyc_Absyn_strct( Cyc_Toc__region_handle_sp);
void* rh_struct_ptr_typ= Cyc_Absyn_cstar_typ( rh_struct_typ, Cyc_Toc_mt_tq);
void* h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp); void* e_typ= Cyc_Toc_typ_to_c(
Cyc_Absyn_exn_typ); struct _tuple1* rh_var= Cyc_Toc_temp_var(); struct _tuple1*
x_var= _temp2282->name; struct _tuple1* h_var= Cyc_Toc_temp_var(); struct
_tuple1* e_var= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* rh_exp= Cyc_Absyn_var_exp(
rh_var, 0); struct Cyc_Absyn_Exp* x_exp= Cyc_Absyn_varb_exp( x_var,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp2390=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp2390[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp2391; _temp2391.tag= Cyc_Absyn_Local_b_tag;
_temp2391.f1= _temp2282; _temp2391;}); _temp2390;}), 0); struct Cyc_Absyn_Exp*
h_exp= Cyc_Absyn_var_exp( h_var, 0); struct Cyc_Absyn_Exp* e_exp= Cyc_Absyn_var_exp(
e_var, 0); struct Cyc_Absyn_Exp* pushhandler_exp= Cyc_Absyn_var_exp(({ struct
_tuple1* _temp2389=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp2389->f1= Cyc_Toc_abs_ns; _temp2389->f2= Cyc_Toc__push_handler_sp;
_temp2389;}), 0); struct Cyc_Absyn_Exp* setjmp_exp= Cyc_Absyn_var_exp(({ struct
_tuple1* _temp2388=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp2388->f1= Cyc_Toc_abs_ns; _temp2388->f2= Cyc_Toc_setjmp_sp; _temp2388;}), 0);
struct Cyc_Absyn_Exp* pophandler_exp= Cyc_Absyn_var_exp(({ struct _tuple1*
_temp2387=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2387->f1=
Cyc_Toc_abs_ns; _temp2387->f2= Cyc_Toc__pop_handler_sp; _temp2387;}), 0); struct
Cyc_Absyn_Exp* freeregion_exp= Cyc_Absyn_var_exp(({ struct _tuple1* _temp2386=(
struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2386->f1= Cyc_Toc_abs_ns;
_temp2386->f2= Cyc_Toc__free_region_sp; _temp2386;}), 0); struct Cyc_Absyn_Exp*
newregion_exp= Cyc_Absyn_var_exp(({ struct _tuple1* _temp2385=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp2385->f1= Cyc_Toc_abs_ns; _temp2385->f2=
Cyc_Toc__new_region_sp; _temp2385;}), 0); struct Cyc_Absyn_Exp* addr_rh_exp= Cyc_Absyn_address_exp(
rh_exp, 0); struct Cyc_Absyn_Exp* newregion_call= Cyc_Absyn_fncall_exp(
newregion_exp, 0, 0); Cyc_Toc_stmt_to_c( Cyc_Toc_add_varmap( nv, x_var, x_exp),
_temp2280);{ struct Cyc_Absyn_Stmt* free_region_stmt= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
freeregion_exp,({ struct Cyc_List_List* _temp2384=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2384->hd=( void*) addr_rh_exp;
_temp2384->tl= 0; _temp2384;}), 0), 0); struct Cyc_Absyn_Stmt* tryandpop_stmt=
Cyc_Absyn_seq_stmt( _temp2280, Cyc_Absyn_seq_stmt( free_region_stmt, Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp( pophandler_exp, 0, 0), 0), 0), 0); struct Cyc_Absyn_Stmt*
rethrow_stmt= Cyc_Absyn_seq_stmt( free_region_stmt, Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp(
e_exp), 0), 0); struct Cyc_Absyn_Stmt* if_stmt= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_prim1_exp((
void*) Cyc_Absyn_Not, e_exp, 0), tryandpop_stmt, rethrow_stmt, 0); struct Cyc_Absyn_Exp*
setjmp_call= Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c( Cyc_Absyn_exn_typ), Cyc_Absyn_fncall_exp(
setjmp_exp,({ struct Cyc_List_List* _temp2383=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2383->hd=( void*) Cyc_Absyn_structmember_exp(
h_exp, Cyc_Toc_handler_sp, 0); _temp2383->tl= 0; _temp2383;}), 0), 0); struct
Cyc_Absyn_Stmt* push_stmt= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
pushhandler_exp,({ struct Cyc_List_List* _temp2382=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2382->hd=( void*) Cyc_Absyn_address_exp(
h_exp, 0); _temp2382->tl= 0; _temp2382;}), 0), 0);( void*)( s->r=( void*)(( void*)(
Cyc_Absyn_declare_stmt( rh_var, rh_struct_typ,( struct Cyc_Absyn_Exp*)
newregion_call, Cyc_Absyn_declare_stmt( x_var, rh_struct_ptr_typ,( struct Cyc_Absyn_Exp*)
addr_rh_exp, Cyc_Absyn_declare_stmt( h_var, h_typ, 0, Cyc_Absyn_seq_stmt(
push_stmt, Cyc_Absyn_declare_stmt( e_var, e_typ,( struct Cyc_Absyn_Exp*)
setjmp_call, if_stmt, 0), 0), 0), 0), 0))->r)); return;}} _LL2163:;}} static
void Cyc_Toc_fndecl_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Fndecl* f){ f->tvs=
0; f->effect= 0;( void*)( f->ret_type=( void*) Cyc_Toc_typ_to_c(( void*) f->ret_type));{
struct Cyc_List_List* args= f->args; for( 0; args != 0; args=({ struct Cyc_List_List*
_temp2392= args; if( _temp2392 == 0){ _throw( Null_Exception);} _temp2392->tl;})){(*((
struct _tuple18*)({ struct Cyc_List_List* _temp2393= args; if( _temp2393 == 0){
_throw( Null_Exception);} _temp2393->hd;}))).f3= Cyc_Toc_typ_to_c((*(( struct
_tuple18*)({ struct Cyc_List_List* _temp2394= args; if( _temp2394 == 0){ _throw(
Null_Exception);} _temp2394->hd;}))).f3);{ struct _tuple1* x=({ struct _tuple1*
_temp2395=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2395->f1=(
void*) Cyc_Absyn_Loc_n; _temp2395->f2=(*(( struct _tuple18*)({ struct Cyc_List_List*
_temp2396= args; if( _temp2396 == 0){ _throw( Null_Exception);} _temp2396->hd;}))).f1;
_temp2395;}); nv= Cyc_Toc_add_varmap( nv, x, Cyc_Absyn_var_exp( x, 0));}}} Cyc_Toc_stmt_to_c(
Cyc_Toc_clear_toplevel( nv), f->body);} static void* Cyc_Toc_scope_to_c( void* s){
void* _temp2397= s; _LL2399: if(( int) _temp2397 == Cyc_Absyn_Abstract){ goto
_LL2400;} else{ goto _LL2401;} _LL2401: if(( int) _temp2397 == Cyc_Absyn_ExternC){
goto _LL2402;} else{ goto _LL2403;} _LL2403: goto _LL2404; _LL2400: return( void*)
Cyc_Absyn_Public; _LL2402: return( void*) Cyc_Absyn_Extern; _LL2404: return s;
_LL2398:;} struct Cyc_Core_Opt* Cyc_Toc_structs_so_far= 0; static void Cyc_Toc_structdecl_to_c(
struct Cyc_Absyn_Structdecl* s){ struct _tuple1* n=( struct _tuple1*)({ struct
Cyc_Core_Opt* _temp2417= s->name; if( _temp2417 == 0){ _throw( Null_Exception);}
_temp2417->v;}); if( Cyc_Toc_structs_so_far == 0){ Cyc_Toc_structs_so_far=({
struct Cyc_Core_Opt* _temp2405=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2405->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp2405;});}{ int seen_defn_before; struct Cyc_Core_Opt* dopt=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp2416= Cyc_Toc_structs_so_far;
if( _temp2416 == 0){ _throw( Null_Exception);} _temp2416->v;}),(* n).f2); if(
dopt == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp2406= Cyc_Toc_structs_so_far; if( _temp2406 == 0){ _throw( Null_Exception);}
_temp2406->v=( void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key, struct Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp2407= Cyc_Toc_structs_so_far;
if( _temp2407 == 0){ _throw( Null_Exception);} _temp2407->v;}),(* n).f2, s);});}
else{ if((( struct Cyc_Absyn_Structdecl*)({ struct Cyc_Core_Opt* _temp2408= dopt;
if( _temp2408 == 0){ _throw( Null_Exception);} _temp2408->v;}))->fields == 0){(
struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp2409= Cyc_Toc_structs_so_far;
if( _temp2409 == 0){ _throw( Null_Exception);} _temp2409->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct
Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)({ struct
Cyc_Core_Opt* _temp2410= Cyc_Toc_structs_so_far; if( _temp2410 == 0){ _throw(
Null_Exception);} _temp2410->v;}),(* n).f2, s);}); seen_defn_before= 0;} else{
seen_defn_before= 1;}}( void*)( s->sc=( void*)(((( void*) s->sc ==( void*) Cyc_Absyn_Extern?
1:( void*) s->sc ==( void*) Cyc_Absyn_Static)? 1:( void*) s->sc ==( void*) Cyc_Absyn_ExternC)?(
void*) Cyc_Absyn_Public: Cyc_Toc_scope_to_c(( void*) s->sc))); s->tvs= 0; s->name=({
struct Cyc_Core_Opt* _temp2411=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2411->v=( void*) n; _temp2411;}); if( s->fields != 0){ if(
seen_defn_before){ s->fields= 0;} else{ struct Cyc_List_List* fields=( struct
Cyc_List_List*)({ struct Cyc_Core_Opt* _temp2415= s->fields; if( _temp2415 == 0){
_throw( Null_Exception);} _temp2415->v;}); for( 0; fields != 0; fields=({ struct
Cyc_List_List* _temp2412= fields; if( _temp2412 == 0){ _throw( Null_Exception);}
_temp2412->tl;})){( void*)((( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp2413= fields; if( _temp2413 == 0){ _throw( Null_Exception);} _temp2413->hd;}))->type=(
void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp2414= fields; if( _temp2414 == 0){ _throw( Null_Exception);}
_temp2414->hd;}))->type));}}}}} struct Cyc_Core_Opt* Cyc_Toc_unions_so_far= 0;
static void Cyc_Toc_uniondecl_to_c( struct Cyc_Absyn_Uniondecl* u){ struct
_tuple1* n=( struct _tuple1*)({ struct Cyc_Core_Opt* _temp2430= u->name; if(
_temp2430 == 0){ _throw( Null_Exception);} _temp2430->v;}); if( Cyc_Toc_unions_so_far
== 0){ Cyc_Toc_unions_so_far=({ struct Cyc_Core_Opt* _temp2418=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2418->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); _temp2418;});}{ int seen_defn_before; struct Cyc_Core_Opt*
dopt=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string*
key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp2429= Cyc_Toc_unions_so_far; if( _temp2429 == 0){ _throw( Null_Exception);}
_temp2429->v;}),(* n).f2); if( dopt == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)({
struct Cyc_Core_Opt* _temp2419= Cyc_Toc_unions_so_far; if( _temp2419 == 0){
_throw( Null_Exception);} _temp2419->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp2420= Cyc_Toc_unions_so_far; if( _temp2420 == 0){ _throw( Null_Exception);}
_temp2420->v;}),(* n).f2, u);});} else{ if((( struct Cyc_Absyn_Uniondecl*)({
struct Cyc_Core_Opt* _temp2421= dopt; if( _temp2421 == 0){ _throw(
Null_Exception);} _temp2421->v;}))->fields == 0){( struct Cyc_Dict_Dict*)({
struct Cyc_Core_Opt* _temp2422= Cyc_Toc_unions_so_far; if( _temp2422 == 0){
_throw( Null_Exception);} _temp2422->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp2423= Cyc_Toc_unions_so_far; if( _temp2423 == 0){ _throw( Null_Exception);}
_temp2423->v;}),(* n).f2, u);}); seen_defn_before= 0;} else{ seen_defn_before= 1;}}(
void*)( u->sc=( void*)(((( void*) u->sc ==( void*) Cyc_Absyn_Extern? 1:( void*)
u->sc ==( void*) Cyc_Absyn_Static)? 1:( void*) u->sc ==( void*) Cyc_Absyn_ExternC)?(
void*) Cyc_Absyn_Public: Cyc_Toc_scope_to_c(( void*) u->sc))); u->tvs= 0; u->name=({
struct Cyc_Core_Opt* _temp2424=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2424->v=( void*) n; _temp2424;}); if( u->fields != 0){ if(
seen_defn_before){ u->fields= 0;} else{ struct Cyc_List_List* fields=( struct
Cyc_List_List*)({ struct Cyc_Core_Opt* _temp2428= u->fields; if( _temp2428 == 0){
_throw( Null_Exception);} _temp2428->v;}); for( 0; fields != 0; fields=({ struct
Cyc_List_List* _temp2425= fields; if( _temp2425 == 0){ _throw( Null_Exception);}
_temp2425->tl;})){( void*)((( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp2426= fields; if( _temp2426 == 0){ _throw( Null_Exception);} _temp2426->hd;}))->type=(
void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp2427= fields; if( _temp2427 == 0){ _throw( Null_Exception);}
_temp2427->hd;}))->type));}}}}} static struct Cyc_Core_Opt* Cyc_Toc_tunions_so_far=
0; static struct Cyc_List_List* Cyc_Toc_tuniondecl_to_c( struct Cyc_Absyn_Tuniondecl*
ed, struct Cyc_List_List* res){ struct _tuple1* n=( struct _tuple1*)({ struct
Cyc_Core_Opt* _temp2471= ed->name; if( _temp2471 == 0){ _throw( Null_Exception);}
_temp2471->v;}); if( Cyc_Toc_tunions_so_far == 0){ Cyc_Toc_tunions_so_far=({
struct Cyc_Core_Opt* _temp2431=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2431->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp2431;});}{ int seen_before; int seen_defn_before; struct Cyc_Core_Opt* dopt=((
struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key))
Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp2470=
Cyc_Toc_tunions_so_far; if( _temp2470 == 0){ _throw( Null_Exception);} _temp2470->v;}),(*
n).f2); if( dopt == 0){ seen_before= 0; seen_defn_before= 0;( struct Cyc_Dict_Dict*)({
struct Cyc_Core_Opt* _temp2432= Cyc_Toc_tunions_so_far; if( _temp2432 == 0){
_throw( Null_Exception);} _temp2432->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Tuniondecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp2433= Cyc_Toc_tunions_so_far; if( _temp2433 == 0){ _throw( Null_Exception);}
_temp2433->v;}),(* n).f2, ed);});} else{ seen_before= 1; if((( struct Cyc_Absyn_Tuniondecl*)({
struct Cyc_Core_Opt* _temp2434= dopt; if( _temp2434 == 0){ _throw(
Null_Exception);} _temp2434->v;}))->fields == 0){ seen_defn_before= 0;( struct
Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp2435= Cyc_Toc_tunions_so_far; if(
_temp2435 == 0){ _throw( Null_Exception);} _temp2435->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Tuniondecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp2436= Cyc_Toc_tunions_so_far; if( _temp2436 == 0){ _throw( Null_Exception);}
_temp2436->v;}),(* n).f2, ed);});} else{ seen_defn_before= 1;}} if( !
seen_before){ struct Cyc_Absyn_Typedefdecl* tdef=({ struct Cyc_Absyn_Typedefdecl*
_temp2440=( struct Cyc_Absyn_Typedefdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Typedefdecl));
_temp2440->name= n; _temp2440->tvs= 0; _temp2440->defn=( void*) Cyc_Absyn_void_star_typ();
_temp2440;}); res=({ struct Cyc_List_List* _temp2437=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2437->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Typedef_d_struct* _temp2438=( struct Cyc_Absyn_Typedef_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct)); _temp2438[ 0]=({ struct
Cyc_Absyn_Typedef_d_struct _temp2439; _temp2439.tag= Cyc_Absyn_Typedef_d_tag;
_temp2439.f1= tdef; _temp2439;}); _temp2438;}), 0); _temp2437->tl= res;
_temp2437;});} if( ! seen_defn_before){ int tag_count= 0; int box_count= 0;
struct Cyc_List_List* fields= ed->fields == 0? 0:( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp2469= ed->fields; if( _temp2469 == 0){ _throw(
Null_Exception);} _temp2469->v;}); for( 0; fields != 0; fields=({ struct Cyc_List_List*
_temp2441= fields; if( _temp2441 == 0){ _throw( Null_Exception);} _temp2441->tl;})){
struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List*
_temp2468= fields; if( _temp2468 == 0){ _throw( Null_Exception);} _temp2468->hd;});
if( f->typs == 0){ struct Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp((
tag_count ++), 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name,
Cyc_Absyn_uint_t, eopt); vd->tq=({ struct Cyc_Absyn_Tqual* _temp2442=( struct
Cyc_Absyn_Tqual*) GC_malloc_atomic( sizeof( struct Cyc_Absyn_Tqual)); _temp2442->q_const=
1; _temp2442->q_volatile= 0; _temp2442->q_restrict= 0; _temp2442;}); res=({
struct Cyc_List_List* _temp2443=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2443->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2444=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2444[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2445; _temp2445.tag= Cyc_Absyn_Var_d_tag; _temp2445.f1= vd; _temp2445;});
_temp2444;}), 0); _temp2443->tl= res; _temp2443;});} else{ struct Cyc_Absyn_Exp*
eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp(( box_count ++), 0);
struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( Cyc_Toc_collapse_qvar_tag(
f->name,( struct _tagged_string)({ char* _temp2466=( char*)"_tag"; struct
_tagged_string _temp2467; _temp2467.curr= _temp2466; _temp2467.base= _temp2466;
_temp2467.last_plus_one= _temp2466 + 5; _temp2467;})), Cyc_Absyn_sint_t, eopt);
vd->tq=({ struct Cyc_Absyn_Tqual* _temp2446=( struct Cyc_Absyn_Tqual*)
GC_malloc_atomic( sizeof( struct Cyc_Absyn_Tqual)); _temp2446->q_const= 1;
_temp2446->q_volatile= 0; _temp2446->q_restrict= 0; _temp2446;}); res=({ struct
Cyc_List_List* _temp2447=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2447->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp2448=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2448[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2449; _temp2449.tag= Cyc_Absyn_Var_d_tag;
_temp2449.f1= vd; _temp2449;}); _temp2448;}), 0); _temp2447->tl= res; _temp2447;});{
struct Cyc_List_List* fs= 0; int i= 1;{ struct Cyc_List_List* ts= f->typs; for(
0; ts != 0; ts=({ struct Cyc_List_List* _temp2450= ts; if( _temp2450 == 0){
_throw( Null_Exception);} _temp2450->tl;}), i ++){ struct _tagged_string* fname=(
struct _tagged_string*)({ struct _tagged_string* _temp2455=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp2455[ 0]=
xprintf("f%d", i); _temp2455;}); struct Cyc_Absyn_Structfield* f=({ struct Cyc_Absyn_Structfield*
_temp2452=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2452->name= fname; _temp2452->tq=(*(( struct _tuple5*)({ struct Cyc_List_List*
_temp2454= ts; if( _temp2454 == 0){ _throw( Null_Exception);} _temp2454->hd;}))).f1;
_temp2452->type=( void*) Cyc_Toc_typ_to_c_array((*(( struct _tuple5*)({ struct
Cyc_List_List* _temp2453= ts; if( _temp2453 == 0){ _throw( Null_Exception);}
_temp2453->hd;}))).f2); _temp2452->width= 0; _temp2452->attributes= 0; _temp2452;});
fs=({ struct Cyc_List_List* _temp2451=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2451->hd=( void*) f; _temp2451->tl= fs;
_temp2451;});}} fs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fs); fs=({ struct Cyc_List_List* _temp2456=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2456->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp2457=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2457->name= Cyc_Toc_tag_sp; _temp2457->tq= Cyc_Toc_mt_tq; _temp2457->type=(
void*) Cyc_Absyn_sint_t; _temp2457->width= 0; _temp2457->attributes= 0;
_temp2457;}); _temp2456->tl= fs; _temp2456;});{ struct Cyc_Absyn_Structdecl* sd=({
struct Cyc_Absyn_Structdecl* _temp2461=( struct Cyc_Absyn_Structdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp2461->sc=( void*)(( void*) Cyc_Absyn_Public);
_temp2461->name=({ struct Cyc_Core_Opt* _temp2463=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2463->v=( void*) Cyc_Toc_collapse_qvar_tag(
f->name,( struct _tagged_string)({ char* _temp2464=( char*)"_struct"; struct
_tagged_string _temp2465; _temp2465.curr= _temp2464; _temp2465.base= _temp2464;
_temp2465.last_plus_one= _temp2464 + 8; _temp2465;})); _temp2463;}); _temp2461->tvs=
0; _temp2461->fields=({ struct Cyc_Core_Opt* _temp2462=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2462->v=( void*) fs; _temp2462;});
_temp2461->attributes= 0; _temp2461;}); res=({ struct Cyc_List_List* _temp2458=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2458->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2459=(
struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp2459[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp2460; _temp2460.tag= Cyc_Absyn_Struct_d_tag;
_temp2460.f1= sd; _temp2460;}); _temp2459;}), 0); _temp2458->tl= res; _temp2458;});}}}}}
return res;}} static struct Cyc_Core_Opt* Cyc_Toc_xtunions_so_far= 0; static
struct Cyc_List_List* Cyc_Toc_xtuniondecl_to_c( struct Cyc_Absyn_XTuniondecl* xd,
struct Cyc_List_List* res){ struct _tuple1* n= xd->name; if( xd->fields == 0){
void* t= Cyc_Absyn_cstar_typ( Cyc_Absyn_strct( Cyc_Toc__xtunion_struct_sp), Cyc_Toc_mt_tq);
struct Cyc_Absyn_Typedefdecl* td=({ struct Cyc_Absyn_Typedefdecl* _temp2475=(
struct Cyc_Absyn_Typedefdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Typedefdecl));
_temp2475->name= n; _temp2475->tvs= 0; _temp2475->defn=( void*) t; _temp2475;});
res=({ struct Cyc_List_List* _temp2472=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2472->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Typedef_d_struct* _temp2473=( struct Cyc_Absyn_Typedef_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct)); _temp2473[ 0]=({ struct
Cyc_Absyn_Typedef_d_struct _temp2474; _temp2474.tag= Cyc_Absyn_Typedef_d_tag;
_temp2474.f1= td; _temp2474;}); _temp2473;}), 0); _temp2472->tl= res; _temp2472;});}
else{ if( Cyc_Toc_xtunions_so_far == 0){ Cyc_Toc_xtunions_so_far=({ struct Cyc_Core_Opt*
_temp2476=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2476->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp2476;});}{ struct Cyc_List_List* fs= xd->fields; for( 0; fs != 0; fs=({
struct Cyc_List_List* _temp2477= fs; if( _temp2477 == 0){ _throw( Null_Exception);}
_temp2477->tl;})){ struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp2508= fs; if( _temp2508 == 0){ _throw( Null_Exception);}
_temp2508->hd;}); struct _tagged_string* fn=(* f->name).f2; struct Cyc_Absyn_Exp*
sz_exp= Cyc_Absyn_uint_exp(({ struct _tagged_string _temp2507=* fn;(
unsigned int)( _temp2507.last_plus_one - _temp2507.curr);}), 0); struct Cyc_Absyn_ArrayType_struct*
tag_typ=({ struct Cyc_Absyn_ArrayType_struct* _temp2505=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp2505[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp2506; _temp2506.tag= Cyc_Absyn_ArrayType_tag;
_temp2506.f1=( void*) Cyc_Absyn_uchar_t; _temp2506.f2= Cyc_Toc_mt_tq; _temp2506.f3=(
struct Cyc_Absyn_Exp*) sz_exp; _temp2506;}); _temp2505;}); struct Cyc_Absyn_Exp*
initopt= 0; if(( void*) xd->sc !=( void*) Cyc_Absyn_Extern){ initopt=( struct
Cyc_Absyn_Exp*) Cyc_Absyn_string_exp(* fn, 0);}{ struct Cyc_Absyn_Vardecl*
tag_decl= Cyc_Absyn_new_vardecl( Cyc_Toc_collapse_qvar_tag( f->name,( struct
_tagged_string)({ char* _temp2503=( char*)"_tag"; struct _tagged_string
_temp2504; _temp2504.curr= _temp2503; _temp2504.base= _temp2503; _temp2504.last_plus_one=
_temp2503 + 5; _temp2504;})),( void*) tag_typ, initopt);( void*)( tag_decl->sc=(
void*)(( void*) xd->sc)); res=({ struct Cyc_List_List* _temp2478=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2478->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Var_d_struct* _temp2479=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2479[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2480; _temp2480.tag= Cyc_Absyn_Var_d_tag; _temp2480.f1= tag_decl; _temp2480;});
_temp2479;}), 0); _temp2478->tl= res; _temp2478;});{ int seen_before; int
seen_defn_before; struct Cyc_Core_Opt* dopt=(( struct Cyc_Core_Opt*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)({
struct Cyc_Core_Opt* _temp2502= Cyc_Toc_xtunions_so_far; if( _temp2502 == 0){
_throw( Null_Exception);} _temp2502->v;}), fn); if( dopt == 0){ seen_before= 0;
seen_defn_before= 0;( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp2481=
Cyc_Toc_xtunions_so_far; if( _temp2481 == 0){ _throw( Null_Exception);}
_temp2481->v=( void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key, struct Cyc_Absyn_XTuniondecl* data)) Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp2482= Cyc_Toc_xtunions_so_far;
if( _temp2482 == 0){ _throw( Null_Exception);} _temp2482->v;}), fn, xd);});}
else{ seen_before= 1; if((( struct Cyc_Absyn_XTuniondecl*)({ struct Cyc_Core_Opt*
_temp2483= dopt; if( _temp2483 == 0){ _throw( Null_Exception);} _temp2483->v;}))->fields
== 0){( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp2484= Cyc_Toc_xtunions_so_far;
if( _temp2484 == 0){ _throw( Null_Exception);} _temp2484->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct
Cyc_Absyn_XTuniondecl* data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)({ struct
Cyc_Core_Opt* _temp2485= Cyc_Toc_xtunions_so_far; if( _temp2485 == 0){ _throw(
Null_Exception);} _temp2485->v;}), fn, xd);}); seen_defn_before= 0;} else{
seen_defn_before= 1;}} if( ! seen_before){ struct Cyc_List_List* fields= 0; int
i= 1;{ struct Cyc_List_List* ts= f->typs; for( 0; ts != 0; ts=({ struct Cyc_List_List*
_temp2486= ts; if( _temp2486 == 0){ _throw( Null_Exception);} _temp2486->tl;}),
i ++){ struct _tagged_string* field_n=( struct _tagged_string*)({ struct
_tagged_string* _temp2491=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp2491[ 0]= xprintf("f%d", i); _temp2491;}); struct Cyc_Absyn_Structfield*
newf=({ struct Cyc_Absyn_Structfield* _temp2488=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2488->name= field_n;
_temp2488->tq=(*(( struct _tuple5*)({ struct Cyc_List_List* _temp2490= ts; if(
_temp2490 == 0){ _throw( Null_Exception);} _temp2490->hd;}))).f1; _temp2488->type=(
void*) Cyc_Toc_typ_to_c_array((*(( struct _tuple5*)({ struct Cyc_List_List*
_temp2489= ts; if( _temp2489 == 0){ _throw( Null_Exception);} _temp2489->hd;}))).f2);
_temp2488->width= 0; _temp2488->attributes= 0; _temp2488;}); fields=({ struct
Cyc_List_List* _temp2487=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2487->hd=( void*) newf; _temp2487->tl= fields; _temp2487;});}} fields=({
struct Cyc_List_List* _temp2492=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2492->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp2493=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2493->name= Cyc_Toc_tag_sp; _temp2493->tq= Cyc_Toc_mt_tq; _temp2493->type=(
void*) Cyc_Absyn_cstar_typ( Cyc_Absyn_uchar_t, Cyc_Toc_mt_tq); _temp2493->width=
0; _temp2493->attributes= 0; _temp2493;}); _temp2492->tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( fields); _temp2492;});{ struct Cyc_Absyn_Structdecl*
strct_decl=({ struct Cyc_Absyn_Structdecl* _temp2497=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp2497->sc=( void*)(( void*)
Cyc_Absyn_Public); _temp2497->name=({ struct Cyc_Core_Opt* _temp2499=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2499->v=( void*)
Cyc_Toc_collapse_qvar_tag( f->name,( struct _tagged_string)({ char* _temp2500=(
char*)"_struct"; struct _tagged_string _temp2501; _temp2501.curr= _temp2500;
_temp2501.base= _temp2500; _temp2501.last_plus_one= _temp2500 + 8; _temp2501;}));
_temp2499;}); _temp2497->tvs= 0; _temp2497->fields=({ struct Cyc_Core_Opt*
_temp2498=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2498->v=( void*) fields; _temp2498;}); _temp2497->attributes= 0; _temp2497;});
res=({ struct Cyc_List_List* _temp2494=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2494->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Struct_d_struct* _temp2495=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp2495[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp2496; _temp2496.tag= Cyc_Absyn_Struct_d_tag;
_temp2496.f1= strct_decl; _temp2496;}); _temp2495;}), 0); _temp2494->tl= res;
_temp2494;});}}}}}}} return res;} static void Cyc_Toc_enumdecl_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Enumdecl* ed){ struct _tuple1* n= ed->name;( void*)( ed->sc=(
void*)(((( void*) ed->sc ==( void*) Cyc_Absyn_Extern? 1:( void*) ed->sc ==( void*)
Cyc_Absyn_Static)? 1:( void*) ed->sc ==( void*) Cyc_Absyn_ExternC)?( void*) Cyc_Absyn_Public:
Cyc_Toc_scope_to_c(( void*) ed->sc)));{ struct Cyc_List_List* fs= ed->fields;
for( 0; fs != 0; fs=({ struct Cyc_List_List* _temp2509= fs; if( _temp2509 == 0){
_throw( Null_Exception);} _temp2509->tl;})){ if((( struct Cyc_Absyn_Enumfield*)({
struct Cyc_List_List* _temp2510= fs; if( _temp2510 == 0){ _throw( Null_Exception);}
_temp2510->hd;}))->tag != 0){ unsigned int i= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2513=(( struct Cyc_Absyn_Enumfield*)({
struct Cyc_List_List* _temp2512= fs; if( _temp2512 == 0){ _throw( Null_Exception);}
_temp2512->hd;}))->tag; if( _temp2513 == 0){ _throw( Null_Exception);} _temp2513;}));((
struct Cyc_Absyn_Enumfield*)({ struct Cyc_List_List* _temp2511= fs; if(
_temp2511 == 0){ _throw( Null_Exception);} _temp2511->hd;}))->tag=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp( i, 0);}}}} static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt,
struct Cyc_Absyn_Exp* e, int exhaust, struct Cyc_Absyn_Stmt* s){ void* t=( void*)({
struct Cyc_Core_Opt* _temp2532= topt; if( _temp2532 == 0){ _throw(
Null_Exception);} _temp2532->v;}); Cyc_Toc_exp_to_c( nv, e);{ struct _tuple1* x=
Cyc_Toc_temp_var(); struct _tagged_string* succ_lab= Cyc_Toc_fresh_label();
struct _tagged_string* fail_lab= exhaust? succ_lab: Cyc_Toc_fresh_label();
struct Cyc_Absyn_Stmt* _temp2516; struct Cyc_List_List* _temp2518; struct Cyc_Toc_Env*
_temp2520; struct _tuple13 _temp2514= Cyc_Toc_xlate_pat( nv, t, Cyc_Absyn_var_exp(
x, 0), Cyc_Absyn_var_exp( x, 0), p, succ_lab, fail_lab, 0); _LL2521: _temp2520=
_temp2514.f1; goto _LL2519; _LL2519: _temp2518= _temp2514.f2; goto _LL2517;
_LL2517: _temp2516= _temp2514.f3; goto _LL2515; _LL2515: Cyc_Toc_stmt_to_c(
_temp2520, s);{ struct Cyc_Absyn_Stmt* succ_stmt= Cyc_Absyn_label_stmt( succ_lab,
s, 0); if( exhaust){ s= Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c( t),( struct
Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp2516, succ_stmt, 0), 0);} else{
struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp( Cyc_Absyn_var_exp(({ struct
_tuple1* _temp2522=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp2522->f1= Cyc_Toc_abs_ns; _temp2522->f2= Cyc_Toc_Match_Exception_sp;
_temp2522;}), 0)); struct Cyc_Absyn_Stmt* fail_stmt= Cyc_Absyn_label_stmt(
fail_lab, Cyc_Absyn_exp_stmt( e3, 0), 0); s= Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c(
t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp2516, Cyc_Absyn_seq_stmt(
fail_stmt, succ_stmt, 0), 0), 0);} for( 0; _temp2518 != 0; _temp2518=({ struct
Cyc_List_List* _temp2523= _temp2518; if( _temp2523 == 0){ _throw( Null_Exception);}
_temp2523->tl;})){ struct _tuple14 _temp2527; void* _temp2528; struct _tuple1*
_temp2530; struct _tuple14* _temp2525=( struct _tuple14*)({ struct Cyc_List_List*
_temp2524= _temp2518; if( _temp2524 == 0){ _throw( Null_Exception);} _temp2524->hd;});
_temp2527=* _temp2525; _LL2531: _temp2530= _temp2527.f1; goto _LL2529; _LL2529:
_temp2528= _temp2527.f2; goto _LL2526; _LL2526: s= Cyc_Absyn_declare_stmt(
_temp2530, _temp2528, 0, s, 0);} return s;}}} static struct _tuple17 Cyc_Toc_decls_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_List_List* ds, int top){ struct Cyc_List_List*
res= 0; for( 0; ds != 0; ds=({ struct Cyc_List_List* _temp2533= ds; if(
_temp2533 == 0){ _throw( Null_Exception);} _temp2533->tl;})){ struct Cyc_Absyn_Decl*
d=( struct Cyc_Absyn_Decl*)({ struct Cyc_List_List* _temp2611= ds; if( _temp2611
== 0){ _throw( Null_Exception);} _temp2611->hd;}); void* _temp2534=( void*) d->r;
struct Cyc_Absyn_Vardecl* _temp2560; struct Cyc_Absyn_Fndecl* _temp2562; int
_temp2564; struct Cyc_Absyn_Exp* _temp2566; struct Cyc_Core_Opt* _temp2568;
struct Cyc_Core_Opt* _temp2570; struct Cyc_Absyn_Pat* _temp2572; struct Cyc_Absyn_Structdecl*
_temp2574; struct Cyc_Absyn_Uniondecl* _temp2576; struct Cyc_Absyn_Tuniondecl*
_temp2578; struct Cyc_Absyn_XTuniondecl* _temp2580; struct Cyc_Absyn_Enumdecl*
_temp2582; struct Cyc_Absyn_Typedefdecl* _temp2584; struct Cyc_List_List*
_temp2586; struct _tagged_string* _temp2588; struct Cyc_List_List* _temp2590;
struct _tuple1* _temp2592; struct Cyc_List_List* _temp2594; _LL2536: if(((
struct _tunion_struct*) _temp2534)->tag == Cyc_Absyn_Var_d_tag){ _LL2561:
_temp2560=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp2534)->f1; goto _LL2537;} else{ goto _LL2538;} _LL2538: if((( struct
_tunion_struct*) _temp2534)->tag == Cyc_Absyn_Fn_d_tag){ _LL2563: _temp2562=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*) _temp2534)->f1; goto
_LL2539;} else{ goto _LL2540;} _LL2540: if((( struct _tunion_struct*) _temp2534)->tag
== Cyc_Absyn_Let_d_tag){ _LL2573: _temp2572=( struct Cyc_Absyn_Pat*)(( struct
Cyc_Absyn_Let_d_struct*) _temp2534)->f1; goto _LL2571; _LL2571: _temp2570=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp2534)->f2; goto
_LL2569; _LL2569: _temp2568=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp2534)->f3; goto _LL2567; _LL2567: _temp2566=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Let_d_struct*) _temp2534)->f4; goto _LL2565; _LL2565: _temp2564=(
int)(( struct Cyc_Absyn_Let_d_struct*) _temp2534)->f5; goto _LL2541;} else{ goto
_LL2542;} _LL2542: if((( struct _tunion_struct*) _temp2534)->tag == Cyc_Absyn_Struct_d_tag){
_LL2575: _temp2574=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*)
_temp2534)->f1; goto _LL2543;} else{ goto _LL2544;} _LL2544: if((( struct
_tunion_struct*) _temp2534)->tag == Cyc_Absyn_Union_d_tag){ _LL2577: _temp2576=(
struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*) _temp2534)->f1;
goto _LL2545;} else{ goto _LL2546;} _LL2546: if((( struct _tunion_struct*)
_temp2534)->tag == Cyc_Absyn_Tunion_d_tag){ _LL2579: _temp2578=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_d_struct*) _temp2534)->f1; goto _LL2547;} else{ goto
_LL2548;} _LL2548: if((( struct _tunion_struct*) _temp2534)->tag == Cyc_Absyn_XTunion_d_tag){
_LL2581: _temp2580=( struct Cyc_Absyn_XTuniondecl*)(( struct Cyc_Absyn_XTunion_d_struct*)
_temp2534)->f1; goto _LL2549;} else{ goto _LL2550;} _LL2550: if((( struct
_tunion_struct*) _temp2534)->tag == Cyc_Absyn_Enum_d_tag){ _LL2583: _temp2582=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*) _temp2534)->f1;
goto _LL2551;} else{ goto _LL2552;} _LL2552: if((( struct _tunion_struct*)
_temp2534)->tag == Cyc_Absyn_Typedef_d_tag){ _LL2585: _temp2584=( struct Cyc_Absyn_Typedefdecl*)((
struct Cyc_Absyn_Typedef_d_struct*) _temp2534)->f1; goto _LL2553;} else{ goto
_LL2554;} _LL2554: if((( struct _tunion_struct*) _temp2534)->tag == Cyc_Absyn_Namespace_d_tag){
_LL2589: _temp2588=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2534)->f1; goto _LL2587; _LL2587: _temp2586=( struct Cyc_List_List*)((
struct Cyc_Absyn_Namespace_d_struct*) _temp2534)->f2; goto _LL2555;} else{ goto
_LL2556;} _LL2556: if((( struct _tunion_struct*) _temp2534)->tag == Cyc_Absyn_Using_d_tag){
_LL2593: _temp2592=( struct _tuple1*)(( struct Cyc_Absyn_Using_d_struct*)
_temp2534)->f1; goto _LL2591; _LL2591: _temp2590=( struct Cyc_List_List*)((
struct Cyc_Absyn_Using_d_struct*) _temp2534)->f2; goto _LL2557;} else{ goto
_LL2558;} _LL2558: if((( struct _tunion_struct*) _temp2534)->tag == Cyc_Absyn_ExternC_d_tag){
_LL2595: _temp2594=( struct Cyc_List_List*)(( struct Cyc_Absyn_ExternC_d_struct*)
_temp2534)->f1; goto _LL2559;} else{ goto _LL2535;} _LL2537: { struct _tuple1*
c_name= _temp2560->name; if(( void*) _temp2560->sc ==( void*) Cyc_Absyn_ExternC){
c_name=({ struct _tuple1* _temp2596=( struct _tuple1*) GC_malloc( sizeof( struct
_tuple1)); _temp2596->f1= Cyc_Toc_abs_ns; _temp2596->f2=(* c_name).f2; _temp2596;});}
if( _temp2560->initializer != 0){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp2597= _temp2560->initializer; if( _temp2597 == 0){
_throw( Null_Exception);} _temp2597;}));} if( ! nv->toplevel){(( void(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp2598=(
char*)"decls_to_c: not at toplevel!"; struct _tagged_string _temp2599; _temp2599.curr=
_temp2598; _temp2599.base= _temp2598; _temp2599.last_plus_one= _temp2598 + 29;
_temp2599;}));} res=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( Cyc_Toc_temp_topdecls, res); Cyc_Toc_temp_topdecls=
0; nv= Cyc_Toc_add_varmap( nv, _temp2560->name, Cyc_Absyn_varb_exp( c_name,(
void*)({ struct Cyc_Absyn_Global_b_struct* _temp2600=( struct Cyc_Absyn_Global_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct)); _temp2600[ 0]=({ struct
Cyc_Absyn_Global_b_struct _temp2601; _temp2601.tag= Cyc_Absyn_Global_b_tag;
_temp2601.f1= _temp2560; _temp2601;}); _temp2600;}), 0)); _temp2560->name=
c_name;( void*)( _temp2560->sc=( void*) Cyc_Toc_scope_to_c(( void*) _temp2560->sc));(
void*)( _temp2560->type=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2560->type));
res=({ struct Cyc_List_List* _temp2602=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2602->hd=( void*) d; _temp2602->tl= res;
_temp2602;}); goto _LL2535;} _LL2539: { struct _tuple1* c_name= _temp2562->name;
if(( void*) _temp2562->sc ==( void*) Cyc_Absyn_ExternC){ c_name=({ struct
_tuple1* _temp2603=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp2603->f1= Cyc_Toc_abs_ns; _temp2603->f2=(* c_name).f2; _temp2603;});} nv=
Cyc_Toc_add_varmap( nv, _temp2562->name, Cyc_Absyn_var_exp( c_name, 0));
_temp2562->name= c_name; Cyc_Toc_fndecl_to_c( nv, _temp2562); res=({ struct Cyc_List_List*
_temp2604=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2604->hd=( void*) d; _temp2604->tl= res; _temp2604;}); goto _LL2535;}
_LL2541:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp2605=( char*)"letdecl"; struct _tagged_string
_temp2606; _temp2606.curr= _temp2605; _temp2606.base= _temp2605; _temp2606.last_plus_one=
_temp2605 + 8; _temp2606;})); goto _LL2535; _LL2543: Cyc_Toc_structdecl_to_c(
_temp2574); res=({ struct Cyc_List_List* _temp2607=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2607->hd=( void*) d; _temp2607->tl=
res; _temp2607;}); goto _LL2535; _LL2545: Cyc_Toc_uniondecl_to_c( _temp2576);
res=({ struct Cyc_List_List* _temp2608=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2608->hd=( void*) d; _temp2608->tl= res;
_temp2608;}); goto _LL2535; _LL2547: res= Cyc_Toc_tuniondecl_to_c( _temp2578,
res); goto _LL2535; _LL2549: res= Cyc_Toc_xtuniondecl_to_c( _temp2580, res);
goto _LL2535; _LL2551: Cyc_Toc_enumdecl_to_c( nv, _temp2582); res=({ struct Cyc_List_List*
_temp2609=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2609->hd=( void*) d; _temp2609->tl= res; _temp2609;}); goto _LL2535;
_LL2553: _temp2584->name= _temp2584->name; _temp2584->tvs= 0;( void*)( _temp2584->defn=(
void*) Cyc_Toc_typ_to_c_array(( void*) _temp2584->defn)); res=({ struct Cyc_List_List*
_temp2610=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2610->hd=( void*) d; _temp2610->tl= res; _temp2610;}); goto _LL2535;
_LL2555: _temp2590= _temp2586; goto _LL2557; _LL2557: _temp2594= _temp2590; goto
_LL2559; _LL2559: { struct _tuple17 p= Cyc_Toc_decls_to_c( nv, _temp2594, top);
nv= p.f1; res=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)((( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( p.f2), res); goto _LL2535;} _LL2535:;} return({ struct
_tuple17 _temp2612; _temp2612.f1= nv; _temp2612.f2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( res); _temp2612;});} static void Cyc_Toc_init(){
Cyc_Toc_added_decls= 0; Cyc_Toc_tuple_types= 0; Cyc_Toc_tuple_type_counter= 0;
Cyc_Toc_tagged_ptr_types= 0; Cyc_Toc_tagged_ptr_counter= 0; Cyc_Toc_temp_var_counter=
0; Cyc_Toc_fresh_label_counter= 0; Cyc_Toc_temp_topdecls= 0; Cyc_Toc_structs_so_far=
0; Cyc_Toc_tunions_so_far= 0; Cyc_Toc_xtunions_so_far= 0;} struct Cyc_List_List*
Cyc_Toc_toc( struct Cyc_List_List* ds){ Cyc_Toc_init();{ struct Cyc_List_List*
_temp2615; struct Cyc_Toc_Env* _temp2617; struct _tuple17 _temp2613= Cyc_Toc_decls_to_c(
Cyc_Toc_empty_env(), ds, 1); _LL2618: _temp2617= _temp2613.f1; goto _LL2616;
_LL2616: _temp2615= _temp2613.f2; goto _LL2614; _LL2614: return(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_Toc_added_decls),
_temp2615);}}