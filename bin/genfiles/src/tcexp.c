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
_temp119= topt; if( _temp119 == 0){ _throw( Null_Exception);}* _temp119;}); void*
_temp120= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Exp* _temp128; struct Cyc_Absyn_Tqual*
_temp130; void* _temp132; struct Cyc_Absyn_Structdecl** _temp134; struct Cyc_List_List*
_temp136; struct _tuple1* _temp138; _LL122: if(( unsigned int) _temp120 > 4u?((
struct _tunion_struct*) _temp120)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL133:
_temp132=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp120)->f1; goto
_LL131; _LL131: _temp130=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp120)->f2; goto _LL129; _LL129: _temp128=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp120)->f3; goto _LL123;} else{ goto _LL124;}
_LL124: if(( unsigned int) _temp120 > 4u?(( struct _tunion_struct*) _temp120)->tag
== Cyc_Absyn_StructType_tag: 0){ _LL139: _temp138=( struct _tuple1*)(( struct
Cyc_Absyn_StructType_struct*) _temp120)->f1; goto _LL137; _LL137: _temp136=(
struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*) _temp120)->f2;
goto _LL135; _LL135: _temp134=( struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*)
_temp120)->f3; goto _LL125;} else{ goto _LL126;} _LL126: goto _LL127; _LL123:(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct* _temp140=(
struct Cyc_Absyn_Array_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp140[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp141; _temp141.tag= Cyc_Absyn_Array_e_tag;
_temp141.f1= des; _temp141;}); _temp140;}))); goto _LL121; _LL125: if( _temp134
== 0? 1: _temp138 == 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp142=( char*)"Compiler bug: struct type not properly set"; struct
_tagged_string _temp143; _temp143.curr= _temp142; _temp143.base= _temp142;
_temp143.last_plus_one= _temp142 + 43; _temp143;}));}( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Struct_e_struct* _temp144=( struct Cyc_Absyn_Struct_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp144[ 0]=({ struct
Cyc_Absyn_Struct_e_struct _temp145; _temp145.tag= Cyc_Absyn_Struct_e_tag;
_temp145.f1=( struct _tuple1*)({ struct _tuple1* _temp146= _temp138; if(
_temp146 == 0){ _throw( Null_Exception);} _temp146;}); _temp145.f2= 0; _temp145.f3=
des; _temp145.f4=( struct Cyc_Absyn_Structdecl*)({ struct Cyc_Absyn_Structdecl**
_temp147= _temp134; if( _temp147 == 0){ _throw( Null_Exception);}* _temp147;});
_temp145;}); _temp144;}))); goto _LL121; _LL127:( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_Array_e_struct* _temp148=( struct Cyc_Absyn_Array_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct)); _temp148[ 0]=({ struct Cyc_Absyn_Array_e_struct
_temp149; _temp149.tag= Cyc_Absyn_Array_e_tag; _temp149.f1= des; _temp149;});
_temp148;}))); goto _LL121; _LL121:;}} static void Cyc_Tcexp_tcExpNoInst( struct
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
int len=( int)({ struct _tagged_string _temp222= _temp189;( unsigned int)(
_temp222.last_plus_one - _temp222.curr);}); struct Cyc_Absyn_Const_e_struct* r=({
struct Cyc_Absyn_Const_e_struct* _temp223=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp223[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp224; _temp224.tag= Cyc_Absyn_Const_e_tag; _temp224.f1=( void*)(( void*)({
struct Cyc_Absyn_Int_c_struct* _temp225=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp225[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp226; _temp226.tag= Cyc_Absyn_Int_c_tag; _temp226.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp226.f2= len; _temp226;}); _temp225;})); _temp224;}); _temp223;}); struct
Cyc_Absyn_Exp* elen= Cyc_Absyn_new_exp(( void*) r, loc); elen->topt=({ struct
Cyc_Core_Opt* _temp227=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp227->v=( void*) Cyc_Absyn_uint_t; _temp227;}); t= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t,(
void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp228=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp228[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp229; _temp229.tag= Cyc_Absyn_Upper_b_tag;
_temp229.f1= elen; _temp229;}); _temp228;})); if( topt != 0){ void* _temp231=
Cyc_Tcutil_compress(({ void** _temp230= topt; if( _temp230 == 0){ _throw(
Null_Exception);}* _temp230;})); struct Cyc_Absyn_Exp* _temp239; struct Cyc_Absyn_Tqual*
_temp241; void* _temp243; struct Cyc_Absyn_PtrInfo _temp245; struct Cyc_Absyn_Conref*
_temp247; struct Cyc_Absyn_Tqual* _temp249; struct Cyc_Absyn_Conref* _temp251;
void* _temp253; void* _temp255; _LL233: if(( unsigned int) _temp231 > 4u?((
struct _tunion_struct*) _temp231)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL244:
_temp243=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp231)->f1; goto
_LL242; _LL242: _temp241=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp231)->f2; goto _LL240; _LL240: _temp239=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp231)->f3; goto _LL234;} else{ goto _LL235;}
_LL235: if(( unsigned int) _temp231 > 4u?(( struct _tunion_struct*) _temp231)->tag
== Cyc_Absyn_PointerType_tag: 0){ _LL246: _temp245=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp231)->f1; _LL256: _temp255=( void*)
_temp245.elt_typ; goto _LL254; _LL254: _temp253=( void*) _temp245.rgn_typ; goto
_LL252; _LL252: _temp251=( struct Cyc_Absyn_Conref*) _temp245.nullable; goto
_LL250; _LL250: _temp249=( struct Cyc_Absyn_Tqual*) _temp245.tq; goto _LL248;
_LL248: _temp247=( struct Cyc_Absyn_Conref*) _temp245.bounds; goto _LL236;}
else{ goto _LL237;} _LL237: goto _LL238; _LL234: return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp257=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp257[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp258; _temp258.tag= Cyc_Absyn_ArrayType_tag;
_temp258.f1=( void*) Cyc_Absyn_uchar_t; _temp258.f2= _temp241; _temp258.f3=(
struct Cyc_Absyn_Exp*) elen; _temp258;}); _temp257;}); _LL236: if( ! Cyc_Tcutil_unify(({
void** _temp259= topt; if( _temp259 == 0){ _throw( Null_Exception);}* _temp259;}),
t)? Cyc_Tcutil_silent_castable( te, loc, t,({ void** _temp260= topt; if(
_temp260 == 0){ _throw( Null_Exception);}* _temp260;})): 0){ e->topt=({ struct
Cyc_Core_Opt* _temp261=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp261->v=( void*) t; _temp261;}); Cyc_Tcutil_unchecked_cast( te, e,({ void**
_temp262= topt; if( _temp262 == 0){ _throw( Null_Exception);}* _temp262;})); t=({
void** _temp263= topt; if( _temp263 == 0){ _throw( Null_Exception);}* _temp263;});}
else{ t= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t, Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind),
Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp264=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp264[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp265; _temp265.tag= Cyc_Absyn_Upper_b_tag;
_temp265.f1= elen; _temp265;}); _temp264;})); if( ! Cyc_Tcutil_unify(({ void**
_temp266= topt; if( _temp266 == 0){ _throw( Null_Exception);}* _temp266;}), t)?
Cyc_Tcutil_silent_castable( te, loc, t,({ void** _temp267= topt; if( _temp267 ==
0){ _throw( Null_Exception);}* _temp267;})): 0){ e->topt=({ struct Cyc_Core_Opt*
_temp268=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp268->v=( void*) t; _temp268;}); Cyc_Tcutil_unchecked_cast( te, e,({ void**
_temp269= topt; if( _temp269 == 0){ _throw( Null_Exception);}* _temp269;})); t=({
void** _temp270= topt; if( _temp270 == 0){ _throw( Null_Exception);}* _temp270;});}}
goto _LL232; _LL238: goto _LL232; _LL232:;} return t;} _LL170: t=( void*)({
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
void* t=( void*)({ struct Cyc_List_List* _temp321= desc_types; if( _temp321 == 0){
_throw( Null_Exception);} _temp321->hd;}); struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp322= args; if( _temp322 == 0){ _throw( Null_Exception);}
_temp322->hd;}); if( ! Cyc_Tcutil_coerce_arg( te, e, t)){ Cyc_Tcutil_terr( e->loc,({
struct _tagged_string _temp324= Cyc_Absynpp_typ2string( t); struct
_tagged_string _temp325= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp323= e->topt; if( _temp323 == 0){ _throw( Null_Exception);} _temp323->v;}));
xprintf("descriptor has type %.*s but argument has type %.*s", _temp324.last_plus_one
- _temp324.curr, _temp324.curr, _temp325.last_plus_one - _temp325.curr, _temp325.curr);}));}}
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
_temp331= e->topt; if( _temp331 == 0){ _throw( Null_Exception);} _temp331->v;}));
void* _temp332= p; _LL334: if(( int) _temp332 == Cyc_Absyn_Plus){ goto _LL335;}
else{ goto _LL336;} _LL336: if(( int) _temp332 == Cyc_Absyn_Minus){ goto _LL337;}
else{ goto _LL338;} _LL338: if(( int) _temp332 == Cyc_Absyn_Not){ goto _LL339;}
else{ goto _LL340;} _LL340: if(( int) _temp332 == Cyc_Absyn_Bitnot){ goto _LL341;}
else{ goto _LL342;} _LL342: if(( int) _temp332 == Cyc_Absyn_Size){ goto _LL343;}
else{ goto _LL344;} _LL344: goto _LL345; _LL335: goto _LL337; _LL337: if( ! Cyc_Tcutil_is_numeric(
e)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp346= Cyc_Absynpp_typ2string(
t); xprintf("expecting arithmetic type but found %.*s", _temp346.last_plus_one -
_temp346.curr, _temp346.curr);}));} return( void*)({ struct Cyc_Core_Opt*
_temp347= e->topt; if( _temp347 == 0){ _throw( Null_Exception);} _temp347->v;});
_LL339: Cyc_Tcutil_check_contains_assign( e); if( ! Cyc_Tcutil_coerce_to_bool(
te, e)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp348= Cyc_Absynpp_typ2string(
t); xprintf("expecting integral or * type but found %.*s", _temp348.last_plus_one
- _temp348.curr, _temp348.curr);}));} return Cyc_Absyn_sint_t; _LL341: if( ! Cyc_Tcutil_is_integral(
e)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp349= Cyc_Absynpp_typ2string(
t); xprintf("expecting integral type but found %.*s", _temp349.last_plus_one -
_temp349.curr, _temp349.curr);}));} return( void*)({ struct Cyc_Core_Opt*
_temp350= e->topt; if( _temp350 == 0){ _throw( Null_Exception);} _temp350->v;});
_LL343:{ void* _temp351= t; struct Cyc_Absyn_Exp* _temp359; struct Cyc_Absyn_Tqual*
_temp361; void* _temp363; struct Cyc_Absyn_PtrInfo _temp365; struct Cyc_Absyn_Conref*
_temp367; struct Cyc_Absyn_Tqual* _temp369; struct Cyc_Absyn_Conref* _temp371;
void* _temp373; void* _temp375; _LL353: if(( unsigned int) _temp351 > 4u?((
struct _tunion_struct*) _temp351)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL364:
_temp363=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp351)->f1; goto
_LL362; _LL362: _temp361=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp351)->f2; goto _LL360; _LL360: _temp359=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp351)->f3; goto _LL354;} else{ goto _LL355;}
_LL355: if(( unsigned int) _temp351 > 4u?(( struct _tunion_struct*) _temp351)->tag
== Cyc_Absyn_PointerType_tag: 0){ _LL366: _temp365=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp351)->f1; _LL376: _temp375=( void*)
_temp365.elt_typ; goto _LL374; _LL374: _temp373=( void*) _temp365.rgn_typ; goto
_LL372; _LL372: _temp371=( struct Cyc_Absyn_Conref*) _temp365.nullable; goto
_LL370; _LL370: _temp369=( struct Cyc_Absyn_Tqual*) _temp365.tq; goto _LL368;
_LL368: _temp367=( struct Cyc_Absyn_Conref*) _temp365.bounds; goto _LL356;}
else{ goto _LL357;} _LL357: goto _LL358; _LL354: goto _LL352; _LL356: goto
_LL352; _LL358: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp377= Cyc_Absynpp_typ2string(
t); xprintf("size requires pointer or array type, not %.*s", _temp377.last_plus_one
- _temp377.curr, _temp377.curr);})); goto _LL352; _LL352:;} return Cyc_Absyn_uint_t;
_LL345: return(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ char* _temp378=( char*)"Non-unary primop"; struct
_tagged_string _temp379; _temp379.curr= _temp378; _temp379.base= _temp378;
_temp379.last_plus_one= _temp378 + 17; _temp379;})); _LL333:;} static void* Cyc_Tcexp_tcArithBinop(
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
void* t1= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp384= e1->topt;
if( _temp384 == 0){ _throw( Null_Exception);} _temp384->v;})); void* t2= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp385= e2->topt; if( _temp385 == 0){ _throw(
Null_Exception);} _temp385->v;})); return Cyc_Tcutil_max_arithmetic_type( t1, t2);}}
static void* Cyc_Tcexp_tcPlus( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ void* t1=( void*)({ struct Cyc_Core_Opt* _temp386=
e1->topt; if( _temp386 == 0){ _throw( Null_Exception);} _temp386->v;}); if( Cyc_Tcutil_is_tagged_pointer_typ(
t1)){ if( ! Cyc_Tcutil_coerce_sint_typ( te, e2)){ Cyc_Tcutil_terr( e2->loc,({
struct _tagged_string _temp388= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp387= e2->topt; if( _temp387 == 0){ _throw( Null_Exception);} _temp387->v;}));
xprintf("expecting int but found %.*s", _temp388.last_plus_one - _temp388.curr,
_temp388.curr);}));} return t1;} else{ return Cyc_Tcexp_tcArithBinop( te, e1, e2,
Cyc_Tcutil_is_numeric);}} static void* Cyc_Tcexp_tcMinus( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ void* t1=( void*)({
struct Cyc_Core_Opt* _temp389= e1->topt; if( _temp389 == 0){ _throw(
Null_Exception);} _temp389->v;}); void* t2=( void*)({ struct Cyc_Core_Opt*
_temp390= e2->topt; if( _temp390 == 0){ _throw( Null_Exception);} _temp390->v;});
if( Cyc_Tcutil_is_tagged_pointer_typ( t1)){ if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){ if( ! Cyc_Tcutil_unify( t1, t2)){ Cyc_Tcutil_terr( e1->loc,({ struct
_tagged_string _temp393= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp391= e1->topt; if( _temp391 == 0){ _throw( Null_Exception);} _temp391->v;}));
struct _tagged_string _temp394= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp392= e2->topt; if( _temp392 == 0){ _throw( Null_Exception);} _temp392->v;}));
xprintf("pointer arithmetic on values of different types (%.*s != %.*s)",
_temp393.last_plus_one - _temp393.curr, _temp393.curr, _temp394.last_plus_one -
_temp394.curr, _temp394.curr);}));} return Cyc_Absyn_sint_t;} else{ if( ! Cyc_Tcutil_coerce_sint_typ(
te, e2)){ Cyc_Tcutil_terr( e2->loc,({ struct _tagged_string _temp395= Cyc_Absynpp_typ2string(
t1); struct _tagged_string _temp396= Cyc_Absynpp_typ2string( t2); xprintf("expecting either %.*s or int but found %.*s",
_temp395.last_plus_one - _temp395.curr, _temp395.curr, _temp396.last_plus_one -
_temp396.curr, _temp396.curr);}));} return t1;}} else{ return Cyc_Tcexp_tcArithBinop(
te, e1, e2, Cyc_Tcutil_is_numeric);}} static void* Cyc_Tcexp_tcAnyBinop( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2){ int e1_is_num= Cyc_Tcutil_is_numeric( e1); int
e2_is_num= Cyc_Tcutil_is_numeric( e2); void* t1= Cyc_Tcutil_compress(( void*)({
struct Cyc_Core_Opt* _temp397= e1->topt; if( _temp397 == 0){ _throw(
Null_Exception);} _temp397->v;})); void* t2= Cyc_Tcutil_compress(( void*)({
struct Cyc_Core_Opt* _temp398= e2->topt; if( _temp398 == 0){ _throw(
Null_Exception);} _temp398->v;})); if( e1_is_num? e2_is_num: 0){ return Cyc_Absyn_sint_t;}
else{ if( Cyc_Tcutil_unify( t1, t2)? Cyc_Tcutil_unify( t1, Cyc_Absyn_new_evar((
void*) Cyc_Absyn_BoxKind)): 0){ return Cyc_Absyn_sint_t;} else{ if( Cyc_Tcutil_silent_castable(
te, loc, t2, t1)){ Cyc_Tcutil_unchecked_cast( te, e2, t1); return Cyc_Absyn_sint_t;}
else{ if( Cyc_Tcutil_silent_castable( te, loc, t1, t2)){ Cyc_Tcutil_unchecked_cast(
te, e1, t2); return Cyc_Absyn_sint_t;} else{ void* t1=( void*)({ struct Cyc_Core_Opt*
_temp399= e1->topt; if( _temp399 == 0){ _throw( Null_Exception);} _temp399->v;});
void* t2=( void*)({ struct Cyc_Core_Opt* _temp400= e2->topt; if( _temp400 == 0){
_throw( Null_Exception);} _temp400->v;});{ struct _tuple0 _temp402=({ struct
_tuple0 _temp401; _temp401.f1= Cyc_Tcutil_compress( t1); _temp401.f2= Cyc_Tcutil_compress(
t2); _temp401;}); void* _temp408; struct Cyc_Absyn_PtrInfo _temp410; struct Cyc_Absyn_Conref*
_temp412; struct Cyc_Absyn_Tqual* _temp414; struct Cyc_Absyn_Conref* _temp416;
void* _temp418; void* _temp420; void* _temp422; struct Cyc_Absyn_PtrInfo
_temp424; struct Cyc_Absyn_Conref* _temp426; struct Cyc_Absyn_Tqual* _temp428;
struct Cyc_Absyn_Conref* _temp430; void* _temp432; void* _temp434; _LL404:
_LL423: _temp422= _temp402.f1; if(( unsigned int) _temp422 > 4u?(( struct
_tunion_struct*) _temp422)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL425:
_temp424=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp422)->f1; _LL435: _temp434=( void*) _temp424.elt_typ; goto _LL433; _LL433:
_temp432=( void*) _temp424.rgn_typ; goto _LL431; _LL431: _temp430=( struct Cyc_Absyn_Conref*)
_temp424.nullable; goto _LL429; _LL429: _temp428=( struct Cyc_Absyn_Tqual*)
_temp424.tq; goto _LL427; _LL427: _temp426=( struct Cyc_Absyn_Conref*) _temp424.bounds;
goto _LL409;} else{ goto _LL406;} _LL409: _temp408= _temp402.f2; if((
unsigned int) _temp408 > 4u?(( struct _tunion_struct*) _temp408)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL411: _temp410=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp408)->f1; _LL421: _temp420=( void*) _temp410.elt_typ; goto _LL419; _LL419:
_temp418=( void*) _temp410.rgn_typ; goto _LL417; _LL417: _temp416=( struct Cyc_Absyn_Conref*)
_temp410.nullable; goto _LL415; _LL415: _temp414=( struct Cyc_Absyn_Tqual*)
_temp410.tq; goto _LL413; _LL413: _temp412=( struct Cyc_Absyn_Conref*) _temp410.bounds;
goto _LL405;} else{ goto _LL406;} _LL406: goto _LL407; _LL405: if( Cyc_Tcutil_unify(
_temp434, _temp420)){ return Cyc_Absyn_sint_t;} goto _LL403; _LL407: goto _LL403;
_LL403:;} Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp436= Cyc_Absynpp_typ2string(
t1); struct _tagged_string _temp437= Cyc_Absynpp_typ2string( t2); xprintf("comparison not allowed between %.*s and %.*s",
_temp436.last_plus_one - _temp436.curr, _temp436.curr, _temp437.last_plus_one -
_temp437.curr, _temp437.curr);})); return Cyc_Absyn_wildtyp();}}}}} static void*
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
struct Cyc_Absyn_Structdecl** _temp668= _temp662; if( _temp668 == 0){ _throw(
Null_Exception);}* _temp668;}), _temp598); if( sf == 0? 1: !({ struct Cyc_Absyn_Structfield*
_temp669= sf; if( _temp669 == 0){ _throw( Null_Exception);} _temp669->tq;})->q_const){
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
struct Cyc_Absyn_Structdecl** _temp701= _temp695; if( _temp701 == 0){ _throw(
Null_Exception);}* _temp701;}), _temp602); if( sf == 0? 1: !({ struct Cyc_Absyn_Structfield*
_temp702= sf; if( _temp702 == 0){ _throw( Null_Exception);} _temp702->tq;})->q_const){
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
struct Cyc_Core_Opt* _temp734= e->topt; if( _temp734 == 0){ _throw(
Null_Exception);} _temp734->v;}); if( ! Cyc_Tcutil_is_numeric( e)? ! Cyc_Tcutil_is_tagged_pointer_typ(
t): 0){ Cyc_Tcutil_terr( e->loc,({ struct _tagged_string _temp735= Cyc_Absynpp_typ2string(
t); xprintf("expecting arithmetic or pointer type but found %.*s", _temp735.last_plus_one
- _temp735.curr, _temp735.curr);}));} return t;}} static void* Cyc_Tcexp_tcConditional(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3){ Cyc_Tcexp_tcTest(
te, e1,( struct _tagged_string)({ char* _temp736=( char*)"conditional expression";
struct _tagged_string _temp737; _temp737.curr= _temp736; _temp737.base= _temp736;
_temp737.last_plus_one= _temp736 + 23; _temp737;})); Cyc_Tcexp_tcExp( te, topt,
e2); Cyc_Tcexp_tcExp( te, topt, e3);{ void* t= Cyc_Absyn_new_evar(( void*) Cyc_Absyn_MemKind);
struct Cyc_List_List* l1=({ struct Cyc_List_List* _temp738=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp738->hd=( void*) e3; _temp738->tl=
0; _temp738;}); struct Cyc_List_List* l2=({ struct Cyc_List_List* _temp739=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp739->hd=(
void*) e2; _temp739->tl= l1; _temp739;}); if( ! Cyc_Tcutil_coerce_list( te, t,
l2)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp742= Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Core_Opt* _temp740= e2->topt; if( _temp740 == 0){ _throw(
Null_Exception);} _temp740->v;})); struct _tagged_string _temp743= Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Core_Opt* _temp741= e3->topt; if( _temp741 == 0){ _throw(
Null_Exception);} _temp741->v;})); xprintf("conditional clause types do not match: %.*s != %.*s",
_temp742.last_plus_one - _temp742.curr, _temp742.curr, _temp743.last_plus_one -
_temp743.curr, _temp743.curr);}));} return t;}} static void* Cyc_Tcexp_tcAssignOp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e1, struct Cyc_Core_Opt* po, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExpNoPromote(
te, 0, e1); Cyc_Tcexp_tcExp( te,( void**)(( void*)({ struct Cyc_Core_Opt*
_temp744= e1->topt; if( _temp744 == 0){ _throw( Null_Exception);}& _temp744->v;})),
e2);{ void* t1=( void*)({ struct Cyc_Core_Opt* _temp745= e1->topt; if( _temp745
== 0){ _throw( Null_Exception);} _temp745->v;}); void* t2=( void*)({ struct Cyc_Core_Opt*
_temp746= e2->topt; if( _temp746 == 0){ _throw( Null_Exception);} _temp746->v;});{
void* _temp747= Cyc_Tcutil_compress( t1); struct Cyc_Absyn_Exp* _temp753; struct
Cyc_Absyn_Tqual* _temp755; void* _temp757; _LL749: if(( unsigned int) _temp747 >
4u?(( struct _tunion_struct*) _temp747)->tag == Cyc_Absyn_ArrayType_tag: 0){
_LL758: _temp757=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp747)->f1;
goto _LL756; _LL756: _temp755=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp747)->f2; goto _LL754; _LL754: _temp753=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp747)->f3; goto _LL750;} else{ goto _LL751;}
_LL751: goto _LL752; _LL750: Cyc_Tcutil_terr( loc,( struct _tagged_string)({
char* _temp759=( char*)"cannot assign to an array"; struct _tagged_string
_temp760; _temp760.curr= _temp759; _temp760.base= _temp759; _temp760.last_plus_one=
_temp759 + 26; _temp760;})); goto _LL748; _LL752: goto _LL748; _LL748:;} if( !
Cyc_Absyn_is_lvalue( e1)){ return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_string)({ char* _temp761=( char*)"assignment to non-lvalue"; struct
_tagged_string _temp762; _temp762.curr= _temp761; _temp762.base= _temp761;
_temp762.last_plus_one= _temp761 + 25; _temp762;}));} Cyc_Tcexp_check_writable(
te, e1); if( po == 0){ if( ! Cyc_Tcutil_coerce_assign( te, e2, t1)){ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp763= Cyc_Absynpp_typ2string( t1); struct
_tagged_string _temp764= Cyc_Absynpp_typ2string( t2); xprintf("type mismatch: %.*s != %.*s",
_temp763.last_plus_one - _temp763.curr, _temp763.curr, _temp764.last_plus_one -
_temp764.curr, _temp764.curr);}));}} else{ void* p=( void*)({ struct Cyc_Core_Opt*
_temp765= po; if( _temp765 == 0){ _throw( Null_Exception);} _temp765->v;}); void*
t_result= Cyc_Tcexp_tcBinPrimop( te, loc, 0, p, e1, e2); if( !( Cyc_Tcutil_unify(
t_result, t1)? 1: Cyc_Tcutil_coerceable( t_result))){ return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_string)({ char* _temp766=( char*)"Cannot use this operator in an assignment";
struct _tagged_string _temp767; _temp767.curr= _temp766; _temp767.base= _temp766;
_temp767.last_plus_one= _temp766 + 42; _temp767;}));}} return t2;}} static void*
Cyc_Tcexp_tcSeqExp( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp(
te, 0, e1); Cyc_Tcexp_tcExp( te, topt, e2); return( void*)({ struct Cyc_Core_Opt*
_temp768= e2->topt; if( _temp768 == 0){ _throw( Null_Exception);} _temp768->v;});}
static void* Cyc_Tcexp_tcFnCall( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_List_List* es){ Cyc_Tcexp_tcExp(
te, 0, e);{ void* t= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp769= e->topt; if( _temp769 == 0){ _throw( Null_Exception);} _temp769->v;}));
void* _temp770= t; struct Cyc_Absyn_PtrInfo _temp776; struct Cyc_Absyn_Conref*
_temp778; struct Cyc_Absyn_Tqual* _temp780; struct Cyc_Absyn_Conref* _temp782;
void* _temp784; void* _temp786; _LL772: if(( unsigned int) _temp770 > 4u?((
struct _tunion_struct*) _temp770)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL777:
_temp776=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp770)->f1; _LL787: _temp786=( void*) _temp776.elt_typ; goto _LL785; _LL785:
_temp784=( void*) _temp776.rgn_typ; goto _LL783; _LL783: _temp782=( struct Cyc_Absyn_Conref*)
_temp776.nullable; goto _LL781; _LL781: _temp780=( struct Cyc_Absyn_Tqual*)
_temp776.tq; goto _LL779; _LL779: _temp778=( struct Cyc_Absyn_Conref*) _temp776.bounds;
goto _LL773;} else{ goto _LL774;} _LL774: goto _LL775; _LL773: Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp784); Cyc_Tcutil_check_nonzero_bound( loc, _temp778);{ void*
_temp788= Cyc_Tcutil_compress( _temp786); struct Cyc_Absyn_FnInfo _temp794;
struct Cyc_List_List* _temp796; int _temp798; struct Cyc_List_List* _temp800;
void* _temp802; struct Cyc_Core_Opt* _temp804; struct Cyc_List_List* _temp806;
_LL790: if(( unsigned int) _temp788 > 4u?(( struct _tunion_struct*) _temp788)->tag
== Cyc_Absyn_FnType_tag: 0){ _LL795: _temp794=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp788)->f1; _LL807: _temp806=( struct Cyc_List_List*)
_temp794.tvars; goto _LL805; _LL805: _temp804=( struct Cyc_Core_Opt*) _temp794.effect;
goto _LL803; _LL803: _temp802=( void*) _temp794.ret_typ; goto _LL801; _LL801:
_temp800=( struct Cyc_List_List*) _temp794.args; goto _LL799; _LL799: _temp798=(
int) _temp794.varargs; goto _LL797; _LL797: _temp796=( struct Cyc_List_List*)
_temp794.attributes; goto _LL791;} else{ goto _LL792;} _LL792: goto _LL793;
_LL791: if( _temp798){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp808=( char*)"vararg functions (...) not supported yet"; struct
_tagged_string _temp809; _temp809.curr= _temp808; _temp809.base= _temp808;
_temp809.last_plus_one= _temp808 + 41; _temp809;}));} if( topt != 0){ Cyc_Tcutil_unify(
_temp802,({ void** _temp810= topt; if( _temp810 == 0){ _throw( Null_Exception);}*
_temp810;}));} while( es != 0? _temp800 != 0: 0) { struct Cyc_Absyn_Exp* e1=(
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp811= es; if( _temp811 == 0){
_throw( Null_Exception);} _temp811->hd;}); void* t2=(*(( struct _tuple2*)({
struct Cyc_List_List* _temp812= _temp800; if( _temp812 == 0){ _throw(
Null_Exception);} _temp812->hd;}))).f3; Cyc_Tcexp_tcExp( te,( void**)& t2, e1);
if( ! Cyc_Tcutil_coerce_arg( te, e1, t2)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp814= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp813= e1->topt; if( _temp813 == 0){ _throw( Null_Exception);} _temp813->v;}));
struct _tagged_string _temp815= Cyc_Absynpp_typ2string( t2); xprintf("actual argument has type %.*s but formal has type %.*s",
_temp814.last_plus_one - _temp814.curr, _temp814.curr, _temp815.last_plus_one -
_temp815.curr, _temp815.curr);}));} es=({ struct Cyc_List_List* _temp816= es;
if( _temp816 == 0){ _throw( Null_Exception);} _temp816->tl;}); _temp800=({
struct Cyc_List_List* _temp817= _temp800; if( _temp817 == 0){ _throw(
Null_Exception);} _temp817->tl;});} if( es != 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp818=( char*)"too many arguments for function";
struct _tagged_string _temp819; _temp819.curr= _temp818; _temp819.base= _temp818;
_temp819.last_plus_one= _temp818 + 32; _temp819;}));} if( _temp800 != 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp820=( char*)"too few arguments for function";
struct _tagged_string _temp821; _temp821.curr= _temp820; _temp821.base= _temp820;
_temp821.last_plus_one= _temp820 + 31; _temp821;}));} Cyc_Tcenv_check_effect_accessible(
te, loc,( void*)({ struct Cyc_Core_Opt* _temp822= _temp804; if( _temp822 == 0){
_throw( Null_Exception);} _temp822->v;})); return _temp802; _LL793: return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_string)({ char* _temp823=( char*)"expected pointer to function";
struct _tagged_string _temp824; _temp824.curr= _temp823; _temp824.base= _temp823;
_temp824.last_plus_one= _temp823 + 29; _temp824;})); _LL789:;} _LL775: return
Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char* _temp825=( char*)"expected pointer to function";
struct _tagged_string _temp826; _temp826.curr= _temp825; _temp826.base= _temp825;
_temp826.last_plus_one= _temp825 + 29; _temp826;})); _LL771:;}} static void* Cyc_Tcexp_tcThrow(
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
struct Cyc_Core_Opt* _temp831= e->topt; if( _temp831 == 0){ _throw(
Null_Exception);} _temp831->v;}));{ void* _temp832= t1; struct Cyc_Absyn_PtrInfo
_temp838; struct Cyc_Absyn_Conref* _temp840; struct Cyc_Absyn_Tqual* _temp842;
struct Cyc_Absyn_Conref* _temp844; void* _temp846; void* _temp848; _LL834: if((
unsigned int) _temp832 > 4u?(( struct _tunion_struct*) _temp832)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL839: _temp838=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp832)->f1; _LL849: _temp848=( void*) _temp838.elt_typ; goto _LL847; _LL847:
_temp846=( void*) _temp838.rgn_typ; goto _LL845; _LL845: _temp844=( struct Cyc_Absyn_Conref*)
_temp838.nullable; goto _LL843; _LL843: _temp842=( struct Cyc_Absyn_Tqual*)
_temp838.tq; goto _LL841; _LL841: _temp840=( struct Cyc_Absyn_Conref*) _temp838.bounds;
goto _LL835;} else{ goto _LL836;} _LL836: goto _LL837; _LL835:{ void* _temp850=
Cyc_Tcutil_compress( _temp848); struct Cyc_Absyn_FnInfo _temp856; struct Cyc_List_List*
_temp858; int _temp860; struct Cyc_List_List* _temp862; void* _temp864; struct
Cyc_Core_Opt* _temp866; struct Cyc_List_List* _temp868; _LL852: if((
unsigned int) _temp850 > 4u?(( struct _tunion_struct*) _temp850)->tag == Cyc_Absyn_FnType_tag:
0){ _LL857: _temp856=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp850)->f1; _LL869: _temp868=( struct Cyc_List_List*) _temp856.tvars; goto
_LL867; _LL867: _temp866=( struct Cyc_Core_Opt*) _temp856.effect; goto _LL865;
_LL865: _temp864=( void*) _temp856.ret_typ; goto _LL863; _LL863: _temp862=(
struct Cyc_List_List*) _temp856.args; goto _LL861; _LL861: _temp860=( int)
_temp856.varargs; goto _LL859; _LL859: _temp858=( struct Cyc_List_List*)
_temp856.attributes; goto _LL853;} else{ goto _LL854;} _LL854: goto _LL855;
_LL853: { struct Cyc_List_List* instantiation= 0; if((( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( ts) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp868)){ return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char*
_temp870=( char*)"type instantiation mismatch"; struct _tagged_string _temp871;
_temp871.curr= _temp870; _temp871.base= _temp870; _temp871.last_plus_one=
_temp870 + 28; _temp871;}));} for( 0; ts != 0; ts=({ struct Cyc_List_List*
_temp872= ts; if( _temp872 == 0){ _throw( Null_Exception);} _temp872->tl;}),
_temp868=({ struct Cyc_List_List* _temp873= _temp868; if( _temp873 == 0){ _throw(
Null_Exception);} _temp873->tl;})){ void* k=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp874= _temp868; if( _temp874 == 0){ _throw( Null_Exception);} _temp874->hd;}))->kind);
Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars( te), k,( void*)({
struct Cyc_List_List* _temp875= ts; if( _temp875 == 0){ _throw( Null_Exception);}
_temp875->hd;})); instantiation=({ struct Cyc_List_List* _temp876=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp876->hd=( void*)({ struct
_tuple4* _temp877=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp877->f1=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp879=
_temp868; if( _temp879 == 0){ _throw( Null_Exception);} _temp879->hd;});
_temp877->f2=( void*)({ struct Cyc_List_List* _temp878= ts; if( _temp878 == 0){
_throw( Null_Exception);} _temp878->hd;}); _temp877;}); _temp876->tl=
instantiation; _temp876;});}{ void* new_fn_typ= Cyc_Tcutil_substitute(
instantiation,( void*)({ struct Cyc_Absyn_FnType_struct* _temp880=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp880[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp881; _temp881.tag= Cyc_Absyn_FnType_tag; _temp881.f1=({ struct Cyc_Absyn_FnInfo
_temp882; _temp882.tvars= 0; _temp882.effect= _temp866; _temp882.ret_typ=( void*)
_temp864; _temp882.args= _temp862; _temp882.varargs= _temp860; _temp882.attributes=
_temp858; _temp882;}); _temp881;}); _temp880;})); void* new_typ=( void*)({
struct Cyc_Absyn_PointerType_struct* _temp883=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp883[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp884; _temp884.tag= Cyc_Absyn_PointerType_tag;
_temp884.f1=({ struct Cyc_Absyn_PtrInfo _temp885; _temp885.elt_typ=( void*)
new_fn_typ; _temp885.rgn_typ=( void*) _temp846; _temp885.nullable= _temp844;
_temp885.tq= _temp842; _temp885.bounds= _temp840; _temp885;}); _temp884;});
_temp883;}); return new_typ;}} _LL855: goto _LL851; _LL851:;} goto _LL833;
_LL837: goto _LL833; _LL833:;} return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp886= Cyc_Absynpp_typ2string( t1); xprintf("expecting polymorphic type but found %.*s",
_temp886.last_plus_one - _temp886.curr, _temp886.curr);}));}} static void* Cyc_Tcexp_tcCast(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, void*
t, struct Cyc_Absyn_Exp* e){ Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars(
te),( void*) Cyc_Absyn_MemKind, t); Cyc_Tcexp_tcExp( te, 0, e);{ void* t2=( void*)({
struct Cyc_Core_Opt* _temp887= e->topt; if( _temp887 == 0){ _throw(
Null_Exception);} _temp887->v;}); if( ! Cyc_Tcutil_silent_castable( te, loc, t2,
t)? ! Cyc_Tcutil_castable( te, loc, t2, t): 0){ return Cyc_Tcexp_expr_err( te,
loc,({ struct _tagged_string _temp888= Cyc_Absynpp_typ2string( t2); struct
_tagged_string _temp889= Cyc_Absynpp_typ2string( t); xprintf("cannot cast %.*s to %.*s",
_temp888.last_plus_one - _temp888.curr, _temp888.curr, _temp889.last_plus_one -
_temp889.curr, _temp889.curr);}));} return t;}} static void* Cyc_Tcexp_tcAddress(
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
_temp957= e->topt; if( _temp957 == 0){ _throw( Null_Exception);} _temp957->v;}));
void* _temp958= t; struct Cyc_Absyn_PtrInfo _temp964; struct Cyc_Absyn_Conref*
_temp966; struct Cyc_Absyn_Tqual* _temp968; struct Cyc_Absyn_Conref* _temp970;
void* _temp972; void* _temp974; _LL960: if(( unsigned int) _temp958 > 4u?((
struct _tunion_struct*) _temp958)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL965:
_temp964=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp958)->f1; _LL975: _temp974=( void*) _temp964.elt_typ; goto _LL973; _LL973:
_temp972=( void*) _temp964.rgn_typ; goto _LL971; _LL971: _temp970=( struct Cyc_Absyn_Conref*)
_temp964.nullable; goto _LL969; _LL969: _temp968=( struct Cyc_Absyn_Tqual*)
_temp964.tq; goto _LL967; _LL967: _temp966=( struct Cyc_Absyn_Conref*) _temp964.bounds;
goto _LL961;} else{ goto _LL962;} _LL962: goto _LL963; _LL961: Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp972); Cyc_Tcutil_check_nonzero_bound( loc, _temp966); return
_temp974; _LL963: return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string
_temp976= Cyc_Absynpp_typ2string( t); xprintf("expecting * or @ type but found %.*s",
_temp976.last_plus_one - _temp976.curr, _temp976.curr);})); _LL959:;}} static
void* Cyc_Tcexp_tcStructMember( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* outer_e, struct Cyc_Absyn_Exp* e, struct
_tagged_string* f){ Cyc_Tcexp_tcExpNoPromote( te, 0, e);{ void* t= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp977= e->topt; if( _temp977 == 0){ _throw(
Null_Exception);} _temp977->v;})); void* _temp978= t; struct Cyc_Absyn_Structdecl**
_temp990; struct Cyc_Absyn_Structdecl* _temp992; struct Cyc_List_List* _temp993;
struct _tuple1* _temp995; struct Cyc_Absyn_Uniondecl** _temp997; struct Cyc_Absyn_Uniondecl*
_temp999; struct Cyc_List_List* _temp1000; struct _tuple1* _temp1002; struct Cyc_Absyn_Exp*
_temp1005; struct Cyc_Absyn_Tqual* _temp1007; void* _temp1009; struct Cyc_Absyn_PtrInfo
_temp1014; _LL980: if(( unsigned int) _temp978 > 4u?(( struct _tunion_struct*)
_temp978)->tag == Cyc_Absyn_StructType_tag: 0){ _LL996: _temp995=( struct
_tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp978)->f1; goto _LL994;
_LL994: _temp993=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp978)->f2; goto _LL991; _LL991: _temp990=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp978)->f3; if( _temp990 == 0){ goto
_LL982;} else{ _temp992=* _temp990; goto _LL981;}} else{ goto _LL982;} _LL982:
if(( unsigned int) _temp978 > 4u?(( struct _tunion_struct*) _temp978)->tag ==
Cyc_Absyn_UnionType_tag: 0){ _LL1003: _temp1002=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp978)->f1; goto _LL1001; _LL1001: _temp1000=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnionType_struct*) _temp978)->f2; goto _LL998; _LL998: _temp997=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp978)->f3;
if( _temp997 == 0){ goto _LL984;} else{ _temp999=* _temp997; goto _LL983;}}
else{ goto _LL984;} _LL984: if(( unsigned int) _temp978 > 4u?(( struct
_tunion_struct*) _temp978)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL1010:
_temp1009=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp978)->f1; goto
_LL1008; _LL1008: _temp1007=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp978)->f2; goto _LL1006; _LL1006: _temp1005=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp978)->f3; goto _LL1004;} else{ goto
_LL986;} _LL1004: if( Cyc_String_zstrcmp(* f,( struct _tagged_string)({ char*
_temp1011=( char*)"size"; struct _tagged_string _temp1012; _temp1012.curr=
_temp1011; _temp1012.base= _temp1011; _temp1012.last_plus_one= _temp1011 + 5;
_temp1012;})) == 0){ goto _LL985;} else{ goto _LL986;} _LL986: if(( unsigned int)
_temp978 > 4u?(( struct _tunion_struct*) _temp978)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL1015: _temp1014=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp978)->f1; goto _LL1013;} else{ goto _LL988;} _LL1013: if( Cyc_String_zstrcmp(*
f,( struct _tagged_string)({ char* _temp1016=( char*)"size"; struct
_tagged_string _temp1017; _temp1017.curr= _temp1016; _temp1017.base= _temp1016;
_temp1017.last_plus_one= _temp1016 + 5; _temp1017;})) == 0){ goto _LL987;} else{
goto _LL988;} _LL988: goto _LL989; _LL981: if( _temp995 == 0){ return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_string)({ char* _temp1018=( char*)"unresolved struct type (COMPILER ERROR)";
struct _tagged_string _temp1019; _temp1019.curr= _temp1018; _temp1019.base=
_temp1018; _temp1019.last_plus_one= _temp1018 + 40; _temp1019;}));}{ struct Cyc_Absyn_Structfield*
finfo= Cyc_Absyn_lookup_struct_field( _temp992, f); if( finfo == 0){ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp1021= Cyc_Absynpp_qvar2string(( struct
_tuple1*)({ struct _tuple1* _temp1020= _temp995; if( _temp1020 == 0){ _throw(
Null_Exception);} _temp1020;})); struct _tagged_string _temp1022=* f; xprintf("struct %.*s has no %.*s field",
_temp1021.last_plus_one - _temp1021.curr, _temp1021.curr, _temp1022.last_plus_one
- _temp1022.curr, _temp1022.curr);}));}{ struct Cyc_List_List* inst=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
_temp992->tvs, _temp993); void* t2= Cyc_Tcutil_substitute( inst,( void*)({
struct Cyc_Absyn_Structfield* _temp1023= finfo; if( _temp1023 == 0){ _throw(
Null_Exception);} _temp1023->type;})); return t2;}} _LL983: if( _temp1002 == 0){
return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char* _temp1024=(
char*)"unresolved union type (COMPILER ERROR)"; struct _tagged_string _temp1025;
_temp1025.curr= _temp1024; _temp1025.base= _temp1024; _temp1025.last_plus_one=
_temp1024 + 39; _temp1025;}));}{ struct Cyc_Absyn_Structfield* finfo= Cyc_Absyn_lookup_union_field(
_temp999, f); if( finfo == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp1027= Cyc_Absynpp_qvar2string(( struct _tuple1*)({ struct
_tuple1* _temp1026= _temp1002; if( _temp1026 == 0){ _throw( Null_Exception);}
_temp1026;})); struct _tagged_string _temp1028=* f; xprintf("union %.*s has no %.*s field",
_temp1027.last_plus_one - _temp1027.curr, _temp1027.curr, _temp1028.last_plus_one
- _temp1028.curr, _temp1028.curr);}));}{ struct Cyc_List_List* inst=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
_temp999->tvs, _temp1000); void* t2= Cyc_Tcutil_substitute( inst,( void*)({
struct Cyc_Absyn_Structfield* _temp1029= finfo; if( _temp1029 == 0){ _throw(
Null_Exception);} _temp1029->type;})); return t2;}} _LL985:( void*)( outer_e->r=(
void*)(( void*)({ struct Cyc_Absyn_Primop_e_struct* _temp1030=( struct Cyc_Absyn_Primop_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp1030[ 0]=({ struct
Cyc_Absyn_Primop_e_struct _temp1031; _temp1031.tag= Cyc_Absyn_Primop_e_tag;
_temp1031.f1=( void*)(( void*) Cyc_Absyn_Size); _temp1031.f2=({ struct Cyc_List_List*
_temp1032=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1032->hd=( void*) e; _temp1032->tl= 0; _temp1032;}); _temp1031;});
_temp1030;}))); return Cyc_Absyn_uint_t; _LL987:( void*)( outer_e->r=( void*)((
void*)({ struct Cyc_Absyn_Primop_e_struct* _temp1033=( struct Cyc_Absyn_Primop_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp1033[ 0]=({ struct
Cyc_Absyn_Primop_e_struct _temp1034; _temp1034.tag= Cyc_Absyn_Primop_e_tag;
_temp1034.f1=( void*)(( void*) Cyc_Absyn_Size); _temp1034.f2=({ struct Cyc_List_List*
_temp1035=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1035->hd=( void*) e; _temp1035->tl= 0; _temp1035;}); _temp1034;});
_temp1033;}))); return Cyc_Absyn_uint_t; _LL989: if( Cyc_String_zstrcmp(* f,(
struct _tagged_string)({ char* _temp1036=( char*)"size"; struct _tagged_string
_temp1037; _temp1037.curr= _temp1036; _temp1037.base= _temp1036; _temp1037.last_plus_one=
_temp1036 + 5; _temp1037;})) == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp1038= Cyc_Absynpp_typ2string( t); xprintf("expecting struct, union or array, found %.*s",
_temp1038.last_plus_one - _temp1038.curr, _temp1038.curr);}));} else{ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp1039= Cyc_Absynpp_typ2string( t); xprintf("expecting struct or union, found %.*s",
_temp1039.last_plus_one - _temp1039.curr, _temp1039.curr);}));} _LL979:;}}
static void* Cyc_Tcexp_tcStructArrow( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e, struct _tagged_string* f){ Cyc_Tcexp_tcExp(
te, 0, e);{ void* t= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp1040= e->topt; if( _temp1040 == 0){ _throw( Null_Exception);} _temp1040->v;}));
void* _temp1041= t; struct Cyc_Absyn_PtrInfo _temp1047; struct Cyc_Absyn_Conref*
_temp1049; struct Cyc_Absyn_Tqual* _temp1051; struct Cyc_Absyn_Conref* _temp1053;
void* _temp1055; void* _temp1057; _LL1043: if(( unsigned int) _temp1041 > 4u?((
struct _tunion_struct*) _temp1041)->tag == Cyc_Absyn_PointerType_tag: 0){
_LL1048: _temp1047=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1041)->f1; _LL1058: _temp1057=( void*) _temp1047.elt_typ; goto _LL1056;
_LL1056: _temp1055=( void*) _temp1047.rgn_typ; goto _LL1054; _LL1054: _temp1053=(
struct Cyc_Absyn_Conref*) _temp1047.nullable; goto _LL1052; _LL1052: _temp1051=(
struct Cyc_Absyn_Tqual*) _temp1047.tq; goto _LL1050; _LL1050: _temp1049=( struct
Cyc_Absyn_Conref*) _temp1047.bounds; goto _LL1044;} else{ goto _LL1045;} _LL1045:
goto _LL1046; _LL1044: Cyc_Tcutil_check_nonzero_bound( loc, _temp1049);{ void*
_temp1059= Cyc_Tcutil_compress( _temp1057); struct Cyc_Absyn_Structdecl**
_temp1067; struct Cyc_Absyn_Structdecl* _temp1069; struct Cyc_List_List*
_temp1070; struct _tuple1* _temp1072; struct Cyc_Absyn_Uniondecl** _temp1074;
struct Cyc_Absyn_Uniondecl* _temp1076; struct Cyc_List_List* _temp1077; struct
_tuple1* _temp1079; _LL1061: if(( unsigned int) _temp1059 > 4u?(( struct
_tunion_struct*) _temp1059)->tag == Cyc_Absyn_StructType_tag: 0){ _LL1073:
_temp1072=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp1059)->f1;
goto _LL1071; _LL1071: _temp1070=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp1059)->f2; goto _LL1068; _LL1068: _temp1067=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp1059)->f3; if( _temp1067 == 0){ goto
_LL1063;} else{ _temp1069=* _temp1067; goto _LL1062;}} else{ goto _LL1063;}
_LL1063: if(( unsigned int) _temp1059 > 4u?(( struct _tunion_struct*) _temp1059)->tag
== Cyc_Absyn_UnionType_tag: 0){ _LL1080: _temp1079=( struct _tuple1*)(( struct
Cyc_Absyn_UnionType_struct*) _temp1059)->f1; goto _LL1078; _LL1078: _temp1077=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*) _temp1059)->f2;
goto _LL1075; _LL1075: _temp1074=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp1059)->f3; if( _temp1074 == 0){ goto _LL1065;} else{ _temp1076=* _temp1074;
goto _LL1064;}} else{ goto _LL1065;} _LL1065: goto _LL1066; _LL1062: { struct
Cyc_Absyn_Structfield* finfo= Cyc_Absyn_lookup_struct_field( _temp1069, f); if(
finfo == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string
_temp1082= Cyc_Absynpp_qvar2string(( struct _tuple1*)({ struct _tuple1*
_temp1081= _temp1072; if( _temp1081 == 0){ _throw( Null_Exception);} _temp1081;}));
struct _tagged_string _temp1083=* f; xprintf("struct %.*s has no %.*s field",
_temp1082.last_plus_one - _temp1082.curr, _temp1082.curr, _temp1083.last_plus_one
- _temp1083.curr, _temp1083.curr);}));}{ struct Cyc_List_List* inst=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
_temp1069->tvs, _temp1070); void* t3= Cyc_Tcutil_substitute( inst,( void*)({
struct Cyc_Absyn_Structfield* _temp1084= finfo; if( _temp1084 == 0){ _throw(
Null_Exception);} _temp1084->type;})); Cyc_Tcenv_check_rgn_accessible( te, loc,
_temp1055); return t3;}} _LL1064: { struct Cyc_Absyn_Structfield* finfo= Cyc_Absyn_lookup_union_field(
_temp1076, f); if( finfo == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp1086= Cyc_Absynpp_qvar2string(( struct _tuple1*)({ struct
_tuple1* _temp1085= _temp1079; if( _temp1085 == 0){ _throw( Null_Exception);}
_temp1085;})); struct _tagged_string _temp1087=* f; xprintf("union %.*s has no %.*s field",
_temp1086.last_plus_one - _temp1086.curr, _temp1086.curr, _temp1087.last_plus_one
- _temp1087.curr, _temp1087.curr);}));}{ struct Cyc_List_List* inst=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
_temp1076->tvs, _temp1077); void* t3= Cyc_Tcutil_substitute( inst,( void*)({
struct Cyc_Absyn_Structfield* _temp1088= finfo; if( _temp1088 == 0){ _throw(
Null_Exception);} _temp1088->type;})); Cyc_Tcenv_check_rgn_accessible( te, loc,
_temp1055); return t3;}} _LL1066: return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp1089= Cyc_Absynpp_typ2string( t); xprintf("expecting struct pointer, found %.*s",
_temp1089.last_plus_one - _temp1089.curr, _temp1089.curr);})); _LL1060:;}
_LL1046: return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string _temp1090=
Cyc_Absynpp_typ2string( t); xprintf("expecting struct pointer, found %.*s",
_temp1090.last_plus_one - _temp1090.curr, _temp1090.curr);})); _LL1042:;}}
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
_temp1101= e1->topt; if( _temp1101 == 0){ _throw( Null_Exception);} _temp1101->v;}));
void* t2= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp1102= e2->topt;
if( _temp1102 == 0){ _throw( Null_Exception);} _temp1102->v;})); if( ! Cyc_Tcutil_coerce_sint_typ(
te, e2)){ return Cyc_Tcexp_expr_err( te, e2->loc,({ struct _tagged_string
_temp1103= Cyc_Absynpp_typ2string( t2); xprintf("expecting int subscript, found %.*s",
_temp1103.last_plus_one - _temp1103.curr, _temp1103.curr);}));}{ void* _temp1104=
t1; struct Cyc_Absyn_PtrInfo _temp1112; struct Cyc_Absyn_Conref* _temp1114;
struct Cyc_Absyn_Tqual* _temp1116; struct Cyc_Absyn_Conref* _temp1118; void*
_temp1120; void* _temp1122; struct Cyc_List_List* _temp1124; _LL1106: if((
unsigned int) _temp1104 > 4u?(( struct _tunion_struct*) _temp1104)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL1113: _temp1112=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1104)->f1; _LL1123: _temp1122=( void*) _temp1112.elt_typ; goto _LL1121;
_LL1121: _temp1120=( void*) _temp1112.rgn_typ; goto _LL1119; _LL1119: _temp1118=(
struct Cyc_Absyn_Conref*) _temp1112.nullable; goto _LL1117; _LL1117: _temp1116=(
struct Cyc_Absyn_Tqual*) _temp1112.tq; goto _LL1115; _LL1115: _temp1114=( struct
Cyc_Absyn_Conref*) _temp1112.bounds; goto _LL1107;} else{ goto _LL1108;} _LL1108:
if(( unsigned int) _temp1104 > 4u?(( struct _tunion_struct*) _temp1104)->tag ==
Cyc_Absyn_TupleType_tag: 0){ _LL1125: _temp1124=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp1104)->f1; goto _LL1109;} else{ goto
_LL1110;} _LL1110: goto _LL1111; _LL1107: if( Cyc_Tcutil_is_const_exp( te, e2)){
Cyc_Tcutil_check_bound( loc, Cyc_Evexp_eval_const_uint_exp( e2), _temp1114);}
else{ Cyc_Tcutil_check_nonzero_bound( loc, _temp1114);} Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp1120); return _temp1122; _LL1109: return Cyc_Tcexp_ithTupleType(
te, loc, _temp1124, e2); _LL1111: return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp1126= Cyc_Absynpp_typ2string( t1); xprintf("subscript applied to %.*s",
_temp1126.last_plus_one - _temp1126.curr, _temp1126.curr);})); _LL1105:;}}}
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
void* topt2=(*(( struct _tuple7*)({ struct Cyc_List_List* _temp1140= _temp1134;
if( _temp1140 == 0){ _throw( Null_Exception);} _temp1140->hd;}))).f2; Cyc_Tcexp_tcExpInitializer(
te,( void**)& topt2,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1141=
es; if( _temp1141 == 0){ _throw( Null_Exception);} _temp1141->hd;})); fields=({
struct Cyc_List_List* _temp1142=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1142->hd=( void*)({ struct _tuple7* _temp1143=(
struct _tuple7*) GC_malloc( sizeof( struct _tuple7)); _temp1143->f1=(*(( struct
_tuple7*)({ struct Cyc_List_List* _temp1146= _temp1134; if( _temp1146 == 0){
_throw( Null_Exception);} _temp1146->hd;}))).f1; _temp1143->f2=( void*)({ struct
Cyc_Core_Opt* _temp1145=(( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1144= es; if( _temp1144 == 0){ _throw( Null_Exception);} _temp1144->hd;}))->topt;
if( _temp1145 == 0){ _throw( Null_Exception);} _temp1145->v;}); _temp1143;});
_temp1142->tl= fields; _temp1142;});} done= 1; goto _LL1129; _LL1133: goto
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
Cyc_Absyn_Const_e_struct* _temp1157=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp1157[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp1158; _temp1158.tag= Cyc_Absyn_Const_e_tag;
_temp1158.f1=( void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp1159=(
struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp1159[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp1160; _temp1160.tag= Cyc_Absyn_Int_c_tag;
_temp1160.f1=( void*)(( void*) Cyc_Absyn_Unsigned); _temp1160.f2=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( es); _temp1160;}); _temp1159;}));
_temp1158;}); _temp1157;}); struct Cyc_Absyn_Exp* sz_exp= Cyc_Absyn_new_exp((
void*) sz_rexp, loc); sz_exp->topt=({ struct Cyc_Core_Opt* _temp1161=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1161->v=( void*)
Cyc_Absyn_uint_t; _temp1161;});{ void* res_t2=( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp1162=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1162[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp1163; _temp1163.tag= Cyc_Absyn_ArrayType_tag;
_temp1163.f1=( void*) res; _temp1163.f2= Cyc_Absyn_empty_tqual(); _temp1163.f3=(
struct Cyc_Absyn_Exp*) sz_exp; _temp1163;}); _temp1162;});{ struct Cyc_List_List*
es2= es; for( 0; es2 != 0; es2=({ struct Cyc_List_List* _temp1164= es2; if(
_temp1164 == 0){ _throw( Null_Exception);} _temp1164->tl;})){ Cyc_Tcexp_tcExpInitializer(
te, elt_topt,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1165= es2;
if( _temp1165 == 0){ _throw( Null_Exception);} _temp1165->hd;}));}} if( ! Cyc_Tcutil_coerce_list(
te, res, es)){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1166= es; if( _temp1166 == 0){ _throw( Null_Exception);} _temp1166->hd;}))->loc,({
struct _tagged_string _temp1167= Cyc_Absynpp_typ2string( res); xprintf("elements of array do not all have the same type (%.*s)",
_temp1167.last_plus_one - _temp1167.curr, _temp1167.curr);}));}{ int offset= 0;
for( 0; des != 0; offset ++, des=({ struct Cyc_List_List* _temp1168= des; if(
_temp1168 == 0){ _throw( Null_Exception);} _temp1168->tl;})){ struct Cyc_List_List*
ds=(*(( struct _tuple6*)({ struct Cyc_List_List* _temp1169= des; if( _temp1169
== 0){ _throw( Null_Exception);} _temp1169->hd;}))).f1; if( ds != 0){ void*
_temp1171=( void*)({ struct Cyc_List_List* _temp1170= ds; if( _temp1170 == 0){
_throw( Null_Exception);} _temp1170->hd;}); struct _tagged_string* _temp1177;
struct Cyc_Absyn_Exp* _temp1179; _LL1173: if((( struct _tunion_struct*)
_temp1171)->tag == Cyc_Absyn_FieldName_tag){ _LL1178: _temp1177=( struct
_tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp1171)->f1; goto
_LL1174;} else{ goto _LL1175;} _LL1175: if((( struct _tunion_struct*) _temp1171)->tag
== Cyc_Absyn_ArrayElement_tag){ _LL1180: _temp1179=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayElement_struct*) _temp1171)->f1; goto _LL1176;} else{ goto
_LL1172;} _LL1174: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp1181=( char*)"only array index designators are supported"; struct
_tagged_string _temp1182; _temp1182.curr= _temp1181; _temp1182.base= _temp1181;
_temp1182.last_plus_one= _temp1181 + 43; _temp1182;})); goto _LL1172; _LL1176:
Cyc_Tcexp_tcExpInitializer( te, 0, _temp1179);{ unsigned int i= Cyc_Evexp_eval_const_uint_exp(
_temp1179); if( i != offset){ Cyc_Tcutil_terr( _temp1179->loc, xprintf("expecting index designator %d but found %d",
offset,( int) i));} goto _LL1172;} _LL1172:;}}} return res_t2;}} static void*
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
res_typ=({ struct Cyc_Absyn_StructType_struct* _temp1226=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1226[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1227; _temp1227.tag= Cyc_Absyn_StructType_tag;
_temp1227.f1=( struct _tuple1*)* tn; _temp1227.f2= typs; _temp1227.f3=({ struct
Cyc_Absyn_Structdecl** _temp1228=( struct Cyc_Absyn_Structdecl**) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl*)); _temp1228[ 0]= sd; _temp1228;});
_temp1227;}); _temp1226;}); if( topt != 0){ Cyc_Tcutil_unify(( void*) res_typ,({
void** _temp1229= topt; if( _temp1229 == 0){ _throw( Null_Exception);}*
_temp1229;}));}* otyps=({ struct Cyc_Core_Opt* _temp1230=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1230->v=( void*) typs; _temp1230;});{
struct Cyc_List_List* fields=(( struct Cyc_List_List*(*)( struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_Absyn_Structdecl* sd)) Cyc_Tcutil_resolve_struct_designators)(
loc, args, sd); for( 0; fields != 0; fields=({ struct Cyc_List_List* _temp1231=
fields; if( _temp1231 == 0){ _throw( Null_Exception);} _temp1231->tl;})){ struct
_tuple8 _temp1235; struct Cyc_Absyn_Exp* _temp1236; struct Cyc_Absyn_Structfield*
_temp1238; struct _tuple8* _temp1233=( struct _tuple8*)({ struct Cyc_List_List*
_temp1232= fields; if( _temp1232 == 0){ _throw( Null_Exception);} _temp1232->hd;});
_temp1235=* _temp1233; _LL1239: _temp1238= _temp1235.f1; goto _LL1237; _LL1237:
_temp1236= _temp1235.f2; goto _LL1234; _LL1234: { void* inst_fieldtyp= Cyc_Tcutil_substitute(
inst,( void*) _temp1238->type); Cyc_Tcexp_tcExpInitializer( te,( void**)&
inst_fieldtyp, _temp1236); if( ! Cyc_Tcutil_coerce_arg( te, _temp1236,
inst_fieldtyp)){ Cyc_Tcutil_terr( _temp1236->loc,({ struct _tagged_string
_temp1241=* _temp1238->name; struct _tagged_string _temp1242= Cyc_Absynpp_qvar2string(*
tn); struct _tagged_string _temp1243= Cyc_Absynpp_typ2string( inst_fieldtyp);
struct _tagged_string _temp1244= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp1240= _temp1236->topt; if( _temp1240 == 0){ _throw( Null_Exception);}
_temp1240->v;})); xprintf("field %.*s of struct %.*s expects type %.*s != %.*s",
_temp1241.last_plus_one - _temp1241.curr, _temp1241.curr, _temp1242.last_plus_one
- _temp1242.curr, _temp1242.curr, _temp1243.last_plus_one - _temp1243.curr,
_temp1243.curr, _temp1244.last_plus_one - _temp1244.curr, _temp1244.curr);}));}}}
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
_temp1247=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp1247[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp1248; _temp1248.tag= Cyc_Absyn_TunionFieldType_tag; _temp1248.f1=({ struct
Cyc_Absyn_TunionFieldInfo _temp1249; _temp1249.name=( struct _tuple1*)({ struct
Cyc_Core_Opt* _temp1250= ed->name; if( _temp1250 == 0){ _throw( Null_Exception);}
_temp1250->v;}); _temp1249.targs= all_typs; _temp1249.fname= ef->name; _temp1249.tud=(
struct Cyc_Absyn_Tuniondecl*) ed; _temp1249.tufd=( struct Cyc_Absyn_Tunionfield*)
ef; _temp1249;}); _temp1248;}); _temp1247;}); if( topt != 0){ void* _temp1252=
Cyc_Tcutil_compress(({ void** _temp1251= topt; if( _temp1251 == 0){ _throw(
Null_Exception);}* _temp1251;})); struct Cyc_Absyn_TunionFieldInfo _temp1260;
struct Cyc_Absyn_TunionInfo _temp1262; struct Cyc_Absyn_Tuniondecl* _temp1264;
void* _temp1266; struct Cyc_List_List* _temp1268; struct _tuple1* _temp1270;
_LL1254: if(( unsigned int) _temp1252 > 4u?(( struct _tunion_struct*) _temp1252)->tag
== Cyc_Absyn_TunionFieldType_tag: 0){ _LL1261: _temp1260=( struct Cyc_Absyn_TunionFieldInfo)((
struct Cyc_Absyn_TunionFieldType_struct*) _temp1252)->f1; goto _LL1255;} else{
goto _LL1256;} _LL1256: if(( unsigned int) _temp1252 > 4u?(( struct
_tunion_struct*) _temp1252)->tag == Cyc_Absyn_TunionType_tag: 0){ _LL1263:
_temp1262=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp1252)->f1; _LL1271: _temp1270=( struct _tuple1*) _temp1262.name; goto
_LL1269; _LL1269: _temp1268=( struct Cyc_List_List*) _temp1262.targs; goto
_LL1267; _LL1267: _temp1266=( void*) _temp1262.rgn; goto _LL1265; _LL1265:
_temp1264=( struct Cyc_Absyn_Tuniondecl*) _temp1262.tud; goto _LL1257;} else{
goto _LL1258;} _LL1258: goto _LL1259; _LL1255: Cyc_Tcutil_unify(({ void**
_temp1272= topt; if( _temp1272 == 0){ _throw( Null_Exception);}* _temp1272;}),
res); goto _LL1253; _LL1257:{ struct Cyc_List_List* a= all_typs; for( 0; a != 0?
_temp1268 != 0: 0; a=({ struct Cyc_List_List* _temp1273= a; if( _temp1273 == 0){
_throw( Null_Exception);} _temp1273->tl;}), _temp1268=({ struct Cyc_List_List*
_temp1274= _temp1268; if( _temp1274 == 0){ _throw( Null_Exception);} _temp1274->tl;})){
Cyc_Tcutil_unify(( void*)({ struct Cyc_List_List* _temp1275= a; if( _temp1275 ==
0){ _throw( Null_Exception);} _temp1275->hd;}),( void*)({ struct Cyc_List_List*
_temp1276= _temp1268; if( _temp1276 == 0){ _throw( Null_Exception);} _temp1276->hd;}));}}
if( ef->typs == 0? es == 0: 0){ e->topt=({ struct Cyc_Core_Opt* _temp1277=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1277->v=(
void*) res; _temp1277;}); res=( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp1278=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp1278[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp1279; _temp1279.tag=
Cyc_Absyn_TunionType_tag; _temp1279.f1=({ struct Cyc_Absyn_TunionInfo _temp1280;
_temp1280.name=( struct _tuple1*)({ struct Cyc_Core_Opt* _temp1281= ed->name;
if( _temp1281 == 0){ _throw( Null_Exception);} _temp1281->v;}); _temp1280.targs=
all_typs; _temp1280.rgn=( void*) Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind);
_temp1280.tud=( struct Cyc_Absyn_Tuniondecl*) ed; _temp1280;}); _temp1279;});
_temp1278;}); Cyc_Tcutil_unchecked_cast( te, e, res); return res;} goto _LL1253;
_LL1259: goto _LL1253; _LL1253:;}{ struct Cyc_List_List* ts= ef->typs; for( 0;
es != 0? ts != 0: 0; es=({ struct Cyc_List_List* _temp1282= es; if( _temp1282 ==
0){ _throw( Null_Exception);} _temp1282->tl;}), ts=({ struct Cyc_List_List*
_temp1283= ts; if( _temp1283 == 0){ _throw( Null_Exception);} _temp1283->tl;})){
struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1284= es; if( _temp1284 == 0){ _throw( Null_Exception);} _temp1284->hd;});
void* t= Cyc_Tcutil_substitute( inst,(*(( struct _tuple7*)({ struct Cyc_List_List*
_temp1285= ts; if( _temp1285 == 0){ _throw( Null_Exception);} _temp1285->hd;}))).f2);
Cyc_Tcexp_tcExpInitializer( te,( void**)& t, e); if( ! Cyc_Tcutil_coerce_arg( te,
e, t)){ Cyc_Tcutil_terr( e->loc,({ struct _tagged_string _temp1289= Cyc_Absynpp_qvar2string(
ef->name); struct _tagged_string _temp1290= Cyc_Absynpp_typ2string( t); struct
_tagged_string _temp1291= e->topt == 0?( struct _tagged_string)({ char*
_temp1286=( char*)"?"; struct _tagged_string _temp1287; _temp1287.curr=
_temp1286; _temp1287.base= _temp1286; _temp1287.last_plus_one= _temp1286 + 2;
_temp1287;}): Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt* _temp1288=
e->topt; if( _temp1288 == 0){ _throw( Null_Exception);} _temp1288->v;}));
xprintf("tunion constructor %.*s expects argument of type %.*s but this argument has type %.*s",
_temp1289.last_plus_one - _temp1289.curr, _temp1289.curr, _temp1290.last_plus_one
- _temp1290.curr, _temp1290.curr, _temp1291.last_plus_one - _temp1291.curr,
_temp1291.curr);}));}} if( es != 0){ return Cyc_Tcexp_expr_err( te,(( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1292= es; if( _temp1292 == 0){ _throw( Null_Exception);}
_temp1292->hd;}))->loc,({ struct _tagged_string _temp1293= Cyc_Absynpp_qvar2string(
ef->name); xprintf("too many arguments for tunion constructor %.*s", _temp1293.last_plus_one
- _temp1293.curr, _temp1293.curr);}));} if( ts != 0){ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp1294= Cyc_Absynpp_qvar2string( ef->name);
xprintf("too few arguments for tunion constructor %.*s", _temp1294.last_plus_one
- _temp1294.curr, _temp1294.curr);}));} return res;}}} static void* Cyc_Tcexp_tcXTunion(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Core_Opt** exist_ref, struct Cyc_List_List* es, struct Cyc_Absyn_XTuniondecl*
xd, struct Cyc_Absyn_Tunionfield* ef){ struct Cyc_List_List* exist_inst=((
struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_Absyn_Tvar*), struct
Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_make_inst_var, ef->tvs); struct Cyc_List_List*
exist_typs=(( struct Cyc_List_List*(*)( void*(* f)( struct _tuple4*), struct Cyc_List_List*
x)) Cyc_List_map)(( void*(*)( struct _tuple4*)) Cyc_Core_snd, exist_inst);
struct Cyc_Absyn_XTunionFieldType_struct* res=({ struct Cyc_Absyn_XTunionFieldType_struct*
_temp1295=( struct Cyc_Absyn_XTunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_XTunionFieldType_struct)); _temp1295[ 0]=({ struct Cyc_Absyn_XTunionFieldType_struct
_temp1296; _temp1296.tag= Cyc_Absyn_XTunionFieldType_tag; _temp1296.f1=({ struct
Cyc_Absyn_XTunionFieldInfo _temp1297; _temp1297.name= xd->name; _temp1297.fname=
ef->name; _temp1297.xtud=( struct Cyc_Absyn_XTuniondecl*) xd; _temp1297.xtufd=(
struct Cyc_Absyn_Tunionfield*) ef; _temp1297;}); _temp1296;}); _temp1295;});
struct Cyc_List_List* ts= ef->typs;* exist_ref=({ struct Cyc_Core_Opt* _temp1298=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1298->v=(
void*) exist_typs; _temp1298;}); while( es != 0? ts != 0: 0) { struct Cyc_Absyn_Exp*
e=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1299= es; if( _temp1299
== 0){ _throw( Null_Exception);} _temp1299->hd;}); void* t= Cyc_Tcutil_substitute(
exist_inst,(*(( struct _tuple7*)({ struct Cyc_List_List* _temp1300= ts; if(
_temp1300 == 0){ _throw( Null_Exception);} _temp1300->hd;}))).f2); Cyc_Tcexp_tcExpInitializer(
te,( void**)& t, e); es=({ struct Cyc_List_List* _temp1301= es; if( _temp1301 ==
0){ _throw( Null_Exception);} _temp1301->tl;}); ts=({ struct Cyc_List_List*
_temp1302= ts; if( _temp1302 == 0){ _throw( Null_Exception);} _temp1302->tl;});
if( ! Cyc_Tcutil_coerce_arg( te, e, t)){ Cyc_Tcutil_terr( e->loc,({ struct
_tagged_string _temp1304= Cyc_Absynpp_qvar2string( ef->name); struct
_tagged_string _temp1305= Cyc_Absynpp_typ2string( t); struct _tagged_string
_temp1306= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt* _temp1303= e->topt;
if( _temp1303 == 0){ _throw( Null_Exception);} _temp1303->v;})); xprintf("xtunion constructor %.*s expects argument of type %.*s but this argument has type %.*s",
_temp1304.last_plus_one - _temp1304.curr, _temp1304.curr, _temp1305.last_plus_one
- _temp1305.curr, _temp1305.curr, _temp1306.last_plus_one - _temp1306.curr,
_temp1306.curr);}));}} if( es != 0){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1307= es; if( _temp1307 == 0){ _throw( Null_Exception);}
_temp1307->hd;}))->loc,({ struct _tagged_string _temp1308= Cyc_Absynpp_qvar2string(
ef->name); xprintf("too many arguments for xtunion constructor %.*s", _temp1308.last_plus_one
- _temp1308.curr, _temp1308.curr);}));} else{ if( ts != 0){ Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1309= es; if( _temp1309 == 0){
_throw( Null_Exception);} _temp1309->hd;}))->loc,({ struct _tagged_string
_temp1310= Cyc_Absynpp_qvar2string( ef->name); xprintf("too few arguments for xtunion constructor %.*s",
_temp1310.last_plus_one - _temp1310.curr, _temp1310.curr);}));}} return( void*)
res;} static void* Cyc_Tcexp_tcMalloc( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* ropt, void* t){ void* rgn=( void*) Cyc_Absyn_HeapRgn;
if( ropt != 0){ void* expected_type=( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp1311=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp1311[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp1312; _temp1312.tag= Cyc_Absyn_RgnHandleType_tag; _temp1312.f1=( void*) Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind); _temp1312;}); _temp1311;}); void* handle_type= Cyc_Tcexp_tcExp(
te,( void**)& expected_type,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1313= ropt; if( _temp1313 == 0){ _throw( Null_Exception);} _temp1313;}));
void* _temp1314= Cyc_Tcutil_compress( handle_type); void* _temp1320; _LL1316:
if(( unsigned int) _temp1314 > 4u?(( struct _tunion_struct*) _temp1314)->tag ==
Cyc_Absyn_RgnHandleType_tag: 0){ _LL1321: _temp1320=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1314)->f1; goto _LL1317;} else{ goto _LL1318;} _LL1318: goto _LL1319;
_LL1317: rgn= _temp1320; Cyc_Tcenv_check_rgn_accessible( te, loc, rgn); goto
_LL1315; _LL1319: Cyc_Tcutil_terr(({ struct Cyc_Absyn_Exp* _temp1322= ropt; if(
_temp1322 == 0){ _throw( Null_Exception);} _temp1322->loc;}),({ struct
_tagged_string _temp1323= Cyc_Absynpp_typ2string( handle_type); xprintf("expecting region_t type but found %.*s",
_temp1323.last_plus_one - _temp1323.curr, _temp1323.curr);})); goto _LL1315;
_LL1315:;} Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars( te),(
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
_temp1403=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp1403[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp1404; _temp1404.tag= Cyc_Absyn_RgnHandleType_tag; _temp1404.f1=( void*) Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind); _temp1404;}); _temp1403;}); void* handle_type= Cyc_Tcexp_tcExp(
te,( void**)& expected_type,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1405= rgn_handle; if( _temp1405 == 0){ _throw( Null_Exception);} _temp1405;}));
void* _temp1406= Cyc_Tcutil_compress( handle_type); void* _temp1412; _LL1408:
if(( unsigned int) _temp1406 > 4u?(( struct _tunion_struct*) _temp1406)->tag ==
Cyc_Absyn_RgnHandleType_tag: 0){ _LL1413: _temp1412=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1406)->f1; goto _LL1409;} else{ goto _LL1410;} _LL1410: goto _LL1411;
_LL1409: rgn= _temp1412; Cyc_Tcenv_check_rgn_accessible( te, loc, rgn); goto
_LL1407; _LL1411: Cyc_Tcutil_terr(({ struct Cyc_Absyn_Exp* _temp1414= rgn_handle;
if( _temp1414 == 0){ _throw( Null_Exception);} _temp1414->loc;}),({ struct
_tagged_string _temp1415= Cyc_Absynpp_typ2string( handle_type); xprintf("expecting region_t type but found %.*s",
_temp1415.last_plus_one - _temp1415.curr, _temp1415.curr);})); goto _LL1407;
_LL1407:;}{ void* _temp1416=( void*) e1->r; struct Cyc_Absyn_Exp* _temp1428;
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
_temp1430)?( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1456=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1456[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1457; _temp1457.tag= Cyc_Absyn_Upper_b_tag;
_temp1457.f1= _temp1430; _temp1457;}); _temp1456;}):( void*) Cyc_Absyn_Unknown_b;
void* res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1458=( struct
Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1458[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1459; _temp1459.tag=
Cyc_Absyn_PointerType_tag; _temp1459.f1=({ struct Cyc_Absyn_PtrInfo _temp1460;
_temp1460.elt_typ=( void*) _temp1454; _temp1460.rgn_typ=( void*) rgn; _temp1460.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp1460.tq=
_temp1452; _temp1460.bounds=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
b); _temp1460;}); _temp1459;}); _temp1458;}); if( topt != 0){ if( ! Cyc_Tcutil_unify(({
void** _temp1461= topt; if( _temp1461 == 0){ _throw( Null_Exception);}*
_temp1461;}), res_typ)? Cyc_Tcutil_silent_castable( te, loc, res_typ,({ void**
_temp1462= topt; if( _temp1462 == 0){ _throw( Null_Exception);}* _temp1462;})):
0){ e->topt=({ struct Cyc_Core_Opt* _temp1463=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1463->v=( void*) res_typ; _temp1463;}); Cyc_Tcutil_unchecked_cast(
te, e,({ void** _temp1464= topt; if( _temp1464 == 0){ _throw( Null_Exception);}*
_temp1464;})); res_typ=({ void** _temp1465= topt; if( _temp1465 == 0){ _throw(
Null_Exception);}* _temp1465;});}} return res_typ;} _LL1449: return(( void*(*)(
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
_temp1543=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1543[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1544; _temp1544.tag=
Cyc_Absyn_PointerType_tag; _temp1544.f1=({ struct Cyc_Absyn_PtrInfo _temp1545;
_temp1545.elt_typ=( void*) telt; _temp1545.rgn_typ=( void*) rgn; _temp1545.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp1545.tq= Cyc_Absyn_empty_tqual();
_temp1545.bounds=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1546=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1546[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1547; _temp1547.tag= Cyc_Absyn_Upper_b_tag;
_temp1547.f1= Cyc_Absyn_uint_exp( 1, 0); _temp1547;}); _temp1546;})); _temp1545;});
_temp1544;}); _temp1543;}); if( topt != 0){ if( ! Cyc_Tcutil_unify(({ void**
_temp1548= topt; if( _temp1548 == 0){ _throw( Null_Exception);}* _temp1548;}),
res_typ)? Cyc_Tcutil_silent_castable( te, loc, res_typ,({ void** _temp1549= topt;
if( _temp1549 == 0){ _throw( Null_Exception);}* _temp1549;})): 0){ e->topt=({
struct Cyc_Core_Opt* _temp1550=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1550->v=( void*) res_typ; _temp1550;}); Cyc_Tcutil_unchecked_cast(
te, e,({ void** _temp1551= topt; if( _temp1551 == 0){ _throw( Null_Exception);}*
_temp1551;})); res_typ=({ void** _temp1552= topt; if( _temp1552 == 0){ _throw(
Null_Exception);}* _temp1552;});}} return res_typ;}} _LL1417:;}} void* Cyc_Tcexp_tcExp(
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
_temp1668=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp1668[ 0]=({ struct Cyc_Absyn_FnType_struct _temp1669; _temp1669.tag= Cyc_Absyn_FnType_tag;
_temp1669.f1=({ struct Cyc_Absyn_FnInfo _temp1670; _temp1670.tvars= 0; _temp1670.effect=
_temp1664; _temp1670.ret_typ=( void*) _temp1662; _temp1670.args= _temp1660;
_temp1670.varargs= _temp1658; _temp1670.attributes= _temp1656; _temp1670;});
_temp1669;}); _temp1668;})); void* new_typ=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp1671=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1671[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1672; _temp1672.tag=
Cyc_Absyn_PointerType_tag; _temp1672.f1=({ struct Cyc_Absyn_PtrInfo _temp1673;
_temp1673.elt_typ=( void*) ftyp; _temp1673.rgn_typ=( void*) _temp1644; _temp1673.nullable=
_temp1642; _temp1673.tq= _temp1640; _temp1673.bounds= _temp1638; _temp1673;});
_temp1672;}); _temp1671;}); struct Cyc_Absyn_Exp* inner= Cyc_Absyn_copy_exp( e);(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Instantiate_e_struct*
_temp1674=( struct Cyc_Absyn_Instantiate_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Instantiate_e_struct)); _temp1674[ 0]=({ struct Cyc_Absyn_Instantiate_e_struct
_temp1675; _temp1675.tag= Cyc_Absyn_Instantiate_e_tag; _temp1675.f1= inner;
_temp1675.f2= ts; _temp1675;}); _temp1674;}))); e->topt=({ struct Cyc_Core_Opt*
_temp1676=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1676->v=( void*) new_typ; _temp1676;});} goto _LL1649; _LL1653: goto
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