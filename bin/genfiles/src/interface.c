#include "cyc_include.h"

 struct _tuple0{ void* f1; struct _tagged_string* f2; } ; struct _tuple1{ struct
Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual* f2; void* f3; } ; struct _tuple2{
struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; } ; struct _tagged_ptr0{
int* curr; int* base; int* last_plus_one; } ; struct _tuple3{ void* f1; int f2;
} ; struct _tuple4{ struct Cyc_Declarator* f1; struct Cyc_Absyn_Exp* f2; } ;
struct _tuple5{ struct Cyc_Absyn_Tqual* f1; struct Cyc_List_List* f2; struct Cyc_List_List*
f3; } ; struct _tuple6{ struct Cyc_Declarator* f1; struct Cyc_Core_Opt* f2; } ;
struct _tuple7{ struct Cyc_List_List* f1; int f2; struct Cyc_Core_Opt* f3; } ;
struct _tuple8{ struct Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ; struct
_tuple9{ struct Cyc_Interface_Ienv* f1; struct Cyc_Interface_Ienv* f2; } ;
struct _tuple10{ struct Cyc_Interface_Ienv* f1; struct Cyc_Interface_Ienv* f2;
int f3; } ; struct _tuple11{ struct Cyc_Interface_T* f1; int f2; } ; typedef int
Cyc_ptrdiff_t; typedef unsigned int Cyc_size_t; typedef int Cyc_wchar_t; typedef
unsigned int Cyc_wint_t; typedef char Cyc_u_char; typedef unsigned short Cyc_u_short;
typedef unsigned int Cyc_u_int; typedef unsigned int Cyc_u_long; typedef
unsigned short Cyc_ushort; typedef unsigned int Cyc_uint; typedef unsigned int
Cyc_clock_t; typedef int Cyc_time_t; struct Cyc_timespec{ int tv_sec; int
tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec it_interval; struct Cyc_timespec
it_value; } ; typedef int Cyc_daddr_t; typedef char* Cyc_caddr_t; typedef
unsigned short Cyc_ino_t; typedef short Cyc_dev_t; typedef int Cyc_off_t;
typedef unsigned short Cyc_uid_t; typedef unsigned short Cyc_gid_t; typedef int
Cyc_pid_t; typedef int Cyc_key_t; typedef int Cyc_ssize_t; typedef unsigned int
Cyc_mode_t; typedef unsigned short Cyc_nlink_t; typedef int Cyc_fd_mask; struct
Cyc__types_fd_set{ int fds_bits[ 8u]; } ; typedef struct Cyc__types_fd_set Cyc__types_fd_set;
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
typedef int Cyc_Stdio_fpos_t; extern char Cyc_Stdio_FileOpenError_tag[ 14u];
struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Stdio_FileCloseError_tag[ 15u]; struct Cyc_Stdio_FileCloseError_struct{
char* tag; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ;
typedef struct Cyc_List_List* Cyc_List_glist_t; typedef struct Cyc_List_List*
Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t; extern char Cyc_List_List_empty_tag[
11u]; struct Cyc_List_List_empty_struct{ char* tag; } ; extern char Cyc_List_List_mismatch_tag[
14u]; struct Cyc_List_List_mismatch_struct{ char* tag; } ; extern void Cyc_List_iter2(
void(* f)( void*, void*), struct Cyc_List_List* x, struct Cyc_List_List* y);
extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x); extern
char Cyc_List_Nth_tag[ 4u]; struct Cyc_List_Nth_struct{ char* tag; } ; struct
Cyc_Lineno_Pos{ struct _tagged_string logical_file; struct _tagged_string line;
int line_no; int col; } ; typedef struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t;
extern char Cyc_Position_Exit_tag[ 5u]; struct Cyc_Position_Exit_struct{ char*
tag; } ; struct Cyc_Position_Segment; typedef struct Cyc_Position_Segment* Cyc_Position_seg_t;
extern struct Cyc_Position_Segment* Cyc_Position_segment_of_abs( int, int);
typedef void* Cyc_Position_Error_kind; static const unsigned int Cyc_Position_Lex=
0; static const unsigned int Cyc_Position_Parse= 1; static const unsigned int
Cyc_Position_Elab= 2; typedef void* Cyc_Position_error_kind_t; struct Cyc_Position_Error{
struct _tagged_string source; struct Cyc_Position_Segment* seg; void* kind;
struct _tagged_string desc; } ; typedef struct Cyc_Position_Error* Cyc_Position_error_t;
extern char Cyc_Position_Nocontext_tag[ 10u]; struct Cyc_Position_Nocontext_struct{
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
char* tag; } ; extern struct Cyc_Absyn_Decl* Cyc_Absyn_new_decl( void* r, struct
Cyc_Position_Segment* loc); struct Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t;
typedef struct Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t;
extern char Cyc_Set_Absent_tag[ 7u]; struct Cyc_Set_Absent_struct{ char* tag; }
; struct Cyc_Dict_Dict; typedef struct Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef
struct Cyc_Dict_Dict* Cyc_Dict_dict_t; extern char Cyc_Dict_Present_tag[ 8u];
struct Cyc_Dict_Present_struct{ char* tag; } ; extern char Cyc_Dict_Absent_tag[
7u]; extern struct Cyc_Dict_Dict* Cyc_Dict_empty( int(* comp)( void*, void*));
extern struct Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict* d, void* key,
void* data); extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d, void* key);
extern void* Cyc_Dict_fold( void*(* f)( void*, void*, void*), struct Cyc_Dict_Dict*
d, void* accum); extern void Cyc_Dict_iter_c( void(* f)( void*, void*, void*),
void* env, struct Cyc_Dict_Dict* d); extern void Cyc_Dict_iter2( void(* f)( void*,
void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2); extern struct Cyc_Dict_Dict*
Cyc_Dict_map( void*(* f)( void*), struct Cyc_Dict_Dict* d); typedef void* Cyc_Tcenv_Resolved;
static const int Cyc_Tcenv_VarRes_tag= 0; struct Cyc_Tcenv_VarRes_struct{ int
tag; void* f1; } ; static const int Cyc_Tcenv_StructRes_tag= 1; struct Cyc_Tcenv_StructRes_struct{
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
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tc_init(); struct Cyc_Interface_T;
typedef struct Cyc_Interface_T* Cyc_Interface_t; extern struct Cyc_Interface_T*
Cyc_Interface_extract( struct Cyc_Dict_Dict* ae); extern int Cyc_Interface_is_subinterface(
struct Cyc_Interface_T* i1, struct Cyc_Interface_T* i2); extern void Cyc_Interface_print(
struct Cyc_Interface_T*, struct Cyc_Stdio___sFILE*); extern struct Cyc_Interface_T*
Cyc_Interface_parse( struct Cyc_Stdio___sFILE*); struct Cyc_PP_Ppstate; typedef
struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t; struct Cyc_PP_Out; typedef struct Cyc_PP_Out*
Cyc_PP_out_t; struct Cyc_PP_Doc; typedef struct Cyc_PP_Doc* Cyc_PP_doc_t; extern
int Cyc_Absynpp_expand_typedefs; extern void Cyc_Absynpp_decllist2file( struct
Cyc_List_List* tdl, struct Cyc_Stdio___sFILE* f); extern struct _tagged_string
Cyc_Absynpp_qvar2string( struct _tuple0*); extern void Cyc_Tc_tc( struct Cyc_Tcenv_Tenv*
te, int add_cyc_namespace, struct Cyc_List_List* ds); extern struct Cyc_List_List*
Cyc_Tc_treeshake( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*); extern char
Cyc_Lexing_Error_tag[ 6u]; struct Cyc_Lexing_Error_struct{ char* tag; struct
_tagged_string f1; } ; struct Cyc_Lexing_lexbuf{ void(* refill_buff)( struct Cyc_Lexing_lexbuf*);
void* refill_state; struct _tagged_string lex_buffer; int lex_buffer_len; int
lex_abs_pos; int lex_start_pos; int lex_curr_pos; int lex_last_pos; int
lex_last_action; int lex_eof_reached; } ; typedef struct Cyc_Lexing_lexbuf* Cyc_Lexing_Lexbuf;
struct Cyc_Lexing_function_lexbuf_state{ int(* read_fun)( struct _tagged_string,
int, void*); void* read_fun_state; } ; typedef struct Cyc_Lexing_function_lexbuf_state*
Cyc_Lexing_Function_lexbuf_state; struct Cyc_Lexing_lex_tables{ struct
_tagged_ptr0 lex_base; struct _tagged_ptr0 lex_backtrk; struct _tagged_ptr0
lex_default; struct _tagged_ptr0 lex_trans; struct _tagged_ptr0 lex_check; } ;
typedef struct Cyc_Lexing_lex_tables* Cyc_Lexing_LexTables; extern struct Cyc_List_List*
Cyc_Parse_parse_file( struct Cyc_Stdio___sFILE* f); typedef void* Cyc_Struct_or_union;
typedef void* Cyc_struct_or_union_t; typedef void* Cyc_Blockitem; typedef void*
Cyc_blockitem_t; typedef void* Cyc_Type_specifier; typedef void* Cyc_type_specifier_t;
typedef void* Cyc_Storage_class; typedef void* Cyc_storage_class_t; struct Cyc_Declaration_spec;
typedef struct Cyc_Declaration_spec* Cyc_decl_spec_t; struct Cyc_Declarator;
typedef struct Cyc_Declarator* Cyc_declarator_t; struct Cyc_Abstractdeclarator;
typedef struct Cyc_Abstractdeclarator* Cyc_abstractdeclarator_t; typedef struct
_xtunion_struct* Cyc_YYSTYPE; extern char Cyc_Okay_tok_tag[ 9u]; struct Cyc_Okay_tok_struct{
char* tag; } ; extern char Cyc_Int_tok_tag[ 8u]; struct Cyc_Int_tok_struct{ char*
tag; struct _tuple3* f1; } ; extern char Cyc_Char_tok_tag[ 9u]; struct Cyc_Char_tok_struct{
char* tag; char f1; } ; extern char Cyc_Pointer_Sort_tok_tag[ 17u]; struct Cyc_Pointer_Sort_tok_struct{
char* tag; void* f1; } ; extern char Cyc_Short_tok_tag[ 10u]; struct Cyc_Short_tok_struct{
char* tag; short f1; } ; extern char Cyc_String_tok_tag[ 11u]; struct Cyc_String_tok_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Stringopt_tok_tag[ 14u];
struct Cyc_Stringopt_tok_struct{ char* tag; struct Cyc_Core_Opt* f1; } ; extern
char Cyc_Type_tok_tag[ 9u]; struct Cyc_Type_tok_struct{ char* tag; void* f1; } ;
extern char Cyc_TypeList_tok_tag[ 13u]; struct Cyc_TypeList_tok_struct{ char*
tag; struct Cyc_List_List* f1; } ; extern char Cyc_Exp_tok_tag[ 8u]; struct Cyc_Exp_tok_struct{
char* tag; struct Cyc_Absyn_Exp* f1; } ; extern char Cyc_ExpList_tok_tag[ 12u];
struct Cyc_ExpList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ; extern
char Cyc_Primop_tok_tag[ 11u]; struct Cyc_Primop_tok_struct{ char* tag; void* f1;
} ; extern char Cyc_Primopopt_tok_tag[ 14u]; struct Cyc_Primopopt_tok_struct{
char* tag; struct Cyc_Core_Opt* f1; } ; extern char Cyc_QualId_tok_tag[ 11u];
struct Cyc_QualId_tok_struct{ char* tag; struct _tuple0* f1; } ; extern char Cyc_Stmt_tok_tag[
9u]; struct Cyc_Stmt_tok_struct{ char* tag; struct Cyc_Absyn_Stmt* f1; } ;
extern char Cyc_SwitchClauseList_tok_tag[ 21u]; struct Cyc_SwitchClauseList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; extern char Cyc_Pattern_tok_tag[ 12u];
struct Cyc_Pattern_tok_struct{ char* tag; struct Cyc_Absyn_Pat* f1; } ; extern
char Cyc_PatternList_tok_tag[ 16u]; struct Cyc_PatternList_tok_struct{ char* tag;
struct Cyc_List_List* f1; } ; extern char Cyc_FnDecl_tok_tag[ 11u]; struct Cyc_FnDecl_tok_struct{
char* tag; struct Cyc_Absyn_Fndecl* f1; } ; extern char Cyc_DeclList_tok_tag[ 13u];
struct Cyc_DeclList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ; extern
char Cyc_DeclSpec_tok_tag[ 13u]; struct Cyc_DeclSpec_tok_struct{ char* tag;
struct Cyc_Declaration_spec* f1; } ; extern char Cyc_InitDecl_tok_tag[ 13u];
struct Cyc_InitDecl_tok_struct{ char* tag; struct _tuple4* f1; } ; extern char
Cyc_InitDeclList_tok_tag[ 17u]; struct Cyc_InitDeclList_tok_struct{ char* tag;
struct Cyc_List_List* f1; } ; extern char Cyc_StorageClass_tok_tag[ 17u]; struct
Cyc_StorageClass_tok_struct{ char* tag; void* f1; } ; extern char Cyc_TypeSpecifier_tok_tag[
18u]; struct Cyc_TypeSpecifier_tok_struct{ char* tag; void* f1; } ; extern char
Cyc_QualSpecList_tok_tag[ 17u]; struct Cyc_QualSpecList_tok_struct{ char* tag;
struct _tuple5* f1; } ; extern char Cyc_TypeQual_tok_tag[ 13u]; struct Cyc_TypeQual_tok_struct{
char* tag; struct Cyc_Absyn_Tqual* f1; } ; extern char Cyc_StructFieldDeclList_tok_tag[
24u]; struct Cyc_StructFieldDeclList_tok_struct{ char* tag; struct Cyc_List_List*
f1; } ; extern char Cyc_StructFieldDeclListList_tok_tag[ 28u]; struct Cyc_StructFieldDeclListList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; extern char Cyc_Declarator_tok_tag[ 15u];
struct Cyc_Declarator_tok_struct{ char* tag; struct Cyc_Declarator* f1; } ;
extern char Cyc_DeclaratorExpopt_tok_tag[ 21u]; struct Cyc_DeclaratorExpopt_tok_struct{
char* tag; struct _tuple6* f1; } ; extern char Cyc_DeclaratorExpoptList_tok_tag[
25u]; struct Cyc_DeclaratorExpoptList_tok_struct{ char* tag; struct Cyc_List_List*
f1; } ; extern char Cyc_AbstractDeclarator_tok_tag[ 23u]; struct Cyc_AbstractDeclarator_tok_struct{
char* tag; struct Cyc_Abstractdeclarator* f1; } ; extern char Cyc_TunionField_tok_tag[
16u]; struct Cyc_TunionField_tok_struct{ char* tag; struct Cyc_Absyn_Tunionfield*
f1; } ; extern char Cyc_TunionFieldList_tok_tag[ 20u]; struct Cyc_TunionFieldList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; extern char Cyc_ParamDecl_tok_tag[ 14u];
struct Cyc_ParamDecl_tok_struct{ char* tag; struct _tuple1* f1; } ; extern char
Cyc_ParamDeclList_tok_tag[ 18u]; struct Cyc_ParamDeclList_tok_struct{ char* tag;
struct Cyc_List_List* f1; } ; extern char Cyc_ParamDeclListBool_tok_tag[ 22u];
struct Cyc_ParamDeclListBool_tok_struct{ char* tag; struct _tuple7* f1; } ;
extern char Cyc_StructOrUnion_tok_tag[ 18u]; struct Cyc_StructOrUnion_tok_struct{
char* tag; void* f1; } ; extern char Cyc_IdList_tok_tag[ 11u]; struct Cyc_IdList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; extern char Cyc_Designator_tok_tag[ 15u];
struct Cyc_Designator_tok_struct{ char* tag; void* f1; } ; extern char Cyc_DesignatorList_tok_tag[
19u]; struct Cyc_DesignatorList_tok_struct{ char* tag; struct Cyc_List_List* f1;
} ; extern char Cyc_TypeModifierList_tok_tag[ 21u]; struct Cyc_TypeModifierList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; extern char Cyc_Rgn_tok_tag[ 8u];
struct Cyc_Rgn_tok_struct{ char* tag; void* f1; } ; extern char Cyc_InitializerList_tok_tag[
20u]; struct Cyc_InitializerList_tok_struct{ char* tag; struct Cyc_List_List* f1;
} ; extern char Cyc_FieldPattern_tok_tag[ 17u]; struct Cyc_FieldPattern_tok_struct{
char* tag; struct _tuple8* f1; } ; extern char Cyc_FieldPatternList_tok_tag[ 21u];
struct Cyc_FieldPatternList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ;
extern char Cyc_BlockItem_tok_tag[ 14u]; struct Cyc_BlockItem_tok_struct{ char*
tag; void* f1; } ; extern char Cyc_Kind_tok_tag[ 9u]; struct Cyc_Kind_tok_struct{
char* tag; void* f1; } ; extern char Cyc_Attribute_tok_tag[ 14u]; struct Cyc_Attribute_tok_struct{
char* tag; void* f1; } ; extern char Cyc_AttributeList_tok_tag[ 18u]; struct Cyc_AttributeList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; extern char Cyc_Enumfield_tok_tag[ 14u];
struct Cyc_Enumfield_tok_struct{ char* tag; struct Cyc_Absyn_Enumfield* f1; } ;
extern char Cyc_EnumfieldList_tok_tag[ 18u]; struct Cyc_EnumfieldList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; struct Cyc_Yyltype{ int timestamp; int
first_line; int first_column; int last_line; int last_column; struct
_tagged_string text; } ; typedef struct Cyc_Yyltype Cyc_yyltype; extern int Cyc_String_strcmp(
struct _tagged_string s1, struct _tagged_string s2); extern int Cyc_String_zstrptrcmp(
struct _tagged_string*, struct _tagged_string*); extern char Cyc_Tcutil_TypeErr_tag[
8u]; struct Cyc_Tcutil_TypeErr_struct{ char* tag; } ; extern void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*, struct _tagged_string); extern int Cyc_Tcutil_is_function_type(
void* t); extern void Cyc_Lex_lex_init(); struct Cyc_Interface_Ienv{ struct Cyc_Dict_Dict*
structdecls; struct Cyc_Dict_Dict* uniondecls; struct Cyc_Dict_Dict* tuniondecls;
struct Cyc_Dict_Dict* xtuniondecls; struct Cyc_Dict_Dict* enumdecls; struct Cyc_Dict_Dict*
typedefdecls; struct Cyc_Dict_Dict* vardecls; } ; typedef struct Cyc_Interface_Ienv*
Cyc_Interface_ienv_t; struct Cyc_Interface_T{ struct Cyc_Dict_Dict* externs;
struct Cyc_Dict_Dict* publics; } ; struct Cyc_Interface_Ienv* Cyc_Interface_new_ienv(){
return({ struct Cyc_Interface_Ienv* _temp0=( struct Cyc_Interface_Ienv*)
GC_malloc( sizeof( struct Cyc_Interface_Ienv)); _temp0->structdecls=(( struct
Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_string*, struct _tagged_string*)))
Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp0->uniondecls=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); _temp0->tuniondecls=(( struct Cyc_Dict_Dict*(*)( int(*
comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp0->xtuniondecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp0->enumdecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp0->typedefdecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp0->vardecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_string*,
struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp0;});}
static void Cyc_Interface_extract_structdecl( struct _tuple9* env, struct
_tagged_string* x, struct Cyc_Absyn_Structdecl** dp){ struct Cyc_Interface_Ienv*
ext=(* env).f1; struct Cyc_Interface_Ienv* pub=(* env).f2; struct Cyc_Absyn_Structdecl*
d=* dp; void* _temp1=( void*) d->sc; _LL3: if(( int) _temp1 == Cyc_Absyn_Static){
goto _LL4;} else{ goto _LL5;} _LL5: if(( int) _temp1 == Cyc_Absyn_Abstract){
goto _LL6;} else{ goto _LL7;} _LL7: if(( int) _temp1 == Cyc_Absyn_Public){ goto
_LL8;} else{ goto _LL9;} _LL9: if(( int) _temp1 == Cyc_Absyn_ExternC){ goto
_LL10;} else{ goto _LL11;} _LL11: if(( int) _temp1 == Cyc_Absyn_Extern){ goto
_LL12;} else{ goto _LL2;} _LL4: goto _LL2; _LL6: if( d->fields != 0){ d=({
struct Cyc_Absyn_Structdecl* _temp13=( struct Cyc_Absyn_Structdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp13[ 0]=* d; _temp13;}); d->fields= 0;}
goto _LL8; _LL8: pub->structdecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)(
pub->structdecls, x, d); goto _LL2; _LL10: goto _LL12; _LL12: ext->structdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)( ext->structdecls, x, d);
goto _LL2; _LL2:;} static void Cyc_Interface_extract_uniondecl( struct _tuple9*
env, struct _tagged_string* x, struct Cyc_Absyn_Uniondecl** dp){ struct Cyc_Interface_Ienv*
ext=(* env).f1; struct Cyc_Interface_Ienv* pub=(* env).f2; struct Cyc_Absyn_Uniondecl*
d=* dp; void* _temp14=( void*) d->sc; _LL16: if(( int) _temp14 == Cyc_Absyn_Static){
goto _LL17;} else{ goto _LL18;} _LL18: if(( int) _temp14 == Cyc_Absyn_Abstract){
goto _LL19;} else{ goto _LL20;} _LL20: if(( int) _temp14 == Cyc_Absyn_Public){
goto _LL21;} else{ goto _LL22;} _LL22: if(( int) _temp14 == Cyc_Absyn_ExternC){
goto _LL23;} else{ goto _LL24;} _LL24: if(( int) _temp14 == Cyc_Absyn_Extern){
goto _LL25;} else{ goto _LL15;} _LL17: goto _LL15; _LL19: if( d->fields != 0){ d=({
struct Cyc_Absyn_Uniondecl* _temp26=( struct Cyc_Absyn_Uniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Uniondecl)); _temp26[ 0]=* d; _temp26;}); d->fields= 0;}
goto _LL21; _LL21: pub->uniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl* data)) Cyc_Dict_insert)(
pub->uniondecls, x, d); goto _LL15; _LL23: goto _LL25; _LL25: ext->uniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Uniondecl* data)) Cyc_Dict_insert)( ext->uniondecls, x, d);
goto _LL15; _LL15:;} static void Cyc_Interface_extract_tuniondecl( struct
_tuple9* env, struct _tagged_string* x, struct Cyc_Absyn_Tuniondecl** dp){
struct Cyc_Interface_Ienv* ext=(* env).f1; struct Cyc_Interface_Ienv* pub=(* env).f2;
struct Cyc_Absyn_Tuniondecl* d=* dp; void* _temp27=( void*) d->sc; _LL29: if((
int) _temp27 == Cyc_Absyn_Static){ goto _LL30;} else{ goto _LL31;} _LL31: if((
int) _temp27 == Cyc_Absyn_Abstract){ goto _LL32;} else{ goto _LL33;} _LL33: if((
int) _temp27 == Cyc_Absyn_Public){ goto _LL34;} else{ goto _LL35;} _LL35: if((
int) _temp27 == Cyc_Absyn_ExternC){ goto _LL36;} else{ goto _LL37;} _LL37: if((
int) _temp27 == Cyc_Absyn_Extern){ goto _LL38;} else{ goto _LL28;} _LL30: goto
_LL28; _LL32: if( d->fields != 0){ d=({ struct Cyc_Absyn_Tuniondecl* _temp39=(
struct Cyc_Absyn_Tuniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp39[ 0]=* d; _temp39;}); d->fields= 0;} goto _LL34; _LL34: pub->tuniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Tuniondecl* data)) Cyc_Dict_insert)( pub->tuniondecls, x, d);
goto _LL28; _LL36: goto _LL38; _LL38: ext->tuniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Tuniondecl*
data)) Cyc_Dict_insert)( ext->tuniondecls, x, d); goto _LL28; _LL28:;} static
void Cyc_Interface_extract_xtuniondecl( struct Cyc_Interface_T* i, struct
_tagged_string* x, struct Cyc_Absyn_XTuniondecl** dp){ struct Cyc_Absyn_XTuniondecl*
d=* dp; void* _temp40=( void*) d->sc; _LL42: if(( int) _temp40 == Cyc_Absyn_Static){
goto _LL43;} else{ goto _LL44;} _LL44: if(( int) _temp40 == Cyc_Absyn_Abstract){
goto _LL45;} else{ goto _LL46;} _LL46: if(( int) _temp40 == Cyc_Absyn_Public){
goto _LL47;} else{ goto _LL48;} _LL48: if(( int) _temp40 == Cyc_Absyn_ExternC){
goto _LL49;} else{ goto _LL50;} _LL50: if(( int) _temp40 == Cyc_Absyn_Extern){
goto _LL51;} else{ goto _LL41;} _LL43: goto _LL41; _LL45: if( d->fields != 0){ d=({
struct Cyc_Absyn_XTuniondecl* _temp52=( struct Cyc_Absyn_XTuniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_XTuniondecl)); _temp52[ 0]=* d; _temp52;}); d->fields=
0;} goto _LL47; _LL47:{ struct Cyc_List_List* f= d->fields; for( 0; f != 0; f=({
struct Cyc_List_List* _temp53= f; if( _temp53 == 0){ _throw( Null_Exception);}
_temp53->tl;})){ struct Cyc_List_List* ns= 0;{ void* _temp55=(*(( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp54= f; if( _temp54 == 0){ _throw( Null_Exception);}
_temp54->hd;}))->name).f1; struct Cyc_List_List* _temp63; struct Cyc_List_List*
_temp65; _LL57: if(( unsigned int) _temp55 > 1u?(( struct _tunion_struct*)
_temp55)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL64: _temp63=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp55)->f1; goto _LL58;} else{ goto _LL59;}
_LL59: if(( unsigned int) _temp55 > 1u?(( struct _tunion_struct*) _temp55)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL66: _temp65=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp55)->f1; goto _LL60;} else{ goto _LL61;} _LL61: goto _LL62; _LL58: _temp65=
_temp63; goto _LL60; _LL60: ns= _temp65; goto _LL56; _LL62: goto _LL56; _LL56:;}{
struct Cyc_Absyn_XTuniondecl* fd=({ struct Cyc_Absyn_XTuniondecl* _temp67=(
struct Cyc_Absyn_XTuniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_XTuniondecl));
_temp67[ 0]=* d; _temp67;}); fd->name=({ struct _tuple0* _temp68=( struct
_tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp68->f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp69=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp69[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp70; _temp70.tag= Cyc_Absyn_Abs_n_tag;
_temp70.f1= ns; _temp70;}); _temp69;}); _temp68->f2=(* fd->name).f2; _temp68;});
fd->fields=({ struct Cyc_List_List* _temp71=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp71->hd=( void*)(( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp72= f; if( _temp72 == 0){ _throw( Null_Exception);}
_temp72->hd;})); _temp71->tl= 0; _temp71;});{ struct Cyc_Interface_Ienv* pub=((
struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)( i->publics, ns); pub->xtuniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_XTuniondecl*
data)) Cyc_Dict_insert)( pub->xtuniondecls, x, fd);}}}} goto _LL41; _LL49: goto
_LL51; _LL51:{ struct Cyc_List_List* f= d->fields; for( 0; f != 0; f=({ struct
Cyc_List_List* _temp73= f; if( _temp73 == 0){ _throw( Null_Exception);} _temp73->tl;})){
struct Cyc_List_List* ns= 0;{ void* _temp75=(*(( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp74= f; if( _temp74 == 0){ _throw( Null_Exception);}
_temp74->hd;}))->name).f1; struct Cyc_List_List* _temp83; struct Cyc_List_List*
_temp85; _LL77: if(( unsigned int) _temp75 > 1u?(( struct _tunion_struct*)
_temp75)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL84: _temp83=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp75)->f1; goto _LL78;} else{ goto _LL79;}
_LL79: if(( unsigned int) _temp75 > 1u?(( struct _tunion_struct*) _temp75)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL86: _temp85=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp75)->f1; goto _LL80;} else{ goto _LL81;} _LL81: goto _LL82; _LL78: _temp85=
_temp83; goto _LL80; _LL80: ns= _temp85; goto _LL76; _LL82: goto _LL76; _LL76:;}{
struct Cyc_Absyn_XTuniondecl* fd=({ struct Cyc_Absyn_XTuniondecl* _temp87=(
struct Cyc_Absyn_XTuniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_XTuniondecl));
_temp87[ 0]=* d; _temp87;}); fd->name=({ struct _tuple0* _temp88=( struct
_tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp88->f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp89=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp89[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp90; _temp90.tag= Cyc_Absyn_Abs_n_tag;
_temp90.f1= ns; _temp90;}); _temp89;}); _temp88->f2=(* fd->name).f2; _temp88;});
fd->fields=({ struct Cyc_List_List* _temp91=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp91->hd=( void*)(( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp92= f; if( _temp92 == 0){ _throw( Null_Exception);}
_temp92->hd;})); _temp91->tl= 0; _temp91;});{ struct Cyc_Interface_Ienv* ext=((
struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)( i->externs, ns); ext->xtuniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_XTuniondecl*
data)) Cyc_Dict_insert)( ext->xtuniondecls, x, fd);}}}} goto _LL41; _LL41:;}
static void Cyc_Interface_extract_enumdecl( struct _tuple9* env, struct
_tagged_string* x, struct Cyc_Absyn_Enumdecl** dp){ struct Cyc_Interface_Ienv*
ext=(* env).f1; struct Cyc_Interface_Ienv* pub=(* env).f2; struct Cyc_Absyn_Enumdecl*
d=* dp; void* _temp93=( void*) d->sc; _LL95: if(( int) _temp93 == Cyc_Absyn_Static){
goto _LL96;} else{ goto _LL97;} _LL97: if(( int) _temp93 == Cyc_Absyn_Abstract){
goto _LL98;} else{ goto _LL99;} _LL99: if(( int) _temp93 == Cyc_Absyn_Public){
goto _LL100;} else{ goto _LL101;} _LL101: if(( int) _temp93 == Cyc_Absyn_ExternC){
goto _LL102;} else{ goto _LL103;} _LL103: if(( int) _temp93 == Cyc_Absyn_Extern){
goto _LL104;} else{ goto _LL94;} _LL96: goto _LL94; _LL98: if( d->fields != 0){
d=({ struct Cyc_Absyn_Enumdecl* _temp105=( struct Cyc_Absyn_Enumdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp105[ 0]=* d; _temp105;}); d->fields= 0;}
goto _LL100; _LL100: pub->enumdecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Enumdecl* data)) Cyc_Dict_insert)(
pub->enumdecls, x, d); goto _LL94; _LL102: goto _LL104; _LL104: ext->enumdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Enumdecl* data)) Cyc_Dict_insert)( ext->enumdecls, x, d); goto
_LL94; _LL94:;} static void Cyc_Interface_extract_typedef( struct _tuple9* env,
struct _tagged_string* x, struct Cyc_Absyn_Typedefdecl* d){ struct Cyc_Interface_Ienv*
pub=(* env).f2; pub->typedefdecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Typedefdecl* data)) Cyc_Dict_insert)(
pub->typedefdecls, x, d);} static void Cyc_Interface_extract_ordinarie( struct
_tuple10* env, struct _tagged_string* x, struct _tuple3* v){ struct Cyc_Interface_Ienv*
ext=(* env).f1; struct Cyc_Interface_Ienv* pub=(* env).f2; int
check_fun_definition=(* env).f3; void* r=(* v).f1; void* _temp106= r; void*
_temp112; _LL108: if((( struct _tunion_struct*) _temp106)->tag == Cyc_Tcenv_VarRes_tag){
_LL113: _temp112=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp106)->f1; goto
_LL109;} else{ goto _LL110;} _LL110: goto _LL111; _LL109:{ void* _temp114=
_temp112; struct Cyc_Absyn_Fndecl* _temp122; struct Cyc_Absyn_Vardecl* _temp124;
_LL116: if(( unsigned int) _temp114 > 1u?(( struct _tunion_struct*) _temp114)->tag
== Cyc_Absyn_Funname_b_tag: 0){ _LL123: _temp122=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Funname_b_struct*) _temp114)->f1; goto _LL117;} else{ goto
_LL118;} _LL118: if(( unsigned int) _temp114 > 1u?(( struct _tunion_struct*)
_temp114)->tag == Cyc_Absyn_Global_b_tag: 0){ _LL125: _temp124=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Global_b_struct*) _temp114)->f1; goto _LL119;} else{ goto
_LL120;} _LL120: goto _LL121; _LL117: { struct Cyc_Absyn_Vardecl* vd=({ struct
Cyc_Absyn_Vardecl* _temp126=( struct Cyc_Absyn_Vardecl*) GC_malloc( sizeof(
struct Cyc_Absyn_Vardecl)); _temp126->sc=( void*)(( void*) _temp122->sc);
_temp126->name= _temp122->name; _temp126->tq=({ struct Cyc_Absyn_Tqual* _temp128=(
struct Cyc_Absyn_Tqual*) GC_malloc_atomic( sizeof( struct Cyc_Absyn_Tqual));
_temp128->q_const= 0; _temp128->q_volatile= 0; _temp128->q_restrict= 0; _temp128;});
_temp126->type=( void*)(( void*)({ struct Cyc_Core_Opt* _temp127= _temp122->cached_typ;
if( _temp127 == 0){ _throw( Null_Exception);} _temp127->v;})); _temp126->initializer=
0; _temp126->shadow= 0; _temp126->region= 0; _temp126->attributes= 0; _temp126;});
check_fun_definition= 0; _temp124= vd; goto _LL119;} _LL119:{ void* _temp129=(
void*) _temp124->sc; _LL131: if(( int) _temp129 == Cyc_Absyn_Static){ goto
_LL132;} else{ goto _LL133;} _LL133: if(( int) _temp129 == Cyc_Absyn_Abstract){
goto _LL134;} else{ goto _LL135;} _LL135: if(( int) _temp129 == Cyc_Absyn_Public){
goto _LL136;} else{ goto _LL137;} _LL137: if(( int) _temp129 == Cyc_Absyn_ExternC){
goto _LL138;} else{ goto _LL139;} _LL139: if(( int) _temp129 == Cyc_Absyn_Extern){
goto _LL140;} else{ goto _LL130;} _LL132: if( check_fun_definition? Cyc_Tcutil_is_function_type((
void*) _temp124->type): 0){ Cyc_Tcutil_terr( Cyc_Position_segment_of_abs( 0, 0),({
struct _tagged_string _temp141= Cyc_Absynpp_qvar2string( _temp124->name);
xprintf("static function %.*s has never been defined", _temp141.last_plus_one -
_temp141.curr, _temp141.curr);}));} goto _LL130; _LL134: goto _LL130; _LL136:
if( check_fun_definition? Cyc_Tcutil_is_function_type(( void*) _temp124->type):
0){ Cyc_Tcutil_terr( Cyc_Position_segment_of_abs( 0, 0),({ struct _tagged_string
_temp142= Cyc_Absynpp_qvar2string( _temp124->name); xprintf("public function %.*s has never been defined",
_temp142.last_plus_one - _temp142.curr, _temp142.curr);}));} pub->vardecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Vardecl* data)) Cyc_Dict_insert)( pub->vardecls, x, _temp124);
goto _LL130; _LL138: goto _LL140; _LL140: ext->vardecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Vardecl*
data)) Cyc_Dict_insert)( ext->vardecls, x, _temp124); goto _LL130; _LL130:;}
goto _LL115; _LL121: goto _LL115; _LL115:;} goto _LL107; _LL111: goto _LL107;
_LL107:;} static void Cyc_Interface_extract_f( struct _tuple11* env_f, struct
Cyc_List_List* ns, struct Cyc_Tcenv_Genv* ge){ struct Cyc_Interface_T* i=(*
env_f).f1; int check_fun_definition=(* env_f).f2; struct _tuple9* env=({ struct
_tuple9* _temp143=( struct _tuple9*) GC_malloc( sizeof( struct _tuple9));
_temp143->f1=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict* d, struct
Cyc_List_List* key)) Cyc_Dict_lookup)( i->externs, ns); _temp143->f2=(( struct
Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)(
i->publics, ns); _temp143;});(( void(*)( void(* f)( struct _tuple9*, struct
_tagged_string*, struct Cyc_Absyn_Structdecl**), struct _tuple9* env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_structdecl, env, ge->structdecls);((
void(*)( void(* f)( struct _tuple9*, struct _tagged_string*, struct Cyc_Absyn_Uniondecl**),
struct _tuple9* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_uniondecl,
env, ge->uniondecls);(( void(*)( void(* f)( struct _tuple9*, struct
_tagged_string*, struct Cyc_Absyn_Tuniondecl**), struct _tuple9* env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_tuniondecl, env, ge->tuniondecls);((
void(*)( void(* f)( struct Cyc_Interface_T*, struct _tagged_string*, struct Cyc_Absyn_XTuniondecl**),
struct Cyc_Interface_T* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_xtuniondecl,
i, ge->xtuniondecls);(( void(*)( void(* f)( struct _tuple9*, struct
_tagged_string*, struct Cyc_Absyn_Enumdecl**), struct _tuple9* env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_enumdecl, env, ge->enumdecls);((
void(*)( void(* f)( struct _tuple9*, struct _tagged_string*, struct Cyc_Absyn_Typedefdecl*),
struct _tuple9* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_typedef,
env, ge->typedefs);(( void(*)( void(* f)( struct _tuple10*, struct
_tagged_string*, struct _tuple3*), struct _tuple10* env, struct Cyc_Dict_Dict* d))
Cyc_Dict_iter_c)( Cyc_Interface_extract_ordinarie,({ struct _tuple10* _temp144=(
struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp144->f1=(* env).f1;
_temp144->f2=(* env).f2; _temp144->f3= check_fun_definition; _temp144;}), ge->ordinaries);}
static struct Cyc_Interface_Ienv* Cyc_Interface_extract_new_ienv( void* ignore){
return Cyc_Interface_new_ienv();} static struct Cyc_Interface_T* Cyc_Interface_gen_extract(
struct Cyc_Dict_Dict* ae, int check_fun_definition){ struct Cyc_Interface_T* i=({
struct Cyc_Interface_T* _temp145=( struct Cyc_Interface_T*) GC_malloc( sizeof(
struct Cyc_Interface_T)); _temp145->externs=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Interface_Ienv*(* f)( struct Cyc_Tcenv_Genv*), struct Cyc_Dict_Dict* d)) Cyc_Dict_map)((
struct Cyc_Interface_Ienv*(*)( struct Cyc_Tcenv_Genv* ignore)) Cyc_Interface_extract_new_ienv,
ae); _temp145->publics=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Interface_Ienv*(*
f)( struct Cyc_Tcenv_Genv*), struct Cyc_Dict_Dict* d)) Cyc_Dict_map)(( struct
Cyc_Interface_Ienv*(*)( struct Cyc_Tcenv_Genv* ignore)) Cyc_Interface_extract_new_ienv,
ae); _temp145;});(( void(*)( void(* f)( struct _tuple11*, struct Cyc_List_List*,
struct Cyc_Tcenv_Genv*), struct _tuple11* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(
Cyc_Interface_extract_f,({ struct _tuple11* _temp146=( struct _tuple11*)
GC_malloc( sizeof( struct _tuple11)); _temp146->f1= i; _temp146->f2=
check_fun_definition; _temp146;}), ae); return i;} struct Cyc_Interface_T* Cyc_Interface_extract(
struct Cyc_Dict_Dict* ae){ return Cyc_Interface_gen_extract( ae, 1);} char Cyc_Interface_Check_False_tag[
6u]="False"; struct Cyc_Interface_Check_False_struct{ char* tag; } ; static void
Cyc_Interface_Check_is_true( int b){ if( ! b){( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Interface_Check_False_struct* _temp147=( struct
Cyc_Interface_Check_False_struct*) GC_malloc( sizeof( struct Cyc_Interface_Check_False_struct));
_temp147[ 0]=({ struct Cyc_Interface_Check_False_struct _temp148; _temp148.tag=
Cyc_Interface_Check_False_tag; _temp148;}); _temp147;}));}} static void Cyc_Interface_Check_box(
void* x, void* y){ Cyc_Interface_Check_is_true( x == y);} static void Cyc_Interface_Check_list(
void(* f)( void*, void*), struct Cyc_List_List* l1, struct Cyc_List_List* l2){((
void(*)( void(* f)( void*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_iter2)( f, l1, l2);} static void Cyc_Interface_Check_opt( void(* f)(
void*, void*), struct Cyc_Core_Opt* o1, struct Cyc_Core_Opt* o2){ if( o1 == 0){
Cyc_Interface_Check_is_true( o2 == 0);} else{ Cyc_Interface_Check_is_true( o2 !=
0); f(( void*)({ struct Cyc_Core_Opt* _temp149= o1; if( _temp149 == 0){ _throw(
Null_Exception);} _temp149->v;}),( void*)({ struct Cyc_Core_Opt* _temp150= o2;
if( _temp150 == 0){ _throw( Null_Exception);} _temp150->v;}));}} static void Cyc_Interface_Check_str(
struct _tagged_string s1, struct _tagged_string s2){ Cyc_Interface_Check_is_true(
Cyc_String_strcmp( s1, s2) == 0);} static void Cyc_Interface_Check_strptr(
struct _tagged_string* n1, struct _tagged_string* n2){ Cyc_Interface_Check_str(*
n1,* n2);} static void Cyc_Interface_Check_tvar( struct Cyc_Absyn_Tvar* t1,
struct Cyc_Absyn_Tvar* t2){;} static void Cyc_Interface_Check_tvar_list( struct
Cyc_List_List* l1, struct Cyc_List_List* l2){(( void(*)( void(* f)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l1, struct Cyc_List_List* l2))
Cyc_Interface_Check_list)( Cyc_Interface_Check_tvar, l1, l2);} static void Cyc_Interface_Check_tqual(
struct Cyc_Absyn_Tqual* t1, struct Cyc_Absyn_Tqual* t2){;} static void Cyc_Interface_Check_structfield(
struct Cyc_Absyn_Structfield* s1, struct Cyc_Absyn_Structfield* s2){ Cyc_Interface_Check_strptr(
s1->name, s2->name); Cyc_Interface_Check_tqual( s1->tq, s2->tq);} static void
Cyc_Interface_Check_structfield_list( struct Cyc_List_List* l1, struct Cyc_List_List*
l2){(( void(*)( void(* f)( struct Cyc_Absyn_Structfield*, struct Cyc_Absyn_Structfield*),
struct Cyc_List_List* l1, struct Cyc_List_List* l2)) Cyc_Interface_Check_list)(
Cyc_Interface_Check_structfield, l1, l2);} static void Cyc_Interface_Check_structdecl(
struct Cyc_Absyn_Structdecl* d1, struct Cyc_Absyn_Structdecl* d2){(( void(*)(
void* x, void* y)) Cyc_Interface_Check_box)(( void*) d1->sc,( void*) d2->sc);
Cyc_Interface_Check_tvar_list( d1->tvs, d2->tvs);(( void(*)( void(* f)( struct
Cyc_List_List*, struct Cyc_List_List*), struct Cyc_Core_Opt* o1, struct Cyc_Core_Opt*
o2)) Cyc_Interface_Check_opt)( Cyc_Interface_Check_structfield_list, d1->fields,
d2->fields);} static void Cyc_Interface_Check_uniondecl( struct Cyc_Absyn_Uniondecl*
d1, struct Cyc_Absyn_Uniondecl* d2){;} static void Cyc_Interface_Check_tuniondecl(
struct Cyc_Absyn_Tuniondecl* d1, struct Cyc_Absyn_Tuniondecl* d2){;} static void
Cyc_Interface_Check_xtuniondecl( struct Cyc_Absyn_XTuniondecl* d1, struct Cyc_Absyn_XTuniondecl*
d2){;} static void Cyc_Interface_Check_enumdecl( struct Cyc_Absyn_Enumdecl* d1,
struct Cyc_Absyn_Enumdecl* d2){;} static void Cyc_Interface_Check_typedefdecl(
struct Cyc_Absyn_Typedefdecl* d1, struct Cyc_Absyn_Typedefdecl* d2){;} static
void Cyc_Interface_Check_vardecl( struct Cyc_Absyn_Vardecl* d1, struct Cyc_Absyn_Vardecl*
d2){;} static void Cyc_Interface_Check_incl_ns( struct Cyc_Interface_Ienv* ie1,
struct Cyc_Interface_Ienv* ie2){(( void(*)( void(* f)( struct Cyc_Absyn_Structdecl*,
struct Cyc_Absyn_Structdecl*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict*
d2)) Cyc_Dict_iter2)( Cyc_Interface_Check_structdecl, ie1->structdecls, ie2->structdecls);((
void(*)( void(* f)( struct Cyc_Absyn_Uniondecl*, struct Cyc_Absyn_Uniondecl*),
struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_iter2)( Cyc_Interface_Check_uniondecl,
ie1->uniondecls, ie2->uniondecls);(( void(*)( void(* f)( struct Cyc_Absyn_Tuniondecl*,
struct Cyc_Absyn_Tuniondecl*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict*
d2)) Cyc_Dict_iter2)( Cyc_Interface_Check_tuniondecl, ie1->tuniondecls, ie2->tuniondecls);((
void(*)( void(* f)( struct Cyc_Absyn_XTuniondecl*, struct Cyc_Absyn_XTuniondecl*),
struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_iter2)( Cyc_Interface_Check_xtuniondecl,
ie1->xtuniondecls, ie2->xtuniondecls);(( void(*)( void(* f)( struct Cyc_Absyn_Enumdecl*,
struct Cyc_Absyn_Enumdecl*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2))
Cyc_Dict_iter2)( Cyc_Interface_Check_enumdecl, ie1->enumdecls, ie2->enumdecls);((
void(*)( void(* f)( struct Cyc_Absyn_Typedefdecl*, struct Cyc_Absyn_Typedefdecl*),
struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_iter2)( Cyc_Interface_Check_typedefdecl,
ie1->typedefdecls, ie2->typedefdecls);(( void(*)( void(* f)( struct Cyc_Absyn_Vardecl*,
struct Cyc_Absyn_Vardecl*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2))
Cyc_Dict_iter2)( Cyc_Interface_Check_vardecl, ie1->vardecls, ie2->vardecls);}
int Cyc_Interface_is_subinterface( struct Cyc_Interface_T* i1, struct Cyc_Interface_T*
i2){{ struct _handler_cons _temp151; _push_handler(& _temp151);{ struct
_xtunion_struct* _temp152=( struct _xtunion_struct*) setjmp( _temp151.handler);
if( ! _temp152){(( void(*)( void(* f)( struct Cyc_Interface_Ienv*, struct Cyc_Interface_Ienv*),
struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_iter2)( Cyc_Interface_Check_incl_ns,
i1->publics, i2->publics);(( void(*)( void(* f)( struct Cyc_Interface_Ienv*,
struct Cyc_Interface_Ienv*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2))
Cyc_Dict_iter2)( Cyc_Interface_Check_incl_ns, i2->externs, i1->externs);;
_pop_handler();} else{ struct _xtunion_struct* _temp154= _temp152; _LL156: if(
_temp154->tag == Cyc_Interface_Check_False_tag){ goto _LL157;} else{ goto _LL158;}
_LL158: if( _temp154->tag == Cyc_Dict_Absent_tag){ goto _LL159;} else{ goto
_LL160;} _LL160: goto _LL161; _LL157: goto _LL159; _LL159: return 0; _LL161:(
void) _throw( _temp154); _LL155:;}}} return 1;} static struct Cyc_List_List* Cyc_Interface_add_namespace(
struct Cyc_List_List* tds, struct Cyc_List_List* ns){ if( ns == 0){ return tds;}
return({ struct Cyc_List_List* _temp162=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp162->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Namespace_d_struct* _temp163=( struct Cyc_Absyn_Namespace_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct)); _temp163[ 0]=({ struct
Cyc_Absyn_Namespace_d_struct _temp164; _temp164.tag= Cyc_Absyn_Namespace_d_tag;
_temp164.f1=( struct _tagged_string*)({ struct Cyc_List_List* _temp165= ns; if(
_temp165 == 0){ _throw( Null_Exception);} _temp165->hd;}); _temp164.f2= Cyc_Interface_add_namespace(
tds,({ struct Cyc_List_List* _temp166= ns; if( _temp166 == 0){ _throw(
Null_Exception);} _temp166->tl;})); _temp164;}); _temp163;}), 0); _temp162->tl=
0; _temp162;});} static struct Cyc_List_List* Cyc_Interface_add_structdecl(
struct _tagged_string* x, struct Cyc_Absyn_Structdecl* d, struct Cyc_List_List*
tds){ return({ struct Cyc_List_List* _temp167=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp167->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Struct_d_struct* _temp168=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp168[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp169; _temp169.tag= Cyc_Absyn_Struct_d_tag;
_temp169.f1= d; _temp169;}); _temp168;}), 0); _temp167->tl= tds; _temp167;});}
static struct Cyc_List_List* Cyc_Interface_add_structdecl_header( struct
_tagged_string* x, struct Cyc_Absyn_Structdecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Structdecl* _temp170=( struct Cyc_Absyn_Structdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp170[ 0]=* d; _temp170;}); d->fields=
0;( void*)( d->sc=( void*)(( void*) Cyc_Absyn_Extern)); return({ struct Cyc_List_List*
_temp171=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp171->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct*
_temp172=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp172[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp173; _temp173.tag= Cyc_Absyn_Struct_d_tag;
_temp173.f1= d; _temp173;}); _temp172;}), 0); _temp171->tl= tds; _temp171;});}
static struct Cyc_List_List* Cyc_Interface_add_uniondecl( struct _tagged_string*
x, struct Cyc_Absyn_Uniondecl* d, struct Cyc_List_List* tds){ return({ struct
Cyc_List_List* _temp174=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp174->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Union_d_struct*
_temp175=( struct Cyc_Absyn_Union_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct));
_temp175[ 0]=({ struct Cyc_Absyn_Union_d_struct _temp176; _temp176.tag= Cyc_Absyn_Union_d_tag;
_temp176.f1= d; _temp176;}); _temp175;}), 0); _temp174->tl= tds; _temp174;});}
static struct Cyc_List_List* Cyc_Interface_add_uniondecl_header( struct
_tagged_string* x, struct Cyc_Absyn_Uniondecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Uniondecl* _temp177=( struct Cyc_Absyn_Uniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Uniondecl)); _temp177[ 0]=* d; _temp177;}); d->fields=
0;( void*)( d->sc=( void*)(( void*) Cyc_Absyn_Extern)); return({ struct Cyc_List_List*
_temp178=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp178->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Union_d_struct*
_temp179=( struct Cyc_Absyn_Union_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct));
_temp179[ 0]=({ struct Cyc_Absyn_Union_d_struct _temp180; _temp180.tag= Cyc_Absyn_Union_d_tag;
_temp180.f1= d; _temp180;}); _temp179;}), 0); _temp178->tl= tds; _temp178;});}
static struct Cyc_List_List* Cyc_Interface_add_tuniondecl( struct _tagged_string*
x, struct Cyc_Absyn_Tuniondecl* d, struct Cyc_List_List* tds){ return({ struct
Cyc_List_List* _temp181=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp181->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct*
_temp182=( struct Cyc_Absyn_Tunion_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp182[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp183; _temp183.tag= Cyc_Absyn_Tunion_d_tag;
_temp183.f1= d; _temp183;}); _temp182;}), 0); _temp181->tl= tds; _temp181;});}
static struct Cyc_List_List* Cyc_Interface_add_tuniondecl_header( struct
_tagged_string* x, struct Cyc_Absyn_Tuniondecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Tuniondecl* _temp184=( struct Cyc_Absyn_Tuniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Tuniondecl)); _temp184[ 0]=* d; _temp184;}); d->fields=
0;( void*)( d->sc=( void*)(( void*) Cyc_Absyn_Extern)); return({ struct Cyc_List_List*
_temp185=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp185->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct*
_temp186=( struct Cyc_Absyn_Tunion_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp186[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp187; _temp187.tag= Cyc_Absyn_Tunion_d_tag;
_temp187.f1= d; _temp187;}); _temp186;}), 0); _temp185->tl= tds; _temp185;});}
static struct Cyc_List_List* Cyc_Interface_add_xtuniondecl( struct
_tagged_string* x, struct Cyc_Absyn_XTuniondecl* d, struct Cyc_List_List* tds){
return({ struct Cyc_List_List* _temp188=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp188->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_XTunion_d_struct* _temp189=( struct Cyc_Absyn_XTunion_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_XTunion_d_struct)); _temp189[ 0]=({ struct
Cyc_Absyn_XTunion_d_struct _temp190; _temp190.tag= Cyc_Absyn_XTunion_d_tag;
_temp190.f1= d; _temp190;}); _temp189;}), 0); _temp188->tl= tds; _temp188;});}
static struct Cyc_List_List* Cyc_Interface_add_xtuniondecl_header( struct
_tagged_string* x, struct Cyc_Absyn_XTuniondecl* d, struct Cyc_List_List* tds){
d=({ struct Cyc_Absyn_XTuniondecl* _temp191=( struct Cyc_Absyn_XTuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_XTuniondecl)); _temp191[ 0]=* d; _temp191;});
d->fields= 0;( void*)( d->sc=( void*)(( void*) Cyc_Absyn_Extern)); return({
struct Cyc_List_List* _temp192=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp192->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_XTunion_d_struct* _temp193=( struct Cyc_Absyn_XTunion_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_XTunion_d_struct)); _temp193[ 0]=({ struct
Cyc_Absyn_XTunion_d_struct _temp194; _temp194.tag= Cyc_Absyn_XTunion_d_tag;
_temp194.f1= d; _temp194;}); _temp193;}), 0); _temp192->tl= tds; _temp192;});}
static struct Cyc_List_List* Cyc_Interface_add_enumdecl( struct _tagged_string*
x, struct Cyc_Absyn_Enumdecl* d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List*
_temp195=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp195->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Enum_d_struct*
_temp196=( struct Cyc_Absyn_Enum_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct));
_temp196[ 0]=({ struct Cyc_Absyn_Enum_d_struct _temp197; _temp197.tag= Cyc_Absyn_Enum_d_tag;
_temp197.f1= d; _temp197;}); _temp196;}), 0); _temp195->tl= tds; _temp195;});}
static struct Cyc_List_List* Cyc_Interface_add_enumdecl_header( struct
_tagged_string* x, struct Cyc_Absyn_Enumdecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Enumdecl* _temp198=( struct Cyc_Absyn_Enumdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp198[ 0]=* d; _temp198;}); d->fields= 0;(
void*)( d->sc=( void*)(( void*) Cyc_Absyn_Extern)); return({ struct Cyc_List_List*
_temp199=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp199->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Enum_d_struct*
_temp200=( struct Cyc_Absyn_Enum_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct));
_temp200[ 0]=({ struct Cyc_Absyn_Enum_d_struct _temp201; _temp201.tag= Cyc_Absyn_Enum_d_tag;
_temp201.f1= d; _temp201;}); _temp200;}), 0); _temp199->tl= tds; _temp199;});}
static struct Cyc_List_List* Cyc_Interface_add_typedef( struct _tagged_string* x,
struct Cyc_Absyn_Typedefdecl* d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List*
_temp202=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp202->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Typedef_d_struct*
_temp203=( struct Cyc_Absyn_Typedef_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct));
_temp203[ 0]=({ struct Cyc_Absyn_Typedef_d_struct _temp204; _temp204.tag= Cyc_Absyn_Typedef_d_tag;
_temp204.f1= d; _temp204;}); _temp203;}), 0); _temp202->tl= tds; _temp202;});}
static struct Cyc_List_List* Cyc_Interface_add_var( struct _tagged_string* x,
struct Cyc_Absyn_Vardecl* d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List*
_temp205=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp205->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp206=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp206[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp207; _temp207.tag= Cyc_Absyn_Var_d_tag;
_temp207.f1= d; _temp207;}); _temp206;}), 0); _temp205->tl= tds; _temp205;});}
static void Cyc_Interface_print_ns_headers( struct Cyc_Stdio___sFILE* f, struct
Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie){ struct Cyc_List_List* tds= 0;
tds=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)( struct
_tagged_string*, struct Cyc_Absyn_Structdecl*, struct Cyc_List_List*), struct
Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_structdecl_header,
ie->structdecls, tds); tds=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_string*, struct Cyc_Absyn_Uniondecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_uniondecl_header,
ie->uniondecls, tds); tds=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)(
struct _tagged_string*, struct Cyc_Absyn_Tuniondecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_tuniondecl_header,
ie->tuniondecls, tds); tds=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_string*, struct Cyc_Absyn_XTuniondecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_xtuniondecl_header,
ie->xtuniondecls, tds); tds=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_string*, struct Cyc_Absyn_Enumdecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_enumdecl_header,
ie->enumdecls, tds); tds=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( tds); tds= Cyc_Interface_add_namespace( tds, ns); Cyc_Absynpp_decllist2file(
tds, f);} static void Cyc_Interface_print_ns_typedefs( struct Cyc_Stdio___sFILE*
f, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie){ struct Cyc_List_List*
tds= 0; tds=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)( struct
_tagged_string*, struct Cyc_Absyn_Typedefdecl*, struct Cyc_List_List*), struct
Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_typedef,
ie->typedefdecls, tds); tds=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( tds); tds= Cyc_Interface_add_namespace( tds, ns); Cyc_Absynpp_decllist2file(
tds, f);} static void Cyc_Interface_print_ns_decls( struct Cyc_Stdio___sFILE* f,
struct Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie){ struct Cyc_List_List*
tds= 0; tds=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)( struct
_tagged_string*, struct Cyc_Absyn_Structdecl*, struct Cyc_List_List*), struct
Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_structdecl,
ie->structdecls, tds); tds=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_string*, struct Cyc_Absyn_Uniondecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_uniondecl,
ie->uniondecls, tds); tds=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)(
struct _tagged_string*, struct Cyc_Absyn_Tuniondecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_tuniondecl,
ie->tuniondecls, tds); tds=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_string*, struct Cyc_Absyn_XTuniondecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_xtuniondecl,
ie->xtuniondecls, tds); tds=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_string*, struct Cyc_Absyn_Enumdecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_enumdecl,
ie->enumdecls, tds); tds=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)(
struct _tagged_string*, struct Cyc_Absyn_Vardecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_var,
ie->vardecls, tds); tds=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( tds); tds= Cyc_Interface_add_namespace( tds, ns); Cyc_Absynpp_decllist2file(
tds, f);} void Cyc_Interface_print( struct Cyc_Interface_T* i, struct Cyc_Stdio___sFILE*
f){ fprintf( f,"/****** needed (headers) ******/\n");(( void(*)( void(* f)(
struct Cyc_Stdio___sFILE*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*),
struct Cyc_Stdio___sFILE* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_headers,
f, i->externs); fprintf( f,"\n/****** provided (headers) ******/\n");(( void(*)(
void(* f)( struct Cyc_Stdio___sFILE*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*),
struct Cyc_Stdio___sFILE* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_headers,
f, i->publics); fprintf( f,"\n/****** provided (typedefs) ******/\n");(( void(*)(
void(* f)( struct Cyc_Stdio___sFILE*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*),
struct Cyc_Stdio___sFILE* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_typedefs,
f, i->publics); fprintf( f,"\n/****** needed (declarations) ******/\n");(( void(*)(
void(* f)( struct Cyc_Stdio___sFILE*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*),
struct Cyc_Stdio___sFILE* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_decls,
f, i->externs); fprintf( f,"\n/****** provided (declarations) ******/\n");((
void(*)( void(* f)( struct Cyc_Stdio___sFILE*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*),
struct Cyc_Stdio___sFILE* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_decls,
f, i->publics);} struct Cyc_Interface_T* Cyc_Interface_parse( struct Cyc_Stdio___sFILE*
f){ Cyc_Lex_lex_init();{ struct Cyc_List_List* tds= Cyc_Parse_parse_file( f);
Cyc_Absynpp_expand_typedefs= 0;{ struct Cyc_Tcenv_Tenv* te= Cyc_Tcenv_tc_init();
Cyc_Tc_tc( te, 0, tds); tds= Cyc_Tc_treeshake( te, tds); return Cyc_Interface_gen_extract(
te->ae, 0);}}}