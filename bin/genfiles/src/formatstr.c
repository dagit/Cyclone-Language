#include "cyc_include.h"

 struct _tuple0{ void* f1; struct _tagged_string* f2; } ; struct _tuple1{ struct
Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual* f2; void* f3; } ; struct _tuple2{
struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; } ; struct _tuple3{ struct
Cyc_List_List* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3; struct
Cyc_List_List* f4; char f5; int f6; } ; struct _tuple4{ int f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; char f4; int f5; } ; typedef int Cyc_ptrdiff_t;
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
Cyc_List_List* Cyc_List_List_t; extern char Cyc_List_List_empty_tag[ 11u];
struct Cyc_List_List_empty_struct{ char* tag; } ; extern char Cyc_List_List_mismatch_tag[
14u]; struct Cyc_List_List_mismatch_struct{ char* tag; } ; extern struct Cyc_List_List*
Cyc_List_imp_rev( struct Cyc_List_List* x); extern char Cyc_List_Nth_tag[ 4u];
struct Cyc_List_Nth_struct{ char* tag; } ; extern int Cyc_Character_isdigit(
char c); struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE;
typedef int Cyc_Stdio_fpos_t; extern char Cyc_Stdio_FileOpenError_tag[ 14u];
struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Stdio_FileCloseError_tag[ 15u]; struct Cyc_Stdio_FileCloseError_struct{
char* tag; } ; extern unsigned int Cyc_String_strlen( struct _tagged_string s);
extern struct _tagged_string Cyc_String_implode( struct Cyc_List_List* c);
struct Cyc_Lineno_Pos{ struct _tagged_string logical_file; struct _tagged_string
line; int line_no; int col; } ; typedef struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t;
extern char Cyc_Position_Exit_tag[ 5u]; struct Cyc_Position_Exit_struct{ char*
tag; } ; struct Cyc_Position_Segment; typedef struct Cyc_Position_Segment* Cyc_Position_seg_t;
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
char* tag; } ; extern struct Cyc_Absyn_Tqual* Cyc_Absyn_empty_tqual(); extern
void* Cyc_Absyn_new_evar( void*); extern void* Cyc_Absyn_uchar_t; extern void*
Cyc_Absyn_ushort_t; extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_ulong_t;
extern void* Cyc_Absyn_schar_t; extern void* Cyc_Absyn_sshort_t; extern void*
Cyc_Absyn_sint_t; extern void* Cyc_Absyn_slong_t; extern void* Cyc_Absyn_float_t;
extern void* Cyc_Absyn_double_t; extern void* Cyc_Absyn_string_typ( void* rgn);
extern void* Cyc_Absyn_at_typ( void* t, void* rgn, struct Cyc_Absyn_Tqual* tq);
struct Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct
Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t; extern
char Cyc_Set_Absent_tag[ 7u]; struct Cyc_Set_Absent_struct{ char* tag; } ;
struct Cyc_Dict_Dict; typedef struct Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef
struct Cyc_Dict_Dict* Cyc_Dict_dict_t; extern char Cyc_Dict_Present_tag[ 8u];
struct Cyc_Dict_Present_struct{ char* tag; } ; extern char Cyc_Dict_Absent_tag[
7u]; typedef void* Cyc_Tcenv_Resolved; static const int Cyc_Tcenv_VarRes_tag= 0;
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
tag; } ; extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*, struct
_tagged_string); extern struct Cyc_List_List* Cyc_Formatstr_get_format_typs(
struct _tagged_string, struct Cyc_Position_Segment*); extern struct Cyc_List_List*
Cyc_Formatstr_get_scanf_typs( struct _tagged_string, struct Cyc_Position_Segment*);
extern struct Cyc_Core_Opt* Cyc_Formatstr_parse_conversionspecification( struct
_tagged_string s, int i); struct Cyc_Core_Opt* Cyc_Formatstr_parse_conversionspecification(
struct _tagged_string s, int i){ unsigned int len= Cyc_String_strlen( s); if( i
< 0? 1: i >= len){ return 0;}{ struct Cyc_List_List* flags= 0; char c=' '; for(
0; i < len; i ++){ c=({ struct _tagged_string _temp0= s; char* _temp2= _temp0.curr
+ i; if( _temp0.base == 0? 1:( _temp2 < _temp0.base? 1: _temp2 >= _temp0.last_plus_one)){
_throw( Null_Exception);}* _temp2;}); switch( c){ case '+': _LL3: goto _LL4;
case '-': _LL4: goto _LL5; case ' ': _LL5: goto _LL6; case '#': _LL6: goto _LL7;
case '0': _LL7: flags=({ struct Cyc_List_List* _temp9=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp9->hd=( void*)(( int) c); _temp9->tl=
flags; _temp9;}); continue; default: _LL8: break;} break;} if( i >= len){ return
0;} flags=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
flags);{ struct Cyc_List_List* width= 0; c=({ struct _tagged_string _temp11= s;
char* _temp13= _temp11.curr + i; if( _temp11.base == 0? 1:( _temp13 < _temp11.base?
1: _temp13 >= _temp11.last_plus_one)){ _throw( Null_Exception);}* _temp13;});
if( c =='*'){ width=({ struct Cyc_List_List* _temp14=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp14->hd=( void*)(( int) c);
_temp14->tl= width; _temp14;}); i ++;} else{ for( 0; i < len; i ++){ c=({ struct
_tagged_string _temp15= s; char* _temp17= _temp15.curr + i; if( _temp15.base ==
0? 1:( _temp17 < _temp15.base? 1: _temp17 >= _temp15.last_plus_one)){ _throw(
Null_Exception);}* _temp17;}); if( Cyc_Character_isdigit( c)){ width=({ struct
Cyc_List_List* _temp18=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp18->hd=( void*)(( int) c); _temp18->tl= width; _temp18;});} else{ break;}}}
if( i >= len){ return 0;} width=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( width);{ struct Cyc_List_List* precision= 0; c=({ struct
_tagged_string _temp19= s; char* _temp21= _temp19.curr + i; if( _temp19.base ==
0? 1:( _temp21 < _temp19.base? 1: _temp21 >= _temp19.last_plus_one)){ _throw(
Null_Exception);}* _temp21;}); if( c =='.'){ precision=({ struct Cyc_List_List*
_temp22=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp22->hd=( void*)(( int) c); _temp22->tl= precision; _temp22;}); i ++; if( i
>= len){ return 0;} c=({ struct _tagged_string _temp23= s; char* _temp25=
_temp23.curr + i; if( _temp23.base == 0? 1:( _temp25 < _temp23.base? 1: _temp25
>= _temp23.last_plus_one)){ _throw( Null_Exception);}* _temp25;}); if( c =='*'){
precision=({ struct Cyc_List_List* _temp26=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp26->hd=( void*)(( int) c); _temp26->tl=
precision; _temp26;}); i ++;} else{ for( 0; i < len; i ++){ c=({ struct
_tagged_string _temp27= s; char* _temp29= _temp27.curr + i; if( _temp27.base ==
0? 1:( _temp29 < _temp27.base? 1: _temp29 >= _temp27.last_plus_one)){ _throw(
Null_Exception);}* _temp29;}); if( Cyc_Character_isdigit( c)){ precision=({
struct Cyc_List_List* _temp30=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp30->hd=( void*)(( int) c); _temp30->tl= precision; _temp30;});}
else{ break;}}}} if( i >= len){ return 0;} precision=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( precision);{ struct Cyc_List_List*
lenmod= 0; c=({ struct _tagged_string _temp31= s; char* _temp33= _temp31.curr +
i; if( _temp31.base == 0? 1:( _temp33 < _temp31.base? 1: _temp33 >= _temp31.last_plus_one)){
_throw( Null_Exception);}* _temp33;}); switch( c){ case 'h': _LL34: lenmod=({
struct Cyc_List_List* _temp36=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp36->hd=( void*)(( int) c); _temp36->tl= lenmod; _temp36;});
i ++; if( i >= len){ return 0;} c=({ struct _tagged_string _temp37= s; char*
_temp39= _temp37.curr + i; if( _temp37.base == 0? 1:( _temp39 < _temp37.base? 1:
_temp39 >= _temp37.last_plus_one)){ _throw( Null_Exception);}* _temp39;}); if( c
=='h'){ lenmod=({ struct Cyc_List_List* _temp40=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp40->hd=( void*)(( int) c);
_temp40->tl= lenmod; _temp40;}); i ++;} break; case 'l': _LL35: lenmod=({ struct
Cyc_List_List* _temp42=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp42->hd=( void*)(( int) c); _temp42->tl= lenmod; _temp42;}); i ++; if( i >=
len){ return 0;} c=({ struct _tagged_string _temp43= s; char* _temp45= _temp43.curr
+ i; if( _temp43.base == 0? 1:( _temp45 < _temp43.base? 1: _temp45 >= _temp43.last_plus_one)){
_throw( Null_Exception);}* _temp45;}); if( c =='l'){ lenmod=({ struct Cyc_List_List*
_temp46=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp46->hd=( void*)(( int) c); _temp46->tl= lenmod; _temp46;}); i ++;} break;
case 'j': _LL41: goto _LL47; case 'z': _LL47: goto _LL48; case 't': _LL48: goto
_LL49; case 'L': _LL49: lenmod=({ struct Cyc_List_List* _temp51=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp51->hd=( void*)(( int) c);
_temp51->tl= lenmod; _temp51;}); i ++; break; default: _LL50: break;} if( i >=
len){ return 0;} lenmod=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( lenmod); c=({ struct _tagged_string _temp53= s; char* _temp55=
_temp53.curr + i; if( _temp53.base == 0? 1:( _temp55 < _temp53.base? 1: _temp55
>= _temp53.last_plus_one)){ _throw( Null_Exception);}* _temp55;}); switch( c){
case 'd': _LL56: break; case 'i': _LL57: break; case 'o': _LL58: break; case 'u':
_LL59: break; case 'x': _LL60: break; case 'X': _LL61: break; case 'f': _LL62:
break; case 'F': _LL63: break; case 'e': _LL64: break; case 'E': _LL65: break;
case 'g': _LL66: break; case 'G': _LL67: break; case 'a': _LL68: break; case 'A':
_LL69: break; case 'c': _LL70: break; case 's': _LL71: break; case 'p': _LL72:
break; case 'n': _LL73: break; case '%': _LL74: break; default: _LL75: return 0;}
return({ struct Cyc_Core_Opt* _temp77=( struct Cyc_Core_Opt*) GC_malloc( sizeof(
struct Cyc_Core_Opt)); _temp77->v=( void*)({ struct _tuple3* _temp78=( struct
_tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp78->f1= flags; _temp78->f2=
width; _temp78->f3= precision; _temp78->f4= lenmod; _temp78->f5= c; _temp78->f6=
i + 1; _temp78;}); _temp77;});}}}}} struct Cyc_List_List* Cyc_Formatstr_get_format_typs(
struct _tagged_string s, struct Cyc_Position_Segment* loc){ unsigned int len=
Cyc_String_strlen( s); struct Cyc_List_List* typs= 0; int i; for( i= 0; i < len;
i ++){ if(({ struct _tagged_string _temp79= s; char* _temp81= _temp79.curr + i;
if( _temp79.base == 0? 1:( _temp81 < _temp79.base? 1: _temp81 >= _temp79.last_plus_one)){
_throw( Null_Exception);}* _temp81;}) !='%'){ continue;}{ struct Cyc_Core_Opt*
cs= Cyc_Formatstr_parse_conversionspecification( s, i + 1); if( cs == 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp82=( char*)"bad format string"; struct
_tagged_string _temp83; _temp83.curr= _temp82; _temp83.base= _temp82; _temp83.last_plus_one=
_temp82 + 18; _temp83;})); return 0;}{ struct _tuple3 _temp87; int _temp88; char
_temp90; struct Cyc_List_List* _temp92; struct Cyc_List_List* _temp94; struct
Cyc_List_List* _temp96; struct Cyc_List_List* _temp98; struct _tuple3* _temp85=(
struct _tuple3*)({ struct Cyc_Core_Opt* _temp84= cs; if( _temp84 == 0){ _throw(
Null_Exception);} _temp84->v;}); _temp87=* _temp85; _LL99: _temp98= _temp87.f1;
goto _LL97; _LL97: _temp96= _temp87.f2; goto _LL95; _LL95: _temp94= _temp87.f3;
goto _LL93; _LL93: _temp92= _temp87.f4; goto _LL91; _LL91: _temp90= _temp87.f5;
goto _LL89; _LL89: _temp88= _temp87.f6; goto _LL86; _LL86: i= _temp88 - 1;{
struct Cyc_List_List* _temp100= _temp92; struct Cyc_List_List _temp107; struct
Cyc_List_List* _temp108; int _temp110; _LL102: if( _temp100 == 0){ goto _LL104;}
else{ _temp107=* _temp100; _LL111: _temp110=( int) _temp107.hd; goto _LL109;
_LL109: _temp108=( struct Cyc_List_List*) _temp107.tl; if( _temp108 == 0){ goto
_LL106;} else{ goto _LL104;}} _LL106: if(( _temp110 =='j'? 1: _temp110 =='z')? 1:
_temp110 =='t'){ goto _LL103;} else{ goto _LL104;} _LL104: goto _LL105; _LL103:
Cyc_Tcutil_terr( loc, xprintf("length modifier '%c' is not supported", _temp110));
return 0; _LL105: goto _LL101; _LL101:;}{ struct Cyc_List_List* _temp112=
_temp96; struct Cyc_List_List _temp119; struct Cyc_List_List* _temp120; int
_temp122; _LL114: if( _temp112 == 0){ goto _LL116;} else{ _temp119=* _temp112;
_LL123: _temp122=( int) _temp119.hd; goto _LL121; _LL121: _temp120=( struct Cyc_List_List*)
_temp119.tl; if( _temp120 == 0){ goto _LL118;} else{ goto _LL116;}} _LL118: if(
_temp122 =='*'){ goto _LL115;} else{ goto _LL116;} _LL116: goto _LL117; _LL115:
typs=({ struct Cyc_List_List* _temp124=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp124->hd=( void*) Cyc_Absyn_sint_t; _temp124->tl=
typs; _temp124;}); goto _LL113; _LL117: goto _LL113; _LL113:;}{ struct Cyc_List_List*
_temp125= _temp94; struct Cyc_List_List _temp132; struct Cyc_List_List* _temp133;
struct Cyc_List_List _temp135; struct Cyc_List_List* _temp136; int _temp138; int
_temp140; _LL127: if( _temp125 == 0){ goto _LL129;} else{ _temp132=* _temp125;
_LL141: _temp140=( int) _temp132.hd; goto _LL134; _LL134: _temp133=( struct Cyc_List_List*)
_temp132.tl; if( _temp133 == 0){ goto _LL129;} else{ _temp135=* _temp133; _LL139:
_temp138=( int) _temp135.hd; goto _LL137; _LL137: _temp136=( struct Cyc_List_List*)
_temp135.tl; if( _temp136 == 0){ goto _LL131;} else{ goto _LL129;}}} _LL131: if(
_temp140 =='.'? _temp138 =='*': 0){ goto _LL128;} else{ goto _LL129;} _LL129:
goto _LL130; _LL128: typs=({ struct Cyc_List_List* _temp142=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp142->hd=( void*) Cyc_Absyn_sint_t;
_temp142->tl= typs; _temp142;}); goto _LL126; _LL130: goto _LL126; _LL126:;}{
void* t; switch( _temp90){ case 'd': _LL143: goto _LL144; case 'i': _LL144:{
struct Cyc_List_List* f= _temp98; for( 0; f != 0; f=({ struct Cyc_List_List*
_temp146= f; if( _temp146 == 0){ _throw( Null_Exception);} _temp146->tl;})){ if((
int)({ struct Cyc_List_List* _temp147= f; if( _temp147 == 0){ _throw(
Null_Exception);} _temp147->hd;}) =='#'){ Cyc_Tcutil_terr( loc, xprintf("flag '#' is not valid with %%%c",(
int) _temp90)); return 0;}}}{ struct Cyc_List_List* _temp148= _temp92; struct
Cyc_List_List _temp161; struct Cyc_List_List* _temp162; int _temp164; struct Cyc_List_List
_temp167; struct Cyc_List_List* _temp168; int _temp170; struct Cyc_List_List
_temp173; struct Cyc_List_List* _temp174; struct Cyc_List_List _temp176; struct
Cyc_List_List* _temp177; int _temp179; int _temp181; _LL150: if( _temp148 == 0){
goto _LL151;} else{ goto _LL152;} _LL152: if( _temp148 == 0){ goto _LL154;}
else{ _temp161=* _temp148; _LL165: _temp164=( int) _temp161.hd; goto _LL163;
_LL163: _temp162=( struct Cyc_List_List*) _temp161.tl; if( _temp162 == 0){ goto
_LL160;} else{ goto _LL154;}} _LL160: if( _temp164 =='l'){ goto _LL153;} else{
goto _LL154;} _LL154: if( _temp148 == 0){ goto _LL156;} else{ _temp167=*
_temp148; _LL171: _temp170=( int) _temp167.hd; goto _LL169; _LL169: _temp168=(
struct Cyc_List_List*) _temp167.tl; if( _temp168 == 0){ goto _LL166;} else{ goto
_LL156;}} _LL166: if( _temp170 =='h'){ goto _LL155;} else{ goto _LL156;} _LL156:
if( _temp148 == 0){ goto _LL158;} else{ _temp173=* _temp148; _LL182: _temp181=(
int) _temp173.hd; goto _LL175; _LL175: _temp174=( struct Cyc_List_List*)
_temp173.tl; if( _temp174 == 0){ goto _LL158;} else{ _temp176=* _temp174; _LL180:
_temp179=( int) _temp176.hd; goto _LL178; _LL178: _temp177=( struct Cyc_List_List*)
_temp176.tl; if( _temp177 == 0){ goto _LL172;} else{ goto _LL158;}}} _LL172: if(
_temp181 =='h'? _temp179 =='h': 0){ goto _LL157;} else{ goto _LL158;} _LL158:
goto _LL159; _LL151: t= Cyc_Absyn_sint_t; goto _LL149; _LL153: t= Cyc_Absyn_slong_t;
goto _LL149; _LL155: t= Cyc_Absyn_sshort_t; goto _LL149; _LL157: t= Cyc_Absyn_schar_t;
goto _LL149; _LL159: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp183= Cyc_String_implode(
_temp92); int _temp184=( int) _temp90; xprintf("length modifier '%.*s' is not allowed with %%%c",
_temp183.last_plus_one - _temp183.curr, _temp183.curr, _temp184);})); return 0;
_LL149:;} typs=({ struct Cyc_List_List* _temp185=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp185->hd=( void*) t; _temp185->tl=
typs; _temp185;}); break; case 'u': _LL145:{ struct Cyc_List_List* f= _temp98;
for( 0; f != 0; f=({ struct Cyc_List_List* _temp187= f; if( _temp187 == 0){
_throw( Null_Exception);} _temp187->tl;})){ if(( int)({ struct Cyc_List_List*
_temp188= f; if( _temp188 == 0){ _throw( Null_Exception);} _temp188->hd;}) =='#'){
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp189=( char*)"Flag '#' not valid with %%u";
struct _tagged_string _temp190; _temp190.curr= _temp189; _temp190.base= _temp189;
_temp190.last_plus_one= _temp189 + 28; _temp190;})); return 0;}}} goto _LL186;
case 'o': _LL186: goto _LL191; case 'x': _LL191: goto _LL192; case 'X': _LL192:{
struct Cyc_List_List* _temp194= _temp92; struct Cyc_List_List _temp207; struct
Cyc_List_List* _temp208; int _temp210; struct Cyc_List_List _temp213; struct Cyc_List_List*
_temp214; int _temp216; struct Cyc_List_List _temp219; struct Cyc_List_List*
_temp220; struct Cyc_List_List _temp222; struct Cyc_List_List* _temp223; int
_temp225; int _temp227; _LL196: if( _temp194 == 0){ goto _LL197;} else{ goto
_LL198;} _LL198: if( _temp194 == 0){ goto _LL200;} else{ _temp207=* _temp194;
_LL211: _temp210=( int) _temp207.hd; goto _LL209; _LL209: _temp208=( struct Cyc_List_List*)
_temp207.tl; if( _temp208 == 0){ goto _LL206;} else{ goto _LL200;}} _LL206: if(
_temp210 =='l'){ goto _LL199;} else{ goto _LL200;} _LL200: if( _temp194 == 0){
goto _LL202;} else{ _temp213=* _temp194; _LL217: _temp216=( int) _temp213.hd;
goto _LL215; _LL215: _temp214=( struct Cyc_List_List*) _temp213.tl; if( _temp214
== 0){ goto _LL212;} else{ goto _LL202;}} _LL212: if( _temp216 =='h'){ goto
_LL201;} else{ goto _LL202;} _LL202: if( _temp194 == 0){ goto _LL204;} else{
_temp219=* _temp194; _LL228: _temp227=( int) _temp219.hd; goto _LL221; _LL221:
_temp220=( struct Cyc_List_List*) _temp219.tl; if( _temp220 == 0){ goto _LL204;}
else{ _temp222=* _temp220; _LL226: _temp225=( int) _temp222.hd; goto _LL224;
_LL224: _temp223=( struct Cyc_List_List*) _temp222.tl; if( _temp223 == 0){ goto
_LL218;} else{ goto _LL204;}}} _LL218: if( _temp227 =='h'? _temp225 =='h': 0){
goto _LL203;} else{ goto _LL204;} _LL204: goto _LL205; _LL197: t= Cyc_Absyn_uint_t;
goto _LL195; _LL199: t= Cyc_Absyn_ulong_t; goto _LL195; _LL201: t= Cyc_Absyn_ushort_t;
goto _LL195; _LL203: t= Cyc_Absyn_uchar_t; goto _LL195; _LL205: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp229= Cyc_String_implode( _temp92); int
_temp230=( int) _temp90; xprintf("length modifier '%.*s' is not allowed with %%%c",
_temp229.last_plus_one - _temp229.curr, _temp229.curr, _temp230);})); return 0;
_LL195:;} typs=({ struct Cyc_List_List* _temp231=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp231->hd=( void*) t; _temp231->tl=
typs; _temp231;}); break; case 'f': _LL193: goto _LL232; case 'F': _LL232: goto
_LL233; case 'e': _LL233: goto _LL234; case 'E': _LL234: goto _LL235; case 'g':
_LL235: goto _LL236; case 'G': _LL236: goto _LL237; case 'a': _LL237: goto
_LL238; case 'A': _LL238: if( _temp92 != 0){ Cyc_Tcutil_terr( loc, xprintf("length modifiers are not allowed with %%%c",(
int) _temp90)); return 0;} typs=({ struct Cyc_List_List* _temp240=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp240->hd=( void*) Cyc_Absyn_double_t;
_temp240->tl= typs; _temp240;}); break; case 'c': _LL239:{ struct Cyc_List_List*
f= _temp98; for( 0; f != 0; f=({ struct Cyc_List_List* _temp242= f; if( _temp242
== 0){ _throw( Null_Exception);} _temp242->tl;})){ if(( int)({ struct Cyc_List_List*
_temp243= f; if( _temp243 == 0){ _throw( Null_Exception);} _temp243->hd;}) =='#'?
1:( int)({ struct Cyc_List_List* _temp244= f; if( _temp244 == 0){ _throw(
Null_Exception);} _temp244->hd;}) =='0'){ Cyc_Tcutil_terr( loc, xprintf("flag '%c' not allowed with %%c",(
int)({ struct Cyc_List_List* _temp245= f; if( _temp245 == 0){ _throw(
Null_Exception);} _temp245->hd;}))); return 0;}}} if( _temp92 != 0){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp246= Cyc_String_implode( _temp92); xprintf("length modifier '%.*s' not allowed with %%c",
_temp246.last_plus_one - _temp246.curr, _temp246.curr);})); return 0;} if(
_temp94 != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp247= Cyc_String_implode(
_temp94); xprintf("precision '%.*s' not allowed with %%c", _temp247.last_plus_one
- _temp247.curr, _temp247.curr);})); return 0;} typs=({ struct Cyc_List_List*
_temp248=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp248->hd=( void*) Cyc_Absyn_sint_t; _temp248->tl= typs; _temp248;}); break;
case 's': _LL241: if( _temp98 != 0){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp250= Cyc_String_implode( _temp98); xprintf("flags '%.*s' not allowed with %%s",
_temp250.last_plus_one - _temp250.curr, _temp250.curr);})); return 0;} if(
_temp96 != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp251= Cyc_String_implode(
_temp96); xprintf("width '%.*s' not allowed with %%s", _temp251.last_plus_one -
_temp251.curr, _temp251.curr);})); return 0;} if( _temp94 != 0){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp252= Cyc_String_implode( _temp94); xprintf("precision '%.*s' not allowed with %%s",
_temp252.last_plus_one - _temp252.curr, _temp252.curr);})); return 0;} if(
_temp92 != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp253= Cyc_String_implode(
_temp92); xprintf("length modifier '%.*s' not allowed with %%s", _temp253.last_plus_one
- _temp253.curr, _temp253.curr);})); return 0;} typs=({ struct Cyc_List_List*
_temp254=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp254->hd=( void*) Cyc_Absyn_string_typ( Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind));
_temp254->tl= typs; _temp254;}); break; case 'p': _LL249: Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp256=( char*)"%%p not supported"; struct
_tagged_string _temp257; _temp257.curr= _temp256; _temp257.base= _temp256;
_temp257.last_plus_one= _temp256 + 18; _temp257;})); return 0; case 'n': _LL255:{
struct Cyc_List_List* f= _temp98; for( 0; f != 0; f=({ struct Cyc_List_List*
_temp259= f; if( _temp259 == 0){ _throw( Null_Exception);} _temp259->tl;})){ if((
int)({ struct Cyc_List_List* _temp260= f; if( _temp260 == 0){ _throw(
Null_Exception);} _temp260->hd;}) =='#'? 1:( int)({ struct Cyc_List_List*
_temp261= f; if( _temp261 == 0){ _throw( Null_Exception);} _temp261->hd;}) =='0'){
Cyc_Tcutil_terr( loc, xprintf("flag '%c' not allowed with %%n",( int)({ struct
Cyc_List_List* _temp262= f; if( _temp262 == 0){ _throw( Null_Exception);}
_temp262->hd;}))); return 0;}}} if( _temp94 != 0){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp263= Cyc_String_implode( _temp94); xprintf("precision '%.*s' not allowed with %%n",
_temp263.last_plus_one - _temp263.curr, _temp263.curr);})); return 0;}{ struct
Cyc_List_List* _temp264= _temp92; struct Cyc_List_List _temp277; struct Cyc_List_List*
_temp278; int _temp280; struct Cyc_List_List _temp283; struct Cyc_List_List*
_temp284; int _temp286; struct Cyc_List_List _temp289; struct Cyc_List_List*
_temp290; struct Cyc_List_List _temp292; struct Cyc_List_List* _temp293; int
_temp295; int _temp297; _LL266: if( _temp264 == 0){ goto _LL267;} else{ goto
_LL268;} _LL268: if( _temp264 == 0){ goto _LL270;} else{ _temp277=* _temp264;
_LL281: _temp280=( int) _temp277.hd; goto _LL279; _LL279: _temp278=( struct Cyc_List_List*)
_temp277.tl; if( _temp278 == 0){ goto _LL276;} else{ goto _LL270;}} _LL276: if(
_temp280 =='l'){ goto _LL269;} else{ goto _LL270;} _LL270: if( _temp264 == 0){
goto _LL272;} else{ _temp283=* _temp264; _LL287: _temp286=( int) _temp283.hd;
goto _LL285; _LL285: _temp284=( struct Cyc_List_List*) _temp283.tl; if( _temp284
== 0){ goto _LL282;} else{ goto _LL272;}} _LL282: if( _temp286 =='h'){ goto
_LL271;} else{ goto _LL272;} _LL272: if( _temp264 == 0){ goto _LL274;} else{
_temp289=* _temp264; _LL298: _temp297=( int) _temp289.hd; goto _LL291; _LL291:
_temp290=( struct Cyc_List_List*) _temp289.tl; if( _temp290 == 0){ goto _LL274;}
else{ _temp292=* _temp290; _LL296: _temp295=( int) _temp292.hd; goto _LL294;
_LL294: _temp293=( struct Cyc_List_List*) _temp292.tl; if( _temp293 == 0){ goto
_LL288;} else{ goto _LL274;}}} _LL288: if( _temp297 =='h'? _temp295 =='h': 0){
goto _LL273;} else{ goto _LL274;} _LL274: goto _LL275; _LL267: t= Cyc_Absyn_at_typ(
Cyc_Absyn_sint_t, Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual());
goto _LL265; _LL269: t= Cyc_Absyn_at_typ( Cyc_Absyn_ulong_t, Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual()); goto _LL265; _LL271: t= Cyc_Absyn_at_typ(
Cyc_Absyn_sshort_t, Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual());
goto _LL265; _LL273: t= Cyc_Absyn_at_typ( Cyc_Absyn_schar_t, Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual()); goto _LL265; _LL275: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp299= Cyc_String_implode( _temp92); xprintf("length modifier '%.*s' not allowed with %%n",
_temp299.last_plus_one - _temp299.curr, _temp299.curr);})); return 0; _LL265:;}
typs=({ struct Cyc_List_List* _temp300=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp300->hd=( void*) t; _temp300->tl= typs;
_temp300;}); break; case '%': _LL258: if( _temp98 != 0){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp302= Cyc_String_implode( _temp98); xprintf("flags '%.*s' not allowed with %%%%",
_temp302.last_plus_one - _temp302.curr, _temp302.curr);})); return 0;} if(
_temp96 != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp303= Cyc_String_implode(
_temp96); xprintf("width '%.*s' not allowed with %%%%", _temp303.last_plus_one -
_temp303.curr, _temp303.curr);})); return 0;} if( _temp94 != 0){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp304= Cyc_String_implode( _temp94); xprintf("precision '%.*s' not allowed with %%%%",
_temp304.last_plus_one - _temp304.curr, _temp304.curr);})); return 0;} if(
_temp92 != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp305= Cyc_String_implode(
_temp92); xprintf("length modifier '%.*s' not allowed with %%%%", _temp305.last_plus_one
- _temp305.curr, _temp305.curr);})); return 0;} break; default: _LL301: return 0;}}}}}
return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
typs);} struct Cyc_Core_Opt* Cyc_Formatstr_parse_inputformat( struct
_tagged_string s, int i){ unsigned int len= Cyc_String_strlen( s); if( i < 0? 1:
i >= len){ return 0;}{ int suppress= 0; char c=({ struct _tagged_string _temp307=
s; char* _temp309= _temp307.curr + i; if( _temp307.base == 0? 1:( _temp309 <
_temp307.base? 1: _temp309 >= _temp307.last_plus_one)){ _throw( Null_Exception);}*
_temp309;}); if( c =='*'){ suppress= 1; i ++; if( i >= len){ return 0;}}{ struct
Cyc_List_List* width= 0; for( 0; i < len; i ++){ c=({ struct _tagged_string
_temp310= s; char* _temp312= _temp310.curr + i; if( _temp310.base == 0? 1:(
_temp312 < _temp310.base? 1: _temp312 >= _temp310.last_plus_one)){ _throw(
Null_Exception);}* _temp312;}); if( Cyc_Character_isdigit( c)){ width=({ struct
Cyc_List_List* _temp313=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp313->hd=( void*)(( int) c); _temp313->tl= width; _temp313;});} else{ break;}}
if( i >= len){ return 0;} width=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( width);{ struct Cyc_List_List* lenmod= 0; c=({ struct
_tagged_string _temp314= s; char* _temp316= _temp314.curr + i; if( _temp314.base
== 0? 1:( _temp316 < _temp314.base? 1: _temp316 >= _temp314.last_plus_one)){
_throw( Null_Exception);}* _temp316;}); switch( c){ case 'h': _LL317: lenmod=({
struct Cyc_List_List* _temp319=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp319->hd=( void*)(( int) c); _temp319->tl= lenmod;
_temp319;}); i ++; if( i >= len){ return 0;} c=({ struct _tagged_string _temp320=
s; char* _temp322= _temp320.curr + i; if( _temp320.base == 0? 1:( _temp322 <
_temp320.base? 1: _temp322 >= _temp320.last_plus_one)){ _throw( Null_Exception);}*
_temp322;}); if( c =='h'){ lenmod=({ struct Cyc_List_List* _temp323=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp323->hd=( void*)(( int) c);
_temp323->tl= lenmod; _temp323;}); i ++;} break; case 'l': _LL318: lenmod=({
struct Cyc_List_List* _temp325=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp325->hd=( void*)(( int) c); _temp325->tl= lenmod;
_temp325;}); i ++; if( i >= len){ return 0;} c=({ struct _tagged_string _temp326=
s; char* _temp328= _temp326.curr + i; if( _temp326.base == 0? 1:( _temp328 <
_temp326.base? 1: _temp328 >= _temp326.last_plus_one)){ _throw( Null_Exception);}*
_temp328;}); if( c =='l'){ lenmod=({ struct Cyc_List_List* _temp329=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp329->hd=( void*)(( int) c);
_temp329->tl= lenmod; _temp329;}); i ++;} break; case 'j': _LL324: goto _LL330;
case 'z': _LL330: goto _LL331; case 't': _LL331: goto _LL332; case 'L': _LL332:
lenmod=({ struct Cyc_List_List* _temp334=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp334->hd=( void*)(( int) c); _temp334->tl=
lenmod; _temp334;}); i ++; break; default: _LL333: break;} if( i >= len){ return
0;} lenmod=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
lenmod); c=({ struct _tagged_string _temp336= s; char* _temp338= _temp336.curr +
i; if( _temp336.base == 0? 1:( _temp338 < _temp336.base? 1: _temp338 >= _temp336.last_plus_one)){
_throw( Null_Exception);}* _temp338;}); switch( c){ case 'd': _LL339: break;
case 'i': _LL340: break; case 'o': _LL341: break; case 'u': _LL342: break; case
'x': _LL343: break; case 'X': _LL344: break; case 'f': _LL345: break; case 'F':
_LL346: break; case 'e': _LL347: break; case 'E': _LL348: break; case 'g':
_LL349: break; case 'G': _LL350: break; case 'a': _LL351: break; case 'A':
_LL352: break; case 'c': _LL353: break; case 's': _LL354: break; case 'p':
_LL355: break; case 'n': _LL356: break; case '%': _LL357: break; default: _LL358:
return 0;} return({ struct Cyc_Core_Opt* _temp360=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp360->v=( void*)({ struct _tuple4*
_temp361=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp361->f1=
suppress; _temp361->f2= width; _temp361->f3= lenmod; _temp361->f4= c; _temp361->f5=
i + 1; _temp361;}); _temp360;});}}}} struct Cyc_List_List* Cyc_Formatstr_get_scanf_typs(
struct _tagged_string s, struct Cyc_Position_Segment* loc){ unsigned int len=
Cyc_String_strlen( s); struct Cyc_List_List* typs= 0; int i; for( i= 0; i < len;
i ++){ if(({ struct _tagged_string _temp362= s; char* _temp364= _temp362.curr +
i; if( _temp362.base == 0? 1:( _temp364 < _temp362.base? 1: _temp364 >= _temp362.last_plus_one)){
_throw( Null_Exception);}* _temp364;}) !='%'){ continue;}{ struct Cyc_Core_Opt*
x= Cyc_Formatstr_parse_inputformat( s, i + 1); if( x == 0){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp365=( char*)"bad format string"; struct
_tagged_string _temp366; _temp366.curr= _temp365; _temp366.base= _temp365;
_temp366.last_plus_one= _temp365 + 18; _temp366;})); return 0;}{ struct _tuple4
_temp370; int _temp371; char _temp373; struct Cyc_List_List* _temp375; struct
Cyc_List_List* _temp377; int _temp379; struct _tuple4* _temp368=( struct _tuple4*)({
struct Cyc_Core_Opt* _temp367= x; if( _temp367 == 0){ _throw( Null_Exception);}
_temp367->v;}); _temp370=* _temp368; _LL380: _temp379= _temp370.f1; goto _LL378;
_LL378: _temp377= _temp370.f2; goto _LL376; _LL376: _temp375= _temp370.f3; goto
_LL374; _LL374: _temp373= _temp370.f4; goto _LL372; _LL372: _temp371= _temp370.f5;
goto _LL369; _LL369: i= _temp371 - 1;{ struct Cyc_List_List* _temp381= _temp375;
struct Cyc_List_List _temp388; struct Cyc_List_List* _temp389; int _temp391;
_LL383: if( _temp381 == 0){ goto _LL385;} else{ _temp388=* _temp381; _LL392:
_temp391=( int) _temp388.hd; goto _LL390; _LL390: _temp389=( struct Cyc_List_List*)
_temp388.tl; if( _temp389 == 0){ goto _LL387;} else{ goto _LL385;}} _LL387: if((
_temp391 =='j'? 1: _temp391 =='z')? 1: _temp391 =='t'){ goto _LL384;} else{ goto
_LL385;} _LL385: goto _LL386; _LL384: Cyc_Tcutil_terr( loc, xprintf("length modifier '%c' is not supported",
_temp391)); return 0; _LL386: goto _LL382; _LL382:;} if( _temp379){ continue;}{
void* t; switch( _temp373){ case 'd': _LL393: goto _LL394; case 'i': _LL394:{
struct Cyc_List_List* _temp396= _temp375; struct Cyc_List_List _temp409; struct
Cyc_List_List* _temp410; int _temp412; struct Cyc_List_List _temp415; struct Cyc_List_List*
_temp416; int _temp418; struct Cyc_List_List _temp421; struct Cyc_List_List*
_temp422; struct Cyc_List_List _temp424; struct Cyc_List_List* _temp425; int
_temp427; int _temp429; _LL398: if( _temp396 == 0){ goto _LL399;} else{ goto
_LL400;} _LL400: if( _temp396 == 0){ goto _LL402;} else{ _temp409=* _temp396;
_LL413: _temp412=( int) _temp409.hd; goto _LL411; _LL411: _temp410=( struct Cyc_List_List*)
_temp409.tl; if( _temp410 == 0){ goto _LL408;} else{ goto _LL402;}} _LL408: if(
_temp412 =='l'){ goto _LL401;} else{ goto _LL402;} _LL402: if( _temp396 == 0){
goto _LL404;} else{ _temp415=* _temp396; _LL419: _temp418=( int) _temp415.hd;
goto _LL417; _LL417: _temp416=( struct Cyc_List_List*) _temp415.tl; if( _temp416
== 0){ goto _LL414;} else{ goto _LL404;}} _LL414: if( _temp418 =='h'){ goto
_LL403;} else{ goto _LL404;} _LL404: if( _temp396 == 0){ goto _LL406;} else{
_temp421=* _temp396; _LL430: _temp429=( int) _temp421.hd; goto _LL423; _LL423:
_temp422=( struct Cyc_List_List*) _temp421.tl; if( _temp422 == 0){ goto _LL406;}
else{ _temp424=* _temp422; _LL428: _temp427=( int) _temp424.hd; goto _LL426;
_LL426: _temp425=( struct Cyc_List_List*) _temp424.tl; if( _temp425 == 0){ goto
_LL420;} else{ goto _LL406;}}} _LL420: if( _temp429 =='h'? _temp427 =='h': 0){
goto _LL405;} else{ goto _LL406;} _LL406: goto _LL407; _LL399: t= Cyc_Absyn_at_typ(
Cyc_Absyn_sint_t, Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual());
goto _LL397; _LL401: t= Cyc_Absyn_at_typ( Cyc_Absyn_slong_t, Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual()); goto _LL397; _LL403: t= Cyc_Absyn_at_typ(
Cyc_Absyn_sshort_t, Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual());
goto _LL397; _LL405: t= Cyc_Absyn_at_typ( Cyc_Absyn_schar_t, Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual()); goto _LL397; _LL407: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp431= Cyc_String_implode( _temp375); int
_temp432=( int) _temp373; xprintf("length modifier '%.*s' is not allowed with %%%c",
_temp431.last_plus_one - _temp431.curr, _temp431.curr, _temp432);})); return 0;
_LL397:;} typs=({ struct Cyc_List_List* _temp433=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp433->hd=( void*) t; _temp433->tl=
typs; _temp433;}); break; case 'u': _LL395: goto _LL434; case 'o': _LL434: goto
_LL435; case 'x': _LL435: goto _LL436; case 'X': _LL436:{ struct Cyc_List_List*
_temp438= _temp375; struct Cyc_List_List _temp451; struct Cyc_List_List*
_temp452; int _temp454; struct Cyc_List_List _temp457; struct Cyc_List_List*
_temp458; int _temp460; struct Cyc_List_List _temp463; struct Cyc_List_List*
_temp464; struct Cyc_List_List _temp466; struct Cyc_List_List* _temp467; int
_temp469; int _temp471; _LL440: if( _temp438 == 0){ goto _LL441;} else{ goto
_LL442;} _LL442: if( _temp438 == 0){ goto _LL444;} else{ _temp451=* _temp438;
_LL455: _temp454=( int) _temp451.hd; goto _LL453; _LL453: _temp452=( struct Cyc_List_List*)
_temp451.tl; if( _temp452 == 0){ goto _LL450;} else{ goto _LL444;}} _LL450: if(
_temp454 =='l'){ goto _LL443;} else{ goto _LL444;} _LL444: if( _temp438 == 0){
goto _LL446;} else{ _temp457=* _temp438; _LL461: _temp460=( int) _temp457.hd;
goto _LL459; _LL459: _temp458=( struct Cyc_List_List*) _temp457.tl; if( _temp458
== 0){ goto _LL456;} else{ goto _LL446;}} _LL456: if( _temp460 =='h'){ goto
_LL445;} else{ goto _LL446;} _LL446: if( _temp438 == 0){ goto _LL448;} else{
_temp463=* _temp438; _LL472: _temp471=( int) _temp463.hd; goto _LL465; _LL465:
_temp464=( struct Cyc_List_List*) _temp463.tl; if( _temp464 == 0){ goto _LL448;}
else{ _temp466=* _temp464; _LL470: _temp469=( int) _temp466.hd; goto _LL468;
_LL468: _temp467=( struct Cyc_List_List*) _temp466.tl; if( _temp467 == 0){ goto
_LL462;} else{ goto _LL448;}}} _LL462: if( _temp471 =='h'? _temp469 =='h': 0){
goto _LL447;} else{ goto _LL448;} _LL448: goto _LL449; _LL441: t= Cyc_Absyn_at_typ(
Cyc_Absyn_uint_t, Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual());
goto _LL439; _LL443: t= Cyc_Absyn_at_typ( Cyc_Absyn_ulong_t, Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual()); goto _LL439; _LL445: t= Cyc_Absyn_at_typ(
Cyc_Absyn_ushort_t, Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual());
goto _LL439; _LL447: t= Cyc_Absyn_at_typ( Cyc_Absyn_uchar_t, Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual()); goto _LL439; _LL449: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp473= Cyc_String_implode( _temp375); int
_temp474=( int) _temp373; xprintf("length modifier '%.*s' is not allowed with %%%c",
_temp473.last_plus_one - _temp473.curr, _temp473.curr, _temp474);})); return 0;
_LL439:;} typs=({ struct Cyc_List_List* _temp475=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp475->hd=( void*) t; _temp475->tl=
typs; _temp475;}); break; case 'f': _LL437: goto _LL476; case 'F': _LL476: goto
_LL477; case 'e': _LL477: goto _LL478; case 'E': _LL478: goto _LL479; case 'g':
_LL479: goto _LL480; case 'G': _LL480: goto _LL481; case 'a': _LL481: goto
_LL482; case 'A': _LL482:{ struct Cyc_List_List* _temp484= _temp375; struct Cyc_List_List
_temp493; struct Cyc_List_List* _temp494; int _temp496; _LL486: if( _temp484 ==
0){ goto _LL487;} else{ goto _LL488;} _LL488: if( _temp484 == 0){ goto _LL490;}
else{ _temp493=* _temp484; _LL497: _temp496=( int) _temp493.hd; goto _LL495;
_LL495: _temp494=( struct Cyc_List_List*) _temp493.tl; if( _temp494 == 0){ goto
_LL492;} else{ goto _LL490;}} _LL492: if( _temp496 =='l'){ goto _LL489;} else{
goto _LL490;} _LL490: goto _LL491; _LL487: t= Cyc_Absyn_at_typ( Cyc_Absyn_float_t,
Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual()); goto
_LL485; _LL489: t= Cyc_Absyn_at_typ( Cyc_Absyn_double_t, Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual()); goto _LL485; _LL491: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp498= Cyc_String_implode( _temp375); int
_temp499=( int) _temp373; xprintf("length modifier '%.*s' is not allowed with %%%c",
_temp498.last_plus_one - _temp498.curr, _temp498.curr, _temp499);})); return 0;
_LL485:;} typs=({ struct Cyc_List_List* _temp500=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp500->hd=( void*) t; _temp500->tl=
typs; _temp500;}); break; case 'c': _LL483: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp502=( char*)"%%c is not supported"; struct
_tagged_string _temp503; _temp503.curr= _temp502; _temp503.base= _temp502;
_temp503.last_plus_one= _temp502 + 21; _temp503;})); return 0; case 's': _LL501:
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp505=( char*)"%%s is not supported";
struct _tagged_string _temp506; _temp506.curr= _temp505; _temp506.base= _temp505;
_temp506.last_plus_one= _temp505 + 21; _temp506;})); return 0; case '[': _LL504:
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp508=( char*)"%%[ is not supported";
struct _tagged_string _temp509; _temp509.curr= _temp508; _temp509.base= _temp508;
_temp509.last_plus_one= _temp508 + 21; _temp509;})); return 0; case 'p': _LL507:
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp511=( char*)"%%p is not supported";
struct _tagged_string _temp512; _temp512.curr= _temp511; _temp512.base= _temp511;
_temp512.last_plus_one= _temp511 + 21; _temp512;})); return 0; case 'n': _LL510:{
struct Cyc_List_List* _temp514= _temp375; struct Cyc_List_List _temp527; struct
Cyc_List_List* _temp528; int _temp530; struct Cyc_List_List _temp533; struct Cyc_List_List*
_temp534; int _temp536; struct Cyc_List_List _temp539; struct Cyc_List_List*
_temp540; struct Cyc_List_List _temp542; struct Cyc_List_List* _temp543; int
_temp545; int _temp547; _LL516: if( _temp514 == 0){ goto _LL517;} else{ goto
_LL518;} _LL518: if( _temp514 == 0){ goto _LL520;} else{ _temp527=* _temp514;
_LL531: _temp530=( int) _temp527.hd; goto _LL529; _LL529: _temp528=( struct Cyc_List_List*)
_temp527.tl; if( _temp528 == 0){ goto _LL526;} else{ goto _LL520;}} _LL526: if(
_temp530 =='l'){ goto _LL519;} else{ goto _LL520;} _LL520: if( _temp514 == 0){
goto _LL522;} else{ _temp533=* _temp514; _LL537: _temp536=( int) _temp533.hd;
goto _LL535; _LL535: _temp534=( struct Cyc_List_List*) _temp533.tl; if( _temp534
== 0){ goto _LL532;} else{ goto _LL522;}} _LL532: if( _temp536 =='h'){ goto
_LL521;} else{ goto _LL522;} _LL522: if( _temp514 == 0){ goto _LL524;} else{
_temp539=* _temp514; _LL548: _temp547=( int) _temp539.hd; goto _LL541; _LL541:
_temp540=( struct Cyc_List_List*) _temp539.tl; if( _temp540 == 0){ goto _LL524;}
else{ _temp542=* _temp540; _LL546: _temp545=( int) _temp542.hd; goto _LL544;
_LL544: _temp543=( struct Cyc_List_List*) _temp542.tl; if( _temp543 == 0){ goto
_LL538;} else{ goto _LL524;}}} _LL538: if( _temp547 =='h'? _temp545 =='h': 0){
goto _LL523;} else{ goto _LL524;} _LL524: goto _LL525; _LL517: t= Cyc_Absyn_at_typ(
Cyc_Absyn_sint_t, Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual());
goto _LL515; _LL519: t= Cyc_Absyn_at_typ( Cyc_Absyn_ulong_t, Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual()); goto _LL515; _LL521: t= Cyc_Absyn_at_typ(
Cyc_Absyn_sshort_t, Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual());
goto _LL515; _LL523: t= Cyc_Absyn_at_typ( Cyc_Absyn_schar_t, Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual()); goto _LL515; _LL525: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp549= Cyc_String_implode( _temp375); xprintf("length modifier '%.*s' not allowed with %%n",
_temp549.last_plus_one - _temp549.curr, _temp549.curr);})); return 0; _LL515:;}
typs=({ struct Cyc_List_List* _temp550=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp550->hd=( void*) t; _temp550->tl= typs;
_temp550;}); break; case '%': _LL513: if( _temp379){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp552=( char*)"Assignment suppression (*) is not allowed with %%%%";
struct _tagged_string _temp553; _temp553.curr= _temp552; _temp553.base= _temp552;
_temp553.last_plus_one= _temp552 + 52; _temp553;})); return 0;} if( _temp377 !=
0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp554= Cyc_String_implode(
_temp377); xprintf("width '%.*s' not allowed with %%%%", _temp554.last_plus_one
- _temp554.curr, _temp554.curr);})); return 0;} if( _temp375 != 0){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp555= Cyc_String_implode( _temp375); xprintf("length modifier '%.*s' not allowed with %%%%",
_temp555.last_plus_one - _temp555.curr, _temp555.curr);})); return 0;} break;
default: _LL551: return 0;}}}}} return(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( typs);}