#include "cyc_include.h"

 struct _tuple0{ void* f1; void* f2; } ; struct _tuple1{ void* f1; struct
_tagged_string* f2; } ; struct _tuple2{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual*
f2; void* f3; } ; struct _tuple3{ struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt*
f2; } ; struct _tuple4{ struct Cyc_Absyn_Tqual* f1; void* f2; } ; struct _tuple5{
struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ; typedef int Cyc_ptrdiff_t;
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
struct Cyc_Core_Opt* Cyc_Core_opt_t; extern void* Cyc_Core_snd( struct _tuple0*);
extern int Cyc_Core_intcmp( int, int); extern char Cyc_Core_InvalidArg_tag[ 11u];
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
struct Cyc_List_List_empty_struct{ char* tag; } ; extern struct Cyc_List_List*
Cyc_List_tl( struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_map(
void*(* f)( void*), struct Cyc_List_List* x); extern char Cyc_List_List_mismatch_tag[
14u]; struct Cyc_List_List_mismatch_struct{ char* tag; } ; extern struct Cyc_List_List*
Cyc_List_imp_rev( struct Cyc_List_List* x); extern char Cyc_List_Nth_tag[ 4u];
struct Cyc_List_Nth_struct{ char* tag; } ; struct Cyc_Dict_Dict; typedef struct
Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t;
extern char Cyc_Dict_Present_tag[ 8u]; struct Cyc_Dict_Present_struct{ char* tag;
} ; extern char Cyc_Dict_Absent_tag[ 7u]; struct Cyc_Dict_Absent_struct{ char*
tag; } ; extern struct Cyc_Dict_Dict* Cyc_Dict_empty( int(* comp)( void*, void*));
extern struct Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict* d, void* key,
void* data); extern struct Cyc_Dict_Dict* Cyc_Dict_insert_new( struct Cyc_Dict_Dict*
d, void* key, void* data); extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d,
void* key); struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE;
typedef int Cyc_Stdio_fpos_t; extern char Cyc_Stdio_FileOpenError_tag[ 14u];
struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Stdio_FileCloseError_tag[ 15u]; struct Cyc_Stdio_FileCloseError_struct{
char* tag; } ; extern int Cyc_String_zstrptrcmp( struct _tagged_string*, struct
_tagged_string*); extern void* Cyc_Assert_AssertFail( struct _tagged_string msg);
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
char* tag; } ; extern struct Cyc_Absyn_Exp* Cyc_Absyn_true_exp( struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_false_exp( struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp( unsigned int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_structmember_exp( struct Cyc_Absyn_Exp*,
struct _tagged_string*, struct Cyc_Position_Segment*); struct Cyc_Set_Set;
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
extern char Cyc_Tcutil_TypeErr_tag[ 8u]; struct Cyc_Tcutil_TypeErr_struct{ char*
tag; } ; extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*, struct
_tagged_string); extern void* Cyc_Tcutil_compress( void* t); extern unsigned int
Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp* e); typedef void* Cyc_CfFlowInfo_LocalRoot;
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
extern int Cyc_CfFlowInfo_local_root_cmp( void*, void*); extern void* Cyc_CfFlowInfo_typ_to_unesc_none_pinfo(
void* t); extern void* Cyc_CfFlowInfo_assign_unknown_dict( void*, void*); extern
void* Cyc_CfFlowInfo_join_flow( void*, void*); extern int Cyc_CfFlowInfo_flow_lessthan_approx(
void* f1, void* f2); extern void Cyc_NewControlFlow_set_encloser( struct Cyc_Absyn_Stmt*
enclosee, struct Cyc_Absyn_Stmt* encloser); struct Cyc_NewControlFlow_AnalEnv{
struct Cyc_Dict_Dict* roots; int in_try; void* tryflow; } ; typedef struct Cyc_NewControlFlow_AnalEnv*
Cyc_NewControlFlow_analenv_t; extern void Cyc_NewControlFlow_update_tryflow(
struct Cyc_NewControlFlow_AnalEnv* env, void* new_flow); extern void* Cyc_NewControlFlow_cf_analyze_stmt(
struct Cyc_NewControlFlow_AnalEnv*, struct Cyc_Absyn_Stmt*, void*); void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List* ds); extern char Cyc_CfAbsexp_BadAbsexp_tag[ 10u]; struct
Cyc_CfAbsexp_BadAbsexp_struct{ char* tag; } ; typedef void* Cyc_CfAbsexp_Absexp;
typedef void* Cyc_CfAbsexp_Absexpgroup; static const unsigned int Cyc_CfAbsexp_OrderedG=
0; static const unsigned int Cyc_CfAbsexp_UnorderedG= 1; static const
unsigned int Cyc_CfAbsexp_OneofG= 2; typedef void* Cyc_CfAbsexp_Absop; typedef
void* Cyc_CfAbsexp_absexp_t; typedef void* Cyc_CfAbsexp_absexpgroup_t; typedef
void* Cyc_CfAbsexp_absop_t; extern void* Cyc_CfAbsexp_mkAnyOrderG(); extern void*
Cyc_CfAbsexp_mkUnknownOp(); extern void* Cyc_CfAbsexp_mkAddressOp( void* ao);
extern void* Cyc_CfAbsexp_mkLocalOp( struct Cyc_Absyn_Vardecl* vd); extern void*
Cyc_CfAbsexp_mkMemberOp( void* ao, void* f); extern void* Cyc_CfAbsexp_mkMallocOp(
struct Cyc_Absyn_Exp* e); extern void* Cyc_CfAbsexp_mkDerefOp( void* ao); extern
void* Cyc_CfAbsexp_mkBottomAE(); extern void* Cyc_CfAbsexp_mkSkipAE(); extern
void* Cyc_CfAbsexp_mkAssignAE( void* l, void* r); extern void* Cyc_CfAbsexp_mkMallocAE(
struct Cyc_Absyn_Exp* e); extern void* Cyc_CfAbsexp_mkStmtAE( struct Cyc_Absyn_Stmt*
s); extern void* Cyc_CfAbsexp_mkGroupAE( void* g, void* ae1, void* ae2); extern
void* Cyc_CfAbsexp_mkGroupAE_l( void* g, struct Cyc_List_List* ael); extern int
Cyc_CfAbsexp_isUnknownOp( void*); extern void* Cyc_CfAbsexp_eval_absexp( struct
Cyc_NewControlFlow_AnalEnv* env, struct Cyc_Position_Segment* loc, void* ae,
void* in_flow); struct Cyc_PP_Ppstate; typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t;
struct Cyc_PP_Out; typedef struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc;
typedef struct Cyc_PP_Doc* Cyc_PP_doc_t; struct Cyc_NewControlFlow_CFStmtAnnot{
struct Cyc_Absyn_Stmt* encloser; int visited; void* flow; struct Cyc_List_List*
absexps; } ; typedef struct Cyc_NewControlFlow_CFStmtAnnot Cyc_NewControlFlow_cf_stmt_annot_t;
char Cyc_NewControlFlow_CFAnnot_tag[ 8u]="CFAnnot"; struct Cyc_NewControlFlow_CFAnnot_struct{
char* tag; struct Cyc_NewControlFlow_CFStmtAnnot f1; } ; void Cyc_NewControlFlow_set_encloser(
struct Cyc_Absyn_Stmt* enclosee, struct Cyc_Absyn_Stmt* encloser){ enclosee->annot=(
struct _xtunion_struct*)({ struct Cyc_NewControlFlow_CFAnnot_struct* _temp0=(
struct Cyc_NewControlFlow_CFAnnot_struct*) GC_malloc( sizeof( struct Cyc_NewControlFlow_CFAnnot_struct));
_temp0[ 0]=({ struct Cyc_NewControlFlow_CFAnnot_struct _temp1; _temp1.tag= Cyc_NewControlFlow_CFAnnot_tag;
_temp1.f1=({ struct Cyc_NewControlFlow_CFStmtAnnot _temp2; _temp2.encloser=
encloser; _temp2.visited= 0; _temp2.flow=( void*)(( void*) Cyc_CfFlowInfo_BottomFL);
_temp2.absexps= 0; _temp2;}); _temp1;}); _temp0;});} static struct Cyc_NewControlFlow_CFStmtAnnot*
Cyc_NewControlFlow_get_stmt_annot( struct Cyc_Absyn_Stmt* s){ struct
_xtunion_struct* _temp3= s->annot; struct Cyc_NewControlFlow_CFStmtAnnot _temp9;
struct Cyc_NewControlFlow_CFStmtAnnot* _temp11; _LL5: if((*(( struct
_xtunion_struct*) _temp3)).tag == Cyc_NewControlFlow_CFAnnot_tag){ _LL10: _temp9=((
struct Cyc_NewControlFlow_CFAnnot_struct*) _temp3)->f1; _temp11=&(( struct Cyc_NewControlFlow_CFAnnot_struct*)
_temp3)->f1; goto _LL6;} else{ goto _LL7;} _LL7: goto _LL8; _LL6: return _temp11;
_LL8:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp12=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp12[ 0]=({ struct Cyc_Core_Impossible_struct _temp13; _temp13.tag= Cyc_Core_Impossible_tag;
_temp13.f1=( struct _tagged_string)({ char* _temp14=( char*)"ControlFlow -- wrong stmt annotation";
struct _tagged_string _temp15; _temp15.curr= _temp14; _temp15.base= _temp14;
_temp15.last_plus_one= _temp14 + 37; _temp15;}); _temp13;}); _temp12;})); _LL4:;}
void Cyc_NewControlFlow_cf_set_absexps( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List*
ael){( Cyc_NewControlFlow_get_stmt_annot( s))->absexps= ael;} struct Cyc_NewControlFlow_SharedAbsEnv{
struct Cyc_Dict_Dict* all_roots; struct Cyc_List_List* comprehension_vars; } ;
struct Cyc_NewControlFlow_AbsEnv{ struct Cyc_NewControlFlow_SharedAbsEnv* shared;
struct Cyc_List_List* lhsides; struct Cyc_List_List* access_path; } ; typedef
struct Cyc_NewControlFlow_AbsEnv Cyc_NewControlFlow_absenv_t; struct Cyc_NewControlFlow_AbsSynOne{
void* inner_exp; void* assigns; } ; typedef struct Cyc_NewControlFlow_AbsSynOne
Cyc_NewControlFlow_abssyn_one_t; struct Cyc_NewControlFlow_AbsSyn{ struct Cyc_NewControlFlow_AbsSynOne
when_true; struct Cyc_NewControlFlow_AbsSynOne when_false; struct Cyc_List_List*
lvalues; } ; typedef struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_abssyn_t;
void Cyc_NewControlFlow_add_var_root( struct Cyc_NewControlFlow_AbsEnv env,
struct Cyc_Absyn_Vardecl* vd){( env.shared)->all_roots=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert_new)(( env.shared)->all_roots,(
void*)({ struct Cyc_CfFlowInfo_VarRoot_struct* _temp16=( struct Cyc_CfFlowInfo_VarRoot_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct)); _temp16[ 0]=({ struct
Cyc_CfFlowInfo_VarRoot_struct _temp17; _temp17.tag= Cyc_CfFlowInfo_VarRoot_tag;
_temp17.f1= vd; _temp17;}); _temp16;}), Cyc_CfFlowInfo_typ_to_unesc_none_pinfo((
void*) vd->type));} void Cyc_NewControlFlow_add_malloc_root( struct Cyc_NewControlFlow_AbsEnv
env, struct Cyc_Absyn_Exp* e, void* t){( env.shared)->all_roots=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert_new)(( env.shared)->all_roots,(
void*)({ struct Cyc_CfFlowInfo_MallocPt_struct* _temp18=( struct Cyc_CfFlowInfo_MallocPt_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct)); _temp18[ 0]=({
struct Cyc_CfFlowInfo_MallocPt_struct _temp19; _temp19.tag= Cyc_CfFlowInfo_MallocPt_tag;
_temp19.f1= e; _temp19;}); _temp18;}), Cyc_CfFlowInfo_typ_to_unesc_none_pinfo( t));}
void Cyc_NewControlFlow_add_tunion_root( struct Cyc_NewControlFlow_AbsEnv env,
struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Tunionfield* ef){ struct Cyc_Absyn_Tunionfield
_temp22; struct Cyc_Position_Segment* _temp23; struct Cyc_List_List* _temp25;
struct Cyc_List_List* _temp27; struct _tuple1* _temp29; struct Cyc_Absyn_Tunionfield*
_temp20= ef; _temp22=* _temp20; _LL30: _temp29=( struct _tuple1*) _temp22.name;
goto _LL28; _LL28: _temp27=( struct Cyc_List_List*) _temp22.tvs; goto _LL26;
_LL26: _temp25=( struct Cyc_List_List*) _temp22.typs; goto _LL24; _LL24: _temp23=(
struct Cyc_Position_Segment*) _temp22.loc; goto _LL21; _LL21: { struct Cyc_Dict_Dict*
d=(( struct Cyc_Dict_Dict*(*)( int(* comp)( int, int))) Cyc_Dict_empty)( Cyc_Core_intcmp);{
int i= 0; for( 0; _temp25 != 0; _temp25=({ struct Cyc_List_List* _temp31=
_temp25; if( _temp31 == 0){ _throw( Null_Exception);} _temp31->tl;}), ++ i){
struct _tuple4 _temp35; void* _temp36; struct Cyc_Absyn_Tqual* _temp38; struct
_tuple4* _temp33=( struct _tuple4*)({ struct Cyc_List_List* _temp32= _temp25;
if( _temp32 == 0){ _throw( Null_Exception);} _temp32->hd;}); _temp35=* _temp33;
_LL39: _temp38= _temp35.f1; goto _LL37; _LL37: _temp36= _temp35.f2; goto _LL34;
_LL34: d=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, int key, void*
data)) Cyc_Dict_insert)( d, i, Cyc_CfFlowInfo_typ_to_unesc_none_pinfo( _temp36));}}(
env.shared)->all_roots=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
void* key, void* data)) Cyc_Dict_insert_new)(( env.shared)->all_roots,( void*)({
struct Cyc_CfFlowInfo_MallocPt_struct* _temp40=( struct Cyc_CfFlowInfo_MallocPt_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct)); _temp40[ 0]=({
struct Cyc_CfFlowInfo_MallocPt_struct _temp41; _temp41.tag= Cyc_CfFlowInfo_MallocPt_tag;
_temp41.f1= e; _temp41;}); _temp40;}),( void*)({ struct Cyc_CfFlowInfo_TuplePI_struct*
_temp42=( struct Cyc_CfFlowInfo_TuplePI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct));
_temp42[ 0]=({ struct Cyc_CfFlowInfo_TuplePI_struct _temp43; _temp43.tag= Cyc_CfFlowInfo_TuplePI_tag;
_temp43.f1= d; _temp43;}); _temp42;}));}} struct Cyc_NewControlFlow_AbsEnv Cyc_NewControlFlow_change_lhs(
struct Cyc_NewControlFlow_AbsEnv src, struct Cyc_List_List* new_lhs){ return({
struct Cyc_NewControlFlow_AbsEnv _temp44; _temp44.shared= src.shared; _temp44.lhsides=
new_lhs; _temp44.access_path= src.access_path; _temp44;});} static struct Cyc_List_List*
Cyc_NewControlFlow_unknown_list= 0; static struct Cyc_List_List* Cyc_NewControlFlow_use_it_list(){
if( Cyc_NewControlFlow_unknown_list == 0){ Cyc_NewControlFlow_unknown_list=({
struct Cyc_List_List* _temp45=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp45->hd=( void*) Cyc_CfAbsexp_mkUnknownOp(); _temp45->tl= 0;
_temp45;});} return Cyc_NewControlFlow_unknown_list;} struct Cyc_NewControlFlow_AbsEnv
Cyc_NewControlFlow_use_it_env( struct Cyc_NewControlFlow_AbsEnv src){ struct Cyc_NewControlFlow_AbsEnv
ans= Cyc_NewControlFlow_change_lhs( src, Cyc_NewControlFlow_use_it_list()); ans.access_path=
0; return ans;} struct Cyc_NewControlFlow_AbsSynOne Cyc_NewControlFlow_meet_absexp(
struct Cyc_NewControlFlow_AbsSyn syn){ struct Cyc_List_List* _temp48; struct Cyc_NewControlFlow_AbsSynOne
_temp50; void* _temp52; void* _temp54; struct Cyc_NewControlFlow_AbsSynOne
_temp56; void* _temp58; void* _temp60; struct Cyc_NewControlFlow_AbsSyn _temp46=
syn; _LL57: _temp56=( struct Cyc_NewControlFlow_AbsSynOne) _temp46.when_true;
_LL61: _temp60=( void*) _temp56.inner_exp; goto _LL59; _LL59: _temp58=( void*)
_temp56.assigns; goto _LL51; _LL51: _temp50=( struct Cyc_NewControlFlow_AbsSynOne)
_temp46.when_false; _LL55: _temp54=( void*) _temp50.inner_exp; goto _LL53; _LL53:
_temp52=( void*) _temp50.assigns; goto _LL49; _LL49: _temp48=( struct Cyc_List_List*)
_temp46.lvalues; goto _LL47; _LL47: { void* inn= _temp60 == _temp54? _temp60:
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OneofG, _temp60, _temp54); void*
ass= _temp58 == _temp52? _temp58: Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OneofG,
_temp58, _temp52); return({ struct Cyc_NewControlFlow_AbsSynOne _temp62; _temp62.inner_exp=(
void*) inn; _temp62.assigns=( void*) ass; _temp62;});}} struct Cyc_NewControlFlow_AbsSyn
Cyc_NewControlFlow_mkSyn_tf( void* ae, struct Cyc_List_List* lvals, void*
assigns){ struct Cyc_NewControlFlow_AbsSynOne one_way=({ struct Cyc_NewControlFlow_AbsSynOne
_temp63; _temp63.inner_exp=( void*) ae; _temp63.assigns=( void*) assigns;
_temp63;}); return({ struct Cyc_NewControlFlow_AbsSyn _temp64; _temp64.when_true=
one_way; _temp64.when_false= one_way; _temp64.lvalues= lvals; _temp64;});} void*
Cyc_NewControlFlow_make_assigns( struct Cyc_NewControlFlow_AbsEnv env, void* ae){
if( env.lhsides == 0){ return Cyc_CfAbsexp_mkSkipAE();} if(({ struct Cyc_List_List*
_temp65= env.lhsides; if( _temp65 == 0){ _throw( Null_Exception);} _temp65->tl;})
== 0){ return Cyc_CfAbsexp_mkAssignAE(( void*)({ struct Cyc_List_List* _temp66=
env.lhsides; if( _temp66 == 0){ _throw( Null_Exception);} _temp66->hd;}), ae);}{
struct Cyc_List_List* l= 0;{ struct Cyc_List_List* lhsides= env.lhsides; for( 0;
lhsides != 0; lhsides=({ struct Cyc_List_List* _temp67= lhsides; if( _temp67 ==
0){ _throw( Null_Exception);} _temp67->tl;})){ l=({ struct Cyc_List_List*
_temp68=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp68->hd=( void*) Cyc_CfAbsexp_mkAssignAE(( void*)({ struct Cyc_List_List*
_temp69= lhsides; if( _temp69 == 0){ _throw( Null_Exception);} _temp69->hd;}),
ae); _temp68->tl= l; _temp68;});}} return Cyc_CfAbsexp_mkGroupAE_l(( void*) Cyc_CfAbsexp_OneofG,
l);}} struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_abstract_exp( struct
Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e); void Cyc_NewControlFlow_cf_prepass(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Stmt* s); struct Cyc_NewControlFlow_AbsSyn
Cyc_NewControlFlow_use_exp_unordered( struct Cyc_NewControlFlow_AbsEnv env,
struct Cyc_List_List* es, struct Cyc_List_List* lhs){ struct Cyc_NewControlFlow_AbsEnv
elt_env= Cyc_NewControlFlow_use_it_env( env); void* es_exp= Cyc_CfAbsexp_mkSkipAE();
void* uses_exp= Cyc_CfAbsexp_mkSkipAE(); for( 0; es != 0; es=({ struct Cyc_List_List*
_temp70= es; if( _temp70 == 0){ _throw( Null_Exception);} _temp70->tl;})){
struct Cyc_NewControlFlow_AbsSyn syn= Cyc_NewControlFlow_abstract_exp( elt_env,(
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp71= es; if( _temp71 == 0){
_throw( Null_Exception);} _temp71->hd;})); void* _temp74; void* _temp76; struct
Cyc_NewControlFlow_AbsSynOne _temp72= Cyc_NewControlFlow_meet_absexp( syn);
_LL77: _temp76=( void*) _temp72.inner_exp; goto _LL75; _LL75: _temp74=( void*)
_temp72.assigns; goto _LL73; _LL73: es_exp= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG,
_temp76, es_exp); uses_exp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_mkAnyOrderG(),
_temp74, uses_exp);} return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, es_exp, uses_exp), lhs, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));} struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_malloc_and_init(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e, void* es_exp,
void* useexp){ void* ansexp= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
Cyc_CfAbsexp_mkMallocAE( e), Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
es_exp, useexp)); return Cyc_NewControlFlow_mkSyn_tf( ansexp, 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkAddressOp( Cyc_CfAbsexp_mkMallocOp( e))));} struct Cyc_NewControlFlow_AbsSyn
Cyc_NewControlFlow_descend_path( struct Cyc_NewControlFlow_AbsEnv env, struct
Cyc_Absyn_Exp* path_e, struct Cyc_Absyn_Exp* inner_e){ env.access_path=({ struct
Cyc_List_List* _temp78=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp78->hd=( void*) path_e; _temp78->tl= env.access_path; _temp78;});{ struct
Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp( env, inner_e);
if( syn1.lvalues == 0){ syn1.lvalues= Cyc_NewControlFlow_use_it_list();} return
syn1;}} void* Cyc_NewControlFlow_destruct_path( struct Cyc_NewControlFlow_AbsEnv
env, void* absop){{ struct Cyc_List_List* path= env.access_path; for( 0; path !=
0; path=({ struct Cyc_List_List* _temp79= path; if( _temp79 == 0){ _throw(
Null_Exception);} _temp79->tl;})){ void* _temp81=( void*)(( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp80= path; if( _temp80 == 0){ _throw( Null_Exception);}
_temp80->hd;}))->r; struct Cyc_Absyn_Exp* _temp95; struct Cyc_Absyn_Exp* _temp97;
struct _tagged_string* _temp99; struct Cyc_Absyn_Exp* _temp101; struct
_tagged_string* _temp103; struct Cyc_Absyn_Exp* _temp105; struct Cyc_Absyn_Exp*
_temp107; struct Cyc_Absyn_Exp* _temp109; _LL83: if((( struct _tunion_struct*)
_temp81)->tag == Cyc_Absyn_Address_e_tag){ _LL96: _temp95=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Address_e_struct*) _temp81)->f1; goto _LL84;} else{ goto _LL85;}
_LL85: if((( struct _tunion_struct*) _temp81)->tag == Cyc_Absyn_Deref_e_tag){
_LL98: _temp97=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp81)->f1; goto _LL86;} else{ goto _LL87;} _LL87: if((( struct _tunion_struct*)
_temp81)->tag == Cyc_Absyn_StructArrow_e_tag){ _LL102: _temp101=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp81)->f1; goto _LL100; _LL100:
_temp99=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp81)->f2; goto _LL88;} else{ goto _LL89;} _LL89: if((( struct _tunion_struct*)
_temp81)->tag == Cyc_Absyn_StructMember_e_tag){ _LL106: _temp105=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp81)->f1; goto _LL104; _LL104:
_temp103=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp81)->f2; goto _LL90;} else{ goto _LL91;} _LL91: if((( struct _tunion_struct*)
_temp81)->tag == Cyc_Absyn_Subscript_e_tag){ _LL110: _temp109=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp81)->f1; goto _LL108; _LL108:
_temp107=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp81)->f2; goto _LL92;} else{ goto _LL93;} _LL93: goto _LL94; _LL84:({ struct
Cyc_List_List* _temp111= path; if( _temp111 == 0){ _throw( Null_Exception);}
_temp111->tl;}) == 0? 0:(( int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)((
struct _tagged_string)({ char* _temp112=( char*)"new_control_flow.cyc:239 path->tl == null";
struct _tagged_string _temp113; _temp113.curr= _temp112; _temp113.base= _temp112;
_temp113.last_plus_one= _temp112 + 42; _temp113;})); return Cyc_CfAbsexp_mkAddressOp(
absop); _LL86: absop= Cyc_CfAbsexp_mkDerefOp( absop); goto _LL82; _LL88: absop=
Cyc_CfAbsexp_mkDerefOp( absop); _temp103= _temp99; goto _LL90; _LL90: absop= Cyc_CfAbsexp_mkMemberOp(
absop,( void*)({ struct Cyc_CfFlowInfo_StructF_struct* _temp114=( struct Cyc_CfFlowInfo_StructF_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructF_struct)); _temp114[ 0]=({
struct Cyc_CfFlowInfo_StructF_struct _temp115; _temp115.tag= Cyc_CfFlowInfo_StructF_tag;
_temp115.f1= _temp103; _temp115;}); _temp114;})); goto _LL82; _LL92: {
unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp107); absop= Cyc_CfAbsexp_mkMemberOp(
absop,( void*)({ struct Cyc_CfFlowInfo_TupleF_struct* _temp116=( struct Cyc_CfFlowInfo_TupleF_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_TupleF_struct)); _temp116[ 0]=({ struct
Cyc_CfFlowInfo_TupleF_struct _temp117; _temp117.tag= Cyc_CfFlowInfo_TupleF_tag;
_temp117.f1=( int) i; _temp117;}); _temp116;})); goto _LL82;} _LL94:( void)
_throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct* _temp118=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp118[ 0]=({ struct Cyc_Core_Impossible_struct _temp119; _temp119.tag= Cyc_Core_Impossible_tag;
_temp119.f1=( struct _tagged_string)({ char* _temp120=( char*)"abstract_exp: unexpected acces path element";
struct _tagged_string _temp121; _temp121.curr= _temp120; _temp121.base= _temp120;
_temp121.last_plus_one= _temp120 + 44; _temp121;}); _temp119;}); _temp118;}));
_LL82:;}} return absop;} struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_abstract_exp(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e){ void* _temp122=(
void*) e->r; struct Cyc_Absyn_Exp* _temp210; struct Cyc_Absyn_Exp* _temp212;
struct Cyc_List_List* _temp214; struct Cyc_Absyn_Exp* _temp216; void* _temp218;
int _temp220; void* _temp222; void* _temp224; int _temp226; void* _temp228; void*
_temp230; void* _temp232; struct Cyc_Absyn_Enumfield* _temp234; struct Cyc_Absyn_Enumdecl*
_temp236; struct _tuple1* _temp238; void* _temp240; struct Cyc_Absyn_Fndecl*
_temp242; struct _tuple1* _temp244; void* _temp246; struct Cyc_Absyn_Vardecl*
_temp248; struct _tuple1* _temp250; void* _temp252; struct Cyc_Absyn_Vardecl*
_temp254; struct _tuple1* _temp256; void* _temp258; struct Cyc_Absyn_Vardecl*
_temp260; struct _tuple1* _temp262; void* _temp264; struct Cyc_Absyn_Vardecl*
_temp266; struct _tuple1* _temp268; struct Cyc_Absyn_Tunionfield* _temp270;
struct Cyc_Absyn_Tuniondecl* _temp272; struct Cyc_List_List* _temp274; struct
Cyc_Core_Opt* _temp276; struct Cyc_Core_Opt* _temp278; struct Cyc_Absyn_Tunionfield*
_temp280; struct Cyc_Absyn_XTuniondecl* _temp282; struct Cyc_List_List* _temp284;
struct Cyc_Core_Opt* _temp286; void* _temp288; struct Cyc_Absyn_Exp* _temp290;
struct Cyc_Absyn_Exp* _temp292; struct Cyc_Core_Opt* _temp294; struct Cyc_Absyn_Exp*
_temp296; struct Cyc_Absyn_Exp* _temp298; struct Cyc_Core_Opt* _temp300; struct
Cyc_Absyn_Exp* _temp302; struct Cyc_List_List* _temp304; void* _temp306; struct
Cyc_Absyn_Exp* _temp308; void* _temp310; struct Cyc_List_List* _temp312; struct
Cyc_Absyn_Exp* _temp314; struct Cyc_Absyn_Exp* _temp316; struct Cyc_Absyn_Exp*
_temp318; struct Cyc_Absyn_Exp* _temp320; struct Cyc_Absyn_Exp* _temp322; struct
Cyc_Absyn_Exp* _temp324; struct Cyc_Absyn_Exp* _temp326; struct _tagged_string*
_temp328; struct Cyc_Absyn_Exp* _temp330; struct _tagged_string* _temp332;
struct Cyc_Absyn_Exp* _temp334; struct Cyc_List_List* _temp336; struct Cyc_Absyn_Structdecl*
_temp338; struct Cyc_List_List* _temp340; struct Cyc_Core_Opt* _temp342; struct
_tuple1* _temp344; struct Cyc_List_List* _temp346; void* _temp348; struct Cyc_Absyn_Exp*
_temp350; struct Cyc_Absyn_Exp* _temp352; struct Cyc_Absyn_Exp* _temp354; struct
Cyc_Absyn_Exp* _temp356; struct Cyc_Absyn_Exp* _temp358; struct Cyc_Absyn_Stmt*
_temp360; struct Cyc_Absyn_Exp* _temp362; struct Cyc_Absyn_Exp* _temp364; struct
Cyc_Absyn_Exp* _temp366; struct Cyc_Absyn_Exp* _temp368; struct Cyc_Absyn_Vardecl*
_temp370; void* _temp372; struct _tuple1* _temp374; struct _tuple1* _temp376;
struct Cyc_List_List* _temp378; struct Cyc_Absyn_Exp* _temp380; struct Cyc_List_List*
_temp382; struct Cyc_Core_Opt* _temp384; struct Cyc_List_List* _temp386; struct
_tuple2* _temp388; struct Cyc_Absyn_Fndecl* _temp390; struct Cyc_Absyn_Exp*
_temp392; _LL124: if((( struct _tunion_struct*) _temp122)->tag == Cyc_Absyn_NoInstantiate_e_tag){
_LL211: _temp210=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp122)->f1; goto _LL125;} else{ goto _LL126;} _LL126: if((( struct
_tunion_struct*) _temp122)->tag == Cyc_Absyn_Sizeofexp_e_tag){ _LL213: _temp212=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Sizeofexp_e_struct*) _temp122)->f1;
goto _LL127;} else{ goto _LL128;} _LL128: if((( struct _tunion_struct*) _temp122)->tag
== Cyc_Absyn_Instantiate_e_tag){ _LL217: _temp216=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Instantiate_e_struct*) _temp122)->f1; goto _LL215; _LL215:
_temp214=( struct Cyc_List_List*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp122)->f2; goto _LL129;} else{ goto _LL130;} _LL130: if((( struct
_tunion_struct*) _temp122)->tag == Cyc_Absyn_Const_e_tag){ _LL219: _temp218=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp122)->f1; if(( unsigned int)
_temp218 > 1u?(( struct _tunion_struct*) _temp218)->tag == Cyc_Absyn_Int_c_tag:
0){ _LL223: _temp222=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp218)->f1;
if(( int) _temp222 == Cyc_Absyn_Signed){ goto _LL221;} else{ goto _LL132;}
_LL221: _temp220=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp218)->f2; if(
_temp220 == 0){ goto _LL131;} else{ goto _LL132;}} else{ goto _LL132;}} else{
goto _LL132;} _LL132: if((( struct _tunion_struct*) _temp122)->tag == Cyc_Absyn_Const_e_tag){
_LL225: _temp224=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp122)->f1; if((
unsigned int) _temp224 > 1u?(( struct _tunion_struct*) _temp224)->tag == Cyc_Absyn_Int_c_tag:
0){ _LL229: _temp228=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp224)->f1;
if(( int) _temp228 == Cyc_Absyn_Signed){ goto _LL227;} else{ goto _LL134;}
_LL227: _temp226=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp224)->f2; if(
_temp226 == 1){ goto _LL133;} else{ goto _LL134;}} else{ goto _LL134;}} else{
goto _LL134;} _LL134: if((( struct _tunion_struct*) _temp122)->tag == Cyc_Absyn_Sizeoftyp_e_tag){
_LL231: _temp230=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp122)->f1;
goto _LL135;} else{ goto _LL136;} _LL136: if((( struct _tunion_struct*) _temp122)->tag
== Cyc_Absyn_Const_e_tag){ _LL233: _temp232=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp122)->f1; goto _LL137;} else{ goto _LL138;} _LL138: if((( struct
_tunion_struct*) _temp122)->tag == Cyc_Absyn_Enum_e_tag){ _LL239: _temp238=(
struct _tuple1*)(( struct Cyc_Absyn_Enum_e_struct*) _temp122)->f1; goto _LL237;
_LL237: _temp236=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp122)->f2; goto _LL235; _LL235: _temp234=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_e_struct*) _temp122)->f3; goto _LL139;} else{ goto _LL140;}
_LL140: if((( struct _tunion_struct*) _temp122)->tag == Cyc_Absyn_Var_e_tag){
_LL245: _temp244=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp122)->f1;
goto _LL241; _LL241: _temp240=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp122)->f2; if(( unsigned int) _temp240 > 1u?(( struct _tunion_struct*)
_temp240)->tag == Cyc_Absyn_Funname_b_tag: 0){ _LL243: _temp242=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Funname_b_struct*) _temp240)->f1; goto _LL141;} else{ goto
_LL142;}} else{ goto _LL142;} _LL142: if((( struct _tunion_struct*) _temp122)->tag
== Cyc_Absyn_Var_e_tag){ _LL251: _temp250=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*)
_temp122)->f1; goto _LL247; _LL247: _temp246=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp122)->f2; if(( unsigned int) _temp246 > 1u?(( struct _tunion_struct*)
_temp246)->tag == Cyc_Absyn_Global_b_tag: 0){ _LL249: _temp248=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Global_b_struct*) _temp246)->f1; goto _LL143;} else{ goto
_LL144;}} else{ goto _LL144;} _LL144: if((( struct _tunion_struct*) _temp122)->tag
== Cyc_Absyn_Var_e_tag){ _LL257: _temp256=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*)
_temp122)->f1; goto _LL253; _LL253: _temp252=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp122)->f2; if(( unsigned int) _temp252 > 1u?(( struct _tunion_struct*)
_temp252)->tag == Cyc_Absyn_Param_b_tag: 0){ _LL255: _temp254=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Param_b_struct*) _temp252)->f1; goto _LL145;} else{ goto _LL146;}}
else{ goto _LL146;} _LL146: if((( struct _tunion_struct*) _temp122)->tag == Cyc_Absyn_Var_e_tag){
_LL263: _temp262=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp122)->f1;
goto _LL259; _LL259: _temp258=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp122)->f2; if(( unsigned int) _temp258 > 1u?(( struct _tunion_struct*)
_temp258)->tag == Cyc_Absyn_Local_b_tag: 0){ _LL261: _temp260=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Local_b_struct*) _temp258)->f1; goto _LL147;} else{ goto _LL148;}}
else{ goto _LL148;} _LL148: if((( struct _tunion_struct*) _temp122)->tag == Cyc_Absyn_Var_e_tag){
_LL269: _temp268=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp122)->f1;
goto _LL265; _LL265: _temp264=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp122)->f2; if(( unsigned int) _temp264 > 1u?(( struct _tunion_struct*)
_temp264)->tag == Cyc_Absyn_Pat_b_tag: 0){ _LL267: _temp266=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Pat_b_struct*) _temp264)->f1; goto _LL149;} else{ goto _LL150;}}
else{ goto _LL150;} _LL150: if((( struct _tunion_struct*) _temp122)->tag == Cyc_Absyn_Tunion_e_tag){
_LL279: _temp278=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp122)->f1; goto _LL277; _LL277: _temp276=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Tunion_e_struct*) _temp122)->f2; goto _LL275; _LL275: _temp274=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp122)->f3; goto
_LL273; _LL273: _temp272=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp122)->f4; goto _LL271; _LL271: _temp270=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp122)->f5; goto _LL151;} else{ goto
_LL152;} _LL152: if((( struct _tunion_struct*) _temp122)->tag == Cyc_Absyn_XTunion_e_tag){
_LL287: _temp286=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_XTunion_e_struct*)
_temp122)->f1; goto _LL285; _LL285: _temp284=( struct Cyc_List_List*)(( struct
Cyc_Absyn_XTunion_e_struct*) _temp122)->f2; goto _LL283; _LL283: _temp282=(
struct Cyc_Absyn_XTuniondecl*)(( struct Cyc_Absyn_XTunion_e_struct*) _temp122)->f3;
goto _LL281; _LL281: _temp280=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_e_struct*)
_temp122)->f4; goto _LL153;} else{ goto _LL154;} _LL154: if((( struct
_tunion_struct*) _temp122)->tag == Cyc_Absyn_Malloc_e_tag){ _LL291: _temp290=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Malloc_e_struct*) _temp122)->f1; goto
_LL289; _LL289: _temp288=( void*)(( struct Cyc_Absyn_Malloc_e_struct*) _temp122)->f2;
goto _LL155;} else{ goto _LL156;} _LL156: if((( struct _tunion_struct*) _temp122)->tag
== Cyc_Absyn_AssignOp_e_tag){ _LL297: _temp296=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp122)->f1; goto _LL295; _LL295: _temp294=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp122)->f2; if(
_temp294 == 0){ goto _LL293;} else{ goto _LL158;} _LL293: _temp292=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp122)->f3; goto _LL157;} else{ goto
_LL158;} _LL158: if((( struct _tunion_struct*) _temp122)->tag == Cyc_Absyn_AssignOp_e_tag){
_LL303: _temp302=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp122)->f1; goto _LL301; _LL301: _temp300=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp122)->f2; goto _LL299; _LL299: _temp298=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp122)->f3;
goto _LL159;} else{ goto _LL160;} _LL160: if((( struct _tunion_struct*) _temp122)->tag
== Cyc_Absyn_Primop_e_tag){ _LL307: _temp306=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp122)->f1; goto _LL305; _LL305: _temp304=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Primop_e_struct*) _temp122)->f2; goto _LL161;} else{ goto _LL162;}
_LL162: if((( struct _tunion_struct*) _temp122)->tag == Cyc_Absyn_Cast_e_tag){
_LL311: _temp310=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp122)->f1; goto
_LL309; _LL309: _temp308=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp122)->f2; goto _LL163;} else{ goto _LL164;} _LL164: if((( struct
_tunion_struct*) _temp122)->tag == Cyc_Absyn_FnCall_e_tag){ _LL315: _temp314=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*) _temp122)->f1; goto
_LL313; _LL313: _temp312=( struct Cyc_List_List*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp122)->f2; goto _LL165;} else{ goto _LL166;} _LL166: if((( struct
_tunion_struct*) _temp122)->tag == Cyc_Absyn_Subscript_e_tag){ _LL319: _temp318=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*) _temp122)->f1;
goto _LL317; _LL317: _temp316=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp122)->f2; goto _LL167;} else{ goto _LL168;} _LL168: if((( struct
_tunion_struct*) _temp122)->tag == Cyc_Absyn_New_e_tag){ _LL323: _temp322=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*) _temp122)->f1; goto
_LL321; _LL321: _temp320=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*)
_temp122)->f2; goto _LL169;} else{ goto _LL170;} _LL170: if((( struct
_tunion_struct*) _temp122)->tag == Cyc_Absyn_Address_e_tag){ _LL325: _temp324=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*) _temp122)->f1; goto
_LL171;} else{ goto _LL172;} _LL172: if((( struct _tunion_struct*) _temp122)->tag
== Cyc_Absyn_Deref_e_tag){ _LL327: _temp326=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Deref_e_struct*) _temp122)->f1; goto _LL173;} else{ goto _LL174;}
_LL174: if((( struct _tunion_struct*) _temp122)->tag == Cyc_Absyn_StructArrow_e_tag){
_LL331: _temp330=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp122)->f1; goto _LL329; _LL329: _temp328=( struct _tagged_string*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp122)->f2; goto _LL175;} else{ goto _LL176;}
_LL176: if((( struct _tunion_struct*) _temp122)->tag == Cyc_Absyn_StructMember_e_tag){
_LL335: _temp334=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp122)->f1; goto _LL333; _LL333: _temp332=( struct _tagged_string*)(( struct
Cyc_Absyn_StructMember_e_struct*) _temp122)->f2; goto _LL177;} else{ goto _LL178;}
_LL178: if((( struct _tunion_struct*) _temp122)->tag == Cyc_Absyn_Tuple_e_tag){
_LL337: _temp336=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp122)->f1; goto _LL179;} else{ goto _LL180;} _LL180: if((( struct
_tunion_struct*) _temp122)->tag == Cyc_Absyn_Struct_e_tag){ _LL345: _temp344=(
struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*) _temp122)->f1; goto _LL343;
_LL343: _temp342=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp122)->f2; goto _LL341; _LL341: _temp340=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp122)->f3; goto _LL339; _LL339: _temp338=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*) _temp122)->f4;
goto _LL181;} else{ goto _LL182;} _LL182: if((( struct _tunion_struct*) _temp122)->tag
== Cyc_Absyn_Array_e_tag){ _LL347: _temp346=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Array_e_struct*) _temp122)->f1; goto _LL183;} else{ goto _LL184;}
_LL184: if((( struct _tunion_struct*) _temp122)->tag == Cyc_Absyn_Increment_e_tag){
_LL351: _temp350=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp122)->f1; goto _LL349; _LL349: _temp348=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp122)->f2; goto _LL185;} else{ goto _LL186;} _LL186: if((( struct
_tunion_struct*) _temp122)->tag == Cyc_Absyn_Throw_e_tag){ _LL353: _temp352=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*) _temp122)->f1; goto
_LL187;} else{ goto _LL188;} _LL188: if((( struct _tunion_struct*) _temp122)->tag
== Cyc_Absyn_Conditional_e_tag){ _LL359: _temp358=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp122)->f1; goto _LL357; _LL357:
_temp356=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp122)->f2; goto _LL355; _LL355: _temp354=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Conditional_e_struct*) _temp122)->f3; goto _LL189;} else{ goto _LL190;}
_LL190: if((( struct _tunion_struct*) _temp122)->tag == Cyc_Absyn_StmtExp_e_tag){
_LL361: _temp360=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp122)->f1; goto _LL191;} else{ goto _LL192;} _LL192: if((( struct
_tunion_struct*) _temp122)->tag == Cyc_Absyn_SeqExp_e_tag){ _LL365: _temp364=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*) _temp122)->f1; goto
_LL363; _LL363: _temp362=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp122)->f2; goto _LL193;} else{ goto _LL194;} _LL194: if((( struct
_tunion_struct*) _temp122)->tag == Cyc_Absyn_Comprehension_e_tag){ _LL371:
_temp370=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp122)->f1; goto _LL369; _LL369: _temp368=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Comprehension_e_struct*) _temp122)->f2; goto _LL367; _LL367: _temp366=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*) _temp122)->f3;
goto _LL195;} else{ goto _LL196;} _LL196: if((( struct _tunion_struct*) _temp122)->tag
== Cyc_Absyn_Var_e_tag){ _LL375: _temp374=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*)
_temp122)->f1; goto _LL373; _LL373: _temp372=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp122)->f2; if(( int) _temp372 == Cyc_Absyn_Unresolved_b){ goto _LL197;}
else{ goto _LL198;}} else{ goto _LL198;} _LL198: if((( struct _tunion_struct*)
_temp122)->tag == Cyc_Absyn_UnknownId_e_tag){ _LL377: _temp376=( struct _tuple1*)((
struct Cyc_Absyn_UnknownId_e_struct*) _temp122)->f1; goto _LL199;} else{ goto
_LL200;} _LL200: if((( struct _tunion_struct*) _temp122)->tag == Cyc_Absyn_UnknownCall_e_tag){
_LL381: _temp380=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp122)->f1; goto _LL379; _LL379: _temp378=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnknownCall_e_struct*) _temp122)->f2; goto _LL201;} else{ goto _LL202;}
_LL202: if((( struct _tunion_struct*) _temp122)->tag == Cyc_Absyn_UnresolvedMem_e_tag){
_LL385: _temp384=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp122)->f1; goto _LL383; _LL383: _temp382=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnresolvedMem_e_struct*) _temp122)->f2; goto _LL203;} else{ goto
_LL204;} _LL204: if((( struct _tunion_struct*) _temp122)->tag == Cyc_Absyn_CompoundLit_e_tag){
_LL389: _temp388=( struct _tuple2*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp122)->f1; goto _LL387; _LL387: _temp386=( struct Cyc_List_List*)(( struct
Cyc_Absyn_CompoundLit_e_struct*) _temp122)->f2; goto _LL205;} else{ goto _LL206;}
_LL206: if((( struct _tunion_struct*) _temp122)->tag == Cyc_Absyn_Codegen_e_tag){
_LL391: _temp390=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*)
_temp122)->f1; goto _LL207;} else{ goto _LL208;} _LL208: if((( struct
_tunion_struct*) _temp122)->tag == Cyc_Absyn_Fill_e_tag){ _LL393: _temp392=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*) _temp122)->f1; goto
_LL209;} else{ goto _LL123;} _LL125: _temp212= _temp210; goto _LL127; _LL127:
_temp216= _temp212; goto _LL129; _LL129: return Cyc_NewControlFlow_abstract_exp(
env, _temp216); _LL131: env.access_path == 0? 0:(( int(*)( struct _tagged_string
msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({ char* _temp394=( char*)"new_control_flow.cyc:263 env.access_path==null";
struct _tagged_string _temp395; _temp395.curr= _temp394; _temp395.base= _temp394;
_temp395.last_plus_one= _temp394 + 47; _temp395;})); return({ struct Cyc_NewControlFlow_AbsSyn
_temp396; _temp396.when_true=({ struct Cyc_NewControlFlow_AbsSynOne _temp398;
_temp398.inner_exp=( void*) Cyc_CfAbsexp_mkBottomAE(); _temp398.assigns=( void*)
Cyc_CfAbsexp_mkBottomAE(); _temp398;}); _temp396.when_false=({ struct Cyc_NewControlFlow_AbsSynOne
_temp397; _temp397.inner_exp=( void*) Cyc_CfAbsexp_mkSkipAE(); _temp397.assigns=(
void*) Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp());
_temp397;}); _temp396.lvalues= 0; _temp396;}); _LL133: env.access_path == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp399=( char*)"new_control_flow.cyc:268 env.access_path==null";
struct _tagged_string _temp400; _temp400.curr= _temp399; _temp400.base= _temp399;
_temp400.last_plus_one= _temp399 + 47; _temp400;})); return({ struct Cyc_NewControlFlow_AbsSyn
_temp401; _temp401.when_true=({ struct Cyc_NewControlFlow_AbsSynOne _temp403;
_temp403.inner_exp=( void*) Cyc_CfAbsexp_mkSkipAE(); _temp403.assigns=( void*)
Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp()); _temp403;});
_temp401.when_false=({ struct Cyc_NewControlFlow_AbsSynOne _temp402; _temp402.inner_exp=(
void*) Cyc_CfAbsexp_mkBottomAE(); _temp402.assigns=( void*) Cyc_CfAbsexp_mkBottomAE();
_temp402;}); _temp401.lvalues= 0; _temp401;}); _LL135: goto _LL137; _LL137: goto
_LL139; _LL139: goto _LL141; _LL141: return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(),
0, Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp())); _LL143:
return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(), Cyc_NewControlFlow_use_it_list(),
Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp())); _LL145:
_temp260= _temp254; goto _LL147; _LL147: _temp266= _temp260; goto _LL149; _LL149: {
void* absop= Cyc_NewControlFlow_destruct_path( env, Cyc_CfAbsexp_mkLocalOp(
_temp266)); return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(),({
struct Cyc_List_List* _temp404=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp404->hd=( void*) absop; _temp404->tl= 0; _temp404;}),
Cyc_NewControlFlow_make_assigns( env, absop));} _LL151: _temp284= _temp274;
_temp280= _temp270; goto _LL153; _LL153: env.access_path == 0? 0:(( int(*)(
struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({
char* _temp405=( char*)"new_control_flow.cyc:299 env.access_path == null";
struct _tagged_string _temp406; _temp406.curr= _temp405; _temp406.base= _temp405;
_temp406.last_plus_one= _temp405 + 49; _temp406;})); if( _temp284 == 0){ return
Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(), 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));} Cyc_NewControlFlow_add_tunion_root( env, e,
_temp280);{ void* es_exp= Cyc_CfAbsexp_mkSkipAE(); void* useexp= Cyc_CfAbsexp_mkSkipAE();{
int i= 0; for( 0; _temp284 != 0; _temp284=({ struct Cyc_List_List* _temp407=
_temp284; if( _temp407 == 0){ _throw( Null_Exception);} _temp407->tl;}), ++ i){
struct Cyc_NewControlFlow_AbsEnv elt_env= Cyc_NewControlFlow_change_lhs( env,({
struct Cyc_List_List* _temp408=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp408->hd=( void*) Cyc_CfAbsexp_mkMemberOp( Cyc_CfAbsexp_mkMallocOp(
e),( void*)({ struct Cyc_CfFlowInfo_TupleF_struct* _temp409=( struct Cyc_CfFlowInfo_TupleF_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_TupleF_struct)); _temp409[ 0]=({ struct
Cyc_CfFlowInfo_TupleF_struct _temp410; _temp410.tag= Cyc_CfFlowInfo_TupleF_tag;
_temp410.f1= i; _temp410;}); _temp409;})); _temp408->tl= 0; _temp408;})); struct
Cyc_NewControlFlow_AbsSyn syn= Cyc_NewControlFlow_abstract_exp( elt_env,( struct
Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp411= _temp284; if( _temp411 == 0){
_throw( Null_Exception);} _temp411->hd;})); void* _temp414; void* _temp416;
struct Cyc_NewControlFlow_AbsSynOne _temp412= Cyc_NewControlFlow_meet_absexp(
syn); _LL417: _temp416=( void*) _temp412.inner_exp; goto _LL415; _LL415:
_temp414=( void*) _temp412.assigns; goto _LL413; _LL413: es_exp= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, _temp416, es_exp); useexp= Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_mkAnyOrderG(), _temp414, useexp);}} return Cyc_NewControlFlow_malloc_and_init(
env, e, es_exp, useexp);} _LL155: Cyc_NewControlFlow_add_malloc_root( env, e,
_temp288); return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkMallocAE( e), 0,
Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkAddressOp( Cyc_CfAbsexp_mkMallocOp(
e)))); _LL157: { struct Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp(
env, _temp296); struct Cyc_List_List* lhs= syn1.lvalues == 0? Cyc_NewControlFlow_use_it_list():
syn1.lvalues; struct Cyc_NewControlFlow_AbsSyn syn2= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_change_lhs( env, lhs), _temp292); void* _temp420; void*
_temp422; struct Cyc_NewControlFlow_AbsSynOne _temp418= Cyc_NewControlFlow_meet_absexp(
syn1); _LL423: _temp422=( void*) _temp418.inner_exp; goto _LL421; _LL421:
_temp420=( void*) _temp418.assigns; goto _LL419; _LL419: { void* _temp426; void*
_temp428; struct Cyc_NewControlFlow_AbsSynOne _temp424= Cyc_NewControlFlow_meet_absexp(
syn2); _LL429: _temp428=( void*) _temp424.inner_exp; goto _LL427; _LL427:
_temp426=( void*) _temp424.assigns; goto _LL425; _LL425: { void* ansexp= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG,
_temp422, _temp428), _temp426); return Cyc_NewControlFlow_mkSyn_tf( ansexp, 0,
_temp420);}}} _LL159: return Cyc_NewControlFlow_use_exp_unordered( env,({ struct
Cyc_List_List* _temp430=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp430->hd=( void*) _temp302; _temp430->tl=({ struct Cyc_List_List* _temp431=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp431->hd=(
void*) _temp298; _temp431->tl= 0; _temp431;}); _temp430;}), 0); _LL161: return
Cyc_NewControlFlow_use_exp_unordered( env, _temp304, 0); _LL163: { int both_leaf=
1;{ void* _temp433=( void*)({ struct Cyc_Core_Opt* _temp432= _temp308->topt; if(
_temp432 == 0){ _throw( Null_Exception);} _temp432->v;}); struct Cyc_Absyn_Structdecl**
_temp441; struct Cyc_List_List* _temp443; struct _tuple1* _temp445; struct Cyc_List_List*
_temp447; _LL435: if(( unsigned int) _temp433 > 4u?(( struct _tunion_struct*)
_temp433)->tag == Cyc_Absyn_StructType_tag: 0){ _LL446: _temp445=( struct
_tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp433)->f1; goto _LL444;
_LL444: _temp443=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp433)->f2; goto _LL442; _LL442: _temp441=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp433)->f3; goto _LL436;} else{ goto
_LL437;} _LL437: if(( unsigned int) _temp433 > 4u?(( struct _tunion_struct*)
_temp433)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL448: _temp447=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp433)->f1; goto _LL438;} else{ goto
_LL439;} _LL439: goto _LL440; _LL436: goto _LL438; _LL438: both_leaf= 0; goto
_LL434; _LL440: goto _LL434; _LL434:;}{ void* _temp450=( void*)({ struct Cyc_Core_Opt*
_temp449= e->topt; if( _temp449 == 0){ _throw( Null_Exception);} _temp449->v;});
struct Cyc_Absyn_Structdecl** _temp458; struct Cyc_List_List* _temp460; struct
_tuple1* _temp462; struct Cyc_List_List* _temp464; _LL452: if(( unsigned int)
_temp450 > 4u?(( struct _tunion_struct*) _temp450)->tag == Cyc_Absyn_StructType_tag:
0){ _LL463: _temp462=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp450)->f1; goto _LL461; _LL461: _temp460=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp450)->f2; goto _LL459; _LL459: _temp458=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp450)->f3;
goto _LL453;} else{ goto _LL454;} _LL454: if(( unsigned int) _temp450 > 4u?((
struct _tunion_struct*) _temp450)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL465:
_temp464=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp450)->f1;
goto _LL455;} else{ goto _LL456;} _LL456: goto _LL457; _LL453: goto _LL455;
_LL455: both_leaf= 0; goto _LL451; _LL457: goto _LL451; _LL451:;} if( both_leaf){
return Cyc_NewControlFlow_abstract_exp( env, _temp308);} return Cyc_NewControlFlow_use_exp_unordered(
env,({ struct Cyc_List_List* _temp466=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp466->hd=( void*) _temp308; _temp466->tl= 0;
_temp466;}), env.lhsides);} _LL165: return Cyc_NewControlFlow_use_exp_unordered(
env,({ struct Cyc_List_List* _temp467=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp467->hd=( void*) _temp314; _temp467->tl=
_temp312; _temp467;}), 0); _LL167: { void* _temp469= Cyc_Tcutil_compress(( void*)({
struct Cyc_Core_Opt* _temp468= _temp318->topt; if( _temp468 == 0){ _throw(
Null_Exception);} _temp468->v;})); struct Cyc_List_List* _temp475; _LL471: if((
unsigned int) _temp469 > 4u?(( struct _tunion_struct*) _temp469)->tag == Cyc_Absyn_TupleType_tag:
0){ _LL476: _temp475=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp469)->f1; goto _LL472;} else{ goto _LL473;} _LL473: goto _LL474; _LL472:
return Cyc_NewControlFlow_descend_path( env, e, _temp318); _LL474: return Cyc_NewControlFlow_use_exp_unordered(
env,({ struct Cyc_List_List* _temp477=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp477->hd=( void*) _temp318; _temp477->tl=({
struct Cyc_List_List* _temp478=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp478->hd=( void*) _temp316; _temp478->tl= 0;
_temp478;}); _temp477;}), Cyc_NewControlFlow_use_it_list()); _LL470:;} _LL169: {
void* _temp479=( void*) _temp320->r; struct Cyc_Absyn_Structdecl* _temp487;
struct Cyc_List_List* _temp489; struct Cyc_Core_Opt* _temp491; struct _tuple1*
_temp493; struct Cyc_List_List* _temp495; _LL481: if((( struct _tunion_struct*)
_temp479)->tag == Cyc_Absyn_Struct_e_tag){ _LL494: _temp493=( struct _tuple1*)((
struct Cyc_Absyn_Struct_e_struct*) _temp479)->f1; goto _LL492; _LL492: _temp491=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*) _temp479)->f2; goto
_LL490; _LL490: _temp489=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp479)->f3; goto _LL488; _LL488: _temp487=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp479)->f4; goto _LL482;} else{ goto
_LL483;} _LL483: if((( struct _tunion_struct*) _temp479)->tag == Cyc_Absyn_Tuple_e_tag){
_LL496: _temp495=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp479)->f1; goto _LL484;} else{ goto _LL485;} _LL485: goto _LL486; _LL482:
goto _LL484; _LL484: { void* malloc_op= Cyc_CfAbsexp_mkMallocOp( e); Cyc_NewControlFlow_add_malloc_root(
env, e,( void*)({ struct Cyc_Core_Opt* _temp497= _temp320->topt; if( _temp497 ==
0){ _throw( Null_Exception);} _temp497->v;}));{ struct Cyc_NewControlFlow_AbsEnv
inner_env= Cyc_NewControlFlow_change_lhs( env,({ struct Cyc_List_List* _temp498=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp498->hd=(
void*) malloc_op; _temp498->tl= 0; _temp498;})); inner_env.access_path= 0;{ void*
_temp501; void* _temp503; struct Cyc_NewControlFlow_AbsSynOne _temp499= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( inner_env, _temp320)); _LL504: _temp503=( void*)
_temp499.inner_exp; goto _LL502; _LL502: _temp501=( void*) _temp499.assigns;
goto _LL500; _LL500: { struct Cyc_NewControlFlow_AbsSyn ans= Cyc_NewControlFlow_malloc_and_init(
env, e, _temp503, _temp501); if( env.access_path != 0){ void* _temp506=( void*)((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp505= env.access_path; if(
_temp505 == 0){ _throw( Null_Exception);} _temp505->hd;}))->r; struct Cyc_Absyn_Exp*
_temp516; struct Cyc_Absyn_Exp* _temp518; struct Cyc_Absyn_Exp* _temp520; struct
_tagged_string* _temp522; struct Cyc_Absyn_Exp* _temp524; _LL508: if((( struct
_tunion_struct*) _temp506)->tag == Cyc_Absyn_Subscript_e_tag){ _LL519: _temp518=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*) _temp506)->f1;
goto _LL517; _LL517: _temp516=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp506)->f2; goto _LL509;} else{ goto _LL510;} _LL510: if((( struct
_tunion_struct*) _temp506)->tag == Cyc_Absyn_Deref_e_tag){ _LL521: _temp520=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*) _temp506)->f1; goto
_LL511;} else{ goto _LL512;} _LL512: if((( struct _tunion_struct*) _temp506)->tag
== Cyc_Absyn_StructArrow_e_tag){ _LL525: _temp524=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp506)->f1; goto _LL523; _LL523:
_temp522=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp506)->f2; goto _LL513;} else{ goto _LL514;} _LL514: goto _LL515; _LL509:
Cyc_Evexp_eval_const_uint_exp( _temp516) == 0? 0:(( int(*)( struct
_tagged_string msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({ char*
_temp526=( char*)"new_control_flow.cyc:412 Evexp::eval_const_uint_exp(e2) == 0";
struct _tagged_string _temp527; _temp527.curr= _temp526; _temp527.base= _temp526;
_temp527.last_plus_one= _temp526 + 61; _temp527;})); goto _LL511; _LL511: env.access_path=({
struct Cyc_List_List* _temp528= env.access_path; if( _temp528 == 0){ _throw(
Null_Exception);} _temp528->tl;}); ans.lvalues=({ struct Cyc_List_List* _temp529=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp529->hd=(
void*) Cyc_NewControlFlow_destruct_path( env, malloc_op); _temp529->tl= 0;
_temp529;}); goto _LL507; _LL513: env.access_path=({ struct Cyc_List_List*
_temp530=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp530->hd=( void*) Cyc_Absyn_structmember_exp( _temp524, _temp522, 0);
_temp530->tl=({ struct Cyc_List_List* _temp531= env.access_path; if( _temp531 ==
0){ _throw( Null_Exception);} _temp531->tl;}); _temp530;}); ans.lvalues=({
struct Cyc_List_List* _temp532=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp532->hd=( void*) Cyc_NewControlFlow_destruct_path(
env, malloc_op); _temp532->tl= 0; _temp532;}); goto _LL507; _LL515:( void)
_throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct* _temp533=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp533[ 0]=({ struct Cyc_Core_Impossible_struct _temp534; _temp534.tag= Cyc_Core_Impossible_tag;
_temp534.f1=( struct _tagged_string)({ char* _temp535=( char*)"bad malloc access path";
struct _tagged_string _temp536; _temp536.curr= _temp535; _temp536.base= _temp535;
_temp536.last_plus_one= _temp535 + 23; _temp536;}); _temp534;}); _temp533;}));
_LL507:;} return ans;}}}} _LL486: if( _temp322 == 0){ return Cyc_NewControlFlow_use_exp_unordered(
env,({ struct Cyc_List_List* _temp537=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp537->hd=( void*) _temp320; _temp537->tl= 0;
_temp537;}), 0);} else{ return Cyc_NewControlFlow_use_exp_unordered( env,({
struct Cyc_List_List* _temp538=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp538->hd=( void*)(( struct Cyc_Absyn_Exp*)({ struct
Cyc_Absyn_Exp* _temp540= _temp322; if( _temp540 == 0){ _throw( Null_Exception);}
_temp540;})); _temp538->tl=({ struct Cyc_List_List* _temp539=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp539->hd=( void*) _temp320;
_temp539->tl= 0; _temp539;}); _temp538;}), 0);} _LL480:;} _LL171:{ void*
_temp541=( void*) _temp324->r; struct Cyc_Absyn_Structdecl* _temp549; struct Cyc_List_List*
_temp551; struct Cyc_Core_Opt* _temp553; struct _tuple1* _temp555; struct Cyc_List_List*
_temp557; _LL543: if((( struct _tunion_struct*) _temp541)->tag == Cyc_Absyn_Struct_e_tag){
_LL556: _temp555=( struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp541)->f1; goto _LL554; _LL554: _temp553=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp541)->f2; goto _LL552; _LL552: _temp551=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*) _temp541)->f3; goto
_LL550; _LL550: _temp549=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp541)->f4; goto _LL544;} else{ goto _LL545;} _LL545: if((( struct
_tunion_struct*) _temp541)->tag == Cyc_Absyn_Tuple_e_tag){ _LL558: _temp557=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*) _temp541)->f1; goto
_LL546;} else{ goto _LL547;} _LL547: goto _LL548; _LL544: goto _LL546; _LL546: {
void* malloc_op= Cyc_CfAbsexp_mkMallocOp( e); Cyc_NewControlFlow_add_malloc_root(
env, e,( void*)({ struct Cyc_Core_Opt* _temp559= _temp324->topt; if( _temp559 ==
0){ _throw( Null_Exception);} _temp559->v;}));{ struct Cyc_NewControlFlow_AbsEnv
inner_env= Cyc_NewControlFlow_change_lhs( env,({ struct Cyc_List_List* _temp560=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp560->hd=(
void*) malloc_op; _temp560->tl= 0; _temp560;})); inner_env.access_path= 0;{ void*
_temp563; void* _temp565; struct Cyc_NewControlFlow_AbsSynOne _temp561= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( inner_env, _temp324)); _LL566: _temp565=( void*)
_temp561.inner_exp; goto _LL564; _LL564: _temp563=( void*) _temp561.assigns;
goto _LL562; _LL562: { struct Cyc_NewControlFlow_AbsSyn ans= Cyc_NewControlFlow_malloc_and_init(
env, e, _temp565, _temp563); if( env.access_path != 0){ void* _temp568=( void*)((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp567= env.access_path; if(
_temp567 == 0){ _throw( Null_Exception);} _temp567->hd;}))->r; struct Cyc_Absyn_Exp*
_temp578; struct Cyc_Absyn_Exp* _temp580; struct Cyc_Absyn_Exp* _temp582; struct
_tagged_string* _temp584; struct Cyc_Absyn_Exp* _temp586; _LL570: if((( struct
_tunion_struct*) _temp568)->tag == Cyc_Absyn_Subscript_e_tag){ _LL581: _temp580=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*) _temp568)->f1;
goto _LL579; _LL579: _temp578=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp568)->f2; goto _LL571;} else{ goto _LL572;} _LL572: if((( struct
_tunion_struct*) _temp568)->tag == Cyc_Absyn_Deref_e_tag){ _LL583: _temp582=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*) _temp568)->f1; goto
_LL573;} else{ goto _LL574;} _LL574: if((( struct _tunion_struct*) _temp568)->tag
== Cyc_Absyn_StructArrow_e_tag){ _LL587: _temp586=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp568)->f1; goto _LL585; _LL585:
_temp584=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp568)->f2; goto _LL575;} else{ goto _LL576;} _LL576: goto _LL577; _LL571:
Cyc_Evexp_eval_const_uint_exp( _temp578) == 0? 0:(( int(*)( struct
_tagged_string msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({ char*
_temp588=( char*)"new_control_flow.cyc:453 Evexp::eval_const_uint_exp(e2) == 0";
struct _tagged_string _temp589; _temp589.curr= _temp588; _temp589.base= _temp588;
_temp589.last_plus_one= _temp588 + 61; _temp589;})); goto _LL573; _LL573: env.access_path=({
struct Cyc_List_List* _temp590= env.access_path; if( _temp590 == 0){ _throw(
Null_Exception);} _temp590->tl;}); ans.lvalues=({ struct Cyc_List_List* _temp591=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp591->hd=(
void*) Cyc_NewControlFlow_destruct_path( env, malloc_op); _temp591->tl= 0;
_temp591;}); goto _LL569; _LL575: env.access_path=({ struct Cyc_List_List*
_temp592=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp592->hd=( void*) Cyc_Absyn_structmember_exp( _temp586, _temp584, 0);
_temp592->tl=({ struct Cyc_List_List* _temp593= env.access_path; if( _temp593 ==
0){ _throw( Null_Exception);} _temp593->tl;}); _temp592;}); ans.lvalues=({
struct Cyc_List_List* _temp594=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp594->hd=( void*) Cyc_NewControlFlow_destruct_path(
env, malloc_op); _temp594->tl= 0; _temp594;}); goto _LL569; _LL577:( void)
_throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct* _temp595=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp595[ 0]=({ struct Cyc_Core_Impossible_struct _temp596; _temp596.tag= Cyc_Core_Impossible_tag;
_temp596.f1=( struct _tagged_string)({ char* _temp597=( char*)"bad malloc access path";
struct _tagged_string _temp598; _temp598.curr= _temp597; _temp598.base= _temp597;
_temp598.last_plus_one= _temp597 + 23; _temp598;}); _temp596;}); _temp595;}));
_LL569:;} return ans;}}}} _LL548: goto _LL542; _LL542:;}{ struct Cyc_List_List*
_temp599= env.access_path; struct Cyc_List_List _temp611; struct Cyc_List_List*
_temp612; struct Cyc_Absyn_Exp* _temp614; struct Cyc_Absyn_Exp _temp616; struct
Cyc_Position_Segment* _temp617; void* _temp619; struct Cyc_Absyn_Exp* _temp621;
struct Cyc_Absyn_Exp* _temp623; struct Cyc_Core_Opt* _temp625; struct Cyc_List_List
_temp627; struct Cyc_List_List* _temp628; struct Cyc_Absyn_Exp* _temp630; struct
Cyc_Absyn_Exp _temp632; struct Cyc_Position_Segment* _temp633; void* _temp635;
struct Cyc_Absyn_Exp* _temp637; struct Cyc_Core_Opt* _temp639; struct Cyc_List_List
_temp641; struct Cyc_List_List* _temp642; struct Cyc_Absyn_Exp* _temp644; struct
Cyc_Absyn_Exp _temp646; struct Cyc_Position_Segment* _temp647; void* _temp649;
struct _tagged_string* _temp651; struct Cyc_Absyn_Exp* _temp653; struct Cyc_Core_Opt*
_temp655; _LL601: if( _temp599 == 0){ goto _LL602;} else{ goto _LL603;} _LL603:
if( _temp599 == 0){ goto _LL605;} else{ _temp611=* _temp599; _LL615: _temp614=(
struct Cyc_Absyn_Exp*) _temp611.hd; _temp616=* _temp614; _LL626: _temp625=(
struct Cyc_Core_Opt*) _temp616.topt; goto _LL620; _LL620: _temp619=( void*)
_temp616.r; if((( struct _tunion_struct*) _temp619)->tag == Cyc_Absyn_Subscript_e_tag){
_LL624: _temp623=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp619)->f1; goto _LL622; _LL622: _temp621=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Subscript_e_struct*) _temp619)->f2; goto _LL618;} else{ goto _LL605;}
_LL618: _temp617=( struct Cyc_Position_Segment*) _temp616.loc; goto _LL613;
_LL613: _temp612=( struct Cyc_List_List*) _temp611.tl; goto _LL604;} _LL605: if(
_temp599 == 0){ goto _LL607;} else{ _temp627=* _temp599; _LL631: _temp630=(
struct Cyc_Absyn_Exp*) _temp627.hd; _temp632=* _temp630; _LL640: _temp639=(
struct Cyc_Core_Opt*) _temp632.topt; goto _LL636; _LL636: _temp635=( void*)
_temp632.r; if((( struct _tunion_struct*) _temp635)->tag == Cyc_Absyn_Deref_e_tag){
_LL638: _temp637=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp635)->f1; goto _LL634;} else{ goto _LL607;} _LL634: _temp633=( struct Cyc_Position_Segment*)
_temp632.loc; goto _LL629; _LL629: _temp628=( struct Cyc_List_List*) _temp627.tl;
goto _LL606;} _LL607: if( _temp599 == 0){ goto _LL609;} else{ _temp641=*
_temp599; _LL645: _temp644=( struct Cyc_Absyn_Exp*) _temp641.hd; _temp646=*
_temp644; _LL656: _temp655=( struct Cyc_Core_Opt*) _temp646.topt; goto _LL650;
_LL650: _temp649=( void*) _temp646.r; if((( struct _tunion_struct*) _temp649)->tag
== Cyc_Absyn_StructArrow_e_tag){ _LL654: _temp653=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp649)->f1; goto _LL652; _LL652:
_temp651=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp649)->f2; goto _LL648;} else{ goto _LL609;} _LL648: _temp647=( struct Cyc_Position_Segment*)
_temp646.loc; goto _LL643; _LL643: _temp642=( struct Cyc_List_List*) _temp641.tl;
goto _LL608;} _LL609: goto _LL610; _LL602: env.access_path=({ struct Cyc_List_List*
_temp657=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp657->hd=( void*) e; _temp657->tl= env.access_path; _temp657;}); goto _LL600;
_LL604: Cyc_Evexp_eval_const_uint_exp( _temp621) == 0? 0:(( int(*)( struct
_tagged_string msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({ char*
_temp658=( char*)"new_control_flow.cyc:481 Evexp::eval_const_uint_exp(e2) == 0";
struct _tagged_string _temp659; _temp659.curr= _temp658; _temp659.base= _temp658;
_temp659.last_plus_one= _temp658 + 61; _temp659;})); _temp628= _temp612; goto
_LL606; _LL606: env.access_path= _temp628; goto _LL600; _LL608: env.access_path=({
struct Cyc_List_List* _temp660=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp660->hd=( void*) Cyc_Absyn_structmember_exp(
_temp653, _temp651, 0); _temp660->tl= _temp642; _temp660;}); goto _LL600; _LL610:(
void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp661=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp661[ 0]=({ struct Cyc_Core_Impossible_struct _temp662; _temp662.tag= Cyc_Core_Impossible_tag;
_temp662.f1=( struct _tagged_string)({ char* _temp663=( char*)"abstract_exp: bad access path for &";
struct _tagged_string _temp664; _temp664.curr= _temp663; _temp664.base= _temp663;
_temp664.last_plus_one= _temp663 + 36; _temp664;}); _temp662;}); _temp661;}));
_LL600:;} return Cyc_NewControlFlow_abstract_exp( env, _temp324); _LL173: {
struct Cyc_List_List* _temp665= env.access_path; struct Cyc_List_List _temp671;
struct Cyc_List_List* _temp672; struct Cyc_Absyn_Exp* _temp674; struct Cyc_Absyn_Exp
_temp676; struct Cyc_Position_Segment* _temp677; void* _temp679; struct Cyc_Absyn_Exp*
_temp681; struct Cyc_Core_Opt* _temp683; _LL667: if( _temp665 == 0){ goto _LL669;}
else{ _temp671=* _temp665; _LL675: _temp674=( struct Cyc_Absyn_Exp*) _temp671.hd;
_temp676=* _temp674; _LL684: _temp683=( struct Cyc_Core_Opt*) _temp676.topt;
goto _LL680; _LL680: _temp679=( void*) _temp676.r; if((( struct _tunion_struct*)
_temp679)->tag == Cyc_Absyn_Address_e_tag){ _LL682: _temp681=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Address_e_struct*) _temp679)->f1; goto _LL678;} else{ goto
_LL669;} _LL678: _temp677=( struct Cyc_Position_Segment*) _temp676.loc; goto
_LL673; _LL673: _temp672=( struct Cyc_List_List*) _temp671.tl; goto _LL668;}
_LL669: goto _LL670; _LL668: env.access_path= _temp672;{ struct Cyc_NewControlFlow_AbsSyn
syn1= Cyc_NewControlFlow_abstract_exp( env, _temp326); if( syn1.lvalues == 0){
syn1.lvalues= Cyc_NewControlFlow_use_it_list();} return syn1;} _LL670: return
Cyc_NewControlFlow_descend_path( env, e, _temp326); _LL666:;} _LL175: { void*
_temp686= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp685= _temp330->topt;
if( _temp685 == 0){ _throw( Null_Exception);} _temp685->v;})); struct Cyc_Absyn_PtrInfo
_temp692; struct Cyc_Absyn_Conref* _temp694; struct Cyc_Absyn_Tqual* _temp696;
struct Cyc_Absyn_Conref* _temp698; void* _temp700; void* _temp702; _LL688: if((
unsigned int) _temp686 > 4u?(( struct _tunion_struct*) _temp686)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL693: _temp692=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp686)->f1; _LL703: _temp702=( void*) _temp692.elt_typ; goto _LL701; _LL701:
_temp700=( void*) _temp692.rgn_typ; goto _LL699; _LL699: _temp698=( struct Cyc_Absyn_Conref*)
_temp692.nullable; goto _LL697; _LL697: _temp696=( struct Cyc_Absyn_Tqual*)
_temp692.tq; goto _LL695; _LL695: _temp694=( struct Cyc_Absyn_Conref*) _temp692.bounds;
goto _LL689;} else{ goto _LL690;} _LL690: goto _LL691; _LL689:{ void* _temp704=
Cyc_Tcutil_compress( _temp702); struct Cyc_Absyn_Structdecl** _temp712; struct
Cyc_List_List* _temp714; struct _tuple1* _temp716; struct Cyc_Absyn_Uniondecl**
_temp718; struct Cyc_List_List* _temp720; struct _tuple1* _temp722; _LL706: if((
unsigned int) _temp704 > 4u?(( struct _tunion_struct*) _temp704)->tag == Cyc_Absyn_StructType_tag:
0){ _LL717: _temp716=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp704)->f1; goto _LL715; _LL715: _temp714=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp704)->f2; goto _LL713; _LL713: _temp712=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp704)->f3;
goto _LL707;} else{ goto _LL708;} _LL708: if(( unsigned int) _temp704 > 4u?((
struct _tunion_struct*) _temp704)->tag == Cyc_Absyn_UnionType_tag: 0){ _LL723:
_temp722=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*) _temp704)->f1;
goto _LL721; _LL721: _temp720=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp704)->f2; goto _LL719; _LL719: _temp718=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp704)->f3; goto _LL709;} else{ goto
_LL710;} _LL710: goto _LL711; _LL707: return Cyc_NewControlFlow_descend_path(
env, e, _temp330); _LL709: { struct Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp(
env, _temp330); syn1.lvalues= Cyc_NewControlFlow_use_it_list(); return syn1;}
_LL711: goto _LL705; _LL705:;} goto _LL691; _LL691:( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Core_Impossible_struct* _temp724=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp724[ 0]=({ struct
Cyc_Core_Impossible_struct _temp725; _temp725.tag= Cyc_Core_Impossible_tag;
_temp725.f1=( struct _tagged_string)({ char* _temp726=( char*)"NewControlFlow: bad type in StructArrow";
struct _tagged_string _temp727; _temp727.curr= _temp726; _temp727.base= _temp726;
_temp727.last_plus_one= _temp726 + 40; _temp727;}); _temp725;}); _temp724;}));
_LL687:;} _LL177: { void* _temp729= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp728= _temp334->topt; if( _temp728 == 0){ _throw( Null_Exception);} _temp728->v;}));
struct Cyc_Absyn_Structdecl** _temp737; struct Cyc_List_List* _temp739; struct
_tuple1* _temp741; struct Cyc_Absyn_Uniondecl** _temp743; struct Cyc_List_List*
_temp745; struct _tuple1* _temp747; _LL731: if(( unsigned int) _temp729 > 4u?((
struct _tunion_struct*) _temp729)->tag == Cyc_Absyn_StructType_tag: 0){ _LL742:
_temp741=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp729)->f1;
goto _LL740; _LL740: _temp739=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp729)->f2; goto _LL738; _LL738: _temp737=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp729)->f3; goto _LL732;} else{ goto
_LL733;} _LL733: if(( unsigned int) _temp729 > 4u?(( struct _tunion_struct*)
_temp729)->tag == Cyc_Absyn_UnionType_tag: 0){ _LL748: _temp747=( struct _tuple1*)((
struct Cyc_Absyn_UnionType_struct*) _temp729)->f1; goto _LL746; _LL746: _temp745=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*) _temp729)->f2; goto
_LL744; _LL744: _temp743=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp729)->f3; goto _LL734;} else{ goto _LL735;} _LL735: goto _LL736; _LL732:
return Cyc_NewControlFlow_descend_path( env, e, _temp334); _LL734: { struct Cyc_NewControlFlow_AbsSyn
syn1= Cyc_NewControlFlow_abstract_exp( env, _temp334); syn1.lvalues= Cyc_NewControlFlow_use_it_list();
return syn1;} _LL736:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp749=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp749[ 0]=({ struct Cyc_Core_Impossible_struct _temp750; _temp750.tag= Cyc_Core_Impossible_tag;
_temp750.f1=( struct _tagged_string)({ char* _temp751=( char*)"NewControlFlow: bad type in StructMember";
struct _tagged_string _temp752; _temp752.curr= _temp751; _temp752.base= _temp751;
_temp752.last_plus_one= _temp751 + 41; _temp752;}); _temp750;}); _temp749;}));
_LL730:;} _LL179: { struct Cyc_List_List* _temp753= env.access_path; struct Cyc_List_List
_temp761; struct Cyc_List_List* _temp762; struct Cyc_Absyn_Exp* _temp764; struct
Cyc_Absyn_Exp _temp766; struct Cyc_Position_Segment* _temp767; void* _temp769;
struct Cyc_Absyn_Exp* _temp771; struct Cyc_Absyn_Exp* _temp773; struct Cyc_Core_Opt*
_temp775; _LL755: if( _temp753 == 0){ goto _LL756;} else{ goto _LL757;} _LL757:
if( _temp753 == 0){ goto _LL759;} else{ _temp761=* _temp753; _LL765: _temp764=(
struct Cyc_Absyn_Exp*) _temp761.hd; _temp766=* _temp764; _LL776: _temp775=(
struct Cyc_Core_Opt*) _temp766.topt; goto _LL770; _LL770: _temp769=( void*)
_temp766.r; if((( struct _tunion_struct*) _temp769)->tag == Cyc_Absyn_Subscript_e_tag){
_LL774: _temp773=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp769)->f1; goto _LL772; _LL772: _temp771=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Subscript_e_struct*) _temp769)->f2; goto _LL768;} else{ goto _LL759;}
_LL768: _temp767=( struct Cyc_Position_Segment*) _temp766.loc; goto _LL763;
_LL763: _temp762=( struct Cyc_List_List*) _temp761.tl; goto _LL758;} _LL759:
goto _LL760; _LL756: { void* ansexp= Cyc_CfAbsexp_mkSkipAE(); void* assignexp=
Cyc_CfAbsexp_mkSkipAE();{ int j= 0; for( 0; _temp336 != 0; _temp336=({ struct
Cyc_List_List* _temp777= _temp336; if( _temp777 == 0){ _throw( Null_Exception);}
_temp777->tl;}), ++ j){ struct Cyc_List_List* ith_lvals= 0;{ struct Cyc_List_List*
ls= env.lhsides; for( 0; ls != 0; ls=({ struct Cyc_List_List* _temp778= ls; if(
_temp778 == 0){ _throw( Null_Exception);} _temp778->tl;})){ if( Cyc_CfAbsexp_isUnknownOp((
void*)({ struct Cyc_List_List* _temp779= ls; if( _temp779 == 0){ _throw(
Null_Exception);} _temp779->hd;}))){ ith_lvals=({ struct Cyc_List_List* _temp780=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp780->hd=(
void*) Cyc_CfAbsexp_mkUnknownOp(); _temp780->tl= ith_lvals; _temp780;});} else{
ith_lvals=({ struct Cyc_List_List* _temp781=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp781->hd=( void*) Cyc_CfAbsexp_mkMemberOp((
void*)({ struct Cyc_List_List* _temp782= ls; if( _temp782 == 0){ _throw(
Null_Exception);} _temp782->hd;}),( void*)({ struct Cyc_CfFlowInfo_TupleF_struct*
_temp783=( struct Cyc_CfFlowInfo_TupleF_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_TupleF_struct));
_temp783[ 0]=({ struct Cyc_CfFlowInfo_TupleF_struct _temp784; _temp784.tag= Cyc_CfFlowInfo_TupleF_tag;
_temp784.f1= j; _temp784;}); _temp783;})); _temp781->tl= ith_lvals; _temp781;});}}}{
struct Cyc_NewControlFlow_AbsSyn ith_syn= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_change_lhs(
env, ith_lvals),( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp785=
_temp336; if( _temp785 == 0){ _throw( Null_Exception);} _temp785->hd;})); void*
_temp788; void* _temp790; struct Cyc_NewControlFlow_AbsSynOne _temp786= Cyc_NewControlFlow_meet_absexp(
ith_syn); _LL791: _temp790=( void*) _temp786.inner_exp; goto _LL789; _LL789:
_temp788=( void*) _temp786.assigns; goto _LL787; _LL787: ansexp= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, ansexp, _temp790); assignexp= Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_mkAnyOrderG(), assignexp, _temp788);}}} return Cyc_NewControlFlow_mkSyn_tf(
ansexp, 0, assignexp);} _LL758: { int i=( int) Cyc_Evexp_eval_const_uint_exp(
_temp771); struct Cyc_NewControlFlow_AbsEnv other_env= Cyc_NewControlFlow_change_lhs(
env, 0); other_env.access_path= 0; env.access_path= _temp762;{ void* ansexp= Cyc_CfAbsexp_mkSkipAE();
void* assignexp= Cyc_CfAbsexp_mkSkipAE();{ int j= 0; for( 0; _temp336 != 0;
_temp336=({ struct Cyc_List_List* _temp792= _temp336; if( _temp792 == 0){ _throw(
Null_Exception);} _temp792->tl;}), ++ j){ if( i == j){ void* _temp796; void*
_temp798; struct Cyc_NewControlFlow_AbsSynOne _temp794= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( env,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp793= _temp336; if( _temp793 == 0){ _throw( Null_Exception);} _temp793->hd;})));
_LL799: _temp798=( void*) _temp794.inner_exp; goto _LL797; _LL797: _temp796=(
void*) _temp794.assigns; goto _LL795; _LL795: assignexp= _temp796; ansexp= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, ansexp, _temp798);} else{ void* _temp803; void*
_temp805; struct Cyc_NewControlFlow_AbsSynOne _temp801= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( other_env,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp800= _temp336; if( _temp800 == 0){ _throw( Null_Exception);} _temp800->hd;})));
_LL806: _temp805=( void*) _temp801.inner_exp; goto _LL804; _LL804: _temp803=(
void*) _temp801.assigns; goto _LL802; _LL802: ansexp= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, ansexp, _temp805);}}} return Cyc_NewControlFlow_mkSyn_tf(
ansexp,({ struct Cyc_List_List* _temp807=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp807->hd=( void*) Cyc_CfAbsexp_mkUnknownOp();
_temp807->tl= 0; _temp807;}), assignexp);}} _LL760:( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Core_Impossible_struct* _temp808=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp808[ 0]=({ struct
Cyc_Core_Impossible_struct _temp809; _temp809.tag= Cyc_Core_Impossible_tag;
_temp809.f1=( struct _tagged_string)({ char* _temp810=( char*)"abstract_exp: unexpected access path to Tuple";
struct _tagged_string _temp811; _temp811.curr= _temp810; _temp811.base= _temp810;
_temp811.last_plus_one= _temp810 + 46; _temp811;}); _temp809;}); _temp808;}));
_LL754:;} _LL181: { struct Cyc_List_List* _temp812= env.access_path; struct Cyc_List_List
_temp820; struct Cyc_List_List* _temp821; struct Cyc_Absyn_Exp* _temp823; struct
Cyc_Absyn_Exp _temp825; struct Cyc_Position_Segment* _temp826; void* _temp828;
struct _tagged_string* _temp830; struct Cyc_Absyn_Exp* _temp832; struct Cyc_Core_Opt*
_temp834; _LL814: if( _temp812 == 0){ goto _LL815;} else{ goto _LL816;} _LL816:
if( _temp812 == 0){ goto _LL818;} else{ _temp820=* _temp812; _LL824: _temp823=(
struct Cyc_Absyn_Exp*) _temp820.hd; _temp825=* _temp823; _LL835: _temp834=(
struct Cyc_Core_Opt*) _temp825.topt; goto _LL829; _LL829: _temp828=( void*)
_temp825.r; if((( struct _tunion_struct*) _temp828)->tag == Cyc_Absyn_StructMember_e_tag){
_LL833: _temp832=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp828)->f1; goto _LL831; _LL831: _temp830=( struct _tagged_string*)(( struct
Cyc_Absyn_StructMember_e_struct*) _temp828)->f2; goto _LL827;} else{ goto _LL818;}
_LL827: _temp826=( struct Cyc_Position_Segment*) _temp825.loc; goto _LL822;
_LL822: _temp821=( struct Cyc_List_List*) _temp820.tl; goto _LL817;} _LL818:
goto _LL819; _LL815: { void* ansexp= Cyc_CfAbsexp_mkSkipAE(); void* assignexp=
Cyc_CfAbsexp_mkSkipAE(); for( 0; _temp340 != 0; _temp340=({ struct Cyc_List_List*
_temp836= _temp340; if( _temp836 == 0){ _throw( Null_Exception);} _temp836->tl;})){
struct Cyc_List_List* ith_lvals= 0;{ struct Cyc_List_List* ls= env.lhsides; for(
0; ls != 0; ls=({ struct Cyc_List_List* _temp837= ls; if( _temp837 == 0){ _throw(
Null_Exception);} _temp837->tl;})){ if( Cyc_CfAbsexp_isUnknownOp(( void*)({
struct Cyc_List_List* _temp838= ls; if( _temp838 == 0){ _throw( Null_Exception);}
_temp838->hd;}))){ ith_lvals=({ struct Cyc_List_List* _temp839=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp839->hd=( void*) Cyc_CfAbsexp_mkUnknownOp();
_temp839->tl= ith_lvals; _temp839;});} else{ struct Cyc_List_List* ds=(*((
struct _tuple5*)({ struct Cyc_List_List* _temp840= _temp340; if( _temp840 == 0){
_throw( Null_Exception);} _temp840->hd;}))).f1; for( 0; ds != 0; ds=({ struct
Cyc_List_List* _temp841= ds; if( _temp841 == 0){ _throw( Null_Exception);}
_temp841->tl;})){ void* _temp843=( void*)({ struct Cyc_List_List* _temp842= ds;
if( _temp842 == 0){ _throw( Null_Exception);} _temp842->hd;}); struct Cyc_Absyn_Exp*
_temp849; struct _tagged_string* _temp851; _LL845: if((( struct _tunion_struct*)
_temp843)->tag == Cyc_Absyn_ArrayElement_tag){ _LL850: _temp849=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayElement_struct*) _temp843)->f1; goto _LL846;} else{ goto
_LL847;} _LL847: if((( struct _tunion_struct*) _temp843)->tag == Cyc_Absyn_FieldName_tag){
_LL852: _temp851=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp843)->f1; goto _LL848;} else{ goto _LL844;} _LL846:( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Core_Impossible_struct* _temp853=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp853[ 0]=({ struct
Cyc_Core_Impossible_struct _temp854; _temp854.tag= Cyc_Core_Impossible_tag;
_temp854.f1=( struct _tagged_string)({ char* _temp855=( char*)"bad struct designator";
struct _tagged_string _temp856; _temp856.curr= _temp855; _temp856.base= _temp855;
_temp856.last_plus_one= _temp855 + 22; _temp856;}); _temp854;}); _temp853;}));
_LL848: ith_lvals=({ struct Cyc_List_List* _temp857=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp857->hd=( void*) Cyc_CfAbsexp_mkMemberOp((
void*)({ struct Cyc_List_List* _temp858= ls; if( _temp858 == 0){ _throw(
Null_Exception);} _temp858->hd;}),( void*)({ struct Cyc_CfFlowInfo_StructF_struct*
_temp859=( struct Cyc_CfFlowInfo_StructF_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructF_struct));
_temp859[ 0]=({ struct Cyc_CfFlowInfo_StructF_struct _temp860; _temp860.tag= Cyc_CfFlowInfo_StructF_tag;
_temp860.f1= _temp851; _temp860;}); _temp859;})); _temp857->tl= ith_lvals;
_temp857;}); goto _LL844; _LL844:;}}}}{ void** ith_topt=( void*)({ struct Cyc_Core_Opt*
_temp862=((*(( struct _tuple5*)({ struct Cyc_List_List* _temp861= _temp340; if(
_temp861 == 0){ _throw( Null_Exception);} _temp861->hd;}))).f2)->topt; if(
_temp862 == 0){ _throw( Null_Exception);}& _temp862->v;}); struct Cyc_NewControlFlow_AbsSyn
ith_syn= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_change_lhs( env,
ith_lvals),(*(( struct _tuple5*)({ struct Cyc_List_List* _temp863= _temp340; if(
_temp863 == 0){ _throw( Null_Exception);} _temp863->hd;}))).f2); void* _temp866;
void* _temp868; struct Cyc_NewControlFlow_AbsSynOne _temp864= Cyc_NewControlFlow_meet_absexp(
ith_syn); _LL869: _temp868=( void*) _temp864.inner_exp; goto _LL867; _LL867:
_temp866=( void*) _temp864.assigns; goto _LL865; _LL865: ansexp= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, ansexp, _temp868); assignexp= Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_mkAnyOrderG(), assignexp, _temp866);}} return Cyc_NewControlFlow_mkSyn_tf(
ansexp, 0, assignexp);} _LL817: { struct Cyc_NewControlFlow_AbsEnv other_env=
Cyc_NewControlFlow_change_lhs( env, 0); other_env.access_path= 0; env.access_path=
_temp821;{ void* ansexp= Cyc_CfAbsexp_mkSkipAE(); void* assignexp= Cyc_CfAbsexp_mkSkipAE();
for( 0; _temp340 != 0; _temp340=({ struct Cyc_List_List* _temp870= _temp340; if(
_temp870 == 0){ _throw( Null_Exception);} _temp870->tl;})){ int used= 0;{ struct
Cyc_List_List* ds=(*(( struct _tuple5*)({ struct Cyc_List_List* _temp871=
_temp340; if( _temp871 == 0){ _throw( Null_Exception);} _temp871->hd;}))).f1;
for( 0; ds != 0; ds=({ struct Cyc_List_List* _temp872= ds; if( _temp872 == 0){
_throw( Null_Exception);} _temp872->tl;})){ void* _temp874=( void*)({ struct Cyc_List_List*
_temp873= ds; if( _temp873 == 0){ _throw( Null_Exception);} _temp873->hd;});
struct Cyc_Absyn_Exp* _temp880; struct _tagged_string* _temp882; _LL876: if(((
struct _tunion_struct*) _temp874)->tag == Cyc_Absyn_ArrayElement_tag){ _LL881:
_temp880=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayElement_struct*)
_temp874)->f1; goto _LL877;} else{ goto _LL878;} _LL878: if((( struct
_tunion_struct*) _temp874)->tag == Cyc_Absyn_FieldName_tag){ _LL883: _temp882=(
struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp874)->f1;
goto _LL879;} else{ goto _LL875;} _LL877:( void) _throw(( struct _xtunion_struct*)({
struct Cyc_Core_Impossible_struct* _temp884=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp884[ 0]=({ struct
Cyc_Core_Impossible_struct _temp885; _temp885.tag= Cyc_Core_Impossible_tag;
_temp885.f1=( struct _tagged_string)({ char* _temp886=( char*)"bad struct designator";
struct _tagged_string _temp887; _temp887.curr= _temp886; _temp887.base= _temp886;
_temp887.last_plus_one= _temp886 + 22; _temp887;}); _temp885;}); _temp884;}));
_LL879: if( Cyc_String_zstrptrcmp( _temp830, _temp882) == 0){ used= 1;} goto
_LL875; _LL875:;}} if( used){ void* _temp891; void* _temp893; struct Cyc_NewControlFlow_AbsSynOne
_temp889= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp( env,(*((
struct _tuple5*)({ struct Cyc_List_List* _temp888= _temp340; if( _temp888 == 0){
_throw( Null_Exception);} _temp888->hd;}))).f2)); _LL894: _temp893=( void*)
_temp889.inner_exp; goto _LL892; _LL892: _temp891=( void*) _temp889.assigns;
goto _LL890; _LL890: assignexp= _temp891; ansexp= Cyc_CfAbsexp_mkGroupAE(( void*)
Cyc_CfAbsexp_UnorderedG, ansexp, _temp893);} else{ void* _temp898; void*
_temp900; struct Cyc_NewControlFlow_AbsSynOne _temp896= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( other_env,(*(( struct _tuple5*)({ struct Cyc_List_List*
_temp895= _temp340; if( _temp895 == 0){ _throw( Null_Exception);} _temp895->hd;}))).f2));
_LL901: _temp900=( void*) _temp896.inner_exp; goto _LL899; _LL899: _temp898=(
void*) _temp896.assigns; goto _LL897; _LL897: ansexp= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, ansexp, _temp900);}} return Cyc_NewControlFlow_mkSyn_tf(
ansexp, 0, assignexp);}} _LL819:( void) _throw(( struct _xtunion_struct*)({
struct Cyc_Core_Impossible_struct* _temp902=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp902[ 0]=({ struct
Cyc_Core_Impossible_struct _temp903; _temp903.tag= Cyc_Core_Impossible_tag;
_temp903.f1=( struct _tagged_string)({ char* _temp904=( char*)"abstract_exp: unexpected access path to Struct";
struct _tagged_string _temp905; _temp905.curr= _temp904; _temp905.base= _temp904;
_temp905.last_plus_one= _temp904 + 47; _temp905;}); _temp903;}); _temp902;}));
_LL813:;} _LL183: return Cyc_NewControlFlow_use_exp_unordered( env,(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)((
struct Cyc_Absyn_Exp*(*)( struct _tuple5*)) Cyc_Core_snd, _temp346), 0); _LL185:
return Cyc_NewControlFlow_use_exp_unordered( env,({ struct Cyc_List_List*
_temp906=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp906->hd=( void*) _temp350; _temp906->tl= 0; _temp906;}), 0); _LL187: {
struct Cyc_NewControlFlow_AbsSyn esyn= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env(
env), _temp352); return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*)( Cyc_NewControlFlow_meet_absexp( esyn)).inner_exp,
Cyc_CfAbsexp_mkBottomAE()), 0, Cyc_CfAbsexp_mkSkipAE());} _LL189: { struct Cyc_List_List*
_temp909; struct Cyc_NewControlFlow_AbsSynOne _temp911; struct Cyc_NewControlFlow_AbsSynOne
_temp913; struct Cyc_NewControlFlow_AbsSyn _temp907= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp358); _LL914: _temp913=( struct Cyc_NewControlFlow_AbsSynOne)
_temp907.when_true; goto _LL912; _LL912: _temp911=( struct Cyc_NewControlFlow_AbsSynOne)
_temp907.when_false; goto _LL910; _LL910: _temp909=( struct Cyc_List_List*)
_temp907.lvalues; goto _LL908; _LL908: { struct Cyc_List_List* _temp917; struct
Cyc_NewControlFlow_AbsSynOne _temp919; struct Cyc_NewControlFlow_AbsSynOne
_temp921; struct Cyc_NewControlFlow_AbsSyn _temp915= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp356); _LL922: _temp921=( struct Cyc_NewControlFlow_AbsSynOne)
_temp915.when_true; goto _LL920; _LL920: _temp919=( struct Cyc_NewControlFlow_AbsSynOne)
_temp915.when_false; goto _LL918; _LL918: _temp917=( struct Cyc_List_List*)
_temp915.lvalues; goto _LL916; _LL916: { struct Cyc_List_List* _temp925; struct
Cyc_NewControlFlow_AbsSynOne _temp927; struct Cyc_NewControlFlow_AbsSynOne
_temp929; struct Cyc_NewControlFlow_AbsSyn _temp923= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp354); _LL930: _temp929=( struct Cyc_NewControlFlow_AbsSynOne)
_temp923.when_true; goto _LL928; _LL928: _temp927=( struct Cyc_NewControlFlow_AbsSynOne)
_temp923.when_false; goto _LL926; _LL926: _temp925=( struct Cyc_List_List*)
_temp923.lvalues; goto _LL924; _LL924: { void* true_exp= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OneofG, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*) _temp913.inner_exp,(
void*) _temp913.assigns), Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*) _temp921.inner_exp,( void*) _temp921.assigns)), Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*) _temp911.inner_exp,( void*) _temp911.assigns), Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) _temp929.inner_exp,( void*) _temp929.assigns)));
void* false_exp= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OneofG, Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*) _temp913.inner_exp,( void*) _temp913.assigns), Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) _temp919.inner_exp,( void*) _temp919.assigns)),
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) _temp911.inner_exp,( void*) _temp911.assigns),
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*) _temp927.inner_exp,(
void*) _temp927.assigns))); return({ struct Cyc_NewControlFlow_AbsSyn _temp931;
_temp931.when_true=({ struct Cyc_NewControlFlow_AbsSynOne _temp933; _temp933.inner_exp=(
void*) true_exp; _temp933.assigns=( void*) Cyc_NewControlFlow_make_assigns( env,
Cyc_CfAbsexp_mkUnknownOp()); _temp933;}); _temp931.when_false=({ struct Cyc_NewControlFlow_AbsSynOne
_temp932; _temp932.inner_exp=( void*) false_exp; _temp932.assigns=( void*) Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()); _temp932;}); _temp931.lvalues= 0; _temp931;});}}}}
_LL191: { struct Cyc_NewControlFlow_AbsEnv env2= Cyc_NewControlFlow_change_lhs(
env, 0); env2.access_path= 0; Cyc_NewControlFlow_cf_prepass( env2, _temp360);{
struct Cyc_Absyn_Stmt* last_s= _temp360; while( 1) { void* _temp934=( void*)
last_s->r; struct Cyc_Absyn_Stmt* _temp944; struct Cyc_Absyn_Stmt* _temp946;
struct Cyc_Absyn_Stmt* _temp948; struct Cyc_Absyn_Decl* _temp950; struct Cyc_Absyn_Exp*
_temp952; _LL936: if(( unsigned int) _temp934 > 1u?(( struct _tunion_struct*)
_temp934)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL947: _temp946=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp934)->f1; goto _LL945; _LL945: _temp944=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp934)->f2; goto
_LL937;} else{ goto _LL938;} _LL938: if(( unsigned int) _temp934 > 1u?(( struct
_tunion_struct*) _temp934)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL951: _temp950=(
struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*) _temp934)->f1; goto
_LL949; _LL949: _temp948=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp934)->f2; goto _LL939;} else{ goto _LL940;} _LL940: if(( unsigned int)
_temp934 > 1u?(( struct _tunion_struct*) _temp934)->tag == Cyc_Absyn_Exp_s_tag:
0){ _LL953: _temp952=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp934)->f1; goto _LL941;} else{ goto _LL942;} _LL942: goto _LL943; _LL937:
last_s= _temp944; continue; _LL939: last_s= _temp948; continue; _LL941: { struct
Cyc_NewControlFlow_AbsSyn syn= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env(
env), _temp952); void* _temp956; void* _temp958; struct Cyc_NewControlFlow_AbsSynOne
_temp954= Cyc_NewControlFlow_meet_absexp( syn); _LL959: _temp958=( void*)
_temp954.inner_exp; goto _LL957; _LL957: _temp956=( void*) _temp954.assigns;
goto _LL955; _LL955:( void*)({ struct Cyc_List_List* _temp960=( Cyc_NewControlFlow_get_stmt_annot(
last_s))->absexps; if( _temp960 == 0){ _throw( Null_Exception);} _temp960->hd=(
void*) Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp958, _temp956);});
return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkStmtAE( _temp360), 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));} _LL943:( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Core_Impossible_struct* _temp961=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp961[ 0]=({ struct
Cyc_Core_Impossible_struct _temp962; _temp962.tag= Cyc_Core_Impossible_tag;
_temp962.f1=( struct _tagged_string)({ char* _temp963=( char*)"abstract_exp: ill-formed StmtExp";
struct _tagged_string _temp964; _temp964.curr= _temp963; _temp964.base= _temp963;
_temp964.last_plus_one= _temp963 + 33; _temp964;}); _temp962;}); _temp961;}));
_LL935:;}}} _LL193: { struct Cyc_NewControlFlow_AbsEnv env1= Cyc_NewControlFlow_change_lhs(
env, 0); env1.access_path= 0;{ struct Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp(
env1, _temp364); struct Cyc_NewControlFlow_AbsSyn syn2= Cyc_NewControlFlow_abstract_exp(
env, _temp362); struct Cyc_NewControlFlow_AbsSynOne syn1meet= Cyc_NewControlFlow_meet_absexp(
syn1); return({ struct Cyc_NewControlFlow_AbsSyn _temp965; _temp965.when_true=({
struct Cyc_NewControlFlow_AbsSynOne _temp967; _temp967.inner_exp=( void*) Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) syn1meet.inner_exp,( void*)( syn2.when_true).inner_exp);
_temp967.assigns=( void*)(( void*)( syn2.when_true).assigns); _temp967;});
_temp965.when_false=({ struct Cyc_NewControlFlow_AbsSynOne _temp966; _temp966.inner_exp=(
void*) Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*) syn1meet.inner_exp,(
void*)( syn2.when_false).inner_exp); _temp966.assigns=( void*)(( void*)( syn2.when_false).assigns);
_temp966;}); _temp965.lvalues= 0; _temp965;});}} _LL195: env.access_path == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp968=( char*)"new_control_flow.cyc:752 env.access_path==null";
struct _tagged_string _temp969; _temp969.curr= _temp968; _temp969.base= _temp968;
_temp969.last_plus_one= _temp968 + 47; _temp969;})); Cyc_NewControlFlow_add_var_root(
env, _temp370);( env.shared)->comprehension_vars=({ struct Cyc_List_List*
_temp970=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp970->hd=( void*) _temp370; _temp970->tl=( env.shared)->comprehension_vars;
_temp970;});{ struct Cyc_NewControlFlow_AbsEnv elt_env= Cyc_NewControlFlow_use_it_env(
env); void* _temp973; void* _temp975; struct Cyc_NewControlFlow_AbsSynOne
_temp971= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
elt_env, _temp368)); _LL976: _temp975=( void*) _temp971.inner_exp; goto _LL974;
_LL974: _temp973=( void*) _temp971.assigns; goto _LL972; _LL972: { void*
_temp979; void* _temp981; struct Cyc_NewControlFlow_AbsSynOne _temp977= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( elt_env, _temp366)); _LL982: _temp981=( void*)
_temp977.inner_exp; goto _LL980; _LL980: _temp979=( void*) _temp977.assigns;
goto _LL978; _LL978: { void* ansexp= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
_temp975, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp973, Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkAssignAE( Cyc_CfAbsexp_mkLocalOp(
_temp370), Cyc_CfAbsexp_mkUnknownOp()), Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OneofG,
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp981, _temp979), Cyc_CfAbsexp_mkSkipAE()))));
return Cyc_NewControlFlow_mkSyn_tf( ansexp, 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));}}} _LL197: goto _LL199; _LL199: goto _LL201;
_LL201: goto _LL203; _LL203: goto _LL205; _LL205: goto _LL207; _LL207: goto
_LL209; _LL209:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp983=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp983[ 0]=({ struct Cyc_Core_Impossible_struct _temp984; _temp984.tag= Cyc_Core_Impossible_tag;
_temp984.f1=( struct _tagged_string)({ char* _temp985=( char*)"abstract_exp, unexpected exp form";
struct _tagged_string _temp986; _temp986.curr= _temp985; _temp986.base= _temp985;
_temp986.last_plus_one= _temp985 + 34; _temp986;}); _temp984;}); _temp983;}));
_LL123:;} void* Cyc_NewControlFlow_abstract_exp_top( struct Cyc_NewControlFlow_AbsEnv
env, struct Cyc_Absyn_Exp* e){ void* _temp989; void* _temp991; struct Cyc_NewControlFlow_AbsSynOne
_temp987= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp( env,
e)); _LL992: _temp991=( void*) _temp987.inner_exp; goto _LL990; _LL990: _temp989=(
void*) _temp987.assigns; goto _LL988; _LL988: return Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, _temp991, _temp989);} struct _tuple0 Cyc_NewControlFlow_abstract_guard(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e){ struct Cyc_NewControlFlow_AbsSyn
syn= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env( env), e);
return({ struct _tuple0 _temp993; _temp993.f1= Cyc_CfAbsexp_mkGroupAE(( void*)
Cyc_CfAbsexp_OrderedG,( void*)( syn.when_true).inner_exp,( void*)( syn.when_true).assigns);
_temp993.f2= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*)( syn.when_false).inner_exp,(
void*)( syn.when_false).assigns); _temp993;});} struct Cyc_List_List* Cyc_NewControlFlow_cf_prepass_scs(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_List_List* scs){ struct Cyc_List_List*
ans= 0; for( 0; scs != 0; scs=({ struct Cyc_List_List* _temp994= scs; if(
_temp994 == 0){ _throw( Null_Exception);} _temp994->tl;})){ struct Cyc_Absyn_Switch_clause
_temp998; struct Cyc_Position_Segment* _temp999; struct Cyc_Absyn_Stmt*
_temp1001; struct Cyc_Absyn_Exp* _temp1003; struct Cyc_Core_Opt* _temp1005;
struct Cyc_Absyn_Pat* _temp1007; struct Cyc_Absyn_Switch_clause* _temp996=(
struct Cyc_Absyn_Switch_clause*)({ struct Cyc_List_List* _temp995= scs; if(
_temp995 == 0){ _throw( Null_Exception);} _temp995->hd;}); _temp998=* _temp996;
_LL1008: _temp1007=( struct Cyc_Absyn_Pat*) _temp998.pattern; goto _LL1006;
_LL1006: _temp1005=( struct Cyc_Core_Opt*) _temp998.pat_vars; goto _LL1004;
_LL1004: _temp1003=( struct Cyc_Absyn_Exp*) _temp998.where_clause; goto _LL1002;
_LL1002: _temp1001=( struct Cyc_Absyn_Stmt*) _temp998.body; goto _LL1000;
_LL1000: _temp999=( struct Cyc_Position_Segment*) _temp998.loc; goto _LL997;
_LL997: if( _temp1005 == 0){( void) _throw(( struct _xtunion_struct*)({ struct
Cyc_Core_Impossible_struct* _temp1009=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp1009[ 0]=({ struct
Cyc_Core_Impossible_struct _temp1010; _temp1010.tag= Cyc_Core_Impossible_tag;
_temp1010.f1=( struct _tagged_string)({ char* _temp1011=( char*)"switch clause vds not set";
struct _tagged_string _temp1012; _temp1012.curr= _temp1011; _temp1012.base=
_temp1011; _temp1012.last_plus_one= _temp1011 + 26; _temp1012;}); _temp1010;});
_temp1009;}));}{ struct Cyc_List_List* vds=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp1013= _temp1005; if( _temp1013 == 0){ _throw( Null_Exception);} _temp1013->v;});
for( 0; vds != 0; vds=({ struct Cyc_List_List* _temp1014= vds; if( _temp1014 ==
0){ _throw( Null_Exception);} _temp1014->tl;})){ Cyc_NewControlFlow_add_var_root(
env,( struct Cyc_Absyn_Vardecl*)({ struct Cyc_List_List* _temp1015= vds; if(
_temp1015 == 0){ _throw( Null_Exception);} _temp1015->hd;}));}} if( _temp1003 !=
0){ void* _temp1019; void* _temp1021; struct _tuple0 _temp1017= Cyc_NewControlFlow_abstract_guard(
env,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1016= _temp1003; if(
_temp1016 == 0){ _throw( Null_Exception);} _temp1016;})); _LL1022: _temp1021=
_temp1017.f1; goto _LL1020; _LL1020: _temp1019= _temp1017.f2; goto _LL1018;
_LL1018: ans=({ struct Cyc_List_List* _temp1023=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1023->hd=( void*) _temp1021;
_temp1023->tl=({ struct Cyc_List_List* _temp1024=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1024->hd=( void*) _temp1019;
_temp1024->tl= ans; _temp1024;}); _temp1023;});} Cyc_NewControlFlow_cf_prepass(
env, _temp1001);} return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( ans);} void Cyc_NewControlFlow_cf_prepass( struct Cyc_NewControlFlow_AbsEnv
env, struct Cyc_Absyn_Stmt* s){ env.lhsides == 0? 0:(( int(*)( struct
_tagged_string msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({ char*
_temp1025=( char*)"new_control_flow.cyc:804 env.lhsides == null"; struct
_tagged_string _temp1026; _temp1026.curr= _temp1025; _temp1026.base= _temp1025;
_temp1026.last_plus_one= _temp1025 + 45; _temp1026;})); env.access_path == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp1027=( char*)"new_control_flow.cyc:805 env.access_path == null";
struct _tagged_string _temp1028; _temp1028.curr= _temp1027; _temp1028.base=
_temp1027; _temp1028.last_plus_one= _temp1027 + 49; _temp1028;}));{ void*
_temp1029=( void*) s->r; struct Cyc_Absyn_Exp* _temp1077; struct Cyc_Absyn_Stmt*
_temp1079; struct Cyc_Absyn_Stmt* _temp1081; struct Cyc_Absyn_Exp* _temp1083;
struct Cyc_Absyn_Exp* _temp1085; struct Cyc_Absyn_Stmt* _temp1087; struct Cyc_Absyn_Stmt*
_temp1089; struct Cyc_Absyn_Exp* _temp1091; struct _tuple3 _temp1093; struct Cyc_Absyn_Stmt*
_temp1095; struct Cyc_Absyn_Exp* _temp1097; struct Cyc_Absyn_Stmt* _temp1099;
struct Cyc_Absyn_Stmt* _temp1101; struct _tuple3 _temp1103; struct Cyc_Absyn_Stmt*
_temp1105; struct Cyc_Absyn_Exp* _temp1107; struct Cyc_Absyn_Stmt* _temp1109;
struct Cyc_Absyn_Stmt* _temp1111; struct Cyc_Absyn_Stmt* _temp1113; struct
_tagged_string* _temp1115; struct Cyc_Absyn_Stmt* _temp1117; struct
_tagged_string* _temp1119; struct Cyc_Absyn_Stmt* _temp1121; struct _tuple3
_temp1123; struct Cyc_Absyn_Stmt* _temp1125; struct Cyc_Absyn_Exp* _temp1127;
struct _tuple3 _temp1129; struct Cyc_Absyn_Stmt* _temp1131; struct Cyc_Absyn_Exp*
_temp1133; struct Cyc_Absyn_Exp* _temp1135; struct Cyc_List_List* _temp1137;
struct Cyc_Absyn_Exp* _temp1139; struct Cyc_Absyn_Switch_clause** _temp1141;
struct Cyc_List_List* _temp1143; struct Cyc_Absyn_Stmt* _temp1145; struct Cyc_Absyn_Decl*
_temp1147; struct Cyc_Absyn_Decl _temp1149; struct Cyc_Position_Segment*
_temp1150; void* _temp1152; struct Cyc_Absyn_Vardecl* _temp1154; struct Cyc_Absyn_Stmt*
_temp1156; struct Cyc_Absyn_Decl* _temp1158; struct Cyc_Absyn_Decl _temp1160;
struct Cyc_Position_Segment* _temp1161; void* _temp1163; int _temp1165; struct
Cyc_Absyn_Exp* _temp1167; struct Cyc_Core_Opt* _temp1169; struct Cyc_Core_Opt*
_temp1171; struct Cyc_Core_Opt _temp1173; struct Cyc_List_List* _temp1174;
struct Cyc_Absyn_Pat* _temp1176; struct Cyc_Absyn_Stmt* _temp1178; struct Cyc_Absyn_Decl*
_temp1180; struct Cyc_Absyn_Stmt* _temp1182; struct Cyc_Absyn_Stmt* _temp1184;
struct Cyc_Absyn_Stmt* _temp1186; struct _tagged_string* _temp1188; struct Cyc_List_List*
_temp1190; struct Cyc_Absyn_Stmt* _temp1192; struct Cyc_Absyn_Stmt* _temp1194;
struct Cyc_Absyn_Vardecl* _temp1196; struct Cyc_Absyn_Tvar* _temp1198; _LL1031:
if(( int) _temp1029 == Cyc_Absyn_Skip_s){ goto _LL1032;} else{ goto _LL1033;}
_LL1033: if(( unsigned int) _temp1029 > 1u?(( struct _tunion_struct*) _temp1029)->tag
== Cyc_Absyn_Exp_s_tag: 0){ _LL1078: _temp1077=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Exp_s_struct*) _temp1029)->f1; goto _LL1034;} else{ goto _LL1035;}
_LL1035: if(( unsigned int) _temp1029 > 1u?(( struct _tunion_struct*) _temp1029)->tag
== Cyc_Absyn_Seq_s_tag: 0){ _LL1082: _temp1081=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp1029)->f1; goto _LL1080; _LL1080: _temp1079=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1029)->f2; goto
_LL1036;} else{ goto _LL1037;} _LL1037: if(( unsigned int) _temp1029 > 1u?((
struct _tunion_struct*) _temp1029)->tag == Cyc_Absyn_Return_s_tag: 0){ _LL1084:
_temp1083=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp1029)->f1; if( _temp1083 == 0){ goto _LL1038;} else{ goto _LL1039;}} else{
goto _LL1039;} _LL1039: if(( unsigned int) _temp1029 > 1u?(( struct
_tunion_struct*) _temp1029)->tag == Cyc_Absyn_Return_s_tag: 0){ _LL1086:
_temp1085=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp1029)->f1; goto _LL1040;} else{ goto _LL1041;} _LL1041: if(( unsigned int)
_temp1029 > 1u?(( struct _tunion_struct*) _temp1029)->tag == Cyc_Absyn_IfThenElse_s_tag:
0){ _LL1092: _temp1091=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1029)->f1; goto _LL1090; _LL1090: _temp1089=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1029)->f2; goto _LL1088; _LL1088:
_temp1087=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1029)->f3; goto _LL1042;} else{ goto _LL1043;} _LL1043: if(( unsigned int)
_temp1029 > 1u?(( struct _tunion_struct*) _temp1029)->tag == Cyc_Absyn_Do_s_tag:
0){ _LL1100: _temp1099=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*)
_temp1029)->f1; goto _LL1094; _LL1094: _temp1093=( struct _tuple3)(( struct Cyc_Absyn_Do_s_struct*)
_temp1029)->f2; _LL1098: _temp1097= _temp1093.f1; goto _LL1096; _LL1096:
_temp1095= _temp1093.f2; goto _LL1044;} else{ goto _LL1045;} _LL1045: if((
unsigned int) _temp1029 > 1u?(( struct _tunion_struct*) _temp1029)->tag == Cyc_Absyn_While_s_tag:
0){ _LL1104: _temp1103=( struct _tuple3)(( struct Cyc_Absyn_While_s_struct*)
_temp1029)->f1; _LL1108: _temp1107= _temp1103.f1; goto _LL1106; _LL1106:
_temp1105= _temp1103.f2; goto _LL1102; _LL1102: _temp1101=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_While_s_struct*) _temp1029)->f2; goto _LL1046;} else{ goto
_LL1047;} _LL1047: if(( unsigned int) _temp1029 > 1u?(( struct _tunion_struct*)
_temp1029)->tag == Cyc_Absyn_Break_s_tag: 0){ _LL1110: _temp1109=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Break_s_struct*) _temp1029)->f1; goto _LL1048;} else{ goto
_LL1049;} _LL1049: if(( unsigned int) _temp1029 > 1u?(( struct _tunion_struct*)
_temp1029)->tag == Cyc_Absyn_Continue_s_tag: 0){ _LL1112: _temp1111=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Continue_s_struct*) _temp1029)->f1; goto _LL1050;} else{ goto
_LL1051;} _LL1051: if(( unsigned int) _temp1029 > 1u?(( struct _tunion_struct*)
_temp1029)->tag == Cyc_Absyn_Goto_s_tag: 0){ _LL1116: _temp1115=( struct
_tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp1029)->f1; goto _LL1114;
_LL1114: _temp1113=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp1029)->f2; if( _temp1113 == 0){ goto _LL1052;} else{ goto _LL1053;}} else{
goto _LL1053;} _LL1053: if(( unsigned int) _temp1029 > 1u?(( struct
_tunion_struct*) _temp1029)->tag == Cyc_Absyn_Goto_s_tag: 0){ _LL1120: _temp1119=(
struct _tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp1029)->f1; goto
_LL1118; _LL1118: _temp1117=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp1029)->f2; goto _LL1054;} else{ goto _LL1055;} _LL1055: if(( unsigned int)
_temp1029 > 1u?(( struct _tunion_struct*) _temp1029)->tag == Cyc_Absyn_For_s_tag:
0){ _LL1136: _temp1135=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp1029)->f1; goto _LL1130; _LL1130: _temp1129=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp1029)->f2; _LL1134: _temp1133= _temp1129.f1; goto _LL1132; _LL1132:
_temp1131= _temp1129.f2; goto _LL1124; _LL1124: _temp1123=( struct _tuple3)((
struct Cyc_Absyn_For_s_struct*) _temp1029)->f3; _LL1128: _temp1127= _temp1123.f1;
goto _LL1126; _LL1126: _temp1125= _temp1123.f2; goto _LL1122; _LL1122: _temp1121=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp1029)->f4; goto
_LL1056;} else{ goto _LL1057;} _LL1057: if(( unsigned int) _temp1029 > 1u?((
struct _tunion_struct*) _temp1029)->tag == Cyc_Absyn_Switch_s_tag: 0){ _LL1140:
_temp1139=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp1029)->f1; goto _LL1138; _LL1138: _temp1137=( struct Cyc_List_List*)((
struct Cyc_Absyn_Switch_s_struct*) _temp1029)->f2; goto _LL1058;} else{ goto
_LL1059;} _LL1059: if(( unsigned int) _temp1029 > 1u?(( struct _tunion_struct*)
_temp1029)->tag == Cyc_Absyn_Fallthru_s_tag: 0){ _LL1144: _temp1143=( struct Cyc_List_List*)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1029)->f1; goto _LL1142; _LL1142:
_temp1141=( struct Cyc_Absyn_Switch_clause**)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1029)->f2; goto _LL1060;} else{ goto _LL1061;} _LL1061: if(( unsigned int)
_temp1029 > 1u?(( struct _tunion_struct*) _temp1029)->tag == Cyc_Absyn_Decl_s_tag:
0){ _LL1148: _temp1147=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1029)->f1; _temp1149=* _temp1147; _LL1153: _temp1152=( void*) _temp1149.r;
if((( struct _tunion_struct*) _temp1152)->tag == Cyc_Absyn_Var_d_tag){ _LL1155:
_temp1154=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp1152)->f1; goto _LL1151;} else{ goto _LL1063;} _LL1151: _temp1150=( struct
Cyc_Position_Segment*) _temp1149.loc; goto _LL1146; _LL1146: _temp1145=( struct
Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1029)->f2; goto _LL1062;}
else{ goto _LL1063;} _LL1063: if(( unsigned int) _temp1029 > 1u?(( struct
_tunion_struct*) _temp1029)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL1159: _temp1158=(
struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1029)->f1;
_temp1160=* _temp1158; _LL1164: _temp1163=( void*) _temp1160.r; if((( struct
_tunion_struct*) _temp1163)->tag == Cyc_Absyn_Let_d_tag){ _LL1177: _temp1176=(
struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*) _temp1163)->f1; goto
_LL1172; _LL1172: _temp1171=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1163)->f2; if( _temp1171 == 0){ goto _LL1065;} else{ _temp1173=* _temp1171;
_LL1175: _temp1174=( struct Cyc_List_List*) _temp1173.v; goto _LL1170;} _LL1170:
_temp1169=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp1163)->f3;
goto _LL1168; _LL1168: _temp1167=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1163)->f4; goto _LL1166; _LL1166: _temp1165=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp1163)->f5; goto _LL1162;} else{ goto _LL1065;} _LL1162: _temp1161=( struct
Cyc_Position_Segment*) _temp1160.loc; goto _LL1157; _LL1157: _temp1156=( struct
Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1029)->f2; goto _LL1064;}
else{ goto _LL1065;} _LL1065: if(( unsigned int) _temp1029 > 1u?(( struct
_tunion_struct*) _temp1029)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL1181: _temp1180=(
struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1029)->f1; goto
_LL1179; _LL1179: _temp1178=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1029)->f2; goto _LL1066;} else{ goto _LL1067;} _LL1067: if(( unsigned int)
_temp1029 > 1u?(( struct _tunion_struct*) _temp1029)->tag == Cyc_Absyn_Cut_s_tag:
0){ _LL1183: _temp1182=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Cut_s_struct*)
_temp1029)->f1; goto _LL1068;} else{ goto _LL1069;} _LL1069: if(( unsigned int)
_temp1029 > 1u?(( struct _tunion_struct*) _temp1029)->tag == Cyc_Absyn_Splice_s_tag:
0){ _LL1185: _temp1184=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Splice_s_struct*)
_temp1029)->f1; goto _LL1070;} else{ goto _LL1071;} _LL1071: if(( unsigned int)
_temp1029 > 1u?(( struct _tunion_struct*) _temp1029)->tag == Cyc_Absyn_Label_s_tag:
0){ _LL1189: _temp1188=( struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*)
_temp1029)->f1; goto _LL1187; _LL1187: _temp1186=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Label_s_struct*) _temp1029)->f2; goto _LL1072;} else{ goto
_LL1073;} _LL1073: if(( unsigned int) _temp1029 > 1u?(( struct _tunion_struct*)
_temp1029)->tag == Cyc_Absyn_TryCatch_s_tag: 0){ _LL1193: _temp1192=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp1029)->f1; goto _LL1191; _LL1191:
_temp1190=( struct Cyc_List_List*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1029)->f2; goto _LL1074;} else{ goto _LL1075;} _LL1075: if(( unsigned int)
_temp1029 > 1u?(( struct _tunion_struct*) _temp1029)->tag == Cyc_Absyn_Region_s_tag:
0){ _LL1199: _temp1198=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_Region_s_struct*)
_temp1029)->f1; goto _LL1197; _LL1197: _temp1196=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Region_s_struct*) _temp1029)->f2; goto _LL1195; _LL1195:
_temp1194=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Region_s_struct*)
_temp1029)->f3; goto _LL1076;} else{ goto _LL1030;} _LL1032: return; _LL1034: {
void* ae= Cyc_NewControlFlow_abstract_exp_top( env, _temp1077); Cyc_NewControlFlow_cf_set_absexps(
s,({ struct Cyc_List_List* _temp1200=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1200->hd=( void*) ae; _temp1200->tl= 0; _temp1200;}));
return;} _LL1036: Cyc_NewControlFlow_cf_prepass( env, _temp1081); Cyc_NewControlFlow_cf_prepass(
env, _temp1079); return; _LL1038: return; _LL1040: { void* ae= Cyc_NewControlFlow_abstract_exp_top(
Cyc_NewControlFlow_use_it_env( env),( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1201= _temp1085; if( _temp1201 == 0){ _throw( Null_Exception);} _temp1201;}));
Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List* _temp1202=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1202->hd=( void*)
ae; _temp1202->tl= 0; _temp1202;})); return;} _LL1042: { void* _temp1205; void*
_temp1207; struct _tuple0 _temp1203= Cyc_NewControlFlow_abstract_guard( env,
_temp1091); _LL1208: _temp1207= _temp1203.f1; goto _LL1206; _LL1206: _temp1205=
_temp1203.f2; goto _LL1204; _LL1204: Cyc_NewControlFlow_cf_set_absexps( s,({
struct Cyc_List_List* _temp1209=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1209->hd=( void*) _temp1207; _temp1209->tl=({
struct Cyc_List_List* _temp1210=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1210->hd=( void*) _temp1205; _temp1210->tl= 0;
_temp1210;}); _temp1209;})); Cyc_NewControlFlow_cf_prepass( env, _temp1089); Cyc_NewControlFlow_cf_prepass(
env, _temp1087); return;} _LL1044: _temp1107= _temp1097; _temp1105= _temp1095;
_temp1101= _temp1099; goto _LL1046; _LL1046: { void* _temp1213; void* _temp1215;
struct _tuple0 _temp1211= Cyc_NewControlFlow_abstract_guard( env, _temp1107);
_LL1216: _temp1215= _temp1211.f1; goto _LL1214; _LL1214: _temp1213= _temp1211.f2;
goto _LL1212; _LL1212: Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1217=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1217->hd=( void*) _temp1215; _temp1217->tl=({ struct Cyc_List_List*
_temp1218=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1218->hd=( void*) _temp1213; _temp1218->tl= 0; _temp1218;}); _temp1217;}));
Cyc_NewControlFlow_cf_prepass( env, _temp1101); return;} _LL1048: return;
_LL1050: return; _LL1052:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp1219=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1219[ 0]=({ struct Cyc_Core_Impossible_struct _temp1220; _temp1220.tag= Cyc_Core_Impossible_tag;
_temp1220.f1=( struct _tagged_string)({ char* _temp1221=( char*)"cf_prepass: goto w/o destination";
struct _tagged_string _temp1222; _temp1222.curr= _temp1221; _temp1222.base=
_temp1221; _temp1222.last_plus_one= _temp1221 + 33; _temp1222;}); _temp1220;});
_temp1219;})); _LL1054: { struct Cyc_Absyn_Stmt* my_encloser=( Cyc_NewControlFlow_get_stmt_annot(
s))->encloser; struct Cyc_Absyn_Stmt* dest_encloser=( Cyc_NewControlFlow_get_stmt_annot((
struct Cyc_Absyn_Stmt*)({ struct Cyc_Absyn_Stmt* _temp1223= _temp1117; if(
_temp1223 == 0){ _throw( Null_Exception);} _temp1223;})))->encloser; while(
dest_encloser != my_encloser) { struct Cyc_Absyn_Stmt* next_encloser=( Cyc_NewControlFlow_get_stmt_annot(
my_encloser))->encloser; if( next_encloser == my_encloser){ Cyc_Tcutil_terr( s->loc,(
struct _tagged_string)({ char* _temp1224=( char*)"goto enters local scope or exception handler";
struct _tagged_string _temp1225; _temp1225.curr= _temp1224; _temp1225.base=
_temp1224; _temp1225.last_plus_one= _temp1224 + 45; _temp1225;})); break;}
my_encloser= next_encloser;} return;} _LL1056: { void* ae1= Cyc_NewControlFlow_abstract_exp_top(
env, _temp1135); void* _temp1228; void* _temp1230; struct _tuple0 _temp1226= Cyc_NewControlFlow_abstract_guard(
env, _temp1133); _LL1231: _temp1230= _temp1226.f1; goto _LL1229; _LL1229:
_temp1228= _temp1226.f2; goto _LL1227; _LL1227: { void* ae3= Cyc_NewControlFlow_abstract_exp_top(
env, _temp1127); Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1232=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1232->hd=( void*) ae1; _temp1232->tl=({ struct Cyc_List_List* _temp1233=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1233->hd=(
void*) _temp1230; _temp1233->tl=({ struct Cyc_List_List* _temp1234=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1234->hd=( void*) _temp1228;
_temp1234->tl=({ struct Cyc_List_List* _temp1235=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1235->hd=( void*) ae3; _temp1235->tl=
0; _temp1235;}); _temp1234;}); _temp1233;}); _temp1232;})); Cyc_NewControlFlow_cf_prepass(
env, _temp1121); return;}} _LL1058: { void* ae= Cyc_NewControlFlow_abstract_exp_top(
Cyc_NewControlFlow_use_it_env( env), _temp1139); struct Cyc_List_List* ael= Cyc_NewControlFlow_cf_prepass_scs(
env, _temp1137); Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1236=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1236->hd=( void*) ae; _temp1236->tl= ael; _temp1236;})); return;} _LL1060: {
void* _temp1239; void* _temp1241; struct Cyc_NewControlFlow_AbsSynOne _temp1237=
Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_use_exp_unordered( env,
_temp1143, 0)); _LL1242: _temp1241=( void*) _temp1237.inner_exp; goto _LL1240;
_LL1240: _temp1239=( void*) _temp1237.assigns; goto _LL1238; _LL1238: { void* ae=
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp1241, _temp1239);
Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List* _temp1243=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1243->hd=( void*)
ae; _temp1243->tl= 0; _temp1243;})); return;}} _LL1062: Cyc_NewControlFlow_add_var_root(
env, _temp1154); if( _temp1154->initializer != 0){ struct Cyc_NewControlFlow_AbsEnv
init_env= Cyc_NewControlFlow_change_lhs( env,({ struct Cyc_List_List* _temp1244=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1244->hd=(
void*) Cyc_CfAbsexp_mkLocalOp( _temp1154); _temp1244->tl= 0; _temp1244;})); void*
ae= Cyc_NewControlFlow_abstract_exp_top( init_env,( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1245= _temp1154->initializer; if( _temp1245 == 0){
_throw( Null_Exception);} _temp1245;})); Cyc_NewControlFlow_cf_set_absexps( s,({
struct Cyc_List_List* _temp1246=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1246->hd=( void*) ae; _temp1246->tl= 0; _temp1246;}));}
Cyc_NewControlFlow_cf_prepass( env, _temp1145); return; _LL1064: for( 0;
_temp1174 != 0; _temp1174=({ struct Cyc_List_List* _temp1247= _temp1174; if(
_temp1247 == 0){ _throw( Null_Exception);} _temp1247->tl;})){ Cyc_NewControlFlow_add_var_root(
env,( struct Cyc_Absyn_Vardecl*)({ struct Cyc_List_List* _temp1248= _temp1174;
if( _temp1248 == 0){ _throw( Null_Exception);} _temp1248->hd;}));}{ void* ae=
Cyc_NewControlFlow_abstract_exp_top( Cyc_NewControlFlow_use_it_env( env),
_temp1167); Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1249=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1249->hd=( void*) ae; _temp1249->tl= 0; _temp1249;})); Cyc_NewControlFlow_cf_prepass(
env, _temp1156); return;} _LL1066: goto _LL1068; _LL1068: goto _LL1070; _LL1070:(
void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp1250=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1250[ 0]=({ struct Cyc_Core_Impossible_struct _temp1251; _temp1251.tag= Cyc_Core_Impossible_tag;
_temp1251.f1=( struct _tagged_string)({ char* _temp1252=( char*)"cf_prepass: bad stmt form";
struct _tagged_string _temp1253; _temp1253.curr= _temp1252; _temp1253.base=
_temp1252; _temp1253.last_plus_one= _temp1252 + 26; _temp1253;}); _temp1251;});
_temp1250;})); _LL1072: Cyc_NewControlFlow_cf_prepass( env, _temp1186); return;
_LL1074: Cyc_NewControlFlow_cf_prepass( env, _temp1192);{ struct Cyc_List_List*
ael= Cyc_NewControlFlow_cf_prepass_scs( env, _temp1190); Cyc_NewControlFlow_cf_set_absexps(
s, ael); return;} _LL1076: Cyc_NewControlFlow_add_var_root( env, _temp1196);{
void* ae= Cyc_NewControlFlow_abstract_exp_top( env, Cyc_Absyn_uint_exp( 0, 0));
Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List* _temp1254=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1254->hd=( void*)
ae; _temp1254->tl= 0; _temp1254;})); Cyc_NewControlFlow_cf_prepass( env,
_temp1194); return;} _LL1030:;}} static int Cyc_NewControlFlow_iterate_cf_check=
0; static int Cyc_NewControlFlow_iteration_num= 0; struct Cyc_NewControlFlow_AnalEnv;
void Cyc_NewControlFlow_update_tryflow( struct Cyc_NewControlFlow_AnalEnv* env,
void* new_flow){ if( env->in_try){( void*)( env->tryflow=( void*) Cyc_CfFlowInfo_join_flow(
new_flow,( void*) env->tryflow));}} static struct Cyc_NewControlFlow_CFStmtAnnot*
Cyc_NewControlFlow_pre_stmt_check( void* in_flow, struct Cyc_Absyn_Stmt* s){
struct Cyc_NewControlFlow_CFStmtAnnot* annot= Cyc_NewControlFlow_get_stmt_annot(
s);( void*)( annot->flow=( void*) Cyc_CfFlowInfo_join_flow( in_flow,( void*)
annot->flow)); ++ annot->visited; annot->visited == Cyc_NewControlFlow_iteration_num?
0:(( int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp1255=( char*)"new_control_flow.cyc:943 annot->visited == iteration_num";
struct _tagged_string _temp1256; _temp1256.curr= _temp1255; _temp1256.base=
_temp1255; _temp1256.last_plus_one= _temp1255 + 57; _temp1256;})); return annot;}
static void Cyc_NewControlFlow_update_flow( struct Cyc_Absyn_Stmt* s, void* flow){
struct Cyc_NewControlFlow_CFStmtAnnot* annot= Cyc_NewControlFlow_get_stmt_annot(
s); void* new_flow= Cyc_CfFlowInfo_join_flow( flow,( void*) annot->flow); if( !
Cyc_CfFlowInfo_flow_lessthan_approx( new_flow,( void*) annot->flow)){( void*)(
annot->flow=( void*) new_flow); if( annot->visited == Cyc_NewControlFlow_iteration_num){
Cyc_NewControlFlow_iterate_cf_check= 1;}}} static void* Cyc_NewControlFlow_add_init_vars_flow(
struct Cyc_NewControlFlow_AnalEnv* env, void* flow, struct Cyc_List_List* vds){
void* _temp1257= flow; struct Cyc_Dict_Dict* _temp1263; _LL1259: if(( int)
_temp1257 == Cyc_CfFlowInfo_BottomFL){ goto _LL1260;} else{ goto _LL1261;}
_LL1261: if(( unsigned int) _temp1257 > 1u?(( struct _tunion_struct*) _temp1257)->tag
== Cyc_CfFlowInfo_InitsFL_tag: 0){ _LL1264: _temp1263=( struct Cyc_Dict_Dict*)((
struct Cyc_CfFlowInfo_InitsFL_struct*) _temp1257)->f1; goto _LL1262;} else{ goto
_LL1258;} _LL1260: return( void*) Cyc_CfFlowInfo_BottomFL; _LL1262: for( 0; vds
!= 0; vds=({ struct Cyc_List_List* _temp1265= vds; if( _temp1265 == 0){ _throw(
Null_Exception);} _temp1265->tl;})){ struct Cyc_CfFlowInfo_VarRoot_struct* root=({
struct Cyc_CfFlowInfo_VarRoot_struct* _temp1266=( struct Cyc_CfFlowInfo_VarRoot_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct)); _temp1266[ 0]=({
struct Cyc_CfFlowInfo_VarRoot_struct _temp1267; _temp1267.tag= Cyc_CfFlowInfo_VarRoot_tag;
_temp1267.f1=( struct Cyc_Absyn_Vardecl*)({ struct Cyc_List_List* _temp1268= vds;
if( _temp1268 == 0){ _throw( Null_Exception);} _temp1268->hd;}); _temp1267;});
_temp1266;}); void* pinfo= Cyc_CfFlowInfo_assign_unknown_dict(( void*) Cyc_CfFlowInfo_AllIL,((
void*(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( env->roots,(
void*) root)); _temp1263=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
void* key, void* data)) Cyc_Dict_insert)( _temp1263,( void*) root, pinfo);}
return( void*)({ struct Cyc_CfFlowInfo_InitsFL_struct* _temp1269=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp1269[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp1270; _temp1270.tag= Cyc_CfFlowInfo_InitsFL_tag;
_temp1270.f1= _temp1263; _temp1270;}); _temp1269;}); _LL1258:;} static void* Cyc_NewControlFlow_add_vardecls_flow(
struct Cyc_NewControlFlow_AnalEnv* env, void* flow, struct Cyc_List_List* vds){
void* _temp1271= flow; struct Cyc_Dict_Dict* _temp1277; _LL1273: if(( int)
_temp1271 == Cyc_CfFlowInfo_BottomFL){ goto _LL1274;} else{ goto _LL1275;}
_LL1275: if(( unsigned int) _temp1271 > 1u?(( struct _tunion_struct*) _temp1271)->tag
== Cyc_CfFlowInfo_InitsFL_tag: 0){ _LL1278: _temp1277=( struct Cyc_Dict_Dict*)((
struct Cyc_CfFlowInfo_InitsFL_struct*) _temp1271)->f1; goto _LL1276;} else{ goto
_LL1272;} _LL1274: return( void*) Cyc_CfFlowInfo_BottomFL; _LL1276: for( 0; vds
!= 0; vds=({ struct Cyc_List_List* _temp1279= vds; if( _temp1279 == 0){ _throw(
Null_Exception);} _temp1279->tl;})){ struct Cyc_CfFlowInfo_VarRoot_struct* root=({
struct Cyc_CfFlowInfo_VarRoot_struct* _temp1280=( struct Cyc_CfFlowInfo_VarRoot_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct)); _temp1280[ 0]=({
struct Cyc_CfFlowInfo_VarRoot_struct _temp1281; _temp1281.tag= Cyc_CfFlowInfo_VarRoot_tag;
_temp1281.f1=( struct Cyc_Absyn_Vardecl*)({ struct Cyc_List_List* _temp1282= vds;
if( _temp1282 == 0){ _throw( Null_Exception);} _temp1282->hd;}); _temp1281;});
_temp1280;}); _temp1277=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
void* key, void* data)) Cyc_Dict_insert)( _temp1277,( void*) root,(( void*(*)(
struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( env->roots,( void*) root));}
return( void*)({ struct Cyc_CfFlowInfo_InitsFL_struct* _temp1283=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp1283[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp1284; _temp1284.tag= Cyc_CfFlowInfo_InitsFL_tag;
_temp1284.f1= _temp1277; _temp1284;}); _temp1283;}); _LL1272:;} static struct
_tuple0 Cyc_NewControlFlow_cf_evalguard( struct Cyc_NewControlFlow_AnalEnv* env,
struct Cyc_Position_Segment* loc, struct Cyc_List_List* aes, void* in_flow){ if((
void*)({ struct Cyc_List_List* _temp1285= aes; if( _temp1285 == 0){ _throw(
Null_Exception);} _temp1285->hd;}) ==( void*)({ struct Cyc_List_List* _temp1287=({
struct Cyc_List_List* _temp1286= aes; if( _temp1286 == 0){ _throw(
Null_Exception);} _temp1286->tl;}); if( _temp1287 == 0){ _throw( Null_Exception);}
_temp1287->hd;})){ void* out_flow= Cyc_CfAbsexp_eval_absexp( env, loc,( void*)({
struct Cyc_List_List* _temp1288= aes; if( _temp1288 == 0){ _throw(
Null_Exception);} _temp1288->hd;}), in_flow); return({ struct _tuple0 _temp1289;
_temp1289.f1= out_flow; _temp1289.f2= out_flow; _temp1289;});} return({ struct
_tuple0 _temp1290; _temp1290.f1= Cyc_CfAbsexp_eval_absexp( env, loc,( void*)({
struct Cyc_List_List* _temp1293= aes; if( _temp1293 == 0){ _throw(
Null_Exception);} _temp1293->hd;}), in_flow); _temp1290.f2= Cyc_CfAbsexp_eval_absexp(
env, loc,( void*)({ struct Cyc_List_List* _temp1292=({ struct Cyc_List_List*
_temp1291= aes; if( _temp1291 == 0){ _throw( Null_Exception);} _temp1291->tl;});
if( _temp1292 == 0){ _throw( Null_Exception);} _temp1292->hd;}), in_flow);
_temp1290;});} static void Cyc_NewControlFlow_cf_analyze_switch_clauses( struct
Cyc_NewControlFlow_AnalEnv* env, struct Cyc_List_List* scs, struct Cyc_List_List*
aes, void* in_flow){ for( 0; scs != 0; scs=({ struct Cyc_List_List* _temp1294=
scs; if( _temp1294 == 0){ _throw( Null_Exception);} _temp1294->tl;})){ struct
Cyc_Absyn_Switch_clause _temp1298; struct Cyc_Position_Segment* _temp1299;
struct Cyc_Absyn_Stmt* _temp1301; struct Cyc_Absyn_Exp* _temp1303; struct Cyc_Core_Opt*
_temp1305; struct Cyc_Absyn_Pat* _temp1307; struct Cyc_Absyn_Switch_clause*
_temp1296=( struct Cyc_Absyn_Switch_clause*)({ struct Cyc_List_List* _temp1295=
scs; if( _temp1295 == 0){ _throw( Null_Exception);} _temp1295->hd;}); _temp1298=*
_temp1296; _LL1308: _temp1307=( struct Cyc_Absyn_Pat*) _temp1298.pattern; goto
_LL1306; _LL1306: _temp1305=( struct Cyc_Core_Opt*) _temp1298.pat_vars; goto
_LL1304; _LL1304: _temp1303=( struct Cyc_Absyn_Exp*) _temp1298.where_clause;
goto _LL1302; _LL1302: _temp1301=( struct Cyc_Absyn_Stmt*) _temp1298.body; goto
_LL1300; _LL1300: _temp1299=( struct Cyc_Position_Segment*) _temp1298.loc; goto
_LL1297; _LL1297: { void* body_inflow= Cyc_NewControlFlow_add_vardecls_flow( env,
in_flow,( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp1309= _temp1305;
if( _temp1309 == 0){ _throw( Null_Exception);} _temp1309->v;})); body_inflow=
Cyc_NewControlFlow_add_init_vars_flow( env, body_inflow,( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp1310= _temp1305; if( _temp1310 == 0){ _throw(
Null_Exception);} _temp1310->v;}));{ void* body_outflow; if( _temp1303 != 0){
void* _temp1314; void* _temp1316; struct _tuple0 _temp1312= Cyc_NewControlFlow_cf_evalguard(
env,(( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1311= _temp1303; if(
_temp1311 == 0){ _throw( Null_Exception);} _temp1311;}))->loc, aes, body_inflow);
_LL1317: _temp1316= _temp1312.f1; goto _LL1315; _LL1315: _temp1314= _temp1312.f2;
goto _LL1313; _LL1313: aes=({ struct Cyc_List_List* _temp1319=({ struct Cyc_List_List*
_temp1318= aes; if( _temp1318 == 0){ _throw( Null_Exception);} _temp1318->tl;});
if( _temp1319 == 0){ _throw( Null_Exception);} _temp1319->tl;}); in_flow=
_temp1314; body_outflow= Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1301,
_temp1316);} else{ body_outflow= Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp1301, body_inflow);}{ void* _temp1320= body_outflow; _LL1322: if(( int)
_temp1320 == Cyc_CfFlowInfo_BottomFL){ goto _LL1323;} else{ goto _LL1324;}
_LL1324: goto _LL1325; _LL1323: goto _LL1321; _LL1325: Cyc_Tcutil_terr(
_temp1301->loc,( struct _tagged_string)({ char* _temp1326=( char*)"switch clause may implicitly fallthru";
struct _tagged_string _temp1327; _temp1327.curr= _temp1326; _temp1327.base=
_temp1326; _temp1327.last_plus_one= _temp1326 + 38; _temp1327;})); goto _LL1321;
_LL1321:;}}}}} void* Cyc_NewControlFlow_cf_analyze_stmt( struct Cyc_NewControlFlow_AnalEnv*
env, struct Cyc_Absyn_Stmt* s, void* in_flow){ struct Cyc_NewControlFlow_CFStmtAnnot*
annot= Cyc_NewControlFlow_pre_stmt_check( in_flow, s); in_flow=( void*) annot->flow;{
struct Cyc_List_List* aes= annot->absexps; void* _temp1328=( void*) s->r; struct
Cyc_Absyn_Exp* _temp1372; struct Cyc_Absyn_Exp* _temp1374; struct Cyc_Absyn_Exp*
_temp1376; struct Cyc_Absyn_Stmt* _temp1378; struct Cyc_Absyn_Stmt* _temp1380;
struct Cyc_Absyn_Stmt* _temp1382; struct Cyc_Absyn_Stmt* _temp1384; struct Cyc_Absyn_Exp*
_temp1386; struct Cyc_Absyn_Stmt* _temp1388; struct _tuple3 _temp1390; struct
Cyc_Absyn_Stmt* _temp1392; struct Cyc_Absyn_Exp* _temp1394; struct _tuple3
_temp1396; struct Cyc_Absyn_Stmt* _temp1398; struct Cyc_Absyn_Exp* _temp1400;
struct Cyc_Absyn_Stmt* _temp1402; struct Cyc_Absyn_Stmt* _temp1404; struct
_tuple3 _temp1406; struct Cyc_Absyn_Stmt* _temp1408; struct Cyc_Absyn_Exp*
_temp1410; struct _tuple3 _temp1412; struct Cyc_Absyn_Stmt* _temp1414; struct
Cyc_Absyn_Exp* _temp1416; struct Cyc_Absyn_Exp* _temp1418; struct Cyc_Absyn_Stmt*
_temp1420; struct Cyc_Absyn_Switch_clause** _temp1422; struct Cyc_Absyn_Switch_clause*
_temp1424; struct Cyc_List_List* _temp1425; struct Cyc_Absyn_Stmt* _temp1427;
struct Cyc_Absyn_Stmt* _temp1429; struct Cyc_Absyn_Stmt* _temp1431; struct
_tagged_string* _temp1433; struct Cyc_List_List* _temp1435; struct Cyc_Absyn_Exp*
_temp1437; struct Cyc_List_List* _temp1439; struct Cyc_Absyn_Stmt* _temp1441;
struct Cyc_Absyn_Stmt* _temp1443; struct Cyc_Absyn_Decl* _temp1445; struct Cyc_Absyn_Decl
_temp1447; struct Cyc_Position_Segment* _temp1448; void* _temp1450; struct Cyc_Absyn_Vardecl*
_temp1452; struct Cyc_Absyn_Stmt* _temp1454; struct Cyc_Absyn_Decl* _temp1456;
struct Cyc_Absyn_Decl _temp1458; struct Cyc_Position_Segment* _temp1459; void*
_temp1461; int _temp1463; struct Cyc_Absyn_Exp* _temp1465; struct Cyc_Core_Opt*
_temp1467; struct Cyc_Core_Opt* _temp1469; struct Cyc_Core_Opt _temp1471; struct
Cyc_List_List* _temp1472; struct Cyc_Absyn_Pat* _temp1474; struct Cyc_Absyn_Stmt*
_temp1476; struct _tagged_string* _temp1478; struct Cyc_Absyn_Stmt* _temp1480;
struct Cyc_Absyn_Vardecl* _temp1482; struct Cyc_Absyn_Tvar* _temp1484; _LL1330:
if(( int) _temp1328 == Cyc_Absyn_Skip_s){ goto _LL1331;} else{ goto _LL1332;}
_LL1332: if(( unsigned int) _temp1328 > 1u?(( struct _tunion_struct*) _temp1328)->tag
== Cyc_Absyn_Return_s_tag: 0){ _LL1373: _temp1372=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Return_s_struct*) _temp1328)->f1; if( _temp1372 == 0){ goto
_LL1333;} else{ goto _LL1334;}} else{ goto _LL1334;} _LL1334: if(( unsigned int)
_temp1328 > 1u?(( struct _tunion_struct*) _temp1328)->tag == Cyc_Absyn_Return_s_tag:
0){ _LL1375: _temp1374=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp1328)->f1; goto _LL1335;} else{ goto _LL1336;} _LL1336: if(( unsigned int)
_temp1328 > 1u?(( struct _tunion_struct*) _temp1328)->tag == Cyc_Absyn_Exp_s_tag:
0){ _LL1377: _temp1376=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp1328)->f1; goto _LL1337;} else{ goto _LL1338;} _LL1338: if(( unsigned int)
_temp1328 > 1u?(( struct _tunion_struct*) _temp1328)->tag == Cyc_Absyn_Seq_s_tag:
0){ _LL1381: _temp1380=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp1328)->f1; goto _LL1379; _LL1379: _temp1378=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp1328)->f2; goto _LL1339;} else{ goto
_LL1340;} _LL1340: if(( unsigned int) _temp1328 > 1u?(( struct _tunion_struct*)
_temp1328)->tag == Cyc_Absyn_IfThenElse_s_tag: 0){ _LL1387: _temp1386=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp1328)->f1; goto
_LL1385; _LL1385: _temp1384=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1328)->f2; goto _LL1383; _LL1383: _temp1382=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1328)->f3; goto _LL1341;} else{ goto
_LL1342;} _LL1342: if(( unsigned int) _temp1328 > 1u?(( struct _tunion_struct*)
_temp1328)->tag == Cyc_Absyn_While_s_tag: 0){ _LL1391: _temp1390=( struct
_tuple3)(( struct Cyc_Absyn_While_s_struct*) _temp1328)->f1; _LL1395: _temp1394=
_temp1390.f1; goto _LL1393; _LL1393: _temp1392= _temp1390.f2; goto _LL1389;
_LL1389: _temp1388=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_While_s_struct*)
_temp1328)->f2; goto _LL1343;} else{ goto _LL1344;} _LL1344: if(( unsigned int)
_temp1328 > 1u?(( struct _tunion_struct*) _temp1328)->tag == Cyc_Absyn_Do_s_tag:
0){ _LL1403: _temp1402=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*)
_temp1328)->f1; goto _LL1397; _LL1397: _temp1396=( struct _tuple3)(( struct Cyc_Absyn_Do_s_struct*)
_temp1328)->f2; _LL1401: _temp1400= _temp1396.f1; goto _LL1399; _LL1399:
_temp1398= _temp1396.f2; goto _LL1345;} else{ goto _LL1346;} _LL1346: if((
unsigned int) _temp1328 > 1u?(( struct _tunion_struct*) _temp1328)->tag == Cyc_Absyn_For_s_tag:
0){ _LL1419: _temp1418=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp1328)->f1; goto _LL1413; _LL1413: _temp1412=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp1328)->f2; _LL1417: _temp1416= _temp1412.f1; goto _LL1415; _LL1415:
_temp1414= _temp1412.f2; goto _LL1407; _LL1407: _temp1406=( struct _tuple3)((
struct Cyc_Absyn_For_s_struct*) _temp1328)->f3; _LL1411: _temp1410= _temp1406.f1;
goto _LL1409; _LL1409: _temp1408= _temp1406.f2; goto _LL1405; _LL1405: _temp1404=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp1328)->f4; goto
_LL1347;} else{ goto _LL1348;} _LL1348: if(( unsigned int) _temp1328 > 1u?((
struct _tunion_struct*) _temp1328)->tag == Cyc_Absyn_Break_s_tag: 0){ _LL1421:
_temp1420=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Break_s_struct*)
_temp1328)->f1; if( _temp1420 == 0){ goto _LL1349;} else{ goto _LL1350;}} else{
goto _LL1350;} _LL1350: if(( unsigned int) _temp1328 > 1u?(( struct
_tunion_struct*) _temp1328)->tag == Cyc_Absyn_Fallthru_s_tag: 0){ _LL1426:
_temp1425=( struct Cyc_List_List*)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1328)->f1; goto _LL1423; _LL1423: _temp1422=( struct Cyc_Absyn_Switch_clause**)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1328)->f2; if( _temp1422 == 0){ goto
_LL1352;} else{ _temp1424=* _temp1422; goto _LL1351;}} else{ goto _LL1352;}
_LL1352: if(( unsigned int) _temp1328 > 1u?(( struct _tunion_struct*) _temp1328)->tag
== Cyc_Absyn_Break_s_tag: 0){ _LL1428: _temp1427=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Break_s_struct*) _temp1328)->f1; goto _LL1353;} else{ goto
_LL1354;} _LL1354: if(( unsigned int) _temp1328 > 1u?(( struct _tunion_struct*)
_temp1328)->tag == Cyc_Absyn_Continue_s_tag: 0){ _LL1430: _temp1429=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Continue_s_struct*) _temp1328)->f1; goto _LL1355;} else{ goto
_LL1356;} _LL1356: if(( unsigned int) _temp1328 > 1u?(( struct _tunion_struct*)
_temp1328)->tag == Cyc_Absyn_Goto_s_tag: 0){ _LL1434: _temp1433=( struct
_tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp1328)->f1; goto _LL1432;
_LL1432: _temp1431=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp1328)->f2; goto _LL1357;} else{ goto _LL1358;} _LL1358: if(( unsigned int)
_temp1328 > 1u?(( struct _tunion_struct*) _temp1328)->tag == Cyc_Absyn_Switch_s_tag:
0){ _LL1438: _temp1437=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp1328)->f1; goto _LL1436; _LL1436: _temp1435=( struct Cyc_List_List*)((
struct Cyc_Absyn_Switch_s_struct*) _temp1328)->f2; goto _LL1359;} else{ goto
_LL1360;} _LL1360: if(( unsigned int) _temp1328 > 1u?(( struct _tunion_struct*)
_temp1328)->tag == Cyc_Absyn_TryCatch_s_tag: 0){ _LL1442: _temp1441=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp1328)->f1; goto _LL1440; _LL1440:
_temp1439=( struct Cyc_List_List*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1328)->f2; goto _LL1361;} else{ goto _LL1362;} _LL1362: if(( unsigned int)
_temp1328 > 1u?(( struct _tunion_struct*) _temp1328)->tag == Cyc_Absyn_Decl_s_tag:
0){ _LL1446: _temp1445=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1328)->f1; _temp1447=* _temp1445; _LL1451: _temp1450=( void*) _temp1447.r;
if((( struct _tunion_struct*) _temp1450)->tag == Cyc_Absyn_Var_d_tag){ _LL1453:
_temp1452=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp1450)->f1; goto _LL1449;} else{ goto _LL1364;} _LL1449: _temp1448=( struct
Cyc_Position_Segment*) _temp1447.loc; goto _LL1444; _LL1444: _temp1443=( struct
Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1328)->f2; goto _LL1363;}
else{ goto _LL1364;} _LL1364: if(( unsigned int) _temp1328 > 1u?(( struct
_tunion_struct*) _temp1328)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL1457: _temp1456=(
struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1328)->f1;
_temp1458=* _temp1456; _LL1462: _temp1461=( void*) _temp1458.r; if((( struct
_tunion_struct*) _temp1461)->tag == Cyc_Absyn_Let_d_tag){ _LL1475: _temp1474=(
struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*) _temp1461)->f1; goto
_LL1470; _LL1470: _temp1469=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1461)->f2; if( _temp1469 == 0){ goto _LL1366;} else{ _temp1471=* _temp1469;
_LL1473: _temp1472=( struct Cyc_List_List*) _temp1471.v; goto _LL1468;} _LL1468:
_temp1467=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp1461)->f3;
goto _LL1466; _LL1466: _temp1465=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1461)->f4; goto _LL1464; _LL1464: _temp1463=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp1461)->f5; goto _LL1460;} else{ goto _LL1366;} _LL1460: _temp1459=( struct
Cyc_Position_Segment*) _temp1458.loc; goto _LL1455; _LL1455: _temp1454=( struct
Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1328)->f2; goto _LL1365;}
else{ goto _LL1366;} _LL1366: if(( unsigned int) _temp1328 > 1u?(( struct
_tunion_struct*) _temp1328)->tag == Cyc_Absyn_Label_s_tag: 0){ _LL1479:
_temp1478=( struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*)
_temp1328)->f1; goto _LL1477; _LL1477: _temp1476=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Label_s_struct*) _temp1328)->f2; goto _LL1367;} else{ goto
_LL1368;} _LL1368: if(( unsigned int) _temp1328 > 1u?(( struct _tunion_struct*)
_temp1328)->tag == Cyc_Absyn_Region_s_tag: 0){ _LL1485: _temp1484=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_Region_s_struct*) _temp1328)->f1; goto _LL1483; _LL1483:
_temp1482=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Region_s_struct*)
_temp1328)->f2; goto _LL1481; _LL1481: _temp1480=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Region_s_struct*) _temp1328)->f3; goto _LL1369;} else{ goto
_LL1370;} _LL1370: goto _LL1371; _LL1331: return in_flow; _LL1333: return( void*)
Cyc_CfFlowInfo_BottomFL; _LL1335: Cyc_CfAbsexp_eval_absexp( env,({ struct Cyc_Absyn_Exp*
_temp1486= _temp1374; if( _temp1486 == 0){ _throw( Null_Exception);} _temp1486->loc;}),(
void*)({ struct Cyc_List_List* _temp1487= aes; if( _temp1487 == 0){ _throw(
Null_Exception);} _temp1487->hd;}), in_flow); return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1337: return Cyc_CfAbsexp_eval_absexp( env, _temp1376->loc,( void*)({ struct
Cyc_List_List* _temp1488= aes; if( _temp1488 == 0){ _throw( Null_Exception);}
_temp1488->hd;}), in_flow); _LL1339: return Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1378, Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1380, in_flow));
_LL1341: { void* _temp1491; void* _temp1493; struct _tuple0 _temp1489= Cyc_NewControlFlow_cf_evalguard(
env, _temp1386->loc, aes, in_flow); _LL1494: _temp1493= _temp1489.f1; goto
_LL1492; _LL1492: _temp1491= _temp1489.f2; goto _LL1490; _LL1490: return Cyc_CfFlowInfo_join_flow(
Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1384, _temp1493), Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1382, _temp1491));} _LL1343: { void* e_inflow=( void*)( Cyc_NewControlFlow_pre_stmt_check(
in_flow, _temp1392))->flow; void* _temp1497; void* _temp1499; struct _tuple0
_temp1495= Cyc_NewControlFlow_cf_evalguard( env, _temp1394->loc, aes, e_inflow);
_LL1500: _temp1499= _temp1495.f1; goto _LL1498; _LL1498: _temp1497= _temp1495.f2;
goto _LL1496; _LL1496: { void* body_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1388, _temp1499); Cyc_NewControlFlow_update_flow( _temp1392,
body_outflow); return _temp1497;}} _LL1345: { void* body_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1402, in_flow); void* e_inflow=( void*)( Cyc_NewControlFlow_pre_stmt_check(
body_outflow, _temp1398))->flow; void* _temp1503; void* _temp1505; struct
_tuple0 _temp1501= Cyc_NewControlFlow_cf_evalguard( env, _temp1400->loc, aes,
e_inflow); _LL1506: _temp1505= _temp1501.f1; goto _LL1504; _LL1504: _temp1503=
_temp1501.f2; goto _LL1502; _LL1502: Cyc_NewControlFlow_update_flow( _temp1402,
_temp1505); return _temp1503;} _LL1347: { void* e1_outflow= Cyc_CfAbsexp_eval_absexp(
env, _temp1418->loc,( void*)({ struct Cyc_List_List* _temp1507= aes; if(
_temp1507 == 0){ _throw( Null_Exception);} _temp1507->hd;}), in_flow); void*
e2_inflow=( void*)( Cyc_NewControlFlow_pre_stmt_check( e1_outflow, _temp1414))->flow;
void* _temp1511; void* _temp1513; struct _tuple0 _temp1509= Cyc_NewControlFlow_cf_evalguard(
env, _temp1416->loc,({ struct Cyc_List_List* _temp1508= aes; if( _temp1508 == 0){
_throw( Null_Exception);} _temp1508->tl;}), e2_inflow); _LL1514: _temp1513=
_temp1509.f1; goto _LL1512; _LL1512: _temp1511= _temp1509.f2; goto _LL1510;
_LL1510: { void* body_outflow= Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp1404, _temp1513); void* e3_inflow=( void*)( Cyc_NewControlFlow_pre_stmt_check(
body_outflow, _temp1408))->flow; void* e3_outflow= Cyc_CfAbsexp_eval_absexp( env,
_temp1410->loc,( void*)({ struct Cyc_List_List* _temp1518=({ struct Cyc_List_List*
_temp1517=({ struct Cyc_List_List* _temp1516=({ struct Cyc_List_List* _temp1515=
aes; if( _temp1515 == 0){ _throw( Null_Exception);} _temp1515->tl;}); if(
_temp1516 == 0){ _throw( Null_Exception);} _temp1516->tl;}); if( _temp1517 == 0){
_throw( Null_Exception);} _temp1517->tl;}); if( _temp1518 == 0){ _throw(
Null_Exception);} _temp1518->hd;}), e3_inflow); Cyc_NewControlFlow_update_flow(
_temp1414, e3_outflow); return _temp1511;}} _LL1349: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1351: in_flow= Cyc_CfAbsexp_eval_absexp( env, s->loc,( void*)({ struct Cyc_List_List*
_temp1519= aes; if( _temp1519 == 0){ _throw( Null_Exception);} _temp1519->hd;}),
in_flow);{ struct Cyc_List_List* ftvars=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp1520= _temp1424->pat_vars; if( _temp1520 == 0){ _throw( Null_Exception);}
_temp1520->v;}); in_flow= Cyc_NewControlFlow_add_vardecls_flow( env, in_flow,
ftvars); in_flow= Cyc_NewControlFlow_add_init_vars_flow( env, in_flow, ftvars);
_temp1427=( struct Cyc_Absyn_Stmt*) _temp1424->body; goto _LL1353;} _LL1353:
_temp1429= _temp1427; goto _LL1355; _LL1355: _temp1431= _temp1429; goto _LL1357;
_LL1357: Cyc_NewControlFlow_update_flow(( struct Cyc_Absyn_Stmt*)({ struct Cyc_Absyn_Stmt*
_temp1521= _temp1431; if( _temp1521 == 0){ _throw( Null_Exception);} _temp1521;}),
in_flow); return( void*) Cyc_CfFlowInfo_BottomFL; _LL1359: in_flow= Cyc_CfAbsexp_eval_absexp(
env, _temp1437->loc,( void*)({ struct Cyc_List_List* _temp1522= aes; if(
_temp1522 == 0){ _throw( Null_Exception);} _temp1522->hd;}), in_flow); Cyc_NewControlFlow_cf_analyze_switch_clauses(
env, _temp1435,({ struct Cyc_List_List* _temp1523= aes; if( _temp1523 == 0){
_throw( Null_Exception);} _temp1523->tl;}), in_flow); return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1361: { struct Cyc_NewControlFlow_AnalEnv _temp1526; void* _temp1527; int
_temp1529; struct Cyc_Dict_Dict* _temp1531; struct Cyc_NewControlFlow_AnalEnv*
_temp1524= env; _temp1526=* _temp1524; _LL1532: _temp1531=( struct Cyc_Dict_Dict*)
_temp1526.roots; goto _LL1530; _LL1530: _temp1529=( int) _temp1526.in_try; goto
_LL1528; _LL1528: _temp1527=( void*) _temp1526.tryflow; goto _LL1525; _LL1525:
env->in_try= 1;( void*)( env->tryflow=( void*) in_flow);{ void* s_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1441, in_flow); void* scs_inflow=( void*) env->tryflow; env->in_try=
_temp1529;( void*)( env->tryflow=( void*) _temp1527); Cyc_NewControlFlow_update_tryflow(
env, scs_inflow); Cyc_NewControlFlow_cf_analyze_switch_clauses( env, _temp1439,
aes, scs_inflow); return s_outflow;}} _LL1363: { struct Cyc_List_List l=({
struct Cyc_List_List _temp1533; _temp1533.hd=( void*) _temp1452; _temp1533.tl= 0;
_temp1533;}); void* body_inflow= Cyc_NewControlFlow_add_vardecls_flow( env,
in_flow,( struct Cyc_List_List*)& l); struct Cyc_Absyn_Exp* e= _temp1452->initializer;
if( e != 0){ body_inflow= Cyc_CfAbsexp_eval_absexp( env,(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1534= e; if( _temp1534 == 0){ _throw( Null_Exception);}
_temp1534;}))->loc,( void*)({ struct Cyc_List_List* _temp1535= aes; if(
_temp1535 == 0){ _throw( Null_Exception);} _temp1535->hd;}), body_inflow);}
return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1443, body_inflow);}
_LL1365: { void* body_inflow= Cyc_NewControlFlow_add_vardecls_flow( env, in_flow,
_temp1472); body_inflow= Cyc_CfAbsexp_eval_absexp( env, _temp1465->loc,( void*)({
struct Cyc_List_List* _temp1536= aes; if( _temp1536 == 0){ _throw(
Null_Exception);} _temp1536->hd;}), body_inflow); body_inflow= Cyc_NewControlFlow_add_init_vars_flow(
env, body_inflow, _temp1472); return Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp1454, body_inflow);} _LL1367: return Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1476, in_flow); _LL1369: { struct Cyc_List_List* vds=({ struct Cyc_List_List*
_temp1537=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1537->hd=( void*) _temp1482; _temp1537->tl= 0; _temp1537;}); void*
body_inflow= Cyc_NewControlFlow_add_vardecls_flow( env, in_flow, vds);
body_inflow= Cyc_NewControlFlow_add_init_vars_flow( env, body_inflow, vds);
return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1480, body_inflow);}
_LL1371:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp1538=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1538[ 0]=({ struct Cyc_Core_Impossible_struct _temp1539; _temp1539.tag= Cyc_Core_Impossible_tag;
_temp1539.f1=( struct _tagged_string)({ char* _temp1540=( char*)"cf_analyze_stmt -- bad stmt syntax or unimplemented stmt form";
struct _tagged_string _temp1541; _temp1541.curr= _temp1540; _temp1541.base=
_temp1540; _temp1541.last_plus_one= _temp1540 + 62; _temp1541;}); _temp1539;});
_temp1538;})); _LL1329:;}} void Cyc_NewControlFlow_cf_postpass( struct Cyc_Absyn_Stmt*
s){ s->annot=( struct _xtunion_struct*)({ struct Cyc_Absyn_EmptyAnnot_struct*
_temp1542=( struct Cyc_Absyn_EmptyAnnot_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EmptyAnnot_struct));
_temp1542[ 0]=({ struct Cyc_Absyn_EmptyAnnot_struct _temp1543; _temp1543.tag=
Cyc_Absyn_EmptyAnnot_tag; _temp1543;}); _temp1542;});{ void* _temp1544=( void*)
s->r; struct Cyc_Absyn_Stmt* _temp1568; struct Cyc_Absyn_Stmt* _temp1570; struct
Cyc_Absyn_Stmt* _temp1572; struct Cyc_Absyn_Stmt* _temp1574; struct Cyc_Absyn_Exp*
_temp1576; struct Cyc_Absyn_Stmt* _temp1578; struct _tuple3 _temp1580; struct
Cyc_Absyn_Stmt* _temp1582; struct Cyc_Absyn_Exp* _temp1584; struct _tuple3
_temp1586; struct Cyc_Absyn_Stmt* _temp1588; struct Cyc_Absyn_Exp* _temp1590;
struct Cyc_Absyn_Exp* _temp1592; struct Cyc_Absyn_Stmt* _temp1594; struct
_tuple3 _temp1596; struct Cyc_Absyn_Stmt* _temp1598; struct Cyc_Absyn_Exp*
_temp1600; struct _tuple3 _temp1602; struct Cyc_Absyn_Stmt* _temp1604; struct
Cyc_Absyn_Exp* _temp1606; struct Cyc_Absyn_Stmt* _temp1608; struct Cyc_Absyn_Stmt*
_temp1610; struct Cyc_Absyn_Vardecl* _temp1612; struct Cyc_Absyn_Tvar* _temp1614;
struct Cyc_Absyn_Stmt* _temp1616; struct _tagged_string* _temp1618; struct Cyc_Absyn_Stmt*
_temp1620; struct Cyc_Absyn_Decl* _temp1622; struct Cyc_List_List* _temp1624;
struct Cyc_Absyn_Stmt* _temp1626; struct Cyc_List_List* _temp1628; struct Cyc_Absyn_Exp*
_temp1630; _LL1546: if(( unsigned int) _temp1544 > 1u?(( struct _tunion_struct*)
_temp1544)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL1571: _temp1570=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp1544)->f1; goto _LL1569; _LL1569: _temp1568=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1544)->f2; goto
_LL1547;} else{ goto _LL1548;} _LL1548: if(( unsigned int) _temp1544 > 1u?((
struct _tunion_struct*) _temp1544)->tag == Cyc_Absyn_IfThenElse_s_tag: 0){
_LL1577: _temp1576=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1544)->f1; goto _LL1575; _LL1575: _temp1574=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1544)->f2; goto _LL1573; _LL1573:
_temp1572=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1544)->f3; goto _LL1549;} else{ goto _LL1550;} _LL1550: if(( unsigned int)
_temp1544 > 1u?(( struct _tunion_struct*) _temp1544)->tag == Cyc_Absyn_For_s_tag:
0){ _LL1593: _temp1592=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp1544)->f1; goto _LL1587; _LL1587: _temp1586=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp1544)->f2; _LL1591: _temp1590= _temp1586.f1; goto _LL1589; _LL1589:
_temp1588= _temp1586.f2; goto _LL1581; _LL1581: _temp1580=( struct _tuple3)((
struct Cyc_Absyn_For_s_struct*) _temp1544)->f3; _LL1585: _temp1584= _temp1580.f1;
goto _LL1583; _LL1583: _temp1582= _temp1580.f2; goto _LL1579; _LL1579: _temp1578=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp1544)->f4; goto
_LL1551;} else{ goto _LL1552;} _LL1552: if(( unsigned int) _temp1544 > 1u?((
struct _tunion_struct*) _temp1544)->tag == Cyc_Absyn_While_s_tag: 0){ _LL1597:
_temp1596=( struct _tuple3)(( struct Cyc_Absyn_While_s_struct*) _temp1544)->f1;
_LL1601: _temp1600= _temp1596.f1; goto _LL1599; _LL1599: _temp1598= _temp1596.f2;
goto _LL1595; _LL1595: _temp1594=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_While_s_struct*)
_temp1544)->f2; goto _LL1553;} else{ goto _LL1554;} _LL1554: if(( unsigned int)
_temp1544 > 1u?(( struct _tunion_struct*) _temp1544)->tag == Cyc_Absyn_Do_s_tag:
0){ _LL1609: _temp1608=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*)
_temp1544)->f1; goto _LL1603; _LL1603: _temp1602=( struct _tuple3)(( struct Cyc_Absyn_Do_s_struct*)
_temp1544)->f2; _LL1607: _temp1606= _temp1602.f1; goto _LL1605; _LL1605:
_temp1604= _temp1602.f2; goto _LL1555;} else{ goto _LL1556;} _LL1556: if((
unsigned int) _temp1544 > 1u?(( struct _tunion_struct*) _temp1544)->tag == Cyc_Absyn_Region_s_tag:
0){ _LL1615: _temp1614=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_Region_s_struct*)
_temp1544)->f1; goto _LL1613; _LL1613: _temp1612=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Region_s_struct*) _temp1544)->f2; goto _LL1611; _LL1611:
_temp1610=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Region_s_struct*)
_temp1544)->f3; goto _LL1557;} else{ goto _LL1558;} _LL1558: if(( unsigned int)
_temp1544 > 1u?(( struct _tunion_struct*) _temp1544)->tag == Cyc_Absyn_Label_s_tag:
0){ _LL1619: _temp1618=( struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*)
_temp1544)->f1; goto _LL1617; _LL1617: _temp1616=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Label_s_struct*) _temp1544)->f2; goto _LL1559;} else{ goto
_LL1560;} _LL1560: if(( unsigned int) _temp1544 > 1u?(( struct _tunion_struct*)
_temp1544)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL1623: _temp1622=( struct Cyc_Absyn_Decl*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1544)->f1; goto _LL1621; _LL1621:
_temp1620=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1544)->f2;
goto _LL1561;} else{ goto _LL1562;} _LL1562: if(( unsigned int) _temp1544 > 1u?((
struct _tunion_struct*) _temp1544)->tag == Cyc_Absyn_TryCatch_s_tag: 0){ _LL1627:
_temp1626=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1544)->f1; goto _LL1625; _LL1625: _temp1624=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp1544)->f2; goto _LL1563;} else{ goto
_LL1564;} _LL1564: if(( unsigned int) _temp1544 > 1u?(( struct _tunion_struct*)
_temp1544)->tag == Cyc_Absyn_Switch_s_tag: 0){ _LL1631: _temp1630=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Switch_s_struct*) _temp1544)->f1; goto _LL1629; _LL1629:
_temp1628=( struct Cyc_List_List*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp1544)->f2; goto _LL1565;} else{ goto _LL1566;} _LL1566: goto _LL1567;
_LL1547: _temp1574= _temp1570; _temp1572= _temp1568; goto _LL1549; _LL1549: Cyc_NewControlFlow_cf_postpass(
_temp1574); Cyc_NewControlFlow_cf_postpass( _temp1572); return; _LL1551: Cyc_NewControlFlow_cf_postpass(
_temp1582); _temp1598= _temp1578; _temp1594= _temp1588; goto _LL1553; _LL1553:
_temp1608= _temp1594; _temp1604= _temp1598; goto _LL1555; _LL1555: Cyc_NewControlFlow_cf_postpass(
_temp1604); _temp1610= _temp1608; goto _LL1557; _LL1557: _temp1616= _temp1610;
goto _LL1559; _LL1559: _temp1620= _temp1616; goto _LL1561; _LL1561: Cyc_NewControlFlow_cf_postpass(
_temp1620); return; _LL1563: Cyc_NewControlFlow_cf_postpass( _temp1626);
_temp1628= _temp1624; goto _LL1565; _LL1565: for( 0; _temp1628 != 0; _temp1628=({
struct Cyc_List_List* _temp1632= _temp1628; if( _temp1632 == 0){ _throw(
Null_Exception);} _temp1632->tl;})){ Cyc_NewControlFlow_cf_postpass((( struct
Cyc_Absyn_Switch_clause*)({ struct Cyc_List_List* _temp1633= _temp1628; if(
_temp1633 == 0){ _throw( Null_Exception);} _temp1633->hd;}))->body);} return;
_LL1567: return; _LL1545:;}} void Cyc_NewControlFlow_cf_check_fun( struct Cyc_Absyn_Fndecl*
fd){ struct Cyc_NewControlFlow_AbsEnv absenv=({ struct Cyc_NewControlFlow_AbsEnv
_temp1634; _temp1634.shared=({ struct Cyc_NewControlFlow_SharedAbsEnv* _temp1635=(
struct Cyc_NewControlFlow_SharedAbsEnv*) GC_malloc( sizeof( struct Cyc_NewControlFlow_SharedAbsEnv));
_temp1635->all_roots=(( struct Cyc_Dict_Dict*(*)( int(* comp)( void*, void*)))
Cyc_Dict_empty)( Cyc_CfFlowInfo_local_root_cmp); _temp1635->comprehension_vars=
0; _temp1635;}); _temp1634.lhsides= 0; _temp1634.access_path= 0; _temp1634;});
struct Cyc_List_List* arg_vds=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp1636= fd->param_vardecls; if( _temp1636 == 0){ _throw( Null_Exception);}
_temp1636->v;});{ struct Cyc_List_List* vds= arg_vds; for( 0; vds != 0; vds=({
struct Cyc_List_List* _temp1637= vds; if( _temp1637 == 0){ _throw(
Null_Exception);} _temp1637->tl;})){ Cyc_NewControlFlow_add_var_root( absenv,(
struct Cyc_Absyn_Vardecl*)({ struct Cyc_List_List* _temp1638= vds; if( _temp1638
== 0){ _throw( Null_Exception);} _temp1638->hd;}));}} Cyc_NewControlFlow_cf_prepass(
absenv, fd->body);{ struct Cyc_List_List* _temp1641; struct Cyc_List_List*
_temp1643; struct Cyc_NewControlFlow_SharedAbsEnv* _temp1645; struct Cyc_NewControlFlow_SharedAbsEnv
_temp1647; struct Cyc_List_List* _temp1648; struct Cyc_Dict_Dict* _temp1650;
struct Cyc_NewControlFlow_AbsEnv _temp1639= absenv; _LL1646: _temp1645=( struct
Cyc_NewControlFlow_SharedAbsEnv*) _temp1639.shared; _temp1647=* _temp1645;
_LL1651: _temp1650=( struct Cyc_Dict_Dict*) _temp1647.all_roots; goto _LL1649;
_LL1649: _temp1648=( struct Cyc_List_List*) _temp1647.comprehension_vars; goto
_LL1644; _LL1644: _temp1643=( struct Cyc_List_List*) _temp1639.lhsides; goto
_LL1642; _LL1642: _temp1641=( struct Cyc_List_List*) _temp1639.access_path; goto
_LL1640; _LL1640: { struct Cyc_NewControlFlow_AnalEnv* analenv=({ struct Cyc_NewControlFlow_AnalEnv*
_temp1652=( struct Cyc_NewControlFlow_AnalEnv*) GC_malloc( sizeof( struct Cyc_NewControlFlow_AnalEnv));
_temp1652->roots= _temp1650; _temp1652->in_try= 0; _temp1652->tryflow=( void*)((
void*) Cyc_CfFlowInfo_BottomFL); _temp1652;}); void* in_flow=( void*)({ struct
Cyc_CfFlowInfo_InitsFL_struct* _temp1653=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp1653[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp1654; _temp1654.tag= Cyc_CfFlowInfo_InitsFL_tag;
_temp1654.f1=(( struct Cyc_Dict_Dict*(*)( int(* comp)( void*, void*))) Cyc_Dict_empty)(
Cyc_CfFlowInfo_local_root_cmp); _temp1654;}); _temp1653;}); in_flow= Cyc_NewControlFlow_add_init_vars_flow(
analenv, in_flow, arg_vds); in_flow= Cyc_NewControlFlow_add_vardecls_flow(
analenv, in_flow, _temp1648);{ void* out_flow= in_flow; Cyc_NewControlFlow_iteration_num=
0; Cyc_NewControlFlow_iterate_cf_check= 1; while( Cyc_NewControlFlow_iterate_cf_check) {
++ Cyc_NewControlFlow_iteration_num; Cyc_NewControlFlow_iterate_cf_check= 0;
out_flow= Cyc_NewControlFlow_cf_analyze_stmt( analenv, fd->body, in_flow);}{
void* _temp1655= out_flow; _LL1657: if(( int) _temp1655 == Cyc_CfFlowInfo_BottomFL){
goto _LL1658;} else{ goto _LL1659;} _LL1659: goto _LL1660; _LL1658: goto _LL1656;
_LL1660: if(( void*) fd->ret_type !=( void*) Cyc_Absyn_VoidType){ Cyc_Tcutil_terr((
fd->body)->loc,( struct _tagged_string)({ char* _temp1661=( char*)"function may complete without returning a value";
struct _tagged_string _temp1662; _temp1662.curr= _temp1661; _temp1662.base=
_temp1661; _temp1662.last_plus_one= _temp1661 + 48; _temp1662;}));} goto _LL1656;
_LL1656:;} Cyc_NewControlFlow_cf_postpass( fd->body);}}}} void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List* ds){ for( 0; ds != 0; ds=({ struct Cyc_List_List*
_temp1663= ds; if( _temp1663 == 0){ _throw( Null_Exception);} _temp1663->tl;})){
void* _temp1665=( void*)(( struct Cyc_Absyn_Decl*)({ struct Cyc_List_List*
_temp1664= ds; if( _temp1664 == 0){ _throw( Null_Exception);} _temp1664->hd;}))->r;
struct Cyc_Absyn_Fndecl* _temp1675; struct Cyc_List_List* _temp1677; struct
_tuple1* _temp1679; struct Cyc_List_List* _temp1681; struct _tagged_string*
_temp1683; _LL1667: if((( struct _tunion_struct*) _temp1665)->tag == Cyc_Absyn_Fn_d_tag){
_LL1676: _temp1675=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*)
_temp1665)->f1; goto _LL1668;} else{ goto _LL1669;} _LL1669: if((( struct
_tunion_struct*) _temp1665)->tag == Cyc_Absyn_Using_d_tag){ _LL1680: _temp1679=(
struct _tuple1*)(( struct Cyc_Absyn_Using_d_struct*) _temp1665)->f1; goto
_LL1678; _LL1678: _temp1677=( struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*)
_temp1665)->f2; goto _LL1670;} else{ goto _LL1671;} _LL1671: if((( struct
_tunion_struct*) _temp1665)->tag == Cyc_Absyn_Namespace_d_tag){ _LL1684:
_temp1683=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1665)->f1; goto _LL1682; _LL1682: _temp1681=( struct Cyc_List_List*)((
struct Cyc_Absyn_Namespace_d_struct*) _temp1665)->f2; goto _LL1672;} else{ goto
_LL1673;} _LL1673: goto _LL1674; _LL1668: Cyc_NewControlFlow_cf_check_fun(
_temp1675); goto _LL1666; _LL1670: _temp1681= _temp1677; goto _LL1672; _LL1672:
Cyc_NewControlFlow_cf_check( _temp1681); goto _LL1666; _LL1674: goto _LL1666;
_LL1666:;}}