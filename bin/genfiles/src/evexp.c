#include "cyc_include.h"

 struct _tuple0{ void* f1; struct _tagged_string* f2; } ; struct _tuple1{ struct
Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual* f2; void* f3; } ; struct _tuple2{
struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; } ; struct _tuple3{ void*
f1; void* f2; } ; struct _tuple4{ struct Cyc_Absyn_Tqual* f1; void* f2; } ;
struct _tuple5{ void* f1; int f2; } ; typedef int Cyc_ptrdiff_t; typedef
unsigned int Cyc_size_t; typedef unsigned short Cyc_wchar_t; typedef
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
14u]; struct Cyc_List_List_mismatch_struct{ char* tag; } ; extern char Cyc_List_Nth_tag[
4u]; struct Cyc_List_Nth_struct{ char* tag; } ; struct Cyc_Lineno_Pos{ struct
_tagged_string logical_file; struct _tagged_string line; int line_no; int col; }
; typedef struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t; extern char Cyc_Position_Exit_tag[
5u]; struct Cyc_Position_Exit_struct{ char* tag; } ; struct Cyc_Position_Segment;
typedef struct Cyc_Position_Segment* Cyc_Position_seg_t; typedef void* Cyc_Position_Error_kind;
static const unsigned int Cyc_Position_Lex= 0; static const unsigned int Cyc_Position_Parse=
1; static const unsigned int Cyc_Position_Elab= 2; typedef void* Cyc_Position_error_kind_t;
struct Cyc_Position_Error{ struct _tagged_string source; struct Cyc_Position_Segment*
seg; void* kind; struct _tagged_string desc; } ; typedef struct Cyc_Position_Error*
Cyc_Position_error_t; extern struct Cyc_Position_Error* Cyc_Position_mk_err_elab(
struct Cyc_Position_Segment*, struct _tagged_string); extern char Cyc_Position_Nocontext_tag[
10u]; struct Cyc_Position_Nocontext_struct{ char* tag; } ; extern void Cyc_Position_post_error(
struct Cyc_Position_Error*); typedef struct _tagged_string* Cyc_Absyn_field_name_t;
typedef struct _tagged_string* Cyc_Absyn_var_t; typedef struct _tagged_string*
Cyc_Absyn_tvarname_t; typedef void* Cyc_Absyn_Nmspace; typedef void* Cyc_Absyn_nmspace_t;
typedef struct _tuple0* Cyc_Absyn_qvar_t; typedef struct _tuple0* Cyc_Absyn_qvar_opt_t;
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
char* tag; } ; extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct
Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref*
x); struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE;
typedef int Cyc_Stdio_fpos_t; extern char Cyc_Stdio_FileOpenError_tag[ 14u];
struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Stdio_FileCloseError_tag[ 15u]; struct Cyc_Stdio_FileCloseError_struct{
char* tag; } ; struct Cyc_PP_Ppstate; typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t;
struct Cyc_PP_Out; typedef struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc;
typedef struct Cyc_PP_Doc* Cyc_PP_doc_t; extern struct _tagged_string Cyc_Absynpp_typ2string(
void*); struct Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef
struct Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t;
extern char Cyc_Set_Absent_tag[ 7u]; struct Cyc_Set_Absent_struct{ char* tag; }
; struct Cyc_Dict_Dict; typedef struct Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef
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
tag; } ; extern void* Cyc_Tcutil_impos( struct _tagged_string); extern void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*, struct _tagged_string); extern void* Cyc_Tcutil_compress(
void* t); extern unsigned int Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp*
e); extern unsigned int Cyc_Evexp_szof( void* t, struct Cyc_Position_Segment*
loc); void Cyc_Evexp_exp_err( struct Cyc_Position_Segment* loc, struct
_tagged_string msg){ Cyc_Position_post_error( Cyc_Position_mk_err_elab( loc,({
struct _tagged_string _temp0= msg; xprintf("Exp_err: %.*s", _temp0.last_plus_one
- _temp0.curr, _temp0.curr);})));} extern void* Cyc_Evexp_promote_const( void*
cn); extern void* Cyc_Evexp_eval_const_exp( struct Cyc_Absyn_Exp* e); extern
void* Cyc_Evexp_eval_const_unprimop( void* p, struct Cyc_Absyn_Exp* e); extern
void* Cyc_Evexp_eval_const_binprimop( void* p, struct Cyc_Absyn_Exp* e1, struct
Cyc_Absyn_Exp* e2); extern void* Cyc_Evexp_eval_sizeof( void* t, struct Cyc_Position_Segment*
loc); unsigned int Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp* e){ void*
_temp1= Cyc_Evexp_promote_const( Cyc_Evexp_eval_const_exp( e)); int _temp7; void*
_temp9; _LL3: if(( unsigned int) _temp1 > 1u?(( struct _tunion_struct*) _temp1)->tag
== Cyc_Absyn_Int_c_tag: 0){ _LL10: _temp9=( void*)(( struct Cyc_Absyn_Int_c_struct*)
_temp1)->f1; goto _LL8; _LL8: _temp7=( int)(( struct Cyc_Absyn_Int_c_struct*)
_temp1)->f2; goto _LL4;} else{ goto _LL5;} _LL5: goto _LL6; _LL4: return(
unsigned int) _temp7; _LL6: Cyc_Evexp_exp_err( e->loc,( struct _tagged_string)({
char* _temp11=( char*)"expecting unsigned int"; struct _tagged_string _temp12;
_temp12.curr= _temp11; _temp12.base= _temp11; _temp12.last_plus_one= _temp11 +
23; _temp12;})); return 0u; _LL2:;} void* Cyc_Evexp_promote_const( void* cn){
void* _temp13= cn; char _temp21; void* _temp23; short _temp25; void* _temp27;
_LL15: if(( unsigned int) _temp13 > 1u?(( struct _tunion_struct*) _temp13)->tag
== Cyc_Absyn_Char_c_tag: 0){ _LL24: _temp23=( void*)(( struct Cyc_Absyn_Char_c_struct*)
_temp13)->f1; goto _LL22; _LL22: _temp21=( char)(( struct Cyc_Absyn_Char_c_struct*)
_temp13)->f2; goto _LL16;} else{ goto _LL17;} _LL17: if(( unsigned int) _temp13
> 1u?(( struct _tunion_struct*) _temp13)->tag == Cyc_Absyn_Short_c_tag: 0){
_LL28: _temp27=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp13)->f1; goto
_LL26; _LL26: _temp25=( short)(( struct Cyc_Absyn_Short_c_struct*) _temp13)->f2;
goto _LL18;} else{ goto _LL19;} _LL19: goto _LL20; _LL16: return( void*)({
struct Cyc_Absyn_Int_c_struct* _temp29=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp29[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp30; _temp30.tag= Cyc_Absyn_Int_c_tag; _temp30.f1=( void*) _temp23; _temp30.f2=(
int) _temp21; _temp30;}); _temp29;}); _LL18: return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp31=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp31[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp32; _temp32.tag= Cyc_Absyn_Int_c_tag;
_temp32.f1=( void*) _temp27; _temp32.f2=( int) _temp25; _temp32;}); _temp31;});
_LL20: return cn; _LL14:;} int Cyc_Evexp_eval_const_bool_exp( struct Cyc_Absyn_Exp*
e){ void* _temp33= Cyc_Evexp_promote_const( Cyc_Evexp_eval_const_exp( e)); int
_temp43; void* _temp45; long long _temp47; void* _temp49; _LL35: if((
unsigned int) _temp33 > 1u?(( struct _tunion_struct*) _temp33)->tag == Cyc_Absyn_Int_c_tag:
0){ _LL46: _temp45=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp33)->f1; goto
_LL44; _LL44: _temp43=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp33)->f2;
goto _LL36;} else{ goto _LL37;} _LL37: if(( unsigned int) _temp33 > 1u?(( struct
_tunion_struct*) _temp33)->tag == Cyc_Absyn_LongLong_c_tag: 0){ _LL50: _temp49=(
void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp33)->f1; goto _LL48; _LL48:
_temp47=( long long)(( struct Cyc_Absyn_LongLong_c_struct*) _temp33)->f2; goto
_LL38;} else{ goto _LL39;} _LL39: if(( int) _temp33 == Cyc_Absyn_Null_c){ goto
_LL40;} else{ goto _LL41;} _LL41: goto _LL42; _LL36: return _temp43 != 0; _LL38:
return _temp47 !=( long long) 0; _LL40: return 0; _LL42: Cyc_Evexp_exp_err( e->loc,(
struct _tagged_string)({ char* _temp51=( char*)"expecting bool"; struct
_tagged_string _temp52; _temp52.curr= _temp51; _temp52.base= _temp51; _temp52.last_plus_one=
_temp51 + 15; _temp52;})); return 0; _LL34:;} void* Cyc_Evexp_eval_const_exp(
struct Cyc_Absyn_Exp* e){ void* cn;{ void* _temp53=( void*) e->r; void* _temp71;
struct Cyc_Absyn_Exp* _temp73; struct Cyc_Absyn_Exp* _temp75; struct Cyc_Absyn_Exp*
_temp77; struct Cyc_List_List* _temp79; void* _temp81; void* _temp83; struct Cyc_Absyn_Exp*
_temp85; struct Cyc_Absyn_Exp* _temp87; void* _temp89; struct Cyc_Absyn_Enumfield*
_temp91; struct Cyc_Absyn_Enumdecl* _temp93; struct _tuple0* _temp95; _LL55: if(((
struct _tunion_struct*) _temp53)->tag == Cyc_Absyn_Const_e_tag){ _LL72: _temp71=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp53)->f1; goto _LL56;} else{ goto
_LL57;} _LL57: if((( struct _tunion_struct*) _temp53)->tag == Cyc_Absyn_Conditional_e_tag){
_LL78: _temp77=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp53)->f1; goto _LL76; _LL76: _temp75=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp53)->f2; goto _LL74; _LL74: _temp73=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp53)->f3; goto _LL58;} else{ goto _LL59;} _LL59: if((( struct _tunion_struct*)
_temp53)->tag == Cyc_Absyn_Primop_e_tag){ _LL82: _temp81=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp53)->f1; goto _LL80; _LL80: _temp79=( struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp53)->f2; goto _LL60;} else{ goto _LL61;} _LL61: if((( struct _tunion_struct*)
_temp53)->tag == Cyc_Absyn_Sizeoftyp_e_tag){ _LL84: _temp83=( void*)(( struct
Cyc_Absyn_Sizeoftyp_e_struct*) _temp53)->f1; goto _LL62;} else{ goto _LL63;}
_LL63: if((( struct _tunion_struct*) _temp53)->tag == Cyc_Absyn_Sizeofexp_e_tag){
_LL86: _temp85=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp53)->f1; goto _LL64;} else{ goto _LL65;} _LL65: if((( struct _tunion_struct*)
_temp53)->tag == Cyc_Absyn_Cast_e_tag){ _LL90: _temp89=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp53)->f1; goto _LL88; _LL88: _temp87=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp53)->f2; goto _LL66;} else{ goto _LL67;} _LL67: if((( struct _tunion_struct*)
_temp53)->tag == Cyc_Absyn_Enum_e_tag){ _LL96: _temp95=( struct _tuple0*)((
struct Cyc_Absyn_Enum_e_struct*) _temp53)->f1; goto _LL94; _LL94: _temp93=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*) _temp53)->f2;
goto _LL92; _LL92: _temp91=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp53)->f3; goto _LL68;} else{ goto _LL69;} _LL69: goto _LL70; _LL56: return
_temp71; _LL58: cn= Cyc_Evexp_eval_const_bool_exp( _temp77)? Cyc_Evexp_eval_const_exp(
_temp75): Cyc_Evexp_eval_const_exp( _temp73); goto _LL54; _LL60: if( _temp79 ==
0){ Cyc_Evexp_exp_err( e->loc,( struct _tagged_string)({ char* _temp97=( char*)"bad static expression (no args to primop)";
struct _tagged_string _temp98; _temp98.curr= _temp97; _temp98.base= _temp97;
_temp98.last_plus_one= _temp97 + 42; _temp98;})); return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp99=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp99[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp100; _temp100.tag= Cyc_Absyn_Int_c_tag;
_temp100.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp100.f2= 0; _temp100;});
_temp99;});} if( _temp79->tl == 0){ cn= Cyc_Evexp_eval_const_unprimop( _temp81,(
struct Cyc_Absyn_Exp*) _temp79->hd);} else{ if(( _temp79->tl)->tl != 0){ Cyc_Evexp_exp_err(
e->loc,( struct _tagged_string)({ char* _temp101=( char*)"bad static expression (too many args to primop)";
struct _tagged_string _temp102; _temp102.curr= _temp101; _temp102.base= _temp101;
_temp102.last_plus_one= _temp101 + 48; _temp102;})); return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp103=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp103[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp104; _temp104.tag= Cyc_Absyn_Int_c_tag;
_temp104.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp104.f2= 0; _temp104;});
_temp103;});} else{ cn= Cyc_Evexp_eval_const_binprimop( _temp81,( struct Cyc_Absyn_Exp*)
_temp79->hd,( struct Cyc_Absyn_Exp*)( _temp79->tl)->hd);}} goto _LL54; _LL62: cn=
Cyc_Evexp_eval_sizeof( _temp83, e->loc); goto _LL54; _LL64: cn= Cyc_Evexp_eval_sizeof((
void*)( _temp85->topt)->v, _temp85->loc); goto _LL54; _LL66: cn= Cyc_Evexp_eval_const_exp(
_temp87);{ struct _tuple3 _temp106=({ struct _tuple3 _temp105; _temp105.f1=
_temp89; _temp105.f2= cn; _temp105;}); void* _temp112; int _temp114; void*
_temp116; void* _temp118; void* _temp120; void* _temp122; _LL108: _LL119:
_temp118= _temp106.f1; if(( unsigned int) _temp118 > 4u?(( struct _tunion_struct*)
_temp118)->tag == Cyc_Absyn_IntType_tag: 0){ _LL123: _temp122=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp118)->f1; goto _LL121; _LL121: _temp120=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp118)->f2; goto _LL113;} else{ goto _LL110;}
_LL113: _temp112= _temp106.f2; if(( unsigned int) _temp112 > 1u?(( struct
_tunion_struct*) _temp112)->tag == Cyc_Absyn_Int_c_tag: 0){ _LL117: _temp116=(
void*)(( struct Cyc_Absyn_Int_c_struct*) _temp112)->f1; goto _LL115; _LL115:
_temp114=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp112)->f2; goto _LL109;}
else{ goto _LL110;} _LL110: goto _LL111; _LL109: if( _temp122 != _temp116){ cn=(
void*)({ struct Cyc_Absyn_Int_c_struct* _temp124=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp124[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp125; _temp125.tag= Cyc_Absyn_Int_c_tag; _temp125.f1=( void*) _temp122;
_temp125.f2= _temp114; _temp125;}); _temp124;});} goto _LL107; _LL111: Cyc_Evexp_exp_err(
e->loc,({ struct _tagged_string _temp126= Cyc_Absynpp_typ2string( _temp89);
xprintf("eval_const: cannot cast to %.*s", _temp126.last_plus_one - _temp126.curr,
_temp126.curr);})); return( void*)({ struct Cyc_Absyn_Int_c_struct* _temp127=(
struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp127[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp128; _temp128.tag= Cyc_Absyn_Int_c_tag;
_temp128.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp128.f2= 0; _temp128;});
_temp127;}); _LL107:;} goto _LL54; _LL68: return Cyc_Evexp_eval_const_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp129= _temp91->tag; if(
_temp129 == 0){ _throw( Null_Exception);} _temp129;})); _LL70: Cyc_Evexp_exp_err(
e->loc,( struct _tagged_string)({ char* _temp130=( char*)"bad static expression";
struct _tagged_string _temp131; _temp131.curr= _temp130; _temp131.base= _temp130;
_temp131.last_plus_one= _temp130 + 22; _temp131;})); return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp132=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp132[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp133; _temp133.tag= Cyc_Absyn_Int_c_tag;
_temp133.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp133.f2= 0; _temp133;});
_temp132;}); _LL54:;}( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp134=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp134[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp135; _temp135.tag= Cyc_Absyn_Const_e_tag;
_temp135.f1=( void*) cn; _temp135;}); _temp134;}))); return cn;} unsigned int
Cyc_Evexp_szof( void* t, struct Cyc_Position_Segment* loc){ void* _temp136= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_Tvar* _temp190; int _temp192; struct Cyc_Core_Opt* _temp194;
void* _temp196; int _temp198; struct Cyc_Core_Opt* _temp200; void* _temp202;
struct Cyc_Absyn_TunionInfo _temp204; struct Cyc_Absyn_XTunionInfo _temp206;
struct Cyc_Absyn_TunionFieldInfo _temp208; struct Cyc_Absyn_Tunionfield*
_temp210; struct Cyc_Absyn_Tuniondecl* _temp212; struct _tuple0* _temp214;
struct Cyc_List_List* _temp216; struct _tuple0* _temp218; struct Cyc_Absyn_XTunionFieldInfo
_temp220; struct Cyc_Absyn_Tunionfield* _temp222; struct Cyc_Absyn_XTuniondecl*
_temp224; struct _tuple0* _temp226; struct _tuple0* _temp228; struct Cyc_Absyn_PtrInfo
_temp230; struct Cyc_Absyn_Conref* _temp232; struct Cyc_Absyn_Tqual* _temp234;
struct Cyc_Absyn_Conref* _temp236; void* _temp238; void* _temp240; void*
_temp242; void* _temp244; void* _temp246; void* _temp248; void* _temp250; void*
_temp252; void* _temp254; void* _temp256; struct Cyc_Absyn_Exp* _temp258; struct
Cyc_Absyn_Tqual* _temp260; void* _temp262; struct Cyc_Absyn_FnInfo _temp264;
struct Cyc_List_List* _temp266; void* _temp268; struct Cyc_Absyn_Structdecl**
_temp270; struct Cyc_List_List* _temp272; struct _tuple0* _temp274; struct Cyc_Absyn_Uniondecl**
_temp276; struct Cyc_List_List* _temp278; struct _tuple0* _temp280; struct Cyc_Absyn_Enumdecl*
_temp282; struct _tuple0* _temp284; struct Cyc_Core_Opt* _temp286; struct Cyc_List_List*
_temp288; struct _tuple0* _temp290; struct Cyc_List_List* _temp292; void*
_temp294; _LL138: if(( int) _temp136 == Cyc_Absyn_VoidType){ goto _LL139;} else{
goto _LL140;} _LL140: if(( unsigned int) _temp136 > 4u?(( struct _tunion_struct*)
_temp136)->tag == Cyc_Absyn_VarType_tag: 0){ _LL191: _temp190=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_VarType_struct*) _temp136)->f1; goto _LL141;} else{ goto _LL142;}
_LL142: if(( unsigned int) _temp136 > 4u?(( struct _tunion_struct*) _temp136)->tag
== Cyc_Absyn_Evar_tag: 0){ _LL197: _temp196=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp136)->f1; if(( int) _temp196 == Cyc_Absyn_BoxKind){ goto _LL195;} else{
goto _LL144;} _LL195: _temp194=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp136)->f2; goto _LL193; _LL193: _temp192=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp136)->f3; goto _LL143;} else{ goto _LL144;} _LL144: if(( unsigned int)
_temp136 > 4u?(( struct _tunion_struct*) _temp136)->tag == Cyc_Absyn_Evar_tag: 0){
_LL203: _temp202=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp136)->f1; goto
_LL201; _LL201: _temp200=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp136)->f2; goto _LL199; _LL199: _temp198=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp136)->f3; goto _LL145;} else{ goto _LL146;} _LL146: if(( unsigned int)
_temp136 > 4u?(( struct _tunion_struct*) _temp136)->tag == Cyc_Absyn_TunionType_tag:
0){ _LL205: _temp204=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp136)->f1; goto _LL147;} else{ goto _LL148;} _LL148: if(( unsigned int)
_temp136 > 4u?(( struct _tunion_struct*) _temp136)->tag == Cyc_Absyn_XTunionType_tag:
0){ _LL207: _temp206=( struct Cyc_Absyn_XTunionInfo)(( struct Cyc_Absyn_XTunionType_struct*)
_temp136)->f1; goto _LL149;} else{ goto _LL150;} _LL150: if(( unsigned int)
_temp136 > 4u?(( struct _tunion_struct*) _temp136)->tag == Cyc_Absyn_TunionFieldType_tag:
0){ _LL209: _temp208=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp136)->f1; _LL219: _temp218=( struct _tuple0*) _temp208.name; goto _LL217;
_LL217: _temp216=( struct Cyc_List_List*) _temp208.targs; goto _LL215; _LL215:
_temp214=( struct _tuple0*) _temp208.fname; goto _LL213; _LL213: _temp212=(
struct Cyc_Absyn_Tuniondecl*) _temp208.tud; goto _LL211; _LL211: _temp210=(
struct Cyc_Absyn_Tunionfield*) _temp208.tufd; goto _LL151;} else{ goto _LL152;}
_LL152: if(( unsigned int) _temp136 > 4u?(( struct _tunion_struct*) _temp136)->tag
== Cyc_Absyn_XTunionFieldType_tag: 0){ _LL221: _temp220=( struct Cyc_Absyn_XTunionFieldInfo)((
struct Cyc_Absyn_XTunionFieldType_struct*) _temp136)->f1; _LL229: _temp228=(
struct _tuple0*) _temp220.name; goto _LL227; _LL227: _temp226=( struct _tuple0*)
_temp220.fname; goto _LL225; _LL225: _temp224=( struct Cyc_Absyn_XTuniondecl*)
_temp220.xtud; goto _LL223; _LL223: _temp222=( struct Cyc_Absyn_Tunionfield*)
_temp220.xtufd; goto _LL153;} else{ goto _LL154;} _LL154: if(( unsigned int)
_temp136 > 4u?(( struct _tunion_struct*) _temp136)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL231: _temp230=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp136)->f1; _LL241: _temp240=( void*) _temp230.elt_typ; goto _LL239; _LL239:
_temp238=( void*) _temp230.rgn_typ; goto _LL237; _LL237: _temp236=( struct Cyc_Absyn_Conref*)
_temp230.nullable; goto _LL235; _LL235: _temp234=( struct Cyc_Absyn_Tqual*)
_temp230.tq; goto _LL233; _LL233: _temp232=( struct Cyc_Absyn_Conref*) _temp230.bounds;
goto _LL155;} else{ goto _LL156;} _LL156: if(( unsigned int) _temp136 > 4u?((
struct _tunion_struct*) _temp136)->tag == Cyc_Absyn_IntType_tag: 0){ _LL245:
_temp244=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp136)->f1; goto _LL243;
_LL243: _temp242=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp136)->f2; if((
int) _temp242 == Cyc_Absyn_B1){ goto _LL157;} else{ goto _LL158;}} else{ goto
_LL158;} _LL158: if(( unsigned int) _temp136 > 4u?(( struct _tunion_struct*)
_temp136)->tag == Cyc_Absyn_IntType_tag: 0){ _LL249: _temp248=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp136)->f1; goto _LL247; _LL247: _temp246=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp136)->f2; if(( int) _temp246 == Cyc_Absyn_B2){
goto _LL159;} else{ goto _LL160;}} else{ goto _LL160;} _LL160: if(( unsigned int)
_temp136 > 4u?(( struct _tunion_struct*) _temp136)->tag == Cyc_Absyn_IntType_tag:
0){ _LL253: _temp252=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp136)->f1;
goto _LL251; _LL251: _temp250=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp136)->f2; if(( int) _temp250 == Cyc_Absyn_B4){ goto _LL161;} else{ goto
_LL162;}} else{ goto _LL162;} _LL162: if(( unsigned int) _temp136 > 4u?(( struct
_tunion_struct*) _temp136)->tag == Cyc_Absyn_IntType_tag: 0){ _LL257: _temp256=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp136)->f1; goto _LL255; _LL255:
_temp254=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp136)->f2; if(( int)
_temp254 == Cyc_Absyn_B8){ goto _LL163;} else{ goto _LL164;}} else{ goto _LL164;}
_LL164: if(( int) _temp136 == Cyc_Absyn_FloatType){ goto _LL165;} else{ goto
_LL166;} _LL166: if(( int) _temp136 == Cyc_Absyn_DoubleType){ goto _LL167;}
else{ goto _LL168;} _LL168: if(( unsigned int) _temp136 > 4u?(( struct
_tunion_struct*) _temp136)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL263: _temp262=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp136)->f1; goto _LL261; _LL261:
_temp260=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp136)->f2; goto _LL259; _LL259: _temp258=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp136)->f3; goto _LL169;} else{ goto _LL170;}
_LL170: if(( unsigned int) _temp136 > 4u?(( struct _tunion_struct*) _temp136)->tag
== Cyc_Absyn_FnType_tag: 0){ _LL265: _temp264=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp136)->f1; goto _LL171;} else{ goto _LL172;}
_LL172: if(( unsigned int) _temp136 > 4u?(( struct _tunion_struct*) _temp136)->tag
== Cyc_Absyn_TupleType_tag: 0){ _LL267: _temp266=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp136)->f1; goto _LL173;} else{ goto
_LL174;} _LL174: if(( unsigned int) _temp136 > 4u?(( struct _tunion_struct*)
_temp136)->tag == Cyc_Absyn_RgnHandleType_tag: 0){ _LL269: _temp268=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp136)->f1; goto _LL175;} else{ goto
_LL176;} _LL176: if(( unsigned int) _temp136 > 4u?(( struct _tunion_struct*)
_temp136)->tag == Cyc_Absyn_StructType_tag: 0){ _LL275: _temp274=( struct
_tuple0*)(( struct Cyc_Absyn_StructType_struct*) _temp136)->f1; goto _LL273;
_LL273: _temp272=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp136)->f2; goto _LL271; _LL271: _temp270=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp136)->f3; goto _LL177;} else{ goto
_LL178;} _LL178: if(( unsigned int) _temp136 > 4u?(( struct _tunion_struct*)
_temp136)->tag == Cyc_Absyn_UnionType_tag: 0){ _LL281: _temp280=( struct _tuple0*)((
struct Cyc_Absyn_UnionType_struct*) _temp136)->f1; goto _LL279; _LL279: _temp278=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*) _temp136)->f2; goto
_LL277; _LL277: _temp276=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp136)->f3; goto _LL179;} else{ goto _LL180;} _LL180: if(( unsigned int)
_temp136 > 4u?(( struct _tunion_struct*) _temp136)->tag == Cyc_Absyn_EnumType_tag:
0){ _LL285: _temp284=( struct _tuple0*)(( struct Cyc_Absyn_EnumType_struct*)
_temp136)->f1; goto _LL283; _LL283: _temp282=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_EnumType_struct*) _temp136)->f2; goto _LL181;} else{ goto
_LL182;} _LL182: if(( int) _temp136 == Cyc_Absyn_HeapRgn){ goto _LL183;} else{
goto _LL184;} _LL184: if(( unsigned int) _temp136 > 4u?(( struct _tunion_struct*)
_temp136)->tag == Cyc_Absyn_TypedefType_tag: 0){ _LL291: _temp290=( struct
_tuple0*)(( struct Cyc_Absyn_TypedefType_struct*) _temp136)->f1; goto _LL289;
_LL289: _temp288=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp136)->f2; goto _LL287; _LL287: _temp286=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_TypedefType_struct*) _temp136)->f3; goto _LL185;} else{ goto _LL186;}
_LL186: if(( unsigned int) _temp136 > 4u?(( struct _tunion_struct*) _temp136)->tag
== Cyc_Absyn_JoinEff_tag: 0){ _LL293: _temp292=( struct Cyc_List_List*)(( struct
Cyc_Absyn_JoinEff_struct*) _temp136)->f1; goto _LL187;} else{ goto _LL188;}
_LL188: if(( unsigned int) _temp136 > 4u?(( struct _tunion_struct*) _temp136)->tag
== Cyc_Absyn_AccessEff_tag: 0){ _LL295: _temp294=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp136)->f1; goto _LL189;} else{ goto _LL137;} _LL139: Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp296=( char*)"cannot apply sizeof to void type";
struct _tagged_string _temp297; _temp297.curr= _temp296; _temp297.base= _temp296;
_temp297.last_plus_one= _temp296 + 33; _temp297;})); return 0u; _LL141: { void*
_temp298=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp190->kind); _LL300: if(( int) _temp298 == Cyc_Absyn_BoxKind){ goto _LL301;}
else{ goto _LL302;} _LL302: if(( int) _temp298 == Cyc_Absyn_AnyKind){ goto
_LL303;} else{ goto _LL304;} _LL304: if(( int) _temp298 == Cyc_Absyn_MemKind){
goto _LL305;} else{ goto _LL306;} _LL306: if(( int) _temp298 == Cyc_Absyn_RgnKind){
goto _LL307;} else{ goto _LL308;} _LL308: if(( int) _temp298 == Cyc_Absyn_EffKind){
goto _LL309;} else{ goto _LL299;} _LL301: return 4u; _LL303: goto _LL305; _LL305:
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp310=( char*)"cannot apply sizeof to a non-boxed type variable";
struct _tagged_string _temp311; _temp311.curr= _temp310; _temp311.base= _temp310;
_temp311.last_plus_one= _temp310 + 49; _temp311;})); return 0u; _LL307: Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp312=( char*)"cannot apply sizeof to a region";
struct _tagged_string _temp313; _temp313.curr= _temp312; _temp313.base= _temp312;
_temp313.last_plus_one= _temp312 + 32; _temp313;})); return 0u; _LL309: Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp314=( char*)"cannot apply sizeof to an effect";
struct _tagged_string _temp315; _temp315.curr= _temp314; _temp315.base= _temp314;
_temp315.last_plus_one= _temp314 + 33; _temp315;})); return 0u; _LL299:;} _LL143:
return 4u; _LL145: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp316=( char*)"cannot apply sizeof to unknown type"; struct _tagged_string
_temp317; _temp317.curr= _temp316; _temp317.base= _temp316; _temp317.last_plus_one=
_temp316 + 36; _temp317;})); return 0u; _LL147: return 4u; _LL149: return 4u;
_LL151: _temp222= _temp210; goto _LL153; _LL153: return 4u + Cyc_Evexp_szof((
void*)({ struct Cyc_Absyn_TupleType_struct* _temp318=( struct Cyc_Absyn_TupleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp318[ 0]=({ struct
Cyc_Absyn_TupleType_struct _temp319; _temp319.tag= Cyc_Absyn_TupleType_tag;
_temp319.f1= _temp222->typs; _temp319;}); _temp318;}), loc); _LL155: { void*
_temp320=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)( _temp232))->v; void* _temp328; void* _temp330;
_LL322: if(( unsigned int) _temp320 > 1u?(( struct _tunion_struct*) _temp320)->tag
== Cyc_Absyn_Eq_constr_tag: 0){ _LL329: _temp328=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp320)->f1; if(( int) _temp328 == Cyc_Absyn_Unknown_b){ goto _LL323;} else{
goto _LL324;}} else{ goto _LL324;} _LL324: if(( unsigned int) _temp320 > 1u?((
struct _tunion_struct*) _temp320)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL331:
_temp330=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp320)->f1; goto
_LL325;} else{ goto _LL326;} _LL326: goto _LL327; _LL323: return 12u; _LL325:
return 4u; _LL327: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp332=( char*)"bounds information not known for pointer type"; struct
_tagged_string _temp333; _temp333.curr= _temp332; _temp333.base= _temp332;
_temp333.last_plus_one= _temp332 + 46; _temp333;})); return 0u; _LL321:;} _LL157:
return 1u; _LL159: return 1u; _LL161: return 1u; _LL163: return 1u; _LL165:
return 4u; _LL167: return 8u; _LL169: if( _temp258 == 0){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp334=( char*)"cannot apply sizeof to an array of unknown size";
struct _tagged_string _temp335; _temp335.curr= _temp334; _temp335.base= _temp334;
_temp335.last_plus_one= _temp334 + 48; _temp335;}));} return Cyc_Evexp_szof(
_temp262, loc) * Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)({ struct
Cyc_Absyn_Exp* _temp336= _temp258; if( _temp336 == 0){ _throw( Null_Exception);}
_temp336;})); _LL171: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp337=( char*)"cannot apply sizeof to function type"; struct _tagged_string
_temp338; _temp338.curr= _temp337; _temp338.base= _temp337; _temp338.last_plus_one=
_temp337 + 37; _temp338;})); return 0u; _LL173: { unsigned int s= 0; for( 0;
_temp266 != 0; _temp266= _temp266->tl){ s += Cyc_Evexp_szof((*(( struct _tuple4*)
_temp266->hd)).f2, loc);} return s;} _LL175: return 4u; _LL177: if( _temp270 ==
0){ return(( unsigned int(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ char* _temp339=( char*)"szof on unchecked StructType"; struct
_tagged_string _temp340; _temp340.curr= _temp339; _temp340.base= _temp339;
_temp340.last_plus_one= _temp339 + 29; _temp340;}));}{ struct Cyc_Absyn_Structdecl*
sd=* _temp270; if( sd->fields == 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp341=( char*)"cannot apply sizeof to abstract struct type";
struct _tagged_string _temp342; _temp342.curr= _temp341; _temp342.base= _temp341;
_temp342.last_plus_one= _temp341 + 44; _temp342;}));}{ unsigned int s= 0;{
struct Cyc_List_List* fs=( struct Cyc_List_List*)( sd->fields)->v; for( 0; fs !=
0; fs= fs->tl){ s += Cyc_Evexp_szof(( void*)(( struct Cyc_Absyn_Structfield*) fs->hd)->type,
loc);}} return s;}} _LL179: if( _temp276 == 0){ return(( unsigned int(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp343=(
char*)"szof on unchecked UnionType"; struct _tagged_string _temp344; _temp344.curr=
_temp343; _temp344.base= _temp343; _temp344.last_plus_one= _temp343 + 28;
_temp344;}));}{ struct Cyc_Absyn_Uniondecl* ud=* _temp276; if( ud->fields == 0){
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp345=( char*)"cannot apply sizeof to abstract union type";
struct _tagged_string _temp346; _temp346.curr= _temp345; _temp346.base= _temp345;
_temp346.last_plus_one= _temp345 + 43; _temp346;}));}{ unsigned int s= 0;{
struct Cyc_List_List* fs=( struct Cyc_List_List*)( ud->fields)->v; for( 0; fs !=
0; fs= fs->tl){ unsigned int i= Cyc_Evexp_szof(( void*)(( struct Cyc_Absyn_Structfield*)
fs->hd)->type, loc); s= s > i? s: i;}} return s;}} _LL181: return 4u; _LL183:
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp347=( char*)"cannot apply sizoef to a region";
struct _tagged_string _temp348; _temp348.curr= _temp347; _temp348.base= _temp347;
_temp348.last_plus_one= _temp347 + 32; _temp348;})); return 0u; _LL185: return((
unsigned int(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({ struct
_tagged_string _temp349= Cyc_Absynpp_typ2string( t); xprintf("szof typedeftype %.*s",
_temp349.last_plus_one - _temp349.curr, _temp349.curr);})); _LL187: goto _LL189;
_LL189: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp350=( char*)"cannot apply sizoef to an effect";
struct _tagged_string _temp351; _temp351.curr= _temp350; _temp351.base= _temp350;
_temp351.last_plus_one= _temp350 + 33; _temp351;})); return 0u; _LL137:;} void*
Cyc_Evexp_eval_sizeof( void* t, struct Cyc_Position_Segment* loc){ return( void*)({
struct Cyc_Absyn_Int_c_struct* _temp352=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp352[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp353; _temp353.tag= Cyc_Absyn_Int_c_tag; _temp353.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp353.f2=( int) Cyc_Evexp_szof( t, loc); _temp353;}); _temp352;});} int Cyc_Evexp_is_arith_const(
void* cn){ void* _temp354= cn; char _temp364; void* _temp366; int _temp368; void*
_temp370; struct _tagged_string _temp372; _LL356: if(( unsigned int) _temp354 >
1u?(( struct _tunion_struct*) _temp354)->tag == Cyc_Absyn_Char_c_tag: 0){ _LL367:
_temp366=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp354)->f1; goto _LL365;
_LL365: _temp364=( char)(( struct Cyc_Absyn_Char_c_struct*) _temp354)->f2; goto
_LL357;} else{ goto _LL358;} _LL358: if(( unsigned int) _temp354 > 1u?(( struct
_tunion_struct*) _temp354)->tag == Cyc_Absyn_Int_c_tag: 0){ _LL371: _temp370=(
void*)(( struct Cyc_Absyn_Int_c_struct*) _temp354)->f1; goto _LL369; _LL369:
_temp368=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp354)->f2; goto _LL359;}
else{ goto _LL360;} _LL360: if(( unsigned int) _temp354 > 1u?(( struct
_tunion_struct*) _temp354)->tag == Cyc_Absyn_Float_c_tag: 0){ _LL373: _temp372=(
struct _tagged_string)(( struct Cyc_Absyn_Float_c_struct*) _temp354)->f1; goto
_LL361;} else{ goto _LL362;} _LL362: goto _LL363; _LL357: return 1; _LL359:
return 1; _LL361: return 1; _LL363: return 0; _LL355:;} void* Cyc_Evexp_eval_const_unprimop(
void* p, struct Cyc_Absyn_Exp* e){ void* cn= Cyc_Evexp_promote_const( Cyc_Evexp_eval_const_exp(
e)); struct _tuple3 _temp375=({ struct _tuple3 _temp374; _temp374.f1= p;
_temp374.f2= cn; _temp374;}); void* _temp393; void* _temp395; void* _temp397;
int _temp399; void* _temp401; void* _temp403; void* _temp405; short _temp407;
void* _temp409; void* _temp411; void* _temp413; char _temp415; void* _temp417;
void* _temp419; void* _temp421; int _temp423; void* _temp425; void* _temp427;
void* _temp429; void* _temp431; void* _temp433; int _temp435; void* _temp437;
void* _temp439; _LL377: _LL396: _temp395= _temp375.f1; if(( int) _temp395 == Cyc_Absyn_Plus){
goto _LL394;} else{ goto _LL379;} _LL394: _temp393= _temp375.f2; goto _LL378;
_LL379: _LL404: _temp403= _temp375.f1; if(( int) _temp403 == Cyc_Absyn_Minus){
goto _LL398;} else{ goto _LL381;} _LL398: _temp397= _temp375.f2; if((
unsigned int) _temp397 > 1u?(( struct _tunion_struct*) _temp397)->tag == Cyc_Absyn_Int_c_tag:
0){ _LL402: _temp401=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp397)->f1;
goto _LL400; _LL400: _temp399=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp397)->f2;
goto _LL380;} else{ goto _LL381;} _LL381: _LL412: _temp411= _temp375.f1; if((
int) _temp411 == Cyc_Absyn_Minus){ goto _LL406;} else{ goto _LL383;} _LL406:
_temp405= _temp375.f2; if(( unsigned int) _temp405 > 1u?(( struct _tunion_struct*)
_temp405)->tag == Cyc_Absyn_Short_c_tag: 0){ _LL410: _temp409=( void*)(( struct
Cyc_Absyn_Short_c_struct*) _temp405)->f1; goto _LL408; _LL408: _temp407=( short)((
struct Cyc_Absyn_Short_c_struct*) _temp405)->f2; goto _LL382;} else{ goto _LL383;}
_LL383: _LL420: _temp419= _temp375.f1; if(( int) _temp419 == Cyc_Absyn_Minus){
goto _LL414;} else{ goto _LL385;} _LL414: _temp413= _temp375.f2; if((
unsigned int) _temp413 > 1u?(( struct _tunion_struct*) _temp413)->tag == Cyc_Absyn_Char_c_tag:
0){ _LL418: _temp417=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp413)->f1;
goto _LL416; _LL416: _temp415=( char)(( struct Cyc_Absyn_Char_c_struct*)
_temp413)->f2; goto _LL384;} else{ goto _LL385;} _LL385: _LL428: _temp427=
_temp375.f1; if(( int) _temp427 == Cyc_Absyn_Not){ goto _LL422;} else{ goto
_LL387;} _LL422: _temp421= _temp375.f2; if(( unsigned int) _temp421 > 1u?((
struct _tunion_struct*) _temp421)->tag == Cyc_Absyn_Int_c_tag: 0){ _LL426:
_temp425=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp421)->f1; goto _LL424;
_LL424: _temp423=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp421)->f2; goto
_LL386;} else{ goto _LL387;} _LL387: _LL432: _temp431= _temp375.f1; if(( int)
_temp431 == Cyc_Absyn_Not){ goto _LL430;} else{ goto _LL389;} _LL430: _temp429=
_temp375.f2; if(( int) _temp429 == Cyc_Absyn_Null_c){ goto _LL388;} else{ goto
_LL389;} _LL389: _LL440: _temp439= _temp375.f1; if(( int) _temp439 == Cyc_Absyn_Bitnot){
goto _LL434;} else{ goto _LL391;} _LL434: _temp433= _temp375.f2; if((
unsigned int) _temp433 > 1u?(( struct _tunion_struct*) _temp433)->tag == Cyc_Absyn_Int_c_tag:
0){ _LL438: _temp437=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp433)->f1;
goto _LL436; _LL436: _temp435=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp433)->f2;
goto _LL390;} else{ goto _LL391;} _LL391: goto _LL392; _LL378: if( ! Cyc_Evexp_is_arith_const(
cn)){ Cyc_Evexp_exp_err( e->loc,( struct _tagged_string)({ char* _temp441=( char*)"expecting arithmetic constant";
struct _tagged_string _temp442; _temp442.curr= _temp441; _temp442.base= _temp441;
_temp442.last_plus_one= _temp441 + 30; _temp442;}));} return cn; _LL380: return(
void*)({ struct Cyc_Absyn_Int_c_struct* _temp443=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp443[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp444; _temp444.tag= Cyc_Absyn_Int_c_tag; _temp444.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp444.f2= - _temp399; _temp444;}); _temp443;}); _LL382: return( void*)({
struct Cyc_Absyn_Short_c_struct* _temp445=( struct Cyc_Absyn_Short_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Short_c_struct)); _temp445[ 0]=({ struct Cyc_Absyn_Short_c_struct
_temp446; _temp446.tag= Cyc_Absyn_Short_c_tag; _temp446.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp446.f2= - _temp407; _temp446;}); _temp445;}); _LL384: return( void*)({
struct Cyc_Absyn_Char_c_struct* _temp447=( struct Cyc_Absyn_Char_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct)); _temp447[ 0]=({ struct Cyc_Absyn_Char_c_struct
_temp448; _temp448.tag= Cyc_Absyn_Char_c_tag; _temp448.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp448.f2= - _temp415; _temp448;}); _temp447;}); _LL386: return( void*)({
struct Cyc_Absyn_Int_c_struct* _temp449=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp449[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp450; _temp450.tag= Cyc_Absyn_Int_c_tag; _temp450.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp450.f2= _temp423 == 0? 1: 0; _temp450;}); _temp449;}); _LL388: return( void*)({
struct Cyc_Absyn_Int_c_struct* _temp451=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp451[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp452; _temp452.tag= Cyc_Absyn_Int_c_tag; _temp452.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp452.f2= 1; _temp452;}); _temp451;}); _LL390: return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp453=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp453[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp454; _temp454.tag= Cyc_Absyn_Int_c_tag;
_temp454.f1=( void*)(( void*) Cyc_Absyn_Unsigned); _temp454.f2= ~ _temp435;
_temp454;}); _temp453;}); _LL392: Cyc_Evexp_exp_err( e->loc,( struct
_tagged_string)({ char* _temp455=( char*)"bad constant expression"; struct
_tagged_string _temp456; _temp456.curr= _temp455; _temp456.base= _temp455;
_temp456.last_plus_one= _temp455 + 24; _temp456;})); return cn; _LL376:;} void*
Cyc_Evexp_eval_const_binprimop( void* p, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp*
e2){ void* cn1= Cyc_Evexp_promote_const( Cyc_Evexp_eval_const_exp( e1)); void*
cn2= Cyc_Evexp_promote_const( Cyc_Evexp_eval_const_exp( e2)); void* s1; void* s2;
int i1; int i2;{ void* _temp457= cn1; int _temp463; void* _temp465; _LL459: if((
unsigned int) _temp457 > 1u?(( struct _tunion_struct*) _temp457)->tag == Cyc_Absyn_Int_c_tag:
0){ _LL466: _temp465=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp457)->f1;
goto _LL464; _LL464: _temp463=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp457)->f2;
goto _LL460;} else{ goto _LL461;} _LL461: goto _LL462; _LL460: s1= _temp465; i1=
_temp463; goto _LL458; _LL462: Cyc_Evexp_exp_err( e1->loc,( struct
_tagged_string)({ char* _temp467=( char*)"bad constant expression"; struct
_tagged_string _temp468; _temp468.curr= _temp467; _temp468.base= _temp467;
_temp468.last_plus_one= _temp467 + 24; _temp468;})); return cn1; _LL458:;}{ void*
_temp469= cn2; int _temp475; void* _temp477; _LL471: if(( unsigned int) _temp469
> 1u?(( struct _tunion_struct*) _temp469)->tag == Cyc_Absyn_Int_c_tag: 0){
_LL478: _temp477=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp469)->f1; goto
_LL476; _LL476: _temp475=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp469)->f2;
goto _LL472;} else{ goto _LL473;} _LL473: goto _LL474; _LL472: s2= _temp477; i2=
_temp475; goto _LL470; _LL474: Cyc_Evexp_exp_err( e2->loc,( struct
_tagged_string)({ char* _temp479=( char*)"bad constant expression"; struct
_tagged_string _temp480; _temp480.curr= _temp479; _temp480.base= _temp479;
_temp480.last_plus_one= _temp479 + 24; _temp480;})); return cn1; _LL470:;}{ void*
_temp481= p; _LL483: if(( int) _temp481 == Cyc_Absyn_Div){ goto _LL484;} else{
goto _LL485;} _LL485: if(( int) _temp481 == Cyc_Absyn_Mod){ goto _LL486;} else{
goto _LL487;} _LL487: goto _LL488; _LL484: if( i2 == 0){ Cyc_Evexp_exp_err( e2->loc,(
struct _tagged_string)({ char* _temp489=( char*)"division by zero in constant expression";
struct _tagged_string _temp490; _temp490.curr= _temp489; _temp490.base= _temp489;
_temp490.last_plus_one= _temp489 + 40; _temp490;})); return cn1;} goto _LL482;
_LL486: if( i2 == 0){ Cyc_Evexp_exp_err( e2->loc,( struct _tagged_string)({ char*
_temp491=( char*)"division by zero in constant expression"; struct
_tagged_string _temp492; _temp492.curr= _temp491; _temp492.base= _temp491;
_temp492.last_plus_one= _temp491 + 40; _temp492;})); return cn1;} goto _LL482;
_LL488: goto _LL482; _LL482:;}{ int has_u_arg= s1 ==( void*) Cyc_Absyn_Unsigned?
1: s2 ==( void*) Cyc_Absyn_Unsigned; unsigned int u1=( unsigned int) i1;
unsigned int u2=( unsigned int) i2; int i3= 0; unsigned int u3= 0; int b3= 1;
int use_i3= 0; int use_u3= 0; int use_b3= 0;{ struct _tuple5 _temp494=({ struct
_tuple5 _temp493; _temp493.f1= p; _temp493.f2= has_u_arg; _temp493;}); int
_temp550; void* _temp552; int _temp554; void* _temp556; int _temp558; void*
_temp560; int _temp562; void* _temp564; int _temp566; void* _temp568; int
_temp570; void* _temp572; int _temp574; void* _temp576; int _temp578; void*
_temp580; int _temp582; void* _temp584; int _temp586; void* _temp588; int
_temp590; void* _temp592; int _temp594; void* _temp596; int _temp598; void*
_temp600; int _temp602; void* _temp604; int _temp606; void* _temp608; int
_temp610; void* _temp612; int _temp614; void* _temp616; int _temp618; void*
_temp620; int _temp622; void* _temp624; int _temp626; void* _temp628; int
_temp630; void* _temp632; int _temp634; void* _temp636; int _temp638; void*
_temp640; int _temp642; void* _temp644; int _temp646; void* _temp648; int
_temp650; void* _temp652; _LL496: _LL553: _temp552= _temp494.f1; if(( int)
_temp552 == Cyc_Absyn_Plus){ goto _LL551;} else{ goto _LL498;} _LL551: _temp550=
_temp494.f2; if( _temp550 == 0){ goto _LL497;} else{ goto _LL498;} _LL498:
_LL557: _temp556= _temp494.f1; if(( int) _temp556 == Cyc_Absyn_Times){ goto
_LL555;} else{ goto _LL500;} _LL555: _temp554= _temp494.f2; if( _temp554 == 0){
goto _LL499;} else{ goto _LL500;} _LL500: _LL561: _temp560= _temp494.f1; if((
int) _temp560 == Cyc_Absyn_Minus){ goto _LL559;} else{ goto _LL502;} _LL559:
_temp558= _temp494.f2; if( _temp558 == 0){ goto _LL501;} else{ goto _LL502;}
_LL502: _LL565: _temp564= _temp494.f1; if(( int) _temp564 == Cyc_Absyn_Div){
goto _LL563;} else{ goto _LL504;} _LL563: _temp562= _temp494.f2; if( _temp562 ==
0){ goto _LL503;} else{ goto _LL504;} _LL504: _LL569: _temp568= _temp494.f1; if((
int) _temp568 == Cyc_Absyn_Mod){ goto _LL567;} else{ goto _LL506;} _LL567:
_temp566= _temp494.f2; if( _temp566 == 0){ goto _LL505;} else{ goto _LL506;}
_LL506: _LL573: _temp572= _temp494.f1; if(( int) _temp572 == Cyc_Absyn_Plus){
goto _LL571;} else{ goto _LL508;} _LL571: _temp570= _temp494.f2; if( _temp570 ==
1){ goto _LL507;} else{ goto _LL508;} _LL508: _LL577: _temp576= _temp494.f1; if((
int) _temp576 == Cyc_Absyn_Times){ goto _LL575;} else{ goto _LL510;} _LL575:
_temp574= _temp494.f2; if( _temp574 == 1){ goto _LL509;} else{ goto _LL510;}
_LL510: _LL581: _temp580= _temp494.f1; if(( int) _temp580 == Cyc_Absyn_Minus){
goto _LL579;} else{ goto _LL512;} _LL579: _temp578= _temp494.f2; if( _temp578 ==
1){ goto _LL511;} else{ goto _LL512;} _LL512: _LL585: _temp584= _temp494.f1; if((
int) _temp584 == Cyc_Absyn_Div){ goto _LL583;} else{ goto _LL514;} _LL583:
_temp582= _temp494.f2; if( _temp582 == 1){ goto _LL513;} else{ goto _LL514;}
_LL514: _LL589: _temp588= _temp494.f1; if(( int) _temp588 == Cyc_Absyn_Mod){
goto _LL587;} else{ goto _LL516;} _LL587: _temp586= _temp494.f2; if( _temp586 ==
1){ goto _LL515;} else{ goto _LL516;} _LL516: _LL593: _temp592= _temp494.f1; if((
int) _temp592 == Cyc_Absyn_Eq){ goto _LL591;} else{ goto _LL518;} _LL591:
_temp590= _temp494.f2; goto _LL517; _LL518: _LL597: _temp596= _temp494.f1; if((
int) _temp596 == Cyc_Absyn_Neq){ goto _LL595;} else{ goto _LL520;} _LL595:
_temp594= _temp494.f2; goto _LL519; _LL520: _LL601: _temp600= _temp494.f1; if((
int) _temp600 == Cyc_Absyn_Gt){ goto _LL599;} else{ goto _LL522;} _LL599:
_temp598= _temp494.f2; if( _temp598 == 0){ goto _LL521;} else{ goto _LL522;}
_LL522: _LL605: _temp604= _temp494.f1; if(( int) _temp604 == Cyc_Absyn_Lt){ goto
_LL603;} else{ goto _LL524;} _LL603: _temp602= _temp494.f2; if( _temp602 == 0){
goto _LL523;} else{ goto _LL524;} _LL524: _LL609: _temp608= _temp494.f1; if((
int) _temp608 == Cyc_Absyn_Gte){ goto _LL607;} else{ goto _LL526;} _LL607:
_temp606= _temp494.f2; if( _temp606 == 0){ goto _LL525;} else{ goto _LL526;}
_LL526: _LL613: _temp612= _temp494.f1; if(( int) _temp612 == Cyc_Absyn_Lte){
goto _LL611;} else{ goto _LL528;} _LL611: _temp610= _temp494.f2; if( _temp610 ==
0){ goto _LL527;} else{ goto _LL528;} _LL528: _LL617: _temp616= _temp494.f1; if((
int) _temp616 == Cyc_Absyn_Gt){ goto _LL615;} else{ goto _LL530;} _LL615:
_temp614= _temp494.f2; if( _temp614 == 1){ goto _LL529;} else{ goto _LL530;}
_LL530: _LL621: _temp620= _temp494.f1; if(( int) _temp620 == Cyc_Absyn_Lt){ goto
_LL619;} else{ goto _LL532;} _LL619: _temp618= _temp494.f2; if( _temp618 == 1){
goto _LL531;} else{ goto _LL532;} _LL532: _LL625: _temp624= _temp494.f1; if((
int) _temp624 == Cyc_Absyn_Gte){ goto _LL623;} else{ goto _LL534;} _LL623:
_temp622= _temp494.f2; if( _temp622 == 1){ goto _LL533;} else{ goto _LL534;}
_LL534: _LL629: _temp628= _temp494.f1; if(( int) _temp628 == Cyc_Absyn_Lte){
goto _LL627;} else{ goto _LL536;} _LL627: _temp626= _temp494.f2; if( _temp626 ==
1){ goto _LL535;} else{ goto _LL536;} _LL536: _LL633: _temp632= _temp494.f1; if((
int) _temp632 == Cyc_Absyn_Bitand){ goto _LL631;} else{ goto _LL538;} _LL631:
_temp630= _temp494.f2; goto _LL537; _LL538: _LL637: _temp636= _temp494.f1; if((
int) _temp636 == Cyc_Absyn_Bitor){ goto _LL635;} else{ goto _LL540;} _LL635:
_temp634= _temp494.f2; goto _LL539; _LL540: _LL641: _temp640= _temp494.f1; if((
int) _temp640 == Cyc_Absyn_Bitxor){ goto _LL639;} else{ goto _LL542;} _LL639:
_temp638= _temp494.f2; goto _LL541; _LL542: _LL645: _temp644= _temp494.f1; if((
int) _temp644 == Cyc_Absyn_Bitlshift){ goto _LL643;} else{ goto _LL544;} _LL643:
_temp642= _temp494.f2; goto _LL543; _LL544: _LL649: _temp648= _temp494.f1; if((
int) _temp648 == Cyc_Absyn_Bitlrshift){ goto _LL647;} else{ goto _LL546;} _LL647:
_temp646= _temp494.f2; goto _LL545; _LL546: _LL653: _temp652= _temp494.f1; if((
int) _temp652 == Cyc_Absyn_Bitarshift){ goto _LL651;} else{ goto _LL548;} _LL651:
_temp650= _temp494.f2; goto _LL547; _LL548: goto _LL549; _LL497: i3= i1 + i2;
use_i3= 1; goto _LL495; _LL499: i3= i1 * i2; use_i3= 1; goto _LL495; _LL501: i3=
i1 - i2; use_i3= 1; goto _LL495; _LL503: i3= i1 / i2; use_i3= 1; goto _LL495;
_LL505: i3= i1 % i2; use_i3= 1; goto _LL495; _LL507: u3= u1 + u2; use_u3= 1;
goto _LL495; _LL509: u3= u1 * u2; use_u3= 1; goto _LL495; _LL511: u3= u1 - u2;
use_u3= 1; goto _LL495; _LL513: u3= u1 / u2; use_u3= 1; goto _LL495; _LL515: u3=
u1 % u2; use_u3= 1; goto _LL495; _LL517: b3= i1 == i2; use_b3= 1; goto _LL495;
_LL519: b3= i1 != i2; use_b3= 1; goto _LL495; _LL521: b3= i1 > i2; use_b3= 1;
goto _LL495; _LL523: b3= i1 < i2; use_b3= 1; goto _LL495; _LL525: b3= i1 >= i2;
use_b3= 1; goto _LL495; _LL527: b3= i1 <= i2; use_b3= 1; goto _LL495; _LL529: b3=
u1 > u2; use_b3= 1; goto _LL495; _LL531: b3= u1 < u2; use_b3= 1; goto _LL495;
_LL533: b3= u1 >= u2; use_b3= 1; goto _LL495; _LL535: b3= u1 <= u2; use_b3= 1;
goto _LL495; _LL537: u3= u1 & u2; use_u3= 1; goto _LL495; _LL539: u3= u1 | u2;
use_u3= 1; goto _LL495; _LL541: u3= u1 ^ u2; use_u3= 1; goto _LL495; _LL543: u3=
u1 << u2; use_u3= 1; goto _LL495; _LL545: u3= u1 >> u2; use_u3= 1; goto _LL495;
_LL547: Cyc_Evexp_exp_err( e1->loc,( struct _tagged_string)({ char* _temp654=(
char*)">>> NOT IMPLEMENTED"; struct _tagged_string _temp655; _temp655.curr=
_temp654; _temp655.base= _temp654; _temp655.last_plus_one= _temp654 + 20;
_temp655;})); return cn1; _LL549: Cyc_Evexp_exp_err( e1->loc,( struct
_tagged_string)({ char* _temp656=( char*)"bad constant expression"; struct
_tagged_string _temp657; _temp657.curr= _temp656; _temp657.base= _temp656;
_temp657.last_plus_one= _temp656 + 24; _temp657;})); return cn1; _LL495:;} if(
use_i3){ return( void*)({ struct Cyc_Absyn_Int_c_struct* _temp658=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp658[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp659; _temp659.tag= Cyc_Absyn_Int_c_tag; _temp659.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp659.f2= i3; _temp659;}); _temp658;});} if( use_u3){ return( void*)({ struct
Cyc_Absyn_Int_c_struct* _temp660=( struct Cyc_Absyn_Int_c_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Int_c_struct)); _temp660[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp661; _temp661.tag= Cyc_Absyn_Int_c_tag; _temp661.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp661.f2=( int) u3; _temp661;}); _temp660;});} if( use_b3){ return( void*)({
struct Cyc_Absyn_Int_c_struct* _temp662=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp662[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp663; _temp663.tag= Cyc_Absyn_Int_c_tag; _temp663.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp663.f2= b3? 1: 0; _temp663;}); _temp662;});}( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Core_Unreachable_struct* _temp664=( struct Cyc_Core_Unreachable_struct*)
GC_malloc( sizeof( struct Cyc_Core_Unreachable_struct)); _temp664[ 0]=({ struct
Cyc_Core_Unreachable_struct _temp665; _temp665.tag= Cyc_Core_Unreachable_tag;
_temp665.f1=( struct _tagged_string)({ char* _temp666=( char*)"Evexp::eval_const_binop";
struct _tagged_string _temp667; _temp667.curr= _temp666; _temp667.base= _temp666;
_temp667.last_plus_one= _temp666 + 24; _temp667;}); _temp665;}); _temp664;}));}}