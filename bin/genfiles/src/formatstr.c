#include "cyc_include.h"

 struct _tuple0{ void* f1; struct _tagged_string* f2; } ; struct _tuple1{ struct
Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual* f2; void* f3; } ; struct _tuple2{
struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; } ; struct _tuple3{ struct
Cyc_List_List* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3; struct
Cyc_List_List* f4; char f5; int f6; } ; struct _tuple4{ int f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; char f4; int f5; } ; typedef int Cyc_ptrdiff_t;
typedef unsigned int Cyc_size_t; typedef unsigned short Cyc_wchar_t; typedef
unsigned int Cyc_wint_t; typedef char Cyc_u_char; typedef unsigned short Cyc_u_short;
typedef unsigned int Cyc_u_int; typedef unsigned int Cyc_u_long; typedef
unsigned short Cyc_ushort; typedef unsigned int Cyc_uint; typedef unsigned int
Cyc_clock_t; typedef int Cyc_time_t; struct Cyc_timespec{ int tv_sec; int
tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec it_interval; struct Cyc_timespec
it_value; } ; typedef int Cyc_daddr_t; typedef char* Cyc_caddr_t; typedef
unsigned int Cyc_ino_t; typedef unsigned int Cyc_vm_offset_t; typedef
unsigned int Cyc_vm_size_t; typedef char Cyc_int8_t; typedef char Cyc_u_int8_t;
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
< 0? 1:( unsigned int) i >= len){ return 0;}{ struct Cyc_List_List* flags= 0;
char c=' '; for( 0;( unsigned int) i < len; i ++){ c=({ struct _tagged_string
_temp0= s; char* _temp2= _temp0.curr + i; if( _temp2 < _temp0.base? 1: _temp2 >=
_temp0.last_plus_one){ _throw( Null_Exception);}* _temp2;}); switch( c){ case '+':
_LL3: goto _LL4; case '-': _LL4: goto _LL5; case ' ': _LL5: goto _LL6; case '#':
_LL6: goto _LL7; case '0': _LL7: flags=({ struct Cyc_List_List* _temp9=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp9->hd=( void*)((
int) c); _temp9->tl= flags; _temp9;}); continue; default: _LL8: break;} break;}
if(( unsigned int) i >= len){ return 0;} flags=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( flags);{ struct Cyc_List_List*
width= 0; c=({ struct _tagged_string _temp11= s; char* _temp13= _temp11.curr + i;
if( _temp13 < _temp11.base? 1: _temp13 >= _temp11.last_plus_one){ _throw(
Null_Exception);}* _temp13;}); if(( int) c ==( int)'*'){ width=({ struct Cyc_List_List*
_temp14=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp14->hd=( void*)(( int) c); _temp14->tl= width; _temp14;}); i ++;} else{
for( 0;( unsigned int) i < len; i ++){ c=({ struct _tagged_string _temp15= s;
char* _temp17= _temp15.curr + i; if( _temp17 < _temp15.base? 1: _temp17 >=
_temp15.last_plus_one){ _throw( Null_Exception);}* _temp17;}); if( Cyc_Character_isdigit(
c)){ width=({ struct Cyc_List_List* _temp18=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp18->hd=( void*)(( int) c); _temp18->tl=
width; _temp18;});} else{ break;}}} if(( unsigned int) i >= len){ return 0;}
width=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
width);{ struct Cyc_List_List* precision= 0; c=({ struct _tagged_string _temp19=
s; char* _temp21= _temp19.curr + i; if( _temp21 < _temp19.base? 1: _temp21 >=
_temp19.last_plus_one){ _throw( Null_Exception);}* _temp21;}); if(( int) c ==(
int)'.'){ precision=({ struct Cyc_List_List* _temp22=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp22->hd=( void*)(( int) c);
_temp22->tl= precision; _temp22;}); i ++; if(( unsigned int) i >= len){ return 0;}
c=({ struct _tagged_string _temp23= s; char* _temp25= _temp23.curr + i; if(
_temp25 < _temp23.base? 1: _temp25 >= _temp23.last_plus_one){ _throw(
Null_Exception);}* _temp25;}); if(( int) c ==( int)'*'){ precision=({ struct Cyc_List_List*
_temp26=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp26->hd=( void*)(( int) c); _temp26->tl= precision; _temp26;}); i ++;} else{
for( 0;( unsigned int) i < len; i ++){ c=({ struct _tagged_string _temp27= s;
char* _temp29= _temp27.curr + i; if( _temp29 < _temp27.base? 1: _temp29 >=
_temp27.last_plus_one){ _throw( Null_Exception);}* _temp29;}); if( Cyc_Character_isdigit(
c)){ precision=({ struct Cyc_List_List* _temp30=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp30->hd=( void*)(( int) c);
_temp30->tl= precision; _temp30;});} else{ break;}}}} if(( unsigned int) i >=
len){ return 0;} precision=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( precision);{ struct Cyc_List_List* lenmod= 0; c=({ struct
_tagged_string _temp31= s; char* _temp33= _temp31.curr + i; if( _temp33 <
_temp31.base? 1: _temp33 >= _temp31.last_plus_one){ _throw( Null_Exception);}*
_temp33;}); switch( c){ case 'h': _LL34: lenmod=({ struct Cyc_List_List* _temp36=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp36->hd=(
void*)(( int) c); _temp36->tl= lenmod; _temp36;}); i ++; if(( unsigned int) i >=
len){ return 0;} c=({ struct _tagged_string _temp37= s; char* _temp39= _temp37.curr
+ i; if( _temp39 < _temp37.base? 1: _temp39 >= _temp37.last_plus_one){ _throw(
Null_Exception);}* _temp39;}); if(( int) c ==( int)'h'){ lenmod=({ struct Cyc_List_List*
_temp40=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp40->hd=( void*)(( int) c); _temp40->tl= lenmod; _temp40;}); i ++;} break;
case 'l': _LL35: lenmod=({ struct Cyc_List_List* _temp42=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp42->hd=( void*)(( int) c);
_temp42->tl= lenmod; _temp42;}); i ++; if(( unsigned int) i >= len){ return 0;}
c=({ struct _tagged_string _temp43= s; char* _temp45= _temp43.curr + i; if(
_temp45 < _temp43.base? 1: _temp45 >= _temp43.last_plus_one){ _throw(
Null_Exception);}* _temp45;}); if(( int) c ==( int)'l'){ lenmod=({ struct Cyc_List_List*
_temp46=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp46->hd=( void*)(( int) c); _temp46->tl= lenmod; _temp46;}); i ++;} break;
case 'j': _LL41: goto _LL47; case 'z': _LL47: goto _LL48; case 't': _LL48: goto
_LL49; case 'L': _LL49: lenmod=({ struct Cyc_List_List* _temp51=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp51->hd=( void*)(( int) c);
_temp51->tl= lenmod; _temp51;}); i ++; break; default: _LL50: break;} if((
unsigned int) i >= len){ return 0;} lenmod=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( lenmod); c=({ struct _tagged_string
_temp53= s; char* _temp55= _temp53.curr + i; if( _temp55 < _temp53.base? 1:
_temp55 >= _temp53.last_plus_one){ _throw( Null_Exception);}* _temp55;});
switch( c){ case 'd': _LL56: break; case 'i': _LL57: break; case 'o': _LL58:
break; case 'u': _LL59: break; case 'x': _LL60: break; case 'X': _LL61: break;
case 'f': _LL62: break; case 'F': _LL63: break; case 'e': _LL64: break; case 'E':
_LL65: break; case 'g': _LL66: break; case 'G': _LL67: break; case 'a': _LL68:
break; case 'A': _LL69: break; case 'c': _LL70: break; case 's': _LL71: break;
case 'p': _LL72: break; case 'n': _LL73: break; case '%': _LL74: break; default:
_LL75: return 0;} return({ struct Cyc_Core_Opt* _temp77=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp77->v=( void*)({ struct _tuple3*
_temp78=( struct _tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp78->f1=
flags; _temp78->f2= width; _temp78->f3= precision; _temp78->f4= lenmod; _temp78->f5=
c; _temp78->f6= i + 1; _temp78;}); _temp77;});}}}}} struct Cyc_List_List* Cyc_Formatstr_get_format_typs(
struct _tagged_string s, struct Cyc_Position_Segment* loc){ unsigned int len=
Cyc_String_strlen( s); struct Cyc_List_List* typs= 0; int i; for( i= 0;(
unsigned int) i < len; i ++){ if(( int)({ struct _tagged_string _temp79= s; char*
_temp81= _temp79.curr + i; if( _temp81 < _temp79.base? 1: _temp81 >= _temp79.last_plus_one){
_throw( Null_Exception);}* _temp81;}) !=( int)'%'){ continue;}{ struct Cyc_Core_Opt*
cs= Cyc_Formatstr_parse_conversionspecification( s, i + 1); if( cs == 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp82=( char*)"bad format string"; struct
_tagged_string _temp83; _temp83.curr= _temp82; _temp83.base= _temp82; _temp83.last_plus_one=
_temp82 + 18; _temp83;})); return 0;}{ struct _tuple3 _temp86; int _temp87; char
_temp89; struct Cyc_List_List* _temp91; struct Cyc_List_List* _temp93; struct
Cyc_List_List* _temp95; struct Cyc_List_List* _temp97; struct _tuple3* _temp84=(
struct _tuple3*) cs->v; _temp86=* _temp84; _LL98: _temp97= _temp86.f1; goto
_LL96; _LL96: _temp95= _temp86.f2; goto _LL94; _LL94: _temp93= _temp86.f3; goto
_LL92; _LL92: _temp91= _temp86.f4; goto _LL90; _LL90: _temp89= _temp86.f5; goto
_LL88; _LL88: _temp87= _temp86.f6; goto _LL85; _LL85: i= _temp87 - 1;{ struct
Cyc_List_List* _temp99= _temp91; struct Cyc_List_List _temp106; struct Cyc_List_List*
_temp107; int _temp109; _LL101: if( _temp99 == 0){ goto _LL103;} else{ _temp106=*
_temp99; _LL110: _temp109=( int) _temp106.hd; goto _LL108; _LL108: _temp107=(
struct Cyc_List_List*) _temp106.tl; if( _temp107 == 0){ goto _LL105;} else{ goto
_LL103;}} _LL105: if(( _temp109 ==( int)'j'? 1: _temp109 ==( int)'z')? 1:
_temp109 ==( int)'t'){ goto _LL102;} else{ goto _LL103;} _LL103: goto _LL104;
_LL102: Cyc_Tcutil_terr( loc, xprintf("length modifier '%c' is not supported",
_temp109)); return 0; _LL104: goto _LL100; _LL100:;}{ struct Cyc_List_List*
_temp111= _temp95; struct Cyc_List_List _temp118; struct Cyc_List_List* _temp119;
int _temp121; _LL113: if( _temp111 == 0){ goto _LL115;} else{ _temp118=*
_temp111; _LL122: _temp121=( int) _temp118.hd; goto _LL120; _LL120: _temp119=(
struct Cyc_List_List*) _temp118.tl; if( _temp119 == 0){ goto _LL117;} else{ goto
_LL115;}} _LL117: if( _temp121 ==( int)'*'){ goto _LL114;} else{ goto _LL115;}
_LL115: goto _LL116; _LL114: typs=({ struct Cyc_List_List* _temp123=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp123->hd=( void*) Cyc_Absyn_sint_t;
_temp123->tl= typs; _temp123;}); goto _LL112; _LL116: goto _LL112; _LL112:;}{
struct Cyc_List_List* _temp124= _temp93; struct Cyc_List_List _temp131; struct
Cyc_List_List* _temp132; struct Cyc_List_List _temp134; struct Cyc_List_List*
_temp135; int _temp137; int _temp139; _LL126: if( _temp124 == 0){ goto _LL128;}
else{ _temp131=* _temp124; _LL140: _temp139=( int) _temp131.hd; goto _LL133;
_LL133: _temp132=( struct Cyc_List_List*) _temp131.tl; if( _temp132 == 0){ goto
_LL128;} else{ _temp134=* _temp132; _LL138: _temp137=( int) _temp134.hd; goto
_LL136; _LL136: _temp135=( struct Cyc_List_List*) _temp134.tl; if( _temp135 == 0){
goto _LL130;} else{ goto _LL128;}}} _LL130: if( _temp139 ==( int)'.'? _temp137
==( int)'*': 0){ goto _LL127;} else{ goto _LL128;} _LL128: goto _LL129; _LL127:
typs=({ struct Cyc_List_List* _temp141=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp141->hd=( void*) Cyc_Absyn_sint_t; _temp141->tl=
typs; _temp141;}); goto _LL125; _LL129: goto _LL125; _LL125:;}{ void* t; switch(
_temp89){ case 'd': _LL142: goto _LL143; case 'i': _LL143:{ struct Cyc_List_List*
f= _temp97; for( 0; f != 0; f= f->tl){ if(( int) f->hd ==( int)'#'){ Cyc_Tcutil_terr(
loc, xprintf("flag '#' is not valid with %%%c",( int) _temp89)); return 0;}}}{
struct Cyc_List_List* _temp145= _temp91; struct Cyc_List_List _temp158; struct
Cyc_List_List* _temp159; int _temp161; struct Cyc_List_List _temp164; struct Cyc_List_List*
_temp165; int _temp167; struct Cyc_List_List _temp170; struct Cyc_List_List*
_temp171; struct Cyc_List_List _temp173; struct Cyc_List_List* _temp174; int
_temp176; int _temp178; _LL147: if( _temp145 == 0){ goto _LL148;} else{ goto
_LL149;} _LL149: if( _temp145 == 0){ goto _LL151;} else{ _temp158=* _temp145;
_LL162: _temp161=( int) _temp158.hd; goto _LL160; _LL160: _temp159=( struct Cyc_List_List*)
_temp158.tl; if( _temp159 == 0){ goto _LL157;} else{ goto _LL151;}} _LL157: if(
_temp161 ==( int)'l'){ goto _LL150;} else{ goto _LL151;} _LL151: if( _temp145 ==
0){ goto _LL153;} else{ _temp164=* _temp145; _LL168: _temp167=( int) _temp164.hd;
goto _LL166; _LL166: _temp165=( struct Cyc_List_List*) _temp164.tl; if( _temp165
== 0){ goto _LL163;} else{ goto _LL153;}} _LL163: if( _temp167 ==( int)'h'){
goto _LL152;} else{ goto _LL153;} _LL153: if( _temp145 == 0){ goto _LL155;}
else{ _temp170=* _temp145; _LL179: _temp178=( int) _temp170.hd; goto _LL172;
_LL172: _temp171=( struct Cyc_List_List*) _temp170.tl; if( _temp171 == 0){ goto
_LL155;} else{ _temp173=* _temp171; _LL177: _temp176=( int) _temp173.hd; goto
_LL175; _LL175: _temp174=( struct Cyc_List_List*) _temp173.tl; if( _temp174 == 0){
goto _LL169;} else{ goto _LL155;}}} _LL169: if( _temp178 ==( int)'h'? _temp176
==( int)'h': 0){ goto _LL154;} else{ goto _LL155;} _LL155: goto _LL156; _LL148:
t= Cyc_Absyn_sint_t; goto _LL146; _LL150: t= Cyc_Absyn_slong_t; goto _LL146;
_LL152: t= Cyc_Absyn_sshort_t; goto _LL146; _LL154: t= Cyc_Absyn_schar_t; goto
_LL146; _LL156: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp180= Cyc_String_implode(
_temp91); int _temp181=( int) _temp89; xprintf("length modifier '%.*s' is not allowed with %%%c",
_temp180.last_plus_one - _temp180.curr, _temp180.curr, _temp181);})); return 0;
_LL146:;} typs=({ struct Cyc_List_List* _temp182=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp182->hd=( void*) t; _temp182->tl=
typs; _temp182;}); break; case 'u': _LL144:{ struct Cyc_List_List* f= _temp97;
for( 0; f != 0; f= f->tl){ if(( int) f->hd ==( int)'#'){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp184=( char*)"Flag '#' not valid with %%u";
struct _tagged_string _temp185; _temp185.curr= _temp184; _temp185.base= _temp184;
_temp185.last_plus_one= _temp184 + 28; _temp185;})); return 0;}}} goto _LL183;
case 'o': _LL183: goto _LL186; case 'x': _LL186: goto _LL187; case 'X': _LL187:{
struct Cyc_List_List* _temp189= _temp91; struct Cyc_List_List _temp202; struct
Cyc_List_List* _temp203; int _temp205; struct Cyc_List_List _temp208; struct Cyc_List_List*
_temp209; int _temp211; struct Cyc_List_List _temp214; struct Cyc_List_List*
_temp215; struct Cyc_List_List _temp217; struct Cyc_List_List* _temp218; int
_temp220; int _temp222; _LL191: if( _temp189 == 0){ goto _LL192;} else{ goto
_LL193;} _LL193: if( _temp189 == 0){ goto _LL195;} else{ _temp202=* _temp189;
_LL206: _temp205=( int) _temp202.hd; goto _LL204; _LL204: _temp203=( struct Cyc_List_List*)
_temp202.tl; if( _temp203 == 0){ goto _LL201;} else{ goto _LL195;}} _LL201: if(
_temp205 ==( int)'l'){ goto _LL194;} else{ goto _LL195;} _LL195: if( _temp189 ==
0){ goto _LL197;} else{ _temp208=* _temp189; _LL212: _temp211=( int) _temp208.hd;
goto _LL210; _LL210: _temp209=( struct Cyc_List_List*) _temp208.tl; if( _temp209
== 0){ goto _LL207;} else{ goto _LL197;}} _LL207: if( _temp211 ==( int)'h'){
goto _LL196;} else{ goto _LL197;} _LL197: if( _temp189 == 0){ goto _LL199;}
else{ _temp214=* _temp189; _LL223: _temp222=( int) _temp214.hd; goto _LL216;
_LL216: _temp215=( struct Cyc_List_List*) _temp214.tl; if( _temp215 == 0){ goto
_LL199;} else{ _temp217=* _temp215; _LL221: _temp220=( int) _temp217.hd; goto
_LL219; _LL219: _temp218=( struct Cyc_List_List*) _temp217.tl; if( _temp218 == 0){
goto _LL213;} else{ goto _LL199;}}} _LL213: if( _temp222 ==( int)'h'? _temp220
==( int)'h': 0){ goto _LL198;} else{ goto _LL199;} _LL199: goto _LL200; _LL192:
t= Cyc_Absyn_uint_t; goto _LL190; _LL194: t= Cyc_Absyn_ulong_t; goto _LL190;
_LL196: t= Cyc_Absyn_ushort_t; goto _LL190; _LL198: t= Cyc_Absyn_uchar_t; goto
_LL190; _LL200: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp224= Cyc_String_implode(
_temp91); int _temp225=( int) _temp89; xprintf("length modifier '%.*s' is not allowed with %%%c",
_temp224.last_plus_one - _temp224.curr, _temp224.curr, _temp225);})); return 0;
_LL190:;} typs=({ struct Cyc_List_List* _temp226=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp226->hd=( void*) t; _temp226->tl=
typs; _temp226;}); break; case 'f': _LL188: goto _LL227; case 'F': _LL227: goto
_LL228; case 'e': _LL228: goto _LL229; case 'E': _LL229: goto _LL230; case 'g':
_LL230: goto _LL231; case 'G': _LL231: goto _LL232; case 'a': _LL232: goto
_LL233; case 'A': _LL233: if( _temp91 != 0){ Cyc_Tcutil_terr( loc, xprintf("length modifiers are not allowed with %%%c",(
int) _temp89)); return 0;} typs=({ struct Cyc_List_List* _temp235=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp235->hd=( void*) Cyc_Absyn_double_t;
_temp235->tl= typs; _temp235;}); break; case 'c': _LL234:{ struct Cyc_List_List*
f= _temp97; for( 0; f != 0; f= f->tl){ if(( int) f->hd ==( int)'#'? 1:( int) f->hd
==( int)'0'){ Cyc_Tcutil_terr( loc, xprintf("flag '%c' not allowed with %%c",(
int) f->hd)); return 0;}}} if( _temp91 != 0){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp237= Cyc_String_implode( _temp91); xprintf("length modifier '%.*s' not allowed with %%c",
_temp237.last_plus_one - _temp237.curr, _temp237.curr);})); return 0;} if(
_temp93 != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp238= Cyc_String_implode(
_temp93); xprintf("precision '%.*s' not allowed with %%c", _temp238.last_plus_one
- _temp238.curr, _temp238.curr);})); return 0;} typs=({ struct Cyc_List_List*
_temp239=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp239->hd=( void*) Cyc_Absyn_sint_t; _temp239->tl= typs; _temp239;}); break;
case 's': _LL236: if( _temp97 != 0){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp241= Cyc_String_implode( _temp97); xprintf("flags '%.*s' not allowed with %%s",
_temp241.last_plus_one - _temp241.curr, _temp241.curr);})); return 0;} if(
_temp95 != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp242= Cyc_String_implode(
_temp95); xprintf("width '%.*s' not allowed with %%s", _temp242.last_plus_one -
_temp242.curr, _temp242.curr);})); return 0;} if( _temp93 != 0){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp243= Cyc_String_implode( _temp93); xprintf("precision '%.*s' not allowed with %%s",
_temp243.last_plus_one - _temp243.curr, _temp243.curr);})); return 0;} if(
_temp91 != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp244= Cyc_String_implode(
_temp91); xprintf("length modifier '%.*s' not allowed with %%s", _temp244.last_plus_one
- _temp244.curr, _temp244.curr);})); return 0;} typs=({ struct Cyc_List_List*
_temp245=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp245->hd=( void*) Cyc_Absyn_string_typ( Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind));
_temp245->tl= typs; _temp245;}); break; case 'p': _LL240: Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp247=( char*)"%%p not supported"; struct
_tagged_string _temp248; _temp248.curr= _temp247; _temp248.base= _temp247;
_temp248.last_plus_one= _temp247 + 18; _temp248;})); return 0; case 'n': _LL246:{
struct Cyc_List_List* f= _temp97; for( 0; f != 0; f= f->tl){ if(( int) f->hd ==(
int)'#'? 1:( int) f->hd ==( int)'0'){ Cyc_Tcutil_terr( loc, xprintf("flag '%c' not allowed with %%n",(
int) f->hd)); return 0;}}} if( _temp93 != 0){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp250= Cyc_String_implode( _temp93); xprintf("precision '%.*s' not allowed with %%n",
_temp250.last_plus_one - _temp250.curr, _temp250.curr);})); return 0;}{ struct
Cyc_List_List* _temp251= _temp91; struct Cyc_List_List _temp264; struct Cyc_List_List*
_temp265; int _temp267; struct Cyc_List_List _temp270; struct Cyc_List_List*
_temp271; int _temp273; struct Cyc_List_List _temp276; struct Cyc_List_List*
_temp277; struct Cyc_List_List _temp279; struct Cyc_List_List* _temp280; int
_temp282; int _temp284; _LL253: if( _temp251 == 0){ goto _LL254;} else{ goto
_LL255;} _LL255: if( _temp251 == 0){ goto _LL257;} else{ _temp264=* _temp251;
_LL268: _temp267=( int) _temp264.hd; goto _LL266; _LL266: _temp265=( struct Cyc_List_List*)
_temp264.tl; if( _temp265 == 0){ goto _LL263;} else{ goto _LL257;}} _LL263: if(
_temp267 ==( int)'l'){ goto _LL256;} else{ goto _LL257;} _LL257: if( _temp251 ==
0){ goto _LL259;} else{ _temp270=* _temp251; _LL274: _temp273=( int) _temp270.hd;
goto _LL272; _LL272: _temp271=( struct Cyc_List_List*) _temp270.tl; if( _temp271
== 0){ goto _LL269;} else{ goto _LL259;}} _LL269: if( _temp273 ==( int)'h'){
goto _LL258;} else{ goto _LL259;} _LL259: if( _temp251 == 0){ goto _LL261;}
else{ _temp276=* _temp251; _LL285: _temp284=( int) _temp276.hd; goto _LL278;
_LL278: _temp277=( struct Cyc_List_List*) _temp276.tl; if( _temp277 == 0){ goto
_LL261;} else{ _temp279=* _temp277; _LL283: _temp282=( int) _temp279.hd; goto
_LL281; _LL281: _temp280=( struct Cyc_List_List*) _temp279.tl; if( _temp280 == 0){
goto _LL275;} else{ goto _LL261;}}} _LL275: if( _temp284 ==( int)'h'? _temp282
==( int)'h': 0){ goto _LL260;} else{ goto _LL261;} _LL261: goto _LL262; _LL254:
t= Cyc_Absyn_at_typ( Cyc_Absyn_sint_t, Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind),
Cyc_Absyn_empty_tqual()); goto _LL252; _LL256: t= Cyc_Absyn_at_typ( Cyc_Absyn_ulong_t,
Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual()); goto
_LL252; _LL258: t= Cyc_Absyn_at_typ( Cyc_Absyn_sshort_t, Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual()); goto _LL252; _LL260: t= Cyc_Absyn_at_typ(
Cyc_Absyn_schar_t, Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual());
goto _LL252; _LL262: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp286= Cyc_String_implode(
_temp91); xprintf("length modifier '%.*s' not allowed with %%n", _temp286.last_plus_one
- _temp286.curr, _temp286.curr);})); return 0; _LL252:;} typs=({ struct Cyc_List_List*
_temp287=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp287->hd=( void*) t; _temp287->tl= typs; _temp287;}); break; case '%':
_LL249: if( _temp97 != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp289= Cyc_String_implode( _temp97); xprintf("flags '%.*s' not allowed with %%%%",
_temp289.last_plus_one - _temp289.curr, _temp289.curr);})); return 0;} if(
_temp95 != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp290= Cyc_String_implode(
_temp95); xprintf("width '%.*s' not allowed with %%%%", _temp290.last_plus_one -
_temp290.curr, _temp290.curr);})); return 0;} if( _temp93 != 0){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp291= Cyc_String_implode( _temp93); xprintf("precision '%.*s' not allowed with %%%%",
_temp291.last_plus_one - _temp291.curr, _temp291.curr);})); return 0;} if(
_temp91 != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp292= Cyc_String_implode(
_temp91); xprintf("length modifier '%.*s' not allowed with %%%%", _temp292.last_plus_one
- _temp292.curr, _temp292.curr);})); return 0;} break; default: _LL288: return 0;}}}}}
return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
typs);} struct Cyc_Core_Opt* Cyc_Formatstr_parse_inputformat( struct
_tagged_string s, int i){ unsigned int len= Cyc_String_strlen( s); if( i < 0? 1:(
unsigned int) i >= len){ return 0;}{ int suppress= 0; char c=({ struct
_tagged_string _temp294= s; char* _temp296= _temp294.curr + i; if( _temp296 <
_temp294.base? 1: _temp296 >= _temp294.last_plus_one){ _throw( Null_Exception);}*
_temp296;}); if(( int) c ==( int)'*'){ suppress= 1; i ++; if(( unsigned int) i
>= len){ return 0;}}{ struct Cyc_List_List* width= 0; for( 0;( unsigned int) i <
len; i ++){ c=({ struct _tagged_string _temp297= s; char* _temp299= _temp297.curr
+ i; if( _temp299 < _temp297.base? 1: _temp299 >= _temp297.last_plus_one){
_throw( Null_Exception);}* _temp299;}); if( Cyc_Character_isdigit( c)){ width=({
struct Cyc_List_List* _temp300=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp300->hd=( void*)(( int) c); _temp300->tl= width;
_temp300;});} else{ break;}} if(( unsigned int) i >= len){ return 0;} width=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( width);{
struct Cyc_List_List* lenmod= 0; c=({ struct _tagged_string _temp301= s; char*
_temp303= _temp301.curr + i; if( _temp303 < _temp301.base? 1: _temp303 >=
_temp301.last_plus_one){ _throw( Null_Exception);}* _temp303;}); switch( c){
case 'h': _LL304: lenmod=({ struct Cyc_List_List* _temp306=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp306->hd=( void*)(( int) c);
_temp306->tl= lenmod; _temp306;}); i ++; if(( unsigned int) i >= len){ return 0;}
c=({ struct _tagged_string _temp307= s; char* _temp309= _temp307.curr + i; if(
_temp309 < _temp307.base? 1: _temp309 >= _temp307.last_plus_one){ _throw(
Null_Exception);}* _temp309;}); if(( int) c ==( int)'h'){ lenmod=({ struct Cyc_List_List*
_temp310=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp310->hd=( void*)(( int) c); _temp310->tl= lenmod; _temp310;}); i ++;}
break; case 'l': _LL305: lenmod=({ struct Cyc_List_List* _temp312=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp312->hd=( void*)(( int) c);
_temp312->tl= lenmod; _temp312;}); i ++; if(( unsigned int) i >= len){ return 0;}
c=({ struct _tagged_string _temp313= s; char* _temp315= _temp313.curr + i; if(
_temp315 < _temp313.base? 1: _temp315 >= _temp313.last_plus_one){ _throw(
Null_Exception);}* _temp315;}); if(( int) c ==( int)'l'){ lenmod=({ struct Cyc_List_List*
_temp316=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp316->hd=( void*)(( int) c); _temp316->tl= lenmod; _temp316;}); i ++;}
break; case 'j': _LL311: goto _LL317; case 'z': _LL317: goto _LL318; case 't':
_LL318: goto _LL319; case 'L': _LL319: lenmod=({ struct Cyc_List_List* _temp321=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp321->hd=(
void*)(( int) c); _temp321->tl= lenmod; _temp321;}); i ++; break; default:
_LL320: break;} if(( unsigned int) i >= len){ return 0;} lenmod=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( lenmod); c=({ struct _tagged_string
_temp323= s; char* _temp325= _temp323.curr + i; if( _temp325 < _temp323.base? 1:
_temp325 >= _temp323.last_plus_one){ _throw( Null_Exception);}* _temp325;});
switch( c){ case 'd': _LL326: break; case 'i': _LL327: break; case 'o': _LL328:
break; case 'u': _LL329: break; case 'x': _LL330: break; case 'X': _LL331:
break; case 'f': _LL332: break; case 'F': _LL333: break; case 'e': _LL334:
break; case 'E': _LL335: break; case 'g': _LL336: break; case 'G': _LL337:
break; case 'a': _LL338: break; case 'A': _LL339: break; case 'c': _LL340:
break; case 's': _LL341: break; case 'p': _LL342: break; case 'n': _LL343:
break; case '%': _LL344: break; default: _LL345: return 0;} return({ struct Cyc_Core_Opt*
_temp347=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp347->v=( void*)({ struct _tuple4* _temp348=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp348->f1= suppress; _temp348->f2= width; _temp348->f3=
lenmod; _temp348->f4= c; _temp348->f5= i + 1; _temp348;}); _temp347;});}}}}
struct Cyc_List_List* Cyc_Formatstr_get_scanf_typs( struct _tagged_string s,
struct Cyc_Position_Segment* loc){ unsigned int len= Cyc_String_strlen( s);
struct Cyc_List_List* typs= 0; int i; for( i= 0;( unsigned int) i < len; i ++){
if(( int)({ struct _tagged_string _temp349= s; char* _temp351= _temp349.curr + i;
if( _temp351 < _temp349.base? 1: _temp351 >= _temp349.last_plus_one){ _throw(
Null_Exception);}* _temp351;}) !=( int)'%'){ continue;}{ struct Cyc_Core_Opt* x=
Cyc_Formatstr_parse_inputformat( s, i + 1); if( x == 0){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp352=( char*)"bad format string"; struct
_tagged_string _temp353; _temp353.curr= _temp352; _temp353.base= _temp352;
_temp353.last_plus_one= _temp352 + 18; _temp353;})); return 0;}{ struct _tuple4
_temp356; int _temp357; char _temp359; struct Cyc_List_List* _temp361; struct
Cyc_List_List* _temp363; int _temp365; struct _tuple4* _temp354=( struct _tuple4*)
x->v; _temp356=* _temp354; _LL366: _temp365= _temp356.f1; goto _LL364; _LL364:
_temp363= _temp356.f2; goto _LL362; _LL362: _temp361= _temp356.f3; goto _LL360;
_LL360: _temp359= _temp356.f4; goto _LL358; _LL358: _temp357= _temp356.f5; goto
_LL355; _LL355: i= _temp357 - 1;{ struct Cyc_List_List* _temp367= _temp361;
struct Cyc_List_List _temp374; struct Cyc_List_List* _temp375; int _temp377;
_LL369: if( _temp367 == 0){ goto _LL371;} else{ _temp374=* _temp367; _LL378:
_temp377=( int) _temp374.hd; goto _LL376; _LL376: _temp375=( struct Cyc_List_List*)
_temp374.tl; if( _temp375 == 0){ goto _LL373;} else{ goto _LL371;}} _LL373: if((
_temp377 ==( int)'j'? 1: _temp377 ==( int)'z')? 1: _temp377 ==( int)'t'){ goto
_LL370;} else{ goto _LL371;} _LL371: goto _LL372; _LL370: Cyc_Tcutil_terr( loc,
xprintf("length modifier '%c' is not supported", _temp377)); return 0; _LL372:
goto _LL368; _LL368:;} if( _temp365){ continue;}{ void* t; switch( _temp359){
case 'd': _LL379: goto _LL380; case 'i': _LL380:{ struct Cyc_List_List* _temp382=
_temp361; struct Cyc_List_List _temp395; struct Cyc_List_List* _temp396; int
_temp398; struct Cyc_List_List _temp401; struct Cyc_List_List* _temp402; int
_temp404; struct Cyc_List_List _temp407; struct Cyc_List_List* _temp408; struct
Cyc_List_List _temp410; struct Cyc_List_List* _temp411; int _temp413; int
_temp415; _LL384: if( _temp382 == 0){ goto _LL385;} else{ goto _LL386;} _LL386:
if( _temp382 == 0){ goto _LL388;} else{ _temp395=* _temp382; _LL399: _temp398=(
int) _temp395.hd; goto _LL397; _LL397: _temp396=( struct Cyc_List_List*)
_temp395.tl; if( _temp396 == 0){ goto _LL394;} else{ goto _LL388;}} _LL394: if(
_temp398 ==( int)'l'){ goto _LL387;} else{ goto _LL388;} _LL388: if( _temp382 ==
0){ goto _LL390;} else{ _temp401=* _temp382; _LL405: _temp404=( int) _temp401.hd;
goto _LL403; _LL403: _temp402=( struct Cyc_List_List*) _temp401.tl; if( _temp402
== 0){ goto _LL400;} else{ goto _LL390;}} _LL400: if( _temp404 ==( int)'h'){
goto _LL389;} else{ goto _LL390;} _LL390: if( _temp382 == 0){ goto _LL392;}
else{ _temp407=* _temp382; _LL416: _temp415=( int) _temp407.hd; goto _LL409;
_LL409: _temp408=( struct Cyc_List_List*) _temp407.tl; if( _temp408 == 0){ goto
_LL392;} else{ _temp410=* _temp408; _LL414: _temp413=( int) _temp410.hd; goto
_LL412; _LL412: _temp411=( struct Cyc_List_List*) _temp410.tl; if( _temp411 == 0){
goto _LL406;} else{ goto _LL392;}}} _LL406: if( _temp415 ==( int)'h'? _temp413
==( int)'h': 0){ goto _LL391;} else{ goto _LL392;} _LL392: goto _LL393; _LL385:
t= Cyc_Absyn_at_typ( Cyc_Absyn_sint_t, Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind),
Cyc_Absyn_empty_tqual()); goto _LL383; _LL387: t= Cyc_Absyn_at_typ( Cyc_Absyn_slong_t,
Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual()); goto
_LL383; _LL389: t= Cyc_Absyn_at_typ( Cyc_Absyn_sshort_t, Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual()); goto _LL383; _LL391: t= Cyc_Absyn_at_typ(
Cyc_Absyn_schar_t, Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual());
goto _LL383; _LL393: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp417= Cyc_String_implode(
_temp361); int _temp418=( int) _temp359; xprintf("length modifier '%.*s' is not allowed with %%%c",
_temp417.last_plus_one - _temp417.curr, _temp417.curr, _temp418);})); return 0;
_LL383:;} typs=({ struct Cyc_List_List* _temp419=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp419->hd=( void*) t; _temp419->tl=
typs; _temp419;}); break; case 'u': _LL381: goto _LL420; case 'o': _LL420: goto
_LL421; case 'x': _LL421: goto _LL422; case 'X': _LL422:{ struct Cyc_List_List*
_temp424= _temp361; struct Cyc_List_List _temp437; struct Cyc_List_List*
_temp438; int _temp440; struct Cyc_List_List _temp443; struct Cyc_List_List*
_temp444; int _temp446; struct Cyc_List_List _temp449; struct Cyc_List_List*
_temp450; struct Cyc_List_List _temp452; struct Cyc_List_List* _temp453; int
_temp455; int _temp457; _LL426: if( _temp424 == 0){ goto _LL427;} else{ goto
_LL428;} _LL428: if( _temp424 == 0){ goto _LL430;} else{ _temp437=* _temp424;
_LL441: _temp440=( int) _temp437.hd; goto _LL439; _LL439: _temp438=( struct Cyc_List_List*)
_temp437.tl; if( _temp438 == 0){ goto _LL436;} else{ goto _LL430;}} _LL436: if(
_temp440 ==( int)'l'){ goto _LL429;} else{ goto _LL430;} _LL430: if( _temp424 ==
0){ goto _LL432;} else{ _temp443=* _temp424; _LL447: _temp446=( int) _temp443.hd;
goto _LL445; _LL445: _temp444=( struct Cyc_List_List*) _temp443.tl; if( _temp444
== 0){ goto _LL442;} else{ goto _LL432;}} _LL442: if( _temp446 ==( int)'h'){
goto _LL431;} else{ goto _LL432;} _LL432: if( _temp424 == 0){ goto _LL434;}
else{ _temp449=* _temp424; _LL458: _temp457=( int) _temp449.hd; goto _LL451;
_LL451: _temp450=( struct Cyc_List_List*) _temp449.tl; if( _temp450 == 0){ goto
_LL434;} else{ _temp452=* _temp450; _LL456: _temp455=( int) _temp452.hd; goto
_LL454; _LL454: _temp453=( struct Cyc_List_List*) _temp452.tl; if( _temp453 == 0){
goto _LL448;} else{ goto _LL434;}}} _LL448: if( _temp457 ==( int)'h'? _temp455
==( int)'h': 0){ goto _LL433;} else{ goto _LL434;} _LL434: goto _LL435; _LL427:
t= Cyc_Absyn_at_typ( Cyc_Absyn_uint_t, Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind),
Cyc_Absyn_empty_tqual()); goto _LL425; _LL429: t= Cyc_Absyn_at_typ( Cyc_Absyn_ulong_t,
Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual()); goto
_LL425; _LL431: t= Cyc_Absyn_at_typ( Cyc_Absyn_ushort_t, Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual()); goto _LL425; _LL433: t= Cyc_Absyn_at_typ(
Cyc_Absyn_uchar_t, Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual());
goto _LL425; _LL435: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp459= Cyc_String_implode(
_temp361); int _temp460=( int) _temp359; xprintf("length modifier '%.*s' is not allowed with %%%c",
_temp459.last_plus_one - _temp459.curr, _temp459.curr, _temp460);})); return 0;
_LL425:;} typs=({ struct Cyc_List_List* _temp461=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp461->hd=( void*) t; _temp461->tl=
typs; _temp461;}); break; case 'f': _LL423: goto _LL462; case 'F': _LL462: goto
_LL463; case 'e': _LL463: goto _LL464; case 'E': _LL464: goto _LL465; case 'g':
_LL465: goto _LL466; case 'G': _LL466: goto _LL467; case 'a': _LL467: goto
_LL468; case 'A': _LL468:{ struct Cyc_List_List* _temp470= _temp361; struct Cyc_List_List
_temp479; struct Cyc_List_List* _temp480; int _temp482; _LL472: if( _temp470 ==
0){ goto _LL473;} else{ goto _LL474;} _LL474: if( _temp470 == 0){ goto _LL476;}
else{ _temp479=* _temp470; _LL483: _temp482=( int) _temp479.hd; goto _LL481;
_LL481: _temp480=( struct Cyc_List_List*) _temp479.tl; if( _temp480 == 0){ goto
_LL478;} else{ goto _LL476;}} _LL478: if( _temp482 ==( int)'l'){ goto _LL475;}
else{ goto _LL476;} _LL476: goto _LL477; _LL473: t= Cyc_Absyn_at_typ( Cyc_Absyn_float_t,
Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual()); goto
_LL471; _LL475: t= Cyc_Absyn_at_typ( Cyc_Absyn_double_t, Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual()); goto _LL471; _LL477: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp484= Cyc_String_implode( _temp361); int
_temp485=( int) _temp359; xprintf("length modifier '%.*s' is not allowed with %%%c",
_temp484.last_plus_one - _temp484.curr, _temp484.curr, _temp485);})); return 0;
_LL471:;} typs=({ struct Cyc_List_List* _temp486=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp486->hd=( void*) t; _temp486->tl=
typs; _temp486;}); break; case 'c': _LL469: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp488=( char*)"%%c is not supported"; struct
_tagged_string _temp489; _temp489.curr= _temp488; _temp489.base= _temp488;
_temp489.last_plus_one= _temp488 + 21; _temp489;})); return 0; case 's': _LL487:
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp491=( char*)"%%s is not supported";
struct _tagged_string _temp492; _temp492.curr= _temp491; _temp492.base= _temp491;
_temp492.last_plus_one= _temp491 + 21; _temp492;})); return 0; case '[': _LL490:
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp494=( char*)"%%[ is not supported";
struct _tagged_string _temp495; _temp495.curr= _temp494; _temp495.base= _temp494;
_temp495.last_plus_one= _temp494 + 21; _temp495;})); return 0; case 'p': _LL493:
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp497=( char*)"%%p is not supported";
struct _tagged_string _temp498; _temp498.curr= _temp497; _temp498.base= _temp497;
_temp498.last_plus_one= _temp497 + 21; _temp498;})); return 0; case 'n': _LL496:{
struct Cyc_List_List* _temp500= _temp361; struct Cyc_List_List _temp513; struct
Cyc_List_List* _temp514; int _temp516; struct Cyc_List_List _temp519; struct Cyc_List_List*
_temp520; int _temp522; struct Cyc_List_List _temp525; struct Cyc_List_List*
_temp526; struct Cyc_List_List _temp528; struct Cyc_List_List* _temp529; int
_temp531; int _temp533; _LL502: if( _temp500 == 0){ goto _LL503;} else{ goto
_LL504;} _LL504: if( _temp500 == 0){ goto _LL506;} else{ _temp513=* _temp500;
_LL517: _temp516=( int) _temp513.hd; goto _LL515; _LL515: _temp514=( struct Cyc_List_List*)
_temp513.tl; if( _temp514 == 0){ goto _LL512;} else{ goto _LL506;}} _LL512: if(
_temp516 ==( int)'l'){ goto _LL505;} else{ goto _LL506;} _LL506: if( _temp500 ==
0){ goto _LL508;} else{ _temp519=* _temp500; _LL523: _temp522=( int) _temp519.hd;
goto _LL521; _LL521: _temp520=( struct Cyc_List_List*) _temp519.tl; if( _temp520
== 0){ goto _LL518;} else{ goto _LL508;}} _LL518: if( _temp522 ==( int)'h'){
goto _LL507;} else{ goto _LL508;} _LL508: if( _temp500 == 0){ goto _LL510;}
else{ _temp525=* _temp500; _LL534: _temp533=( int) _temp525.hd; goto _LL527;
_LL527: _temp526=( struct Cyc_List_List*) _temp525.tl; if( _temp526 == 0){ goto
_LL510;} else{ _temp528=* _temp526; _LL532: _temp531=( int) _temp528.hd; goto
_LL530; _LL530: _temp529=( struct Cyc_List_List*) _temp528.tl; if( _temp529 == 0){
goto _LL524;} else{ goto _LL510;}}} _LL524: if( _temp533 ==( int)'h'? _temp531
==( int)'h': 0){ goto _LL509;} else{ goto _LL510;} _LL510: goto _LL511; _LL503:
t= Cyc_Absyn_at_typ( Cyc_Absyn_sint_t, Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind),
Cyc_Absyn_empty_tqual()); goto _LL501; _LL505: t= Cyc_Absyn_at_typ( Cyc_Absyn_ulong_t,
Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual()); goto
_LL501; _LL507: t= Cyc_Absyn_at_typ( Cyc_Absyn_sshort_t, Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual()); goto _LL501; _LL509: t= Cyc_Absyn_at_typ(
Cyc_Absyn_schar_t, Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual());
goto _LL501; _LL511: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp535= Cyc_String_implode(
_temp361); xprintf("length modifier '%.*s' not allowed with %%n", _temp535.last_plus_one
- _temp535.curr, _temp535.curr);})); return 0; _LL501:;} typs=({ struct Cyc_List_List*
_temp536=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp536->hd=( void*) t; _temp536->tl= typs; _temp536;}); break; case '%':
_LL499: if( _temp365){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp538=( char*)"Assignment suppression (*) is not allowed with %%%%"; struct
_tagged_string _temp539; _temp539.curr= _temp538; _temp539.base= _temp538;
_temp539.last_plus_one= _temp538 + 52; _temp539;})); return 0;} if( _temp363 !=
0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp540= Cyc_String_implode(
_temp363); xprintf("width '%.*s' not allowed with %%%%", _temp540.last_plus_one
- _temp540.curr, _temp540.curr);})); return 0;} if( _temp361 != 0){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp541= Cyc_String_implode( _temp361); xprintf("length modifier '%.*s' not allowed with %%%%",
_temp541.last_plus_one - _temp541.curr, _temp541.curr);})); return 0;} break;
default: _LL537: return 0;}}}}} return(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( typs);}