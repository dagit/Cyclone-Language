#include "cyc_include.h"

 struct _tagged_ptr0{ struct _tagged_string* curr; struct _tagged_string* base;
struct _tagged_string* last_plus_one; } ; struct _tuple0{ void* f1; struct
_tagged_string* f2; } ; struct _tuple1{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual*
f2; void* f3; } ; struct _tuple2{ struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt*
f2; } ; struct _tagged_ptr1{ int* curr; int* base; int* last_plus_one; } ;
struct _tuple3{ void* f1; int f2; } ; struct _tuple4{ struct Cyc_Declarator* f1;
struct Cyc_Absyn_Exp* f2; } ; struct _tuple5{ struct Cyc_Absyn_Tqual* f1; struct
Cyc_List_List* f2; struct Cyc_List_List* f3; } ; struct _tuple6{ struct Cyc_Declarator*
f1; struct Cyc_Core_Opt* f2; } ; struct _tuple7{ struct Cyc_List_List* f1; int
f2; struct Cyc_Core_Opt* f3; } ; struct _tuple8{ struct Cyc_List_List* f1;
struct Cyc_Absyn_Pat* f2; } ; struct _tuple9{ struct _tagged_string f1; void* f2;
struct _tagged_string f3; } ; typedef int Cyc_ptrdiff_t; typedef unsigned int
Cyc_size_t; typedef unsigned short Cyc_wchar_t; typedef unsigned int Cyc_wint_t;
typedef char Cyc_u_char; typedef unsigned short Cyc_u_short; typedef
unsigned int Cyc_u_int; typedef unsigned int Cyc_u_long; typedef unsigned short
Cyc_ushort; typedef unsigned int Cyc_uint; typedef unsigned int Cyc_clock_t;
typedef int Cyc_time_t; struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct
Cyc_itimerspec{ struct Cyc_timespec it_interval; struct Cyc_timespec it_value; }
; typedef int Cyc_daddr_t; typedef char* Cyc_caddr_t; typedef unsigned int Cyc_ino_t;
typedef unsigned int Cyc_vm_offset_t; typedef unsigned int Cyc_vm_size_t;
typedef char Cyc_int8_t; typedef char Cyc_u_int8_t; typedef short Cyc_int16_t;
typedef unsigned short Cyc_u_int16_t; typedef int Cyc_int32_t; typedef
unsigned int Cyc_u_int32_t; typedef long long Cyc_int64_t; typedef
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
Cyc_List_rev( struct Cyc_List_List* x); extern char Cyc_List_Nth_tag[ 4u];
struct Cyc_List_Nth_struct{ char* tag; } ; extern char Cyc_Arg_Error_tag[ 6u];
struct Cyc_Arg_Error_struct{ char* tag; } ; extern char Cyc_Arg_Bad_tag[ 4u];
struct Cyc_Arg_Bad_struct{ char* tag; struct _tagged_string f1; } ; typedef void*
Cyc_Arg_Spec; static const int Cyc_Arg_Unit_spec_tag= 0; struct Cyc_Arg_Unit_spec_struct{
int tag; void(* f1)(); } ; static const int Cyc_Arg_Set_spec_tag= 1; struct Cyc_Arg_Set_spec_struct{
int tag; int* f1; } ; static const int Cyc_Arg_Clear_spec_tag= 2; struct Cyc_Arg_Clear_spec_struct{
int tag; int* f1; } ; static const int Cyc_Arg_String_spec_tag= 3; struct Cyc_Arg_String_spec_struct{
int tag; void(* f1)( struct _tagged_string); } ; static const int Cyc_Arg_Int_spec_tag=
4; struct Cyc_Arg_Int_spec_struct{ int tag; void(* f1)( int); } ; static const
int Cyc_Arg_Rest_spec_tag= 5; struct Cyc_Arg_Rest_spec_struct{ int tag; void(*
f1)( struct _tagged_string); } ; typedef void* Cyc_Arg_gspec_t; typedef void*
Cyc_Arg_spec_t; extern void Cyc_Arg_parse( struct Cyc_List_List* specs, void(*
anonfun)( struct _tagged_string), struct _tagged_string errmsg, struct
_tagged_ptr0 args); struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE
Cyc_Stdio_FILE; extern struct Cyc_Stdio___sFILE* Cyc_Stdio_stdout; extern struct
Cyc_Stdio___sFILE* Cyc_Stdio_stderr; typedef int Cyc_Stdio_fpos_t; extern int
Cyc_Stdio_remove( struct _tagged_string); extern int Cyc_Stdio_fclose( struct
Cyc_Stdio___sFILE*); extern int Cyc_Stdio_fflush( struct Cyc_Stdio___sFILE*);
extern char Cyc_Stdio_FileOpenError_tag[ 14u]; struct Cyc_Stdio_FileOpenError_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Stdio_FileCloseError_tag[
15u]; struct Cyc_Stdio_FileCloseError_struct{ char* tag; } ; extern struct Cyc_Stdio___sFILE*
Cyc_Stdio_file_open( struct _tagged_string fname, struct _tagged_string mode);
extern void Cyc_Stdio_file_close( struct Cyc_Stdio___sFILE*); extern int Cyc_String_strcmp(
struct _tagged_string s1, struct _tagged_string s2); extern struct
_tagged_string Cyc_String_strconcat( struct _tagged_string, struct
_tagged_string); extern struct _tagged_string Cyc_String_str_sepstr( struct Cyc_List_List*,
struct _tagged_string); struct Cyc_Dict_Dict; typedef struct Cyc_Dict_Dict* Cyc_Dict_hdict_t;
typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t; extern char Cyc_Dict_Present_tag[
8u]; struct Cyc_Dict_Present_struct{ char* tag; } ; extern char Cyc_Dict_Absent_tag[
7u]; struct Cyc_Dict_Absent_struct{ char* tag; } ; extern struct _tagged_string
Cyc_Filename_chop_extension( struct _tagged_string); struct Cyc_Id_id; typedef
struct Cyc_Id_id* Cyc_Id_Id; struct Cyc_Lineno_Pos{ struct _tagged_string
logical_file; struct _tagged_string line; int line_no; int col; } ; typedef
struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t; extern char Cyc_Position_Exit_tag[ 5u];
struct Cyc_Position_Exit_struct{ char* tag; } ; extern void Cyc_Position_reset_position(
struct _tagged_string); struct Cyc_Position_Segment; typedef struct Cyc_Position_Segment*
Cyc_Position_seg_t; typedef void* Cyc_Position_Error_kind; static const
unsigned int Cyc_Position_Lex= 0; static const unsigned int Cyc_Position_Parse=
1; static const unsigned int Cyc_Position_Elab= 2; typedef void* Cyc_Position_error_kind_t;
struct Cyc_Position_Error{ struct _tagged_string source; struct Cyc_Position_Segment*
seg; void* kind; struct _tagged_string desc; } ; typedef struct Cyc_Position_Error*
Cyc_Position_error_t; extern char Cyc_Position_Nocontext_tag[ 10u]; struct Cyc_Position_Nocontext_struct{
char* tag; } ; extern int Cyc_Position_error_p(); typedef struct _tagged_string*
Cyc_Absyn_field_name_t; typedef struct _tagged_string* Cyc_Absyn_var_t; typedef
struct _tagged_string* Cyc_Absyn_tvarname_t; typedef void* Cyc_Absyn_Nmspace;
typedef void* Cyc_Absyn_nmspace_t; typedef struct _tuple0* Cyc_Absyn_qvar_t;
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
char* tag; } ; extern char Cyc_Lexing_Error_tag[ 6u]; struct Cyc_Lexing_lexbuf{
void(* refill_buff)( struct Cyc_Lexing_lexbuf*); void* refill_state; struct
_tagged_string lex_buffer; int lex_buffer_len; int lex_abs_pos; int
lex_start_pos; int lex_curr_pos; int lex_last_pos; int lex_last_action; int
lex_eof_reached; } ; typedef struct Cyc_Lexing_lexbuf* Cyc_Lexing_Lexbuf; struct
Cyc_Lexing_function_lexbuf_state{ int(* read_fun)( struct _tagged_string, int,
void*); void* read_fun_state; } ; typedef struct Cyc_Lexing_function_lexbuf_state*
Cyc_Lexing_Function_lexbuf_state; struct Cyc_Lexing_lex_tables{ struct
_tagged_ptr1 lex_base; struct _tagged_ptr1 lex_backtrk; struct _tagged_ptr1
lex_default; struct _tagged_ptr1 lex_trans; struct _tagged_ptr1 lex_check; } ;
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
_tagged_string text; } ; typedef struct Cyc_Yyltype Cyc_yyltype; struct Cyc_PP_Ppstate;
typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t; struct Cyc_PP_Out; typedef
struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc; typedef struct Cyc_PP_Doc*
Cyc_PP_doc_t; extern int Cyc_Absynpp_expand_typedefs; extern int Cyc_Absynpp_qvar_to_Cids;
extern int Cyc_Absynpp_decls_first; extern int Cyc_Absynpp_print_all_tvars;
extern int Cyc_Absynpp_print_all_kinds; extern int Cyc_Absynpp_print_using_stmts;
extern int Cyc_Absynpp_print_externC_stmts; extern void Cyc_Absynpp_decllist2file(
struct Cyc_List_List* tdl, struct Cyc_Stdio___sFILE* f); extern void Cyc_Absyndump_dumpdecllist2file(
struct Cyc_List_List* tdl, struct Cyc_Stdio___sFILE* f); struct Cyc_Set_Set;
typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct Cyc_Set_Set* Cyc_Set_hset_t;
typedef struct Cyc_Set_Set* Cyc_Set_set_t; extern char Cyc_Set_Absent_tag[ 7u];
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
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tc_init(); extern char Cyc_Tcutil_TypeErr_tag[
8u]; struct Cyc_Tcutil_TypeErr_struct{ char* tag; } ; extern void Cyc_Tcutil_flush_warnings();
extern void Cyc_Tc_tc( struct Cyc_Tcenv_Tenv* te, int add_cyc_namespace, struct
Cyc_List_List* ds); extern struct Cyc_List_List* Cyc_Tc_treeshake( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List*); extern struct Cyc_List_List* Cyc_Toc_toc( struct Cyc_List_List*
ds); extern int Cyc_Toc_check_for_null; typedef void* Cyc_CfFlowInfo_LocalRoot;
typedef void* Cyc_CfFlowInfo_Field; struct Cyc_CfFlowInfo_Place; typedef void*
Cyc_CfFlowInfo_Escaped; typedef void* Cyc_CfFlowInfo_InitLevel; typedef void*
Cyc_CfFlowInfo_InitState; typedef void* Cyc_CfFlowInfo_PathInfo; typedef void*
Cyc_CfFlowInfo_FlowInfo; typedef void* Cyc_CfFlowInfo_local_root_t; typedef void*
Cyc_CfFlowInfo_field_t; typedef struct Cyc_CfFlowInfo_Place* Cyc_CfFlowInfo_place_t;
typedef void* Cyc_CfFlowInfo_escaped_t; typedef void* Cyc_CfFlowInfo_init_level_t;
typedef void* Cyc_CfFlowInfo_init_state_t; typedef void* Cyc_CfFlowInfo_path_info_t;
typedef struct Cyc_Dict_Dict* Cyc_CfFlowInfo_pinfo_dict_t; typedef void* Cyc_CfFlowInfo_flow_info_t;
static const int Cyc_CfFlowInfo_VarRoot_tag= 0; struct Cyc_CfFlowInfo_VarRoot_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_CfFlowInfo_MallocPt_tag=
1; struct Cyc_CfFlowInfo_MallocPt_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_CfFlowInfo_StructF_tag= 0; struct Cyc_CfFlowInfo_StructF_struct{
int tag; struct _tagged_string* f1; } ; static const int Cyc_CfFlowInfo_TupleF_tag=
1; struct Cyc_CfFlowInfo_TupleF_struct{ int tag; int f1; } ; struct Cyc_CfFlowInfo_Place{
void* root; struct Cyc_List_List* fields; } ; static const unsigned int Cyc_CfFlowInfo_Esc=
0; static const unsigned int Cyc_CfFlowInfo_Unesc= 1; static const unsigned int
Cyc_CfFlowInfo_NoneIL= 0; static const unsigned int Cyc_CfFlowInfo_ThisIL= 1;
static const unsigned int Cyc_CfFlowInfo_AllIL= 2; static const int Cyc_CfFlowInfo_UnknownIS_tag=
0; struct Cyc_CfFlowInfo_UnknownIS_struct{ int tag; void* f1; void* f2; } ;
static const int Cyc_CfFlowInfo_MustPointTo_tag= 1; struct Cyc_CfFlowInfo_MustPointTo_struct{
int tag; struct Cyc_CfFlowInfo_Place* f1; } ; static const int Cyc_CfFlowInfo_LeafPI_tag=
0; struct Cyc_CfFlowInfo_LeafPI_struct{ int tag; void* f1; } ; static const int
Cyc_CfFlowInfo_TuplePI_tag= 1; struct Cyc_CfFlowInfo_TuplePI_struct{ int tag;
struct Cyc_Dict_Dict* f1; } ; static const int Cyc_CfFlowInfo_StructPI_tag= 2;
struct Cyc_CfFlowInfo_StructPI_struct{ int tag; struct Cyc_Dict_Dict* f1; } ;
static const unsigned int Cyc_CfFlowInfo_BottomFL= 0; static const int Cyc_CfFlowInfo_InitsFL_tag=
0; struct Cyc_CfFlowInfo_InitsFL_struct{ int tag; struct Cyc_Dict_Dict* f1; } ;
struct Cyc_NewControlFlow_AnalEnv{ struct Cyc_Dict_Dict* roots; int in_try; void*
tryflow; } ; typedef struct Cyc_NewControlFlow_AnalEnv* Cyc_NewControlFlow_analenv_t;
void Cyc_NewControlFlow_cf_check( struct Cyc_List_List* ds); extern void Cyc_Lex_lex_init();
static int Cyc_pp_r= 0; static int Cyc_up_r= 0; static int Cyc_noexpand_r= 0;
static int Cyc_noshake_r= 0; static int Cyc_tc_r= 0; static int Cyc_toc_r= 0;
static int Cyc_v_r= 0; static int Cyc_q_r= 0; static int Cyc_add_cyc_namespace_r=
1; int* Cyc_pp=& Cyc_pp_r; int* Cyc_up=& Cyc_up_r; int* Cyc_noexpand=& Cyc_noexpand_r;
int* Cyc_noshake=& Cyc_noshake_r; int* Cyc_tc=& Cyc_tc_r; int* Cyc_toc=& Cyc_toc_r;
int* Cyc_v=& Cyc_v_r; int* Cyc_q=& Cyc_q_r; int* Cyc_add_cyc_namespace=& Cyc_add_cyc_namespace_r;
static struct _tagged_string* Cyc_out_file_name_opt= 0; static void Cyc_set_out_file_name(
struct _tagged_string s){ Cyc_out_file_name_opt=( struct _tagged_string*)({
struct _tagged_string* _temp0=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string)); _temp0[ 0]= s; _temp0;});} static void Cyc_set_check_null(){
Cyc_Toc_check_for_null= 1;} static struct Cyc_List_List* Cyc_includes= 0; static
void Cyc_add_include( struct _tagged_string s){ Cyc_includes=({ struct Cyc_List_List*
_temp1=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1->hd=( void*)({ struct _tagged_string* _temp2=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp2[ 0]= s; _temp2;}); _temp1->tl=
Cyc_includes; _temp1;});} static struct Cyc_List_List* Cyc_defines= 0; static
void Cyc_add_define( struct _tagged_string s){ Cyc_defines=({ struct Cyc_List_List*
_temp3=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3->hd=( void*)({ struct _tagged_string* _temp4=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp4[ 0]= s; _temp4;}); _temp3->tl=
Cyc_defines; _temp3;});} int Cyc_compile_failure= 0; struct Cyc_Stdio___sFILE*
Cyc_try_file_open( struct _tagged_string filename, struct _tagged_string mode,
struct _tagged_string msg_part){ struct _handler_cons _temp5; _push_handler(&
_temp5);{ struct _xtunion_struct* _temp6=( struct _xtunion_struct*) setjmp(
_temp5.handler); if( ! _temp6){{ struct Cyc_Stdio___sFILE* _temp7=( struct Cyc_Stdio___sFILE*)
Cyc_Stdio_file_open( filename, mode); _npop_handler( 0u); return _temp7;};
_pop_handler();} else{ struct _xtunion_struct* _temp9= _temp6; _LL11: goto _LL12;
_LL13: goto _LL14; _LL12: Cyc_compile_failure= 1;({ struct _tagged_string
_temp15= msg_part; struct _tagged_string _temp16= filename; fprintf( Cyc_Stdio_stderr,"\nError: couldn't open %.*s %.*s\n",
_temp15.last_plus_one - _temp15.curr, _temp15.curr, _temp16.last_plus_one -
_temp16.curr, _temp16.curr);}); Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*)
Cyc_Stdio_stderr); return 0; _LL14:( void) _throw( _temp9); _LL10:;}}} struct
Cyc_List_List* Cyc_do_stage( struct _tagged_string stage_name, struct Cyc_List_List*(*
f)( void*, struct Cyc_List_List*), void* env, struct Cyc_List_List* tds, struct
_tagged_string preprocfile){ struct _tagged_string exn_string=( struct
_tagged_string)({ char* _temp17=( char*)""; struct _tagged_string _temp18;
_temp18.curr= _temp17; _temp18.base= _temp17; _temp18.last_plus_one= _temp17 + 1;
_temp18;}); struct _tagged_string explain_string=( struct _tagged_string)({ char*
_temp19=( char*)""; struct _tagged_string _temp20; _temp20.curr= _temp19;
_temp20.base= _temp19; _temp20.last_plus_one= _temp19 + 1; _temp20;}); int
other_exn= 0; struct _xtunion_struct* ex=( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp21=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp21[ 0]=({ struct Cyc_Core_Impossible_struct _temp22; _temp22.tag= Cyc_Core_Impossible_tag;
_temp22.f1=( struct _tagged_string)({ char* _temp23=( char*)""; struct
_tagged_string _temp24; _temp24.curr= _temp23; _temp24.base= _temp23; _temp24.last_plus_one=
_temp23 + 1; _temp24;}); _temp22;}); _temp21;}); struct Cyc_List_List* ans= 0;{
struct _handler_cons _temp25; _push_handler(& _temp25);{ struct _xtunion_struct*
_temp26=( struct _xtunion_struct*) setjmp( _temp25.handler); if( ! _temp26){ ans=
f( env, tds);; _pop_handler();} else{ struct _xtunion_struct* _temp28= _temp26;
struct _tagged_string _temp38; _LL30: if((*(( struct _xtunion_struct*) _temp28)).tag
== Cyc_Core_Impossible_tag){ _LL39: _temp38=(( struct Cyc_Core_Impossible_struct*)
_temp28)->f1; goto _LL31;} else{ goto _LL32;} _LL32: if( _temp28->tag == Cyc_Dict_Absent_tag){
goto _LL33;} else{ goto _LL34;} _LL34: goto _LL35; _LL36: goto _LL37; _LL31:
exn_string=( struct _tagged_string)({ char* _temp40=( char*)"Exception Core::Impossible";
struct _tagged_string _temp41; _temp41.curr= _temp40; _temp41.base= _temp40;
_temp41.last_plus_one= _temp40 + 27; _temp41;}); explain_string= _temp38; goto
_LL29; _LL33: exn_string=( struct _tagged_string)({ char* _temp42=( char*)"Exception Dict::Absent";
struct _tagged_string _temp43; _temp43.curr= _temp42; _temp43.base= _temp42;
_temp43.last_plus_one= _temp42 + 23; _temp43;}); goto _LL29; _LL35: ex= _temp28;
other_exn= 1; exn_string=( struct _tagged_string)({ char* _temp44=( char*)"Uncaught exception";
struct _tagged_string _temp45; _temp45.curr= _temp44; _temp45.base= _temp44;
_temp45.last_plus_one= _temp44 + 19; _temp45;}); goto _LL29; _LL37:( void)
_throw( _temp28); _LL29:;}}} if( Cyc_Position_error_p()){ Cyc_compile_failure= 1;}
if( Cyc_String_strcmp( exn_string,( struct _tagged_string)({ char* _temp46=(
char*)""; struct _tagged_string _temp47; _temp47.curr= _temp46; _temp47.base=
_temp46; _temp47.last_plus_one= _temp46 + 1; _temp47;})) != 0){ Cyc_compile_failure=
1;({ struct _tagged_string _temp48= exn_string; struct _tagged_string _temp49=
stage_name; struct _tagged_string _temp50= explain_string; fprintf( Cyc_Stdio_stderr,"\n%.*s thrown during %.*s: %.*s",
_temp48.last_plus_one - _temp48.curr, _temp48.curr, _temp49.last_plus_one -
_temp49.curr, _temp49.curr, _temp50.last_plus_one - _temp50.curr, _temp50.curr);});}
if( Cyc_compile_failure){ fprintf( Cyc_Stdio_stderr,"\nCOMPILATION FAILED!\n");
Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*) Cyc_Stdio_stderr); Cyc_Stdio_remove(
preprocfile); if( other_exn){( void) _throw( ex);} return ans;} if(* Cyc_v){({
struct _tagged_string _temp51= stage_name; fprintf( Cyc_Stdio_stderr,"%.*s completed.\n",
_temp51.last_plus_one - _temp51.curr, _temp51.curr);});} else{ if( !(* Cyc_q)){
fprintf( Cyc_Stdio_stderr,".");}} Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*)
Cyc_Stdio_stderr); return ans;} struct Cyc_List_List* Cyc_do_parse( struct Cyc_Stdio___sFILE*
f, struct Cyc_List_List* ignore){ Cyc_Lex_lex_init(); return Cyc_Parse_parse_file(
f);} struct Cyc_List_List* Cyc_do_typecheck( struct Cyc_Tcenv_Tenv* te, struct
Cyc_List_List* tds){ Cyc_Absynpp_expand_typedefs= !(* Cyc_noexpand); Cyc_Tc_tc(
te, Cyc_add_cyc_namespace_r? Cyc_toc_r: 0, tds); if( !(* Cyc_noshake)){ tds= Cyc_Tc_treeshake(
te, tds);} return tds;} struct Cyc_List_List* Cyc_do_cfcheck( int ignore, struct
Cyc_List_List* tds){ Cyc_NewControlFlow_cf_check( tds); return tds;} struct Cyc_List_List*
Cyc_do_translate( int ignore, struct Cyc_List_List* tds){ return Cyc_Toc_toc(
tds);} struct Cyc_List_List* Cyc_do_print( struct Cyc_Stdio___sFILE* out_file,
struct Cyc_List_List* tds){ Cyc_Absynpp_expand_typedefs= !(* Cyc_noexpand); Cyc_Absynpp_decls_first=(
Cyc_Absynpp_qvar_to_Cids=* Cyc_toc); Cyc_Absynpp_print_using_stmts=( Cyc_Absynpp_print_externC_stmts=
!(* Cyc_tc)); if(* Cyc_toc){ fprintf( out_file,"#include \"cyc_include.h\"\n\n");
Cyc_Absynpp_qvar_to_Cids= 1;} if(* Cyc_pp){ Cyc_Absynpp_decllist2file( tds,
out_file);} else{ if(* Cyc_up){ Cyc_Absyndump_dumpdecllist2file( tds, out_file);}}
Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*) out_file); return tds;} void Cyc_compile_file(
struct _tagged_string filename){ struct _tagged_string basename= Cyc_Filename_chop_extension(
filename); struct _tagged_string preprocfile= Cyc_String_strconcat( basename,(
struct _tagged_string)({ char* _temp52=( char*)".i"; struct _tagged_string
_temp53; _temp53.curr= _temp52; _temp53.base= _temp52; _temp53.last_plus_one=
_temp52 + 3; _temp53;})); if( !(* Cyc_q)? 1:* Cyc_v){({ struct _tagged_string
_temp54= basename; fprintf( Cyc_Stdio_stderr,"Compiling file: %.*s", _temp54.last_plus_one
- _temp54.curr, _temp54.curr);});} if(* Cyc_v){ fprintf( Cyc_Stdio_stderr,"\n");}{
struct _tagged_string inc_string= Cyc_String_str_sepstr(({ struct Cyc_List_List*
_temp55=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp55->hd=( void*)({ struct _tagged_string* _temp56=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp56[ 0]=( struct _tagged_string)({
char* _temp57=( char*)""; struct _tagged_string _temp58; _temp58.curr= _temp57;
_temp58.base= _temp57; _temp58.last_plus_one= _temp57 + 1; _temp58;}); _temp56;});
_temp55->tl=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(
Cyc_includes); _temp55;}),( struct _tagged_string)({ char* _temp59=( char*)" -I";
struct _tagged_string _temp60; _temp60.curr= _temp59; _temp60.base= _temp59;
_temp60.last_plus_one= _temp59 + 4; _temp60;})); struct _tagged_string
def_string= Cyc_String_str_sepstr(({ struct Cyc_List_List* _temp61=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp61->hd=( void*)({ struct
_tagged_string* _temp62=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp62[ 0]=( struct _tagged_string)({ char* _temp63=( char*)"";
struct _tagged_string _temp64; _temp64.curr= _temp63; _temp64.base= _temp63;
_temp64.last_plus_one= _temp63 + 1; _temp64;}); _temp62;}); _temp61->tl=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( Cyc_defines);
_temp61;}),( struct _tagged_string)({ char* _temp65=( char*)" -D"; struct
_tagged_string _temp66; _temp66.curr= _temp65; _temp66.base= _temp65; _temp66.last_plus_one=
_temp65 + 4; _temp66;})); struct Cyc_Stdio___sFILE* f0= Cyc_try_file_open(
filename,( struct _tagged_string)({ char* _temp67=( char*)"r"; struct
_tagged_string _temp68; _temp68.curr= _temp67; _temp68.base= _temp67; _temp68.last_plus_one=
_temp67 + 2; _temp68;}),( struct _tagged_string)({ char* _temp69=( char*)"input file";
struct _tagged_string _temp70; _temp70.curr= _temp69; _temp70.base= _temp69;
_temp70.last_plus_one= _temp69 + 11; _temp70;})); if( Cyc_compile_failure){
return;} Cyc_Stdio_fclose(( struct Cyc_Stdio___sFILE*)(( struct Cyc_Stdio___sFILE*)({
struct Cyc_Stdio___sFILE* _temp71= f0; if( _temp71 == 0){ _throw( Null_Exception);}
_temp71;})));{ struct _tagged_string cmd=({ struct _tagged_string _temp72=
inc_string; struct _tagged_string _temp73= def_string; struct _tagged_string
_temp74= filename; struct _tagged_string _temp75= preprocfile; xprintf("gcc -x c -E -U__GNUC__ %.*s %.*s %.*s > %.*s",
_temp72.last_plus_one - _temp72.curr, _temp72.curr, _temp73.last_plus_one -
_temp73.curr, _temp73.curr, _temp74.last_plus_one - _temp74.curr, _temp74.curr,
_temp75.last_plus_one - _temp75.curr, _temp75.curr);}); if(* Cyc_v){({ struct
_tagged_string _temp76= cmd; fprintf( Cyc_Stdio_stderr,"%.*s\n", _temp76.last_plus_one
- _temp76.curr, _temp76.curr);});} if( system( string_to_Cstring( cmd)) != 0){
Cyc_compile_failure= 1; fprintf( Cyc_Stdio_stderr,"\nError: pre-processing failed!\n");
return;} Cyc_Position_reset_position( preprocfile);{ struct Cyc_Stdio___sFILE*
in_file= Cyc_try_file_open( preprocfile,( struct _tagged_string)({ char* _temp77=(
char*)"r"; struct _tagged_string _temp78; _temp78.curr= _temp77; _temp78.base=
_temp77; _temp78.last_plus_one= _temp77 + 2; _temp78;}),( struct _tagged_string)({
char* _temp79=( char*)"file"; struct _tagged_string _temp80; _temp80.curr=
_temp79; _temp80.base= _temp79; _temp80.last_plus_one= _temp79 + 5; _temp80;}));
struct Cyc_Stdio___sFILE* out_file=( struct Cyc_Stdio___sFILE*) Cyc_Stdio_stdout;
if( Cyc_compile_failure){ return;}{ struct Cyc_List_List* tds= 0; if( ! Cyc_compile_failure){
tds=(( struct Cyc_List_List*(*)( struct _tagged_string stage_name, struct Cyc_List_List*(*
f)( struct Cyc_Stdio___sFILE*, struct Cyc_List_List*), struct Cyc_Stdio___sFILE*
env, struct Cyc_List_List* tds, struct _tagged_string preprocfile)) Cyc_do_stage)((
struct _tagged_string)({ char* _temp81=( char*)"parsing"; struct _tagged_string
_temp82; _temp82.curr= _temp81; _temp82.base= _temp81; _temp82.last_plus_one=
_temp81 + 8; _temp82;}), Cyc_do_parse,( struct Cyc_Stdio___sFILE*)({ struct Cyc_Stdio___sFILE*
_temp83= in_file; if( _temp83 == 0){ _throw( Null_Exception);} _temp83;}), tds,
preprocfile); Cyc_Stdio_file_close(( struct Cyc_Stdio___sFILE*)({ struct Cyc_Stdio___sFILE*
_temp84= in_file; if( _temp84 == 0){ _throw( Null_Exception);} _temp84;}));} if((
! Cyc_compile_failure? tds != 0: 0)?* Cyc_tc: 0){ tds=(( struct Cyc_List_List*(*)(
struct _tagged_string stage_name, struct Cyc_List_List*(* f)( struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List*), struct Cyc_Tcenv_Tenv* env, struct Cyc_List_List* tds,
struct _tagged_string preprocfile)) Cyc_do_stage)(( struct _tagged_string)({
char* _temp85=( char*)"type checking"; struct _tagged_string _temp86; _temp86.curr=
_temp85; _temp86.base= _temp85; _temp86.last_plus_one= _temp85 + 14; _temp86;}),
Cyc_do_typecheck, Cyc_Tcenv_tc_init(), tds, preprocfile);} if( ! Cyc_compile_failure?*
Cyc_tc: 0){ tds=(( struct Cyc_List_List*(*)( struct _tagged_string stage_name,
struct Cyc_List_List*(* f)( int, struct Cyc_List_List*), int env, struct Cyc_List_List*
tds, struct _tagged_string preprocfile)) Cyc_do_stage)(( struct _tagged_string)({
char* _temp87=( char*)"control-flow checking"; struct _tagged_string _temp88;
_temp88.curr= _temp87; _temp88.base= _temp87; _temp88.last_plus_one= _temp87 +
22; _temp88;}), Cyc_do_cfcheck, 1, tds, preprocfile);} if( ! Cyc_compile_failure?*
Cyc_toc: 0){ tds=(( struct Cyc_List_List*(*)( struct _tagged_string stage_name,
struct Cyc_List_List*(* f)( int, struct Cyc_List_List*), int env, struct Cyc_List_List*
tds, struct _tagged_string preprocfile)) Cyc_do_stage)(( struct _tagged_string)({
char* _temp89=( char*)"translation to C"; struct _tagged_string _temp90; _temp90.curr=
_temp89; _temp90.base= _temp89; _temp90.last_plus_one= _temp89 + 17; _temp90;}),
Cyc_do_translate, 1, tds, preprocfile);} if( ! Cyc_compile_failure? Cyc_out_file_name_opt
!= 0: 0){ out_file= Cyc_try_file_open(* Cyc_out_file_name_opt,( struct
_tagged_string)({ char* _temp91=( char*)"w"; struct _tagged_string _temp92;
_temp92.curr= _temp91; _temp92.base= _temp91; _temp92.last_plus_one= _temp91 + 2;
_temp92;}),( struct _tagged_string)({ char* _temp93=( char*)"output file";
struct _tagged_string _temp94; _temp94.curr= _temp93; _temp94.base= _temp93;
_temp94.last_plus_one= _temp93 + 12; _temp94;}));} if(( ! Cyc_compile_failure?
tds != 0: 0)?* Cyc_pp? 1:* Cyc_up: 0){ tds=(( struct Cyc_List_List*(*)( struct
_tagged_string stage_name, struct Cyc_List_List*(* f)( struct Cyc_Stdio___sFILE*,
struct Cyc_List_List*), struct Cyc_Stdio___sFILE* env, struct Cyc_List_List* tds,
struct _tagged_string preprocfile)) Cyc_do_stage)(( struct _tagged_string)({
char* _temp95=( char*)"printing"; struct _tagged_string _temp96; _temp96.curr=
_temp95; _temp96.base= _temp95; _temp96.last_plus_one= _temp95 + 9; _temp96;}),
Cyc_do_print,( struct Cyc_Stdio___sFILE*)({ struct Cyc_Stdio___sFILE* _temp97=
out_file; if( _temp97 == 0){ _throw( Null_Exception);} _temp97;}), tds,
preprocfile);} if( out_file != 0? out_file !=( struct Cyc_Stdio___sFILE*) Cyc_Stdio_stdout:
0){ Cyc_Stdio_file_close(( struct Cyc_Stdio___sFILE*)({ struct Cyc_Stdio___sFILE*
_temp98= out_file; if( _temp98 == 0){ _throw( Null_Exception);} _temp98;}));}
fprintf( Cyc_Stdio_stderr,"\n"); if( ! Cyc_compile_failure){ Cyc_Tcutil_flush_warnings();}
Cyc_Stdio_remove( preprocfile); return;}}}}} int Cyc_main( int argc, struct
_tagged_ptr0 argv){ struct _tuple9 specs[ 15u]={({ struct _tuple9 _temp99;
_temp99.f1=( struct _tagged_string)({ char* _temp104=( char*)"-nocyc"; struct
_tagged_string _temp105; _temp105.curr= _temp104; _temp105.base= _temp104;
_temp105.last_plus_one= _temp104 + 7; _temp105;}); _temp99.f2=( void*)({ struct
Cyc_Arg_Clear_spec_struct* _temp102=( struct Cyc_Arg_Clear_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Clear_spec_struct)); _temp102[ 0]=({ struct
Cyc_Arg_Clear_spec_struct _temp103; _temp103.tag= Cyc_Arg_Clear_spec_tag;
_temp103.f1= Cyc_add_cyc_namespace; _temp103;}); _temp102;}); _temp99.f3=(
struct _tagged_string)({ char* _temp100=( char*)"does not add implicit namespace Cyc";
struct _tagged_string _temp101; _temp101.curr= _temp100; _temp101.base= _temp100;
_temp101.last_plus_one= _temp100 + 36; _temp101;}); _temp99;}),({ struct _tuple9
_temp106; _temp106.f1=( struct _tagged_string)({ char* _temp111=( char*)"-q";
struct _tagged_string _temp112; _temp112.curr= _temp111; _temp112.base= _temp111;
_temp112.last_plus_one= _temp111 + 3; _temp112;}); _temp106.f2=( void*)({ struct
Cyc_Arg_Set_spec_struct* _temp109=( struct Cyc_Arg_Set_spec_struct*) GC_malloc(
sizeof( struct Cyc_Arg_Set_spec_struct)); _temp109[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp110; _temp110.tag= Cyc_Arg_Set_spec_tag; _temp110.f1= Cyc_q; _temp110;});
_temp109;}); _temp106.f3=( struct _tagged_string)({ char* _temp107=( char*)"compile quietly";
struct _tagged_string _temp108; _temp108.curr= _temp107; _temp108.base= _temp107;
_temp108.last_plus_one= _temp107 + 16; _temp108;}); _temp106;}),({ struct
_tuple9 _temp113; _temp113.f1=( struct _tagged_string)({ char* _temp118=( char*)"-v";
struct _tagged_string _temp119; _temp119.curr= _temp118; _temp119.base= _temp118;
_temp119.last_plus_one= _temp118 + 3; _temp119;}); _temp113.f2=( void*)({ struct
Cyc_Arg_Set_spec_struct* _temp116=( struct Cyc_Arg_Set_spec_struct*) GC_malloc(
sizeof( struct Cyc_Arg_Set_spec_struct)); _temp116[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp117; _temp117.tag= Cyc_Arg_Set_spec_tag; _temp117.f1= Cyc_v; _temp117;});
_temp116;}); _temp113.f3=( struct _tagged_string)({ char* _temp114=( char*)"print compilation stages verbosely";
struct _tagged_string _temp115; _temp115.curr= _temp114; _temp115.base= _temp114;
_temp115.last_plus_one= _temp114 + 35; _temp115;}); _temp113;}),({ struct
_tuple9 _temp120; _temp120.f1=( struct _tagged_string)({ char* _temp125=( char*)"-check_null";
struct _tagged_string _temp126; _temp126.curr= _temp125; _temp126.base= _temp125;
_temp126.last_plus_one= _temp125 + 12; _temp126;}); _temp120.f2=( void*)({
struct Cyc_Arg_Unit_spec_struct* _temp123=( struct Cyc_Arg_Unit_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Unit_spec_struct)); _temp123[ 0]=({ struct Cyc_Arg_Unit_spec_struct
_temp124; _temp124.tag= Cyc_Arg_Unit_spec_tag; _temp124.f1= Cyc_set_check_null;
_temp124;}); _temp123;}); _temp120.f3=( struct _tagged_string)({ char* _temp121=(
char*)"turn on null-checking in generated C code"; struct _tagged_string
_temp122; _temp122.curr= _temp121; _temp122.base= _temp121; _temp122.last_plus_one=
_temp121 + 42; _temp122;}); _temp120;}),({ struct _tuple9 _temp127; _temp127.f1=(
struct _tagged_string)({ char* _temp132=( char*)"-D"; struct _tagged_string
_temp133; _temp133.curr= _temp132; _temp133.base= _temp132; _temp133.last_plus_one=
_temp132 + 3; _temp133;}); _temp127.f2=( void*)({ struct Cyc_Arg_String_spec_struct*
_temp130=( struct Cyc_Arg_String_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_String_spec_struct));
_temp130[ 0]=({ struct Cyc_Arg_String_spec_struct _temp131; _temp131.tag= Cyc_Arg_String_spec_tag;
_temp131.f1= Cyc_add_define; _temp131;}); _temp130;}); _temp127.f3=( struct
_tagged_string)({ char* _temp128=( char*)"pass definition to pre-processor";
struct _tagged_string _temp129; _temp129.curr= _temp128; _temp129.base= _temp128;
_temp129.last_plus_one= _temp128 + 33; _temp129;}); _temp127;}),({ struct
_tuple9 _temp134; _temp134.f1=( struct _tagged_string)({ char* _temp139=( char*)"-I";
struct _tagged_string _temp140; _temp140.curr= _temp139; _temp140.base= _temp139;
_temp140.last_plus_one= _temp139 + 3; _temp140;}); _temp134.f2=( void*)({ struct
Cyc_Arg_String_spec_struct* _temp137=( struct Cyc_Arg_String_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_String_spec_struct)); _temp137[ 0]=({ struct
Cyc_Arg_String_spec_struct _temp138; _temp138.tag= Cyc_Arg_String_spec_tag;
_temp138.f1= Cyc_add_include; _temp138;}); _temp137;}); _temp134.f3=( struct
_tagged_string)({ char* _temp135=( char*)"include directory"; struct
_tagged_string _temp136; _temp136.curr= _temp135; _temp136.base= _temp135;
_temp136.last_plus_one= _temp135 + 18; _temp136;}); _temp134;}),({ struct
_tuple9 _temp141; _temp141.f1=( struct _tagged_string)({ char* _temp146=( char*)"-toc";
struct _tagged_string _temp147; _temp147.curr= _temp146; _temp147.base= _temp146;
_temp147.last_plus_one= _temp146 + 5; _temp147;}); _temp141.f2=( void*)({ struct
Cyc_Arg_Set_spec_struct* _temp144=( struct Cyc_Arg_Set_spec_struct*) GC_malloc(
sizeof( struct Cyc_Arg_Set_spec_struct)); _temp144[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp145; _temp145.tag= Cyc_Arg_Set_spec_tag; _temp145.f1= Cyc_toc; _temp145;});
_temp144;}); _temp141.f3=( struct _tagged_string)({ char* _temp142=( char*)"translate to C";
struct _tagged_string _temp143; _temp143.curr= _temp142; _temp143.base= _temp142;
_temp143.last_plus_one= _temp142 + 15; _temp143;}); _temp141;}),({ struct
_tuple9 _temp148; _temp148.f1=( struct _tagged_string)({ char* _temp153=( char*)"-tc";
struct _tagged_string _temp154; _temp154.curr= _temp153; _temp154.base= _temp153;
_temp154.last_plus_one= _temp153 + 4; _temp154;}); _temp148.f2=( void*)({ struct
Cyc_Arg_Set_spec_struct* _temp151=( struct Cyc_Arg_Set_spec_struct*) GC_malloc(
sizeof( struct Cyc_Arg_Set_spec_struct)); _temp151[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp152; _temp152.tag= Cyc_Arg_Set_spec_tag; _temp152.f1= Cyc_tc; _temp152;});
_temp151;}); _temp148.f3=( struct _tagged_string)({ char* _temp149=( char*)"type check";
struct _tagged_string _temp150; _temp150.curr= _temp149; _temp150.base= _temp149;
_temp150.last_plus_one= _temp149 + 11; _temp150;}); _temp148;}),({ struct
_tuple9 _temp155; _temp155.f1=( struct _tagged_string)({ char* _temp160=( char*)"-noremoveunused";
struct _tagged_string _temp161; _temp161.curr= _temp160; _temp161.base= _temp160;
_temp161.last_plus_one= _temp160 + 16; _temp161;}); _temp155.f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp158=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp158[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp159; _temp159.tag= Cyc_Arg_Set_spec_tag; _temp159.f1= Cyc_noshake; _temp159;});
_temp158;}); _temp155.f3=( struct _tagged_string)({ char* _temp156=( char*)"don't remove externed variables that aren't used";
struct _tagged_string _temp157; _temp157.curr= _temp156; _temp157.base= _temp156;
_temp157.last_plus_one= _temp156 + 49; _temp157;}); _temp155;}),({ struct
_tuple9 _temp162; _temp162.f1=( struct _tagged_string)({ char* _temp167=( char*)"-noexpandtypedefs";
struct _tagged_string _temp168; _temp168.curr= _temp167; _temp168.base= _temp167;
_temp168.last_plus_one= _temp167 + 18; _temp168;}); _temp162.f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp165=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp165[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp166; _temp166.tag= Cyc_Arg_Set_spec_tag; _temp166.f1= Cyc_noexpand;
_temp166;}); _temp165;}); _temp162.f3=( struct _tagged_string)({ char* _temp163=(
char*)"don't expand typedefs in pretty printing"; struct _tagged_string _temp164;
_temp164.curr= _temp163; _temp164.base= _temp163; _temp164.last_plus_one=
_temp163 + 41; _temp164;}); _temp162;}),({ struct _tuple9 _temp169; _temp169.f1=(
struct _tagged_string)({ char* _temp174=( char*)"-printalltvars"; struct
_tagged_string _temp175; _temp175.curr= _temp174; _temp175.base= _temp174;
_temp175.last_plus_one= _temp174 + 15; _temp175;}); _temp169.f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp172=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp172[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp173; _temp173.tag= Cyc_Arg_Set_spec_tag; _temp173.f1=& Cyc_Absynpp_print_all_tvars;
_temp173;}); _temp172;}); _temp169.f3=( struct _tagged_string)({ char* _temp170=(
char*)"print all type variables (even implicit default effects)"; struct
_tagged_string _temp171; _temp171.curr= _temp170; _temp171.base= _temp170;
_temp171.last_plus_one= _temp170 + 57; _temp171;}); _temp169;}),({ struct
_tuple9 _temp176; _temp176.f1=( struct _tagged_string)({ char* _temp181=( char*)"-printallkinds";
struct _tagged_string _temp182; _temp182.curr= _temp181; _temp182.base= _temp181;
_temp182.last_plus_one= _temp181 + 15; _temp182;}); _temp176.f2=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp179=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp179[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp180; _temp180.tag= Cyc_Arg_Set_spec_tag; _temp180.f1=& Cyc_Absynpp_print_all_kinds;
_temp180;}); _temp179;}); _temp176.f3=( struct _tagged_string)({ char* _temp177=(
char*)"always print kinds of type variables"; struct _tagged_string _temp178;
_temp178.curr= _temp177; _temp178.base= _temp177; _temp178.last_plus_one=
_temp177 + 37; _temp178;}); _temp176;}),({ struct _tuple9 _temp183; _temp183.f1=(
struct _tagged_string)({ char* _temp188=( char*)"-up"; struct _tagged_string
_temp189; _temp189.curr= _temp188; _temp189.base= _temp188; _temp189.last_plus_one=
_temp188 + 4; _temp189;}); _temp183.f2=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp186=( struct Cyc_Arg_Set_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp186[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp187; _temp187.tag= Cyc_Arg_Set_spec_tag;
_temp187.f1= Cyc_up; _temp187;}); _temp186;}); _temp183.f3=( struct
_tagged_string)({ char* _temp184=( char*)"ugly print"; struct _tagged_string
_temp185; _temp185.curr= _temp184; _temp185.base= _temp184; _temp185.last_plus_one=
_temp184 + 11; _temp185;}); _temp183;}),({ struct _tuple9 _temp190; _temp190.f1=(
struct _tagged_string)({ char* _temp195=( char*)"-pp"; struct _tagged_string
_temp196; _temp196.curr= _temp195; _temp196.base= _temp195; _temp196.last_plus_one=
_temp195 + 4; _temp196;}); _temp190.f2=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp193=( struct Cyc_Arg_Set_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp193[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp194; _temp194.tag= Cyc_Arg_Set_spec_tag;
_temp194.f1= Cyc_pp; _temp194;}); _temp193;}); _temp190.f3=( struct
_tagged_string)({ char* _temp191=( char*)"pretty print"; struct _tagged_string
_temp192; _temp192.curr= _temp191; _temp192.base= _temp191; _temp192.last_plus_one=
_temp191 + 13; _temp192;}); _temp190;}),({ struct _tuple9 _temp197; _temp197.f1=(
struct _tagged_string)({ char* _temp202=( char*)"-o"; struct _tagged_string
_temp203; _temp203.curr= _temp202; _temp203.base= _temp202; _temp203.last_plus_one=
_temp202 + 3; _temp203;}); _temp197.f2=( void*)({ struct Cyc_Arg_String_spec_struct*
_temp200=( struct Cyc_Arg_String_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_String_spec_struct));
_temp200[ 0]=({ struct Cyc_Arg_String_spec_struct _temp201; _temp201.tag= Cyc_Arg_String_spec_tag;
_temp201.f1= Cyc_set_out_file_name; _temp201;}); _temp200;}); _temp197.f3=(
struct _tagged_string)({ char* _temp198=( char*)"Output file name"; struct
_tagged_string _temp199; _temp199.curr= _temp198; _temp199.base= _temp198;
_temp199.last_plus_one= _temp198 + 17; _temp199;}); _temp197;})}; struct Cyc_List_List
l1=({ struct Cyc_List_List _temp204; _temp204.hd=( void*)({ struct _tuple9*
_temp205=( struct _tuple9*) specs; unsigned int _temp206= 0; if( _temp206 >= 15u){
_throw( Null_Exception);}& _temp205[ _temp206];}); _temp204.tl= 0; _temp204;});
struct Cyc_List_List l2=({ struct Cyc_List_List _temp207; _temp207.hd=( void*)({
struct _tuple9* _temp208=( struct _tuple9*) specs; unsigned int _temp209= 1; if(
_temp209 >= 15u){ _throw( Null_Exception);}& _temp208[ _temp209];}); _temp207.tl=(
struct Cyc_List_List*)& l1; _temp207;}); struct Cyc_List_List l3=({ struct Cyc_List_List
_temp210; _temp210.hd=( void*)({ struct _tuple9* _temp211=( struct _tuple9*)
specs; unsigned int _temp212= 2; if( _temp212 >= 15u){ _throw( Null_Exception);}&
_temp211[ _temp212];}); _temp210.tl=( struct Cyc_List_List*)& l2; _temp210;});
struct Cyc_List_List l4=({ struct Cyc_List_List _temp213; _temp213.hd=( void*)({
struct _tuple9* _temp214=( struct _tuple9*) specs; unsigned int _temp215= 3; if(
_temp215 >= 15u){ _throw( Null_Exception);}& _temp214[ _temp215];}); _temp213.tl=(
struct Cyc_List_List*)& l3; _temp213;}); struct Cyc_List_List l5=({ struct Cyc_List_List
_temp216; _temp216.hd=( void*)({ struct _tuple9* _temp217=( struct _tuple9*)
specs; unsigned int _temp218= 4; if( _temp218 >= 15u){ _throw( Null_Exception);}&
_temp217[ _temp218];}); _temp216.tl=( struct Cyc_List_List*)& l4; _temp216;});
struct Cyc_List_List l6=({ struct Cyc_List_List _temp219; _temp219.hd=( void*)({
struct _tuple9* _temp220=( struct _tuple9*) specs; unsigned int _temp221= 5; if(
_temp221 >= 15u){ _throw( Null_Exception);}& _temp220[ _temp221];}); _temp219.tl=(
struct Cyc_List_List*)& l5; _temp219;}); struct Cyc_List_List l7=({ struct Cyc_List_List
_temp222; _temp222.hd=( void*)({ struct _tuple9* _temp223=( struct _tuple9*)
specs; unsigned int _temp224= 6; if( _temp224 >= 15u){ _throw( Null_Exception);}&
_temp223[ _temp224];}); _temp222.tl=( struct Cyc_List_List*)& l6; _temp222;});
struct Cyc_List_List l8=({ struct Cyc_List_List _temp225; _temp225.hd=( void*)({
struct _tuple9* _temp226=( struct _tuple9*) specs; unsigned int _temp227= 7; if(
_temp227 >= 15u){ _throw( Null_Exception);}& _temp226[ _temp227];}); _temp225.tl=(
struct Cyc_List_List*)& l7; _temp225;}); struct Cyc_List_List l9=({ struct Cyc_List_List
_temp228; _temp228.hd=( void*)({ struct _tuple9* _temp229=( struct _tuple9*)
specs; unsigned int _temp230= 8; if( _temp230 >= 15u){ _throw( Null_Exception);}&
_temp229[ _temp230];}); _temp228.tl=( struct Cyc_List_List*)& l8; _temp228;});
struct Cyc_List_List l10=({ struct Cyc_List_List _temp231; _temp231.hd=( void*)({
struct _tuple9* _temp232=( struct _tuple9*) specs; unsigned int _temp233= 9; if(
_temp233 >= 15u){ _throw( Null_Exception);}& _temp232[ _temp233];}); _temp231.tl=(
struct Cyc_List_List*)& l9; _temp231;}); struct Cyc_List_List l11=({ struct Cyc_List_List
_temp234; _temp234.hd=( void*)({ struct _tuple9* _temp235=( struct _tuple9*)
specs; unsigned int _temp236= 10; if( _temp236 >= 15u){ _throw( Null_Exception);}&
_temp235[ _temp236];}); _temp234.tl=( struct Cyc_List_List*)& l10; _temp234;});
struct Cyc_List_List l12=({ struct Cyc_List_List _temp237; _temp237.hd=( void*)({
struct _tuple9* _temp238=( struct _tuple9*) specs; unsigned int _temp239= 11;
if( _temp239 >= 15u){ _throw( Null_Exception);}& _temp238[ _temp239];});
_temp237.tl=( struct Cyc_List_List*)& l11; _temp237;}); struct Cyc_List_List l13=({
struct Cyc_List_List _temp240; _temp240.hd=( void*)({ struct _tuple9* _temp241=(
struct _tuple9*) specs; unsigned int _temp242= 12; if( _temp242 >= 15u){ _throw(
Null_Exception);}& _temp241[ _temp242];}); _temp240.tl=( struct Cyc_List_List*)&
l12; _temp240;}); struct Cyc_List_List l14=({ struct Cyc_List_List _temp243;
_temp243.hd=( void*)({ struct _tuple9* _temp244=( struct _tuple9*) specs;
unsigned int _temp245= 13; if( _temp245 >= 15u){ _throw( Null_Exception);}&
_temp244[ _temp245];}); _temp243.tl=( struct Cyc_List_List*)& l13; _temp243;});
struct Cyc_List_List l15=({ struct Cyc_List_List _temp246; _temp246.hd=( void*)({
struct _tuple9* _temp247=( struct _tuple9*) specs; unsigned int _temp248= 14;
if( _temp248 >= 15u){ _throw( Null_Exception);}& _temp247[ _temp248];});
_temp246.tl=( struct Cyc_List_List*)& l14; _temp246;}); struct Cyc_List_List*
options=& l15; Cyc_Arg_parse(( struct Cyc_List_List*) options, Cyc_compile_file,(
struct _tagged_string)({ char* _temp249=( char*)"Options:"; struct
_tagged_string _temp250; _temp250.curr= _temp249; _temp250.base= _temp249;
_temp250.last_plus_one= _temp249 + 9; _temp250;}), argv); return Cyc_compile_failure?
1: 0;}