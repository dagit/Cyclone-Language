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
region; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Fndecl{ void* sc;
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
struct Cyc_Core_Opt* _temp292; struct Cyc_Absyn_Exp* _temp294; struct Cyc_Absyn_Exp*
_temp296; struct Cyc_Core_Opt* _temp298; struct Cyc_Absyn_Exp* _temp300; struct
Cyc_List_List* _temp302; void* _temp304; struct Cyc_Absyn_Exp* _temp306; void*
_temp308; struct Cyc_List_List* _temp310; struct Cyc_Absyn_Exp* _temp312; struct
Cyc_Absyn_Exp* _temp314; struct Cyc_Absyn_Exp* _temp316; struct Cyc_Absyn_Exp*
_temp318; struct Cyc_Absyn_Exp* _temp320; struct Cyc_Absyn_Exp* _temp322; struct
_tagged_string* _temp324; struct Cyc_Absyn_Exp* _temp326; struct _tagged_string*
_temp328; struct Cyc_Absyn_Exp* _temp330; struct Cyc_List_List* _temp332; struct
Cyc_Absyn_Structdecl* _temp334; struct Cyc_List_List* _temp336; struct Cyc_Core_Opt*
_temp338; struct _tuple1* _temp340; struct Cyc_List_List* _temp342; void*
_temp344; struct Cyc_Absyn_Exp* _temp346; struct Cyc_Absyn_Exp* _temp348; struct
Cyc_Absyn_Exp* _temp350; struct Cyc_Absyn_Exp* _temp352; struct Cyc_Absyn_Exp*
_temp354; struct Cyc_Absyn_Stmt* _temp356; struct Cyc_Absyn_Exp* _temp358;
struct Cyc_Absyn_Exp* _temp360; struct Cyc_Absyn_Exp* _temp362; struct Cyc_Absyn_Exp*
_temp364; struct Cyc_Absyn_Vardecl* _temp366; void* _temp368; struct _tuple1*
_temp370; struct _tuple1* _temp372; struct Cyc_List_List* _temp374; struct Cyc_Absyn_Exp*
_temp376; struct Cyc_List_List* _temp378; struct Cyc_Core_Opt* _temp380; struct
Cyc_List_List* _temp382; struct _tuple2* _temp384; struct Cyc_Absyn_Fndecl*
_temp386; struct Cyc_Absyn_Exp* _temp388; _LL124: if((( struct _tunion_struct*)
_temp122)->tag == Cyc_Absyn_NoInstantiate_e_tag){ _LL211: _temp210=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_NoInstantiate_e_struct*) _temp122)->f1; goto _LL125;} else{
goto _LL126;} _LL126: if((( struct _tunion_struct*) _temp122)->tag == Cyc_Absyn_Sizeofexp_e_tag){
_LL213: _temp212=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp122)->f1; goto _LL127;} else{ goto _LL128;} _LL128: if((( struct
_tunion_struct*) _temp122)->tag == Cyc_Absyn_Instantiate_e_tag){ _LL217:
_temp216=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp122)->f1; goto _LL215; _LL215: _temp214=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Instantiate_e_struct*) _temp122)->f2; goto _LL129;} else{ goto _LL130;}
_LL130: if((( struct _tunion_struct*) _temp122)->tag == Cyc_Absyn_Const_e_tag){
_LL219: _temp218=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp122)->f1; if((
unsigned int) _temp218 > 1u?(( struct _tunion_struct*) _temp218)->tag == Cyc_Absyn_Int_c_tag:
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
_tunion_struct*) _temp122)->tag == Cyc_Absyn_Malloc_e_tag){ _LL289: _temp288=(
void*)(( struct Cyc_Absyn_Malloc_e_struct*) _temp122)->f1; goto _LL155;} else{
goto _LL156;} _LL156: if((( struct _tunion_struct*) _temp122)->tag == Cyc_Absyn_AssignOp_e_tag){
_LL295: _temp294=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp122)->f1; goto _LL293; _LL293: _temp292=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp122)->f2; if( _temp292 == 0){ goto _LL291;}
else{ goto _LL158;} _LL291: _temp290=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp122)->f3; goto _LL157;} else{ goto _LL158;} _LL158: if((( struct
_tunion_struct*) _temp122)->tag == Cyc_Absyn_AssignOp_e_tag){ _LL301: _temp300=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp122)->f1;
goto _LL299; _LL299: _temp298=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp122)->f2; goto _LL297; _LL297: _temp296=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp122)->f3; goto _LL159;} else{ goto _LL160;}
_LL160: if((( struct _tunion_struct*) _temp122)->tag == Cyc_Absyn_Primop_e_tag){
_LL305: _temp304=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp122)->f1;
goto _LL303; _LL303: _temp302=( struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp122)->f2; goto _LL161;} else{ goto _LL162;} _LL162: if((( struct
_tunion_struct*) _temp122)->tag == Cyc_Absyn_Cast_e_tag){ _LL309: _temp308=(
void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp122)->f1; goto _LL307; _LL307:
_temp306=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*) _temp122)->f2;
goto _LL163;} else{ goto _LL164;} _LL164: if((( struct _tunion_struct*) _temp122)->tag
== Cyc_Absyn_FnCall_e_tag){ _LL313: _temp312=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_FnCall_e_struct*) _temp122)->f1; goto _LL311; _LL311: _temp310=(
struct Cyc_List_List*)(( struct Cyc_Absyn_FnCall_e_struct*) _temp122)->f2; goto
_LL165;} else{ goto _LL166;} _LL166: if((( struct _tunion_struct*) _temp122)->tag
== Cyc_Absyn_Subscript_e_tag){ _LL317: _temp316=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp122)->f1; goto _LL315; _LL315:
_temp314=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp122)->f2; goto _LL167;} else{ goto _LL168;} _LL168: if((( struct
_tunion_struct*) _temp122)->tag == Cyc_Absyn_New_e_tag){ _LL319: _temp318=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*) _temp122)->f1; goto
_LL169;} else{ goto _LL170;} _LL170: if((( struct _tunion_struct*) _temp122)->tag
== Cyc_Absyn_Address_e_tag){ _LL321: _temp320=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Address_e_struct*) _temp122)->f1; goto _LL171;} else{ goto _LL172;}
_LL172: if((( struct _tunion_struct*) _temp122)->tag == Cyc_Absyn_Deref_e_tag){
_LL323: _temp322=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp122)->f1; goto _LL173;} else{ goto _LL174;} _LL174: if((( struct
_tunion_struct*) _temp122)->tag == Cyc_Absyn_StructArrow_e_tag){ _LL327:
_temp326=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp122)->f1; goto _LL325; _LL325: _temp324=( struct _tagged_string*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp122)->f2; goto _LL175;} else{ goto _LL176;}
_LL176: if((( struct _tunion_struct*) _temp122)->tag == Cyc_Absyn_StructMember_e_tag){
_LL331: _temp330=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp122)->f1; goto _LL329; _LL329: _temp328=( struct _tagged_string*)(( struct
Cyc_Absyn_StructMember_e_struct*) _temp122)->f2; goto _LL177;} else{ goto _LL178;}
_LL178: if((( struct _tunion_struct*) _temp122)->tag == Cyc_Absyn_Tuple_e_tag){
_LL333: _temp332=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp122)->f1; goto _LL179;} else{ goto _LL180;} _LL180: if((( struct
_tunion_struct*) _temp122)->tag == Cyc_Absyn_Struct_e_tag){ _LL341: _temp340=(
struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*) _temp122)->f1; goto _LL339;
_LL339: _temp338=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp122)->f2; goto _LL337; _LL337: _temp336=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp122)->f3; goto _LL335; _LL335: _temp334=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*) _temp122)->f4;
goto _LL181;} else{ goto _LL182;} _LL182: if((( struct _tunion_struct*) _temp122)->tag
== Cyc_Absyn_Array_e_tag){ _LL343: _temp342=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Array_e_struct*) _temp122)->f1; goto _LL183;} else{ goto _LL184;}
_LL184: if((( struct _tunion_struct*) _temp122)->tag == Cyc_Absyn_Increment_e_tag){
_LL347: _temp346=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp122)->f1; goto _LL345; _LL345: _temp344=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp122)->f2; goto _LL185;} else{ goto _LL186;} _LL186: if((( struct
_tunion_struct*) _temp122)->tag == Cyc_Absyn_Throw_e_tag){ _LL349: _temp348=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*) _temp122)->f1; goto
_LL187;} else{ goto _LL188;} _LL188: if((( struct _tunion_struct*) _temp122)->tag
== Cyc_Absyn_Conditional_e_tag){ _LL355: _temp354=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp122)->f1; goto _LL353; _LL353:
_temp352=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp122)->f2; goto _LL351; _LL351: _temp350=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Conditional_e_struct*) _temp122)->f3; goto _LL189;} else{ goto _LL190;}
_LL190: if((( struct _tunion_struct*) _temp122)->tag == Cyc_Absyn_StmtExp_e_tag){
_LL357: _temp356=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp122)->f1; goto _LL191;} else{ goto _LL192;} _LL192: if((( struct
_tunion_struct*) _temp122)->tag == Cyc_Absyn_SeqExp_e_tag){ _LL361: _temp360=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*) _temp122)->f1; goto
_LL359; _LL359: _temp358=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp122)->f2; goto _LL193;} else{ goto _LL194;} _LL194: if((( struct
_tunion_struct*) _temp122)->tag == Cyc_Absyn_Comprehension_e_tag){ _LL367:
_temp366=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp122)->f1; goto _LL365; _LL365: _temp364=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Comprehension_e_struct*) _temp122)->f2; goto _LL363; _LL363: _temp362=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*) _temp122)->f3;
goto _LL195;} else{ goto _LL196;} _LL196: if((( struct _tunion_struct*) _temp122)->tag
== Cyc_Absyn_Var_e_tag){ _LL371: _temp370=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*)
_temp122)->f1; goto _LL369; _LL369: _temp368=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp122)->f2; if(( int) _temp368 == Cyc_Absyn_Unresolved_b){ goto _LL197;}
else{ goto _LL198;}} else{ goto _LL198;} _LL198: if((( struct _tunion_struct*)
_temp122)->tag == Cyc_Absyn_UnknownId_e_tag){ _LL373: _temp372=( struct _tuple1*)((
struct Cyc_Absyn_UnknownId_e_struct*) _temp122)->f1; goto _LL199;} else{ goto
_LL200;} _LL200: if((( struct _tunion_struct*) _temp122)->tag == Cyc_Absyn_UnknownCall_e_tag){
_LL377: _temp376=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp122)->f1; goto _LL375; _LL375: _temp374=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnknownCall_e_struct*) _temp122)->f2; goto _LL201;} else{ goto _LL202;}
_LL202: if((( struct _tunion_struct*) _temp122)->tag == Cyc_Absyn_UnresolvedMem_e_tag){
_LL381: _temp380=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp122)->f1; goto _LL379; _LL379: _temp378=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnresolvedMem_e_struct*) _temp122)->f2; goto _LL203;} else{ goto
_LL204;} _LL204: if((( struct _tunion_struct*) _temp122)->tag == Cyc_Absyn_CompoundLit_e_tag){
_LL385: _temp384=( struct _tuple2*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp122)->f1; goto _LL383; _LL383: _temp382=( struct Cyc_List_List*)(( struct
Cyc_Absyn_CompoundLit_e_struct*) _temp122)->f2; goto _LL205;} else{ goto _LL206;}
_LL206: if((( struct _tunion_struct*) _temp122)->tag == Cyc_Absyn_Codegen_e_tag){
_LL387: _temp386=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*)
_temp122)->f1; goto _LL207;} else{ goto _LL208;} _LL208: if((( struct
_tunion_struct*) _temp122)->tag == Cyc_Absyn_Fill_e_tag){ _LL389: _temp388=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*) _temp122)->f1; goto
_LL209;} else{ goto _LL123;} _LL125: _temp212= _temp210; goto _LL127; _LL127:
_temp216= _temp212; goto _LL129; _LL129: return Cyc_NewControlFlow_abstract_exp(
env, _temp216); _LL131: env.access_path == 0? 0:(( int(*)( struct _tagged_string
msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({ char* _temp390=( char*)"new_control_flow.cyc:263 env.access_path==null";
struct _tagged_string _temp391; _temp391.curr= _temp390; _temp391.base= _temp390;
_temp391.last_plus_one= _temp390 + 47; _temp391;})); return({ struct Cyc_NewControlFlow_AbsSyn
_temp392; _temp392.when_true=({ struct Cyc_NewControlFlow_AbsSynOne _temp394;
_temp394.inner_exp=( void*) Cyc_CfAbsexp_mkBottomAE(); _temp394.assigns=( void*)
Cyc_CfAbsexp_mkBottomAE(); _temp394;}); _temp392.when_false=({ struct Cyc_NewControlFlow_AbsSynOne
_temp393; _temp393.inner_exp=( void*) Cyc_CfAbsexp_mkSkipAE(); _temp393.assigns=(
void*) Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp());
_temp393;}); _temp392.lvalues= 0; _temp392;}); _LL133: env.access_path == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp395=( char*)"new_control_flow.cyc:268 env.access_path==null";
struct _tagged_string _temp396; _temp396.curr= _temp395; _temp396.base= _temp395;
_temp396.last_plus_one= _temp395 + 47; _temp396;})); return({ struct Cyc_NewControlFlow_AbsSyn
_temp397; _temp397.when_true=({ struct Cyc_NewControlFlow_AbsSynOne _temp399;
_temp399.inner_exp=( void*) Cyc_CfAbsexp_mkSkipAE(); _temp399.assigns=( void*)
Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp()); _temp399;});
_temp397.when_false=({ struct Cyc_NewControlFlow_AbsSynOne _temp398; _temp398.inner_exp=(
void*) Cyc_CfAbsexp_mkBottomAE(); _temp398.assigns=( void*) Cyc_CfAbsexp_mkBottomAE();
_temp398;}); _temp397.lvalues= 0; _temp397;}); _LL135: goto _LL137; _LL137: goto
_LL139; _LL139: goto _LL141; _LL141: return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(),
0, Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp())); _LL143:
return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(), Cyc_NewControlFlow_use_it_list(),
Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp())); _LL145:
_temp260= _temp254; goto _LL147; _LL147: _temp266= _temp260; goto _LL149; _LL149: {
void* absop= Cyc_NewControlFlow_destruct_path( env, Cyc_CfAbsexp_mkLocalOp(
_temp266)); return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(),({
struct Cyc_List_List* _temp400=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp400->hd=( void*) absop; _temp400->tl= 0; _temp400;}),
Cyc_NewControlFlow_make_assigns( env, absop));} _LL151: _temp284= _temp274;
_temp280= _temp270; goto _LL153; _LL153: env.access_path == 0? 0:(( int(*)(
struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({
char* _temp401=( char*)"new_control_flow.cyc:299 env.access_path == null";
struct _tagged_string _temp402; _temp402.curr= _temp401; _temp402.base= _temp401;
_temp402.last_plus_one= _temp401 + 49; _temp402;})); if( _temp284 == 0){ return
Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(), 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));} Cyc_NewControlFlow_add_tunion_root( env, e,
_temp280);{ void* es_exp= Cyc_CfAbsexp_mkSkipAE(); void* useexp= Cyc_CfAbsexp_mkSkipAE();{
int i= 0; for( 0; _temp284 != 0; _temp284=({ struct Cyc_List_List* _temp403=
_temp284; if( _temp403 == 0){ _throw( Null_Exception);} _temp403->tl;}), ++ i){
struct Cyc_NewControlFlow_AbsEnv elt_env= Cyc_NewControlFlow_change_lhs( env,({
struct Cyc_List_List* _temp404=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp404->hd=( void*) Cyc_CfAbsexp_mkMemberOp( Cyc_CfAbsexp_mkMallocOp(
e),( void*)({ struct Cyc_CfFlowInfo_TupleF_struct* _temp405=( struct Cyc_CfFlowInfo_TupleF_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_TupleF_struct)); _temp405[ 0]=({ struct
Cyc_CfFlowInfo_TupleF_struct _temp406; _temp406.tag= Cyc_CfFlowInfo_TupleF_tag;
_temp406.f1= i; _temp406;}); _temp405;})); _temp404->tl= 0; _temp404;})); struct
Cyc_NewControlFlow_AbsSyn syn= Cyc_NewControlFlow_abstract_exp( elt_env,( struct
Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp407= _temp284; if( _temp407 == 0){
_throw( Null_Exception);} _temp407->hd;})); void* _temp410; void* _temp412;
struct Cyc_NewControlFlow_AbsSynOne _temp408= Cyc_NewControlFlow_meet_absexp(
syn); _LL413: _temp412=( void*) _temp408.inner_exp; goto _LL411; _LL411:
_temp410=( void*) _temp408.assigns; goto _LL409; _LL409: es_exp= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, _temp412, es_exp); useexp= Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_mkAnyOrderG(), _temp410, useexp);}} return Cyc_NewControlFlow_malloc_and_init(
env, e, es_exp, useexp);} _LL155: Cyc_NewControlFlow_add_malloc_root( env, e,
_temp288); return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkMallocAE( e), 0,
Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkAddressOp( Cyc_CfAbsexp_mkMallocOp(
e)))); _LL157: { struct Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp(
env, _temp294); struct Cyc_List_List* lhs= syn1.lvalues == 0? Cyc_NewControlFlow_use_it_list():
syn1.lvalues; struct Cyc_NewControlFlow_AbsSyn syn2= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_change_lhs( env, lhs), _temp290); void* _temp416; void*
_temp418; struct Cyc_NewControlFlow_AbsSynOne _temp414= Cyc_NewControlFlow_meet_absexp(
syn1); _LL419: _temp418=( void*) _temp414.inner_exp; goto _LL417; _LL417:
_temp416=( void*) _temp414.assigns; goto _LL415; _LL415: { void* _temp422; void*
_temp424; struct Cyc_NewControlFlow_AbsSynOne _temp420= Cyc_NewControlFlow_meet_absexp(
syn2); _LL425: _temp424=( void*) _temp420.inner_exp; goto _LL423; _LL423:
_temp422=( void*) _temp420.assigns; goto _LL421; _LL421: { void* ansexp= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG,
_temp418, _temp424), _temp422); return Cyc_NewControlFlow_mkSyn_tf( ansexp, 0,
_temp416);}}} _LL159: return Cyc_NewControlFlow_use_exp_unordered( env,({ struct
Cyc_List_List* _temp426=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp426->hd=( void*) _temp300; _temp426->tl=({ struct Cyc_List_List* _temp427=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp427->hd=(
void*) _temp296; _temp427->tl= 0; _temp427;}); _temp426;}), 0); _LL161: return
Cyc_NewControlFlow_use_exp_unordered( env, _temp302, 0); _LL163: { int both_leaf=
1;{ void* _temp429=( void*)({ struct Cyc_Core_Opt* _temp428= _temp306->topt; if(
_temp428 == 0){ _throw( Null_Exception);} _temp428->v;}); struct Cyc_Absyn_Structdecl**
_temp437; struct Cyc_List_List* _temp439; struct _tuple1* _temp441; struct Cyc_List_List*
_temp443; _LL431: if(( unsigned int) _temp429 > 4u?(( struct _tunion_struct*)
_temp429)->tag == Cyc_Absyn_StructType_tag: 0){ _LL442: _temp441=( struct
_tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp429)->f1; goto _LL440;
_LL440: _temp439=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp429)->f2; goto _LL438; _LL438: _temp437=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp429)->f3; goto _LL432;} else{ goto
_LL433;} _LL433: if(( unsigned int) _temp429 > 4u?(( struct _tunion_struct*)
_temp429)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL444: _temp443=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp429)->f1; goto _LL434;} else{ goto
_LL435;} _LL435: goto _LL436; _LL432: goto _LL434; _LL434: both_leaf= 0; goto
_LL430; _LL436: goto _LL430; _LL430:;}{ void* _temp446=( void*)({ struct Cyc_Core_Opt*
_temp445= e->topt; if( _temp445 == 0){ _throw( Null_Exception);} _temp445->v;});
struct Cyc_Absyn_Structdecl** _temp454; struct Cyc_List_List* _temp456; struct
_tuple1* _temp458; struct Cyc_List_List* _temp460; _LL448: if(( unsigned int)
_temp446 > 4u?(( struct _tunion_struct*) _temp446)->tag == Cyc_Absyn_StructType_tag:
0){ _LL459: _temp458=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp446)->f1; goto _LL457; _LL457: _temp456=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp446)->f2; goto _LL455; _LL455: _temp454=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp446)->f3;
goto _LL449;} else{ goto _LL450;} _LL450: if(( unsigned int) _temp446 > 4u?((
struct _tunion_struct*) _temp446)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL461:
_temp460=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp446)->f1;
goto _LL451;} else{ goto _LL452;} _LL452: goto _LL453; _LL449: goto _LL451;
_LL451: both_leaf= 0; goto _LL447; _LL453: goto _LL447; _LL447:;} if( both_leaf){
return Cyc_NewControlFlow_abstract_exp( env, _temp306);} return Cyc_NewControlFlow_use_exp_unordered(
env,({ struct Cyc_List_List* _temp462=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp462->hd=( void*) _temp306; _temp462->tl= 0;
_temp462;}), env.lhsides);} _LL165: return Cyc_NewControlFlow_use_exp_unordered(
env,({ struct Cyc_List_List* _temp463=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp463->hd=( void*) _temp312; _temp463->tl=
_temp310; _temp463;}), 0); _LL167: { void* _temp465= Cyc_Tcutil_compress(( void*)({
struct Cyc_Core_Opt* _temp464= _temp316->topt; if( _temp464 == 0){ _throw(
Null_Exception);} _temp464->v;})); struct Cyc_List_List* _temp471; _LL467: if((
unsigned int) _temp465 > 4u?(( struct _tunion_struct*) _temp465)->tag == Cyc_Absyn_TupleType_tag:
0){ _LL472: _temp471=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp465)->f1; goto _LL468;} else{ goto _LL469;} _LL469: goto _LL470; _LL468:
return Cyc_NewControlFlow_descend_path( env, e, _temp316); _LL470: return Cyc_NewControlFlow_use_exp_unordered(
env,({ struct Cyc_List_List* _temp473=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp473->hd=( void*) _temp316; _temp473->tl=({
struct Cyc_List_List* _temp474=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp474->hd=( void*) _temp314; _temp474->tl= 0;
_temp474;}); _temp473;}), Cyc_NewControlFlow_use_it_list()); _LL466:;} _LL169: {
void* _temp475=( void*) _temp318->r; struct Cyc_Absyn_Structdecl* _temp483;
struct Cyc_List_List* _temp485; struct Cyc_Core_Opt* _temp487; struct _tuple1*
_temp489; struct Cyc_List_List* _temp491; _LL477: if((( struct _tunion_struct*)
_temp475)->tag == Cyc_Absyn_Struct_e_tag){ _LL490: _temp489=( struct _tuple1*)((
struct Cyc_Absyn_Struct_e_struct*) _temp475)->f1; goto _LL488; _LL488: _temp487=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*) _temp475)->f2; goto
_LL486; _LL486: _temp485=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp475)->f3; goto _LL484; _LL484: _temp483=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp475)->f4; goto _LL478;} else{ goto
_LL479;} _LL479: if((( struct _tunion_struct*) _temp475)->tag == Cyc_Absyn_Tuple_e_tag){
_LL492: _temp491=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp475)->f1; goto _LL480;} else{ goto _LL481;} _LL481: goto _LL482; _LL478:
goto _LL480; _LL480: { void* malloc_op= Cyc_CfAbsexp_mkMallocOp( e); Cyc_NewControlFlow_add_malloc_root(
env, e,( void*)({ struct Cyc_Core_Opt* _temp493= _temp318->topt; if( _temp493 ==
0){ _throw( Null_Exception);} _temp493->v;}));{ struct Cyc_NewControlFlow_AbsEnv
inner_env= Cyc_NewControlFlow_change_lhs( env,({ struct Cyc_List_List* _temp494=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp494->hd=(
void*) malloc_op; _temp494->tl= 0; _temp494;})); inner_env.access_path= 0;{ void*
_temp497; void* _temp499; struct Cyc_NewControlFlow_AbsSynOne _temp495= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( inner_env, _temp318)); _LL500: _temp499=( void*)
_temp495.inner_exp; goto _LL498; _LL498: _temp497=( void*) _temp495.assigns;
goto _LL496; _LL496: { struct Cyc_NewControlFlow_AbsSyn ans= Cyc_NewControlFlow_malloc_and_init(
env, e, _temp499, _temp497); if( env.access_path != 0){ void* _temp502=( void*)((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp501= env.access_path; if(
_temp501 == 0){ _throw( Null_Exception);} _temp501->hd;}))->r; struct Cyc_Absyn_Exp*
_temp512; struct Cyc_Absyn_Exp* _temp514; struct Cyc_Absyn_Exp* _temp516; struct
_tagged_string* _temp518; struct Cyc_Absyn_Exp* _temp520; _LL504: if((( struct
_tunion_struct*) _temp502)->tag == Cyc_Absyn_Subscript_e_tag){ _LL515: _temp514=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*) _temp502)->f1;
goto _LL513; _LL513: _temp512=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp502)->f2; goto _LL505;} else{ goto _LL506;} _LL506: if((( struct
_tunion_struct*) _temp502)->tag == Cyc_Absyn_Deref_e_tag){ _LL517: _temp516=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*) _temp502)->f1; goto
_LL507;} else{ goto _LL508;} _LL508: if((( struct _tunion_struct*) _temp502)->tag
== Cyc_Absyn_StructArrow_e_tag){ _LL521: _temp520=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp502)->f1; goto _LL519; _LL519:
_temp518=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp502)->f2; goto _LL509;} else{ goto _LL510;} _LL510: goto _LL511; _LL505:
Cyc_Evexp_eval_const_uint_exp( _temp512) ==( unsigned int) 0? 0:(( int(*)(
struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({
char* _temp522=( char*)"new_control_flow.cyc:410 Evexp::eval_const_uint_exp(e2) == 0";
struct _tagged_string _temp523; _temp523.curr= _temp522; _temp523.base= _temp522;
_temp523.last_plus_one= _temp522 + 61; _temp523;})); goto _LL507; _LL507: env.access_path=({
struct Cyc_List_List* _temp524= env.access_path; if( _temp524 == 0){ _throw(
Null_Exception);} _temp524->tl;}); ans.lvalues=({ struct Cyc_List_List* _temp525=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp525->hd=(
void*) Cyc_NewControlFlow_destruct_path( env, malloc_op); _temp525->tl= 0;
_temp525;}); goto _LL503; _LL509: env.access_path=({ struct Cyc_List_List*
_temp526=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp526->hd=( void*) Cyc_Absyn_structmember_exp( _temp520, _temp518, 0);
_temp526->tl=({ struct Cyc_List_List* _temp527= env.access_path; if( _temp527 ==
0){ _throw( Null_Exception);} _temp527->tl;}); _temp526;}); ans.lvalues=({
struct Cyc_List_List* _temp528=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp528->hd=( void*) Cyc_NewControlFlow_destruct_path(
env, malloc_op); _temp528->tl= 0; _temp528;}); goto _LL503; _LL511:( void)
_throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct* _temp529=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp529[ 0]=({ struct Cyc_Core_Impossible_struct _temp530; _temp530.tag= Cyc_Core_Impossible_tag;
_temp530.f1=( struct _tagged_string)({ char* _temp531=( char*)"bad malloc access path";
struct _tagged_string _temp532; _temp532.curr= _temp531; _temp532.base= _temp531;
_temp532.last_plus_one= _temp531 + 23; _temp532;}); _temp530;}); _temp529;}));
_LL503:;} return ans;}}}} _LL482: return Cyc_NewControlFlow_use_exp_unordered(
env,({ struct Cyc_List_List* _temp533=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp533->hd=( void*) _temp318; _temp533->tl= 0;
_temp533;}), 0); _LL476:;} _LL171:{ void* _temp534=( void*) _temp320->r; struct
Cyc_Absyn_Structdecl* _temp542; struct Cyc_List_List* _temp544; struct Cyc_Core_Opt*
_temp546; struct _tuple1* _temp548; struct Cyc_List_List* _temp550; _LL536: if(((
struct _tunion_struct*) _temp534)->tag == Cyc_Absyn_Struct_e_tag){ _LL549:
_temp548=( struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*) _temp534)->f1;
goto _LL547; _LL547: _temp546=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp534)->f2; goto _LL545; _LL545: _temp544=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp534)->f3; goto _LL543; _LL543: _temp542=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*) _temp534)->f4;
goto _LL537;} else{ goto _LL538;} _LL538: if((( struct _tunion_struct*) _temp534)->tag
== Cyc_Absyn_Tuple_e_tag){ _LL551: _temp550=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Tuple_e_struct*) _temp534)->f1; goto _LL539;} else{ goto _LL540;}
_LL540: goto _LL541; _LL537: goto _LL539; _LL539: { void* malloc_op= Cyc_CfAbsexp_mkMallocOp(
e); Cyc_NewControlFlow_add_malloc_root( env, e,( void*)({ struct Cyc_Core_Opt*
_temp552= _temp320->topt; if( _temp552 == 0){ _throw( Null_Exception);} _temp552->v;}));{
struct Cyc_NewControlFlow_AbsEnv inner_env= Cyc_NewControlFlow_change_lhs( env,({
struct Cyc_List_List* _temp553=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp553->hd=( void*) malloc_op; _temp553->tl= 0;
_temp553;})); inner_env.access_path= 0;{ void* _temp556; void* _temp558; struct
Cyc_NewControlFlow_AbsSynOne _temp554= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
inner_env, _temp320)); _LL559: _temp558=( void*) _temp554.inner_exp; goto _LL557;
_LL557: _temp556=( void*) _temp554.assigns; goto _LL555; _LL555: { struct Cyc_NewControlFlow_AbsSyn
ans= Cyc_NewControlFlow_malloc_and_init( env, e, _temp558, _temp556); if( env.access_path
!= 0){ void* _temp561=( void*)(( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp560= env.access_path; if( _temp560 == 0){ _throw( Null_Exception);}
_temp560->hd;}))->r; struct Cyc_Absyn_Exp* _temp571; struct Cyc_Absyn_Exp*
_temp573; struct Cyc_Absyn_Exp* _temp575; struct _tagged_string* _temp577;
struct Cyc_Absyn_Exp* _temp579; _LL563: if((( struct _tunion_struct*) _temp561)->tag
== Cyc_Absyn_Subscript_e_tag){ _LL574: _temp573=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp561)->f1; goto _LL572; _LL572:
_temp571=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp561)->f2; goto _LL564;} else{ goto _LL565;} _LL565: if((( struct
_tunion_struct*) _temp561)->tag == Cyc_Absyn_Deref_e_tag){ _LL576: _temp575=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*) _temp561)->f1; goto
_LL566;} else{ goto _LL567;} _LL567: if((( struct _tunion_struct*) _temp561)->tag
== Cyc_Absyn_StructArrow_e_tag){ _LL580: _temp579=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp561)->f1; goto _LL578; _LL578:
_temp577=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp561)->f2; goto _LL568;} else{ goto _LL569;} _LL569: goto _LL570; _LL564:
Cyc_Evexp_eval_const_uint_exp( _temp571) ==( unsigned int) 0? 0:(( int(*)(
struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({
char* _temp581=( char*)"new_control_flow.cyc:447 Evexp::eval_const_uint_exp(e2) == 0";
struct _tagged_string _temp582; _temp582.curr= _temp581; _temp582.base= _temp581;
_temp582.last_plus_one= _temp581 + 61; _temp582;})); goto _LL566; _LL566: env.access_path=({
struct Cyc_List_List* _temp583= env.access_path; if( _temp583 == 0){ _throw(
Null_Exception);} _temp583->tl;}); ans.lvalues=({ struct Cyc_List_List* _temp584=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp584->hd=(
void*) Cyc_NewControlFlow_destruct_path( env, malloc_op); _temp584->tl= 0;
_temp584;}); goto _LL562; _LL568: env.access_path=({ struct Cyc_List_List*
_temp585=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp585->hd=( void*) Cyc_Absyn_structmember_exp( _temp579, _temp577, 0);
_temp585->tl=({ struct Cyc_List_List* _temp586= env.access_path; if( _temp586 ==
0){ _throw( Null_Exception);} _temp586->tl;}); _temp585;}); ans.lvalues=({
struct Cyc_List_List* _temp587=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp587->hd=( void*) Cyc_NewControlFlow_destruct_path(
env, malloc_op); _temp587->tl= 0; _temp587;}); goto _LL562; _LL570:( void)
_throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct* _temp588=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp588[ 0]=({ struct Cyc_Core_Impossible_struct _temp589; _temp589.tag= Cyc_Core_Impossible_tag;
_temp589.f1=( struct _tagged_string)({ char* _temp590=( char*)"bad malloc access path";
struct _tagged_string _temp591; _temp591.curr= _temp590; _temp591.base= _temp590;
_temp591.last_plus_one= _temp590 + 23; _temp591;}); _temp589;}); _temp588;}));
_LL562:;} return ans;}}}} _LL541: goto _LL535; _LL535:;}{ struct Cyc_List_List*
_temp592= env.access_path; struct Cyc_List_List _temp604; struct Cyc_List_List*
_temp605; struct Cyc_Absyn_Exp* _temp607; struct Cyc_Absyn_Exp _temp609; struct
Cyc_Position_Segment* _temp610; void* _temp612; struct Cyc_Absyn_Exp* _temp614;
struct Cyc_Absyn_Exp* _temp616; struct Cyc_Core_Opt* _temp618; struct Cyc_List_List
_temp620; struct Cyc_List_List* _temp621; struct Cyc_Absyn_Exp* _temp623; struct
Cyc_Absyn_Exp _temp625; struct Cyc_Position_Segment* _temp626; void* _temp628;
struct Cyc_Absyn_Exp* _temp630; struct Cyc_Core_Opt* _temp632; struct Cyc_List_List
_temp634; struct Cyc_List_List* _temp635; struct Cyc_Absyn_Exp* _temp637; struct
Cyc_Absyn_Exp _temp639; struct Cyc_Position_Segment* _temp640; void* _temp642;
struct _tagged_string* _temp644; struct Cyc_Absyn_Exp* _temp646; struct Cyc_Core_Opt*
_temp648; _LL594: if( _temp592 == 0){ goto _LL595;} else{ goto _LL596;} _LL596:
if( _temp592 == 0){ goto _LL598;} else{ _temp604=* _temp592; _LL608: _temp607=(
struct Cyc_Absyn_Exp*) _temp604.hd; _temp609=* _temp607; _LL619: _temp618=(
struct Cyc_Core_Opt*) _temp609.topt; goto _LL613; _LL613: _temp612=( void*)
_temp609.r; if((( struct _tunion_struct*) _temp612)->tag == Cyc_Absyn_Subscript_e_tag){
_LL617: _temp616=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp612)->f1; goto _LL615; _LL615: _temp614=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Subscript_e_struct*) _temp612)->f2; goto _LL611;} else{ goto _LL598;}
_LL611: _temp610=( struct Cyc_Position_Segment*) _temp609.loc; goto _LL606;
_LL606: _temp605=( struct Cyc_List_List*) _temp604.tl; goto _LL597;} _LL598: if(
_temp592 == 0){ goto _LL600;} else{ _temp620=* _temp592; _LL624: _temp623=(
struct Cyc_Absyn_Exp*) _temp620.hd; _temp625=* _temp623; _LL633: _temp632=(
struct Cyc_Core_Opt*) _temp625.topt; goto _LL629; _LL629: _temp628=( void*)
_temp625.r; if((( struct _tunion_struct*) _temp628)->tag == Cyc_Absyn_Deref_e_tag){
_LL631: _temp630=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp628)->f1; goto _LL627;} else{ goto _LL600;} _LL627: _temp626=( struct Cyc_Position_Segment*)
_temp625.loc; goto _LL622; _LL622: _temp621=( struct Cyc_List_List*) _temp620.tl;
goto _LL599;} _LL600: if( _temp592 == 0){ goto _LL602;} else{ _temp634=*
_temp592; _LL638: _temp637=( struct Cyc_Absyn_Exp*) _temp634.hd; _temp639=*
_temp637; _LL649: _temp648=( struct Cyc_Core_Opt*) _temp639.topt; goto _LL643;
_LL643: _temp642=( void*) _temp639.r; if((( struct _tunion_struct*) _temp642)->tag
== Cyc_Absyn_StructArrow_e_tag){ _LL647: _temp646=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp642)->f1; goto _LL645; _LL645:
_temp644=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp642)->f2; goto _LL641;} else{ goto _LL602;} _LL641: _temp640=( struct Cyc_Position_Segment*)
_temp639.loc; goto _LL636; _LL636: _temp635=( struct Cyc_List_List*) _temp634.tl;
goto _LL601;} _LL602: goto _LL603; _LL595: env.access_path=({ struct Cyc_List_List*
_temp650=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp650->hd=( void*) e; _temp650->tl= env.access_path; _temp650;}); goto _LL593;
_LL597: Cyc_Evexp_eval_const_uint_exp( _temp614) ==( unsigned int) 0? 0:(( int(*)(
struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({
char* _temp651=( char*)"new_control_flow.cyc:475 Evexp::eval_const_uint_exp(e2) == 0";
struct _tagged_string _temp652; _temp652.curr= _temp651; _temp652.base= _temp651;
_temp652.last_plus_one= _temp651 + 61; _temp652;})); _temp621= _temp605; goto
_LL599; _LL599: env.access_path= _temp621; goto _LL593; _LL601: env.access_path=({
struct Cyc_List_List* _temp653=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp653->hd=( void*) Cyc_Absyn_structmember_exp(
_temp646, _temp644, 0); _temp653->tl= _temp635; _temp653;}); goto _LL593; _LL603:(
void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp654=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp654[ 0]=({ struct Cyc_Core_Impossible_struct _temp655; _temp655.tag= Cyc_Core_Impossible_tag;
_temp655.f1=( struct _tagged_string)({ char* _temp656=( char*)"abstract_exp: bad access path for &";
struct _tagged_string _temp657; _temp657.curr= _temp656; _temp657.base= _temp656;
_temp657.last_plus_one= _temp656 + 36; _temp657;}); _temp655;}); _temp654;}));
_LL593:;} return Cyc_NewControlFlow_abstract_exp( env, _temp320); _LL173: {
struct Cyc_List_List* _temp658= env.access_path; struct Cyc_List_List _temp664;
struct Cyc_List_List* _temp665; struct Cyc_Absyn_Exp* _temp667; struct Cyc_Absyn_Exp
_temp669; struct Cyc_Position_Segment* _temp670; void* _temp672; struct Cyc_Absyn_Exp*
_temp674; struct Cyc_Core_Opt* _temp676; _LL660: if( _temp658 == 0){ goto _LL662;}
else{ _temp664=* _temp658; _LL668: _temp667=( struct Cyc_Absyn_Exp*) _temp664.hd;
_temp669=* _temp667; _LL677: _temp676=( struct Cyc_Core_Opt*) _temp669.topt;
goto _LL673; _LL673: _temp672=( void*) _temp669.r; if((( struct _tunion_struct*)
_temp672)->tag == Cyc_Absyn_Address_e_tag){ _LL675: _temp674=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Address_e_struct*) _temp672)->f1; goto _LL671;} else{ goto
_LL662;} _LL671: _temp670=( struct Cyc_Position_Segment*) _temp669.loc; goto
_LL666; _LL666: _temp665=( struct Cyc_List_List*) _temp664.tl; goto _LL661;}
_LL662: goto _LL663; _LL661: env.access_path= _temp665;{ struct Cyc_NewControlFlow_AbsSyn
syn1= Cyc_NewControlFlow_abstract_exp( env, _temp322); if( syn1.lvalues == 0){
syn1.lvalues= Cyc_NewControlFlow_use_it_list();} return syn1;} _LL663: return
Cyc_NewControlFlow_descend_path( env, e, _temp322); _LL659:;} _LL175: { void*
_temp679= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp678= _temp326->topt;
if( _temp678 == 0){ _throw( Null_Exception);} _temp678->v;})); struct Cyc_Absyn_PtrInfo
_temp685; struct Cyc_Absyn_Conref* _temp687; struct Cyc_Absyn_Tqual* _temp689;
struct Cyc_Absyn_Conref* _temp691; void* _temp693; void* _temp695; _LL681: if((
unsigned int) _temp679 > 4u?(( struct _tunion_struct*) _temp679)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL686: _temp685=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp679)->f1; _LL696: _temp695=( void*) _temp685.elt_typ; goto _LL694; _LL694:
_temp693=( void*) _temp685.rgn_typ; goto _LL692; _LL692: _temp691=( struct Cyc_Absyn_Conref*)
_temp685.nullable; goto _LL690; _LL690: _temp689=( struct Cyc_Absyn_Tqual*)
_temp685.tq; goto _LL688; _LL688: _temp687=( struct Cyc_Absyn_Conref*) _temp685.bounds;
goto _LL682;} else{ goto _LL683;} _LL683: goto _LL684; _LL682:{ void* _temp697=
Cyc_Tcutil_compress( _temp695); struct Cyc_Absyn_Structdecl** _temp705; struct
Cyc_List_List* _temp707; struct _tuple1* _temp709; struct Cyc_Absyn_Uniondecl**
_temp711; struct Cyc_List_List* _temp713; struct _tuple1* _temp715; _LL699: if((
unsigned int) _temp697 > 4u?(( struct _tunion_struct*) _temp697)->tag == Cyc_Absyn_StructType_tag:
0){ _LL710: _temp709=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp697)->f1; goto _LL708; _LL708: _temp707=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp697)->f2; goto _LL706; _LL706: _temp705=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp697)->f3;
goto _LL700;} else{ goto _LL701;} _LL701: if(( unsigned int) _temp697 > 4u?((
struct _tunion_struct*) _temp697)->tag == Cyc_Absyn_UnionType_tag: 0){ _LL716:
_temp715=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*) _temp697)->f1;
goto _LL714; _LL714: _temp713=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp697)->f2; goto _LL712; _LL712: _temp711=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp697)->f3; goto _LL702;} else{ goto
_LL703;} _LL703: goto _LL704; _LL700: return Cyc_NewControlFlow_descend_path(
env, e, _temp326); _LL702: { struct Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp(
env, _temp326); syn1.lvalues= Cyc_NewControlFlow_use_it_list(); return syn1;}
_LL704: goto _LL698; _LL698:;} goto _LL684; _LL684:( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Core_Impossible_struct* _temp717=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp717[ 0]=({ struct
Cyc_Core_Impossible_struct _temp718; _temp718.tag= Cyc_Core_Impossible_tag;
_temp718.f1=( struct _tagged_string)({ char* _temp719=( char*)"NewControlFlow: bad type in StructArrow";
struct _tagged_string _temp720; _temp720.curr= _temp719; _temp720.base= _temp719;
_temp720.last_plus_one= _temp719 + 40; _temp720;}); _temp718;}); _temp717;}));
_LL680:;} _LL177: { void* _temp722= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp721= _temp330->topt; if( _temp721 == 0){ _throw( Null_Exception);} _temp721->v;}));
struct Cyc_Absyn_Structdecl** _temp730; struct Cyc_List_List* _temp732; struct
_tuple1* _temp734; struct Cyc_Absyn_Uniondecl** _temp736; struct Cyc_List_List*
_temp738; struct _tuple1* _temp740; _LL724: if(( unsigned int) _temp722 > 4u?((
struct _tunion_struct*) _temp722)->tag == Cyc_Absyn_StructType_tag: 0){ _LL735:
_temp734=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp722)->f1;
goto _LL733; _LL733: _temp732=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp722)->f2; goto _LL731; _LL731: _temp730=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp722)->f3; goto _LL725;} else{ goto
_LL726;} _LL726: if(( unsigned int) _temp722 > 4u?(( struct _tunion_struct*)
_temp722)->tag == Cyc_Absyn_UnionType_tag: 0){ _LL741: _temp740=( struct _tuple1*)((
struct Cyc_Absyn_UnionType_struct*) _temp722)->f1; goto _LL739; _LL739: _temp738=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*) _temp722)->f2; goto
_LL737; _LL737: _temp736=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp722)->f3; goto _LL727;} else{ goto _LL728;} _LL728: goto _LL729; _LL725:
return Cyc_NewControlFlow_descend_path( env, e, _temp330); _LL727: { struct Cyc_NewControlFlow_AbsSyn
syn1= Cyc_NewControlFlow_abstract_exp( env, _temp330); syn1.lvalues= Cyc_NewControlFlow_use_it_list();
return syn1;} _LL729:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp742=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp742[ 0]=({ struct Cyc_Core_Impossible_struct _temp743; _temp743.tag= Cyc_Core_Impossible_tag;
_temp743.f1=( struct _tagged_string)({ char* _temp744=( char*)"NewControlFlow: bad type in StructMember";
struct _tagged_string _temp745; _temp745.curr= _temp744; _temp745.base= _temp744;
_temp745.last_plus_one= _temp744 + 41; _temp745;}); _temp743;}); _temp742;}));
_LL723:;} _LL179: { struct Cyc_List_List* _temp746= env.access_path; struct Cyc_List_List
_temp754; struct Cyc_List_List* _temp755; struct Cyc_Absyn_Exp* _temp757; struct
Cyc_Absyn_Exp _temp759; struct Cyc_Position_Segment* _temp760; void* _temp762;
struct Cyc_Absyn_Exp* _temp764; struct Cyc_Absyn_Exp* _temp766; struct Cyc_Core_Opt*
_temp768; _LL748: if( _temp746 == 0){ goto _LL749;} else{ goto _LL750;} _LL750:
if( _temp746 == 0){ goto _LL752;} else{ _temp754=* _temp746; _LL758: _temp757=(
struct Cyc_Absyn_Exp*) _temp754.hd; _temp759=* _temp757; _LL769: _temp768=(
struct Cyc_Core_Opt*) _temp759.topt; goto _LL763; _LL763: _temp762=( void*)
_temp759.r; if((( struct _tunion_struct*) _temp762)->tag == Cyc_Absyn_Subscript_e_tag){
_LL767: _temp766=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp762)->f1; goto _LL765; _LL765: _temp764=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Subscript_e_struct*) _temp762)->f2; goto _LL761;} else{ goto _LL752;}
_LL761: _temp760=( struct Cyc_Position_Segment*) _temp759.loc; goto _LL756;
_LL756: _temp755=( struct Cyc_List_List*) _temp754.tl; goto _LL751;} _LL752:
goto _LL753; _LL749: { void* ansexp= Cyc_CfAbsexp_mkSkipAE(); void* assignexp=
Cyc_CfAbsexp_mkSkipAE();{ int j= 0; for( 0; _temp332 != 0; _temp332=({ struct
Cyc_List_List* _temp770= _temp332; if( _temp770 == 0){ _throw( Null_Exception);}
_temp770->tl;}), ++ j){ struct Cyc_List_List* ith_lvals= 0;{ struct Cyc_List_List*
ls= env.lhsides; for( 0; ls != 0; ls=({ struct Cyc_List_List* _temp771= ls; if(
_temp771 == 0){ _throw( Null_Exception);} _temp771->tl;})){ if( Cyc_CfAbsexp_isUnknownOp((
void*)({ struct Cyc_List_List* _temp772= ls; if( _temp772 == 0){ _throw(
Null_Exception);} _temp772->hd;}))){ ith_lvals=({ struct Cyc_List_List* _temp773=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp773->hd=(
void*) Cyc_CfAbsexp_mkUnknownOp(); _temp773->tl= ith_lvals; _temp773;});} else{
ith_lvals=({ struct Cyc_List_List* _temp774=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp774->hd=( void*) Cyc_CfAbsexp_mkMemberOp((
void*)({ struct Cyc_List_List* _temp775= ls; if( _temp775 == 0){ _throw(
Null_Exception);} _temp775->hd;}),( void*)({ struct Cyc_CfFlowInfo_TupleF_struct*
_temp776=( struct Cyc_CfFlowInfo_TupleF_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_TupleF_struct));
_temp776[ 0]=({ struct Cyc_CfFlowInfo_TupleF_struct _temp777; _temp777.tag= Cyc_CfFlowInfo_TupleF_tag;
_temp777.f1= j; _temp777;}); _temp776;})); _temp774->tl= ith_lvals; _temp774;});}}}{
struct Cyc_NewControlFlow_AbsSyn ith_syn= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_change_lhs(
env, ith_lvals),( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp778=
_temp332; if( _temp778 == 0){ _throw( Null_Exception);} _temp778->hd;})); void*
_temp781; void* _temp783; struct Cyc_NewControlFlow_AbsSynOne _temp779= Cyc_NewControlFlow_meet_absexp(
ith_syn); _LL784: _temp783=( void*) _temp779.inner_exp; goto _LL782; _LL782:
_temp781=( void*) _temp779.assigns; goto _LL780; _LL780: ansexp= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, ansexp, _temp783); assignexp= Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_mkAnyOrderG(), assignexp, _temp781);}}} return Cyc_NewControlFlow_mkSyn_tf(
ansexp, 0, assignexp);} _LL751: { int i=( int) Cyc_Evexp_eval_const_uint_exp(
_temp764); struct Cyc_NewControlFlow_AbsEnv other_env= Cyc_NewControlFlow_change_lhs(
env, 0); other_env.access_path= 0; env.access_path= _temp755;{ void* ansexp= Cyc_CfAbsexp_mkSkipAE();
void* assignexp= Cyc_CfAbsexp_mkSkipAE();{ int j= 0; for( 0; _temp332 != 0;
_temp332=({ struct Cyc_List_List* _temp785= _temp332; if( _temp785 == 0){ _throw(
Null_Exception);} _temp785->tl;}), ++ j){ if( i == j){ void* _temp789; void*
_temp791; struct Cyc_NewControlFlow_AbsSynOne _temp787= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( env,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp786= _temp332; if( _temp786 == 0){ _throw( Null_Exception);} _temp786->hd;})));
_LL792: _temp791=( void*) _temp787.inner_exp; goto _LL790; _LL790: _temp789=(
void*) _temp787.assigns; goto _LL788; _LL788: assignexp= _temp789; ansexp= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, ansexp, _temp791);} else{ void* _temp796; void*
_temp798; struct Cyc_NewControlFlow_AbsSynOne _temp794= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( other_env,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp793= _temp332; if( _temp793 == 0){ _throw( Null_Exception);} _temp793->hd;})));
_LL799: _temp798=( void*) _temp794.inner_exp; goto _LL797; _LL797: _temp796=(
void*) _temp794.assigns; goto _LL795; _LL795: ansexp= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, ansexp, _temp798);}}} return Cyc_NewControlFlow_mkSyn_tf(
ansexp,({ struct Cyc_List_List* _temp800=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp800->hd=( void*) Cyc_CfAbsexp_mkUnknownOp();
_temp800->tl= 0; _temp800;}), assignexp);}} _LL753:( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Core_Impossible_struct* _temp801=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp801[ 0]=({ struct
Cyc_Core_Impossible_struct _temp802; _temp802.tag= Cyc_Core_Impossible_tag;
_temp802.f1=( struct _tagged_string)({ char* _temp803=( char*)"abstract_exp: unexpected access path to Tuple";
struct _tagged_string _temp804; _temp804.curr= _temp803; _temp804.base= _temp803;
_temp804.last_plus_one= _temp803 + 46; _temp804;}); _temp802;}); _temp801;}));
_LL747:;} _LL181: { struct Cyc_List_List* _temp805= env.access_path; struct Cyc_List_List
_temp813; struct Cyc_List_List* _temp814; struct Cyc_Absyn_Exp* _temp816; struct
Cyc_Absyn_Exp _temp818; struct Cyc_Position_Segment* _temp819; void* _temp821;
struct _tagged_string* _temp823; struct Cyc_Absyn_Exp* _temp825; struct Cyc_Core_Opt*
_temp827; _LL807: if( _temp805 == 0){ goto _LL808;} else{ goto _LL809;} _LL809:
if( _temp805 == 0){ goto _LL811;} else{ _temp813=* _temp805; _LL817: _temp816=(
struct Cyc_Absyn_Exp*) _temp813.hd; _temp818=* _temp816; _LL828: _temp827=(
struct Cyc_Core_Opt*) _temp818.topt; goto _LL822; _LL822: _temp821=( void*)
_temp818.r; if((( struct _tunion_struct*) _temp821)->tag == Cyc_Absyn_StructMember_e_tag){
_LL826: _temp825=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp821)->f1; goto _LL824; _LL824: _temp823=( struct _tagged_string*)(( struct
Cyc_Absyn_StructMember_e_struct*) _temp821)->f2; goto _LL820;} else{ goto _LL811;}
_LL820: _temp819=( struct Cyc_Position_Segment*) _temp818.loc; goto _LL815;
_LL815: _temp814=( struct Cyc_List_List*) _temp813.tl; goto _LL810;} _LL811:
goto _LL812; _LL808: { void* ansexp= Cyc_CfAbsexp_mkSkipAE(); void* assignexp=
Cyc_CfAbsexp_mkSkipAE(); for( 0; _temp336 != 0; _temp336=({ struct Cyc_List_List*
_temp829= _temp336; if( _temp829 == 0){ _throw( Null_Exception);} _temp829->tl;})){
struct Cyc_List_List* ith_lvals= 0;{ struct Cyc_List_List* ls= env.lhsides; for(
0; ls != 0; ls=({ struct Cyc_List_List* _temp830= ls; if( _temp830 == 0){ _throw(
Null_Exception);} _temp830->tl;})){ if( Cyc_CfAbsexp_isUnknownOp(( void*)({
struct Cyc_List_List* _temp831= ls; if( _temp831 == 0){ _throw( Null_Exception);}
_temp831->hd;}))){ ith_lvals=({ struct Cyc_List_List* _temp832=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp832->hd=( void*) Cyc_CfAbsexp_mkUnknownOp();
_temp832->tl= ith_lvals; _temp832;});} else{ struct Cyc_List_List* ds=(*((
struct _tuple5*)({ struct Cyc_List_List* _temp833= _temp336; if( _temp833 == 0){
_throw( Null_Exception);} _temp833->hd;}))).f1; for( 0; ds != 0; ds=({ struct
Cyc_List_List* _temp834= ds; if( _temp834 == 0){ _throw( Null_Exception);}
_temp834->tl;})){ void* _temp836=( void*)({ struct Cyc_List_List* _temp835= ds;
if( _temp835 == 0){ _throw( Null_Exception);} _temp835->hd;}); struct Cyc_Absyn_Exp*
_temp842; struct _tagged_string* _temp844; _LL838: if((( struct _tunion_struct*)
_temp836)->tag == Cyc_Absyn_ArrayElement_tag){ _LL843: _temp842=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayElement_struct*) _temp836)->f1; goto _LL839;} else{ goto
_LL840;} _LL840: if((( struct _tunion_struct*) _temp836)->tag == Cyc_Absyn_FieldName_tag){
_LL845: _temp844=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp836)->f1; goto _LL841;} else{ goto _LL837;} _LL839:( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Core_Impossible_struct* _temp846=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp846[ 0]=({ struct
Cyc_Core_Impossible_struct _temp847; _temp847.tag= Cyc_Core_Impossible_tag;
_temp847.f1=( struct _tagged_string)({ char* _temp848=( char*)"bad struct designator";
struct _tagged_string _temp849; _temp849.curr= _temp848; _temp849.base= _temp848;
_temp849.last_plus_one= _temp848 + 22; _temp849;}); _temp847;}); _temp846;}));
_LL841: ith_lvals=({ struct Cyc_List_List* _temp850=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp850->hd=( void*) Cyc_CfAbsexp_mkMemberOp((
void*)({ struct Cyc_List_List* _temp851= ls; if( _temp851 == 0){ _throw(
Null_Exception);} _temp851->hd;}),( void*)({ struct Cyc_CfFlowInfo_StructF_struct*
_temp852=( struct Cyc_CfFlowInfo_StructF_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructF_struct));
_temp852[ 0]=({ struct Cyc_CfFlowInfo_StructF_struct _temp853; _temp853.tag= Cyc_CfFlowInfo_StructF_tag;
_temp853.f1= _temp844; _temp853;}); _temp852;})); _temp850->tl= ith_lvals;
_temp850;}); goto _LL837; _LL837:;}}}}{ void** ith_topt=( void*)({ struct Cyc_Core_Opt*
_temp855=((*(( struct _tuple5*)({ struct Cyc_List_List* _temp854= _temp336; if(
_temp854 == 0){ _throw( Null_Exception);} _temp854->hd;}))).f2)->topt; if(
_temp855 == 0){ _throw( Null_Exception);}& _temp855->v;}); struct Cyc_NewControlFlow_AbsSyn
ith_syn= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_change_lhs( env,
ith_lvals),(*(( struct _tuple5*)({ struct Cyc_List_List* _temp856= _temp336; if(
_temp856 == 0){ _throw( Null_Exception);} _temp856->hd;}))).f2); void* _temp859;
void* _temp861; struct Cyc_NewControlFlow_AbsSynOne _temp857= Cyc_NewControlFlow_meet_absexp(
ith_syn); _LL862: _temp861=( void*) _temp857.inner_exp; goto _LL860; _LL860:
_temp859=( void*) _temp857.assigns; goto _LL858; _LL858: ansexp= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, ansexp, _temp861); assignexp= Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_mkAnyOrderG(), assignexp, _temp859);}} return Cyc_NewControlFlow_mkSyn_tf(
ansexp, 0, assignexp);} _LL810: { struct Cyc_NewControlFlow_AbsEnv other_env=
Cyc_NewControlFlow_change_lhs( env, 0); other_env.access_path= 0; env.access_path=
_temp814;{ void* ansexp= Cyc_CfAbsexp_mkSkipAE(); void* assignexp= Cyc_CfAbsexp_mkSkipAE();
for( 0; _temp336 != 0; _temp336=({ struct Cyc_List_List* _temp863= _temp336; if(
_temp863 == 0){ _throw( Null_Exception);} _temp863->tl;})){ int used= 0;{ struct
Cyc_List_List* ds=(*(( struct _tuple5*)({ struct Cyc_List_List* _temp864=
_temp336; if( _temp864 == 0){ _throw( Null_Exception);} _temp864->hd;}))).f1;
for( 0; ds != 0; ds=({ struct Cyc_List_List* _temp865= ds; if( _temp865 == 0){
_throw( Null_Exception);} _temp865->tl;})){ void* _temp867=( void*)({ struct Cyc_List_List*
_temp866= ds; if( _temp866 == 0){ _throw( Null_Exception);} _temp866->hd;});
struct Cyc_Absyn_Exp* _temp873; struct _tagged_string* _temp875; _LL869: if(((
struct _tunion_struct*) _temp867)->tag == Cyc_Absyn_ArrayElement_tag){ _LL874:
_temp873=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayElement_struct*)
_temp867)->f1; goto _LL870;} else{ goto _LL871;} _LL871: if((( struct
_tunion_struct*) _temp867)->tag == Cyc_Absyn_FieldName_tag){ _LL876: _temp875=(
struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp867)->f1;
goto _LL872;} else{ goto _LL868;} _LL870:( void) _throw(( struct _xtunion_struct*)({
struct Cyc_Core_Impossible_struct* _temp877=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp877[ 0]=({ struct
Cyc_Core_Impossible_struct _temp878; _temp878.tag= Cyc_Core_Impossible_tag;
_temp878.f1=( struct _tagged_string)({ char* _temp879=( char*)"bad struct designator";
struct _tagged_string _temp880; _temp880.curr= _temp879; _temp880.base= _temp879;
_temp880.last_plus_one= _temp879 + 22; _temp880;}); _temp878;}); _temp877;}));
_LL872: if( Cyc_String_zstrptrcmp( _temp823, _temp875) == 0){ used= 1;} goto
_LL868; _LL868:;}} if( used){ void* _temp884; void* _temp886; struct Cyc_NewControlFlow_AbsSynOne
_temp882= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp( env,(*((
struct _tuple5*)({ struct Cyc_List_List* _temp881= _temp336; if( _temp881 == 0){
_throw( Null_Exception);} _temp881->hd;}))).f2)); _LL887: _temp886=( void*)
_temp882.inner_exp; goto _LL885; _LL885: _temp884=( void*) _temp882.assigns;
goto _LL883; _LL883: assignexp= _temp884; ansexp= Cyc_CfAbsexp_mkGroupAE(( void*)
Cyc_CfAbsexp_UnorderedG, ansexp, _temp886);} else{ void* _temp891; void*
_temp893; struct Cyc_NewControlFlow_AbsSynOne _temp889= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( other_env,(*(( struct _tuple5*)({ struct Cyc_List_List*
_temp888= _temp336; if( _temp888 == 0){ _throw( Null_Exception);} _temp888->hd;}))).f2));
_LL894: _temp893=( void*) _temp889.inner_exp; goto _LL892; _LL892: _temp891=(
void*) _temp889.assigns; goto _LL890; _LL890: ansexp= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, ansexp, _temp893);}} return Cyc_NewControlFlow_mkSyn_tf(
ansexp, 0, assignexp);}} _LL812:( void) _throw(( struct _xtunion_struct*)({
struct Cyc_Core_Impossible_struct* _temp895=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp895[ 0]=({ struct
Cyc_Core_Impossible_struct _temp896; _temp896.tag= Cyc_Core_Impossible_tag;
_temp896.f1=( struct _tagged_string)({ char* _temp897=( char*)"abstract_exp: unexpected access path to Struct";
struct _tagged_string _temp898; _temp898.curr= _temp897; _temp898.base= _temp897;
_temp898.last_plus_one= _temp897 + 47; _temp898;}); _temp896;}); _temp895;}));
_LL806:;} _LL183: return Cyc_NewControlFlow_use_exp_unordered( env,(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)((
struct Cyc_Absyn_Exp*(*)( struct _tuple5*)) Cyc_Core_snd, _temp342), 0); _LL185:
return Cyc_NewControlFlow_use_exp_unordered( env,({ struct Cyc_List_List*
_temp899=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp899->hd=( void*) _temp346; _temp899->tl= 0; _temp899;}), 0); _LL187: {
struct Cyc_NewControlFlow_AbsSyn esyn= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env(
env), _temp348); return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*)( Cyc_NewControlFlow_meet_absexp( esyn)).inner_exp,
Cyc_CfAbsexp_mkBottomAE()), 0, Cyc_CfAbsexp_mkSkipAE());} _LL189: { struct Cyc_List_List*
_temp902; struct Cyc_NewControlFlow_AbsSynOne _temp904; struct Cyc_NewControlFlow_AbsSynOne
_temp906; struct Cyc_NewControlFlow_AbsSyn _temp900= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp354); _LL907: _temp906=( struct Cyc_NewControlFlow_AbsSynOne)
_temp900.when_true; goto _LL905; _LL905: _temp904=( struct Cyc_NewControlFlow_AbsSynOne)
_temp900.when_false; goto _LL903; _LL903: _temp902=( struct Cyc_List_List*)
_temp900.lvalues; goto _LL901; _LL901: { struct Cyc_List_List* _temp910; struct
Cyc_NewControlFlow_AbsSynOne _temp912; struct Cyc_NewControlFlow_AbsSynOne
_temp914; struct Cyc_NewControlFlow_AbsSyn _temp908= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp352); _LL915: _temp914=( struct Cyc_NewControlFlow_AbsSynOne)
_temp908.when_true; goto _LL913; _LL913: _temp912=( struct Cyc_NewControlFlow_AbsSynOne)
_temp908.when_false; goto _LL911; _LL911: _temp910=( struct Cyc_List_List*)
_temp908.lvalues; goto _LL909; _LL909: { struct Cyc_List_List* _temp918; struct
Cyc_NewControlFlow_AbsSynOne _temp920; struct Cyc_NewControlFlow_AbsSynOne
_temp922; struct Cyc_NewControlFlow_AbsSyn _temp916= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp350); _LL923: _temp922=( struct Cyc_NewControlFlow_AbsSynOne)
_temp916.when_true; goto _LL921; _LL921: _temp920=( struct Cyc_NewControlFlow_AbsSynOne)
_temp916.when_false; goto _LL919; _LL919: _temp918=( struct Cyc_List_List*)
_temp916.lvalues; goto _LL917; _LL917: { void* true_exp= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OneofG, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*) _temp906.inner_exp,(
void*) _temp906.assigns), Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*) _temp914.inner_exp,( void*) _temp914.assigns)), Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*) _temp904.inner_exp,( void*) _temp904.assigns), Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) _temp922.inner_exp,( void*) _temp922.assigns)));
void* false_exp= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OneofG, Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*) _temp906.inner_exp,( void*) _temp906.assigns), Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) _temp912.inner_exp,( void*) _temp912.assigns)),
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) _temp904.inner_exp,( void*) _temp904.assigns),
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*) _temp920.inner_exp,(
void*) _temp920.assigns))); return({ struct Cyc_NewControlFlow_AbsSyn _temp924;
_temp924.when_true=({ struct Cyc_NewControlFlow_AbsSynOne _temp926; _temp926.inner_exp=(
void*) true_exp; _temp926.assigns=( void*) Cyc_NewControlFlow_make_assigns( env,
Cyc_CfAbsexp_mkUnknownOp()); _temp926;}); _temp924.when_false=({ struct Cyc_NewControlFlow_AbsSynOne
_temp925; _temp925.inner_exp=( void*) false_exp; _temp925.assigns=( void*) Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()); _temp925;}); _temp924.lvalues= 0; _temp924;});}}}}
_LL191: { struct Cyc_NewControlFlow_AbsEnv env2= Cyc_NewControlFlow_change_lhs(
env, 0); env2.access_path= 0; Cyc_NewControlFlow_cf_prepass( env2, _temp356);{
struct Cyc_Absyn_Stmt* last_s= _temp356; while( 1) { void* _temp927=( void*)
last_s->r; struct Cyc_Absyn_Stmt* _temp937; struct Cyc_Absyn_Stmt* _temp939;
struct Cyc_Absyn_Stmt* _temp941; struct Cyc_Absyn_Decl* _temp943; struct Cyc_Absyn_Exp*
_temp945; _LL929: if(( unsigned int) _temp927 > 1u?(( struct _tunion_struct*)
_temp927)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL940: _temp939=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp927)->f1; goto _LL938; _LL938: _temp937=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp927)->f2; goto
_LL930;} else{ goto _LL931;} _LL931: if(( unsigned int) _temp927 > 1u?(( struct
_tunion_struct*) _temp927)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL944: _temp943=(
struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*) _temp927)->f1; goto
_LL942; _LL942: _temp941=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp927)->f2; goto _LL932;} else{ goto _LL933;} _LL933: if(( unsigned int)
_temp927 > 1u?(( struct _tunion_struct*) _temp927)->tag == Cyc_Absyn_Exp_s_tag:
0){ _LL946: _temp945=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp927)->f1; goto _LL934;} else{ goto _LL935;} _LL935: goto _LL936; _LL930:
last_s= _temp937; continue; _LL932: last_s= _temp941; continue; _LL934: { struct
Cyc_NewControlFlow_AbsSyn syn= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env(
env), _temp945); void* _temp949; void* _temp951; struct Cyc_NewControlFlow_AbsSynOne
_temp947= Cyc_NewControlFlow_meet_absexp( syn); _LL952: _temp951=( void*)
_temp947.inner_exp; goto _LL950; _LL950: _temp949=( void*) _temp947.assigns;
goto _LL948; _LL948:( void*)({ struct Cyc_List_List* _temp953=( Cyc_NewControlFlow_get_stmt_annot(
last_s))->absexps; if( _temp953 == 0){ _throw( Null_Exception);} _temp953->hd=(
void*) Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp951, _temp949);});
return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkStmtAE( _temp356), 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));} _LL936:( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Core_Impossible_struct* _temp954=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp954[ 0]=({ struct
Cyc_Core_Impossible_struct _temp955; _temp955.tag= Cyc_Core_Impossible_tag;
_temp955.f1=( struct _tagged_string)({ char* _temp956=( char*)"abstract_exp: ill-formed StmtExp";
struct _tagged_string _temp957; _temp957.curr= _temp956; _temp957.base= _temp956;
_temp957.last_plus_one= _temp956 + 33; _temp957;}); _temp955;}); _temp954;}));
_LL928:;}}} _LL193: { struct Cyc_NewControlFlow_AbsEnv env1= Cyc_NewControlFlow_change_lhs(
env, 0); env1.access_path= 0;{ struct Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp(
env1, _temp360); struct Cyc_NewControlFlow_AbsSyn syn2= Cyc_NewControlFlow_abstract_exp(
env, _temp358); struct Cyc_NewControlFlow_AbsSynOne syn1meet= Cyc_NewControlFlow_meet_absexp(
syn1); return({ struct Cyc_NewControlFlow_AbsSyn _temp958; _temp958.when_true=({
struct Cyc_NewControlFlow_AbsSynOne _temp960; _temp960.inner_exp=( void*) Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) syn1meet.inner_exp,( void*)( syn2.when_true).inner_exp);
_temp960.assigns=( void*)(( void*)( syn2.when_true).assigns); _temp960;});
_temp958.when_false=({ struct Cyc_NewControlFlow_AbsSynOne _temp959; _temp959.inner_exp=(
void*) Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*) syn1meet.inner_exp,(
void*)( syn2.when_false).inner_exp); _temp959.assigns=( void*)(( void*)( syn2.when_false).assigns);
_temp959;}); _temp958.lvalues= 0; _temp958;});}} _LL195: env.access_path == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp961=( char*)"new_control_flow.cyc:746 env.access_path==null";
struct _tagged_string _temp962; _temp962.curr= _temp961; _temp962.base= _temp961;
_temp962.last_plus_one= _temp961 + 47; _temp962;})); Cyc_NewControlFlow_add_var_root(
env, _temp366);( env.shared)->comprehension_vars=({ struct Cyc_List_List*
_temp963=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp963->hd=( void*) _temp366; _temp963->tl=( env.shared)->comprehension_vars;
_temp963;});{ struct Cyc_NewControlFlow_AbsEnv elt_env= Cyc_NewControlFlow_use_it_env(
env); void* _temp966; void* _temp968; struct Cyc_NewControlFlow_AbsSynOne
_temp964= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
elt_env, _temp364)); _LL969: _temp968=( void*) _temp964.inner_exp; goto _LL967;
_LL967: _temp966=( void*) _temp964.assigns; goto _LL965; _LL965: { void*
_temp972; void* _temp974; struct Cyc_NewControlFlow_AbsSynOne _temp970= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( elt_env, _temp362)); _LL975: _temp974=( void*)
_temp970.inner_exp; goto _LL973; _LL973: _temp972=( void*) _temp970.assigns;
goto _LL971; _LL971: { void* ansexp= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
_temp968, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp966, Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkAssignAE( Cyc_CfAbsexp_mkLocalOp(
_temp366), Cyc_CfAbsexp_mkUnknownOp()), Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OneofG,
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp974, _temp972), Cyc_CfAbsexp_mkSkipAE()))));
return Cyc_NewControlFlow_mkSyn_tf( ansexp, 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));}}} _LL197: goto _LL199; _LL199: goto _LL201;
_LL201: goto _LL203; _LL203: goto _LL205; _LL205: goto _LL207; _LL207: goto
_LL209; _LL209:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp976=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp976[ 0]=({ struct Cyc_Core_Impossible_struct _temp977; _temp977.tag= Cyc_Core_Impossible_tag;
_temp977.f1=( struct _tagged_string)({ char* _temp978=( char*)"abstract_exp, unexpected exp form";
struct _tagged_string _temp979; _temp979.curr= _temp978; _temp979.base= _temp978;
_temp979.last_plus_one= _temp978 + 34; _temp979;}); _temp977;}); _temp976;}));
_LL123:;} void* Cyc_NewControlFlow_abstract_exp_top( struct Cyc_NewControlFlow_AbsEnv
env, struct Cyc_Absyn_Exp* e){ void* _temp982; void* _temp984; struct Cyc_NewControlFlow_AbsSynOne
_temp980= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp( env,
e)); _LL985: _temp984=( void*) _temp980.inner_exp; goto _LL983; _LL983: _temp982=(
void*) _temp980.assigns; goto _LL981; _LL981: return Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, _temp984, _temp982);} struct _tuple0 Cyc_NewControlFlow_abstract_guard(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e){ struct Cyc_NewControlFlow_AbsSyn
syn= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env( env), e);
return({ struct _tuple0 _temp986; _temp986.f1= Cyc_CfAbsexp_mkGroupAE(( void*)
Cyc_CfAbsexp_OrderedG,( void*)( syn.when_true).inner_exp,( void*)( syn.when_true).assigns);
_temp986.f2= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*)( syn.when_false).inner_exp,(
void*)( syn.when_false).assigns); _temp986;});} struct Cyc_List_List* Cyc_NewControlFlow_cf_prepass_scs(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_List_List* scs){ struct Cyc_List_List*
ans= 0; for( 0; scs != 0; scs=({ struct Cyc_List_List* _temp987= scs; if(
_temp987 == 0){ _throw( Null_Exception);} _temp987->tl;})){ struct Cyc_Absyn_Switch_clause
_temp991; struct Cyc_Position_Segment* _temp992; struct Cyc_Absyn_Stmt* _temp994;
struct Cyc_Absyn_Exp* _temp996; struct Cyc_Core_Opt* _temp998; struct Cyc_Absyn_Pat*
_temp1000; struct Cyc_Absyn_Switch_clause* _temp989=( struct Cyc_Absyn_Switch_clause*)({
struct Cyc_List_List* _temp988= scs; if( _temp988 == 0){ _throw( Null_Exception);}
_temp988->hd;}); _temp991=* _temp989; _LL1001: _temp1000=( struct Cyc_Absyn_Pat*)
_temp991.pattern; goto _LL999; _LL999: _temp998=( struct Cyc_Core_Opt*) _temp991.pat_vars;
goto _LL997; _LL997: _temp996=( struct Cyc_Absyn_Exp*) _temp991.where_clause;
goto _LL995; _LL995: _temp994=( struct Cyc_Absyn_Stmt*) _temp991.body; goto
_LL993; _LL993: _temp992=( struct Cyc_Position_Segment*) _temp991.loc; goto
_LL990; _LL990: if( _temp998 == 0){( void) _throw(( struct _xtunion_struct*)({
struct Cyc_Core_Impossible_struct* _temp1002=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp1002[ 0]=({ struct
Cyc_Core_Impossible_struct _temp1003; _temp1003.tag= Cyc_Core_Impossible_tag;
_temp1003.f1=( struct _tagged_string)({ char* _temp1004=( char*)"switch clause vds not set";
struct _tagged_string _temp1005; _temp1005.curr= _temp1004; _temp1005.base=
_temp1004; _temp1005.last_plus_one= _temp1004 + 26; _temp1005;}); _temp1003;});
_temp1002;}));}{ struct Cyc_List_List* vds=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp1006= _temp998; if( _temp1006 == 0){ _throw( Null_Exception);} _temp1006->v;});
for( 0; vds != 0; vds=({ struct Cyc_List_List* _temp1007= vds; if( _temp1007 ==
0){ _throw( Null_Exception);} _temp1007->tl;})){ Cyc_NewControlFlow_add_var_root(
env,( struct Cyc_Absyn_Vardecl*)({ struct Cyc_List_List* _temp1008= vds; if(
_temp1008 == 0){ _throw( Null_Exception);} _temp1008->hd;}));}} if( _temp996 !=
0){ void* _temp1012; void* _temp1014; struct _tuple0 _temp1010= Cyc_NewControlFlow_abstract_guard(
env,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1009= _temp996; if(
_temp1009 == 0){ _throw( Null_Exception);} _temp1009;})); _LL1015: _temp1014=
_temp1010.f1; goto _LL1013; _LL1013: _temp1012= _temp1010.f2; goto _LL1011;
_LL1011: ans=({ struct Cyc_List_List* _temp1016=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1016->hd=( void*) _temp1014;
_temp1016->tl=({ struct Cyc_List_List* _temp1017=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1017->hd=( void*) _temp1012;
_temp1017->tl= ans; _temp1017;}); _temp1016;});} Cyc_NewControlFlow_cf_prepass(
env, _temp994);} return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( ans);} void Cyc_NewControlFlow_cf_prepass( struct Cyc_NewControlFlow_AbsEnv
env, struct Cyc_Absyn_Stmt* s){ env.lhsides == 0? 0:(( int(*)( struct
_tagged_string msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({ char*
_temp1018=( char*)"new_control_flow.cyc:798 env.lhsides == null"; struct
_tagged_string _temp1019; _temp1019.curr= _temp1018; _temp1019.base= _temp1018;
_temp1019.last_plus_one= _temp1018 + 45; _temp1019;})); env.access_path == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp1020=( char*)"new_control_flow.cyc:799 env.access_path == null";
struct _tagged_string _temp1021; _temp1021.curr= _temp1020; _temp1021.base=
_temp1020; _temp1021.last_plus_one= _temp1020 + 49; _temp1021;}));{ void*
_temp1022=( void*) s->r; struct Cyc_Absyn_Exp* _temp1068; struct Cyc_Absyn_Stmt*
_temp1070; struct Cyc_Absyn_Stmt* _temp1072; struct Cyc_Absyn_Exp* _temp1074;
struct Cyc_Absyn_Exp* _temp1076; struct Cyc_Absyn_Stmt* _temp1078; struct Cyc_Absyn_Stmt*
_temp1080; struct Cyc_Absyn_Exp* _temp1082; struct _tuple3 _temp1084; struct Cyc_Absyn_Stmt*
_temp1086; struct Cyc_Absyn_Exp* _temp1088; struct Cyc_Absyn_Stmt* _temp1090;
struct Cyc_Absyn_Stmt* _temp1092; struct _tuple3 _temp1094; struct Cyc_Absyn_Stmt*
_temp1096; struct Cyc_Absyn_Exp* _temp1098; struct Cyc_Absyn_Stmt* _temp1100;
struct Cyc_Absyn_Stmt* _temp1102; struct Cyc_Absyn_Stmt* _temp1104; struct
_tagged_string* _temp1106; struct Cyc_Absyn_Stmt* _temp1108; struct
_tagged_string* _temp1110; struct Cyc_Absyn_Stmt* _temp1112; struct _tuple3
_temp1114; struct Cyc_Absyn_Stmt* _temp1116; struct Cyc_Absyn_Exp* _temp1118;
struct _tuple3 _temp1120; struct Cyc_Absyn_Stmt* _temp1122; struct Cyc_Absyn_Exp*
_temp1124; struct Cyc_Absyn_Exp* _temp1126; struct Cyc_List_List* _temp1128;
struct Cyc_Absyn_Exp* _temp1130; struct Cyc_Absyn_Switch_clause** _temp1132;
struct Cyc_List_List* _temp1134; struct Cyc_Absyn_Stmt* _temp1136; struct Cyc_Absyn_Decl*
_temp1138; struct Cyc_Absyn_Decl _temp1140; struct Cyc_Position_Segment*
_temp1141; void* _temp1143; struct Cyc_Absyn_Vardecl* _temp1145; struct Cyc_Absyn_Stmt*
_temp1147; struct Cyc_Absyn_Decl* _temp1149; struct Cyc_Absyn_Decl _temp1151;
struct Cyc_Position_Segment* _temp1152; void* _temp1154; int _temp1156; struct
Cyc_Absyn_Exp* _temp1158; struct Cyc_Core_Opt* _temp1160; struct Cyc_Core_Opt*
_temp1162; struct Cyc_Core_Opt _temp1164; struct Cyc_List_List* _temp1165;
struct Cyc_Absyn_Pat* _temp1167; struct Cyc_Absyn_Stmt* _temp1169; struct Cyc_Absyn_Decl*
_temp1171; struct Cyc_Absyn_Stmt* _temp1173; struct Cyc_Absyn_Stmt* _temp1175;
struct Cyc_Absyn_Stmt* _temp1177; struct _tagged_string* _temp1179; struct Cyc_List_List*
_temp1181; struct Cyc_Absyn_Stmt* _temp1183; _LL1024: if(( int) _temp1022 == Cyc_Absyn_Skip_s){
goto _LL1025;} else{ goto _LL1026;} _LL1026: if(( unsigned int) _temp1022 > 1u?((
struct _tunion_struct*) _temp1022)->tag == Cyc_Absyn_Exp_s_tag: 0){ _LL1069:
_temp1068=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*) _temp1022)->f1;
goto _LL1027;} else{ goto _LL1028;} _LL1028: if(( unsigned int) _temp1022 > 1u?((
struct _tunion_struct*) _temp1022)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL1073:
_temp1072=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1022)->f1;
goto _LL1071; _LL1071: _temp1070=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp1022)->f2; goto _LL1029;} else{ goto _LL1030;} _LL1030: if(( unsigned int)
_temp1022 > 1u?(( struct _tunion_struct*) _temp1022)->tag == Cyc_Absyn_Return_s_tag:
0){ _LL1075: _temp1074=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp1022)->f1; if( _temp1074 == 0){ goto _LL1031;} else{ goto _LL1032;}} else{
goto _LL1032;} _LL1032: if(( unsigned int) _temp1022 > 1u?(( struct
_tunion_struct*) _temp1022)->tag == Cyc_Absyn_Return_s_tag: 0){ _LL1077:
_temp1076=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp1022)->f1; goto _LL1033;} else{ goto _LL1034;} _LL1034: if(( unsigned int)
_temp1022 > 1u?(( struct _tunion_struct*) _temp1022)->tag == Cyc_Absyn_IfThenElse_s_tag:
0){ _LL1083: _temp1082=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1022)->f1; goto _LL1081; _LL1081: _temp1080=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1022)->f2; goto _LL1079; _LL1079:
_temp1078=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1022)->f3; goto _LL1035;} else{ goto _LL1036;} _LL1036: if(( unsigned int)
_temp1022 > 1u?(( struct _tunion_struct*) _temp1022)->tag == Cyc_Absyn_Do_s_tag:
0){ _LL1091: _temp1090=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*)
_temp1022)->f1; goto _LL1085; _LL1085: _temp1084=( struct _tuple3)(( struct Cyc_Absyn_Do_s_struct*)
_temp1022)->f2; _LL1089: _temp1088= _temp1084.f1; goto _LL1087; _LL1087:
_temp1086= _temp1084.f2; goto _LL1037;} else{ goto _LL1038;} _LL1038: if((
unsigned int) _temp1022 > 1u?(( struct _tunion_struct*) _temp1022)->tag == Cyc_Absyn_While_s_tag:
0){ _LL1095: _temp1094=( struct _tuple3)(( struct Cyc_Absyn_While_s_struct*)
_temp1022)->f1; _LL1099: _temp1098= _temp1094.f1; goto _LL1097; _LL1097:
_temp1096= _temp1094.f2; goto _LL1093; _LL1093: _temp1092=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_While_s_struct*) _temp1022)->f2; goto _LL1039;} else{ goto
_LL1040;} _LL1040: if(( unsigned int) _temp1022 > 1u?(( struct _tunion_struct*)
_temp1022)->tag == Cyc_Absyn_Break_s_tag: 0){ _LL1101: _temp1100=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Break_s_struct*) _temp1022)->f1; goto _LL1041;} else{ goto
_LL1042;} _LL1042: if(( unsigned int) _temp1022 > 1u?(( struct _tunion_struct*)
_temp1022)->tag == Cyc_Absyn_Continue_s_tag: 0){ _LL1103: _temp1102=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Continue_s_struct*) _temp1022)->f1; goto _LL1043;} else{ goto
_LL1044;} _LL1044: if(( unsigned int) _temp1022 > 1u?(( struct _tunion_struct*)
_temp1022)->tag == Cyc_Absyn_Goto_s_tag: 0){ _LL1107: _temp1106=( struct
_tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp1022)->f1; goto _LL1105;
_LL1105: _temp1104=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp1022)->f2; if( _temp1104 == 0){ goto _LL1045;} else{ goto _LL1046;}} else{
goto _LL1046;} _LL1046: if(( unsigned int) _temp1022 > 1u?(( struct
_tunion_struct*) _temp1022)->tag == Cyc_Absyn_Goto_s_tag: 0){ _LL1111: _temp1110=(
struct _tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp1022)->f1; goto
_LL1109; _LL1109: _temp1108=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp1022)->f2; goto _LL1047;} else{ goto _LL1048;} _LL1048: if(( unsigned int)
_temp1022 > 1u?(( struct _tunion_struct*) _temp1022)->tag == Cyc_Absyn_For_s_tag:
0){ _LL1127: _temp1126=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp1022)->f1; goto _LL1121; _LL1121: _temp1120=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp1022)->f2; _LL1125: _temp1124= _temp1120.f1; goto _LL1123; _LL1123:
_temp1122= _temp1120.f2; goto _LL1115; _LL1115: _temp1114=( struct _tuple3)((
struct Cyc_Absyn_For_s_struct*) _temp1022)->f3; _LL1119: _temp1118= _temp1114.f1;
goto _LL1117; _LL1117: _temp1116= _temp1114.f2; goto _LL1113; _LL1113: _temp1112=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp1022)->f4; goto
_LL1049;} else{ goto _LL1050;} _LL1050: if(( unsigned int) _temp1022 > 1u?((
struct _tunion_struct*) _temp1022)->tag == Cyc_Absyn_Switch_s_tag: 0){ _LL1131:
_temp1130=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp1022)->f1; goto _LL1129; _LL1129: _temp1128=( struct Cyc_List_List*)((
struct Cyc_Absyn_Switch_s_struct*) _temp1022)->f2; goto _LL1051;} else{ goto
_LL1052;} _LL1052: if(( unsigned int) _temp1022 > 1u?(( struct _tunion_struct*)
_temp1022)->tag == Cyc_Absyn_Fallthru_s_tag: 0){ _LL1135: _temp1134=( struct Cyc_List_List*)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1022)->f1; goto _LL1133; _LL1133:
_temp1132=( struct Cyc_Absyn_Switch_clause**)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1022)->f2; goto _LL1053;} else{ goto _LL1054;} _LL1054: if(( unsigned int)
_temp1022 > 1u?(( struct _tunion_struct*) _temp1022)->tag == Cyc_Absyn_Decl_s_tag:
0){ _LL1139: _temp1138=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1022)->f1; _temp1140=* _temp1138; _LL1144: _temp1143=( void*) _temp1140.r;
if((( struct _tunion_struct*) _temp1143)->tag == Cyc_Absyn_Var_d_tag){ _LL1146:
_temp1145=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp1143)->f1; goto _LL1142;} else{ goto _LL1056;} _LL1142: _temp1141=( struct
Cyc_Position_Segment*) _temp1140.loc; goto _LL1137; _LL1137: _temp1136=( struct
Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1022)->f2; goto _LL1055;}
else{ goto _LL1056;} _LL1056: if(( unsigned int) _temp1022 > 1u?(( struct
_tunion_struct*) _temp1022)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL1150: _temp1149=(
struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1022)->f1;
_temp1151=* _temp1149; _LL1155: _temp1154=( void*) _temp1151.r; if((( struct
_tunion_struct*) _temp1154)->tag == Cyc_Absyn_Let_d_tag){ _LL1168: _temp1167=(
struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*) _temp1154)->f1; goto
_LL1163; _LL1163: _temp1162=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1154)->f2; if( _temp1162 == 0){ goto _LL1058;} else{ _temp1164=* _temp1162;
_LL1166: _temp1165=( struct Cyc_List_List*) _temp1164.v; goto _LL1161;} _LL1161:
_temp1160=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp1154)->f3;
goto _LL1159; _LL1159: _temp1158=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1154)->f4; goto _LL1157; _LL1157: _temp1156=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp1154)->f5; goto _LL1153;} else{ goto _LL1058;} _LL1153: _temp1152=( struct
Cyc_Position_Segment*) _temp1151.loc; goto _LL1148; _LL1148: _temp1147=( struct
Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1022)->f2; goto _LL1057;}
else{ goto _LL1058;} _LL1058: if(( unsigned int) _temp1022 > 1u?(( struct
_tunion_struct*) _temp1022)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL1172: _temp1171=(
struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1022)->f1; goto
_LL1170; _LL1170: _temp1169=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1022)->f2; goto _LL1059;} else{ goto _LL1060;} _LL1060: if(( unsigned int)
_temp1022 > 1u?(( struct _tunion_struct*) _temp1022)->tag == Cyc_Absyn_Cut_s_tag:
0){ _LL1174: _temp1173=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Cut_s_struct*)
_temp1022)->f1; goto _LL1061;} else{ goto _LL1062;} _LL1062: if(( unsigned int)
_temp1022 > 1u?(( struct _tunion_struct*) _temp1022)->tag == Cyc_Absyn_Splice_s_tag:
0){ _LL1176: _temp1175=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Splice_s_struct*)
_temp1022)->f1; goto _LL1063;} else{ goto _LL1064;} _LL1064: if(( unsigned int)
_temp1022 > 1u?(( struct _tunion_struct*) _temp1022)->tag == Cyc_Absyn_Label_s_tag:
0){ _LL1180: _temp1179=( struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*)
_temp1022)->f1; goto _LL1178; _LL1178: _temp1177=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Label_s_struct*) _temp1022)->f2; goto _LL1065;} else{ goto
_LL1066;} _LL1066: if(( unsigned int) _temp1022 > 1u?(( struct _tunion_struct*)
_temp1022)->tag == Cyc_Absyn_TryCatch_s_tag: 0){ _LL1184: _temp1183=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp1022)->f1; goto _LL1182; _LL1182:
_temp1181=( struct Cyc_List_List*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1022)->f2; goto _LL1067;} else{ goto _LL1023;} _LL1025: return; _LL1027: {
void* ae= Cyc_NewControlFlow_abstract_exp_top( env, _temp1068); Cyc_NewControlFlow_cf_set_absexps(
s,({ struct Cyc_List_List* _temp1185=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1185->hd=( void*) ae; _temp1185->tl= 0; _temp1185;}));
return;} _LL1029: Cyc_NewControlFlow_cf_prepass( env, _temp1072); Cyc_NewControlFlow_cf_prepass(
env, _temp1070); return; _LL1031: return; _LL1033: { void* ae= Cyc_NewControlFlow_abstract_exp_top(
Cyc_NewControlFlow_use_it_env( env),( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1186= _temp1076; if( _temp1186 == 0){ _throw( Null_Exception);} _temp1186;}));
Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List* _temp1187=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1187->hd=( void*)
ae; _temp1187->tl= 0; _temp1187;})); return;} _LL1035: { void* _temp1190; void*
_temp1192; struct _tuple0 _temp1188= Cyc_NewControlFlow_abstract_guard( env,
_temp1082); _LL1193: _temp1192= _temp1188.f1; goto _LL1191; _LL1191: _temp1190=
_temp1188.f2; goto _LL1189; _LL1189: Cyc_NewControlFlow_cf_set_absexps( s,({
struct Cyc_List_List* _temp1194=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1194->hd=( void*) _temp1192; _temp1194->tl=({
struct Cyc_List_List* _temp1195=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1195->hd=( void*) _temp1190; _temp1195->tl= 0;
_temp1195;}); _temp1194;})); Cyc_NewControlFlow_cf_prepass( env, _temp1080); Cyc_NewControlFlow_cf_prepass(
env, _temp1078); return;} _LL1037: _temp1098= _temp1088; _temp1096= _temp1086;
_temp1092= _temp1090; goto _LL1039; _LL1039: { void* _temp1198; void* _temp1200;
struct _tuple0 _temp1196= Cyc_NewControlFlow_abstract_guard( env, _temp1098);
_LL1201: _temp1200= _temp1196.f1; goto _LL1199; _LL1199: _temp1198= _temp1196.f2;
goto _LL1197; _LL1197: Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1202=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1202->hd=( void*) _temp1200; _temp1202->tl=({ struct Cyc_List_List*
_temp1203=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1203->hd=( void*) _temp1198; _temp1203->tl= 0; _temp1203;}); _temp1202;}));
Cyc_NewControlFlow_cf_prepass( env, _temp1092); return;} _LL1041: return;
_LL1043: return; _LL1045:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp1204=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1204[ 0]=({ struct Cyc_Core_Impossible_struct _temp1205; _temp1205.tag= Cyc_Core_Impossible_tag;
_temp1205.f1=( struct _tagged_string)({ char* _temp1206=( char*)"cf_prepass: goto w/o destination";
struct _tagged_string _temp1207; _temp1207.curr= _temp1206; _temp1207.base=
_temp1206; _temp1207.last_plus_one= _temp1206 + 33; _temp1207;}); _temp1205;});
_temp1204;})); _LL1047: { struct Cyc_Absyn_Stmt* my_encloser=( Cyc_NewControlFlow_get_stmt_annot(
s))->encloser; struct Cyc_Absyn_Stmt* dest_encloser=( Cyc_NewControlFlow_get_stmt_annot((
struct Cyc_Absyn_Stmt*)({ struct Cyc_Absyn_Stmt* _temp1208= _temp1108; if(
_temp1208 == 0){ _throw( Null_Exception);} _temp1208;})))->encloser; while(
dest_encloser != my_encloser) { struct Cyc_Absyn_Stmt* next_encloser=( Cyc_NewControlFlow_get_stmt_annot(
my_encloser))->encloser; if( next_encloser == my_encloser){ Cyc_Tcutil_terr( s->loc,(
struct _tagged_string)({ char* _temp1209=( char*)"goto enters local scope or exception handler";
struct _tagged_string _temp1210; _temp1210.curr= _temp1209; _temp1210.base=
_temp1209; _temp1210.last_plus_one= _temp1209 + 45; _temp1210;})); break;}
my_encloser= next_encloser;} return;} _LL1049: { void* ae1= Cyc_NewControlFlow_abstract_exp_top(
env, _temp1126); void* _temp1213; void* _temp1215; struct _tuple0 _temp1211= Cyc_NewControlFlow_abstract_guard(
env, _temp1124); _LL1216: _temp1215= _temp1211.f1; goto _LL1214; _LL1214:
_temp1213= _temp1211.f2; goto _LL1212; _LL1212: { void* ae3= Cyc_NewControlFlow_abstract_exp_top(
env, _temp1118); Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1217=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1217->hd=( void*) ae1; _temp1217->tl=({ struct Cyc_List_List* _temp1218=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1218->hd=(
void*) _temp1215; _temp1218->tl=({ struct Cyc_List_List* _temp1219=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1219->hd=( void*) _temp1213;
_temp1219->tl=({ struct Cyc_List_List* _temp1220=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1220->hd=( void*) ae3; _temp1220->tl=
0; _temp1220;}); _temp1219;}); _temp1218;}); _temp1217;})); Cyc_NewControlFlow_cf_prepass(
env, _temp1112); return;}} _LL1051: { void* ae= Cyc_NewControlFlow_abstract_exp_top(
Cyc_NewControlFlow_use_it_env( env), _temp1130); struct Cyc_List_List* ael= Cyc_NewControlFlow_cf_prepass_scs(
env, _temp1128); Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1221=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1221->hd=( void*) ae; _temp1221->tl= ael; _temp1221;})); return;} _LL1053: {
void* _temp1224; void* _temp1226; struct Cyc_NewControlFlow_AbsSynOne _temp1222=
Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_use_exp_unordered( env,
_temp1134, 0)); _LL1227: _temp1226=( void*) _temp1222.inner_exp; goto _LL1225;
_LL1225: _temp1224=( void*) _temp1222.assigns; goto _LL1223; _LL1223: { void* ae=
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp1226, _temp1224);
Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List* _temp1228=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1228->hd=( void*)
ae; _temp1228->tl= 0; _temp1228;})); return;}} _LL1055: Cyc_NewControlFlow_add_var_root(
env, _temp1145); if( _temp1145->initializer != 0){ struct Cyc_NewControlFlow_AbsEnv
init_env= Cyc_NewControlFlow_change_lhs( env,({ struct Cyc_List_List* _temp1229=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1229->hd=(
void*) Cyc_CfAbsexp_mkLocalOp( _temp1145); _temp1229->tl= 0; _temp1229;})); void*
ae= Cyc_NewControlFlow_abstract_exp_top( init_env,( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1230= _temp1145->initializer; if( _temp1230 == 0){
_throw( Null_Exception);} _temp1230;})); Cyc_NewControlFlow_cf_set_absexps( s,({
struct Cyc_List_List* _temp1231=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1231->hd=( void*) ae; _temp1231->tl= 0; _temp1231;}));}
Cyc_NewControlFlow_cf_prepass( env, _temp1136); return; _LL1057: for( 0;
_temp1165 != 0; _temp1165=({ struct Cyc_List_List* _temp1232= _temp1165; if(
_temp1232 == 0){ _throw( Null_Exception);} _temp1232->tl;})){ Cyc_NewControlFlow_add_var_root(
env,( struct Cyc_Absyn_Vardecl*)({ struct Cyc_List_List* _temp1233= _temp1165;
if( _temp1233 == 0){ _throw( Null_Exception);} _temp1233->hd;}));}{ void* ae=
Cyc_NewControlFlow_abstract_exp_top( Cyc_NewControlFlow_use_it_env( env),
_temp1158); Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1234=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1234->hd=( void*) ae; _temp1234->tl= 0; _temp1234;})); Cyc_NewControlFlow_cf_prepass(
env, _temp1147); return;} _LL1059: goto _LL1061; _LL1061: goto _LL1063; _LL1063:(
void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp1235=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1235[ 0]=({ struct Cyc_Core_Impossible_struct _temp1236; _temp1236.tag= Cyc_Core_Impossible_tag;
_temp1236.f1=( struct _tagged_string)({ char* _temp1237=( char*)"cf_prepass: bad stmt form";
struct _tagged_string _temp1238; _temp1238.curr= _temp1237; _temp1238.base=
_temp1237; _temp1238.last_plus_one= _temp1237 + 26; _temp1238;}); _temp1236;});
_temp1235;})); _LL1065: Cyc_NewControlFlow_cf_prepass( env, _temp1177); return;
_LL1067: Cyc_NewControlFlow_cf_prepass( env, _temp1183);{ struct Cyc_List_List*
ael= Cyc_NewControlFlow_cf_prepass_scs( env, _temp1181); Cyc_NewControlFlow_cf_set_absexps(
s, ael); return;} _LL1023:;}} static int Cyc_NewControlFlow_iterate_cf_check= 0;
static int Cyc_NewControlFlow_iteration_num= 0; struct Cyc_NewControlFlow_AnalEnv;
void Cyc_NewControlFlow_update_tryflow( struct Cyc_NewControlFlow_AnalEnv* env,
void* new_flow){ if( env->in_try){( void*)( env->tryflow=( void*) Cyc_CfFlowInfo_join_flow(
new_flow,( void*) env->tryflow));}} static struct Cyc_NewControlFlow_CFStmtAnnot*
Cyc_NewControlFlow_pre_stmt_check( void* in_flow, struct Cyc_Absyn_Stmt* s){
struct Cyc_NewControlFlow_CFStmtAnnot* annot= Cyc_NewControlFlow_get_stmt_annot(
s);( void*)( annot->flow=( void*) Cyc_CfFlowInfo_join_flow( in_flow,( void*)
annot->flow)); ++ annot->visited; annot->visited == Cyc_NewControlFlow_iteration_num?
0:(( int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp1239=( char*)"new_control_flow.cyc:927 annot->visited == iteration_num";
struct _tagged_string _temp1240; _temp1240.curr= _temp1239; _temp1240.base=
_temp1239; _temp1240.last_plus_one= _temp1239 + 57; _temp1240;})); return annot;}
static void Cyc_NewControlFlow_update_flow( struct Cyc_Absyn_Stmt* s, void* flow){
struct Cyc_NewControlFlow_CFStmtAnnot* annot= Cyc_NewControlFlow_get_stmt_annot(
s); void* new_flow= Cyc_CfFlowInfo_join_flow( flow,( void*) annot->flow); if( !
Cyc_CfFlowInfo_flow_lessthan_approx( new_flow,( void*) annot->flow)){( void*)(
annot->flow=( void*) new_flow); if( annot->visited == Cyc_NewControlFlow_iteration_num){
Cyc_NewControlFlow_iterate_cf_check= 1;}}} static void* Cyc_NewControlFlow_add_init_vars_flow(
struct Cyc_NewControlFlow_AnalEnv* env, void* flow, struct Cyc_List_List* vds){
void* _temp1241= flow; struct Cyc_Dict_Dict* _temp1247; _LL1243: if(( int)
_temp1241 == Cyc_CfFlowInfo_BottomFL){ goto _LL1244;} else{ goto _LL1245;}
_LL1245: if(( unsigned int) _temp1241 > 1u?(( struct _tunion_struct*) _temp1241)->tag
== Cyc_CfFlowInfo_InitsFL_tag: 0){ _LL1248: _temp1247=( struct Cyc_Dict_Dict*)((
struct Cyc_CfFlowInfo_InitsFL_struct*) _temp1241)->f1; goto _LL1246;} else{ goto
_LL1242;} _LL1244: return( void*) Cyc_CfFlowInfo_BottomFL; _LL1246: for( 0; vds
!= 0; vds=({ struct Cyc_List_List* _temp1249= vds; if( _temp1249 == 0){ _throw(
Null_Exception);} _temp1249->tl;})){ struct Cyc_CfFlowInfo_VarRoot_struct* root=({
struct Cyc_CfFlowInfo_VarRoot_struct* _temp1250=( struct Cyc_CfFlowInfo_VarRoot_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct)); _temp1250[ 0]=({
struct Cyc_CfFlowInfo_VarRoot_struct _temp1251; _temp1251.tag= Cyc_CfFlowInfo_VarRoot_tag;
_temp1251.f1=( struct Cyc_Absyn_Vardecl*)({ struct Cyc_List_List* _temp1252= vds;
if( _temp1252 == 0){ _throw( Null_Exception);} _temp1252->hd;}); _temp1251;});
_temp1250;}); void* pinfo= Cyc_CfFlowInfo_assign_unknown_dict(( void*) Cyc_CfFlowInfo_AllIL,((
void*(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( env->roots,(
void*) root)); _temp1247=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
void* key, void* data)) Cyc_Dict_insert)( _temp1247,( void*) root, pinfo);}
return( void*)({ struct Cyc_CfFlowInfo_InitsFL_struct* _temp1253=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp1253[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp1254; _temp1254.tag= Cyc_CfFlowInfo_InitsFL_tag;
_temp1254.f1= _temp1247; _temp1254;}); _temp1253;}); _LL1242:;} static void* Cyc_NewControlFlow_add_vardecls_flow(
struct Cyc_NewControlFlow_AnalEnv* env, void* flow, struct Cyc_List_List* vds){
void* _temp1255= flow; struct Cyc_Dict_Dict* _temp1261; _LL1257: if(( int)
_temp1255 == Cyc_CfFlowInfo_BottomFL){ goto _LL1258;} else{ goto _LL1259;}
_LL1259: if(( unsigned int) _temp1255 > 1u?(( struct _tunion_struct*) _temp1255)->tag
== Cyc_CfFlowInfo_InitsFL_tag: 0){ _LL1262: _temp1261=( struct Cyc_Dict_Dict*)((
struct Cyc_CfFlowInfo_InitsFL_struct*) _temp1255)->f1; goto _LL1260;} else{ goto
_LL1256;} _LL1258: return( void*) Cyc_CfFlowInfo_BottomFL; _LL1260: for( 0; vds
!= 0; vds=({ struct Cyc_List_List* _temp1263= vds; if( _temp1263 == 0){ _throw(
Null_Exception);} _temp1263->tl;})){ struct Cyc_CfFlowInfo_VarRoot_struct* root=({
struct Cyc_CfFlowInfo_VarRoot_struct* _temp1264=( struct Cyc_CfFlowInfo_VarRoot_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct)); _temp1264[ 0]=({
struct Cyc_CfFlowInfo_VarRoot_struct _temp1265; _temp1265.tag= Cyc_CfFlowInfo_VarRoot_tag;
_temp1265.f1=( struct Cyc_Absyn_Vardecl*)({ struct Cyc_List_List* _temp1266= vds;
if( _temp1266 == 0){ _throw( Null_Exception);} _temp1266->hd;}); _temp1265;});
_temp1264;}); _temp1261=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
void* key, void* data)) Cyc_Dict_insert)( _temp1261,( void*) root,(( void*(*)(
struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( env->roots,( void*) root));}
return( void*)({ struct Cyc_CfFlowInfo_InitsFL_struct* _temp1267=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp1267[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp1268; _temp1268.tag= Cyc_CfFlowInfo_InitsFL_tag;
_temp1268.f1= _temp1261; _temp1268;}); _temp1267;}); _LL1256:;} static struct
_tuple0 Cyc_NewControlFlow_cf_evalguard( struct Cyc_NewControlFlow_AnalEnv* env,
struct Cyc_Position_Segment* loc, struct Cyc_List_List* aes, void* in_flow){ if((
void*)({ struct Cyc_List_List* _temp1269= aes; if( _temp1269 == 0){ _throw(
Null_Exception);} _temp1269->hd;}) ==( void*)({ struct Cyc_List_List* _temp1271=({
struct Cyc_List_List* _temp1270= aes; if( _temp1270 == 0){ _throw(
Null_Exception);} _temp1270->tl;}); if( _temp1271 == 0){ _throw( Null_Exception);}
_temp1271->hd;})){ void* out_flow= Cyc_CfAbsexp_eval_absexp( env, loc,( void*)({
struct Cyc_List_List* _temp1272= aes; if( _temp1272 == 0){ _throw(
Null_Exception);} _temp1272->hd;}), in_flow); return({ struct _tuple0 _temp1273;
_temp1273.f1= out_flow; _temp1273.f2= out_flow; _temp1273;});} return({ struct
_tuple0 _temp1274; _temp1274.f1= Cyc_CfAbsexp_eval_absexp( env, loc,( void*)({
struct Cyc_List_List* _temp1277= aes; if( _temp1277 == 0){ _throw(
Null_Exception);} _temp1277->hd;}), in_flow); _temp1274.f2= Cyc_CfAbsexp_eval_absexp(
env, loc,( void*)({ struct Cyc_List_List* _temp1276=({ struct Cyc_List_List*
_temp1275= aes; if( _temp1275 == 0){ _throw( Null_Exception);} _temp1275->tl;});
if( _temp1276 == 0){ _throw( Null_Exception);} _temp1276->hd;}), in_flow);
_temp1274;});} static void Cyc_NewControlFlow_cf_analyze_switch_clauses( struct
Cyc_NewControlFlow_AnalEnv* env, struct Cyc_List_List* scs, struct Cyc_List_List*
aes, void* in_flow){ for( 0; scs != 0; scs=({ struct Cyc_List_List* _temp1278=
scs; if( _temp1278 == 0){ _throw( Null_Exception);} _temp1278->tl;})){ struct
Cyc_Absyn_Switch_clause _temp1282; struct Cyc_Position_Segment* _temp1283;
struct Cyc_Absyn_Stmt* _temp1285; struct Cyc_Absyn_Exp* _temp1287; struct Cyc_Core_Opt*
_temp1289; struct Cyc_Absyn_Pat* _temp1291; struct Cyc_Absyn_Switch_clause*
_temp1280=( struct Cyc_Absyn_Switch_clause*)({ struct Cyc_List_List* _temp1279=
scs; if( _temp1279 == 0){ _throw( Null_Exception);} _temp1279->hd;}); _temp1282=*
_temp1280; _LL1292: _temp1291=( struct Cyc_Absyn_Pat*) _temp1282.pattern; goto
_LL1290; _LL1290: _temp1289=( struct Cyc_Core_Opt*) _temp1282.pat_vars; goto
_LL1288; _LL1288: _temp1287=( struct Cyc_Absyn_Exp*) _temp1282.where_clause;
goto _LL1286; _LL1286: _temp1285=( struct Cyc_Absyn_Stmt*) _temp1282.body; goto
_LL1284; _LL1284: _temp1283=( struct Cyc_Position_Segment*) _temp1282.loc; goto
_LL1281; _LL1281: { void* body_inflow= Cyc_NewControlFlow_add_vardecls_flow( env,
in_flow,( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp1293= _temp1289;
if( _temp1293 == 0){ _throw( Null_Exception);} _temp1293->v;})); body_inflow=
Cyc_NewControlFlow_add_init_vars_flow( env, body_inflow,( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp1294= _temp1289; if( _temp1294 == 0){ _throw(
Null_Exception);} _temp1294->v;}));{ void* body_outflow; if( _temp1287 != 0){
void* _temp1298; void* _temp1300; struct _tuple0 _temp1296= Cyc_NewControlFlow_cf_evalguard(
env,(( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1295= _temp1287; if(
_temp1295 == 0){ _throw( Null_Exception);} _temp1295;}))->loc, aes, body_inflow);
_LL1301: _temp1300= _temp1296.f1; goto _LL1299; _LL1299: _temp1298= _temp1296.f2;
goto _LL1297; _LL1297: aes=({ struct Cyc_List_List* _temp1303=({ struct Cyc_List_List*
_temp1302= aes; if( _temp1302 == 0){ _throw( Null_Exception);} _temp1302->tl;});
if( _temp1303 == 0){ _throw( Null_Exception);} _temp1303->tl;}); in_flow=
_temp1298; body_outflow= Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1285,
_temp1300);} else{ body_outflow= Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp1285, body_inflow);}{ void* _temp1304= body_outflow; _LL1306: if(( int)
_temp1304 == Cyc_CfFlowInfo_BottomFL){ goto _LL1307;} else{ goto _LL1308;}
_LL1308: goto _LL1309; _LL1307: goto _LL1305; _LL1309: Cyc_Tcutil_terr(
_temp1285->loc,( struct _tagged_string)({ char* _temp1310=( char*)"switch clause may implicitly fallthru";
struct _tagged_string _temp1311; _temp1311.curr= _temp1310; _temp1311.base=
_temp1310; _temp1311.last_plus_one= _temp1310 + 38; _temp1311;})); goto _LL1305;
_LL1305:;}}}}} void* Cyc_NewControlFlow_cf_analyze_stmt( struct Cyc_NewControlFlow_AnalEnv*
env, struct Cyc_Absyn_Stmt* s, void* in_flow){ struct Cyc_NewControlFlow_CFStmtAnnot*
annot= Cyc_NewControlFlow_pre_stmt_check( in_flow, s); in_flow=( void*) annot->flow;{
struct Cyc_List_List* aes= annot->absexps; void* _temp1312=( void*) s->r; struct
Cyc_Absyn_Exp* _temp1354; struct Cyc_Absyn_Exp* _temp1356; struct Cyc_Absyn_Exp*
_temp1358; struct Cyc_Absyn_Stmt* _temp1360; struct Cyc_Absyn_Stmt* _temp1362;
struct Cyc_Absyn_Stmt* _temp1364; struct Cyc_Absyn_Stmt* _temp1366; struct Cyc_Absyn_Exp*
_temp1368; struct Cyc_Absyn_Stmt* _temp1370; struct _tuple3 _temp1372; struct
Cyc_Absyn_Stmt* _temp1374; struct Cyc_Absyn_Exp* _temp1376; struct _tuple3
_temp1378; struct Cyc_Absyn_Stmt* _temp1380; struct Cyc_Absyn_Exp* _temp1382;
struct Cyc_Absyn_Stmt* _temp1384; struct Cyc_Absyn_Stmt* _temp1386; struct
_tuple3 _temp1388; struct Cyc_Absyn_Stmt* _temp1390; struct Cyc_Absyn_Exp*
_temp1392; struct _tuple3 _temp1394; struct Cyc_Absyn_Stmt* _temp1396; struct
Cyc_Absyn_Exp* _temp1398; struct Cyc_Absyn_Exp* _temp1400; struct Cyc_Absyn_Stmt*
_temp1402; struct Cyc_Absyn_Switch_clause** _temp1404; struct Cyc_Absyn_Switch_clause*
_temp1406; struct Cyc_List_List* _temp1407; struct Cyc_Absyn_Stmt* _temp1409;
struct Cyc_Absyn_Stmt* _temp1411; struct Cyc_Absyn_Stmt* _temp1413; struct
_tagged_string* _temp1415; struct Cyc_List_List* _temp1417; struct Cyc_Absyn_Exp*
_temp1419; struct Cyc_List_List* _temp1421; struct Cyc_Absyn_Stmt* _temp1423;
struct Cyc_Absyn_Stmt* _temp1425; struct Cyc_Absyn_Decl* _temp1427; struct Cyc_Absyn_Decl
_temp1429; struct Cyc_Position_Segment* _temp1430; void* _temp1432; struct Cyc_Absyn_Vardecl*
_temp1434; struct Cyc_Absyn_Stmt* _temp1436; struct Cyc_Absyn_Decl* _temp1438;
struct Cyc_Absyn_Decl _temp1440; struct Cyc_Position_Segment* _temp1441; void*
_temp1443; int _temp1445; struct Cyc_Absyn_Exp* _temp1447; struct Cyc_Core_Opt*
_temp1449; struct Cyc_Core_Opt* _temp1451; struct Cyc_Core_Opt _temp1453; struct
Cyc_List_List* _temp1454; struct Cyc_Absyn_Pat* _temp1456; struct Cyc_Absyn_Stmt*
_temp1458; struct _tagged_string* _temp1460; _LL1314: if(( int) _temp1312 == Cyc_Absyn_Skip_s){
goto _LL1315;} else{ goto _LL1316;} _LL1316: if(( unsigned int) _temp1312 > 1u?((
struct _tunion_struct*) _temp1312)->tag == Cyc_Absyn_Return_s_tag: 0){ _LL1355:
_temp1354=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp1312)->f1; if( _temp1354 == 0){ goto _LL1317;} else{ goto _LL1318;}} else{
goto _LL1318;} _LL1318: if(( unsigned int) _temp1312 > 1u?(( struct
_tunion_struct*) _temp1312)->tag == Cyc_Absyn_Return_s_tag: 0){ _LL1357:
_temp1356=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp1312)->f1; goto _LL1319;} else{ goto _LL1320;} _LL1320: if(( unsigned int)
_temp1312 > 1u?(( struct _tunion_struct*) _temp1312)->tag == Cyc_Absyn_Exp_s_tag:
0){ _LL1359: _temp1358=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp1312)->f1; goto _LL1321;} else{ goto _LL1322;} _LL1322: if(( unsigned int)
_temp1312 > 1u?(( struct _tunion_struct*) _temp1312)->tag == Cyc_Absyn_Seq_s_tag:
0){ _LL1363: _temp1362=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp1312)->f1; goto _LL1361; _LL1361: _temp1360=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp1312)->f2; goto _LL1323;} else{ goto
_LL1324;} _LL1324: if(( unsigned int) _temp1312 > 1u?(( struct _tunion_struct*)
_temp1312)->tag == Cyc_Absyn_IfThenElse_s_tag: 0){ _LL1369: _temp1368=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp1312)->f1; goto
_LL1367; _LL1367: _temp1366=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1312)->f2; goto _LL1365; _LL1365: _temp1364=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1312)->f3; goto _LL1325;} else{ goto
_LL1326;} _LL1326: if(( unsigned int) _temp1312 > 1u?(( struct _tunion_struct*)
_temp1312)->tag == Cyc_Absyn_While_s_tag: 0){ _LL1373: _temp1372=( struct
_tuple3)(( struct Cyc_Absyn_While_s_struct*) _temp1312)->f1; _LL1377: _temp1376=
_temp1372.f1; goto _LL1375; _LL1375: _temp1374= _temp1372.f2; goto _LL1371;
_LL1371: _temp1370=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_While_s_struct*)
_temp1312)->f2; goto _LL1327;} else{ goto _LL1328;} _LL1328: if(( unsigned int)
_temp1312 > 1u?(( struct _tunion_struct*) _temp1312)->tag == Cyc_Absyn_Do_s_tag:
0){ _LL1385: _temp1384=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*)
_temp1312)->f1; goto _LL1379; _LL1379: _temp1378=( struct _tuple3)(( struct Cyc_Absyn_Do_s_struct*)
_temp1312)->f2; _LL1383: _temp1382= _temp1378.f1; goto _LL1381; _LL1381:
_temp1380= _temp1378.f2; goto _LL1329;} else{ goto _LL1330;} _LL1330: if((
unsigned int) _temp1312 > 1u?(( struct _tunion_struct*) _temp1312)->tag == Cyc_Absyn_For_s_tag:
0){ _LL1401: _temp1400=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp1312)->f1; goto _LL1395; _LL1395: _temp1394=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp1312)->f2; _LL1399: _temp1398= _temp1394.f1; goto _LL1397; _LL1397:
_temp1396= _temp1394.f2; goto _LL1389; _LL1389: _temp1388=( struct _tuple3)((
struct Cyc_Absyn_For_s_struct*) _temp1312)->f3; _LL1393: _temp1392= _temp1388.f1;
goto _LL1391; _LL1391: _temp1390= _temp1388.f2; goto _LL1387; _LL1387: _temp1386=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp1312)->f4; goto
_LL1331;} else{ goto _LL1332;} _LL1332: if(( unsigned int) _temp1312 > 1u?((
struct _tunion_struct*) _temp1312)->tag == Cyc_Absyn_Break_s_tag: 0){ _LL1403:
_temp1402=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Break_s_struct*)
_temp1312)->f1; if( _temp1402 == 0){ goto _LL1333;} else{ goto _LL1334;}} else{
goto _LL1334;} _LL1334: if(( unsigned int) _temp1312 > 1u?(( struct
_tunion_struct*) _temp1312)->tag == Cyc_Absyn_Fallthru_s_tag: 0){ _LL1408:
_temp1407=( struct Cyc_List_List*)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1312)->f1; goto _LL1405; _LL1405: _temp1404=( struct Cyc_Absyn_Switch_clause**)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1312)->f2; if( _temp1404 == 0){ goto
_LL1336;} else{ _temp1406=* _temp1404; goto _LL1335;}} else{ goto _LL1336;}
_LL1336: if(( unsigned int) _temp1312 > 1u?(( struct _tunion_struct*) _temp1312)->tag
== Cyc_Absyn_Break_s_tag: 0){ _LL1410: _temp1409=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Break_s_struct*) _temp1312)->f1; goto _LL1337;} else{ goto
_LL1338;} _LL1338: if(( unsigned int) _temp1312 > 1u?(( struct _tunion_struct*)
_temp1312)->tag == Cyc_Absyn_Continue_s_tag: 0){ _LL1412: _temp1411=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Continue_s_struct*) _temp1312)->f1; goto _LL1339;} else{ goto
_LL1340;} _LL1340: if(( unsigned int) _temp1312 > 1u?(( struct _tunion_struct*)
_temp1312)->tag == Cyc_Absyn_Goto_s_tag: 0){ _LL1416: _temp1415=( struct
_tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp1312)->f1; goto _LL1414;
_LL1414: _temp1413=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp1312)->f2; goto _LL1341;} else{ goto _LL1342;} _LL1342: if(( unsigned int)
_temp1312 > 1u?(( struct _tunion_struct*) _temp1312)->tag == Cyc_Absyn_Switch_s_tag:
0){ _LL1420: _temp1419=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp1312)->f1; goto _LL1418; _LL1418: _temp1417=( struct Cyc_List_List*)((
struct Cyc_Absyn_Switch_s_struct*) _temp1312)->f2; goto _LL1343;} else{ goto
_LL1344;} _LL1344: if(( unsigned int) _temp1312 > 1u?(( struct _tunion_struct*)
_temp1312)->tag == Cyc_Absyn_TryCatch_s_tag: 0){ _LL1424: _temp1423=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp1312)->f1; goto _LL1422; _LL1422:
_temp1421=( struct Cyc_List_List*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1312)->f2; goto _LL1345;} else{ goto _LL1346;} _LL1346: if(( unsigned int)
_temp1312 > 1u?(( struct _tunion_struct*) _temp1312)->tag == Cyc_Absyn_Decl_s_tag:
0){ _LL1428: _temp1427=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1312)->f1; _temp1429=* _temp1427; _LL1433: _temp1432=( void*) _temp1429.r;
if((( struct _tunion_struct*) _temp1432)->tag == Cyc_Absyn_Var_d_tag){ _LL1435:
_temp1434=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp1432)->f1; goto _LL1431;} else{ goto _LL1348;} _LL1431: _temp1430=( struct
Cyc_Position_Segment*) _temp1429.loc; goto _LL1426; _LL1426: _temp1425=( struct
Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1312)->f2; goto _LL1347;}
else{ goto _LL1348;} _LL1348: if(( unsigned int) _temp1312 > 1u?(( struct
_tunion_struct*) _temp1312)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL1439: _temp1438=(
struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1312)->f1;
_temp1440=* _temp1438; _LL1444: _temp1443=( void*) _temp1440.r; if((( struct
_tunion_struct*) _temp1443)->tag == Cyc_Absyn_Let_d_tag){ _LL1457: _temp1456=(
struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*) _temp1443)->f1; goto
_LL1452; _LL1452: _temp1451=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1443)->f2; if( _temp1451 == 0){ goto _LL1350;} else{ _temp1453=* _temp1451;
_LL1455: _temp1454=( struct Cyc_List_List*) _temp1453.v; goto _LL1450;} _LL1450:
_temp1449=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp1443)->f3;
goto _LL1448; _LL1448: _temp1447=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1443)->f4; goto _LL1446; _LL1446: _temp1445=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp1443)->f5; goto _LL1442;} else{ goto _LL1350;} _LL1442: _temp1441=( struct
Cyc_Position_Segment*) _temp1440.loc; goto _LL1437; _LL1437: _temp1436=( struct
Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1312)->f2; goto _LL1349;}
else{ goto _LL1350;} _LL1350: if(( unsigned int) _temp1312 > 1u?(( struct
_tunion_struct*) _temp1312)->tag == Cyc_Absyn_Label_s_tag: 0){ _LL1461:
_temp1460=( struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*)
_temp1312)->f1; goto _LL1459; _LL1459: _temp1458=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Label_s_struct*) _temp1312)->f2; goto _LL1351;} else{ goto
_LL1352;} _LL1352: goto _LL1353; _LL1315: return in_flow; _LL1317: return( void*)
Cyc_CfFlowInfo_BottomFL; _LL1319: Cyc_CfAbsexp_eval_absexp( env,({ struct Cyc_Absyn_Exp*
_temp1462= _temp1356; if( _temp1462 == 0){ _throw( Null_Exception);} _temp1462->loc;}),(
void*)({ struct Cyc_List_List* _temp1463= aes; if( _temp1463 == 0){ _throw(
Null_Exception);} _temp1463->hd;}), in_flow); return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1321: return Cyc_CfAbsexp_eval_absexp( env, _temp1358->loc,( void*)({ struct
Cyc_List_List* _temp1464= aes; if( _temp1464 == 0){ _throw( Null_Exception);}
_temp1464->hd;}), in_flow); _LL1323: return Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1360, Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1362, in_flow));
_LL1325: { void* _temp1467; void* _temp1469; struct _tuple0 _temp1465= Cyc_NewControlFlow_cf_evalguard(
env, _temp1368->loc, aes, in_flow); _LL1470: _temp1469= _temp1465.f1; goto
_LL1468; _LL1468: _temp1467= _temp1465.f2; goto _LL1466; _LL1466: return Cyc_CfFlowInfo_join_flow(
Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1366, _temp1469), Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1364, _temp1467));} _LL1327: { void* e_inflow=( void*)( Cyc_NewControlFlow_pre_stmt_check(
in_flow, _temp1374))->flow; void* _temp1473; void* _temp1475; struct _tuple0
_temp1471= Cyc_NewControlFlow_cf_evalguard( env, _temp1376->loc, aes, e_inflow);
_LL1476: _temp1475= _temp1471.f1; goto _LL1474; _LL1474: _temp1473= _temp1471.f2;
goto _LL1472; _LL1472: { void* body_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1370, _temp1475); Cyc_NewControlFlow_update_flow( _temp1374,
body_outflow); return _temp1473;}} _LL1329: { void* body_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1384, in_flow); void* e_inflow=( void*)( Cyc_NewControlFlow_pre_stmt_check(
body_outflow, _temp1380))->flow; void* _temp1479; void* _temp1481; struct
_tuple0 _temp1477= Cyc_NewControlFlow_cf_evalguard( env, _temp1382->loc, aes,
e_inflow); _LL1482: _temp1481= _temp1477.f1; goto _LL1480; _LL1480: _temp1479=
_temp1477.f2; goto _LL1478; _LL1478: Cyc_NewControlFlow_update_flow( _temp1384,
_temp1481); return _temp1479;} _LL1331: { void* e1_outflow= Cyc_CfAbsexp_eval_absexp(
env, _temp1400->loc,( void*)({ struct Cyc_List_List* _temp1483= aes; if(
_temp1483 == 0){ _throw( Null_Exception);} _temp1483->hd;}), in_flow); void*
e2_inflow=( void*)( Cyc_NewControlFlow_pre_stmt_check( e1_outflow, _temp1396))->flow;
void* _temp1487; void* _temp1489; struct _tuple0 _temp1485= Cyc_NewControlFlow_cf_evalguard(
env, _temp1398->loc,({ struct Cyc_List_List* _temp1484= aes; if( _temp1484 == 0){
_throw( Null_Exception);} _temp1484->tl;}), e2_inflow); _LL1490: _temp1489=
_temp1485.f1; goto _LL1488; _LL1488: _temp1487= _temp1485.f2; goto _LL1486;
_LL1486: { void* body_outflow= Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp1386, _temp1489); void* e3_inflow=( void*)( Cyc_NewControlFlow_pre_stmt_check(
body_outflow, _temp1390))->flow; void* e3_outflow= Cyc_CfAbsexp_eval_absexp( env,
_temp1392->loc,( void*)({ struct Cyc_List_List* _temp1494=({ struct Cyc_List_List*
_temp1493=({ struct Cyc_List_List* _temp1492=({ struct Cyc_List_List* _temp1491=
aes; if( _temp1491 == 0){ _throw( Null_Exception);} _temp1491->tl;}); if(
_temp1492 == 0){ _throw( Null_Exception);} _temp1492->tl;}); if( _temp1493 == 0){
_throw( Null_Exception);} _temp1493->tl;}); if( _temp1494 == 0){ _throw(
Null_Exception);} _temp1494->hd;}), e3_inflow); Cyc_NewControlFlow_update_flow(
_temp1396, e3_outflow); return _temp1487;}} _LL1333: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1335: in_flow= Cyc_CfAbsexp_eval_absexp( env, s->loc,( void*)({ struct Cyc_List_List*
_temp1495= aes; if( _temp1495 == 0){ _throw( Null_Exception);} _temp1495->hd;}),
in_flow);{ struct Cyc_List_List* ftvars=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp1496= _temp1406->pat_vars; if( _temp1496 == 0){ _throw( Null_Exception);}
_temp1496->v;}); in_flow= Cyc_NewControlFlow_add_vardecls_flow( env, in_flow,
ftvars); in_flow= Cyc_NewControlFlow_add_init_vars_flow( env, in_flow, ftvars);
_temp1409=( struct Cyc_Absyn_Stmt*) _temp1406->body; goto _LL1337;} _LL1337:
_temp1411= _temp1409; goto _LL1339; _LL1339: _temp1413= _temp1411; goto _LL1341;
_LL1341: Cyc_NewControlFlow_update_flow(( struct Cyc_Absyn_Stmt*)({ struct Cyc_Absyn_Stmt*
_temp1497= _temp1413; if( _temp1497 == 0){ _throw( Null_Exception);} _temp1497;}),
in_flow); return( void*) Cyc_CfFlowInfo_BottomFL; _LL1343: in_flow= Cyc_CfAbsexp_eval_absexp(
env, _temp1419->loc,( void*)({ struct Cyc_List_List* _temp1498= aes; if(
_temp1498 == 0){ _throw( Null_Exception);} _temp1498->hd;}), in_flow); Cyc_NewControlFlow_cf_analyze_switch_clauses(
env, _temp1417,({ struct Cyc_List_List* _temp1499= aes; if( _temp1499 == 0){
_throw( Null_Exception);} _temp1499->tl;}), in_flow); return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1345: { struct Cyc_NewControlFlow_AnalEnv _temp1502; void* _temp1503; int
_temp1505; struct Cyc_Dict_Dict* _temp1507; struct Cyc_NewControlFlow_AnalEnv*
_temp1500= env; _temp1502=* _temp1500; _LL1508: _temp1507=( struct Cyc_Dict_Dict*)
_temp1502.roots; goto _LL1506; _LL1506: _temp1505=( int) _temp1502.in_try; goto
_LL1504; _LL1504: _temp1503=( void*) _temp1502.tryflow; goto _LL1501; _LL1501:
env->in_try= 1;( void*)( env->tryflow=( void*) in_flow);{ void* s_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1423, in_flow); void* scs_inflow=( void*) env->tryflow; env->in_try=
_temp1505;( void*)( env->tryflow=( void*) _temp1503); Cyc_NewControlFlow_update_tryflow(
env, scs_inflow); Cyc_NewControlFlow_cf_analyze_switch_clauses( env, _temp1421,
aes, scs_inflow); return s_outflow;}} _LL1347: { struct Cyc_List_List l=({
struct Cyc_List_List _temp1509; _temp1509.hd=( void*) _temp1434; _temp1509.tl= 0;
_temp1509;}); void* body_inflow= Cyc_NewControlFlow_add_vardecls_flow( env,
in_flow,( struct Cyc_List_List*)& l); struct Cyc_Absyn_Exp* e= _temp1434->initializer;
if( e != 0){ body_inflow= Cyc_CfAbsexp_eval_absexp( env,(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1510= e; if( _temp1510 == 0){ _throw( Null_Exception);}
_temp1510;}))->loc,( void*)({ struct Cyc_List_List* _temp1511= aes; if(
_temp1511 == 0){ _throw( Null_Exception);} _temp1511->hd;}), body_inflow);}
return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1425, body_inflow);}
_LL1349: { void* body_inflow= Cyc_NewControlFlow_add_vardecls_flow( env, in_flow,
_temp1454); body_inflow= Cyc_CfAbsexp_eval_absexp( env, _temp1447->loc,( void*)({
struct Cyc_List_List* _temp1512= aes; if( _temp1512 == 0){ _throw(
Null_Exception);} _temp1512->hd;}), body_inflow); body_inflow= Cyc_NewControlFlow_add_init_vars_flow(
env, body_inflow, _temp1454); return Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp1436, body_inflow);} _LL1351: return Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1458, in_flow); _LL1353:( void) _throw(( struct _xtunion_struct*)({
struct Cyc_Core_Impossible_struct* _temp1513=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp1513[ 0]=({ struct
Cyc_Core_Impossible_struct _temp1514; _temp1514.tag= Cyc_Core_Impossible_tag;
_temp1514.f1=( struct _tagged_string)({ char* _temp1515=( char*)"cf_analyze_stmt -- bad stmt syntax or unimplemented stmt form";
struct _tagged_string _temp1516; _temp1516.curr= _temp1515; _temp1516.base=
_temp1515; _temp1516.last_plus_one= _temp1515 + 62; _temp1516;}); _temp1514;});
_temp1513;})); _LL1313:;}} void Cyc_NewControlFlow_cf_postpass( struct Cyc_Absyn_Stmt*
s){ s->annot=( struct _xtunion_struct*)({ struct Cyc_Absyn_EmptyAnnot_struct*
_temp1517=( struct Cyc_Absyn_EmptyAnnot_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EmptyAnnot_struct));
_temp1517[ 0]=({ struct Cyc_Absyn_EmptyAnnot_struct _temp1518; _temp1518.tag=
Cyc_Absyn_EmptyAnnot_tag; _temp1518;}); _temp1517;});{ void* _temp1519=( void*)
s->r; struct Cyc_Absyn_Stmt* _temp1541; struct Cyc_Absyn_Stmt* _temp1543; struct
Cyc_Absyn_Stmt* _temp1545; struct Cyc_Absyn_Stmt* _temp1547; struct Cyc_Absyn_Exp*
_temp1549; struct Cyc_Absyn_Stmt* _temp1551; struct _tuple3 _temp1553; struct
Cyc_Absyn_Stmt* _temp1555; struct Cyc_Absyn_Exp* _temp1557; struct _tuple3
_temp1559; struct Cyc_Absyn_Stmt* _temp1561; struct Cyc_Absyn_Exp* _temp1563;
struct Cyc_Absyn_Exp* _temp1565; struct Cyc_Absyn_Stmt* _temp1567; struct
_tuple3 _temp1569; struct Cyc_Absyn_Stmt* _temp1571; struct Cyc_Absyn_Exp*
_temp1573; struct _tuple3 _temp1575; struct Cyc_Absyn_Stmt* _temp1577; struct
Cyc_Absyn_Exp* _temp1579; struct Cyc_Absyn_Stmt* _temp1581; struct Cyc_Absyn_Stmt*
_temp1583; struct _tagged_string* _temp1585; struct Cyc_Absyn_Stmt* _temp1587;
struct Cyc_Absyn_Decl* _temp1589; struct Cyc_List_List* _temp1591; struct Cyc_Absyn_Stmt*
_temp1593; struct Cyc_List_List* _temp1595; struct Cyc_Absyn_Exp* _temp1597;
_LL1521: if(( unsigned int) _temp1519 > 1u?(( struct _tunion_struct*) _temp1519)->tag
== Cyc_Absyn_Seq_s_tag: 0){ _LL1544: _temp1543=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp1519)->f1; goto _LL1542; _LL1542: _temp1541=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1519)->f2; goto
_LL1522;} else{ goto _LL1523;} _LL1523: if(( unsigned int) _temp1519 > 1u?((
struct _tunion_struct*) _temp1519)->tag == Cyc_Absyn_IfThenElse_s_tag: 0){
_LL1550: _temp1549=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1519)->f1; goto _LL1548; _LL1548: _temp1547=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1519)->f2; goto _LL1546; _LL1546:
_temp1545=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1519)->f3; goto _LL1524;} else{ goto _LL1525;} _LL1525: if(( unsigned int)
_temp1519 > 1u?(( struct _tunion_struct*) _temp1519)->tag == Cyc_Absyn_For_s_tag:
0){ _LL1566: _temp1565=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp1519)->f1; goto _LL1560; _LL1560: _temp1559=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp1519)->f2; _LL1564: _temp1563= _temp1559.f1; goto _LL1562; _LL1562:
_temp1561= _temp1559.f2; goto _LL1554; _LL1554: _temp1553=( struct _tuple3)((
struct Cyc_Absyn_For_s_struct*) _temp1519)->f3; _LL1558: _temp1557= _temp1553.f1;
goto _LL1556; _LL1556: _temp1555= _temp1553.f2; goto _LL1552; _LL1552: _temp1551=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp1519)->f4; goto
_LL1526;} else{ goto _LL1527;} _LL1527: if(( unsigned int) _temp1519 > 1u?((
struct _tunion_struct*) _temp1519)->tag == Cyc_Absyn_While_s_tag: 0){ _LL1570:
_temp1569=( struct _tuple3)(( struct Cyc_Absyn_While_s_struct*) _temp1519)->f1;
_LL1574: _temp1573= _temp1569.f1; goto _LL1572; _LL1572: _temp1571= _temp1569.f2;
goto _LL1568; _LL1568: _temp1567=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_While_s_struct*)
_temp1519)->f2; goto _LL1528;} else{ goto _LL1529;} _LL1529: if(( unsigned int)
_temp1519 > 1u?(( struct _tunion_struct*) _temp1519)->tag == Cyc_Absyn_Do_s_tag:
0){ _LL1582: _temp1581=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*)
_temp1519)->f1; goto _LL1576; _LL1576: _temp1575=( struct _tuple3)(( struct Cyc_Absyn_Do_s_struct*)
_temp1519)->f2; _LL1580: _temp1579= _temp1575.f1; goto _LL1578; _LL1578:
_temp1577= _temp1575.f2; goto _LL1530;} else{ goto _LL1531;} _LL1531: if((
unsigned int) _temp1519 > 1u?(( struct _tunion_struct*) _temp1519)->tag == Cyc_Absyn_Label_s_tag:
0){ _LL1586: _temp1585=( struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*)
_temp1519)->f1; goto _LL1584; _LL1584: _temp1583=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Label_s_struct*) _temp1519)->f2; goto _LL1532;} else{ goto
_LL1533;} _LL1533: if(( unsigned int) _temp1519 > 1u?(( struct _tunion_struct*)
_temp1519)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL1590: _temp1589=( struct Cyc_Absyn_Decl*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1519)->f1; goto _LL1588; _LL1588:
_temp1587=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1519)->f2;
goto _LL1534;} else{ goto _LL1535;} _LL1535: if(( unsigned int) _temp1519 > 1u?((
struct _tunion_struct*) _temp1519)->tag == Cyc_Absyn_TryCatch_s_tag: 0){ _LL1594:
_temp1593=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1519)->f1; goto _LL1592; _LL1592: _temp1591=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp1519)->f2; goto _LL1536;} else{ goto
_LL1537;} _LL1537: if(( unsigned int) _temp1519 > 1u?(( struct _tunion_struct*)
_temp1519)->tag == Cyc_Absyn_Switch_s_tag: 0){ _LL1598: _temp1597=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Switch_s_struct*) _temp1519)->f1; goto _LL1596; _LL1596:
_temp1595=( struct Cyc_List_List*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp1519)->f2; goto _LL1538;} else{ goto _LL1539;} _LL1539: goto _LL1540;
_LL1522: _temp1547= _temp1543; _temp1545= _temp1541; goto _LL1524; _LL1524: Cyc_NewControlFlow_cf_postpass(
_temp1547); Cyc_NewControlFlow_cf_postpass( _temp1545); return; _LL1526: Cyc_NewControlFlow_cf_postpass(
_temp1555); _temp1571= _temp1551; _temp1567= _temp1561; goto _LL1528; _LL1528:
_temp1581= _temp1567; _temp1577= _temp1571; goto _LL1530; _LL1530: Cyc_NewControlFlow_cf_postpass(
_temp1577); _temp1583= _temp1581; goto _LL1532; _LL1532: _temp1587= _temp1583;
goto _LL1534; _LL1534: Cyc_NewControlFlow_cf_postpass( _temp1587); return;
_LL1536: Cyc_NewControlFlow_cf_postpass( _temp1593); _temp1595= _temp1591; goto
_LL1538; _LL1538: for( 0; _temp1595 != 0; _temp1595=({ struct Cyc_List_List*
_temp1599= _temp1595; if( _temp1599 == 0){ _throw( Null_Exception);} _temp1599->tl;})){
Cyc_NewControlFlow_cf_postpass((( struct Cyc_Absyn_Switch_clause*)({ struct Cyc_List_List*
_temp1600= _temp1595; if( _temp1600 == 0){ _throw( Null_Exception);} _temp1600->hd;}))->body);}
return; _LL1540: return; _LL1520:;}} void Cyc_NewControlFlow_cf_check_fun(
struct Cyc_Absyn_Fndecl* fd){ struct Cyc_NewControlFlow_AbsEnv absenv=({ struct
Cyc_NewControlFlow_AbsEnv _temp1601; _temp1601.shared=({ struct Cyc_NewControlFlow_SharedAbsEnv*
_temp1602=( struct Cyc_NewControlFlow_SharedAbsEnv*) GC_malloc( sizeof( struct
Cyc_NewControlFlow_SharedAbsEnv)); _temp1602->all_roots=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( void*, void*))) Cyc_Dict_empty)( Cyc_CfFlowInfo_local_root_cmp);
_temp1602->comprehension_vars= 0; _temp1602;}); _temp1601.lhsides= 0; _temp1601.access_path=
0; _temp1601;}); struct Cyc_List_List* arg_vds=( struct Cyc_List_List*)({ struct
Cyc_Core_Opt* _temp1603= fd->param_vardecls; if( _temp1603 == 0){ _throw(
Null_Exception);} _temp1603->v;});{ struct Cyc_List_List* vds= arg_vds; for( 0;
vds != 0; vds=({ struct Cyc_List_List* _temp1604= vds; if( _temp1604 == 0){
_throw( Null_Exception);} _temp1604->tl;})){ Cyc_NewControlFlow_add_var_root(
absenv,( struct Cyc_Absyn_Vardecl*)({ struct Cyc_List_List* _temp1605= vds; if(
_temp1605 == 0){ _throw( Null_Exception);} _temp1605->hd;}));}} Cyc_NewControlFlow_cf_prepass(
absenv, fd->body);{ struct Cyc_List_List* _temp1608; struct Cyc_List_List*
_temp1610; struct Cyc_NewControlFlow_SharedAbsEnv* _temp1612; struct Cyc_NewControlFlow_SharedAbsEnv
_temp1614; struct Cyc_List_List* _temp1615; struct Cyc_Dict_Dict* _temp1617;
struct Cyc_NewControlFlow_AbsEnv _temp1606= absenv; _LL1613: _temp1612=( struct
Cyc_NewControlFlow_SharedAbsEnv*) _temp1606.shared; _temp1614=* _temp1612;
_LL1618: _temp1617=( struct Cyc_Dict_Dict*) _temp1614.all_roots; goto _LL1616;
_LL1616: _temp1615=( struct Cyc_List_List*) _temp1614.comprehension_vars; goto
_LL1611; _LL1611: _temp1610=( struct Cyc_List_List*) _temp1606.lhsides; goto
_LL1609; _LL1609: _temp1608=( struct Cyc_List_List*) _temp1606.access_path; goto
_LL1607; _LL1607: { struct Cyc_NewControlFlow_AnalEnv* analenv=({ struct Cyc_NewControlFlow_AnalEnv*
_temp1619=( struct Cyc_NewControlFlow_AnalEnv*) GC_malloc( sizeof( struct Cyc_NewControlFlow_AnalEnv));
_temp1619->roots= _temp1617; _temp1619->in_try= 0; _temp1619->tryflow=( void*)((
void*) Cyc_CfFlowInfo_BottomFL); _temp1619;}); void* in_flow=( void*)({ struct
Cyc_CfFlowInfo_InitsFL_struct* _temp1620=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp1620[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp1621; _temp1621.tag= Cyc_CfFlowInfo_InitsFL_tag;
_temp1621.f1=(( struct Cyc_Dict_Dict*(*)( int(* comp)( void*, void*))) Cyc_Dict_empty)(
Cyc_CfFlowInfo_local_root_cmp); _temp1621;}); _temp1620;}); in_flow= Cyc_NewControlFlow_add_init_vars_flow(
analenv, in_flow, arg_vds); in_flow= Cyc_NewControlFlow_add_vardecls_flow(
analenv, in_flow, _temp1615);{ void* out_flow= in_flow; Cyc_NewControlFlow_iteration_num=
0; Cyc_NewControlFlow_iterate_cf_check= 1; while( Cyc_NewControlFlow_iterate_cf_check) {
++ Cyc_NewControlFlow_iteration_num; Cyc_NewControlFlow_iterate_cf_check= 0;
out_flow= Cyc_NewControlFlow_cf_analyze_stmt( analenv, fd->body, in_flow);}{
void* _temp1622= out_flow; _LL1624: if(( int) _temp1622 == Cyc_CfFlowInfo_BottomFL){
goto _LL1625;} else{ goto _LL1626;} _LL1626: goto _LL1627; _LL1625: goto _LL1623;
_LL1627: if(( void*) fd->ret_type !=( void*) Cyc_Absyn_VoidType){ Cyc_Tcutil_terr((
fd->body)->loc,( struct _tagged_string)({ char* _temp1628=( char*)"function may complete without returning a value";
struct _tagged_string _temp1629; _temp1629.curr= _temp1628; _temp1629.base=
_temp1628; _temp1629.last_plus_one= _temp1628 + 48; _temp1629;}));} goto _LL1623;
_LL1623:;} Cyc_NewControlFlow_cf_postpass( fd->body);}}}} void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List* ds){ for( 0; ds != 0; ds=({ struct Cyc_List_List*
_temp1630= ds; if( _temp1630 == 0){ _throw( Null_Exception);} _temp1630->tl;})){
void* _temp1632=( void*)(( struct Cyc_Absyn_Decl*)({ struct Cyc_List_List*
_temp1631= ds; if( _temp1631 == 0){ _throw( Null_Exception);} _temp1631->hd;}))->r;
struct Cyc_Absyn_Fndecl* _temp1642; struct Cyc_List_List* _temp1644; struct
_tuple1* _temp1646; struct Cyc_List_List* _temp1648; struct _tagged_string*
_temp1650; _LL1634: if((( struct _tunion_struct*) _temp1632)->tag == Cyc_Absyn_Fn_d_tag){
_LL1643: _temp1642=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*)
_temp1632)->f1; goto _LL1635;} else{ goto _LL1636;} _LL1636: if((( struct
_tunion_struct*) _temp1632)->tag == Cyc_Absyn_Using_d_tag){ _LL1647: _temp1646=(
struct _tuple1*)(( struct Cyc_Absyn_Using_d_struct*) _temp1632)->f1; goto
_LL1645; _LL1645: _temp1644=( struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*)
_temp1632)->f2; goto _LL1637;} else{ goto _LL1638;} _LL1638: if((( struct
_tunion_struct*) _temp1632)->tag == Cyc_Absyn_Namespace_d_tag){ _LL1651:
_temp1650=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1632)->f1; goto _LL1649; _LL1649: _temp1648=( struct Cyc_List_List*)((
struct Cyc_Absyn_Namespace_d_struct*) _temp1632)->f2; goto _LL1639;} else{ goto
_LL1640;} _LL1640: goto _LL1641; _LL1635: Cyc_NewControlFlow_cf_check_fun(
_temp1642); goto _LL1633; _LL1637: _temp1648= _temp1644; goto _LL1639; _LL1639:
Cyc_NewControlFlow_cf_check( _temp1648); goto _LL1633; _LL1641: goto _LL1633;
_LL1633:;}}