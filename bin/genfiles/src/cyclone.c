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
struct Cyc_Core_Opt* Cyc_Core_opt_t; extern char Cyc_Core_InvalidArg[ 15u];
struct Cyc_Core_InvalidArg_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{ char* tag;
struct _tagged_string f1; } ; extern char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Core_Not_found[ 14u];
extern char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{ char*
tag; struct _tagged_string f1; } ; extern char* string_to_Cstring( struct
_tagged_string); extern char* underlying_Cstring( struct _tagged_string); extern
struct _tagged_string Cstring_to_string( char*); extern int system( char*);
struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List*
Cyc_List_glist_t; typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct
Cyc_List_List* Cyc_List_List_t; extern char Cyc_List_List_empty[ 15u]; extern
char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List* Cyc_List_rev(
struct Cyc_List_List* x); extern char Cyc_List_Nth[ 8u]; extern char Cyc_Arg_Bad[
8u]; struct Cyc_Arg_Bad_struct{ char* tag; struct _tagged_string f1; } ; extern
char Cyc_Arg_Error[ 10u]; static const int Cyc_Arg_Unit_spec= 0; struct Cyc_Arg_Unit_spec_struct{
int tag; void(* f1)(); } ; static const int Cyc_Arg_Set_spec= 1; struct Cyc_Arg_Set_spec_struct{
int tag; int* f1; } ; static const int Cyc_Arg_Clear_spec= 2; struct Cyc_Arg_Clear_spec_struct{
int tag; int* f1; } ; static const int Cyc_Arg_String_spec= 3; struct Cyc_Arg_String_spec_struct{
int tag; void(* f1)( struct _tagged_string); } ; static const int Cyc_Arg_Int_spec=
4; struct Cyc_Arg_Int_spec_struct{ int tag; void(* f1)( int); } ; static const
int Cyc_Arg_Rest_spec= 5; struct Cyc_Arg_Rest_spec_struct{ int tag; void(* f1)(
struct _tagged_string); } ; typedef void* Cyc_Arg_gspec_t; typedef void* Cyc_Arg_spec_t;
struct _tagged_ptr0{ struct _tagged_string* curr; struct _tagged_string* base;
struct _tagged_string* last_plus_one; } ; extern void Cyc_Arg_parse( struct Cyc_List_List*
specs, void(* anonfun)( struct _tagged_string), struct _tagged_string errmsg,
struct _tagged_ptr0 args); struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE
Cyc_Stdio_FILE; extern struct Cyc_Stdio___sFILE* Cyc_Stdio_stdout; extern struct
Cyc_Stdio___sFILE* Cyc_Stdio_stderr; typedef int Cyc_Stdio_fpos_t; extern int
Cyc_Stdio_remove( struct _tagged_string); extern int Cyc_Stdio_fclose( struct
Cyc_Stdio___sFILE*); extern int Cyc_Stdio_fflush( struct Cyc_Stdio___sFILE*);
extern char Cyc_Stdio_FileCloseError[ 19u]; extern char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1;
} ; extern struct Cyc_Stdio___sFILE* Cyc_Stdio_file_open( struct _tagged_string
fname, struct _tagged_string mode); extern void Cyc_Stdio_file_close( struct Cyc_Stdio___sFILE*);
extern int Cyc_String_strcmp( struct _tagged_string s1, struct _tagged_string s2);
extern struct _tagged_string Cyc_String_strconcat( struct _tagged_string, struct
_tagged_string); extern struct _tagged_string Cyc_String_str_sepstr( struct Cyc_List_List*,
struct _tagged_string); struct Cyc_Dict_Dict; typedef struct Cyc_Dict_Dict* Cyc_Dict_hdict_t;
typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t; extern char Cyc_Dict_Present[ 12u];
extern char Cyc_Dict_Absent[ 11u]; extern struct _tagged_string Cyc_Filename_chop_extension(
struct _tagged_string); struct Cyc_Id_id; typedef struct Cyc_Id_id* Cyc_Id_Id;
struct Cyc_Lineno_Pos{ struct _tagged_string logical_file; struct _tagged_string
line; int line_no; int col; } ; typedef struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t;
extern char Cyc_Position_Exit[ 9u]; extern void Cyc_Position_reset_position(
struct _tagged_string); struct Cyc_Position_Segment; typedef struct Cyc_Position_Segment*
Cyc_Position_seg_t; static const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse=
1; static const int Cyc_Position_Elab= 2; typedef void* Cyc_Position_error_kind_t;
struct Cyc_Position_Error{ struct _tagged_string source; struct Cyc_Position_Segment*
seg; void* kind; struct _tagged_string desc; } ; typedef struct Cyc_Position_Error*
Cyc_Position_error_t; extern char Cyc_Position_Nocontext[ 14u]; extern int Cyc_Position_error_p();
typedef struct _tagged_string* Cyc_Absyn_field_name_t; typedef struct
_tagged_string* Cyc_Absyn_var_t; typedef struct _tagged_string* Cyc_Absyn_tvarname_t;
typedef void* Cyc_Absyn_nmspace_t; struct _tuple0{ void* f1; struct
_tagged_string* f2; } ; typedef struct _tuple0* Cyc_Absyn_qvar_t; typedef struct
_tuple0* Cyc_Absyn_qvar_opt_t; typedef struct _tuple0* Cyc_Absyn_typedef_name_t;
typedef struct _tuple0* Cyc_Absyn_typedef_name_opt_t; struct Cyc_Absyn_Tvar;
struct Cyc_Absyn_Tqual; struct Cyc_Absyn_Conref; struct Cyc_Absyn_PtrInfo;
struct Cyc_Absyn_FnInfo; struct Cyc_Absyn_TunionInfo; struct Cyc_Absyn_TunionFieldInfo;
struct Cyc_Absyn_Exp; struct Cyc_Absyn_Stmt; struct Cyc_Absyn_Pat; struct Cyc_Absyn_Switch_clause;
struct Cyc_Absyn_Fndecl; struct Cyc_Absyn_Structdecl; struct Cyc_Absyn_Uniondecl;
struct Cyc_Absyn_Tuniondecl; struct Cyc_Absyn_Tunionfield; struct Cyc_Absyn_Enumfield;
struct Cyc_Absyn_Enumdecl; struct Cyc_Absyn_Typedefdecl; struct Cyc_Absyn_Vardecl;
struct Cyc_Absyn_Decl; struct Cyc_Absyn_Structfield; typedef void* Cyc_Absyn_scope_t;
typedef struct Cyc_Absyn_Tqual Cyc_Absyn_tqual_t; typedef void* Cyc_Absyn_size_of_t;
typedef void* Cyc_Absyn_kind_t; typedef struct Cyc_Absyn_Tvar* Cyc_Absyn_tvar_t;
typedef void* Cyc_Absyn_sign_t; typedef struct Cyc_Absyn_Conref* Cyc_Absyn_conref_t;
typedef void* Cyc_Absyn_constraint_t; typedef void* Cyc_Absyn_bounds_t; typedef
struct Cyc_Absyn_PtrInfo Cyc_Absyn_ptr_info_t; typedef struct Cyc_Absyn_FnInfo
Cyc_Absyn_fn_info_t; typedef struct Cyc_Absyn_TunionInfo Cyc_Absyn_tunion_info_t;
typedef struct Cyc_Absyn_TunionFieldInfo Cyc_Absyn_tunion_field_info_t; typedef
void* Cyc_Absyn_type_t; typedef void* Cyc_Absyn_rgntype_t; typedef void* Cyc_Absyn_funcparams_t;
typedef void* Cyc_Absyn_type_modifier_t; typedef void* Cyc_Absyn_cnst_t; typedef
void* Cyc_Absyn_primop_t; typedef void* Cyc_Absyn_incrementor_t; typedef void*
Cyc_Absyn_raw_exp_t; typedef struct Cyc_Absyn_Exp* Cyc_Absyn_exp_t; typedef
struct Cyc_Absyn_Exp* Cyc_Absyn_exp_opt_t; typedef void* Cyc_Absyn_raw_stmt_t;
typedef struct Cyc_Absyn_Stmt* Cyc_Absyn_stmt_t; typedef struct Cyc_Absyn_Stmt*
Cyc_Absyn_stmt_opt_t; typedef void* Cyc_Absyn_raw_pat_t; typedef struct Cyc_Absyn_Pat*
Cyc_Absyn_pat_t; typedef void* Cyc_Absyn_binding_t; typedef struct Cyc_Absyn_Switch_clause*
Cyc_Absyn_switch_clause_t; typedef struct Cyc_Absyn_Fndecl* Cyc_Absyn_fndecl_t;
typedef struct Cyc_Absyn_Structdecl* Cyc_Absyn_structdecl_t; typedef struct Cyc_Absyn_Uniondecl*
Cyc_Absyn_uniondecl_t; typedef struct Cyc_Absyn_Tunionfield* Cyc_Absyn_tunionfield_t;
typedef struct Cyc_Absyn_Tuniondecl* Cyc_Absyn_tuniondecl_t; typedef struct Cyc_Absyn_Typedefdecl*
Cyc_Absyn_typedefdecl_t; typedef struct Cyc_Absyn_Enumfield* Cyc_Absyn_enumfield_t;
typedef struct Cyc_Absyn_Enumdecl* Cyc_Absyn_enumdecl_t; typedef struct Cyc_Absyn_Vardecl*
Cyc_Absyn_vardecl_t; typedef void* Cyc_Absyn_raw_decl_t; typedef struct Cyc_Absyn_Decl*
Cyc_Absyn_decl_t; typedef void* Cyc_Absyn_designator_t; typedef void* Cyc_Absyn_stmt_annot_t;
typedef void* Cyc_Absyn_attribute_t; typedef struct Cyc_List_List* Cyc_Absyn_attributes_t;
typedef struct Cyc_Absyn_Structfield* Cyc_Absyn_structfield_t; static const int
Cyc_Absyn_Loc_n= 0; static const int Cyc_Absyn_Rel_n= 0; struct Cyc_Absyn_Rel_n_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Abs_n= 1;
struct Cyc_Absyn_Abs_n_struct{ int tag; struct Cyc_List_List* f1; } ; static
const int Cyc_Absyn_Static= 0; static const int Cyc_Absyn_Abstract= 1; static
const int Cyc_Absyn_Public= 2; static const int Cyc_Absyn_Extern= 3; static
const int Cyc_Absyn_ExternC= 4; struct Cyc_Absyn_Tqual{ int q_const: 1; int
q_volatile: 1; int q_restrict: 1; } ; static const int Cyc_Absyn_B1= 0; static
const int Cyc_Absyn_B2= 1; static const int Cyc_Absyn_B4= 2; static const int
Cyc_Absyn_B8= 3; static const int Cyc_Absyn_AnyKind= 0; static const int Cyc_Absyn_MemKind=
1; static const int Cyc_Absyn_BoxKind= 2; static const int Cyc_Absyn_RgnKind= 3;
static const int Cyc_Absyn_EffKind= 4; static const int Cyc_Absyn_Signed= 0;
static const int Cyc_Absyn_Unsigned= 1; struct Cyc_Absyn_Conref{ void* v; } ;
static const int Cyc_Absyn_Eq_constr= 0; struct Cyc_Absyn_Eq_constr_struct{ int
tag; void* f1; } ; static const int Cyc_Absyn_Forward_constr= 1; struct Cyc_Absyn_Forward_constr_struct{
int tag; struct Cyc_Absyn_Conref* f1; } ; static const int Cyc_Absyn_No_constr=
0; struct Cyc_Absyn_Tvar{ struct _tagged_string* name; struct Cyc_Absyn_Conref*
kind; } ; static const int Cyc_Absyn_Unknown_b= 0; static const int Cyc_Absyn_Upper_b=
0; struct Cyc_Absyn_Upper_b_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
struct Cyc_Absyn_PtrInfo{ void* elt_typ; void* rgn_typ; struct Cyc_Absyn_Conref*
nullable; struct Cyc_Absyn_Tqual tq; struct Cyc_Absyn_Conref* bounds; } ; struct
Cyc_Absyn_FnInfo{ struct Cyc_List_List* tvars; struct Cyc_Core_Opt* effect; void*
ret_typ; struct Cyc_List_List* args; int varargs; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_UnknownTunionInfo{ struct _tuple0* name; int
is_xtunion; } ; static const int Cyc_Absyn_UnknownTunion= 0; struct Cyc_Absyn_UnknownTunion_struct{
int tag; struct Cyc_Absyn_UnknownTunionInfo f1; } ; static const int Cyc_Absyn_KnownTunion=
1; struct Cyc_Absyn_KnownTunion_struct{ int tag; struct Cyc_Absyn_Tuniondecl* f1;
} ; struct Cyc_Absyn_TunionInfo{ void* tunion_info; struct Cyc_List_List* targs;
void* rgn; } ; struct Cyc_Absyn_UnknownTunionFieldInfo{ struct _tuple0*
tunion_name; struct _tuple0* field_name; int is_xtunion; } ; static const int
Cyc_Absyn_UnknownTunionfield= 0; struct Cyc_Absyn_UnknownTunionfield_struct{ int
tag; struct Cyc_Absyn_UnknownTunionFieldInfo f1; } ; static const int Cyc_Absyn_KnownTunionfield=
1; struct Cyc_Absyn_KnownTunionfield_struct{ int tag; struct Cyc_Absyn_Tuniondecl*
f1; struct Cyc_Absyn_Tunionfield* f2; } ; struct Cyc_Absyn_TunionFieldInfo{ void*
field_info; struct Cyc_List_List* targs; } ; static const int Cyc_Absyn_VoidType=
0; static const int Cyc_Absyn_Evar= 0; struct Cyc_Absyn_Evar_struct{ int tag;
void* f1; struct Cyc_Core_Opt* f2; int f3; } ; static const int Cyc_Absyn_VarType=
1; struct Cyc_Absyn_VarType_struct{ int tag; struct Cyc_Absyn_Tvar* f1; } ;
static const int Cyc_Absyn_TunionType= 2; struct Cyc_Absyn_TunionType_struct{
int tag; struct Cyc_Absyn_TunionInfo f1; } ; static const int Cyc_Absyn_TunionFieldType=
3; struct Cyc_Absyn_TunionFieldType_struct{ int tag; struct Cyc_Absyn_TunionFieldInfo
f1; } ; static const int Cyc_Absyn_PointerType= 4; struct Cyc_Absyn_PointerType_struct{
int tag; struct Cyc_Absyn_PtrInfo f1; } ; static const int Cyc_Absyn_IntType= 5;
struct Cyc_Absyn_IntType_struct{ int tag; void* f1; void* f2; } ; static const
int Cyc_Absyn_FloatType= 1; static const int Cyc_Absyn_DoubleType= 2; static
const int Cyc_Absyn_ArrayType= 6; struct Cyc_Absyn_ArrayType_struct{ int tag;
void* f1; struct Cyc_Absyn_Tqual f2; struct Cyc_Absyn_Exp* f3; } ; static const
int Cyc_Absyn_FnType= 7; struct Cyc_Absyn_FnType_struct{ int tag; struct Cyc_Absyn_FnInfo
f1; } ; static const int Cyc_Absyn_TupleType= 8; struct Cyc_Absyn_TupleType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_StructType= 9;
struct Cyc_Absyn_StructType_struct{ int tag; struct _tuple0* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Structdecl** f3; } ; static const int Cyc_Absyn_UnionType=
10; struct Cyc_Absyn_UnionType_struct{ int tag; struct _tuple0* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Uniondecl** f3; } ; static const int Cyc_Absyn_AnonStructType=
11; struct Cyc_Absyn_AnonStructType_struct{ int tag; struct Cyc_List_List* f1; }
; static const int Cyc_Absyn_AnonUnionType= 12; struct Cyc_Absyn_AnonUnionType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_EnumType= 13;
struct Cyc_Absyn_EnumType_struct{ int tag; struct _tuple0* f1; struct Cyc_Absyn_Enumdecl*
f2; } ; static const int Cyc_Absyn_RgnHandleType= 14; struct Cyc_Absyn_RgnHandleType_struct{
int tag; void* f1; } ; static const int Cyc_Absyn_TypedefType= 15; struct Cyc_Absyn_TypedefType_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3;
} ; static const int Cyc_Absyn_HeapRgn= 3; static const int Cyc_Absyn_AccessEff=
16; struct Cyc_Absyn_AccessEff_struct{ int tag; void* f1; } ; static const int
Cyc_Absyn_JoinEff= 17; struct Cyc_Absyn_JoinEff_struct{ int tag; struct Cyc_List_List*
f1; } ; static const int Cyc_Absyn_NoTypes= 0; struct Cyc_Absyn_NoTypes_struct{
int tag; struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; } ; static
const int Cyc_Absyn_WithTypes= 1; struct Cyc_Absyn_WithTypes_struct{ int tag;
struct Cyc_List_List* f1; int f2; struct Cyc_Core_Opt* f3; } ; static const int
Cyc_Absyn_NonNullable_ps= 0; struct Cyc_Absyn_NonNullable_ps_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_Nullable_ps= 1; struct
Cyc_Absyn_Nullable_ps_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; static
const int Cyc_Absyn_TaggedArray_ps= 0; static const int Cyc_Absyn_Regparm_att= 0;
struct Cyc_Absyn_Regparm_att_struct{ int tag; int f1; } ; static const int Cyc_Absyn_Stdcall_att=
0; static const int Cyc_Absyn_Cdecl_att= 1; static const int Cyc_Absyn_Noreturn_att=
2; static const int Cyc_Absyn_Const_att= 3; static const int Cyc_Absyn_Aligned_att=
1; struct Cyc_Absyn_Aligned_att_struct{ int tag; int f1; } ; static const int
Cyc_Absyn_Packed_att= 4; static const int Cyc_Absyn_Section_att= 2; struct Cyc_Absyn_Section_att_struct{
int tag; struct _tagged_string f1; } ; static const int Cyc_Absyn_Nocommon_att=
5; static const int Cyc_Absyn_Shared_att= 6; static const int Cyc_Absyn_Unused_att=
7; static const int Cyc_Absyn_Weak_att= 8; static const int Cyc_Absyn_Dllimport_att=
9; static const int Cyc_Absyn_Dllexport_att= 10; static const int Cyc_Absyn_No_instrument_function_att=
11; static const int Cyc_Absyn_Constructor_att= 12; static const int Cyc_Absyn_Destructor_att=
13; static const int Cyc_Absyn_No_check_memory_usage_att= 14; static const int
Cyc_Absyn_Carray_mod= 0; static const int Cyc_Absyn_ConstArray_mod= 0; struct
Cyc_Absyn_ConstArray_mod_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; static
const int Cyc_Absyn_Pointer_mod= 1; struct Cyc_Absyn_Pointer_mod_struct{ int tag;
void* f1; void* f2; struct Cyc_Absyn_Tqual f3; } ; static const int Cyc_Absyn_Function_mod=
2; struct Cyc_Absyn_Function_mod_struct{ int tag; void* f1; } ; static const int
Cyc_Absyn_TypeParams_mod= 3; struct Cyc_Absyn_TypeParams_mod_struct{ int tag;
struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; int f3; } ; static
const int Cyc_Absyn_Attributes_mod= 4; struct Cyc_Absyn_Attributes_mod_struct{
int tag; struct Cyc_Position_Segment* f1; struct Cyc_List_List* f2; } ; static
const int Cyc_Absyn_Char_c= 0; struct Cyc_Absyn_Char_c_struct{ int tag; void* f1;
char f2; } ; static const int Cyc_Absyn_Short_c= 1; struct Cyc_Absyn_Short_c_struct{
int tag; void* f1; short f2; } ; static const int Cyc_Absyn_Int_c= 2; struct Cyc_Absyn_Int_c_struct{
int tag; void* f1; int f2; } ; static const int Cyc_Absyn_LongLong_c= 3; struct
Cyc_Absyn_LongLong_c_struct{ int tag; void* f1; long long f2; } ; static const
int Cyc_Absyn_Float_c= 4; struct Cyc_Absyn_Float_c_struct{ int tag; struct
_tagged_string f1; } ; static const int Cyc_Absyn_String_c= 5; struct Cyc_Absyn_String_c_struct{
int tag; struct _tagged_string f1; } ; static const int Cyc_Absyn_Null_c= 0;
static const int Cyc_Absyn_Plus= 0; static const int Cyc_Absyn_Times= 1; static
const int Cyc_Absyn_Minus= 2; static const int Cyc_Absyn_Div= 3; static const
int Cyc_Absyn_Mod= 4; static const int Cyc_Absyn_Eq= 5; static const int Cyc_Absyn_Neq=
6; static const int Cyc_Absyn_Gt= 7; static const int Cyc_Absyn_Lt= 8; static
const int Cyc_Absyn_Gte= 9; static const int Cyc_Absyn_Lte= 10; static const int
Cyc_Absyn_Not= 11; static const int Cyc_Absyn_Bitnot= 12; static const int Cyc_Absyn_Bitand=
13; static const int Cyc_Absyn_Bitor= 14; static const int Cyc_Absyn_Bitxor= 15;
static const int Cyc_Absyn_Bitlshift= 16; static const int Cyc_Absyn_Bitlrshift=
17; static const int Cyc_Absyn_Bitarshift= 18; static const int Cyc_Absyn_Size=
19; static const int Cyc_Absyn_Printf= 20; static const int Cyc_Absyn_Fprintf=
21; static const int Cyc_Absyn_Xprintf= 22; static const int Cyc_Absyn_Scanf= 23;
static const int Cyc_Absyn_Fscanf= 24; static const int Cyc_Absyn_Sscanf= 25;
static const int Cyc_Absyn_PreInc= 0; static const int Cyc_Absyn_PostInc= 1;
static const int Cyc_Absyn_PreDec= 2; static const int Cyc_Absyn_PostDec= 3;
static const int Cyc_Absyn_Const_e= 0; struct Cyc_Absyn_Const_e_struct{ int tag;
void* f1; } ; static const int Cyc_Absyn_Var_e= 1; struct Cyc_Absyn_Var_e_struct{
int tag; struct _tuple0* f1; void* f2; } ; static const int Cyc_Absyn_UnknownId_e=
2; struct Cyc_Absyn_UnknownId_e_struct{ int tag; struct _tuple0* f1; } ; static
const int Cyc_Absyn_Primop_e= 3; struct Cyc_Absyn_Primop_e_struct{ int tag; void*
f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_AssignOp_e= 4;
struct Cyc_Absyn_AssignOp_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
Cyc_Core_Opt* f2; struct Cyc_Absyn_Exp* f3; } ; static const int Cyc_Absyn_Increment_e=
5; struct Cyc_Absyn_Increment_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; void*
f2; } ; static const int Cyc_Absyn_Conditional_e= 6; struct Cyc_Absyn_Conditional_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp*
f3; } ; static const int Cyc_Absyn_SeqExp_e= 7; struct Cyc_Absyn_SeqExp_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp* f2; } ; static const
int Cyc_Absyn_UnknownCall_e= 8; struct Cyc_Absyn_UnknownCall_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_FnCall_e=
9; struct Cyc_Absyn_FnCall_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
Cyc_List_List* f2; } ; static const int Cyc_Absyn_Throw_e= 10; struct Cyc_Absyn_Throw_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_NoInstantiate_e=
11; struct Cyc_Absyn_NoInstantiate_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
} ; static const int Cyc_Absyn_Instantiate_e= 12; struct Cyc_Absyn_Instantiate_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_Cast_e= 13; struct Cyc_Absyn_Cast_e_struct{ int tag; void* f1;
struct Cyc_Absyn_Exp* f2; } ; static const int Cyc_Absyn_Address_e= 14; struct
Cyc_Absyn_Address_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; static const
int Cyc_Absyn_New_e= 15; struct Cyc_Absyn_New_e_struct{ int tag; struct Cyc_Absyn_Exp*
f1; struct Cyc_Absyn_Exp* f2; } ; static const int Cyc_Absyn_Sizeoftyp_e= 16;
struct Cyc_Absyn_Sizeoftyp_e_struct{ int tag; void* f1; } ; static const int Cyc_Absyn_Sizeofexp_e=
17; struct Cyc_Absyn_Sizeofexp_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_Deref_e= 18; struct Cyc_Absyn_Deref_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_StructMember_e= 19;
struct Cyc_Absyn_StructMember_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct _tagged_string* f2; } ; static const int Cyc_Absyn_StructArrow_e= 20;
struct Cyc_Absyn_StructArrow_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
_tagged_string* f2; } ; static const int Cyc_Absyn_Subscript_e= 21; struct Cyc_Absyn_Subscript_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp* f2; } ; static const
int Cyc_Absyn_Tuple_e= 22; struct Cyc_Absyn_Tuple_e_struct{ int tag; struct Cyc_List_List*
f1; } ; static const int Cyc_Absyn_CompoundLit_e= 23; struct _tuple1{ struct Cyc_Core_Opt*
f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; struct Cyc_Absyn_CompoundLit_e_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Array_e=
24; struct Cyc_Absyn_Array_e_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_Comprehension_e= 25; struct Cyc_Absyn_Comprehension_e_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp*
f3; } ; static const int Cyc_Absyn_Struct_e= 26; struct Cyc_Absyn_Struct_e_struct{
int tag; struct _tuple0* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List* f3;
struct Cyc_Absyn_Structdecl* f4; } ; static const int Cyc_Absyn_AnonStruct_e= 27;
struct Cyc_Absyn_AnonStruct_e_struct{ int tag; void* f1; struct Cyc_List_List*
f2; } ; static const int Cyc_Absyn_Tunion_e= 28; struct Cyc_Absyn_Tunion_e_struct{
int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List*
f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield* f5; } ;
static const int Cyc_Absyn_Enum_e= 29; struct Cyc_Absyn_Enum_e_struct{ int tag;
struct _tuple0* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
f3; } ; static const int Cyc_Absyn_Malloc_e= 30; struct Cyc_Absyn_Malloc_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; void* f2; } ; static const int Cyc_Absyn_UnresolvedMem_e=
31; struct Cyc_Absyn_UnresolvedMem_e_struct{ int tag; struct Cyc_Core_Opt* f1;
struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_StmtExp_e= 32; struct
Cyc_Absyn_StmtExp_e_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; static const
int Cyc_Absyn_Codegen_e= 33; struct Cyc_Absyn_Codegen_e_struct{ int tag; struct
Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Fill_e= 34; struct Cyc_Absyn_Fill_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; struct Cyc_Absyn_Exp{ struct Cyc_Core_Opt*
topt; void* r; struct Cyc_Position_Segment* loc; } ; static const int Cyc_Absyn_Skip_s=
0; static const int Cyc_Absyn_Exp_s= 0; struct Cyc_Absyn_Exp_s_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_Seq_s= 1; struct Cyc_Absyn_Seq_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; struct Cyc_Absyn_Stmt* f2; } ; static const
int Cyc_Absyn_Return_s= 2; struct Cyc_Absyn_Return_s_struct{ int tag; struct Cyc_Absyn_Exp*
f1; } ; static const int Cyc_Absyn_IfThenElse_s= 3; struct Cyc_Absyn_IfThenElse_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; struct Cyc_Absyn_Stmt*
f3; } ; static const int Cyc_Absyn_While_s= 4; struct _tuple2{ struct Cyc_Absyn_Exp*
f1; struct Cyc_Absyn_Stmt* f2; } ; struct Cyc_Absyn_While_s_struct{ int tag;
struct _tuple2 f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Break_s=
5; struct Cyc_Absyn_Break_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ;
static const int Cyc_Absyn_Continue_s= 6; struct Cyc_Absyn_Continue_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Absyn_Goto_s= 7;
struct Cyc_Absyn_Goto_s_struct{ int tag; struct _tagged_string* f1; struct Cyc_Absyn_Stmt*
f2; } ; static const int Cyc_Absyn_For_s= 8; struct Cyc_Absyn_For_s_struct{ int
tag; struct Cyc_Absyn_Exp* f1; struct _tuple2 f2; struct _tuple2 f3; struct Cyc_Absyn_Stmt*
f4; } ; static const int Cyc_Absyn_Switch_s= 9; struct Cyc_Absyn_Switch_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_Fallthru_s= 10; struct Cyc_Absyn_Fallthru_s_struct{ int tag;
struct Cyc_List_List* f1; struct Cyc_Absyn_Switch_clause** f2; } ; static const
int Cyc_Absyn_Decl_s= 11; struct Cyc_Absyn_Decl_s_struct{ int tag; struct Cyc_Absyn_Decl*
f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Cut_s= 12; struct
Cyc_Absyn_Cut_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; static const int
Cyc_Absyn_Splice_s= 13; struct Cyc_Absyn_Splice_s_struct{ int tag; struct Cyc_Absyn_Stmt*
f1; } ; static const int Cyc_Absyn_Label_s= 14; struct Cyc_Absyn_Label_s_struct{
int tag; struct _tagged_string* f1; struct Cyc_Absyn_Stmt* f2; } ; static const
int Cyc_Absyn_Do_s= 15; struct Cyc_Absyn_Do_s_struct{ int tag; struct Cyc_Absyn_Stmt*
f1; struct _tuple2 f2; } ; static const int Cyc_Absyn_TryCatch_s= 16; struct Cyc_Absyn_TryCatch_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_Region_s= 17; struct Cyc_Absyn_Region_s_struct{ int tag; struct
Cyc_Absyn_Tvar* f1; struct Cyc_Absyn_Vardecl* f2; struct Cyc_Absyn_Stmt* f3; } ;
struct Cyc_Absyn_Stmt{ void* r; struct Cyc_Position_Segment* loc; struct Cyc_List_List*
non_local_preds; int try_depth; void* annot; } ; static const int Cyc_Absyn_Wild_p=
0; static const int Cyc_Absyn_Var_p= 0; struct Cyc_Absyn_Var_p_struct{ int tag;
struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_Absyn_Null_p= 1; static
const int Cyc_Absyn_Int_p= 1; struct Cyc_Absyn_Int_p_struct{ int tag; void* f1;
int f2; } ; static const int Cyc_Absyn_Char_p= 2; struct Cyc_Absyn_Char_p_struct{
int tag; char f1; } ; static const int Cyc_Absyn_Float_p= 3; struct Cyc_Absyn_Float_p_struct{
int tag; struct _tagged_string f1; } ; static const int Cyc_Absyn_Tuple_p= 4;
struct Cyc_Absyn_Tuple_p_struct{ int tag; struct Cyc_List_List* f1; } ; static
const int Cyc_Absyn_Pointer_p= 5; struct Cyc_Absyn_Pointer_p_struct{ int tag;
struct Cyc_Absyn_Pat* f1; } ; static const int Cyc_Absyn_Reference_p= 6; struct
Cyc_Absyn_Reference_p_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ; static
const int Cyc_Absyn_Struct_p= 7; struct Cyc_Absyn_Struct_p_struct{ int tag;
struct Cyc_Absyn_Structdecl* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List*
f3; struct Cyc_List_List* f4; } ; static const int Cyc_Absyn_Tunion_p= 8; struct
Cyc_Absyn_Tunion_p_struct{ int tag; struct Cyc_Absyn_Tuniondecl* f1; struct Cyc_Absyn_Tunionfield*
f2; struct Cyc_List_List* f3; struct Cyc_List_List* f4; } ; static const int Cyc_Absyn_Enum_p=
9; struct Cyc_Absyn_Enum_p_struct{ int tag; struct Cyc_Absyn_Enumdecl* f1;
struct Cyc_Absyn_Enumfield* f2; } ; static const int Cyc_Absyn_UnknownId_p= 10;
struct Cyc_Absyn_UnknownId_p_struct{ int tag; struct _tuple0* f1; } ; static
const int Cyc_Absyn_UnknownCall_p= 11; struct Cyc_Absyn_UnknownCall_p_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
} ; static const int Cyc_Absyn_UnknownFields_p= 12; struct Cyc_Absyn_UnknownFields_p_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
} ; struct Cyc_Absyn_Pat{ void* r; struct Cyc_Core_Opt* topt; struct Cyc_Position_Segment*
loc; } ; struct Cyc_Absyn_Switch_clause{ struct Cyc_Absyn_Pat* pattern; struct
Cyc_Core_Opt* pat_vars; struct Cyc_Absyn_Exp* where_clause; struct Cyc_Absyn_Stmt*
body; struct Cyc_Position_Segment* loc; } ; static const int Cyc_Absyn_Unresolved_b=
0; static const int Cyc_Absyn_Global_b= 0; struct Cyc_Absyn_Global_b_struct{ int
tag; struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_Absyn_Funname_b= 1;
struct Cyc_Absyn_Funname_b_struct{ int tag; struct Cyc_Absyn_Fndecl* f1; } ;
static const int Cyc_Absyn_Param_b= 2; struct Cyc_Absyn_Param_b_struct{ int tag;
struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_Absyn_Local_b= 3; struct
Cyc_Absyn_Local_b_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ; static
const int Cyc_Absyn_Pat_b= 4; struct Cyc_Absyn_Pat_b_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; } ; struct Cyc_Absyn_Vardecl{ void* sc; struct _tuple0* name; struct Cyc_Absyn_Tqual
tq; void* type; struct Cyc_Absyn_Exp* initializer; struct Cyc_Core_Opt* rgn;
struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Fndecl{ void* sc; int
is_inline; struct _tuple0* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
effect; void* ret_type; struct Cyc_List_List* args; int varargs; struct Cyc_Absyn_Stmt*
body; struct Cyc_Core_Opt* cached_typ; struct Cyc_Core_Opt* param_vardecls;
struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Structfield{ struct
_tagged_string* name; struct Cyc_Absyn_Tqual tq; void* type; struct Cyc_Core_Opt*
width; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Structdecl{ void*
sc; struct Cyc_Core_Opt* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
fields; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Uniondecl{ void*
sc; struct Cyc_Core_Opt* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
fields; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Tunionfield{
struct _tuple0* name; struct Cyc_List_List* tvs; struct Cyc_List_List* typs;
struct Cyc_Position_Segment* loc; void* sc; } ; struct Cyc_Absyn_Tuniondecl{
void* sc; struct _tuple0* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
fields; int is_xtunion; } ; struct Cyc_Absyn_Enumfield{ struct _tuple0* name;
struct Cyc_Absyn_Exp* tag; struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_Enumdecl{
void* sc; struct _tuple0* name; struct Cyc_Core_Opt* fields; } ; struct Cyc_Absyn_Typedefdecl{
struct _tuple0* name; struct Cyc_List_List* tvs; void* defn; } ; static const
int Cyc_Absyn_Var_d= 0; struct Cyc_Absyn_Var_d_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; } ; static const int Cyc_Absyn_Fn_d= 1; struct Cyc_Absyn_Fn_d_struct{ int
tag; struct Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Let_d= 2;
struct Cyc_Absyn_Let_d_struct{ int tag; struct Cyc_Absyn_Pat* f1; struct Cyc_Core_Opt*
f2; struct Cyc_Core_Opt* f3; struct Cyc_Absyn_Exp* f4; int f5; } ; static const
int Cyc_Absyn_Struct_d= 3; struct Cyc_Absyn_Struct_d_struct{ int tag; struct Cyc_Absyn_Structdecl*
f1; } ; static const int Cyc_Absyn_Union_d= 4; struct Cyc_Absyn_Union_d_struct{
int tag; struct Cyc_Absyn_Uniondecl* f1; } ; static const int Cyc_Absyn_Tunion_d=
5; struct Cyc_Absyn_Tunion_d_struct{ int tag; struct Cyc_Absyn_Tuniondecl* f1; }
; static const int Cyc_Absyn_Enum_d= 6; struct Cyc_Absyn_Enum_d_struct{ int tag;
struct Cyc_Absyn_Enumdecl* f1; } ; static const int Cyc_Absyn_Typedef_d= 7;
struct Cyc_Absyn_Typedef_d_struct{ int tag; struct Cyc_Absyn_Typedefdecl* f1; }
; static const int Cyc_Absyn_Namespace_d= 8; struct Cyc_Absyn_Namespace_d_struct{
int tag; struct _tagged_string* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_Using_d= 9; struct Cyc_Absyn_Using_d_struct{ int tag; struct
_tuple0* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d=
10; struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ;
struct Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc; } ; static
const int Cyc_Absyn_ArrayElement= 0; struct Cyc_Absyn_ArrayElement_struct{ int
tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_FieldName= 1;
struct Cyc_Absyn_FieldName_struct{ int tag; struct _tagged_string* f1; } ;
extern char Cyc_Absyn_EmptyAnnot[ 15u]; extern char Cyc_Lexing_Error[ 10u];
struct Cyc_Lexing_Error_struct{ char* tag; struct _tagged_string f1; } ; struct
Cyc_Lexing_lexbuf{ void(* refill_buff)( struct Cyc_Lexing_lexbuf*); void*
refill_state; struct _tagged_string lex_buffer; int lex_buffer_len; int
lex_abs_pos; int lex_start_pos; int lex_curr_pos; int lex_last_pos; int
lex_last_action; int lex_eof_reached; } ; typedef struct Cyc_Lexing_lexbuf* Cyc_Lexing_Lexbuf;
struct Cyc_Lexing_function_lexbuf_state{ int(* read_fun)( struct _tagged_string,
int, void*); void* read_fun_state; } ; typedef struct Cyc_Lexing_function_lexbuf_state*
Cyc_Lexing_Function_lexbuf_state; struct _tagged_ptr1{ int* curr; int* base; int*
last_plus_one; } ; struct Cyc_Lexing_lex_tables{ struct _tagged_ptr1 lex_base;
struct _tagged_ptr1 lex_backtrk; struct _tagged_ptr1 lex_default; struct
_tagged_ptr1 lex_trans; struct _tagged_ptr1 lex_check; } ; typedef struct Cyc_Lexing_lex_tables*
Cyc_Lexing_LexTables; extern struct Cyc_List_List* Cyc_Parse_parse_file( struct
Cyc_Stdio___sFILE* f); typedef void* Cyc_struct_or_union_t; typedef void* Cyc_blockitem_t;
typedef void* Cyc_type_specifier_t; typedef void* Cyc_storage_class_t; struct
Cyc_Declaration_spec; typedef struct Cyc_Declaration_spec* Cyc_decl_spec_t;
struct Cyc_Declarator; typedef struct Cyc_Declarator* Cyc_declarator_t; struct
Cyc_Abstractdeclarator; typedef struct Cyc_Abstractdeclarator* Cyc_abstractdeclarator_t;
extern char Cyc_AbstractDeclarator_tok[ 27u]; struct Cyc_AbstractDeclarator_tok_struct{
char* tag; struct Cyc_Abstractdeclarator* f1; } ; extern char Cyc_AttributeList_tok[
22u]; struct Cyc_AttributeList_tok_struct{ char* tag; struct Cyc_List_List* f1;
} ; extern char Cyc_Attribute_tok[ 18u]; struct Cyc_Attribute_tok_struct{ char*
tag; void* f1; } ; extern char Cyc_BlockItem_tok[ 18u]; struct Cyc_BlockItem_tok_struct{
char* tag; void* f1; } ; extern char Cyc_Bool_tok[ 13u]; struct Cyc_Bool_tok_struct{
char* tag; int f1; } ; extern char Cyc_Char_tok[ 13u]; struct Cyc_Char_tok_struct{
char* tag; char f1; } ; extern char Cyc_DeclList_tok[ 17u]; struct Cyc_DeclList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; extern char Cyc_DeclSpec_tok[ 17u];
struct Cyc_DeclSpec_tok_struct{ char* tag; struct Cyc_Declaration_spec* f1; } ;
extern char Cyc_DeclaratorExpoptList_tok[ 29u]; struct Cyc_DeclaratorExpoptList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; extern char Cyc_DeclaratorExpopt_tok[
25u]; struct _tuple3{ struct Cyc_Declarator* f1; struct Cyc_Core_Opt* f2; } ;
struct Cyc_DeclaratorExpopt_tok_struct{ char* tag; struct _tuple3* f1; } ;
extern char Cyc_Declarator_tok[ 19u]; struct Cyc_Declarator_tok_struct{ char*
tag; struct Cyc_Declarator* f1; } ; extern char Cyc_DesignatorList_tok[ 23u];
struct Cyc_DesignatorList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ;
extern char Cyc_Designator_tok[ 19u]; struct Cyc_Designator_tok_struct{ char*
tag; void* f1; } ; extern char Cyc_EnumfieldList_tok[ 22u]; struct Cyc_EnumfieldList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; extern char Cyc_Enumfield_tok[ 18u];
struct Cyc_Enumfield_tok_struct{ char* tag; struct Cyc_Absyn_Enumfield* f1; } ;
extern char Cyc_ExpList_tok[ 16u]; struct Cyc_ExpList_tok_struct{ char* tag;
struct Cyc_List_List* f1; } ; extern char Cyc_Exp_tok[ 12u]; struct Cyc_Exp_tok_struct{
char* tag; struct Cyc_Absyn_Exp* f1; } ; extern char Cyc_FieldPatternList_tok[
25u]; struct Cyc_FieldPatternList_tok_struct{ char* tag; struct Cyc_List_List*
f1; } ; extern char Cyc_FieldPattern_tok[ 21u]; struct _tuple4{ struct Cyc_List_List*
f1; struct Cyc_Absyn_Pat* f2; } ; struct Cyc_FieldPattern_tok_struct{ char* tag;
struct _tuple4* f1; } ; extern char Cyc_FnDecl_tok[ 15u]; struct Cyc_FnDecl_tok_struct{
char* tag; struct Cyc_Absyn_Fndecl* f1; } ; extern char Cyc_IdList_tok[ 15u];
struct Cyc_IdList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ; extern
char Cyc_InitDeclList_tok[ 21u]; struct Cyc_InitDeclList_tok_struct{ char* tag;
struct Cyc_List_List* f1; } ; extern char Cyc_InitDecl_tok[ 17u]; struct _tuple5{
struct Cyc_Declarator* f1; struct Cyc_Absyn_Exp* f2; } ; struct Cyc_InitDecl_tok_struct{
char* tag; struct _tuple5* f1; } ; extern char Cyc_InitializerList_tok[ 24u];
struct Cyc_InitializerList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ;
extern char Cyc_Int_tok[ 12u]; struct _tuple6{ void* f1; int f2; } ; struct Cyc_Int_tok_struct{
char* tag; struct _tuple6* f1; } ; extern char Cyc_Kind_tok[ 13u]; struct Cyc_Kind_tok_struct{
char* tag; void* f1; } ; extern char Cyc_Okay_tok[ 13u]; extern char Cyc_ParamDeclListBool_tok[
26u]; struct _tuple7{ struct Cyc_List_List* f1; int f2; struct Cyc_Core_Opt* f3;
} ; struct Cyc_ParamDeclListBool_tok_struct{ char* tag; struct _tuple7* f1; } ;
extern char Cyc_ParamDeclList_tok[ 22u]; struct Cyc_ParamDeclList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; extern char Cyc_ParamDecl_tok[ 18u];
struct Cyc_ParamDecl_tok_struct{ char* tag; struct _tuple1* f1; } ; extern char
Cyc_PatternList_tok[ 20u]; struct Cyc_PatternList_tok_struct{ char* tag; struct
Cyc_List_List* f1; } ; extern char Cyc_Pattern_tok[ 16u]; struct Cyc_Pattern_tok_struct{
char* tag; struct Cyc_Absyn_Pat* f1; } ; extern char Cyc_Pointer_Sort_tok[ 21u];
struct Cyc_Pointer_Sort_tok_struct{ char* tag; void* f1; } ; extern char Cyc_Primop_tok[
15u]; struct Cyc_Primop_tok_struct{ char* tag; void* f1; } ; extern char Cyc_Primopopt_tok[
18u]; struct Cyc_Primopopt_tok_struct{ char* tag; struct Cyc_Core_Opt* f1; } ;
extern char Cyc_QualId_tok[ 15u]; struct Cyc_QualId_tok_struct{ char* tag;
struct _tuple0* f1; } ; extern char Cyc_QualSpecList_tok[ 21u]; struct _tuple8{
struct Cyc_Absyn_Tqual f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3; }
; struct Cyc_QualSpecList_tok_struct{ char* tag; struct _tuple8* f1; } ; extern
char Cyc_Rgn_tok[ 12u]; struct Cyc_Rgn_tok_struct{ char* tag; void* f1; } ;
extern char Cyc_Scope_tok[ 14u]; struct Cyc_Scope_tok_struct{ char* tag; void*
f1; } ; extern char Cyc_Short_tok[ 14u]; struct Cyc_Short_tok_struct{ char* tag;
short f1; } ; extern char Cyc_Stmt_tok[ 13u]; struct Cyc_Stmt_tok_struct{ char*
tag; struct Cyc_Absyn_Stmt* f1; } ; extern char Cyc_StorageClass_tok[ 21u];
struct Cyc_StorageClass_tok_struct{ char* tag; void* f1; } ; extern char Cyc_String_tok[
15u]; struct Cyc_String_tok_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Stringopt_tok[ 18u]; struct Cyc_Stringopt_tok_struct{ char* tag;
struct Cyc_Core_Opt* f1; } ; extern char Cyc_StructFieldDeclListList_tok[ 32u];
struct Cyc_StructFieldDeclListList_tok_struct{ char* tag; struct Cyc_List_List*
f1; } ; extern char Cyc_StructFieldDeclList_tok[ 28u]; struct Cyc_StructFieldDeclList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; extern char Cyc_StructOrUnion_tok[ 22u];
struct Cyc_StructOrUnion_tok_struct{ char* tag; void* f1; } ; extern char Cyc_SwitchClauseList_tok[
25u]; struct Cyc_SwitchClauseList_tok_struct{ char* tag; struct Cyc_List_List*
f1; } ; extern char Cyc_TunionFieldList_tok[ 24u]; struct Cyc_TunionFieldList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; extern char Cyc_TunionField_tok[ 20u];
struct Cyc_TunionField_tok_struct{ char* tag; struct Cyc_Absyn_Tunionfield* f1;
} ; extern char Cyc_TypeList_tok[ 17u]; struct Cyc_TypeList_tok_struct{ char*
tag; struct Cyc_List_List* f1; } ; extern char Cyc_TypeModifierList_tok[ 25u];
struct Cyc_TypeModifierList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ;
extern char Cyc_TypeQual_tok[ 17u]; struct Cyc_TypeQual_tok_struct{ char* tag;
struct Cyc_Absyn_Tqual f1; } ; extern char Cyc_TypeSpecifier_tok[ 22u]; struct
Cyc_TypeSpecifier_tok_struct{ char* tag; void* f1; } ; extern char Cyc_Type_tok[
13u]; struct Cyc_Type_tok_struct{ char* tag; void* f1; } ; struct Cyc_Yyltype{
int timestamp; int first_line; int first_column; int last_line; int last_column;
struct _tagged_string text; } ; typedef struct Cyc_Yyltype Cyc_yyltype; struct
Cyc_PP_Ppstate; typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t; struct Cyc_PP_Out;
typedef struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc; typedef struct Cyc_PP_Doc*
Cyc_PP_doc_t; extern int Cyc_Absynpp_expand_typedefs; extern int Cyc_Absynpp_qvar_to_Cids;
extern int Cyc_Absynpp_decls_first; extern int Cyc_Absynpp_print_all_tvars;
extern int Cyc_Absynpp_print_all_kinds; extern int Cyc_Absynpp_print_using_stmts;
extern int Cyc_Absynpp_print_externC_stmts; extern void Cyc_Absynpp_decllist2file(
struct Cyc_List_List* tdl, struct Cyc_Stdio___sFILE* f); extern void Cyc_Absyndump_dumpdecllist2file(
struct Cyc_List_List* tdl, struct Cyc_Stdio___sFILE* f); struct Cyc_Set_Set;
typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct Cyc_Set_Set* Cyc_Set_hset_t;
typedef struct Cyc_Set_Set* Cyc_Set_set_t; extern char Cyc_Set_Absent[ 11u];
static const int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{ int tag;
void* f1; } ; static const int Cyc_Tcenv_StructRes= 1; struct Cyc_Tcenv_StructRes_struct{
int tag; struct Cyc_Absyn_Structdecl* f1; } ; static const int Cyc_Tcenv_TunionRes=
2; struct Cyc_Tcenv_TunionRes_struct{ int tag; struct Cyc_Absyn_Tuniondecl* f1;
struct Cyc_Absyn_Tunionfield* f2; } ; static const int Cyc_Tcenv_EnumRes= 3;
struct Cyc_Tcenv_EnumRes_struct{ int tag; struct Cyc_Absyn_Enumdecl* f1; struct
Cyc_Absyn_Enumfield* f2; } ; typedef void* Cyc_Tcenv_resolved_t; struct Cyc_Tcenv_Genv{
struct Cyc_Set_Set* namespaces; struct Cyc_Dict_Dict* structdecls; struct Cyc_Dict_Dict*
uniondecls; struct Cyc_Dict_Dict* tuniondecls; struct Cyc_Dict_Dict* enumdecls;
struct Cyc_Dict_Dict* typedefs; struct Cyc_Dict_Dict* ordinaries; struct Cyc_List_List*
availables; } ; typedef struct Cyc_Tcenv_Genv* Cyc_Tcenv_genv_t; struct Cyc_Tcenv_Fenv;
typedef struct Cyc_Tcenv_Fenv* Cyc_Tcenv_fenv_t; static const int Cyc_Tcenv_NotLoop_j=
0; static const int Cyc_Tcenv_CaseEnd_j= 1; static const int Cyc_Tcenv_FnEnd_j=
2; static const int Cyc_Tcenv_Stmt_j= 0; struct Cyc_Tcenv_Stmt_j_struct{ int tag;
struct Cyc_Absyn_Stmt* f1; } ; typedef void* Cyc_Tcenv_jumpee_t; static const
int Cyc_Tcenv_Outermost= 0; struct Cyc_Tcenv_Outermost_struct{ int tag; void* f1;
} ; static const int Cyc_Tcenv_Frame= 1; struct Cyc_Tcenv_Frame_struct{ int tag;
void* f1; void* f2; } ; static const int Cyc_Tcenv_Hidden= 2; struct Cyc_Tcenv_Hidden_struct{
int tag; void* f1; void* f2; } ; typedef void* Cyc_Tcenv_frames_t; struct Cyc_Tcenv_Tenv{
struct Cyc_List_List* ns; struct Cyc_Dict_Dict* ae; struct Cyc_Core_Opt* le; } ;
typedef struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tenv_t; extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_tc_init(); extern char Cyc_Tcutil_TypeErr[ 12u]; extern void Cyc_Tcutil_flush_warnings();
extern void Cyc_Tc_tc( struct Cyc_Tcenv_Tenv* te, int add_cyc_namespace, int
var_default_init, struct Cyc_List_List* ds); extern struct Cyc_List_List* Cyc_Tc_treeshake(
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*); extern struct Cyc_List_List*
Cyc_Toc_toc( struct Cyc_List_List* ds); struct Cyc_CfFlowInfo_Place; typedef
void* Cyc_CfFlowInfo_local_root_t; typedef void* Cyc_CfFlowInfo_field_t; typedef
struct Cyc_CfFlowInfo_Place* Cyc_CfFlowInfo_place_t; typedef void* Cyc_CfFlowInfo_escaped_t;
typedef void* Cyc_CfFlowInfo_init_level_t; typedef void* Cyc_CfFlowInfo_init_state_t;
typedef void* Cyc_CfFlowInfo_path_info_t; typedef struct Cyc_Dict_Dict* Cyc_CfFlowInfo_pinfo_dict_t;
typedef void* Cyc_CfFlowInfo_flow_info_t; static const int Cyc_CfFlowInfo_VarRoot=
0; struct Cyc_CfFlowInfo_VarRoot_struct{ int tag; struct Cyc_Absyn_Vardecl* f1;
} ; static const int Cyc_CfFlowInfo_MallocPt= 1; struct Cyc_CfFlowInfo_MallocPt_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_CfFlowInfo_StructF=
0; struct Cyc_CfFlowInfo_StructF_struct{ int tag; struct _tagged_string* f1; } ;
static const int Cyc_CfFlowInfo_TupleF= 1; struct Cyc_CfFlowInfo_TupleF_struct{
int tag; int f1; } ; struct Cyc_CfFlowInfo_Place{ void* root; struct Cyc_List_List*
fields; } ; static const int Cyc_CfFlowInfo_Esc= 0; static const int Cyc_CfFlowInfo_Unesc=
1; static const int Cyc_CfFlowInfo_NoneIL= 0; static const int Cyc_CfFlowInfo_ThisIL=
1; static const int Cyc_CfFlowInfo_AllIL= 2; static const int Cyc_CfFlowInfo_UnknownIS=
0; struct Cyc_CfFlowInfo_UnknownIS_struct{ int tag; void* f1; void* f2; } ;
static const int Cyc_CfFlowInfo_MustPointTo= 1; struct Cyc_CfFlowInfo_MustPointTo_struct{
int tag; struct Cyc_CfFlowInfo_Place* f1; } ; static const int Cyc_CfFlowInfo_LeafPI=
0; struct Cyc_CfFlowInfo_LeafPI_struct{ int tag; void* f1; } ; static const int
Cyc_CfFlowInfo_TuplePI= 1; struct Cyc_CfFlowInfo_TuplePI_struct{ int tag; struct
Cyc_Dict_Dict* f1; } ; static const int Cyc_CfFlowInfo_StructPI= 2; struct Cyc_CfFlowInfo_StructPI_struct{
int tag; struct Cyc_Dict_Dict* f1; } ; static const int Cyc_CfFlowInfo_BottomFL=
0; static const int Cyc_CfFlowInfo_InitsFL= 0; struct Cyc_CfFlowInfo_InitsFL_struct{
int tag; struct Cyc_Dict_Dict* f1; } ; struct Cyc_NewControlFlow_AnalEnv{ struct
Cyc_Dict_Dict* roots; int in_try; void* tryflow; } ; typedef struct Cyc_NewControlFlow_AnalEnv*
Cyc_NewControlFlow_analenv_t; void Cyc_NewControlFlow_cf_check( struct Cyc_List_List*
ds); struct Cyc_Interface_I; typedef struct Cyc_Interface_I* Cyc_Interface_i_t;
extern struct Cyc_Interface_I* Cyc_Interface_extract( struct Cyc_Dict_Dict* ae);
extern void Cyc_Interface_print( struct Cyc_Interface_I*, struct Cyc_Stdio___sFILE*);
typedef struct Cyc_Interface_I* Cyc_interface_t; extern void Cyc_Lex_lex_init();
static int Cyc_pp_r= 0; static int Cyc_up_r= 0; static int Cyc_noexpand_r= 0;
static int Cyc_noshake_r= 0; static int Cyc_tc_r= 0; static int Cyc_ic_r= 0;
static int Cyc_toc_r= 0; static int Cyc_v_r= 0; static int Cyc_q_r= 0; static
int Cyc_add_cyc_namespace_r= 1; int* Cyc_pp=& Cyc_pp_r; int* Cyc_up=& Cyc_up_r;
int* Cyc_noexpand=& Cyc_noexpand_r; int* Cyc_noshake=& Cyc_noshake_r; int* Cyc_tc=&
Cyc_tc_r; int* Cyc_ic=& Cyc_ic_r; int* Cyc_toc=& Cyc_toc_r; int* Cyc_v=& Cyc_v_r;
int* Cyc_q=& Cyc_q_r; int* Cyc_add_cyc_namespace=& Cyc_add_cyc_namespace_r;
static struct _tagged_string* Cyc_out_file_name_opt= 0; static void Cyc_set_out_file_name(
struct _tagged_string s){ Cyc_out_file_name_opt=( struct _tagged_string*)({
struct _tagged_string* _temp0=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string)); _temp0[ 0]= s; _temp0;});} static struct Cyc_List_List*
Cyc_includes= 0; static void Cyc_add_include( struct _tagged_string s){ Cyc_includes=({
struct Cyc_List_List* _temp1=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp1->hd=( void*)({ struct _tagged_string* _temp2=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp2[ 0]= s;
_temp2;}); _temp1->tl= Cyc_includes; _temp1;});} static struct Cyc_List_List*
Cyc_defines= 0; static void Cyc_add_define( struct _tagged_string s){ Cyc_defines=({
struct Cyc_List_List* _temp3=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp3->hd=( void*)({ struct _tagged_string* _temp4=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp4[ 0]= s;
_temp4;}); _temp3->tl= Cyc_defines; _temp3;});} int Cyc_compile_failure= 0;
struct Cyc_Stdio___sFILE* Cyc_try_file_open( struct _tagged_string filename,
struct _tagged_string mode, struct _tagged_string msg_part){ struct
_handler_cons _temp5; _push_handler(& _temp5);{ int _temp7= 0; if( setjmp(
_temp5.handler)){ _temp7= 1;} if( ! _temp7){{ struct Cyc_Stdio___sFILE* _temp8=(
struct Cyc_Stdio___sFILE*) Cyc_Stdio_file_open( filename, mode); _npop_handler(
0u); return _temp8;}; _pop_handler();} else{ void* _temp6=( void*) _exn_thrown;
void* _temp10= _temp6; _LL12: goto _LL13; _LL14: goto _LL15; _LL13: Cyc_compile_failure=
1;({ struct _tagged_string _temp16= msg_part; struct _tagged_string _temp17=
filename; fprintf( Cyc_Stdio_stderr,"\nError: couldn't open %.*s %.*s\n",
_temp16.last_plus_one - _temp16.curr, _temp16.curr, _temp17.last_plus_one -
_temp17.curr, _temp17.curr);}); Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*)
Cyc_Stdio_stderr); return 0; _LL15:( void) _throw( _temp10); _LL11:;}}} struct
Cyc_List_List* Cyc_do_stage( struct _tagged_string stage_name, struct Cyc_List_List*(*
f)( void*, struct Cyc_List_List*), void* env, struct Cyc_List_List* tds, struct
_tagged_string preprocfile){ struct _tagged_string exn_string=( struct
_tagged_string)({ char* _temp54=( char*)""; struct _tagged_string _temp55;
_temp55.curr= _temp54; _temp55.base= _temp54; _temp55.last_plus_one= _temp54 + 1;
_temp55;}); struct _tagged_string explain_string=( struct _tagged_string)({ char*
_temp52=( char*)""; struct _tagged_string _temp53; _temp53.curr= _temp52;
_temp53.base= _temp52; _temp53.last_plus_one= _temp52 + 1; _temp53;}); int
other_exn= 0; void* ex=( void*)({ struct Cyc_Core_Impossible_struct* _temp48=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp48[ 0]=({ struct Cyc_Core_Impossible_struct _temp49; _temp49.tag= Cyc_Core_Impossible;
_temp49.f1=( struct _tagged_string)({ char* _temp50=( char*)""; struct
_tagged_string _temp51; _temp51.curr= _temp50; _temp51.base= _temp50; _temp51.last_plus_one=
_temp50 + 1; _temp51;}); _temp49;}); _temp48;}); struct Cyc_List_List* _temp18=
0; goto _LL19; _LL19:{ struct _handler_cons _temp20; _push_handler(& _temp20);{
int _temp22= 0; if( setjmp( _temp20.handler)){ _temp22= 1;} if( ! _temp22){
_temp18= f( env, tds);; _pop_handler();} else{ void* _temp21=( void*)
_exn_thrown; void* _temp24= _temp21; struct _tagged_string _temp34; _LL26: if(*((
void**) _temp24) == Cyc_Core_Impossible){ _LL35: _temp34=( struct _tagged_string)((
struct Cyc_Core_Impossible_struct*) _temp24)->f1; goto _LL27;} else{ goto _LL28;}
_LL28: if( _temp24 == Cyc_Dict_Absent){ goto _LL29;} else{ goto _LL30;} _LL30:
goto _LL31; _LL32: goto _LL33; _LL27: exn_string=( struct _tagged_string)({ char*
_temp36=( char*)"Exception Core::Impossible"; struct _tagged_string _temp37;
_temp37.curr= _temp36; _temp37.base= _temp36; _temp37.last_plus_one= _temp36 +
27; _temp37;}); explain_string= _temp34; goto _LL25; _LL29: exn_string=( struct
_tagged_string)({ char* _temp38=( char*)"Exception Dict::Absent"; struct
_tagged_string _temp39; _temp39.curr= _temp38; _temp39.base= _temp38; _temp39.last_plus_one=
_temp38 + 23; _temp39;}); goto _LL25; _LL31: ex= _temp24; other_exn= 1;
exn_string=( struct _tagged_string)({ char* _temp40=( char*)"Uncaught exception";
struct _tagged_string _temp41; _temp41.curr= _temp40; _temp41.base= _temp40;
_temp41.last_plus_one= _temp40 + 19; _temp41;}); goto _LL25; _LL33:( void)
_throw( _temp24); _LL25:;}}} if( Cyc_Position_error_p()){ Cyc_compile_failure= 1;}
if( Cyc_String_strcmp( exn_string,( struct _tagged_string)({ char* _temp42=(
char*)""; struct _tagged_string _temp43; _temp43.curr= _temp42; _temp43.base=
_temp42; _temp43.last_plus_one= _temp42 + 1; _temp43;})) != 0){ Cyc_compile_failure=
1;({ struct _tagged_string _temp44= exn_string; struct _tagged_string _temp45=
stage_name; struct _tagged_string _temp46= explain_string; fprintf( Cyc_Stdio_stderr,"\n%.*s thrown during %.*s: %.*s",
_temp44.last_plus_one - _temp44.curr, _temp44.curr, _temp45.last_plus_one -
_temp45.curr, _temp45.curr, _temp46.last_plus_one - _temp46.curr, _temp46.curr);});}
if( Cyc_compile_failure){ fprintf( Cyc_Stdio_stderr,"\nCOMPILATION FAILED!\n");
Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*) Cyc_Stdio_stderr); Cyc_Stdio_remove(
preprocfile); if( other_exn){( void) _throw( ex);} return _temp18;} if(* Cyc_v){({
struct _tagged_string _temp47= stage_name; fprintf( Cyc_Stdio_stderr,"%.*s completed.\n",
_temp47.last_plus_one - _temp47.curr, _temp47.curr);});} else{ if( !(* Cyc_q)){
fprintf( Cyc_Stdio_stderr,".");}} Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*)
Cyc_Stdio_stderr); return _temp18;} struct Cyc_List_List* Cyc_do_parse( struct
Cyc_Stdio___sFILE* f, struct Cyc_List_List* ignore){ Cyc_Lex_lex_init(); return
Cyc_Parse_parse_file( f);} struct Cyc_List_List* Cyc_do_typecheck( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* tds){ Cyc_Absynpp_expand_typedefs= !(* Cyc_noexpand);
Cyc_add_cyc_namespace_r= Cyc_add_cyc_namespace_r? Cyc_toc_r: 0; Cyc_Tc_tc( te,
Cyc_add_cyc_namespace_r, 1, tds); if( !(* Cyc_noshake)){ tds= Cyc_Tc_treeshake(
te, tds);} return tds;} struct Cyc_List_List* Cyc_do_cfcheck( int ignore, struct
Cyc_List_List* tds){ Cyc_NewControlFlow_cf_check( tds); return tds;} struct
_tuple9{ struct Cyc_Tcenv_Tenv* f1; struct Cyc_Stdio___sFILE* f2; } ; struct Cyc_List_List*
Cyc_do_interface( struct _tuple9* params, struct Cyc_List_List* tds){ struct Cyc_Tcenv_Tenv*
_temp56=(* params).f1; goto _LL57; _LL57: { struct Cyc_Stdio___sFILE* _temp58=(*
params).f2; goto _LL59; _LL59: { struct Cyc_Interface_I* _temp60= Cyc_Interface_extract(
_temp56->ae); goto _LL61; _LL61: Cyc_Interface_print( _temp60, _temp58); return
tds;}}} struct Cyc_List_List* Cyc_do_translate( int ignore, struct Cyc_List_List*
tds){ return Cyc_Toc_toc( tds);} struct Cyc_List_List* Cyc_do_print( struct Cyc_Stdio___sFILE*
out_file, struct Cyc_List_List* tds){ Cyc_Absynpp_expand_typedefs= !(* Cyc_noexpand);
Cyc_Absynpp_decls_first=( Cyc_Absynpp_qvar_to_Cids=* Cyc_toc); Cyc_Absynpp_print_using_stmts=(
Cyc_Absynpp_print_externC_stmts= !(* Cyc_tc)); if(* Cyc_toc){ fprintf( out_file,"#include \"cyc_include.h\"\n\n");
Cyc_Absynpp_qvar_to_Cids= 1;} if(* Cyc_pp){ Cyc_Absynpp_decllist2file( tds,
out_file);} else{ if(* Cyc_up){ Cyc_Absyndump_dumpdecllist2file( tds, out_file);}}
Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*) out_file); return tds;} void Cyc_compile_file(
struct _tagged_string filename){ struct _tagged_string basename= Cyc_Filename_chop_extension(
filename); struct _tagged_string preprocfile= Cyc_String_strconcat( basename,(
struct _tagged_string)({ char* _temp113=( char*)".i"; struct _tagged_string
_temp114; _temp114.curr= _temp113; _temp114.base= _temp113; _temp114.last_plus_one=
_temp113 + 3; _temp114;})); struct _tagged_string interfacefile= Cyc_String_strconcat(
basename,( struct _tagged_string)({ char* _temp111=( char*)".cyci"; struct
_tagged_string _temp112; _temp112.curr= _temp111; _temp112.base= _temp111;
_temp112.last_plus_one= _temp111 + 6; _temp112;})); if( !(* Cyc_q)? 1:* Cyc_v){({
struct _tagged_string _temp62= basename; fprintf( Cyc_Stdio_stderr,"Compiling file: %.*s",
_temp62.last_plus_one - _temp62.curr, _temp62.curr);});} if(* Cyc_v){ fprintf(
Cyc_Stdio_stderr,"\n");}{ struct _tagged_string inc_string= Cyc_String_str_sepstr(({
struct Cyc_List_List* _temp105=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp105->hd=( void*)({ struct _tagged_string* _temp106=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp106[ 0]=(
struct _tagged_string)({ char* _temp107=( char*)""; struct _tagged_string
_temp108; _temp108.curr= _temp107; _temp108.base= _temp107; _temp108.last_plus_one=
_temp107 + 1; _temp108;}); _temp106;}); _temp105->tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( Cyc_includes); _temp105;}),( struct
_tagged_string)({ char* _temp109=( char*)" -I"; struct _tagged_string _temp110;
_temp110.curr= _temp109; _temp110.base= _temp109; _temp110.last_plus_one=
_temp109 + 4; _temp110;})); struct _tagged_string def_string= Cyc_String_str_sepstr(({
struct Cyc_List_List* _temp99=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp99->hd=( void*)({ struct _tagged_string* _temp100=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp100[ 0]=(
struct _tagged_string)({ char* _temp101=( char*)""; struct _tagged_string
_temp102; _temp102.curr= _temp101; _temp102.base= _temp101; _temp102.last_plus_one=
_temp101 + 1; _temp102;}); _temp100;}); _temp99->tl=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( Cyc_defines); _temp99;}),( struct
_tagged_string)({ char* _temp103=( char*)" -D"; struct _tagged_string _temp104;
_temp104.curr= _temp103; _temp104.base= _temp103; _temp104.last_plus_one=
_temp103 + 4; _temp104;})); struct Cyc_Stdio___sFILE* f0= Cyc_try_file_open(
filename,( struct _tagged_string)({ char* _temp95=( char*)"r"; struct
_tagged_string _temp96; _temp96.curr= _temp95; _temp96.base= _temp95; _temp96.last_plus_one=
_temp95 + 2; _temp96;}),( struct _tagged_string)({ char* _temp97=( char*)"input file";
struct _tagged_string _temp98; _temp98.curr= _temp97; _temp98.base= _temp97;
_temp98.last_plus_one= _temp97 + 11; _temp98;})); if( Cyc_compile_failure){
return;} Cyc_Stdio_fclose(( struct Cyc_Stdio___sFILE*)(( struct Cyc_Stdio___sFILE*)
_check_null( f0)));{ struct _tagged_string cmd=({ struct _tagged_string _temp91=
inc_string; struct _tagged_string _temp92= def_string; struct _tagged_string
_temp93= filename; struct _tagged_string _temp94= preprocfile; xprintf("gcc -x c -E -U__GNUC__ %.*s %.*s %.*s > %.*s",
_temp91.last_plus_one - _temp91.curr, _temp91.curr, _temp92.last_plus_one -
_temp92.curr, _temp92.curr, _temp93.last_plus_one - _temp93.curr, _temp93.curr,
_temp94.last_plus_one - _temp94.curr, _temp94.curr);}); if(* Cyc_v){({ struct
_tagged_string _temp63= cmd; fprintf( Cyc_Stdio_stderr,"%.*s\n", _temp63.last_plus_one
- _temp63.curr, _temp63.curr);});} if( system( string_to_Cstring( cmd)) != 0){
Cyc_compile_failure= 1; fprintf( Cyc_Stdio_stderr,"\nError: pre-processing failed!\n");
return;} Cyc_Position_reset_position( preprocfile);{ struct Cyc_Stdio___sFILE*
in_file= Cyc_try_file_open( preprocfile,( struct _tagged_string)({ char* _temp87=(
char*)"r"; struct _tagged_string _temp88; _temp88.curr= _temp87; _temp88.base=
_temp87; _temp88.last_plus_one= _temp87 + 2; _temp88;}),( struct _tagged_string)({
char* _temp89=( char*)"file"; struct _tagged_string _temp90; _temp90.curr=
_temp89; _temp90.base= _temp89; _temp90.last_plus_one= _temp89 + 5; _temp90;}));
struct Cyc_Stdio___sFILE* out_file=( struct Cyc_Stdio___sFILE*) Cyc_Stdio_stdout;
if( Cyc_compile_failure){ return;}{ struct Cyc_List_List* tds= 0; if( ! Cyc_compile_failure){
tds=(( struct Cyc_List_List*(*)( struct _tagged_string stage_name, struct Cyc_List_List*(*
f)( struct Cyc_Stdio___sFILE*, struct Cyc_List_List*), struct Cyc_Stdio___sFILE*
env, struct Cyc_List_List* tds, struct _tagged_string preprocfile)) Cyc_do_stage)((
struct _tagged_string)({ char* _temp64=( char*)"parsing"; struct _tagged_string
_temp65; _temp65.curr= _temp64; _temp65.base= _temp64; _temp65.last_plus_one=
_temp64 + 8; _temp65;}), Cyc_do_parse,( struct Cyc_Stdio___sFILE*)(( struct Cyc_Stdio___sFILE*)
_check_null( in_file)), tds, preprocfile); Cyc_Stdio_file_close(( struct Cyc_Stdio___sFILE*)((
struct Cyc_Stdio___sFILE*) _check_null( in_file)));}{ struct Cyc_Tcenv_Tenv*
_temp66= Cyc_Tcenv_tc_init(); goto _LL67; _LL67: if(( ! Cyc_compile_failure? tds
!= 0: 0)?* Cyc_tc: 0){ tds=(( struct Cyc_List_List*(*)( struct _tagged_string
stage_name, struct Cyc_List_List*(* f)( struct Cyc_Tcenv_Tenv*, struct Cyc_List_List*),
struct Cyc_Tcenv_Tenv* env, struct Cyc_List_List* tds, struct _tagged_string
preprocfile)) Cyc_do_stage)(( struct _tagged_string)({ char* _temp68=( char*)"type checking";
struct _tagged_string _temp69; _temp69.curr= _temp68; _temp69.base= _temp68;
_temp69.last_plus_one= _temp68 + 14; _temp69;}), Cyc_do_typecheck, _temp66, tds,
preprocfile);} if( ! Cyc_compile_failure?* Cyc_tc: 0){ tds=(( struct Cyc_List_List*(*)(
struct _tagged_string stage_name, struct Cyc_List_List*(* f)( int, struct Cyc_List_List*),
int env, struct Cyc_List_List* tds, struct _tagged_string preprocfile)) Cyc_do_stage)((
struct _tagged_string)({ char* _temp70=( char*)"control-flow checking"; struct
_tagged_string _temp71; _temp71.curr= _temp70; _temp71.base= _temp70; _temp71.last_plus_one=
_temp70 + 22; _temp71;}), Cyc_do_cfcheck, 1, tds, preprocfile);} if( ! Cyc_compile_failure?*
Cyc_ic: 0){ struct Cyc_Stdio___sFILE* inter_file= Cyc_try_file_open(
interfacefile,( struct _tagged_string)({ char* _temp75=( char*)"w"; struct
_tagged_string _temp76; _temp76.curr= _temp75; _temp76.base= _temp75; _temp76.last_plus_one=
_temp75 + 2; _temp76;}),( struct _tagged_string)({ char* _temp77=( char*)"interface file";
struct _tagged_string _temp78; _temp78.curr= _temp77; _temp78.base= _temp77;
_temp78.last_plus_one= _temp77 + 15; _temp78;})); tds=(( struct Cyc_List_List*(*)(
struct _tagged_string stage_name, struct Cyc_List_List*(* f)( struct _tuple9*,
struct Cyc_List_List*), struct _tuple9* env, struct Cyc_List_List* tds, struct
_tagged_string preprocfile)) Cyc_do_stage)(( struct _tagged_string)({ char*
_temp72=( char*)"interface checking"; struct _tagged_string _temp73; _temp73.curr=
_temp72; _temp73.base= _temp72; _temp73.last_plus_one= _temp72 + 19; _temp73;}),
Cyc_do_interface,({ struct _tuple9* _temp74=( struct _tuple9*) GC_malloc(
sizeof( struct _tuple9)); _temp74->f1= _temp66; _temp74->f2=( struct Cyc_Stdio___sFILE*)((
struct Cyc_Stdio___sFILE*) _check_null( inter_file)); _temp74;}), tds,
preprocfile); if( inter_file != 0){ Cyc_Stdio_file_close(( struct Cyc_Stdio___sFILE*)((
struct Cyc_Stdio___sFILE*) _check_null( inter_file)));}} if( ! Cyc_compile_failure?*
Cyc_toc: 0){ tds=(( struct Cyc_List_List*(*)( struct _tagged_string stage_name,
struct Cyc_List_List*(* f)( int, struct Cyc_List_List*), int env, struct Cyc_List_List*
tds, struct _tagged_string preprocfile)) Cyc_do_stage)(( struct _tagged_string)({
char* _temp79=( char*)"translation to C"; struct _tagged_string _temp80; _temp80.curr=
_temp79; _temp80.base= _temp79; _temp80.last_plus_one= _temp79 + 17; _temp80;}),
Cyc_do_translate, 1, tds, preprocfile);} if( ! Cyc_compile_failure? Cyc_out_file_name_opt
!= 0: 0){ out_file= Cyc_try_file_open(*(( struct _tagged_string*) _check_null(
Cyc_out_file_name_opt)),( struct _tagged_string)({ char* _temp81=( char*)"w";
struct _tagged_string _temp82; _temp82.curr= _temp81; _temp82.base= _temp81;
_temp82.last_plus_one= _temp81 + 2; _temp82;}),( struct _tagged_string)({ char*
_temp83=( char*)"output file"; struct _tagged_string _temp84; _temp84.curr=
_temp83; _temp84.base= _temp83; _temp84.last_plus_one= _temp83 + 12; _temp84;}));}
if(( ! Cyc_compile_failure? tds != 0: 0)?* Cyc_pp? 1:* Cyc_up: 0){ tds=(( struct
Cyc_List_List*(*)( struct _tagged_string stage_name, struct Cyc_List_List*(* f)(
struct Cyc_Stdio___sFILE*, struct Cyc_List_List*), struct Cyc_Stdio___sFILE* env,
struct Cyc_List_List* tds, struct _tagged_string preprocfile)) Cyc_do_stage)((
struct _tagged_string)({ char* _temp85=( char*)"printing"; struct _tagged_string
_temp86; _temp86.curr= _temp85; _temp86.base= _temp85; _temp86.last_plus_one=
_temp85 + 9; _temp86;}), Cyc_do_print,( struct Cyc_Stdio___sFILE*)(( struct Cyc_Stdio___sFILE*)
_check_null( out_file)), tds, preprocfile);} if( out_file != 0? out_file !=(
struct Cyc_Stdio___sFILE*) Cyc_Stdio_stdout: 0){ Cyc_Stdio_file_close(( struct
Cyc_Stdio___sFILE*)(( struct Cyc_Stdio___sFILE*) _check_null( out_file)));}
fprintf( Cyc_Stdio_stderr,"\n"); if( ! Cyc_compile_failure){ Cyc_Tcutil_flush_warnings();}
Cyc_Stdio_remove( preprocfile); return;}}}}}} struct _tuple10{ struct
_tagged_string f1; void* f2; struct _tagged_string f3; } ; int Cyc_main( int
argc, struct _tagged_ptr0 argv){ struct _tuple10 specs[ 15u]={({ struct _tuple10
_temp164; _temp164.f1=( struct _tagged_string)({ char* _temp169=( char*)"-nocyc";
struct _tagged_string _temp170; _temp170.curr= _temp169; _temp170.base= _temp169;
_temp170.last_plus_one= _temp169 + 7; _temp170;}); _temp164.f2=( void*)({ struct
Cyc_Arg_Clear_spec_struct* _temp167=( struct Cyc_Arg_Clear_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Clear_spec_struct)); _temp167[ 0]=({ struct
Cyc_Arg_Clear_spec_struct _temp168; _temp168.tag= Cyc_Arg_Clear_spec; _temp168.f1=
Cyc_add_cyc_namespace; _temp168;}); _temp167;}); _temp164.f3=( struct
_tagged_string)({ char* _temp165=( char*)"does not add implicit namespace Cyc";
struct _tagged_string _temp166; _temp166.curr= _temp165; _temp166.base= _temp165;
_temp166.last_plus_one= _temp165 + 36; _temp166;}); _temp164;}),({ struct
_tuple10 _temp171; _temp171.f1=( struct _tagged_string)({ char* _temp176=( char*)"-q";
struct _tagged_string _temp177; _temp177.curr= _temp176; _temp177.base= _temp176;
_temp177.last_plus_one= _temp176 + 3; _temp177;}); _temp171.f2=( void*)({ struct
Cyc_Arg_Set_spec_struct* _temp174=( struct Cyc_Arg_Set_spec_struct*) GC_malloc(
sizeof( struct Cyc_Arg_Set_spec_struct)); _temp174[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp175; _temp175.tag= Cyc_Arg_Set_spec; _temp175.f1= Cyc_q; _temp175;});
_temp174;}); _temp171.f3=( struct _tagged_string)({ char* _temp172=( char*)"compile quietly";
struct _tagged_string _temp173; _temp173.curr= _temp172; _temp173.base= _temp172;
_temp173.last_plus_one= _temp172 + 16; _temp173;}); _temp171;}),({ struct
_tuple10 _temp178; _temp178.f1=( struct _tagged_string)({ char* _temp183=( char*)"-v";
struct _tagged_string _temp184; _temp184.curr= _temp183; _temp184.base= _temp183;
_temp184.last_plus_one= _temp183 + 3; _temp184;}); _temp178.f2=( void*)({ struct
Cyc_Arg_Set_spec_struct* _temp181=( struct Cyc_Arg_Set_spec_struct*) GC_malloc(
sizeof( struct Cyc_Arg_Set_spec_struct)); _temp181[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp182; _temp182.tag= Cyc_Arg_Set_spec; _temp182.f1= Cyc_v; _temp182;});
_temp181;}); _temp178.f3=( struct _tagged_string)({ char* _temp179=( char*)"print compilation stages verbosely";
struct _tagged_string _temp180; _temp180.curr= _temp179; _temp180.base= _temp179;
_temp180.last_plus_one= _temp179 + 35; _temp180;}); _temp178;}),({ struct
_tuple10 _temp185; _temp185.f1=( struct _tagged_string)({ char* _temp190=( char*)"-D";
struct _tagged_string _temp191; _temp191.curr= _temp190; _temp191.base= _temp190;
_temp191.last_plus_one= _temp190 + 3; _temp191;}); _temp185.f2=( void*)({ struct
Cyc_Arg_String_spec_struct* _temp188=( struct Cyc_Arg_String_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_String_spec_struct)); _temp188[ 0]=({ struct
Cyc_Arg_String_spec_struct _temp189; _temp189.tag= Cyc_Arg_String_spec; _temp189.f1=
Cyc_add_define; _temp189;}); _temp188;}); _temp185.f3=( struct _tagged_string)({
char* _temp186=( char*)"pass definition to pre-processor"; struct _tagged_string
_temp187; _temp187.curr= _temp186; _temp187.base= _temp186; _temp187.last_plus_one=
_temp186 + 33; _temp187;}); _temp185;}),({ struct _tuple10 _temp192; _temp192.f1=(
struct _tagged_string)({ char* _temp197=( char*)"-I"; struct _tagged_string
_temp198; _temp198.curr= _temp197; _temp198.base= _temp197; _temp198.last_plus_one=
_temp197 + 3; _temp198;}); _temp192.f2=( void*)({ struct Cyc_Arg_String_spec_struct*
_temp195=( struct Cyc_Arg_String_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_String_spec_struct));
_temp195[ 0]=({ struct Cyc_Arg_String_spec_struct _temp196; _temp196.tag= Cyc_Arg_String_spec;
_temp196.f1= Cyc_add_include; _temp196;}); _temp195;}); _temp192.f3=( struct
_tagged_string)({ char* _temp193=( char*)"include directory"; struct
_tagged_string _temp194; _temp194.curr= _temp193; _temp194.base= _temp193;
_temp194.last_plus_one= _temp193 + 18; _temp194;}); _temp192;}),({ struct
_tuple10 _temp199; _temp199.f1=( struct _tagged_string)({ char* _temp204=( char*)"-toc";
struct _tagged_string _temp205; _temp205.curr= _temp204; _temp205.base= _temp204;
_temp205.last_plus_one= _temp204 + 5; _temp205;}); _temp199.f2=( void*)({ struct
Cyc_Arg_Set_spec_struct* _temp202=( struct Cyc_Arg_Set_spec_struct*) GC_malloc(
sizeof( struct Cyc_Arg_Set_spec_struct)); _temp202[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp203; _temp203.tag= Cyc_Arg_Set_spec; _temp203.f1= Cyc_toc; _temp203;});
_temp202;}); _temp199.f3=( struct _tagged_string)({ char* _temp200=( char*)"translate to C";
struct _tagged_string _temp201; _temp201.curr= _temp200; _temp201.base= _temp200;
_temp201.last_plus_one= _temp200 + 15; _temp201;}); _temp199;}),({ struct
_tuple10 _temp206; _temp206.f1=( struct _tagged_string)({ char* _temp211=( char*)"-tc";
struct _tagged_string _temp212; _temp212.curr= _temp211; _temp212.base= _temp211;
_temp212.last_plus_one= _temp211 + 4; _temp212;}); _temp206.f2=( void*)({ struct
Cyc_Arg_Set_spec_struct* _temp209=( struct Cyc_Arg_Set_spec_struct*) GC_malloc(
sizeof( struct Cyc_Arg_Set_spec_struct)); _temp209[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp210; _temp210.tag= Cyc_Arg_Set_spec; _temp210.f1= Cyc_tc; _temp210;});
_temp209;}); _temp206.f3=( struct _tagged_string)({ char* _temp207=( char*)"type check";
struct _tagged_string _temp208; _temp208.curr= _temp207; _temp208.base= _temp207;
_temp208.last_plus_one= _temp207 + 11; _temp208;}); _temp206;}),({ struct
_tuple10 _temp213; _temp213.f1=( struct _tagged_string)({ char* _temp218=( char*)"-ic";
struct _tagged_string _temp219; _temp219.curr= _temp218; _temp219.base= _temp218;
_temp219.last_plus_one= _temp218 + 4; _temp219;}); _temp213.f2=( void*)({ struct
Cyc_Arg_Set_spec_struct* _temp216=( struct Cyc_Arg_Set_spec_struct*) GC_malloc(
sizeof( struct Cyc_Arg_Set_spec_struct)); _temp216[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp217; _temp217.tag= Cyc_Arg_Set_spec; _temp217.f1= Cyc_ic; _temp217;});
_temp216;}); _temp213.f3=( struct _tagged_string)({ char* _temp214=( char*)"interface check";
struct _tagged_string _temp215; _temp215.curr= _temp214; _temp215.base= _temp214;
_temp215.last_plus_one= _temp214 + 16; _temp215;}); _temp213;}),({ struct
_tuple10 _temp220; _temp220.f1=( struct _tagged_string)({ char* _temp225=( char*)"-noremoveunused";
struct _tagged_string _temp226; _temp226.curr= _temp225; _temp226.base= _temp225;
_temp226.last_plus_one= _temp225 + 16; _temp226;}); _temp220.f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp223=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp223[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp224; _temp224.tag= Cyc_Arg_Set_spec; _temp224.f1= Cyc_noshake; _temp224;});
_temp223;}); _temp220.f3=( struct _tagged_string)({ char* _temp221=( char*)"don't remove externed variables that aren't used";
struct _tagged_string _temp222; _temp222.curr= _temp221; _temp222.base= _temp221;
_temp222.last_plus_one= _temp221 + 49; _temp222;}); _temp220;}),({ struct
_tuple10 _temp227; _temp227.f1=( struct _tagged_string)({ char* _temp232=( char*)"-noexpandtypedefs";
struct _tagged_string _temp233; _temp233.curr= _temp232; _temp233.base= _temp232;
_temp233.last_plus_one= _temp232 + 18; _temp233;}); _temp227.f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp230=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp230[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp231; _temp231.tag= Cyc_Arg_Set_spec; _temp231.f1= Cyc_noexpand; _temp231;});
_temp230;}); _temp227.f3=( struct _tagged_string)({ char* _temp228=( char*)"don't expand typedefs in pretty printing";
struct _tagged_string _temp229; _temp229.curr= _temp228; _temp229.base= _temp228;
_temp229.last_plus_one= _temp228 + 41; _temp229;}); _temp227;}),({ struct
_tuple10 _temp234; _temp234.f1=( struct _tagged_string)({ char* _temp239=( char*)"-printalltvars";
struct _tagged_string _temp240; _temp240.curr= _temp239; _temp240.base= _temp239;
_temp240.last_plus_one= _temp239 + 15; _temp240;}); _temp234.f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp237=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp237[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp238; _temp238.tag= Cyc_Arg_Set_spec; _temp238.f1=& Cyc_Absynpp_print_all_tvars;
_temp238;}); _temp237;}); _temp234.f3=( struct _tagged_string)({ char* _temp235=(
char*)"print all type variables (even implicit default effects)"; struct
_tagged_string _temp236; _temp236.curr= _temp235; _temp236.base= _temp235;
_temp236.last_plus_one= _temp235 + 57; _temp236;}); _temp234;}),({ struct
_tuple10 _temp241; _temp241.f1=( struct _tagged_string)({ char* _temp246=( char*)"-printallkinds";
struct _tagged_string _temp247; _temp247.curr= _temp246; _temp247.base= _temp246;
_temp247.last_plus_one= _temp246 + 15; _temp247;}); _temp241.f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp244=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp244[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp245; _temp245.tag= Cyc_Arg_Set_spec; _temp245.f1=& Cyc_Absynpp_print_all_kinds;
_temp245;}); _temp244;}); _temp241.f3=( struct _tagged_string)({ char* _temp242=(
char*)"always print kinds of type variables"; struct _tagged_string _temp243;
_temp243.curr= _temp242; _temp243.base= _temp242; _temp243.last_plus_one=
_temp242 + 37; _temp243;}); _temp241;}),({ struct _tuple10 _temp248; _temp248.f1=(
struct _tagged_string)({ char* _temp253=( char*)"-up"; struct _tagged_string
_temp254; _temp254.curr= _temp253; _temp254.base= _temp253; _temp254.last_plus_one=
_temp253 + 4; _temp254;}); _temp248.f2=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp251=( struct Cyc_Arg_Set_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp251[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp252; _temp252.tag= Cyc_Arg_Set_spec;
_temp252.f1= Cyc_up; _temp252;}); _temp251;}); _temp248.f3=( struct
_tagged_string)({ char* _temp249=( char*)"ugly print"; struct _tagged_string
_temp250; _temp250.curr= _temp249; _temp250.base= _temp249; _temp250.last_plus_one=
_temp249 + 11; _temp250;}); _temp248;}),({ struct _tuple10 _temp255; _temp255.f1=(
struct _tagged_string)({ char* _temp260=( char*)"-pp"; struct _tagged_string
_temp261; _temp261.curr= _temp260; _temp261.base= _temp260; _temp261.last_plus_one=
_temp260 + 4; _temp261;}); _temp255.f2=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp258=( struct Cyc_Arg_Set_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp258[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp259; _temp259.tag= Cyc_Arg_Set_spec;
_temp259.f1= Cyc_pp; _temp259;}); _temp258;}); _temp255.f3=( struct
_tagged_string)({ char* _temp256=( char*)"pretty print"; struct _tagged_string
_temp257; _temp257.curr= _temp256; _temp257.base= _temp256; _temp257.last_plus_one=
_temp256 + 13; _temp257;}); _temp255;}),({ struct _tuple10 _temp262; _temp262.f1=(
struct _tagged_string)({ char* _temp267=( char*)"-o"; struct _tagged_string
_temp268; _temp268.curr= _temp267; _temp268.base= _temp267; _temp268.last_plus_one=
_temp267 + 3; _temp268;}); _temp262.f2=( void*)({ struct Cyc_Arg_String_spec_struct*
_temp265=( struct Cyc_Arg_String_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_String_spec_struct));
_temp265[ 0]=({ struct Cyc_Arg_String_spec_struct _temp266; _temp266.tag= Cyc_Arg_String_spec;
_temp266.f1= Cyc_set_out_file_name; _temp266;}); _temp265;}); _temp262.f3=(
struct _tagged_string)({ char* _temp263=( char*)"Output file name"; struct
_tagged_string _temp264; _temp264.curr= _temp263; _temp264.base= _temp263;
_temp264.last_plus_one= _temp263 + 17; _temp264;}); _temp262;})}; struct Cyc_List_List
_temp116=({ struct Cyc_List_List _temp115; _temp115.hd=( void*)&(( struct
_tuple10*) specs)[ _check_known_subscript_notnull( 15u, 0)]; _temp115.tl= 0;
_temp115;}); goto _LL117; _LL117: { struct Cyc_List_List _temp119=({ struct Cyc_List_List
_temp118; _temp118.hd=( void*)&(( struct _tuple10*) specs)[
_check_known_subscript_notnull( 15u, 1)]; _temp118.tl=( struct Cyc_List_List*)&
_temp116; _temp118;}); goto _LL120; _LL120: { struct Cyc_List_List _temp122=({
struct Cyc_List_List _temp121; _temp121.hd=( void*)&(( struct _tuple10*) specs)[
_check_known_subscript_notnull( 15u, 2)]; _temp121.tl=( struct Cyc_List_List*)&
_temp119; _temp121;}); goto _LL123; _LL123: { struct Cyc_List_List _temp125=({
struct Cyc_List_List _temp124; _temp124.hd=( void*)&(( struct _tuple10*) specs)[
_check_known_subscript_notnull( 15u, 3)]; _temp124.tl=( struct Cyc_List_List*)&
_temp122; _temp124;}); goto _LL126; _LL126: { struct Cyc_List_List _temp128=({
struct Cyc_List_List _temp127; _temp127.hd=( void*)&(( struct _tuple10*) specs)[
_check_known_subscript_notnull( 15u, 4)]; _temp127.tl=( struct Cyc_List_List*)&
_temp125; _temp127;}); goto _LL129; _LL129: { struct Cyc_List_List _temp131=({
struct Cyc_List_List _temp130; _temp130.hd=( void*)&(( struct _tuple10*) specs)[
_check_known_subscript_notnull( 15u, 5)]; _temp130.tl=( struct Cyc_List_List*)&
_temp128; _temp130;}); goto _LL132; _LL132: { struct Cyc_List_List _temp134=({
struct Cyc_List_List _temp133; _temp133.hd=( void*)&(( struct _tuple10*) specs)[
_check_known_subscript_notnull( 15u, 6)]; _temp133.tl=( struct Cyc_List_List*)&
_temp131; _temp133;}); goto _LL135; _LL135: { struct Cyc_List_List _temp137=({
struct Cyc_List_List _temp136; _temp136.hd=( void*)&(( struct _tuple10*) specs)[
_check_known_subscript_notnull( 15u, 7)]; _temp136.tl=( struct Cyc_List_List*)&
_temp134; _temp136;}); goto _LL138; _LL138: { struct Cyc_List_List _temp140=({
struct Cyc_List_List _temp139; _temp139.hd=( void*)&(( struct _tuple10*) specs)[
_check_known_subscript_notnull( 15u, 8)]; _temp139.tl=( struct Cyc_List_List*)&
_temp137; _temp139;}); goto _LL141; _LL141: { struct Cyc_List_List _temp143=({
struct Cyc_List_List _temp142; _temp142.hd=( void*)&(( struct _tuple10*) specs)[
_check_known_subscript_notnull( 15u, 9)]; _temp142.tl=( struct Cyc_List_List*)&
_temp140; _temp142;}); goto _LL144; _LL144: { struct Cyc_List_List _temp146=({
struct Cyc_List_List _temp145; _temp145.hd=( void*)&(( struct _tuple10*) specs)[
_check_known_subscript_notnull( 15u, 10)]; _temp145.tl=( struct Cyc_List_List*)&
_temp143; _temp145;}); goto _LL147; _LL147: { struct Cyc_List_List _temp149=({
struct Cyc_List_List _temp148; _temp148.hd=( void*)&(( struct _tuple10*) specs)[
_check_known_subscript_notnull( 15u, 11)]; _temp148.tl=( struct Cyc_List_List*)&
_temp146; _temp148;}); goto _LL150; _LL150: { struct Cyc_List_List _temp152=({
struct Cyc_List_List _temp151; _temp151.hd=( void*)&(( struct _tuple10*) specs)[
_check_known_subscript_notnull( 15u, 12)]; _temp151.tl=( struct Cyc_List_List*)&
_temp149; _temp151;}); goto _LL153; _LL153: { struct Cyc_List_List _temp155=({
struct Cyc_List_List _temp154; _temp154.hd=( void*)&(( struct _tuple10*) specs)[
_check_known_subscript_notnull( 15u, 13)]; _temp154.tl=( struct Cyc_List_List*)&
_temp152; _temp154;}); goto _LL156; _LL156: { struct Cyc_List_List _temp158=({
struct Cyc_List_List _temp157; _temp157.hd=( void*)&(( struct _tuple10*) specs)[
_check_known_subscript_notnull( 15u, 14)]; _temp157.tl=( struct Cyc_List_List*)&
_temp155; _temp157;}); goto _LL159; _LL159: { struct Cyc_List_List* _temp160=&
_temp158; goto _LL161; _LL161: Cyc_Arg_parse(( struct Cyc_List_List*) _temp160,
Cyc_compile_file,( struct _tagged_string)({ char* _temp162=( char*)"Options:";
struct _tagged_string _temp163; _temp163.curr= _temp162; _temp163.base= _temp162;
_temp163.last_plus_one= _temp162 + 9; _temp163;}), argv); return Cyc_compile_failure?
1: 0;}}}}}}}}}}}}}}}}