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
struct Cyc_List_List* vs2= vs; for( 0; vs2 != 0; vs2=({ struct Cyc_List_List*
_temp1= vs2; if( _temp1 == 0){ _throw( Null_Exception);} _temp1->tl;})){ te2=
Cyc_Tcenv_add_pat_var( loc, te2,( struct Cyc_Absyn_Vardecl*)({ struct Cyc_List_List*
_temp2= vs2; if( _temp2 == 0){ _throw( Null_Exception);} _temp2->hd;}));((
struct Cyc_Absyn_Vardecl*)({ struct Cyc_List_List* _temp3= vs2; if( _temp3 == 0){
_throw( Null_Exception);} _temp3->hd;}))->region= ropt;}} if( where_opt != 0){
Cyc_Tcexp_tcTest( te2,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp4=
where_opt; if( _temp4 == 0){ _throw( Null_Exception);} _temp4;}),( struct
_tagged_string)({ char* _temp5=( char*)"switch clause guard"; struct
_tagged_string _temp6; _temp6.curr= _temp5; _temp6.base= _temp5; _temp6.last_plus_one=
_temp5 + 20; _temp6;}));} if( vs != 0){ te2= Cyc_Tcenv_set_encloser( te2, s);}
Cyc_Tcstmt_tcStmt( te2, s, 0); if( vs != 0){ Cyc_NewControlFlow_set_encloser( s,
Cyc_Tcenv_get_encloser( te));}}} void Cyc_Tcstmt_tcStmt( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Stmt* s0, int new_block){ Cyc_Tcstmt_decorate_stmt( te, s0);{
void* _temp7=( void*) s0->r; struct Cyc_Absyn_Exp* _temp45; struct Cyc_Absyn_Stmt*
_temp47; struct Cyc_Absyn_Stmt* _temp49; struct Cyc_Absyn_Exp* _temp51; struct
Cyc_Absyn_Stmt* _temp53; struct Cyc_Absyn_Stmt* _temp55; struct Cyc_Absyn_Exp*
_temp57; struct Cyc_Absyn_Stmt* _temp59; struct _tuple2 _temp61; struct Cyc_Absyn_Stmt*
_temp63; struct Cyc_Absyn_Exp* _temp65; struct Cyc_Absyn_Stmt* _temp67; struct
_tuple2 _temp69; struct Cyc_Absyn_Stmt* _temp71; struct Cyc_Absyn_Exp* _temp73;
struct _tuple2 _temp75; struct Cyc_Absyn_Stmt* _temp77; struct Cyc_Absyn_Exp*
_temp79; struct Cyc_Absyn_Exp* _temp81; struct _tuple2 _temp83; struct Cyc_Absyn_Stmt*
_temp85; struct Cyc_Absyn_Exp* _temp87; struct Cyc_Absyn_Stmt* _temp89; struct
Cyc_Absyn_Stmt* _temp91; struct Cyc_Absyn_Stmt** _temp93; struct Cyc_Absyn_Stmt*
_temp94; struct Cyc_Absyn_Stmt** _temp96; struct Cyc_Absyn_Stmt* _temp97; struct
Cyc_Absyn_Stmt** _temp99; struct _tagged_string* _temp100; struct Cyc_Absyn_Switch_clause**
_temp102; struct Cyc_Absyn_Switch_clause*** _temp104; struct Cyc_List_List*
_temp105; struct Cyc_Absyn_Stmt* _temp107; struct _tagged_string* _temp109;
struct Cyc_List_List* _temp111; struct Cyc_Absyn_Exp* _temp113; struct Cyc_List_List*
_temp115; struct Cyc_Absyn_Stmt* _temp117; struct Cyc_Absyn_Stmt* _temp119;
struct Cyc_Absyn_Decl* _temp121; struct Cyc_Absyn_Stmt* _temp123; struct Cyc_Absyn_Stmt*
_temp125; _LL9: if(( int) _temp7 == Cyc_Absyn_Skip_s){ goto _LL10;} else{ goto
_LL11;} _LL11: if(( unsigned int) _temp7 > 1u?(( struct _tunion_struct*) _temp7)->tag
== Cyc_Absyn_Exp_s_tag: 0){ _LL46: _temp45=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp7)->f1; goto _LL12;} else{ goto _LL13;} _LL13: if(( unsigned int) _temp7 >
1u?(( struct _tunion_struct*) _temp7)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL50:
_temp49=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp7)->f1;
goto _LL48; _LL48: _temp47=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp7)->f2; goto _LL14;} else{ goto _LL15;} _LL15: if(( unsigned int) _temp7 >
1u?(( struct _tunion_struct*) _temp7)->tag == Cyc_Absyn_Return_s_tag: 0){ _LL52:
_temp51=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*) _temp7)->f1;
goto _LL16;} else{ goto _LL17;} _LL17: if(( unsigned int) _temp7 > 1u?(( struct
_tunion_struct*) _temp7)->tag == Cyc_Absyn_IfThenElse_s_tag: 0){ _LL58: _temp57=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp7)->f1;
goto _LL56; _LL56: _temp55=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp7)->f2; goto _LL54; _LL54: _temp53=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp7)->f3; goto _LL18;} else{ goto _LL19;} _LL19: if(( unsigned int) _temp7 >
1u?(( struct _tunion_struct*) _temp7)->tag == Cyc_Absyn_While_s_tag: 0){ _LL62:
_temp61=( struct _tuple2)(( struct Cyc_Absyn_While_s_struct*) _temp7)->f1; _LL66:
_temp65= _temp61.f1; goto _LL64; _LL64: _temp63= _temp61.f2; goto _LL60; _LL60:
_temp59=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_While_s_struct*) _temp7)->f2;
goto _LL20;} else{ goto _LL21;} _LL21: if(( unsigned int) _temp7 > 1u?(( struct
_tunion_struct*) _temp7)->tag == Cyc_Absyn_For_s_tag: 0){ _LL82: _temp81=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*) _temp7)->f1; goto _LL76;
_LL76: _temp75=( struct _tuple2)(( struct Cyc_Absyn_For_s_struct*) _temp7)->f2;
_LL80: _temp79= _temp75.f1; goto _LL78; _LL78: _temp77= _temp75.f2; goto _LL70;
_LL70: _temp69=( struct _tuple2)(( struct Cyc_Absyn_For_s_struct*) _temp7)->f3;
_LL74: _temp73= _temp69.f1; goto _LL72; _LL72: _temp71= _temp69.f2; goto _LL68;
_LL68: _temp67=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*)
_temp7)->f4; goto _LL22;} else{ goto _LL23;} _LL23: if(( unsigned int) _temp7 >
1u?(( struct _tunion_struct*) _temp7)->tag == Cyc_Absyn_Do_s_tag: 0){ _LL90:
_temp89=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*) _temp7)->f1;
goto _LL84; _LL84: _temp83=( struct _tuple2)(( struct Cyc_Absyn_Do_s_struct*)
_temp7)->f2; _LL88: _temp87= _temp83.f1; goto _LL86; _LL86: _temp85= _temp83.f2;
goto _LL24;} else{ goto _LL25;} _LL25: if(( unsigned int) _temp7 > 1u?(( struct
_tunion_struct*) _temp7)->tag == Cyc_Absyn_Break_s_tag: 0){ _LL92: _temp91=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Break_s_struct*) _temp7)->f1; _temp93=&((
struct Cyc_Absyn_Break_s_struct*) _temp7)->f1; goto _LL26;} else{ goto _LL27;}
_LL27: if(( unsigned int) _temp7 > 1u?(( struct _tunion_struct*) _temp7)->tag ==
Cyc_Absyn_Continue_s_tag: 0){ _LL95: _temp94=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_Continue_s_struct*) _temp7)->f1; _temp96=&(( struct Cyc_Absyn_Continue_s_struct*)
_temp7)->f1; goto _LL28;} else{ goto _LL29;} _LL29: if(( unsigned int) _temp7 >
1u?(( struct _tunion_struct*) _temp7)->tag == Cyc_Absyn_Goto_s_tag: 0){ _LL101:
_temp100=( struct _tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp7)->f1;
goto _LL98; _LL98: _temp97=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp7)->f2; _temp99=&(( struct Cyc_Absyn_Goto_s_struct*) _temp7)->f2; goto
_LL30;} else{ goto _LL31;} _LL31: if(( unsigned int) _temp7 > 1u?(( struct
_tunion_struct*) _temp7)->tag == Cyc_Absyn_Fallthru_s_tag: 0){ _LL106: _temp105=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Fallthru_s_struct*) _temp7)->f1; goto
_LL103; _LL103: _temp102=( struct Cyc_Absyn_Switch_clause**)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp7)->f2; _temp104=&(( struct Cyc_Absyn_Fallthru_s_struct*) _temp7)->f2; goto
_LL32;} else{ goto _LL33;} _LL33: if(( unsigned int) _temp7 > 1u?(( struct
_tunion_struct*) _temp7)->tag == Cyc_Absyn_Label_s_tag: 0){ _LL110: _temp109=(
struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*) _temp7)->f1; goto
_LL108; _LL108: _temp107=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp7)->f2; goto _LL34;} else{ goto _LL35;} _LL35: if(( unsigned int) _temp7 >
1u?(( struct _tunion_struct*) _temp7)->tag == Cyc_Absyn_Switch_s_tag: 0){ _LL114:
_temp113=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Switch_s_struct*) _temp7)->f1;
goto _LL112; _LL112: _temp111=( struct Cyc_List_List*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp7)->f2; goto _LL36;} else{ goto _LL37;} _LL37: if(( unsigned int) _temp7 >
1u?(( struct _tunion_struct*) _temp7)->tag == Cyc_Absyn_TryCatch_s_tag: 0){
_LL118: _temp117=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp7)->f1; goto _LL116; _LL116: _temp115=( struct Cyc_List_List*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp7)->f2; goto _LL38;} else{ goto _LL39;} _LL39: if(( unsigned int) _temp7 >
1u?(( struct _tunion_struct*) _temp7)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL122:
_temp121=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*) _temp7)->f1;
goto _LL120; _LL120: _temp119=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp7)->f2; goto _LL40;} else{ goto _LL41;} _LL41: if(( unsigned int) _temp7 >
1u?(( struct _tunion_struct*) _temp7)->tag == Cyc_Absyn_Cut_s_tag: 0){ _LL124:
_temp123=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Cut_s_struct*) _temp7)->f1;
goto _LL42;} else{ goto _LL43;} _LL43: if(( unsigned int) _temp7 > 1u?(( struct
_tunion_struct*) _temp7)->tag == Cyc_Absyn_Splice_s_tag: 0){ _LL126: _temp125=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Splice_s_struct*) _temp7)->f1; goto
_LL44;} else{ goto _LL8;} _LL10: return; _LL12: Cyc_Tcexp_tcExp( te, 0, _temp45);
return; _LL14: { struct Cyc_Tcenv_Tenv* te2= Cyc_Tcenv_set_next( te,( void*)({
struct Cyc_Tcenv_Stmt_j_struct* _temp127=( struct Cyc_Tcenv_Stmt_j_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Stmt_j_struct)); _temp127[ 0]=({ struct Cyc_Tcenv_Stmt_j_struct
_temp128; _temp128.tag= Cyc_Tcenv_Stmt_j_tag; _temp128.f1= _temp47; _temp128;});
_temp127;})); Cyc_Tcstmt_tcStmt( te2, _temp49, 1); while( 1) { void* _temp129=(
void*) _temp47->r; struct Cyc_Absyn_Stmt* _temp135; struct Cyc_Absyn_Stmt*
_temp137; _LL131: if(( unsigned int) _temp129 > 1u?(( struct _tunion_struct*)
_temp129)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL138: _temp137=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp129)->f1; goto _LL136; _LL136: _temp135=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp129)->f2; goto
_LL132;} else{ goto _LL133;} _LL133: goto _LL134; _LL132: Cyc_Tcstmt_decorate_stmt(
te, _temp47); te2= Cyc_Tcenv_set_next( te,( void*)({ struct Cyc_Tcenv_Stmt_j_struct*
_temp139=( struct Cyc_Tcenv_Stmt_j_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Stmt_j_struct));
_temp139[ 0]=({ struct Cyc_Tcenv_Stmt_j_struct _temp140; _temp140.tag= Cyc_Tcenv_Stmt_j_tag;
_temp140.f1= _temp135; _temp140;}); _temp139;})); Cyc_Tcstmt_tcStmt( te2,
_temp137, 1); _temp47= _temp135; continue; _LL134: goto seq_end; _LL130:;}
seq_end: Cyc_Tcstmt_tcStmt( te, _temp47, 1); return;} _LL16: { void* t= Cyc_Tcenv_return_typ(
te); if( _temp51 == 0){ void* _temp141= Cyc_Tcutil_compress( t); _LL143: if((
int) _temp141 == Cyc_Absyn_VoidType){ goto _LL144;} else{ goto _LL145;} _LL145:
goto _LL146; _LL144: goto _LL142; _LL146: Cyc_Tcutil_terr( s0->loc,({ struct
_tagged_string _temp147= Cyc_Absynpp_typ2string( t); xprintf("must return a value of type %.*s",
_temp147.last_plus_one - _temp147.curr, _temp147.curr);})); goto _LL142; _LL142:;}
else{ struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp148= _temp51; if( _temp148 == 0){ _throw( Null_Exception);} _temp148;});
Cyc_Tcexp_tcExp( te,( void**)& t, e); if( ! Cyc_Tcutil_coerce_use( te, e, t)){
Cyc_Tcutil_terr( s0->loc,({ struct _tagged_string _temp150= Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Core_Opt* _temp149= e->topt; if( _temp149 == 0){ _throw(
Null_Exception);} _temp149->v;})); struct _tagged_string _temp151= Cyc_Absynpp_typ2string(
t); xprintf("returns value of type %.*s but requires %.*s", _temp150.last_plus_one
- _temp150.curr, _temp150.curr, _temp151.last_plus_one - _temp151.curr, _temp151.curr);}));}{
void* _temp152= Cyc_Tcutil_compress( t); _LL154: if(( int) _temp152 == Cyc_Absyn_VoidType){
goto _LL155;} else{ goto _LL156;} _LL156: goto _LL157; _LL155: Cyc_Tcutil_terr(
s0->loc,( struct _tagged_string)({ char* _temp158=( char*)"function returns void expression";
struct _tagged_string _temp159; _temp159.curr= _temp158; _temp159.base= _temp158;
_temp159.last_plus_one= _temp158 + 33; _temp159;})); goto _LL153; _LL157: goto
_LL153; _LL153:;}} return;} _LL18: Cyc_Tcexp_tcTest( te, _temp57,( struct
_tagged_string)({ char* _temp160=( char*)"if statement"; struct _tagged_string
_temp161; _temp161.curr= _temp160; _temp161.base= _temp160; _temp161.last_plus_one=
_temp160 + 13; _temp161;})); Cyc_Tcstmt_tcStmt( te, _temp55, 1); Cyc_Tcstmt_tcStmt(
te, _temp53, 1); return; _LL20: Cyc_Tcstmt_decorate_stmt( te, _temp63); Cyc_Tcexp_tcTest(
te, _temp65,( struct _tagged_string)({ char* _temp162=( char*)"while loop";
struct _tagged_string _temp163; _temp163.curr= _temp162; _temp163.base= _temp162;
_temp163.last_plus_one= _temp162 + 11; _temp163;})); Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_in_loop(
te, _temp63), _temp59, 1); return; _LL22: Cyc_Tcstmt_decorate_stmt( te, _temp77);
Cyc_Tcstmt_decorate_stmt( te, _temp71); Cyc_Tcexp_tcExp( te, 0, _temp81); Cyc_Tcexp_tcTest(
te, _temp79,( struct _tagged_string)({ char* _temp164=( char*)"for loop"; struct
_tagged_string _temp165; _temp165.curr= _temp164; _temp165.base= _temp164;
_temp165.last_plus_one= _temp164 + 9; _temp165;})); te= Cyc_Tcenv_set_in_loop(
te, _temp71); Cyc_Tcstmt_tcStmt( te, _temp67, 1); Cyc_Tcexp_tcExp( te, 0,
_temp73); return; _LL24: Cyc_Tcstmt_decorate_stmt( te, _temp85); Cyc_Tcstmt_tcStmt(
Cyc_Tcenv_set_in_loop( te, _temp85), _temp89, 1); Cyc_Tcexp_tcTest( te, _temp87,(
struct _tagged_string)({ char* _temp166=( char*)"do loop"; struct _tagged_string
_temp167; _temp167.curr= _temp166; _temp167.base= _temp166; _temp167.last_plus_one=
_temp166 + 8; _temp167;})); return; _LL26: Cyc_Tcenv_process_break( te, s0,
_temp93); return; _LL28: Cyc_Tcenv_process_continue( te, s0, _temp96); return;
_LL30: Cyc_Tcenv_process_goto( te, s0, _temp100, _temp99); return; _LL32: {
struct _tuple4* trip_opt= Cyc_Tcenv_process_fallthru( te, s0, _temp104); if(
trip_opt == 0){ Cyc_Tcutil_terr( s0->loc,( struct _tagged_string)({ char*
_temp168=( char*)"fallthru not in a non-last case"; struct _tagged_string
_temp169; _temp169.curr= _temp168; _temp169.base= _temp168; _temp169.last_plus_one=
_temp168 + 32; _temp169;})); return;}{ struct _tuple4 _temp173; struct Cyc_List_List*
_temp174; struct Cyc_List_List* _temp176; struct Cyc_Absyn_Switch_clause*
_temp178; struct _tuple4* _temp171=( struct _tuple4*)({ struct _tuple4* _temp170=
trip_opt; if( _temp170 == 0){ _throw( Null_Exception);} _temp170;}); _temp173=*
_temp171; _LL179: _temp178= _temp173.f1; goto _LL177; _LL177: _temp176= _temp173.f2;
goto _LL175; _LL175: _temp174= _temp173.f3; goto _LL172; _LL172: { struct Cyc_List_List*
instantiation=(( struct Cyc_List_List*(*)( struct _tuple5*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_make_inst_var, _temp176);
_temp174=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_List_List*, void*),
struct Cyc_List_List* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcutil_substitute,
instantiation, _temp174); for( 0; _temp174 != 0? _temp105 != 0: 0; _temp174=({
struct Cyc_List_List* _temp180= _temp174; if( _temp180 == 0){ _throw(
Null_Exception);} _temp180->tl;}), _temp105=({ struct Cyc_List_List* _temp181=
_temp105; if( _temp181 == 0){ _throw( Null_Exception);} _temp181->tl;})){ Cyc_Tcexp_tcExp(
te, 0,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp182= _temp105; if(
_temp182 == 0){ _throw( Null_Exception);} _temp182->hd;})); if( ! Cyc_Tcutil_coerce_arg(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp183= _temp105; if(
_temp183 == 0){ _throw( Null_Exception);} _temp183->hd;}),( void*)({ struct Cyc_List_List*
_temp184= _temp174; if( _temp184 == 0){ _throw( Null_Exception);} _temp184->hd;}))){
Cyc_Tcutil_terr( s0->loc,({ struct _tagged_string _temp188= Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Core_Opt* _temp186=(( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp185= _temp105; if( _temp185 == 0){ _throw( Null_Exception);} _temp185->hd;}))->topt;
if( _temp186 == 0){ _throw( Null_Exception);} _temp186->v;})); struct
_tagged_string _temp189= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_List_List*
_temp187= _temp174; if( _temp187 == 0){ _throw( Null_Exception);} _temp187->hd;}));
xprintf("fallthru argument has type %.*s but pattern variable has type %.*s",
_temp188.last_plus_one - _temp188.curr, _temp188.curr, _temp189.last_plus_one -
_temp189.curr, _temp189.curr);}));}} if( _temp105 != 0){ Cyc_Tcutil_terr( s0->loc,(
struct _tagged_string)({ char* _temp190=( char*)"too many arguments to explicit fallthru";
struct _tagged_string _temp191; _temp191.curr= _temp190; _temp191.base= _temp190;
_temp191.last_plus_one= _temp190 + 40; _temp191;}));} if( _temp174 != 0){ Cyc_Tcutil_terr(
s0->loc,( struct _tagged_string)({ char* _temp192=( char*)"too few arguments to explicit fallthru";
struct _tagged_string _temp193; _temp193.curr= _temp192; _temp193.base= _temp192;
_temp193.last_plus_one= _temp192 + 39; _temp193;}));} return;}}} _LL34: Cyc_Tcstmt_tcStmt(
Cyc_Tcenv_new_named_block( s0->loc, Cyc_Tcenv_add_label( te, _temp109, _temp107),({
struct Cyc_Absyn_Tvar* _temp194=( struct Cyc_Absyn_Tvar*) GC_malloc( sizeof(
struct Cyc_Absyn_Tvar)); _temp194->name=({ struct _tagged_string* _temp195=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp195[ 0]=({
struct _tagged_string _temp196=* _temp109; xprintf("`%.*s", _temp196.last_plus_one
- _temp196.curr, _temp196.curr);}); _temp195;}); _temp194->kind=(( struct Cyc_Absyn_Conref*(*)(
void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind); _temp194;})),
_temp107, 0); return; _LL36: Cyc_Tcexp_tcExp( te, 0, _temp113);{ void* t=( void*)({
struct Cyc_Core_Opt* _temp197= _temp113->topt; if( _temp197 == 0){ _throw(
Null_Exception);} _temp197->v;}); te= Cyc_Tcenv_set_in_switch( te); te= Cyc_Tcenv_clear_fallthru(
te);{ struct Cyc_List_List* scs=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( _temp111); for( 0; scs != 0; scs=({ struct Cyc_List_List*
_temp198= scs; if( _temp198 == 0){ _throw( Null_Exception);} _temp198->tl;})){
struct Cyc_Absyn_Pat* p=(( struct Cyc_Absyn_Switch_clause*)({ struct Cyc_List_List*
_temp199= scs; if( _temp199 == 0){ _throw( Null_Exception);} _temp199->hd;}))->pattern;
struct _tuple3* pat_typ= Cyc_Tcpat_tcPat( te, p, 0); struct _tuple3 _temp202;
struct Cyc_List_List* _temp203; struct Cyc_List_List* _temp205; struct _tuple3*
_temp200= pat_typ; _temp202=* _temp200; _LL206: _temp205= _temp202.f1; goto
_LL204; _LL204: _temp203= _temp202.f2; goto _LL201; _LL201: if( ! Cyc_Tcutil_unify((
void*)({ struct Cyc_Core_Opt* _temp207= p->topt; if( _temp207 == 0){ _throw(
Null_Exception);} _temp207->v;}), t)){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Switch_clause*)({
struct Cyc_List_List* _temp208= scs; if( _temp208 == 0){ _throw( Null_Exception);}
_temp208->hd;}))->loc,({ struct _tagged_string _temp210= Cyc_Absynpp_typ2string(
t); struct _tagged_string _temp211= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp209= p->topt; if( _temp209 == 0){ _throw( Null_Exception);} _temp209->v;}));
xprintf("switch on type %.*s, but case expects type %.*s", _temp210.last_plus_one
- _temp210.curr, _temp210.curr, _temp211.last_plus_one - _temp211.curr, _temp211.curr);}));}
else{ Cyc_Tcpat_check_pat_regions( te, p);}(( struct Cyc_Absyn_Switch_clause*)({
struct Cyc_List_List* _temp212= scs; if( _temp212 == 0){ _throw( Null_Exception);}
_temp212->hd;}))->pat_vars=({ struct Cyc_Core_Opt* _temp213=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp213->v=( void*) _temp203;
_temp213;}); Cyc_Tcstmt_pattern_synth((( struct Cyc_Absyn_Switch_clause*)({
struct Cyc_List_List* _temp214= scs; if( _temp214 == 0){ _throw( Null_Exception);}
_temp214->hd;}))->loc, te, _temp205, _temp203,(( struct Cyc_Absyn_Switch_clause*)({
struct Cyc_List_List* _temp215= scs; if( _temp215 == 0){ _throw( Null_Exception);}
_temp215->hd;}))->body,(( struct Cyc_Absyn_Switch_clause*)({ struct Cyc_List_List*
_temp216= scs; if( _temp216 == 0){ _throw( Null_Exception);} _temp216->hd;}))->where_clause,
1); te= Cyc_Tcenv_set_fallthru( te,( struct _tuple3*) pat_typ,( struct Cyc_Absyn_Switch_clause*)({
struct Cyc_List_List* _temp217= scs; if( _temp217 == 0){ _throw( Null_Exception);}
_temp217->hd;}));}} Cyc_Tcpat_check_switch_exhaustive( s0->loc, _temp111);
return;} _LL38:( void*)( _temp117->r=( void*)(( void*)( Cyc_Absyn_seq_stmt( Cyc_Absyn_new_stmt((
void*) _temp117->r, _temp117->loc), Cyc_Absyn_skip_stmt( _temp117->loc),
_temp117->loc))->r)); Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser( Cyc_Tcenv_enter_try(
te), s0), _temp117, 1); te= Cyc_Tcenv_set_in_switch( te); te= Cyc_Tcenv_clear_fallthru(
te);{ struct Cyc_List_List* scs=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( _temp115); for( 0; scs != 0; scs=({ struct Cyc_List_List*
_temp218= scs; if( _temp218 == 0){ _throw( Null_Exception);} _temp218->tl;})){
struct Cyc_Absyn_Pat* p=(( struct Cyc_Absyn_Switch_clause*)({ struct Cyc_List_List*
_temp219= scs; if( _temp219 == 0){ _throw( Null_Exception);} _temp219->hd;}))->pattern;
struct _tuple3* pat_typ= Cyc_Tcpat_tcPat( te, p, 0); struct _tuple3 _temp222;
struct Cyc_List_List* _temp223; struct Cyc_List_List* _temp225; struct _tuple3*
_temp220= pat_typ; _temp222=* _temp220; _LL226: _temp225= _temp222.f1; goto
_LL224; _LL224: _temp223= _temp222.f2; goto _LL221; _LL221: if( ! Cyc_Tcutil_unify((
void*)({ struct Cyc_Core_Opt* _temp227= p->topt; if( _temp227 == 0){ _throw(
Null_Exception);} _temp227->v;}), Cyc_Absyn_exn_typ)){ Cyc_Tcutil_terr((( struct
Cyc_Absyn_Switch_clause*)({ struct Cyc_List_List* _temp228= scs; if( _temp228 ==
0){ _throw( Null_Exception);} _temp228->hd;}))->loc,({ struct _tagged_string
_temp230= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt* _temp229= p->topt;
if( _temp229 == 0){ _throw( Null_Exception);} _temp229->v;})); xprintf("expected xtunion exn but found %.*s",
_temp230.last_plus_one - _temp230.curr, _temp230.curr);}));} else{ Cyc_Tcpat_check_pat_regions(
te, p);}(( struct Cyc_Absyn_Switch_clause*)({ struct Cyc_List_List* _temp231=
scs; if( _temp231 == 0){ _throw( Null_Exception);} _temp231->hd;}))->pat_vars=({
struct Cyc_Core_Opt* _temp232=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp232->v=( void*) _temp223; _temp232;}); Cyc_Tcstmt_pattern_synth(((
struct Cyc_Absyn_Switch_clause*)({ struct Cyc_List_List* _temp233= scs; if(
_temp233 == 0){ _throw( Null_Exception);} _temp233->hd;}))->loc, te, _temp225,
_temp223,(( struct Cyc_Absyn_Switch_clause*)({ struct Cyc_List_List* _temp234=
scs; if( _temp234 == 0){ _throw( Null_Exception);} _temp234->hd;}))->body,((
struct Cyc_Absyn_Switch_clause*)({ struct Cyc_List_List* _temp235= scs; if(
_temp235 == 0){ _throw( Null_Exception);} _temp235->hd;}))->where_clause, 1); te=
Cyc_Tcenv_set_fallthru( te,( struct _tuple3*) pat_typ,( struct Cyc_Absyn_Switch_clause*)({
struct Cyc_List_List* _temp236= scs; if( _temp236 == 0){ _throw( Null_Exception);}
_temp236->hd;}));}} Cyc_Tcpat_check_catch_overlap( s0->loc, _temp115); return;
_LL40: { struct _tagged_string unimp_msg_part; if( new_block){ te= Cyc_Tcenv_new_block(
s0->loc, te);}{ void* _temp237=( void*) _temp121->r; struct Cyc_Absyn_Vardecl*
_temp263; int _temp265; int* _temp267; struct Cyc_Absyn_Exp* _temp268; struct
Cyc_Core_Opt* _temp270; struct Cyc_Core_Opt** _temp272; struct Cyc_Core_Opt*
_temp273; struct Cyc_Core_Opt** _temp275; struct Cyc_Absyn_Pat* _temp276; struct
Cyc_List_List* _temp278; struct _tagged_string* _temp280; struct Cyc_List_List*
_temp282; struct _tuple0* _temp284; struct Cyc_Absyn_Fndecl* _temp286; struct
Cyc_Absyn_Structdecl* _temp288; struct Cyc_Absyn_Uniondecl* _temp290; struct Cyc_Absyn_Tuniondecl*
_temp292; struct Cyc_Absyn_XTuniondecl* _temp294; struct Cyc_Absyn_Enumdecl*
_temp296; struct Cyc_Absyn_Typedefdecl* _temp298; struct Cyc_List_List* _temp300;
_LL239: if((( struct _tunion_struct*) _temp237)->tag == Cyc_Absyn_Var_d_tag){
_LL264: _temp263=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp237)->f1; goto _LL240;} else{ goto _LL241;} _LL241: if((( struct
_tunion_struct*) _temp237)->tag == Cyc_Absyn_Let_d_tag){ _LL277: _temp276=(
struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*) _temp237)->f1; goto
_LL274; _LL274: _temp273=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp237)->f2; _temp275=&(( struct Cyc_Absyn_Let_d_struct*) _temp237)->f2; goto
_LL271; _LL271: _temp270=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp237)->f3; _temp272=&(( struct Cyc_Absyn_Let_d_struct*) _temp237)->f3; goto
_LL269; _LL269: _temp268=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp237)->f4; goto _LL266; _LL266: _temp265=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp237)->f5; _temp267=&(( struct Cyc_Absyn_Let_d_struct*) _temp237)->f5; goto
_LL242;} else{ goto _LL243;} _LL243: if((( struct _tunion_struct*) _temp237)->tag
== Cyc_Absyn_Namespace_d_tag){ _LL281: _temp280=( struct _tagged_string*)((
struct Cyc_Absyn_Namespace_d_struct*) _temp237)->f1; goto _LL279; _LL279:
_temp278=( struct Cyc_List_List*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp237)->f2; goto _LL244;} else{ goto _LL245;} _LL245: if((( struct
_tunion_struct*) _temp237)->tag == Cyc_Absyn_Using_d_tag){ _LL285: _temp284=(
struct _tuple0*)(( struct Cyc_Absyn_Using_d_struct*) _temp237)->f1; goto _LL283;
_LL283: _temp282=( struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*)
_temp237)->f2; goto _LL246;} else{ goto _LL247;} _LL247: if((( struct
_tunion_struct*) _temp237)->tag == Cyc_Absyn_Fn_d_tag){ _LL287: _temp286=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*) _temp237)->f1; goto
_LL248;} else{ goto _LL249;} _LL249: if((( struct _tunion_struct*) _temp237)->tag
== Cyc_Absyn_Struct_d_tag){ _LL289: _temp288=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_d_struct*) _temp237)->f1; goto _LL250;} else{ goto
_LL251;} _LL251: if((( struct _tunion_struct*) _temp237)->tag == Cyc_Absyn_Union_d_tag){
_LL291: _temp290=( struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*)
_temp237)->f1; goto _LL252;} else{ goto _LL253;} _LL253: if((( struct
_tunion_struct*) _temp237)->tag == Cyc_Absyn_Tunion_d_tag){ _LL293: _temp292=(
struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_d_struct*) _temp237)->f1;
goto _LL254;} else{ goto _LL255;} _LL255: if((( struct _tunion_struct*) _temp237)->tag
== Cyc_Absyn_XTunion_d_tag){ _LL295: _temp294=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_d_struct*) _temp237)->f1; goto _LL256;} else{ goto
_LL257;} _LL257: if((( struct _tunion_struct*) _temp237)->tag == Cyc_Absyn_Enum_d_tag){
_LL297: _temp296=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*)
_temp237)->f1; goto _LL258;} else{ goto _LL259;} _LL259: if((( struct
_tunion_struct*) _temp237)->tag == Cyc_Absyn_Typedef_d_tag){ _LL299: _temp298=(
struct Cyc_Absyn_Typedefdecl*)(( struct Cyc_Absyn_Typedef_d_struct*) _temp237)->f1;
goto _LL260;} else{ goto _LL261;} _LL261: if((( struct _tunion_struct*) _temp237)->tag
== Cyc_Absyn_ExternC_d_tag){ _LL301: _temp300=( struct Cyc_List_List*)(( struct
Cyc_Absyn_ExternC_d_struct*) _temp237)->f1; goto _LL262;} else{ goto _LL238;}
_LL240: { struct Cyc_Absyn_Vardecl _temp304; struct Cyc_List_List* _temp305;
struct Cyc_Core_Opt* _temp307; struct Cyc_Core_Opt** _temp309; int _temp310;
struct Cyc_Absyn_Exp* _temp312; void* _temp314; struct Cyc_Absyn_Tqual* _temp316;
struct _tuple0* _temp318; struct _tuple0 _temp320; struct _tagged_string*
_temp321; void* _temp323; void* _temp325; struct Cyc_Absyn_Vardecl* _temp302=
_temp263; _temp304=* _temp302; _LL326: _temp325=( void*) _temp304.sc; goto
_LL319; _LL319: _temp318=( struct _tuple0*) _temp304.name; _temp320=* _temp318;
_LL324: _temp323= _temp320.f1; goto _LL322; _LL322: _temp321= _temp320.f2; goto
_LL317; _LL317: _temp316=( struct Cyc_Absyn_Tqual*) _temp304.tq; goto _LL315;
_LL315: _temp314=( void*) _temp304.type; goto _LL313; _LL313: _temp312=( struct
Cyc_Absyn_Exp*) _temp304.initializer; goto _LL311; _LL311: _temp310=( int)
_temp304.shadow; goto _LL308; _LL308: _temp307=( struct Cyc_Core_Opt*) _temp304.region;
_temp309=&(* _temp302).region; goto _LL306; _LL306: _temp305=( struct Cyc_List_List*)
_temp304.attributes; goto _LL303; _LL303: { void* curr_bl= Cyc_Tcenv_curr_rgn(
te);* _temp309=({ struct Cyc_Core_Opt* _temp327=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp327->v=( void*) curr_bl; _temp327;});{
void* _temp328= _temp323; struct Cyc_List_List* _temp338; struct Cyc_List_List*
_temp340; _LL330: if(( int) _temp328 == Cyc_Absyn_Loc_n){ goto _LL331;} else{
goto _LL332;} _LL332: if(( unsigned int) _temp328 > 1u?(( struct _tunion_struct*)
_temp328)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL339: _temp338=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp328)->f1; if( _temp338 == 0){ goto _LL333;}
else{ goto _LL334;}} else{ goto _LL334;} _LL334: if(( unsigned int) _temp328 > 1u?((
struct _tunion_struct*) _temp328)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL341:
_temp340=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp328)->f1;
goto _LL335;} else{ goto _LL336;} _LL336: goto _LL337; _LL331: goto _LL329;
_LL333:(* _temp263->name).f1=( void*) Cyc_Absyn_Loc_n; goto _LL329; _LL335:(
void) _throw((( struct _xtunion_struct*(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp342=( char*)"tcstmt: Abs_n declaration";
struct _tagged_string _temp343; _temp343.curr= _temp342; _temp343.base= _temp342;
_temp343.last_plus_one= _temp342 + 26; _temp343;}))); _LL337: Cyc_Tcutil_terr(
_temp121->loc,( struct _tagged_string)({ char* _temp344=( char*)"cannot declare a qualified local variable";
struct _tagged_string _temp345; _temp345.curr= _temp344; _temp345.base= _temp344;
_temp345.last_plus_one= _temp344 + 42; _temp345;})); goto _LL329; _LL329:;} Cyc_Tcutil_check_type(
s0->loc, te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind,
_temp314); if( _temp312 != 0){{ void* _temp346= Cyc_Tcutil_compress( _temp314);
struct Cyc_Absyn_Exp* _temp352; struct Cyc_Absyn_Tqual* _temp354; void* _temp356;
_LL348: if(( unsigned int) _temp346 > 4u?(( struct _tunion_struct*) _temp346)->tag
== Cyc_Absyn_ArrayType_tag: 0){ _LL357: _temp356=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp346)->f1; goto _LL355; _LL355: _temp354=( struct Cyc_Absyn_Tqual*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp346)->f2; goto _LL353; _LL353: _temp352=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*) _temp346)->f3; goto
_LL349;} else{ goto _LL350;} _LL350: goto _LL351; _LL349: if( _temp352 == 0){
void* _temp359=( void*)({ struct Cyc_Absyn_Exp* _temp358= _temp312; if( _temp358
== 0){ _throw( Null_Exception);} _temp358->r;}); void* _temp369; struct
_tagged_string _temp371; struct Cyc_List_List* _temp373; struct Cyc_Core_Opt*
_temp375; struct Cyc_List_List* _temp377; _LL361: if((( struct _tunion_struct*)
_temp359)->tag == Cyc_Absyn_Const_e_tag){ _LL370: _temp369=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp359)->f1; if(( unsigned int) _temp369 > 1u?(( struct _tunion_struct*)
_temp369)->tag == Cyc_Absyn_String_c_tag: 0){ _LL372: _temp371=( struct
_tagged_string)(( struct Cyc_Absyn_String_c_struct*) _temp369)->f1; goto _LL362;}
else{ goto _LL363;}} else{ goto _LL363;} _LL363: if((( struct _tunion_struct*)
_temp359)->tag == Cyc_Absyn_UnresolvedMem_e_tag){ _LL376: _temp375=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp359)->f1; goto _LL374; _LL374:
_temp373=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp359)->f2; goto _LL364;} else{ goto _LL365;} _LL365: if((( struct
_tunion_struct*) _temp359)->tag == Cyc_Absyn_Array_e_tag){ _LL378: _temp377=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp359)->f1; goto
_LL366;} else{ goto _LL367;} _LL367: goto _LL368; _LL362: _temp314=( void*)(
_temp263->type=( void*)(( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp379=(
struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp379[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp380; _temp380.tag= Cyc_Absyn_ArrayType_tag;
_temp380.f1=( void*) _temp356; _temp380.f2= _temp354; _temp380.f3=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(({ struct _tagged_string _temp381= _temp371;( unsigned int)(
_temp381.last_plus_one - _temp381.curr);}), 0); _temp380;}); _temp379;}))); goto
_LL360; _LL364: _temp377= _temp373; goto _LL366; _LL366: _temp314=( void*)(
_temp263->type=( void*)(( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp382=(
struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp382[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp383; _temp383.tag= Cyc_Absyn_ArrayType_tag;
_temp383.f1=( void*) _temp356; _temp383.f2= _temp354; _temp383.f3=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(( unsigned int)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp377), 0); _temp383;}); _temp382;}))); goto _LL360; _LL368: goto _LL360;
_LL360:;} goto _LL347; _LL351: goto _LL347; _LL347:;} Cyc_Tcexp_tcExpInitializer(
te,( void**)& _temp314,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp384=
_temp312; if( _temp384 == 0){ _throw( Null_Exception);} _temp384;})); if( ! Cyc_Tcutil_coerce_assign(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp385= _temp312; if(
_temp385 == 0){ _throw( Null_Exception);} _temp385;}), _temp314)){ Cyc_Tcutil_terr(
_temp121->loc,({ struct _tagged_string _temp388=* _temp321; struct
_tagged_string _temp389= Cyc_Absynpp_typ2string( _temp314); struct
_tagged_string _temp390= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp387=({ struct Cyc_Absyn_Exp* _temp386= _temp312; if( _temp386 == 0){ _throw(
Null_Exception);} _temp386->topt;}); if( _temp387 == 0){ _throw( Null_Exception);}
_temp387->v;})); xprintf("%.*s declared with type %.*s, initialized with type %.*s",
_temp388.last_plus_one - _temp388.curr, _temp388.curr, _temp389.last_plus_one -
_temp389.curr, _temp389.curr, _temp390.last_plus_one - _temp390.curr, _temp390.curr);}));}}
Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser( Cyc_Tcenv_add_local_var( _temp119->loc,
te, _temp263), s0), _temp119, 0); return;}} _LL242: { struct Cyc_Core_Opt* topt=*
_temp272; Cyc_Tcexp_tcExpInitializer( te, 0, _temp268);* _temp272= _temp268->topt;{
struct _tuple3 _temp393; struct Cyc_List_List* _temp394; struct Cyc_List_List*
_temp396; struct _tuple3* _temp391= Cyc_Tcpat_tcPat( te, _temp276, 0); _temp393=*
_temp391; _LL397: _temp396= _temp393.f1; goto _LL395; _LL395: _temp394= _temp393.f2;
goto _LL392; _LL392:* _temp275=({ struct Cyc_Core_Opt* _temp398=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp398->v=( void*) _temp394;
_temp398;}); if( _temp396 != 0){ Cyc_Tcutil_terr( _temp121->loc,( struct
_tagged_string)({ char* _temp399=( char*)"can't introduce type variables in let-clause";
struct _tagged_string _temp400; _temp400.curr= _temp399; _temp400.base= _temp399;
_temp400.last_plus_one= _temp399 + 45; _temp400;}));} if( topt != 0? ! Cyc_Tcutil_unify((
void*)({ struct Cyc_Core_Opt* _temp401= _temp276->topt; if( _temp401 == 0){
_throw( Null_Exception);} _temp401->v;}),( void*)({ struct Cyc_Core_Opt*
_temp402= topt; if( _temp402 == 0){ _throw( Null_Exception);} _temp402->v;})): 0){
Cyc_Tcutil_terr( _temp121->loc,( struct _tagged_string)({ char* _temp403=( char*)"type of pattern does not match declared type";
struct _tagged_string _temp404; _temp404.curr= _temp403; _temp404.base= _temp403;
_temp404.last_plus_one= _temp403 + 45; _temp404;}));} if( ! Cyc_Tcutil_unify((
void*)({ struct Cyc_Core_Opt* _temp405= _temp276->topt; if( _temp405 == 0){
_throw( Null_Exception);} _temp405->v;}),( void*)({ struct Cyc_Core_Opt*
_temp406= _temp268->topt; if( _temp406 == 0){ _throw( Null_Exception);} _temp406->v;}))){
Cyc_Tcutil_terr( _temp121->loc,({ struct _tagged_string _temp409= Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Core_Opt* _temp407= _temp276->topt; if( _temp407 == 0){
_throw( Null_Exception);} _temp407->v;})); struct _tagged_string _temp410= Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Core_Opt* _temp408= _temp268->topt; if( _temp408 == 0){
_throw( Null_Exception);} _temp408->v;})); xprintf("pattern type %.*s does not match definition type %.*s",
_temp409.last_plus_one - _temp409.curr, _temp409.curr, _temp410.last_plus_one -
_temp410.curr, _temp410.curr);}));} else{ Cyc_Tcpat_check_pat_regions( te,
_temp276);}* _temp267= Cyc_Tcpat_check_let_pat_exhaustive( _temp276->loc,
_temp276); Cyc_Tcstmt_pattern_synth( s0->loc, te, 0, _temp394, _temp119, 0, 0);
return;}} _LL244: unimp_msg_part=( struct _tagged_string)({ char* _temp411=(
char*)"namespace"; struct _tagged_string _temp412; _temp412.curr= _temp411;
_temp412.base= _temp411; _temp412.last_plus_one= _temp411 + 10; _temp412;});
goto _LL238; _LL246: unimp_msg_part=( struct _tagged_string)({ char* _temp413=(
char*)"using"; struct _tagged_string _temp414; _temp414.curr= _temp413; _temp414.base=
_temp413; _temp414.last_plus_one= _temp413 + 6; _temp414;}); goto _LL238; _LL248:
unimp_msg_part=( struct _tagged_string)({ char* _temp415=( char*)"function";
struct _tagged_string _temp416; _temp416.curr= _temp415; _temp416.base= _temp415;
_temp416.last_plus_one= _temp415 + 9; _temp416;}); goto _LL238; _LL250:
unimp_msg_part=( struct _tagged_string)({ char* _temp417=( char*)"struct";
struct _tagged_string _temp418; _temp418.curr= _temp417; _temp418.base= _temp417;
_temp418.last_plus_one= _temp417 + 7; _temp418;}); goto _LL238; _LL252:
unimp_msg_part=( struct _tagged_string)({ char* _temp419=( char*)"union"; struct
_tagged_string _temp420; _temp420.curr= _temp419; _temp420.base= _temp419;
_temp420.last_plus_one= _temp419 + 6; _temp420;}); goto _LL238; _LL254:
unimp_msg_part=( struct _tagged_string)({ char* _temp421=( char*)"tunion";
struct _tagged_string _temp422; _temp422.curr= _temp421; _temp422.base= _temp421;
_temp422.last_plus_one= _temp421 + 7; _temp422;}); goto _LL238; _LL256:
unimp_msg_part=( struct _tagged_string)({ char* _temp423=( char*)"xtunion";
struct _tagged_string _temp424; _temp424.curr= _temp423; _temp424.base= _temp423;
_temp424.last_plus_one= _temp423 + 8; _temp424;}); goto _LL238; _LL258:
unimp_msg_part=( struct _tagged_string)({ char* _temp425=( char*)"enum"; struct
_tagged_string _temp426; _temp426.curr= _temp425; _temp426.base= _temp425;
_temp426.last_plus_one= _temp425 + 5; _temp426;}); goto _LL238; _LL260:
unimp_msg_part=( struct _tagged_string)({ char* _temp427=( char*)"typedef";
struct _tagged_string _temp428; _temp428.curr= _temp427; _temp428.base= _temp427;
_temp428.last_plus_one= _temp427 + 8; _temp428;}); goto _LL238; _LL262:
unimp_msg_part=( struct _tagged_string)({ char* _temp429=( char*)"extern \"C\"";
struct _tagged_string _temp430; _temp430.curr= _temp429; _temp430.base= _temp429;
_temp430.last_plus_one= _temp429 + 11; _temp430;}); goto _LL238; _LL238:;}( void)
_throw((( struct _xtunion_struct*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
struct _tagged_string _temp431= unimp_msg_part; xprintf("tcStmt: nested %.*s declarations unimplemented",
_temp431.last_plus_one - _temp431.curr, _temp431.curr);})));} _LL42:( void)
_throw((( struct _xtunion_struct*(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp432=( char*)"tcStmt: cut is not implemented";
struct _tagged_string _temp433; _temp433.curr= _temp432; _temp433.base= _temp432;
_temp433.last_plus_one= _temp432 + 31; _temp433;}))); _LL44:( void) _throw(((
struct _xtunion_struct*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ char* _temp434=( char*)"tcStmt: splice is not implemented";
struct _tagged_string _temp435; _temp435.curr= _temp434; _temp435.base= _temp434;
_temp435.last_plus_one= _temp434 + 34; _temp435;}))); _LL8:;}}