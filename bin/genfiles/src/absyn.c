#include "cyc_include.h"

 struct _tuple0{ void* f1; struct _tagged_string* f2; } ; struct _tuple1{ struct
Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual* f2; void* f3; } ; struct _tuple2{
struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; } ; struct _tuple3{ struct
Cyc_Absyn_Tqual* f1; void* f2; } ; struct _tuple4{ void* f1; void* f2; } ;
struct _tuple5{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ; typedef
int Cyc_ptrdiff_t; typedef unsigned int Cyc_size_t; typedef unsigned short Cyc_wchar_t;
typedef unsigned int Cyc_wint_t; typedef char Cyc_u_char; typedef unsigned short
Cyc_u_short; typedef unsigned int Cyc_u_int; typedef unsigned int Cyc_u_long;
typedef unsigned short Cyc_ushort; typedef unsigned int Cyc_uint; typedef
unsigned int Cyc_clock_t; typedef int Cyc_time_t; struct Cyc_timespec{ int
tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec it_interval;
struct Cyc_timespec it_value; } ; typedef int Cyc_daddr_t; typedef char* Cyc_caddr_t;
typedef unsigned int Cyc_ino_t; typedef unsigned int Cyc_vm_offset_t; typedef
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
struct Cyc_List_List_empty_struct{ char* tag; } ; extern struct Cyc_List_List*
Cyc_List_map( void*(* f)( void*), struct Cyc_List_List* x); extern char Cyc_List_List_mismatch_tag[
14u]; struct Cyc_List_List_mismatch_struct{ char* tag; } ; extern struct Cyc_List_List*
Cyc_List_imp_rev( struct Cyc_List_List* x); extern char Cyc_List_Nth_tag[ 4u];
struct Cyc_List_Nth_struct{ char* tag; } ; extern int Cyc_List_list_cmp( int(*
cmp)( void*, void*), struct Cyc_List_List* l1, struct Cyc_List_List* l2); struct
Cyc_Lineno_Pos{ struct _tagged_string logical_file; struct _tagged_string line;
int line_no; int col; } ; typedef struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t;
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
char Cyc_Absyn_EmptyAnnot_tag[ 11u]="EmptyAnnot"; struct Cyc_Absyn_EmptyAnnot_struct{
char* tag; } ; extern int Cyc_Absyn_qvar_cmp( struct _tuple0*, struct _tuple0*);
extern int Cyc_Absyn_varlist_cmp( struct Cyc_List_List*, struct Cyc_List_List*);
extern int Cyc_Absyn_tvar_cmp( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*);
extern struct Cyc_Absyn_Tqual* Cyc_Absyn_combine_tqual( struct Cyc_Absyn_Tqual*
x, struct Cyc_Absyn_Tqual* y); extern struct Cyc_Absyn_Tqual* Cyc_Absyn_empty_tqual();
extern struct Cyc_Absyn_Conref* Cyc_Absyn_new_conref( void* x); extern struct
Cyc_Absyn_Conref* Cyc_Absyn_empty_conref(); extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref*
x); extern void* Cyc_Absyn_new_evar( void*); extern void* Cyc_Absyn_wildtyp();
extern void* Cyc_Absyn_uchar_t; extern void* Cyc_Absyn_ushort_t; extern void*
Cyc_Absyn_uint_t; extern void* Cyc_Absyn_ulong_t; extern void* Cyc_Absyn_schar_t;
extern void* Cyc_Absyn_sshort_t; extern void* Cyc_Absyn_sint_t; extern void* Cyc_Absyn_slong_t;
extern void* Cyc_Absyn_float_t; extern void* Cyc_Absyn_double_t; extern struct
_tuple0* Cyc_Absyn_exn_name; extern struct Cyc_Absyn_XTuniondecl* Cyc_Absyn_exn_xed;
extern void* Cyc_Absyn_exn_typ; extern void* Cyc_Absyn_string_typ( void* rgn);
extern void* Cyc_Absyn_file_typ(); extern void* Cyc_Absyn_starb_typ( void* t,
void* rgn, struct Cyc_Absyn_Tqual* tq, void* b); extern void* Cyc_Absyn_atb_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual* tq, void* b); extern void* Cyc_Absyn_star_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual* tq); extern void* Cyc_Absyn_at_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual* tq); extern void* Cyc_Absyn_cstar_typ(
void* t, struct Cyc_Absyn_Tqual* tq); extern void* Cyc_Absyn_tagged_typ( void* t,
void* rgn, struct Cyc_Absyn_Tqual* tq); extern void* Cyc_Absyn_void_star_typ();
extern void* Cyc_Absyn_strct( struct _tagged_string* name); extern void* Cyc_Absyn_strctq(
struct _tuple0* name); extern void* Cyc_Absyn_unionq_typ( struct _tuple0* name);
extern void* Cyc_Absyn_union_typ( struct _tagged_string* name); extern struct
Cyc_Absyn_Exp* Cyc_Absyn_new_exp( void*, struct Cyc_Position_Segment*); extern
struct Cyc_Absyn_Exp* Cyc_Absyn_New_exp( struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_copy_exp( struct Cyc_Absyn_Exp*); extern
struct Cyc_Absyn_Exp* Cyc_Absyn_const_exp( void*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_null_exp( struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_bool_exp( int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_true_exp( struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_false_exp( struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_int_exp( void*, int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_signed_int_exp( int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp( unsigned int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_char_exp( char c, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_float_exp( struct _tagged_string f,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_string_exp(
struct _tagged_string s, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_var_exp( struct _tuple0*, struct Cyc_Position_Segment*); extern struct
Cyc_Absyn_Exp* Cyc_Absyn_varb_exp( struct _tuple0*, void*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_unknownid_exp( struct _tuple0*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_primop_exp( void*,
struct Cyc_List_List* es, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_prim1_exp( void*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_prim2_exp( void*, struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_add_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_subtract_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_times_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_divide_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_eq_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_neq_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_gt_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_lt_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_gte_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_lte_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Core_Opt*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_assign_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_increment_exp(
struct Cyc_Absyn_Exp*, void*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_post_inc_exp( struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_post_dec_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_pre_inc_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_pre_dec_exp( struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_conditional_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_and_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_or_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_seq_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_unknowncall_exp(
struct Cyc_Absyn_Exp*, struct Cyc_List_List*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_fncall_exp( struct Cyc_Absyn_Exp*, struct
Cyc_List_List*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_throw_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_noinstantiate_exp( struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_instantiate_exp( struct Cyc_Absyn_Exp*,
struct Cyc_List_List*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_cast_exp( void*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_address_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_sizeoftyp_exp(
void* t, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_sizeofexp_exp(
struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_deref_exp( struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_structmember_exp( struct Cyc_Absyn_Exp*,
struct _tagged_string*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_structarrow_exp( struct Cyc_Absyn_Exp*, struct _tagged_string*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_subscript_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_tuple_exp( struct Cyc_List_List*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_stmt_exp( struct
Cyc_Absyn_Stmt*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_null_pointer_exn_exp(
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_array_exp(
struct Cyc_List_List*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_unresolvedmem_exp( struct Cyc_Core_Opt*, struct Cyc_List_List*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_new_stmt( void*
s, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_skip_stmt(
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_exp_stmt(
struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_seq_stmt( struct Cyc_Absyn_Stmt* s1, struct Cyc_Absyn_Stmt* s2, struct
Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_seq_stmts(
struct Cyc_List_List*, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_return_stmt( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc);
extern struct Cyc_Absyn_Stmt* Cyc_Absyn_ifthenelse_stmt( struct Cyc_Absyn_Exp* e,
struct Cyc_Absyn_Stmt* s1, struct Cyc_Absyn_Stmt* s2, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_while_stmt( struct Cyc_Absyn_Exp*
e, struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Stmt* Cyc_Absyn_break_stmt( struct Cyc_Position_Segment* loc); extern
struct Cyc_Absyn_Stmt* Cyc_Absyn_continue_stmt( struct Cyc_Position_Segment* loc);
extern struct Cyc_Absyn_Stmt* Cyc_Absyn_for_stmt( struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3, struct Cyc_Absyn_Stmt* s,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_switch_stmt(
struct Cyc_Absyn_Exp* e, struct Cyc_List_List*, struct Cyc_Position_Segment* loc);
extern struct Cyc_Absyn_Stmt* Cyc_Absyn_fallthru_stmt( struct Cyc_List_List* el,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_decl_stmt(
struct Cyc_Absyn_Decl* d, struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_declare_stmt( struct _tuple0*,
void*, struct Cyc_Absyn_Exp* init, struct Cyc_Absyn_Stmt*, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_cut_stmt( struct Cyc_Absyn_Stmt* s,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_splice_stmt(
struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_label_stmt( struct _tagged_string* v, struct Cyc_Absyn_Stmt* s, struct
Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_do_stmt(
struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_trycatch_stmt( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* scs, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Stmt* Cyc_Absyn_goto_stmt( struct _tagged_string* lab, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_assign_stmt( struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Pat* Cyc_Absyn_new_pat( void* p, struct Cyc_Position_Segment* s);
extern struct Cyc_Absyn_Decl* Cyc_Absyn_new_decl( void* r, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Decl* Cyc_Absyn_let_decl( struct Cyc_Absyn_Pat* p,
struct Cyc_Core_Opt* t_opt, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Vardecl* Cyc_Absyn_new_vardecl( struct _tuple0* x,
void* t, struct Cyc_Absyn_Exp* init); extern struct Cyc_Absyn_Vardecl* Cyc_Absyn_static_vardecl(
struct _tuple0* x, void* t, struct Cyc_Absyn_Exp* init); extern struct Cyc_Absyn_Decl*
Cyc_Absyn_struct_decl( void* s, struct Cyc_Core_Opt* n, struct Cyc_List_List* ts,
struct Cyc_Core_Opt* fs, struct Cyc_List_List* atts, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Decl* Cyc_Absyn_union_decl( void* s, struct Cyc_Core_Opt*
n, struct Cyc_List_List* ts, struct Cyc_Core_Opt* fs, struct Cyc_List_List* atts,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Decl* Cyc_Absyn_tunion_decl(
void* s, struct Cyc_Core_Opt* n, struct Cyc_List_List* ts, struct Cyc_Core_Opt*
fs, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Decl* Cyc_Absyn_xtunion_decl(
void* s, struct _tuple0* n, struct Cyc_List_List* fs, struct Cyc_Position_Segment*
loc); extern int Cyc_Absyn_is_format_prim( void* p); extern void* Cyc_Absyn_function_typ(
struct Cyc_List_List* tvs, struct Cyc_Core_Opt* eff_typ, void* ret_typ, struct
Cyc_List_List* args, int varargs, struct Cyc_List_List*); extern void* Cyc_Absyn_pointer_expand(
void*); extern int Cyc_Absyn_is_lvalue( struct Cyc_Absyn_Exp*); extern struct
Cyc_Absyn_Structfield* Cyc_Absyn_lookup_field( struct Cyc_Core_Opt*, struct
_tagged_string*); extern struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_struct_field(
struct Cyc_Absyn_Structdecl*, struct _tagged_string*); extern struct Cyc_Absyn_Structfield*
Cyc_Absyn_lookup_union_field( struct Cyc_Absyn_Uniondecl*, struct _tagged_string*);
extern struct _tuple3* Cyc_Absyn_lookup_tuple_field( struct Cyc_List_List*, int);
extern struct _tagged_string Cyc_Absyn_attribute2string( void*); struct Cyc_Stdio___sFILE;
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; typedef int Cyc_Stdio_fpos_t;
extern char Cyc_Stdio_FileOpenError_tag[ 14u]; struct Cyc_Stdio_FileOpenError_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Stdio_FileCloseError_tag[
15u]; struct Cyc_Stdio_FileCloseError_struct{ char* tag; } ; extern int Cyc_String_zstrptrcmp(
struct _tagged_string*, struct _tagged_string*); struct Cyc_Set_Set; typedef
struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct Cyc_Set_Set* Cyc_Set_hset_t;
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
tag; } ; extern void* Cyc_Tcutil_impos( struct _tagged_string); extern void* Cyc_Tcutil_compress(
void* t); static int Cyc_Absyn_zstrlist_cmp( struct Cyc_List_List* ss1, struct
Cyc_List_List* ss2){ return(( int(*)( int(* cmp)( struct _tagged_string*, struct
_tagged_string*), struct Cyc_List_List* l1, struct Cyc_List_List* l2)) Cyc_List_list_cmp)(
Cyc_String_zstrptrcmp, ss1, ss2);} int Cyc_Absyn_varlist_cmp( struct Cyc_List_List*
vs1, struct Cyc_List_List* vs2){ return Cyc_Absyn_zstrlist_cmp( vs1, vs2);} int
Cyc_Absyn_qvar_cmp( struct _tuple0* q1, struct _tuple0* q2){ void* n1=(* q1).f1;
void* n2=(* q2).f1;{ struct _tuple4 _temp1=({ struct _tuple4 _temp0; _temp0.f1=
n1; _temp0.f2= n2; _temp0;}); void* _temp17; void* _temp19; void* _temp21;
struct Cyc_List_List* _temp23; void* _temp25; struct Cyc_List_List* _temp27;
void* _temp29; struct Cyc_List_List* _temp31; void* _temp33; struct Cyc_List_List*
_temp35; void* _temp37; void* _temp39; void* _temp41; void* _temp43; void*
_temp45; void* _temp47; struct Cyc_List_List* _temp49; void* _temp51; struct Cyc_List_List*
_temp53; void* _temp55; _LL3: _LL20: _temp19= _temp1.f1; if(( int) _temp19 ==
Cyc_Absyn_Loc_n){ goto _LL18;} else{ goto _LL5;} _LL18: _temp17= _temp1.f2; if((
int) _temp17 == Cyc_Absyn_Loc_n){ goto _LL4;} else{ goto _LL5;} _LL5: _LL26:
_temp25= _temp1.f1; if(( unsigned int) _temp25 > 1u?(( struct _tunion_struct*)
_temp25)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL28: _temp27=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp25)->f1; goto _LL22;} else{ goto _LL7;}
_LL22: _temp21= _temp1.f2; if(( unsigned int) _temp21 > 1u?(( struct
_tunion_struct*) _temp21)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL24: _temp23=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp21)->f1; goto _LL6;}
else{ goto _LL7;} _LL7: _LL34: _temp33= _temp1.f1; if(( unsigned int) _temp33 >
1u?(( struct _tunion_struct*) _temp33)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL36:
_temp35=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp33)->f1;
goto _LL30;} else{ goto _LL9;} _LL30: _temp29= _temp1.f2; if(( unsigned int)
_temp29 > 1u?(( struct _tunion_struct*) _temp29)->tag == Cyc_Absyn_Abs_n_tag: 0){
_LL32: _temp31=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp29)->f1; goto _LL8;} else{ goto _LL9;} _LL9: _LL40: _temp39= _temp1.f1; if((
int) _temp39 == Cyc_Absyn_Loc_n){ goto _LL38;} else{ goto _LL11;} _LL38: _temp37=
_temp1.f2; goto _LL10; _LL11: _LL44: _temp43= _temp1.f1; goto _LL42; _LL42:
_temp41= _temp1.f2; if(( int) _temp41 == Cyc_Absyn_Loc_n){ goto _LL12;} else{
goto _LL13;} _LL13: _LL48: _temp47= _temp1.f1; if(( unsigned int) _temp47 > 1u?((
struct _tunion_struct*) _temp47)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL50: _temp49=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp47)->f1; goto
_LL46;} else{ goto _LL15;} _LL46: _temp45= _temp1.f2; goto _LL14; _LL15: _LL56:
_temp55= _temp1.f1; goto _LL52; _LL52: _temp51= _temp1.f2; if(( unsigned int)
_temp51 > 1u?(( struct _tunion_struct*) _temp51)->tag == Cyc_Absyn_Rel_n_tag: 0){
_LL54: _temp53=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp51)->f1; goto _LL16;} else{ goto _LL2;} _LL4: goto _LL2; _LL6: _temp35=
_temp27; _temp31= _temp23; goto _LL8; _LL8: { int i= Cyc_Absyn_zstrlist_cmp(
_temp35, _temp31); if( i != 0){ return i;} goto _LL2;} _LL10: return - 1; _LL12:
return 1; _LL14: return - 1; _LL16: return 1; _LL2:;} return Cyc_String_zstrptrcmp((*
q1).f2,(* q2).f2);} int Cyc_Absyn_tvar_cmp( struct Cyc_Absyn_Tvar* tv1, struct
Cyc_Absyn_Tvar* tv2){ return Cyc_String_zstrptrcmp( tv1->name, tv2->name);}
static int Cyc_Absyn_new_type_counter= 0; void* Cyc_Absyn_new_evar( void* k){
return( void*)({ struct Cyc_Absyn_Evar_struct* _temp57=( struct Cyc_Absyn_Evar_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Evar_struct)); _temp57[ 0]=({ struct Cyc_Absyn_Evar_struct
_temp58; _temp58.tag= Cyc_Absyn_Evar_tag; _temp58.f1=( void*) k; _temp58.f2= 0;
_temp58.f3=( Cyc_Absyn_new_type_counter ++); _temp58;}); _temp57;});} void* Cyc_Absyn_wildtyp(){
return Cyc_Absyn_new_evar(( void*) Cyc_Absyn_MemKind);} struct Cyc_Absyn_Tqual*
Cyc_Absyn_combine_tqual( struct Cyc_Absyn_Tqual* x, struct Cyc_Absyn_Tqual* y){
return({ struct Cyc_Absyn_Tqual* _temp59=( struct Cyc_Absyn_Tqual*)
GC_malloc_atomic( sizeof( struct Cyc_Absyn_Tqual)); _temp59->q_const= x->q_const?
1: y->q_const; _temp59->q_volatile= x->q_volatile? 1: y->q_volatile; _temp59->q_restrict=
x->q_restrict? 1: y->q_restrict; _temp59;});} struct Cyc_Absyn_Tqual* Cyc_Absyn_empty_tqual(){
return({ struct Cyc_Absyn_Tqual* _temp60=( struct Cyc_Absyn_Tqual*)
GC_malloc_atomic( sizeof( struct Cyc_Absyn_Tqual)); _temp60->q_const= 0; _temp60->q_volatile=
0; _temp60->q_restrict= 0; _temp60;});} int Cyc_Absyn_is_format_prim( void* p){
void* _temp61= p; _LL63: if(( int) _temp61 == Cyc_Absyn_Printf){ goto _LL64;}
else{ goto _LL65;} _LL65: if(( int) _temp61 == Cyc_Absyn_Fprintf){ goto _LL66;}
else{ goto _LL67;} _LL67: if(( int) _temp61 == Cyc_Absyn_Xprintf){ goto _LL68;}
else{ goto _LL69;} _LL69: if(( int) _temp61 == Cyc_Absyn_Scanf){ goto _LL70;}
else{ goto _LL71;} _LL71: if(( int) _temp61 == Cyc_Absyn_Fscanf){ goto _LL72;}
else{ goto _LL73;} _LL73: if(( int) _temp61 == Cyc_Absyn_Sscanf){ goto _LL74;}
else{ goto _LL75;} _LL75: goto _LL76; _LL64: return 1; _LL66: return 1; _LL68:
return 1; _LL70: return 1; _LL72: return 1; _LL74: return 1; _LL76: return 0;
_LL62:;} struct Cyc_Absyn_Conref* Cyc_Absyn_new_conref( void* x){ return({
struct Cyc_Absyn_Conref* _temp77=( struct Cyc_Absyn_Conref*) GC_malloc( sizeof(
struct Cyc_Absyn_Conref)); _temp77->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp78=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp78[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp79; _temp79.tag= Cyc_Absyn_Eq_constr_tag;
_temp79.f1=( void*) x; _temp79;}); _temp78;})); _temp77;});} struct Cyc_Absyn_Conref*
Cyc_Absyn_empty_conref(){ return({ struct Cyc_Absyn_Conref* _temp80=( struct Cyc_Absyn_Conref*)
GC_malloc( sizeof( struct Cyc_Absyn_Conref)); _temp80->v=( void*)(( void*) Cyc_Absyn_No_constr);
_temp80;});} static struct Cyc_Absyn_Eq_constr_struct Cyc_Absyn_true_constraint={
0u,( void*) 1}; static struct Cyc_Absyn_Eq_constr_struct Cyc_Absyn_false_constraint={
0u,( void*) 0}; struct Cyc_Absyn_Conref Cyc_Absyn_true_conref_v=( struct Cyc_Absyn_Conref){.v=(
void*)(( void*)& Cyc_Absyn_true_constraint)}; struct Cyc_Absyn_Conref Cyc_Absyn_false_conref_v=(
struct Cyc_Absyn_Conref){.v=( void*)(( void*)& Cyc_Absyn_false_constraint)};
struct Cyc_Absyn_Conref* Cyc_Absyn_true_conref=& Cyc_Absyn_true_conref_v; struct
Cyc_Absyn_Conref* Cyc_Absyn_false_conref=& Cyc_Absyn_false_conref_v; struct Cyc_Absyn_Conref*
Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref* x){ void* _temp83=( void*) x->v;
void* _temp91; struct Cyc_Absyn_Conref* _temp93; struct Cyc_Absyn_Conref**
_temp95; _LL85: if(( int) _temp83 == Cyc_Absyn_No_constr){ goto _LL86;} else{
goto _LL87;} _LL87: if(( unsigned int) _temp83 > 1u?(( struct _tunion_struct*)
_temp83)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL92: _temp91=( void*)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp83)->f1; goto _LL88;} else{ goto _LL89;} _LL89:
if(( unsigned int) _temp83 > 1u?(( struct _tunion_struct*) _temp83)->tag == Cyc_Absyn_Forward_constr_tag:
0){ _LL94: _temp93=( struct Cyc_Absyn_Conref*)(( struct Cyc_Absyn_Forward_constr_struct*)
_temp83)->f1; _temp95=&(( struct Cyc_Absyn_Forward_constr_struct*) _temp83)->f1;
goto _LL90;} else{ goto _LL84;} _LL86: return x; _LL88: return x; _LL90: {
struct Cyc_Absyn_Conref* z=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)(* _temp95);* _temp95= z; return z;} _LL84:;} void*
Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref* x){ void* _temp96=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
x))->v; void* _temp102; _LL98: if(( unsigned int) _temp96 > 1u?(( struct
_tunion_struct*) _temp96)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL103: _temp102=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp96)->f1; goto _LL99;} else{
goto _LL100;} _LL100: goto _LL101; _LL99: return _temp102; _LL101: return(( void*(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp104=( char*)"conref_val"; struct _tagged_string _temp105; _temp105.curr=
_temp104; _temp105.base= _temp104; _temp105.last_plus_one= _temp104 + 11;
_temp105;})); _LL97:;} static struct Cyc_Absyn_IntType_struct Cyc_Absyn_uchar_tt={
7u,( void*)(( void*) 1u),( void*)(( void*) 0u)}; static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_ushort_tt={ 7u,( void*)(( void*) 1u),( void*)(( void*) 1u)}; static
struct Cyc_Absyn_IntType_struct Cyc_Absyn_uint_tt={ 7u,( void*)(( void*) 1u),(
void*)(( void*) 2u)}; static struct Cyc_Absyn_IntType_struct Cyc_Absyn_ulong_tt={
7u,( void*)(( void*) 1u),( void*)(( void*) 3u)}; void* Cyc_Absyn_uchar_t=( void*)&
Cyc_Absyn_uchar_tt; void* Cyc_Absyn_ushort_t=( void*)& Cyc_Absyn_ushort_tt; void*
Cyc_Absyn_uint_t=( void*)& Cyc_Absyn_uint_tt; void* Cyc_Absyn_ulong_t=( void*)&
Cyc_Absyn_ulong_tt; static struct Cyc_Absyn_IntType_struct Cyc_Absyn_schar_tt={
7u,( void*)(( void*) 0u),( void*)(( void*) 0u)}; static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_sshort_tt={ 7u,( void*)(( void*) 0u),( void*)(( void*) 1u)}; static
struct Cyc_Absyn_IntType_struct Cyc_Absyn_sint_tt={ 7u,( void*)(( void*) 0u),(
void*)(( void*) 2u)}; static struct Cyc_Absyn_IntType_struct Cyc_Absyn_slong_tt={
7u,( void*)(( void*) 0u),( void*)(( void*) 3u)}; void* Cyc_Absyn_schar_t=( void*)&
Cyc_Absyn_schar_tt; void* Cyc_Absyn_sshort_t=( void*)& Cyc_Absyn_sshort_tt; void*
Cyc_Absyn_sint_t=( void*)& Cyc_Absyn_sint_tt; void* Cyc_Absyn_slong_t=( void*)&
Cyc_Absyn_slong_tt; void* Cyc_Absyn_float_t=( void*) 1u; void* Cyc_Absyn_double_t=(
void*) 2u; static char _temp116[ 4u]="exn"; static struct _tagged_string Cyc_Absyn_exn_string=(
struct _tagged_string){ _temp116, _temp116, _temp116 + 4u}; static struct Cyc_Absyn_Abs_n_struct
Cyc_Absyn_abs_null={ 1u, 0}; static struct _tuple0 Cyc_Absyn_exn_name_v=( struct
_tuple0){.f1=( void*)& Cyc_Absyn_abs_null,.f2=& Cyc_Absyn_exn_string}; struct
_tuple0* Cyc_Absyn_exn_name=& Cyc_Absyn_exn_name_v; static struct Cyc_Absyn_XTuniondecl
Cyc_Absyn_exn_xed_v=( struct Cyc_Absyn_XTuniondecl){.sc=( void*)(( void*) 3u),.name=&
Cyc_Absyn_exn_name_v,.fields= 0}; struct Cyc_Absyn_XTuniondecl* Cyc_Absyn_exn_xed=&
Cyc_Absyn_exn_xed_v; static struct Cyc_Absyn_XTunionType_struct Cyc_Absyn_exn_typ_tt={
3u,( struct Cyc_Absyn_XTunionInfo){.name=& Cyc_Absyn_exn_name_v,.rgn=( void*)((
void*) 3u),.xtud=( struct Cyc_Absyn_XTuniondecl*)& Cyc_Absyn_exn_xed_v}}; void*
Cyc_Absyn_exn_typ=( void*)& Cyc_Absyn_exn_typ_tt; static struct Cyc_Core_Opt*
Cyc_Absyn_string_t_opt= 0; void* Cyc_Absyn_string_typ( void* rgn){ if( Cyc_Absyn_string_t_opt
== 0){ void* t= Cyc_Absyn_starb_typ( Cyc_Absyn_uchar_t, rgn, Cyc_Absyn_empty_tqual(),(
void*) Cyc_Absyn_Unknown_b); Cyc_Absyn_string_t_opt=({ struct Cyc_Core_Opt*
_temp119=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp119->v=( void*) t; _temp119;});} return( void*)({ struct Cyc_Core_Opt*
_temp120= Cyc_Absyn_string_t_opt; if( _temp120 == 0){ _throw( Null_Exception);}
_temp120->v;});} void* Cyc_Absyn_starb_typ( void* t, void* r, struct Cyc_Absyn_Tqual*
tq, void* b){ return( void*)({ struct Cyc_Absyn_PointerType_struct* _temp121=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp121[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp122; _temp122.tag= Cyc_Absyn_PointerType_tag;
_temp122.f1=({ struct Cyc_Absyn_PtrInfo _temp123; _temp123.elt_typ=( void*) t;
_temp123.rgn_typ=( void*) r; _temp123.nullable= Cyc_Absyn_true_conref; _temp123.tq=
tq; _temp123.bounds=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
b); _temp123;}); _temp122;}); _temp121;});} void* Cyc_Absyn_atb_typ( void* t,
void* r, struct Cyc_Absyn_Tqual* tq, void* b){ return( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp124=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp124[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp125; _temp125.tag= Cyc_Absyn_PointerType_tag;
_temp125.f1=({ struct Cyc_Absyn_PtrInfo _temp126; _temp126.elt_typ=( void*) t;
_temp126.rgn_typ=( void*) r; _temp126.nullable= Cyc_Absyn_false_conref; _temp126.tq=
tq; _temp126.bounds=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
b); _temp126;}); _temp125;}); _temp124;});} void* Cyc_Absyn_star_typ( void* t,
void* r, struct Cyc_Absyn_Tqual* tq){ return Cyc_Absyn_starb_typ( t, r, tq,(
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp127=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp127[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp128; _temp128.tag= Cyc_Absyn_Upper_b_tag; _temp128.f1= Cyc_Absyn_signed_int_exp(
1, 0); _temp128;}); _temp127;}));} void* Cyc_Absyn_cstar_typ( void* t, struct
Cyc_Absyn_Tqual* tq){ return Cyc_Absyn_starb_typ( t,( void*) Cyc_Absyn_HeapRgn,
tq,( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp129=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp129[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp130; _temp130.tag= Cyc_Absyn_Upper_b_tag; _temp130.f1= Cyc_Absyn_signed_int_exp(
1, 0); _temp130;}); _temp129;}));} void* Cyc_Absyn_at_typ( void* t, void* r,
struct Cyc_Absyn_Tqual* tq){ return Cyc_Absyn_atb_typ( t, r, tq,( void*)({
struct Cyc_Absyn_Upper_b_struct* _temp131=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp131[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp132; _temp132.tag= Cyc_Absyn_Upper_b_tag; _temp132.f1= Cyc_Absyn_signed_int_exp(
1, 0); _temp132;}); _temp131;}));} void* Cyc_Absyn_tagged_typ( void* t, void* r,
struct Cyc_Absyn_Tqual* tq){ return( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp133=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp133[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp134; _temp134.tag= Cyc_Absyn_PointerType_tag;
_temp134.f1=({ struct Cyc_Absyn_PtrInfo _temp135; _temp135.elt_typ=( void*) t;
_temp135.rgn_typ=( void*) r; _temp135.nullable= Cyc_Absyn_true_conref; _temp135.tq=
tq; _temp135.bounds=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*) Cyc_Absyn_Unknown_b); _temp135;}); _temp134;}); _temp133;});} static
struct Cyc_Core_Opt* Cyc_Absyn_file_t_opt= 0; static char _temp138[ 8u]="__sFILE";
static struct _tagged_string Cyc_Absyn_sf_str=( struct _tagged_string){ _temp138,
_temp138, _temp138 + 8u}; static struct _tagged_string* Cyc_Absyn_sf=& Cyc_Absyn_sf_str;
static char _temp141[ 4u]="Cyc"; static struct _tagged_string Cyc_Absyn_cyc_str=(
struct _tagged_string){ _temp141, _temp141, _temp141 + 4u}; static struct
_tagged_string* Cyc_Absyn_cyc=& Cyc_Absyn_cyc_str; static char _temp144[ 6u]="Stdio";
static struct _tagged_string Cyc_Absyn_st_str=( struct _tagged_string){ _temp144,
_temp144, _temp144 + 6u}; static struct _tagged_string* Cyc_Absyn_st=& Cyc_Absyn_st_str;
void* Cyc_Absyn_file_typ(){ if( Cyc_Absyn_file_t_opt == 0){ struct _tuple0*
file_t_name=({ struct _tuple0* _temp145=( struct _tuple0*) GC_malloc( sizeof(
struct _tuple0)); _temp145->f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp146=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp146[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp147; _temp147.tag= Cyc_Absyn_Abs_n_tag;
_temp147.f1=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp148=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp148->hd=( void*)
Cyc_Absyn_cyc; _temp148->tl=({ struct Cyc_List_List* _temp149=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp149->hd=( void*) Cyc_Absyn_st;
_temp149->tl= 0; _temp149;}); _temp148;}); _temp147;}); _temp146;}); _temp145->f2=
Cyc_Absyn_sf; _temp145;}); struct Cyc_Absyn_Structdecl* sd=({ struct Cyc_Absyn_Structdecl*
_temp150=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp150->sc=( void*)(( void*) Cyc_Absyn_Abstract); _temp150->name=({ struct Cyc_Core_Opt*
_temp151=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp151->v=( void*) file_t_name; _temp151;}); _temp150->tvs= 0; _temp150->fields=
0; _temp150->attributes= 0; _temp150;}); void* file_struct_typ=( void*)({ struct
Cyc_Absyn_StructType_struct* _temp152=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp152[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp153; _temp153.tag= Cyc_Absyn_StructType_tag;
_temp153.f1=( struct _tuple0*) file_t_name; _temp153.f2= 0; _temp153.f3=({
struct Cyc_Absyn_Structdecl** _temp154=( struct Cyc_Absyn_Structdecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*) * 1); _temp154[ 0]= sd;
_temp154;}); _temp153;}); _temp152;}); Cyc_Absyn_file_t_opt=({ struct Cyc_Core_Opt*
_temp155=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp155->v=( void*) Cyc_Absyn_at_typ( file_struct_typ,( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual()); _temp155;});} return( void*)({ struct Cyc_Core_Opt*
_temp156= Cyc_Absyn_file_t_opt; if( _temp156 == 0){ _throw( Null_Exception);}
_temp156->v;});} static struct Cyc_Core_Opt* Cyc_Absyn_void_star_t_opt= 0; void*
Cyc_Absyn_void_star_typ(){ if( Cyc_Absyn_void_star_t_opt == 0){ Cyc_Absyn_void_star_t_opt=({
struct Cyc_Core_Opt* _temp157=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp157->v=( void*) Cyc_Absyn_star_typ(( void*) Cyc_Absyn_VoidType,(
void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual()); _temp157;});} return( void*)({
struct Cyc_Core_Opt* _temp158= Cyc_Absyn_void_star_t_opt; if( _temp158 == 0){
_throw( Null_Exception);} _temp158->v;});} void* Cyc_Absyn_strct( struct
_tagged_string* name){ return( void*)({ struct Cyc_Absyn_StructType_struct*
_temp159=( struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp159[ 0]=({ struct Cyc_Absyn_StructType_struct _temp160; _temp160.tag= Cyc_Absyn_StructType_tag;
_temp160.f1=({ struct _tuple0* _temp161=( struct _tuple0*) GC_malloc( sizeof(
struct _tuple0) * 1); _temp161[ 0]=({ struct _tuple0 _temp162; _temp162.f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp163=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp163[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp164; _temp164.tag= Cyc_Absyn_Abs_n_tag; _temp164.f1= 0; _temp164;});
_temp163;}); _temp162.f2= name; _temp162;}); _temp161;}); _temp160.f2= 0;
_temp160.f3= 0; _temp160;}); _temp159;});} void* Cyc_Absyn_union_typ( struct
_tagged_string* name){ return( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp165=( struct Cyc_Absyn_UnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp165[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp166; _temp166.tag= Cyc_Absyn_UnionType_tag;
_temp166.f1=({ struct _tuple0* _temp167=( struct _tuple0*) GC_malloc( sizeof(
struct _tuple0) * 1); _temp167[ 0]=({ struct _tuple0 _temp168; _temp168.f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp169=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp169[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp170; _temp170.tag= Cyc_Absyn_Abs_n_tag; _temp170.f1= 0; _temp170;});
_temp169;}); _temp168.f2= name; _temp168;}); _temp167;}); _temp166.f2= 0;
_temp166.f3= 0; _temp166;}); _temp165;});} void* Cyc_Absyn_strctq( struct
_tuple0* name){ return( void*)({ struct Cyc_Absyn_StructType_struct* _temp171=(
struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp171[ 0]=({ struct Cyc_Absyn_StructType_struct _temp172; _temp172.tag= Cyc_Absyn_StructType_tag;
_temp172.f1=( struct _tuple0*) name; _temp172.f2= 0; _temp172.f3= 0; _temp172;});
_temp171;});} void* Cyc_Absyn_unionq_typ( struct _tuple0* name){ return( void*)({
struct Cyc_Absyn_UnionType_struct* _temp173=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp173[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp174; _temp174.tag= Cyc_Absyn_UnionType_tag;
_temp174.f1=( struct _tuple0*) name; _temp174.f2= 0; _temp174.f3= 0; _temp174;});
_temp173;});} struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp( void* r, struct Cyc_Position_Segment*
loc){ return({ struct Cyc_Absyn_Exp* _temp175=( struct Cyc_Absyn_Exp*) GC_malloc(
sizeof( struct Cyc_Absyn_Exp)); _temp175->topt= 0; _temp175->r=( void*) r;
_temp175->loc= loc; _temp175;});} struct Cyc_Absyn_Exp* Cyc_Absyn_New_exp(
struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_New_e_struct* _temp176=( struct Cyc_Absyn_New_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_New_e_struct)); _temp176[ 0]=({ struct Cyc_Absyn_New_e_struct
_temp177; _temp177.tag= Cyc_Absyn_New_e_tag; _temp177.f1= e; _temp177;});
_temp176;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_copy_exp( struct Cyc_Absyn_Exp*
e){ return({ struct Cyc_Absyn_Exp* _temp178=( struct Cyc_Absyn_Exp*) GC_malloc(
sizeof( struct Cyc_Absyn_Exp)); _temp178->topt= e->topt; _temp178->r=( void*)((
void*) e->r); _temp178->loc= e->loc; _temp178;});} struct Cyc_Absyn_Exp* Cyc_Absyn_const_exp(
void* c, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Const_e_struct* _temp179=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp179[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp180; _temp180.tag= Cyc_Absyn_Const_e_tag; _temp180.f1=( void*) c; _temp180;});
_temp179;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_null_exp( struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp181=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp181[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp182; _temp182.tag= Cyc_Absyn_Const_e_tag;
_temp182.f1=( void*)(( void*) Cyc_Absyn_Null_c); _temp182;}); _temp181;}), loc);}
struct Cyc_Absyn_Exp* Cyc_Absyn_int_exp( void* s, int i, struct Cyc_Position_Segment*
seg){ return Cyc_Absyn_const_exp(( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp183=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp183[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp184; _temp184.tag= Cyc_Absyn_Int_c_tag;
_temp184.f1=( void*) s; _temp184.f2= i; _temp184;}); _temp183;}), seg);} struct
Cyc_Absyn_Exp* Cyc_Absyn_signed_int_exp( int i, struct Cyc_Position_Segment* loc){
return Cyc_Absyn_int_exp(( void*) Cyc_Absyn_Signed, i, loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_uint_exp( unsigned int i, struct Cyc_Position_Segment* loc){ return
Cyc_Absyn_int_exp(( void*) Cyc_Absyn_Unsigned,( int) i, loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_bool_exp( int b, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_signed_int_exp(
b? 1: 0, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_true_exp( struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_bool_exp( 1, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_false_exp(
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_bool_exp( 0, loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_char_exp( char c, struct Cyc_Position_Segment* loc){
return Cyc_Absyn_const_exp(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp185=(
struct Cyc_Absyn_Char_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct));
_temp185[ 0]=({ struct Cyc_Absyn_Char_c_struct _temp186; _temp186.tag= Cyc_Absyn_Char_c_tag;
_temp186.f1=( void*)(( void*) Cyc_Absyn_Unsigned); _temp186.f2= c; _temp186;});
_temp185;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_float_exp( struct
_tagged_string f, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_const_exp((
void*)({ struct Cyc_Absyn_Float_c_struct* _temp187=( struct Cyc_Absyn_Float_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Float_c_struct)); _temp187[ 0]=({ struct Cyc_Absyn_Float_c_struct
_temp188; _temp188.tag= Cyc_Absyn_Float_c_tag; _temp188.f1= f; _temp188;});
_temp187;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_string_exp( struct
_tagged_string s, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_const_exp((
void*)({ struct Cyc_Absyn_String_c_struct* _temp189=( struct Cyc_Absyn_String_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_String_c_struct)); _temp189[ 0]=({ struct
Cyc_Absyn_String_c_struct _temp190; _temp190.tag= Cyc_Absyn_String_c_tag;
_temp190.f1= s; _temp190;}); _temp189;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_var_exp(
struct _tuple0* q, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Var_e_struct* _temp191=( struct Cyc_Absyn_Var_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_e_struct)); _temp191[ 0]=({ struct Cyc_Absyn_Var_e_struct
_temp192; _temp192.tag= Cyc_Absyn_Var_e_tag; _temp192.f1= q; _temp192.f2=( void*)((
void*) Cyc_Absyn_Unresolved_b); _temp192;}); _temp191;}), loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_varb_exp( struct _tuple0* q, void* b, struct Cyc_Position_Segment* loc){
return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Var_e_struct* _temp193=(
struct Cyc_Absyn_Var_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_e_struct));
_temp193[ 0]=({ struct Cyc_Absyn_Var_e_struct _temp194; _temp194.tag= Cyc_Absyn_Var_e_tag;
_temp194.f1= q; _temp194.f2=( void*) b; _temp194;}); _temp193;}), loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_unknownid_exp( struct _tuple0* q, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct*
_temp195=( struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp195[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp196; _temp196.tag= Cyc_Absyn_UnknownId_e_tag;
_temp196.f1= q; _temp196;}); _temp195;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_primop_exp(
void* p, struct Cyc_List_List* es, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Primop_e_struct* _temp197=( struct Cyc_Absyn_Primop_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp197[ 0]=({ struct
Cyc_Absyn_Primop_e_struct _temp198; _temp198.tag= Cyc_Absyn_Primop_e_tag;
_temp198.f1=( void*) p; _temp198.f2= es; _temp198;}); _temp197;}), loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_prim1_exp( void* p, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_primop_exp( p,({ struct Cyc_List_List* _temp199=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp199->hd=( void*)
e; _temp199->tl= 0; _temp199;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_prim2_exp(
void* p, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_primop_exp( p,({ struct Cyc_List_List* _temp200=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp200->hd=( void*)
e1; _temp200->tl=({ struct Cyc_List_List* _temp201=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp201->hd=( void*) e2; _temp201->tl=
0; _temp201;}); _temp200;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_add_exp(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Plus, e1, e2, loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_subtract_exp( struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp*
e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Minus,
e1, e2, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_times_exp( struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Times, e1, e2, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_divide_exp(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Div, e1, e2, loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_eq_exp( struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp*
e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Eq,
e1, e2, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_neq_exp( struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Neq, e1, e2, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_gt_exp(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Gt, e1, e2, loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_lt_exp( struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp*
e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Lt,
e1, e2, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_gte_exp( struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Gte, e1, e2, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_lte_exp(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Lte, e1, e2, loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_assignop_exp( struct Cyc_Absyn_Exp* e1, struct Cyc_Core_Opt*
popt, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_AssignOp_e_struct* _temp202=( struct Cyc_Absyn_AssignOp_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AssignOp_e_struct)); _temp202[ 0]=({ struct
Cyc_Absyn_AssignOp_e_struct _temp203; _temp203.tag= Cyc_Absyn_AssignOp_e_tag;
_temp203.f1= e1; _temp203.f2= popt; _temp203.f3= e2; _temp203;}); _temp202;}),
loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_assign_exp( struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_assignop_exp(
e1, 0, e2, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_increment_exp( struct Cyc_Absyn_Exp*
e, void* i, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Increment_e_struct* _temp204=( struct Cyc_Absyn_Increment_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Increment_e_struct)); _temp204[ 0]=({ struct
Cyc_Absyn_Increment_e_struct _temp205; _temp205.tag= Cyc_Absyn_Increment_e_tag;
_temp205.f1= e; _temp205.f2=( void*) i; _temp205;}); _temp204;}), loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_post_inc_exp( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_increment_exp( e,( void*) Cyc_Absyn_PostInc, loc);}
struct Cyc_Absyn_Exp* Cyc_Absyn_pre_inc_exp( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_increment_exp( e,( void*) Cyc_Absyn_PreInc, loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_pre_dec_exp( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_increment_exp( e,( void*) Cyc_Absyn_PreDec, loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_post_dec_exp( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_increment_exp( e,( void*) Cyc_Absyn_PostDec, loc);}
struct Cyc_Absyn_Exp* Cyc_Absyn_conditional_exp( struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Conditional_e_struct*
_temp206=( struct Cyc_Absyn_Conditional_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Conditional_e_struct));
_temp206[ 0]=({ struct Cyc_Absyn_Conditional_e_struct _temp207; _temp207.tag=
Cyc_Absyn_Conditional_e_tag; _temp207.f1= e1; _temp207.f2= e2; _temp207.f3= e3;
_temp207;}); _temp206;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_and_exp( struct
Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc){
return Cyc_Absyn_conditional_exp( e1, e2, Cyc_Absyn_false_exp( loc), loc);}
struct Cyc_Absyn_Exp* Cyc_Absyn_or_exp( struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp*
e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_conditional_exp( e1, Cyc_Absyn_true_exp(
loc), e2, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_seq_exp( struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_SeqExp_e_struct* _temp208=( struct Cyc_Absyn_SeqExp_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_SeqExp_e_struct)); _temp208[ 0]=({ struct
Cyc_Absyn_SeqExp_e_struct _temp209; _temp209.tag= Cyc_Absyn_SeqExp_e_tag;
_temp209.f1= e1; _temp209.f2= e2; _temp209;}); _temp208;}), loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_unknowncall_exp( struct Cyc_Absyn_Exp* e, struct Cyc_List_List* es,
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(( void*)({ struct
Cyc_Absyn_UnknownCall_e_struct* _temp210=( struct Cyc_Absyn_UnknownCall_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownCall_e_struct)); _temp210[ 0]=({
struct Cyc_Absyn_UnknownCall_e_struct _temp211; _temp211.tag= Cyc_Absyn_UnknownCall_e_tag;
_temp211.f1= e; _temp211.f2= es; _temp211;}); _temp210;}), loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_fncall_exp( struct Cyc_Absyn_Exp* e, struct Cyc_List_List* es, struct
Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_FnCall_e_struct*
_temp212=( struct Cyc_Absyn_FnCall_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct));
_temp212[ 0]=({ struct Cyc_Absyn_FnCall_e_struct _temp213; _temp213.tag= Cyc_Absyn_FnCall_e_tag;
_temp213.f1= e; _temp213.f2= es; _temp213;}); _temp212;}), loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_noinstantiate_exp( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_NoInstantiate_e_struct*
_temp214=( struct Cyc_Absyn_NoInstantiate_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NoInstantiate_e_struct)); _temp214[ 0]=({ struct Cyc_Absyn_NoInstantiate_e_struct
_temp215; _temp215.tag= Cyc_Absyn_NoInstantiate_e_tag; _temp215.f1= e; _temp215;});
_temp214;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_instantiate_exp( struct Cyc_Absyn_Exp*
e, struct Cyc_List_List* ts, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Instantiate_e_struct* _temp216=( struct Cyc_Absyn_Instantiate_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Instantiate_e_struct)); _temp216[ 0]=({
struct Cyc_Absyn_Instantiate_e_struct _temp217; _temp217.tag= Cyc_Absyn_Instantiate_e_tag;
_temp217.f1= e; _temp217.f2= ts; _temp217;}); _temp216;}), loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_cast_exp( void* t, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Cast_e_struct*
_temp218=( struct Cyc_Absyn_Cast_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct));
_temp218[ 0]=({ struct Cyc_Absyn_Cast_e_struct _temp219; _temp219.tag= Cyc_Absyn_Cast_e_tag;
_temp219.f1=( void*) t; _temp219.f2= e; _temp219;}); _temp218;}), loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_throw_exp( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Throw_e_struct*
_temp220=( struct Cyc_Absyn_Throw_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Throw_e_struct));
_temp220[ 0]=({ struct Cyc_Absyn_Throw_e_struct _temp221; _temp221.tag= Cyc_Absyn_Throw_e_tag;
_temp221.f1= e; _temp221;}); _temp220;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_address_exp(
struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Address_e_struct* _temp222=( struct Cyc_Absyn_Address_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Address_e_struct)); _temp222[ 0]=({ struct
Cyc_Absyn_Address_e_struct _temp223; _temp223.tag= Cyc_Absyn_Address_e_tag;
_temp223.f1= e; _temp223;}); _temp222;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_sizeoftyp_exp(
void* t, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Sizeoftyp_e_struct* _temp224=( struct Cyc_Absyn_Sizeoftyp_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Sizeoftyp_e_struct)); _temp224[ 0]=({ struct
Cyc_Absyn_Sizeoftyp_e_struct _temp225; _temp225.tag= Cyc_Absyn_Sizeoftyp_e_tag;
_temp225.f1=( void*) t; _temp225;}); _temp224;}), loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_sizeofexp_exp( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Sizeofexp_e_struct*
_temp226=( struct Cyc_Absyn_Sizeofexp_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Sizeofexp_e_struct));
_temp226[ 0]=({ struct Cyc_Absyn_Sizeofexp_e_struct _temp227; _temp227.tag= Cyc_Absyn_Sizeofexp_e_tag;
_temp227.f1= e; _temp227;}); _temp226;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_deref_exp(
struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Deref_e_struct* _temp228=( struct Cyc_Absyn_Deref_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Deref_e_struct)); _temp228[ 0]=({ struct Cyc_Absyn_Deref_e_struct
_temp229; _temp229.tag= Cyc_Absyn_Deref_e_tag; _temp229.f1= e; _temp229;});
_temp228;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_structmember_exp( struct Cyc_Absyn_Exp*
e, struct _tagged_string* n, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_StructMember_e_struct* _temp230=( struct Cyc_Absyn_StructMember_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructMember_e_struct)); _temp230[ 0]=({
struct Cyc_Absyn_StructMember_e_struct _temp231; _temp231.tag= Cyc_Absyn_StructMember_e_tag;
_temp231.f1= e; _temp231.f2= n; _temp231;}); _temp230;}), loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_structarrow_exp( struct Cyc_Absyn_Exp* e, struct _tagged_string* n,
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(( void*)({ struct
Cyc_Absyn_StructArrow_e_struct* _temp232=( struct Cyc_Absyn_StructArrow_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructArrow_e_struct)); _temp232[ 0]=({
struct Cyc_Absyn_StructArrow_e_struct _temp233; _temp233.tag= Cyc_Absyn_StructArrow_e_tag;
_temp233.f1= e; _temp233.f2= n; _temp233;}); _temp232;}), loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_arrow_exp( struct Cyc_Absyn_Exp* e, struct _tagged_string* n, struct
Cyc_Position_Segment* loc){ return Cyc_Absyn_deref_exp( Cyc_Absyn_structmember_exp(
e, n, loc), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_subscript_exp( struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Subscript_e_struct* _temp234=( struct Cyc_Absyn_Subscript_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Subscript_e_struct)); _temp234[ 0]=({ struct
Cyc_Absyn_Subscript_e_struct _temp235; _temp235.tag= Cyc_Absyn_Subscript_e_tag;
_temp235.f1= e1; _temp235.f2= e2; _temp235;}); _temp234;}), loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_tuple_exp( struct Cyc_List_List* es, struct Cyc_Position_Segment* loc){
return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Tuple_e_struct* _temp236=(
struct Cyc_Absyn_Tuple_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tuple_e_struct));
_temp236[ 0]=({ struct Cyc_Absyn_Tuple_e_struct _temp237; _temp237.tag= Cyc_Absyn_Tuple_e_tag;
_temp237.f1= es; _temp237;}); _temp236;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_stmt_exp(
struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_StmtExp_e_struct* _temp238=( struct Cyc_Absyn_StmtExp_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StmtExp_e_struct)); _temp238[ 0]=({ struct
Cyc_Absyn_StmtExp_e_struct _temp239; _temp239.tag= Cyc_Absyn_StmtExp_e_tag;
_temp239.f1= s; _temp239;}); _temp238;}), loc);} static char _temp242[ 15u]="Null_Exception";
static struct _tagged_string Cyc_Absyn_ne_str=( struct _tagged_string){ _temp242,
_temp242, _temp242 + 15u}; static struct _tagged_string* Cyc_Absyn_ne=& Cyc_Absyn_ne_str;
struct Cyc_Absyn_Exp* Cyc_Absyn_null_pointer_exn_exp( struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_var_exp(({ struct _tuple0* _temp243=( struct _tuple0*)
GC_malloc( sizeof( struct _tuple0)); _temp243->f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp244=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp244[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp245; _temp245.tag= Cyc_Absyn_Abs_n_tag;
_temp245.f1= 0; _temp245;}); _temp244;}); _temp243->f2= Cyc_Absyn_ne; _temp243;}),
loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_array_exp( struct Cyc_List_List* es,
struct Cyc_Position_Segment* loc){ struct Cyc_List_List* dles= 0; for( 0; es !=
0; es=({ struct Cyc_List_List* _temp246= es; if( _temp246 == 0){ _throw(
Null_Exception);} _temp246->tl;})){ dles=({ struct Cyc_List_List* _temp247=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp247->hd=(
void*)({ struct _tuple5* _temp248=( struct _tuple5*) GC_malloc( sizeof( struct
_tuple5)); _temp248->f1= 0; _temp248->f2=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp249= es; if( _temp249 == 0){ _throw( Null_Exception);} _temp249->hd;});
_temp248;}); _temp247->tl= dles; _temp247;});} dles=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( dles); return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Array_e_struct* _temp250=( struct Cyc_Absyn_Array_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct)); _temp250[ 0]=({ struct Cyc_Absyn_Array_e_struct
_temp251; _temp251.tag= Cyc_Absyn_Array_e_tag; _temp251.f1= dles; _temp251;});
_temp250;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_unresolvedmem_exp( struct
Cyc_Core_Opt* n, struct Cyc_List_List* dles, struct Cyc_Position_Segment* loc){
return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp252=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp252[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp253; _temp253.tag= Cyc_Absyn_UnresolvedMem_e_tag; _temp253.f1= n; _temp253.f2=
dles; _temp253;}); _temp252;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_new_stmt(
void* s, struct Cyc_Position_Segment* loc){ return({ struct Cyc_Absyn_Stmt*
_temp254=( struct Cyc_Absyn_Stmt*) GC_malloc( sizeof( struct Cyc_Absyn_Stmt));
_temp254->r=( void*) s; _temp254->loc= loc; _temp254->non_local_preds= 0;
_temp254->try_depth= 0; _temp254->annot=( struct _xtunion_struct*)({ struct Cyc_Absyn_EmptyAnnot_struct*
_temp255=( struct Cyc_Absyn_EmptyAnnot_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EmptyAnnot_struct));
_temp255[ 0]=({ struct Cyc_Absyn_EmptyAnnot_struct _temp256; _temp256.tag= Cyc_Absyn_EmptyAnnot_tag;
_temp256;}); _temp255;}); _temp254;});} struct Cyc_Absyn_Stmt* Cyc_Absyn_skip_stmt(
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(( void*) Cyc_Absyn_Skip_s,
loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_exp_stmt( struct Cyc_Absyn_Exp* e,
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(( void*)({ struct
Cyc_Absyn_Exp_s_struct* _temp257=( struct Cyc_Absyn_Exp_s_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Exp_s_struct)); _temp257[ 0]=({ struct Cyc_Absyn_Exp_s_struct
_temp258; _temp258.tag= Cyc_Absyn_Exp_s_tag; _temp258.f1= e; _temp258;});
_temp257;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_seq_stmts( struct Cyc_List_List*
ss, struct Cyc_Position_Segment* loc){ if( ss == 0){ return Cyc_Absyn_skip_stmt(
loc);} else{ if(({ struct Cyc_List_List* _temp259= ss; if( _temp259 == 0){
_throw( Null_Exception);} _temp259->tl;}) == 0){ return( struct Cyc_Absyn_Stmt*)({
struct Cyc_List_List* _temp260= ss; if( _temp260 == 0){ _throw( Null_Exception);}
_temp260->hd;});} else{ return Cyc_Absyn_seq_stmt(( struct Cyc_Absyn_Stmt*)({
struct Cyc_List_List* _temp261= ss; if( _temp261 == 0){ _throw( Null_Exception);}
_temp261->hd;}), Cyc_Absyn_seq_stmts(({ struct Cyc_List_List* _temp262= ss; if(
_temp262 == 0){ _throw( Null_Exception);} _temp262->tl;}), loc), loc);}}} struct
Cyc_Absyn_Stmt* Cyc_Absyn_return_stmt( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Return_s_struct*
_temp263=( struct Cyc_Absyn_Return_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Return_s_struct));
_temp263[ 0]=({ struct Cyc_Absyn_Return_s_struct _temp264; _temp264.tag= Cyc_Absyn_Return_s_tag;
_temp264.f1= e; _temp264;}); _temp263;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_ifthenelse_stmt(
struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Stmt* s1, struct Cyc_Absyn_Stmt* s2,
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(( void*)({ struct
Cyc_Absyn_IfThenElse_s_struct* _temp265=( struct Cyc_Absyn_IfThenElse_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IfThenElse_s_struct)); _temp265[ 0]=({
struct Cyc_Absyn_IfThenElse_s_struct _temp266; _temp266.tag= Cyc_Absyn_IfThenElse_s_tag;
_temp266.f1= e; _temp266.f2= s1; _temp266.f3= s2; _temp266;}); _temp265;}), loc);}
struct Cyc_Absyn_Stmt* Cyc_Absyn_while_stmt( struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Stmt*
s, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(( void*)({
struct Cyc_Absyn_While_s_struct* _temp267=( struct Cyc_Absyn_While_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_While_s_struct)); _temp267[ 0]=({ struct Cyc_Absyn_While_s_struct
_temp268; _temp268.tag= Cyc_Absyn_While_s_tag; _temp268.f1=({ struct _tuple2
_temp269; _temp269.f1= e; _temp269.f2= Cyc_Absyn_skip_stmt( e->loc); _temp269;});
_temp268.f2= s; _temp268;}); _temp267;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_break_stmt(
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(( void*)({ struct
Cyc_Absyn_Break_s_struct* _temp270=( struct Cyc_Absyn_Break_s_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Break_s_struct)); _temp270[ 0]=({ struct Cyc_Absyn_Break_s_struct
_temp271; _temp271.tag= Cyc_Absyn_Break_s_tag; _temp271.f1= 0; _temp271;});
_temp270;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_continue_stmt( struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Continue_s_struct*
_temp272=( struct Cyc_Absyn_Continue_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Continue_s_struct));
_temp272[ 0]=({ struct Cyc_Absyn_Continue_s_struct _temp273; _temp273.tag= Cyc_Absyn_Continue_s_tag;
_temp273.f1= 0; _temp273;}); _temp272;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_for_stmt(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3,
struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt((
void*)({ struct Cyc_Absyn_For_s_struct* _temp274=( struct Cyc_Absyn_For_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_For_s_struct)); _temp274[ 0]=({ struct Cyc_Absyn_For_s_struct
_temp275; _temp275.tag= Cyc_Absyn_For_s_tag; _temp275.f1= e1; _temp275.f2=({
struct _tuple2 _temp276; _temp276.f1= e2; _temp276.f2= Cyc_Absyn_skip_stmt( e3->loc);
_temp276;}); _temp275.f3=({ struct _tuple2 _temp277; _temp277.f1= e3; _temp277.f2=
Cyc_Absyn_skip_stmt( e3->loc); _temp277;}); _temp275.f4= s; _temp275;});
_temp274;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_switch_stmt( struct Cyc_Absyn_Exp*
e, struct Cyc_List_List* scs, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt((
void*)({ struct Cyc_Absyn_Switch_s_struct* _temp278=( struct Cyc_Absyn_Switch_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_s_struct)); _temp278[ 0]=({ struct
Cyc_Absyn_Switch_s_struct _temp279; _temp279.tag= Cyc_Absyn_Switch_s_tag;
_temp279.f1= e; _temp279.f2= scs; _temp279;}); _temp278;}), loc);} struct Cyc_Absyn_Stmt*
Cyc_Absyn_seq_stmt( struct Cyc_Absyn_Stmt* s1, struct Cyc_Absyn_Stmt* s2, struct
Cyc_Position_Segment* loc){ void* _temp280=( void*) s1->r; _LL282: if(( int)
_temp280 == Cyc_Absyn_Skip_s){ goto _LL283;} else{ goto _LL284;} _LL284: goto
_LL285; _LL283: return s2; _LL285: return Cyc_Absyn_new_stmt(( void*)({ struct
Cyc_Absyn_Seq_s_struct* _temp286=( struct Cyc_Absyn_Seq_s_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Seq_s_struct)); _temp286[ 0]=({ struct Cyc_Absyn_Seq_s_struct
_temp287; _temp287.tag= Cyc_Absyn_Seq_s_tag; _temp287.f1= s1; _temp287.f2= s2;
_temp287;}); _temp286;}), loc); _LL281:;} struct Cyc_Absyn_Stmt* Cyc_Absyn_fallthru_stmt(
struct Cyc_List_List* el, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt((
void*)({ struct Cyc_Absyn_Fallthru_s_struct* _temp288=( struct Cyc_Absyn_Fallthru_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fallthru_s_struct)); _temp288[ 0]=({ struct
Cyc_Absyn_Fallthru_s_struct _temp289; _temp289.tag= Cyc_Absyn_Fallthru_s_tag;
_temp289.f1= el; _temp289.f2= 0; _temp289;}); _temp288;}), loc);} struct Cyc_Absyn_Stmt*
Cyc_Absyn_decl_stmt( struct Cyc_Absyn_Decl* d, struct Cyc_Absyn_Stmt* s, struct
Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Decl_s_struct*
_temp290=( struct Cyc_Absyn_Decl_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Decl_s_struct));
_temp290[ 0]=({ struct Cyc_Absyn_Decl_s_struct _temp291; _temp291.tag= Cyc_Absyn_Decl_s_tag;
_temp291.f1= d; _temp291.f2= s; _temp291;}); _temp290;}), loc);} struct Cyc_Absyn_Stmt*
Cyc_Absyn_declare_stmt( struct _tuple0* x, void* t, struct Cyc_Absyn_Exp* init,
struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc){ struct Cyc_Absyn_Decl*
d= Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp292=(
struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp292[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp293; _temp293.tag= Cyc_Absyn_Var_d_tag;
_temp293.f1= Cyc_Absyn_new_vardecl( x, t, init); _temp293;}); _temp292;}), loc);
return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Decl_s_struct* _temp294=(
struct Cyc_Absyn_Decl_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Decl_s_struct));
_temp294[ 0]=({ struct Cyc_Absyn_Decl_s_struct _temp295; _temp295.tag= Cyc_Absyn_Decl_s_tag;
_temp295.f1= d; _temp295.f2= s; _temp295;}); _temp294;}), loc);} struct Cyc_Absyn_Stmt*
Cyc_Absyn_cut_stmt( struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc){
return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Cut_s_struct* _temp296=(
struct Cyc_Absyn_Cut_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Cut_s_struct));
_temp296[ 0]=({ struct Cyc_Absyn_Cut_s_struct _temp297; _temp297.tag= Cyc_Absyn_Cut_s_tag;
_temp297.f1= s; _temp297;}); _temp296;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_splice_stmt(
struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt((
void*)({ struct Cyc_Absyn_Splice_s_struct* _temp298=( struct Cyc_Absyn_Splice_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Splice_s_struct)); _temp298[ 0]=({ struct
Cyc_Absyn_Splice_s_struct _temp299; _temp299.tag= Cyc_Absyn_Splice_s_tag;
_temp299.f1= s; _temp299;}); _temp298;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_label_stmt(
struct _tagged_string* v, struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Label_s_struct*
_temp300=( struct Cyc_Absyn_Label_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Label_s_struct));
_temp300[ 0]=({ struct Cyc_Absyn_Label_s_struct _temp301; _temp301.tag= Cyc_Absyn_Label_s_tag;
_temp301.f1= v; _temp301.f2= s; _temp301;}); _temp300;}), loc);} struct Cyc_Absyn_Stmt*
Cyc_Absyn_do_stmt( struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Do_s_struct*
_temp302=( struct Cyc_Absyn_Do_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Do_s_struct));
_temp302[ 0]=({ struct Cyc_Absyn_Do_s_struct _temp303; _temp303.tag= Cyc_Absyn_Do_s_tag;
_temp303.f1= s; _temp303.f2=({ struct _tuple2 _temp304; _temp304.f1= e; _temp304.f2=
Cyc_Absyn_skip_stmt( e->loc); _temp304;}); _temp303;}); _temp302;}), loc);}
struct Cyc_Absyn_Stmt* Cyc_Absyn_trycatch_stmt( struct Cyc_Absyn_Stmt* s, struct
Cyc_List_List* scs, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt((
void*)({ struct Cyc_Absyn_TryCatch_s_struct* _temp305=( struct Cyc_Absyn_TryCatch_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TryCatch_s_struct)); _temp305[ 0]=({ struct
Cyc_Absyn_TryCatch_s_struct _temp306; _temp306.tag= Cyc_Absyn_TryCatch_s_tag;
_temp306.f1= s; _temp306.f2= scs; _temp306;}); _temp305;}), loc);} struct Cyc_Absyn_Stmt*
Cyc_Absyn_goto_stmt( struct _tagged_string* lab, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Goto_s_struct*
_temp307=( struct Cyc_Absyn_Goto_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Goto_s_struct));
_temp307[ 0]=({ struct Cyc_Absyn_Goto_s_struct _temp308; _temp308.tag= Cyc_Absyn_Goto_s_tag;
_temp308.f1= lab; _temp308.f2= 0; _temp308;}); _temp307;}), loc);} struct Cyc_Absyn_Stmt*
Cyc_Absyn_assign_stmt( struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2,
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
e1, e2, loc), loc);} struct Cyc_Absyn_Pat* Cyc_Absyn_new_pat( void* p, struct
Cyc_Position_Segment* s){ return({ struct Cyc_Absyn_Pat* _temp309=( struct Cyc_Absyn_Pat*)
GC_malloc( sizeof( struct Cyc_Absyn_Pat)); _temp309->r=( void*) p; _temp309->topt=
0; _temp309->loc= s; _temp309;});} struct Cyc_Absyn_Decl* Cyc_Absyn_new_decl(
void* r, struct Cyc_Position_Segment* loc){ return({ struct Cyc_Absyn_Decl*
_temp310=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp310->r=( void*) r; _temp310->loc= loc; _temp310;});} struct Cyc_Absyn_Decl*
Cyc_Absyn_let_decl( struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* t_opt, struct
Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Let_d_struct* _temp311=( struct Cyc_Absyn_Let_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Let_d_struct)); _temp311[ 0]=({ struct Cyc_Absyn_Let_d_struct
_temp312; _temp312.tag= Cyc_Absyn_Let_d_tag; _temp312.f1= p; _temp312.f2= 0;
_temp312.f3= t_opt; _temp312.f4= e; _temp312.f5= 0; _temp312;}); _temp311;}),
loc);} struct Cyc_Absyn_Vardecl* Cyc_Absyn_new_vardecl( struct _tuple0* x, void*
t, struct Cyc_Absyn_Exp* init){ return({ struct Cyc_Absyn_Vardecl* _temp313=(
struct Cyc_Absyn_Vardecl*) GC_malloc( sizeof( struct Cyc_Absyn_Vardecl));
_temp313->sc=( void*)(( void*) Cyc_Absyn_Public); _temp313->name= x; _temp313->tq=
Cyc_Absyn_empty_tqual(); _temp313->type=( void*) t; _temp313->initializer= init;
_temp313->shadow= 0; _temp313->region= 0; _temp313->attributes= 0; _temp313;});}
struct Cyc_Absyn_Vardecl* Cyc_Absyn_static_vardecl( struct _tuple0* x, void* t,
struct Cyc_Absyn_Exp* init){ return({ struct Cyc_Absyn_Vardecl* _temp314=(
struct Cyc_Absyn_Vardecl*) GC_malloc( sizeof( struct Cyc_Absyn_Vardecl));
_temp314->sc=( void*)(( void*) Cyc_Absyn_Static); _temp314->name= x; _temp314->tq=
Cyc_Absyn_empty_tqual(); _temp314->type=( void*) t; _temp314->initializer= init;
_temp314->shadow= 0; _temp314->region= 0; _temp314->attributes= 0; _temp314;});}
struct Cyc_Absyn_Decl* Cyc_Absyn_struct_decl( void* s, struct Cyc_Core_Opt* n,
struct Cyc_List_List* ts, struct Cyc_Core_Opt* fs, struct Cyc_List_List* atts,
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_decl(( void*)({ struct
Cyc_Absyn_Struct_d_struct* _temp315=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp315[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp316; _temp316.tag= Cyc_Absyn_Struct_d_tag;
_temp316.f1=({ struct Cyc_Absyn_Structdecl* _temp317=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp317->sc=( void*) s;
_temp317->name= n; _temp317->tvs= ts; _temp317->fields= fs; _temp317->attributes=
atts; _temp317;}); _temp316;}); _temp315;}), loc);} struct Cyc_Absyn_Decl* Cyc_Absyn_union_decl(
void* s, struct Cyc_Core_Opt* n, struct Cyc_List_List* ts, struct Cyc_Core_Opt*
fs, struct Cyc_List_List* atts, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Union_d_struct* _temp318=( struct Cyc_Absyn_Union_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct)); _temp318[ 0]=({ struct Cyc_Absyn_Union_d_struct
_temp319; _temp319.tag= Cyc_Absyn_Union_d_tag; _temp319.f1=({ struct Cyc_Absyn_Uniondecl*
_temp320=( struct Cyc_Absyn_Uniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl));
_temp320->sc=( void*) s; _temp320->name= n; _temp320->tvs= ts; _temp320->fields=
fs; _temp320->attributes= atts; _temp320;}); _temp319;}); _temp318;}), loc);}
struct Cyc_Absyn_Decl* Cyc_Absyn_tunion_decl( void* s, struct Cyc_Core_Opt* n,
struct Cyc_List_List* ts, struct Cyc_Core_Opt* fs, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct*
_temp321=( struct Cyc_Absyn_Tunion_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp321[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp322; _temp322.tag= Cyc_Absyn_Tunion_d_tag;
_temp322.f1=({ struct Cyc_Absyn_Tuniondecl* _temp323=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp323->sc=( void*) s;
_temp323->name= n; _temp323->tvs= ts; _temp323->fields= fs; _temp323;});
_temp322;}); _temp321;}), loc);} struct Cyc_Absyn_Decl* Cyc_Absyn_xtunion_decl(
void* s, struct _tuple0* n, struct Cyc_List_List* fs, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_XTunion_d_struct*
_temp324=( struct Cyc_Absyn_XTunion_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_XTunion_d_struct));
_temp324[ 0]=({ struct Cyc_Absyn_XTunion_d_struct _temp325; _temp325.tag= Cyc_Absyn_XTunion_d_tag;
_temp325.f1=({ struct Cyc_Absyn_XTuniondecl* _temp326=( struct Cyc_Absyn_XTuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_XTuniondecl)); _temp326->sc=( void*) s;
_temp326->name= n; _temp326->fields= fs; _temp326;}); _temp325;}); _temp324;}),
loc);} static struct _tuple1* Cyc_Absyn_expand_arg( struct _tuple1* a){ return({
struct _tuple1* _temp327=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp327->f1=(* a).f1; _temp327->f2=(* a).f2; _temp327->f3= Cyc_Absyn_pointer_expand((*
a).f3); _temp327;});} void* Cyc_Absyn_function_typ( struct Cyc_List_List* tvs,
struct Cyc_Core_Opt* eff_typ, void* ret_typ, struct Cyc_List_List* args, int
varargs, struct Cyc_List_List* atts){ return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp328=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp328[ 0]=({ struct Cyc_Absyn_FnType_struct _temp329; _temp329.tag= Cyc_Absyn_FnType_tag;
_temp329.f1=({ struct Cyc_Absyn_FnInfo _temp330; _temp330.tvars= tvs; _temp330.ret_typ=(
void*) Cyc_Absyn_pointer_expand( ret_typ); _temp330.effect= eff_typ; _temp330.args=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple1*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absyn_expand_arg, args); _temp330.varargs= varargs;
_temp330.attributes= atts; _temp330;}); _temp329;}); _temp328;});} void* Cyc_Absyn_pointer_expand(
void* t){ void* _temp331= t; struct Cyc_Absyn_FnInfo _temp337; _LL333: if((
unsigned int) _temp331 > 4u?(( struct _tunion_struct*) _temp331)->tag == Cyc_Absyn_FnType_tag:
0){ _LL338: _temp337=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp331)->f1; goto _LL334;} else{ goto _LL335;} _LL335: goto _LL336; _LL334:
return Cyc_Absyn_at_typ( t,( void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual());
_LL336: return t; _LL332:;} int Cyc_Absyn_is_lvalue( struct Cyc_Absyn_Exp* e){
void* _temp339=( void*) e->r; void* _temp359; struct Cyc_Absyn_Fndecl* _temp361;
struct _tuple0* _temp363; void* _temp365; struct Cyc_Absyn_Vardecl* _temp367;
struct _tuple0* _temp369; void* _temp371; struct Cyc_Absyn_Vardecl* _temp373;
struct _tuple0* _temp375; void* _temp377; struct _tuple0* _temp379; struct Cyc_Absyn_Exp*
_temp381; struct Cyc_Absyn_Exp* _temp383; struct _tagged_string* _temp385;
struct Cyc_Absyn_Exp* _temp387; struct _tagged_string* _temp389; struct Cyc_Absyn_Exp*
_temp391; struct Cyc_Absyn_Exp* _temp393; _LL341: if((( struct _tunion_struct*)
_temp339)->tag == Cyc_Absyn_Var_e_tag){ _LL364: _temp363=( struct _tuple0*)((
struct Cyc_Absyn_Var_e_struct*) _temp339)->f1; goto _LL360; _LL360: _temp359=(
void*)(( struct Cyc_Absyn_Var_e_struct*) _temp339)->f2; if(( unsigned int)
_temp359 > 1u?(( struct _tunion_struct*) _temp359)->tag == Cyc_Absyn_Funname_b_tag:
0){ _LL362: _temp361=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Funname_b_struct*)
_temp359)->f1; goto _LL342;} else{ goto _LL343;}} else{ goto _LL343;} _LL343:
if((( struct _tunion_struct*) _temp339)->tag == Cyc_Absyn_Var_e_tag){ _LL370:
_temp369=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*) _temp339)->f1;
goto _LL366; _LL366: _temp365=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp339)->f2; if(( unsigned int) _temp365 > 1u?(( struct _tunion_struct*)
_temp365)->tag == Cyc_Absyn_Global_b_tag: 0){ _LL368: _temp367=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Global_b_struct*) _temp365)->f1; goto _LL344;} else{ goto
_LL345;}} else{ goto _LL345;} _LL345: if((( struct _tunion_struct*) _temp339)->tag
== Cyc_Absyn_Var_e_tag){ _LL376: _temp375=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*)
_temp339)->f1; goto _LL372; _LL372: _temp371=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp339)->f2; if(( unsigned int) _temp371 > 1u?(( struct _tunion_struct*)
_temp371)->tag == Cyc_Absyn_Local_b_tag: 0){ _LL374: _temp373=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Local_b_struct*) _temp371)->f1; goto _LL346;} else{ goto _LL347;}}
else{ goto _LL347;} _LL347: if((( struct _tunion_struct*) _temp339)->tag == Cyc_Absyn_Var_e_tag){
_LL380: _temp379=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*) _temp339)->f1;
goto _LL378; _LL378: _temp377=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp339)->f2; goto _LL348;} else{ goto _LL349;} _LL349: if((( struct
_tunion_struct*) _temp339)->tag == Cyc_Absyn_Subscript_e_tag){ _LL384: _temp383=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*) _temp339)->f1;
goto _LL382; _LL382: _temp381=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp339)->f2; goto _LL350;} else{ goto _LL351;} _LL351: if((( struct
_tunion_struct*) _temp339)->tag == Cyc_Absyn_StructMember_e_tag){ _LL388:
_temp387=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp339)->f1; goto _LL386; _LL386: _temp385=( struct _tagged_string*)(( struct
Cyc_Absyn_StructMember_e_struct*) _temp339)->f2; goto _LL352;} else{ goto _LL353;}
_LL353: if((( struct _tunion_struct*) _temp339)->tag == Cyc_Absyn_StructArrow_e_tag){
_LL392: _temp391=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp339)->f1; goto _LL390; _LL390: _temp389=( struct _tagged_string*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp339)->f2; goto _LL354;} else{ goto _LL355;}
_LL355: if((( struct _tunion_struct*) _temp339)->tag == Cyc_Absyn_Deref_e_tag){
_LL394: _temp393=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp339)->f1; goto _LL356;} else{ goto _LL357;} _LL357: goto _LL358; _LL342:
return 0; _LL344: _temp373= _temp367; goto _LL346; _LL346: { void* _temp395= Cyc_Tcutil_compress((
void*) _temp373->type); struct Cyc_Absyn_Exp* _temp401; struct Cyc_Absyn_Tqual*
_temp403; void* _temp405; _LL397: if(( unsigned int) _temp395 > 4u?(( struct
_tunion_struct*) _temp395)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL406: _temp405=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp395)->f1; goto _LL404; _LL404:
_temp403=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp395)->f2; goto _LL402; _LL402: _temp401=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp395)->f3; goto _LL398;} else{ goto _LL399;}
_LL399: goto _LL400; _LL398: return 0; _LL400: return 1; _LL396:;} _LL348:
return 1; _LL350: return 1; _LL352: return Cyc_Absyn_is_lvalue( _temp387);
_LL354: return 1; _LL356: return 1; _LL358: return 0; _LL340:;} struct Cyc_Absyn_Structfield*
Cyc_Absyn_lookup_field( struct Cyc_Core_Opt* fields, struct _tagged_string* v){
if( fields == 0){ return 0;}{ struct Cyc_List_List* fs=( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp407= fields; if( _temp407 == 0){ _throw(
Null_Exception);} _temp407->v;}); for( 0; fs != 0; fs=({ struct Cyc_List_List*
_temp408= fs; if( _temp408 == 0){ _throw( Null_Exception);} _temp408->tl;})){
struct Cyc_Absyn_Structfield* f=( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp409= fs; if( _temp409 == 0){ _throw( Null_Exception);} _temp409->hd;}); if(
Cyc_String_zstrptrcmp( f->name, v) == 0){ return( struct Cyc_Absyn_Structfield*)
f;}}} return 0;} struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_struct_field(
struct Cyc_Absyn_Structdecl* sd, struct _tagged_string* v){ return Cyc_Absyn_lookup_field(
sd->fields, v);} struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_union_field(
struct Cyc_Absyn_Uniondecl* ud, struct _tagged_string* v){ return Cyc_Absyn_lookup_field(
ud->fields, v);} struct _tuple3* Cyc_Absyn_lookup_tuple_field( struct Cyc_List_List*
ts, int i){ for( 0; i != 0; -- i){ if( ts == 0){ return 0;} ts=({ struct Cyc_List_List*
_temp410= ts; if( _temp410 == 0){ _throw( Null_Exception);} _temp410->tl;});}
if( ts == 0){ return 0;} return( struct _tuple3*)(( struct _tuple3*)({ struct
Cyc_List_List* _temp411= ts; if( _temp411 == 0){ _throw( Null_Exception);}
_temp411->hd;}));} struct _tagged_string Cyc_Absyn_attribute2string( void* a){
void* _temp412= a; int _temp450; int _temp452; struct _tagged_string _temp454;
_LL414: if(( unsigned int) _temp412 > 15u?(( struct _tunion_struct*) _temp412)->tag
== Cyc_Absyn_Regparm_att_tag: 0){ _LL451: _temp450=( int)(( struct Cyc_Absyn_Regparm_att_struct*)
_temp412)->f1; goto _LL415;} else{ goto _LL416;} _LL416: if(( int) _temp412 ==
Cyc_Absyn_Stdcall_att){ goto _LL417;} else{ goto _LL418;} _LL418: if(( int)
_temp412 == Cyc_Absyn_Cdecl_att){ goto _LL419;} else{ goto _LL420;} _LL420: if((
int) _temp412 == Cyc_Absyn_Noreturn_att){ goto _LL421;} else{ goto _LL422;}
_LL422: if(( int) _temp412 == Cyc_Absyn_Const_att){ goto _LL423;} else{ goto
_LL424;} _LL424: if(( unsigned int) _temp412 > 15u?(( struct _tunion_struct*)
_temp412)->tag == Cyc_Absyn_Aligned_att_tag: 0){ _LL453: _temp452=( int)((
struct Cyc_Absyn_Aligned_att_struct*) _temp412)->f1; goto _LL425;} else{ goto
_LL426;} _LL426: if(( int) _temp412 == Cyc_Absyn_Packed_att){ goto _LL427;}
else{ goto _LL428;} _LL428: if(( unsigned int) _temp412 > 15u?(( struct
_tunion_struct*) _temp412)->tag == Cyc_Absyn_Section_att_tag: 0){ _LL455:
_temp454=( struct _tagged_string)(( struct Cyc_Absyn_Section_att_struct*)
_temp412)->f1; goto _LL429;} else{ goto _LL430;} _LL430: if(( int) _temp412 ==
Cyc_Absyn_Nocommon_att){ goto _LL431;} else{ goto _LL432;} _LL432: if(( int)
_temp412 == Cyc_Absyn_Shared_att){ goto _LL433;} else{ goto _LL434;} _LL434: if((
int) _temp412 == Cyc_Absyn_Unused_att){ goto _LL435;} else{ goto _LL436;} _LL436:
if(( int) _temp412 == Cyc_Absyn_Weak_att){ goto _LL437;} else{ goto _LL438;}
_LL438: if(( int) _temp412 == Cyc_Absyn_Dllimport_att){ goto _LL439;} else{ goto
_LL440;} _LL440: if(( int) _temp412 == Cyc_Absyn_Dllexport_att){ goto _LL441;}
else{ goto _LL442;} _LL442: if(( int) _temp412 == Cyc_Absyn_No_instrument_function_att){
goto _LL443;} else{ goto _LL444;} _LL444: if(( int) _temp412 == Cyc_Absyn_Constructor_att){
goto _LL445;} else{ goto _LL446;} _LL446: if(( int) _temp412 == Cyc_Absyn_Destructor_att){
goto _LL447;} else{ goto _LL448;} _LL448: if(( int) _temp412 == Cyc_Absyn_No_check_memory_usage_att){
goto _LL449;} else{ goto _LL413;} _LL415: return xprintf("regparm(%d)", _temp450);
_LL417: return( struct _tagged_string)({ char* _temp456=( char*)"stdcall";
struct _tagged_string _temp457; _temp457.curr= _temp456; _temp457.base= _temp456;
_temp457.last_plus_one= _temp456 + 8; _temp457;}); _LL419: return( struct
_tagged_string)({ char* _temp458=( char*)"cdecl"; struct _tagged_string _temp459;
_temp459.curr= _temp458; _temp459.base= _temp458; _temp459.last_plus_one=
_temp458 + 6; _temp459;}); _LL421: return( struct _tagged_string)({ char*
_temp460=( char*)"noreturn"; struct _tagged_string _temp461; _temp461.curr=
_temp460; _temp461.base= _temp460; _temp461.last_plus_one= _temp460 + 9;
_temp461;}); _LL423: return( struct _tagged_string)({ char* _temp462=( char*)"const";
struct _tagged_string _temp463; _temp463.curr= _temp462; _temp463.base= _temp462;
_temp463.last_plus_one= _temp462 + 6; _temp463;}); _LL425: if( _temp452 == - 1){
return( struct _tagged_string)({ char* _temp464=( char*)"aligned"; struct
_tagged_string _temp465; _temp465.curr= _temp464; _temp465.base= _temp464;
_temp465.last_plus_one= _temp464 + 8; _temp465;});} else{ return xprintf("aligned(%d)",
_temp452);} _LL427: return( struct _tagged_string)({ char* _temp466=( char*)"packed";
struct _tagged_string _temp467; _temp467.curr= _temp466; _temp467.base= _temp466;
_temp467.last_plus_one= _temp466 + 7; _temp467;}); _LL429: return({ struct
_tagged_string _temp468= _temp454; xprintf("section(\"%.*s\")", _temp468.last_plus_one
- _temp468.curr, _temp468.curr);}); _LL431: return( struct _tagged_string)({
char* _temp469=( char*)"nocommon"; struct _tagged_string _temp470; _temp470.curr=
_temp469; _temp470.base= _temp469; _temp470.last_plus_one= _temp469 + 9;
_temp470;}); _LL433: return( struct _tagged_string)({ char* _temp471=( char*)"shared";
struct _tagged_string _temp472; _temp472.curr= _temp471; _temp472.base= _temp471;
_temp472.last_plus_one= _temp471 + 7; _temp472;}); _LL435: return( struct
_tagged_string)({ char* _temp473=( char*)"unused"; struct _tagged_string
_temp474; _temp474.curr= _temp473; _temp474.base= _temp473; _temp474.last_plus_one=
_temp473 + 7; _temp474;}); _LL437: return( struct _tagged_string)({ char*
_temp475=( char*)"weak"; struct _tagged_string _temp476; _temp476.curr= _temp475;
_temp476.base= _temp475; _temp476.last_plus_one= _temp475 + 5; _temp476;});
_LL439: return( struct _tagged_string)({ char* _temp477=( char*)"dllimport";
struct _tagged_string _temp478; _temp478.curr= _temp477; _temp478.base= _temp477;
_temp478.last_plus_one= _temp477 + 10; _temp478;}); _LL441: return( struct
_tagged_string)({ char* _temp479=( char*)"dllexport"; struct _tagged_string
_temp480; _temp480.curr= _temp479; _temp480.base= _temp479; _temp480.last_plus_one=
_temp479 + 10; _temp480;}); _LL443: return( struct _tagged_string)({ char*
_temp481=( char*)"no_instrument_function"; struct _tagged_string _temp482;
_temp482.curr= _temp481; _temp482.base= _temp481; _temp482.last_plus_one=
_temp481 + 23; _temp482;}); _LL445: return( struct _tagged_string)({ char*
_temp483=( char*)"constructor"; struct _tagged_string _temp484; _temp484.curr=
_temp483; _temp484.base= _temp483; _temp484.last_plus_one= _temp483 + 12;
_temp484;}); _LL447: return( struct _tagged_string)({ char* _temp485=( char*)"destructor";
struct _tagged_string _temp486; _temp486.curr= _temp485; _temp486.base= _temp485;
_temp486.last_plus_one= _temp485 + 11; _temp486;}); _LL449: return( struct
_tagged_string)({ char* _temp487=( char*)"no_check_memory_usage"; struct
_tagged_string _temp488; _temp488.curr= _temp487; _temp488.base= _temp487;
_temp488.last_plus_one= _temp487 + 22; _temp488;}); _LL413:;}