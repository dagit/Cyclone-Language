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
struct _tuple0{ void* f1; struct _tagged_string* f2; } ; typedef struct _tuple0*
Cyc_Absyn_qvar_t; typedef struct _tuple0* Cyc_Absyn_qvar_opt_t; typedef struct
_tuple0* Cyc_Absyn_typedef_name_t; typedef struct _tuple0* Cyc_Absyn_typedef_name_opt_t;
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
return _temp47 != 0; _LL40: return 0; _LL42: Cyc_Evexp_exp_err( e->loc,( struct
_tagged_string)({ char* _temp51=( char*)"expecting bool"; struct _tagged_string
_temp52; _temp52.curr= _temp51; _temp52.base= _temp51; _temp52.last_plus_one=
_temp51 + 15; _temp52;})); return 0; _LL34:;} struct _tuple3{ void* f1; void* f2;
} ; void* Cyc_Evexp_eval_const_exp( struct Cyc_Absyn_Exp* e){ void* cn;{ void*
_temp53=( void*) e->r; void* _temp71; struct Cyc_Absyn_Exp* _temp73; struct Cyc_Absyn_Exp*
_temp75; struct Cyc_Absyn_Exp* _temp77; struct Cyc_List_List* _temp79; void*
_temp81; void* _temp83; struct Cyc_Absyn_Exp* _temp85; struct Cyc_Absyn_Exp*
_temp87; void* _temp89; struct Cyc_Absyn_Enumfield* _temp91; struct Cyc_Absyn_Enumdecl*
_temp93; struct _tuple0* _temp95; _LL55: if((( struct _tunion_struct*) _temp53)->tag
== Cyc_Absyn_Const_e_tag){ _LL72: _temp71=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp53)->f1; goto _LL56;} else{ goto _LL57;} _LL57: if((( struct _tunion_struct*)
_temp53)->tag == Cyc_Absyn_Conditional_e_tag){ _LL78: _temp77=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp53)->f1; goto _LL76; _LL76: _temp75=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*) _temp53)->f2;
goto _LL74; _LL74: _temp73=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
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
_temp99;});} if(({ struct Cyc_List_List* _temp101= _temp79; if( _temp101 == 0){
_throw( Null_Exception);} _temp101->tl;}) == 0){ cn= Cyc_Evexp_eval_const_unprimop(
_temp81,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp102= _temp79; if(
_temp102 == 0){ _throw( Null_Exception);} _temp102->hd;}));} else{ if(({ struct
Cyc_List_List* _temp104=({ struct Cyc_List_List* _temp103= _temp79; if( _temp103
== 0){ _throw( Null_Exception);} _temp103->tl;}); if( _temp104 == 0){ _throw(
Null_Exception);} _temp104->tl;}) != 0){ Cyc_Evexp_exp_err( e->loc,( struct
_tagged_string)({ char* _temp105=( char*)"bad static expression (too many args to primop)";
struct _tagged_string _temp106; _temp106.curr= _temp105; _temp106.base= _temp105;
_temp106.last_plus_one= _temp105 + 48; _temp106;})); return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp107=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp107[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp108; _temp108.tag= Cyc_Absyn_Int_c_tag;
_temp108.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp108.f2= 0; _temp108;});
_temp107;});} else{ cn= Cyc_Evexp_eval_const_binprimop( _temp81,( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp109= _temp79; if( _temp109 == 0){ _throw(
Null_Exception);} _temp109->hd;}),( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp111=({ struct Cyc_List_List* _temp110= _temp79; if( _temp110 == 0){ _throw(
Null_Exception);} _temp110->tl;}); if( _temp111 == 0){ _throw( Null_Exception);}
_temp111->hd;}));}} goto _LL54; _LL62: cn= Cyc_Evexp_eval_sizeof( _temp83, e->loc);
goto _LL54; _LL64: cn= Cyc_Evexp_eval_sizeof(( void*)({ struct Cyc_Core_Opt*
_temp112= _temp85->topt; if( _temp112 == 0){ _throw( Null_Exception);} _temp112->v;}),
_temp85->loc); goto _LL54; _LL66: cn= Cyc_Evexp_eval_const_exp( _temp87);{
struct _tuple3 _temp114=({ struct _tuple3 _temp113; _temp113.f1= _temp89;
_temp113.f2= cn; _temp113;}); void* _temp120; int _temp122; void* _temp124; void*
_temp126; void* _temp128; void* _temp130; _LL116: _LL127: _temp126= _temp114.f1;
if(( unsigned int) _temp126 > 4u?(( struct _tunion_struct*) _temp126)->tag ==
Cyc_Absyn_IntType_tag: 0){ _LL131: _temp130=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp126)->f1; goto _LL129; _LL129: _temp128=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp126)->f2; goto _LL121;} else{ goto _LL118;} _LL121: _temp120= _temp114.f2;
if(( unsigned int) _temp120 > 1u?(( struct _tunion_struct*) _temp120)->tag ==
Cyc_Absyn_Int_c_tag: 0){ _LL125: _temp124=( void*)(( struct Cyc_Absyn_Int_c_struct*)
_temp120)->f1; goto _LL123; _LL123: _temp122=( int)(( struct Cyc_Absyn_Int_c_struct*)
_temp120)->f2; goto _LL117;} else{ goto _LL118;} _LL118: goto _LL119; _LL117:
if( _temp130 != _temp124){ cn=( void*)({ struct Cyc_Absyn_Int_c_struct* _temp132=(
struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp132[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp133; _temp133.tag= Cyc_Absyn_Int_c_tag;
_temp133.f1=( void*) _temp130; _temp133.f2= _temp122; _temp133;}); _temp132;});}
goto _LL115; _LL119: Cyc_Evexp_exp_err( e->loc,({ struct _tagged_string _temp134=
Cyc_Absynpp_typ2string( _temp89); xprintf("eval_const: cannot cast to %.*s",
_temp134.last_plus_one - _temp134.curr, _temp134.curr);})); return( void*)({
struct Cyc_Absyn_Int_c_struct* _temp135=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp135[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp136; _temp136.tag= Cyc_Absyn_Int_c_tag; _temp136.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp136.f2= 0; _temp136;}); _temp135;}); _LL115:;} goto _LL54; _LL68: return
Cyc_Evexp_eval_const_exp(( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp138=({ struct Cyc_Absyn_Enumfield* _temp137= _temp91; if( _temp137 == 0){
_throw( Null_Exception);} _temp137->tag;}); if( _temp138 == 0){ _throw(
Null_Exception);} _temp138;})); _LL70: Cyc_Evexp_exp_err( e->loc,( struct
_tagged_string)({ char* _temp139=( char*)"bad static expression"; struct
_tagged_string _temp140; _temp140.curr= _temp139; _temp140.base= _temp139;
_temp140.last_plus_one= _temp139 + 22; _temp140;})); return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp141=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp141[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp142; _temp142.tag= Cyc_Absyn_Int_c_tag;
_temp142.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp142.f2= 0; _temp142;});
_temp141;}); _LL54:;}( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp143=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp143[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp144; _temp144.tag= Cyc_Absyn_Const_e_tag;
_temp144.f1=( void*) cn; _temp144;}); _temp143;}))); return cn;} struct _tuple4{
struct Cyc_Absyn_Tqual f1; void* f2; } ; unsigned int Cyc_Evexp_szof( void* t,
struct Cyc_Position_Segment* loc){ void* _temp145= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Tvar* _temp199; int _temp201; struct Cyc_Core_Opt* _temp203;
void* _temp205; int _temp207; struct Cyc_Core_Opt* _temp209; void* _temp211;
struct Cyc_Absyn_TunionInfo _temp213; struct Cyc_Absyn_XTunionInfo _temp215;
struct Cyc_Absyn_TunionFieldInfo _temp217; struct Cyc_Absyn_Tunionfield*
_temp219; struct Cyc_Absyn_Tuniondecl* _temp221; struct _tuple0* _temp223;
struct Cyc_List_List* _temp225; struct _tuple0* _temp227; struct Cyc_Absyn_XTunionFieldInfo
_temp229; struct Cyc_Absyn_Tunionfield* _temp231; struct Cyc_Absyn_XTuniondecl*
_temp233; struct _tuple0* _temp235; struct _tuple0* _temp237; struct Cyc_Absyn_PtrInfo
_temp239; struct Cyc_Absyn_Conref* _temp241; struct Cyc_Absyn_Tqual _temp243;
struct Cyc_Absyn_Conref* _temp245; void* _temp247; void* _temp249; void*
_temp251; void* _temp253; void* _temp255; void* _temp257; void* _temp259; void*
_temp261; void* _temp263; void* _temp265; struct Cyc_Absyn_Exp* _temp267; struct
Cyc_Absyn_Tqual _temp269; void* _temp271; struct Cyc_Absyn_FnInfo _temp273;
struct Cyc_List_List* _temp275; void* _temp277; struct Cyc_Absyn_Structdecl**
_temp279; struct Cyc_List_List* _temp281; struct _tuple0* _temp283; struct Cyc_Absyn_Uniondecl**
_temp285; struct Cyc_List_List* _temp287; struct _tuple0* _temp289; struct Cyc_Absyn_Enumdecl*
_temp291; struct _tuple0* _temp293; struct Cyc_Core_Opt* _temp295; struct Cyc_List_List*
_temp297; struct _tuple0* _temp299; struct Cyc_List_List* _temp301; void*
_temp303; _LL147: if(( int) _temp145 == Cyc_Absyn_VoidType){ goto _LL148;} else{
goto _LL149;} _LL149: if(( unsigned int) _temp145 > 4u?(( struct _tunion_struct*)
_temp145)->tag == Cyc_Absyn_VarType_tag: 0){ _LL200: _temp199=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_VarType_struct*) _temp145)->f1; goto _LL150;} else{ goto _LL151;}
_LL151: if(( unsigned int) _temp145 > 4u?(( struct _tunion_struct*) _temp145)->tag
== Cyc_Absyn_Evar_tag: 0){ _LL206: _temp205=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp145)->f1; if(( int) _temp205 == Cyc_Absyn_BoxKind){ goto _LL204;} else{
goto _LL153;} _LL204: _temp203=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp145)->f2; goto _LL202; _LL202: _temp201=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp145)->f3; goto _LL152;} else{ goto _LL153;} _LL153: if(( unsigned int)
_temp145 > 4u?(( struct _tunion_struct*) _temp145)->tag == Cyc_Absyn_Evar_tag: 0){
_LL212: _temp211=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp145)->f1; goto
_LL210; _LL210: _temp209=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp145)->f2; goto _LL208; _LL208: _temp207=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp145)->f3; goto _LL154;} else{ goto _LL155;} _LL155: if(( unsigned int)
_temp145 > 4u?(( struct _tunion_struct*) _temp145)->tag == Cyc_Absyn_TunionType_tag:
0){ _LL214: _temp213=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp145)->f1; goto _LL156;} else{ goto _LL157;} _LL157: if(( unsigned int)
_temp145 > 4u?(( struct _tunion_struct*) _temp145)->tag == Cyc_Absyn_XTunionType_tag:
0){ _LL216: _temp215=( struct Cyc_Absyn_XTunionInfo)(( struct Cyc_Absyn_XTunionType_struct*)
_temp145)->f1; goto _LL158;} else{ goto _LL159;} _LL159: if(( unsigned int)
_temp145 > 4u?(( struct _tunion_struct*) _temp145)->tag == Cyc_Absyn_TunionFieldType_tag:
0){ _LL218: _temp217=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp145)->f1; _LL228: _temp227=( struct _tuple0*) _temp217.name; goto _LL226;
_LL226: _temp225=( struct Cyc_List_List*) _temp217.targs; goto _LL224; _LL224:
_temp223=( struct _tuple0*) _temp217.fname; goto _LL222; _LL222: _temp221=(
struct Cyc_Absyn_Tuniondecl*) _temp217.tud; goto _LL220; _LL220: _temp219=(
struct Cyc_Absyn_Tunionfield*) _temp217.tufd; goto _LL160;} else{ goto _LL161;}
_LL161: if(( unsigned int) _temp145 > 4u?(( struct _tunion_struct*) _temp145)->tag
== Cyc_Absyn_XTunionFieldType_tag: 0){ _LL230: _temp229=( struct Cyc_Absyn_XTunionFieldInfo)((
struct Cyc_Absyn_XTunionFieldType_struct*) _temp145)->f1; _LL238: _temp237=(
struct _tuple0*) _temp229.name; goto _LL236; _LL236: _temp235=( struct _tuple0*)
_temp229.fname; goto _LL234; _LL234: _temp233=( struct Cyc_Absyn_XTuniondecl*)
_temp229.xtud; goto _LL232; _LL232: _temp231=( struct Cyc_Absyn_Tunionfield*)
_temp229.xtufd; goto _LL162;} else{ goto _LL163;} _LL163: if(( unsigned int)
_temp145 > 4u?(( struct _tunion_struct*) _temp145)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL240: _temp239=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp145)->f1; _LL250: _temp249=( void*) _temp239.elt_typ; goto _LL248; _LL248:
_temp247=( void*) _temp239.rgn_typ; goto _LL246; _LL246: _temp245=( struct Cyc_Absyn_Conref*)
_temp239.nullable; goto _LL244; _LL244: _temp243=( struct Cyc_Absyn_Tqual)
_temp239.tq; goto _LL242; _LL242: _temp241=( struct Cyc_Absyn_Conref*) _temp239.bounds;
goto _LL164;} else{ goto _LL165;} _LL165: if(( unsigned int) _temp145 > 4u?((
struct _tunion_struct*) _temp145)->tag == Cyc_Absyn_IntType_tag: 0){ _LL254:
_temp253=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp145)->f1; goto _LL252;
_LL252: _temp251=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp145)->f2; if((
int) _temp251 == Cyc_Absyn_B1){ goto _LL166;} else{ goto _LL167;}} else{ goto
_LL167;} _LL167: if(( unsigned int) _temp145 > 4u?(( struct _tunion_struct*)
_temp145)->tag == Cyc_Absyn_IntType_tag: 0){ _LL258: _temp257=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp145)->f1; goto _LL256; _LL256: _temp255=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp145)->f2; if(( int) _temp255 == Cyc_Absyn_B2){
goto _LL168;} else{ goto _LL169;}} else{ goto _LL169;} _LL169: if(( unsigned int)
_temp145 > 4u?(( struct _tunion_struct*) _temp145)->tag == Cyc_Absyn_IntType_tag:
0){ _LL262: _temp261=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp145)->f1;
goto _LL260; _LL260: _temp259=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp145)->f2; if(( int) _temp259 == Cyc_Absyn_B4){ goto _LL170;} else{ goto
_LL171;}} else{ goto _LL171;} _LL171: if(( unsigned int) _temp145 > 4u?(( struct
_tunion_struct*) _temp145)->tag == Cyc_Absyn_IntType_tag: 0){ _LL266: _temp265=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp145)->f1; goto _LL264; _LL264:
_temp263=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp145)->f2; if(( int)
_temp263 == Cyc_Absyn_B8){ goto _LL172;} else{ goto _LL173;}} else{ goto _LL173;}
_LL173: if(( int) _temp145 == Cyc_Absyn_FloatType){ goto _LL174;} else{ goto
_LL175;} _LL175: if(( int) _temp145 == Cyc_Absyn_DoubleType){ goto _LL176;}
else{ goto _LL177;} _LL177: if(( unsigned int) _temp145 > 4u?(( struct
_tunion_struct*) _temp145)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL272: _temp271=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp145)->f1; goto _LL270; _LL270:
_temp269=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp145)->f2; goto _LL268; _LL268: _temp267=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp145)->f3; goto _LL178;} else{ goto _LL179;}
_LL179: if(( unsigned int) _temp145 > 4u?(( struct _tunion_struct*) _temp145)->tag
== Cyc_Absyn_FnType_tag: 0){ _LL274: _temp273=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp145)->f1; goto _LL180;} else{ goto _LL181;}
_LL181: if(( unsigned int) _temp145 > 4u?(( struct _tunion_struct*) _temp145)->tag
== Cyc_Absyn_TupleType_tag: 0){ _LL276: _temp275=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp145)->f1; goto _LL182;} else{ goto
_LL183;} _LL183: if(( unsigned int) _temp145 > 4u?(( struct _tunion_struct*)
_temp145)->tag == Cyc_Absyn_RgnHandleType_tag: 0){ _LL278: _temp277=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp145)->f1; goto _LL184;} else{ goto
_LL185;} _LL185: if(( unsigned int) _temp145 > 4u?(( struct _tunion_struct*)
_temp145)->tag == Cyc_Absyn_StructType_tag: 0){ _LL284: _temp283=( struct
_tuple0*)(( struct Cyc_Absyn_StructType_struct*) _temp145)->f1; goto _LL282;
_LL282: _temp281=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp145)->f2; goto _LL280; _LL280: _temp279=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp145)->f3; goto _LL186;} else{ goto
_LL187;} _LL187: if(( unsigned int) _temp145 > 4u?(( struct _tunion_struct*)
_temp145)->tag == Cyc_Absyn_UnionType_tag: 0){ _LL290: _temp289=( struct _tuple0*)((
struct Cyc_Absyn_UnionType_struct*) _temp145)->f1; goto _LL288; _LL288: _temp287=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*) _temp145)->f2; goto
_LL286; _LL286: _temp285=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp145)->f3; goto _LL188;} else{ goto _LL189;} _LL189: if(( unsigned int)
_temp145 > 4u?(( struct _tunion_struct*) _temp145)->tag == Cyc_Absyn_EnumType_tag:
0){ _LL294: _temp293=( struct _tuple0*)(( struct Cyc_Absyn_EnumType_struct*)
_temp145)->f1; goto _LL292; _LL292: _temp291=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_EnumType_struct*) _temp145)->f2; goto _LL190;} else{ goto
_LL191;} _LL191: if(( int) _temp145 == Cyc_Absyn_HeapRgn){ goto _LL192;} else{
goto _LL193;} _LL193: if(( unsigned int) _temp145 > 4u?(( struct _tunion_struct*)
_temp145)->tag == Cyc_Absyn_TypedefType_tag: 0){ _LL300: _temp299=( struct
_tuple0*)(( struct Cyc_Absyn_TypedefType_struct*) _temp145)->f1; goto _LL298;
_LL298: _temp297=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp145)->f2; goto _LL296; _LL296: _temp295=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_TypedefType_struct*) _temp145)->f3; goto _LL194;} else{ goto _LL195;}
_LL195: if(( unsigned int) _temp145 > 4u?(( struct _tunion_struct*) _temp145)->tag
== Cyc_Absyn_JoinEff_tag: 0){ _LL302: _temp301=( struct Cyc_List_List*)(( struct
Cyc_Absyn_JoinEff_struct*) _temp145)->f1; goto _LL196;} else{ goto _LL197;}
_LL197: if(( unsigned int) _temp145 > 4u?(( struct _tunion_struct*) _temp145)->tag
== Cyc_Absyn_AccessEff_tag: 0){ _LL304: _temp303=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp145)->f1; goto _LL198;} else{ goto _LL146;} _LL148: Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp305=( char*)"cannot apply sizeof to void type";
struct _tagged_string _temp306; _temp306.curr= _temp305; _temp306.base= _temp305;
_temp306.last_plus_one= _temp305 + 33; _temp306;})); return 0u; _LL150: { void*
_temp307=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp199->kind); _LL309: if(( int) _temp307 == Cyc_Absyn_BoxKind){ goto _LL310;}
else{ goto _LL311;} _LL311: if(( int) _temp307 == Cyc_Absyn_AnyKind){ goto
_LL312;} else{ goto _LL313;} _LL313: if(( int) _temp307 == Cyc_Absyn_MemKind){
goto _LL314;} else{ goto _LL315;} _LL315: if(( int) _temp307 == Cyc_Absyn_RgnKind){
goto _LL316;} else{ goto _LL317;} _LL317: if(( int) _temp307 == Cyc_Absyn_EffKind){
goto _LL318;} else{ goto _LL308;} _LL310: return 4u; _LL312: goto _LL314; _LL314:
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp319=( char*)"cannot apply sizeof to a non-boxed type variable";
struct _tagged_string _temp320; _temp320.curr= _temp319; _temp320.base= _temp319;
_temp320.last_plus_one= _temp319 + 49; _temp320;})); return 0u; _LL316: Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp321=( char*)"cannot apply sizeof to a region";
struct _tagged_string _temp322; _temp322.curr= _temp321; _temp322.base= _temp321;
_temp322.last_plus_one= _temp321 + 32; _temp322;})); return 0u; _LL318: Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp323=( char*)"cannot apply sizeof to an effect";
struct _tagged_string _temp324; _temp324.curr= _temp323; _temp324.base= _temp323;
_temp324.last_plus_one= _temp323 + 33; _temp324;})); return 0u; _LL308:;} _LL152:
return 4u; _LL154: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp325=( char*)"cannot apply sizeof to unknown type"; struct _tagged_string
_temp326; _temp326.curr= _temp325; _temp326.base= _temp325; _temp326.last_plus_one=
_temp325 + 36; _temp326;})); return 0u; _LL156: return 4u; _LL158: return 4u;
_LL160: _temp231= _temp219; goto _LL162; _LL162: return 4u + Cyc_Evexp_szof((
void*)({ struct Cyc_Absyn_TupleType_struct* _temp327=( struct Cyc_Absyn_TupleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp327[ 0]=({ struct
Cyc_Absyn_TupleType_struct _temp328; _temp328.tag= Cyc_Absyn_TupleType_tag;
_temp328.f1=({ struct Cyc_Absyn_Tunionfield* _temp329= _temp231; if( _temp329 ==
0){ _throw( Null_Exception);} _temp329->typs;}); _temp328;}); _temp327;}), loc);
_LL164: { void* _temp330=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp241))->v; void* _temp338; void* _temp340;
_LL332: if(( unsigned int) _temp330 > 1u?(( struct _tunion_struct*) _temp330)->tag
== Cyc_Absyn_Eq_constr_tag: 0){ _LL339: _temp338=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp330)->f1; if(( int) _temp338 == Cyc_Absyn_Unknown_b){ goto _LL333;} else{
goto _LL334;}} else{ goto _LL334;} _LL334: if(( unsigned int) _temp330 > 1u?((
struct _tunion_struct*) _temp330)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL341:
_temp340=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp330)->f1; goto
_LL335;} else{ goto _LL336;} _LL336: goto _LL337; _LL333: return 12u; _LL335:
return 4u; _LL337: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp342=( char*)"bounds information not known for pointer type"; struct
_tagged_string _temp343; _temp343.curr= _temp342; _temp343.base= _temp342;
_temp343.last_plus_one= _temp342 + 46; _temp343;})); return 0u; _LL331:;} _LL166:
return 1u; _LL168: return 1u; _LL170: return 1u; _LL172: return 1u; _LL174:
return 4u; _LL176: return 8u; _LL178: if( _temp267 == 0){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp344=( char*)"cannot apply sizeof to an array of unknown size";
struct _tagged_string _temp345; _temp345.curr= _temp344; _temp345.base= _temp344;
_temp345.last_plus_one= _temp344 + 48; _temp345;}));} return Cyc_Evexp_szof(
_temp271, loc) * Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)({ struct
Cyc_Absyn_Exp* _temp346= _temp267; if( _temp346 == 0){ _throw( Null_Exception);}
_temp346;})); _LL180: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp347=( char*)"cannot apply sizeof to function type"; struct _tagged_string
_temp348; _temp348.curr= _temp347; _temp348.base= _temp347; _temp348.last_plus_one=
_temp347 + 37; _temp348;})); return 0u; _LL182: { unsigned int s= 0; for( 0;
_temp275 != 0; _temp275=({ struct Cyc_List_List* _temp349= _temp275; if(
_temp349 == 0){ _throw( Null_Exception);} _temp349->tl;})){ s += Cyc_Evexp_szof((*((
struct _tuple4*)({ struct Cyc_List_List* _temp350= _temp275; if( _temp350 == 0){
_throw( Null_Exception);} _temp350->hd;}))).f2, loc);} return s;} _LL184: return
4u; _LL186: if( _temp279 == 0){ return(( unsigned int(*)( struct _tagged_string))
Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp351=( char*)"szof on unchecked StructType";
struct _tagged_string _temp352; _temp352.curr= _temp351; _temp352.base= _temp351;
_temp352.last_plus_one= _temp351 + 29; _temp352;}));}{ struct Cyc_Absyn_Structdecl*
sd=({ struct Cyc_Absyn_Structdecl** _temp358= _temp279; if( _temp358 == 0){
_throw( Null_Exception);}* _temp358;}); if( sd->fields == 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp353=( char*)"cannot apply sizeof to abstract struct type";
struct _tagged_string _temp354; _temp354.curr= _temp353; _temp354.base= _temp353;
_temp354.last_plus_one= _temp353 + 44; _temp354;}));}{ unsigned int s= 0;{
struct Cyc_List_List* fs=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp357= sd->fields; if( _temp357 == 0){ _throw( Null_Exception);} _temp357->v;});
for( 0; fs != 0; fs=({ struct Cyc_List_List* _temp355= fs; if( _temp355 == 0){
_throw( Null_Exception);} _temp355->tl;})){ s += Cyc_Evexp_szof(( void*)((
struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp356= fs; if(
_temp356 == 0){ _throw( Null_Exception);} _temp356->hd;}))->type, loc);}} return
s;}} _LL188: if( _temp285 == 0){ return(( unsigned int(*)( struct _tagged_string))
Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp359=( char*)"szof on unchecked UnionType";
struct _tagged_string _temp360; _temp360.curr= _temp359; _temp360.base= _temp359;
_temp360.last_plus_one= _temp359 + 28; _temp360;}));}{ struct Cyc_Absyn_Uniondecl*
ud=({ struct Cyc_Absyn_Uniondecl** _temp366= _temp285; if( _temp366 == 0){
_throw( Null_Exception);}* _temp366;}); if( ud->fields == 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp361=( char*)"cannot apply sizeof to abstract union type";
struct _tagged_string _temp362; _temp362.curr= _temp361; _temp362.base= _temp361;
_temp362.last_plus_one= _temp361 + 43; _temp362;}));}{ unsigned int s= 0;{
struct Cyc_List_List* fs=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp365= ud->fields; if( _temp365 == 0){ _throw( Null_Exception);} _temp365->v;});
for( 0; fs != 0; fs=({ struct Cyc_List_List* _temp363= fs; if( _temp363 == 0){
_throw( Null_Exception);} _temp363->tl;})){ unsigned int i= Cyc_Evexp_szof((
void*)(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp364= fs;
if( _temp364 == 0){ _throw( Null_Exception);} _temp364->hd;}))->type, loc); s= s
> i? s: i;}} return s;}} _LL190: return 4u; _LL192: Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp367=( char*)"cannot apply sizoef to a region";
struct _tagged_string _temp368; _temp368.curr= _temp367; _temp368.base= _temp367;
_temp368.last_plus_one= _temp367 + 32; _temp368;})); return 0u; _LL194: return((
unsigned int(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({ struct
_tagged_string _temp369= Cyc_Absynpp_typ2string( t); xprintf("szof typedeftype %.*s",
_temp369.last_plus_one - _temp369.curr, _temp369.curr);})); _LL196: goto _LL198;
_LL198: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp370=( char*)"cannot apply sizoef to an effect";
struct _tagged_string _temp371; _temp371.curr= _temp370; _temp371.base= _temp370;
_temp371.last_plus_one= _temp370 + 33; _temp371;})); return 0u; _LL146:;} void*
Cyc_Evexp_eval_sizeof( void* t, struct Cyc_Position_Segment* loc){ return( void*)({
struct Cyc_Absyn_Int_c_struct* _temp372=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp372[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp373; _temp373.tag= Cyc_Absyn_Int_c_tag; _temp373.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp373.f2=( int) Cyc_Evexp_szof( t, loc); _temp373;}); _temp372;});} int Cyc_Evexp_is_arith_const(
void* cn){ void* _temp374= cn; char _temp384; void* _temp386; int _temp388; void*
_temp390; struct _tagged_string _temp392; _LL376: if(( unsigned int) _temp374 >
1u?(( struct _tunion_struct*) _temp374)->tag == Cyc_Absyn_Char_c_tag: 0){ _LL387:
_temp386=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp374)->f1; goto _LL385;
_LL385: _temp384=( char)(( struct Cyc_Absyn_Char_c_struct*) _temp374)->f2; goto
_LL377;} else{ goto _LL378;} _LL378: if(( unsigned int) _temp374 > 1u?(( struct
_tunion_struct*) _temp374)->tag == Cyc_Absyn_Int_c_tag: 0){ _LL391: _temp390=(
void*)(( struct Cyc_Absyn_Int_c_struct*) _temp374)->f1; goto _LL389; _LL389:
_temp388=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp374)->f2; goto _LL379;}
else{ goto _LL380;} _LL380: if(( unsigned int) _temp374 > 1u?(( struct
_tunion_struct*) _temp374)->tag == Cyc_Absyn_Float_c_tag: 0){ _LL393: _temp392=(
struct _tagged_string)(( struct Cyc_Absyn_Float_c_struct*) _temp374)->f1; goto
_LL381;} else{ goto _LL382;} _LL382: goto _LL383; _LL377: return 1; _LL379:
return 1; _LL381: return 1; _LL383: return 0; _LL375:;} void* Cyc_Evexp_eval_const_unprimop(
void* p, struct Cyc_Absyn_Exp* e){ void* cn= Cyc_Evexp_promote_const( Cyc_Evexp_eval_const_exp(
e)); struct _tuple3 _temp395=({ struct _tuple3 _temp394; _temp394.f1= p;
_temp394.f2= cn; _temp394;}); void* _temp413; void* _temp415; void* _temp417;
int _temp419; void* _temp421; void* _temp423; void* _temp425; short _temp427;
void* _temp429; void* _temp431; void* _temp433; char _temp435; void* _temp437;
void* _temp439; void* _temp441; int _temp443; void* _temp445; void* _temp447;
void* _temp449; void* _temp451; void* _temp453; int _temp455; void* _temp457;
void* _temp459; _LL397: _LL416: _temp415= _temp395.f1; if(( int) _temp415 == Cyc_Absyn_Plus){
goto _LL414;} else{ goto _LL399;} _LL414: _temp413= _temp395.f2; goto _LL398;
_LL399: _LL424: _temp423= _temp395.f1; if(( int) _temp423 == Cyc_Absyn_Minus){
goto _LL418;} else{ goto _LL401;} _LL418: _temp417= _temp395.f2; if((
unsigned int) _temp417 > 1u?(( struct _tunion_struct*) _temp417)->tag == Cyc_Absyn_Int_c_tag:
0){ _LL422: _temp421=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp417)->f1;
goto _LL420; _LL420: _temp419=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp417)->f2;
goto _LL400;} else{ goto _LL401;} _LL401: _LL432: _temp431= _temp395.f1; if((
int) _temp431 == Cyc_Absyn_Minus){ goto _LL426;} else{ goto _LL403;} _LL426:
_temp425= _temp395.f2; if(( unsigned int) _temp425 > 1u?(( struct _tunion_struct*)
_temp425)->tag == Cyc_Absyn_Short_c_tag: 0){ _LL430: _temp429=( void*)(( struct
Cyc_Absyn_Short_c_struct*) _temp425)->f1; goto _LL428; _LL428: _temp427=( short)((
struct Cyc_Absyn_Short_c_struct*) _temp425)->f2; goto _LL402;} else{ goto _LL403;}
_LL403: _LL440: _temp439= _temp395.f1; if(( int) _temp439 == Cyc_Absyn_Minus){
goto _LL434;} else{ goto _LL405;} _LL434: _temp433= _temp395.f2; if((
unsigned int) _temp433 > 1u?(( struct _tunion_struct*) _temp433)->tag == Cyc_Absyn_Char_c_tag:
0){ _LL438: _temp437=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp433)->f1;
goto _LL436; _LL436: _temp435=( char)(( struct Cyc_Absyn_Char_c_struct*)
_temp433)->f2; goto _LL404;} else{ goto _LL405;} _LL405: _LL448: _temp447=
_temp395.f1; if(( int) _temp447 == Cyc_Absyn_Not){ goto _LL442;} else{ goto
_LL407;} _LL442: _temp441= _temp395.f2; if(( unsigned int) _temp441 > 1u?((
struct _tunion_struct*) _temp441)->tag == Cyc_Absyn_Int_c_tag: 0){ _LL446:
_temp445=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp441)->f1; goto _LL444;
_LL444: _temp443=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp441)->f2; goto
_LL406;} else{ goto _LL407;} _LL407: _LL452: _temp451= _temp395.f1; if(( int)
_temp451 == Cyc_Absyn_Not){ goto _LL450;} else{ goto _LL409;} _LL450: _temp449=
_temp395.f2; if(( int) _temp449 == Cyc_Absyn_Null_c){ goto _LL408;} else{ goto
_LL409;} _LL409: _LL460: _temp459= _temp395.f1; if(( int) _temp459 == Cyc_Absyn_Bitnot){
goto _LL454;} else{ goto _LL411;} _LL454: _temp453= _temp395.f2; if((
unsigned int) _temp453 > 1u?(( struct _tunion_struct*) _temp453)->tag == Cyc_Absyn_Int_c_tag:
0){ _LL458: _temp457=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp453)->f1;
goto _LL456; _LL456: _temp455=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp453)->f2;
goto _LL410;} else{ goto _LL411;} _LL411: goto _LL412; _LL398: if( ! Cyc_Evexp_is_arith_const(
cn)){ Cyc_Evexp_exp_err( e->loc,( struct _tagged_string)({ char* _temp461=( char*)"expecting arithmetic constant";
struct _tagged_string _temp462; _temp462.curr= _temp461; _temp462.base= _temp461;
_temp462.last_plus_one= _temp461 + 30; _temp462;}));} return cn; _LL400: return(
void*)({ struct Cyc_Absyn_Int_c_struct* _temp463=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp463[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp464; _temp464.tag= Cyc_Absyn_Int_c_tag; _temp464.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp464.f2= - _temp419; _temp464;}); _temp463;}); _LL402: return( void*)({
struct Cyc_Absyn_Short_c_struct* _temp465=( struct Cyc_Absyn_Short_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Short_c_struct)); _temp465[ 0]=({ struct Cyc_Absyn_Short_c_struct
_temp466; _temp466.tag= Cyc_Absyn_Short_c_tag; _temp466.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp466.f2= - _temp427; _temp466;}); _temp465;}); _LL404: return( void*)({
struct Cyc_Absyn_Char_c_struct* _temp467=( struct Cyc_Absyn_Char_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct)); _temp467[ 0]=({ struct Cyc_Absyn_Char_c_struct
_temp468; _temp468.tag= Cyc_Absyn_Char_c_tag; _temp468.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp468.f2= - _temp435; _temp468;}); _temp467;}); _LL406: return( void*)({
struct Cyc_Absyn_Int_c_struct* _temp469=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp469[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp470; _temp470.tag= Cyc_Absyn_Int_c_tag; _temp470.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp470.f2= _temp443 == 0? 1: 0; _temp470;}); _temp469;}); _LL408: return( void*)({
struct Cyc_Absyn_Int_c_struct* _temp471=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp471[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp472; _temp472.tag= Cyc_Absyn_Int_c_tag; _temp472.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp472.f2= 1; _temp472;}); _temp471;}); _LL410: return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp473=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp473[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp474; _temp474.tag= Cyc_Absyn_Int_c_tag;
_temp474.f1=( void*)(( void*) Cyc_Absyn_Unsigned); _temp474.f2= ~ _temp455;
_temp474;}); _temp473;}); _LL412: Cyc_Evexp_exp_err( e->loc,( struct
_tagged_string)({ char* _temp475=( char*)"bad constant expression"; struct
_tagged_string _temp476; _temp476.curr= _temp475; _temp476.base= _temp475;
_temp476.last_plus_one= _temp475 + 24; _temp476;})); return cn; _LL396:;} struct
_tuple5{ void* f1; int f2; } ; void* Cyc_Evexp_eval_const_binprimop( void* p,
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ void* cn1= Cyc_Evexp_promote_const(
Cyc_Evexp_eval_const_exp( e1)); void* cn2= Cyc_Evexp_promote_const( Cyc_Evexp_eval_const_exp(
e2)); void* s1; void* s2; int i1; int i2;{ void* _temp477= cn1; int _temp483;
void* _temp485; _LL479: if(( unsigned int) _temp477 > 1u?(( struct
_tunion_struct*) _temp477)->tag == Cyc_Absyn_Int_c_tag: 0){ _LL486: _temp485=(
void*)(( struct Cyc_Absyn_Int_c_struct*) _temp477)->f1; goto _LL484; _LL484:
_temp483=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp477)->f2; goto _LL480;}
else{ goto _LL481;} _LL481: goto _LL482; _LL480: s1= _temp485; i1= _temp483;
goto _LL478; _LL482: Cyc_Evexp_exp_err( e1->loc,( struct _tagged_string)({ char*
_temp487=( char*)"bad constant expression"; struct _tagged_string _temp488;
_temp488.curr= _temp487; _temp488.base= _temp487; _temp488.last_plus_one=
_temp487 + 24; _temp488;})); return cn1; _LL478:;}{ void* _temp489= cn2; int
_temp495; void* _temp497; _LL491: if(( unsigned int) _temp489 > 1u?(( struct
_tunion_struct*) _temp489)->tag == Cyc_Absyn_Int_c_tag: 0){ _LL498: _temp497=(
void*)(( struct Cyc_Absyn_Int_c_struct*) _temp489)->f1; goto _LL496; _LL496:
_temp495=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp489)->f2; goto _LL492;}
else{ goto _LL493;} _LL493: goto _LL494; _LL492: s2= _temp497; i2= _temp495;
goto _LL490; _LL494: Cyc_Evexp_exp_err( e2->loc,( struct _tagged_string)({ char*
_temp499=( char*)"bad constant expression"; struct _tagged_string _temp500;
_temp500.curr= _temp499; _temp500.base= _temp499; _temp500.last_plus_one=
_temp499 + 24; _temp500;})); return cn1; _LL490:;}{ void* _temp501= p; _LL503:
if(( int) _temp501 == Cyc_Absyn_Div){ goto _LL504;} else{ goto _LL505;} _LL505:
if(( int) _temp501 == Cyc_Absyn_Mod){ goto _LL506;} else{ goto _LL507;} _LL507:
goto _LL508; _LL504: if( i2 == 0){ Cyc_Evexp_exp_err( e2->loc,( struct
_tagged_string)({ char* _temp509=( char*)"division by zero in constant expression";
struct _tagged_string _temp510; _temp510.curr= _temp509; _temp510.base= _temp509;
_temp510.last_plus_one= _temp509 + 40; _temp510;})); return cn1;} goto _LL502;
_LL506: if( i2 == 0){ Cyc_Evexp_exp_err( e2->loc,( struct _tagged_string)({ char*
_temp511=( char*)"division by zero in constant expression"; struct
_tagged_string _temp512; _temp512.curr= _temp511; _temp512.base= _temp511;
_temp512.last_plus_one= _temp511 + 40; _temp512;})); return cn1;} goto _LL502;
_LL508: goto _LL502; _LL502:;}{ int has_u_arg= s1 ==( void*) Cyc_Absyn_Unsigned?
1: s2 ==( void*) Cyc_Absyn_Unsigned; unsigned int u1=( unsigned int) i1;
unsigned int u2=( unsigned int) i2; int i3= 0; unsigned int u3= 0; int b3= 1;
int use_i3= 0; int use_u3= 0; int use_b3= 0;{ struct _tuple5 _temp514=({ struct
_tuple5 _temp513; _temp513.f1= p; _temp513.f2= has_u_arg; _temp513;}); int
_temp570; void* _temp572; int _temp574; void* _temp576; int _temp578; void*
_temp580; int _temp582; void* _temp584; int _temp586; void* _temp588; int
_temp590; void* _temp592; int _temp594; void* _temp596; int _temp598; void*
_temp600; int _temp602; void* _temp604; int _temp606; void* _temp608; int
_temp610; void* _temp612; int _temp614; void* _temp616; int _temp618; void*
_temp620; int _temp622; void* _temp624; int _temp626; void* _temp628; int
_temp630; void* _temp632; int _temp634; void* _temp636; int _temp638; void*
_temp640; int _temp642; void* _temp644; int _temp646; void* _temp648; int
_temp650; void* _temp652; int _temp654; void* _temp656; int _temp658; void*
_temp660; int _temp662; void* _temp664; int _temp666; void* _temp668; int
_temp670; void* _temp672; _LL516: _LL573: _temp572= _temp514.f1; if(( int)
_temp572 == Cyc_Absyn_Plus){ goto _LL571;} else{ goto _LL518;} _LL571: _temp570=
_temp514.f2; if( _temp570 == 0){ goto _LL517;} else{ goto _LL518;} _LL518:
_LL577: _temp576= _temp514.f1; if(( int) _temp576 == Cyc_Absyn_Times){ goto
_LL575;} else{ goto _LL520;} _LL575: _temp574= _temp514.f2; if( _temp574 == 0){
goto _LL519;} else{ goto _LL520;} _LL520: _LL581: _temp580= _temp514.f1; if((
int) _temp580 == Cyc_Absyn_Minus){ goto _LL579;} else{ goto _LL522;} _LL579:
_temp578= _temp514.f2; if( _temp578 == 0){ goto _LL521;} else{ goto _LL522;}
_LL522: _LL585: _temp584= _temp514.f1; if(( int) _temp584 == Cyc_Absyn_Div){
goto _LL583;} else{ goto _LL524;} _LL583: _temp582= _temp514.f2; if( _temp582 ==
0){ goto _LL523;} else{ goto _LL524;} _LL524: _LL589: _temp588= _temp514.f1; if((
int) _temp588 == Cyc_Absyn_Mod){ goto _LL587;} else{ goto _LL526;} _LL587:
_temp586= _temp514.f2; if( _temp586 == 0){ goto _LL525;} else{ goto _LL526;}
_LL526: _LL593: _temp592= _temp514.f1; if(( int) _temp592 == Cyc_Absyn_Plus){
goto _LL591;} else{ goto _LL528;} _LL591: _temp590= _temp514.f2; if( _temp590 ==
1){ goto _LL527;} else{ goto _LL528;} _LL528: _LL597: _temp596= _temp514.f1; if((
int) _temp596 == Cyc_Absyn_Times){ goto _LL595;} else{ goto _LL530;} _LL595:
_temp594= _temp514.f2; if( _temp594 == 1){ goto _LL529;} else{ goto _LL530;}
_LL530: _LL601: _temp600= _temp514.f1; if(( int) _temp600 == Cyc_Absyn_Minus){
goto _LL599;} else{ goto _LL532;} _LL599: _temp598= _temp514.f2; if( _temp598 ==
1){ goto _LL531;} else{ goto _LL532;} _LL532: _LL605: _temp604= _temp514.f1; if((
int) _temp604 == Cyc_Absyn_Div){ goto _LL603;} else{ goto _LL534;} _LL603:
_temp602= _temp514.f2; if( _temp602 == 1){ goto _LL533;} else{ goto _LL534;}
_LL534: _LL609: _temp608= _temp514.f1; if(( int) _temp608 == Cyc_Absyn_Mod){
goto _LL607;} else{ goto _LL536;} _LL607: _temp606= _temp514.f2; if( _temp606 ==
1){ goto _LL535;} else{ goto _LL536;} _LL536: _LL613: _temp612= _temp514.f1; if((
int) _temp612 == Cyc_Absyn_Eq){ goto _LL611;} else{ goto _LL538;} _LL611:
_temp610= _temp514.f2; goto _LL537; _LL538: _LL617: _temp616= _temp514.f1; if((
int) _temp616 == Cyc_Absyn_Neq){ goto _LL615;} else{ goto _LL540;} _LL615:
_temp614= _temp514.f2; goto _LL539; _LL540: _LL621: _temp620= _temp514.f1; if((
int) _temp620 == Cyc_Absyn_Gt){ goto _LL619;} else{ goto _LL542;} _LL619:
_temp618= _temp514.f2; if( _temp618 == 0){ goto _LL541;} else{ goto _LL542;}
_LL542: _LL625: _temp624= _temp514.f1; if(( int) _temp624 == Cyc_Absyn_Lt){ goto
_LL623;} else{ goto _LL544;} _LL623: _temp622= _temp514.f2; if( _temp622 == 0){
goto _LL543;} else{ goto _LL544;} _LL544: _LL629: _temp628= _temp514.f1; if((
int) _temp628 == Cyc_Absyn_Gte){ goto _LL627;} else{ goto _LL546;} _LL627:
_temp626= _temp514.f2; if( _temp626 == 0){ goto _LL545;} else{ goto _LL546;}
_LL546: _LL633: _temp632= _temp514.f1; if(( int) _temp632 == Cyc_Absyn_Lte){
goto _LL631;} else{ goto _LL548;} _LL631: _temp630= _temp514.f2; if( _temp630 ==
0){ goto _LL547;} else{ goto _LL548;} _LL548: _LL637: _temp636= _temp514.f1; if((
int) _temp636 == Cyc_Absyn_Gt){ goto _LL635;} else{ goto _LL550;} _LL635:
_temp634= _temp514.f2; if( _temp634 == 1){ goto _LL549;} else{ goto _LL550;}
_LL550: _LL641: _temp640= _temp514.f1; if(( int) _temp640 == Cyc_Absyn_Lt){ goto
_LL639;} else{ goto _LL552;} _LL639: _temp638= _temp514.f2; if( _temp638 == 1){
goto _LL551;} else{ goto _LL552;} _LL552: _LL645: _temp644= _temp514.f1; if((
int) _temp644 == Cyc_Absyn_Gte){ goto _LL643;} else{ goto _LL554;} _LL643:
_temp642= _temp514.f2; if( _temp642 == 1){ goto _LL553;} else{ goto _LL554;}
_LL554: _LL649: _temp648= _temp514.f1; if(( int) _temp648 == Cyc_Absyn_Lte){
goto _LL647;} else{ goto _LL556;} _LL647: _temp646= _temp514.f2; if( _temp646 ==
1){ goto _LL555;} else{ goto _LL556;} _LL556: _LL653: _temp652= _temp514.f1; if((
int) _temp652 == Cyc_Absyn_Bitand){ goto _LL651;} else{ goto _LL558;} _LL651:
_temp650= _temp514.f2; goto _LL557; _LL558: _LL657: _temp656= _temp514.f1; if((
int) _temp656 == Cyc_Absyn_Bitor){ goto _LL655;} else{ goto _LL560;} _LL655:
_temp654= _temp514.f2; goto _LL559; _LL560: _LL661: _temp660= _temp514.f1; if((
int) _temp660 == Cyc_Absyn_Bitxor){ goto _LL659;} else{ goto _LL562;} _LL659:
_temp658= _temp514.f2; goto _LL561; _LL562: _LL665: _temp664= _temp514.f1; if((
int) _temp664 == Cyc_Absyn_Bitlshift){ goto _LL663;} else{ goto _LL564;} _LL663:
_temp662= _temp514.f2; goto _LL563; _LL564: _LL669: _temp668= _temp514.f1; if((
int) _temp668 == Cyc_Absyn_Bitlrshift){ goto _LL667;} else{ goto _LL566;} _LL667:
_temp666= _temp514.f2; goto _LL565; _LL566: _LL673: _temp672= _temp514.f1; if((
int) _temp672 == Cyc_Absyn_Bitarshift){ goto _LL671;} else{ goto _LL568;} _LL671:
_temp670= _temp514.f2; goto _LL567; _LL568: goto _LL569; _LL517: i3= i1 + i2;
use_i3= 1; goto _LL515; _LL519: i3= i1 * i2; use_i3= 1; goto _LL515; _LL521: i3=
i1 - i2; use_i3= 1; goto _LL515; _LL523: i3= i1 / i2; use_i3= 1; goto _LL515;
_LL525: i3= i1 % i2; use_i3= 1; goto _LL515; _LL527: u3= u1 + u2; use_u3= 1;
goto _LL515; _LL529: u3= u1 * u2; use_u3= 1; goto _LL515; _LL531: u3= u1 - u2;
use_u3= 1; goto _LL515; _LL533: u3= u1 / u2; use_u3= 1; goto _LL515; _LL535: u3=
u1 % u2; use_u3= 1; goto _LL515; _LL537: b3= i1 == i2; use_b3= 1; goto _LL515;
_LL539: b3= i1 != i2; use_b3= 1; goto _LL515; _LL541: b3= i1 > i2; use_b3= 1;
goto _LL515; _LL543: b3= i1 < i2; use_b3= 1; goto _LL515; _LL545: b3= i1 >= i2;
use_b3= 1; goto _LL515; _LL547: b3= i1 <= i2; use_b3= 1; goto _LL515; _LL549: b3=
u1 > u2; use_b3= 1; goto _LL515; _LL551: b3= u1 < u2; use_b3= 1; goto _LL515;
_LL553: b3= u1 >= u2; use_b3= 1; goto _LL515; _LL555: b3= u1 <= u2; use_b3= 1;
goto _LL515; _LL557: u3= u1 & u2; use_u3= 1; goto _LL515; _LL559: u3= u1 | u2;
use_u3= 1; goto _LL515; _LL561: u3= u1 ^ u2; use_u3= 1; goto _LL515; _LL563: u3=
u1 << u2; use_u3= 1; goto _LL515; _LL565: u3= u1 >> u2; use_u3= 1; goto _LL515;
_LL567: Cyc_Evexp_exp_err( e1->loc,( struct _tagged_string)({ char* _temp674=(
char*)">>> NOT IMPLEMENTED"; struct _tagged_string _temp675; _temp675.curr=
_temp674; _temp675.base= _temp674; _temp675.last_plus_one= _temp674 + 20;
_temp675;})); return cn1; _LL569: Cyc_Evexp_exp_err( e1->loc,( struct
_tagged_string)({ char* _temp676=( char*)"bad constant expression"; struct
_tagged_string _temp677; _temp677.curr= _temp676; _temp677.base= _temp676;
_temp677.last_plus_one= _temp676 + 24; _temp677;})); return cn1; _LL515:;} if(
use_i3){ return( void*)({ struct Cyc_Absyn_Int_c_struct* _temp678=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp678[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp679; _temp679.tag= Cyc_Absyn_Int_c_tag; _temp679.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp679.f2= i3; _temp679;}); _temp678;});} if( use_u3){ return( void*)({ struct
Cyc_Absyn_Int_c_struct* _temp680=( struct Cyc_Absyn_Int_c_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Int_c_struct)); _temp680[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp681; _temp681.tag= Cyc_Absyn_Int_c_tag; _temp681.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp681.f2=( int) u3; _temp681;}); _temp680;});} if( use_b3){ return( void*)({
struct Cyc_Absyn_Int_c_struct* _temp682=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp682[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp683; _temp683.tag= Cyc_Absyn_Int_c_tag; _temp683.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp683.f2= b3? 1: 0; _temp683;}); _temp682;});}( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Core_Unreachable_struct* _temp684=( struct Cyc_Core_Unreachable_struct*)
GC_malloc( sizeof( struct Cyc_Core_Unreachable_struct)); _temp684[ 0]=({ struct
Cyc_Core_Unreachable_struct _temp685; _temp685.tag= Cyc_Core_Unreachable_tag;
_temp685.f1=( struct _tagged_string)({ char* _temp686=( char*)"Evexp::eval_const_binop";
struct _tagged_string _temp687; _temp687.curr= _temp686; _temp687.base= _temp686;
_temp687.last_plus_one= _temp686 + 24; _temp687;}); _temp685;}); _temp684;}));}}