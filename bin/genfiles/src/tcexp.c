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
return t;}} static void* Cyc_Tcexp_tcIncrement( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e, void* i){
Cyc_Tcexp_tcExpNoPromote( te, 0, e); if( ! Cyc_Absyn_is_lvalue( e)){ return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_string)({ char* _temp550=( char*)"increment/decrement of non-lvalue";
struct _tagged_string _temp551; _temp551.curr= _temp550; _temp551.base= _temp550;
_temp551.last_plus_one= _temp550 + 34; _temp551;}));}{ void* t=( void*)({ struct
Cyc_Core_Opt* _temp552= e->topt; if( _temp552 == 0){ _throw( Null_Exception);}
_temp552->v;}); if( ! Cyc_Tcutil_is_numeric( e)? ! Cyc_Tcutil_is_tagged_pointer_typ(
t): 0){ Cyc_Tcutil_terr( e->loc,({ struct _tagged_string _temp553= Cyc_Absynpp_typ2string(
t); xprintf("expecting arithmetic or pointer type but found %.*s", _temp553.last_plus_one
- _temp553.curr, _temp553.curr);}));} return t;}} static void* Cyc_Tcexp_tcConditional(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3){ Cyc_Tcexp_tcTest(
te, e1,( struct _tagged_string)({ char* _temp554=( char*)"conditional expression";
struct _tagged_string _temp555; _temp555.curr= _temp554; _temp555.base= _temp554;
_temp555.last_plus_one= _temp554 + 23; _temp555;})); Cyc_Tcexp_tcExp( te, topt,
e2); Cyc_Tcexp_tcExp( te, topt, e3);{ void* t= Cyc_Absyn_new_evar(( void*) Cyc_Absyn_MemKind);
struct Cyc_List_List* l1=({ struct Cyc_List_List* _temp556=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp556->hd=( void*) e3; _temp556->tl=
0; _temp556;}); struct Cyc_List_List* l2=({ struct Cyc_List_List* _temp557=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp557->hd=(
void*) e2; _temp557->tl= l1; _temp557;}); if( ! Cyc_Tcutil_coerce_list( te, t,
l2)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp560= Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Core_Opt* _temp558= e2->topt; if( _temp558 == 0){ _throw(
Null_Exception);} _temp558->v;})); struct _tagged_string _temp561= Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Core_Opt* _temp559= e3->topt; if( _temp559 == 0){ _throw(
Null_Exception);} _temp559->v;})); xprintf("conditional clause types do not match: %.*s != %.*s",
_temp560.last_plus_one - _temp560.curr, _temp560.curr, _temp561.last_plus_one -
_temp561.curr, _temp561.curr);}));} return t;}} static void* Cyc_Tcexp_tcAssignOp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e1, struct Cyc_Core_Opt* po, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExpNoPromote(
te, 0, e1); Cyc_Tcexp_tcExp( te,( void**)(( void*)({ struct Cyc_Core_Opt*
_temp562= e1->topt; if( _temp562 == 0){ _throw( Null_Exception);}& _temp562->v;})),
e2);{ void* t1=( void*)({ struct Cyc_Core_Opt* _temp563= e1->topt; if( _temp563
== 0){ _throw( Null_Exception);} _temp563->v;}); void* t2=( void*)({ struct Cyc_Core_Opt*
_temp564= e2->topt; if( _temp564 == 0){ _throw( Null_Exception);} _temp564->v;});{
void* _temp565= Cyc_Tcutil_compress( t1); struct Cyc_Absyn_Exp* _temp571; struct
Cyc_Absyn_Tqual* _temp573; void* _temp575; _LL567: if(( unsigned int) _temp565 >
4u?(( struct _tunion_struct*) _temp565)->tag == Cyc_Absyn_ArrayType_tag: 0){
_LL576: _temp575=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp565)->f1;
goto _LL574; _LL574: _temp573=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp565)->f2; goto _LL572; _LL572: _temp571=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp565)->f3; goto _LL568;} else{ goto _LL569;}
_LL569: goto _LL570; _LL568: Cyc_Tcutil_terr( loc,( struct _tagged_string)({
char* _temp577=( char*)"cannot assign to an array"; struct _tagged_string
_temp578; _temp578.curr= _temp577; _temp578.base= _temp577; _temp578.last_plus_one=
_temp577 + 26; _temp578;})); goto _LL566; _LL570: goto _LL566; _LL566:;} if( !
Cyc_Absyn_is_lvalue( e1)){ return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_string)({ char* _temp579=( char*)"assignment to non-lvalue"; struct
_tagged_string _temp580; _temp580.curr= _temp579; _temp580.base= _temp579;
_temp580.last_plus_one= _temp579 + 25; _temp580;}));} if( po == 0){ if( ! Cyc_Tcutil_coerce_assign(
te, e2, t1)){ return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string
_temp581= Cyc_Absynpp_typ2string( t1); struct _tagged_string _temp582= Cyc_Absynpp_typ2string(
t2); xprintf("type mismatch: %.*s != %.*s", _temp581.last_plus_one - _temp581.curr,
_temp581.curr, _temp582.last_plus_one - _temp582.curr, _temp582.curr);}));}}
else{ void* p=( void*)({ struct Cyc_Core_Opt* _temp583= po; if( _temp583 == 0){
_throw( Null_Exception);} _temp583->v;}); void* t_result= Cyc_Tcexp_tcBinPrimop(
te, loc, 0, p, e1, e2); if( !( Cyc_Tcutil_unify( t_result, t1)? 1: Cyc_Tcutil_coerceable(
t_result))){ return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char*
_temp584=( char*)"Cannot use this operator in an assignment"; struct
_tagged_string _temp585; _temp585.curr= _temp584; _temp585.base= _temp584;
_temp585.last_plus_one= _temp584 + 42; _temp585;}));}} return t2;}} static void*
Cyc_Tcexp_tcSeqExp( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp(
te, 0, e1); Cyc_Tcexp_tcExp( te, topt, e2); return( void*)({ struct Cyc_Core_Opt*
_temp586= e2->topt; if( _temp586 == 0){ _throw( Null_Exception);} _temp586->v;});}
static void* Cyc_Tcexp_tcFnCall( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_List_List* es){ Cyc_Tcexp_tcExp(
te, 0, e);{ void* t= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp587= e->topt; if( _temp587 == 0){ _throw( Null_Exception);} _temp587->v;}));
void* _temp588= t; struct Cyc_Absyn_PtrInfo _temp594; struct Cyc_Absyn_Conref*
_temp596; struct Cyc_Absyn_Tqual* _temp598; struct Cyc_Absyn_Conref* _temp600;
void* _temp602; void* _temp604; _LL590: if(( unsigned int) _temp588 > 4u?((
struct _tunion_struct*) _temp588)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL595:
_temp594=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp588)->f1; _LL605: _temp604=( void*) _temp594.elt_typ; goto _LL603; _LL603:
_temp602=( void*) _temp594.rgn_typ; goto _LL601; _LL601: _temp600=( struct Cyc_Absyn_Conref*)
_temp594.nullable; goto _LL599; _LL599: _temp598=( struct Cyc_Absyn_Tqual*)
_temp594.tq; goto _LL597; _LL597: _temp596=( struct Cyc_Absyn_Conref*) _temp594.bounds;
goto _LL591;} else{ goto _LL592;} _LL592: goto _LL593; _LL591: Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp602); Cyc_Tcutil_check_nonzero_bound( loc, _temp596);{ void*
_temp606= Cyc_Tcutil_compress( _temp604); struct Cyc_Absyn_FnInfo _temp612;
struct Cyc_List_List* _temp614; int _temp616; struct Cyc_List_List* _temp618;
void* _temp620; struct Cyc_Core_Opt* _temp622; struct Cyc_List_List* _temp624;
_LL608: if(( unsigned int) _temp606 > 4u?(( struct _tunion_struct*) _temp606)->tag
== Cyc_Absyn_FnType_tag: 0){ _LL613: _temp612=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp606)->f1; _LL625: _temp624=( struct Cyc_List_List*)
_temp612.tvars; goto _LL623; _LL623: _temp622=( struct Cyc_Core_Opt*) _temp612.effect;
goto _LL621; _LL621: _temp620=( void*) _temp612.ret_typ; goto _LL619; _LL619:
_temp618=( struct Cyc_List_List*) _temp612.args; goto _LL617; _LL617: _temp616=(
int) _temp612.varargs; goto _LL615; _LL615: _temp614=( struct Cyc_List_List*)
_temp612.attributes; goto _LL609;} else{ goto _LL610;} _LL610: goto _LL611;
_LL609: if( _temp616){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp626=( char*)"vararg functions (...) not supported yet"; struct
_tagged_string _temp627; _temp627.curr= _temp626; _temp627.base= _temp626;
_temp627.last_plus_one= _temp626 + 41; _temp627;}));} if( topt != 0){ Cyc_Tcutil_unify(
_temp620,({ void** _temp628= topt; if( _temp628 == 0){ _throw( Null_Exception);}*
_temp628;}));} while( es != 0? _temp618 != 0: 0) { struct Cyc_Absyn_Exp* e1=(
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp629= es; if( _temp629 == 0){
_throw( Null_Exception);} _temp629->hd;}); void* t2=(*(( struct _tuple2*)({
struct Cyc_List_List* _temp630= _temp618; if( _temp630 == 0){ _throw(
Null_Exception);} _temp630->hd;}))).f3; Cyc_Tcexp_tcExp( te,( void**)& t2, e1);
if( ! Cyc_Tcutil_coerce_arg( te, e1, t2)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp632= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp631= e1->topt; if( _temp631 == 0){ _throw( Null_Exception);} _temp631->v;}));
struct _tagged_string _temp633= Cyc_Absynpp_typ2string( t2); xprintf("actual argument has type %.*s but formal has type %.*s",
_temp632.last_plus_one - _temp632.curr, _temp632.curr, _temp633.last_plus_one -
_temp633.curr, _temp633.curr);}));} es=({ struct Cyc_List_List* _temp634= es;
if( _temp634 == 0){ _throw( Null_Exception);} _temp634->tl;}); _temp618=({
struct Cyc_List_List* _temp635= _temp618; if( _temp635 == 0){ _throw(
Null_Exception);} _temp635->tl;});} if( es != 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp636=( char*)"too many arguments for function";
struct _tagged_string _temp637; _temp637.curr= _temp636; _temp637.base= _temp636;
_temp637.last_plus_one= _temp636 + 32; _temp637;}));} if( _temp618 != 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp638=( char*)"too few arguments for function";
struct _tagged_string _temp639; _temp639.curr= _temp638; _temp639.base= _temp638;
_temp639.last_plus_one= _temp638 + 31; _temp639;}));} Cyc_Tcenv_check_effect_accessible(
te, loc,( void*)({ struct Cyc_Core_Opt* _temp640= _temp622; if( _temp640 == 0){
_throw( Null_Exception);} _temp640->v;})); return _temp620; _LL611: return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_string)({ char* _temp641=( char*)"expected pointer to function";
struct _tagged_string _temp642; _temp642.curr= _temp641; _temp642.base= _temp641;
_temp642.last_plus_one= _temp641 + 29; _temp642;})); _LL607:;} _LL593: return
Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char* _temp643=( char*)"expected pointer to function";
struct _tagged_string _temp644; _temp644.curr= _temp643; _temp644.base= _temp643;
_temp644.last_plus_one= _temp643 + 29; _temp644;})); _LL589:;}} static void* Cyc_Tcexp_tcThrow(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp( te,( void**)& Cyc_Absyn_exn_typ, e); if( !
Cyc_Tcutil_coerce_use( te, e, Cyc_Absyn_exn_typ)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp646= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp645= e->topt; if( _temp645 == 0){ _throw( Null_Exception);} _temp645->v;}));
xprintf("expected xtunion exn but found %.*s", _temp646.last_plus_one - _temp646.curr,
_temp646.curr);}));} return Cyc_Absyn_wildtyp();} static void* Cyc_Tcexp_tcInstantiate(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e, struct Cyc_List_List* ts){ Cyc_Tcexp_tcExpNoInst( te, 0, e);(
void*)({ struct Cyc_Core_Opt* _temp647= e->topt; if( _temp647 == 0){ _throw(
Null_Exception);} _temp647->v=( void*) Cyc_Absyn_pointer_expand( Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp648= e->topt; if( _temp648 == 0){ _throw(
Null_Exception);} _temp648->v;})));});{ void* t1= Cyc_Tcutil_compress(( void*)({
struct Cyc_Core_Opt* _temp649= e->topt; if( _temp649 == 0){ _throw(
Null_Exception);} _temp649->v;}));{ void* _temp650= t1; struct Cyc_Absyn_PtrInfo
_temp656; struct Cyc_Absyn_Conref* _temp658; struct Cyc_Absyn_Tqual* _temp660;
struct Cyc_Absyn_Conref* _temp662; void* _temp664; void* _temp666; _LL652: if((
unsigned int) _temp650 > 4u?(( struct _tunion_struct*) _temp650)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL657: _temp656=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp650)->f1; _LL667: _temp666=( void*) _temp656.elt_typ; goto _LL665; _LL665:
_temp664=( void*) _temp656.rgn_typ; goto _LL663; _LL663: _temp662=( struct Cyc_Absyn_Conref*)
_temp656.nullable; goto _LL661; _LL661: _temp660=( struct Cyc_Absyn_Tqual*)
_temp656.tq; goto _LL659; _LL659: _temp658=( struct Cyc_Absyn_Conref*) _temp656.bounds;
goto _LL653;} else{ goto _LL654;} _LL654: goto _LL655; _LL653:{ void* _temp668=
Cyc_Tcutil_compress( _temp666); struct Cyc_Absyn_FnInfo _temp674; struct Cyc_List_List*
_temp676; int _temp678; struct Cyc_List_List* _temp680; void* _temp682; struct
Cyc_Core_Opt* _temp684; struct Cyc_List_List* _temp686; _LL670: if((
unsigned int) _temp668 > 4u?(( struct _tunion_struct*) _temp668)->tag == Cyc_Absyn_FnType_tag:
0){ _LL675: _temp674=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp668)->f1; _LL687: _temp686=( struct Cyc_List_List*) _temp674.tvars; goto
_LL685; _LL685: _temp684=( struct Cyc_Core_Opt*) _temp674.effect; goto _LL683;
_LL683: _temp682=( void*) _temp674.ret_typ; goto _LL681; _LL681: _temp680=(
struct Cyc_List_List*) _temp674.args; goto _LL679; _LL679: _temp678=( int)
_temp674.varargs; goto _LL677; _LL677: _temp676=( struct Cyc_List_List*)
_temp674.attributes; goto _LL671;} else{ goto _LL672;} _LL672: goto _LL673;
_LL671: { struct Cyc_List_List* instantiation= 0; if((( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( ts) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp686)){ return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char*
_temp688=( char*)"type instantiation mismatch"; struct _tagged_string _temp689;
_temp689.curr= _temp688; _temp689.base= _temp688; _temp689.last_plus_one=
_temp688 + 28; _temp689;}));} for( 0; ts != 0; ts=({ struct Cyc_List_List*
_temp690= ts; if( _temp690 == 0){ _throw( Null_Exception);} _temp690->tl;}),
_temp686=({ struct Cyc_List_List* _temp691= _temp686; if( _temp691 == 0){ _throw(
Null_Exception);} _temp691->tl;})){ void* k=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp692= _temp686; if( _temp692 == 0){ _throw( Null_Exception);} _temp692->hd;}))->kind);
Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars( te), k,( void*)({
struct Cyc_List_List* _temp693= ts; if( _temp693 == 0){ _throw( Null_Exception);}
_temp693->hd;})); instantiation=({ struct Cyc_List_List* _temp694=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp694->hd=( void*)({ struct
_tuple4* _temp695=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp695->f1=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp697=
_temp686; if( _temp697 == 0){ _throw( Null_Exception);} _temp697->hd;});
_temp695->f2=( void*)({ struct Cyc_List_List* _temp696= ts; if( _temp696 == 0){
_throw( Null_Exception);} _temp696->hd;}); _temp695;}); _temp694->tl=
instantiation; _temp694;});}{ void* new_fn_typ= Cyc_Tcutil_substitute(
instantiation,( void*)({ struct Cyc_Absyn_FnType_struct* _temp698=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp698[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp699; _temp699.tag= Cyc_Absyn_FnType_tag; _temp699.f1=({ struct Cyc_Absyn_FnInfo
_temp700; _temp700.tvars= 0; _temp700.effect= _temp684; _temp700.ret_typ=( void*)
_temp682; _temp700.args= _temp680; _temp700.varargs= _temp678; _temp700.attributes=
_temp676; _temp700;}); _temp699;}); _temp698;})); void* new_typ=( void*)({
struct Cyc_Absyn_PointerType_struct* _temp701=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp701[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp702; _temp702.tag= Cyc_Absyn_PointerType_tag;
_temp702.f1=({ struct Cyc_Absyn_PtrInfo _temp703; _temp703.elt_typ=( void*)
new_fn_typ; _temp703.rgn_typ=( void*) _temp664; _temp703.nullable= _temp662;
_temp703.tq= _temp660; _temp703.bounds= _temp658; _temp703;}); _temp702;});
_temp701;}); return new_typ;}} _LL673: goto _LL669; _LL669:;} goto _LL651;
_LL655: goto _LL651; _LL651:;} return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp704= Cyc_Absynpp_typ2string( t1); xprintf("expecting polymorphic type but found %.*s",
_temp704.last_plus_one - _temp704.curr, _temp704.curr);}));}} static void* Cyc_Tcexp_tcCast(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, void*
t, struct Cyc_Absyn_Exp* e){ Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars(
te),( void*) Cyc_Absyn_MemKind, t); Cyc_Tcexp_tcExp( te, 0, e);{ void* t2=( void*)({
struct Cyc_Core_Opt* _temp705= e->topt; if( _temp705 == 0){ _throw(
Null_Exception);} _temp705->v;}); if( ! Cyc_Tcutil_silent_castable( te, loc, t2,
t)? ! Cyc_Tcutil_castable( te, loc, t2, t): 0){ return Cyc_Tcexp_expr_err( te,
loc,({ struct _tagged_string _temp706= Cyc_Absynpp_typ2string( t2); struct
_tagged_string _temp707= Cyc_Absynpp_typ2string( t); xprintf("cannot cast %.*s to %.*s",
_temp706.last_plus_one - _temp706.curr, _temp706.curr, _temp707.last_plus_one -
_temp707.curr, _temp707.curr);}));} return t;}} static void* Cyc_Tcexp_tcAddress(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e){ void** topt2= 0; struct Cyc_Absyn_Tqual* tq2= Cyc_Absyn_empty_tqual();
if( topt != 0){ void* _temp709= Cyc_Tcutil_compress(({ void** _temp708= topt;
if( _temp708 == 0){ _throw( Null_Exception);}* _temp708;})); struct Cyc_Absyn_PtrInfo
_temp715; struct Cyc_Absyn_Conref* _temp717; struct Cyc_Absyn_Tqual* _temp719;
struct Cyc_Absyn_Conref* _temp721; void* _temp723; void* _temp725; _LL711: if((
unsigned int) _temp709 > 4u?(( struct _tunion_struct*) _temp709)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL716: _temp715=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp709)->f1; _LL726: _temp725=( void*) _temp715.elt_typ; goto _LL724; _LL724:
_temp723=( void*) _temp715.rgn_typ; goto _LL722; _LL722: _temp721=( struct Cyc_Absyn_Conref*)
_temp715.nullable; goto _LL720; _LL720: _temp719=( struct Cyc_Absyn_Tqual*)
_temp715.tq; goto _LL718; _LL718: _temp717=( struct Cyc_Absyn_Conref*) _temp715.bounds;
goto _LL712;} else{ goto _LL713;} _LL713: goto _LL714; _LL712: topt2=({ void**
_temp727=( void**) GC_malloc( sizeof( void*)); _temp727[ 0]= _temp725; _temp727;});
tq2= _temp719; goto _LL710; _LL714: goto _LL710; _LL710:;} Cyc_Tcexp_tcExpNoInst(
te, topt2, e);{ void* _temp728=( void*) e->r; struct Cyc_Absyn_Structdecl*
_temp736; struct Cyc_List_List* _temp738; struct Cyc_Core_Opt* _temp740; struct
_tuple1* _temp742; struct Cyc_List_List* _temp744; _LL730: if((( struct
_tunion_struct*) _temp728)->tag == Cyc_Absyn_Struct_e_tag){ _LL743: _temp742=(
struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*) _temp728)->f1; goto _LL741;
_LL741: _temp740=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp728)->f2; goto _LL739; _LL739: _temp738=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp728)->f3; goto _LL737; _LL737: _temp736=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*) _temp728)->f4;
goto _LL731;} else{ goto _LL732;} _LL732: if((( struct _tunion_struct*) _temp728)->tag
== Cyc_Absyn_Tuple_e_tag){ _LL745: _temp744=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Tuple_e_struct*) _temp728)->f1; goto _LL733;} else{ goto _LL734;}
_LL734: goto _LL735; _LL731: goto _LL733; _LL733: Cyc_Tcutil_warn( loc,( struct
_tagged_string)({ char* _temp746=( char*)"& used to allocate"; struct
_tagged_string _temp747; _temp747.curr= _temp746; _temp747.base= _temp746;
_temp747.last_plus_one= _temp746 + 19; _temp747;}));{ void*(* ptr_maker)( void*
t, void* rgn, struct Cyc_Absyn_Tqual* tq)= Cyc_Absyn_at_typ; if( topt != 0){
void* _temp749= Cyc_Tcutil_compress(({ void** _temp748= topt; if( _temp748 == 0){
_throw( Null_Exception);}* _temp748;})); struct Cyc_Absyn_PtrInfo _temp755;
struct Cyc_Absyn_Conref* _temp757; struct Cyc_Absyn_Tqual* _temp759; struct Cyc_Absyn_Conref*
_temp761; void* _temp763; void* _temp765; _LL751: if(( unsigned int) _temp749 >
4u?(( struct _tunion_struct*) _temp749)->tag == Cyc_Absyn_PointerType_tag: 0){
_LL756: _temp755=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp749)->f1; _LL766: _temp765=( void*) _temp755.elt_typ; goto _LL764; _LL764:
_temp763=( void*) _temp755.rgn_typ; goto _LL762; _LL762: _temp761=( struct Cyc_Absyn_Conref*)
_temp755.nullable; goto _LL760; _LL760: _temp759=( struct Cyc_Absyn_Tqual*)
_temp755.tq; goto _LL758; _LL758: _temp757=( struct Cyc_Absyn_Conref*) _temp755.bounds;
goto _LL752;} else{ goto _LL753;} _LL753: goto _LL754; _LL752: if((( int(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp761)){ ptr_maker= Cyc_Absyn_star_typ;}
goto _LL750; _LL754: goto _LL750; _LL750:;} return ptr_maker(( void*)({ struct
Cyc_Core_Opt* _temp767= e->topt; if( _temp767 == 0){ _throw( Null_Exception);}
_temp767->v;}),( void*) Cyc_Absyn_HeapRgn, tq2);} _LL735: goto _LL729; _LL729:;}{
void* _temp770; int _temp772; struct _tuple5 _temp768= Cyc_Tcutil_addressof_props(
te, e); _LL773: _temp772= _temp768.f1; goto _LL771; _LL771: _temp770= _temp768.f2;
goto _LL769; _LL769: { struct Cyc_Absyn_Tqual* tq= Cyc_Absyn_empty_tqual(); if(
_temp772){ tq->q_const= 1;}{ void* t= Cyc_Absyn_at_typ(( void*)({ struct Cyc_Core_Opt*
_temp774= e->topt; if( _temp774 == 0){ _throw( Null_Exception);} _temp774->v;}),
_temp770, tq); return t;}}}} static void* Cyc_Tcexp_tcSizeof( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, void* t){ Cyc_Tcutil_check_type(
loc, te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind, t); Cyc_Evexp_szof(
t, loc); return Cyc_Absyn_uint_t;} static void* Cyc_Tcexp_tcDeref( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp(
te, 0, e);{ void* t= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp775= e->topt; if( _temp775 == 0){ _throw( Null_Exception);} _temp775->v;}));
void* _temp776= t; struct Cyc_Absyn_PtrInfo _temp782; struct Cyc_Absyn_Conref*
_temp784; struct Cyc_Absyn_Tqual* _temp786; struct Cyc_Absyn_Conref* _temp788;
void* _temp790; void* _temp792; _LL778: if(( unsigned int) _temp776 > 4u?((
struct _tunion_struct*) _temp776)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL783:
_temp782=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp776)->f1; _LL793: _temp792=( void*) _temp782.elt_typ; goto _LL791; _LL791:
_temp790=( void*) _temp782.rgn_typ; goto _LL789; _LL789: _temp788=( struct Cyc_Absyn_Conref*)
_temp782.nullable; goto _LL787; _LL787: _temp786=( struct Cyc_Absyn_Tqual*)
_temp782.tq; goto _LL785; _LL785: _temp784=( struct Cyc_Absyn_Conref*) _temp782.bounds;
goto _LL779;} else{ goto _LL780;} _LL780: goto _LL781; _LL779: Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp790); Cyc_Tcutil_check_nonzero_bound( loc, _temp784); return
_temp792; _LL781: return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string
_temp794= Cyc_Absynpp_typ2string( t); xprintf("expecting * or @ type but found %.*s",
_temp794.last_plus_one - _temp794.curr, _temp794.curr);})); _LL777:;}} static
void* Cyc_Tcexp_tcStructMember( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* outer_e, struct Cyc_Absyn_Exp* e, struct
_tagged_string* f){ Cyc_Tcexp_tcExpNoPromote( te, 0, e);{ void* t= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp795= e->topt; if( _temp795 == 0){ _throw(
Null_Exception);} _temp795->v;})); void* _temp796= t; struct Cyc_Absyn_Structdecl**
_temp808; struct Cyc_Absyn_Structdecl* _temp810; struct Cyc_List_List* _temp811;
struct _tuple1* _temp813; struct Cyc_Absyn_Uniondecl** _temp815; struct Cyc_Absyn_Uniondecl*
_temp817; struct Cyc_List_List* _temp818; struct _tuple1* _temp820; struct Cyc_Absyn_Exp*
_temp823; struct Cyc_Absyn_Tqual* _temp825; void* _temp827; struct Cyc_Absyn_PtrInfo
_temp832; _LL798: if(( unsigned int) _temp796 > 4u?(( struct _tunion_struct*)
_temp796)->tag == Cyc_Absyn_StructType_tag: 0){ _LL814: _temp813=( struct
_tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp796)->f1; goto _LL812;
_LL812: _temp811=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp796)->f2; goto _LL809; _LL809: _temp808=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp796)->f3; if( _temp808 == 0){ goto
_LL800;} else{ _temp810=* _temp808; goto _LL799;}} else{ goto _LL800;} _LL800:
if(( unsigned int) _temp796 > 4u?(( struct _tunion_struct*) _temp796)->tag ==
Cyc_Absyn_UnionType_tag: 0){ _LL821: _temp820=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp796)->f1; goto _LL819; _LL819: _temp818=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp796)->f2; goto _LL816; _LL816: _temp815=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp796)->f3;
if( _temp815 == 0){ goto _LL802;} else{ _temp817=* _temp815; goto _LL801;}}
else{ goto _LL802;} _LL802: if(( unsigned int) _temp796 > 4u?(( struct
_tunion_struct*) _temp796)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL828: _temp827=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp796)->f1; goto _LL826; _LL826:
_temp825=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp796)->f2; goto _LL824; _LL824: _temp823=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp796)->f3; goto _LL822;} else{ goto _LL804;}
_LL822: if( Cyc_String_zstrcmp(* f,( struct _tagged_string)({ char* _temp829=(
char*)"size"; struct _tagged_string _temp830; _temp830.curr= _temp829; _temp830.base=
_temp829; _temp830.last_plus_one= _temp829 + 5; _temp830;})) == 0){ goto _LL803;}
else{ goto _LL804;} _LL804: if(( unsigned int) _temp796 > 4u?(( struct
_tunion_struct*) _temp796)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL833:
_temp832=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp796)->f1; goto _LL831;} else{ goto _LL806;} _LL831: if( Cyc_String_zstrcmp(*
f,( struct _tagged_string)({ char* _temp834=( char*)"size"; struct
_tagged_string _temp835; _temp835.curr= _temp834; _temp835.base= _temp834;
_temp835.last_plus_one= _temp834 + 5; _temp835;})) == 0){ goto _LL805;} else{
goto _LL806;} _LL806: goto _LL807; _LL799: if( _temp813 == 0){ return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_string)({ char* _temp836=( char*)"unresolved struct type (COMPILER ERROR)";
struct _tagged_string _temp837; _temp837.curr= _temp836; _temp837.base= _temp836;
_temp837.last_plus_one= _temp836 + 40; _temp837;}));}{ struct Cyc_Absyn_Structfield*
finfo= Cyc_Absyn_lookup_struct_field( _temp810, f); if( finfo == 0){ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp839= Cyc_Absynpp_qvar2string(( struct
_tuple1*)({ struct _tuple1* _temp838= _temp813; if( _temp838 == 0){ _throw(
Null_Exception);} _temp838;})); struct _tagged_string _temp840=* f; xprintf("struct %.*s has no %.*s field",
_temp839.last_plus_one - _temp839.curr, _temp839.curr, _temp840.last_plus_one -
_temp840.curr, _temp840.curr);}));}{ struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp810->tvs,
_temp811); void* t2= Cyc_Tcutil_substitute( inst,( void*)({ struct Cyc_Absyn_Structfield*
_temp841= finfo; if( _temp841 == 0){ _throw( Null_Exception);} _temp841->type;}));
return t2;}} _LL801: if( _temp820 == 0){ return Cyc_Tcexp_expr_err( te, loc,(
struct _tagged_string)({ char* _temp842=( char*)"unresolved union type (COMPILER ERROR)";
struct _tagged_string _temp843; _temp843.curr= _temp842; _temp843.base= _temp842;
_temp843.last_plus_one= _temp842 + 39; _temp843;}));}{ struct Cyc_Absyn_Structfield*
finfo= Cyc_Absyn_lookup_union_field( _temp817, f); if( finfo == 0){ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp845= Cyc_Absynpp_qvar2string(( struct
_tuple1*)({ struct _tuple1* _temp844= _temp820; if( _temp844 == 0){ _throw(
Null_Exception);} _temp844;})); struct _tagged_string _temp846=* f; xprintf("union %.*s has no %.*s field",
_temp845.last_plus_one - _temp845.curr, _temp845.curr, _temp846.last_plus_one -
_temp846.curr, _temp846.curr);}));}{ struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp817->tvs,
_temp818); void* t2= Cyc_Tcutil_substitute( inst,( void*)({ struct Cyc_Absyn_Structfield*
_temp847= finfo; if( _temp847 == 0){ _throw( Null_Exception);} _temp847->type;}));
return t2;}} _LL803:( void*)( outer_e->r=( void*)(( void*)({ struct Cyc_Absyn_Primop_e_struct*
_temp848=( struct Cyc_Absyn_Primop_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct));
_temp848[ 0]=({ struct Cyc_Absyn_Primop_e_struct _temp849; _temp849.tag= Cyc_Absyn_Primop_e_tag;
_temp849.f1=( void*)(( void*) Cyc_Absyn_Size); _temp849.f2=({ struct Cyc_List_List*
_temp850=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp850->hd=( void*) e; _temp850->tl= 0; _temp850;}); _temp849;}); _temp848;})));
return Cyc_Absyn_uint_t; _LL805:( void*)( outer_e->r=( void*)(( void*)({ struct
Cyc_Absyn_Primop_e_struct* _temp851=( struct Cyc_Absyn_Primop_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp851[ 0]=({ struct
Cyc_Absyn_Primop_e_struct _temp852; _temp852.tag= Cyc_Absyn_Primop_e_tag;
_temp852.f1=( void*)(( void*) Cyc_Absyn_Size); _temp852.f2=({ struct Cyc_List_List*
_temp853=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp853->hd=( void*) e; _temp853->tl= 0; _temp853;}); _temp852;}); _temp851;})));
return Cyc_Absyn_uint_t; _LL807: if( Cyc_String_zstrcmp(* f,( struct
_tagged_string)({ char* _temp854=( char*)"size"; struct _tagged_string _temp855;
_temp855.curr= _temp854; _temp855.base= _temp854; _temp855.last_plus_one=
_temp854 + 5; _temp855;})) == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp856= Cyc_Absynpp_typ2string( t); xprintf("expecting struct, union or array, found %.*s",
_temp856.last_plus_one - _temp856.curr, _temp856.curr);}));} else{ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp857= Cyc_Absynpp_typ2string( t); xprintf("expecting struct or union, found %.*s",
_temp857.last_plus_one - _temp857.curr, _temp857.curr);}));} _LL797:;}} static
void* Cyc_Tcexp_tcStructArrow( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e, struct _tagged_string* f){ Cyc_Tcexp_tcExp(
te, 0, e);{ void* t= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp858= e->topt; if( _temp858 == 0){ _throw( Null_Exception);} _temp858->v;}));
void* _temp859= t; struct Cyc_Absyn_PtrInfo _temp865; struct Cyc_Absyn_Conref*
_temp867; struct Cyc_Absyn_Tqual* _temp869; struct Cyc_Absyn_Conref* _temp871;
void* _temp873; void* _temp875; _LL861: if(( unsigned int) _temp859 > 4u?((
struct _tunion_struct*) _temp859)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL866:
_temp865=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp859)->f1; _LL876: _temp875=( void*) _temp865.elt_typ; goto _LL874; _LL874:
_temp873=( void*) _temp865.rgn_typ; goto _LL872; _LL872: _temp871=( struct Cyc_Absyn_Conref*)
_temp865.nullable; goto _LL870; _LL870: _temp869=( struct Cyc_Absyn_Tqual*)
_temp865.tq; goto _LL868; _LL868: _temp867=( struct Cyc_Absyn_Conref*) _temp865.bounds;
goto _LL862;} else{ goto _LL863;} _LL863: goto _LL864; _LL862: Cyc_Tcutil_check_nonzero_bound(
loc, _temp867);{ void* _temp877= Cyc_Tcutil_compress( _temp875); struct Cyc_Absyn_Structdecl**
_temp885; struct Cyc_Absyn_Structdecl* _temp887; struct Cyc_List_List* _temp888;
struct _tuple1* _temp890; struct Cyc_Absyn_Uniondecl** _temp892; struct Cyc_Absyn_Uniondecl*
_temp894; struct Cyc_List_List* _temp895; struct _tuple1* _temp897; _LL879: if((
unsigned int) _temp877 > 4u?(( struct _tunion_struct*) _temp877)->tag == Cyc_Absyn_StructType_tag:
0){ _LL891: _temp890=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp877)->f1; goto _LL889; _LL889: _temp888=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp877)->f2; goto _LL886; _LL886: _temp885=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp877)->f3;
if( _temp885 == 0){ goto _LL881;} else{ _temp887=* _temp885; goto _LL880;}}
else{ goto _LL881;} _LL881: if(( unsigned int) _temp877 > 4u?(( struct
_tunion_struct*) _temp877)->tag == Cyc_Absyn_UnionType_tag: 0){ _LL898: _temp897=(
struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*) _temp877)->f1; goto
_LL896; _LL896: _temp895=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp877)->f2; goto _LL893; _LL893: _temp892=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp877)->f3; if( _temp892 == 0){ goto
_LL883;} else{ _temp894=* _temp892; goto _LL882;}} else{ goto _LL883;} _LL883:
goto _LL884; _LL880: { struct Cyc_Absyn_Structfield* finfo= Cyc_Absyn_lookup_struct_field(
_temp887, f); if( finfo == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp900= Cyc_Absynpp_qvar2string(( struct _tuple1*)({ struct
_tuple1* _temp899= _temp890; if( _temp899 == 0){ _throw( Null_Exception);}
_temp899;})); struct _tagged_string _temp901=* f; xprintf("struct %.*s has no %.*s field",
_temp900.last_plus_one - _temp900.curr, _temp900.curr, _temp901.last_plus_one -
_temp901.curr, _temp901.curr);}));}{ struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp887->tvs,
_temp888); void* t3= Cyc_Tcutil_substitute( inst,( void*)({ struct Cyc_Absyn_Structfield*
_temp902= finfo; if( _temp902 == 0){ _throw( Null_Exception);} _temp902->type;}));
Cyc_Tcenv_check_rgn_accessible( te, loc, _temp873); return t3;}} _LL882: {
struct Cyc_Absyn_Structfield* finfo= Cyc_Absyn_lookup_union_field( _temp894, f);
if( finfo == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string
_temp904= Cyc_Absynpp_qvar2string(( struct _tuple1*)({ struct _tuple1* _temp903=
_temp897; if( _temp903 == 0){ _throw( Null_Exception);} _temp903;})); struct
_tagged_string _temp905=* f; xprintf("union %.*s has no %.*s field", _temp904.last_plus_one
- _temp904.curr, _temp904.curr, _temp905.last_plus_one - _temp905.curr, _temp905.curr);}));}{
struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_zip)( _temp894->tvs, _temp895); void* t3= Cyc_Tcutil_substitute(
inst,( void*)({ struct Cyc_Absyn_Structfield* _temp906= finfo; if( _temp906 == 0){
_throw( Null_Exception);} _temp906->type;})); Cyc_Tcenv_check_rgn_accessible( te,
loc, _temp873); return t3;}} _LL884: return Cyc_Tcexp_expr_err( te, loc,({
struct _tagged_string _temp907= Cyc_Absynpp_typ2string( t); xprintf("expecting struct pointer, found %.*s",
_temp907.last_plus_one - _temp907.curr, _temp907.curr);})); _LL878:;} _LL864:
return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string _temp908= Cyc_Absynpp_typ2string(
t); xprintf("expecting struct pointer, found %.*s", _temp908.last_plus_one -
_temp908.curr, _temp908.curr);})); _LL860:;}} static void* Cyc_Tcexp_ithTupleType(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
ts, struct Cyc_Absyn_Exp* index){ unsigned int i= Cyc_Evexp_eval_const_uint_exp(
index); struct _handler_cons _temp909; _push_handler(& _temp909);{ struct
_xtunion_struct* _temp910=( struct _xtunion_struct*) setjmp( _temp909.handler);
if( ! _temp910){{ void* _temp911=(*(( struct _tuple7*(*)( struct Cyc_List_List*
x, int i)) Cyc_List_nth)( ts,( int) i)).f2; _npop_handler( 0u); return _temp911;};
_pop_handler();} else{ struct _xtunion_struct* _temp913= _temp910; _LL915: if(
_temp913->tag == Cyc_List_Nth_tag){ goto _LL916;} else{ goto _LL917;} _LL917:
goto _LL918; _LL916: return Cyc_Tcexp_expr_err( te, loc, xprintf("index is %d but tuple has only %d fields",(
int) i,(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( ts))); _LL918:(
void) _throw( _temp913); _LL914:;}}} static void* Cyc_Tcexp_tcSubscript( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp( te, 0, e1); Cyc_Tcexp_tcExp( te,
0, e2);{ void* t1= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp919=
e1->topt; if( _temp919 == 0){ _throw( Null_Exception);} _temp919->v;})); void*
t2= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp920= e2->topt; if(
_temp920 == 0){ _throw( Null_Exception);} _temp920->v;})); if( ! Cyc_Tcutil_coerce_sint_typ(
te, e2)){ return Cyc_Tcexp_expr_err( te, e2->loc,({ struct _tagged_string
_temp921= Cyc_Absynpp_typ2string( t2); xprintf("expecting int subscript, found %.*s",
_temp921.last_plus_one - _temp921.curr, _temp921.curr);}));}{ void* _temp922= t1;
struct Cyc_Absyn_PtrInfo _temp930; struct Cyc_Absyn_Conref* _temp932; struct Cyc_Absyn_Tqual*
_temp934; struct Cyc_Absyn_Conref* _temp936; void* _temp938; void* _temp940;
struct Cyc_List_List* _temp942; _LL924: if(( unsigned int) _temp922 > 4u?((
struct _tunion_struct*) _temp922)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL931:
_temp930=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp922)->f1; _LL941: _temp940=( void*) _temp930.elt_typ; goto _LL939; _LL939:
_temp938=( void*) _temp930.rgn_typ; goto _LL937; _LL937: _temp936=( struct Cyc_Absyn_Conref*)
_temp930.nullable; goto _LL935; _LL935: _temp934=( struct Cyc_Absyn_Tqual*)
_temp930.tq; goto _LL933; _LL933: _temp932=( struct Cyc_Absyn_Conref*) _temp930.bounds;
goto _LL925;} else{ goto _LL926;} _LL926: if(( unsigned int) _temp922 > 4u?((
struct _tunion_struct*) _temp922)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL943:
_temp942=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp922)->f1;
goto _LL927;} else{ goto _LL928;} _LL928: goto _LL929; _LL925: if( Cyc_Tcutil_is_const_exp(
te, e2)){ Cyc_Tcutil_check_bound( loc, Cyc_Evexp_eval_const_uint_exp( e2),
_temp932);} else{ Cyc_Tcutil_check_nonzero_bound( loc, _temp932);} Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp938); return _temp940; _LL927: return Cyc_Tcexp_ithTupleType( te,
loc, _temp942, e2); _LL929: return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp944= Cyc_Absynpp_typ2string( t1); xprintf("subscript applied to %.*s",
_temp944.last_plus_one - _temp944.curr, _temp944.curr);})); _LL923:;}}} static
void* Cyc_Tcexp_tcTuple( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_List_List* es){ int done= 0; struct Cyc_List_List*
fields= 0; if( topt != 0){ void* _temp946= Cyc_Tcutil_compress(({ void**
_temp945= topt; if( _temp945 == 0){ _throw( Null_Exception);}* _temp945;}));
struct Cyc_List_List* _temp952; _LL948: if(( unsigned int) _temp946 > 4u?((
struct _tunion_struct*) _temp946)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL953:
_temp952=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp946)->f1;
goto _LL949;} else{ goto _LL950;} _LL950: goto _LL951; _LL949: if((( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp952) !=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( es)){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp954=( char*)"tuple expression has the wrong number of fields"; struct
_tagged_string _temp955; _temp955.curr= _temp954; _temp955.base= _temp954;
_temp955.last_plus_one= _temp954 + 48; _temp955;})); goto _LL947;} for( 0; es !=
0; es=({ struct Cyc_List_List* _temp956= es; if( _temp956 == 0){ _throw(
Null_Exception);} _temp956->tl;}), _temp952=({ struct Cyc_List_List* _temp957=
_temp952; if( _temp957 == 0){ _throw( Null_Exception);} _temp957->tl;})){ void*
topt2=(*(( struct _tuple7*)({ struct Cyc_List_List* _temp958= _temp952; if(
_temp958 == 0){ _throw( Null_Exception);} _temp958->hd;}))).f2; Cyc_Tcexp_tcExpInitializer(
te,( void**)& topt2,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp959=
es; if( _temp959 == 0){ _throw( Null_Exception);} _temp959->hd;})); fields=({
struct Cyc_List_List* _temp960=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp960->hd=( void*)({ struct _tuple7* _temp961=(
struct _tuple7*) GC_malloc( sizeof( struct _tuple7)); _temp961->f1=(*(( struct
_tuple7*)({ struct Cyc_List_List* _temp964= _temp952; if( _temp964 == 0){ _throw(
Null_Exception);} _temp964->hd;}))).f1; _temp961->f2=( void*)({ struct Cyc_Core_Opt*
_temp963=(( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp962= es; if(
_temp962 == 0){ _throw( Null_Exception);} _temp962->hd;}))->topt; if( _temp963
== 0){ _throw( Null_Exception);} _temp963->v;}); _temp961;}); _temp960->tl=
fields; _temp960;});} done= 1; goto _LL947; _LL951: goto _LL947; _LL947:;} if( !
done){ for( 0; es != 0; es=({ struct Cyc_List_List* _temp965= es; if( _temp965
== 0){ _throw( Null_Exception);} _temp965->tl;})){ Cyc_Tcexp_tcExpInitializer(
te, 0,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp966= es; if(
_temp966 == 0){ _throw( Null_Exception);} _temp966->hd;})); fields=({ struct Cyc_List_List*
_temp967=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp967->hd=( void*)({ struct _tuple7* _temp968=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp968->f1= Cyc_Absyn_empty_tqual(); _temp968->f2=(
void*)({ struct Cyc_Core_Opt* _temp970=(( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp969= es; if( _temp969 == 0){ _throw( Null_Exception);} _temp969->hd;}))->topt;
if( _temp970 == 0){ _throw( Null_Exception);} _temp970->v;}); _temp968;});
_temp967->tl= fields; _temp967;});}} return( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp971=( struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp971[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp972; _temp972.tag= Cyc_Absyn_TupleType_tag;
_temp972.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fields); _temp972;}); _temp971;});} static void* Cyc_Tcexp_tcCompoundLit( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
_tuple2* t, struct Cyc_List_List* des){ return Cyc_Tcexp_expr_err( te, loc,(
struct _tagged_string)({ char* _temp973=( char*)"tcCompoundLit"; struct
_tagged_string _temp974; _temp974.curr= _temp973; _temp974.base= _temp973;
_temp974.last_plus_one= _temp973 + 14; _temp974;}));} static void* Cyc_Tcexp_tcArray(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** elt_topt,
struct Cyc_List_List* des){ struct Cyc_List_List* es=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)((
struct Cyc_Absyn_Exp*(*)( struct _tuple6*)) Cyc_Core_snd, des); void* res= Cyc_Absyn_new_evar((
void*) Cyc_Absyn_MemKind); struct Cyc_Absyn_Const_e_struct* sz_rexp=({ struct
Cyc_Absyn_Const_e_struct* _temp975=( struct Cyc_Absyn_Const_e_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Const_e_struct)); _temp975[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp976; _temp976.tag= Cyc_Absyn_Const_e_tag; _temp976.f1=( void*)(( void*)({
struct Cyc_Absyn_Int_c_struct* _temp977=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp977[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp978; _temp978.tag= Cyc_Absyn_Int_c_tag; _temp978.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp978.f2=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es); _temp978;});
_temp977;})); _temp976;}); _temp975;}); struct Cyc_Absyn_Exp* sz_exp= Cyc_Absyn_new_exp((
void*) sz_rexp, loc); sz_exp->topt=({ struct Cyc_Core_Opt* _temp979=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp979->v=( void*) Cyc_Absyn_uint_t;
_temp979;});{ void* res_t2=( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp980=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp980[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp981; _temp981.tag= Cyc_Absyn_ArrayType_tag;
_temp981.f1=( void*) res; _temp981.f2= Cyc_Absyn_empty_tqual(); _temp981.f3=(
struct Cyc_Absyn_Exp*) sz_exp; _temp981;}); _temp980;});{ struct Cyc_List_List*
es2= es; for( 0; es2 != 0; es2=({ struct Cyc_List_List* _temp982= es2; if(
_temp982 == 0){ _throw( Null_Exception);} _temp982->tl;})){ Cyc_Tcexp_tcExpInitializer(
te, elt_topt,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp983= es2; if(
_temp983 == 0){ _throw( Null_Exception);} _temp983->hd;}));}} if( ! Cyc_Tcutil_coerce_list(
te, res, es)){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp984= es; if( _temp984 == 0){ _throw( Null_Exception);} _temp984->hd;}))->loc,({
struct _tagged_string _temp985= Cyc_Absynpp_typ2string( res); xprintf("elements of array do not all have the same type (%.*s)",
_temp985.last_plus_one - _temp985.curr, _temp985.curr);}));}{ int offset= 0;
for( 0; des != 0; offset ++, des=({ struct Cyc_List_List* _temp986= des; if(
_temp986 == 0){ _throw( Null_Exception);} _temp986->tl;})){ struct Cyc_List_List*
ds=(*(( struct _tuple6*)({ struct Cyc_List_List* _temp987= des; if( _temp987 ==
0){ _throw( Null_Exception);} _temp987->hd;}))).f1; if( ds != 0){ void* _temp989=(
void*)({ struct Cyc_List_List* _temp988= ds; if( _temp988 == 0){ _throw(
Null_Exception);} _temp988->hd;}); struct _tagged_string* _temp995; struct Cyc_Absyn_Exp*
_temp997; _LL991: if((( struct _tunion_struct*) _temp989)->tag == Cyc_Absyn_FieldName_tag){
_LL996: _temp995=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp989)->f1; goto _LL992;} else{ goto _LL993;} _LL993: if((( struct
_tunion_struct*) _temp989)->tag == Cyc_Absyn_ArrayElement_tag){ _LL998: _temp997=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayElement_struct*) _temp989)->f1;
goto _LL994;} else{ goto _LL990;} _LL992: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp999=( char*)"only array index designators are supported";
struct _tagged_string _temp1000; _temp1000.curr= _temp999; _temp1000.base=
_temp999; _temp1000.last_plus_one= _temp999 + 43; _temp1000;})); goto _LL990;
_LL994: Cyc_Tcexp_tcExpInitializer( te, 0, _temp997);{ unsigned int i= Cyc_Evexp_eval_const_uint_exp(
_temp997); if( i != offset){ Cyc_Tcutil_terr( _temp997->loc, xprintf("expecting index designator %d but found %d",
offset,( int) i));} goto _LL990;} _LL990:;}}} return res_t2;}} static void* Cyc_Tcexp_tcComprehension(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp* bound, struct Cyc_Absyn_Exp* body){
Cyc_Tcexp_tcExp( te, 0, bound); if( ! Cyc_Tcutil_coerce_uint_typ( te, bound)){
Cyc_Tcutil_terr( bound->loc,({ struct _tagged_string _temp1002= Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Core_Opt* _temp1001= bound->topt; if( _temp1001 == 0){
_throw( Null_Exception);} _temp1001->v;})); xprintf("expecting unsigned int, found %.*s",
_temp1002.last_plus_one - _temp1002.curr, _temp1002.curr);}));} if( te->le != 0){
te= Cyc_Tcenv_new_block( loc, te); te= Cyc_Tcenv_add_local_var( loc, te, vd);}{
struct Cyc_Absyn_PtrInfo pinfo; void** topt2= 0; struct Cyc_Absyn_Tqual** tqopt=
0; if( topt != 0){ void* _temp1004= Cyc_Tcutil_compress(({ void** _temp1003=
topt; if( _temp1003 == 0){ _throw( Null_Exception);}* _temp1003;})); struct Cyc_Absyn_PtrInfo
_temp1012; struct Cyc_Absyn_Exp* _temp1014; struct Cyc_Absyn_Tqual* _temp1016;
struct Cyc_Absyn_Tqual** _temp1018; void* _temp1019; void** _temp1021; _LL1006:
if(( unsigned int) _temp1004 > 4u?(( struct _tunion_struct*) _temp1004)->tag ==
Cyc_Absyn_PointerType_tag: 0){ _LL1013: _temp1012=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1004)->f1; goto _LL1007;} else{ goto
_LL1008;} _LL1008: if(( unsigned int) _temp1004 > 4u?(( struct _tunion_struct*)
_temp1004)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL1020: _temp1019=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1004)->f1; _temp1021=&(( struct Cyc_Absyn_ArrayType_struct*)
_temp1004)->f1; goto _LL1017; _LL1017: _temp1016=( struct Cyc_Absyn_Tqual*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1004)->f2; _temp1018=&(( struct Cyc_Absyn_ArrayType_struct*)
_temp1004)->f2; goto _LL1015; _LL1015: _temp1014=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1004)->f3; goto _LL1009;} else{ goto
_LL1010;} _LL1010: goto _LL1011; _LL1007: pinfo= _temp1012; topt2=( void**)((
void*)& pinfo.elt_typ); tqopt=( struct Cyc_Absyn_Tqual**)& pinfo.tq; goto
_LL1005; _LL1009: topt2=( void**) _temp1021; tqopt=( struct Cyc_Absyn_Tqual**)
_temp1018; goto _LL1005; _LL1011: goto _LL1005; _LL1005:;}{ void* t= Cyc_Tcexp_tcExp(
te, topt2, body); return( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp1022=(
struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1022[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp1023; _temp1023.tag= Cyc_Absyn_ArrayType_tag;
_temp1023.f1=( void*) t; _temp1023.f2= tqopt == 0? Cyc_Absyn_empty_tqual():({
struct Cyc_Absyn_Tqual** _temp1024= tqopt; if( _temp1024 == 0){ _throw(
Null_Exception);}* _temp1024;}); _temp1023.f3=( struct Cyc_Absyn_Exp*) bound;
_temp1023;}); _temp1022;});}}} static void* Cyc_Tcexp_tcStruct( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct _tuple1** tn, struct
Cyc_Core_Opt** otyps, struct Cyc_List_List* args, struct Cyc_Absyn_Structdecl**
sd_opt){ struct Cyc_Absyn_Structdecl* sd; if(* sd_opt != 0){ sd=( struct Cyc_Absyn_Structdecl*)({
struct Cyc_Absyn_Structdecl* _temp1025=* sd_opt; if( _temp1025 == 0){ _throw(
Null_Exception);} _temp1025;});} else{{ struct _handler_cons _temp1026;
_push_handler(& _temp1026);{ struct _xtunion_struct* _temp1027=( struct
_xtunion_struct*) setjmp( _temp1026.handler); if( ! _temp1027){ sd=* Cyc_Tcenv_lookup_structdecl(
te, loc,* tn);; _pop_handler();} else{ struct _xtunion_struct* _temp1029=
_temp1027; _LL1031: if( _temp1029->tag == Cyc_Dict_Absent_tag){ goto _LL1032;}
else{ goto _LL1033;} _LL1033: goto _LL1034; _LL1032: Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp1035= Cyc_Absynpp_qvar2string(* tn); xprintf("unbound struct name %.*s",
_temp1035.last_plus_one - _temp1035.curr, _temp1035.curr);})); return topt != 0?({
void** _temp1036= topt; if( _temp1036 == 0){ _throw( Null_Exception);}*
_temp1036;}):( void*) Cyc_Absyn_VoidType; _LL1034:( void) _throw( _temp1029);
_LL1030:;}}}* sd_opt=( struct Cyc_Absyn_Structdecl*) sd; if( sd->name != 0){* tn=(
struct _tuple1*)({ struct Cyc_Core_Opt* _temp1037= sd->name; if( _temp1037 == 0){
_throw( Null_Exception);} _temp1037->v;});}}{ struct Cyc_List_List* inst=((
struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_Absyn_Tvar*), struct
Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_make_inst_var, sd->tvs); struct Cyc_List_List*
typs=(( struct Cyc_List_List*(*)( void*(* f)( struct _tuple4*), struct Cyc_List_List*
x)) Cyc_List_map)(( void*(*)( struct _tuple4*)) Cyc_Core_snd, inst); struct Cyc_Absyn_StructType_struct*
res_typ=({ struct Cyc_Absyn_StructType_struct* _temp1038=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1038[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1039; _temp1039.tag= Cyc_Absyn_StructType_tag;
_temp1039.f1=( struct _tuple1*)* tn; _temp1039.f2= typs; _temp1039.f3=({ struct
Cyc_Absyn_Structdecl** _temp1040=( struct Cyc_Absyn_Structdecl**) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl*)); _temp1040[ 0]= sd; _temp1040;});
_temp1039;}); _temp1038;}); if( topt != 0){ Cyc_Tcutil_unify(( void*) res_typ,({
void** _temp1041= topt; if( _temp1041 == 0){ _throw( Null_Exception);}*
_temp1041;}));}* otyps=({ struct Cyc_Core_Opt* _temp1042=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1042->v=( void*) typs; _temp1042;});{
struct Cyc_List_List* fields=(( struct Cyc_List_List*(*)( struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_Absyn_Structdecl* sd)) Cyc_Tcutil_resolve_struct_designators)(
loc, args, sd); for( 0; fields != 0; fields=({ struct Cyc_List_List* _temp1043=
fields; if( _temp1043 == 0){ _throw( Null_Exception);} _temp1043->tl;})){ struct
_tuple8 _temp1047; struct Cyc_Absyn_Exp* _temp1048; struct Cyc_Absyn_Structfield*
_temp1050; struct _tuple8* _temp1045=( struct _tuple8*)({ struct Cyc_List_List*
_temp1044= fields; if( _temp1044 == 0){ _throw( Null_Exception);} _temp1044->hd;});
_temp1047=* _temp1045; _LL1051: _temp1050= _temp1047.f1; goto _LL1049; _LL1049:
_temp1048= _temp1047.f2; goto _LL1046; _LL1046: { void* inst_fieldtyp= Cyc_Tcutil_substitute(
inst,( void*) _temp1050->type); Cyc_Tcexp_tcExpInitializer( te,( void**)&
inst_fieldtyp, _temp1048); if( ! Cyc_Tcutil_coerce_arg( te, _temp1048,
inst_fieldtyp)){ Cyc_Tcutil_terr( _temp1048->loc,({ struct _tagged_string
_temp1053=* _temp1050->name; struct _tagged_string _temp1054= Cyc_Absynpp_qvar2string(*
tn); struct _tagged_string _temp1055= Cyc_Absynpp_typ2string( inst_fieldtyp);
struct _tagged_string _temp1056= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp1052= _temp1048->topt; if( _temp1052 == 0){ _throw( Null_Exception);}
_temp1052->v;})); xprintf("field %.*s of struct %.*s expects type %.*s != %.*s",
_temp1053.last_plus_one - _temp1053.curr, _temp1053.curr, _temp1054.last_plus_one
- _temp1054.curr, _temp1054.curr, _temp1055.last_plus_one - _temp1055.curr,
_temp1055.curr, _temp1056.last_plus_one - _temp1056.curr, _temp1056.curr);}));}}}
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
Cyc_Core_Opt* _temp1057=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1057->v=( void*) all_typs; _temp1057;});* exist_ref=({ struct Cyc_Core_Opt*
_temp1058=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1058->v=( void*) exist_typs; _temp1058;});{ void* res=( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp1059=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp1059[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp1060; _temp1060.tag= Cyc_Absyn_TunionFieldType_tag; _temp1060.f1=({ struct
Cyc_Absyn_TunionFieldInfo _temp1061; _temp1061.name=( struct _tuple1*)({ struct
Cyc_Core_Opt* _temp1062= ed->name; if( _temp1062 == 0){ _throw( Null_Exception);}
_temp1062->v;}); _temp1061.targs= all_typs; _temp1061.fname= ef->name; _temp1061.tud=(
struct Cyc_Absyn_Tuniondecl*) ed; _temp1061.tufd=( struct Cyc_Absyn_Tunionfield*)
ef; _temp1061;}); _temp1060;}); _temp1059;}); if( topt != 0){ void* _temp1064=
Cyc_Tcutil_compress(({ void** _temp1063= topt; if( _temp1063 == 0){ _throw(
Null_Exception);}* _temp1063;})); struct Cyc_Absyn_TunionFieldInfo _temp1072;
struct Cyc_Absyn_TunionInfo _temp1074; struct Cyc_Absyn_Tuniondecl* _temp1076;
void* _temp1078; struct Cyc_List_List* _temp1080; struct _tuple1* _temp1082;
_LL1066: if(( unsigned int) _temp1064 > 4u?(( struct _tunion_struct*) _temp1064)->tag
== Cyc_Absyn_TunionFieldType_tag: 0){ _LL1073: _temp1072=( struct Cyc_Absyn_TunionFieldInfo)((
struct Cyc_Absyn_TunionFieldType_struct*) _temp1064)->f1; goto _LL1067;} else{
goto _LL1068;} _LL1068: if(( unsigned int) _temp1064 > 4u?(( struct
_tunion_struct*) _temp1064)->tag == Cyc_Absyn_TunionType_tag: 0){ _LL1075:
_temp1074=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp1064)->f1; _LL1083: _temp1082=( struct _tuple1*) _temp1074.name; goto
_LL1081; _LL1081: _temp1080=( struct Cyc_List_List*) _temp1074.targs; goto
_LL1079; _LL1079: _temp1078=( void*) _temp1074.rgn; goto _LL1077; _LL1077:
_temp1076=( struct Cyc_Absyn_Tuniondecl*) _temp1074.tud; goto _LL1069;} else{
goto _LL1070;} _LL1070: goto _LL1071; _LL1067: Cyc_Tcutil_unify(({ void**
_temp1084= topt; if( _temp1084 == 0){ _throw( Null_Exception);}* _temp1084;}),
res); goto _LL1065; _LL1069:{ struct Cyc_List_List* a= all_typs; for( 0; a != 0?
_temp1080 != 0: 0; a=({ struct Cyc_List_List* _temp1085= a; if( _temp1085 == 0){
_throw( Null_Exception);} _temp1085->tl;}), _temp1080=({ struct Cyc_List_List*
_temp1086= _temp1080; if( _temp1086 == 0){ _throw( Null_Exception);} _temp1086->tl;})){
Cyc_Tcutil_unify(( void*)({ struct Cyc_List_List* _temp1087= a; if( _temp1087 ==
0){ _throw( Null_Exception);} _temp1087->hd;}),( void*)({ struct Cyc_List_List*
_temp1088= _temp1080; if( _temp1088 == 0){ _throw( Null_Exception);} _temp1088->hd;}));}}
if( ef->typs == 0? es == 0: 0){ e->topt=({ struct Cyc_Core_Opt* _temp1089=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1089->v=(
void*) res; _temp1089;}); res=( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp1090=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp1090[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp1091; _temp1091.tag=
Cyc_Absyn_TunionType_tag; _temp1091.f1=({ struct Cyc_Absyn_TunionInfo _temp1092;
_temp1092.name=( struct _tuple1*)({ struct Cyc_Core_Opt* _temp1093= ed->name;
if( _temp1093 == 0){ _throw( Null_Exception);} _temp1093->v;}); _temp1092.targs=
all_typs; _temp1092.rgn=( void*) Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind);
_temp1092.tud=( struct Cyc_Absyn_Tuniondecl*) ed; _temp1092;}); _temp1091;});
_temp1090;}); Cyc_Tcutil_unchecked_cast( te, e, res); return res;} goto _LL1065;
_LL1071: goto _LL1065; _LL1065:;}{ struct Cyc_List_List* ts= ef->typs; for( 0;
es != 0? ts != 0: 0; es=({ struct Cyc_List_List* _temp1094= es; if( _temp1094 ==
0){ _throw( Null_Exception);} _temp1094->tl;}), ts=({ struct Cyc_List_List*
_temp1095= ts; if( _temp1095 == 0){ _throw( Null_Exception);} _temp1095->tl;})){
struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1096= es; if( _temp1096 == 0){ _throw( Null_Exception);} _temp1096->hd;});
void* t= Cyc_Tcutil_substitute( inst,(*(( struct _tuple7*)({ struct Cyc_List_List*
_temp1097= ts; if( _temp1097 == 0){ _throw( Null_Exception);} _temp1097->hd;}))).f2);
Cyc_Tcexp_tcExpInitializer( te,( void**)& t, e); if( ! Cyc_Tcutil_coerce_arg( te,
e, t)){ Cyc_Tcutil_terr( e->loc,({ struct _tagged_string _temp1101= Cyc_Absynpp_qvar2string(
ef->name); struct _tagged_string _temp1102= Cyc_Absynpp_typ2string( t); struct
_tagged_string _temp1103= e->topt == 0?( struct _tagged_string)({ char*
_temp1098=( char*)"?"; struct _tagged_string _temp1099; _temp1099.curr=
_temp1098; _temp1099.base= _temp1098; _temp1099.last_plus_one= _temp1098 + 2;
_temp1099;}): Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt* _temp1100=
e->topt; if( _temp1100 == 0){ _throw( Null_Exception);} _temp1100->v;}));
xprintf("tunion constructor %.*s expects argument of type %.*s but this argument has type %.*s",
_temp1101.last_plus_one - _temp1101.curr, _temp1101.curr, _temp1102.last_plus_one
- _temp1102.curr, _temp1102.curr, _temp1103.last_plus_one - _temp1103.curr,
_temp1103.curr);}));}} if( es != 0){ return Cyc_Tcexp_expr_err( te,(( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1104= es; if( _temp1104 == 0){ _throw( Null_Exception);}
_temp1104->hd;}))->loc,({ struct _tagged_string _temp1105= Cyc_Absynpp_qvar2string(
ef->name); xprintf("too many arguments for tunion constructor %.*s", _temp1105.last_plus_one
- _temp1105.curr, _temp1105.curr);}));} if( ts != 0){ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp1106= Cyc_Absynpp_qvar2string( ef->name);
xprintf("too few arguments for tunion constructor %.*s", _temp1106.last_plus_one
- _temp1106.curr, _temp1106.curr);}));} return res;}}} static void* Cyc_Tcexp_tcXTunion(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Core_Opt** exist_ref, struct Cyc_List_List* es, struct Cyc_Absyn_XTuniondecl*
xd, struct Cyc_Absyn_Tunionfield* ef){ struct Cyc_List_List* exist_inst=((
struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_Absyn_Tvar*), struct
Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_make_inst_var, ef->tvs); struct Cyc_List_List*
exist_typs=(( struct Cyc_List_List*(*)( void*(* f)( struct _tuple4*), struct Cyc_List_List*
x)) Cyc_List_map)(( void*(*)( struct _tuple4*)) Cyc_Core_snd, exist_inst);
struct Cyc_Absyn_XTunionFieldType_struct* res=({ struct Cyc_Absyn_XTunionFieldType_struct*
_temp1107=( struct Cyc_Absyn_XTunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_XTunionFieldType_struct)); _temp1107[ 0]=({ struct Cyc_Absyn_XTunionFieldType_struct
_temp1108; _temp1108.tag= Cyc_Absyn_XTunionFieldType_tag; _temp1108.f1=({ struct
Cyc_Absyn_XTunionFieldInfo _temp1109; _temp1109.name= xd->name; _temp1109.fname=
ef->name; _temp1109.xtud=( struct Cyc_Absyn_XTuniondecl*) xd; _temp1109.xtufd=(
struct Cyc_Absyn_Tunionfield*) ef; _temp1109;}); _temp1108;}); _temp1107;});
struct Cyc_List_List* ts= ef->typs;* exist_ref=({ struct Cyc_Core_Opt* _temp1110=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1110->v=(
void*) exist_typs; _temp1110;}); while( es != 0? ts != 0: 0) { struct Cyc_Absyn_Exp*
e=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1111= es; if( _temp1111
== 0){ _throw( Null_Exception);} _temp1111->hd;}); void* t= Cyc_Tcutil_substitute(
exist_inst,(*(( struct _tuple7*)({ struct Cyc_List_List* _temp1112= ts; if(
_temp1112 == 0){ _throw( Null_Exception);} _temp1112->hd;}))).f2); Cyc_Tcexp_tcExpInitializer(
te,( void**)& t, e); es=({ struct Cyc_List_List* _temp1113= es; if( _temp1113 ==
0){ _throw( Null_Exception);} _temp1113->tl;}); ts=({ struct Cyc_List_List*
_temp1114= ts; if( _temp1114 == 0){ _throw( Null_Exception);} _temp1114->tl;});
if( ! Cyc_Tcutil_coerce_arg( te, e, t)){ Cyc_Tcutil_terr( e->loc,({ struct
_tagged_string _temp1116= Cyc_Absynpp_qvar2string( ef->name); struct
_tagged_string _temp1117= Cyc_Absynpp_typ2string( t); struct _tagged_string
_temp1118= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt* _temp1115= e->topt;
if( _temp1115 == 0){ _throw( Null_Exception);} _temp1115->v;})); xprintf("xtunion constructor %.*s expects argument of type %.*s but this argument has type %.*s",
_temp1116.last_plus_one - _temp1116.curr, _temp1116.curr, _temp1117.last_plus_one
- _temp1117.curr, _temp1117.curr, _temp1118.last_plus_one - _temp1118.curr,
_temp1118.curr);}));}} if( es != 0){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1119= es; if( _temp1119 == 0){ _throw( Null_Exception);}
_temp1119->hd;}))->loc,({ struct _tagged_string _temp1120= Cyc_Absynpp_qvar2string(
ef->name); xprintf("too many arguments for xtunion constructor %.*s", _temp1120.last_plus_one
- _temp1120.curr, _temp1120.curr);}));} else{ if( ts != 0){ Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1121= es; if( _temp1121 == 0){
_throw( Null_Exception);} _temp1121->hd;}))->loc,({ struct _tagged_string
_temp1122= Cyc_Absynpp_qvar2string( ef->name); xprintf("too few arguments for xtunion constructor %.*s",
_temp1122.last_plus_one - _temp1122.curr, _temp1122.curr);}));}} return( void*)
res;} static void* Cyc_Tcexp_tcMalloc( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* ropt, void* t){ void* rgn=( void*) Cyc_Absyn_HeapRgn;
if( ropt != 0){ void* expected_type=( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp1123=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp1123[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp1124; _temp1124.tag= Cyc_Absyn_RgnHandleType_tag; _temp1124.f1=( void*) Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind); _temp1124;}); _temp1123;}); void* handle_type= Cyc_Tcexp_tcExp(
te,( void**)& expected_type,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1125= ropt; if( _temp1125 == 0){ _throw( Null_Exception);} _temp1125;}));
void* _temp1126= Cyc_Tcutil_compress( handle_type); void* _temp1132; _LL1128:
if(( unsigned int) _temp1126 > 4u?(( struct _tunion_struct*) _temp1126)->tag ==
Cyc_Absyn_RgnHandleType_tag: 0){ _LL1133: _temp1132=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1126)->f1; goto _LL1129;} else{ goto _LL1130;} _LL1130: goto _LL1131;
_LL1129: rgn= _temp1132; Cyc_Tcenv_check_rgn_accessible( te, loc, rgn); goto
_LL1127; _LL1131: Cyc_Tcutil_terr(({ struct Cyc_Absyn_Exp* _temp1134= ropt; if(
_temp1134 == 0){ _throw( Null_Exception);} _temp1134->loc;}),({ struct
_tagged_string _temp1135= Cyc_Absynpp_typ2string( handle_type); xprintf("expecting region_t type but found %.*s",
_temp1135.last_plus_one - _temp1135.curr, _temp1135.curr);})); goto _LL1127;
_LL1127:;} Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars( te),(
void*) Cyc_Absyn_MemKind, t);{ void* _temp1136= t; struct Cyc_Absyn_TunionFieldInfo
_temp1144; struct Cyc_Absyn_Tunionfield* _temp1146; struct Cyc_Absyn_Tuniondecl*
_temp1148; struct _tuple1* _temp1150; struct Cyc_List_List* _temp1152; struct
_tuple1* _temp1154; struct Cyc_Absyn_XTunionFieldInfo _temp1156; struct Cyc_Absyn_Tunionfield*
_temp1158; struct Cyc_Absyn_XTuniondecl* _temp1160; struct _tuple1* _temp1162;
struct _tuple1* _temp1164; _LL1138: if(( unsigned int) _temp1136 > 4u?(( struct
_tunion_struct*) _temp1136)->tag == Cyc_Absyn_TunionFieldType_tag: 0){ _LL1145:
_temp1144=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp1136)->f1; _LL1155: _temp1154=( struct _tuple1*) _temp1144.name; goto
_LL1153; _LL1153: _temp1152=( struct Cyc_List_List*) _temp1144.targs; goto
_LL1151; _LL1151: _temp1150=( struct _tuple1*) _temp1144.fname; goto _LL1149;
_LL1149: _temp1148=( struct Cyc_Absyn_Tuniondecl*) _temp1144.tud; goto _LL1147;
_LL1147: _temp1146=( struct Cyc_Absyn_Tunionfield*) _temp1144.tufd; goto _LL1139;}
else{ goto _LL1140;} _LL1140: if(( unsigned int) _temp1136 > 4u?(( struct
_tunion_struct*) _temp1136)->tag == Cyc_Absyn_XTunionFieldType_tag: 0){ _LL1157:
_temp1156=( struct Cyc_Absyn_XTunionFieldInfo)(( struct Cyc_Absyn_XTunionFieldType_struct*)
_temp1136)->f1; _LL1165: _temp1164=( struct _tuple1*) _temp1156.name; goto
_LL1163; _LL1163: _temp1162=( struct _tuple1*) _temp1156.fname; goto _LL1161;
_LL1161: _temp1160=( struct Cyc_Absyn_XTuniondecl*) _temp1156.xtud; goto _LL1159;
_LL1159: _temp1158=( struct Cyc_Absyn_Tunionfield*) _temp1156.xtufd; goto
_LL1141;} else{ goto _LL1142;} _LL1142: goto _LL1143; _LL1139: _temp1158=
_temp1146; goto _LL1141; _LL1141: if(({ struct Cyc_Absyn_Tunionfield* _temp1166=
_temp1158; if( _temp1166 == 0){ _throw( Null_Exception);} _temp1166->tvs;}) != 0){
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp1167=( char*)"malloc with existential types not yet implemented";
struct _tagged_string _temp1168; _temp1168.curr= _temp1167; _temp1168.base=
_temp1167; _temp1168.last_plus_one= _temp1167 + 50; _temp1168;}));} goto _LL1137;
_LL1143: goto _LL1137; _LL1137:;}{ void*(* ptr_maker)( void* t, void* rgn,
struct Cyc_Absyn_Tqual* tq)= Cyc_Absyn_at_typ; if( topt != 0){ void* _temp1170=
Cyc_Tcutil_compress(({ void** _temp1169= topt; if( _temp1169 == 0){ _throw(
Null_Exception);}* _temp1169;})); struct Cyc_Absyn_PtrInfo _temp1176; struct Cyc_Absyn_Conref*
_temp1178; struct Cyc_Absyn_Tqual* _temp1180; struct Cyc_Absyn_Conref* _temp1182;
void* _temp1184; void* _temp1186; _LL1172: if(( unsigned int) _temp1170 > 4u?((
struct _tunion_struct*) _temp1170)->tag == Cyc_Absyn_PointerType_tag: 0){
_LL1177: _temp1176=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1170)->f1; _LL1187: _temp1186=( void*) _temp1176.elt_typ; goto _LL1185;
_LL1185: _temp1184=( void*) _temp1176.rgn_typ; goto _LL1183; _LL1183: _temp1182=(
struct Cyc_Absyn_Conref*) _temp1176.nullable; goto _LL1181; _LL1181: _temp1180=(
struct Cyc_Absyn_Tqual*) _temp1176.tq; goto _LL1179; _LL1179: _temp1178=( struct
Cyc_Absyn_Conref*) _temp1176.bounds; goto _LL1173;} else{ goto _LL1174;} _LL1174:
goto _LL1175; _LL1173: if((( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1182)){ ptr_maker= Cyc_Absyn_star_typ;} goto _LL1171; _LL1175: goto _LL1171;
_LL1171:;} return ptr_maker( t, rgn, Cyc_Absyn_empty_tqual());}} static void*
Cyc_Tcexp_tcStmtExp( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, struct Cyc_Absyn_Stmt* s){ Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser(
te, s), s, 1); Cyc_NewControlFlow_set_encloser( s, Cyc_Tcenv_get_encloser( te));
while( 1) { void* _temp1188=( void*) s->r; struct Cyc_Absyn_Exp* _temp1198;
struct Cyc_Absyn_Stmt* _temp1200; struct Cyc_Absyn_Stmt* _temp1202; struct Cyc_Absyn_Stmt*
_temp1204; struct Cyc_Absyn_Decl* _temp1206; _LL1190: if(( unsigned int)
_temp1188 > 1u?(( struct _tunion_struct*) _temp1188)->tag == Cyc_Absyn_Exp_s_tag:
0){ _LL1199: _temp1198=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp1188)->f1; goto _LL1191;} else{ goto _LL1192;} _LL1192: if(( unsigned int)
_temp1188 > 1u?(( struct _tunion_struct*) _temp1188)->tag == Cyc_Absyn_Seq_s_tag:
0){ _LL1203: _temp1202=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp1188)->f1; goto _LL1201; _LL1201: _temp1200=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp1188)->f2; goto _LL1193;} else{ goto
_LL1194;} _LL1194: if(( unsigned int) _temp1188 > 1u?(( struct _tunion_struct*)
_temp1188)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL1207: _temp1206=( struct Cyc_Absyn_Decl*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1188)->f1; goto _LL1205; _LL1205:
_temp1204=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1188)->f2;
goto _LL1195;} else{ goto _LL1196;} _LL1196: goto _LL1197; _LL1191: return( void*)({
struct Cyc_Core_Opt* _temp1208= _temp1198->topt; if( _temp1208 == 0){ _throw(
Null_Exception);} _temp1208->v;}); _LL1193: s= _temp1200; continue; _LL1195: s=
_temp1204; continue; _LL1197: return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_string)({ char* _temp1209=( char*)"statement expression must end with expression";
struct _tagged_string _temp1210; _temp1210.curr= _temp1209; _temp1210.base=
_temp1209; _temp1210.last_plus_one= _temp1209 + 46; _temp1210;})); _LL1189:;}}
static void* Cyc_Tcexp_tcCodegen( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Fndecl* fd){ return Cyc_Tcexp_expr_err( te,
loc,( struct _tagged_string)({ char* _temp1211=( char*)"tcCodegen"; struct
_tagged_string _temp1212; _temp1212.curr= _temp1211; _temp1212.base= _temp1211;
_temp1212.last_plus_one= _temp1211 + 10; _temp1212;}));} static void* Cyc_Tcexp_tcFill(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e){ return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({
char* _temp1213=( char*)"tcFill"; struct _tagged_string _temp1214; _temp1214.curr=
_temp1213; _temp1214.base= _temp1213; _temp1214.last_plus_one= _temp1213 + 7;
_temp1214;}));} static void* Cyc_Tcexp_tcNew( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* rgn_handle, struct
Cyc_Absyn_Exp* e, struct Cyc_Absyn_Exp* e1){ void* rgn=( void*) Cyc_Absyn_HeapRgn;
if( rgn_handle != 0){ void* expected_type=( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp1215=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp1215[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp1216; _temp1216.tag= Cyc_Absyn_RgnHandleType_tag; _temp1216.f1=( void*) Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind); _temp1216;}); _temp1215;}); void* handle_type= Cyc_Tcexp_tcExp(
te,( void**)& expected_type,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1217= rgn_handle; if( _temp1217 == 0){ _throw( Null_Exception);} _temp1217;}));
void* _temp1218= Cyc_Tcutil_compress( handle_type); void* _temp1224; _LL1220:
if(( unsigned int) _temp1218 > 4u?(( struct _tunion_struct*) _temp1218)->tag ==
Cyc_Absyn_RgnHandleType_tag: 0){ _LL1225: _temp1224=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1218)->f1; goto _LL1221;} else{ goto _LL1222;} _LL1222: goto _LL1223;
_LL1221: rgn= _temp1224; Cyc_Tcenv_check_rgn_accessible( te, loc, rgn); goto
_LL1219; _LL1223: Cyc_Tcutil_terr(({ struct Cyc_Absyn_Exp* _temp1226= rgn_handle;
if( _temp1226 == 0){ _throw( Null_Exception);} _temp1226->loc;}),({ struct
_tagged_string _temp1227= Cyc_Absynpp_typ2string( handle_type); xprintf("expecting region_t type but found %.*s",
_temp1227.last_plus_one - _temp1227.curr, _temp1227.curr);})); goto _LL1219;
_LL1219:;}{ void* _temp1228=( void*) e1->r; struct Cyc_Absyn_Exp* _temp1240;
struct Cyc_Absyn_Exp* _temp1242; struct Cyc_Absyn_Vardecl* _temp1244; struct Cyc_List_List*
_temp1246; struct Cyc_Core_Opt* _temp1248; struct Cyc_List_List* _temp1250; void*
_temp1252; struct _tagged_string _temp1254; _LL1230: if((( struct _tunion_struct*)
_temp1228)->tag == Cyc_Absyn_Comprehension_e_tag){ _LL1245: _temp1244=( struct
Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1228)->f1;
goto _LL1243; _LL1243: _temp1242=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1228)->f2; goto _LL1241; _LL1241: _temp1240=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1228)->f3; goto _LL1231;} else{
goto _LL1232;} _LL1232: if((( struct _tunion_struct*) _temp1228)->tag == Cyc_Absyn_UnresolvedMem_e_tag){
_LL1249: _temp1248=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1228)->f1; goto _LL1247; _LL1247: _temp1246=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp1228)->f2; goto _LL1233;} else{
goto _LL1234;} _LL1234: if((( struct _tunion_struct*) _temp1228)->tag == Cyc_Absyn_Array_e_tag){
_LL1251: _temp1250=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp1228)->f1; goto _LL1235;} else{ goto _LL1236;} _LL1236: if((( struct
_tunion_struct*) _temp1228)->tag == Cyc_Absyn_Const_e_tag){ _LL1253: _temp1252=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1228)->f1; if(( unsigned int)
_temp1252 > 1u?(( struct _tunion_struct*) _temp1252)->tag == Cyc_Absyn_String_c_tag:
0){ _LL1255: _temp1254=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp1252)->f1; goto _LL1237;} else{ goto _LL1238;}} else{ goto _LL1238;}
_LL1238: goto _LL1239; _LL1231: { void* t_arr= Cyc_Tcexp_tcExpNoPromote( te,
topt, e1); void* _temp1256= Cyc_Tcutil_compress( t_arr); struct Cyc_Absyn_Exp*
_temp1262; struct Cyc_Absyn_Tqual* _temp1264; void* _temp1266; _LL1258: if((
unsigned int) _temp1256 > 4u?(( struct _tunion_struct*) _temp1256)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL1267: _temp1266=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1256)->f1;
goto _LL1265; _LL1265: _temp1264=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1256)->f2; goto _LL1263; _LL1263: _temp1262=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1256)->f3; goto _LL1259;} else{ goto
_LL1260;} _LL1260: goto _LL1261; _LL1259: { void* b= Cyc_Tcutil_is_const_exp( te,
_temp1242)?( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1268=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1268[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1269; _temp1269.tag= Cyc_Absyn_Upper_b_tag;
_temp1269.f1= _temp1242; _temp1269;}); _temp1268;}):( void*) Cyc_Absyn_Unknown_b;
void* res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1270=( struct
Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1270[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1271; _temp1271.tag=
Cyc_Absyn_PointerType_tag; _temp1271.f1=({ struct Cyc_Absyn_PtrInfo _temp1272;
_temp1272.elt_typ=( void*) _temp1266; _temp1272.rgn_typ=( void*) rgn; _temp1272.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp1272.tq=
_temp1264; _temp1272.bounds=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
b); _temp1272;}); _temp1271;}); _temp1270;}); if( topt != 0){ if( ! Cyc_Tcutil_unify(({
void** _temp1273= topt; if( _temp1273 == 0){ _throw( Null_Exception);}*
_temp1273;}), res_typ)? Cyc_Tcutil_silent_castable( te, loc, res_typ,({ void**
_temp1274= topt; if( _temp1274 == 0){ _throw( Null_Exception);}* _temp1274;})):
0){ e->topt=({ struct Cyc_Core_Opt* _temp1275=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1275->v=( void*) res_typ; _temp1275;}); Cyc_Tcutil_unchecked_cast(
te, e,({ void** _temp1276= topt; if( _temp1276 == 0){ _throw( Null_Exception);}*
_temp1276;})); res_typ=({ void** _temp1277= topt; if( _temp1277 == 0){ _throw(
Null_Exception);}* _temp1277;});}} return res_typ;} _LL1261: return(( void*(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp1278=( char*)"tcNew: comprehension returned non-array type"; struct
_tagged_string _temp1279; _temp1279.curr= _temp1278; _temp1279.base= _temp1278;
_temp1279.last_plus_one= _temp1278 + 45; _temp1279;})); _LL1257:;} _LL1233:(
void*)( e1->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct* _temp1280=(
struct Cyc_Absyn_Array_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp1280[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp1281; _temp1281.tag= Cyc_Absyn_Array_e_tag;
_temp1281.f1= _temp1246; _temp1281;}); _temp1280;}))); _temp1250= _temp1246;
goto _LL1235; _LL1235: { void** elt_typ_opt= 0; if( topt != 0){ void* _temp1283=
Cyc_Tcutil_compress(({ void** _temp1282= topt; if( _temp1282 == 0){ _throw(
Null_Exception);}* _temp1282;})); struct Cyc_Absyn_PtrInfo _temp1289; struct Cyc_Absyn_Conref*
_temp1291; struct Cyc_Absyn_Tqual* _temp1293; struct Cyc_Absyn_Conref* _temp1295;
void* _temp1297; void* _temp1299; void** _temp1301; _LL1285: if(( unsigned int)
_temp1283 > 4u?(( struct _tunion_struct*) _temp1283)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL1290: _temp1289=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1283)->f1; _LL1300: _temp1299=( void*) _temp1289.elt_typ; _temp1301=&(((
struct Cyc_Absyn_PointerType_struct*) _temp1283)->f1).elt_typ; goto _LL1298;
_LL1298: _temp1297=( void*) _temp1289.rgn_typ; goto _LL1296; _LL1296: _temp1295=(
struct Cyc_Absyn_Conref*) _temp1289.nullable; goto _LL1294; _LL1294: _temp1293=(
struct Cyc_Absyn_Tqual*) _temp1289.tq; goto _LL1292; _LL1292: _temp1291=( struct
Cyc_Absyn_Conref*) _temp1289.bounds; goto _LL1286;} else{ goto _LL1287;} _LL1287:
goto _LL1288; _LL1286: elt_typ_opt=( void**) _temp1301; goto _LL1284; _LL1288:
goto _LL1284; _LL1284:;}{ void* t_array= Cyc_Tcexp_tcExpNoPromote( te,
elt_typ_opt, e1); void* res_typ;{ void* _temp1302= Cyc_Tcutil_compress( t_array);
struct Cyc_Absyn_Exp* _temp1308; struct Cyc_Absyn_Tqual* _temp1310; void*
_temp1312; _LL1304: if(( unsigned int) _temp1302 > 4u?(( struct _tunion_struct*)
_temp1302)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL1313: _temp1312=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1302)->f1; goto _LL1311; _LL1311:
_temp1310=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1302)->f2; goto _LL1309; _LL1309: _temp1308=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1302)->f3; goto _LL1305;} else{ goto
_LL1306;} _LL1306: goto _LL1307; _LL1305: res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp1314=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1314[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1315; _temp1315.tag=
Cyc_Absyn_PointerType_tag; _temp1315.f1=({ struct Cyc_Absyn_PtrInfo _temp1316;
_temp1316.elt_typ=( void*) _temp1312; _temp1316.rgn_typ=( void*) rgn; _temp1316.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp1316.tq=
_temp1310; _temp1316.bounds=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1317=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1317[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1318; _temp1318.tag= Cyc_Absyn_Upper_b_tag;
_temp1318.f1=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1319=
_temp1308; if( _temp1319 == 0){ _throw( Null_Exception);} _temp1319;});
_temp1318;}); _temp1317;})); _temp1316;}); _temp1315;}); _temp1314;}); if( topt
!= 0){ if( ! Cyc_Tcutil_unify(({ void** _temp1320= topt; if( _temp1320 == 0){
_throw( Null_Exception);}* _temp1320;}), res_typ)? Cyc_Tcutil_silent_castable(
te, loc, res_typ,({ void** _temp1321= topt; if( _temp1321 == 0){ _throw(
Null_Exception);}* _temp1321;})): 0){ e->topt=({ struct Cyc_Core_Opt* _temp1322=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1322->v=(
void*) res_typ; _temp1322;}); Cyc_Tcutil_unchecked_cast( te, e,({ void**
_temp1323= topt; if( _temp1323 == 0){ _throw( Null_Exception);}* _temp1323;}));
res_typ=({ void** _temp1324= topt; if( _temp1324 == 0){ _throw( Null_Exception);}*
_temp1324;});}} goto _LL1303; _LL1307: return(( void*(*)( struct _tagged_string))
Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp1325=( char*)"tcExpNoPromote on Array_e returned non-array type";
struct _tagged_string _temp1326; _temp1326.curr= _temp1325; _temp1326.base=
_temp1325; _temp1326.last_plus_one= _temp1325 + 50; _temp1326;})); _LL1303:;}
return res_typ;}} _LL1237: { void* topt2= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t,
rgn, Cyc_Absyn_empty_tqual(),( void*) Cyc_Absyn_Unknown_b); void* t= Cyc_Tcexp_tcExp(
te,( void**)& topt2, e1); return Cyc_Absyn_atb_typ( t, rgn, Cyc_Absyn_empty_tqual(),(
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1327=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1327[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1328; _temp1328.tag= Cyc_Absyn_Upper_b_tag;
_temp1328.f1= Cyc_Absyn_uint_exp( 1, 0); _temp1328;}); _temp1327;}));} _LL1239: {
void** topt2= 0; if( topt != 0){ void* _temp1330= Cyc_Tcutil_compress(({ void**
_temp1329= topt; if( _temp1329 == 0){ _throw( Null_Exception);}* _temp1329;}));
struct Cyc_Absyn_PtrInfo _temp1338; struct Cyc_Absyn_Conref* _temp1340; struct
Cyc_Absyn_Tqual* _temp1342; struct Cyc_Absyn_Conref* _temp1344; void* _temp1346;
void* _temp1348; void** _temp1350; struct Cyc_Absyn_TunionInfo _temp1351;
_LL1332: if(( unsigned int) _temp1330 > 4u?(( struct _tunion_struct*) _temp1330)->tag
== Cyc_Absyn_PointerType_tag: 0){ _LL1339: _temp1338=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1330)->f1; _LL1349: _temp1348=( void*)
_temp1338.elt_typ; _temp1350=&((( struct Cyc_Absyn_PointerType_struct*)
_temp1330)->f1).elt_typ; goto _LL1347; _LL1347: _temp1346=( void*) _temp1338.rgn_typ;
goto _LL1345; _LL1345: _temp1344=( struct Cyc_Absyn_Conref*) _temp1338.nullable;
goto _LL1343; _LL1343: _temp1342=( struct Cyc_Absyn_Tqual*) _temp1338.tq; goto
_LL1341; _LL1341: _temp1340=( struct Cyc_Absyn_Conref*) _temp1338.bounds; goto
_LL1333;} else{ goto _LL1334;} _LL1334: if(( unsigned int) _temp1330 > 4u?((
struct _tunion_struct*) _temp1330)->tag == Cyc_Absyn_TunionType_tag: 0){ _LL1352:
_temp1351=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp1330)->f1; goto _LL1335;} else{ goto _LL1336;} _LL1336: goto _LL1337;
_LL1333: topt2=( void**) _temp1350; goto _LL1331; _LL1335: topt2=({ void**
_temp1353=( void**) GC_malloc( sizeof( void*)); _temp1353[ 0]=({ void**
_temp1354= topt; if( _temp1354 == 0){ _throw( Null_Exception);}* _temp1354;});
_temp1353;}); goto _LL1331; _LL1337: goto _LL1331; _LL1331:;}{ void* telt= Cyc_Tcexp_tcExp(
te, topt2, e1); void* res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp1355=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1355[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1356; _temp1356.tag=
Cyc_Absyn_PointerType_tag; _temp1356.f1=({ struct Cyc_Absyn_PtrInfo _temp1357;
_temp1357.elt_typ=( void*) telt; _temp1357.rgn_typ=( void*) rgn; _temp1357.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp1357.tq= Cyc_Absyn_empty_tqual();
_temp1357.bounds=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1358=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1358[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1359; _temp1359.tag= Cyc_Absyn_Upper_b_tag;
_temp1359.f1= Cyc_Absyn_uint_exp( 1, 0); _temp1359;}); _temp1358;})); _temp1357;});
_temp1356;}); _temp1355;}); if( topt != 0){ if( ! Cyc_Tcutil_unify(({ void**
_temp1360= topt; if( _temp1360 == 0){ _throw( Null_Exception);}* _temp1360;}),
res_typ)? Cyc_Tcutil_silent_castable( te, loc, res_typ,({ void** _temp1361= topt;
if( _temp1361 == 0){ _throw( Null_Exception);}* _temp1361;})): 0){ e->topt=({
struct Cyc_Core_Opt* _temp1362=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1362->v=( void*) res_typ; _temp1362;}); Cyc_Tcutil_unchecked_cast(
te, e,({ void** _temp1363= topt; if( _temp1363 == 0){ _throw( Null_Exception);}*
_temp1363;})); res_typ=({ void** _temp1364= topt; if( _temp1364 == 0){ _throw(
Null_Exception);}* _temp1364;});}} return res_typ;}} _LL1229:;}} void* Cyc_Tcexp_tcExp(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcexp_tcExpNoPromote(
te, topt, e); t= Cyc_Tcutil_compress( t);{ void* _temp1365= t; struct Cyc_Absyn_Exp*
_temp1371; struct Cyc_Absyn_Tqual* _temp1373; void* _temp1375; _LL1367: if((
unsigned int) _temp1365 > 4u?(( struct _tunion_struct*) _temp1365)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL1376: _temp1375=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1365)->f1;
goto _LL1374; _LL1374: _temp1373=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1365)->f2; goto _LL1372; _LL1372: _temp1371=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1365)->f3; goto _LL1368;} else{ goto
_LL1369;} _LL1369: goto _LL1370; _LL1368: { void* _temp1379; int _temp1381;
struct _tuple5 _temp1377= Cyc_Tcutil_addressof_props( te, e); _LL1382: _temp1381=
_temp1377.f1; goto _LL1380; _LL1380: _temp1379= _temp1377.f2; goto _LL1378;
_LL1378: { void* b= _temp1371 == 0?( void*) Cyc_Absyn_Unknown_b:( void*)({
struct Cyc_Absyn_Upper_b_struct* _temp1383=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1383[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1384; _temp1384.tag= Cyc_Absyn_Upper_b_tag;
_temp1384.f1=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1385=
_temp1371; if( _temp1385 == 0){ _throw( Null_Exception);} _temp1385;});
_temp1384;}); _temp1383;}); t= Cyc_Absyn_atb_typ( _temp1375, _temp1379,
_temp1373, b); Cyc_Tcutil_unchecked_cast( te, e, t); return t;}} _LL1370: return
t; _LL1366:;}} void* Cyc_Tcexp_tcExpInitializer( struct Cyc_Tcenv_Tenv* te, void**
topt, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcexp_tcExpNoPromote( te, topt, e);
void* _temp1386=( void*) e->r; struct Cyc_List_List* _temp1396; struct Cyc_Absyn_Exp*
_temp1398; struct Cyc_Absyn_Exp* _temp1400; struct Cyc_Absyn_Vardecl* _temp1402;
void* _temp1404; struct _tagged_string _temp1406; _LL1388: if((( struct
_tunion_struct*) _temp1386)->tag == Cyc_Absyn_Array_e_tag){ _LL1397: _temp1396=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1386)->f1; goto
_LL1389;} else{ goto _LL1390;} _LL1390: if((( struct _tunion_struct*) _temp1386)->tag
== Cyc_Absyn_Comprehension_e_tag){ _LL1403: _temp1402=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1386)->f1; goto _LL1401; _LL1401:
_temp1400=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1386)->f2; goto _LL1399; _LL1399: _temp1398=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1386)->f3; goto _LL1391;} else{
goto _LL1392;} _LL1392: if((( struct _tunion_struct*) _temp1386)->tag == Cyc_Absyn_Const_e_tag){
_LL1405: _temp1404=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1386)->f1;
if(( unsigned int) _temp1404 > 1u?(( struct _tunion_struct*) _temp1404)->tag ==
Cyc_Absyn_String_c_tag: 0){ _LL1407: _temp1406=( struct _tagged_string)(( struct
Cyc_Absyn_String_c_struct*) _temp1404)->f1; goto _LL1393;} else{ goto _LL1394;}}
else{ goto _LL1394;} _LL1394: goto _LL1395; _LL1389: return t; _LL1391: return t;
_LL1393: return t; _LL1395: t= Cyc_Tcutil_compress( t);{ void* _temp1408= t;
struct Cyc_Absyn_Exp* _temp1414; struct Cyc_Absyn_Tqual* _temp1416; void*
_temp1418; _LL1410: if(( unsigned int) _temp1408 > 4u?(( struct _tunion_struct*)
_temp1408)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL1419: _temp1418=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1408)->f1; goto _LL1417; _LL1417:
_temp1416=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1408)->f2; goto _LL1415; _LL1415: _temp1414=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1408)->f3; goto _LL1411;} else{ goto
_LL1412;} _LL1412: goto _LL1413; _LL1411: { void* _temp1422; int _temp1424;
struct _tuple5 _temp1420= Cyc_Tcutil_addressof_props( te, e); _LL1425: _temp1424=
_temp1420.f1; goto _LL1423; _LL1423: _temp1422= _temp1420.f2; goto _LL1421;
_LL1421: { void* b= _temp1414 == 0?( void*) Cyc_Absyn_Unknown_b:( void*)({
struct Cyc_Absyn_Upper_b_struct* _temp1426=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1426[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1427; _temp1427.tag= Cyc_Absyn_Upper_b_tag;
_temp1427.f1=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1428=
_temp1414; if( _temp1428 == 0){ _throw( Null_Exception);} _temp1428;});
_temp1427;}); _temp1426;}); t= Cyc_Absyn_atb_typ( _temp1418, _temp1422,
_temp1416, b); Cyc_Tcutil_unchecked_cast( te, e, t); return t;}} _LL1413: return
t; _LL1409:;} _LL1387:;} static void* Cyc_Tcexp_tcExpNoPromote( struct Cyc_Tcenv_Tenv*
te, void** topt, struct Cyc_Absyn_Exp* e){{ void* _temp1429=( void*) e->r;
struct Cyc_Absyn_Exp* _temp1435; _LL1431: if((( struct _tunion_struct*)
_temp1429)->tag == Cyc_Absyn_NoInstantiate_e_tag){ _LL1436: _temp1435=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*) _temp1429)->f1; goto
_LL1432;} else{ goto _LL1433;} _LL1433: goto _LL1434; _LL1432: Cyc_Tcexp_tcExpNoInst(
te, topt, _temp1435); e->topt= _temp1435->topt; goto _LL1430; _LL1434: Cyc_Tcexp_tcExpNoInst(
te, topt, e);( void*)({ struct Cyc_Core_Opt* _temp1437= e->topt; if( _temp1437
== 0){ _throw( Null_Exception);} _temp1437->v=( void*) Cyc_Absyn_pointer_expand(
Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp1438= e->topt; if(
_temp1438 == 0){ _throw( Null_Exception);} _temp1438->v;})));});{ void*
_temp1440= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp1439= e->topt;
if( _temp1439 == 0){ _throw( Null_Exception);} _temp1439->v;})); struct Cyc_Absyn_PtrInfo
_temp1446; struct Cyc_Absyn_Conref* _temp1448; struct Cyc_Absyn_Tqual* _temp1450;
struct Cyc_Absyn_Conref* _temp1452; void* _temp1454; void* _temp1456; _LL1442:
if(( unsigned int) _temp1440 > 4u?(( struct _tunion_struct*) _temp1440)->tag ==
Cyc_Absyn_PointerType_tag: 0){ _LL1447: _temp1446=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1440)->f1; _LL1457: _temp1456=( void*)
_temp1446.elt_typ; goto _LL1455; _LL1455: _temp1454=( void*) _temp1446.rgn_typ;
goto _LL1453; _LL1453: _temp1452=( struct Cyc_Absyn_Conref*) _temp1446.nullable;
goto _LL1451; _LL1451: _temp1450=( struct Cyc_Absyn_Tqual*) _temp1446.tq; goto
_LL1449; _LL1449: _temp1448=( struct Cyc_Absyn_Conref*) _temp1446.bounds; goto
_LL1443;} else{ goto _LL1444;} _LL1444: goto _LL1445; _LL1443:{ void* _temp1458=
Cyc_Tcutil_compress( _temp1456); struct Cyc_Absyn_FnInfo _temp1464; struct Cyc_List_List*
_temp1466; int _temp1468; struct Cyc_List_List* _temp1470; void* _temp1472;
struct Cyc_Core_Opt* _temp1474; struct Cyc_List_List* _temp1476; _LL1460: if((
unsigned int) _temp1458 > 4u?(( struct _tunion_struct*) _temp1458)->tag == Cyc_Absyn_FnType_tag:
0){ _LL1465: _temp1464=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp1458)->f1; _LL1477: _temp1476=( struct Cyc_List_List*) _temp1464.tvars;
goto _LL1475; _LL1475: _temp1474=( struct Cyc_Core_Opt*) _temp1464.effect; goto
_LL1473; _LL1473: _temp1472=( void*) _temp1464.ret_typ; goto _LL1471; _LL1471:
_temp1470=( struct Cyc_List_List*) _temp1464.args; goto _LL1469; _LL1469:
_temp1468=( int) _temp1464.varargs; goto _LL1467; _LL1467: _temp1466=( struct
Cyc_List_List*) _temp1464.attributes; goto _LL1461;} else{ goto _LL1462;}
_LL1462: goto _LL1463; _LL1461: if( _temp1476 != 0){ struct Cyc_List_List* inst=((
struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_Absyn_Tvar*), struct
Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_make_inst_var, _temp1476); struct
Cyc_List_List* ts=(( struct Cyc_List_List*(*)( void*(* f)( struct _tuple4*),
struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct _tuple4*)) Cyc_Core_snd,
inst); void* ftyp= Cyc_Tcutil_substitute( inst,( void*)({ struct Cyc_Absyn_FnType_struct*
_temp1478=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp1478[ 0]=({ struct Cyc_Absyn_FnType_struct _temp1479; _temp1479.tag= Cyc_Absyn_FnType_tag;
_temp1479.f1=({ struct Cyc_Absyn_FnInfo _temp1480; _temp1480.tvars= 0; _temp1480.effect=
_temp1474; _temp1480.ret_typ=( void*) _temp1472; _temp1480.args= _temp1470;
_temp1480.varargs= _temp1468; _temp1480.attributes= _temp1466; _temp1480;});
_temp1479;}); _temp1478;})); void* new_typ=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp1481=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1481[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1482; _temp1482.tag=
Cyc_Absyn_PointerType_tag; _temp1482.f1=({ struct Cyc_Absyn_PtrInfo _temp1483;
_temp1483.elt_typ=( void*) ftyp; _temp1483.rgn_typ=( void*) _temp1454; _temp1483.nullable=
_temp1452; _temp1483.tq= _temp1450; _temp1483.bounds= _temp1448; _temp1483;});
_temp1482;}); _temp1481;}); struct Cyc_Absyn_Exp* inner= Cyc_Absyn_copy_exp( e);(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Instantiate_e_struct*
_temp1484=( struct Cyc_Absyn_Instantiate_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Instantiate_e_struct)); _temp1484[ 0]=({ struct Cyc_Absyn_Instantiate_e_struct
_temp1485; _temp1485.tag= Cyc_Absyn_Instantiate_e_tag; _temp1485.f1= inner;
_temp1485.f2= ts; _temp1485;}); _temp1484;}))); e->topt=({ struct Cyc_Core_Opt*
_temp1486=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1486->v=( void*) new_typ; _temp1486;});} goto _LL1459; _LL1463: goto
_LL1459; _LL1459:;} goto _LL1441; _LL1445: goto _LL1441; _LL1441:;} goto _LL1430;
_LL1430:;} return( void*)({ struct Cyc_Core_Opt* _temp1487= e->topt; if(
_temp1487 == 0){ _throw( Null_Exception);} _temp1487->v;});} static void Cyc_Tcexp_tcExpNoInst(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){ struct Cyc_Position_Segment*
loc= e->loc; void* t;{ void* _temp1488=( void*) e->r; struct Cyc_Absyn_Exp*
_temp1560; struct _tuple1* _temp1562; struct Cyc_List_List* _temp1564; struct
Cyc_Absyn_Exp* _temp1566; struct Cyc_List_List* _temp1568; struct Cyc_Core_Opt*
_temp1570; void* _temp1572; void* _temp1574; struct _tuple1* _temp1576; struct
Cyc_List_List* _temp1578; void* _temp1580; void* _temp1582; struct Cyc_Absyn_Exp*
_temp1584; struct Cyc_Absyn_Exp* _temp1586; struct Cyc_Core_Opt* _temp1588;
struct Cyc_Absyn_Exp* _temp1590; struct Cyc_Absyn_Exp* _temp1592; struct Cyc_Absyn_Exp*
_temp1594; struct Cyc_Absyn_Exp* _temp1596; struct Cyc_Absyn_Exp* _temp1598;
struct Cyc_Absyn_Exp* _temp1600; struct Cyc_List_List* _temp1602; struct Cyc_Absyn_Exp*
_temp1604; struct Cyc_Absyn_Exp* _temp1606; struct Cyc_List_List* _temp1608;
struct Cyc_Absyn_Exp* _temp1610; struct Cyc_Absyn_Exp* _temp1612; void*
_temp1614; struct Cyc_Absyn_Exp* _temp1616; struct Cyc_Absyn_Exp* _temp1618;
struct Cyc_Absyn_Exp* _temp1620; struct Cyc_Absyn_Exp* _temp1622; void*
_temp1624; struct Cyc_Absyn_Exp* _temp1626; struct _tagged_string* _temp1628;
struct Cyc_Absyn_Exp* _temp1630; struct _tagged_string* _temp1632; struct Cyc_Absyn_Exp*
_temp1634; struct Cyc_Absyn_Exp* _temp1636; struct Cyc_Absyn_Exp* _temp1638;
struct Cyc_List_List* _temp1640; struct Cyc_List_List* _temp1642; struct _tuple2*
_temp1644; struct Cyc_List_List* _temp1646; struct Cyc_Absyn_Stmt* _temp1648;
struct Cyc_Absyn_Fndecl* _temp1650; struct Cyc_Absyn_Exp* _temp1652; struct Cyc_Absyn_Exp*
_temp1654; struct Cyc_Absyn_Exp* _temp1656; struct Cyc_Absyn_Vardecl* _temp1658;
struct Cyc_Absyn_Structdecl* _temp1660; struct Cyc_Absyn_Structdecl** _temp1662;
struct Cyc_List_List* _temp1663; struct Cyc_Core_Opt* _temp1665; struct Cyc_Core_Opt**
_temp1667; struct _tuple1* _temp1668; struct _tuple1** _temp1670; struct Cyc_Absyn_Tunionfield*
_temp1671; struct Cyc_Absyn_Tuniondecl* _temp1673; struct Cyc_List_List*
_temp1675; struct Cyc_Core_Opt* _temp1677; struct Cyc_Core_Opt** _temp1679;
struct Cyc_Core_Opt* _temp1680; struct Cyc_Core_Opt** _temp1682; struct Cyc_Absyn_Tunionfield*
_temp1683; struct Cyc_Absyn_XTuniondecl* _temp1685; struct Cyc_List_List*
_temp1687; struct Cyc_Core_Opt* _temp1689; struct Cyc_Core_Opt** _temp1691;
struct Cyc_Absyn_Enumfield* _temp1692; struct Cyc_Absyn_Enumdecl* _temp1694;
struct _tuple1* _temp1696; void* _temp1698; struct Cyc_Absyn_Exp* _temp1700;
_LL1490: if((( struct _tunion_struct*) _temp1488)->tag == Cyc_Absyn_NoInstantiate_e_tag){
_LL1561: _temp1560=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1488)->f1; goto _LL1491;} else{ goto _LL1492;} _LL1492: if((( struct
_tunion_struct*) _temp1488)->tag == Cyc_Absyn_UnknownId_e_tag){ _LL1563:
_temp1562=( struct _tuple1*)(( struct Cyc_Absyn_UnknownId_e_struct*) _temp1488)->f1;
goto _LL1493;} else{ goto _LL1494;} _LL1494: if((( struct _tunion_struct*)
_temp1488)->tag == Cyc_Absyn_UnknownCall_e_tag){ _LL1567: _temp1566=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_UnknownCall_e_struct*) _temp1488)->f1; goto _LL1565; _LL1565:
_temp1564=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1488)->f2; goto _LL1495;} else{ goto _LL1496;} _LL1496: if((( struct
_tunion_struct*) _temp1488)->tag == Cyc_Absyn_UnresolvedMem_e_tag){ _LL1571:
_temp1570=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1488)->f1; goto _LL1569; _LL1569: _temp1568=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp1488)->f2; goto _LL1497;} else{
goto _LL1498;} _LL1498: if((( struct _tunion_struct*) _temp1488)->tag == Cyc_Absyn_Const_e_tag){
_LL1573: _temp1572=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1488)->f1;
goto _LL1499;} else{ goto _LL1500;} _LL1500: if((( struct _tunion_struct*)
_temp1488)->tag == Cyc_Absyn_Var_e_tag){ _LL1577: _temp1576=( struct _tuple1*)((
struct Cyc_Absyn_Var_e_struct*) _temp1488)->f1; goto _LL1575; _LL1575: _temp1574=(
void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1488)->f2; goto _LL1501;} else{
goto _LL1502;} _LL1502: if((( struct _tunion_struct*) _temp1488)->tag == Cyc_Absyn_Primop_e_tag){
_LL1581: _temp1580=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1488)->f1;
goto _LL1579; _LL1579: _temp1578=( struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp1488)->f2; goto _LL1503;} else{ goto _LL1504;} _LL1504: if((( struct
_tunion_struct*) _temp1488)->tag == Cyc_Absyn_Increment_e_tag){ _LL1585:
_temp1584=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1488)->f1; goto _LL1583; _LL1583: _temp1582=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1488)->f2; goto _LL1505;} else{ goto _LL1506;} _LL1506: if((( struct
_tunion_struct*) _temp1488)->tag == Cyc_Absyn_AssignOp_e_tag){ _LL1591:
_temp1590=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1488)->f1; goto _LL1589; _LL1589: _temp1588=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1488)->f2; goto _LL1587; _LL1587:
_temp1586=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1488)->f3; goto _LL1507;} else{ goto _LL1508;} _LL1508: if((( struct
_tunion_struct*) _temp1488)->tag == Cyc_Absyn_Conditional_e_tag){ _LL1597:
_temp1596=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1488)->f1; goto _LL1595; _LL1595: _temp1594=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp1488)->f2; goto _LL1593; _LL1593:
_temp1592=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1488)->f3; goto _LL1509;} else{ goto _LL1510;} _LL1510: if((( struct
_tunion_struct*) _temp1488)->tag == Cyc_Absyn_SeqExp_e_tag){ _LL1601: _temp1600=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*) _temp1488)->f1; goto
_LL1599; _LL1599: _temp1598=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1488)->f2; goto _LL1511;} else{ goto _LL1512;} _LL1512: if((( struct
_tunion_struct*) _temp1488)->tag == Cyc_Absyn_FnCall_e_tag){ _LL1605: _temp1604=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*) _temp1488)->f1; goto
_LL1603; _LL1603: _temp1602=( struct Cyc_List_List*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1488)->f2; goto _LL1513;} else{ goto _LL1514;} _LL1514: if((( struct
_tunion_struct*) _temp1488)->tag == Cyc_Absyn_Throw_e_tag){ _LL1607: _temp1606=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*) _temp1488)->f1; goto
_LL1515;} else{ goto _LL1516;} _LL1516: if((( struct _tunion_struct*) _temp1488)->tag
== Cyc_Absyn_Instantiate_e_tag){ _LL1611: _temp1610=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Instantiate_e_struct*) _temp1488)->f1; goto _LL1609; _LL1609:
_temp1608=( struct Cyc_List_List*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1488)->f2; goto _LL1517;} else{ goto _LL1518;} _LL1518: if((( struct
_tunion_struct*) _temp1488)->tag == Cyc_Absyn_Cast_e_tag){ _LL1615: _temp1614=(
void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp1488)->f1; goto _LL1613; _LL1613:
_temp1612=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*) _temp1488)->f2;
goto _LL1519;} else{ goto _LL1520;} _LL1520: if((( struct _tunion_struct*)
_temp1488)->tag == Cyc_Absyn_Address_e_tag){ _LL1617: _temp1616=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Address_e_struct*) _temp1488)->f1; goto _LL1521;} else{ goto
_LL1522;} _LL1522: if((( struct _tunion_struct*) _temp1488)->tag == Cyc_Absyn_New_e_tag){
_LL1621: _temp1620=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*)
_temp1488)->f1; goto _LL1619; _LL1619: _temp1618=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_New_e_struct*) _temp1488)->f2; goto _LL1523;} else{ goto
_LL1524;} _LL1524: if((( struct _tunion_struct*) _temp1488)->tag == Cyc_Absyn_Sizeofexp_e_tag){
_LL1623: _temp1622=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1488)->f1; goto _LL1525;} else{ goto _LL1526;} _LL1526: if((( struct
_tunion_struct*) _temp1488)->tag == Cyc_Absyn_Sizeoftyp_e_tag){ _LL1625:
_temp1624=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp1488)->f1; goto
_LL1527;} else{ goto _LL1528;} _LL1528: if((( struct _tunion_struct*) _temp1488)->tag
== Cyc_Absyn_Deref_e_tag){ _LL1627: _temp1626=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Deref_e_struct*) _temp1488)->f1; goto _LL1529;} else{ goto _LL1530;}
_LL1530: if((( struct _tunion_struct*) _temp1488)->tag == Cyc_Absyn_StructMember_e_tag){
_LL1631: _temp1630=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1488)->f1; goto _LL1629; _LL1629: _temp1628=( struct _tagged_string*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp1488)->f2; goto _LL1531;} else{
goto _LL1532;} _LL1532: if((( struct _tunion_struct*) _temp1488)->tag == Cyc_Absyn_StructArrow_e_tag){
_LL1635: _temp1634=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1488)->f1; goto _LL1633; _LL1633: _temp1632=( struct _tagged_string*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp1488)->f2; goto _LL1533;} else{
goto _LL1534;} _LL1534: if((( struct _tunion_struct*) _temp1488)->tag == Cyc_Absyn_Subscript_e_tag){
_LL1639: _temp1638=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1488)->f1; goto _LL1637; _LL1637: _temp1636=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp1488)->f2; goto _LL1535;} else{ goto
_LL1536;} _LL1536: if((( struct _tunion_struct*) _temp1488)->tag == Cyc_Absyn_Tuple_e_tag){
_LL1641: _temp1640=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1488)->f1; goto _LL1537;} else{ goto _LL1538;} _LL1538: if((( struct
_tunion_struct*) _temp1488)->tag == Cyc_Absyn_CompoundLit_e_tag){ _LL1645:
_temp1644=( struct _tuple2*)(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp1488)->f1;
goto _LL1643; _LL1643: _temp1642=( struct Cyc_List_List*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1488)->f2; goto _LL1539;} else{ goto _LL1540;} _LL1540: if((( struct
_tunion_struct*) _temp1488)->tag == Cyc_Absyn_Array_e_tag){ _LL1647: _temp1646=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1488)->f1; goto
_LL1541;} else{ goto _LL1542;} _LL1542: if((( struct _tunion_struct*) _temp1488)->tag
== Cyc_Absyn_StmtExp_e_tag){ _LL1649: _temp1648=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_StmtExp_e_struct*) _temp1488)->f1; goto _LL1543;} else{ goto
_LL1544;} _LL1544: if((( struct _tunion_struct*) _temp1488)->tag == Cyc_Absyn_Codegen_e_tag){
_LL1651: _temp1650=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1488)->f1; goto _LL1545;} else{ goto _LL1546;} _LL1546: if((( struct
_tunion_struct*) _temp1488)->tag == Cyc_Absyn_Fill_e_tag){ _LL1653: _temp1652=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*) _temp1488)->f1; goto
_LL1547;} else{ goto _LL1548;} _LL1548: if((( struct _tunion_struct*) _temp1488)->tag
== Cyc_Absyn_Comprehension_e_tag){ _LL1659: _temp1658=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1488)->f1; goto _LL1657; _LL1657:
_temp1656=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1488)->f2; goto _LL1655; _LL1655: _temp1654=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1488)->f3; goto _LL1549;} else{
goto _LL1550;} _LL1550: if((( struct _tunion_struct*) _temp1488)->tag == Cyc_Absyn_Struct_e_tag){
_LL1669: _temp1668=( struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1488)->f1; _temp1670=&(( struct Cyc_Absyn_Struct_e_struct*) _temp1488)->f1;
goto _LL1666; _LL1666: _temp1665=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1488)->f2; _temp1667=&(( struct Cyc_Absyn_Struct_e_struct*) _temp1488)->f2;
goto _LL1664; _LL1664: _temp1663=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1488)->f3; goto _LL1661; _LL1661: _temp1660=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1488)->f4; _temp1662=&(( struct Cyc_Absyn_Struct_e_struct*)
_temp1488)->f4; goto _LL1551;} else{ goto _LL1552;} _LL1552: if((( struct
_tunion_struct*) _temp1488)->tag == Cyc_Absyn_Tunion_e_tag){ _LL1681: _temp1680=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp1488)->f1;
_temp1682=&(( struct Cyc_Absyn_Tunion_e_struct*) _temp1488)->f1; goto _LL1678;
_LL1678: _temp1677=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1488)->f2; _temp1679=&(( struct Cyc_Absyn_Tunion_e_struct*) _temp1488)->f2;
goto _LL1676; _LL1676: _temp1675=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1488)->f3; goto _LL1674; _LL1674: _temp1673=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1488)->f4; goto _LL1672; _LL1672:
_temp1671=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1488)->f5; goto _LL1553;} else{ goto _LL1554;} _LL1554: if((( struct
_tunion_struct*) _temp1488)->tag == Cyc_Absyn_XTunion_e_tag){ _LL1690: _temp1689=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_XTunion_e_struct*) _temp1488)->f1;
_temp1691=&(( struct Cyc_Absyn_XTunion_e_struct*) _temp1488)->f1; goto _LL1688;
_LL1688: _temp1687=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_e_struct*)
_temp1488)->f2; goto _LL1686; _LL1686: _temp1685=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_e_struct*) _temp1488)->f3; goto _LL1684; _LL1684:
_temp1683=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_e_struct*)
_temp1488)->f4; goto _LL1555;} else{ goto _LL1556;} _LL1556: if((( struct
_tunion_struct*) _temp1488)->tag == Cyc_Absyn_Enum_e_tag){ _LL1697: _temp1696=(
struct _tuple1*)(( struct Cyc_Absyn_Enum_e_struct*) _temp1488)->f1; goto _LL1695;
_LL1695: _temp1694=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp1488)->f2; goto _LL1693; _LL1693: _temp1692=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_e_struct*) _temp1488)->f3; goto _LL1557;} else{ goto
_LL1558;} _LL1558: if((( struct _tunion_struct*) _temp1488)->tag == Cyc_Absyn_Malloc_e_tag){
_LL1701: _temp1700=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1488)->f1; goto _LL1699; _LL1699: _temp1698=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1488)->f2; goto _LL1559;} else{ goto _LL1489;} _LL1491: Cyc_Tcexp_tcExpNoInst(
te, 0, _temp1560); return; _LL1493: Cyc_Tcexp_resolve_unknown_id( te, e,
_temp1562); Cyc_Tcexp_tcExpNoInst( te, topt, e); return; _LL1495: Cyc_Tcexp_resolve_unknown_fn(
te, e, _temp1566, _temp1564); Cyc_Tcexp_tcExpNoInst( te, topt, e); return;
_LL1497: Cyc_Tcexp_resolve_unresolved_mem( te, loc, topt, e, _temp1568); Cyc_Tcexp_tcExpNoInst(
te, topt, e); return; _LL1499: t= Cyc_Tcexp_tcConst( te, loc, topt, _temp1572, e);
goto _LL1489; _LL1501: t= Cyc_Tcexp_tcVar( te, loc, _temp1576, _temp1574); goto
_LL1489; _LL1503: t= Cyc_Tcexp_tcPrimop( te, loc, topt, _temp1580, _temp1578);
goto _LL1489; _LL1505: t= Cyc_Tcexp_tcIncrement( te, loc, topt, _temp1584,
_temp1582); goto _LL1489; _LL1507: t= Cyc_Tcexp_tcAssignOp( te, loc, topt,
_temp1590, _temp1588, _temp1586); goto _LL1489; _LL1509: t= Cyc_Tcexp_tcConditional(
te, loc, topt, _temp1596, _temp1594, _temp1592); goto _LL1489; _LL1511: t= Cyc_Tcexp_tcSeqExp(
te, loc, topt, _temp1600, _temp1598); goto _LL1489; _LL1513: t= Cyc_Tcexp_tcFnCall(
te, loc, topt, _temp1604, _temp1602); goto _LL1489; _LL1515: t= Cyc_Tcexp_tcThrow(
te, loc, topt, _temp1606); goto _LL1489; _LL1517: t= Cyc_Tcexp_tcInstantiate( te,
loc, topt, _temp1610, _temp1608); goto _LL1489; _LL1519: t= Cyc_Tcexp_tcCast( te,
loc, topt, _temp1614, _temp1612); goto _LL1489; _LL1521: t= Cyc_Tcexp_tcAddress(
te, loc, topt, _temp1616); goto _LL1489; _LL1523: t= Cyc_Tcexp_tcNew( te, loc,
topt, _temp1620, e, _temp1618); goto _LL1489; _LL1525: { void* t1= Cyc_Tcexp_tcExpNoPromote(
te, 0, _temp1622); t= Cyc_Tcexp_tcSizeof( te, loc, topt, t1); goto _LL1489;}
_LL1527: t= Cyc_Tcexp_tcSizeof( te, loc, topt, _temp1624); goto _LL1489; _LL1529:
t= Cyc_Tcexp_tcDeref( te, loc, topt, _temp1626); goto _LL1489; _LL1531: t= Cyc_Tcexp_tcStructMember(
te, loc, topt, e, _temp1630, _temp1628); goto _LL1489; _LL1533: t= Cyc_Tcexp_tcStructArrow(
te, loc, topt, _temp1634, _temp1632); goto _LL1489; _LL1535: t= Cyc_Tcexp_tcSubscript(
te, loc, topt, _temp1638, _temp1636); goto _LL1489; _LL1537: t= Cyc_Tcexp_tcTuple(
te, loc, topt, _temp1640); goto _LL1489; _LL1539: t= Cyc_Tcexp_tcCompoundLit( te,
loc, topt, _temp1644, _temp1642); goto _LL1489; _LL1541: { void** elt_topt= 0;
if( topt != 0){ void* _temp1703= Cyc_Tcutil_compress(({ void** _temp1702= topt;
if( _temp1702 == 0){ _throw( Null_Exception);}* _temp1702;})); struct Cyc_Absyn_Exp*
_temp1709; struct Cyc_Absyn_Tqual* _temp1711; void* _temp1713; void** _temp1715;
_LL1705: if(( unsigned int) _temp1703 > 4u?(( struct _tunion_struct*) _temp1703)->tag
== Cyc_Absyn_ArrayType_tag: 0){ _LL1714: _temp1713=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1703)->f1; _temp1715=&(( struct Cyc_Absyn_ArrayType_struct*) _temp1703)->f1;
goto _LL1712; _LL1712: _temp1711=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1703)->f2; goto _LL1710; _LL1710: _temp1709=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1703)->f3; goto _LL1706;} else{ goto
_LL1707;} _LL1707: goto _LL1708; _LL1706: elt_topt=( void**) _temp1715; goto
_LL1704; _LL1708: goto _LL1704; _LL1704:;} t= Cyc_Tcexp_tcArray( te, loc,
elt_topt, _temp1646); goto _LL1489;} _LL1543: t= Cyc_Tcexp_tcStmtExp( te, loc,
topt, _temp1648); goto _LL1489; _LL1545: t= Cyc_Tcexp_tcCodegen( te, loc, topt,
_temp1650); goto _LL1489; _LL1547: t= Cyc_Tcexp_tcFill( te, loc, topt, _temp1652);
goto _LL1489; _LL1549: t= Cyc_Tcexp_tcComprehension( te, loc, topt, _temp1658,
_temp1656, _temp1654); goto _LL1489; _LL1551: t= Cyc_Tcexp_tcStruct( te, loc,
topt, _temp1670, _temp1667, _temp1663, _temp1662); goto _LL1489; _LL1553: t= Cyc_Tcexp_tcTunion(
te, loc, topt, e, _temp1682, _temp1679, _temp1675, _temp1673, _temp1671); goto
_LL1489; _LL1555: t= Cyc_Tcexp_tcXTunion( te, loc, topt, _temp1691, _temp1687,
_temp1685, _temp1683); goto _LL1489; _LL1557: t=( void*)({ struct Cyc_Absyn_EnumType_struct*
_temp1716=( struct Cyc_Absyn_EnumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp1716[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp1717; _temp1717.tag= Cyc_Absyn_EnumType_tag;
_temp1717.f1=({ struct Cyc_Absyn_Enumdecl* _temp1718= _temp1694; if( _temp1718
== 0){ _throw( Null_Exception);} _temp1718->name;}); _temp1717.f2= _temp1694;
_temp1717;}); _temp1716;}); goto _LL1489; _LL1559: t= Cyc_Tcexp_tcMalloc( te,
loc, topt, _temp1700, _temp1698); goto _LL1489; _LL1489:;} e->topt=({ struct Cyc_Core_Opt*
_temp1719=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1719->v=( void*) t; _temp1719;});}