#include "cyc_include.h"

 struct _tuple0{ void* f1; void* f2; } ; struct _tuple1{ void* f1; struct
_tagged_string* f2; } ; struct _tuple2{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual*
f2; void* f3; } ; struct _tuple3{ struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt*
f2; } ; struct _tuple4{ struct Cyc_Absyn_Tvar* f1; void* f2; } ; struct _tuple5{
int f1; void* f2; } ; struct _tuple6{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp*
f2; } ; struct _tuple7{ struct Cyc_Absyn_Tqual* f1; void* f2; } ; struct _tuple8{
struct Cyc_Absyn_Structfield* f1; struct Cyc_Absyn_Exp* f2; } ; typedef int Cyc_ptrdiff_t;
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
extern char Cyc_Core_InvalidArg_tag[ 11u]; struct Cyc_Core_InvalidArg_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Core_Failure_tag[ 8u];
struct Cyc_Core_Failure_struct{ char* tag; struct _tagged_string f1; } ; extern
char Cyc_Core_Impossible_tag[ 11u]; struct Cyc_Core_Impossible_struct{ char* tag;
struct _tagged_string f1; } ; extern char Cyc_Core_Not_found_tag[ 10u]; struct
Cyc_Core_Not_found_struct{ char* tag; } ; extern char Cyc_Core_Unreachable_tag[
12u]; struct Cyc_Core_Unreachable_struct{ char* tag; struct _tagged_string f1; }
; extern char* string_to_Cstring( struct _tagged_string); extern char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( char*); extern int system( char*); struct Cyc_List_List{ void*
hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List* Cyc_List_glist_t;
typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t;
extern int Cyc_List_length( struct Cyc_List_List* x); extern char Cyc_List_List_empty_tag[
11u]; struct Cyc_List_List_empty_struct{ char* tag; } ; extern struct Cyc_List_List*
Cyc_List_map( void*(* f)( void*), struct Cyc_List_List* x); extern char Cyc_List_List_mismatch_tag[
14u]; struct Cyc_List_List_mismatch_struct{ char* tag; } ; extern struct Cyc_List_List*
Cyc_List_imp_rev( struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_append(
struct Cyc_List_List* x, struct Cyc_List_List* y); extern char Cyc_List_Nth_tag[
4u]; struct Cyc_List_Nth_struct{ char* tag; } ; extern void* Cyc_List_nth(
struct Cyc_List_List* x, int i); extern struct Cyc_List_List* Cyc_List_zip(
struct Cyc_List_List* x, struct Cyc_List_List* y); struct Cyc_Lineno_Pos{ struct
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
char* tag; } ; extern struct Cyc_Absyn_Tqual* Cyc_Absyn_empty_tqual(); extern
struct Cyc_Absyn_Conref* Cyc_Absyn_new_conref( void* x); extern struct Cyc_Absyn_Conref*
Cyc_Absyn_empty_conref(); extern void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref*
x); extern void* Cyc_Absyn_new_evar( void*); extern void* Cyc_Absyn_wildtyp();
extern void* Cyc_Absyn_uchar_t; extern void* Cyc_Absyn_ushort_t; extern void*
Cyc_Absyn_uint_t; extern void* Cyc_Absyn_ulong_t; extern void* Cyc_Absyn_schar_t;
extern void* Cyc_Absyn_sshort_t; extern void* Cyc_Absyn_sint_t; extern void* Cyc_Absyn_slong_t;
extern void* Cyc_Absyn_float_t; extern void* Cyc_Absyn_exn_typ; extern void* Cyc_Absyn_string_typ(
void* rgn); extern void* Cyc_Absyn_file_typ(); extern void* Cyc_Absyn_atb_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual* tq, void* b); extern void* Cyc_Absyn_star_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual* tq); extern void* Cyc_Absyn_at_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual* tq); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_new_exp( void*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_copy_exp( struct Cyc_Absyn_Exp*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp(
unsigned int, struct Cyc_Position_Segment*); extern void* Cyc_Absyn_pointer_expand(
void*); extern int Cyc_Absyn_is_lvalue( struct Cyc_Absyn_Exp*); extern struct
Cyc_Absyn_Structfield* Cyc_Absyn_lookup_struct_field( struct Cyc_Absyn_Structdecl*,
struct _tagged_string*); extern struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_union_field(
struct Cyc_Absyn_Uniondecl*, struct _tagged_string*); struct Cyc_Stdio___sFILE;
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; typedef int Cyc_Stdio_fpos_t;
extern char Cyc_Stdio_FileOpenError_tag[ 14u]; struct Cyc_Stdio_FileOpenError_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Stdio_FileCloseError_tag[
15u]; struct Cyc_Stdio_FileCloseError_struct{ char* tag; } ; extern int Cyc_String_zstrcmp(
struct _tagged_string, struct _tagged_string); struct Cyc_PP_Ppstate; typedef
struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t; struct Cyc_PP_Out; typedef struct Cyc_PP_Out*
Cyc_PP_out_t; struct Cyc_PP_Doc; typedef struct Cyc_PP_Doc* Cyc_PP_doc_t; extern
struct _tagged_string Cyc_Absynpp_typ2string( void*); extern struct
_tagged_string Cyc_Absynpp_qvar2string( struct _tuple1*); struct Cyc_Set_Set;
typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct Cyc_Set_Set* Cyc_Set_hset_t;
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
extern void* Cyc_Tcenv_lookup_ordinary( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tuple1*); extern struct Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple1*); extern
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_local_var( struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Vardecl*); extern struct Cyc_List_List*
Cyc_Tcenv_lookup_type_vars( struct Cyc_Tcenv_Tenv*); extern struct Cyc_Absyn_Stmt*
Cyc_Tcenv_get_encloser( struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_set_encloser( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*); extern
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_new_block( struct Cyc_Position_Segment*, struct
Cyc_Tcenv_Tenv*); extern void Cyc_Tcenv_check_rgn_accessible( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, void* rgn); extern void Cyc_Tcenv_check_effect_accessible(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* eff); extern
char Cyc_Tcutil_TypeErr_tag[ 8u]; struct Cyc_Tcutil_TypeErr_struct{ char* tag; }
; extern void* Cyc_Tcutil_impos( struct _tagged_string); extern void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*, struct _tagged_string); extern void Cyc_Tcutil_warn(
struct Cyc_Position_Segment*, struct _tagged_string); extern void* Cyc_Tcutil_compress(
void* t); extern void Cyc_Tcutil_unchecked_cast( struct Cyc_Tcenv_Tenv*, struct
Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_arg( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_assign( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_to_bool( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerce_list( struct Cyc_Tcenv_Tenv*,
void*, struct Cyc_List_List*); extern int Cyc_Tcutil_coerce_uint_typ( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerce_sint_typ( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerce_use( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerceable( void*); extern
int Cyc_Tcutil_silent_castable( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
void*, void*); extern int Cyc_Tcutil_castable( struct Cyc_Tcenv_Tenv*, struct
Cyc_Position_Segment*, void*, void*); extern int Cyc_Tcutil_is_integral( struct
Cyc_Absyn_Exp*); extern int Cyc_Tcutil_is_numeric( struct Cyc_Absyn_Exp*);
extern void* Cyc_Tcutil_max_arithmetic_type( void*, void*); extern int Cyc_Tcutil_unify(
void*, void*); extern void* Cyc_Tcutil_substitute( struct Cyc_List_List*, void*);
extern void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl*); extern struct
_tuple4* Cyc_Tcutil_make_inst_var( struct Cyc_Absyn_Tvar*); extern void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*); extern void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*, struct Cyc_List_List* bound_tvars, void* k, void*);
extern void Cyc_Tcutil_check_nonzero_bound( struct Cyc_Position_Segment*, struct
Cyc_Absyn_Conref*); extern void Cyc_Tcutil_check_bound( struct Cyc_Position_Segment*,
unsigned int i, struct Cyc_Absyn_Conref*); extern struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* des, struct Cyc_Absyn_Structdecl*
sd); extern int Cyc_Tcutil_is_tagged_pointer_typ( void*); extern struct _tuple5
Cyc_Tcutil_addressof_props( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e);
extern int Cyc_Tcutil_is_const_exp( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e); extern unsigned int Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp* e);
extern unsigned int Cyc_Evexp_szof( void* t, struct Cyc_Position_Segment* loc);
extern void Cyc_Tcstmt_tcStmt( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt*
s, int new_block); extern struct Cyc_List_List* Cyc_Formatstr_get_format_typs(
struct _tagged_string, struct Cyc_Position_Segment*); extern struct Cyc_List_List*
Cyc_Formatstr_get_scanf_typs( struct _tagged_string, struct Cyc_Position_Segment*);
extern void* Cyc_Tcexp_tcExp( struct Cyc_Tcenv_Tenv*, void**, struct Cyc_Absyn_Exp*);
extern void* Cyc_Tcexp_tcExpInitializer( struct Cyc_Tcenv_Tenv*, void**, struct
Cyc_Absyn_Exp*); extern void Cyc_Tcexp_tcTest( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Exp* e, struct _tagged_string msg_part); typedef void* Cyc_CfFlowInfo_LocalRoot;
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
ds); static void* Cyc_Tcexp_expr_err( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct _tagged_string msg){ Cyc_Tcutil_terr( loc, msg); return Cyc_Absyn_wildtyp();}
static void Cyc_Tcexp_resolve_unknown_id( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, struct _tuple1* q){ struct _handler_cons _temp0; _push_handler(& _temp0);{
struct _xtunion_struct* _temp1=( struct _xtunion_struct*) setjmp( _temp0.handler);
if( ! _temp1){{ void* _temp2= Cyc_Tcenv_lookup_ordinary( te, e->loc, q); void*
_temp14; struct Cyc_Absyn_Enumfield* _temp16; struct Cyc_Absyn_Enumdecl* _temp18;
struct Cyc_Absyn_Tunionfield* _temp20; struct Cyc_Absyn_Tuniondecl* _temp22;
struct Cyc_Absyn_Tunionfield* _temp24; struct Cyc_Absyn_XTuniondecl* _temp26;
struct Cyc_Absyn_Structdecl* _temp28; _LL4: if((( struct _tunion_struct*) _temp2)->tag
== Cyc_Tcenv_VarRes_tag){ _LL15: _temp14=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp2)->f1; goto _LL5;} else{ goto _LL6;} _LL6: if((( struct _tunion_struct*)
_temp2)->tag == Cyc_Tcenv_EnumRes_tag){ _LL19: _temp18=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Tcenv_EnumRes_struct*) _temp2)->f1; goto _LL17; _LL17: _temp16=(
struct Cyc_Absyn_Enumfield*)(( struct Cyc_Tcenv_EnumRes_struct*) _temp2)->f2;
goto _LL7;} else{ goto _LL8;} _LL8: if((( struct _tunion_struct*) _temp2)->tag
== Cyc_Tcenv_TunionRes_tag){ _LL23: _temp22=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Tcenv_TunionRes_struct*) _temp2)->f1; goto _LL21; _LL21: _temp20=(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Tcenv_TunionRes_struct*) _temp2)->f2;
goto _LL9;} else{ goto _LL10;} _LL10: if((( struct _tunion_struct*) _temp2)->tag
== Cyc_Tcenv_XTunionRes_tag){ _LL27: _temp26=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Tcenv_XTunionRes_struct*) _temp2)->f1; goto _LL25; _LL25: _temp24=(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Tcenv_XTunionRes_struct*) _temp2)->f2;
goto _LL11;} else{ goto _LL12;} _LL12: if((( struct _tunion_struct*) _temp2)->tag
== Cyc_Tcenv_StructRes_tag){ _LL29: _temp28=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Tcenv_StructRes_struct*) _temp2)->f1; goto _LL13;} else{ goto _LL3;}
_LL5:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Var_e_struct* _temp30=(
struct Cyc_Absyn_Var_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_e_struct));
_temp30[ 0]=({ struct Cyc_Absyn_Var_e_struct _temp31; _temp31.tag= Cyc_Absyn_Var_e_tag;
_temp31.f1= q; _temp31.f2=( void*) _temp14; _temp31;}); _temp30;}))); goto _LL3;
_LL7:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Enum_e_struct* _temp32=(
struct Cyc_Absyn_Enum_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_e_struct));
_temp32[ 0]=({ struct Cyc_Absyn_Enum_e_struct _temp33; _temp33.tag= Cyc_Absyn_Enum_e_tag;
_temp33.f1= q; _temp33.f2=( struct Cyc_Absyn_Enumdecl*) _temp18; _temp33.f3=(
struct Cyc_Absyn_Enumfield*) _temp16; _temp33;}); _temp32;}))); goto _LL3; _LL9:(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Tunion_e_struct* _temp34=(
struct Cyc_Absyn_Tunion_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_e_struct));
_temp34[ 0]=({ struct Cyc_Absyn_Tunion_e_struct _temp35; _temp35.tag= Cyc_Absyn_Tunion_e_tag;
_temp35.f1= 0; _temp35.f2= 0; _temp35.f3= 0; _temp35.f4= _temp22; _temp35.f5=
_temp20; _temp35;}); _temp34;}))); goto _LL3; _LL11:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_XTunion_e_struct* _temp36=( struct Cyc_Absyn_XTunion_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_XTunion_e_struct)); _temp36[ 0]=({ struct
Cyc_Absyn_XTunion_e_struct _temp37; _temp37.tag= Cyc_Absyn_XTunion_e_tag;
_temp37.f1= 0; _temp37.f2= 0; _temp37.f3= _temp26; _temp37.f4= _temp24; _temp37;});
_temp36;}))); goto _LL3; _LL13: Cyc_Tcutil_terr( e->loc,({ struct _tagged_string
_temp38= Cyc_Absynpp_qvar2string( q); xprintf("bad occurrence of struct name %.*s",
_temp38.last_plus_one - _temp38.curr, _temp38.curr);}));( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Var_e_struct* _temp39=( struct Cyc_Absyn_Var_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_e_struct)); _temp39[ 0]=({ struct Cyc_Absyn_Var_e_struct
_temp40; _temp40.tag= Cyc_Absyn_Var_e_tag; _temp40.f1= q; _temp40.f2=( void*)((
void*) Cyc_Absyn_Unresolved_b); _temp40;}); _temp39;}))); goto _LL3; _LL3:;};
_pop_handler();} else{ struct _xtunion_struct* _temp42= _temp1; _LL44: if(
_temp42->tag == Cyc_Dict_Absent_tag){ goto _LL45;} else{ goto _LL46;} _LL46:
goto _LL47; _LL45:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Var_e_struct*
_temp48=( struct Cyc_Absyn_Var_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_e_struct));
_temp48[ 0]=({ struct Cyc_Absyn_Var_e_struct _temp49; _temp49.tag= Cyc_Absyn_Var_e_tag;
_temp49.f1= q; _temp49.f2=( void*)(( void*) Cyc_Absyn_Unresolved_b); _temp49;});
_temp48;}))); goto _LL43; _LL47:( void) _throw( _temp42); _LL43:;}}} static
struct _tuple6* Cyc_Tcexp_make_struct_arg( struct Cyc_Absyn_Exp* e){ return({
struct _tuple6* _temp50=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6));
_temp50->f1= 0; _temp50->f2= e; _temp50;});} static void Cyc_Tcexp_resolve_unknown_fn(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Exp* e1,
struct Cyc_List_List* es){ void* _temp51=( void*) e1->r; struct _tuple1* _temp57;
_LL53: if((( struct _tunion_struct*) _temp51)->tag == Cyc_Absyn_UnknownId_e_tag){
_LL58: _temp57=( struct _tuple1*)(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp51)->f1; goto _LL54;} else{ goto _LL55;} _LL55: goto _LL56; _LL54: { struct
_handler_cons _temp59; _push_handler(& _temp59);{ struct _xtunion_struct*
_temp60=( struct _xtunion_struct*) setjmp( _temp59.handler); if( ! _temp60){{
void* _temp61= Cyc_Tcenv_lookup_ordinary( te, e1->loc, _temp57); void* _temp73;
struct Cyc_Absyn_Tunionfield* _temp75; struct Cyc_Absyn_Tuniondecl* _temp77;
struct Cyc_Absyn_Tunionfield* _temp79; struct Cyc_Absyn_XTuniondecl* _temp81;
struct Cyc_Absyn_Structdecl* _temp83; struct Cyc_Absyn_Enumfield* _temp85;
struct Cyc_Absyn_Enumdecl* _temp87; _LL63: if((( struct _tunion_struct*) _temp61)->tag
== Cyc_Tcenv_VarRes_tag){ _LL74: _temp73=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp61)->f1; goto _LL64;} else{ goto _LL65;} _LL65: if((( struct _tunion_struct*)
_temp61)->tag == Cyc_Tcenv_TunionRes_tag){ _LL78: _temp77=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Tcenv_TunionRes_struct*) _temp61)->f1; goto _LL76; _LL76: _temp75=(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Tcenv_TunionRes_struct*) _temp61)->f2;
goto _LL66;} else{ goto _LL67;} _LL67: if((( struct _tunion_struct*) _temp61)->tag
== Cyc_Tcenv_XTunionRes_tag){ _LL82: _temp81=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Tcenv_XTunionRes_struct*) _temp61)->f1; goto _LL80; _LL80: _temp79=(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Tcenv_XTunionRes_struct*) _temp61)->f2;
goto _LL68;} else{ goto _LL69;} _LL69: if((( struct _tunion_struct*) _temp61)->tag
== Cyc_Tcenv_StructRes_tag){ _LL84: _temp83=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Tcenv_StructRes_struct*) _temp61)->f1; goto _LL70;} else{ goto _LL71;}
_LL71: if((( struct _tunion_struct*) _temp61)->tag == Cyc_Tcenv_EnumRes_tag){
_LL88: _temp87=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Tcenv_EnumRes_struct*)
_temp61)->f1; goto _LL86; _LL86: _temp85=( struct Cyc_Absyn_Enumfield*)(( struct
Cyc_Tcenv_EnumRes_struct*) _temp61)->f2; goto _LL72;} else{ goto _LL62;} _LL64:(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_FnCall_e_struct* _temp89=(
struct Cyc_Absyn_FnCall_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct));
_temp89[ 0]=({ struct Cyc_Absyn_FnCall_e_struct _temp90; _temp90.tag= Cyc_Absyn_FnCall_e_tag;
_temp90.f1= e1; _temp90.f2= es; _temp90;}); _temp89;}))); _npop_handler( 0u);
return; _LL66: if( _temp75->typs == 0){ Cyc_Tcutil_terr( e->loc,({ struct
_tagged_string _temp91= Cyc_Absynpp_qvar2string( _temp75->name); xprintf("%.*s is a constant, not a function",
_temp91.last_plus_one - _temp91.curr, _temp91.curr);}));}( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Tunion_e_struct* _temp92=( struct Cyc_Absyn_Tunion_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunion_e_struct)); _temp92[ 0]=({ struct Cyc_Absyn_Tunion_e_struct
_temp93; _temp93.tag= Cyc_Absyn_Tunion_e_tag; _temp93.f1= 0; _temp93.f2= 0;
_temp93.f3= es; _temp93.f4= _temp77; _temp93.f5= _temp75; _temp93;}); _temp92;})));
_npop_handler( 0u); return; _LL68: if( _temp79->typs == 0){ Cyc_Tcutil_terr( e->loc,({
struct _tagged_string _temp94= Cyc_Absynpp_qvar2string( _temp79->name); xprintf("%.*s is a constant, not a function",
_temp94.last_plus_one - _temp94.curr, _temp94.curr);}));}( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_XTunion_e_struct* _temp95=( struct Cyc_Absyn_XTunion_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_XTunion_e_struct)); _temp95[ 0]=({ struct
Cyc_Absyn_XTunion_e_struct _temp96; _temp96.tag= Cyc_Absyn_XTunion_e_tag;
_temp96.f1= 0; _temp96.f2= es; _temp96.f3= _temp81; _temp96.f4= _temp79; _temp96;});
_temp95;}))); _npop_handler( 0u); return; _LL70: { struct Cyc_List_List* des=((
struct Cyc_List_List*(*)( struct _tuple6*(* f)( struct Cyc_Absyn_Exp*), struct
Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcexp_make_struct_arg, es); if( _temp83->name
!= 0){( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_e_struct*
_temp97=( struct Cyc_Absyn_Struct_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct));
_temp97[ 0]=({ struct Cyc_Absyn_Struct_e_struct _temp98; _temp98.tag= Cyc_Absyn_Struct_e_tag;
_temp98.f1=( struct _tuple1*)({ struct Cyc_Core_Opt* _temp99= _temp83->name; if(
_temp99 == 0){ _throw( Null_Exception);} _temp99->v;}); _temp98.f2= 0; _temp98.f3=
des; _temp98.f4=( struct Cyc_Absyn_Structdecl*) _temp83; _temp98;}); _temp97;})));}
else{ Cyc_Tcutil_terr( e->loc,( struct _tagged_string)({ char* _temp100=( char*)"missing struct name";
struct _tagged_string _temp101; _temp101.curr= _temp100; _temp101.base= _temp100;
_temp101.last_plus_one= _temp100 + 20; _temp101;}));( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Struct_e_struct* _temp102=( struct Cyc_Absyn_Struct_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp102[ 0]=({ struct
Cyc_Absyn_Struct_e_struct _temp103; _temp103.tag= Cyc_Absyn_Struct_e_tag;
_temp103.f1= _temp57; _temp103.f2= 0; _temp103.f3= des; _temp103.f4=( struct Cyc_Absyn_Structdecl*)
_temp83; _temp103;}); _temp102;})));} _npop_handler( 0u); return;} _LL72: Cyc_Tcutil_terr(
e->loc,({ struct _tagged_string _temp104= Cyc_Absynpp_qvar2string( _temp57);
xprintf("%.*s is an enum constructor, not a function", _temp104.last_plus_one -
_temp104.curr, _temp104.curr);})); _npop_handler( 0u); return; _LL62:;};
_pop_handler();} else{ struct _xtunion_struct* _temp106= _temp60; _LL108: if(
_temp106->tag == Cyc_Dict_Absent_tag){ goto _LL109;} else{ goto _LL110;} _LL110:
goto _LL111; _LL109: Cyc_Tcutil_terr( e1->loc,({ struct _tagged_string _temp112=
Cyc_Absynpp_qvar2string( _temp57); xprintf("unbound identifier %.*s", _temp112.last_plus_one
- _temp112.curr, _temp112.curr);}));( void*)( e->r=( void*)(( void*)({ struct
Cyc_Absyn_FnCall_e_struct* _temp113=( struct Cyc_Absyn_FnCall_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct)); _temp113[ 0]=({ struct
Cyc_Absyn_FnCall_e_struct _temp114; _temp114.tag= Cyc_Absyn_FnCall_e_tag;
_temp114.f1= e1; _temp114.f2= es; _temp114;}); _temp113;}))); return; _LL111:(
void) _throw( _temp106); _LL107:;}}} _LL56:( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_FnCall_e_struct* _temp115=( struct Cyc_Absyn_FnCall_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct)); _temp115[ 0]=({ struct
Cyc_Absyn_FnCall_e_struct _temp116; _temp116.tag= Cyc_Absyn_FnCall_e_tag;
_temp116.f1= e1; _temp116.f2= es; _temp116;}); _temp115;}))); return; _LL52:;}
static void Cyc_Tcexp_resolve_unresolved_mem( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_List_List*
des){ if( topt == 0){( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct*
_temp117=( struct Cyc_Absyn_Array_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp117[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp118; _temp118.tag= Cyc_Absyn_Array_e_tag;
_temp118.f1= des; _temp118;}); _temp117;}))); return;}{ void* t=({ void**
_temp149= topt; if( _temp149 == 0){ _throw( Null_Exception);}* _temp149;}); void*
_temp119= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Exp* _temp127; struct Cyc_Absyn_Tqual*
_temp129; void* _temp131; struct Cyc_Absyn_Structdecl** _temp133; struct Cyc_List_List*
_temp135; struct _tuple1* _temp137; _LL121: if(( unsigned int) _temp119 > 4u?((
struct _tunion_struct*) _temp119)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL132:
_temp131=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp119)->f1; goto
_LL130; _LL130: _temp129=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp119)->f2; goto _LL128; _LL128: _temp127=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp119)->f3; goto _LL122;} else{ goto _LL123;}
_LL123: if(( unsigned int) _temp119 > 4u?(( struct _tunion_struct*) _temp119)->tag
== Cyc_Absyn_StructType_tag: 0){ _LL138: _temp137=( struct _tuple1*)(( struct
Cyc_Absyn_StructType_struct*) _temp119)->f1; goto _LL136; _LL136: _temp135=(
struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*) _temp119)->f2;
goto _LL134; _LL134: _temp133=( struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*)
_temp119)->f3; goto _LL124;} else{ goto _LL125;} _LL125: goto _LL126; _LL122:(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct* _temp139=(
struct Cyc_Absyn_Array_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp139[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp140; _temp140.tag= Cyc_Absyn_Array_e_tag;
_temp140.f1= des; _temp140;}); _temp139;}))); goto _LL120; _LL124: if( _temp133
== 0? 1: _temp137 == 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp141=( char*)"Compiler bug: struct type not properly set"; struct
_tagged_string _temp142; _temp142.curr= _temp141; _temp142.base= _temp141;
_temp142.last_plus_one= _temp141 + 43; _temp142;}));}( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Struct_e_struct* _temp143=( struct Cyc_Absyn_Struct_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp143[ 0]=({ struct
Cyc_Absyn_Struct_e_struct _temp144; _temp144.tag= Cyc_Absyn_Struct_e_tag;
_temp144.f1=( struct _tuple1*)({ struct _tuple1* _temp145= _temp137; if(
_temp145 == 0){ _throw( Null_Exception);} _temp145;}); _temp144.f2= 0; _temp144.f3=
des; _temp144.f4=( struct Cyc_Absyn_Structdecl*)({ struct Cyc_Absyn_Structdecl**
_temp146= _temp133; if( _temp146 == 0){ _throw( Null_Exception);}* _temp146;});
_temp144;}); _temp143;}))); goto _LL120; _LL126:( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_Array_e_struct* _temp147=( struct Cyc_Absyn_Array_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct)); _temp147[ 0]=({ struct Cyc_Absyn_Array_e_struct
_temp148; _temp148.tag= Cyc_Absyn_Array_e_tag; _temp148.f1= des; _temp148;});
_temp147;}))); goto _LL120; _LL120:;}} static void Cyc_Tcexp_tcExpNoInst( struct
Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e); static void* Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e); static void
Cyc_Tcexp_tcExpList( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* es){ for(
0; es != 0; es=({ struct Cyc_List_List* _temp150= es; if( _temp150 == 0){ _throw(
Null_Exception);} _temp150->tl;})){ Cyc_Tcexp_tcExp( te, 0,( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp151= es; if( _temp151 == 0){ _throw( Null_Exception);}
_temp151->hd;}));}} void Cyc_Tcexp_tcTest( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, struct _tagged_string msg_part){ Cyc_Tcutil_check_contains_assign( e); Cyc_Tcexp_tcExp(
te,( void**)& Cyc_Absyn_sint_t, e); if( ! Cyc_Tcutil_coerce_to_bool( te, e)){
Cyc_Tcutil_terr( e->loc,({ struct _tagged_string _temp153= msg_part; struct
_tagged_string _temp154= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp152= e->topt; if( _temp152 == 0){ _throw( Null_Exception);} _temp152->v;}));
xprintf("test of %.*s has type %.*s instead of integral or * type", _temp153.last_plus_one
- _temp153.curr, _temp153.curr, _temp154.last_plus_one - _temp154.curr, _temp154.curr);}));}}
static void* Cyc_Tcexp_tcConst( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* c, struct Cyc_Absyn_Exp* e){ void* t;{ void* _temp155= c;
char _temp171; void* _temp173; short _temp175; void* _temp177; long long
_temp179; void* _temp181; struct _tagged_string _temp183; int _temp185; void*
_temp187; struct _tagged_string _temp189; _LL157: if(( unsigned int) _temp155 >
1u?(( struct _tunion_struct*) _temp155)->tag == Cyc_Absyn_Char_c_tag: 0){ _LL174:
_temp173=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp155)->f1; goto _LL172;
_LL172: _temp171=( char)(( struct Cyc_Absyn_Char_c_struct*) _temp155)->f2; goto
_LL158;} else{ goto _LL159;} _LL159: if(( unsigned int) _temp155 > 1u?(( struct
_tunion_struct*) _temp155)->tag == Cyc_Absyn_Short_c_tag: 0){ _LL178: _temp177=(
void*)(( struct Cyc_Absyn_Short_c_struct*) _temp155)->f1; goto _LL176; _LL176:
_temp175=( short)(( struct Cyc_Absyn_Short_c_struct*) _temp155)->f2; goto _LL160;}
else{ goto _LL161;} _LL161: if(( unsigned int) _temp155 > 1u?(( struct
_tunion_struct*) _temp155)->tag == Cyc_Absyn_LongLong_c_tag: 0){ _LL182:
_temp181=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp155)->f1; goto
_LL180; _LL180: _temp179=( long long)(( struct Cyc_Absyn_LongLong_c_struct*)
_temp155)->f2; goto _LL162;} else{ goto _LL163;} _LL163: if(( unsigned int)
_temp155 > 1u?(( struct _tunion_struct*) _temp155)->tag == Cyc_Absyn_Float_c_tag:
0){ _LL184: _temp183=( struct _tagged_string)(( struct Cyc_Absyn_Float_c_struct*)
_temp155)->f1; goto _LL164;} else{ goto _LL165;} _LL165: if(( unsigned int)
_temp155 > 1u?(( struct _tunion_struct*) _temp155)->tag == Cyc_Absyn_Int_c_tag:
0){ _LL188: _temp187=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp155)->f1;
goto _LL186; _LL186: _temp185=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp155)->f2;
goto _LL166;} else{ goto _LL167;} _LL167: if(( unsigned int) _temp155 > 1u?((
struct _tunion_struct*) _temp155)->tag == Cyc_Absyn_String_c_tag: 0){ _LL190:
_temp189=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*) _temp155)->f1;
goto _LL168;} else{ goto _LL169;} _LL169: if(( int) _temp155 == Cyc_Absyn_Null_c){
goto _LL170;} else{ goto _LL156;} _LL158: t= _temp173 ==( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_uchar_t: Cyc_Absyn_schar_t; goto _LL156; _LL160: t= _temp177 ==( void*)
Cyc_Absyn_Unsigned? Cyc_Absyn_ushort_t: Cyc_Absyn_sshort_t; goto _LL156; _LL162:
t= _temp181 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ulong_t: Cyc_Absyn_slong_t;
goto _LL156; _LL164: t= Cyc_Absyn_float_t; goto _LL156; _LL166: if( topt == 0){
t= _temp187 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t: Cyc_Absyn_sint_t;}
else{ void* _temp192=({ void** _temp191= topt; if( _temp191 == 0){ _throw(
Null_Exception);}* _temp191;}); void* _temp202; void* _temp204; void* _temp206;
void* _temp208; void* _temp210; void* _temp212; _LL194: if(( unsigned int)
_temp192 > 4u?(( struct _tunion_struct*) _temp192)->tag == Cyc_Absyn_IntType_tag:
0){ _LL205: _temp204=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp192)->f1;
goto _LL203; _LL203: _temp202=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp192)->f2; if(( int) _temp202 == Cyc_Absyn_B1){ goto _LL195;} else{ goto
_LL196;}} else{ goto _LL196;} _LL196: if(( unsigned int) _temp192 > 4u?(( struct
_tunion_struct*) _temp192)->tag == Cyc_Absyn_IntType_tag: 0){ _LL209: _temp208=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp192)->f1; goto _LL207; _LL207:
_temp206=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp192)->f2; if(( int)
_temp206 == Cyc_Absyn_B2){ goto _LL197;} else{ goto _LL198;}} else{ goto _LL198;}
_LL198: if(( unsigned int) _temp192 > 4u?(( struct _tunion_struct*) _temp192)->tag
== Cyc_Absyn_IntType_tag: 0){ _LL213: _temp212=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp192)->f1; goto _LL211; _LL211: _temp210=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp192)->f2; goto _LL199;} else{ goto _LL200;} _LL200: goto _LL201; _LL195: t=
_temp204 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uchar_t: Cyc_Absyn_schar_t;(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp214=(
struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp214[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp215; _temp215.tag= Cyc_Absyn_Const_e_tag;
_temp215.f1=( void*)(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp216=(
struct Cyc_Absyn_Char_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct));
_temp216[ 0]=({ struct Cyc_Absyn_Char_c_struct _temp217; _temp217.tag= Cyc_Absyn_Char_c_tag;
_temp217.f1=( void*) _temp204; _temp217.f2=( char) _temp185; _temp217;});
_temp216;})); _temp215;}); _temp214;}))); goto _LL193; _LL197: t= _temp208 ==(
void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ushort_t: Cyc_Absyn_sshort_t;( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp218=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp218[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp219; _temp219.tag= Cyc_Absyn_Const_e_tag; _temp219.f1=( void*)(( void*)({
struct Cyc_Absyn_Short_c_struct* _temp220=( struct Cyc_Absyn_Short_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Short_c_struct)); _temp220[ 0]=({ struct Cyc_Absyn_Short_c_struct
_temp221; _temp221.tag= Cyc_Absyn_Short_c_tag; _temp221.f1=( void*) _temp208;
_temp221.f2=( short) _temp185; _temp221;}); _temp220;})); _temp219;}); _temp218;})));
goto _LL193; _LL199: t= _temp212 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t:
Cyc_Absyn_sint_t; goto _LL193; _LL201: t= _temp187 ==( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_uint_t: Cyc_Absyn_sint_t; goto _LL193; _LL193:;} goto _LL156; _LL168: {
int len=( int)({ struct _tagged_string _temp270= _temp189;( unsigned int)(
_temp270.last_plus_one - _temp270.curr);}); struct Cyc_Absyn_Const_e_struct* r=({
struct Cyc_Absyn_Const_e_struct* _temp266=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp266[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp267; _temp267.tag= Cyc_Absyn_Const_e_tag; _temp267.f1=( void*)(( void*)({
struct Cyc_Absyn_Int_c_struct* _temp268=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp268[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp269; _temp269.tag= Cyc_Absyn_Int_c_tag; _temp269.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp269.f2= len; _temp269;}); _temp268;})); _temp267;}); _temp266;}); struct
Cyc_Absyn_Exp* elen= Cyc_Absyn_new_exp(( void*) r, loc); elen->topt=({ struct
Cyc_Core_Opt* _temp222=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp222->v=( void*) Cyc_Absyn_uint_t; _temp222;}); t= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t,(
void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp223=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp223[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp224; _temp224.tag= Cyc_Absyn_Upper_b_tag;
_temp224.f1= elen; _temp224;}); _temp223;})); if( topt != 0){ void* _temp226=
Cyc_Tcutil_compress(({ void** _temp225= topt; if( _temp225 == 0){ _throw(
Null_Exception);}* _temp225;})); struct Cyc_Absyn_Exp* _temp234; struct Cyc_Absyn_Tqual*
_temp236; void* _temp238; struct Cyc_Absyn_PtrInfo _temp240; struct Cyc_Absyn_Conref*
_temp242; struct Cyc_Absyn_Tqual* _temp244; struct Cyc_Absyn_Conref* _temp246;
void* _temp248; void* _temp250; _LL228: if(( unsigned int) _temp226 > 4u?((
struct _tunion_struct*) _temp226)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL239:
_temp238=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp226)->f1; goto
_LL237; _LL237: _temp236=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp226)->f2; goto _LL235; _LL235: _temp234=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp226)->f3; goto _LL229;} else{ goto _LL230;}
_LL230: if(( unsigned int) _temp226 > 4u?(( struct _tunion_struct*) _temp226)->tag
== Cyc_Absyn_PointerType_tag: 0){ _LL241: _temp240=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp226)->f1; _LL251: _temp250=( void*)
_temp240.elt_typ; goto _LL249; _LL249: _temp248=( void*) _temp240.rgn_typ; goto
_LL247; _LL247: _temp246=( struct Cyc_Absyn_Conref*) _temp240.nullable; goto
_LL245; _LL245: _temp244=( struct Cyc_Absyn_Tqual*) _temp240.tq; goto _LL243;
_LL243: _temp242=( struct Cyc_Absyn_Conref*) _temp240.bounds; goto _LL231;}
else{ goto _LL232;} _LL232: goto _LL233; _LL229: return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp252=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp252[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp253; _temp253.tag= Cyc_Absyn_ArrayType_tag;
_temp253.f1=( void*) Cyc_Absyn_uchar_t; _temp253.f2= _temp236; _temp253.f3=(
struct Cyc_Absyn_Exp*) elen; _temp253;}); _temp252;}); _LL231: if( ! Cyc_Tcutil_unify(({
void** _temp254= topt; if( _temp254 == 0){ _throw( Null_Exception);}* _temp254;}),
t)? Cyc_Tcutil_silent_castable( te, loc, t,({ void** _temp255= topt; if(
_temp255 == 0){ _throw( Null_Exception);}* _temp255;})): 0){ e->topt=({ struct
Cyc_Core_Opt* _temp256=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp256->v=( void*) t; _temp256;}); Cyc_Tcutil_unchecked_cast( te, e,({ void**
_temp257= topt; if( _temp257 == 0){ _throw( Null_Exception);}* _temp257;})); t=({
void** _temp258= topt; if( _temp258 == 0){ _throw( Null_Exception);}* _temp258;});}
else{ t= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t, Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind),
Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp259=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp259[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp260; _temp260.tag= Cyc_Absyn_Upper_b_tag;
_temp260.f1= elen; _temp260;}); _temp259;})); if( ! Cyc_Tcutil_unify(({ void**
_temp261= topt; if( _temp261 == 0){ _throw( Null_Exception);}* _temp261;}), t)?
Cyc_Tcutil_silent_castable( te, loc, t,({ void** _temp262= topt; if( _temp262 ==
0){ _throw( Null_Exception);}* _temp262;})): 0){ e->topt=({ struct Cyc_Core_Opt*
_temp263=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp263->v=( void*) t; _temp263;}); Cyc_Tcutil_unchecked_cast( te, e,({ void**
_temp264= topt; if( _temp264 == 0){ _throw( Null_Exception);}* _temp264;})); t=({
void** _temp265= topt; if( _temp265 == 0){ _throw( Null_Exception);}* _temp265;});}}
goto _LL227; _LL233: goto _LL227; _LL227:;} return t;} _LL170: t=( void*)({
struct Cyc_Absyn_PointerType_struct* _temp271=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp271[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp272; _temp272.tag= Cyc_Absyn_PointerType_tag;
_temp272.f1=({ struct Cyc_Absyn_PtrInfo _temp273; _temp273.elt_typ=( void*) Cyc_Absyn_new_evar((
void*) Cyc_Absyn_MemKind); _temp273.rgn_typ=( void*) Cyc_Absyn_new_evar(( void*)
Cyc_Absyn_RgnKind); _temp273.nullable=(( struct Cyc_Absyn_Conref*(*)( int x))
Cyc_Absyn_new_conref)( 1); _temp273.tq= Cyc_Absyn_empty_tqual(); _temp273.bounds=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp273;}); _temp272;});
_temp271;}); goto _LL156; _LL156:;} return t;} static void* Cyc_Tcexp_tcVar(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct _tuple1* q,
void* b){ void* _temp274= b; struct Cyc_Absyn_Vardecl* _temp288; struct Cyc_Absyn_Fndecl*
_temp290; struct Cyc_Absyn_Vardecl* _temp292; struct Cyc_Absyn_Vardecl* _temp294;
struct Cyc_Absyn_Vardecl* _temp296; _LL276: if(( int) _temp274 == Cyc_Absyn_Unresolved_b){
goto _LL277;} else{ goto _LL278;} _LL278: if(( unsigned int) _temp274 > 1u?((
struct _tunion_struct*) _temp274)->tag == Cyc_Absyn_Global_b_tag: 0){ _LL289:
_temp288=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Global_b_struct*)
_temp274)->f1; goto _LL279;} else{ goto _LL280;} _LL280: if(( unsigned int)
_temp274 > 1u?(( struct _tunion_struct*) _temp274)->tag == Cyc_Absyn_Funname_b_tag:
0){ _LL291: _temp290=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Funname_b_struct*)
_temp274)->f1; goto _LL281;} else{ goto _LL282;} _LL282: if(( unsigned int)
_temp274 > 1u?(( struct _tunion_struct*) _temp274)->tag == Cyc_Absyn_Pat_b_tag:
0){ _LL293: _temp292=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Pat_b_struct*)
_temp274)->f1; goto _LL283;} else{ goto _LL284;} _LL284: if(( unsigned int)
_temp274 > 1u?(( struct _tunion_struct*) _temp274)->tag == Cyc_Absyn_Local_b_tag:
0){ _LL295: _temp294=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Local_b_struct*)
_temp274)->f1; goto _LL285;} else{ goto _LL286;} _LL286: if(( unsigned int)
_temp274 > 1u?(( struct _tunion_struct*) _temp274)->tag == Cyc_Absyn_Param_b_tag:
0){ _LL297: _temp296=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Param_b_struct*)
_temp274)->f1; goto _LL287;} else{ goto _LL275;} _LL277: return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp298= Cyc_Absynpp_qvar2string( q); xprintf("unbound identifier %.*s",
_temp298.last_plus_one - _temp298.curr, _temp298.curr);})); _LL279:({ struct
_tuple1* _temp299= q; unsigned int _temp300= 0; if( _temp300 >= 1u){ _throw(
Null_Exception);} _temp299[ _temp300]=({ struct _tuple1* _temp301= _temp288->name;
unsigned int _temp302= 0; if( _temp302 >= 1u){ _throw( Null_Exception);}
_temp301[ _temp302];});}); return( void*) _temp288->type; _LL281:({ struct
_tuple1* _temp303= q; unsigned int _temp304= 0; if( _temp304 >= 1u){ _throw(
Null_Exception);} _temp303[ _temp304]=({ struct _tuple1* _temp305= _temp290->name;
unsigned int _temp306= 0; if( _temp306 >= 1u){ _throw( Null_Exception);}
_temp305[ _temp306];});}); return Cyc_Tcutil_fndecl2typ( _temp290); _LL283:
_temp294= _temp292; goto _LL285; _LL285: _temp296= _temp294; goto _LL287; _LL287:(*
q).f1=( void*) Cyc_Absyn_Loc_n; return( void*) _temp296->type; _LL275:;} static
void Cyc_Tcexp_check_format_args( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
fmt, struct Cyc_List_List* args, struct Cyc_List_List*(* type_getter)( struct
_tagged_string, struct Cyc_Position_Segment*)){ struct Cyc_List_List* desc_types;{
void* _temp307=( void*) fmt->r; void* _temp313; struct _tagged_string _temp315;
_LL309: if((( struct _tunion_struct*) _temp307)->tag == Cyc_Absyn_Const_e_tag){
_LL314: _temp313=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp307)->f1; if((
unsigned int) _temp313 > 1u?(( struct _tunion_struct*) _temp313)->tag == Cyc_Absyn_String_c_tag:
0){ _LL316: _temp315=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp313)->f1; goto _LL310;} else{ goto _LL311;}} else{ goto _LL311;} _LL311:
goto _LL312; _LL310: desc_types= type_getter( _temp315, fmt->loc); goto _LL308;
_LL312: Cyc_Tcutil_terr( fmt->loc,( struct _tagged_string)({ char* _temp317=(
char*)"expecting a literal format string"; struct _tagged_string _temp318;
_temp318.curr= _temp317; _temp318.base= _temp317; _temp318.last_plus_one=
_temp317 + 34; _temp318;})); return; _LL308:;} for( 0; desc_types != 0? args !=
0: 0; desc_types=({ struct Cyc_List_List* _temp319= desc_types; if( _temp319 ==
0){ _throw( Null_Exception);} _temp319->tl;}), args=({ struct Cyc_List_List*
_temp320= args; if( _temp320 == 0){ _throw( Null_Exception);} _temp320->tl;})){
void* t=( void*)({ struct Cyc_List_List* _temp325= desc_types; if( _temp325 == 0){
_throw( Null_Exception);} _temp325->hd;}); struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp324= args; if( _temp324 == 0){ _throw( Null_Exception);}
_temp324->hd;}); if( ! Cyc_Tcutil_coerce_arg( te, e, t)){ Cyc_Tcutil_terr( e->loc,({
struct _tagged_string _temp322= Cyc_Absynpp_typ2string( t); struct
_tagged_string _temp323= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp321= e->topt; if( _temp321 == 0){ _throw( Null_Exception);} _temp321->v;}));
xprintf("descriptor has type %.*s but argument has type %.*s", _temp322.last_plus_one
- _temp322.curr, _temp322.curr, _temp323.last_plus_one - _temp323.curr, _temp323.curr);}));}}
if( desc_types != 0){ Cyc_Tcutil_terr( fmt->loc,( struct _tagged_string)({ char*
_temp326=( char*)"too few arguments"; struct _tagged_string _temp327; _temp327.curr=
_temp326; _temp327.base= _temp326; _temp327.last_plus_one= _temp326 + 18;
_temp327;}));} if( args != 0){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp328= args; if( _temp328 == 0){ _throw( Null_Exception);}
_temp328->hd;}))->loc,( struct _tagged_string)({ char* _temp329=( char*)"too many arguments";
struct _tagged_string _temp330; _temp330.curr= _temp329; _temp330.base= _temp329;
_temp330.last_plus_one= _temp329 + 19; _temp330;}));}} static void* Cyc_Tcexp_tcUnPrimop(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, void*
p, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp379= e->topt; if( _temp379 == 0){ _throw( Null_Exception);} _temp379->v;}));
void* _temp331= p; _LL333: if(( int) _temp331 == Cyc_Absyn_Plus){ goto _LL334;}
else{ goto _LL335;} _LL335: if(( int) _temp331 == Cyc_Absyn_Minus){ goto _LL336;}
else{ goto _LL337;} _LL337: if(( int) _temp331 == Cyc_Absyn_Not){ goto _LL338;}
else{ goto _LL339;} _LL339: if(( int) _temp331 == Cyc_Absyn_Bitnot){ goto _LL340;}
else{ goto _LL341;} _LL341: if(( int) _temp331 == Cyc_Absyn_Size){ goto _LL342;}
else{ goto _LL343;} _LL343: goto _LL344; _LL334: goto _LL336; _LL336: if( ! Cyc_Tcutil_is_numeric(
e)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp345= Cyc_Absynpp_typ2string(
t); xprintf("expecting arithmetic type but found %.*s", _temp345.last_plus_one -
_temp345.curr, _temp345.curr);}));} return( void*)({ struct Cyc_Core_Opt*
_temp346= e->topt; if( _temp346 == 0){ _throw( Null_Exception);} _temp346->v;});
_LL338: Cyc_Tcutil_check_contains_assign( e); if( ! Cyc_Tcutil_coerce_to_bool(
te, e)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp347= Cyc_Absynpp_typ2string(
t); xprintf("expecting integral or * type but found %.*s", _temp347.last_plus_one
- _temp347.curr, _temp347.curr);}));} return Cyc_Absyn_sint_t; _LL340: if( ! Cyc_Tcutil_is_integral(
e)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp348= Cyc_Absynpp_typ2string(
t); xprintf("expecting integral type but found %.*s", _temp348.last_plus_one -
_temp348.curr, _temp348.curr);}));} return( void*)({ struct Cyc_Core_Opt*
_temp349= e->topt; if( _temp349 == 0){ _throw( Null_Exception);} _temp349->v;});
_LL342:{ void* _temp350= t; struct Cyc_Absyn_Exp* _temp358; struct Cyc_Absyn_Tqual*
_temp360; void* _temp362; struct Cyc_Absyn_PtrInfo _temp364; struct Cyc_Absyn_Conref*
_temp366; struct Cyc_Absyn_Tqual* _temp368; struct Cyc_Absyn_Conref* _temp370;
void* _temp372; void* _temp374; _LL352: if(( unsigned int) _temp350 > 4u?((
struct _tunion_struct*) _temp350)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL363:
_temp362=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp350)->f1; goto
_LL361; _LL361: _temp360=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp350)->f2; goto _LL359; _LL359: _temp358=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp350)->f3; goto _LL353;} else{ goto _LL354;}
_LL354: if(( unsigned int) _temp350 > 4u?(( struct _tunion_struct*) _temp350)->tag
== Cyc_Absyn_PointerType_tag: 0){ _LL365: _temp364=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp350)->f1; _LL375: _temp374=( void*)
_temp364.elt_typ; goto _LL373; _LL373: _temp372=( void*) _temp364.rgn_typ; goto
_LL371; _LL371: _temp370=( struct Cyc_Absyn_Conref*) _temp364.nullable; goto
_LL369; _LL369: _temp368=( struct Cyc_Absyn_Tqual*) _temp364.tq; goto _LL367;
_LL367: _temp366=( struct Cyc_Absyn_Conref*) _temp364.bounds; goto _LL355;}
else{ goto _LL356;} _LL356: goto _LL357; _LL353: goto _LL351; _LL355: goto
_LL351; _LL357: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp376= Cyc_Absynpp_typ2string(
t); xprintf("size requires pointer or array type, not %.*s", _temp376.last_plus_one
- _temp376.curr, _temp376.curr);})); goto _LL351; _LL351:;} return Cyc_Absyn_uint_t;
_LL344: return(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ char* _temp377=( char*)"Non-unary primop"; struct
_tagged_string _temp378; _temp378.curr= _temp377; _temp378.base= _temp377;
_temp378.last_plus_one= _temp377 + 17; _temp378;})); _LL332:;} static void* Cyc_Tcexp_tcArithBinop(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2,
int(* checker)( struct Cyc_Absyn_Exp*)){ if( ! checker( e1)){ Cyc_Tcutil_terr(
e1->loc,({ struct _tagged_string _temp381= Cyc_Absynpp_typ2string(( void*)({
struct Cyc_Core_Opt* _temp380= e1->topt; if( _temp380 == 0){ _throw(
Null_Exception);} _temp380->v;})); xprintf("type %.*s cannot be used here",
_temp381.last_plus_one - _temp381.curr, _temp381.curr);})); return Cyc_Absyn_wildtyp();}
if( ! checker( e2)){ Cyc_Tcutil_terr( e2->loc,({ struct _tagged_string _temp383=
Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt* _temp382= e2->topt; if(
_temp382 == 0){ _throw( Null_Exception);} _temp382->v;})); xprintf("type %.*s cannot be used here",
_temp383.last_plus_one - _temp383.curr, _temp383.curr);})); return Cyc_Absyn_wildtyp();}{
void* t1= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp385= e1->topt;
if( _temp385 == 0){ _throw( Null_Exception);} _temp385->v;})); void* t2= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp384= e2->topt; if( _temp384 == 0){ _throw(
Null_Exception);} _temp384->v;})); return Cyc_Tcutil_max_arithmetic_type( t1, t2);}}
static void* Cyc_Tcexp_tcPlus( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ void* t1=( void*)({ struct Cyc_Core_Opt* _temp388=
e1->topt; if( _temp388 == 0){ _throw( Null_Exception);} _temp388->v;}); if( Cyc_Tcutil_is_tagged_pointer_typ(
t1)){ if( ! Cyc_Tcutil_coerce_sint_typ( te, e2)){ Cyc_Tcutil_terr( e2->loc,({
struct _tagged_string _temp387= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp386= e2->topt; if( _temp386 == 0){ _throw( Null_Exception);} _temp386->v;}));
xprintf("expecting int but found %.*s", _temp387.last_plus_one - _temp387.curr,
_temp387.curr);}));} return t1;} else{ return Cyc_Tcexp_tcArithBinop( te, e1, e2,
Cyc_Tcutil_is_numeric);}} static void* Cyc_Tcexp_tcMinus( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ void* t1=( void*)({
struct Cyc_Core_Opt* _temp396= e1->topt; if( _temp396 == 0){ _throw(
Null_Exception);} _temp396->v;}); void* t2=( void*)({ struct Cyc_Core_Opt*
_temp395= e2->topt; if( _temp395 == 0){ _throw( Null_Exception);} _temp395->v;});
if( Cyc_Tcutil_is_tagged_pointer_typ( t1)){ if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){ if( ! Cyc_Tcutil_unify( t1, t2)){ Cyc_Tcutil_terr( e1->loc,({ struct
_tagged_string _temp391= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp389= e1->topt; if( _temp389 == 0){ _throw( Null_Exception);} _temp389->v;}));
struct _tagged_string _temp392= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp390= e2->topt; if( _temp390 == 0){ _throw( Null_Exception);} _temp390->v;}));
xprintf("pointer arithmetic on values of different types (%.*s != %.*s)",
_temp391.last_plus_one - _temp391.curr, _temp391.curr, _temp392.last_plus_one -
_temp392.curr, _temp392.curr);}));} return Cyc_Absyn_sint_t;} else{ if( ! Cyc_Tcutil_coerce_sint_typ(
te, e2)){ Cyc_Tcutil_terr( e2->loc,({ struct _tagged_string _temp393= Cyc_Absynpp_typ2string(
t1); struct _tagged_string _temp394= Cyc_Absynpp_typ2string( t2); xprintf("expecting either %.*s or int but found %.*s",
_temp393.last_plus_one - _temp393.curr, _temp393.curr, _temp394.last_plus_one -
_temp394.curr, _temp394.curr);}));} return t1;}} else{ return Cyc_Tcexp_tcArithBinop(
te, e1, e2, Cyc_Tcutil_is_numeric);}} static void* Cyc_Tcexp_tcAnyBinop( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2){ int e1_is_num= Cyc_Tcutil_is_numeric( e1); int
e2_is_num= Cyc_Tcutil_is_numeric( e2); void* t1= Cyc_Tcutil_compress(( void*)({
struct Cyc_Core_Opt* _temp437= e1->topt; if( _temp437 == 0){ _throw(
Null_Exception);} _temp437->v;})); void* t2= Cyc_Tcutil_compress(( void*)({
struct Cyc_Core_Opt* _temp436= e2->topt; if( _temp436 == 0){ _throw(
Null_Exception);} _temp436->v;})); if( e1_is_num? e2_is_num: 0){ return Cyc_Absyn_sint_t;}
else{ if( Cyc_Tcutil_unify( t1, t2)? Cyc_Tcutil_unify( t1, Cyc_Absyn_new_evar((
void*) Cyc_Absyn_BoxKind)): 0){ return Cyc_Absyn_sint_t;} else{ if( Cyc_Tcutil_silent_castable(
te, loc, t2, t1)){ Cyc_Tcutil_unchecked_cast( te, e2, t1); return Cyc_Absyn_sint_t;}
else{ if( Cyc_Tcutil_silent_castable( te, loc, t1, t2)){ Cyc_Tcutil_unchecked_cast(
te, e1, t2); return Cyc_Absyn_sint_t;} else{ void* t1=( void*)({ struct Cyc_Core_Opt*
_temp435= e1->topt; if( _temp435 == 0){ _throw( Null_Exception);} _temp435->v;});
void* t2=( void*)({ struct Cyc_Core_Opt* _temp434= e2->topt; if( _temp434 == 0){
_throw( Null_Exception);} _temp434->v;});{ struct _tuple0 _temp398=({ struct
_tuple0 _temp397; _temp397.f1= Cyc_Tcutil_compress( t1); _temp397.f2= Cyc_Tcutil_compress(
t2); _temp397;}); void* _temp404; struct Cyc_Absyn_PtrInfo _temp406; struct Cyc_Absyn_Conref*
_temp408; struct Cyc_Absyn_Tqual* _temp410; struct Cyc_Absyn_Conref* _temp412;
void* _temp414; void* _temp416; void* _temp418; struct Cyc_Absyn_PtrInfo
_temp420; struct Cyc_Absyn_Conref* _temp422; struct Cyc_Absyn_Tqual* _temp424;
struct Cyc_Absyn_Conref* _temp426; void* _temp428; void* _temp430; _LL400:
_LL419: _temp418= _temp398.f1; if(( unsigned int) _temp418 > 4u?(( struct
_tunion_struct*) _temp418)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL421:
_temp420=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp418)->f1; _LL431: _temp430=( void*) _temp420.elt_typ; goto _LL429; _LL429:
_temp428=( void*) _temp420.rgn_typ; goto _LL427; _LL427: _temp426=( struct Cyc_Absyn_Conref*)
_temp420.nullable; goto _LL425; _LL425: _temp424=( struct Cyc_Absyn_Tqual*)
_temp420.tq; goto _LL423; _LL423: _temp422=( struct Cyc_Absyn_Conref*) _temp420.bounds;
goto _LL405;} else{ goto _LL402;} _LL405: _temp404= _temp398.f2; if((
unsigned int) _temp404 > 4u?(( struct _tunion_struct*) _temp404)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL407: _temp406=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp404)->f1; _LL417: _temp416=( void*) _temp406.elt_typ; goto _LL415; _LL415:
_temp414=( void*) _temp406.rgn_typ; goto _LL413; _LL413: _temp412=( struct Cyc_Absyn_Conref*)
_temp406.nullable; goto _LL411; _LL411: _temp410=( struct Cyc_Absyn_Tqual*)
_temp406.tq; goto _LL409; _LL409: _temp408=( struct Cyc_Absyn_Conref*) _temp406.bounds;
goto _LL401;} else{ goto _LL402;} _LL402: goto _LL403; _LL401: if( Cyc_Tcutil_unify(
_temp430, _temp416)){ return Cyc_Absyn_sint_t;} goto _LL399; _LL403: goto _LL399;
_LL399:;} Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp432= Cyc_Absynpp_typ2string(
t1); struct _tagged_string _temp433= Cyc_Absynpp_typ2string( t2); xprintf("comparison not allowed between %.*s and %.*s",
_temp432.last_plus_one - _temp432.curr, _temp432.curr, _temp433.last_plus_one -
_temp433.curr, _temp433.curr);})); return Cyc_Absyn_wildtyp();}}}}} static void*
Cyc_Tcexp_tcBinPrimop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* p, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){
void* _temp438= p; _LL440: if(( int) _temp438 == Cyc_Absyn_Plus){ goto _LL441;}
else{ goto _LL442;} _LL442: if(( int) _temp438 == Cyc_Absyn_Minus){ goto _LL443;}
else{ goto _LL444;} _LL444: if(( int) _temp438 == Cyc_Absyn_Times){ goto _LL445;}
else{ goto _LL446;} _LL446: if(( int) _temp438 == Cyc_Absyn_Div){ goto _LL447;}
else{ goto _LL448;} _LL448: if(( int) _temp438 == Cyc_Absyn_Mod){ goto _LL449;}
else{ goto _LL450;} _LL450: if(( int) _temp438 == Cyc_Absyn_Bitand){ goto _LL451;}
else{ goto _LL452;} _LL452: if(( int) _temp438 == Cyc_Absyn_Bitor){ goto _LL453;}
else{ goto _LL454;} _LL454: if(( int) _temp438 == Cyc_Absyn_Bitxor){ goto _LL455;}
else{ goto _LL456;} _LL456: if(( int) _temp438 == Cyc_Absyn_Bitlshift){ goto
_LL457;} else{ goto _LL458;} _LL458: if(( int) _temp438 == Cyc_Absyn_Bitlrshift){
goto _LL459;} else{ goto _LL460;} _LL460: if(( int) _temp438 == Cyc_Absyn_Bitarshift){
goto _LL461;} else{ goto _LL462;} _LL462: if(( int) _temp438 == Cyc_Absyn_Eq){
goto _LL463;} else{ goto _LL464;} _LL464: if(( int) _temp438 == Cyc_Absyn_Neq){
goto _LL465;} else{ goto _LL466;} _LL466: if(( int) _temp438 == Cyc_Absyn_Gt){
goto _LL467;} else{ goto _LL468;} _LL468: if(( int) _temp438 == Cyc_Absyn_Lt){
goto _LL469;} else{ goto _LL470;} _LL470: if(( int) _temp438 == Cyc_Absyn_Gte){
goto _LL471;} else{ goto _LL472;} _LL472: if(( int) _temp438 == Cyc_Absyn_Lte){
goto _LL473;} else{ goto _LL474;} _LL474: goto _LL475; _LL441: return Cyc_Tcexp_tcPlus(
te, e1, e2); _LL443: return Cyc_Tcexp_tcMinus( te, e1, e2); _LL445: goto _LL447;
_LL447: return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_is_numeric);
_LL449: goto _LL451; _LL451: goto _LL453; _LL453: goto _LL455; _LL455: goto
_LL457; _LL457: goto _LL459; _LL459: goto _LL461; _LL461: return Cyc_Tcexp_tcArithBinop(
te, e1, e2, Cyc_Tcutil_is_integral); _LL463: goto _LL465; _LL465: goto _LL467;
_LL467: goto _LL469; _LL469: goto _LL471; _LL471: goto _LL473; _LL473: return
Cyc_Tcexp_tcAnyBinop( te, loc, e1, e2); _LL475:( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Core_Failure_struct* _temp476=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp476[ 0]=({ struct Cyc_Core_Failure_struct
_temp477; _temp477.tag= Cyc_Core_Failure_tag; _temp477.f1=( struct
_tagged_string)({ char* _temp478=( char*)"bad binary primop"; struct
_tagged_string _temp479; _temp479.curr= _temp478; _temp479.base= _temp478;
_temp479.last_plus_one= _temp478 + 18; _temp479;}); _temp477;}); _temp476;}));
_LL439:;} static void* Cyc_Tcexp_tcPrimop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* p, struct Cyc_List_List* es){ if( p ==( void*) Cyc_Absyn_Minus?((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es) == 1: 0){ return Cyc_Tcexp_tcExp(
te, topt,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp480= es; if(
_temp480 == 0){ _throw( Null_Exception);} _temp480->hd;}));} Cyc_Tcexp_tcExpList(
te, es);{ void* t;{ void* _temp481= p; _LL483: if(( int) _temp481 == Cyc_Absyn_Printf){
goto _LL484;} else{ goto _LL485;} _LL485: if(( int) _temp481 == Cyc_Absyn_Fprintf){
goto _LL486;} else{ goto _LL487;} _LL487: if(( int) _temp481 == Cyc_Absyn_Xprintf){
goto _LL488;} else{ goto _LL489;} _LL489: if(( int) _temp481 == Cyc_Absyn_Scanf){
goto _LL490;} else{ goto _LL491;} _LL491: if(( int) _temp481 == Cyc_Absyn_Fscanf){
goto _LL492;} else{ goto _LL493;} _LL493: if(( int) _temp481 == Cyc_Absyn_Sscanf){
goto _LL494;} else{ goto _LL495;} _LL495: goto _LL496; _LL484: if( es == 0){
return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char* _temp497=(
char*)"missing format string in printf"; struct _tagged_string _temp498;
_temp498.curr= _temp497; _temp498.base= _temp497; _temp498.last_plus_one=
_temp497 + 32; _temp498;}));} Cyc_Tcexp_check_format_args( te,( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp499= es; if( _temp499 == 0){ _throw( Null_Exception);}
_temp499->hd;}),({ struct Cyc_List_List* _temp500= es; if( _temp500 == 0){
_throw( Null_Exception);} _temp500->tl;}), Cyc_Formatstr_get_format_typs); t=(
void*) Cyc_Absyn_VoidType; goto _LL482; _LL486: if( es == 0? 1:({ struct Cyc_List_List*
_temp501= es; if( _temp501 == 0){ _throw( Null_Exception);} _temp501->tl;}) == 0){
return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char* _temp502=(
char*)"too few arguments to fprintf"; struct _tagged_string _temp503; _temp503.curr=
_temp502; _temp503.base= _temp502; _temp503.last_plus_one= _temp502 + 29;
_temp503;}));} if( ! Cyc_Tcutil_unify(( void*)({ struct Cyc_Core_Opt* _temp505=((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp504= es; if( _temp504 == 0){
_throw( Null_Exception);} _temp504->hd;}))->topt; if( _temp505 == 0){ _throw(
Null_Exception);} _temp505->v;}), Cyc_Absyn_file_typ())){ Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp506= es; if( _temp506 == 0){
_throw( Null_Exception);} _temp506->hd;}))->loc,({ struct _tagged_string
_temp509= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt* _temp508=((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp507= es; if( _temp507 == 0){
_throw( Null_Exception);} _temp507->hd;}))->topt; if( _temp508 == 0){ _throw(
Null_Exception);} _temp508->v;})); xprintf("expecting FILE @ but found %.*s",
_temp509.last_plus_one - _temp509.curr, _temp509.curr);}));} Cyc_Tcexp_check_format_args(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp511=({ struct Cyc_List_List*
_temp510= es; if( _temp510 == 0){ _throw( Null_Exception);} _temp510->tl;}); if(
_temp511 == 0){ _throw( Null_Exception);} _temp511->hd;}),({ struct Cyc_List_List*
_temp513=({ struct Cyc_List_List* _temp512= es; if( _temp512 == 0){ _throw(
Null_Exception);} _temp512->tl;}); if( _temp513 == 0){ _throw( Null_Exception);}
_temp513->tl;}), Cyc_Formatstr_get_format_typs); t=( void*) Cyc_Absyn_VoidType;
goto _LL482; _LL488: if( es == 0){ return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_string)({ char* _temp514=( char*)"missing format string in xprintf";
struct _tagged_string _temp515; _temp515.curr= _temp514; _temp515.base= _temp514;
_temp515.last_plus_one= _temp514 + 33; _temp515;}));} Cyc_Tcexp_check_format_args(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp516= es; if( _temp516
== 0){ _throw( Null_Exception);} _temp516->hd;}),({ struct Cyc_List_List*
_temp517= es; if( _temp517 == 0){ _throw( Null_Exception);} _temp517->tl;}), Cyc_Formatstr_get_format_typs);
t= Cyc_Absyn_string_typ( Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind)); goto
_LL482; _LL490: if( es == 0){ return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_string)({ char* _temp518=( char*)"missing format string in scanf";
struct _tagged_string _temp519; _temp519.curr= _temp518; _temp519.base= _temp518;
_temp519.last_plus_one= _temp518 + 31; _temp519;}));} Cyc_Tcexp_check_format_args(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp520= es; if( _temp520
== 0){ _throw( Null_Exception);} _temp520->hd;}),({ struct Cyc_List_List*
_temp521= es; if( _temp521 == 0){ _throw( Null_Exception);} _temp521->tl;}), Cyc_Formatstr_get_scanf_typs);
t= Cyc_Absyn_sint_t; goto _LL482; _LL492: if( es == 0? 1:({ struct Cyc_List_List*
_temp522= es; if( _temp522 == 0){ _throw( Null_Exception);} _temp522->tl;}) == 0){
return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char* _temp523=(
char*)"too few arguments to fscanf"; struct _tagged_string _temp524; _temp524.curr=
_temp523; _temp524.base= _temp523; _temp524.last_plus_one= _temp523 + 28;
_temp524;}));} if( ! Cyc_Tcutil_unify(( void*)({ struct Cyc_Core_Opt* _temp526=((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp525= es; if( _temp525 == 0){
_throw( Null_Exception);} _temp525->hd;}))->topt; if( _temp526 == 0){ _throw(
Null_Exception);} _temp526->v;}), Cyc_Absyn_file_typ())){ Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp527= es; if( _temp527 == 0){
_throw( Null_Exception);} _temp527->hd;}))->loc,({ struct _tagged_string
_temp530= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt* _temp529=((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp528= es; if( _temp528 == 0){
_throw( Null_Exception);} _temp528->hd;}))->topt; if( _temp529 == 0){ _throw(
Null_Exception);} _temp529->v;})); xprintf("expecting FILE @ but found %.*s",
_temp530.last_plus_one - _temp530.curr, _temp530.curr);}));} Cyc_Tcexp_check_format_args(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp532=({ struct Cyc_List_List*
_temp531= es; if( _temp531 == 0){ _throw( Null_Exception);} _temp531->tl;}); if(
_temp532 == 0){ _throw( Null_Exception);} _temp532->hd;}),({ struct Cyc_List_List*
_temp534=({ struct Cyc_List_List* _temp533= es; if( _temp533 == 0){ _throw(
Null_Exception);} _temp533->tl;}); if( _temp534 == 0){ _throw( Null_Exception);}
_temp534->tl;}), Cyc_Formatstr_get_scanf_typs); t= Cyc_Absyn_sint_t; goto _LL482;
_LL494: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp535=( char*)"sscanf is not supported";
struct _tagged_string _temp536; _temp536.curr= _temp535; _temp536.base= _temp535;
_temp536.last_plus_one= _temp535 + 24; _temp536;})); t= Cyc_Absyn_sint_t; goto
_LL482; _LL496: switch((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es)){
case 0: _LL537: return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({
char* _temp539=( char*)"primitive operator has 0 arguments"; struct
_tagged_string _temp540; _temp540.curr= _temp539; _temp540.base= _temp539;
_temp540.last_plus_one= _temp539 + 35; _temp540;})); case 1: _LL538: t= Cyc_Tcexp_tcUnPrimop(
te, loc, topt, p,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp542= es;
if( _temp542 == 0){ _throw( Null_Exception);} _temp542->hd;})); break; case 2:
_LL541: t= Cyc_Tcexp_tcBinPrimop( te, loc, topt, p,( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp544= es; if( _temp544 == 0){ _throw( Null_Exception);}
_temp544->hd;}),( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp546=({
struct Cyc_List_List* _temp545= es; if( _temp545 == 0){ _throw( Null_Exception);}
_temp545->tl;}); if( _temp546 == 0){ _throw( Null_Exception);} _temp546->hd;}));
break; default: _LL543: return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_string)({ char* _temp548=( char*)"primitive operator has > 2 arguments";
struct _tagged_string _temp549; _temp549.curr= _temp548; _temp549.base= _temp548;
_temp549.last_plus_one= _temp548 + 37; _temp549;}));} goto _LL482; _LL482:;}
return t;}} static void Cyc_Tcexp_check_writable( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e){{ void* _temp550=( void*) e->r; void* _temp570; struct
Cyc_Absyn_Vardecl* _temp572; struct _tuple1* _temp574; void* _temp576; struct
Cyc_Absyn_Vardecl* _temp578; struct _tuple1* _temp580; void* _temp582; struct
Cyc_Absyn_Vardecl* _temp584; struct _tuple1* _temp586; void* _temp588; struct
Cyc_Absyn_Vardecl* _temp590; struct _tuple1* _temp592; struct Cyc_Absyn_Exp*
_temp594; struct Cyc_Absyn_Exp* _temp596; struct _tagged_string* _temp598;
struct Cyc_Absyn_Exp* _temp600; struct _tagged_string* _temp602; struct Cyc_Absyn_Exp*
_temp604; struct Cyc_Absyn_Exp* _temp606; _LL552: if((( struct _tunion_struct*)
_temp550)->tag == Cyc_Absyn_Var_e_tag){ _LL575: _temp574=( struct _tuple1*)((
struct Cyc_Absyn_Var_e_struct*) _temp550)->f1; goto _LL571; _LL571: _temp570=(
void*)(( struct Cyc_Absyn_Var_e_struct*) _temp550)->f2; if(( unsigned int)
_temp570 > 1u?(( struct _tunion_struct*) _temp570)->tag == Cyc_Absyn_Param_b_tag:
0){ _LL573: _temp572=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Param_b_struct*)
_temp570)->f1; goto _LL553;} else{ goto _LL554;}} else{ goto _LL554;} _LL554:
if((( struct _tunion_struct*) _temp550)->tag == Cyc_Absyn_Var_e_tag){ _LL581:
_temp580=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp550)->f1;
goto _LL577; _LL577: _temp576=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp550)->f2; if(( unsigned int) _temp576 > 1u?(( struct _tunion_struct*)
_temp576)->tag == Cyc_Absyn_Local_b_tag: 0){ _LL579: _temp578=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Local_b_struct*) _temp576)->f1; goto _LL555;} else{ goto _LL556;}}
else{ goto _LL556;} _LL556: if((( struct _tunion_struct*) _temp550)->tag == Cyc_Absyn_Var_e_tag){
_LL587: _temp586=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp550)->f1;
goto _LL583; _LL583: _temp582=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp550)->f2; if(( unsigned int) _temp582 > 1u?(( struct _tunion_struct*)
_temp582)->tag == Cyc_Absyn_Pat_b_tag: 0){ _LL585: _temp584=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Pat_b_struct*) _temp582)->f1; goto _LL557;} else{ goto _LL558;}}
else{ goto _LL558;} _LL558: if((( struct _tunion_struct*) _temp550)->tag == Cyc_Absyn_Var_e_tag){
_LL593: _temp592=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp550)->f1;
goto _LL589; _LL589: _temp588=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp550)->f2; if(( unsigned int) _temp588 > 1u?(( struct _tunion_struct*)
_temp588)->tag == Cyc_Absyn_Global_b_tag: 0){ _LL591: _temp590=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Global_b_struct*) _temp588)->f1; goto _LL559;} else{ goto
_LL560;}} else{ goto _LL560;} _LL560: if((( struct _tunion_struct*) _temp550)->tag
== Cyc_Absyn_Subscript_e_tag){ _LL597: _temp596=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp550)->f1; goto _LL595; _LL595:
_temp594=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp550)->f2; goto _LL561;} else{ goto _LL562;} _LL562: if((( struct
_tunion_struct*) _temp550)->tag == Cyc_Absyn_StructMember_e_tag){ _LL601:
_temp600=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp550)->f1; goto _LL599; _LL599: _temp598=( struct _tagged_string*)(( struct
Cyc_Absyn_StructMember_e_struct*) _temp550)->f2; goto _LL563;} else{ goto _LL564;}
_LL564: if((( struct _tunion_struct*) _temp550)->tag == Cyc_Absyn_StructArrow_e_tag){
_LL605: _temp604=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp550)->f1; goto _LL603; _LL603: _temp602=( struct _tagged_string*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp550)->f2; goto _LL565;} else{ goto _LL566;}
_LL566: if((( struct _tunion_struct*) _temp550)->tag == Cyc_Absyn_Deref_e_tag){
_LL607: _temp606=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp550)->f1; goto _LL567;} else{ goto _LL568;} _LL568: goto _LL569; _LL553:
_temp578= _temp572; goto _LL555; _LL555: _temp584= _temp578; goto _LL557; _LL557:
_temp590= _temp584; goto _LL559; _LL559: if( !( _temp590->tq)->q_const){ return;}
goto _LL551; _LL561:{ void* _temp609= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp608= _temp596->topt; if( _temp608 == 0){ _throw( Null_Exception);} _temp608->v;}));
struct Cyc_Absyn_PtrInfo _temp619; struct Cyc_Absyn_Conref* _temp621; struct Cyc_Absyn_Tqual*
_temp623; struct Cyc_Absyn_Conref* _temp625; void* _temp627; void* _temp629;
struct Cyc_Absyn_Exp* _temp631; struct Cyc_Absyn_Tqual* _temp633; void* _temp635;
struct Cyc_List_List* _temp637; _LL611: if(( unsigned int) _temp609 > 4u?((
struct _tunion_struct*) _temp609)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL620:
_temp619=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp609)->f1; _LL630: _temp629=( void*) _temp619.elt_typ; goto _LL628; _LL628:
_temp627=( void*) _temp619.rgn_typ; goto _LL626; _LL626: _temp625=( struct Cyc_Absyn_Conref*)
_temp619.nullable; goto _LL624; _LL624: _temp623=( struct Cyc_Absyn_Tqual*)
_temp619.tq; goto _LL622; _LL622: _temp621=( struct Cyc_Absyn_Conref*) _temp619.bounds;
goto _LL612;} else{ goto _LL613;} _LL613: if(( unsigned int) _temp609 > 4u?((
struct _tunion_struct*) _temp609)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL636:
_temp635=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp609)->f1; goto
_LL634; _LL634: _temp633=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp609)->f2; goto _LL632; _LL632: _temp631=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp609)->f3; goto _LL614;} else{ goto _LL615;}
_LL615: if(( unsigned int) _temp609 > 4u?(( struct _tunion_struct*) _temp609)->tag
== Cyc_Absyn_TupleType_tag: 0){ _LL638: _temp637=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp609)->f1; goto _LL616;} else{ goto
_LL617;} _LL617: goto _LL618; _LL612: _temp633= _temp623; goto _LL614; _LL614:
if( ! _temp633->q_const){ return;} goto _LL610; _LL616: { unsigned int i= Cyc_Evexp_eval_const_uint_exp(
_temp594);{ struct _handler_cons _temp639; _push_handler(& _temp639);{ struct
_xtunion_struct* _temp640=( struct _xtunion_struct*) setjmp( _temp639.handler);
if( ! _temp640){{ struct _tuple7 _temp643; void* _temp644; struct Cyc_Absyn_Tqual*
_temp646; struct _tuple7* _temp641=(( struct _tuple7*(*)( struct Cyc_List_List*
x, int i)) Cyc_List_nth)( _temp637,( int) i); _temp643=* _temp641; _LL647:
_temp646= _temp643.f1; goto _LL645; _LL645: _temp644= _temp643.f2; goto _LL642;
_LL642: if( ! _temp646->q_const){ _npop_handler( 0u); return;}}; _pop_handler();}
else{ struct _xtunion_struct* _temp649= _temp640; _LL651: if( _temp649->tag ==
Cyc_List_Nth_tag){ goto _LL652;} else{ goto _LL653;} _LL653: goto _LL654; _LL652:
return; _LL654:( void) _throw( _temp649); _LL650:;}}} goto _LL610;} _LL618: goto
_LL610; _LL610:;} goto _LL551; _LL563:{ void* _temp656= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp655= _temp600->topt; if( _temp655 == 0){
_throw( Null_Exception);} _temp655->v;})); struct Cyc_Absyn_Structdecl**
_temp662; struct Cyc_List_List* _temp664; struct _tuple1* _temp666; _LL658: if((
unsigned int) _temp656 > 4u?(( struct _tunion_struct*) _temp656)->tag == Cyc_Absyn_StructType_tag:
0){ _LL667: _temp666=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp656)->f1; goto _LL665; _LL665: _temp664=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp656)->f2; goto _LL663; _LL663: _temp662=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp656)->f3;
goto _LL659;} else{ goto _LL660;} _LL660: goto _LL661; _LL659: { struct Cyc_Absyn_Structfield*
sf= _temp662 == 0? 0: Cyc_Absyn_lookup_struct_field(( struct Cyc_Absyn_Structdecl*)({
struct Cyc_Absyn_Structdecl** _temp669= _temp662; if( _temp669 == 0){ _throw(
Null_Exception);}* _temp669;}), _temp598); if( sf == 0? 1: !({ struct Cyc_Absyn_Structfield*
_temp668= sf; if( _temp668 == 0){ _throw( Null_Exception);} _temp668->tq;})->q_const){
return;} goto _LL657;} _LL661: goto _LL657; _LL657:;} goto _LL551; _LL565:{ void*
_temp671= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp670= _temp604->topt;
if( _temp670 == 0){ _throw( Null_Exception);} _temp670->v;})); struct Cyc_Absyn_PtrInfo
_temp677; struct Cyc_Absyn_Conref* _temp679; struct Cyc_Absyn_Tqual* _temp681;
struct Cyc_Absyn_Conref* _temp683; void* _temp685; void* _temp687; _LL673: if((
unsigned int) _temp671 > 4u?(( struct _tunion_struct*) _temp671)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL678: _temp677=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp671)->f1; _LL688: _temp687=( void*) _temp677.elt_typ; goto _LL686; _LL686:
_temp685=( void*) _temp677.rgn_typ; goto _LL684; _LL684: _temp683=( struct Cyc_Absyn_Conref*)
_temp677.nullable; goto _LL682; _LL682: _temp681=( struct Cyc_Absyn_Tqual*)
_temp677.tq; goto _LL680; _LL680: _temp679=( struct Cyc_Absyn_Conref*) _temp677.bounds;
goto _LL674;} else{ goto _LL675;} _LL675: goto _LL676; _LL674:{ void* _temp689=
Cyc_Tcutil_compress( _temp687); struct Cyc_Absyn_Structdecl** _temp695; struct
Cyc_List_List* _temp697; struct _tuple1* _temp699; _LL691: if(( unsigned int)
_temp689 > 4u?(( struct _tunion_struct*) _temp689)->tag == Cyc_Absyn_StructType_tag:
0){ _LL700: _temp699=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp689)->f1; goto _LL698; _LL698: _temp697=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp689)->f2; goto _LL696; _LL696: _temp695=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp689)->f3;
goto _LL692;} else{ goto _LL693;} _LL693: goto _LL694; _LL692: { struct Cyc_Absyn_Structfield*
sf= _temp695 == 0? 0: Cyc_Absyn_lookup_struct_field(( struct Cyc_Absyn_Structdecl*)({
struct Cyc_Absyn_Structdecl** _temp702= _temp695; if( _temp702 == 0){ _throw(
Null_Exception);}* _temp702;}), _temp602); if( sf == 0? 1: !({ struct Cyc_Absyn_Structfield*
_temp701= sf; if( _temp701 == 0){ _throw( Null_Exception);} _temp701->tq;})->q_const){
return;} goto _LL690;} _LL694: goto _LL690; _LL690:;} goto _LL672; _LL676: goto
_LL672; _LL672:;} goto _LL551; _LL567:{ void* _temp704= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp703= _temp606->topt; if( _temp703 == 0){
_throw( Null_Exception);} _temp703->v;})); struct Cyc_Absyn_PtrInfo _temp712;
struct Cyc_Absyn_Conref* _temp714; struct Cyc_Absyn_Tqual* _temp716; struct Cyc_Absyn_Conref*
_temp718; void* _temp720; void* _temp722; struct Cyc_Absyn_Exp* _temp724; struct
Cyc_Absyn_Tqual* _temp726; void* _temp728; _LL706: if(( unsigned int) _temp704 >
4u?(( struct _tunion_struct*) _temp704)->tag == Cyc_Absyn_PointerType_tag: 0){
_LL713: _temp712=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp704)->f1; _LL723: _temp722=( void*) _temp712.elt_typ; goto _LL721; _LL721:
_temp720=( void*) _temp712.rgn_typ; goto _LL719; _LL719: _temp718=( struct Cyc_Absyn_Conref*)
_temp712.nullable; goto _LL717; _LL717: _temp716=( struct Cyc_Absyn_Tqual*)
_temp712.tq; goto _LL715; _LL715: _temp714=( struct Cyc_Absyn_Conref*) _temp712.bounds;
goto _LL707;} else{ goto _LL708;} _LL708: if(( unsigned int) _temp704 > 4u?((
struct _tunion_struct*) _temp704)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL729:
_temp728=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp704)->f1; goto
_LL727; _LL727: _temp726=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp704)->f2; goto _LL725; _LL725: _temp724=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp704)->f3; goto _LL709;} else{ goto _LL710;}
_LL710: goto _LL711; _LL707: _temp726= _temp716; goto _LL709; _LL709: if( !
_temp726->q_const){ return;} goto _LL705; _LL711: goto _LL705; _LL705:;} goto
_LL551; _LL569: goto _LL551; _LL551:;} Cyc_Tcutil_terr( e->loc,( struct
_tagged_string)({ char* _temp730=( char*)"attempt to write a read-only location";
struct _tagged_string _temp731; _temp731.curr= _temp730; _temp731.base= _temp730;
_temp731.last_plus_one= _temp730 + 38; _temp731;}));} static void* Cyc_Tcexp_tcIncrement(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e, void* i){ Cyc_Tcexp_tcExpNoPromote( te, 0, e); if( ! Cyc_Absyn_is_lvalue(
e)){ return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char*
_temp732=( char*)"increment/decrement of non-lvalue"; struct _tagged_string
_temp733; _temp733.curr= _temp732; _temp733.base= _temp732; _temp733.last_plus_one=
_temp732 + 34; _temp733;}));} Cyc_Tcexp_check_writable( te, e);{ void* t=( void*)({
struct Cyc_Core_Opt* _temp735= e->topt; if( _temp735 == 0){ _throw(
Null_Exception);} _temp735->v;}); if( ! Cyc_Tcutil_is_numeric( e)? ! Cyc_Tcutil_is_tagged_pointer_typ(
t): 0){ Cyc_Tcutil_terr( e->loc,({ struct _tagged_string _temp734= Cyc_Absynpp_typ2string(
t); xprintf("expecting arithmetic or pointer type but found %.*s", _temp734.last_plus_one
- _temp734.curr, _temp734.curr);}));} return t;}} static void* Cyc_Tcexp_tcConditional(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3){ Cyc_Tcexp_tcTest(
te, e1,( struct _tagged_string)({ char* _temp736=( char*)"conditional expression";
struct _tagged_string _temp737; _temp737.curr= _temp736; _temp737.base= _temp736;
_temp737.last_plus_one= _temp736 + 23; _temp737;})); Cyc_Tcexp_tcExp( te, topt,
e2); Cyc_Tcexp_tcExp( te, topt, e3);{ void* t= Cyc_Absyn_new_evar(( void*) Cyc_Absyn_MemKind);
struct Cyc_List_List* l1=({ struct Cyc_List_List* _temp743=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp743->hd=( void*) e3; _temp743->tl=
0; _temp743;}); struct Cyc_List_List* l2=({ struct Cyc_List_List* _temp742=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp742->hd=(
void*) e2; _temp742->tl= l1; _temp742;}); if( ! Cyc_Tcutil_coerce_list( te, t,
l2)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp740= Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Core_Opt* _temp738= e2->topt; if( _temp738 == 0){ _throw(
Null_Exception);} _temp738->v;})); struct _tagged_string _temp741= Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Core_Opt* _temp739= e3->topt; if( _temp739 == 0){ _throw(
Null_Exception);} _temp739->v;})); xprintf("conditional clause types do not match: %.*s != %.*s",
_temp740.last_plus_one - _temp740.curr, _temp740.curr, _temp741.last_plus_one -
_temp741.curr, _temp741.curr);}));} return t;}} static void* Cyc_Tcexp_tcAssignOp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e1, struct Cyc_Core_Opt* po, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExpNoPromote(
te, 0, e1); Cyc_Tcexp_tcExp( te,( void**)(( void*)({ struct Cyc_Core_Opt*
_temp744= e1->topt; if( _temp744 == 0){ _throw( Null_Exception);}& _temp744->v;})),
e2);{ void* t1=( void*)({ struct Cyc_Core_Opt* _temp767= e1->topt; if( _temp767
== 0){ _throw( Null_Exception);} _temp767->v;}); void* t2=( void*)({ struct Cyc_Core_Opt*
_temp766= e2->topt; if( _temp766 == 0){ _throw( Null_Exception);} _temp766->v;});{
void* _temp745= Cyc_Tcutil_compress( t1); struct Cyc_Absyn_Exp* _temp751; struct
Cyc_Absyn_Tqual* _temp753; void* _temp755; _LL747: if(( unsigned int) _temp745 >
4u?(( struct _tunion_struct*) _temp745)->tag == Cyc_Absyn_ArrayType_tag: 0){
_LL756: _temp755=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp745)->f1;
goto _LL754; _LL754: _temp753=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp745)->f2; goto _LL752; _LL752: _temp751=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp745)->f3; goto _LL748;} else{ goto _LL749;}
_LL749: goto _LL750; _LL748: Cyc_Tcutil_terr( loc,( struct _tagged_string)({
char* _temp757=( char*)"cannot assign to an array"; struct _tagged_string
_temp758; _temp758.curr= _temp757; _temp758.base= _temp757; _temp758.last_plus_one=
_temp757 + 26; _temp758;})); goto _LL746; _LL750: goto _LL746; _LL746:;} if( !
Cyc_Absyn_is_lvalue( e1)){ return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_string)({ char* _temp759=( char*)"assignment to non-lvalue"; struct
_tagged_string _temp760; _temp760.curr= _temp759; _temp760.base= _temp759;
_temp760.last_plus_one= _temp759 + 25; _temp760;}));} Cyc_Tcexp_check_writable(
te, e1); if( po == 0){ if( ! Cyc_Tcutil_coerce_assign( te, e2, t1)){ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp761= Cyc_Absynpp_typ2string( t1); struct
_tagged_string _temp762= Cyc_Absynpp_typ2string( t2); xprintf("type mismatch: %.*s != %.*s",
_temp761.last_plus_one - _temp761.curr, _temp761.curr, _temp762.last_plus_one -
_temp762.curr, _temp762.curr);}));}} else{ void* p=( void*)({ struct Cyc_Core_Opt*
_temp765= po; if( _temp765 == 0){ _throw( Null_Exception);} _temp765->v;}); void*
t_result= Cyc_Tcexp_tcBinPrimop( te, loc, 0, p, e1, e2); if( !( Cyc_Tcutil_unify(
t_result, t1)? 1: Cyc_Tcutil_coerceable( t_result))){ return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_string)({ char* _temp763=( char*)"Cannot use this operator in an assignment";
struct _tagged_string _temp764; _temp764.curr= _temp763; _temp764.base= _temp763;
_temp764.last_plus_one= _temp763 + 42; _temp764;}));}} return t2;}} static void*
Cyc_Tcexp_tcSeqExp( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp(
te, 0, e1); Cyc_Tcexp_tcExp( te, topt, e2); return( void*)({ struct Cyc_Core_Opt*
_temp768= e2->topt; if( _temp768 == 0){ _throw( Null_Exception);} _temp768->v;});}
static void* Cyc_Tcexp_tcFnCall( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_List_List* es){ Cyc_Tcexp_tcExp(
te, 0, e);{ void* t= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp826= e->topt; if( _temp826 == 0){ _throw( Null_Exception);} _temp826->v;}));
void* _temp769= t; struct Cyc_Absyn_PtrInfo _temp775; struct Cyc_Absyn_Conref*
_temp777; struct Cyc_Absyn_Tqual* _temp779; struct Cyc_Absyn_Conref* _temp781;
void* _temp783; void* _temp785; _LL771: if(( unsigned int) _temp769 > 4u?((
struct _tunion_struct*) _temp769)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL776:
_temp775=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp769)->f1; _LL786: _temp785=( void*) _temp775.elt_typ; goto _LL784; _LL784:
_temp783=( void*) _temp775.rgn_typ; goto _LL782; _LL782: _temp781=( struct Cyc_Absyn_Conref*)
_temp775.nullable; goto _LL780; _LL780: _temp779=( struct Cyc_Absyn_Tqual*)
_temp775.tq; goto _LL778; _LL778: _temp777=( struct Cyc_Absyn_Conref*) _temp775.bounds;
goto _LL772;} else{ goto _LL773;} _LL773: goto _LL774; _LL772: Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp783); Cyc_Tcutil_check_nonzero_bound( loc, _temp777);{ void*
_temp787= Cyc_Tcutil_compress( _temp785); struct Cyc_Absyn_FnInfo _temp793;
struct Cyc_List_List* _temp795; int _temp797; struct Cyc_List_List* _temp799;
void* _temp801; struct Cyc_Core_Opt* _temp803; struct Cyc_List_List* _temp805;
_LL789: if(( unsigned int) _temp787 > 4u?(( struct _tunion_struct*) _temp787)->tag
== Cyc_Absyn_FnType_tag: 0){ _LL794: _temp793=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp787)->f1; _LL806: _temp805=( struct Cyc_List_List*)
_temp793.tvars; goto _LL804; _LL804: _temp803=( struct Cyc_Core_Opt*) _temp793.effect;
goto _LL802; _LL802: _temp801=( void*) _temp793.ret_typ; goto _LL800; _LL800:
_temp799=( struct Cyc_List_List*) _temp793.args; goto _LL798; _LL798: _temp797=(
int) _temp793.varargs; goto _LL796; _LL796: _temp795=( struct Cyc_List_List*)
_temp793.attributes; goto _LL790;} else{ goto _LL791;} _LL791: goto _LL792;
_LL790: if( _temp797){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp807=( char*)"vararg functions (...) not supported yet"; struct
_tagged_string _temp808; _temp808.curr= _temp807; _temp808.base= _temp807;
_temp808.last_plus_one= _temp807 + 41; _temp808;}));} if( topt != 0){ Cyc_Tcutil_unify(
_temp801,({ void** _temp809= topt; if( _temp809 == 0){ _throw( Null_Exception);}*
_temp809;}));} while( es != 0? _temp799 != 0: 0) { struct Cyc_Absyn_Exp* e1=(
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp816= es; if( _temp816 == 0){
_throw( Null_Exception);} _temp816->hd;}); void* t2=(*(( struct _tuple2*)({
struct Cyc_List_List* _temp815= _temp799; if( _temp815 == 0){ _throw(
Null_Exception);} _temp815->hd;}))).f3; Cyc_Tcexp_tcExp( te,( void**)& t2, e1);
if( ! Cyc_Tcutil_coerce_arg( te, e1, t2)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp811= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp810= e1->topt; if( _temp810 == 0){ _throw( Null_Exception);} _temp810->v;}));
struct _tagged_string _temp812= Cyc_Absynpp_typ2string( t2); xprintf("actual argument has type %.*s but formal has type %.*s",
_temp811.last_plus_one - _temp811.curr, _temp811.curr, _temp812.last_plus_one -
_temp812.curr, _temp812.curr);}));} es=({ struct Cyc_List_List* _temp813= es;
if( _temp813 == 0){ _throw( Null_Exception);} _temp813->tl;}); _temp799=({
struct Cyc_List_List* _temp814= _temp799; if( _temp814 == 0){ _throw(
Null_Exception);} _temp814->tl;});} if( es != 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp817=( char*)"too many arguments for function";
struct _tagged_string _temp818; _temp818.curr= _temp817; _temp818.base= _temp817;
_temp818.last_plus_one= _temp817 + 32; _temp818;}));} if( _temp799 != 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp819=( char*)"too few arguments for function";
struct _tagged_string _temp820; _temp820.curr= _temp819; _temp820.base= _temp819;
_temp820.last_plus_one= _temp819 + 31; _temp820;}));} Cyc_Tcenv_check_effect_accessible(
te, loc,( void*)({ struct Cyc_Core_Opt* _temp821= _temp803; if( _temp821 == 0){
_throw( Null_Exception);} _temp821->v;})); return _temp801; _LL792: return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_string)({ char* _temp822=( char*)"expected pointer to function";
struct _tagged_string _temp823; _temp823.curr= _temp822; _temp823.base= _temp822;
_temp823.last_plus_one= _temp822 + 29; _temp823;})); _LL788:;} _LL774: return
Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char* _temp824=( char*)"expected pointer to function";
struct _tagged_string _temp825; _temp825.curr= _temp824; _temp825.base= _temp824;
_temp825.last_plus_one= _temp824 + 29; _temp825;})); _LL770:;}} static void* Cyc_Tcexp_tcThrow(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp( te,( void**)& Cyc_Absyn_exn_typ, e); if( !
Cyc_Tcutil_coerce_use( te, e, Cyc_Absyn_exn_typ)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp828= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp827= e->topt; if( _temp827 == 0){ _throw( Null_Exception);} _temp827->v;}));
xprintf("expected xtunion exn but found %.*s", _temp828.last_plus_one - _temp828.curr,
_temp828.curr);}));} return Cyc_Absyn_wildtyp();} static void* Cyc_Tcexp_tcInstantiate(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e, struct Cyc_List_List* ts){ Cyc_Tcexp_tcExpNoInst( te, 0, e);(
void*)({ struct Cyc_Core_Opt* _temp829= e->topt; if( _temp829 == 0){ _throw(
Null_Exception);} _temp829->v=( void*) Cyc_Absyn_pointer_expand( Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp830= e->topt; if( _temp830 == 0){ _throw(
Null_Exception);} _temp830->v;})));});{ void* t1= Cyc_Tcutil_compress(( void*)({
struct Cyc_Core_Opt* _temp886= e->topt; if( _temp886 == 0){ _throw(
Null_Exception);} _temp886->v;}));{ void* _temp831= t1; struct Cyc_Absyn_PtrInfo
_temp837; struct Cyc_Absyn_Conref* _temp839; struct Cyc_Absyn_Tqual* _temp841;
struct Cyc_Absyn_Conref* _temp843; void* _temp845; void* _temp847; _LL833: if((
unsigned int) _temp831 > 4u?(( struct _tunion_struct*) _temp831)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL838: _temp837=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp831)->f1; _LL848: _temp847=( void*) _temp837.elt_typ; goto _LL846; _LL846:
_temp845=( void*) _temp837.rgn_typ; goto _LL844; _LL844: _temp843=( struct Cyc_Absyn_Conref*)
_temp837.nullable; goto _LL842; _LL842: _temp841=( struct Cyc_Absyn_Tqual*)
_temp837.tq; goto _LL840; _LL840: _temp839=( struct Cyc_Absyn_Conref*) _temp837.bounds;
goto _LL834;} else{ goto _LL835;} _LL835: goto _LL836; _LL834:{ void* _temp849=
Cyc_Tcutil_compress( _temp847); struct Cyc_Absyn_FnInfo _temp855; struct Cyc_List_List*
_temp857; int _temp859; struct Cyc_List_List* _temp861; void* _temp863; struct
Cyc_Core_Opt* _temp865; struct Cyc_List_List* _temp867; _LL851: if((
unsigned int) _temp849 > 4u?(( struct _tunion_struct*) _temp849)->tag == Cyc_Absyn_FnType_tag:
0){ _LL856: _temp855=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp849)->f1; _LL868: _temp867=( struct Cyc_List_List*) _temp855.tvars; goto
_LL866; _LL866: _temp865=( struct Cyc_Core_Opt*) _temp855.effect; goto _LL864;
_LL864: _temp863=( void*) _temp855.ret_typ; goto _LL862; _LL862: _temp861=(
struct Cyc_List_List*) _temp855.args; goto _LL860; _LL860: _temp859=( int)
_temp855.varargs; goto _LL858; _LL858: _temp857=( struct Cyc_List_List*)
_temp855.attributes; goto _LL852;} else{ goto _LL853;} _LL853: goto _LL854;
_LL852: { struct Cyc_List_List* instantiation= 0; if((( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( ts) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp867)){ return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char*
_temp869=( char*)"type instantiation mismatch"; struct _tagged_string _temp870;
_temp870.curr= _temp869; _temp870.base= _temp869; _temp870.last_plus_one=
_temp869 + 28; _temp870;}));} for( 0; ts != 0; ts=({ struct Cyc_List_List*
_temp871= ts; if( _temp871 == 0){ _throw( Null_Exception);} _temp871->tl;}),
_temp867=({ struct Cyc_List_List* _temp872= _temp867; if( _temp872 == 0){ _throw(
Null_Exception);} _temp872->tl;})){ void* k=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp878= _temp867; if( _temp878 == 0){ _throw( Null_Exception);} _temp878->hd;}))->kind);
Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars( te), k,( void*)({
struct Cyc_List_List* _temp873= ts; if( _temp873 == 0){ _throw( Null_Exception);}
_temp873->hd;})); instantiation=({ struct Cyc_List_List* _temp874=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp874->hd=( void*)({ struct
_tuple4* _temp875=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp875->f1=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp877=
_temp867; if( _temp877 == 0){ _throw( Null_Exception);} _temp877->hd;});
_temp875->f2=( void*)({ struct Cyc_List_List* _temp876= ts; if( _temp876 == 0){
_throw( Null_Exception);} _temp876->hd;}); _temp875;}); _temp874->tl=
instantiation; _temp874;});}{ void* new_fn_typ= Cyc_Tcutil_substitute(
instantiation,( void*)({ struct Cyc_Absyn_FnType_struct* _temp882=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp882[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp883; _temp883.tag= Cyc_Absyn_FnType_tag; _temp883.f1=({ struct Cyc_Absyn_FnInfo
_temp884; _temp884.tvars= 0; _temp884.effect= _temp865; _temp884.ret_typ=( void*)
_temp863; _temp884.args= _temp861; _temp884.varargs= _temp859; _temp884.attributes=
_temp857; _temp884;}); _temp883;}); _temp882;})); void* new_typ=( void*)({
struct Cyc_Absyn_PointerType_struct* _temp879=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp879[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp880; _temp880.tag= Cyc_Absyn_PointerType_tag;
_temp880.f1=({ struct Cyc_Absyn_PtrInfo _temp881; _temp881.elt_typ=( void*)
new_fn_typ; _temp881.rgn_typ=( void*) _temp845; _temp881.nullable= _temp843;
_temp881.tq= _temp841; _temp881.bounds= _temp839; _temp881;}); _temp880;});
_temp879;}); return new_typ;}} _LL854: goto _LL850; _LL850:;} goto _LL832;
_LL836: goto _LL832; _LL832:;} return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp885= Cyc_Absynpp_typ2string( t1); xprintf("expecting polymorphic type but found %.*s",
_temp885.last_plus_one - _temp885.curr, _temp885.curr);}));}} static void* Cyc_Tcexp_tcCast(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, void*
t, struct Cyc_Absyn_Exp* e){ Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars(
te),( void*) Cyc_Absyn_MemKind, t); Cyc_Tcexp_tcExp( te, 0, e);{ void* t2=( void*)({
struct Cyc_Core_Opt* _temp889= e->topt; if( _temp889 == 0){ _throw(
Null_Exception);} _temp889->v;}); if( ! Cyc_Tcutil_silent_castable( te, loc, t2,
t)? ! Cyc_Tcutil_castable( te, loc, t2, t): 0){ return Cyc_Tcexp_expr_err( te,
loc,({ struct _tagged_string _temp887= Cyc_Absynpp_typ2string( t2); struct
_tagged_string _temp888= Cyc_Absynpp_typ2string( t); xprintf("cannot cast %.*s to %.*s",
_temp887.last_plus_one - _temp887.curr, _temp887.curr, _temp888.last_plus_one -
_temp888.curr, _temp888.curr);}));} return t;}} static void* Cyc_Tcexp_tcAddress(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e){ void** topt2= 0; struct Cyc_Absyn_Tqual* tq2= Cyc_Absyn_empty_tqual();
if( topt != 0){ void* _temp891= Cyc_Tcutil_compress(({ void** _temp890= topt;
if( _temp890 == 0){ _throw( Null_Exception);}* _temp890;})); struct Cyc_Absyn_PtrInfo
_temp897; struct Cyc_Absyn_Conref* _temp899; struct Cyc_Absyn_Tqual* _temp901;
struct Cyc_Absyn_Conref* _temp903; void* _temp905; void* _temp907; _LL893: if((
unsigned int) _temp891 > 4u?(( struct _tunion_struct*) _temp891)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL898: _temp897=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp891)->f1; _LL908: _temp907=( void*) _temp897.elt_typ; goto _LL906; _LL906:
_temp905=( void*) _temp897.rgn_typ; goto _LL904; _LL904: _temp903=( struct Cyc_Absyn_Conref*)
_temp897.nullable; goto _LL902; _LL902: _temp901=( struct Cyc_Absyn_Tqual*)
_temp897.tq; goto _LL900; _LL900: _temp899=( struct Cyc_Absyn_Conref*) _temp897.bounds;
goto _LL894;} else{ goto _LL895;} _LL895: goto _LL896; _LL894: topt2=({ void**
_temp909=( void**) GC_malloc( sizeof( void*)); _temp909[ 0]= _temp907; _temp909;});
tq2= _temp901; goto _LL892; _LL896: goto _LL892; _LL892:;} Cyc_Tcexp_tcExpNoInst(
te, topt2, e);{ void* _temp910=( void*) e->r; struct Cyc_Absyn_Structdecl*
_temp918; struct Cyc_List_List* _temp920; struct Cyc_Core_Opt* _temp922; struct
_tuple1* _temp924; struct Cyc_List_List* _temp926; _LL912: if((( struct
_tunion_struct*) _temp910)->tag == Cyc_Absyn_Struct_e_tag){ _LL925: _temp924=(
struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*) _temp910)->f1; goto _LL923;
_LL923: _temp922=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp910)->f2; goto _LL921; _LL921: _temp920=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp910)->f3; goto _LL919; _LL919: _temp918=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*) _temp910)->f4;
goto _LL913;} else{ goto _LL914;} _LL914: if((( struct _tunion_struct*) _temp910)->tag
== Cyc_Absyn_Tuple_e_tag){ _LL927: _temp926=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Tuple_e_struct*) _temp910)->f1; goto _LL915;} else{ goto _LL916;}
_LL916: goto _LL917; _LL913: goto _LL915; _LL915: Cyc_Tcutil_warn( loc,( struct
_tagged_string)({ char* _temp928=( char*)"& used to allocate"; struct
_tagged_string _temp929; _temp929.curr= _temp928; _temp929.base= _temp928;
_temp929.last_plus_one= _temp928 + 19; _temp929;}));{ void*(* ptr_maker)( void*
t, void* rgn, struct Cyc_Absyn_Tqual* tq)= Cyc_Absyn_at_typ; if( topt != 0){
void* _temp931= Cyc_Tcutil_compress(({ void** _temp930= topt; if( _temp930 == 0){
_throw( Null_Exception);}* _temp930;})); struct Cyc_Absyn_PtrInfo _temp937;
struct Cyc_Absyn_Conref* _temp939; struct Cyc_Absyn_Tqual* _temp941; struct Cyc_Absyn_Conref*
_temp943; void* _temp945; void* _temp947; _LL933: if(( unsigned int) _temp931 >
4u?(( struct _tunion_struct*) _temp931)->tag == Cyc_Absyn_PointerType_tag: 0){
_LL938: _temp937=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp931)->f1; _LL948: _temp947=( void*) _temp937.elt_typ; goto _LL946; _LL946:
_temp945=( void*) _temp937.rgn_typ; goto _LL944; _LL944: _temp943=( struct Cyc_Absyn_Conref*)
_temp937.nullable; goto _LL942; _LL942: _temp941=( struct Cyc_Absyn_Tqual*)
_temp937.tq; goto _LL940; _LL940: _temp939=( struct Cyc_Absyn_Conref*) _temp937.bounds;
goto _LL934;} else{ goto _LL935;} _LL935: goto _LL936; _LL934: if((( int(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp943)){ ptr_maker= Cyc_Absyn_star_typ;}
goto _LL932; _LL936: goto _LL932; _LL932:;} return ptr_maker(( void*)({ struct
Cyc_Core_Opt* _temp949= e->topt; if( _temp949 == 0){ _throw( Null_Exception);}
_temp949->v;}),( void*) Cyc_Absyn_HeapRgn, tq2);} _LL917: goto _LL911; _LL911:;}{
void* _temp952; int _temp954; struct _tuple5 _temp950= Cyc_Tcutil_addressof_props(
te, e); _LL955: _temp954= _temp950.f1; goto _LL953; _LL953: _temp952= _temp950.f2;
goto _LL951; _LL951: { struct Cyc_Absyn_Tqual* tq= Cyc_Absyn_empty_tqual(); if(
_temp954){ tq->q_const= 1;}{ void* t= Cyc_Absyn_at_typ(( void*)({ struct Cyc_Core_Opt*
_temp956= e->topt; if( _temp956 == 0){ _throw( Null_Exception);} _temp956->v;}),
_temp952, tq); return t;}}}} static void* Cyc_Tcexp_tcSizeof( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, void* t){ Cyc_Tcutil_check_type(
loc, te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind, t); Cyc_Evexp_szof(
t, loc); return Cyc_Absyn_uint_t;} static void* Cyc_Tcexp_tcDeref( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp(
te, 0, e);{ void* t= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp976= e->topt; if( _temp976 == 0){ _throw( Null_Exception);} _temp976->v;}));
void* _temp957= t; struct Cyc_Absyn_PtrInfo _temp963; struct Cyc_Absyn_Conref*
_temp965; struct Cyc_Absyn_Tqual* _temp967; struct Cyc_Absyn_Conref* _temp969;
void* _temp971; void* _temp973; _LL959: if(( unsigned int) _temp957 > 4u?((
struct _tunion_struct*) _temp957)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL964:
_temp963=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp957)->f1; _LL974: _temp973=( void*) _temp963.elt_typ; goto _LL972; _LL972:
_temp971=( void*) _temp963.rgn_typ; goto _LL970; _LL970: _temp969=( struct Cyc_Absyn_Conref*)
_temp963.nullable; goto _LL968; _LL968: _temp967=( struct Cyc_Absyn_Tqual*)
_temp963.tq; goto _LL966; _LL966: _temp965=( struct Cyc_Absyn_Conref*) _temp963.bounds;
goto _LL960;} else{ goto _LL961;} _LL961: goto _LL962; _LL960: Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp971); Cyc_Tcutil_check_nonzero_bound( loc, _temp965); return
_temp973; _LL962: return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string
_temp975= Cyc_Absynpp_typ2string( t); xprintf("expecting * or @ type but found %.*s",
_temp975.last_plus_one - _temp975.curr, _temp975.curr);})); _LL958:;}} static
void* Cyc_Tcexp_tcStructMember( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* outer_e, struct Cyc_Absyn_Exp* e, struct
_tagged_string* f){ Cyc_Tcexp_tcExpNoPromote( te, 0, e);{ void* t= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp1039= e->topt; if( _temp1039 == 0){ _throw(
Null_Exception);} _temp1039->v;})); void* _temp977= t; struct Cyc_Absyn_Structdecl**
_temp989; struct Cyc_Absyn_Structdecl* _temp991; struct Cyc_List_List* _temp992;
struct _tuple1* _temp994; struct Cyc_Absyn_Uniondecl** _temp996; struct Cyc_Absyn_Uniondecl*
_temp998; struct Cyc_List_List* _temp999; struct _tuple1* _temp1001; struct Cyc_Absyn_Exp*
_temp1004; struct Cyc_Absyn_Tqual* _temp1006; void* _temp1008; struct Cyc_Absyn_PtrInfo
_temp1013; _LL979: if(( unsigned int) _temp977 > 4u?(( struct _tunion_struct*)
_temp977)->tag == Cyc_Absyn_StructType_tag: 0){ _LL995: _temp994=( struct
_tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp977)->f1; goto _LL993;
_LL993: _temp992=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp977)->f2; goto _LL990; _LL990: _temp989=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp977)->f3; if( _temp989 == 0){ goto
_LL981;} else{ _temp991=* _temp989; goto _LL980;}} else{ goto _LL981;} _LL981:
if(( unsigned int) _temp977 > 4u?(( struct _tunion_struct*) _temp977)->tag ==
Cyc_Absyn_UnionType_tag: 0){ _LL1002: _temp1001=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp977)->f1; goto _LL1000; _LL1000: _temp999=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp977)->f2; goto _LL997; _LL997: _temp996=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp977)->f3;
if( _temp996 == 0){ goto _LL983;} else{ _temp998=* _temp996; goto _LL982;}}
else{ goto _LL983;} _LL983: if(( unsigned int) _temp977 > 4u?(( struct
_tunion_struct*) _temp977)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL1009:
_temp1008=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp977)->f1; goto
_LL1007; _LL1007: _temp1006=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp977)->f2; goto _LL1005; _LL1005: _temp1004=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp977)->f3; goto _LL1003;} else{ goto
_LL985;} _LL1003: if( Cyc_String_zstrcmp(* f,( struct _tagged_string)({ char*
_temp1010=( char*)"size"; struct _tagged_string _temp1011; _temp1011.curr=
_temp1010; _temp1011.base= _temp1010; _temp1011.last_plus_one= _temp1010 + 5;
_temp1011;})) == 0){ goto _LL984;} else{ goto _LL985;} _LL985: if(( unsigned int)
_temp977 > 4u?(( struct _tunion_struct*) _temp977)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL1014: _temp1013=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp977)->f1; goto _LL1012;} else{ goto _LL987;} _LL1012: if( Cyc_String_zstrcmp(*
f,( struct _tagged_string)({ char* _temp1015=( char*)"size"; struct
_tagged_string _temp1016; _temp1016.curr= _temp1015; _temp1016.base= _temp1015;
_temp1016.last_plus_one= _temp1015 + 5; _temp1016;})) == 0){ goto _LL986;} else{
goto _LL987;} _LL987: goto _LL988; _LL980: if( _temp994 == 0){ return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_string)({ char* _temp1017=( char*)"unresolved struct type (COMPILER ERROR)";
struct _tagged_string _temp1018; _temp1018.curr= _temp1017; _temp1018.base=
_temp1017; _temp1018.last_plus_one= _temp1017 + 40; _temp1018;}));}{ struct Cyc_Absyn_Structfield*
finfo= Cyc_Absyn_lookup_struct_field( _temp991, f); if( finfo == 0){ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp1020= Cyc_Absynpp_qvar2string(( struct
_tuple1*)({ struct _tuple1* _temp1019= _temp994; if( _temp1019 == 0){ _throw(
Null_Exception);} _temp1019;})); struct _tagged_string _temp1021=* f; xprintf("struct %.*s has no %.*s field",
_temp1020.last_plus_one - _temp1020.curr, _temp1020.curr, _temp1021.last_plus_one
- _temp1021.curr, _temp1021.curr);}));}{ struct Cyc_List_List* inst=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
_temp991->tvs, _temp992); void* t2= Cyc_Tcutil_substitute( inst,( void*)({
struct Cyc_Absyn_Structfield* _temp1022= finfo; if( _temp1022 == 0){ _throw(
Null_Exception);} _temp1022->type;})); return t2;}} _LL982: if( _temp1001 == 0){
return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char* _temp1023=(
char*)"unresolved union type (COMPILER ERROR)"; struct _tagged_string _temp1024;
_temp1024.curr= _temp1023; _temp1024.base= _temp1023; _temp1024.last_plus_one=
_temp1023 + 39; _temp1024;}));}{ struct Cyc_Absyn_Structfield* finfo= Cyc_Absyn_lookup_union_field(
_temp998, f); if( finfo == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp1026= Cyc_Absynpp_qvar2string(( struct _tuple1*)({ struct
_tuple1* _temp1025= _temp1001; if( _temp1025 == 0){ _throw( Null_Exception);}
_temp1025;})); struct _tagged_string _temp1027=* f; xprintf("union %.*s has no %.*s field",
_temp1026.last_plus_one - _temp1026.curr, _temp1026.curr, _temp1027.last_plus_one
- _temp1027.curr, _temp1027.curr);}));}{ struct Cyc_List_List* inst=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
_temp998->tvs, _temp999); void* t2= Cyc_Tcutil_substitute( inst,( void*)({
struct Cyc_Absyn_Structfield* _temp1028= finfo; if( _temp1028 == 0){ _throw(
Null_Exception);} _temp1028->type;})); return t2;}} _LL984:( void*)( outer_e->r=(
void*)(( void*)({ struct Cyc_Absyn_Primop_e_struct* _temp1029=( struct Cyc_Absyn_Primop_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp1029[ 0]=({ struct
Cyc_Absyn_Primop_e_struct _temp1030; _temp1030.tag= Cyc_Absyn_Primop_e_tag;
_temp1030.f1=( void*)(( void*) Cyc_Absyn_Size); _temp1030.f2=({ struct Cyc_List_List*
_temp1031=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1031->hd=( void*) e; _temp1031->tl= 0; _temp1031;}); _temp1030;});
_temp1029;}))); return Cyc_Absyn_uint_t; _LL986:( void*)( outer_e->r=( void*)((
void*)({ struct Cyc_Absyn_Primop_e_struct* _temp1032=( struct Cyc_Absyn_Primop_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp1032[ 0]=({ struct
Cyc_Absyn_Primop_e_struct _temp1033; _temp1033.tag= Cyc_Absyn_Primop_e_tag;
_temp1033.f1=( void*)(( void*) Cyc_Absyn_Size); _temp1033.f2=({ struct Cyc_List_List*
_temp1034=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1034->hd=( void*) e; _temp1034->tl= 0; _temp1034;}); _temp1033;});
_temp1032;}))); return Cyc_Absyn_uint_t; _LL988: if( Cyc_String_zstrcmp(* f,(
struct _tagged_string)({ char* _temp1035=( char*)"size"; struct _tagged_string
_temp1036; _temp1036.curr= _temp1035; _temp1036.base= _temp1035; _temp1036.last_plus_one=
_temp1035 + 5; _temp1036;})) == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp1037= Cyc_Absynpp_typ2string( t); xprintf("expecting struct, union or array, found %.*s",
_temp1037.last_plus_one - _temp1037.curr, _temp1037.curr);}));} else{ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp1038= Cyc_Absynpp_typ2string( t); xprintf("expecting struct or union, found %.*s",
_temp1038.last_plus_one - _temp1038.curr, _temp1038.curr);}));} _LL978:;}}
static void* Cyc_Tcexp_tcStructArrow( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e, struct _tagged_string* f){ Cyc_Tcexp_tcExp(
te, 0, e);{ void* t= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp1090= e->topt; if( _temp1090 == 0){ _throw( Null_Exception);} _temp1090->v;}));
void* _temp1040= t; struct Cyc_Absyn_PtrInfo _temp1046; struct Cyc_Absyn_Conref*
_temp1048; struct Cyc_Absyn_Tqual* _temp1050; struct Cyc_Absyn_Conref* _temp1052;
void* _temp1054; void* _temp1056; _LL1042: if(( unsigned int) _temp1040 > 4u?((
struct _tunion_struct*) _temp1040)->tag == Cyc_Absyn_PointerType_tag: 0){
_LL1047: _temp1046=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1040)->f1; _LL1057: _temp1056=( void*) _temp1046.elt_typ; goto _LL1055;
_LL1055: _temp1054=( void*) _temp1046.rgn_typ; goto _LL1053; _LL1053: _temp1052=(
struct Cyc_Absyn_Conref*) _temp1046.nullable; goto _LL1051; _LL1051: _temp1050=(
struct Cyc_Absyn_Tqual*) _temp1046.tq; goto _LL1049; _LL1049: _temp1048=( struct
Cyc_Absyn_Conref*) _temp1046.bounds; goto _LL1043;} else{ goto _LL1044;} _LL1044:
goto _LL1045; _LL1043: Cyc_Tcutil_check_nonzero_bound( loc, _temp1048);{ void*
_temp1058= Cyc_Tcutil_compress( _temp1056); struct Cyc_Absyn_Structdecl**
_temp1066; struct Cyc_Absyn_Structdecl* _temp1068; struct Cyc_List_List*
_temp1069; struct _tuple1* _temp1071; struct Cyc_Absyn_Uniondecl** _temp1073;
struct Cyc_Absyn_Uniondecl* _temp1075; struct Cyc_List_List* _temp1076; struct
_tuple1* _temp1078; _LL1060: if(( unsigned int) _temp1058 > 4u?(( struct
_tunion_struct*) _temp1058)->tag == Cyc_Absyn_StructType_tag: 0){ _LL1072:
_temp1071=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp1058)->f1;
goto _LL1070; _LL1070: _temp1069=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp1058)->f2; goto _LL1067; _LL1067: _temp1066=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp1058)->f3; if( _temp1066 == 0){ goto
_LL1062;} else{ _temp1068=* _temp1066; goto _LL1061;}} else{ goto _LL1062;}
_LL1062: if(( unsigned int) _temp1058 > 4u?(( struct _tunion_struct*) _temp1058)->tag
== Cyc_Absyn_UnionType_tag: 0){ _LL1079: _temp1078=( struct _tuple1*)(( struct
Cyc_Absyn_UnionType_struct*) _temp1058)->f1; goto _LL1077; _LL1077: _temp1076=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*) _temp1058)->f2;
goto _LL1074; _LL1074: _temp1073=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp1058)->f3; if( _temp1073 == 0){ goto _LL1064;} else{ _temp1075=* _temp1073;
goto _LL1063;}} else{ goto _LL1064;} _LL1064: goto _LL1065; _LL1061: { struct
Cyc_Absyn_Structfield* finfo= Cyc_Absyn_lookup_struct_field( _temp1068, f); if(
finfo == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string
_temp1081= Cyc_Absynpp_qvar2string(( struct _tuple1*)({ struct _tuple1*
_temp1080= _temp1071; if( _temp1080 == 0){ _throw( Null_Exception);} _temp1080;}));
struct _tagged_string _temp1082=* f; xprintf("struct %.*s has no %.*s field",
_temp1081.last_plus_one - _temp1081.curr, _temp1081.curr, _temp1082.last_plus_one
- _temp1082.curr, _temp1082.curr);}));}{ struct Cyc_List_List* inst=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
_temp1068->tvs, _temp1069); void* t3= Cyc_Tcutil_substitute( inst,( void*)({
struct Cyc_Absyn_Structfield* _temp1083= finfo; if( _temp1083 == 0){ _throw(
Null_Exception);} _temp1083->type;})); Cyc_Tcenv_check_rgn_accessible( te, loc,
_temp1054); return t3;}} _LL1063: { struct Cyc_Absyn_Structfield* finfo= Cyc_Absyn_lookup_union_field(
_temp1075, f); if( finfo == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp1085= Cyc_Absynpp_qvar2string(( struct _tuple1*)({ struct
_tuple1* _temp1084= _temp1078; if( _temp1084 == 0){ _throw( Null_Exception);}
_temp1084;})); struct _tagged_string _temp1086=* f; xprintf("union %.*s has no %.*s field",
_temp1085.last_plus_one - _temp1085.curr, _temp1085.curr, _temp1086.last_plus_one
- _temp1086.curr, _temp1086.curr);}));}{ struct Cyc_List_List* inst=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
_temp1075->tvs, _temp1076); void* t3= Cyc_Tcutil_substitute( inst,( void*)({
struct Cyc_Absyn_Structfield* _temp1087= finfo; if( _temp1087 == 0){ _throw(
Null_Exception);} _temp1087->type;})); Cyc_Tcenv_check_rgn_accessible( te, loc,
_temp1054); return t3;}} _LL1065: return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp1088= Cyc_Absynpp_typ2string( t); xprintf("expecting struct pointer, found %.*s",
_temp1088.last_plus_one - _temp1088.curr, _temp1088.curr);})); _LL1059:;}
_LL1045: return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string _temp1089=
Cyc_Absynpp_typ2string( t); xprintf("expecting struct pointer, found %.*s",
_temp1089.last_plus_one - _temp1089.curr, _temp1089.curr);})); _LL1041:;}}
static void* Cyc_Tcexp_ithTupleType( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* ts, struct Cyc_Absyn_Exp* index){ unsigned int i= Cyc_Evexp_eval_const_uint_exp(
index); struct _handler_cons _temp1091; _push_handler(& _temp1091);{ struct
_xtunion_struct* _temp1092=( struct _xtunion_struct*) setjmp( _temp1091.handler);
if( ! _temp1092){{ void* _temp1093=(*(( struct _tuple7*(*)( struct Cyc_List_List*
x, int i)) Cyc_List_nth)( ts,( int) i)).f2; _npop_handler( 0u); return _temp1093;};
_pop_handler();} else{ struct _xtunion_struct* _temp1095= _temp1092; _LL1097:
if( _temp1095->tag == Cyc_List_Nth_tag){ goto _LL1098;} else{ goto _LL1099;}
_LL1099: goto _LL1100; _LL1098: return Cyc_Tcexp_expr_err( te, loc, xprintf("index is %d but tuple has only %d fields",(
int) i,(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( ts))); _LL1100:(
void) _throw( _temp1095); _LL1096:;}}} static void* Cyc_Tcexp_tcSubscript(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp( te, 0, e1); Cyc_Tcexp_tcExp(
te, 0, e2);{ void* t1= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp1126= e1->topt; if( _temp1126 == 0){ _throw( Null_Exception);} _temp1126->v;}));
void* t2= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp1125= e2->topt;
if( _temp1125 == 0){ _throw( Null_Exception);} _temp1125->v;})); if( ! Cyc_Tcutil_coerce_sint_typ(
te, e2)){ return Cyc_Tcexp_expr_err( te, e2->loc,({ struct _tagged_string
_temp1101= Cyc_Absynpp_typ2string( t2); xprintf("expecting int subscript, found %.*s",
_temp1101.last_plus_one - _temp1101.curr, _temp1101.curr);}));}{ void* _temp1102=
t1; struct Cyc_Absyn_PtrInfo _temp1110; struct Cyc_Absyn_Conref* _temp1112;
struct Cyc_Absyn_Tqual* _temp1114; struct Cyc_Absyn_Conref* _temp1116; void*
_temp1118; void* _temp1120; struct Cyc_List_List* _temp1122; _LL1104: if((
unsigned int) _temp1102 > 4u?(( struct _tunion_struct*) _temp1102)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL1111: _temp1110=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1102)->f1; _LL1121: _temp1120=( void*) _temp1110.elt_typ; goto _LL1119;
_LL1119: _temp1118=( void*) _temp1110.rgn_typ; goto _LL1117; _LL1117: _temp1116=(
struct Cyc_Absyn_Conref*) _temp1110.nullable; goto _LL1115; _LL1115: _temp1114=(
struct Cyc_Absyn_Tqual*) _temp1110.tq; goto _LL1113; _LL1113: _temp1112=( struct
Cyc_Absyn_Conref*) _temp1110.bounds; goto _LL1105;} else{ goto _LL1106;} _LL1106:
if(( unsigned int) _temp1102 > 4u?(( struct _tunion_struct*) _temp1102)->tag ==
Cyc_Absyn_TupleType_tag: 0){ _LL1123: _temp1122=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp1102)->f1; goto _LL1107;} else{ goto
_LL1108;} _LL1108: goto _LL1109; _LL1105: if( Cyc_Tcutil_is_const_exp( te, e2)){
Cyc_Tcutil_check_bound( loc, Cyc_Evexp_eval_const_uint_exp( e2), _temp1112);}
else{ Cyc_Tcutil_check_nonzero_bound( loc, _temp1112);} Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp1118); return _temp1120; _LL1107: return Cyc_Tcexp_ithTupleType(
te, loc, _temp1122, e2); _LL1109: return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp1124= Cyc_Absynpp_typ2string( t1); xprintf("subscript applied to %.*s",
_temp1124.last_plus_one - _temp1124.curr, _temp1124.curr);})); _LL1103:;}}}
static void* Cyc_Tcexp_tcTuple( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_List_List* es){ int done= 0; struct Cyc_List_List*
fields= 0; if( topt != 0){ void* _temp1128= Cyc_Tcutil_compress(({ void**
_temp1127= topt; if( _temp1127 == 0){ _throw( Null_Exception);}* _temp1127;}));
struct Cyc_List_List* _temp1134; _LL1130: if(( unsigned int) _temp1128 > 4u?((
struct _tunion_struct*) _temp1128)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL1135:
_temp1134=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp1128)->f1; goto _LL1131;} else{ goto _LL1132;} _LL1132: goto _LL1133;
_LL1131: if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1134) !=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es)){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp1136=( char*)"tuple expression has the wrong number of fields";
struct _tagged_string _temp1137; _temp1137.curr= _temp1136; _temp1137.base=
_temp1136; _temp1137.last_plus_one= _temp1136 + 48; _temp1137;})); goto _LL1129;}
for( 0; es != 0; es=({ struct Cyc_List_List* _temp1138= es; if( _temp1138 == 0){
_throw( Null_Exception);} _temp1138->tl;}), _temp1134=({ struct Cyc_List_List*
_temp1139= _temp1134; if( _temp1139 == 0){ _throw( Null_Exception);} _temp1139->tl;})){
void* topt2=(*(( struct _tuple7*)({ struct Cyc_List_List* _temp1146= _temp1134;
if( _temp1146 == 0){ _throw( Null_Exception);} _temp1146->hd;}))).f2; Cyc_Tcexp_tcExpInitializer(
te,( void**)& topt2,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1140=
es; if( _temp1140 == 0){ _throw( Null_Exception);} _temp1140->hd;})); fields=({
struct Cyc_List_List* _temp1141=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1141->hd=( void*)({ struct _tuple7* _temp1142=(
struct _tuple7*) GC_malloc( sizeof( struct _tuple7)); _temp1142->f1=(*(( struct
_tuple7*)({ struct Cyc_List_List* _temp1145= _temp1134; if( _temp1145 == 0){
_throw( Null_Exception);} _temp1145->hd;}))).f1; _temp1142->f2=( void*)({ struct
Cyc_Core_Opt* _temp1144=(( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1143= es; if( _temp1143 == 0){ _throw( Null_Exception);} _temp1143->hd;}))->topt;
if( _temp1144 == 0){ _throw( Null_Exception);} _temp1144->v;}); _temp1142;});
_temp1141->tl= fields; _temp1141;});} done= 1; goto _LL1129; _LL1133: goto
_LL1129; _LL1129:;} if( ! done){ for( 0; es != 0; es=({ struct Cyc_List_List*
_temp1147= es; if( _temp1147 == 0){ _throw( Null_Exception);} _temp1147->tl;})){
Cyc_Tcexp_tcExpInitializer( te, 0,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1148= es; if( _temp1148 == 0){ _throw( Null_Exception);} _temp1148->hd;}));
fields=({ struct Cyc_List_List* _temp1149=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1149->hd=( void*)({ struct _tuple7*
_temp1150=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7)); _temp1150->f1=
Cyc_Absyn_empty_tqual(); _temp1150->f2=( void*)({ struct Cyc_Core_Opt* _temp1152=((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1151= es; if( _temp1151 == 0){
_throw( Null_Exception);} _temp1151->hd;}))->topt; if( _temp1152 == 0){ _throw(
Null_Exception);} _temp1152->v;}); _temp1150;}); _temp1149->tl= fields;
_temp1149;});}} return( void*)({ struct Cyc_Absyn_TupleType_struct* _temp1153=(
struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp1153[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp1154; _temp1154.tag= Cyc_Absyn_TupleType_tag;
_temp1154.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fields); _temp1154;}); _temp1153;});} static void* Cyc_Tcexp_tcCompoundLit(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
_tuple2* t, struct Cyc_List_List* des){ return Cyc_Tcexp_expr_err( te, loc,(
struct _tagged_string)({ char* _temp1155=( char*)"tcCompoundLit"; struct
_tagged_string _temp1156; _temp1156.curr= _temp1155; _temp1156.base= _temp1155;
_temp1156.last_plus_one= _temp1155 + 14; _temp1156;}));} static void* Cyc_Tcexp_tcArray(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** elt_topt,
struct Cyc_List_List* des){ struct Cyc_List_List* es=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)((
struct Cyc_Absyn_Exp*(*)( struct _tuple6*)) Cyc_Core_snd, des); void* res= Cyc_Absyn_new_evar((
void*) Cyc_Absyn_MemKind); struct Cyc_Absyn_Const_e_struct* sz_rexp=({ struct
Cyc_Absyn_Const_e_struct* _temp1179=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp1179[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp1180; _temp1180.tag= Cyc_Absyn_Const_e_tag;
_temp1180.f1=( void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp1181=(
struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp1181[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp1182; _temp1182.tag= Cyc_Absyn_Int_c_tag;
_temp1182.f1=( void*)(( void*) Cyc_Absyn_Unsigned); _temp1182.f2=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( es); _temp1182;}); _temp1181;}));
_temp1180;}); _temp1179;}); struct Cyc_Absyn_Exp* sz_exp= Cyc_Absyn_new_exp((
void*) sz_rexp, loc); sz_exp->topt=({ struct Cyc_Core_Opt* _temp1157=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1157->v=( void*)
Cyc_Absyn_uint_t; _temp1157;});{ void* res_t2=( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp1177=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1177[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp1178; _temp1178.tag= Cyc_Absyn_ArrayType_tag;
_temp1178.f1=( void*) res; _temp1178.f2= Cyc_Absyn_empty_tqual(); _temp1178.f3=(
struct Cyc_Absyn_Exp*) sz_exp; _temp1178;}); _temp1177;});{ struct Cyc_List_List*
es2= es; for( 0; es2 != 0; es2=({ struct Cyc_List_List* _temp1158= es2; if(
_temp1158 == 0){ _throw( Null_Exception);} _temp1158->tl;})){ Cyc_Tcexp_tcExpInitializer(
te, elt_topt,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1159= es2;
if( _temp1159 == 0){ _throw( Null_Exception);} _temp1159->hd;}));}} if( ! Cyc_Tcutil_coerce_list(
te, res, es)){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1160= es; if( _temp1160 == 0){ _throw( Null_Exception);} _temp1160->hd;}))->loc,({
struct _tagged_string _temp1161= Cyc_Absynpp_typ2string( res); xprintf("elements of array do not all have the same type (%.*s)",
_temp1161.last_plus_one - _temp1161.curr, _temp1161.curr);}));}{ int offset= 0;
for( 0; des != 0; offset ++, des=({ struct Cyc_List_List* _temp1162= des; if(
_temp1162 == 0){ _throw( Null_Exception);} _temp1162->tl;})){ struct Cyc_List_List*
ds=(*(( struct _tuple6*)({ struct Cyc_List_List* _temp1176= des; if( _temp1176
== 0){ _throw( Null_Exception);} _temp1176->hd;}))).f1; if( ds != 0){ void*
_temp1164=( void*)({ struct Cyc_List_List* _temp1163= ds; if( _temp1163 == 0){
_throw( Null_Exception);} _temp1163->hd;}); struct _tagged_string* _temp1170;
struct Cyc_Absyn_Exp* _temp1172; _LL1166: if((( struct _tunion_struct*)
_temp1164)->tag == Cyc_Absyn_FieldName_tag){ _LL1171: _temp1170=( struct
_tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp1164)->f1; goto
_LL1167;} else{ goto _LL1168;} _LL1168: if((( struct _tunion_struct*) _temp1164)->tag
== Cyc_Absyn_ArrayElement_tag){ _LL1173: _temp1172=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayElement_struct*) _temp1164)->f1; goto _LL1169;} else{ goto
_LL1165;} _LL1167: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp1174=( char*)"only array index designators are supported"; struct
_tagged_string _temp1175; _temp1175.curr= _temp1174; _temp1175.base= _temp1174;
_temp1175.last_plus_one= _temp1174 + 43; _temp1175;})); goto _LL1165; _LL1169:
Cyc_Tcexp_tcExpInitializer( te, 0, _temp1172);{ unsigned int i= Cyc_Evexp_eval_const_uint_exp(
_temp1172); if( i != offset){ Cyc_Tcutil_terr( _temp1172->loc, xprintf("expecting index designator %d but found %d",
offset,( int) i));} goto _LL1165;} _LL1165:;}}} return res_t2;}} static void*
Cyc_Tcexp_tcComprehension( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp* bound,
struct Cyc_Absyn_Exp* body){ Cyc_Tcexp_tcExp( te, 0, bound); if( ! Cyc_Tcutil_coerce_uint_typ(
te, bound)){ Cyc_Tcutil_terr( bound->loc,({ struct _tagged_string _temp1184= Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Core_Opt* _temp1183= bound->topt; if( _temp1183 == 0){
_throw( Null_Exception);} _temp1183->v;})); xprintf("expecting unsigned int, found %.*s",
_temp1184.last_plus_one - _temp1184.curr, _temp1184.curr);}));} if( !( vd->tq)->q_const){((
void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp1185=( char*)"comprehension index variable is not declared const!";
struct _tagged_string _temp1186; _temp1186.curr= _temp1185; _temp1186.base=
_temp1185; _temp1186.last_plus_one= _temp1185 + 52; _temp1186;}));} if( te->le
!= 0){ te= Cyc_Tcenv_new_block( loc, te); te= Cyc_Tcenv_add_local_var( loc, te,
vd);} else{ if( ! Cyc_Tcutil_is_const_exp( te, bound)){ Cyc_Tcutil_terr( bound->loc,(
struct _tagged_string)({ char* _temp1187=( char*)"bound is not constant"; struct
_tagged_string _temp1188; _temp1188.curr= _temp1187; _temp1188.base= _temp1187;
_temp1188.last_plus_one= _temp1187 + 22; _temp1188;}));} if( ! Cyc_Tcutil_is_const_exp(
te, body)){ Cyc_Tcutil_terr( bound->loc,( struct _tagged_string)({ char*
_temp1189=( char*)"body is not constant"; struct _tagged_string _temp1190;
_temp1190.curr= _temp1189; _temp1190.base= _temp1189; _temp1190.last_plus_one=
_temp1189 + 21; _temp1190;}));}}{ struct Cyc_Absyn_PtrInfo pinfo; void** topt2=
0; struct Cyc_Absyn_Tqual** tqopt= 0; if( topt != 0){ void* _temp1192= Cyc_Tcutil_compress(({
void** _temp1191= topt; if( _temp1191 == 0){ _throw( Null_Exception);}*
_temp1191;})); struct Cyc_Absyn_PtrInfo _temp1200; struct Cyc_Absyn_Exp*
_temp1202; struct Cyc_Absyn_Tqual* _temp1204; struct Cyc_Absyn_Tqual** _temp1206;
void* _temp1207; void** _temp1209; _LL1194: if(( unsigned int) _temp1192 > 4u?((
struct _tunion_struct*) _temp1192)->tag == Cyc_Absyn_PointerType_tag: 0){
_LL1201: _temp1200=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1192)->f1; goto _LL1195;} else{ goto _LL1196;} _LL1196: if(( unsigned int)
_temp1192 > 4u?(( struct _tunion_struct*) _temp1192)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL1208: _temp1207=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1192)->f1;
_temp1209=&(( struct Cyc_Absyn_ArrayType_struct*) _temp1192)->f1; goto _LL1205;
_LL1205: _temp1204=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1192)->f2; _temp1206=&(( struct Cyc_Absyn_ArrayType_struct*) _temp1192)->f2;
goto _LL1203; _LL1203: _temp1202=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1192)->f3; goto _LL1197;} else{ goto _LL1198;} _LL1198: goto _LL1199;
_LL1195: pinfo= _temp1200; topt2=( void**)(( void*)& pinfo.elt_typ); tqopt=(
struct Cyc_Absyn_Tqual**)& pinfo.tq; goto _LL1193; _LL1197: topt2=( void**)
_temp1209; tqopt=( struct Cyc_Absyn_Tqual**) _temp1206; goto _LL1193; _LL1199:
goto _LL1193; _LL1193:;}{ void* t= Cyc_Tcexp_tcExp( te, topt2, body); return(
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp1210=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp1210[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp1211; _temp1211.tag= Cyc_Absyn_ArrayType_tag;
_temp1211.f1=( void*) t; _temp1211.f2= tqopt == 0? Cyc_Absyn_empty_tqual():({
struct Cyc_Absyn_Tqual** _temp1212= tqopt; if( _temp1212 == 0){ _throw(
Null_Exception);}* _temp1212;}); _temp1211.f3=( struct Cyc_Absyn_Exp*) bound;
_temp1211;}); _temp1210;});}}} static void* Cyc_Tcexp_tcStruct( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct _tuple1** tn, struct
Cyc_Core_Opt** otyps, struct Cyc_List_List* args, struct Cyc_Absyn_Structdecl**
sd_opt){ struct Cyc_Absyn_Structdecl* sd; if(* sd_opt != 0){ sd=( struct Cyc_Absyn_Structdecl*)({
struct Cyc_Absyn_Structdecl* _temp1213=* sd_opt; if( _temp1213 == 0){ _throw(
Null_Exception);} _temp1213;});} else{{ struct _handler_cons _temp1214;
_push_handler(& _temp1214);{ struct _xtunion_struct* _temp1215=( struct
_xtunion_struct*) setjmp( _temp1214.handler); if( ! _temp1215){ sd=* Cyc_Tcenv_lookup_structdecl(
te, loc,* tn);; _pop_handler();} else{ struct _xtunion_struct* _temp1217=
_temp1215; _LL1219: if( _temp1217->tag == Cyc_Dict_Absent_tag){ goto _LL1220;}
else{ goto _LL1221;} _LL1221: goto _LL1222; _LL1220: Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp1223= Cyc_Absynpp_qvar2string(* tn); xprintf("unbound struct name %.*s",
_temp1223.last_plus_one - _temp1223.curr, _temp1223.curr);})); return topt != 0?({
void** _temp1224= topt; if( _temp1224 == 0){ _throw( Null_Exception);}*
_temp1224;}):( void*) Cyc_Absyn_VoidType; _LL1222:( void) _throw( _temp1217);
_LL1218:;}}}* sd_opt=( struct Cyc_Absyn_Structdecl*) sd; if( sd->name != 0){* tn=(
struct _tuple1*)({ struct Cyc_Core_Opt* _temp1225= sd->name; if( _temp1225 == 0){
_throw( Null_Exception);} _temp1225->v;});}}{ struct Cyc_List_List* inst=((
struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_Absyn_Tvar*), struct
Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_make_inst_var, sd->tvs); struct Cyc_List_List*
typs=(( struct Cyc_List_List*(*)( void*(* f)( struct _tuple4*), struct Cyc_List_List*
x)) Cyc_List_map)(( void*(*)( struct _tuple4*)) Cyc_Core_snd, inst); struct Cyc_Absyn_StructType_struct*
res_typ=({ struct Cyc_Absyn_StructType_struct* _temp1242=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1242[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1243; _temp1243.tag= Cyc_Absyn_StructType_tag;
_temp1243.f1=( struct _tuple1*)* tn; _temp1243.f2= typs; _temp1243.f3=({ struct
Cyc_Absyn_Structdecl** _temp1244=( struct Cyc_Absyn_Structdecl**) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl*)); _temp1244[ 0]= sd; _temp1244;});
_temp1243;}); _temp1242;}); if( topt != 0){ Cyc_Tcutil_unify(( void*) res_typ,({
void** _temp1226= topt; if( _temp1226 == 0){ _throw( Null_Exception);}*
_temp1226;}));}* otyps=({ struct Cyc_Core_Opt* _temp1227=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1227->v=( void*) typs; _temp1227;});{
struct Cyc_List_List* fields=(( struct Cyc_List_List*(*)( struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_Absyn_Structdecl* sd)) Cyc_Tcutil_resolve_struct_designators)(
loc, args, sd); for( 0; fields != 0; fields=({ struct Cyc_List_List* _temp1228=
fields; if( _temp1228 == 0){ _throw( Null_Exception);} _temp1228->tl;})){ struct
_tuple8 _temp1232; struct Cyc_Absyn_Exp* _temp1233; struct Cyc_Absyn_Structfield*
_temp1235; struct _tuple8* _temp1230=( struct _tuple8*)({ struct Cyc_List_List*
_temp1229= fields; if( _temp1229 == 0){ _throw( Null_Exception);} _temp1229->hd;});
_temp1232=* _temp1230; _LL1236: _temp1235= _temp1232.f1; goto _LL1234; _LL1234:
_temp1233= _temp1232.f2; goto _LL1231; _LL1231: { void* inst_fieldtyp= Cyc_Tcutil_substitute(
inst,( void*) _temp1235->type); Cyc_Tcexp_tcExpInitializer( te,( void**)&
inst_fieldtyp, _temp1233); if( ! Cyc_Tcutil_coerce_arg( te, _temp1233,
inst_fieldtyp)){ Cyc_Tcutil_terr( _temp1233->loc,({ struct _tagged_string
_temp1238=* _temp1235->name; struct _tagged_string _temp1239= Cyc_Absynpp_qvar2string(*
tn); struct _tagged_string _temp1240= Cyc_Absynpp_typ2string( inst_fieldtyp);
struct _tagged_string _temp1241= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp1237= _temp1233->topt; if( _temp1237 == 0){ _throw( Null_Exception);}
_temp1237->v;})); xprintf("field %.*s of struct %.*s expects type %.*s != %.*s",
_temp1238.last_plus_one - _temp1238.curr, _temp1238.curr, _temp1239.last_plus_one
- _temp1239.curr, _temp1239.curr, _temp1240.last_plus_one - _temp1240.curr,
_temp1240.curr, _temp1241.last_plus_one - _temp1241.curr, _temp1241.curr);}));}}}
return( void*) res_typ;}}} static void* Cyc_Tcexp_tcTunion( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e,
struct Cyc_Core_Opt** all_ref, struct Cyc_Core_Opt** exist_ref, struct Cyc_List_List*
es, struct Cyc_Absyn_Tuniondecl* ed, struct Cyc_Absyn_Tunionfield* ef){ struct
Cyc_List_List* all_inst=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)(
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_make_inst_var,
ed->tvs); struct Cyc_List_List* exist_inst=(( struct Cyc_List_List*(*)( struct
_tuple4*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcutil_make_inst_var, ef->tvs); struct Cyc_List_List* all_typs=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple4*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple4*)) Cyc_Core_snd, all_inst); struct Cyc_List_List* exist_typs=((
struct Cyc_List_List*(*)( void*(* f)( struct _tuple4*), struct Cyc_List_List* x))
Cyc_List_map)(( void*(*)( struct _tuple4*)) Cyc_Core_snd, exist_inst); struct
Cyc_List_List* inst=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( all_inst, exist_inst);* all_ref=({ struct
Cyc_Core_Opt* _temp1245=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1245->v=( void*) all_typs; _temp1245;});* exist_ref=({ struct Cyc_Core_Opt*
_temp1246=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1246->v=( void*) exist_typs; _temp1246;});{ void* res=( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp1291=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp1291[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp1292; _temp1292.tag= Cyc_Absyn_TunionFieldType_tag; _temp1292.f1=({ struct
Cyc_Absyn_TunionFieldInfo _temp1293; _temp1293.name=( struct _tuple1*)({ struct
Cyc_Core_Opt* _temp1294= ed->name; if( _temp1294 == 0){ _throw( Null_Exception);}
_temp1294->v;}); _temp1293.targs= all_typs; _temp1293.fname= ef->name; _temp1293.tud=(
struct Cyc_Absyn_Tuniondecl*) ed; _temp1293.tufd=( struct Cyc_Absyn_Tunionfield*)
ef; _temp1293;}); _temp1292;}); _temp1291;}); if( topt != 0){ void* _temp1248=
Cyc_Tcutil_compress(({ void** _temp1247= topt; if( _temp1247 == 0){ _throw(
Null_Exception);}* _temp1247;})); struct Cyc_Absyn_TunionFieldInfo _temp1256;
struct Cyc_Absyn_TunionInfo _temp1258; struct Cyc_Absyn_Tuniondecl* _temp1260;
void* _temp1262; struct Cyc_List_List* _temp1264; struct _tuple1* _temp1266;
_LL1250: if(( unsigned int) _temp1248 > 4u?(( struct _tunion_struct*) _temp1248)->tag
== Cyc_Absyn_TunionFieldType_tag: 0){ _LL1257: _temp1256=( struct Cyc_Absyn_TunionFieldInfo)((
struct Cyc_Absyn_TunionFieldType_struct*) _temp1248)->f1; goto _LL1251;} else{
goto _LL1252;} _LL1252: if(( unsigned int) _temp1248 > 4u?(( struct
_tunion_struct*) _temp1248)->tag == Cyc_Absyn_TunionType_tag: 0){ _LL1259:
_temp1258=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp1248)->f1; _LL1267: _temp1266=( struct _tuple1*) _temp1258.name; goto
_LL1265; _LL1265: _temp1264=( struct Cyc_List_List*) _temp1258.targs; goto
_LL1263; _LL1263: _temp1262=( void*) _temp1258.rgn; goto _LL1261; _LL1261:
_temp1260=( struct Cyc_Absyn_Tuniondecl*) _temp1258.tud; goto _LL1253;} else{
goto _LL1254;} _LL1254: goto _LL1255; _LL1251: Cyc_Tcutil_unify(({ void**
_temp1268= topt; if( _temp1268 == 0){ _throw( Null_Exception);}* _temp1268;}),
res); goto _LL1249; _LL1253:{ struct Cyc_List_List* a= all_typs; for( 0; a != 0?
_temp1264 != 0: 0; a=({ struct Cyc_List_List* _temp1269= a; if( _temp1269 == 0){
_throw( Null_Exception);} _temp1269->tl;}), _temp1264=({ struct Cyc_List_List*
_temp1270= _temp1264; if( _temp1270 == 0){ _throw( Null_Exception);} _temp1270->tl;})){
Cyc_Tcutil_unify(( void*)({ struct Cyc_List_List* _temp1271= a; if( _temp1271 ==
0){ _throw( Null_Exception);} _temp1271->hd;}),( void*)({ struct Cyc_List_List*
_temp1272= _temp1264; if( _temp1272 == 0){ _throw( Null_Exception);} _temp1272->hd;}));}}
if( ef->typs == 0? es == 0: 0){ e->topt=({ struct Cyc_Core_Opt* _temp1273=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1273->v=(
void*) res; _temp1273;}); res=( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp1274=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp1274[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp1275; _temp1275.tag=
Cyc_Absyn_TunionType_tag; _temp1275.f1=({ struct Cyc_Absyn_TunionInfo _temp1276;
_temp1276.name=( struct _tuple1*)({ struct Cyc_Core_Opt* _temp1277= ed->name;
if( _temp1277 == 0){ _throw( Null_Exception);} _temp1277->v;}); _temp1276.targs=
all_typs; _temp1276.rgn=( void*) Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind);
_temp1276.tud=( struct Cyc_Absyn_Tuniondecl*) ed; _temp1276;}); _temp1275;});
_temp1274;}); Cyc_Tcutil_unchecked_cast( te, e, res); return res;} goto _LL1249;
_LL1255: goto _LL1249; _LL1249:;}{ struct Cyc_List_List* ts= ef->typs; for( 0;
es != 0? ts != 0: 0; es=({ struct Cyc_List_List* _temp1278= es; if( _temp1278 ==
0){ _throw( Null_Exception);} _temp1278->tl;}), ts=({ struct Cyc_List_List*
_temp1279= ts; if( _temp1279 == 0){ _throw( Null_Exception);} _temp1279->tl;})){
struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1287= es; if( _temp1287 == 0){ _throw( Null_Exception);} _temp1287->hd;});
void* t= Cyc_Tcutil_substitute( inst,(*(( struct _tuple7*)({ struct Cyc_List_List*
_temp1286= ts; if( _temp1286 == 0){ _throw( Null_Exception);} _temp1286->hd;}))).f2);
Cyc_Tcexp_tcExpInitializer( te,( void**)& t, e); if( ! Cyc_Tcutil_coerce_arg( te,
e, t)){ Cyc_Tcutil_terr( e->loc,({ struct _tagged_string _temp1283= Cyc_Absynpp_qvar2string(
ef->name); struct _tagged_string _temp1284= Cyc_Absynpp_typ2string( t); struct
_tagged_string _temp1285= e->topt == 0?( struct _tagged_string)({ char*
_temp1280=( char*)"?"; struct _tagged_string _temp1281; _temp1281.curr=
_temp1280; _temp1281.base= _temp1280; _temp1281.last_plus_one= _temp1280 + 2;
_temp1281;}): Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt* _temp1282=
e->topt; if( _temp1282 == 0){ _throw( Null_Exception);} _temp1282->v;}));
xprintf("tunion constructor %.*s expects argument of type %.*s but this argument has type %.*s",
_temp1283.last_plus_one - _temp1283.curr, _temp1283.curr, _temp1284.last_plus_one
- _temp1284.curr, _temp1284.curr, _temp1285.last_plus_one - _temp1285.curr,
_temp1285.curr);}));}} if( es != 0){ return Cyc_Tcexp_expr_err( te,(( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1288= es; if( _temp1288 == 0){ _throw( Null_Exception);}
_temp1288->hd;}))->loc,({ struct _tagged_string _temp1289= Cyc_Absynpp_qvar2string(
ef->name); xprintf("too many arguments for tunion constructor %.*s", _temp1289.last_plus_one
- _temp1289.curr, _temp1289.curr);}));} if( ts != 0){ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp1290= Cyc_Absynpp_qvar2string( ef->name);
xprintf("too few arguments for tunion constructor %.*s", _temp1290.last_plus_one
- _temp1290.curr, _temp1290.curr);}));} return res;}}} static void* Cyc_Tcexp_tcXTunion(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Core_Opt** exist_ref, struct Cyc_List_List* es, struct Cyc_Absyn_XTuniondecl*
xd, struct Cyc_Absyn_Tunionfield* ef){ struct Cyc_List_List* exist_inst=((
struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_Absyn_Tvar*), struct
Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_make_inst_var, ef->tvs); struct Cyc_List_List*
exist_typs=(( struct Cyc_List_List*(*)( void*(* f)( struct _tuple4*), struct Cyc_List_List*
x)) Cyc_List_map)(( void*(*)( struct _tuple4*)) Cyc_Core_snd, exist_inst);
struct Cyc_Absyn_XTunionFieldType_struct* res=({ struct Cyc_Absyn_XTunionFieldType_struct*
_temp1308=( struct Cyc_Absyn_XTunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_XTunionFieldType_struct)); _temp1308[ 0]=({ struct Cyc_Absyn_XTunionFieldType_struct
_temp1309; _temp1309.tag= Cyc_Absyn_XTunionFieldType_tag; _temp1309.f1=({ struct
Cyc_Absyn_XTunionFieldInfo _temp1310; _temp1310.name= xd->name; _temp1310.fname=
ef->name; _temp1310.xtud=( struct Cyc_Absyn_XTuniondecl*) xd; _temp1310.xtufd=(
struct Cyc_Absyn_Tunionfield*) ef; _temp1310;}); _temp1309;}); _temp1308;});
struct Cyc_List_List* ts= ef->typs;* exist_ref=({ struct Cyc_Core_Opt* _temp1295=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1295->v=(
void*) exist_typs; _temp1295;}); while( es != 0? ts != 0: 0) { struct Cyc_Absyn_Exp*
e=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1303= es; if( _temp1303
== 0){ _throw( Null_Exception);} _temp1303->hd;}); void* t= Cyc_Tcutil_substitute(
exist_inst,(*(( struct _tuple7*)({ struct Cyc_List_List* _temp1302= ts; if(
_temp1302 == 0){ _throw( Null_Exception);} _temp1302->hd;}))).f2); Cyc_Tcexp_tcExpInitializer(
te,( void**)& t, e); es=({ struct Cyc_List_List* _temp1296= es; if( _temp1296 ==
0){ _throw( Null_Exception);} _temp1296->tl;}); ts=({ struct Cyc_List_List*
_temp1297= ts; if( _temp1297 == 0){ _throw( Null_Exception);} _temp1297->tl;});
if( ! Cyc_Tcutil_coerce_arg( te, e, t)){ Cyc_Tcutil_terr( e->loc,({ struct
_tagged_string _temp1299= Cyc_Absynpp_qvar2string( ef->name); struct
_tagged_string _temp1300= Cyc_Absynpp_typ2string( t); struct _tagged_string
_temp1301= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt* _temp1298= e->topt;
if( _temp1298 == 0){ _throw( Null_Exception);} _temp1298->v;})); xprintf("xtunion constructor %.*s expects argument of type %.*s but this argument has type %.*s",
_temp1299.last_plus_one - _temp1299.curr, _temp1299.curr, _temp1300.last_plus_one
- _temp1300.curr, _temp1300.curr, _temp1301.last_plus_one - _temp1301.curr,
_temp1301.curr);}));}} if( es != 0){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1304= es; if( _temp1304 == 0){ _throw( Null_Exception);}
_temp1304->hd;}))->loc,({ struct _tagged_string _temp1305= Cyc_Absynpp_qvar2string(
ef->name); xprintf("too many arguments for xtunion constructor %.*s", _temp1305.last_plus_one
- _temp1305.curr, _temp1305.curr);}));} else{ if( ts != 0){ Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1306= es; if( _temp1306 == 0){
_throw( Null_Exception);} _temp1306->hd;}))->loc,({ struct _tagged_string
_temp1307= Cyc_Absynpp_qvar2string( ef->name); xprintf("too few arguments for xtunion constructor %.*s",
_temp1307.last_plus_one - _temp1307.curr, _temp1307.curr);}));}} return( void*)
res;} static void* Cyc_Tcexp_tcMalloc( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* ropt, void* t){ void* rgn=( void*) Cyc_Absyn_HeapRgn;
if( ropt != 0){ void* expected_type=( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp1322=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp1322[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp1323; _temp1323.tag= Cyc_Absyn_RgnHandleType_tag; _temp1323.f1=( void*) Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind); _temp1323;}); _temp1322;}); void* handle_type= Cyc_Tcexp_tcExp(
te,( void**)& expected_type,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1321= ropt; if( _temp1321 == 0){ _throw( Null_Exception);} _temp1321;}));
void* _temp1311= Cyc_Tcutil_compress( handle_type); void* _temp1317; _LL1313:
if(( unsigned int) _temp1311 > 4u?(( struct _tunion_struct*) _temp1311)->tag ==
Cyc_Absyn_RgnHandleType_tag: 0){ _LL1318: _temp1317=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1311)->f1; goto _LL1314;} else{ goto _LL1315;} _LL1315: goto _LL1316;
_LL1314: rgn= _temp1317; Cyc_Tcenv_check_rgn_accessible( te, loc, rgn); goto
_LL1312; _LL1316: Cyc_Tcutil_terr(({ struct Cyc_Absyn_Exp* _temp1319= ropt; if(
_temp1319 == 0){ _throw( Null_Exception);} _temp1319->loc;}),({ struct
_tagged_string _temp1320= Cyc_Absynpp_typ2string( handle_type); xprintf("expecting region_t type but found %.*s",
_temp1320.last_plus_one - _temp1320.curr, _temp1320.curr);})); goto _LL1312;
_LL1312:;} Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars( te),(
void*) Cyc_Absyn_MemKind, t);{ void* _temp1324= t; struct Cyc_Absyn_TunionFieldInfo
_temp1332; struct Cyc_Absyn_Tunionfield* _temp1334; struct Cyc_Absyn_Tuniondecl*
_temp1336; struct _tuple1* _temp1338; struct Cyc_List_List* _temp1340; struct
_tuple1* _temp1342; struct Cyc_Absyn_XTunionFieldInfo _temp1344; struct Cyc_Absyn_Tunionfield*
_temp1346; struct Cyc_Absyn_XTuniondecl* _temp1348; struct _tuple1* _temp1350;
struct _tuple1* _temp1352; _LL1326: if(( unsigned int) _temp1324 > 4u?(( struct
_tunion_struct*) _temp1324)->tag == Cyc_Absyn_TunionFieldType_tag: 0){ _LL1333:
_temp1332=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp1324)->f1; _LL1343: _temp1342=( struct _tuple1*) _temp1332.name; goto
_LL1341; _LL1341: _temp1340=( struct Cyc_List_List*) _temp1332.targs; goto
_LL1339; _LL1339: _temp1338=( struct _tuple1*) _temp1332.fname; goto _LL1337;
_LL1337: _temp1336=( struct Cyc_Absyn_Tuniondecl*) _temp1332.tud; goto _LL1335;
_LL1335: _temp1334=( struct Cyc_Absyn_Tunionfield*) _temp1332.tufd; goto _LL1327;}
else{ goto _LL1328;} _LL1328: if(( unsigned int) _temp1324 > 4u?(( struct
_tunion_struct*) _temp1324)->tag == Cyc_Absyn_XTunionFieldType_tag: 0){ _LL1345:
_temp1344=( struct Cyc_Absyn_XTunionFieldInfo)(( struct Cyc_Absyn_XTunionFieldType_struct*)
_temp1324)->f1; _LL1353: _temp1352=( struct _tuple1*) _temp1344.name; goto
_LL1351; _LL1351: _temp1350=( struct _tuple1*) _temp1344.fname; goto _LL1349;
_LL1349: _temp1348=( struct Cyc_Absyn_XTuniondecl*) _temp1344.xtud; goto _LL1347;
_LL1347: _temp1346=( struct Cyc_Absyn_Tunionfield*) _temp1344.xtufd; goto
_LL1329;} else{ goto _LL1330;} _LL1330: goto _LL1331; _LL1327: _temp1346=
_temp1334; goto _LL1329; _LL1329: if(({ struct Cyc_Absyn_Tunionfield* _temp1354=
_temp1346; if( _temp1354 == 0){ _throw( Null_Exception);} _temp1354->tvs;}) != 0){
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp1355=( char*)"malloc with existential types not yet implemented";
struct _tagged_string _temp1356; _temp1356.curr= _temp1355; _temp1356.base=
_temp1355; _temp1356.last_plus_one= _temp1355 + 50; _temp1356;}));} goto _LL1325;
_LL1331: goto _LL1325; _LL1325:;}{ void*(* ptr_maker)( void* t, void* rgn,
struct Cyc_Absyn_Tqual* tq)= Cyc_Absyn_at_typ; if( topt != 0){ void* _temp1358=
Cyc_Tcutil_compress(({ void** _temp1357= topt; if( _temp1357 == 0){ _throw(
Null_Exception);}* _temp1357;})); struct Cyc_Absyn_PtrInfo _temp1364; struct Cyc_Absyn_Conref*
_temp1366; struct Cyc_Absyn_Tqual* _temp1368; struct Cyc_Absyn_Conref* _temp1370;
void* _temp1372; void* _temp1374; _LL1360: if(( unsigned int) _temp1358 > 4u?((
struct _tunion_struct*) _temp1358)->tag == Cyc_Absyn_PointerType_tag: 0){
_LL1365: _temp1364=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1358)->f1; _LL1375: _temp1374=( void*) _temp1364.elt_typ; goto _LL1373;
_LL1373: _temp1372=( void*) _temp1364.rgn_typ; goto _LL1371; _LL1371: _temp1370=(
struct Cyc_Absyn_Conref*) _temp1364.nullable; goto _LL1369; _LL1369: _temp1368=(
struct Cyc_Absyn_Tqual*) _temp1364.tq; goto _LL1367; _LL1367: _temp1366=( struct
Cyc_Absyn_Conref*) _temp1364.bounds; goto _LL1361;} else{ goto _LL1362;} _LL1362:
goto _LL1363; _LL1361: if((( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1370)){ ptr_maker= Cyc_Absyn_star_typ;} goto _LL1359; _LL1363: goto _LL1359;
_LL1359:;} return ptr_maker( t, rgn, Cyc_Absyn_empty_tqual());}} static void*
Cyc_Tcexp_tcStmtExp( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, struct Cyc_Absyn_Stmt* s){ Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser(
te, s), s, 1); Cyc_NewControlFlow_set_encloser( s, Cyc_Tcenv_get_encloser( te));
while( 1) { void* _temp1376=( void*) s->r; struct Cyc_Absyn_Exp* _temp1386;
struct Cyc_Absyn_Stmt* _temp1388; struct Cyc_Absyn_Stmt* _temp1390; struct Cyc_Absyn_Stmt*
_temp1392; struct Cyc_Absyn_Decl* _temp1394; _LL1378: if(( unsigned int)
_temp1376 > 1u?(( struct _tunion_struct*) _temp1376)->tag == Cyc_Absyn_Exp_s_tag:
0){ _LL1387: _temp1386=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp1376)->f1; goto _LL1379;} else{ goto _LL1380;} _LL1380: if(( unsigned int)
_temp1376 > 1u?(( struct _tunion_struct*) _temp1376)->tag == Cyc_Absyn_Seq_s_tag:
0){ _LL1391: _temp1390=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp1376)->f1; goto _LL1389; _LL1389: _temp1388=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp1376)->f2; goto _LL1381;} else{ goto
_LL1382;} _LL1382: if(( unsigned int) _temp1376 > 1u?(( struct _tunion_struct*)
_temp1376)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL1395: _temp1394=( struct Cyc_Absyn_Decl*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1376)->f1; goto _LL1393; _LL1393:
_temp1392=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1376)->f2;
goto _LL1383;} else{ goto _LL1384;} _LL1384: goto _LL1385; _LL1379: return( void*)({
struct Cyc_Core_Opt* _temp1396= _temp1386->topt; if( _temp1396 == 0){ _throw(
Null_Exception);} _temp1396->v;}); _LL1381: s= _temp1388; continue; _LL1383: s=
_temp1392; continue; _LL1385: return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_string)({ char* _temp1397=( char*)"statement expression must end with expression";
struct _tagged_string _temp1398; _temp1398.curr= _temp1397; _temp1398.base=
_temp1397; _temp1398.last_plus_one= _temp1397 + 46; _temp1398;})); _LL1377:;}}
static void* Cyc_Tcexp_tcCodegen( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Fndecl* fd){ return Cyc_Tcexp_expr_err( te,
loc,( struct _tagged_string)({ char* _temp1399=( char*)"tcCodegen"; struct
_tagged_string _temp1400; _temp1400.curr= _temp1399; _temp1400.base= _temp1399;
_temp1400.last_plus_one= _temp1399 + 10; _temp1400;}));} static void* Cyc_Tcexp_tcFill(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e){ return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({
char* _temp1401=( char*)"tcFill"; struct _tagged_string _temp1402; _temp1402.curr=
_temp1401; _temp1402.base= _temp1401; _temp1402.last_plus_one= _temp1401 + 7;
_temp1402;}));} static void* Cyc_Tcexp_tcNew( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* rgn_handle, struct
Cyc_Absyn_Exp* e, struct Cyc_Absyn_Exp* e1){ void* rgn=( void*) Cyc_Absyn_HeapRgn;
if( rgn_handle != 0){ void* expected_type=( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp1414=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp1414[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp1415; _temp1415.tag= Cyc_Absyn_RgnHandleType_tag; _temp1415.f1=( void*) Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind); _temp1415;}); _temp1414;}); void* handle_type= Cyc_Tcexp_tcExp(
te,( void**)& expected_type,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1413= rgn_handle; if( _temp1413 == 0){ _throw( Null_Exception);} _temp1413;}));
void* _temp1403= Cyc_Tcutil_compress( handle_type); void* _temp1409; _LL1405:
if(( unsigned int) _temp1403 > 4u?(( struct _tunion_struct*) _temp1403)->tag ==
Cyc_Absyn_RgnHandleType_tag: 0){ _LL1410: _temp1409=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1403)->f1; goto _LL1406;} else{ goto _LL1407;} _LL1407: goto _LL1408;
_LL1406: rgn= _temp1409; Cyc_Tcenv_check_rgn_accessible( te, loc, rgn); goto
_LL1404; _LL1408: Cyc_Tcutil_terr(({ struct Cyc_Absyn_Exp* _temp1411= rgn_handle;
if( _temp1411 == 0){ _throw( Null_Exception);} _temp1411->loc;}),({ struct
_tagged_string _temp1412= Cyc_Absynpp_typ2string( handle_type); xprintf("expecting region_t type but found %.*s",
_temp1412.last_plus_one - _temp1412.curr, _temp1412.curr);})); goto _LL1404;
_LL1404:;}{ void* _temp1416=( void*) e1->r; struct Cyc_Absyn_Exp* _temp1428;
struct Cyc_Absyn_Exp* _temp1430; struct Cyc_Absyn_Vardecl* _temp1432; struct Cyc_List_List*
_temp1434; struct Cyc_Core_Opt* _temp1436; struct Cyc_List_List* _temp1438; void*
_temp1440; struct _tagged_string _temp1442; _LL1418: if((( struct _tunion_struct*)
_temp1416)->tag == Cyc_Absyn_Comprehension_e_tag){ _LL1433: _temp1432=( struct
Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1416)->f1;
goto _LL1431; _LL1431: _temp1430=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1416)->f2; goto _LL1429; _LL1429: _temp1428=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1416)->f3; goto _LL1419;} else{
goto _LL1420;} _LL1420: if((( struct _tunion_struct*) _temp1416)->tag == Cyc_Absyn_UnresolvedMem_e_tag){
_LL1437: _temp1436=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1416)->f1; goto _LL1435; _LL1435: _temp1434=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp1416)->f2; goto _LL1421;} else{
goto _LL1422;} _LL1422: if((( struct _tunion_struct*) _temp1416)->tag == Cyc_Absyn_Array_e_tag){
_LL1439: _temp1438=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp1416)->f1; goto _LL1423;} else{ goto _LL1424;} _LL1424: if((( struct
_tunion_struct*) _temp1416)->tag == Cyc_Absyn_Const_e_tag){ _LL1441: _temp1440=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1416)->f1; if(( unsigned int)
_temp1440 > 1u?(( struct _tunion_struct*) _temp1440)->tag == Cyc_Absyn_String_c_tag:
0){ _LL1443: _temp1442=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp1440)->f1; goto _LL1425;} else{ goto _LL1426;}} else{ goto _LL1426;}
_LL1426: goto _LL1427; _LL1419: { void* t_arr= Cyc_Tcexp_tcExpNoPromote( te,
topt, e1); void* _temp1444= Cyc_Tcutil_compress( t_arr); struct Cyc_Absyn_Exp*
_temp1450; struct Cyc_Absyn_Tqual* _temp1452; void* _temp1454; _LL1446: if((
unsigned int) _temp1444 > 4u?(( struct _tunion_struct*) _temp1444)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL1455: _temp1454=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1444)->f1;
goto _LL1453; _LL1453: _temp1452=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1444)->f2; goto _LL1451; _LL1451: _temp1450=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1444)->f3; goto _LL1447;} else{ goto
_LL1448;} _LL1448: goto _LL1449; _LL1447: { void* b= Cyc_Tcutil_is_const_exp( te,
_temp1430)?( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1464=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1464[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1465; _temp1465.tag= Cyc_Absyn_Upper_b_tag;
_temp1465.f1= _temp1430; _temp1465;}); _temp1464;}):( void*) Cyc_Absyn_Unknown_b;
void* res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1461=( struct
Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1461[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1462; _temp1462.tag=
Cyc_Absyn_PointerType_tag; _temp1462.f1=({ struct Cyc_Absyn_PtrInfo _temp1463;
_temp1463.elt_typ=( void*) _temp1454; _temp1463.rgn_typ=( void*) rgn; _temp1463.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp1463.tq=
_temp1452; _temp1463.bounds=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
b); _temp1463;}); _temp1462;}); _temp1461;}); if( topt != 0){ if( ! Cyc_Tcutil_unify(({
void** _temp1456= topt; if( _temp1456 == 0){ _throw( Null_Exception);}*
_temp1456;}), res_typ)? Cyc_Tcutil_silent_castable( te, loc, res_typ,({ void**
_temp1457= topt; if( _temp1457 == 0){ _throw( Null_Exception);}* _temp1457;})):
0){ e->topt=({ struct Cyc_Core_Opt* _temp1458=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1458->v=( void*) res_typ; _temp1458;}); Cyc_Tcutil_unchecked_cast(
te, e,({ void** _temp1459= topt; if( _temp1459 == 0){ _throw( Null_Exception);}*
_temp1459;})); res_typ=({ void** _temp1460= topt; if( _temp1460 == 0){ _throw(
Null_Exception);}* _temp1460;});}} return res_typ;} _LL1449: return(( void*(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp1466=( char*)"tcNew: comprehension returned non-array type"; struct
_tagged_string _temp1467; _temp1467.curr= _temp1466; _temp1467.base= _temp1466;
_temp1467.last_plus_one= _temp1466 + 45; _temp1467;})); _LL1445:;} _LL1421:(
void*)( e1->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct* _temp1468=(
struct Cyc_Absyn_Array_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp1468[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp1469; _temp1469.tag= Cyc_Absyn_Array_e_tag;
_temp1469.f1= _temp1434; _temp1469;}); _temp1468;}))); _temp1438= _temp1434;
goto _LL1423; _LL1423: { void** elt_typ_opt= 0; if( topt != 0){ void* _temp1471=
Cyc_Tcutil_compress(({ void** _temp1470= topt; if( _temp1470 == 0){ _throw(
Null_Exception);}* _temp1470;})); struct Cyc_Absyn_PtrInfo _temp1477; struct Cyc_Absyn_Conref*
_temp1479; struct Cyc_Absyn_Tqual* _temp1481; struct Cyc_Absyn_Conref* _temp1483;
void* _temp1485; void* _temp1487; void** _temp1489; _LL1473: if(( unsigned int)
_temp1471 > 4u?(( struct _tunion_struct*) _temp1471)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL1478: _temp1477=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1471)->f1; _LL1488: _temp1487=( void*) _temp1477.elt_typ; _temp1489=&(((
struct Cyc_Absyn_PointerType_struct*) _temp1471)->f1).elt_typ; goto _LL1486;
_LL1486: _temp1485=( void*) _temp1477.rgn_typ; goto _LL1484; _LL1484: _temp1483=(
struct Cyc_Absyn_Conref*) _temp1477.nullable; goto _LL1482; _LL1482: _temp1481=(
struct Cyc_Absyn_Tqual*) _temp1477.tq; goto _LL1480; _LL1480: _temp1479=( struct
Cyc_Absyn_Conref*) _temp1477.bounds; goto _LL1474;} else{ goto _LL1475;} _LL1475:
goto _LL1476; _LL1474: elt_typ_opt=( void**) _temp1489; goto _LL1472; _LL1476:
goto _LL1472; _LL1472:;}{ void* t_array= Cyc_Tcexp_tcExpNoPromote( te,
elt_typ_opt, e1); void* res_typ;{ void* _temp1490= Cyc_Tcutil_compress( t_array);
struct Cyc_Absyn_Exp* _temp1496; struct Cyc_Absyn_Tqual* _temp1498; void*
_temp1500; _LL1492: if(( unsigned int) _temp1490 > 4u?(( struct _tunion_struct*)
_temp1490)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL1501: _temp1500=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1490)->f1; goto _LL1499; _LL1499:
_temp1498=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1490)->f2; goto _LL1497; _LL1497: _temp1496=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1490)->f3; goto _LL1493;} else{ goto
_LL1494;} _LL1494: goto _LL1495; _LL1493: res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp1502=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1502[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1503; _temp1503.tag=
Cyc_Absyn_PointerType_tag; _temp1503.f1=({ struct Cyc_Absyn_PtrInfo _temp1504;
_temp1504.elt_typ=( void*) _temp1500; _temp1504.rgn_typ=( void*) rgn; _temp1504.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp1504.tq=
_temp1498; _temp1504.bounds=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1505=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1505[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1506; _temp1506.tag= Cyc_Absyn_Upper_b_tag;
_temp1506.f1=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1507=
_temp1496; if( _temp1507 == 0){ _throw( Null_Exception);} _temp1507;});
_temp1506;}); _temp1505;})); _temp1504;}); _temp1503;}); _temp1502;}); if( topt
!= 0){ if( ! Cyc_Tcutil_unify(({ void** _temp1508= topt; if( _temp1508 == 0){
_throw( Null_Exception);}* _temp1508;}), res_typ)? Cyc_Tcutil_silent_castable(
te, loc, res_typ,({ void** _temp1509= topt; if( _temp1509 == 0){ _throw(
Null_Exception);}* _temp1509;})): 0){ e->topt=({ struct Cyc_Core_Opt* _temp1510=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1510->v=(
void*) res_typ; _temp1510;}); Cyc_Tcutil_unchecked_cast( te, e,({ void**
_temp1511= topt; if( _temp1511 == 0){ _throw( Null_Exception);}* _temp1511;}));
res_typ=({ void** _temp1512= topt; if( _temp1512 == 0){ _throw( Null_Exception);}*
_temp1512;});}} goto _LL1491; _LL1495: return(( void*(*)( struct _tagged_string))
Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp1513=( char*)"tcExpNoPromote on Array_e returned non-array type";
struct _tagged_string _temp1514; _temp1514.curr= _temp1513; _temp1514.base=
_temp1513; _temp1514.last_plus_one= _temp1513 + 50; _temp1514;})); _LL1491:;}
return res_typ;}} _LL1425: { void* topt2= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t,
rgn, Cyc_Absyn_empty_tqual(),( void*) Cyc_Absyn_Unknown_b); void* t= Cyc_Tcexp_tcExp(
te,( void**)& topt2, e1); return Cyc_Absyn_atb_typ( t, rgn, Cyc_Absyn_empty_tqual(),(
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1515=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1515[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1516; _temp1516.tag= Cyc_Absyn_Upper_b_tag;
_temp1516.f1= Cyc_Absyn_uint_exp( 1, 0); _temp1516;}); _temp1515;}));} _LL1427: {
void** topt2= 0; if( topt != 0){ void* _temp1518= Cyc_Tcutil_compress(({ void**
_temp1517= topt; if( _temp1517 == 0){ _throw( Null_Exception);}* _temp1517;}));
struct Cyc_Absyn_PtrInfo _temp1526; struct Cyc_Absyn_Conref* _temp1528; struct
Cyc_Absyn_Tqual* _temp1530; struct Cyc_Absyn_Conref* _temp1532; void* _temp1534;
void* _temp1536; void** _temp1538; struct Cyc_Absyn_TunionInfo _temp1539;
_LL1520: if(( unsigned int) _temp1518 > 4u?(( struct _tunion_struct*) _temp1518)->tag
== Cyc_Absyn_PointerType_tag: 0){ _LL1527: _temp1526=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1518)->f1; _LL1537: _temp1536=( void*)
_temp1526.elt_typ; _temp1538=&((( struct Cyc_Absyn_PointerType_struct*)
_temp1518)->f1).elt_typ; goto _LL1535; _LL1535: _temp1534=( void*) _temp1526.rgn_typ;
goto _LL1533; _LL1533: _temp1532=( struct Cyc_Absyn_Conref*) _temp1526.nullable;
goto _LL1531; _LL1531: _temp1530=( struct Cyc_Absyn_Tqual*) _temp1526.tq; goto
_LL1529; _LL1529: _temp1528=( struct Cyc_Absyn_Conref*) _temp1526.bounds; goto
_LL1521;} else{ goto _LL1522;} _LL1522: if(( unsigned int) _temp1518 > 4u?((
struct _tunion_struct*) _temp1518)->tag == Cyc_Absyn_TunionType_tag: 0){ _LL1540:
_temp1539=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp1518)->f1; goto _LL1523;} else{ goto _LL1524;} _LL1524: goto _LL1525;
_LL1521: topt2=( void**) _temp1538; goto _LL1519; _LL1523: topt2=({ void**
_temp1541=( void**) GC_malloc( sizeof( void*)); _temp1541[ 0]=({ void**
_temp1542= topt; if( _temp1542 == 0){ _throw( Null_Exception);}* _temp1542;});
_temp1541;}); goto _LL1519; _LL1525: goto _LL1519; _LL1519:;}{ void* telt= Cyc_Tcexp_tcExp(
te, topt2, e1); void* res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp1548=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1548[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1549; _temp1549.tag=
Cyc_Absyn_PointerType_tag; _temp1549.f1=({ struct Cyc_Absyn_PtrInfo _temp1550;
_temp1550.elt_typ=( void*) telt; _temp1550.rgn_typ=( void*) rgn; _temp1550.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp1550.tq= Cyc_Absyn_empty_tqual();
_temp1550.bounds=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1551=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1551[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1552; _temp1552.tag= Cyc_Absyn_Upper_b_tag;
_temp1552.f1= Cyc_Absyn_uint_exp( 1, 0); _temp1552;}); _temp1551;})); _temp1550;});
_temp1549;}); _temp1548;}); if( topt != 0){ if( ! Cyc_Tcutil_unify(({ void**
_temp1543= topt; if( _temp1543 == 0){ _throw( Null_Exception);}* _temp1543;}),
res_typ)? Cyc_Tcutil_silent_castable( te, loc, res_typ,({ void** _temp1544= topt;
if( _temp1544 == 0){ _throw( Null_Exception);}* _temp1544;})): 0){ e->topt=({
struct Cyc_Core_Opt* _temp1545=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1545->v=( void*) res_typ; _temp1545;}); Cyc_Tcutil_unchecked_cast(
te, e,({ void** _temp1546= topt; if( _temp1546 == 0){ _throw( Null_Exception);}*
_temp1546;})); res_typ=({ void** _temp1547= topt; if( _temp1547 == 0){ _throw(
Null_Exception);}* _temp1547;});}} return res_typ;}} _LL1417:;}} void* Cyc_Tcexp_tcExp(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcexp_tcExpNoPromote(
te, topt, e); t= Cyc_Tcutil_compress( t);{ void* _temp1553= t; struct Cyc_Absyn_Exp*
_temp1559; struct Cyc_Absyn_Tqual* _temp1561; void* _temp1563; _LL1555: if((
unsigned int) _temp1553 > 4u?(( struct _tunion_struct*) _temp1553)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL1564: _temp1563=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1553)->f1;
goto _LL1562; _LL1562: _temp1561=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1553)->f2; goto _LL1560; _LL1560: _temp1559=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1553)->f3; goto _LL1556;} else{ goto
_LL1557;} _LL1557: goto _LL1558; _LL1556: { void* _temp1567; int _temp1569;
struct _tuple5 _temp1565= Cyc_Tcutil_addressof_props( te, e); _LL1570: _temp1569=
_temp1565.f1; goto _LL1568; _LL1568: _temp1567= _temp1565.f2; goto _LL1566;
_LL1566: { void* b= _temp1559 == 0?( void*) Cyc_Absyn_Unknown_b:( void*)({
struct Cyc_Absyn_Upper_b_struct* _temp1571=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1571[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1572; _temp1572.tag= Cyc_Absyn_Upper_b_tag;
_temp1572.f1=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1573=
_temp1559; if( _temp1573 == 0){ _throw( Null_Exception);} _temp1573;});
_temp1572;}); _temp1571;}); t= Cyc_Absyn_atb_typ( _temp1563, _temp1567,
_temp1561, b); Cyc_Tcutil_unchecked_cast( te, e, t); return t;}} _LL1558: return
t; _LL1554:;}} void* Cyc_Tcexp_tcExpInitializer( struct Cyc_Tcenv_Tenv* te, void**
topt, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcexp_tcExpNoPromote( te, topt, e);
void* _temp1574=( void*) e->r; struct Cyc_List_List* _temp1584; struct Cyc_Absyn_Exp*
_temp1586; struct Cyc_Absyn_Exp* _temp1588; struct Cyc_Absyn_Vardecl* _temp1590;
void* _temp1592; struct _tagged_string _temp1594; _LL1576: if((( struct
_tunion_struct*) _temp1574)->tag == Cyc_Absyn_Array_e_tag){ _LL1585: _temp1584=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1574)->f1; goto
_LL1577;} else{ goto _LL1578;} _LL1578: if((( struct _tunion_struct*) _temp1574)->tag
== Cyc_Absyn_Comprehension_e_tag){ _LL1591: _temp1590=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1574)->f1; goto _LL1589; _LL1589:
_temp1588=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1574)->f2; goto _LL1587; _LL1587: _temp1586=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1574)->f3; goto _LL1579;} else{
goto _LL1580;} _LL1580: if((( struct _tunion_struct*) _temp1574)->tag == Cyc_Absyn_Const_e_tag){
_LL1593: _temp1592=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1574)->f1;
if(( unsigned int) _temp1592 > 1u?(( struct _tunion_struct*) _temp1592)->tag ==
Cyc_Absyn_String_c_tag: 0){ _LL1595: _temp1594=( struct _tagged_string)(( struct
Cyc_Absyn_String_c_struct*) _temp1592)->f1; goto _LL1581;} else{ goto _LL1582;}}
else{ goto _LL1582;} _LL1582: goto _LL1583; _LL1577: return t; _LL1579: return t;
_LL1581: return t; _LL1583: t= Cyc_Tcutil_compress( t);{ void* _temp1596= t;
struct Cyc_Absyn_Exp* _temp1602; struct Cyc_Absyn_Tqual* _temp1604; void*
_temp1606; _LL1598: if(( unsigned int) _temp1596 > 4u?(( struct _tunion_struct*)
_temp1596)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL1607: _temp1606=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1596)->f1; goto _LL1605; _LL1605:
_temp1604=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1596)->f2; goto _LL1603; _LL1603: _temp1602=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1596)->f3; goto _LL1599;} else{ goto
_LL1600;} _LL1600: goto _LL1601; _LL1599: { void* _temp1610; int _temp1612;
struct _tuple5 _temp1608= Cyc_Tcutil_addressof_props( te, e); _LL1613: _temp1612=
_temp1608.f1; goto _LL1611; _LL1611: _temp1610= _temp1608.f2; goto _LL1609;
_LL1609: { void* b= _temp1602 == 0?( void*) Cyc_Absyn_Unknown_b:( void*)({
struct Cyc_Absyn_Upper_b_struct* _temp1614=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1614[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1615; _temp1615.tag= Cyc_Absyn_Upper_b_tag;
_temp1615.f1=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1616=
_temp1602; if( _temp1616 == 0){ _throw( Null_Exception);} _temp1616;});
_temp1615;}); _temp1614;}); t= Cyc_Absyn_atb_typ( _temp1606, _temp1610,
_temp1604, b); Cyc_Tcutil_unchecked_cast( te, e, t); return t;}} _LL1601: return
t; _LL1597:;} _LL1575:;} static void* Cyc_Tcexp_tcExpNoPromote( struct Cyc_Tcenv_Tenv*
te, void** topt, struct Cyc_Absyn_Exp* e){{ void* _temp1617=( void*) e->r;
struct Cyc_Absyn_Exp* _temp1623; _LL1619: if((( struct _tunion_struct*)
_temp1617)->tag == Cyc_Absyn_NoInstantiate_e_tag){ _LL1624: _temp1623=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*) _temp1617)->f1; goto
_LL1620;} else{ goto _LL1621;} _LL1621: goto _LL1622; _LL1620: Cyc_Tcexp_tcExpNoInst(
te, topt, _temp1623);( void*)({ struct Cyc_Core_Opt* _temp1625= _temp1623->topt;
if( _temp1625 == 0){ _throw( Null_Exception);} _temp1625->v=( void*) Cyc_Absyn_pointer_expand(
Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp1626= _temp1623->topt;
if( _temp1626 == 0){ _throw( Null_Exception);} _temp1626->v;})));}); e->topt=
_temp1623->topt; goto _LL1618; _LL1622: Cyc_Tcexp_tcExpNoInst( te, topt, e);(
void*)({ struct Cyc_Core_Opt* _temp1627= e->topt; if( _temp1627 == 0){ _throw(
Null_Exception);} _temp1627->v=( void*) Cyc_Absyn_pointer_expand( Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp1628= e->topt; if( _temp1628 == 0){ _throw(
Null_Exception);} _temp1628->v;})));});{ void* _temp1630= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp1629= e->topt; if( _temp1629 == 0){ _throw(
Null_Exception);} _temp1629->v;})); struct Cyc_Absyn_PtrInfo _temp1636; struct
Cyc_Absyn_Conref* _temp1638; struct Cyc_Absyn_Tqual* _temp1640; struct Cyc_Absyn_Conref*
_temp1642; void* _temp1644; void* _temp1646; _LL1632: if(( unsigned int)
_temp1630 > 4u?(( struct _tunion_struct*) _temp1630)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL1637: _temp1636=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1630)->f1; _LL1647: _temp1646=( void*) _temp1636.elt_typ; goto _LL1645;
_LL1645: _temp1644=( void*) _temp1636.rgn_typ; goto _LL1643; _LL1643: _temp1642=(
struct Cyc_Absyn_Conref*) _temp1636.nullable; goto _LL1641; _LL1641: _temp1640=(
struct Cyc_Absyn_Tqual*) _temp1636.tq; goto _LL1639; _LL1639: _temp1638=( struct
Cyc_Absyn_Conref*) _temp1636.bounds; goto _LL1633;} else{ goto _LL1634;} _LL1634:
goto _LL1635; _LL1633:{ void* _temp1648= Cyc_Tcutil_compress( _temp1646); struct
Cyc_Absyn_FnInfo _temp1654; struct Cyc_List_List* _temp1656; int _temp1658;
struct Cyc_List_List* _temp1660; void* _temp1662; struct Cyc_Core_Opt* _temp1664;
struct Cyc_List_List* _temp1666; _LL1650: if(( unsigned int) _temp1648 > 4u?((
struct _tunion_struct*) _temp1648)->tag == Cyc_Absyn_FnType_tag: 0){ _LL1655:
_temp1654=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp1648)->f1; _LL1667: _temp1666=( struct Cyc_List_List*) _temp1654.tvars;
goto _LL1665; _LL1665: _temp1664=( struct Cyc_Core_Opt*) _temp1654.effect; goto
_LL1663; _LL1663: _temp1662=( void*) _temp1654.ret_typ; goto _LL1661; _LL1661:
_temp1660=( struct Cyc_List_List*) _temp1654.args; goto _LL1659; _LL1659:
_temp1658=( int) _temp1654.varargs; goto _LL1657; _LL1657: _temp1656=( struct
Cyc_List_List*) _temp1654.attributes; goto _LL1651;} else{ goto _LL1652;}
_LL1652: goto _LL1653; _LL1651: if( _temp1666 != 0){ struct Cyc_List_List* inst=((
struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_Absyn_Tvar*), struct
Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_make_inst_var, _temp1666); struct
Cyc_List_List* ts=(( struct Cyc_List_List*(*)( void*(* f)( struct _tuple4*),
struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct _tuple4*)) Cyc_Core_snd,
inst); void* ftyp= Cyc_Tcutil_substitute( inst,( void*)({ struct Cyc_Absyn_FnType_struct*
_temp1674=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp1674[ 0]=({ struct Cyc_Absyn_FnType_struct _temp1675; _temp1675.tag= Cyc_Absyn_FnType_tag;
_temp1675.f1=({ struct Cyc_Absyn_FnInfo _temp1676; _temp1676.tvars= 0; _temp1676.effect=
_temp1664; _temp1676.ret_typ=( void*) _temp1662; _temp1676.args= _temp1660;
_temp1676.varargs= _temp1658; _temp1676.attributes= _temp1656; _temp1676;});
_temp1675;}); _temp1674;})); void* new_typ=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp1671=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1671[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1672; _temp1672.tag=
Cyc_Absyn_PointerType_tag; _temp1672.f1=({ struct Cyc_Absyn_PtrInfo _temp1673;
_temp1673.elt_typ=( void*) ftyp; _temp1673.rgn_typ=( void*) _temp1644; _temp1673.nullable=
_temp1642; _temp1673.tq= _temp1640; _temp1673.bounds= _temp1638; _temp1673;});
_temp1672;}); _temp1671;}); struct Cyc_Absyn_Exp* inner= Cyc_Absyn_copy_exp( e);(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Instantiate_e_struct*
_temp1668=( struct Cyc_Absyn_Instantiate_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Instantiate_e_struct)); _temp1668[ 0]=({ struct Cyc_Absyn_Instantiate_e_struct
_temp1669; _temp1669.tag= Cyc_Absyn_Instantiate_e_tag; _temp1669.f1= inner;
_temp1669.f2= ts; _temp1669;}); _temp1668;}))); e->topt=({ struct Cyc_Core_Opt*
_temp1670=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1670->v=( void*) new_typ; _temp1670;});} goto _LL1649; _LL1653: goto
_LL1649; _LL1649:;} goto _LL1631; _LL1635: goto _LL1631; _LL1631:;} goto _LL1618;
_LL1618:;} return( void*)({ struct Cyc_Core_Opt* _temp1677= e->topt; if(
_temp1677 == 0){ _throw( Null_Exception);} _temp1677->v;});} static void Cyc_Tcexp_tcExpNoInst(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){ struct Cyc_Position_Segment*
loc= e->loc; void* t;{ void* _temp1678=( void*) e->r; struct Cyc_Absyn_Exp*
_temp1750; struct _tuple1* _temp1752; struct Cyc_List_List* _temp1754; struct
Cyc_Absyn_Exp* _temp1756; struct Cyc_List_List* _temp1758; struct Cyc_Core_Opt*
_temp1760; void* _temp1762; void* _temp1764; struct _tuple1* _temp1766; struct
Cyc_List_List* _temp1768; void* _temp1770; void* _temp1772; struct Cyc_Absyn_Exp*
_temp1774; struct Cyc_Absyn_Exp* _temp1776; struct Cyc_Core_Opt* _temp1778;
struct Cyc_Absyn_Exp* _temp1780; struct Cyc_Absyn_Exp* _temp1782; struct Cyc_Absyn_Exp*
_temp1784; struct Cyc_Absyn_Exp* _temp1786; struct Cyc_Absyn_Exp* _temp1788;
struct Cyc_Absyn_Exp* _temp1790; struct Cyc_List_List* _temp1792; struct Cyc_Absyn_Exp*
_temp1794; struct Cyc_Absyn_Exp* _temp1796; struct Cyc_List_List* _temp1798;
struct Cyc_Absyn_Exp* _temp1800; struct Cyc_Absyn_Exp* _temp1802; void*
_temp1804; struct Cyc_Absyn_Exp* _temp1806; struct Cyc_Absyn_Exp* _temp1808;
struct Cyc_Absyn_Exp* _temp1810; struct Cyc_Absyn_Exp* _temp1812; void*
_temp1814; struct Cyc_Absyn_Exp* _temp1816; struct _tagged_string* _temp1818;
struct Cyc_Absyn_Exp* _temp1820; struct _tagged_string* _temp1822; struct Cyc_Absyn_Exp*
_temp1824; struct Cyc_Absyn_Exp* _temp1826; struct Cyc_Absyn_Exp* _temp1828;
struct Cyc_List_List* _temp1830; struct Cyc_List_List* _temp1832; struct _tuple2*
_temp1834; struct Cyc_List_List* _temp1836; struct Cyc_Absyn_Stmt* _temp1838;
struct Cyc_Absyn_Fndecl* _temp1840; struct Cyc_Absyn_Exp* _temp1842; struct Cyc_Absyn_Exp*
_temp1844; struct Cyc_Absyn_Exp* _temp1846; struct Cyc_Absyn_Vardecl* _temp1848;
struct Cyc_Absyn_Structdecl* _temp1850; struct Cyc_Absyn_Structdecl** _temp1852;
struct Cyc_List_List* _temp1853; struct Cyc_Core_Opt* _temp1855; struct Cyc_Core_Opt**
_temp1857; struct _tuple1* _temp1858; struct _tuple1** _temp1860; struct Cyc_Absyn_Tunionfield*
_temp1861; struct Cyc_Absyn_Tuniondecl* _temp1863; struct Cyc_List_List*
_temp1865; struct Cyc_Core_Opt* _temp1867; struct Cyc_Core_Opt** _temp1869;
struct Cyc_Core_Opt* _temp1870; struct Cyc_Core_Opt** _temp1872; struct Cyc_Absyn_Tunionfield*
_temp1873; struct Cyc_Absyn_XTuniondecl* _temp1875; struct Cyc_List_List*
_temp1877; struct Cyc_Core_Opt* _temp1879; struct Cyc_Core_Opt** _temp1881;
struct Cyc_Absyn_Enumfield* _temp1882; struct Cyc_Absyn_Enumdecl* _temp1884;
struct _tuple1* _temp1886; void* _temp1888; struct Cyc_Absyn_Exp* _temp1890;
_LL1680: if((( struct _tunion_struct*) _temp1678)->tag == Cyc_Absyn_NoInstantiate_e_tag){
_LL1751: _temp1750=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1678)->f1; goto _LL1681;} else{ goto _LL1682;} _LL1682: if((( struct
_tunion_struct*) _temp1678)->tag == Cyc_Absyn_UnknownId_e_tag){ _LL1753:
_temp1752=( struct _tuple1*)(( struct Cyc_Absyn_UnknownId_e_struct*) _temp1678)->f1;
goto _LL1683;} else{ goto _LL1684;} _LL1684: if((( struct _tunion_struct*)
_temp1678)->tag == Cyc_Absyn_UnknownCall_e_tag){ _LL1757: _temp1756=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_UnknownCall_e_struct*) _temp1678)->f1; goto _LL1755; _LL1755:
_temp1754=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1678)->f2; goto _LL1685;} else{ goto _LL1686;} _LL1686: if((( struct
_tunion_struct*) _temp1678)->tag == Cyc_Absyn_UnresolvedMem_e_tag){ _LL1761:
_temp1760=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1678)->f1; goto _LL1759; _LL1759: _temp1758=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp1678)->f2; goto _LL1687;} else{
goto _LL1688;} _LL1688: if((( struct _tunion_struct*) _temp1678)->tag == Cyc_Absyn_Const_e_tag){
_LL1763: _temp1762=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1678)->f1;
goto _LL1689;} else{ goto _LL1690;} _LL1690: if((( struct _tunion_struct*)
_temp1678)->tag == Cyc_Absyn_Var_e_tag){ _LL1767: _temp1766=( struct _tuple1*)((
struct Cyc_Absyn_Var_e_struct*) _temp1678)->f1; goto _LL1765; _LL1765: _temp1764=(
void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1678)->f2; goto _LL1691;} else{
goto _LL1692;} _LL1692: if((( struct _tunion_struct*) _temp1678)->tag == Cyc_Absyn_Primop_e_tag){
_LL1771: _temp1770=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1678)->f1;
goto _LL1769; _LL1769: _temp1768=( struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp1678)->f2; goto _LL1693;} else{ goto _LL1694;} _LL1694: if((( struct
_tunion_struct*) _temp1678)->tag == Cyc_Absyn_Increment_e_tag){ _LL1775:
_temp1774=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1678)->f1; goto _LL1773; _LL1773: _temp1772=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1678)->f2; goto _LL1695;} else{ goto _LL1696;} _LL1696: if((( struct
_tunion_struct*) _temp1678)->tag == Cyc_Absyn_AssignOp_e_tag){ _LL1781:
_temp1780=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1678)->f1; goto _LL1779; _LL1779: _temp1778=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1678)->f2; goto _LL1777; _LL1777:
_temp1776=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1678)->f3; goto _LL1697;} else{ goto _LL1698;} _LL1698: if((( struct
_tunion_struct*) _temp1678)->tag == Cyc_Absyn_Conditional_e_tag){ _LL1787:
_temp1786=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1678)->f1; goto _LL1785; _LL1785: _temp1784=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp1678)->f2; goto _LL1783; _LL1783:
_temp1782=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1678)->f3; goto _LL1699;} else{ goto _LL1700;} _LL1700: if((( struct
_tunion_struct*) _temp1678)->tag == Cyc_Absyn_SeqExp_e_tag){ _LL1791: _temp1790=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*) _temp1678)->f1; goto
_LL1789; _LL1789: _temp1788=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1678)->f2; goto _LL1701;} else{ goto _LL1702;} _LL1702: if((( struct
_tunion_struct*) _temp1678)->tag == Cyc_Absyn_FnCall_e_tag){ _LL1795: _temp1794=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*) _temp1678)->f1; goto
_LL1793; _LL1793: _temp1792=( struct Cyc_List_List*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1678)->f2; goto _LL1703;} else{ goto _LL1704;} _LL1704: if((( struct
_tunion_struct*) _temp1678)->tag == Cyc_Absyn_Throw_e_tag){ _LL1797: _temp1796=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*) _temp1678)->f1; goto
_LL1705;} else{ goto _LL1706;} _LL1706: if((( struct _tunion_struct*) _temp1678)->tag
== Cyc_Absyn_Instantiate_e_tag){ _LL1801: _temp1800=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Instantiate_e_struct*) _temp1678)->f1; goto _LL1799; _LL1799:
_temp1798=( struct Cyc_List_List*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1678)->f2; goto _LL1707;} else{ goto _LL1708;} _LL1708: if((( struct
_tunion_struct*) _temp1678)->tag == Cyc_Absyn_Cast_e_tag){ _LL1805: _temp1804=(
void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp1678)->f1; goto _LL1803; _LL1803:
_temp1802=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*) _temp1678)->f2;
goto _LL1709;} else{ goto _LL1710;} _LL1710: if((( struct _tunion_struct*)
_temp1678)->tag == Cyc_Absyn_Address_e_tag){ _LL1807: _temp1806=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Address_e_struct*) _temp1678)->f1; goto _LL1711;} else{ goto
_LL1712;} _LL1712: if((( struct _tunion_struct*) _temp1678)->tag == Cyc_Absyn_New_e_tag){
_LL1811: _temp1810=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*)
_temp1678)->f1; goto _LL1809; _LL1809: _temp1808=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_New_e_struct*) _temp1678)->f2; goto _LL1713;} else{ goto
_LL1714;} _LL1714: if((( struct _tunion_struct*) _temp1678)->tag == Cyc_Absyn_Sizeofexp_e_tag){
_LL1813: _temp1812=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1678)->f1; goto _LL1715;} else{ goto _LL1716;} _LL1716: if((( struct
_tunion_struct*) _temp1678)->tag == Cyc_Absyn_Sizeoftyp_e_tag){ _LL1815:
_temp1814=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp1678)->f1; goto
_LL1717;} else{ goto _LL1718;} _LL1718: if((( struct _tunion_struct*) _temp1678)->tag
== Cyc_Absyn_Deref_e_tag){ _LL1817: _temp1816=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Deref_e_struct*) _temp1678)->f1; goto _LL1719;} else{ goto _LL1720;}
_LL1720: if((( struct _tunion_struct*) _temp1678)->tag == Cyc_Absyn_StructMember_e_tag){
_LL1821: _temp1820=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1678)->f1; goto _LL1819; _LL1819: _temp1818=( struct _tagged_string*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp1678)->f2; goto _LL1721;} else{
goto _LL1722;} _LL1722: if((( struct _tunion_struct*) _temp1678)->tag == Cyc_Absyn_StructArrow_e_tag){
_LL1825: _temp1824=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1678)->f1; goto _LL1823; _LL1823: _temp1822=( struct _tagged_string*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp1678)->f2; goto _LL1723;} else{
goto _LL1724;} _LL1724: if((( struct _tunion_struct*) _temp1678)->tag == Cyc_Absyn_Subscript_e_tag){
_LL1829: _temp1828=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1678)->f1; goto _LL1827; _LL1827: _temp1826=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp1678)->f2; goto _LL1725;} else{ goto
_LL1726;} _LL1726: if((( struct _tunion_struct*) _temp1678)->tag == Cyc_Absyn_Tuple_e_tag){
_LL1831: _temp1830=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1678)->f1; goto _LL1727;} else{ goto _LL1728;} _LL1728: if((( struct
_tunion_struct*) _temp1678)->tag == Cyc_Absyn_CompoundLit_e_tag){ _LL1835:
_temp1834=( struct _tuple2*)(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp1678)->f1;
goto _LL1833; _LL1833: _temp1832=( struct Cyc_List_List*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1678)->f2; goto _LL1729;} else{ goto _LL1730;} _LL1730: if((( struct
_tunion_struct*) _temp1678)->tag == Cyc_Absyn_Array_e_tag){ _LL1837: _temp1836=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1678)->f1; goto
_LL1731;} else{ goto _LL1732;} _LL1732: if((( struct _tunion_struct*) _temp1678)->tag
== Cyc_Absyn_StmtExp_e_tag){ _LL1839: _temp1838=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_StmtExp_e_struct*) _temp1678)->f1; goto _LL1733;} else{ goto
_LL1734;} _LL1734: if((( struct _tunion_struct*) _temp1678)->tag == Cyc_Absyn_Codegen_e_tag){
_LL1841: _temp1840=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1678)->f1; goto _LL1735;} else{ goto _LL1736;} _LL1736: if((( struct
_tunion_struct*) _temp1678)->tag == Cyc_Absyn_Fill_e_tag){ _LL1843: _temp1842=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*) _temp1678)->f1; goto
_LL1737;} else{ goto _LL1738;} _LL1738: if((( struct _tunion_struct*) _temp1678)->tag
== Cyc_Absyn_Comprehension_e_tag){ _LL1849: _temp1848=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1678)->f1; goto _LL1847; _LL1847:
_temp1846=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1678)->f2; goto _LL1845; _LL1845: _temp1844=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1678)->f3; goto _LL1739;} else{
goto _LL1740;} _LL1740: if((( struct _tunion_struct*) _temp1678)->tag == Cyc_Absyn_Struct_e_tag){
_LL1859: _temp1858=( struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1678)->f1; _temp1860=&(( struct Cyc_Absyn_Struct_e_struct*) _temp1678)->f1;
goto _LL1856; _LL1856: _temp1855=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1678)->f2; _temp1857=&(( struct Cyc_Absyn_Struct_e_struct*) _temp1678)->f2;
goto _LL1854; _LL1854: _temp1853=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1678)->f3; goto _LL1851; _LL1851: _temp1850=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1678)->f4; _temp1852=&(( struct Cyc_Absyn_Struct_e_struct*)
_temp1678)->f4; goto _LL1741;} else{ goto _LL1742;} _LL1742: if((( struct
_tunion_struct*) _temp1678)->tag == Cyc_Absyn_Tunion_e_tag){ _LL1871: _temp1870=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp1678)->f1;
_temp1872=&(( struct Cyc_Absyn_Tunion_e_struct*) _temp1678)->f1; goto _LL1868;
_LL1868: _temp1867=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1678)->f2; _temp1869=&(( struct Cyc_Absyn_Tunion_e_struct*) _temp1678)->f2;
goto _LL1866; _LL1866: _temp1865=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1678)->f3; goto _LL1864; _LL1864: _temp1863=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1678)->f4; goto _LL1862; _LL1862:
_temp1861=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1678)->f5; goto _LL1743;} else{ goto _LL1744;} _LL1744: if((( struct
_tunion_struct*) _temp1678)->tag == Cyc_Absyn_XTunion_e_tag){ _LL1880: _temp1879=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_XTunion_e_struct*) _temp1678)->f1;
_temp1881=&(( struct Cyc_Absyn_XTunion_e_struct*) _temp1678)->f1; goto _LL1878;
_LL1878: _temp1877=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_e_struct*)
_temp1678)->f2; goto _LL1876; _LL1876: _temp1875=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_e_struct*) _temp1678)->f3; goto _LL1874; _LL1874:
_temp1873=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_e_struct*)
_temp1678)->f4; goto _LL1745;} else{ goto _LL1746;} _LL1746: if((( struct
_tunion_struct*) _temp1678)->tag == Cyc_Absyn_Enum_e_tag){ _LL1887: _temp1886=(
struct _tuple1*)(( struct Cyc_Absyn_Enum_e_struct*) _temp1678)->f1; goto _LL1885;
_LL1885: _temp1884=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp1678)->f2; goto _LL1883; _LL1883: _temp1882=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_e_struct*) _temp1678)->f3; goto _LL1747;} else{ goto
_LL1748;} _LL1748: if((( struct _tunion_struct*) _temp1678)->tag == Cyc_Absyn_Malloc_e_tag){
_LL1891: _temp1890=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1678)->f1; goto _LL1889; _LL1889: _temp1888=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1678)->f2; goto _LL1749;} else{ goto _LL1679;} _LL1681: Cyc_Tcexp_tcExpNoInst(
te, 0, _temp1750); return; _LL1683: Cyc_Tcexp_resolve_unknown_id( te, e,
_temp1752); Cyc_Tcexp_tcExpNoInst( te, topt, e); return; _LL1685: Cyc_Tcexp_resolve_unknown_fn(
te, e, _temp1756, _temp1754); Cyc_Tcexp_tcExpNoInst( te, topt, e); return;
_LL1687: Cyc_Tcexp_resolve_unresolved_mem( te, loc, topt, e, _temp1758); Cyc_Tcexp_tcExpNoInst(
te, topt, e); return; _LL1689: t= Cyc_Tcexp_tcConst( te, loc, topt, _temp1762, e);
goto _LL1679; _LL1691: t= Cyc_Tcexp_tcVar( te, loc, _temp1766, _temp1764); goto
_LL1679; _LL1693: t= Cyc_Tcexp_tcPrimop( te, loc, topt, _temp1770, _temp1768);
goto _LL1679; _LL1695: t= Cyc_Tcexp_tcIncrement( te, loc, topt, _temp1774,
_temp1772); goto _LL1679; _LL1697: t= Cyc_Tcexp_tcAssignOp( te, loc, topt,
_temp1780, _temp1778, _temp1776); goto _LL1679; _LL1699: t= Cyc_Tcexp_tcConditional(
te, loc, topt, _temp1786, _temp1784, _temp1782); goto _LL1679; _LL1701: t= Cyc_Tcexp_tcSeqExp(
te, loc, topt, _temp1790, _temp1788); goto _LL1679; _LL1703: t= Cyc_Tcexp_tcFnCall(
te, loc, topt, _temp1794, _temp1792); goto _LL1679; _LL1705: t= Cyc_Tcexp_tcThrow(
te, loc, topt, _temp1796); goto _LL1679; _LL1707: t= Cyc_Tcexp_tcInstantiate( te,
loc, topt, _temp1800, _temp1798); goto _LL1679; _LL1709: t= Cyc_Tcexp_tcCast( te,
loc, topt, _temp1804, _temp1802); goto _LL1679; _LL1711: t= Cyc_Tcexp_tcAddress(
te, loc, topt, _temp1806); goto _LL1679; _LL1713: t= Cyc_Tcexp_tcNew( te, loc,
topt, _temp1810, e, _temp1808); goto _LL1679; _LL1715: { void* t1= Cyc_Tcexp_tcExpNoPromote(
te, 0, _temp1812); t= Cyc_Tcexp_tcSizeof( te, loc, topt, t1); goto _LL1679;}
_LL1717: t= Cyc_Tcexp_tcSizeof( te, loc, topt, _temp1814); goto _LL1679; _LL1719:
t= Cyc_Tcexp_tcDeref( te, loc, topt, _temp1816); goto _LL1679; _LL1721: t= Cyc_Tcexp_tcStructMember(
te, loc, topt, e, _temp1820, _temp1818); goto _LL1679; _LL1723: t= Cyc_Tcexp_tcStructArrow(
te, loc, topt, _temp1824, _temp1822); goto _LL1679; _LL1725: t= Cyc_Tcexp_tcSubscript(
te, loc, topt, _temp1828, _temp1826); goto _LL1679; _LL1727: t= Cyc_Tcexp_tcTuple(
te, loc, topt, _temp1830); goto _LL1679; _LL1729: t= Cyc_Tcexp_tcCompoundLit( te,
loc, topt, _temp1834, _temp1832); goto _LL1679; _LL1731: { void** elt_topt= 0;
if( topt != 0){ void* _temp1893= Cyc_Tcutil_compress(({ void** _temp1892= topt;
if( _temp1892 == 0){ _throw( Null_Exception);}* _temp1892;})); struct Cyc_Absyn_Exp*
_temp1899; struct Cyc_Absyn_Tqual* _temp1901; void* _temp1903; void** _temp1905;
_LL1895: if(( unsigned int) _temp1893 > 4u?(( struct _tunion_struct*) _temp1893)->tag
== Cyc_Absyn_ArrayType_tag: 0){ _LL1904: _temp1903=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1893)->f1; _temp1905=&(( struct Cyc_Absyn_ArrayType_struct*) _temp1893)->f1;
goto _LL1902; _LL1902: _temp1901=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1893)->f2; goto _LL1900; _LL1900: _temp1899=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1893)->f3; goto _LL1896;} else{ goto
_LL1897;} _LL1897: goto _LL1898; _LL1896: elt_topt=( void**) _temp1905; goto
_LL1894; _LL1898: goto _LL1894; _LL1894:;} t= Cyc_Tcexp_tcArray( te, loc,
elt_topt, _temp1836); goto _LL1679;} _LL1733: t= Cyc_Tcexp_tcStmtExp( te, loc,
topt, _temp1838); goto _LL1679; _LL1735: t= Cyc_Tcexp_tcCodegen( te, loc, topt,
_temp1840); goto _LL1679; _LL1737: t= Cyc_Tcexp_tcFill( te, loc, topt, _temp1842);
goto _LL1679; _LL1739: t= Cyc_Tcexp_tcComprehension( te, loc, topt, _temp1848,
_temp1846, _temp1844); goto _LL1679; _LL1741: t= Cyc_Tcexp_tcStruct( te, loc,
topt, _temp1860, _temp1857, _temp1853, _temp1852); goto _LL1679; _LL1743: t= Cyc_Tcexp_tcTunion(
te, loc, topt, e, _temp1872, _temp1869, _temp1865, _temp1863, _temp1861); goto
_LL1679; _LL1745: t= Cyc_Tcexp_tcXTunion( te, loc, topt, _temp1881, _temp1877,
_temp1875, _temp1873); goto _LL1679; _LL1747: t=( void*)({ struct Cyc_Absyn_EnumType_struct*
_temp1906=( struct Cyc_Absyn_EnumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp1906[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp1907; _temp1907.tag= Cyc_Absyn_EnumType_tag;
_temp1907.f1=({ struct Cyc_Absyn_Enumdecl* _temp1908= _temp1884; if( _temp1908
== 0){ _throw( Null_Exception);} _temp1908->name;}); _temp1907.f2= _temp1884;
_temp1907;}); _temp1906;}); goto _LL1679; _LL1749: t= Cyc_Tcexp_tcMalloc( te,
loc, topt, _temp1890, _temp1888); goto _LL1679; _LL1679:;} e->topt=({ struct Cyc_Core_Opt*
_temp1909=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1909->v=( void*) t; _temp1909;});}