#include "cyc_include.h"

 struct _tuple0{ void* f1; struct _tagged_string* f2; } ; struct _tuple1{ struct
Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual* f2; void* f3; } ; struct _tuple2{
struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; } ; struct _tuple3{ void*
f1; void* f2; } ; struct _tuple4{ struct Cyc_Absyn_Tvar* f1; void* f2; } ;
struct _tuple5{ struct Cyc_Absyn_Tqual* f1; void* f2; } ; struct _tuple6{ void*
f1; int f2; } ; struct _tuple7{ struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2;
} ; struct _tuple8{ struct Cyc_Absyn_XTuniondecl* f1; } ; struct _tuple9{ struct
Cyc_Tcenv_Genv* f1; struct Cyc_Set_Set* f2; } ; typedef int Cyc_ptrdiff_t;
typedef unsigned int Cyc_size_t; typedef int Cyc_wchar_t; typedef unsigned int
Cyc_wint_t; typedef char Cyc_u_char; typedef unsigned short Cyc_u_short; typedef
unsigned int Cyc_u_int; typedef unsigned int Cyc_u_long; typedef unsigned short
Cyc_ushort; typedef unsigned int Cyc_uint; typedef unsigned int Cyc_clock_t;
typedef int Cyc_time_t; struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct
Cyc_itimerspec{ struct Cyc_timespec it_interval; struct Cyc_timespec it_value; }
; typedef int Cyc_daddr_t; typedef char* Cyc_caddr_t; typedef unsigned short Cyc_ino_t;
typedef short Cyc_dev_t; typedef int Cyc_off_t; typedef unsigned short Cyc_uid_t;
typedef unsigned short Cyc_gid_t; typedef int Cyc_pid_t; typedef int Cyc_key_t;
typedef int Cyc_ssize_t; typedef unsigned int Cyc_mode_t; typedef unsigned short
Cyc_nlink_t; typedef int Cyc_fd_mask; struct Cyc__types_fd_set{ int fds_bits[ 8u];
} ; typedef struct Cyc__types_fd_set Cyc__types_fd_set; typedef char* Cyc_Cstring;
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
struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List*
Cyc_List_glist_t; typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct
Cyc_List_List* Cyc_List_List_t; extern int Cyc_List_length( struct Cyc_List_List*
x); extern char Cyc_List_List_empty_tag[ 11u]; struct Cyc_List_List_empty_struct{
char* tag; } ; extern char Cyc_List_List_mismatch_tag[ 14u]; struct Cyc_List_List_mismatch_struct{
char* tag; } ; extern struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List*
x, struct Cyc_List_List* y); extern char Cyc_List_Nth_tag[ 4u]; struct Cyc_List_Nth_struct{
char* tag; } ; extern int Cyc_List_mem( int(* compare)( void*, void*), struct
Cyc_List_List* l, void* x); extern struct Cyc_List_List* Cyc_List_filter_c( int(*
f)( void*, void*), void* env, struct Cyc_List_List* l); struct Cyc_Stdio___sFILE;
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; typedef int Cyc_Stdio_fpos_t;
extern char Cyc_Stdio_FileOpenError_tag[ 14u]; struct Cyc_Stdio_FileOpenError_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Stdio_FileCloseError_tag[
15u]; struct Cyc_Stdio_FileCloseError_struct{ char* tag; } ; extern int Cyc_String_strcmp(
struct _tagged_string s1, struct _tagged_string s2); extern int Cyc_String_strptrcmp(
struct _tagged_string* s1, struct _tagged_string* s2); extern int Cyc_String_zstrptrcmp(
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
char* tag; } ; extern int Cyc_Absyn_qvar_cmp( struct _tuple0*, struct _tuple0*);
extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref*
x); extern void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref* x); extern
struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp( unsigned int, struct Cyc_Position_Segment*);
extern struct _tagged_string Cyc_Absyn_attribute2string( void*); struct Cyc_PP_Ppstate;
typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t; struct Cyc_PP_Out; typedef
struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc; typedef struct Cyc_PP_Doc*
Cyc_PP_doc_t; extern struct _tagged_string Cyc_Absynpp_typ2string( void*);
extern struct _tagged_string Cyc_Absynpp_ckind2string( struct Cyc_Absyn_Conref*);
extern struct _tagged_string Cyc_Absynpp_qvar2string( struct _tuple0*); struct
Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct Cyc_Set_Set*
Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t; extern struct Cyc_Set_Set*
Cyc_Set_empty( int(* comp)( void*, void*)); extern struct Cyc_Set_Set* Cyc_Set_insert(
struct Cyc_Set_Set* s, void* elt); extern int Cyc_Set_member( struct Cyc_Set_Set*
s, void* elt); extern char Cyc_Set_Absent_tag[ 7u]; struct Cyc_Set_Absent_struct{
char* tag; } ; struct Cyc_Dict_Dict; typedef struct Cyc_Dict_Dict* Cyc_Dict_hdict_t;
typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t; extern char Cyc_Dict_Present_tag[
8u]; struct Cyc_Dict_Present_struct{ char* tag; } ; extern char Cyc_Dict_Absent_tag[
7u]; extern int Cyc_Dict_member( struct Cyc_Dict_Dict* d, void* key); extern
struct Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict* d, void* key, void*
data); extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d, void* key); extern
struct Cyc_Core_Opt* Cyc_Dict_lookup_opt( struct Cyc_Dict_Dict* d, void* key);
extern void Cyc_Dict_iter( void(* f)( void*, void*), struct Cyc_Dict_Dict* d);
extern struct Cyc_Dict_Dict* Cyc_Dict_map( void*(* f)( void*), struct Cyc_Dict_Dict*
d); extern struct Cyc_Dict_Dict* Cyc_Dict_filter_c( int(* f)( void*, void*, void*),
void* env, struct Cyc_Dict_Dict* d); typedef void* Cyc_Tcenv_Resolved; static
const int Cyc_Tcenv_VarRes_tag= 0; struct Cyc_Tcenv_VarRes_struct{ int tag; void*
f1; } ; static const int Cyc_Tcenv_StructRes_tag= 1; struct Cyc_Tcenv_StructRes_struct{
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
extern struct Cyc_Tcenv_Genv* Cyc_Tcenv_empty_genv(); extern struct Cyc_Tcenv_Fenv*
Cyc_Tcenv_new_fenv( struct Cyc_Position_Segment*, struct Cyc_Absyn_Fndecl*);
extern struct Cyc_List_List* Cyc_Tcenv_resolve_namespace( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tagged_string*, struct Cyc_List_List*);
extern struct Cyc_Core_Opt* Cyc_Tcenv_lookup_xtuniondecl( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tuple0*); extern int Cyc_Tcenv_all_labels_resolved(
struct Cyc_Tcenv_Tenv*); extern char Cyc_Tcutil_TypeErr_tag[ 8u]; struct Cyc_Tcutil_TypeErr_struct{
char* tag; } ; extern void* Cyc_Tcutil_impos( struct _tagged_string); extern
void Cyc_Tcutil_terr( struct Cyc_Position_Segment*, struct _tagged_string);
extern void Cyc_Tcutil_warn( struct Cyc_Position_Segment*, struct _tagged_string);
extern void* Cyc_Tcutil_compress( void* t); extern int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_is_function_type(
void* t); extern int Cyc_Tcutil_unify( void*, void*); extern void* Cyc_Tcutil_substitute(
struct Cyc_List_List*, void*); extern void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl*);
extern struct Cyc_Absyn_Exp* Cyc_Tcutil_default_initializer( struct Cyc_Tcenv_Tenv*,
void*, struct Cyc_Position_Segment*); extern void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, void*); extern void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Fndecl*);
extern void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List* bound_tvars, void* k, void*); extern void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*, struct Cyc_List_List*); extern int Cyc_Tcutil_equal_tqual(
struct Cyc_Absyn_Tqual* tq1, struct Cyc_Absyn_Tqual* tq2); extern int Cyc_Tcutil_same_atts(
struct Cyc_List_List*, struct Cyc_List_List*); extern int Cyc_Tcutil_bits_only(
void* t); extern int Cyc_Tcutil_is_const_exp( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Exp* e); extern void* Cyc_Tcexp_tcExpInitializer( struct Cyc_Tcenv_Tenv*,
void**, struct Cyc_Absyn_Exp*); extern void Cyc_Tcstmt_tcStmt( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Stmt* s, int new_block); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); extern void Cyc_Tc_tc( struct Cyc_Tcenv_Tenv* te, int
add_cyc_namespace, struct Cyc_List_List* ds); extern struct Cyc_List_List* Cyc_Tc_treeshake(
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*); static void Cyc_Tc_redecl_err(
struct Cyc_Position_Segment* loc, struct _tagged_string t, struct _tagged_string*
v, struct _tagged_string new_sc, struct _tagged_string old_sc){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp0= t; struct _tagged_string _temp1=* v; struct
_tagged_string _temp2= new_sc; struct _tagged_string _temp3= old_sc; xprintf("redeclaration of %.*s %.*s cannot be %.*s when earlier definition is %.*s",
_temp0.last_plus_one - _temp0.curr, _temp0.curr, _temp1.last_plus_one - _temp1.curr,
_temp1.curr, _temp2.last_plus_one - _temp2.curr, _temp2.curr, _temp3.last_plus_one
- _temp3.curr, _temp3.curr);}));} static void* Cyc_Tc_scope_redecl_okay( void*
s1, struct Cyc_Core_Opt* fields1, void* s2, struct Cyc_Core_Opt* fields2, struct
Cyc_Position_Segment* loc, struct _tagged_string t, struct _tagged_string* v){{
struct _tuple3 _temp5=({ struct _tuple3 _temp4; _temp4.f1= s1; _temp4.f2= s2;
_temp4;}); void* _temp35; void* _temp37; void* _temp39; void* _temp41; void*
_temp43; void* _temp45; void* _temp47; void* _temp49; void* _temp51; void*
_temp53; void* _temp55; void* _temp57; void* _temp59; void* _temp61; void*
_temp63; void* _temp65; void* _temp68; void* _temp70; void* _temp72; void*
_temp74; void* _temp76; void* _temp78; void* _temp80; void* _temp82; void*
_temp85; void* _temp87; void* _temp89; void* _temp91; _LL7: _LL38: _temp37=
_temp5.f1; if(( int) _temp37 == Cyc_Absyn_ExternC){ goto _LL36;} else{ goto _LL9;}
_LL36: _temp35= _temp5.f2; if(( int) _temp35 == Cyc_Absyn_ExternC){ goto _LL8;}
else{ goto _LL9;} _LL9: _LL42: _temp41= _temp5.f1; goto _LL40; _LL40: _temp39=
_temp5.f2; if(( int) _temp39 == Cyc_Absyn_ExternC){ goto _LL10;} else{ goto
_LL11;} _LL11: _LL46: _temp45= _temp5.f1; if(( int) _temp45 == Cyc_Absyn_Static){
goto _LL44;} else{ goto _LL13;} _LL44: _temp43= _temp5.f2; if(( int) _temp43 ==
Cyc_Absyn_Static){ goto _LL12;} else{ goto _LL13;} _LL13: _LL50: _temp49= _temp5.f1;
goto _LL48; _LL48: _temp47= _temp5.f2; if(( int) _temp47 == Cyc_Absyn_Static){
goto _LL14;} else{ goto _LL15;} _LL15: _LL54: _temp53= _temp5.f1; if(( int)
_temp53 == Cyc_Absyn_Static){ goto _LL52;} else{ goto _LL17;} _LL52: _temp51=
_temp5.f2; if(( int) _temp51 == Cyc_Absyn_Public){ goto _LL16;} else{ goto _LL17;}
_LL17: _LL58: _temp57= _temp5.f1; if(( int) _temp57 == Cyc_Absyn_Abstract){ goto
_LL56;} else{ goto _LL19;} _LL56: _temp55= _temp5.f2; if(( int) _temp55 == Cyc_Absyn_Public){
goto _LL18;} else{ goto _LL19;} _LL19: _LL62: _temp61= _temp5.f1; goto _LL60;
_LL60: _temp59= _temp5.f2; if(( int) _temp59 == Cyc_Absyn_Public){ goto _LL20;}
else{ goto _LL21;} _LL21: _LL66: _temp65= _temp5.f1; if(( int) _temp65 == Cyc_Absyn_Static){
goto _LL64;} else{ goto _LL23;} _LL64: _temp63= _temp5.f2; if(( int) _temp63 ==
Cyc_Absyn_Extern){ goto _LL22;} else{ goto _LL23;} _LL23: _LL71: _temp70= _temp5.f1;
if(( int) _temp70 == Cyc_Absyn_Abstract){ goto _LL69;} else{ goto _LL25;} _LL69:
_temp68= _temp5.f2; if(( int) _temp68 == Cyc_Absyn_Extern){ goto _LL67;} else{
goto _LL25;} _LL67: if( fields2 != 0){ goto _LL24;} else{ goto _LL25;} _LL25:
_LL75: _temp74= _temp5.f1; goto _LL73; _LL73: _temp72= _temp5.f2; if(( int)
_temp72 == Cyc_Absyn_Extern){ goto _LL26;} else{ goto _LL27;} _LL27: _LL79:
_temp78= _temp5.f1; if(( int) _temp78 == Cyc_Absyn_Static){ goto _LL77;} else{
goto _LL29;} _LL77: _temp76= _temp5.f2; if(( int) _temp76 == Cyc_Absyn_Abstract){
goto _LL28;} else{ goto _LL29;} _LL29: _LL83: _temp82= _temp5.f1; if(( int)
_temp82 == Cyc_Absyn_Public){ goto _LL81;} else{ goto _LL31;} _LL81: _temp80=
_temp5.f2; if(( int) _temp80 == Cyc_Absyn_Abstract){ goto _LL30;} else{ goto
_LL31;} _LL31: _LL88: _temp87= _temp5.f1; if(( int) _temp87 == Cyc_Absyn_Extern){
goto _LL86;} else{ goto _LL33;} _LL86: _temp85= _temp5.f2; if(( int) _temp85 ==
Cyc_Absyn_Abstract){ goto _LL84;} else{ goto _LL33;} _LL84: if( fields1 != 0){
goto _LL32;} else{ goto _LL33;} _LL33: _LL92: _temp91= _temp5.f1; goto _LL90;
_LL90: _temp89= _temp5.f2; if(( int) _temp89 == Cyc_Absyn_Abstract){ goto _LL34;}
else{ goto _LL6;} _LL8: goto _LL6; _LL10: Cyc_Tc_redecl_err( loc, t, v,( struct
_tagged_string)({ char* _temp93=( char*)"non-extern \"C\""; struct
_tagged_string _temp94; _temp94.curr= _temp93; _temp94.base= _temp93; _temp94.last_plus_one=
_temp93 + 15; _temp94;}),( struct _tagged_string)({ char* _temp95=( char*)"extern \"C\"";
struct _tagged_string _temp96; _temp96.curr= _temp95; _temp96.base= _temp95;
_temp96.last_plus_one= _temp95 + 11; _temp96;})); goto _LL6; _LL12: goto _LL6;
_LL14: Cyc_Tc_redecl_err( loc, t, v,( struct _tagged_string)({ char* _temp97=(
char*)"non-static"; struct _tagged_string _temp98; _temp98.curr= _temp97;
_temp98.base= _temp97; _temp98.last_plus_one= _temp97 + 11; _temp98;}),( struct
_tagged_string)({ char* _temp99=( char*)"static"; struct _tagged_string _temp100;
_temp100.curr= _temp99; _temp100.base= _temp99; _temp100.last_plus_one= _temp99
+ 7; _temp100;})); goto _LL6; _LL16: Cyc_Tc_redecl_err( loc, t, v,( struct
_tagged_string)({ char* _temp101=( char*)"static"; struct _tagged_string
_temp102; _temp102.curr= _temp101; _temp102.base= _temp101; _temp102.last_plus_one=
_temp101 + 7; _temp102;}),( struct _tagged_string)({ char* _temp103=( char*)"public";
struct _tagged_string _temp104; _temp104.curr= _temp103; _temp104.base= _temp103;
_temp104.last_plus_one= _temp103 + 7; _temp104;})); goto _LL6; _LL18: Cyc_Tc_redecl_err(
loc, t, v,( struct _tagged_string)({ char* _temp105=( char*)"abstract"; struct
_tagged_string _temp106; _temp106.curr= _temp105; _temp106.base= _temp105;
_temp106.last_plus_one= _temp105 + 9; _temp106;}),( struct _tagged_string)({
char* _temp107=( char*)"public"; struct _tagged_string _temp108; _temp108.curr=
_temp107; _temp108.base= _temp107; _temp108.last_plus_one= _temp107 + 7;
_temp108;})); goto _LL6; _LL20: s1=( void*) Cyc_Absyn_Public; goto _LL6; _LL22:
Cyc_Tc_redecl_err( loc, t, v,( struct _tagged_string)({ char* _temp109=( char*)"static";
struct _tagged_string _temp110; _temp110.curr= _temp109; _temp110.base= _temp109;
_temp110.last_plus_one= _temp109 + 7; _temp110;}),( struct _tagged_string)({
char* _temp111=( char*)"extern"; struct _tagged_string _temp112; _temp112.curr=
_temp111; _temp112.base= _temp111; _temp112.last_plus_one= _temp111 + 7;
_temp112;})); goto _LL6; _LL24: Cyc_Tc_redecl_err( loc, t, v,( struct
_tagged_string)({ char* _temp113=( char*)"abstract"; struct _tagged_string
_temp114; _temp114.curr= _temp113; _temp114.base= _temp113; _temp114.last_plus_one=
_temp113 + 9; _temp114;}),( struct _tagged_string)({ char* _temp115=( char*)"transparent";
struct _tagged_string _temp116; _temp116.curr= _temp115; _temp116.base= _temp115;
_temp116.last_plus_one= _temp115 + 12; _temp116;})); goto _LL6; _LL26: goto _LL6;
_LL28: Cyc_Tc_redecl_err( loc, t, v,( struct _tagged_string)({ char* _temp117=(
char*)"static"; struct _tagged_string _temp118; _temp118.curr= _temp117;
_temp118.base= _temp117; _temp118.last_plus_one= _temp117 + 7; _temp118;}),(
struct _tagged_string)({ char* _temp119=( char*)"abstract"; struct
_tagged_string _temp120; _temp120.curr= _temp119; _temp120.base= _temp119;
_temp120.last_plus_one= _temp119 + 9; _temp120;})); goto _LL6; _LL30: Cyc_Tc_redecl_err(
loc, t, v,( struct _tagged_string)({ char* _temp121=( char*)"public"; struct
_tagged_string _temp122; _temp122.curr= _temp121; _temp122.base= _temp121;
_temp122.last_plus_one= _temp121 + 7; _temp122;}),( struct _tagged_string)({
char* _temp123=( char*)"abstract"; struct _tagged_string _temp124; _temp124.curr=
_temp123; _temp124.base= _temp123; _temp124.last_plus_one= _temp123 + 9;
_temp124;})); goto _LL6; _LL32: Cyc_Tc_redecl_err( loc, t, v,( struct
_tagged_string)({ char* _temp125=( char*)"[extern] transparent"; struct
_tagged_string _temp126; _temp126.curr= _temp125; _temp126.base= _temp125;
_temp126.last_plus_one= _temp125 + 21; _temp126;}),( struct _tagged_string)({
char* _temp127=( char*)"abstract"; struct _tagged_string _temp128; _temp128.curr=
_temp127; _temp128.base= _temp127; _temp128.last_plus_one= _temp127 + 9;
_temp128;})); goto _LL6; _LL34: s1=( void*) Cyc_Absyn_Abstract; goto _LL6; _LL6:;}
return s1;} static void Cyc_Tc_field_redecl_okay( struct Cyc_Absyn_Tunionfield*
f1, struct Cyc_Absyn_Tunionfield* f2, struct Cyc_List_List* inst, struct Cyc_Tcenv_Tenv*
te, struct _tagged_string t, struct _tagged_string* v){ struct Cyc_Position_Segment*
loc= f1->loc; if( Cyc_String_zstrptrcmp((* f1->name).f2,(* f2->name).f2) != 0){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp129= t; struct _tagged_string
_temp130=* v; struct _tagged_string _temp131=*(* f1->name).f2; struct
_tagged_string _temp132=*(* f2->name).f2; xprintf("redeclaration of %.*s %.*s: field name mismatch %.*s != %.*s",
_temp129.last_plus_one - _temp129.curr, _temp129.curr, _temp130.last_plus_one -
_temp130.curr, _temp130.curr, _temp131.last_plus_one - _temp131.curr, _temp131.curr,
_temp132.last_plus_one - _temp132.curr, _temp132.curr);}));}{ struct Cyc_List_List*
tvs1= f1->tvs; struct Cyc_List_List* tvs2= f2->tvs; if((( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( tvs1) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
tvs2)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp133= t; struct
_tagged_string _temp134=* v; struct _tagged_string _temp135=*(* f1->name).f2;
xprintf("redeclaration of %.*s %.*s, field %.*s: type parameter number mismatch",
_temp133.last_plus_one - _temp133.curr, _temp133.curr, _temp134.last_plus_one -
_temp134.curr, _temp134.curr, _temp135.last_plus_one - _temp135.curr, _temp135.curr);}));}
for( 0; tvs1 != 0; tvs1=({ struct Cyc_List_List* _temp136= tvs1; if( _temp136 ==
0){ _throw( Null_Exception);} _temp136->tl;}), tvs2=({ struct Cyc_List_List*
_temp137= tvs2; if( _temp137 == 0){ _throw( Null_Exception);} _temp137->tl;})){
inst=({ struct Cyc_List_List* _temp138=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp138->hd=( void*)({ struct _tuple4* _temp139=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp139->f1=( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp143= tvs2; if( _temp143 == 0){ _throw( Null_Exception);}
_temp143->hd;}); _temp139->f2=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp140=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp140[ 0]=({ struct Cyc_Absyn_VarType_struct _temp141; _temp141.tag= Cyc_Absyn_VarType_tag;
_temp141.f1=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp142= tvs1;
if( _temp142 == 0){ _throw( Null_Exception);} _temp142->hd;}); _temp141;});
_temp140;}); _temp139;}); _temp138->tl= inst; _temp138;});}{ struct Cyc_List_List*
typs1= f1->typs; struct Cyc_List_List* typs2= f2->typs; if((( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( typs1) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
typs2)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp144= t; struct
_tagged_string _temp145=* v; struct _tagged_string _temp146=*(* f1->name).f2;
xprintf("redeclaration of %.*s %.*s, field %.*s: parameter number mismatch",
_temp144.last_plus_one - _temp144.curr, _temp144.curr, _temp145.last_plus_one -
_temp145.curr, _temp145.curr, _temp146.last_plus_one - _temp146.curr, _temp146.curr);}));}
for( 0; typs1 != 0; typs1=({ struct Cyc_List_List* _temp147= typs1; if( _temp147
== 0){ _throw( Null_Exception);} _temp147->tl;}), typs2=({ struct Cyc_List_List*
_temp148= typs2; if( _temp148 == 0){ _throw( Null_Exception);} _temp148->tl;})){
if( ! Cyc_Tcutil_equal_tqual((*(( struct _tuple5*)({ struct Cyc_List_List*
_temp149= typs1; if( _temp149 == 0){ _throw( Null_Exception);} _temp149->hd;}))).f1,(*((
struct _tuple5*)({ struct Cyc_List_List* _temp150= typs2; if( _temp150 == 0){
_throw( Null_Exception);} _temp150->hd;}))).f1)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp151= t; struct _tagged_string _temp152=* v; struct
_tagged_string _temp153=*(* f1->name).f2; xprintf("redeclaration of %.*s %.*s, field %.*s: parameter qualifier",
_temp151.last_plus_one - _temp151.curr, _temp151.curr, _temp152.last_plus_one -
_temp152.curr, _temp152.curr, _temp153.last_plus_one - _temp153.curr, _temp153.curr);}));}{
void* subst_t2= Cyc_Tcutil_substitute( inst,(*(( struct _tuple5*)({ struct Cyc_List_List*
_temp154= typs2; if( _temp154 == 0){ _throw( Null_Exception);} _temp154->hd;}))).f2);
if( ! Cyc_Tcutil_unify((*(( struct _tuple5*)({ struct Cyc_List_List* _temp155=
typs1; if( _temp155 == 0){ _throw( Null_Exception);} _temp155->hd;}))).f2,
subst_t2)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp157= t; struct
_tagged_string _temp158=* v; struct _tagged_string _temp159=*(* f1->name).f2;
struct _tagged_string _temp160= Cyc_Absynpp_typ2string((*(( struct _tuple5*)({
struct Cyc_List_List* _temp156= typs1; if( _temp156 == 0){ _throw(
Null_Exception);} _temp156->hd;}))).f2); struct _tagged_string _temp161= Cyc_Absynpp_typ2string(
subst_t2); xprintf("redeclaration of %.*s %.*s, field %.*s: parameter type mismatch %.*s != %.*s",
_temp157.last_plus_one - _temp157.curr, _temp157.curr, _temp158.last_plus_one -
_temp158.curr, _temp158.curr, _temp159.last_plus_one - _temp159.curr, _temp159.curr,
_temp160.last_plus_one - _temp160.curr, _temp160.curr, _temp161.last_plus_one -
_temp161.curr, _temp161.curr);}));}}}}}} static int Cyc_Tc_var_redecl_okay(
struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct
_tagged_string* name, void* t, void* sc, struct Cyc_List_List* atts, int
check_no_fun){ struct Cyc_Core_Opt* ans=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup_opt)( ge->ordinaries, name); if(
ans == 0){ return 0;} else{ void* sc2; void* t2; struct Cyc_List_List* atts2;{
void* _temp163=(*(( struct _tuple6*)({ struct Cyc_Core_Opt* _temp162= ans; if(
_temp162 == 0){ _throw( Null_Exception);} _temp162->v;}))).f1; void* _temp171;
struct Cyc_Absyn_Fndecl* _temp173; void* _temp175; struct Cyc_Absyn_Vardecl*
_temp177; _LL165: if((( struct _tunion_struct*) _temp163)->tag == Cyc_Tcenv_VarRes_tag){
_LL172: _temp171=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp163)->f1; if((
unsigned int) _temp171 > 1u?(( struct _tunion_struct*) _temp171)->tag == Cyc_Absyn_Funname_b_tag:
0){ _LL174: _temp173=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Funname_b_struct*)
_temp171)->f1; goto _LL166;} else{ goto _LL167;}} else{ goto _LL167;} _LL167:
if((( struct _tunion_struct*) _temp163)->tag == Cyc_Tcenv_VarRes_tag){ _LL176:
_temp175=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp163)->f1; if((
unsigned int) _temp175 > 1u?(( struct _tunion_struct*) _temp175)->tag == Cyc_Absyn_Global_b_tag:
0){ _LL178: _temp177=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Global_b_struct*)
_temp175)->f1; goto _LL168;} else{ goto _LL169;}} else{ goto _LL169;} _LL169:
goto _LL170; _LL166: if( check_no_fun){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp179=* name; xprintf("redefinition of function %.*s",
_temp179.last_plus_one - _temp179.curr, _temp179.curr);}));} sc2=( void*)
_temp173->sc; t2=( void*)({ struct Cyc_Core_Opt* _temp180= _temp173->cached_typ;
if( _temp180 == 0){ _throw( Null_Exception);} _temp180->v;}); atts2= _temp173->attributes;
goto _LL164; _LL168: sc2=( void*) _temp177->sc; t2=( void*) _temp177->type;
atts2= _temp177->attributes; goto _LL164; _LL170: return 1; _LL164:;} if( sc ==(
void*) Cyc_Absyn_Static? sc2 !=( void*) Cyc_Absyn_Static: 0){ Cyc_Tcutil_warn(
loc,({ struct _tagged_string _temp181=* name; xprintf("static declaration of %.*s follows non-static declaration",
_temp181.last_plus_one - _temp181.curr, _temp181.curr);}));} if( sc2 ==( void*)
Cyc_Absyn_Static? sc !=( void*) Cyc_Absyn_Static: 0){ Cyc_Tcutil_warn( loc,({
struct _tagged_string _temp182=* name; xprintf("non-static declaration of %.*s follows static declaration",
_temp182.last_plus_one - _temp182.curr, _temp182.curr);}));} if(( sc2 ==( void*)
Cyc_Absyn_ExternC? sc !=( void*) Cyc_Absyn_ExternC: 0)? 1:( sc2 !=( void*) Cyc_Absyn_ExternC?
sc ==( void*) Cyc_Absyn_ExternC: 0)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp183=* name; xprintf("incompatible redeclaration of %.*s",
_temp183.last_plus_one - _temp183.curr, _temp183.curr);}));} else{ if((( sc !=(
void*) Cyc_Absyn_Extern? sc2 !=( void*) Cyc_Absyn_Extern: 0)? !( sc ==( void*)
Cyc_Absyn_Static? sc2 ==( void*) Cyc_Absyn_Static: 0): 0)? !( sc ==( void*) Cyc_Absyn_Public?
sc2 ==( void*) Cyc_Absyn_Public: 0): 0){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp184=* name; xprintf("incompatible redeclaration of %.*s",
_temp184.last_plus_one - _temp184.curr, _temp184.curr);}));}} if( ! Cyc_Tcutil_unify(
t, t2)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp185=* name; struct
_tagged_string _temp186= Cyc_Absynpp_typ2string( t); struct _tagged_string
_temp187= Cyc_Absynpp_typ2string( t2); xprintf("redeclaration of %.*s at a different type: %.*s != %.*s",
_temp185.last_plus_one - _temp185.curr, _temp185.curr, _temp186.last_plus_one -
_temp186.curr, _temp186.curr, _temp187.last_plus_one - _temp187.curr, _temp187.curr);}));}
if( ! Cyc_Tcutil_same_atts( atts, atts2)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp188=* name; xprintf("redeclaration of %.*s with different attributes",
_temp188.last_plus_one - _temp188.curr, _temp188.curr);}));} return 1;}} static
struct Cyc_List_List* Cyc_Tc_transfer_fn_type_atts( void* t, struct Cyc_List_List*
atts){ void* _temp189= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo _temp195;
struct Cyc_List_List* _temp197; struct Cyc_List_List** _temp199; int _temp200;
struct Cyc_List_List* _temp202; void* _temp204; struct Cyc_Core_Opt* _temp206;
struct Cyc_List_List* _temp208; _LL191: if(( unsigned int) _temp189 > 4u?((
struct _tunion_struct*) _temp189)->tag == Cyc_Absyn_FnType_tag: 0){ _LL196:
_temp195=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*) _temp189)->f1;
_LL209: _temp208=( struct Cyc_List_List*) _temp195.tvars; goto _LL207; _LL207:
_temp206=( struct Cyc_Core_Opt*) _temp195.effect; goto _LL205; _LL205: _temp204=(
void*) _temp195.ret_typ; goto _LL203; _LL203: _temp202=( struct Cyc_List_List*)
_temp195.args; goto _LL201; _LL201: _temp200=( int) _temp195.varargs; goto
_LL198; _LL198: _temp197=( struct Cyc_List_List*) _temp195.attributes; _temp199=&(((
struct Cyc_Absyn_FnType_struct*) _temp189)->f1).attributes; goto _LL192;} else{
goto _LL193;} _LL193: goto _LL194; _LL192: { struct Cyc_List_List* res_atts= 0;
for( 0; atts != 0; atts=({ struct Cyc_List_List* _temp210= atts; if( _temp210 ==
0){ _throw( Null_Exception);} _temp210->tl;})){ void* _temp212=( void*)({ struct
Cyc_List_List* _temp211= atts; if( _temp211 == 0){ _throw( Null_Exception);}
_temp211->hd;}); int _temp226; _LL214: if(( unsigned int) _temp212 > 15u?((
struct _tunion_struct*) _temp212)->tag == Cyc_Absyn_Regparm_att_tag: 0){ _LL227:
_temp226=( int)(( struct Cyc_Absyn_Regparm_att_struct*) _temp212)->f1; goto
_LL215;} else{ goto _LL216;} _LL216: if(( int) _temp212 == Cyc_Absyn_Stdcall_att){
goto _LL217;} else{ goto _LL218;} _LL218: if(( int) _temp212 == Cyc_Absyn_Cdecl_att){
goto _LL219;} else{ goto _LL220;} _LL220: if(( int) _temp212 == Cyc_Absyn_Noreturn_att){
goto _LL221;} else{ goto _LL222;} _LL222: if(( int) _temp212 == Cyc_Absyn_Const_att){
goto _LL223;} else{ goto _LL224;} _LL224: goto _LL225; _LL215: goto _LL217;
_LL217: goto _LL219; _LL219: goto _LL221; _LL221: goto _LL223; _LL223:* _temp199=({
struct Cyc_List_List* _temp228=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp228->hd=( void*)(( void*)({ struct Cyc_List_List*
_temp229= atts; if( _temp229 == 0){ _throw( Null_Exception);} _temp229->hd;}));
_temp228->tl=* _temp199; _temp228;}); continue; _LL225: res_atts=({ struct Cyc_List_List*
_temp230=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp230->hd=( void*)(( void*)({ struct Cyc_List_List* _temp231= atts; if(
_temp231 == 0){ _throw( Null_Exception);} _temp231->hd;})); _temp230->tl=
res_atts; _temp230;}); continue; _LL213:;} return res_atts;} _LL194: return((
struct Cyc_List_List*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ char* _temp232=( char*)"transfer_fn_type_atts"; struct
_tagged_string _temp233; _temp233.curr= _temp232; _temp233.base= _temp232;
_temp233.last_plus_one= _temp232 + 22; _temp233;})); _LL190:;} static void Cyc_Tc_tcVardecl(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Vardecl* vd){ struct _tagged_string* v=(* vd->name).f2;
void* t=( void*) vd->type; void* sc=( void*) vd->sc; struct Cyc_List_List* atts=
vd->attributes;{ void* _temp234=(* vd->name).f1; struct Cyc_List_List* _temp242;
struct Cyc_List_List* _temp244; _LL236: if(( unsigned int) _temp234 > 1u?((
struct _tunion_struct*) _temp234)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL243:
_temp242=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp234)->f1;
if( _temp242 == 0){ goto _LL237;} else{ goto _LL238;}} else{ goto _LL238;}
_LL238: if(( unsigned int) _temp234 > 1u?(( struct _tunion_struct*) _temp234)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL245: _temp244=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp234)->f1; if( _temp244 == 0){ goto _LL239;} else{
goto _LL240;}} else{ goto _LL240;} _LL240: goto _LL241; _LL237: goto _LL235;
_LL239: goto _LL235; _LL241: Cyc_Tcutil_terr( loc,( struct _tagged_string)({
char* _temp246=( char*)"qualified declarations are not implemented"; struct
_tagged_string _temp247; _temp247.curr= _temp246; _temp247.base= _temp246;
_temp247.last_plus_one= _temp246 + 43; _temp247;})); return; _LL235:;}(* vd->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp248=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp248[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp249; _temp249.tag= Cyc_Absyn_Abs_n_tag; _temp249.f1= te->ns; _temp249;});
_temp248;});{ void* _temp250= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Exp*
_temp257; struct Cyc_Absyn_Tqual* _temp259; void* _temp261; _LL252: if((
unsigned int) _temp250 > 4u?(( struct _tunion_struct*) _temp250)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL262: _temp261=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp250)->f1;
goto _LL260; _LL260: _temp259=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp250)->f2; goto _LL258; _LL258: _temp257=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp250)->f3; if( _temp257 == 0){ goto _LL256;}
else{ goto _LL254;}} else{ goto _LL254;} _LL256: if( vd->initializer != 0){ goto
_LL253;} else{ goto _LL254;} _LL254: goto _LL255; _LL253:{ void* _temp264=( void*)({
struct Cyc_Absyn_Exp* _temp263= vd->initializer; if( _temp263 == 0){ _throw(
Null_Exception);} _temp263->r;}); void* _temp274; struct _tagged_string _temp276;
struct Cyc_List_List* _temp278; struct Cyc_Core_Opt* _temp280; struct Cyc_List_List*
_temp282; _LL266: if((( struct _tunion_struct*) _temp264)->tag == Cyc_Absyn_Const_e_tag){
_LL275: _temp274=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp264)->f1; if((
unsigned int) _temp274 > 1u?(( struct _tunion_struct*) _temp274)->tag == Cyc_Absyn_String_c_tag:
0){ _LL277: _temp276=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp274)->f1; goto _LL267;} else{ goto _LL268;}} else{ goto _LL268;} _LL268:
if((( struct _tunion_struct*) _temp264)->tag == Cyc_Absyn_UnresolvedMem_e_tag){
_LL281: _temp280=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp264)->f1; goto _LL279; _LL279: _temp278=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnresolvedMem_e_struct*) _temp264)->f2; goto _LL269;} else{ goto
_LL270;} _LL270: if((( struct _tunion_struct*) _temp264)->tag == Cyc_Absyn_Array_e_tag){
_LL283: _temp282=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp264)->f1; goto _LL271;} else{ goto _LL272;} _LL272: goto _LL273; _LL267: t=(
void*)( vd->type=( void*)(( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp284=(
struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp284[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp285; _temp285.tag= Cyc_Absyn_ArrayType_tag;
_temp285.f1=( void*) _temp261; _temp285.f2= _temp259; _temp285.f3=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(({ struct _tagged_string _temp286= _temp276;( unsigned int)(
_temp286.last_plus_one - _temp286.curr);}), 0); _temp285;}); _temp284;}))); goto
_LL265; _LL269: _temp282= _temp278; goto _LL271; _LL271: t=( void*)( vd->type=(
void*)(( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp287=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp287[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp288; _temp288.tag= Cyc_Absyn_ArrayType_tag;
_temp288.f1=( void*) _temp261; _temp288.f2= _temp259; _temp288.f3=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(( unsigned int)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp282), 0); _temp288;}); _temp287;}))); goto _LL265; _LL273: goto _LL265;
_LL265:;} goto _LL251; _LL255: goto _LL251; _LL251:;} Cyc_Tcutil_check_valid_toplevel_type(
loc, te, t); if( Cyc_Tcutil_is_function_type( t)){ atts= Cyc_Tc_transfer_fn_type_atts(
t, atts);}{ int is_redecl= Cyc_Tc_var_redecl_okay( ge, loc, v, t, sc, atts, 0);
if( sc ==( void*) Cyc_Absyn_Extern? 1: sc ==( void*) Cyc_Absyn_ExternC){ if( vd->initializer
!= 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp289=( char*)"extern declaration should not have initializer";
struct _tagged_string _temp290; _temp290.curr= _temp289; _temp290.base= _temp289;
_temp290.last_plus_one= _temp289 + 47; _temp290;}));}} else{ if( ! Cyc_Tcutil_is_function_type(
t)){ for( 0; atts != 0; atts=({ struct Cyc_List_List* _temp291= atts; if(
_temp291 == 0){ _throw( Null_Exception);} _temp291->tl;})){ void* _temp293=(
void*)({ struct Cyc_List_List* _temp292= atts; if( _temp292 == 0){ _throw(
Null_Exception);} _temp292->hd;}); int _temp313; struct _tagged_string _temp315;
_LL295: if(( unsigned int) _temp293 > 15u?(( struct _tunion_struct*) _temp293)->tag
== Cyc_Absyn_Aligned_att_tag: 0){ _LL314: _temp313=( int)(( struct Cyc_Absyn_Aligned_att_struct*)
_temp293)->f1; goto _LL296;} else{ goto _LL297;} _LL297: if(( unsigned int)
_temp293 > 15u?(( struct _tunion_struct*) _temp293)->tag == Cyc_Absyn_Section_att_tag:
0){ _LL316: _temp315=( struct _tagged_string)(( struct Cyc_Absyn_Section_att_struct*)
_temp293)->f1; goto _LL298;} else{ goto _LL299;} _LL299: if(( int) _temp293 ==
Cyc_Absyn_Nocommon_att){ goto _LL300;} else{ goto _LL301;} _LL301: if(( int)
_temp293 == Cyc_Absyn_Shared_att){ goto _LL302;} else{ goto _LL303;} _LL303: if((
int) _temp293 == Cyc_Absyn_Unused_att){ goto _LL304;} else{ goto _LL305;} _LL305:
if(( int) _temp293 == Cyc_Absyn_Weak_att){ goto _LL306;} else{ goto _LL307;}
_LL307: if(( int) _temp293 == Cyc_Absyn_Dllimport_att){ goto _LL308;} else{ goto
_LL309;} _LL309: if(( int) _temp293 == Cyc_Absyn_Dllexport_att){ goto _LL310;}
else{ goto _LL311;} _LL311: goto _LL312; _LL296: continue; _LL298: continue;
_LL300: continue; _LL302: continue; _LL304: continue; _LL306: continue; _LL308:
continue; _LL310: continue; _LL312: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp318= Cyc_Absyn_attribute2string(( void*)({ struct Cyc_List_List*
_temp317= atts; if( _temp317 == 0){ _throw( Null_Exception);} _temp317->hd;}));
struct _tagged_string _temp319= Cyc_Absynpp_qvar2string( vd->name); xprintf("bad attribute %.*s for variable %.*s",
_temp318.last_plus_one - _temp318.curr, _temp318.curr, _temp319.last_plus_one -
_temp319.curr, _temp319.curr);})); goto _LL294; _LL294:;}{ struct Cyc_Absyn_Exp*
e; if( vd->initializer == 0){ e= Cyc_Tcutil_default_initializer( te, t, loc); vd->initializer=(
struct Cyc_Absyn_Exp*) e;} else{ e=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp320= vd->initializer; if( _temp320 == 0){ _throw( Null_Exception);}
_temp320;});}{ void* t2= Cyc_Tcexp_tcExpInitializer( te,( void**)& t, e); if( !
Cyc_Tcutil_coerce_assign( te, e, t)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp321= Cyc_Absynpp_qvar2string( vd->name); struct
_tagged_string _temp322= Cyc_Absynpp_typ2string( t); struct _tagged_string
_temp323= Cyc_Absynpp_typ2string( t2); xprintf("%.*s is declared with type %.*s but initialized with type %.*s",
_temp321.last_plus_one - _temp321.curr, _temp321.curr, _temp322.last_plus_one -
_temp322.curr, _temp322.curr, _temp323.last_plus_one - _temp323.curr, _temp323.curr);}));}
if( ! Cyc_Tcutil_is_const_exp( te, e)){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp324=( char*)"initializer is not a constant expression";
struct _tagged_string _temp325; _temp325.curr= _temp324; _temp325.base= _temp324;
_temp325.last_plus_one= _temp324 + 41; _temp325;}));}}}} else{ for( 0; atts != 0;
atts=({ struct Cyc_List_List* _temp326= atts; if( _temp326 == 0){ _throw(
Null_Exception);} _temp326->tl;})){ void* _temp328=( void*)({ struct Cyc_List_List*
_temp327= atts; if( _temp327 == 0){ _throw( Null_Exception);} _temp327->hd;});
int _temp346; int _temp348; _LL330: if(( unsigned int) _temp328 > 15u?(( struct
_tunion_struct*) _temp328)->tag == Cyc_Absyn_Regparm_att_tag: 0){ _LL347:
_temp346=( int)(( struct Cyc_Absyn_Regparm_att_struct*) _temp328)->f1; goto
_LL331;} else{ goto _LL332;} _LL332: if(( int) _temp328 == Cyc_Absyn_Stdcall_att){
goto _LL333;} else{ goto _LL334;} _LL334: if(( int) _temp328 == Cyc_Absyn_Cdecl_att){
goto _LL335;} else{ goto _LL336;} _LL336: if(( int) _temp328 == Cyc_Absyn_Noreturn_att){
goto _LL337;} else{ goto _LL338;} _LL338: if(( int) _temp328 == Cyc_Absyn_Const_att){
goto _LL339;} else{ goto _LL340;} _LL340: if(( unsigned int) _temp328 > 15u?((
struct _tunion_struct*) _temp328)->tag == Cyc_Absyn_Aligned_att_tag: 0){ _LL349:
_temp348=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp328)->f1; goto
_LL341;} else{ goto _LL342;} _LL342: if(( int) _temp328 == Cyc_Absyn_Packed_att){
goto _LL343;} else{ goto _LL344;} _LL344: goto _LL345; _LL331: goto _LL333;
_LL333: goto _LL335; _LL335: goto _LL337; _LL337: goto _LL339; _LL339:(( void(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp350=( char*)"tcVardecl: fn type attributes in function var decl"; struct
_tagged_string _temp351; _temp351.curr= _temp350; _temp351.base= _temp350;
_temp351.last_plus_one= _temp350 + 51; _temp351;})); goto _LL329; _LL341: goto
_LL343; _LL343: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp353= Cyc_Absyn_attribute2string((
void*)({ struct Cyc_List_List* _temp352= atts; if( _temp352 == 0){ _throw(
Null_Exception);} _temp352->hd;})); xprintf("bad attribute %.*s in function declaration",
_temp353.last_plus_one - _temp353.curr, _temp353.curr);})); goto _LL329; _LL345:
continue; _LL329:;}}} if( !( is_redecl? sc ==( void*) Cyc_Absyn_Extern? 1: Cyc_Tcutil_is_function_type(
t): 0)){ ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key, struct _tuple6* data)) Cyc_Dict_insert)( ge->ordinaries,
v,({ struct _tuple6* _temp354=( struct _tuple6*) GC_malloc( sizeof( struct
_tuple6)); _temp354->f1=( void*)({ struct Cyc_Tcenv_VarRes_struct* _temp355=(
struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp355[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp356; _temp356.tag= Cyc_Tcenv_VarRes_tag;
_temp356.f1=( void*)(( void*)({ struct Cyc_Absyn_Global_b_struct* _temp357=(
struct Cyc_Absyn_Global_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp357[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp358; _temp358.tag= Cyc_Absyn_Global_b_tag;
_temp358.f1= vd; _temp358;}); _temp357;})); _temp356;}); _temp355;}); _temp354->f2=
is_redecl; _temp354;}));}}} static void Cyc_Tc_tcFndecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Fndecl*
fd){ struct _tagged_string* v=(* fd->name).f2; void* sc=( void*) fd->sc;{ void*
_temp359=(* fd->name).f1; struct Cyc_List_List* _temp367; struct Cyc_List_List*
_temp369; _LL361: if(( unsigned int) _temp359 > 1u?(( struct _tunion_struct*)
_temp359)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL368: _temp367=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp359)->f1; if( _temp367 == 0){ goto _LL362;}
else{ goto _LL363;}} else{ goto _LL363;} _LL363: if(( unsigned int) _temp359 > 1u?((
struct _tunion_struct*) _temp359)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL370:
_temp369=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp359)->f1;
goto _LL364;} else{ goto _LL365;} _LL365: goto _LL366; _LL362: goto _LL360;
_LL364:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp371=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp371[ 0]=({ struct Cyc_Core_Impossible_struct _temp372; _temp372.tag= Cyc_Core_Impossible_tag;
_temp372.f1=( struct _tagged_string)({ char* _temp373=( char*)"tc: Abs_n in tcFndecl";
struct _tagged_string _temp374; _temp374.curr= _temp373; _temp374.base= _temp373;
_temp374.last_plus_one= _temp373 + 22; _temp374;}); _temp372;}); _temp371;}));
_LL366: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp375=( char*)"qualified declarations are not implemented";
struct _tagged_string _temp376; _temp376.curr= _temp375; _temp376.base= _temp375;
_temp376.last_plus_one= _temp375 + 43; _temp376;})); return; _LL360:;}(* fd->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp377=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp377[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp378; _temp378.tag= Cyc_Absyn_Abs_n_tag; _temp378.f1= te->ns; _temp378;});
_temp377;}); Cyc_Tcutil_check_fndecl_valid_type( loc, te, fd);{ void* t= Cyc_Tcutil_fndecl2typ(
fd); int is_redecl= Cyc_Tc_var_redecl_okay( ge, loc, v, t, sc, fd->attributes, 1);{
struct Cyc_List_List* atts= fd->attributes; for( 0; atts != 0; atts=({ struct
Cyc_List_List* _temp379= atts; if( _temp379 == 0){ _throw( Null_Exception);}
_temp379->tl;})){ void* _temp381=( void*)({ struct Cyc_List_List* _temp380= atts;
if( _temp380 == 0){ _throw( Null_Exception);} _temp380->hd;}); int _temp389;
_LL383: if(( int) _temp381 == Cyc_Absyn_Packed_att){ goto _LL384;} else{ goto
_LL385;} _LL385: if(( unsigned int) _temp381 > 15u?(( struct _tunion_struct*)
_temp381)->tag == Cyc_Absyn_Aligned_att_tag: 0){ _LL390: _temp389=( int)((
struct Cyc_Absyn_Aligned_att_struct*) _temp381)->f1; goto _LL386;} else{ goto
_LL387;} _LL387: goto _LL388; _LL384: goto _LL386; _LL386: Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp392= Cyc_Absyn_attribute2string(( void*)({ struct Cyc_List_List*
_temp391= atts; if( _temp391 == 0){ _throw( Null_Exception);} _temp391->hd;}));
xprintf("bad attribute %.*s for function", _temp392.last_plus_one - _temp392.curr,
_temp392.curr);})); goto _LL382; _LL388: goto _LL382; _LL382:;}} ge->ordinaries=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct _tuple6* data)) Cyc_Dict_insert)( ge->ordinaries, v,({ struct _tuple6*
_temp393=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6)); _temp393->f1=(
void*)({ struct Cyc_Tcenv_VarRes_struct* _temp394=( struct Cyc_Tcenv_VarRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp394[ 0]=({ struct Cyc_Tcenv_VarRes_struct
_temp395; _temp395.tag= Cyc_Tcenv_VarRes_tag; _temp395.f1=( void*)(( void*)({
struct Cyc_Absyn_Funname_b_struct* _temp396=( struct Cyc_Absyn_Funname_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Funname_b_struct)); _temp396[ 0]=({ struct
Cyc_Absyn_Funname_b_struct _temp397; _temp397.tag= Cyc_Absyn_Funname_b_tag;
_temp397.f1= fd; _temp397;}); _temp396;})); _temp395;}); _temp394;}); _temp393->f2=
is_redecl; _temp393;})); te->le=({ struct Cyc_Core_Opt* _temp398=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp398->v=( void*)(( void*)({ struct
Cyc_Tcenv_Outermost_struct* _temp399=( struct Cyc_Tcenv_Outermost_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Outermost_struct)); _temp399[ 0]=({ struct
Cyc_Tcenv_Outermost_struct _temp400; _temp400.tag= Cyc_Tcenv_Outermost_tag;
_temp400.f1=( void*) Cyc_Tcenv_new_fenv( loc, fd); _temp400;}); _temp399;}));
_temp398;}); Cyc_Tcstmt_tcStmt( te, fd->body, 0); if( ! Cyc_Tcenv_all_labels_resolved(
te)){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp401=( char*)"function has goto statements to undefined labels";
struct _tagged_string _temp402; _temp402.curr= _temp401; _temp402.base= _temp401;
_temp402.last_plus_one= _temp401 + 49; _temp402;}));} te->le= 0;}} static void
Cyc_Tc_tcTypedefdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge,
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Typedefdecl* td){ struct
_tagged_string* v=(* td->name).f2;{ void* _temp403=(* td->name).f1; struct Cyc_List_List*
_temp411; struct Cyc_List_List* _temp413; _LL405: if(( unsigned int) _temp403 >
1u?(( struct _tunion_struct*) _temp403)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL412:
_temp411=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp403)->f1;
if( _temp411 == 0){ goto _LL406;} else{ goto _LL407;}} else{ goto _LL407;}
_LL407: if(( unsigned int) _temp403 > 1u?(( struct _tunion_struct*) _temp403)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL414: _temp413=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp403)->f1; if( _temp413 == 0){ goto _LL408;} else{
goto _LL409;}} else{ goto _LL409;} _LL409: goto _LL410; _LL406: goto _LL404;
_LL408: goto _LL404; _LL410: Cyc_Tcutil_terr( loc,( struct _tagged_string)({
char* _temp415=( char*)"qualified declarations are not implemented"; struct
_tagged_string _temp416; _temp416.curr= _temp415; _temp416.base= _temp415;
_temp416.last_plus_one= _temp415 + 43; _temp416;})); return; _LL404:;} if((( int(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_member)( ge->typedefs,
v)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp417=* v; xprintf("redeclaration of typedef %.*s",
_temp417.last_plus_one - _temp417.curr, _temp417.curr);})); return;}(* td->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp418=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp418[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp419; _temp419.tag= Cyc_Absyn_Abs_n_tag; _temp419.f1= te->ns; _temp419;});
_temp418;}); Cyc_Tcutil_check_unique_tvars( loc, td->tvs); Cyc_Tcutil_check_type(
loc, te, td->tvs,( void*) Cyc_Absyn_AnyKind,( void*) td->defn); ge->typedefs=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Typedefdecl* data)) Cyc_Dict_insert)( ge->typedefs, v, td);}
static void Cyc_Tc_tcStructdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Structdecl* sd){ if( sd->name
== 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp420=( char*)"anonymous structs are not allowed at top level";
struct _tagged_string _temp421; _temp421.curr= _temp420; _temp421.base= _temp420;
_temp421.last_plus_one= _temp420 + 47; _temp421;})); return;}{ struct
_tagged_string* v=(*(( struct _tuple0*)({ struct Cyc_Core_Opt* _temp422= sd->name;
if( _temp422 == 0){ _throw( Null_Exception);} _temp422->v;}))).f2;{ struct Cyc_List_List*
atts= sd->attributes; for( 0; atts != 0; atts=({ struct Cyc_List_List* _temp423=
atts; if( _temp423 == 0){ _throw( Null_Exception);} _temp423->tl;})){ void*
_temp425=( void*)({ struct Cyc_List_List* _temp424= atts; if( _temp424 == 0){
_throw( Null_Exception);} _temp424->hd;}); int _temp433; _LL427: if(( int)
_temp425 == Cyc_Absyn_Packed_att){ goto _LL428;} else{ goto _LL429;} _LL429: if((
unsigned int) _temp425 > 15u?(( struct _tunion_struct*) _temp425)->tag == Cyc_Absyn_Aligned_att_tag:
0){ _LL434: _temp433=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp425)->f1;
goto _LL430;} else{ goto _LL431;} _LL431: goto _LL432; _LL428: continue; _LL430:
continue; _LL432: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp436= Cyc_Absyn_attribute2string((
void*)({ struct Cyc_List_List* _temp435= atts; if( _temp435 == 0){ _throw(
Null_Exception);} _temp435->hd;})); struct _tagged_string _temp437=* v; xprintf("bad attribute %.*s in struct %.*s definition",
_temp436.last_plus_one - _temp436.curr, _temp436.curr, _temp437.last_plus_one -
_temp437.curr, _temp437.curr);})); goto _LL426; _LL426:;}}{ struct Cyc_List_List*
tvs2= sd->tvs; for( 0; tvs2 != 0; tvs2=({ struct Cyc_List_List* _temp438= tvs2;
if( _temp438 == 0){ _throw( Null_Exception);} _temp438->tl;})){ struct Cyc_Absyn_Conref*
c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp439= tvs2; if( _temp439 ==
0){ _throw( Null_Exception);} _temp439->hd;}))->kind); void* _temp440=( void*) c->v;
void* _temp448; _LL442: if(( int) _temp440 == Cyc_Absyn_No_constr){ goto _LL443;}
else{ goto _LL444;} _LL444: if(( unsigned int) _temp440 > 1u?(( struct
_tunion_struct*) _temp440)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL449: _temp448=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp440)->f1; if(( int) _temp448
== Cyc_Absyn_MemKind){ goto _LL445;} else{ goto _LL446;}} else{ goto _LL446;}
_LL446: goto _LL447; _LL443:( void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp450=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp450[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp451; _temp451.tag= Cyc_Absyn_Eq_constr_tag;
_temp451.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp451;}); _temp450;})));
continue; _LL445: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp453=* v;
struct _tagged_string _temp454=*(( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp452= tvs2; if( _temp452 == 0){ _throw( Null_Exception);} _temp452->hd;}))->name;
xprintf("struct %.*s attempts to abstract type variable %.*s of kind M",
_temp453.last_plus_one - _temp453.curr, _temp453.curr, _temp454.last_plus_one -
_temp454.curr, _temp454.curr);})); continue; _LL447: continue; _LL441:;}}{
struct Cyc_List_List* tvs= sd->tvs;{ void* _temp456=(*(( struct _tuple0*)({
struct Cyc_Core_Opt* _temp455= sd->name; if( _temp455 == 0){ _throw(
Null_Exception);} _temp455->v;}))).f1; struct Cyc_List_List* _temp464; struct
Cyc_List_List* _temp466; _LL458: if(( unsigned int) _temp456 > 1u?(( struct
_tunion_struct*) _temp456)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL465: _temp464=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp456)->f1; if(
_temp464 == 0){ goto _LL459;} else{ goto _LL460;}} else{ goto _LL460;} _LL460:
if(( unsigned int) _temp456 > 1u?(( struct _tunion_struct*) _temp456)->tag ==
Cyc_Absyn_Abs_n_tag: 0){ _LL467: _temp466=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp456)->f1; if( _temp466 == 0){ goto _LL461;} else{ goto _LL462;}} else{ goto
_LL462;} _LL462: goto _LL463; _LL459: goto _LL457; _LL461: goto _LL457; _LL463:
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp468=( char*)"qualified declarations are not implemented";
struct _tagged_string _temp469; _temp469.curr= _temp468; _temp469.base= _temp468;
_temp469.last_plus_one= _temp468 + 43; _temp469;})); return; _LL457:;}(*((
struct _tuple0*)({ struct Cyc_Core_Opt* _temp470= sd->name; if( _temp470 == 0){
_throw( Null_Exception);} _temp470->v;}))).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp471=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp471[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp472; _temp472.tag= Cyc_Absyn_Abs_n_tag;
_temp472.f1= te->ns; _temp472;}); _temp471;}); Cyc_Tcutil_check_unique_tvars(
loc, tvs);{ struct Cyc_Core_Opt* _temp473=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup_opt)( ge->structdecls, v);
struct Cyc_Core_Opt _temp479; struct Cyc_Absyn_Structdecl** _temp480; _LL475:
if( _temp473 == 0){ goto _LL476;} else{ goto _LL477;} _LL477: if( _temp473 == 0){
goto _LL474;} else{ _temp479=* _temp473; _LL481: _temp480=( struct Cyc_Absyn_Structdecl**)
_temp479.v; goto _LL478;} _LL476: ge->structdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Structdecl**
data)) Cyc_Dict_insert)( ge->structdecls, v,({ struct Cyc_Absyn_Structdecl**
_temp482=( struct Cyc_Absyn_Structdecl**) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*));
_temp482[ 0]= sd; _temp482;})); if( sd->fields == 0){ return;} goto _LL474;
_LL478: { struct Cyc_Absyn_Structdecl* sd2=* _temp480; struct Cyc_List_List*
tvs2= sd2->tvs; if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs)
!=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs2)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp483=* v; xprintf("redeclaration of struct %.*s has a different number of type parameters",
_temp483.last_plus_one - _temp483.curr, _temp483.curr);}));}{ struct Cyc_List_List*
x1= tvs; struct Cyc_List_List* x2= tvs2; for( 0; x1 != 0; x1=({ struct Cyc_List_List*
_temp484= x1; if( _temp484 == 0){ _throw( Null_Exception);} _temp484->tl;}), x2=({
struct Cyc_List_List* _temp485= x2; if( _temp485 == 0){ _throw( Null_Exception);}
_temp485->tl;})){ struct Cyc_Absyn_Conref* c1=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp486= x1; if( _temp486 == 0){ _throw( Null_Exception);}
_temp486->hd;}))->kind); struct Cyc_Absyn_Conref* c2=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp487= x2; if( _temp487 == 0){ _throw( Null_Exception);}
_temp487->hd;}))->kind); if(( void*) c1->v ==( void*) Cyc_Absyn_No_constr? c1 !=
c2: 0){( void*)( c1->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp488=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp488[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp489; _temp489.tag= Cyc_Absyn_Forward_constr_tag; _temp489.f1= c2; _temp489;});
_temp488;})));} if((( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
c1) !=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( c2)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp491=* v; struct _tagged_string _temp492= Cyc_Absynpp_ckind2string(
c1); struct _tagged_string _temp493=*(( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp490= x1; if( _temp490 == 0){ _throw( Null_Exception);} _temp490->hd;}))->name;
struct _tagged_string _temp494= Cyc_Absynpp_ckind2string( c2); xprintf("redeclaration of struct %.*s has a different kind (%.*s) for type parameter %.*s (%.*s)",
_temp491.last_plus_one - _temp491.curr, _temp491.curr, _temp492.last_plus_one -
_temp492.curr, _temp492.curr, _temp493.last_plus_one - _temp493.curr, _temp493.curr,
_temp494.last_plus_one - _temp494.curr, _temp494.curr);}));}}( void*)( sd->sc=(
void*)(( void*(*)( void* s1, struct Cyc_Core_Opt* fields1, void* s2, struct Cyc_Core_Opt*
fields2, struct Cyc_Position_Segment* loc, struct _tagged_string t, struct
_tagged_string* v)) Cyc_Tc_scope_redecl_okay)(( void*) sd->sc, sd->fields,( void*)
sd2->sc, sd2->fields, loc,( struct _tagged_string)({ char* _temp495=( char*)"struct";
struct _tagged_string _temp496; _temp496.curr= _temp495; _temp496.base= _temp495;
_temp496.last_plus_one= _temp495 + 7; _temp496;}), v));{ struct _tuple7 _temp498=({
struct _tuple7 _temp497; _temp497.f1= sd->fields; _temp497.f2= sd2->fields;
_temp497;}); struct Cyc_Core_Opt* _temp508; struct Cyc_Core_Opt* _temp510;
struct Cyc_Core_Opt* _temp512; struct Cyc_Core_Opt* _temp514; struct Cyc_Core_Opt*
_temp516; struct Cyc_Core_Opt* _temp518; struct Cyc_Core_Opt* _temp520; struct
Cyc_Core_Opt* _temp522; _LL500: _LL511: _temp510= _temp498.f1; if( _temp510 == 0){
goto _LL509;} else{ goto _LL502;} _LL509: _temp508= _temp498.f2; if( _temp508 ==
0){ goto _LL501;} else{ goto _LL502;} _LL502: _LL515: _temp514= _temp498.f1;
goto _LL513; _LL513: _temp512= _temp498.f2; if( _temp512 == 0){ goto _LL503;}
else{ goto _LL504;} _LL504: _LL519: _temp518= _temp498.f1; if( _temp518 == 0){
goto _LL517;} else{ goto _LL506;} _LL517: _temp516= _temp498.f2; goto _LL505;
_LL506: _LL523: _temp522= _temp498.f1; goto _LL521; _LL521: _temp520= _temp498.f2;
goto _LL507; _LL501: return; _LL503:* _temp480= sd; goto _LL499; _LL505: sd->fields=
sd2->fields; sd->tvs= tvs; return; _LL507: { struct Cyc_List_List* inst= 0;{
struct Cyc_List_List* tvs0= tvs; for( 0; tvs0 != 0; tvs0=({ struct Cyc_List_List*
_temp524= tvs0; if( _temp524 == 0){ _throw( Null_Exception);} _temp524->tl;}),
tvs2=({ struct Cyc_List_List* _temp525= tvs2; if( _temp525 == 0){ _throw(
Null_Exception);} _temp525->tl;})){ inst=({ struct Cyc_List_List* _temp526=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp526->hd=(
void*)({ struct _tuple4* _temp527=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp527->f1=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp531= tvs2; if( _temp531 == 0){ _throw( Null_Exception);} _temp531->hd;});
_temp527->f2=( void*)({ struct Cyc_Absyn_VarType_struct* _temp528=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp528[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp529; _temp529.tag= Cyc_Absyn_VarType_tag; _temp529.f1=( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp530= tvs0; if( _temp530 == 0){ _throw( Null_Exception);}
_temp530->hd;}); _temp529;}); _temp528;}); _temp527;}); _temp526->tl= inst;
_temp526;});}} if( ! Cyc_Tcutil_same_atts( sd->attributes, sd2->attributes)){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp532=* v; xprintf("redeclaration of struct %.*s has different attributes",
_temp532.last_plus_one - _temp532.curr, _temp532.curr);}));}{ struct Cyc_List_List*
f1s=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp533= sd->fields; if(
_temp533 == 0){ _throw( Null_Exception);} _temp533->v;}); struct Cyc_List_List*
f2s=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp534= sd2->fields; if(
_temp534 == 0){ _throw( Null_Exception);} _temp534->v;}); for( 0; f1s != 0? f2s
!= 0: 0; f1s=({ struct Cyc_List_List* _temp535= f1s; if( _temp535 == 0){ _throw(
Null_Exception);} _temp535->tl;}), f2s=({ struct Cyc_List_List* _temp536= f2s;
if( _temp536 == 0){ _throw( Null_Exception);} _temp536->tl;})){ struct Cyc_Absyn_Structfield
_temp540; struct Cyc_List_List* _temp541; struct Cyc_Core_Opt* _temp543; void*
_temp545; struct Cyc_Absyn_Tqual* _temp547; struct _tagged_string* _temp549;
struct Cyc_Absyn_Structfield* _temp538=( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp537= f1s; if( _temp537 == 0){ _throw( Null_Exception);}
_temp537->hd;}); _temp540=* _temp538; _LL550: _temp549=( struct _tagged_string*)
_temp540.name; goto _LL548; _LL548: _temp547=( struct Cyc_Absyn_Tqual*) _temp540.tq;
goto _LL546; _LL546: _temp545=( void*) _temp540.type; goto _LL544; _LL544:
_temp543=( struct Cyc_Core_Opt*) _temp540.width; goto _LL542; _LL542: _temp541=(
struct Cyc_List_List*) _temp540.attributes; goto _LL539; _LL539: { struct Cyc_Absyn_Structfield
_temp554; struct Cyc_List_List* _temp555; struct Cyc_Core_Opt* _temp557; void*
_temp559; struct Cyc_Absyn_Tqual* _temp561; struct _tagged_string* _temp563;
struct Cyc_Absyn_Structfield* _temp552=( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp551= f2s; if( _temp551 == 0){ _throw( Null_Exception);}
_temp551->hd;}); _temp554=* _temp552; _LL564: _temp563=( struct _tagged_string*)
_temp554.name; goto _LL562; _LL562: _temp561=( struct Cyc_Absyn_Tqual*) _temp554.tq;
goto _LL560; _LL560: _temp559=( void*) _temp554.type; goto _LL558; _LL558:
_temp557=( struct Cyc_Core_Opt*) _temp554.width; goto _LL556; _LL556: _temp555=(
struct Cyc_List_List*) _temp554.attributes; goto _LL553; _LL553: if( Cyc_String_zstrptrcmp(
_temp549, _temp563) != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp565=* v; struct _tagged_string _temp566=* _temp549; struct _tagged_string
_temp567=* _temp563; xprintf("redeclaration of struct %.*s: field name mismatch %.*s != %.*s",
_temp565.last_plus_one - _temp565.curr, _temp565.curr, _temp566.last_plus_one -
_temp566.curr, _temp566.curr, _temp567.last_plus_one - _temp567.curr, _temp567.curr);}));}
if( ! Cyc_Tcutil_same_atts( _temp541, _temp555)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp568=* v; struct _tagged_string _temp569=* _temp549; xprintf("redeclaration of struct %.*s: attribute mismatch on field %.*s",
_temp568.last_plus_one - _temp568.curr, _temp568.curr, _temp569.last_plus_one -
_temp569.curr, _temp569.curr);}));} if( ! Cyc_Tcutil_equal_tqual( _temp547,
_temp561)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp570=* v; struct
_tagged_string _temp571=* _temp549; xprintf("redeclaration of struct %.*s: qualifier mismatch on field %.*s",
_temp570.last_plus_one - _temp570.curr, _temp570.curr, _temp571.last_plus_one -
_temp571.curr, _temp571.curr);}));} if( _temp543 != 0? ! Cyc_Tcutil_is_const_exp(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp572= _temp543; if(
_temp572 == 0){ _throw( Null_Exception);} _temp572->v;})): 0){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp573=* v; struct _tagged_string _temp574=*
_temp549; xprintf("redeclaration of struct %.*s: bad bitfield %.*s", _temp573.last_plus_one
- _temp573.curr, _temp573.curr, _temp574.last_plus_one - _temp574.curr, _temp574.curr);}));}
else{ if((( _temp543 != 0? _temp557 != 0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp575= _temp543; if( _temp575
== 0){ _throw( Null_Exception);} _temp575->v;})) != Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp576= _temp557; if( _temp576
== 0){ _throw( Null_Exception);} _temp576->v;})): 0)? 1: _temp543 != _temp557){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp577=* v; struct
_tagged_string _temp578=* _temp549; xprintf("redeclaration of struct %.*s: bitfield mismatch on field %.*s",
_temp577.last_plus_one - _temp577.curr, _temp577.curr, _temp578.last_plus_one -
_temp578.curr, _temp578.curr);}));}} Cyc_Tcutil_check_type( loc, te, tvs,( void*)
Cyc_Absyn_MemKind, _temp545);{ void* subst_t2= Cyc_Tcutil_substitute( inst,
_temp559); if( ! Cyc_Tcutil_unify( _temp545, subst_t2)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp579=* v; struct _tagged_string _temp580=* _temp549;
struct _tagged_string _temp581= Cyc_Absynpp_typ2string( _temp545); struct
_tagged_string _temp582= Cyc_Absynpp_typ2string( subst_t2); xprintf("redeclaration of struct %.*s: type mismatch on field %.*s: %.*s != %.*s",
_temp579.last_plus_one - _temp579.curr, _temp579.curr, _temp580.last_plus_one -
_temp580.curr, _temp580.curr, _temp581.last_plus_one - _temp581.curr, _temp581.curr,
_temp582.last_plus_one - _temp582.curr, _temp582.curr);}));}}}} if( f2s != 0){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp584=* v; struct
_tagged_string _temp585=*(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp583= f2s; if( _temp583 == 0){ _throw( Null_Exception);} _temp583->hd;}))->name;
xprintf("redeclaration of struct %.*s is missing field %.*s", _temp584.last_plus_one
- _temp584.curr, _temp584.curr, _temp585.last_plus_one - _temp585.curr, _temp585.curr);}));}
if( f1s != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp587=* v;
struct _tagged_string _temp588=*(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp586= f1s; if( _temp586 == 0){ _throw( Null_Exception);} _temp586->hd;}))->name;
xprintf("redeclaration of struct %.*s has extra field %.*s", _temp587.last_plus_one
- _temp587.curr, _temp587.curr, _temp588.last_plus_one - _temp588.curr, _temp588.curr);}));}
return;}} _LL499:;} goto _LL474;}} _LL474:;}{ struct Cyc_Absyn_Structdecl** sdp=((
struct Cyc_Absyn_Structdecl**(*)( struct Cyc_Dict_Dict* d, struct _tagged_string*
key)) Cyc_Dict_lookup)( ge->structdecls, v);* sdp=({ struct Cyc_Absyn_Structdecl*
_temp589=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp589->sc=( void*)(( void*) sd->sc); _temp589->name= sd->name; _temp589->tvs=
tvs; _temp589->fields= 0; _temp589->attributes= 0; _temp589;});{ struct Cyc_List_List*
prev_fields= 0;{ struct Cyc_List_List* fs=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp590= sd->fields; if( _temp590 == 0){ _throw( Null_Exception);} _temp590->v;});
for( 0; fs != 0; fs=({ struct Cyc_List_List* _temp591= fs; if( _temp591 == 0){
_throw( Null_Exception);} _temp591->tl;})){ struct Cyc_Absyn_Structfield
_temp595; struct Cyc_List_List* _temp596; struct Cyc_Core_Opt* _temp598; void*
_temp600; struct Cyc_Absyn_Tqual* _temp602; struct _tagged_string* _temp604;
struct Cyc_Absyn_Structfield* _temp593=( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp592= fs; if( _temp592 == 0){ _throw( Null_Exception);}
_temp592->hd;}); _temp595=* _temp593; _LL605: _temp604=( struct _tagged_string*)
_temp595.name; goto _LL603; _LL603: _temp602=( struct Cyc_Absyn_Tqual*) _temp595.tq;
goto _LL601; _LL601: _temp600=( void*) _temp595.type; goto _LL599; _LL599:
_temp598=( struct Cyc_Core_Opt*) _temp595.width; goto _LL597; _LL597: _temp596=(
struct Cyc_List_List*) _temp595.attributes; goto _LL594; _LL594: if((( int(*)(
int(* compare)( struct _tagged_string*, struct _tagged_string*), struct Cyc_List_List*
l, struct _tagged_string* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp, prev_fields,
_temp604)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp606=* _temp604;
xprintf("duplicate field %.*s in struct", _temp606.last_plus_one - _temp606.curr,
_temp606.curr);}));} if( Cyc_String_strcmp(* _temp604,( struct _tagged_string)({
char* _temp607=( char*)""; struct _tagged_string _temp608; _temp608.curr=
_temp607; _temp608.base= _temp607; _temp608.last_plus_one= _temp607 + 1;
_temp608;})) != 0){ prev_fields=({ struct Cyc_List_List* _temp609=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp609->hd=( void*) _temp604;
_temp609->tl= prev_fields; _temp609;});} Cyc_Tcutil_check_type( loc, te, tvs,(
void*) Cyc_Absyn_MemKind, _temp600); if( _temp598 != 0){ unsigned int w= 0; if(
! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt*
_temp610= _temp598; if( _temp610 == 0){ _throw( Null_Exception);} _temp610->v;}))){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp611=* _temp604; xprintf("bitfield %.*s does not have constant width",
_temp611.last_plus_one - _temp611.curr, _temp611.curr);}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp612= _temp598; if( _temp612
== 0){ _throw( Null_Exception);} _temp612->v;}));}{ void* _temp613= Cyc_Tcutil_compress(
_temp600); void* _temp619; void* _temp621; _LL615: if(( unsigned int) _temp613 >
4u?(( struct _tunion_struct*) _temp613)->tag == Cyc_Absyn_IntType_tag: 0){
_LL622: _temp621=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp613)->f1;
goto _LL620; _LL620: _temp619=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp613)->f2; goto _LL616;} else{ goto _LL617;} _LL617: goto _LL618; _LL616:{
void* _temp623= _temp619; _LL625: if(( int) _temp623 == Cyc_Absyn_B1){ goto
_LL626;} else{ goto _LL627;} _LL627: if(( int) _temp623 == Cyc_Absyn_B2){ goto
_LL628;} else{ goto _LL629;} _LL629: if(( int) _temp623 == Cyc_Absyn_B4){ goto
_LL630;} else{ goto _LL631;} _LL631: if(( int) _temp623 == Cyc_Absyn_B8){ goto
_LL632;} else{ goto _LL624;} _LL626: if( w > 8){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp633=( char*)"bitfield larger than type"; struct
_tagged_string _temp634; _temp634.curr= _temp633; _temp634.base= _temp633;
_temp634.last_plus_one= _temp633 + 26; _temp634;}));} goto _LL624; _LL628: if( w
> 16){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp635=( char*)"bitfield larger than type";
struct _tagged_string _temp636; _temp636.curr= _temp635; _temp636.base= _temp635;
_temp636.last_plus_one= _temp635 + 26; _temp636;}));} goto _LL624; _LL630: if( w
> 32){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp637=( char*)"bitfield larger than type";
struct _tagged_string _temp638; _temp638.curr= _temp637; _temp638.base= _temp637;
_temp638.last_plus_one= _temp637 + 26; _temp638;}));} goto _LL624; _LL632: if( w
> 64){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp639=( char*)"bitfield larger than type";
struct _tagged_string _temp640; _temp640.curr= _temp639; _temp640.base= _temp639;
_temp640.last_plus_one= _temp639 + 26; _temp640;}));} goto _LL624; _LL624:;}
goto _LL614; _LL618: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp641=*
_temp604; struct _tagged_string _temp642= Cyc_Absynpp_typ2string( _temp600);
xprintf("bitfield %.*s must have integral type but has type %.*s", _temp641.last_plus_one
- _temp641.curr, _temp641.curr, _temp642.last_plus_one - _temp642.curr, _temp642.curr);}));
goto _LL614; _LL614:;}} for( 0; _temp596 != 0; _temp596=({ struct Cyc_List_List*
_temp643= _temp596; if( _temp643 == 0){ _throw( Null_Exception);} _temp643->tl;})){
void* _temp645=( void*)({ struct Cyc_List_List* _temp644= _temp596; if( _temp644
== 0){ _throw( Null_Exception);} _temp644->hd;}); int _temp653; _LL647: if(( int)
_temp645 == Cyc_Absyn_Packed_att){ goto _LL648;} else{ goto _LL649;} _LL649: if((
unsigned int) _temp645 > 15u?(( struct _tunion_struct*) _temp645)->tag == Cyc_Absyn_Aligned_att_tag:
0){ _LL654: _temp653=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp645)->f1;
goto _LL650;} else{ goto _LL651;} _LL651: goto _LL652; _LL648: continue; _LL650:
continue; _LL652: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp656= Cyc_Absyn_attribute2string((
void*)({ struct Cyc_List_List* _temp655= _temp596; if( _temp655 == 0){ _throw(
Null_Exception);} _temp655->hd;})); struct _tagged_string _temp657=* v; struct
_tagged_string _temp658=* _temp604; xprintf("bad attribute %.*s in struct %.*s, member %.*s",
_temp656.last_plus_one - _temp656.curr, _temp656.curr, _temp657.last_plus_one -
_temp657.curr, _temp657.curr, _temp658.last_plus_one - _temp658.curr, _temp658.curr);}));
goto _LL646; _LL646:;}}} ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct _tuple6* data)) Cyc_Dict_insert)( ge->ordinaries,
v,({ struct _tuple6* _temp659=( struct _tuple6*) GC_malloc( sizeof( struct
_tuple6)); _temp659->f1=( void*)({ struct Cyc_Tcenv_StructRes_struct* _temp660=(
struct Cyc_Tcenv_StructRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_StructRes_struct));
_temp660[ 0]=({ struct Cyc_Tcenv_StructRes_struct _temp661; _temp661.tag= Cyc_Tcenv_StructRes_tag;
_temp661.f1= sd; _temp661;}); _temp660;}); _temp659->f2= 1; _temp659;}));* sdp=
sd;}}}}} static void Cyc_Tc_tcUniondecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Uniondecl* ud){ if( ud->name
== 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp662=( char*)"anonymous unions are not allowed at top level";
struct _tagged_string _temp663; _temp663.curr= _temp662; _temp663.base= _temp662;
_temp663.last_plus_one= _temp662 + 46; _temp663;})); return;}{ struct
_tagged_string* v=(*(( struct _tuple0*)({ struct Cyc_Core_Opt* _temp664= ud->name;
if( _temp664 == 0){ _throw( Null_Exception);} _temp664->v;}))).f2;{ struct Cyc_List_List*
atts= ud->attributes; for( 0; atts != 0; atts=({ struct Cyc_List_List* _temp665=
atts; if( _temp665 == 0){ _throw( Null_Exception);} _temp665->tl;})){ void*
_temp667=( void*)({ struct Cyc_List_List* _temp666= atts; if( _temp666 == 0){
_throw( Null_Exception);} _temp666->hd;}); int _temp675; _LL669: if(( int)
_temp667 == Cyc_Absyn_Packed_att){ goto _LL670;} else{ goto _LL671;} _LL671: if((
unsigned int) _temp667 > 15u?(( struct _tunion_struct*) _temp667)->tag == Cyc_Absyn_Aligned_att_tag:
0){ _LL676: _temp675=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp667)->f1;
goto _LL672;} else{ goto _LL673;} _LL673: goto _LL674; _LL670: continue; _LL672:
continue; _LL674: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp678= Cyc_Absyn_attribute2string((
void*)({ struct Cyc_List_List* _temp677= atts; if( _temp677 == 0){ _throw(
Null_Exception);} _temp677->hd;})); struct _tagged_string _temp679=* v; xprintf("bad attribute %.*s in union %.*s definition",
_temp678.last_plus_one - _temp678.curr, _temp678.curr, _temp679.last_plus_one -
_temp679.curr, _temp679.curr);})); goto _LL668; _LL668:;}}{ struct Cyc_List_List*
tvs2= ud->tvs; for( 0; tvs2 != 0; tvs2=({ struct Cyc_List_List* _temp680= tvs2;
if( _temp680 == 0){ _throw( Null_Exception);} _temp680->tl;})){ struct Cyc_Absyn_Conref*
c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp681= tvs2; if( _temp681 ==
0){ _throw( Null_Exception);} _temp681->hd;}))->kind); void* _temp682=( void*) c->v;
void* _temp690; _LL684: if(( int) _temp682 == Cyc_Absyn_No_constr){ goto _LL685;}
else{ goto _LL686;} _LL686: if(( unsigned int) _temp682 > 1u?(( struct
_tunion_struct*) _temp682)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL691: _temp690=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp682)->f1; if(( int) _temp690
== Cyc_Absyn_MemKind){ goto _LL687;} else{ goto _LL688;}} else{ goto _LL688;}
_LL688: goto _LL689; _LL685:( void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp692=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp692[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp693; _temp693.tag= Cyc_Absyn_Eq_constr_tag;
_temp693.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp693;}); _temp692;})));
continue; _LL687: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp695=* v;
struct _tagged_string _temp696=*(( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp694= tvs2; if( _temp694 == 0){ _throw( Null_Exception);} _temp694->hd;}))->name;
xprintf("union %.*s attempts to abstract type variable %.*s of kind M", _temp695.last_plus_one
- _temp695.curr, _temp695.curr, _temp696.last_plus_one - _temp696.curr, _temp696.curr);}));
continue; _LL689: continue; _LL683:;}}{ struct Cyc_List_List* tvs= ud->tvs;{
void* _temp698=(*(( struct _tuple0*)({ struct Cyc_Core_Opt* _temp697= ud->name;
if( _temp697 == 0){ _throw( Null_Exception);} _temp697->v;}))).f1; struct Cyc_List_List*
_temp706; struct Cyc_List_List* _temp708; _LL700: if(( unsigned int) _temp698 >
1u?(( struct _tunion_struct*) _temp698)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL707:
_temp706=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp698)->f1;
if( _temp706 == 0){ goto _LL701;} else{ goto _LL702;}} else{ goto _LL702;}
_LL702: if(( unsigned int) _temp698 > 1u?(( struct _tunion_struct*) _temp698)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL709: _temp708=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp698)->f1; if( _temp708 == 0){ goto _LL703;} else{
goto _LL704;}} else{ goto _LL704;} _LL704: goto _LL705; _LL701: goto _LL699;
_LL703: goto _LL699; _LL705: Cyc_Tcutil_terr( loc,( struct _tagged_string)({
char* _temp710=( char*)"qualified declarations are not implemented"; struct
_tagged_string _temp711; _temp711.curr= _temp710; _temp711.base= _temp710;
_temp711.last_plus_one= _temp710 + 43; _temp711;})); return; _LL699:;}(*((
struct _tuple0*)({ struct Cyc_Core_Opt* _temp712= ud->name; if( _temp712 == 0){
_throw( Null_Exception);} _temp712->v;}))).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp713=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp713[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp714; _temp714.tag= Cyc_Absyn_Abs_n_tag;
_temp714.f1= te->ns; _temp714;}); _temp713;}); Cyc_Tcutil_check_unique_tvars(
loc, tvs);{ struct Cyc_Core_Opt* _temp715=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup_opt)( ge->uniondecls, v); struct
Cyc_Core_Opt _temp721; struct Cyc_Absyn_Uniondecl** _temp722; _LL717: if(
_temp715 == 0){ goto _LL718;} else{ goto _LL719;} _LL719: if( _temp715 == 0){
goto _LL716;} else{ _temp721=* _temp715; _LL723: _temp722=( struct Cyc_Absyn_Uniondecl**)
_temp721.v; goto _LL720;} _LL718: ge->uniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl**
data)) Cyc_Dict_insert)( ge->uniondecls, v,({ struct Cyc_Absyn_Uniondecl**
_temp724=( struct Cyc_Absyn_Uniondecl**) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl*));
_temp724[ 0]= ud; _temp724;})); if( ud->fields == 0){ return;} goto _LL716;
_LL720: { struct Cyc_Absyn_Uniondecl* ud2=* _temp722; struct Cyc_List_List* tvs2=
ud2->tvs; if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs) !=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs2)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp725=* v; xprintf("redeclaration of union %.*s has a different number of type parameters",
_temp725.last_plus_one - _temp725.curr, _temp725.curr);}));}{ struct Cyc_List_List*
x1= tvs; struct Cyc_List_List* x2= tvs2; for( 0; x1 != 0; x1=({ struct Cyc_List_List*
_temp726= x1; if( _temp726 == 0){ _throw( Null_Exception);} _temp726->tl;}), x2=({
struct Cyc_List_List* _temp727= x2; if( _temp727 == 0){ _throw( Null_Exception);}
_temp727->tl;})){ struct Cyc_Absyn_Conref* c1=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp728= x1; if( _temp728 == 0){ _throw( Null_Exception);}
_temp728->hd;}))->kind); struct Cyc_Absyn_Conref* c2=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp729= x2; if( _temp729 == 0){ _throw( Null_Exception);}
_temp729->hd;}))->kind); if(( void*) c1->v ==( void*) Cyc_Absyn_No_constr? c1 !=
c2: 0){( void*)( c1->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp730=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp730[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp731; _temp731.tag= Cyc_Absyn_Forward_constr_tag; _temp731.f1= c2; _temp731;});
_temp730;})));} if((( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
c1) !=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( c2)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp733=* v; struct _tagged_string _temp734= Cyc_Absynpp_ckind2string(
c1); struct _tagged_string _temp735=*(( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp732= x1; if( _temp732 == 0){ _throw( Null_Exception);} _temp732->hd;}))->name;
struct _tagged_string _temp736= Cyc_Absynpp_ckind2string( c2); xprintf("redeclaration of union %.*s has a different kind (%.*s) for type parameter %.*s (%.*s)",
_temp733.last_plus_one - _temp733.curr, _temp733.curr, _temp734.last_plus_one -
_temp734.curr, _temp734.curr, _temp735.last_plus_one - _temp735.curr, _temp735.curr,
_temp736.last_plus_one - _temp736.curr, _temp736.curr);}));}}( void*)( ud->sc=(
void*)(( void*(*)( void* s1, struct Cyc_Core_Opt* fields1, void* s2, struct Cyc_Core_Opt*
fields2, struct Cyc_Position_Segment* loc, struct _tagged_string t, struct
_tagged_string* v)) Cyc_Tc_scope_redecl_okay)(( void*) ud->sc, ud->fields,( void*)
ud2->sc, ud2->fields, loc,( struct _tagged_string)({ char* _temp737=( char*)"union";
struct _tagged_string _temp738; _temp738.curr= _temp737; _temp738.base= _temp737;
_temp738.last_plus_one= _temp737 + 6; _temp738;}), v));{ struct _tuple7 _temp740=({
struct _tuple7 _temp739; _temp739.f1= ud->fields; _temp739.f2= ud2->fields;
_temp739;}); struct Cyc_Core_Opt* _temp750; struct Cyc_Core_Opt* _temp752;
struct Cyc_Core_Opt* _temp754; struct Cyc_Core_Opt* _temp756; struct Cyc_Core_Opt*
_temp758; struct Cyc_Core_Opt* _temp760; struct Cyc_Core_Opt* _temp762; struct
Cyc_Core_Opt* _temp764; _LL742: _LL753: _temp752= _temp740.f1; if( _temp752 == 0){
goto _LL751;} else{ goto _LL744;} _LL751: _temp750= _temp740.f2; if( _temp750 ==
0){ goto _LL743;} else{ goto _LL744;} _LL744: _LL757: _temp756= _temp740.f1;
goto _LL755; _LL755: _temp754= _temp740.f2; if( _temp754 == 0){ goto _LL745;}
else{ goto _LL746;} _LL746: _LL761: _temp760= _temp740.f1; if( _temp760 == 0){
goto _LL759;} else{ goto _LL748;} _LL759: _temp758= _temp740.f2; goto _LL747;
_LL748: _LL765: _temp764= _temp740.f1; goto _LL763; _LL763: _temp762= _temp740.f2;
goto _LL749; _LL743: return; _LL745:* _temp722= ud; goto _LL741; _LL747: ud->fields=
ud2->fields; ud->tvs= tvs; return; _LL749: { struct Cyc_List_List* inst= 0;{
struct Cyc_List_List* tvs0= tvs; for( 0; tvs0 != 0; tvs0=({ struct Cyc_List_List*
_temp766= tvs0; if( _temp766 == 0){ _throw( Null_Exception);} _temp766->tl;}),
tvs2=({ struct Cyc_List_List* _temp767= tvs2; if( _temp767 == 0){ _throw(
Null_Exception);} _temp767->tl;})){ inst=({ struct Cyc_List_List* _temp768=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp768->hd=(
void*)({ struct _tuple4* _temp769=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp769->f1=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp773= tvs2; if( _temp773 == 0){ _throw( Null_Exception);} _temp773->hd;});
_temp769->f2=( void*)({ struct Cyc_Absyn_VarType_struct* _temp770=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp770[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp771; _temp771.tag= Cyc_Absyn_VarType_tag; _temp771.f1=( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp772= tvs0; if( _temp772 == 0){ _throw( Null_Exception);}
_temp772->hd;}); _temp771;}); _temp770;}); _temp769;}); _temp768->tl= inst;
_temp768;});}} if( ! Cyc_Tcutil_same_atts( ud->attributes, ud2->attributes)){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp774=* v; xprintf("redeclaration of union %.*s has different attributes",
_temp774.last_plus_one - _temp774.curr, _temp774.curr);}));}{ struct Cyc_List_List*
f1s=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp775= ud->fields; if(
_temp775 == 0){ _throw( Null_Exception);} _temp775->v;}); struct Cyc_List_List*
f2s=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp776= ud2->fields; if(
_temp776 == 0){ _throw( Null_Exception);} _temp776->v;}); for( 0; f1s != 0? f2s
!= 0: 0; f1s=({ struct Cyc_List_List* _temp777= f1s; if( _temp777 == 0){ _throw(
Null_Exception);} _temp777->tl;}), f2s=({ struct Cyc_List_List* _temp778= f2s;
if( _temp778 == 0){ _throw( Null_Exception);} _temp778->tl;})){ struct Cyc_Absyn_Structfield
_temp782; struct Cyc_List_List* _temp783; struct Cyc_Core_Opt* _temp785; void*
_temp787; struct Cyc_Absyn_Tqual* _temp789; struct _tagged_string* _temp791;
struct Cyc_Absyn_Structfield* _temp780=( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp779= f1s; if( _temp779 == 0){ _throw( Null_Exception);}
_temp779->hd;}); _temp782=* _temp780; _LL792: _temp791=( struct _tagged_string*)
_temp782.name; goto _LL790; _LL790: _temp789=( struct Cyc_Absyn_Tqual*) _temp782.tq;
goto _LL788; _LL788: _temp787=( void*) _temp782.type; goto _LL786; _LL786:
_temp785=( struct Cyc_Core_Opt*) _temp782.width; goto _LL784; _LL784: _temp783=(
struct Cyc_List_List*) _temp782.attributes; goto _LL781; _LL781: { struct Cyc_Absyn_Structfield
_temp796; struct Cyc_List_List* _temp797; struct Cyc_Core_Opt* _temp799; void*
_temp801; struct Cyc_Absyn_Tqual* _temp803; struct _tagged_string* _temp805;
struct Cyc_Absyn_Structfield* _temp794=( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp793= f2s; if( _temp793 == 0){ _throw( Null_Exception);}
_temp793->hd;}); _temp796=* _temp794; _LL806: _temp805=( struct _tagged_string*)
_temp796.name; goto _LL804; _LL804: _temp803=( struct Cyc_Absyn_Tqual*) _temp796.tq;
goto _LL802; _LL802: _temp801=( void*) _temp796.type; goto _LL800; _LL800:
_temp799=( struct Cyc_Core_Opt*) _temp796.width; goto _LL798; _LL798: _temp797=(
struct Cyc_List_List*) _temp796.attributes; goto _LL795; _LL795: if( Cyc_String_zstrptrcmp(
_temp791, _temp805) != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp807=* v; struct _tagged_string _temp808=* _temp791; struct _tagged_string
_temp809=* _temp805; xprintf("redeclaration of union %.*s: field name mismatch %.*s != %.*s",
_temp807.last_plus_one - _temp807.curr, _temp807.curr, _temp808.last_plus_one -
_temp808.curr, _temp808.curr, _temp809.last_plus_one - _temp809.curr, _temp809.curr);}));}
if( ! Cyc_Tcutil_same_atts( _temp783, _temp797)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp810=* v; struct _tagged_string _temp811=* _temp791; xprintf("redeclaration of union %.*s: attribute mismatch on field %.*s",
_temp810.last_plus_one - _temp810.curr, _temp810.curr, _temp811.last_plus_one -
_temp811.curr, _temp811.curr);}));} if( ! Cyc_Tcutil_equal_tqual( _temp789,
_temp803)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp812=* v; struct
_tagged_string _temp813=* _temp791; xprintf("redeclaration of union %.*s: qualifier mismatch on field %.*s",
_temp812.last_plus_one - _temp812.curr, _temp812.curr, _temp813.last_plus_one -
_temp813.curr, _temp813.curr);}));} if( _temp785 != 0? ! Cyc_Tcutil_is_const_exp(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp814= _temp785; if(
_temp814 == 0){ _throw( Null_Exception);} _temp814->v;})): 0){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp815=* v; struct _tagged_string _temp816=*
_temp791; xprintf("redeclaration of struct %.*s: bad bitfield %.*s", _temp815.last_plus_one
- _temp815.curr, _temp815.curr, _temp816.last_plus_one - _temp816.curr, _temp816.curr);}));}
else{ if((( _temp785 != 0? _temp799 != 0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp817= _temp785; if( _temp817
== 0){ _throw( Null_Exception);} _temp817->v;})) != Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp818= _temp799; if( _temp818
== 0){ _throw( Null_Exception);} _temp818->v;})): 0)? 1: _temp785 != _temp799){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp819=* v; struct
_tagged_string _temp820=* _temp791; xprintf("redeclaration of struct %.*s: bitfield mismatch on field %.*s",
_temp819.last_plus_one - _temp819.curr, _temp819.curr, _temp820.last_plus_one -
_temp820.curr, _temp820.curr);}));}} Cyc_Tcutil_check_type( loc, te, tvs,( void*)
Cyc_Absyn_MemKind, _temp787);{ void* subst_t2= Cyc_Tcutil_substitute( inst,
_temp801); if( ! Cyc_Tcutil_unify( _temp787, subst_t2)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp821=* v; struct _tagged_string _temp822=* _temp791;
struct _tagged_string _temp823= Cyc_Absynpp_typ2string( _temp787); struct
_tagged_string _temp824= Cyc_Absynpp_typ2string( subst_t2); xprintf("redeclaration of union %.*s: type mismatch on field %.*s: %.*s != %.*s",
_temp821.last_plus_one - _temp821.curr, _temp821.curr, _temp822.last_plus_one -
_temp822.curr, _temp822.curr, _temp823.last_plus_one - _temp823.curr, _temp823.curr,
_temp824.last_plus_one - _temp824.curr, _temp824.curr);}));}}}} if( f2s != 0){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp826=* v; struct
_tagged_string _temp827=*(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp825= f2s; if( _temp825 == 0){ _throw( Null_Exception);} _temp825->hd;}))->name;
xprintf("redeclaration of union %.*s is missing field %.*s", _temp826.last_plus_one
- _temp826.curr, _temp826.curr, _temp827.last_plus_one - _temp827.curr, _temp827.curr);}));}
if( f1s != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp829=* v;
struct _tagged_string _temp830=*(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp828= f1s; if( _temp828 == 0){ _throw( Null_Exception);} _temp828->hd;}))->name;
xprintf("redeclaration of union %.*s has extra field %.*s", _temp829.last_plus_one
- _temp829.curr, _temp829.curr, _temp830.last_plus_one - _temp830.curr, _temp830.curr);}));}
return;}} _LL741:;} goto _LL716;}} _LL716:;}{ struct Cyc_Absyn_Uniondecl** udp=((
struct Cyc_Absyn_Uniondecl**(*)( struct Cyc_Dict_Dict* d, struct _tagged_string*
key)) Cyc_Dict_lookup)( ge->uniondecls, v);* udp=({ struct Cyc_Absyn_Uniondecl*
_temp831=( struct Cyc_Absyn_Uniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl));
_temp831->sc=( void*)(( void*) ud->sc); _temp831->name= ud->name; _temp831->tvs=
tvs; _temp831->fields= 0; _temp831->attributes= 0; _temp831;});{ struct Cyc_List_List*
prev_fields= 0; struct Cyc_List_List* prev_types= 0;{ struct Cyc_List_List* fs=(
struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp832= ud->fields; if( _temp832
== 0){ _throw( Null_Exception);} _temp832->v;}); for( 0; fs != 0; fs=({ struct
Cyc_List_List* _temp833= fs; if( _temp833 == 0){ _throw( Null_Exception);}
_temp833->tl;})){ struct Cyc_Absyn_Structfield _temp837; struct Cyc_List_List*
_temp838; struct Cyc_Core_Opt* _temp840; void* _temp842; struct Cyc_Absyn_Tqual*
_temp844; struct _tagged_string* _temp846; struct Cyc_Absyn_Structfield*
_temp835=( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp834= fs;
if( _temp834 == 0){ _throw( Null_Exception);} _temp834->hd;}); _temp837=*
_temp835; _LL847: _temp846=( struct _tagged_string*) _temp837.name; goto _LL845;
_LL845: _temp844=( struct Cyc_Absyn_Tqual*) _temp837.tq; goto _LL843; _LL843:
_temp842=( void*) _temp837.type; goto _LL841; _LL841: _temp840=( struct Cyc_Core_Opt*)
_temp837.width; goto _LL839; _LL839: _temp838=( struct Cyc_List_List*) _temp837.attributes;
goto _LL836; _LL836: if((( int(*)( int(* compare)( struct _tagged_string*,
struct _tagged_string*), struct Cyc_List_List* l, struct _tagged_string* x)) Cyc_List_mem)(
Cyc_String_zstrptrcmp, prev_fields, _temp846)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp848=* _temp846; xprintf("duplicate field %.*s in union",
_temp848.last_plus_one - _temp848.curr, _temp848.curr);}));} prev_fields=({
struct Cyc_List_List* _temp849=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp849->hd=( void*) _temp846; _temp849->tl=
prev_fields; _temp849;}); Cyc_Tcutil_check_type( loc, te, tvs,( void*) Cyc_Absyn_MemKind,
_temp842); if( _temp840 != 0){ unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp850= _temp840; if(
_temp850 == 0){ _throw( Null_Exception);} _temp850->v;}))){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp851=* _temp846; xprintf("bitfield %.*s does not have constant width",
_temp851.last_plus_one - _temp851.curr, _temp851.curr);}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp852= _temp840; if( _temp852
== 0){ _throw( Null_Exception);} _temp852->v;}));}{ void* _temp853= Cyc_Tcutil_compress(
_temp842); void* _temp859; void* _temp861; _LL855: if(( unsigned int) _temp853 >
4u?(( struct _tunion_struct*) _temp853)->tag == Cyc_Absyn_IntType_tag: 0){
_LL862: _temp861=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp853)->f1;
goto _LL860; _LL860: _temp859=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp853)->f2; goto _LL856;} else{ goto _LL857;} _LL857: goto _LL858; _LL856:{
void* _temp863= _temp859; _LL865: if(( int) _temp863 == Cyc_Absyn_B1){ goto
_LL866;} else{ goto _LL867;} _LL867: if(( int) _temp863 == Cyc_Absyn_B2){ goto
_LL868;} else{ goto _LL869;} _LL869: if(( int) _temp863 == Cyc_Absyn_B4){ goto
_LL870;} else{ goto _LL871;} _LL871: if(( int) _temp863 == Cyc_Absyn_B8){ goto
_LL872;} else{ goto _LL864;} _LL866: if( w > 8){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp873=( char*)"bitfield larger than type"; struct
_tagged_string _temp874; _temp874.curr= _temp873; _temp874.base= _temp873;
_temp874.last_plus_one= _temp873 + 26; _temp874;}));} goto _LL864; _LL868: if( w
> 16){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp875=( char*)"bitfield larger than type";
struct _tagged_string _temp876; _temp876.curr= _temp875; _temp876.base= _temp875;
_temp876.last_plus_one= _temp875 + 26; _temp876;}));} goto _LL864; _LL870: if( w
> 32){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp877=( char*)"bitfield larger than type";
struct _tagged_string _temp878; _temp878.curr= _temp877; _temp878.base= _temp877;
_temp878.last_plus_one= _temp877 + 26; _temp878;}));} goto _LL864; _LL872: if( w
> 64){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp879=( char*)"bitfield larger than type";
struct _tagged_string _temp880; _temp880.curr= _temp879; _temp880.base= _temp879;
_temp880.last_plus_one= _temp879 + 26; _temp880;}));} goto _LL864; _LL864:;}
goto _LL854; _LL858: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp881=*
_temp846; struct _tagged_string _temp882= Cyc_Absynpp_typ2string( _temp842);
xprintf("bitfield %.*s must have integral type but has type %.*s", _temp881.last_plus_one
- _temp881.curr, _temp881.curr, _temp882.last_plus_one - _temp882.curr, _temp882.curr);}));
goto _LL854; _LL854:;}} if( ! Cyc_Tcutil_bits_only( _temp842)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp883=* _temp846; struct _tagged_string _temp884=*
v; xprintf("%.*s has a type that is possibly incompatible with other members of union %.*s",
_temp883.last_plus_one - _temp883.curr, _temp883.curr, _temp884.last_plus_one -
_temp884.curr, _temp884.curr);}));} for( 0; _temp838 != 0; _temp838=({ struct
Cyc_List_List* _temp885= _temp838; if( _temp885 == 0){ _throw( Null_Exception);}
_temp885->tl;})){ void* _temp887=( void*)({ struct Cyc_List_List* _temp886=
_temp838; if( _temp886 == 0){ _throw( Null_Exception);} _temp886->hd;}); int
_temp895; _LL889: if(( int) _temp887 == Cyc_Absyn_Packed_att){ goto _LL890;}
else{ goto _LL891;} _LL891: if(( unsigned int) _temp887 > 15u?(( struct
_tunion_struct*) _temp887)->tag == Cyc_Absyn_Aligned_att_tag: 0){ _LL896:
_temp895=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp887)->f1; goto
_LL892;} else{ goto _LL893;} _LL893: goto _LL894; _LL890: continue; _LL892:
continue; _LL894: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp898= Cyc_Absyn_attribute2string((
void*)({ struct Cyc_List_List* _temp897= _temp838; if( _temp897 == 0){ _throw(
Null_Exception);} _temp897->hd;})); struct _tagged_string _temp899=* v; struct
_tagged_string _temp900=* _temp846; xprintf("bad attribute %.*s in union %.*s, member %.*s",
_temp898.last_plus_one - _temp898.curr, _temp898.curr, _temp899.last_plus_one -
_temp899.curr, _temp899.curr, _temp900.last_plus_one - _temp900.curr, _temp900.curr);}));
goto _LL888; _LL888:;}}}* udp= ud;}}}}} static void Cyc_Tc_tcTuniondecl( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc,
struct Cyc_Absyn_Tuniondecl* ed){ if( ed->name == 0){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp901=( char*)"anonymous tunions are not allowed";
struct _tagged_string _temp902; _temp902.curr= _temp901; _temp902.base= _temp901;
_temp902.last_plus_one= _temp901 + 34; _temp902;})); return;}{ struct
_tagged_string* v=(*(( struct _tuple0*)({ struct Cyc_Core_Opt* _temp903= ed->name;
if( _temp903 == 0){ _throw( Null_Exception);} _temp903->v;}))).f2; struct Cyc_List_List*
tvs= ed->tvs;{ struct Cyc_List_List* tvs2= tvs; for( 0; tvs2 != 0; tvs2=({
struct Cyc_List_List* _temp904= tvs2; if( _temp904 == 0){ _throw( Null_Exception);}
_temp904->tl;})){ struct Cyc_Absyn_Conref* c=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp905= tvs2; if( _temp905 == 0){ _throw( Null_Exception);}
_temp905->hd;}))->kind); void* _temp906=( void*) c->v; void* _temp914; _LL908:
if(( int) _temp906 == Cyc_Absyn_No_constr){ goto _LL909;} else{ goto _LL910;}
_LL910: if(( unsigned int) _temp906 > 1u?(( struct _tunion_struct*) _temp906)->tag
== Cyc_Absyn_Eq_constr_tag: 0){ _LL915: _temp914=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp906)->f1; if(( int) _temp914 == Cyc_Absyn_MemKind){ goto _LL911;} else{
goto _LL912;}} else{ goto _LL912;} _LL912: goto _LL913; _LL909:( void*)( c->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp916=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp916[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp917; _temp917.tag= Cyc_Absyn_Eq_constr_tag;
_temp917.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp917;}); _temp916;})));
goto _LL907; _LL911: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp919=* v;
struct _tagged_string _temp920=*(( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp918= tvs2; if( _temp918 == 0){ _throw( Null_Exception);} _temp918->hd;}))->name;
xprintf("tunion %.*s attempts to abstract type variable %.*s of kind M",
_temp919.last_plus_one - _temp919.curr, _temp919.curr, _temp920.last_plus_one -
_temp920.curr, _temp920.curr);})); goto _LL907; _LL913: goto _LL907; _LL907:;}}{
void* _temp922=(*(( struct _tuple0*)({ struct Cyc_Core_Opt* _temp921= ed->name;
if( _temp921 == 0){ _throw( Null_Exception);} _temp921->v;}))).f1; struct Cyc_List_List*
_temp930; struct Cyc_List_List* _temp932; _LL924: if(( unsigned int) _temp922 >
1u?(( struct _tunion_struct*) _temp922)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL931:
_temp930=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp922)->f1;
if( _temp930 == 0){ goto _LL925;} else{ goto _LL926;}} else{ goto _LL926;}
_LL926: if(( unsigned int) _temp922 > 1u?(( struct _tunion_struct*) _temp922)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL933: _temp932=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp922)->f1; if( _temp932 == 0){ goto _LL927;} else{
goto _LL928;}} else{ goto _LL928;} _LL928: goto _LL929; _LL925: goto _LL923;
_LL927: goto _LL923; _LL929: Cyc_Tcutil_terr( loc,( struct _tagged_string)({
char* _temp934=( char*)"qualified declarations are not implemented"; struct
_tagged_string _temp935; _temp935.curr= _temp934; _temp935.base= _temp934;
_temp935.last_plus_one= _temp934 + 43; _temp935;})); return; _LL923:;}(*((
struct _tuple0*)({ struct Cyc_Core_Opt* _temp936= ed->name; if( _temp936 == 0){
_throw( Null_Exception);} _temp936->v;}))).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp937=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp937[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp938; _temp938.tag= Cyc_Absyn_Abs_n_tag;
_temp938.f1= te->ns; _temp938;}); _temp937;}); Cyc_Tcutil_check_unique_tvars(
loc, tvs);{ struct Cyc_Core_Opt* _temp939=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup_opt)( ge->tuniondecls, v);
struct Cyc_Core_Opt _temp945; struct Cyc_Absyn_Tuniondecl** _temp946; _LL941:
if( _temp939 == 0){ goto _LL942;} else{ goto _LL943;} _LL943: if( _temp939 == 0){
goto _LL940;} else{ _temp945=* _temp939; _LL947: _temp946=( struct Cyc_Absyn_Tuniondecl**)
_temp945.v; goto _LL944;} _LL942: ge->tuniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Tuniondecl**
data)) Cyc_Dict_insert)( ge->tuniondecls, v,({ struct Cyc_Absyn_Tuniondecl**
_temp948=( struct Cyc_Absyn_Tuniondecl**) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl*));
_temp948[ 0]= ed; _temp948;})); if( ed->fields == 0){ return;} goto _LL940;
_LL944: { struct Cyc_Absyn_Tuniondecl* ed2=* _temp946; struct Cyc_List_List*
tvs2= ed2->tvs; if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs)
!=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs2)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp949=* v; xprintf("redeclaration of tunion %.*s has a different number of type parameters",
_temp949.last_plus_one - _temp949.curr, _temp949.curr);}));}{ struct Cyc_List_List*
x1= tvs; struct Cyc_List_List* x2= tvs2; for( 0; x1 != 0; x1=({ struct Cyc_List_List*
_temp950= x1; if( _temp950 == 0){ _throw( Null_Exception);} _temp950->tl;}), x2=({
struct Cyc_List_List* _temp951= x2; if( _temp951 == 0){ _throw( Null_Exception);}
_temp951->tl;})){ struct Cyc_Absyn_Conref* c1=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp952= x1; if( _temp952 == 0){ _throw( Null_Exception);}
_temp952->hd;}))->kind); struct Cyc_Absyn_Conref* c2=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp953= x2; if( _temp953 == 0){ _throw( Null_Exception);}
_temp953->hd;}))->kind); if(( void*) c1->v ==( void*) Cyc_Absyn_No_constr? c1 !=
c2: 0){( void*)( c1->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp954=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp954[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp955; _temp955.tag= Cyc_Absyn_Forward_constr_tag; _temp955.f1= c2; _temp955;});
_temp954;})));} if((( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
c1) !=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( c2)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp957=* v; struct _tagged_string _temp958= Cyc_Absynpp_ckind2string(
c1); struct _tagged_string _temp959=*(( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp956= x1; if( _temp956 == 0){ _throw( Null_Exception);} _temp956->hd;}))->name;
struct _tagged_string _temp960= Cyc_Absynpp_ckind2string( c2); xprintf("redeclaration of tunion %.*s has a different kind (%.*s) for type parameter %.*s (%.*s)",
_temp957.last_plus_one - _temp957.curr, _temp957.curr, _temp958.last_plus_one -
_temp958.curr, _temp958.curr, _temp959.last_plus_one - _temp959.curr, _temp959.curr,
_temp960.last_plus_one - _temp960.curr, _temp960.curr);}));}}( void*)( ed->sc=(
void*)(( void*(*)( void* s1, struct Cyc_Core_Opt* fields1, void* s2, struct Cyc_Core_Opt*
fields2, struct Cyc_Position_Segment* loc, struct _tagged_string t, struct
_tagged_string* v)) Cyc_Tc_scope_redecl_okay)(( void*) ed->sc, ed->fields,( void*)
ed2->sc, ed2->fields, loc,( struct _tagged_string)({ char* _temp961=( char*)"tunion";
struct _tagged_string _temp962; _temp962.curr= _temp961; _temp962.base= _temp961;
_temp962.last_plus_one= _temp961 + 7; _temp962;}), v));{ struct _tuple7 _temp964=({
struct _tuple7 _temp963; _temp963.f1= ed->fields; _temp963.f2= ed2->fields;
_temp963;}); struct Cyc_Core_Opt* _temp974; struct Cyc_Core_Opt* _temp976;
struct Cyc_Core_Opt* _temp978; struct Cyc_Core_Opt* _temp980; struct Cyc_Core_Opt*
_temp982; struct Cyc_Core_Opt* _temp984; struct Cyc_Core_Opt* _temp986; struct
Cyc_Core_Opt* _temp988; _LL966: _LL977: _temp976= _temp964.f1; if( _temp976 == 0){
goto _LL975;} else{ goto _LL968;} _LL975: _temp974= _temp964.f2; if( _temp974 ==
0){ goto _LL967;} else{ goto _LL968;} _LL968: _LL981: _temp980= _temp964.f1;
goto _LL979; _LL979: _temp978= _temp964.f2; if( _temp978 == 0){ goto _LL969;}
else{ goto _LL970;} _LL970: _LL985: _temp984= _temp964.f1; if( _temp984 == 0){
goto _LL983;} else{ goto _LL972;} _LL983: _temp982= _temp964.f2; goto _LL971;
_LL972: _LL989: _temp988= _temp964.f1; goto _LL987; _LL987: _temp986= _temp964.f2;
goto _LL973; _LL967: return; _LL969:* _temp946= ed; goto _LL965; _LL971: ed->fields=
ed2->fields; ed->tvs= tvs2; return; _LL973: { struct Cyc_List_List* inst= 0;{
struct Cyc_List_List* tvs0= tvs; for( 0; tvs0 != 0; tvs0=({ struct Cyc_List_List*
_temp990= tvs0; if( _temp990 == 0){ _throw( Null_Exception);} _temp990->tl;}),
tvs2=({ struct Cyc_List_List* _temp991= tvs2; if( _temp991 == 0){ _throw(
Null_Exception);} _temp991->tl;})){ inst=({ struct Cyc_List_List* _temp992=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp992->hd=(
void*)({ struct _tuple4* _temp993=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp993->f1=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp997= tvs2; if( _temp997 == 0){ _throw( Null_Exception);} _temp997->hd;});
_temp993->f2=( void*)({ struct Cyc_Absyn_VarType_struct* _temp994=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp994[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp995; _temp995.tag= Cyc_Absyn_VarType_tag; _temp995.f1=( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp996= tvs0; if( _temp996 == 0){ _throw( Null_Exception);}
_temp996->hd;}); _temp995;}); _temp994;}); _temp993;}); _temp992->tl= inst;
_temp992;});}}{ struct Cyc_List_List* f1s=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp998= ed->fields; if( _temp998 == 0){ _throw( Null_Exception);} _temp998->v;});
struct Cyc_List_List* f2s=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp999= ed2->fields; if( _temp999 == 0){ _throw( Null_Exception);} _temp999->v;});
for( 0; f1s != 0? f2s != 0: 0; f1s=({ struct Cyc_List_List* _temp1000= f1s; if(
_temp1000 == 0){ _throw( Null_Exception);} _temp1000->tl;}), f2s=({ struct Cyc_List_List*
_temp1001= f2s; if( _temp1001 == 0){ _throw( Null_Exception);} _temp1001->tl;})){(*((
struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp1002= f1s; if(
_temp1002 == 0){ _throw( Null_Exception);} _temp1002->hd;}))->name).f1=( void*)({
struct Cyc_Absyn_Abs_n_struct* _temp1003=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp1003[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp1004; _temp1004.tag= Cyc_Absyn_Abs_n_tag; _temp1004.f1= te->ns; _temp1004;});
_temp1003;});{ struct Cyc_List_List* alltvs=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tvs,(( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp1005= f1s; if( _temp1005 == 0){ _throw(
Null_Exception);} _temp1005->hd;}))->tvs); Cyc_Tcutil_check_unique_tvars(((
struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp1006= f1s; if(
_temp1006 == 0){ _throw( Null_Exception);} _temp1006->hd;}))->loc, alltvs);{
struct Cyc_List_List* typs=(( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List*
_temp1007= f1s; if( _temp1007 == 0){ _throw( Null_Exception);} _temp1007->hd;}))->typs;
for( 0; typs != 0; typs=({ struct Cyc_List_List* _temp1008= typs; if( _temp1008
== 0){ _throw( Null_Exception);} _temp1008->tl;})){ void* t=(*(( struct _tuple5*)({
struct Cyc_List_List* _temp1009= typs; if( _temp1009 == 0){ _throw(
Null_Exception);} _temp1009->hd;}))).f2; Cyc_Tcutil_check_type((( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp1010= f1s; if( _temp1010 == 0){ _throw(
Null_Exception);} _temp1010->hd;}))->loc, te, alltvs,( void*) Cyc_Absyn_MemKind,
t);}} Cyc_Tc_field_redecl_okay(( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List*
_temp1011= f1s; if( _temp1011 == 0){ _throw( Null_Exception);} _temp1011->hd;}),(
struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp1012= f2s; if(
_temp1012 == 0){ _throw( Null_Exception);} _temp1012->hd;}), inst, te,( struct
_tagged_string)({ char* _temp1013=( char*)"tunion"; struct _tagged_string
_temp1014; _temp1014.curr= _temp1013; _temp1014.base= _temp1013; _temp1014.last_plus_one=
_temp1013 + 7; _temp1014;}), v);}} if( f1s != 0){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp1016=* v; struct _tagged_string _temp1017=*(*(( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp1015= f1s; if( _temp1015 == 0){ _throw(
Null_Exception);} _temp1015->hd;}))->name).f2; xprintf("redeclaration of tunion %.*s has extra field %.*s",
_temp1016.last_plus_one - _temp1016.curr, _temp1016.curr, _temp1017.last_plus_one
- _temp1017.curr, _temp1017.curr);}));} if( f2s != 0){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp1019=* v; struct _tagged_string _temp1020=*(*((
struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp1018= f2s; if(
_temp1018 == 0){ _throw( Null_Exception);} _temp1018->hd;}))->name).f2; xprintf("redeclaration of tunion %.*s is missing field %.*s",
_temp1019.last_plus_one - _temp1019.curr, _temp1019.curr, _temp1020.last_plus_one
- _temp1020.curr, _temp1020.curr);}));} return;}} _LL965:;} goto _LL940;}}
_LL940:;}{ struct Cyc_Absyn_Tuniondecl** edp=(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->tuniondecls,
v);* edp=({ struct Cyc_Absyn_Tuniondecl* _temp1021=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp1021->sc=( void*)(( void*)
ed->sc); _temp1021->name= ed->name; _temp1021->tvs= tvs; _temp1021->fields= 0;
_temp1021;});{ struct Cyc_List_List* prev_fields= 0;{ struct Cyc_List_List* fs=(
struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp1022= ed->fields; if(
_temp1022 == 0){ _throw( Null_Exception);} _temp1022->v;}); for( 0; fs != 0; fs=({
struct Cyc_List_List* _temp1023= fs; if( _temp1023 == 0){ _throw( Null_Exception);}
_temp1023->tl;})){ struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp1024= fs; if( _temp1024 == 0){ _throw( Null_Exception);}
_temp1024->hd;}); if((( int(*)( int(* compare)( struct _tagged_string*, struct
_tagged_string*), struct Cyc_List_List* l, struct _tagged_string* x)) Cyc_List_mem)(
Cyc_String_zstrptrcmp, prev_fields,(* f->name).f2)){ Cyc_Tcutil_terr( f->loc,({
struct _tagged_string _temp1025=*(* f->name).f2; xprintf("duplicate field name %.*s",
_temp1025.last_plus_one - _temp1025.curr, _temp1025.curr);}));} prev_fields=({
struct Cyc_List_List* _temp1026=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1026->hd=( void*)(* f->name).f2; _temp1026->tl=
prev_fields; _temp1026;});{ struct Cyc_List_List* tvs= f->tvs; for( 0; tvs != 0;
tvs=({ struct Cyc_List_List* _temp1027= tvs; if( _temp1027 == 0){ _throw(
Null_Exception);} _temp1027->tl;})){ struct Cyc_Absyn_Tvar* tv=( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp1028= tvs; if( _temp1028 == 0){ _throw(
Null_Exception);} _temp1028->hd;}); struct Cyc_Absyn_Conref* c=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( tv->kind); void*
_temp1029=( void*) c->v; void* _temp1037; _LL1031: if(( int) _temp1029 == Cyc_Absyn_No_constr){
goto _LL1032;} else{ goto _LL1033;} _LL1033: if(( unsigned int) _temp1029 > 1u?((
struct _tunion_struct*) _temp1029)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL1038:
_temp1037=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1029)->f1; if((
int) _temp1037 == Cyc_Absyn_MemKind){ goto _LL1034;} else{ goto _LL1035;}} else{
goto _LL1035;} _LL1035: goto _LL1036; _LL1032:( void*)( c->v=( void*)(( void*)({
struct Cyc_Absyn_Eq_constr_struct* _temp1039=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp1039[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp1040; _temp1040.tag= Cyc_Absyn_Eq_constr_tag;
_temp1040.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp1040;}); _temp1039;})));
goto _LL1030; _LL1034: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp1041=*(*
f->name).f2; xprintf("field %.*s abstracts type variable of kind M", _temp1041.last_plus_one
- _temp1041.curr, _temp1041.curr);})); goto _LL1030; _LL1036: goto _LL1030;
_LL1030:;}}{ struct Cyc_List_List* alltvs=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tvs, f->tvs); Cyc_Tcutil_check_unique_tvars(
loc, alltvs);{ struct Cyc_List_List* typs= f->typs; for( 0; typs != 0; typs=({
struct Cyc_List_List* _temp1042= typs; if( _temp1042 == 0){ _throw(
Null_Exception);} _temp1042->tl;})){ void* t=(*(( struct _tuple5*)({ struct Cyc_List_List*
_temp1043= typs; if( _temp1043 == 0){ _throw( Null_Exception);} _temp1043->hd;}))).f2;
Cyc_Tcutil_check_type( f->loc, te, alltvs,( void*) Cyc_Absyn_MemKind, t);}}(* f->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp1044=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp1044[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp1045; _temp1045.tag= Cyc_Absyn_Abs_n_tag; _temp1045.f1= te->ns; _temp1045;});
_temp1044;}); ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct _tuple6* data)) Cyc_Dict_insert)( ge->ordinaries,(*
f->name).f2,({ struct _tuple6* _temp1046=( struct _tuple6*) GC_malloc( sizeof(
struct _tuple6)); _temp1046->f1=( void*)({ struct Cyc_Tcenv_TunionRes_struct*
_temp1047=( struct Cyc_Tcenv_TunionRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_TunionRes_struct));
_temp1047[ 0]=({ struct Cyc_Tcenv_TunionRes_struct _temp1048; _temp1048.tag= Cyc_Tcenv_TunionRes_tag;
_temp1048.f1= ed; _temp1048.f2= f; _temp1048;}); _temp1047;}); _temp1046->f2= 1;
_temp1046;}));}}} ge->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Tuniondecl** data)) Cyc_Dict_insert)(
ge->tuniondecls, v,({ struct Cyc_Absyn_Tuniondecl** _temp1049=( struct Cyc_Absyn_Tuniondecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl*)); _temp1049[ 0]= ed; _temp1049;}));*
edp= ed;}}}} static void Cyc_Tc_tcXTuniondecl( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_XTuniondecl*
xd){ struct _tagged_string* v=(* xd->name).f2; struct Cyc_Core_Opt* xd2o;{
struct _handler_cons _temp1050; _push_handler(& _temp1050);{ struct
_xtunion_struct* _temp1051=( struct _xtunion_struct*) setjmp( _temp1050.handler);
if( ! _temp1051){ xd2o= Cyc_Tcenv_lookup_xtuniondecl( te, loc, xd->name);;
_pop_handler();} else{ struct _xtunion_struct* _temp1053= _temp1051; _LL1055:
if( _temp1053->tag == Cyc_Dict_Absent_tag){ goto _LL1056;} else{ goto _LL1057;}
_LL1057: goto _LL1058; _LL1056: Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp1059= Cyc_Absynpp_qvar2string( xd->name); xprintf("qualified xtunion declaration %.*s is not an existing xtunion",
_temp1059.last_plus_one - _temp1059.curr, _temp1059.curr);})); return; _LL1058:(
void) _throw( _temp1053); _LL1054:;}}}{ void* _temp1060=(* xd->name).f1; struct
Cyc_List_List* _temp1070; struct Cyc_List_List* _temp1072; struct Cyc_List_List*
_temp1074; _LL1062: if(( int) _temp1060 == Cyc_Absyn_Loc_n){ goto _LL1063;}
else{ goto _LL1064;} _LL1064: if(( unsigned int) _temp1060 > 1u?(( struct
_tunion_struct*) _temp1060)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL1071: _temp1070=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp1060)->f1; if(
_temp1070 == 0){ goto _LL1065;} else{ goto _LL1066;}} else{ goto _LL1066;}
_LL1066: if(( unsigned int) _temp1060 > 1u?(( struct _tunion_struct*) _temp1060)->tag
== Cyc_Absyn_Rel_n_tag: 0){ _LL1073: _temp1072=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Rel_n_struct*) _temp1060)->f1; goto _LL1067;} else{ goto _LL1068;}
_LL1068: if(( unsigned int) _temp1060 > 1u?(( struct _tunion_struct*) _temp1060)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL1075: _temp1074=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp1060)->f1; goto _LL1069;} else{ goto _LL1061;}
_LL1063:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp1076=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1076[ 0]=({ struct Cyc_Core_Impossible_struct _temp1077; _temp1077.tag= Cyc_Core_Impossible_tag;
_temp1077.f1=( struct _tagged_string)({ char* _temp1078=( char*)"tcXTuniondecl: Loc_n";
struct _tagged_string _temp1079; _temp1079.curr= _temp1078; _temp1079.base=
_temp1078; _temp1079.last_plus_one= _temp1078 + 21; _temp1079;}); _temp1077;});
_temp1076;})); _LL1065:(* xd->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp1080=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp1080[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp1081; _temp1081.tag= Cyc_Absyn_Abs_n_tag;
_temp1081.f1= te->ns; _temp1081;}); _temp1080;}); goto _LL1061; _LL1067:( void)
_throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct* _temp1082=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1082[ 0]=({ struct Cyc_Core_Impossible_struct _temp1083; _temp1083.tag= Cyc_Core_Impossible_tag;
_temp1083.f1=( struct _tagged_string)({ char* _temp1084=( char*)"tcXTuniondecl: Rel_n non-null";
struct _tagged_string _temp1085; _temp1085.curr= _temp1084; _temp1085.base=
_temp1084; _temp1085.last_plus_one= _temp1084 + 30; _temp1085;}); _temp1083;});
_temp1082;})); _LL1069:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp1086=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1086[ 0]=({ struct Cyc_Core_Impossible_struct _temp1087; _temp1087.tag= Cyc_Core_Impossible_tag;
_temp1087.f1=( struct _tagged_string)({ char* _temp1088=( char*)"tcXTuniondecl: Abs_n";
struct _tagged_string _temp1089; _temp1089.curr= _temp1088; _temp1089.base=
_temp1088; _temp1089.last_plus_one= _temp1088 + 21; _temp1089;}); _temp1087;});
_temp1086;})); _LL1061:;} if( xd2o == 0){ struct Cyc_Absyn_XTuniondecl** x=(
struct Cyc_Absyn_XTuniondecl**)({ struct _tuple8* _temp1090=( struct _tuple8*)
GC_malloc( sizeof( struct _tuple8)); _temp1090->f1=({ struct Cyc_Absyn_XTuniondecl*
_temp1091=( struct Cyc_Absyn_XTuniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_XTuniondecl));
_temp1091->sc=( void*)(( void*) xd->sc); _temp1091->name= xd->name; _temp1091->fields=
0; _temp1091;}); _temp1090;}); xd2o=({ struct Cyc_Core_Opt* _temp1092=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1092->v=( void*) x;
_temp1092;}); ge->xtuniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_XTuniondecl** data)) Cyc_Dict_insert)(
ge->xtuniondecls, v, x);} else{ struct Cyc_Absyn_XTuniondecl* x=*(( struct Cyc_Absyn_XTuniondecl**)({
struct Cyc_Core_Opt* _temp1093= xd2o; if( _temp1093 == 0){ _throw(
Null_Exception);} _temp1093->v;}));( void*)( x->sc=( void*)(( void*(*)( void* s1,
struct Cyc_Core_Opt* fields1, void* s2, struct Cyc_Core_Opt* fields2, struct Cyc_Position_Segment*
loc, struct _tagged_string t, struct _tagged_string* v)) Cyc_Tc_scope_redecl_okay)((
void*) xd->sc,({ struct Cyc_Core_Opt* _temp1094=( struct Cyc_Core_Opt*)
GC_malloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp1094->v=( void*) 0;
_temp1094;}),( void*) x->sc,({ struct Cyc_Core_Opt* _temp1095=( struct Cyc_Core_Opt*)
GC_malloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp1095->v=( void*) 0;
_temp1095;}), loc,( struct _tagged_string)({ char* _temp1096=( char*)"xtunion";
struct _tagged_string _temp1097; _temp1097.curr= _temp1096; _temp1097.base=
_temp1096; _temp1097.last_plus_one= _temp1096 + 8; _temp1097;}), v));}{ struct
Cyc_Absyn_XTuniondecl* xd_result=*(( struct Cyc_Absyn_XTuniondecl**)({ struct
Cyc_Core_Opt* _temp1098= xd2o; if( _temp1098 == 0){ _throw( Null_Exception);}
_temp1098->v;})); struct Cyc_List_List* fs= xd->fields; for( 0; fs != 0; fs=({
struct Cyc_List_List* _temp1099= fs; if( _temp1099 == 0){ _throw( Null_Exception);}
_temp1099->tl;})){ struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp1100= fs; if( _temp1100 == 0){ _throw( Null_Exception);}
_temp1100->hd;});{ void* _temp1101=(* f->name).f1; struct Cyc_List_List*
_temp1111; struct Cyc_List_List* _temp1113; struct Cyc_List_List* _temp1115;
_LL1103: if(( unsigned int) _temp1101 > 1u?(( struct _tunion_struct*) _temp1101)->tag
== Cyc_Absyn_Rel_n_tag: 0){ _LL1112: _temp1111=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Rel_n_struct*) _temp1101)->f1; if( _temp1111 == 0){ goto _LL1104;}
else{ goto _LL1105;}} else{ goto _LL1105;} _LL1105: if(( unsigned int) _temp1101
> 1u?(( struct _tunion_struct*) _temp1101)->tag == Cyc_Absyn_Rel_n_tag: 0){
_LL1114: _temp1113=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp1101)->f1; goto _LL1106;} else{ goto _LL1107;} _LL1107: if(( unsigned int)
_temp1101 > 1u?(( struct _tunion_struct*) _temp1101)->tag == Cyc_Absyn_Abs_n_tag:
0){ _LL1116: _temp1115=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp1101)->f1; goto _LL1108;} else{ goto _LL1109;} _LL1109: if(( int) _temp1101
== Cyc_Absyn_Loc_n){ goto _LL1110;} else{ goto _LL1102;} _LL1104:(* f->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp1117=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp1117[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp1118; _temp1118.tag= Cyc_Absyn_Abs_n_tag; _temp1118.f1= te->ns; _temp1118;});
_temp1117;}); goto _LL1102; _LL1106: goto _LL1108; _LL1108: Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp1119=( char*)"qualified declarations are not allowed\n";
struct _tagged_string _temp1120; _temp1120.curr= _temp1119; _temp1120.base=
_temp1119; _temp1120.last_plus_one= _temp1119 + 40; _temp1120;})); goto _LL1102;
_LL1110:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp1121=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1121[ 0]=({ struct Cyc_Core_Impossible_struct _temp1122; _temp1122.tag= Cyc_Core_Impossible_tag;
_temp1122.f1=( struct _tagged_string)({ char* _temp1123=( char*)"tcXtuniondecl: Loc_n";
struct _tagged_string _temp1124; _temp1124.curr= _temp1123; _temp1124.base=
_temp1123; _temp1124.last_plus_one= _temp1123 + 21; _temp1124;}); _temp1122;});
_temp1121;})); goto _LL1102; _LL1102:;}{ struct Cyc_List_List* tvs= f->tvs; for(
0; tvs != 0; tvs=({ struct Cyc_List_List* _temp1125= tvs; if( _temp1125 == 0){
_throw( Null_Exception);} _temp1125->tl;})){ struct Cyc_Absyn_Tvar* tv=( struct
Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp1126= tvs; if( _temp1126 == 0){
_throw( Null_Exception);} _temp1126->hd;}); struct Cyc_Absyn_Conref* c=(( struct
Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
tv->kind); void* _temp1127=( void*) c->v; void* _temp1135; _LL1129: if(( int)
_temp1127 == Cyc_Absyn_No_constr){ goto _LL1130;} else{ goto _LL1131;} _LL1131:
if(( unsigned int) _temp1127 > 1u?(( struct _tunion_struct*) _temp1127)->tag ==
Cyc_Absyn_Eq_constr_tag: 0){ _LL1136: _temp1135=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp1127)->f1; if(( int) _temp1135 == Cyc_Absyn_MemKind){ goto _LL1132;} else{
goto _LL1133;}} else{ goto _LL1133;} _LL1133: goto _LL1134; _LL1130:( void*)( c->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp1137=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp1137[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp1138; _temp1138.tag= Cyc_Absyn_Eq_constr_tag;
_temp1138.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp1138;}); _temp1137;})));
goto _LL1128; _LL1132: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp1139=*(*
f->name).f2; xprintf("field %.*s abstracts type variable of kind M", _temp1139.last_plus_one
- _temp1139.curr, _temp1139.curr);})); goto _LL1128; _LL1134: goto _LL1128;
_LL1128:;}}{ struct Cyc_List_List* tvs= f->tvs; Cyc_Tcutil_check_unique_tvars(
loc, tvs);{ struct Cyc_List_List* typs= f->typs; for( 0; typs != 0; typs=({
struct Cyc_List_List* _temp1140= typs; if( _temp1140 == 0){ _throw(
Null_Exception);} _temp1140->tl;})){ void* t=(*(( struct _tuple5*)({ struct Cyc_List_List*
_temp1141= typs; if( _temp1141 == 0){ _throw( Null_Exception);} _temp1141->hd;}))).f2;
Cyc_Tcutil_check_type( f->loc, te, tvs,( void*) Cyc_Absyn_MemKind, t);}}{ int
dup= 0;{ struct Cyc_List_List* prev_fs= xd_result->fields; for( 0; prev_fs != 0;
prev_fs=({ struct Cyc_List_List* _temp1142= prev_fs; if( _temp1142 == 0){ _throw(
Null_Exception);} _temp1142->tl;})){ struct Cyc_Absyn_Tunionfield* prev=( struct
Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp1143= prev_fs; if(
_temp1143 == 0){ _throw( Null_Exception);} _temp1143->hd;}); if( Cyc_Absyn_qvar_cmp(
f->name, prev->name) == 0){ Cyc_Tc_field_redecl_okay( prev, f, 0, te,( struct
_tagged_string)({ char* _temp1144=( char*)"xtunion"; struct _tagged_string
_temp1145; _temp1145.curr= _temp1144; _temp1145.base= _temp1144; _temp1145.last_plus_one=
_temp1144 + 8; _temp1145;}), v); dup= 1; break;}}} if( dup){ continue;}
xd_result->fields=({ struct Cyc_List_List* _temp1146=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1146->hd=( void*) f; _temp1146->tl=
xd_result->fields; _temp1146;});{ struct Cyc_Tcenv_Genv* ge_f; struct Cyc_List_List*
ns_f;{ void* _temp1147=(* f->name).f1; struct Cyc_List_List* _temp1155; struct
Cyc_List_List* _temp1157; _LL1149: if(( unsigned int) _temp1147 > 1u?(( struct
_tunion_struct*) _temp1147)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL1156: _temp1155=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp1147)->f1; goto
_LL1150;} else{ goto _LL1151;} _LL1151: if(( int) _temp1147 == Cyc_Absyn_Loc_n){
goto _LL1152;} else{ goto _LL1153;} _LL1153: if(( unsigned int) _temp1147 > 1u?((
struct _tunion_struct*) _temp1147)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL1158:
_temp1157=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp1147)->f1;
goto _LL1154;} else{ goto _LL1148;} _LL1150: ns_f= _temp1155; goto _LL1148;
_LL1152: goto _LL1154; _LL1154:( void) _throw(( struct _xtunion_struct*)({
struct Cyc_Core_Impossible_struct* _temp1159=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp1159[ 0]=({ struct
Cyc_Core_Impossible_struct _temp1160; _temp1160.tag= Cyc_Core_Impossible_tag;
_temp1160.f1=( struct _tagged_string)({ char* _temp1161=( char*)"tcXTuniondecl: Rel_n or Loc_n";
struct _tagged_string _temp1162; _temp1162.curr= _temp1161; _temp1162.base=
_temp1161; _temp1162.last_plus_one= _temp1161 + 30; _temp1162;}); _temp1160;});
_temp1159;})); goto _LL1148; _LL1148:;}{ struct _handler_cons _temp1163;
_push_handler(& _temp1163);{ struct _xtunion_struct* _temp1164=( struct
_xtunion_struct*) setjmp( _temp1163.handler); if( ! _temp1164){ ge_f=(( struct
Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)(
te->ae, ns_f);; _pop_handler();} else{ struct _xtunion_struct* _temp1166=
_temp1164; _LL1168: if( _temp1166->tag == Cyc_Dict_Absent_tag){ goto _LL1169;}
else{ goto _LL1170;} _LL1170: goto _LL1171; _LL1169: Cyc_Tcutil_terr( f->loc,({
struct _tagged_string _temp1172= Cyc_Absynpp_qvar2string( f->name); xprintf("bad namespace for xtunion field %.*s",
_temp1172.last_plus_one - _temp1172.curr, _temp1172.curr);})); return; _LL1171:(
void) _throw( _temp1166); _LL1167:;}}} ge_f->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct _tuple6* data)) Cyc_Dict_insert)(
ge_f->ordinaries,(* f->name).f2,({ struct _tuple6* _temp1173=( struct _tuple6*)
GC_malloc( sizeof( struct _tuple6)); _temp1173->f1=( void*)({ struct Cyc_Tcenv_XTunionRes_struct*
_temp1174=( struct Cyc_Tcenv_XTunionRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_XTunionRes_struct));
_temp1174[ 0]=({ struct Cyc_Tcenv_XTunionRes_struct _temp1175; _temp1175.tag=
Cyc_Tcenv_XTunionRes_tag; _temp1175.f1= xd_result; _temp1175.f2= f; _temp1175;});
_temp1174;}); _temp1173->f2= 1; _temp1173;}));}}}}}} static void Cyc_Tc_tcEnumdecl(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Enumdecl* ed){ struct _tagged_string* v=(* ed->name).f2;{
void* _temp1176=(* ed->name).f1; struct Cyc_List_List* _temp1184; struct Cyc_List_List*
_temp1186; _LL1178: if(( unsigned int) _temp1176 > 1u?(( struct _tunion_struct*)
_temp1176)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL1185: _temp1184=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp1176)->f1; if( _temp1184 == 0){ goto
_LL1179;} else{ goto _LL1180;}} else{ goto _LL1180;} _LL1180: if(( unsigned int)
_temp1176 > 1u?(( struct _tunion_struct*) _temp1176)->tag == Cyc_Absyn_Abs_n_tag:
0){ _LL1187: _temp1186=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp1176)->f1; if( _temp1186 == 0){ goto _LL1181;} else{ goto _LL1182;}} else{
goto _LL1182;} _LL1182: goto _LL1183; _LL1179: goto _LL1177; _LL1181: goto
_LL1177; _LL1183: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp1188=( char*)"qualified declarations are not implemented"; struct
_tagged_string _temp1189; _temp1189.curr= _temp1188; _temp1189.base= _temp1188;
_temp1189.last_plus_one= _temp1188 + 43; _temp1189;})); return; _LL1177:;}(* ed->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp1190=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp1190[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp1191; _temp1191.tag= Cyc_Absyn_Abs_n_tag; _temp1191.f1= te->ns; _temp1191;});
_temp1190;});{ struct Cyc_Core_Opt* _temp1192=(( struct Cyc_Core_Opt*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup_opt)( ge->enumdecls,
v); struct Cyc_Core_Opt _temp1198; struct Cyc_Absyn_Enumdecl** _temp1199;
_LL1194: if( _temp1192 == 0){ goto _LL1195;} else{ goto _LL1196;} _LL1196: if(
_temp1192 == 0){ goto _LL1193;} else{ _temp1198=* _temp1192; _LL1200: _temp1199=(
struct Cyc_Absyn_Enumdecl**) _temp1198.v; goto _LL1197;} _LL1195: ge->enumdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Enumdecl** data)) Cyc_Dict_insert)( ge->enumdecls, v,({ struct
Cyc_Absyn_Enumdecl** _temp1201=( struct Cyc_Absyn_Enumdecl**) GC_malloc( sizeof(
struct Cyc_Absyn_Enumdecl*)); _temp1201[ 0]= ed; _temp1201;})); goto _LL1193;
_LL1197: { struct Cyc_Absyn_Enumdecl* ed2=* _temp1199;( void*)( ed->sc=( void*)((
void*(*)( void* s1, struct Cyc_Core_Opt* fields1, void* s2, struct Cyc_Core_Opt*
fields2, struct Cyc_Position_Segment* loc, struct _tagged_string t, struct
_tagged_string* v)) Cyc_Tc_scope_redecl_okay)(( void*) ed->sc, 0,( void*) ed2->sc,
0, loc,( struct _tagged_string)({ char* _temp1202=( char*)"enum"; struct
_tagged_string _temp1203; _temp1203.curr= _temp1202; _temp1203.base= _temp1202;
_temp1203.last_plus_one= _temp1202 + 5; _temp1203;}), v));{ struct Cyc_List_List*
f1s= ed->fields; struct Cyc_List_List* f2s= ed2->fields; unsigned int tag_count=
0; for( 0; f1s != 0? f2s != 0: 0; f1s=({ struct Cyc_List_List* _temp1204= f1s;
if( _temp1204 == 0){ _throw( Null_Exception);} _temp1204->tl;}), f2s=({ struct
Cyc_List_List* _temp1205= f2s; if( _temp1205 == 0){ _throw( Null_Exception);}
_temp1205->tl;})){ struct Cyc_Absyn_Enumfield _temp1209; struct Cyc_Position_Segment*
_temp1210; struct Cyc_Absyn_Exp* _temp1212; struct Cyc_Absyn_Exp** _temp1214;
struct _tuple0* _temp1215; struct Cyc_Absyn_Enumfield* _temp1207=( struct Cyc_Absyn_Enumfield*)({
struct Cyc_List_List* _temp1206= f1s; if( _temp1206 == 0){ _throw(
Null_Exception);} _temp1206->hd;}); _temp1209=* _temp1207; _LL1216: _temp1215=(
struct _tuple0*) _temp1209.name; goto _LL1213; _LL1213: _temp1212=( struct Cyc_Absyn_Exp*)
_temp1209.tag; _temp1214=&(* _temp1207).tag; goto _LL1211; _LL1211: _temp1210=(
struct Cyc_Position_Segment*) _temp1209.loc; goto _LL1208; _LL1208: { struct Cyc_Absyn_Enumfield
_temp1220; struct Cyc_Position_Segment* _temp1221; struct Cyc_Absyn_Exp*
_temp1223; struct _tuple0* _temp1225; struct Cyc_Absyn_Enumfield* _temp1218=(
struct Cyc_Absyn_Enumfield*)({ struct Cyc_List_List* _temp1217= f2s; if(
_temp1217 == 0){ _throw( Null_Exception);} _temp1217->hd;}); _temp1220=*
_temp1218; _LL1226: _temp1225=( struct _tuple0*) _temp1220.name; goto _LL1224;
_LL1224: _temp1223=( struct Cyc_Absyn_Exp*) _temp1220.tag; goto _LL1222; _LL1222:
_temp1221=( struct Cyc_Position_Segment*) _temp1220.loc; goto _LL1219; _LL1219:(*
_temp1215).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct* _temp1227=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp1227[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp1228; _temp1228.tag= Cyc_Absyn_Abs_n_tag; _temp1228.f1= te->ns; _temp1228;});
_temp1227;}); if( Cyc_String_zstrptrcmp((* _temp1215).f2,(* _temp1225).f2) != 0){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp1229=* v; struct
_tagged_string _temp1230=*(* _temp1215).f2; struct _tagged_string _temp1231=*(*
_temp1225).f2; xprintf("redeclaration of enum %.*s: field name mismatch %.*s != %.*s",
_temp1229.last_plus_one - _temp1229.curr, _temp1229.curr, _temp1230.last_plus_one
- _temp1230.curr, _temp1230.curr, _temp1231.last_plus_one - _temp1231.curr,
_temp1231.curr);}));} if(* _temp1214 == 0){* _temp1214=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp( tag_count, _temp1210);} else{ if( ! Cyc_Tcutil_is_const_exp(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1232=* _temp1214; if(
_temp1232 == 0){ _throw( Null_Exception);} _temp1232;}))){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp1233=* v; struct _tagged_string _temp1234=*(*
_temp1215).f2; xprintf("redeclaration of enum %.*s, field %.*s: expression is not constant",
_temp1233.last_plus_one - _temp1233.curr, _temp1233.curr, _temp1234.last_plus_one
- _temp1234.curr, _temp1234.curr);}));}}{ unsigned int t1= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1235=* _temp1214; if(
_temp1235 == 0){ _throw( Null_Exception);} _temp1235;})); tag_count= t1 + 1; if(
t1 != Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1236= _temp1223; if( _temp1236 == 0){ _throw( Null_Exception);} _temp1236;}))){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp1237=* v; struct
_tagged_string _temp1238=*(* _temp1215).f2; xprintf("redeclaration of enum %.*s, field %.*s, value mismatch",
_temp1237.last_plus_one - _temp1237.curr, _temp1237.curr, _temp1238.last_plus_one
- _temp1238.curr, _temp1238.curr);}));}}}} return;}} _LL1193:;}{ struct Cyc_List_List*
prev_fields= 0; unsigned int tag_count= 0;{ struct Cyc_List_List* fs= ed->fields;
for( 0; fs != 0; fs=({ struct Cyc_List_List* _temp1239= fs; if( _temp1239 == 0){
_throw( Null_Exception);} _temp1239->tl;})){ struct Cyc_Absyn_Enumfield* f=(
struct Cyc_Absyn_Enumfield*)({ struct Cyc_List_List* _temp1240= fs; if(
_temp1240 == 0){ _throw( Null_Exception);} _temp1240->hd;}); if((( int(*)( int(*
compare)( struct _tagged_string*, struct _tagged_string*), struct Cyc_List_List*
l, struct _tagged_string* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp, prev_fields,(*
f->name).f2)){ Cyc_Tcutil_terr( f->loc,({ struct _tagged_string _temp1241=*(* f->name).f2;
xprintf("duplicate field name %.*s", _temp1241.last_plus_one - _temp1241.curr,
_temp1241.curr);}));} prev_fields=({ struct Cyc_List_List* _temp1242=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1242->hd=( void*)(*
f->name).f2; _temp1242->tl= prev_fields; _temp1242;}); if( f->tag == 0){ f->tag=(
struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp( tag_count, f->loc);} else{ if( ! Cyc_Tcutil_is_const_exp(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1243= f->tag; if(
_temp1243 == 0){ _throw( Null_Exception);} _temp1243;}))){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp1244=* v; struct _tagged_string _temp1245=*(* f->name).f2;
xprintf("enum %.*s, field %.*s: expression is not constant", _temp1244.last_plus_one
- _temp1244.curr, _temp1244.curr, _temp1245.last_plus_one - _temp1245.curr,
_temp1245.curr);}));}}{ unsigned int t1= Cyc_Evexp_eval_const_uint_exp(( struct
Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1246= f->tag; if( _temp1246 == 0){
_throw( Null_Exception);} _temp1246;})); tag_count= t1 + 1;(* f->name).f1=( void*)({
struct Cyc_Absyn_Abs_n_struct* _temp1247=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp1247[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp1248; _temp1248.tag= Cyc_Absyn_Abs_n_tag; _temp1248.f1= te->ns; _temp1248;});
_temp1247;}); ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct _tuple6* data)) Cyc_Dict_insert)( ge->ordinaries,(*
f->name).f2,({ struct _tuple6* _temp1249=( struct _tuple6*) GC_malloc( sizeof(
struct _tuple6)); _temp1249->f1=( void*)({ struct Cyc_Tcenv_EnumRes_struct*
_temp1250=( struct Cyc_Tcenv_EnumRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_EnumRes_struct));
_temp1250[ 0]=({ struct Cyc_Tcenv_EnumRes_struct _temp1251; _temp1251.tag= Cyc_Tcenv_EnumRes_tag;
_temp1251.f1= ed; _temp1251.f2= f; _temp1251;}); _temp1250;}); _temp1249->f2= 1;
_temp1249;}));}}} ge->enumdecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Enumdecl** data)) Cyc_Dict_insert)(
ge->enumdecls, v,({ struct Cyc_Absyn_Enumdecl** _temp1252=( struct Cyc_Absyn_Enumdecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl*)); _temp1252[ 0]= ed; _temp1252;}));}}
static int Cyc_Tc_okay_externC( struct Cyc_Position_Segment* loc, void* sc){
void* _temp1253= sc; _LL1255: if(( int) _temp1253 == Cyc_Absyn_Static){ goto
_LL1256;} else{ goto _LL1257;} _LL1257: if(( int) _temp1253 == Cyc_Absyn_Abstract){
goto _LL1258;} else{ goto _LL1259;} _LL1259: if(( int) _temp1253 == Cyc_Absyn_Public){
goto _LL1260;} else{ goto _LL1261;} _LL1261: if(( int) _temp1253 == Cyc_Absyn_Extern){
goto _LL1262;} else{ goto _LL1263;} _LL1263: if(( int) _temp1253 == Cyc_Absyn_ExternC){
goto _LL1264;} else{ goto _LL1254;} _LL1256: Cyc_Tcutil_warn( loc,( struct
_tagged_string)({ char* _temp1265=( char*)"static declaration nested within extern \"C\"";
struct _tagged_string _temp1266; _temp1266.curr= _temp1265; _temp1266.base=
_temp1265; _temp1266.last_plus_one= _temp1265 + 44; _temp1266;})); return 0;
_LL1258: Cyc_Tcutil_warn( loc,( struct _tagged_string)({ char* _temp1267=( char*)"abstract declaration nested within extern \"C\"";
struct _tagged_string _temp1268; _temp1268.curr= _temp1267; _temp1268.base=
_temp1267; _temp1268.last_plus_one= _temp1267 + 46; _temp1268;})); return 0;
_LL1260: return 1; _LL1262: return 1; _LL1264: Cyc_Tcutil_warn( loc,( struct
_tagged_string)({ char* _temp1269=( char*)"nested extern \"C\" declaration";
struct _tagged_string _temp1270; _temp1270.curr= _temp1269; _temp1270.base=
_temp1269; _temp1270.last_plus_one= _temp1269 + 30; _temp1270;})); return 1;
_LL1254:;} static void Cyc_Tc_tc_decls( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
ds0, int in_externC){ struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
te->ns); struct Cyc_List_List* ds= ds0; for( 0; ds != 0; ds=({ struct Cyc_List_List*
_temp1271= ds; if( _temp1271 == 0){ _throw( Null_Exception);} _temp1271->tl;})){
struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)({ struct Cyc_List_List*
_temp1272= ds; if( _temp1272 == 0){ _throw( Null_Exception);} _temp1272->hd;});
struct Cyc_Position_Segment* loc= d->loc; void* _temp1273=( void*) d->r; int
_temp1299; struct Cyc_Absyn_Exp* _temp1301; struct Cyc_Core_Opt* _temp1303;
struct Cyc_Core_Opt* _temp1305; struct Cyc_Absyn_Pat* _temp1307; struct Cyc_Absyn_Vardecl*
_temp1309; struct Cyc_Absyn_Fndecl* _temp1311; struct Cyc_Absyn_Typedefdecl*
_temp1313; struct Cyc_Absyn_Structdecl* _temp1315; struct Cyc_Absyn_Uniondecl*
_temp1317; struct Cyc_Absyn_Tuniondecl* _temp1319; struct Cyc_Absyn_XTuniondecl*
_temp1321; struct Cyc_Absyn_Enumdecl* _temp1323; struct Cyc_List_List* _temp1325;
struct _tagged_string* _temp1327; struct Cyc_List_List* _temp1329; struct
_tuple0* _temp1331; struct _tuple0 _temp1333; struct _tagged_string* _temp1334;
void* _temp1336; struct Cyc_List_List* _temp1338; _LL1275: if((( struct
_tunion_struct*) _temp1273)->tag == Cyc_Absyn_Let_d_tag){ _LL1308: _temp1307=(
struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*) _temp1273)->f1; goto
_LL1306; _LL1306: _temp1305=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1273)->f2; goto _LL1304; _LL1304: _temp1303=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Let_d_struct*) _temp1273)->f3; goto _LL1302; _LL1302: _temp1301=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*) _temp1273)->f4; goto
_LL1300; _LL1300: _temp1299=( int)(( struct Cyc_Absyn_Let_d_struct*) _temp1273)->f5;
goto _LL1276;} else{ goto _LL1277;} _LL1277: if((( struct _tunion_struct*)
_temp1273)->tag == Cyc_Absyn_Var_d_tag){ _LL1310: _temp1309=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_d_struct*) _temp1273)->f1; goto _LL1278;} else{ goto
_LL1279;} _LL1279: if((( struct _tunion_struct*) _temp1273)->tag == Cyc_Absyn_Fn_d_tag){
_LL1312: _temp1311=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*)
_temp1273)->f1; goto _LL1280;} else{ goto _LL1281;} _LL1281: if((( struct
_tunion_struct*) _temp1273)->tag == Cyc_Absyn_Typedef_d_tag){ _LL1314: _temp1313=(
struct Cyc_Absyn_Typedefdecl*)(( struct Cyc_Absyn_Typedef_d_struct*) _temp1273)->f1;
goto _LL1282;} else{ goto _LL1283;} _LL1283: if((( struct _tunion_struct*)
_temp1273)->tag == Cyc_Absyn_Struct_d_tag){ _LL1316: _temp1315=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_d_struct*) _temp1273)->f1; goto _LL1284;} else{ goto
_LL1285;} _LL1285: if((( struct _tunion_struct*) _temp1273)->tag == Cyc_Absyn_Union_d_tag){
_LL1318: _temp1317=( struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*)
_temp1273)->f1; goto _LL1286;} else{ goto _LL1287;} _LL1287: if((( struct
_tunion_struct*) _temp1273)->tag == Cyc_Absyn_Tunion_d_tag){ _LL1320: _temp1319=(
struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_d_struct*) _temp1273)->f1;
goto _LL1288;} else{ goto _LL1289;} _LL1289: if((( struct _tunion_struct*)
_temp1273)->tag == Cyc_Absyn_XTunion_d_tag){ _LL1322: _temp1321=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_d_struct*) _temp1273)->f1; goto _LL1290;} else{ goto
_LL1291;} _LL1291: if((( struct _tunion_struct*) _temp1273)->tag == Cyc_Absyn_Enum_d_tag){
_LL1324: _temp1323=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*)
_temp1273)->f1; goto _LL1292;} else{ goto _LL1293;} _LL1293: if((( struct
_tunion_struct*) _temp1273)->tag == Cyc_Absyn_Namespace_d_tag){ _LL1328:
_temp1327=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1273)->f1; goto _LL1326; _LL1326: _temp1325=( struct Cyc_List_List*)((
struct Cyc_Absyn_Namespace_d_struct*) _temp1273)->f2; goto _LL1294;} else{ goto
_LL1295;} _LL1295: if((( struct _tunion_struct*) _temp1273)->tag == Cyc_Absyn_Using_d_tag){
_LL1332: _temp1331=( struct _tuple0*)(( struct Cyc_Absyn_Using_d_struct*)
_temp1273)->f1; _temp1333=* _temp1331; _LL1337: _temp1336= _temp1333.f1; goto
_LL1335; _LL1335: _temp1334= _temp1333.f2; goto _LL1330; _LL1330: _temp1329=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*) _temp1273)->f2; goto
_LL1296;} else{ goto _LL1297;} _LL1297: if((( struct _tunion_struct*) _temp1273)->tag
== Cyc_Absyn_ExternC_d_tag){ _LL1339: _temp1338=( struct Cyc_List_List*)((
struct Cyc_Absyn_ExternC_d_struct*) _temp1273)->f1; goto _LL1298;} else{ goto
_LL1274;} _LL1276: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp1340=( char*)"top level let-declarations are not implemented"; struct
_tagged_string _temp1341; _temp1341.curr= _temp1340; _temp1341.base= _temp1340;
_temp1341.last_plus_one= _temp1340 + 47; _temp1341;})); goto _LL1274; _LL1278:
if( in_externC? Cyc_Tc_okay_externC( d->loc,( void*) _temp1309->sc): 0){( void*)(
_temp1309->sc=( void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcVardecl( te, ge,
loc, _temp1309); goto _LL1274; _LL1280: if( in_externC? Cyc_Tc_okay_externC( d->loc,(
void*) _temp1311->sc): 0){( void*)( _temp1311->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcFndecl( te, ge, loc, _temp1311); goto _LL1274; _LL1282: Cyc_Tc_tcTypedefdecl(
te, ge, loc, _temp1313); goto _LL1274; _LL1284: if( in_externC? Cyc_Tc_okay_externC(
d->loc,( void*) _temp1315->sc): 0){( void*)( _temp1315->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcStructdecl( te, ge, loc, _temp1315); goto _LL1274; _LL1286: if(
in_externC? Cyc_Tc_okay_externC( d->loc,( void*) _temp1317->sc): 0){( void*)(
_temp1317->sc=( void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcUniondecl( te, ge,
loc, _temp1317); goto _LL1274; _LL1288: if( in_externC? Cyc_Tc_okay_externC( d->loc,(
void*) _temp1319->sc): 0){( void*)( _temp1319->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcTuniondecl( te, ge, loc, _temp1319); goto _LL1274; _LL1290: if(
in_externC? Cyc_Tc_okay_externC( d->loc,( void*) _temp1321->sc): 0){( void*)(
_temp1321->sc=( void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcXTuniondecl( te,
ge, loc, _temp1321); goto _LL1274; _LL1292: if( in_externC? Cyc_Tc_okay_externC(
d->loc,( void*) _temp1323->sc): 0){( void*)( _temp1323->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcEnumdecl( te, ge, loc, _temp1323); goto _LL1274; _LL1294: { struct Cyc_List_List*
ns= te->ns; struct Cyc_List_List* ns2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( ns,({ struct Cyc_List_List*
_temp1342=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1342->hd=( void*) _temp1327; _temp1342->tl= 0; _temp1342;})); if( !(( int(*)(
struct Cyc_Set_Set* s, struct _tagged_string* elt)) Cyc_Set_member)( ge->namespaces,
_temp1327)){ ge->namespaces=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s,
struct _tagged_string* elt)) Cyc_Set_insert)( ge->namespaces, _temp1327); te->ae=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key,
struct Cyc_Tcenv_Genv* data)) Cyc_Dict_insert)( te->ae, ns2, Cyc_Tcenv_empty_genv());}
te->ns= ns2; Cyc_Tc_tc_decls( te, _temp1325, in_externC); te->ns= ns; goto
_LL1274;} _LL1296: { struct _tagged_string* first; struct Cyc_List_List* rest;{
void* _temp1343= _temp1336; struct Cyc_List_List* _temp1355; struct Cyc_List_List*
_temp1357; struct Cyc_List_List* _temp1359; struct Cyc_List_List _temp1361;
struct Cyc_List_List* _temp1362; struct _tagged_string* _temp1364; struct Cyc_List_List*
_temp1366; struct Cyc_List_List _temp1368; struct Cyc_List_List* _temp1369;
struct _tagged_string* _temp1371; _LL1345: if(( int) _temp1343 == Cyc_Absyn_Loc_n){
goto _LL1346;} else{ goto _LL1347;} _LL1347: if(( unsigned int) _temp1343 > 1u?((
struct _tunion_struct*) _temp1343)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL1356:
_temp1355=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp1343)->f1;
if( _temp1355 == 0){ goto _LL1348;} else{ goto _LL1349;}} else{ goto _LL1349;}
_LL1349: if(( unsigned int) _temp1343 > 1u?(( struct _tunion_struct*) _temp1343)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL1358: _temp1357=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp1343)->f1; if( _temp1357 == 0){ goto _LL1350;}
else{ goto _LL1351;}} else{ goto _LL1351;} _LL1351: if(( unsigned int) _temp1343
> 1u?(( struct _tunion_struct*) _temp1343)->tag == Cyc_Absyn_Rel_n_tag: 0){
_LL1360: _temp1359=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp1343)->f1; if( _temp1359 == 0){ goto _LL1353;} else{ _temp1361=* _temp1359;
_LL1365: _temp1364=( struct _tagged_string*) _temp1361.hd; goto _LL1363; _LL1363:
_temp1362=( struct Cyc_List_List*) _temp1361.tl; goto _LL1352;}} else{ goto
_LL1353;} _LL1353: if(( unsigned int) _temp1343 > 1u?(( struct _tunion_struct*)
_temp1343)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL1367: _temp1366=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp1343)->f1; if( _temp1366 == 0){ goto
_LL1344;} else{ _temp1368=* _temp1366; _LL1372: _temp1371=( struct
_tagged_string*) _temp1368.hd; goto _LL1370; _LL1370: _temp1369=( struct Cyc_List_List*)
_temp1368.tl; goto _LL1354;}} else{ goto _LL1344;} _LL1346: goto _LL1348;
_LL1348: goto _LL1350; _LL1350: first= _temp1334; rest= 0; goto _LL1344; _LL1352:
_temp1371= _temp1364; _temp1369= _temp1362; goto _LL1354; _LL1354: first=
_temp1371; rest=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( _temp1369,({ struct Cyc_List_List* _temp1373=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1373->hd=( void*) _temp1334;
_temp1373->tl= 0; _temp1373;})); goto _LL1344; _LL1344:;}{ struct Cyc_List_List*
ns2= Cyc_Tcenv_resolve_namespace( te, loc, first, rest); ge->availables=({
struct Cyc_List_List* _temp1374=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1374->hd=( void*) ns2; _temp1374->tl= ge->availables;
_temp1374;}); Cyc_Tc_tc_decls( te, _temp1329, in_externC); ge->availables=({
struct Cyc_List_List* _temp1375= ge->availables; if( _temp1375 == 0){ _throw(
Null_Exception);} _temp1375->tl;}); goto _LL1274;}} _LL1298: Cyc_Tc_tc_decls( te,
_temp1338, 1); goto _LL1274; _LL1274:;}} static char _temp1378[ 4u]="Cyc";
static struct _tagged_string Cyc_Tc_cyc_string=( struct _tagged_string){
_temp1378, _temp1378, _temp1378 + 4u}; static struct _tagged_string* Cyc_Tc_cyc_ns=&
Cyc_Tc_cyc_string; void Cyc_Tc_tc( struct Cyc_Tcenv_Tenv* te, int
add_cyc_namespace, struct Cyc_List_List* ds){ if( add_cyc_namespace){ ds=({
struct Cyc_List_List* _temp1379=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1379->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1380=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1380->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp1381=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp1381[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp1382; _temp1382.tag=
Cyc_Absyn_Namespace_d_tag; _temp1382.f1= Cyc_Tc_cyc_ns; _temp1382.f2= ds;
_temp1382;}); _temp1381;})); _temp1380->loc= 0; _temp1380;}); _temp1379->tl= 0;
_temp1379;});} Cyc_Tc_tc_decls( te, ds, 0);} typedef struct Cyc_Dict_Dict* Cyc_Tc_treeshake_env_t;
static int Cyc_Tc_vardecl_needed( struct Cyc_Dict_Dict* env, struct Cyc_Absyn_Decl*
d); static struct Cyc_List_List* Cyc_Tc_treeshake_f( struct Cyc_Dict_Dict* env,
struct Cyc_List_List* ds){ return(( struct Cyc_List_List*(*)( int(* f)( struct
Cyc_Dict_Dict*, struct Cyc_Absyn_Decl*), struct Cyc_Dict_Dict* env, struct Cyc_List_List*
l)) Cyc_List_filter_c)( Cyc_Tc_vardecl_needed, env, ds);} static int Cyc_Tc_vardecl_needed(
struct Cyc_Dict_Dict* env, struct Cyc_Absyn_Decl* d){ void* _temp1383=( void*) d->r;
struct Cyc_Absyn_Vardecl* _temp1393; struct Cyc_List_List* _temp1395; struct Cyc_List_List**
_temp1397; struct _tuple0* _temp1398; struct Cyc_List_List* _temp1400; struct
Cyc_List_List** _temp1402; struct _tagged_string* _temp1403; _LL1385: if(((
struct _tunion_struct*) _temp1383)->tag == Cyc_Absyn_Var_d_tag){ _LL1394:
_temp1393=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp1383)->f1; goto _LL1386;} else{ goto _LL1387;} _LL1387: if((( struct
_tunion_struct*) _temp1383)->tag == Cyc_Absyn_Using_d_tag){ _LL1399: _temp1398=(
struct _tuple0*)(( struct Cyc_Absyn_Using_d_struct*) _temp1383)->f1; goto
_LL1396; _LL1396: _temp1395=( struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*)
_temp1383)->f2; _temp1397=&(( struct Cyc_Absyn_Using_d_struct*) _temp1383)->f2;
goto _LL1388;} else{ goto _LL1389;} _LL1389: if((( struct _tunion_struct*)
_temp1383)->tag == Cyc_Absyn_Namespace_d_tag){ _LL1404: _temp1403=( struct
_tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*) _temp1383)->f1; goto
_LL1401; _LL1401: _temp1400=( struct Cyc_List_List*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1383)->f2; _temp1402=&(( struct Cyc_Absyn_Namespace_d_struct*) _temp1383)->f2;
goto _LL1390;} else{ goto _LL1391;} _LL1391: goto _LL1392; _LL1386: if(( void*)
_temp1393->sc !=( void*) Cyc_Absyn_Extern){ return 1;}{ struct _tuple0 _temp1407;
struct _tagged_string* _temp1408; void* _temp1410; struct _tuple0* _temp1405=
_temp1393->name; _temp1407=* _temp1405; _LL1411: _temp1410= _temp1407.f1; goto
_LL1409; _LL1409: _temp1408= _temp1407.f2; goto _LL1406; _LL1406: { struct Cyc_List_List*
ns;{ void* _temp1412= _temp1410; struct Cyc_List_List* _temp1420; struct Cyc_List_List*
_temp1422; _LL1414: if(( int) _temp1412 == Cyc_Absyn_Loc_n){ goto _LL1415;}
else{ goto _LL1416;} _LL1416: if(( unsigned int) _temp1412 > 1u?(( struct
_tunion_struct*) _temp1412)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL1421: _temp1420=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp1412)->f1; goto
_LL1417;} else{ goto _LL1418;} _LL1418: if(( unsigned int) _temp1412 > 1u?((
struct _tunion_struct*) _temp1412)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL1423:
_temp1422=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp1412)->f1;
goto _LL1419;} else{ goto _LL1413;} _LL1415: ns= 0; goto _LL1413; _LL1417: ns=
_temp1420; goto _LL1413; _LL1419: ns= _temp1422; goto _LL1413; _LL1413:;}{
struct _tuple9* nsenv=(( struct _tuple9*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)( env, ns); struct Cyc_Tcenv_Genv* ge=(* nsenv).f1; int
needed=(*(( struct _tuple6*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string*
key)) Cyc_Dict_lookup)( ge->ordinaries, _temp1408)).f2; if( ! needed){(* nsenv).f2=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tagged_string* elt)) Cyc_Set_insert)((*
nsenv).f2, _temp1408);} return needed;}}} _LL1388: _temp1402= _temp1397; goto
_LL1390; _LL1390:* _temp1402= Cyc_Tc_treeshake_f( env,* _temp1402); return 1;
_LL1392: return 1; _LL1384:;} static int Cyc_Tc_treeshake_remove_f( struct Cyc_Set_Set*
set, struct _tagged_string* x, struct _tuple6* y){ return !(( int(*)( struct Cyc_Set_Set*
s, struct _tagged_string* elt)) Cyc_Set_member)( set, x);} static void Cyc_Tc_treeshake_remove(
struct Cyc_List_List* ns, struct _tuple9* nsenv){ struct Cyc_Tcenv_Genv* ge=(*
nsenv).f1; struct Cyc_Set_Set* set=(* nsenv).f2; ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
int(* f)( struct Cyc_Set_Set*, struct _tagged_string*, struct _tuple6*), struct
Cyc_Set_Set* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_filter_c)( Cyc_Tc_treeshake_remove_f,
set, ge->ordinaries);} static struct _tuple9* Cyc_Tc_treeshake_make_env_f(
struct Cyc_Tcenv_Genv* ge){ return({ struct _tuple9* _temp1424=( struct _tuple9*)
GC_malloc( sizeof( struct _tuple9)); _temp1424->f1= ge; _temp1424->f2=(( struct
Cyc_Set_Set*(*)( int(* comp)( struct _tagged_string*, struct _tagged_string*)))
Cyc_Set_empty)( Cyc_String_strptrcmp); _temp1424;});} struct Cyc_List_List* Cyc_Tc_treeshake(
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* ds){ struct Cyc_Dict_Dict* env=((
struct Cyc_Dict_Dict*(*)( struct _tuple9*(* f)( struct Cyc_Tcenv_Genv*), struct
Cyc_Dict_Dict* d)) Cyc_Dict_map)( Cyc_Tc_treeshake_make_env_f, te->ae); struct
Cyc_List_List* ds1= Cyc_Tc_treeshake_f( env, ds);(( void(*)( void(* f)( struct
Cyc_List_List*, struct _tuple9*), struct Cyc_Dict_Dict* d)) Cyc_Dict_iter)( Cyc_Tc_treeshake_remove,
env); return ds1;}