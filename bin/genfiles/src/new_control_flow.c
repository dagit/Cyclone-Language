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
int i= 0; for( 0; _temp25 != 0; _temp25= _temp25->tl, ++ i){ struct _tuple4
_temp33; void* _temp34; struct Cyc_Absyn_Tqual* _temp36; struct _tuple4* _temp31=(
struct _tuple4*) _temp25->hd; _temp33=* _temp31; _LL37: _temp36= _temp33.f1;
goto _LL35; _LL35: _temp34= _temp33.f2; goto _LL32; _LL32: d=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, int key, void* data)) Cyc_Dict_insert)( d, i, Cyc_CfFlowInfo_typ_to_unesc_none_pinfo(
_temp34));}}( env.shared)->all_roots=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, void* key, void* data)) Cyc_Dict_insert_new)(( env.shared)->all_roots,( void*)({
struct Cyc_CfFlowInfo_MallocPt_struct* _temp38=( struct Cyc_CfFlowInfo_MallocPt_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct)); _temp38[ 0]=({
struct Cyc_CfFlowInfo_MallocPt_struct _temp39; _temp39.tag= Cyc_CfFlowInfo_MallocPt_tag;
_temp39.f1= e; _temp39;}); _temp38;}),( void*)({ struct Cyc_CfFlowInfo_TuplePI_struct*
_temp40=( struct Cyc_CfFlowInfo_TuplePI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct));
_temp40[ 0]=({ struct Cyc_CfFlowInfo_TuplePI_struct _temp41; _temp41.tag= Cyc_CfFlowInfo_TuplePI_tag;
_temp41.f1= d; _temp41;}); _temp40;}));}} struct Cyc_NewControlFlow_AbsEnv Cyc_NewControlFlow_change_lhs(
struct Cyc_NewControlFlow_AbsEnv src, struct Cyc_List_List* new_lhs){ return({
struct Cyc_NewControlFlow_AbsEnv _temp42; _temp42.shared= src.shared; _temp42.lhsides=
new_lhs; _temp42.access_path= src.access_path; _temp42;});} static struct Cyc_List_List*
Cyc_NewControlFlow_unknown_list= 0; static struct Cyc_List_List* Cyc_NewControlFlow_use_it_list(){
if( Cyc_NewControlFlow_unknown_list == 0){ Cyc_NewControlFlow_unknown_list=({
struct Cyc_List_List* _temp43=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp43->hd=( void*) Cyc_CfAbsexp_mkUnknownOp(); _temp43->tl= 0;
_temp43;});} return Cyc_NewControlFlow_unknown_list;} struct Cyc_NewControlFlow_AbsEnv
Cyc_NewControlFlow_use_it_env( struct Cyc_NewControlFlow_AbsEnv src){ struct Cyc_NewControlFlow_AbsEnv
ans= Cyc_NewControlFlow_change_lhs( src, Cyc_NewControlFlow_use_it_list()); ans.access_path=
0; return ans;} struct Cyc_NewControlFlow_AbsSynOne Cyc_NewControlFlow_meet_absexp(
struct Cyc_NewControlFlow_AbsSyn syn){ struct Cyc_List_List* _temp46; struct Cyc_NewControlFlow_AbsSynOne
_temp48; void* _temp50; void* _temp52; struct Cyc_NewControlFlow_AbsSynOne
_temp54; void* _temp56; void* _temp58; struct Cyc_NewControlFlow_AbsSyn _temp44=
syn; _LL55: _temp54=( struct Cyc_NewControlFlow_AbsSynOne) _temp44.when_true;
_LL59: _temp58=( void*) _temp54.inner_exp; goto _LL57; _LL57: _temp56=( void*)
_temp54.assigns; goto _LL49; _LL49: _temp48=( struct Cyc_NewControlFlow_AbsSynOne)
_temp44.when_false; _LL53: _temp52=( void*) _temp48.inner_exp; goto _LL51; _LL51:
_temp50=( void*) _temp48.assigns; goto _LL47; _LL47: _temp46=( struct Cyc_List_List*)
_temp44.lvalues; goto _LL45; _LL45: { void* inn= _temp58 == _temp52? _temp58:
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OneofG, _temp58, _temp52); void*
ass= _temp56 == _temp50? _temp56: Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OneofG,
_temp56, _temp50); return({ struct Cyc_NewControlFlow_AbsSynOne _temp60; _temp60.inner_exp=(
void*) inn; _temp60.assigns=( void*) ass; _temp60;});}} struct Cyc_NewControlFlow_AbsSyn
Cyc_NewControlFlow_mkSyn_tf( void* ae, struct Cyc_List_List* lvals, void*
assigns){ struct Cyc_NewControlFlow_AbsSynOne one_way=({ struct Cyc_NewControlFlow_AbsSynOne
_temp61; _temp61.inner_exp=( void*) ae; _temp61.assigns=( void*) assigns;
_temp61;}); return({ struct Cyc_NewControlFlow_AbsSyn _temp62; _temp62.when_true=
one_way; _temp62.when_false= one_way; _temp62.lvalues= lvals; _temp62;});} void*
Cyc_NewControlFlow_make_assigns( struct Cyc_NewControlFlow_AbsEnv env, void* ae){
if( env.lhsides == 0){ return Cyc_CfAbsexp_mkSkipAE();} if(( env.lhsides)->tl ==
0){ return Cyc_CfAbsexp_mkAssignAE(( void*)( env.lhsides)->hd, ae);}{ struct Cyc_List_List*
l= 0;{ struct Cyc_List_List* lhsides= env.lhsides; for( 0; lhsides != 0; lhsides=
lhsides->tl){ l=({ struct Cyc_List_List* _temp63=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp63->hd=( void*) Cyc_CfAbsexp_mkAssignAE((
void*) lhsides->hd, ae); _temp63->tl= l; _temp63;});}} return Cyc_CfAbsexp_mkGroupAE_l((
void*) Cyc_CfAbsexp_OneofG, l);}} struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_abstract_exp(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e); void Cyc_NewControlFlow_cf_prepass(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Stmt* s); struct Cyc_NewControlFlow_AbsSyn
Cyc_NewControlFlow_use_exp_unordered( struct Cyc_NewControlFlow_AbsEnv env,
struct Cyc_List_List* es, struct Cyc_List_List* lhs){ struct Cyc_NewControlFlow_AbsEnv
elt_env= Cyc_NewControlFlow_use_it_env( env); void* es_exp= Cyc_CfAbsexp_mkSkipAE();
void* uses_exp= Cyc_CfAbsexp_mkSkipAE(); for( 0; es != 0; es= es->tl){ struct
Cyc_NewControlFlow_AbsSyn syn= Cyc_NewControlFlow_abstract_exp( elt_env,( struct
Cyc_Absyn_Exp*) es->hd); void* _temp66; void* _temp68; struct Cyc_NewControlFlow_AbsSynOne
_temp64= Cyc_NewControlFlow_meet_absexp( syn); _LL69: _temp68=( void*) _temp64.inner_exp;
goto _LL67; _LL67: _temp66=( void*) _temp64.assigns; goto _LL65; _LL65: es_exp=
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG, _temp68, es_exp);
uses_exp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_mkAnyOrderG(), _temp66, uses_exp);}
return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
es_exp, uses_exp), lhs, Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp()));}
struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_malloc_and_init( struct Cyc_NewControlFlow_AbsEnv
env, struct Cyc_Absyn_Exp* e, void* es_exp, void* useexp){ void* ansexp= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkMallocAE( e), Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, es_exp, useexp)); return Cyc_NewControlFlow_mkSyn_tf(
ansexp, 0, Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkAddressOp( Cyc_CfAbsexp_mkMallocOp(
e))));} struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_descend_path( struct
Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* path_e, struct Cyc_Absyn_Exp*
inner_e){ env.access_path=({ struct Cyc_List_List* _temp70=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp70->hd=( void*) path_e; _temp70->tl=
env.access_path; _temp70;});{ struct Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp(
env, inner_e); if( syn1.lvalues == 0){ syn1.lvalues= Cyc_NewControlFlow_use_it_list();}
return syn1;}} void* Cyc_NewControlFlow_destruct_path( struct Cyc_NewControlFlow_AbsEnv
env, void* absop){{ struct Cyc_List_List* path= env.access_path; for( 0; path !=
0; path= path->tl){ void* _temp71=( void*)(( struct Cyc_Absyn_Exp*) path->hd)->r;
struct Cyc_Absyn_Exp* _temp85; struct Cyc_Absyn_Exp* _temp87; struct
_tagged_string* _temp89; struct Cyc_Absyn_Exp* _temp91; struct _tagged_string*
_temp93; struct Cyc_Absyn_Exp* _temp95; struct Cyc_Absyn_Exp* _temp97; struct
Cyc_Absyn_Exp* _temp99; _LL73: if((( struct _tunion_struct*) _temp71)->tag ==
Cyc_Absyn_Address_e_tag){ _LL86: _temp85=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*)
_temp71)->f1; goto _LL74;} else{ goto _LL75;} _LL75: if((( struct _tunion_struct*)
_temp71)->tag == Cyc_Absyn_Deref_e_tag){ _LL88: _temp87=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Deref_e_struct*) _temp71)->f1; goto _LL76;} else{ goto _LL77;}
_LL77: if((( struct _tunion_struct*) _temp71)->tag == Cyc_Absyn_StructArrow_e_tag){
_LL92: _temp91=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp71)->f1; goto _LL90; _LL90: _temp89=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp71)->f2; goto _LL78;} else{ goto _LL79;} _LL79: if((( struct _tunion_struct*)
_temp71)->tag == Cyc_Absyn_StructMember_e_tag){ _LL96: _temp95=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp71)->f1; goto _LL94; _LL94:
_temp93=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp71)->f2; goto _LL80;} else{ goto _LL81;} _LL81: if((( struct _tunion_struct*)
_temp71)->tag == Cyc_Absyn_Subscript_e_tag){ _LL100: _temp99=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp71)->f1; goto _LL98; _LL98: _temp97=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*) _temp71)->f2;
goto _LL82;} else{ goto _LL83;} _LL83: goto _LL84; _LL74: path->tl == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp101=( char*)"new_control_flow.cyc:239 path->tl == null";
struct _tagged_string _temp102; _temp102.curr= _temp101; _temp102.base= _temp101;
_temp102.last_plus_one= _temp101 + 42; _temp102;})); return Cyc_CfAbsexp_mkAddressOp(
absop); _LL76: absop= Cyc_CfAbsexp_mkDerefOp( absop); goto _LL72; _LL78: absop=
Cyc_CfAbsexp_mkDerefOp( absop); _temp93= _temp89; goto _LL80; _LL80: absop= Cyc_CfAbsexp_mkMemberOp(
absop,( void*)({ struct Cyc_CfFlowInfo_StructF_struct* _temp103=( struct Cyc_CfFlowInfo_StructF_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructF_struct)); _temp103[ 0]=({
struct Cyc_CfFlowInfo_StructF_struct _temp104; _temp104.tag= Cyc_CfFlowInfo_StructF_tag;
_temp104.f1= _temp93; _temp104;}); _temp103;})); goto _LL72; _LL82: {
unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp97); absop= Cyc_CfAbsexp_mkMemberOp(
absop,( void*)({ struct Cyc_CfFlowInfo_TupleF_struct* _temp105=( struct Cyc_CfFlowInfo_TupleF_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_TupleF_struct)); _temp105[ 0]=({ struct
Cyc_CfFlowInfo_TupleF_struct _temp106; _temp106.tag= Cyc_CfFlowInfo_TupleF_tag;
_temp106.f1=( int) i; _temp106;}); _temp105;})); goto _LL72;} _LL84:( void)
_throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct* _temp107=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp107[ 0]=({ struct Cyc_Core_Impossible_struct _temp108; _temp108.tag= Cyc_Core_Impossible_tag;
_temp108.f1=( struct _tagged_string)({ char* _temp109=( char*)"abstract_exp: unexpected acces path element";
struct _tagged_string _temp110; _temp110.curr= _temp109; _temp110.base= _temp109;
_temp110.last_plus_one= _temp109 + 44; _temp110;}); _temp108;}); _temp107;}));
_LL72:;}} return absop;} struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_abstract_exp(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e){ void* _temp111=(
void*) e->r; struct Cyc_Absyn_Exp* _temp199; struct Cyc_Absyn_Exp* _temp201;
struct Cyc_List_List* _temp203; struct Cyc_Absyn_Exp* _temp205; void* _temp207;
int _temp209; void* _temp211; void* _temp213; int _temp215; void* _temp217; void*
_temp219; void* _temp221; struct Cyc_Absyn_Enumfield* _temp223; struct Cyc_Absyn_Enumdecl*
_temp225; struct _tuple1* _temp227; void* _temp229; struct Cyc_Absyn_Fndecl*
_temp231; struct _tuple1* _temp233; void* _temp235; struct Cyc_Absyn_Vardecl*
_temp237; struct _tuple1* _temp239; void* _temp241; struct Cyc_Absyn_Vardecl*
_temp243; struct _tuple1* _temp245; void* _temp247; struct Cyc_Absyn_Vardecl*
_temp249; struct _tuple1* _temp251; void* _temp253; struct Cyc_Absyn_Vardecl*
_temp255; struct _tuple1* _temp257; struct Cyc_Absyn_Tunionfield* _temp259;
struct Cyc_Absyn_Tuniondecl* _temp261; struct Cyc_List_List* _temp263; struct
Cyc_Core_Opt* _temp265; struct Cyc_Core_Opt* _temp267; struct Cyc_Absyn_Tunionfield*
_temp269; struct Cyc_Absyn_XTuniondecl* _temp271; struct Cyc_List_List* _temp273;
struct Cyc_Core_Opt* _temp275; void* _temp277; struct Cyc_Absyn_Exp* _temp279;
struct Cyc_Core_Opt* _temp281; struct Cyc_Absyn_Exp* _temp283; struct Cyc_Absyn_Exp*
_temp285; struct Cyc_Core_Opt* _temp287; struct Cyc_Absyn_Exp* _temp289; struct
Cyc_List_List* _temp291; void* _temp293; struct Cyc_Absyn_Exp* _temp295; void*
_temp297; struct Cyc_List_List* _temp299; struct Cyc_Absyn_Exp* _temp301; struct
Cyc_Absyn_Exp* _temp303; struct Cyc_Absyn_Exp* _temp305; struct Cyc_Absyn_Exp*
_temp307; struct Cyc_Absyn_Exp* _temp309; struct Cyc_Absyn_Exp* _temp311; struct
_tagged_string* _temp313; struct Cyc_Absyn_Exp* _temp315; struct _tagged_string*
_temp317; struct Cyc_Absyn_Exp* _temp319; struct Cyc_List_List* _temp321; struct
Cyc_Absyn_Structdecl* _temp323; struct Cyc_List_List* _temp325; struct Cyc_Core_Opt*
_temp327; struct _tuple1* _temp329; struct Cyc_List_List* _temp331; void*
_temp333; struct Cyc_Absyn_Exp* _temp335; struct Cyc_Absyn_Exp* _temp337; struct
Cyc_Absyn_Exp* _temp339; struct Cyc_Absyn_Exp* _temp341; struct Cyc_Absyn_Exp*
_temp343; struct Cyc_Absyn_Stmt* _temp345; struct Cyc_Absyn_Exp* _temp347;
struct Cyc_Absyn_Exp* _temp349; struct Cyc_Absyn_Exp* _temp351; struct Cyc_Absyn_Exp*
_temp353; struct Cyc_Absyn_Vardecl* _temp355; void* _temp357; struct _tuple1*
_temp359; struct _tuple1* _temp361; struct Cyc_List_List* _temp363; struct Cyc_Absyn_Exp*
_temp365; struct Cyc_List_List* _temp367; struct Cyc_Core_Opt* _temp369; struct
Cyc_List_List* _temp371; struct _tuple2* _temp373; struct Cyc_Absyn_Fndecl*
_temp375; struct Cyc_Absyn_Exp* _temp377; _LL113: if((( struct _tunion_struct*)
_temp111)->tag == Cyc_Absyn_NoInstantiate_e_tag){ _LL200: _temp199=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_NoInstantiate_e_struct*) _temp111)->f1; goto _LL114;} else{
goto _LL115;} _LL115: if((( struct _tunion_struct*) _temp111)->tag == Cyc_Absyn_Sizeofexp_e_tag){
_LL202: _temp201=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp111)->f1; goto _LL116;} else{ goto _LL117;} _LL117: if((( struct
_tunion_struct*) _temp111)->tag == Cyc_Absyn_Instantiate_e_tag){ _LL206:
_temp205=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp111)->f1; goto _LL204; _LL204: _temp203=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Instantiate_e_struct*) _temp111)->f2; goto _LL118;} else{ goto _LL119;}
_LL119: if((( struct _tunion_struct*) _temp111)->tag == Cyc_Absyn_Const_e_tag){
_LL208: _temp207=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp111)->f1; if((
unsigned int) _temp207 > 1u?(( struct _tunion_struct*) _temp207)->tag == Cyc_Absyn_Int_c_tag:
0){ _LL212: _temp211=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp207)->f1;
if(( int) _temp211 == Cyc_Absyn_Signed){ goto _LL210;} else{ goto _LL121;}
_LL210: _temp209=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp207)->f2; if(
_temp209 == 0){ goto _LL120;} else{ goto _LL121;}} else{ goto _LL121;}} else{
goto _LL121;} _LL121: if((( struct _tunion_struct*) _temp111)->tag == Cyc_Absyn_Const_e_tag){
_LL214: _temp213=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp111)->f1; if((
unsigned int) _temp213 > 1u?(( struct _tunion_struct*) _temp213)->tag == Cyc_Absyn_Int_c_tag:
0){ _LL218: _temp217=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp213)->f1;
if(( int) _temp217 == Cyc_Absyn_Signed){ goto _LL216;} else{ goto _LL123;}
_LL216: _temp215=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp213)->f2; if(
_temp215 == 1){ goto _LL122;} else{ goto _LL123;}} else{ goto _LL123;}} else{
goto _LL123;} _LL123: if((( struct _tunion_struct*) _temp111)->tag == Cyc_Absyn_Sizeoftyp_e_tag){
_LL220: _temp219=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp111)->f1;
goto _LL124;} else{ goto _LL125;} _LL125: if((( struct _tunion_struct*) _temp111)->tag
== Cyc_Absyn_Const_e_tag){ _LL222: _temp221=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp111)->f1; goto _LL126;} else{ goto _LL127;} _LL127: if((( struct
_tunion_struct*) _temp111)->tag == Cyc_Absyn_Enum_e_tag){ _LL228: _temp227=(
struct _tuple1*)(( struct Cyc_Absyn_Enum_e_struct*) _temp111)->f1; goto _LL226;
_LL226: _temp225=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp111)->f2; goto _LL224; _LL224: _temp223=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_e_struct*) _temp111)->f3; goto _LL128;} else{ goto _LL129;}
_LL129: if((( struct _tunion_struct*) _temp111)->tag == Cyc_Absyn_Var_e_tag){
_LL234: _temp233=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp111)->f1;
goto _LL230; _LL230: _temp229=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp111)->f2; if(( unsigned int) _temp229 > 1u?(( struct _tunion_struct*)
_temp229)->tag == Cyc_Absyn_Funname_b_tag: 0){ _LL232: _temp231=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Funname_b_struct*) _temp229)->f1; goto _LL130;} else{ goto
_LL131;}} else{ goto _LL131;} _LL131: if((( struct _tunion_struct*) _temp111)->tag
== Cyc_Absyn_Var_e_tag){ _LL240: _temp239=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*)
_temp111)->f1; goto _LL236; _LL236: _temp235=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp111)->f2; if(( unsigned int) _temp235 > 1u?(( struct _tunion_struct*)
_temp235)->tag == Cyc_Absyn_Global_b_tag: 0){ _LL238: _temp237=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Global_b_struct*) _temp235)->f1; goto _LL132;} else{ goto
_LL133;}} else{ goto _LL133;} _LL133: if((( struct _tunion_struct*) _temp111)->tag
== Cyc_Absyn_Var_e_tag){ _LL246: _temp245=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*)
_temp111)->f1; goto _LL242; _LL242: _temp241=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp111)->f2; if(( unsigned int) _temp241 > 1u?(( struct _tunion_struct*)
_temp241)->tag == Cyc_Absyn_Param_b_tag: 0){ _LL244: _temp243=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Param_b_struct*) _temp241)->f1; goto _LL134;} else{ goto _LL135;}}
else{ goto _LL135;} _LL135: if((( struct _tunion_struct*) _temp111)->tag == Cyc_Absyn_Var_e_tag){
_LL252: _temp251=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp111)->f1;
goto _LL248; _LL248: _temp247=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp111)->f2; if(( unsigned int) _temp247 > 1u?(( struct _tunion_struct*)
_temp247)->tag == Cyc_Absyn_Local_b_tag: 0){ _LL250: _temp249=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Local_b_struct*) _temp247)->f1; goto _LL136;} else{ goto _LL137;}}
else{ goto _LL137;} _LL137: if((( struct _tunion_struct*) _temp111)->tag == Cyc_Absyn_Var_e_tag){
_LL258: _temp257=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp111)->f1;
goto _LL254; _LL254: _temp253=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp111)->f2; if(( unsigned int) _temp253 > 1u?(( struct _tunion_struct*)
_temp253)->tag == Cyc_Absyn_Pat_b_tag: 0){ _LL256: _temp255=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Pat_b_struct*) _temp253)->f1; goto _LL138;} else{ goto _LL139;}}
else{ goto _LL139;} _LL139: if((( struct _tunion_struct*) _temp111)->tag == Cyc_Absyn_Tunion_e_tag){
_LL268: _temp267=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp111)->f1; goto _LL266; _LL266: _temp265=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Tunion_e_struct*) _temp111)->f2; goto _LL264; _LL264: _temp263=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp111)->f3; goto
_LL262; _LL262: _temp261=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp111)->f4; goto _LL260; _LL260: _temp259=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp111)->f5; goto _LL140;} else{ goto
_LL141;} _LL141: if((( struct _tunion_struct*) _temp111)->tag == Cyc_Absyn_XTunion_e_tag){
_LL276: _temp275=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_XTunion_e_struct*)
_temp111)->f1; goto _LL274; _LL274: _temp273=( struct Cyc_List_List*)(( struct
Cyc_Absyn_XTunion_e_struct*) _temp111)->f2; goto _LL272; _LL272: _temp271=(
struct Cyc_Absyn_XTuniondecl*)(( struct Cyc_Absyn_XTunion_e_struct*) _temp111)->f3;
goto _LL270; _LL270: _temp269=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_e_struct*)
_temp111)->f4; goto _LL142;} else{ goto _LL143;} _LL143: if((( struct
_tunion_struct*) _temp111)->tag == Cyc_Absyn_Malloc_e_tag){ _LL278: _temp277=(
void*)(( struct Cyc_Absyn_Malloc_e_struct*) _temp111)->f1; goto _LL144;} else{
goto _LL145;} _LL145: if((( struct _tunion_struct*) _temp111)->tag == Cyc_Absyn_AssignOp_e_tag){
_LL284: _temp283=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp111)->f1; goto _LL282; _LL282: _temp281=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp111)->f2; if( _temp281 == 0){ goto _LL280;}
else{ goto _LL147;} _LL280: _temp279=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp111)->f3; goto _LL146;} else{ goto _LL147;} _LL147: if((( struct
_tunion_struct*) _temp111)->tag == Cyc_Absyn_AssignOp_e_tag){ _LL290: _temp289=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp111)->f1;
goto _LL288; _LL288: _temp287=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp111)->f2; goto _LL286; _LL286: _temp285=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp111)->f3; goto _LL148;} else{ goto _LL149;}
_LL149: if((( struct _tunion_struct*) _temp111)->tag == Cyc_Absyn_Primop_e_tag){
_LL294: _temp293=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp111)->f1;
goto _LL292; _LL292: _temp291=( struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp111)->f2; goto _LL150;} else{ goto _LL151;} _LL151: if((( struct
_tunion_struct*) _temp111)->tag == Cyc_Absyn_Cast_e_tag){ _LL298: _temp297=(
void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp111)->f1; goto _LL296; _LL296:
_temp295=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*) _temp111)->f2;
goto _LL152;} else{ goto _LL153;} _LL153: if((( struct _tunion_struct*) _temp111)->tag
== Cyc_Absyn_FnCall_e_tag){ _LL302: _temp301=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_FnCall_e_struct*) _temp111)->f1; goto _LL300; _LL300: _temp299=(
struct Cyc_List_List*)(( struct Cyc_Absyn_FnCall_e_struct*) _temp111)->f2; goto
_LL154;} else{ goto _LL155;} _LL155: if((( struct _tunion_struct*) _temp111)->tag
== Cyc_Absyn_Subscript_e_tag){ _LL306: _temp305=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp111)->f1; goto _LL304; _LL304:
_temp303=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp111)->f2; goto _LL156;} else{ goto _LL157;} _LL157: if((( struct
_tunion_struct*) _temp111)->tag == Cyc_Absyn_New_e_tag){ _LL308: _temp307=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*) _temp111)->f1; goto
_LL158;} else{ goto _LL159;} _LL159: if((( struct _tunion_struct*) _temp111)->tag
== Cyc_Absyn_Address_e_tag){ _LL310: _temp309=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Address_e_struct*) _temp111)->f1; goto _LL160;} else{ goto _LL161;}
_LL161: if((( struct _tunion_struct*) _temp111)->tag == Cyc_Absyn_Deref_e_tag){
_LL312: _temp311=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp111)->f1; goto _LL162;} else{ goto _LL163;} _LL163: if((( struct
_tunion_struct*) _temp111)->tag == Cyc_Absyn_StructArrow_e_tag){ _LL316:
_temp315=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp111)->f1; goto _LL314; _LL314: _temp313=( struct _tagged_string*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp111)->f2; goto _LL164;} else{ goto _LL165;}
_LL165: if((( struct _tunion_struct*) _temp111)->tag == Cyc_Absyn_StructMember_e_tag){
_LL320: _temp319=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp111)->f1; goto _LL318; _LL318: _temp317=( struct _tagged_string*)(( struct
Cyc_Absyn_StructMember_e_struct*) _temp111)->f2; goto _LL166;} else{ goto _LL167;}
_LL167: if((( struct _tunion_struct*) _temp111)->tag == Cyc_Absyn_Tuple_e_tag){
_LL322: _temp321=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp111)->f1; goto _LL168;} else{ goto _LL169;} _LL169: if((( struct
_tunion_struct*) _temp111)->tag == Cyc_Absyn_Struct_e_tag){ _LL330: _temp329=(
struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*) _temp111)->f1; goto _LL328;
_LL328: _temp327=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp111)->f2; goto _LL326; _LL326: _temp325=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp111)->f3; goto _LL324; _LL324: _temp323=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*) _temp111)->f4;
goto _LL170;} else{ goto _LL171;} _LL171: if((( struct _tunion_struct*) _temp111)->tag
== Cyc_Absyn_Array_e_tag){ _LL332: _temp331=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Array_e_struct*) _temp111)->f1; goto _LL172;} else{ goto _LL173;}
_LL173: if((( struct _tunion_struct*) _temp111)->tag == Cyc_Absyn_Increment_e_tag){
_LL336: _temp335=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp111)->f1; goto _LL334; _LL334: _temp333=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp111)->f2; goto _LL174;} else{ goto _LL175;} _LL175: if((( struct
_tunion_struct*) _temp111)->tag == Cyc_Absyn_Throw_e_tag){ _LL338: _temp337=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*) _temp111)->f1; goto
_LL176;} else{ goto _LL177;} _LL177: if((( struct _tunion_struct*) _temp111)->tag
== Cyc_Absyn_Conditional_e_tag){ _LL344: _temp343=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp111)->f1; goto _LL342; _LL342:
_temp341=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp111)->f2; goto _LL340; _LL340: _temp339=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Conditional_e_struct*) _temp111)->f3; goto _LL178;} else{ goto _LL179;}
_LL179: if((( struct _tunion_struct*) _temp111)->tag == Cyc_Absyn_StmtExp_e_tag){
_LL346: _temp345=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp111)->f1; goto _LL180;} else{ goto _LL181;} _LL181: if((( struct
_tunion_struct*) _temp111)->tag == Cyc_Absyn_SeqExp_e_tag){ _LL350: _temp349=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*) _temp111)->f1; goto
_LL348; _LL348: _temp347=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp111)->f2; goto _LL182;} else{ goto _LL183;} _LL183: if((( struct
_tunion_struct*) _temp111)->tag == Cyc_Absyn_Comprehension_e_tag){ _LL356:
_temp355=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp111)->f1; goto _LL354; _LL354: _temp353=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Comprehension_e_struct*) _temp111)->f2; goto _LL352; _LL352: _temp351=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*) _temp111)->f3;
goto _LL184;} else{ goto _LL185;} _LL185: if((( struct _tunion_struct*) _temp111)->tag
== Cyc_Absyn_Var_e_tag){ _LL360: _temp359=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*)
_temp111)->f1; goto _LL358; _LL358: _temp357=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp111)->f2; if(( int) _temp357 == Cyc_Absyn_Unresolved_b){ goto _LL186;}
else{ goto _LL187;}} else{ goto _LL187;} _LL187: if((( struct _tunion_struct*)
_temp111)->tag == Cyc_Absyn_UnknownId_e_tag){ _LL362: _temp361=( struct _tuple1*)((
struct Cyc_Absyn_UnknownId_e_struct*) _temp111)->f1; goto _LL188;} else{ goto
_LL189;} _LL189: if((( struct _tunion_struct*) _temp111)->tag == Cyc_Absyn_UnknownCall_e_tag){
_LL366: _temp365=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp111)->f1; goto _LL364; _LL364: _temp363=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnknownCall_e_struct*) _temp111)->f2; goto _LL190;} else{ goto _LL191;}
_LL191: if((( struct _tunion_struct*) _temp111)->tag == Cyc_Absyn_UnresolvedMem_e_tag){
_LL370: _temp369=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp111)->f1; goto _LL368; _LL368: _temp367=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnresolvedMem_e_struct*) _temp111)->f2; goto _LL192;} else{ goto
_LL193;} _LL193: if((( struct _tunion_struct*) _temp111)->tag == Cyc_Absyn_CompoundLit_e_tag){
_LL374: _temp373=( struct _tuple2*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp111)->f1; goto _LL372; _LL372: _temp371=( struct Cyc_List_List*)(( struct
Cyc_Absyn_CompoundLit_e_struct*) _temp111)->f2; goto _LL194;} else{ goto _LL195;}
_LL195: if((( struct _tunion_struct*) _temp111)->tag == Cyc_Absyn_Codegen_e_tag){
_LL376: _temp375=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*)
_temp111)->f1; goto _LL196;} else{ goto _LL197;} _LL197: if((( struct
_tunion_struct*) _temp111)->tag == Cyc_Absyn_Fill_e_tag){ _LL378: _temp377=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*) _temp111)->f1; goto
_LL198;} else{ goto _LL112;} _LL114: _temp201= _temp199; goto _LL116; _LL116:
_temp205= _temp201; goto _LL118; _LL118: return Cyc_NewControlFlow_abstract_exp(
env, _temp205); _LL120: env.access_path == 0? 0:(( int(*)( struct _tagged_string
msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({ char* _temp379=( char*)"new_control_flow.cyc:263 env.access_path==null";
struct _tagged_string _temp380; _temp380.curr= _temp379; _temp380.base= _temp379;
_temp380.last_plus_one= _temp379 + 47; _temp380;})); return({ struct Cyc_NewControlFlow_AbsSyn
_temp381; _temp381.when_true=({ struct Cyc_NewControlFlow_AbsSynOne _temp383;
_temp383.inner_exp=( void*) Cyc_CfAbsexp_mkBottomAE(); _temp383.assigns=( void*)
Cyc_CfAbsexp_mkBottomAE(); _temp383;}); _temp381.when_false=({ struct Cyc_NewControlFlow_AbsSynOne
_temp382; _temp382.inner_exp=( void*) Cyc_CfAbsexp_mkSkipAE(); _temp382.assigns=(
void*) Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp());
_temp382;}); _temp381.lvalues= 0; _temp381;}); _LL122: env.access_path == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp384=( char*)"new_control_flow.cyc:268 env.access_path==null";
struct _tagged_string _temp385; _temp385.curr= _temp384; _temp385.base= _temp384;
_temp385.last_plus_one= _temp384 + 47; _temp385;})); return({ struct Cyc_NewControlFlow_AbsSyn
_temp386; _temp386.when_true=({ struct Cyc_NewControlFlow_AbsSynOne _temp388;
_temp388.inner_exp=( void*) Cyc_CfAbsexp_mkSkipAE(); _temp388.assigns=( void*)
Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp()); _temp388;});
_temp386.when_false=({ struct Cyc_NewControlFlow_AbsSynOne _temp387; _temp387.inner_exp=(
void*) Cyc_CfAbsexp_mkBottomAE(); _temp387.assigns=( void*) Cyc_CfAbsexp_mkBottomAE();
_temp387;}); _temp386.lvalues= 0; _temp386;}); _LL124: goto _LL126; _LL126: goto
_LL128; _LL128: goto _LL130; _LL130: return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(),
0, Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp())); _LL132:
return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(), Cyc_NewControlFlow_use_it_list(),
Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp())); _LL134:
_temp249= _temp243; goto _LL136; _LL136: _temp255= _temp249; goto _LL138; _LL138: {
void* absop= Cyc_NewControlFlow_destruct_path( env, Cyc_CfAbsexp_mkLocalOp(
_temp255)); return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(),({
struct Cyc_List_List* _temp389=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp389->hd=( void*) absop; _temp389->tl= 0; _temp389;}),
Cyc_NewControlFlow_make_assigns( env, absop));} _LL140: _temp273= _temp263;
_temp269= _temp259; goto _LL142; _LL142: env.access_path == 0? 0:(( int(*)(
struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({
char* _temp390=( char*)"new_control_flow.cyc:299 env.access_path == null";
struct _tagged_string _temp391; _temp391.curr= _temp390; _temp391.base= _temp390;
_temp391.last_plus_one= _temp390 + 49; _temp391;})); if( _temp273 == 0){ return
Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(), 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));} Cyc_NewControlFlow_add_tunion_root( env, e,
_temp269);{ void* es_exp= Cyc_CfAbsexp_mkSkipAE(); void* useexp= Cyc_CfAbsexp_mkSkipAE();{
int i= 0; for( 0; _temp273 != 0; _temp273= _temp273->tl, ++ i){ struct Cyc_NewControlFlow_AbsEnv
elt_env= Cyc_NewControlFlow_change_lhs( env,({ struct Cyc_List_List* _temp392=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp392->hd=(
void*) Cyc_CfAbsexp_mkMemberOp( Cyc_CfAbsexp_mkMallocOp( e),( void*)({ struct
Cyc_CfFlowInfo_TupleF_struct* _temp393=( struct Cyc_CfFlowInfo_TupleF_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_TupleF_struct)); _temp393[ 0]=({ struct
Cyc_CfFlowInfo_TupleF_struct _temp394; _temp394.tag= Cyc_CfFlowInfo_TupleF_tag;
_temp394.f1= i; _temp394;}); _temp393;})); _temp392->tl= 0; _temp392;})); struct
Cyc_NewControlFlow_AbsSyn syn= Cyc_NewControlFlow_abstract_exp( elt_env,( struct
Cyc_Absyn_Exp*) _temp273->hd); void* _temp397; void* _temp399; struct Cyc_NewControlFlow_AbsSynOne
_temp395= Cyc_NewControlFlow_meet_absexp( syn); _LL400: _temp399=( void*)
_temp395.inner_exp; goto _LL398; _LL398: _temp397=( void*) _temp395.assigns;
goto _LL396; _LL396: es_exp= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG,
_temp399, es_exp); useexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_mkAnyOrderG(),
_temp397, useexp);}} return Cyc_NewControlFlow_malloc_and_init( env, e, es_exp,
useexp);} _LL144: Cyc_NewControlFlow_add_malloc_root( env, e, _temp277); return
Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkMallocAE( e), 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkAddressOp( Cyc_CfAbsexp_mkMallocOp( e)))); _LL146: { struct
Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp( env, _temp283);
struct Cyc_List_List* lhs= syn1.lvalues == 0? Cyc_NewControlFlow_use_it_list():
syn1.lvalues; struct Cyc_NewControlFlow_AbsSyn syn2= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_change_lhs( env, lhs), _temp279); void* _temp403; void*
_temp405; struct Cyc_NewControlFlow_AbsSynOne _temp401= Cyc_NewControlFlow_meet_absexp(
syn1); _LL406: _temp405=( void*) _temp401.inner_exp; goto _LL404; _LL404:
_temp403=( void*) _temp401.assigns; goto _LL402; _LL402: { void* _temp409; void*
_temp411; struct Cyc_NewControlFlow_AbsSynOne _temp407= Cyc_NewControlFlow_meet_absexp(
syn2); _LL412: _temp411=( void*) _temp407.inner_exp; goto _LL410; _LL410:
_temp409=( void*) _temp407.assigns; goto _LL408; _LL408: { void* ansexp= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG,
_temp405, _temp411), _temp409); return Cyc_NewControlFlow_mkSyn_tf( ansexp, 0,
_temp403);}}} _LL148: return Cyc_NewControlFlow_use_exp_unordered( env,({ struct
Cyc_List_List* _temp413=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp413->hd=( void*) _temp289; _temp413->tl=({ struct Cyc_List_List* _temp414=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp414->hd=(
void*) _temp285; _temp414->tl= 0; _temp414;}); _temp413;}), 0); _LL150: return
Cyc_NewControlFlow_use_exp_unordered( env, _temp291, 0); _LL152: { int both_leaf=
1;{ void* _temp415=( void*)( _temp295->topt)->v; struct Cyc_Absyn_Structdecl**
_temp423; struct Cyc_List_List* _temp425; struct _tuple1* _temp427; struct Cyc_List_List*
_temp429; _LL417: if(( unsigned int) _temp415 > 4u?(( struct _tunion_struct*)
_temp415)->tag == Cyc_Absyn_StructType_tag: 0){ _LL428: _temp427=( struct
_tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp415)->f1; goto _LL426;
_LL426: _temp425=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp415)->f2; goto _LL424; _LL424: _temp423=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp415)->f3; goto _LL418;} else{ goto
_LL419;} _LL419: if(( unsigned int) _temp415 > 4u?(( struct _tunion_struct*)
_temp415)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL430: _temp429=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp415)->f1; goto _LL420;} else{ goto
_LL421;} _LL421: goto _LL422; _LL418: goto _LL420; _LL420: both_leaf= 0; goto
_LL416; _LL422: goto _LL416; _LL416:;}{ void* _temp431=( void*)( e->topt)->v;
struct Cyc_Absyn_Structdecl** _temp439; struct Cyc_List_List* _temp441; struct
_tuple1* _temp443; struct Cyc_List_List* _temp445; _LL433: if(( unsigned int)
_temp431 > 4u?(( struct _tunion_struct*) _temp431)->tag == Cyc_Absyn_StructType_tag:
0){ _LL444: _temp443=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp431)->f1; goto _LL442; _LL442: _temp441=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp431)->f2; goto _LL440; _LL440: _temp439=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp431)->f3;
goto _LL434;} else{ goto _LL435;} _LL435: if(( unsigned int) _temp431 > 4u?((
struct _tunion_struct*) _temp431)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL446:
_temp445=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp431)->f1;
goto _LL436;} else{ goto _LL437;} _LL437: goto _LL438; _LL434: goto _LL436;
_LL436: both_leaf= 0; goto _LL432; _LL438: goto _LL432; _LL432:;} if( both_leaf){
return Cyc_NewControlFlow_abstract_exp( env, _temp295);} return Cyc_NewControlFlow_use_exp_unordered(
env,({ struct Cyc_List_List* _temp447=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp447->hd=( void*) _temp295; _temp447->tl= 0;
_temp447;}), env.lhsides);} _LL154: return Cyc_NewControlFlow_use_exp_unordered(
env,({ struct Cyc_List_List* _temp448=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp448->hd=( void*) _temp301; _temp448->tl=
_temp299; _temp448;}), 0); _LL156: { void* _temp449= Cyc_Tcutil_compress(( void*)(
_temp305->topt)->v); struct Cyc_List_List* _temp455; _LL451: if(( unsigned int)
_temp449 > 4u?(( struct _tunion_struct*) _temp449)->tag == Cyc_Absyn_TupleType_tag:
0){ _LL456: _temp455=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp449)->f1; goto _LL452;} else{ goto _LL453;} _LL453: goto _LL454; _LL452:
return Cyc_NewControlFlow_descend_path( env, e, _temp305); _LL454: return Cyc_NewControlFlow_use_exp_unordered(
env,({ struct Cyc_List_List* _temp457=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp457->hd=( void*) _temp305; _temp457->tl=({
struct Cyc_List_List* _temp458=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp458->hd=( void*) _temp303; _temp458->tl= 0;
_temp458;}); _temp457;}), Cyc_NewControlFlow_use_it_list()); _LL450:;} _LL158: {
void* _temp459=( void*) _temp307->r; struct Cyc_Absyn_Structdecl* _temp467;
struct Cyc_List_List* _temp469; struct Cyc_Core_Opt* _temp471; struct _tuple1*
_temp473; struct Cyc_List_List* _temp475; _LL461: if((( struct _tunion_struct*)
_temp459)->tag == Cyc_Absyn_Struct_e_tag){ _LL474: _temp473=( struct _tuple1*)((
struct Cyc_Absyn_Struct_e_struct*) _temp459)->f1; goto _LL472; _LL472: _temp471=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*) _temp459)->f2; goto
_LL470; _LL470: _temp469=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp459)->f3; goto _LL468; _LL468: _temp467=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp459)->f4; goto _LL462;} else{ goto
_LL463;} _LL463: if((( struct _tunion_struct*) _temp459)->tag == Cyc_Absyn_Tuple_e_tag){
_LL476: _temp475=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp459)->f1; goto _LL464;} else{ goto _LL465;} _LL465: goto _LL466; _LL462:
goto _LL464; _LL464: { void* malloc_op= Cyc_CfAbsexp_mkMallocOp( e); Cyc_NewControlFlow_add_malloc_root(
env, e,( void*)( _temp307->topt)->v);{ struct Cyc_NewControlFlow_AbsEnv
inner_env= Cyc_NewControlFlow_change_lhs( env,({ struct Cyc_List_List* _temp477=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp477->hd=(
void*) malloc_op; _temp477->tl= 0; _temp477;})); inner_env.access_path= 0;{ void*
_temp480; void* _temp482; struct Cyc_NewControlFlow_AbsSynOne _temp478= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( inner_env, _temp307)); _LL483: _temp482=( void*)
_temp478.inner_exp; goto _LL481; _LL481: _temp480=( void*) _temp478.assigns;
goto _LL479; _LL479: { struct Cyc_NewControlFlow_AbsSyn ans= Cyc_NewControlFlow_malloc_and_init(
env, e, _temp482, _temp480); if( env.access_path != 0){ void* _temp484=( void*)((
struct Cyc_Absyn_Exp*)( env.access_path)->hd)->r; struct Cyc_Absyn_Exp* _temp494;
struct Cyc_Absyn_Exp* _temp496; struct Cyc_Absyn_Exp* _temp498; struct
_tagged_string* _temp500; struct Cyc_Absyn_Exp* _temp502; _LL486: if((( struct
_tunion_struct*) _temp484)->tag == Cyc_Absyn_Subscript_e_tag){ _LL497: _temp496=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*) _temp484)->f1;
goto _LL495; _LL495: _temp494=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp484)->f2; goto _LL487;} else{ goto _LL488;} _LL488: if((( struct
_tunion_struct*) _temp484)->tag == Cyc_Absyn_Deref_e_tag){ _LL499: _temp498=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*) _temp484)->f1; goto
_LL489;} else{ goto _LL490;} _LL490: if((( struct _tunion_struct*) _temp484)->tag
== Cyc_Absyn_StructArrow_e_tag){ _LL503: _temp502=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp484)->f1; goto _LL501; _LL501:
_temp500=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp484)->f2; goto _LL491;} else{ goto _LL492;} _LL492: goto _LL493; _LL487:
Cyc_Evexp_eval_const_uint_exp( _temp494) ==( unsigned int) 0? 0:(( int(*)(
struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({
char* _temp504=( char*)"new_control_flow.cyc:410 Evexp::eval_const_uint_exp(e2) == 0";
struct _tagged_string _temp505; _temp505.curr= _temp504; _temp505.base= _temp504;
_temp505.last_plus_one= _temp504 + 61; _temp505;})); goto _LL489; _LL489: env.access_path=(
env.access_path)->tl; ans.lvalues=({ struct Cyc_List_List* _temp506=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp506->hd=( void*) Cyc_NewControlFlow_destruct_path(
env, malloc_op); _temp506->tl= 0; _temp506;}); goto _LL485; _LL491: env.access_path=({
struct Cyc_List_List* _temp507=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp507->hd=( void*) Cyc_Absyn_structmember_exp(
_temp502, _temp500, 0); _temp507->tl=( env.access_path)->tl; _temp507;}); ans.lvalues=({
struct Cyc_List_List* _temp508=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp508->hd=( void*) Cyc_NewControlFlow_destruct_path(
env, malloc_op); _temp508->tl= 0; _temp508;}); goto _LL485; _LL493:( void)
_throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct* _temp509=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp509[ 0]=({ struct Cyc_Core_Impossible_struct _temp510; _temp510.tag= Cyc_Core_Impossible_tag;
_temp510.f1=( struct _tagged_string)({ char* _temp511=( char*)"bad malloc access path";
struct _tagged_string _temp512; _temp512.curr= _temp511; _temp512.base= _temp511;
_temp512.last_plus_one= _temp511 + 23; _temp512;}); _temp510;}); _temp509;}));
_LL485:;} return ans;}}}} _LL466: return Cyc_NewControlFlow_use_exp_unordered(
env,({ struct Cyc_List_List* _temp513=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp513->hd=( void*) _temp307; _temp513->tl= 0;
_temp513;}), 0); _LL460:;} _LL160:{ void* _temp514=( void*) _temp309->r; struct
Cyc_Absyn_Structdecl* _temp522; struct Cyc_List_List* _temp524; struct Cyc_Core_Opt*
_temp526; struct _tuple1* _temp528; struct Cyc_List_List* _temp530; _LL516: if(((
struct _tunion_struct*) _temp514)->tag == Cyc_Absyn_Struct_e_tag){ _LL529:
_temp528=( struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*) _temp514)->f1;
goto _LL527; _LL527: _temp526=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp514)->f2; goto _LL525; _LL525: _temp524=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp514)->f3; goto _LL523; _LL523: _temp522=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*) _temp514)->f4;
goto _LL517;} else{ goto _LL518;} _LL518: if((( struct _tunion_struct*) _temp514)->tag
== Cyc_Absyn_Tuple_e_tag){ _LL531: _temp530=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Tuple_e_struct*) _temp514)->f1; goto _LL519;} else{ goto _LL520;}
_LL520: goto _LL521; _LL517: goto _LL519; _LL519: { void* malloc_op= Cyc_CfAbsexp_mkMallocOp(
e); Cyc_NewControlFlow_add_malloc_root( env, e,( void*)( _temp309->topt)->v);{
struct Cyc_NewControlFlow_AbsEnv inner_env= Cyc_NewControlFlow_change_lhs( env,({
struct Cyc_List_List* _temp532=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp532->hd=( void*) malloc_op; _temp532->tl= 0;
_temp532;})); inner_env.access_path= 0;{ void* _temp535; void* _temp537; struct
Cyc_NewControlFlow_AbsSynOne _temp533= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
inner_env, _temp309)); _LL538: _temp537=( void*) _temp533.inner_exp; goto _LL536;
_LL536: _temp535=( void*) _temp533.assigns; goto _LL534; _LL534: { struct Cyc_NewControlFlow_AbsSyn
ans= Cyc_NewControlFlow_malloc_and_init( env, e, _temp537, _temp535); if( env.access_path
!= 0){ void* _temp539=( void*)(( struct Cyc_Absyn_Exp*)( env.access_path)->hd)->r;
struct Cyc_Absyn_Exp* _temp549; struct Cyc_Absyn_Exp* _temp551; struct Cyc_Absyn_Exp*
_temp553; struct _tagged_string* _temp555; struct Cyc_Absyn_Exp* _temp557;
_LL541: if((( struct _tunion_struct*) _temp539)->tag == Cyc_Absyn_Subscript_e_tag){
_LL552: _temp551=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp539)->f1; goto _LL550; _LL550: _temp549=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Subscript_e_struct*) _temp539)->f2; goto _LL542;} else{ goto _LL543;}
_LL543: if((( struct _tunion_struct*) _temp539)->tag == Cyc_Absyn_Deref_e_tag){
_LL554: _temp553=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp539)->f1; goto _LL544;} else{ goto _LL545;} _LL545: if((( struct
_tunion_struct*) _temp539)->tag == Cyc_Absyn_StructArrow_e_tag){ _LL558:
_temp557=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp539)->f1; goto _LL556; _LL556: _temp555=( struct _tagged_string*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp539)->f2; goto _LL546;} else{ goto _LL547;}
_LL547: goto _LL548; _LL542: Cyc_Evexp_eval_const_uint_exp( _temp549) ==(
unsigned int) 0? 0:(( int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)((
struct _tagged_string)({ char* _temp559=( char*)"new_control_flow.cyc:447 Evexp::eval_const_uint_exp(e2) == 0";
struct _tagged_string _temp560; _temp560.curr= _temp559; _temp560.base= _temp559;
_temp560.last_plus_one= _temp559 + 61; _temp560;})); goto _LL544; _LL544: env.access_path=(
env.access_path)->tl; ans.lvalues=({ struct Cyc_List_List* _temp561=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp561->hd=( void*) Cyc_NewControlFlow_destruct_path(
env, malloc_op); _temp561->tl= 0; _temp561;}); goto _LL540; _LL546: env.access_path=({
struct Cyc_List_List* _temp562=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp562->hd=( void*) Cyc_Absyn_structmember_exp(
_temp557, _temp555, 0); _temp562->tl=( env.access_path)->tl; _temp562;}); ans.lvalues=({
struct Cyc_List_List* _temp563=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp563->hd=( void*) Cyc_NewControlFlow_destruct_path(
env, malloc_op); _temp563->tl= 0; _temp563;}); goto _LL540; _LL548:( void)
_throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct* _temp564=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp564[ 0]=({ struct Cyc_Core_Impossible_struct _temp565; _temp565.tag= Cyc_Core_Impossible_tag;
_temp565.f1=( struct _tagged_string)({ char* _temp566=( char*)"bad malloc access path";
struct _tagged_string _temp567; _temp567.curr= _temp566; _temp567.base= _temp566;
_temp567.last_plus_one= _temp566 + 23; _temp567;}); _temp565;}); _temp564;}));
_LL540:;} return ans;}}}} _LL521: goto _LL515; _LL515:;}{ struct Cyc_List_List*
_temp568= env.access_path; struct Cyc_List_List _temp580; struct Cyc_List_List*
_temp581; struct Cyc_Absyn_Exp* _temp583; struct Cyc_Absyn_Exp _temp585; struct
Cyc_Position_Segment* _temp586; void* _temp588; struct Cyc_Absyn_Exp* _temp590;
struct Cyc_Absyn_Exp* _temp592; struct Cyc_Core_Opt* _temp594; struct Cyc_List_List
_temp596; struct Cyc_List_List* _temp597; struct Cyc_Absyn_Exp* _temp599; struct
Cyc_Absyn_Exp _temp601; struct Cyc_Position_Segment* _temp602; void* _temp604;
struct Cyc_Absyn_Exp* _temp606; struct Cyc_Core_Opt* _temp608; struct Cyc_List_List
_temp610; struct Cyc_List_List* _temp611; struct Cyc_Absyn_Exp* _temp613; struct
Cyc_Absyn_Exp _temp615; struct Cyc_Position_Segment* _temp616; void* _temp618;
struct _tagged_string* _temp620; struct Cyc_Absyn_Exp* _temp622; struct Cyc_Core_Opt*
_temp624; _LL570: if( _temp568 == 0){ goto _LL571;} else{ goto _LL572;} _LL572:
if( _temp568 == 0){ goto _LL574;} else{ _temp580=* _temp568; _LL584: _temp583=(
struct Cyc_Absyn_Exp*) _temp580.hd; _temp585=* _temp583; _LL595: _temp594=(
struct Cyc_Core_Opt*) _temp585.topt; goto _LL589; _LL589: _temp588=( void*)
_temp585.r; if((( struct _tunion_struct*) _temp588)->tag == Cyc_Absyn_Subscript_e_tag){
_LL593: _temp592=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp588)->f1; goto _LL591; _LL591: _temp590=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Subscript_e_struct*) _temp588)->f2; goto _LL587;} else{ goto _LL574;}
_LL587: _temp586=( struct Cyc_Position_Segment*) _temp585.loc; goto _LL582;
_LL582: _temp581=( struct Cyc_List_List*) _temp580.tl; goto _LL573;} _LL574: if(
_temp568 == 0){ goto _LL576;} else{ _temp596=* _temp568; _LL600: _temp599=(
struct Cyc_Absyn_Exp*) _temp596.hd; _temp601=* _temp599; _LL609: _temp608=(
struct Cyc_Core_Opt*) _temp601.topt; goto _LL605; _LL605: _temp604=( void*)
_temp601.r; if((( struct _tunion_struct*) _temp604)->tag == Cyc_Absyn_Deref_e_tag){
_LL607: _temp606=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp604)->f1; goto _LL603;} else{ goto _LL576;} _LL603: _temp602=( struct Cyc_Position_Segment*)
_temp601.loc; goto _LL598; _LL598: _temp597=( struct Cyc_List_List*) _temp596.tl;
goto _LL575;} _LL576: if( _temp568 == 0){ goto _LL578;} else{ _temp610=*
_temp568; _LL614: _temp613=( struct Cyc_Absyn_Exp*) _temp610.hd; _temp615=*
_temp613; _LL625: _temp624=( struct Cyc_Core_Opt*) _temp615.topt; goto _LL619;
_LL619: _temp618=( void*) _temp615.r; if((( struct _tunion_struct*) _temp618)->tag
== Cyc_Absyn_StructArrow_e_tag){ _LL623: _temp622=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp618)->f1; goto _LL621; _LL621:
_temp620=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp618)->f2; goto _LL617;} else{ goto _LL578;} _LL617: _temp616=( struct Cyc_Position_Segment*)
_temp615.loc; goto _LL612; _LL612: _temp611=( struct Cyc_List_List*) _temp610.tl;
goto _LL577;} _LL578: goto _LL579; _LL571: env.access_path=({ struct Cyc_List_List*
_temp626=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp626->hd=( void*) e; _temp626->tl= env.access_path; _temp626;}); goto _LL569;
_LL573: Cyc_Evexp_eval_const_uint_exp( _temp590) ==( unsigned int) 0? 0:(( int(*)(
struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({
char* _temp627=( char*)"new_control_flow.cyc:475 Evexp::eval_const_uint_exp(e2) == 0";
struct _tagged_string _temp628; _temp628.curr= _temp627; _temp628.base= _temp627;
_temp628.last_plus_one= _temp627 + 61; _temp628;})); _temp597= _temp581; goto
_LL575; _LL575: env.access_path= _temp597; goto _LL569; _LL577: env.access_path=({
struct Cyc_List_List* _temp629=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp629->hd=( void*) Cyc_Absyn_structmember_exp(
_temp622, _temp620, 0); _temp629->tl= _temp611; _temp629;}); goto _LL569; _LL579:(
void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp630=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp630[ 0]=({ struct Cyc_Core_Impossible_struct _temp631; _temp631.tag= Cyc_Core_Impossible_tag;
_temp631.f1=( struct _tagged_string)({ char* _temp632=( char*)"abstract_exp: bad access path for &";
struct _tagged_string _temp633; _temp633.curr= _temp632; _temp633.base= _temp632;
_temp633.last_plus_one= _temp632 + 36; _temp633;}); _temp631;}); _temp630;}));
_LL569:;} return Cyc_NewControlFlow_abstract_exp( env, _temp309); _LL162: {
struct Cyc_List_List* _temp634= env.access_path; struct Cyc_List_List _temp640;
struct Cyc_List_List* _temp641; struct Cyc_Absyn_Exp* _temp643; struct Cyc_Absyn_Exp
_temp645; struct Cyc_Position_Segment* _temp646; void* _temp648; struct Cyc_Absyn_Exp*
_temp650; struct Cyc_Core_Opt* _temp652; _LL636: if( _temp634 == 0){ goto _LL638;}
else{ _temp640=* _temp634; _LL644: _temp643=( struct Cyc_Absyn_Exp*) _temp640.hd;
_temp645=* _temp643; _LL653: _temp652=( struct Cyc_Core_Opt*) _temp645.topt;
goto _LL649; _LL649: _temp648=( void*) _temp645.r; if((( struct _tunion_struct*)
_temp648)->tag == Cyc_Absyn_Address_e_tag){ _LL651: _temp650=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Address_e_struct*) _temp648)->f1; goto _LL647;} else{ goto
_LL638;} _LL647: _temp646=( struct Cyc_Position_Segment*) _temp645.loc; goto
_LL642; _LL642: _temp641=( struct Cyc_List_List*) _temp640.tl; goto _LL637;}
_LL638: goto _LL639; _LL637: env.access_path= _temp641;{ struct Cyc_NewControlFlow_AbsSyn
syn1= Cyc_NewControlFlow_abstract_exp( env, _temp311); if( syn1.lvalues == 0){
syn1.lvalues= Cyc_NewControlFlow_use_it_list();} return syn1;} _LL639: return
Cyc_NewControlFlow_descend_path( env, e, _temp311); _LL635:;} _LL164: { void*
_temp654= Cyc_Tcutil_compress(( void*)( _temp315->topt)->v); struct Cyc_Absyn_PtrInfo
_temp660; struct Cyc_Absyn_Conref* _temp662; struct Cyc_Absyn_Tqual* _temp664;
struct Cyc_Absyn_Conref* _temp666; void* _temp668; void* _temp670; _LL656: if((
unsigned int) _temp654 > 4u?(( struct _tunion_struct*) _temp654)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL661: _temp660=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp654)->f1; _LL671: _temp670=( void*) _temp660.elt_typ; goto _LL669; _LL669:
_temp668=( void*) _temp660.rgn_typ; goto _LL667; _LL667: _temp666=( struct Cyc_Absyn_Conref*)
_temp660.nullable; goto _LL665; _LL665: _temp664=( struct Cyc_Absyn_Tqual*)
_temp660.tq; goto _LL663; _LL663: _temp662=( struct Cyc_Absyn_Conref*) _temp660.bounds;
goto _LL657;} else{ goto _LL658;} _LL658: goto _LL659; _LL657:{ void* _temp672=
Cyc_Tcutil_compress( _temp670); struct Cyc_Absyn_Structdecl** _temp680; struct
Cyc_List_List* _temp682; struct _tuple1* _temp684; struct Cyc_Absyn_Uniondecl**
_temp686; struct Cyc_List_List* _temp688; struct _tuple1* _temp690; _LL674: if((
unsigned int) _temp672 > 4u?(( struct _tunion_struct*) _temp672)->tag == Cyc_Absyn_StructType_tag:
0){ _LL685: _temp684=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp672)->f1; goto _LL683; _LL683: _temp682=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp672)->f2; goto _LL681; _LL681: _temp680=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp672)->f3;
goto _LL675;} else{ goto _LL676;} _LL676: if(( unsigned int) _temp672 > 4u?((
struct _tunion_struct*) _temp672)->tag == Cyc_Absyn_UnionType_tag: 0){ _LL691:
_temp690=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*) _temp672)->f1;
goto _LL689; _LL689: _temp688=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp672)->f2; goto _LL687; _LL687: _temp686=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp672)->f3; goto _LL677;} else{ goto
_LL678;} _LL678: goto _LL679; _LL675: return Cyc_NewControlFlow_descend_path(
env, e, _temp315); _LL677: { struct Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp(
env, _temp315); syn1.lvalues= Cyc_NewControlFlow_use_it_list(); return syn1;}
_LL679: goto _LL673; _LL673:;} goto _LL659; _LL659:( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Core_Impossible_struct* _temp692=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp692[ 0]=({ struct
Cyc_Core_Impossible_struct _temp693; _temp693.tag= Cyc_Core_Impossible_tag;
_temp693.f1=( struct _tagged_string)({ char* _temp694=( char*)"NewControlFlow: bad type in StructArrow";
struct _tagged_string _temp695; _temp695.curr= _temp694; _temp695.base= _temp694;
_temp695.last_plus_one= _temp694 + 40; _temp695;}); _temp693;}); _temp692;}));
_LL655:;} _LL166: { void* _temp696= Cyc_Tcutil_compress(( void*)( _temp319->topt)->v);
struct Cyc_Absyn_Structdecl** _temp704; struct Cyc_List_List* _temp706; struct
_tuple1* _temp708; struct Cyc_Absyn_Uniondecl** _temp710; struct Cyc_List_List*
_temp712; struct _tuple1* _temp714; _LL698: if(( unsigned int) _temp696 > 4u?((
struct _tunion_struct*) _temp696)->tag == Cyc_Absyn_StructType_tag: 0){ _LL709:
_temp708=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp696)->f1;
goto _LL707; _LL707: _temp706=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp696)->f2; goto _LL705; _LL705: _temp704=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp696)->f3; goto _LL699;} else{ goto
_LL700;} _LL700: if(( unsigned int) _temp696 > 4u?(( struct _tunion_struct*)
_temp696)->tag == Cyc_Absyn_UnionType_tag: 0){ _LL715: _temp714=( struct _tuple1*)((
struct Cyc_Absyn_UnionType_struct*) _temp696)->f1; goto _LL713; _LL713: _temp712=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*) _temp696)->f2; goto
_LL711; _LL711: _temp710=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp696)->f3; goto _LL701;} else{ goto _LL702;} _LL702: goto _LL703; _LL699:
return Cyc_NewControlFlow_descend_path( env, e, _temp319); _LL701: { struct Cyc_NewControlFlow_AbsSyn
syn1= Cyc_NewControlFlow_abstract_exp( env, _temp319); syn1.lvalues= Cyc_NewControlFlow_use_it_list();
return syn1;} _LL703:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp716=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp716[ 0]=({ struct Cyc_Core_Impossible_struct _temp717; _temp717.tag= Cyc_Core_Impossible_tag;
_temp717.f1=( struct _tagged_string)({ char* _temp718=( char*)"NewControlFlow: bad type in StructMember";
struct _tagged_string _temp719; _temp719.curr= _temp718; _temp719.base= _temp718;
_temp719.last_plus_one= _temp718 + 41; _temp719;}); _temp717;}); _temp716;}));
_LL697:;} _LL168: { struct Cyc_List_List* _temp720= env.access_path; struct Cyc_List_List
_temp728; struct Cyc_List_List* _temp729; struct Cyc_Absyn_Exp* _temp731; struct
Cyc_Absyn_Exp _temp733; struct Cyc_Position_Segment* _temp734; void* _temp736;
struct Cyc_Absyn_Exp* _temp738; struct Cyc_Absyn_Exp* _temp740; struct Cyc_Core_Opt*
_temp742; _LL722: if( _temp720 == 0){ goto _LL723;} else{ goto _LL724;} _LL724:
if( _temp720 == 0){ goto _LL726;} else{ _temp728=* _temp720; _LL732: _temp731=(
struct Cyc_Absyn_Exp*) _temp728.hd; _temp733=* _temp731; _LL743: _temp742=(
struct Cyc_Core_Opt*) _temp733.topt; goto _LL737; _LL737: _temp736=( void*)
_temp733.r; if((( struct _tunion_struct*) _temp736)->tag == Cyc_Absyn_Subscript_e_tag){
_LL741: _temp740=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp736)->f1; goto _LL739; _LL739: _temp738=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Subscript_e_struct*) _temp736)->f2; goto _LL735;} else{ goto _LL726;}
_LL735: _temp734=( struct Cyc_Position_Segment*) _temp733.loc; goto _LL730;
_LL730: _temp729=( struct Cyc_List_List*) _temp728.tl; goto _LL725;} _LL726:
goto _LL727; _LL723: { void* ansexp= Cyc_CfAbsexp_mkSkipAE(); void* assignexp=
Cyc_CfAbsexp_mkSkipAE();{ int j= 0; for( 0; _temp321 != 0; _temp321= _temp321->tl,
++ j){ struct Cyc_List_List* ith_lvals= 0;{ struct Cyc_List_List* ls= env.lhsides;
for( 0; ls != 0; ls= ls->tl){ if( Cyc_CfAbsexp_isUnknownOp(( void*) ls->hd)){
ith_lvals=({ struct Cyc_List_List* _temp744=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp744->hd=( void*) Cyc_CfAbsexp_mkUnknownOp();
_temp744->tl= ith_lvals; _temp744;});} else{ ith_lvals=({ struct Cyc_List_List*
_temp745=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp745->hd=( void*) Cyc_CfAbsexp_mkMemberOp(( void*) ls->hd,( void*)({ struct
Cyc_CfFlowInfo_TupleF_struct* _temp746=( struct Cyc_CfFlowInfo_TupleF_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_TupleF_struct)); _temp746[ 0]=({ struct
Cyc_CfFlowInfo_TupleF_struct _temp747; _temp747.tag= Cyc_CfFlowInfo_TupleF_tag;
_temp747.f1= j; _temp747;}); _temp746;})); _temp745->tl= ith_lvals; _temp745;});}}}{
struct Cyc_NewControlFlow_AbsSyn ith_syn= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_change_lhs(
env, ith_lvals),( struct Cyc_Absyn_Exp*) _temp321->hd); void* _temp750; void*
_temp752; struct Cyc_NewControlFlow_AbsSynOne _temp748= Cyc_NewControlFlow_meet_absexp(
ith_syn); _LL753: _temp752=( void*) _temp748.inner_exp; goto _LL751; _LL751:
_temp750=( void*) _temp748.assigns; goto _LL749; _LL749: ansexp= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, ansexp, _temp752); assignexp= Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_mkAnyOrderG(), assignexp, _temp750);}}} return Cyc_NewControlFlow_mkSyn_tf(
ansexp, 0, assignexp);} _LL725: { int i=( int) Cyc_Evexp_eval_const_uint_exp(
_temp738); struct Cyc_NewControlFlow_AbsEnv other_env= Cyc_NewControlFlow_change_lhs(
env, 0); other_env.access_path= 0; env.access_path= _temp729;{ void* ansexp= Cyc_CfAbsexp_mkSkipAE();
void* assignexp= Cyc_CfAbsexp_mkSkipAE();{ int j= 0; for( 0; _temp321 != 0;
_temp321= _temp321->tl, ++ j){ if( i == j){ void* _temp756; void* _temp758;
struct Cyc_NewControlFlow_AbsSynOne _temp754= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( env,( struct Cyc_Absyn_Exp*) _temp321->hd));
_LL759: _temp758=( void*) _temp754.inner_exp; goto _LL757; _LL757: _temp756=(
void*) _temp754.assigns; goto _LL755; _LL755: assignexp= _temp756; ansexp= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, ansexp, _temp758);} else{ void* _temp762; void*
_temp764; struct Cyc_NewControlFlow_AbsSynOne _temp760= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( other_env,( struct Cyc_Absyn_Exp*) _temp321->hd));
_LL765: _temp764=( void*) _temp760.inner_exp; goto _LL763; _LL763: _temp762=(
void*) _temp760.assigns; goto _LL761; _LL761: ansexp= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, ansexp, _temp764);}}} return Cyc_NewControlFlow_mkSyn_tf(
ansexp,({ struct Cyc_List_List* _temp766=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp766->hd=( void*) Cyc_CfAbsexp_mkUnknownOp();
_temp766->tl= 0; _temp766;}), assignexp);}} _LL727:( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Core_Impossible_struct* _temp767=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp767[ 0]=({ struct
Cyc_Core_Impossible_struct _temp768; _temp768.tag= Cyc_Core_Impossible_tag;
_temp768.f1=( struct _tagged_string)({ char* _temp769=( char*)"abstract_exp: unexpected access path to Tuple";
struct _tagged_string _temp770; _temp770.curr= _temp769; _temp770.base= _temp769;
_temp770.last_plus_one= _temp769 + 46; _temp770;}); _temp768;}); _temp767;}));
_LL721:;} _LL170: { struct Cyc_List_List* _temp771= env.access_path; struct Cyc_List_List
_temp779; struct Cyc_List_List* _temp780; struct Cyc_Absyn_Exp* _temp782; struct
Cyc_Absyn_Exp _temp784; struct Cyc_Position_Segment* _temp785; void* _temp787;
struct _tagged_string* _temp789; struct Cyc_Absyn_Exp* _temp791; struct Cyc_Core_Opt*
_temp793; _LL773: if( _temp771 == 0){ goto _LL774;} else{ goto _LL775;} _LL775:
if( _temp771 == 0){ goto _LL777;} else{ _temp779=* _temp771; _LL783: _temp782=(
struct Cyc_Absyn_Exp*) _temp779.hd; _temp784=* _temp782; _LL794: _temp793=(
struct Cyc_Core_Opt*) _temp784.topt; goto _LL788; _LL788: _temp787=( void*)
_temp784.r; if((( struct _tunion_struct*) _temp787)->tag == Cyc_Absyn_StructMember_e_tag){
_LL792: _temp791=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp787)->f1; goto _LL790; _LL790: _temp789=( struct _tagged_string*)(( struct
Cyc_Absyn_StructMember_e_struct*) _temp787)->f2; goto _LL786;} else{ goto _LL777;}
_LL786: _temp785=( struct Cyc_Position_Segment*) _temp784.loc; goto _LL781;
_LL781: _temp780=( struct Cyc_List_List*) _temp779.tl; goto _LL776;} _LL777:
goto _LL778; _LL774: { void* ansexp= Cyc_CfAbsexp_mkSkipAE(); void* assignexp=
Cyc_CfAbsexp_mkSkipAE(); for( 0; _temp325 != 0; _temp325= _temp325->tl){ struct
Cyc_List_List* ith_lvals= 0;{ struct Cyc_List_List* ls= env.lhsides; for( 0; ls
!= 0; ls= ls->tl){ if( Cyc_CfAbsexp_isUnknownOp(( void*) ls->hd)){ ith_lvals=({
struct Cyc_List_List* _temp795=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp795->hd=( void*) Cyc_CfAbsexp_mkUnknownOp();
_temp795->tl= ith_lvals; _temp795;});} else{ struct Cyc_List_List* ds=(*((
struct _tuple5*) _temp325->hd)).f1; for( 0; ds != 0; ds= ds->tl){ void* _temp796=(
void*) ds->hd; struct Cyc_Absyn_Exp* _temp802; struct _tagged_string* _temp804;
_LL798: if((( struct _tunion_struct*) _temp796)->tag == Cyc_Absyn_ArrayElement_tag){
_LL803: _temp802=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayElement_struct*)
_temp796)->f1; goto _LL799;} else{ goto _LL800;} _LL800: if((( struct
_tunion_struct*) _temp796)->tag == Cyc_Absyn_FieldName_tag){ _LL805: _temp804=(
struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp796)->f1;
goto _LL801;} else{ goto _LL797;} _LL799:( void) _throw(( struct _xtunion_struct*)({
struct Cyc_Core_Impossible_struct* _temp806=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp806[ 0]=({ struct
Cyc_Core_Impossible_struct _temp807; _temp807.tag= Cyc_Core_Impossible_tag;
_temp807.f1=( struct _tagged_string)({ char* _temp808=( char*)"bad struct designator";
struct _tagged_string _temp809; _temp809.curr= _temp808; _temp809.base= _temp808;
_temp809.last_plus_one= _temp808 + 22; _temp809;}); _temp807;}); _temp806;}));
_LL801: ith_lvals=({ struct Cyc_List_List* _temp810=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp810->hd=( void*) Cyc_CfAbsexp_mkMemberOp((
void*) ls->hd,( void*)({ struct Cyc_CfFlowInfo_StructF_struct* _temp811=( struct
Cyc_CfFlowInfo_StructF_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructF_struct));
_temp811[ 0]=({ struct Cyc_CfFlowInfo_StructF_struct _temp812; _temp812.tag= Cyc_CfFlowInfo_StructF_tag;
_temp812.f1= _temp804; _temp812;}); _temp811;})); _temp810->tl= ith_lvals;
_temp810;}); goto _LL797; _LL797:;}}}}{ void** ith_topt=( void*)&(((*(( struct
_tuple5*) _temp325->hd)).f2)->topt)->v; struct Cyc_NewControlFlow_AbsSyn ith_syn=
Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_change_lhs( env, ith_lvals),(*((
struct _tuple5*) _temp325->hd)).f2); void* _temp815; void* _temp817; struct Cyc_NewControlFlow_AbsSynOne
_temp813= Cyc_NewControlFlow_meet_absexp( ith_syn); _LL818: _temp817=( void*)
_temp813.inner_exp; goto _LL816; _LL816: _temp815=( void*) _temp813.assigns;
goto _LL814; _LL814: ansexp= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG,
ansexp, _temp817); assignexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_mkAnyOrderG(),
assignexp, _temp815);}} return Cyc_NewControlFlow_mkSyn_tf( ansexp, 0, assignexp);}
_LL776: { struct Cyc_NewControlFlow_AbsEnv other_env= Cyc_NewControlFlow_change_lhs(
env, 0); other_env.access_path= 0; env.access_path= _temp780;{ void* ansexp= Cyc_CfAbsexp_mkSkipAE();
void* assignexp= Cyc_CfAbsexp_mkSkipAE(); for( 0; _temp325 != 0; _temp325=
_temp325->tl){ int used= 0;{ struct Cyc_List_List* ds=(*(( struct _tuple5*)
_temp325->hd)).f1; for( 0; ds != 0; ds= ds->tl){ void* _temp819=( void*) ds->hd;
struct Cyc_Absyn_Exp* _temp825; struct _tagged_string* _temp827; _LL821: if(((
struct _tunion_struct*) _temp819)->tag == Cyc_Absyn_ArrayElement_tag){ _LL826:
_temp825=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayElement_struct*)
_temp819)->f1; goto _LL822;} else{ goto _LL823;} _LL823: if((( struct
_tunion_struct*) _temp819)->tag == Cyc_Absyn_FieldName_tag){ _LL828: _temp827=(
struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp819)->f1;
goto _LL824;} else{ goto _LL820;} _LL822:( void) _throw(( struct _xtunion_struct*)({
struct Cyc_Core_Impossible_struct* _temp829=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp829[ 0]=({ struct
Cyc_Core_Impossible_struct _temp830; _temp830.tag= Cyc_Core_Impossible_tag;
_temp830.f1=( struct _tagged_string)({ char* _temp831=( char*)"bad struct designator";
struct _tagged_string _temp832; _temp832.curr= _temp831; _temp832.base= _temp831;
_temp832.last_plus_one= _temp831 + 22; _temp832;}); _temp830;}); _temp829;}));
_LL824: if( Cyc_String_zstrptrcmp( _temp789, _temp827) == 0){ used= 1;} goto
_LL820; _LL820:;}} if( used){ void* _temp835; void* _temp837; struct Cyc_NewControlFlow_AbsSynOne
_temp833= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp( env,(*((
struct _tuple5*) _temp325->hd)).f2)); _LL838: _temp837=( void*) _temp833.inner_exp;
goto _LL836; _LL836: _temp835=( void*) _temp833.assigns; goto _LL834; _LL834:
assignexp= _temp835; ansexp= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG,
ansexp, _temp837);} else{ void* _temp841; void* _temp843; struct Cyc_NewControlFlow_AbsSynOne
_temp839= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
other_env,(*(( struct _tuple5*) _temp325->hd)).f2)); _LL844: _temp843=( void*)
_temp839.inner_exp; goto _LL842; _LL842: _temp841=( void*) _temp839.assigns;
goto _LL840; _LL840: ansexp= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG,
ansexp, _temp843);}} return Cyc_NewControlFlow_mkSyn_tf( ansexp, 0, assignexp);}}
_LL778:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp845=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp845[ 0]=({ struct Cyc_Core_Impossible_struct _temp846; _temp846.tag= Cyc_Core_Impossible_tag;
_temp846.f1=( struct _tagged_string)({ char* _temp847=( char*)"abstract_exp: unexpected access path to Struct";
struct _tagged_string _temp848; _temp848.curr= _temp847; _temp848.base= _temp847;
_temp848.last_plus_one= _temp847 + 47; _temp848;}); _temp846;}); _temp845;}));
_LL772:;} _LL172: return Cyc_NewControlFlow_use_exp_unordered( env,(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)((
struct Cyc_Absyn_Exp*(*)( struct _tuple5*)) Cyc_Core_snd, _temp331), 0); _LL174:
return Cyc_NewControlFlow_use_exp_unordered( env,({ struct Cyc_List_List*
_temp849=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp849->hd=( void*) _temp335; _temp849->tl= 0; _temp849;}), 0); _LL176: {
struct Cyc_NewControlFlow_AbsSyn esyn= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env(
env), _temp337); return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*)( Cyc_NewControlFlow_meet_absexp( esyn)).inner_exp,
Cyc_CfAbsexp_mkBottomAE()), 0, Cyc_CfAbsexp_mkSkipAE());} _LL178: { struct Cyc_List_List*
_temp852; struct Cyc_NewControlFlow_AbsSynOne _temp854; struct Cyc_NewControlFlow_AbsSynOne
_temp856; struct Cyc_NewControlFlow_AbsSyn _temp850= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp343); _LL857: _temp856=( struct Cyc_NewControlFlow_AbsSynOne)
_temp850.when_true; goto _LL855; _LL855: _temp854=( struct Cyc_NewControlFlow_AbsSynOne)
_temp850.when_false; goto _LL853; _LL853: _temp852=( struct Cyc_List_List*)
_temp850.lvalues; goto _LL851; _LL851: { struct Cyc_List_List* _temp860; struct
Cyc_NewControlFlow_AbsSynOne _temp862; struct Cyc_NewControlFlow_AbsSynOne
_temp864; struct Cyc_NewControlFlow_AbsSyn _temp858= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp341); _LL865: _temp864=( struct Cyc_NewControlFlow_AbsSynOne)
_temp858.when_true; goto _LL863; _LL863: _temp862=( struct Cyc_NewControlFlow_AbsSynOne)
_temp858.when_false; goto _LL861; _LL861: _temp860=( struct Cyc_List_List*)
_temp858.lvalues; goto _LL859; _LL859: { struct Cyc_List_List* _temp868; struct
Cyc_NewControlFlow_AbsSynOne _temp870; struct Cyc_NewControlFlow_AbsSynOne
_temp872; struct Cyc_NewControlFlow_AbsSyn _temp866= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp339); _LL873: _temp872=( struct Cyc_NewControlFlow_AbsSynOne)
_temp866.when_true; goto _LL871; _LL871: _temp870=( struct Cyc_NewControlFlow_AbsSynOne)
_temp866.when_false; goto _LL869; _LL869: _temp868=( struct Cyc_List_List*)
_temp866.lvalues; goto _LL867; _LL867: { void* true_exp= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OneofG, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*) _temp856.inner_exp,(
void*) _temp856.assigns), Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*) _temp864.inner_exp,( void*) _temp864.assigns)), Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*) _temp854.inner_exp,( void*) _temp854.assigns), Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) _temp872.inner_exp,( void*) _temp872.assigns)));
void* false_exp= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OneofG, Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*) _temp856.inner_exp,( void*) _temp856.assigns), Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) _temp862.inner_exp,( void*) _temp862.assigns)),
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) _temp854.inner_exp,( void*) _temp854.assigns),
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*) _temp870.inner_exp,(
void*) _temp870.assigns))); return({ struct Cyc_NewControlFlow_AbsSyn _temp874;
_temp874.when_true=({ struct Cyc_NewControlFlow_AbsSynOne _temp876; _temp876.inner_exp=(
void*) true_exp; _temp876.assigns=( void*) Cyc_NewControlFlow_make_assigns( env,
Cyc_CfAbsexp_mkUnknownOp()); _temp876;}); _temp874.when_false=({ struct Cyc_NewControlFlow_AbsSynOne
_temp875; _temp875.inner_exp=( void*) false_exp; _temp875.assigns=( void*) Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()); _temp875;}); _temp874.lvalues= 0; _temp874;});}}}}
_LL180: { struct Cyc_NewControlFlow_AbsEnv env2= Cyc_NewControlFlow_change_lhs(
env, 0); env2.access_path= 0; Cyc_NewControlFlow_cf_prepass( env2, _temp345);{
struct Cyc_Absyn_Stmt* last_s= _temp345; while( 1) { void* _temp877=( void*)
last_s->r; struct Cyc_Absyn_Stmt* _temp887; struct Cyc_Absyn_Stmt* _temp889;
struct Cyc_Absyn_Stmt* _temp891; struct Cyc_Absyn_Decl* _temp893; struct Cyc_Absyn_Exp*
_temp895; _LL879: if(( unsigned int) _temp877 > 1u?(( struct _tunion_struct*)
_temp877)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL890: _temp889=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp877)->f1; goto _LL888; _LL888: _temp887=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp877)->f2; goto
_LL880;} else{ goto _LL881;} _LL881: if(( unsigned int) _temp877 > 1u?(( struct
_tunion_struct*) _temp877)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL894: _temp893=(
struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*) _temp877)->f1; goto
_LL892; _LL892: _temp891=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp877)->f2; goto _LL882;} else{ goto _LL883;} _LL883: if(( unsigned int)
_temp877 > 1u?(( struct _tunion_struct*) _temp877)->tag == Cyc_Absyn_Exp_s_tag:
0){ _LL896: _temp895=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp877)->f1; goto _LL884;} else{ goto _LL885;} _LL885: goto _LL886; _LL880:
last_s= _temp887; continue; _LL882: last_s= _temp891; continue; _LL884: { struct
Cyc_NewControlFlow_AbsSyn syn= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env(
env), _temp895); void* _temp899; void* _temp901; struct Cyc_NewControlFlow_AbsSynOne
_temp897= Cyc_NewControlFlow_meet_absexp( syn); _LL902: _temp901=( void*)
_temp897.inner_exp; goto _LL900; _LL900: _temp899=( void*) _temp897.assigns;
goto _LL898; _LL898:( void*)((( Cyc_NewControlFlow_get_stmt_annot( last_s))->absexps)->hd=(
void*) Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp901, _temp899));
return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkStmtAE( _temp345), 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));} _LL886:( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Core_Impossible_struct* _temp903=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp903[ 0]=({ struct
Cyc_Core_Impossible_struct _temp904; _temp904.tag= Cyc_Core_Impossible_tag;
_temp904.f1=( struct _tagged_string)({ char* _temp905=( char*)"abstract_exp: ill-formed StmtExp";
struct _tagged_string _temp906; _temp906.curr= _temp905; _temp906.base= _temp905;
_temp906.last_plus_one= _temp905 + 33; _temp906;}); _temp904;}); _temp903;}));
_LL878:;}}} _LL182: { struct Cyc_NewControlFlow_AbsEnv env1= Cyc_NewControlFlow_change_lhs(
env, 0); env1.access_path= 0;{ struct Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp(
env1, _temp349); struct Cyc_NewControlFlow_AbsSyn syn2= Cyc_NewControlFlow_abstract_exp(
env, _temp347); struct Cyc_NewControlFlow_AbsSynOne syn1meet= Cyc_NewControlFlow_meet_absexp(
syn1); return({ struct Cyc_NewControlFlow_AbsSyn _temp907; _temp907.when_true=({
struct Cyc_NewControlFlow_AbsSynOne _temp909; _temp909.inner_exp=( void*) Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) syn1meet.inner_exp,( void*)( syn2.when_true).inner_exp);
_temp909.assigns=( void*)(( void*)( syn2.when_true).assigns); _temp909;});
_temp907.when_false=({ struct Cyc_NewControlFlow_AbsSynOne _temp908; _temp908.inner_exp=(
void*) Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*) syn1meet.inner_exp,(
void*)( syn2.when_false).inner_exp); _temp908.assigns=( void*)(( void*)( syn2.when_false).assigns);
_temp908;}); _temp907.lvalues= 0; _temp907;});}} _LL184: env.access_path == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp910=( char*)"new_control_flow.cyc:746 env.access_path==null";
struct _tagged_string _temp911; _temp911.curr= _temp910; _temp911.base= _temp910;
_temp911.last_plus_one= _temp910 + 47; _temp911;})); Cyc_NewControlFlow_add_var_root(
env, _temp355);( env.shared)->comprehension_vars=({ struct Cyc_List_List*
_temp912=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp912->hd=( void*) _temp355; _temp912->tl=( env.shared)->comprehension_vars;
_temp912;});{ struct Cyc_NewControlFlow_AbsEnv elt_env= Cyc_NewControlFlow_use_it_env(
env); void* _temp915; void* _temp917; struct Cyc_NewControlFlow_AbsSynOne
_temp913= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
elt_env, _temp353)); _LL918: _temp917=( void*) _temp913.inner_exp; goto _LL916;
_LL916: _temp915=( void*) _temp913.assigns; goto _LL914; _LL914: { void*
_temp921; void* _temp923; struct Cyc_NewControlFlow_AbsSynOne _temp919= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( elt_env, _temp351)); _LL924: _temp923=( void*)
_temp919.inner_exp; goto _LL922; _LL922: _temp921=( void*) _temp919.assigns;
goto _LL920; _LL920: { void* ansexp= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
_temp917, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp915, Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkAssignAE( Cyc_CfAbsexp_mkLocalOp(
_temp355), Cyc_CfAbsexp_mkUnknownOp()), Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OneofG,
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp923, _temp921), Cyc_CfAbsexp_mkSkipAE()))));
return Cyc_NewControlFlow_mkSyn_tf( ansexp, 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));}}} _LL186: goto _LL188; _LL188: goto _LL190;
_LL190: goto _LL192; _LL192: goto _LL194; _LL194: goto _LL196; _LL196: goto
_LL198; _LL198:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp925=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp925[ 0]=({ struct Cyc_Core_Impossible_struct _temp926; _temp926.tag= Cyc_Core_Impossible_tag;
_temp926.f1=( struct _tagged_string)({ char* _temp927=( char*)"abstract_exp, unexpected exp form";
struct _tagged_string _temp928; _temp928.curr= _temp927; _temp928.base= _temp927;
_temp928.last_plus_one= _temp927 + 34; _temp928;}); _temp926;}); _temp925;}));
_LL112:;} void* Cyc_NewControlFlow_abstract_exp_top( struct Cyc_NewControlFlow_AbsEnv
env, struct Cyc_Absyn_Exp* e){ void* _temp931; void* _temp933; struct Cyc_NewControlFlow_AbsSynOne
_temp929= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp( env,
e)); _LL934: _temp933=( void*) _temp929.inner_exp; goto _LL932; _LL932: _temp931=(
void*) _temp929.assigns; goto _LL930; _LL930: return Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, _temp933, _temp931);} struct _tuple0 Cyc_NewControlFlow_abstract_guard(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e){ struct Cyc_NewControlFlow_AbsSyn
syn= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env( env), e);
return({ struct _tuple0 _temp935; _temp935.f1= Cyc_CfAbsexp_mkGroupAE(( void*)
Cyc_CfAbsexp_OrderedG,( void*)( syn.when_true).inner_exp,( void*)( syn.when_true).assigns);
_temp935.f2= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*)( syn.when_false).inner_exp,(
void*)( syn.when_false).assigns); _temp935;});} struct Cyc_List_List* Cyc_NewControlFlow_cf_prepass_scs(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_List_List* scs){ struct Cyc_List_List*
ans= 0; for( 0; scs != 0; scs= scs->tl){ struct Cyc_Absyn_Switch_clause _temp938;
struct Cyc_Position_Segment* _temp939; struct Cyc_Absyn_Stmt* _temp941; struct
Cyc_Absyn_Exp* _temp943; struct Cyc_Core_Opt* _temp945; struct Cyc_Absyn_Pat*
_temp947; struct Cyc_Absyn_Switch_clause* _temp936=( struct Cyc_Absyn_Switch_clause*)
scs->hd; _temp938=* _temp936; _LL948: _temp947=( struct Cyc_Absyn_Pat*) _temp938.pattern;
goto _LL946; _LL946: _temp945=( struct Cyc_Core_Opt*) _temp938.pat_vars; goto
_LL944; _LL944: _temp943=( struct Cyc_Absyn_Exp*) _temp938.where_clause; goto
_LL942; _LL942: _temp941=( struct Cyc_Absyn_Stmt*) _temp938.body; goto _LL940;
_LL940: _temp939=( struct Cyc_Position_Segment*) _temp938.loc; goto _LL937;
_LL937: if( _temp945 == 0){( void) _throw(( struct _xtunion_struct*)({ struct
Cyc_Core_Impossible_struct* _temp949=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp949[ 0]=({ struct
Cyc_Core_Impossible_struct _temp950; _temp950.tag= Cyc_Core_Impossible_tag;
_temp950.f1=( struct _tagged_string)({ char* _temp951=( char*)"switch clause vds not set";
struct _tagged_string _temp952; _temp952.curr= _temp951; _temp952.base= _temp951;
_temp952.last_plus_one= _temp951 + 26; _temp952;}); _temp950;}); _temp949;}));}{
struct Cyc_List_List* vds=( struct Cyc_List_List*) _temp945->v; for( 0; vds != 0;
vds= vds->tl){ Cyc_NewControlFlow_add_var_root( env,( struct Cyc_Absyn_Vardecl*)
vds->hd);}} if( _temp943 != 0){ void* _temp956; void* _temp958; struct _tuple0
_temp954= Cyc_NewControlFlow_abstract_guard( env,( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp953= _temp943; if( _temp953 == 0){ _throw(
Null_Exception);} _temp953;})); _LL959: _temp958= _temp954.f1; goto _LL957;
_LL957: _temp956= _temp954.f2; goto _LL955; _LL955: ans=({ struct Cyc_List_List*
_temp960=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp960->hd=( void*) _temp958; _temp960->tl=({ struct Cyc_List_List* _temp961=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp961->hd=(
void*) _temp956; _temp961->tl= ans; _temp961;}); _temp960;});} Cyc_NewControlFlow_cf_prepass(
env, _temp941);} return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( ans);} void Cyc_NewControlFlow_cf_prepass( struct Cyc_NewControlFlow_AbsEnv
env, struct Cyc_Absyn_Stmt* s){ env.lhsides == 0? 0:(( int(*)( struct
_tagged_string msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({ char*
_temp962=( char*)"new_control_flow.cyc:798 env.lhsides == null"; struct
_tagged_string _temp963; _temp963.curr= _temp962; _temp963.base= _temp962;
_temp963.last_plus_one= _temp962 + 45; _temp963;})); env.access_path == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp964=( char*)"new_control_flow.cyc:799 env.access_path == null";
struct _tagged_string _temp965; _temp965.curr= _temp964; _temp965.base= _temp964;
_temp965.last_plus_one= _temp964 + 49; _temp965;}));{ void* _temp966=( void*) s->r;
struct Cyc_Absyn_Exp* _temp1012; struct Cyc_Absyn_Stmt* _temp1014; struct Cyc_Absyn_Stmt*
_temp1016; struct Cyc_Absyn_Exp* _temp1018; struct Cyc_Absyn_Exp* _temp1020;
struct Cyc_Absyn_Stmt* _temp1022; struct Cyc_Absyn_Stmt* _temp1024; struct Cyc_Absyn_Exp*
_temp1026; struct _tuple3 _temp1028; struct Cyc_Absyn_Stmt* _temp1030; struct
Cyc_Absyn_Exp* _temp1032; struct Cyc_Absyn_Stmt* _temp1034; struct Cyc_Absyn_Stmt*
_temp1036; struct _tuple3 _temp1038; struct Cyc_Absyn_Stmt* _temp1040; struct
Cyc_Absyn_Exp* _temp1042; struct Cyc_Absyn_Stmt* _temp1044; struct Cyc_Absyn_Stmt*
_temp1046; struct Cyc_Absyn_Stmt* _temp1048; struct _tagged_string* _temp1050;
struct Cyc_Absyn_Stmt* _temp1052; struct _tagged_string* _temp1054; struct Cyc_Absyn_Stmt*
_temp1056; struct _tuple3 _temp1058; struct Cyc_Absyn_Stmt* _temp1060; struct
Cyc_Absyn_Exp* _temp1062; struct _tuple3 _temp1064; struct Cyc_Absyn_Stmt*
_temp1066; struct Cyc_Absyn_Exp* _temp1068; struct Cyc_Absyn_Exp* _temp1070;
struct Cyc_List_List* _temp1072; struct Cyc_Absyn_Exp* _temp1074; struct Cyc_Absyn_Switch_clause**
_temp1076; struct Cyc_List_List* _temp1078; struct Cyc_Absyn_Stmt* _temp1080;
struct Cyc_Absyn_Decl* _temp1082; struct Cyc_Absyn_Decl _temp1084; struct Cyc_Position_Segment*
_temp1085; void* _temp1087; struct Cyc_Absyn_Vardecl* _temp1089; struct Cyc_Absyn_Stmt*
_temp1091; struct Cyc_Absyn_Decl* _temp1093; struct Cyc_Absyn_Decl _temp1095;
struct Cyc_Position_Segment* _temp1096; void* _temp1098; int _temp1100; struct
Cyc_Absyn_Exp* _temp1102; struct Cyc_Core_Opt* _temp1104; struct Cyc_Core_Opt*
_temp1106; struct Cyc_Core_Opt _temp1108; struct Cyc_List_List* _temp1109;
struct Cyc_Absyn_Pat* _temp1111; struct Cyc_Absyn_Stmt* _temp1113; struct Cyc_Absyn_Decl*
_temp1115; struct Cyc_Absyn_Stmt* _temp1117; struct Cyc_Absyn_Stmt* _temp1119;
struct Cyc_Absyn_Stmt* _temp1121; struct _tagged_string* _temp1123; struct Cyc_List_List*
_temp1125; struct Cyc_Absyn_Stmt* _temp1127; _LL968: if(( int) _temp966 == Cyc_Absyn_Skip_s){
goto _LL969;} else{ goto _LL970;} _LL970: if(( unsigned int) _temp966 > 1u?((
struct _tunion_struct*) _temp966)->tag == Cyc_Absyn_Exp_s_tag: 0){ _LL1013:
_temp1012=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*) _temp966)->f1;
goto _LL971;} else{ goto _LL972;} _LL972: if(( unsigned int) _temp966 > 1u?((
struct _tunion_struct*) _temp966)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL1017:
_temp1016=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp966)->f1;
goto _LL1015; _LL1015: _temp1014=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp966)->f2; goto _LL973;} else{ goto _LL974;} _LL974: if(( unsigned int)
_temp966 > 1u?(( struct _tunion_struct*) _temp966)->tag == Cyc_Absyn_Return_s_tag:
0){ _LL1019: _temp1018=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp966)->f1; if( _temp1018 == 0){ goto _LL975;} else{ goto _LL976;}} else{
goto _LL976;} _LL976: if(( unsigned int) _temp966 > 1u?(( struct _tunion_struct*)
_temp966)->tag == Cyc_Absyn_Return_s_tag: 0){ _LL1021: _temp1020=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Return_s_struct*) _temp966)->f1; goto _LL977;} else{ goto
_LL978;} _LL978: if(( unsigned int) _temp966 > 1u?(( struct _tunion_struct*)
_temp966)->tag == Cyc_Absyn_IfThenElse_s_tag: 0){ _LL1027: _temp1026=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp966)->f1; goto
_LL1025; _LL1025: _temp1024=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp966)->f2; goto _LL1023; _LL1023: _temp1022=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp966)->f3; goto _LL979;} else{ goto
_LL980;} _LL980: if(( unsigned int) _temp966 > 1u?(( struct _tunion_struct*)
_temp966)->tag == Cyc_Absyn_Do_s_tag: 0){ _LL1035: _temp1034=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Do_s_struct*) _temp966)->f1; goto _LL1029; _LL1029: _temp1028=(
struct _tuple3)(( struct Cyc_Absyn_Do_s_struct*) _temp966)->f2; _LL1033:
_temp1032= _temp1028.f1; goto _LL1031; _LL1031: _temp1030= _temp1028.f2; goto
_LL981;} else{ goto _LL982;} _LL982: if(( unsigned int) _temp966 > 1u?(( struct
_tunion_struct*) _temp966)->tag == Cyc_Absyn_While_s_tag: 0){ _LL1039: _temp1038=(
struct _tuple3)(( struct Cyc_Absyn_While_s_struct*) _temp966)->f1; _LL1043:
_temp1042= _temp1038.f1; goto _LL1041; _LL1041: _temp1040= _temp1038.f2; goto
_LL1037; _LL1037: _temp1036=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_While_s_struct*)
_temp966)->f2; goto _LL983;} else{ goto _LL984;} _LL984: if(( unsigned int)
_temp966 > 1u?(( struct _tunion_struct*) _temp966)->tag == Cyc_Absyn_Break_s_tag:
0){ _LL1045: _temp1044=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Break_s_struct*)
_temp966)->f1; goto _LL985;} else{ goto _LL986;} _LL986: if(( unsigned int)
_temp966 > 1u?(( struct _tunion_struct*) _temp966)->tag == Cyc_Absyn_Continue_s_tag:
0){ _LL1047: _temp1046=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Continue_s_struct*)
_temp966)->f1; goto _LL987;} else{ goto _LL988;} _LL988: if(( unsigned int)
_temp966 > 1u?(( struct _tunion_struct*) _temp966)->tag == Cyc_Absyn_Goto_s_tag:
0){ _LL1051: _temp1050=( struct _tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp966)->f1; goto _LL1049; _LL1049: _temp1048=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Goto_s_struct*) _temp966)->f2; if( _temp1048 == 0){ goto _LL989;}
else{ goto _LL990;}} else{ goto _LL990;} _LL990: if(( unsigned int) _temp966 > 1u?((
struct _tunion_struct*) _temp966)->tag == Cyc_Absyn_Goto_s_tag: 0){ _LL1055:
_temp1054=( struct _tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp966)->f1;
goto _LL1053; _LL1053: _temp1052=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp966)->f2; goto _LL991;} else{ goto _LL992;} _LL992: if(( unsigned int)
_temp966 > 1u?(( struct _tunion_struct*) _temp966)->tag == Cyc_Absyn_For_s_tag:
0){ _LL1071: _temp1070=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp966)->f1; goto _LL1065; _LL1065: _temp1064=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp966)->f2; _LL1069: _temp1068= _temp1064.f1; goto _LL1067; _LL1067:
_temp1066= _temp1064.f2; goto _LL1059; _LL1059: _temp1058=( struct _tuple3)((
struct Cyc_Absyn_For_s_struct*) _temp966)->f3; _LL1063: _temp1062= _temp1058.f1;
goto _LL1061; _LL1061: _temp1060= _temp1058.f2; goto _LL1057; _LL1057: _temp1056=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp966)->f4; goto
_LL993;} else{ goto _LL994;} _LL994: if(( unsigned int) _temp966 > 1u?(( struct
_tunion_struct*) _temp966)->tag == Cyc_Absyn_Switch_s_tag: 0){ _LL1075:
_temp1074=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Switch_s_struct*) _temp966)->f1;
goto _LL1073; _LL1073: _temp1072=( struct Cyc_List_List*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp966)->f2; goto _LL995;} else{ goto _LL996;} _LL996: if(( unsigned int)
_temp966 > 1u?(( struct _tunion_struct*) _temp966)->tag == Cyc_Absyn_Fallthru_s_tag:
0){ _LL1079: _temp1078=( struct Cyc_List_List*)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp966)->f1; goto _LL1077; _LL1077: _temp1076=( struct Cyc_Absyn_Switch_clause**)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp966)->f2; goto _LL997;} else{ goto
_LL998;} _LL998: if(( unsigned int) _temp966 > 1u?(( struct _tunion_struct*)
_temp966)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL1083: _temp1082=( struct Cyc_Absyn_Decl*)((
struct Cyc_Absyn_Decl_s_struct*) _temp966)->f1; _temp1084=* _temp1082; _LL1088:
_temp1087=( void*) _temp1084.r; if((( struct _tunion_struct*) _temp1087)->tag ==
Cyc_Absyn_Var_d_tag){ _LL1090: _temp1089=( struct Cyc_Absyn_Vardecl*)(( struct
Cyc_Absyn_Var_d_struct*) _temp1087)->f1; goto _LL1086;} else{ goto _LL1000;}
_LL1086: _temp1085=( struct Cyc_Position_Segment*) _temp1084.loc; goto _LL1081;
_LL1081: _temp1080=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp966)->f2; goto _LL999;} else{ goto _LL1000;} _LL1000: if(( unsigned int)
_temp966 > 1u?(( struct _tunion_struct*) _temp966)->tag == Cyc_Absyn_Decl_s_tag:
0){ _LL1094: _temp1093=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp966)->f1; _temp1095=* _temp1093; _LL1099: _temp1098=( void*) _temp1095.r;
if((( struct _tunion_struct*) _temp1098)->tag == Cyc_Absyn_Let_d_tag){ _LL1112:
_temp1111=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*) _temp1098)->f1;
goto _LL1107; _LL1107: _temp1106=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1098)->f2; if( _temp1106 == 0){ goto _LL1002;} else{ _temp1108=* _temp1106;
_LL1110: _temp1109=( struct Cyc_List_List*) _temp1108.v; goto _LL1105;} _LL1105:
_temp1104=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp1098)->f3;
goto _LL1103; _LL1103: _temp1102=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1098)->f4; goto _LL1101; _LL1101: _temp1100=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp1098)->f5; goto _LL1097;} else{ goto _LL1002;} _LL1097: _temp1096=( struct
Cyc_Position_Segment*) _temp1095.loc; goto _LL1092; _LL1092: _temp1091=( struct
Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp966)->f2; goto _LL1001;}
else{ goto _LL1002;} _LL1002: if(( unsigned int) _temp966 > 1u?(( struct
_tunion_struct*) _temp966)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL1116: _temp1115=(
struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*) _temp966)->f1; goto
_LL1114; _LL1114: _temp1113=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp966)->f2; goto _LL1003;} else{ goto _LL1004;} _LL1004: if(( unsigned int)
_temp966 > 1u?(( struct _tunion_struct*) _temp966)->tag == Cyc_Absyn_Cut_s_tag:
0){ _LL1118: _temp1117=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Cut_s_struct*)
_temp966)->f1; goto _LL1005;} else{ goto _LL1006;} _LL1006: if(( unsigned int)
_temp966 > 1u?(( struct _tunion_struct*) _temp966)->tag == Cyc_Absyn_Splice_s_tag:
0){ _LL1120: _temp1119=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Splice_s_struct*)
_temp966)->f1; goto _LL1007;} else{ goto _LL1008;} _LL1008: if(( unsigned int)
_temp966 > 1u?(( struct _tunion_struct*) _temp966)->tag == Cyc_Absyn_Label_s_tag:
0){ _LL1124: _temp1123=( struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*)
_temp966)->f1; goto _LL1122; _LL1122: _temp1121=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Label_s_struct*) _temp966)->f2; goto _LL1009;} else{ goto
_LL1010;} _LL1010: if(( unsigned int) _temp966 > 1u?(( struct _tunion_struct*)
_temp966)->tag == Cyc_Absyn_TryCatch_s_tag: 0){ _LL1128: _temp1127=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp966)->f1; goto _LL1126; _LL1126:
_temp1125=( struct Cyc_List_List*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp966)->f2; goto _LL1011;} else{ goto _LL967;} _LL969: return; _LL971: { void*
ae= Cyc_NewControlFlow_abstract_exp_top( env, _temp1012); Cyc_NewControlFlow_cf_set_absexps(
s,({ struct Cyc_List_List* _temp1129=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1129->hd=( void*) ae; _temp1129->tl= 0; _temp1129;}));
return;} _LL973: Cyc_NewControlFlow_cf_prepass( env, _temp1016); Cyc_NewControlFlow_cf_prepass(
env, _temp1014); return; _LL975: return; _LL977: { void* ae= Cyc_NewControlFlow_abstract_exp_top(
Cyc_NewControlFlow_use_it_env( env),( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1130= _temp1020; if( _temp1130 == 0){ _throw( Null_Exception);} _temp1130;}));
Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List* _temp1131=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1131->hd=( void*)
ae; _temp1131->tl= 0; _temp1131;})); return;} _LL979: { void* _temp1134; void*
_temp1136; struct _tuple0 _temp1132= Cyc_NewControlFlow_abstract_guard( env,
_temp1026); _LL1137: _temp1136= _temp1132.f1; goto _LL1135; _LL1135: _temp1134=
_temp1132.f2; goto _LL1133; _LL1133: Cyc_NewControlFlow_cf_set_absexps( s,({
struct Cyc_List_List* _temp1138=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1138->hd=( void*) _temp1136; _temp1138->tl=({
struct Cyc_List_List* _temp1139=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1139->hd=( void*) _temp1134; _temp1139->tl= 0;
_temp1139;}); _temp1138;})); Cyc_NewControlFlow_cf_prepass( env, _temp1024); Cyc_NewControlFlow_cf_prepass(
env, _temp1022); return;} _LL981: _temp1042= _temp1032; _temp1040= _temp1030;
_temp1036= _temp1034; goto _LL983; _LL983: { void* _temp1142; void* _temp1144;
struct _tuple0 _temp1140= Cyc_NewControlFlow_abstract_guard( env, _temp1042);
_LL1145: _temp1144= _temp1140.f1; goto _LL1143; _LL1143: _temp1142= _temp1140.f2;
goto _LL1141; _LL1141: Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1146=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1146->hd=( void*) _temp1144; _temp1146->tl=({ struct Cyc_List_List*
_temp1147=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1147->hd=( void*) _temp1142; _temp1147->tl= 0; _temp1147;}); _temp1146;}));
Cyc_NewControlFlow_cf_prepass( env, _temp1036); return;} _LL985: return; _LL987:
return; _LL989:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp1148=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1148[ 0]=({ struct Cyc_Core_Impossible_struct _temp1149; _temp1149.tag= Cyc_Core_Impossible_tag;
_temp1149.f1=( struct _tagged_string)({ char* _temp1150=( char*)"cf_prepass: goto w/o destination";
struct _tagged_string _temp1151; _temp1151.curr= _temp1150; _temp1151.base=
_temp1150; _temp1151.last_plus_one= _temp1150 + 33; _temp1151;}); _temp1149;});
_temp1148;})); _LL991: { struct Cyc_Absyn_Stmt* my_encloser=( Cyc_NewControlFlow_get_stmt_annot(
s))->encloser; struct Cyc_Absyn_Stmt* dest_encloser=( Cyc_NewControlFlow_get_stmt_annot((
struct Cyc_Absyn_Stmt*)({ struct Cyc_Absyn_Stmt* _temp1152= _temp1052; if(
_temp1152 == 0){ _throw( Null_Exception);} _temp1152;})))->encloser; while(
dest_encloser != my_encloser) { struct Cyc_Absyn_Stmt* next_encloser=( Cyc_NewControlFlow_get_stmt_annot(
my_encloser))->encloser; if( next_encloser == my_encloser){ Cyc_Tcutil_terr( s->loc,(
struct _tagged_string)({ char* _temp1153=( char*)"goto enters local scope or exception handler";
struct _tagged_string _temp1154; _temp1154.curr= _temp1153; _temp1154.base=
_temp1153; _temp1154.last_plus_one= _temp1153 + 45; _temp1154;})); break;}
my_encloser= next_encloser;} return;} _LL993: { void* ae1= Cyc_NewControlFlow_abstract_exp_top(
env, _temp1070); void* _temp1157; void* _temp1159; struct _tuple0 _temp1155= Cyc_NewControlFlow_abstract_guard(
env, _temp1068); _LL1160: _temp1159= _temp1155.f1; goto _LL1158; _LL1158:
_temp1157= _temp1155.f2; goto _LL1156; _LL1156: { void* ae3= Cyc_NewControlFlow_abstract_exp_top(
env, _temp1062); Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1161=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1161->hd=( void*) ae1; _temp1161->tl=({ struct Cyc_List_List* _temp1162=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1162->hd=(
void*) _temp1159; _temp1162->tl=({ struct Cyc_List_List* _temp1163=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1163->hd=( void*) _temp1157;
_temp1163->tl=({ struct Cyc_List_List* _temp1164=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1164->hd=( void*) ae3; _temp1164->tl=
0; _temp1164;}); _temp1163;}); _temp1162;}); _temp1161;})); Cyc_NewControlFlow_cf_prepass(
env, _temp1056); return;}} _LL995: { void* ae= Cyc_NewControlFlow_abstract_exp_top(
Cyc_NewControlFlow_use_it_env( env), _temp1074); struct Cyc_List_List* ael= Cyc_NewControlFlow_cf_prepass_scs(
env, _temp1072); Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1165=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1165->hd=( void*) ae; _temp1165->tl= ael; _temp1165;})); return;} _LL997: {
void* _temp1168; void* _temp1170; struct Cyc_NewControlFlow_AbsSynOne _temp1166=
Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_use_exp_unordered( env,
_temp1078, 0)); _LL1171: _temp1170=( void*) _temp1166.inner_exp; goto _LL1169;
_LL1169: _temp1168=( void*) _temp1166.assigns; goto _LL1167; _LL1167: { void* ae=
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp1170, _temp1168);
Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List* _temp1172=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1172->hd=( void*)
ae; _temp1172->tl= 0; _temp1172;})); return;}} _LL999: Cyc_NewControlFlow_add_var_root(
env, _temp1089); if( _temp1089->initializer != 0){ struct Cyc_NewControlFlow_AbsEnv
init_env= Cyc_NewControlFlow_change_lhs( env,({ struct Cyc_List_List* _temp1173=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1173->hd=(
void*) Cyc_CfAbsexp_mkLocalOp( _temp1089); _temp1173->tl= 0; _temp1173;})); void*
ae= Cyc_NewControlFlow_abstract_exp_top( init_env,( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1174= _temp1089->initializer; if( _temp1174 == 0){
_throw( Null_Exception);} _temp1174;})); Cyc_NewControlFlow_cf_set_absexps( s,({
struct Cyc_List_List* _temp1175=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1175->hd=( void*) ae; _temp1175->tl= 0; _temp1175;}));}
Cyc_NewControlFlow_cf_prepass( env, _temp1080); return; _LL1001: for( 0;
_temp1109 != 0; _temp1109= _temp1109->tl){ Cyc_NewControlFlow_add_var_root( env,(
struct Cyc_Absyn_Vardecl*) _temp1109->hd);}{ void* ae= Cyc_NewControlFlow_abstract_exp_top(
Cyc_NewControlFlow_use_it_env( env), _temp1102); Cyc_NewControlFlow_cf_set_absexps(
s,({ struct Cyc_List_List* _temp1176=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1176->hd=( void*) ae; _temp1176->tl= 0; _temp1176;}));
Cyc_NewControlFlow_cf_prepass( env, _temp1091); return;} _LL1003: goto _LL1005;
_LL1005: goto _LL1007; _LL1007:( void) _throw(( struct _xtunion_struct*)({
struct Cyc_Core_Impossible_struct* _temp1177=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp1177[ 0]=({ struct
Cyc_Core_Impossible_struct _temp1178; _temp1178.tag= Cyc_Core_Impossible_tag;
_temp1178.f1=( struct _tagged_string)({ char* _temp1179=( char*)"cf_prepass: bad stmt form";
struct _tagged_string _temp1180; _temp1180.curr= _temp1179; _temp1180.base=
_temp1179; _temp1180.last_plus_one= _temp1179 + 26; _temp1180;}); _temp1178;});
_temp1177;})); _LL1009: Cyc_NewControlFlow_cf_prepass( env, _temp1121); return;
_LL1011: Cyc_NewControlFlow_cf_prepass( env, _temp1127);{ struct Cyc_List_List*
ael= Cyc_NewControlFlow_cf_prepass_scs( env, _temp1125); Cyc_NewControlFlow_cf_set_absexps(
s, ael); return;} _LL967:;}} static int Cyc_NewControlFlow_iterate_cf_check= 0;
static int Cyc_NewControlFlow_iteration_num= 0; struct Cyc_NewControlFlow_AnalEnv;
void Cyc_NewControlFlow_update_tryflow( struct Cyc_NewControlFlow_AnalEnv* env,
void* new_flow){ if( env->in_try){( void*)( env->tryflow=( void*) Cyc_CfFlowInfo_join_flow(
new_flow,( void*) env->tryflow));}} static struct Cyc_NewControlFlow_CFStmtAnnot*
Cyc_NewControlFlow_pre_stmt_check( void* in_flow, struct Cyc_Absyn_Stmt* s){
struct Cyc_NewControlFlow_CFStmtAnnot* annot= Cyc_NewControlFlow_get_stmt_annot(
s);( void*)( annot->flow=( void*) Cyc_CfFlowInfo_join_flow( in_flow,( void*)
annot->flow)); ++ annot->visited; annot->visited == Cyc_NewControlFlow_iteration_num?
0:(( int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp1181=( char*)"new_control_flow.cyc:927 annot->visited == iteration_num";
struct _tagged_string _temp1182; _temp1182.curr= _temp1181; _temp1182.base=
_temp1181; _temp1182.last_plus_one= _temp1181 + 57; _temp1182;})); return annot;}
static void Cyc_NewControlFlow_update_flow( struct Cyc_Absyn_Stmt* s, void* flow){
struct Cyc_NewControlFlow_CFStmtAnnot* annot= Cyc_NewControlFlow_get_stmt_annot(
s); void* new_flow= Cyc_CfFlowInfo_join_flow( flow,( void*) annot->flow); if( !
Cyc_CfFlowInfo_flow_lessthan_approx( new_flow,( void*) annot->flow)){( void*)(
annot->flow=( void*) new_flow); if( annot->visited == Cyc_NewControlFlow_iteration_num){
Cyc_NewControlFlow_iterate_cf_check= 1;}}} static void* Cyc_NewControlFlow_add_init_vars_flow(
struct Cyc_NewControlFlow_AnalEnv* env, void* flow, struct Cyc_List_List* vds){
void* _temp1183= flow; struct Cyc_Dict_Dict* _temp1189; _LL1185: if(( int)
_temp1183 == Cyc_CfFlowInfo_BottomFL){ goto _LL1186;} else{ goto _LL1187;}
_LL1187: if(( unsigned int) _temp1183 > 1u?(( struct _tunion_struct*) _temp1183)->tag
== Cyc_CfFlowInfo_InitsFL_tag: 0){ _LL1190: _temp1189=( struct Cyc_Dict_Dict*)((
struct Cyc_CfFlowInfo_InitsFL_struct*) _temp1183)->f1; goto _LL1188;} else{ goto
_LL1184;} _LL1186: return( void*) Cyc_CfFlowInfo_BottomFL; _LL1188: for( 0; vds
!= 0; vds= vds->tl){ struct Cyc_CfFlowInfo_VarRoot_struct* root=({ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp1191=( struct Cyc_CfFlowInfo_VarRoot_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct));
_temp1191[ 0]=({ struct Cyc_CfFlowInfo_VarRoot_struct _temp1192; _temp1192.tag=
Cyc_CfFlowInfo_VarRoot_tag; _temp1192.f1=( struct Cyc_Absyn_Vardecl*) vds->hd;
_temp1192;}); _temp1191;}); void* pinfo= Cyc_CfFlowInfo_assign_unknown_dict((
void*) Cyc_CfFlowInfo_AllIL,(( void*(*)( struct Cyc_Dict_Dict* d, void* key))
Cyc_Dict_lookup)( env->roots,( void*) root)); _temp1189=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert)( _temp1189,(
void*) root, pinfo);} return( void*)({ struct Cyc_CfFlowInfo_InitsFL_struct*
_temp1193=( struct Cyc_CfFlowInfo_InitsFL_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct));
_temp1193[ 0]=({ struct Cyc_CfFlowInfo_InitsFL_struct _temp1194; _temp1194.tag=
Cyc_CfFlowInfo_InitsFL_tag; _temp1194.f1= _temp1189; _temp1194;}); _temp1193;});
_LL1184:;} static void* Cyc_NewControlFlow_add_vardecls_flow( struct Cyc_NewControlFlow_AnalEnv*
env, void* flow, struct Cyc_List_List* vds){ void* _temp1195= flow; struct Cyc_Dict_Dict*
_temp1201; _LL1197: if(( int) _temp1195 == Cyc_CfFlowInfo_BottomFL){ goto
_LL1198;} else{ goto _LL1199;} _LL1199: if(( unsigned int) _temp1195 > 1u?((
struct _tunion_struct*) _temp1195)->tag == Cyc_CfFlowInfo_InitsFL_tag: 0){
_LL1202: _temp1201=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp1195)->f1; goto _LL1200;} else{ goto _LL1196;} _LL1198: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1200: for( 0; vds != 0; vds= vds->tl){ struct Cyc_CfFlowInfo_VarRoot_struct*
root=({ struct Cyc_CfFlowInfo_VarRoot_struct* _temp1203=( struct Cyc_CfFlowInfo_VarRoot_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct)); _temp1203[ 0]=({
struct Cyc_CfFlowInfo_VarRoot_struct _temp1204; _temp1204.tag= Cyc_CfFlowInfo_VarRoot_tag;
_temp1204.f1=( struct Cyc_Absyn_Vardecl*) vds->hd; _temp1204;}); _temp1203;});
_temp1201=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void* key, void*
data)) Cyc_Dict_insert)( _temp1201,( void*) root,(( void*(*)( struct Cyc_Dict_Dict*
d, void* key)) Cyc_Dict_lookup)( env->roots,( void*) root));} return( void*)({
struct Cyc_CfFlowInfo_InitsFL_struct* _temp1205=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp1205[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp1206; _temp1206.tag= Cyc_CfFlowInfo_InitsFL_tag;
_temp1206.f1= _temp1201; _temp1206;}); _temp1205;}); _LL1196:;} static struct
_tuple0 Cyc_NewControlFlow_cf_evalguard( struct Cyc_NewControlFlow_AnalEnv* env,
struct Cyc_Position_Segment* loc, struct Cyc_List_List* aes, void* in_flow){ if((
void*) aes->hd ==( void*)( aes->tl)->hd){ void* out_flow= Cyc_CfAbsexp_eval_absexp(
env, loc,( void*) aes->hd, in_flow); return({ struct _tuple0 _temp1207;
_temp1207.f1= out_flow; _temp1207.f2= out_flow; _temp1207;});} return({ struct
_tuple0 _temp1208; _temp1208.f1= Cyc_CfAbsexp_eval_absexp( env, loc,( void*) aes->hd,
in_flow); _temp1208.f2= Cyc_CfAbsexp_eval_absexp( env, loc,( void*)( aes->tl)->hd,
in_flow); _temp1208;});} static void Cyc_NewControlFlow_cf_analyze_switch_clauses(
struct Cyc_NewControlFlow_AnalEnv* env, struct Cyc_List_List* scs, struct Cyc_List_List*
aes, void* in_flow){ for( 0; scs != 0; scs= scs->tl){ struct Cyc_Absyn_Switch_clause
_temp1211; struct Cyc_Position_Segment* _temp1212; struct Cyc_Absyn_Stmt*
_temp1214; struct Cyc_Absyn_Exp* _temp1216; struct Cyc_Core_Opt* _temp1218;
struct Cyc_Absyn_Pat* _temp1220; struct Cyc_Absyn_Switch_clause* _temp1209=(
struct Cyc_Absyn_Switch_clause*) scs->hd; _temp1211=* _temp1209; _LL1221:
_temp1220=( struct Cyc_Absyn_Pat*) _temp1211.pattern; goto _LL1219; _LL1219:
_temp1218=( struct Cyc_Core_Opt*) _temp1211.pat_vars; goto _LL1217; _LL1217:
_temp1216=( struct Cyc_Absyn_Exp*) _temp1211.where_clause; goto _LL1215; _LL1215:
_temp1214=( struct Cyc_Absyn_Stmt*) _temp1211.body; goto _LL1213; _LL1213:
_temp1212=( struct Cyc_Position_Segment*) _temp1211.loc; goto _LL1210; _LL1210: {
void* body_inflow= Cyc_NewControlFlow_add_vardecls_flow( env, in_flow,( struct
Cyc_List_List*) _temp1218->v); body_inflow= Cyc_NewControlFlow_add_init_vars_flow(
env, body_inflow,( struct Cyc_List_List*) _temp1218->v);{ void* body_outflow;
if( _temp1216 != 0){ void* _temp1225; void* _temp1227; struct _tuple0 _temp1223=
Cyc_NewControlFlow_cf_evalguard( env,(( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1222= _temp1216; if( _temp1222 == 0){ _throw( Null_Exception);} _temp1222;}))->loc,
aes, body_inflow); _LL1228: _temp1227= _temp1223.f1; goto _LL1226; _LL1226:
_temp1225= _temp1223.f2; goto _LL1224; _LL1224: aes=( aes->tl)->tl; in_flow=
_temp1225; body_outflow= Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1214,
_temp1227);} else{ body_outflow= Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp1214, body_inflow);}{ void* _temp1229= body_outflow; _LL1231: if(( int)
_temp1229 == Cyc_CfFlowInfo_BottomFL){ goto _LL1232;} else{ goto _LL1233;}
_LL1233: goto _LL1234; _LL1232: goto _LL1230; _LL1234: Cyc_Tcutil_terr(
_temp1214->loc,( struct _tagged_string)({ char* _temp1235=( char*)"switch clause may implicitly fallthru";
struct _tagged_string _temp1236; _temp1236.curr= _temp1235; _temp1236.base=
_temp1235; _temp1236.last_plus_one= _temp1235 + 38; _temp1236;})); goto _LL1230;
_LL1230:;}}}}} void* Cyc_NewControlFlow_cf_analyze_stmt( struct Cyc_NewControlFlow_AnalEnv*
env, struct Cyc_Absyn_Stmt* s, void* in_flow){ struct Cyc_NewControlFlow_CFStmtAnnot*
annot= Cyc_NewControlFlow_pre_stmt_check( in_flow, s); in_flow=( void*) annot->flow;{
struct Cyc_List_List* aes= annot->absexps; void* _temp1237=( void*) s->r; struct
Cyc_Absyn_Exp* _temp1279; struct Cyc_Absyn_Exp* _temp1281; struct Cyc_Absyn_Exp*
_temp1283; struct Cyc_Absyn_Stmt* _temp1285; struct Cyc_Absyn_Stmt* _temp1287;
struct Cyc_Absyn_Stmt* _temp1289; struct Cyc_Absyn_Stmt* _temp1291; struct Cyc_Absyn_Exp*
_temp1293; struct Cyc_Absyn_Stmt* _temp1295; struct _tuple3 _temp1297; struct
Cyc_Absyn_Stmt* _temp1299; struct Cyc_Absyn_Exp* _temp1301; struct _tuple3
_temp1303; struct Cyc_Absyn_Stmt* _temp1305; struct Cyc_Absyn_Exp* _temp1307;
struct Cyc_Absyn_Stmt* _temp1309; struct Cyc_Absyn_Stmt* _temp1311; struct
_tuple3 _temp1313; struct Cyc_Absyn_Stmt* _temp1315; struct Cyc_Absyn_Exp*
_temp1317; struct _tuple3 _temp1319; struct Cyc_Absyn_Stmt* _temp1321; struct
Cyc_Absyn_Exp* _temp1323; struct Cyc_Absyn_Exp* _temp1325; struct Cyc_Absyn_Stmt*
_temp1327; struct Cyc_Absyn_Switch_clause** _temp1329; struct Cyc_Absyn_Switch_clause*
_temp1331; struct Cyc_List_List* _temp1332; struct Cyc_Absyn_Stmt* _temp1334;
struct Cyc_Absyn_Stmt* _temp1336; struct Cyc_Absyn_Stmt* _temp1338; struct
_tagged_string* _temp1340; struct Cyc_List_List* _temp1342; struct Cyc_Absyn_Exp*
_temp1344; struct Cyc_List_List* _temp1346; struct Cyc_Absyn_Stmt* _temp1348;
struct Cyc_Absyn_Stmt* _temp1350; struct Cyc_Absyn_Decl* _temp1352; struct Cyc_Absyn_Decl
_temp1354; struct Cyc_Position_Segment* _temp1355; void* _temp1357; struct Cyc_Absyn_Vardecl*
_temp1359; struct Cyc_Absyn_Stmt* _temp1361; struct Cyc_Absyn_Decl* _temp1363;
struct Cyc_Absyn_Decl _temp1365; struct Cyc_Position_Segment* _temp1366; void*
_temp1368; int _temp1370; struct Cyc_Absyn_Exp* _temp1372; struct Cyc_Core_Opt*
_temp1374; struct Cyc_Core_Opt* _temp1376; struct Cyc_Core_Opt _temp1378; struct
Cyc_List_List* _temp1379; struct Cyc_Absyn_Pat* _temp1381; struct Cyc_Absyn_Stmt*
_temp1383; struct _tagged_string* _temp1385; _LL1239: if(( int) _temp1237 == Cyc_Absyn_Skip_s){
goto _LL1240;} else{ goto _LL1241;} _LL1241: if(( unsigned int) _temp1237 > 1u?((
struct _tunion_struct*) _temp1237)->tag == Cyc_Absyn_Return_s_tag: 0){ _LL1280:
_temp1279=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp1237)->f1; if( _temp1279 == 0){ goto _LL1242;} else{ goto _LL1243;}} else{
goto _LL1243;} _LL1243: if(( unsigned int) _temp1237 > 1u?(( struct
_tunion_struct*) _temp1237)->tag == Cyc_Absyn_Return_s_tag: 0){ _LL1282:
_temp1281=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp1237)->f1; goto _LL1244;} else{ goto _LL1245;} _LL1245: if(( unsigned int)
_temp1237 > 1u?(( struct _tunion_struct*) _temp1237)->tag == Cyc_Absyn_Exp_s_tag:
0){ _LL1284: _temp1283=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp1237)->f1; goto _LL1246;} else{ goto _LL1247;} _LL1247: if(( unsigned int)
_temp1237 > 1u?(( struct _tunion_struct*) _temp1237)->tag == Cyc_Absyn_Seq_s_tag:
0){ _LL1288: _temp1287=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp1237)->f1; goto _LL1286; _LL1286: _temp1285=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp1237)->f2; goto _LL1248;} else{ goto
_LL1249;} _LL1249: if(( unsigned int) _temp1237 > 1u?(( struct _tunion_struct*)
_temp1237)->tag == Cyc_Absyn_IfThenElse_s_tag: 0){ _LL1294: _temp1293=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp1237)->f1; goto
_LL1292; _LL1292: _temp1291=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1237)->f2; goto _LL1290; _LL1290: _temp1289=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1237)->f3; goto _LL1250;} else{ goto
_LL1251;} _LL1251: if(( unsigned int) _temp1237 > 1u?(( struct _tunion_struct*)
_temp1237)->tag == Cyc_Absyn_While_s_tag: 0){ _LL1298: _temp1297=( struct
_tuple3)(( struct Cyc_Absyn_While_s_struct*) _temp1237)->f1; _LL1302: _temp1301=
_temp1297.f1; goto _LL1300; _LL1300: _temp1299= _temp1297.f2; goto _LL1296;
_LL1296: _temp1295=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_While_s_struct*)
_temp1237)->f2; goto _LL1252;} else{ goto _LL1253;} _LL1253: if(( unsigned int)
_temp1237 > 1u?(( struct _tunion_struct*) _temp1237)->tag == Cyc_Absyn_Do_s_tag:
0){ _LL1310: _temp1309=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*)
_temp1237)->f1; goto _LL1304; _LL1304: _temp1303=( struct _tuple3)(( struct Cyc_Absyn_Do_s_struct*)
_temp1237)->f2; _LL1308: _temp1307= _temp1303.f1; goto _LL1306; _LL1306:
_temp1305= _temp1303.f2; goto _LL1254;} else{ goto _LL1255;} _LL1255: if((
unsigned int) _temp1237 > 1u?(( struct _tunion_struct*) _temp1237)->tag == Cyc_Absyn_For_s_tag:
0){ _LL1326: _temp1325=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp1237)->f1; goto _LL1320; _LL1320: _temp1319=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp1237)->f2; _LL1324: _temp1323= _temp1319.f1; goto _LL1322; _LL1322:
_temp1321= _temp1319.f2; goto _LL1314; _LL1314: _temp1313=( struct _tuple3)((
struct Cyc_Absyn_For_s_struct*) _temp1237)->f3; _LL1318: _temp1317= _temp1313.f1;
goto _LL1316; _LL1316: _temp1315= _temp1313.f2; goto _LL1312; _LL1312: _temp1311=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp1237)->f4; goto
_LL1256;} else{ goto _LL1257;} _LL1257: if(( unsigned int) _temp1237 > 1u?((
struct _tunion_struct*) _temp1237)->tag == Cyc_Absyn_Break_s_tag: 0){ _LL1328:
_temp1327=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Break_s_struct*)
_temp1237)->f1; if( _temp1327 == 0){ goto _LL1258;} else{ goto _LL1259;}} else{
goto _LL1259;} _LL1259: if(( unsigned int) _temp1237 > 1u?(( struct
_tunion_struct*) _temp1237)->tag == Cyc_Absyn_Fallthru_s_tag: 0){ _LL1333:
_temp1332=( struct Cyc_List_List*)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1237)->f1; goto _LL1330; _LL1330: _temp1329=( struct Cyc_Absyn_Switch_clause**)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1237)->f2; if( _temp1329 == 0){ goto
_LL1261;} else{ _temp1331=* _temp1329; goto _LL1260;}} else{ goto _LL1261;}
_LL1261: if(( unsigned int) _temp1237 > 1u?(( struct _tunion_struct*) _temp1237)->tag
== Cyc_Absyn_Break_s_tag: 0){ _LL1335: _temp1334=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Break_s_struct*) _temp1237)->f1; goto _LL1262;} else{ goto
_LL1263;} _LL1263: if(( unsigned int) _temp1237 > 1u?(( struct _tunion_struct*)
_temp1237)->tag == Cyc_Absyn_Continue_s_tag: 0){ _LL1337: _temp1336=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Continue_s_struct*) _temp1237)->f1; goto _LL1264;} else{ goto
_LL1265;} _LL1265: if(( unsigned int) _temp1237 > 1u?(( struct _tunion_struct*)
_temp1237)->tag == Cyc_Absyn_Goto_s_tag: 0){ _LL1341: _temp1340=( struct
_tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp1237)->f1; goto _LL1339;
_LL1339: _temp1338=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp1237)->f2; goto _LL1266;} else{ goto _LL1267;} _LL1267: if(( unsigned int)
_temp1237 > 1u?(( struct _tunion_struct*) _temp1237)->tag == Cyc_Absyn_Switch_s_tag:
0){ _LL1345: _temp1344=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp1237)->f1; goto _LL1343; _LL1343: _temp1342=( struct Cyc_List_List*)((
struct Cyc_Absyn_Switch_s_struct*) _temp1237)->f2; goto _LL1268;} else{ goto
_LL1269;} _LL1269: if(( unsigned int) _temp1237 > 1u?(( struct _tunion_struct*)
_temp1237)->tag == Cyc_Absyn_TryCatch_s_tag: 0){ _LL1349: _temp1348=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp1237)->f1; goto _LL1347; _LL1347:
_temp1346=( struct Cyc_List_List*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1237)->f2; goto _LL1270;} else{ goto _LL1271;} _LL1271: if(( unsigned int)
_temp1237 > 1u?(( struct _tunion_struct*) _temp1237)->tag == Cyc_Absyn_Decl_s_tag:
0){ _LL1353: _temp1352=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1237)->f1; _temp1354=* _temp1352; _LL1358: _temp1357=( void*) _temp1354.r;
if((( struct _tunion_struct*) _temp1357)->tag == Cyc_Absyn_Var_d_tag){ _LL1360:
_temp1359=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp1357)->f1; goto _LL1356;} else{ goto _LL1273;} _LL1356: _temp1355=( struct
Cyc_Position_Segment*) _temp1354.loc; goto _LL1351; _LL1351: _temp1350=( struct
Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1237)->f2; goto _LL1272;}
else{ goto _LL1273;} _LL1273: if(( unsigned int) _temp1237 > 1u?(( struct
_tunion_struct*) _temp1237)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL1364: _temp1363=(
struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1237)->f1;
_temp1365=* _temp1363; _LL1369: _temp1368=( void*) _temp1365.r; if((( struct
_tunion_struct*) _temp1368)->tag == Cyc_Absyn_Let_d_tag){ _LL1382: _temp1381=(
struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*) _temp1368)->f1; goto
_LL1377; _LL1377: _temp1376=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1368)->f2; if( _temp1376 == 0){ goto _LL1275;} else{ _temp1378=* _temp1376;
_LL1380: _temp1379=( struct Cyc_List_List*) _temp1378.v; goto _LL1375;} _LL1375:
_temp1374=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp1368)->f3;
goto _LL1373; _LL1373: _temp1372=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1368)->f4; goto _LL1371; _LL1371: _temp1370=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp1368)->f5; goto _LL1367;} else{ goto _LL1275;} _LL1367: _temp1366=( struct
Cyc_Position_Segment*) _temp1365.loc; goto _LL1362; _LL1362: _temp1361=( struct
Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1237)->f2; goto _LL1274;}
else{ goto _LL1275;} _LL1275: if(( unsigned int) _temp1237 > 1u?(( struct
_tunion_struct*) _temp1237)->tag == Cyc_Absyn_Label_s_tag: 0){ _LL1386:
_temp1385=( struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*)
_temp1237)->f1; goto _LL1384; _LL1384: _temp1383=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Label_s_struct*) _temp1237)->f2; goto _LL1276;} else{ goto
_LL1277;} _LL1277: goto _LL1278; _LL1240: return in_flow; _LL1242: return( void*)
Cyc_CfFlowInfo_BottomFL; _LL1244: Cyc_CfAbsexp_eval_absexp( env, _temp1281->loc,(
void*) aes->hd, in_flow); return( void*) Cyc_CfFlowInfo_BottomFL; _LL1246:
return Cyc_CfAbsexp_eval_absexp( env, _temp1283->loc,( void*) aes->hd, in_flow);
_LL1248: return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1285, Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1287, in_flow)); _LL1250: { void* _temp1389; void* _temp1391; struct
_tuple0 _temp1387= Cyc_NewControlFlow_cf_evalguard( env, _temp1293->loc, aes,
in_flow); _LL1392: _temp1391= _temp1387.f1; goto _LL1390; _LL1390: _temp1389=
_temp1387.f2; goto _LL1388; _LL1388: return Cyc_CfFlowInfo_join_flow( Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1291, _temp1391), Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1289,
_temp1389));} _LL1252: { void* e_inflow=( void*)( Cyc_NewControlFlow_pre_stmt_check(
in_flow, _temp1299))->flow; void* _temp1395; void* _temp1397; struct _tuple0
_temp1393= Cyc_NewControlFlow_cf_evalguard( env, _temp1301->loc, aes, e_inflow);
_LL1398: _temp1397= _temp1393.f1; goto _LL1396; _LL1396: _temp1395= _temp1393.f2;
goto _LL1394; _LL1394: { void* body_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1295, _temp1397); Cyc_NewControlFlow_update_flow( _temp1299,
body_outflow); return _temp1395;}} _LL1254: { void* body_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1309, in_flow); void* e_inflow=( void*)( Cyc_NewControlFlow_pre_stmt_check(
body_outflow, _temp1305))->flow; void* _temp1401; void* _temp1403; struct
_tuple0 _temp1399= Cyc_NewControlFlow_cf_evalguard( env, _temp1307->loc, aes,
e_inflow); _LL1404: _temp1403= _temp1399.f1; goto _LL1402; _LL1402: _temp1401=
_temp1399.f2; goto _LL1400; _LL1400: Cyc_NewControlFlow_update_flow( _temp1309,
_temp1403); return _temp1401;} _LL1256: { void* e1_outflow= Cyc_CfAbsexp_eval_absexp(
env, _temp1325->loc,( void*) aes->hd, in_flow); void* e2_inflow=( void*)( Cyc_NewControlFlow_pre_stmt_check(
e1_outflow, _temp1321))->flow; void* _temp1407; void* _temp1409; struct _tuple0
_temp1405= Cyc_NewControlFlow_cf_evalguard( env, _temp1323->loc, aes->tl,
e2_inflow); _LL1410: _temp1409= _temp1405.f1; goto _LL1408; _LL1408: _temp1407=
_temp1405.f2; goto _LL1406; _LL1406: { void* body_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1311, _temp1409); void* e3_inflow=( void*)( Cyc_NewControlFlow_pre_stmt_check(
body_outflow, _temp1315))->flow; void* e3_outflow= Cyc_CfAbsexp_eval_absexp( env,
_temp1317->loc,( void*)((( aes->tl)->tl)->tl)->hd, e3_inflow); Cyc_NewControlFlow_update_flow(
_temp1321, e3_outflow); return _temp1407;}} _LL1258: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1260: in_flow= Cyc_CfAbsexp_eval_absexp( env, s->loc,( void*) aes->hd,
in_flow);{ struct Cyc_List_List* ftvars=( struct Cyc_List_List*)( _temp1331->pat_vars)->v;
in_flow= Cyc_NewControlFlow_add_vardecls_flow( env, in_flow, ftvars); in_flow=
Cyc_NewControlFlow_add_init_vars_flow( env, in_flow, ftvars); _temp1334=( struct
Cyc_Absyn_Stmt*) _temp1331->body; goto _LL1262;} _LL1262: _temp1336= _temp1334;
goto _LL1264; _LL1264: _temp1338= _temp1336; goto _LL1266; _LL1266: Cyc_NewControlFlow_update_flow((
struct Cyc_Absyn_Stmt*)({ struct Cyc_Absyn_Stmt* _temp1411= _temp1338; if(
_temp1411 == 0){ _throw( Null_Exception);} _temp1411;}), in_flow); return( void*)
Cyc_CfFlowInfo_BottomFL; _LL1268: in_flow= Cyc_CfAbsexp_eval_absexp( env,
_temp1344->loc,( void*) aes->hd, in_flow); Cyc_NewControlFlow_cf_analyze_switch_clauses(
env, _temp1342, aes->tl, in_flow); return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1270: { struct Cyc_NewControlFlow_AnalEnv _temp1414; void* _temp1415; int
_temp1417; struct Cyc_Dict_Dict* _temp1419; struct Cyc_NewControlFlow_AnalEnv*
_temp1412= env; _temp1414=* _temp1412; _LL1420: _temp1419=( struct Cyc_Dict_Dict*)
_temp1414.roots; goto _LL1418; _LL1418: _temp1417=( int) _temp1414.in_try; goto
_LL1416; _LL1416: _temp1415=( void*) _temp1414.tryflow; goto _LL1413; _LL1413:
env->in_try= 1;( void*)( env->tryflow=( void*) in_flow);{ void* s_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1348, in_flow); void* scs_inflow=( void*) env->tryflow; env->in_try=
_temp1417;( void*)( env->tryflow=( void*) _temp1415); Cyc_NewControlFlow_update_tryflow(
env, scs_inflow); Cyc_NewControlFlow_cf_analyze_switch_clauses( env, _temp1346,
aes, scs_inflow); return s_outflow;}} _LL1272: { struct Cyc_List_List l=({
struct Cyc_List_List _temp1421; _temp1421.hd=( void*) _temp1359; _temp1421.tl= 0;
_temp1421;}); void* body_inflow= Cyc_NewControlFlow_add_vardecls_flow( env,
in_flow,( struct Cyc_List_List*)& l); struct Cyc_Absyn_Exp* e= _temp1359->initializer;
if( e != 0){ body_inflow= Cyc_CfAbsexp_eval_absexp( env,(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1422= e; if( _temp1422 == 0){ _throw( Null_Exception);}
_temp1422;}))->loc,( void*) aes->hd, body_inflow);} return Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1350, body_inflow);} _LL1274: { void* body_inflow= Cyc_NewControlFlow_add_vardecls_flow(
env, in_flow, _temp1379); body_inflow= Cyc_CfAbsexp_eval_absexp( env, _temp1372->loc,(
void*) aes->hd, body_inflow); body_inflow= Cyc_NewControlFlow_add_init_vars_flow(
env, body_inflow, _temp1379); return Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp1361, body_inflow);} _LL1276: return Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1383, in_flow); _LL1278:( void) _throw(( struct _xtunion_struct*)({
struct Cyc_Core_Impossible_struct* _temp1423=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp1423[ 0]=({ struct
Cyc_Core_Impossible_struct _temp1424; _temp1424.tag= Cyc_Core_Impossible_tag;
_temp1424.f1=( struct _tagged_string)({ char* _temp1425=( char*)"cf_analyze_stmt -- bad stmt syntax or unimplemented stmt form";
struct _tagged_string _temp1426; _temp1426.curr= _temp1425; _temp1426.base=
_temp1425; _temp1426.last_plus_one= _temp1425 + 62; _temp1426;}); _temp1424;});
_temp1423;})); _LL1238:;}} void Cyc_NewControlFlow_cf_postpass( struct Cyc_Absyn_Stmt*
s){ s->annot=( struct _xtunion_struct*)({ struct Cyc_Absyn_EmptyAnnot_struct*
_temp1427=( struct Cyc_Absyn_EmptyAnnot_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EmptyAnnot_struct));
_temp1427[ 0]=({ struct Cyc_Absyn_EmptyAnnot_struct _temp1428; _temp1428.tag=
Cyc_Absyn_EmptyAnnot_tag; _temp1428;}); _temp1427;});{ void* _temp1429=( void*)
s->r; struct Cyc_Absyn_Stmt* _temp1451; struct Cyc_Absyn_Stmt* _temp1453; struct
Cyc_Absyn_Stmt* _temp1455; struct Cyc_Absyn_Stmt* _temp1457; struct Cyc_Absyn_Exp*
_temp1459; struct Cyc_Absyn_Stmt* _temp1461; struct _tuple3 _temp1463; struct
Cyc_Absyn_Stmt* _temp1465; struct Cyc_Absyn_Exp* _temp1467; struct _tuple3
_temp1469; struct Cyc_Absyn_Stmt* _temp1471; struct Cyc_Absyn_Exp* _temp1473;
struct Cyc_Absyn_Exp* _temp1475; struct Cyc_Absyn_Stmt* _temp1477; struct
_tuple3 _temp1479; struct Cyc_Absyn_Stmt* _temp1481; struct Cyc_Absyn_Exp*
_temp1483; struct _tuple3 _temp1485; struct Cyc_Absyn_Stmt* _temp1487; struct
Cyc_Absyn_Exp* _temp1489; struct Cyc_Absyn_Stmt* _temp1491; struct Cyc_Absyn_Stmt*
_temp1493; struct _tagged_string* _temp1495; struct Cyc_Absyn_Stmt* _temp1497;
struct Cyc_Absyn_Decl* _temp1499; struct Cyc_List_List* _temp1501; struct Cyc_Absyn_Stmt*
_temp1503; struct Cyc_List_List* _temp1505; struct Cyc_Absyn_Exp* _temp1507;
_LL1431: if(( unsigned int) _temp1429 > 1u?(( struct _tunion_struct*) _temp1429)->tag
== Cyc_Absyn_Seq_s_tag: 0){ _LL1454: _temp1453=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp1429)->f1; goto _LL1452; _LL1452: _temp1451=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1429)->f2; goto
_LL1432;} else{ goto _LL1433;} _LL1433: if(( unsigned int) _temp1429 > 1u?((
struct _tunion_struct*) _temp1429)->tag == Cyc_Absyn_IfThenElse_s_tag: 0){
_LL1460: _temp1459=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1429)->f1; goto _LL1458; _LL1458: _temp1457=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1429)->f2; goto _LL1456; _LL1456:
_temp1455=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1429)->f3; goto _LL1434;} else{ goto _LL1435;} _LL1435: if(( unsigned int)
_temp1429 > 1u?(( struct _tunion_struct*) _temp1429)->tag == Cyc_Absyn_For_s_tag:
0){ _LL1476: _temp1475=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp1429)->f1; goto _LL1470; _LL1470: _temp1469=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp1429)->f2; _LL1474: _temp1473= _temp1469.f1; goto _LL1472; _LL1472:
_temp1471= _temp1469.f2; goto _LL1464; _LL1464: _temp1463=( struct _tuple3)((
struct Cyc_Absyn_For_s_struct*) _temp1429)->f3; _LL1468: _temp1467= _temp1463.f1;
goto _LL1466; _LL1466: _temp1465= _temp1463.f2; goto _LL1462; _LL1462: _temp1461=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp1429)->f4; goto
_LL1436;} else{ goto _LL1437;} _LL1437: if(( unsigned int) _temp1429 > 1u?((
struct _tunion_struct*) _temp1429)->tag == Cyc_Absyn_While_s_tag: 0){ _LL1480:
_temp1479=( struct _tuple3)(( struct Cyc_Absyn_While_s_struct*) _temp1429)->f1;
_LL1484: _temp1483= _temp1479.f1; goto _LL1482; _LL1482: _temp1481= _temp1479.f2;
goto _LL1478; _LL1478: _temp1477=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_While_s_struct*)
_temp1429)->f2; goto _LL1438;} else{ goto _LL1439;} _LL1439: if(( unsigned int)
_temp1429 > 1u?(( struct _tunion_struct*) _temp1429)->tag == Cyc_Absyn_Do_s_tag:
0){ _LL1492: _temp1491=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*)
_temp1429)->f1; goto _LL1486; _LL1486: _temp1485=( struct _tuple3)(( struct Cyc_Absyn_Do_s_struct*)
_temp1429)->f2; _LL1490: _temp1489= _temp1485.f1; goto _LL1488; _LL1488:
_temp1487= _temp1485.f2; goto _LL1440;} else{ goto _LL1441;} _LL1441: if((
unsigned int) _temp1429 > 1u?(( struct _tunion_struct*) _temp1429)->tag == Cyc_Absyn_Label_s_tag:
0){ _LL1496: _temp1495=( struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*)
_temp1429)->f1; goto _LL1494; _LL1494: _temp1493=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Label_s_struct*) _temp1429)->f2; goto _LL1442;} else{ goto
_LL1443;} _LL1443: if(( unsigned int) _temp1429 > 1u?(( struct _tunion_struct*)
_temp1429)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL1500: _temp1499=( struct Cyc_Absyn_Decl*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1429)->f1; goto _LL1498; _LL1498:
_temp1497=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1429)->f2;
goto _LL1444;} else{ goto _LL1445;} _LL1445: if(( unsigned int) _temp1429 > 1u?((
struct _tunion_struct*) _temp1429)->tag == Cyc_Absyn_TryCatch_s_tag: 0){ _LL1504:
_temp1503=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1429)->f1; goto _LL1502; _LL1502: _temp1501=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp1429)->f2; goto _LL1446;} else{ goto
_LL1447;} _LL1447: if(( unsigned int) _temp1429 > 1u?(( struct _tunion_struct*)
_temp1429)->tag == Cyc_Absyn_Switch_s_tag: 0){ _LL1508: _temp1507=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Switch_s_struct*) _temp1429)->f1; goto _LL1506; _LL1506:
_temp1505=( struct Cyc_List_List*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp1429)->f2; goto _LL1448;} else{ goto _LL1449;} _LL1449: goto _LL1450;
_LL1432: _temp1457= _temp1453; _temp1455= _temp1451; goto _LL1434; _LL1434: Cyc_NewControlFlow_cf_postpass(
_temp1457); Cyc_NewControlFlow_cf_postpass( _temp1455); return; _LL1436: Cyc_NewControlFlow_cf_postpass(
_temp1465); _temp1481= _temp1461; _temp1477= _temp1471; goto _LL1438; _LL1438:
_temp1491= _temp1477; _temp1487= _temp1481; goto _LL1440; _LL1440: Cyc_NewControlFlow_cf_postpass(
_temp1487); _temp1493= _temp1491; goto _LL1442; _LL1442: _temp1497= _temp1493;
goto _LL1444; _LL1444: Cyc_NewControlFlow_cf_postpass( _temp1497); return;
_LL1446: Cyc_NewControlFlow_cf_postpass( _temp1503); _temp1505= _temp1501; goto
_LL1448; _LL1448: for( 0; _temp1505 != 0; _temp1505= _temp1505->tl){ Cyc_NewControlFlow_cf_postpass(((
struct Cyc_Absyn_Switch_clause*) _temp1505->hd)->body);} return; _LL1450:
return; _LL1430:;}} void Cyc_NewControlFlow_cf_check_fun( struct Cyc_Absyn_Fndecl*
fd){ struct Cyc_NewControlFlow_AbsEnv absenv=({ struct Cyc_NewControlFlow_AbsEnv
_temp1509; _temp1509.shared=({ struct Cyc_NewControlFlow_SharedAbsEnv* _temp1510=(
struct Cyc_NewControlFlow_SharedAbsEnv*) GC_malloc( sizeof( struct Cyc_NewControlFlow_SharedAbsEnv));
_temp1510->all_roots=(( struct Cyc_Dict_Dict*(*)( int(* comp)( void*, void*)))
Cyc_Dict_empty)( Cyc_CfFlowInfo_local_root_cmp); _temp1510->comprehension_vars=
0; _temp1510;}); _temp1509.lhsides= 0; _temp1509.access_path= 0; _temp1509;});
struct Cyc_List_List* arg_vds=( struct Cyc_List_List*)( fd->param_vardecls)->v;{
struct Cyc_List_List* vds= arg_vds; for( 0; vds != 0; vds= vds->tl){ Cyc_NewControlFlow_add_var_root(
absenv,( struct Cyc_Absyn_Vardecl*) vds->hd);}} Cyc_NewControlFlow_cf_prepass(
absenv, fd->body);{ struct Cyc_List_List* _temp1513; struct Cyc_List_List*
_temp1515; struct Cyc_NewControlFlow_SharedAbsEnv* _temp1517; struct Cyc_NewControlFlow_SharedAbsEnv
_temp1519; struct Cyc_List_List* _temp1520; struct Cyc_Dict_Dict* _temp1522;
struct Cyc_NewControlFlow_AbsEnv _temp1511= absenv; _LL1518: _temp1517=( struct
Cyc_NewControlFlow_SharedAbsEnv*) _temp1511.shared; _temp1519=* _temp1517;
_LL1523: _temp1522=( struct Cyc_Dict_Dict*) _temp1519.all_roots; goto _LL1521;
_LL1521: _temp1520=( struct Cyc_List_List*) _temp1519.comprehension_vars; goto
_LL1516; _LL1516: _temp1515=( struct Cyc_List_List*) _temp1511.lhsides; goto
_LL1514; _LL1514: _temp1513=( struct Cyc_List_List*) _temp1511.access_path; goto
_LL1512; _LL1512: { struct Cyc_NewControlFlow_AnalEnv* analenv=({ struct Cyc_NewControlFlow_AnalEnv*
_temp1524=( struct Cyc_NewControlFlow_AnalEnv*) GC_malloc( sizeof( struct Cyc_NewControlFlow_AnalEnv));
_temp1524->roots= _temp1522; _temp1524->in_try= 0; _temp1524->tryflow=( void*)((
void*) Cyc_CfFlowInfo_BottomFL); _temp1524;}); void* in_flow=( void*)({ struct
Cyc_CfFlowInfo_InitsFL_struct* _temp1525=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp1525[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp1526; _temp1526.tag= Cyc_CfFlowInfo_InitsFL_tag;
_temp1526.f1=(( struct Cyc_Dict_Dict*(*)( int(* comp)( void*, void*))) Cyc_Dict_empty)(
Cyc_CfFlowInfo_local_root_cmp); _temp1526;}); _temp1525;}); in_flow= Cyc_NewControlFlow_add_init_vars_flow(
analenv, in_flow, arg_vds); in_flow= Cyc_NewControlFlow_add_vardecls_flow(
analenv, in_flow, _temp1520);{ void* out_flow= in_flow; Cyc_NewControlFlow_iteration_num=
0; Cyc_NewControlFlow_iterate_cf_check= 1; while( Cyc_NewControlFlow_iterate_cf_check) {
++ Cyc_NewControlFlow_iteration_num; Cyc_NewControlFlow_iterate_cf_check= 0;
out_flow= Cyc_NewControlFlow_cf_analyze_stmt( analenv, fd->body, in_flow);}{
void* _temp1527= out_flow; _LL1529: if(( int) _temp1527 == Cyc_CfFlowInfo_BottomFL){
goto _LL1530;} else{ goto _LL1531;} _LL1531: goto _LL1532; _LL1530: goto _LL1528;
_LL1532: if(( void*) fd->ret_type !=( void*) Cyc_Absyn_VoidType){ Cyc_Tcutil_terr((
fd->body)->loc,( struct _tagged_string)({ char* _temp1533=( char*)"function may complete without returning a value";
struct _tagged_string _temp1534; _temp1534.curr= _temp1533; _temp1534.base=
_temp1533; _temp1534.last_plus_one= _temp1533 + 48; _temp1534;}));} goto _LL1528;
_LL1528:;} Cyc_NewControlFlow_cf_postpass( fd->body);}}}} void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List* ds){ for( 0; ds != 0; ds= ds->tl){ void* _temp1535=( void*)((
struct Cyc_Absyn_Decl*) ds->hd)->r; struct Cyc_Absyn_Fndecl* _temp1545; struct
Cyc_List_List* _temp1547; struct _tuple1* _temp1549; struct Cyc_List_List*
_temp1551; struct _tagged_string* _temp1553; _LL1537: if((( struct
_tunion_struct*) _temp1535)->tag == Cyc_Absyn_Fn_d_tag){ _LL1546: _temp1545=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*) _temp1535)->f1; goto
_LL1538;} else{ goto _LL1539;} _LL1539: if((( struct _tunion_struct*) _temp1535)->tag
== Cyc_Absyn_Using_d_tag){ _LL1550: _temp1549=( struct _tuple1*)(( struct Cyc_Absyn_Using_d_struct*)
_temp1535)->f1; goto _LL1548; _LL1548: _temp1547=( struct Cyc_List_List*)((
struct Cyc_Absyn_Using_d_struct*) _temp1535)->f2; goto _LL1540;} else{ goto
_LL1541;} _LL1541: if((( struct _tunion_struct*) _temp1535)->tag == Cyc_Absyn_Namespace_d_tag){
_LL1554: _temp1553=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1535)->f1; goto _LL1552; _LL1552: _temp1551=( struct Cyc_List_List*)((
struct Cyc_Absyn_Namespace_d_struct*) _temp1535)->f2; goto _LL1542;} else{ goto
_LL1543;} _LL1543: goto _LL1544; _LL1538: Cyc_NewControlFlow_cf_check_fun(
_temp1545); goto _LL1536; _LL1540: _temp1551= _temp1547; goto _LL1542; _LL1542:
Cyc_NewControlFlow_cf_check( _temp1551); goto _LL1536; _LL1544: goto _LL1536;
_LL1536:;}}