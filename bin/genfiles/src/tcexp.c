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
goto _LL232; _LL238: goto _LL232; _LL232:;} return t;} _LL170: t=( void*)({
struct Cyc_Absyn_PointerType_struct* _temp264=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp264[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp265; _temp265.tag= Cyc_Absyn_PointerType_tag;
_temp265.f1=({ struct Cyc_Absyn_PtrInfo _temp266; _temp266.elt_typ=( void*) Cyc_Absyn_new_evar((
void*) Cyc_Absyn_MemKind); _temp266.rgn_typ=( void*) Cyc_Absyn_new_evar(( void*)
Cyc_Absyn_RgnKind); _temp266.nullable=(( struct Cyc_Absyn_Conref*(*)( int x))
Cyc_Absyn_new_conref)( 1); _temp266.tq= Cyc_Absyn_empty_tqual(); _temp266.bounds=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp266;}); _temp265;});
_temp264;}); goto _LL156; _LL156:;} return t;} static void* Cyc_Tcexp_tcVar(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct _tuple1* q,
void* b){ void* _temp267= b; struct Cyc_Absyn_Vardecl* _temp281; struct Cyc_Absyn_Fndecl*
_temp283; struct Cyc_Absyn_Vardecl* _temp285; struct Cyc_Absyn_Vardecl* _temp287;
struct Cyc_Absyn_Vardecl* _temp289; _LL269: if(( int) _temp267 == Cyc_Absyn_Unresolved_b){
goto _LL270;} else{ goto _LL271;} _LL271: if(( unsigned int) _temp267 > 1u?((
struct _tunion_struct*) _temp267)->tag == Cyc_Absyn_Global_b_tag: 0){ _LL282:
_temp281=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Global_b_struct*)
_temp267)->f1; goto _LL272;} else{ goto _LL273;} _LL273: if(( unsigned int)
_temp267 > 1u?(( struct _tunion_struct*) _temp267)->tag == Cyc_Absyn_Funname_b_tag:
0){ _LL284: _temp283=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Funname_b_struct*)
_temp267)->f1; goto _LL274;} else{ goto _LL275;} _LL275: if(( unsigned int)
_temp267 > 1u?(( struct _tunion_struct*) _temp267)->tag == Cyc_Absyn_Pat_b_tag:
0){ _LL286: _temp285=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Pat_b_struct*)
_temp267)->f1; goto _LL276;} else{ goto _LL277;} _LL277: if(( unsigned int)
_temp267 > 1u?(( struct _tunion_struct*) _temp267)->tag == Cyc_Absyn_Local_b_tag:
0){ _LL288: _temp287=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Local_b_struct*)
_temp267)->f1; goto _LL278;} else{ goto _LL279;} _LL279: if(( unsigned int)
_temp267 > 1u?(( struct _tunion_struct*) _temp267)->tag == Cyc_Absyn_Param_b_tag:
0){ _LL290: _temp289=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Param_b_struct*)
_temp267)->f1; goto _LL280;} else{ goto _LL268;} _LL270: return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp291= Cyc_Absynpp_qvar2string( q); xprintf("unbound identifier %.*s",
_temp291.last_plus_one - _temp291.curr, _temp291.curr);})); _LL272:({ struct
_tuple1* _temp292= q; unsigned int _temp293= 0; if( _temp293 >= 1u){ _throw(
Null_Exception);} _temp292[ _temp293]=({ struct _tuple1* _temp294= _temp281->name;
unsigned int _temp295= 0; if( _temp295 >= 1u){ _throw( Null_Exception);}
_temp294[ _temp295];});}); return( void*) _temp281->type; _LL274:({ struct
_tuple1* _temp296= q; unsigned int _temp297= 0; if( _temp297 >= 1u){ _throw(
Null_Exception);} _temp296[ _temp297]=({ struct _tuple1* _temp298= _temp283->name;
unsigned int _temp299= 0; if( _temp299 >= 1u){ _throw( Null_Exception);}
_temp298[ _temp299];});}); return Cyc_Tcutil_fndecl2typ( _temp283); _LL276:
_temp287= _temp285; goto _LL278; _LL278: _temp289= _temp287; goto _LL280; _LL280:(*
q).f1=( void*) Cyc_Absyn_Loc_n; return( void*) _temp289->type; _LL268:;} static
void Cyc_Tcexp_check_format_args( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
fmt, struct Cyc_List_List* args, struct Cyc_List_List*(* type_getter)( struct
_tagged_string, struct Cyc_Position_Segment*)){ struct Cyc_List_List* desc_types;{
void* _temp300=( void*) fmt->r; void* _temp306; struct _tagged_string _temp308;
_LL302: if((( struct _tunion_struct*) _temp300)->tag == Cyc_Absyn_Const_e_tag){
_LL307: _temp306=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp300)->f1; if((
unsigned int) _temp306 > 1u?(( struct _tunion_struct*) _temp306)->tag == Cyc_Absyn_String_c_tag:
0){ _LL309: _temp308=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp306)->f1; goto _LL303;} else{ goto _LL304;}} else{ goto _LL304;} _LL304:
goto _LL305; _LL303: desc_types= type_getter( _temp308, fmt->loc); goto _LL301;
_LL305: Cyc_Tcutil_terr( fmt->loc,( struct _tagged_string)({ char* _temp310=(
char*)"expecting a literal format string"; struct _tagged_string _temp311;
_temp311.curr= _temp310; _temp311.base= _temp310; _temp311.last_plus_one=
_temp310 + 34; _temp311;})); return; _LL301:;} for( 0; desc_types != 0? args !=
0: 0; desc_types=({ struct Cyc_List_List* _temp312= desc_types; if( _temp312 ==
0){ _throw( Null_Exception);} _temp312->tl;}), args=({ struct Cyc_List_List*
_temp313= args; if( _temp313 == 0){ _throw( Null_Exception);} _temp313->tl;})){
void* t=( void*)({ struct Cyc_List_List* _temp314= desc_types; if( _temp314 == 0){
_throw( Null_Exception);} _temp314->hd;}); struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp315= args; if( _temp315 == 0){ _throw( Null_Exception);}
_temp315->hd;}); if( ! Cyc_Tcutil_coerce_arg( te, e, t)){ Cyc_Tcutil_terr( e->loc,({
struct _tagged_string _temp317= Cyc_Absynpp_typ2string( t); struct
_tagged_string _temp318= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp316= e->topt; if( _temp316 == 0){ _throw( Null_Exception);} _temp316->v;}));
xprintf("descriptor has type %.*s but argument has type %.*s", _temp317.last_plus_one
- _temp317.curr, _temp317.curr, _temp318.last_plus_one - _temp318.curr, _temp318.curr);}));}}
if( desc_types != 0){ Cyc_Tcutil_terr( fmt->loc,( struct _tagged_string)({ char*
_temp319=( char*)"too few arguments"; struct _tagged_string _temp320; _temp320.curr=
_temp319; _temp320.base= _temp319; _temp320.last_plus_one= _temp319 + 18;
_temp320;}));} if( args != 0){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp321= args; if( _temp321 == 0){ _throw( Null_Exception);}
_temp321->hd;}))->loc,( struct _tagged_string)({ char* _temp322=( char*)"too many arguments";
struct _tagged_string _temp323; _temp323.curr= _temp322; _temp323.base= _temp322;
_temp323.last_plus_one= _temp322 + 19; _temp323;}));}} static void* Cyc_Tcexp_tcUnPrimop(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, void*
p, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp324= e->topt; if( _temp324 == 0){ _throw( Null_Exception);} _temp324->v;}));
void* _temp325= p; _LL327: if(( int) _temp325 == Cyc_Absyn_Plus){ goto _LL328;}
else{ goto _LL329;} _LL329: if(( int) _temp325 == Cyc_Absyn_Minus){ goto _LL330;}
else{ goto _LL331;} _LL331: if(( int) _temp325 == Cyc_Absyn_Not){ goto _LL332;}
else{ goto _LL333;} _LL333: if(( int) _temp325 == Cyc_Absyn_Bitnot){ goto _LL334;}
else{ goto _LL335;} _LL335: if(( int) _temp325 == Cyc_Absyn_Size){ goto _LL336;}
else{ goto _LL337;} _LL337: goto _LL338; _LL328: goto _LL330; _LL330: if( ! Cyc_Tcutil_is_numeric(
e)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp339= Cyc_Absynpp_typ2string(
t); xprintf("expecting arithmetic type but found %.*s", _temp339.last_plus_one -
_temp339.curr, _temp339.curr);}));} return( void*)({ struct Cyc_Core_Opt*
_temp340= e->topt; if( _temp340 == 0){ _throw( Null_Exception);} _temp340->v;});
_LL332: Cyc_Tcutil_check_contains_assign( e); if( ! Cyc_Tcutil_coerce_to_bool(
te, e)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp341= Cyc_Absynpp_typ2string(
t); xprintf("expecting integral or * type but found %.*s", _temp341.last_plus_one
- _temp341.curr, _temp341.curr);}));} return Cyc_Absyn_sint_t; _LL334: if( ! Cyc_Tcutil_is_integral(
e)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp342= Cyc_Absynpp_typ2string(
t); xprintf("expecting integral type but found %.*s", _temp342.last_plus_one -
_temp342.curr, _temp342.curr);}));} return( void*)({ struct Cyc_Core_Opt*
_temp343= e->topt; if( _temp343 == 0){ _throw( Null_Exception);} _temp343->v;});
_LL336:{ void* _temp344= t; struct Cyc_Absyn_Exp* _temp352; struct Cyc_Absyn_Tqual*
_temp354; void* _temp356; struct Cyc_Absyn_PtrInfo _temp358; struct Cyc_Absyn_Conref*
_temp360; struct Cyc_Absyn_Tqual* _temp362; struct Cyc_Absyn_Conref* _temp364;
void* _temp366; void* _temp368; _LL346: if(( unsigned int) _temp344 > 4u?((
struct _tunion_struct*) _temp344)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL357:
_temp356=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp344)->f1; goto
_LL355; _LL355: _temp354=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp344)->f2; goto _LL353; _LL353: _temp352=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp344)->f3; goto _LL347;} else{ goto _LL348;}
_LL348: if(( unsigned int) _temp344 > 4u?(( struct _tunion_struct*) _temp344)->tag
== Cyc_Absyn_PointerType_tag: 0){ _LL359: _temp358=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp344)->f1; _LL369: _temp368=( void*)
_temp358.elt_typ; goto _LL367; _LL367: _temp366=( void*) _temp358.rgn_typ; goto
_LL365; _LL365: _temp364=( struct Cyc_Absyn_Conref*) _temp358.nullable; goto
_LL363; _LL363: _temp362=( struct Cyc_Absyn_Tqual*) _temp358.tq; goto _LL361;
_LL361: _temp360=( struct Cyc_Absyn_Conref*) _temp358.bounds; goto _LL349;}
else{ goto _LL350;} _LL350: goto _LL351; _LL347: goto _LL345; _LL349: goto
_LL345; _LL351: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp370= Cyc_Absynpp_typ2string(
t); xprintf("size requires pointer or array type, not %.*s", _temp370.last_plus_one
- _temp370.curr, _temp370.curr);})); goto _LL345; _LL345:;} return Cyc_Absyn_uint_t;
_LL338: return(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ char* _temp371=( char*)"Non-unary primop"; struct
_tagged_string _temp372; _temp372.curr= _temp371; _temp372.base= _temp371;
_temp372.last_plus_one= _temp371 + 17; _temp372;})); _LL326:;} static void* Cyc_Tcexp_tcArithBinop(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2,
int(* checker)( struct Cyc_Absyn_Exp*)){ if( ! checker( e1)){ Cyc_Tcutil_terr(
e1->loc,({ struct _tagged_string _temp374= Cyc_Absynpp_typ2string(( void*)({
struct Cyc_Core_Opt* _temp373= e1->topt; if( _temp373 == 0){ _throw(
Null_Exception);} _temp373->v;})); xprintf("type %.*s cannot be used here",
_temp374.last_plus_one - _temp374.curr, _temp374.curr);})); return Cyc_Absyn_wildtyp();}
if( ! checker( e2)){ Cyc_Tcutil_terr( e2->loc,({ struct _tagged_string _temp376=
Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt* _temp375= e2->topt; if(
_temp375 == 0){ _throw( Null_Exception);} _temp375->v;})); xprintf("type %.*s cannot be used here",
_temp376.last_plus_one - _temp376.curr, _temp376.curr);})); return Cyc_Absyn_wildtyp();}{
void* t1= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp377= e1->topt;
if( _temp377 == 0){ _throw( Null_Exception);} _temp377->v;})); void* t2= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp378= e2->topt; if( _temp378 == 0){ _throw(
Null_Exception);} _temp378->v;})); return Cyc_Tcutil_max_arithmetic_type( t1, t2);}}
static void* Cyc_Tcexp_tcPlus( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ void* t1=( void*)({ struct Cyc_Core_Opt* _temp379=
e1->topt; if( _temp379 == 0){ _throw( Null_Exception);} _temp379->v;}); if( Cyc_Tcutil_is_tagged_pointer_typ(
t1)){ if( ! Cyc_Tcutil_coerce_sint_typ( te, e2)){ Cyc_Tcutil_terr( e2->loc,({
struct _tagged_string _temp381= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp380= e2->topt; if( _temp380 == 0){ _throw( Null_Exception);} _temp380->v;}));
xprintf("expecting int but found %.*s", _temp381.last_plus_one - _temp381.curr,
_temp381.curr);}));} return t1;} else{ return Cyc_Tcexp_tcArithBinop( te, e1, e2,
Cyc_Tcutil_is_numeric);}} static void* Cyc_Tcexp_tcMinus( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ void* t1=( void*)({
struct Cyc_Core_Opt* _temp382= e1->topt; if( _temp382 == 0){ _throw(
Null_Exception);} _temp382->v;}); void* t2=( void*)({ struct Cyc_Core_Opt*
_temp383= e2->topt; if( _temp383 == 0){ _throw( Null_Exception);} _temp383->v;});
if( Cyc_Tcutil_is_tagged_pointer_typ( t1)){ if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){ if( ! Cyc_Tcutil_unify( t1, t2)){ Cyc_Tcutil_terr( e1->loc,({ struct
_tagged_string _temp386= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp384= e1->topt; if( _temp384 == 0){ _throw( Null_Exception);} _temp384->v;}));
struct _tagged_string _temp387= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp385= e2->topt; if( _temp385 == 0){ _throw( Null_Exception);} _temp385->v;}));
xprintf("pointer arithmetic on values of different types (%.*s != %.*s)",
_temp386.last_plus_one - _temp386.curr, _temp386.curr, _temp387.last_plus_one -
_temp387.curr, _temp387.curr);}));} return Cyc_Absyn_sint_t;} else{ if( ! Cyc_Tcutil_coerce_sint_typ(
te, e2)){ Cyc_Tcutil_terr( e2->loc,({ struct _tagged_string _temp388= Cyc_Absynpp_typ2string(
t1); struct _tagged_string _temp389= Cyc_Absynpp_typ2string( t2); xprintf("expecting either %.*s or int but found %.*s",
_temp388.last_plus_one - _temp388.curr, _temp388.curr, _temp389.last_plus_one -
_temp389.curr, _temp389.curr);}));} return t1;}} else{ return Cyc_Tcexp_tcArithBinop(
te, e1, e2, Cyc_Tcutil_is_numeric);}} static void* Cyc_Tcexp_tcAnyBinop( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2){ int e1_is_num= Cyc_Tcutil_is_numeric( e1); int
e2_is_num= Cyc_Tcutil_is_numeric( e2); void* t1= Cyc_Tcutil_compress(( void*)({
struct Cyc_Core_Opt* _temp390= e1->topt; if( _temp390 == 0){ _throw(
Null_Exception);} _temp390->v;})); void* t2= Cyc_Tcutil_compress(( void*)({
struct Cyc_Core_Opt* _temp391= e2->topt; if( _temp391 == 0){ _throw(
Null_Exception);} _temp391->v;})); if( e1_is_num? e2_is_num: 0){ return Cyc_Absyn_sint_t;}
else{ if( Cyc_Tcutil_unify( t1, t2)? Cyc_Tcutil_unify( t1, Cyc_Absyn_new_evar((
void*) Cyc_Absyn_BoxKind)): 0){ return Cyc_Absyn_sint_t;} else{ if( Cyc_Tcutil_silent_castable(
te, loc, t2, t1)){ Cyc_Tcutil_unchecked_cast( te, e2, t1); return Cyc_Absyn_sint_t;}
else{ if( Cyc_Tcutil_silent_castable( te, loc, t1, t2)){ Cyc_Tcutil_unchecked_cast(
te, e1, t2); return Cyc_Absyn_sint_t;} else{ void* t1=( void*)({ struct Cyc_Core_Opt*
_temp392= e1->topt; if( _temp392 == 0){ _throw( Null_Exception);} _temp392->v;});
void* t2=( void*)({ struct Cyc_Core_Opt* _temp393= e2->topt; if( _temp393 == 0){
_throw( Null_Exception);} _temp393->v;});{ struct _tuple0 _temp395=({ struct
_tuple0 _temp394; _temp394.f1= Cyc_Tcutil_compress( t1); _temp394.f2= Cyc_Tcutil_compress(
t2); _temp394;}); void* _temp401; struct Cyc_Absyn_PtrInfo _temp403; struct Cyc_Absyn_Conref*
_temp405; struct Cyc_Absyn_Tqual* _temp407; struct Cyc_Absyn_Conref* _temp409;
void* _temp411; void* _temp413; void* _temp415; struct Cyc_Absyn_PtrInfo
_temp417; struct Cyc_Absyn_Conref* _temp419; struct Cyc_Absyn_Tqual* _temp421;
struct Cyc_Absyn_Conref* _temp423; void* _temp425; void* _temp427; _LL397:
_LL416: _temp415= _temp395.f1; if(( unsigned int) _temp415 > 4u?(( struct
_tunion_struct*) _temp415)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL418:
_temp417=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp415)->f1; _LL428: _temp427=( void*) _temp417.elt_typ; goto _LL426; _LL426:
_temp425=( void*) _temp417.rgn_typ; goto _LL424; _LL424: _temp423=( struct Cyc_Absyn_Conref*)
_temp417.nullable; goto _LL422; _LL422: _temp421=( struct Cyc_Absyn_Tqual*)
_temp417.tq; goto _LL420; _LL420: _temp419=( struct Cyc_Absyn_Conref*) _temp417.bounds;
goto _LL402;} else{ goto _LL399;} _LL402: _temp401= _temp395.f2; if((
unsigned int) _temp401 > 4u?(( struct _tunion_struct*) _temp401)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL404: _temp403=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp401)->f1; _LL414: _temp413=( void*) _temp403.elt_typ; goto _LL412; _LL412:
_temp411=( void*) _temp403.rgn_typ; goto _LL410; _LL410: _temp409=( struct Cyc_Absyn_Conref*)
_temp403.nullable; goto _LL408; _LL408: _temp407=( struct Cyc_Absyn_Tqual*)
_temp403.tq; goto _LL406; _LL406: _temp405=( struct Cyc_Absyn_Conref*) _temp403.bounds;
goto _LL398;} else{ goto _LL399;} _LL399: goto _LL400; _LL398: if( Cyc_Tcutil_unify(
_temp427, _temp413)){ return Cyc_Absyn_sint_t;} goto _LL396; _LL400: goto _LL396;
_LL396:;} Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp429= Cyc_Absynpp_typ2string(
t1); struct _tagged_string _temp430= Cyc_Absynpp_typ2string( t2); xprintf("comparison not allowed between %.*s and %.*s",
_temp429.last_plus_one - _temp429.curr, _temp429.curr, _temp430.last_plus_one -
_temp430.curr, _temp430.curr);})); return Cyc_Absyn_wildtyp();}}}}} static void*
Cyc_Tcexp_tcBinPrimop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* p, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){
void* _temp431= p; _LL433: if(( int) _temp431 == Cyc_Absyn_Plus){ goto _LL434;}
else{ goto _LL435;} _LL435: if(( int) _temp431 == Cyc_Absyn_Minus){ goto _LL436;}
else{ goto _LL437;} _LL437: if(( int) _temp431 == Cyc_Absyn_Times){ goto _LL438;}
else{ goto _LL439;} _LL439: if(( int) _temp431 == Cyc_Absyn_Div){ goto _LL440;}
else{ goto _LL441;} _LL441: if(( int) _temp431 == Cyc_Absyn_Mod){ goto _LL442;}
else{ goto _LL443;} _LL443: if(( int) _temp431 == Cyc_Absyn_Bitand){ goto _LL444;}
else{ goto _LL445;} _LL445: if(( int) _temp431 == Cyc_Absyn_Bitor){ goto _LL446;}
else{ goto _LL447;} _LL447: if(( int) _temp431 == Cyc_Absyn_Bitxor){ goto _LL448;}
else{ goto _LL449;} _LL449: if(( int) _temp431 == Cyc_Absyn_Bitlshift){ goto
_LL450;} else{ goto _LL451;} _LL451: if(( int) _temp431 == Cyc_Absyn_Bitlrshift){
goto _LL452;} else{ goto _LL453;} _LL453: if(( int) _temp431 == Cyc_Absyn_Bitarshift){
goto _LL454;} else{ goto _LL455;} _LL455: if(( int) _temp431 == Cyc_Absyn_Eq){
goto _LL456;} else{ goto _LL457;} _LL457: if(( int) _temp431 == Cyc_Absyn_Neq){
goto _LL458;} else{ goto _LL459;} _LL459: if(( int) _temp431 == Cyc_Absyn_Gt){
goto _LL460;} else{ goto _LL461;} _LL461: if(( int) _temp431 == Cyc_Absyn_Lt){
goto _LL462;} else{ goto _LL463;} _LL463: if(( int) _temp431 == Cyc_Absyn_Gte){
goto _LL464;} else{ goto _LL465;} _LL465: if(( int) _temp431 == Cyc_Absyn_Lte){
goto _LL466;} else{ goto _LL467;} _LL467: goto _LL468; _LL434: return Cyc_Tcexp_tcPlus(
te, e1, e2); _LL436: return Cyc_Tcexp_tcMinus( te, e1, e2); _LL438: goto _LL440;
_LL440: return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_is_numeric);
_LL442: goto _LL444; _LL444: goto _LL446; _LL446: goto _LL448; _LL448: goto
_LL450; _LL450: goto _LL452; _LL452: goto _LL454; _LL454: return Cyc_Tcexp_tcArithBinop(
te, e1, e2, Cyc_Tcutil_is_integral); _LL456: goto _LL458; _LL458: goto _LL460;
_LL460: goto _LL462; _LL462: goto _LL464; _LL464: goto _LL466; _LL466: return
Cyc_Tcexp_tcAnyBinop( te, loc, e1, e2); _LL468:( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Core_Failure_struct* _temp469=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp469[ 0]=({ struct Cyc_Core_Failure_struct
_temp470; _temp470.tag= Cyc_Core_Failure_tag; _temp470.f1=( struct
_tagged_string)({ char* _temp471=( char*)"bad binary primop"; struct
_tagged_string _temp472; _temp472.curr= _temp471; _temp472.base= _temp471;
_temp472.last_plus_one= _temp471 + 18; _temp472;}); _temp470;}); _temp469;}));
_LL432:;} static void* Cyc_Tcexp_tcPrimop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* p, struct Cyc_List_List* es){ if( p ==( void*) Cyc_Absyn_Minus?((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es) == 1: 0){ return Cyc_Tcexp_tcExp(
te, topt,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp473= es; if(
_temp473 == 0){ _throw( Null_Exception);} _temp473->hd;}));} Cyc_Tcexp_tcExpList(
te, es);{ void* t;{ void* _temp474= p; _LL476: if(( int) _temp474 == Cyc_Absyn_Printf){
goto _LL477;} else{ goto _LL478;} _LL478: if(( int) _temp474 == Cyc_Absyn_Fprintf){
goto _LL479;} else{ goto _LL480;} _LL480: if(( int) _temp474 == Cyc_Absyn_Xprintf){
goto _LL481;} else{ goto _LL482;} _LL482: if(( int) _temp474 == Cyc_Absyn_Scanf){
goto _LL483;} else{ goto _LL484;} _LL484: if(( int) _temp474 == Cyc_Absyn_Fscanf){
goto _LL485;} else{ goto _LL486;} _LL486: if(( int) _temp474 == Cyc_Absyn_Sscanf){
goto _LL487;} else{ goto _LL488;} _LL488: goto _LL489; _LL477: if( es == 0){
return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char* _temp490=(
char*)"missing format string in printf"; struct _tagged_string _temp491;
_temp491.curr= _temp490; _temp491.base= _temp490; _temp491.last_plus_one=
_temp490 + 32; _temp491;}));} Cyc_Tcexp_check_format_args( te,( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp492= es; if( _temp492 == 0){ _throw( Null_Exception);}
_temp492->hd;}),({ struct Cyc_List_List* _temp493= es; if( _temp493 == 0){
_throw( Null_Exception);} _temp493->tl;}), Cyc_Formatstr_get_format_typs); t=(
void*) Cyc_Absyn_VoidType; goto _LL475; _LL479: if( es == 0? 1:({ struct Cyc_List_List*
_temp494= es; if( _temp494 == 0){ _throw( Null_Exception);} _temp494->tl;}) == 0){
return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char* _temp495=(
char*)"too few arguments to fprintf"; struct _tagged_string _temp496; _temp496.curr=
_temp495; _temp496.base= _temp495; _temp496.last_plus_one= _temp495 + 29;
_temp496;}));} if( ! Cyc_Tcutil_unify(( void*)({ struct Cyc_Core_Opt* _temp498=((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp497= es; if( _temp497 == 0){
_throw( Null_Exception);} _temp497->hd;}))->topt; if( _temp498 == 0){ _throw(
Null_Exception);} _temp498->v;}), Cyc_Absyn_file_typ())){ Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp499= es; if( _temp499 == 0){
_throw( Null_Exception);} _temp499->hd;}))->loc,({ struct _tagged_string
_temp502= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt* _temp501=((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp500= es; if( _temp500 == 0){
_throw( Null_Exception);} _temp500->hd;}))->topt; if( _temp501 == 0){ _throw(
Null_Exception);} _temp501->v;})); xprintf("expecting FILE @ but found %.*s",
_temp502.last_plus_one - _temp502.curr, _temp502.curr);}));} Cyc_Tcexp_check_format_args(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp504=({ struct Cyc_List_List*
_temp503= es; if( _temp503 == 0){ _throw( Null_Exception);} _temp503->tl;}); if(
_temp504 == 0){ _throw( Null_Exception);} _temp504->hd;}),({ struct Cyc_List_List*
_temp506=({ struct Cyc_List_List* _temp505= es; if( _temp505 == 0){ _throw(
Null_Exception);} _temp505->tl;}); if( _temp506 == 0){ _throw( Null_Exception);}
_temp506->tl;}), Cyc_Formatstr_get_format_typs); t=( void*) Cyc_Absyn_VoidType;
goto _LL475; _LL481: if( es == 0){ return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_string)({ char* _temp507=( char*)"missing format string in xprintf";
struct _tagged_string _temp508; _temp508.curr= _temp507; _temp508.base= _temp507;
_temp508.last_plus_one= _temp507 + 33; _temp508;}));} Cyc_Tcexp_check_format_args(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp509= es; if( _temp509
== 0){ _throw( Null_Exception);} _temp509->hd;}),({ struct Cyc_List_List*
_temp510= es; if( _temp510 == 0){ _throw( Null_Exception);} _temp510->tl;}), Cyc_Formatstr_get_format_typs);
t= Cyc_Absyn_string_typ( Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind)); goto
_LL475; _LL483: if( es == 0){ return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_string)({ char* _temp511=( char*)"missing format string in scanf";
struct _tagged_string _temp512; _temp512.curr= _temp511; _temp512.base= _temp511;
_temp512.last_plus_one= _temp511 + 31; _temp512;}));} Cyc_Tcexp_check_format_args(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp513= es; if( _temp513
== 0){ _throw( Null_Exception);} _temp513->hd;}),({ struct Cyc_List_List*
_temp514= es; if( _temp514 == 0){ _throw( Null_Exception);} _temp514->tl;}), Cyc_Formatstr_get_scanf_typs);
t= Cyc_Absyn_sint_t; goto _LL475; _LL485: if( es == 0? 1:({ struct Cyc_List_List*
_temp515= es; if( _temp515 == 0){ _throw( Null_Exception);} _temp515->tl;}) == 0){
return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char* _temp516=(
char*)"too few arguments to fscanf"; struct _tagged_string _temp517; _temp517.curr=
_temp516; _temp517.base= _temp516; _temp517.last_plus_one= _temp516 + 28;
_temp517;}));} if( ! Cyc_Tcutil_unify(( void*)({ struct Cyc_Core_Opt* _temp519=((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp518= es; if( _temp518 == 0){
_throw( Null_Exception);} _temp518->hd;}))->topt; if( _temp519 == 0){ _throw(
Null_Exception);} _temp519->v;}), Cyc_Absyn_file_typ())){ Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp520= es; if( _temp520 == 0){
_throw( Null_Exception);} _temp520->hd;}))->loc,({ struct _tagged_string
_temp523= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt* _temp522=((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp521= es; if( _temp521 == 0){
_throw( Null_Exception);} _temp521->hd;}))->topt; if( _temp522 == 0){ _throw(
Null_Exception);} _temp522->v;})); xprintf("expecting FILE @ but found %.*s",
_temp523.last_plus_one - _temp523.curr, _temp523.curr);}));} Cyc_Tcexp_check_format_args(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp525=({ struct Cyc_List_List*
_temp524= es; if( _temp524 == 0){ _throw( Null_Exception);} _temp524->tl;}); if(
_temp525 == 0){ _throw( Null_Exception);} _temp525->hd;}),({ struct Cyc_List_List*
_temp527=({ struct Cyc_List_List* _temp526= es; if( _temp526 == 0){ _throw(
Null_Exception);} _temp526->tl;}); if( _temp527 == 0){ _throw( Null_Exception);}
_temp527->tl;}), Cyc_Formatstr_get_scanf_typs); t= Cyc_Absyn_sint_t; goto _LL475;
_LL487: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp528=( char*)"sscanf is not supported";
struct _tagged_string _temp529; _temp529.curr= _temp528; _temp529.base= _temp528;
_temp529.last_plus_one= _temp528 + 24; _temp529;})); t= Cyc_Absyn_sint_t; goto
_LL475; _LL489: switch((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es)){
case 0: _LL530: return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({
char* _temp532=( char*)"primitive operator has 0 arguments"; struct
_tagged_string _temp533; _temp533.curr= _temp532; _temp533.base= _temp532;
_temp533.last_plus_one= _temp532 + 35; _temp533;})); case 1: _LL531: t= Cyc_Tcexp_tcUnPrimop(
te, loc, topt, p,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp535= es;
if( _temp535 == 0){ _throw( Null_Exception);} _temp535->hd;})); break; case 2:
_LL534: t= Cyc_Tcexp_tcBinPrimop( te, loc, topt, p,( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp537= es; if( _temp537 == 0){ _throw( Null_Exception);}
_temp537->hd;}),( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp539=({
struct Cyc_List_List* _temp538= es; if( _temp538 == 0){ _throw( Null_Exception);}
_temp538->tl;}); if( _temp539 == 0){ _throw( Null_Exception);} _temp539->hd;}));
break; default: _LL536: return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_string)({ char* _temp541=( char*)"primitive operator has > 2 arguments";
struct _tagged_string _temp542; _temp542.curr= _temp541; _temp542.base= _temp541;
_temp542.last_plus_one= _temp541 + 37; _temp542;}));} goto _LL475; _LL475:;}
return t;}} static void* Cyc_Tcexp_tcIncrement( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e, void* i){
Cyc_Tcexp_tcExpNoPromote( te, 0, e); if( ! Cyc_Absyn_is_lvalue( e)){ return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_string)({ char* _temp543=( char*)"increment/decrement of non-lvalue";
struct _tagged_string _temp544; _temp544.curr= _temp543; _temp544.base= _temp543;
_temp544.last_plus_one= _temp543 + 34; _temp544;}));}{ void* t=( void*)({ struct
Cyc_Core_Opt* _temp545= e->topt; if( _temp545 == 0){ _throw( Null_Exception);}
_temp545->v;}); if( ! Cyc_Tcutil_is_numeric( e)? ! Cyc_Tcutil_is_tagged_pointer_typ(
t): 0){ Cyc_Tcutil_terr( e->loc,({ struct _tagged_string _temp546= Cyc_Absynpp_typ2string(
t); xprintf("expecting arithmetic or pointer type but found %.*s", _temp546.last_plus_one
- _temp546.curr, _temp546.curr);}));} return t;}} static void* Cyc_Tcexp_tcConditional(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3){ Cyc_Tcexp_tcTest(
te, e1,( struct _tagged_string)({ char* _temp547=( char*)"conditional expression";
struct _tagged_string _temp548; _temp548.curr= _temp547; _temp548.base= _temp547;
_temp548.last_plus_one= _temp547 + 23; _temp548;})); Cyc_Tcexp_tcExp( te, topt,
e2); Cyc_Tcexp_tcExp( te, topt, e3);{ void* t= Cyc_Absyn_new_evar(( void*) Cyc_Absyn_MemKind);
struct Cyc_List_List* l1=({ struct Cyc_List_List* _temp549=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp549->hd=( void*) e3; _temp549->tl=
0; _temp549;}); struct Cyc_List_List* l2=({ struct Cyc_List_List* _temp550=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp550->hd=(
void*) e2; _temp550->tl= l1; _temp550;}); if( ! Cyc_Tcutil_coerce_list( te, t,
l2)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp553= Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Core_Opt* _temp551= e2->topt; if( _temp551 == 0){ _throw(
Null_Exception);} _temp551->v;})); struct _tagged_string _temp554= Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Core_Opt* _temp552= e3->topt; if( _temp552 == 0){ _throw(
Null_Exception);} _temp552->v;})); xprintf("conditional clause types do not match: %.*s != %.*s",
_temp553.last_plus_one - _temp553.curr, _temp553.curr, _temp554.last_plus_one -
_temp554.curr, _temp554.curr);}));} return t;}} static void* Cyc_Tcexp_tcAssignOp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e1, struct Cyc_Core_Opt* po, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExpNoPromote(
te, 0, e1); Cyc_Tcexp_tcExp( te,( void**)(( void*)({ struct Cyc_Core_Opt*
_temp555= e1->topt; if( _temp555 == 0){ _throw( Null_Exception);}& _temp555->v;})),
e2);{ void* t1=( void*)({ struct Cyc_Core_Opt* _temp556= e1->topt; if( _temp556
== 0){ _throw( Null_Exception);} _temp556->v;}); void* t2=( void*)({ struct Cyc_Core_Opt*
_temp557= e2->topt; if( _temp557 == 0){ _throw( Null_Exception);} _temp557->v;});{
void* _temp558= Cyc_Tcutil_compress( t1); struct Cyc_Absyn_Exp* _temp564; struct
Cyc_Absyn_Tqual* _temp566; void* _temp568; _LL560: if(( unsigned int) _temp558 >
4u?(( struct _tunion_struct*) _temp558)->tag == Cyc_Absyn_ArrayType_tag: 0){
_LL569: _temp568=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp558)->f1;
goto _LL567; _LL567: _temp566=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp558)->f2; goto _LL565; _LL565: _temp564=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp558)->f3; goto _LL561;} else{ goto _LL562;}
_LL562: goto _LL563; _LL561: Cyc_Tcutil_terr( loc,( struct _tagged_string)({
char* _temp570=( char*)"cannot assign to an array"; struct _tagged_string
_temp571; _temp571.curr= _temp570; _temp571.base= _temp570; _temp571.last_plus_one=
_temp570 + 26; _temp571;})); goto _LL559; _LL563: goto _LL559; _LL559:;} if( !
Cyc_Absyn_is_lvalue( e1)){ return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_string)({ char* _temp572=( char*)"assignment to non-lvalue"; struct
_tagged_string _temp573; _temp573.curr= _temp572; _temp573.base= _temp572;
_temp573.last_plus_one= _temp572 + 25; _temp573;}));} if( po == 0){ if( ! Cyc_Tcutil_coerce_assign(
te, e2, t1)){ return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string
_temp574= Cyc_Absynpp_typ2string( t1); struct _tagged_string _temp575= Cyc_Absynpp_typ2string(
t2); xprintf("type mismatch: %.*s != %.*s", _temp574.last_plus_one - _temp574.curr,
_temp574.curr, _temp575.last_plus_one - _temp575.curr, _temp575.curr);}));}}
else{ void* p=( void*)({ struct Cyc_Core_Opt* _temp576= po; if( _temp576 == 0){
_throw( Null_Exception);} _temp576->v;}); void* t_result= Cyc_Tcexp_tcBinPrimop(
te, loc, 0, p, e1, e2); if( !( Cyc_Tcutil_unify( t_result, t1)? 1: Cyc_Tcutil_coerceable(
t_result))){ return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char*
_temp577=( char*)"Cannot use this operator in an assignment"; struct
_tagged_string _temp578; _temp578.curr= _temp577; _temp578.base= _temp577;
_temp578.last_plus_one= _temp577 + 42; _temp578;}));}} return t2;}} static void*
Cyc_Tcexp_tcSeqExp( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp(
te, 0, e1); Cyc_Tcexp_tcExp( te, topt, e2); return( void*)({ struct Cyc_Core_Opt*
_temp579= e2->topt; if( _temp579 == 0){ _throw( Null_Exception);} _temp579->v;});}
static void* Cyc_Tcexp_tcFnCall( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_List_List* es){ Cyc_Tcexp_tcExp(
te, 0, e);{ void* t= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp580= e->topt; if( _temp580 == 0){ _throw( Null_Exception);} _temp580->v;}));
void* _temp581= t; struct Cyc_Absyn_PtrInfo _temp587; struct Cyc_Absyn_Conref*
_temp589; struct Cyc_Absyn_Tqual* _temp591; struct Cyc_Absyn_Conref* _temp593;
void* _temp595; void* _temp597; _LL583: if(( unsigned int) _temp581 > 4u?((
struct _tunion_struct*) _temp581)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL588:
_temp587=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp581)->f1; _LL598: _temp597=( void*) _temp587.elt_typ; goto _LL596; _LL596:
_temp595=( void*) _temp587.rgn_typ; goto _LL594; _LL594: _temp593=( struct Cyc_Absyn_Conref*)
_temp587.nullable; goto _LL592; _LL592: _temp591=( struct Cyc_Absyn_Tqual*)
_temp587.tq; goto _LL590; _LL590: _temp589=( struct Cyc_Absyn_Conref*) _temp587.bounds;
goto _LL584;} else{ goto _LL585;} _LL585: goto _LL586; _LL584: Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp595); Cyc_Tcutil_check_nonzero_bound( loc, _temp589);{ void*
_temp599= Cyc_Tcutil_compress( _temp597); struct Cyc_Absyn_FnInfo _temp605;
struct Cyc_List_List* _temp607; int _temp609; struct Cyc_List_List* _temp611;
void* _temp613; struct Cyc_Core_Opt* _temp615; struct Cyc_List_List* _temp617;
_LL601: if(( unsigned int) _temp599 > 4u?(( struct _tunion_struct*) _temp599)->tag
== Cyc_Absyn_FnType_tag: 0){ _LL606: _temp605=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp599)->f1; _LL618: _temp617=( struct Cyc_List_List*)
_temp605.tvars; goto _LL616; _LL616: _temp615=( struct Cyc_Core_Opt*) _temp605.effect;
goto _LL614; _LL614: _temp613=( void*) _temp605.ret_typ; goto _LL612; _LL612:
_temp611=( struct Cyc_List_List*) _temp605.args; goto _LL610; _LL610: _temp609=(
int) _temp605.varargs; goto _LL608; _LL608: _temp607=( struct Cyc_List_List*)
_temp605.attributes; goto _LL602;} else{ goto _LL603;} _LL603: goto _LL604;
_LL602: if( _temp609){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp619=( char*)"vararg functions (...) not supported yet"; struct
_tagged_string _temp620; _temp620.curr= _temp619; _temp620.base= _temp619;
_temp620.last_plus_one= _temp619 + 41; _temp620;}));} if( topt != 0){ Cyc_Tcutil_unify(
_temp613,({ void** _temp621= topt; if( _temp621 == 0){ _throw( Null_Exception);}*
_temp621;}));} while( es != 0? _temp611 != 0: 0) { struct Cyc_Absyn_Exp* e1=(
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp622= es; if( _temp622 == 0){
_throw( Null_Exception);} _temp622->hd;}); void* t2=(*(( struct _tuple2*)({
struct Cyc_List_List* _temp623= _temp611; if( _temp623 == 0){ _throw(
Null_Exception);} _temp623->hd;}))).f3; Cyc_Tcexp_tcExp( te,( void**)& t2, e1);
if( ! Cyc_Tcutil_coerce_arg( te, e1, t2)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp625= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp624= e1->topt; if( _temp624 == 0){ _throw( Null_Exception);} _temp624->v;}));
struct _tagged_string _temp626= Cyc_Absynpp_typ2string( t2); xprintf("actual argument has type %.*s but formal has type %.*s",
_temp625.last_plus_one - _temp625.curr, _temp625.curr, _temp626.last_plus_one -
_temp626.curr, _temp626.curr);}));} es=({ struct Cyc_List_List* _temp627= es;
if( _temp627 == 0){ _throw( Null_Exception);} _temp627->tl;}); _temp611=({
struct Cyc_List_List* _temp628= _temp611; if( _temp628 == 0){ _throw(
Null_Exception);} _temp628->tl;});} if( es != 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp629=( char*)"too many arguments for function";
struct _tagged_string _temp630; _temp630.curr= _temp629; _temp630.base= _temp629;
_temp630.last_plus_one= _temp629 + 32; _temp630;}));} if( _temp611 != 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp631=( char*)"too few arguments for function";
struct _tagged_string _temp632; _temp632.curr= _temp631; _temp632.base= _temp631;
_temp632.last_plus_one= _temp631 + 31; _temp632;}));} Cyc_Tcenv_check_effect_accessible(
te, loc,( void*)({ struct Cyc_Core_Opt* _temp633= _temp615; if( _temp633 == 0){
_throw( Null_Exception);} _temp633->v;})); return _temp613; _LL604: return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_string)({ char* _temp634=( char*)"expected pointer to function";
struct _tagged_string _temp635; _temp635.curr= _temp634; _temp635.base= _temp634;
_temp635.last_plus_one= _temp634 + 29; _temp635;})); _LL600:;} _LL586: return
Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char* _temp636=( char*)"expected pointer to function";
struct _tagged_string _temp637; _temp637.curr= _temp636; _temp637.base= _temp636;
_temp637.last_plus_one= _temp636 + 29; _temp637;})); _LL582:;}} static void* Cyc_Tcexp_tcThrow(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp( te,( void**)& Cyc_Absyn_exn_typ, e); if( !
Cyc_Tcutil_coerce_use( te, e, Cyc_Absyn_exn_typ)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp639= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp638= e->topt; if( _temp638 == 0){ _throw( Null_Exception);} _temp638->v;}));
xprintf("expected xtunion exn but found %.*s", _temp639.last_plus_one - _temp639.curr,
_temp639.curr);}));} return Cyc_Absyn_wildtyp();} static void* Cyc_Tcexp_tcInstantiate(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e, struct Cyc_List_List* ts){ Cyc_Tcexp_tcExpNoInst( te, 0, e);(
void*)({ struct Cyc_Core_Opt* _temp640= e->topt; if( _temp640 == 0){ _throw(
Null_Exception);} _temp640->v=( void*) Cyc_Absyn_pointer_expand( Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp641= e->topt; if( _temp641 == 0){ _throw(
Null_Exception);} _temp641->v;})));});{ void* t1= Cyc_Tcutil_compress(( void*)({
struct Cyc_Core_Opt* _temp642= e->topt; if( _temp642 == 0){ _throw(
Null_Exception);} _temp642->v;}));{ void* _temp643= t1; struct Cyc_Absyn_PtrInfo
_temp649; struct Cyc_Absyn_Conref* _temp651; struct Cyc_Absyn_Tqual* _temp653;
struct Cyc_Absyn_Conref* _temp655; void* _temp657; void* _temp659; _LL645: if((
unsigned int) _temp643 > 4u?(( struct _tunion_struct*) _temp643)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL650: _temp649=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp643)->f1; _LL660: _temp659=( void*) _temp649.elt_typ; goto _LL658; _LL658:
_temp657=( void*) _temp649.rgn_typ; goto _LL656; _LL656: _temp655=( struct Cyc_Absyn_Conref*)
_temp649.nullable; goto _LL654; _LL654: _temp653=( struct Cyc_Absyn_Tqual*)
_temp649.tq; goto _LL652; _LL652: _temp651=( struct Cyc_Absyn_Conref*) _temp649.bounds;
goto _LL646;} else{ goto _LL647;} _LL647: goto _LL648; _LL646:{ void* _temp661=
Cyc_Tcutil_compress( _temp659); struct Cyc_Absyn_FnInfo _temp667; struct Cyc_List_List*
_temp669; int _temp671; struct Cyc_List_List* _temp673; void* _temp675; struct
Cyc_Core_Opt* _temp677; struct Cyc_List_List* _temp679; _LL663: if((
unsigned int) _temp661 > 4u?(( struct _tunion_struct*) _temp661)->tag == Cyc_Absyn_FnType_tag:
0){ _LL668: _temp667=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp661)->f1; _LL680: _temp679=( struct Cyc_List_List*) _temp667.tvars; goto
_LL678; _LL678: _temp677=( struct Cyc_Core_Opt*) _temp667.effect; goto _LL676;
_LL676: _temp675=( void*) _temp667.ret_typ; goto _LL674; _LL674: _temp673=(
struct Cyc_List_List*) _temp667.args; goto _LL672; _LL672: _temp671=( int)
_temp667.varargs; goto _LL670; _LL670: _temp669=( struct Cyc_List_List*)
_temp667.attributes; goto _LL664;} else{ goto _LL665;} _LL665: goto _LL666;
_LL664: { struct Cyc_List_List* instantiation= 0; if((( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( ts) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp679)){ return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char*
_temp681=( char*)"type instantiation mismatch"; struct _tagged_string _temp682;
_temp682.curr= _temp681; _temp682.base= _temp681; _temp682.last_plus_one=
_temp681 + 28; _temp682;}));} for( 0; ts != 0; ts=({ struct Cyc_List_List*
_temp683= ts; if( _temp683 == 0){ _throw( Null_Exception);} _temp683->tl;}),
_temp679=({ struct Cyc_List_List* _temp684= _temp679; if( _temp684 == 0){ _throw(
Null_Exception);} _temp684->tl;})){ void* k=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp685= _temp679; if( _temp685 == 0){ _throw( Null_Exception);} _temp685->hd;}))->kind);
Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars( te), k,( void*)({
struct Cyc_List_List* _temp686= ts; if( _temp686 == 0){ _throw( Null_Exception);}
_temp686->hd;})); instantiation=({ struct Cyc_List_List* _temp687=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp687->hd=( void*)({ struct
_tuple4* _temp688=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp688->f1=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp690=
_temp679; if( _temp690 == 0){ _throw( Null_Exception);} _temp690->hd;});
_temp688->f2=( void*)({ struct Cyc_List_List* _temp689= ts; if( _temp689 == 0){
_throw( Null_Exception);} _temp689->hd;}); _temp688;}); _temp687->tl=
instantiation; _temp687;});}{ void* new_fn_typ= Cyc_Tcutil_substitute(
instantiation,( void*)({ struct Cyc_Absyn_FnType_struct* _temp691=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp691[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp692; _temp692.tag= Cyc_Absyn_FnType_tag; _temp692.f1=({ struct Cyc_Absyn_FnInfo
_temp693; _temp693.tvars= 0; _temp693.effect= _temp677; _temp693.ret_typ=( void*)
_temp675; _temp693.args= _temp673; _temp693.varargs= _temp671; _temp693.attributes=
_temp669; _temp693;}); _temp692;}); _temp691;})); void* new_typ=( void*)({
struct Cyc_Absyn_PointerType_struct* _temp694=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp694[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp695; _temp695.tag= Cyc_Absyn_PointerType_tag;
_temp695.f1=({ struct Cyc_Absyn_PtrInfo _temp696; _temp696.elt_typ=( void*)
new_fn_typ; _temp696.rgn_typ=( void*) _temp657; _temp696.nullable= _temp655;
_temp696.tq= _temp653; _temp696.bounds= _temp651; _temp696;}); _temp695;});
_temp694;}); return new_typ;}} _LL666: goto _LL662; _LL662:;} goto _LL644;
_LL648: goto _LL644; _LL644:;} return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp697= Cyc_Absynpp_typ2string( t1); xprintf("expecting polymorphic type but found %.*s",
_temp697.last_plus_one - _temp697.curr, _temp697.curr);}));}} static void* Cyc_Tcexp_tcCast(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, void*
t, struct Cyc_Absyn_Exp* e){ Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars(
te),( void*) Cyc_Absyn_MemKind, t); Cyc_Tcexp_tcExp( te, 0, e);{ void* t2=( void*)({
struct Cyc_Core_Opt* _temp698= e->topt; if( _temp698 == 0){ _throw(
Null_Exception);} _temp698->v;}); if( ! Cyc_Tcutil_silent_castable( te, loc, t2,
t)? ! Cyc_Tcutil_castable( te, loc, t2, t): 0){ return Cyc_Tcexp_expr_err( te,
loc,({ struct _tagged_string _temp699= Cyc_Absynpp_typ2string( t2); struct
_tagged_string _temp700= Cyc_Absynpp_typ2string( t); xprintf("cannot cast %.*s to %.*s",
_temp699.last_plus_one - _temp699.curr, _temp699.curr, _temp700.last_plus_one -
_temp700.curr, _temp700.curr);}));} return t;}} static void* Cyc_Tcexp_tcAddress(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e){ void** topt2= 0; struct Cyc_Absyn_Tqual* tq2= Cyc_Absyn_empty_tqual();
if( topt != 0){ void* _temp702= Cyc_Tcutil_compress(({ void** _temp701= topt;
if( _temp701 == 0){ _throw( Null_Exception);}* _temp701;})); struct Cyc_Absyn_PtrInfo
_temp708; struct Cyc_Absyn_Conref* _temp710; struct Cyc_Absyn_Tqual* _temp712;
struct Cyc_Absyn_Conref* _temp714; void* _temp716; void* _temp718; _LL704: if((
unsigned int) _temp702 > 4u?(( struct _tunion_struct*) _temp702)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL709: _temp708=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp702)->f1; _LL719: _temp718=( void*) _temp708.elt_typ; goto _LL717; _LL717:
_temp716=( void*) _temp708.rgn_typ; goto _LL715; _LL715: _temp714=( struct Cyc_Absyn_Conref*)
_temp708.nullable; goto _LL713; _LL713: _temp712=( struct Cyc_Absyn_Tqual*)
_temp708.tq; goto _LL711; _LL711: _temp710=( struct Cyc_Absyn_Conref*) _temp708.bounds;
goto _LL705;} else{ goto _LL706;} _LL706: goto _LL707; _LL705: topt2=({ void**
_temp720=( void**) GC_malloc( sizeof( void*)); _temp720[ 0]= _temp718; _temp720;});
tq2= _temp712; goto _LL703; _LL707: goto _LL703; _LL703:;} Cyc_Tcexp_tcExpNoInst(
te, topt2, e);{ void* _temp721=( void*) e->r; struct Cyc_Absyn_Structdecl*
_temp729; struct Cyc_List_List* _temp731; struct Cyc_Core_Opt* _temp733; struct
_tuple1* _temp735; struct Cyc_List_List* _temp737; _LL723: if((( struct
_tunion_struct*) _temp721)->tag == Cyc_Absyn_Struct_e_tag){ _LL736: _temp735=(
struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*) _temp721)->f1; goto _LL734;
_LL734: _temp733=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp721)->f2; goto _LL732; _LL732: _temp731=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp721)->f3; goto _LL730; _LL730: _temp729=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*) _temp721)->f4;
goto _LL724;} else{ goto _LL725;} _LL725: if((( struct _tunion_struct*) _temp721)->tag
== Cyc_Absyn_Tuple_e_tag){ _LL738: _temp737=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Tuple_e_struct*) _temp721)->f1; goto _LL726;} else{ goto _LL727;}
_LL727: goto _LL728; _LL724: goto _LL726; _LL726: Cyc_Tcutil_warn( loc,( struct
_tagged_string)({ char* _temp739=( char*)"& used to allocate"; struct
_tagged_string _temp740; _temp740.curr= _temp739; _temp740.base= _temp739;
_temp740.last_plus_one= _temp739 + 19; _temp740;}));{ void*(* ptr_maker)( void*
t, void* rgn, struct Cyc_Absyn_Tqual* tq)= Cyc_Absyn_at_typ; if( topt != 0){
void* _temp742= Cyc_Tcutil_compress(({ void** _temp741= topt; if( _temp741 == 0){
_throw( Null_Exception);}* _temp741;})); struct Cyc_Absyn_PtrInfo _temp748;
struct Cyc_Absyn_Conref* _temp750; struct Cyc_Absyn_Tqual* _temp752; struct Cyc_Absyn_Conref*
_temp754; void* _temp756; void* _temp758; _LL744: if(( unsigned int) _temp742 >
4u?(( struct _tunion_struct*) _temp742)->tag == Cyc_Absyn_PointerType_tag: 0){
_LL749: _temp748=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp742)->f1; _LL759: _temp758=( void*) _temp748.elt_typ; goto _LL757; _LL757:
_temp756=( void*) _temp748.rgn_typ; goto _LL755; _LL755: _temp754=( struct Cyc_Absyn_Conref*)
_temp748.nullable; goto _LL753; _LL753: _temp752=( struct Cyc_Absyn_Tqual*)
_temp748.tq; goto _LL751; _LL751: _temp750=( struct Cyc_Absyn_Conref*) _temp748.bounds;
goto _LL745;} else{ goto _LL746;} _LL746: goto _LL747; _LL745: if((( int(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp754)){ ptr_maker= Cyc_Absyn_star_typ;}
goto _LL743; _LL747: goto _LL743; _LL743:;} return ptr_maker(( void*)({ struct
Cyc_Core_Opt* _temp760= e->topt; if( _temp760 == 0){ _throw( Null_Exception);}
_temp760->v;}),( void*) Cyc_Absyn_HeapRgn, tq2);} _LL728: goto _LL722; _LL722:;}{
void* _temp763; int _temp765; struct _tuple5 _temp761= Cyc_Tcutil_addressof_props(
te, e); _LL766: _temp765= _temp761.f1; goto _LL764; _LL764: _temp763= _temp761.f2;
goto _LL762; _LL762: { struct Cyc_Absyn_Tqual* tq= Cyc_Absyn_empty_tqual(); if(
_temp765){ tq->q_const= 1;}{ void* t= Cyc_Absyn_at_typ(( void*)({ struct Cyc_Core_Opt*
_temp767= e->topt; if( _temp767 == 0){ _throw( Null_Exception);} _temp767->v;}),
_temp763, tq); return t;}}}} static void* Cyc_Tcexp_tcSizeof( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, void* t){ Cyc_Tcutil_check_type(
loc, te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind, t); Cyc_Evexp_szof(
t, loc); return Cyc_Absyn_uint_t;} static void* Cyc_Tcexp_tcDeref( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp(
te, 0, e);{ void* t= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp768= e->topt; if( _temp768 == 0){ _throw( Null_Exception);} _temp768->v;}));
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
te, loc, _temp783); Cyc_Tcutil_check_nonzero_bound( loc, _temp777); return
_temp785; _LL774: return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string
_temp787= Cyc_Absynpp_typ2string( t); xprintf("expecting * or @ type but found %.*s",
_temp787.last_plus_one - _temp787.curr, _temp787.curr);})); _LL770:;}} static
void* Cyc_Tcexp_tcStructMember( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* outer_e, struct Cyc_Absyn_Exp* e, struct
_tagged_string* f){ Cyc_Tcexp_tcExpNoPromote( te, 0, e);{ void* t= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp788= e->topt; if( _temp788 == 0){ _throw(
Null_Exception);} _temp788->v;})); void* _temp789= t; struct Cyc_Absyn_Structdecl**
_temp801; struct Cyc_Absyn_Structdecl* _temp803; struct Cyc_List_List* _temp804;
struct _tuple1* _temp806; struct Cyc_Absyn_Uniondecl** _temp808; struct Cyc_Absyn_Uniondecl*
_temp810; struct Cyc_List_List* _temp811; struct _tuple1* _temp813; struct Cyc_Absyn_Exp*
_temp816; struct Cyc_Absyn_Tqual* _temp818; void* _temp820; struct Cyc_Absyn_PtrInfo
_temp825; _LL791: if(( unsigned int) _temp789 > 4u?(( struct _tunion_struct*)
_temp789)->tag == Cyc_Absyn_StructType_tag: 0){ _LL807: _temp806=( struct
_tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp789)->f1; goto _LL805;
_LL805: _temp804=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp789)->f2; goto _LL802; _LL802: _temp801=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp789)->f3; if( _temp801 == 0){ goto
_LL793;} else{ _temp803=* _temp801; goto _LL792;}} else{ goto _LL793;} _LL793:
if(( unsigned int) _temp789 > 4u?(( struct _tunion_struct*) _temp789)->tag ==
Cyc_Absyn_UnionType_tag: 0){ _LL814: _temp813=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp789)->f1; goto _LL812; _LL812: _temp811=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp789)->f2; goto _LL809; _LL809: _temp808=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp789)->f3;
if( _temp808 == 0){ goto _LL795;} else{ _temp810=* _temp808; goto _LL794;}}
else{ goto _LL795;} _LL795: if(( unsigned int) _temp789 > 4u?(( struct
_tunion_struct*) _temp789)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL821: _temp820=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp789)->f1; goto _LL819; _LL819:
_temp818=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp789)->f2; goto _LL817; _LL817: _temp816=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp789)->f3; goto _LL815;} else{ goto _LL797;}
_LL815: if( Cyc_String_zstrcmp(* f,( struct _tagged_string)({ char* _temp822=(
char*)"size"; struct _tagged_string _temp823; _temp823.curr= _temp822; _temp823.base=
_temp822; _temp823.last_plus_one= _temp822 + 5; _temp823;})) == 0){ goto _LL796;}
else{ goto _LL797;} _LL797: if(( unsigned int) _temp789 > 4u?(( struct
_tunion_struct*) _temp789)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL826:
_temp825=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp789)->f1; goto _LL824;} else{ goto _LL799;} _LL824: if( Cyc_String_zstrcmp(*
f,( struct _tagged_string)({ char* _temp827=( char*)"size"; struct
_tagged_string _temp828; _temp828.curr= _temp827; _temp828.base= _temp827;
_temp828.last_plus_one= _temp827 + 5; _temp828;})) == 0){ goto _LL798;} else{
goto _LL799;} _LL799: goto _LL800; _LL792: if( _temp806 == 0){ return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_string)({ char* _temp829=( char*)"unresolved struct type (COMPILER ERROR)";
struct _tagged_string _temp830; _temp830.curr= _temp829; _temp830.base= _temp829;
_temp830.last_plus_one= _temp829 + 40; _temp830;}));}{ struct Cyc_Absyn_Structfield*
finfo= Cyc_Absyn_lookup_struct_field( _temp803, f); if( finfo == 0){ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp832= Cyc_Absynpp_qvar2string(( struct
_tuple1*)({ struct _tuple1* _temp831= _temp806; if( _temp831 == 0){ _throw(
Null_Exception);} _temp831;})); struct _tagged_string _temp833=* f; xprintf("struct %.*s has no %.*s field",
_temp832.last_plus_one - _temp832.curr, _temp832.curr, _temp833.last_plus_one -
_temp833.curr, _temp833.curr);}));}{ struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp803->tvs,
_temp804); void* t2= Cyc_Tcutil_substitute( inst,( void*)({ struct Cyc_Absyn_Structfield*
_temp834= finfo; if( _temp834 == 0){ _throw( Null_Exception);} _temp834->type;}));
return t2;}} _LL794: if( _temp813 == 0){ return Cyc_Tcexp_expr_err( te, loc,(
struct _tagged_string)({ char* _temp835=( char*)"unresolved union type (COMPILER ERROR)";
struct _tagged_string _temp836; _temp836.curr= _temp835; _temp836.base= _temp835;
_temp836.last_plus_one= _temp835 + 39; _temp836;}));}{ struct Cyc_Absyn_Structfield*
finfo= Cyc_Absyn_lookup_union_field( _temp810, f); if( finfo == 0){ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp838= Cyc_Absynpp_qvar2string(( struct
_tuple1*)({ struct _tuple1* _temp837= _temp813; if( _temp837 == 0){ _throw(
Null_Exception);} _temp837;})); struct _tagged_string _temp839=* f; xprintf("union %.*s has no %.*s field",
_temp838.last_plus_one - _temp838.curr, _temp838.curr, _temp839.last_plus_one -
_temp839.curr, _temp839.curr);}));}{ struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp810->tvs,
_temp811); void* t2= Cyc_Tcutil_substitute( inst,( void*)({ struct Cyc_Absyn_Structfield*
_temp840= finfo; if( _temp840 == 0){ _throw( Null_Exception);} _temp840->type;}));
return t2;}} _LL796:( void*)( outer_e->r=( void*)(( void*)({ struct Cyc_Absyn_Primop_e_struct*
_temp841=( struct Cyc_Absyn_Primop_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct));
_temp841[ 0]=({ struct Cyc_Absyn_Primop_e_struct _temp842; _temp842.tag= Cyc_Absyn_Primop_e_tag;
_temp842.f1=( void*)(( void*) Cyc_Absyn_Size); _temp842.f2=({ struct Cyc_List_List*
_temp843=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp843->hd=( void*) e; _temp843->tl= 0; _temp843;}); _temp842;}); _temp841;})));
return Cyc_Absyn_uint_t; _LL798:( void*)( outer_e->r=( void*)(( void*)({ struct
Cyc_Absyn_Primop_e_struct* _temp844=( struct Cyc_Absyn_Primop_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp844[ 0]=({ struct
Cyc_Absyn_Primop_e_struct _temp845; _temp845.tag= Cyc_Absyn_Primop_e_tag;
_temp845.f1=( void*)(( void*) Cyc_Absyn_Size); _temp845.f2=({ struct Cyc_List_List*
_temp846=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp846->hd=( void*) e; _temp846->tl= 0; _temp846;}); _temp845;}); _temp844;})));
return Cyc_Absyn_uint_t; _LL800: if( Cyc_String_zstrcmp(* f,( struct
_tagged_string)({ char* _temp847=( char*)"size"; struct _tagged_string _temp848;
_temp848.curr= _temp847; _temp848.base= _temp847; _temp848.last_plus_one=
_temp847 + 5; _temp848;})) == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp849= Cyc_Absynpp_typ2string( t); xprintf("expecting struct, union or array, found %.*s",
_temp849.last_plus_one - _temp849.curr, _temp849.curr);}));} else{ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp850= Cyc_Absynpp_typ2string( t); xprintf("expecting struct or union, found %.*s",
_temp850.last_plus_one - _temp850.curr, _temp850.curr);}));} _LL790:;}} static
void* Cyc_Tcexp_tcStructArrow( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e, struct _tagged_string* f){ Cyc_Tcexp_tcExp(
te, 0, e);{ void* t= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp851= e->topt; if( _temp851 == 0){ _throw( Null_Exception);} _temp851->v;}));
void* _temp852= t; struct Cyc_Absyn_PtrInfo _temp858; struct Cyc_Absyn_Conref*
_temp860; struct Cyc_Absyn_Tqual* _temp862; struct Cyc_Absyn_Conref* _temp864;
void* _temp866; void* _temp868; _LL854: if(( unsigned int) _temp852 > 4u?((
struct _tunion_struct*) _temp852)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL859:
_temp858=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp852)->f1; _LL869: _temp868=( void*) _temp858.elt_typ; goto _LL867; _LL867:
_temp866=( void*) _temp858.rgn_typ; goto _LL865; _LL865: _temp864=( struct Cyc_Absyn_Conref*)
_temp858.nullable; goto _LL863; _LL863: _temp862=( struct Cyc_Absyn_Tqual*)
_temp858.tq; goto _LL861; _LL861: _temp860=( struct Cyc_Absyn_Conref*) _temp858.bounds;
goto _LL855;} else{ goto _LL856;} _LL856: goto _LL857; _LL855: Cyc_Tcutil_check_nonzero_bound(
loc, _temp860);{ void* _temp870= Cyc_Tcutil_compress( _temp868); struct Cyc_Absyn_Structdecl**
_temp878; struct Cyc_Absyn_Structdecl* _temp880; struct Cyc_List_List* _temp881;
struct _tuple1* _temp883; struct Cyc_Absyn_Uniondecl** _temp885; struct Cyc_Absyn_Uniondecl*
_temp887; struct Cyc_List_List* _temp888; struct _tuple1* _temp890; _LL872: if((
unsigned int) _temp870 > 4u?(( struct _tunion_struct*) _temp870)->tag == Cyc_Absyn_StructType_tag:
0){ _LL884: _temp883=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp870)->f1; goto _LL882; _LL882: _temp881=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp870)->f2; goto _LL879; _LL879: _temp878=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp870)->f3;
if( _temp878 == 0){ goto _LL874;} else{ _temp880=* _temp878; goto _LL873;}}
else{ goto _LL874;} _LL874: if(( unsigned int) _temp870 > 4u?(( struct
_tunion_struct*) _temp870)->tag == Cyc_Absyn_UnionType_tag: 0){ _LL891: _temp890=(
struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*) _temp870)->f1; goto
_LL889; _LL889: _temp888=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp870)->f2; goto _LL886; _LL886: _temp885=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp870)->f3; if( _temp885 == 0){ goto
_LL876;} else{ _temp887=* _temp885; goto _LL875;}} else{ goto _LL876;} _LL876:
goto _LL877; _LL873: { struct Cyc_Absyn_Structfield* finfo= Cyc_Absyn_lookup_struct_field(
_temp880, f); if( finfo == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp893= Cyc_Absynpp_qvar2string(( struct _tuple1*)({ struct
_tuple1* _temp892= _temp883; if( _temp892 == 0){ _throw( Null_Exception);}
_temp892;})); struct _tagged_string _temp894=* f; xprintf("struct %.*s has no %.*s field",
_temp893.last_plus_one - _temp893.curr, _temp893.curr, _temp894.last_plus_one -
_temp894.curr, _temp894.curr);}));}{ struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp880->tvs,
_temp881); void* t3= Cyc_Tcutil_substitute( inst,( void*)({ struct Cyc_Absyn_Structfield*
_temp895= finfo; if( _temp895 == 0){ _throw( Null_Exception);} _temp895->type;}));
Cyc_Tcenv_check_rgn_accessible( te, loc, _temp866); return t3;}} _LL875: {
struct Cyc_Absyn_Structfield* finfo= Cyc_Absyn_lookup_union_field( _temp887, f);
if( finfo == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string
_temp897= Cyc_Absynpp_qvar2string(( struct _tuple1*)({ struct _tuple1* _temp896=
_temp890; if( _temp896 == 0){ _throw( Null_Exception);} _temp896;})); struct
_tagged_string _temp898=* f; xprintf("union %.*s has no %.*s field", _temp897.last_plus_one
- _temp897.curr, _temp897.curr, _temp898.last_plus_one - _temp898.curr, _temp898.curr);}));}{
struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_zip)( _temp887->tvs, _temp888); void* t3= Cyc_Tcutil_substitute(
inst,( void*)({ struct Cyc_Absyn_Structfield* _temp899= finfo; if( _temp899 == 0){
_throw( Null_Exception);} _temp899->type;})); Cyc_Tcenv_check_rgn_accessible( te,
loc, _temp866); return t3;}} _LL877: return Cyc_Tcexp_expr_err( te, loc,({
struct _tagged_string _temp900= Cyc_Absynpp_typ2string( t); xprintf("expecting struct pointer, found %.*s",
_temp900.last_plus_one - _temp900.curr, _temp900.curr);})); _LL871:;} _LL857:
return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string _temp901= Cyc_Absynpp_typ2string(
t); xprintf("expecting struct pointer, found %.*s", _temp901.last_plus_one -
_temp901.curr, _temp901.curr);})); _LL853:;}} static void* Cyc_Tcexp_ithTupleType(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
ts, struct Cyc_Absyn_Exp* index){ unsigned int i= Cyc_Evexp_eval_const_uint_exp(
index); struct _handler_cons _temp902; _push_handler(& _temp902);{ struct
_xtunion_struct* _temp903=( struct _xtunion_struct*) setjmp( _temp902.handler);
if( ! _temp903){{ void* _temp904=(*(( struct _tuple7*(*)( struct Cyc_List_List*
x, int i)) Cyc_List_nth)( ts,( int) i)).f2; _npop_handler( 0u); return _temp904;};
_pop_handler();} else{ struct _xtunion_struct* _temp906= _temp903; _LL908: if(
_temp906->tag == Cyc_List_Nth_tag){ goto _LL909;} else{ goto _LL910;} _LL910:
goto _LL911; _LL909: return Cyc_Tcexp_expr_err( te, loc, xprintf("index is %d but tuple has only %d fields",(
int) i,(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( ts))); _LL911:(
void) _throw( _temp906); _LL907:;}}} static void* Cyc_Tcexp_tcSubscript( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp( te, 0, e1); Cyc_Tcexp_tcExp( te,
0, e2);{ void* t1= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp912=
e1->topt; if( _temp912 == 0){ _throw( Null_Exception);} _temp912->v;})); void*
t2= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp913= e2->topt; if(
_temp913 == 0){ _throw( Null_Exception);} _temp913->v;})); if( ! Cyc_Tcutil_coerce_sint_typ(
te, e2)){ return Cyc_Tcexp_expr_err( te, e2->loc,({ struct _tagged_string
_temp914= Cyc_Absynpp_typ2string( t2); xprintf("expecting int subscript, found %.*s",
_temp914.last_plus_one - _temp914.curr, _temp914.curr);}));}{ void* _temp915= t1;
struct Cyc_Absyn_PtrInfo _temp923; struct Cyc_Absyn_Conref* _temp925; struct Cyc_Absyn_Tqual*
_temp927; struct Cyc_Absyn_Conref* _temp929; void* _temp931; void* _temp933;
struct Cyc_List_List* _temp935; _LL917: if(( unsigned int) _temp915 > 4u?((
struct _tunion_struct*) _temp915)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL924:
_temp923=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp915)->f1; _LL934: _temp933=( void*) _temp923.elt_typ; goto _LL932; _LL932:
_temp931=( void*) _temp923.rgn_typ; goto _LL930; _LL930: _temp929=( struct Cyc_Absyn_Conref*)
_temp923.nullable; goto _LL928; _LL928: _temp927=( struct Cyc_Absyn_Tqual*)
_temp923.tq; goto _LL926; _LL926: _temp925=( struct Cyc_Absyn_Conref*) _temp923.bounds;
goto _LL918;} else{ goto _LL919;} _LL919: if(( unsigned int) _temp915 > 4u?((
struct _tunion_struct*) _temp915)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL936:
_temp935=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp915)->f1;
goto _LL920;} else{ goto _LL921;} _LL921: goto _LL922; _LL918: if( Cyc_Tcutil_is_const_exp(
te, e2)){ Cyc_Tcutil_check_bound( loc, Cyc_Evexp_eval_const_uint_exp( e2),
_temp925);} else{ Cyc_Tcutil_check_nonzero_bound( loc, _temp925);} Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp931); return _temp933; _LL920: return Cyc_Tcexp_ithTupleType( te,
loc, _temp935, e2); _LL922: return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp937= Cyc_Absynpp_typ2string( t1); xprintf("subscript applied to %.*s",
_temp937.last_plus_one - _temp937.curr, _temp937.curr);})); _LL916:;}}} static
void* Cyc_Tcexp_tcTuple( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_List_List* es){ int done= 0; struct Cyc_List_List*
fields= 0; if( topt != 0){ void* _temp939= Cyc_Tcutil_compress(({ void**
_temp938= topt; if( _temp938 == 0){ _throw( Null_Exception);}* _temp938;}));
struct Cyc_List_List* _temp945; _LL941: if(( unsigned int) _temp939 > 4u?((
struct _tunion_struct*) _temp939)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL946:
_temp945=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp939)->f1;
goto _LL942;} else{ goto _LL943;} _LL943: goto _LL944; _LL942: if((( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp945) !=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( es)){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp947=( char*)"tuple expression has the wrong number of fields"; struct
_tagged_string _temp948; _temp948.curr= _temp947; _temp948.base= _temp947;
_temp948.last_plus_one= _temp947 + 48; _temp948;})); goto _LL940;} for( 0; es !=
0; es=({ struct Cyc_List_List* _temp949= es; if( _temp949 == 0){ _throw(
Null_Exception);} _temp949->tl;}), _temp945=({ struct Cyc_List_List* _temp950=
_temp945; if( _temp950 == 0){ _throw( Null_Exception);} _temp950->tl;})){ void*
topt2=(*(( struct _tuple7*)({ struct Cyc_List_List* _temp951= _temp945; if(
_temp951 == 0){ _throw( Null_Exception);} _temp951->hd;}))).f2; Cyc_Tcexp_tcExpInitializer(
te,( void**)& topt2,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp952=
es; if( _temp952 == 0){ _throw( Null_Exception);} _temp952->hd;})); fields=({
struct Cyc_List_List* _temp953=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp953->hd=( void*)({ struct _tuple7* _temp954=(
struct _tuple7*) GC_malloc( sizeof( struct _tuple7)); _temp954->f1=(*(( struct
_tuple7*)({ struct Cyc_List_List* _temp957= _temp945; if( _temp957 == 0){ _throw(
Null_Exception);} _temp957->hd;}))).f1; _temp954->f2=( void*)({ struct Cyc_Core_Opt*
_temp956=(( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp955= es; if(
_temp955 == 0){ _throw( Null_Exception);} _temp955->hd;}))->topt; if( _temp956
== 0){ _throw( Null_Exception);} _temp956->v;}); _temp954;}); _temp953->tl=
fields; _temp953;});} done= 1; goto _LL940; _LL944: goto _LL940; _LL940:;} if( !
done){ for( 0; es != 0; es=({ struct Cyc_List_List* _temp958= es; if( _temp958
== 0){ _throw( Null_Exception);} _temp958->tl;})){ Cyc_Tcexp_tcExpInitializer(
te, 0,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp959= es; if(
_temp959 == 0){ _throw( Null_Exception);} _temp959->hd;})); fields=({ struct Cyc_List_List*
_temp960=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp960->hd=( void*)({ struct _tuple7* _temp961=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp961->f1= Cyc_Absyn_empty_tqual(); _temp961->f2=(
void*)({ struct Cyc_Core_Opt* _temp963=(( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp962= es; if( _temp962 == 0){ _throw( Null_Exception);} _temp962->hd;}))->topt;
if( _temp963 == 0){ _throw( Null_Exception);} _temp963->v;}); _temp961;});
_temp960->tl= fields; _temp960;});}} return( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp964=( struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp964[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp965; _temp965.tag= Cyc_Absyn_TupleType_tag;
_temp965.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fields); _temp965;}); _temp964;});} static void* Cyc_Tcexp_tcCompoundLit( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
_tuple2* t, struct Cyc_List_List* des){ return Cyc_Tcexp_expr_err( te, loc,(
struct _tagged_string)({ char* _temp966=( char*)"tcCompoundLit"; struct
_tagged_string _temp967; _temp967.curr= _temp966; _temp967.base= _temp966;
_temp967.last_plus_one= _temp966 + 14; _temp967;}));} static void* Cyc_Tcexp_tcArray(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** elt_topt,
struct Cyc_List_List* des){ struct Cyc_List_List* es=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)((
struct Cyc_Absyn_Exp*(*)( struct _tuple6*)) Cyc_Core_snd, des); void* res= Cyc_Absyn_new_evar((
void*) Cyc_Absyn_MemKind); struct Cyc_Absyn_Const_e_struct* sz_rexp=({ struct
Cyc_Absyn_Const_e_struct* _temp968=( struct Cyc_Absyn_Const_e_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Const_e_struct)); _temp968[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp969; _temp969.tag= Cyc_Absyn_Const_e_tag; _temp969.f1=( void*)(( void*)({
struct Cyc_Absyn_Int_c_struct* _temp970=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp970[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp971; _temp971.tag= Cyc_Absyn_Int_c_tag; _temp971.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp971.f2=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es); _temp971;});
_temp970;})); _temp969;}); _temp968;}); struct Cyc_Absyn_Exp* sz_exp= Cyc_Absyn_new_exp((
void*) sz_rexp, loc); sz_exp->topt=({ struct Cyc_Core_Opt* _temp972=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp972->v=( void*) Cyc_Absyn_uint_t;
_temp972;});{ void* res_t2=( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp973=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp973[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp974; _temp974.tag= Cyc_Absyn_ArrayType_tag;
_temp974.f1=( void*) res; _temp974.f2= Cyc_Absyn_empty_tqual(); _temp974.f3=(
struct Cyc_Absyn_Exp*) sz_exp; _temp974;}); _temp973;});{ struct Cyc_List_List*
es2= es; for( 0; es2 != 0; es2=({ struct Cyc_List_List* _temp975= es2; if(
_temp975 == 0){ _throw( Null_Exception);} _temp975->tl;})){ Cyc_Tcexp_tcExpInitializer(
te, elt_topt,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp976= es2; if(
_temp976 == 0){ _throw( Null_Exception);} _temp976->hd;}));}} if( ! Cyc_Tcutil_coerce_list(
te, res, es)){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp977= es; if( _temp977 == 0){ _throw( Null_Exception);} _temp977->hd;}))->loc,({
struct _tagged_string _temp978= Cyc_Absynpp_typ2string( res); xprintf("elements of array do not all have the same type (%.*s)",
_temp978.last_plus_one - _temp978.curr, _temp978.curr);}));}{ int offset= 0;
for( 0; des != 0; offset ++, des=({ struct Cyc_List_List* _temp979= des; if(
_temp979 == 0){ _throw( Null_Exception);} _temp979->tl;})){ struct Cyc_List_List*
ds=(*(( struct _tuple6*)({ struct Cyc_List_List* _temp980= des; if( _temp980 ==
0){ _throw( Null_Exception);} _temp980->hd;}))).f1; if( ds != 0){ void* _temp982=(
void*)({ struct Cyc_List_List* _temp981= ds; if( _temp981 == 0){ _throw(
Null_Exception);} _temp981->hd;}); struct _tagged_string* _temp988; struct Cyc_Absyn_Exp*
_temp990; _LL984: if((( struct _tunion_struct*) _temp982)->tag == Cyc_Absyn_FieldName_tag){
_LL989: _temp988=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp982)->f1; goto _LL985;} else{ goto _LL986;} _LL986: if((( struct
_tunion_struct*) _temp982)->tag == Cyc_Absyn_ArrayElement_tag){ _LL991: _temp990=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayElement_struct*) _temp982)->f1;
goto _LL987;} else{ goto _LL983;} _LL985: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp992=( char*)"only array index designators are supported";
struct _tagged_string _temp993; _temp993.curr= _temp992; _temp993.base= _temp992;
_temp993.last_plus_one= _temp992 + 43; _temp993;})); goto _LL983; _LL987: Cyc_Tcexp_tcExpInitializer(
te, 0, _temp990);{ unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp990); if(
i != offset){ Cyc_Tcutil_terr( _temp990->loc, xprintf("expecting index designator %d but found %d",
offset,( int) i));} goto _LL983;} _LL983:;}}} return res_t2;}} static void* Cyc_Tcexp_tcComprehension(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp* bound, struct Cyc_Absyn_Exp* body){
Cyc_Tcexp_tcExp( te, 0, bound); if( ! Cyc_Tcutil_coerce_uint_typ( te, bound)){
Cyc_Tcutil_terr( bound->loc,({ struct _tagged_string _temp995= Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Core_Opt* _temp994= bound->topt; if( _temp994 == 0){ _throw(
Null_Exception);} _temp994->v;})); xprintf("expecting unsigned int, found %.*s",
_temp995.last_plus_one - _temp995.curr, _temp995.curr);}));} if( te->le != 0){
te= Cyc_Tcenv_new_block( loc, te); te= Cyc_Tcenv_add_local_var( loc, te, vd);}{
struct Cyc_Absyn_PtrInfo pinfo; void** topt2= 0; struct Cyc_Absyn_Tqual** tqopt=
0; if( topt != 0){ void* _temp997= Cyc_Tcutil_compress(({ void** _temp996= topt;
if( _temp996 == 0){ _throw( Null_Exception);}* _temp996;})); struct Cyc_Absyn_PtrInfo
_temp1005; struct Cyc_Absyn_Exp* _temp1007; struct Cyc_Absyn_Tqual* _temp1009;
struct Cyc_Absyn_Tqual** _temp1011; void* _temp1012; void** _temp1014; _LL999:
if(( unsigned int) _temp997 > 4u?(( struct _tunion_struct*) _temp997)->tag ==
Cyc_Absyn_PointerType_tag: 0){ _LL1006: _temp1005=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp997)->f1; goto _LL1000;} else{ goto
_LL1001;} _LL1001: if(( unsigned int) _temp997 > 4u?(( struct _tunion_struct*)
_temp997)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL1013: _temp1012=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp997)->f1; _temp1014=&(( struct Cyc_Absyn_ArrayType_struct*)
_temp997)->f1; goto _LL1010; _LL1010: _temp1009=( struct Cyc_Absyn_Tqual*)((
struct Cyc_Absyn_ArrayType_struct*) _temp997)->f2; _temp1011=&(( struct Cyc_Absyn_ArrayType_struct*)
_temp997)->f2; goto _LL1008; _LL1008: _temp1007=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp997)->f3; goto _LL1002;} else{ goto
_LL1003;} _LL1003: goto _LL1004; _LL1000: pinfo= _temp1005; topt2=( void**)((
void*)& pinfo.elt_typ); tqopt=( struct Cyc_Absyn_Tqual**)& pinfo.tq; goto _LL998;
_LL1002: topt2=( void**) _temp1014; tqopt=( struct Cyc_Absyn_Tqual**) _temp1011;
goto _LL998; _LL1004: goto _LL998; _LL998:;}{ void* t= Cyc_Tcexp_tcExp( te,
topt2, body); return( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp1015=(
struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1015[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp1016; _temp1016.tag= Cyc_Absyn_ArrayType_tag;
_temp1016.f1=( void*) t; _temp1016.f2= tqopt == 0? Cyc_Absyn_empty_tqual():({
struct Cyc_Absyn_Tqual** _temp1017= tqopt; if( _temp1017 == 0){ _throw(
Null_Exception);}* _temp1017;}); _temp1016.f3=( struct Cyc_Absyn_Exp*) bound;
_temp1016;}); _temp1015;});}}} static void* Cyc_Tcexp_tcStruct( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct _tuple1** tn, struct
Cyc_Core_Opt** otyps, struct Cyc_List_List* args, struct Cyc_Absyn_Structdecl**
sd_opt){ struct Cyc_Absyn_Structdecl* sd; if(* sd_opt != 0){ sd=( struct Cyc_Absyn_Structdecl*)({
struct Cyc_Absyn_Structdecl* _temp1018=* sd_opt; if( _temp1018 == 0){ _throw(
Null_Exception);} _temp1018;});} else{{ struct _handler_cons _temp1019;
_push_handler(& _temp1019);{ struct _xtunion_struct* _temp1020=( struct
_xtunion_struct*) setjmp( _temp1019.handler); if( ! _temp1020){ sd=* Cyc_Tcenv_lookup_structdecl(
te, loc,* tn);; _pop_handler();} else{ struct _xtunion_struct* _temp1022=
_temp1020; _LL1024: if( _temp1022->tag == Cyc_Dict_Absent_tag){ goto _LL1025;}
else{ goto _LL1026;} _LL1026: goto _LL1027; _LL1025: Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp1028= Cyc_Absynpp_qvar2string(* tn); xprintf("unbound struct name %.*s",
_temp1028.last_plus_one - _temp1028.curr, _temp1028.curr);})); return topt != 0?({
void** _temp1029= topt; if( _temp1029 == 0){ _throw( Null_Exception);}*
_temp1029;}):( void*) Cyc_Absyn_VoidType; _LL1027:( void) _throw( _temp1022);
_LL1023:;}}}* sd_opt=( struct Cyc_Absyn_Structdecl*) sd; if( sd->name != 0){* tn=(
struct _tuple1*)({ struct Cyc_Core_Opt* _temp1030= sd->name; if( _temp1030 == 0){
_throw( Null_Exception);} _temp1030->v;});}}{ struct Cyc_List_List* inst=((
struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_Absyn_Tvar*), struct
Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_make_inst_var, sd->tvs); struct Cyc_List_List*
typs=(( struct Cyc_List_List*(*)( void*(* f)( struct _tuple4*), struct Cyc_List_List*
x)) Cyc_List_map)(( void*(*)( struct _tuple4*)) Cyc_Core_snd, inst); struct Cyc_Absyn_StructType_struct*
res_typ=({ struct Cyc_Absyn_StructType_struct* _temp1031=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1031[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1032; _temp1032.tag= Cyc_Absyn_StructType_tag;
_temp1032.f1=( struct _tuple1*)* tn; _temp1032.f2= typs; _temp1032.f3=({ struct
Cyc_Absyn_Structdecl** _temp1033=( struct Cyc_Absyn_Structdecl**) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl*)); _temp1033[ 0]= sd; _temp1033;});
_temp1032;}); _temp1031;}); if( topt != 0){ Cyc_Tcutil_unify(( void*) res_typ,({
void** _temp1034= topt; if( _temp1034 == 0){ _throw( Null_Exception);}*
_temp1034;}));}* otyps=({ struct Cyc_Core_Opt* _temp1035=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1035->v=( void*) typs; _temp1035;});{
struct Cyc_List_List* fields=(( struct Cyc_List_List*(*)( struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_Absyn_Structdecl* sd)) Cyc_Tcutil_resolve_struct_designators)(
loc, args, sd); for( 0; fields != 0; fields=({ struct Cyc_List_List* _temp1036=
fields; if( _temp1036 == 0){ _throw( Null_Exception);} _temp1036->tl;})){ struct
_tuple8 _temp1040; struct Cyc_Absyn_Exp* _temp1041; struct Cyc_Absyn_Structfield*
_temp1043; struct _tuple8* _temp1038=( struct _tuple8*)({ struct Cyc_List_List*
_temp1037= fields; if( _temp1037 == 0){ _throw( Null_Exception);} _temp1037->hd;});
_temp1040=* _temp1038; _LL1044: _temp1043= _temp1040.f1; goto _LL1042; _LL1042:
_temp1041= _temp1040.f2; goto _LL1039; _LL1039: { void* inst_fieldtyp= Cyc_Tcutil_substitute(
inst,( void*) _temp1043->type); Cyc_Tcexp_tcExpInitializer( te,( void**)&
inst_fieldtyp, _temp1041); if( ! Cyc_Tcutil_coerce_arg( te, _temp1041,
inst_fieldtyp)){ Cyc_Tcutil_terr( _temp1041->loc,({ struct _tagged_string
_temp1046=* _temp1043->name; struct _tagged_string _temp1047= Cyc_Absynpp_qvar2string(*
tn); struct _tagged_string _temp1048= Cyc_Absynpp_typ2string( inst_fieldtyp);
struct _tagged_string _temp1049= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp1045= _temp1041->topt; if( _temp1045 == 0){ _throw( Null_Exception);}
_temp1045->v;})); xprintf("field %.*s of struct %.*s expects type %.*s != %.*s",
_temp1046.last_plus_one - _temp1046.curr, _temp1046.curr, _temp1047.last_plus_one
- _temp1047.curr, _temp1047.curr, _temp1048.last_plus_one - _temp1048.curr,
_temp1048.curr, _temp1049.last_plus_one - _temp1049.curr, _temp1049.curr);}));}}}
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
Cyc_Core_Opt* _temp1050=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1050->v=( void*) all_typs; _temp1050;});* exist_ref=({ struct Cyc_Core_Opt*
_temp1051=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1051->v=( void*) exist_typs; _temp1051;});{ void* res=( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp1052=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp1052[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp1053; _temp1053.tag= Cyc_Absyn_TunionFieldType_tag; _temp1053.f1=({ struct
Cyc_Absyn_TunionFieldInfo _temp1054; _temp1054.name=( struct _tuple1*)({ struct
Cyc_Core_Opt* _temp1055= ed->name; if( _temp1055 == 0){ _throw( Null_Exception);}
_temp1055->v;}); _temp1054.targs= all_typs; _temp1054.fname= ef->name; _temp1054.tud=(
struct Cyc_Absyn_Tuniondecl*) ed; _temp1054.tufd=( struct Cyc_Absyn_Tunionfield*)
ef; _temp1054;}); _temp1053;}); _temp1052;}); if( topt != 0){ void* _temp1057=
Cyc_Tcutil_compress(({ void** _temp1056= topt; if( _temp1056 == 0){ _throw(
Null_Exception);}* _temp1056;})); struct Cyc_Absyn_TunionFieldInfo _temp1065;
struct Cyc_Absyn_TunionInfo _temp1067; struct Cyc_Absyn_Tuniondecl* _temp1069;
void* _temp1071; struct Cyc_List_List* _temp1073; struct _tuple1* _temp1075;
_LL1059: if(( unsigned int) _temp1057 > 4u?(( struct _tunion_struct*) _temp1057)->tag
== Cyc_Absyn_TunionFieldType_tag: 0){ _LL1066: _temp1065=( struct Cyc_Absyn_TunionFieldInfo)((
struct Cyc_Absyn_TunionFieldType_struct*) _temp1057)->f1; goto _LL1060;} else{
goto _LL1061;} _LL1061: if(( unsigned int) _temp1057 > 4u?(( struct
_tunion_struct*) _temp1057)->tag == Cyc_Absyn_TunionType_tag: 0){ _LL1068:
_temp1067=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp1057)->f1; _LL1076: _temp1075=( struct _tuple1*) _temp1067.name; goto
_LL1074; _LL1074: _temp1073=( struct Cyc_List_List*) _temp1067.targs; goto
_LL1072; _LL1072: _temp1071=( void*) _temp1067.rgn; goto _LL1070; _LL1070:
_temp1069=( struct Cyc_Absyn_Tuniondecl*) _temp1067.tud; goto _LL1062;} else{
goto _LL1063;} _LL1063: goto _LL1064; _LL1060: Cyc_Tcutil_unify(({ void**
_temp1077= topt; if( _temp1077 == 0){ _throw( Null_Exception);}* _temp1077;}),
res); goto _LL1058; _LL1062:{ struct Cyc_List_List* a= all_typs; for( 0; a != 0?
_temp1073 != 0: 0; a=({ struct Cyc_List_List* _temp1078= a; if( _temp1078 == 0){
_throw( Null_Exception);} _temp1078->tl;}), _temp1073=({ struct Cyc_List_List*
_temp1079= _temp1073; if( _temp1079 == 0){ _throw( Null_Exception);} _temp1079->tl;})){
Cyc_Tcutil_unify(( void*)({ struct Cyc_List_List* _temp1080= a; if( _temp1080 ==
0){ _throw( Null_Exception);} _temp1080->hd;}),( void*)({ struct Cyc_List_List*
_temp1081= _temp1073; if( _temp1081 == 0){ _throw( Null_Exception);} _temp1081->hd;}));}}
if( ef->typs == 0? es == 0: 0){ e->topt=({ struct Cyc_Core_Opt* _temp1082=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1082->v=(
void*) res; _temp1082;}); res=( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp1083=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp1083[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp1084; _temp1084.tag=
Cyc_Absyn_TunionType_tag; _temp1084.f1=({ struct Cyc_Absyn_TunionInfo _temp1085;
_temp1085.name=( struct _tuple1*)({ struct Cyc_Core_Opt* _temp1086= ed->name;
if( _temp1086 == 0){ _throw( Null_Exception);} _temp1086->v;}); _temp1085.targs=
all_typs; _temp1085.rgn=( void*) Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind);
_temp1085.tud=( struct Cyc_Absyn_Tuniondecl*) ed; _temp1085;}); _temp1084;});
_temp1083;}); Cyc_Tcutil_unchecked_cast( te, e, res); return res;} goto _LL1058;
_LL1064: goto _LL1058; _LL1058:;}{ struct Cyc_List_List* ts= ef->typs; for( 0;
es != 0? ts != 0: 0; es=({ struct Cyc_List_List* _temp1087= es; if( _temp1087 ==
0){ _throw( Null_Exception);} _temp1087->tl;}), ts=({ struct Cyc_List_List*
_temp1088= ts; if( _temp1088 == 0){ _throw( Null_Exception);} _temp1088->tl;})){
struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1089= es; if( _temp1089 == 0){ _throw( Null_Exception);} _temp1089->hd;});
void* t= Cyc_Tcutil_substitute( inst,(*(( struct _tuple7*)({ struct Cyc_List_List*
_temp1090= ts; if( _temp1090 == 0){ _throw( Null_Exception);} _temp1090->hd;}))).f2);
Cyc_Tcexp_tcExpInitializer( te,( void**)& t, e); if( ! Cyc_Tcutil_coerce_arg( te,
e, t)){ Cyc_Tcutil_terr( e->loc,({ struct _tagged_string _temp1094= Cyc_Absynpp_qvar2string(
ef->name); struct _tagged_string _temp1095= Cyc_Absynpp_typ2string( t); struct
_tagged_string _temp1096= e->topt == 0?( struct _tagged_string)({ char*
_temp1091=( char*)"?"; struct _tagged_string _temp1092; _temp1092.curr=
_temp1091; _temp1092.base= _temp1091; _temp1092.last_plus_one= _temp1091 + 2;
_temp1092;}): Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt* _temp1093=
e->topt; if( _temp1093 == 0){ _throw( Null_Exception);} _temp1093->v;}));
xprintf("tunion constructor %.*s expects argument of type %.*s but this argument has type %.*s",
_temp1094.last_plus_one - _temp1094.curr, _temp1094.curr, _temp1095.last_plus_one
- _temp1095.curr, _temp1095.curr, _temp1096.last_plus_one - _temp1096.curr,
_temp1096.curr);}));}} if( es != 0){ return Cyc_Tcexp_expr_err( te,(( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1097= es; if( _temp1097 == 0){ _throw( Null_Exception);}
_temp1097->hd;}))->loc,({ struct _tagged_string _temp1098= Cyc_Absynpp_qvar2string(
ef->name); xprintf("too many arguments for tunion constructor %.*s", _temp1098.last_plus_one
- _temp1098.curr, _temp1098.curr);}));} if( ts != 0){ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp1099= Cyc_Absynpp_qvar2string( ef->name);
xprintf("too few arguments for tunion constructor %.*s", _temp1099.last_plus_one
- _temp1099.curr, _temp1099.curr);}));} return res;}}} static void* Cyc_Tcexp_tcXTunion(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Core_Opt** exist_ref, struct Cyc_List_List* es, struct Cyc_Absyn_XTuniondecl*
xd, struct Cyc_Absyn_Tunionfield* ef){ struct Cyc_List_List* exist_inst=((
struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_Absyn_Tvar*), struct
Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_make_inst_var, ef->tvs); struct Cyc_List_List*
exist_typs=(( struct Cyc_List_List*(*)( void*(* f)( struct _tuple4*), struct Cyc_List_List*
x)) Cyc_List_map)(( void*(*)( struct _tuple4*)) Cyc_Core_snd, exist_inst);
struct Cyc_Absyn_XTunionFieldType_struct* res=({ struct Cyc_Absyn_XTunionFieldType_struct*
_temp1100=( struct Cyc_Absyn_XTunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_XTunionFieldType_struct)); _temp1100[ 0]=({ struct Cyc_Absyn_XTunionFieldType_struct
_temp1101; _temp1101.tag= Cyc_Absyn_XTunionFieldType_tag; _temp1101.f1=({ struct
Cyc_Absyn_XTunionFieldInfo _temp1102; _temp1102.name= xd->name; _temp1102.fname=
ef->name; _temp1102.xtud=( struct Cyc_Absyn_XTuniondecl*) xd; _temp1102.xtufd=(
struct Cyc_Absyn_Tunionfield*) ef; _temp1102;}); _temp1101;}); _temp1100;});
struct Cyc_List_List* ts= ef->typs;* exist_ref=({ struct Cyc_Core_Opt* _temp1103=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1103->v=(
void*) exist_typs; _temp1103;}); while( es != 0? ts != 0: 0) { struct Cyc_Absyn_Exp*
e=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1104= es; if( _temp1104
== 0){ _throw( Null_Exception);} _temp1104->hd;}); void* t= Cyc_Tcutil_substitute(
exist_inst,(*(( struct _tuple7*)({ struct Cyc_List_List* _temp1105= ts; if(
_temp1105 == 0){ _throw( Null_Exception);} _temp1105->hd;}))).f2); Cyc_Tcexp_tcExpInitializer(
te,( void**)& t, e); es=({ struct Cyc_List_List* _temp1106= es; if( _temp1106 ==
0){ _throw( Null_Exception);} _temp1106->tl;}); ts=({ struct Cyc_List_List*
_temp1107= ts; if( _temp1107 == 0){ _throw( Null_Exception);} _temp1107->tl;});
if( ! Cyc_Tcutil_coerce_arg( te, e, t)){ Cyc_Tcutil_terr( e->loc,({ struct
_tagged_string _temp1109= Cyc_Absynpp_qvar2string( ef->name); struct
_tagged_string _temp1110= Cyc_Absynpp_typ2string( t); struct _tagged_string
_temp1111= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt* _temp1108= e->topt;
if( _temp1108 == 0){ _throw( Null_Exception);} _temp1108->v;})); xprintf("xtunion constructor %.*s expects argument of type %.*s but this argument has type %.*s",
_temp1109.last_plus_one - _temp1109.curr, _temp1109.curr, _temp1110.last_plus_one
- _temp1110.curr, _temp1110.curr, _temp1111.last_plus_one - _temp1111.curr,
_temp1111.curr);}));}} if( es != 0){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1112= es; if( _temp1112 == 0){ _throw( Null_Exception);}
_temp1112->hd;}))->loc,({ struct _tagged_string _temp1113= Cyc_Absynpp_qvar2string(
ef->name); xprintf("too many arguments for xtunion constructor %.*s", _temp1113.last_plus_one
- _temp1113.curr, _temp1113.curr);}));} else{ if( ts != 0){ Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1114= es; if( _temp1114 == 0){
_throw( Null_Exception);} _temp1114->hd;}))->loc,({ struct _tagged_string
_temp1115= Cyc_Absynpp_qvar2string( ef->name); xprintf("too few arguments for xtunion constructor %.*s",
_temp1115.last_plus_one - _temp1115.curr, _temp1115.curr);}));}} return( void*)
res;} static void* Cyc_Tcexp_tcMalloc( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* t){ Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars(
te),( void*) Cyc_Absyn_MemKind, t);{ void* _temp1116= t; struct Cyc_Absyn_TunionFieldInfo
_temp1124; struct Cyc_Absyn_Tunionfield* _temp1126; struct Cyc_Absyn_Tuniondecl*
_temp1128; struct _tuple1* _temp1130; struct Cyc_List_List* _temp1132; struct
_tuple1* _temp1134; struct Cyc_Absyn_XTunionFieldInfo _temp1136; struct Cyc_Absyn_Tunionfield*
_temp1138; struct Cyc_Absyn_XTuniondecl* _temp1140; struct _tuple1* _temp1142;
struct _tuple1* _temp1144; _LL1118: if(( unsigned int) _temp1116 > 4u?(( struct
_tunion_struct*) _temp1116)->tag == Cyc_Absyn_TunionFieldType_tag: 0){ _LL1125:
_temp1124=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp1116)->f1; _LL1135: _temp1134=( struct _tuple1*) _temp1124.name; goto
_LL1133; _LL1133: _temp1132=( struct Cyc_List_List*) _temp1124.targs; goto
_LL1131; _LL1131: _temp1130=( struct _tuple1*) _temp1124.fname; goto _LL1129;
_LL1129: _temp1128=( struct Cyc_Absyn_Tuniondecl*) _temp1124.tud; goto _LL1127;
_LL1127: _temp1126=( struct Cyc_Absyn_Tunionfield*) _temp1124.tufd; goto _LL1119;}
else{ goto _LL1120;} _LL1120: if(( unsigned int) _temp1116 > 4u?(( struct
_tunion_struct*) _temp1116)->tag == Cyc_Absyn_XTunionFieldType_tag: 0){ _LL1137:
_temp1136=( struct Cyc_Absyn_XTunionFieldInfo)(( struct Cyc_Absyn_XTunionFieldType_struct*)
_temp1116)->f1; _LL1145: _temp1144=( struct _tuple1*) _temp1136.name; goto
_LL1143; _LL1143: _temp1142=( struct _tuple1*) _temp1136.fname; goto _LL1141;
_LL1141: _temp1140=( struct Cyc_Absyn_XTuniondecl*) _temp1136.xtud; goto _LL1139;
_LL1139: _temp1138=( struct Cyc_Absyn_Tunionfield*) _temp1136.xtufd; goto
_LL1121;} else{ goto _LL1122;} _LL1122: goto _LL1123; _LL1119: _temp1138=
_temp1126; goto _LL1121; _LL1121: if(({ struct Cyc_Absyn_Tunionfield* _temp1146=
_temp1138; if( _temp1146 == 0){ _throw( Null_Exception);} _temp1146->tvs;}) != 0){
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp1147=( char*)"malloc with existential types not yet implemented";
struct _tagged_string _temp1148; _temp1148.curr= _temp1147; _temp1148.base=
_temp1147; _temp1148.last_plus_one= _temp1147 + 50; _temp1148;}));} goto _LL1117;
_LL1123: goto _LL1117; _LL1117:;}{ void*(* ptr_maker)( void* t, void* rgn,
struct Cyc_Absyn_Tqual* tq)= Cyc_Absyn_at_typ; if( topt != 0){ void* _temp1150=
Cyc_Tcutil_compress(({ void** _temp1149= topt; if( _temp1149 == 0){ _throw(
Null_Exception);}* _temp1149;})); struct Cyc_Absyn_PtrInfo _temp1156; struct Cyc_Absyn_Conref*
_temp1158; struct Cyc_Absyn_Tqual* _temp1160; struct Cyc_Absyn_Conref* _temp1162;
void* _temp1164; void* _temp1166; _LL1152: if(( unsigned int) _temp1150 > 4u?((
struct _tunion_struct*) _temp1150)->tag == Cyc_Absyn_PointerType_tag: 0){
_LL1157: _temp1156=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1150)->f1; _LL1167: _temp1166=( void*) _temp1156.elt_typ; goto _LL1165;
_LL1165: _temp1164=( void*) _temp1156.rgn_typ; goto _LL1163; _LL1163: _temp1162=(
struct Cyc_Absyn_Conref*) _temp1156.nullable; goto _LL1161; _LL1161: _temp1160=(
struct Cyc_Absyn_Tqual*) _temp1156.tq; goto _LL1159; _LL1159: _temp1158=( struct
Cyc_Absyn_Conref*) _temp1156.bounds; goto _LL1153;} else{ goto _LL1154;} _LL1154:
goto _LL1155; _LL1153: if((( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1162)){ ptr_maker= Cyc_Absyn_star_typ;} goto _LL1151; _LL1155: goto _LL1151;
_LL1151:;} return ptr_maker( t,( void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual());}}
static void* Cyc_Tcexp_tcStmtExp( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Stmt* s){ Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser(
te, s), s, 1); Cyc_NewControlFlow_set_encloser( s, Cyc_Tcenv_get_encloser( te));
while( 1) { void* _temp1168=( void*) s->r; struct Cyc_Absyn_Exp* _temp1178;
struct Cyc_Absyn_Stmt* _temp1180; struct Cyc_Absyn_Stmt* _temp1182; struct Cyc_Absyn_Stmt*
_temp1184; struct Cyc_Absyn_Decl* _temp1186; _LL1170: if(( unsigned int)
_temp1168 > 1u?(( struct _tunion_struct*) _temp1168)->tag == Cyc_Absyn_Exp_s_tag:
0){ _LL1179: _temp1178=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp1168)->f1; goto _LL1171;} else{ goto _LL1172;} _LL1172: if(( unsigned int)
_temp1168 > 1u?(( struct _tunion_struct*) _temp1168)->tag == Cyc_Absyn_Seq_s_tag:
0){ _LL1183: _temp1182=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp1168)->f1; goto _LL1181; _LL1181: _temp1180=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp1168)->f2; goto _LL1173;} else{ goto
_LL1174;} _LL1174: if(( unsigned int) _temp1168 > 1u?(( struct _tunion_struct*)
_temp1168)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL1187: _temp1186=( struct Cyc_Absyn_Decl*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1168)->f1; goto _LL1185; _LL1185:
_temp1184=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1168)->f2;
goto _LL1175;} else{ goto _LL1176;} _LL1176: goto _LL1177; _LL1171: return( void*)({
struct Cyc_Core_Opt* _temp1188= _temp1178->topt; if( _temp1188 == 0){ _throw(
Null_Exception);} _temp1188->v;}); _LL1173: s= _temp1180; continue; _LL1175: s=
_temp1184; continue; _LL1177: return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_string)({ char* _temp1189=( char*)"statement expression must end with expression";
struct _tagged_string _temp1190; _temp1190.curr= _temp1189; _temp1190.base=
_temp1189; _temp1190.last_plus_one= _temp1189 + 46; _temp1190;})); _LL1169:;}}
static void* Cyc_Tcexp_tcCodegen( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Fndecl* fd){ return Cyc_Tcexp_expr_err( te,
loc,( struct _tagged_string)({ char* _temp1191=( char*)"tcCodegen"; struct
_tagged_string _temp1192; _temp1192.curr= _temp1191; _temp1192.base= _temp1191;
_temp1192.last_plus_one= _temp1191 + 10; _temp1192;}));} static void* Cyc_Tcexp_tcFill(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e){ return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({
char* _temp1193=( char*)"tcFill"; struct _tagged_string _temp1194; _temp1194.curr=
_temp1193; _temp1194.base= _temp1193; _temp1194.last_plus_one= _temp1193 + 7;
_temp1194;}));} static void* Cyc_Tcexp_tcNew( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Exp*
e1){ void* _temp1195=( void*) e1->r; struct Cyc_Absyn_Exp* _temp1207; struct Cyc_Absyn_Exp*
_temp1209; struct Cyc_Absyn_Vardecl* _temp1211; struct Cyc_List_List* _temp1213;
struct Cyc_Core_Opt* _temp1215; struct Cyc_List_List* _temp1217; void* _temp1219;
struct _tagged_string _temp1221; _LL1197: if((( struct _tunion_struct*)
_temp1195)->tag == Cyc_Absyn_Comprehension_e_tag){ _LL1212: _temp1211=( struct
Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1195)->f1;
goto _LL1210; _LL1210: _temp1209=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1195)->f2; goto _LL1208; _LL1208: _temp1207=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1195)->f3; goto _LL1198;} else{
goto _LL1199;} _LL1199: if((( struct _tunion_struct*) _temp1195)->tag == Cyc_Absyn_UnresolvedMem_e_tag){
_LL1216: _temp1215=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1195)->f1; goto _LL1214; _LL1214: _temp1213=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp1195)->f2; goto _LL1200;} else{
goto _LL1201;} _LL1201: if((( struct _tunion_struct*) _temp1195)->tag == Cyc_Absyn_Array_e_tag){
_LL1218: _temp1217=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp1195)->f1; goto _LL1202;} else{ goto _LL1203;} _LL1203: if((( struct
_tunion_struct*) _temp1195)->tag == Cyc_Absyn_Const_e_tag){ _LL1220: _temp1219=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1195)->f1; if(( unsigned int)
_temp1219 > 1u?(( struct _tunion_struct*) _temp1219)->tag == Cyc_Absyn_String_c_tag:
0){ _LL1222: _temp1221=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp1219)->f1; goto _LL1204;} else{ goto _LL1205;}} else{ goto _LL1205;}
_LL1205: goto _LL1206; _LL1198: { void* t_arr= Cyc_Tcexp_tcExpNoPromote( te,
topt, e1); void* _temp1223= Cyc_Tcutil_compress( t_arr); struct Cyc_Absyn_Exp*
_temp1229; struct Cyc_Absyn_Tqual* _temp1231; void* _temp1233; _LL1225: if((
unsigned int) _temp1223 > 4u?(( struct _tunion_struct*) _temp1223)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL1234: _temp1233=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1223)->f1;
goto _LL1232; _LL1232: _temp1231=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1223)->f2; goto _LL1230; _LL1230: _temp1229=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1223)->f3; goto _LL1226;} else{ goto
_LL1227;} _LL1227: goto _LL1228; _LL1226: { void* b= Cyc_Tcutil_is_const_exp( te,
_temp1209)?( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1235=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1235[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1236; _temp1236.tag= Cyc_Absyn_Upper_b_tag;
_temp1236.f1= _temp1209; _temp1236;}); _temp1235;}):( void*) Cyc_Absyn_Unknown_b;
void* res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1237=( struct
Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1237[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1238; _temp1238.tag=
Cyc_Absyn_PointerType_tag; _temp1238.f1=({ struct Cyc_Absyn_PtrInfo _temp1239;
_temp1239.elt_typ=( void*) _temp1233; _temp1239.rgn_typ=( void*)(( void*) Cyc_Absyn_HeapRgn);
_temp1239.nullable=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp1239.tq= _temp1231; _temp1239.bounds=(( struct Cyc_Absyn_Conref*(*)( void*
x)) Cyc_Absyn_new_conref)( b); _temp1239;}); _temp1238;}); _temp1237;}); if(
topt != 0){ if( ! Cyc_Tcutil_unify(({ void** _temp1240= topt; if( _temp1240 == 0){
_throw( Null_Exception);}* _temp1240;}), res_typ)? Cyc_Tcutil_silent_castable(
te, loc, res_typ,({ void** _temp1241= topt; if( _temp1241 == 0){ _throw(
Null_Exception);}* _temp1241;})): 0){ e->topt=({ struct Cyc_Core_Opt* _temp1242=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1242->v=(
void*) res_typ; _temp1242;}); Cyc_Tcutil_unchecked_cast( te, e,({ void**
_temp1243= topt; if( _temp1243 == 0){ _throw( Null_Exception);}* _temp1243;}));
res_typ=({ void** _temp1244= topt; if( _temp1244 == 0){ _throw( Null_Exception);}*
_temp1244;});}} return res_typ;} _LL1228: return(( void*(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp1245=(
char*)"tcNew: comprehension returned non-array type"; struct _tagged_string
_temp1246; _temp1246.curr= _temp1245; _temp1246.base= _temp1245; _temp1246.last_plus_one=
_temp1245 + 45; _temp1246;})); _LL1224:;} _LL1200:( void*)( e1->r=( void*)((
void*)({ struct Cyc_Absyn_Array_e_struct* _temp1247=( struct Cyc_Absyn_Array_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct)); _temp1247[ 0]=({ struct
Cyc_Absyn_Array_e_struct _temp1248; _temp1248.tag= Cyc_Absyn_Array_e_tag;
_temp1248.f1= _temp1213; _temp1248;}); _temp1247;}))); _temp1217= _temp1213;
goto _LL1202; _LL1202: { void** elt_typ_opt= 0; if( topt != 0){ void* _temp1250=
Cyc_Tcutil_compress(({ void** _temp1249= topt; if( _temp1249 == 0){ _throw(
Null_Exception);}* _temp1249;})); struct Cyc_Absyn_PtrInfo _temp1256; struct Cyc_Absyn_Conref*
_temp1258; struct Cyc_Absyn_Tqual* _temp1260; struct Cyc_Absyn_Conref* _temp1262;
void* _temp1264; void* _temp1266; void** _temp1268; _LL1252: if(( unsigned int)
_temp1250 > 4u?(( struct _tunion_struct*) _temp1250)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL1257: _temp1256=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1250)->f1; _LL1267: _temp1266=( void*) _temp1256.elt_typ; _temp1268=&(((
struct Cyc_Absyn_PointerType_struct*) _temp1250)->f1).elt_typ; goto _LL1265;
_LL1265: _temp1264=( void*) _temp1256.rgn_typ; goto _LL1263; _LL1263: _temp1262=(
struct Cyc_Absyn_Conref*) _temp1256.nullable; goto _LL1261; _LL1261: _temp1260=(
struct Cyc_Absyn_Tqual*) _temp1256.tq; goto _LL1259; _LL1259: _temp1258=( struct
Cyc_Absyn_Conref*) _temp1256.bounds; goto _LL1253;} else{ goto _LL1254;} _LL1254:
goto _LL1255; _LL1253: elt_typ_opt=( void**) _temp1268; goto _LL1251; _LL1255:
goto _LL1251; _LL1251:;}{ void* t_array= Cyc_Tcexp_tcExpNoPromote( te,
elt_typ_opt, e1); void* res_typ;{ void* _temp1269= Cyc_Tcutil_compress( t_array);
struct Cyc_Absyn_Exp* _temp1275; struct Cyc_Absyn_Tqual* _temp1277; void*
_temp1279; _LL1271: if(( unsigned int) _temp1269 > 4u?(( struct _tunion_struct*)
_temp1269)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL1280: _temp1279=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1269)->f1; goto _LL1278; _LL1278:
_temp1277=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1269)->f2; goto _LL1276; _LL1276: _temp1275=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1269)->f3; goto _LL1272;} else{ goto
_LL1273;} _LL1273: goto _LL1274; _LL1272: res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp1281=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1281[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1282; _temp1282.tag=
Cyc_Absyn_PointerType_tag; _temp1282.f1=({ struct Cyc_Absyn_PtrInfo _temp1283;
_temp1283.elt_typ=( void*) _temp1279; _temp1283.rgn_typ=( void*)(( void*) Cyc_Absyn_HeapRgn);
_temp1283.nullable=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp1283.tq= _temp1277; _temp1283.bounds=(( struct Cyc_Absyn_Conref*(*)( void*
x)) Cyc_Absyn_new_conref)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1284=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1284[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1285; _temp1285.tag= Cyc_Absyn_Upper_b_tag;
_temp1285.f1=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1286=
_temp1275; if( _temp1286 == 0){ _throw( Null_Exception);} _temp1286;});
_temp1285;}); _temp1284;})); _temp1283;}); _temp1282;}); _temp1281;}); if( topt
!= 0){ if( ! Cyc_Tcutil_unify(({ void** _temp1287= topt; if( _temp1287 == 0){
_throw( Null_Exception);}* _temp1287;}), res_typ)? Cyc_Tcutil_silent_castable(
te, loc, res_typ,({ void** _temp1288= topt; if( _temp1288 == 0){ _throw(
Null_Exception);}* _temp1288;})): 0){ e->topt=({ struct Cyc_Core_Opt* _temp1289=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1289->v=(
void*) res_typ; _temp1289;}); Cyc_Tcutil_unchecked_cast( te, e,({ void**
_temp1290= topt; if( _temp1290 == 0){ _throw( Null_Exception);}* _temp1290;}));
res_typ=({ void** _temp1291= topt; if( _temp1291 == 0){ _throw( Null_Exception);}*
_temp1291;});}} goto _LL1270; _LL1274: return(( void*(*)( struct _tagged_string))
Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp1292=( char*)"tcExpNoPromote on Array_e returned non-array type";
struct _tagged_string _temp1293; _temp1293.curr= _temp1292; _temp1293.base=
_temp1292; _temp1293.last_plus_one= _temp1292 + 50; _temp1293;})); _LL1270:;}
return res_typ;}} _LL1204: { void* topt2= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t,(
void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual(),( void*) Cyc_Absyn_Unknown_b);
void* t= Cyc_Tcexp_tcExp( te,( void**)& topt2, e1); return Cyc_Absyn_atb_typ( t,(
void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp1294=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1294[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1295; _temp1295.tag= Cyc_Absyn_Upper_b_tag;
_temp1295.f1= Cyc_Absyn_uint_exp( 1, 0); _temp1295;}); _temp1294;}));} _LL1206: {
void** topt2= 0; if( topt != 0){ void* _temp1297= Cyc_Tcutil_compress(({ void**
_temp1296= topt; if( _temp1296 == 0){ _throw( Null_Exception);}* _temp1296;}));
struct Cyc_Absyn_PtrInfo _temp1305; struct Cyc_Absyn_Conref* _temp1307; struct
Cyc_Absyn_Tqual* _temp1309; struct Cyc_Absyn_Conref* _temp1311; void* _temp1313;
void* _temp1315; void** _temp1317; struct Cyc_Absyn_TunionInfo _temp1318;
_LL1299: if(( unsigned int) _temp1297 > 4u?(( struct _tunion_struct*) _temp1297)->tag
== Cyc_Absyn_PointerType_tag: 0){ _LL1306: _temp1305=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1297)->f1; _LL1316: _temp1315=( void*)
_temp1305.elt_typ; _temp1317=&((( struct Cyc_Absyn_PointerType_struct*)
_temp1297)->f1).elt_typ; goto _LL1314; _LL1314: _temp1313=( void*) _temp1305.rgn_typ;
goto _LL1312; _LL1312: _temp1311=( struct Cyc_Absyn_Conref*) _temp1305.nullable;
goto _LL1310; _LL1310: _temp1309=( struct Cyc_Absyn_Tqual*) _temp1305.tq; goto
_LL1308; _LL1308: _temp1307=( struct Cyc_Absyn_Conref*) _temp1305.bounds; goto
_LL1300;} else{ goto _LL1301;} _LL1301: if(( unsigned int) _temp1297 > 4u?((
struct _tunion_struct*) _temp1297)->tag == Cyc_Absyn_TunionType_tag: 0){ _LL1319:
_temp1318=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp1297)->f1; goto _LL1302;} else{ goto _LL1303;} _LL1303: goto _LL1304;
_LL1300: topt2=( void**) _temp1317; goto _LL1298; _LL1302: topt2=({ void**
_temp1320=( void**) GC_malloc( sizeof( void*)); _temp1320[ 0]=({ void**
_temp1321= topt; if( _temp1321 == 0){ _throw( Null_Exception);}* _temp1321;});
_temp1320;}); goto _LL1298; _LL1304: goto _LL1298; _LL1298:;}{ void* telt= Cyc_Tcexp_tcExp(
te, topt2, e1); void* res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp1322=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1322[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1323; _temp1323.tag=
Cyc_Absyn_PointerType_tag; _temp1323.f1=({ struct Cyc_Absyn_PtrInfo _temp1324;
_temp1324.elt_typ=( void*) telt; _temp1324.rgn_typ=( void*)(( void*) Cyc_Absyn_HeapRgn);
_temp1324.nullable=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp1324.tq= Cyc_Absyn_empty_tqual(); _temp1324.bounds=(( struct Cyc_Absyn_Conref*(*)(
void* x)) Cyc_Absyn_new_conref)(( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp1325=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1325[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1326; _temp1326.tag= Cyc_Absyn_Upper_b_tag;
_temp1326.f1= Cyc_Absyn_uint_exp( 1, 0); _temp1326;}); _temp1325;})); _temp1324;});
_temp1323;}); _temp1322;}); if( topt != 0){ if( ! Cyc_Tcutil_unify(({ void**
_temp1327= topt; if( _temp1327 == 0){ _throw( Null_Exception);}* _temp1327;}),
res_typ)? Cyc_Tcutil_silent_castable( te, loc, res_typ,({ void** _temp1328= topt;
if( _temp1328 == 0){ _throw( Null_Exception);}* _temp1328;})): 0){ e->topt=({
struct Cyc_Core_Opt* _temp1329=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1329->v=( void*) res_typ; _temp1329;}); Cyc_Tcutil_unchecked_cast(
te, e,({ void** _temp1330= topt; if( _temp1330 == 0){ _throw( Null_Exception);}*
_temp1330;})); res_typ=({ void** _temp1331= topt; if( _temp1331 == 0){ _throw(
Null_Exception);}* _temp1331;});}} return res_typ;}} _LL1196:;} void* Cyc_Tcexp_tcExp(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcexp_tcExpNoPromote(
te, topt, e); t= Cyc_Tcutil_compress( t);{ void* _temp1332= t; struct Cyc_Absyn_Exp*
_temp1338; struct Cyc_Absyn_Tqual* _temp1340; void* _temp1342; _LL1334: if((
unsigned int) _temp1332 > 4u?(( struct _tunion_struct*) _temp1332)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL1343: _temp1342=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1332)->f1;
goto _LL1341; _LL1341: _temp1340=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1332)->f2; goto _LL1339; _LL1339: _temp1338=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1332)->f3; goto _LL1335;} else{ goto
_LL1336;} _LL1336: goto _LL1337; _LL1335: { void* _temp1346; int _temp1348;
struct _tuple5 _temp1344= Cyc_Tcutil_addressof_props( te, e); _LL1349: _temp1348=
_temp1344.f1; goto _LL1347; _LL1347: _temp1346= _temp1344.f2; goto _LL1345;
_LL1345: { void* b= _temp1338 == 0?( void*) Cyc_Absyn_Unknown_b:( void*)({
struct Cyc_Absyn_Upper_b_struct* _temp1350=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1350[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1351; _temp1351.tag= Cyc_Absyn_Upper_b_tag;
_temp1351.f1=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1352=
_temp1338; if( _temp1352 == 0){ _throw( Null_Exception);} _temp1352;});
_temp1351;}); _temp1350;}); t= Cyc_Absyn_atb_typ( _temp1342, _temp1346,
_temp1340, b); Cyc_Tcutil_unchecked_cast( te, e, t); return t;}} _LL1337: return
t; _LL1333:;}} void* Cyc_Tcexp_tcExpInitializer( struct Cyc_Tcenv_Tenv* te, void**
topt, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcexp_tcExpNoPromote( te, topt, e);
void* _temp1353=( void*) e->r; struct Cyc_List_List* _temp1363; struct Cyc_Absyn_Exp*
_temp1365; struct Cyc_Absyn_Exp* _temp1367; struct Cyc_Absyn_Vardecl* _temp1369;
void* _temp1371; struct _tagged_string _temp1373; _LL1355: if((( struct
_tunion_struct*) _temp1353)->tag == Cyc_Absyn_Array_e_tag){ _LL1364: _temp1363=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1353)->f1; goto
_LL1356;} else{ goto _LL1357;} _LL1357: if((( struct _tunion_struct*) _temp1353)->tag
== Cyc_Absyn_Comprehension_e_tag){ _LL1370: _temp1369=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1353)->f1; goto _LL1368; _LL1368:
_temp1367=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1353)->f2; goto _LL1366; _LL1366: _temp1365=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1353)->f3; goto _LL1358;} else{
goto _LL1359;} _LL1359: if((( struct _tunion_struct*) _temp1353)->tag == Cyc_Absyn_Const_e_tag){
_LL1372: _temp1371=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1353)->f1;
if(( unsigned int) _temp1371 > 1u?(( struct _tunion_struct*) _temp1371)->tag ==
Cyc_Absyn_String_c_tag: 0){ _LL1374: _temp1373=( struct _tagged_string)(( struct
Cyc_Absyn_String_c_struct*) _temp1371)->f1; goto _LL1360;} else{ goto _LL1361;}}
else{ goto _LL1361;} _LL1361: goto _LL1362; _LL1356: return t; _LL1358: return t;
_LL1360: return t; _LL1362: t= Cyc_Tcutil_compress( t);{ void* _temp1375= t;
struct Cyc_Absyn_Exp* _temp1381; struct Cyc_Absyn_Tqual* _temp1383; void*
_temp1385; _LL1377: if(( unsigned int) _temp1375 > 4u?(( struct _tunion_struct*)
_temp1375)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL1386: _temp1385=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1375)->f1; goto _LL1384; _LL1384:
_temp1383=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1375)->f2; goto _LL1382; _LL1382: _temp1381=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1375)->f3; goto _LL1378;} else{ goto
_LL1379;} _LL1379: goto _LL1380; _LL1378: { void* _temp1389; int _temp1391;
struct _tuple5 _temp1387= Cyc_Tcutil_addressof_props( te, e); _LL1392: _temp1391=
_temp1387.f1; goto _LL1390; _LL1390: _temp1389= _temp1387.f2; goto _LL1388;
_LL1388: { void* b= _temp1381 == 0?( void*) Cyc_Absyn_Unknown_b:( void*)({
struct Cyc_Absyn_Upper_b_struct* _temp1393=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1393[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1394; _temp1394.tag= Cyc_Absyn_Upper_b_tag;
_temp1394.f1=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1395=
_temp1381; if( _temp1395 == 0){ _throw( Null_Exception);} _temp1395;});
_temp1394;}); _temp1393;}); t= Cyc_Absyn_atb_typ( _temp1385, _temp1389,
_temp1383, b); Cyc_Tcutil_unchecked_cast( te, e, t); return t;}} _LL1380: return
t; _LL1376:;} _LL1354:;} static void* Cyc_Tcexp_tcExpNoPromote( struct Cyc_Tcenv_Tenv*
te, void** topt, struct Cyc_Absyn_Exp* e){{ void* _temp1396=( void*) e->r;
struct Cyc_Absyn_Exp* _temp1402; _LL1398: if((( struct _tunion_struct*)
_temp1396)->tag == Cyc_Absyn_NoInstantiate_e_tag){ _LL1403: _temp1402=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*) _temp1396)->f1; goto
_LL1399;} else{ goto _LL1400;} _LL1400: goto _LL1401; _LL1399: Cyc_Tcexp_tcExpNoInst(
te, topt, _temp1402); e->topt= _temp1402->topt; goto _LL1397; _LL1401: Cyc_Tcexp_tcExpNoInst(
te, topt, e);( void*)({ struct Cyc_Core_Opt* _temp1404= e->topt; if( _temp1404
== 0){ _throw( Null_Exception);} _temp1404->v=( void*) Cyc_Absyn_pointer_expand(
Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp1405= e->topt; if(
_temp1405 == 0){ _throw( Null_Exception);} _temp1405->v;})));});{ void*
_temp1407= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp1406= e->topt;
if( _temp1406 == 0){ _throw( Null_Exception);} _temp1406->v;})); struct Cyc_Absyn_PtrInfo
_temp1413; struct Cyc_Absyn_Conref* _temp1415; struct Cyc_Absyn_Tqual* _temp1417;
struct Cyc_Absyn_Conref* _temp1419; void* _temp1421; void* _temp1423; _LL1409:
if(( unsigned int) _temp1407 > 4u?(( struct _tunion_struct*) _temp1407)->tag ==
Cyc_Absyn_PointerType_tag: 0){ _LL1414: _temp1413=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1407)->f1; _LL1424: _temp1423=( void*)
_temp1413.elt_typ; goto _LL1422; _LL1422: _temp1421=( void*) _temp1413.rgn_typ;
goto _LL1420; _LL1420: _temp1419=( struct Cyc_Absyn_Conref*) _temp1413.nullable;
goto _LL1418; _LL1418: _temp1417=( struct Cyc_Absyn_Tqual*) _temp1413.tq; goto
_LL1416; _LL1416: _temp1415=( struct Cyc_Absyn_Conref*) _temp1413.bounds; goto
_LL1410;} else{ goto _LL1411;} _LL1411: goto _LL1412; _LL1410:{ void* _temp1425=
Cyc_Tcutil_compress( _temp1423); struct Cyc_Absyn_FnInfo _temp1431; struct Cyc_List_List*
_temp1433; int _temp1435; struct Cyc_List_List* _temp1437; void* _temp1439;
struct Cyc_Core_Opt* _temp1441; struct Cyc_List_List* _temp1443; _LL1427: if((
unsigned int) _temp1425 > 4u?(( struct _tunion_struct*) _temp1425)->tag == Cyc_Absyn_FnType_tag:
0){ _LL1432: _temp1431=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp1425)->f1; _LL1444: _temp1443=( struct Cyc_List_List*) _temp1431.tvars;
goto _LL1442; _LL1442: _temp1441=( struct Cyc_Core_Opt*) _temp1431.effect; goto
_LL1440; _LL1440: _temp1439=( void*) _temp1431.ret_typ; goto _LL1438; _LL1438:
_temp1437=( struct Cyc_List_List*) _temp1431.args; goto _LL1436; _LL1436:
_temp1435=( int) _temp1431.varargs; goto _LL1434; _LL1434: _temp1433=( struct
Cyc_List_List*) _temp1431.attributes; goto _LL1428;} else{ goto _LL1429;}
_LL1429: goto _LL1430; _LL1428: if( _temp1443 != 0){ struct Cyc_List_List* inst=((
struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_Absyn_Tvar*), struct
Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_make_inst_var, _temp1443); struct
Cyc_List_List* ts=(( struct Cyc_List_List*(*)( void*(* f)( struct _tuple4*),
struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct _tuple4*)) Cyc_Core_snd,
inst); void* ftyp= Cyc_Tcutil_substitute( inst,( void*)({ struct Cyc_Absyn_FnType_struct*
_temp1445=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp1445[ 0]=({ struct Cyc_Absyn_FnType_struct _temp1446; _temp1446.tag= Cyc_Absyn_FnType_tag;
_temp1446.f1=({ struct Cyc_Absyn_FnInfo _temp1447; _temp1447.tvars= 0; _temp1447.effect=
_temp1441; _temp1447.ret_typ=( void*) _temp1439; _temp1447.args= _temp1437;
_temp1447.varargs= _temp1435; _temp1447.attributes= _temp1433; _temp1447;});
_temp1446;}); _temp1445;})); void* new_typ=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp1448=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1448[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1449; _temp1449.tag=
Cyc_Absyn_PointerType_tag; _temp1449.f1=({ struct Cyc_Absyn_PtrInfo _temp1450;
_temp1450.elt_typ=( void*) ftyp; _temp1450.rgn_typ=( void*) _temp1421; _temp1450.nullable=
_temp1419; _temp1450.tq= _temp1417; _temp1450.bounds= _temp1415; _temp1450;});
_temp1449;}); _temp1448;}); struct Cyc_Absyn_Exp* inner= Cyc_Absyn_copy_exp( e);(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Instantiate_e_struct*
_temp1451=( struct Cyc_Absyn_Instantiate_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Instantiate_e_struct)); _temp1451[ 0]=({ struct Cyc_Absyn_Instantiate_e_struct
_temp1452; _temp1452.tag= Cyc_Absyn_Instantiate_e_tag; _temp1452.f1= inner;
_temp1452.f2= ts; _temp1452;}); _temp1451;}))); e->topt=({ struct Cyc_Core_Opt*
_temp1453=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1453->v=( void*) new_typ; _temp1453;});} goto _LL1426; _LL1430: goto
_LL1426; _LL1426:;} goto _LL1408; _LL1412: goto _LL1408; _LL1408:;} goto _LL1397;
_LL1397:;} return( void*)({ struct Cyc_Core_Opt* _temp1454= e->topt; if(
_temp1454 == 0){ _throw( Null_Exception);} _temp1454->v;});} static void Cyc_Tcexp_tcExpNoInst(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){ struct Cyc_Position_Segment*
loc= e->loc; void* t;{ void* _temp1455=( void*) e->r; struct Cyc_Absyn_Exp*
_temp1527; struct _tuple1* _temp1529; struct Cyc_List_List* _temp1531; struct
Cyc_Absyn_Exp* _temp1533; struct Cyc_List_List* _temp1535; struct Cyc_Core_Opt*
_temp1537; void* _temp1539; void* _temp1541; struct _tuple1* _temp1543; struct
Cyc_List_List* _temp1545; void* _temp1547; void* _temp1549; struct Cyc_Absyn_Exp*
_temp1551; struct Cyc_Absyn_Exp* _temp1553; struct Cyc_Core_Opt* _temp1555;
struct Cyc_Absyn_Exp* _temp1557; struct Cyc_Absyn_Exp* _temp1559; struct Cyc_Absyn_Exp*
_temp1561; struct Cyc_Absyn_Exp* _temp1563; struct Cyc_Absyn_Exp* _temp1565;
struct Cyc_Absyn_Exp* _temp1567; struct Cyc_List_List* _temp1569; struct Cyc_Absyn_Exp*
_temp1571; struct Cyc_Absyn_Exp* _temp1573; struct Cyc_List_List* _temp1575;
struct Cyc_Absyn_Exp* _temp1577; struct Cyc_Absyn_Exp* _temp1579; void*
_temp1581; struct Cyc_Absyn_Exp* _temp1583; struct Cyc_Absyn_Exp* _temp1585;
struct Cyc_Absyn_Exp* _temp1587; void* _temp1589; struct Cyc_Absyn_Exp*
_temp1591; struct _tagged_string* _temp1593; struct Cyc_Absyn_Exp* _temp1595;
struct _tagged_string* _temp1597; struct Cyc_Absyn_Exp* _temp1599; struct Cyc_Absyn_Exp*
_temp1601; struct Cyc_Absyn_Exp* _temp1603; struct Cyc_List_List* _temp1605;
struct Cyc_List_List* _temp1607; struct _tuple2* _temp1609; struct Cyc_List_List*
_temp1611; struct Cyc_Absyn_Stmt* _temp1613; struct Cyc_Absyn_Fndecl* _temp1615;
struct Cyc_Absyn_Exp* _temp1617; struct Cyc_Absyn_Exp* _temp1619; struct Cyc_Absyn_Exp*
_temp1621; struct Cyc_Absyn_Vardecl* _temp1623; struct Cyc_Absyn_Structdecl*
_temp1625; struct Cyc_Absyn_Structdecl** _temp1627; struct Cyc_List_List*
_temp1628; struct Cyc_Core_Opt* _temp1630; struct Cyc_Core_Opt** _temp1632;
struct _tuple1* _temp1633; struct _tuple1** _temp1635; struct Cyc_Absyn_Tunionfield*
_temp1636; struct Cyc_Absyn_Tuniondecl* _temp1638; struct Cyc_List_List*
_temp1640; struct Cyc_Core_Opt* _temp1642; struct Cyc_Core_Opt** _temp1644;
struct Cyc_Core_Opt* _temp1645; struct Cyc_Core_Opt** _temp1647; struct Cyc_Absyn_Tunionfield*
_temp1648; struct Cyc_Absyn_XTuniondecl* _temp1650; struct Cyc_List_List*
_temp1652; struct Cyc_Core_Opt* _temp1654; struct Cyc_Core_Opt** _temp1656;
struct Cyc_Absyn_Enumfield* _temp1657; struct Cyc_Absyn_Enumdecl* _temp1659;
struct _tuple1* _temp1661; void* _temp1663; _LL1457: if((( struct _tunion_struct*)
_temp1455)->tag == Cyc_Absyn_NoInstantiate_e_tag){ _LL1528: _temp1527=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*) _temp1455)->f1; goto
_LL1458;} else{ goto _LL1459;} _LL1459: if((( struct _tunion_struct*) _temp1455)->tag
== Cyc_Absyn_UnknownId_e_tag){ _LL1530: _temp1529=( struct _tuple1*)(( struct
Cyc_Absyn_UnknownId_e_struct*) _temp1455)->f1; goto _LL1460;} else{ goto _LL1461;}
_LL1461: if((( struct _tunion_struct*) _temp1455)->tag == Cyc_Absyn_UnknownCall_e_tag){
_LL1534: _temp1533=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1455)->f1; goto _LL1532; _LL1532: _temp1531=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_e_struct*) _temp1455)->f2; goto _LL1462;} else{
goto _LL1463;} _LL1463: if((( struct _tunion_struct*) _temp1455)->tag == Cyc_Absyn_UnresolvedMem_e_tag){
_LL1538: _temp1537=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1455)->f1; goto _LL1536; _LL1536: _temp1535=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp1455)->f2; goto _LL1464;} else{
goto _LL1465;} _LL1465: if((( struct _tunion_struct*) _temp1455)->tag == Cyc_Absyn_Const_e_tag){
_LL1540: _temp1539=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1455)->f1;
goto _LL1466;} else{ goto _LL1467;} _LL1467: if((( struct _tunion_struct*)
_temp1455)->tag == Cyc_Absyn_Var_e_tag){ _LL1544: _temp1543=( struct _tuple1*)((
struct Cyc_Absyn_Var_e_struct*) _temp1455)->f1; goto _LL1542; _LL1542: _temp1541=(
void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1455)->f2; goto _LL1468;} else{
goto _LL1469;} _LL1469: if((( struct _tunion_struct*) _temp1455)->tag == Cyc_Absyn_Primop_e_tag){
_LL1548: _temp1547=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1455)->f1;
goto _LL1546; _LL1546: _temp1545=( struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp1455)->f2; goto _LL1470;} else{ goto _LL1471;} _LL1471: if((( struct
_tunion_struct*) _temp1455)->tag == Cyc_Absyn_Increment_e_tag){ _LL1552:
_temp1551=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1455)->f1; goto _LL1550; _LL1550: _temp1549=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1455)->f2; goto _LL1472;} else{ goto _LL1473;} _LL1473: if((( struct
_tunion_struct*) _temp1455)->tag == Cyc_Absyn_AssignOp_e_tag){ _LL1558:
_temp1557=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1455)->f1; goto _LL1556; _LL1556: _temp1555=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1455)->f2; goto _LL1554; _LL1554:
_temp1553=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1455)->f3; goto _LL1474;} else{ goto _LL1475;} _LL1475: if((( struct
_tunion_struct*) _temp1455)->tag == Cyc_Absyn_Conditional_e_tag){ _LL1564:
_temp1563=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1455)->f1; goto _LL1562; _LL1562: _temp1561=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp1455)->f2; goto _LL1560; _LL1560:
_temp1559=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1455)->f3; goto _LL1476;} else{ goto _LL1477;} _LL1477: if((( struct
_tunion_struct*) _temp1455)->tag == Cyc_Absyn_SeqExp_e_tag){ _LL1568: _temp1567=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*) _temp1455)->f1; goto
_LL1566; _LL1566: _temp1565=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1455)->f2; goto _LL1478;} else{ goto _LL1479;} _LL1479: if((( struct
_tunion_struct*) _temp1455)->tag == Cyc_Absyn_FnCall_e_tag){ _LL1572: _temp1571=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*) _temp1455)->f1; goto
_LL1570; _LL1570: _temp1569=( struct Cyc_List_List*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1455)->f2; goto _LL1480;} else{ goto _LL1481;} _LL1481: if((( struct
_tunion_struct*) _temp1455)->tag == Cyc_Absyn_Throw_e_tag){ _LL1574: _temp1573=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*) _temp1455)->f1; goto
_LL1482;} else{ goto _LL1483;} _LL1483: if((( struct _tunion_struct*) _temp1455)->tag
== Cyc_Absyn_Instantiate_e_tag){ _LL1578: _temp1577=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Instantiate_e_struct*) _temp1455)->f1; goto _LL1576; _LL1576:
_temp1575=( struct Cyc_List_List*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1455)->f2; goto _LL1484;} else{ goto _LL1485;} _LL1485: if((( struct
_tunion_struct*) _temp1455)->tag == Cyc_Absyn_Cast_e_tag){ _LL1582: _temp1581=(
void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp1455)->f1; goto _LL1580; _LL1580:
_temp1579=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*) _temp1455)->f2;
goto _LL1486;} else{ goto _LL1487;} _LL1487: if((( struct _tunion_struct*)
_temp1455)->tag == Cyc_Absyn_Address_e_tag){ _LL1584: _temp1583=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Address_e_struct*) _temp1455)->f1; goto _LL1488;} else{ goto
_LL1489;} _LL1489: if((( struct _tunion_struct*) _temp1455)->tag == Cyc_Absyn_New_e_tag){
_LL1586: _temp1585=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*)
_temp1455)->f1; goto _LL1490;} else{ goto _LL1491;} _LL1491: if((( struct
_tunion_struct*) _temp1455)->tag == Cyc_Absyn_Sizeofexp_e_tag){ _LL1588:
_temp1587=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1455)->f1; goto _LL1492;} else{ goto _LL1493;} _LL1493: if((( struct
_tunion_struct*) _temp1455)->tag == Cyc_Absyn_Sizeoftyp_e_tag){ _LL1590:
_temp1589=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp1455)->f1; goto
_LL1494;} else{ goto _LL1495;} _LL1495: if((( struct _tunion_struct*) _temp1455)->tag
== Cyc_Absyn_Deref_e_tag){ _LL1592: _temp1591=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Deref_e_struct*) _temp1455)->f1; goto _LL1496;} else{ goto _LL1497;}
_LL1497: if((( struct _tunion_struct*) _temp1455)->tag == Cyc_Absyn_StructMember_e_tag){
_LL1596: _temp1595=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1455)->f1; goto _LL1594; _LL1594: _temp1593=( struct _tagged_string*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp1455)->f2; goto _LL1498;} else{
goto _LL1499;} _LL1499: if((( struct _tunion_struct*) _temp1455)->tag == Cyc_Absyn_StructArrow_e_tag){
_LL1600: _temp1599=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1455)->f1; goto _LL1598; _LL1598: _temp1597=( struct _tagged_string*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp1455)->f2; goto _LL1500;} else{
goto _LL1501;} _LL1501: if((( struct _tunion_struct*) _temp1455)->tag == Cyc_Absyn_Subscript_e_tag){
_LL1604: _temp1603=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1455)->f1; goto _LL1602; _LL1602: _temp1601=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp1455)->f2; goto _LL1502;} else{ goto
_LL1503;} _LL1503: if((( struct _tunion_struct*) _temp1455)->tag == Cyc_Absyn_Tuple_e_tag){
_LL1606: _temp1605=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1455)->f1; goto _LL1504;} else{ goto _LL1505;} _LL1505: if((( struct
_tunion_struct*) _temp1455)->tag == Cyc_Absyn_CompoundLit_e_tag){ _LL1610:
_temp1609=( struct _tuple2*)(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp1455)->f1;
goto _LL1608; _LL1608: _temp1607=( struct Cyc_List_List*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1455)->f2; goto _LL1506;} else{ goto _LL1507;} _LL1507: if((( struct
_tunion_struct*) _temp1455)->tag == Cyc_Absyn_Array_e_tag){ _LL1612: _temp1611=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1455)->f1; goto
_LL1508;} else{ goto _LL1509;} _LL1509: if((( struct _tunion_struct*) _temp1455)->tag
== Cyc_Absyn_StmtExp_e_tag){ _LL1614: _temp1613=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_StmtExp_e_struct*) _temp1455)->f1; goto _LL1510;} else{ goto
_LL1511;} _LL1511: if((( struct _tunion_struct*) _temp1455)->tag == Cyc_Absyn_Codegen_e_tag){
_LL1616: _temp1615=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1455)->f1; goto _LL1512;} else{ goto _LL1513;} _LL1513: if((( struct
_tunion_struct*) _temp1455)->tag == Cyc_Absyn_Fill_e_tag){ _LL1618: _temp1617=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*) _temp1455)->f1; goto
_LL1514;} else{ goto _LL1515;} _LL1515: if((( struct _tunion_struct*) _temp1455)->tag
== Cyc_Absyn_Comprehension_e_tag){ _LL1624: _temp1623=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1455)->f1; goto _LL1622; _LL1622:
_temp1621=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1455)->f2; goto _LL1620; _LL1620: _temp1619=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1455)->f3; goto _LL1516;} else{
goto _LL1517;} _LL1517: if((( struct _tunion_struct*) _temp1455)->tag == Cyc_Absyn_Struct_e_tag){
_LL1634: _temp1633=( struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1455)->f1; _temp1635=&(( struct Cyc_Absyn_Struct_e_struct*) _temp1455)->f1;
goto _LL1631; _LL1631: _temp1630=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1455)->f2; _temp1632=&(( struct Cyc_Absyn_Struct_e_struct*) _temp1455)->f2;
goto _LL1629; _LL1629: _temp1628=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1455)->f3; goto _LL1626; _LL1626: _temp1625=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1455)->f4; _temp1627=&(( struct Cyc_Absyn_Struct_e_struct*)
_temp1455)->f4; goto _LL1518;} else{ goto _LL1519;} _LL1519: if((( struct
_tunion_struct*) _temp1455)->tag == Cyc_Absyn_Tunion_e_tag){ _LL1646: _temp1645=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp1455)->f1;
_temp1647=&(( struct Cyc_Absyn_Tunion_e_struct*) _temp1455)->f1; goto _LL1643;
_LL1643: _temp1642=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1455)->f2; _temp1644=&(( struct Cyc_Absyn_Tunion_e_struct*) _temp1455)->f2;
goto _LL1641; _LL1641: _temp1640=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1455)->f3; goto _LL1639; _LL1639: _temp1638=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1455)->f4; goto _LL1637; _LL1637:
_temp1636=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1455)->f5; goto _LL1520;} else{ goto _LL1521;} _LL1521: if((( struct
_tunion_struct*) _temp1455)->tag == Cyc_Absyn_XTunion_e_tag){ _LL1655: _temp1654=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_XTunion_e_struct*) _temp1455)->f1;
_temp1656=&(( struct Cyc_Absyn_XTunion_e_struct*) _temp1455)->f1; goto _LL1653;
_LL1653: _temp1652=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_e_struct*)
_temp1455)->f2; goto _LL1651; _LL1651: _temp1650=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_e_struct*) _temp1455)->f3; goto _LL1649; _LL1649:
_temp1648=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_e_struct*)
_temp1455)->f4; goto _LL1522;} else{ goto _LL1523;} _LL1523: if((( struct
_tunion_struct*) _temp1455)->tag == Cyc_Absyn_Enum_e_tag){ _LL1662: _temp1661=(
struct _tuple1*)(( struct Cyc_Absyn_Enum_e_struct*) _temp1455)->f1; goto _LL1660;
_LL1660: _temp1659=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp1455)->f2; goto _LL1658; _LL1658: _temp1657=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_e_struct*) _temp1455)->f3; goto _LL1524;} else{ goto
_LL1525;} _LL1525: if((( struct _tunion_struct*) _temp1455)->tag == Cyc_Absyn_Malloc_e_tag){
_LL1664: _temp1663=( void*)(( struct Cyc_Absyn_Malloc_e_struct*) _temp1455)->f1;
goto _LL1526;} else{ goto _LL1456;} _LL1458: Cyc_Tcexp_tcExpNoInst( te, 0,
_temp1527); return; _LL1460: Cyc_Tcexp_resolve_unknown_id( te, e, _temp1529);
Cyc_Tcexp_tcExpNoInst( te, topt, e); return; _LL1462: Cyc_Tcexp_resolve_unknown_fn(
te, e, _temp1533, _temp1531); Cyc_Tcexp_tcExpNoInst( te, topt, e); return;
_LL1464: Cyc_Tcexp_resolve_unresolved_mem( te, loc, topt, e, _temp1535); Cyc_Tcexp_tcExpNoInst(
te, topt, e); return; _LL1466: t= Cyc_Tcexp_tcConst( te, loc, topt, _temp1539, e);
goto _LL1456; _LL1468: t= Cyc_Tcexp_tcVar( te, loc, _temp1543, _temp1541); goto
_LL1456; _LL1470: t= Cyc_Tcexp_tcPrimop( te, loc, topt, _temp1547, _temp1545);
goto _LL1456; _LL1472: t= Cyc_Tcexp_tcIncrement( te, loc, topt, _temp1551,
_temp1549); goto _LL1456; _LL1474: t= Cyc_Tcexp_tcAssignOp( te, loc, topt,
_temp1557, _temp1555, _temp1553); goto _LL1456; _LL1476: t= Cyc_Tcexp_tcConditional(
te, loc, topt, _temp1563, _temp1561, _temp1559); goto _LL1456; _LL1478: t= Cyc_Tcexp_tcSeqExp(
te, loc, topt, _temp1567, _temp1565); goto _LL1456; _LL1480: t= Cyc_Tcexp_tcFnCall(
te, loc, topt, _temp1571, _temp1569); goto _LL1456; _LL1482: t= Cyc_Tcexp_tcThrow(
te, loc, topt, _temp1573); goto _LL1456; _LL1484: t= Cyc_Tcexp_tcInstantiate( te,
loc, topt, _temp1577, _temp1575); goto _LL1456; _LL1486: t= Cyc_Tcexp_tcCast( te,
loc, topt, _temp1581, _temp1579); goto _LL1456; _LL1488: t= Cyc_Tcexp_tcAddress(
te, loc, topt, _temp1583); goto _LL1456; _LL1490: t= Cyc_Tcexp_tcNew( te, loc,
topt, e, _temp1585); goto _LL1456; _LL1492: { void* t1= Cyc_Tcexp_tcExpNoPromote(
te, 0, _temp1587); t= Cyc_Tcexp_tcSizeof( te, loc, topt, t1); goto _LL1456;}
_LL1494: t= Cyc_Tcexp_tcSizeof( te, loc, topt, _temp1589); goto _LL1456; _LL1496:
t= Cyc_Tcexp_tcDeref( te, loc, topt, _temp1591); goto _LL1456; _LL1498: t= Cyc_Tcexp_tcStructMember(
te, loc, topt, e, _temp1595, _temp1593); goto _LL1456; _LL1500: t= Cyc_Tcexp_tcStructArrow(
te, loc, topt, _temp1599, _temp1597); goto _LL1456; _LL1502: t= Cyc_Tcexp_tcSubscript(
te, loc, topt, _temp1603, _temp1601); goto _LL1456; _LL1504: t= Cyc_Tcexp_tcTuple(
te, loc, topt, _temp1605); goto _LL1456; _LL1506: t= Cyc_Tcexp_tcCompoundLit( te,
loc, topt, _temp1609, _temp1607); goto _LL1456; _LL1508: { void** elt_topt= 0;
if( topt != 0){ void* _temp1666= Cyc_Tcutil_compress(({ void** _temp1665= topt;
if( _temp1665 == 0){ _throw( Null_Exception);}* _temp1665;})); struct Cyc_Absyn_Exp*
_temp1672; struct Cyc_Absyn_Tqual* _temp1674; void* _temp1676; void** _temp1678;
_LL1668: if(( unsigned int) _temp1666 > 4u?(( struct _tunion_struct*) _temp1666)->tag
== Cyc_Absyn_ArrayType_tag: 0){ _LL1677: _temp1676=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1666)->f1; _temp1678=&(( struct Cyc_Absyn_ArrayType_struct*) _temp1666)->f1;
goto _LL1675; _LL1675: _temp1674=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1666)->f2; goto _LL1673; _LL1673: _temp1672=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1666)->f3; goto _LL1669;} else{ goto
_LL1670;} _LL1670: goto _LL1671; _LL1669: elt_topt=( void**) _temp1678; goto
_LL1667; _LL1671: goto _LL1667; _LL1667:;} t= Cyc_Tcexp_tcArray( te, loc,
elt_topt, _temp1611); goto _LL1456;} _LL1510: t= Cyc_Tcexp_tcStmtExp( te, loc,
topt, _temp1613); goto _LL1456; _LL1512: t= Cyc_Tcexp_tcCodegen( te, loc, topt,
_temp1615); goto _LL1456; _LL1514: t= Cyc_Tcexp_tcFill( te, loc, topt, _temp1617);
goto _LL1456; _LL1516: t= Cyc_Tcexp_tcComprehension( te, loc, topt, _temp1623,
_temp1621, _temp1619); goto _LL1456; _LL1518: t= Cyc_Tcexp_tcStruct( te, loc,
topt, _temp1635, _temp1632, _temp1628, _temp1627); goto _LL1456; _LL1520: t= Cyc_Tcexp_tcTunion(
te, loc, topt, e, _temp1647, _temp1644, _temp1640, _temp1638, _temp1636); goto
_LL1456; _LL1522: t= Cyc_Tcexp_tcXTunion( te, loc, topt, _temp1656, _temp1652,
_temp1650, _temp1648); goto _LL1456; _LL1524: t=( void*)({ struct Cyc_Absyn_EnumType_struct*
_temp1679=( struct Cyc_Absyn_EnumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp1679[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp1680; _temp1680.tag= Cyc_Absyn_EnumType_tag;
_temp1680.f1=({ struct Cyc_Absyn_Enumdecl* _temp1681= _temp1659; if( _temp1681
== 0){ _throw( Null_Exception);} _temp1681->name;}); _temp1680.f2= _temp1659;
_temp1680;}); _temp1679;}); goto _LL1456; _LL1526: t= Cyc_Tcexp_tcMalloc( te,
loc, topt, _temp1663); goto _LL1456; _LL1456:;} e->topt=({ struct Cyc_Core_Opt*
_temp1682=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1682->v=( void*) t; _temp1682;});}