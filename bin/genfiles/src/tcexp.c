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
struct Cyc_Core_Opt* Cyc_Core_opt_t; struct _tuple0{ void* f1; void* f2; } ;
extern void* Cyc_Core_snd( struct _tuple0*); extern char Cyc_Core_InvalidArg_tag[
11u]; struct Cyc_Core_InvalidArg_struct{ char* tag; struct _tagged_string f1; }
; extern char Cyc_Core_Failure_tag[ 8u]; struct Cyc_Core_Failure_struct{ char*
tag; struct _tagged_string f1; } ; extern char Cyc_Core_Impossible_tag[ 11u];
struct Cyc_Core_Impossible_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Core_Not_found_tag[ 10u]; struct Cyc_Core_Not_found_struct{ char*
tag; } ; extern char Cyc_Core_Unreachable_tag[ 12u]; struct Cyc_Core_Unreachable_struct{
char* tag; struct _tagged_string f1; } ; extern char* string_to_Cstring( struct
_tagged_string); extern char* underlying_Cstring( struct _tagged_string); extern
struct _tagged_string Cstring_to_string( char*); extern int system( char*);
struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List*
Cyc_List_glist_t; typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct
Cyc_List_List* Cyc_List_List_t; extern int Cyc_List_length( struct Cyc_List_List*
x); extern char Cyc_List_List_empty_tag[ 11u]; struct Cyc_List_List_empty_struct{
char* tag; } ; extern struct Cyc_List_List* Cyc_List_map( void*(* f)( void*),
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_rmap_c( struct
_RegionHandle*, void*(* f)( void*, void*), void* env, struct Cyc_List_List* x);
extern char Cyc_List_List_mismatch_tag[ 14u]; struct Cyc_List_List_mismatch_struct{
char* tag; } ; extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List*
x); extern struct Cyc_List_List* Cyc_List_rappend( struct _RegionHandle*, struct
Cyc_List_List* x, struct Cyc_List_List* y); extern char Cyc_List_Nth_tag[ 4u];
struct Cyc_List_Nth_struct{ char* tag; } ; extern void* Cyc_List_nth( struct Cyc_List_List*
x, int i); extern struct Cyc_List_List* Cyc_List_zip( struct Cyc_List_List* x,
struct Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_rzip( struct
_RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List*
y); struct Cyc_Lineno_Pos{ struct _tagged_string logical_file; struct
_tagged_string line; int line_no; int col; } ; typedef struct Cyc_Lineno_Pos*
Cyc_Lineno_pos_t; extern char Cyc_Position_Exit_tag[ 5u]; struct Cyc_Position_Exit_struct{
char* tag; } ; struct Cyc_Position_Segment; typedef struct Cyc_Position_Segment*
Cyc_Position_seg_t; typedef void* Cyc_Position_Error_kind; static const
unsigned int Cyc_Position_Lex= 0; static const unsigned int Cyc_Position_Parse=
1; static const unsigned int Cyc_Position_Elab= 2; typedef void* Cyc_Position_error_kind_t;
struct Cyc_Position_Error{ struct _tagged_string source; struct Cyc_Position_Segment*
seg; void* kind; struct _tagged_string desc; } ; typedef struct Cyc_Position_Error*
Cyc_Position_error_t; extern char Cyc_Position_Nocontext_tag[ 10u]; struct Cyc_Position_Nocontext_struct{
char* tag; } ; typedef struct _tagged_string* Cyc_Absyn_field_name_t; typedef
struct _tagged_string* Cyc_Absyn_var_t; typedef struct _tagged_string* Cyc_Absyn_tvarname_t;
typedef void* Cyc_Absyn_Nmspace; typedef void* Cyc_Absyn_nmspace_t; struct
_tuple1{ void* f1; struct _tagged_string* f2; } ; typedef struct _tuple1* Cyc_Absyn_qvar_t;
typedef struct _tuple1* Cyc_Absyn_qvar_opt_t; typedef struct _tuple1* Cyc_Absyn_typedef_name_t;
typedef struct _tuple1* Cyc_Absyn_typedef_name_opt_t; struct Cyc_Absyn_Tvar;
typedef void* Cyc_Absyn_Scope; struct Cyc_Absyn_Tqual; typedef void* Cyc_Absyn_Size_of;
typedef void* Cyc_Absyn_Kind; typedef void* Cyc_Absyn_Array_kind; typedef void*
Cyc_Absyn_Sign; struct Cyc_Absyn_Conref; typedef void* Cyc_Absyn_Constraint;
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
8; struct Cyc_Absyn_ArrayType_struct{ int tag; void* f1; struct Cyc_Absyn_Tqual
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
23; struct _tuple2{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; void* f3;
} ; struct Cyc_Absyn_CompoundLit_e_struct{ int tag; struct _tuple2* f1; struct
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
f3; } ; static const int Cyc_Absyn_While_s_tag= 4; struct _tuple3{ struct Cyc_Absyn_Exp*
f1; struct Cyc_Absyn_Stmt* f2; } ; struct Cyc_Absyn_While_s_struct{ int tag;
struct _tuple3 f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Break_s_tag=
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
Cyc_Absyn_Vardecl{ void* sc; struct _tuple1* name; struct Cyc_Absyn_Tqual tq;
void* type; struct Cyc_Absyn_Exp* initializer; struct Cyc_Core_Opt* rgn; struct
Cyc_List_List* attributes; } ; struct Cyc_Absyn_Fndecl{ void* sc; int is_inline;
struct _tuple1* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt* effect;
void* ret_type; struct Cyc_List_List* args; int varargs; struct Cyc_Absyn_Stmt*
body; struct Cyc_Core_Opt* cached_typ; struct Cyc_Core_Opt* param_vardecls;
struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Structfield{ struct
_tagged_string* name; struct Cyc_Absyn_Tqual tq; void* type; struct Cyc_Core_Opt*
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
char* tag; } ; extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(); extern
struct Cyc_Absyn_Conref* Cyc_Absyn_new_conref( void* x); extern struct Cyc_Absyn_Conref*
Cyc_Absyn_empty_conref(); extern void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref*
x); extern void* Cyc_Absyn_new_evar( void*); extern void* Cyc_Absyn_wildtyp();
extern void* Cyc_Absyn_uchar_t; extern void* Cyc_Absyn_ushort_t; extern void*
Cyc_Absyn_uint_t; extern void* Cyc_Absyn_ulong_t; extern void* Cyc_Absyn_schar_t;
extern void* Cyc_Absyn_sshort_t; extern void* Cyc_Absyn_sint_t; extern void* Cyc_Absyn_slong_t;
extern void* Cyc_Absyn_float_t; extern void* Cyc_Absyn_exn_typ; extern void* Cyc_Absyn_string_typ(
void* rgn); extern void* Cyc_Absyn_file_typ(); extern void* Cyc_Absyn_atb_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual tq, void* b); extern void* Cyc_Absyn_star_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual tq); extern void* Cyc_Absyn_at_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual tq); extern struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp(
void*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_copy_exp(
struct Cyc_Absyn_Exp*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp(
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
extern void* Cyc_Tcutil_rsubstitute( struct _RegionHandle*, struct Cyc_List_List*,
void*); extern void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl*); struct
_tuple4{ struct Cyc_Absyn_Tvar* f1; void* f2; } ; extern struct _tuple4* Cyc_Tcutil_r_make_inst_var(
struct _RegionHandle*, struct Cyc_Absyn_Tvar*); extern void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*); extern void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*, struct Cyc_List_List* bound_tvars, void* k, void*);
extern void Cyc_Tcutil_check_nonzero_bound( struct Cyc_Position_Segment*, struct
Cyc_Absyn_Conref*); extern void Cyc_Tcutil_check_bound( struct Cyc_Position_Segment*,
unsigned int i, struct Cyc_Absyn_Conref*); extern struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* des, struct Cyc_Absyn_Structdecl*
sd); extern int Cyc_Tcutil_is_tagged_pointer_typ( void*); struct _tuple5{ int f1;
void* f2; } ; extern struct _tuple5 Cyc_Tcutil_addressof_props( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e); extern int Cyc_Tcutil_is_const_exp( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); extern unsigned int Cyc_Evexp_szof( void* t, struct
Cyc_Position_Segment* loc); extern void Cyc_Tcstmt_tcStmt( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Stmt* s, int new_block); extern struct Cyc_List_List* Cyc_Formatstr_get_format_typs(
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
_temp48;}))); goto _LL43; _LL47:( void) _throw( _temp42); _LL43:;}}} struct
_tuple6{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ; static struct
_tuple6* Cyc_Tcexp_make_struct_arg( struct Cyc_Absyn_Exp* e){ return({ struct
_tuple6* _temp50=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6)); _temp50->f1=
0; _temp50->f2= e; _temp50;});} static void Cyc_Tcexp_resolve_unknown_fn( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Exp* e1, struct
Cyc_List_List* es){ void* _temp51=( void*) e1->r; struct _tuple1* _temp57; _LL53:
if((( struct _tunion_struct*) _temp51)->tag == Cyc_Absyn_UnknownId_e_tag){ _LL58:
_temp57=( struct _tuple1*)(( struct Cyc_Absyn_UnknownId_e_struct*) _temp51)->f1;
goto _LL54;} else{ goto _LL55;} _LL55: goto _LL56; _LL54: { struct _handler_cons
_temp59; _push_handler(& _temp59);{ struct _xtunion_struct* _temp60=( struct
_xtunion_struct*) setjmp( _temp59.handler); if( ! _temp60){{ void* _temp61= Cyc_Tcenv_lookup_ordinary(
te, e1->loc, _temp57); void* _temp73; struct Cyc_Absyn_Tunionfield* _temp75;
struct Cyc_Absyn_Tuniondecl* _temp77; struct Cyc_Absyn_Tunionfield* _temp79;
struct Cyc_Absyn_XTuniondecl* _temp81; struct Cyc_Absyn_Structdecl* _temp83;
struct Cyc_Absyn_Enumfield* _temp85; struct Cyc_Absyn_Enumdecl* _temp87; _LL63:
if((( struct _tunion_struct*) _temp61)->tag == Cyc_Tcenv_VarRes_tag){ _LL74:
_temp73=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp61)->f1; goto _LL64;}
else{ goto _LL65;} _LL65: if((( struct _tunion_struct*) _temp61)->tag == Cyc_Tcenv_TunionRes_tag){
_LL78: _temp77=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Tcenv_TunionRes_struct*)
_temp61)->f1; goto _LL76; _LL76: _temp75=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Tcenv_TunionRes_struct*) _temp61)->f2; goto _LL66;} else{ goto _LL67;}
_LL67: if((( struct _tunion_struct*) _temp61)->tag == Cyc_Tcenv_XTunionRes_tag){
_LL82: _temp81=( struct Cyc_Absyn_XTuniondecl*)(( struct Cyc_Tcenv_XTunionRes_struct*)
_temp61)->f1; goto _LL80; _LL80: _temp79=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Tcenv_XTunionRes_struct*) _temp61)->f2; goto _LL68;} else{ goto _LL69;}
_LL69: if((( struct _tunion_struct*) _temp61)->tag == Cyc_Tcenv_StructRes_tag){
_LL84: _temp83=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Tcenv_StructRes_struct*)
_temp61)->f1; goto _LL70;} else{ goto _LL71;} _LL71: if((( struct _tunion_struct*)
_temp61)->tag == Cyc_Tcenv_EnumRes_tag){ _LL88: _temp87=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Tcenv_EnumRes_struct*) _temp61)->f1; goto _LL86; _LL86: _temp85=(
struct Cyc_Absyn_Enumfield*)(( struct Cyc_Tcenv_EnumRes_struct*) _temp61)->f2;
goto _LL72;} else{ goto _LL62;} _LL64:( void*)( e->r=( void*)(( void*)({ struct
Cyc_Absyn_FnCall_e_struct* _temp89=( struct Cyc_Absyn_FnCall_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct)); _temp89[ 0]=({ struct Cyc_Absyn_FnCall_e_struct
_temp90; _temp90.tag= Cyc_Absyn_FnCall_e_tag; _temp90.f1= e1; _temp90.f2= es;
_temp90;}); _temp89;}))); _npop_handler( 0u); return; _LL66: if( _temp75->typs
== 0){ Cyc_Tcutil_terr( e->loc,({ struct _tagged_string _temp91= Cyc_Absynpp_qvar2string(
_temp75->name); xprintf("%.*s is a constant, not a function", _temp91.last_plus_one
- _temp91.curr, _temp91.curr);}));}( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Tunion_e_struct*
_temp92=( struct Cyc_Absyn_Tunion_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_e_struct));
_temp92[ 0]=({ struct Cyc_Absyn_Tunion_e_struct _temp93; _temp93.tag= Cyc_Absyn_Tunion_e_tag;
_temp93.f1= 0; _temp93.f2= 0; _temp93.f3= es; _temp93.f4= _temp77; _temp93.f5=
_temp75; _temp93;}); _temp92;}))); _npop_handler( 0u); return; _LL68: if(
_temp79->typs == 0){ Cyc_Tcutil_terr( e->loc,({ struct _tagged_string _temp94=
Cyc_Absynpp_qvar2string( _temp79->name); xprintf("%.*s is a constant, not a function",
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
_temp119= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Exp* _temp127; struct Cyc_Absyn_Tqual
_temp129; void* _temp131; struct Cyc_Absyn_Structdecl** _temp133; struct Cyc_List_List*
_temp135; struct _tuple1* _temp137; _LL121: if(( unsigned int) _temp119 > 4u?((
struct _tunion_struct*) _temp119)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL132:
_temp131=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp119)->f1; goto
_LL130; _LL130: _temp129=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
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
Null_Exception);}* _temp225;})); struct Cyc_Absyn_Exp* _temp234; struct Cyc_Absyn_Tqual
_temp236; void* _temp238; struct Cyc_Absyn_PtrInfo _temp240; struct Cyc_Absyn_Conref*
_temp242; struct Cyc_Absyn_Tqual _temp244; struct Cyc_Absyn_Conref* _temp246;
void* _temp248; void* _temp250; _LL228: if(( unsigned int) _temp226 > 4u?((
struct _tunion_struct*) _temp226)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL239:
_temp238=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp226)->f1; goto
_LL237; _LL237: _temp236=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp226)->f2; goto _LL235; _LL235: _temp234=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp226)->f3; goto _LL229;} else{ goto _LL230;}
_LL230: if(( unsigned int) _temp226 > 4u?(( struct _tunion_struct*) _temp226)->tag
== Cyc_Absyn_PointerType_tag: 0){ _LL241: _temp240=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp226)->f1; _LL251: _temp250=( void*)
_temp240.elt_typ; goto _LL249; _LL249: _temp248=( void*) _temp240.rgn_typ; goto
_LL247; _LL247: _temp246=( struct Cyc_Absyn_Conref*) _temp240.nullable; goto
_LL245; _LL245: _temp244=( struct Cyc_Absyn_Tqual) _temp240.tq; goto _LL243;
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
_LL342:{ void* _temp350= t; struct Cyc_Absyn_Exp* _temp358; struct Cyc_Absyn_Tqual
_temp360; void* _temp362; struct Cyc_Absyn_PtrInfo _temp364; struct Cyc_Absyn_Conref*
_temp366; struct Cyc_Absyn_Tqual _temp368; struct Cyc_Absyn_Conref* _temp370;
void* _temp372; void* _temp374; _LL352: if(( unsigned int) _temp350 > 4u?((
struct _tunion_struct*) _temp350)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL363:
_temp362=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp350)->f1; goto
_LL361; _LL361: _temp360=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp350)->f2; goto _LL359; _LL359: _temp358=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp350)->f3; goto _LL353;} else{ goto _LL354;}
_LL354: if(( unsigned int) _temp350 > 4u?(( struct _tunion_struct*) _temp350)->tag
== Cyc_Absyn_PointerType_tag: 0){ _LL365: _temp364=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp350)->f1; _LL375: _temp374=( void*)
_temp364.elt_typ; goto _LL373; _LL373: _temp372=( void*) _temp364.rgn_typ; goto
_LL371; _LL371: _temp370=( struct Cyc_Absyn_Conref*) _temp364.nullable; goto
_LL369; _LL369: _temp368=( struct Cyc_Absyn_Tqual) _temp364.tq; goto _LL367;
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
_temp408; struct Cyc_Absyn_Tqual _temp410; struct Cyc_Absyn_Conref* _temp412;
void* _temp414; void* _temp416; void* _temp418; struct Cyc_Absyn_PtrInfo
_temp420; struct Cyc_Absyn_Conref* _temp422; struct Cyc_Absyn_Tqual _temp424;
struct Cyc_Absyn_Conref* _temp426; void* _temp428; void* _temp430; _LL400:
_LL419: _temp418= _temp398.f1; if(( unsigned int) _temp418 > 4u?(( struct
_tunion_struct*) _temp418)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL421:
_temp420=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp418)->f1; _LL431: _temp430=( void*) _temp420.elt_typ; goto _LL429; _LL429:
_temp428=( void*) _temp420.rgn_typ; goto _LL427; _LL427: _temp426=( struct Cyc_Absyn_Conref*)
_temp420.nullable; goto _LL425; _LL425: _temp424=( struct Cyc_Absyn_Tqual)
_temp420.tq; goto _LL423; _LL423: _temp422=( struct Cyc_Absyn_Conref*) _temp420.bounds;
goto _LL405;} else{ goto _LL402;} _LL405: _temp404= _temp398.f2; if((
unsigned int) _temp404 > 4u?(( struct _tunion_struct*) _temp404)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL407: _temp406=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp404)->f1; _LL417: _temp416=( void*) _temp406.elt_typ; goto _LL415; _LL415:
_temp414=( void*) _temp406.rgn_typ; goto _LL413; _LL413: _temp412=( struct Cyc_Absyn_Conref*)
_temp406.nullable; goto _LL411; _LL411: _temp410=( struct Cyc_Absyn_Tqual)
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
return t;}} struct _tuple7{ struct Cyc_Absyn_Tqual f1; void* f2; } ; static void
Cyc_Tcexp_check_writable( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){{
void* _temp550=( void*) e->r; void* _temp570; struct Cyc_Absyn_Vardecl* _temp572;
struct _tuple1* _temp574; void* _temp576; struct Cyc_Absyn_Vardecl* _temp578;
struct _tuple1* _temp580; void* _temp582; struct Cyc_Absyn_Vardecl* _temp584;
struct _tuple1* _temp586; void* _temp588; struct Cyc_Absyn_Vardecl* _temp590;
struct _tuple1* _temp592; struct Cyc_Absyn_Exp* _temp594; struct Cyc_Absyn_Exp*
_temp596; struct _tagged_string* _temp598; struct Cyc_Absyn_Exp* _temp600;
struct _tagged_string* _temp602; struct Cyc_Absyn_Exp* _temp604; struct Cyc_Absyn_Exp*
_temp606; _LL552: if((( struct _tunion_struct*) _temp550)->tag == Cyc_Absyn_Var_e_tag){
_LL575: _temp574=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp550)->f1;
goto _LL571; _LL571: _temp570=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp550)->f2; if(( unsigned int) _temp570 > 1u?(( struct _tunion_struct*)
_temp570)->tag == Cyc_Absyn_Param_b_tag: 0){ _LL573: _temp572=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Param_b_struct*) _temp570)->f1; goto _LL553;} else{ goto _LL554;}}
else{ goto _LL554;} _LL554: if((( struct _tunion_struct*) _temp550)->tag == Cyc_Absyn_Var_e_tag){
_LL581: _temp580=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp550)->f1;
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
_temp590= _temp584; goto _LL559; _LL559: if( !( _temp590->tq).q_const){ return;}
goto _LL551; _LL561:{ void* _temp609= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp608= _temp596->topt; if( _temp608 == 0){ _throw( Null_Exception);} _temp608->v;}));
struct Cyc_Absyn_PtrInfo _temp619; struct Cyc_Absyn_Conref* _temp621; struct Cyc_Absyn_Tqual
_temp623; struct Cyc_Absyn_Conref* _temp625; void* _temp627; void* _temp629;
struct Cyc_Absyn_Exp* _temp631; struct Cyc_Absyn_Tqual _temp633; void* _temp635;
struct Cyc_List_List* _temp637; _LL611: if(( unsigned int) _temp609 > 4u?((
struct _tunion_struct*) _temp609)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL620:
_temp619=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp609)->f1; _LL630: _temp629=( void*) _temp619.elt_typ; goto _LL628; _LL628:
_temp627=( void*) _temp619.rgn_typ; goto _LL626; _LL626: _temp625=( struct Cyc_Absyn_Conref*)
_temp619.nullable; goto _LL624; _LL624: _temp623=( struct Cyc_Absyn_Tqual)
_temp619.tq; goto _LL622; _LL622: _temp621=( struct Cyc_Absyn_Conref*) _temp619.bounds;
goto _LL612;} else{ goto _LL613;} _LL613: if(( unsigned int) _temp609 > 4u?((
struct _tunion_struct*) _temp609)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL636:
_temp635=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp609)->f1; goto
_LL634; _LL634: _temp633=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp609)->f2; goto _LL632; _LL632: _temp631=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp609)->f3; goto _LL614;} else{ goto _LL615;}
_LL615: if(( unsigned int) _temp609 > 4u?(( struct _tunion_struct*) _temp609)->tag
== Cyc_Absyn_TupleType_tag: 0){ _LL638: _temp637=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp609)->f1; goto _LL616;} else{ goto
_LL617;} _LL617: goto _LL618; _LL612: _temp633= _temp623; goto _LL614; _LL614:
if( ! _temp633.q_const){ return;} goto _LL610; _LL616: { unsigned int i= Cyc_Evexp_eval_const_uint_exp(
_temp594);{ struct _handler_cons _temp639; _push_handler(& _temp639);{ struct
_xtunion_struct* _temp640=( struct _xtunion_struct*) setjmp( _temp639.handler);
if( ! _temp640){{ struct _tuple7 _temp643; void* _temp644; struct Cyc_Absyn_Tqual
_temp646; struct _tuple7* _temp641=(( struct _tuple7*(*)( struct Cyc_List_List*
x, int i)) Cyc_List_nth)( _temp637,( int) i); _temp643=* _temp641; _LL647:
_temp646= _temp643.f1; goto _LL645; _LL645: _temp644= _temp643.f2; goto _LL642;
_LL642: if( ! _temp646.q_const){ _npop_handler( 0u); return;}}; _pop_handler();}
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
_temp668= sf; if( _temp668 == 0){ _throw( Null_Exception);} _temp668->tq;}).q_const){
return;} goto _LL657;} _LL661: goto _LL657; _LL657:;} goto _LL551; _LL565:{ void*
_temp671= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp670= _temp604->topt;
if( _temp670 == 0){ _throw( Null_Exception);} _temp670->v;})); struct Cyc_Absyn_PtrInfo
_temp677; struct Cyc_Absyn_Conref* _temp679; struct Cyc_Absyn_Tqual _temp681;
struct Cyc_Absyn_Conref* _temp683; void* _temp685; void* _temp687; _LL673: if((
unsigned int) _temp671 > 4u?(( struct _tunion_struct*) _temp671)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL678: _temp677=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp671)->f1; _LL688: _temp687=( void*) _temp677.elt_typ; goto _LL686; _LL686:
_temp685=( void*) _temp677.rgn_typ; goto _LL684; _LL684: _temp683=( struct Cyc_Absyn_Conref*)
_temp677.nullable; goto _LL682; _LL682: _temp681=( struct Cyc_Absyn_Tqual)
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
_temp701= sf; if( _temp701 == 0){ _throw( Null_Exception);} _temp701->tq;}).q_const){
return;} goto _LL690;} _LL694: goto _LL690; _LL690:;} goto _LL672; _LL676: goto
_LL672; _LL672:;} goto _LL551; _LL567:{ void* _temp704= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp703= _temp606->topt; if( _temp703 == 0){
_throw( Null_Exception);} _temp703->v;})); struct Cyc_Absyn_PtrInfo _temp712;
struct Cyc_Absyn_Conref* _temp714; struct Cyc_Absyn_Tqual _temp716; struct Cyc_Absyn_Conref*
_temp718; void* _temp720; void* _temp722; struct Cyc_Absyn_Exp* _temp724; struct
Cyc_Absyn_Tqual _temp726; void* _temp728; _LL706: if(( unsigned int) _temp704 >
4u?(( struct _tunion_struct*) _temp704)->tag == Cyc_Absyn_PointerType_tag: 0){
_LL713: _temp712=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp704)->f1; _LL723: _temp722=( void*) _temp712.elt_typ; goto _LL721; _LL721:
_temp720=( void*) _temp712.rgn_typ; goto _LL719; _LL719: _temp718=( struct Cyc_Absyn_Conref*)
_temp712.nullable; goto _LL717; _LL717: _temp716=( struct Cyc_Absyn_Tqual)
_temp712.tq; goto _LL715; _LL715: _temp714=( struct Cyc_Absyn_Conref*) _temp712.bounds;
goto _LL707;} else{ goto _LL708;} _LL708: if(( unsigned int) _temp704 > 4u?((
struct _tunion_struct*) _temp704)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL729:
_temp728=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp704)->f1; goto
_LL727; _LL727: _temp726=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp704)->f2; goto _LL725; _LL725: _temp724=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp704)->f3; goto _LL709;} else{ goto _LL710;}
_LL710: goto _LL711; _LL707: _temp726= _temp716; goto _LL709; _LL709: if( !
_temp726.q_const){ return;} goto _LL705; _LL711: goto _LL705; _LL705:;} goto
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
Cyc_Absyn_Tqual _temp753; void* _temp755; _LL747: if(( unsigned int) _temp745 >
4u?(( struct _tunion_struct*) _temp745)->tag == Cyc_Absyn_ArrayType_tag: 0){
_LL756: _temp755=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp745)->f1;
goto _LL754; _LL754: _temp753=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
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
_temp777; struct Cyc_Absyn_Tqual _temp779; struct Cyc_Absyn_Conref* _temp781;
void* _temp783; void* _temp785; _LL771: if(( unsigned int) _temp769 > 4u?((
struct _tunion_struct*) _temp769)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL776:
_temp775=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp769)->f1; _LL786: _temp785=( void*) _temp775.elt_typ; goto _LL784; _LL784:
_temp783=( void*) _temp775.rgn_typ; goto _LL782; _LL782: _temp781=( struct Cyc_Absyn_Conref*)
_temp775.nullable; goto _LL780; _LL780: _temp779=( struct Cyc_Absyn_Tqual)
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
_temp837; struct Cyc_Absyn_Conref* _temp839; struct Cyc_Absyn_Tqual _temp841;
struct Cyc_Absyn_Conref* _temp843; void* _temp845; void* _temp847; _LL833: if((
unsigned int) _temp831 > 4u?(( struct _tunion_struct*) _temp831)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL838: _temp837=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp831)->f1; _LL848: _temp847=( void*) _temp837.elt_typ; goto _LL846; _LL846:
_temp845=( void*) _temp837.rgn_typ; goto _LL844; _LL844: _temp843=( struct Cyc_Absyn_Conref*)
_temp837.nullable; goto _LL842; _LL842: _temp841=( struct Cyc_Absyn_Tqual)
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
Cyc_Absyn_Exp* e){ void** topt2= 0; struct Cyc_Absyn_Tqual tq2= Cyc_Absyn_empty_tqual();
if( topt != 0){ void* _temp891= Cyc_Tcutil_compress(({ void** _temp890= topt;
if( _temp890 == 0){ _throw( Null_Exception);}* _temp890;})); struct Cyc_Absyn_PtrInfo
_temp897; struct Cyc_Absyn_Conref* _temp899; struct Cyc_Absyn_Tqual _temp901;
struct Cyc_Absyn_Conref* _temp903; void* _temp905; void* _temp907; _LL893: if((
unsigned int) _temp891 > 4u?(( struct _tunion_struct*) _temp891)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL898: _temp897=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp891)->f1; _LL908: _temp907=( void*) _temp897.elt_typ; goto _LL906; _LL906:
_temp905=( void*) _temp897.rgn_typ; goto _LL904; _LL904: _temp903=( struct Cyc_Absyn_Conref*)
_temp897.nullable; goto _LL902; _LL902: _temp901=( struct Cyc_Absyn_Tqual)
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
t, void* rgn, struct Cyc_Absyn_Tqual tq)= Cyc_Absyn_at_typ; if( topt != 0){ void*
_temp931= Cyc_Tcutil_compress(({ void** _temp930= topt; if( _temp930 == 0){
_throw( Null_Exception);}* _temp930;})); struct Cyc_Absyn_PtrInfo _temp937;
struct Cyc_Absyn_Conref* _temp939; struct Cyc_Absyn_Tqual _temp941; struct Cyc_Absyn_Conref*
_temp943; void* _temp945; void* _temp947; _LL933: if(( unsigned int) _temp931 >
4u?(( struct _tunion_struct*) _temp931)->tag == Cyc_Absyn_PointerType_tag: 0){
_LL938: _temp937=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp931)->f1; _LL948: _temp947=( void*) _temp937.elt_typ; goto _LL946; _LL946:
_temp945=( void*) _temp937.rgn_typ; goto _LL944; _LL944: _temp943=( struct Cyc_Absyn_Conref*)
_temp937.nullable; goto _LL942; _LL942: _temp941=( struct Cyc_Absyn_Tqual)
_temp937.tq; goto _LL940; _LL940: _temp939=( struct Cyc_Absyn_Conref*) _temp937.bounds;
goto _LL934;} else{ goto _LL935;} _LL935: goto _LL936; _LL934: if((( int(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp943)){ ptr_maker= Cyc_Absyn_star_typ;}
goto _LL932; _LL936: goto _LL932; _LL932:;} return ptr_maker(( void*)({ struct
Cyc_Core_Opt* _temp949= e->topt; if( _temp949 == 0){ _throw( Null_Exception);}
_temp949->v;}),( void*) Cyc_Absyn_HeapRgn, tq2);} _LL917: goto _LL911; _LL911:;}{
void* _temp952; int _temp954; struct _tuple5 _temp950= Cyc_Tcutil_addressof_props(
te, e); _LL955: _temp954= _temp950.f1; goto _LL953; _LL953: _temp952= _temp950.f2;
goto _LL951; _LL951: { struct Cyc_Absyn_Tqual tq= Cyc_Absyn_empty_tqual(); if(
_temp954){ tq.q_const= 1;}{ void* t= Cyc_Absyn_at_typ(( void*)({ struct Cyc_Core_Opt*
_temp956= e->topt; if( _temp956 == 0){ _throw( Null_Exception);} _temp956->v;}),
_temp952, tq); return t;}}}} static void* Cyc_Tcexp_tcSizeof( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, void* t){ Cyc_Tcutil_check_type(
loc, te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind, t); Cyc_Evexp_szof(
t, loc); return Cyc_Absyn_uint_t;} static void* Cyc_Tcexp_tcDeref( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp(
te, 0, e);{ void* t= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp976= e->topt; if( _temp976 == 0){ _throw( Null_Exception);} _temp976->v;}));
void* _temp957= t; struct Cyc_Absyn_PtrInfo _temp963; struct Cyc_Absyn_Conref*
_temp965; struct Cyc_Absyn_Tqual _temp967; struct Cyc_Absyn_Conref* _temp969;
void* _temp971; void* _temp973; _LL959: if(( unsigned int) _temp957 > 4u?((
struct _tunion_struct*) _temp957)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL964:
_temp963=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp957)->f1; _LL974: _temp973=( void*) _temp963.elt_typ; goto _LL972; _LL972:
_temp971=( void*) _temp963.rgn_typ; goto _LL970; _LL970: _temp969=( struct Cyc_Absyn_Conref*)
_temp963.nullable; goto _LL968; _LL968: _temp967=( struct Cyc_Absyn_Tqual)
_temp963.tq; goto _LL966; _LL966: _temp965=( struct Cyc_Absyn_Conref*) _temp963.bounds;
goto _LL960;} else{ goto _LL961;} _LL961: goto _LL962; _LL960: Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp971); Cyc_Tcutil_check_nonzero_bound( loc, _temp965); return
_temp973; _LL962: return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string
_temp975= Cyc_Absynpp_typ2string( t); xprintf("expecting * or @ type but found %.*s",
_temp975.last_plus_one - _temp975.curr, _temp975.curr);})); _LL958:;}} static
void* Cyc_Tcexp_tcStructMember( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* outer_e, struct Cyc_Absyn_Exp* e, struct
_tagged_string* f){ Cyc_Tcexp_tcExpNoPromote( te, 0, e);{ void* t= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp1042= e->topt; if( _temp1042 == 0){ _throw(
Null_Exception);} _temp1042->v;})); void* _temp977= t; struct Cyc_Absyn_Structdecl**
_temp989; struct Cyc_Absyn_Structdecl* _temp991; struct Cyc_List_List* _temp992;
struct _tuple1* _temp994; struct Cyc_Absyn_Uniondecl** _temp996; struct Cyc_Absyn_Uniondecl*
_temp998; struct Cyc_List_List* _temp999; struct _tuple1* _temp1001; struct Cyc_Absyn_Exp*
_temp1004; struct Cyc_Absyn_Tqual _temp1006; void* _temp1008; struct Cyc_Absyn_PtrInfo
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
_LL1007; _LL1007: _temp1006=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
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
- _temp1021.curr, _temp1021.curr);}));}{ void* t2;{ struct _RegionHandle
_temp1022= _new_region(); struct _RegionHandle* rgn=& _temp1022; struct
_handler_cons _temp1023; _push_handler(& _temp1023);{ struct _xtunion_struct*
_temp1024=( struct _xtunion_struct*) setjmp( _temp1023.handler); if( ! _temp1024){{
struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)( struct _RegionHandle* r3,
struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, _temp991->tvs, _temp992); t2= Cyc_Tcutil_rsubstitute( rgn, inst,( void*)({
struct Cyc_Absyn_Structfield* _temp1025= finfo; if( _temp1025 == 0){ _throw(
Null_Exception);} _temp1025->type;}));} _free_region(& _temp1022); _pop_handler();}
else{ _free_region(& _temp1022); _throw( _temp1024);}}} return t2;}} _LL982: if(
_temp1001 == 0){ return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({
char* _temp1026=( char*)"unresolved union type (COMPILER ERROR)"; struct
_tagged_string _temp1027; _temp1027.curr= _temp1026; _temp1027.base= _temp1026;
_temp1027.last_plus_one= _temp1026 + 39; _temp1027;}));}{ struct Cyc_Absyn_Structfield*
finfo= Cyc_Absyn_lookup_union_field( _temp998, f); if( finfo == 0){ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp1029= Cyc_Absynpp_qvar2string(( struct
_tuple1*)({ struct _tuple1* _temp1028= _temp1001; if( _temp1028 == 0){ _throw(
Null_Exception);} _temp1028;})); struct _tagged_string _temp1030=* f; xprintf("union %.*s has no %.*s field",
_temp1029.last_plus_one - _temp1029.curr, _temp1029.curr, _temp1030.last_plus_one
- _temp1030.curr, _temp1030.curr);}));}{ struct Cyc_List_List* inst=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
_temp998->tvs, _temp999); void* t2= Cyc_Tcutil_substitute( inst,( void*)({
struct Cyc_Absyn_Structfield* _temp1031= finfo; if( _temp1031 == 0){ _throw(
Null_Exception);} _temp1031->type;})); return t2;}} _LL984:( void*)( outer_e->r=(
void*)(( void*)({ struct Cyc_Absyn_Primop_e_struct* _temp1032=( struct Cyc_Absyn_Primop_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp1032[ 0]=({ struct
Cyc_Absyn_Primop_e_struct _temp1033; _temp1033.tag= Cyc_Absyn_Primop_e_tag;
_temp1033.f1=( void*)(( void*) Cyc_Absyn_Size); _temp1033.f2=({ struct Cyc_List_List*
_temp1034=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1034->hd=( void*) e; _temp1034->tl= 0; _temp1034;}); _temp1033;});
_temp1032;}))); return Cyc_Absyn_uint_t; _LL986:( void*)( outer_e->r=( void*)((
void*)({ struct Cyc_Absyn_Primop_e_struct* _temp1035=( struct Cyc_Absyn_Primop_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp1035[ 0]=({ struct
Cyc_Absyn_Primop_e_struct _temp1036; _temp1036.tag= Cyc_Absyn_Primop_e_tag;
_temp1036.f1=( void*)(( void*) Cyc_Absyn_Size); _temp1036.f2=({ struct Cyc_List_List*
_temp1037=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1037->hd=( void*) e; _temp1037->tl= 0; _temp1037;}); _temp1036;});
_temp1035;}))); return Cyc_Absyn_uint_t; _LL988: if( Cyc_String_zstrcmp(* f,(
struct _tagged_string)({ char* _temp1038=( char*)"size"; struct _tagged_string
_temp1039; _temp1039.curr= _temp1038; _temp1039.base= _temp1038; _temp1039.last_plus_one=
_temp1038 + 5; _temp1039;})) == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp1040= Cyc_Absynpp_typ2string( t); xprintf("expecting struct, union or array, found %.*s",
_temp1040.last_plus_one - _temp1040.curr, _temp1040.curr);}));} else{ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp1041= Cyc_Absynpp_typ2string( t); xprintf("expecting struct or union, found %.*s",
_temp1041.last_plus_one - _temp1041.curr, _temp1041.curr);}));} _LL978:;}}
static void* Cyc_Tcexp_tcStructArrow( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e, struct _tagged_string* f){ Cyc_Tcexp_tcExp(
te, 0, e);{ void* t= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp1096= e->topt; if( _temp1096 == 0){ _throw( Null_Exception);} _temp1096->v;}));
void* _temp1043= t; struct Cyc_Absyn_PtrInfo _temp1049; struct Cyc_Absyn_Conref*
_temp1051; struct Cyc_Absyn_Tqual _temp1053; struct Cyc_Absyn_Conref* _temp1055;
void* _temp1057; void* _temp1059; _LL1045: if(( unsigned int) _temp1043 > 4u?((
struct _tunion_struct*) _temp1043)->tag == Cyc_Absyn_PointerType_tag: 0){
_LL1050: _temp1049=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1043)->f1; _LL1060: _temp1059=( void*) _temp1049.elt_typ; goto _LL1058;
_LL1058: _temp1057=( void*) _temp1049.rgn_typ; goto _LL1056; _LL1056: _temp1055=(
struct Cyc_Absyn_Conref*) _temp1049.nullable; goto _LL1054; _LL1054: _temp1053=(
struct Cyc_Absyn_Tqual) _temp1049.tq; goto _LL1052; _LL1052: _temp1051=( struct
Cyc_Absyn_Conref*) _temp1049.bounds; goto _LL1046;} else{ goto _LL1047;} _LL1047:
goto _LL1048; _LL1046: Cyc_Tcutil_check_nonzero_bound( loc, _temp1051);{ void*
_temp1061= Cyc_Tcutil_compress( _temp1059); struct Cyc_Absyn_Structdecl**
_temp1069; struct Cyc_Absyn_Structdecl* _temp1071; struct Cyc_List_List*
_temp1072; struct _tuple1* _temp1074; struct Cyc_Absyn_Uniondecl** _temp1076;
struct Cyc_Absyn_Uniondecl* _temp1078; struct Cyc_List_List* _temp1079; struct
_tuple1* _temp1081; _LL1063: if(( unsigned int) _temp1061 > 4u?(( struct
_tunion_struct*) _temp1061)->tag == Cyc_Absyn_StructType_tag: 0){ _LL1075:
_temp1074=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp1061)->f1;
goto _LL1073; _LL1073: _temp1072=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp1061)->f2; goto _LL1070; _LL1070: _temp1069=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp1061)->f3; if( _temp1069 == 0){ goto
_LL1065;} else{ _temp1071=* _temp1069; goto _LL1064;}} else{ goto _LL1065;}
_LL1065: if(( unsigned int) _temp1061 > 4u?(( struct _tunion_struct*) _temp1061)->tag
== Cyc_Absyn_UnionType_tag: 0){ _LL1082: _temp1081=( struct _tuple1*)(( struct
Cyc_Absyn_UnionType_struct*) _temp1061)->f1; goto _LL1080; _LL1080: _temp1079=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*) _temp1061)->f2;
goto _LL1077; _LL1077: _temp1076=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp1061)->f3; if( _temp1076 == 0){ goto _LL1067;} else{ _temp1078=* _temp1076;
goto _LL1066;}} else{ goto _LL1067;} _LL1067: goto _LL1068; _LL1064: { struct
Cyc_Absyn_Structfield* finfo= Cyc_Absyn_lookup_struct_field( _temp1071, f); if(
finfo == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string
_temp1084= Cyc_Absynpp_qvar2string(( struct _tuple1*)({ struct _tuple1*
_temp1083= _temp1074; if( _temp1083 == 0){ _throw( Null_Exception);} _temp1083;}));
struct _tagged_string _temp1085=* f; xprintf("struct %.*s has no %.*s field",
_temp1084.last_plus_one - _temp1084.curr, _temp1084.curr, _temp1085.last_plus_one
- _temp1085.curr, _temp1085.curr);}));}{ void* t3;{ struct _RegionHandle
_temp1086= _new_region(); struct _RegionHandle* rgn=& _temp1086; struct
_handler_cons _temp1087; _push_handler(& _temp1087);{ struct _xtunion_struct*
_temp1088=( struct _xtunion_struct*) setjmp( _temp1087.handler); if( ! _temp1088){{
struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)( struct _RegionHandle* r3,
struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, _temp1071->tvs, _temp1072); t3= Cyc_Tcutil_rsubstitute( rgn, inst,(
void*)({ struct Cyc_Absyn_Structfield* _temp1089= finfo; if( _temp1089 == 0){
_throw( Null_Exception);} _temp1089->type;}));} _free_region(& _temp1086);
_pop_handler();} else{ _free_region(& _temp1086); _throw( _temp1088);}}} Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp1057); return t3;}} _LL1066: { struct Cyc_Absyn_Structfield* finfo=
Cyc_Absyn_lookup_union_field( _temp1078, f); if( finfo == 0){ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp1091= Cyc_Absynpp_qvar2string(( struct
_tuple1*)({ struct _tuple1* _temp1090= _temp1081; if( _temp1090 == 0){ _throw(
Null_Exception);} _temp1090;})); struct _tagged_string _temp1092=* f; xprintf("union %.*s has no %.*s field",
_temp1091.last_plus_one - _temp1091.curr, _temp1091.curr, _temp1092.last_plus_one
- _temp1092.curr, _temp1092.curr);}));}{ struct Cyc_List_List* inst=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
_temp1078->tvs, _temp1079); void* t3= Cyc_Tcutil_substitute( inst,( void*)({
struct Cyc_Absyn_Structfield* _temp1093= finfo; if( _temp1093 == 0){ _throw(
Null_Exception);} _temp1093->type;})); Cyc_Tcenv_check_rgn_accessible( te, loc,
_temp1057); return t3;}} _LL1068: return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp1094= Cyc_Absynpp_typ2string( t); xprintf("expecting struct pointer, found %.*s",
_temp1094.last_plus_one - _temp1094.curr, _temp1094.curr);})); _LL1062:;}
_LL1048: return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string _temp1095=
Cyc_Absynpp_typ2string( t); xprintf("expecting struct pointer, found %.*s",
_temp1095.last_plus_one - _temp1095.curr, _temp1095.curr);})); _LL1044:;}}
static void* Cyc_Tcexp_ithTupleType( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* ts, struct Cyc_Absyn_Exp* index){ unsigned int i= Cyc_Evexp_eval_const_uint_exp(
index); struct _handler_cons _temp1097; _push_handler(& _temp1097);{ struct
_xtunion_struct* _temp1098=( struct _xtunion_struct*) setjmp( _temp1097.handler);
if( ! _temp1098){{ void* _temp1099=(*(( struct _tuple7*(*)( struct Cyc_List_List*
x, int i)) Cyc_List_nth)( ts,( int) i)).f2; _npop_handler( 0u); return _temp1099;};
_pop_handler();} else{ struct _xtunion_struct* _temp1101= _temp1098; _LL1103:
if( _temp1101->tag == Cyc_List_Nth_tag){ goto _LL1104;} else{ goto _LL1105;}
_LL1105: goto _LL1106; _LL1104: return Cyc_Tcexp_expr_err( te, loc, xprintf("index is %d but tuple has only %d fields",(
int) i,(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( ts))); _LL1106:(
void) _throw( _temp1101); _LL1102:;}}} static void* Cyc_Tcexp_tcSubscript(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp( te, 0, e1); Cyc_Tcexp_tcExp(
te, 0, e2);{ void* t1= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp1132= e1->topt; if( _temp1132 == 0){ _throw( Null_Exception);} _temp1132->v;}));
void* t2= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp1131= e2->topt;
if( _temp1131 == 0){ _throw( Null_Exception);} _temp1131->v;})); if( ! Cyc_Tcutil_coerce_sint_typ(
te, e2)){ return Cyc_Tcexp_expr_err( te, e2->loc,({ struct _tagged_string
_temp1107= Cyc_Absynpp_typ2string( t2); xprintf("expecting int subscript, found %.*s",
_temp1107.last_plus_one - _temp1107.curr, _temp1107.curr);}));}{ void* _temp1108=
t1; struct Cyc_Absyn_PtrInfo _temp1116; struct Cyc_Absyn_Conref* _temp1118;
struct Cyc_Absyn_Tqual _temp1120; struct Cyc_Absyn_Conref* _temp1122; void*
_temp1124; void* _temp1126; struct Cyc_List_List* _temp1128; _LL1110: if((
unsigned int) _temp1108 > 4u?(( struct _tunion_struct*) _temp1108)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL1117: _temp1116=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1108)->f1; _LL1127: _temp1126=( void*) _temp1116.elt_typ; goto _LL1125;
_LL1125: _temp1124=( void*) _temp1116.rgn_typ; goto _LL1123; _LL1123: _temp1122=(
struct Cyc_Absyn_Conref*) _temp1116.nullable; goto _LL1121; _LL1121: _temp1120=(
struct Cyc_Absyn_Tqual) _temp1116.tq; goto _LL1119; _LL1119: _temp1118=( struct
Cyc_Absyn_Conref*) _temp1116.bounds; goto _LL1111;} else{ goto _LL1112;} _LL1112:
if(( unsigned int) _temp1108 > 4u?(( struct _tunion_struct*) _temp1108)->tag ==
Cyc_Absyn_TupleType_tag: 0){ _LL1129: _temp1128=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp1108)->f1; goto _LL1113;} else{ goto
_LL1114;} _LL1114: goto _LL1115; _LL1111: if( Cyc_Tcutil_is_const_exp( te, e2)){
Cyc_Tcutil_check_bound( loc, Cyc_Evexp_eval_const_uint_exp( e2), _temp1118);}
else{ Cyc_Tcutil_check_nonzero_bound( loc, _temp1118);} Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp1124); return _temp1126; _LL1113: return Cyc_Tcexp_ithTupleType(
te, loc, _temp1128, e2); _LL1115: return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp1130= Cyc_Absynpp_typ2string( t1); xprintf("subscript applied to %.*s",
_temp1130.last_plus_one - _temp1130.curr, _temp1130.curr);})); _LL1109:;}}}
static void* Cyc_Tcexp_tcTuple( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_List_List* es){ int done= 0; struct Cyc_List_List*
fields= 0; if( topt != 0){ void* _temp1134= Cyc_Tcutil_compress(({ void**
_temp1133= topt; if( _temp1133 == 0){ _throw( Null_Exception);}* _temp1133;}));
struct Cyc_List_List* _temp1140; _LL1136: if(( unsigned int) _temp1134 > 4u?((
struct _tunion_struct*) _temp1134)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL1141:
_temp1140=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp1134)->f1; goto _LL1137;} else{ goto _LL1138;} _LL1138: goto _LL1139;
_LL1137: if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1140) !=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es)){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp1142=( char*)"tuple expression has the wrong number of fields";
struct _tagged_string _temp1143; _temp1143.curr= _temp1142; _temp1143.base=
_temp1142; _temp1143.last_plus_one= _temp1142 + 48; _temp1143;})); goto _LL1135;}
for( 0; es != 0; es=({ struct Cyc_List_List* _temp1144= es; if( _temp1144 == 0){
_throw( Null_Exception);} _temp1144->tl;}), _temp1140=({ struct Cyc_List_List*
_temp1145= _temp1140; if( _temp1145 == 0){ _throw( Null_Exception);} _temp1145->tl;})){
void* topt2=(*(( struct _tuple7*)({ struct Cyc_List_List* _temp1152= _temp1140;
if( _temp1152 == 0){ _throw( Null_Exception);} _temp1152->hd;}))).f2; Cyc_Tcexp_tcExpInitializer(
te,( void**)& topt2,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1146=
es; if( _temp1146 == 0){ _throw( Null_Exception);} _temp1146->hd;})); fields=({
struct Cyc_List_List* _temp1147=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1147->hd=( void*)({ struct _tuple7* _temp1148=(
struct _tuple7*) GC_malloc( sizeof( struct _tuple7)); _temp1148->f1=(*(( struct
_tuple7*)({ struct Cyc_List_List* _temp1151= _temp1140; if( _temp1151 == 0){
_throw( Null_Exception);} _temp1151->hd;}))).f1; _temp1148->f2=( void*)({ struct
Cyc_Core_Opt* _temp1150=(( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1149= es; if( _temp1149 == 0){ _throw( Null_Exception);} _temp1149->hd;}))->topt;
if( _temp1150 == 0){ _throw( Null_Exception);} _temp1150->v;}); _temp1148;});
_temp1147->tl= fields; _temp1147;});} done= 1; goto _LL1135; _LL1139: goto
_LL1135; _LL1135:;} if( ! done){ for( 0; es != 0; es=({ struct Cyc_List_List*
_temp1153= es; if( _temp1153 == 0){ _throw( Null_Exception);} _temp1153->tl;})){
Cyc_Tcexp_tcExpInitializer( te, 0,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1154= es; if( _temp1154 == 0){ _throw( Null_Exception);} _temp1154->hd;}));
fields=({ struct Cyc_List_List* _temp1155=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1155->hd=( void*)({ struct _tuple7*
_temp1156=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7)); _temp1156->f1=
Cyc_Absyn_empty_tqual(); _temp1156->f2=( void*)({ struct Cyc_Core_Opt* _temp1158=((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1157= es; if( _temp1157 == 0){
_throw( Null_Exception);} _temp1157->hd;}))->topt; if( _temp1158 == 0){ _throw(
Null_Exception);} _temp1158->v;}); _temp1156;}); _temp1155->tl= fields;
_temp1155;});}} return( void*)({ struct Cyc_Absyn_TupleType_struct* _temp1159=(
struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp1159[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp1160; _temp1160.tag= Cyc_Absyn_TupleType_tag;
_temp1160.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fields); _temp1160;}); _temp1159;});} static void* Cyc_Tcexp_tcCompoundLit(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
_tuple2* t, struct Cyc_List_List* des){ return Cyc_Tcexp_expr_err( te, loc,(
struct _tagged_string)({ char* _temp1161=( char*)"tcCompoundLit"; struct
_tagged_string _temp1162; _temp1162.curr= _temp1161; _temp1162.base= _temp1161;
_temp1162.last_plus_one= _temp1161 + 14; _temp1162;}));} static void* Cyc_Tcexp_tcArray(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** elt_topt,
struct Cyc_List_List* des){ struct Cyc_List_List* es=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)((
struct Cyc_Absyn_Exp*(*)( struct _tuple6*)) Cyc_Core_snd, des); void* res= Cyc_Absyn_new_evar((
void*) Cyc_Absyn_MemKind); struct Cyc_Absyn_Const_e_struct* sz_rexp=({ struct
Cyc_Absyn_Const_e_struct* _temp1185=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp1185[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp1186; _temp1186.tag= Cyc_Absyn_Const_e_tag;
_temp1186.f1=( void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp1187=(
struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp1187[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp1188; _temp1188.tag= Cyc_Absyn_Int_c_tag;
_temp1188.f1=( void*)(( void*) Cyc_Absyn_Unsigned); _temp1188.f2=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( es); _temp1188;}); _temp1187;}));
_temp1186;}); _temp1185;}); struct Cyc_Absyn_Exp* sz_exp= Cyc_Absyn_new_exp((
void*) sz_rexp, loc); sz_exp->topt=({ struct Cyc_Core_Opt* _temp1163=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1163->v=( void*)
Cyc_Absyn_uint_t; _temp1163;});{ void* res_t2=( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp1183=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1183[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp1184; _temp1184.tag= Cyc_Absyn_ArrayType_tag;
_temp1184.f1=( void*) res; _temp1184.f2= Cyc_Absyn_empty_tqual(); _temp1184.f3=(
struct Cyc_Absyn_Exp*) sz_exp; _temp1184;}); _temp1183;});{ struct Cyc_List_List*
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
ds=(*(( struct _tuple6*)({ struct Cyc_List_List* _temp1182= des; if( _temp1182
== 0){ _throw( Null_Exception);} _temp1182->hd;}))).f1; if( ds != 0){ void*
_temp1170=( void*)({ struct Cyc_List_List* _temp1169= ds; if( _temp1169 == 0){
_throw( Null_Exception);} _temp1169->hd;}); struct _tagged_string* _temp1176;
struct Cyc_Absyn_Exp* _temp1178; _LL1172: if((( struct _tunion_struct*)
_temp1170)->tag == Cyc_Absyn_FieldName_tag){ _LL1177: _temp1176=( struct
_tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp1170)->f1; goto
_LL1173;} else{ goto _LL1174;} _LL1174: if((( struct _tunion_struct*) _temp1170)->tag
== Cyc_Absyn_ArrayElement_tag){ _LL1179: _temp1178=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayElement_struct*) _temp1170)->f1; goto _LL1175;} else{ goto
_LL1171;} _LL1173: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp1180=( char*)"only array index designators are supported"; struct
_tagged_string _temp1181; _temp1181.curr= _temp1180; _temp1181.base= _temp1180;
_temp1181.last_plus_one= _temp1180 + 43; _temp1181;})); goto _LL1171; _LL1175:
Cyc_Tcexp_tcExpInitializer( te, 0, _temp1178);{ unsigned int i= Cyc_Evexp_eval_const_uint_exp(
_temp1178); if( i != offset){ Cyc_Tcutil_terr( _temp1178->loc, xprintf("expecting index designator %d but found %d",
offset,( int) i));} goto _LL1171;} _LL1171:;}}} return res_t2;}} static void*
Cyc_Tcexp_tcComprehension( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp* bound,
struct Cyc_Absyn_Exp* body){ Cyc_Tcexp_tcExp( te, 0, bound); if( ! Cyc_Tcutil_coerce_uint_typ(
te, bound)){ Cyc_Tcutil_terr( bound->loc,({ struct _tagged_string _temp1190= Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Core_Opt* _temp1189= bound->topt; if( _temp1189 == 0){
_throw( Null_Exception);} _temp1189->v;})); xprintf("expecting unsigned int, found %.*s",
_temp1190.last_plus_one - _temp1190.curr, _temp1190.curr);}));} if( !( vd->tq).q_const){((
void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp1191=( char*)"comprehension index variable is not declared const!";
struct _tagged_string _temp1192; _temp1192.curr= _temp1191; _temp1192.base=
_temp1191; _temp1192.last_plus_one= _temp1191 + 52; _temp1192;}));} if( te->le
!= 0){ te= Cyc_Tcenv_new_block( loc, te); te= Cyc_Tcenv_add_local_var( loc, te,
vd);} else{ if( ! Cyc_Tcutil_is_const_exp( te, bound)){ Cyc_Tcutil_terr( bound->loc,(
struct _tagged_string)({ char* _temp1193=( char*)"bound is not constant"; struct
_tagged_string _temp1194; _temp1194.curr= _temp1193; _temp1194.base= _temp1193;
_temp1194.last_plus_one= _temp1193 + 22; _temp1194;}));} if( ! Cyc_Tcutil_is_const_exp(
te, body)){ Cyc_Tcutil_terr( bound->loc,( struct _tagged_string)({ char*
_temp1195=( char*)"body is not constant"; struct _tagged_string _temp1196;
_temp1196.curr= _temp1195; _temp1196.base= _temp1195; _temp1196.last_plus_one=
_temp1195 + 21; _temp1196;}));}}{ struct Cyc_Absyn_PtrInfo pinfo; void** topt2=
0; struct Cyc_Absyn_Tqual* tqopt= 0; if( topt != 0){ void* _temp1198= Cyc_Tcutil_compress(({
void** _temp1197= topt; if( _temp1197 == 0){ _throw( Null_Exception);}*
_temp1197;})); struct Cyc_Absyn_PtrInfo _temp1206; struct Cyc_Absyn_Exp*
_temp1208; struct Cyc_Absyn_Tqual _temp1210; struct Cyc_Absyn_Tqual* _temp1212;
void* _temp1213; void** _temp1215; _LL1200: if(( unsigned int) _temp1198 > 4u?((
struct _tunion_struct*) _temp1198)->tag == Cyc_Absyn_PointerType_tag: 0){
_LL1207: _temp1206=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1198)->f1; goto _LL1201;} else{ goto _LL1202;} _LL1202: if(( unsigned int)
_temp1198 > 4u?(( struct _tunion_struct*) _temp1198)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL1214: _temp1213=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1198)->f1;
_temp1215=&(( struct Cyc_Absyn_ArrayType_struct*) _temp1198)->f1; goto _LL1211;
_LL1211: _temp1210=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1198)->f2; _temp1212=&(( struct Cyc_Absyn_ArrayType_struct*) _temp1198)->f2;
goto _LL1209; _LL1209: _temp1208=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1198)->f3; goto _LL1203;} else{ goto _LL1204;} _LL1204: goto _LL1205;
_LL1201: pinfo= _temp1206; topt2=( void**)(( void*)& pinfo.elt_typ); tqopt=(
struct Cyc_Absyn_Tqual*)& pinfo.tq; goto _LL1199; _LL1203: topt2=( void**)
_temp1215; tqopt=( struct Cyc_Absyn_Tqual*) _temp1212; goto _LL1199; _LL1205:
goto _LL1199; _LL1199:;}{ void* t= Cyc_Tcexp_tcExp( te, topt2, body); return(
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp1216=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp1216[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp1217; _temp1217.tag= Cyc_Absyn_ArrayType_tag;
_temp1217.f1=( void*) t; _temp1217.f2= tqopt == 0? Cyc_Absyn_empty_tqual():({
struct Cyc_Absyn_Tqual* _temp1218= tqopt; if( _temp1218 == 0){ _throw(
Null_Exception);}* _temp1218;}); _temp1217.f3=( struct Cyc_Absyn_Exp*) bound;
_temp1217;}); _temp1216;});}}} struct _tuple8{ struct Cyc_Absyn_Structfield* f1;
struct Cyc_Absyn_Exp* f2; } ; static void* Cyc_Tcexp_tcStruct( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct _tuple1** tn, struct
Cyc_Core_Opt** otyps, struct Cyc_List_List* args, struct Cyc_Absyn_Structdecl**
sd_opt){ struct Cyc_Absyn_Structdecl* sd; if(* sd_opt != 0){ sd=( struct Cyc_Absyn_Structdecl*)({
struct Cyc_Absyn_Structdecl* _temp1219=* sd_opt; if( _temp1219 == 0){ _throw(
Null_Exception);} _temp1219;});} else{{ struct _handler_cons _temp1220;
_push_handler(& _temp1220);{ struct _xtunion_struct* _temp1221=( struct
_xtunion_struct*) setjmp( _temp1220.handler); if( ! _temp1221){ sd=* Cyc_Tcenv_lookup_structdecl(
te, loc,* tn);; _pop_handler();} else{ struct _xtunion_struct* _temp1223=
_temp1221; _LL1225: if( _temp1223->tag == Cyc_Dict_Absent_tag){ goto _LL1226;}
else{ goto _LL1227;} _LL1227: goto _LL1228; _LL1226: Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp1229= Cyc_Absynpp_qvar2string(* tn); xprintf("unbound struct name %.*s",
_temp1229.last_plus_one - _temp1229.curr, _temp1229.curr);})); return topt != 0?({
void** _temp1230= topt; if( _temp1230 == 0){ _throw( Null_Exception);}*
_temp1230;}):( void*) Cyc_Absyn_VoidType; _LL1228:( void) _throw( _temp1223);
_LL1224:;}}}* sd_opt=( struct Cyc_Absyn_Structdecl*) sd; if( sd->name != 0){* tn=(
struct _tuple1*)({ struct Cyc_Core_Opt* _temp1231= sd->name; if( _temp1231 == 0){
_throw( Null_Exception);} _temp1231->v;});}}{ struct _RegionHandle _temp1232=
_new_region(); struct _RegionHandle* rgn=& _temp1232; struct _handler_cons
_temp1233; _push_handler(& _temp1233);{ struct _xtunion_struct* _temp1234=(
struct _xtunion_struct*) setjmp( _temp1233.handler); if( ! _temp1234){{ struct
Cyc_List_List* inst=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct
_tuple4*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Tvar*), struct
_RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,
rgn, sd->tvs); struct Cyc_List_List* typs=(( struct Cyc_List_List*(*)( void*(* f)(
struct _tuple4*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct
_tuple4*)) Cyc_Core_snd, inst); struct Cyc_Absyn_StructType_struct* res_typ=({
struct Cyc_Absyn_StructType_struct* _temp1252=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1252[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1253; _temp1253.tag= Cyc_Absyn_StructType_tag;
_temp1253.f1=( struct _tuple1*)* tn; _temp1253.f2= typs; _temp1253.f3=({ struct
Cyc_Absyn_Structdecl** _temp1254=( struct Cyc_Absyn_Structdecl**) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl*)); _temp1254[ 0]= sd; _temp1254;});
_temp1253;}); _temp1252;}); if( topt != 0){ Cyc_Tcutil_unify(( void*) res_typ,({
void** _temp1235= topt; if( _temp1235 == 0){ _throw( Null_Exception);}*
_temp1235;}));}* otyps=({ struct Cyc_Core_Opt* _temp1236=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1236->v=( void*) typs; _temp1236;});{
struct Cyc_List_List* fields=(( struct Cyc_List_List*(*)( struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_Absyn_Structdecl* sd)) Cyc_Tcutil_resolve_struct_designators)(
loc, args, sd); for( 0; fields != 0; fields=({ struct Cyc_List_List* _temp1237=
fields; if( _temp1237 == 0){ _throw( Null_Exception);} _temp1237->tl;})){ struct
_tuple8 _temp1241; struct Cyc_Absyn_Exp* _temp1242; struct Cyc_Absyn_Structfield*
_temp1244; struct _tuple8* _temp1239=( struct _tuple8*)({ struct Cyc_List_List*
_temp1238= fields; if( _temp1238 == 0){ _throw( Null_Exception);} _temp1238->hd;});
_temp1241=* _temp1239; _LL1245: _temp1244= _temp1241.f1; goto _LL1243; _LL1243:
_temp1242= _temp1241.f2; goto _LL1240; _LL1240: { void* inst_fieldtyp= Cyc_Tcutil_rsubstitute(
rgn, inst,( void*) _temp1244->type); Cyc_Tcexp_tcExpInitializer( te,( void**)&
inst_fieldtyp, _temp1242); if( ! Cyc_Tcutil_coerce_arg( te, _temp1242,
inst_fieldtyp)){ Cyc_Tcutil_terr( _temp1242->loc,({ struct _tagged_string
_temp1247=* _temp1244->name; struct _tagged_string _temp1248= Cyc_Absynpp_qvar2string(*
tn); struct _tagged_string _temp1249= Cyc_Absynpp_typ2string( inst_fieldtyp);
struct _tagged_string _temp1250= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp1246= _temp1242->topt; if( _temp1246 == 0){ _throw( Null_Exception);}
_temp1246->v;})); xprintf("field %.*s of struct %.*s expects type %.*s != %.*s",
_temp1247.last_plus_one - _temp1247.curr, _temp1247.curr, _temp1248.last_plus_one
- _temp1248.curr, _temp1248.curr, _temp1249.last_plus_one - _temp1249.curr,
_temp1249.curr, _temp1250.last_plus_one - _temp1250.curr, _temp1250.curr);}));}}}{
void* _temp1251=( void*) res_typ; _npop_handler( 0u); return _temp1251;}}}
_free_region(& _temp1232); _pop_handler();} else{ _free_region(& _temp1232);
_throw( _temp1234);}}}} static void* Cyc_Tcexp_tcTunion( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e,
struct Cyc_Core_Opt** all_ref, struct Cyc_Core_Opt** exist_ref, struct Cyc_List_List*
es, struct Cyc_Absyn_Tuniondecl* ed, struct Cyc_Absyn_Tunionfield* ef){ struct
_RegionHandle _temp1255= _new_region(); struct _RegionHandle* rgn=& _temp1255;
struct _handler_cons _temp1256; _push_handler(& _temp1256);{ struct
_xtunion_struct* _temp1257=( struct _xtunion_struct*) setjmp( _temp1256.handler);
if( ! _temp1257){{ struct Cyc_List_List* all_inst=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple4*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Tvar*),
struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,
rgn, ed->tvs); struct Cyc_List_List* exist_inst=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple4*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Tvar*),
struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,
rgn, ef->tvs); struct Cyc_List_List* all_typs=(( struct Cyc_List_List*(*)( void*(*
f)( struct _tuple4*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct
_tuple4*)) Cyc_Core_snd, all_inst); struct Cyc_List_List* exist_typs=(( struct
Cyc_List_List*(*)( void*(* f)( struct _tuple4*), struct Cyc_List_List* x)) Cyc_List_map)((
void*(*)( struct _tuple4*)) Cyc_Core_snd, exist_inst); struct Cyc_List_List*
inst=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rappend)( rgn, all_inst, exist_inst);*
all_ref=({ struct Cyc_Core_Opt* _temp1258=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1258->v=( void*) all_typs; _temp1258;});*
exist_ref=({ struct Cyc_Core_Opt* _temp1259=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1259->v=( void*) exist_typs; _temp1259;});{
void* res=( void*)({ struct Cyc_Absyn_TunionFieldType_struct* _temp1308=( struct
Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct));
_temp1308[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct _temp1309; _temp1309.tag=
Cyc_Absyn_TunionFieldType_tag; _temp1309.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp1310; _temp1310.name=( struct _tuple1*)({ struct Cyc_Core_Opt* _temp1311=
ed->name; if( _temp1311 == 0){ _throw( Null_Exception);} _temp1311->v;});
_temp1310.targs= all_typs; _temp1310.fname= ef->name; _temp1310.tud=( struct Cyc_Absyn_Tuniondecl*)
ed; _temp1310.tufd=( struct Cyc_Absyn_Tunionfield*) ef; _temp1310;}); _temp1309;});
_temp1308;}); if( topt != 0){ void* _temp1261= Cyc_Tcutil_compress(({ void**
_temp1260= topt; if( _temp1260 == 0){ _throw( Null_Exception);}* _temp1260;}));
struct Cyc_Absyn_TunionFieldInfo _temp1269; struct Cyc_Absyn_TunionInfo
_temp1271; struct Cyc_Absyn_Tuniondecl* _temp1273; void* _temp1275; struct Cyc_List_List*
_temp1277; struct _tuple1* _temp1279; _LL1263: if(( unsigned int) _temp1261 > 4u?((
struct _tunion_struct*) _temp1261)->tag == Cyc_Absyn_TunionFieldType_tag: 0){
_LL1270: _temp1269=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp1261)->f1; goto _LL1264;} else{ goto _LL1265;} _LL1265: if(( unsigned int)
_temp1261 > 4u?(( struct _tunion_struct*) _temp1261)->tag == Cyc_Absyn_TunionType_tag:
0){ _LL1272: _temp1271=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp1261)->f1; _LL1280: _temp1279=( struct _tuple1*) _temp1271.name; goto
_LL1278; _LL1278: _temp1277=( struct Cyc_List_List*) _temp1271.targs; goto
_LL1276; _LL1276: _temp1275=( void*) _temp1271.rgn; goto _LL1274; _LL1274:
_temp1273=( struct Cyc_Absyn_Tuniondecl*) _temp1271.tud; goto _LL1266;} else{
goto _LL1267;} _LL1267: goto _LL1268; _LL1264: Cyc_Tcutil_unify(({ void**
_temp1281= topt; if( _temp1281 == 0){ _throw( Null_Exception);}* _temp1281;}),
res); goto _LL1262; _LL1266:{ struct Cyc_List_List* a= all_typs; for( 0; a != 0?
_temp1277 != 0: 0; a=({ struct Cyc_List_List* _temp1282= a; if( _temp1282 == 0){
_throw( Null_Exception);} _temp1282->tl;}), _temp1277=({ struct Cyc_List_List*
_temp1283= _temp1277; if( _temp1283 == 0){ _throw( Null_Exception);} _temp1283->tl;})){
Cyc_Tcutil_unify(( void*)({ struct Cyc_List_List* _temp1284= a; if( _temp1284 ==
0){ _throw( Null_Exception);} _temp1284->hd;}),( void*)({ struct Cyc_List_List*
_temp1285= _temp1277; if( _temp1285 == 0){ _throw( Null_Exception);} _temp1285->hd;}));}}
if( ef->typs == 0? es == 0: 0){ e->topt=({ struct Cyc_Core_Opt* _temp1286=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1286->v=(
void*) res; _temp1286;}); res=( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp1287=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp1287[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp1288; _temp1288.tag=
Cyc_Absyn_TunionType_tag; _temp1288.f1=({ struct Cyc_Absyn_TunionInfo _temp1289;
_temp1289.name=( struct _tuple1*)({ struct Cyc_Core_Opt* _temp1290= ed->name;
if( _temp1290 == 0){ _throw( Null_Exception);} _temp1290->v;}); _temp1289.targs=
all_typs; _temp1289.rgn=( void*) Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind);
_temp1289.tud=( struct Cyc_Absyn_Tuniondecl*) ed; _temp1289;}); _temp1288;});
_temp1287;}); Cyc_Tcutil_unchecked_cast( te, e, res);{ void* _temp1291= res;
_npop_handler( 0u); return _temp1291;}} goto _LL1262; _LL1268: goto _LL1262;
_LL1262:;}{ struct Cyc_List_List* ts= ef->typs; for( 0; es != 0? ts != 0: 0; es=({
struct Cyc_List_List* _temp1292= es; if( _temp1292 == 0){ _throw( Null_Exception);}
_temp1292->tl;}), ts=({ struct Cyc_List_List* _temp1293= ts; if( _temp1293 == 0){
_throw( Null_Exception);} _temp1293->tl;})){ struct Cyc_Absyn_Exp* e=( struct
Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1301= es; if( _temp1301 == 0){
_throw( Null_Exception);} _temp1301->hd;}); void* t= Cyc_Tcutil_rsubstitute( rgn,
inst,(*(( struct _tuple7*)({ struct Cyc_List_List* _temp1300= ts; if( _temp1300
== 0){ _throw( Null_Exception);} _temp1300->hd;}))).f2); Cyc_Tcexp_tcExpInitializer(
te,( void**)& t, e); if( ! Cyc_Tcutil_coerce_arg( te, e, t)){ Cyc_Tcutil_terr( e->loc,({
struct _tagged_string _temp1297= Cyc_Absynpp_qvar2string( ef->name); struct
_tagged_string _temp1298= Cyc_Absynpp_typ2string( t); struct _tagged_string
_temp1299= e->topt == 0?( struct _tagged_string)({ char* _temp1294=( char*)"?";
struct _tagged_string _temp1295; _temp1295.curr= _temp1294; _temp1295.base=
_temp1294; _temp1295.last_plus_one= _temp1294 + 2; _temp1295;}): Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Core_Opt* _temp1296= e->topt; if( _temp1296 == 0){ _throw(
Null_Exception);} _temp1296->v;})); xprintf("tunion constructor %.*s expects argument of type %.*s but this argument has type %.*s",
_temp1297.last_plus_one - _temp1297.curr, _temp1297.curr, _temp1298.last_plus_one
- _temp1298.curr, _temp1298.curr, _temp1299.last_plus_one - _temp1299.curr,
_temp1299.curr);}));}} if( es != 0){ void* _temp1304= Cyc_Tcexp_expr_err( te,((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1302= es; if( _temp1302 == 0){
_throw( Null_Exception);} _temp1302->hd;}))->loc,({ struct _tagged_string
_temp1303= Cyc_Absynpp_qvar2string( ef->name); xprintf("too many arguments for tunion constructor %.*s",
_temp1303.last_plus_one - _temp1303.curr, _temp1303.curr);})); _npop_handler( 0u);
return _temp1304;} if( ts != 0){ void* _temp1306= Cyc_Tcexp_expr_err( te, loc,({
struct _tagged_string _temp1305= Cyc_Absynpp_qvar2string( ef->name); xprintf("too few arguments for tunion constructor %.*s",
_temp1305.last_plus_one - _temp1305.curr, _temp1305.curr);})); _npop_handler( 0u);
return _temp1306;}{ void* _temp1307= res; _npop_handler( 0u); return _temp1307;}}}}
_free_region(& _temp1255); _pop_handler();} else{ _free_region(& _temp1255);
_throw( _temp1257);}}} static void* Cyc_Tcexp_tcXTunion( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Core_Opt**
exist_ref, struct Cyc_List_List* es, struct Cyc_Absyn_XTuniondecl* xd, struct
Cyc_Absyn_Tunionfield* ef){ struct _RegionHandle _temp1312= _new_region();
struct _RegionHandle* rgn=& _temp1312; struct _handler_cons _temp1313;
_push_handler(& _temp1313);{ struct _xtunion_struct* _temp1314=( struct
_xtunion_struct*) setjmp( _temp1313.handler); if( ! _temp1314){{ struct Cyc_List_List*
exist_inst=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple4*(*
f)( struct _RegionHandle*, struct Cyc_Absyn_Tvar*), struct _RegionHandle* env,
struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var, rgn,
ef->tvs); struct Cyc_List_List* exist_typs=(( struct Cyc_List_List*(*)( void*(*
f)( struct _tuple4*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct
_tuple4*)) Cyc_Core_snd, exist_inst); struct Cyc_Absyn_XTunionFieldType_struct*
res=({ struct Cyc_Absyn_XTunionFieldType_struct* _temp1329=( struct Cyc_Absyn_XTunionFieldType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_XTunionFieldType_struct)); _temp1329[ 0]=({
struct Cyc_Absyn_XTunionFieldType_struct _temp1330; _temp1330.tag= Cyc_Absyn_XTunionFieldType_tag;
_temp1330.f1=({ struct Cyc_Absyn_XTunionFieldInfo _temp1331; _temp1331.name= xd->name;
_temp1331.fname= ef->name; _temp1331.xtud=( struct Cyc_Absyn_XTuniondecl*) xd;
_temp1331.xtufd=( struct Cyc_Absyn_Tunionfield*) ef; _temp1331;}); _temp1330;});
_temp1329;}); struct Cyc_List_List* ts= ef->typs;* exist_ref=({ struct Cyc_Core_Opt*
_temp1315=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1315->v=( void*) exist_typs; _temp1315;}); while( es != 0? ts != 0: 0) {
struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1323= es; if( _temp1323 == 0){ _throw( Null_Exception);} _temp1323->hd;});
void* t= Cyc_Tcutil_rsubstitute( rgn, exist_inst,(*(( struct _tuple7*)({ struct
Cyc_List_List* _temp1322= ts; if( _temp1322 == 0){ _throw( Null_Exception);}
_temp1322->hd;}))).f2); Cyc_Tcexp_tcExpInitializer( te,( void**)& t, e); es=({
struct Cyc_List_List* _temp1316= es; if( _temp1316 == 0){ _throw( Null_Exception);}
_temp1316->tl;}); ts=({ struct Cyc_List_List* _temp1317= ts; if( _temp1317 == 0){
_throw( Null_Exception);} _temp1317->tl;}); if( ! Cyc_Tcutil_coerce_arg( te, e,
t)){ Cyc_Tcutil_terr( e->loc,({ struct _tagged_string _temp1319= Cyc_Absynpp_qvar2string(
ef->name); struct _tagged_string _temp1320= Cyc_Absynpp_typ2string( t); struct
_tagged_string _temp1321= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp1318= e->topt; if( _temp1318 == 0){ _throw( Null_Exception);} _temp1318->v;}));
xprintf("xtunion constructor %.*s expects argument of type %.*s but this argument has type %.*s",
_temp1319.last_plus_one - _temp1319.curr, _temp1319.curr, _temp1320.last_plus_one
- _temp1320.curr, _temp1320.curr, _temp1321.last_plus_one - _temp1321.curr,
_temp1321.curr);}));}} if( es != 0){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1324= es; if( _temp1324 == 0){ _throw( Null_Exception);}
_temp1324->hd;}))->loc,({ struct _tagged_string _temp1325= Cyc_Absynpp_qvar2string(
ef->name); xprintf("too many arguments for xtunion constructor %.*s", _temp1325.last_plus_one
- _temp1325.curr, _temp1325.curr);}));} else{ if( ts != 0){ Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1326= es; if( _temp1326 == 0){
_throw( Null_Exception);} _temp1326->hd;}))->loc,({ struct _tagged_string
_temp1327= Cyc_Absynpp_qvar2string( ef->name); xprintf("too few arguments for xtunion constructor %.*s",
_temp1327.last_plus_one - _temp1327.curr, _temp1327.curr);}));}}{ void*
_temp1328=( void*) res; _npop_handler( 0u); return _temp1328;}} _free_region(&
_temp1312); _pop_handler();} else{ _free_region(& _temp1312); _throw( _temp1314);}}}
static void* Cyc_Tcexp_tcMalloc( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* ropt, void* t){ void* rgn=( void*) Cyc_Absyn_HeapRgn;
if( ropt != 0){ void* expected_type=( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp1343=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp1343[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp1344; _temp1344.tag= Cyc_Absyn_RgnHandleType_tag; _temp1344.f1=( void*) Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind); _temp1344;}); _temp1343;}); void* handle_type= Cyc_Tcexp_tcExp(
te,( void**)& expected_type,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1342= ropt; if( _temp1342 == 0){ _throw( Null_Exception);} _temp1342;}));
void* _temp1332= Cyc_Tcutil_compress( handle_type); void* _temp1338; _LL1334:
if(( unsigned int) _temp1332 > 4u?(( struct _tunion_struct*) _temp1332)->tag ==
Cyc_Absyn_RgnHandleType_tag: 0){ _LL1339: _temp1338=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1332)->f1; goto _LL1335;} else{ goto _LL1336;} _LL1336: goto _LL1337;
_LL1335: rgn= _temp1338; Cyc_Tcenv_check_rgn_accessible( te, loc, rgn); goto
_LL1333; _LL1337: Cyc_Tcutil_terr(({ struct Cyc_Absyn_Exp* _temp1340= ropt; if(
_temp1340 == 0){ _throw( Null_Exception);} _temp1340->loc;}),({ struct
_tagged_string _temp1341= Cyc_Absynpp_typ2string( handle_type); xprintf("expecting region_t type but found %.*s",
_temp1341.last_plus_one - _temp1341.curr, _temp1341.curr);})); goto _LL1333;
_LL1333:;} Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars( te),(
void*) Cyc_Absyn_MemKind, t);{ void* _temp1345= t; struct Cyc_Absyn_TunionFieldInfo
_temp1353; struct Cyc_Absyn_Tunionfield* _temp1355; struct Cyc_Absyn_Tuniondecl*
_temp1357; struct _tuple1* _temp1359; struct Cyc_List_List* _temp1361; struct
_tuple1* _temp1363; struct Cyc_Absyn_XTunionFieldInfo _temp1365; struct Cyc_Absyn_Tunionfield*
_temp1367; struct Cyc_Absyn_XTuniondecl* _temp1369; struct _tuple1* _temp1371;
struct _tuple1* _temp1373; _LL1347: if(( unsigned int) _temp1345 > 4u?(( struct
_tunion_struct*) _temp1345)->tag == Cyc_Absyn_TunionFieldType_tag: 0){ _LL1354:
_temp1353=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp1345)->f1; _LL1364: _temp1363=( struct _tuple1*) _temp1353.name; goto
_LL1362; _LL1362: _temp1361=( struct Cyc_List_List*) _temp1353.targs; goto
_LL1360; _LL1360: _temp1359=( struct _tuple1*) _temp1353.fname; goto _LL1358;
_LL1358: _temp1357=( struct Cyc_Absyn_Tuniondecl*) _temp1353.tud; goto _LL1356;
_LL1356: _temp1355=( struct Cyc_Absyn_Tunionfield*) _temp1353.tufd; goto _LL1348;}
else{ goto _LL1349;} _LL1349: if(( unsigned int) _temp1345 > 4u?(( struct
_tunion_struct*) _temp1345)->tag == Cyc_Absyn_XTunionFieldType_tag: 0){ _LL1366:
_temp1365=( struct Cyc_Absyn_XTunionFieldInfo)(( struct Cyc_Absyn_XTunionFieldType_struct*)
_temp1345)->f1; _LL1374: _temp1373=( struct _tuple1*) _temp1365.name; goto
_LL1372; _LL1372: _temp1371=( struct _tuple1*) _temp1365.fname; goto _LL1370;
_LL1370: _temp1369=( struct Cyc_Absyn_XTuniondecl*) _temp1365.xtud; goto _LL1368;
_LL1368: _temp1367=( struct Cyc_Absyn_Tunionfield*) _temp1365.xtufd; goto
_LL1350;} else{ goto _LL1351;} _LL1351: goto _LL1352; _LL1348: _temp1367=
_temp1355; goto _LL1350; _LL1350: if(({ struct Cyc_Absyn_Tunionfield* _temp1375=
_temp1367; if( _temp1375 == 0){ _throw( Null_Exception);} _temp1375->tvs;}) != 0){
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp1376=( char*)"malloc with existential types not yet implemented";
struct _tagged_string _temp1377; _temp1377.curr= _temp1376; _temp1377.base=
_temp1376; _temp1377.last_plus_one= _temp1376 + 50; _temp1377;}));} goto _LL1346;
_LL1352: goto _LL1346; _LL1346:;}{ void*(* ptr_maker)( void* t, void* rgn,
struct Cyc_Absyn_Tqual tq)= Cyc_Absyn_at_typ; if( topt != 0){ void* _temp1379=
Cyc_Tcutil_compress(({ void** _temp1378= topt; if( _temp1378 == 0){ _throw(
Null_Exception);}* _temp1378;})); struct Cyc_Absyn_PtrInfo _temp1385; struct Cyc_Absyn_Conref*
_temp1387; struct Cyc_Absyn_Tqual _temp1389; struct Cyc_Absyn_Conref* _temp1391;
void* _temp1393; void* _temp1395; _LL1381: if(( unsigned int) _temp1379 > 4u?((
struct _tunion_struct*) _temp1379)->tag == Cyc_Absyn_PointerType_tag: 0){
_LL1386: _temp1385=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1379)->f1; _LL1396: _temp1395=( void*) _temp1385.elt_typ; goto _LL1394;
_LL1394: _temp1393=( void*) _temp1385.rgn_typ; goto _LL1392; _LL1392: _temp1391=(
struct Cyc_Absyn_Conref*) _temp1385.nullable; goto _LL1390; _LL1390: _temp1389=(
struct Cyc_Absyn_Tqual) _temp1385.tq; goto _LL1388; _LL1388: _temp1387=( struct
Cyc_Absyn_Conref*) _temp1385.bounds; goto _LL1382;} else{ goto _LL1383;} _LL1383:
goto _LL1384; _LL1382: if((( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1391)){ ptr_maker= Cyc_Absyn_star_typ;} goto _LL1380; _LL1384: goto _LL1380;
_LL1380:;} return ptr_maker( t, rgn, Cyc_Absyn_empty_tqual());}} static void*
Cyc_Tcexp_tcStmtExp( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, struct Cyc_Absyn_Stmt* s){ Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser(
te, s), s, 1); Cyc_NewControlFlow_set_encloser( s, Cyc_Tcenv_get_encloser( te));
while( 1) { void* _temp1397=( void*) s->r; struct Cyc_Absyn_Exp* _temp1407;
struct Cyc_Absyn_Stmt* _temp1409; struct Cyc_Absyn_Stmt* _temp1411; struct Cyc_Absyn_Stmt*
_temp1413; struct Cyc_Absyn_Decl* _temp1415; _LL1399: if(( unsigned int)
_temp1397 > 1u?(( struct _tunion_struct*) _temp1397)->tag == Cyc_Absyn_Exp_s_tag:
0){ _LL1408: _temp1407=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp1397)->f1; goto _LL1400;} else{ goto _LL1401;} _LL1401: if(( unsigned int)
_temp1397 > 1u?(( struct _tunion_struct*) _temp1397)->tag == Cyc_Absyn_Seq_s_tag:
0){ _LL1412: _temp1411=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp1397)->f1; goto _LL1410; _LL1410: _temp1409=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp1397)->f2; goto _LL1402;} else{ goto
_LL1403;} _LL1403: if(( unsigned int) _temp1397 > 1u?(( struct _tunion_struct*)
_temp1397)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL1416: _temp1415=( struct Cyc_Absyn_Decl*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1397)->f1; goto _LL1414; _LL1414:
_temp1413=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1397)->f2;
goto _LL1404;} else{ goto _LL1405;} _LL1405: goto _LL1406; _LL1400: return( void*)({
struct Cyc_Core_Opt* _temp1417= _temp1407->topt; if( _temp1417 == 0){ _throw(
Null_Exception);} _temp1417->v;}); _LL1402: s= _temp1409; continue; _LL1404: s=
_temp1413; continue; _LL1406: return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_string)({ char* _temp1418=( char*)"statement expression must end with expression";
struct _tagged_string _temp1419; _temp1419.curr= _temp1418; _temp1419.base=
_temp1418; _temp1419.last_plus_one= _temp1418 + 46; _temp1419;})); _LL1398:;}}
static void* Cyc_Tcexp_tcCodegen( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Fndecl* fd){ return Cyc_Tcexp_expr_err( te,
loc,( struct _tagged_string)({ char* _temp1420=( char*)"tcCodegen"; struct
_tagged_string _temp1421; _temp1421.curr= _temp1420; _temp1421.base= _temp1420;
_temp1421.last_plus_one= _temp1420 + 10; _temp1421;}));} static void* Cyc_Tcexp_tcFill(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e){ return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({
char* _temp1422=( char*)"tcFill"; struct _tagged_string _temp1423; _temp1423.curr=
_temp1422; _temp1423.base= _temp1422; _temp1423.last_plus_one= _temp1422 + 7;
_temp1423;}));} static void* Cyc_Tcexp_tcNew( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* rgn_handle, struct
Cyc_Absyn_Exp* e, struct Cyc_Absyn_Exp* e1){ void* rgn=( void*) Cyc_Absyn_HeapRgn;
if( rgn_handle != 0){ void* expected_type=( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp1435=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp1435[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp1436; _temp1436.tag= Cyc_Absyn_RgnHandleType_tag; _temp1436.f1=( void*) Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind); _temp1436;}); _temp1435;}); void* handle_type= Cyc_Tcexp_tcExp(
te,( void**)& expected_type,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1434= rgn_handle; if( _temp1434 == 0){ _throw( Null_Exception);} _temp1434;}));
void* _temp1424= Cyc_Tcutil_compress( handle_type); void* _temp1430; _LL1426:
if(( unsigned int) _temp1424 > 4u?(( struct _tunion_struct*) _temp1424)->tag ==
Cyc_Absyn_RgnHandleType_tag: 0){ _LL1431: _temp1430=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1424)->f1; goto _LL1427;} else{ goto _LL1428;} _LL1428: goto _LL1429;
_LL1427: rgn= _temp1430; Cyc_Tcenv_check_rgn_accessible( te, loc, rgn); goto
_LL1425; _LL1429: Cyc_Tcutil_terr(({ struct Cyc_Absyn_Exp* _temp1432= rgn_handle;
if( _temp1432 == 0){ _throw( Null_Exception);} _temp1432->loc;}),({ struct
_tagged_string _temp1433= Cyc_Absynpp_typ2string( handle_type); xprintf("expecting region_t type but found %.*s",
_temp1433.last_plus_one - _temp1433.curr, _temp1433.curr);})); goto _LL1425;
_LL1425:;}{ void* _temp1437=( void*) e1->r; struct Cyc_Absyn_Exp* _temp1449;
struct Cyc_Absyn_Exp* _temp1451; struct Cyc_Absyn_Vardecl* _temp1453; struct Cyc_List_List*
_temp1455; struct Cyc_Core_Opt* _temp1457; struct Cyc_List_List* _temp1459; void*
_temp1461; struct _tagged_string _temp1463; _LL1439: if((( struct _tunion_struct*)
_temp1437)->tag == Cyc_Absyn_Comprehension_e_tag){ _LL1454: _temp1453=( struct
Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1437)->f1;
goto _LL1452; _LL1452: _temp1451=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1437)->f2; goto _LL1450; _LL1450: _temp1449=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1437)->f3; goto _LL1440;} else{
goto _LL1441;} _LL1441: if((( struct _tunion_struct*) _temp1437)->tag == Cyc_Absyn_UnresolvedMem_e_tag){
_LL1458: _temp1457=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1437)->f1; goto _LL1456; _LL1456: _temp1455=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp1437)->f2; goto _LL1442;} else{
goto _LL1443;} _LL1443: if((( struct _tunion_struct*) _temp1437)->tag == Cyc_Absyn_Array_e_tag){
_LL1460: _temp1459=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp1437)->f1; goto _LL1444;} else{ goto _LL1445;} _LL1445: if((( struct
_tunion_struct*) _temp1437)->tag == Cyc_Absyn_Const_e_tag){ _LL1462: _temp1461=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1437)->f1; if(( unsigned int)
_temp1461 > 1u?(( struct _tunion_struct*) _temp1461)->tag == Cyc_Absyn_String_c_tag:
0){ _LL1464: _temp1463=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp1461)->f1; goto _LL1446;} else{ goto _LL1447;}} else{ goto _LL1447;}
_LL1447: goto _LL1448; _LL1440: { void* t_arr= Cyc_Tcexp_tcExpNoPromote( te,
topt, e1); void* _temp1465= Cyc_Tcutil_compress( t_arr); struct Cyc_Absyn_Exp*
_temp1471; struct Cyc_Absyn_Tqual _temp1473; void* _temp1475; _LL1467: if((
unsigned int) _temp1465 > 4u?(( struct _tunion_struct*) _temp1465)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL1476: _temp1475=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1465)->f1;
goto _LL1474; _LL1474: _temp1473=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1465)->f2; goto _LL1472; _LL1472: _temp1471=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1465)->f3; goto _LL1468;} else{ goto
_LL1469;} _LL1469: goto _LL1470; _LL1468: { void* b= Cyc_Tcutil_is_const_exp( te,
_temp1451)?( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1485=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1485[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1486; _temp1486.tag= Cyc_Absyn_Upper_b_tag;
_temp1486.f1= _temp1451; _temp1486;}); _temp1485;}):( void*) Cyc_Absyn_Unknown_b;
void* res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1482=( struct
Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1482[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1483; _temp1483.tag=
Cyc_Absyn_PointerType_tag; _temp1483.f1=({ struct Cyc_Absyn_PtrInfo _temp1484;
_temp1484.elt_typ=( void*) _temp1475; _temp1484.rgn_typ=( void*) rgn; _temp1484.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp1484.tq=
_temp1473; _temp1484.bounds=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
b); _temp1484;}); _temp1483;}); _temp1482;}); if( topt != 0){ if( ! Cyc_Tcutil_unify(({
void** _temp1477= topt; if( _temp1477 == 0){ _throw( Null_Exception);}*
_temp1477;}), res_typ)? Cyc_Tcutil_silent_castable( te, loc, res_typ,({ void**
_temp1478= topt; if( _temp1478 == 0){ _throw( Null_Exception);}* _temp1478;})):
0){ e->topt=({ struct Cyc_Core_Opt* _temp1479=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1479->v=( void*) res_typ; _temp1479;}); Cyc_Tcutil_unchecked_cast(
te, e,({ void** _temp1480= topt; if( _temp1480 == 0){ _throw( Null_Exception);}*
_temp1480;})); res_typ=({ void** _temp1481= topt; if( _temp1481 == 0){ _throw(
Null_Exception);}* _temp1481;});}} return res_typ;} _LL1470: return(( void*(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp1487=( char*)"tcNew: comprehension returned non-array type"; struct
_tagged_string _temp1488; _temp1488.curr= _temp1487; _temp1488.base= _temp1487;
_temp1488.last_plus_one= _temp1487 + 45; _temp1488;})); _LL1466:;} _LL1442:(
void*)( e1->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct* _temp1489=(
struct Cyc_Absyn_Array_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp1489[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp1490; _temp1490.tag= Cyc_Absyn_Array_e_tag;
_temp1490.f1= _temp1455; _temp1490;}); _temp1489;}))); _temp1459= _temp1455;
goto _LL1444; _LL1444: { void** elt_typ_opt= 0; if( topt != 0){ void* _temp1492=
Cyc_Tcutil_compress(({ void** _temp1491= topt; if( _temp1491 == 0){ _throw(
Null_Exception);}* _temp1491;})); struct Cyc_Absyn_PtrInfo _temp1498; struct Cyc_Absyn_Conref*
_temp1500; struct Cyc_Absyn_Tqual _temp1502; struct Cyc_Absyn_Conref* _temp1504;
void* _temp1506; void* _temp1508; void** _temp1510; _LL1494: if(( unsigned int)
_temp1492 > 4u?(( struct _tunion_struct*) _temp1492)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL1499: _temp1498=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1492)->f1; _LL1509: _temp1508=( void*) _temp1498.elt_typ; _temp1510=&(((
struct Cyc_Absyn_PointerType_struct*) _temp1492)->f1).elt_typ; goto _LL1507;
_LL1507: _temp1506=( void*) _temp1498.rgn_typ; goto _LL1505; _LL1505: _temp1504=(
struct Cyc_Absyn_Conref*) _temp1498.nullable; goto _LL1503; _LL1503: _temp1502=(
struct Cyc_Absyn_Tqual) _temp1498.tq; goto _LL1501; _LL1501: _temp1500=( struct
Cyc_Absyn_Conref*) _temp1498.bounds; goto _LL1495;} else{ goto _LL1496;} _LL1496:
goto _LL1497; _LL1495: elt_typ_opt=( void**) _temp1510; goto _LL1493; _LL1497:
goto _LL1493; _LL1493:;}{ void* t_array= Cyc_Tcexp_tcExpNoPromote( te,
elt_typ_opt, e1); void* res_typ;{ void* _temp1511= Cyc_Tcutil_compress( t_array);
struct Cyc_Absyn_Exp* _temp1517; struct Cyc_Absyn_Tqual _temp1519; void*
_temp1521; _LL1513: if(( unsigned int) _temp1511 > 4u?(( struct _tunion_struct*)
_temp1511)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL1522: _temp1521=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1511)->f1; goto _LL1520; _LL1520:
_temp1519=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1511)->f2; goto _LL1518; _LL1518: _temp1517=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1511)->f3; goto _LL1514;} else{ goto
_LL1515;} _LL1515: goto _LL1516; _LL1514: res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp1523=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1523[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1524; _temp1524.tag=
Cyc_Absyn_PointerType_tag; _temp1524.f1=({ struct Cyc_Absyn_PtrInfo _temp1525;
_temp1525.elt_typ=( void*) _temp1521; _temp1525.rgn_typ=( void*) rgn; _temp1525.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp1525.tq=
_temp1519; _temp1525.bounds=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1526=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1526[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1527; _temp1527.tag= Cyc_Absyn_Upper_b_tag;
_temp1527.f1=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1528=
_temp1517; if( _temp1528 == 0){ _throw( Null_Exception);} _temp1528;});
_temp1527;}); _temp1526;})); _temp1525;}); _temp1524;}); _temp1523;}); if( topt
!= 0){ if( ! Cyc_Tcutil_unify(({ void** _temp1529= topt; if( _temp1529 == 0){
_throw( Null_Exception);}* _temp1529;}), res_typ)? Cyc_Tcutil_silent_castable(
te, loc, res_typ,({ void** _temp1530= topt; if( _temp1530 == 0){ _throw(
Null_Exception);}* _temp1530;})): 0){ e->topt=({ struct Cyc_Core_Opt* _temp1531=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1531->v=(
void*) res_typ; _temp1531;}); Cyc_Tcutil_unchecked_cast( te, e,({ void**
_temp1532= topt; if( _temp1532 == 0){ _throw( Null_Exception);}* _temp1532;}));
res_typ=({ void** _temp1533= topt; if( _temp1533 == 0){ _throw( Null_Exception);}*
_temp1533;});}} goto _LL1512; _LL1516: return(( void*(*)( struct _tagged_string))
Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp1534=( char*)"tcExpNoPromote on Array_e returned non-array type";
struct _tagged_string _temp1535; _temp1535.curr= _temp1534; _temp1535.base=
_temp1534; _temp1535.last_plus_one= _temp1534 + 50; _temp1535;})); _LL1512:;}
return res_typ;}} _LL1446: { void* topt2= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t,
rgn, Cyc_Absyn_empty_tqual(),( void*) Cyc_Absyn_Unknown_b); void* t= Cyc_Tcexp_tcExp(
te,( void**)& topt2, e1); return Cyc_Absyn_atb_typ( t, rgn, Cyc_Absyn_empty_tqual(),(
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1536=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1536[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1537; _temp1537.tag= Cyc_Absyn_Upper_b_tag;
_temp1537.f1= Cyc_Absyn_uint_exp( 1, 0); _temp1537;}); _temp1536;}));} _LL1448: {
void** topt2= 0; if( topt != 0){ void* _temp1539= Cyc_Tcutil_compress(({ void**
_temp1538= topt; if( _temp1538 == 0){ _throw( Null_Exception);}* _temp1538;}));
struct Cyc_Absyn_PtrInfo _temp1547; struct Cyc_Absyn_Conref* _temp1549; struct
Cyc_Absyn_Tqual _temp1551; struct Cyc_Absyn_Conref* _temp1553; void* _temp1555;
void* _temp1557; void** _temp1559; struct Cyc_Absyn_TunionInfo _temp1560;
_LL1541: if(( unsigned int) _temp1539 > 4u?(( struct _tunion_struct*) _temp1539)->tag
== Cyc_Absyn_PointerType_tag: 0){ _LL1548: _temp1547=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1539)->f1; _LL1558: _temp1557=( void*)
_temp1547.elt_typ; _temp1559=&((( struct Cyc_Absyn_PointerType_struct*)
_temp1539)->f1).elt_typ; goto _LL1556; _LL1556: _temp1555=( void*) _temp1547.rgn_typ;
goto _LL1554; _LL1554: _temp1553=( struct Cyc_Absyn_Conref*) _temp1547.nullable;
goto _LL1552; _LL1552: _temp1551=( struct Cyc_Absyn_Tqual) _temp1547.tq; goto
_LL1550; _LL1550: _temp1549=( struct Cyc_Absyn_Conref*) _temp1547.bounds; goto
_LL1542;} else{ goto _LL1543;} _LL1543: if(( unsigned int) _temp1539 > 4u?((
struct _tunion_struct*) _temp1539)->tag == Cyc_Absyn_TunionType_tag: 0){ _LL1561:
_temp1560=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp1539)->f1; goto _LL1544;} else{ goto _LL1545;} _LL1545: goto _LL1546;
_LL1542: topt2=( void**) _temp1559; goto _LL1540; _LL1544: topt2=({ void**
_temp1562=( void**) GC_malloc( sizeof( void*)); _temp1562[ 0]=({ void**
_temp1563= topt; if( _temp1563 == 0){ _throw( Null_Exception);}* _temp1563;});
_temp1562;}); goto _LL1540; _LL1546: goto _LL1540; _LL1540:;}{ void* telt= Cyc_Tcexp_tcExp(
te, topt2, e1); void* res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp1569=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1569[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1570; _temp1570.tag=
Cyc_Absyn_PointerType_tag; _temp1570.f1=({ struct Cyc_Absyn_PtrInfo _temp1571;
_temp1571.elt_typ=( void*) telt; _temp1571.rgn_typ=( void*) rgn; _temp1571.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp1571.tq= Cyc_Absyn_empty_tqual();
_temp1571.bounds=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1572=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1572[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1573; _temp1573.tag= Cyc_Absyn_Upper_b_tag;
_temp1573.f1= Cyc_Absyn_uint_exp( 1, 0); _temp1573;}); _temp1572;})); _temp1571;});
_temp1570;}); _temp1569;}); if( topt != 0){ if( ! Cyc_Tcutil_unify(({ void**
_temp1564= topt; if( _temp1564 == 0){ _throw( Null_Exception);}* _temp1564;}),
res_typ)? Cyc_Tcutil_silent_castable( te, loc, res_typ,({ void** _temp1565= topt;
if( _temp1565 == 0){ _throw( Null_Exception);}* _temp1565;})): 0){ e->topt=({
struct Cyc_Core_Opt* _temp1566=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1566->v=( void*) res_typ; _temp1566;}); Cyc_Tcutil_unchecked_cast(
te, e,({ void** _temp1567= topt; if( _temp1567 == 0){ _throw( Null_Exception);}*
_temp1567;})); res_typ=({ void** _temp1568= topt; if( _temp1568 == 0){ _throw(
Null_Exception);}* _temp1568;});}} return res_typ;}} _LL1438:;}} void* Cyc_Tcexp_tcExp(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcexp_tcExpNoPromote(
te, topt, e); t= Cyc_Tcutil_compress( t);{ void* _temp1574= t; struct Cyc_Absyn_Exp*
_temp1580; struct Cyc_Absyn_Tqual _temp1582; void* _temp1584; _LL1576: if((
unsigned int) _temp1574 > 4u?(( struct _tunion_struct*) _temp1574)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL1585: _temp1584=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1574)->f1;
goto _LL1583; _LL1583: _temp1582=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1574)->f2; goto _LL1581; _LL1581: _temp1580=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1574)->f3; goto _LL1577;} else{ goto
_LL1578;} _LL1578: goto _LL1579; _LL1577: { void* _temp1588; int _temp1590;
struct _tuple5 _temp1586= Cyc_Tcutil_addressof_props( te, e); _LL1591: _temp1590=
_temp1586.f1; goto _LL1589; _LL1589: _temp1588= _temp1586.f2; goto _LL1587;
_LL1587: { void* b= _temp1580 == 0?( void*) Cyc_Absyn_Unknown_b:( void*)({
struct Cyc_Absyn_Upper_b_struct* _temp1592=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1592[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1593; _temp1593.tag= Cyc_Absyn_Upper_b_tag;
_temp1593.f1=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1594=
_temp1580; if( _temp1594 == 0){ _throw( Null_Exception);} _temp1594;});
_temp1593;}); _temp1592;}); t= Cyc_Absyn_atb_typ( _temp1584, _temp1588,
_temp1582, b); Cyc_Tcutil_unchecked_cast( te, e, t); return t;}} _LL1579: return
t; _LL1575:;}} void* Cyc_Tcexp_tcExpInitializer( struct Cyc_Tcenv_Tenv* te, void**
topt, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcexp_tcExpNoPromote( te, topt, e);
void* _temp1595=( void*) e->r; struct Cyc_List_List* _temp1605; struct Cyc_Absyn_Exp*
_temp1607; struct Cyc_Absyn_Exp* _temp1609; struct Cyc_Absyn_Vardecl* _temp1611;
void* _temp1613; struct _tagged_string _temp1615; _LL1597: if((( struct
_tunion_struct*) _temp1595)->tag == Cyc_Absyn_Array_e_tag){ _LL1606: _temp1605=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1595)->f1; goto
_LL1598;} else{ goto _LL1599;} _LL1599: if((( struct _tunion_struct*) _temp1595)->tag
== Cyc_Absyn_Comprehension_e_tag){ _LL1612: _temp1611=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1595)->f1; goto _LL1610; _LL1610:
_temp1609=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1595)->f2; goto _LL1608; _LL1608: _temp1607=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1595)->f3; goto _LL1600;} else{
goto _LL1601;} _LL1601: if((( struct _tunion_struct*) _temp1595)->tag == Cyc_Absyn_Const_e_tag){
_LL1614: _temp1613=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1595)->f1;
if(( unsigned int) _temp1613 > 1u?(( struct _tunion_struct*) _temp1613)->tag ==
Cyc_Absyn_String_c_tag: 0){ _LL1616: _temp1615=( struct _tagged_string)(( struct
Cyc_Absyn_String_c_struct*) _temp1613)->f1; goto _LL1602;} else{ goto _LL1603;}}
else{ goto _LL1603;} _LL1603: goto _LL1604; _LL1598: return t; _LL1600: return t;
_LL1602: return t; _LL1604: t= Cyc_Tcutil_compress( t);{ void* _temp1617= t;
struct Cyc_Absyn_Exp* _temp1623; struct Cyc_Absyn_Tqual _temp1625; void*
_temp1627; _LL1619: if(( unsigned int) _temp1617 > 4u?(( struct _tunion_struct*)
_temp1617)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL1628: _temp1627=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1617)->f1; goto _LL1626; _LL1626:
_temp1625=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1617)->f2; goto _LL1624; _LL1624: _temp1623=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1617)->f3; goto _LL1620;} else{ goto
_LL1621;} _LL1621: goto _LL1622; _LL1620: { void* _temp1631; int _temp1633;
struct _tuple5 _temp1629= Cyc_Tcutil_addressof_props( te, e); _LL1634: _temp1633=
_temp1629.f1; goto _LL1632; _LL1632: _temp1631= _temp1629.f2; goto _LL1630;
_LL1630: { void* b= _temp1623 == 0?( void*) Cyc_Absyn_Unknown_b:( void*)({
struct Cyc_Absyn_Upper_b_struct* _temp1635=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1635[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1636; _temp1636.tag= Cyc_Absyn_Upper_b_tag;
_temp1636.f1=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1637=
_temp1623; if( _temp1637 == 0){ _throw( Null_Exception);} _temp1637;});
_temp1636;}); _temp1635;}); t= Cyc_Absyn_atb_typ( _temp1627, _temp1631,
_temp1625, b); Cyc_Tcutil_unchecked_cast( te, e, t); return t;}} _LL1622: return
t; _LL1618:;} _LL1596:;} static void* Cyc_Tcexp_tcExpNoPromote( struct Cyc_Tcenv_Tenv*
te, void** topt, struct Cyc_Absyn_Exp* e){{ void* _temp1638=( void*) e->r;
struct Cyc_Absyn_Exp* _temp1644; _LL1640: if((( struct _tunion_struct*)
_temp1638)->tag == Cyc_Absyn_NoInstantiate_e_tag){ _LL1645: _temp1644=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*) _temp1638)->f1; goto
_LL1641;} else{ goto _LL1642;} _LL1642: goto _LL1643; _LL1641: Cyc_Tcexp_tcExpNoInst(
te, topt, _temp1644);( void*)({ struct Cyc_Core_Opt* _temp1646= _temp1644->topt;
if( _temp1646 == 0){ _throw( Null_Exception);} _temp1646->v=( void*) Cyc_Absyn_pointer_expand(
Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp1647= _temp1644->topt;
if( _temp1647 == 0){ _throw( Null_Exception);} _temp1647->v;})));}); e->topt=
_temp1644->topt; goto _LL1639; _LL1643: Cyc_Tcexp_tcExpNoInst( te, topt, e);(
void*)({ struct Cyc_Core_Opt* _temp1648= e->topt; if( _temp1648 == 0){ _throw(
Null_Exception);} _temp1648->v=( void*) Cyc_Absyn_pointer_expand( Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp1649= e->topt; if( _temp1649 == 0){ _throw(
Null_Exception);} _temp1649->v;})));});{ void* _temp1651= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp1650= e->topt; if( _temp1650 == 0){ _throw(
Null_Exception);} _temp1650->v;})); struct Cyc_Absyn_PtrInfo _temp1657; struct
Cyc_Absyn_Conref* _temp1659; struct Cyc_Absyn_Tqual _temp1661; struct Cyc_Absyn_Conref*
_temp1663; void* _temp1665; void* _temp1667; _LL1653: if(( unsigned int)
_temp1651 > 4u?(( struct _tunion_struct*) _temp1651)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL1658: _temp1657=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1651)->f1; _LL1668: _temp1667=( void*) _temp1657.elt_typ; goto _LL1666;
_LL1666: _temp1665=( void*) _temp1657.rgn_typ; goto _LL1664; _LL1664: _temp1663=(
struct Cyc_Absyn_Conref*) _temp1657.nullable; goto _LL1662; _LL1662: _temp1661=(
struct Cyc_Absyn_Tqual) _temp1657.tq; goto _LL1660; _LL1660: _temp1659=( struct
Cyc_Absyn_Conref*) _temp1657.bounds; goto _LL1654;} else{ goto _LL1655;} _LL1655:
goto _LL1656; _LL1654:{ void* _temp1669= Cyc_Tcutil_compress( _temp1667); struct
Cyc_Absyn_FnInfo _temp1675; struct Cyc_List_List* _temp1677; int _temp1679;
struct Cyc_List_List* _temp1681; void* _temp1683; struct Cyc_Core_Opt* _temp1685;
struct Cyc_List_List* _temp1687; _LL1671: if(( unsigned int) _temp1669 > 4u?((
struct _tunion_struct*) _temp1669)->tag == Cyc_Absyn_FnType_tag: 0){ _LL1676:
_temp1675=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp1669)->f1; _LL1688: _temp1687=( struct Cyc_List_List*) _temp1675.tvars;
goto _LL1686; _LL1686: _temp1685=( struct Cyc_Core_Opt*) _temp1675.effect; goto
_LL1684; _LL1684: _temp1683=( void*) _temp1675.ret_typ; goto _LL1682; _LL1682:
_temp1681=( struct Cyc_List_List*) _temp1675.args; goto _LL1680; _LL1680:
_temp1679=( int) _temp1675.varargs; goto _LL1678; _LL1678: _temp1677=( struct
Cyc_List_List*) _temp1675.attributes; goto _LL1672;} else{ goto _LL1673;}
_LL1673: goto _LL1674; _LL1672: if( _temp1687 != 0){ struct _RegionHandle
_temp1689= _new_region(); struct _RegionHandle* rgn=& _temp1689; struct
_handler_cons _temp1690; _push_handler(& _temp1690);{ struct _xtunion_struct*
_temp1691=( struct _xtunion_struct*) setjmp( _temp1690.handler); if( ! _temp1691){{
struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct _tuple4*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Tvar*), struct
_RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,
rgn, _temp1687); struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)( void*(* f)(
struct _tuple4*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct
_tuple4*)) Cyc_Core_snd, inst); void* ftyp= Cyc_Tcutil_rsubstitute( rgn, inst,(
void*)({ struct Cyc_Absyn_FnType_struct* _temp1698=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1698[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1699; _temp1699.tag= Cyc_Absyn_FnType_tag; _temp1699.f1=({ struct Cyc_Absyn_FnInfo
_temp1700; _temp1700.tvars= 0; _temp1700.effect= _temp1685; _temp1700.ret_typ=(
void*) _temp1683; _temp1700.args= _temp1681; _temp1700.varargs= _temp1679;
_temp1700.attributes= _temp1677; _temp1700;}); _temp1699;}); _temp1698;})); void*
new_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1695=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp1695[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp1696; _temp1696.tag= Cyc_Absyn_PointerType_tag;
_temp1696.f1=({ struct Cyc_Absyn_PtrInfo _temp1697; _temp1697.elt_typ=( void*)
ftyp; _temp1697.rgn_typ=( void*) _temp1665; _temp1697.nullable= _temp1663;
_temp1697.tq= _temp1661; _temp1697.bounds= _temp1659; _temp1697;}); _temp1696;});
_temp1695;}); struct Cyc_Absyn_Exp* inner= Cyc_Absyn_copy_exp( e);( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Instantiate_e_struct* _temp1692=( struct Cyc_Absyn_Instantiate_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Instantiate_e_struct)); _temp1692[ 0]=({
struct Cyc_Absyn_Instantiate_e_struct _temp1693; _temp1693.tag= Cyc_Absyn_Instantiate_e_tag;
_temp1693.f1= inner; _temp1693.f2= ts; _temp1693;}); _temp1692;}))); e->topt=({
struct Cyc_Core_Opt* _temp1694=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1694->v=( void*) new_typ; _temp1694;});} _free_region(&
_temp1689); _pop_handler();} else{ _free_region(& _temp1689); _throw( _temp1691);}}}
goto _LL1670; _LL1674: goto _LL1670; _LL1670:;} goto _LL1652; _LL1656: goto
_LL1652; _LL1652:;} goto _LL1639; _LL1639:;} return( void*)({ struct Cyc_Core_Opt*
_temp1701= e->topt; if( _temp1701 == 0){ _throw( Null_Exception);} _temp1701->v;});}
static void Cyc_Tcexp_tcExpNoInst( struct Cyc_Tcenv_Tenv* te, void** topt,
struct Cyc_Absyn_Exp* e){ struct Cyc_Position_Segment* loc= e->loc; void* t;{
void* _temp1702=( void*) e->r; struct Cyc_Absyn_Exp* _temp1774; struct _tuple1*
_temp1776; struct Cyc_List_List* _temp1778; struct Cyc_Absyn_Exp* _temp1780;
struct Cyc_List_List* _temp1782; struct Cyc_Core_Opt* _temp1784; void* _temp1786;
void* _temp1788; struct _tuple1* _temp1790; struct Cyc_List_List* _temp1792;
void* _temp1794; void* _temp1796; struct Cyc_Absyn_Exp* _temp1798; struct Cyc_Absyn_Exp*
_temp1800; struct Cyc_Core_Opt* _temp1802; struct Cyc_Absyn_Exp* _temp1804;
struct Cyc_Absyn_Exp* _temp1806; struct Cyc_Absyn_Exp* _temp1808; struct Cyc_Absyn_Exp*
_temp1810; struct Cyc_Absyn_Exp* _temp1812; struct Cyc_Absyn_Exp* _temp1814;
struct Cyc_List_List* _temp1816; struct Cyc_Absyn_Exp* _temp1818; struct Cyc_Absyn_Exp*
_temp1820; struct Cyc_List_List* _temp1822; struct Cyc_Absyn_Exp* _temp1824;
struct Cyc_Absyn_Exp* _temp1826; void* _temp1828; struct Cyc_Absyn_Exp*
_temp1830; struct Cyc_Absyn_Exp* _temp1832; struct Cyc_Absyn_Exp* _temp1834;
struct Cyc_Absyn_Exp* _temp1836; void* _temp1838; struct Cyc_Absyn_Exp*
_temp1840; struct _tagged_string* _temp1842; struct Cyc_Absyn_Exp* _temp1844;
struct _tagged_string* _temp1846; struct Cyc_Absyn_Exp* _temp1848; struct Cyc_Absyn_Exp*
_temp1850; struct Cyc_Absyn_Exp* _temp1852; struct Cyc_List_List* _temp1854;
struct Cyc_List_List* _temp1856; struct _tuple2* _temp1858; struct Cyc_List_List*
_temp1860; struct Cyc_Absyn_Stmt* _temp1862; struct Cyc_Absyn_Fndecl* _temp1864;
struct Cyc_Absyn_Exp* _temp1866; struct Cyc_Absyn_Exp* _temp1868; struct Cyc_Absyn_Exp*
_temp1870; struct Cyc_Absyn_Vardecl* _temp1872; struct Cyc_Absyn_Structdecl*
_temp1874; struct Cyc_Absyn_Structdecl** _temp1876; struct Cyc_List_List*
_temp1877; struct Cyc_Core_Opt* _temp1879; struct Cyc_Core_Opt** _temp1881;
struct _tuple1* _temp1882; struct _tuple1** _temp1884; struct Cyc_Absyn_Tunionfield*
_temp1885; struct Cyc_Absyn_Tuniondecl* _temp1887; struct Cyc_List_List*
_temp1889; struct Cyc_Core_Opt* _temp1891; struct Cyc_Core_Opt** _temp1893;
struct Cyc_Core_Opt* _temp1894; struct Cyc_Core_Opt** _temp1896; struct Cyc_Absyn_Tunionfield*
_temp1897; struct Cyc_Absyn_XTuniondecl* _temp1899; struct Cyc_List_List*
_temp1901; struct Cyc_Core_Opt* _temp1903; struct Cyc_Core_Opt** _temp1905;
struct Cyc_Absyn_Enumfield* _temp1906; struct Cyc_Absyn_Enumdecl* _temp1908;
struct _tuple1* _temp1910; void* _temp1912; struct Cyc_Absyn_Exp* _temp1914;
_LL1704: if((( struct _tunion_struct*) _temp1702)->tag == Cyc_Absyn_NoInstantiate_e_tag){
_LL1775: _temp1774=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1702)->f1; goto _LL1705;} else{ goto _LL1706;} _LL1706: if((( struct
_tunion_struct*) _temp1702)->tag == Cyc_Absyn_UnknownId_e_tag){ _LL1777:
_temp1776=( struct _tuple1*)(( struct Cyc_Absyn_UnknownId_e_struct*) _temp1702)->f1;
goto _LL1707;} else{ goto _LL1708;} _LL1708: if((( struct _tunion_struct*)
_temp1702)->tag == Cyc_Absyn_UnknownCall_e_tag){ _LL1781: _temp1780=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_UnknownCall_e_struct*) _temp1702)->f1; goto _LL1779; _LL1779:
_temp1778=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1702)->f2; goto _LL1709;} else{ goto _LL1710;} _LL1710: if((( struct
_tunion_struct*) _temp1702)->tag == Cyc_Absyn_UnresolvedMem_e_tag){ _LL1785:
_temp1784=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1702)->f1; goto _LL1783; _LL1783: _temp1782=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp1702)->f2; goto _LL1711;} else{
goto _LL1712;} _LL1712: if((( struct _tunion_struct*) _temp1702)->tag == Cyc_Absyn_Const_e_tag){
_LL1787: _temp1786=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1702)->f1;
goto _LL1713;} else{ goto _LL1714;} _LL1714: if((( struct _tunion_struct*)
_temp1702)->tag == Cyc_Absyn_Var_e_tag){ _LL1791: _temp1790=( struct _tuple1*)((
struct Cyc_Absyn_Var_e_struct*) _temp1702)->f1; goto _LL1789; _LL1789: _temp1788=(
void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1702)->f2; goto _LL1715;} else{
goto _LL1716;} _LL1716: if((( struct _tunion_struct*) _temp1702)->tag == Cyc_Absyn_Primop_e_tag){
_LL1795: _temp1794=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1702)->f1;
goto _LL1793; _LL1793: _temp1792=( struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp1702)->f2; goto _LL1717;} else{ goto _LL1718;} _LL1718: if((( struct
_tunion_struct*) _temp1702)->tag == Cyc_Absyn_Increment_e_tag){ _LL1799:
_temp1798=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1702)->f1; goto _LL1797; _LL1797: _temp1796=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1702)->f2; goto _LL1719;} else{ goto _LL1720;} _LL1720: if((( struct
_tunion_struct*) _temp1702)->tag == Cyc_Absyn_AssignOp_e_tag){ _LL1805:
_temp1804=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1702)->f1; goto _LL1803; _LL1803: _temp1802=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1702)->f2; goto _LL1801; _LL1801:
_temp1800=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1702)->f3; goto _LL1721;} else{ goto _LL1722;} _LL1722: if((( struct
_tunion_struct*) _temp1702)->tag == Cyc_Absyn_Conditional_e_tag){ _LL1811:
_temp1810=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1702)->f1; goto _LL1809; _LL1809: _temp1808=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp1702)->f2; goto _LL1807; _LL1807:
_temp1806=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1702)->f3; goto _LL1723;} else{ goto _LL1724;} _LL1724: if((( struct
_tunion_struct*) _temp1702)->tag == Cyc_Absyn_SeqExp_e_tag){ _LL1815: _temp1814=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*) _temp1702)->f1; goto
_LL1813; _LL1813: _temp1812=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1702)->f2; goto _LL1725;} else{ goto _LL1726;} _LL1726: if((( struct
_tunion_struct*) _temp1702)->tag == Cyc_Absyn_FnCall_e_tag){ _LL1819: _temp1818=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*) _temp1702)->f1; goto
_LL1817; _LL1817: _temp1816=( struct Cyc_List_List*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1702)->f2; goto _LL1727;} else{ goto _LL1728;} _LL1728: if((( struct
_tunion_struct*) _temp1702)->tag == Cyc_Absyn_Throw_e_tag){ _LL1821: _temp1820=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*) _temp1702)->f1; goto
_LL1729;} else{ goto _LL1730;} _LL1730: if((( struct _tunion_struct*) _temp1702)->tag
== Cyc_Absyn_Instantiate_e_tag){ _LL1825: _temp1824=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Instantiate_e_struct*) _temp1702)->f1; goto _LL1823; _LL1823:
_temp1822=( struct Cyc_List_List*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1702)->f2; goto _LL1731;} else{ goto _LL1732;} _LL1732: if((( struct
_tunion_struct*) _temp1702)->tag == Cyc_Absyn_Cast_e_tag){ _LL1829: _temp1828=(
void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp1702)->f1; goto _LL1827; _LL1827:
_temp1826=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*) _temp1702)->f2;
goto _LL1733;} else{ goto _LL1734;} _LL1734: if((( struct _tunion_struct*)
_temp1702)->tag == Cyc_Absyn_Address_e_tag){ _LL1831: _temp1830=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Address_e_struct*) _temp1702)->f1; goto _LL1735;} else{ goto
_LL1736;} _LL1736: if((( struct _tunion_struct*) _temp1702)->tag == Cyc_Absyn_New_e_tag){
_LL1835: _temp1834=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*)
_temp1702)->f1; goto _LL1833; _LL1833: _temp1832=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_New_e_struct*) _temp1702)->f2; goto _LL1737;} else{ goto
_LL1738;} _LL1738: if((( struct _tunion_struct*) _temp1702)->tag == Cyc_Absyn_Sizeofexp_e_tag){
_LL1837: _temp1836=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1702)->f1; goto _LL1739;} else{ goto _LL1740;} _LL1740: if((( struct
_tunion_struct*) _temp1702)->tag == Cyc_Absyn_Sizeoftyp_e_tag){ _LL1839:
_temp1838=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp1702)->f1; goto
_LL1741;} else{ goto _LL1742;} _LL1742: if((( struct _tunion_struct*) _temp1702)->tag
== Cyc_Absyn_Deref_e_tag){ _LL1841: _temp1840=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Deref_e_struct*) _temp1702)->f1; goto _LL1743;} else{ goto _LL1744;}
_LL1744: if((( struct _tunion_struct*) _temp1702)->tag == Cyc_Absyn_StructMember_e_tag){
_LL1845: _temp1844=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1702)->f1; goto _LL1843; _LL1843: _temp1842=( struct _tagged_string*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp1702)->f2; goto _LL1745;} else{
goto _LL1746;} _LL1746: if((( struct _tunion_struct*) _temp1702)->tag == Cyc_Absyn_StructArrow_e_tag){
_LL1849: _temp1848=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1702)->f1; goto _LL1847; _LL1847: _temp1846=( struct _tagged_string*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp1702)->f2; goto _LL1747;} else{
goto _LL1748;} _LL1748: if((( struct _tunion_struct*) _temp1702)->tag == Cyc_Absyn_Subscript_e_tag){
_LL1853: _temp1852=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1702)->f1; goto _LL1851; _LL1851: _temp1850=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp1702)->f2; goto _LL1749;} else{ goto
_LL1750;} _LL1750: if((( struct _tunion_struct*) _temp1702)->tag == Cyc_Absyn_Tuple_e_tag){
_LL1855: _temp1854=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1702)->f1; goto _LL1751;} else{ goto _LL1752;} _LL1752: if((( struct
_tunion_struct*) _temp1702)->tag == Cyc_Absyn_CompoundLit_e_tag){ _LL1859:
_temp1858=( struct _tuple2*)(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp1702)->f1;
goto _LL1857; _LL1857: _temp1856=( struct Cyc_List_List*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1702)->f2; goto _LL1753;} else{ goto _LL1754;} _LL1754: if((( struct
_tunion_struct*) _temp1702)->tag == Cyc_Absyn_Array_e_tag){ _LL1861: _temp1860=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1702)->f1; goto
_LL1755;} else{ goto _LL1756;} _LL1756: if((( struct _tunion_struct*) _temp1702)->tag
== Cyc_Absyn_StmtExp_e_tag){ _LL1863: _temp1862=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_StmtExp_e_struct*) _temp1702)->f1; goto _LL1757;} else{ goto
_LL1758;} _LL1758: if((( struct _tunion_struct*) _temp1702)->tag == Cyc_Absyn_Codegen_e_tag){
_LL1865: _temp1864=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1702)->f1; goto _LL1759;} else{ goto _LL1760;} _LL1760: if((( struct
_tunion_struct*) _temp1702)->tag == Cyc_Absyn_Fill_e_tag){ _LL1867: _temp1866=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*) _temp1702)->f1; goto
_LL1761;} else{ goto _LL1762;} _LL1762: if((( struct _tunion_struct*) _temp1702)->tag
== Cyc_Absyn_Comprehension_e_tag){ _LL1873: _temp1872=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1702)->f1; goto _LL1871; _LL1871:
_temp1870=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1702)->f2; goto _LL1869; _LL1869: _temp1868=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1702)->f3; goto _LL1763;} else{
goto _LL1764;} _LL1764: if((( struct _tunion_struct*) _temp1702)->tag == Cyc_Absyn_Struct_e_tag){
_LL1883: _temp1882=( struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1702)->f1; _temp1884=&(( struct Cyc_Absyn_Struct_e_struct*) _temp1702)->f1;
goto _LL1880; _LL1880: _temp1879=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1702)->f2; _temp1881=&(( struct Cyc_Absyn_Struct_e_struct*) _temp1702)->f2;
goto _LL1878; _LL1878: _temp1877=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1702)->f3; goto _LL1875; _LL1875: _temp1874=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1702)->f4; _temp1876=&(( struct Cyc_Absyn_Struct_e_struct*)
_temp1702)->f4; goto _LL1765;} else{ goto _LL1766;} _LL1766: if((( struct
_tunion_struct*) _temp1702)->tag == Cyc_Absyn_Tunion_e_tag){ _LL1895: _temp1894=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp1702)->f1;
_temp1896=&(( struct Cyc_Absyn_Tunion_e_struct*) _temp1702)->f1; goto _LL1892;
_LL1892: _temp1891=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1702)->f2; _temp1893=&(( struct Cyc_Absyn_Tunion_e_struct*) _temp1702)->f2;
goto _LL1890; _LL1890: _temp1889=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1702)->f3; goto _LL1888; _LL1888: _temp1887=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1702)->f4; goto _LL1886; _LL1886:
_temp1885=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1702)->f5; goto _LL1767;} else{ goto _LL1768;} _LL1768: if((( struct
_tunion_struct*) _temp1702)->tag == Cyc_Absyn_XTunion_e_tag){ _LL1904: _temp1903=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_XTunion_e_struct*) _temp1702)->f1;
_temp1905=&(( struct Cyc_Absyn_XTunion_e_struct*) _temp1702)->f1; goto _LL1902;
_LL1902: _temp1901=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_e_struct*)
_temp1702)->f2; goto _LL1900; _LL1900: _temp1899=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_e_struct*) _temp1702)->f3; goto _LL1898; _LL1898:
_temp1897=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_e_struct*)
_temp1702)->f4; goto _LL1769;} else{ goto _LL1770;} _LL1770: if((( struct
_tunion_struct*) _temp1702)->tag == Cyc_Absyn_Enum_e_tag){ _LL1911: _temp1910=(
struct _tuple1*)(( struct Cyc_Absyn_Enum_e_struct*) _temp1702)->f1; goto _LL1909;
_LL1909: _temp1908=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp1702)->f2; goto _LL1907; _LL1907: _temp1906=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_e_struct*) _temp1702)->f3; goto _LL1771;} else{ goto
_LL1772;} _LL1772: if((( struct _tunion_struct*) _temp1702)->tag == Cyc_Absyn_Malloc_e_tag){
_LL1915: _temp1914=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1702)->f1; goto _LL1913; _LL1913: _temp1912=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1702)->f2; goto _LL1773;} else{ goto _LL1703;} _LL1705: Cyc_Tcexp_tcExpNoInst(
te, 0, _temp1774); return; _LL1707: Cyc_Tcexp_resolve_unknown_id( te, e,
_temp1776); Cyc_Tcexp_tcExpNoInst( te, topt, e); return; _LL1709: Cyc_Tcexp_resolve_unknown_fn(
te, e, _temp1780, _temp1778); Cyc_Tcexp_tcExpNoInst( te, topt, e); return;
_LL1711: Cyc_Tcexp_resolve_unresolved_mem( te, loc, topt, e, _temp1782); Cyc_Tcexp_tcExpNoInst(
te, topt, e); return; _LL1713: t= Cyc_Tcexp_tcConst( te, loc, topt, _temp1786, e);
goto _LL1703; _LL1715: t= Cyc_Tcexp_tcVar( te, loc, _temp1790, _temp1788); goto
_LL1703; _LL1717: t= Cyc_Tcexp_tcPrimop( te, loc, topt, _temp1794, _temp1792);
goto _LL1703; _LL1719: t= Cyc_Tcexp_tcIncrement( te, loc, topt, _temp1798,
_temp1796); goto _LL1703; _LL1721: t= Cyc_Tcexp_tcAssignOp( te, loc, topt,
_temp1804, _temp1802, _temp1800); goto _LL1703; _LL1723: t= Cyc_Tcexp_tcConditional(
te, loc, topt, _temp1810, _temp1808, _temp1806); goto _LL1703; _LL1725: t= Cyc_Tcexp_tcSeqExp(
te, loc, topt, _temp1814, _temp1812); goto _LL1703; _LL1727: t= Cyc_Tcexp_tcFnCall(
te, loc, topt, _temp1818, _temp1816); goto _LL1703; _LL1729: t= Cyc_Tcexp_tcThrow(
te, loc, topt, _temp1820); goto _LL1703; _LL1731: t= Cyc_Tcexp_tcInstantiate( te,
loc, topt, _temp1824, _temp1822); goto _LL1703; _LL1733: t= Cyc_Tcexp_tcCast( te,
loc, topt, _temp1828, _temp1826); goto _LL1703; _LL1735: t= Cyc_Tcexp_tcAddress(
te, loc, topt, _temp1830); goto _LL1703; _LL1737: t= Cyc_Tcexp_tcNew( te, loc,
topt, _temp1834, e, _temp1832); goto _LL1703; _LL1739: { void* t1= Cyc_Tcexp_tcExpNoPromote(
te, 0, _temp1836); t= Cyc_Tcexp_tcSizeof( te, loc, topt, t1); goto _LL1703;}
_LL1741: t= Cyc_Tcexp_tcSizeof( te, loc, topt, _temp1838); goto _LL1703; _LL1743:
t= Cyc_Tcexp_tcDeref( te, loc, topt, _temp1840); goto _LL1703; _LL1745: t= Cyc_Tcexp_tcStructMember(
te, loc, topt, e, _temp1844, _temp1842); goto _LL1703; _LL1747: t= Cyc_Tcexp_tcStructArrow(
te, loc, topt, _temp1848, _temp1846); goto _LL1703; _LL1749: t= Cyc_Tcexp_tcSubscript(
te, loc, topt, _temp1852, _temp1850); goto _LL1703; _LL1751: t= Cyc_Tcexp_tcTuple(
te, loc, topt, _temp1854); goto _LL1703; _LL1753: t= Cyc_Tcexp_tcCompoundLit( te,
loc, topt, _temp1858, _temp1856); goto _LL1703; _LL1755: { void** elt_topt= 0;
if( topt != 0){ void* _temp1917= Cyc_Tcutil_compress(({ void** _temp1916= topt;
if( _temp1916 == 0){ _throw( Null_Exception);}* _temp1916;})); struct Cyc_Absyn_Exp*
_temp1923; struct Cyc_Absyn_Tqual _temp1925; void* _temp1927; void** _temp1929;
_LL1919: if(( unsigned int) _temp1917 > 4u?(( struct _tunion_struct*) _temp1917)->tag
== Cyc_Absyn_ArrayType_tag: 0){ _LL1928: _temp1927=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1917)->f1; _temp1929=&(( struct Cyc_Absyn_ArrayType_struct*) _temp1917)->f1;
goto _LL1926; _LL1926: _temp1925=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1917)->f2; goto _LL1924; _LL1924: _temp1923=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1917)->f3; goto _LL1920;} else{ goto
_LL1921;} _LL1921: goto _LL1922; _LL1920: elt_topt=( void**) _temp1929; goto
_LL1918; _LL1922: goto _LL1918; _LL1918:;} t= Cyc_Tcexp_tcArray( te, loc,
elt_topt, _temp1860); goto _LL1703;} _LL1757: t= Cyc_Tcexp_tcStmtExp( te, loc,
topt, _temp1862); goto _LL1703; _LL1759: t= Cyc_Tcexp_tcCodegen( te, loc, topt,
_temp1864); goto _LL1703; _LL1761: t= Cyc_Tcexp_tcFill( te, loc, topt, _temp1866);
goto _LL1703; _LL1763: t= Cyc_Tcexp_tcComprehension( te, loc, topt, _temp1872,
_temp1870, _temp1868); goto _LL1703; _LL1765: t= Cyc_Tcexp_tcStruct( te, loc,
topt, _temp1884, _temp1881, _temp1877, _temp1876); goto _LL1703; _LL1767: t= Cyc_Tcexp_tcTunion(
te, loc, topt, e, _temp1896, _temp1893, _temp1889, _temp1887, _temp1885); goto
_LL1703; _LL1769: t= Cyc_Tcexp_tcXTunion( te, loc, topt, _temp1905, _temp1901,
_temp1899, _temp1897); goto _LL1703; _LL1771: t=( void*)({ struct Cyc_Absyn_EnumType_struct*
_temp1930=( struct Cyc_Absyn_EnumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp1930[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp1931; _temp1931.tag= Cyc_Absyn_EnumType_tag;
_temp1931.f1=({ struct Cyc_Absyn_Enumdecl* _temp1932= _temp1908; if( _temp1932
== 0){ _throw( Null_Exception);} _temp1932->name;}); _temp1931.f2= _temp1908;
_temp1931;}); _temp1930;}); goto _LL1703; _LL1773: t= Cyc_Tcexp_tcMalloc( te,
loc, topt, _temp1914, _temp1912); goto _LL1703; _LL1703:;} e->topt=({ struct Cyc_Core_Opt*
_temp1933=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1933->v=( void*) t; _temp1933;});}