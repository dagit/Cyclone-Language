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
struct Cyc_Absyn_Tqual tq1, struct Cyc_Absyn_Tqual tq2); extern int Cyc_Tcutil_same_atts(
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
- _temp3.curr, _temp3.curr);}));} struct _tuple3{ void* f1; void* f2; } ; static
void* Cyc_Tc_scope_redecl_okay( void* s1, struct Cyc_Core_Opt* fields1, void* s2,
struct Cyc_Core_Opt* fields2, struct Cyc_Position_Segment* loc, struct
_tagged_string t, struct _tagged_string* v){{ struct _tuple3 _temp5=({ struct
_tuple3 _temp4; _temp4.f1= s1; _temp4.f2= s2; _temp4;}); void* _temp35; void*
_temp37; void* _temp39; void* _temp41; void* _temp43; void* _temp45; void*
_temp47; void* _temp49; void* _temp51; void* _temp53; void* _temp55; void*
_temp57; void* _temp59; void* _temp61; void* _temp63; void* _temp65; void*
_temp68; void* _temp70; void* _temp72; void* _temp74; void* _temp76; void*
_temp78; void* _temp80; void* _temp82; void* _temp85; void* _temp87; void*
_temp89; void* _temp91; _LL7: _LL38: _temp37= _temp5.f1; if(( int) _temp37 ==
Cyc_Absyn_ExternC){ goto _LL36;} else{ goto _LL9;} _LL36: _temp35= _temp5.f2;
if(( int) _temp35 == Cyc_Absyn_ExternC){ goto _LL8;} else{ goto _LL9;} _LL9:
_LL42: _temp41= _temp5.f1; goto _LL40; _LL40: _temp39= _temp5.f2; if(( int)
_temp39 == Cyc_Absyn_ExternC){ goto _LL10;} else{ goto _LL11;} _LL11: _LL46:
_temp45= _temp5.f1; if(( int) _temp45 == Cyc_Absyn_Static){ goto _LL44;} else{
goto _LL13;} _LL44: _temp43= _temp5.f2; if(( int) _temp43 == Cyc_Absyn_Static){
goto _LL12;} else{ goto _LL13;} _LL13: _LL50: _temp49= _temp5.f1; goto _LL48;
_LL48: _temp47= _temp5.f2; if(( int) _temp47 == Cyc_Absyn_Static){ goto _LL14;}
else{ goto _LL15;} _LL15: _LL54: _temp53= _temp5.f1; if(( int) _temp53 == Cyc_Absyn_Static){
goto _LL52;} else{ goto _LL17;} _LL52: _temp51= _temp5.f2; if(( int) _temp51 ==
Cyc_Absyn_Public){ goto _LL16;} else{ goto _LL17;} _LL17: _LL58: _temp57= _temp5.f1;
if(( int) _temp57 == Cyc_Absyn_Abstract){ goto _LL56;} else{ goto _LL19;} _LL56:
_temp55= _temp5.f2; if(( int) _temp55 == Cyc_Absyn_Public){ goto _LL18;} else{
goto _LL19;} _LL19: _LL62: _temp61= _temp5.f1; goto _LL60; _LL60: _temp59=
_temp5.f2; if(( int) _temp59 == Cyc_Absyn_Public){ goto _LL20;} else{ goto _LL21;}
_LL21: _LL66: _temp65= _temp5.f1; if(( int) _temp65 == Cyc_Absyn_Static){ goto
_LL64;} else{ goto _LL23;} _LL64: _temp63= _temp5.f2; if(( int) _temp63 == Cyc_Absyn_Extern){
goto _LL22;} else{ goto _LL23;} _LL23: _LL71: _temp70= _temp5.f1; if(( int)
_temp70 == Cyc_Absyn_Abstract){ goto _LL69;} else{ goto _LL25;} _LL69: _temp68=
_temp5.f2; if(( int) _temp68 == Cyc_Absyn_Extern){ goto _LL67;} else{ goto _LL25;}
_LL67: if( fields2 != 0){ goto _LL24;} else{ goto _LL25;} _LL25: _LL75: _temp74=
_temp5.f1; goto _LL73; _LL73: _temp72= _temp5.f2; if(( int) _temp72 == Cyc_Absyn_Extern){
goto _LL26;} else{ goto _LL27;} _LL27: _LL79: _temp78= _temp5.f1; if(( int)
_temp78 == Cyc_Absyn_Static){ goto _LL77;} else{ goto _LL29;} _LL77: _temp76=
_temp5.f2; if(( int) _temp76 == Cyc_Absyn_Abstract){ goto _LL28;} else{ goto
_LL29;} _LL29: _LL83: _temp82= _temp5.f1; if(( int) _temp82 == Cyc_Absyn_Public){
goto _LL81;} else{ goto _LL31;} _LL81: _temp80= _temp5.f2; if(( int) _temp80 ==
Cyc_Absyn_Abstract){ goto _LL30;} else{ goto _LL31;} _LL31: _LL88: _temp87=
_temp5.f1; if(( int) _temp87 == Cyc_Absyn_Extern){ goto _LL86;} else{ goto _LL33;}
_LL86: _temp85= _temp5.f2; if(( int) _temp85 == Cyc_Absyn_Abstract){ goto _LL84;}
else{ goto _LL33;} _LL84: if( fields1 != 0){ goto _LL32;} else{ goto _LL33;}
_LL33: _LL92: _temp91= _temp5.f1; goto _LL90; _LL90: _temp89= _temp5.f2; if((
int) _temp89 == Cyc_Absyn_Abstract){ goto _LL34;} else{ goto _LL6;} _LL8: goto
_LL6; _LL10: Cyc_Tc_redecl_err( loc, t, v,( struct _tagged_string)({ char*
_temp93=( char*)"non-extern \"C\""; struct _tagged_string _temp94; _temp94.curr=
_temp93; _temp94.base= _temp93; _temp94.last_plus_one= _temp93 + 15; _temp94;}),(
struct _tagged_string)({ char* _temp95=( char*)"extern \"C\""; struct
_tagged_string _temp96; _temp96.curr= _temp95; _temp96.base= _temp95; _temp96.last_plus_one=
_temp95 + 11; _temp96;})); goto _LL6; _LL12: goto _LL6; _LL14: Cyc_Tc_redecl_err(
loc, t, v,( struct _tagged_string)({ char* _temp97=( char*)"non-static"; struct
_tagged_string _temp98; _temp98.curr= _temp97; _temp98.base= _temp97; _temp98.last_plus_one=
_temp97 + 11; _temp98;}),( struct _tagged_string)({ char* _temp99=( char*)"static";
struct _tagged_string _temp100; _temp100.curr= _temp99; _temp100.base= _temp99;
_temp100.last_plus_one= _temp99 + 7; _temp100;})); goto _LL6; _LL16: Cyc_Tc_redecl_err(
loc, t, v,( struct _tagged_string)({ char* _temp101=( char*)"static"; struct
_tagged_string _temp102; _temp102.curr= _temp101; _temp102.base= _temp101;
_temp102.last_plus_one= _temp101 + 7; _temp102;}),( struct _tagged_string)({
char* _temp103=( char*)"public"; struct _tagged_string _temp104; _temp104.curr=
_temp103; _temp104.base= _temp103; _temp104.last_plus_one= _temp103 + 7;
_temp104;})); goto _LL6; _LL18: Cyc_Tc_redecl_err( loc, t, v,( struct
_tagged_string)({ char* _temp105=( char*)"abstract"; struct _tagged_string
_temp106; _temp106.curr= _temp105; _temp106.base= _temp105; _temp106.last_plus_one=
_temp105 + 9; _temp106;}),( struct _tagged_string)({ char* _temp107=( char*)"public";
struct _tagged_string _temp108; _temp108.curr= _temp107; _temp108.base= _temp107;
_temp108.last_plus_one= _temp107 + 7; _temp108;})); goto _LL6; _LL20: s1=( void*)
Cyc_Absyn_Public; goto _LL6; _LL22: Cyc_Tc_redecl_err( loc, t, v,( struct
_tagged_string)({ char* _temp109=( char*)"static"; struct _tagged_string
_temp110; _temp110.curr= _temp109; _temp110.base= _temp109; _temp110.last_plus_one=
_temp109 + 7; _temp110;}),( struct _tagged_string)({ char* _temp111=( char*)"extern";
struct _tagged_string _temp112; _temp112.curr= _temp111; _temp112.base= _temp111;
_temp112.last_plus_one= _temp111 + 7; _temp112;})); goto _LL6; _LL24: Cyc_Tc_redecl_err(
loc, t, v,( struct _tagged_string)({ char* _temp113=( char*)"abstract"; struct
_tagged_string _temp114; _temp114.curr= _temp113; _temp114.base= _temp113;
_temp114.last_plus_one= _temp113 + 9; _temp114;}),( struct _tagged_string)({
char* _temp115=( char*)"transparent"; struct _tagged_string _temp116; _temp116.curr=
_temp115; _temp116.base= _temp115; _temp116.last_plus_one= _temp115 + 12;
_temp116;})); goto _LL6; _LL26: goto _LL6; _LL28: Cyc_Tc_redecl_err( loc, t, v,(
struct _tagged_string)({ char* _temp117=( char*)"static"; struct _tagged_string
_temp118; _temp118.curr= _temp117; _temp118.base= _temp117; _temp118.last_plus_one=
_temp117 + 7; _temp118;}),( struct _tagged_string)({ char* _temp119=( char*)"abstract";
struct _tagged_string _temp120; _temp120.curr= _temp119; _temp120.base= _temp119;
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
return s1;} struct _tuple4{ struct Cyc_Absyn_Tvar* f1; void* f2; } ; struct
_tuple5{ struct Cyc_Absyn_Tqual f1; void* f2; } ; static void Cyc_Tc_field_redecl_okay(
struct Cyc_Absyn_Tunionfield* f1, struct Cyc_Absyn_Tunionfield* f2, struct Cyc_List_List*
inst, struct Cyc_Tcenv_Tenv* te, struct _tagged_string t, struct _tagged_string*
v){ struct Cyc_Position_Segment* loc= f1->loc; if( Cyc_String_zstrptrcmp((* f1->name).f2,(*
f2->name).f2) != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp129= t;
struct _tagged_string _temp130=* v; struct _tagged_string _temp131=*(* f1->name).f2;
struct _tagged_string _temp132=*(* f2->name).f2; xprintf("redeclaration of %.*s %.*s: field name mismatch %.*s != %.*s",
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
_temp161= typs2; if( _temp161 == 0){ _throw( Null_Exception);} _temp161->hd;}))).f2);
if( ! Cyc_Tcutil_unify((*(( struct _tuple5*)({ struct Cyc_List_List* _temp154=
typs1; if( _temp154 == 0){ _throw( Null_Exception);} _temp154->hd;}))).f2,
subst_t2)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp156= t; struct
_tagged_string _temp157=* v; struct _tagged_string _temp158=*(* f1->name).f2;
struct _tagged_string _temp159= Cyc_Absynpp_typ2string((*(( struct _tuple5*)({
struct Cyc_List_List* _temp155= typs1; if( _temp155 == 0){ _throw(
Null_Exception);} _temp155->hd;}))).f2); struct _tagged_string _temp160= Cyc_Absynpp_typ2string(
subst_t2); xprintf("redeclaration of %.*s %.*s, field %.*s: parameter type mismatch %.*s != %.*s",
_temp156.last_plus_one - _temp156.curr, _temp156.curr, _temp157.last_plus_one -
_temp157.curr, _temp157.curr, _temp158.last_plus_one - _temp158.curr, _temp158.curr,
_temp159.last_plus_one - _temp159.curr, _temp159.curr, _temp160.last_plus_one -
_temp160.curr, _temp160.curr);}));}}}}}} struct _tuple6{ void* f1; int f2; } ;
static int Cyc_Tc_var_redecl_okay( struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment*
loc, struct _tagged_string* name, void* t, void* sc, struct Cyc_List_List* atts,
int check_no_fun){ struct Cyc_Core_Opt* ans=(( struct Cyc_Core_Opt*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup_opt)( ge->ordinaries,
name); if( ans == 0){ return 0;} else{ void* sc2; void* t2; struct Cyc_List_List*
atts2;{ void* _temp163=(*(( struct _tuple6*)({ struct Cyc_Core_Opt* _temp162=
ans; if( _temp162 == 0){ _throw( Null_Exception);} _temp162->v;}))).f1; void*
_temp171; struct Cyc_Absyn_Fndecl* _temp173; void* _temp175; struct Cyc_Absyn_Vardecl*
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
_temp257; struct Cyc_Absyn_Tqual _temp259; void* _temp261; _LL252: if((
unsigned int) _temp250 > 4u?(( struct _tunion_struct*) _temp250)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL262: _temp261=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp250)->f1;
goto _LL260; _LL260: _temp259=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
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
struct _tuple7{ struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; } ; static
void Cyc_Tc_tcStructdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge,
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Structdecl* sd){ if( sd->name
== 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp420=( char*)"anonymous structs are not allowed at top level";
struct _tagged_string _temp421; _temp421.curr= _temp420; _temp421.base= _temp420;
_temp421.last_plus_one= _temp420 + 47; _temp421;})); return;}{ struct
_tagged_string* v=(*(( struct _tuple0*)({ struct Cyc_Core_Opt* _temp661= sd->name;
if( _temp661 == 0){ _throw( Null_Exception);} _temp661->v;}))).f2;{ struct Cyc_List_List*
atts= sd->attributes; for( 0; atts != 0; atts=({ struct Cyc_List_List* _temp422=
atts; if( _temp422 == 0){ _throw( Null_Exception);} _temp422->tl;})){ void*
_temp424=( void*)({ struct Cyc_List_List* _temp423= atts; if( _temp423 == 0){
_throw( Null_Exception);} _temp423->hd;}); int _temp432; _LL426: if(( int)
_temp424 == Cyc_Absyn_Packed_att){ goto _LL427;} else{ goto _LL428;} _LL428: if((
unsigned int) _temp424 > 15u?(( struct _tunion_struct*) _temp424)->tag == Cyc_Absyn_Aligned_att_tag:
0){ _LL433: _temp432=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp424)->f1;
goto _LL429;} else{ goto _LL430;} _LL430: goto _LL431; _LL427: continue; _LL429:
continue; _LL431: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp435= Cyc_Absyn_attribute2string((
void*)({ struct Cyc_List_List* _temp434= atts; if( _temp434 == 0){ _throw(
Null_Exception);} _temp434->hd;})); struct _tagged_string _temp436=* v; xprintf("bad attribute %.*s in struct %.*s definition",
_temp435.last_plus_one - _temp435.curr, _temp435.curr, _temp436.last_plus_one -
_temp436.curr, _temp436.curr);})); goto _LL425; _LL425:;}}{ struct Cyc_List_List*
tvs2= sd->tvs; for( 0; tvs2 != 0; tvs2=({ struct Cyc_List_List* _temp437= tvs2;
if( _temp437 == 0){ _throw( Null_Exception);} _temp437->tl;})){ struct Cyc_Absyn_Conref*
c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp453= tvs2; if( _temp453 ==
0){ _throw( Null_Exception);} _temp453->hd;}))->kind); void* _temp438=( void*) c->v;
void* _temp446; _LL440: if(( int) _temp438 == Cyc_Absyn_No_constr){ goto _LL441;}
else{ goto _LL442;} _LL442: if(( unsigned int) _temp438 > 1u?(( struct
_tunion_struct*) _temp438)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL447: _temp446=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp438)->f1; if(( int) _temp446
== Cyc_Absyn_MemKind){ goto _LL443;} else{ goto _LL444;}} else{ goto _LL444;}
_LL444: goto _LL445; _LL441:( void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp448=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp448[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp449; _temp449.tag= Cyc_Absyn_Eq_constr_tag;
_temp449.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp449;}); _temp448;})));
continue; _LL443: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp451=* v;
struct _tagged_string _temp452=*(( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp450= tvs2; if( _temp450 == 0){ _throw( Null_Exception);} _temp450->hd;}))->name;
xprintf("struct %.*s attempts to abstract type variable %.*s of kind M",
_temp451.last_plus_one - _temp451.curr, _temp451.curr, _temp452.last_plus_one -
_temp452.curr, _temp452.curr);})); continue; _LL445: continue; _LL439:;}}{
struct Cyc_List_List* tvs= sd->tvs;{ void* _temp455=(*(( struct _tuple0*)({
struct Cyc_Core_Opt* _temp454= sd->name; if( _temp454 == 0){ _throw(
Null_Exception);} _temp454->v;}))).f1; struct Cyc_List_List* _temp463; struct
Cyc_List_List* _temp465; _LL457: if(( unsigned int) _temp455 > 1u?(( struct
_tunion_struct*) _temp455)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL464: _temp463=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp455)->f1; if(
_temp463 == 0){ goto _LL458;} else{ goto _LL459;}} else{ goto _LL459;} _LL459:
if(( unsigned int) _temp455 > 1u?(( struct _tunion_struct*) _temp455)->tag ==
Cyc_Absyn_Abs_n_tag: 0){ _LL466: _temp465=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp455)->f1; if( _temp465 == 0){ goto _LL460;} else{ goto _LL461;}} else{ goto
_LL461;} _LL461: goto _LL462; _LL458: goto _LL456; _LL460: goto _LL456; _LL462:
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp467=( char*)"qualified declarations are not implemented";
struct _tagged_string _temp468; _temp468.curr= _temp467; _temp468.base= _temp467;
_temp468.last_plus_one= _temp467 + 43; _temp468;})); return; _LL456:;}(*((
struct _tuple0*)({ struct Cyc_Core_Opt* _temp469= sd->name; if( _temp469 == 0){
_throw( Null_Exception);} _temp469->v;}))).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp470=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp470[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp471; _temp471.tag= Cyc_Absyn_Abs_n_tag;
_temp471.f1= te->ns; _temp471;}); _temp470;}); Cyc_Tcutil_check_unique_tvars(
loc, tvs);{ struct Cyc_Core_Opt* _temp472=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup_opt)( ge->structdecls, v);
struct Cyc_Core_Opt _temp478; struct Cyc_Absyn_Structdecl** _temp479; _LL474:
if( _temp472 == 0){ goto _LL475;} else{ goto _LL476;} _LL476: if( _temp472 == 0){
goto _LL473;} else{ _temp478=* _temp472; _LL480: _temp479=( struct Cyc_Absyn_Structdecl**)
_temp478.v; goto _LL477;} _LL475: ge->structdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Structdecl**
data)) Cyc_Dict_insert)( ge->structdecls, v,({ struct Cyc_Absyn_Structdecl**
_temp481=( struct Cyc_Absyn_Structdecl**) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*));
_temp481[ 0]= sd; _temp481;})); if( sd->fields == 0){ return;} goto _LL473;
_LL477: { struct Cyc_Absyn_Structdecl* sd2=* _temp479; struct Cyc_List_List*
tvs2= sd2->tvs; if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs)
!=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs2)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp482=* v; xprintf("redeclaration of struct %.*s has a different number of type parameters",
_temp482.last_plus_one - _temp482.curr, _temp482.curr);}));}{ struct Cyc_List_List*
x1= tvs; struct Cyc_List_List* x2= tvs2; for( 0; x1 != 0; x1=({ struct Cyc_List_List*
_temp483= x1; if( _temp483 == 0){ _throw( Null_Exception);} _temp483->tl;}), x2=({
struct Cyc_List_List* _temp484= x2; if( _temp484 == 0){ _throw( Null_Exception);}
_temp484->tl;})){ struct Cyc_Absyn_Conref* c1=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp493= x1; if( _temp493 == 0){ _throw( Null_Exception);}
_temp493->hd;}))->kind); struct Cyc_Absyn_Conref* c2=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp492= x2; if( _temp492 == 0){ _throw( Null_Exception);}
_temp492->hd;}))->kind); if(( void*) c1->v ==( void*) Cyc_Absyn_No_constr? c1 !=
c2: 0){( void*)( c1->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp485=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp485[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp486; _temp486.tag= Cyc_Absyn_Forward_constr_tag; _temp486.f1= c2; _temp486;});
_temp485;})));} if((( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
c1) !=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( c2)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp488=* v; struct _tagged_string _temp489= Cyc_Absynpp_ckind2string(
c1); struct _tagged_string _temp490=*(( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp487= x1; if( _temp487 == 0){ _throw( Null_Exception);} _temp487->hd;}))->name;
struct _tagged_string _temp491= Cyc_Absynpp_ckind2string( c2); xprintf("redeclaration of struct %.*s has a different kind (%.*s) for type parameter %.*s (%.*s)",
_temp488.last_plus_one - _temp488.curr, _temp488.curr, _temp489.last_plus_one -
_temp489.curr, _temp489.curr, _temp490.last_plus_one - _temp490.curr, _temp490.curr,
_temp491.last_plus_one - _temp491.curr, _temp491.curr);}));}}( void*)( sd->sc=(
void*)(( void*(*)( void* s1, struct Cyc_Core_Opt* fields1, void* s2, struct Cyc_Core_Opt*
fields2, struct Cyc_Position_Segment* loc, struct _tagged_string t, struct
_tagged_string* v)) Cyc_Tc_scope_redecl_okay)(( void*) sd->sc, sd->fields,( void*)
sd2->sc, sd2->fields, loc,( struct _tagged_string)({ char* _temp494=( char*)"struct";
struct _tagged_string _temp495; _temp495.curr= _temp494; _temp495.base= _temp494;
_temp495.last_plus_one= _temp494 + 7; _temp495;}), v));{ struct _tuple7 _temp497=({
struct _tuple7 _temp496; _temp496.f1= sd->fields; _temp496.f2= sd2->fields;
_temp496;}); struct Cyc_Core_Opt* _temp507; struct Cyc_Core_Opt* _temp509;
struct Cyc_Core_Opt* _temp511; struct Cyc_Core_Opt* _temp513; struct Cyc_Core_Opt*
_temp515; struct Cyc_Core_Opt* _temp517; struct Cyc_Core_Opt* _temp519; struct
Cyc_Core_Opt* _temp521; _LL499: _LL510: _temp509= _temp497.f1; if( _temp509 == 0){
goto _LL508;} else{ goto _LL501;} _LL508: _temp507= _temp497.f2; if( _temp507 ==
0){ goto _LL500;} else{ goto _LL501;} _LL501: _LL514: _temp513= _temp497.f1;
goto _LL512; _LL512: _temp511= _temp497.f2; if( _temp511 == 0){ goto _LL502;}
else{ goto _LL503;} _LL503: _LL518: _temp517= _temp497.f1; if( _temp517 == 0){
goto _LL516;} else{ goto _LL505;} _LL516: _temp515= _temp497.f2; goto _LL504;
_LL505: _LL522: _temp521= _temp497.f1; goto _LL520; _LL520: _temp519= _temp497.f2;
goto _LL506; _LL500: return; _LL502:* _temp479= sd; goto _LL498; _LL504: sd->fields=
sd2->fields; sd->tvs= tvs; return; _LL506: { struct Cyc_List_List* inst= 0;{
struct Cyc_List_List* tvs0= tvs; for( 0; tvs0 != 0; tvs0=({ struct Cyc_List_List*
_temp523= tvs0; if( _temp523 == 0){ _throw( Null_Exception);} _temp523->tl;}),
tvs2=({ struct Cyc_List_List* _temp524= tvs2; if( _temp524 == 0){ _throw(
Null_Exception);} _temp524->tl;})){ inst=({ struct Cyc_List_List* _temp525=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp525->hd=(
void*)({ struct _tuple4* _temp526=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp526->f1=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp530= tvs2; if( _temp530 == 0){ _throw( Null_Exception);} _temp530->hd;});
_temp526->f2=( void*)({ struct Cyc_Absyn_VarType_struct* _temp527=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp527[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp528; _temp528.tag= Cyc_Absyn_VarType_tag; _temp528.f1=( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp529= tvs0; if( _temp529 == 0){ _throw( Null_Exception);}
_temp529->hd;}); _temp528;}); _temp527;}); _temp526;}); _temp525->tl= inst;
_temp525;});}} if( ! Cyc_Tcutil_same_atts( sd->attributes, sd2->attributes)){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp531=* v; xprintf("redeclaration of struct %.*s has different attributes",
_temp531.last_plus_one - _temp531.curr, _temp531.curr);}));}{ struct Cyc_List_List*
f1s=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp587= sd->fields; if(
_temp587 == 0){ _throw( Null_Exception);} _temp587->v;}); struct Cyc_List_List*
f2s=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp586= sd2->fields; if(
_temp586 == 0){ _throw( Null_Exception);} _temp586->v;}); for( 0; f1s != 0? f2s
!= 0: 0; f1s=({ struct Cyc_List_List* _temp532= f1s; if( _temp532 == 0){ _throw(
Null_Exception);} _temp532->tl;}), f2s=({ struct Cyc_List_List* _temp533= f2s;
if( _temp533 == 0){ _throw( Null_Exception);} _temp533->tl;})){ struct Cyc_Absyn_Structfield
_temp537; struct Cyc_List_List* _temp538; struct Cyc_Core_Opt* _temp540; void*
_temp542; struct Cyc_Absyn_Tqual _temp544; struct _tagged_string* _temp546;
struct Cyc_Absyn_Structfield* _temp535=( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp534= f1s; if( _temp534 == 0){ _throw( Null_Exception);}
_temp534->hd;}); _temp537=* _temp535; _LL547: _temp546=( struct _tagged_string*)
_temp537.name; goto _LL545; _LL545: _temp544=( struct Cyc_Absyn_Tqual) _temp537.tq;
goto _LL543; _LL543: _temp542=( void*) _temp537.type; goto _LL541; _LL541:
_temp540=( struct Cyc_Core_Opt*) _temp537.width; goto _LL539; _LL539: _temp538=(
struct Cyc_List_List*) _temp537.attributes; goto _LL536; _LL536: { struct Cyc_Absyn_Structfield
_temp551; struct Cyc_List_List* _temp552; struct Cyc_Core_Opt* _temp554; void*
_temp556; struct Cyc_Absyn_Tqual _temp558; struct _tagged_string* _temp560;
struct Cyc_Absyn_Structfield* _temp549=( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp548= f2s; if( _temp548 == 0){ _throw( Null_Exception);}
_temp548->hd;}); _temp551=* _temp549; _LL561: _temp560=( struct _tagged_string*)
_temp551.name; goto _LL559; _LL559: _temp558=( struct Cyc_Absyn_Tqual) _temp551.tq;
goto _LL557; _LL557: _temp556=( void*) _temp551.type; goto _LL555; _LL555:
_temp554=( struct Cyc_Core_Opt*) _temp551.width; goto _LL553; _LL553: _temp552=(
struct Cyc_List_List*) _temp551.attributes; goto _LL550; _LL550: if( Cyc_String_zstrptrcmp(
_temp546, _temp560) != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp562=* v; struct _tagged_string _temp563=* _temp546; struct _tagged_string
_temp564=* _temp560; xprintf("redeclaration of struct %.*s: field name mismatch %.*s != %.*s",
_temp562.last_plus_one - _temp562.curr, _temp562.curr, _temp563.last_plus_one -
_temp563.curr, _temp563.curr, _temp564.last_plus_one - _temp564.curr, _temp564.curr);}));}
if( ! Cyc_Tcutil_same_atts( _temp538, _temp552)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp565=* v; struct _tagged_string _temp566=* _temp546; xprintf("redeclaration of struct %.*s: attribute mismatch on field %.*s",
_temp565.last_plus_one - _temp565.curr, _temp565.curr, _temp566.last_plus_one -
_temp566.curr, _temp566.curr);}));} if( ! Cyc_Tcutil_equal_tqual( _temp544,
_temp558)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp567=* v; struct
_tagged_string _temp568=* _temp546; xprintf("redeclaration of struct %.*s: qualifier mismatch on field %.*s",
_temp567.last_plus_one - _temp567.curr, _temp567.curr, _temp568.last_plus_one -
_temp568.curr, _temp568.curr);}));} if( _temp540 != 0? ! Cyc_Tcutil_is_const_exp(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp569= _temp540; if(
_temp569 == 0){ _throw( Null_Exception);} _temp569->v;})): 0){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp570=* v; struct _tagged_string _temp571=*
_temp546; xprintf("redeclaration of struct %.*s: bad bitfield %.*s", _temp570.last_plus_one
- _temp570.curr, _temp570.curr, _temp571.last_plus_one - _temp571.curr, _temp571.curr);}));}
else{ if((( _temp540 != 0? _temp554 != 0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp572= _temp540; if( _temp572
== 0){ _throw( Null_Exception);} _temp572->v;})) != Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp573= _temp554; if( _temp573
== 0){ _throw( Null_Exception);} _temp573->v;})): 0)? 1: _temp540 != _temp554){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp574=* v; struct
_tagged_string _temp575=* _temp546; xprintf("redeclaration of struct %.*s: bitfield mismatch on field %.*s",
_temp574.last_plus_one - _temp574.curr, _temp574.curr, _temp575.last_plus_one -
_temp575.curr, _temp575.curr);}));}} Cyc_Tcutil_check_type( loc, te, tvs,( void*)
Cyc_Absyn_MemKind, _temp542);{ void* subst_t2= Cyc_Tcutil_substitute( inst,
_temp556); if( ! Cyc_Tcutil_unify( _temp542, subst_t2)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp576=* v; struct _tagged_string _temp577=* _temp546;
struct _tagged_string _temp578= Cyc_Absynpp_typ2string( _temp542); struct
_tagged_string _temp579= Cyc_Absynpp_typ2string( subst_t2); xprintf("redeclaration of struct %.*s: type mismatch on field %.*s: %.*s != %.*s",
_temp576.last_plus_one - _temp576.curr, _temp576.curr, _temp577.last_plus_one -
_temp577.curr, _temp577.curr, _temp578.last_plus_one - _temp578.curr, _temp578.curr,
_temp579.last_plus_one - _temp579.curr, _temp579.curr);}));}}}} if( f2s != 0){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp581=* v; struct
_tagged_string _temp582=*(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp580= f2s; if( _temp580 == 0){ _throw( Null_Exception);} _temp580->hd;}))->name;
xprintf("redeclaration of struct %.*s is missing field %.*s", _temp581.last_plus_one
- _temp581.curr, _temp581.curr, _temp582.last_plus_one - _temp582.curr, _temp582.curr);}));}
if( f1s != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp584=* v;
struct _tagged_string _temp585=*(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp583= f1s; if( _temp583 == 0){ _throw( Null_Exception);} _temp583->hd;}))->name;
xprintf("redeclaration of struct %.*s has extra field %.*s", _temp584.last_plus_one
- _temp584.curr, _temp584.curr, _temp585.last_plus_one - _temp585.curr, _temp585.curr);}));}
return;}} _LL498:;} goto _LL473;}} _LL473:;}{ struct Cyc_Absyn_Structdecl** sdp=((
struct Cyc_Absyn_Structdecl**(*)( struct Cyc_Dict_Dict* d, struct _tagged_string*
key)) Cyc_Dict_lookup)( ge->structdecls, v);* sdp=({ struct Cyc_Absyn_Structdecl*
_temp588=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp588->sc=( void*)(( void*) sd->sc); _temp588->name= sd->name; _temp588->tvs=
tvs; _temp588->fields= 0; _temp588->attributes= 0; _temp588;});{ struct Cyc_List_List*
prev_fields= 0;{ struct Cyc_List_List* fs=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp657= sd->fields; if( _temp657 == 0){ _throw( Null_Exception);} _temp657->v;});
for( 0; fs != 0; fs=({ struct Cyc_List_List* _temp589= fs; if( _temp589 == 0){
_throw( Null_Exception);} _temp589->tl;})){ struct Cyc_Absyn_Structfield
_temp593; struct Cyc_List_List* _temp594; struct Cyc_Core_Opt* _temp596; void*
_temp598; struct Cyc_Absyn_Tqual _temp600; struct _tagged_string* _temp602;
struct Cyc_Absyn_Structfield* _temp591=( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp590= fs; if( _temp590 == 0){ _throw( Null_Exception);}
_temp590->hd;}); _temp593=* _temp591; _LL603: _temp602=( struct _tagged_string*)
_temp593.name; goto _LL601; _LL601: _temp600=( struct Cyc_Absyn_Tqual) _temp593.tq;
goto _LL599; _LL599: _temp598=( void*) _temp593.type; goto _LL597; _LL597:
_temp596=( struct Cyc_Core_Opt*) _temp593.width; goto _LL595; _LL595: _temp594=(
struct Cyc_List_List*) _temp593.attributes; goto _LL592; _LL592: if((( int(*)(
int(* compare)( struct _tagged_string*, struct _tagged_string*), struct Cyc_List_List*
l, struct _tagged_string* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp, prev_fields,
_temp602)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp604=* _temp602;
xprintf("duplicate field %.*s in struct", _temp604.last_plus_one - _temp604.curr,
_temp604.curr);}));} if( Cyc_String_strcmp(* _temp602,( struct _tagged_string)({
char* _temp605=( char*)""; struct _tagged_string _temp606; _temp606.curr=
_temp605; _temp606.base= _temp605; _temp606.last_plus_one= _temp605 + 1;
_temp606;})) != 0){ prev_fields=({ struct Cyc_List_List* _temp607=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp607->hd=( void*) _temp602;
_temp607->tl= prev_fields; _temp607;});} Cyc_Tcutil_check_type( loc, te, tvs,(
void*) Cyc_Absyn_MemKind, _temp598); if( _temp596 != 0){ unsigned int w= 0; if(
! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt*
_temp608= _temp596; if( _temp608 == 0){ _throw( Null_Exception);} _temp608->v;}))){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp609=* _temp602; xprintf("bitfield %.*s does not have constant width",
_temp609.last_plus_one - _temp609.curr, _temp609.curr);}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp610= _temp596; if( _temp610
== 0){ _throw( Null_Exception);} _temp610->v;}));}{ void* _temp611= Cyc_Tcutil_compress(
_temp598); void* _temp617; void* _temp619; _LL613: if(( unsigned int) _temp611 >
4u?(( struct _tunion_struct*) _temp611)->tag == Cyc_Absyn_IntType_tag: 0){
_LL620: _temp619=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp611)->f1;
goto _LL618; _LL618: _temp617=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp611)->f2; goto _LL614;} else{ goto _LL615;} _LL615: goto _LL616; _LL614:{
void* _temp621= _temp617; _LL623: if(( int) _temp621 == Cyc_Absyn_B1){ goto
_LL624;} else{ goto _LL625;} _LL625: if(( int) _temp621 == Cyc_Absyn_B2){ goto
_LL626;} else{ goto _LL627;} _LL627: if(( int) _temp621 == Cyc_Absyn_B4){ goto
_LL628;} else{ goto _LL629;} _LL629: if(( int) _temp621 == Cyc_Absyn_B8){ goto
_LL630;} else{ goto _LL622;} _LL624: if( w > 8){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp631=( char*)"bitfield larger than type"; struct
_tagged_string _temp632; _temp632.curr= _temp631; _temp632.base= _temp631;
_temp632.last_plus_one= _temp631 + 26; _temp632;}));} goto _LL622; _LL626: if( w
> 16){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp633=( char*)"bitfield larger than type";
struct _tagged_string _temp634; _temp634.curr= _temp633; _temp634.base= _temp633;
_temp634.last_plus_one= _temp633 + 26; _temp634;}));} goto _LL622; _LL628: if( w
> 32){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp635=( char*)"bitfield larger than type";
struct _tagged_string _temp636; _temp636.curr= _temp635; _temp636.base= _temp635;
_temp636.last_plus_one= _temp635 + 26; _temp636;}));} goto _LL622; _LL630: if( w
> 64){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp637=( char*)"bitfield larger than type";
struct _tagged_string _temp638; _temp638.curr= _temp637; _temp638.base= _temp637;
_temp638.last_plus_one= _temp637 + 26; _temp638;}));} goto _LL622; _LL622:;}
goto _LL612; _LL616: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp639=*
_temp602; struct _tagged_string _temp640= Cyc_Absynpp_typ2string( _temp598);
xprintf("bitfield %.*s must have integral type but has type %.*s", _temp639.last_plus_one
- _temp639.curr, _temp639.curr, _temp640.last_plus_one - _temp640.curr, _temp640.curr);}));
goto _LL612; _LL612:;}} for( 0; _temp594 != 0; _temp594=({ struct Cyc_List_List*
_temp641= _temp594; if( _temp641 == 0){ _throw( Null_Exception);} _temp641->tl;})){
void* _temp643=( void*)({ struct Cyc_List_List* _temp642= _temp594; if( _temp642
== 0){ _throw( Null_Exception);} _temp642->hd;}); int _temp651; _LL645: if(( int)
_temp643 == Cyc_Absyn_Packed_att){ goto _LL646;} else{ goto _LL647;} _LL647: if((
unsigned int) _temp643 > 15u?(( struct _tunion_struct*) _temp643)->tag == Cyc_Absyn_Aligned_att_tag:
0){ _LL652: _temp651=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp643)->f1;
goto _LL648;} else{ goto _LL649;} _LL649: goto _LL650; _LL646: continue; _LL648:
continue; _LL650: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp654= Cyc_Absyn_attribute2string((
void*)({ struct Cyc_List_List* _temp653= _temp594; if( _temp653 == 0){ _throw(
Null_Exception);} _temp653->hd;})); struct _tagged_string _temp655=* v; struct
_tagged_string _temp656=* _temp602; xprintf("bad attribute %.*s in struct %.*s, member %.*s",
_temp654.last_plus_one - _temp654.curr, _temp654.curr, _temp655.last_plus_one -
_temp655.curr, _temp655.curr, _temp656.last_plus_one - _temp656.curr, _temp656.curr);}));
goto _LL644; _LL644:;}}} ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct _tuple6* data)) Cyc_Dict_insert)( ge->ordinaries,
v,({ struct _tuple6* _temp658=( struct _tuple6*) GC_malloc( sizeof( struct
_tuple6)); _temp658->f1=( void*)({ struct Cyc_Tcenv_StructRes_struct* _temp659=(
struct Cyc_Tcenv_StructRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_StructRes_struct));
_temp659[ 0]=({ struct Cyc_Tcenv_StructRes_struct _temp660; _temp660.tag= Cyc_Tcenv_StructRes_tag;
_temp660.f1= sd; _temp660;}); _temp659;}); _temp658->f2= 1; _temp658;}));* sdp=
sd;}}}}} static void Cyc_Tc_tcUniondecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Uniondecl* ud){ if( ud->name
== 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp662=( char*)"anonymous unions are not allowed at top level";
struct _tagged_string _temp663; _temp663.curr= _temp662; _temp663.base= _temp662;
_temp663.last_plus_one= _temp662 + 46; _temp663;})); return;}{ struct
_tagged_string* v=(*(( struct _tuple0*)({ struct Cyc_Core_Opt* _temp900= ud->name;
if( _temp900 == 0){ _throw( Null_Exception);} _temp900->v;}))).f2;{ struct Cyc_List_List*
atts= ud->attributes; for( 0; atts != 0; atts=({ struct Cyc_List_List* _temp664=
atts; if( _temp664 == 0){ _throw( Null_Exception);} _temp664->tl;})){ void*
_temp666=( void*)({ struct Cyc_List_List* _temp665= atts; if( _temp665 == 0){
_throw( Null_Exception);} _temp665->hd;}); int _temp674; _LL668: if(( int)
_temp666 == Cyc_Absyn_Packed_att){ goto _LL669;} else{ goto _LL670;} _LL670: if((
unsigned int) _temp666 > 15u?(( struct _tunion_struct*) _temp666)->tag == Cyc_Absyn_Aligned_att_tag:
0){ _LL675: _temp674=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp666)->f1;
goto _LL671;} else{ goto _LL672;} _LL672: goto _LL673; _LL669: continue; _LL671:
continue; _LL673: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp677= Cyc_Absyn_attribute2string((
void*)({ struct Cyc_List_List* _temp676= atts; if( _temp676 == 0){ _throw(
Null_Exception);} _temp676->hd;})); struct _tagged_string _temp678=* v; xprintf("bad attribute %.*s in union %.*s definition",
_temp677.last_plus_one - _temp677.curr, _temp677.curr, _temp678.last_plus_one -
_temp678.curr, _temp678.curr);})); goto _LL667; _LL667:;}}{ struct Cyc_List_List*
tvs2= ud->tvs; for( 0; tvs2 != 0; tvs2=({ struct Cyc_List_List* _temp679= tvs2;
if( _temp679 == 0){ _throw( Null_Exception);} _temp679->tl;})){ struct Cyc_Absyn_Conref*
c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp695= tvs2; if( _temp695 ==
0){ _throw( Null_Exception);} _temp695->hd;}))->kind); void* _temp680=( void*) c->v;
void* _temp688; _LL682: if(( int) _temp680 == Cyc_Absyn_No_constr){ goto _LL683;}
else{ goto _LL684;} _LL684: if(( unsigned int) _temp680 > 1u?(( struct
_tunion_struct*) _temp680)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL689: _temp688=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp680)->f1; if(( int) _temp688
== Cyc_Absyn_MemKind){ goto _LL685;} else{ goto _LL686;}} else{ goto _LL686;}
_LL686: goto _LL687; _LL683:( void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp690=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp690[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp691; _temp691.tag= Cyc_Absyn_Eq_constr_tag;
_temp691.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp691;}); _temp690;})));
continue; _LL685: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp693=* v;
struct _tagged_string _temp694=*(( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp692= tvs2; if( _temp692 == 0){ _throw( Null_Exception);} _temp692->hd;}))->name;
xprintf("union %.*s attempts to abstract type variable %.*s of kind M", _temp693.last_plus_one
- _temp693.curr, _temp693.curr, _temp694.last_plus_one - _temp694.curr, _temp694.curr);}));
continue; _LL687: continue; _LL681:;}}{ struct Cyc_List_List* tvs= ud->tvs;{
void* _temp697=(*(( struct _tuple0*)({ struct Cyc_Core_Opt* _temp696= ud->name;
if( _temp696 == 0){ _throw( Null_Exception);} _temp696->v;}))).f1; struct Cyc_List_List*
_temp705; struct Cyc_List_List* _temp707; _LL699: if(( unsigned int) _temp697 >
1u?(( struct _tunion_struct*) _temp697)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL706:
_temp705=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp697)->f1;
if( _temp705 == 0){ goto _LL700;} else{ goto _LL701;}} else{ goto _LL701;}
_LL701: if(( unsigned int) _temp697 > 1u?(( struct _tunion_struct*) _temp697)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL708: _temp707=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp697)->f1; if( _temp707 == 0){ goto _LL702;} else{
goto _LL703;}} else{ goto _LL703;} _LL703: goto _LL704; _LL700: goto _LL698;
_LL702: goto _LL698; _LL704: Cyc_Tcutil_terr( loc,( struct _tagged_string)({
char* _temp709=( char*)"qualified declarations are not implemented"; struct
_tagged_string _temp710; _temp710.curr= _temp709; _temp710.base= _temp709;
_temp710.last_plus_one= _temp709 + 43; _temp710;})); return; _LL698:;}(*((
struct _tuple0*)({ struct Cyc_Core_Opt* _temp711= ud->name; if( _temp711 == 0){
_throw( Null_Exception);} _temp711->v;}))).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp712=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp712[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp713; _temp713.tag= Cyc_Absyn_Abs_n_tag;
_temp713.f1= te->ns; _temp713;}); _temp712;}); Cyc_Tcutil_check_unique_tvars(
loc, tvs);{ struct Cyc_Core_Opt* _temp714=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup_opt)( ge->uniondecls, v); struct
Cyc_Core_Opt _temp720; struct Cyc_Absyn_Uniondecl** _temp721; _LL716: if(
_temp714 == 0){ goto _LL717;} else{ goto _LL718;} _LL718: if( _temp714 == 0){
goto _LL715;} else{ _temp720=* _temp714; _LL722: _temp721=( struct Cyc_Absyn_Uniondecl**)
_temp720.v; goto _LL719;} _LL717: ge->uniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl**
data)) Cyc_Dict_insert)( ge->uniondecls, v,({ struct Cyc_Absyn_Uniondecl**
_temp723=( struct Cyc_Absyn_Uniondecl**) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl*));
_temp723[ 0]= ud; _temp723;})); if( ud->fields == 0){ return;} goto _LL715;
_LL719: { struct Cyc_Absyn_Uniondecl* ud2=* _temp721; struct Cyc_List_List* tvs2=
ud2->tvs; if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs) !=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs2)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp724=* v; xprintf("redeclaration of union %.*s has a different number of type parameters",
_temp724.last_plus_one - _temp724.curr, _temp724.curr);}));}{ struct Cyc_List_List*
x1= tvs; struct Cyc_List_List* x2= tvs2; for( 0; x1 != 0; x1=({ struct Cyc_List_List*
_temp725= x1; if( _temp725 == 0){ _throw( Null_Exception);} _temp725->tl;}), x2=({
struct Cyc_List_List* _temp726= x2; if( _temp726 == 0){ _throw( Null_Exception);}
_temp726->tl;})){ struct Cyc_Absyn_Conref* c1=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp735= x1; if( _temp735 == 0){ _throw( Null_Exception);}
_temp735->hd;}))->kind); struct Cyc_Absyn_Conref* c2=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp734= x2; if( _temp734 == 0){ _throw( Null_Exception);}
_temp734->hd;}))->kind); if(( void*) c1->v ==( void*) Cyc_Absyn_No_constr? c1 !=
c2: 0){( void*)( c1->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp727=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp727[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp728; _temp728.tag= Cyc_Absyn_Forward_constr_tag; _temp728.f1= c2; _temp728;});
_temp727;})));} if((( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
c1) !=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( c2)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp730=* v; struct _tagged_string _temp731= Cyc_Absynpp_ckind2string(
c1); struct _tagged_string _temp732=*(( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp729= x1; if( _temp729 == 0){ _throw( Null_Exception);} _temp729->hd;}))->name;
struct _tagged_string _temp733= Cyc_Absynpp_ckind2string( c2); xprintf("redeclaration of union %.*s has a different kind (%.*s) for type parameter %.*s (%.*s)",
_temp730.last_plus_one - _temp730.curr, _temp730.curr, _temp731.last_plus_one -
_temp731.curr, _temp731.curr, _temp732.last_plus_one - _temp732.curr, _temp732.curr,
_temp733.last_plus_one - _temp733.curr, _temp733.curr);}));}}( void*)( ud->sc=(
void*)(( void*(*)( void* s1, struct Cyc_Core_Opt* fields1, void* s2, struct Cyc_Core_Opt*
fields2, struct Cyc_Position_Segment* loc, struct _tagged_string t, struct
_tagged_string* v)) Cyc_Tc_scope_redecl_okay)(( void*) ud->sc, ud->fields,( void*)
ud2->sc, ud2->fields, loc,( struct _tagged_string)({ char* _temp736=( char*)"union";
struct _tagged_string _temp737; _temp737.curr= _temp736; _temp737.base= _temp736;
_temp737.last_plus_one= _temp736 + 6; _temp737;}), v));{ struct _tuple7 _temp739=({
struct _tuple7 _temp738; _temp738.f1= ud->fields; _temp738.f2= ud2->fields;
_temp738;}); struct Cyc_Core_Opt* _temp749; struct Cyc_Core_Opt* _temp751;
struct Cyc_Core_Opt* _temp753; struct Cyc_Core_Opt* _temp755; struct Cyc_Core_Opt*
_temp757; struct Cyc_Core_Opt* _temp759; struct Cyc_Core_Opt* _temp761; struct
Cyc_Core_Opt* _temp763; _LL741: _LL752: _temp751= _temp739.f1; if( _temp751 == 0){
goto _LL750;} else{ goto _LL743;} _LL750: _temp749= _temp739.f2; if( _temp749 ==
0){ goto _LL742;} else{ goto _LL743;} _LL743: _LL756: _temp755= _temp739.f1;
goto _LL754; _LL754: _temp753= _temp739.f2; if( _temp753 == 0){ goto _LL744;}
else{ goto _LL745;} _LL745: _LL760: _temp759= _temp739.f1; if( _temp759 == 0){
goto _LL758;} else{ goto _LL747;} _LL758: _temp757= _temp739.f2; goto _LL746;
_LL747: _LL764: _temp763= _temp739.f1; goto _LL762; _LL762: _temp761= _temp739.f2;
goto _LL748; _LL742: return; _LL744:* _temp721= ud; goto _LL740; _LL746: ud->fields=
ud2->fields; ud->tvs= tvs; return; _LL748: { struct Cyc_List_List* inst= 0;{
struct Cyc_List_List* tvs0= tvs; for( 0; tvs0 != 0; tvs0=({ struct Cyc_List_List*
_temp765= tvs0; if( _temp765 == 0){ _throw( Null_Exception);} _temp765->tl;}),
tvs2=({ struct Cyc_List_List* _temp766= tvs2; if( _temp766 == 0){ _throw(
Null_Exception);} _temp766->tl;})){ inst=({ struct Cyc_List_List* _temp767=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp767->hd=(
void*)({ struct _tuple4* _temp768=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp768->f1=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp772= tvs2; if( _temp772 == 0){ _throw( Null_Exception);} _temp772->hd;});
_temp768->f2=( void*)({ struct Cyc_Absyn_VarType_struct* _temp769=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp769[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp770; _temp770.tag= Cyc_Absyn_VarType_tag; _temp770.f1=( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp771= tvs0; if( _temp771 == 0){ _throw( Null_Exception);}
_temp771->hd;}); _temp770;}); _temp769;}); _temp768;}); _temp767->tl= inst;
_temp767;});}} if( ! Cyc_Tcutil_same_atts( ud->attributes, ud2->attributes)){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp773=* v; xprintf("redeclaration of union %.*s has different attributes",
_temp773.last_plus_one - _temp773.curr, _temp773.curr);}));}{ struct Cyc_List_List*
f1s=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp829= ud->fields; if(
_temp829 == 0){ _throw( Null_Exception);} _temp829->v;}); struct Cyc_List_List*
f2s=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp828= ud2->fields; if(
_temp828 == 0){ _throw( Null_Exception);} _temp828->v;}); for( 0; f1s != 0? f2s
!= 0: 0; f1s=({ struct Cyc_List_List* _temp774= f1s; if( _temp774 == 0){ _throw(
Null_Exception);} _temp774->tl;}), f2s=({ struct Cyc_List_List* _temp775= f2s;
if( _temp775 == 0){ _throw( Null_Exception);} _temp775->tl;})){ struct Cyc_Absyn_Structfield
_temp779; struct Cyc_List_List* _temp780; struct Cyc_Core_Opt* _temp782; void*
_temp784; struct Cyc_Absyn_Tqual _temp786; struct _tagged_string* _temp788;
struct Cyc_Absyn_Structfield* _temp777=( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp776= f1s; if( _temp776 == 0){ _throw( Null_Exception);}
_temp776->hd;}); _temp779=* _temp777; _LL789: _temp788=( struct _tagged_string*)
_temp779.name; goto _LL787; _LL787: _temp786=( struct Cyc_Absyn_Tqual) _temp779.tq;
goto _LL785; _LL785: _temp784=( void*) _temp779.type; goto _LL783; _LL783:
_temp782=( struct Cyc_Core_Opt*) _temp779.width; goto _LL781; _LL781: _temp780=(
struct Cyc_List_List*) _temp779.attributes; goto _LL778; _LL778: { struct Cyc_Absyn_Structfield
_temp793; struct Cyc_List_List* _temp794; struct Cyc_Core_Opt* _temp796; void*
_temp798; struct Cyc_Absyn_Tqual _temp800; struct _tagged_string* _temp802;
struct Cyc_Absyn_Structfield* _temp791=( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp790= f2s; if( _temp790 == 0){ _throw( Null_Exception);}
_temp790->hd;}); _temp793=* _temp791; _LL803: _temp802=( struct _tagged_string*)
_temp793.name; goto _LL801; _LL801: _temp800=( struct Cyc_Absyn_Tqual) _temp793.tq;
goto _LL799; _LL799: _temp798=( void*) _temp793.type; goto _LL797; _LL797:
_temp796=( struct Cyc_Core_Opt*) _temp793.width; goto _LL795; _LL795: _temp794=(
struct Cyc_List_List*) _temp793.attributes; goto _LL792; _LL792: if( Cyc_String_zstrptrcmp(
_temp788, _temp802) != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp804=* v; struct _tagged_string _temp805=* _temp788; struct _tagged_string
_temp806=* _temp802; xprintf("redeclaration of union %.*s: field name mismatch %.*s != %.*s",
_temp804.last_plus_one - _temp804.curr, _temp804.curr, _temp805.last_plus_one -
_temp805.curr, _temp805.curr, _temp806.last_plus_one - _temp806.curr, _temp806.curr);}));}
if( ! Cyc_Tcutil_same_atts( _temp780, _temp794)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp807=* v; struct _tagged_string _temp808=* _temp788; xprintf("redeclaration of union %.*s: attribute mismatch on field %.*s",
_temp807.last_plus_one - _temp807.curr, _temp807.curr, _temp808.last_plus_one -
_temp808.curr, _temp808.curr);}));} if( ! Cyc_Tcutil_equal_tqual( _temp786,
_temp800)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp809=* v; struct
_tagged_string _temp810=* _temp788; xprintf("redeclaration of union %.*s: qualifier mismatch on field %.*s",
_temp809.last_plus_one - _temp809.curr, _temp809.curr, _temp810.last_plus_one -
_temp810.curr, _temp810.curr);}));} if( _temp782 != 0? ! Cyc_Tcutil_is_const_exp(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp811= _temp782; if(
_temp811 == 0){ _throw( Null_Exception);} _temp811->v;})): 0){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp812=* v; struct _tagged_string _temp813=*
_temp788; xprintf("redeclaration of struct %.*s: bad bitfield %.*s", _temp812.last_plus_one
- _temp812.curr, _temp812.curr, _temp813.last_plus_one - _temp813.curr, _temp813.curr);}));}
else{ if((( _temp782 != 0? _temp796 != 0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp814= _temp782; if( _temp814
== 0){ _throw( Null_Exception);} _temp814->v;})) != Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp815= _temp796; if( _temp815
== 0){ _throw( Null_Exception);} _temp815->v;})): 0)? 1: _temp782 != _temp796){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp816=* v; struct
_tagged_string _temp817=* _temp788; xprintf("redeclaration of struct %.*s: bitfield mismatch on field %.*s",
_temp816.last_plus_one - _temp816.curr, _temp816.curr, _temp817.last_plus_one -
_temp817.curr, _temp817.curr);}));}} Cyc_Tcutil_check_type( loc, te, tvs,( void*)
Cyc_Absyn_MemKind, _temp784);{ void* subst_t2= Cyc_Tcutil_substitute( inst,
_temp798); if( ! Cyc_Tcutil_unify( _temp784, subst_t2)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp818=* v; struct _tagged_string _temp819=* _temp788;
struct _tagged_string _temp820= Cyc_Absynpp_typ2string( _temp784); struct
_tagged_string _temp821= Cyc_Absynpp_typ2string( subst_t2); xprintf("redeclaration of union %.*s: type mismatch on field %.*s: %.*s != %.*s",
_temp818.last_plus_one - _temp818.curr, _temp818.curr, _temp819.last_plus_one -
_temp819.curr, _temp819.curr, _temp820.last_plus_one - _temp820.curr, _temp820.curr,
_temp821.last_plus_one - _temp821.curr, _temp821.curr);}));}}}} if( f2s != 0){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp823=* v; struct
_tagged_string _temp824=*(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp822= f2s; if( _temp822 == 0){ _throw( Null_Exception);} _temp822->hd;}))->name;
xprintf("redeclaration of union %.*s is missing field %.*s", _temp823.last_plus_one
- _temp823.curr, _temp823.curr, _temp824.last_plus_one - _temp824.curr, _temp824.curr);}));}
if( f1s != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp826=* v;
struct _tagged_string _temp827=*(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp825= f1s; if( _temp825 == 0){ _throw( Null_Exception);} _temp825->hd;}))->name;
xprintf("redeclaration of union %.*s has extra field %.*s", _temp826.last_plus_one
- _temp826.curr, _temp826.curr, _temp827.last_plus_one - _temp827.curr, _temp827.curr);}));}
return;}} _LL740:;} goto _LL715;}} _LL715:;}{ struct Cyc_Absyn_Uniondecl** udp=((
struct Cyc_Absyn_Uniondecl**(*)( struct Cyc_Dict_Dict* d, struct _tagged_string*
key)) Cyc_Dict_lookup)( ge->uniondecls, v);* udp=({ struct Cyc_Absyn_Uniondecl*
_temp830=( struct Cyc_Absyn_Uniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl));
_temp830->sc=( void*)(( void*) ud->sc); _temp830->name= ud->name; _temp830->tvs=
tvs; _temp830->fields= 0; _temp830->attributes= 0; _temp830;});{ struct Cyc_List_List*
prev_fields= 0; struct Cyc_List_List* prev_types= 0;{ struct Cyc_List_List* fs=(
struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp899= ud->fields; if( _temp899
== 0){ _throw( Null_Exception);} _temp899->v;}); for( 0; fs != 0; fs=({ struct
Cyc_List_List* _temp831= fs; if( _temp831 == 0){ _throw( Null_Exception);}
_temp831->tl;})){ struct Cyc_Absyn_Structfield _temp835; struct Cyc_List_List*
_temp836; struct Cyc_Core_Opt* _temp838; void* _temp840; struct Cyc_Absyn_Tqual
_temp842; struct _tagged_string* _temp844; struct Cyc_Absyn_Structfield*
_temp833=( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp832= fs;
if( _temp832 == 0){ _throw( Null_Exception);} _temp832->hd;}); _temp835=*
_temp833; _LL845: _temp844=( struct _tagged_string*) _temp835.name; goto _LL843;
_LL843: _temp842=( struct Cyc_Absyn_Tqual) _temp835.tq; goto _LL841; _LL841:
_temp840=( void*) _temp835.type; goto _LL839; _LL839: _temp838=( struct Cyc_Core_Opt*)
_temp835.width; goto _LL837; _LL837: _temp836=( struct Cyc_List_List*) _temp835.attributes;
goto _LL834; _LL834: if((( int(*)( int(* compare)( struct _tagged_string*,
struct _tagged_string*), struct Cyc_List_List* l, struct _tagged_string* x)) Cyc_List_mem)(
Cyc_String_zstrptrcmp, prev_fields, _temp844)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp846=* _temp844; xprintf("duplicate field %.*s in union",
_temp846.last_plus_one - _temp846.curr, _temp846.curr);}));} prev_fields=({
struct Cyc_List_List* _temp847=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp847->hd=( void*) _temp844; _temp847->tl=
prev_fields; _temp847;}); Cyc_Tcutil_check_type( loc, te, tvs,( void*) Cyc_Absyn_MemKind,
_temp840); if( _temp838 != 0){ unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp848= _temp838; if(
_temp848 == 0){ _throw( Null_Exception);} _temp848->v;}))){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp849=* _temp844; xprintf("bitfield %.*s does not have constant width",
_temp849.last_plus_one - _temp849.curr, _temp849.curr);}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp850= _temp838; if( _temp850
== 0){ _throw( Null_Exception);} _temp850->v;}));}{ void* _temp851= Cyc_Tcutil_compress(
_temp840); void* _temp857; void* _temp859; _LL853: if(( unsigned int) _temp851 >
4u?(( struct _tunion_struct*) _temp851)->tag == Cyc_Absyn_IntType_tag: 0){
_LL860: _temp859=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp851)->f1;
goto _LL858; _LL858: _temp857=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp851)->f2; goto _LL854;} else{ goto _LL855;} _LL855: goto _LL856; _LL854:{
void* _temp861= _temp857; _LL863: if(( int) _temp861 == Cyc_Absyn_B1){ goto
_LL864;} else{ goto _LL865;} _LL865: if(( int) _temp861 == Cyc_Absyn_B2){ goto
_LL866;} else{ goto _LL867;} _LL867: if(( int) _temp861 == Cyc_Absyn_B4){ goto
_LL868;} else{ goto _LL869;} _LL869: if(( int) _temp861 == Cyc_Absyn_B8){ goto
_LL870;} else{ goto _LL862;} _LL864: if( w > 8){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp871=( char*)"bitfield larger than type"; struct
_tagged_string _temp872; _temp872.curr= _temp871; _temp872.base= _temp871;
_temp872.last_plus_one= _temp871 + 26; _temp872;}));} goto _LL862; _LL866: if( w
> 16){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp873=( char*)"bitfield larger than type";
struct _tagged_string _temp874; _temp874.curr= _temp873; _temp874.base= _temp873;
_temp874.last_plus_one= _temp873 + 26; _temp874;}));} goto _LL862; _LL868: if( w
> 32){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp875=( char*)"bitfield larger than type";
struct _tagged_string _temp876; _temp876.curr= _temp875; _temp876.base= _temp875;
_temp876.last_plus_one= _temp875 + 26; _temp876;}));} goto _LL862; _LL870: if( w
> 64){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp877=( char*)"bitfield larger than type";
struct _tagged_string _temp878; _temp878.curr= _temp877; _temp878.base= _temp877;
_temp878.last_plus_one= _temp877 + 26; _temp878;}));} goto _LL862; _LL862:;}
goto _LL852; _LL856: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp879=*
_temp844; struct _tagged_string _temp880= Cyc_Absynpp_typ2string( _temp840);
xprintf("bitfield %.*s must have integral type but has type %.*s", _temp879.last_plus_one
- _temp879.curr, _temp879.curr, _temp880.last_plus_one - _temp880.curr, _temp880.curr);}));
goto _LL852; _LL852:;}} if( ! Cyc_Tcutil_bits_only( _temp840)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp881=* _temp844; struct _tagged_string _temp882=*
v; xprintf("%.*s has a type that is possibly incompatible with other members of union %.*s",
_temp881.last_plus_one - _temp881.curr, _temp881.curr, _temp882.last_plus_one -
_temp882.curr, _temp882.curr);}));} for( 0; _temp836 != 0; _temp836=({ struct
Cyc_List_List* _temp883= _temp836; if( _temp883 == 0){ _throw( Null_Exception);}
_temp883->tl;})){ void* _temp885=( void*)({ struct Cyc_List_List* _temp884=
_temp836; if( _temp884 == 0){ _throw( Null_Exception);} _temp884->hd;}); int
_temp893; _LL887: if(( int) _temp885 == Cyc_Absyn_Packed_att){ goto _LL888;}
else{ goto _LL889;} _LL889: if(( unsigned int) _temp885 > 15u?(( struct
_tunion_struct*) _temp885)->tag == Cyc_Absyn_Aligned_att_tag: 0){ _LL894:
_temp893=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp885)->f1; goto
_LL890;} else{ goto _LL891;} _LL891: goto _LL892; _LL888: continue; _LL890:
continue; _LL892: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp896= Cyc_Absyn_attribute2string((
void*)({ struct Cyc_List_List* _temp895= _temp836; if( _temp895 == 0){ _throw(
Null_Exception);} _temp895->hd;})); struct _tagged_string _temp897=* v; struct
_tagged_string _temp898=* _temp844; xprintf("bad attribute %.*s in union %.*s, member %.*s",
_temp896.last_plus_one - _temp896.curr, _temp896.curr, _temp897.last_plus_one -
_temp897.curr, _temp897.curr, _temp898.last_plus_one - _temp898.curr, _temp898.curr);}));
goto _LL886; _LL886:;}}}* udp= ud;}}}}} static void Cyc_Tc_tcTuniondecl( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc,
struct Cyc_Absyn_Tuniondecl* ed){ if( ed->name == 0){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp901=( char*)"anonymous tunions are not allowed";
struct _tagged_string _temp902; _temp902.curr= _temp901; _temp902.base= _temp901;
_temp902.last_plus_one= _temp901 + 34; _temp902;})); return;}{ struct
_tagged_string* v=(*(( struct _tuple0*)({ struct Cyc_Core_Opt* _temp1049= ed->name;
if( _temp1049 == 0){ _throw( Null_Exception);} _temp1049->v;}))).f2; struct Cyc_List_List*
tvs= ed->tvs;{ struct Cyc_List_List* tvs2= tvs; for( 0; tvs2 != 0; tvs2=({
struct Cyc_List_List* _temp903= tvs2; if( _temp903 == 0){ _throw( Null_Exception);}
_temp903->tl;})){ struct Cyc_Absyn_Conref* c=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp919= tvs2; if( _temp919 == 0){ _throw( Null_Exception);}
_temp919->hd;}))->kind); void* _temp904=( void*) c->v; void* _temp912; _LL906:
if(( int) _temp904 == Cyc_Absyn_No_constr){ goto _LL907;} else{ goto _LL908;}
_LL908: if(( unsigned int) _temp904 > 1u?(( struct _tunion_struct*) _temp904)->tag
== Cyc_Absyn_Eq_constr_tag: 0){ _LL913: _temp912=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp904)->f1; if(( int) _temp912 == Cyc_Absyn_MemKind){ goto _LL909;} else{
goto _LL910;}} else{ goto _LL910;} _LL910: goto _LL911; _LL907:( void*)( c->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp914=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp914[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp915; _temp915.tag= Cyc_Absyn_Eq_constr_tag;
_temp915.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp915;}); _temp914;})));
goto _LL905; _LL909: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp917=* v;
struct _tagged_string _temp918=*(( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp916= tvs2; if( _temp916 == 0){ _throw( Null_Exception);} _temp916->hd;}))->name;
xprintf("tunion %.*s attempts to abstract type variable %.*s of kind M",
_temp917.last_plus_one - _temp917.curr, _temp917.curr, _temp918.last_plus_one -
_temp918.curr, _temp918.curr);})); goto _LL905; _LL911: goto _LL905; _LL905:;}}{
void* _temp921=(*(( struct _tuple0*)({ struct Cyc_Core_Opt* _temp920= ed->name;
if( _temp920 == 0){ _throw( Null_Exception);} _temp920->v;}))).f1; struct Cyc_List_List*
_temp929; struct Cyc_List_List* _temp931; _LL923: if(( unsigned int) _temp921 >
1u?(( struct _tunion_struct*) _temp921)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL930:
_temp929=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp921)->f1;
if( _temp929 == 0){ goto _LL924;} else{ goto _LL925;}} else{ goto _LL925;}
_LL925: if(( unsigned int) _temp921 > 1u?(( struct _tunion_struct*) _temp921)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL932: _temp931=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp921)->f1; if( _temp931 == 0){ goto _LL926;} else{
goto _LL927;}} else{ goto _LL927;} _LL927: goto _LL928; _LL924: goto _LL922;
_LL926: goto _LL922; _LL928: Cyc_Tcutil_terr( loc,( struct _tagged_string)({
char* _temp933=( char*)"qualified declarations are not implemented"; struct
_tagged_string _temp934; _temp934.curr= _temp933; _temp934.base= _temp933;
_temp934.last_plus_one= _temp933 + 43; _temp934;})); return; _LL922:;}(*((
struct _tuple0*)({ struct Cyc_Core_Opt* _temp935= ed->name; if( _temp935 == 0){
_throw( Null_Exception);} _temp935->v;}))).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp936=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp936[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp937; _temp937.tag= Cyc_Absyn_Abs_n_tag;
_temp937.f1= te->ns; _temp937;}); _temp936;}); Cyc_Tcutil_check_unique_tvars(
loc, tvs);{ struct Cyc_Core_Opt* _temp938=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup_opt)( ge->tuniondecls, v);
struct Cyc_Core_Opt _temp944; struct Cyc_Absyn_Tuniondecl** _temp945; _LL940:
if( _temp938 == 0){ goto _LL941;} else{ goto _LL942;} _LL942: if( _temp938 == 0){
goto _LL939;} else{ _temp944=* _temp938; _LL946: _temp945=( struct Cyc_Absyn_Tuniondecl**)
_temp944.v; goto _LL943;} _LL941: ge->tuniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Tuniondecl**
data)) Cyc_Dict_insert)( ge->tuniondecls, v,({ struct Cyc_Absyn_Tuniondecl**
_temp947=( struct Cyc_Absyn_Tuniondecl**) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl*));
_temp947[ 0]= ed; _temp947;})); if( ed->fields == 0){ return;} goto _LL939;
_LL943: { struct Cyc_Absyn_Tuniondecl* ed2=* _temp945; struct Cyc_List_List*
tvs2= ed2->tvs; if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs)
!=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs2)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp948=* v; xprintf("redeclaration of tunion %.*s has a different number of type parameters",
_temp948.last_plus_one - _temp948.curr, _temp948.curr);}));}{ struct Cyc_List_List*
x1= tvs; struct Cyc_List_List* x2= tvs2; for( 0; x1 != 0; x1=({ struct Cyc_List_List*
_temp949= x1; if( _temp949 == 0){ _throw( Null_Exception);} _temp949->tl;}), x2=({
struct Cyc_List_List* _temp950= x2; if( _temp950 == 0){ _throw( Null_Exception);}
_temp950->tl;})){ struct Cyc_Absyn_Conref* c1=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp959= x1; if( _temp959 == 0){ _throw( Null_Exception);}
_temp959->hd;}))->kind); struct Cyc_Absyn_Conref* c2=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp958= x2; if( _temp958 == 0){ _throw( Null_Exception);}
_temp958->hd;}))->kind); if(( void*) c1->v ==( void*) Cyc_Absyn_No_constr? c1 !=
c2: 0){( void*)( c1->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp951=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp951[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp952; _temp952.tag= Cyc_Absyn_Forward_constr_tag; _temp952.f1= c2; _temp952;});
_temp951;})));} if((( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
c1) !=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( c2)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp954=* v; struct _tagged_string _temp955= Cyc_Absynpp_ckind2string(
c1); struct _tagged_string _temp956=*(( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp953= x1; if( _temp953 == 0){ _throw( Null_Exception);} _temp953->hd;}))->name;
struct _tagged_string _temp957= Cyc_Absynpp_ckind2string( c2); xprintf("redeclaration of tunion %.*s has a different kind (%.*s) for type parameter %.*s (%.*s)",
_temp954.last_plus_one - _temp954.curr, _temp954.curr, _temp955.last_plus_one -
_temp955.curr, _temp955.curr, _temp956.last_plus_one - _temp956.curr, _temp956.curr,
_temp957.last_plus_one - _temp957.curr, _temp957.curr);}));}}( void*)( ed->sc=(
void*)(( void*(*)( void* s1, struct Cyc_Core_Opt* fields1, void* s2, struct Cyc_Core_Opt*
fields2, struct Cyc_Position_Segment* loc, struct _tagged_string t, struct
_tagged_string* v)) Cyc_Tc_scope_redecl_okay)(( void*) ed->sc, ed->fields,( void*)
ed2->sc, ed2->fields, loc,( struct _tagged_string)({ char* _temp960=( char*)"tunion";
struct _tagged_string _temp961; _temp961.curr= _temp960; _temp961.base= _temp960;
_temp961.last_plus_one= _temp960 + 7; _temp961;}), v));{ struct _tuple7 _temp963=({
struct _tuple7 _temp962; _temp962.f1= ed->fields; _temp962.f2= ed2->fields;
_temp962;}); struct Cyc_Core_Opt* _temp973; struct Cyc_Core_Opt* _temp975;
struct Cyc_Core_Opt* _temp977; struct Cyc_Core_Opt* _temp979; struct Cyc_Core_Opt*
_temp981; struct Cyc_Core_Opt* _temp983; struct Cyc_Core_Opt* _temp985; struct
Cyc_Core_Opt* _temp987; _LL965: _LL976: _temp975= _temp963.f1; if( _temp975 == 0){
goto _LL974;} else{ goto _LL967;} _LL974: _temp973= _temp963.f2; if( _temp973 ==
0){ goto _LL966;} else{ goto _LL967;} _LL967: _LL980: _temp979= _temp963.f1;
goto _LL978; _LL978: _temp977= _temp963.f2; if( _temp977 == 0){ goto _LL968;}
else{ goto _LL969;} _LL969: _LL984: _temp983= _temp963.f1; if( _temp983 == 0){
goto _LL982;} else{ goto _LL971;} _LL982: _temp981= _temp963.f2; goto _LL970;
_LL971: _LL988: _temp987= _temp963.f1; goto _LL986; _LL986: _temp985= _temp963.f2;
goto _LL972; _LL966: return; _LL968:* _temp945= ed; goto _LL964; _LL970: ed->fields=
ed2->fields; ed->tvs= tvs2; return; _LL972: { struct Cyc_List_List* inst= 0;{
struct Cyc_List_List* tvs0= tvs; for( 0; tvs0 != 0; tvs0=({ struct Cyc_List_List*
_temp989= tvs0; if( _temp989 == 0){ _throw( Null_Exception);} _temp989->tl;}),
tvs2=({ struct Cyc_List_List* _temp990= tvs2; if( _temp990 == 0){ _throw(
Null_Exception);} _temp990->tl;})){ inst=({ struct Cyc_List_List* _temp991=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp991->hd=(
void*)({ struct _tuple4* _temp992=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp992->f1=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp996= tvs2; if( _temp996 == 0){ _throw( Null_Exception);} _temp996->hd;});
_temp992->f2=( void*)({ struct Cyc_Absyn_VarType_struct* _temp993=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp993[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp994; _temp994.tag= Cyc_Absyn_VarType_tag; _temp994.f1=( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp995= tvs0; if( _temp995 == 0){ _throw( Null_Exception);}
_temp995->hd;}); _temp994;}); _temp993;}); _temp992;}); _temp991->tl= inst;
_temp991;});}}{ struct Cyc_List_List* f1s=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp1019= ed->fields; if( _temp1019 == 0){ _throw( Null_Exception);} _temp1019->v;});
struct Cyc_List_List* f2s=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp1018= ed2->fields; if( _temp1018 == 0){ _throw( Null_Exception);} _temp1018->v;});
for( 0; f1s != 0? f2s != 0: 0; f1s=({ struct Cyc_List_List* _temp997= f1s; if(
_temp997 == 0){ _throw( Null_Exception);} _temp997->tl;}), f2s=({ struct Cyc_List_List*
_temp998= f2s; if( _temp998 == 0){ _throw( Null_Exception);} _temp998->tl;})){(*((
struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp999= f1s; if(
_temp999 == 0){ _throw( Null_Exception);} _temp999->hd;}))->name).f1=( void*)({
struct Cyc_Absyn_Abs_n_struct* _temp1000=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp1000[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp1001; _temp1001.tag= Cyc_Absyn_Abs_n_tag; _temp1001.f1= te->ns; _temp1001;});
_temp1000;});{ struct Cyc_List_List* alltvs=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tvs,(( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp1011= f1s; if( _temp1011 == 0){ _throw(
Null_Exception);} _temp1011->hd;}))->tvs); Cyc_Tcutil_check_unique_tvars(((
struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp1002= f1s; if(
_temp1002 == 0){ _throw( Null_Exception);} _temp1002->hd;}))->loc, alltvs);{
struct Cyc_List_List* typs=(( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List*
_temp1006= f1s; if( _temp1006 == 0){ _throw( Null_Exception);} _temp1006->hd;}))->typs;
for( 0; typs != 0; typs=({ struct Cyc_List_List* _temp1003= typs; if( _temp1003
== 0){ _throw( Null_Exception);} _temp1003->tl;})){ void* t=(*(( struct _tuple5*)({
struct Cyc_List_List* _temp1005= typs; if( _temp1005 == 0){ _throw(
Null_Exception);} _temp1005->hd;}))).f2; Cyc_Tcutil_check_type((( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp1004= f1s; if( _temp1004 == 0){ _throw(
Null_Exception);} _temp1004->hd;}))->loc, te, alltvs,( void*) Cyc_Absyn_MemKind,
t);}} Cyc_Tc_field_redecl_okay(( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List*
_temp1007= f1s; if( _temp1007 == 0){ _throw( Null_Exception);} _temp1007->hd;}),(
struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp1008= f2s; if(
_temp1008 == 0){ _throw( Null_Exception);} _temp1008->hd;}), inst, te,( struct
_tagged_string)({ char* _temp1009=( char*)"tunion"; struct _tagged_string
_temp1010; _temp1010.curr= _temp1009; _temp1010.base= _temp1009; _temp1010.last_plus_one=
_temp1009 + 7; _temp1010;}), v);}} if( f1s != 0){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp1013=* v; struct _tagged_string _temp1014=*(*(( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp1012= f1s; if( _temp1012 == 0){ _throw(
Null_Exception);} _temp1012->hd;}))->name).f2; xprintf("redeclaration of tunion %.*s has extra field %.*s",
_temp1013.last_plus_one - _temp1013.curr, _temp1013.curr, _temp1014.last_plus_one
- _temp1014.curr, _temp1014.curr);}));} if( f2s != 0){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp1016=* v; struct _tagged_string _temp1017=*(*((
struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp1015= f2s; if(
_temp1015 == 0){ _throw( Null_Exception);} _temp1015->hd;}))->name).f2; xprintf("redeclaration of tunion %.*s is missing field %.*s",
_temp1016.last_plus_one - _temp1016.curr, _temp1016.curr, _temp1017.last_plus_one
- _temp1017.curr, _temp1017.curr);}));} return;}} _LL964:;} goto _LL939;}}
_LL939:;}{ struct Cyc_Absyn_Tuniondecl** edp=(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->tuniondecls,
v);* edp=({ struct Cyc_Absyn_Tuniondecl* _temp1020=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp1020->sc=( void*)(( void*)
ed->sc); _temp1020->name= ed->name; _temp1020->tvs= tvs; _temp1020->fields= 0;
_temp1020;});{ struct Cyc_List_List* prev_fields= 0;{ struct Cyc_List_List* fs=(
struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp1047= ed->fields; if(
_temp1047 == 0){ _throw( Null_Exception);} _temp1047->v;}); for( 0; fs != 0; fs=({
struct Cyc_List_List* _temp1021= fs; if( _temp1021 == 0){ _throw( Null_Exception);}
_temp1021->tl;})){ struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp1046= fs; if( _temp1046 == 0){ _throw( Null_Exception);}
_temp1046->hd;}); if((( int(*)( int(* compare)( struct _tagged_string*, struct
_tagged_string*), struct Cyc_List_List* l, struct _tagged_string* x)) Cyc_List_mem)(
Cyc_String_zstrptrcmp, prev_fields,(* f->name).f2)){ Cyc_Tcutil_terr( f->loc,({
struct _tagged_string _temp1022=*(* f->name).f2; xprintf("duplicate field name %.*s",
_temp1022.last_plus_one - _temp1022.curr, _temp1022.curr);}));} prev_fields=({
struct Cyc_List_List* _temp1023=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1023->hd=( void*)(* f->name).f2; _temp1023->tl=
prev_fields; _temp1023;});{ struct Cyc_List_List* tvs= f->tvs; for( 0; tvs != 0;
tvs=({ struct Cyc_List_List* _temp1024= tvs; if( _temp1024 == 0){ _throw(
Null_Exception);} _temp1024->tl;})){ struct Cyc_Absyn_Tvar* tv=( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp1038= tvs; if( _temp1038 == 0){ _throw(
Null_Exception);} _temp1038->hd;}); struct Cyc_Absyn_Conref* c=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( tv->kind); void*
_temp1025=( void*) c->v; void* _temp1033; _LL1027: if(( int) _temp1025 == Cyc_Absyn_No_constr){
goto _LL1028;} else{ goto _LL1029;} _LL1029: if(( unsigned int) _temp1025 > 1u?((
struct _tunion_struct*) _temp1025)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL1034:
_temp1033=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1025)->f1; if((
int) _temp1033 == Cyc_Absyn_MemKind){ goto _LL1030;} else{ goto _LL1031;}} else{
goto _LL1031;} _LL1031: goto _LL1032; _LL1028:( void*)( c->v=( void*)(( void*)({
struct Cyc_Absyn_Eq_constr_struct* _temp1035=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp1035[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp1036; _temp1036.tag= Cyc_Absyn_Eq_constr_tag;
_temp1036.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp1036;}); _temp1035;})));
goto _LL1026; _LL1030: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp1037=*(*
f->name).f2; xprintf("field %.*s abstracts type variable of kind M", _temp1037.last_plus_one
- _temp1037.curr, _temp1037.curr);})); goto _LL1026; _LL1032: goto _LL1026;
_LL1026:;}}{ struct Cyc_List_List* alltvs=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tvs, f->tvs); Cyc_Tcutil_check_unique_tvars(
loc, alltvs);{ struct Cyc_List_List* typs= f->typs; for( 0; typs != 0; typs=({
struct Cyc_List_List* _temp1039= typs; if( _temp1039 == 0){ _throw(
Null_Exception);} _temp1039->tl;})){ void* t=(*(( struct _tuple5*)({ struct Cyc_List_List*
_temp1040= typs; if( _temp1040 == 0){ _throw( Null_Exception);} _temp1040->hd;}))).f2;
Cyc_Tcutil_check_type( f->loc, te, alltvs,( void*) Cyc_Absyn_MemKind, t);}}(* f->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp1041=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp1041[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp1042; _temp1042.tag= Cyc_Absyn_Abs_n_tag; _temp1042.f1= te->ns; _temp1042;});
_temp1041;}); ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct _tuple6* data)) Cyc_Dict_insert)( ge->ordinaries,(*
f->name).f2,({ struct _tuple6* _temp1043=( struct _tuple6*) GC_malloc( sizeof(
struct _tuple6)); _temp1043->f1=( void*)({ struct Cyc_Tcenv_TunionRes_struct*
_temp1044=( struct Cyc_Tcenv_TunionRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_TunionRes_struct));
_temp1044[ 0]=({ struct Cyc_Tcenv_TunionRes_struct _temp1045; _temp1045.tag= Cyc_Tcenv_TunionRes_tag;
_temp1045.f1= ed; _temp1045.f2= f; _temp1045;}); _temp1044;}); _temp1043->f2= 1;
_temp1043;}));}}} ge->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Tuniondecl** data)) Cyc_Dict_insert)(
ge->tuniondecls, v,({ struct Cyc_Absyn_Tuniondecl** _temp1048=( struct Cyc_Absyn_Tuniondecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl*)); _temp1048[ 0]= ed; _temp1048;}));*
edp= ed;}}}} struct _tuple8{ struct Cyc_Absyn_XTuniondecl* f1; } ; static void
Cyc_Tc_tcXTuniondecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge,
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_XTuniondecl* xd){ struct
_tagged_string* v=(* xd->name).f2; struct Cyc_Core_Opt* xd2o;{ struct
_handler_cons _temp1050; _push_handler(& _temp1050);{ struct _xtunion_struct*
_temp1051=( struct _xtunion_struct*) setjmp( _temp1050.handler); if( ! _temp1051){
xd2o= Cyc_Tcenv_lookup_xtuniondecl( te, loc, xd->name);; _pop_handler();} else{
struct _xtunion_struct* _temp1053= _temp1051; _LL1055: if( _temp1053->tag == Cyc_Dict_Absent_tag){
goto _LL1056;} else{ goto _LL1057;} _LL1057: goto _LL1058; _LL1056: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp1059= Cyc_Absynpp_qvar2string( xd->name);
xprintf("qualified xtunion declaration %.*s is not an existing xtunion",
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
struct Cyc_Absyn_XTuniondecl**)({ struct _tuple8* _temp1091=( struct _tuple8*)
GC_malloc( sizeof( struct _tuple8)); _temp1091->f1=({ struct Cyc_Absyn_XTuniondecl*
_temp1092=( struct Cyc_Absyn_XTuniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_XTuniondecl));
_temp1092->sc=( void*)(( void*) xd->sc); _temp1092->name= xd->name; _temp1092->fields=
0; _temp1092;}); _temp1091;}); xd2o=({ struct Cyc_Core_Opt* _temp1090=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1090->v=( void*) x;
_temp1090;}); ge->xtuniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_XTuniondecl** data)) Cyc_Dict_insert)(
ge->xtuniondecls, v, x);} else{ struct Cyc_Absyn_XTuniondecl* x=*(( struct Cyc_Absyn_XTuniondecl**)({
struct Cyc_Core_Opt* _temp1097= xd2o; if( _temp1097 == 0){ _throw(
Null_Exception);} _temp1097->v;}));( void*)( x->sc=( void*)(( void*(*)( void* s1,
struct Cyc_Core_Opt* fields1, void* s2, struct Cyc_Core_Opt* fields2, struct Cyc_Position_Segment*
loc, struct _tagged_string t, struct _tagged_string* v)) Cyc_Tc_scope_redecl_okay)((
void*) xd->sc,({ struct Cyc_Core_Opt* _temp1093=( struct Cyc_Core_Opt*)
GC_malloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp1093->v=( void*) 0;
_temp1093;}),( void*) x->sc,({ struct Cyc_Core_Opt* _temp1094=( struct Cyc_Core_Opt*)
GC_malloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp1094->v=( void*) 0;
_temp1094;}), loc,( struct _tagged_string)({ char* _temp1095=( char*)"xtunion";
struct _tagged_string _temp1096; _temp1096.curr= _temp1095; _temp1096.base=
_temp1095; _temp1096.last_plus_one= _temp1095 + 8; _temp1096;}), v));}{ struct
Cyc_Absyn_XTuniondecl* xd_result=*(( struct Cyc_Absyn_XTuniondecl**)({ struct
Cyc_Core_Opt* _temp1175= xd2o; if( _temp1175 == 0){ _throw( Null_Exception);}
_temp1175->v;})); struct Cyc_List_List* fs= xd->fields; for( 0; fs != 0; fs=({
struct Cyc_List_List* _temp1098= fs; if( _temp1098 == 0){ _throw( Null_Exception);}
_temp1098->tl;})){ struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp1174= fs; if( _temp1174 == 0){ _throw( Null_Exception);}
_temp1174->hd;});{ void* _temp1099=(* f->name).f1; struct Cyc_List_List*
_temp1109; struct Cyc_List_List* _temp1111; struct Cyc_List_List* _temp1113;
_LL1101: if(( unsigned int) _temp1099 > 1u?(( struct _tunion_struct*) _temp1099)->tag
== Cyc_Absyn_Rel_n_tag: 0){ _LL1110: _temp1109=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Rel_n_struct*) _temp1099)->f1; if( _temp1109 == 0){ goto _LL1102;}
else{ goto _LL1103;}} else{ goto _LL1103;} _LL1103: if(( unsigned int) _temp1099
> 1u?(( struct _tunion_struct*) _temp1099)->tag == Cyc_Absyn_Rel_n_tag: 0){
_LL1112: _temp1111=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp1099)->f1; goto _LL1104;} else{ goto _LL1105;} _LL1105: if(( unsigned int)
_temp1099 > 1u?(( struct _tunion_struct*) _temp1099)->tag == Cyc_Absyn_Abs_n_tag:
0){ _LL1114: _temp1113=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp1099)->f1; goto _LL1106;} else{ goto _LL1107;} _LL1107: if(( int) _temp1099
== Cyc_Absyn_Loc_n){ goto _LL1108;} else{ goto _LL1100;} _LL1102:(* f->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp1115=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp1115[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp1116; _temp1116.tag= Cyc_Absyn_Abs_n_tag; _temp1116.f1= te->ns; _temp1116;});
_temp1115;}); goto _LL1100; _LL1104: goto _LL1106; _LL1106: Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp1117=( char*)"qualified declarations are not allowed\n";
struct _tagged_string _temp1118; _temp1118.curr= _temp1117; _temp1118.base=
_temp1117; _temp1118.last_plus_one= _temp1117 + 40; _temp1118;})); goto _LL1100;
_LL1108:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp1119=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1119[ 0]=({ struct Cyc_Core_Impossible_struct _temp1120; _temp1120.tag= Cyc_Core_Impossible_tag;
_temp1120.f1=( struct _tagged_string)({ char* _temp1121=( char*)"tcXtuniondecl: Loc_n";
struct _tagged_string _temp1122; _temp1122.curr= _temp1121; _temp1122.base=
_temp1121; _temp1122.last_plus_one= _temp1121 + 21; _temp1122;}); _temp1120;});
_temp1119;})); goto _LL1100; _LL1100:;}{ struct Cyc_List_List* tvs= f->tvs; for(
0; tvs != 0; tvs=({ struct Cyc_List_List* _temp1123= tvs; if( _temp1123 == 0){
_throw( Null_Exception);} _temp1123->tl;})){ struct Cyc_Absyn_Tvar* tv=( struct
Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp1137= tvs; if( _temp1137 == 0){
_throw( Null_Exception);} _temp1137->hd;}); struct Cyc_Absyn_Conref* c=(( struct
Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
tv->kind); void* _temp1124=( void*) c->v; void* _temp1132; _LL1126: if(( int)
_temp1124 == Cyc_Absyn_No_constr){ goto _LL1127;} else{ goto _LL1128;} _LL1128:
if(( unsigned int) _temp1124 > 1u?(( struct _tunion_struct*) _temp1124)->tag ==
Cyc_Absyn_Eq_constr_tag: 0){ _LL1133: _temp1132=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp1124)->f1; if(( int) _temp1132 == Cyc_Absyn_MemKind){ goto _LL1129;} else{
goto _LL1130;}} else{ goto _LL1130;} _LL1130: goto _LL1131; _LL1127:( void*)( c->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp1134=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp1134[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp1135; _temp1135.tag= Cyc_Absyn_Eq_constr_tag;
_temp1135.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp1135;}); _temp1134;})));
goto _LL1125; _LL1129: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp1136=*(*
f->name).f2; xprintf("field %.*s abstracts type variable of kind M", _temp1136.last_plus_one
- _temp1136.curr, _temp1136.curr);})); goto _LL1125; _LL1131: goto _LL1125;
_LL1125:;}}{ struct Cyc_List_List* tvs= f->tvs; Cyc_Tcutil_check_unique_tvars(
loc, tvs);{ struct Cyc_List_List* typs= f->typs; for( 0; typs != 0; typs=({
struct Cyc_List_List* _temp1138= typs; if( _temp1138 == 0){ _throw(
Null_Exception);} _temp1138->tl;})){ void* t=(*(( struct _tuple5*)({ struct Cyc_List_List*
_temp1139= typs; if( _temp1139 == 0){ _throw( Null_Exception);} _temp1139->hd;}))).f2;
Cyc_Tcutil_check_type( f->loc, te, tvs,( void*) Cyc_Absyn_MemKind, t);}}{ int
dup= 0;{ struct Cyc_List_List* prev_fs= xd_result->fields; for( 0; prev_fs != 0;
prev_fs=({ struct Cyc_List_List* _temp1140= prev_fs; if( _temp1140 == 0){ _throw(
Null_Exception);} _temp1140->tl;})){ struct Cyc_Absyn_Tunionfield* prev=( struct
Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp1143= prev_fs; if(
_temp1143 == 0){ _throw( Null_Exception);} _temp1143->hd;}); if( Cyc_Absyn_qvar_cmp(
f->name, prev->name) == 0){ Cyc_Tc_field_redecl_okay( prev, f, 0, te,( struct
_tagged_string)({ char* _temp1141=( char*)"xtunion"; struct _tagged_string
_temp1142; _temp1142.curr= _temp1141; _temp1142.base= _temp1141; _temp1142.last_plus_one=
_temp1141 + 8; _temp1142;}), v); dup= 1; break;}}} if( dup){ continue;}
xd_result->fields=({ struct Cyc_List_List* _temp1144=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1144->hd=( void*) f; _temp1144->tl=
xd_result->fields; _temp1144;});{ struct Cyc_Tcenv_Genv* ge_f; struct Cyc_List_List*
ns_f;{ void* _temp1145=(* f->name).f1; struct Cyc_List_List* _temp1153; struct
Cyc_List_List* _temp1155; _LL1147: if(( unsigned int) _temp1145 > 1u?(( struct
_tunion_struct*) _temp1145)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL1154: _temp1153=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp1145)->f1; goto
_LL1148;} else{ goto _LL1149;} _LL1149: if(( int) _temp1145 == Cyc_Absyn_Loc_n){
goto _LL1150;} else{ goto _LL1151;} _LL1151: if(( unsigned int) _temp1145 > 1u?((
struct _tunion_struct*) _temp1145)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL1156:
_temp1155=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp1145)->f1;
goto _LL1152;} else{ goto _LL1146;} _LL1148: ns_f= _temp1153; goto _LL1146;
_LL1150: goto _LL1152; _LL1152:( void) _throw(( struct _xtunion_struct*)({
struct Cyc_Core_Impossible_struct* _temp1157=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp1157[ 0]=({ struct
Cyc_Core_Impossible_struct _temp1158; _temp1158.tag= Cyc_Core_Impossible_tag;
_temp1158.f1=( struct _tagged_string)({ char* _temp1159=( char*)"tcXTuniondecl: Rel_n or Loc_n";
struct _tagged_string _temp1160; _temp1160.curr= _temp1159; _temp1160.base=
_temp1159; _temp1160.last_plus_one= _temp1159 + 30; _temp1160;}); _temp1158;});
_temp1157;})); goto _LL1146; _LL1146:;}{ struct _handler_cons _temp1161;
_push_handler(& _temp1161);{ struct _xtunion_struct* _temp1162=( struct
_xtunion_struct*) setjmp( _temp1161.handler); if( ! _temp1162){ ge_f=(( struct
Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)(
te->ae, ns_f);; _pop_handler();} else{ struct _xtunion_struct* _temp1164=
_temp1162; _LL1166: if( _temp1164->tag == Cyc_Dict_Absent_tag){ goto _LL1167;}
else{ goto _LL1168;} _LL1168: goto _LL1169; _LL1167: Cyc_Tcutil_terr( f->loc,({
struct _tagged_string _temp1170= Cyc_Absynpp_qvar2string( f->name); xprintf("bad namespace for xtunion field %.*s",
_temp1170.last_plus_one - _temp1170.curr, _temp1170.curr);})); return; _LL1169:(
void) _throw( _temp1164); _LL1165:;}}} ge_f->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct _tuple6* data)) Cyc_Dict_insert)(
ge_f->ordinaries,(* f->name).f2,({ struct _tuple6* _temp1171=( struct _tuple6*)
GC_malloc( sizeof( struct _tuple6)); _temp1171->f1=( void*)({ struct Cyc_Tcenv_XTunionRes_struct*
_temp1172=( struct Cyc_Tcenv_XTunionRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_XTunionRes_struct));
_temp1172[ 0]=({ struct Cyc_Tcenv_XTunionRes_struct _temp1173; _temp1173.tag=
Cyc_Tcenv_XTunionRes_tag; _temp1173.f1= xd_result; _temp1173.f2= f; _temp1173;});
_temp1172;}); _temp1171->f2= 1; _temp1171;}));}}}}}} static void Cyc_Tc_tcEnumdecl(
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
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1238=* _temp1214; if(
_temp1238 == 0){ _throw( Null_Exception);} _temp1238;})); tag_count= t1 + 1; if(
t1 != Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1235= _temp1223; if( _temp1235 == 0){ _throw( Null_Exception);} _temp1235;}))){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp1236=* v; struct
_tagged_string _temp1237=*(* _temp1215).f2; xprintf("redeclaration of enum %.*s, field %.*s, value mismatch",
_temp1236.last_plus_one - _temp1236.curr, _temp1236.curr, _temp1237.last_plus_one
- _temp1237.curr, _temp1237.curr);}));}}}} return;}} _LL1193:;}{ struct Cyc_List_List*
prev_fields= 0; unsigned int tag_count= 0;{ struct Cyc_List_List* fs= ed->fields;
for( 0; fs != 0; fs=({ struct Cyc_List_List* _temp1239= fs; if( _temp1239 == 0){
_throw( Null_Exception);} _temp1239->tl;})){ struct Cyc_Absyn_Enumfield* f=(
struct Cyc_Absyn_Enumfield*)({ struct Cyc_List_List* _temp1251= fs; if(
_temp1251 == 0){ _throw( Null_Exception);} _temp1251->hd;}); if((( int(*)( int(*
compare)( struct _tagged_string*, struct _tagged_string*), struct Cyc_List_List*
l, struct _tagged_string* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp, prev_fields,(*
f->name).f2)){ Cyc_Tcutil_terr( f->loc,({ struct _tagged_string _temp1240=*(* f->name).f2;
xprintf("duplicate field name %.*s", _temp1240.last_plus_one - _temp1240.curr,
_temp1240.curr);}));} prev_fields=({ struct Cyc_List_List* _temp1241=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1241->hd=( void*)(*
f->name).f2; _temp1241->tl= prev_fields; _temp1241;}); if( f->tag == 0){ f->tag=(
struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp( tag_count, f->loc);} else{ if( ! Cyc_Tcutil_is_const_exp(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1242= f->tag; if(
_temp1242 == 0){ _throw( Null_Exception);} _temp1242;}))){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp1243=* v; struct _tagged_string _temp1244=*(* f->name).f2;
xprintf("enum %.*s, field %.*s: expression is not constant", _temp1243.last_plus_one
- _temp1243.curr, _temp1243.curr, _temp1244.last_plus_one - _temp1244.curr,
_temp1244.curr);}));}}{ unsigned int t1= Cyc_Evexp_eval_const_uint_exp(( struct
Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1250= f->tag; if( _temp1250 == 0){
_throw( Null_Exception);} _temp1250;})); tag_count= t1 + 1;(* f->name).f1=( void*)({
struct Cyc_Absyn_Abs_n_struct* _temp1245=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp1245[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp1246; _temp1246.tag= Cyc_Absyn_Abs_n_tag; _temp1246.f1= te->ns; _temp1246;});
_temp1245;}); ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct _tuple6* data)) Cyc_Dict_insert)( ge->ordinaries,(*
f->name).f2,({ struct _tuple6* _temp1247=( struct _tuple6*) GC_malloc( sizeof(
struct _tuple6)); _temp1247->f1=( void*)({ struct Cyc_Tcenv_EnumRes_struct*
_temp1248=( struct Cyc_Tcenv_EnumRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_EnumRes_struct));
_temp1248[ 0]=({ struct Cyc_Tcenv_EnumRes_struct _temp1249; _temp1249.tag= Cyc_Tcenv_EnumRes_tag;
_temp1249.f1= ed; _temp1249.f2= f; _temp1249;}); _temp1248;}); _temp1247->f2= 1;
_temp1247;}));}}} ge->enumdecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
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
_temp1375= ds; if( _temp1375 == 0){ _throw( Null_Exception);} _temp1375->hd;});
struct Cyc_Position_Segment* loc= d->loc; void* _temp1272=( void*) d->r; int
_temp1298; struct Cyc_Absyn_Exp* _temp1300; struct Cyc_Core_Opt* _temp1302;
struct Cyc_Core_Opt* _temp1304; struct Cyc_Absyn_Pat* _temp1306; struct Cyc_Absyn_Vardecl*
_temp1308; struct Cyc_Absyn_Fndecl* _temp1310; struct Cyc_Absyn_Typedefdecl*
_temp1312; struct Cyc_Absyn_Structdecl* _temp1314; struct Cyc_Absyn_Uniondecl*
_temp1316; struct Cyc_Absyn_Tuniondecl* _temp1318; struct Cyc_Absyn_XTuniondecl*
_temp1320; struct Cyc_Absyn_Enumdecl* _temp1322; struct Cyc_List_List* _temp1324;
struct _tagged_string* _temp1326; struct Cyc_List_List* _temp1328; struct
_tuple0* _temp1330; struct _tuple0 _temp1332; struct _tagged_string* _temp1333;
void* _temp1335; struct Cyc_List_List* _temp1337; _LL1274: if((( struct
_tunion_struct*) _temp1272)->tag == Cyc_Absyn_Let_d_tag){ _LL1307: _temp1306=(
struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*) _temp1272)->f1; goto
_LL1305; _LL1305: _temp1304=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1272)->f2; goto _LL1303; _LL1303: _temp1302=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Let_d_struct*) _temp1272)->f3; goto _LL1301; _LL1301: _temp1300=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*) _temp1272)->f4; goto
_LL1299; _LL1299: _temp1298=( int)(( struct Cyc_Absyn_Let_d_struct*) _temp1272)->f5;
goto _LL1275;} else{ goto _LL1276;} _LL1276: if((( struct _tunion_struct*)
_temp1272)->tag == Cyc_Absyn_Var_d_tag){ _LL1309: _temp1308=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_d_struct*) _temp1272)->f1; goto _LL1277;} else{ goto
_LL1278;} _LL1278: if((( struct _tunion_struct*) _temp1272)->tag == Cyc_Absyn_Fn_d_tag){
_LL1311: _temp1310=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*)
_temp1272)->f1; goto _LL1279;} else{ goto _LL1280;} _LL1280: if((( struct
_tunion_struct*) _temp1272)->tag == Cyc_Absyn_Typedef_d_tag){ _LL1313: _temp1312=(
struct Cyc_Absyn_Typedefdecl*)(( struct Cyc_Absyn_Typedef_d_struct*) _temp1272)->f1;
goto _LL1281;} else{ goto _LL1282;} _LL1282: if((( struct _tunion_struct*)
_temp1272)->tag == Cyc_Absyn_Struct_d_tag){ _LL1315: _temp1314=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_d_struct*) _temp1272)->f1; goto _LL1283;} else{ goto
_LL1284;} _LL1284: if((( struct _tunion_struct*) _temp1272)->tag == Cyc_Absyn_Union_d_tag){
_LL1317: _temp1316=( struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*)
_temp1272)->f1; goto _LL1285;} else{ goto _LL1286;} _LL1286: if((( struct
_tunion_struct*) _temp1272)->tag == Cyc_Absyn_Tunion_d_tag){ _LL1319: _temp1318=(
struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_d_struct*) _temp1272)->f1;
goto _LL1287;} else{ goto _LL1288;} _LL1288: if((( struct _tunion_struct*)
_temp1272)->tag == Cyc_Absyn_XTunion_d_tag){ _LL1321: _temp1320=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_d_struct*) _temp1272)->f1; goto _LL1289;} else{ goto
_LL1290;} _LL1290: if((( struct _tunion_struct*) _temp1272)->tag == Cyc_Absyn_Enum_d_tag){
_LL1323: _temp1322=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*)
_temp1272)->f1; goto _LL1291;} else{ goto _LL1292;} _LL1292: if((( struct
_tunion_struct*) _temp1272)->tag == Cyc_Absyn_Namespace_d_tag){ _LL1327:
_temp1326=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1272)->f1; goto _LL1325; _LL1325: _temp1324=( struct Cyc_List_List*)((
struct Cyc_Absyn_Namespace_d_struct*) _temp1272)->f2; goto _LL1293;} else{ goto
_LL1294;} _LL1294: if((( struct _tunion_struct*) _temp1272)->tag == Cyc_Absyn_Using_d_tag){
_LL1331: _temp1330=( struct _tuple0*)(( struct Cyc_Absyn_Using_d_struct*)
_temp1272)->f1; _temp1332=* _temp1330; _LL1336: _temp1335= _temp1332.f1; goto
_LL1334; _LL1334: _temp1333= _temp1332.f2; goto _LL1329; _LL1329: _temp1328=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*) _temp1272)->f2; goto
_LL1295;} else{ goto _LL1296;} _LL1296: if((( struct _tunion_struct*) _temp1272)->tag
== Cyc_Absyn_ExternC_d_tag){ _LL1338: _temp1337=( struct Cyc_List_List*)((
struct Cyc_Absyn_ExternC_d_struct*) _temp1272)->f1; goto _LL1297;} else{ goto
_LL1273;} _LL1275: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp1339=( char*)"top level let-declarations are not implemented"; struct
_tagged_string _temp1340; _temp1340.curr= _temp1339; _temp1340.base= _temp1339;
_temp1340.last_plus_one= _temp1339 + 47; _temp1340;})); goto _LL1273; _LL1277:
if( in_externC? Cyc_Tc_okay_externC( d->loc,( void*) _temp1308->sc): 0){( void*)(
_temp1308->sc=( void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcVardecl( te, ge,
loc, _temp1308); goto _LL1273; _LL1279: if( in_externC? Cyc_Tc_okay_externC( d->loc,(
void*) _temp1310->sc): 0){( void*)( _temp1310->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcFndecl( te, ge, loc, _temp1310); goto _LL1273; _LL1281: Cyc_Tc_tcTypedefdecl(
te, ge, loc, _temp1312); goto _LL1273; _LL1283: if( in_externC? Cyc_Tc_okay_externC(
d->loc,( void*) _temp1314->sc): 0){( void*)( _temp1314->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcStructdecl( te, ge, loc, _temp1314); goto _LL1273; _LL1285: if(
in_externC? Cyc_Tc_okay_externC( d->loc,( void*) _temp1316->sc): 0){( void*)(
_temp1316->sc=( void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcUniondecl( te, ge,
loc, _temp1316); goto _LL1273; _LL1287: if( in_externC? Cyc_Tc_okay_externC( d->loc,(
void*) _temp1318->sc): 0){( void*)( _temp1318->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcTuniondecl( te, ge, loc, _temp1318); goto _LL1273; _LL1289: if(
in_externC? Cyc_Tc_okay_externC( d->loc,( void*) _temp1320->sc): 0){( void*)(
_temp1320->sc=( void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcXTuniondecl( te,
ge, loc, _temp1320); goto _LL1273; _LL1291: if( in_externC? Cyc_Tc_okay_externC(
d->loc,( void*) _temp1322->sc): 0){( void*)( _temp1322->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcEnumdecl( te, ge, loc, _temp1322); goto _LL1273; _LL1293: { struct Cyc_List_List*
ns= te->ns; struct Cyc_List_List* ns2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( ns,({ struct Cyc_List_List*
_temp1341=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1341->hd=( void*) _temp1326; _temp1341->tl= 0; _temp1341;})); if( !(( int(*)(
struct Cyc_Set_Set* s, struct _tagged_string* elt)) Cyc_Set_member)( ge->namespaces,
_temp1326)){ ge->namespaces=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s,
struct _tagged_string* elt)) Cyc_Set_insert)( ge->namespaces, _temp1326); te->ae=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key,
struct Cyc_Tcenv_Genv* data)) Cyc_Dict_insert)( te->ae, ns2, Cyc_Tcenv_empty_genv());}
te->ns= ns2; Cyc_Tc_tc_decls( te, _temp1324, in_externC); te->ns= ns; goto
_LL1273;} _LL1295: { struct _tagged_string* first; struct Cyc_List_List* rest;{
void* _temp1342= _temp1335; struct Cyc_List_List* _temp1354; struct Cyc_List_List*
_temp1356; struct Cyc_List_List* _temp1358; struct Cyc_List_List _temp1360;
struct Cyc_List_List* _temp1361; struct _tagged_string* _temp1363; struct Cyc_List_List*
_temp1365; struct Cyc_List_List _temp1367; struct Cyc_List_List* _temp1368;
struct _tagged_string* _temp1370; _LL1344: if(( int) _temp1342 == Cyc_Absyn_Loc_n){
goto _LL1345;} else{ goto _LL1346;} _LL1346: if(( unsigned int) _temp1342 > 1u?((
struct _tunion_struct*) _temp1342)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL1355:
_temp1354=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp1342)->f1;
if( _temp1354 == 0){ goto _LL1347;} else{ goto _LL1348;}} else{ goto _LL1348;}
_LL1348: if(( unsigned int) _temp1342 > 1u?(( struct _tunion_struct*) _temp1342)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL1357: _temp1356=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp1342)->f1; if( _temp1356 == 0){ goto _LL1349;}
else{ goto _LL1350;}} else{ goto _LL1350;} _LL1350: if(( unsigned int) _temp1342
> 1u?(( struct _tunion_struct*) _temp1342)->tag == Cyc_Absyn_Rel_n_tag: 0){
_LL1359: _temp1358=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp1342)->f1; if( _temp1358 == 0){ goto _LL1352;} else{ _temp1360=* _temp1358;
_LL1364: _temp1363=( struct _tagged_string*) _temp1360.hd; goto _LL1362; _LL1362:
_temp1361=( struct Cyc_List_List*) _temp1360.tl; goto _LL1351;}} else{ goto
_LL1352;} _LL1352: if(( unsigned int) _temp1342 > 1u?(( struct _tunion_struct*)
_temp1342)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL1366: _temp1365=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp1342)->f1; if( _temp1365 == 0){ goto
_LL1343;} else{ _temp1367=* _temp1365; _LL1371: _temp1370=( struct
_tagged_string*) _temp1367.hd; goto _LL1369; _LL1369: _temp1368=( struct Cyc_List_List*)
_temp1367.tl; goto _LL1353;}} else{ goto _LL1343;} _LL1345: goto _LL1347;
_LL1347: goto _LL1349; _LL1349: first= _temp1333; rest= 0; goto _LL1343; _LL1351:
_temp1370= _temp1363; _temp1368= _temp1361; goto _LL1353; _LL1353: first=
_temp1370; rest=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( _temp1368,({ struct Cyc_List_List* _temp1372=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1372->hd=( void*) _temp1333;
_temp1372->tl= 0; _temp1372;})); goto _LL1343; _LL1343:;}{ struct Cyc_List_List*
ns2= Cyc_Tcenv_resolve_namespace( te, loc, first, rest); ge->availables=({
struct Cyc_List_List* _temp1373=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1373->hd=( void*) ns2; _temp1373->tl= ge->availables;
_temp1373;}); Cyc_Tc_tc_decls( te, _temp1328, in_externC); ge->availables=({
struct Cyc_List_List* _temp1374= ge->availables; if( _temp1374 == 0){ _throw(
Null_Exception);} _temp1374->tl;}); goto _LL1273;}} _LL1297: Cyc_Tc_tc_decls( te,
_temp1337, 1); goto _LL1273; _LL1273:;}} static char _temp1378[ 4u]="Cyc";
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
l)) Cyc_List_filter_c)( Cyc_Tc_vardecl_needed, env, ds);} struct _tuple9{ struct
Cyc_Tcenv_Genv* f1; struct Cyc_Set_Set* f2; } ; static int Cyc_Tc_vardecl_needed(
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