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
Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_decl_stmt(
struct Cyc_Absyn_Decl* d, struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_declare_stmt( struct _tuple1*,
void*, struct Cyc_Absyn_Exp* init, struct Cyc_Absyn_Stmt*, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_label_stmt( struct _tagged_string*
v, struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Stmt* Cyc_Absyn_goto_stmt( struct _tagged_string* lab, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_assign_stmt( struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Decl* Cyc_Absyn_new_decl( void* r, struct Cyc_Position_Segment* loc);
extern struct Cyc_Absyn_Vardecl* Cyc_Absyn_new_vardecl( struct _tuple1* x, void*
t, struct Cyc_Absyn_Exp* init); extern struct Cyc_Absyn_Vardecl* Cyc_Absyn_static_vardecl(
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
tag; } ; extern void* Cyc_Tcutil_typ_kind( void* t); extern void* Cyc_Tcutil_compress(
void* t); extern int Cyc_Tcutil_unify( void*, void*); extern struct Cyc_List_List*
Cyc_Tcutil_resolve_struct_designators( struct Cyc_Position_Segment* loc, struct
Cyc_List_List* des, struct Cyc_Absyn_Structdecl* sd); extern int Cyc_Tcutil_is_tagged_pointer_typ(
void*); extern unsigned int Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp*
e); extern struct Cyc_Core_Opt* Cyc_Formatstr_parse_conversionspecification(
struct _tagged_string s, int i); extern struct Cyc_List_List* Cyc_Toc_toc(
struct Cyc_List_List* ds); struct Cyc_Absyn_Abs_n_struct Cyc_Toc_abs_ns_value={
1u, 0}; void* Cyc_Toc_abs_ns=( void*)& Cyc_Toc_abs_ns_value; struct Cyc_Absyn_Tqual
Cyc_Toc_mt_tq_v=( struct Cyc_Absyn_Tqual){.q_const= 0,.q_volatile= 0,.q_restrict=
0}; struct Cyc_Absyn_Tqual* Cyc_Toc_mt_tq=& Cyc_Toc_mt_tq_v; char Cyc_Toc_Toc_Unimplemented_tag[
18u]="Toc_Unimplemented"; struct Cyc_Toc_Toc_Unimplemented_struct{ char* tag; }
; char Cyc_Toc_Toc_Impossible_tag[ 15u]="Toc_Impossible"; struct Cyc_Toc_Toc_Impossible_struct{
char* tag; } ; static void* Cyc_Toc_unimp( struct _tagged_string s){({ struct
_tagged_string _temp1= s; fprintf( Cyc_Stdio_stderr,"%.*s\n", _temp1.last_plus_one
- _temp1.curr, _temp1.curr);}); Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*)
Cyc_Stdio_stderr);( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Toc_Toc_Unimplemented_struct*
_temp2=( struct Cyc_Toc_Toc_Unimplemented_struct*) GC_malloc( sizeof( struct Cyc_Toc_Toc_Unimplemented_struct));
_temp2[ 0]=({ struct Cyc_Toc_Toc_Unimplemented_struct _temp3; _temp3.tag= Cyc_Toc_Toc_Unimplemented_tag;
_temp3;}); _temp2;}));} static void* Cyc_Toc_toc_impos( struct _tagged_string s){({
struct _tagged_string _temp4= s; fprintf( Cyc_Stdio_stderr,"%.*s\n", _temp4.last_plus_one
- _temp4.curr, _temp4.curr);}); Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*)
Cyc_Stdio_stderr);( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Toc_Toc_Impossible_struct*
_temp5=( struct Cyc_Toc_Toc_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Toc_Toc_Impossible_struct));
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
Cyc_Toc_Cyc_new_string_sp=& Cyc_Toc_Cyc_new_string_string; static int Cyc_Toc_is_nullable(
void* t){ void* _temp55= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp61; struct Cyc_Absyn_Conref* _temp63; struct Cyc_Absyn_Tqual* _temp65;
struct Cyc_Absyn_Conref* _temp67; void* _temp69; void* _temp71; _LL57: if((
unsigned int) _temp55 > 4u?(( struct _tunion_struct*) _temp55)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL62: _temp61=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp55)->f1; _LL72: _temp71=( void*) _temp61.elt_typ; goto _LL70; _LL70:
_temp69=( void*) _temp61.rgn_typ; goto _LL68; _LL68: _temp67=( struct Cyc_Absyn_Conref*)
_temp61.nullable; goto _LL66; _LL66: _temp65=( struct Cyc_Absyn_Tqual*) _temp61.tq;
goto _LL64; _LL64: _temp63=( struct Cyc_Absyn_Conref*) _temp61.bounds; goto
_LL58;} else{ goto _LL59;} _LL59: goto _LL60; _LL58: { void* _temp73=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp67))->v; int _temp79; _LL75: if(( unsigned int) _temp73 > 1u?(( struct
_tunion_struct*) _temp73)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL80: _temp79=(
int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp73)->f1; goto _LL76;} else{ goto
_LL77;} _LL77: goto _LL78; _LL76: return _temp79; _LL78: return 0; _LL74:;}
_LL60:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp81=( char*)"is_nullable"; struct _tagged_string
_temp82; _temp82.curr= _temp81; _temp82.base= _temp81; _temp82.last_plus_one=
_temp81 + 12; _temp82;})); return 0; _LL56:;} static struct _tuple1* Cyc_Toc_collapse_qvar_tag(
struct _tuple1* x, struct _tagged_string tag){ return({ struct _tuple1* _temp83=(
struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp83->f1=(* x).f1;
_temp83->f2=({ struct _tagged_string* _temp84=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp84[ 0]= Cyc_String_strconcat(*(*
x).f2, tag); _temp84;}); _temp83;});} static struct Cyc_List_List* Cyc_Toc_added_decls=
0; static void Cyc_Toc_add_tuple_decl( struct _tagged_string* x, struct Cyc_List_List*
ts){ struct Cyc_List_List* fs= 0;{ int i= 1; for( 0; ts != 0; ts=({ struct Cyc_List_List*
_temp85= ts; if( _temp85 == 0){ _throw( Null_Exception);} _temp85->tl;}), i ++){
fs=({ struct Cyc_List_List* _temp86=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp86->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp87=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp87->name=({ struct _tagged_string* _temp89=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp89[ 0]= xprintf("f%d", i);
_temp89;}); _temp87->tq= Cyc_Toc_mt_tq; _temp87->type=( void*)(( void*)({ struct
Cyc_List_List* _temp88= ts; if( _temp88 == 0){ _throw( Null_Exception);} _temp88->hd;}));
_temp87->width= 0; _temp87->attributes= 0; _temp87;}); _temp86->tl= fs; _temp86;});}}
fs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( fs);{
struct Cyc_Absyn_Structdecl* sd=({ struct Cyc_Absyn_Structdecl* _temp90=( struct
Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp90->sc=(
void*)(( void*) Cyc_Absyn_Public); _temp90->name=({ struct Cyc_Core_Opt* _temp92=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp92->v=(
void*)({ struct _tuple1* _temp93=( struct _tuple1*) GC_malloc( sizeof( struct
_tuple1)); _temp93->f1= Cyc_Toc_abs_ns; _temp93->f2= x; _temp93;}); _temp92;});
_temp90->tvs= 0; _temp90->fields=({ struct Cyc_Core_Opt* _temp91=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp91->v=( void*) fs; _temp91;});
_temp90->attributes= 0; _temp90;}); Cyc_Toc_added_decls=({ struct Cyc_List_List*
_temp94=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp94->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct*
_temp95=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp95[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp96; _temp96.tag= Cyc_Absyn_Struct_d_tag;
_temp96.f1= sd; _temp96;}); _temp95;}), 0); _temp94->tl= Cyc_Toc_added_decls;
_temp94;});}} static void Cyc_Toc_add_tagged_ptr_decl( struct _tagged_string* x,
void* t){ void* tptr= Cyc_Absyn_star_typ( t,( void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual());
struct Cyc_Absyn_Structfield* f1=({ struct Cyc_Absyn_Structfield* _temp97=(
struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp97->name= Cyc_Toc_curr_sp; _temp97->tq= Cyc_Toc_mt_tq; _temp97->type=( void*)
tptr; _temp97->width= 0; _temp97->attributes= 0; _temp97;}); struct Cyc_Absyn_Structfield*
f2=({ struct Cyc_Absyn_Structfield* _temp98=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp98->name= Cyc_Toc_base_sp;
_temp98->tq= Cyc_Toc_mt_tq; _temp98->type=( void*) tptr; _temp98->width= 0;
_temp98->attributes= 0; _temp98;}); struct Cyc_Absyn_Structfield* f3=({ struct
Cyc_Absyn_Structfield* _temp99=( struct Cyc_Absyn_Structfield*) GC_malloc(
sizeof( struct Cyc_Absyn_Structfield)); _temp99->name= Cyc_Toc_last_plus_one_sp;
_temp99->tq= Cyc_Toc_mt_tq; _temp99->type=( void*) tptr; _temp99->width= 0;
_temp99->attributes= 0; _temp99;}); struct Cyc_List_List* fs=({ struct Cyc_List_List*
_temp100=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp100->hd=( void*) f1; _temp100->tl=({ struct Cyc_List_List* _temp101=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp101->hd=(
void*) f2; _temp101->tl=({ struct Cyc_List_List* _temp102=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp102->hd=( void*) f3; _temp102->tl=
0; _temp102;}); _temp101;}); _temp100;}); struct Cyc_Absyn_Structdecl* sd=({
struct Cyc_Absyn_Structdecl* _temp103=( struct Cyc_Absyn_Structdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp103->sc=( void*)(( void*) Cyc_Absyn_Public);
_temp103->name=({ struct Cyc_Core_Opt* _temp105=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp105->v=( void*)({ struct _tuple1*
_temp106=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp106->f1=
Cyc_Toc_abs_ns; _temp106->f2= x; _temp106;}); _temp105;}); _temp103->tvs= 0;
_temp103->fields=({ struct Cyc_Core_Opt* _temp104=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp104->v=( void*)(( struct Cyc_List_List*)
fs); _temp104;}); _temp103->attributes= 0; _temp103;}); Cyc_Toc_added_decls=({
struct Cyc_List_List* _temp107=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp107->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Struct_d_struct* _temp108=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp108[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp109; _temp109.tag= Cyc_Absyn_Struct_d_tag;
_temp109.f1= sd; _temp109;}); _temp108;}), 0); _temp107->tl= Cyc_Toc_added_decls;
_temp107;});} static struct Cyc_List_List* Cyc_Toc_tuple_types= 0; static int
Cyc_Toc_tuple_type_counter= 0; static struct _tagged_string* Cyc_Toc_add_tuple_type(
struct Cyc_List_List* tqs0){{ struct Cyc_List_List* tts= Cyc_Toc_tuple_types;
for( 0; tts != 0; tts=({ struct Cyc_List_List* _temp110= tts; if( _temp110 == 0){
_throw( Null_Exception);} _temp110->tl;})){ struct _tuple4 _temp114; struct Cyc_List_List*
_temp115; struct _tagged_string* _temp117; struct _tuple4* _temp112=( struct
_tuple4*)({ struct Cyc_List_List* _temp111= tts; if( _temp111 == 0){ _throw(
Null_Exception);} _temp111->hd;}); _temp114=* _temp112; _LL118: _temp117=
_temp114.f1; goto _LL116; _LL116: _temp115= _temp114.f2; goto _LL113; _LL113: {
int okay= 1; struct Cyc_List_List* tqs= tqs0; for( 0; tqs != 0? _temp115 != 0: 0;
tqs=({ struct Cyc_List_List* _temp119= tqs; if( _temp119 == 0){ _throw(
Null_Exception);} _temp119->tl;}), _temp115=({ struct Cyc_List_List* _temp120=
_temp115; if( _temp120 == 0){ _throw( Null_Exception);} _temp120->tl;})){ if( !
Cyc_Tcutil_unify((*(( struct _tuple5*)({ struct Cyc_List_List* _temp121= tqs;
if( _temp121 == 0){ _throw( Null_Exception);} _temp121->hd;}))).f2,( void*)({
struct Cyc_List_List* _temp122= _temp115; if( _temp122 == 0){ _throw(
Null_Exception);} _temp122->hd;}))){ okay= 0; break;}} if(( ! okay? 1: tqs != 0)?
1: _temp115 != 0){ continue;} return _temp117;}}}{ struct _tagged_string* x=({
struct _tagged_string* _temp123=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp123[ 0]= xprintf("_tuple%d",( Cyc_Toc_tuple_type_counter
++)); _temp123;}); struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)( void*(*
f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct
_tuple5*)) Cyc_Core_snd, tqs0); Cyc_Toc_add_tuple_decl( x, ts); Cyc_Toc_tuple_types=({
struct Cyc_List_List* _temp124=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp124->hd=( void*)({ struct _tuple4* _temp125=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp125->f1= x; _temp125->f2=
ts; _temp125;}); _temp124->tl= Cyc_Toc_tuple_types; _temp124;}); return x;}}
static struct Cyc_List_List* Cyc_Toc_tagged_ptr_types= 0; static int Cyc_Toc_tagged_ptr_counter=
0; static struct _tagged_string* Cyc_Toc_add_tagged_ptr_type( void* t, struct
Cyc_Absyn_Tqual* tq){{ void* _temp126= t; void* _temp132; void* _temp134; _LL128:
if(( unsigned int) _temp126 > 4u?(( struct _tunion_struct*) _temp126)->tag ==
Cyc_Absyn_IntType_tag: 0){ _LL135: _temp134=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp126)->f1; if(( int) _temp134 == Cyc_Absyn_Unsigned){ goto _LL133;} else{
goto _LL130;} _LL133: _temp132=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp126)->f2; if(( int) _temp132 == Cyc_Absyn_B1){ goto _LL129;} else{ goto
_LL130;}} else{ goto _LL130;} _LL130: goto _LL131; _LL129: return Cyc_Toc__tagged_string_sp;
_LL131: goto _LL127; _LL127:;}{ struct Cyc_List_List* tts= Cyc_Toc_tagged_ptr_types;
for( 0; tts != 0; tts=({ struct Cyc_List_List* _temp136= tts; if( _temp136 == 0){
_throw( Null_Exception);} _temp136->tl;})){ struct _tuple6 _temp140; void*
_temp141; struct _tagged_string* _temp143; struct _tuple6* _temp138=( struct
_tuple6*)({ struct Cyc_List_List* _temp137= tts; if( _temp137 == 0){ _throw(
Null_Exception);} _temp137->hd;}); _temp140=* _temp138; _LL144: _temp143=
_temp140.f1; goto _LL142; _LL142: _temp141= _temp140.f2; goto _LL139; _LL139:
if( ! Cyc_Tcutil_unify( t, _temp141)){ continue;} return _temp143;}}{ struct
_tagged_string* x=({ struct _tagged_string* _temp145=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp145[ 0]= xprintf("_tagged_ptr%d",(
Cyc_Toc_tagged_ptr_counter ++)); _temp145;}); Cyc_Toc_add_tagged_ptr_decl( x, t);
Cyc_Toc_tagged_ptr_types=({ struct Cyc_List_List* _temp146=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp146->hd=( void*)({ struct
_tuple6* _temp147=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6));
_temp147->f1= x; _temp147->f2= t; _temp147;}); _temp146->tl= Cyc_Toc_tagged_ptr_types;
_temp146;}); return x;}} static int Cyc_Toc_temp_var_counter= 0; static struct
_tuple1* Cyc_Toc_temp_var(){ return({ struct _tuple1* _temp148=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp148->f1= Cyc_Toc_abs_ns; _temp148->f2=({
struct _tagged_string* _temp149=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp149[ 0]= xprintf("_temp%d",( Cyc_Toc_temp_var_counter
++)); _temp149;}); _temp148;});} static int Cyc_Toc_fresh_label_counter= 0;
static struct _tagged_string* Cyc_Toc_fresh_label(){ return({ struct
_tagged_string* _temp150=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp150[ 0]= xprintf("_LL%d",( Cyc_Toc_temp_var_counter
++)); _temp150;});} static struct Cyc_List_List* Cyc_Toc_temp_topdecls= 0;
static void* Cyc_Toc_typ_to_c( void* t); static struct _tuple2* Cyc_Toc_arg_to_c(
struct _tuple2* a){ void* _temp153; struct Cyc_Absyn_Tqual* _temp155; struct Cyc_Core_Opt*
_temp157; struct _tuple2 _temp151=* a; _LL158: _temp157= _temp151.f1; goto
_LL156; _LL156: _temp155= _temp151.f2; goto _LL154; _LL154: _temp153= _temp151.f3;
goto _LL152; _LL152: return({ struct _tuple2* _temp159=( struct _tuple2*)
GC_malloc( sizeof( struct _tuple2)); _temp159->f1= _temp157; _temp159->f2=
_temp155; _temp159->f3= Cyc_Toc_typ_to_c( _temp153); _temp159;});} static struct
_tuple5* Cyc_Toc_typ_to_c_f( struct _tuple5* x){ void* _temp162; struct Cyc_Absyn_Tqual*
_temp164; struct _tuple5 _temp160=* x; _LL165: _temp164= _temp160.f1; goto
_LL163; _LL163: _temp162= _temp160.f2; goto _LL161; _LL161: return({ struct
_tuple5* _temp166=( struct _tuple5*) GC_malloc( sizeof( struct _tuple5));
_temp166->f1= _temp164; _temp166->f2= Cyc_Toc_typ_to_c( _temp162); _temp166;});}
static void* Cyc_Toc_typ_to_c_array( void* t){ void* _temp167= t; struct Cyc_Absyn_Exp*
_temp175; struct Cyc_Absyn_Tqual* _temp177; void* _temp179; int _temp181; struct
Cyc_Core_Opt* _temp183; struct Cyc_Core_Opt _temp185; void* _temp186; void*
_temp188; _LL169: if(( unsigned int) _temp167 > 4u?(( struct _tunion_struct*)
_temp167)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL180: _temp179=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp167)->f1; goto _LL178; _LL178: _temp177=(
struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*) _temp167)->f2;
goto _LL176; _LL176: _temp175=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp167)->f3; goto _LL170;} else{ goto _LL171;} _LL171: if(( unsigned int)
_temp167 > 4u?(( struct _tunion_struct*) _temp167)->tag == Cyc_Absyn_Evar_tag: 0){
_LL189: _temp188=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp167)->f1; goto
_LL184; _LL184: _temp183=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp167)->f2; if( _temp183 == 0){ goto _LL173;} else{ _temp185=* _temp183;
_LL187: _temp186=( void*) _temp185.v; goto _LL182;} _LL182: _temp181=( int)((
struct Cyc_Absyn_Evar_struct*) _temp167)->f3; goto _LL172;} else{ goto _LL173;}
_LL173: goto _LL174; _LL170: return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp190=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp190[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp191; _temp191.tag= Cyc_Absyn_ArrayType_tag;
_temp191.f1=( void*) Cyc_Toc_typ_to_c_array( _temp179); _temp191.f2= _temp177;
_temp191.f3= _temp175; _temp191;}); _temp190;}); _LL172: return Cyc_Toc_typ_to_c_array(
_temp186); _LL174: return Cyc_Toc_typ_to_c( t); _LL168:;} static void* Cyc_Toc_typ_to_c(
void* t){ void* _temp192= t; int _temp240; struct Cyc_Core_Opt* _temp242; void*
_temp244; int _temp246; struct Cyc_Core_Opt* _temp248; struct Cyc_Core_Opt
_temp250; void* _temp251; void* _temp253; struct Cyc_Absyn_Tvar* _temp255;
struct Cyc_Absyn_TunionInfo _temp257; struct Cyc_Absyn_Tuniondecl* _temp259;
void* _temp261; struct Cyc_List_List* _temp263; struct _tuple1* _temp265; struct
Cyc_Absyn_XTunionInfo _temp267; struct Cyc_Absyn_XTuniondecl* _temp269; void*
_temp271; struct _tuple1* _temp273; struct Cyc_Absyn_TunionFieldInfo _temp275;
struct Cyc_Absyn_Tunionfield* _temp277; struct Cyc_Absyn_Tuniondecl* _temp279;
struct _tuple1* _temp281; struct Cyc_List_List* _temp283; struct _tuple1*
_temp285; struct Cyc_Absyn_XTunionFieldInfo _temp287; struct Cyc_Absyn_Tunionfield*
_temp289; struct Cyc_Absyn_XTuniondecl* _temp291; struct _tuple1* _temp293;
struct _tuple1* _temp295; struct Cyc_Absyn_PtrInfo _temp297; struct Cyc_Absyn_Conref*
_temp299; struct Cyc_Absyn_Tqual* _temp301; struct Cyc_Absyn_Conref* _temp303;
void* _temp305; void* _temp307; void* _temp309; void* _temp311; struct Cyc_Absyn_Exp*
_temp313; struct Cyc_Absyn_Tqual* _temp315; void* _temp317; struct Cyc_Absyn_FnInfo
_temp319; struct Cyc_List_List* _temp321; int _temp323; struct Cyc_List_List*
_temp325; void* _temp327; struct Cyc_Core_Opt* _temp329; struct Cyc_List_List*
_temp331; struct Cyc_List_List* _temp333; struct Cyc_Absyn_Uniondecl** _temp335;
struct Cyc_List_List* _temp337; struct _tuple1* _temp339; struct Cyc_Absyn_Structdecl**
_temp341; struct Cyc_List_List* _temp343; struct _tuple1* _temp345; struct Cyc_Absyn_Enumdecl*
_temp347; struct _tuple1* _temp349; struct Cyc_Core_Opt* _temp351; struct Cyc_List_List*
_temp353; struct _tuple1* _temp355; void* _temp357; void* _temp359; struct Cyc_List_List*
_temp361; _LL194: if(( int) _temp192 == Cyc_Absyn_VoidType){ goto _LL195;} else{
goto _LL196;} _LL196: if(( unsigned int) _temp192 > 4u?(( struct _tunion_struct*)
_temp192)->tag == Cyc_Absyn_Evar_tag: 0){ _LL245: _temp244=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp192)->f1; goto _LL243; _LL243: _temp242=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Evar_struct*) _temp192)->f2; if( _temp242 == 0){ goto _LL241;} else{
goto _LL198;} _LL241: _temp240=( int)(( struct Cyc_Absyn_Evar_struct*) _temp192)->f3;
goto _LL197;} else{ goto _LL198;} _LL198: if(( unsigned int) _temp192 > 4u?((
struct _tunion_struct*) _temp192)->tag == Cyc_Absyn_Evar_tag: 0){ _LL254:
_temp253=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp192)->f1; goto _LL249;
_LL249: _temp248=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp192)->f2; if( _temp248 == 0){ goto _LL200;} else{ _temp250=* _temp248;
_LL252: _temp251=( void*) _temp250.v; goto _LL247;} _LL247: _temp246=( int)((
struct Cyc_Absyn_Evar_struct*) _temp192)->f3; goto _LL199;} else{ goto _LL200;}
_LL200: if(( unsigned int) _temp192 > 4u?(( struct _tunion_struct*) _temp192)->tag
== Cyc_Absyn_VarType_tag: 0){ _LL256: _temp255=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_VarType_struct*) _temp192)->f1; goto _LL201;} else{ goto _LL202;}
_LL202: if(( unsigned int) _temp192 > 4u?(( struct _tunion_struct*) _temp192)->tag
== Cyc_Absyn_TunionType_tag: 0){ _LL258: _temp257=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp192)->f1; _LL266: _temp265=( struct
_tuple1*) _temp257.name; goto _LL264; _LL264: _temp263=( struct Cyc_List_List*)
_temp257.targs; goto _LL262; _LL262: _temp261=( void*) _temp257.rgn; goto _LL260;
_LL260: _temp259=( struct Cyc_Absyn_Tuniondecl*) _temp257.tud; goto _LL203;}
else{ goto _LL204;} _LL204: if(( unsigned int) _temp192 > 4u?(( struct
_tunion_struct*) _temp192)->tag == Cyc_Absyn_XTunionType_tag: 0){ _LL268:
_temp267=( struct Cyc_Absyn_XTunionInfo)(( struct Cyc_Absyn_XTunionType_struct*)
_temp192)->f1; _LL274: _temp273=( struct _tuple1*) _temp267.name; goto _LL272;
_LL272: _temp271=( void*) _temp267.rgn; goto _LL270; _LL270: _temp269=( struct
Cyc_Absyn_XTuniondecl*) _temp267.xtud; goto _LL205;} else{ goto _LL206;} _LL206:
if(( unsigned int) _temp192 > 4u?(( struct _tunion_struct*) _temp192)->tag ==
Cyc_Absyn_TunionFieldType_tag: 0){ _LL276: _temp275=( struct Cyc_Absyn_TunionFieldInfo)((
struct Cyc_Absyn_TunionFieldType_struct*) _temp192)->f1; _LL286: _temp285=(
struct _tuple1*) _temp275.name; goto _LL284; _LL284: _temp283=( struct Cyc_List_List*)
_temp275.targs; goto _LL282; _LL282: _temp281=( struct _tuple1*) _temp275.fname;
goto _LL280; _LL280: _temp279=( struct Cyc_Absyn_Tuniondecl*) _temp275.tud; goto
_LL278; _LL278: _temp277=( struct Cyc_Absyn_Tunionfield*) _temp275.tufd; goto
_LL207;} else{ goto _LL208;} _LL208: if(( unsigned int) _temp192 > 4u?(( struct
_tunion_struct*) _temp192)->tag == Cyc_Absyn_XTunionFieldType_tag: 0){ _LL288:
_temp287=( struct Cyc_Absyn_XTunionFieldInfo)(( struct Cyc_Absyn_XTunionFieldType_struct*)
_temp192)->f1; _LL296: _temp295=( struct _tuple1*) _temp287.name; goto _LL294;
_LL294: _temp293=( struct _tuple1*) _temp287.fname; goto _LL292; _LL292:
_temp291=( struct Cyc_Absyn_XTuniondecl*) _temp287.xtud; goto _LL290; _LL290:
_temp289=( struct Cyc_Absyn_Tunionfield*) _temp287.xtufd; goto _LL209;} else{
goto _LL210;} _LL210: if(( unsigned int) _temp192 > 4u?(( struct _tunion_struct*)
_temp192)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL298: _temp297=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp192)->f1; _LL308: _temp307=( void*)
_temp297.elt_typ; goto _LL306; _LL306: _temp305=( void*) _temp297.rgn_typ; goto
_LL304; _LL304: _temp303=( struct Cyc_Absyn_Conref*) _temp297.nullable; goto
_LL302; _LL302: _temp301=( struct Cyc_Absyn_Tqual*) _temp297.tq; goto _LL300;
_LL300: _temp299=( struct Cyc_Absyn_Conref*) _temp297.bounds; goto _LL211;}
else{ goto _LL212;} _LL212: if(( unsigned int) _temp192 > 4u?(( struct
_tunion_struct*) _temp192)->tag == Cyc_Absyn_IntType_tag: 0){ _LL312: _temp311=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp192)->f1; goto _LL310; _LL310:
_temp309=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp192)->f2; goto _LL213;}
else{ goto _LL214;} _LL214: if(( int) _temp192 == Cyc_Absyn_FloatType){ goto
_LL215;} else{ goto _LL216;} _LL216: if(( int) _temp192 == Cyc_Absyn_DoubleType){
goto _LL217;} else{ goto _LL218;} _LL218: if(( unsigned int) _temp192 > 4u?((
struct _tunion_struct*) _temp192)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL318:
_temp317=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp192)->f1; goto
_LL316; _LL316: _temp315=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp192)->f2; goto _LL314; _LL314: _temp313=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp192)->f3; goto _LL219;} else{ goto _LL220;}
_LL220: if(( unsigned int) _temp192 > 4u?(( struct _tunion_struct*) _temp192)->tag
== Cyc_Absyn_FnType_tag: 0){ _LL320: _temp319=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp192)->f1; _LL332: _temp331=( struct Cyc_List_List*)
_temp319.tvars; goto _LL330; _LL330: _temp329=( struct Cyc_Core_Opt*) _temp319.effect;
goto _LL328; _LL328: _temp327=( void*) _temp319.ret_typ; goto _LL326; _LL326:
_temp325=( struct Cyc_List_List*) _temp319.args; goto _LL324; _LL324: _temp323=(
int) _temp319.varargs; goto _LL322; _LL322: _temp321=( struct Cyc_List_List*)
_temp319.attributes; goto _LL221;} else{ goto _LL222;} _LL222: if(( unsigned int)
_temp192 > 4u?(( struct _tunion_struct*) _temp192)->tag == Cyc_Absyn_TupleType_tag:
0){ _LL334: _temp333=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp192)->f1; goto _LL223;} else{ goto _LL224;} _LL224: if(( unsigned int)
_temp192 > 4u?(( struct _tunion_struct*) _temp192)->tag == Cyc_Absyn_UnionType_tag:
0){ _LL340: _temp339=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp192)->f1; goto _LL338; _LL338: _temp337=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp192)->f2; goto _LL336; _LL336: _temp335=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp192)->f3;
goto _LL225;} else{ goto _LL226;} _LL226: if(( unsigned int) _temp192 > 4u?((
struct _tunion_struct*) _temp192)->tag == Cyc_Absyn_StructType_tag: 0){ _LL346:
_temp345=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp192)->f1;
goto _LL344; _LL344: _temp343=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp192)->f2; goto _LL342; _LL342: _temp341=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp192)->f3; goto _LL227;} else{ goto
_LL228;} _LL228: if(( unsigned int) _temp192 > 4u?(( struct _tunion_struct*)
_temp192)->tag == Cyc_Absyn_EnumType_tag: 0){ _LL350: _temp349=( struct _tuple1*)((
struct Cyc_Absyn_EnumType_struct*) _temp192)->f1; goto _LL348; _LL348: _temp347=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*) _temp192)->f2;
goto _LL229;} else{ goto _LL230;} _LL230: if(( unsigned int) _temp192 > 4u?((
struct _tunion_struct*) _temp192)->tag == Cyc_Absyn_TypedefType_tag: 0){ _LL356:
_temp355=( struct _tuple1*)(( struct Cyc_Absyn_TypedefType_struct*) _temp192)->f1;
goto _LL354; _LL354: _temp353=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp192)->f2; goto _LL352; _LL352: _temp351=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_TypedefType_struct*) _temp192)->f3; goto _LL231;} else{ goto _LL232;}
_LL232: if(( unsigned int) _temp192 > 4u?(( struct _tunion_struct*) _temp192)->tag
== Cyc_Absyn_RgnHandleType_tag: 0){ _LL358: _temp357=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp192)->f1; goto _LL233;} else{ goto _LL234;} _LL234: if(( int) _temp192 ==
Cyc_Absyn_HeapRgn){ goto _LL235;} else{ goto _LL236;} _LL236: if(( unsigned int)
_temp192 > 4u?(( struct _tunion_struct*) _temp192)->tag == Cyc_Absyn_AccessEff_tag:
0){ _LL360: _temp359=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp192)->f1;
goto _LL237;} else{ goto _LL238;} _LL238: if(( unsigned int) _temp192 > 4u?((
struct _tunion_struct*) _temp192)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL362:
_temp361=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp192)->f1;
goto _LL239;} else{ goto _LL193;} _LL195: return t; _LL197: return( void*) Cyc_Absyn_VoidType;
_LL199: return Cyc_Toc_typ_to_c( _temp251); _LL201: return Cyc_Absyn_void_star_typ();
_LL203: return( void*)({ struct Cyc_Absyn_TypedefType_struct* _temp363=( struct
Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp363[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp364; _temp364.tag= Cyc_Absyn_TypedefType_tag;
_temp364.f1=( struct _tuple1*) _temp265; _temp364.f2= 0; _temp364.f3=({ struct
Cyc_Core_Opt* _temp365=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp365->v=( void*) Cyc_Absyn_void_star_typ(); _temp365;}); _temp364;});
_temp363;}); _LL205: return( void*)({ struct Cyc_Absyn_TypedefType_struct*
_temp366=( struct Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp366[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp367; _temp367.tag= Cyc_Absyn_TypedefType_tag;
_temp367.f1= _temp273; _temp367.f2= 0; _temp367.f3=({ struct Cyc_Core_Opt*
_temp368=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp368->v=( void*) Cyc_Absyn_star_typ( Cyc_Absyn_strct( Cyc_Toc__xtunion_struct_sp),(
void*) Cyc_Absyn_HeapRgn, Cyc_Toc_mt_tq); _temp368;}); _temp367;}); _temp366;});
_LL207: if(({ struct Cyc_Absyn_Tunionfield* _temp369= _temp277; if( _temp369 ==
0){ _throw( Null_Exception);} _temp369->typs;}) == 0){ return Cyc_Absyn_uint_t;}
else{ return Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( _temp281,( struct
_tagged_string)({ char* _temp370=( char*)"_struct"; struct _tagged_string
_temp371; _temp371.curr= _temp370; _temp371.base= _temp370; _temp371.last_plus_one=
_temp370 + 8; _temp371;})));} _LL209: return Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag(
_temp293,( struct _tagged_string)({ char* _temp372=( char*)"_struct"; struct
_tagged_string _temp373; _temp373.curr= _temp372; _temp373.base= _temp372;
_temp373.last_plus_one= _temp372 + 8; _temp373;}))); _LL211: _temp307= Cyc_Toc_typ_to_c_array(
_temp307);{ void* _temp374=( void*) _temp299->v; void* _temp380; _LL376: if((
unsigned int) _temp374 > 1u?(( struct _tunion_struct*) _temp374)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL381: _temp380=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp374)->f1;
if(( int) _temp380 == Cyc_Absyn_Unknown_b){ goto _LL377;} else{ goto _LL378;}}
else{ goto _LL378;} _LL378: goto _LL379; _LL377: { struct _tagged_string* n= Cyc_Toc_add_tagged_ptr_type(
_temp307, _temp301); return Cyc_Absyn_strct( n);} _LL379: return Cyc_Absyn_star_typ(
_temp307,( void*) Cyc_Absyn_HeapRgn, _temp301); _LL375:;} _LL213: return t;
_LL215: return t; _LL217: return t; _LL219: return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp382=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp382[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp383; _temp383.tag= Cyc_Absyn_ArrayType_tag;
_temp383.f1=( void*) Cyc_Toc_typ_to_c_array( _temp317); _temp383.f2= _temp315;
_temp383.f3= _temp313; _temp383;}); _temp382;}); _LL221: return( void*)({ struct
Cyc_Absyn_FnType_struct* _temp384=( struct Cyc_Absyn_FnType_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_FnType_struct)); _temp384[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp385; _temp385.tag= Cyc_Absyn_FnType_tag; _temp385.f1=({ struct Cyc_Absyn_FnInfo
_temp386; _temp386.tvars= 0; _temp386.effect= 0; _temp386.ret_typ=( void*) Cyc_Toc_typ_to_c(
_temp327); _temp386.args=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)(
struct _tuple2*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_arg_to_c,
_temp325); _temp386.varargs= _temp323; _temp386.attributes= 0; _temp386;});
_temp385;}); _temp384;}); _LL223: _temp333=(( struct Cyc_List_List*(*)( struct
_tuple5*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_typ_to_c_f,
_temp333);{ struct _tagged_string* n= Cyc_Toc_add_tuple_type( _temp333); return
Cyc_Absyn_strct( n);} _LL225: if( _temp339 == 0){(( void(*)( struct
_tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char* _temp387=(
char*)"anonymous union"; struct _tagged_string _temp388; _temp388.curr= _temp387;
_temp388.base= _temp387; _temp388.last_plus_one= _temp387 + 16; _temp388;}));}
return Cyc_Absyn_unionq_typ(( struct _tuple1*)({ struct _tuple1* _temp389=
_temp339; if( _temp389 == 0){ _throw( Null_Exception);} _temp389;})); _LL227:
if( _temp345 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(( struct
_tagged_string)({ char* _temp390=( char*)"anonymous struct"; struct
_tagged_string _temp391; _temp391.curr= _temp390; _temp391.base= _temp390;
_temp391.last_plus_one= _temp390 + 17; _temp391;}));} return Cyc_Absyn_strctq((
struct _tuple1*)({ struct _tuple1* _temp392= _temp345; if( _temp392 == 0){
_throw( Null_Exception);} _temp392;})); _LL229: return t; _LL231: if( _temp351
== 0){ return(( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp393=( char*)"Toc::typ_to_c: unresolved TypedefType";
struct _tagged_string _temp394; _temp394.curr= _temp393; _temp394.base= _temp393;
_temp394.last_plus_one= _temp393 + 38; _temp394;}));} return( void*)({ struct
Cyc_Absyn_TypedefType_struct* _temp395=( struct Cyc_Absyn_TypedefType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp395[ 0]=({ struct
Cyc_Absyn_TypedefType_struct _temp396; _temp396.tag= Cyc_Absyn_TypedefType_tag;
_temp396.f1= _temp355; _temp396.f2= 0; _temp396.f3=({ struct Cyc_Core_Opt*
_temp397=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp397->v=( void*) Cyc_Toc_typ_to_c_array(( void*)({ struct Cyc_Core_Opt*
_temp398= _temp351; if( _temp398 == 0){ _throw( Null_Exception);} _temp398->v;}));
_temp397;}); _temp396;}); _temp395;}); _LL233: return Cyc_Absyn_void_star_typ();
_LL235: return(( void*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp399=( char*)"Toc::typ_to_c: type translation passed the heap region";
struct _tagged_string _temp400; _temp400.curr= _temp399; _temp400.base= _temp399;
_temp400.last_plus_one= _temp399 + 55; _temp400;})); _LL237: return(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp401=( char*)"Toc::typ_to_c: type translation passed the access eff"; struct
_tagged_string _temp402; _temp402.curr= _temp401; _temp402.base= _temp401;
_temp402.last_plus_one= _temp401 + 54; _temp402;})); _LL239: return(( void*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp403=( char*)"Toc::typ_to_c: type translation passed the join eff"; struct
_tagged_string _temp404; _temp404.curr= _temp403; _temp404.base= _temp403;
_temp404.last_plus_one= _temp403 + 52; _temp404;})); _LL193:;} static struct Cyc_Absyn_Exp*
Cyc_Toc_array_to_ptr( void* t, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
l){ void* _temp405= t; struct Cyc_Absyn_Exp* _temp411; struct Cyc_Absyn_Tqual*
_temp413; void* _temp415; _LL407: if(( unsigned int) _temp405 > 4u?(( struct
_tunion_struct*) _temp405)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL416: _temp415=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp405)->f1; goto _LL414; _LL414:
_temp413=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp405)->f2; goto _LL412; _LL412: _temp411=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp405)->f3; goto _LL408;} else{ goto _LL409;}
_LL409: goto _LL410; _LL408: return Cyc_Absyn_cast_exp( Cyc_Absyn_star_typ(
_temp415,( void*) Cyc_Absyn_HeapRgn, _temp413), e, l); _LL410: return e; _LL406:;}
static struct Cyc_Absyn_Exp* Cyc_Toc_array_to_ptr_cast( void* t, struct Cyc_Absyn_Exp*
e, struct Cyc_Position_Segment* l){ void* _temp417= t; struct Cyc_Absyn_Exp*
_temp423; struct Cyc_Absyn_Tqual* _temp425; void* _temp427; _LL419: if((
unsigned int) _temp417 > 4u?(( struct _tunion_struct*) _temp417)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL428: _temp427=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp417)->f1;
goto _LL426; _LL426: _temp425=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp417)->f2; goto _LL424; _LL424: _temp423=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp417)->f3; goto _LL420;} else{ goto _LL421;}
_LL421: goto _LL422; _LL420: return Cyc_Absyn_cast_exp( Cyc_Absyn_star_typ(
_temp427,( void*) Cyc_Absyn_HeapRgn, _temp425), e, l); _LL422: return Cyc_Absyn_cast_exp(
t, e, l); _LL418:;} static int Cyc_Toc_atomic_typ( void* t){ void* _temp429= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_Tvar* _temp465; void* _temp467; void* _temp469; struct Cyc_Absyn_Exp*
_temp471; struct Cyc_Absyn_Tqual* _temp473; void* _temp475; struct Cyc_Absyn_FnInfo
_temp477; struct Cyc_List_List* _temp479; struct Cyc_Absyn_Structdecl** _temp481;
struct Cyc_List_List* _temp483; struct _tuple1* _temp485; struct Cyc_Absyn_TunionInfo
_temp487; struct Cyc_Absyn_XTunionInfo _temp489; struct Cyc_Absyn_TunionFieldInfo
_temp491; struct Cyc_Absyn_Tunionfield* _temp493; struct Cyc_Absyn_Tuniondecl*
_temp495; struct _tuple1* _temp497; struct Cyc_List_List* _temp499; struct
_tuple1* _temp501; struct Cyc_Absyn_XTunionFieldInfo _temp503; struct Cyc_Absyn_Uniondecl**
_temp505; struct Cyc_List_List* _temp507; struct _tuple1* _temp509; struct Cyc_Absyn_PtrInfo
_temp511; void* _temp513; _LL431: if(( int) _temp429 == Cyc_Absyn_VoidType){
goto _LL432;} else{ goto _LL433;} _LL433: if(( unsigned int) _temp429 > 4u?((
struct _tunion_struct*) _temp429)->tag == Cyc_Absyn_VarType_tag: 0){ _LL466:
_temp465=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*) _temp429)->f1;
goto _LL434;} else{ goto _LL435;} _LL435: if(( unsigned int) _temp429 > 4u?((
struct _tunion_struct*) _temp429)->tag == Cyc_Absyn_IntType_tag: 0){ _LL470:
_temp469=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp429)->f1; goto _LL468;
_LL468: _temp467=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp429)->f2;
goto _LL436;} else{ goto _LL437;} _LL437: if(( int) _temp429 == Cyc_Absyn_FloatType){
goto _LL438;} else{ goto _LL439;} _LL439: if(( int) _temp429 == Cyc_Absyn_DoubleType){
goto _LL440;} else{ goto _LL441;} _LL441: if(( unsigned int) _temp429 > 4u?((
struct _tunion_struct*) _temp429)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL476:
_temp475=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp429)->f1; goto
_LL474; _LL474: _temp473=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp429)->f2; goto _LL472; _LL472: _temp471=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp429)->f3; goto _LL442;} else{ goto _LL443;}
_LL443: if(( unsigned int) _temp429 > 4u?(( struct _tunion_struct*) _temp429)->tag
== Cyc_Absyn_FnType_tag: 0){ _LL478: _temp477=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp429)->f1; goto _LL444;} else{ goto _LL445;}
_LL445: if(( unsigned int) _temp429 > 4u?(( struct _tunion_struct*) _temp429)->tag
== Cyc_Absyn_TupleType_tag: 0){ _LL480: _temp479=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp429)->f1; goto _LL446;} else{ goto
_LL447;} _LL447: if(( unsigned int) _temp429 > 4u?(( struct _tunion_struct*)
_temp429)->tag == Cyc_Absyn_StructType_tag: 0){ _LL486: _temp485=( struct
_tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp429)->f1; goto _LL484;
_LL484: _temp483=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp429)->f2; goto _LL482; _LL482: _temp481=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp429)->f3; goto _LL448;} else{ goto
_LL449;} _LL449: if(( unsigned int) _temp429 > 4u?(( struct _tunion_struct*)
_temp429)->tag == Cyc_Absyn_TunionType_tag: 0){ _LL488: _temp487=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp429)->f1; goto _LL450;} else{ goto
_LL451;} _LL451: if(( unsigned int) _temp429 > 4u?(( struct _tunion_struct*)
_temp429)->tag == Cyc_Absyn_XTunionType_tag: 0){ _LL490: _temp489=( struct Cyc_Absyn_XTunionInfo)((
struct Cyc_Absyn_XTunionType_struct*) _temp429)->f1; goto _LL452;} else{ goto
_LL453;} _LL453: if(( unsigned int) _temp429 > 4u?(( struct _tunion_struct*)
_temp429)->tag == Cyc_Absyn_TunionFieldType_tag: 0){ _LL492: _temp491=( struct
Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*) _temp429)->f1;
_LL502: _temp501=( struct _tuple1*) _temp491.name; goto _LL500; _LL500: _temp499=(
struct Cyc_List_List*) _temp491.targs; goto _LL498; _LL498: _temp497=( struct
_tuple1*) _temp491.fname; goto _LL496; _LL496: _temp495=( struct Cyc_Absyn_Tuniondecl*)
_temp491.tud; goto _LL494; _LL494: _temp493=( struct Cyc_Absyn_Tunionfield*)
_temp491.tufd; goto _LL454;} else{ goto _LL455;} _LL455: if(( unsigned int)
_temp429 > 4u?(( struct _tunion_struct*) _temp429)->tag == Cyc_Absyn_XTunionFieldType_tag:
0){ _LL504: _temp503=( struct Cyc_Absyn_XTunionFieldInfo)(( struct Cyc_Absyn_XTunionFieldType_struct*)
_temp429)->f1; goto _LL456;} else{ goto _LL457;} _LL457: if(( unsigned int)
_temp429 > 4u?(( struct _tunion_struct*) _temp429)->tag == Cyc_Absyn_UnionType_tag:
0){ _LL510: _temp509=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp429)->f1; goto _LL508; _LL508: _temp507=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp429)->f2; goto _LL506; _LL506: _temp505=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp429)->f3;
goto _LL458;} else{ goto _LL459;} _LL459: if(( unsigned int) _temp429 > 4u?((
struct _tunion_struct*) _temp429)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL512:
_temp511=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp429)->f1; goto _LL460;} else{ goto _LL461;} _LL461: if(( unsigned int)
_temp429 > 4u?(( struct _tunion_struct*) _temp429)->tag == Cyc_Absyn_RgnHandleType_tag:
0){ _LL514: _temp513=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp429)->f1;
goto _LL462;} else{ goto _LL463;} _LL463: goto _LL464; _LL432: return 1; _LL434:
return 0; _LL436: return 1; _LL438: return 1; _LL440: return 0; _LL442: return
Cyc_Toc_atomic_typ( _temp475); _LL444: return 1; _LL446: for( 0; _temp479 != 0;
_temp479=({ struct Cyc_List_List* _temp515= _temp479; if( _temp515 == 0){ _throw(
Null_Exception);} _temp515->tl;})){ if( ! Cyc_Toc_atomic_typ((*(( struct _tuple5*)({
struct Cyc_List_List* _temp516= _temp479; if( _temp516 == 0){ _throw(
Null_Exception);} _temp516->hd;}))).f2)){ return 0;}} return 1; _LL448: return 0;
_LL450: return 0; _LL452: return 0; _LL454: return({ struct Cyc_Absyn_Tunionfield*
_temp517= _temp493; if( _temp517 == 0){ _throw( Null_Exception);} _temp517->typs;})
== 0; _LL456: return 0; _LL458: return 0; _LL460: return 0; _LL462: return 0;
_LL464:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct
_tagged_string _temp518= Cyc_Absynpp_typ2string( t); xprintf("atomic_typ:  bad type %.*s",
_temp518.last_plus_one - _temp518.curr, _temp518.curr);})); return 0; _LL430:;}
static int Cyc_Toc_is_void_star( void* t){ void* _temp519= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_PtrInfo _temp525; struct Cyc_Absyn_Conref* _temp527; struct
Cyc_Absyn_Tqual* _temp529; struct Cyc_Absyn_Conref* _temp531; void* _temp533;
void* _temp535; _LL521: if(( unsigned int) _temp519 > 4u?(( struct
_tunion_struct*) _temp519)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL526:
_temp525=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp519)->f1; _LL536: _temp535=( void*) _temp525.elt_typ; goto _LL534; _LL534:
_temp533=( void*) _temp525.rgn_typ; goto _LL532; _LL532: _temp531=( struct Cyc_Absyn_Conref*)
_temp525.nullable; goto _LL530; _LL530: _temp529=( struct Cyc_Absyn_Tqual*)
_temp525.tq; goto _LL528; _LL528: _temp527=( struct Cyc_Absyn_Conref*) _temp525.bounds;
goto _LL522;} else{ goto _LL523;} _LL523: goto _LL524; _LL522: { void* _temp537=
Cyc_Tcutil_compress( _temp535); _LL539: if(( int) _temp537 == Cyc_Absyn_VoidType){
goto _LL540;} else{ goto _LL541;} _LL541: goto _LL542; _LL540: return 1; _LL542:
return 0; _LL538:;} _LL524: return 0; _LL520:;} static int Cyc_Toc_is_poly_field(
void* t, struct _tagged_string* f){ void* _temp543= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Structdecl** _temp555; struct Cyc_Absyn_Structdecl* _temp557;
struct Cyc_List_List* _temp558; struct _tuple1* _temp560; struct Cyc_Absyn_Structdecl**
_temp562; struct Cyc_List_List* _temp564; struct _tuple1* _temp566; struct Cyc_Absyn_Uniondecl**
_temp568; struct Cyc_Absyn_Uniondecl* _temp570; struct Cyc_List_List* _temp571;
struct _tuple1* _temp573; struct Cyc_Absyn_Uniondecl** _temp575; struct Cyc_List_List*
_temp577; struct _tuple1* _temp579; _LL545: if(( unsigned int) _temp543 > 4u?((
struct _tunion_struct*) _temp543)->tag == Cyc_Absyn_StructType_tag: 0){ _LL561:
_temp560=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp543)->f1;
goto _LL559; _LL559: _temp558=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp543)->f2; goto _LL556; _LL556: _temp555=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp543)->f3; if( _temp555 == 0){ goto
_LL547;} else{ _temp557=* _temp555; goto _LL546;}} else{ goto _LL547;} _LL547:
if(( unsigned int) _temp543 > 4u?(( struct _tunion_struct*) _temp543)->tag ==
Cyc_Absyn_StructType_tag: 0){ _LL567: _temp566=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp543)->f1; goto _LL565; _LL565: _temp564=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp543)->f2; goto _LL563; _LL563: _temp562=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp543)->f3;
if( _temp562 == 0){ goto _LL548;} else{ goto _LL549;}} else{ goto _LL549;}
_LL549: if(( unsigned int) _temp543 > 4u?(( struct _tunion_struct*) _temp543)->tag
== Cyc_Absyn_UnionType_tag: 0){ _LL574: _temp573=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp543)->f1; goto _LL572; _LL572: _temp571=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp543)->f2; goto _LL569; _LL569: _temp568=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp543)->f3;
if( _temp568 == 0){ goto _LL551;} else{ _temp570=* _temp568; goto _LL550;}}
else{ goto _LL551;} _LL551: if(( unsigned int) _temp543 > 4u?(( struct
_tunion_struct*) _temp543)->tag == Cyc_Absyn_UnionType_tag: 0){ _LL580: _temp579=(
struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*) _temp543)->f1; goto
_LL578; _LL578: _temp577=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp543)->f2; goto _LL576; _LL576: _temp575=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp543)->f3; if( _temp575 == 0){ goto
_LL552;} else{ goto _LL553;}} else{ goto _LL553;} _LL553: goto _LL554; _LL546:
if( _temp560 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp581=( char*)"StructType name not properly set";
struct _tagged_string _temp582; _temp582.curr= _temp581; _temp582.base= _temp581;
_temp582.last_plus_one= _temp581 + 33; _temp582;}));} if( _temp557->fields == 0){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string
_temp584= Cyc_Absynpp_qvar2string(( struct _tuple1*)({ struct _tuple1* _temp583=
_temp560; if( _temp583 == 0){ _throw( Null_Exception);} _temp583;})); xprintf("is_poly_field:  struct %.*s missing fields",
_temp584.last_plus_one - _temp584.curr, _temp584.curr);}));}{ struct Cyc_List_List*
fields=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp585= _temp557->fields;
if( _temp585 == 0){ _throw( Null_Exception);} _temp585->v;}); for( 0; fields !=
0; fields=({ struct Cyc_List_List* _temp586= fields; if( _temp586 == 0){ _throw(
Null_Exception);} _temp586->tl;})){ if( Cyc_String_zstrptrcmp( f,(( struct Cyc_Absyn_Structfield*)({
struct Cyc_List_List* _temp587= fields; if( _temp587 == 0){ _throw(
Null_Exception);} _temp587->hd;}))->name) == 0){ return Cyc_Toc_is_void_star((
void*)(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp588= fields;
if( _temp588 == 0){ _throw( Null_Exception);} _temp588->hd;}))->type);}}(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp590=
Cyc_Absynpp_qvar2string(( struct _tuple1*)({ struct _tuple1* _temp589= _temp560;
if( _temp589 == 0){ _throw( Null_Exception);} _temp589;})); struct
_tagged_string _temp591=* f; xprintf("is_poly_field:  struct %.*s has no %.*s field",
_temp590.last_plus_one - _temp590.curr, _temp590.curr, _temp591.last_plus_one -
_temp591.curr, _temp591.curr);})); return 0;} _LL548: if( _temp566 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp592=( char*)"StructType name not properly set"; struct _tagged_string
_temp593; _temp593.curr= _temp592; _temp593.base= _temp592; _temp593.last_plus_one=
_temp592 + 33; _temp593;}));}(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp595= Cyc_Absynpp_qvar2string(( struct _tuple1*)({
struct _tuple1* _temp594= _temp566; if( _temp594 == 0){ _throw( Null_Exception);}
_temp594;})); xprintf("is_poly_field:  struct %.*s missing structdecl", _temp595.last_plus_one
- _temp595.curr, _temp595.curr);})); return 0; _LL550: if( _temp573 == 0){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
char* _temp596=( char*)"UnionType name not properly set"; struct _tagged_string
_temp597; _temp597.curr= _temp596; _temp597.base= _temp596; _temp597.last_plus_one=
_temp596 + 32; _temp597;}));} if( _temp570->fields == 0){(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp599= Cyc_Absynpp_qvar2string((
struct _tuple1*)({ struct _tuple1* _temp598= _temp573; if( _temp598 == 0){
_throw( Null_Exception);} _temp598;})); xprintf("is_poly_field:  union %.*s missing fields",
_temp599.last_plus_one - _temp599.curr, _temp599.curr);}));}{ struct Cyc_List_List*
fields=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp600= _temp570->fields;
if( _temp600 == 0){ _throw( Null_Exception);} _temp600->v;}); for( 0; fields !=
0; fields=({ struct Cyc_List_List* _temp601= fields; if( _temp601 == 0){ _throw(
Null_Exception);} _temp601->tl;})){ if( Cyc_String_zstrptrcmp( f,(( struct Cyc_Absyn_Structfield*)({
struct Cyc_List_List* _temp602= fields; if( _temp602 == 0){ _throw(
Null_Exception);} _temp602->hd;}))->name) == 0){ return Cyc_Toc_is_void_star((
void*)(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp603= fields;
if( _temp603 == 0){ _throw( Null_Exception);} _temp603->hd;}))->type);}}(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp605=
Cyc_Absynpp_qvar2string(( struct _tuple1*)({ struct _tuple1* _temp604= _temp573;
if( _temp604 == 0){ _throw( Null_Exception);} _temp604;})); struct
_tagged_string _temp606=* f; xprintf("is_poly_field:  union %.*s has no %.*s field",
_temp605.last_plus_one - _temp605.curr, _temp605.curr, _temp606.last_plus_one -
_temp606.curr, _temp606.curr);})); return 0;} _LL552: if( _temp579 == 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp607=( char*)"UnionType name not properly set"; struct _tagged_string
_temp608; _temp608.curr= _temp607; _temp608.base= _temp607; _temp608.last_plus_one=
_temp607 + 32; _temp608;}));}(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp610= Cyc_Absynpp_qvar2string(( struct _tuple1*)({
struct _tuple1* _temp609= _temp579; if( _temp609 == 0){ _throw( Null_Exception);}
_temp609;})); xprintf("is_poly_field:  union %.*s missing structdecl", _temp610.last_plus_one
- _temp610.curr, _temp610.curr);})); return 0; _LL554:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp611= Cyc_Absynpp_typ2string(
t); xprintf("is_poly_field:  bad type %.*s", _temp611.last_plus_one - _temp611.curr,
_temp611.curr);})); return 0; _LL544:;} static int Cyc_Toc_is_poly_project(
struct Cyc_Absyn_Exp* e){ void* _temp612=( void*) e->r; struct _tagged_string*
_temp620; struct Cyc_Absyn_Exp* _temp622; struct _tagged_string* _temp624;
struct Cyc_Absyn_Exp* _temp626; _LL614: if((( struct _tunion_struct*) _temp612)->tag
== Cyc_Absyn_StructMember_e_tag){ _LL623: _temp622=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp612)->f1; goto _LL621; _LL621:
_temp620=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp612)->f2; goto _LL615;} else{ goto _LL616;} _LL616: if((( struct
_tunion_struct*) _temp612)->tag == Cyc_Absyn_StructArrow_e_tag){ _LL627:
_temp626=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp612)->f1; goto _LL625; _LL625: _temp624=( struct _tagged_string*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp612)->f2; goto _LL617;} else{ goto _LL618;}
_LL618: goto _LL619; _LL615: return Cyc_Toc_is_poly_field(( void*)({ struct Cyc_Core_Opt*
_temp628= _temp622->topt; if( _temp628 == 0){ _throw( Null_Exception);} _temp628->v;}),
_temp620); _LL617: { void* _temp630= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp629= _temp626->topt; if( _temp629 == 0){ _throw( Null_Exception);} _temp629->v;}));
struct Cyc_Absyn_PtrInfo _temp636; struct Cyc_Absyn_Conref* _temp638; struct Cyc_Absyn_Tqual*
_temp640; struct Cyc_Absyn_Conref* _temp642; void* _temp644; void* _temp646;
_LL632: if(( unsigned int) _temp630 > 4u?(( struct _tunion_struct*) _temp630)->tag
== Cyc_Absyn_PointerType_tag: 0){ _LL637: _temp636=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp630)->f1; _LL647: _temp646=( void*)
_temp636.elt_typ; goto _LL645; _LL645: _temp644=( void*) _temp636.rgn_typ; goto
_LL643; _LL643: _temp642=( struct Cyc_Absyn_Conref*) _temp636.nullable; goto
_LL641; _LL641: _temp640=( struct Cyc_Absyn_Tqual*) _temp636.tq; goto _LL639;
_LL639: _temp638=( struct Cyc_Absyn_Conref*) _temp636.bounds; goto _LL633;}
else{ goto _LL634;} _LL634: goto _LL635; _LL633: return Cyc_Toc_is_poly_field(
_temp646, _temp624); _LL635:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({
struct _tagged_string _temp649= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp648= _temp626->topt; if( _temp648 == 0){ _throw( Null_Exception);} _temp648->v;}));
xprintf("is_poly_project:  bad type %.*s", _temp649.last_plus_one - _temp649.curr,
_temp649.curr);})); return 0; _LL631:;} _LL619: return 0; _LL613:;} static char
_temp652[ 10u]="GC_malloc"; static struct _tagged_string Cyc_Toc_gc_malloc_string=(
struct _tagged_string){ _temp652, _temp652, _temp652 + 10u}; static struct
_tuple1 Cyc_Toc_gc_malloc_var=( struct _tuple1){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=&
Cyc_Toc_gc_malloc_string}; static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_ptr(
struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(& Cyc_Toc_gc_malloc_var,
0),({ struct Cyc_List_List* _temp653=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp653->hd=( void*) s; _temp653->tl= 0; _temp653;}), 0);}
static char _temp656[ 17u]="GC_malloc_atomic"; static struct _tagged_string Cyc_Toc_gc_malloc_atomic_string=(
struct _tagged_string){ _temp656, _temp656, _temp656 + 17u}; static struct
_tuple1 Cyc_Toc_gc_malloc_atomic_var=( struct _tuple1){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=&
Cyc_Toc_gc_malloc_atomic_string}; static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_atomic(
struct Cyc_Absyn_Exp* s){ return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(& Cyc_Toc_gc_malloc_atomic_var,
0),({ struct Cyc_List_List* _temp657=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp657->hd=( void*) s; _temp657->tl= 0; _temp657;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_malloc_exp( void* t, struct Cyc_Absyn_Exp*
s){ if( Cyc_Toc_atomic_typ( t)){ return Cyc_Toc_malloc_atomic( s);} return Cyc_Toc_malloc_ptr(
s);} typedef void* Cyc_Toc_Conv; static const int Cyc_Toc_NullCheck_tag= 0;
struct Cyc_Toc_NullCheck_struct{ int tag; void* f1; } ; static const int Cyc_Toc_UntagPtr_tag=
1; struct Cyc_Toc_UntagPtr_struct{ int tag; void* f1; struct Cyc_Absyn_Exp* f2;
int f3; } ; static const int Cyc_Toc_TagPtr_tag= 2; struct Cyc_Toc_TagPtr_struct{
int tag; void* f1; struct Cyc_Absyn_Exp* f2; } ; typedef void* Cyc_Toc_conv;
static struct Cyc_List_List* Cyc_Toc_conversion( void* t1, void* t2){ t1= Cyc_Tcutil_compress(
t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple0 _temp659=({ struct _tuple0
_temp658; _temp658.f1= t1; _temp658.f2= t2; _temp658;}); void* _temp665; struct
Cyc_Absyn_PtrInfo _temp667; void* _temp669; struct Cyc_Absyn_PtrInfo _temp671;
_LL661: _LL670: _temp669= _temp659.f1; if(( unsigned int) _temp669 > 4u?((
struct _tunion_struct*) _temp669)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL672:
_temp671=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp669)->f1; goto _LL666;} else{ goto _LL663;} _LL666: _temp665= _temp659.f2;
if(( unsigned int) _temp665 > 4u?(( struct _tunion_struct*) _temp665)->tag ==
Cyc_Absyn_PointerType_tag: 0){ _LL668: _temp667=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp665)->f1; goto _LL662;} else{ goto
_LL663;} _LL663: goto _LL664; _LL662: { int n1=(( int(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp671.nullable); int n2=(( int(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp667.nullable); void* b1=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp671.bounds); void* b2=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp667.bounds); struct Cyc_List_List* convs= 0;{
struct _tuple0 _temp674=({ struct _tuple0 _temp673; _temp673.f1= b1; _temp673.f2=
b2; _temp673;}); void* _temp684; struct Cyc_Absyn_Exp* _temp686; void* _temp688;
struct Cyc_Absyn_Exp* _temp690; void* _temp692; void* _temp694; void* _temp696;
void* _temp698; struct Cyc_Absyn_Exp* _temp700; void* _temp702; struct Cyc_Absyn_Exp*
_temp704; void* _temp706; _LL676: _LL689: _temp688= _temp674.f1; if((
unsigned int) _temp688 > 1u?(( struct _tunion_struct*) _temp688)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL691: _temp690=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp688)->f1; goto _LL685;} else{ goto _LL678;} _LL685: _temp684= _temp674.f2;
if(( unsigned int) _temp684 > 1u?(( struct _tunion_struct*) _temp684)->tag ==
Cyc_Absyn_Upper_b_tag: 0){ _LL687: _temp686=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Upper_b_struct*) _temp684)->f1; goto _LL677;} else{ goto _LL678;}
_LL678: _LL695: _temp694= _temp674.f1; if(( int) _temp694 == Cyc_Absyn_Unknown_b){
goto _LL693;} else{ goto _LL680;} _LL693: _temp692= _temp674.f2; if(( int)
_temp692 == Cyc_Absyn_Unknown_b){ goto _LL679;} else{ goto _LL680;} _LL680:
_LL699: _temp698= _temp674.f1; if(( unsigned int) _temp698 > 1u?(( struct
_tunion_struct*) _temp698)->tag == Cyc_Absyn_Upper_b_tag: 0){ _LL701: _temp700=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp698)->f1; goto
_LL697;} else{ goto _LL682;} _LL697: _temp696= _temp674.f2; if(( int) _temp696
== Cyc_Absyn_Unknown_b){ goto _LL681;} else{ goto _LL682;} _LL682: _LL707:
_temp706= _temp674.f1; if(( int) _temp706 == Cyc_Absyn_Unknown_b){ goto _LL703;}
else{ goto _LL675;} _LL703: _temp702= _temp674.f2; if(( unsigned int) _temp702 >
1u?(( struct _tunion_struct*) _temp702)->tag == Cyc_Absyn_Upper_b_tag: 0){
_LL705: _temp704=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp702)->f1; goto _LL683;} else{ goto _LL675;} _LL677: if( n1? ! n2: 0){ convs=({
struct Cyc_List_List* _temp708=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp708->hd=( void*)(( void*)({ struct Cyc_Toc_NullCheck_struct*
_temp709=( struct Cyc_Toc_NullCheck_struct*) GC_malloc( sizeof( struct Cyc_Toc_NullCheck_struct));
_temp709[ 0]=({ struct Cyc_Toc_NullCheck_struct _temp710; _temp710.tag= Cyc_Toc_NullCheck_tag;
_temp710.f1=( void*) t2; _temp710;}); _temp709;})); _temp708->tl= convs;
_temp708;});} goto _LL675; _LL679: goto _LL675; _LL681: convs=({ struct Cyc_List_List*
_temp711=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp711->hd=( void*)(( void*)({ struct Cyc_Toc_TagPtr_struct* _temp712=( struct
Cyc_Toc_TagPtr_struct*) GC_malloc( sizeof( struct Cyc_Toc_TagPtr_struct));
_temp712[ 0]=({ struct Cyc_Toc_TagPtr_struct _temp713; _temp713.tag= Cyc_Toc_TagPtr_tag;
_temp713.f1=( void*) Cyc_Toc_typ_to_c( t1); _temp713.f2= _temp700; _temp713;});
_temp712;})); _temp711->tl= convs; _temp711;}); goto _LL675; _LL683: convs=({
struct Cyc_List_List* _temp714=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp714->hd=( void*)(( void*)({ struct Cyc_Toc_UntagPtr_struct*
_temp715=( struct Cyc_Toc_UntagPtr_struct*) GC_malloc( sizeof( struct Cyc_Toc_UntagPtr_struct));
_temp715[ 0]=({ struct Cyc_Toc_UntagPtr_struct _temp716; _temp716.tag= Cyc_Toc_UntagPtr_tag;
_temp716.f1=( void*) Cyc_Toc_typ_to_c( t1); _temp716.f2= _temp704; _temp716.f3=
n2; _temp716;}); _temp715;})); _temp714->tl= convs; _temp714;}); goto _LL675;
_LL675:;} return convs;} _LL664: return 0; _LL660:;}} static char _temp719[ 7u]="_throw";
static struct _tagged_string Cyc_Toc__throw_string=( struct _tagged_string){
_temp719, _temp719, _temp719 + 7u}; static struct _tuple1 Cyc_Toc__throw_var=(
struct _tuple1){.f1=( void*)& Cyc_Toc_abs_ns_value,.f2=& Cyc_Toc__throw_string};
static struct Cyc_Absyn_Exp* Cyc_Toc_newthrow_exp( struct Cyc_Absyn_Exp* e){
return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(& Cyc_Toc__throw_var, 0),({
struct Cyc_List_List* _temp720=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp720->hd=( void*) e; _temp720->tl= 0; _temp720;}), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_null_check_conv( int toplevel, void* t,
struct Cyc_Absyn_Exp* e){ void* new_typ= Cyc_Toc_typ_to_c( t); if( toplevel){((
void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({
char* _temp721=( char*)"can't do null-check conversion at top-level"; struct
_tagged_string _temp722; _temp722.curr= _temp721; _temp722.base= _temp721;
_temp722.last_plus_one= _temp721 + 44; _temp722;}));}{ struct _tuple1* x= Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt* s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp(
Cyc_Absyn_null_pointer_exn_exp( 0)), 0), Cyc_Absyn_skip_stmt( 0), 0); s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( x, 0), 0), 0); return Cyc_Absyn_stmt_exp(
Cyc_Absyn_declare_stmt( x, new_typ,( struct Cyc_Absyn_Exp*) e, s, 0), 0);}}
static struct Cyc_Absyn_Exp* Cyc_Toc_tag_ptr_conv( int toplevel, void* t, struct
Cyc_Absyn_Exp* sz, struct Cyc_Absyn_Exp* e){ struct _tuple1* a= Cyc_Toc_temp_var();
struct _tuple1* b= Cyc_Toc_temp_var(); int szvalue=( int) Cyc_Evexp_eval_const_uint_exp(
sz); void* strt_typ; void* elt_typ;{ void* _temp723= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_PtrInfo _temp729; struct Cyc_Absyn_Conref* _temp731; struct Cyc_Absyn_Tqual*
_temp733; struct Cyc_Absyn_Conref* _temp735; void* _temp737; void* _temp739;
_LL725: if(( unsigned int) _temp723 > 4u?(( struct _tunion_struct*) _temp723)->tag
== Cyc_Absyn_PointerType_tag: 0){ _LL730: _temp729=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp723)->f1; _LL740: _temp739=( void*)
_temp729.elt_typ; goto _LL738; _LL738: _temp737=( void*) _temp729.rgn_typ; goto
_LL736; _LL736: _temp735=( struct Cyc_Absyn_Conref*) _temp729.nullable; goto
_LL734; _LL734: _temp733=( struct Cyc_Absyn_Tqual*) _temp729.tq; goto _LL732;
_LL732: _temp731=( struct Cyc_Absyn_Conref*) _temp729.bounds; goto _LL726;}
else{ goto _LL727;} _LL727: goto _LL728; _LL726: elt_typ= _temp739;{ struct
_tagged_string* n= Cyc_Toc_add_tagged_ptr_type( _temp739, Cyc_Toc_mt_tq);
strt_typ= Cyc_Absyn_strct( n); goto _LL724;} _LL728: return(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp741=( char*)"tag_ptr_conv: not a pointer"; struct _tagged_string _temp742;
_temp742.curr= _temp741; _temp742.base= _temp741; _temp742.last_plus_one=
_temp741 + 28; _temp742;})); _LL724:;} if( toplevel){ int is_string= 0;{ void*
_temp743=( void*) e->r; void* _temp749; struct _tagged_string _temp751; _LL745:
if((( struct _tunion_struct*) _temp743)->tag == Cyc_Absyn_Const_e_tag){ _LL750:
_temp749=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp743)->f1; if((
unsigned int) _temp749 > 1u?(( struct _tunion_struct*) _temp749)->tag == Cyc_Absyn_String_c_tag:
0){ _LL752: _temp751=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp749)->f1; goto _LL746;} else{ goto _LL747;}} else{ goto _LL747;} _LL747:
goto _LL748; _LL746: is_string= 1; goto _LL744; _LL748: goto _LL744; _LL744:;}{
struct Cyc_Absyn_Exp* xexp; if( is_string){ struct _tuple1* x= Cyc_Toc_temp_var();
void* vd_typ=( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp753=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp753[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp754; _temp754.tag= Cyc_Absyn_ArrayType_tag;
_temp754.f1=( void*) elt_typ; _temp754.f2= Cyc_Toc_mt_tq; _temp754.f3=( struct
Cyc_Absyn_Exp*) sz; _temp754;}); _temp753;}); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl(
x, vd_typ,( struct Cyc_Absyn_Exp*) e); Cyc_Toc_temp_topdecls=({ struct Cyc_List_List*
_temp755=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp755->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp756=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp756[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp757; _temp757.tag= Cyc_Absyn_Var_d_tag;
_temp757.f1= vd; _temp757;}); _temp756;}), 0); _temp755->tl= Cyc_Toc_temp_topdecls;
_temp755;}); xexp= Cyc_Absyn_var_exp( x, 0);} else{ xexp= e;}{ struct Cyc_Absyn_Exp*
xplussz= Cyc_Absyn_add_exp( xexp, sz, 0); struct Cyc_Absyn_Exp* urm_exp= Cyc_Absyn_unresolvedmem_exp(
0,({ struct Cyc_List_List* _temp758=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp758->hd=( void*)({ struct _tuple7* _temp763=(
struct _tuple7*) GC_malloc( sizeof( struct _tuple7)); _temp763->f1= 0; _temp763->f2=
xexp; _temp763;}); _temp758->tl=({ struct Cyc_List_List* _temp759=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp759->hd=( void*)({ struct
_tuple7* _temp762=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7));
_temp762->f1= 0; _temp762->f2= xexp; _temp762;}); _temp759->tl=({ struct Cyc_List_List*
_temp760=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp760->hd=( void*)({ struct _tuple7* _temp761=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp761->f1= 0; _temp761->f2= xplussz; _temp761;});
_temp760->tl= 0; _temp760;}); _temp759;}); _temp758;}), 0); return urm_exp;}}}
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
_tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char* _temp764=(
char*)"can't coerce t? to t* or t@ at the top-level"; struct _tagged_string
_temp765; _temp765.curr= _temp764; _temp765.base= _temp764; _temp765.last_plus_one=
_temp764 + 45; _temp765;}));}{ struct _tuple1* x= Cyc_Toc_temp_var(); int
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
_temp766= cs; if( _temp766 == 0){ _throw( Null_Exception);} _temp766->tl;})){
void* _temp768=( void*)({ struct Cyc_List_List* _temp767= cs; if( _temp767 == 0){
_throw( Null_Exception);} _temp767->hd;}); void* _temp776; struct Cyc_Absyn_Exp*
_temp778; void* _temp780; int _temp782; struct Cyc_Absyn_Exp* _temp784; void*
_temp786; _LL770: if((( struct _tunion_struct*) _temp768)->tag == Cyc_Toc_NullCheck_tag){
_LL777: _temp776=( void*)(( struct Cyc_Toc_NullCheck_struct*) _temp768)->f1;
goto _LL771;} else{ goto _LL772;} _LL772: if((( struct _tunion_struct*) _temp768)->tag
== Cyc_Toc_TagPtr_tag){ _LL781: _temp780=( void*)(( struct Cyc_Toc_TagPtr_struct*)
_temp768)->f1; goto _LL779; _LL779: _temp778=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Toc_TagPtr_struct*) _temp768)->f2; goto _LL773;} else{ goto _LL774;} _LL774:
if((( struct _tunion_struct*) _temp768)->tag == Cyc_Toc_UntagPtr_tag){ _LL787:
_temp786=( void*)(( struct Cyc_Toc_UntagPtr_struct*) _temp768)->f1; goto _LL785;
_LL785: _temp784=( struct Cyc_Absyn_Exp*)(( struct Cyc_Toc_UntagPtr_struct*)
_temp768)->f2; goto _LL783; _LL783: _temp782=( int)(( struct Cyc_Toc_UntagPtr_struct*)
_temp768)->f3; goto _LL775;} else{ goto _LL769;} _LL771: e= Cyc_Toc_null_check_conv(
toplevel, _temp776, e); goto _LL769; _LL773: e= Cyc_Toc_tag_ptr_conv( toplevel,
_temp780, _temp778, e); goto _LL769; _LL775: e= Cyc_Toc_untag_ptr_conv( toplevel,
_temp786, _temp784, _temp782, e); goto _LL769; _LL769:;}} return e;} struct Cyc_Toc_Env{
struct Cyc_Core_Opt* break_lab; struct Cyc_Core_Opt* continue_lab; struct Cyc_Core_Opt*
fallthru_info; struct Cyc_Dict_Dict* varmap; int toplevel; } ; typedef struct
Cyc_Toc_Env* Cyc_Toc_env_t; static struct Cyc_Toc_Env* Cyc_Toc_empty_env(){
return({ struct Cyc_Toc_Env* _temp788=( struct Cyc_Toc_Env*) GC_malloc( sizeof(
struct Cyc_Toc_Env)); _temp788->break_lab= 0; _temp788->continue_lab= 0;
_temp788->fallthru_info= 0; _temp788->varmap=(( struct Cyc_Dict_Dict*(*)( int(*
comp)( struct _tuple1*, struct _tuple1*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp);
_temp788->toplevel= 1; _temp788;});} static struct Cyc_Toc_Env* Cyc_Toc_copy_env(
struct Cyc_Toc_Env* e){ return({ struct Cyc_Toc_Env* _temp789=( struct Cyc_Toc_Env*)
GC_malloc( sizeof( struct Cyc_Toc_Env)); _temp789->break_lab= e->break_lab;
_temp789->continue_lab= e->continue_lab; _temp789->fallthru_info= e->fallthru_info;
_temp789->varmap= e->varmap; _temp789->toplevel= e->toplevel; _temp789;});}
static struct Cyc_Toc_Env* Cyc_Toc_clear_toplevel( struct Cyc_Toc_Env* e){
struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->toplevel= 0; return ans;}
static struct Cyc_Toc_Env* Cyc_Toc_add_varmap( struct Cyc_Toc_Env* e, struct
_tuple1* x, struct Cyc_Absyn_Exp* y){{ void* _temp790=(* x).f1; struct Cyc_List_List*
_temp796; _LL792: if(( unsigned int) _temp790 > 1u?(( struct _tunion_struct*)
_temp790)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL797: _temp796=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp790)->f1; goto _LL793;} else{ goto _LL794;}
_LL794: goto _LL795; _LL793:( void) _throw(( struct _xtunion_struct*)({ struct
Cyc_Core_Impossible_struct* _temp798=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp798[ 0]=({ struct
Cyc_Core_Impossible_struct _temp799; _temp799.tag= Cyc_Core_Impossible_tag;
_temp799.f1=({ struct _tagged_string _temp800= Cyc_Absynpp_qvar2string( x);
xprintf("Toc::add_varmap on Rel_n: %.*s\n", _temp800.last_plus_one - _temp800.curr,
_temp800.curr);}); _temp799;}); _temp798;})); _LL795: goto _LL791; _LL791:;}{
struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->varmap=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tuple1* key, struct Cyc_Absyn_Exp* data)) Cyc_Dict_insert)(
ans->varmap, x, y); return ans;}} static struct Cyc_Toc_Env* Cyc_Toc_loop_env(
struct Cyc_Toc_Env* e){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab=
0; ans->continue_lab= 0; return ans;} static struct Cyc_Toc_Env* Cyc_Toc_non_last_switchclause_env(
struct Cyc_Toc_Env* e, struct _tagged_string* break_l, struct _tagged_string*
fallthru_l, struct Cyc_List_List* fallthru_binders, struct Cyc_Toc_Env*
next_case_env){ struct Cyc_List_List* fallthru_vars= 0; for( 0; fallthru_binders
!= 0; fallthru_binders=({ struct Cyc_List_List* _temp801= fallthru_binders; if(
_temp801 == 0){ _throw( Null_Exception);} _temp801->tl;})){ fallthru_vars=({
struct Cyc_List_List* _temp802=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp802->hd=( void*)(( struct Cyc_Absyn_Vardecl*)({
struct Cyc_List_List* _temp803= fallthru_binders; if( _temp803 == 0){ _throw(
Null_Exception);} _temp803->hd;}))->name; _temp802->tl= fallthru_vars; _temp802;});}
fallthru_vars=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fallthru_vars);{ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env( e); ans->break_lab=({
struct Cyc_Core_Opt* _temp804=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp804->v=( void*) break_l; _temp804;}); ans->fallthru_info=({
struct Cyc_Core_Opt* _temp805=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp805->v=( void*)({ struct _tuple8* _temp806=( struct _tuple8*)
GC_malloc( sizeof( struct _tuple8)); _temp806->f1= fallthru_l; _temp806->f2=
fallthru_vars; _temp806->f3= next_case_env->varmap; _temp806;}); _temp805;});
return ans;}} static struct Cyc_Toc_Env* Cyc_Toc_last_switchclause_env( struct
Cyc_Toc_Env* e, struct _tagged_string* break_l){ struct Cyc_Toc_Env* ans= Cyc_Toc_copy_env(
e); ans->break_lab=({ struct Cyc_Core_Opt* _temp807=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp807->v=( void*) break_l; _temp807;});
ans->fallthru_info= 0; return ans;} static struct Cyc_Toc_Env* Cyc_Toc_switch_as_switch_env(
struct Cyc_Toc_Env* e, struct _tagged_string* next_l){ struct Cyc_Toc_Env* ans=
Cyc_Toc_copy_env( e); ans->break_lab= 0; ans->fallthru_info=({ struct Cyc_Core_Opt*
_temp808=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp808->v=( void*)({ struct _tuple8* _temp809=( struct _tuple8*) GC_malloc(
sizeof( struct _tuple8)); _temp809->f1= next_l; _temp809->f2= 0; _temp809->f3=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple1*, struct _tuple1*))) Cyc_Dict_empty)(
Cyc_Absyn_qvar_cmp); _temp809;}); _temp808;}); return ans;} static struct Cyc_Absyn_Exp*
Cyc_Toc_printf_to_c( void* p, struct Cyc_Core_Opt* aopt, struct Cyc_List_List*
args, struct Cyc_List_List* typs){ if( args == 0){ return(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char*
_temp810=( char*)"expecting a literal format string"; struct _tagged_string
_temp811; _temp811.curr= _temp810; _temp811.base= _temp810; _temp811.last_plus_one=
_temp810 + 34; _temp811;}));}{ struct _tagged_string fmt_str;{ void* _temp813=(
void*)(( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp812= args; if(
_temp812 == 0){ _throw( Null_Exception);} _temp812->hd;}))->r; void* _temp819;
struct _tagged_string _temp821; _LL815: if((( struct _tunion_struct*) _temp813)->tag
== Cyc_Absyn_Const_e_tag){ _LL820: _temp819=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp813)->f1; if(( unsigned int) _temp819 > 1u?(( struct _tunion_struct*)
_temp819)->tag == Cyc_Absyn_String_c_tag: 0){ _LL822: _temp821=( struct
_tagged_string)(( struct Cyc_Absyn_String_c_struct*) _temp819)->f1; goto _LL816;}
else{ goto _LL817;}} else{ goto _LL817;} _LL817: goto _LL818; _LL816: fmt_str=
_temp821; goto _LL814; _LL818: return(( struct Cyc_Absyn_Exp*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp823=(
char*)"expecting a literal format string"; struct _tagged_string _temp824;
_temp824.curr= _temp823; _temp824.base= _temp823; _temp824.last_plus_one=
_temp823 + 34; _temp824;})); _LL814:;}{ int len=( int)({ struct _tagged_string
_temp825= fmt_str;( unsigned int)( _temp825.last_plus_one - _temp825.curr);});
int i= 0; for( i= 0; i < len; i ++){ if(({ struct _tagged_string _temp826=
fmt_str; char* _temp828= _temp826.curr + i; if( _temp826.base == 0? 1:( _temp828
< _temp826.base? 1: _temp828 >= _temp826.last_plus_one)){ _throw( Null_Exception);}*
_temp828;}) =='%'){ i ++; if( i < len?({ struct _tagged_string _temp829= fmt_str;
char* _temp831= _temp829.curr + i; if( _temp829.base == 0? 1:( _temp831 <
_temp829.base? 1: _temp831 >= _temp829.last_plus_one)){ _throw( Null_Exception);}*
_temp831;}) =='s': 0){ break;}}} if( i == len){ if( aopt != 0){ args=({ struct
Cyc_List_List* _temp832=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp832->hd=( void*)(( struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp833=
aopt; if( _temp833 == 0){ _throw( Null_Exception);} _temp833->v;})); _temp832->tl=
args; _temp832;});} return Cyc_Absyn_primop_exp( p, args, 0);} args=({ struct
Cyc_List_List* _temp834= args; if( _temp834 == 0){ _throw( Null_Exception);}
_temp834->tl;}); typs=({ struct Cyc_List_List* _temp835= typs; if( _temp835 == 0){
_throw( Null_Exception);} _temp835->tl;});{ struct Cyc_List_List* rev_fmt= 0;
struct Cyc_List_List* rev_result= 0; struct Cyc_List_List* rev_temps= 0; for( i=
0; i < len; i ++){ char c=({ struct _tagged_string _temp836= fmt_str; char*
_temp838= _temp836.curr + i; if( _temp836.base == 0? 1:( _temp838 < _temp836.base?
1: _temp838 >= _temp836.last_plus_one)){ _throw( Null_Exception);}* _temp838;});
rev_fmt=({ struct Cyc_List_List* _temp839=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp839->hd=( void*)(( int) c); _temp839->tl=
rev_fmt; _temp839;}); if( c !='%'){ continue;}{ struct Cyc_Core_Opt* x= Cyc_Formatstr_parse_conversionspecification(
fmt_str, i + 1); if( x == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp840=( char*)"bad format string"; struct
_tagged_string _temp841; _temp841.curr= _temp840; _temp841.base= _temp840;
_temp841.last_plus_one= _temp840 + 18; _temp841;}));}{ struct _tuple9 _temp845;
int _temp846; char _temp848; struct Cyc_List_List* _temp850; struct Cyc_List_List*
_temp852; struct Cyc_List_List* _temp854; struct Cyc_List_List* _temp856; struct
_tuple9* _temp843=( struct _tuple9*)({ struct Cyc_Core_Opt* _temp842= x; if(
_temp842 == 0){ _throw( Null_Exception);} _temp842->v;}); _temp845=* _temp843;
_LL857: _temp856= _temp845.f1; goto _LL855; _LL855: _temp854= _temp845.f2; goto
_LL853; _LL853: _temp852= _temp845.f3; goto _LL851; _LL851: _temp850= _temp845.f4;
goto _LL849; _LL849: _temp848= _temp845.f5; goto _LL847; _LL847: _temp846=
_temp845.f6; goto _LL844; _LL844: i= _temp846 - 1; if( _temp848 !='s'){ rev_fmt=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_revappend)(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)(({ struct
Cyc_List_List* _temp858=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp858->hd=( void*) _temp856; _temp858->tl=({ struct Cyc_List_List* _temp859=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp859->hd=(
void*) _temp854; _temp859->tl=({ struct Cyc_List_List* _temp860=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp860->hd=( void*) _temp852;
_temp860->tl=({ struct Cyc_List_List* _temp861=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp861->hd=( void*) _temp850;
_temp861->tl= 0; _temp861;}); _temp860;}); _temp859;}); _temp858;})), rev_fmt);
rev_fmt=({ struct Cyc_List_List* _temp862=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp862->hd=( void*)(( int) _temp848); _temp862->tl=
rev_fmt; _temp862;});{ struct Cyc_List_List* _temp863= _temp854; struct Cyc_List_List
_temp870; struct Cyc_List_List* _temp871; int _temp873; _LL865: if( _temp863 ==
0){ goto _LL867;} else{ _temp870=* _temp863; _LL874: _temp873=( int) _temp870.hd;
goto _LL872; _LL872: _temp871=( struct Cyc_List_List*) _temp870.tl; if( _temp871
== 0){ goto _LL869;} else{ goto _LL867;}} _LL869: if( _temp873 =='*'){ goto
_LL866;} else{ goto _LL867;} _LL867: goto _LL868; _LL866: { struct _tuple1* temp=
Cyc_Toc_temp_var(); rev_temps=({ struct Cyc_List_List* _temp875=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp875->hd=( void*)({ struct
_tuple10* _temp876=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10));
_temp876->f1= temp; _temp876->f2=( void*)({ struct Cyc_List_List* _temp878= typs;
if( _temp878 == 0){ _throw( Null_Exception);} _temp878->hd;}); _temp876->f3=(
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp877= args; if( _temp877 == 0){
_throw( Null_Exception);} _temp877->hd;}); _temp876;}); _temp875->tl= rev_temps;
_temp875;}); rev_result=({ struct Cyc_List_List* _temp879=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp879->hd=( void*) Cyc_Absyn_var_exp(
temp, 0); _temp879->tl= rev_result; _temp879;}); args=({ struct Cyc_List_List*
_temp880= args; if( _temp880 == 0){ _throw( Null_Exception);} _temp880->tl;});
typs=({ struct Cyc_List_List* _temp881= typs; if( _temp881 == 0){ _throw(
Null_Exception);} _temp881->tl;}); goto _LL864;} _LL868: goto _LL864; _LL864:;}{
struct Cyc_List_List* _temp882= _temp852; struct Cyc_List_List _temp889; struct
Cyc_List_List* _temp890; struct Cyc_List_List _temp892; struct Cyc_List_List*
_temp893; int _temp895; int _temp897; _LL884: if( _temp882 == 0){ goto _LL886;}
else{ _temp889=* _temp882; _LL898: _temp897=( int) _temp889.hd; goto _LL891;
_LL891: _temp890=( struct Cyc_List_List*) _temp889.tl; if( _temp890 == 0){ goto
_LL886;} else{ _temp892=* _temp890; _LL896: _temp895=( int) _temp892.hd; goto
_LL894; _LL894: _temp893=( struct Cyc_List_List*) _temp892.tl; if( _temp893 == 0){
goto _LL888;} else{ goto _LL886;}}} _LL888: if( _temp897 =='.'? _temp895 =='*':
0){ goto _LL885;} else{ goto _LL886;} _LL886: goto _LL887; _LL885: { struct
_tuple1* temp= Cyc_Toc_temp_var(); rev_temps=({ struct Cyc_List_List* _temp899=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp899->hd=(
void*)({ struct _tuple10* _temp900=( struct _tuple10*) GC_malloc( sizeof( struct
_tuple10)); _temp900->f1= temp; _temp900->f2=( void*)({ struct Cyc_List_List*
_temp902= typs; if( _temp902 == 0){ _throw( Null_Exception);} _temp902->hd;});
_temp900->f3=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp901= args;
if( _temp901 == 0){ _throw( Null_Exception);} _temp901->hd;}); _temp900;});
_temp899->tl= rev_temps; _temp899;}); rev_result=({ struct Cyc_List_List*
_temp903=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp903->hd=( void*) Cyc_Absyn_var_exp( temp, 0); _temp903->tl= rev_result;
_temp903;}); args=({ struct Cyc_List_List* _temp904= args; if( _temp904 == 0){
_throw( Null_Exception);} _temp904->tl;}); typs=({ struct Cyc_List_List*
_temp905= typs; if( _temp905 == 0){ _throw( Null_Exception);} _temp905->tl;});
goto _LL883;} _LL887: goto _LL883; _LL883:;} if( _temp848 !='%'){ if( args == 0?
1: typs == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct
_tagged_string _temp906= Cyc_Absynpp_prim2string( p); xprintf("missing arg to %.*s",
_temp906.last_plus_one - _temp906.curr, _temp906.curr);}));}{ struct _tuple1*
temp= Cyc_Toc_temp_var(); rev_temps=({ struct Cyc_List_List* _temp907=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp907->hd=( void*)({
struct _tuple10* _temp908=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10));
_temp908->f1= temp; _temp908->f2=( void*)({ struct Cyc_List_List* _temp910= typs;
if( _temp910 == 0){ _throw( Null_Exception);} _temp910->hd;}); _temp908->f3=(
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp909= args; if( _temp909 == 0){
_throw( Null_Exception);} _temp909->hd;}); _temp908;}); _temp907->tl= rev_temps;
_temp907;}); rev_result=({ struct Cyc_List_List* _temp911=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp911->hd=( void*) Cyc_Absyn_var_exp(
temp, 0); _temp911->tl= rev_result; _temp911;}); args=({ struct Cyc_List_List*
_temp912= args; if( _temp912 == 0){ _throw( Null_Exception);} _temp912->tl;});
typs=({ struct Cyc_List_List* _temp913= typs; if( _temp913 == 0){ _throw(
Null_Exception);} _temp913->tl;});}}} else{ rev_fmt=({ struct Cyc_List_List*
_temp914=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp914->hd=( void*)(( int)'.'); _temp914->tl= rev_fmt; _temp914;}); rev_fmt=({
struct Cyc_List_List* _temp915=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp915->hd=( void*)(( int)'*'); _temp915->tl= rev_fmt;
_temp915;}); rev_fmt=({ struct Cyc_List_List* _temp916=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp916->hd=( void*)(( int)'s');
_temp916->tl= rev_fmt; _temp916;});{ struct _tuple1* temp= Cyc_Toc_temp_var();
rev_temps=({ struct Cyc_List_List* _temp917=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp917->hd=( void*)({ struct _tuple10*
_temp918=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp918->f1=
temp; _temp918->f2=( void*)({ struct Cyc_List_List* _temp920= typs; if( _temp920
== 0){ _throw( Null_Exception);} _temp920->hd;}); _temp918->f3=( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp919= args; if( _temp919 == 0){ _throw( Null_Exception);}
_temp919->hd;}); _temp918;}); _temp917->tl= rev_temps; _temp917;});{ struct Cyc_Absyn_Exp*
size_exp= Cyc_Absyn_subtract_exp( Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
temp, 0), Cyc_Toc_last_plus_one_sp, 0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
temp, 0), Cyc_Toc_curr_sp, 0), 0); struct Cyc_Absyn_Exp* cstring_exp= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0); rev_result=({ struct Cyc_List_List*
_temp921=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp921->hd=( void*) size_exp; _temp921->tl= rev_result; _temp921;});
rev_result=({ struct Cyc_List_List* _temp922=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp922->hd=( void*) cstring_exp; _temp922->tl=
rev_result; _temp922;}); args=({ struct Cyc_List_List* _temp923= args; if(
_temp923 == 0){ _throw( Null_Exception);} _temp923->tl;}); typs=({ struct Cyc_List_List*
_temp924= typs; if( _temp924 == 0){ _throw( Null_Exception);} _temp924->tl;});}}}}}}{
struct Cyc_List_List* es=({ struct Cyc_List_List* _temp925=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp925->hd=( void*) Cyc_Absyn_string_exp(
Cyc_String_implode((( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
rev_fmt)), 0); _temp925->tl=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_rev)( rev_result); _temp925;}); if( aopt != 0){ es=({ struct Cyc_List_List*
_temp926=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp926->hd=( void*)(( struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp927=
aopt; if( _temp927 == 0){ _throw( Null_Exception);} _temp927->v;})); _temp926->tl=
es; _temp926;});}{ struct Cyc_Absyn_Exp* e= Cyc_Absyn_primop_exp( p, es, 0);
struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( e, 0); for( 0; rev_temps != 0;
rev_temps=({ struct Cyc_List_List* _temp928= rev_temps; if( _temp928 == 0){
_throw( Null_Exception);} _temp928->tl;})){ s= Cyc_Absyn_declare_stmt((*((
struct _tuple10*)({ struct Cyc_List_List* _temp929= rev_temps; if( _temp929 == 0){
_throw( Null_Exception);} _temp929->hd;}))).f1,(*(( struct _tuple10*)({ struct
Cyc_List_List* _temp930= rev_temps; if( _temp930 == 0){ _throw( Null_Exception);}
_temp930->hd;}))).f2,( struct Cyc_Absyn_Exp*)(*(( struct _tuple10*)({ struct Cyc_List_List*
_temp931= rev_temps; if( _temp931 == 0){ _throw( Null_Exception);} _temp931->hd;}))).f3,
s, 0);} return Cyc_Absyn_stmt_exp( s, 0);}}}}}} static void* Cyc_Toc_get_c_typ(
struct Cyc_Absyn_Exp* e){ if( e->topt == 0){(( void(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp932=( char*)"Missing type in primop ";
struct _tagged_string _temp933; _temp933.curr= _temp932; _temp933.base= _temp932;
_temp933.last_plus_one= _temp932 + 24; _temp933;}));} return Cyc_Toc_typ_to_c((
void*)({ struct Cyc_Core_Opt* _temp934= e->topt; if( _temp934 == 0){ _throw(
Null_Exception);} _temp934->v;}));} static void* Cyc_Toc_get_cyc_typ( struct Cyc_Absyn_Exp*
e){ if( e->topt == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp935=( char*)"Missing type in primop ";
struct _tagged_string _temp936; _temp936.curr= _temp935; _temp936.base= _temp935;
_temp936.last_plus_one= _temp935 + 24; _temp936;}));} return( void*)({ struct
Cyc_Core_Opt* _temp937= e->topt; if( _temp937 == 0){ _throw( Null_Exception);}
_temp937->v;});} static struct _tuple5* Cyc_Toc_tup_to_c( struct Cyc_Absyn_Exp*
e){ return({ struct _tuple5* _temp938=( struct _tuple5*) GC_malloc( sizeof(
struct _tuple5)); _temp938->f1= Cyc_Toc_mt_tq; _temp938->f2= Cyc_Toc_typ_to_c((
void*)({ struct Cyc_Core_Opt* _temp939= e->topt; if( _temp939 == 0){ _throw(
Null_Exception);} _temp939->v;})); _temp938;});} static void Cyc_Toc_exp_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* e); static struct _tuple7* Cyc_Toc_add_designator(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Exp* e){ Cyc_Toc_exp_to_c( nv, e);
return({ struct _tuple7* _temp940=( struct _tuple7*) GC_malloc( sizeof( struct
_tuple7) * 1); _temp940[ 0]=({ struct _tuple7 _temp941; _temp941.f1= 0; _temp941.f2=
e; _temp941;}); _temp940;});} static struct Cyc_Absyn_Exp* Cyc_Toc_make_struct(
struct _tuple1* x, void* struct_typ, struct Cyc_Absyn_Stmt* s, int pointer, int
is_atomic){ struct Cyc_Absyn_Exp* eo; void* t; if( pointer){ t= Cyc_Absyn_cstar_typ(
struct_typ, Cyc_Toc_mt_tq);{ struct Cyc_Absyn_Exp* se= Cyc_Absyn_sizeoftyp_exp(
struct_typ, 0); eo=( struct Cyc_Absyn_Exp*) Cyc_Absyn_cast_exp( t, is_atomic?
Cyc_Toc_malloc_atomic( se): Cyc_Toc_malloc_ptr( se), 0);}} else{ t= struct_typ;
eo= 0;} return Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, t, eo, s, 0), 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_init_tuple( struct Cyc_Toc_Env* nv, int
pointer, struct Cyc_List_List* es){ struct Cyc_List_List* tqs=(( struct Cyc_List_List*(*)(
struct _tuple5*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Toc_tup_to_c, es); struct _tagged_string* n= Cyc_Toc_add_tuple_type( tqs);
struct _tuple1* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp(
x, 0); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( xexp, 0); struct Cyc_Absyn_Exp*(*
lval_f)( struct Cyc_Absyn_Exp*, struct _tagged_string*, struct Cyc_Position_Segment*)=
pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp; int is_atomic= 1;
es=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( es);{
int i=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es); for( 0; es !=
0; es=({ struct Cyc_List_List* _temp942= es; if( _temp942 == 0){ _throw(
Null_Exception);} _temp942->tl;}), -- i){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp943= es; if( _temp943 == 0){ _throw( Null_Exception);}
_temp943->hd;})); is_atomic= is_atomic? Cyc_Toc_atomic_typ(( void*)({ struct Cyc_Core_Opt*
_temp945=(( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp944= es; if(
_temp944 == 0){ _throw( Null_Exception);} _temp944->hd;}))->topt; if( _temp945
== 0){ _throw( Null_Exception);} _temp945->v;})): 0; s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval_f( xexp,({ struct _tagged_string* _temp946=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp946[ 0]=
xprintf("f%d", i); _temp946;}), 0),( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp947= es; if( _temp947 == 0){ _throw( Null_Exception);} _temp947->hd;}), 0),
0), s, 0);}} return Cyc_Toc_make_struct( x, Cyc_Absyn_strct( n), s, pointer,
is_atomic);} static struct Cyc_Absyn_Exp* Cyc_Toc_init_struct( struct Cyc_Toc_Env*
nv, void* struct_type, int pointer, struct Cyc_List_List* dles, struct _tuple1*
tdn){ struct _tuple1* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp(
x, 0); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( xexp, 0); struct Cyc_Absyn_Exp*(*
lval_f)( struct Cyc_Absyn_Exp*, struct _tagged_string*, struct Cyc_Position_Segment*)=
pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp; int is_atomic= 1;{
struct Cyc_List_List* es=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_rev)( dles); for( 0; es != 0; es=({ struct Cyc_List_List* _temp948= es;
if( _temp948 == 0){ _throw( Null_Exception);} _temp948->tl;})){ struct _tuple7
_temp952; struct Cyc_Absyn_Exp* _temp953; struct Cyc_List_List* _temp955; struct
_tuple7* _temp950=( struct _tuple7*)({ struct Cyc_List_List* _temp949= es; if(
_temp949 == 0){ _throw( Null_Exception);} _temp949->hd;}); _temp952=* _temp950;
_LL956: _temp955= _temp952.f1; goto _LL954; _LL954: _temp953= _temp952.f2; goto
_LL951; _LL951: is_atomic= is_atomic? Cyc_Toc_atomic_typ(( void*)({ struct Cyc_Core_Opt*
_temp957= _temp953->topt; if( _temp957 == 0){ _throw( Null_Exception);} _temp957->v;})):
0; if( _temp955 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp958=( char*)"empty designator list"; struct
_tagged_string _temp959; _temp959.curr= _temp958; _temp959.base= _temp958;
_temp959.last_plus_one= _temp958 + 22; _temp959;}));} if(({ struct Cyc_List_List*
_temp960= _temp955; if( _temp960 == 0){ _throw( Null_Exception);} _temp960->tl;})
!= 0){ struct _tuple1* y= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* yexp= Cyc_Absyn_var_exp(
y, 0); for( 0; _temp955 != 0; _temp955=({ struct Cyc_List_List* _temp961=
_temp955; if( _temp961 == 0){ _throw( Null_Exception);} _temp961->tl;})){ void*
_temp963=( void*)({ struct Cyc_List_List* _temp962= _temp955; if( _temp962 == 0){
_throw( Null_Exception);} _temp962->hd;}); struct _tagged_string* _temp969;
_LL965: if((( struct _tunion_struct*) _temp963)->tag == Cyc_Absyn_FieldName_tag){
_LL970: _temp969=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp963)->f1; goto _LL966;} else{ goto _LL967;} _LL967: goto _LL968; _LL966:
if( Cyc_Toc_is_poly_field( struct_type, _temp969)){ yexp= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), yexp, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval_f( xexp, _temp969, 0), yexp, 0), 0), s, 0); goto
_LL964; _LL968:( void) _throw((( struct _xtunion_struct*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp971=(
char*)"array designator in struct"; struct _tagged_string _temp972; _temp972.curr=
_temp971; _temp972.base= _temp971; _temp972.last_plus_one= _temp971 + 27;
_temp972;}))); _LL964:;} Cyc_Toc_exp_to_c( nv, _temp953); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( yexp, _temp953, 0), 0), s, 0);} else{
void* _temp974=( void*)({ struct Cyc_List_List* _temp973= _temp955; if( _temp973
== 0){ _throw( Null_Exception);} _temp973->hd;}); struct _tagged_string*
_temp980; _LL976: if((( struct _tunion_struct*) _temp974)->tag == Cyc_Absyn_FieldName_tag){
_LL981: _temp980=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp974)->f1; goto _LL977;} else{ goto _LL978;} _LL978: goto _LL979; _LL977:
Cyc_Toc_exp_to_c( nv, _temp953); if( Cyc_Toc_is_poly_field( struct_type,
_temp980)){ _temp953= Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(), _temp953, 0);}
s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( lval_f( xexp,
_temp980, 0), _temp953, 0), 0), s, 0); goto _LL975; _LL979:( void) _throw(((
struct _xtunion_struct*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp982=( char*)"array designator in struct";
struct _tagged_string _temp983; _temp983.curr= _temp982; _temp983.base= _temp982;
_temp983.last_plus_one= _temp982 + 27; _temp983;}))); _LL975:;}}} return Cyc_Toc_make_struct(
x, Cyc_Absyn_strctq( tdn), s, pointer, is_atomic);} static struct Cyc_Absyn_Exp*
Cyc_Toc_assignop_lvalue( struct Cyc_Absyn_Exp* el, struct _tuple11* pr){ return
Cyc_Absyn_assignop_exp( el,(* pr).f1,(* pr).f2, 0);} static struct Cyc_Absyn_Exp*
Cyc_Toc_address_lvalue( struct Cyc_Absyn_Exp* e1, int ignore){ return Cyc_Absyn_address_exp(
e1, 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_incr_lvalue( struct Cyc_Absyn_Exp*
e1, void* incr){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Increment_e_struct*
_temp984=( struct Cyc_Absyn_Increment_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Increment_e_struct));
_temp984[ 0]=({ struct Cyc_Absyn_Increment_e_struct _temp985; _temp985.tag= Cyc_Absyn_Increment_e_tag;
_temp985.f1= e1; _temp985.f2=( void*) incr; _temp985;}); _temp984;}), 0);}
static void Cyc_Toc_lvalue_assign_stmt( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env);
static void Cyc_Toc_lvalue_assign( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env){
void* _temp986=( void*) e1->r; struct Cyc_Absyn_Stmt* _temp996; struct Cyc_Absyn_Exp*
_temp998; void* _temp1000; struct _tagged_string* _temp1002; struct Cyc_Absyn_Exp*
_temp1004; _LL988: if((( struct _tunion_struct*) _temp986)->tag == Cyc_Absyn_StmtExp_e_tag){
_LL997: _temp996=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp986)->f1; goto _LL989;} else{ goto _LL990;} _LL990: if((( struct
_tunion_struct*) _temp986)->tag == Cyc_Absyn_Cast_e_tag){ _LL1001: _temp1000=(
void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp986)->f1; goto _LL999; _LL999:
_temp998=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*) _temp986)->f2;
goto _LL991;} else{ goto _LL992;} _LL992: if((( struct _tunion_struct*) _temp986)->tag
== Cyc_Absyn_StructMember_e_tag){ _LL1005: _temp1004=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp986)->f1; goto _LL1003; _LL1003:
_temp1002=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp986)->f2; goto _LL993;} else{ goto _LL994;} _LL994: goto _LL995; _LL989:((
void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(*
f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp996, fs, f, f_env); goto _LL987; _LL991:(( void(*)( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp998, fs, f, f_env); goto
_LL987; _LL993:( void*)( e1->r=( void*)(( void*) _temp1004->r));(( void(*)(
struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)( e1,({
struct Cyc_List_List* _temp1006=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1006->hd=( void*) _temp1002; _temp1006->tl= fs;
_temp1006;}), f, f_env); goto _LL987; _LL995: if( ! Cyc_Absyn_is_lvalue( e1)){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string
_temp1007= Cyc_Absynpp_exp2string( e1); xprintf("lvalue_assign: could not find lvalue in `%.*s'",
_temp1007.last_plus_one - _temp1007.curr, _temp1007.curr);}));}{ struct Cyc_Absyn_Exp*
e1_copy= Cyc_Absyn_copy_exp( e1); for( 0; fs != 0; fs=({ struct Cyc_List_List*
_temp1008= fs; if( _temp1008 == 0){ _throw( Null_Exception);} _temp1008->tl;})){
e1_copy= Cyc_Absyn_structmember_exp( e1_copy,( struct _tagged_string*)({ struct
Cyc_List_List* _temp1009= fs; if( _temp1009 == 0){ _throw( Null_Exception);}
_temp1009->hd;}), e1_copy->loc);}( void*)( e1->r=( void*)(( void*)( f( e1_copy,
f_env))->r)); goto _LL987;} _LL987:;} static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, void*), void* f_env){ void* _temp1010=( void*) s->r;
struct Cyc_Absyn_Exp* _temp1020; struct Cyc_Absyn_Stmt* _temp1022; struct Cyc_Absyn_Decl*
_temp1024; struct Cyc_Absyn_Stmt* _temp1026; struct Cyc_Absyn_Stmt* _temp1028;
_LL1012: if(( unsigned int) _temp1010 > 1u?(( struct _tunion_struct*) _temp1010)->tag
== Cyc_Absyn_Exp_s_tag: 0){ _LL1021: _temp1020=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Exp_s_struct*) _temp1010)->f1; goto _LL1013;} else{ goto _LL1014;}
_LL1014: if(( unsigned int) _temp1010 > 1u?(( struct _tunion_struct*) _temp1010)->tag
== Cyc_Absyn_Decl_s_tag: 0){ _LL1025: _temp1024=( struct Cyc_Absyn_Decl*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1010)->f1; goto _LL1023; _LL1023:
_temp1022=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1010)->f2;
goto _LL1015;} else{ goto _LL1016;} _LL1016: if(( unsigned int) _temp1010 > 1u?((
struct _tunion_struct*) _temp1010)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL1029:
_temp1028=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1010)->f1;
goto _LL1027; _LL1027: _temp1026=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp1010)->f2; goto _LL1017;} else{ goto _LL1018;} _LL1018: goto _LL1019;
_LL1013:(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct
Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)(
_temp1020, fs, f, f_env); goto _LL1011; _LL1015:(( void(*)( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)( _temp1022, fs, f, f_env);
goto _LL1011; _LL1017:(( void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp1026, fs, f, f_env); goto _LL1011; _LL1019:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1030= Cyc_Absynpp_stmt2string(
s); xprintf("lvalue_assign_stmt: %.*s", _temp1030.last_plus_one - _temp1030.curr,
_temp1030.curr);})); goto _LL1011; _LL1011:;} static struct Cyc_Absyn_Exp* Cyc_Toc_known_size_subscript(
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
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1031=
Cyc_Absynpp_exp2string( e); xprintf("exp_to_c: no type for %.*s", _temp1031.last_plus_one
- _temp1031.curr, _temp1031.curr);})); return;}{ void* old_typ=( void*)({ struct
Cyc_Core_Opt* _temp1032= e->topt; if( _temp1032 == 0){ _throw( Null_Exception);}
_temp1032->v;}); void* _temp1033= r; void* _temp1109; void* _temp1111; void*
_temp1113; struct _tuple1* _temp1115; struct _tuple1* _temp1117; struct Cyc_List_List*
_temp1119; void* _temp1121; void* _temp1123; struct Cyc_Absyn_Exp* _temp1125;
struct Cyc_Absyn_Exp* _temp1127; struct Cyc_Core_Opt* _temp1129; struct Cyc_Absyn_Exp*
_temp1131; struct Cyc_Absyn_Exp* _temp1133; struct Cyc_Absyn_Exp* _temp1135;
struct Cyc_Absyn_Exp* _temp1137; struct Cyc_Absyn_Exp* _temp1139; struct Cyc_Absyn_Exp*
_temp1141; struct Cyc_List_List* _temp1143; struct Cyc_Absyn_Exp* _temp1145;
struct Cyc_List_List* _temp1147; struct Cyc_Absyn_Exp* _temp1149; struct Cyc_Absyn_Exp*
_temp1151; struct Cyc_Absyn_Exp* _temp1153; struct Cyc_List_List* _temp1155;
struct Cyc_Absyn_Exp* _temp1157; struct Cyc_Absyn_Exp* _temp1159; struct Cyc_Absyn_Exp**
_temp1161; void* _temp1162; void** _temp1164; struct Cyc_Absyn_Exp* _temp1165;
struct Cyc_Absyn_Exp* _temp1167; void* _temp1169; struct Cyc_Absyn_Exp*
_temp1171; struct Cyc_Absyn_Exp* _temp1173; struct _tagged_string* _temp1175;
struct Cyc_Absyn_Exp* _temp1177; struct _tagged_string* _temp1179; struct Cyc_Absyn_Exp*
_temp1181; struct Cyc_Absyn_Exp* _temp1183; struct Cyc_Absyn_Exp* _temp1185;
struct Cyc_List_List* _temp1187; struct Cyc_List_List* _temp1189; struct _tuple2*
_temp1191; struct Cyc_List_List* _temp1193; struct Cyc_Absyn_Exp* _temp1195;
struct Cyc_Absyn_Exp* _temp1197; struct Cyc_Absyn_Vardecl* _temp1199; struct Cyc_Absyn_Structdecl*
_temp1201; struct Cyc_List_List* _temp1203; struct Cyc_Core_Opt* _temp1205;
struct _tuple1* _temp1207; struct Cyc_Absyn_Tunionfield* _temp1209; struct Cyc_Absyn_Tuniondecl*
_temp1211; struct Cyc_List_List* _temp1213; struct Cyc_Core_Opt* _temp1215;
struct Cyc_Core_Opt* _temp1217; struct Cyc_Absyn_Tunionfield* _temp1219; struct
Cyc_Absyn_Tuniondecl* _temp1221; struct Cyc_List_List* _temp1223; struct Cyc_Core_Opt*
_temp1225; struct Cyc_Core_Opt* _temp1227; struct Cyc_Absyn_Tunionfield*
_temp1229; struct Cyc_Absyn_XTuniondecl* _temp1231; struct Cyc_List_List*
_temp1233; struct Cyc_Core_Opt* _temp1235; struct Cyc_Absyn_Enumfield* _temp1237;
struct Cyc_Absyn_Enumdecl* _temp1239; struct _tuple1* _temp1241; void* _temp1243;
struct Cyc_Absyn_Stmt* _temp1245; struct Cyc_List_List* _temp1247; struct Cyc_Core_Opt*
_temp1249; struct Cyc_Absyn_Fndecl* _temp1251; struct Cyc_Absyn_Exp* _temp1253;
_LL1035: if((( struct _tunion_struct*) _temp1033)->tag == Cyc_Absyn_Const_e_tag){
_LL1110: _temp1109=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1033)->f1;
if(( int) _temp1109 == Cyc_Absyn_Null_c){ goto _LL1036;} else{ goto _LL1037;}}
else{ goto _LL1037;} _LL1037: if((( struct _tunion_struct*) _temp1033)->tag ==
Cyc_Absyn_Const_e_tag){ _LL1112: _temp1111=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1033)->f1; goto _LL1038;} else{ goto _LL1039;} _LL1039: if((( struct
_tunion_struct*) _temp1033)->tag == Cyc_Absyn_Var_e_tag){ _LL1116: _temp1115=(
struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp1033)->f1; goto _LL1114;
_LL1114: _temp1113=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1033)->f2;
goto _LL1040;} else{ goto _LL1041;} _LL1041: if((( struct _tunion_struct*)
_temp1033)->tag == Cyc_Absyn_UnknownId_e_tag){ _LL1118: _temp1117=( struct
_tuple1*)(( struct Cyc_Absyn_UnknownId_e_struct*) _temp1033)->f1; goto _LL1042;}
else{ goto _LL1043;} _LL1043: if((( struct _tunion_struct*) _temp1033)->tag ==
Cyc_Absyn_Primop_e_tag){ _LL1122: _temp1121=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp1033)->f1; goto _LL1120; _LL1120: _temp1119=( struct Cyc_List_List*)((
struct Cyc_Absyn_Primop_e_struct*) _temp1033)->f2; goto _LL1044;} else{ goto
_LL1045;} _LL1045: if((( struct _tunion_struct*) _temp1033)->tag == Cyc_Absyn_Increment_e_tag){
_LL1126: _temp1125=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1033)->f1; goto _LL1124; _LL1124: _temp1123=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1033)->f2; goto _LL1046;} else{ goto _LL1047;} _LL1047: if((( struct
_tunion_struct*) _temp1033)->tag == Cyc_Absyn_AssignOp_e_tag){ _LL1132:
_temp1131=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1033)->f1; goto _LL1130; _LL1130: _temp1129=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1033)->f2; goto _LL1128; _LL1128:
_temp1127=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1033)->f3; goto _LL1048;} else{ goto _LL1049;} _LL1049: if((( struct
_tunion_struct*) _temp1033)->tag == Cyc_Absyn_Conditional_e_tag){ _LL1138:
_temp1137=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1033)->f1; goto _LL1136; _LL1136: _temp1135=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp1033)->f2; goto _LL1134; _LL1134:
_temp1133=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1033)->f3; goto _LL1050;} else{ goto _LL1051;} _LL1051: if((( struct
_tunion_struct*) _temp1033)->tag == Cyc_Absyn_SeqExp_e_tag){ _LL1142: _temp1141=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*) _temp1033)->f1; goto
_LL1140; _LL1140: _temp1139=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1033)->f2; goto _LL1052;} else{ goto _LL1053;} _LL1053: if((( struct
_tunion_struct*) _temp1033)->tag == Cyc_Absyn_UnknownCall_e_tag){ _LL1146:
_temp1145=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1033)->f1; goto _LL1144; _LL1144: _temp1143=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_e_struct*) _temp1033)->f2; goto _LL1054;} else{
goto _LL1055;} _LL1055: if((( struct _tunion_struct*) _temp1033)->tag == Cyc_Absyn_FnCall_e_tag){
_LL1150: _temp1149=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1033)->f1; goto _LL1148; _LL1148: _temp1147=( struct Cyc_List_List*)((
struct Cyc_Absyn_FnCall_e_struct*) _temp1033)->f2; goto _LL1056;} else{ goto
_LL1057;} _LL1057: if((( struct _tunion_struct*) _temp1033)->tag == Cyc_Absyn_Throw_e_tag){
_LL1152: _temp1151=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*)
_temp1033)->f1; goto _LL1058;} else{ goto _LL1059;} _LL1059: if((( struct
_tunion_struct*) _temp1033)->tag == Cyc_Absyn_NoInstantiate_e_tag){ _LL1154:
_temp1153=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1033)->f1; goto _LL1060;} else{ goto _LL1061;} _LL1061: if((( struct
_tunion_struct*) _temp1033)->tag == Cyc_Absyn_Instantiate_e_tag){ _LL1158:
_temp1157=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1033)->f1; goto _LL1156; _LL1156: _temp1155=( struct Cyc_List_List*)((
struct Cyc_Absyn_Instantiate_e_struct*) _temp1033)->f2; goto _LL1062;} else{
goto _LL1063;} _LL1063: if((( struct _tunion_struct*) _temp1033)->tag == Cyc_Absyn_Cast_e_tag){
_LL1163: _temp1162=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp1033)->f1;
_temp1164=&(( struct Cyc_Absyn_Cast_e_struct*) _temp1033)->f1; goto _LL1160;
_LL1160: _temp1159=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1033)->f2; _temp1161=&(( struct Cyc_Absyn_Cast_e_struct*) _temp1033)->f2;
goto _LL1064;} else{ goto _LL1065;} _LL1065: if((( struct _tunion_struct*)
_temp1033)->tag == Cyc_Absyn_Address_e_tag){ _LL1166: _temp1165=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Address_e_struct*) _temp1033)->f1; goto _LL1066;} else{ goto
_LL1067;} _LL1067: if((( struct _tunion_struct*) _temp1033)->tag == Cyc_Absyn_New_e_tag){
_LL1168: _temp1167=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*)
_temp1033)->f1; goto _LL1068;} else{ goto _LL1069;} _LL1069: if((( struct
_tunion_struct*) _temp1033)->tag == Cyc_Absyn_Sizeoftyp_e_tag){ _LL1170:
_temp1169=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp1033)->f1; goto
_LL1070;} else{ goto _LL1071;} _LL1071: if((( struct _tunion_struct*) _temp1033)->tag
== Cyc_Absyn_Sizeofexp_e_tag){ _LL1172: _temp1171=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Sizeofexp_e_struct*) _temp1033)->f1; goto _LL1072;} else{ goto
_LL1073;} _LL1073: if((( struct _tunion_struct*) _temp1033)->tag == Cyc_Absyn_Deref_e_tag){
_LL1174: _temp1173=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp1033)->f1; goto _LL1074;} else{ goto _LL1075;} _LL1075: if((( struct
_tunion_struct*) _temp1033)->tag == Cyc_Absyn_StructMember_e_tag){ _LL1178:
_temp1177=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1033)->f1; goto _LL1176; _LL1176: _temp1175=( struct _tagged_string*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp1033)->f2; goto _LL1076;} else{
goto _LL1077;} _LL1077: if((( struct _tunion_struct*) _temp1033)->tag == Cyc_Absyn_StructArrow_e_tag){
_LL1182: _temp1181=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1033)->f1; goto _LL1180; _LL1180: _temp1179=( struct _tagged_string*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp1033)->f2; goto _LL1078;} else{
goto _LL1079;} _LL1079: if((( struct _tunion_struct*) _temp1033)->tag == Cyc_Absyn_Subscript_e_tag){
_LL1186: _temp1185=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1033)->f1; goto _LL1184; _LL1184: _temp1183=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp1033)->f2; goto _LL1080;} else{ goto
_LL1081;} _LL1081: if((( struct _tunion_struct*) _temp1033)->tag == Cyc_Absyn_Tuple_e_tag){
_LL1188: _temp1187=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1033)->f1; goto _LL1082;} else{ goto _LL1083;} _LL1083: if((( struct
_tunion_struct*) _temp1033)->tag == Cyc_Absyn_CompoundLit_e_tag){ _LL1192:
_temp1191=( struct _tuple2*)(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp1033)->f1;
goto _LL1190; _LL1190: _temp1189=( struct Cyc_List_List*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1033)->f2; goto _LL1084;} else{ goto _LL1085;} _LL1085: if((( struct
_tunion_struct*) _temp1033)->tag == Cyc_Absyn_Array_e_tag){ _LL1194: _temp1193=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1033)->f1; goto
_LL1086;} else{ goto _LL1087;} _LL1087: if((( struct _tunion_struct*) _temp1033)->tag
== Cyc_Absyn_Comprehension_e_tag){ _LL1200: _temp1199=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1033)->f1; goto _LL1198; _LL1198:
_temp1197=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1033)->f2; goto _LL1196; _LL1196: _temp1195=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1033)->f3; goto _LL1088;} else{
goto _LL1089;} _LL1089: if((( struct _tunion_struct*) _temp1033)->tag == Cyc_Absyn_Struct_e_tag){
_LL1208: _temp1207=( struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1033)->f1; goto _LL1206; _LL1206: _temp1205=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1033)->f2; goto _LL1204; _LL1204:
_temp1203=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1033)->f3; goto _LL1202; _LL1202: _temp1201=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1033)->f4; goto _LL1090;} else{ goto
_LL1091;} _LL1091: if((( struct _tunion_struct*) _temp1033)->tag == Cyc_Absyn_Tunion_e_tag){
_LL1218: _temp1217=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1033)->f1; goto _LL1216; _LL1216: _temp1215=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1033)->f2; goto _LL1214; _LL1214:
_temp1213=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1033)->f3; if( _temp1213 == 0){ goto _LL1212;} else{ goto _LL1093;} _LL1212:
_temp1211=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1033)->f4; goto _LL1210; _LL1210: _temp1209=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1033)->f5; goto _LL1092;} else{ goto
_LL1093;} _LL1093: if((( struct _tunion_struct*) _temp1033)->tag == Cyc_Absyn_Tunion_e_tag){
_LL1228: _temp1227=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1033)->f1; goto _LL1226; _LL1226: _temp1225=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1033)->f2; goto _LL1224; _LL1224:
_temp1223=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1033)->f3; goto _LL1222; _LL1222: _temp1221=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1033)->f4; goto _LL1220; _LL1220:
_temp1219=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1033)->f5; goto _LL1094;} else{ goto _LL1095;} _LL1095: if((( struct
_tunion_struct*) _temp1033)->tag == Cyc_Absyn_XTunion_e_tag){ _LL1236: _temp1235=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_XTunion_e_struct*) _temp1033)->f1; goto
_LL1234; _LL1234: _temp1233=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_e_struct*)
_temp1033)->f2; goto _LL1232; _LL1232: _temp1231=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_e_struct*) _temp1033)->f3; goto _LL1230; _LL1230:
_temp1229=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_e_struct*)
_temp1033)->f4; goto _LL1096;} else{ goto _LL1097;} _LL1097: if((( struct
_tunion_struct*) _temp1033)->tag == Cyc_Absyn_Enum_e_tag){ _LL1242: _temp1241=(
struct _tuple1*)(( struct Cyc_Absyn_Enum_e_struct*) _temp1033)->f1; goto _LL1240;
_LL1240: _temp1239=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp1033)->f2; goto _LL1238; _LL1238: _temp1237=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_e_struct*) _temp1033)->f3; goto _LL1098;} else{ goto
_LL1099;} _LL1099: if((( struct _tunion_struct*) _temp1033)->tag == Cyc_Absyn_Malloc_e_tag){
_LL1244: _temp1243=( void*)(( struct Cyc_Absyn_Malloc_e_struct*) _temp1033)->f1;
goto _LL1100;} else{ goto _LL1101;} _LL1101: if((( struct _tunion_struct*)
_temp1033)->tag == Cyc_Absyn_StmtExp_e_tag){ _LL1246: _temp1245=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_StmtExp_e_struct*) _temp1033)->f1; goto _LL1102;} else{ goto
_LL1103;} _LL1103: if((( struct _tunion_struct*) _temp1033)->tag == Cyc_Absyn_UnresolvedMem_e_tag){
_LL1250: _temp1249=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1033)->f1; goto _LL1248; _LL1248: _temp1247=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp1033)->f2; goto _LL1104;} else{
goto _LL1105;} _LL1105: if((( struct _tunion_struct*) _temp1033)->tag == Cyc_Absyn_Codegen_e_tag){
_LL1252: _temp1251=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1033)->f1; goto _LL1106;} else{ goto _LL1107;} _LL1107: if((( struct
_tunion_struct*) _temp1033)->tag == Cyc_Absyn_Fill_e_tag){ _LL1254: _temp1253=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*) _temp1033)->f1; goto
_LL1108;} else{ goto _LL1034;} _LL1036:{ void* _temp1255= Cyc_Tcutil_compress(
old_typ); struct Cyc_Absyn_PtrInfo _temp1261; struct Cyc_Absyn_Conref* _temp1263;
struct Cyc_Absyn_Tqual* _temp1265; struct Cyc_Absyn_Conref* _temp1267; void*
_temp1269; void* _temp1271; _LL1257: if(( unsigned int) _temp1255 > 4u?(( struct
_tunion_struct*) _temp1255)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1262:
_temp1261=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1255)->f1; _LL1272: _temp1271=( void*) _temp1261.elt_typ; goto _LL1270;
_LL1270: _temp1269=( void*) _temp1261.rgn_typ; goto _LL1268; _LL1268: _temp1267=(
struct Cyc_Absyn_Conref*) _temp1261.nullable; goto _LL1266; _LL1266: _temp1265=(
struct Cyc_Absyn_Tqual*) _temp1261.tq; goto _LL1264; _LL1264: _temp1263=( struct
Cyc_Absyn_Conref*) _temp1261.bounds; goto _LL1258;} else{ goto _LL1259;} _LL1259:
goto _LL1260; _LL1258: if(( void*) _temp1263->v ==( void*) Cyc_Absyn_No_constr){(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp( 0, 0))->r)); goto
_LL1256;}{ void* _temp1273=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1263); struct Cyc_Absyn_Exp* _temp1279; _LL1275: if(( int) _temp1273 == Cyc_Absyn_Unknown_b){
goto _LL1276;} else{ goto _LL1277;} _LL1277: if(( unsigned int) _temp1273 > 1u?((
struct _tunion_struct*) _temp1273)->tag == Cyc_Absyn_Upper_b_tag: 0){ _LL1280:
_temp1279=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp1273)->f1;
goto _LL1278;} else{ goto _LL1274;} _LL1276: { struct _tuple7* zero=({ struct
_tuple7* _temp1281=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7));
_temp1281->f1= 0; _temp1281->f2= Cyc_Absyn_signed_int_exp( 0, 0); _temp1281;});
struct Cyc_Absyn_Exp* um= Cyc_Absyn_unresolvedmem_exp( 0,({ struct Cyc_List_List*
_temp1282=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1282->hd=( void*) zero; _temp1282->tl=({ struct Cyc_List_List* _temp1283=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1283->hd=(
void*) zero; _temp1283->tl=({ struct Cyc_List_List* _temp1284=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1284->hd=( void*) zero;
_temp1284->tl= 0; _temp1284;}); _temp1283;}); _temp1282;}), 0); if( nv->toplevel){(
void*)( e->r=( void*)(( void*) um->r));} else{ void* t= Cyc_Absyn_strct( Cyc_Toc_add_tagged_ptr_type(
Cyc_Toc_typ_to_c( _temp1271), Cyc_Toc_mt_tq)); struct _tuple1* x= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp( x, 0); struct Cyc_Absyn_Stmt* s=
Cyc_Absyn_declare_stmt( x, t,( struct Cyc_Absyn_Exp*) um, Cyc_Absyn_exp_stmt(
xexp, 0), 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r));}
goto _LL1274;} _LL1278:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp(
0, 0))->r)); goto _LL1274; _LL1274:;} goto _LL1256; _LL1260:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1285= Cyc_Absynpp_typ2string(
old_typ); xprintf("exp_to_c: null given type %.*s", _temp1285.last_plus_one -
_temp1285.curr, _temp1285.curr);})); goto _LL1256; _LL1256:;} goto _LL1034;
_LL1038: goto _LL1034; _LL1040:{ struct _handler_cons _temp1286; _push_handler(&
_temp1286);{ struct _xtunion_struct* _temp1287=( struct _xtunion_struct*) setjmp(
_temp1286.handler); if( ! _temp1287){( void*)( e->r=( void*)(( void*)((( struct
Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict* d, struct _tuple1* key)) Cyc_Dict_lookup)(
nv->varmap, _temp1115))->r));; _pop_handler();} else{ struct _xtunion_struct*
_temp1289= _temp1287; _LL1291: if( _temp1289->tag == Cyc_Dict_Absent_tag){ goto
_LL1292;} else{ goto _LL1293;} _LL1293: goto _LL1294; _LL1292:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1295= Cyc_Absynpp_qvar2string(
_temp1115); xprintf("Can't find %.*s in exp_to_c, Var\n", _temp1295.last_plus_one
- _temp1295.curr, _temp1295.curr);})); return; _LL1294:( void) _throw( _temp1289);
_LL1290:;}}} goto _LL1034; _LL1042:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1296=( char*)"unknownid"; struct
_tagged_string _temp1297; _temp1297.curr= _temp1296; _temp1297.base= _temp1296;
_temp1297.last_plus_one= _temp1296 + 10; _temp1297;})); goto _LL1034; _LL1044: {
struct Cyc_List_List* old_typs=(( struct Cyc_List_List*(*)( void*(* f)( struct
Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_cyc_typ,
_temp1119); struct Cyc_List_List* typs=(( struct Cyc_List_List*(*)( void*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_c_typ,
_temp1119);(( void(*)( void(* f)( struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*),
struct Cyc_Toc_Env* env, struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c,
nv, _temp1119);{ void* _temp1298= _temp1121; _LL1300: if(( int) _temp1298 == Cyc_Absyn_Size){
goto _LL1301;} else{ goto _LL1302;} _LL1302: if(( int) _temp1298 == Cyc_Absyn_Printf){
goto _LL1303;} else{ goto _LL1304;} _LL1304: if(( int) _temp1298 == Cyc_Absyn_Xprintf){
goto _LL1305;} else{ goto _LL1306;} _LL1306: if(( int) _temp1298 == Cyc_Absyn_Fprintf){
goto _LL1307;} else{ goto _LL1308;} _LL1308: if(( int) _temp1298 == Cyc_Absyn_Sscanf){
goto _LL1309;} else{ goto _LL1310;} _LL1310: if(( int) _temp1298 == Cyc_Absyn_Plus){
goto _LL1311;} else{ goto _LL1312;} _LL1312: if(( int) _temp1298 == Cyc_Absyn_Minus){
goto _LL1313;} else{ goto _LL1314;} _LL1314: if(( int) _temp1298 == Cyc_Absyn_Eq){
goto _LL1315;} else{ goto _LL1316;} _LL1316: if(( int) _temp1298 == Cyc_Absyn_Neq){
goto _LL1317;} else{ goto _LL1318;} _LL1318: if(( int) _temp1298 == Cyc_Absyn_Gt){
goto _LL1319;} else{ goto _LL1320;} _LL1320: if(( int) _temp1298 == Cyc_Absyn_Gte){
goto _LL1321;} else{ goto _LL1322;} _LL1322: if(( int) _temp1298 == Cyc_Absyn_Lt){
goto _LL1323;} else{ goto _LL1324;} _LL1324: if(( int) _temp1298 == Cyc_Absyn_Lte){
goto _LL1325;} else{ goto _LL1326;} _LL1326: goto _LL1327; _LL1301: { struct Cyc_Absyn_Exp*
arg=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1328= _temp1119; if(
_temp1328 == 0){ _throw( Null_Exception);} _temp1328->hd;});{ void* _temp1330=(
void*)({ struct Cyc_Core_Opt* _temp1329= arg->topt; if( _temp1329 == 0){ _throw(
Null_Exception);} _temp1329->v;}); struct Cyc_Absyn_Exp* _temp1338; struct Cyc_Absyn_Tqual*
_temp1340; void* _temp1342; struct Cyc_Absyn_PtrInfo _temp1344; struct Cyc_Absyn_Conref*
_temp1346; struct Cyc_Absyn_Tqual* _temp1348; struct Cyc_Absyn_Conref* _temp1350;
void* _temp1352; void* _temp1354; _LL1332: if(( unsigned int) _temp1330 > 4u?((
struct _tunion_struct*) _temp1330)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL1343:
_temp1342=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1330)->f1; goto
_LL1341; _LL1341: _temp1340=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1330)->f2; goto _LL1339; _LL1339: _temp1338=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1330)->f3; goto _LL1333;} else{ goto
_LL1334;} _LL1334: if(( unsigned int) _temp1330 > 4u?(( struct _tunion_struct*)
_temp1330)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1345: _temp1344=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp1330)->f1;
_LL1355: _temp1354=( void*) _temp1344.elt_typ; goto _LL1353; _LL1353: _temp1352=(
void*) _temp1344.rgn_typ; goto _LL1351; _LL1351: _temp1350=( struct Cyc_Absyn_Conref*)
_temp1344.nullable; goto _LL1349; _LL1349: _temp1348=( struct Cyc_Absyn_Tqual*)
_temp1344.tq; goto _LL1347; _LL1347: _temp1346=( struct Cyc_Absyn_Conref*)
_temp1344.bounds; goto _LL1335;} else{ goto _LL1336;} _LL1336: goto _LL1337;
_LL1333:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Exp* _temp1356=
_temp1338; if( _temp1356 == 0){ _throw( Null_Exception);} _temp1356->r;})));
goto _LL1331; _LL1335: { struct _tuple1* temp= Cyc_Toc_temp_var(); void*
temp_typ= Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Core_Opt* _temp1357= arg->topt;
if( _temp1357 == 0){ _throw( Null_Exception);} _temp1357->v;})); struct Cyc_Absyn_Exp*
e2= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_last_plus_one_sp,
0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
temp, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Exp* e4= Cyc_Absyn_cast_exp( Cyc_Absyn_uint_t,
Cyc_Absyn_subtract_exp( e2, e3, 0), 0); struct Cyc_Absyn_Stmt* result= Cyc_Absyn_exp_stmt(
e4, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
temp, temp_typ,( struct Cyc_Absyn_Exp*) arg, result, 0), 0))->r)); goto _LL1331;}
_LL1337:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct
_tagged_string _temp1360= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_List_List*
_temp1358= typs; if( _temp1358 == 0){ _throw( Null_Exception);} _temp1358->hd;}));
struct _tagged_string _temp1361= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp1359= arg->topt; if( _temp1359 == 0){ _throw( Null_Exception);} _temp1359->v;}));
xprintf("size primop applied to non-array %.*s (%.*s)", _temp1360.last_plus_one
- _temp1360.curr, _temp1360.curr, _temp1361.last_plus_one - _temp1361.curr,
_temp1361.curr);})); return; _LL1331:;} goto _LL1299;} _LL1303:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_printf_to_c( _temp1121, 0, _temp1119, typs))->r)); goto
_LL1299; _LL1305:( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp1121, 0, _temp1119, typs))->r)); goto _LL1299; _LL1307: if( _temp1119 == 0){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
char* _temp1362=( char*)"fprintf without arguments"; struct _tagged_string
_temp1363; _temp1363.curr= _temp1362; _temp1363.base= _temp1362; _temp1363.last_plus_one=
_temp1362 + 26; _temp1363;}));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp1121,({ struct Cyc_Core_Opt* _temp1364=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1364->v=( void*)(( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1365= _temp1119; if( _temp1365 == 0){ _throw(
Null_Exception);} _temp1365->hd;})); _temp1364;}),({ struct Cyc_List_List*
_temp1366= _temp1119; if( _temp1366 == 0){ _throw( Null_Exception);} _temp1366->tl;}),({
struct Cyc_List_List* _temp1367= typs; if( _temp1367 == 0){ _throw(
Null_Exception);} _temp1367->tl;})))->r)); goto _LL1299; _LL1309: if( _temp1119
== 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp1368=( char*)"sscanf without arguments"; struct
_tagged_string _temp1369; _temp1369.curr= _temp1368; _temp1369.base= _temp1368;
_temp1369.last_plus_one= _temp1368 + 25; _temp1369;}));}( void*)((( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1370= _temp1119; if( _temp1370 == 0){ _throw(
Null_Exception);} _temp1370->hd;}))->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*)(( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1371= _temp1119; if( _temp1371 == 0){ _throw( Null_Exception);} _temp1371->hd;}))->r,((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1372= _temp1119; if(
_temp1372 == 0){ _throw( Null_Exception);} _temp1372->hd;}))->loc), Cyc_Toc_curr_sp,
0))->r)); goto _LL1299; _LL1311: if( Cyc_Tcutil_is_tagged_pointer_typ( old_typ)){
struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1373= _temp1119; if( _temp1373 == 0){ _throw( Null_Exception);} _temp1373->hd;});
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1375=({ struct Cyc_List_List* _temp1374= _temp1119; if( _temp1374 == 0){
_throw( Null_Exception);} _temp1374->tl;}); if( _temp1375 == 0){ _throw(
Null_Exception);} _temp1375->hd;}); struct _tuple1* temp= Cyc_Toc_temp_var();
void* temp_typ= Cyc_Toc_typ_to_c( old_typ); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp( temp, 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Exp* e4= Cyc_Absyn_assignop_exp(
e3,({ struct Cyc_Core_Opt* _temp1376=( struct Cyc_Core_Opt*) GC_malloc( sizeof(
struct Cyc_Core_Opt)); _temp1376->v=( void*)(( void*) Cyc_Absyn_Plus); _temp1376;}),
e2, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( e4, 0), s, 0); s= Cyc_Absyn_declare_stmt(
temp, temp_typ,( struct Cyc_Absyn_Exp*) e1, s, 0);( void*)( e->r=( void*)(( void*)(
Cyc_Absyn_stmt_exp( s, 0))->r));} goto _LL1299; _LL1313: if( Cyc_Tcutil_is_tagged_pointer_typ((
void*)({ struct Cyc_List_List* _temp1377= old_typs; if( _temp1377 == 0){ _throw(
Null_Exception);} _temp1377->hd;}))){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1378= _temp1119; if( _temp1378 == 0){ _throw(
Null_Exception);} _temp1378->hd;}); struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1380=({ struct Cyc_List_List* _temp1379= _temp1119;
if( _temp1379 == 0){ _throw( Null_Exception);} _temp1379->tl;}); if( _temp1380
== 0){ _throw( Null_Exception);} _temp1380->hd;}); void* old_typ_e2=( void*)({
struct Cyc_List_List* _temp1382=({ struct Cyc_List_List* _temp1381= old_typs;
if( _temp1381 == 0){ _throw( Null_Exception);} _temp1381->tl;}); if( _temp1382
== 0){ _throw( Null_Exception);} _temp1382->hd;}); if( Cyc_Tcutil_is_tagged_pointer_typ(
old_typ_e2)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));( void*)( e2->r=( void*)(( void*)(
Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) e2->r, 0), Cyc_Toc_curr_sp,
0))->r));} else{ struct _tuple1* temp= Cyc_Toc_temp_var(); void* temp_typ= Cyc_Toc_typ_to_c((
void*)({ struct Cyc_List_List* _temp1383= old_typs; if( _temp1383 == 0){ _throw(
Null_Exception);} _temp1383->hd;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp( temp, 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Exp* e4= Cyc_Absyn_assignop_exp(
e3,({ struct Cyc_Core_Opt* _temp1384=( struct Cyc_Core_Opt*) GC_malloc( sizeof(
struct Cyc_Core_Opt)); _temp1384->v=( void*)(( void*) Cyc_Absyn_Minus);
_temp1384;}), e2, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( e4, 0), s, 0);
s= Cyc_Absyn_declare_stmt( temp, temp_typ,( struct Cyc_Absyn_Exp*) e1, s, 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r));}} goto _LL1299;
_LL1315: goto _LL1317; _LL1317: goto _LL1319; _LL1319: goto _LL1321; _LL1321:
goto _LL1323; _LL1323: goto _LL1325; _LL1325: { struct Cyc_Absyn_Exp* e1=(
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1385= _temp1119; if(
_temp1385 == 0){ _throw( Null_Exception);} _temp1385->hd;}); struct Cyc_Absyn_Exp*
e2=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1387=({ struct Cyc_List_List*
_temp1386= _temp1119; if( _temp1386 == 0){ _throw( Null_Exception);} _temp1386->tl;});
if( _temp1387 == 0){ _throw( Null_Exception);} _temp1387->hd;}); void* t1=( void*)({
struct Cyc_List_List* _temp1388= old_typs; if( _temp1388 == 0){ _throw(
Null_Exception);} _temp1388->hd;}); void* t2=( void*)({ struct Cyc_List_List*
_temp1390=({ struct Cyc_List_List* _temp1389= old_typs; if( _temp1389 == 0){
_throw( Null_Exception);} _temp1389->tl;}); if( _temp1390 == 0){ _throw(
Null_Exception);} _temp1390->hd;}); if( Cyc_Tcutil_is_tagged_pointer_typ( t1)){(
void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));} goto _LL1299;} _LL1327: goto
_LL1299; _LL1299:;} goto _LL1034;} _LL1046: { void* e2_cyc_typ=( void*)({ struct
Cyc_Core_Opt* _temp1391= _temp1125->topt; if( _temp1391 == 0){ _throw(
Null_Exception);} _temp1391->v;}); Cyc_Toc_exp_to_c( nv, _temp1125); if( Cyc_Tcutil_is_tagged_pointer_typ(
e2_cyc_typ)){ if( _temp1123 ==( void*) Cyc_Absyn_PreInc? 1: _temp1123 ==( void*)
Cyc_Absyn_PreDec){ struct _tuple1* temp= Cyc_Toc_temp_var(); void* temp_typ= Cyc_Absyn_star_typ(
Cyc_Toc_typ_to_c( e2_cyc_typ),( void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual());
struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp(
temp, 0), 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_increment_exp( Cyc_Absyn_structarrow_exp(
Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0), _temp1123, 0); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( e3, 0), s, 0); s= Cyc_Absyn_declare_stmt( temp, temp_typ,(
struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( _temp1125, 0), s, 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r));} else{ struct _tuple1* temp=
Cyc_Toc_temp_var(); struct _tuple1* temp2= Cyc_Toc_temp_var(); void* temp2_typ=
Cyc_Toc_typ_to_c( e2_cyc_typ); void* temp_typ= Cyc_Absyn_star_typ( Cyc_Toc_typ_to_c(
e2_cyc_typ),( void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual()); struct Cyc_Absyn_Stmt*
s= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( temp2, 0), 0); struct Cyc_Absyn_Exp*
e3= Cyc_Absyn_increment_exp( Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( temp,
0), Cyc_Toc_curr_sp, 0), _temp1123, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
e3, 0), s, 0); s= Cyc_Absyn_declare_stmt( temp2, temp2_typ,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( temp, 0), 0), s, 0); s= Cyc_Absyn_declare_stmt(
temp, temp_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( _temp1125, 0), s,
0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r));}} else{
if( ! Cyc_Absyn_is_lvalue( _temp1125)){(( void(*)( struct Cyc_Absyn_Exp* e1,
struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1125, 0, Cyc_Toc_incr_lvalue,
_temp1123);( void*)( e->r=( void*)(( void*) _temp1125->r));}} goto _LL1034;}
_LL1048: { int e1_poly= Cyc_Toc_is_poly_project( _temp1131); void* e1_old_typ=(
void*)({ struct Cyc_Core_Opt* _temp1392= _temp1131->topt; if( _temp1392 == 0){
_throw( Null_Exception);} _temp1392->v;}); void* e2_old_typ=( void*)({ struct
Cyc_Core_Opt* _temp1393= _temp1127->topt; if( _temp1393 == 0){ _throw(
Null_Exception);} _temp1393->v;}); Cyc_Toc_exp_to_c( nv, _temp1131); Cyc_Toc_exp_to_c(
nv, _temp1127); if( _temp1129 != 0?( void*)({ struct Cyc_Core_Opt* _temp1394=
_temp1129; if( _temp1394 == 0){ _throw( Null_Exception);} _temp1394->v;}) ==(
void*) Cyc_Absyn_Plus? 1:( void*)({ struct Cyc_Core_Opt* _temp1395= _temp1129;
if( _temp1395 == 0){ _throw( Null_Exception);} _temp1395->v;}) ==( void*) Cyc_Absyn_Minus:
0){ if( Cyc_Tcutil_is_tagged_pointer_typ( e1_old_typ)){( void*)( _temp1131->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*)
_temp1131->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
e2_old_typ)){( void*)( _temp1127->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*) _temp1127->r, 0), Cyc_Toc_curr_sp, 0))->r));}} if(
e1_poly){( void*)( _temp1127->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1127->r, 0), 0))->r));} if( ! Cyc_Absyn_is_lvalue(
_temp1131)){(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, struct _tuple11*), struct
_tuple11* f_env)) Cyc_Toc_lvalue_assign)( _temp1131, 0, Cyc_Toc_assignop_lvalue,({
struct _tuple11* _temp1396=( struct _tuple11*) GC_malloc( sizeof( struct
_tuple11) * 1); _temp1396[ 0]=({ struct _tuple11 _temp1397; _temp1397.f1=
_temp1129; _temp1397.f2= _temp1127; _temp1397;}); _temp1396;}));( void*)( e->r=(
void*)(( void*) _temp1131->r));} goto _LL1034;} _LL1050: Cyc_Toc_exp_to_c( nv,
_temp1137); Cyc_Toc_exp_to_c( nv, _temp1135); Cyc_Toc_exp_to_c( nv, _temp1133);
goto _LL1034; _LL1052: Cyc_Toc_exp_to_c( nv, _temp1141); Cyc_Toc_exp_to_c( nv,
_temp1139); goto _LL1034; _LL1054: _temp1149= _temp1145; _temp1147= _temp1143;
goto _LL1056; _LL1056: Cyc_Toc_exp_to_c( nv, _temp1149);(( void(*)( void(* f)(
struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List*
x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp1147); goto _LL1034; _LL1058:
Cyc_Toc_exp_to_c( nv, _temp1151);( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c( old_typ), Cyc_Toc_newthrow_exp( _temp1151), 0))->r)); goto
_LL1034; _LL1060: Cyc_Toc_exp_to_c( nv, _temp1153); goto _LL1034; _LL1062: Cyc_Toc_exp_to_c(
nv, _temp1157);{ void* t_inst= Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Core_Opt*
_temp1398= e->topt; if( _temp1398 == 0){ _throw( Null_Exception);} _temp1398->v;}));
for( 0; _temp1155 != 0; _temp1155=({ struct Cyc_List_List* _temp1399= _temp1155;
if( _temp1399 == 0){ _throw( Null_Exception);} _temp1399->tl;})){ void* k= Cyc_Tcutil_typ_kind((
void*)({ struct Cyc_List_List* _temp1400= _temp1155; if( _temp1400 == 0){ _throw(
Null_Exception);} _temp1400->hd;})); if( k !=( void*) Cyc_Absyn_EffKind? k !=(
void*) Cyc_Absyn_RgnKind: 0){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
t_inst, _temp1157, 0))->r)); break;}} goto _LL1034;} _LL1064: { void* old_t2=(
void*)({ struct Cyc_Core_Opt* _temp1401=(* _temp1161)->topt; if( _temp1401 == 0){
_throw( Null_Exception);} _temp1401->v;}); void* new_typ=* _temp1164;* _temp1164=
Cyc_Toc_typ_to_c( new_typ); Cyc_Toc_exp_to_c( nv,* _temp1161);* _temp1161= Cyc_Toc_convert_exp(
nv->toplevel, old_t2, new_typ,* _temp1161); goto _LL1034;} _LL1066:{ void*
_temp1402=( void*) _temp1165->r; struct Cyc_Absyn_Structdecl* _temp1410; struct
Cyc_List_List* _temp1412; struct Cyc_Core_Opt* _temp1414; struct _tuple1*
_temp1416; struct Cyc_List_List* _temp1418; _LL1404: if((( struct _tunion_struct*)
_temp1402)->tag == Cyc_Absyn_Struct_e_tag){ _LL1417: _temp1416=( struct _tuple1*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1402)->f1; goto _LL1415; _LL1415:
_temp1414=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*) _temp1402)->f2;
goto _LL1413; _LL1413: _temp1412=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1402)->f3; goto _LL1411; _LL1411: _temp1410=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1402)->f4; goto _LL1405;} else{ goto
_LL1406;} _LL1406: if((( struct _tunion_struct*) _temp1402)->tag == Cyc_Absyn_Tuple_e_tag){
_LL1419: _temp1418=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1402)->f1; goto _LL1407;} else{ goto _LL1408;} _LL1408: goto _LL1409;
_LL1405: if( nv->toplevel){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({
struct _tagged_string _temp1420= Cyc_Position_string_of_segment( _temp1165->loc);
xprintf("%.*s: & on non-identifiers at the top-level", _temp1420.last_plus_one -
_temp1420.curr, _temp1420.curr);}));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_struct(
nv,( void*)({ struct Cyc_Core_Opt* _temp1421= _temp1165->topt; if( _temp1421 ==
0){ _throw( Null_Exception);} _temp1421->v;}), 1, _temp1412, _temp1416))->r));
goto _LL1403; _LL1407: if( nv->toplevel){(( void(*)( struct _tagged_string s))
Cyc_Toc_unimp)(({ struct _tagged_string _temp1422= Cyc_Position_string_of_segment(
_temp1165->loc); xprintf("%.*s: & on non-identifiers at the top-level",
_temp1422.last_plus_one - _temp1422.curr, _temp1422.curr);}));}( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_tuple( nv, 1, _temp1418))->r)); goto _LL1403;
_LL1409: Cyc_Toc_exp_to_c( nv, _temp1165); if( ! Cyc_Absyn_is_lvalue( _temp1165)){((
void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(*
f)( struct Cyc_Absyn_Exp*, int), int f_env)) Cyc_Toc_lvalue_assign)( _temp1165,
0, Cyc_Toc_address_lvalue, 1);( void*)( e->r=( void*)(( void*) _temp1165->r));}
goto _LL1403; _LL1403:;} goto _LL1034; _LL1068: if( nv->toplevel){(( void(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(({ struct _tagged_string _temp1423= Cyc_Position_string_of_segment(
_temp1167->loc); xprintf("%.*s: new at top-level", _temp1423.last_plus_one -
_temp1423.curr, _temp1423.curr);}));}{ void* _temp1424=( void*) _temp1167->r;
struct Cyc_List_List* _temp1438; void* _temp1440; struct _tagged_string
_temp1442; struct Cyc_Absyn_Exp* _temp1444; struct Cyc_Absyn_Exp* _temp1446;
struct Cyc_Absyn_Vardecl* _temp1448; struct Cyc_Absyn_Structdecl* _temp1450;
struct Cyc_List_List* _temp1452; struct Cyc_Core_Opt* _temp1454; struct _tuple1*
_temp1456; struct Cyc_List_List* _temp1458; _LL1426: if((( struct _tunion_struct*)
_temp1424)->tag == Cyc_Absyn_Array_e_tag){ _LL1439: _temp1438=( struct Cyc_List_List*)((
struct Cyc_Absyn_Array_e_struct*) _temp1424)->f1; goto _LL1427;} else{ goto
_LL1428;} _LL1428: if((( struct _tunion_struct*) _temp1424)->tag == Cyc_Absyn_Const_e_tag){
_LL1441: _temp1440=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1424)->f1;
if(( unsigned int) _temp1440 > 1u?(( struct _tunion_struct*) _temp1440)->tag ==
Cyc_Absyn_String_c_tag: 0){ _LL1443: _temp1442=( struct _tagged_string)(( struct
Cyc_Absyn_String_c_struct*) _temp1440)->f1; goto _LL1429;} else{ goto _LL1430;}}
else{ goto _LL1430;} _LL1430: if((( struct _tunion_struct*) _temp1424)->tag ==
Cyc_Absyn_Comprehension_e_tag){ _LL1449: _temp1448=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1424)->f1; goto _LL1447; _LL1447:
_temp1446=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1424)->f2; goto _LL1445; _LL1445: _temp1444=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1424)->f3; goto _LL1431;} else{
goto _LL1432;} _LL1432: if((( struct _tunion_struct*) _temp1424)->tag == Cyc_Absyn_Struct_e_tag){
_LL1457: _temp1456=( struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1424)->f1; goto _LL1455; _LL1455: _temp1454=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1424)->f2; goto _LL1453; _LL1453:
_temp1452=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1424)->f3; goto _LL1451; _LL1451: _temp1450=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1424)->f4; goto _LL1433;} else{ goto
_LL1434;} _LL1434: if((( struct _tunion_struct*) _temp1424)->tag == Cyc_Absyn_Tuple_e_tag){
_LL1459: _temp1458=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1424)->f1; goto _LL1435;} else{ goto _LL1436;} _LL1436: goto _LL1437;
_LL1427: { struct _tuple1* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp( x, 0), 0); int count=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp1438) - 1;{ struct Cyc_List_List* dles=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( _temp1438); for( 0; dles != 0; dles=({ struct
Cyc_List_List* _temp1460= dles; if( _temp1460 == 0){ _throw( Null_Exception);}
_temp1460->tl;})){ struct _tuple7 _temp1464; struct Cyc_Absyn_Exp* _temp1465;
struct Cyc_List_List* _temp1467; struct _tuple7* _temp1462=( struct _tuple7*)({
struct Cyc_List_List* _temp1461= dles; if( _temp1461 == 0){ _throw(
Null_Exception);} _temp1461->hd;}); _temp1464=* _temp1462; _LL1468: _temp1467=
_temp1464.f1; goto _LL1466; _LL1466: _temp1465= _temp1464.f2; goto _LL1463;
_LL1463: { struct Cyc_Absyn_Exp* e_index; if( _temp1467 == 0){ e_index= Cyc_Absyn_signed_int_exp((
count --), 0);} else{ if(({ struct Cyc_List_List* _temp1469= _temp1467; if(
_temp1469 == 0){ _throw( Null_Exception);} _temp1469->tl;}) != 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char*
_temp1470=( char*)"multiple designators in array"; struct _tagged_string
_temp1471; _temp1471.curr= _temp1470; _temp1471.base= _temp1470; _temp1471.last_plus_one=
_temp1470 + 30; _temp1471;}));}{ void* d=( void*)({ struct Cyc_List_List*
_temp1472= _temp1467; if( _temp1472 == 0){ _throw( Null_Exception);} _temp1472->hd;});
void* _temp1473= d; struct Cyc_Absyn_Exp* _temp1479; struct _tagged_string*
_temp1481; _LL1475: if((( struct _tunion_struct*) _temp1473)->tag == Cyc_Absyn_ArrayElement_tag){
_LL1480: _temp1479=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayElement_struct*)
_temp1473)->f1; goto _LL1476;} else{ goto _LL1477;} _LL1477: if((( struct
_tunion_struct*) _temp1473)->tag == Cyc_Absyn_FieldName_tag){ _LL1482: _temp1481=(
struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp1473)->f1;
goto _LL1478;} else{ goto _LL1474;} _LL1476: Cyc_Toc_exp_to_c( nv, _temp1479);
e_index= _temp1479; goto _LL1474; _LL1478: e_index=(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char*
_temp1483=( char*)"field name designators in array"; struct _tagged_string
_temp1484; _temp1484.curr= _temp1483; _temp1484.base= _temp1483; _temp1484.last_plus_one=
_temp1483 + 32; _temp1484;})); goto _LL1474; _LL1474:;}} Cyc_Toc_exp_to_c( nv,
_temp1465); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
Cyc_Absyn_var_exp( x, 0), e_index, 0), _temp1465, 0), s, 0);}}}{ void*
old_elt_typ;{ void* _temp1485= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1491; struct Cyc_Absyn_Conref* _temp1493; struct Cyc_Absyn_Tqual* _temp1495;
struct Cyc_Absyn_Conref* _temp1497; void* _temp1499; void* _temp1501; _LL1487:
if(( unsigned int) _temp1485 > 4u?(( struct _tunion_struct*) _temp1485)->tag ==
Cyc_Absyn_PointerType_tag: 0){ _LL1492: _temp1491=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1485)->f1; _LL1502: _temp1501=( void*)
_temp1491.elt_typ; goto _LL1500; _LL1500: _temp1499=( void*) _temp1491.rgn_typ;
goto _LL1498; _LL1498: _temp1497=( struct Cyc_Absyn_Conref*) _temp1491.nullable;
goto _LL1496; _LL1496: _temp1495=( struct Cyc_Absyn_Tqual*) _temp1491.tq; goto
_LL1494; _LL1494: _temp1493=( struct Cyc_Absyn_Conref*) _temp1491.bounds; goto
_LL1488;} else{ goto _LL1489;} _LL1489: goto _LL1490; _LL1488: old_elt_typ=
_temp1501; goto _LL1486; _LL1490: old_elt_typ=(( void*(*)( struct _tagged_string
s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1503=( char*)"exp_to_c:new array expression doesn't have ptr type";
struct _tagged_string _temp1504; _temp1504.curr= _temp1503; _temp1504.base=
_temp1503; _temp1504.last_plus_one= _temp1503 + 52; _temp1504;})); goto _LL1486;
_LL1486:;}{ void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp(
elt_typ, 0), Cyc_Absyn_signed_int_exp((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp1438), 0), 0); struct Cyc_Absyn_Exp* e1= Cyc_Toc_malloc_exp( old_elt_typ,
e2); struct Cyc_Absyn_Exp* e0= Cyc_Absyn_cast_exp( ptr_typ, e1, 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, ptr_typ,( struct
Cyc_Absyn_Exp*) e0, s, 0), 0))->r)); goto _LL1425;}}} _LL1429:( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(({ struct _tuple1*
_temp1505=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp1505->f1=
Cyc_Toc_abs_ns; _temp1505->f2= Cyc_Toc_Cyc_new_string_sp; _temp1505;}), 0),({
struct Cyc_List_List* _temp1506=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1506->hd=( void*) Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Const_e_struct* _temp1507=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp1507[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp1508; _temp1508.tag= Cyc_Absyn_Const_e_tag;
_temp1508.f1=( void*)(( void*)({ struct Cyc_Absyn_String_c_struct* _temp1509=(
struct Cyc_Absyn_String_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_String_c_struct));
_temp1509[ 0]=({ struct Cyc_Absyn_String_c_struct _temp1510; _temp1510.tag= Cyc_Absyn_String_c_tag;
_temp1510.f1= _temp1442; _temp1510;}); _temp1509;})); _temp1508;}); _temp1507;}),
0); _temp1506->tl= 0; _temp1506;}), 0))->r)); goto _LL1425; _LL1431: { struct
_tuple1* x1= _temp1448->name; int is_tagged_ptr= 0;{ void* _temp1511= Cyc_Tcutil_compress(
old_typ); struct Cyc_Absyn_PtrInfo _temp1517; struct Cyc_Absyn_Conref* _temp1519;
struct Cyc_Absyn_Tqual* _temp1521; struct Cyc_Absyn_Conref* _temp1523; void*
_temp1525; void* _temp1527; _LL1513: if(( unsigned int) _temp1511 > 4u?(( struct
_tunion_struct*) _temp1511)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1518:
_temp1517=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1511)->f1; _LL1528: _temp1527=( void*) _temp1517.elt_typ; goto _LL1526;
_LL1526: _temp1525=( void*) _temp1517.rgn_typ; goto _LL1524; _LL1524: _temp1523=(
struct Cyc_Absyn_Conref*) _temp1517.nullable; goto _LL1522; _LL1522: _temp1521=(
struct Cyc_Absyn_Tqual*) _temp1517.tq; goto _LL1520; _LL1520: _temp1519=( struct
Cyc_Absyn_Conref*) _temp1517.bounds; goto _LL1514;} else{ goto _LL1515;} _LL1515:
goto _LL1516; _LL1514:{ void* _temp1529=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp1519))->v; void*
_temp1535; _LL1531: if(( unsigned int) _temp1529 > 1u?(( struct _tunion_struct*)
_temp1529)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL1536: _temp1535=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp1529)->f1; if(( int) _temp1535 == Cyc_Absyn_Unknown_b){
goto _LL1532;} else{ goto _LL1533;}} else{ goto _LL1533;} _LL1533: goto _LL1534;
_LL1532: is_tagged_ptr= 1; goto _LL1530; _LL1534: goto _LL1530; _LL1530:;} goto
_LL1512; _LL1516:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1537=( char*)"exp_to_c: comprehension not an array type";
struct _tagged_string _temp1538; _temp1538.curr= _temp1537; _temp1538.base=
_temp1537; _temp1538.last_plus_one= _temp1537 + 42; _temp1538;})); goto _LL1512;
_LL1512:;}{ void* old_elt_typ=( void*)({ struct Cyc_Core_Opt* _temp1539=
_temp1444->topt; if( _temp1539 == 0){ _throw( Null_Exception);} _temp1539->v;});
void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq); Cyc_Toc_exp_to_c( nv, _temp1446);{ struct Cyc_Toc_Env*
nv2= Cyc_Toc_add_varmap( nv, x1, Cyc_Absyn_varb_exp( x1,( void*)({ struct Cyc_Absyn_Local_b_struct*
_temp1540=( struct Cyc_Absyn_Local_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp1540[ 0]=({ struct Cyc_Absyn_Local_b_struct _temp1541; _temp1541.tag= Cyc_Absyn_Local_b_tag;
_temp1541.f1= _temp1448; _temp1541;}); _temp1540;}), 0)); Cyc_Toc_exp_to_c( nv2,
_temp1444);{ struct _tuple1* max= Cyc_Toc_temp_var(); struct _tuple1* a= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* ea= Cyc_Absyn_assign_exp( Cyc_Absyn_var_exp( x1, 0), Cyc_Absyn_signed_int_exp(
0, 0), 0); struct Cyc_Absyn_Exp* eb= Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp( x1, 0),
Cyc_Absyn_var_exp( max, 0), 0); struct Cyc_Absyn_Exp* ec= Cyc_Absyn_post_inc_exp(
Cyc_Absyn_var_exp( x1, 0), 0); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt( ea,
eb, ec, Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp( Cyc_Absyn_var_exp( a, 0),
Cyc_Absyn_var_exp( x1, 0), 0), _temp1444, 0), 0); struct Cyc_List_List* decls=({
struct Cyc_List_List* _temp1542=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1542->hd=( void*)({ struct _tuple10* _temp1543=(
struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp1543->f1= max;
_temp1543->f2= Cyc_Absyn_uint_t; _temp1543->f3=( struct Cyc_Absyn_Exp*)
_temp1446; _temp1543;}); _temp1542->tl= 0; _temp1542;}); struct Cyc_Absyn_Exp*
ainit= Cyc_Absyn_cast_exp( ptr_typ, Cyc_Toc_malloc_exp( old_elt_typ, Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_var_exp( max, 0), 0)), 0); decls=({
struct Cyc_List_List* _temp1544=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1544->hd=( void*)({ struct _tuple10* _temp1545=(
struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp1545->f1= a;
_temp1545->f2= ptr_typ; _temp1545->f3=( struct Cyc_Absyn_Exp*) ainit; _temp1545;});
_temp1544->tl= decls; _temp1544;}); decls=({ struct Cyc_List_List* _temp1546=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1546->hd=(
void*)({ struct _tuple10* _temp1547=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp1547->f1= x1; _temp1547->f2= Cyc_Absyn_uint_t; _temp1547->f3=
0; _temp1547;}); _temp1546->tl= decls; _temp1546;}); if( is_tagged_ptr){ struct
_tuple1* b= Cyc_Toc_temp_var(); void* t= Cyc_Toc_typ_to_c( old_typ); struct Cyc_Absyn_Exp*
binit= Cyc_Absyn_unresolvedmem_exp( 0,({ struct Cyc_List_List* _temp1548=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1548->hd=(
void*)({ struct _tuple7* _temp1553=( struct _tuple7*) GC_malloc( sizeof( struct
_tuple7)); _temp1553->f1= 0; _temp1553->f2= Cyc_Absyn_var_exp( a, 0); _temp1553;});
_temp1548->tl=({ struct Cyc_List_List* _temp1549=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1549->hd=( void*)({ struct
_tuple7* _temp1552=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7));
_temp1552->f1= 0; _temp1552->f2= Cyc_Absyn_var_exp( a, 0); _temp1552;});
_temp1549->tl=({ struct Cyc_List_List* _temp1550=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1550->hd=( void*)({ struct
_tuple7* _temp1551=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7));
_temp1551->f1= 0; _temp1551->f2= Cyc_Absyn_add_exp( Cyc_Absyn_var_exp( a, 0),
Cyc_Absyn_var_exp( max, 0), 0); _temp1551;}); _temp1550->tl= 0; _temp1550;});
_temp1549;}); _temp1548;}), 0); decls=({ struct Cyc_List_List* _temp1554=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1554->hd=(
void*)({ struct _tuple10* _temp1555=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp1555->f1= b; _temp1555->f2= t; _temp1555->f3=( struct
Cyc_Absyn_Exp*) binit; _temp1555;}); _temp1554->tl= decls; _temp1554;}); s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( b, 0), 0), 0);} else{ s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( a, 0), 0), 0);}{ struct Cyc_List_List*
ds= decls; for( 0; ds != 0; ds=({ struct Cyc_List_List* _temp1556= ds; if(
_temp1556 == 0){ _throw( Null_Exception);} _temp1556->tl;})){ struct Cyc_Absyn_Exp*
_temp1560; void* _temp1562; struct _tuple1* _temp1564; struct _tuple10 _temp1558=*((
struct _tuple10*)({ struct Cyc_List_List* _temp1557= ds; if( _temp1557 == 0){
_throw( Null_Exception);} _temp1557->hd;})); _LL1565: _temp1564= _temp1558.f1;
goto _LL1563; _LL1563: _temp1562= _temp1558.f2; goto _LL1561; _LL1561: _temp1560=
_temp1558.f3; goto _LL1559; _LL1559: s= Cyc_Absyn_declare_stmt( _temp1564,
_temp1562, _temp1560, s, 0);}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
s, 0))->r)); goto _LL1425;}}}} _LL1433:( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_struct(
nv,( void*)({ struct Cyc_Core_Opt* _temp1566= _temp1167->topt; if( _temp1566 ==
0){ _throw( Null_Exception);} _temp1566->v;}), 1, _temp1452, _temp1456))->r));
goto _LL1425; _LL1435:( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple( nv,
1, _temp1458))->r)); goto _LL1425; _LL1437: { void* old_elt_typ=( void*)({
struct Cyc_Core_Opt* _temp1567= _temp1167->topt; if( _temp1567 == 0){ _throw(
Null_Exception);} _temp1567->v;}); struct _tuple1* x= Cyc_Toc_temp_var(); struct
Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( x, 0), 0); struct Cyc_Absyn_Exp*
e_index= Cyc_Absyn_signed_int_exp( 0, 0); Cyc_Toc_exp_to_c( nv, _temp1167); s=
Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp( Cyc_Absyn_var_exp(
x, 0), e_index, 0), _temp1167, 0), s, 0);{ void* elt_typ= Cyc_Toc_typ_to_c(
old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp* e2= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0); struct Cyc_Absyn_Exp*
e1= Cyc_Toc_malloc_exp( old_elt_typ, e2); struct Cyc_Absyn_Exp* e0= Cyc_Absyn_cast_exp(
ptr_typ, e1, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
x, ptr_typ,( struct Cyc_Absyn_Exp*) e0, s, 0), 0))->r)); goto _LL1425;}} _LL1425:;}
goto _LL1034; _LL1070:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Sizeoftyp_e_struct*
_temp1568=( struct Cyc_Absyn_Sizeoftyp_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Sizeoftyp_e_struct));
_temp1568[ 0]=({ struct Cyc_Absyn_Sizeoftyp_e_struct _temp1569; _temp1569.tag=
Cyc_Absyn_Sizeoftyp_e_tag; _temp1569.f1=( void*) Cyc_Toc_typ_to_c_array(
_temp1169); _temp1569;}); _temp1568;}))); goto _LL1034; _LL1072: Cyc_Toc_exp_to_c(
nv, _temp1171); goto _LL1034; _LL1074: { void* e1_typ= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp1570= _temp1173->topt; if( _temp1570 == 0){
_throw( Null_Exception);} _temp1570->v;})); Cyc_Toc_exp_to_c( nv, _temp1173);{
void* _temp1571= e1_typ; struct Cyc_Absyn_PtrInfo _temp1577; struct Cyc_Absyn_Conref*
_temp1579; struct Cyc_Absyn_Tqual* _temp1581; struct Cyc_Absyn_Conref* _temp1583;
void* _temp1585; void* _temp1587; _LL1573: if(( unsigned int) _temp1571 > 4u?((
struct _tunion_struct*) _temp1571)->tag == Cyc_Absyn_PointerType_tag: 0){
_LL1578: _temp1577=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1571)->f1; _LL1588: _temp1587=( void*) _temp1577.elt_typ; goto _LL1586;
_LL1586: _temp1585=( void*) _temp1577.rgn_typ; goto _LL1584; _LL1584: _temp1583=(
struct Cyc_Absyn_Conref*) _temp1577.nullable; goto _LL1582; _LL1582: _temp1581=(
struct Cyc_Absyn_Tqual*) _temp1577.tq; goto _LL1580; _LL1580: _temp1579=( struct
Cyc_Absyn_Conref*) _temp1577.bounds; goto _LL1574;} else{ goto _LL1575;} _LL1575:
goto _LL1576; _LL1574:{ void* _temp1589=(( void*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_val)( _temp1579); struct Cyc_Absyn_Exp* _temp1595; _LL1591: if((
int) _temp1589 == Cyc_Absyn_Unknown_b){ goto _LL1592;} else{ goto _LL1593;}
_LL1593: if(( unsigned int) _temp1589 > 1u?(( struct _tunion_struct*) _temp1589)->tag
== Cyc_Absyn_Upper_b_tag: 0){ _LL1596: _temp1595=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Upper_b_struct*) _temp1589)->f1; goto _LL1594;} else{ goto
_LL1590;} _LL1592: { struct _tuple1* x= Cyc_Toc_temp_var(); struct _tuple1* a=
Cyc_Toc_temp_var(); void* ta1= Cyc_Toc_typ_to_c_array( _temp1587); struct
_tagged_string* n= Cyc_Toc_add_tagged_ptr_type( ta1, _temp1581); void* tx= Cyc_Absyn_strct(
n); void* ta2= Cyc_Absyn_cstar_typ( ta1, _temp1581); struct Cyc_Absyn_Exp*
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
Cyc_Absyn_declare_stmt( x, tx,( struct Cyc_Absyn_Exp*) _temp1173, s1, 0), 0))->r));
goto _LL1590;} _LL1594: if( Cyc_Toc_is_nullable( e1_typ)){ struct _tuple1* x=
Cyc_Toc_temp_var(); void* tx= Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Core_Opt*
_temp1597= _temp1173->topt; if( _temp1597 == 0){ _throw( Null_Exception);}
_temp1597->v;})); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp(
Cyc_Absyn_null_pointer_exn_exp( 0)); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_ifthenelse_stmt(
e2, Cyc_Absyn_exp_stmt( e3, 0), Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( x, 0), 0), 0);
struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( s1, s2, 0);( void*)( e->r=( void*)((
void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, tx,( struct Cyc_Absyn_Exp*)
_temp1173, s3, 0), 0))->r));} goto _LL1590; _LL1590:;} goto _LL1572; _LL1576:((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
char* _temp1598=( char*)"exp_to_c: Deref: non-pointer"; struct _tagged_string
_temp1599; _temp1599.curr= _temp1598; _temp1599.base= _temp1598; _temp1599.last_plus_one=
_temp1598 + 29; _temp1599;})); goto _LL1572; _LL1572:;} goto _LL1034;} _LL1076:
Cyc_Toc_exp_to_c( nv, _temp1177); if( Cyc_Toc_is_poly_project( e)){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)({ struct
Cyc_Core_Opt* _temp1600= e->topt; if( _temp1600 == 0){ _throw( Null_Exception);}
_temp1600->v;})), Cyc_Absyn_new_exp(( void*) e->r, 0), 0))->r));} goto _LL1034;
_LL1078: Cyc_Toc_exp_to_c( nv, _temp1181);{ int is_poly= Cyc_Toc_is_poly_project(
e); if( Cyc_Toc_is_nullable(( void*)({ struct Cyc_Core_Opt* _temp1601= _temp1181->topt;
if( _temp1601 == 0){ _throw( Null_Exception);} _temp1601->v;}))){ struct _tuple1*
x= Cyc_Toc_temp_var(); void* tx= Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Core_Opt*
_temp1602= _temp1181->topt; if( _temp1602 == 0){ _throw( Null_Exception);}
_temp1602->v;})); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp(
Cyc_Absyn_null_pointer_exn_exp( 0)); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_ifthenelse_stmt(
e2, Cyc_Absyn_exp_stmt( e3, 0), Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_exp_stmt( Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( x, 0),
_temp1179, 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( s1, s2, 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x,
tx,( struct Cyc_Absyn_Exp*) _temp1181, s3, 0), 0))->r));} if( is_poly){( void*)(
e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)({
struct Cyc_Core_Opt* _temp1603= e->topt; if( _temp1603 == 0){ _throw(
Null_Exception);} _temp1603->v;})), Cyc_Absyn_new_exp(( void*) e->r, 0), 0))->r));}
goto _LL1034;} _LL1080: { void* e1_typ= Cyc_Tcutil_compress(( void*)({ struct
Cyc_Core_Opt* _temp1604= _temp1185->topt; if( _temp1604 == 0){ _throw(
Null_Exception);} _temp1604->v;})); Cyc_Toc_exp_to_c( nv, _temp1185); Cyc_Toc_exp_to_c(
nv, _temp1183);( void*)( _temp1185->r=( void*)(( void*)( Cyc_Toc_array_to_ptr(
Cyc_Toc_typ_to_c( e1_typ), Cyc_Absyn_new_exp(( void*) _temp1185->r, 0), 0))->r));{
void* _temp1605= e1_typ; struct Cyc_List_List* _temp1615; struct Cyc_Absyn_Exp*
_temp1617; struct Cyc_Absyn_Tqual* _temp1619; void* _temp1621; struct Cyc_Absyn_PtrInfo
_temp1623; struct Cyc_Absyn_Conref* _temp1625; struct Cyc_Absyn_Tqual* _temp1627;
struct Cyc_Absyn_Conref* _temp1629; void* _temp1631; void* _temp1633; _LL1607:
if(( unsigned int) _temp1605 > 4u?(( struct _tunion_struct*) _temp1605)->tag ==
Cyc_Absyn_TupleType_tag: 0){ _LL1616: _temp1615=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp1605)->f1; goto _LL1608;} else{ goto
_LL1609;} _LL1609: if(( unsigned int) _temp1605 > 4u?(( struct _tunion_struct*)
_temp1605)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL1622: _temp1621=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1605)->f1; goto _LL1620; _LL1620:
_temp1619=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1605)->f2; goto _LL1618; _LL1618: _temp1617=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1605)->f3; goto _LL1610;} else{ goto
_LL1611;} _LL1611: if(( unsigned int) _temp1605 > 4u?(( struct _tunion_struct*)
_temp1605)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1624: _temp1623=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp1605)->f1;
_LL1634: _temp1633=( void*) _temp1623.elt_typ; goto _LL1632; _LL1632: _temp1631=(
void*) _temp1623.rgn_typ; goto _LL1630; _LL1630: _temp1629=( struct Cyc_Absyn_Conref*)
_temp1623.nullable; goto _LL1628; _LL1628: _temp1627=( struct Cyc_Absyn_Tqual*)
_temp1623.tq; goto _LL1626; _LL1626: _temp1625=( struct Cyc_Absyn_Conref*)
_temp1623.bounds; goto _LL1612;} else{ goto _LL1613;} _LL1613: goto _LL1614;
_LL1608: { unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp1183) + 1;( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( _temp1185,({ struct
_tagged_string* _temp1635=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp1635[ 0]= xprintf("f%d",( int) i); _temp1635;}), 0))->r));
goto _LL1606;} _LL1610: if( _temp1617 == 0){(( void(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1636=( char*)"exp_to_c: Subscript_e array type bound null";
struct _tagged_string _temp1637; _temp1637.curr= _temp1636; _temp1637.base=
_temp1636; _temp1637.last_plus_one= _temp1636 + 44; _temp1637;}));}( void*)( e->r=(
void*)(( void*)( Cyc_Toc_known_size_subscript( _temp1185, _temp1183, 0,( struct
Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1638= _temp1617; if( _temp1638 == 0){
_throw( Null_Exception);} _temp1638;}), _temp1621, _temp1619, 0))->r)); goto
_LL1606; _LL1612:{ void* _temp1639=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1625); struct Cyc_Absyn_Exp* _temp1645; _LL1641: if(( unsigned int)
_temp1639 > 1u?(( struct _tunion_struct*) _temp1639)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL1646: _temp1645=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp1639)->f1; goto _LL1642;} else{ goto _LL1643;} _LL1643: if(( int) _temp1639
== Cyc_Absyn_Unknown_b){ goto _LL1644;} else{ goto _LL1640;} _LL1642: { int
possibly_null= 1;{ void* _temp1647=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp1629))->v; int
_temp1653; _LL1649: if(( unsigned int) _temp1647 > 1u?(( struct _tunion_struct*)
_temp1647)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL1654: _temp1653=( int)((
struct Cyc_Absyn_Eq_constr_struct*) _temp1647)->f1; goto _LL1650;} else{ goto
_LL1651;} _LL1651: goto _LL1652; _LL1650: possibly_null= _temp1653; goto _LL1648;
_LL1652: possibly_null= 0; goto _LL1648; _LL1648:;}( void*)( e->r=( void*)((
void*)( Cyc_Toc_known_size_subscript( _temp1185, _temp1183, possibly_null,
_temp1645, _temp1633, _temp1627, 0))->r)); goto _LL1640;} _LL1644: { struct
_tuple1* x= Cyc_Toc_temp_var(); struct _tuple1* i= Cyc_Toc_temp_var(); struct
_tuple1* a= Cyc_Toc_temp_var(); void* ta1= Cyc_Toc_typ_to_c_array( _temp1633);
struct _tagged_string* n= Cyc_Toc_add_tagged_ptr_type( ta1, _temp1627); void* tx=
Cyc_Absyn_strct( n); void* ta2= Cyc_Absyn_cstar_typ( ta1, _temp1627); struct Cyc_Absyn_Exp*
test_exp= Cyc_Absyn_or_exp( Cyc_Absyn_eq_exp( Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Toc_base_sp, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_or_exp(
Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Toc_base_sp, 0), 0), Cyc_Absyn_gte_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_last_plus_one_sp, 0), 0), 0), 0); struct Cyc_Absyn_Exp*
a_init= Cyc_Absyn_add_exp( Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0),
Cyc_Toc_curr_sp, 0), _temp1183, 0); struct Cyc_Absyn_Stmt* s5= Cyc_Absyn_exp_stmt(
Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( a, 0), 0), 0); struct Cyc_Absyn_Stmt* s4=
Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp( 0)), 0);
struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_ifthenelse_stmt( test_exp, s4, Cyc_Absyn_skip_stmt(
0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s3, s5, 0); struct Cyc_Absyn_Stmt*
s1= Cyc_Absyn_declare_stmt( a, ta2,( struct Cyc_Absyn_Exp*) a_init, s2, 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x,
tx,( struct Cyc_Absyn_Exp*) _temp1185, s1, 0), 0))->r)); goto _LL1640;} _LL1640:;}
goto _LL1606; _LL1614:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1655=( char*)"exp_to_c: Subscript on non-tuple/array/tuple ptr";
struct _tagged_string _temp1656; _temp1656.curr= _temp1655; _temp1656.base=
_temp1655; _temp1656.last_plus_one= _temp1655 + 49; _temp1656;})); goto _LL1606;
_LL1606:;} goto _LL1034;} _LL1082: if( ! nv->toplevel){( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_tuple( nv, 0, _temp1187))->r));} else{ struct Cyc_List_List*
tqs=(( struct Cyc_List_List*(*)( struct _tuple5*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_tup_to_c, _temp1187); struct
_tagged_string* n= Cyc_Toc_add_tuple_type( tqs); struct Cyc_List_List* dles= 0;{
int i= 1; for( 0; _temp1187 != 0; _temp1187=({ struct Cyc_List_List* _temp1657=
_temp1187; if( _temp1657 == 0){ _throw( Null_Exception);} _temp1657->tl;}), i ++){
Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1658=
_temp1187; if( _temp1658 == 0){ _throw( Null_Exception);} _temp1658->hd;}));{
struct Cyc_List_List* des=({ struct Cyc_List_List* _temp1659=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1659->hd=( void*)(( void*)({
struct Cyc_Absyn_FieldName_struct* _temp1660=( struct Cyc_Absyn_FieldName_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct)); _temp1660[ 0]=({ struct
Cyc_Absyn_FieldName_struct _temp1661; _temp1661.tag= Cyc_Absyn_FieldName_tag;
_temp1661.f1=({ struct _tagged_string* _temp1662=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp1662[ 0]= xprintf("f%d", i);
_temp1662;}); _temp1661;}); _temp1660;})); _temp1659->tl= 0; _temp1659;}); dles=({
struct Cyc_List_List* _temp1663=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1663->hd=( void*)({ struct _tuple7* _temp1664=(
struct _tuple7*) GC_malloc( sizeof( struct _tuple7)); _temp1664->f1=( struct Cyc_List_List*)
des; _temp1664->f2=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1665=
_temp1187; if( _temp1665 == 0){ _throw( Null_Exception);} _temp1665->hd;});
_temp1664;}); _temp1663->tl= dles; _temp1663;});}}} dles=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles);( void*)( e->r=( void*)((
void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_strct( n), Cyc_Absyn_unresolvedmem_exp( 0,
dles, 0), 0))->r));} goto _LL1034; _LL1084:(( void(*)( struct _tagged_string s))
Cyc_Toc_unimp)(( struct _tagged_string)({ char* _temp1666=( char*)"compoundlit";
struct _tagged_string _temp1667; _temp1667.curr= _temp1666; _temp1667.base=
_temp1666; _temp1667.last_plus_one= _temp1666 + 12; _temp1667;})); goto _LL1034;
_LL1086:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0,
_temp1193, 0))->r));{ struct Cyc_List_List* dles= _temp1193; for( 0; dles != 0;
dles=({ struct Cyc_List_List* _temp1668= dles; if( _temp1668 == 0){ _throw(
Null_Exception);} _temp1668->tl;})){ struct _tuple7 _temp1672; struct Cyc_Absyn_Exp*
_temp1673; struct Cyc_List_List* _temp1675; struct _tuple7* _temp1670=( struct
_tuple7*)({ struct Cyc_List_List* _temp1669= dles; if( _temp1669 == 0){ _throw(
Null_Exception);} _temp1669->hd;}); _temp1672=* _temp1670; _LL1676: _temp1675=
_temp1672.f1; goto _LL1674; _LL1674: _temp1673= _temp1672.f2; goto _LL1671;
_LL1671: Cyc_Toc_exp_to_c( nv, _temp1673);}} goto _LL1034; _LL1088: {
unsigned int sz= Cyc_Evexp_eval_const_uint_exp( _temp1197); Cyc_Toc_exp_to_c( nv,
_temp1195);{ struct Cyc_List_List* es= 0;{ unsigned int i= 0; for( 0; i < sz; i
++){ es=({ struct Cyc_List_List* _temp1677=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1677->hd=( void*)({ struct _tuple7*
_temp1678=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7)); _temp1678->f1=
0; _temp1678->f2= _temp1195; _temp1678;}); _temp1677->tl= es; _temp1677;});}}(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0, es, 0))->r));
goto _LL1034;}} _LL1090: if( ! nv->toplevel){( void*)( e->r=( void*)(( void*)(
Cyc_Toc_init_struct( nv, old_typ, 0, _temp1203, _temp1207))->r));} else{ struct
_tuple1* n= _temp1207; struct Cyc_Absyn_Structdecl* sd2=( struct Cyc_Absyn_Structdecl*)({
struct Cyc_Absyn_Structdecl* _temp1680= _temp1201 == 0?(( struct Cyc_Absyn_Structdecl*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1679=
Cyc_Absynpp_qvar2string( _temp1207); xprintf("no structdecl %.*s", _temp1679.last_plus_one
- _temp1679.curr, _temp1679.curr);})): _temp1201; if( _temp1680 == 0){ _throw(
Null_Exception);} _temp1680;}); struct Cyc_List_List* fields=(( struct Cyc_List_List*(*)(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* des, struct Cyc_Absyn_Structdecl*
sd)) Cyc_Tcutil_resolve_struct_designators)( e->loc, _temp1203,( struct Cyc_Absyn_Structdecl*)
sd2); for( 0; fields != 0; fields=({ struct Cyc_List_List* _temp1681= fields;
if( _temp1681 == 0){ _throw( Null_Exception);} _temp1681->tl;})){ struct
_tuple12 _temp1685; struct Cyc_Absyn_Exp* _temp1686; struct Cyc_Absyn_Structfield*
_temp1688; struct _tuple12* _temp1683=( struct _tuple12*)({ struct Cyc_List_List*
_temp1682= fields; if( _temp1682 == 0){ _throw( Null_Exception);} _temp1682->hd;});
_temp1685=* _temp1683; _LL1689: _temp1688= _temp1685.f1; goto _LL1687; _LL1687:
_temp1686= _temp1685.f2; goto _LL1684; _LL1684: { void* fieldtyp=( void*)
_temp1688->type; Cyc_Toc_exp_to_c( nv, _temp1686); if( Cyc_Toc_is_void_star(
fieldtyp)){( void*)( _temp1686->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1686->r, 0), 0))->r));}}}( void*)( e->r=( void*)((
void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_strctq( n), Cyc_Absyn_unresolvedmem_exp( 0,
_temp1203, 0), 0))->r));} goto _LL1034; _LL1092: { struct _tuple1* qv= _temp1209->name;
if( ! nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_var_exp( qv, 0))->r));}
else{ int tag_count= 0; struct Cyc_List_List* fields= _temp1211->fields == 0? 0:(
struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp1690= _temp1211->fields; if(
_temp1690 == 0){ _throw( Null_Exception);} _temp1690->v;}); while( Cyc_Absyn_qvar_cmp(
qv,(( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp1691= fields;
if( _temp1691 == 0){ _throw( Null_Exception);} _temp1691->hd;}))->name) != 0) {
if((( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp1692= fields;
if( _temp1692 == 0){ _throw( Null_Exception);} _temp1692->hd;}))->typs == 0){
tag_count ++;} fields=({ struct Cyc_List_List* _temp1693= fields; if( _temp1693
== 0){ _throw( Null_Exception);} _temp1693->tl;});}( void*)( e->r=( void*)((
void*)( Cyc_Absyn_uint_exp(( unsigned int) tag_count, 0))->r));} goto _LL1034;}
_LL1094: { struct _tuple1* qv= _temp1219->name; struct Cyc_List_List* tqts=
_temp1219->typs; struct _tuple1* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
xexp= Cyc_Absyn_var_exp( x, 0); void* strct_typ= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag(
qv,( struct _tagged_string)({ char* _temp1694=( char*)"_struct"; struct
_tagged_string _temp1695; _temp1695.curr= _temp1694; _temp1695.base= _temp1694;
_temp1695.last_plus_one= _temp1694 + 8; _temp1695;}))); if( nv->toplevel){ int
tag_count= 0; struct Cyc_List_List* fields= _temp1221->fields == 0? 0:( struct
Cyc_List_List*)({ struct Cyc_Core_Opt* _temp1696= _temp1221->fields; if(
_temp1696 == 0){ _throw( Null_Exception);} _temp1696->v;}); while( Cyc_Absyn_qvar_cmp(
qv,(( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp1697= fields;
if( _temp1697 == 0){ _throw( Null_Exception);} _temp1697->hd;}))->name) != 0) {
if((( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp1698= fields;
if( _temp1698 == 0){ _throw( Null_Exception);} _temp1698->hd;}))->typs != 0){
tag_count ++;} fields=({ struct Cyc_List_List* _temp1699= fields; if( _temp1699
== 0){ _throw( Null_Exception);} _temp1699->tl;});}{ struct Cyc_List_List* dles=
0; for( 0; _temp1223 != 0; _temp1223=({ struct Cyc_List_List* _temp1700=
_temp1223; if( _temp1700 == 0){ _throw( Null_Exception);} _temp1700->tl;}), tqts=({
struct Cyc_List_List* _temp1701= tqts; if( _temp1701 == 0){ _throw(
Null_Exception);} _temp1701->tl;})){ struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1702= _temp1223; if( _temp1702 == 0){ _throw(
Null_Exception);} _temp1702->hd;}); void* field_typ= Cyc_Toc_typ_to_c((*((
struct _tuple5*)({ struct Cyc_List_List* _temp1703= tqts; if( _temp1703 == 0){
_throw( Null_Exception);} _temp1703->hd;}))).f2); Cyc_Toc_exp_to_c( nv, cur_e);
if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp( field_typ,
cur_e, 0);} dles=({ struct Cyc_List_List* _temp1704=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp1704[ 0]=({ struct Cyc_List_List
_temp1705; _temp1705.hd=( void*)({ struct _tuple7* _temp1706=( struct _tuple7*)
GC_malloc( sizeof( struct _tuple7) * 1); _temp1706[ 0]=({ struct _tuple7
_temp1707; _temp1707.f1= 0; _temp1707.f2= cur_e; _temp1707;}); _temp1706;});
_temp1705.tl= dles; _temp1705;}); _temp1704;});}{ struct Cyc_Absyn_Exp* tag_exp=
Cyc_Absyn_uint_exp(( unsigned int) tag_count, 0); dles=({ struct Cyc_List_List*
_temp1708=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp1708[ 0]=({ struct Cyc_List_List _temp1709; _temp1709.hd=( void*)({ struct
_tuple7* _temp1710=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7) * 1);
_temp1710[ 0]=({ struct _tuple7 _temp1711; _temp1711.f1= 0; _temp1711.f2=
tag_exp; _temp1711;}); _temp1710;}); _temp1709.tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles); _temp1709;}); _temp1708;});(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(({ struct Cyc_Core_Opt*
_temp1712=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1712->v=( void*) Cyc_Toc_collapse_qvar_tag( qv,( struct _tagged_string)({
char* _temp1713=( char*)"_struct"; struct _tagged_string _temp1714; _temp1714.curr=
_temp1713; _temp1714.base= _temp1713; _temp1714.last_plus_one= _temp1713 + 8;
_temp1714;})); _temp1712;}), dles, 0))->r));}}} else{ struct Cyc_List_List*
rev_assign=({ struct Cyc_List_List* _temp1715=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1715->hd=( void*) Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( xexp, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp( Cyc_Toc_collapse_qvar_tag(
qv,( struct _tagged_string)({ char* _temp1716=( char*)"_tag"; struct
_tagged_string _temp1717; _temp1717.curr= _temp1716; _temp1717.base= _temp1716;
_temp1717.last_plus_one= _temp1716 + 5; _temp1717;})), 0), 0); _temp1715->tl= 0;
_temp1715;});{ int i= 1; for( 0; _temp1223 != 0;( _temp1223=({ struct Cyc_List_List*
_temp1718= _temp1223; if( _temp1718 == 0){ _throw( Null_Exception);} _temp1718->tl;}),
i ++), tqts=({ struct Cyc_List_List* _temp1719= tqts; if( _temp1719 == 0){
_throw( Null_Exception);} _temp1719->tl;})){ struct Cyc_Absyn_Exp* cur_e=(
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1720= _temp1223; if(
_temp1720 == 0){ _throw( Null_Exception);} _temp1720->hd;}); void* field_typ=
Cyc_Toc_typ_to_c((*(( struct _tuple5*)({ struct Cyc_List_List* _temp1721= tqts;
if( _temp1721 == 0){ _throw( Null_Exception);} _temp1721->hd;}))).f2); Cyc_Toc_exp_to_c(
nv, cur_e); if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp(
field_typ, cur_e, 0);}{ struct Cyc_Absyn_Stmt* a= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
xexp,({ struct _tagged_string* _temp1722=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp1722[ 0]= xprintf("f%d", i); _temp1722;}),
0), cur_e, 0); rev_assign=({ struct Cyc_List_List* _temp1723=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1723->hd=( void*) a; _temp1723->tl=
rev_assign; _temp1723;});}}}{ struct Cyc_Absyn_Stmt* final_stmt= Cyc_Absyn_exp_stmt(
xexp, 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmts((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)(({ struct Cyc_List_List* _temp1724=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1724->hd=(
void*) final_stmt; _temp1724->tl= rev_assign; _temp1724;})), 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, strct_typ, 0, s2,
0), 0))->r));}} goto _LL1034;} _LL1096: { struct _tuple1* qv= _temp1229->name;
struct Cyc_List_List* tqts= _temp1229->typs; struct Cyc_Absyn_Exp* tag_exp= Cyc_Absyn_var_exp(
Cyc_Toc_collapse_qvar_tag( qv,( struct _tagged_string)({ char* _temp1725=( char*)"_tag";
struct _tagged_string _temp1726; _temp1726.curr= _temp1725; _temp1726.base=
_temp1725; _temp1726.last_plus_one= _temp1725 + 5; _temp1726;})), 0); if( nv->toplevel){
struct Cyc_List_List* dles=(( struct Cyc_List_List*(*)( struct _tuple7*(* f)(
struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Toc_add_designator, nv, _temp1233); dles=({ struct Cyc_List_List*
_temp1727=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1727->hd=( void*)({ struct _tuple7* _temp1728=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp1728->f1= 0; _temp1728->f2= tag_exp; _temp1728;});
_temp1727->tl= dles; _temp1727;});( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(({
struct Cyc_Core_Opt* _temp1729=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1729->v=( void*) Cyc_Toc_collapse_qvar_tag( qv,( struct
_tagged_string)({ char* _temp1730=( char*)"_struct"; struct _tagged_string
_temp1731; _temp1731.curr= _temp1730; _temp1731.base= _temp1730; _temp1731.last_plus_one=
_temp1730 + 8; _temp1731;})); _temp1729;}), dles, 0))->r));} else{ struct
_tuple1* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp( x,
0); void* strct_typ= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( qv,( struct
_tagged_string)({ char* _temp1732=( char*)"_struct"; struct _tagged_string
_temp1733; _temp1733.curr= _temp1732; _temp1733.base= _temp1732; _temp1733.last_plus_one=
_temp1732 + 8; _temp1733;}))); struct Cyc_List_List* rev_assign=({ struct Cyc_List_List*
_temp1734=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1734->hd=( void*) Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( xexp,
Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp( Cyc_Toc_collapse_qvar_tag( qv,( struct
_tagged_string)({ char* _temp1735=( char*)"_tag"; struct _tagged_string
_temp1736; _temp1736.curr= _temp1735; _temp1736.base= _temp1735; _temp1736.last_plus_one=
_temp1735 + 5; _temp1736;})), 0), 0); _temp1734->tl= 0; _temp1734;});{ int i= 1;
for( 0; _temp1233 != 0;( _temp1233=({ struct Cyc_List_List* _temp1737= _temp1233;
if( _temp1737 == 0){ _throw( Null_Exception);} _temp1737->tl;}), i ++), tqts=({
struct Cyc_List_List* _temp1738= tqts; if( _temp1738 == 0){ _throw(
Null_Exception);} _temp1738->tl;})){ struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1739= _temp1233; if( _temp1739 == 0){ _throw(
Null_Exception);} _temp1739->hd;}); void* field_typ= Cyc_Toc_typ_to_c((*((
struct _tuple5*)({ struct Cyc_List_List* _temp1740= tqts; if( _temp1740 == 0){
_throw( Null_Exception);} _temp1740->hd;}))).f2); Cyc_Toc_exp_to_c( nv, cur_e);
if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp( field_typ,
cur_e, 0);}{ struct Cyc_Absyn_Stmt* a= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
xexp,({ struct _tagged_string* _temp1741=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp1741[ 0]= xprintf("f%d", i); _temp1741;}),
0), cur_e, 0); rev_assign=({ struct Cyc_List_List* _temp1742=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1742->hd=( void*) a; _temp1742->tl=
rev_assign; _temp1742;});}}}{ struct Cyc_Absyn_Stmt* final_stmt= Cyc_Absyn_exp_stmt(
xexp, 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmts((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)(({ struct Cyc_List_List* _temp1743=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1743->hd=(
void*) final_stmt; _temp1743->tl= rev_assign; _temp1743;})), 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, strct_typ, 0, s2,
0), 0))->r));}} goto _LL1034;} _LL1098: goto _LL1034; _LL1100: { void* t_c= Cyc_Toc_typ_to_c(
_temp1243);( void*)( e->r=( void*)(( void*)( Cyc_Toc_malloc_exp( _temp1243, Cyc_Absyn_sizeoftyp_exp(
t_c, 0)))->r)); goto _LL1034;} _LL1102: Cyc_Toc_stmt_to_c( nv, _temp1245); goto
_LL1034; _LL1104:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1744=( char*)"UnresolvedMem"; struct
_tagged_string _temp1745; _temp1745.curr= _temp1744; _temp1745.base= _temp1744;
_temp1745.last_plus_one= _temp1744 + 14; _temp1745;})); goto _LL1034; _LL1106:((
void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({
char* _temp1746=( char*)"codegen"; struct _tagged_string _temp1747; _temp1747.curr=
_temp1746; _temp1747.base= _temp1746; _temp1747.last_plus_one= _temp1746 + 8;
_temp1747;})); goto _LL1034; _LL1108:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ char* _temp1748=( char*)"fill"; struct _tagged_string
_temp1749; _temp1749.curr= _temp1748; _temp1749.base= _temp1748; _temp1749.last_plus_one=
_temp1748 + 5; _temp1749;})); goto _LL1034; _LL1034:;}} static struct Cyc_Absyn_Stmt*
Cyc_Toc_if_eq_goto_stmt( struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2,
struct _tagged_string* succ_lab, struct _tagged_string* fail_lab){ return Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( e1, e2, 0), Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0);} static struct _tuple13 Cyc_Toc_xlate_pat( struct Cyc_Toc_Env*
nv, void* t, struct Cyc_Absyn_Exp* r, struct Cyc_Absyn_Exp* path, struct Cyc_Absyn_Pat*
p, struct _tagged_string* succ_lab, struct _tagged_string* fail_lab, struct Cyc_List_List*
decls){ struct Cyc_Absyn_Stmt* s;{ void* _temp1750=( void*) p->r; struct Cyc_Absyn_Vardecl*
_temp1790; struct Cyc_Absyn_Vardecl _temp1792; struct Cyc_List_List* _temp1793;
struct Cyc_Core_Opt* _temp1795; int _temp1797; struct Cyc_Absyn_Exp* _temp1799;
void* _temp1801; struct Cyc_Absyn_Tqual* _temp1803; struct _tuple1* _temp1805;
void* _temp1807; struct Cyc_Absyn_Vardecl* _temp1809; int _temp1811; void*
_temp1813; char _temp1815; struct _tagged_string _temp1817; struct Cyc_Absyn_Enumfield*
_temp1819; struct Cyc_Absyn_Enumdecl* _temp1821; struct _tuple1* _temp1823;
struct Cyc_Absyn_Tunionfield* _temp1825; struct Cyc_Absyn_Tuniondecl* _temp1827;
struct Cyc_List_List* _temp1829; struct Cyc_List_List* _temp1831; struct Cyc_Core_Opt*
_temp1833; struct _tuple1* _temp1835; struct Cyc_Absyn_Pat* _temp1837; struct
Cyc_Absyn_Pat _temp1839; struct Cyc_Position_Segment* _temp1840; struct Cyc_Core_Opt*
_temp1842; void* _temp1844; struct Cyc_Absyn_Tunionfield* _temp1846; struct Cyc_Absyn_Tuniondecl*
_temp1848; struct Cyc_List_List* _temp1850; struct Cyc_List_List* _temp1852;
struct Cyc_Core_Opt* _temp1854; struct _tuple1* _temp1856; struct Cyc_Absyn_Pat*
_temp1858; struct Cyc_Absyn_Pat _temp1860; struct Cyc_Position_Segment*
_temp1861; struct Cyc_Core_Opt* _temp1863; void* _temp1865; struct Cyc_Absyn_Tunionfield*
_temp1867; struct Cyc_Absyn_XTuniondecl* _temp1869; struct Cyc_List_List*
_temp1871; struct Cyc_List_List* _temp1873; struct _tuple1* _temp1875; struct
Cyc_List_List* _temp1877; struct Cyc_List_List* _temp1879; struct Cyc_List_List*
_temp1881; struct Cyc_Core_Opt* _temp1883; struct Cyc_Absyn_Structdecl*
_temp1885; struct Cyc_Absyn_Pat* _temp1887; struct _tuple1* _temp1889; struct
Cyc_List_List* _temp1891; struct Cyc_List_List* _temp1893; struct _tuple1*
_temp1895; struct Cyc_List_List* _temp1897; struct Cyc_List_List* _temp1899;
struct _tuple1* _temp1901; struct Cyc_Absyn_Tunionfield* _temp1903; struct Cyc_Absyn_Tuniondecl*
_temp1905; struct Cyc_List_List* _temp1907; struct Cyc_List_List* _temp1909;
struct Cyc_Core_Opt* _temp1911; struct _tuple1* _temp1913; struct Cyc_Absyn_Tunionfield*
_temp1915; struct Cyc_Absyn_XTuniondecl* _temp1917; struct Cyc_List_List*
_temp1919; struct Cyc_List_List* _temp1921; struct _tuple1* _temp1923; _LL1752:
if(( int) _temp1750 == Cyc_Absyn_Wild_p){ goto _LL1753;} else{ goto _LL1754;}
_LL1754: if(( unsigned int) _temp1750 > 2u?(( struct _tunion_struct*) _temp1750)->tag
== Cyc_Absyn_Var_p_tag: 0){ _LL1791: _temp1790=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_p_struct*) _temp1750)->f1; _temp1792=* _temp1790; _LL1808:
_temp1807=( void*) _temp1792.sc; goto _LL1806; _LL1806: _temp1805=( struct
_tuple1*) _temp1792.name; goto _LL1804; _LL1804: _temp1803=( struct Cyc_Absyn_Tqual*)
_temp1792.tq; goto _LL1802; _LL1802: _temp1801=( void*) _temp1792.type; goto
_LL1800; _LL1800: _temp1799=( struct Cyc_Absyn_Exp*) _temp1792.initializer; goto
_LL1798; _LL1798: _temp1797=( int) _temp1792.shadow; goto _LL1796; _LL1796:
_temp1795=( struct Cyc_Core_Opt*) _temp1792.region; goto _LL1794; _LL1794:
_temp1793=( struct Cyc_List_List*) _temp1792.attributes; goto _LL1755;} else{
goto _LL1756;} _LL1756: if(( unsigned int) _temp1750 > 2u?(( struct
_tunion_struct*) _temp1750)->tag == Cyc_Absyn_Reference_p_tag: 0){ _LL1810:
_temp1809=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Reference_p_struct*)
_temp1750)->f1; goto _LL1757;} else{ goto _LL1758;} _LL1758: if(( int) _temp1750
== Cyc_Absyn_Null_p){ goto _LL1759;} else{ goto _LL1760;} _LL1760: if((
unsigned int) _temp1750 > 2u?(( struct _tunion_struct*) _temp1750)->tag == Cyc_Absyn_Int_p_tag:
0){ _LL1814: _temp1813=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp1750)->f1;
goto _LL1812; _LL1812: _temp1811=( int)(( struct Cyc_Absyn_Int_p_struct*)
_temp1750)->f2; goto _LL1761;} else{ goto _LL1762;} _LL1762: if(( unsigned int)
_temp1750 > 2u?(( struct _tunion_struct*) _temp1750)->tag == Cyc_Absyn_Char_p_tag:
0){ _LL1816: _temp1815=( char)(( struct Cyc_Absyn_Char_p_struct*) _temp1750)->f1;
goto _LL1763;} else{ goto _LL1764;} _LL1764: if(( unsigned int) _temp1750 > 2u?((
struct _tunion_struct*) _temp1750)->tag == Cyc_Absyn_Float_p_tag: 0){ _LL1818:
_temp1817=( struct _tagged_string)(( struct Cyc_Absyn_Float_p_struct*) _temp1750)->f1;
goto _LL1765;} else{ goto _LL1766;} _LL1766: if(( unsigned int) _temp1750 > 2u?((
struct _tunion_struct*) _temp1750)->tag == Cyc_Absyn_Enum_p_tag: 0){ _LL1824:
_temp1823=( struct _tuple1*)(( struct Cyc_Absyn_Enum_p_struct*) _temp1750)->f1;
goto _LL1822; _LL1822: _temp1821=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp1750)->f2; goto _LL1820; _LL1820: _temp1819=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_p_struct*) _temp1750)->f3; goto _LL1767;} else{ goto
_LL1768;} _LL1768: if(( unsigned int) _temp1750 > 2u?(( struct _tunion_struct*)
_temp1750)->tag == Cyc_Absyn_Tunion_p_tag: 0){ _LL1836: _temp1835=( struct
_tuple1*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp1750)->f1; goto _LL1834;
_LL1834: _temp1833=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1750)->f2; goto _LL1832; _LL1832: _temp1831=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1750)->f3; goto _LL1830; _LL1830:
_temp1829=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1750)->f4; if( _temp1829 == 0){ goto _LL1828;} else{ goto _LL1770;} _LL1828:
_temp1827=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1750)->f5; goto _LL1826; _LL1826: _temp1825=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1750)->f6; goto _LL1769;} else{ goto
_LL1770;} _LL1770: if(( unsigned int) _temp1750 > 2u?(( struct _tunion_struct*)
_temp1750)->tag == Cyc_Absyn_Pointer_p_tag: 0){ _LL1838: _temp1837=( struct Cyc_Absyn_Pat*)((
struct Cyc_Absyn_Pointer_p_struct*) _temp1750)->f1; _temp1839=* _temp1837;
_LL1845: _temp1844=( void*) _temp1839.r; if(( unsigned int) _temp1844 > 2u?((
struct _tunion_struct*) _temp1844)->tag == Cyc_Absyn_Tunion_p_tag: 0){ _LL1857:
_temp1856=( struct _tuple1*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp1844)->f1;
goto _LL1855; _LL1855: _temp1854=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1844)->f2; goto _LL1853; _LL1853: _temp1852=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1844)->f3; goto _LL1851; _LL1851:
_temp1850=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1844)->f4; goto _LL1849; _LL1849: _temp1848=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1844)->f5; goto _LL1847; _LL1847:
_temp1846=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1844)->f6; goto _LL1843;} else{ goto _LL1772;} _LL1843: _temp1842=( struct
Cyc_Core_Opt*) _temp1839.topt; goto _LL1841; _LL1841: _temp1840=( struct Cyc_Position_Segment*)
_temp1839.loc; goto _LL1771;} else{ goto _LL1772;} _LL1772: if(( unsigned int)
_temp1750 > 2u?(( struct _tunion_struct*) _temp1750)->tag == Cyc_Absyn_Pointer_p_tag:
0){ _LL1859: _temp1858=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1750)->f1; _temp1860=* _temp1858; _LL1866: _temp1865=( void*) _temp1860.r;
if(( unsigned int) _temp1865 > 2u?(( struct _tunion_struct*) _temp1865)->tag ==
Cyc_Absyn_XTunion_p_tag: 0){ _LL1876: _temp1875=( struct _tuple1*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp1865)->f1; goto _LL1874; _LL1874: _temp1873=( struct Cyc_List_List*)((
struct Cyc_Absyn_XTunion_p_struct*) _temp1865)->f2; goto _LL1872; _LL1872:
_temp1871=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp1865)->f3; goto _LL1870; _LL1870: _temp1869=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_p_struct*) _temp1865)->f4; goto _LL1868; _LL1868:
_temp1867=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp1865)->f5; goto _LL1864;} else{ goto _LL1774;} _LL1864: _temp1863=( struct
Cyc_Core_Opt*) _temp1860.topt; goto _LL1862; _LL1862: _temp1861=( struct Cyc_Position_Segment*)
_temp1860.loc; goto _LL1773;} else{ goto _LL1774;} _LL1774: if(( unsigned int)
_temp1750 > 2u?(( struct _tunion_struct*) _temp1750)->tag == Cyc_Absyn_Tuple_p_tag:
0){ _LL1878: _temp1877=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*)
_temp1750)->f1; goto _LL1775;} else{ goto _LL1776;} _LL1776: if(( unsigned int)
_temp1750 > 2u?(( struct _tunion_struct*) _temp1750)->tag == Cyc_Absyn_Struct_p_tag:
0){ _LL1886: _temp1885=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1750)->f1; goto _LL1884; _LL1884: _temp1883=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1750)->f2; goto _LL1882; _LL1882:
_temp1881=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1750)->f3; goto _LL1880; _LL1880: _temp1879=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1750)->f4; goto _LL1777;} else{ goto
_LL1778;} _LL1778: if(( unsigned int) _temp1750 > 2u?(( struct _tunion_struct*)
_temp1750)->tag == Cyc_Absyn_Pointer_p_tag: 0){ _LL1888: _temp1887=( struct Cyc_Absyn_Pat*)((
struct Cyc_Absyn_Pointer_p_struct*) _temp1750)->f1; goto _LL1779;} else{ goto
_LL1780;} _LL1780: if(( unsigned int) _temp1750 > 2u?(( struct _tunion_struct*)
_temp1750)->tag == Cyc_Absyn_UnknownId_p_tag: 0){ _LL1890: _temp1889=( struct
_tuple1*)(( struct Cyc_Absyn_UnknownId_p_struct*) _temp1750)->f1; goto _LL1781;}
else{ goto _LL1782;} _LL1782: if(( unsigned int) _temp1750 > 2u?(( struct
_tunion_struct*) _temp1750)->tag == Cyc_Absyn_UnknownCall_p_tag: 0){ _LL1896:
_temp1895=( struct _tuple1*)(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp1750)->f1;
goto _LL1894; _LL1894: _temp1893=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1750)->f2; goto _LL1892; _LL1892: _temp1891=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_p_struct*) _temp1750)->f3; goto _LL1783;} else{
goto _LL1784;} _LL1784: if(( unsigned int) _temp1750 > 2u?(( struct
_tunion_struct*) _temp1750)->tag == Cyc_Absyn_UnknownFields_p_tag: 0){ _LL1902:
_temp1901=( struct _tuple1*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1750)->f1; goto _LL1900; _LL1900: _temp1899=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownFields_p_struct*) _temp1750)->f2; goto _LL1898; _LL1898:
_temp1897=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1750)->f3; goto _LL1785;} else{ goto _LL1786;} _LL1786: if(( unsigned int)
_temp1750 > 2u?(( struct _tunion_struct*) _temp1750)->tag == Cyc_Absyn_Tunion_p_tag:
0){ _LL1914: _temp1913=( struct _tuple1*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1750)->f1; goto _LL1912; _LL1912: _temp1911=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1750)->f2; goto _LL1910; _LL1910:
_temp1909=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1750)->f3; goto _LL1908; _LL1908: _temp1907=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1750)->f4; goto _LL1906; _LL1906:
_temp1905=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1750)->f5; goto _LL1904; _LL1904: _temp1903=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1750)->f6; goto _LL1787;} else{ goto
_LL1788;} _LL1788: if(( unsigned int) _temp1750 > 2u?(( struct _tunion_struct*)
_temp1750)->tag == Cyc_Absyn_XTunion_p_tag: 0){ _LL1924: _temp1923=( struct
_tuple1*)(( struct Cyc_Absyn_XTunion_p_struct*) _temp1750)->f1; goto _LL1922;
_LL1922: _temp1921=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp1750)->f2; goto _LL1920; _LL1920: _temp1919=( struct Cyc_List_List*)((
struct Cyc_Absyn_XTunion_p_struct*) _temp1750)->f3; goto _LL1918; _LL1918:
_temp1917=( struct Cyc_Absyn_XTuniondecl*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp1750)->f4; goto _LL1916; _LL1916: _temp1915=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_XTunion_p_struct*) _temp1750)->f5; goto _LL1789;} else{ goto
_LL1751;} _LL1753: s= Cyc_Absyn_goto_stmt( succ_lab, 0); goto _LL1751; _LL1755:
nv= Cyc_Toc_add_varmap( nv, _temp1805, r); s= Cyc_Absyn_goto_stmt( succ_lab, 0);
goto _LL1751; _LL1757: { struct _tuple1* v= Cyc_Toc_temp_var(); decls=({ struct
Cyc_List_List* _temp1925=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1925->hd=( void*)({ struct _tuple14* _temp1926=( struct _tuple14*)
GC_malloc( sizeof( struct _tuple14)); _temp1926->f1= v; _temp1926->f2= Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c( t), Cyc_Toc_mt_tq); _temp1926;}); _temp1925->tl= decls;
_temp1925;}); nv= Cyc_Toc_add_varmap( nv, _temp1809->name, Cyc_Absyn_var_exp( v,
0)); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_address_exp(
path, 0), 0), Cyc_Absyn_goto_stmt( succ_lab, 0), 0); goto _LL1751;} _LL1759: s=
Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_signed_int_exp( 0, 0), succ_lab, fail_lab);
goto _LL1751; _LL1761: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_int_exp(
_temp1813, _temp1811, 0), succ_lab, fail_lab); goto _LL1751; _LL1763: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_char_exp( _temp1815, 0), succ_lab, fail_lab); goto _LL1751; _LL1765:
s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_float_exp( _temp1817, 0), succ_lab,
fail_lab); goto _LL1751; _LL1767: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Enum_e_struct* _temp1927=( struct Cyc_Absyn_Enum_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_e_struct)); _temp1927[ 0]=({ struct Cyc_Absyn_Enum_e_struct
_temp1928; _temp1928.tag= Cyc_Absyn_Enum_e_tag; _temp1928.f1= _temp1823;
_temp1928.f2=( struct Cyc_Absyn_Enumdecl*) _temp1821; _temp1928.f3=( struct Cyc_Absyn_Enumfield*)
_temp1819; _temp1928;}); _temp1927;}), 0), succ_lab, fail_lab); goto _LL1751;
_LL1769: s= Cyc_Toc_if_eq_goto_stmt( Cyc_Absyn_cast_exp( Cyc_Absyn_sint_t, r, 0),
Cyc_Absyn_var_exp( _temp1835, 0), succ_lab, fail_lab); goto _LL1751; _LL1771: {
struct Cyc_List_List* ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp1850); struct _tuple1* efstrct= Cyc_Toc_collapse_qvar_tag( _temp1846->name,(
struct _tagged_string)({ char* _temp1929=( char*)"_struct"; struct
_tagged_string _temp1930; _temp1930.curr= _temp1929; _temp1930.base= _temp1929;
_temp1930.last_plus_one= _temp1929 + 8; _temp1930;})); struct Cyc_Absyn_Exp*
rcast= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_strctq( efstrct), Cyc_Toc_mt_tq),
r, 0);{ struct Cyc_List_List* rps=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( _temp1850); for( 0; rps != 0; rps=({ struct Cyc_List_List*
_temp1931= rps; if( _temp1931 == 0){ _throw( Null_Exception);} _temp1931->tl;}),
cnt --){ struct Cyc_Absyn_Pat* p2=( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List*
_temp1932= rps; if( _temp1932 == 0){ _throw( Null_Exception);} _temp1932->hd;});
struct _tuple1* v= Cyc_Toc_temp_var(); void* t2=( void*)({ struct Cyc_Core_Opt*
_temp1933= p2->topt; if( _temp1933 == 0){ _throw( Null_Exception);} _temp1933->v;});
void* t2c= Cyc_Toc_typ_to_c( t2); struct _tagged_string* lab= Cyc_Toc_fresh_label();
decls=({ struct Cyc_List_List* _temp1934=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1934->hd=( void*)({ struct _tuple14*
_temp1935=( struct _tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp1935->f1=
v; _temp1935->f2= t2c; _temp1935;}); _temp1934->tl= decls; _temp1934;});{ struct
_tuple13 result= Cyc_Toc_xlate_pat( nv, t2, Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structarrow_exp(
rcast,({ struct _tagged_string* _temp1936=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp1936[ 0]= xprintf("f%d", cnt);
_temp1936;}), 0), p2, succ_lab, fail_lab, decls); nv= result.f1; decls= result.f2;{
struct Cyc_Absyn_Stmt* s3= result.f3; struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_cast_exp( t2c, Cyc_Absyn_structarrow_exp(
rcast,({ struct _tagged_string* _temp1937=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp1937[ 0]= xprintf("f%d", cnt);
_temp1937;}), 0), 0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s1, s3,
0); ss=({ struct Cyc_List_List* _temp1938=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1938->hd=( void*) Cyc_Absyn_label_stmt( lab,
s2, 0); _temp1938->tl= ss; _temp1938;}); succ_lab= lab;}}}}{ unsigned int
max_tag= 0;{ struct Cyc_List_List* fs=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp1939= _temp1848->fields; if( _temp1939 == 0){ _throw( Null_Exception);}
_temp1939->v;}); for( 0; fs != 0; fs=({ struct Cyc_List_List* _temp1940= fs; if(
_temp1940 == 0){ _throw( Null_Exception);} _temp1940->tl;})){ struct Cyc_Absyn_Tunionfield*
f=( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp1941= fs; if(
_temp1941 == 0){ _throw( Null_Exception);} _temp1941->hd;}); if( f->typs == 0){
max_tag ++;}}}{ struct Cyc_Absyn_Exp* max_tag_exp= Cyc_Absyn_uint_exp( max_tag,
0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_strct(
Cyc_Toc__tunion_struct_sp), Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_structarrow_exp(
e3, Cyc_Toc_tag_sp, 0); struct Cyc_Absyn_Exp* e5= Cyc_Absyn_gt_exp( Cyc_Absyn_cast_exp(
Cyc_Absyn_uint_t, r, 0), max_tag_exp, 0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp(
Cyc_Toc_collapse_qvar_tag( _temp1856,( struct _tagged_string)({ char* _temp1942=(
char*)"_tag"; struct _tagged_string _temp1943; _temp1943.curr= _temp1942;
_temp1943.base= _temp1942; _temp1943.last_plus_one= _temp1942 + 5; _temp1943;})),
0); struct Cyc_Absyn_Exp* test_exp= max_tag > 0? Cyc_Absyn_and_exp( e5, Cyc_Absyn_eq_exp(
e1, e, 0), 0): Cyc_Absyn_eq_exp( e1, e, 0); s= Cyc_Absyn_ifthenelse_stmt(
test_exp, Cyc_Absyn_seq_stmts( ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);
goto _LL1751;}}} _LL1773: { struct Cyc_List_List* ss= 0; int cnt=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp1871); struct _tuple1* xfstrct=
Cyc_Toc_collapse_qvar_tag( _temp1867->name,( struct _tagged_string)({ char*
_temp1944=( char*)"_struct"; struct _tagged_string _temp1945; _temp1945.curr=
_temp1944; _temp1945.base= _temp1944; _temp1945.last_plus_one= _temp1944 + 8;
_temp1945;})); struct Cyc_Absyn_Exp* rcast= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ(
Cyc_Absyn_strctq( xfstrct), Cyc_Toc_mt_tq), r, 0);{ struct Cyc_List_List* rps=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( _temp1871);
for( 0; rps != 0; rps=({ struct Cyc_List_List* _temp1946= rps; if( _temp1946 ==
0){ _throw( Null_Exception);} _temp1946->tl;}), cnt --){ struct Cyc_Absyn_Pat*
p2=( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List* _temp1947= rps; if(
_temp1947 == 0){ _throw( Null_Exception);} _temp1947->hd;}); struct _tuple1* v=
Cyc_Toc_temp_var(); void* t2=( void*)({ struct Cyc_Core_Opt* _temp1948= p2->topt;
if( _temp1948 == 0){ _throw( Null_Exception);} _temp1948->v;}); struct
_tagged_string* lab= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp1949=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1949->hd=( void*)({ struct _tuple14* _temp1950=( struct _tuple14*)
GC_malloc( sizeof( struct _tuple14)); _temp1950->f1= v; _temp1950->f2= Cyc_Toc_typ_to_c(
t2); _temp1950;}); _temp1949->tl= decls; _temp1949;});{ struct _tuple13 result=
Cyc_Toc_xlate_pat( nv, t2, Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structarrow_exp(
rcast,({ struct _tagged_string* _temp1951=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp1951[ 0]= xprintf("f%d", cnt);
_temp1951;}), 0), p2, succ_lab, fail_lab, decls); nv= result.f1; decls= result.f2;{
struct Cyc_Absyn_Stmt* s3= result.f3; struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structarrow_exp( rcast,({ struct
_tagged_string* _temp1952=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp1952[ 0]= xprintf("f%d", cnt); _temp1952;}), 0), 0);
struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s1, s3, 0); ss=({ struct Cyc_List_List*
_temp1953=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1953->hd=( void*) Cyc_Absyn_label_stmt( lab, s2, 0); _temp1953->tl= ss;
_temp1953;}); succ_lab= lab;}}}} if( _temp1871 == 0){ s= Cyc_Toc_if_eq_goto_stmt(
Cyc_Absyn_structarrow_exp( r, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp( Cyc_Toc_collapse_qvar_tag(
_temp1875,( struct _tagged_string)({ char* _temp1954=( char*)"_tag"; struct
_tagged_string _temp1955; _temp1955.curr= _temp1954; _temp1955.base= _temp1954;
_temp1955.last_plus_one= _temp1954 + 5; _temp1955;})), 0), succ_lab, fail_lab);}
else{ struct Cyc_Absyn_Exp* e3= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_strct(
Cyc_Toc__xtunion_struct_sp), Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp* e2=
Cyc_Absyn_deref_exp( e3, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_structmember_exp(
e2, Cyc_Toc_tag_sp, 0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp( Cyc_Toc_collapse_qvar_tag(
_temp1875,( struct _tagged_string)({ char* _temp1956=( char*)"_tag"; struct
_tagged_string _temp1957; _temp1957.curr= _temp1956; _temp1957.base= _temp1956;
_temp1957.last_plus_one= _temp1956 + 5; _temp1957;})), 0); s= Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( e1, e, 0), Cyc_Absyn_seq_stmts( ss, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0);} goto _LL1751;} _LL1775: { struct Cyc_List_List* ss= 0; int
cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1877);{ struct
Cyc_List_List* rps=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp1877); for( 0; rps != 0; rps=({ struct Cyc_List_List* _temp1958= rps; if(
_temp1958 == 0){ _throw( Null_Exception);} _temp1958->tl;}), cnt --){ struct Cyc_Absyn_Pat*
p2=( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List* _temp1959= rps; if(
_temp1959 == 0){ _throw( Null_Exception);} _temp1959->hd;}); struct _tuple1* v=
Cyc_Toc_temp_var(); void* t2=( void*)({ struct Cyc_Core_Opt* _temp1960= p2->topt;
if( _temp1960 == 0){ _throw( Null_Exception);} _temp1960->v;}); struct
_tagged_string* lab= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp1961=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1961->hd=( void*)({ struct _tuple14* _temp1962=( struct _tuple14*)
GC_malloc( sizeof( struct _tuple14)); _temp1962->f1= v; _temp1962->f2= Cyc_Toc_typ_to_c(
t2); _temp1962;}); _temp1961->tl= decls; _temp1961;});{ struct _tuple13 result=
Cyc_Toc_xlate_pat( nv, t2, Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structmember_exp(
path,({ struct _tagged_string* _temp1963=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp1963[ 0]= xprintf("f%d", cnt);
_temp1963;}), 0), p2, succ_lab, fail_lab, decls); nv= result.f1; decls= result.f2;{
struct Cyc_Absyn_Stmt* s2= result.f3; struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structmember_exp( r,({
struct _tagged_string* _temp1964=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp1964[ 0]= xprintf("f%d", cnt); _temp1964;}), 0),
0), s2, 0); ss=({ struct Cyc_List_List* _temp1965=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1965->hd=( void*) Cyc_Absyn_label_stmt(
lab, s3, 0); _temp1965->tl= ss; _temp1965;}); succ_lab= lab;}}}} s= Cyc_Absyn_seq_stmts(
ss, 0); goto _LL1751;} _LL1777: { struct Cyc_List_List* ss= 0;{ struct Cyc_List_List*
dlps=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp1879); for( 0; dlps != 0; dlps=({ struct Cyc_List_List* _temp1966= dlps;
if( _temp1966 == 0){ _throw( Null_Exception);} _temp1966->tl;})){ struct
_tuple15* tup=( struct _tuple15*)({ struct Cyc_List_List* _temp1967= dlps; if(
_temp1967 == 0){ _throw( Null_Exception);} _temp1967->hd;}); struct Cyc_Absyn_Pat*
p2=(* tup).f2; struct _tagged_string* f;{ void* _temp1969=( void*)({ struct Cyc_List_List*
_temp1968=(* tup).f1; if( _temp1968 == 0){ _throw( Null_Exception);} _temp1968->hd;});
struct _tagged_string* _temp1975; _LL1971: if((( struct _tunion_struct*)
_temp1969)->tag == Cyc_Absyn_FieldName_tag){ _LL1976: _temp1975=( struct
_tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp1969)->f1; goto
_LL1972;} else{ goto _LL1973;} _LL1973: goto _LL1974; _LL1972: f= _temp1975;
goto _LL1970; _LL1974:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Toc_Match_error_struct*
_temp1977=( struct Cyc_Toc_Match_error_struct*) GC_malloc( sizeof( struct Cyc_Toc_Match_error_struct));
_temp1977[ 0]=({ struct Cyc_Toc_Match_error_struct _temp1978; _temp1978.tag= Cyc_Toc_Match_error_tag;
_temp1978;}); _temp1977;})); _LL1970:;}{ struct _tuple1* v= Cyc_Toc_temp_var();
void* t2=( void*)({ struct Cyc_Core_Opt* _temp1979= p2->topt; if( _temp1979 == 0){
_throw( Null_Exception);} _temp1979->v;}); void* t2c= Cyc_Toc_typ_to_c( t2);
struct _tagged_string* lab= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp1980=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1980->hd=( void*)({ struct _tuple14* _temp1981=( struct _tuple14*)
GC_malloc( sizeof( struct _tuple14)); _temp1981->f1= v; _temp1981->f2= t2c;
_temp1981;}); _temp1980->tl= decls; _temp1980;});{ struct _tuple13 result= Cyc_Toc_xlate_pat(
nv, t2, Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structmember_exp( path, f, 0), p2,
succ_lab, fail_lab, decls); nv= result.f1; decls= result.f2;{ struct Cyc_Absyn_Stmt*
s2= result.f3; struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_cast_exp( t2c, Cyc_Absyn_structmember_exp( r,
f, 0), 0), 0), s2, 0); ss=({ struct Cyc_List_List* _temp1982=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1982->hd=( void*) Cyc_Absyn_label_stmt(
lab, s3, 0); _temp1982->tl= ss; _temp1982;}); succ_lab= lab;}}}}} s= Cyc_Absyn_seq_stmts(
ss, 0); goto _LL1751;} _LL1779: { struct _tuple1* v= Cyc_Toc_temp_var(); void*
t2=( void*)({ struct Cyc_Core_Opt* _temp1983= _temp1887->topt; if( _temp1983 ==
0){ _throw( Null_Exception);} _temp1983->v;}); decls=({ struct Cyc_List_List*
_temp1984=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1984->hd=( void*)({ struct _tuple14* _temp1985=( struct _tuple14*)
GC_malloc( sizeof( struct _tuple14)); _temp1985->f1= v; _temp1985->f2= Cyc_Toc_typ_to_c(
t2); _temp1985;}); _temp1984->tl= decls; _temp1984;});{ struct _tuple13 result=
Cyc_Toc_xlate_pat( nv, t2, Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_deref_exp( path,
0), _temp1887, succ_lab, fail_lab, decls); nv= result.f1; decls= result.f2;{
struct Cyc_Absyn_Stmt* s2= result.f3; struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_deref_exp( r, 0), 0),
s2, 0); if( Cyc_Toc_is_nullable( t)){ s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp(
r, Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_goto_stmt( fail_lab, 0), s3, 0);}
else{ s= s3;} goto _LL1751;}}} _LL1781: s=(( struct Cyc_Absyn_Stmt*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1986=(
char*)"unknownid"; struct _tagged_string _temp1987; _temp1987.curr= _temp1986;
_temp1987.base= _temp1986; _temp1987.last_plus_one= _temp1986 + 10; _temp1987;}));
goto _LL1751; _LL1783: s=(( struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1988=( char*)"unknowncall";
struct _tagged_string _temp1989; _temp1989.curr= _temp1988; _temp1989.base=
_temp1988; _temp1989.last_plus_one= _temp1988 + 12; _temp1989;})); goto _LL1751;
_LL1785: s=(( struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1990=( char*)"unknownfields"; struct
_tagged_string _temp1991; _temp1991.curr= _temp1990; _temp1991.base= _temp1990;
_temp1991.last_plus_one= _temp1990 + 14; _temp1991;})); goto _LL1751; _LL1787:
goto _LL1789; _LL1789: s=(( struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1992=( char*)"non-null [x]tunion pattern not under &";
struct _tagged_string _temp1993; _temp1993.curr= _temp1992; _temp1993.base=
_temp1992; _temp1993.last_plus_one= _temp1992 + 39; _temp1993;})); goto _LL1751;
_LL1751:;} return({ struct _tuple13 _temp1994; _temp1994.f1= nv; _temp1994.f2=
decls; _temp1994.f3= s; _temp1994;});} static struct _tuple16* Cyc_Toc_gen_label(
struct Cyc_Absyn_Switch_clause* sc){ return({ struct _tuple16* _temp1995=(
struct _tuple16*) GC_malloc( sizeof( struct _tuple16)); _temp1995->f1= Cyc_Toc_fresh_label();
_temp1995->f2= Cyc_Toc_fresh_label(); _temp1995->f3= sc; _temp1995;});} static
void Cyc_Toc_xlate_switch( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Stmt*
whole_s, struct Cyc_Absyn_Exp* e, struct Cyc_List_List* scs){ Cyc_Toc_exp_to_c(
nv, e);{ void* t=( void*)({ struct Cyc_Core_Opt* _temp1996= e->topt; if(
_temp1996 == 0){ _throw( Null_Exception);} _temp1996->v;}); int leave_as_switch;{
void* _temp1997= Cyc_Tcutil_compress( t); void* _temp2003; void* _temp2005;
_LL1999: if(( unsigned int) _temp1997 > 4u?(( struct _tunion_struct*) _temp1997)->tag
== Cyc_Absyn_IntType_tag: 0){ _LL2006: _temp2005=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1997)->f1; goto _LL2004; _LL2004: _temp2003=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1997)->f2; goto _LL2000;} else{ goto _LL2001;} _LL2001: goto _LL2002;
_LL2000: leave_as_switch= 1; goto _LL1998; _LL2002: leave_as_switch= 0; goto
_LL1998; _LL1998:;}{ struct Cyc_List_List* scs1= scs; for( 0; scs1 != 0; scs1=({
struct Cyc_List_List* _temp2007= scs1; if( _temp2007 == 0){ _throw(
Null_Exception);} _temp2007->tl;})){ if(( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp2009=(( struct Cyc_Absyn_Switch_clause*)({ struct Cyc_List_List* _temp2008=
scs1; if( _temp2008 == 0){ _throw( Null_Exception);} _temp2008->hd;}))->pat_vars;
if( _temp2009 == 0){ _throw( Null_Exception);} _temp2009->v;}) != 0){
leave_as_switch= 0; break;}}} if( leave_as_switch){ struct _tagged_string*
next_l= Cyc_Toc_fresh_label();{ struct Cyc_List_List* scs1= scs; for( 0; scs1 !=
0; scs1=({ struct Cyc_List_List* _temp2010= scs1; if( _temp2010 == 0){ _throw(
Null_Exception);} _temp2010->tl;})){ struct Cyc_Absyn_Stmt* s=(( struct Cyc_Absyn_Switch_clause*)({
struct Cyc_List_List* _temp2011= scs1; if( _temp2011 == 0){ _throw(
Null_Exception);} _temp2011->hd;}))->body;(( struct Cyc_Absyn_Switch_clause*)({
struct Cyc_List_List* _temp2012= scs1; if( _temp2012 == 0){ _throw(
Null_Exception);} _temp2012->hd;}))->body= Cyc_Absyn_label_stmt( next_l, s, 0);
next_l= Cyc_Toc_fresh_label(); Cyc_Toc_stmt_to_c( Cyc_Toc_switch_as_switch_env(
nv, next_l), s);}} return;}{ struct _tuple1* v= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
r= Cyc_Absyn_var_exp( v, 0); struct Cyc_Absyn_Exp* path= Cyc_Absyn_var_exp( v, 0);
struct _tagged_string* end_l= Cyc_Toc_fresh_label(); struct Cyc_List_List* lscs=((
struct Cyc_List_List*(*)( struct _tuple16*(* f)( struct Cyc_Absyn_Switch_clause*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_gen_label, scs); struct Cyc_List_List*
test_stmts= 0; struct Cyc_List_List* nvs= 0; struct Cyc_List_List* decls= 0;{
struct Cyc_List_List* lscs2= lscs; for( 0; lscs2 != 0; lscs2=({ struct Cyc_List_List*
_temp2013= lscs2; if( _temp2013 == 0){ _throw( Null_Exception);} _temp2013->tl;})){
struct Cyc_Absyn_Switch_clause* sc=(*(( struct _tuple16*)({ struct Cyc_List_List*
_temp2014= lscs2; if( _temp2014 == 0){ _throw( Null_Exception);} _temp2014->hd;}))).f3;
struct _tagged_string* fail_lab=({ struct Cyc_List_List* _temp2015= lscs2; if(
_temp2015 == 0){ _throw( Null_Exception);} _temp2015->tl;}) == 0? end_l:(*((
struct _tuple16*)({ struct Cyc_List_List* _temp2017=({ struct Cyc_List_List*
_temp2016= lscs2; if( _temp2016 == 0){ _throw( Null_Exception);} _temp2016->tl;});
if( _temp2017 == 0){ _throw( Null_Exception);} _temp2017->hd;}))).f1; struct
_tagged_string* succ_lab=(*(( struct _tuple16*)({ struct Cyc_List_List*
_temp2018= lscs2; if( _temp2018 == 0){ _throw( Null_Exception);} _temp2018->hd;}))).f2;
if( sc->where_clause == 0){ struct _tuple13 res= Cyc_Toc_xlate_pat( nv, t, r,
path, sc->pattern, succ_lab, fail_lab, decls); nvs=({ struct Cyc_List_List*
_temp2019=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2019->hd=( void*) res.f1; _temp2019->tl= nvs; _temp2019;}); decls= res.f2;
test_stmts=({ struct Cyc_List_List* _temp2020=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2020->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple16*)({ struct Cyc_List_List* _temp2021= lscs2; if( _temp2021 == 0){
_throw( Null_Exception);} _temp2021->hd;}))).f1, res.f3, 0); _temp2020->tl=
test_stmts; _temp2020;});} else{ struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp2022= sc->where_clause; if( _temp2022 == 0){ _throw(
Null_Exception);} _temp2022;}); struct _tagged_string* intermed_lab= Cyc_Toc_fresh_label();
struct _tuple13 res= Cyc_Toc_xlate_pat( nv, t, r, path, sc->pattern,
intermed_lab, fail_lab, decls); Cyc_Toc_exp_to_c( res.f1, e); nvs=({ struct Cyc_List_List*
_temp2023=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2023->hd=( void*) res.f1; _temp2023->tl= nvs; _temp2023;}); decls= res.f2;{
struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_ifthenelse_stmt( e, Cyc_Absyn_goto_stmt(
succ_lab, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0); struct Cyc_Absyn_Stmt* s3=
Cyc_Absyn_label_stmt( intermed_lab, s2, 0); test_stmts=({ struct Cyc_List_List*
_temp2024=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2024->hd=( void*) Cyc_Absyn_label_stmt((*(( struct _tuple16*)({ struct Cyc_List_List*
_temp2025= lscs2; if( _temp2025 == 0){ _throw( Null_Exception);} _temp2025->hd;}))).f1,
Cyc_Absyn_seq_stmt( res.f3, s3, 0), 0); _temp2024->tl= test_stmts; _temp2024;});}}}}{
struct Cyc_Absyn_Stmt* test_stmt= Cyc_Absyn_seq_stmts((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( test_stmts), 0); nvs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( nvs);{ struct Cyc_Absyn_Stmt*
clauses= Cyc_Absyn_skip_stmt( 0); if( lscs != 0){ for( 0;({ struct Cyc_List_List*
_temp2026= lscs; if( _temp2026 == 0){ _throw( Null_Exception);} _temp2026->tl;})
!= 0; lscs=({ struct Cyc_List_List* _temp2027= lscs; if( _temp2027 == 0){ _throw(
Null_Exception);} _temp2027->tl;}), nvs=({ struct Cyc_List_List* _temp2028= nvs;
if( _temp2028 == 0){ _throw( Null_Exception);} _temp2028->tl;})){ struct Cyc_Toc_Env*
e=( struct Cyc_Toc_Env*)({ struct Cyc_List_List* _temp2029= nvs; if( _temp2029
== 0){ _throw( Null_Exception);} _temp2029->hd;}); struct Cyc_Absyn_Stmt* s=((*((
struct _tuple16*)({ struct Cyc_List_List* _temp2030= lscs; if( _temp2030 == 0){
_throw( Null_Exception);} _temp2030->hd;}))).f3)->body; Cyc_Toc_stmt_to_c( Cyc_Toc_non_last_switchclause_env(
e, end_l,(*(( struct _tuple16*)({ struct Cyc_List_List* _temp2032=({ struct Cyc_List_List*
_temp2031= lscs; if( _temp2031 == 0){ _throw( Null_Exception);} _temp2031->tl;});
if( _temp2032 == 0){ _throw( Null_Exception);} _temp2032->hd;}))).f2,( struct
Cyc_List_List*)({ struct Cyc_Core_Opt* _temp2035=((*(( struct _tuple16*)({
struct Cyc_List_List* _temp2034=({ struct Cyc_List_List* _temp2033= lscs; if(
_temp2033 == 0){ _throw( Null_Exception);} _temp2033->tl;}); if( _temp2034 == 0){
_throw( Null_Exception);} _temp2034->hd;}))).f3)->pat_vars; if( _temp2035 == 0){
_throw( Null_Exception);} _temp2035->v;}),( struct Cyc_Toc_Env*)({ struct Cyc_List_List*
_temp2037=({ struct Cyc_List_List* _temp2036= nvs; if( _temp2036 == 0){ _throw(
Null_Exception);} _temp2036->tl;}); if( _temp2037 == 0){ _throw( Null_Exception);}
_temp2037->hd;})), s); clauses= Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*((
struct _tuple16*)({ struct Cyc_List_List* _temp2038= lscs; if( _temp2038 == 0){
_throw( Null_Exception);} _temp2038->hd;}))).f2, s, 0), 0);}{ struct Cyc_Toc_Env*
e=( struct Cyc_Toc_Env*)({ struct Cyc_List_List* _temp2039= nvs; if( _temp2039
== 0){ _throw( Null_Exception);} _temp2039->hd;}); struct Cyc_Absyn_Stmt* s=((*((
struct _tuple16*)({ struct Cyc_List_List* _temp2040= lscs; if( _temp2040 == 0){
_throw( Null_Exception);} _temp2040->hd;}))).f3)->body; Cyc_Toc_stmt_to_c( Cyc_Toc_last_switchclause_env(
e, end_l), s); clauses= Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*((
struct _tuple16*)({ struct Cyc_List_List* _temp2041= lscs; if( _temp2041 == 0){
_throw( Null_Exception);} _temp2041->hd;}))).f2, s, 0), 0);}}{ struct Cyc_Absyn_Stmt*
res= Cyc_Absyn_seq_stmt( test_stmt, Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt(
end_l, Cyc_Absyn_skip_stmt( 0), 0), 0), 0); for( decls; decls != 0; decls=({
struct Cyc_List_List* _temp2042= decls; if( _temp2042 == 0){ _throw(
Null_Exception);} _temp2042->tl;})){ struct _tuple14 _temp2046; void* _temp2047;
struct _tuple1* _temp2049; struct _tuple14* _temp2044=( struct _tuple14*)({
struct Cyc_List_List* _temp2043= decls; if( _temp2043 == 0){ _throw(
Null_Exception);} _temp2043->hd;}); _temp2046=* _temp2044; _LL2050: _temp2049=
_temp2046.f1; goto _LL2048; _LL2048: _temp2047= _temp2046.f2; goto _LL2045;
_LL2045: res= Cyc_Absyn_declare_stmt( _temp2049, _temp2047, 0, res, 0);}( void*)(
whole_s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt( v, Cyc_Toc_typ_to_c(( void*)({
struct Cyc_Core_Opt* _temp2051= e->topt; if( _temp2051 == 0){ _throw(
Null_Exception);} _temp2051->v;})),( struct Cyc_Absyn_Exp*) e, res, 0))->r));}}}}}}
static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e, int
exhaust, struct Cyc_Absyn_Stmt* s); static struct _tuple17 Cyc_Toc_decls_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_List_List* ds, int top); struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler( int n){ return Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__npop_handler_qv, 0),({ struct Cyc_List_List*
_temp2052=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp2052[ 0]=({ struct Cyc_List_List _temp2053; _temp2053.hd=( void*) Cyc_Absyn_uint_exp((
unsigned int)( n - 1), 0); _temp2053.tl= 0; _temp2053;}); _temp2052;}), 0), 0);}
void Cyc_Toc_do_npop_before( int n, struct Cyc_Absyn_Stmt* s){ if( n > 0){( void*)(
s->r=( void*)(( void*)( Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( n), Cyc_Absyn_new_stmt((
void*) s->r, 0), 0))->r));}} static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Stmt* s){ while( 1) { void* _temp2054=( void*) s->r; struct
Cyc_Absyn_Exp* _temp2092; struct Cyc_Absyn_Stmt* _temp2094; struct Cyc_Absyn_Stmt*
_temp2096; struct Cyc_Absyn_Exp* _temp2098; struct Cyc_Absyn_Stmt* _temp2100;
struct Cyc_Absyn_Stmt* _temp2102; struct Cyc_Absyn_Exp* _temp2104; struct Cyc_Absyn_Stmt*
_temp2106; struct _tuple3 _temp2108; struct Cyc_Absyn_Stmt* _temp2110; struct
Cyc_Absyn_Exp* _temp2112; struct Cyc_Absyn_Stmt* _temp2114; struct Cyc_Absyn_Stmt*
_temp2116; struct Cyc_Absyn_Stmt* _temp2118; struct _tagged_string* _temp2120;
struct Cyc_Absyn_Stmt* _temp2122; struct _tuple3 _temp2124; struct Cyc_Absyn_Stmt*
_temp2126; struct Cyc_Absyn_Exp* _temp2128; struct _tuple3 _temp2130; struct Cyc_Absyn_Stmt*
_temp2132; struct Cyc_Absyn_Exp* _temp2134; struct Cyc_Absyn_Exp* _temp2136;
struct Cyc_List_List* _temp2138; struct Cyc_Absyn_Exp* _temp2140; struct Cyc_Absyn_Switch_clause**
_temp2142; struct Cyc_List_List* _temp2144; struct Cyc_Absyn_Stmt* _temp2146;
struct Cyc_Absyn_Decl* _temp2148; struct Cyc_Absyn_Stmt* _temp2150; struct Cyc_Absyn_Stmt*
_temp2152; struct Cyc_Absyn_Stmt* _temp2154; struct _tagged_string* _temp2156;
struct _tuple3 _temp2158; struct Cyc_Absyn_Stmt* _temp2160; struct Cyc_Absyn_Exp*
_temp2162; struct Cyc_Absyn_Stmt* _temp2164; struct Cyc_List_List* _temp2166;
struct Cyc_Absyn_Stmt* _temp2168; _LL2056: if(( int) _temp2054 == Cyc_Absyn_Skip_s){
goto _LL2057;} else{ goto _LL2058;} _LL2058: if(( unsigned int) _temp2054 > 1u?((
struct _tunion_struct*) _temp2054)->tag == Cyc_Absyn_Exp_s_tag: 0){ _LL2093:
_temp2092=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*) _temp2054)->f1;
goto _LL2059;} else{ goto _LL2060;} _LL2060: if(( unsigned int) _temp2054 > 1u?((
struct _tunion_struct*) _temp2054)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL2097:
_temp2096=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp2054)->f1;
goto _LL2095; _LL2095: _temp2094=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp2054)->f2; goto _LL2061;} else{ goto _LL2062;} _LL2062: if(( unsigned int)
_temp2054 > 1u?(( struct _tunion_struct*) _temp2054)->tag == Cyc_Absyn_Return_s_tag:
0){ _LL2099: _temp2098=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp2054)->f1; goto _LL2063;} else{ goto _LL2064;} _LL2064: if(( unsigned int)
_temp2054 > 1u?(( struct _tunion_struct*) _temp2054)->tag == Cyc_Absyn_IfThenElse_s_tag:
0){ _LL2105: _temp2104=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2054)->f1; goto _LL2103; _LL2103: _temp2102=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp2054)->f2; goto _LL2101; _LL2101:
_temp2100=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2054)->f3; goto _LL2065;} else{ goto _LL2066;} _LL2066: if(( unsigned int)
_temp2054 > 1u?(( struct _tunion_struct*) _temp2054)->tag == Cyc_Absyn_While_s_tag:
0){ _LL2109: _temp2108=( struct _tuple3)(( struct Cyc_Absyn_While_s_struct*)
_temp2054)->f1; _LL2113: _temp2112= _temp2108.f1; goto _LL2111; _LL2111:
_temp2110= _temp2108.f2; goto _LL2107; _LL2107: _temp2106=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_While_s_struct*) _temp2054)->f2; goto _LL2067;} else{ goto
_LL2068;} _LL2068: if(( unsigned int) _temp2054 > 1u?(( struct _tunion_struct*)
_temp2054)->tag == Cyc_Absyn_Break_s_tag: 0){ _LL2115: _temp2114=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Break_s_struct*) _temp2054)->f1; goto _LL2069;} else{ goto
_LL2070;} _LL2070: if(( unsigned int) _temp2054 > 1u?(( struct _tunion_struct*)
_temp2054)->tag == Cyc_Absyn_Continue_s_tag: 0){ _LL2117: _temp2116=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Continue_s_struct*) _temp2054)->f1; goto _LL2071;} else{ goto
_LL2072;} _LL2072: if(( unsigned int) _temp2054 > 1u?(( struct _tunion_struct*)
_temp2054)->tag == Cyc_Absyn_Goto_s_tag: 0){ _LL2121: _temp2120=( struct
_tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp2054)->f1; goto _LL2119;
_LL2119: _temp2118=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp2054)->f2; goto _LL2073;} else{ goto _LL2074;} _LL2074: if(( unsigned int)
_temp2054 > 1u?(( struct _tunion_struct*) _temp2054)->tag == Cyc_Absyn_For_s_tag:
0){ _LL2137: _temp2136=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp2054)->f1; goto _LL2131; _LL2131: _temp2130=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp2054)->f2; _LL2135: _temp2134= _temp2130.f1; goto _LL2133; _LL2133:
_temp2132= _temp2130.f2; goto _LL2125; _LL2125: _temp2124=( struct _tuple3)((
struct Cyc_Absyn_For_s_struct*) _temp2054)->f3; _LL2129: _temp2128= _temp2124.f1;
goto _LL2127; _LL2127: _temp2126= _temp2124.f2; goto _LL2123; _LL2123: _temp2122=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp2054)->f4; goto
_LL2075;} else{ goto _LL2076;} _LL2076: if(( unsigned int) _temp2054 > 1u?((
struct _tunion_struct*) _temp2054)->tag == Cyc_Absyn_Switch_s_tag: 0){ _LL2141:
_temp2140=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp2054)->f1; goto _LL2139; _LL2139: _temp2138=( struct Cyc_List_List*)((
struct Cyc_Absyn_Switch_s_struct*) _temp2054)->f2; goto _LL2077;} else{ goto
_LL2078;} _LL2078: if(( unsigned int) _temp2054 > 1u?(( struct _tunion_struct*)
_temp2054)->tag == Cyc_Absyn_Fallthru_s_tag: 0){ _LL2145: _temp2144=( struct Cyc_List_List*)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp2054)->f1; goto _LL2143; _LL2143:
_temp2142=( struct Cyc_Absyn_Switch_clause**)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp2054)->f2; goto _LL2079;} else{ goto _LL2080;} _LL2080: if(( unsigned int)
_temp2054 > 1u?(( struct _tunion_struct*) _temp2054)->tag == Cyc_Absyn_Decl_s_tag:
0){ _LL2149: _temp2148=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp2054)->f1; goto _LL2147; _LL2147: _temp2146=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Decl_s_struct*) _temp2054)->f2; goto _LL2081;} else{ goto
_LL2082;} _LL2082: if(( unsigned int) _temp2054 > 1u?(( struct _tunion_struct*)
_temp2054)->tag == Cyc_Absyn_Cut_s_tag: 0){ _LL2151: _temp2150=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Cut_s_struct*) _temp2054)->f1; goto _LL2083;} else{ goto
_LL2084;} _LL2084: if(( unsigned int) _temp2054 > 1u?(( struct _tunion_struct*)
_temp2054)->tag == Cyc_Absyn_Splice_s_tag: 0){ _LL2153: _temp2152=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Splice_s_struct*) _temp2054)->f1; goto _LL2085;} else{ goto
_LL2086;} _LL2086: if(( unsigned int) _temp2054 > 1u?(( struct _tunion_struct*)
_temp2054)->tag == Cyc_Absyn_Label_s_tag: 0){ _LL2157: _temp2156=( struct
_tagged_string*)(( struct Cyc_Absyn_Label_s_struct*) _temp2054)->f1; goto
_LL2155; _LL2155: _temp2154=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp2054)->f2; goto _LL2087;} else{ goto _LL2088;} _LL2088: if(( unsigned int)
_temp2054 > 1u?(( struct _tunion_struct*) _temp2054)->tag == Cyc_Absyn_Do_s_tag:
0){ _LL2165: _temp2164=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*)
_temp2054)->f1; goto _LL2159; _LL2159: _temp2158=( struct _tuple3)(( struct Cyc_Absyn_Do_s_struct*)
_temp2054)->f2; _LL2163: _temp2162= _temp2158.f1; goto _LL2161; _LL2161:
_temp2160= _temp2158.f2; goto _LL2089;} else{ goto _LL2090;} _LL2090: if((
unsigned int) _temp2054 > 1u?(( struct _tunion_struct*) _temp2054)->tag == Cyc_Absyn_TryCatch_s_tag:
0){ _LL2169: _temp2168=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp2054)->f1; goto _LL2167; _LL2167: _temp2166=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp2054)->f2; goto _LL2091;} else{ goto
_LL2055;} _LL2057: return; _LL2059: Cyc_Toc_exp_to_c( nv, _temp2092); return;
_LL2061: Cyc_Toc_stmt_to_c( nv, _temp2096); s= _temp2094; continue; _LL2063: {
struct Cyc_Core_Opt* topt= 0; if( _temp2098 != 0){ topt=({ struct Cyc_Core_Opt*
_temp2170=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2170->v=( void*) Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Core_Opt* _temp2172=({
struct Cyc_Absyn_Exp* _temp2171= _temp2098; if( _temp2171 == 0){ _throw(
Null_Exception);} _temp2171->topt;}); if( _temp2172 == 0){ _throw(
Null_Exception);} _temp2172->v;})); _temp2170;}); Cyc_Toc_exp_to_c( nv,( struct
Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2173= _temp2098; if( _temp2173 == 0){
_throw( Null_Exception);} _temp2173;}));} if( s->try_depth > 0){ if( topt != 0){
struct _tuple1* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt* retn_stmt= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_Absyn_var_exp( x, 0), 0);( void*)( s->r=( void*)((
void*)( Cyc_Absyn_declare_stmt( x,( void*)({ struct Cyc_Core_Opt* _temp2174=
topt; if( _temp2174 == 0){ _throw( Null_Exception);} _temp2174->v;}), _temp2098,
Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( s->try_depth), retn_stmt, 0), 0))->r));}
else{ Cyc_Toc_do_npop_before( s->try_depth, s);}} return;} _LL2065: Cyc_Toc_exp_to_c(
nv, _temp2104); Cyc_Toc_stmt_to_c( nv, _temp2102); s= _temp2100; continue;
_LL2067: Cyc_Toc_exp_to_c( nv, _temp2112); Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env(
nv), _temp2106); return; _LL2069: if( nv->break_lab != 0){( void*)( s->r=( void*)((
void*)( Cyc_Absyn_goto_stmt(( struct _tagged_string*)({ struct Cyc_Core_Opt*
_temp2175= nv->break_lab; if( _temp2175 == 0){ _throw( Null_Exception);}
_temp2175->v;}), 0))->r));}{ int dest_depth= _temp2114 == 0? 0:({ struct Cyc_Absyn_Stmt*
_temp2176= _temp2114; if( _temp2176 == 0){ _throw( Null_Exception);} _temp2176->try_depth;});
Cyc_Toc_do_npop_before( s->try_depth - dest_depth, s); return;} _LL2071: if( nv->continue_lab
!= 0){( void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct
_tagged_string*)({ struct Cyc_Core_Opt* _temp2177= nv->continue_lab; if(
_temp2177 == 0){ _throw( Null_Exception);} _temp2177->v;}), 0))->r));} Cyc_Toc_do_npop_before(
s->try_depth -({ struct Cyc_Absyn_Stmt* _temp2178= _temp2116; if( _temp2178 == 0){
_throw( Null_Exception);} _temp2178->try_depth;}), s); return; _LL2073: Cyc_Toc_do_npop_before(
s->try_depth -({ struct Cyc_Absyn_Stmt* _temp2179= _temp2118; if( _temp2179 == 0){
_throw( Null_Exception);} _temp2179->try_depth;}), s); return; _LL2075: Cyc_Toc_exp_to_c(
nv, _temp2136); Cyc_Toc_exp_to_c( nv, _temp2134); Cyc_Toc_exp_to_c( nv,
_temp2128); Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2122); return;
_LL2077: Cyc_Toc_xlate_switch( nv, s, _temp2140, _temp2138); return; _LL2079:
if( nv->fallthru_info == 0){( void) _throw((( struct _xtunion_struct*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp2180=(
char*)"fallthru in unexpected place"; struct _tagged_string _temp2181; _temp2181.curr=
_temp2180; _temp2181.base= _temp2180; _temp2181.last_plus_one= _temp2180 + 29;
_temp2181;})));}{ struct _tuple8 _temp2185; struct Cyc_Dict_Dict* _temp2186;
struct Cyc_List_List* _temp2188; struct _tagged_string* _temp2190; struct
_tuple8* _temp2183=( struct _tuple8*)({ struct Cyc_Core_Opt* _temp2182= nv->fallthru_info;
if( _temp2182 == 0){ _throw( Null_Exception);} _temp2182->v;}); _temp2185=*
_temp2183; _LL2191: _temp2190= _temp2185.f1; goto _LL2189; _LL2189: _temp2188=
_temp2185.f2; goto _LL2187; _LL2187: _temp2186= _temp2185.f3; goto _LL2184;
_LL2184: { struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_goto_stmt( _temp2190, 0); Cyc_Toc_do_npop_before(
s->try_depth -(({ struct Cyc_Absyn_Switch_clause** _temp2192= _temp2142; if(
_temp2192 == 0){ _throw( Null_Exception);}* _temp2192;})->body)->try_depth, s2);{
struct Cyc_List_List* vs2=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_rev)( _temp2188); struct Cyc_List_List* es2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( _temp2144); for( 0; vs2 != 0; vs2=({
struct Cyc_List_List* _temp2193= vs2; if( _temp2193 == 0){ _throw(
Null_Exception);} _temp2193->tl;}), es2=({ struct Cyc_List_List* _temp2194= es2;
if( _temp2194 == 0){ _throw( Null_Exception);} _temp2194->tl;})){ Cyc_Toc_exp_to_c(
nv,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp2195= es2; if(
_temp2195 == 0){ _throw( Null_Exception);} _temp2195->hd;})); s2= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt((( struct Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict* d,
struct _tuple1* key)) Cyc_Dict_lookup)( _temp2186,( struct _tuple1*)({ struct
Cyc_List_List* _temp2196= vs2; if( _temp2196 == 0){ _throw( Null_Exception);}
_temp2196->hd;})),( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp2197=
es2; if( _temp2197 == 0){ _throw( Null_Exception);} _temp2197->hd;}), 0), s2, 0);}(
void*)( s->r=( void*)(( void*) s2->r)); return;}}} _LL2081:{ void* _temp2198=(
void*) _temp2148->r; int _temp2204; struct Cyc_Absyn_Exp* _temp2206; struct Cyc_Core_Opt*
_temp2208; struct Cyc_Core_Opt* _temp2210; struct Cyc_Absyn_Pat* _temp2212;
_LL2200: if((( struct _tunion_struct*) _temp2198)->tag == Cyc_Absyn_Let_d_tag){
_LL2213: _temp2212=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*)
_temp2198)->f1; goto _LL2211; _LL2211: _temp2210=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Let_d_struct*) _temp2198)->f2; goto _LL2209; _LL2209: _temp2208=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp2198)->f3; goto
_LL2207; _LL2207: _temp2206=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp2198)->f4; goto _LL2205; _LL2205: _temp2204=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp2198)->f5; goto _LL2201;} else{ goto _LL2202;} _LL2202: goto _LL2203;
_LL2201: { int var_case= 0;{ void* _temp2214=( void*) _temp2212->r; struct Cyc_Absyn_Vardecl*
_temp2220; _LL2216: if(( unsigned int) _temp2214 > 2u?(( struct _tunion_struct*)
_temp2214)->tag == Cyc_Absyn_Var_p_tag: 0){ _LL2221: _temp2220=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_p_struct*) _temp2214)->f1; goto _LL2217;} else{ goto
_LL2218;} _LL2218: goto _LL2219; _LL2217: var_case= 1; _temp2220->initializer=(
struct Cyc_Absyn_Exp*) _temp2206;( void*)( _temp2148->r=( void*)(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2222=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2222[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2223; _temp2223.tag= Cyc_Absyn_Var_d_tag; _temp2223.f1= _temp2220;
_temp2223;}); _temp2222;}))); goto _LL2215; _LL2219: goto _LL2215; _LL2215:;}
if( var_case){ goto _LL2203;}( void*)( s->r=( void*)(( void*)( Cyc_Toc_letdecl_to_c(
nv, _temp2212, _temp2208, _temp2206, _temp2204, _temp2146))->r)); goto _LL2199;}
_LL2203: { struct Cyc_List_List* _temp2227; struct Cyc_Toc_Env* _temp2229;
struct _tuple17 _temp2225= Cyc_Toc_decls_to_c( nv,({ struct Cyc_List_List*
_temp2224=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2224->hd=( void*) _temp2148; _temp2224->tl= 0; _temp2224;}), 0); _LL2230:
_temp2229= _temp2225.f1; goto _LL2228; _LL2228: _temp2227= _temp2225.f2; goto
_LL2226; _LL2226: Cyc_Toc_stmt_to_c( _temp2229, _temp2146); for( 0; _temp2227 !=
0; _temp2227=({ struct Cyc_List_List* _temp2231= _temp2227; if( _temp2231 == 0){
_throw( Null_Exception);} _temp2231->tl;})){ _temp2146= Cyc_Absyn_decl_stmt((
struct Cyc_Absyn_Decl*)({ struct Cyc_List_List* _temp2232= _temp2227; if(
_temp2232 == 0){ _throw( Null_Exception);} _temp2232->hd;}), _temp2146, 0);}(
void*)( s->r=( void*)(( void*) _temp2146->r)); goto _LL2199;} _LL2199:;} return;
_LL2083:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(( struct
_tagged_string)({ char* _temp2233=( char*)"cut"; struct _tagged_string _temp2234;
_temp2234.curr= _temp2233; _temp2234.base= _temp2233; _temp2234.last_plus_one=
_temp2233 + 4; _temp2234;})); return; _LL2085:(( void(*)( struct _tagged_string
s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char* _temp2235=( char*)"splice";
struct _tagged_string _temp2236; _temp2236.curr= _temp2235; _temp2236.base=
_temp2235; _temp2236.last_plus_one= _temp2235 + 7; _temp2236;})); return;
_LL2087: s= _temp2154; continue; _LL2089: Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env(
nv), _temp2164); Cyc_Toc_exp_to_c( nv, _temp2162); return; _LL2091: { struct
_tuple1* h_var= Cyc_Toc_temp_var(); struct _tuple1* e_var= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* h_exp= Cyc_Absyn_var_exp( h_var, 0); struct Cyc_Absyn_Exp*
e_exp= Cyc_Absyn_var_exp( e_var, 0); void* h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp);
void* e_typ= Cyc_Toc_typ_to_c( Cyc_Absyn_exn_typ); struct Cyc_Absyn_Exp*
pushhandler_exp= Cyc_Absyn_var_exp(({ struct _tuple1* _temp2237=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp2237->f1= Cyc_Toc_abs_ns; _temp2237->f2=
Cyc_Toc__push_handler_sp; _temp2237;}), 0); struct Cyc_Absyn_Exp* setjmp_exp=
Cyc_Absyn_var_exp(({ struct _tuple1* _temp2238=( struct _tuple1*) GC_malloc(
sizeof( struct _tuple1)); _temp2238->f1= Cyc_Toc_abs_ns; _temp2238->f2= Cyc_Toc_setjmp_sp;
_temp2238;}), 0); struct Cyc_Absyn_Exp* pophandler_exp= Cyc_Absyn_var_exp(({
struct _tuple1* _temp2239=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp2239->f1= Cyc_Toc_abs_ns; _temp2239->f2= Cyc_Toc__pop_handler_sp; _temp2239;}),
0); e_exp->topt=({ struct Cyc_Core_Opt* _temp2240=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2240->v=( void*) e_typ; _temp2240;});
nv= Cyc_Toc_add_varmap( nv, e_var, e_exp); Cyc_Toc_stmt_to_c( nv, _temp2168);{
struct Cyc_Absyn_Stmt* tryandpop_stmt= Cyc_Absyn_seq_stmt( _temp2168, Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp( pophandler_exp, 0, 0), 0), 0); struct _tuple1* x= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp( x, 0); struct Cyc_Absyn_Vardecl*
x_vd= Cyc_Absyn_new_vardecl( x, Cyc_Absyn_exn_typ, 0); xexp->topt=({ struct Cyc_Core_Opt*
_temp2241=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2241->v=( void*) Cyc_Absyn_exn_typ; _temp2241;});{ struct Cyc_Absyn_Pat* p=({
struct Cyc_Absyn_Pat* _temp2242=( struct Cyc_Absyn_Pat*) GC_malloc( sizeof(
struct Cyc_Absyn_Pat)); _temp2242->r=( void*)(( void*)({ struct Cyc_Absyn_Var_p_struct*
_temp2244=( struct Cyc_Absyn_Var_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct));
_temp2244[ 0]=({ struct Cyc_Absyn_Var_p_struct _temp2245; _temp2245.tag= Cyc_Absyn_Var_p_tag;
_temp2245.f1= x_vd; _temp2245;}); _temp2244;})); _temp2242->topt=({ struct Cyc_Core_Opt*
_temp2243=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2243->v=( void*) Cyc_Absyn_exn_typ; _temp2243;}); _temp2242->loc= 0;
_temp2242;}); struct Cyc_Absyn_Exp* te= Cyc_Absyn_throw_exp( xexp, 0); te->topt=({
struct Cyc_Core_Opt* _temp2246=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2246->v=( void*)(( void*) Cyc_Absyn_VoidType); _temp2246;});{
struct Cyc_Absyn_Stmt* rs= Cyc_Absyn_exp_stmt( te, 0); struct Cyc_Absyn_Switch_clause*
default_case=({ struct Cyc_Absyn_Switch_clause* _temp2247=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2247->pattern= p;
_temp2247->pat_vars=({ struct Cyc_Core_Opt* _temp2248=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2248->v=( void*)({ struct Cyc_List_List*
_temp2249=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2249->hd=( void*) x_vd; _temp2249->tl= 0; _temp2249;}); _temp2248;});
_temp2247->where_clause= 0; _temp2247->body= rs; _temp2247->loc= 0; _temp2247;});
struct Cyc_Absyn_Stmt* handler_stmt= Cyc_Absyn_switch_stmt( e_exp,(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp2166,({
struct Cyc_List_List* _temp2250=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2250->hd=( void*) default_case; _temp2250->tl= 0;
_temp2250;})), 0); Cyc_Toc_stmt_to_c( nv, handler_stmt);{ struct Cyc_Absyn_Exp*
setjmp_call= Cyc_Absyn_cast_exp( e_typ, Cyc_Absyn_fncall_exp( setjmp_exp,({
struct Cyc_List_List* _temp2251=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2251->hd=( void*) Cyc_Absyn_structmember_exp( h_exp,
Cyc_Toc_handler_sp, 0); _temp2251->tl= 0; _temp2251;}), 0), 0);( void*)( s->r=(
void*)(( void*)( Cyc_Absyn_declare_stmt( h_var, h_typ, 0, Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( pushhandler_exp,({ struct Cyc_List_List*
_temp2252=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2252->hd=( void*) Cyc_Absyn_address_exp( h_exp, 0); _temp2252->tl= 0;
_temp2252;}), 0), 0), Cyc_Absyn_declare_stmt( e_var, e_typ,( struct Cyc_Absyn_Exp*)
setjmp_call, Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_prim1_exp(( void*) Cyc_Absyn_Not,
e_exp, 0), tryandpop_stmt, handler_stmt, 0), 0), 0), 0))->r)); return;}}}}}
_LL2055:;}} static void Cyc_Toc_fndecl_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Fndecl*
f){ f->tvs= 0; f->effect= 0;( void*)( f->ret_type=( void*) Cyc_Toc_typ_to_c((
void*) f->ret_type));{ struct Cyc_List_List* args= f->args; for( 0; args != 0;
args=({ struct Cyc_List_List* _temp2253= args; if( _temp2253 == 0){ _throw(
Null_Exception);} _temp2253->tl;})){(*(( struct _tuple18*)({ struct Cyc_List_List*
_temp2254= args; if( _temp2254 == 0){ _throw( Null_Exception);} _temp2254->hd;}))).f3=
Cyc_Toc_typ_to_c((*(( struct _tuple18*)({ struct Cyc_List_List* _temp2255= args;
if( _temp2255 == 0){ _throw( Null_Exception);} _temp2255->hd;}))).f3);{ struct
_tuple1* x=({ struct _tuple1* _temp2256=( struct _tuple1*) GC_malloc( sizeof(
struct _tuple1)); _temp2256->f1=( void*) Cyc_Absyn_Loc_n; _temp2256->f2=(*((
struct _tuple18*)({ struct Cyc_List_List* _temp2257= args; if( _temp2257 == 0){
_throw( Null_Exception);} _temp2257->hd;}))).f1; _temp2256;}); nv= Cyc_Toc_add_varmap(
nv, x, Cyc_Absyn_var_exp( x, 0));}}} Cyc_Toc_stmt_to_c( Cyc_Toc_clear_toplevel(
nv), f->body);} static void* Cyc_Toc_scope_to_c( void* s){ void* _temp2258= s;
_LL2260: if(( int) _temp2258 == Cyc_Absyn_Abstract){ goto _LL2261;} else{ goto
_LL2262;} _LL2262: if(( int) _temp2258 == Cyc_Absyn_ExternC){ goto _LL2263;}
else{ goto _LL2264;} _LL2264: goto _LL2265; _LL2261: return( void*) Cyc_Absyn_Public;
_LL2263: return( void*) Cyc_Absyn_Extern; _LL2265: return s; _LL2259:;} struct
Cyc_Core_Opt* Cyc_Toc_structs_so_far= 0; static void Cyc_Toc_structdecl_to_c(
struct Cyc_Absyn_Structdecl* s){ struct _tuple1* n=( struct _tuple1*)({ struct
Cyc_Core_Opt* _temp2266= s->name; if( _temp2266 == 0){ _throw( Null_Exception);}
_temp2266->v;}); if( Cyc_Toc_structs_so_far == 0){ Cyc_Toc_structs_so_far=({
struct Cyc_Core_Opt* _temp2267=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2267->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp2267;});}{ int seen_defn_before; struct Cyc_Core_Opt* dopt=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp2268= Cyc_Toc_structs_so_far;
if( _temp2268 == 0){ _throw( Null_Exception);} _temp2268->v;}),(* n).f2); if(
dopt == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp2269= Cyc_Toc_structs_so_far; if( _temp2269 == 0){ _throw( Null_Exception);}
_temp2269->v=( void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key, struct Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp2270= Cyc_Toc_structs_so_far;
if( _temp2270 == 0){ _throw( Null_Exception);} _temp2270->v;}),(* n).f2, s);});}
else{ if((( struct Cyc_Absyn_Structdecl*)({ struct Cyc_Core_Opt* _temp2271= dopt;
if( _temp2271 == 0){ _throw( Null_Exception);} _temp2271->v;}))->fields == 0){(
struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp2272= Cyc_Toc_structs_so_far;
if( _temp2272 == 0){ _throw( Null_Exception);} _temp2272->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct
Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)({ struct
Cyc_Core_Opt* _temp2273= Cyc_Toc_structs_so_far; if( _temp2273 == 0){ _throw(
Null_Exception);} _temp2273->v;}),(* n).f2, s);}); seen_defn_before= 0;} else{
seen_defn_before= 1;}}( void*)( s->sc=( void*)(((( void*) s->sc ==( void*) Cyc_Absyn_Extern?
1:( void*) s->sc ==( void*) Cyc_Absyn_Static)? 1:( void*) s->sc ==( void*) Cyc_Absyn_ExternC)?(
void*) Cyc_Absyn_Public: Cyc_Toc_scope_to_c(( void*) s->sc))); s->tvs= 0; s->name=({
struct Cyc_Core_Opt* _temp2274=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2274->v=( void*) n; _temp2274;}); if( s->fields != 0){ if(
seen_defn_before){ s->fields= 0;} else{ struct Cyc_List_List* fields=( struct
Cyc_List_List*)({ struct Cyc_Core_Opt* _temp2275= s->fields; if( _temp2275 == 0){
_throw( Null_Exception);} _temp2275->v;}); for( 0; fields != 0; fields=({ struct
Cyc_List_List* _temp2276= fields; if( _temp2276 == 0){ _throw( Null_Exception);}
_temp2276->tl;})){( void*)((( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp2277= fields; if( _temp2277 == 0){ _throw( Null_Exception);} _temp2277->hd;}))->type=(
void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp2278= fields; if( _temp2278 == 0){ _throw( Null_Exception);}
_temp2278->hd;}))->type));}}}}} struct Cyc_Core_Opt* Cyc_Toc_unions_so_far= 0;
static void Cyc_Toc_uniondecl_to_c( struct Cyc_Absyn_Uniondecl* u){ struct
_tuple1* n=( struct _tuple1*)({ struct Cyc_Core_Opt* _temp2279= u->name; if(
_temp2279 == 0){ _throw( Null_Exception);} _temp2279->v;}); if( Cyc_Toc_unions_so_far
== 0){ Cyc_Toc_unions_so_far=({ struct Cyc_Core_Opt* _temp2280=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2280->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); _temp2280;});}{ int seen_defn_before; struct Cyc_Core_Opt*
dopt=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string*
key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp2281= Cyc_Toc_unions_so_far; if( _temp2281 == 0){ _throw( Null_Exception);}
_temp2281->v;}),(* n).f2); if( dopt == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)({
struct Cyc_Core_Opt* _temp2282= Cyc_Toc_unions_so_far; if( _temp2282 == 0){
_throw( Null_Exception);} _temp2282->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp2283= Cyc_Toc_unions_so_far; if( _temp2283 == 0){ _throw( Null_Exception);}
_temp2283->v;}),(* n).f2, u);});} else{ if((( struct Cyc_Absyn_Uniondecl*)({
struct Cyc_Core_Opt* _temp2284= dopt; if( _temp2284 == 0){ _throw(
Null_Exception);} _temp2284->v;}))->fields == 0){( struct Cyc_Dict_Dict*)({
struct Cyc_Core_Opt* _temp2285= Cyc_Toc_unions_so_far; if( _temp2285 == 0){
_throw( Null_Exception);} _temp2285->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp2286= Cyc_Toc_unions_so_far; if( _temp2286 == 0){ _throw( Null_Exception);}
_temp2286->v;}),(* n).f2, u);}); seen_defn_before= 0;} else{ seen_defn_before= 1;}}(
void*)( u->sc=( void*)(((( void*) u->sc ==( void*) Cyc_Absyn_Extern? 1:( void*)
u->sc ==( void*) Cyc_Absyn_Static)? 1:( void*) u->sc ==( void*) Cyc_Absyn_ExternC)?(
void*) Cyc_Absyn_Public: Cyc_Toc_scope_to_c(( void*) u->sc))); u->tvs= 0; u->name=({
struct Cyc_Core_Opt* _temp2287=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2287->v=( void*) n; _temp2287;}); if( u->fields != 0){ if(
seen_defn_before){ u->fields= 0;} else{ struct Cyc_List_List* fields=( struct
Cyc_List_List*)({ struct Cyc_Core_Opt* _temp2288= u->fields; if( _temp2288 == 0){
_throw( Null_Exception);} _temp2288->v;}); for( 0; fields != 0; fields=({ struct
Cyc_List_List* _temp2289= fields; if( _temp2289 == 0){ _throw( Null_Exception);}
_temp2289->tl;})){( void*)((( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp2290= fields; if( _temp2290 == 0){ _throw( Null_Exception);} _temp2290->hd;}))->type=(
void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp2291= fields; if( _temp2291 == 0){ _throw( Null_Exception);}
_temp2291->hd;}))->type));}}}}} static struct Cyc_Core_Opt* Cyc_Toc_tunions_so_far=
0; static struct Cyc_List_List* Cyc_Toc_tuniondecl_to_c( struct Cyc_Absyn_Tuniondecl*
ed, struct Cyc_List_List* res){ struct _tuple1* n=( struct _tuple1*)({ struct
Cyc_Core_Opt* _temp2292= ed->name; if( _temp2292 == 0){ _throw( Null_Exception);}
_temp2292->v;}); if( Cyc_Toc_tunions_so_far == 0){ Cyc_Toc_tunions_so_far=({
struct Cyc_Core_Opt* _temp2293=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2293->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp2293;});}{ int seen_before; int seen_defn_before; struct Cyc_Core_Opt* dopt=((
struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key))
Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp2294=
Cyc_Toc_tunions_so_far; if( _temp2294 == 0){ _throw( Null_Exception);} _temp2294->v;}),(*
n).f2); if( dopt == 0){ seen_before= 0; seen_defn_before= 0;( struct Cyc_Dict_Dict*)({
struct Cyc_Core_Opt* _temp2295= Cyc_Toc_tunions_so_far; if( _temp2295 == 0){
_throw( Null_Exception);} _temp2295->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Tuniondecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp2296= Cyc_Toc_tunions_so_far; if( _temp2296 == 0){ _throw( Null_Exception);}
_temp2296->v;}),(* n).f2, ed);});} else{ seen_before= 1; if((( struct Cyc_Absyn_Tuniondecl*)({
struct Cyc_Core_Opt* _temp2297= dopt; if( _temp2297 == 0){ _throw(
Null_Exception);} _temp2297->v;}))->fields == 0){ seen_defn_before= 0;( struct
Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp2298= Cyc_Toc_tunions_so_far; if(
_temp2298 == 0){ _throw( Null_Exception);} _temp2298->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Tuniondecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp2299= Cyc_Toc_tunions_so_far; if( _temp2299 == 0){ _throw( Null_Exception);}
_temp2299->v;}),(* n).f2, ed);});} else{ seen_defn_before= 1;}} if( !
seen_before){ struct Cyc_Absyn_Typedefdecl* tdef=({ struct Cyc_Absyn_Typedefdecl*
_temp2300=( struct Cyc_Absyn_Typedefdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Typedefdecl));
_temp2300->name= n; _temp2300->tvs= 0; _temp2300->defn=( void*) Cyc_Absyn_void_star_typ();
_temp2300;}); res=({ struct Cyc_List_List* _temp2301=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2301->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Typedef_d_struct* _temp2302=( struct Cyc_Absyn_Typedef_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct)); _temp2302[ 0]=({ struct
Cyc_Absyn_Typedef_d_struct _temp2303; _temp2303.tag= Cyc_Absyn_Typedef_d_tag;
_temp2303.f1= tdef; _temp2303;}); _temp2302;}), 0); _temp2301->tl= res;
_temp2301;});} if( ! seen_defn_before){ int tag_count= 0; int box_count= 0;
struct Cyc_List_List* fields= ed->fields == 0? 0:( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp2304= ed->fields; if( _temp2304 == 0){ _throw(
Null_Exception);} _temp2304->v;}); for( 0; fields != 0; fields=({ struct Cyc_List_List*
_temp2305= fields; if( _temp2305 == 0){ _throw( Null_Exception);} _temp2305->tl;})){
struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List*
_temp2306= fields; if( _temp2306 == 0){ _throw( Null_Exception);} _temp2306->hd;});
if( f->typs == 0){ struct Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp((
tag_count ++), 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name,
Cyc_Absyn_uint_t, eopt); vd->tq=({ struct Cyc_Absyn_Tqual* _temp2307=( struct
Cyc_Absyn_Tqual*) GC_malloc_atomic( sizeof( struct Cyc_Absyn_Tqual)); _temp2307->q_const=
1; _temp2307->q_volatile= 0; _temp2307->q_restrict= 0; _temp2307;}); res=({
struct Cyc_List_List* _temp2308=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2308->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2309=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2309[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2310; _temp2310.tag= Cyc_Absyn_Var_d_tag; _temp2310.f1= vd; _temp2310;});
_temp2309;}), 0); _temp2308->tl= res; _temp2308;});} else{ struct Cyc_Absyn_Exp*
eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp(( box_count ++), 0);
struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( Cyc_Toc_collapse_qvar_tag(
f->name,( struct _tagged_string)({ char* _temp2311=( char*)"_tag"; struct
_tagged_string _temp2312; _temp2312.curr= _temp2311; _temp2312.base= _temp2311;
_temp2312.last_plus_one= _temp2311 + 5; _temp2312;})), Cyc_Absyn_sint_t, eopt);
vd->tq=({ struct Cyc_Absyn_Tqual* _temp2313=( struct Cyc_Absyn_Tqual*)
GC_malloc_atomic( sizeof( struct Cyc_Absyn_Tqual)); _temp2313->q_const= 1;
_temp2313->q_volatile= 0; _temp2313->q_restrict= 0; _temp2313;}); res=({ struct
Cyc_List_List* _temp2314=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2314->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp2315=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2315[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2316; _temp2316.tag= Cyc_Absyn_Var_d_tag;
_temp2316.f1= vd; _temp2316;}); _temp2315;}), 0); _temp2314->tl= res; _temp2314;});{
struct Cyc_List_List* fs= 0; int i= 1;{ struct Cyc_List_List* ts= f->typs; for(
0; ts != 0; ts=({ struct Cyc_List_List* _temp2317= ts; if( _temp2317 == 0){
_throw( Null_Exception);} _temp2317->tl;}), i ++){ struct _tagged_string* fname=(
struct _tagged_string*)({ struct _tagged_string* _temp2318=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp2318[ 0]=
xprintf("f%d", i); _temp2318;}); struct Cyc_Absyn_Structfield* f=({ struct Cyc_Absyn_Structfield*
_temp2319=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2319->name= fname; _temp2319->tq=(*(( struct _tuple5*)({ struct Cyc_List_List*
_temp2321= ts; if( _temp2321 == 0){ _throw( Null_Exception);} _temp2321->hd;}))).f1;
_temp2319->type=( void*) Cyc_Toc_typ_to_c_array((*(( struct _tuple5*)({ struct
Cyc_List_List* _temp2320= ts; if( _temp2320 == 0){ _throw( Null_Exception);}
_temp2320->hd;}))).f2); _temp2319->width= 0; _temp2319->attributes= 0; _temp2319;});
fs=({ struct Cyc_List_List* _temp2322=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2322->hd=( void*) f; _temp2322->tl= fs;
_temp2322;});}} fs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fs); fs=({ struct Cyc_List_List* _temp2323=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2323->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp2324=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2324->name= Cyc_Toc_tag_sp; _temp2324->tq= Cyc_Toc_mt_tq; _temp2324->type=(
void*) Cyc_Absyn_sint_t; _temp2324->width= 0; _temp2324->attributes= 0;
_temp2324;}); _temp2323->tl= fs; _temp2323;});{ struct Cyc_Absyn_Structdecl* sd=({
struct Cyc_Absyn_Structdecl* _temp2325=( struct Cyc_Absyn_Structdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp2325->sc=( void*)(( void*) Cyc_Absyn_Public);
_temp2325->name=({ struct Cyc_Core_Opt* _temp2327=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2327->v=( void*) Cyc_Toc_collapse_qvar_tag(
f->name,( struct _tagged_string)({ char* _temp2328=( char*)"_struct"; struct
_tagged_string _temp2329; _temp2329.curr= _temp2328; _temp2329.base= _temp2328;
_temp2329.last_plus_one= _temp2328 + 8; _temp2329;})); _temp2327;}); _temp2325->tvs=
0; _temp2325->fields=({ struct Cyc_Core_Opt* _temp2326=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2326->v=( void*) fs; _temp2326;});
_temp2325->attributes= 0; _temp2325;}); res=({ struct Cyc_List_List* _temp2330=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2330->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2331=(
struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp2331[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp2332; _temp2332.tag= Cyc_Absyn_Struct_d_tag;
_temp2332.f1= sd; _temp2332;}); _temp2331;}), 0); _temp2330->tl= res; _temp2330;});}}}}}
return res;}} static struct Cyc_Core_Opt* Cyc_Toc_xtunions_so_far= 0; static
struct Cyc_List_List* Cyc_Toc_xtuniondecl_to_c( struct Cyc_Absyn_XTuniondecl* xd,
struct Cyc_List_List* res){ struct _tuple1* n= xd->name; if( xd->fields == 0){
void* t= Cyc_Absyn_cstar_typ( Cyc_Absyn_strct( Cyc_Toc__xtunion_struct_sp), Cyc_Toc_mt_tq);
struct Cyc_Absyn_Typedefdecl* td=({ struct Cyc_Absyn_Typedefdecl* _temp2333=(
struct Cyc_Absyn_Typedefdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Typedefdecl));
_temp2333->name= n; _temp2333->tvs= 0; _temp2333->defn=( void*) t; _temp2333;});
res=({ struct Cyc_List_List* _temp2334=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2334->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Typedef_d_struct* _temp2335=( struct Cyc_Absyn_Typedef_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct)); _temp2335[ 0]=({ struct
Cyc_Absyn_Typedef_d_struct _temp2336; _temp2336.tag= Cyc_Absyn_Typedef_d_tag;
_temp2336.f1= td; _temp2336;}); _temp2335;}), 0); _temp2334->tl= res; _temp2334;});}
else{ if( Cyc_Toc_xtunions_so_far == 0){ Cyc_Toc_xtunions_so_far=({ struct Cyc_Core_Opt*
_temp2337=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2337->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp2337;});}{ struct Cyc_List_List* fs= xd->fields; for( 0; fs != 0; fs=({
struct Cyc_List_List* _temp2338= fs; if( _temp2338 == 0){ _throw( Null_Exception);}
_temp2338->tl;})){ struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp2339= fs; if( _temp2339 == 0){ _throw( Null_Exception);}
_temp2339->hd;}); struct _tagged_string* fn=(* f->name).f2; struct Cyc_Absyn_Exp*
sz_exp= Cyc_Absyn_uint_exp(({ struct _tagged_string _temp2340=* fn;(
unsigned int)( _temp2340.last_plus_one - _temp2340.curr);}), 0); struct Cyc_Absyn_ArrayType_struct*
tag_typ=({ struct Cyc_Absyn_ArrayType_struct* _temp2341=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp2341[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp2342; _temp2342.tag= Cyc_Absyn_ArrayType_tag;
_temp2342.f1=( void*) Cyc_Absyn_uchar_t; _temp2342.f2= Cyc_Toc_mt_tq; _temp2342.f3=(
struct Cyc_Absyn_Exp*) sz_exp; _temp2342;}); _temp2341;}); struct Cyc_Absyn_Exp*
initopt= 0; if(( void*) xd->sc !=( void*) Cyc_Absyn_Extern){ initopt=( struct
Cyc_Absyn_Exp*) Cyc_Absyn_string_exp(* fn, 0);}{ struct Cyc_Absyn_Vardecl*
tag_decl= Cyc_Absyn_new_vardecl( Cyc_Toc_collapse_qvar_tag( f->name,( struct
_tagged_string)({ char* _temp2343=( char*)"_tag"; struct _tagged_string
_temp2344; _temp2344.curr= _temp2343; _temp2344.base= _temp2343; _temp2344.last_plus_one=
_temp2343 + 5; _temp2344;})),( void*) tag_typ, initopt);( void*)( tag_decl->sc=(
void*)(( void*) xd->sc)); res=({ struct Cyc_List_List* _temp2345=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2345->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Var_d_struct* _temp2346=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2346[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2347; _temp2347.tag= Cyc_Absyn_Var_d_tag; _temp2347.f1= tag_decl; _temp2347;});
_temp2346;}), 0); _temp2345->tl= res; _temp2345;});{ int seen_before; int
seen_defn_before; struct Cyc_Core_Opt* dopt=(( struct Cyc_Core_Opt*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)({
struct Cyc_Core_Opt* _temp2348= Cyc_Toc_xtunions_so_far; if( _temp2348 == 0){
_throw( Null_Exception);} _temp2348->v;}), fn); if( dopt == 0){ seen_before= 0;
seen_defn_before= 0;( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp2349=
Cyc_Toc_xtunions_so_far; if( _temp2349 == 0){ _throw( Null_Exception);}
_temp2349->v=( void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key, struct Cyc_Absyn_XTuniondecl* data)) Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp2350= Cyc_Toc_xtunions_so_far;
if( _temp2350 == 0){ _throw( Null_Exception);} _temp2350->v;}), fn, xd);});}
else{ seen_before= 1; if((( struct Cyc_Absyn_XTuniondecl*)({ struct Cyc_Core_Opt*
_temp2351= dopt; if( _temp2351 == 0){ _throw( Null_Exception);} _temp2351->v;}))->fields
== 0){( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp2352= Cyc_Toc_xtunions_so_far;
if( _temp2352 == 0){ _throw( Null_Exception);} _temp2352->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct
Cyc_Absyn_XTuniondecl* data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)({ struct
Cyc_Core_Opt* _temp2353= Cyc_Toc_xtunions_so_far; if( _temp2353 == 0){ _throw(
Null_Exception);} _temp2353->v;}), fn, xd);}); seen_defn_before= 0;} else{
seen_defn_before= 1;}} if( ! seen_before){ struct Cyc_List_List* fields= 0; int
i= 1;{ struct Cyc_List_List* ts= f->typs; for( 0; ts != 0; ts=({ struct Cyc_List_List*
_temp2354= ts; if( _temp2354 == 0){ _throw( Null_Exception);} _temp2354->tl;}),
i ++){ struct _tagged_string* field_n=( struct _tagged_string*)({ struct
_tagged_string* _temp2355=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp2355[ 0]= xprintf("f%d", i); _temp2355;}); struct Cyc_Absyn_Structfield*
newf=({ struct Cyc_Absyn_Structfield* _temp2356=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2356->name= field_n;
_temp2356->tq=(*(( struct _tuple5*)({ struct Cyc_List_List* _temp2358= ts; if(
_temp2358 == 0){ _throw( Null_Exception);} _temp2358->hd;}))).f1; _temp2356->type=(
void*) Cyc_Toc_typ_to_c_array((*(( struct _tuple5*)({ struct Cyc_List_List*
_temp2357= ts; if( _temp2357 == 0){ _throw( Null_Exception);} _temp2357->hd;}))).f2);
_temp2356->width= 0; _temp2356->attributes= 0; _temp2356;}); fields=({ struct
Cyc_List_List* _temp2359=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2359->hd=( void*) newf; _temp2359->tl= fields; _temp2359;});}} fields=({
struct Cyc_List_List* _temp2360=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2360->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp2361=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2361->name= Cyc_Toc_tag_sp; _temp2361->tq= Cyc_Toc_mt_tq; _temp2361->type=(
void*) Cyc_Absyn_cstar_typ( Cyc_Absyn_uchar_t, Cyc_Toc_mt_tq); _temp2361->width=
0; _temp2361->attributes= 0; _temp2361;}); _temp2360->tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( fields); _temp2360;});{ struct Cyc_Absyn_Structdecl*
strct_decl=({ struct Cyc_Absyn_Structdecl* _temp2362=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp2362->sc=( void*)(( void*)
Cyc_Absyn_Public); _temp2362->name=({ struct Cyc_Core_Opt* _temp2364=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2364->v=( void*)
Cyc_Toc_collapse_qvar_tag( f->name,( struct _tagged_string)({ char* _temp2365=(
char*)"_struct"; struct _tagged_string _temp2366; _temp2366.curr= _temp2365;
_temp2366.base= _temp2365; _temp2366.last_plus_one= _temp2365 + 8; _temp2366;}));
_temp2364;}); _temp2362->tvs= 0; _temp2362->fields=({ struct Cyc_Core_Opt*
_temp2363=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2363->v=( void*) fields; _temp2363;}); _temp2362->attributes= 0; _temp2362;});
res=({ struct Cyc_List_List* _temp2367=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2367->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Struct_d_struct* _temp2368=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp2368[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp2369; _temp2369.tag= Cyc_Absyn_Struct_d_tag;
_temp2369.f1= strct_decl; _temp2369;}); _temp2368;}), 0); _temp2367->tl= res;
_temp2367;});}}}}}}} return res;} static void Cyc_Toc_enumdecl_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Enumdecl* ed){ struct _tuple1* n= ed->name;( void*)( ed->sc=(
void*)(((( void*) ed->sc ==( void*) Cyc_Absyn_Extern? 1:( void*) ed->sc ==( void*)
Cyc_Absyn_Static)? 1:( void*) ed->sc ==( void*) Cyc_Absyn_ExternC)?( void*) Cyc_Absyn_Public:
Cyc_Toc_scope_to_c(( void*) ed->sc)));{ struct Cyc_List_List* fs= ed->fields;
for( 0; fs != 0; fs=({ struct Cyc_List_List* _temp2370= fs; if( _temp2370 == 0){
_throw( Null_Exception);} _temp2370->tl;})){ if((( struct Cyc_Absyn_Enumfield*)({
struct Cyc_List_List* _temp2371= fs; if( _temp2371 == 0){ _throw( Null_Exception);}
_temp2371->hd;}))->tag != 0){ unsigned int i= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2373=(( struct Cyc_Absyn_Enumfield*)({
struct Cyc_List_List* _temp2372= fs; if( _temp2372 == 0){ _throw( Null_Exception);}
_temp2372->hd;}))->tag; if( _temp2373 == 0){ _throw( Null_Exception);} _temp2373;}));((
struct Cyc_Absyn_Enumfield*)({ struct Cyc_List_List* _temp2374= fs; if(
_temp2374 == 0){ _throw( Null_Exception);} _temp2374->hd;}))->tag=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp( i, 0);}}}} static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt,
struct Cyc_Absyn_Exp* e, int exhaust, struct Cyc_Absyn_Stmt* s){ void* t=( void*)({
struct Cyc_Core_Opt* _temp2375= topt; if( _temp2375 == 0){ _throw(
Null_Exception);} _temp2375->v;}); Cyc_Toc_exp_to_c( nv, e);{ struct _tuple1* x=
Cyc_Toc_temp_var(); struct _tagged_string* succ_lab= Cyc_Toc_fresh_label();
struct _tagged_string* fail_lab= exhaust? succ_lab: Cyc_Toc_fresh_label();
struct Cyc_Absyn_Stmt* _temp2378; struct Cyc_List_List* _temp2380; struct Cyc_Toc_Env*
_temp2382; struct _tuple13 _temp2376= Cyc_Toc_xlate_pat( nv, t, Cyc_Absyn_var_exp(
x, 0), Cyc_Absyn_var_exp( x, 0), p, succ_lab, fail_lab, 0); _LL2383: _temp2382=
_temp2376.f1; goto _LL2381; _LL2381: _temp2380= _temp2376.f2; goto _LL2379;
_LL2379: _temp2378= _temp2376.f3; goto _LL2377; _LL2377: Cyc_Toc_stmt_to_c(
_temp2382, s);{ struct Cyc_Absyn_Stmt* succ_stmt= Cyc_Absyn_label_stmt( succ_lab,
s, 0); if( exhaust){ s= Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c( t),( struct
Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp2378, succ_stmt, 0), 0);} else{
struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp( Cyc_Absyn_var_exp(({ struct
_tuple1* _temp2384=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp2384->f1= Cyc_Toc_abs_ns; _temp2384->f2= Cyc_Toc_Match_Exception_sp;
_temp2384;}), 0)); struct Cyc_Absyn_Stmt* fail_stmt= Cyc_Absyn_label_stmt(
fail_lab, Cyc_Absyn_exp_stmt( e3, 0), 0); s= Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c(
t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp2378, Cyc_Absyn_seq_stmt(
fail_stmt, succ_stmt, 0), 0), 0);} for( 0; _temp2380 != 0; _temp2380=({ struct
Cyc_List_List* _temp2385= _temp2380; if( _temp2385 == 0){ _throw( Null_Exception);}
_temp2385->tl;})){ struct _tuple14 _temp2389; void* _temp2390; struct _tuple1*
_temp2392; struct _tuple14* _temp2387=( struct _tuple14*)({ struct Cyc_List_List*
_temp2386= _temp2380; if( _temp2386 == 0){ _throw( Null_Exception);} _temp2386->hd;});
_temp2389=* _temp2387; _LL2393: _temp2392= _temp2389.f1; goto _LL2391; _LL2391:
_temp2390= _temp2389.f2; goto _LL2388; _LL2388: s= Cyc_Absyn_declare_stmt(
_temp2392, _temp2390, 0, s, 0);} return s;}}} static struct _tuple17 Cyc_Toc_decls_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_List_List* ds, int top){ struct Cyc_List_List*
res= 0; for( 0; ds != 0; ds=({ struct Cyc_List_List* _temp2394= ds; if(
_temp2394 == 0){ _throw( Null_Exception);} _temp2394->tl;})){ struct Cyc_Absyn_Decl*
d=( struct Cyc_Absyn_Decl*)({ struct Cyc_List_List* _temp2395= ds; if( _temp2395
== 0){ _throw( Null_Exception);} _temp2395->hd;}); void* _temp2396=( void*) d->r;
struct Cyc_Absyn_Vardecl* _temp2422; struct Cyc_Absyn_Fndecl* _temp2424; int
_temp2426; struct Cyc_Absyn_Exp* _temp2428; struct Cyc_Core_Opt* _temp2430;
struct Cyc_Core_Opt* _temp2432; struct Cyc_Absyn_Pat* _temp2434; struct Cyc_Absyn_Structdecl*
_temp2436; struct Cyc_Absyn_Uniondecl* _temp2438; struct Cyc_Absyn_Tuniondecl*
_temp2440; struct Cyc_Absyn_XTuniondecl* _temp2442; struct Cyc_Absyn_Enumdecl*
_temp2444; struct Cyc_Absyn_Typedefdecl* _temp2446; struct Cyc_List_List*
_temp2448; struct _tagged_string* _temp2450; struct Cyc_List_List* _temp2452;
struct _tuple1* _temp2454; struct Cyc_List_List* _temp2456; _LL2398: if(((
struct _tunion_struct*) _temp2396)->tag == Cyc_Absyn_Var_d_tag){ _LL2423:
_temp2422=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp2396)->f1; goto _LL2399;} else{ goto _LL2400;} _LL2400: if((( struct
_tunion_struct*) _temp2396)->tag == Cyc_Absyn_Fn_d_tag){ _LL2425: _temp2424=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*) _temp2396)->f1; goto
_LL2401;} else{ goto _LL2402;} _LL2402: if((( struct _tunion_struct*) _temp2396)->tag
== Cyc_Absyn_Let_d_tag){ _LL2435: _temp2434=( struct Cyc_Absyn_Pat*)(( struct
Cyc_Absyn_Let_d_struct*) _temp2396)->f1; goto _LL2433; _LL2433: _temp2432=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp2396)->f2; goto
_LL2431; _LL2431: _temp2430=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp2396)->f3; goto _LL2429; _LL2429: _temp2428=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Let_d_struct*) _temp2396)->f4; goto _LL2427; _LL2427: _temp2426=(
int)(( struct Cyc_Absyn_Let_d_struct*) _temp2396)->f5; goto _LL2403;} else{ goto
_LL2404;} _LL2404: if((( struct _tunion_struct*) _temp2396)->tag == Cyc_Absyn_Struct_d_tag){
_LL2437: _temp2436=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*)
_temp2396)->f1; goto _LL2405;} else{ goto _LL2406;} _LL2406: if((( struct
_tunion_struct*) _temp2396)->tag == Cyc_Absyn_Union_d_tag){ _LL2439: _temp2438=(
struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*) _temp2396)->f1;
goto _LL2407;} else{ goto _LL2408;} _LL2408: if((( struct _tunion_struct*)
_temp2396)->tag == Cyc_Absyn_Tunion_d_tag){ _LL2441: _temp2440=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_d_struct*) _temp2396)->f1; goto _LL2409;} else{ goto
_LL2410;} _LL2410: if((( struct _tunion_struct*) _temp2396)->tag == Cyc_Absyn_XTunion_d_tag){
_LL2443: _temp2442=( struct Cyc_Absyn_XTuniondecl*)(( struct Cyc_Absyn_XTunion_d_struct*)
_temp2396)->f1; goto _LL2411;} else{ goto _LL2412;} _LL2412: if((( struct
_tunion_struct*) _temp2396)->tag == Cyc_Absyn_Enum_d_tag){ _LL2445: _temp2444=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*) _temp2396)->f1;
goto _LL2413;} else{ goto _LL2414;} _LL2414: if((( struct _tunion_struct*)
_temp2396)->tag == Cyc_Absyn_Typedef_d_tag){ _LL2447: _temp2446=( struct Cyc_Absyn_Typedefdecl*)((
struct Cyc_Absyn_Typedef_d_struct*) _temp2396)->f1; goto _LL2415;} else{ goto
_LL2416;} _LL2416: if((( struct _tunion_struct*) _temp2396)->tag == Cyc_Absyn_Namespace_d_tag){
_LL2451: _temp2450=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2396)->f1; goto _LL2449; _LL2449: _temp2448=( struct Cyc_List_List*)((
struct Cyc_Absyn_Namespace_d_struct*) _temp2396)->f2; goto _LL2417;} else{ goto
_LL2418;} _LL2418: if((( struct _tunion_struct*) _temp2396)->tag == Cyc_Absyn_Using_d_tag){
_LL2455: _temp2454=( struct _tuple1*)(( struct Cyc_Absyn_Using_d_struct*)
_temp2396)->f1; goto _LL2453; _LL2453: _temp2452=( struct Cyc_List_List*)((
struct Cyc_Absyn_Using_d_struct*) _temp2396)->f2; goto _LL2419;} else{ goto
_LL2420;} _LL2420: if((( struct _tunion_struct*) _temp2396)->tag == Cyc_Absyn_ExternC_d_tag){
_LL2457: _temp2456=( struct Cyc_List_List*)(( struct Cyc_Absyn_ExternC_d_struct*)
_temp2396)->f1; goto _LL2421;} else{ goto _LL2397;} _LL2399: { struct _tuple1*
c_name= _temp2422->name; if(( void*) _temp2422->sc ==( void*) Cyc_Absyn_ExternC){
c_name=({ struct _tuple1* _temp2458=( struct _tuple1*) GC_malloc( sizeof( struct
_tuple1)); _temp2458->f1= Cyc_Toc_abs_ns; _temp2458->f2=(* c_name).f2; _temp2458;});}
if( _temp2422->initializer != 0){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp2459= _temp2422->initializer; if( _temp2459 == 0){
_throw( Null_Exception);} _temp2459;}));} if( nv->toplevel){ res=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( Cyc_Toc_temp_topdecls,
res); Cyc_Toc_temp_topdecls= 0;} if( top){ nv= Cyc_Toc_add_varmap( nv, _temp2422->name,
Cyc_Absyn_varb_exp( c_name,( void*)({ struct Cyc_Absyn_Global_b_struct*
_temp2460=( struct Cyc_Absyn_Global_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp2460[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp2461; _temp2461.tag= Cyc_Absyn_Global_b_tag;
_temp2461.f1= _temp2422; _temp2461;}); _temp2460;}), 0));} else{ nv= Cyc_Toc_add_varmap(
nv, _temp2422->name, Cyc_Absyn_varb_exp( c_name,( void*)({ struct Cyc_Absyn_Local_b_struct*
_temp2462=( struct Cyc_Absyn_Local_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp2462[ 0]=({ struct Cyc_Absyn_Local_b_struct _temp2463; _temp2463.tag= Cyc_Absyn_Local_b_tag;
_temp2463.f1= _temp2422; _temp2463;}); _temp2462;}), 0));} _temp2422->name=
c_name;( void*)( _temp2422->sc=( void*) Cyc_Toc_scope_to_c(( void*) _temp2422->sc));(
void*)( _temp2422->type=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2422->type));
res=({ struct Cyc_List_List* _temp2464=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2464->hd=( void*) d; _temp2464->tl= res;
_temp2464;}); goto _LL2397;} _LL2401: { struct _tuple1* c_name= _temp2424->name;
if(( void*) _temp2424->sc ==( void*) Cyc_Absyn_ExternC){ c_name=({ struct
_tuple1* _temp2465=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp2465->f1= Cyc_Toc_abs_ns; _temp2465->f2=(* c_name).f2; _temp2465;});} nv=
Cyc_Toc_add_varmap( nv, _temp2424->name, Cyc_Absyn_var_exp( c_name, 0));
_temp2424->name= c_name; Cyc_Toc_fndecl_to_c( nv, _temp2424); res=({ struct Cyc_List_List*
_temp2466=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2466->hd=( void*) d; _temp2466->tl= res; _temp2466;}); goto _LL2397;}
_LL2403:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp2467=( char*)"letdecl"; struct _tagged_string
_temp2468; _temp2468.curr= _temp2467; _temp2468.base= _temp2467; _temp2468.last_plus_one=
_temp2467 + 8; _temp2468;})); goto _LL2397; _LL2405: Cyc_Toc_structdecl_to_c(
_temp2436); res=({ struct Cyc_List_List* _temp2469=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2469->hd=( void*) d; _temp2469->tl=
res; _temp2469;}); goto _LL2397; _LL2407: Cyc_Toc_uniondecl_to_c( _temp2438);
res=({ struct Cyc_List_List* _temp2470=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2470->hd=( void*) d; _temp2470->tl= res;
_temp2470;}); goto _LL2397; _LL2409: res= Cyc_Toc_tuniondecl_to_c( _temp2440,
res); goto _LL2397; _LL2411: res= Cyc_Toc_xtuniondecl_to_c( _temp2442, res);
goto _LL2397; _LL2413: Cyc_Toc_enumdecl_to_c( nv, _temp2444); res=({ struct Cyc_List_List*
_temp2471=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2471->hd=( void*) d; _temp2471->tl= res; _temp2471;}); goto _LL2397;
_LL2415: _temp2446->name= _temp2446->name; _temp2446->tvs= 0;( void*)( _temp2446->defn=(
void*) Cyc_Toc_typ_to_c_array(( void*) _temp2446->defn)); res=({ struct Cyc_List_List*
_temp2472=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2472->hd=( void*) d; _temp2472->tl= res; _temp2472;}); goto _LL2397;
_LL2417: _temp2452= _temp2448; goto _LL2419; _LL2419: _temp2456= _temp2452; goto
_LL2421; _LL2421: { struct _tuple17 p= Cyc_Toc_decls_to_c( nv, _temp2456, top);
nv= p.f1; res=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)((( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( p.f2), res); goto _LL2397;} _LL2397:;} return({ struct
_tuple17 _temp2473; _temp2473.f1= nv; _temp2473.f2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( res); _temp2473;});} static void Cyc_Toc_init(){
Cyc_Toc_added_decls= 0; Cyc_Toc_tuple_types= 0; Cyc_Toc_tuple_type_counter= 0;
Cyc_Toc_tagged_ptr_types= 0; Cyc_Toc_tagged_ptr_counter= 0; Cyc_Toc_temp_var_counter=
0; Cyc_Toc_fresh_label_counter= 0; Cyc_Toc_temp_topdecls= 0; Cyc_Toc_structs_so_far=
0; Cyc_Toc_tunions_so_far= 0; Cyc_Toc_xtunions_so_far= 0;} struct Cyc_List_List*
Cyc_Toc_toc( struct Cyc_List_List* ds){ Cyc_Toc_init();{ struct Cyc_List_List*
_temp2476; struct Cyc_Toc_Env* _temp2478; struct _tuple17 _temp2474= Cyc_Toc_decls_to_c(
Cyc_Toc_empty_env(), ds, 1); _LL2479: _temp2478= _temp2474.f1; goto _LL2477;
_LL2477: _temp2476= _temp2474.f2; goto _LL2475; _LL2475: return(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_Toc_added_decls),
_temp2476);}}