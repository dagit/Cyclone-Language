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
} ; typedef int Cyc_ptrdiff_t; typedef unsigned int Cyc_size_t; typedef int Cyc_wchar_t;
typedef unsigned int Cyc_wint_t; typedef char Cyc_u_char; typedef unsigned short
Cyc_u_short; typedef unsigned int Cyc_u_int; typedef unsigned int Cyc_u_long;
typedef unsigned short Cyc_ushort; typedef unsigned int Cyc_uint; typedef
unsigned int Cyc_clock_t; typedef int Cyc_time_t; struct Cyc_timespec{ int
tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec it_interval;
struct Cyc_timespec it_value; } ; typedef int Cyc_daddr_t; typedef char* Cyc_caddr_t;
typedef unsigned short Cyc_ino_t; typedef short Cyc_dev_t; typedef int Cyc_off_t;
typedef unsigned short Cyc_uid_t; typedef unsigned short Cyc_gid_t; typedef int
Cyc_pid_t; typedef int Cyc_key_t; typedef int Cyc_ssize_t; typedef unsigned int
Cyc_mode_t; typedef unsigned short Cyc_nlink_t; typedef int Cyc_fd_mask; struct
Cyc__types_fd_set{ int fds_bits[ 8u]; } ; typedef struct Cyc__types_fd_set Cyc__types_fd_set;
typedef char* Cyc_Cstring; typedef struct _tagged_string Cyc_string; typedef
struct _tagged_string Cyc_string_t; typedef struct _tagged_string* Cyc_stringptr;
typedef int Cyc_bool; extern void* exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; typedef struct Cyc_Core_Opt* Cyc_Core_opt_t; extern void* Cyc_Core_snd(
struct _tuple0*); extern char Cyc_Core_InvalidArg_tag[ 11u]; struct Cyc_Core_InvalidArg_struct{
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
- _temp1.curr, _temp1.curr);}); Cyc_Stdio_fflush( Cyc_Stdio_stderr);( void)
_throw(( struct _xtunion_struct*)({ struct Cyc_Toc_Toc_Unimplemented_struct*
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
fs);{ struct Cyc_Absyn_Structdecl* sd=({ struct Cyc_Absyn_Structdecl* _temp102=(
struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp102->sc=( void*)(( void*) Cyc_Absyn_Public); _temp102->name=({ struct Cyc_Core_Opt*
_temp104=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp104->v=( void*)({ struct _tuple1* _temp105=( struct _tuple1*) GC_malloc(
sizeof( struct _tuple1)); _temp105->f1= Cyc_Toc_abs_ns; _temp105->f2= x;
_temp105;}); _temp104;}); _temp102->tvs= 0; _temp102->fields=({ struct Cyc_Core_Opt*
_temp103=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp103->v=( void*) fs; _temp103;}); _temp102->attributes= 0; _temp102;}); Cyc_Toc_added_decls=({
struct Cyc_List_List* _temp106=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp106->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Struct_d_struct* _temp107=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp107[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp108; _temp108.tag= Cyc_Absyn_Struct_d_tag;
_temp108.f1= sd; _temp108;}); _temp107;}), 0); _temp106->tl= Cyc_Toc_added_decls;
_temp106;});}} static void Cyc_Toc_add_tagged_ptr_decl( struct _tagged_string* x,
void* t){ void* tptr= Cyc_Absyn_star_typ( t,( void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual());
struct Cyc_Absyn_Structfield* f1=({ struct Cyc_Absyn_Structfield* _temp109=(
struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp109->name= Cyc_Toc_curr_sp; _temp109->tq= Cyc_Toc_mt_tq; _temp109->type=(
void*) tptr; _temp109->width= 0; _temp109->attributes= 0; _temp109;}); struct
Cyc_Absyn_Structfield* f2=({ struct Cyc_Absyn_Structfield* _temp110=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp110->name= Cyc_Toc_base_sp;
_temp110->tq= Cyc_Toc_mt_tq; _temp110->type=( void*) tptr; _temp110->width= 0;
_temp110->attributes= 0; _temp110;}); struct Cyc_Absyn_Structfield* f3=({ struct
Cyc_Absyn_Structfield* _temp111=( struct Cyc_Absyn_Structfield*) GC_malloc(
sizeof( struct Cyc_Absyn_Structfield)); _temp111->name= Cyc_Toc_last_plus_one_sp;
_temp111->tq= Cyc_Toc_mt_tq; _temp111->type=( void*) tptr; _temp111->width= 0;
_temp111->attributes= 0; _temp111;}); struct Cyc_List_List* fs=({ struct Cyc_List_List*
_temp112=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp112->hd=( void*) f1; _temp112->tl=({ struct Cyc_List_List* _temp113=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp113->hd=(
void*) f2; _temp113->tl=({ struct Cyc_List_List* _temp114=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp114->hd=( void*) f3; _temp114->tl=
0; _temp114;}); _temp113;}); _temp112;}); struct Cyc_Absyn_Structdecl* sd=({
struct Cyc_Absyn_Structdecl* _temp115=( struct Cyc_Absyn_Structdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp115->sc=( void*)(( void*) Cyc_Absyn_Public);
_temp115->name=({ struct Cyc_Core_Opt* _temp117=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp117->v=( void*)({ struct _tuple1*
_temp118=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp118->f1=
Cyc_Toc_abs_ns; _temp118->f2= x; _temp118;}); _temp117;}); _temp115->tvs= 0;
_temp115->fields=({ struct Cyc_Core_Opt* _temp116=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp116->v=( void*)(( struct Cyc_List_List*)
fs); _temp116;}); _temp115->attributes= 0; _temp115;}); Cyc_Toc_added_decls=({
struct Cyc_List_List* _temp119=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp119->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Struct_d_struct* _temp120=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp120[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp121; _temp121.tag= Cyc_Absyn_Struct_d_tag;
_temp121.f1= sd; _temp121;}); _temp120;}), 0); _temp119->tl= Cyc_Toc_added_decls;
_temp119;});} static struct Cyc_List_List* Cyc_Toc_tuple_types= 0; static int
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
struct _tagged_string* _temp135=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp135[ 0]= xprintf("_tuple%d",( Cyc_Toc_tuple_type_counter
++)); _temp135;}); struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)( void*(*
f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct
_tuple5*)) Cyc_Core_snd, tqs0); Cyc_Toc_add_tuple_decl( x, ts); Cyc_Toc_tuple_types=({
struct Cyc_List_List* _temp136=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp136->hd=( void*)({ struct _tuple4* _temp137=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp137->f1= x; _temp137->f2=
ts; _temp137;}); _temp136->tl= Cyc_Toc_tuple_types; _temp136;}); return x;}}
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
_tagged_string* x=({ struct _tagged_string* _temp157=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp157[ 0]= xprintf("_tagged_ptr%d",(
Cyc_Toc_tagged_ptr_counter ++)); _temp157;}); Cyc_Toc_add_tagged_ptr_decl( x, t);
Cyc_Toc_tagged_ptr_types=({ struct Cyc_List_List* _temp158=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp158->hd=( void*)({ struct
_tuple6* _temp159=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6));
_temp159->f1= x; _temp159->f2= t; _temp159;}); _temp158->tl= Cyc_Toc_tagged_ptr_types;
_temp158;}); return x;}} static int Cyc_Toc_temp_var_counter= 0; static struct
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
fields=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp611= _temp583->fields;
if( _temp611 == 0){ _throw( Null_Exception);} _temp611->v;}); for( 0; fields !=
0; fields=({ struct Cyc_List_List* _temp612= fields; if( _temp612 == 0){ _throw(
Null_Exception);} _temp612->tl;})){ if( Cyc_String_zstrptrcmp( f,(( struct Cyc_Absyn_Structfield*)({
struct Cyc_List_List* _temp613= fields; if( _temp613 == 0){ _throw(
Null_Exception);} _temp613->hd;}))->name) == 0){ return Cyc_Toc_is_void_star((
void*)(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp614= fields;
if( _temp614 == 0){ _throw( Null_Exception);} _temp614->hd;}))->type);}}(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp616=
Cyc_Absynpp_qvar2string(( struct _tuple1*)({ struct _tuple1* _temp615= _temp586;
if( _temp615 == 0){ _throw( Null_Exception);} _temp615;})); struct
_tagged_string _temp617=* f; xprintf("is_poly_field:  struct %.*s has no %.*s field",
_temp616.last_plus_one - _temp616.curr, _temp616.curr, _temp617.last_plus_one -
_temp617.curr, _temp617.curr);})); return 0;} _LL574: if( _temp592 == 0){(( void(*)(
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
fields=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp626= _temp596->fields;
if( _temp626 == 0){ _throw( Null_Exception);} _temp626->v;}); for( 0; fields !=
0; fields=({ struct Cyc_List_List* _temp627= fields; if( _temp627 == 0){ _throw(
Null_Exception);} _temp627->tl;})){ if( Cyc_String_zstrptrcmp( f,(( struct Cyc_Absyn_Structfield*)({
struct Cyc_List_List* _temp628= fields; if( _temp628 == 0){ _throw(
Null_Exception);} _temp628->hd;}))->name) == 0){ return Cyc_Toc_is_void_star((
void*)(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp629= fields;
if( _temp629 == 0){ _throw( Null_Exception);} _temp629->hd;}))->type);}}(( void(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp631=
Cyc_Absynpp_qvar2string(( struct _tuple1*)({ struct _tuple1* _temp630= _temp599;
if( _temp630 == 0){ _throw( Null_Exception);} _temp630;})); struct
_tagged_string _temp632=* f; xprintf("is_poly_field:  union %.*s has no %.*s field",
_temp631.last_plus_one - _temp631.curr, _temp631.curr, _temp632.last_plus_one -
_temp632.curr, _temp632.curr);})); return 0;} _LL578: if( _temp605 == 0){(( void(*)(
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
void* vd_typ=( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp784=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp784[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp785; _temp785.tag= Cyc_Absyn_ArrayType_tag;
_temp785.f1=( void*) elt_typ; _temp785.f2= Cyc_Toc_mt_tq; _temp785.f3=( struct
Cyc_Absyn_Exp*) sz; _temp785;}); _temp784;}); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl(
x, vd_typ,( struct Cyc_Absyn_Exp*) e); Cyc_Toc_temp_topdecls=({ struct Cyc_List_List*
_temp786=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp786->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp787=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp787[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp788; _temp788.tag= Cyc_Absyn_Var_d_tag;
_temp788.f1= vd; _temp788;}); _temp787;}), 0); _temp786->tl= Cyc_Toc_temp_topdecls;
_temp786;}); xexp= Cyc_Absyn_var_exp( x, 0);} else{ xexp= e;}{ struct Cyc_Absyn_Exp*
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
_temp856= fmt_str;( unsigned int)( _temp856.last_plus_one - _temp856.curr);});
int i= 0; for( i= 0; i < len; i ++){ if(({ struct _tagged_string _temp857=
fmt_str; char* _temp859= _temp857.curr + i; if( _temp857.base == 0? 1:( _temp859
< _temp857.base? 1: _temp859 >= _temp857.last_plus_one)){ _throw( Null_Exception);}*
_temp859;}) =='%'){ i ++; if( i < len?({ struct _tagged_string _temp860= fmt_str;
char* _temp862= _temp860.curr + i; if( _temp860.base == 0? 1:( _temp862 <
_temp860.base? 1: _temp862 >= _temp860.last_plus_one)){ _throw( Null_Exception);}*
_temp862;}) =='s': 0){ break;}}} if( i == len){ if( aopt != 0){ args=({ struct
Cyc_List_List* _temp863=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp863->hd=( void*)(( struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp864=
aopt; if( _temp864 == 0){ _throw( Null_Exception);} _temp864->v;})); _temp863->tl=
args; _temp863;});} return Cyc_Absyn_primop_exp( p, args, 0);} args=({ struct
Cyc_List_List* _temp865= args; if( _temp865 == 0){ _throw( Null_Exception);}
_temp865->tl;}); typs=({ struct Cyc_List_List* _temp866= typs; if( _temp866 == 0){
_throw( Null_Exception);} _temp866->tl;});{ struct Cyc_List_List* rev_fmt= 0;
struct Cyc_List_List* rev_result= 0; struct Cyc_List_List* rev_temps= 0; for( i=
0; i < len; i ++){ char c=({ struct _tagged_string _temp867= fmt_str; char*
_temp869= _temp867.curr + i; if( _temp867.base == 0? 1:( _temp869 < _temp867.base?
1: _temp869 >= _temp867.last_plus_one)){ _throw( Null_Exception);}* _temp869;});
rev_fmt=({ struct Cyc_List_List* _temp870=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp870->hd=( void*)(( int) c); _temp870->tl=
rev_fmt; _temp870;}); if( c !='%'){ continue;}{ struct Cyc_Core_Opt* x= Cyc_Formatstr_parse_conversionspecification(
fmt_str, i + 1); if( x == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp871=( char*)"bad format string"; struct
_tagged_string _temp872; _temp872.curr= _temp871; _temp872.base= _temp871;
_temp872.last_plus_one= _temp871 + 18; _temp872;}));}{ struct _tuple9 _temp876;
int _temp877; char _temp879; struct Cyc_List_List* _temp881; struct Cyc_List_List*
_temp883; struct Cyc_List_List* _temp885; struct Cyc_List_List* _temp887; struct
_tuple9* _temp874=( struct _tuple9*)({ struct Cyc_Core_Opt* _temp873= x; if(
_temp873 == 0){ _throw( Null_Exception);} _temp873->v;}); _temp876=* _temp874;
_LL888: _temp887= _temp876.f1; goto _LL886; _LL886: _temp885= _temp876.f2; goto
_LL884; _LL884: _temp883= _temp876.f3; goto _LL882; _LL882: _temp881= _temp876.f4;
goto _LL880; _LL880: _temp879= _temp876.f5; goto _LL878; _LL878: _temp877=
_temp876.f6; goto _LL875; _LL875: i= _temp877 - 1; if( _temp879 !='s'){ rev_fmt=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_revappend)(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)(({ struct
Cyc_List_List* _temp889=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp889->hd=( void*) _temp887; _temp889->tl=({ struct Cyc_List_List* _temp890=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp890->hd=(
void*) _temp885; _temp890->tl=({ struct Cyc_List_List* _temp891=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp891->hd=( void*) _temp883;
_temp891->tl=({ struct Cyc_List_List* _temp892=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp892->hd=( void*) _temp881;
_temp892->tl= 0; _temp892;}); _temp891;}); _temp890;}); _temp889;})), rev_fmt);
rev_fmt=({ struct Cyc_List_List* _temp893=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp893->hd=( void*)(( int) _temp879); _temp893->tl=
rev_fmt; _temp893;});{ struct Cyc_List_List* _temp894= _temp885; struct Cyc_List_List
_temp901; struct Cyc_List_List* _temp902; int _temp904; _LL896: if( _temp894 ==
0){ goto _LL898;} else{ _temp901=* _temp894; _LL905: _temp904=( int) _temp901.hd;
goto _LL903; _LL903: _temp902=( struct Cyc_List_List*) _temp901.tl; if( _temp902
== 0){ goto _LL900;} else{ goto _LL898;}} _LL900: if( _temp904 =='*'){ goto
_LL897;} else{ goto _LL898;} _LL898: goto _LL899; _LL897: { struct _tuple1* temp=
Cyc_Toc_temp_var(); rev_temps=({ struct Cyc_List_List* _temp906=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp906->hd=( void*)({ struct
_tuple10* _temp907=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10));
_temp907->f1= temp; _temp907->f2=( void*)({ struct Cyc_List_List* _temp909= typs;
if( _temp909 == 0){ _throw( Null_Exception);} _temp909->hd;}); _temp907->f3=(
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp908= args; if( _temp908 == 0){
_throw( Null_Exception);} _temp908->hd;}); _temp907;}); _temp906->tl= rev_temps;
_temp906;}); rev_result=({ struct Cyc_List_List* _temp910=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp910->hd=( void*) Cyc_Absyn_var_exp(
temp, 0); _temp910->tl= rev_result; _temp910;}); args=({ struct Cyc_List_List*
_temp911= args; if( _temp911 == 0){ _throw( Null_Exception);} _temp911->tl;});
typs=({ struct Cyc_List_List* _temp912= typs; if( _temp912 == 0){ _throw(
Null_Exception);} _temp912->tl;}); goto _LL895;} _LL899: goto _LL895; _LL895:;}{
struct Cyc_List_List* _temp913= _temp883; struct Cyc_List_List _temp920; struct
Cyc_List_List* _temp921; struct Cyc_List_List _temp923; struct Cyc_List_List*
_temp924; int _temp926; int _temp928; _LL915: if( _temp913 == 0){ goto _LL917;}
else{ _temp920=* _temp913; _LL929: _temp928=( int) _temp920.hd; goto _LL922;
_LL922: _temp921=( struct Cyc_List_List*) _temp920.tl; if( _temp921 == 0){ goto
_LL917;} else{ _temp923=* _temp921; _LL927: _temp926=( int) _temp923.hd; goto
_LL925; _LL925: _temp924=( struct Cyc_List_List*) _temp923.tl; if( _temp924 == 0){
goto _LL919;} else{ goto _LL917;}}} _LL919: if( _temp928 =='.'? _temp926 =='*':
0){ goto _LL916;} else{ goto _LL917;} _LL917: goto _LL918; _LL916: { struct
_tuple1* temp= Cyc_Toc_temp_var(); rev_temps=({ struct Cyc_List_List* _temp930=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp930->hd=(
void*)({ struct _tuple10* _temp931=( struct _tuple10*) GC_malloc( sizeof( struct
_tuple10)); _temp931->f1= temp; _temp931->f2=( void*)({ struct Cyc_List_List*
_temp933= typs; if( _temp933 == 0){ _throw( Null_Exception);} _temp933->hd;});
_temp931->f3=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp932= args;
if( _temp932 == 0){ _throw( Null_Exception);} _temp932->hd;}); _temp931;});
_temp930->tl= rev_temps; _temp930;}); rev_result=({ struct Cyc_List_List*
_temp934=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp934->hd=( void*) Cyc_Absyn_var_exp( temp, 0); _temp934->tl= rev_result;
_temp934;}); args=({ struct Cyc_List_List* _temp935= args; if( _temp935 == 0){
_throw( Null_Exception);} _temp935->tl;}); typs=({ struct Cyc_List_List*
_temp936= typs; if( _temp936 == 0){ _throw( Null_Exception);} _temp936->tl;});
goto _LL914;} _LL918: goto _LL914; _LL914:;} if( _temp879 !='%'){ if( args == 0?
1: typs == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct
_tagged_string _temp937= Cyc_Absynpp_prim2string( p); xprintf("missing arg to %.*s",
_temp937.last_plus_one - _temp937.curr, _temp937.curr);}));}{ struct _tuple1*
temp= Cyc_Toc_temp_var(); rev_temps=({ struct Cyc_List_List* _temp938=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp938->hd=( void*)({
struct _tuple10* _temp939=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10));
_temp939->f1= temp; _temp939->f2=( void*)({ struct Cyc_List_List* _temp941= typs;
if( _temp941 == 0){ _throw( Null_Exception);} _temp941->hd;}); _temp939->f3=(
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp940= args; if( _temp940 == 0){
_throw( Null_Exception);} _temp940->hd;}); _temp939;}); _temp938->tl= rev_temps;
_temp938;}); rev_result=({ struct Cyc_List_List* _temp942=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp942->hd=( void*) Cyc_Absyn_var_exp(
temp, 0); _temp942->tl= rev_result; _temp942;}); args=({ struct Cyc_List_List*
_temp943= args; if( _temp943 == 0){ _throw( Null_Exception);} _temp943->tl;});
typs=({ struct Cyc_List_List* _temp944= typs; if( _temp944 == 0){ _throw(
Null_Exception);} _temp944->tl;});}}} else{ rev_fmt=({ struct Cyc_List_List*
_temp945=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp945->hd=( void*)(( int)'.'); _temp945->tl= rev_fmt; _temp945;}); rev_fmt=({
struct Cyc_List_List* _temp946=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp946->hd=( void*)(( int)'*'); _temp946->tl= rev_fmt;
_temp946;}); rev_fmt=({ struct Cyc_List_List* _temp947=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp947->hd=( void*)(( int)'s');
_temp947->tl= rev_fmt; _temp947;});{ struct _tuple1* temp= Cyc_Toc_temp_var();
rev_temps=({ struct Cyc_List_List* _temp948=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp948->hd=( void*)({ struct _tuple10*
_temp949=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp949->f1=
temp; _temp949->f2=( void*)({ struct Cyc_List_List* _temp951= typs; if( _temp951
== 0){ _throw( Null_Exception);} _temp951->hd;}); _temp949->f3=( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp950= args; if( _temp950 == 0){ _throw( Null_Exception);}
_temp950->hd;}); _temp949;}); _temp948->tl= rev_temps; _temp948;});{ struct Cyc_Absyn_Exp*
size_exp= Cyc_Absyn_subtract_exp( Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
temp, 0), Cyc_Toc_last_plus_one_sp, 0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
temp, 0), Cyc_Toc_curr_sp, 0), 0); struct Cyc_Absyn_Exp* cstring_exp= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0); rev_result=({ struct Cyc_List_List*
_temp952=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp952->hd=( void*) size_exp; _temp952->tl= rev_result; _temp952;});
rev_result=({ struct Cyc_List_List* _temp953=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp953->hd=( void*) cstring_exp; _temp953->tl=
rev_result; _temp953;}); args=({ struct Cyc_List_List* _temp954= args; if(
_temp954 == 0){ _throw( Null_Exception);} _temp954->tl;}); typs=({ struct Cyc_List_List*
_temp955= typs; if( _temp955 == 0){ _throw( Null_Exception);} _temp955->tl;});}}}}}}{
struct Cyc_List_List* es=({ struct Cyc_List_List* _temp956=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp956->hd=( void*) Cyc_Absyn_string_exp(
Cyc_String_implode((( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
rev_fmt)), 0); _temp956->tl=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_rev)( rev_result); _temp956;}); if( aopt != 0){ es=({ struct Cyc_List_List*
_temp957=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp957->hd=( void*)(( struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp958=
aopt; if( _temp958 == 0){ _throw( Null_Exception);} _temp958->v;})); _temp957->tl=
es; _temp957;});}{ struct Cyc_Absyn_Exp* e= Cyc_Absyn_primop_exp( p, es, 0);
struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( e, 0); for( 0; rev_temps != 0;
rev_temps=({ struct Cyc_List_List* _temp959= rev_temps; if( _temp959 == 0){
_throw( Null_Exception);} _temp959->tl;})){ s= Cyc_Absyn_declare_stmt((*((
struct _tuple10*)({ struct Cyc_List_List* _temp960= rev_temps; if( _temp960 == 0){
_throw( Null_Exception);} _temp960->hd;}))).f1,(*(( struct _tuple10*)({ struct
Cyc_List_List* _temp961= rev_temps; if( _temp961 == 0){ _throw( Null_Exception);}
_temp961->hd;}))).f2,( struct Cyc_Absyn_Exp*)(*(( struct _tuple10*)({ struct Cyc_List_List*
_temp962= rev_temps; if( _temp962 == 0){ _throw( Null_Exception);} _temp962->hd;}))).f3,
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
static struct Cyc_Absyn_Exp* Cyc_Toc_init_tuple( struct Cyc_Toc_Env* nv, int
pointer, struct Cyc_Absyn_Exp* rgnopt, struct Cyc_List_List* es){ struct Cyc_List_List*
tqs=(( struct Cyc_List_List*(*)( struct _tuple5*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_tup_to_c, es); struct
_tagged_string* n= Cyc_Toc_add_tuple_type( tqs); struct _tuple1* x= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp( x, 0); struct Cyc_Absyn_Stmt* s=
Cyc_Absyn_exp_stmt( xexp, 0); struct Cyc_Absyn_Exp*(* lval_f)( struct Cyc_Absyn_Exp*,
struct _tagged_string*, struct Cyc_Position_Segment*)= pointer? Cyc_Absyn_structarrow_exp:
Cyc_Absyn_structmember_exp; int is_atomic= 1; es=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( es);{ int i=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( es); for( 0; es != 0; es=({ struct Cyc_List_List* _temp974=
es; if( _temp974 == 0){ _throw( Null_Exception);} _temp974->tl;}), -- i){ Cyc_Toc_exp_to_c(
nv,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp975= es; if( _temp975
== 0){ _throw( Null_Exception);} _temp975->hd;})); is_atomic= is_atomic? Cyc_Toc_atomic_typ((
void*)({ struct Cyc_Core_Opt* _temp977=(( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp976= es; if( _temp976 == 0){ _throw( Null_Exception);} _temp976->hd;}))->topt;
if( _temp977 == 0){ _throw( Null_Exception);} _temp977->v;})): 0; s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( lval_f( xexp,({ struct _tagged_string*
_temp978=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp978[ 0]= xprintf("f%d", i); _temp978;}), 0),( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp979= es; if( _temp979 == 0){ _throw( Null_Exception);}
_temp979->hd;}), 0), 0), s, 0);}} return Cyc_Toc_make_struct( nv, x, Cyc_Absyn_strct(
n), s, pointer, rgnopt, is_atomic);} static struct Cyc_Absyn_Exp* Cyc_Toc_init_struct(
struct Cyc_Toc_Env* nv, void* struct_type, int pointer, struct Cyc_Absyn_Exp*
rgnopt, struct Cyc_List_List* dles, struct _tuple1* tdn){ struct _tuple1* x= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp( x, 0); struct Cyc_Absyn_Stmt* s=
Cyc_Absyn_exp_stmt( xexp, 0); struct Cyc_Absyn_Exp*(* lval_f)( struct Cyc_Absyn_Exp*,
struct _tagged_string*, struct Cyc_Position_Segment*)= pointer? Cyc_Absyn_structarrow_exp:
Cyc_Absyn_structmember_exp; int is_atomic= 1;{ struct Cyc_List_List* es=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( dles); for( 0;
es != 0; es=({ struct Cyc_List_List* _temp980= es; if( _temp980 == 0){ _throw(
Null_Exception);} _temp980->tl;})){ struct _tuple7 _temp984; struct Cyc_Absyn_Exp*
_temp985; struct Cyc_List_List* _temp987; struct _tuple7* _temp982=( struct
_tuple7*)({ struct Cyc_List_List* _temp981= es; if( _temp981 == 0){ _throw(
Null_Exception);} _temp981->hd;}); _temp984=* _temp982; _LL988: _temp987=
_temp984.f1; goto _LL986; _LL986: _temp985= _temp984.f2; goto _LL983; _LL983:
is_atomic= is_atomic? Cyc_Toc_atomic_typ(( void*)({ struct Cyc_Core_Opt*
_temp989= _temp985->topt; if( _temp989 == 0){ _throw( Null_Exception);} _temp989->v;})):
0; if( _temp987 == 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp990=( char*)"empty designator list"; struct
_tagged_string _temp991; _temp991.curr= _temp990; _temp991.base= _temp990;
_temp991.last_plus_one= _temp990 + 22; _temp991;}));} if(({ struct Cyc_List_List*
_temp992= _temp987; if( _temp992 == 0){ _throw( Null_Exception);} _temp992->tl;})
!= 0){ struct _tuple1* y= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* yexp= Cyc_Absyn_var_exp(
y, 0); for( 0; _temp987 != 0; _temp987=({ struct Cyc_List_List* _temp993=
_temp987; if( _temp993 == 0){ _throw( Null_Exception);} _temp993->tl;})){ void*
_temp995=( void*)({ struct Cyc_List_List* _temp994= _temp987; if( _temp994 == 0){
_throw( Null_Exception);} _temp994->hd;}); struct _tagged_string* _temp1001;
_LL997: if((( struct _tunion_struct*) _temp995)->tag == Cyc_Absyn_FieldName_tag){
_LL1002: _temp1001=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp995)->f1; goto _LL998;} else{ goto _LL999;} _LL999: goto _LL1000; _LL998:
if( Cyc_Toc_is_poly_field( struct_type, _temp1001)){ yexp= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), yexp, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval_f( xexp, _temp1001, 0), yexp, 0), 0), s, 0); goto
_LL996; _LL1000:( void) _throw((( struct _xtunion_struct*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1003=(
char*)"array designator in struct"; struct _tagged_string _temp1004; _temp1004.curr=
_temp1003; _temp1004.base= _temp1003; _temp1004.last_plus_one= _temp1003 + 27;
_temp1004;}))); _LL996:;} Cyc_Toc_exp_to_c( nv, _temp985); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp( yexp, _temp985, 0), 0), s, 0);} else{
void* _temp1006=( void*)({ struct Cyc_List_List* _temp1005= _temp987; if(
_temp1005 == 0){ _throw( Null_Exception);} _temp1005->hd;}); struct
_tagged_string* _temp1012; _LL1008: if((( struct _tunion_struct*) _temp1006)->tag
== Cyc_Absyn_FieldName_tag){ _LL1013: _temp1012=( struct _tagged_string*)((
struct Cyc_Absyn_FieldName_struct*) _temp1006)->f1; goto _LL1009;} else{ goto
_LL1010;} _LL1010: goto _LL1011; _LL1009: Cyc_Toc_exp_to_c( nv, _temp985); if(
Cyc_Toc_is_poly_field( struct_type, _temp1012)){ _temp985= Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(), _temp985, 0);} s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( lval_f( xexp, _temp1012, 0), _temp985, 0), 0), s, 0); goto
_LL1007; _LL1011:( void) _throw((( struct _xtunion_struct*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1014=(
char*)"array designator in struct"; struct _tagged_string _temp1015; _temp1015.curr=
_temp1014; _temp1015.base= _temp1014; _temp1015.last_plus_one= _temp1014 + 27;
_temp1015;}))); _LL1007:;}}} return Cyc_Toc_make_struct( nv, x, Cyc_Absyn_strctq(
tdn), s, pointer, rgnopt, is_atomic);} static struct Cyc_Absyn_Exp* Cyc_Toc_assignop_lvalue(
struct Cyc_Absyn_Exp* el, struct _tuple11* pr){ return Cyc_Absyn_assignop_exp(
el,(* pr).f1,(* pr).f2, 0);} static struct Cyc_Absyn_Exp* Cyc_Toc_address_lvalue(
struct Cyc_Absyn_Exp* e1, int ignore){ return Cyc_Absyn_address_exp( e1, 0);}
static struct Cyc_Absyn_Exp* Cyc_Toc_incr_lvalue( struct Cyc_Absyn_Exp* e1, void*
incr){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Increment_e_struct*
_temp1016=( struct Cyc_Absyn_Increment_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Increment_e_struct));
_temp1016[ 0]=({ struct Cyc_Absyn_Increment_e_struct _temp1017; _temp1017.tag=
Cyc_Absyn_Increment_e_tag; _temp1017.f1= e1; _temp1017.f2=( void*) incr;
_temp1017;}); _temp1016;}), 0);} static void Cyc_Toc_lvalue_assign_stmt( struct
Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct
Cyc_Absyn_Exp*, void*), void* f_env); static void Cyc_Toc_lvalue_assign( struct
Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct
Cyc_Absyn_Exp*, void*), void* f_env){ void* _temp1018=( void*) e1->r; struct Cyc_Absyn_Stmt*
_temp1028; struct Cyc_Absyn_Exp* _temp1030; void* _temp1032; struct
_tagged_string* _temp1034; struct Cyc_Absyn_Exp* _temp1036; _LL1020: if(((
struct _tunion_struct*) _temp1018)->tag == Cyc_Absyn_StmtExp_e_tag){ _LL1029:
_temp1028=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1018)->f1; goto _LL1021;} else{ goto _LL1022;} _LL1022: if((( struct
_tunion_struct*) _temp1018)->tag == Cyc_Absyn_Cast_e_tag){ _LL1033: _temp1032=(
void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp1018)->f1; goto _LL1031; _LL1031:
_temp1030=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*) _temp1018)->f2;
goto _LL1023;} else{ goto _LL1024;} _LL1024: if((( struct _tunion_struct*)
_temp1018)->tag == Cyc_Absyn_StructMember_e_tag){ _LL1037: _temp1036=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*) _temp1018)->f1; goto
_LL1035; _LL1035: _temp1034=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1018)->f2; goto _LL1025;} else{ goto _LL1026;} _LL1026: goto _LL1027;
_LL1021:(( void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs, struct
Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp1028, fs, f, f_env); goto _LL1019; _LL1023:(( void(*)( struct Cyc_Absyn_Exp*
e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1030, fs, f, f_env); goto
_LL1019; _LL1025:( void*)( e1->r=( void*)(( void*) _temp1036->r));(( void(*)(
struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)( e1,({
struct Cyc_List_List* _temp1038=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1038->hd=( void*) _temp1034; _temp1038->tl= fs;
_temp1038;}), f, f_env); goto _LL1019; _LL1027: if( ! Cyc_Absyn_is_lvalue( e1)){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string
_temp1039= Cyc_Absynpp_exp2string( e1); xprintf("lvalue_assign: could not find lvalue in `%.*s'",
_temp1039.last_plus_one - _temp1039.curr, _temp1039.curr);}));}{ struct Cyc_Absyn_Exp*
e1_copy= Cyc_Absyn_copy_exp( e1); for( 0; fs != 0; fs=({ struct Cyc_List_List*
_temp1040= fs; if( _temp1040 == 0){ _throw( Null_Exception);} _temp1040->tl;})){
e1_copy= Cyc_Absyn_structmember_exp( e1_copy,( struct _tagged_string*)({ struct
Cyc_List_List* _temp1041= fs; if( _temp1041 == 0){ _throw( Null_Exception);}
_temp1041->hd;}), e1_copy->loc);}( void*)( e1->r=( void*)(( void*)( f( e1_copy,
f_env))->r)); goto _LL1019;} _LL1019:;} static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Absyn_Exp*, void*), void* f_env){ void* _temp1042=( void*) s->r;
struct Cyc_Absyn_Exp* _temp1052; struct Cyc_Absyn_Stmt* _temp1054; struct Cyc_Absyn_Decl*
_temp1056; struct Cyc_Absyn_Stmt* _temp1058; struct Cyc_Absyn_Stmt* _temp1060;
_LL1044: if(( unsigned int) _temp1042 > 1u?(( struct _tunion_struct*) _temp1042)->tag
== Cyc_Absyn_Exp_s_tag: 0){ _LL1053: _temp1052=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Exp_s_struct*) _temp1042)->f1; goto _LL1045;} else{ goto _LL1046;}
_LL1046: if(( unsigned int) _temp1042 > 1u?(( struct _tunion_struct*) _temp1042)->tag
== Cyc_Absyn_Decl_s_tag: 0){ _LL1057: _temp1056=( struct Cyc_Absyn_Decl*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1042)->f1; goto _LL1055; _LL1055:
_temp1054=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1042)->f2;
goto _LL1047;} else{ goto _LL1048;} _LL1048: if(( unsigned int) _temp1042 > 1u?((
struct _tunion_struct*) _temp1042)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL1061:
_temp1060=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1042)->f1;
goto _LL1059; _LL1059: _temp1058=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp1042)->f2; goto _LL1049;} else{ goto _LL1050;} _LL1050: goto _LL1051;
_LL1045:(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct
Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign)(
_temp1052, fs, f, f_env); goto _LL1043; _LL1047:(( void(*)( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)( _temp1054, fs, f, f_env);
goto _LL1043; _LL1049:(( void(*)( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List*
fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, void*), void* f_env)) Cyc_Toc_lvalue_assign_stmt)(
_temp1058, fs, f, f_env); goto _LL1043; _LL1051:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1062= Cyc_Absynpp_stmt2string(
s); xprintf("lvalue_assign_stmt: %.*s", _temp1062.last_plus_one - _temp1062.curr,
_temp1062.curr);})); goto _LL1043; _LL1043:;} static struct Cyc_Absyn_Exp* Cyc_Toc_known_size_subscript(
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
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1063=
Cyc_Absynpp_exp2string( e); xprintf("exp_to_c: no type for %.*s", _temp1063.last_plus_one
- _temp1063.curr, _temp1063.curr);})); return;}{ void* old_typ=( void*)({ struct
Cyc_Core_Opt* _temp1064= e->topt; if( _temp1064 == 0){ _throw( Null_Exception);}
_temp1064->v;}); void* _temp1065= r; void* _temp1141; void* _temp1143; void*
_temp1145; struct _tuple1* _temp1147; struct _tuple1* _temp1149; struct Cyc_List_List*
_temp1151; void* _temp1153; void* _temp1155; struct Cyc_Absyn_Exp* _temp1157;
struct Cyc_Absyn_Exp* _temp1159; struct Cyc_Core_Opt* _temp1161; struct Cyc_Absyn_Exp*
_temp1163; struct Cyc_Absyn_Exp* _temp1165; struct Cyc_Absyn_Exp* _temp1167;
struct Cyc_Absyn_Exp* _temp1169; struct Cyc_Absyn_Exp* _temp1171; struct Cyc_Absyn_Exp*
_temp1173; struct Cyc_List_List* _temp1175; struct Cyc_Absyn_Exp* _temp1177;
struct Cyc_List_List* _temp1179; struct Cyc_Absyn_Exp* _temp1181; struct Cyc_Absyn_Exp*
_temp1183; struct Cyc_Absyn_Exp* _temp1185; struct Cyc_List_List* _temp1187;
struct Cyc_Absyn_Exp* _temp1189; struct Cyc_Absyn_Exp* _temp1191; struct Cyc_Absyn_Exp**
_temp1193; void* _temp1194; void** _temp1196; struct Cyc_Absyn_Exp* _temp1197;
struct Cyc_Absyn_Exp* _temp1199; struct Cyc_Absyn_Exp* _temp1201; void*
_temp1203; struct Cyc_Absyn_Exp* _temp1205; struct Cyc_Absyn_Exp* _temp1207;
struct _tagged_string* _temp1209; struct Cyc_Absyn_Exp* _temp1211; struct
_tagged_string* _temp1213; struct Cyc_Absyn_Exp* _temp1215; struct Cyc_Absyn_Exp*
_temp1217; struct Cyc_Absyn_Exp* _temp1219; struct Cyc_List_List* _temp1221;
struct Cyc_List_List* _temp1223; struct _tuple2* _temp1225; struct Cyc_List_List*
_temp1227; struct Cyc_Absyn_Exp* _temp1229; struct Cyc_Absyn_Exp* _temp1231;
struct Cyc_Absyn_Vardecl* _temp1233; struct Cyc_Absyn_Structdecl* _temp1235;
struct Cyc_List_List* _temp1237; struct Cyc_Core_Opt* _temp1239; struct _tuple1*
_temp1241; struct Cyc_Absyn_Tunionfield* _temp1243; struct Cyc_Absyn_Tuniondecl*
_temp1245; struct Cyc_List_List* _temp1247; struct Cyc_Core_Opt* _temp1249;
struct Cyc_Core_Opt* _temp1251; struct Cyc_Absyn_Tunionfield* _temp1253; struct
Cyc_Absyn_Tuniondecl* _temp1255; struct Cyc_List_List* _temp1257; struct Cyc_Core_Opt*
_temp1259; struct Cyc_Core_Opt* _temp1261; struct Cyc_Absyn_Tunionfield*
_temp1263; struct Cyc_Absyn_XTuniondecl* _temp1265; struct Cyc_List_List*
_temp1267; struct Cyc_Core_Opt* _temp1269; struct Cyc_Absyn_Enumfield* _temp1271;
struct Cyc_Absyn_Enumdecl* _temp1273; struct _tuple1* _temp1275; void* _temp1277;
struct Cyc_Absyn_Exp* _temp1279; struct Cyc_Absyn_Stmt* _temp1281; struct Cyc_List_List*
_temp1283; struct Cyc_Core_Opt* _temp1285; struct Cyc_Absyn_Fndecl* _temp1287;
struct Cyc_Absyn_Exp* _temp1289; _LL1067: if((( struct _tunion_struct*)
_temp1065)->tag == Cyc_Absyn_Const_e_tag){ _LL1142: _temp1141=( void*)(( struct
Cyc_Absyn_Const_e_struct*) _temp1065)->f1; if(( int) _temp1141 == Cyc_Absyn_Null_c){
goto _LL1068;} else{ goto _LL1069;}} else{ goto _LL1069;} _LL1069: if((( struct
_tunion_struct*) _temp1065)->tag == Cyc_Absyn_Const_e_tag){ _LL1144: _temp1143=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1065)->f1; goto _LL1070;} else{
goto _LL1071;} _LL1071: if((( struct _tunion_struct*) _temp1065)->tag == Cyc_Absyn_Var_e_tag){
_LL1148: _temp1147=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1065)->f1; goto _LL1146; _LL1146: _temp1145=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1065)->f2; goto _LL1072;} else{ goto _LL1073;} _LL1073: if((( struct
_tunion_struct*) _temp1065)->tag == Cyc_Absyn_UnknownId_e_tag){ _LL1150:
_temp1149=( struct _tuple1*)(( struct Cyc_Absyn_UnknownId_e_struct*) _temp1065)->f1;
goto _LL1074;} else{ goto _LL1075;} _LL1075: if((( struct _tunion_struct*)
_temp1065)->tag == Cyc_Absyn_Primop_e_tag){ _LL1154: _temp1153=( void*)(( struct
Cyc_Absyn_Primop_e_struct*) _temp1065)->f1; goto _LL1152; _LL1152: _temp1151=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1065)->f2; goto
_LL1076;} else{ goto _LL1077;} _LL1077: if((( struct _tunion_struct*) _temp1065)->tag
== Cyc_Absyn_Increment_e_tag){ _LL1158: _temp1157=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Increment_e_struct*) _temp1065)->f1; goto _LL1156; _LL1156:
_temp1155=( void*)(( struct Cyc_Absyn_Increment_e_struct*) _temp1065)->f2; goto
_LL1078;} else{ goto _LL1079;} _LL1079: if((( struct _tunion_struct*) _temp1065)->tag
== Cyc_Absyn_AssignOp_e_tag){ _LL1164: _temp1163=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1065)->f1; goto _LL1162; _LL1162:
_temp1161=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1065)->f2; goto _LL1160; _LL1160: _temp1159=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1065)->f3; goto _LL1080;} else{ goto
_LL1081;} _LL1081: if((( struct _tunion_struct*) _temp1065)->tag == Cyc_Absyn_Conditional_e_tag){
_LL1170: _temp1169=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1065)->f1; goto _LL1168; _LL1168: _temp1167=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp1065)->f2; goto _LL1166; _LL1166:
_temp1165=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1065)->f3; goto _LL1082;} else{ goto _LL1083;} _LL1083: if((( struct
_tunion_struct*) _temp1065)->tag == Cyc_Absyn_SeqExp_e_tag){ _LL1174: _temp1173=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*) _temp1065)->f1; goto
_LL1172; _LL1172: _temp1171=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1065)->f2; goto _LL1084;} else{ goto _LL1085;} _LL1085: if((( struct
_tunion_struct*) _temp1065)->tag == Cyc_Absyn_UnknownCall_e_tag){ _LL1178:
_temp1177=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1065)->f1; goto _LL1176; _LL1176: _temp1175=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_e_struct*) _temp1065)->f2; goto _LL1086;} else{
goto _LL1087;} _LL1087: if((( struct _tunion_struct*) _temp1065)->tag == Cyc_Absyn_FnCall_e_tag){
_LL1182: _temp1181=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1065)->f1; goto _LL1180; _LL1180: _temp1179=( struct Cyc_List_List*)((
struct Cyc_Absyn_FnCall_e_struct*) _temp1065)->f2; goto _LL1088;} else{ goto
_LL1089;} _LL1089: if((( struct _tunion_struct*) _temp1065)->tag == Cyc_Absyn_Throw_e_tag){
_LL1184: _temp1183=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*)
_temp1065)->f1; goto _LL1090;} else{ goto _LL1091;} _LL1091: if((( struct
_tunion_struct*) _temp1065)->tag == Cyc_Absyn_NoInstantiate_e_tag){ _LL1186:
_temp1185=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1065)->f1; goto _LL1092;} else{ goto _LL1093;} _LL1093: if((( struct
_tunion_struct*) _temp1065)->tag == Cyc_Absyn_Instantiate_e_tag){ _LL1190:
_temp1189=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1065)->f1; goto _LL1188; _LL1188: _temp1187=( struct Cyc_List_List*)((
struct Cyc_Absyn_Instantiate_e_struct*) _temp1065)->f2; goto _LL1094;} else{
goto _LL1095;} _LL1095: if((( struct _tunion_struct*) _temp1065)->tag == Cyc_Absyn_Cast_e_tag){
_LL1195: _temp1194=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp1065)->f1;
_temp1196=&(( struct Cyc_Absyn_Cast_e_struct*) _temp1065)->f1; goto _LL1192;
_LL1192: _temp1191=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1065)->f2; _temp1193=&(( struct Cyc_Absyn_Cast_e_struct*) _temp1065)->f2;
goto _LL1096;} else{ goto _LL1097;} _LL1097: if((( struct _tunion_struct*)
_temp1065)->tag == Cyc_Absyn_Address_e_tag){ _LL1198: _temp1197=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Address_e_struct*) _temp1065)->f1; goto _LL1098;} else{ goto
_LL1099;} _LL1099: if((( struct _tunion_struct*) _temp1065)->tag == Cyc_Absyn_New_e_tag){
_LL1202: _temp1201=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*)
_temp1065)->f1; goto _LL1200; _LL1200: _temp1199=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_New_e_struct*) _temp1065)->f2; goto _LL1100;} else{ goto
_LL1101;} _LL1101: if((( struct _tunion_struct*) _temp1065)->tag == Cyc_Absyn_Sizeoftyp_e_tag){
_LL1204: _temp1203=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp1065)->f1;
goto _LL1102;} else{ goto _LL1103;} _LL1103: if((( struct _tunion_struct*)
_temp1065)->tag == Cyc_Absyn_Sizeofexp_e_tag){ _LL1206: _temp1205=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Sizeofexp_e_struct*) _temp1065)->f1; goto _LL1104;} else{ goto
_LL1105;} _LL1105: if((( struct _tunion_struct*) _temp1065)->tag == Cyc_Absyn_Deref_e_tag){
_LL1208: _temp1207=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp1065)->f1; goto _LL1106;} else{ goto _LL1107;} _LL1107: if((( struct
_tunion_struct*) _temp1065)->tag == Cyc_Absyn_StructMember_e_tag){ _LL1212:
_temp1211=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1065)->f1; goto _LL1210; _LL1210: _temp1209=( struct _tagged_string*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp1065)->f2; goto _LL1108;} else{
goto _LL1109;} _LL1109: if((( struct _tunion_struct*) _temp1065)->tag == Cyc_Absyn_StructArrow_e_tag){
_LL1216: _temp1215=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1065)->f1; goto _LL1214; _LL1214: _temp1213=( struct _tagged_string*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp1065)->f2; goto _LL1110;} else{
goto _LL1111;} _LL1111: if((( struct _tunion_struct*) _temp1065)->tag == Cyc_Absyn_Subscript_e_tag){
_LL1220: _temp1219=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1065)->f1; goto _LL1218; _LL1218: _temp1217=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp1065)->f2; goto _LL1112;} else{ goto
_LL1113;} _LL1113: if((( struct _tunion_struct*) _temp1065)->tag == Cyc_Absyn_Tuple_e_tag){
_LL1222: _temp1221=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1065)->f1; goto _LL1114;} else{ goto _LL1115;} _LL1115: if((( struct
_tunion_struct*) _temp1065)->tag == Cyc_Absyn_CompoundLit_e_tag){ _LL1226:
_temp1225=( struct _tuple2*)(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp1065)->f1;
goto _LL1224; _LL1224: _temp1223=( struct Cyc_List_List*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1065)->f2; goto _LL1116;} else{ goto _LL1117;} _LL1117: if((( struct
_tunion_struct*) _temp1065)->tag == Cyc_Absyn_Array_e_tag){ _LL1228: _temp1227=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1065)->f1; goto
_LL1118;} else{ goto _LL1119;} _LL1119: if((( struct _tunion_struct*) _temp1065)->tag
== Cyc_Absyn_Comprehension_e_tag){ _LL1234: _temp1233=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1065)->f1; goto _LL1232; _LL1232:
_temp1231=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1065)->f2; goto _LL1230; _LL1230: _temp1229=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1065)->f3; goto _LL1120;} else{
goto _LL1121;} _LL1121: if((( struct _tunion_struct*) _temp1065)->tag == Cyc_Absyn_Struct_e_tag){
_LL1242: _temp1241=( struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1065)->f1; goto _LL1240; _LL1240: _temp1239=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1065)->f2; goto _LL1238; _LL1238:
_temp1237=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1065)->f3; goto _LL1236; _LL1236: _temp1235=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1065)->f4; goto _LL1122;} else{ goto
_LL1123;} _LL1123: if((( struct _tunion_struct*) _temp1065)->tag == Cyc_Absyn_Tunion_e_tag){
_LL1252: _temp1251=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1065)->f1; goto _LL1250; _LL1250: _temp1249=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1065)->f2; goto _LL1248; _LL1248:
_temp1247=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1065)->f3; if( _temp1247 == 0){ goto _LL1246;} else{ goto _LL1125;} _LL1246:
_temp1245=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1065)->f4; goto _LL1244; _LL1244: _temp1243=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1065)->f5; goto _LL1124;} else{ goto
_LL1125;} _LL1125: if((( struct _tunion_struct*) _temp1065)->tag == Cyc_Absyn_Tunion_e_tag){
_LL1262: _temp1261=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1065)->f1; goto _LL1260; _LL1260: _temp1259=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1065)->f2; goto _LL1258; _LL1258:
_temp1257=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1065)->f3; goto _LL1256; _LL1256: _temp1255=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1065)->f4; goto _LL1254; _LL1254:
_temp1253=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1065)->f5; goto _LL1126;} else{ goto _LL1127;} _LL1127: if((( struct
_tunion_struct*) _temp1065)->tag == Cyc_Absyn_XTunion_e_tag){ _LL1270: _temp1269=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_XTunion_e_struct*) _temp1065)->f1; goto
_LL1268; _LL1268: _temp1267=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_e_struct*)
_temp1065)->f2; goto _LL1266; _LL1266: _temp1265=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_e_struct*) _temp1065)->f3; goto _LL1264; _LL1264:
_temp1263=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_e_struct*)
_temp1065)->f4; goto _LL1128;} else{ goto _LL1129;} _LL1129: if((( struct
_tunion_struct*) _temp1065)->tag == Cyc_Absyn_Enum_e_tag){ _LL1276: _temp1275=(
struct _tuple1*)(( struct Cyc_Absyn_Enum_e_struct*) _temp1065)->f1; goto _LL1274;
_LL1274: _temp1273=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp1065)->f2; goto _LL1272; _LL1272: _temp1271=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_e_struct*) _temp1065)->f3; goto _LL1130;} else{ goto
_LL1131;} _LL1131: if((( struct _tunion_struct*) _temp1065)->tag == Cyc_Absyn_Malloc_e_tag){
_LL1280: _temp1279=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1065)->f1; goto _LL1278; _LL1278: _temp1277=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1065)->f2; goto _LL1132;} else{ goto _LL1133;} _LL1133: if((( struct
_tunion_struct*) _temp1065)->tag == Cyc_Absyn_StmtExp_e_tag){ _LL1282: _temp1281=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*) _temp1065)->f1;
goto _LL1134;} else{ goto _LL1135;} _LL1135: if((( struct _tunion_struct*)
_temp1065)->tag == Cyc_Absyn_UnresolvedMem_e_tag){ _LL1286: _temp1285=( struct
Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp1065)->f1; goto
_LL1284; _LL1284: _temp1283=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1065)->f2; goto _LL1136;} else{ goto _LL1137;} _LL1137: if((( struct
_tunion_struct*) _temp1065)->tag == Cyc_Absyn_Codegen_e_tag){ _LL1288: _temp1287=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*) _temp1065)->f1;
goto _LL1138;} else{ goto _LL1139;} _LL1139: if((( struct _tunion_struct*)
_temp1065)->tag == Cyc_Absyn_Fill_e_tag){ _LL1290: _temp1289=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Fill_e_struct*) _temp1065)->f1; goto _LL1140;} else{ goto
_LL1066;} _LL1068:{ void* _temp1291= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1297; struct Cyc_Absyn_Conref* _temp1299; struct Cyc_Absyn_Tqual* _temp1301;
struct Cyc_Absyn_Conref* _temp1303; void* _temp1305; void* _temp1307; _LL1293:
if(( unsigned int) _temp1291 > 4u?(( struct _tunion_struct*) _temp1291)->tag ==
Cyc_Absyn_PointerType_tag: 0){ _LL1298: _temp1297=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1291)->f1; _LL1308: _temp1307=( void*)
_temp1297.elt_typ; goto _LL1306; _LL1306: _temp1305=( void*) _temp1297.rgn_typ;
goto _LL1304; _LL1304: _temp1303=( struct Cyc_Absyn_Conref*) _temp1297.nullable;
goto _LL1302; _LL1302: _temp1301=( struct Cyc_Absyn_Tqual*) _temp1297.tq; goto
_LL1300; _LL1300: _temp1299=( struct Cyc_Absyn_Conref*) _temp1297.bounds; goto
_LL1294;} else{ goto _LL1295;} _LL1295: goto _LL1296; _LL1294: if(( void*)
_temp1299->v ==( void*) Cyc_Absyn_No_constr){( void*)( e->r=( void*)(( void*)(
Cyc_Absyn_signed_int_exp( 0, 0))->r)); goto _LL1292;}{ void* _temp1309=(( void*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp1299); struct Cyc_Absyn_Exp*
_temp1315; _LL1311: if(( int) _temp1309 == Cyc_Absyn_Unknown_b){ goto _LL1312;}
else{ goto _LL1313;} _LL1313: if(( unsigned int) _temp1309 > 1u?(( struct
_tunion_struct*) _temp1309)->tag == Cyc_Absyn_Upper_b_tag: 0){ _LL1316:
_temp1315=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp1309)->f1;
goto _LL1314;} else{ goto _LL1310;} _LL1312: { struct _tuple7* zero=({ struct
_tuple7* _temp1317=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7));
_temp1317->f1= 0; _temp1317->f2= Cyc_Absyn_signed_int_exp( 0, 0); _temp1317;});
struct Cyc_Absyn_Exp* um= Cyc_Absyn_unresolvedmem_exp( 0,({ struct Cyc_List_List*
_temp1318=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1318->hd=( void*) zero; _temp1318->tl=({ struct Cyc_List_List* _temp1319=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1319->hd=(
void*) zero; _temp1319->tl=({ struct Cyc_List_List* _temp1320=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1320->hd=( void*) zero;
_temp1320->tl= 0; _temp1320;}); _temp1319;}); _temp1318;}), 0); if( nv->toplevel){(
void*)( e->r=( void*)(( void*) um->r));} else{ void* t= Cyc_Absyn_strct( Cyc_Toc_add_tagged_ptr_type(
Cyc_Toc_typ_to_c( _temp1307), Cyc_Toc_mt_tq)); struct _tuple1* x= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp( x, 0); struct Cyc_Absyn_Stmt* s=
Cyc_Absyn_declare_stmt( x, t,( struct Cyc_Absyn_Exp*) um, Cyc_Absyn_exp_stmt(
xexp, 0), 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r));}
goto _LL1310;} _LL1314:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_signed_int_exp(
0, 0))->r)); goto _LL1310; _LL1310:;} goto _LL1292; _LL1296:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1321= Cyc_Absynpp_typ2string(
old_typ); xprintf("exp_to_c: null given type %.*s", _temp1321.last_plus_one -
_temp1321.curr, _temp1321.curr);})); goto _LL1292; _LL1292:;} goto _LL1066;
_LL1070: goto _LL1066; _LL1072:{ struct _handler_cons _temp1322; _push_handler(&
_temp1322);{ struct _xtunion_struct* _temp1323=( struct _xtunion_struct*) setjmp(
_temp1322.handler); if( ! _temp1323){( void*)( e->r=( void*)(( void*)((( struct
Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict* d, struct _tuple1* key)) Cyc_Dict_lookup)(
nv->varmap, _temp1147))->r));; _pop_handler();} else{ struct _xtunion_struct*
_temp1325= _temp1323; _LL1327: if( _temp1325->tag == Cyc_Dict_Absent_tag){ goto
_LL1328;} else{ goto _LL1329;} _LL1329: goto _LL1330; _LL1328:(( void(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1331= Cyc_Absynpp_qvar2string(
_temp1147); xprintf("Can't find %.*s in exp_to_c, Var\n", _temp1331.last_plus_one
- _temp1331.curr, _temp1331.curr);})); return; _LL1330:( void) _throw( _temp1325);
_LL1326:;}}} goto _LL1066; _LL1074:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1332=( char*)"unknownid"; struct
_tagged_string _temp1333; _temp1333.curr= _temp1332; _temp1333.base= _temp1332;
_temp1333.last_plus_one= _temp1332 + 10; _temp1333;})); goto _LL1066; _LL1076: {
struct Cyc_List_List* old_typs=(( struct Cyc_List_List*(*)( void*(* f)( struct
Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_cyc_typ,
_temp1151); struct Cyc_List_List* typs=(( struct Cyc_List_List*(*)( void*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_get_c_typ,
_temp1151);(( void(*)( void(* f)( struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*),
struct Cyc_Toc_Env* env, struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c,
nv, _temp1151);{ void* _temp1334= _temp1153; _LL1336: if(( int) _temp1334 == Cyc_Absyn_Size){
goto _LL1337;} else{ goto _LL1338;} _LL1338: if(( int) _temp1334 == Cyc_Absyn_Printf){
goto _LL1339;} else{ goto _LL1340;} _LL1340: if(( int) _temp1334 == Cyc_Absyn_Xprintf){
goto _LL1341;} else{ goto _LL1342;} _LL1342: if(( int) _temp1334 == Cyc_Absyn_Fprintf){
goto _LL1343;} else{ goto _LL1344;} _LL1344: if(( int) _temp1334 == Cyc_Absyn_Sscanf){
goto _LL1345;} else{ goto _LL1346;} _LL1346: if(( int) _temp1334 == Cyc_Absyn_Plus){
goto _LL1347;} else{ goto _LL1348;} _LL1348: if(( int) _temp1334 == Cyc_Absyn_Minus){
goto _LL1349;} else{ goto _LL1350;} _LL1350: if(( int) _temp1334 == Cyc_Absyn_Eq){
goto _LL1351;} else{ goto _LL1352;} _LL1352: if(( int) _temp1334 == Cyc_Absyn_Neq){
goto _LL1353;} else{ goto _LL1354;} _LL1354: if(( int) _temp1334 == Cyc_Absyn_Gt){
goto _LL1355;} else{ goto _LL1356;} _LL1356: if(( int) _temp1334 == Cyc_Absyn_Gte){
goto _LL1357;} else{ goto _LL1358;} _LL1358: if(( int) _temp1334 == Cyc_Absyn_Lt){
goto _LL1359;} else{ goto _LL1360;} _LL1360: if(( int) _temp1334 == Cyc_Absyn_Lte){
goto _LL1361;} else{ goto _LL1362;} _LL1362: goto _LL1363; _LL1337: { struct Cyc_Absyn_Exp*
arg=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1364= _temp1151; if(
_temp1364 == 0){ _throw( Null_Exception);} _temp1364->hd;});{ void* _temp1366=(
void*)({ struct Cyc_Core_Opt* _temp1365= arg->topt; if( _temp1365 == 0){ _throw(
Null_Exception);} _temp1365->v;}); struct Cyc_Absyn_Exp* _temp1374; struct Cyc_Absyn_Tqual*
_temp1376; void* _temp1378; struct Cyc_Absyn_PtrInfo _temp1380; struct Cyc_Absyn_Conref*
_temp1382; struct Cyc_Absyn_Tqual* _temp1384; struct Cyc_Absyn_Conref* _temp1386;
void* _temp1388; void* _temp1390; _LL1368: if(( unsigned int) _temp1366 > 4u?((
struct _tunion_struct*) _temp1366)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL1379:
_temp1378=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1366)->f1; goto
_LL1377; _LL1377: _temp1376=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1366)->f2; goto _LL1375; _LL1375: _temp1374=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1366)->f3; goto _LL1369;} else{ goto
_LL1370;} _LL1370: if(( unsigned int) _temp1366 > 4u?(( struct _tunion_struct*)
_temp1366)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1381: _temp1380=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp1366)->f1;
_LL1391: _temp1390=( void*) _temp1380.elt_typ; goto _LL1389; _LL1389: _temp1388=(
void*) _temp1380.rgn_typ; goto _LL1387; _LL1387: _temp1386=( struct Cyc_Absyn_Conref*)
_temp1380.nullable; goto _LL1385; _LL1385: _temp1384=( struct Cyc_Absyn_Tqual*)
_temp1380.tq; goto _LL1383; _LL1383: _temp1382=( struct Cyc_Absyn_Conref*)
_temp1380.bounds; goto _LL1371;} else{ goto _LL1372;} _LL1372: goto _LL1373;
_LL1369:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Exp* _temp1392=
_temp1374; if( _temp1392 == 0){ _throw( Null_Exception);} _temp1392->r;})));
goto _LL1367; _LL1371: { struct _tuple1* temp= Cyc_Toc_temp_var(); void*
temp_typ= Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Core_Opt* _temp1393= arg->topt;
if( _temp1393 == 0){ _throw( Null_Exception);} _temp1393->v;})); struct Cyc_Absyn_Exp*
e2= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_last_plus_one_sp,
0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
temp, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Exp* e4= Cyc_Absyn_cast_exp( Cyc_Absyn_uint_t,
Cyc_Absyn_subtract_exp( e2, e3, 0), 0); struct Cyc_Absyn_Stmt* result= Cyc_Absyn_exp_stmt(
e4, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
temp, temp_typ,( struct Cyc_Absyn_Exp*) arg, result, 0), 0))->r)); goto _LL1367;}
_LL1373:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct
_tagged_string _temp1396= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_List_List*
_temp1394= typs; if( _temp1394 == 0){ _throw( Null_Exception);} _temp1394->hd;}));
struct _tagged_string _temp1397= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp1395= arg->topt; if( _temp1395 == 0){ _throw( Null_Exception);} _temp1395->v;}));
xprintf("size primop applied to non-array %.*s (%.*s)", _temp1396.last_plus_one
- _temp1396.curr, _temp1396.curr, _temp1397.last_plus_one - _temp1397.curr,
_temp1397.curr);})); return; _LL1367:;} goto _LL1335;} _LL1339:( void*)( e->r=(
void*)(( void*)( Cyc_Toc_printf_to_c( _temp1153, 0, _temp1151, typs))->r)); goto
_LL1335; _LL1341:( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp1153, 0, _temp1151, typs))->r)); goto _LL1335; _LL1343: if( _temp1151 == 0){((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
char* _temp1398=( char*)"fprintf without arguments"; struct _tagged_string
_temp1399; _temp1399.curr= _temp1398; _temp1399.base= _temp1398; _temp1399.last_plus_one=
_temp1398 + 26; _temp1399;}));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_printf_to_c(
_temp1153,({ struct Cyc_Core_Opt* _temp1400=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1400->v=( void*)(( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1401= _temp1151; if( _temp1401 == 0){ _throw(
Null_Exception);} _temp1401->hd;})); _temp1400;}),({ struct Cyc_List_List*
_temp1402= _temp1151; if( _temp1402 == 0){ _throw( Null_Exception);} _temp1402->tl;}),({
struct Cyc_List_List* _temp1403= typs; if( _temp1403 == 0){ _throw(
Null_Exception);} _temp1403->tl;})))->r)); goto _LL1335; _LL1345: if( _temp1151
== 0){(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp1404=( char*)"sscanf without arguments"; struct
_tagged_string _temp1405; _temp1405.curr= _temp1404; _temp1405.base= _temp1404;
_temp1405.last_plus_one= _temp1404 + 25; _temp1405;}));}( void*)((( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1406= _temp1151; if( _temp1406 == 0){ _throw(
Null_Exception);} _temp1406->hd;}))->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*)(( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1407= _temp1151; if( _temp1407 == 0){ _throw( Null_Exception);} _temp1407->hd;}))->r,((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1408= _temp1151; if(
_temp1408 == 0){ _throw( Null_Exception);} _temp1408->hd;}))->loc), Cyc_Toc_curr_sp,
0))->r)); goto _LL1335; _LL1347: if( Cyc_Tcutil_is_tagged_pointer_typ( old_typ)){
struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1409= _temp1151; if( _temp1409 == 0){ _throw( Null_Exception);} _temp1409->hd;});
struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1411=({ struct Cyc_List_List* _temp1410= _temp1151; if( _temp1410 == 0){
_throw( Null_Exception);} _temp1410->tl;}); if( _temp1411 == 0){ _throw(
Null_Exception);} _temp1411->hd;}); struct _tuple1* temp= Cyc_Toc_temp_var();
void* temp_typ= Cyc_Toc_typ_to_c( old_typ); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp( temp, 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Exp* e4= Cyc_Absyn_assignop_exp(
e3,({ struct Cyc_Core_Opt* _temp1412=( struct Cyc_Core_Opt*) GC_malloc( sizeof(
struct Cyc_Core_Opt)); _temp1412->v=( void*)(( void*) Cyc_Absyn_Plus); _temp1412;}),
e2, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( e4, 0), s, 0); s= Cyc_Absyn_declare_stmt(
temp, temp_typ,( struct Cyc_Absyn_Exp*) e1, s, 0);( void*)( e->r=( void*)(( void*)(
Cyc_Absyn_stmt_exp( s, 0))->r));} goto _LL1335; _LL1349: if( Cyc_Tcutil_is_tagged_pointer_typ((
void*)({ struct Cyc_List_List* _temp1413= old_typs; if( _temp1413 == 0){ _throw(
Null_Exception);} _temp1413->hd;}))){ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1414= _temp1151; if( _temp1414 == 0){ _throw(
Null_Exception);} _temp1414->hd;}); struct Cyc_Absyn_Exp* e2=( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1416=({ struct Cyc_List_List* _temp1415= _temp1151;
if( _temp1415 == 0){ _throw( Null_Exception);} _temp1415->tl;}); if( _temp1416
== 0){ _throw( Null_Exception);} _temp1416->hd;}); void* old_typ_e2=( void*)({
struct Cyc_List_List* _temp1418=({ struct Cyc_List_List* _temp1417= old_typs;
if( _temp1417 == 0){ _throw( Null_Exception);} _temp1417->tl;}); if( _temp1418
== 0){ _throw( Null_Exception);} _temp1418->hd;}); if( Cyc_Tcutil_is_tagged_pointer_typ(
old_typ_e2)){( void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));( void*)( e2->r=( void*)(( void*)(
Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*) e2->r, 0), Cyc_Toc_curr_sp,
0))->r));} else{ struct _tuple1* temp= Cyc_Toc_temp_var(); void* temp_typ= Cyc_Toc_typ_to_c((
void*)({ struct Cyc_List_List* _temp1419= old_typs; if( _temp1419 == 0){ _throw(
Null_Exception);} _temp1419->hd;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp( temp, 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0); struct Cyc_Absyn_Exp* e4= Cyc_Absyn_assignop_exp(
e3,({ struct Cyc_Core_Opt* _temp1420=( struct Cyc_Core_Opt*) GC_malloc( sizeof(
struct Cyc_Core_Opt)); _temp1420->v=( void*)(( void*) Cyc_Absyn_Minus);
_temp1420;}), e2, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt( e4, 0), s, 0);
s= Cyc_Absyn_declare_stmt( temp, temp_typ,( struct Cyc_Absyn_Exp*) e1, s, 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r));}} goto _LL1335;
_LL1351: goto _LL1353; _LL1353: goto _LL1355; _LL1355: goto _LL1357; _LL1357:
goto _LL1359; _LL1359: goto _LL1361; _LL1361: { struct Cyc_Absyn_Exp* e1=(
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1421= _temp1151; if(
_temp1421 == 0){ _throw( Null_Exception);} _temp1421->hd;}); struct Cyc_Absyn_Exp*
e2=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1423=({ struct Cyc_List_List*
_temp1422= _temp1151; if( _temp1422 == 0){ _throw( Null_Exception);} _temp1422->tl;});
if( _temp1423 == 0){ _throw( Null_Exception);} _temp1423->hd;}); void* t1=( void*)({
struct Cyc_List_List* _temp1424= old_typs; if( _temp1424 == 0){ _throw(
Null_Exception);} _temp1424->hd;}); void* t2=( void*)({ struct Cyc_List_List*
_temp1426=({ struct Cyc_List_List* _temp1425= old_typs; if( _temp1425 == 0){
_throw( Null_Exception);} _temp1425->tl;}); if( _temp1426 == 0){ _throw(
Null_Exception);} _temp1426->hd;}); if( Cyc_Tcutil_is_tagged_pointer_typ( t1)){(
void*)( e1->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e1->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){( void*)( e2->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp((
void*) e2->r, 0), Cyc_Toc_curr_sp, 0))->r));} goto _LL1335;} _LL1363: goto
_LL1335; _LL1335:;} goto _LL1066;} _LL1078: { void* e2_cyc_typ=( void*)({ struct
Cyc_Core_Opt* _temp1427= _temp1157->topt; if( _temp1427 == 0){ _throw(
Null_Exception);} _temp1427->v;}); Cyc_Toc_exp_to_c( nv, _temp1157); if( Cyc_Tcutil_is_tagged_pointer_typ(
e2_cyc_typ)){ if( _temp1155 ==( void*) Cyc_Absyn_PreInc? 1: _temp1155 ==( void*)
Cyc_Absyn_PreDec){ struct _tuple1* temp= Cyc_Toc_temp_var(); void* temp_typ= Cyc_Absyn_star_typ(
Cyc_Toc_typ_to_c( e2_cyc_typ),( void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual());
struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp(
temp, 0), 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_increment_exp( Cyc_Absyn_structarrow_exp(
Cyc_Absyn_var_exp( temp, 0), Cyc_Toc_curr_sp, 0), _temp1155, 0); s= Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( e3, 0), s, 0); s= Cyc_Absyn_declare_stmt( temp, temp_typ,(
struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( _temp1157, 0), s, 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r));} else{ struct _tuple1* temp=
Cyc_Toc_temp_var(); struct _tuple1* temp2= Cyc_Toc_temp_var(); void* temp2_typ=
Cyc_Toc_typ_to_c( e2_cyc_typ); void* temp_typ= Cyc_Absyn_star_typ( Cyc_Toc_typ_to_c(
e2_cyc_typ),( void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual()); struct Cyc_Absyn_Stmt*
s= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( temp2, 0), 0); struct Cyc_Absyn_Exp*
e3= Cyc_Absyn_increment_exp( Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( temp,
0), Cyc_Toc_curr_sp, 0), _temp1155, 0); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
e3, 0), s, 0); s= Cyc_Absyn_declare_stmt( temp2, temp2_typ,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( temp, 0), 0), s, 0); s= Cyc_Absyn_declare_stmt(
temp, temp_typ,( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( _temp1157, 0), s,
0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( s, 0))->r));}} else{
if( ! Cyc_Absyn_is_lvalue( _temp1157)){(( void(*)( struct Cyc_Absyn_Exp* e1,
struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*,
void*), void* f_env)) Cyc_Toc_lvalue_assign)( _temp1157, 0, Cyc_Toc_incr_lvalue,
_temp1155);( void*)( e->r=( void*)(( void*) _temp1157->r));}} goto _LL1066;}
_LL1080: { int e1_poly= Cyc_Toc_is_poly_project( _temp1163); void* e1_old_typ=(
void*)({ struct Cyc_Core_Opt* _temp1428= _temp1163->topt; if( _temp1428 == 0){
_throw( Null_Exception);} _temp1428->v;}); void* e2_old_typ=( void*)({ struct
Cyc_Core_Opt* _temp1429= _temp1159->topt; if( _temp1429 == 0){ _throw(
Null_Exception);} _temp1429->v;}); Cyc_Toc_exp_to_c( nv, _temp1163); Cyc_Toc_exp_to_c(
nv, _temp1159); if( _temp1161 != 0?( void*)({ struct Cyc_Core_Opt* _temp1430=
_temp1161; if( _temp1430 == 0){ _throw( Null_Exception);} _temp1430->v;}) ==(
void*) Cyc_Absyn_Plus? 1:( void*)({ struct Cyc_Core_Opt* _temp1431= _temp1161;
if( _temp1431 == 0){ _throw( Null_Exception);} _temp1431->v;}) ==( void*) Cyc_Absyn_Minus:
0){ if( Cyc_Tcutil_is_tagged_pointer_typ( e1_old_typ)){( void*)( _temp1163->r=(
void*)(( void*)( Cyc_Absyn_structmember_exp( Cyc_Absyn_new_exp(( void*)
_temp1163->r, 0), Cyc_Toc_curr_sp, 0))->r));} if( Cyc_Tcutil_is_tagged_pointer_typ(
e2_old_typ)){( void*)( _temp1159->r=( void*)(( void*)( Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp(( void*) _temp1159->r, 0), Cyc_Toc_curr_sp, 0))->r));}} if(
e1_poly){( void*)( _temp1159->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1159->r, 0), 0))->r));} if( ! Cyc_Absyn_is_lvalue(
_temp1163)){(( void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs,
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Absyn_Exp*, struct _tuple11*), struct
_tuple11* f_env)) Cyc_Toc_lvalue_assign)( _temp1163, 0, Cyc_Toc_assignop_lvalue,({
struct _tuple11* _temp1432=( struct _tuple11*) GC_malloc( sizeof( struct
_tuple11) * 1); _temp1432[ 0]=({ struct _tuple11 _temp1433; _temp1433.f1=
_temp1161; _temp1433.f2= _temp1159; _temp1433;}); _temp1432;}));( void*)( e->r=(
void*)(( void*) _temp1163->r));} goto _LL1066;} _LL1082: Cyc_Toc_exp_to_c( nv,
_temp1169); Cyc_Toc_exp_to_c( nv, _temp1167); Cyc_Toc_exp_to_c( nv, _temp1165);
goto _LL1066; _LL1084: Cyc_Toc_exp_to_c( nv, _temp1173); Cyc_Toc_exp_to_c( nv,
_temp1171); goto _LL1066; _LL1086: _temp1181= _temp1177; _temp1179= _temp1175;
goto _LL1088; _LL1088: Cyc_Toc_exp_to_c( nv, _temp1181);(( void(*)( void(* f)(
struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List*
x)) Cyc_List_iter_c)( Cyc_Toc_exp_to_c, nv, _temp1179); goto _LL1066; _LL1090:
Cyc_Toc_exp_to_c( nv, _temp1183);( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c( old_typ), Cyc_Toc_newthrow_exp( _temp1183), 0))->r)); goto
_LL1066; _LL1092: Cyc_Toc_exp_to_c( nv, _temp1185); goto _LL1066; _LL1094: Cyc_Toc_exp_to_c(
nv, _temp1189);{ void* t_inst= Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Core_Opt*
_temp1434= e->topt; if( _temp1434 == 0){ _throw( Null_Exception);} _temp1434->v;}));
for( 0; _temp1187 != 0; _temp1187=({ struct Cyc_List_List* _temp1435= _temp1187;
if( _temp1435 == 0){ _throw( Null_Exception);} _temp1435->tl;})){ void* k= Cyc_Tcutil_typ_kind((
void*)({ struct Cyc_List_List* _temp1436= _temp1187; if( _temp1436 == 0){ _throw(
Null_Exception);} _temp1436->hd;})); if( k !=( void*) Cyc_Absyn_EffKind? k !=(
void*) Cyc_Absyn_RgnKind: 0){( void*)( e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast(
t_inst, _temp1189, 0))->r)); break;}} goto _LL1066;} _LL1096: { void* old_t2=(
void*)({ struct Cyc_Core_Opt* _temp1437=(* _temp1193)->topt; if( _temp1437 == 0){
_throw( Null_Exception);} _temp1437->v;}); void* new_typ=* _temp1196;* _temp1196=
Cyc_Toc_typ_to_c( new_typ); Cyc_Toc_exp_to_c( nv,* _temp1193);* _temp1193= Cyc_Toc_convert_exp(
nv->toplevel, old_t2, new_typ,* _temp1193); goto _LL1066;} _LL1098:{ void*
_temp1438=( void*) _temp1197->r; struct Cyc_Absyn_Structdecl* _temp1446; struct
Cyc_List_List* _temp1448; struct Cyc_Core_Opt* _temp1450; struct _tuple1*
_temp1452; struct Cyc_List_List* _temp1454; _LL1440: if((( struct _tunion_struct*)
_temp1438)->tag == Cyc_Absyn_Struct_e_tag){ _LL1453: _temp1452=( struct _tuple1*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1438)->f1; goto _LL1451; _LL1451:
_temp1450=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*) _temp1438)->f2;
goto _LL1449; _LL1449: _temp1448=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1438)->f3; goto _LL1447; _LL1447: _temp1446=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1438)->f4; goto _LL1441;} else{ goto
_LL1442;} _LL1442: if((( struct _tunion_struct*) _temp1438)->tag == Cyc_Absyn_Tuple_e_tag){
_LL1455: _temp1454=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1438)->f1; goto _LL1443;} else{ goto _LL1444;} _LL1444: goto _LL1445;
_LL1441: if( nv->toplevel){(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(({
struct _tagged_string _temp1456= Cyc_Position_string_of_segment( _temp1197->loc);
xprintf("%.*s: & on non-identifiers at the top-level", _temp1456.last_plus_one -
_temp1456.curr, _temp1456.curr);}));}( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_struct(
nv,( void*)({ struct Cyc_Core_Opt* _temp1457= _temp1197->topt; if( _temp1457 ==
0){ _throw( Null_Exception);} _temp1457->v;}), 1, 0, _temp1448, _temp1452))->r));
goto _LL1439; _LL1443: if( nv->toplevel){(( void(*)( struct _tagged_string s))
Cyc_Toc_unimp)(({ struct _tagged_string _temp1458= Cyc_Position_string_of_segment(
_temp1197->loc); xprintf("%.*s: & on non-identifiers at the top-level",
_temp1458.last_plus_one - _temp1458.curr, _temp1458.curr);}));}( void*)( e->r=(
void*)(( void*)( Cyc_Toc_init_tuple( nv, 1, 0, _temp1454))->r)); goto _LL1439;
_LL1445: Cyc_Toc_exp_to_c( nv, _temp1197); if( ! Cyc_Absyn_is_lvalue( _temp1197)){((
void(*)( struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* fs, struct Cyc_Absyn_Exp*(*
f)( struct Cyc_Absyn_Exp*, int), int f_env)) Cyc_Toc_lvalue_assign)( _temp1197,
0, Cyc_Toc_address_lvalue, 1);( void*)( e->r=( void*)(( void*) _temp1197->r));}
goto _LL1439; _LL1439:;} goto _LL1066; _LL1100: if( nv->toplevel){(( void(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(({ struct _tagged_string _temp1459= Cyc_Position_string_of_segment(
_temp1199->loc); xprintf("%.*s: new at top-level", _temp1459.last_plus_one -
_temp1459.curr, _temp1459.curr);}));}{ void* _temp1460=( void*) _temp1199->r;
struct Cyc_List_List* _temp1474; void* _temp1476; struct _tagged_string
_temp1478; struct Cyc_Absyn_Exp* _temp1480; struct Cyc_Absyn_Exp* _temp1482;
struct Cyc_Absyn_Vardecl* _temp1484; struct Cyc_Absyn_Structdecl* _temp1486;
struct Cyc_List_List* _temp1488; struct Cyc_Core_Opt* _temp1490; struct _tuple1*
_temp1492; struct Cyc_List_List* _temp1494; _LL1462: if((( struct _tunion_struct*)
_temp1460)->tag == Cyc_Absyn_Array_e_tag){ _LL1475: _temp1474=( struct Cyc_List_List*)((
struct Cyc_Absyn_Array_e_struct*) _temp1460)->f1; goto _LL1463;} else{ goto
_LL1464;} _LL1464: if((( struct _tunion_struct*) _temp1460)->tag == Cyc_Absyn_Const_e_tag){
_LL1477: _temp1476=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1460)->f1;
if(( unsigned int) _temp1476 > 1u?(( struct _tunion_struct*) _temp1476)->tag ==
Cyc_Absyn_String_c_tag: 0){ _LL1479: _temp1478=( struct _tagged_string)(( struct
Cyc_Absyn_String_c_struct*) _temp1476)->f1; goto _LL1465;} else{ goto _LL1466;}}
else{ goto _LL1466;} _LL1466: if((( struct _tunion_struct*) _temp1460)->tag ==
Cyc_Absyn_Comprehension_e_tag){ _LL1485: _temp1484=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1460)->f1; goto _LL1483; _LL1483:
_temp1482=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1460)->f2; goto _LL1481; _LL1481: _temp1480=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1460)->f3; goto _LL1467;} else{
goto _LL1468;} _LL1468: if((( struct _tunion_struct*) _temp1460)->tag == Cyc_Absyn_Struct_e_tag){
_LL1493: _temp1492=( struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1460)->f1; goto _LL1491; _LL1491: _temp1490=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1460)->f2; goto _LL1489; _LL1489:
_temp1488=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1460)->f3; goto _LL1487; _LL1487: _temp1486=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1460)->f4; goto _LL1469;} else{ goto
_LL1470;} _LL1470: if((( struct _tunion_struct*) _temp1460)->tag == Cyc_Absyn_Tuple_e_tag){
_LL1495: _temp1494=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1460)->f1; goto _LL1471;} else{ goto _LL1472;} _LL1472: goto _LL1473;
_LL1463: { struct _tuple1* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp( x, 0), 0); int count=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp1474) - 1;{ struct Cyc_List_List* dles=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( _temp1474); for( 0; dles != 0; dles=({ struct
Cyc_List_List* _temp1496= dles; if( _temp1496 == 0){ _throw( Null_Exception);}
_temp1496->tl;})){ struct _tuple7 _temp1500; struct Cyc_Absyn_Exp* _temp1501;
struct Cyc_List_List* _temp1503; struct _tuple7* _temp1498=( struct _tuple7*)({
struct Cyc_List_List* _temp1497= dles; if( _temp1497 == 0){ _throw(
Null_Exception);} _temp1497->hd;}); _temp1500=* _temp1498; _LL1504: _temp1503=
_temp1500.f1; goto _LL1502; _LL1502: _temp1501= _temp1500.f2; goto _LL1499;
_LL1499: { struct Cyc_Absyn_Exp* e_index; if( _temp1503 == 0){ e_index= Cyc_Absyn_signed_int_exp((
count --), 0);} else{ if(({ struct Cyc_List_List* _temp1505= _temp1503; if(
_temp1505 == 0){ _throw( Null_Exception);} _temp1505->tl;}) != 0){(( void(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char*
_temp1506=( char*)"multiple designators in array"; struct _tagged_string
_temp1507; _temp1507.curr= _temp1506; _temp1507.base= _temp1506; _temp1507.last_plus_one=
_temp1506 + 30; _temp1507;}));}{ void* d=( void*)({ struct Cyc_List_List*
_temp1508= _temp1503; if( _temp1508 == 0){ _throw( Null_Exception);} _temp1508->hd;});
void* _temp1509= d; struct Cyc_Absyn_Exp* _temp1515; struct _tagged_string*
_temp1517; _LL1511: if((( struct _tunion_struct*) _temp1509)->tag == Cyc_Absyn_ArrayElement_tag){
_LL1516: _temp1515=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayElement_struct*)
_temp1509)->f1; goto _LL1512;} else{ goto _LL1513;} _LL1513: if((( struct
_tunion_struct*) _temp1509)->tag == Cyc_Absyn_FieldName_tag){ _LL1518: _temp1517=(
struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp1509)->f1;
goto _LL1514;} else{ goto _LL1510;} _LL1512: Cyc_Toc_exp_to_c( nv, _temp1515);
e_index= _temp1515; goto _LL1510; _LL1514: e_index=(( struct Cyc_Absyn_Exp*(*)(
struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char*
_temp1519=( char*)"field name designators in array"; struct _tagged_string
_temp1520; _temp1520.curr= _temp1519; _temp1520.base= _temp1519; _temp1520.last_plus_one=
_temp1519 + 32; _temp1520;})); goto _LL1510; _LL1510:;}} Cyc_Toc_exp_to_c( nv,
_temp1501); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
Cyc_Absyn_var_exp( x, 0), e_index, 0), _temp1501, 0), s, 0);}}}{ void*
old_elt_typ;{ void* _temp1521= Cyc_Tcutil_compress( old_typ); struct Cyc_Absyn_PtrInfo
_temp1527; struct Cyc_Absyn_Conref* _temp1529; struct Cyc_Absyn_Tqual* _temp1531;
struct Cyc_Absyn_Conref* _temp1533; void* _temp1535; void* _temp1537; _LL1523:
if(( unsigned int) _temp1521 > 4u?(( struct _tunion_struct*) _temp1521)->tag ==
Cyc_Absyn_PointerType_tag: 0){ _LL1528: _temp1527=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1521)->f1; _LL1538: _temp1537=( void*)
_temp1527.elt_typ; goto _LL1536; _LL1536: _temp1535=( void*) _temp1527.rgn_typ;
goto _LL1534; _LL1534: _temp1533=( struct Cyc_Absyn_Conref*) _temp1527.nullable;
goto _LL1532; _LL1532: _temp1531=( struct Cyc_Absyn_Tqual*) _temp1527.tq; goto
_LL1530; _LL1530: _temp1529=( struct Cyc_Absyn_Conref*) _temp1527.bounds; goto
_LL1524;} else{ goto _LL1525;} _LL1525: goto _LL1526; _LL1524: old_elt_typ=
_temp1537; goto _LL1522; _LL1526: old_elt_typ=(( void*(*)( struct _tagged_string
s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1539=( char*)"exp_to_c:new array expression doesn't have ptr type";
struct _tagged_string _temp1540; _temp1540.curr= _temp1539; _temp1540.base=
_temp1539; _temp1540.last_plus_one= _temp1539 + 52; _temp1540;})); goto _LL1522;
_LL1522:;}{ void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp(
elt_typ, 0), Cyc_Absyn_signed_int_exp((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp1474), 0), 0); struct Cyc_Absyn_Exp* e1; if( _temp1201 == 0){ e1= Cyc_Toc_malloc_exp(
old_elt_typ, e2);} else{ struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1541= _temp1201; if( _temp1541 == 0){ _throw(
Null_Exception);} _temp1541;}); Cyc_Toc_exp_to_c( nv, r); e1= Cyc_Toc_rmalloc_exp(
r, e2);}{ struct Cyc_Absyn_Exp* e0= Cyc_Absyn_cast_exp( ptr_typ, e1, 0);( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, ptr_typ,(
struct Cyc_Absyn_Exp*) e0, s, 0), 0))->r)); goto _LL1461;}}}} _LL1465: if(
_temp1201 == 0){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(({
struct _tuple1* _temp1542=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp1542->f1= Cyc_Toc_abs_ns; _temp1542->f2= Cyc_Toc_Cyc_new_string_sp;
_temp1542;}), 0),({ struct Cyc_List_List* _temp1543=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1543->hd=( void*) Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp1544=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp1544[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp1545; _temp1545.tag= Cyc_Absyn_Const_e_tag;
_temp1545.f1=( void*)(( void*)({ struct Cyc_Absyn_String_c_struct* _temp1546=(
struct Cyc_Absyn_String_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_String_c_struct));
_temp1546[ 0]=({ struct Cyc_Absyn_String_c_struct _temp1547; _temp1547.tag= Cyc_Absyn_String_c_tag;
_temp1547.f1= _temp1478; _temp1547;}); _temp1546;})); _temp1545;}); _temp1544;}),
0); _temp1543->tl= 0; _temp1543;}), 0))->r));} else{ struct Cyc_Absyn_Exp* r=(
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1548= _temp1201; if(
_temp1548 == 0){ _throw( Null_Exception);} _temp1548;}); Cyc_Toc_exp_to_c( nv, r);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp(({
struct _tuple1* _temp1549=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp1549->f1= Cyc_Toc_abs_ns; _temp1549->f2= Cyc_Toc_Cyc_rnew_string_sp;
_temp1549;}), 0),({ struct Cyc_List_List* _temp1550=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1550->hd=( void*) Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp1551=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp1551[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp1552; _temp1552.tag= Cyc_Absyn_Const_e_tag;
_temp1552.f1=( void*)(( void*)({ struct Cyc_Absyn_String_c_struct* _temp1553=(
struct Cyc_Absyn_String_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_String_c_struct));
_temp1553[ 0]=({ struct Cyc_Absyn_String_c_struct _temp1554; _temp1554.tag= Cyc_Absyn_String_c_tag;
_temp1554.f1= _temp1478; _temp1554;}); _temp1553;})); _temp1552;}); _temp1551;}),
0); _temp1550->tl= 0; _temp1550;}), 0))->r));} goto _LL1461; _LL1467: { struct
_tuple1* x1= _temp1484->name; int is_tagged_ptr= 0;{ void* _temp1555= Cyc_Tcutil_compress(
old_typ); struct Cyc_Absyn_PtrInfo _temp1561; struct Cyc_Absyn_Conref* _temp1563;
struct Cyc_Absyn_Tqual* _temp1565; struct Cyc_Absyn_Conref* _temp1567; void*
_temp1569; void* _temp1571; _LL1557: if(( unsigned int) _temp1555 > 4u?(( struct
_tunion_struct*) _temp1555)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1562:
_temp1561=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1555)->f1; _LL1572: _temp1571=( void*) _temp1561.elt_typ; goto _LL1570;
_LL1570: _temp1569=( void*) _temp1561.rgn_typ; goto _LL1568; _LL1568: _temp1567=(
struct Cyc_Absyn_Conref*) _temp1561.nullable; goto _LL1566; _LL1566: _temp1565=(
struct Cyc_Absyn_Tqual*) _temp1561.tq; goto _LL1564; _LL1564: _temp1563=( struct
Cyc_Absyn_Conref*) _temp1561.bounds; goto _LL1558;} else{ goto _LL1559;} _LL1559:
goto _LL1560; _LL1558:{ void* _temp1573=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp1563))->v; void*
_temp1579; _LL1575: if(( unsigned int) _temp1573 > 1u?(( struct _tunion_struct*)
_temp1573)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL1580: _temp1579=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp1573)->f1; if(( int) _temp1579 == Cyc_Absyn_Unknown_b){
goto _LL1576;} else{ goto _LL1577;}} else{ goto _LL1577;} _LL1577: goto _LL1578;
_LL1576: is_tagged_ptr= 1; goto _LL1574; _LL1578: goto _LL1574; _LL1574:;} goto
_LL1556; _LL1560:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1581=( char*)"exp_to_c: comprehension not an array type";
struct _tagged_string _temp1582; _temp1582.curr= _temp1581; _temp1582.base=
_temp1581; _temp1582.last_plus_one= _temp1581 + 42; _temp1582;})); goto _LL1556;
_LL1556:;}{ void* old_elt_typ=( void*)({ struct Cyc_Core_Opt* _temp1583=
_temp1480->topt; if( _temp1583 == 0){ _throw( Null_Exception);} _temp1583->v;});
void* elt_typ= Cyc_Toc_typ_to_c( old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ(
elt_typ, Cyc_Toc_mt_tq); Cyc_Toc_exp_to_c( nv, _temp1482);{ struct Cyc_Toc_Env*
nv2= Cyc_Toc_add_varmap( nv, x1, Cyc_Absyn_varb_exp( x1,( void*)({ struct Cyc_Absyn_Local_b_struct*
_temp1584=( struct Cyc_Absyn_Local_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp1584[ 0]=({ struct Cyc_Absyn_Local_b_struct _temp1585; _temp1585.tag= Cyc_Absyn_Local_b_tag;
_temp1585.f1= _temp1484; _temp1585;}); _temp1584;}), 0)); Cyc_Toc_exp_to_c( nv2,
_temp1480);{ struct _tuple1* max= Cyc_Toc_temp_var(); struct _tuple1* a= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* ea= Cyc_Absyn_assign_exp( Cyc_Absyn_var_exp( x1, 0), Cyc_Absyn_signed_int_exp(
0, 0), 0); struct Cyc_Absyn_Exp* eb= Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp( x1, 0),
Cyc_Absyn_var_exp( max, 0), 0); struct Cyc_Absyn_Exp* ec= Cyc_Absyn_post_inc_exp(
Cyc_Absyn_var_exp( x1, 0), 0); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt( ea,
eb, ec, Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp( Cyc_Absyn_var_exp( a, 0),
Cyc_Absyn_var_exp( x1, 0), 0), _temp1480, 0), 0); struct Cyc_List_List* decls=({
struct Cyc_List_List* _temp1586=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1586->hd=( void*)({ struct _tuple10* _temp1587=(
struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp1587->f1= max;
_temp1587->f2= Cyc_Absyn_uint_t; _temp1587->f3=( struct Cyc_Absyn_Exp*)
_temp1482; _temp1587;}); _temp1586->tl= 0; _temp1586;}); struct Cyc_Absyn_Exp*
ai; if( _temp1201 == 0){ ai= Cyc_Toc_malloc_exp( old_elt_typ, Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp( elt_typ, 0), Cyc_Absyn_var_exp( max, 0), 0));} else{
struct Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1588= _temp1201; if( _temp1588 == 0){ _throw( Null_Exception);} _temp1588;});
Cyc_Toc_exp_to_c( nv, r); ai= Cyc_Toc_rmalloc_exp( r, Cyc_Absyn_times_exp( Cyc_Absyn_sizeoftyp_exp(
elt_typ, 0), Cyc_Absyn_var_exp( max, 0), 0));}{ struct Cyc_Absyn_Exp* ainit= Cyc_Absyn_cast_exp(
ptr_typ, ai, 0); decls=({ struct Cyc_List_List* _temp1589=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1589->hd=( void*)({ struct
_tuple10* _temp1590=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10));
_temp1590->f1= a; _temp1590->f2= ptr_typ; _temp1590->f3=( struct Cyc_Absyn_Exp*)
ainit; _temp1590;}); _temp1589->tl= decls; _temp1589;}); decls=({ struct Cyc_List_List*
_temp1591=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1591->hd=( void*)({ struct _tuple10* _temp1592=( struct _tuple10*)
GC_malloc( sizeof( struct _tuple10)); _temp1592->f1= x1; _temp1592->f2= Cyc_Absyn_uint_t;
_temp1592->f3= 0; _temp1592;}); _temp1591->tl= decls; _temp1591;}); if(
is_tagged_ptr){ struct _tuple1* b= Cyc_Toc_temp_var(); void* t= Cyc_Toc_typ_to_c(
old_typ); struct Cyc_Absyn_Exp* binit= Cyc_Absyn_unresolvedmem_exp( 0,({ struct
Cyc_List_List* _temp1593=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1593->hd=( void*)({ struct _tuple7* _temp1598=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp1598->f1= 0; _temp1598->f2= Cyc_Absyn_var_exp( a,
0); _temp1598;}); _temp1593->tl=({ struct Cyc_List_List* _temp1594=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1594->hd=( void*)({ struct
_tuple7* _temp1597=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7));
_temp1597->f1= 0; _temp1597->f2= Cyc_Absyn_var_exp( a, 0); _temp1597;});
_temp1594->tl=({ struct Cyc_List_List* _temp1595=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1595->hd=( void*)({ struct
_tuple7* _temp1596=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7));
_temp1596->f1= 0; _temp1596->f2= Cyc_Absyn_add_exp( Cyc_Absyn_var_exp( a, 0),
Cyc_Absyn_var_exp( max, 0), 0); _temp1596;}); _temp1595->tl= 0; _temp1595;});
_temp1594;}); _temp1593;}), 0); decls=({ struct Cyc_List_List* _temp1599=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1599->hd=(
void*)({ struct _tuple10* _temp1600=( struct _tuple10*) GC_malloc( sizeof(
struct _tuple10)); _temp1600->f1= b; _temp1600->f2= t; _temp1600->f3=( struct
Cyc_Absyn_Exp*) binit; _temp1600;}); _temp1599->tl= decls; _temp1599;}); s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( b, 0), 0), 0);} else{ s= Cyc_Absyn_seq_stmt(
s, Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( a, 0), 0), 0);}{ struct Cyc_List_List*
ds= decls; for( 0; ds != 0; ds=({ struct Cyc_List_List* _temp1601= ds; if(
_temp1601 == 0){ _throw( Null_Exception);} _temp1601->tl;})){ struct Cyc_Absyn_Exp*
_temp1605; void* _temp1607; struct _tuple1* _temp1609; struct _tuple10 _temp1603=*((
struct _tuple10*)({ struct Cyc_List_List* _temp1602= ds; if( _temp1602 == 0){
_throw( Null_Exception);} _temp1602->hd;})); _LL1610: _temp1609= _temp1603.f1;
goto _LL1608; _LL1608: _temp1607= _temp1603.f2; goto _LL1606; _LL1606: _temp1605=
_temp1603.f3; goto _LL1604; _LL1604: s= Cyc_Absyn_declare_stmt( _temp1609,
_temp1607, _temp1605, s, 0);}}( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp(
s, 0))->r)); goto _LL1461;}}}}} _LL1469:( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_struct(
nv,( void*)({ struct Cyc_Core_Opt* _temp1611= _temp1199->topt; if( _temp1611 ==
0){ _throw( Null_Exception);} _temp1611->v;}), 1, _temp1201, _temp1488,
_temp1492))->r)); goto _LL1461; _LL1471:( void*)( e->r=( void*)(( void*)( Cyc_Toc_init_tuple(
nv, 1, _temp1201, _temp1494))->r)); goto _LL1461; _LL1473: { void* old_elt_typ=(
void*)({ struct Cyc_Core_Opt* _temp1612= _temp1199->topt; if( _temp1612 == 0){
_throw( Null_Exception);} _temp1612->v;}); struct _tuple1* x= Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt* s= Cyc_Absyn_exp_stmt( Cyc_Absyn_var_exp( x, 0), 0);
struct Cyc_Absyn_Exp* e_index= Cyc_Absyn_signed_int_exp( 0, 0); Cyc_Toc_exp_to_c(
nv, _temp1199); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_subscript_exp(
Cyc_Absyn_var_exp( x, 0), e_index, 0), _temp1199, 0), s, 0);{ void* elt_typ= Cyc_Toc_typ_to_c(
old_elt_typ); void* ptr_typ= Cyc_Absyn_cstar_typ( elt_typ, Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp* e2= Cyc_Absyn_sizeoftyp_exp( elt_typ, 0); struct Cyc_Absyn_Exp*
e1; if( _temp1201 == 0){ e1= Cyc_Toc_malloc_exp( old_elt_typ, e2);} else{ struct
Cyc_Absyn_Exp* r=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1613=
_temp1201; if( _temp1613 == 0){ _throw( Null_Exception);} _temp1613;}); Cyc_Toc_exp_to_c(
nv, r); e1= Cyc_Toc_rmalloc_exp( r, e2);}{ struct Cyc_Absyn_Exp* e0= Cyc_Absyn_cast_exp(
ptr_typ, e1, 0);( void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt(
x, ptr_typ,( struct Cyc_Absyn_Exp*) e0, s, 0), 0))->r)); goto _LL1461;}}}
_LL1461:;} goto _LL1066; _LL1102:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Sizeoftyp_e_struct*
_temp1614=( struct Cyc_Absyn_Sizeoftyp_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Sizeoftyp_e_struct));
_temp1614[ 0]=({ struct Cyc_Absyn_Sizeoftyp_e_struct _temp1615; _temp1615.tag=
Cyc_Absyn_Sizeoftyp_e_tag; _temp1615.f1=( void*) Cyc_Toc_typ_to_c_array(
_temp1203); _temp1615;}); _temp1614;}))); goto _LL1066; _LL1104: Cyc_Toc_exp_to_c(
nv, _temp1205); goto _LL1066; _LL1106: { void* e1_typ= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp1616= _temp1207->topt; if( _temp1616 == 0){
_throw( Null_Exception);} _temp1616->v;})); Cyc_Toc_exp_to_c( nv, _temp1207);{
void* _temp1617= e1_typ; struct Cyc_Absyn_PtrInfo _temp1623; struct Cyc_Absyn_Conref*
_temp1625; struct Cyc_Absyn_Tqual* _temp1627; struct Cyc_Absyn_Conref* _temp1629;
void* _temp1631; void* _temp1633; _LL1619: if(( unsigned int) _temp1617 > 4u?((
struct _tunion_struct*) _temp1617)->tag == Cyc_Absyn_PointerType_tag: 0){
_LL1624: _temp1623=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1617)->f1; _LL1634: _temp1633=( void*) _temp1623.elt_typ; goto _LL1632;
_LL1632: _temp1631=( void*) _temp1623.rgn_typ; goto _LL1630; _LL1630: _temp1629=(
struct Cyc_Absyn_Conref*) _temp1623.nullable; goto _LL1628; _LL1628: _temp1627=(
struct Cyc_Absyn_Tqual*) _temp1623.tq; goto _LL1626; _LL1626: _temp1625=( struct
Cyc_Absyn_Conref*) _temp1623.bounds; goto _LL1620;} else{ goto _LL1621;} _LL1621:
goto _LL1622; _LL1620:{ void* _temp1635=(( void*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_val)( _temp1625); struct Cyc_Absyn_Exp* _temp1641; _LL1637: if((
int) _temp1635 == Cyc_Absyn_Unknown_b){ goto _LL1638;} else{ goto _LL1639;}
_LL1639: if(( unsigned int) _temp1635 > 1u?(( struct _tunion_struct*) _temp1635)->tag
== Cyc_Absyn_Upper_b_tag: 0){ _LL1642: _temp1641=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Upper_b_struct*) _temp1635)->f1; goto _LL1640;} else{ goto
_LL1636;} _LL1638: { struct _tuple1* x= Cyc_Toc_temp_var(); struct _tuple1* a=
Cyc_Toc_temp_var(); void* ta1= Cyc_Toc_typ_to_c_array( _temp1633); struct
_tagged_string* n= Cyc_Toc_add_tagged_ptr_type( ta1, _temp1627); void* tx= Cyc_Absyn_strct(
n); void* ta2= Cyc_Absyn_cstar_typ( ta1, _temp1627); struct Cyc_Absyn_Exp*
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
Cyc_Absyn_declare_stmt( x, tx,( struct Cyc_Absyn_Exp*) _temp1207, s1, 0), 0))->r));
goto _LL1636;} _LL1640: if( Cyc_Toc_is_nullable( e1_typ)){ struct _tuple1* x=
Cyc_Toc_temp_var(); void* tx= Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Core_Opt*
_temp1643= _temp1207->topt; if( _temp1643 == 0){ _throw( Null_Exception);}
_temp1643->v;})); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp(
Cyc_Absyn_null_pointer_exn_exp( 0)); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_ifthenelse_stmt(
e2, Cyc_Absyn_exp_stmt( e3, 0), Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_exp_stmt( Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( x, 0), 0), 0);
struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( s1, s2, 0);( void*)( e->r=( void*)((
void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, tx,( struct Cyc_Absyn_Exp*)
_temp1207, s3, 0), 0))->r));} goto _LL1636; _LL1636:;} goto _LL1618; _LL1622:((
void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({
char* _temp1644=( char*)"exp_to_c: Deref: non-pointer"; struct _tagged_string
_temp1645; _temp1645.curr= _temp1644; _temp1645.base= _temp1644; _temp1645.last_plus_one=
_temp1644 + 29; _temp1645;})); goto _LL1618; _LL1618:;} goto _LL1066;} _LL1108:
Cyc_Toc_exp_to_c( nv, _temp1211); if( Cyc_Toc_is_poly_project( e)){( void*)( e->r=(
void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)({ struct
Cyc_Core_Opt* _temp1646= e->topt; if( _temp1646 == 0){ _throw( Null_Exception);}
_temp1646->v;})), Cyc_Absyn_new_exp(( void*) e->r, 0), 0))->r));} goto _LL1066;
_LL1110: Cyc_Toc_exp_to_c( nv, _temp1215);{ int is_poly= Cyc_Toc_is_poly_project(
e); if( Cyc_Toc_is_nullable(( void*)({ struct Cyc_Core_Opt* _temp1647= _temp1215->topt;
if( _temp1647 == 0){ _throw( Null_Exception);} _temp1647->v;}))){ struct _tuple1*
x= Cyc_Toc_temp_var(); void* tx= Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Core_Opt*
_temp1648= _temp1215->topt; if( _temp1648 == 0){ _throw( Null_Exception);}
_temp1648->v;})); struct Cyc_Absyn_Exp* e2= Cyc_Absyn_eq_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0); struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp(
Cyc_Absyn_null_pointer_exn_exp( 0)); struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_ifthenelse_stmt(
e2, Cyc_Absyn_exp_stmt( e3, 0), Cyc_Absyn_skip_stmt( 0), 0); struct Cyc_Absyn_Stmt*
s2= Cyc_Absyn_exp_stmt( Cyc_Absyn_structarrow_exp( Cyc_Absyn_var_exp( x, 0),
_temp1213, 0), 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( s1, s2, 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x,
tx,( struct Cyc_Absyn_Exp*) _temp1215, s3, 0), 0))->r));} if( is_poly){( void*)(
e->r=( void*)(( void*)( Cyc_Toc_array_to_ptr_cast( Cyc_Toc_typ_to_c(( void*)({
struct Cyc_Core_Opt* _temp1649= e->topt; if( _temp1649 == 0){ _throw(
Null_Exception);} _temp1649->v;})), Cyc_Absyn_new_exp(( void*) e->r, 0), 0))->r));}
goto _LL1066;} _LL1112: { void* e1_typ= Cyc_Tcutil_compress(( void*)({ struct
Cyc_Core_Opt* _temp1650= _temp1219->topt; if( _temp1650 == 0){ _throw(
Null_Exception);} _temp1650->v;})); Cyc_Toc_exp_to_c( nv, _temp1219); Cyc_Toc_exp_to_c(
nv, _temp1217);( void*)( _temp1219->r=( void*)(( void*)( Cyc_Toc_array_to_ptr(
Cyc_Toc_typ_to_c( e1_typ), Cyc_Absyn_new_exp(( void*) _temp1219->r, 0), 0))->r));{
void* _temp1651= e1_typ; struct Cyc_List_List* _temp1661; struct Cyc_Absyn_Exp*
_temp1663; struct Cyc_Absyn_Tqual* _temp1665; void* _temp1667; struct Cyc_Absyn_PtrInfo
_temp1669; struct Cyc_Absyn_Conref* _temp1671; struct Cyc_Absyn_Tqual* _temp1673;
struct Cyc_Absyn_Conref* _temp1675; void* _temp1677; void* _temp1679; _LL1653:
if(( unsigned int) _temp1651 > 4u?(( struct _tunion_struct*) _temp1651)->tag ==
Cyc_Absyn_TupleType_tag: 0){ _LL1662: _temp1661=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp1651)->f1; goto _LL1654;} else{ goto
_LL1655;} _LL1655: if(( unsigned int) _temp1651 > 4u?(( struct _tunion_struct*)
_temp1651)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL1668: _temp1667=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1651)->f1; goto _LL1666; _LL1666:
_temp1665=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1651)->f2; goto _LL1664; _LL1664: _temp1663=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1651)->f3; goto _LL1656;} else{ goto
_LL1657;} _LL1657: if(( unsigned int) _temp1651 > 4u?(( struct _tunion_struct*)
_temp1651)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1670: _temp1669=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp1651)->f1;
_LL1680: _temp1679=( void*) _temp1669.elt_typ; goto _LL1678; _LL1678: _temp1677=(
void*) _temp1669.rgn_typ; goto _LL1676; _LL1676: _temp1675=( struct Cyc_Absyn_Conref*)
_temp1669.nullable; goto _LL1674; _LL1674: _temp1673=( struct Cyc_Absyn_Tqual*)
_temp1669.tq; goto _LL1672; _LL1672: _temp1671=( struct Cyc_Absyn_Conref*)
_temp1669.bounds; goto _LL1658;} else{ goto _LL1659;} _LL1659: goto _LL1660;
_LL1654: { unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp1217) + 1;( void*)(
e->r=( void*)(( void*)( Cyc_Absyn_structmember_exp( _temp1219,({ struct
_tagged_string* _temp1681=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp1681[ 0]= xprintf("f%d",( int) i); _temp1681;}), 0))->r));
goto _LL1652;} _LL1656: if( _temp1663 == 0){(( void(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp1682=( char*)"exp_to_c: Subscript_e array type bound null";
struct _tagged_string _temp1683; _temp1683.curr= _temp1682; _temp1683.base=
_temp1682; _temp1683.last_plus_one= _temp1682 + 44; _temp1683;}));}( void*)( e->r=(
void*)(( void*)( Cyc_Toc_known_size_subscript( _temp1219, _temp1217, 0,( struct
Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1684= _temp1663; if( _temp1684 == 0){
_throw( Null_Exception);} _temp1684;}), _temp1667, _temp1665, 0))->r)); goto
_LL1652; _LL1658:{ void* _temp1685=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1671); struct Cyc_Absyn_Exp* _temp1691; _LL1687: if(( unsigned int)
_temp1685 > 1u?(( struct _tunion_struct*) _temp1685)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL1692: _temp1691=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp1685)->f1; goto _LL1688;} else{ goto _LL1689;} _LL1689: if(( int) _temp1685
== Cyc_Absyn_Unknown_b){ goto _LL1690;} else{ goto _LL1686;} _LL1688: { int
possibly_null= 1;{ void* _temp1693=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp1675))->v; int
_temp1699; _LL1695: if(( unsigned int) _temp1693 > 1u?(( struct _tunion_struct*)
_temp1693)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL1700: _temp1699=( int)((
struct Cyc_Absyn_Eq_constr_struct*) _temp1693)->f1; goto _LL1696;} else{ goto
_LL1697;} _LL1697: goto _LL1698; _LL1696: possibly_null= _temp1699; goto _LL1694;
_LL1698: possibly_null= 0; goto _LL1694; _LL1694:;}( void*)( e->r=( void*)((
void*)( Cyc_Toc_known_size_subscript( _temp1219, _temp1217, possibly_null,
_temp1691, _temp1679, _temp1673, 0))->r)); goto _LL1686;} _LL1690: { struct
_tuple1* x= Cyc_Toc_temp_var(); struct _tuple1* i= Cyc_Toc_temp_var(); struct
_tuple1* a= Cyc_Toc_temp_var(); void* ta1= Cyc_Toc_typ_to_c_array( _temp1679);
struct _tagged_string* n= Cyc_Toc_add_tagged_ptr_type( ta1, _temp1673); void* tx=
Cyc_Absyn_strct( n); void* ta2= Cyc_Absyn_cstar_typ( ta1, _temp1673); struct Cyc_Absyn_Exp*
test_exp= Cyc_Absyn_or_exp( Cyc_Absyn_eq_exp( Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Toc_base_sp, 0), Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_or_exp(
Cyc_Absyn_lt_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp(
x, 0), Cyc_Toc_base_sp, 0), 0), Cyc_Absyn_gte_exp( Cyc_Absyn_var_exp( a, 0), Cyc_Absyn_structmember_exp(
Cyc_Absyn_var_exp( x, 0), Cyc_Toc_last_plus_one_sp, 0), 0), 0), 0); struct Cyc_Absyn_Exp*
a_init= Cyc_Absyn_add_exp( Cyc_Absyn_structmember_exp( Cyc_Absyn_var_exp( x, 0),
Cyc_Toc_curr_sp, 0), _temp1217, 0); struct Cyc_Absyn_Stmt* s5= Cyc_Absyn_exp_stmt(
Cyc_Absyn_deref_exp( Cyc_Absyn_var_exp( a, 0), 0), 0); struct Cyc_Absyn_Stmt* s4=
Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp( Cyc_Absyn_null_pointer_exn_exp( 0)), 0);
struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_ifthenelse_stmt( test_exp, s4, Cyc_Absyn_skip_stmt(
0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s3, s5, 0); struct Cyc_Absyn_Stmt*
s1= Cyc_Absyn_declare_stmt( a, ta2,( struct Cyc_Absyn_Exp*) a_init, s2, 0);(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x,
tx,( struct Cyc_Absyn_Exp*) _temp1219, s1, 0), 0))->r)); goto _LL1686;} _LL1686:;}
goto _LL1652; _LL1660:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1701=( char*)"exp_to_c: Subscript on non-tuple/array/tuple ptr";
struct _tagged_string _temp1702; _temp1702.curr= _temp1701; _temp1702.base=
_temp1701; _temp1702.last_plus_one= _temp1701 + 49; _temp1702;})); goto _LL1652;
_LL1652:;} goto _LL1066;} _LL1114: if( ! nv->toplevel){( void*)( e->r=( void*)((
void*)( Cyc_Toc_init_tuple( nv, 0, 0, _temp1221))->r));} else{ struct Cyc_List_List*
tqs=(( struct Cyc_List_List*(*)( struct _tuple5*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_tup_to_c, _temp1221); struct
_tagged_string* n= Cyc_Toc_add_tuple_type( tqs); struct Cyc_List_List* dles= 0;{
int i= 1; for( 0; _temp1221 != 0; _temp1221=({ struct Cyc_List_List* _temp1703=
_temp1221; if( _temp1703 == 0){ _throw( Null_Exception);} _temp1703->tl;}), i ++){
Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1704=
_temp1221; if( _temp1704 == 0){ _throw( Null_Exception);} _temp1704->hd;}));{
struct Cyc_List_List* des=({ struct Cyc_List_List* _temp1705=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1705->hd=( void*)(( void*)({
struct Cyc_Absyn_FieldName_struct* _temp1706=( struct Cyc_Absyn_FieldName_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct)); _temp1706[ 0]=({ struct
Cyc_Absyn_FieldName_struct _temp1707; _temp1707.tag= Cyc_Absyn_FieldName_tag;
_temp1707.f1=({ struct _tagged_string* _temp1708=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp1708[ 0]= xprintf("f%d", i);
_temp1708;}); _temp1707;}); _temp1706;})); _temp1705->tl= 0; _temp1705;}); dles=({
struct Cyc_List_List* _temp1709=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1709->hd=( void*)({ struct _tuple7* _temp1710=(
struct _tuple7*) GC_malloc( sizeof( struct _tuple7)); _temp1710->f1=( struct Cyc_List_List*)
des; _temp1710->f2=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1711=
_temp1221; if( _temp1711 == 0){ _throw( Null_Exception);} _temp1711->hd;});
_temp1710;}); _temp1709->tl= dles; _temp1709;});}}} dles=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles);( void*)( e->r=( void*)((
void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_strct( n), Cyc_Absyn_unresolvedmem_exp( 0,
dles, 0), 0))->r));} goto _LL1066; _LL1116:(( void(*)( struct _tagged_string s))
Cyc_Toc_unimp)(( struct _tagged_string)({ char* _temp1712=( char*)"compoundlit";
struct _tagged_string _temp1713; _temp1713.curr= _temp1712; _temp1713.base=
_temp1712; _temp1713.last_plus_one= _temp1712 + 12; _temp1713;})); goto _LL1066;
_LL1118:( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0,
_temp1227, 0))->r));{ struct Cyc_List_List* dles= _temp1227; for( 0; dles != 0;
dles=({ struct Cyc_List_List* _temp1714= dles; if( _temp1714 == 0){ _throw(
Null_Exception);} _temp1714->tl;})){ struct _tuple7 _temp1718; struct Cyc_Absyn_Exp*
_temp1719; struct Cyc_List_List* _temp1721; struct _tuple7* _temp1716=( struct
_tuple7*)({ struct Cyc_List_List* _temp1715= dles; if( _temp1715 == 0){ _throw(
Null_Exception);} _temp1715->hd;}); _temp1718=* _temp1716; _LL1722: _temp1721=
_temp1718.f1; goto _LL1720; _LL1720: _temp1719= _temp1718.f2; goto _LL1717;
_LL1717: Cyc_Toc_exp_to_c( nv, _temp1719);}} goto _LL1066; _LL1120: {
unsigned int sz= Cyc_Evexp_eval_const_uint_exp( _temp1231); Cyc_Toc_exp_to_c( nv,
_temp1229);{ struct Cyc_List_List* es= 0;{ unsigned int i= 0; for( 0; i < sz; i
++){ es=({ struct Cyc_List_List* _temp1723=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1723->hd=( void*)({ struct _tuple7*
_temp1724=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7)); _temp1724->f1=
0; _temp1724->f2= _temp1229; _temp1724;}); _temp1723->tl= es; _temp1723;});}}(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp( 0, es, 0))->r));
goto _LL1066;}} _LL1122: if( ! nv->toplevel){( void*)( e->r=( void*)(( void*)(
Cyc_Toc_init_struct( nv, old_typ, 0, 0, _temp1237, _temp1241))->r));} else{
struct _tuple1* n= _temp1241; struct Cyc_Absyn_Structdecl* sd2=( struct Cyc_Absyn_Structdecl*)({
struct Cyc_Absyn_Structdecl* _temp1726= _temp1235 == 0?(( struct Cyc_Absyn_Structdecl*(*)(
struct _tagged_string s)) Cyc_Toc_toc_impos)(({ struct _tagged_string _temp1725=
Cyc_Absynpp_qvar2string( _temp1241); xprintf("no structdecl %.*s", _temp1725.last_plus_one
- _temp1725.curr, _temp1725.curr);})): _temp1235; if( _temp1726 == 0){ _throw(
Null_Exception);} _temp1726;}); struct Cyc_List_List* fields=(( struct Cyc_List_List*(*)(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* des, struct Cyc_Absyn_Structdecl*
sd)) Cyc_Tcutil_resolve_struct_designators)( e->loc, _temp1237,( struct Cyc_Absyn_Structdecl*)
sd2); for( 0; fields != 0; fields=({ struct Cyc_List_List* _temp1727= fields;
if( _temp1727 == 0){ _throw( Null_Exception);} _temp1727->tl;})){ struct
_tuple12 _temp1731; struct Cyc_Absyn_Exp* _temp1732; struct Cyc_Absyn_Structfield*
_temp1734; struct _tuple12* _temp1729=( struct _tuple12*)({ struct Cyc_List_List*
_temp1728= fields; if( _temp1728 == 0){ _throw( Null_Exception);} _temp1728->hd;});
_temp1731=* _temp1729; _LL1735: _temp1734= _temp1731.f1; goto _LL1733; _LL1733:
_temp1732= _temp1731.f2; goto _LL1730; _LL1730: { void* fieldtyp=( void*)
_temp1734->type; Cyc_Toc_exp_to_c( nv, _temp1732); if( Cyc_Toc_is_void_star(
fieldtyp)){( void*)( _temp1732->r=( void*)(( void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(( void*) _temp1732->r, 0), 0))->r));}}}( void*)( e->r=( void*)((
void*)( Cyc_Absyn_cast_exp( Cyc_Absyn_strctq( n), Cyc_Absyn_unresolvedmem_exp( 0,
_temp1237, 0), 0))->r));} goto _LL1066; _LL1124: { struct _tuple1* qv= _temp1243->name;
if( ! nv->toplevel){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_var_exp( qv, 0))->r));}
else{ int tag_count= 0; struct Cyc_List_List* fields= _temp1245->fields == 0? 0:(
struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp1736= _temp1245->fields; if(
_temp1736 == 0){ _throw( Null_Exception);} _temp1736->v;}); while( Cyc_Absyn_qvar_cmp(
qv,(( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp1737= fields;
if( _temp1737 == 0){ _throw( Null_Exception);} _temp1737->hd;}))->name) != 0) {
if((( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp1738= fields;
if( _temp1738 == 0){ _throw( Null_Exception);} _temp1738->hd;}))->typs == 0){
tag_count ++;} fields=({ struct Cyc_List_List* _temp1739= fields; if( _temp1739
== 0){ _throw( Null_Exception);} _temp1739->tl;});}( void*)( e->r=( void*)((
void*)( Cyc_Absyn_uint_exp(( unsigned int) tag_count, 0))->r));} goto _LL1066;}
_LL1126: { struct _tuple1* qv= _temp1253->name; struct Cyc_List_List* tqts=
_temp1253->typs; struct _tuple1* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
xexp= Cyc_Absyn_var_exp( x, 0); void* strct_typ= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag(
qv,( struct _tagged_string)({ char* _temp1740=( char*)"_struct"; struct
_tagged_string _temp1741; _temp1741.curr= _temp1740; _temp1741.base= _temp1740;
_temp1741.last_plus_one= _temp1740 + 8; _temp1741;}))); if( nv->toplevel){ int
tag_count= 0; struct Cyc_List_List* fields= _temp1255->fields == 0? 0:( struct
Cyc_List_List*)({ struct Cyc_Core_Opt* _temp1742= _temp1255->fields; if(
_temp1742 == 0){ _throw( Null_Exception);} _temp1742->v;}); while( Cyc_Absyn_qvar_cmp(
qv,(( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp1743= fields;
if( _temp1743 == 0){ _throw( Null_Exception);} _temp1743->hd;}))->name) != 0) {
if((( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp1744= fields;
if( _temp1744 == 0){ _throw( Null_Exception);} _temp1744->hd;}))->typs != 0){
tag_count ++;} fields=({ struct Cyc_List_List* _temp1745= fields; if( _temp1745
== 0){ _throw( Null_Exception);} _temp1745->tl;});}{ struct Cyc_List_List* dles=
0; for( 0; _temp1257 != 0; _temp1257=({ struct Cyc_List_List* _temp1746=
_temp1257; if( _temp1746 == 0){ _throw( Null_Exception);} _temp1746->tl;}), tqts=({
struct Cyc_List_List* _temp1747= tqts; if( _temp1747 == 0){ _throw(
Null_Exception);} _temp1747->tl;})){ struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1748= _temp1257; if( _temp1748 == 0){ _throw(
Null_Exception);} _temp1748->hd;}); void* field_typ= Cyc_Toc_typ_to_c((*((
struct _tuple5*)({ struct Cyc_List_List* _temp1749= tqts; if( _temp1749 == 0){
_throw( Null_Exception);} _temp1749->hd;}))).f2); Cyc_Toc_exp_to_c( nv, cur_e);
if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp( field_typ,
cur_e, 0);} dles=({ struct Cyc_List_List* _temp1750=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp1750[ 0]=({ struct Cyc_List_List
_temp1751; _temp1751.hd=( void*)({ struct _tuple7* _temp1752=( struct _tuple7*)
GC_malloc( sizeof( struct _tuple7) * 1); _temp1752[ 0]=({ struct _tuple7
_temp1753; _temp1753.f1= 0; _temp1753.f2= cur_e; _temp1753;}); _temp1752;});
_temp1751.tl= dles; _temp1751;}); _temp1750;});}{ struct Cyc_Absyn_Exp* tag_exp=
Cyc_Absyn_uint_exp(( unsigned int) tag_count, 0); dles=({ struct Cyc_List_List*
_temp1754=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp1754[ 0]=({ struct Cyc_List_List _temp1755; _temp1755.hd=( void*)({ struct
_tuple7* _temp1756=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7) * 1);
_temp1756[ 0]=({ struct _tuple7 _temp1757; _temp1757.f1= 0; _temp1757.f2=
tag_exp; _temp1757;}); _temp1756;}); _temp1755.tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles); _temp1755;}); _temp1754;});(
void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(({ struct Cyc_Core_Opt*
_temp1758=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1758->v=( void*) Cyc_Toc_collapse_qvar_tag( qv,( struct _tagged_string)({
char* _temp1759=( char*)"_struct"; struct _tagged_string _temp1760; _temp1760.curr=
_temp1759; _temp1760.base= _temp1759; _temp1760.last_plus_one= _temp1759 + 8;
_temp1760;})); _temp1758;}), dles, 0))->r));}}} else{ struct Cyc_List_List*
rev_assign=({ struct Cyc_List_List* _temp1761=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1761->hd=( void*) Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp( xexp, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp( Cyc_Toc_collapse_qvar_tag(
qv,( struct _tagged_string)({ char* _temp1762=( char*)"_tag"; struct
_tagged_string _temp1763; _temp1763.curr= _temp1762; _temp1763.base= _temp1762;
_temp1763.last_plus_one= _temp1762 + 5; _temp1763;})), 0), 0); _temp1761->tl= 0;
_temp1761;});{ int i= 1; for( 0; _temp1257 != 0;( _temp1257=({ struct Cyc_List_List*
_temp1764= _temp1257; if( _temp1764 == 0){ _throw( Null_Exception);} _temp1764->tl;}),
i ++), tqts=({ struct Cyc_List_List* _temp1765= tqts; if( _temp1765 == 0){
_throw( Null_Exception);} _temp1765->tl;})){ struct Cyc_Absyn_Exp* cur_e=(
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1766= _temp1257; if(
_temp1766 == 0){ _throw( Null_Exception);} _temp1766->hd;}); void* field_typ=
Cyc_Toc_typ_to_c((*(( struct _tuple5*)({ struct Cyc_List_List* _temp1767= tqts;
if( _temp1767 == 0){ _throw( Null_Exception);} _temp1767->hd;}))).f2); Cyc_Toc_exp_to_c(
nv, cur_e); if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp(
field_typ, cur_e, 0);}{ struct Cyc_Absyn_Stmt* a= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
xexp,({ struct _tagged_string* _temp1768=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp1768[ 0]= xprintf("f%d", i); _temp1768;}),
0), cur_e, 0); rev_assign=({ struct Cyc_List_List* _temp1769=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1769->hd=( void*) a; _temp1769->tl=
rev_assign; _temp1769;});}}}{ struct Cyc_Absyn_Stmt* final_stmt= Cyc_Absyn_exp_stmt(
xexp, 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmts((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)(({ struct Cyc_List_List* _temp1770=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1770->hd=(
void*) final_stmt; _temp1770->tl= rev_assign; _temp1770;})), 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, strct_typ, 0, s2,
0), 0))->r));}} goto _LL1066;} _LL1128: { struct _tuple1* qv= _temp1263->name;
struct Cyc_List_List* tqts= _temp1263->typs; struct Cyc_Absyn_Exp* tag_exp= Cyc_Absyn_var_exp(
Cyc_Toc_collapse_qvar_tag( qv,( struct _tagged_string)({ char* _temp1771=( char*)"_tag";
struct _tagged_string _temp1772; _temp1772.curr= _temp1771; _temp1772.base=
_temp1771; _temp1772.last_plus_one= _temp1771 + 5; _temp1772;})), 0); if( nv->toplevel){
struct Cyc_List_List* dles=(( struct Cyc_List_List*(*)( struct _tuple7*(* f)(
struct Cyc_Toc_Env*, struct Cyc_Absyn_Exp*), struct Cyc_Toc_Env* env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Toc_add_designator, nv, _temp1267); dles=({ struct Cyc_List_List*
_temp1773=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1773->hd=( void*)({ struct _tuple7* _temp1774=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp1774->f1= 0; _temp1774->f2= tag_exp; _temp1774;});
_temp1773->tl= dles; _temp1773;});( void*)( e->r=( void*)(( void*)( Cyc_Absyn_unresolvedmem_exp(({
struct Cyc_Core_Opt* _temp1775=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1775->v=( void*) Cyc_Toc_collapse_qvar_tag( qv,( struct
_tagged_string)({ char* _temp1776=( char*)"_struct"; struct _tagged_string
_temp1777; _temp1777.curr= _temp1776; _temp1777.base= _temp1776; _temp1777.last_plus_one=
_temp1776 + 8; _temp1777;})); _temp1775;}), dles, 0))->r));} else{ struct
_tuple1* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp( x,
0); void* strct_typ= Cyc_Absyn_strctq( Cyc_Toc_collapse_qvar_tag( qv,( struct
_tagged_string)({ char* _temp1778=( char*)"_struct"; struct _tagged_string
_temp1779; _temp1779.curr= _temp1778; _temp1779.base= _temp1778; _temp1779.last_plus_one=
_temp1778 + 8; _temp1779;}))); struct Cyc_List_List* rev_assign=({ struct Cyc_List_List*
_temp1780=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1780->hd=( void*) Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp( xexp,
Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp( Cyc_Toc_collapse_qvar_tag( qv,( struct
_tagged_string)({ char* _temp1781=( char*)"_tag"; struct _tagged_string
_temp1782; _temp1782.curr= _temp1781; _temp1782.base= _temp1781; _temp1782.last_plus_one=
_temp1781 + 5; _temp1782;})), 0), 0); _temp1780->tl= 0; _temp1780;});{ int i= 1;
for( 0; _temp1267 != 0;( _temp1267=({ struct Cyc_List_List* _temp1783= _temp1267;
if( _temp1783 == 0){ _throw( Null_Exception);} _temp1783->tl;}), i ++), tqts=({
struct Cyc_List_List* _temp1784= tqts; if( _temp1784 == 0){ _throw(
Null_Exception);} _temp1784->tl;})){ struct Cyc_Absyn_Exp* cur_e=( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1785= _temp1267; if( _temp1785 == 0){ _throw(
Null_Exception);} _temp1785->hd;}); void* field_typ= Cyc_Toc_typ_to_c((*((
struct _tuple5*)({ struct Cyc_List_List* _temp1786= tqts; if( _temp1786 == 0){
_throw( Null_Exception);} _temp1786->hd;}))).f2); Cyc_Toc_exp_to_c( nv, cur_e);
if( Cyc_Toc_is_void_star( field_typ)){ cur_e= Cyc_Absyn_cast_exp( field_typ,
cur_e, 0);}{ struct Cyc_Absyn_Stmt* a= Cyc_Absyn_assign_stmt( Cyc_Absyn_structmember_exp(
xexp,({ struct _tagged_string* _temp1787=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp1787[ 0]= xprintf("f%d", i); _temp1787;}),
0), cur_e, 0); rev_assign=({ struct Cyc_List_List* _temp1788=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1788->hd=( void*) a; _temp1788->tl=
rev_assign; _temp1788;});}}}{ struct Cyc_Absyn_Stmt* final_stmt= Cyc_Absyn_exp_stmt(
xexp, 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmts((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)(({ struct Cyc_List_List* _temp1789=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1789->hd=(
void*) final_stmt; _temp1789->tl= rev_assign; _temp1789;})), 0);( void*)( e->r=(
void*)(( void*)( Cyc_Absyn_stmt_exp( Cyc_Absyn_declare_stmt( x, strct_typ, 0, s2,
0), 0))->r));}} goto _LL1066;} _LL1130: goto _LL1066; _LL1132: { void* t_c= Cyc_Toc_typ_to_c(
_temp1277); if( _temp1279 != 0){ struct Cyc_Absyn_Exp* rgn=( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1790= _temp1279; if( _temp1790 == 0){ _throw(
Null_Exception);} _temp1790;}); Cyc_Toc_exp_to_c( nv, rgn);( void*)( e->r=( void*)((
void*)( Cyc_Toc_rmalloc_exp( rgn, Cyc_Absyn_sizeoftyp_exp( t_c, 0)))->r));}
else{( void*)( e->r=( void*)(( void*)( Cyc_Toc_malloc_exp( _temp1277, Cyc_Absyn_sizeoftyp_exp(
t_c, 0)))->r));} goto _LL1066;} _LL1134: Cyc_Toc_stmt_to_c( nv, _temp1281); goto
_LL1066; _LL1136:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp1791=( char*)"UnresolvedMem"; struct
_tagged_string _temp1792; _temp1792.curr= _temp1791; _temp1792.base= _temp1791;
_temp1792.last_plus_one= _temp1791 + 14; _temp1792;})); goto _LL1066; _LL1138:((
void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(( struct _tagged_string)({
char* _temp1793=( char*)"codegen"; struct _tagged_string _temp1794; _temp1794.curr=
_temp1793; _temp1794.base= _temp1793; _temp1794.last_plus_one= _temp1793 + 8;
_temp1794;})); goto _LL1066; _LL1140:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)((
struct _tagged_string)({ char* _temp1795=( char*)"fill"; struct _tagged_string
_temp1796; _temp1796.curr= _temp1795; _temp1796.base= _temp1795; _temp1796.last_plus_one=
_temp1795 + 5; _temp1796;})); goto _LL1066; _LL1066:;}} static struct Cyc_Absyn_Stmt*
Cyc_Toc_if_eq_goto_stmt( struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2,
struct _tagged_string* succ_lab, struct _tagged_string* fail_lab){ return Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( e1, e2, 0), Cyc_Absyn_goto_stmt( succ_lab, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0);} static struct _tuple13 Cyc_Toc_xlate_pat( struct Cyc_Toc_Env*
nv, void* t, struct Cyc_Absyn_Exp* r, struct Cyc_Absyn_Exp* path, struct Cyc_Absyn_Pat*
p, struct _tagged_string* succ_lab, struct _tagged_string* fail_lab, struct Cyc_List_List*
decls){ struct Cyc_Absyn_Stmt* s;{ void* _temp1797=( void*) p->r; struct Cyc_Absyn_Vardecl*
_temp1837; struct Cyc_Absyn_Vardecl _temp1839; struct Cyc_List_List* _temp1840;
struct Cyc_Core_Opt* _temp1842; int _temp1844; struct Cyc_Absyn_Exp* _temp1846;
void* _temp1848; struct Cyc_Absyn_Tqual* _temp1850; struct _tuple1* _temp1852;
void* _temp1854; struct Cyc_Absyn_Vardecl* _temp1856; int _temp1858; void*
_temp1860; char _temp1862; struct _tagged_string _temp1864; struct Cyc_Absyn_Enumfield*
_temp1866; struct Cyc_Absyn_Enumdecl* _temp1868; struct _tuple1* _temp1870;
struct Cyc_Absyn_Tunionfield* _temp1872; struct Cyc_Absyn_Tuniondecl* _temp1874;
struct Cyc_List_List* _temp1876; struct Cyc_List_List* _temp1878; struct Cyc_Core_Opt*
_temp1880; struct _tuple1* _temp1882; struct Cyc_Absyn_Pat* _temp1884; struct
Cyc_Absyn_Pat _temp1886; struct Cyc_Position_Segment* _temp1887; struct Cyc_Core_Opt*
_temp1889; void* _temp1891; struct Cyc_Absyn_Tunionfield* _temp1893; struct Cyc_Absyn_Tuniondecl*
_temp1895; struct Cyc_List_List* _temp1897; struct Cyc_List_List* _temp1899;
struct Cyc_Core_Opt* _temp1901; struct _tuple1* _temp1903; struct Cyc_Absyn_Pat*
_temp1905; struct Cyc_Absyn_Pat _temp1907; struct Cyc_Position_Segment*
_temp1908; struct Cyc_Core_Opt* _temp1910; void* _temp1912; struct Cyc_Absyn_Tunionfield*
_temp1914; struct Cyc_Absyn_XTuniondecl* _temp1916; struct Cyc_List_List*
_temp1918; struct Cyc_List_List* _temp1920; struct _tuple1* _temp1922; struct
Cyc_List_List* _temp1924; struct Cyc_List_List* _temp1926; struct Cyc_List_List*
_temp1928; struct Cyc_Core_Opt* _temp1930; struct Cyc_Absyn_Structdecl*
_temp1932; struct Cyc_Absyn_Pat* _temp1934; struct _tuple1* _temp1936; struct
Cyc_List_List* _temp1938; struct Cyc_List_List* _temp1940; struct _tuple1*
_temp1942; struct Cyc_List_List* _temp1944; struct Cyc_List_List* _temp1946;
struct _tuple1* _temp1948; struct Cyc_Absyn_Tunionfield* _temp1950; struct Cyc_Absyn_Tuniondecl*
_temp1952; struct Cyc_List_List* _temp1954; struct Cyc_List_List* _temp1956;
struct Cyc_Core_Opt* _temp1958; struct _tuple1* _temp1960; struct Cyc_Absyn_Tunionfield*
_temp1962; struct Cyc_Absyn_XTuniondecl* _temp1964; struct Cyc_List_List*
_temp1966; struct Cyc_List_List* _temp1968; struct _tuple1* _temp1970; _LL1799:
if(( int) _temp1797 == Cyc_Absyn_Wild_p){ goto _LL1800;} else{ goto _LL1801;}
_LL1801: if(( unsigned int) _temp1797 > 2u?(( struct _tunion_struct*) _temp1797)->tag
== Cyc_Absyn_Var_p_tag: 0){ _LL1838: _temp1837=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_p_struct*) _temp1797)->f1; _temp1839=* _temp1837; _LL1855:
_temp1854=( void*) _temp1839.sc; goto _LL1853; _LL1853: _temp1852=( struct
_tuple1*) _temp1839.name; goto _LL1851; _LL1851: _temp1850=( struct Cyc_Absyn_Tqual*)
_temp1839.tq; goto _LL1849; _LL1849: _temp1848=( void*) _temp1839.type; goto
_LL1847; _LL1847: _temp1846=( struct Cyc_Absyn_Exp*) _temp1839.initializer; goto
_LL1845; _LL1845: _temp1844=( int) _temp1839.shadow; goto _LL1843; _LL1843:
_temp1842=( struct Cyc_Core_Opt*) _temp1839.rgn; goto _LL1841; _LL1841:
_temp1840=( struct Cyc_List_List*) _temp1839.attributes; goto _LL1802;} else{
goto _LL1803;} _LL1803: if(( unsigned int) _temp1797 > 2u?(( struct
_tunion_struct*) _temp1797)->tag == Cyc_Absyn_Reference_p_tag: 0){ _LL1857:
_temp1856=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Reference_p_struct*)
_temp1797)->f1; goto _LL1804;} else{ goto _LL1805;} _LL1805: if(( int) _temp1797
== Cyc_Absyn_Null_p){ goto _LL1806;} else{ goto _LL1807;} _LL1807: if((
unsigned int) _temp1797 > 2u?(( struct _tunion_struct*) _temp1797)->tag == Cyc_Absyn_Int_p_tag:
0){ _LL1861: _temp1860=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp1797)->f1;
goto _LL1859; _LL1859: _temp1858=( int)(( struct Cyc_Absyn_Int_p_struct*)
_temp1797)->f2; goto _LL1808;} else{ goto _LL1809;} _LL1809: if(( unsigned int)
_temp1797 > 2u?(( struct _tunion_struct*) _temp1797)->tag == Cyc_Absyn_Char_p_tag:
0){ _LL1863: _temp1862=( char)(( struct Cyc_Absyn_Char_p_struct*) _temp1797)->f1;
goto _LL1810;} else{ goto _LL1811;} _LL1811: if(( unsigned int) _temp1797 > 2u?((
struct _tunion_struct*) _temp1797)->tag == Cyc_Absyn_Float_p_tag: 0){ _LL1865:
_temp1864=( struct _tagged_string)(( struct Cyc_Absyn_Float_p_struct*) _temp1797)->f1;
goto _LL1812;} else{ goto _LL1813;} _LL1813: if(( unsigned int) _temp1797 > 2u?((
struct _tunion_struct*) _temp1797)->tag == Cyc_Absyn_Enum_p_tag: 0){ _LL1871:
_temp1870=( struct _tuple1*)(( struct Cyc_Absyn_Enum_p_struct*) _temp1797)->f1;
goto _LL1869; _LL1869: _temp1868=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp1797)->f2; goto _LL1867; _LL1867: _temp1866=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_p_struct*) _temp1797)->f3; goto _LL1814;} else{ goto
_LL1815;} _LL1815: if(( unsigned int) _temp1797 > 2u?(( struct _tunion_struct*)
_temp1797)->tag == Cyc_Absyn_Tunion_p_tag: 0){ _LL1883: _temp1882=( struct
_tuple1*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp1797)->f1; goto _LL1881;
_LL1881: _temp1880=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1797)->f2; goto _LL1879; _LL1879: _temp1878=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1797)->f3; goto _LL1877; _LL1877:
_temp1876=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1797)->f4; if( _temp1876 == 0){ goto _LL1875;} else{ goto _LL1817;} _LL1875:
_temp1874=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1797)->f5; goto _LL1873; _LL1873: _temp1872=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1797)->f6; goto _LL1816;} else{ goto
_LL1817;} _LL1817: if(( unsigned int) _temp1797 > 2u?(( struct _tunion_struct*)
_temp1797)->tag == Cyc_Absyn_Pointer_p_tag: 0){ _LL1885: _temp1884=( struct Cyc_Absyn_Pat*)((
struct Cyc_Absyn_Pointer_p_struct*) _temp1797)->f1; _temp1886=* _temp1884;
_LL1892: _temp1891=( void*) _temp1886.r; if(( unsigned int) _temp1891 > 2u?((
struct _tunion_struct*) _temp1891)->tag == Cyc_Absyn_Tunion_p_tag: 0){ _LL1904:
_temp1903=( struct _tuple1*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp1891)->f1;
goto _LL1902; _LL1902: _temp1901=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1891)->f2; goto _LL1900; _LL1900: _temp1899=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1891)->f3; goto _LL1898; _LL1898:
_temp1897=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1891)->f4; goto _LL1896; _LL1896: _temp1895=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1891)->f5; goto _LL1894; _LL1894:
_temp1893=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1891)->f6; goto _LL1890;} else{ goto _LL1819;} _LL1890: _temp1889=( struct
Cyc_Core_Opt*) _temp1886.topt; goto _LL1888; _LL1888: _temp1887=( struct Cyc_Position_Segment*)
_temp1886.loc; goto _LL1818;} else{ goto _LL1819;} _LL1819: if(( unsigned int)
_temp1797 > 2u?(( struct _tunion_struct*) _temp1797)->tag == Cyc_Absyn_Pointer_p_tag:
0){ _LL1906: _temp1905=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1797)->f1; _temp1907=* _temp1905; _LL1913: _temp1912=( void*) _temp1907.r;
if(( unsigned int) _temp1912 > 2u?(( struct _tunion_struct*) _temp1912)->tag ==
Cyc_Absyn_XTunion_p_tag: 0){ _LL1923: _temp1922=( struct _tuple1*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp1912)->f1; goto _LL1921; _LL1921: _temp1920=( struct Cyc_List_List*)((
struct Cyc_Absyn_XTunion_p_struct*) _temp1912)->f2; goto _LL1919; _LL1919:
_temp1918=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp1912)->f3; goto _LL1917; _LL1917: _temp1916=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_p_struct*) _temp1912)->f4; goto _LL1915; _LL1915:
_temp1914=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp1912)->f5; goto _LL1911;} else{ goto _LL1821;} _LL1911: _temp1910=( struct
Cyc_Core_Opt*) _temp1907.topt; goto _LL1909; _LL1909: _temp1908=( struct Cyc_Position_Segment*)
_temp1907.loc; goto _LL1820;} else{ goto _LL1821;} _LL1821: if(( unsigned int)
_temp1797 > 2u?(( struct _tunion_struct*) _temp1797)->tag == Cyc_Absyn_Tuple_p_tag:
0){ _LL1925: _temp1924=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*)
_temp1797)->f1; goto _LL1822;} else{ goto _LL1823;} _LL1823: if(( unsigned int)
_temp1797 > 2u?(( struct _tunion_struct*) _temp1797)->tag == Cyc_Absyn_Struct_p_tag:
0){ _LL1933: _temp1932=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1797)->f1; goto _LL1931; _LL1931: _temp1930=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1797)->f2; goto _LL1929; _LL1929:
_temp1928=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1797)->f3; goto _LL1927; _LL1927: _temp1926=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1797)->f4; goto _LL1824;} else{ goto
_LL1825;} _LL1825: if(( unsigned int) _temp1797 > 2u?(( struct _tunion_struct*)
_temp1797)->tag == Cyc_Absyn_Pointer_p_tag: 0){ _LL1935: _temp1934=( struct Cyc_Absyn_Pat*)((
struct Cyc_Absyn_Pointer_p_struct*) _temp1797)->f1; goto _LL1826;} else{ goto
_LL1827;} _LL1827: if(( unsigned int) _temp1797 > 2u?(( struct _tunion_struct*)
_temp1797)->tag == Cyc_Absyn_UnknownId_p_tag: 0){ _LL1937: _temp1936=( struct
_tuple1*)(( struct Cyc_Absyn_UnknownId_p_struct*) _temp1797)->f1; goto _LL1828;}
else{ goto _LL1829;} _LL1829: if(( unsigned int) _temp1797 > 2u?(( struct
_tunion_struct*) _temp1797)->tag == Cyc_Absyn_UnknownCall_p_tag: 0){ _LL1943:
_temp1942=( struct _tuple1*)(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp1797)->f1;
goto _LL1941; _LL1941: _temp1940=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1797)->f2; goto _LL1939; _LL1939: _temp1938=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_p_struct*) _temp1797)->f3; goto _LL1830;} else{
goto _LL1831;} _LL1831: if(( unsigned int) _temp1797 > 2u?(( struct
_tunion_struct*) _temp1797)->tag == Cyc_Absyn_UnknownFields_p_tag: 0){ _LL1949:
_temp1948=( struct _tuple1*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1797)->f1; goto _LL1947; _LL1947: _temp1946=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownFields_p_struct*) _temp1797)->f2; goto _LL1945; _LL1945:
_temp1944=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1797)->f3; goto _LL1832;} else{ goto _LL1833;} _LL1833: if(( unsigned int)
_temp1797 > 2u?(( struct _tunion_struct*) _temp1797)->tag == Cyc_Absyn_Tunion_p_tag:
0){ _LL1961: _temp1960=( struct _tuple1*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1797)->f1; goto _LL1959; _LL1959: _temp1958=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1797)->f2; goto _LL1957; _LL1957:
_temp1956=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1797)->f3; goto _LL1955; _LL1955: _temp1954=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1797)->f4; goto _LL1953; _LL1953:
_temp1952=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1797)->f5; goto _LL1951; _LL1951: _temp1950=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1797)->f6; goto _LL1834;} else{ goto
_LL1835;} _LL1835: if(( unsigned int) _temp1797 > 2u?(( struct _tunion_struct*)
_temp1797)->tag == Cyc_Absyn_XTunion_p_tag: 0){ _LL1971: _temp1970=( struct
_tuple1*)(( struct Cyc_Absyn_XTunion_p_struct*) _temp1797)->f1; goto _LL1969;
_LL1969: _temp1968=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp1797)->f2; goto _LL1967; _LL1967: _temp1966=( struct Cyc_List_List*)((
struct Cyc_Absyn_XTunion_p_struct*) _temp1797)->f3; goto _LL1965; _LL1965:
_temp1964=( struct Cyc_Absyn_XTuniondecl*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp1797)->f4; goto _LL1963; _LL1963: _temp1962=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_XTunion_p_struct*) _temp1797)->f5; goto _LL1836;} else{ goto
_LL1798;} _LL1800: s= Cyc_Absyn_goto_stmt( succ_lab, 0); goto _LL1798; _LL1802:
nv= Cyc_Toc_add_varmap( nv, _temp1852, r); s= Cyc_Absyn_goto_stmt( succ_lab, 0);
goto _LL1798; _LL1804: { struct _tuple1* v= Cyc_Toc_temp_var(); decls=({ struct
Cyc_List_List* _temp1972=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1972->hd=( void*)({ struct _tuple14* _temp1973=( struct _tuple14*)
GC_malloc( sizeof( struct _tuple14)); _temp1973->f1= v; _temp1973->f2= Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c( t), Cyc_Toc_mt_tq); _temp1973;}); _temp1972->tl= decls;
_temp1972;}); nv= Cyc_Toc_add_varmap( nv, _temp1856->name, Cyc_Absyn_var_exp( v,
0)); s= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_address_exp(
path, 0), 0), Cyc_Absyn_goto_stmt( succ_lab, 0), 0); goto _LL1798;} _LL1806: s=
Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_signed_int_exp( 0, 0), succ_lab, fail_lab);
goto _LL1798; _LL1808: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_int_exp(
_temp1860, _temp1858, 0), succ_lab, fail_lab); goto _LL1798; _LL1810: s= Cyc_Toc_if_eq_goto_stmt(
r, Cyc_Absyn_char_exp( _temp1862, 0), succ_lab, fail_lab); goto _LL1798; _LL1812:
s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_float_exp( _temp1864, 0), succ_lab,
fail_lab); goto _LL1798; _LL1814: s= Cyc_Toc_if_eq_goto_stmt( r, Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Enum_e_struct* _temp1974=( struct Cyc_Absyn_Enum_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_e_struct)); _temp1974[ 0]=({ struct Cyc_Absyn_Enum_e_struct
_temp1975; _temp1975.tag= Cyc_Absyn_Enum_e_tag; _temp1975.f1= _temp1870;
_temp1975.f2=( struct Cyc_Absyn_Enumdecl*) _temp1868; _temp1975.f3=( struct Cyc_Absyn_Enumfield*)
_temp1866; _temp1975;}); _temp1974;}), 0), succ_lab, fail_lab); goto _LL1798;
_LL1816: s= Cyc_Toc_if_eq_goto_stmt( Cyc_Absyn_cast_exp( Cyc_Absyn_sint_t, r, 0),
Cyc_Absyn_var_exp( _temp1882, 0), succ_lab, fail_lab); goto _LL1798; _LL1818: {
struct Cyc_List_List* ss= 0; int cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp1897); struct _tuple1* efstrct= Cyc_Toc_collapse_qvar_tag( _temp1893->name,(
struct _tagged_string)({ char* _temp1976=( char*)"_struct"; struct
_tagged_string _temp1977; _temp1977.curr= _temp1976; _temp1977.base= _temp1976;
_temp1977.last_plus_one= _temp1976 + 8; _temp1977;})); struct Cyc_Absyn_Exp*
rcast= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_strctq( efstrct), Cyc_Toc_mt_tq),
r, 0);{ struct Cyc_List_List* rps=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( _temp1897); for( 0; rps != 0; rps=({ struct Cyc_List_List*
_temp1978= rps; if( _temp1978 == 0){ _throw( Null_Exception);} _temp1978->tl;}),
cnt --){ struct Cyc_Absyn_Pat* p2=( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List*
_temp1979= rps; if( _temp1979 == 0){ _throw( Null_Exception);} _temp1979->hd;});
struct _tuple1* v= Cyc_Toc_temp_var(); void* t2=( void*)({ struct Cyc_Core_Opt*
_temp1980= p2->topt; if( _temp1980 == 0){ _throw( Null_Exception);} _temp1980->v;});
void* t2c= Cyc_Toc_typ_to_c( t2); struct _tagged_string* lab= Cyc_Toc_fresh_label();
decls=({ struct Cyc_List_List* _temp1981=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1981->hd=( void*)({ struct _tuple14*
_temp1982=( struct _tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp1982->f1=
v; _temp1982->f2= t2c; _temp1982;}); _temp1981->tl= decls; _temp1981;});{ struct
_tuple13 result= Cyc_Toc_xlate_pat( nv, t2, Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structarrow_exp(
rcast,({ struct _tagged_string* _temp1983=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp1983[ 0]= xprintf("f%d", cnt);
_temp1983;}), 0), p2, succ_lab, fail_lab, decls); nv= result.f1; decls= result.f2;{
struct Cyc_Absyn_Stmt* s3= result.f3; struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_cast_exp( t2c, Cyc_Absyn_structarrow_exp(
rcast,({ struct _tagged_string* _temp1984=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp1984[ 0]= xprintf("f%d", cnt);
_temp1984;}), 0), 0), 0); struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s1, s3,
0); ss=({ struct Cyc_List_List* _temp1985=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1985->hd=( void*) Cyc_Absyn_label_stmt( lab,
s2, 0); _temp1985->tl= ss; _temp1985;}); succ_lab= lab;}}}}{ unsigned int
max_tag= 0;{ struct Cyc_List_List* fs=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp1986= _temp1895->fields; if( _temp1986 == 0){ _throw( Null_Exception);}
_temp1986->v;}); for( 0; fs != 0; fs=({ struct Cyc_List_List* _temp1987= fs; if(
_temp1987 == 0){ _throw( Null_Exception);} _temp1987->tl;})){ struct Cyc_Absyn_Tunionfield*
f=( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp1988= fs; if(
_temp1988 == 0){ _throw( Null_Exception);} _temp1988->hd;}); if( f->typs == 0){
max_tag ++;}}}{ struct Cyc_Absyn_Exp* max_tag_exp= Cyc_Absyn_uint_exp( max_tag,
0); struct Cyc_Absyn_Exp* e3= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_strct(
Cyc_Toc__tunion_struct_sp), Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_structarrow_exp(
e3, Cyc_Toc_tag_sp, 0); struct Cyc_Absyn_Exp* e5= Cyc_Absyn_gt_exp( Cyc_Absyn_cast_exp(
Cyc_Absyn_uint_t, r, 0), max_tag_exp, 0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp(
Cyc_Toc_collapse_qvar_tag( _temp1903,( struct _tagged_string)({ char* _temp1989=(
char*)"_tag"; struct _tagged_string _temp1990; _temp1990.curr= _temp1989;
_temp1990.base= _temp1989; _temp1990.last_plus_one= _temp1989 + 5; _temp1990;})),
0); struct Cyc_Absyn_Exp* test_exp= max_tag > 0? Cyc_Absyn_and_exp( e5, Cyc_Absyn_eq_exp(
e1, e, 0), 0): Cyc_Absyn_eq_exp( e1, e, 0); s= Cyc_Absyn_ifthenelse_stmt(
test_exp, Cyc_Absyn_seq_stmts( ss, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0);
goto _LL1798;}}} _LL1820: { struct Cyc_List_List* ss= 0; int cnt=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp1918); struct _tuple1* xfstrct=
Cyc_Toc_collapse_qvar_tag( _temp1914->name,( struct _tagged_string)({ char*
_temp1991=( char*)"_struct"; struct _tagged_string _temp1992; _temp1992.curr=
_temp1991; _temp1992.base= _temp1991; _temp1992.last_plus_one= _temp1991 + 8;
_temp1992;})); struct Cyc_Absyn_Exp* rcast= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ(
Cyc_Absyn_strctq( xfstrct), Cyc_Toc_mt_tq), r, 0);{ struct Cyc_List_List* rps=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( _temp1918);
for( 0; rps != 0; rps=({ struct Cyc_List_List* _temp1993= rps; if( _temp1993 ==
0){ _throw( Null_Exception);} _temp1993->tl;}), cnt --){ struct Cyc_Absyn_Pat*
p2=( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List* _temp1994= rps; if(
_temp1994 == 0){ _throw( Null_Exception);} _temp1994->hd;}); struct _tuple1* v=
Cyc_Toc_temp_var(); void* t2=( void*)({ struct Cyc_Core_Opt* _temp1995= p2->topt;
if( _temp1995 == 0){ _throw( Null_Exception);} _temp1995->v;}); struct
_tagged_string* lab= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp1996=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1996->hd=( void*)({ struct _tuple14* _temp1997=( struct _tuple14*)
GC_malloc( sizeof( struct _tuple14)); _temp1997->f1= v; _temp1997->f2= Cyc_Toc_typ_to_c(
t2); _temp1997;}); _temp1996->tl= decls; _temp1996;});{ struct _tuple13 result=
Cyc_Toc_xlate_pat( nv, t2, Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structarrow_exp(
rcast,({ struct _tagged_string* _temp1998=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp1998[ 0]= xprintf("f%d", cnt);
_temp1998;}), 0), p2, succ_lab, fail_lab, decls); nv= result.f1; decls= result.f2;{
struct Cyc_Absyn_Stmt* s3= result.f3; struct Cyc_Absyn_Stmt* s1= Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structarrow_exp( rcast,({ struct
_tagged_string* _temp1999=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp1999[ 0]= xprintf("f%d", cnt); _temp1999;}), 0), 0);
struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_seq_stmt( s1, s3, 0); ss=({ struct Cyc_List_List*
_temp2000=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2000->hd=( void*) Cyc_Absyn_label_stmt( lab, s2, 0); _temp2000->tl= ss;
_temp2000;}); succ_lab= lab;}}}} if( _temp1918 == 0){ s= Cyc_Toc_if_eq_goto_stmt(
Cyc_Absyn_structarrow_exp( r, Cyc_Toc_tag_sp, 0), Cyc_Absyn_var_exp( Cyc_Toc_collapse_qvar_tag(
_temp1922,( struct _tagged_string)({ char* _temp2001=( char*)"_tag"; struct
_tagged_string _temp2002; _temp2002.curr= _temp2001; _temp2002.base= _temp2001;
_temp2002.last_plus_one= _temp2001 + 5; _temp2002;})), 0), succ_lab, fail_lab);}
else{ struct Cyc_Absyn_Exp* e3= Cyc_Absyn_cast_exp( Cyc_Absyn_cstar_typ( Cyc_Absyn_strct(
Cyc_Toc__xtunion_struct_sp), Cyc_Toc_mt_tq), r, 0); struct Cyc_Absyn_Exp* e2=
Cyc_Absyn_deref_exp( e3, 0); struct Cyc_Absyn_Exp* e1= Cyc_Absyn_structmember_exp(
e2, Cyc_Toc_tag_sp, 0); struct Cyc_Absyn_Exp* e= Cyc_Absyn_var_exp( Cyc_Toc_collapse_qvar_tag(
_temp1922,( struct _tagged_string)({ char* _temp2003=( char*)"_tag"; struct
_tagged_string _temp2004; _temp2004.curr= _temp2003; _temp2004.base= _temp2003;
_temp2004.last_plus_one= _temp2003 + 5; _temp2004;})), 0); s= Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp( e1, e, 0), Cyc_Absyn_seq_stmts( ss, 0), Cyc_Absyn_goto_stmt(
fail_lab, 0), 0);} goto _LL1798;} _LL1822: { struct Cyc_List_List* ss= 0; int
cnt=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1924);{ struct
Cyc_List_List* rps=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp1924); for( 0; rps != 0; rps=({ struct Cyc_List_List* _temp2005= rps; if(
_temp2005 == 0){ _throw( Null_Exception);} _temp2005->tl;}), cnt --){ struct Cyc_Absyn_Pat*
p2=( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List* _temp2006= rps; if(
_temp2006 == 0){ _throw( Null_Exception);} _temp2006->hd;}); struct _tuple1* v=
Cyc_Toc_temp_var(); void* t2=( void*)({ struct Cyc_Core_Opt* _temp2007= p2->topt;
if( _temp2007 == 0){ _throw( Null_Exception);} _temp2007->v;}); struct
_tagged_string* lab= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp2008=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2008->hd=( void*)({ struct _tuple14* _temp2009=( struct _tuple14*)
GC_malloc( sizeof( struct _tuple14)); _temp2009->f1= v; _temp2009->f2= Cyc_Toc_typ_to_c(
t2); _temp2009;}); _temp2008->tl= decls; _temp2008;});{ struct _tuple13 result=
Cyc_Toc_xlate_pat( nv, t2, Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structmember_exp(
path,({ struct _tagged_string* _temp2010=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp2010[ 0]= xprintf("f%d", cnt);
_temp2010;}), 0), p2, succ_lab, fail_lab, decls); nv= result.f1; decls= result.f2;{
struct Cyc_Absyn_Stmt* s2= result.f3; struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structmember_exp( r,({
struct _tagged_string* _temp2011=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp2011[ 0]= xprintf("f%d", cnt); _temp2011;}), 0),
0), s2, 0); ss=({ struct Cyc_List_List* _temp2012=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2012->hd=( void*) Cyc_Absyn_label_stmt(
lab, s3, 0); _temp2012->tl= ss; _temp2012;}); succ_lab= lab;}}}} s= Cyc_Absyn_seq_stmts(
ss, 0); goto _LL1798;} _LL1824: { struct Cyc_List_List* ss= 0;{ struct Cyc_List_List*
dlps=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
_temp1926); for( 0; dlps != 0; dlps=({ struct Cyc_List_List* _temp2013= dlps;
if( _temp2013 == 0){ _throw( Null_Exception);} _temp2013->tl;})){ struct
_tuple15* tup=( struct _tuple15*)({ struct Cyc_List_List* _temp2014= dlps; if(
_temp2014 == 0){ _throw( Null_Exception);} _temp2014->hd;}); struct Cyc_Absyn_Pat*
p2=(* tup).f2; struct _tagged_string* f;{ void* _temp2016=( void*)({ struct Cyc_List_List*
_temp2015=(* tup).f1; if( _temp2015 == 0){ _throw( Null_Exception);} _temp2015->hd;});
struct _tagged_string* _temp2022; _LL2018: if((( struct _tunion_struct*)
_temp2016)->tag == Cyc_Absyn_FieldName_tag){ _LL2023: _temp2022=( struct
_tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp2016)->f1; goto
_LL2019;} else{ goto _LL2020;} _LL2020: goto _LL2021; _LL2019: f= _temp2022;
goto _LL2017; _LL2021:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Toc_Match_error_struct*
_temp2024=( struct Cyc_Toc_Match_error_struct*) GC_malloc( sizeof( struct Cyc_Toc_Match_error_struct));
_temp2024[ 0]=({ struct Cyc_Toc_Match_error_struct _temp2025; _temp2025.tag= Cyc_Toc_Match_error_tag;
_temp2025;}); _temp2024;})); _LL2017:;}{ struct _tuple1* v= Cyc_Toc_temp_var();
void* t2=( void*)({ struct Cyc_Core_Opt* _temp2026= p2->topt; if( _temp2026 == 0){
_throw( Null_Exception);} _temp2026->v;}); void* t2c= Cyc_Toc_typ_to_c( t2);
struct _tagged_string* lab= Cyc_Toc_fresh_label(); decls=({ struct Cyc_List_List*
_temp2027=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2027->hd=( void*)({ struct _tuple14* _temp2028=( struct _tuple14*)
GC_malloc( sizeof( struct _tuple14)); _temp2028->f1= v; _temp2028->f2= t2c;
_temp2028;}); _temp2027->tl= decls; _temp2027;});{ struct _tuple13 result= Cyc_Toc_xlate_pat(
nv, t2, Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_structmember_exp( path, f, 0), p2,
succ_lab, fail_lab, decls); nv= result.f1; decls= result.f2;{ struct Cyc_Absyn_Stmt*
s2= result.f3; struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt( Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_cast_exp( t2c, Cyc_Absyn_structmember_exp( r,
f, 0), 0), 0), s2, 0); ss=({ struct Cyc_List_List* _temp2029=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2029->hd=( void*) Cyc_Absyn_label_stmt(
lab, s3, 0); _temp2029->tl= ss; _temp2029;}); succ_lab= lab;}}}}} s= Cyc_Absyn_seq_stmts(
ss, 0); goto _LL1798;} _LL1826: { struct _tuple1* v= Cyc_Toc_temp_var(); void*
t2=( void*)({ struct Cyc_Core_Opt* _temp2030= _temp1934->topt; if( _temp2030 ==
0){ _throw( Null_Exception);} _temp2030->v;}); decls=({ struct Cyc_List_List*
_temp2031=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2031->hd=( void*)({ struct _tuple14* _temp2032=( struct _tuple14*)
GC_malloc( sizeof( struct _tuple14)); _temp2032->f1= v; _temp2032->f2= Cyc_Toc_typ_to_c(
t2); _temp2032;}); _temp2031->tl= decls; _temp2031;});{ struct _tuple13 result=
Cyc_Toc_xlate_pat( nv, t2, Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_deref_exp( path,
0), _temp1934, succ_lab, fail_lab, decls); nv= result.f1; decls= result.f2;{
struct Cyc_Absyn_Stmt* s2= result.f3; struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt( Cyc_Absyn_var_exp( v, 0), Cyc_Absyn_deref_exp( r, 0), 0),
s2, 0); if( Cyc_Toc_is_nullable( t)){ s= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_eq_exp(
r, Cyc_Absyn_signed_int_exp( 0, 0), 0), Cyc_Absyn_goto_stmt( fail_lab, 0), s3, 0);}
else{ s= s3;} goto _LL1798;}}} _LL1828: s=(( struct Cyc_Absyn_Stmt*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp2033=(
char*)"unknownid"; struct _tagged_string _temp2034; _temp2034.curr= _temp2033;
_temp2034.base= _temp2033; _temp2034.last_plus_one= _temp2033 + 10; _temp2034;}));
goto _LL1798; _LL1830: s=(( struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp2035=( char*)"unknowncall";
struct _tagged_string _temp2036; _temp2036.curr= _temp2035; _temp2036.base=
_temp2035; _temp2036.last_plus_one= _temp2035 + 12; _temp2036;})); goto _LL1798;
_LL1832: s=(( struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)((
struct _tagged_string)({ char* _temp2037=( char*)"unknownfields"; struct
_tagged_string _temp2038; _temp2038.curr= _temp2037; _temp2038.base= _temp2037;
_temp2038.last_plus_one= _temp2037 + 14; _temp2038;})); goto _LL1798; _LL1834:
goto _LL1836; _LL1836: s=(( struct Cyc_Absyn_Stmt*(*)( struct _tagged_string s))
Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp2039=( char*)"non-null [x]tunion pattern not under &";
struct _tagged_string _temp2040; _temp2040.curr= _temp2039; _temp2040.base=
_temp2039; _temp2040.last_plus_one= _temp2039 + 39; _temp2040;})); goto _LL1798;
_LL1798:;} return({ struct _tuple13 _temp2041; _temp2041.f1= nv; _temp2041.f2=
decls; _temp2041.f3= s; _temp2041;});} static struct _tuple16* Cyc_Toc_gen_label(
struct Cyc_Absyn_Switch_clause* sc){ return({ struct _tuple16* _temp2042=(
struct _tuple16*) GC_malloc( sizeof( struct _tuple16)); _temp2042->f1= Cyc_Toc_fresh_label();
_temp2042->f2= Cyc_Toc_fresh_label(); _temp2042->f3= sc; _temp2042;});} static
void Cyc_Toc_xlate_switch( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Stmt*
whole_s, struct Cyc_Absyn_Exp* e, struct Cyc_List_List* scs){ Cyc_Toc_exp_to_c(
nv, e);{ void* t=( void*)({ struct Cyc_Core_Opt* _temp2043= e->topt; if(
_temp2043 == 0){ _throw( Null_Exception);} _temp2043->v;}); int leave_as_switch;{
void* _temp2044= Cyc_Tcutil_compress( t); void* _temp2050; void* _temp2052;
_LL2046: if(( unsigned int) _temp2044 > 4u?(( struct _tunion_struct*) _temp2044)->tag
== Cyc_Absyn_IntType_tag: 0){ _LL2053: _temp2052=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2044)->f1; goto _LL2051; _LL2051: _temp2050=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2044)->f2; goto _LL2047;} else{ goto _LL2048;} _LL2048: goto _LL2049;
_LL2047: leave_as_switch= 1; goto _LL2045; _LL2049: leave_as_switch= 0; goto
_LL2045; _LL2045:;}{ struct Cyc_List_List* scs1= scs; for( 0; scs1 != 0; scs1=({
struct Cyc_List_List* _temp2054= scs1; if( _temp2054 == 0){ _throw(
Null_Exception);} _temp2054->tl;})){ if(( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp2056=(( struct Cyc_Absyn_Switch_clause*)({ struct Cyc_List_List* _temp2055=
scs1; if( _temp2055 == 0){ _throw( Null_Exception);} _temp2055->hd;}))->pat_vars;
if( _temp2056 == 0){ _throw( Null_Exception);} _temp2056->v;}) != 0){
leave_as_switch= 0; break;}}} if( leave_as_switch){ struct _tagged_string*
next_l= Cyc_Toc_fresh_label();{ struct Cyc_List_List* scs1= scs; for( 0; scs1 !=
0; scs1=({ struct Cyc_List_List* _temp2057= scs1; if( _temp2057 == 0){ _throw(
Null_Exception);} _temp2057->tl;})){ struct Cyc_Absyn_Stmt* s=(( struct Cyc_Absyn_Switch_clause*)({
struct Cyc_List_List* _temp2058= scs1; if( _temp2058 == 0){ _throw(
Null_Exception);} _temp2058->hd;}))->body;(( struct Cyc_Absyn_Switch_clause*)({
struct Cyc_List_List* _temp2059= scs1; if( _temp2059 == 0){ _throw(
Null_Exception);} _temp2059->hd;}))->body= Cyc_Absyn_label_stmt( next_l, s, 0);
next_l= Cyc_Toc_fresh_label(); Cyc_Toc_stmt_to_c( Cyc_Toc_switch_as_switch_env(
nv, next_l), s);}} return;}{ struct _tuple1* v= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp*
r= Cyc_Absyn_var_exp( v, 0); struct Cyc_Absyn_Exp* path= Cyc_Absyn_var_exp( v, 0);
struct _tagged_string* end_l= Cyc_Toc_fresh_label(); struct Cyc_List_List* lscs=((
struct Cyc_List_List*(*)( struct _tuple16*(* f)( struct Cyc_Absyn_Switch_clause*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Toc_gen_label, scs); struct Cyc_List_List*
test_stmts= 0; struct Cyc_List_List* nvs= 0; struct Cyc_List_List* decls= 0;{
struct Cyc_List_List* lscs2= lscs; for( 0; lscs2 != 0; lscs2=({ struct Cyc_List_List*
_temp2060= lscs2; if( _temp2060 == 0){ _throw( Null_Exception);} _temp2060->tl;})){
struct Cyc_Absyn_Switch_clause* sc=(*(( struct _tuple16*)({ struct Cyc_List_List*
_temp2061= lscs2; if( _temp2061 == 0){ _throw( Null_Exception);} _temp2061->hd;}))).f3;
struct _tagged_string* fail_lab=({ struct Cyc_List_List* _temp2062= lscs2; if(
_temp2062 == 0){ _throw( Null_Exception);} _temp2062->tl;}) == 0? end_l:(*((
struct _tuple16*)({ struct Cyc_List_List* _temp2064=({ struct Cyc_List_List*
_temp2063= lscs2; if( _temp2063 == 0){ _throw( Null_Exception);} _temp2063->tl;});
if( _temp2064 == 0){ _throw( Null_Exception);} _temp2064->hd;}))).f1; struct
_tagged_string* succ_lab=(*(( struct _tuple16*)({ struct Cyc_List_List*
_temp2065= lscs2; if( _temp2065 == 0){ _throw( Null_Exception);} _temp2065->hd;}))).f2;
if( sc->where_clause == 0){ struct _tuple13 res= Cyc_Toc_xlate_pat( nv, t, r,
path, sc->pattern, succ_lab, fail_lab, decls); nvs=({ struct Cyc_List_List*
_temp2066=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2066->hd=( void*) res.f1; _temp2066->tl= nvs; _temp2066;}); decls= res.f2;
test_stmts=({ struct Cyc_List_List* _temp2067=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2067->hd=( void*) Cyc_Absyn_label_stmt((*((
struct _tuple16*)({ struct Cyc_List_List* _temp2068= lscs2; if( _temp2068 == 0){
_throw( Null_Exception);} _temp2068->hd;}))).f1, res.f3, 0); _temp2067->tl=
test_stmts; _temp2067;});} else{ struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp2069= sc->where_clause; if( _temp2069 == 0){ _throw(
Null_Exception);} _temp2069;}); struct _tagged_string* intermed_lab= Cyc_Toc_fresh_label();
struct _tuple13 res= Cyc_Toc_xlate_pat( nv, t, r, path, sc->pattern,
intermed_lab, fail_lab, decls); Cyc_Toc_exp_to_c( res.f1, e); nvs=({ struct Cyc_List_List*
_temp2070=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2070->hd=( void*) res.f1; _temp2070->tl= nvs; _temp2070;}); decls= res.f2;{
struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_ifthenelse_stmt( e, Cyc_Absyn_goto_stmt(
succ_lab, 0), Cyc_Absyn_goto_stmt( fail_lab, 0), 0); struct Cyc_Absyn_Stmt* s3=
Cyc_Absyn_label_stmt( intermed_lab, s2, 0); test_stmts=({ struct Cyc_List_List*
_temp2071=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2071->hd=( void*) Cyc_Absyn_label_stmt((*(( struct _tuple16*)({ struct Cyc_List_List*
_temp2072= lscs2; if( _temp2072 == 0){ _throw( Null_Exception);} _temp2072->hd;}))).f1,
Cyc_Absyn_seq_stmt( res.f3, s3, 0), 0); _temp2071->tl= test_stmts; _temp2071;});}}}}{
struct Cyc_Absyn_Stmt* test_stmt= Cyc_Absyn_seq_stmts((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( test_stmts), 0); nvs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( nvs);{ struct Cyc_Absyn_Stmt*
clauses= Cyc_Absyn_skip_stmt( 0); if( lscs != 0){ for( 0;({ struct Cyc_List_List*
_temp2073= lscs; if( _temp2073 == 0){ _throw( Null_Exception);} _temp2073->tl;})
!= 0; lscs=({ struct Cyc_List_List* _temp2074= lscs; if( _temp2074 == 0){ _throw(
Null_Exception);} _temp2074->tl;}), nvs=({ struct Cyc_List_List* _temp2075= nvs;
if( _temp2075 == 0){ _throw( Null_Exception);} _temp2075->tl;})){ struct Cyc_Toc_Env*
e=( struct Cyc_Toc_Env*)({ struct Cyc_List_List* _temp2076= nvs; if( _temp2076
== 0){ _throw( Null_Exception);} _temp2076->hd;}); struct Cyc_Absyn_Stmt* s=((*((
struct _tuple16*)({ struct Cyc_List_List* _temp2077= lscs; if( _temp2077 == 0){
_throw( Null_Exception);} _temp2077->hd;}))).f3)->body; Cyc_Toc_stmt_to_c( Cyc_Toc_non_last_switchclause_env(
e, end_l,(*(( struct _tuple16*)({ struct Cyc_List_List* _temp2079=({ struct Cyc_List_List*
_temp2078= lscs; if( _temp2078 == 0){ _throw( Null_Exception);} _temp2078->tl;});
if( _temp2079 == 0){ _throw( Null_Exception);} _temp2079->hd;}))).f2,( struct
Cyc_List_List*)({ struct Cyc_Core_Opt* _temp2082=((*(( struct _tuple16*)({
struct Cyc_List_List* _temp2081=({ struct Cyc_List_List* _temp2080= lscs; if(
_temp2080 == 0){ _throw( Null_Exception);} _temp2080->tl;}); if( _temp2081 == 0){
_throw( Null_Exception);} _temp2081->hd;}))).f3)->pat_vars; if( _temp2082 == 0){
_throw( Null_Exception);} _temp2082->v;}),( struct Cyc_Toc_Env*)({ struct Cyc_List_List*
_temp2084=({ struct Cyc_List_List* _temp2083= nvs; if( _temp2083 == 0){ _throw(
Null_Exception);} _temp2083->tl;}); if( _temp2084 == 0){ _throw( Null_Exception);}
_temp2084->hd;})), s); clauses= Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*((
struct _tuple16*)({ struct Cyc_List_List* _temp2085= lscs; if( _temp2085 == 0){
_throw( Null_Exception);} _temp2085->hd;}))).f2, s, 0), 0);}{ struct Cyc_Toc_Env*
e=( struct Cyc_Toc_Env*)({ struct Cyc_List_List* _temp2086= nvs; if( _temp2086
== 0){ _throw( Null_Exception);} _temp2086->hd;}); struct Cyc_Absyn_Stmt* s=((*((
struct _tuple16*)({ struct Cyc_List_List* _temp2087= lscs; if( _temp2087 == 0){
_throw( Null_Exception);} _temp2087->hd;}))).f3)->body; Cyc_Toc_stmt_to_c( Cyc_Toc_last_switchclause_env(
e, end_l), s); clauses= Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt((*((
struct _tuple16*)({ struct Cyc_List_List* _temp2088= lscs; if( _temp2088 == 0){
_throw( Null_Exception);} _temp2088->hd;}))).f2, s, 0), 0);}}{ struct Cyc_Absyn_Stmt*
res= Cyc_Absyn_seq_stmt( test_stmt, Cyc_Absyn_seq_stmt( clauses, Cyc_Absyn_label_stmt(
end_l, Cyc_Absyn_skip_stmt( 0), 0), 0), 0); for( decls; decls != 0; decls=({
struct Cyc_List_List* _temp2089= decls; if( _temp2089 == 0){ _throw(
Null_Exception);} _temp2089->tl;})){ struct _tuple14 _temp2093; void* _temp2094;
struct _tuple1* _temp2096; struct _tuple14* _temp2091=( struct _tuple14*)({
struct Cyc_List_List* _temp2090= decls; if( _temp2090 == 0){ _throw(
Null_Exception);} _temp2090->hd;}); _temp2093=* _temp2091; _LL2097: _temp2096=
_temp2093.f1; goto _LL2095; _LL2095: _temp2094= _temp2093.f2; goto _LL2092;
_LL2092: res= Cyc_Absyn_declare_stmt( _temp2096, _temp2094, 0, res, 0);}( void*)(
whole_s->r=( void*)(( void*)( Cyc_Absyn_declare_stmt( v, Cyc_Toc_typ_to_c(( void*)({
struct Cyc_Core_Opt* _temp2098= e->topt; if( _temp2098 == 0){ _throw(
Null_Exception);} _temp2098->v;})),( struct Cyc_Absyn_Exp*) e, res, 0))->r));}}}}}}
static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c( struct Cyc_Toc_Env* nv,
struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt, struct Cyc_Absyn_Exp* e, int
exhaust, struct Cyc_Absyn_Stmt* s); static struct _tuple17 Cyc_Toc_decls_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_List_List* ds, int top); struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler( int n){ return Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
Cyc_Absyn_var_exp( Cyc_Toc__npop_handler_qv, 0),({ struct Cyc_List_List*
_temp2099=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp2099[ 0]=({ struct Cyc_List_List _temp2100; _temp2100.hd=( void*) Cyc_Absyn_uint_exp((
unsigned int)( n - 1), 0); _temp2100.tl= 0; _temp2100;}); _temp2099;}), 0), 0);}
void Cyc_Toc_do_npop_before( int n, struct Cyc_Absyn_Stmt* s){ if( n > 0){( void*)(
s->r=( void*)(( void*)( Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( n), Cyc_Absyn_new_stmt((
void*) s->r, 0), 0))->r));}} static void Cyc_Toc_stmt_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Stmt* s){ while( 1) { void* _temp2101=( void*) s->r; struct
Cyc_Absyn_Exp* _temp2141; struct Cyc_Absyn_Stmt* _temp2143; struct Cyc_Absyn_Stmt*
_temp2145; struct Cyc_Absyn_Exp* _temp2147; struct Cyc_Absyn_Stmt* _temp2149;
struct Cyc_Absyn_Stmt* _temp2151; struct Cyc_Absyn_Exp* _temp2153; struct Cyc_Absyn_Stmt*
_temp2155; struct _tuple3 _temp2157; struct Cyc_Absyn_Stmt* _temp2159; struct
Cyc_Absyn_Exp* _temp2161; struct Cyc_Absyn_Stmt* _temp2163; struct Cyc_Absyn_Stmt*
_temp2165; struct Cyc_Absyn_Stmt* _temp2167; struct _tagged_string* _temp2169;
struct Cyc_Absyn_Stmt* _temp2171; struct _tuple3 _temp2173; struct Cyc_Absyn_Stmt*
_temp2175; struct Cyc_Absyn_Exp* _temp2177; struct _tuple3 _temp2179; struct Cyc_Absyn_Stmt*
_temp2181; struct Cyc_Absyn_Exp* _temp2183; struct Cyc_Absyn_Exp* _temp2185;
struct Cyc_List_List* _temp2187; struct Cyc_Absyn_Exp* _temp2189; struct Cyc_Absyn_Switch_clause**
_temp2191; struct Cyc_List_List* _temp2193; struct Cyc_Absyn_Stmt* _temp2195;
struct Cyc_Absyn_Decl* _temp2197; struct Cyc_Absyn_Stmt* _temp2199; struct Cyc_Absyn_Stmt*
_temp2201; struct Cyc_Absyn_Stmt* _temp2203; struct _tagged_string* _temp2205;
struct _tuple3 _temp2207; struct Cyc_Absyn_Stmt* _temp2209; struct Cyc_Absyn_Exp*
_temp2211; struct Cyc_Absyn_Stmt* _temp2213; struct Cyc_List_List* _temp2215;
struct Cyc_Absyn_Stmt* _temp2217; struct Cyc_Absyn_Stmt* _temp2219; struct Cyc_Absyn_Vardecl*
_temp2221; struct Cyc_Absyn_Tvar* _temp2223; _LL2103: if(( int) _temp2101 == Cyc_Absyn_Skip_s){
goto _LL2104;} else{ goto _LL2105;} _LL2105: if(( unsigned int) _temp2101 > 1u?((
struct _tunion_struct*) _temp2101)->tag == Cyc_Absyn_Exp_s_tag: 0){ _LL2142:
_temp2141=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*) _temp2101)->f1;
goto _LL2106;} else{ goto _LL2107;} _LL2107: if(( unsigned int) _temp2101 > 1u?((
struct _tunion_struct*) _temp2101)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL2146:
_temp2145=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp2101)->f1;
goto _LL2144; _LL2144: _temp2143=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp2101)->f2; goto _LL2108;} else{ goto _LL2109;} _LL2109: if(( unsigned int)
_temp2101 > 1u?(( struct _tunion_struct*) _temp2101)->tag == Cyc_Absyn_Return_s_tag:
0){ _LL2148: _temp2147=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp2101)->f1; goto _LL2110;} else{ goto _LL2111;} _LL2111: if(( unsigned int)
_temp2101 > 1u?(( struct _tunion_struct*) _temp2101)->tag == Cyc_Absyn_IfThenElse_s_tag:
0){ _LL2154: _temp2153=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2101)->f1; goto _LL2152; _LL2152: _temp2151=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp2101)->f2; goto _LL2150; _LL2150:
_temp2149=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2101)->f3; goto _LL2112;} else{ goto _LL2113;} _LL2113: if(( unsigned int)
_temp2101 > 1u?(( struct _tunion_struct*) _temp2101)->tag == Cyc_Absyn_While_s_tag:
0){ _LL2158: _temp2157=( struct _tuple3)(( struct Cyc_Absyn_While_s_struct*)
_temp2101)->f1; _LL2162: _temp2161= _temp2157.f1; goto _LL2160; _LL2160:
_temp2159= _temp2157.f2; goto _LL2156; _LL2156: _temp2155=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_While_s_struct*) _temp2101)->f2; goto _LL2114;} else{ goto
_LL2115;} _LL2115: if(( unsigned int) _temp2101 > 1u?(( struct _tunion_struct*)
_temp2101)->tag == Cyc_Absyn_Break_s_tag: 0){ _LL2164: _temp2163=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Break_s_struct*) _temp2101)->f1; goto _LL2116;} else{ goto
_LL2117;} _LL2117: if(( unsigned int) _temp2101 > 1u?(( struct _tunion_struct*)
_temp2101)->tag == Cyc_Absyn_Continue_s_tag: 0){ _LL2166: _temp2165=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Continue_s_struct*) _temp2101)->f1; goto _LL2118;} else{ goto
_LL2119;} _LL2119: if(( unsigned int) _temp2101 > 1u?(( struct _tunion_struct*)
_temp2101)->tag == Cyc_Absyn_Goto_s_tag: 0){ _LL2170: _temp2169=( struct
_tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp2101)->f1; goto _LL2168;
_LL2168: _temp2167=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp2101)->f2; goto _LL2120;} else{ goto _LL2121;} _LL2121: if(( unsigned int)
_temp2101 > 1u?(( struct _tunion_struct*) _temp2101)->tag == Cyc_Absyn_For_s_tag:
0){ _LL2186: _temp2185=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp2101)->f1; goto _LL2180; _LL2180: _temp2179=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp2101)->f2; _LL2184: _temp2183= _temp2179.f1; goto _LL2182; _LL2182:
_temp2181= _temp2179.f2; goto _LL2174; _LL2174: _temp2173=( struct _tuple3)((
struct Cyc_Absyn_For_s_struct*) _temp2101)->f3; _LL2178: _temp2177= _temp2173.f1;
goto _LL2176; _LL2176: _temp2175= _temp2173.f2; goto _LL2172; _LL2172: _temp2171=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp2101)->f4; goto
_LL2122;} else{ goto _LL2123;} _LL2123: if(( unsigned int) _temp2101 > 1u?((
struct _tunion_struct*) _temp2101)->tag == Cyc_Absyn_Switch_s_tag: 0){ _LL2190:
_temp2189=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp2101)->f1; goto _LL2188; _LL2188: _temp2187=( struct Cyc_List_List*)((
struct Cyc_Absyn_Switch_s_struct*) _temp2101)->f2; goto _LL2124;} else{ goto
_LL2125;} _LL2125: if(( unsigned int) _temp2101 > 1u?(( struct _tunion_struct*)
_temp2101)->tag == Cyc_Absyn_Fallthru_s_tag: 0){ _LL2194: _temp2193=( struct Cyc_List_List*)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp2101)->f1; goto _LL2192; _LL2192:
_temp2191=( struct Cyc_Absyn_Switch_clause**)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp2101)->f2; goto _LL2126;} else{ goto _LL2127;} _LL2127: if(( unsigned int)
_temp2101 > 1u?(( struct _tunion_struct*) _temp2101)->tag == Cyc_Absyn_Decl_s_tag:
0){ _LL2198: _temp2197=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp2101)->f1; goto _LL2196; _LL2196: _temp2195=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Decl_s_struct*) _temp2101)->f2; goto _LL2128;} else{ goto
_LL2129;} _LL2129: if(( unsigned int) _temp2101 > 1u?(( struct _tunion_struct*)
_temp2101)->tag == Cyc_Absyn_Cut_s_tag: 0){ _LL2200: _temp2199=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Cut_s_struct*) _temp2101)->f1; goto _LL2130;} else{ goto
_LL2131;} _LL2131: if(( unsigned int) _temp2101 > 1u?(( struct _tunion_struct*)
_temp2101)->tag == Cyc_Absyn_Splice_s_tag: 0){ _LL2202: _temp2201=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Splice_s_struct*) _temp2101)->f1; goto _LL2132;} else{ goto
_LL2133;} _LL2133: if(( unsigned int) _temp2101 > 1u?(( struct _tunion_struct*)
_temp2101)->tag == Cyc_Absyn_Label_s_tag: 0){ _LL2206: _temp2205=( struct
_tagged_string*)(( struct Cyc_Absyn_Label_s_struct*) _temp2101)->f1; goto
_LL2204; _LL2204: _temp2203=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp2101)->f2; goto _LL2134;} else{ goto _LL2135;} _LL2135: if(( unsigned int)
_temp2101 > 1u?(( struct _tunion_struct*) _temp2101)->tag == Cyc_Absyn_Do_s_tag:
0){ _LL2214: _temp2213=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*)
_temp2101)->f1; goto _LL2208; _LL2208: _temp2207=( struct _tuple3)(( struct Cyc_Absyn_Do_s_struct*)
_temp2101)->f2; _LL2212: _temp2211= _temp2207.f1; goto _LL2210; _LL2210:
_temp2209= _temp2207.f2; goto _LL2136;} else{ goto _LL2137;} _LL2137: if((
unsigned int) _temp2101 > 1u?(( struct _tunion_struct*) _temp2101)->tag == Cyc_Absyn_TryCatch_s_tag:
0){ _LL2218: _temp2217=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp2101)->f1; goto _LL2216; _LL2216: _temp2215=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp2101)->f2; goto _LL2138;} else{ goto
_LL2139;} _LL2139: if(( unsigned int) _temp2101 > 1u?(( struct _tunion_struct*)
_temp2101)->tag == Cyc_Absyn_Region_s_tag: 0){ _LL2224: _temp2223=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_Region_s_struct*) _temp2101)->f1; goto _LL2222; _LL2222:
_temp2221=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Region_s_struct*)
_temp2101)->f2; goto _LL2220; _LL2220: _temp2219=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Region_s_struct*) _temp2101)->f3; goto _LL2140;} else{ goto
_LL2102;} _LL2104: return; _LL2106: Cyc_Toc_exp_to_c( nv, _temp2141); return;
_LL2108: Cyc_Toc_stmt_to_c( nv, _temp2145); s= _temp2143; continue; _LL2110: {
struct Cyc_Core_Opt* topt= 0; if( _temp2147 != 0){ topt=({ struct Cyc_Core_Opt*
_temp2225=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2225->v=( void*) Cyc_Toc_typ_to_c(( void*)({ struct Cyc_Core_Opt* _temp2227=({
struct Cyc_Absyn_Exp* _temp2226= _temp2147; if( _temp2226 == 0){ _throw(
Null_Exception);} _temp2226->topt;}); if( _temp2227 == 0){ _throw(
Null_Exception);} _temp2227->v;})); _temp2225;}); Cyc_Toc_exp_to_c( nv,( struct
Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2228= _temp2147; if( _temp2228 == 0){
_throw( Null_Exception);} _temp2228;}));} if( s->try_depth > 0){ if( topt != 0){
struct _tuple1* x= Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt* retn_stmt= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_Absyn_var_exp( x, 0), 0);( void*)( s->r=( void*)((
void*)( Cyc_Absyn_declare_stmt( x,( void*)({ struct Cyc_Core_Opt* _temp2229=
topt; if( _temp2229 == 0){ _throw( Null_Exception);} _temp2229->v;}), _temp2147,
Cyc_Absyn_seq_stmt( Cyc_Toc_make_npop_handler( s->try_depth), retn_stmt, 0), 0))->r));}
else{ Cyc_Toc_do_npop_before( s->try_depth, s);}} return;} _LL2112: Cyc_Toc_exp_to_c(
nv, _temp2153); Cyc_Toc_stmt_to_c( nv, _temp2151); s= _temp2149; continue;
_LL2114: Cyc_Toc_exp_to_c( nv, _temp2161); Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env(
nv), _temp2155); return; _LL2116: if( nv->break_lab != 0){( void*)( s->r=( void*)((
void*)( Cyc_Absyn_goto_stmt(( struct _tagged_string*)({ struct Cyc_Core_Opt*
_temp2230= nv->break_lab; if( _temp2230 == 0){ _throw( Null_Exception);}
_temp2230->v;}), 0))->r));}{ int dest_depth= _temp2163 == 0? 0:({ struct Cyc_Absyn_Stmt*
_temp2231= _temp2163; if( _temp2231 == 0){ _throw( Null_Exception);} _temp2231->try_depth;});
Cyc_Toc_do_npop_before( s->try_depth - dest_depth, s); return;} _LL2118: if( nv->continue_lab
!= 0){( void*)( s->r=( void*)(( void*)( Cyc_Absyn_goto_stmt(( struct
_tagged_string*)({ struct Cyc_Core_Opt* _temp2232= nv->continue_lab; if(
_temp2232 == 0){ _throw( Null_Exception);} _temp2232->v;}), 0))->r));} Cyc_Toc_do_npop_before(
s->try_depth -({ struct Cyc_Absyn_Stmt* _temp2233= _temp2165; if( _temp2233 == 0){
_throw( Null_Exception);} _temp2233->try_depth;}), s); return; _LL2120: Cyc_Toc_do_npop_before(
s->try_depth -({ struct Cyc_Absyn_Stmt* _temp2234= _temp2167; if( _temp2234 == 0){
_throw( Null_Exception);} _temp2234->try_depth;}), s); return; _LL2122: Cyc_Toc_exp_to_c(
nv, _temp2185); Cyc_Toc_exp_to_c( nv, _temp2183); Cyc_Toc_exp_to_c( nv,
_temp2177); Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env( nv), _temp2171); return;
_LL2124: Cyc_Toc_xlate_switch( nv, s, _temp2189, _temp2187); return; _LL2126:
if( nv->fallthru_info == 0){( void) _throw((( struct _xtunion_struct*(*)( struct
_tagged_string s)) Cyc_Toc_toc_impos)(( struct _tagged_string)({ char* _temp2235=(
char*)"fallthru in unexpected place"; struct _tagged_string _temp2236; _temp2236.curr=
_temp2235; _temp2236.base= _temp2235; _temp2236.last_plus_one= _temp2235 + 29;
_temp2236;})));}{ struct _tuple8 _temp2240; struct Cyc_Dict_Dict* _temp2241;
struct Cyc_List_List* _temp2243; struct _tagged_string* _temp2245; struct
_tuple8* _temp2238=( struct _tuple8*)({ struct Cyc_Core_Opt* _temp2237= nv->fallthru_info;
if( _temp2237 == 0){ _throw( Null_Exception);} _temp2237->v;}); _temp2240=*
_temp2238; _LL2246: _temp2245= _temp2240.f1; goto _LL2244; _LL2244: _temp2243=
_temp2240.f2; goto _LL2242; _LL2242: _temp2241= _temp2240.f3; goto _LL2239;
_LL2239: { struct Cyc_Absyn_Stmt* s2= Cyc_Absyn_goto_stmt( _temp2245, 0); Cyc_Toc_do_npop_before(
s->try_depth -(({ struct Cyc_Absyn_Switch_clause** _temp2247= _temp2191; if(
_temp2247 == 0){ _throw( Null_Exception);}* _temp2247;})->body)->try_depth, s2);{
struct Cyc_List_List* vs2=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_rev)( _temp2243); struct Cyc_List_List* es2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( _temp2193); for( 0; vs2 != 0; vs2=({
struct Cyc_List_List* _temp2248= vs2; if( _temp2248 == 0){ _throw(
Null_Exception);} _temp2248->tl;}), es2=({ struct Cyc_List_List* _temp2249= es2;
if( _temp2249 == 0){ _throw( Null_Exception);} _temp2249->tl;})){ Cyc_Toc_exp_to_c(
nv,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp2250= es2; if(
_temp2250 == 0){ _throw( Null_Exception);} _temp2250->hd;})); s2= Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt((( struct Cyc_Absyn_Exp*(*)( struct Cyc_Dict_Dict* d,
struct _tuple1* key)) Cyc_Dict_lookup)( _temp2241,( struct _tuple1*)({ struct
Cyc_List_List* _temp2251= vs2; if( _temp2251 == 0){ _throw( Null_Exception);}
_temp2251->hd;})),( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp2252=
es2; if( _temp2252 == 0){ _throw( Null_Exception);} _temp2252->hd;}), 0), s2, 0);}(
void*)( s->r=( void*)(( void*) s2->r)); return;}}} _LL2128:{ void* _temp2253=(
void*) _temp2197->r; int _temp2259; struct Cyc_Absyn_Exp* _temp2261; struct Cyc_Core_Opt*
_temp2263; struct Cyc_Core_Opt* _temp2265; struct Cyc_Absyn_Pat* _temp2267;
_LL2255: if((( struct _tunion_struct*) _temp2253)->tag == Cyc_Absyn_Let_d_tag){
_LL2268: _temp2267=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*)
_temp2253)->f1; goto _LL2266; _LL2266: _temp2265=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Let_d_struct*) _temp2253)->f2; goto _LL2264; _LL2264: _temp2263=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp2253)->f3; goto
_LL2262; _LL2262: _temp2261=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp2253)->f4; goto _LL2260; _LL2260: _temp2259=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp2253)->f5; goto _LL2256;} else{ goto _LL2257;} _LL2257: goto _LL2258;
_LL2256: { int var_case= 0;{ void* _temp2269=( void*) _temp2267->r; struct Cyc_Absyn_Vardecl*
_temp2275; _LL2271: if(( unsigned int) _temp2269 > 2u?(( struct _tunion_struct*)
_temp2269)->tag == Cyc_Absyn_Var_p_tag: 0){ _LL2276: _temp2275=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_p_struct*) _temp2269)->f1; goto _LL2272;} else{ goto
_LL2273;} _LL2273: goto _LL2274; _LL2272: var_case= 1; _temp2275->initializer=(
struct Cyc_Absyn_Exp*) _temp2261;( void*)( _temp2197->r=( void*)(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2277=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2277[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2278; _temp2278.tag= Cyc_Absyn_Var_d_tag; _temp2278.f1= _temp2275;
_temp2278;}); _temp2277;}))); goto _LL2270; _LL2274: goto _LL2270; _LL2270:;}
if( var_case){ goto _LL2258;}( void*)( s->r=( void*)(( void*)( Cyc_Toc_letdecl_to_c(
nv, _temp2267, _temp2263, _temp2261, _temp2259, _temp2195))->r)); goto _LL2254;}
_LL2258: { struct Cyc_List_List* _temp2282; struct Cyc_Toc_Env* _temp2284;
struct _tuple17 _temp2280= Cyc_Toc_decls_to_c( nv,({ struct Cyc_List_List*
_temp2279=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2279->hd=( void*) _temp2197; _temp2279->tl= 0; _temp2279;}), 0); _LL2285:
_temp2284= _temp2280.f1; goto _LL2283; _LL2283: _temp2282= _temp2280.f2; goto
_LL2281; _LL2281: Cyc_Toc_stmt_to_c( _temp2284, _temp2195); for( 0; _temp2282 !=
0; _temp2282=({ struct Cyc_List_List* _temp2286= _temp2282; if( _temp2286 == 0){
_throw( Null_Exception);} _temp2286->tl;})){ _temp2195= Cyc_Absyn_decl_stmt((
struct Cyc_Absyn_Decl*)({ struct Cyc_List_List* _temp2287= _temp2282; if(
_temp2287 == 0){ _throw( Null_Exception);} _temp2287->hd;}), _temp2195, 0);}(
void*)( s->r=( void*)(( void*) _temp2195->r)); goto _LL2254;} _LL2254:;} return;
_LL2130:(( void(*)( struct _tagged_string s)) Cyc_Toc_unimp)(( struct
_tagged_string)({ char* _temp2288=( char*)"cut"; struct _tagged_string _temp2289;
_temp2289.curr= _temp2288; _temp2289.base= _temp2288; _temp2289.last_plus_one=
_temp2288 + 4; _temp2289;})); return; _LL2132:(( void(*)( struct _tagged_string
s)) Cyc_Toc_unimp)(( struct _tagged_string)({ char* _temp2290=( char*)"splice";
struct _tagged_string _temp2291; _temp2291.curr= _temp2290; _temp2291.base=
_temp2290; _temp2291.last_plus_one= _temp2290 + 7; _temp2291;})); return;
_LL2134: s= _temp2203; continue; _LL2136: Cyc_Toc_stmt_to_c( Cyc_Toc_loop_env(
nv), _temp2213); Cyc_Toc_exp_to_c( nv, _temp2211); return; _LL2138: { struct
_tuple1* h_var= Cyc_Toc_temp_var(); struct _tuple1* e_var= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* h_exp= Cyc_Absyn_var_exp( h_var, 0); struct Cyc_Absyn_Exp*
e_exp= Cyc_Absyn_var_exp( e_var, 0); void* h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp);
void* e_typ= Cyc_Toc_typ_to_c( Cyc_Absyn_exn_typ); struct Cyc_Absyn_Exp*
pushhandler_exp= Cyc_Absyn_var_exp(({ struct _tuple1* _temp2292=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp2292->f1= Cyc_Toc_abs_ns; _temp2292->f2=
Cyc_Toc__push_handler_sp; _temp2292;}), 0); struct Cyc_Absyn_Exp* setjmp_exp=
Cyc_Absyn_var_exp(({ struct _tuple1* _temp2293=( struct _tuple1*) GC_malloc(
sizeof( struct _tuple1)); _temp2293->f1= Cyc_Toc_abs_ns; _temp2293->f2= Cyc_Toc_setjmp_sp;
_temp2293;}), 0); struct Cyc_Absyn_Exp* pophandler_exp= Cyc_Absyn_var_exp(({
struct _tuple1* _temp2294=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp2294->f1= Cyc_Toc_abs_ns; _temp2294->f2= Cyc_Toc__pop_handler_sp; _temp2294;}),
0); e_exp->topt=({ struct Cyc_Core_Opt* _temp2295=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2295->v=( void*) e_typ; _temp2295;});
nv= Cyc_Toc_add_varmap( nv, e_var, e_exp); Cyc_Toc_stmt_to_c( nv, _temp2217);{
struct Cyc_Absyn_Stmt* tryandpop_stmt= Cyc_Absyn_seq_stmt( _temp2217, Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp( pophandler_exp, 0, 0), 0), 0); struct _tuple1* x= Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp* xexp= Cyc_Absyn_var_exp( x, 0); struct Cyc_Absyn_Vardecl*
x_vd= Cyc_Absyn_new_vardecl( x, Cyc_Absyn_exn_typ, 0); xexp->topt=({ struct Cyc_Core_Opt*
_temp2296=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2296->v=( void*) Cyc_Absyn_exn_typ; _temp2296;});{ struct Cyc_Absyn_Pat* p=({
struct Cyc_Absyn_Pat* _temp2297=( struct Cyc_Absyn_Pat*) GC_malloc( sizeof(
struct Cyc_Absyn_Pat)); _temp2297->r=( void*)(( void*)({ struct Cyc_Absyn_Var_p_struct*
_temp2299=( struct Cyc_Absyn_Var_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct));
_temp2299[ 0]=({ struct Cyc_Absyn_Var_p_struct _temp2300; _temp2300.tag= Cyc_Absyn_Var_p_tag;
_temp2300.f1= x_vd; _temp2300;}); _temp2299;})); _temp2297->topt=({ struct Cyc_Core_Opt*
_temp2298=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2298->v=( void*) Cyc_Absyn_exn_typ; _temp2298;}); _temp2297->loc= 0;
_temp2297;}); struct Cyc_Absyn_Exp* te= Cyc_Absyn_throw_exp( xexp, 0); te->topt=({
struct Cyc_Core_Opt* _temp2301=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2301->v=( void*)(( void*) Cyc_Absyn_VoidType); _temp2301;});{
struct Cyc_Absyn_Stmt* rs= Cyc_Absyn_exp_stmt( te, 0); struct Cyc_Absyn_Switch_clause*
default_case=({ struct Cyc_Absyn_Switch_clause* _temp2302=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp2302->pattern= p;
_temp2302->pat_vars=({ struct Cyc_Core_Opt* _temp2303=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2303->v=( void*)({ struct Cyc_List_List*
_temp2304=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2304->hd=( void*) x_vd; _temp2304->tl= 0; _temp2304;}); _temp2303;});
_temp2302->where_clause= 0; _temp2302->body= rs; _temp2302->loc= 0; _temp2302;});
struct Cyc_Absyn_Stmt* handler_stmt= Cyc_Absyn_switch_stmt( e_exp,(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp2215,({
struct Cyc_List_List* _temp2305=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2305->hd=( void*) default_case; _temp2305->tl= 0;
_temp2305;})), 0); Cyc_Toc_stmt_to_c( nv, handler_stmt);{ struct Cyc_Absyn_Exp*
setjmp_call= Cyc_Absyn_cast_exp( e_typ, Cyc_Absyn_fncall_exp( setjmp_exp,({
struct Cyc_List_List* _temp2306=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2306->hd=( void*) Cyc_Absyn_structmember_exp( h_exp,
Cyc_Toc_handler_sp, 0); _temp2306->tl= 0; _temp2306;}), 0), 0);( void*)( s->r=(
void*)(( void*)( Cyc_Absyn_declare_stmt( h_var, h_typ, 0, Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp( pushhandler_exp,({ struct Cyc_List_List*
_temp2307=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2307->hd=( void*) Cyc_Absyn_address_exp( h_exp, 0); _temp2307->tl= 0;
_temp2307;}), 0), 0), Cyc_Absyn_declare_stmt( e_var, e_typ,( struct Cyc_Absyn_Exp*)
setjmp_call, Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_prim1_exp(( void*) Cyc_Absyn_Not,
e_exp, 0), tryandpop_stmt, handler_stmt, 0), 0), 0), 0))->r)); return;}}}}}
_LL2140: { void* rh_struct_typ= Cyc_Absyn_strct( Cyc_Toc__region_handle_sp);
void* rh_struct_ptr_typ= Cyc_Absyn_cstar_typ( rh_struct_typ, Cyc_Toc_mt_tq);
void* h_typ= Cyc_Absyn_strct( Cyc_Toc__handler_cons_sp); void* e_typ= Cyc_Toc_typ_to_c(
Cyc_Absyn_exn_typ); struct _tuple1* rh_var= Cyc_Toc_temp_var(); struct _tuple1*
x_var= _temp2221->name; struct _tuple1* h_var= Cyc_Toc_temp_var(); struct
_tuple1* e_var= Cyc_Toc_temp_var(); struct Cyc_Absyn_Exp* rh_exp= Cyc_Absyn_var_exp(
rh_var, 0); struct Cyc_Absyn_Exp* x_exp= Cyc_Absyn_varb_exp( x_var,( void*)({
struct Cyc_Absyn_Local_b_struct* _temp2308=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp2308[ 0]=({ struct
Cyc_Absyn_Local_b_struct _temp2309; _temp2309.tag= Cyc_Absyn_Local_b_tag;
_temp2309.f1= _temp2221; _temp2309;}); _temp2308;}), 0); struct Cyc_Absyn_Exp*
h_exp= Cyc_Absyn_var_exp( h_var, 0); struct Cyc_Absyn_Exp* e_exp= Cyc_Absyn_var_exp(
e_var, 0); struct Cyc_Absyn_Exp* pushhandler_exp= Cyc_Absyn_var_exp(({ struct
_tuple1* _temp2310=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp2310->f1= Cyc_Toc_abs_ns; _temp2310->f2= Cyc_Toc__push_handler_sp;
_temp2310;}), 0); struct Cyc_Absyn_Exp* setjmp_exp= Cyc_Absyn_var_exp(({ struct
_tuple1* _temp2311=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp2311->f1= Cyc_Toc_abs_ns; _temp2311->f2= Cyc_Toc_setjmp_sp; _temp2311;}), 0);
struct Cyc_Absyn_Exp* pophandler_exp= Cyc_Absyn_var_exp(({ struct _tuple1*
_temp2312=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2312->f1=
Cyc_Toc_abs_ns; _temp2312->f2= Cyc_Toc__pop_handler_sp; _temp2312;}), 0); struct
Cyc_Absyn_Exp* freeregion_exp= Cyc_Absyn_var_exp(({ struct _tuple1* _temp2313=(
struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2313->f1= Cyc_Toc_abs_ns;
_temp2313->f2= Cyc_Toc__free_region_sp; _temp2313;}), 0); struct Cyc_Absyn_Exp*
newregion_exp= Cyc_Absyn_var_exp(({ struct _tuple1* _temp2314=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp2314->f1= Cyc_Toc_abs_ns; _temp2314->f2=
Cyc_Toc__new_region_sp; _temp2314;}), 0); struct Cyc_Absyn_Exp* addr_rh_exp= Cyc_Absyn_address_exp(
rh_exp, 0); struct Cyc_Absyn_Exp* newregion_call= Cyc_Absyn_fncall_exp(
newregion_exp, 0, 0); Cyc_Toc_stmt_to_c( Cyc_Toc_add_varmap( nv, x_var, x_exp),
_temp2219);{ struct Cyc_Absyn_Stmt* free_region_stmt= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
freeregion_exp,({ struct Cyc_List_List* _temp2315=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2315->hd=( void*) addr_rh_exp;
_temp2315->tl= 0; _temp2315;}), 0), 0); struct Cyc_Absyn_Stmt* tryandpop_stmt=
Cyc_Absyn_seq_stmt( _temp2219, Cyc_Absyn_seq_stmt( free_region_stmt, Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp( pophandler_exp, 0, 0), 0), 0), 0); struct Cyc_Absyn_Stmt*
rethrow_stmt= Cyc_Absyn_seq_stmt( free_region_stmt, Cyc_Absyn_exp_stmt( Cyc_Toc_newthrow_exp(
e_exp), 0), 0); struct Cyc_Absyn_Stmt* if_stmt= Cyc_Absyn_ifthenelse_stmt( Cyc_Absyn_prim1_exp((
void*) Cyc_Absyn_Not, e_exp, 0), tryandpop_stmt, rethrow_stmt, 0); struct Cyc_Absyn_Exp*
setjmp_call= Cyc_Absyn_cast_exp( Cyc_Toc_typ_to_c( Cyc_Absyn_exn_typ), Cyc_Absyn_fncall_exp(
setjmp_exp,({ struct Cyc_List_List* _temp2316=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2316->hd=( void*) Cyc_Absyn_structmember_exp(
h_exp, Cyc_Toc_handler_sp, 0); _temp2316->tl= 0; _temp2316;}), 0), 0); struct
Cyc_Absyn_Stmt* push_stmt= Cyc_Absyn_exp_stmt( Cyc_Absyn_fncall_exp(
pushhandler_exp,({ struct Cyc_List_List* _temp2317=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2317->hd=( void*) Cyc_Absyn_address_exp(
h_exp, 0); _temp2317->tl= 0; _temp2317;}), 0), 0);( void*)( s->r=( void*)(( void*)(
Cyc_Absyn_declare_stmt( rh_var, rh_struct_typ,( struct Cyc_Absyn_Exp*)
newregion_call, Cyc_Absyn_declare_stmt( x_var, rh_struct_ptr_typ,( struct Cyc_Absyn_Exp*)
addr_rh_exp, Cyc_Absyn_declare_stmt( h_var, h_typ, 0, Cyc_Absyn_seq_stmt(
push_stmt, Cyc_Absyn_declare_stmt( e_var, e_typ,( struct Cyc_Absyn_Exp*)
setjmp_call, if_stmt, 0), 0), 0), 0), 0))->r)); return;}} _LL2102:;}} static
void Cyc_Toc_fndecl_to_c( struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Fndecl* f){ f->tvs=
0; f->effect= 0;( void*)( f->ret_type=( void*) Cyc_Toc_typ_to_c(( void*) f->ret_type));{
struct Cyc_List_List* args= f->args; for( 0; args != 0; args=({ struct Cyc_List_List*
_temp2318= args; if( _temp2318 == 0){ _throw( Null_Exception);} _temp2318->tl;})){(*((
struct _tuple18*)({ struct Cyc_List_List* _temp2319= args; if( _temp2319 == 0){
_throw( Null_Exception);} _temp2319->hd;}))).f3= Cyc_Toc_typ_to_c((*(( struct
_tuple18*)({ struct Cyc_List_List* _temp2320= args; if( _temp2320 == 0){ _throw(
Null_Exception);} _temp2320->hd;}))).f3);{ struct _tuple1* x=({ struct _tuple1*
_temp2321=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2321->f1=(
void*) Cyc_Absyn_Loc_n; _temp2321->f2=(*(( struct _tuple18*)({ struct Cyc_List_List*
_temp2322= args; if( _temp2322 == 0){ _throw( Null_Exception);} _temp2322->hd;}))).f1;
_temp2321;}); nv= Cyc_Toc_add_varmap( nv, x, Cyc_Absyn_var_exp( x, 0));}}} Cyc_Toc_stmt_to_c(
Cyc_Toc_clear_toplevel( nv), f->body);} static void* Cyc_Toc_scope_to_c( void* s){
void* _temp2323= s; _LL2325: if(( int) _temp2323 == Cyc_Absyn_Abstract){ goto
_LL2326;} else{ goto _LL2327;} _LL2327: if(( int) _temp2323 == Cyc_Absyn_ExternC){
goto _LL2328;} else{ goto _LL2329;} _LL2329: goto _LL2330; _LL2326: return( void*)
Cyc_Absyn_Public; _LL2328: return( void*) Cyc_Absyn_Extern; _LL2330: return s;
_LL2324:;} struct Cyc_Core_Opt* Cyc_Toc_structs_so_far= 0; static void Cyc_Toc_structdecl_to_c(
struct Cyc_Absyn_Structdecl* s){ struct _tuple1* n=( struct _tuple1*)({ struct
Cyc_Core_Opt* _temp2331= s->name; if( _temp2331 == 0){ _throw( Null_Exception);}
_temp2331->v;}); if( Cyc_Toc_structs_so_far == 0){ Cyc_Toc_structs_so_far=({
struct Cyc_Core_Opt* _temp2332=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2332->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp2332;});}{ int seen_defn_before; struct Cyc_Core_Opt* dopt=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp2333= Cyc_Toc_structs_so_far;
if( _temp2333 == 0){ _throw( Null_Exception);} _temp2333->v;}),(* n).f2); if(
dopt == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp2334= Cyc_Toc_structs_so_far; if( _temp2334 == 0){ _throw( Null_Exception);}
_temp2334->v=( void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key, struct Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp2335= Cyc_Toc_structs_so_far;
if( _temp2335 == 0){ _throw( Null_Exception);} _temp2335->v;}),(* n).f2, s);});}
else{ if((( struct Cyc_Absyn_Structdecl*)({ struct Cyc_Core_Opt* _temp2336= dopt;
if( _temp2336 == 0){ _throw( Null_Exception);} _temp2336->v;}))->fields == 0){(
struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp2337= Cyc_Toc_structs_so_far;
if( _temp2337 == 0){ _throw( Null_Exception);} _temp2337->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct
Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)({ struct
Cyc_Core_Opt* _temp2338= Cyc_Toc_structs_so_far; if( _temp2338 == 0){ _throw(
Null_Exception);} _temp2338->v;}),(* n).f2, s);}); seen_defn_before= 0;} else{
seen_defn_before= 1;}}( void*)( s->sc=( void*)(((( void*) s->sc ==( void*) Cyc_Absyn_Extern?
1:( void*) s->sc ==( void*) Cyc_Absyn_Static)? 1:( void*) s->sc ==( void*) Cyc_Absyn_ExternC)?(
void*) Cyc_Absyn_Public: Cyc_Toc_scope_to_c(( void*) s->sc))); s->tvs= 0; s->name=({
struct Cyc_Core_Opt* _temp2339=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2339->v=( void*) n; _temp2339;}); if( s->fields != 0){ if(
seen_defn_before){ s->fields= 0;} else{ struct Cyc_List_List* fields=( struct
Cyc_List_List*)({ struct Cyc_Core_Opt* _temp2340= s->fields; if( _temp2340 == 0){
_throw( Null_Exception);} _temp2340->v;}); for( 0; fields != 0; fields=({ struct
Cyc_List_List* _temp2341= fields; if( _temp2341 == 0){ _throw( Null_Exception);}
_temp2341->tl;})){( void*)((( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp2342= fields; if( _temp2342 == 0){ _throw( Null_Exception);} _temp2342->hd;}))->type=(
void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp2343= fields; if( _temp2343 == 0){ _throw( Null_Exception);}
_temp2343->hd;}))->type));}}}}} struct Cyc_Core_Opt* Cyc_Toc_unions_so_far= 0;
static void Cyc_Toc_uniondecl_to_c( struct Cyc_Absyn_Uniondecl* u){ struct
_tuple1* n=( struct _tuple1*)({ struct Cyc_Core_Opt* _temp2344= u->name; if(
_temp2344 == 0){ _throw( Null_Exception);} _temp2344->v;}); if( Cyc_Toc_unions_so_far
== 0){ Cyc_Toc_unions_so_far=({ struct Cyc_Core_Opt* _temp2345=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2345->v=( void*)(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); _temp2345;});}{ int seen_defn_before; struct Cyc_Core_Opt*
dopt=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string*
key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp2346= Cyc_Toc_unions_so_far; if( _temp2346 == 0){ _throw( Null_Exception);}
_temp2346->v;}),(* n).f2); if( dopt == 0){ seen_defn_before= 0;( struct Cyc_Dict_Dict*)({
struct Cyc_Core_Opt* _temp2347= Cyc_Toc_unions_so_far; if( _temp2347 == 0){
_throw( Null_Exception);} _temp2347->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp2348= Cyc_Toc_unions_so_far; if( _temp2348 == 0){ _throw( Null_Exception);}
_temp2348->v;}),(* n).f2, u);});} else{ if((( struct Cyc_Absyn_Uniondecl*)({
struct Cyc_Core_Opt* _temp2349= dopt; if( _temp2349 == 0){ _throw(
Null_Exception);} _temp2349->v;}))->fields == 0){( struct Cyc_Dict_Dict*)({
struct Cyc_Core_Opt* _temp2350= Cyc_Toc_unions_so_far; if( _temp2350 == 0){
_throw( Null_Exception);} _temp2350->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp2351= Cyc_Toc_unions_so_far; if( _temp2351 == 0){ _throw( Null_Exception);}
_temp2351->v;}),(* n).f2, u);}); seen_defn_before= 0;} else{ seen_defn_before= 1;}}(
void*)( u->sc=( void*)(((( void*) u->sc ==( void*) Cyc_Absyn_Extern? 1:( void*)
u->sc ==( void*) Cyc_Absyn_Static)? 1:( void*) u->sc ==( void*) Cyc_Absyn_ExternC)?(
void*) Cyc_Absyn_Public: Cyc_Toc_scope_to_c(( void*) u->sc))); u->tvs= 0; u->name=({
struct Cyc_Core_Opt* _temp2352=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2352->v=( void*) n; _temp2352;}); if( u->fields != 0){ if(
seen_defn_before){ u->fields= 0;} else{ struct Cyc_List_List* fields=( struct
Cyc_List_List*)({ struct Cyc_Core_Opt* _temp2353= u->fields; if( _temp2353 == 0){
_throw( Null_Exception);} _temp2353->v;}); for( 0; fields != 0; fields=({ struct
Cyc_List_List* _temp2354= fields; if( _temp2354 == 0){ _throw( Null_Exception);}
_temp2354->tl;})){( void*)((( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp2355= fields; if( _temp2355 == 0){ _throw( Null_Exception);} _temp2355->hd;}))->type=(
void*) Cyc_Toc_typ_to_c_array(( void*)(( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp2356= fields; if( _temp2356 == 0){ _throw( Null_Exception);}
_temp2356->hd;}))->type));}}}}} static struct Cyc_Core_Opt* Cyc_Toc_tunions_so_far=
0; static struct Cyc_List_List* Cyc_Toc_tuniondecl_to_c( struct Cyc_Absyn_Tuniondecl*
ed, struct Cyc_List_List* res){ struct _tuple1* n=( struct _tuple1*)({ struct
Cyc_Core_Opt* _temp2357= ed->name; if( _temp2357 == 0){ _throw( Null_Exception);}
_temp2357->v;}); if( Cyc_Toc_tunions_so_far == 0){ Cyc_Toc_tunions_so_far=({
struct Cyc_Core_Opt* _temp2358=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2358->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp2358;});}{ int seen_before; int seen_defn_before; struct Cyc_Core_Opt* dopt=((
struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key))
Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp2359=
Cyc_Toc_tunions_so_far; if( _temp2359 == 0){ _throw( Null_Exception);} _temp2359->v;}),(*
n).f2); if( dopt == 0){ seen_before= 0; seen_defn_before= 0;( struct Cyc_Dict_Dict*)({
struct Cyc_Core_Opt* _temp2360= Cyc_Toc_tunions_so_far; if( _temp2360 == 0){
_throw( Null_Exception);} _temp2360->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Tuniondecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp2361= Cyc_Toc_tunions_so_far; if( _temp2361 == 0){ _throw( Null_Exception);}
_temp2361->v;}),(* n).f2, ed);});} else{ seen_before= 1; if((( struct Cyc_Absyn_Tuniondecl*)({
struct Cyc_Core_Opt* _temp2362= dopt; if( _temp2362 == 0){ _throw(
Null_Exception);} _temp2362->v;}))->fields == 0){ seen_defn_before= 0;( struct
Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp2363= Cyc_Toc_tunions_so_far; if(
_temp2363 == 0){ _throw( Null_Exception);} _temp2363->v=( void*)(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Tuniondecl*
data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp2364= Cyc_Toc_tunions_so_far; if( _temp2364 == 0){ _throw( Null_Exception);}
_temp2364->v;}),(* n).f2, ed);});} else{ seen_defn_before= 1;}} if( !
seen_before){ struct Cyc_Absyn_Typedefdecl* tdef=({ struct Cyc_Absyn_Typedefdecl*
_temp2365=( struct Cyc_Absyn_Typedefdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Typedefdecl));
_temp2365->name= n; _temp2365->tvs= 0; _temp2365->defn=( void*) Cyc_Absyn_void_star_typ();
_temp2365;}); res=({ struct Cyc_List_List* _temp2366=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2366->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Typedef_d_struct* _temp2367=( struct Cyc_Absyn_Typedef_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct)); _temp2367[ 0]=({ struct
Cyc_Absyn_Typedef_d_struct _temp2368; _temp2368.tag= Cyc_Absyn_Typedef_d_tag;
_temp2368.f1= tdef; _temp2368;}); _temp2367;}), 0); _temp2366->tl= res;
_temp2366;});} if( ! seen_defn_before){ int tag_count= 0; int box_count= 0;
struct Cyc_List_List* fields= ed->fields == 0? 0:( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp2369= ed->fields; if( _temp2369 == 0){ _throw(
Null_Exception);} _temp2369->v;}); for( 0; fields != 0; fields=({ struct Cyc_List_List*
_temp2370= fields; if( _temp2370 == 0){ _throw( Null_Exception);} _temp2370->tl;})){
struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List*
_temp2371= fields; if( _temp2371 == 0){ _throw( Null_Exception);} _temp2371->hd;});
if( f->typs == 0){ struct Cyc_Absyn_Exp* eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp((
tag_count ++), 0); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( f->name,
Cyc_Absyn_uint_t, eopt); vd->tq=({ struct Cyc_Absyn_Tqual* _temp2372=( struct
Cyc_Absyn_Tqual*) GC_malloc_atomic( sizeof( struct Cyc_Absyn_Tqual)); _temp2372->q_const=
1; _temp2372->q_volatile= 0; _temp2372->q_restrict= 0; _temp2372;}); res=({
struct Cyc_List_List* _temp2373=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2373->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Var_d_struct* _temp2374=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2374[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2375; _temp2375.tag= Cyc_Absyn_Var_d_tag; _temp2375.f1= vd; _temp2375;});
_temp2374;}), 0); _temp2373->tl= res; _temp2373;});} else{ struct Cyc_Absyn_Exp*
eopt=( struct Cyc_Absyn_Exp*) Cyc_Absyn_signed_int_exp(( box_count ++), 0);
struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_static_vardecl( Cyc_Toc_collapse_qvar_tag(
f->name,( struct _tagged_string)({ char* _temp2376=( char*)"_tag"; struct
_tagged_string _temp2377; _temp2377.curr= _temp2376; _temp2377.base= _temp2376;
_temp2377.last_plus_one= _temp2376 + 5; _temp2377;})), Cyc_Absyn_sint_t, eopt);
vd->tq=({ struct Cyc_Absyn_Tqual* _temp2378=( struct Cyc_Absyn_Tqual*)
GC_malloc_atomic( sizeof( struct Cyc_Absyn_Tqual)); _temp2378->q_const= 1;
_temp2378->q_volatile= 0; _temp2378->q_restrict= 0; _temp2378;}); res=({ struct
Cyc_List_List* _temp2379=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2379->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp2380=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp2380[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp2381; _temp2381.tag= Cyc_Absyn_Var_d_tag;
_temp2381.f1= vd; _temp2381;}); _temp2380;}), 0); _temp2379->tl= res; _temp2379;});{
struct Cyc_List_List* fs= 0; int i= 1;{ struct Cyc_List_List* ts= f->typs; for(
0; ts != 0; ts=({ struct Cyc_List_List* _temp2382= ts; if( _temp2382 == 0){
_throw( Null_Exception);} _temp2382->tl;}), i ++){ struct _tagged_string* fname=(
struct _tagged_string*)({ struct _tagged_string* _temp2383=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp2383[ 0]=
xprintf("f%d", i); _temp2383;}); struct Cyc_Absyn_Structfield* f=({ struct Cyc_Absyn_Structfield*
_temp2384=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2384->name= fname; _temp2384->tq=(*(( struct _tuple5*)({ struct Cyc_List_List*
_temp2386= ts; if( _temp2386 == 0){ _throw( Null_Exception);} _temp2386->hd;}))).f1;
_temp2384->type=( void*) Cyc_Toc_typ_to_c_array((*(( struct _tuple5*)({ struct
Cyc_List_List* _temp2385= ts; if( _temp2385 == 0){ _throw( Null_Exception);}
_temp2385->hd;}))).f2); _temp2384->width= 0; _temp2384->attributes= 0; _temp2384;});
fs=({ struct Cyc_List_List* _temp2387=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2387->hd=( void*) f; _temp2387->tl= fs;
_temp2387;});}} fs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fs); fs=({ struct Cyc_List_List* _temp2388=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2388->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp2389=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2389->name= Cyc_Toc_tag_sp; _temp2389->tq= Cyc_Toc_mt_tq; _temp2389->type=(
void*) Cyc_Absyn_sint_t; _temp2389->width= 0; _temp2389->attributes= 0;
_temp2389;}); _temp2388->tl= fs; _temp2388;});{ struct Cyc_Absyn_Structdecl* sd=({
struct Cyc_Absyn_Structdecl* _temp2390=( struct Cyc_Absyn_Structdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp2390->sc=( void*)(( void*) Cyc_Absyn_Public);
_temp2390->name=({ struct Cyc_Core_Opt* _temp2392=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2392->v=( void*) Cyc_Toc_collapse_qvar_tag(
f->name,( struct _tagged_string)({ char* _temp2393=( char*)"_struct"; struct
_tagged_string _temp2394; _temp2394.curr= _temp2393; _temp2394.base= _temp2393;
_temp2394.last_plus_one= _temp2393 + 8; _temp2394;})); _temp2392;}); _temp2390->tvs=
0; _temp2390->fields=({ struct Cyc_Core_Opt* _temp2391=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2391->v=( void*) fs; _temp2391;});
_temp2390->attributes= 0; _temp2390;}); res=({ struct Cyc_List_List* _temp2395=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2395->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct* _temp2396=(
struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp2396[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp2397; _temp2397.tag= Cyc_Absyn_Struct_d_tag;
_temp2397.f1= sd; _temp2397;}); _temp2396;}), 0); _temp2395->tl= res; _temp2395;});}}}}}
return res;}} static struct Cyc_Core_Opt* Cyc_Toc_xtunions_so_far= 0; static
struct Cyc_List_List* Cyc_Toc_xtuniondecl_to_c( struct Cyc_Absyn_XTuniondecl* xd,
struct Cyc_List_List* res){ struct _tuple1* n= xd->name; if( xd->fields == 0){
void* t= Cyc_Absyn_cstar_typ( Cyc_Absyn_strct( Cyc_Toc__xtunion_struct_sp), Cyc_Toc_mt_tq);
struct Cyc_Absyn_Typedefdecl* td=({ struct Cyc_Absyn_Typedefdecl* _temp2398=(
struct Cyc_Absyn_Typedefdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Typedefdecl));
_temp2398->name= n; _temp2398->tvs= 0; _temp2398->defn=( void*) t; _temp2398;});
res=({ struct Cyc_List_List* _temp2399=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2399->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Typedef_d_struct* _temp2400=( struct Cyc_Absyn_Typedef_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct)); _temp2400[ 0]=({ struct
Cyc_Absyn_Typedef_d_struct _temp2401; _temp2401.tag= Cyc_Absyn_Typedef_d_tag;
_temp2401.f1= td; _temp2401;}); _temp2400;}), 0); _temp2399->tl= res; _temp2399;});}
else{ if( Cyc_Toc_xtunions_so_far == 0){ Cyc_Toc_xtunions_so_far=({ struct Cyc_Core_Opt*
_temp2402=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2402->v=( void*)(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp2402;});}{ struct Cyc_List_List* fs= xd->fields; for( 0; fs != 0; fs=({
struct Cyc_List_List* _temp2403= fs; if( _temp2403 == 0){ _throw( Null_Exception);}
_temp2403->tl;})){ struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp2404= fs; if( _temp2404 == 0){ _throw( Null_Exception);}
_temp2404->hd;}); struct _tagged_string* fn=(* f->name).f2; struct Cyc_Absyn_Exp*
sz_exp= Cyc_Absyn_uint_exp(({ struct _tagged_string _temp2405=* fn;(
unsigned int)( _temp2405.last_plus_one - _temp2405.curr);}), 0); struct Cyc_Absyn_ArrayType_struct*
tag_typ=({ struct Cyc_Absyn_ArrayType_struct* _temp2406=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp2406[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp2407; _temp2407.tag= Cyc_Absyn_ArrayType_tag;
_temp2407.f1=( void*) Cyc_Absyn_uchar_t; _temp2407.f2= Cyc_Toc_mt_tq; _temp2407.f3=(
struct Cyc_Absyn_Exp*) sz_exp; _temp2407;}); _temp2406;}); struct Cyc_Absyn_Exp*
initopt= 0; if(( void*) xd->sc !=( void*) Cyc_Absyn_Extern){ initopt=( struct
Cyc_Absyn_Exp*) Cyc_Absyn_string_exp(* fn, 0);}{ struct Cyc_Absyn_Vardecl*
tag_decl= Cyc_Absyn_new_vardecl( Cyc_Toc_collapse_qvar_tag( f->name,( struct
_tagged_string)({ char* _temp2408=( char*)"_tag"; struct _tagged_string
_temp2409; _temp2409.curr= _temp2408; _temp2409.base= _temp2408; _temp2409.last_plus_one=
_temp2408 + 5; _temp2409;})),( void*) tag_typ, initopt);( void*)( tag_decl->sc=(
void*)(( void*) xd->sc)); res=({ struct Cyc_List_List* _temp2410=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2410->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Var_d_struct* _temp2411=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp2411[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp2412; _temp2412.tag= Cyc_Absyn_Var_d_tag; _temp2412.f1= tag_decl; _temp2412;});
_temp2411;}), 0); _temp2410->tl= res; _temp2410;});{ int seen_before; int
seen_defn_before; struct Cyc_Core_Opt* dopt=(( struct Cyc_Core_Opt*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup_opt)(( struct Cyc_Dict_Dict*)({
struct Cyc_Core_Opt* _temp2413= Cyc_Toc_xtunions_so_far; if( _temp2413 == 0){
_throw( Null_Exception);} _temp2413->v;}), fn); if( dopt == 0){ seen_before= 0;
seen_defn_before= 0;( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp2414=
Cyc_Toc_xtunions_so_far; if( _temp2414 == 0){ _throw( Null_Exception);}
_temp2414->v=( void*)(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key, struct Cyc_Absyn_XTuniondecl* data)) Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp2415= Cyc_Toc_xtunions_so_far;
if( _temp2415 == 0){ _throw( Null_Exception);} _temp2415->v;}), fn, xd);});}
else{ seen_before= 1; if((( struct Cyc_Absyn_XTuniondecl*)({ struct Cyc_Core_Opt*
_temp2416= dopt; if( _temp2416 == 0){ _throw( Null_Exception);} _temp2416->v;}))->fields
== 0){( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp2417= Cyc_Toc_xtunions_so_far;
if( _temp2417 == 0){ _throw( Null_Exception);} _temp2417->v=( void*)(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct
Cyc_Absyn_XTuniondecl* data)) Cyc_Dict_insert)(( struct Cyc_Dict_Dict*)({ struct
Cyc_Core_Opt* _temp2418= Cyc_Toc_xtunions_so_far; if( _temp2418 == 0){ _throw(
Null_Exception);} _temp2418->v;}), fn, xd);}); seen_defn_before= 0;} else{
seen_defn_before= 1;}} if( ! seen_before){ struct Cyc_List_List* fields= 0; int
i= 1;{ struct Cyc_List_List* ts= f->typs; for( 0; ts != 0; ts=({ struct Cyc_List_List*
_temp2419= ts; if( _temp2419 == 0){ _throw( Null_Exception);} _temp2419->tl;}),
i ++){ struct _tagged_string* field_n=( struct _tagged_string*)({ struct
_tagged_string* _temp2420=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp2420[ 0]= xprintf("f%d", i); _temp2420;}); struct Cyc_Absyn_Structfield*
newf=({ struct Cyc_Absyn_Structfield* _temp2421=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2421->name= field_n;
_temp2421->tq=(*(( struct _tuple5*)({ struct Cyc_List_List* _temp2423= ts; if(
_temp2423 == 0){ _throw( Null_Exception);} _temp2423->hd;}))).f1; _temp2421->type=(
void*) Cyc_Toc_typ_to_c_array((*(( struct _tuple5*)({ struct Cyc_List_List*
_temp2422= ts; if( _temp2422 == 0){ _throw( Null_Exception);} _temp2422->hd;}))).f2);
_temp2421->width= 0; _temp2421->attributes= 0; _temp2421;}); fields=({ struct
Cyc_List_List* _temp2424=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2424->hd=( void*) newf; _temp2424->tl= fields; _temp2424;});}} fields=({
struct Cyc_List_List* _temp2425=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2425->hd=( void*)({ struct Cyc_Absyn_Structfield*
_temp2426=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp2426->name= Cyc_Toc_tag_sp; _temp2426->tq= Cyc_Toc_mt_tq; _temp2426->type=(
void*) Cyc_Absyn_cstar_typ( Cyc_Absyn_uchar_t, Cyc_Toc_mt_tq); _temp2426->width=
0; _temp2426->attributes= 0; _temp2426;}); _temp2425->tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( fields); _temp2425;});{ struct Cyc_Absyn_Structdecl*
strct_decl=({ struct Cyc_Absyn_Structdecl* _temp2427=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp2427->sc=( void*)(( void*)
Cyc_Absyn_Public); _temp2427->name=({ struct Cyc_Core_Opt* _temp2429=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2429->v=( void*)
Cyc_Toc_collapse_qvar_tag( f->name,( struct _tagged_string)({ char* _temp2430=(
char*)"_struct"; struct _tagged_string _temp2431; _temp2431.curr= _temp2430;
_temp2431.base= _temp2430; _temp2431.last_plus_one= _temp2430 + 8; _temp2431;}));
_temp2429;}); _temp2427->tvs= 0; _temp2427->fields=({ struct Cyc_Core_Opt*
_temp2428=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2428->v=( void*) fields; _temp2428;}); _temp2427->attributes= 0; _temp2427;});
res=({ struct Cyc_List_List* _temp2432=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2432->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Struct_d_struct* _temp2433=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp2433[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp2434; _temp2434.tag= Cyc_Absyn_Struct_d_tag;
_temp2434.f1= strct_decl; _temp2434;}); _temp2433;}), 0); _temp2432->tl= res;
_temp2432;});}}}}}}} return res;} static void Cyc_Toc_enumdecl_to_c( struct Cyc_Toc_Env*
nv, struct Cyc_Absyn_Enumdecl* ed){ struct _tuple1* n= ed->name;( void*)( ed->sc=(
void*)(((( void*) ed->sc ==( void*) Cyc_Absyn_Extern? 1:( void*) ed->sc ==( void*)
Cyc_Absyn_Static)? 1:( void*) ed->sc ==( void*) Cyc_Absyn_ExternC)?( void*) Cyc_Absyn_Public:
Cyc_Toc_scope_to_c(( void*) ed->sc)));{ struct Cyc_List_List* fs= ed->fields;
for( 0; fs != 0; fs=({ struct Cyc_List_List* _temp2435= fs; if( _temp2435 == 0){
_throw( Null_Exception);} _temp2435->tl;})){ if((( struct Cyc_Absyn_Enumfield*)({
struct Cyc_List_List* _temp2436= fs; if( _temp2436 == 0){ _throw( Null_Exception);}
_temp2436->hd;}))->tag != 0){ unsigned int i= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2438=(( struct Cyc_Absyn_Enumfield*)({
struct Cyc_List_List* _temp2437= fs; if( _temp2437 == 0){ _throw( Null_Exception);}
_temp2437->hd;}))->tag; if( _temp2438 == 0){ _throw( Null_Exception);} _temp2438;}));((
struct Cyc_Absyn_Enumfield*)({ struct Cyc_List_List* _temp2439= fs; if(
_temp2439 == 0){ _throw( Null_Exception);} _temp2439->hd;}))->tag=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp( i, 0);}}}} static struct Cyc_Absyn_Stmt* Cyc_Toc_letdecl_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* topt,
struct Cyc_Absyn_Exp* e, int exhaust, struct Cyc_Absyn_Stmt* s){ void* t=( void*)({
struct Cyc_Core_Opt* _temp2440= topt; if( _temp2440 == 0){ _throw(
Null_Exception);} _temp2440->v;}); Cyc_Toc_exp_to_c( nv, e);{ struct _tuple1* x=
Cyc_Toc_temp_var(); struct _tagged_string* succ_lab= Cyc_Toc_fresh_label();
struct _tagged_string* fail_lab= exhaust? succ_lab: Cyc_Toc_fresh_label();
struct Cyc_Absyn_Stmt* _temp2443; struct Cyc_List_List* _temp2445; struct Cyc_Toc_Env*
_temp2447; struct _tuple13 _temp2441= Cyc_Toc_xlate_pat( nv, t, Cyc_Absyn_var_exp(
x, 0), Cyc_Absyn_var_exp( x, 0), p, succ_lab, fail_lab, 0); _LL2448: _temp2447=
_temp2441.f1; goto _LL2446; _LL2446: _temp2445= _temp2441.f2; goto _LL2444;
_LL2444: _temp2443= _temp2441.f3; goto _LL2442; _LL2442: Cyc_Toc_stmt_to_c(
_temp2447, s);{ struct Cyc_Absyn_Stmt* succ_stmt= Cyc_Absyn_label_stmt( succ_lab,
s, 0); if( exhaust){ s= Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c( t),( struct
Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp2443, succ_stmt, 0), 0);} else{
struct Cyc_Absyn_Exp* e3= Cyc_Toc_newthrow_exp( Cyc_Absyn_var_exp(({ struct
_tuple1* _temp2449=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp2449->f1= Cyc_Toc_abs_ns; _temp2449->f2= Cyc_Toc_Match_Exception_sp;
_temp2449;}), 0)); struct Cyc_Absyn_Stmt* fail_stmt= Cyc_Absyn_label_stmt(
fail_lab, Cyc_Absyn_exp_stmt( e3, 0), 0); s= Cyc_Absyn_declare_stmt( x, Cyc_Toc_typ_to_c(
t),( struct Cyc_Absyn_Exp*) e, Cyc_Absyn_seq_stmt( _temp2443, Cyc_Absyn_seq_stmt(
fail_stmt, succ_stmt, 0), 0), 0);} for( 0; _temp2445 != 0; _temp2445=({ struct
Cyc_List_List* _temp2450= _temp2445; if( _temp2450 == 0){ _throw( Null_Exception);}
_temp2450->tl;})){ struct _tuple14 _temp2454; void* _temp2455; struct _tuple1*
_temp2457; struct _tuple14* _temp2452=( struct _tuple14*)({ struct Cyc_List_List*
_temp2451= _temp2445; if( _temp2451 == 0){ _throw( Null_Exception);} _temp2451->hd;});
_temp2454=* _temp2452; _LL2458: _temp2457= _temp2454.f1; goto _LL2456; _LL2456:
_temp2455= _temp2454.f2; goto _LL2453; _LL2453: s= Cyc_Absyn_declare_stmt(
_temp2457, _temp2455, 0, s, 0);} return s;}}} static struct _tuple17 Cyc_Toc_decls_to_c(
struct Cyc_Toc_Env* nv, struct Cyc_List_List* ds, int top){ struct Cyc_List_List*
res= 0; for( 0; ds != 0; ds=({ struct Cyc_List_List* _temp2459= ds; if(
_temp2459 == 0){ _throw( Null_Exception);} _temp2459->tl;})){ struct Cyc_Absyn_Decl*
d=( struct Cyc_Absyn_Decl*)({ struct Cyc_List_List* _temp2460= ds; if( _temp2460
== 0){ _throw( Null_Exception);} _temp2460->hd;}); void* _temp2461=( void*) d->r;
struct Cyc_Absyn_Vardecl* _temp2487; struct Cyc_Absyn_Fndecl* _temp2489; int
_temp2491; struct Cyc_Absyn_Exp* _temp2493; struct Cyc_Core_Opt* _temp2495;
struct Cyc_Core_Opt* _temp2497; struct Cyc_Absyn_Pat* _temp2499; struct Cyc_Absyn_Structdecl*
_temp2501; struct Cyc_Absyn_Uniondecl* _temp2503; struct Cyc_Absyn_Tuniondecl*
_temp2505; struct Cyc_Absyn_XTuniondecl* _temp2507; struct Cyc_Absyn_Enumdecl*
_temp2509; struct Cyc_Absyn_Typedefdecl* _temp2511; struct Cyc_List_List*
_temp2513; struct _tagged_string* _temp2515; struct Cyc_List_List* _temp2517;
struct _tuple1* _temp2519; struct Cyc_List_List* _temp2521; _LL2463: if(((
struct _tunion_struct*) _temp2461)->tag == Cyc_Absyn_Var_d_tag){ _LL2488:
_temp2487=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp2461)->f1; goto _LL2464;} else{ goto _LL2465;} _LL2465: if((( struct
_tunion_struct*) _temp2461)->tag == Cyc_Absyn_Fn_d_tag){ _LL2490: _temp2489=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*) _temp2461)->f1; goto
_LL2466;} else{ goto _LL2467;} _LL2467: if((( struct _tunion_struct*) _temp2461)->tag
== Cyc_Absyn_Let_d_tag){ _LL2500: _temp2499=( struct Cyc_Absyn_Pat*)(( struct
Cyc_Absyn_Let_d_struct*) _temp2461)->f1; goto _LL2498; _LL2498: _temp2497=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp2461)->f2; goto
_LL2496; _LL2496: _temp2495=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp2461)->f3; goto _LL2494; _LL2494: _temp2493=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Let_d_struct*) _temp2461)->f4; goto _LL2492; _LL2492: _temp2491=(
int)(( struct Cyc_Absyn_Let_d_struct*) _temp2461)->f5; goto _LL2468;} else{ goto
_LL2469;} _LL2469: if((( struct _tunion_struct*) _temp2461)->tag == Cyc_Absyn_Struct_d_tag){
_LL2502: _temp2501=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*)
_temp2461)->f1; goto _LL2470;} else{ goto _LL2471;} _LL2471: if((( struct
_tunion_struct*) _temp2461)->tag == Cyc_Absyn_Union_d_tag){ _LL2504: _temp2503=(
struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*) _temp2461)->f1;
goto _LL2472;} else{ goto _LL2473;} _LL2473: if((( struct _tunion_struct*)
_temp2461)->tag == Cyc_Absyn_Tunion_d_tag){ _LL2506: _temp2505=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_d_struct*) _temp2461)->f1; goto _LL2474;} else{ goto
_LL2475;} _LL2475: if((( struct _tunion_struct*) _temp2461)->tag == Cyc_Absyn_XTunion_d_tag){
_LL2508: _temp2507=( struct Cyc_Absyn_XTuniondecl*)(( struct Cyc_Absyn_XTunion_d_struct*)
_temp2461)->f1; goto _LL2476;} else{ goto _LL2477;} _LL2477: if((( struct
_tunion_struct*) _temp2461)->tag == Cyc_Absyn_Enum_d_tag){ _LL2510: _temp2509=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*) _temp2461)->f1;
goto _LL2478;} else{ goto _LL2479;} _LL2479: if((( struct _tunion_struct*)
_temp2461)->tag == Cyc_Absyn_Typedef_d_tag){ _LL2512: _temp2511=( struct Cyc_Absyn_Typedefdecl*)((
struct Cyc_Absyn_Typedef_d_struct*) _temp2461)->f1; goto _LL2480;} else{ goto
_LL2481;} _LL2481: if((( struct _tunion_struct*) _temp2461)->tag == Cyc_Absyn_Namespace_d_tag){
_LL2516: _temp2515=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2461)->f1; goto _LL2514; _LL2514: _temp2513=( struct Cyc_List_List*)((
struct Cyc_Absyn_Namespace_d_struct*) _temp2461)->f2; goto _LL2482;} else{ goto
_LL2483;} _LL2483: if((( struct _tunion_struct*) _temp2461)->tag == Cyc_Absyn_Using_d_tag){
_LL2520: _temp2519=( struct _tuple1*)(( struct Cyc_Absyn_Using_d_struct*)
_temp2461)->f1; goto _LL2518; _LL2518: _temp2517=( struct Cyc_List_List*)((
struct Cyc_Absyn_Using_d_struct*) _temp2461)->f2; goto _LL2484;} else{ goto
_LL2485;} _LL2485: if((( struct _tunion_struct*) _temp2461)->tag == Cyc_Absyn_ExternC_d_tag){
_LL2522: _temp2521=( struct Cyc_List_List*)(( struct Cyc_Absyn_ExternC_d_struct*)
_temp2461)->f1; goto _LL2486;} else{ goto _LL2462;} _LL2464: { struct _tuple1*
c_name= _temp2487->name; if(( void*) _temp2487->sc ==( void*) Cyc_Absyn_ExternC){
c_name=({ struct _tuple1* _temp2523=( struct _tuple1*) GC_malloc( sizeof( struct
_tuple1)); _temp2523->f1= Cyc_Toc_abs_ns; _temp2523->f2=(* c_name).f2; _temp2523;});}
if( _temp2487->initializer != 0){ Cyc_Toc_exp_to_c( nv,( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp2524= _temp2487->initializer; if( _temp2524 == 0){
_throw( Null_Exception);} _temp2524;}));} if( nv->toplevel){ res=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( Cyc_Toc_temp_topdecls,
res); Cyc_Toc_temp_topdecls= 0;} if( top){ nv= Cyc_Toc_add_varmap( nv, _temp2487->name,
Cyc_Absyn_varb_exp( c_name,( void*)({ struct Cyc_Absyn_Global_b_struct*
_temp2525=( struct Cyc_Absyn_Global_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp2525[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp2526; _temp2526.tag= Cyc_Absyn_Global_b_tag;
_temp2526.f1= _temp2487; _temp2526;}); _temp2525;}), 0));} else{ nv= Cyc_Toc_add_varmap(
nv, _temp2487->name, Cyc_Absyn_varb_exp( c_name,( void*)({ struct Cyc_Absyn_Local_b_struct*
_temp2527=( struct Cyc_Absyn_Local_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct));
_temp2527[ 0]=({ struct Cyc_Absyn_Local_b_struct _temp2528; _temp2528.tag= Cyc_Absyn_Local_b_tag;
_temp2528.f1= _temp2487; _temp2528;}); _temp2527;}), 0));} _temp2487->name=
c_name;( void*)( _temp2487->sc=( void*) Cyc_Toc_scope_to_c(( void*) _temp2487->sc));(
void*)( _temp2487->type=( void*) Cyc_Toc_typ_to_c_array(( void*) _temp2487->type));
res=({ struct Cyc_List_List* _temp2529=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2529->hd=( void*) d; _temp2529->tl= res;
_temp2529;}); goto _LL2462;} _LL2466: { struct _tuple1* c_name= _temp2489->name;
if(( void*) _temp2489->sc ==( void*) Cyc_Absyn_ExternC){ c_name=({ struct
_tuple1* _temp2530=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp2530->f1= Cyc_Toc_abs_ns; _temp2530->f2=(* c_name).f2; _temp2530;});} nv=
Cyc_Toc_add_varmap( nv, _temp2489->name, Cyc_Absyn_var_exp( c_name, 0));
_temp2489->name= c_name; Cyc_Toc_fndecl_to_c( nv, _temp2489); res=({ struct Cyc_List_List*
_temp2531=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2531->hd=( void*) d; _temp2531->tl= res; _temp2531;}); goto _LL2462;}
_LL2468:(( void(*)( struct _tagged_string s)) Cyc_Toc_toc_impos)(( struct
_tagged_string)({ char* _temp2532=( char*)"letdecl"; struct _tagged_string
_temp2533; _temp2533.curr= _temp2532; _temp2533.base= _temp2532; _temp2533.last_plus_one=
_temp2532 + 8; _temp2533;})); goto _LL2462; _LL2470: Cyc_Toc_structdecl_to_c(
_temp2501); res=({ struct Cyc_List_List* _temp2534=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2534->hd=( void*) d; _temp2534->tl=
res; _temp2534;}); goto _LL2462; _LL2472: Cyc_Toc_uniondecl_to_c( _temp2503);
res=({ struct Cyc_List_List* _temp2535=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp2535->hd=( void*) d; _temp2535->tl= res;
_temp2535;}); goto _LL2462; _LL2474: res= Cyc_Toc_tuniondecl_to_c( _temp2505,
res); goto _LL2462; _LL2476: res= Cyc_Toc_xtuniondecl_to_c( _temp2507, res);
goto _LL2462; _LL2478: Cyc_Toc_enumdecl_to_c( nv, _temp2509); res=({ struct Cyc_List_List*
_temp2536=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2536->hd=( void*) d; _temp2536->tl= res; _temp2536;}); goto _LL2462;
_LL2480: _temp2511->name= _temp2511->name; _temp2511->tvs= 0;( void*)( _temp2511->defn=(
void*) Cyc_Toc_typ_to_c_array(( void*) _temp2511->defn)); res=({ struct Cyc_List_List*
_temp2537=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2537->hd=( void*) d; _temp2537->tl= res; _temp2537;}); goto _LL2462;
_LL2482: _temp2517= _temp2513; goto _LL2484; _LL2484: _temp2521= _temp2517; goto
_LL2486; _LL2486: { struct _tuple17 p= Cyc_Toc_decls_to_c( nv, _temp2521, top);
nv= p.f1; res=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)((( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( p.f2), res); goto _LL2462;} _LL2462:;} return({ struct
_tuple17 _temp2538; _temp2538.f1= nv; _temp2538.f2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( res); _temp2538;});} static void Cyc_Toc_init(){
Cyc_Toc_added_decls= 0; Cyc_Toc_tuple_types= 0; Cyc_Toc_tuple_type_counter= 0;
Cyc_Toc_tagged_ptr_types= 0; Cyc_Toc_tagged_ptr_counter= 0; Cyc_Toc_temp_var_counter=
0; Cyc_Toc_fresh_label_counter= 0; Cyc_Toc_temp_topdecls= 0; Cyc_Toc_structs_so_far=
0; Cyc_Toc_tunions_so_far= 0; Cyc_Toc_xtunions_so_far= 0;} struct Cyc_List_List*
Cyc_Toc_toc( struct Cyc_List_List* ds){ Cyc_Toc_init();{ struct Cyc_List_List*
_temp2541; struct Cyc_Toc_Env* _temp2543; struct _tuple17 _temp2539= Cyc_Toc_decls_to_c(
Cyc_Toc_empty_env(), ds, 1); _LL2544: _temp2543= _temp2539.f1; goto _LL2542;
_LL2542: _temp2541= _temp2539.f2; goto _LL2540; _LL2540: return(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_Toc_added_decls),
_temp2541);}}