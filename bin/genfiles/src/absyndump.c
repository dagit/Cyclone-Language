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
char* tag; struct _tagged_string f1; } ; extern struct _tagged_string Cyc_Core_string_of_int(
int); extern char* string_to_Cstring( struct _tagged_string); extern char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( char*); extern int system( char*); struct Cyc_Stdio___sFILE;
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; extern struct Cyc_Stdio___sFILE*
Cyc_Stdio_stdout; typedef int Cyc_Stdio_fpos_t; extern int Cyc_Stdio_fputc( int,
struct Cyc_Stdio___sFILE*); extern char Cyc_Stdio_FileOpenError_tag[ 14u];
struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Stdio_FileCloseError_tag[ 15u]; struct Cyc_Stdio_FileCloseError_struct{
char* tag; } ; extern int Cyc_Stdio_file_string_write( struct Cyc_Stdio___sFILE*
fd, struct _tagged_string src, int src_offset, int max_count); struct Cyc_List_List{
void* hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List* Cyc_List_glist_t;
typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t;
extern int Cyc_List_length( struct Cyc_List_List* x); extern char Cyc_List_List_empty_tag[
11u]; struct Cyc_List_List_empty_struct{ char* tag; } ; extern struct Cyc_List_List*
Cyc_List_map( void*(* f)( void*), struct Cyc_List_List* x); extern char Cyc_List_List_mismatch_tag[
14u]; struct Cyc_List_List_mismatch_struct{ char* tag; } ; extern struct Cyc_List_List*
Cyc_List_imp_rev( struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_imp_append(
struct Cyc_List_List* x, struct Cyc_List_List* y); extern char Cyc_List_Nth_tag[
4u]; struct Cyc_List_Nth_struct{ char* tag; } ; struct Cyc_Lineno_Pos{ struct
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
char* tag; } ; extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct
Cyc_Absyn_Conref* x); extern int Cyc_Absyn_is_format_prim( void* p); extern
struct _tagged_string Cyc_Absyn_attribute2string( void*); struct Cyc_PP_Ppstate;
typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t; struct Cyc_PP_Out; typedef
struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc; typedef struct Cyc_PP_Doc*
Cyc_PP_doc_t; extern int Cyc_Absynpp_qvar_to_Cids; extern int Cyc_Absynpp_exp_prec(
struct Cyc_Absyn_Exp*); extern struct _tagged_string Cyc_Absynpp_char_escape(
char); extern struct _tagged_string Cyc_Absynpp_string_escape( struct
_tagged_string); extern struct _tagged_string Cyc_Absynpp_prim2str( void* p);
extern int Cyc_Absynpp_is_declaration( struct Cyc_Absyn_Stmt* s); struct _tuple3{
struct _tagged_string* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; extern
struct _tuple1* Cyc_Absynpp_arg_mk_opt( struct _tuple3* arg); struct _tuple4{
struct Cyc_Absyn_Tqual f1; void* f2; struct Cyc_List_List* f3; } ; extern struct
_tuple4 Cyc_Absynpp_to_tms( struct Cyc_Absyn_Tqual tq, void* t); extern
unsigned int Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp* e); struct Cyc_Set_Set;
typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct Cyc_Set_Set* Cyc_Set_hset_t;
typedef struct Cyc_Set_Set* Cyc_Set_set_t; extern char Cyc_Set_Absent_tag[ 7u];
struct Cyc_Set_Absent_struct{ char* tag; } ; struct Cyc_Dict_Dict; typedef
struct Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t;
extern char Cyc_Dict_Present_tag[ 8u]; struct Cyc_Dict_Present_struct{ char* tag;
} ; extern char Cyc_Dict_Absent_tag[ 7u]; typedef void* Cyc_Tcenv_Resolved;
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
extern char Cyc_Tcutil_TypeErr_tag[ 8u]; struct Cyc_Tcutil_TypeErr_struct{ char*
tag; } ; extern void* Cyc_Tcutil_compress( void* t); extern void Cyc_Absyndump_dumptyp(
void*); extern void Cyc_Absyndump_dumpntyp( void* t); extern void Cyc_Absyndump_dumpexp(
struct Cyc_Absyn_Exp*); extern void Cyc_Absyndump_dumpexp_prec( int, struct Cyc_Absyn_Exp*);
extern void Cyc_Absyndump_dumppat( struct Cyc_Absyn_Pat*); extern void Cyc_Absyndump_dumpstmt(
struct Cyc_Absyn_Stmt*); extern void Cyc_Absyndump_dumpdecl( struct Cyc_Absyn_Decl*);
extern void Cyc_Absyndump_dumptqtd( struct Cyc_Absyn_Tqual, void*, void(* f)(
void*), void*); typedef struct _tagged_string Cyc_Absyndump_dump_string_t;
struct Cyc_Stdio___sFILE** Cyc_Absyndump_dump_file=& Cyc_Stdio_stdout; void Cyc_Absyndump_ignore(
void* x){ return;} static unsigned int Cyc_Absyndump_pos= 0; void Cyc_Absyndump_dump(
struct _tagged_string s){ int sz=( int)({ struct _tagged_string _temp3= s;(
unsigned int)( _temp3.last_plus_one - _temp3.curr);}); if( !(( int)({ struct
_tagged_string _temp0= s; char* _temp2= _temp0.curr +( sz - 1); if( _temp0.base
== 0? 1:( _temp2 < _temp0.base? 1: _temp2 >= _temp0.last_plus_one)){ _throw(
Null_Exception);}* _temp2;}))){ -- sz;} Cyc_Absyndump_pos += sz + 1; if( Cyc_Absyndump_pos
> 80){ Cyc_Absyndump_pos=( unsigned int) sz; Cyc_Stdio_fputc(( int)'\n',* Cyc_Absyndump_dump_file);}
else{ Cyc_Stdio_fputc(( int)' ',* Cyc_Absyndump_dump_file);} Cyc_Stdio_file_string_write(*
Cyc_Absyndump_dump_file, s, 0, sz);} void Cyc_Absyndump_dump_nospace( struct
_tagged_string s){ int sz=( int)({ struct _tagged_string _temp7= s;(
unsigned int)( _temp7.last_plus_one - _temp7.curr);}); if( !(( int)({ struct
_tagged_string _temp4= s; char* _temp6= _temp4.curr +( sz - 1); if( _temp4.base
== 0? 1:( _temp6 < _temp4.base? 1: _temp6 >= _temp4.last_plus_one)){ _throw(
Null_Exception);}* _temp6;}))){ -- sz;} Cyc_Absyndump_pos += sz; Cyc_Stdio_file_string_write(*
Cyc_Absyndump_dump_file, s, 0, sz);} void Cyc_Absyndump_dump_char( int c){ ++
Cyc_Absyndump_pos; Cyc_Stdio_fputc( c,* Cyc_Absyndump_dump_file);} void Cyc_Absyndump_dump_str(
struct _tagged_string* s){ Cyc_Absyndump_dump(* s);} void Cyc_Absyndump_dump_semi(){
Cyc_Absyndump_dump_char(( int)';');} void Cyc_Absyndump_dump_sep( void(* f)(
void*), struct Cyc_List_List* l, struct _tagged_string sep){ if( l == 0){
return;} for( 0;({ struct Cyc_List_List* _temp8= l; if( _temp8 == 0){ _throw(
Null_Exception);} _temp8->tl;}) != 0; l=({ struct Cyc_List_List* _temp9= l; if(
_temp9 == 0){ _throw( Null_Exception);} _temp9->tl;})){ f(( void*)({ struct Cyc_List_List*
_temp10= l; if( _temp10 == 0){ _throw( Null_Exception);} _temp10->hd;})); Cyc_Absyndump_dump_nospace(
sep);} f(( void*)({ struct Cyc_List_List* _temp11= l; if( _temp11 == 0){ _throw(
Null_Exception);} _temp11->hd;}));} void Cyc_Absyndump_dump_sep_c( void(* f)(
void*, void*), void* env, struct Cyc_List_List* l, struct _tagged_string sep){
if( l == 0){ return;} for( 0;({ struct Cyc_List_List* _temp12= l; if( _temp12 ==
0){ _throw( Null_Exception);} _temp12->tl;}) != 0; l=({ struct Cyc_List_List*
_temp13= l; if( _temp13 == 0){ _throw( Null_Exception);} _temp13->tl;})){ f( env,(
void*)({ struct Cyc_List_List* _temp14= l; if( _temp14 == 0){ _throw(
Null_Exception);} _temp14->hd;})); Cyc_Absyndump_dump_nospace( sep);} f( env,(
void*)({ struct Cyc_List_List* _temp15= l; if( _temp15 == 0){ _throw(
Null_Exception);} _temp15->hd;}));} void Cyc_Absyndump_group( void(* f)( void*),
struct Cyc_List_List* l, struct _tagged_string start, struct _tagged_string end,
struct _tagged_string sep){ Cyc_Absyndump_dump_nospace( start);(( void(*)( void(*
f)( void*), struct Cyc_List_List* l, struct _tagged_string sep)) Cyc_Absyndump_dump_sep)(
f, l, sep); Cyc_Absyndump_dump_nospace( end);} void Cyc_Absyndump_group_c( void(*
f)( void*, void*), void* env, struct Cyc_List_List* l, struct _tagged_string
start, struct _tagged_string end, struct _tagged_string sep){ Cyc_Absyndump_dump_nospace(
start);(( void(*)( void(* f)( void*, void*), void* env, struct Cyc_List_List* l,
struct _tagged_string sep)) Cyc_Absyndump_dump_sep_c)( f, env, l, sep); Cyc_Absyndump_dump_nospace(
end);} void Cyc_Absyndump_egroup( void(* f)( void*), struct Cyc_List_List* l,
struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep){ if( l != 0){(( void(*)( void(* f)( void*), struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)(
f, l, start, end, sep);}} void Cyc_Absyndump_dumpqvar( struct _tuple0* v){
struct Cyc_List_List* nsl= 0;{ void* _temp16=(* v).f1; struct Cyc_List_List*
_temp28; struct Cyc_List_List* _temp30; struct Cyc_List_List* _temp32; struct
Cyc_List_List* _temp34; _LL18: if(( int) _temp16 == Cyc_Absyn_Loc_n){ goto _LL19;}
else{ goto _LL20;} _LL20: if(( unsigned int) _temp16 > 1u?(( struct
_tunion_struct*) _temp16)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL29: _temp28=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp16)->f1; if(
_temp28 == 0){ goto _LL21;} else{ goto _LL22;}} else{ goto _LL22;} _LL22: if((
unsigned int) _temp16 > 1u?(( struct _tunion_struct*) _temp16)->tag == Cyc_Absyn_Abs_n_tag:
0){ _LL31: _temp30=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp16)->f1; if( _temp30 == 0){ goto _LL23;} else{ goto _LL24;}} else{ goto
_LL24;} _LL24: if(( unsigned int) _temp16 > 1u?(( struct _tunion_struct*)
_temp16)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL33: _temp32=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp16)->f1; goto _LL25;} else{ goto _LL26;}
_LL26: if(( unsigned int) _temp16 > 1u?(( struct _tunion_struct*) _temp16)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL35: _temp34=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp16)->f1; goto _LL27;} else{ goto _LL17;} _LL19: goto _LL21; _LL21: goto
_LL23; _LL23: Cyc_Absyndump_dump_str((* v).f2); return; _LL25: _temp34= _temp32;
goto _LL27; _LL27: nsl= _temp34; goto _LL17; _LL17:;} Cyc_Absyndump_dump_str((
struct _tagged_string*)({ struct Cyc_List_List* _temp36= nsl; if( _temp36 == 0){
_throw( Null_Exception);} _temp36->hd;})); for( nsl=({ struct Cyc_List_List*
_temp37= nsl; if( _temp37 == 0){ _throw( Null_Exception);} _temp37->tl;}); nsl
!= 0; nsl=({ struct Cyc_List_List* _temp38= nsl; if( _temp38 == 0){ _throw(
Null_Exception);} _temp38->tl;})){ if( Cyc_Absynpp_qvar_to_Cids){ Cyc_Absyndump_dump_char((
int)'_');} else{ Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char*
_temp39=( char*)"::"; struct _tagged_string _temp40; _temp40.curr= _temp39;
_temp40.base= _temp39; _temp40.last_plus_one= _temp39 + 3; _temp40;}));} Cyc_Absyndump_dump_nospace(*((
struct _tagged_string*)({ struct Cyc_List_List* _temp41= nsl; if( _temp41 == 0){
_throw( Null_Exception);} _temp41->hd;})));} if( Cyc_Absynpp_qvar_to_Cids){ Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp42=( char*)"_"; struct _tagged_string
_temp43; _temp43.curr= _temp42; _temp43.base= _temp42; _temp43.last_plus_one=
_temp42 + 2; _temp43;}));} else{ Cyc_Absyndump_dump_nospace(( struct
_tagged_string)({ char* _temp44=( char*)"::"; struct _tagged_string _temp45;
_temp45.curr= _temp44; _temp45.base= _temp44; _temp45.last_plus_one= _temp44 + 3;
_temp45;}));} Cyc_Absyndump_dump_nospace(*(* v).f2);} void Cyc_Absyndump_dumptq(
struct Cyc_Absyn_Tqual tq){ if( tq.q_restrict){ Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp46=( char*)"restrict"; struct _tagged_string
_temp47; _temp47.curr= _temp46; _temp47.base= _temp46; _temp47.last_plus_one=
_temp46 + 9; _temp47;}));} if( tq.q_volatile){ Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp48=( char*)"volatile"; struct _tagged_string
_temp49; _temp49.curr= _temp48; _temp49.base= _temp48; _temp49.last_plus_one=
_temp48 + 9; _temp49;}));} if( tq.q_const){ Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp50=( char*)"const"; struct _tagged_string _temp51;
_temp51.curr= _temp50; _temp51.base= _temp50; _temp51.last_plus_one= _temp50 + 6;
_temp51;}));}} void Cyc_Absyndump_dumpscope( void* sc){ void* _temp52= sc; _LL54:
if(( int) _temp52 == Cyc_Absyn_Static){ goto _LL55;} else{ goto _LL56;} _LL56:
if(( int) _temp52 == Cyc_Absyn_Public){ goto _LL57;} else{ goto _LL58;} _LL58:
if(( int) _temp52 == Cyc_Absyn_Extern){ goto _LL59;} else{ goto _LL60;} _LL60:
if(( int) _temp52 == Cyc_Absyn_ExternC){ goto _LL61;} else{ goto _LL62;} _LL62:
if(( int) _temp52 == Cyc_Absyn_Abstract){ goto _LL63;} else{ goto _LL53;} _LL55:
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp64=( char*)"static";
struct _tagged_string _temp65; _temp65.curr= _temp64; _temp65.base= _temp64;
_temp65.last_plus_one= _temp64 + 7; _temp65;})); return; _LL57: return; _LL59:
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp66=( char*)"extern";
struct _tagged_string _temp67; _temp67.curr= _temp66; _temp67.base= _temp66;
_temp67.last_plus_one= _temp66 + 7; _temp67;})); return; _LL61: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp68=( char*)"extern \"C\""; struct
_tagged_string _temp69; _temp69.curr= _temp68; _temp69.base= _temp68; _temp69.last_plus_one=
_temp68 + 11; _temp69;})); return; _LL63: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp70=( char*)"abstract"; struct _tagged_string
_temp71; _temp71.curr= _temp70; _temp71.base= _temp70; _temp71.last_plus_one=
_temp70 + 9; _temp71;})); return; _LL53:;} void Cyc_Absyndump_dumpkind( void* k){
void* _temp72= k; _LL74: if(( int) _temp72 == Cyc_Absyn_AnyKind){ goto _LL75;}
else{ goto _LL76;} _LL76: if(( int) _temp72 == Cyc_Absyn_MemKind){ goto _LL77;}
else{ goto _LL78;} _LL78: if(( int) _temp72 == Cyc_Absyn_BoxKind){ goto _LL79;}
else{ goto _LL80;} _LL80: if(( int) _temp72 == Cyc_Absyn_RgnKind){ goto _LL81;}
else{ goto _LL82;} _LL82: if(( int) _temp72 == Cyc_Absyn_EffKind){ goto _LL83;}
else{ goto _LL73;} _LL75: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp84=( char*)"A"; struct _tagged_string _temp85; _temp85.curr= _temp84;
_temp85.base= _temp84; _temp85.last_plus_one= _temp84 + 2; _temp85;})); return;
_LL77: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp86=( char*)"M";
struct _tagged_string _temp87; _temp87.curr= _temp86; _temp87.base= _temp86;
_temp87.last_plus_one= _temp86 + 2; _temp87;})); return; _LL79: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp88=( char*)"B"; struct _tagged_string
_temp89; _temp89.curr= _temp88; _temp89.base= _temp88; _temp89.last_plus_one=
_temp88 + 2; _temp89;})); return; _LL81: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp90=( char*)"R"; struct _tagged_string _temp91;
_temp91.curr= _temp90; _temp91.base= _temp90; _temp91.last_plus_one= _temp90 + 2;
_temp91;})); return; _LL83: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp92=( char*)"E"; struct _tagged_string _temp93; _temp93.curr= _temp92;
_temp93.base= _temp92; _temp93.last_plus_one= _temp92 + 2; _temp93;})); return;
_LL73:;} void Cyc_Absyndump_dumptps( struct Cyc_List_List* ts){(( void(*)( void(*
f)( void*), struct Cyc_List_List* l, struct _tagged_string start, struct
_tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_egroup)( Cyc_Absyndump_dumptyp,
ts,( struct _tagged_string)({ char* _temp94=( char*)"<"; struct _tagged_string
_temp95; _temp95.curr= _temp94; _temp95.base= _temp94; _temp95.last_plus_one=
_temp94 + 2; _temp95;}),( struct _tagged_string)({ char* _temp96=( char*)">";
struct _tagged_string _temp97; _temp97.curr= _temp96; _temp97.base= _temp96;
_temp97.last_plus_one= _temp96 + 2; _temp97;}),( struct _tagged_string)({ char*
_temp98=( char*)","; struct _tagged_string _temp99; _temp99.curr= _temp98;
_temp99.base= _temp98; _temp99.last_plus_one= _temp98 + 2; _temp99;}));} void
Cyc_Absyndump_dumptvar( struct Cyc_Absyn_Tvar* tv){ Cyc_Absyndump_dump_str( tv->name);}
void Cyc_Absyndump_dumpkindedtvar( struct Cyc_Absyn_Tvar* tv){ Cyc_Absyndump_dump_str(
tv->name);{ void* _temp100=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( tv->kind))->v; void* _temp108; void* _temp110;
_LL102: if(( unsigned int) _temp100 > 1u?(( struct _tunion_struct*) _temp100)->tag
== Cyc_Absyn_Eq_constr_tag: 0){ _LL109: _temp108=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp100)->f1; if(( int) _temp108 == Cyc_Absyn_BoxKind){ goto _LL103;} else{
goto _LL104;}} else{ goto _LL104;} _LL104: if(( unsigned int) _temp100 > 1u?((
struct _tunion_struct*) _temp100)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL111:
_temp110=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp100)->f1; goto
_LL105;} else{ goto _LL106;} _LL106: goto _LL107; _LL103: goto _LL101; _LL105:
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp112=( char*)"::";
struct _tagged_string _temp113; _temp113.curr= _temp112; _temp113.base= _temp112;
_temp113.last_plus_one= _temp112 + 3; _temp113;})); Cyc_Absyndump_dumpkind(
_temp110); goto _LL101; _LL107: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp114=( char*)"::?"; struct _tagged_string _temp115; _temp115.curr=
_temp114; _temp115.base= _temp114; _temp115.last_plus_one= _temp114 + 4;
_temp115;})); goto _LL101; _LL101:;}} void Cyc_Absyndump_dumptvars( struct Cyc_List_List*
tvs){(( void(*)( void(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l,
struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_egroup)( Cyc_Absyndump_dumptvar, tvs,( struct _tagged_string)({
char* _temp116=( char*)"<"; struct _tagged_string _temp117; _temp117.curr=
_temp116; _temp117.base= _temp116; _temp117.last_plus_one= _temp116 + 2;
_temp117;}),( struct _tagged_string)({ char* _temp118=( char*)">"; struct
_tagged_string _temp119; _temp119.curr= _temp118; _temp119.base= _temp118;
_temp119.last_plus_one= _temp118 + 2; _temp119;}),( struct _tagged_string)({
char* _temp120=( char*)","; struct _tagged_string _temp121; _temp121.curr=
_temp120; _temp121.base= _temp120; _temp121.last_plus_one= _temp120 + 2;
_temp121;}));} void Cyc_Absyndump_dumpkindedtvars( struct Cyc_List_List* tvs){((
void(*)( void(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumpkindedtvar, tvs,( struct _tagged_string)({ char* _temp122=(
char*)"<"; struct _tagged_string _temp123; _temp123.curr= _temp122; _temp123.base=
_temp122; _temp123.last_plus_one= _temp122 + 2; _temp123;}),( struct
_tagged_string)({ char* _temp124=( char*)">"; struct _tagged_string _temp125;
_temp125.curr= _temp124; _temp125.base= _temp124; _temp125.last_plus_one=
_temp124 + 2; _temp125;}),( struct _tagged_string)({ char* _temp126=( char*)",";
struct _tagged_string _temp127; _temp127.curr= _temp126; _temp127.base= _temp126;
_temp127.last_plus_one= _temp126 + 2; _temp127;}));} struct _tuple5{ struct Cyc_Absyn_Tqual
f1; void* f2; } ; void Cyc_Absyndump_dumparg( struct _tuple5* pr){(( void(*)(
struct Cyc_Absyn_Tqual, void*, void(* f)( int), int)) Cyc_Absyndump_dumptqtd)((*
pr).f1,(* pr).f2,( void(*)( int x)) Cyc_Absyndump_ignore, 0);} void Cyc_Absyndump_dumpargs(
struct Cyc_List_List* ts){(( void(*)( void(* f)( struct _tuple5*), struct Cyc_List_List*
l, struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumparg, ts,( struct _tagged_string)({
char* _temp128=( char*)"("; struct _tagged_string _temp129; _temp129.curr=
_temp128; _temp129.base= _temp128; _temp129.last_plus_one= _temp128 + 2;
_temp129;}),( struct _tagged_string)({ char* _temp130=( char*)")"; struct
_tagged_string _temp131; _temp131.curr= _temp130; _temp131.base= _temp130;
_temp131.last_plus_one= _temp130 + 2; _temp131;}),( struct _tagged_string)({
char* _temp132=( char*)","; struct _tagged_string _temp133; _temp133.curr=
_temp132; _temp133.base= _temp132; _temp133.last_plus_one= _temp132 + 2;
_temp133;}));} void Cyc_Absyndump_dumpatts( struct Cyc_List_List* atts){ if(
atts == 0){ return;} Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp134=( char*)" __attribute__(("; struct _tagged_string _temp135; _temp135.curr=
_temp134; _temp135.base= _temp134; _temp135.last_plus_one= _temp134 + 17;
_temp135;})); for( 0; atts != 0; atts=({ struct Cyc_List_List* _temp136= atts;
if( _temp136 == 0){ _throw( Null_Exception);} _temp136->tl;})){ Cyc_Absyndump_dump(
Cyc_Absyn_attribute2string(( void*)({ struct Cyc_List_List* _temp137= atts; if(
_temp137 == 0){ _throw( Null_Exception);} _temp137->hd;}))); if(({ struct Cyc_List_List*
_temp138= atts; if( _temp138 == 0){ _throw( Null_Exception);} _temp138->tl;}) !=
0){ Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp139=( char*)",";
struct _tagged_string _temp140; _temp140.curr= _temp139; _temp140.base= _temp139;
_temp140.last_plus_one= _temp139 + 2; _temp140;}));}} Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp141=( char*)")) "; struct _tagged_string
_temp142; _temp142.curr= _temp141; _temp142.base= _temp141; _temp142.last_plus_one=
_temp141 + 4; _temp142;}));} int Cyc_Absyndump_next_is_pointer( struct Cyc_List_List*
tms){ if( tms == 0){ return 0;}{ void* _temp144=( void*)({ struct Cyc_List_List*
_temp143= tms; if( _temp143 == 0){ _throw( Null_Exception);} _temp143->hd;});
struct Cyc_Absyn_Tqual _temp150; void* _temp152; void* _temp154; _LL146: if((
unsigned int) _temp144 > 1u?(( struct _tunion_struct*) _temp144)->tag == Cyc_Absyn_Pointer_mod_tag:
0){ _LL155: _temp154=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp144)->f1;
goto _LL153; _LL153: _temp152=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp144)->f2; goto _LL151; _LL151: _temp150=( struct Cyc_Absyn_Tqual)(( struct
Cyc_Absyn_Pointer_mod_struct*) _temp144)->f3; goto _LL147;} else{ goto _LL148;}
_LL148: goto _LL149; _LL147: return 1; _LL149: return 0; _LL145:;}} static void
Cyc_Absyndump_dumprgn( void* t){ void* _temp156= Cyc_Tcutil_compress( t); _LL158:
if(( int) _temp156 == Cyc_Absyn_HeapRgn){ goto _LL159;} else{ goto _LL160;}
_LL160: goto _LL161; _LL159: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp162=( char*)"`H"; struct _tagged_string _temp163; _temp163.curr= _temp162;
_temp163.base= _temp162; _temp163.last_plus_one= _temp162 + 3; _temp163;}));
goto _LL157; _LL161: Cyc_Absyndump_dumpntyp( t); goto _LL157; _LL157:;} struct
_tuple6{ struct Cyc_List_List* f1; struct Cyc_List_List* f2; } ; static struct
_tuple6 Cyc_Absyndump_effects_split( void* t){ struct Cyc_List_List* regions= 0;
struct Cyc_List_List* effects= 0;{ void* _temp164= Cyc_Tcutil_compress( t); void*
_temp172; struct Cyc_List_List* _temp174; _LL166: if(( unsigned int) _temp164 >
4u?(( struct _tunion_struct*) _temp164)->tag == Cyc_Absyn_AccessEff_tag: 0){
_LL173: _temp172=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp164)->f1;
goto _LL167;} else{ goto _LL168;} _LL168: if(( unsigned int) _temp164 > 4u?((
struct _tunion_struct*) _temp164)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL175:
_temp174=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp164)->f1;
goto _LL169;} else{ goto _LL170;} _LL170: goto _LL171; _LL167: regions=({ struct
Cyc_List_List* _temp176=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp176->hd=( void*) _temp172; _temp176->tl= regions; _temp176;}); goto _LL165;
_LL169: for( 0; _temp174 != 0; _temp174=({ struct Cyc_List_List* _temp177=
_temp174; if( _temp177 == 0){ _throw( Null_Exception);} _temp177->tl;})){ struct
Cyc_List_List* _temp181; struct Cyc_List_List* _temp183; struct _tuple6 _temp179=
Cyc_Absyndump_effects_split(( void*)({ struct Cyc_List_List* _temp178= _temp174;
if( _temp178 == 0){ _throw( Null_Exception);} _temp178->hd;})); _LL184: _temp183=
_temp179.f1; goto _LL182; _LL182: _temp181= _temp179.f2; goto _LL180; _LL180:
regions=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( _temp183, regions); effects=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
_temp181, effects);} goto _LL165; _LL171: effects=({ struct Cyc_List_List*
_temp185=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp185->hd=( void*) t; _temp185->tl= effects; _temp185;}); goto _LL165; _LL165:;}
return({ struct _tuple6 _temp186; _temp186.f1= regions; _temp186.f2= effects;
_temp186;});} static void Cyc_Absyndump_dumpeff( void* t){ struct Cyc_List_List*
_temp189; struct Cyc_List_List* _temp191; struct _tuple6 _temp187= Cyc_Absyndump_effects_split(
t); _LL192: _temp191= _temp187.f1; goto _LL190; _LL190: _temp189= _temp187.f2;
goto _LL188; _LL188: _temp191=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp191); _temp189=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( _temp189); for( 0; _temp189 != 0; _temp189=({
struct Cyc_List_List* _temp193= _temp189; if( _temp193 == 0){ _throw(
Null_Exception);} _temp193->tl;})){ Cyc_Absyndump_dumpntyp(( void*)({ struct Cyc_List_List*
_temp194= _temp189; if( _temp194 == 0){ _throw( Null_Exception);} _temp194->hd;}));
Cyc_Absyndump_dump_char(( int)'+');} Cyc_Absyndump_dump_char(( int)'{'); for( 0;
_temp191 != 0; _temp191=({ struct Cyc_List_List* _temp195= _temp191; if(
_temp195 == 0){ _throw( Null_Exception);} _temp195->tl;})){ Cyc_Absyndump_dumprgn((
void*)({ struct Cyc_List_List* _temp196= _temp191; if( _temp196 == 0){ _throw(
Null_Exception);} _temp196->hd;})); if(({ struct Cyc_List_List* _temp197=
_temp191; if( _temp197 == 0){ _throw( Null_Exception);} _temp197->tl;}) != 0){
Cyc_Absyndump_dump_char(( int)',');}} Cyc_Absyndump_dump_char(( int)'}');} void
Cyc_Absyndump_dumpntyp( void* t){ void* _temp198= t; struct Cyc_Absyn_Exp*
_temp264; struct Cyc_Absyn_Tqual _temp266; void* _temp268; struct Cyc_Absyn_FnInfo
_temp270; struct Cyc_Absyn_PtrInfo _temp272; struct Cyc_Absyn_Tvar* _temp274;
int _temp276; struct Cyc_Core_Opt* _temp278; void* _temp280; int _temp282;
struct Cyc_Core_Opt* _temp284; struct Cyc_Core_Opt _temp286; void* _temp287;
void* _temp289; struct Cyc_Absyn_TunionInfo _temp291; struct Cyc_Absyn_Tuniondecl*
_temp293; void* _temp295; struct Cyc_List_List* _temp297; struct _tuple0*
_temp299; struct Cyc_Absyn_XTunionInfo _temp301; struct Cyc_Absyn_XTuniondecl*
_temp303; void* _temp305; struct _tuple0* _temp307; struct Cyc_Absyn_TunionFieldInfo
_temp309; struct Cyc_Absyn_Tunionfield* _temp311; struct Cyc_Absyn_Tuniondecl*
_temp313; struct _tuple0* _temp315; struct Cyc_List_List* _temp317; struct
_tuple0* _temp319; struct Cyc_Absyn_XTunionFieldInfo _temp321; struct Cyc_Absyn_Tunionfield*
_temp323; struct Cyc_Absyn_XTuniondecl* _temp325; struct _tuple0* _temp327;
struct _tuple0* _temp329; struct Cyc_Absyn_Enumdecl* _temp331; struct _tuple0*
_temp333; void* _temp335; void* _temp337; void* _temp339; void* _temp341; void*
_temp343; void* _temp345; void* _temp347; void* _temp349; void* _temp351; void*
_temp353; void* _temp355; void* _temp357; void* _temp359; void* _temp361; void*
_temp363; void* _temp365; struct Cyc_List_List* _temp367; struct Cyc_Absyn_Structdecl**
_temp369; struct Cyc_List_List* _temp371; struct _tuple0* _temp373; struct Cyc_Absyn_Structdecl**
_temp375; struct Cyc_List_List* _temp377; struct _tuple0* _temp379; struct Cyc_Absyn_Uniondecl**
_temp381; struct Cyc_List_List* _temp383; struct _tuple0* _temp385; struct Cyc_Absyn_Uniondecl**
_temp387; struct Cyc_List_List* _temp389; struct _tuple0* _temp391; struct Cyc_Core_Opt*
_temp393; struct Cyc_List_List* _temp395; struct _tuple0* _temp397; void*
_temp399; void* _temp401; struct Cyc_List_List* _temp403; _LL200: if((
unsigned int) _temp198 > 4u?(( struct _tunion_struct*) _temp198)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL269: _temp268=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp198)->f1;
goto _LL267; _LL267: _temp266=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp198)->f2; goto _LL265; _LL265: _temp264=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp198)->f3; goto _LL201;} else{ goto _LL202;}
_LL202: if(( unsigned int) _temp198 > 4u?(( struct _tunion_struct*) _temp198)->tag
== Cyc_Absyn_FnType_tag: 0){ _LL271: _temp270=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp198)->f1; goto _LL203;} else{ goto _LL204;}
_LL204: if(( unsigned int) _temp198 > 4u?(( struct _tunion_struct*) _temp198)->tag
== Cyc_Absyn_PointerType_tag: 0){ _LL273: _temp272=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp198)->f1; goto _LL205;} else{ goto
_LL206;} _LL206: if(( int) _temp198 == Cyc_Absyn_VoidType){ goto _LL207;} else{
goto _LL208;} _LL208: if(( unsigned int) _temp198 > 4u?(( struct _tunion_struct*)
_temp198)->tag == Cyc_Absyn_VarType_tag: 0){ _LL275: _temp274=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_VarType_struct*) _temp198)->f1; goto _LL209;} else{ goto _LL210;}
_LL210: if(( unsigned int) _temp198 > 4u?(( struct _tunion_struct*) _temp198)->tag
== Cyc_Absyn_Evar_tag: 0){ _LL281: _temp280=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp198)->f1; goto _LL279; _LL279: _temp278=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Evar_struct*) _temp198)->f2; if( _temp278 == 0){ goto _LL277;} else{
goto _LL212;} _LL277: _temp276=( int)(( struct Cyc_Absyn_Evar_struct*) _temp198)->f3;
goto _LL211;} else{ goto _LL212;} _LL212: if(( unsigned int) _temp198 > 4u?((
struct _tunion_struct*) _temp198)->tag == Cyc_Absyn_Evar_tag: 0){ _LL290:
_temp289=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp198)->f1; goto _LL285;
_LL285: _temp284=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp198)->f2; if( _temp284 == 0){ goto _LL214;} else{ _temp286=* _temp284;
_LL288: _temp287=( void*) _temp286.v; goto _LL283;} _LL283: _temp282=( int)((
struct Cyc_Absyn_Evar_struct*) _temp198)->f3; goto _LL213;} else{ goto _LL214;}
_LL214: if(( unsigned int) _temp198 > 4u?(( struct _tunion_struct*) _temp198)->tag
== Cyc_Absyn_TunionType_tag: 0){ _LL292: _temp291=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp198)->f1; _LL300: _temp299=( struct
_tuple0*) _temp291.name; goto _LL298; _LL298: _temp297=( struct Cyc_List_List*)
_temp291.targs; goto _LL296; _LL296: _temp295=( void*) _temp291.rgn; goto _LL294;
_LL294: _temp293=( struct Cyc_Absyn_Tuniondecl*) _temp291.tud; goto _LL215;}
else{ goto _LL216;} _LL216: if(( unsigned int) _temp198 > 4u?(( struct
_tunion_struct*) _temp198)->tag == Cyc_Absyn_XTunionType_tag: 0){ _LL302:
_temp301=( struct Cyc_Absyn_XTunionInfo)(( struct Cyc_Absyn_XTunionType_struct*)
_temp198)->f1; _LL308: _temp307=( struct _tuple0*) _temp301.name; goto _LL306;
_LL306: _temp305=( void*) _temp301.rgn; goto _LL304; _LL304: _temp303=( struct
Cyc_Absyn_XTuniondecl*) _temp301.xtud; goto _LL217;} else{ goto _LL218;} _LL218:
if(( unsigned int) _temp198 > 4u?(( struct _tunion_struct*) _temp198)->tag ==
Cyc_Absyn_TunionFieldType_tag: 0){ _LL310: _temp309=( struct Cyc_Absyn_TunionFieldInfo)((
struct Cyc_Absyn_TunionFieldType_struct*) _temp198)->f1; _LL320: _temp319=(
struct _tuple0*) _temp309.name; goto _LL318; _LL318: _temp317=( struct Cyc_List_List*)
_temp309.targs; goto _LL316; _LL316: _temp315=( struct _tuple0*) _temp309.fname;
goto _LL314; _LL314: _temp313=( struct Cyc_Absyn_Tuniondecl*) _temp309.tud; goto
_LL312; _LL312: _temp311=( struct Cyc_Absyn_Tunionfield*) _temp309.tufd; goto
_LL219;} else{ goto _LL220;} _LL220: if(( unsigned int) _temp198 > 4u?(( struct
_tunion_struct*) _temp198)->tag == Cyc_Absyn_XTunionFieldType_tag: 0){ _LL322:
_temp321=( struct Cyc_Absyn_XTunionFieldInfo)(( struct Cyc_Absyn_XTunionFieldType_struct*)
_temp198)->f1; _LL330: _temp329=( struct _tuple0*) _temp321.name; goto _LL328;
_LL328: _temp327=( struct _tuple0*) _temp321.fname; goto _LL326; _LL326:
_temp325=( struct Cyc_Absyn_XTuniondecl*) _temp321.xtud; goto _LL324; _LL324:
_temp323=( struct Cyc_Absyn_Tunionfield*) _temp321.xtufd; goto _LL221;} else{
goto _LL222;} _LL222: if(( unsigned int) _temp198 > 4u?(( struct _tunion_struct*)
_temp198)->tag == Cyc_Absyn_EnumType_tag: 0){ _LL334: _temp333=( struct _tuple0*)((
struct Cyc_Absyn_EnumType_struct*) _temp198)->f1; goto _LL332; _LL332: _temp331=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*) _temp198)->f2;
goto _LL223;} else{ goto _LL224;} _LL224: if(( unsigned int) _temp198 > 4u?((
struct _tunion_struct*) _temp198)->tag == Cyc_Absyn_IntType_tag: 0){ _LL338:
_temp337=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp198)->f1; if(( int)
_temp337 == Cyc_Absyn_Signed){ goto _LL336;} else{ goto _LL226;} _LL336:
_temp335=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp198)->f2; if(( int)
_temp335 == Cyc_Absyn_B1){ goto _LL225;} else{ goto _LL226;}} else{ goto _LL226;}
_LL226: if(( unsigned int) _temp198 > 4u?(( struct _tunion_struct*) _temp198)->tag
== Cyc_Absyn_IntType_tag: 0){ _LL342: _temp341=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp198)->f1; if(( int) _temp341 == Cyc_Absyn_Unsigned){ goto _LL340;} else{
goto _LL228;} _LL340: _temp339=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp198)->f2; if(( int) _temp339 == Cyc_Absyn_B1){ goto _LL227;} else{ goto
_LL228;}} else{ goto _LL228;} _LL228: if(( unsigned int) _temp198 > 4u?(( struct
_tunion_struct*) _temp198)->tag == Cyc_Absyn_IntType_tag: 0){ _LL346: _temp345=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp198)->f1; if(( int) _temp345 ==
Cyc_Absyn_Signed){ goto _LL344;} else{ goto _LL230;} _LL344: _temp343=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp198)->f2; if(( int) _temp343 == Cyc_Absyn_B2){
goto _LL229;} else{ goto _LL230;}} else{ goto _LL230;} _LL230: if(( unsigned int)
_temp198 > 4u?(( struct _tunion_struct*) _temp198)->tag == Cyc_Absyn_IntType_tag:
0){ _LL350: _temp349=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp198)->f1;
if(( int) _temp349 == Cyc_Absyn_Unsigned){ goto _LL348;} else{ goto _LL232;}
_LL348: _temp347=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp198)->f2; if((
int) _temp347 == Cyc_Absyn_B2){ goto _LL231;} else{ goto _LL232;}} else{ goto
_LL232;} _LL232: if(( unsigned int) _temp198 > 4u?(( struct _tunion_struct*)
_temp198)->tag == Cyc_Absyn_IntType_tag: 0){ _LL354: _temp353=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp198)->f1; if(( int) _temp353 == Cyc_Absyn_Signed){
goto _LL352;} else{ goto _LL234;} _LL352: _temp351=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp198)->f2; if(( int) _temp351 == Cyc_Absyn_B4){ goto _LL233;} else{ goto
_LL234;}} else{ goto _LL234;} _LL234: if(( unsigned int) _temp198 > 4u?(( struct
_tunion_struct*) _temp198)->tag == Cyc_Absyn_IntType_tag: 0){ _LL358: _temp357=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp198)->f1; if(( int) _temp357 ==
Cyc_Absyn_Unsigned){ goto _LL356;} else{ goto _LL236;} _LL356: _temp355=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp198)->f2; if(( int) _temp355 == Cyc_Absyn_B4){
goto _LL235;} else{ goto _LL236;}} else{ goto _LL236;} _LL236: if(( unsigned int)
_temp198 > 4u?(( struct _tunion_struct*) _temp198)->tag == Cyc_Absyn_IntType_tag:
0){ _LL362: _temp361=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp198)->f1;
if(( int) _temp361 == Cyc_Absyn_Signed){ goto _LL360;} else{ goto _LL238;}
_LL360: _temp359=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp198)->f2; if((
int) _temp359 == Cyc_Absyn_B8){ goto _LL237;} else{ goto _LL238;}} else{ goto
_LL238;} _LL238: if(( unsigned int) _temp198 > 4u?(( struct _tunion_struct*)
_temp198)->tag == Cyc_Absyn_IntType_tag: 0){ _LL366: _temp365=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp198)->f1; if(( int) _temp365 == Cyc_Absyn_Unsigned){
goto _LL364;} else{ goto _LL240;} _LL364: _temp363=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp198)->f2; if(( int) _temp363 == Cyc_Absyn_B8){ goto _LL239;} else{ goto
_LL240;}} else{ goto _LL240;} _LL240: if(( int) _temp198 == Cyc_Absyn_FloatType){
goto _LL241;} else{ goto _LL242;} _LL242: if(( int) _temp198 == Cyc_Absyn_DoubleType){
goto _LL243;} else{ goto _LL244;} _LL244: if(( unsigned int) _temp198 > 4u?((
struct _tunion_struct*) _temp198)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL368:
_temp367=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp198)->f1;
goto _LL245;} else{ goto _LL246;} _LL246: if(( unsigned int) _temp198 > 4u?((
struct _tunion_struct*) _temp198)->tag == Cyc_Absyn_StructType_tag: 0){ _LL374:
_temp373=( struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*) _temp198)->f1;
if( _temp373 == 0){ goto _LL372;} else{ goto _LL248;} _LL372: _temp371=( struct
Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*) _temp198)->f2; goto
_LL370; _LL370: _temp369=( struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*)
_temp198)->f3; goto _LL247;} else{ goto _LL248;} _LL248: if(( unsigned int)
_temp198 > 4u?(( struct _tunion_struct*) _temp198)->tag == Cyc_Absyn_StructType_tag:
0){ _LL380: _temp379=( struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*)
_temp198)->f1; goto _LL378; _LL378: _temp377=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp198)->f2; goto _LL376; _LL376: _temp375=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp198)->f3;
goto _LL249;} else{ goto _LL250;} _LL250: if(( unsigned int) _temp198 > 4u?((
struct _tunion_struct*) _temp198)->tag == Cyc_Absyn_UnionType_tag: 0){ _LL386:
_temp385=( struct _tuple0*)(( struct Cyc_Absyn_UnionType_struct*) _temp198)->f1;
if( _temp385 == 0){ goto _LL384;} else{ goto _LL252;} _LL384: _temp383=( struct
Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*) _temp198)->f2; goto _LL382;
_LL382: _temp381=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp198)->f3; goto _LL251;} else{ goto _LL252;} _LL252: if(( unsigned int)
_temp198 > 4u?(( struct _tunion_struct*) _temp198)->tag == Cyc_Absyn_UnionType_tag:
0){ _LL392: _temp391=( struct _tuple0*)(( struct Cyc_Absyn_UnionType_struct*)
_temp198)->f1; goto _LL390; _LL390: _temp389=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp198)->f2; goto _LL388; _LL388: _temp387=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp198)->f3;
goto _LL253;} else{ goto _LL254;} _LL254: if(( unsigned int) _temp198 > 4u?((
struct _tunion_struct*) _temp198)->tag == Cyc_Absyn_TypedefType_tag: 0){ _LL398:
_temp397=( struct _tuple0*)(( struct Cyc_Absyn_TypedefType_struct*) _temp198)->f1;
goto _LL396; _LL396: _temp395=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp198)->f2; goto _LL394; _LL394: _temp393=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_TypedefType_struct*) _temp198)->f3; goto _LL255;} else{ goto _LL256;}
_LL256: if(( unsigned int) _temp198 > 4u?(( struct _tunion_struct*) _temp198)->tag
== Cyc_Absyn_RgnHandleType_tag: 0){ _LL400: _temp399=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp198)->f1; goto _LL257;} else{ goto _LL258;} _LL258: if(( int) _temp198 ==
Cyc_Absyn_HeapRgn){ goto _LL259;} else{ goto _LL260;} _LL260: if(( unsigned int)
_temp198 > 4u?(( struct _tunion_struct*) _temp198)->tag == Cyc_Absyn_AccessEff_tag:
0){ _LL402: _temp401=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp198)->f1;
goto _LL261;} else{ goto _LL262;} _LL262: if(( unsigned int) _temp198 > 4u?((
struct _tunion_struct*) _temp198)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL404:
_temp403=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp198)->f1;
goto _LL263;} else{ goto _LL199;} _LL201: return; _LL203: return; _LL205:
return; _LL207: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp405=(
char*)"void"; struct _tagged_string _temp406; _temp406.curr= _temp405; _temp406.base=
_temp405; _temp406.last_plus_one= _temp405 + 5; _temp406;})); return; _LL209:
Cyc_Absyndump_dump_str( _temp274->name); return; _LL211: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp407=( char*)"%"; struct _tagged_string
_temp408; _temp408.curr= _temp407; _temp408.base= _temp407; _temp408.last_plus_one=
_temp407 + 2; _temp408;})); Cyc_Absyndump_dumpkind( _temp280); Cyc_Absyndump_dump(
xprintf("(%d)", _temp276)); return; _LL213: Cyc_Absyndump_dumpntyp( _temp287);
return; _LL215: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp409=(
char*)"tunion "; struct _tagged_string _temp410; _temp410.curr= _temp409;
_temp410.base= _temp409; _temp410.last_plus_one= _temp409 + 8; _temp410;}));{
void* _temp411= Cyc_Tcutil_compress( _temp295); _LL413: if(( int) _temp411 ==
Cyc_Absyn_HeapRgn){ goto _LL414;} else{ goto _LL415;} _LL415: goto _LL416;
_LL414: goto _LL412; _LL416: Cyc_Absyndump_dumptyp( _temp295); Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp417=( char*)" "; struct _tagged_string
_temp418; _temp418.curr= _temp417; _temp418.base= _temp417; _temp418.last_plus_one=
_temp417 + 2; _temp418;})); goto _LL412; _LL412:;} Cyc_Absyndump_dumpqvar(
_temp299); Cyc_Absyndump_dumptps( _temp297); goto _LL199; _LL217: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp419=( char*)"xtunion "; struct
_tagged_string _temp420; _temp420.curr= _temp419; _temp420.base= _temp419;
_temp420.last_plus_one= _temp419 + 9; _temp420;}));{ void* _temp421= Cyc_Tcutil_compress(
_temp305); _LL423: if(( int) _temp421 == Cyc_Absyn_HeapRgn){ goto _LL424;} else{
goto _LL425;} _LL425: goto _LL426; _LL424: goto _LL422; _LL426: Cyc_Absyndump_dumptyp(
_temp305); Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp427=( char*)" ";
struct _tagged_string _temp428; _temp428.curr= _temp427; _temp428.base= _temp427;
_temp428.last_plus_one= _temp427 + 2; _temp428;})); goto _LL422; _LL422:;} Cyc_Absyndump_dumpqvar(
_temp307); goto _LL199; _LL219: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp429=( char*)"tunion "; struct _tagged_string _temp430; _temp430.curr=
_temp429; _temp430.base= _temp429; _temp430.last_plus_one= _temp429 + 8;
_temp430;})); Cyc_Absyndump_dumpqvar( _temp319); Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp431=( char*)"."; struct _tagged_string _temp432;
_temp432.curr= _temp431; _temp432.base= _temp431; _temp432.last_plus_one=
_temp431 + 2; _temp432;})); Cyc_Absyndump_dumpqvar( _temp315); Cyc_Absyndump_dumptps(
_temp317); goto _LL199; _LL221: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp433=( char*)"xtunion "; struct _tagged_string _temp434; _temp434.curr=
_temp433; _temp434.base= _temp433; _temp434.last_plus_one= _temp433 + 9;
_temp434;})); Cyc_Absyndump_dumpqvar( _temp329); Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp435=( char*)"."; struct _tagged_string _temp436;
_temp436.curr= _temp435; _temp436.base= _temp435; _temp436.last_plus_one=
_temp435 + 2; _temp436;})); Cyc_Absyndump_dumpqvar( _temp327); goto _LL199;
_LL223: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp437=( char*)"enum ";
struct _tagged_string _temp438; _temp438.curr= _temp437; _temp438.base= _temp437;
_temp438.last_plus_one= _temp437 + 6; _temp438;})); Cyc_Absyndump_dumpqvar(
_temp333); return; _LL225: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp439=( char*)"signed char"; struct _tagged_string _temp440; _temp440.curr=
_temp439; _temp440.base= _temp439; _temp440.last_plus_one= _temp439 + 12;
_temp440;})); return; _LL227: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp441=( char*)"char"; struct _tagged_string _temp442; _temp442.curr= _temp441;
_temp442.base= _temp441; _temp442.last_plus_one= _temp441 + 5; _temp442;}));
return; _LL229: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp443=(
char*)"short"; struct _tagged_string _temp444; _temp444.curr= _temp443; _temp444.base=
_temp443; _temp444.last_plus_one= _temp443 + 6; _temp444;})); return; _LL231:
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp445=( char*)"unsigned short";
struct _tagged_string _temp446; _temp446.curr= _temp445; _temp446.base= _temp445;
_temp446.last_plus_one= _temp445 + 15; _temp446;})); return; _LL233: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp447=( char*)"int"; struct _tagged_string
_temp448; _temp448.curr= _temp447; _temp448.base= _temp447; _temp448.last_plus_one=
_temp447 + 4; _temp448;})); return; _LL235: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp449=( char*)"unsigned int"; struct _tagged_string
_temp450; _temp450.curr= _temp449; _temp450.base= _temp449; _temp450.last_plus_one=
_temp449 + 13; _temp450;})); return; _LL237: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp451=( char*)"long long"; struct _tagged_string
_temp452; _temp452.curr= _temp451; _temp452.base= _temp451; _temp452.last_plus_one=
_temp451 + 10; _temp452;})); return; _LL239: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp453=( char*)"unsigned long long"; struct
_tagged_string _temp454; _temp454.curr= _temp453; _temp454.base= _temp453;
_temp454.last_plus_one= _temp453 + 19; _temp454;})); return; _LL241: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp455=( char*)"float"; struct _tagged_string
_temp456; _temp456.curr= _temp455; _temp456.base= _temp455; _temp456.last_plus_one=
_temp455 + 6; _temp456;})); return; _LL243: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp457=( char*)"double"; struct _tagged_string
_temp458; _temp458.curr= _temp457; _temp458.base= _temp457; _temp458.last_plus_one=
_temp457 + 7; _temp458;})); return; _LL245: Cyc_Absyndump_dump_char(( int)'$');
Cyc_Absyndump_dumpargs( _temp367); return; _LL247: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp459=( char*)"struct"; struct _tagged_string
_temp460; _temp460.curr= _temp459; _temp460.base= _temp459; _temp460.last_plus_one=
_temp459 + 7; _temp460;})); Cyc_Absyndump_dumptps( _temp371); return; _LL249:
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp461=( char*)"struct";
struct _tagged_string _temp462; _temp462.curr= _temp461; _temp462.base= _temp461;
_temp462.last_plus_one= _temp461 + 7; _temp462;})); Cyc_Absyndump_dumpqvar((
struct _tuple0*)({ struct _tuple0* _temp463= _temp379; if( _temp463 == 0){
_throw( Null_Exception);} _temp463;})); Cyc_Absyndump_dumptps( _temp377);
return; _LL251: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp464=(
char*)"union"; struct _tagged_string _temp465; _temp465.curr= _temp464; _temp465.base=
_temp464; _temp465.last_plus_one= _temp464 + 6; _temp465;})); Cyc_Absyndump_dumptps(
_temp383); return; _LL253: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp466=( char*)"union"; struct _tagged_string _temp467; _temp467.curr=
_temp466; _temp467.base= _temp466; _temp467.last_plus_one= _temp466 + 6;
_temp467;})); Cyc_Absyndump_dumpqvar(( struct _tuple0*)({ struct _tuple0*
_temp468= _temp391; if( _temp468 == 0){ _throw( Null_Exception);} _temp468;}));
Cyc_Absyndump_dumptps( _temp389); return; _LL255: Cyc_Absyndump_dumpqvar(
_temp397), Cyc_Absyndump_dumptps( _temp395); return; _LL257: Cyc_Absyndump_dumprgn(
_temp399); return; _LL259: return; _LL261: return; _LL263: return; _LL199:;}
void Cyc_Absyndump_dumpvaropt( struct Cyc_Core_Opt* vo){ if( vo != 0){ Cyc_Absyndump_dump_str((
struct _tagged_string*)({ struct Cyc_Core_Opt* _temp469= vo; if( _temp469 == 0){
_throw( Null_Exception);} _temp469->v;}));}} void Cyc_Absyndump_dumpfunarg(
struct _tuple1* t){(( void(*)( struct Cyc_Absyn_Tqual, void*, void(* f)( struct
Cyc_Core_Opt*), struct Cyc_Core_Opt*)) Cyc_Absyndump_dumptqtd)((* t).f2,(* t).f3,
Cyc_Absyndump_dumpvaropt,(* t).f1);} void Cyc_Absyndump_dumpfunargs( struct Cyc_List_List*
args, int varargs, struct Cyc_Core_Opt* effopt){ Cyc_Absyndump_dump_char(( int)'(');
for( 0; args != 0; args=({ struct Cyc_List_List* _temp470= args; if( _temp470 ==
0){ _throw( Null_Exception);} _temp470->tl;})){ Cyc_Absyndump_dumpfunarg((
struct _tuple1*)({ struct Cyc_List_List* _temp471= args; if( _temp471 == 0){
_throw( Null_Exception);} _temp471->hd;})); if(({ struct Cyc_List_List* _temp472=
args; if( _temp472 == 0){ _throw( Null_Exception);} _temp472->tl;}) != 0? 1:
varargs){ Cyc_Absyndump_dump_char(( int)',');}} if( varargs){ Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp473=( char*)"..."; struct _tagged_string
_temp474; _temp474.curr= _temp473; _temp474.base= _temp473; _temp474.last_plus_one=
_temp473 + 4; _temp474;}));} if( effopt != 0){ Cyc_Absyndump_dump_semi(); Cyc_Absyndump_dumpeff((
void*)({ struct Cyc_Core_Opt* _temp475= effopt; if( _temp475 == 0){ _throw(
Null_Exception);} _temp475->v;}));} Cyc_Absyndump_dump_char(( int)')');} void
Cyc_Absyndump_dumptyp( void* t){(( void(*)( struct Cyc_Absyn_Tqual, void*, void(*
f)( int), int)) Cyc_Absyndump_dumptqtd)(({ struct Cyc_Absyn_Tqual _temp476;
_temp476.q_const= 0; _temp476.q_volatile= 0; _temp476.q_restrict= 0; _temp476;}),
t,( void(*)( int x)) Cyc_Absyndump_ignore, 0);} void Cyc_Absyndump_dumpdesignator(
void* d){ void* _temp477= d; struct Cyc_Absyn_Exp* _temp483; struct
_tagged_string* _temp485; _LL479: if((( struct _tunion_struct*) _temp477)->tag
== Cyc_Absyn_ArrayElement_tag){ _LL484: _temp483=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayElement_struct*) _temp477)->f1; goto _LL480;} else{ goto
_LL481;} _LL481: if((( struct _tunion_struct*) _temp477)->tag == Cyc_Absyn_FieldName_tag){
_LL486: _temp485=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp477)->f1; goto _LL482;} else{ goto _LL478;} _LL480: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp487=( char*)".["; struct _tagged_string
_temp488; _temp488.curr= _temp487; _temp488.base= _temp487; _temp488.last_plus_one=
_temp487 + 3; _temp488;})); Cyc_Absyndump_dumpexp( _temp483); Cyc_Absyndump_dump_char((
int)']'); goto _LL478; _LL482: Cyc_Absyndump_dump_char(( int)'.'); Cyc_Absyndump_dump_nospace(*
_temp485); goto _LL478; _LL478:;} struct _tuple7{ struct Cyc_List_List* f1;
struct Cyc_Absyn_Exp* f2; } ; void Cyc_Absyndump_dumpde( struct _tuple7* de){((
void(*)( void(* f)( void*), struct Cyc_List_List* l, struct _tagged_string start,
struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumpdesignator,(* de).f1,( struct _tagged_string)({ char* _temp489=(
char*)""; struct _tagged_string _temp490; _temp490.curr= _temp489; _temp490.base=
_temp489; _temp490.last_plus_one= _temp489 + 1; _temp490;}),( struct
_tagged_string)({ char* _temp491=( char*)"="; struct _tagged_string _temp492;
_temp492.curr= _temp491; _temp492.base= _temp491; _temp492.last_plus_one=
_temp491 + 2; _temp492;}),( struct _tagged_string)({ char* _temp493=( char*)"=";
struct _tagged_string _temp494; _temp494.curr= _temp493; _temp494.base= _temp493;
_temp494.last_plus_one= _temp493 + 2; _temp494;})); Cyc_Absyndump_dumpexp((* de).f2);}
void Cyc_Absyndump_dumpexps_prec( int inprec, struct Cyc_List_List* es){(( void(*)(
void(* f)( int, struct Cyc_Absyn_Exp*), int env, struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group_c)(
Cyc_Absyndump_dumpexp_prec, inprec, es,( struct _tagged_string)({ char* _temp495=(
char*)""; struct _tagged_string _temp496; _temp496.curr= _temp495; _temp496.base=
_temp495; _temp496.last_plus_one= _temp495 + 1; _temp496;}),( struct
_tagged_string)({ char* _temp497=( char*)""; struct _tagged_string _temp498;
_temp498.curr= _temp497; _temp498.base= _temp497; _temp498.last_plus_one=
_temp497 + 1; _temp498;}),( struct _tagged_string)({ char* _temp499=( char*)",";
struct _tagged_string _temp500; _temp500.curr= _temp499; _temp500.base= _temp499;
_temp500.last_plus_one= _temp499 + 2; _temp500;}));} void Cyc_Absyndump_dumpexp_prec(
int inprec, struct Cyc_Absyn_Exp* e){ int myprec= Cyc_Absynpp_exp_prec( e); if(
inprec >= myprec){ Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char*
_temp501=( char*)"("; struct _tagged_string _temp502; _temp502.curr= _temp501;
_temp502.base= _temp501; _temp502.last_plus_one= _temp501 + 2; _temp502;}));}{
void* _temp503=( void*) e->r; void* _temp595; char _temp597; void* _temp599;
void* _temp601; short _temp603; void* _temp605; void* _temp607; int _temp609;
void* _temp611; void* _temp613; int _temp615; void* _temp617; void* _temp619;
long long _temp621; void* _temp623; void* _temp625; struct _tagged_string
_temp627; void* _temp629; void* _temp631; struct _tagged_string _temp633; struct
_tuple0* _temp635; void* _temp637; struct _tuple0* _temp639; struct Cyc_List_List*
_temp641; void* _temp643; struct Cyc_Absyn_Exp* _temp645; struct Cyc_Core_Opt*
_temp647; struct Cyc_Absyn_Exp* _temp649; void* _temp651; struct Cyc_Absyn_Exp*
_temp653; void* _temp655; struct Cyc_Absyn_Exp* _temp657; void* _temp659; struct
Cyc_Absyn_Exp* _temp661; void* _temp663; struct Cyc_Absyn_Exp* _temp665; struct
Cyc_Absyn_Exp* _temp667; struct Cyc_Absyn_Exp* _temp669; struct Cyc_Absyn_Exp*
_temp671; struct Cyc_Absyn_Exp* _temp673; struct Cyc_Absyn_Exp* _temp675; struct
Cyc_List_List* _temp677; struct Cyc_Absyn_Exp* _temp679; struct Cyc_List_List*
_temp681; struct Cyc_Absyn_Exp* _temp683; struct Cyc_Absyn_Exp* _temp685; struct
Cyc_Absyn_Exp* _temp687; struct Cyc_List_List* _temp689; struct Cyc_Absyn_Exp*
_temp691; struct Cyc_Absyn_Exp* _temp693; void* _temp695; struct Cyc_Absyn_Exp*
_temp697; struct Cyc_Absyn_Exp* _temp699; struct Cyc_Absyn_Exp* _temp701; void*
_temp703; struct Cyc_Absyn_Exp* _temp705; struct Cyc_Absyn_Exp* _temp707; struct
_tagged_string* _temp709; struct Cyc_Absyn_Exp* _temp711; struct _tagged_string*
_temp713; struct Cyc_Absyn_Exp* _temp715; struct Cyc_Absyn_Exp* _temp717; struct
Cyc_Absyn_Exp* _temp719; struct Cyc_List_List* _temp721; struct Cyc_List_List*
_temp723; struct _tuple1* _temp725; struct Cyc_List_List* _temp727; struct Cyc_Absyn_Exp*
_temp729; struct Cyc_Absyn_Exp* _temp731; struct Cyc_Absyn_Vardecl* _temp733;
struct Cyc_Absyn_Structdecl* _temp735; struct Cyc_List_List* _temp737; struct
Cyc_Core_Opt* _temp739; struct _tuple0* _temp741; struct Cyc_Absyn_Tunionfield*
_temp743; struct Cyc_Absyn_Tuniondecl* _temp745; struct Cyc_List_List* _temp747;
struct Cyc_Core_Opt* _temp749; struct Cyc_Core_Opt* _temp751; struct Cyc_Absyn_Tunionfield*
_temp753; struct Cyc_Absyn_XTuniondecl* _temp755; struct Cyc_List_List* _temp757;
struct Cyc_Core_Opt* _temp759; struct Cyc_Absyn_Enumfield* _temp761; struct Cyc_Absyn_Enumdecl*
_temp763; struct _tuple0* _temp765; void* _temp767; struct Cyc_Absyn_Exp*
_temp769; struct Cyc_List_List* _temp771; struct Cyc_Core_Opt* _temp773; struct
Cyc_Absyn_Stmt* _temp775; struct Cyc_Absyn_Fndecl* _temp777; struct Cyc_Absyn_Exp*
_temp779; _LL505: if((( struct _tunion_struct*) _temp503)->tag == Cyc_Absyn_Const_e_tag){
_LL596: _temp595=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp503)->f1; if((
unsigned int) _temp595 > 1u?(( struct _tunion_struct*) _temp595)->tag == Cyc_Absyn_Char_c_tag:
0){ _LL600: _temp599=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp595)->f1;
goto _LL598; _LL598: _temp597=( char)(( struct Cyc_Absyn_Char_c_struct*)
_temp595)->f2; goto _LL506;} else{ goto _LL507;}} else{ goto _LL507;} _LL507:
if((( struct _tunion_struct*) _temp503)->tag == Cyc_Absyn_Const_e_tag){ _LL602:
_temp601=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp503)->f1; if((
unsigned int) _temp601 > 1u?(( struct _tunion_struct*) _temp601)->tag == Cyc_Absyn_Short_c_tag:
0){ _LL606: _temp605=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp601)->f1;
goto _LL604; _LL604: _temp603=( short)(( struct Cyc_Absyn_Short_c_struct*)
_temp601)->f2; goto _LL508;} else{ goto _LL509;}} else{ goto _LL509;} _LL509:
if((( struct _tunion_struct*) _temp503)->tag == Cyc_Absyn_Const_e_tag){ _LL608:
_temp607=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp503)->f1; if((
unsigned int) _temp607 > 1u?(( struct _tunion_struct*) _temp607)->tag == Cyc_Absyn_Int_c_tag:
0){ _LL612: _temp611=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp607)->f1;
if(( int) _temp611 == Cyc_Absyn_Signed){ goto _LL610;} else{ goto _LL511;}
_LL610: _temp609=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp607)->f2; goto
_LL510;} else{ goto _LL511;}} else{ goto _LL511;} _LL511: if((( struct
_tunion_struct*) _temp503)->tag == Cyc_Absyn_Const_e_tag){ _LL614: _temp613=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp503)->f1; if(( unsigned int)
_temp613 > 1u?(( struct _tunion_struct*) _temp613)->tag == Cyc_Absyn_Int_c_tag:
0){ _LL618: _temp617=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp613)->f1;
if(( int) _temp617 == Cyc_Absyn_Unsigned){ goto _LL616;} else{ goto _LL513;}
_LL616: _temp615=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp613)->f2; goto
_LL512;} else{ goto _LL513;}} else{ goto _LL513;} _LL513: if((( struct
_tunion_struct*) _temp503)->tag == Cyc_Absyn_Const_e_tag){ _LL620: _temp619=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp503)->f1; if(( unsigned int)
_temp619 > 1u?(( struct _tunion_struct*) _temp619)->tag == Cyc_Absyn_LongLong_c_tag:
0){ _LL624: _temp623=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp619)->f1;
goto _LL622; _LL622: _temp621=( long long)(( struct Cyc_Absyn_LongLong_c_struct*)
_temp619)->f2; goto _LL514;} else{ goto _LL515;}} else{ goto _LL515;} _LL515:
if((( struct _tunion_struct*) _temp503)->tag == Cyc_Absyn_Const_e_tag){ _LL626:
_temp625=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp503)->f1; if((
unsigned int) _temp625 > 1u?(( struct _tunion_struct*) _temp625)->tag == Cyc_Absyn_Float_c_tag:
0){ _LL628: _temp627=( struct _tagged_string)(( struct Cyc_Absyn_Float_c_struct*)
_temp625)->f1; goto _LL516;} else{ goto _LL517;}} else{ goto _LL517;} _LL517:
if((( struct _tunion_struct*) _temp503)->tag == Cyc_Absyn_Const_e_tag){ _LL630:
_temp629=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp503)->f1; if(( int)
_temp629 == Cyc_Absyn_Null_c){ goto _LL518;} else{ goto _LL519;}} else{ goto
_LL519;} _LL519: if((( struct _tunion_struct*) _temp503)->tag == Cyc_Absyn_Const_e_tag){
_LL632: _temp631=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp503)->f1; if((
unsigned int) _temp631 > 1u?(( struct _tunion_struct*) _temp631)->tag == Cyc_Absyn_String_c_tag:
0){ _LL634: _temp633=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp631)->f1; goto _LL520;} else{ goto _LL521;}} else{ goto _LL521;} _LL521:
if((( struct _tunion_struct*) _temp503)->tag == Cyc_Absyn_UnknownId_e_tag){
_LL636: _temp635=( struct _tuple0*)(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp503)->f1; goto _LL522;} else{ goto _LL523;} _LL523: if((( struct
_tunion_struct*) _temp503)->tag == Cyc_Absyn_Var_e_tag){ _LL640: _temp639=(
struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*) _temp503)->f1; goto _LL638;
_LL638: _temp637=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp503)->f2; goto
_LL524;} else{ goto _LL525;} _LL525: if((( struct _tunion_struct*) _temp503)->tag
== Cyc_Absyn_Primop_e_tag){ _LL644: _temp643=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp503)->f1; goto _LL642; _LL642: _temp641=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Primop_e_struct*) _temp503)->f2; goto _LL526;} else{ goto _LL527;}
_LL527: if((( struct _tunion_struct*) _temp503)->tag == Cyc_Absyn_AssignOp_e_tag){
_LL650: _temp649=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp503)->f1; goto _LL648; _LL648: _temp647=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp503)->f2; goto _LL646; _LL646: _temp645=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp503)->f3;
goto _LL528;} else{ goto _LL529;} _LL529: if((( struct _tunion_struct*) _temp503)->tag
== Cyc_Absyn_Increment_e_tag){ _LL654: _temp653=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Increment_e_struct*) _temp503)->f1; goto _LL652; _LL652:
_temp651=( void*)(( struct Cyc_Absyn_Increment_e_struct*) _temp503)->f2; if((
int) _temp651 == Cyc_Absyn_PreInc){ goto _LL530;} else{ goto _LL531;}} else{
goto _LL531;} _LL531: if((( struct _tunion_struct*) _temp503)->tag == Cyc_Absyn_Increment_e_tag){
_LL658: _temp657=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp503)->f1; goto _LL656; _LL656: _temp655=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp503)->f2; if(( int) _temp655 == Cyc_Absyn_PreDec){ goto _LL532;} else{ goto
_LL533;}} else{ goto _LL533;} _LL533: if((( struct _tunion_struct*) _temp503)->tag
== Cyc_Absyn_Increment_e_tag){ _LL662: _temp661=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Increment_e_struct*) _temp503)->f1; goto _LL660; _LL660:
_temp659=( void*)(( struct Cyc_Absyn_Increment_e_struct*) _temp503)->f2; if((
int) _temp659 == Cyc_Absyn_PostInc){ goto _LL534;} else{ goto _LL535;}} else{
goto _LL535;} _LL535: if((( struct _tunion_struct*) _temp503)->tag == Cyc_Absyn_Increment_e_tag){
_LL666: _temp665=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp503)->f1; goto _LL664; _LL664: _temp663=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp503)->f2; if(( int) _temp663 == Cyc_Absyn_PostDec){ goto _LL536;} else{
goto _LL537;}} else{ goto _LL537;} _LL537: if((( struct _tunion_struct*)
_temp503)->tag == Cyc_Absyn_Conditional_e_tag){ _LL672: _temp671=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp503)->f1; goto _LL670; _LL670:
_temp669=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp503)->f2; goto _LL668; _LL668: _temp667=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Conditional_e_struct*) _temp503)->f3; goto _LL538;} else{ goto _LL539;}
_LL539: if((( struct _tunion_struct*) _temp503)->tag == Cyc_Absyn_SeqExp_e_tag){
_LL676: _temp675=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp503)->f1; goto _LL674; _LL674: _temp673=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_SeqExp_e_struct*) _temp503)->f2; goto _LL540;} else{ goto _LL541;}
_LL541: if((( struct _tunion_struct*) _temp503)->tag == Cyc_Absyn_UnknownCall_e_tag){
_LL680: _temp679=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp503)->f1; goto _LL678; _LL678: _temp677=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnknownCall_e_struct*) _temp503)->f2; goto _LL542;} else{ goto _LL543;}
_LL543: if((( struct _tunion_struct*) _temp503)->tag == Cyc_Absyn_FnCall_e_tag){
_LL684: _temp683=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp503)->f1; goto _LL682; _LL682: _temp681=( struct Cyc_List_List*)(( struct
Cyc_Absyn_FnCall_e_struct*) _temp503)->f2; goto _LL544;} else{ goto _LL545;}
_LL545: if((( struct _tunion_struct*) _temp503)->tag == Cyc_Absyn_Throw_e_tag){
_LL686: _temp685=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*)
_temp503)->f1; goto _LL546;} else{ goto _LL547;} _LL547: if((( struct
_tunion_struct*) _temp503)->tag == Cyc_Absyn_NoInstantiate_e_tag){ _LL688:
_temp687=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp503)->f1; goto _LL548;} else{ goto _LL549;} _LL549: if((( struct
_tunion_struct*) _temp503)->tag == Cyc_Absyn_Instantiate_e_tag){ _LL692:
_temp691=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp503)->f1; goto _LL690; _LL690: _temp689=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Instantiate_e_struct*) _temp503)->f2; goto _LL550;} else{ goto _LL551;}
_LL551: if((( struct _tunion_struct*) _temp503)->tag == Cyc_Absyn_Cast_e_tag){
_LL696: _temp695=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp503)->f1; goto
_LL694; _LL694: _temp693=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp503)->f2; goto _LL552;} else{ goto _LL553;} _LL553: if((( struct
_tunion_struct*) _temp503)->tag == Cyc_Absyn_Address_e_tag){ _LL698: _temp697=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*) _temp503)->f1; goto
_LL554;} else{ goto _LL555;} _LL555: if((( struct _tunion_struct*) _temp503)->tag
== Cyc_Absyn_New_e_tag){ _LL702: _temp701=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*)
_temp503)->f1; goto _LL700; _LL700: _temp699=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_New_e_struct*) _temp503)->f2; goto _LL556;} else{ goto _LL557;} _LL557:
if((( struct _tunion_struct*) _temp503)->tag == Cyc_Absyn_Sizeoftyp_e_tag){
_LL704: _temp703=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp503)->f1;
goto _LL558;} else{ goto _LL559;} _LL559: if((( struct _tunion_struct*) _temp503)->tag
== Cyc_Absyn_Sizeofexp_e_tag){ _LL706: _temp705=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Sizeofexp_e_struct*) _temp503)->f1; goto _LL560;} else{ goto
_LL561;} _LL561: if((( struct _tunion_struct*) _temp503)->tag == Cyc_Absyn_Deref_e_tag){
_LL708: _temp707=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp503)->f1; goto _LL562;} else{ goto _LL563;} _LL563: if((( struct
_tunion_struct*) _temp503)->tag == Cyc_Absyn_StructMember_e_tag){ _LL712:
_temp711=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp503)->f1; goto _LL710; _LL710: _temp709=( struct _tagged_string*)(( struct
Cyc_Absyn_StructMember_e_struct*) _temp503)->f2; goto _LL564;} else{ goto _LL565;}
_LL565: if((( struct _tunion_struct*) _temp503)->tag == Cyc_Absyn_StructArrow_e_tag){
_LL716: _temp715=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp503)->f1; goto _LL714; _LL714: _temp713=( struct _tagged_string*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp503)->f2; goto _LL566;} else{ goto _LL567;}
_LL567: if((( struct _tunion_struct*) _temp503)->tag == Cyc_Absyn_Subscript_e_tag){
_LL720: _temp719=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp503)->f1; goto _LL718; _LL718: _temp717=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Subscript_e_struct*) _temp503)->f2; goto _LL568;} else{ goto _LL569;}
_LL569: if((( struct _tunion_struct*) _temp503)->tag == Cyc_Absyn_Tuple_e_tag){
_LL722: _temp721=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp503)->f1; goto _LL570;} else{ goto _LL571;} _LL571: if((( struct
_tunion_struct*) _temp503)->tag == Cyc_Absyn_CompoundLit_e_tag){ _LL726:
_temp725=( struct _tuple1*)(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp503)->f1;
goto _LL724; _LL724: _temp723=( struct Cyc_List_List*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp503)->f2; goto _LL572;} else{ goto _LL573;} _LL573: if((( struct
_tunion_struct*) _temp503)->tag == Cyc_Absyn_Array_e_tag){ _LL728: _temp727=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp503)->f1; goto
_LL574;} else{ goto _LL575;} _LL575: if((( struct _tunion_struct*) _temp503)->tag
== Cyc_Absyn_Comprehension_e_tag){ _LL734: _temp733=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp503)->f1; goto _LL732; _LL732:
_temp731=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp503)->f2; goto _LL730; _LL730: _temp729=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Comprehension_e_struct*) _temp503)->f3; goto _LL576;} else{ goto
_LL577;} _LL577: if((( struct _tunion_struct*) _temp503)->tag == Cyc_Absyn_Struct_e_tag){
_LL742: _temp741=( struct _tuple0*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp503)->f1; goto _LL740; _LL740: _temp739=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp503)->f2; goto _LL738; _LL738: _temp737=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*) _temp503)->f3; goto
_LL736; _LL736: _temp735=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp503)->f4; goto _LL578;} else{ goto _LL579;} _LL579: if((( struct
_tunion_struct*) _temp503)->tag == Cyc_Absyn_Tunion_e_tag){ _LL752: _temp751=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp503)->f1; goto
_LL750; _LL750: _temp749=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp503)->f2; goto _LL748; _LL748: _temp747=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Tunion_e_struct*) _temp503)->f3; goto _LL746; _LL746: _temp745=(
struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp503)->f4;
goto _LL744; _LL744: _temp743=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp503)->f5; goto _LL580;} else{ goto _LL581;} _LL581: if((( struct
_tunion_struct*) _temp503)->tag == Cyc_Absyn_XTunion_e_tag){ _LL760: _temp759=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_XTunion_e_struct*) _temp503)->f1; goto
_LL758; _LL758: _temp757=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_e_struct*)
_temp503)->f2; goto _LL756; _LL756: _temp755=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_e_struct*) _temp503)->f3; goto _LL754; _LL754: _temp753=(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_e_struct*) _temp503)->f4;
goto _LL582;} else{ goto _LL583;} _LL583: if((( struct _tunion_struct*) _temp503)->tag
== Cyc_Absyn_Enum_e_tag){ _LL766: _temp765=( struct _tuple0*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp503)->f1; goto _LL764; _LL764: _temp763=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_e_struct*) _temp503)->f2; goto _LL762; _LL762: _temp761=(
struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_e_struct*) _temp503)->f3;
goto _LL584;} else{ goto _LL585;} _LL585: if((( struct _tunion_struct*) _temp503)->tag
== Cyc_Absyn_Malloc_e_tag){ _LL770: _temp769=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Malloc_e_struct*) _temp503)->f1; goto _LL768; _LL768: _temp767=( void*)((
struct Cyc_Absyn_Malloc_e_struct*) _temp503)->f2; goto _LL586;} else{ goto
_LL587;} _LL587: if((( struct _tunion_struct*) _temp503)->tag == Cyc_Absyn_UnresolvedMem_e_tag){
_LL774: _temp773=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp503)->f1; goto _LL772; _LL772: _temp771=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnresolvedMem_e_struct*) _temp503)->f2; goto _LL588;} else{ goto
_LL589;} _LL589: if((( struct _tunion_struct*) _temp503)->tag == Cyc_Absyn_StmtExp_e_tag){
_LL776: _temp775=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp503)->f1; goto _LL590;} else{ goto _LL591;} _LL591: if((( struct
_tunion_struct*) _temp503)->tag == Cyc_Absyn_Codegen_e_tag){ _LL778: _temp777=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*) _temp503)->f1;
goto _LL592;} else{ goto _LL593;} _LL593: if((( struct _tunion_struct*) _temp503)->tag
== Cyc_Absyn_Fill_e_tag){ _LL780: _temp779=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*)
_temp503)->f1; goto _LL594;} else{ goto _LL504;} _LL506: Cyc_Absyndump_dump_char((
int)'\''); Cyc_Absyndump_dump_nospace( Cyc_Absynpp_char_escape( _temp597)); Cyc_Absyndump_dump_char((
int)'\''); goto _LL504; _LL508: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp781=( char*)"(short)"; struct _tagged_string _temp782; _temp782.curr=
_temp781; _temp782.base= _temp781; _temp782.last_plus_one= _temp781 + 8;
_temp782;})); Cyc_Absyndump_dump_nospace( Cyc_Core_string_of_int(( int) _temp603));
goto _LL504; _LL510: Cyc_Absyndump_dump( Cyc_Core_string_of_int( _temp609));
goto _LL504; _LL512: Cyc_Absyndump_dump( Cyc_Core_string_of_int( _temp615)); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp783=( char*)"u"; struct _tagged_string
_temp784; _temp784.curr= _temp783; _temp784.base= _temp783; _temp784.last_plus_one=
_temp783 + 2; _temp784;})); goto _LL504; _LL514: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp785=( char*)"<<FIX LONG LONG CONSTANT>>"; struct
_tagged_string _temp786; _temp786.curr= _temp785; _temp786.base= _temp785;
_temp786.last_plus_one= _temp785 + 27; _temp786;})); goto _LL504; _LL516: Cyc_Absyndump_dump(
_temp627); goto _LL504; _LL518: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp787=( char*)"null"; struct _tagged_string _temp788; _temp788.curr=
_temp787; _temp788.base= _temp787; _temp788.last_plus_one= _temp787 + 5;
_temp788;})); goto _LL504; _LL520: Cyc_Absyndump_dump_char(( int)'"'); Cyc_Absyndump_dump_nospace(
Cyc_Absynpp_string_escape( _temp633)); Cyc_Absyndump_dump_char(( int)'"'); goto
_LL504; _LL522: _temp639= _temp635; goto _LL524; _LL524: Cyc_Absyndump_dumpqvar(
_temp639); goto _LL504; _LL526: { struct _tagged_string pstr= Cyc_Absynpp_prim2str(
_temp643); if( ! Cyc_Absyn_is_format_prim( _temp643)){ switch((( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)( _temp641)){ case 1: _LL789: if( _temp643 ==(
void*) Cyc_Absyn_Size){ Cyc_Absyndump_dumpexp_prec( myprec,( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp791= _temp641; if( _temp791 == 0){ _throw(
Null_Exception);} _temp791->hd;})); Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp792=( char*)".size"; struct _tagged_string _temp793; _temp793.curr=
_temp792; _temp793.base= _temp792; _temp793.last_plus_one= _temp792 + 6;
_temp793;}));} else{ Cyc_Absyndump_dump( pstr); Cyc_Absyndump_dumpexp_prec(
myprec,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp794= _temp641; if(
_temp794 == 0){ _throw( Null_Exception);} _temp794->hd;}));} break; case 2:
_LL790: Cyc_Absyndump_dumpexp_prec( myprec,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp796= _temp641; if( _temp796 == 0){ _throw( Null_Exception);} _temp796->hd;}));
Cyc_Absyndump_dump( pstr); Cyc_Absyndump_dumpexp_prec( myprec,( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp798=({ struct Cyc_List_List* _temp797= _temp641; if(
_temp797 == 0){ _throw( Null_Exception);} _temp797->tl;}); if( _temp798 == 0){
_throw( Null_Exception);} _temp798->hd;})); break; default: _LL795:( void)
_throw(( struct _xtunion_struct*)({ struct Cyc_Core_Failure_struct* _temp800=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp800[ 0]=({ struct Cyc_Core_Failure_struct _temp801; _temp801.tag= Cyc_Core_Failure_tag;
_temp801.f1=( struct _tagged_string)({ char* _temp802=( char*)"Absyndump -- Bad number of arguments to primop";
struct _tagged_string _temp803; _temp803.curr= _temp802; _temp803.base= _temp802;
_temp803.last_plus_one= _temp802 + 47; _temp803;}); _temp801;}); _temp800;}));}}
else{ Cyc_Absyndump_dump( pstr); Cyc_Absyndump_dump_nospace(( struct
_tagged_string)({ char* _temp804=( char*)"("; struct _tagged_string _temp805;
_temp805.curr= _temp804; _temp805.base= _temp804; _temp805.last_plus_one=
_temp804 + 2; _temp805;})); Cyc_Absyndump_dumpexps_prec( 20, _temp641); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp806=( char*)")"; struct _tagged_string
_temp807; _temp807.curr= _temp806; _temp807.base= _temp806; _temp807.last_plus_one=
_temp806 + 2; _temp807;}));} goto _LL504;} _LL528: Cyc_Absyndump_dumpexp_prec(
myprec, _temp649); if( _temp647 != 0){ Cyc_Absyndump_dump( Cyc_Absynpp_prim2str((
void*)({ struct Cyc_Core_Opt* _temp808= _temp647; if( _temp808 == 0){ _throw(
Null_Exception);} _temp808->v;})));} Cyc_Absyndump_dump_nospace(( struct
_tagged_string)({ char* _temp809=( char*)"="; struct _tagged_string _temp810;
_temp810.curr= _temp809; _temp810.base= _temp809; _temp810.last_plus_one=
_temp809 + 2; _temp810;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp645); goto
_LL504; _LL530: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp811=(
char*)"++"; struct _tagged_string _temp812; _temp812.curr= _temp811; _temp812.base=
_temp811; _temp812.last_plus_one= _temp811 + 3; _temp812;})); Cyc_Absyndump_dumpexp_prec(
myprec, _temp653); goto _LL504; _LL532: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp813=( char*)"--"; struct _tagged_string _temp814;
_temp814.curr= _temp813; _temp814.base= _temp813; _temp814.last_plus_one=
_temp813 + 3; _temp814;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp657); goto
_LL504; _LL534: Cyc_Absyndump_dumpexp_prec( myprec, _temp661); Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp815=( char*)"++"; struct _tagged_string
_temp816; _temp816.curr= _temp815; _temp816.base= _temp815; _temp816.last_plus_one=
_temp815 + 3; _temp816;})); goto _LL504; _LL536: Cyc_Absyndump_dumpexp_prec(
myprec, _temp665); Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp817=(
char*)"--"; struct _tagged_string _temp818; _temp818.curr= _temp817; _temp818.base=
_temp817; _temp818.last_plus_one= _temp817 + 3; _temp818;})); goto _LL504;
_LL538: Cyc_Absyndump_dumpexp_prec( myprec, _temp671); Cyc_Absyndump_dump_char((
int)'?'); Cyc_Absyndump_dumpexp_prec( 0, _temp669); Cyc_Absyndump_dump_char((
int)':'); Cyc_Absyndump_dumpexp_prec( myprec, _temp667); goto _LL504; _LL540:
Cyc_Absyndump_dumpexp_prec( myprec, _temp675); Cyc_Absyndump_dump_char(( int)',');
Cyc_Absyndump_dumpexp_prec( myprec, _temp673); goto _LL504; _LL542: _temp683=
_temp679; _temp681= _temp677; goto _LL544; _LL544: Cyc_Absyndump_dumpexp_prec(
myprec, _temp683); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char*
_temp819=( char*)"("; struct _tagged_string _temp820; _temp820.curr= _temp819;
_temp820.base= _temp819; _temp820.last_plus_one= _temp819 + 2; _temp820;})); Cyc_Absyndump_dumpexps_prec(
20, _temp681); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char*
_temp821=( char*)")"; struct _tagged_string _temp822; _temp822.curr= _temp821;
_temp822.base= _temp821; _temp822.last_plus_one= _temp821 + 2; _temp822;}));
goto _LL504; _LL546: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp823=( char*)"throw"; struct _tagged_string _temp824; _temp824.curr=
_temp823; _temp824.base= _temp823; _temp824.last_plus_one= _temp823 + 6;
_temp824;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp685); goto _LL504; _LL548:
_temp691= _temp687; goto _LL550; _LL550: Cyc_Absyndump_dumpexp_prec( inprec,
_temp691); goto _LL504; _LL552: Cyc_Absyndump_dump_char(( int)'('); Cyc_Absyndump_dumptyp(
_temp695); Cyc_Absyndump_dump_char(( int)')'); Cyc_Absyndump_dumpexp_prec(
myprec, _temp693); goto _LL504; _LL554: Cyc_Absyndump_dump_char(( int)'&'); Cyc_Absyndump_dumpexp_prec(
myprec, _temp697); goto _LL504; _LL556: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp825=( char*)"new "; struct _tagged_string _temp826;
_temp826.curr= _temp825; _temp826.base= _temp825; _temp826.last_plus_one=
_temp825 + 5; _temp826;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp699); goto
_LL504; _LL558: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp827=(
char*)"sizeof("; struct _tagged_string _temp828; _temp828.curr= _temp827;
_temp828.base= _temp827; _temp828.last_plus_one= _temp827 + 8; _temp828;})); Cyc_Absyndump_dumptyp(
_temp703); Cyc_Absyndump_dump_char(( int)')'); goto _LL504; _LL560: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp829=( char*)"sizeof("; struct _tagged_string
_temp830; _temp830.curr= _temp829; _temp830.base= _temp829; _temp830.last_plus_one=
_temp829 + 8; _temp830;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp705); Cyc_Absyndump_dump_char((
int)')'); goto _LL504; _LL562: Cyc_Absyndump_dump_char(( int)'*'); Cyc_Absyndump_dumpexp_prec(
myprec, _temp707); goto _LL504; _LL564: Cyc_Absyndump_dumpexp_prec( myprec,
_temp711); Cyc_Absyndump_dump_char(( int)'.'); Cyc_Absyndump_dump_nospace(*
_temp709); goto _LL504; _LL566: Cyc_Absyndump_dumpexp_prec( myprec, _temp715);
Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char* _temp831=( char*)"->";
struct _tagged_string _temp832; _temp832.curr= _temp831; _temp832.base= _temp831;
_temp832.last_plus_one= _temp831 + 3; _temp832;})); Cyc_Absyndump_dump_nospace(*
_temp713); goto _LL504; _LL568: Cyc_Absyndump_dumpexp_prec( myprec, _temp719);
Cyc_Absyndump_dump_char(( int)'['); Cyc_Absyndump_dumpexp( _temp717); Cyc_Absyndump_dump_char((
int)']'); goto _LL504; _LL570: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp833=( char*)"$("; struct _tagged_string _temp834; _temp834.curr=
_temp833; _temp834.base= _temp833; _temp834.last_plus_one= _temp833 + 3;
_temp834;})); Cyc_Absyndump_dumpexps_prec( 20, _temp721); Cyc_Absyndump_dump_char((
int)')'); goto _LL504; _LL572: Cyc_Absyndump_dump_char(( int)'('); Cyc_Absyndump_dumptyp((*
_temp725).f3); Cyc_Absyndump_dump_char(( int)')');(( void(*)( void(* f)( struct
_tuple7*), struct Cyc_List_List* l, struct _tagged_string start, struct
_tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde,
_temp723,( struct _tagged_string)({ char* _temp835=( char*)"{"; struct
_tagged_string _temp836; _temp836.curr= _temp835; _temp836.base= _temp835;
_temp836.last_plus_one= _temp835 + 2; _temp836;}),( struct _tagged_string)({
char* _temp837=( char*)"}"; struct _tagged_string _temp838; _temp838.curr=
_temp837; _temp838.base= _temp837; _temp838.last_plus_one= _temp837 + 2;
_temp838;}),( struct _tagged_string)({ char* _temp839=( char*)","; struct
_tagged_string _temp840; _temp840.curr= _temp839; _temp840.base= _temp839;
_temp840.last_plus_one= _temp839 + 2; _temp840;})); goto _LL504; _LL574:(( void(*)(
void(* f)( struct _tuple7*), struct Cyc_List_List* l, struct _tagged_string
start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde, _temp727,( struct _tagged_string)({ char* _temp841=( char*)"{";
struct _tagged_string _temp842; _temp842.curr= _temp841; _temp842.base= _temp841;
_temp842.last_plus_one= _temp841 + 2; _temp842;}),( struct _tagged_string)({
char* _temp843=( char*)"}"; struct _tagged_string _temp844; _temp844.curr=
_temp843; _temp844.base= _temp843; _temp844.last_plus_one= _temp843 + 2;
_temp844;}),( struct _tagged_string)({ char* _temp845=( char*)","; struct
_tagged_string _temp846; _temp846.curr= _temp845; _temp846.base= _temp845;
_temp846.last_plus_one= _temp845 + 2; _temp846;})); goto _LL504; _LL576: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp847=( char*)"new {for"; struct
_tagged_string _temp848; _temp848.curr= _temp847; _temp848.base= _temp847;
_temp848.last_plus_one= _temp847 + 9; _temp848;})); Cyc_Absyndump_dump_str((*
_temp733->name).f2); Cyc_Absyndump_dump_char(( int)'<'); Cyc_Absyndump_dumpexp(
_temp731); Cyc_Absyndump_dump_char(( int)':'); Cyc_Absyndump_dumpexp( _temp729);
Cyc_Absyndump_dump_char(( int)'}'); goto _LL504; _LL578: Cyc_Absyndump_dumpqvar(
_temp741);(( void(*)( void(* f)( struct _tuple7*), struct Cyc_List_List* l,
struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde, _temp737,( struct
_tagged_string)({ char* _temp849=( char*)"{"; struct _tagged_string _temp850;
_temp850.curr= _temp849; _temp850.base= _temp849; _temp850.last_plus_one=
_temp849 + 2; _temp850;}),( struct _tagged_string)({ char* _temp851=( char*)"}";
struct _tagged_string _temp852; _temp852.curr= _temp851; _temp852.base= _temp851;
_temp852.last_plus_one= _temp851 + 2; _temp852;}),( struct _tagged_string)({
char* _temp853=( char*)","; struct _tagged_string _temp854; _temp854.curr=
_temp853; _temp854.base= _temp853; _temp854.last_plus_one= _temp853 + 2;
_temp854;})); goto _LL504; _LL580: _temp757= _temp747; _temp753= _temp743; goto
_LL582; _LL582: Cyc_Absyndump_dumpqvar( _temp753->name); if( _temp757 != 0){((
void(*)( void(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpexp, _temp757,( struct _tagged_string)({ char* _temp855=( char*)"(";
struct _tagged_string _temp856; _temp856.curr= _temp855; _temp856.base= _temp855;
_temp856.last_plus_one= _temp855 + 2; _temp856;}),( struct _tagged_string)({
char* _temp857=( char*)")"; struct _tagged_string _temp858; _temp858.curr=
_temp857; _temp858.base= _temp857; _temp858.last_plus_one= _temp857 + 2;
_temp858;}),( struct _tagged_string)({ char* _temp859=( char*)","; struct
_tagged_string _temp860; _temp860.curr= _temp859; _temp860.base= _temp859;
_temp860.last_plus_one= _temp859 + 2; _temp860;}));} goto _LL504; _LL584: Cyc_Absyndump_dumpqvar(
_temp765); goto _LL504; _LL586: if( _temp769 != 0){ Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp861=( char*)"rmalloc("; struct _tagged_string
_temp862; _temp862.curr= _temp861; _temp862.base= _temp861; _temp862.last_plus_one=
_temp861 + 9; _temp862;})); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp863= _temp769; if( _temp863 == 0){ _throw(
Null_Exception);} _temp863;})); Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp864=( char*)","; struct _tagged_string _temp865; _temp865.curr=
_temp864; _temp865.base= _temp864; _temp865.last_plus_one= _temp864 + 2;
_temp865;}));} else{ Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp866=( char*)"malloc("; struct _tagged_string _temp867; _temp867.curr=
_temp866; _temp867.base= _temp866; _temp867.last_plus_one= _temp866 + 8;
_temp867;}));} Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp868=(
char*)"sizeof("; struct _tagged_string _temp869; _temp869.curr= _temp868;
_temp869.base= _temp868; _temp869.last_plus_one= _temp868 + 8; _temp869;})); Cyc_Absyndump_dumptyp(
_temp767); Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp870=( char*)"))";
struct _tagged_string _temp871; _temp871.curr= _temp870; _temp871.base= _temp870;
_temp871.last_plus_one= _temp870 + 3; _temp871;})); goto _LL504; _LL588:(( void(*)(
void(* f)( struct _tuple7*), struct Cyc_List_List* l, struct _tagged_string
start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde, _temp771,( struct _tagged_string)({ char* _temp872=( char*)"{";
struct _tagged_string _temp873; _temp873.curr= _temp872; _temp873.base= _temp872;
_temp873.last_plus_one= _temp872 + 2; _temp873;}),( struct _tagged_string)({
char* _temp874=( char*)"}"; struct _tagged_string _temp875; _temp875.curr=
_temp874; _temp875.base= _temp874; _temp875.last_plus_one= _temp874 + 2;
_temp875;}),( struct _tagged_string)({ char* _temp876=( char*)","; struct
_tagged_string _temp877; _temp877.curr= _temp876; _temp877.base= _temp876;
_temp877.last_plus_one= _temp876 + 2; _temp877;})); goto _LL504; _LL590: Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp878=( char*)"({"; struct _tagged_string
_temp879; _temp879.curr= _temp878; _temp879.base= _temp878; _temp879.last_plus_one=
_temp878 + 3; _temp879;})); Cyc_Absyndump_dumpstmt( _temp775); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp880=( char*)"})"; struct _tagged_string
_temp881; _temp881.curr= _temp880; _temp881.base= _temp880; _temp881.last_plus_one=
_temp880 + 3; _temp881;})); goto _LL504; _LL592: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp882=( char*)"codegen("; struct _tagged_string
_temp883; _temp883.curr= _temp882; _temp883.base= _temp882; _temp883.last_plus_one=
_temp882 + 9; _temp883;})); Cyc_Absyndump_dumpdecl(({ struct Cyc_Absyn_Decl*
_temp884=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp884->r=( void*)(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp885=( struct
Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp885[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp886; _temp886.tag= Cyc_Absyn_Fn_d_tag;
_temp886.f1= _temp777; _temp886;}); _temp885;})); _temp884->loc= e->loc;
_temp884;})); Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp887=(
char*)")"; struct _tagged_string _temp888; _temp888.curr= _temp887; _temp888.base=
_temp887; _temp888.last_plus_one= _temp887 + 2; _temp888;})); goto _LL504;
_LL594: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp889=( char*)"fill(";
struct _tagged_string _temp890; _temp890.curr= _temp889; _temp890.base= _temp889;
_temp890.last_plus_one= _temp889 + 6; _temp890;})); Cyc_Absyndump_dumpexp(
_temp779); Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp891=( char*)")";
struct _tagged_string _temp892; _temp892.curr= _temp891; _temp892.base= _temp891;
_temp892.last_plus_one= _temp891 + 2; _temp892;})); goto _LL504; _LL504:;} if(
inprec >= myprec){ Cyc_Absyndump_dump_char(( int)')');}} void Cyc_Absyndump_dumpexp(
struct Cyc_Absyn_Exp* e){ Cyc_Absyndump_dumpexp_prec( 0, e);} void Cyc_Absyndump_dumpswitchclauses(
struct Cyc_List_List* scs){ for( 0; scs != 0; scs=({ struct Cyc_List_List*
_temp893= scs; if( _temp893 == 0){ _throw( Null_Exception);} _temp893->tl;})){
struct Cyc_Absyn_Switch_clause* c=( struct Cyc_Absyn_Switch_clause*)({ struct
Cyc_List_List* _temp903= scs; if( _temp903 == 0){ _throw( Null_Exception);}
_temp903->hd;}); if( c->where_clause == 0?( void*)( c->pattern)->r ==( void*)
Cyc_Absyn_Wild_p: 0){ Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp894=( char*)"default:"; struct _tagged_string _temp895; _temp895.curr=
_temp894; _temp895.base= _temp894; _temp895.last_plus_one= _temp894 + 9;
_temp895;}));} else{ Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp896=( char*)"case"; struct _tagged_string _temp897; _temp897.curr= _temp896;
_temp897.base= _temp896; _temp897.last_plus_one= _temp896 + 5; _temp897;})); Cyc_Absyndump_dumppat(
c->pattern); if( c->where_clause != 0){ Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp898=( char*)"&&"; struct _tagged_string _temp899;
_temp899.curr= _temp898; _temp899.base= _temp898; _temp899.last_plus_one=
_temp898 + 3; _temp899;})); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp900= c->where_clause; if( _temp900 == 0){ _throw(
Null_Exception);} _temp900;}));} Cyc_Absyndump_dump_nospace(( struct
_tagged_string)({ char* _temp901=( char*)":"; struct _tagged_string _temp902;
_temp902.curr= _temp901; _temp902.base= _temp901; _temp902.last_plus_one=
_temp901 + 2; _temp902;}));} Cyc_Absyndump_dumpstmt( c->body);}} void Cyc_Absyndump_dumpstmt(
struct Cyc_Absyn_Stmt* s){ void* _temp904=( void*) s->r; struct Cyc_Absyn_Exp*
_temp948; struct Cyc_Absyn_Stmt* _temp950; struct Cyc_Absyn_Stmt* _temp952;
struct Cyc_Absyn_Exp* _temp954; struct Cyc_Absyn_Exp* _temp956; struct Cyc_Absyn_Stmt*
_temp958; struct Cyc_Absyn_Stmt* _temp960; struct Cyc_Absyn_Exp* _temp962;
struct Cyc_Absyn_Stmt* _temp964; struct _tuple2 _temp966; struct Cyc_Absyn_Stmt*
_temp968; struct Cyc_Absyn_Exp* _temp970; struct Cyc_Absyn_Stmt* _temp972;
struct Cyc_Absyn_Stmt* _temp974; struct Cyc_Absyn_Stmt* _temp976; struct
_tagged_string* _temp978; struct Cyc_Absyn_Switch_clause** _temp980; struct Cyc_List_List*
_temp982; struct Cyc_Absyn_Switch_clause** _temp984; struct Cyc_List_List*
_temp986; struct Cyc_Absyn_Stmt* _temp988; struct _tuple2 _temp990; struct Cyc_Absyn_Stmt*
_temp992; struct Cyc_Absyn_Exp* _temp994; struct _tuple2 _temp996; struct Cyc_Absyn_Stmt*
_temp998; struct Cyc_Absyn_Exp* _temp1000; struct Cyc_Absyn_Exp* _temp1002;
struct Cyc_List_List* _temp1004; struct Cyc_Absyn_Exp* _temp1006; struct Cyc_Absyn_Stmt*
_temp1008; struct Cyc_Absyn_Decl* _temp1010; struct Cyc_Absyn_Stmt* _temp1012;
struct Cyc_Absyn_Stmt* _temp1014; struct Cyc_Absyn_Stmt* _temp1016; struct
_tagged_string* _temp1018; struct _tuple2 _temp1020; struct Cyc_Absyn_Stmt*
_temp1022; struct Cyc_Absyn_Exp* _temp1024; struct Cyc_Absyn_Stmt* _temp1026;
struct Cyc_List_List* _temp1028; struct Cyc_Absyn_Stmt* _temp1030; struct Cyc_Absyn_Stmt*
_temp1032; struct Cyc_Absyn_Vardecl* _temp1034; struct Cyc_Absyn_Tvar* _temp1036;
_LL906: if(( int) _temp904 == Cyc_Absyn_Skip_s){ goto _LL907;} else{ goto _LL908;}
_LL908: if(( unsigned int) _temp904 > 1u?(( struct _tunion_struct*) _temp904)->tag
== Cyc_Absyn_Exp_s_tag: 0){ _LL949: _temp948=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Exp_s_struct*) _temp904)->f1; goto _LL909;} else{ goto _LL910;} _LL910:
if(( unsigned int) _temp904 > 1u?(( struct _tunion_struct*) _temp904)->tag ==
Cyc_Absyn_Seq_s_tag: 0){ _LL953: _temp952=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp904)->f1; goto _LL951; _LL951: _temp950=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_Seq_s_struct*) _temp904)->f2; goto _LL911;} else{ goto _LL912;} _LL912:
if(( unsigned int) _temp904 > 1u?(( struct _tunion_struct*) _temp904)->tag ==
Cyc_Absyn_Return_s_tag: 0){ _LL955: _temp954=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Return_s_struct*) _temp904)->f1; if( _temp954 == 0){ goto _LL913;}
else{ goto _LL914;}} else{ goto _LL914;} _LL914: if(( unsigned int) _temp904 > 1u?((
struct _tunion_struct*) _temp904)->tag == Cyc_Absyn_Return_s_tag: 0){ _LL957:
_temp956=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*) _temp904)->f1;
goto _LL915;} else{ goto _LL916;} _LL916: if(( unsigned int) _temp904 > 1u?((
struct _tunion_struct*) _temp904)->tag == Cyc_Absyn_IfThenElse_s_tag: 0){ _LL963:
_temp962=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp904)->f1; goto _LL961; _LL961: _temp960=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_IfThenElse_s_struct*) _temp904)->f2; goto _LL959; _LL959: _temp958=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp904)->f3;
goto _LL917;} else{ goto _LL918;} _LL918: if(( unsigned int) _temp904 > 1u?((
struct _tunion_struct*) _temp904)->tag == Cyc_Absyn_While_s_tag: 0){ _LL967:
_temp966=( struct _tuple2)(( struct Cyc_Absyn_While_s_struct*) _temp904)->f1;
_LL971: _temp970= _temp966.f1; goto _LL969; _LL969: _temp968= _temp966.f2; goto
_LL965; _LL965: _temp964=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_While_s_struct*)
_temp904)->f2; goto _LL919;} else{ goto _LL920;} _LL920: if(( unsigned int)
_temp904 > 1u?(( struct _tunion_struct*) _temp904)->tag == Cyc_Absyn_Break_s_tag:
0){ _LL973: _temp972=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Break_s_struct*)
_temp904)->f1; goto _LL921;} else{ goto _LL922;} _LL922: if(( unsigned int)
_temp904 > 1u?(( struct _tunion_struct*) _temp904)->tag == Cyc_Absyn_Continue_s_tag:
0){ _LL975: _temp974=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Continue_s_struct*)
_temp904)->f1; goto _LL923;} else{ goto _LL924;} _LL924: if(( unsigned int)
_temp904 > 1u?(( struct _tunion_struct*) _temp904)->tag == Cyc_Absyn_Goto_s_tag:
0){ _LL979: _temp978=( struct _tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp904)->f1; goto _LL977; _LL977: _temp976=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_Goto_s_struct*) _temp904)->f2; goto _LL925;} else{ goto _LL926;}
_LL926: if(( unsigned int) _temp904 > 1u?(( struct _tunion_struct*) _temp904)->tag
== Cyc_Absyn_Fallthru_s_tag: 0){ _LL983: _temp982=( struct Cyc_List_List*)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp904)->f1; if( _temp982 == 0){ goto
_LL981;} else{ goto _LL928;} _LL981: _temp980=( struct Cyc_Absyn_Switch_clause**)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp904)->f2; goto _LL927;} else{ goto
_LL928;} _LL928: if(( unsigned int) _temp904 > 1u?(( struct _tunion_struct*)
_temp904)->tag == Cyc_Absyn_Fallthru_s_tag: 0){ _LL987: _temp986=( struct Cyc_List_List*)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp904)->f1; goto _LL985; _LL985:
_temp984=( struct Cyc_Absyn_Switch_clause**)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp904)->f2; goto _LL929;} else{ goto _LL930;} _LL930: if(( unsigned int)
_temp904 > 1u?(( struct _tunion_struct*) _temp904)->tag == Cyc_Absyn_For_s_tag:
0){ _LL1003: _temp1002=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp904)->f1; goto _LL997; _LL997: _temp996=( struct _tuple2)(( struct Cyc_Absyn_For_s_struct*)
_temp904)->f2; _LL1001: _temp1000= _temp996.f1; goto _LL999; _LL999: _temp998=
_temp996.f2; goto _LL991; _LL991: _temp990=( struct _tuple2)(( struct Cyc_Absyn_For_s_struct*)
_temp904)->f3; _LL995: _temp994= _temp990.f1; goto _LL993; _LL993: _temp992=
_temp990.f2; goto _LL989; _LL989: _temp988=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_For_s_struct*) _temp904)->f4; goto _LL931;} else{ goto _LL932;} _LL932:
if(( unsigned int) _temp904 > 1u?(( struct _tunion_struct*) _temp904)->tag ==
Cyc_Absyn_Switch_s_tag: 0){ _LL1007: _temp1006=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Switch_s_struct*) _temp904)->f1; goto _LL1005; _LL1005: _temp1004=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Switch_s_struct*) _temp904)->f2; goto
_LL933;} else{ goto _LL934;} _LL934: if(( unsigned int) _temp904 > 1u?(( struct
_tunion_struct*) _temp904)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL1011: _temp1010=(
struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*) _temp904)->f1; goto
_LL1009; _LL1009: _temp1008=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp904)->f2; goto _LL935;} else{ goto _LL936;} _LL936: if(( unsigned int)
_temp904 > 1u?(( struct _tunion_struct*) _temp904)->tag == Cyc_Absyn_Cut_s_tag:
0){ _LL1013: _temp1012=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Cut_s_struct*)
_temp904)->f1; goto _LL937;} else{ goto _LL938;} _LL938: if(( unsigned int)
_temp904 > 1u?(( struct _tunion_struct*) _temp904)->tag == Cyc_Absyn_Splice_s_tag:
0){ _LL1015: _temp1014=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Splice_s_struct*)
_temp904)->f1; goto _LL939;} else{ goto _LL940;} _LL940: if(( unsigned int)
_temp904 > 1u?(( struct _tunion_struct*) _temp904)->tag == Cyc_Absyn_Label_s_tag:
0){ _LL1019: _temp1018=( struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*)
_temp904)->f1; goto _LL1017; _LL1017: _temp1016=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Label_s_struct*) _temp904)->f2; goto _LL941;} else{ goto _LL942;}
_LL942: if(( unsigned int) _temp904 > 1u?(( struct _tunion_struct*) _temp904)->tag
== Cyc_Absyn_Do_s_tag: 0){ _LL1027: _temp1026=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_Do_s_struct*) _temp904)->f1; goto _LL1021; _LL1021: _temp1020=( struct
_tuple2)(( struct Cyc_Absyn_Do_s_struct*) _temp904)->f2; _LL1025: _temp1024=
_temp1020.f1; goto _LL1023; _LL1023: _temp1022= _temp1020.f2; goto _LL943;}
else{ goto _LL944;} _LL944: if(( unsigned int) _temp904 > 1u?(( struct
_tunion_struct*) _temp904)->tag == Cyc_Absyn_TryCatch_s_tag: 0){ _LL1031:
_temp1030=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp904)->f1; goto _LL1029; _LL1029: _temp1028=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp904)->f2; goto _LL945;} else{ goto
_LL946;} _LL946: if(( unsigned int) _temp904 > 1u?(( struct _tunion_struct*)
_temp904)->tag == Cyc_Absyn_Region_s_tag: 0){ _LL1037: _temp1036=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_Region_s_struct*) _temp904)->f1; goto _LL1035; _LL1035:
_temp1034=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Region_s_struct*)
_temp904)->f2; goto _LL1033; _LL1033: _temp1032=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Region_s_struct*) _temp904)->f3; goto _LL947;} else{ goto
_LL905;} _LL907: Cyc_Absyndump_dump_semi(); goto _LL905; _LL909: Cyc_Absyndump_dumpexp(
_temp948); Cyc_Absyndump_dump_semi(); goto _LL905; _LL911: if( Cyc_Absynpp_is_declaration(
_temp952)){ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstmt( _temp952);
Cyc_Absyndump_dump_char(( int)'}');} else{ Cyc_Absyndump_dumpstmt( _temp952);}
if( Cyc_Absynpp_is_declaration( _temp950)){ Cyc_Absyndump_dump_char(( int)'{');
Cyc_Absyndump_dumpstmt( _temp950); Cyc_Absyndump_dump_char(( int)'}');} else{
Cyc_Absyndump_dumpstmt( _temp950);} goto _LL905; _LL913: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1038=( char*)"return;"; struct
_tagged_string _temp1039; _temp1039.curr= _temp1038; _temp1039.base= _temp1038;
_temp1039.last_plus_one= _temp1038 + 8; _temp1039;})); goto _LL905; _LL915: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1040=( char*)"return"; struct _tagged_string
_temp1041; _temp1041.curr= _temp1040; _temp1041.base= _temp1040; _temp1041.last_plus_one=
_temp1040 + 7; _temp1041;})); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1042= _temp956; if( _temp1042 == 0){ _throw(
Null_Exception);} _temp1042;})); Cyc_Absyndump_dump_semi(); goto _LL905; _LL917:
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1043=( char*)"if(";
struct _tagged_string _temp1044; _temp1044.curr= _temp1043; _temp1044.base=
_temp1043; _temp1044.last_plus_one= _temp1043 + 4; _temp1044;})); Cyc_Absyndump_dumpexp(
_temp962); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char* _temp1045=(
char*)"){"; struct _tagged_string _temp1046; _temp1046.curr= _temp1045;
_temp1046.base= _temp1045; _temp1046.last_plus_one= _temp1045 + 3; _temp1046;}));
Cyc_Absyndump_dumpstmt( _temp960); Cyc_Absyndump_dump_char(( int)'}');{ void*
_temp1047=( void*) _temp958->r; _LL1049: if(( int) _temp1047 == Cyc_Absyn_Skip_s){
goto _LL1050;} else{ goto _LL1051;} _LL1051: goto _LL1052; _LL1050: goto _LL1048;
_LL1052: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1053=( char*)"else{";
struct _tagged_string _temp1054; _temp1054.curr= _temp1053; _temp1054.base=
_temp1053; _temp1054.last_plus_one= _temp1053 + 6; _temp1054;})); Cyc_Absyndump_dumpstmt(
_temp958); Cyc_Absyndump_dump_char(( int)'}'); goto _LL1048; _LL1048:;} goto
_LL905; _LL919: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1055=(
char*)"while("; struct _tagged_string _temp1056; _temp1056.curr= _temp1055;
_temp1056.base= _temp1055; _temp1056.last_plus_one= _temp1055 + 7; _temp1056;}));
Cyc_Absyndump_dumpexp( _temp970); Cyc_Absyndump_dump_nospace(( struct
_tagged_string)({ char* _temp1057=( char*)") {"; struct _tagged_string _temp1058;
_temp1058.curr= _temp1057; _temp1058.base= _temp1057; _temp1058.last_plus_one=
_temp1057 + 4; _temp1058;})); Cyc_Absyndump_dumpstmt( _temp964); Cyc_Absyndump_dump_char((
int)'}'); goto _LL905; _LL921: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1059=( char*)"break;"; struct _tagged_string _temp1060; _temp1060.curr=
_temp1059; _temp1060.base= _temp1059; _temp1060.last_plus_one= _temp1059 + 7;
_temp1060;})); goto _LL905; _LL923: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1061=( char*)"continue;"; struct _tagged_string _temp1062; _temp1062.curr=
_temp1061; _temp1062.base= _temp1061; _temp1062.last_plus_one= _temp1061 + 10;
_temp1062;})); goto _LL905; _LL925: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1063=( char*)"goto"; struct _tagged_string _temp1064; _temp1064.curr=
_temp1063; _temp1064.base= _temp1063; _temp1064.last_plus_one= _temp1063 + 5;
_temp1064;})); Cyc_Absyndump_dump_str( _temp978); Cyc_Absyndump_dump_semi();
goto _LL905; _LL927: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp1065=( char*)"fallthru;"; struct _tagged_string _temp1066; _temp1066.curr=
_temp1065; _temp1066.base= _temp1065; _temp1066.last_plus_one= _temp1065 + 10;
_temp1066;})); goto _LL905; _LL929: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1067=( char*)"fallthru("; struct _tagged_string _temp1068; _temp1068.curr=
_temp1067; _temp1068.base= _temp1067; _temp1068.last_plus_one= _temp1067 + 10;
_temp1068;})); Cyc_Absyndump_dumpexps_prec( 20, _temp986); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp1069=( char*)");"; struct _tagged_string
_temp1070; _temp1070.curr= _temp1069; _temp1070.base= _temp1069; _temp1070.last_plus_one=
_temp1069 + 3; _temp1070;})); goto _LL905; _LL931: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp1071=( char*)"for("; struct _tagged_string
_temp1072; _temp1072.curr= _temp1071; _temp1072.base= _temp1071; _temp1072.last_plus_one=
_temp1071 + 5; _temp1072;})); Cyc_Absyndump_dumpexp( _temp1002); Cyc_Absyndump_dump_semi();
Cyc_Absyndump_dumpexp( _temp1000); Cyc_Absyndump_dump_semi(); Cyc_Absyndump_dumpexp(
_temp994); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char* _temp1073=(
char*)"){"; struct _tagged_string _temp1074; _temp1074.curr= _temp1073;
_temp1074.base= _temp1073; _temp1074.last_plus_one= _temp1073 + 3; _temp1074;}));
Cyc_Absyndump_dumpstmt( _temp988); Cyc_Absyndump_dump_char(( int)'}'); goto
_LL905; _LL933: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1075=(
char*)"switch("; struct _tagged_string _temp1076; _temp1076.curr= _temp1075;
_temp1076.base= _temp1075; _temp1076.last_plus_one= _temp1075 + 8; _temp1076;}));
Cyc_Absyndump_dumpexp( _temp1006); Cyc_Absyndump_dump_nospace(( struct
_tagged_string)({ char* _temp1077=( char*)"){"; struct _tagged_string _temp1078;
_temp1078.curr= _temp1077; _temp1078.base= _temp1077; _temp1078.last_plus_one=
_temp1077 + 3; _temp1078;})); Cyc_Absyndump_dumpswitchclauses( _temp1004); Cyc_Absyndump_dump_char((
int)'}'); goto _LL905; _LL935: Cyc_Absyndump_dumpdecl( _temp1010); Cyc_Absyndump_dumpstmt(
_temp1008); goto _LL905; _LL937: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1079=( char*)"cut"; struct _tagged_string _temp1080; _temp1080.curr=
_temp1079; _temp1080.base= _temp1079; _temp1080.last_plus_one= _temp1079 + 4;
_temp1080;})); Cyc_Absyndump_dumpstmt( _temp1012); goto _LL905; _LL939: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1081=( char*)"splice"; struct _tagged_string
_temp1082; _temp1082.curr= _temp1081; _temp1082.base= _temp1081; _temp1082.last_plus_one=
_temp1081 + 7; _temp1082;})); Cyc_Absyndump_dumpstmt( _temp1014); goto _LL905;
_LL941: if( Cyc_Absynpp_is_declaration( _temp1016)){ Cyc_Absyndump_dump_str(
_temp1018); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char*
_temp1083=( char*)": {"; struct _tagged_string _temp1084; _temp1084.curr=
_temp1083; _temp1084.base= _temp1083; _temp1084.last_plus_one= _temp1083 + 4;
_temp1084;})); Cyc_Absyndump_dumpstmt( _temp1016); Cyc_Absyndump_dump_char(( int)'}');}
else{ Cyc_Absyndump_dump_str( _temp1018); Cyc_Absyndump_dump_char(( int)':');
Cyc_Absyndump_dumpstmt( _temp1016);} goto _LL905; _LL943: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1085=( char*)"do {"; struct _tagged_string
_temp1086; _temp1086.curr= _temp1085; _temp1086.base= _temp1085; _temp1086.last_plus_one=
_temp1085 + 5; _temp1086;})); Cyc_Absyndump_dumpstmt( _temp1026); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp1087=( char*)"} while ("; struct
_tagged_string _temp1088; _temp1088.curr= _temp1087; _temp1088.base= _temp1087;
_temp1088.last_plus_one= _temp1087 + 10; _temp1088;})); Cyc_Absyndump_dumpexp(
_temp1024); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char*
_temp1089=( char*)");"; struct _tagged_string _temp1090; _temp1090.curr=
_temp1089; _temp1090.base= _temp1089; _temp1090.last_plus_one= _temp1089 + 3;
_temp1090;})); goto _LL905; _LL945: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1091=( char*)"try"; struct _tagged_string _temp1092; _temp1092.curr=
_temp1091; _temp1092.base= _temp1091; _temp1092.last_plus_one= _temp1091 + 4;
_temp1092;})); Cyc_Absyndump_dumpstmt( _temp1030); Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp1093=( char*)"catch {"; struct _tagged_string
_temp1094; _temp1094.curr= _temp1093; _temp1094.base= _temp1093; _temp1094.last_plus_one=
_temp1093 + 8; _temp1094;})); Cyc_Absyndump_dumpswitchclauses( _temp1028); Cyc_Absyndump_dump_char((
int)'}'); goto _LL905; _LL947: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1095=( char*)"region<"; struct _tagged_string _temp1096; _temp1096.curr=
_temp1095; _temp1096.base= _temp1095; _temp1096.last_plus_one= _temp1095 + 8;
_temp1096;})); Cyc_Absyndump_dumptvar( _temp1036); Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp1097=( char*)"> "; struct _tagged_string _temp1098;
_temp1098.curr= _temp1097; _temp1098.base= _temp1097; _temp1098.last_plus_one=
_temp1097 + 3; _temp1098;})); Cyc_Absyndump_dumpqvar( _temp1034->name); Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1099=( char*)"{"; struct _tagged_string
_temp1100; _temp1100.curr= _temp1099; _temp1100.base= _temp1099; _temp1100.last_plus_one=
_temp1099 + 2; _temp1100;})); Cyc_Absyndump_dumpstmt( _temp1032); Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1101=( char*)"}"; struct _tagged_string
_temp1102; _temp1102.curr= _temp1101; _temp1102.base= _temp1101; _temp1102.last_plus_one=
_temp1101 + 2; _temp1102;})); goto _LL905; _LL905:;} struct _tuple8{ struct Cyc_List_List*
f1; struct Cyc_Absyn_Pat* f2; } ; void Cyc_Absyndump_dumpdp( struct _tuple8* dp){((
void(*)( void(* f)( void*), struct Cyc_List_List* l, struct _tagged_string start,
struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumpdesignator,(* dp).f1,( struct _tagged_string)({ char*
_temp1103=( char*)""; struct _tagged_string _temp1104; _temp1104.curr= _temp1103;
_temp1104.base= _temp1103; _temp1104.last_plus_one= _temp1103 + 1; _temp1104;}),(
struct _tagged_string)({ char* _temp1105=( char*)"="; struct _tagged_string
_temp1106; _temp1106.curr= _temp1105; _temp1106.base= _temp1105; _temp1106.last_plus_one=
_temp1105 + 2; _temp1106;}),( struct _tagged_string)({ char* _temp1107=( char*)"=";
struct _tagged_string _temp1108; _temp1108.curr= _temp1107; _temp1108.base=
_temp1107; _temp1108.last_plus_one= _temp1107 + 2; _temp1108;})); Cyc_Absyndump_dumppat((*
dp).f2);} void Cyc_Absyndump_dumppat( struct Cyc_Absyn_Pat* p){ void* _temp1109=(
void*) p->r; int _temp1145; void* _temp1147; int _temp1149; void* _temp1151;
char _temp1153; struct _tagged_string _temp1155; struct Cyc_Absyn_Vardecl*
_temp1157; struct Cyc_List_List* _temp1159; struct Cyc_Absyn_Pat* _temp1161;
struct Cyc_Absyn_Vardecl* _temp1163; struct _tuple0* _temp1165; struct Cyc_List_List*
_temp1167; struct Cyc_List_List* _temp1169; struct _tuple0* _temp1171; struct
Cyc_List_List* _temp1173; struct Cyc_List_List* _temp1175; struct _tuple0*
_temp1177; struct Cyc_List_List* _temp1179; struct Cyc_List_List* _temp1181;
struct Cyc_Core_Opt* _temp1183; struct Cyc_Absyn_Structdecl* _temp1185; struct
Cyc_Absyn_Tunionfield* _temp1187; struct Cyc_Absyn_Tuniondecl* _temp1189; struct
Cyc_List_List* _temp1191; struct Cyc_List_List* _temp1193; struct _tuple0*
_temp1195; struct Cyc_Absyn_Tunionfield* _temp1197; struct Cyc_Absyn_XTuniondecl*
_temp1199; struct Cyc_List_List* _temp1201; struct Cyc_List_List* _temp1203;
struct _tuple0* _temp1205; struct Cyc_Absyn_Enumfield* _temp1207; struct Cyc_Absyn_Enumdecl*
_temp1209; struct _tuple0* _temp1211; _LL1111: if(( int) _temp1109 == Cyc_Absyn_Wild_p){
goto _LL1112;} else{ goto _LL1113;} _LL1113: if(( int) _temp1109 == Cyc_Absyn_Null_p){
goto _LL1114;} else{ goto _LL1115;} _LL1115: if(( unsigned int) _temp1109 > 2u?((
struct _tunion_struct*) _temp1109)->tag == Cyc_Absyn_Int_p_tag: 0){ _LL1148:
_temp1147=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp1109)->f1; if(( int)
_temp1147 == Cyc_Absyn_Signed){ goto _LL1146;} else{ goto _LL1117;} _LL1146:
_temp1145=( int)(( struct Cyc_Absyn_Int_p_struct*) _temp1109)->f2; goto _LL1116;}
else{ goto _LL1117;} _LL1117: if(( unsigned int) _temp1109 > 2u?(( struct
_tunion_struct*) _temp1109)->tag == Cyc_Absyn_Int_p_tag: 0){ _LL1152: _temp1151=(
void*)(( struct Cyc_Absyn_Int_p_struct*) _temp1109)->f1; if(( int) _temp1151 ==
Cyc_Absyn_Unsigned){ goto _LL1150;} else{ goto _LL1119;} _LL1150: _temp1149=(
int)(( struct Cyc_Absyn_Int_p_struct*) _temp1109)->f2; goto _LL1118;} else{ goto
_LL1119;} _LL1119: if(( unsigned int) _temp1109 > 2u?(( struct _tunion_struct*)
_temp1109)->tag == Cyc_Absyn_Char_p_tag: 0){ _LL1154: _temp1153=( char)(( struct
Cyc_Absyn_Char_p_struct*) _temp1109)->f1; goto _LL1120;} else{ goto _LL1121;}
_LL1121: if(( unsigned int) _temp1109 > 2u?(( struct _tunion_struct*) _temp1109)->tag
== Cyc_Absyn_Float_p_tag: 0){ _LL1156: _temp1155=( struct _tagged_string)((
struct Cyc_Absyn_Float_p_struct*) _temp1109)->f1; goto _LL1122;} else{ goto
_LL1123;} _LL1123: if(( unsigned int) _temp1109 > 2u?(( struct _tunion_struct*)
_temp1109)->tag == Cyc_Absyn_Var_p_tag: 0){ _LL1158: _temp1157=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_p_struct*) _temp1109)->f1; goto _LL1124;} else{ goto
_LL1125;} _LL1125: if(( unsigned int) _temp1109 > 2u?(( struct _tunion_struct*)
_temp1109)->tag == Cyc_Absyn_Tuple_p_tag: 0){ _LL1160: _temp1159=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tuple_p_struct*) _temp1109)->f1; goto _LL1126;} else{ goto
_LL1127;} _LL1127: if(( unsigned int) _temp1109 > 2u?(( struct _tunion_struct*)
_temp1109)->tag == Cyc_Absyn_Pointer_p_tag: 0){ _LL1162: _temp1161=( struct Cyc_Absyn_Pat*)((
struct Cyc_Absyn_Pointer_p_struct*) _temp1109)->f1; goto _LL1128;} else{ goto
_LL1129;} _LL1129: if(( unsigned int) _temp1109 > 2u?(( struct _tunion_struct*)
_temp1109)->tag == Cyc_Absyn_Reference_p_tag: 0){ _LL1164: _temp1163=( struct
Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Reference_p_struct*) _temp1109)->f1; goto
_LL1130;} else{ goto _LL1131;} _LL1131: if(( unsigned int) _temp1109 > 2u?((
struct _tunion_struct*) _temp1109)->tag == Cyc_Absyn_UnknownId_p_tag: 0){
_LL1166: _temp1165=( struct _tuple0*)(( struct Cyc_Absyn_UnknownId_p_struct*)
_temp1109)->f1; goto _LL1132;} else{ goto _LL1133;} _LL1133: if(( unsigned int)
_temp1109 > 2u?(( struct _tunion_struct*) _temp1109)->tag == Cyc_Absyn_UnknownCall_p_tag:
0){ _LL1172: _temp1171=( struct _tuple0*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1109)->f1; goto _LL1170; _LL1170: _temp1169=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_p_struct*) _temp1109)->f2; goto _LL1168; _LL1168:
_temp1167=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1109)->f3; goto _LL1134;} else{ goto _LL1135;} _LL1135: if(( unsigned int)
_temp1109 > 2u?(( struct _tunion_struct*) _temp1109)->tag == Cyc_Absyn_UnknownFields_p_tag:
0){ _LL1178: _temp1177=( struct _tuple0*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1109)->f1; goto _LL1176; _LL1176: _temp1175=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownFields_p_struct*) _temp1109)->f2; goto _LL1174; _LL1174:
_temp1173=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1109)->f3; goto _LL1136;} else{ goto _LL1137;} _LL1137: if(( unsigned int)
_temp1109 > 2u?(( struct _tunion_struct*) _temp1109)->tag == Cyc_Absyn_Struct_p_tag:
0){ _LL1186: _temp1185=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1109)->f1; goto _LL1184; _LL1184: _temp1183=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1109)->f2; goto _LL1182; _LL1182:
_temp1181=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1109)->f3; goto _LL1180; _LL1180: _temp1179=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1109)->f4; goto _LL1138;} else{ goto
_LL1139;} _LL1139: if(( unsigned int) _temp1109 > 2u?(( struct _tunion_struct*)
_temp1109)->tag == Cyc_Absyn_Tunion_p_tag: 0){ _LL1196: _temp1195=( struct
_tuple0*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp1109)->f1; goto _LL1194;
_LL1194: _temp1193=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1109)->f2; goto _LL1192; _LL1192: _temp1191=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1109)->f3; goto _LL1190; _LL1190:
_temp1189=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1109)->f4; goto _LL1188; _LL1188: _temp1187=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1109)->f5; goto _LL1140;} else{ goto
_LL1141;} _LL1141: if(( unsigned int) _temp1109 > 2u?(( struct _tunion_struct*)
_temp1109)->tag == Cyc_Absyn_XTunion_p_tag: 0){ _LL1206: _temp1205=( struct
_tuple0*)(( struct Cyc_Absyn_XTunion_p_struct*) _temp1109)->f1; goto _LL1204;
_LL1204: _temp1203=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp1109)->f2; goto _LL1202; _LL1202: _temp1201=( struct Cyc_List_List*)((
struct Cyc_Absyn_XTunion_p_struct*) _temp1109)->f3; goto _LL1200; _LL1200:
_temp1199=( struct Cyc_Absyn_XTuniondecl*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp1109)->f4; goto _LL1198; _LL1198: _temp1197=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_XTunion_p_struct*) _temp1109)->f5; goto _LL1142;} else{ goto
_LL1143;} _LL1143: if(( unsigned int) _temp1109 > 2u?(( struct _tunion_struct*)
_temp1109)->tag == Cyc_Absyn_Enum_p_tag: 0){ _LL1212: _temp1211=( struct _tuple0*)((
struct Cyc_Absyn_Enum_p_struct*) _temp1109)->f1; goto _LL1210; _LL1210:
_temp1209=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp1109)->f2; goto _LL1208; _LL1208: _temp1207=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_p_struct*) _temp1109)->f3; goto _LL1144;} else{ goto
_LL1110;} _LL1112: Cyc_Absyndump_dump_char(( int)'_'); goto _LL1110; _LL1114:
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1213=( char*)"null";
struct _tagged_string _temp1214; _temp1214.curr= _temp1213; _temp1214.base=
_temp1213; _temp1214.last_plus_one= _temp1213 + 5; _temp1214;})); goto _LL1110;
_LL1116: Cyc_Absyndump_dump( xprintf("%d", _temp1145)); goto _LL1110; _LL1118:
Cyc_Absyndump_dump( xprintf("%u",( unsigned int) _temp1149)); goto _LL1110;
_LL1120: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1215=( char*)"'";
struct _tagged_string _temp1216; _temp1216.curr= _temp1215; _temp1216.base=
_temp1215; _temp1216.last_plus_one= _temp1215 + 2; _temp1216;})); Cyc_Absyndump_dump_nospace(
Cyc_Absynpp_char_escape( _temp1153)); Cyc_Absyndump_dump_nospace(( struct
_tagged_string)({ char* _temp1217=( char*)"'"; struct _tagged_string _temp1218;
_temp1218.curr= _temp1217; _temp1218.base= _temp1217; _temp1218.last_plus_one=
_temp1217 + 2; _temp1218;})); goto _LL1110; _LL1122: Cyc_Absyndump_dump(
_temp1155); goto _LL1110; _LL1124: Cyc_Absyndump_dumpqvar( _temp1157->name);
goto _LL1110; _LL1126:(( void(*)( void(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List*
l, struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumppat, _temp1159,( struct
_tagged_string)({ char* _temp1219=( char*)"$("; struct _tagged_string _temp1220;
_temp1220.curr= _temp1219; _temp1220.base= _temp1219; _temp1220.last_plus_one=
_temp1219 + 3; _temp1220;}),( struct _tagged_string)({ char* _temp1221=( char*)")";
struct _tagged_string _temp1222; _temp1222.curr= _temp1221; _temp1222.base=
_temp1221; _temp1222.last_plus_one= _temp1221 + 2; _temp1222;}),( struct
_tagged_string)({ char* _temp1223=( char*)","; struct _tagged_string _temp1224;
_temp1224.curr= _temp1223; _temp1224.base= _temp1223; _temp1224.last_plus_one=
_temp1223 + 2; _temp1224;})); goto _LL1110; _LL1128: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp1225=( char*)"&"; struct _tagged_string _temp1226;
_temp1226.curr= _temp1225; _temp1226.base= _temp1225; _temp1226.last_plus_one=
_temp1225 + 2; _temp1226;})); Cyc_Absyndump_dumppat( _temp1161); goto _LL1110;
_LL1130: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1227=( char*)"*";
struct _tagged_string _temp1228; _temp1228.curr= _temp1227; _temp1228.base=
_temp1227; _temp1228.last_plus_one= _temp1227 + 2; _temp1228;})); Cyc_Absyndump_dumpqvar(
_temp1163->name); goto _LL1110; _LL1132: Cyc_Absyndump_dumpqvar( _temp1165);
goto _LL1110; _LL1134: Cyc_Absyndump_dumpqvar( _temp1171); Cyc_Absyndump_dumptvars(
_temp1169);(( void(*)( void(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List*
l, struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumppat, _temp1167,( struct
_tagged_string)({ char* _temp1229=( char*)"("; struct _tagged_string _temp1230;
_temp1230.curr= _temp1229; _temp1230.base= _temp1229; _temp1230.last_plus_one=
_temp1229 + 2; _temp1230;}),( struct _tagged_string)({ char* _temp1231=( char*)")";
struct _tagged_string _temp1232; _temp1232.curr= _temp1231; _temp1232.base=
_temp1231; _temp1232.last_plus_one= _temp1231 + 2; _temp1232;}),( struct
_tagged_string)({ char* _temp1233=( char*)","; struct _tagged_string _temp1234;
_temp1234.curr= _temp1233; _temp1234.base= _temp1233; _temp1234.last_plus_one=
_temp1233 + 2; _temp1234;})); goto _LL1110; _LL1136: Cyc_Absyndump_dumpqvar(
_temp1177); Cyc_Absyndump_dumptvars( _temp1175);(( void(*)( void(* f)( struct
_tuple8*), struct Cyc_List_List* l, struct _tagged_string start, struct
_tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpdp,
_temp1173,( struct _tagged_string)({ char* _temp1235=( char*)"{"; struct
_tagged_string _temp1236; _temp1236.curr= _temp1235; _temp1236.base= _temp1235;
_temp1236.last_plus_one= _temp1235 + 2; _temp1236;}),( struct _tagged_string)({
char* _temp1237=( char*)"}"; struct _tagged_string _temp1238; _temp1238.curr=
_temp1237; _temp1238.base= _temp1237; _temp1238.last_plus_one= _temp1237 + 2;
_temp1238;}),( struct _tagged_string)({ char* _temp1239=( char*)","; struct
_tagged_string _temp1240; _temp1240.curr= _temp1239; _temp1240.base= _temp1239;
_temp1240.last_plus_one= _temp1239 + 2; _temp1240;})); goto _LL1110; _LL1138:
if( _temp1185->name != 0){ Cyc_Absyndump_dumpqvar(( struct _tuple0*)({ struct
Cyc_Core_Opt* _temp1241= _temp1185->name; if( _temp1241 == 0){ _throw(
Null_Exception);} _temp1241->v;}));} Cyc_Absyndump_dumptvars( _temp1181);(( void(*)(
void(* f)( struct _tuple8*), struct Cyc_List_List* l, struct _tagged_string
start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpdp, _temp1179,( struct _tagged_string)({ char* _temp1242=(
char*)"{"; struct _tagged_string _temp1243; _temp1243.curr= _temp1242; _temp1243.base=
_temp1242; _temp1243.last_plus_one= _temp1242 + 2; _temp1243;}),( struct
_tagged_string)({ char* _temp1244=( char*)"}"; struct _tagged_string _temp1245;
_temp1245.curr= _temp1244; _temp1245.base= _temp1244; _temp1245.last_plus_one=
_temp1244 + 2; _temp1245;}),( struct _tagged_string)({ char* _temp1246=( char*)",";
struct _tagged_string _temp1247; _temp1247.curr= _temp1246; _temp1247.base=
_temp1246; _temp1247.last_plus_one= _temp1246 + 2; _temp1247;})); goto _LL1110;
_LL1140: _temp1205= _temp1195; _temp1203= _temp1193; _temp1201= _temp1191; goto
_LL1142; _LL1142: Cyc_Absyndump_dumpqvar( _temp1205); Cyc_Absyndump_dumptvars(
_temp1203); if( _temp1201 != 0){(( void(*)( void(* f)( struct Cyc_Absyn_Pat*),
struct Cyc_List_List* l, struct _tagged_string start, struct _tagged_string end,
struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumppat,
_temp1201,( struct _tagged_string)({ char* _temp1248=( char*)"("; struct
_tagged_string _temp1249; _temp1249.curr= _temp1248; _temp1249.base= _temp1248;
_temp1249.last_plus_one= _temp1248 + 2; _temp1249;}),( struct _tagged_string)({
char* _temp1250=( char*)")"; struct _tagged_string _temp1251; _temp1251.curr=
_temp1250; _temp1251.base= _temp1250; _temp1251.last_plus_one= _temp1250 + 2;
_temp1251;}),( struct _tagged_string)({ char* _temp1252=( char*)","; struct
_tagged_string _temp1253; _temp1253.curr= _temp1252; _temp1253.base= _temp1252;
_temp1253.last_plus_one= _temp1252 + 2; _temp1253;}));} goto _LL1110; _LL1144:
Cyc_Absyndump_dumpqvar( _temp1211); goto _LL1110; _LL1110:;} void Cyc_Absyndump_dumptunionfield(
struct Cyc_Absyn_Tunionfield* ef){ Cyc_Absyndump_dumpqvar( ef->name); if( ef->typs
!= 0){ Cyc_Absyndump_dumpargs( ef->typs);}} void Cyc_Absyndump_dumptunionfields(
struct Cyc_List_List* fields){(( void(*)( void(* f)( struct Cyc_Absyn_Tunionfield*),
struct Cyc_List_List* l, struct _tagged_string sep)) Cyc_Absyndump_dump_sep)(
Cyc_Absyndump_dumptunionfield, fields,( struct _tagged_string)({ char* _temp1254=(
char*)","; struct _tagged_string _temp1255; _temp1255.curr= _temp1254; _temp1255.base=
_temp1254; _temp1255.last_plus_one= _temp1254 + 2; _temp1255;}));} void Cyc_Absyndump_dumpenumfield(
struct Cyc_Absyn_Enumfield* ef){ Cyc_Absyndump_dumpqvar( ef->name); if( ef->tag
!= 0){ Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1256=( char*)" = ";
struct _tagged_string _temp1257; _temp1257.curr= _temp1256; _temp1257.base=
_temp1256; _temp1257.last_plus_one= _temp1256 + 4; _temp1257;})); Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1258= ef->tag; if( _temp1258
== 0){ _throw( Null_Exception);} _temp1258;}));}} void Cyc_Absyndump_dumpenumfields(
struct Cyc_List_List* fields){(( void(*)( void(* f)( struct Cyc_Absyn_Enumfield*),
struct Cyc_List_List* l, struct _tagged_string sep)) Cyc_Absyndump_dump_sep)(
Cyc_Absyndump_dumpenumfield, fields,( struct _tagged_string)({ char* _temp1259=(
char*)","; struct _tagged_string _temp1260; _temp1260.curr= _temp1259; _temp1260.base=
_temp1259; _temp1260.last_plus_one= _temp1259 + 2; _temp1260;}));} void Cyc_Absyndump_dumpstructfields(
struct Cyc_List_List* fields){ for( 0; fields != 0; fields=({ struct Cyc_List_List*
_temp1261= fields; if( _temp1261 == 0){ _throw( Null_Exception);} _temp1261->tl;})){
struct Cyc_Absyn_Structfield _temp1265; struct Cyc_List_List* _temp1266; struct
Cyc_Core_Opt* _temp1268; void* _temp1270; struct Cyc_Absyn_Tqual _temp1272;
struct _tagged_string* _temp1274; struct Cyc_Absyn_Structfield* _temp1263=(
struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp1262= fields; if(
_temp1262 == 0){ _throw( Null_Exception);} _temp1262->hd;}); _temp1265=*
_temp1263; _LL1275: _temp1274=( struct _tagged_string*) _temp1265.name; goto
_LL1273; _LL1273: _temp1272=( struct Cyc_Absyn_Tqual) _temp1265.tq; goto _LL1271;
_LL1271: _temp1270=( void*) _temp1265.type; goto _LL1269; _LL1269: _temp1268=(
struct Cyc_Core_Opt*) _temp1265.width; goto _LL1267; _LL1267: _temp1266=( struct
Cyc_List_List*) _temp1265.attributes; goto _LL1264; _LL1264:(( void(*)( struct
Cyc_Absyn_Tqual, void*, void(* f)( struct _tagged_string*), struct
_tagged_string*)) Cyc_Absyndump_dumptqtd)( _temp1272, _temp1270, Cyc_Absyndump_dump_str,
_temp1274); Cyc_Absyndump_dumpatts( _temp1266); if( _temp1268 != 0){ Cyc_Absyndump_dump_char((
int)':'); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt*
_temp1276= _temp1268; if( _temp1276 == 0){ _throw( Null_Exception);} _temp1276->v;}));}
Cyc_Absyndump_dump_semi();}} void Cyc_Absyndump_dumptypedefname( struct Cyc_Absyn_Typedefdecl*
td){ Cyc_Absyndump_dumpqvar( td->name); Cyc_Absyndump_dumptvars( td->tvs);}
static void Cyc_Absyndump_dump_atts_qvar( struct Cyc_Absyn_Fndecl* fd){ Cyc_Absyndump_dumpatts(
fd->attributes); Cyc_Absyndump_dumpqvar( fd->name);} void Cyc_Absyndump_dumpdecl(
struct Cyc_Absyn_Decl* d){ void* _temp1277=( void*) d->r; struct Cyc_Absyn_Fndecl*
_temp1303; struct Cyc_Absyn_Structdecl* _temp1305; struct Cyc_Absyn_Uniondecl*
_temp1307; struct Cyc_Absyn_Vardecl* _temp1309; struct Cyc_Absyn_Vardecl
_temp1311; struct Cyc_List_List* _temp1312; struct Cyc_Core_Opt* _temp1314;
struct Cyc_Absyn_Exp* _temp1316; void* _temp1318; struct Cyc_Absyn_Tqual
_temp1320; struct _tuple0* _temp1322; void* _temp1324; struct Cyc_Absyn_Tuniondecl*
_temp1326; struct Cyc_Absyn_XTuniondecl* _temp1328; struct Cyc_Absyn_Enumdecl*
_temp1330; struct Cyc_Absyn_Enumdecl _temp1332; struct Cyc_List_List* _temp1333;
struct _tuple0* _temp1335; void* _temp1337; int _temp1339; struct Cyc_Absyn_Exp*
_temp1341; struct Cyc_Core_Opt* _temp1343; struct Cyc_Core_Opt* _temp1345;
struct Cyc_Absyn_Pat* _temp1347; struct Cyc_Absyn_Typedefdecl* _temp1349; struct
Cyc_List_List* _temp1351; struct _tagged_string* _temp1353; struct Cyc_List_List*
_temp1355; struct _tuple0* _temp1357; struct Cyc_List_List* _temp1359; _LL1279:
if((( struct _tunion_struct*) _temp1277)->tag == Cyc_Absyn_Fn_d_tag){ _LL1304:
_temp1303=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*) _temp1277)->f1;
goto _LL1280;} else{ goto _LL1281;} _LL1281: if((( struct _tunion_struct*)
_temp1277)->tag == Cyc_Absyn_Struct_d_tag){ _LL1306: _temp1305=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_d_struct*) _temp1277)->f1; goto _LL1282;} else{ goto
_LL1283;} _LL1283: if((( struct _tunion_struct*) _temp1277)->tag == Cyc_Absyn_Union_d_tag){
_LL1308: _temp1307=( struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*)
_temp1277)->f1; goto _LL1284;} else{ goto _LL1285;} _LL1285: if((( struct
_tunion_struct*) _temp1277)->tag == Cyc_Absyn_Var_d_tag){ _LL1310: _temp1309=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*) _temp1277)->f1;
_temp1311=* _temp1309; _LL1325: _temp1324=( void*) _temp1311.sc; goto _LL1323;
_LL1323: _temp1322=( struct _tuple0*) _temp1311.name; goto _LL1321; _LL1321:
_temp1320=( struct Cyc_Absyn_Tqual) _temp1311.tq; goto _LL1319; _LL1319:
_temp1318=( void*) _temp1311.type; goto _LL1317; _LL1317: _temp1316=( struct Cyc_Absyn_Exp*)
_temp1311.initializer; goto _LL1315; _LL1315: _temp1314=( struct Cyc_Core_Opt*)
_temp1311.rgn; goto _LL1313; _LL1313: _temp1312=( struct Cyc_List_List*)
_temp1311.attributes; goto _LL1286;} else{ goto _LL1287;} _LL1287: if((( struct
_tunion_struct*) _temp1277)->tag == Cyc_Absyn_Tunion_d_tag){ _LL1327: _temp1326=(
struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_d_struct*) _temp1277)->f1;
goto _LL1288;} else{ goto _LL1289;} _LL1289: if((( struct _tunion_struct*)
_temp1277)->tag == Cyc_Absyn_XTunion_d_tag){ _LL1329: _temp1328=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_d_struct*) _temp1277)->f1; goto _LL1290;} else{ goto
_LL1291;} _LL1291: if((( struct _tunion_struct*) _temp1277)->tag == Cyc_Absyn_Enum_d_tag){
_LL1331: _temp1330=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*)
_temp1277)->f1; _temp1332=* _temp1330; _LL1338: _temp1337=( void*) _temp1332.sc;
goto _LL1336; _LL1336: _temp1335=( struct _tuple0*) _temp1332.name; goto _LL1334;
_LL1334: _temp1333=( struct Cyc_List_List*) _temp1332.fields; goto _LL1292;}
else{ goto _LL1293;} _LL1293: if((( struct _tunion_struct*) _temp1277)->tag ==
Cyc_Absyn_Let_d_tag){ _LL1348: _temp1347=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1277)->f1; goto _LL1346; _LL1346: _temp1345=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Let_d_struct*) _temp1277)->f2; goto _LL1344; _LL1344: _temp1343=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp1277)->f3; goto
_LL1342; _LL1342: _temp1341=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1277)->f4; goto _LL1340; _LL1340: _temp1339=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp1277)->f5; goto _LL1294;} else{ goto _LL1295;} _LL1295: if((( struct
_tunion_struct*) _temp1277)->tag == Cyc_Absyn_Typedef_d_tag){ _LL1350: _temp1349=(
struct Cyc_Absyn_Typedefdecl*)(( struct Cyc_Absyn_Typedef_d_struct*) _temp1277)->f1;
goto _LL1296;} else{ goto _LL1297;} _LL1297: if((( struct _tunion_struct*)
_temp1277)->tag == Cyc_Absyn_Namespace_d_tag){ _LL1354: _temp1353=( struct
_tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*) _temp1277)->f1; goto
_LL1352; _LL1352: _temp1351=( struct Cyc_List_List*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1277)->f2; goto _LL1298;} else{ goto _LL1299;} _LL1299: if((( struct
_tunion_struct*) _temp1277)->tag == Cyc_Absyn_Using_d_tag){ _LL1358: _temp1357=(
struct _tuple0*)(( struct Cyc_Absyn_Using_d_struct*) _temp1277)->f1; goto
_LL1356; _LL1356: _temp1355=( struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*)
_temp1277)->f2; goto _LL1300;} else{ goto _LL1301;} _LL1301: if((( struct
_tunion_struct*) _temp1277)->tag == Cyc_Absyn_ExternC_d_tag){ _LL1360: _temp1359=(
struct Cyc_List_List*)(( struct Cyc_Absyn_ExternC_d_struct*) _temp1277)->f1;
goto _LL1302;} else{ goto _LL1278;} _LL1280: if( _temp1303->is_inline){ Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1361=( char*)"inline"; struct _tagged_string
_temp1362; _temp1362.curr= _temp1361; _temp1362.base= _temp1361; _temp1362.last_plus_one=
_temp1361 + 7; _temp1362;}));} Cyc_Absyndump_dumpscope(( void*) _temp1303->sc);{
void* t=( void*)({ struct Cyc_Absyn_FnType_struct* _temp1364=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1364[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1365; _temp1365.tag= Cyc_Absyn_FnType_tag; _temp1365.f1=({ struct Cyc_Absyn_FnInfo
_temp1366; _temp1366.tvars= _temp1303->tvs; _temp1366.effect= _temp1303->effect;
_temp1366.ret_typ=( void*)(( void*) _temp1303->ret_type); _temp1366.args=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple3*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_arg_mk_opt, _temp1303->args); _temp1366.varargs=
_temp1303->varargs; _temp1366.attributes= 0; _temp1366;}); _temp1365;});
_temp1364;});(( void(*)( struct Cyc_Absyn_Tqual, void*, void(* f)( struct Cyc_Absyn_Fndecl*),
struct Cyc_Absyn_Fndecl*)) Cyc_Absyndump_dumptqtd)(({ struct Cyc_Absyn_Tqual
_temp1363; _temp1363.q_const= 0; _temp1363.q_volatile= 0; _temp1363.q_restrict=
0; _temp1363;}), t, Cyc_Absyndump_dump_atts_qvar, _temp1303); Cyc_Absyndump_dump_char((
int)'{'); Cyc_Absyndump_dumpstmt( _temp1303->body); Cyc_Absyndump_dump_char((
int)'}'); goto _LL1278;} _LL1282: Cyc_Absyndump_dumpscope(( void*) _temp1305->sc);
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1367=( char*)"struct";
struct _tagged_string _temp1368; _temp1368.curr= _temp1367; _temp1368.base=
_temp1367; _temp1368.last_plus_one= _temp1367 + 7; _temp1368;})); if( _temp1305->name
!= 0){ Cyc_Absyndump_dumpqvar(( struct _tuple0*)({ struct Cyc_Core_Opt*
_temp1369= _temp1305->name; if( _temp1369 == 0){ _throw( Null_Exception);}
_temp1369->v;}));} Cyc_Absyndump_dumptvars( _temp1305->tvs); if( _temp1305->fields
== 0){ Cyc_Absyndump_dump_semi();} else{ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstructfields((
struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp1370= _temp1305->fields; if(
_temp1370 == 0){ _throw( Null_Exception);} _temp1370->v;})); Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1371=( char*)"}"; struct _tagged_string
_temp1372; _temp1372.curr= _temp1371; _temp1372.base= _temp1371; _temp1372.last_plus_one=
_temp1371 + 2; _temp1372;})); Cyc_Absyndump_dumpatts( _temp1305->attributes);
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1373=( char*)";";
struct _tagged_string _temp1374; _temp1374.curr= _temp1373; _temp1374.base=
_temp1373; _temp1374.last_plus_one= _temp1373 + 2; _temp1374;}));} goto _LL1278;
_LL1284: Cyc_Absyndump_dumpscope(( void*) _temp1307->sc); Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1375=( char*)"union"; struct _tagged_string
_temp1376; _temp1376.curr= _temp1375; _temp1376.base= _temp1375; _temp1376.last_plus_one=
_temp1375 + 6; _temp1376;})); if( _temp1307->name != 0){ Cyc_Absyndump_dumpqvar((
struct _tuple0*)({ struct Cyc_Core_Opt* _temp1377= _temp1307->name; if(
_temp1377 == 0){ _throw( Null_Exception);} _temp1377->v;}));} Cyc_Absyndump_dumptvars(
_temp1307->tvs); if( _temp1307->fields == 0){ Cyc_Absyndump_dump_semi();} else{
Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstructfields(( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp1378= _temp1307->fields; if( _temp1378 == 0){ _throw(
Null_Exception);} _temp1378->v;})); Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1379=( char*)"}"; struct _tagged_string _temp1380; _temp1380.curr=
_temp1379; _temp1380.base= _temp1379; _temp1380.last_plus_one= _temp1379 + 2;
_temp1380;})); Cyc_Absyndump_dumpatts( _temp1307->attributes); Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1381=( char*)";"; struct _tagged_string
_temp1382; _temp1382.curr= _temp1381; _temp1382.base= _temp1381; _temp1382.last_plus_one=
_temp1381 + 2; _temp1382;}));} goto _LL1278; _LL1286: Cyc_Absyndump_dumpscope(
_temp1324);(( void(*)( struct Cyc_Absyn_Tqual, void*, void(* f)( struct _tuple0*),
struct _tuple0*)) Cyc_Absyndump_dumptqtd)( _temp1320, _temp1318, Cyc_Absyndump_dumpqvar,
_temp1322); Cyc_Absyndump_dumpatts( _temp1312); if( _temp1316 != 0){ Cyc_Absyndump_dump_char((
int)'='); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1383= _temp1316; if( _temp1383 == 0){ _throw( Null_Exception);} _temp1383;}));}
Cyc_Absyndump_dump_semi(); goto _LL1278; _LL1288: Cyc_Absyndump_dumpscope(( void*)
_temp1326->sc); Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1384=(
char*)"tunion "; struct _tagged_string _temp1385; _temp1385.curr= _temp1384;
_temp1385.base= _temp1384; _temp1385.last_plus_one= _temp1384 + 8; _temp1385;}));
if( _temp1326->name != 0){ Cyc_Absyndump_dumpqvar(( struct _tuple0*)({ struct
Cyc_Core_Opt* _temp1386= _temp1326->name; if( _temp1386 == 0){ _throw(
Null_Exception);} _temp1386->v;}));} Cyc_Absyndump_dumptvars( _temp1326->tvs);
if( _temp1326->fields == 0){ Cyc_Absyndump_dump_semi();} else{ Cyc_Absyndump_dump_char((
int)'{'); Cyc_Absyndump_dumptunionfields(( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp1387= _temp1326->fields; if( _temp1387 == 0){ _throw( Null_Exception);}
_temp1387->v;})); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char*
_temp1388=( char*)"};"; struct _tagged_string _temp1389; _temp1389.curr=
_temp1388; _temp1389.base= _temp1388; _temp1389.last_plus_one= _temp1388 + 3;
_temp1389;}));} goto _LL1278; _LL1290: Cyc_Absyndump_dumpscope(( void*)
_temp1328->sc); Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1390=(
char*)"xtunion "; struct _tagged_string _temp1391; _temp1391.curr= _temp1390;
_temp1391.base= _temp1390; _temp1391.last_plus_one= _temp1390 + 9; _temp1391;}));
Cyc_Absyndump_dumpqvar( _temp1328->name); if( _temp1328->fields == 0){ Cyc_Absyndump_dump_semi();}
else{ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumptunionfields(
_temp1328->fields); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char*
_temp1392=( char*)"};"; struct _tagged_string _temp1393; _temp1393.curr=
_temp1392; _temp1393.base= _temp1392; _temp1393.last_plus_one= _temp1392 + 3;
_temp1393;}));} goto _LL1278; _LL1292: Cyc_Absyndump_dumpscope( _temp1337); Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1394=( char*)"enum "; struct _tagged_string
_temp1395; _temp1395.curr= _temp1394; _temp1395.base= _temp1394; _temp1395.last_plus_one=
_temp1394 + 6; _temp1395;})); Cyc_Absyndump_dumpqvar( _temp1335); Cyc_Absyndump_dump_char((
int)'{'); Cyc_Absyndump_dumpenumfields( _temp1333); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp1396=( char*)"};"; struct _tagged_string
_temp1397; _temp1397.curr= _temp1396; _temp1397.base= _temp1396; _temp1397.last_plus_one=
_temp1396 + 3; _temp1397;})); return; _LL1294: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp1398=( char*)"let"; struct _tagged_string _temp1399;
_temp1399.curr= _temp1398; _temp1399.base= _temp1398; _temp1399.last_plus_one=
_temp1398 + 4; _temp1399;})); Cyc_Absyndump_dumppat( _temp1347); Cyc_Absyndump_dump_char((
int)'='); Cyc_Absyndump_dumpexp( _temp1341); Cyc_Absyndump_dump_semi(); goto
_LL1278; _LL1296: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1400=(
char*)"typedef"; struct _tagged_string _temp1401; _temp1401.curr= _temp1400;
_temp1401.base= _temp1400; _temp1401.last_plus_one= _temp1400 + 8; _temp1401;}));((
void(*)( struct Cyc_Absyn_Tqual, void*, void(* f)( struct Cyc_Absyn_Typedefdecl*),
struct Cyc_Absyn_Typedefdecl*)) Cyc_Absyndump_dumptqtd)(({ struct Cyc_Absyn_Tqual
_temp1402; _temp1402.q_const= 0; _temp1402.q_volatile= 0; _temp1402.q_restrict=
0; _temp1402;}),( void*) _temp1349->defn, Cyc_Absyndump_dumptypedefname,
_temp1349); Cyc_Absyndump_dump_semi(); goto _LL1278; _LL1298: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1403=( char*)"namespace"; struct
_tagged_string _temp1404; _temp1404.curr= _temp1403; _temp1404.base= _temp1403;
_temp1404.last_plus_one= _temp1403 + 10; _temp1404;})); Cyc_Absyndump_dump_str(
_temp1353); Cyc_Absyndump_dump_char(( int)'{'); for( 0; _temp1351 != 0;
_temp1351=({ struct Cyc_List_List* _temp1405= _temp1351; if( _temp1405 == 0){
_throw( Null_Exception);} _temp1405->tl;})){ Cyc_Absyndump_dumpdecl(( struct Cyc_Absyn_Decl*)({
struct Cyc_List_List* _temp1406= _temp1351; if( _temp1406 == 0){ _throw(
Null_Exception);} _temp1406->hd;}));} Cyc_Absyndump_dump_char(( int)'}'); goto
_LL1278; _LL1300: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1407=(
char*)"using"; struct _tagged_string _temp1408; _temp1408.curr= _temp1407;
_temp1408.base= _temp1407; _temp1408.last_plus_one= _temp1407 + 6; _temp1408;}));
Cyc_Absyndump_dumpqvar( _temp1357); Cyc_Absyndump_dump_char(( int)'{'); for( 0;
_temp1355 != 0; _temp1355=({ struct Cyc_List_List* _temp1409= _temp1355; if(
_temp1409 == 0){ _throw( Null_Exception);} _temp1409->tl;})){ Cyc_Absyndump_dumpdecl((
struct Cyc_Absyn_Decl*)({ struct Cyc_List_List* _temp1410= _temp1355; if(
_temp1410 == 0){ _throw( Null_Exception);} _temp1410->hd;}));} Cyc_Absyndump_dump_char((
int)'}'); goto _LL1278; _LL1302: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1411=( char*)"extern \"C\" {"; struct _tagged_string _temp1412;
_temp1412.curr= _temp1411; _temp1412.base= _temp1411; _temp1412.last_plus_one=
_temp1411 + 13; _temp1412;})); for( 0; _temp1359 != 0; _temp1359=({ struct Cyc_List_List*
_temp1413= _temp1359; if( _temp1413 == 0){ _throw( Null_Exception);} _temp1413->tl;})){
Cyc_Absyndump_dumpdecl(( struct Cyc_Absyn_Decl*)({ struct Cyc_List_List*
_temp1414= _temp1359; if( _temp1414 == 0){ _throw( Null_Exception);} _temp1414->hd;}));}
Cyc_Absyndump_dump_char(( int)'}'); goto _LL1278; _LL1278:;} static void Cyc_Absyndump_dump_upperbound(
struct Cyc_Absyn_Exp* e){ unsigned int i= Cyc_Evexp_eval_const_uint_exp( e); if(
i != 1){ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpexp( e); Cyc_Absyndump_dump_char((
int)'}');}} void Cyc_Absyndump_dumptms( struct Cyc_List_List* tms, void(* f)(
void*), void* a){ if( tms == 0){ f( a); return;}{ void* _temp1416=( void*)({
struct Cyc_List_List* _temp1415= tms; if( _temp1415 == 0){ _throw(
Null_Exception);} _temp1415->hd;}); struct Cyc_Absyn_Tqual _temp1434; void*
_temp1436; void* _temp1438; struct Cyc_Absyn_Exp* _temp1440; struct Cyc_Absyn_Tqual
_temp1442; void* _temp1444; void* _temp1446; struct Cyc_Absyn_Exp* _temp1448;
struct Cyc_Absyn_Tqual _temp1450; void* _temp1452; void* _temp1454; struct Cyc_Absyn_Tqual
_temp1456; void* _temp1458; struct Cyc_Absyn_Tvar* _temp1460; void* _temp1462;
struct Cyc_Absyn_Exp* _temp1464; struct Cyc_Absyn_Tqual _temp1466; void*
_temp1468; struct Cyc_Absyn_Tvar* _temp1470; void* _temp1472; struct Cyc_Absyn_Exp*
_temp1474; struct Cyc_Absyn_Tqual _temp1476; void* _temp1478; struct Cyc_Absyn_Tvar*
_temp1480; void* _temp1482; struct Cyc_Absyn_Tqual _temp1484; void* _temp1486;
void* _temp1488; _LL1418: if(( unsigned int) _temp1416 > 1u?(( struct
_tunion_struct*) _temp1416)->tag == Cyc_Absyn_Pointer_mod_tag: 0){ _LL1439:
_temp1438=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1416)->f1; if((
unsigned int) _temp1438 > 1u?(( struct _tunion_struct*) _temp1438)->tag == Cyc_Absyn_Nullable_ps_tag:
0){ _LL1441: _temp1440=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Nullable_ps_struct*)
_temp1438)->f1; goto _LL1437;} else{ goto _LL1420;} _LL1437: _temp1436=( void*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1416)->f2; if(( int) _temp1436 == Cyc_Absyn_HeapRgn){
goto _LL1435;} else{ goto _LL1420;} _LL1435: _temp1434=( struct Cyc_Absyn_Tqual)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1416)->f3; goto _LL1419;} else{ goto
_LL1420;} _LL1420: if(( unsigned int) _temp1416 > 1u?(( struct _tunion_struct*)
_temp1416)->tag == Cyc_Absyn_Pointer_mod_tag: 0){ _LL1447: _temp1446=( void*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1416)->f1; if(( unsigned int)
_temp1446 > 1u?(( struct _tunion_struct*) _temp1446)->tag == Cyc_Absyn_NonNullable_ps_tag:
0){ _LL1449: _temp1448=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NonNullable_ps_struct*)
_temp1446)->f1; goto _LL1445;} else{ goto _LL1422;} _LL1445: _temp1444=( void*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1416)->f2; if(( int) _temp1444 == Cyc_Absyn_HeapRgn){
goto _LL1443;} else{ goto _LL1422;} _LL1443: _temp1442=( struct Cyc_Absyn_Tqual)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1416)->f3; goto _LL1421;} else{ goto
_LL1422;} _LL1422: if(( unsigned int) _temp1416 > 1u?(( struct _tunion_struct*)
_temp1416)->tag == Cyc_Absyn_Pointer_mod_tag: 0){ _LL1455: _temp1454=( void*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1416)->f1; if(( int) _temp1454 == Cyc_Absyn_TaggedArray_ps){
goto _LL1453;} else{ goto _LL1424;} _LL1453: _temp1452=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1416)->f2; if(( int) _temp1452 == Cyc_Absyn_HeapRgn){ goto _LL1451;} else{
goto _LL1424;} _LL1451: _temp1450=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1416)->f3; goto _LL1423;} else{ goto _LL1424;} _LL1424: if(( unsigned int)
_temp1416 > 1u?(( struct _tunion_struct*) _temp1416)->tag == Cyc_Absyn_Pointer_mod_tag:
0){ _LL1463: _temp1462=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1416)->f1; if(( unsigned int) _temp1462 > 1u?(( struct _tunion_struct*)
_temp1462)->tag == Cyc_Absyn_Nullable_ps_tag: 0){ _LL1465: _temp1464=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Nullable_ps_struct*) _temp1462)->f1; goto
_LL1459;} else{ goto _LL1426;} _LL1459: _temp1458=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1416)->f2; if(( unsigned int) _temp1458 > 4u?(( struct _tunion_struct*)
_temp1458)->tag == Cyc_Absyn_VarType_tag: 0){ _LL1461: _temp1460=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_VarType_struct*) _temp1458)->f1; goto _LL1457;} else{ goto
_LL1426;} _LL1457: _temp1456=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1416)->f3; goto _LL1425;} else{ goto _LL1426;} _LL1426: if(( unsigned int)
_temp1416 > 1u?(( struct _tunion_struct*) _temp1416)->tag == Cyc_Absyn_Pointer_mod_tag:
0){ _LL1473: _temp1472=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1416)->f1; if(( unsigned int) _temp1472 > 1u?(( struct _tunion_struct*)
_temp1472)->tag == Cyc_Absyn_NonNullable_ps_tag: 0){ _LL1475: _temp1474=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp1472)->f1; goto
_LL1469;} else{ goto _LL1428;} _LL1469: _temp1468=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1416)->f2; if(( unsigned int) _temp1468 > 4u?(( struct _tunion_struct*)
_temp1468)->tag == Cyc_Absyn_VarType_tag: 0){ _LL1471: _temp1470=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_VarType_struct*) _temp1468)->f1; goto _LL1467;} else{ goto
_LL1428;} _LL1467: _temp1466=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1416)->f3; goto _LL1427;} else{ goto _LL1428;} _LL1428: if(( unsigned int)
_temp1416 > 1u?(( struct _tunion_struct*) _temp1416)->tag == Cyc_Absyn_Pointer_mod_tag:
0){ _LL1483: _temp1482=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1416)->f1; if(( int) _temp1482 == Cyc_Absyn_TaggedArray_ps){ goto _LL1479;}
else{ goto _LL1430;} _LL1479: _temp1478=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1416)->f2; if(( unsigned int) _temp1478 > 4u?(( struct _tunion_struct*)
_temp1478)->tag == Cyc_Absyn_VarType_tag: 0){ _LL1481: _temp1480=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_VarType_struct*) _temp1478)->f1; goto _LL1477;} else{ goto
_LL1430;} _LL1477: _temp1476=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1416)->f3; goto _LL1429;} else{ goto _LL1430;} _LL1430: if(( unsigned int)
_temp1416 > 1u?(( struct _tunion_struct*) _temp1416)->tag == Cyc_Absyn_Pointer_mod_tag:
0){ _LL1489: _temp1488=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1416)->f1; goto _LL1487; _LL1487: _temp1486=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1416)->f2; goto _LL1485; _LL1485: _temp1484=( struct Cyc_Absyn_Tqual)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1416)->f3; goto _LL1431;} else{ goto
_LL1432;} _LL1432: goto _LL1433; _LL1419: Cyc_Absyndump_dump_char(( int)'*');
Cyc_Absyndump_dump_upperbound( _temp1440);(( void(*)( struct Cyc_List_List* tms,
void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)(({ struct Cyc_List_List*
_temp1490= tms; if( _temp1490 == 0){ _throw( Null_Exception);} _temp1490->tl;}),
f, a); return; _LL1421: Cyc_Absyndump_dump_char(( int)'@'); Cyc_Absyndump_dump_upperbound(
_temp1448);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a))
Cyc_Absyndump_dumptms)(({ struct Cyc_List_List* _temp1491= tms; if( _temp1491 ==
0){ _throw( Null_Exception);} _temp1491->tl;}), f, a); return; _LL1423: Cyc_Absyndump_dump_char((
int)'?');(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)(({
struct Cyc_List_List* _temp1492= tms; if( _temp1492 == 0){ _throw(
Null_Exception);} _temp1492->tl;}), f, a); return; _LL1425: Cyc_Absyndump_dump_char((
int)'*'); Cyc_Absyndump_dump_upperbound( _temp1464); Cyc_Absyndump_dump_str(
_temp1460->name);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void*
a)) Cyc_Absyndump_dumptms)(({ struct Cyc_List_List* _temp1493= tms; if(
_temp1493 == 0){ _throw( Null_Exception);} _temp1493->tl;}), f, a); return;
_LL1427: Cyc_Absyndump_dump_char(( int)'@'); Cyc_Absyndump_dump_upperbound(
_temp1474); Cyc_Absyndump_dump_str( _temp1470->name);(( void(*)( struct Cyc_List_List*
tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)(({ struct Cyc_List_List*
_temp1494= tms; if( _temp1494 == 0){ _throw( Null_Exception);} _temp1494->tl;}),
f, a); return; _LL1429: Cyc_Absyndump_dump_char(( int)'?'); Cyc_Absyndump_dump_str(
_temp1480->name);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void*
a)) Cyc_Absyndump_dumptms)(({ struct Cyc_List_List* _temp1495= tms; if(
_temp1495 == 0){ _throw( Null_Exception);} _temp1495->tl;}), f, a); return;
_LL1431:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp1496=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1496[ 0]=({ struct Cyc_Core_Impossible_struct _temp1497; _temp1497.tag= Cyc_Core_Impossible_tag;
_temp1497.f1=( struct _tagged_string)({ char* _temp1498=( char*)"dumptms: bad Pointer_mod";
struct _tagged_string _temp1499; _temp1499.curr= _temp1498; _temp1499.base=
_temp1498; _temp1499.last_plus_one= _temp1498 + 25; _temp1499;}); _temp1497;});
_temp1496;})); _LL1433: { int next_is_pointer= 0; if(({ struct Cyc_List_List*
_temp1500= tms; if( _temp1500 == 0){ _throw( Null_Exception);} _temp1500->tl;})
!= 0){ void* _temp1503=( void*)({ struct Cyc_List_List* _temp1502=({ struct Cyc_List_List*
_temp1501= tms; if( _temp1501 == 0){ _throw( Null_Exception);} _temp1501->tl;});
if( _temp1502 == 0){ _throw( Null_Exception);} _temp1502->hd;}); struct Cyc_Absyn_Tqual
_temp1509; void* _temp1511; void* _temp1513; _LL1505: if(( unsigned int)
_temp1503 > 1u?(( struct _tunion_struct*) _temp1503)->tag == Cyc_Absyn_Pointer_mod_tag:
0){ _LL1514: _temp1513=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1503)->f1; goto _LL1512; _LL1512: _temp1511=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1503)->f2; goto _LL1510; _LL1510: _temp1509=( struct Cyc_Absyn_Tqual)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1503)->f3; goto _LL1506;} else{ goto
_LL1507;} _LL1507: goto _LL1508; _LL1506: next_is_pointer= 1; goto _LL1504;
_LL1508: goto _LL1504; _LL1504:;} if( next_is_pointer){ Cyc_Absyndump_dump_char((
int)'(');}(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a))
Cyc_Absyndump_dumptms)(({ struct Cyc_List_List* _temp1515= tms; if( _temp1515 ==
0){ _throw( Null_Exception);} _temp1515->tl;}), f, a); if( next_is_pointer){ Cyc_Absyndump_dump_char((
int)')');}{ void* _temp1517=( void*)({ struct Cyc_List_List* _temp1516= tms; if(
_temp1516 == 0){ _throw( Null_Exception);} _temp1516->hd;}); struct Cyc_Absyn_Exp*
_temp1533; void* _temp1535; struct Cyc_Core_Opt* _temp1537; int _temp1539;
struct Cyc_List_List* _temp1541; void* _temp1543; struct Cyc_Position_Segment*
_temp1545; struct Cyc_List_List* _temp1547; int _temp1549; struct Cyc_Position_Segment*
_temp1551; struct Cyc_List_List* _temp1553; struct Cyc_List_List* _temp1555;
struct Cyc_Position_Segment* _temp1557; struct Cyc_Absyn_Tqual _temp1559; void*
_temp1561; void* _temp1563; _LL1519: if(( int) _temp1517 == Cyc_Absyn_Carray_mod){
goto _LL1520;} else{ goto _LL1521;} _LL1521: if(( unsigned int) _temp1517 > 1u?((
struct _tunion_struct*) _temp1517)->tag == Cyc_Absyn_ConstArray_mod_tag: 0){
_LL1534: _temp1533=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ConstArray_mod_struct*)
_temp1517)->f1; goto _LL1522;} else{ goto _LL1523;} _LL1523: if(( unsigned int)
_temp1517 > 1u?(( struct _tunion_struct*) _temp1517)->tag == Cyc_Absyn_Function_mod_tag:
0){ _LL1536: _temp1535=( void*)(( struct Cyc_Absyn_Function_mod_struct*)
_temp1517)->f1; if((( struct _tunion_struct*) _temp1535)->tag == Cyc_Absyn_WithTypes_tag){
_LL1542: _temp1541=( struct Cyc_List_List*)(( struct Cyc_Absyn_WithTypes_struct*)
_temp1535)->f1; goto _LL1540; _LL1540: _temp1539=( int)(( struct Cyc_Absyn_WithTypes_struct*)
_temp1535)->f2; goto _LL1538; _LL1538: _temp1537=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_WithTypes_struct*) _temp1535)->f3; goto _LL1524;} else{ goto
_LL1525;}} else{ goto _LL1525;} _LL1525: if(( unsigned int) _temp1517 > 1u?((
struct _tunion_struct*) _temp1517)->tag == Cyc_Absyn_Function_mod_tag: 0){
_LL1544: _temp1543=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp1517)->f1;
if((( struct _tunion_struct*) _temp1543)->tag == Cyc_Absyn_NoTypes_tag){ _LL1548:
_temp1547=( struct Cyc_List_List*)(( struct Cyc_Absyn_NoTypes_struct*) _temp1543)->f1;
goto _LL1546; _LL1546: _temp1545=( struct Cyc_Position_Segment*)(( struct Cyc_Absyn_NoTypes_struct*)
_temp1543)->f2; goto _LL1526;} else{ goto _LL1527;}} else{ goto _LL1527;}
_LL1527: if(( unsigned int) _temp1517 > 1u?(( struct _tunion_struct*) _temp1517)->tag
== Cyc_Absyn_TypeParams_mod_tag: 0){ _LL1554: _temp1553=( struct Cyc_List_List*)((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp1517)->f1; goto _LL1552; _LL1552:
_temp1551=( struct Cyc_Position_Segment*)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp1517)->f2; goto _LL1550; _LL1550: _temp1549=( int)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp1517)->f3; goto _LL1528;} else{ goto _LL1529;} _LL1529: if(( unsigned int)
_temp1517 > 1u?(( struct _tunion_struct*) _temp1517)->tag == Cyc_Absyn_Attributes_mod_tag:
0){ _LL1558: _temp1557=( struct Cyc_Position_Segment*)(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp1517)->f1; goto _LL1556; _LL1556: _temp1555=( struct Cyc_List_List*)((
struct Cyc_Absyn_Attributes_mod_struct*) _temp1517)->f2; goto _LL1530;} else{
goto _LL1531;} _LL1531: if(( unsigned int) _temp1517 > 1u?(( struct
_tunion_struct*) _temp1517)->tag == Cyc_Absyn_Pointer_mod_tag: 0){ _LL1564:
_temp1563=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1517)->f1; goto
_LL1562; _LL1562: _temp1561=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1517)->f2; goto _LL1560; _LL1560: _temp1559=( struct Cyc_Absyn_Tqual)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1517)->f3; goto _LL1532;} else{ goto
_LL1518;} _LL1520: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1565=(
char*)"[]"; struct _tagged_string _temp1566; _temp1566.curr= _temp1565;
_temp1566.base= _temp1565; _temp1566.last_plus_one= _temp1565 + 3; _temp1566;}));
goto _LL1518; _LL1522: Cyc_Absyndump_dump_char(( int)'['); Cyc_Absyndump_dumpexp(
_temp1533); Cyc_Absyndump_dump_char(( int)']'); goto _LL1518; _LL1524: Cyc_Absyndump_dumpfunargs(
_temp1541, _temp1539, _temp1537); goto _LL1518; _LL1526:(( void(*)( void(* f)(
struct _tagged_string*), struct Cyc_List_List* l, struct _tagged_string start,
struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dump_str,
_temp1547,( struct _tagged_string)({ char* _temp1567=( char*)"("; struct
_tagged_string _temp1568; _temp1568.curr= _temp1567; _temp1568.base= _temp1567;
_temp1568.last_plus_one= _temp1567 + 2; _temp1568;}),( struct _tagged_string)({
char* _temp1569=( char*)")"; struct _tagged_string _temp1570; _temp1570.curr=
_temp1569; _temp1570.base= _temp1569; _temp1570.last_plus_one= _temp1569 + 2;
_temp1570;}),( struct _tagged_string)({ char* _temp1571=( char*)","; struct
_tagged_string _temp1572; _temp1572.curr= _temp1571; _temp1572.base= _temp1571;
_temp1572.last_plus_one= _temp1571 + 2; _temp1572;})); goto _LL1518; _LL1528:
if( _temp1549){ Cyc_Absyndump_dumpkindedtvars( _temp1553);} else{ Cyc_Absyndump_dumptvars(
_temp1553);} goto _LL1518; _LL1530: Cyc_Absyndump_dumpatts( _temp1555); goto
_LL1518; _LL1532:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp1573=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1573[ 0]=({ struct Cyc_Core_Impossible_struct _temp1574; _temp1574.tag= Cyc_Core_Impossible_tag;
_temp1574.f1=( struct _tagged_string)({ char* _temp1575=( char*)"dumptms";
struct _tagged_string _temp1576; _temp1576.curr= _temp1575; _temp1576.base=
_temp1575; _temp1576.last_plus_one= _temp1575 + 8; _temp1576;}); _temp1574;});
_temp1573;})); _LL1518:;} return;} _LL1417:;}} void Cyc_Absyndump_dumptqtd(
struct Cyc_Absyn_Tqual tq, void* t, void(* f)( void*), void* a){ struct Cyc_List_List*
_temp1579; void* _temp1581; struct Cyc_Absyn_Tqual _temp1583; struct _tuple4
_temp1577= Cyc_Absynpp_to_tms(( struct Cyc_Absyn_Tqual) tq, t); _LL1584:
_temp1583= _temp1577.f1; goto _LL1582; _LL1582: _temp1581= _temp1577.f2; goto
_LL1580; _LL1580: _temp1579= _temp1577.f3; goto _LL1578; _LL1578: Cyc_Absyndump_dumptq(
_temp1583); Cyc_Absyndump_dumpntyp( _temp1581);(( void(*)( struct Cyc_List_List*
tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp1579), f, a);} void Cyc_Absyndump_dumpdecllist2file(
struct Cyc_List_List* tdl, struct Cyc_Stdio___sFILE* f){ Cyc_Absyndump_pos= 0;*
Cyc_Absyndump_dump_file= f; for( 0; tdl != 0; tdl=({ struct Cyc_List_List*
_temp1585= tdl; if( _temp1585 == 0){ _throw( Null_Exception);} _temp1585->tl;})){
Cyc_Absyndump_dumpdecl(( struct Cyc_Absyn_Decl*)({ struct Cyc_List_List*
_temp1586= tdl; if( _temp1586 == 0){ _throw( Null_Exception);} _temp1586->hd;}));}}