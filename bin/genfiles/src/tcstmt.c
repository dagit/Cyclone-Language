#include "cyc_include.h"

 struct _tuple0{ void* f1; struct _tagged_string* f2; } ; struct _tuple1{ struct
Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual* f2; void* f3; } ; struct _tuple2{
struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; } ; struct _tuple3{ struct
Cyc_List_List* f1; struct Cyc_List_List* f2; } ; struct _tuple4{ struct Cyc_Absyn_Switch_clause*
f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3; } ; struct _tuple5{
struct Cyc_Absyn_Tvar* f1; void* f2; } ; typedef int Cyc_ptrdiff_t; typedef
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
Cyc_List_List* Cyc_List_List_t; extern int Cyc_List_length( struct Cyc_List_List*
x); extern char Cyc_List_List_empty_tag[ 11u]; struct Cyc_List_List_empty_struct{
char* tag; } ; extern struct Cyc_List_List* Cyc_List_map( void*(* f)( void*),
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_map_c( void*(* f)(
void*, void*), void* env, struct Cyc_List_List* x); extern char Cyc_List_List_mismatch_tag[
14u]; struct Cyc_List_List_mismatch_struct{ char* tag; } ; extern struct Cyc_List_List*
Cyc_List_rev( struct Cyc_List_List* x); extern char Cyc_List_Nth_tag[ 4u];
struct Cyc_List_Nth_struct{ char* tag; } ; struct Cyc_Lineno_Pos{ struct
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
extern char Cyc_Absyn_EmptyAnnot_tag[ 11u]; struct Cyc_Absyn_EmptyAnnot_struct{
char* tag; } ; extern struct Cyc_Absyn_Conref* Cyc_Absyn_new_conref( void* x);
extern void* Cyc_Absyn_exn_typ; extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp(
unsigned int, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_new_stmt(
void* s, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_skip_stmt(
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_seq_stmt(
struct Cyc_Absyn_Stmt* s1, struct Cyc_Absyn_Stmt* s2, struct Cyc_Position_Segment*
loc); struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE;
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
extern void* Cyc_Tcenv_return_typ( struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_local_var( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Vardecl*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_pat_var(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Vardecl*);
extern struct Cyc_List_List* Cyc_Tcenv_lookup_type_vars( struct Cyc_Tcenv_Tenv*);
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_type_vars( struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*, struct Cyc_List_List*); extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_set_in_loop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt*
continue_dest); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_in_switch( struct
Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_fallthru( struct
Cyc_Tcenv_Tenv* te, struct _tuple3* pat_typ, struct Cyc_Absyn_Switch_clause*
clause); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_clear_fallthru( struct Cyc_Tcenv_Tenv*);
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_next( struct Cyc_Tcenv_Tenv*, void*);
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_enter_try( struct Cyc_Tcenv_Tenv* te);
extern int Cyc_Tcenv_get_try_depth( struct Cyc_Tcenv_Tenv* te); extern void Cyc_Tcenv_process_continue(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*, struct Cyc_Absyn_Stmt**); extern
void Cyc_Tcenv_process_break( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*,
struct Cyc_Absyn_Stmt**); extern void Cyc_Tcenv_process_goto( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*, struct _tagged_string*, struct Cyc_Absyn_Stmt**); extern
struct _tuple4* Cyc_Tcenv_process_fallthru( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*,
struct Cyc_Absyn_Switch_clause***); extern struct Cyc_Absyn_Stmt* Cyc_Tcenv_get_encloser(
struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_encloser(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*); extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_label( struct Cyc_Tcenv_Tenv*, struct _tagged_string*, struct Cyc_Absyn_Stmt*);
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_new_block( struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_new_named_block(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Tvar*
name); extern void* Cyc_Tcenv_curr_rgn( struct Cyc_Tcenv_Tenv*); extern char Cyc_Tcutil_TypeErr_tag[
8u]; struct Cyc_Tcutil_TypeErr_struct{ char* tag; } ; extern void* Cyc_Tcutil_impos(
struct _tagged_string); extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*,
struct _tagged_string); extern void* Cyc_Tcutil_compress( void* t); extern int
Cyc_Tcutil_coerce_arg( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*);
extern int Cyc_Tcutil_coerce_assign( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*,
void*); extern int Cyc_Tcutil_coerce_use( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*,
void*); extern int Cyc_Tcutil_unify( void*, void*); extern void* Cyc_Tcutil_substitute(
struct Cyc_List_List*, void*); extern struct _tuple5* Cyc_Tcutil_make_inst_var(
struct Cyc_Absyn_Tvar*); extern void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*, struct Cyc_List_List* bound_tvars, void* k, void*);
extern void* Cyc_Tcexp_tcExp( struct Cyc_Tcenv_Tenv*, void**, struct Cyc_Absyn_Exp*);
extern void* Cyc_Tcexp_tcExpInitializer( struct Cyc_Tcenv_Tenv*, void**, struct
Cyc_Absyn_Exp*); extern void Cyc_Tcexp_tcTest( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Exp* e, struct _tagged_string msg_part); extern struct _tuple3* Cyc_Tcpat_tcPat(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat* p, void** region_opt); extern
void Cyc_Tcpat_check_pat_regions( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat*
p); extern void Cyc_Tcpat_check_switch_exhaustive( struct Cyc_Position_Segment*,
struct Cyc_List_List*); extern int Cyc_Tcpat_check_let_pat_exhaustive( struct
Cyc_Position_Segment*, struct Cyc_Absyn_Pat* p); extern void Cyc_Tcpat_check_catch_overlap(
struct Cyc_Position_Segment*, struct Cyc_List_List*); typedef void* Cyc_CfFlowInfo_LocalRoot;
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
extern void Cyc_NewControlFlow_set_encloser( struct Cyc_Absyn_Stmt* enclosee,
struct Cyc_Absyn_Stmt* encloser); struct Cyc_NewControlFlow_AnalEnv{ struct Cyc_Dict_Dict*
roots; int in_try; void* tryflow; } ; typedef struct Cyc_NewControlFlow_AnalEnv*
Cyc_NewControlFlow_analenv_t; void Cyc_NewControlFlow_cf_check( struct Cyc_List_List*
ds); extern void Cyc_Tcstmt_tcStmt( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt*
s, int new_block); static void Cyc_Tcstmt_decorate_stmt( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Stmt* s){ Cyc_NewControlFlow_set_encloser( s, Cyc_Tcenv_get_encloser(
te)); s->try_depth= Cyc_Tcenv_get_try_depth( te);} static void Cyc_Tcstmt_pattern_synth(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
tvs, struct Cyc_List_List* vs, struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Exp*
where_opt, int new_block){ struct Cyc_Tcenv_Tenv* te2= Cyc_Tcenv_add_type_vars(
loc, te, tvs); if( new_block){ te2= Cyc_Tcenv_new_block( loc, te2);}{ struct Cyc_Core_Opt*
ropt=({ struct Cyc_Core_Opt* _temp0=( struct Cyc_Core_Opt*) GC_malloc( sizeof(
struct Cyc_Core_Opt)); _temp0->v=( void*) Cyc_Tcenv_curr_rgn( te2); _temp0;});{
struct Cyc_List_List* vs2= vs; for( 0; vs2 != 0; vs2= vs2->tl){ te2= Cyc_Tcenv_add_pat_var(
loc, te2,( struct Cyc_Absyn_Vardecl*) vs2->hd);(( struct Cyc_Absyn_Vardecl*) vs2->hd)->region=
ropt;}} if( where_opt != 0){ Cyc_Tcexp_tcTest( te2,( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1= where_opt; if( _temp1 == 0){ _throw(
Null_Exception);} _temp1;}),( struct _tagged_string)({ char* _temp2=( char*)"switch clause guard";
struct _tagged_string _temp3; _temp3.curr= _temp2; _temp3.base= _temp2; _temp3.last_plus_one=
_temp2 + 20; _temp3;}));} if( vs != 0){ te2= Cyc_Tcenv_set_encloser( te2, s);}
Cyc_Tcstmt_tcStmt( te2, s, 0); if( vs != 0){ Cyc_NewControlFlow_set_encloser( s,
Cyc_Tcenv_get_encloser( te));}}} void Cyc_Tcstmt_tcStmt( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Stmt* s0, int new_block){ Cyc_Tcstmt_decorate_stmt( te, s0);{
void* _temp4=( void*) s0->r; struct Cyc_Absyn_Exp* _temp42; struct Cyc_Absyn_Stmt*
_temp44; struct Cyc_Absyn_Stmt* _temp46; struct Cyc_Absyn_Exp* _temp48; struct
Cyc_Absyn_Stmt* _temp50; struct Cyc_Absyn_Stmt* _temp52; struct Cyc_Absyn_Exp*
_temp54; struct Cyc_Absyn_Stmt* _temp56; struct _tuple2 _temp58; struct Cyc_Absyn_Stmt*
_temp60; struct Cyc_Absyn_Exp* _temp62; struct Cyc_Absyn_Stmt* _temp64; struct
_tuple2 _temp66; struct Cyc_Absyn_Stmt* _temp68; struct Cyc_Absyn_Exp* _temp70;
struct _tuple2 _temp72; struct Cyc_Absyn_Stmt* _temp74; struct Cyc_Absyn_Exp*
_temp76; struct Cyc_Absyn_Exp* _temp78; struct _tuple2 _temp80; struct Cyc_Absyn_Stmt*
_temp82; struct Cyc_Absyn_Exp* _temp84; struct Cyc_Absyn_Stmt* _temp86; struct
Cyc_Absyn_Stmt* _temp88; struct Cyc_Absyn_Stmt** _temp90; struct Cyc_Absyn_Stmt*
_temp91; struct Cyc_Absyn_Stmt** _temp93; struct Cyc_Absyn_Stmt* _temp94; struct
Cyc_Absyn_Stmt** _temp96; struct _tagged_string* _temp97; struct Cyc_Absyn_Switch_clause**
_temp99; struct Cyc_Absyn_Switch_clause*** _temp101; struct Cyc_List_List*
_temp102; struct Cyc_Absyn_Stmt* _temp104; struct _tagged_string* _temp106;
struct Cyc_List_List* _temp108; struct Cyc_Absyn_Exp* _temp110; struct Cyc_List_List*
_temp112; struct Cyc_Absyn_Stmt* _temp114; struct Cyc_Absyn_Stmt* _temp116;
struct Cyc_Absyn_Decl* _temp118; struct Cyc_Absyn_Stmt* _temp120; struct Cyc_Absyn_Stmt*
_temp122; _LL6: if(( int) _temp4 == Cyc_Absyn_Skip_s){ goto _LL7;} else{ goto
_LL8;} _LL8: if(( unsigned int) _temp4 > 1u?(( struct _tunion_struct*) _temp4)->tag
== Cyc_Absyn_Exp_s_tag: 0){ _LL43: _temp42=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp4)->f1; goto _LL9;} else{ goto _LL10;} _LL10: if(( unsigned int) _temp4 > 1u?((
struct _tunion_struct*) _temp4)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL47: _temp46=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp4)->f1; goto
_LL45; _LL45: _temp44=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp4)->f2; goto _LL11;} else{ goto _LL12;} _LL12: if(( unsigned int) _temp4 >
1u?(( struct _tunion_struct*) _temp4)->tag == Cyc_Absyn_Return_s_tag: 0){ _LL49:
_temp48=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*) _temp4)->f1;
goto _LL13;} else{ goto _LL14;} _LL14: if(( unsigned int) _temp4 > 1u?(( struct
_tunion_struct*) _temp4)->tag == Cyc_Absyn_IfThenElse_s_tag: 0){ _LL55: _temp54=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp4)->f1;
goto _LL53; _LL53: _temp52=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp4)->f2; goto _LL51; _LL51: _temp50=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp4)->f3; goto _LL15;} else{ goto _LL16;} _LL16: if(( unsigned int) _temp4 >
1u?(( struct _tunion_struct*) _temp4)->tag == Cyc_Absyn_While_s_tag: 0){ _LL59:
_temp58=( struct _tuple2)(( struct Cyc_Absyn_While_s_struct*) _temp4)->f1; _LL63:
_temp62= _temp58.f1; goto _LL61; _LL61: _temp60= _temp58.f2; goto _LL57; _LL57:
_temp56=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_While_s_struct*) _temp4)->f2;
goto _LL17;} else{ goto _LL18;} _LL18: if(( unsigned int) _temp4 > 1u?(( struct
_tunion_struct*) _temp4)->tag == Cyc_Absyn_For_s_tag: 0){ _LL79: _temp78=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*) _temp4)->f1; goto _LL73;
_LL73: _temp72=( struct _tuple2)(( struct Cyc_Absyn_For_s_struct*) _temp4)->f2;
_LL77: _temp76= _temp72.f1; goto _LL75; _LL75: _temp74= _temp72.f2; goto _LL67;
_LL67: _temp66=( struct _tuple2)(( struct Cyc_Absyn_For_s_struct*) _temp4)->f3;
_LL71: _temp70= _temp66.f1; goto _LL69; _LL69: _temp68= _temp66.f2; goto _LL65;
_LL65: _temp64=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*)
_temp4)->f4; goto _LL19;} else{ goto _LL20;} _LL20: if(( unsigned int) _temp4 >
1u?(( struct _tunion_struct*) _temp4)->tag == Cyc_Absyn_Do_s_tag: 0){ _LL87:
_temp86=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*) _temp4)->f1;
goto _LL81; _LL81: _temp80=( struct _tuple2)(( struct Cyc_Absyn_Do_s_struct*)
_temp4)->f2; _LL85: _temp84= _temp80.f1; goto _LL83; _LL83: _temp82= _temp80.f2;
goto _LL21;} else{ goto _LL22;} _LL22: if(( unsigned int) _temp4 > 1u?(( struct
_tunion_struct*) _temp4)->tag == Cyc_Absyn_Break_s_tag: 0){ _LL89: _temp88=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Break_s_struct*) _temp4)->f1; _temp90=&((
struct Cyc_Absyn_Break_s_struct*) _temp4)->f1; goto _LL23;} else{ goto _LL24;}
_LL24: if(( unsigned int) _temp4 > 1u?(( struct _tunion_struct*) _temp4)->tag ==
Cyc_Absyn_Continue_s_tag: 0){ _LL92: _temp91=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_Continue_s_struct*) _temp4)->f1; _temp93=&(( struct Cyc_Absyn_Continue_s_struct*)
_temp4)->f1; goto _LL25;} else{ goto _LL26;} _LL26: if(( unsigned int) _temp4 >
1u?(( struct _tunion_struct*) _temp4)->tag == Cyc_Absyn_Goto_s_tag: 0){ _LL98:
_temp97=( struct _tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp4)->f1;
goto _LL95; _LL95: _temp94=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp4)->f2; _temp96=&(( struct Cyc_Absyn_Goto_s_struct*) _temp4)->f2; goto
_LL27;} else{ goto _LL28;} _LL28: if(( unsigned int) _temp4 > 1u?(( struct
_tunion_struct*) _temp4)->tag == Cyc_Absyn_Fallthru_s_tag: 0){ _LL103: _temp102=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Fallthru_s_struct*) _temp4)->f1; goto
_LL100; _LL100: _temp99=( struct Cyc_Absyn_Switch_clause**)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp4)->f2; _temp101=&(( struct Cyc_Absyn_Fallthru_s_struct*) _temp4)->f2; goto
_LL29;} else{ goto _LL30;} _LL30: if(( unsigned int) _temp4 > 1u?(( struct
_tunion_struct*) _temp4)->tag == Cyc_Absyn_Label_s_tag: 0){ _LL107: _temp106=(
struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*) _temp4)->f1; goto
_LL105; _LL105: _temp104=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp4)->f2; goto _LL31;} else{ goto _LL32;} _LL32: if(( unsigned int) _temp4 >
1u?(( struct _tunion_struct*) _temp4)->tag == Cyc_Absyn_Switch_s_tag: 0){ _LL111:
_temp110=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Switch_s_struct*) _temp4)->f1;
goto _LL109; _LL109: _temp108=( struct Cyc_List_List*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp4)->f2; goto _LL33;} else{ goto _LL34;} _LL34: if(( unsigned int) _temp4 >
1u?(( struct _tunion_struct*) _temp4)->tag == Cyc_Absyn_TryCatch_s_tag: 0){
_LL115: _temp114=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp4)->f1; goto _LL113; _LL113: _temp112=( struct Cyc_List_List*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp4)->f2; goto _LL35;} else{ goto _LL36;} _LL36: if(( unsigned int) _temp4 >
1u?(( struct _tunion_struct*) _temp4)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL119:
_temp118=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*) _temp4)->f1;
goto _LL117; _LL117: _temp116=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp4)->f2; goto _LL37;} else{ goto _LL38;} _LL38: if(( unsigned int) _temp4 >
1u?(( struct _tunion_struct*) _temp4)->tag == Cyc_Absyn_Cut_s_tag: 0){ _LL121:
_temp120=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Cut_s_struct*) _temp4)->f1;
goto _LL39;} else{ goto _LL40;} _LL40: if(( unsigned int) _temp4 > 1u?(( struct
_tunion_struct*) _temp4)->tag == Cyc_Absyn_Splice_s_tag: 0){ _LL123: _temp122=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Splice_s_struct*) _temp4)->f1; goto
_LL41;} else{ goto _LL5;} _LL7: return; _LL9: Cyc_Tcexp_tcExp( te, 0, _temp42);
return; _LL11: { struct Cyc_Tcenv_Tenv* te2= Cyc_Tcenv_set_next( te,( void*)({
struct Cyc_Tcenv_Stmt_j_struct* _temp124=( struct Cyc_Tcenv_Stmt_j_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Stmt_j_struct)); _temp124[ 0]=({ struct Cyc_Tcenv_Stmt_j_struct
_temp125; _temp125.tag= Cyc_Tcenv_Stmt_j_tag; _temp125.f1= _temp44; _temp125;});
_temp124;})); Cyc_Tcstmt_tcStmt( te2, _temp46, 1); while( 1) { void* _temp126=(
void*) _temp44->r; struct Cyc_Absyn_Stmt* _temp132; struct Cyc_Absyn_Stmt*
_temp134; _LL128: if(( unsigned int) _temp126 > 1u?(( struct _tunion_struct*)
_temp126)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL135: _temp134=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp126)->f1; goto _LL133; _LL133: _temp132=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp126)->f2; goto
_LL129;} else{ goto _LL130;} _LL130: goto _LL131; _LL129: Cyc_Tcstmt_decorate_stmt(
te, _temp44); te2= Cyc_Tcenv_set_next( te,( void*)({ struct Cyc_Tcenv_Stmt_j_struct*
_temp136=( struct Cyc_Tcenv_Stmt_j_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Stmt_j_struct));
_temp136[ 0]=({ struct Cyc_Tcenv_Stmt_j_struct _temp137; _temp137.tag= Cyc_Tcenv_Stmt_j_tag;
_temp137.f1= _temp132; _temp137;}); _temp136;})); Cyc_Tcstmt_tcStmt( te2,
_temp134, 1); _temp44= _temp132; continue; _LL131: goto seq_end; _LL127:;}
seq_end: Cyc_Tcstmt_tcStmt( te, _temp44, 1); return;} _LL13: { void* t= Cyc_Tcenv_return_typ(
te); if( _temp48 == 0){ void* _temp138= Cyc_Tcutil_compress( t); _LL140: if((
int) _temp138 == Cyc_Absyn_VoidType){ goto _LL141;} else{ goto _LL142;} _LL142:
goto _LL143; _LL141: goto _LL139; _LL143: Cyc_Tcutil_terr( s0->loc,({ struct
_tagged_string _temp144= Cyc_Absynpp_typ2string( t); xprintf("must return a value of type %.*s",
_temp144.last_plus_one - _temp144.curr, _temp144.curr);})); goto _LL139; _LL139:;}
else{ struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp145= _temp48; if( _temp145 == 0){ _throw( Null_Exception);} _temp145;});
Cyc_Tcexp_tcExp( te,( void**)& t, e); if( ! Cyc_Tcutil_coerce_use( te, e, t)){
Cyc_Tcutil_terr( s0->loc,({ struct _tagged_string _temp146= Cyc_Absynpp_typ2string((
void*)( e->topt)->v); struct _tagged_string _temp147= Cyc_Absynpp_typ2string( t);
xprintf("returns value of type %.*s but requires %.*s", _temp146.last_plus_one -
_temp146.curr, _temp146.curr, _temp147.last_plus_one - _temp147.curr, _temp147.curr);}));}{
void* _temp148= Cyc_Tcutil_compress( t); _LL150: if(( int) _temp148 == Cyc_Absyn_VoidType){
goto _LL151;} else{ goto _LL152;} _LL152: goto _LL153; _LL151: Cyc_Tcutil_terr(
s0->loc,( struct _tagged_string)({ char* _temp154=( char*)"function returns void expression";
struct _tagged_string _temp155; _temp155.curr= _temp154; _temp155.base= _temp154;
_temp155.last_plus_one= _temp154 + 33; _temp155;})); goto _LL149; _LL153: goto
_LL149; _LL149:;}} return;} _LL15: Cyc_Tcexp_tcTest( te, _temp54,( struct
_tagged_string)({ char* _temp156=( char*)"if statement"; struct _tagged_string
_temp157; _temp157.curr= _temp156; _temp157.base= _temp156; _temp157.last_plus_one=
_temp156 + 13; _temp157;})); Cyc_Tcstmt_tcStmt( te, _temp52, 1); Cyc_Tcstmt_tcStmt(
te, _temp50, 1); return; _LL17: Cyc_Tcstmt_decorate_stmt( te, _temp60); Cyc_Tcexp_tcTest(
te, _temp62,( struct _tagged_string)({ char* _temp158=( char*)"while loop";
struct _tagged_string _temp159; _temp159.curr= _temp158; _temp159.base= _temp158;
_temp159.last_plus_one= _temp158 + 11; _temp159;})); Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_in_loop(
te, _temp60), _temp56, 1); return; _LL19: Cyc_Tcstmt_decorate_stmt( te, _temp74);
Cyc_Tcstmt_decorate_stmt( te, _temp68); Cyc_Tcexp_tcExp( te, 0, _temp78); Cyc_Tcexp_tcTest(
te, _temp76,( struct _tagged_string)({ char* _temp160=( char*)"for loop"; struct
_tagged_string _temp161; _temp161.curr= _temp160; _temp161.base= _temp160;
_temp161.last_plus_one= _temp160 + 9; _temp161;})); te= Cyc_Tcenv_set_in_loop(
te, _temp68); Cyc_Tcstmt_tcStmt( te, _temp64, 1); Cyc_Tcexp_tcExp( te, 0,
_temp70); return; _LL21: Cyc_Tcstmt_decorate_stmt( te, _temp82); Cyc_Tcstmt_tcStmt(
Cyc_Tcenv_set_in_loop( te, _temp82), _temp86, 1); Cyc_Tcexp_tcTest( te, _temp84,(
struct _tagged_string)({ char* _temp162=( char*)"do loop"; struct _tagged_string
_temp163; _temp163.curr= _temp162; _temp163.base= _temp162; _temp163.last_plus_one=
_temp162 + 8; _temp163;})); return; _LL23: Cyc_Tcenv_process_break( te, s0,
_temp90); return; _LL25: Cyc_Tcenv_process_continue( te, s0, _temp93); return;
_LL27: Cyc_Tcenv_process_goto( te, s0, _temp97, _temp96); return; _LL29: {
struct _tuple4* trip_opt= Cyc_Tcenv_process_fallthru( te, s0, _temp101); if(
trip_opt == 0){ Cyc_Tcutil_terr( s0->loc,( struct _tagged_string)({ char*
_temp164=( char*)"fallthru not in a non-last case"; struct _tagged_string
_temp165; _temp165.curr= _temp164; _temp165.base= _temp164; _temp165.last_plus_one=
_temp164 + 32; _temp165;})); return;}{ struct _tuple4 _temp169; struct Cyc_List_List*
_temp170; struct Cyc_List_List* _temp172; struct Cyc_Absyn_Switch_clause*
_temp174; struct _tuple4* _temp167=( struct _tuple4*)({ struct _tuple4* _temp166=
trip_opt; if( _temp166 == 0){ _throw( Null_Exception);} _temp166;}); _temp169=*
_temp167; _LL175: _temp174= _temp169.f1; goto _LL173; _LL173: _temp172= _temp169.f2;
goto _LL171; _LL171: _temp170= _temp169.f3; goto _LL168; _LL168: { struct Cyc_List_List*
instantiation=(( struct Cyc_List_List*(*)( struct _tuple5*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_make_inst_var, _temp172);
_temp170=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_List_List*, void*),
struct Cyc_List_List* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcutil_substitute,
instantiation, _temp170); for( 0; _temp170 != 0? _temp102 != 0: 0; _temp170=
_temp170->tl, _temp102= _temp102->tl){ Cyc_Tcexp_tcExp( te, 0,( struct Cyc_Absyn_Exp*)
_temp102->hd); if( ! Cyc_Tcutil_coerce_arg( te,( struct Cyc_Absyn_Exp*) _temp102->hd,(
void*) _temp170->hd)){ Cyc_Tcutil_terr( s0->loc,({ struct _tagged_string
_temp176= Cyc_Absynpp_typ2string(( void*)((( struct Cyc_Absyn_Exp*) _temp102->hd)->topt)->v);
struct _tagged_string _temp177= Cyc_Absynpp_typ2string(( void*) _temp170->hd);
xprintf("fallthru argument has type %.*s but pattern variable has type %.*s",
_temp176.last_plus_one - _temp176.curr, _temp176.curr, _temp177.last_plus_one -
_temp177.curr, _temp177.curr);}));}} if( _temp102 != 0){ Cyc_Tcutil_terr( s0->loc,(
struct _tagged_string)({ char* _temp178=( char*)"too many arguments to explicit fallthru";
struct _tagged_string _temp179; _temp179.curr= _temp178; _temp179.base= _temp178;
_temp179.last_plus_one= _temp178 + 40; _temp179;}));} if( _temp170 != 0){ Cyc_Tcutil_terr(
s0->loc,( struct _tagged_string)({ char* _temp180=( char*)"too few arguments to explicit fallthru";
struct _tagged_string _temp181; _temp181.curr= _temp180; _temp181.base= _temp180;
_temp181.last_plus_one= _temp180 + 39; _temp181;}));} return;}}} _LL31: Cyc_Tcstmt_tcStmt(
Cyc_Tcenv_new_named_block( s0->loc, Cyc_Tcenv_add_label( te, _temp106, _temp104),({
struct Cyc_Absyn_Tvar* _temp182=( struct Cyc_Absyn_Tvar*) GC_malloc( sizeof(
struct Cyc_Absyn_Tvar)); _temp182->name=({ struct _tagged_string* _temp183=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp183[ 0]=({
struct _tagged_string _temp184=* _temp106; xprintf("`%.*s", _temp184.last_plus_one
- _temp184.curr, _temp184.curr);}); _temp183;}); _temp182->kind=(( struct Cyc_Absyn_Conref*(*)(
void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind); _temp182;})),
_temp104, 0); return; _LL33: Cyc_Tcexp_tcExp( te, 0, _temp110);{ void* t=( void*)(
_temp110->topt)->v; te= Cyc_Tcenv_set_in_switch( te); te= Cyc_Tcenv_clear_fallthru(
te);{ struct Cyc_List_List* scs=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( _temp108); for( 0; scs != 0; scs= scs->tl){ struct Cyc_Absyn_Pat*
p=(( struct Cyc_Absyn_Switch_clause*) scs->hd)->pattern; struct _tuple3* pat_typ=
Cyc_Tcpat_tcPat( te, p, 0); struct _tuple3 _temp187; struct Cyc_List_List*
_temp188; struct Cyc_List_List* _temp190; struct _tuple3* _temp185= pat_typ;
_temp187=* _temp185; _LL191: _temp190= _temp187.f1; goto _LL189; _LL189:
_temp188= _temp187.f2; goto _LL186; _LL186: if( ! Cyc_Tcutil_unify(( void*)( p->topt)->v,
t)){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Switch_clause*) scs->hd)->loc,({ struct
_tagged_string _temp192= Cyc_Absynpp_typ2string( t); struct _tagged_string
_temp193= Cyc_Absynpp_typ2string(( void*)( p->topt)->v); xprintf("switch on type %.*s, but case expects type %.*s",
_temp192.last_plus_one - _temp192.curr, _temp192.curr, _temp193.last_plus_one -
_temp193.curr, _temp193.curr);}));} else{ Cyc_Tcpat_check_pat_regions( te, p);}((
struct Cyc_Absyn_Switch_clause*) scs->hd)->pat_vars=({ struct Cyc_Core_Opt*
_temp194=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp194->v=( void*) _temp188; _temp194;}); Cyc_Tcstmt_pattern_synth((( struct
Cyc_Absyn_Switch_clause*) scs->hd)->loc, te, _temp190, _temp188,(( struct Cyc_Absyn_Switch_clause*)
scs->hd)->body,(( struct Cyc_Absyn_Switch_clause*) scs->hd)->where_clause, 1);
te= Cyc_Tcenv_set_fallthru( te,( struct _tuple3*) pat_typ,( struct Cyc_Absyn_Switch_clause*)
scs->hd);}} Cyc_Tcpat_check_switch_exhaustive( s0->loc, _temp108); return;}
_LL35:( void*)( _temp114->r=( void*)(( void*)( Cyc_Absyn_seq_stmt( Cyc_Absyn_new_stmt((
void*) _temp114->r, _temp114->loc), Cyc_Absyn_skip_stmt( _temp114->loc),
_temp114->loc))->r)); Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser( Cyc_Tcenv_enter_try(
te), s0), _temp114, 1); te= Cyc_Tcenv_set_in_switch( te); te= Cyc_Tcenv_clear_fallthru(
te);{ struct Cyc_List_List* scs=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( _temp112); for( 0; scs != 0; scs= scs->tl){ struct Cyc_Absyn_Pat*
p=(( struct Cyc_Absyn_Switch_clause*) scs->hd)->pattern; struct _tuple3* pat_typ=
Cyc_Tcpat_tcPat( te, p, 0); struct _tuple3 _temp197; struct Cyc_List_List*
_temp198; struct Cyc_List_List* _temp200; struct _tuple3* _temp195= pat_typ;
_temp197=* _temp195; _LL201: _temp200= _temp197.f1; goto _LL199; _LL199:
_temp198= _temp197.f2; goto _LL196; _LL196: if( ! Cyc_Tcutil_unify(( void*)( p->topt)->v,
Cyc_Absyn_exn_typ)){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Switch_clause*) scs->hd)->loc,({
struct _tagged_string _temp202= Cyc_Absynpp_typ2string(( void*)( p->topt)->v);
xprintf("expected xtunion exn but found %.*s", _temp202.last_plus_one - _temp202.curr,
_temp202.curr);}));} else{ Cyc_Tcpat_check_pat_regions( te, p);}(( struct Cyc_Absyn_Switch_clause*)
scs->hd)->pat_vars=({ struct Cyc_Core_Opt* _temp203=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp203->v=( void*) _temp198;
_temp203;}); Cyc_Tcstmt_pattern_synth((( struct Cyc_Absyn_Switch_clause*) scs->hd)->loc,
te, _temp200, _temp198,(( struct Cyc_Absyn_Switch_clause*) scs->hd)->body,((
struct Cyc_Absyn_Switch_clause*) scs->hd)->where_clause, 1); te= Cyc_Tcenv_set_fallthru(
te,( struct _tuple3*) pat_typ,( struct Cyc_Absyn_Switch_clause*) scs->hd);}} Cyc_Tcpat_check_catch_overlap(
s0->loc, _temp112); return; _LL37: { struct _tagged_string unimp_msg_part; if(
new_block){ te= Cyc_Tcenv_new_block( s0->loc, te);}{ void* _temp204=( void*)
_temp118->r; struct Cyc_Absyn_Vardecl* _temp230; int _temp232; int* _temp234;
struct Cyc_Absyn_Exp* _temp235; struct Cyc_Core_Opt* _temp237; struct Cyc_Core_Opt**
_temp239; struct Cyc_Core_Opt* _temp240; struct Cyc_Core_Opt** _temp242; struct
Cyc_Absyn_Pat* _temp243; struct Cyc_List_List* _temp245; struct _tagged_string*
_temp247; struct Cyc_List_List* _temp249; struct _tuple0* _temp251; struct Cyc_Absyn_Fndecl*
_temp253; struct Cyc_Absyn_Structdecl* _temp255; struct Cyc_Absyn_Uniondecl*
_temp257; struct Cyc_Absyn_Tuniondecl* _temp259; struct Cyc_Absyn_XTuniondecl*
_temp261; struct Cyc_Absyn_Enumdecl* _temp263; struct Cyc_Absyn_Typedefdecl*
_temp265; struct Cyc_List_List* _temp267; _LL206: if((( struct _tunion_struct*)
_temp204)->tag == Cyc_Absyn_Var_d_tag){ _LL231: _temp230=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_d_struct*) _temp204)->f1; goto _LL207;} else{ goto _LL208;}
_LL208: if((( struct _tunion_struct*) _temp204)->tag == Cyc_Absyn_Let_d_tag){
_LL244: _temp243=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*)
_temp204)->f1; goto _LL241; _LL241: _temp240=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Let_d_struct*) _temp204)->f2; _temp242=&(( struct Cyc_Absyn_Let_d_struct*)
_temp204)->f2; goto _LL238; _LL238: _temp237=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Let_d_struct*) _temp204)->f3; _temp239=&(( struct Cyc_Absyn_Let_d_struct*)
_temp204)->f3; goto _LL236; _LL236: _temp235=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Let_d_struct*) _temp204)->f4; goto _LL233; _LL233: _temp232=( int)((
struct Cyc_Absyn_Let_d_struct*) _temp204)->f5; _temp234=&(( struct Cyc_Absyn_Let_d_struct*)
_temp204)->f5; goto _LL209;} else{ goto _LL210;} _LL210: if((( struct
_tunion_struct*) _temp204)->tag == Cyc_Absyn_Namespace_d_tag){ _LL248: _temp247=(
struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*) _temp204)->f1;
goto _LL246; _LL246: _temp245=( struct Cyc_List_List*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp204)->f2; goto _LL211;} else{ goto _LL212;} _LL212: if((( struct
_tunion_struct*) _temp204)->tag == Cyc_Absyn_Using_d_tag){ _LL252: _temp251=(
struct _tuple0*)(( struct Cyc_Absyn_Using_d_struct*) _temp204)->f1; goto _LL250;
_LL250: _temp249=( struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*)
_temp204)->f2; goto _LL213;} else{ goto _LL214;} _LL214: if((( struct
_tunion_struct*) _temp204)->tag == Cyc_Absyn_Fn_d_tag){ _LL254: _temp253=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*) _temp204)->f1; goto
_LL215;} else{ goto _LL216;} _LL216: if((( struct _tunion_struct*) _temp204)->tag
== Cyc_Absyn_Struct_d_tag){ _LL256: _temp255=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_d_struct*) _temp204)->f1; goto _LL217;} else{ goto
_LL218;} _LL218: if((( struct _tunion_struct*) _temp204)->tag == Cyc_Absyn_Union_d_tag){
_LL258: _temp257=( struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*)
_temp204)->f1; goto _LL219;} else{ goto _LL220;} _LL220: if((( struct
_tunion_struct*) _temp204)->tag == Cyc_Absyn_Tunion_d_tag){ _LL260: _temp259=(
struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_d_struct*) _temp204)->f1;
goto _LL221;} else{ goto _LL222;} _LL222: if((( struct _tunion_struct*) _temp204)->tag
== Cyc_Absyn_XTunion_d_tag){ _LL262: _temp261=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_d_struct*) _temp204)->f1; goto _LL223;} else{ goto
_LL224;} _LL224: if((( struct _tunion_struct*) _temp204)->tag == Cyc_Absyn_Enum_d_tag){
_LL264: _temp263=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*)
_temp204)->f1; goto _LL225;} else{ goto _LL226;} _LL226: if((( struct
_tunion_struct*) _temp204)->tag == Cyc_Absyn_Typedef_d_tag){ _LL266: _temp265=(
struct Cyc_Absyn_Typedefdecl*)(( struct Cyc_Absyn_Typedef_d_struct*) _temp204)->f1;
goto _LL227;} else{ goto _LL228;} _LL228: if((( struct _tunion_struct*) _temp204)->tag
== Cyc_Absyn_ExternC_d_tag){ _LL268: _temp267=( struct Cyc_List_List*)(( struct
Cyc_Absyn_ExternC_d_struct*) _temp204)->f1; goto _LL229;} else{ goto _LL205;}
_LL207: { struct Cyc_Absyn_Vardecl _temp271; struct Cyc_List_List* _temp272;
struct Cyc_Core_Opt* _temp274; struct Cyc_Core_Opt** _temp276; int _temp277;
struct Cyc_Absyn_Exp* _temp279; void* _temp281; struct Cyc_Absyn_Tqual* _temp283;
struct _tuple0* _temp285; struct _tuple0 _temp287; struct _tagged_string*
_temp288; void* _temp290; void* _temp292; struct Cyc_Absyn_Vardecl* _temp269=
_temp230; _temp271=* _temp269; _LL293: _temp292=( void*) _temp271.sc; goto
_LL286; _LL286: _temp285=( struct _tuple0*) _temp271.name; _temp287=* _temp285;
_LL291: _temp290= _temp287.f1; goto _LL289; _LL289: _temp288= _temp287.f2; goto
_LL284; _LL284: _temp283=( struct Cyc_Absyn_Tqual*) _temp271.tq; goto _LL282;
_LL282: _temp281=( void*) _temp271.type; goto _LL280; _LL280: _temp279=( struct
Cyc_Absyn_Exp*) _temp271.initializer; goto _LL278; _LL278: _temp277=( int)
_temp271.shadow; goto _LL275; _LL275: _temp274=( struct Cyc_Core_Opt*) _temp271.region;
_temp276=&(* _temp269).region; goto _LL273; _LL273: _temp272=( struct Cyc_List_List*)
_temp271.attributes; goto _LL270; _LL270: { void* curr_bl= Cyc_Tcenv_curr_rgn(
te);* _temp276=({ struct Cyc_Core_Opt* _temp294=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp294->v=( void*) curr_bl; _temp294;});{
void* _temp295= _temp290; struct Cyc_List_List* _temp305; struct Cyc_List_List*
_temp307; _LL297: if(( int) _temp295 == Cyc_Absyn_Loc_n){ goto _LL298;} else{
goto _LL299;} _LL299: if(( unsigned int) _temp295 > 1u?(( struct _tunion_struct*)
_temp295)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL306: _temp305=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp295)->f1; if( _temp305 == 0){ goto _LL300;}
else{ goto _LL301;}} else{ goto _LL301;} _LL301: if(( unsigned int) _temp295 > 1u?((
struct _tunion_struct*) _temp295)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL308:
_temp307=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp295)->f1;
goto _LL302;} else{ goto _LL303;} _LL303: goto _LL304; _LL298: goto _LL296;
_LL300:(* _temp230->name).f1=( void*) Cyc_Absyn_Loc_n; goto _LL296; _LL302:(
void) _throw((( struct _xtunion_struct*(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp309=( char*)"tcstmt: Abs_n declaration";
struct _tagged_string _temp310; _temp310.curr= _temp309; _temp310.base= _temp309;
_temp310.last_plus_one= _temp309 + 26; _temp310;}))); _LL304: Cyc_Tcutil_terr(
_temp118->loc,( struct _tagged_string)({ char* _temp311=( char*)"cannot declare a qualified local variable";
struct _tagged_string _temp312; _temp312.curr= _temp311; _temp312.base= _temp311;
_temp312.last_plus_one= _temp311 + 42; _temp312;})); goto _LL296; _LL296:;} Cyc_Tcutil_check_type(
s0->loc, te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind,
_temp281); if( _temp279 != 0){{ void* _temp313= Cyc_Tcutil_compress( _temp281);
struct Cyc_Absyn_Exp* _temp319; struct Cyc_Absyn_Tqual* _temp321; void* _temp323;
_LL315: if(( unsigned int) _temp313 > 4u?(( struct _tunion_struct*) _temp313)->tag
== Cyc_Absyn_ArrayType_tag: 0){ _LL324: _temp323=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp313)->f1; goto _LL322; _LL322: _temp321=( struct Cyc_Absyn_Tqual*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp313)->f2; goto _LL320; _LL320: _temp319=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*) _temp313)->f3; goto
_LL316;} else{ goto _LL317;} _LL317: goto _LL318; _LL316: if( _temp319 == 0){
void* _temp325=( void*) _temp279->r; void* _temp335; struct _tagged_string
_temp337; struct Cyc_List_List* _temp339; struct Cyc_Core_Opt* _temp341; struct
Cyc_List_List* _temp343; _LL327: if((( struct _tunion_struct*) _temp325)->tag ==
Cyc_Absyn_Const_e_tag){ _LL336: _temp335=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp325)->f1; if(( unsigned int) _temp335 > 1u?(( struct _tunion_struct*)
_temp335)->tag == Cyc_Absyn_String_c_tag: 0){ _LL338: _temp337=( struct
_tagged_string)(( struct Cyc_Absyn_String_c_struct*) _temp335)->f1; goto _LL328;}
else{ goto _LL329;}} else{ goto _LL329;} _LL329: if((( struct _tunion_struct*)
_temp325)->tag == Cyc_Absyn_UnresolvedMem_e_tag){ _LL342: _temp341=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp325)->f1; goto _LL340; _LL340:
_temp339=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp325)->f2; goto _LL330;} else{ goto _LL331;} _LL331: if((( struct
_tunion_struct*) _temp325)->tag == Cyc_Absyn_Array_e_tag){ _LL344: _temp343=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp325)->f1; goto
_LL332;} else{ goto _LL333;} _LL333: goto _LL334; _LL328: _temp281=( void*)(
_temp230->type=( void*)(( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp345=(
struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp345[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp346; _temp346.tag= Cyc_Absyn_ArrayType_tag;
_temp346.f1=( void*) _temp323; _temp346.f2= _temp321; _temp346.f3=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(({ struct _tagged_string _temp347= _temp337;( unsigned int)(
_temp347.last_plus_one - _temp347.curr);}), 0); _temp346;}); _temp345;}))); goto
_LL326; _LL330: _temp343= _temp339; goto _LL332; _LL332: _temp281=( void*)(
_temp230->type=( void*)(( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp348=(
struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp348[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp349; _temp349.tag= Cyc_Absyn_ArrayType_tag;
_temp349.f1=( void*) _temp323; _temp349.f2= _temp321; _temp349.f3=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(( unsigned int)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp343), 0); _temp349;}); _temp348;}))); goto _LL326; _LL334: goto _LL326;
_LL326:;} goto _LL314; _LL318: goto _LL314; _LL314:;} Cyc_Tcexp_tcExpInitializer(
te,( void**)& _temp281,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp350=
_temp279; if( _temp350 == 0){ _throw( Null_Exception);} _temp350;})); if( ! Cyc_Tcutil_coerce_assign(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp351= _temp279; if(
_temp351 == 0){ _throw( Null_Exception);} _temp351;}), _temp281)){ Cyc_Tcutil_terr(
_temp118->loc,({ struct _tagged_string _temp352=* _temp288; struct
_tagged_string _temp353= Cyc_Absynpp_typ2string( _temp281); struct
_tagged_string _temp354= Cyc_Absynpp_typ2string(( void*)( _temp279->topt)->v);
xprintf("%.*s declared with type %.*s, initialized with type %.*s", _temp352.last_plus_one
- _temp352.curr, _temp352.curr, _temp353.last_plus_one - _temp353.curr, _temp353.curr,
_temp354.last_plus_one - _temp354.curr, _temp354.curr);}));}} Cyc_Tcstmt_tcStmt(
Cyc_Tcenv_set_encloser( Cyc_Tcenv_add_local_var( _temp116->loc, te, _temp230),
s0), _temp116, 0); return;}} _LL209: { struct Cyc_Core_Opt* topt=* _temp239; Cyc_Tcexp_tcExpInitializer(
te, 0, _temp235);* _temp239= _temp235->topt;{ struct _tuple3 _temp357; struct
Cyc_List_List* _temp358; struct Cyc_List_List* _temp360; struct _tuple3*
_temp355= Cyc_Tcpat_tcPat( te, _temp243, 0); _temp357=* _temp355; _LL361:
_temp360= _temp357.f1; goto _LL359; _LL359: _temp358= _temp357.f2; goto _LL356;
_LL356:* _temp242=({ struct Cyc_Core_Opt* _temp362=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp362->v=( void*) _temp358;
_temp362;}); if( _temp360 != 0){ Cyc_Tcutil_terr( _temp118->loc,( struct
_tagged_string)({ char* _temp363=( char*)"can't introduce type variables in let-clause";
struct _tagged_string _temp364; _temp364.curr= _temp363; _temp364.base= _temp363;
_temp364.last_plus_one= _temp363 + 45; _temp364;}));} if( topt != 0? ! Cyc_Tcutil_unify((
void*)( _temp243->topt)->v,( void*) topt->v): 0){ Cyc_Tcutil_terr( _temp118->loc,(
struct _tagged_string)({ char* _temp365=( char*)"type of pattern does not match declared type";
struct _tagged_string _temp366; _temp366.curr= _temp365; _temp366.base= _temp365;
_temp366.last_plus_one= _temp365 + 45; _temp366;}));} if( ! Cyc_Tcutil_unify((
void*)( _temp243->topt)->v,( void*)( _temp235->topt)->v)){ Cyc_Tcutil_terr(
_temp118->loc,({ struct _tagged_string _temp367= Cyc_Absynpp_typ2string(( void*)(
_temp243->topt)->v); struct _tagged_string _temp368= Cyc_Absynpp_typ2string((
void*)( _temp235->topt)->v); xprintf("pattern type %.*s does not match definition type %.*s",
_temp367.last_plus_one - _temp367.curr, _temp367.curr, _temp368.last_plus_one -
_temp368.curr, _temp368.curr);}));} else{ Cyc_Tcpat_check_pat_regions( te,
_temp243);}* _temp234= Cyc_Tcpat_check_let_pat_exhaustive( _temp243->loc,
_temp243); Cyc_Tcstmt_pattern_synth( s0->loc, te, 0, _temp358, _temp116, 0, 0);
return;}} _LL211: unimp_msg_part=( struct _tagged_string)({ char* _temp369=(
char*)"namespace"; struct _tagged_string _temp370; _temp370.curr= _temp369;
_temp370.base= _temp369; _temp370.last_plus_one= _temp369 + 10; _temp370;});
goto _LL205; _LL213: unimp_msg_part=( struct _tagged_string)({ char* _temp371=(
char*)"using"; struct _tagged_string _temp372; _temp372.curr= _temp371; _temp372.base=
_temp371; _temp372.last_plus_one= _temp371 + 6; _temp372;}); goto _LL205; _LL215:
unimp_msg_part=( struct _tagged_string)({ char* _temp373=( char*)"function";
struct _tagged_string _temp374; _temp374.curr= _temp373; _temp374.base= _temp373;
_temp374.last_plus_one= _temp373 + 9; _temp374;}); goto _LL205; _LL217:
unimp_msg_part=( struct _tagged_string)({ char* _temp375=( char*)"struct";
struct _tagged_string _temp376; _temp376.curr= _temp375; _temp376.base= _temp375;
_temp376.last_plus_one= _temp375 + 7; _temp376;}); goto _LL205; _LL219:
unimp_msg_part=( struct _tagged_string)({ char* _temp377=( char*)"union"; struct
_tagged_string _temp378; _temp378.curr= _temp377; _temp378.base= _temp377;
_temp378.last_plus_one= _temp377 + 6; _temp378;}); goto _LL205; _LL221:
unimp_msg_part=( struct _tagged_string)({ char* _temp379=( char*)"tunion";
struct _tagged_string _temp380; _temp380.curr= _temp379; _temp380.base= _temp379;
_temp380.last_plus_one= _temp379 + 7; _temp380;}); goto _LL205; _LL223:
unimp_msg_part=( struct _tagged_string)({ char* _temp381=( char*)"xtunion";
struct _tagged_string _temp382; _temp382.curr= _temp381; _temp382.base= _temp381;
_temp382.last_plus_one= _temp381 + 8; _temp382;}); goto _LL205; _LL225:
unimp_msg_part=( struct _tagged_string)({ char* _temp383=( char*)"enum"; struct
_tagged_string _temp384; _temp384.curr= _temp383; _temp384.base= _temp383;
_temp384.last_plus_one= _temp383 + 5; _temp384;}); goto _LL205; _LL227:
unimp_msg_part=( struct _tagged_string)({ char* _temp385=( char*)"typedef";
struct _tagged_string _temp386; _temp386.curr= _temp385; _temp386.base= _temp385;
_temp386.last_plus_one= _temp385 + 8; _temp386;}); goto _LL205; _LL229:
unimp_msg_part=( struct _tagged_string)({ char* _temp387=( char*)"extern \"C\"";
struct _tagged_string _temp388; _temp388.curr= _temp387; _temp388.base= _temp387;
_temp388.last_plus_one= _temp387 + 11; _temp388;}); goto _LL205; _LL205:;}( void)
_throw((( struct _xtunion_struct*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
struct _tagged_string _temp389= unimp_msg_part; xprintf("tcStmt: nested %.*s declarations unimplemented",
_temp389.last_plus_one - _temp389.curr, _temp389.curr);})));} _LL39:( void)
_throw((( struct _xtunion_struct*(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp390=( char*)"tcStmt: cut is not implemented";
struct _tagged_string _temp391; _temp391.curr= _temp390; _temp391.base= _temp390;
_temp391.last_plus_one= _temp390 + 31; _temp391;}))); _LL41:( void) _throw(((
struct _xtunion_struct*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ char* _temp392=( char*)"tcStmt: splice is not implemented";
struct _tagged_string _temp393; _temp393.curr= _temp392; _temp393.base= _temp392;
_temp393.last_plus_one= _temp392 + 34; _temp393;}))); _LL5:;}}