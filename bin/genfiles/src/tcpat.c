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
extern void* Cyc_Core_snd( struct _tuple0*); extern char Cyc_Core_InvalidArg[ 15u];
struct Cyc_Core_InvalidArg_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{ char* tag;
struct _tagged_string f1; } ; extern char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Core_Not_found[ 14u];
extern char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{ char*
tag; struct _tagged_string f1; } ; extern char* string_to_Cstring( struct
_tagged_string); extern char* underlying_Cstring( struct _tagged_string); extern
struct _tagged_string Cstring_to_string( char*); extern int system( char*);
struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List*
Cyc_List_glist_t; typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct
Cyc_List_List* Cyc_List_List_t; extern int Cyc_List_length( struct Cyc_List_List*
x); extern char Cyc_List_List_empty[ 15u]; extern struct Cyc_List_List* Cyc_List_map(
void*(* f)( void*), struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_rmap(
struct _RegionHandle*, void*(* f)( void*), struct Cyc_List_List* x); extern
struct Cyc_List_List* Cyc_List_map_c( void*(* f)( void*, void*), void* env,
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_rmap_c( struct
_RegionHandle*, void*(* f)( void*, void*), void* env, struct Cyc_List_List* x);
extern char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List* Cyc_List_rev(
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_imp_rev( struct
Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List*
x, struct Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_rappend(
struct _RegionHandle*, struct Cyc_List_List* x, struct Cyc_List_List* y); extern
char Cyc_List_Nth[ 8u]; extern struct Cyc_List_List* Cyc_List_rzip( struct
_RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List*
y); extern struct Cyc_List_List* Cyc_List_tabulate_c( int n, void*(* f)( void*,
int), void* env); struct Cyc_Dict_Dict; typedef struct Cyc_Dict_Dict* Cyc_Dict_hdict_t;
typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t; extern char Cyc_Dict_Present[ 12u];
extern char Cyc_Dict_Absent[ 11u]; struct Cyc_Set_Set; typedef struct Cyc_Set_Set*
Cyc_Set_gset_t; typedef struct Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set*
Cyc_Set_set_t; extern struct Cyc_Set_Set* Cyc_Set_empty( int(* comp)( void*,
void*)); extern struct Cyc_Set_Set* Cyc_Set_insert( struct Cyc_Set_Set* s, void*
elt); extern int Cyc_Set_cardinality( struct Cyc_Set_Set* s); extern int Cyc_Set_member(
struct Cyc_Set_Set* s, void* elt); extern char Cyc_Set_Absent[ 11u]; struct Cyc_Stdio___sFILE;
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; typedef int Cyc_Stdio_fpos_t;
extern char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Stdio_FileCloseError[
19u]; extern int Cyc_String_strcmp( struct _tagged_string s1, struct
_tagged_string s2); extern int Cyc_String_zstrcmp( struct _tagged_string, struct
_tagged_string); extern int Cyc_String_zstrptrcmp( struct _tagged_string*,
struct _tagged_string*); struct Cyc_Lineno_Pos{ struct _tagged_string
logical_file; struct _tagged_string line; int line_no; int col; } ; typedef
struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t; extern char Cyc_Position_Exit[ 9u];
struct Cyc_Position_Segment; typedef struct Cyc_Position_Segment* Cyc_Position_seg_t;
static const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1;
static const int Cyc_Position_Elab= 2; typedef void* Cyc_Position_error_kind_t;
struct Cyc_Position_Error{ struct _tagged_string source; struct Cyc_Position_Segment*
seg; void* kind; struct _tagged_string desc; } ; typedef struct Cyc_Position_Error*
Cyc_Position_error_t; extern char Cyc_Position_Nocontext[ 14u]; typedef struct
_tagged_string* Cyc_Absyn_field_name_t; typedef struct _tagged_string* Cyc_Absyn_var_t;
typedef struct _tagged_string* Cyc_Absyn_tvarname_t; typedef void* Cyc_Absyn_nmspace_t;
struct _tuple1{ void* f1; struct _tagged_string* f2; } ; typedef struct _tuple1*
Cyc_Absyn_qvar_t; typedef struct _tuple1* Cyc_Absyn_qvar_opt_t; typedef struct
_tuple1* Cyc_Absyn_typedef_name_t; typedef struct _tuple1* Cyc_Absyn_typedef_name_opt_t;
struct Cyc_Absyn_Tvar; struct Cyc_Absyn_Tqual; struct Cyc_Absyn_Conref; struct
Cyc_Absyn_PtrInfo; struct Cyc_Absyn_FnInfo; struct Cyc_Absyn_TunionInfo; struct
Cyc_Absyn_TunionFieldInfo; struct Cyc_Absyn_Exp; struct Cyc_Absyn_Stmt; struct
Cyc_Absyn_Pat; struct Cyc_Absyn_Switch_clause; struct Cyc_Absyn_Fndecl; struct
Cyc_Absyn_Structdecl; struct Cyc_Absyn_Uniondecl; struct Cyc_Absyn_Tuniondecl;
struct Cyc_Absyn_Tunionfield; struct Cyc_Absyn_Enumfield; struct Cyc_Absyn_Enumdecl;
struct Cyc_Absyn_Typedefdecl; struct Cyc_Absyn_Vardecl; struct Cyc_Absyn_Decl;
struct Cyc_Absyn_Structfield; typedef void* Cyc_Absyn_scope_t; typedef struct
Cyc_Absyn_Tqual Cyc_Absyn_tqual_t; typedef void* Cyc_Absyn_size_of_t; typedef
void* Cyc_Absyn_kind_t; typedef struct Cyc_Absyn_Tvar* Cyc_Absyn_tvar_t; typedef
void* Cyc_Absyn_sign_t; typedef struct Cyc_Absyn_Conref* Cyc_Absyn_conref_t;
typedef void* Cyc_Absyn_constraint_t; typedef void* Cyc_Absyn_bounds_t; typedef
struct Cyc_Absyn_PtrInfo Cyc_Absyn_ptr_info_t; typedef struct Cyc_Absyn_FnInfo
Cyc_Absyn_fn_info_t; typedef struct Cyc_Absyn_TunionInfo Cyc_Absyn_tunion_info_t;
typedef struct Cyc_Absyn_TunionFieldInfo Cyc_Absyn_tunion_field_info_t; typedef
void* Cyc_Absyn_type_t; typedef void* Cyc_Absyn_rgntype_t; typedef void* Cyc_Absyn_funcparams_t;
typedef void* Cyc_Absyn_type_modifier_t; typedef void* Cyc_Absyn_cnst_t; typedef
void* Cyc_Absyn_primop_t; typedef void* Cyc_Absyn_incrementor_t; typedef void*
Cyc_Absyn_raw_exp_t; typedef struct Cyc_Absyn_Exp* Cyc_Absyn_exp_t; typedef
struct Cyc_Absyn_Exp* Cyc_Absyn_exp_opt_t; typedef void* Cyc_Absyn_raw_stmt_t;
typedef struct Cyc_Absyn_Stmt* Cyc_Absyn_stmt_t; typedef struct Cyc_Absyn_Stmt*
Cyc_Absyn_stmt_opt_t; typedef void* Cyc_Absyn_raw_pat_t; typedef struct Cyc_Absyn_Pat*
Cyc_Absyn_pat_t; typedef void* Cyc_Absyn_binding_t; typedef struct Cyc_Absyn_Switch_clause*
Cyc_Absyn_switch_clause_t; typedef struct Cyc_Absyn_Fndecl* Cyc_Absyn_fndecl_t;
typedef struct Cyc_Absyn_Structdecl* Cyc_Absyn_structdecl_t; typedef struct Cyc_Absyn_Uniondecl*
Cyc_Absyn_uniondecl_t; typedef struct Cyc_Absyn_Tunionfield* Cyc_Absyn_tunionfield_t;
typedef struct Cyc_Absyn_Tuniondecl* Cyc_Absyn_tuniondecl_t; typedef struct Cyc_Absyn_Typedefdecl*
Cyc_Absyn_typedefdecl_t; typedef struct Cyc_Absyn_Enumfield* Cyc_Absyn_enumfield_t;
typedef struct Cyc_Absyn_Enumdecl* Cyc_Absyn_enumdecl_t; typedef struct Cyc_Absyn_Vardecl*
Cyc_Absyn_vardecl_t; typedef void* Cyc_Absyn_raw_decl_t; typedef struct Cyc_Absyn_Decl*
Cyc_Absyn_decl_t; typedef void* Cyc_Absyn_designator_t; typedef void* Cyc_Absyn_stmt_annot_t;
typedef void* Cyc_Absyn_attribute_t; typedef struct Cyc_List_List* Cyc_Absyn_attributes_t;
typedef struct Cyc_Absyn_Structfield* Cyc_Absyn_structfield_t; static const int
Cyc_Absyn_Loc_n= 0; static const int Cyc_Absyn_Rel_n= 0; struct Cyc_Absyn_Rel_n_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Abs_n= 1;
struct Cyc_Absyn_Abs_n_struct{ int tag; struct Cyc_List_List* f1; } ; static
const int Cyc_Absyn_Static= 0; static const int Cyc_Absyn_Abstract= 1; static
const int Cyc_Absyn_Public= 2; static const int Cyc_Absyn_Extern= 3; static
const int Cyc_Absyn_ExternC= 4; struct Cyc_Absyn_Tqual{ int q_const: 1; int
q_volatile: 1; int q_restrict: 1; } ; static const int Cyc_Absyn_B1= 0; static
const int Cyc_Absyn_B2= 1; static const int Cyc_Absyn_B4= 2; static const int
Cyc_Absyn_B8= 3; static const int Cyc_Absyn_AnyKind= 0; static const int Cyc_Absyn_MemKind=
1; static const int Cyc_Absyn_BoxKind= 2; static const int Cyc_Absyn_RgnKind= 3;
static const int Cyc_Absyn_EffKind= 4; static const int Cyc_Absyn_Signed= 0;
static const int Cyc_Absyn_Unsigned= 1; struct Cyc_Absyn_Conref{ void* v; } ;
static const int Cyc_Absyn_Eq_constr= 0; struct Cyc_Absyn_Eq_constr_struct{ int
tag; void* f1; } ; static const int Cyc_Absyn_Forward_constr= 1; struct Cyc_Absyn_Forward_constr_struct{
int tag; struct Cyc_Absyn_Conref* f1; } ; static const int Cyc_Absyn_No_constr=
0; struct Cyc_Absyn_Tvar{ struct _tagged_string* name; struct Cyc_Absyn_Conref*
kind; } ; static const int Cyc_Absyn_Unknown_b= 0; static const int Cyc_Absyn_Upper_b=
0; struct Cyc_Absyn_Upper_b_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
struct Cyc_Absyn_PtrInfo{ void* elt_typ; void* rgn_typ; struct Cyc_Absyn_Conref*
nullable; struct Cyc_Absyn_Tqual tq; struct Cyc_Absyn_Conref* bounds; } ; struct
Cyc_Absyn_FnInfo{ struct Cyc_List_List* tvars; struct Cyc_Core_Opt* effect; void*
ret_typ; struct Cyc_List_List* args; int varargs; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_UnknownTunionInfo{ struct _tuple1* name; int
is_xtunion; } ; static const int Cyc_Absyn_UnknownTunion= 0; struct Cyc_Absyn_UnknownTunion_struct{
int tag; struct Cyc_Absyn_UnknownTunionInfo f1; } ; static const int Cyc_Absyn_KnownTunion=
1; struct Cyc_Absyn_KnownTunion_struct{ int tag; struct Cyc_Absyn_Tuniondecl* f1;
} ; struct Cyc_Absyn_TunionInfo{ void* tunion_info; struct Cyc_List_List* targs;
void* rgn; } ; struct Cyc_Absyn_UnknownTunionFieldInfo{ struct _tuple1*
tunion_name; struct _tuple1* field_name; int is_xtunion; } ; static const int
Cyc_Absyn_UnknownTunionfield= 0; struct Cyc_Absyn_UnknownTunionfield_struct{ int
tag; struct Cyc_Absyn_UnknownTunionFieldInfo f1; } ; static const int Cyc_Absyn_KnownTunionfield=
1; struct Cyc_Absyn_KnownTunionfield_struct{ int tag; struct Cyc_Absyn_Tuniondecl*
f1; struct Cyc_Absyn_Tunionfield* f2; } ; struct Cyc_Absyn_TunionFieldInfo{ void*
field_info; struct Cyc_List_List* targs; } ; static const int Cyc_Absyn_VoidType=
0; static const int Cyc_Absyn_Evar= 0; struct Cyc_Absyn_Evar_struct{ int tag;
void* f1; struct Cyc_Core_Opt* f2; int f3; } ; static const int Cyc_Absyn_VarType=
1; struct Cyc_Absyn_VarType_struct{ int tag; struct Cyc_Absyn_Tvar* f1; } ;
static const int Cyc_Absyn_TunionType= 2; struct Cyc_Absyn_TunionType_struct{
int tag; struct Cyc_Absyn_TunionInfo f1; } ; static const int Cyc_Absyn_TunionFieldType=
3; struct Cyc_Absyn_TunionFieldType_struct{ int tag; struct Cyc_Absyn_TunionFieldInfo
f1; } ; static const int Cyc_Absyn_PointerType= 4; struct Cyc_Absyn_PointerType_struct{
int tag; struct Cyc_Absyn_PtrInfo f1; } ; static const int Cyc_Absyn_IntType= 5;
struct Cyc_Absyn_IntType_struct{ int tag; void* f1; void* f2; } ; static const
int Cyc_Absyn_FloatType= 1; static const int Cyc_Absyn_DoubleType= 2; static
const int Cyc_Absyn_ArrayType= 6; struct Cyc_Absyn_ArrayType_struct{ int tag;
void* f1; struct Cyc_Absyn_Tqual f2; struct Cyc_Absyn_Exp* f3; } ; static const
int Cyc_Absyn_FnType= 7; struct Cyc_Absyn_FnType_struct{ int tag; struct Cyc_Absyn_FnInfo
f1; } ; static const int Cyc_Absyn_TupleType= 8; struct Cyc_Absyn_TupleType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_StructType= 9;
struct Cyc_Absyn_StructType_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Structdecl** f3; } ; static const int Cyc_Absyn_UnionType=
10; struct Cyc_Absyn_UnionType_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Uniondecl** f3; } ; static const int Cyc_Absyn_EnumType= 11;
struct Cyc_Absyn_EnumType_struct{ int tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl*
f2; } ; static const int Cyc_Absyn_RgnHandleType= 12; struct Cyc_Absyn_RgnHandleType_struct{
int tag; void* f1; } ; static const int Cyc_Absyn_TypedefType= 13; struct Cyc_Absyn_TypedefType_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3;
} ; static const int Cyc_Absyn_HeapRgn= 3; static const int Cyc_Absyn_AccessEff=
14; struct Cyc_Absyn_AccessEff_struct{ int tag; void* f1; } ; static const int
Cyc_Absyn_JoinEff= 15; struct Cyc_Absyn_JoinEff_struct{ int tag; struct Cyc_List_List*
f1; } ; static const int Cyc_Absyn_NoTypes= 0; struct Cyc_Absyn_NoTypes_struct{
int tag; struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; } ; static
const int Cyc_Absyn_WithTypes= 1; struct Cyc_Absyn_WithTypes_struct{ int tag;
struct Cyc_List_List* f1; int f2; struct Cyc_Core_Opt* f3; } ; static const int
Cyc_Absyn_NonNullable_ps= 0; struct Cyc_Absyn_NonNullable_ps_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_Nullable_ps= 1; struct
Cyc_Absyn_Nullable_ps_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; static
const int Cyc_Absyn_TaggedArray_ps= 0; static const int Cyc_Absyn_Regparm_att= 0;
struct Cyc_Absyn_Regparm_att_struct{ int tag; int f1; } ; static const int Cyc_Absyn_Stdcall_att=
0; static const int Cyc_Absyn_Cdecl_att= 1; static const int Cyc_Absyn_Noreturn_att=
2; static const int Cyc_Absyn_Const_att= 3; static const int Cyc_Absyn_Aligned_att=
1; struct Cyc_Absyn_Aligned_att_struct{ int tag; int f1; } ; static const int
Cyc_Absyn_Packed_att= 4; static const int Cyc_Absyn_Section_att= 2; struct Cyc_Absyn_Section_att_struct{
int tag; struct _tagged_string f1; } ; static const int Cyc_Absyn_Nocommon_att=
5; static const int Cyc_Absyn_Shared_att= 6; static const int Cyc_Absyn_Unused_att=
7; static const int Cyc_Absyn_Weak_att= 8; static const int Cyc_Absyn_Dllimport_att=
9; static const int Cyc_Absyn_Dllexport_att= 10; static const int Cyc_Absyn_No_instrument_function_att=
11; static const int Cyc_Absyn_Constructor_att= 12; static const int Cyc_Absyn_Destructor_att=
13; static const int Cyc_Absyn_No_check_memory_usage_att= 14; static const int
Cyc_Absyn_Carray_mod= 0; static const int Cyc_Absyn_ConstArray_mod= 0; struct
Cyc_Absyn_ConstArray_mod_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; static
const int Cyc_Absyn_Pointer_mod= 1; struct Cyc_Absyn_Pointer_mod_struct{ int tag;
void* f1; void* f2; struct Cyc_Absyn_Tqual f3; } ; static const int Cyc_Absyn_Function_mod=
2; struct Cyc_Absyn_Function_mod_struct{ int tag; void* f1; } ; static const int
Cyc_Absyn_TypeParams_mod= 3; struct Cyc_Absyn_TypeParams_mod_struct{ int tag;
struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; int f3; } ; static
const int Cyc_Absyn_Attributes_mod= 4; struct Cyc_Absyn_Attributes_mod_struct{
int tag; struct Cyc_Position_Segment* f1; struct Cyc_List_List* f2; } ; static
const int Cyc_Absyn_Char_c= 0; struct Cyc_Absyn_Char_c_struct{ int tag; void* f1;
char f2; } ; static const int Cyc_Absyn_Short_c= 1; struct Cyc_Absyn_Short_c_struct{
int tag; void* f1; short f2; } ; static const int Cyc_Absyn_Int_c= 2; struct Cyc_Absyn_Int_c_struct{
int tag; void* f1; int f2; } ; static const int Cyc_Absyn_LongLong_c= 3; struct
Cyc_Absyn_LongLong_c_struct{ int tag; void* f1; long long f2; } ; static const
int Cyc_Absyn_Float_c= 4; struct Cyc_Absyn_Float_c_struct{ int tag; struct
_tagged_string f1; } ; static const int Cyc_Absyn_String_c= 5; struct Cyc_Absyn_String_c_struct{
int tag; struct _tagged_string f1; } ; static const int Cyc_Absyn_Null_c= 0;
static const int Cyc_Absyn_Plus= 0; static const int Cyc_Absyn_Times= 1; static
const int Cyc_Absyn_Minus= 2; static const int Cyc_Absyn_Div= 3; static const
int Cyc_Absyn_Mod= 4; static const int Cyc_Absyn_Eq= 5; static const int Cyc_Absyn_Neq=
6; static const int Cyc_Absyn_Gt= 7; static const int Cyc_Absyn_Lt= 8; static
const int Cyc_Absyn_Gte= 9; static const int Cyc_Absyn_Lte= 10; static const int
Cyc_Absyn_Not= 11; static const int Cyc_Absyn_Bitnot= 12; static const int Cyc_Absyn_Bitand=
13; static const int Cyc_Absyn_Bitor= 14; static const int Cyc_Absyn_Bitxor= 15;
static const int Cyc_Absyn_Bitlshift= 16; static const int Cyc_Absyn_Bitlrshift=
17; static const int Cyc_Absyn_Bitarshift= 18; static const int Cyc_Absyn_Size=
19; static const int Cyc_Absyn_Printf= 20; static const int Cyc_Absyn_Fprintf=
21; static const int Cyc_Absyn_Xprintf= 22; static const int Cyc_Absyn_Scanf= 23;
static const int Cyc_Absyn_Fscanf= 24; static const int Cyc_Absyn_Sscanf= 25;
static const int Cyc_Absyn_PreInc= 0; static const int Cyc_Absyn_PostInc= 1;
static const int Cyc_Absyn_PreDec= 2; static const int Cyc_Absyn_PostDec= 3;
static const int Cyc_Absyn_Const_e= 0; struct Cyc_Absyn_Const_e_struct{ int tag;
void* f1; } ; static const int Cyc_Absyn_Var_e= 1; struct Cyc_Absyn_Var_e_struct{
int tag; struct _tuple1* f1; void* f2; } ; static const int Cyc_Absyn_UnknownId_e=
2; struct Cyc_Absyn_UnknownId_e_struct{ int tag; struct _tuple1* f1; } ; static
const int Cyc_Absyn_Primop_e= 3; struct Cyc_Absyn_Primop_e_struct{ int tag; void*
f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_AssignOp_e= 4;
struct Cyc_Absyn_AssignOp_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
Cyc_Core_Opt* f2; struct Cyc_Absyn_Exp* f3; } ; static const int Cyc_Absyn_Increment_e=
5; struct Cyc_Absyn_Increment_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; void*
f2; } ; static const int Cyc_Absyn_Conditional_e= 6; struct Cyc_Absyn_Conditional_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp*
f3; } ; static const int Cyc_Absyn_SeqExp_e= 7; struct Cyc_Absyn_SeqExp_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp* f2; } ; static const
int Cyc_Absyn_UnknownCall_e= 8; struct Cyc_Absyn_UnknownCall_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_FnCall_e=
9; struct Cyc_Absyn_FnCall_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
Cyc_List_List* f2; } ; static const int Cyc_Absyn_Throw_e= 10; struct Cyc_Absyn_Throw_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_NoInstantiate_e=
11; struct Cyc_Absyn_NoInstantiate_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
} ; static const int Cyc_Absyn_Instantiate_e= 12; struct Cyc_Absyn_Instantiate_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_Cast_e= 13; struct Cyc_Absyn_Cast_e_struct{ int tag; void* f1;
struct Cyc_Absyn_Exp* f2; } ; static const int Cyc_Absyn_Address_e= 14; struct
Cyc_Absyn_Address_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; static const
int Cyc_Absyn_New_e= 15; struct Cyc_Absyn_New_e_struct{ int tag; struct Cyc_Absyn_Exp*
f1; struct Cyc_Absyn_Exp* f2; } ; static const int Cyc_Absyn_Sizeoftyp_e= 16;
struct Cyc_Absyn_Sizeoftyp_e_struct{ int tag; void* f1; } ; static const int Cyc_Absyn_Sizeofexp_e=
17; struct Cyc_Absyn_Sizeofexp_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_Deref_e= 18; struct Cyc_Absyn_Deref_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_StructMember_e= 19;
struct Cyc_Absyn_StructMember_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct _tagged_string* f2; } ; static const int Cyc_Absyn_StructArrow_e= 20;
struct Cyc_Absyn_StructArrow_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
_tagged_string* f2; } ; static const int Cyc_Absyn_Subscript_e= 21; struct Cyc_Absyn_Subscript_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp* f2; } ; static const
int Cyc_Absyn_Tuple_e= 22; struct Cyc_Absyn_Tuple_e_struct{ int tag; struct Cyc_List_List*
f1; } ; static const int Cyc_Absyn_CompoundLit_e= 23; struct _tuple2{ struct Cyc_Core_Opt*
f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; struct Cyc_Absyn_CompoundLit_e_struct{
int tag; struct _tuple2* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Array_e=
24; struct Cyc_Absyn_Array_e_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_Comprehension_e= 25; struct Cyc_Absyn_Comprehension_e_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp*
f3; } ; static const int Cyc_Absyn_Struct_e= 26; struct Cyc_Absyn_Struct_e_struct{
int tag; struct _tuple1* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List* f3;
struct Cyc_Absyn_Structdecl* f4; } ; static const int Cyc_Absyn_Tunion_e= 27;
struct Cyc_Absyn_Tunion_e_struct{ int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield*
f5; } ; static const int Cyc_Absyn_Enum_e= 28; struct Cyc_Absyn_Enum_e_struct{
int tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
f3; } ; static const int Cyc_Absyn_Malloc_e= 29; struct Cyc_Absyn_Malloc_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; void* f2; } ; static const int Cyc_Absyn_UnresolvedMem_e=
30; struct Cyc_Absyn_UnresolvedMem_e_struct{ int tag; struct Cyc_Core_Opt* f1;
struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_StmtExp_e= 31; struct
Cyc_Absyn_StmtExp_e_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; static const
int Cyc_Absyn_Codegen_e= 32; struct Cyc_Absyn_Codegen_e_struct{ int tag; struct
Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Fill_e= 33; struct Cyc_Absyn_Fill_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; struct Cyc_Absyn_Exp{ struct Cyc_Core_Opt*
topt; void* r; struct Cyc_Position_Segment* loc; } ; static const int Cyc_Absyn_Skip_s=
0; static const int Cyc_Absyn_Exp_s= 0; struct Cyc_Absyn_Exp_s_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_Seq_s= 1; struct Cyc_Absyn_Seq_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; struct Cyc_Absyn_Stmt* f2; } ; static const
int Cyc_Absyn_Return_s= 2; struct Cyc_Absyn_Return_s_struct{ int tag; struct Cyc_Absyn_Exp*
f1; } ; static const int Cyc_Absyn_IfThenElse_s= 3; struct Cyc_Absyn_IfThenElse_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; struct Cyc_Absyn_Stmt*
f3; } ; static const int Cyc_Absyn_While_s= 4; struct _tuple3{ struct Cyc_Absyn_Exp*
f1; struct Cyc_Absyn_Stmt* f2; } ; struct Cyc_Absyn_While_s_struct{ int tag;
struct _tuple3 f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Break_s=
5; struct Cyc_Absyn_Break_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ;
static const int Cyc_Absyn_Continue_s= 6; struct Cyc_Absyn_Continue_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Absyn_Goto_s= 7;
struct Cyc_Absyn_Goto_s_struct{ int tag; struct _tagged_string* f1; struct Cyc_Absyn_Stmt*
f2; } ; static const int Cyc_Absyn_For_s= 8; struct Cyc_Absyn_For_s_struct{ int
tag; struct Cyc_Absyn_Exp* f1; struct _tuple3 f2; struct _tuple3 f3; struct Cyc_Absyn_Stmt*
f4; } ; static const int Cyc_Absyn_Switch_s= 9; struct Cyc_Absyn_Switch_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_Fallthru_s= 10; struct Cyc_Absyn_Fallthru_s_struct{ int tag;
struct Cyc_List_List* f1; struct Cyc_Absyn_Switch_clause** f2; } ; static const
int Cyc_Absyn_Decl_s= 11; struct Cyc_Absyn_Decl_s_struct{ int tag; struct Cyc_Absyn_Decl*
f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Cut_s= 12; struct
Cyc_Absyn_Cut_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; static const int
Cyc_Absyn_Splice_s= 13; struct Cyc_Absyn_Splice_s_struct{ int tag; struct Cyc_Absyn_Stmt*
f1; } ; static const int Cyc_Absyn_Label_s= 14; struct Cyc_Absyn_Label_s_struct{
int tag; struct _tagged_string* f1; struct Cyc_Absyn_Stmt* f2; } ; static const
int Cyc_Absyn_Do_s= 15; struct Cyc_Absyn_Do_s_struct{ int tag; struct Cyc_Absyn_Stmt*
f1; struct _tuple3 f2; } ; static const int Cyc_Absyn_TryCatch_s= 16; struct Cyc_Absyn_TryCatch_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_Region_s= 17; struct Cyc_Absyn_Region_s_struct{ int tag; struct
Cyc_Absyn_Tvar* f1; struct Cyc_Absyn_Vardecl* f2; struct Cyc_Absyn_Stmt* f3; } ;
struct Cyc_Absyn_Stmt{ void* r; struct Cyc_Position_Segment* loc; struct Cyc_List_List*
non_local_preds; int try_depth; void* annot; } ; static const int Cyc_Absyn_Wild_p=
0; static const int Cyc_Absyn_Var_p= 0; struct Cyc_Absyn_Var_p_struct{ int tag;
struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_Absyn_Null_p= 1; static
const int Cyc_Absyn_Int_p= 1; struct Cyc_Absyn_Int_p_struct{ int tag; void* f1;
int f2; } ; static const int Cyc_Absyn_Char_p= 2; struct Cyc_Absyn_Char_p_struct{
int tag; char f1; } ; static const int Cyc_Absyn_Float_p= 3; struct Cyc_Absyn_Float_p_struct{
int tag; struct _tagged_string f1; } ; static const int Cyc_Absyn_Tuple_p= 4;
struct Cyc_Absyn_Tuple_p_struct{ int tag; struct Cyc_List_List* f1; } ; static
const int Cyc_Absyn_Pointer_p= 5; struct Cyc_Absyn_Pointer_p_struct{ int tag;
struct Cyc_Absyn_Pat* f1; } ; static const int Cyc_Absyn_Reference_p= 6; struct
Cyc_Absyn_Reference_p_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ; static
const int Cyc_Absyn_Struct_p= 7; struct Cyc_Absyn_Struct_p_struct{ int tag;
struct Cyc_Absyn_Structdecl* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List*
f3; struct Cyc_List_List* f4; } ; static const int Cyc_Absyn_Tunion_p= 8; struct
Cyc_Absyn_Tunion_p_struct{ int tag; struct Cyc_Absyn_Tuniondecl* f1; struct Cyc_Absyn_Tunionfield*
f2; struct Cyc_List_List* f3; struct Cyc_List_List* f4; } ; static const int Cyc_Absyn_Enum_p=
9; struct Cyc_Absyn_Enum_p_struct{ int tag; struct Cyc_Absyn_Enumdecl* f1;
struct Cyc_Absyn_Enumfield* f2; } ; static const int Cyc_Absyn_UnknownId_p= 10;
struct Cyc_Absyn_UnknownId_p_struct{ int tag; struct _tuple1* f1; } ; static
const int Cyc_Absyn_UnknownCall_p= 11; struct Cyc_Absyn_UnknownCall_p_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
} ; static const int Cyc_Absyn_UnknownFields_p= 12; struct Cyc_Absyn_UnknownFields_p_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
} ; struct Cyc_Absyn_Pat{ void* r; struct Cyc_Core_Opt* topt; struct Cyc_Position_Segment*
loc; } ; struct Cyc_Absyn_Switch_clause{ struct Cyc_Absyn_Pat* pattern; struct
Cyc_Core_Opt* pat_vars; struct Cyc_Absyn_Exp* where_clause; struct Cyc_Absyn_Stmt*
body; struct Cyc_Position_Segment* loc; } ; static const int Cyc_Absyn_Unresolved_b=
0; static const int Cyc_Absyn_Global_b= 0; struct Cyc_Absyn_Global_b_struct{ int
tag; struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_Absyn_Funname_b= 1;
struct Cyc_Absyn_Funname_b_struct{ int tag; struct Cyc_Absyn_Fndecl* f1; } ;
static const int Cyc_Absyn_Param_b= 2; struct Cyc_Absyn_Param_b_struct{ int tag;
struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_Absyn_Local_b= 3; struct
Cyc_Absyn_Local_b_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ; static
const int Cyc_Absyn_Pat_b= 4; struct Cyc_Absyn_Pat_b_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; } ; struct Cyc_Absyn_Vardecl{ void* sc; struct _tuple1* name; struct Cyc_Absyn_Tqual
tq; void* type; struct Cyc_Absyn_Exp* initializer; struct Cyc_Core_Opt* rgn;
struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Fndecl{ void* sc; int
is_inline; struct _tuple1* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
effect; void* ret_type; struct Cyc_List_List* args; int varargs; struct Cyc_Absyn_Stmt*
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
struct _tuple1* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt* fields;
int is_xtunion; } ; struct Cyc_Absyn_Enumfield{ struct _tuple1* name; struct Cyc_Absyn_Exp*
tag; struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_Enumdecl{ void* sc;
struct _tuple1* name; struct Cyc_List_List* fields; } ; struct Cyc_Absyn_Typedefdecl{
struct _tuple1* name; struct Cyc_List_List* tvs; void* defn; } ; static const
int Cyc_Absyn_Var_d= 0; struct Cyc_Absyn_Var_d_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; } ; static const int Cyc_Absyn_Fn_d= 1; struct Cyc_Absyn_Fn_d_struct{ int
tag; struct Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Let_d= 2;
struct Cyc_Absyn_Let_d_struct{ int tag; struct Cyc_Absyn_Pat* f1; struct Cyc_Core_Opt*
f2; struct Cyc_Core_Opt* f3; struct Cyc_Absyn_Exp* f4; int f5; } ; static const
int Cyc_Absyn_Struct_d= 3; struct Cyc_Absyn_Struct_d_struct{ int tag; struct Cyc_Absyn_Structdecl*
f1; } ; static const int Cyc_Absyn_Union_d= 4; struct Cyc_Absyn_Union_d_struct{
int tag; struct Cyc_Absyn_Uniondecl* f1; } ; static const int Cyc_Absyn_Tunion_d=
5; struct Cyc_Absyn_Tunion_d_struct{ int tag; struct Cyc_Absyn_Tuniondecl* f1; }
; static const int Cyc_Absyn_Enum_d= 6; struct Cyc_Absyn_Enum_d_struct{ int tag;
struct Cyc_Absyn_Enumdecl* f1; } ; static const int Cyc_Absyn_Typedef_d= 7;
struct Cyc_Absyn_Typedef_d_struct{ int tag; struct Cyc_Absyn_Typedefdecl* f1; }
; static const int Cyc_Absyn_Namespace_d= 8; struct Cyc_Absyn_Namespace_d_struct{
int tag; struct _tagged_string* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_Using_d= 9; struct Cyc_Absyn_Using_d_struct{ int tag; struct
_tuple1* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d=
10; struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ;
struct Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc; } ; static
const int Cyc_Absyn_ArrayElement= 0; struct Cyc_Absyn_ArrayElement_struct{ int
tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_FieldName= 1;
struct Cyc_Absyn_FieldName_struct{ int tag; struct _tagged_string* f1; } ;
extern char Cyc_Absyn_EmptyAnnot[ 15u]; extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();
extern struct Cyc_Absyn_Conref* Cyc_Absyn_new_conref( void* x); extern struct
Cyc_Absyn_Conref* Cyc_Absyn_empty_conref(); extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_new_evar( void*); extern
void* Cyc_Absyn_wildtyp(); extern void* Cyc_Absyn_uchar_t; extern void* Cyc_Absyn_uint_t;
extern void* Cyc_Absyn_sint_t; extern void* Cyc_Absyn_float_t; extern struct Cyc_Absyn_Vardecl*
Cyc_Absyn_new_vardecl( struct _tuple1* x, void* t, struct Cyc_Absyn_Exp* init);
struct Cyc_PP_Ppstate; typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t; struct
Cyc_PP_Out; typedef struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc; typedef
struct Cyc_PP_Doc* Cyc_PP_doc_t; extern struct _tagged_string Cyc_Absynpp_typ2string(
void*); extern struct _tagged_string Cyc_Absynpp_kind2string( void*); extern
struct _tagged_string Cyc_Absynpp_qvar2string( struct _tuple1*); static const
int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{ int tag; void* f1; } ;
static const int Cyc_Tcenv_StructRes= 1; struct Cyc_Tcenv_StructRes_struct{ int
tag; struct Cyc_Absyn_Structdecl* f1; } ; static const int Cyc_Tcenv_TunionRes=
2; struct Cyc_Tcenv_TunionRes_struct{ int tag; struct Cyc_Absyn_Tuniondecl* f1;
struct Cyc_Absyn_Tunionfield* f2; } ; static const int Cyc_Tcenv_EnumRes= 3;
struct Cyc_Tcenv_EnumRes_struct{ int tag; struct Cyc_Absyn_Enumdecl* f1; struct
Cyc_Absyn_Enumfield* f2; } ; typedef void* Cyc_Tcenv_resolved_t; struct Cyc_Tcenv_Genv{
struct Cyc_Set_Set* namespaces; struct Cyc_Dict_Dict* structdecls; struct Cyc_Dict_Dict*
uniondecls; struct Cyc_Dict_Dict* tuniondecls; struct Cyc_Dict_Dict* enumdecls;
struct Cyc_Dict_Dict* typedefs; struct Cyc_Dict_Dict* ordinaries; struct Cyc_List_List*
availables; } ; typedef struct Cyc_Tcenv_Genv* Cyc_Tcenv_genv_t; struct Cyc_Tcenv_Fenv;
typedef struct Cyc_Tcenv_Fenv* Cyc_Tcenv_fenv_t; static const int Cyc_Tcenv_NotLoop_j=
0; static const int Cyc_Tcenv_CaseEnd_j= 1; static const int Cyc_Tcenv_FnEnd_j=
2; static const int Cyc_Tcenv_Stmt_j= 0; struct Cyc_Tcenv_Stmt_j_struct{ int tag;
struct Cyc_Absyn_Stmt* f1; } ; typedef void* Cyc_Tcenv_jumpee_t; static const
int Cyc_Tcenv_Outermost= 0; struct Cyc_Tcenv_Outermost_struct{ int tag; void* f1;
} ; static const int Cyc_Tcenv_Frame= 1; struct Cyc_Tcenv_Frame_struct{ int tag;
void* f1; void* f2; } ; static const int Cyc_Tcenv_Hidden= 2; struct Cyc_Tcenv_Hidden_struct{
int tag; void* f1; void* f2; } ; typedef void* Cyc_Tcenv_frames_t; struct Cyc_Tcenv_Tenv{
struct Cyc_List_List* ns; struct Cyc_Dict_Dict* ae; struct Cyc_Core_Opt* le; } ;
typedef struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tenv_t; extern void* Cyc_Tcenv_lookup_ordinary(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple1*); extern
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_type_vars( struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*, struct Cyc_List_List*); extern void Cyc_Tcenv_check_rgn_accessible(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, void* rgn); extern char
Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_impos( struct _tagged_string);
extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*, struct _tagged_string);
extern void Cyc_Tcutil_warn( struct Cyc_Position_Segment*, struct _tagged_string);
extern void* Cyc_Tcutil_compress( void* t); extern int Cyc_Tcutil_coerceable(
void*); extern int Cyc_Tcutil_unify( void*, void*); extern void* Cyc_Tcutil_rsubstitute(
struct _RegionHandle*, struct Cyc_List_List*, void*); struct _tuple4{ struct Cyc_Absyn_Tvar*
f1; void* f2; } ; extern struct _tuple4* Cyc_Tcutil_r_make_inst_var( struct
_RegionHandle*, struct Cyc_Absyn_Tvar*); extern void Cyc_Tcutil_check_unique_vars(
struct Cyc_List_List* vs, struct Cyc_Position_Segment* loc, struct
_tagged_string err_msg); extern struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_Absyn_Structdecl* sd); struct _tuple5{ struct Cyc_List_List* f1;
struct Cyc_List_List* f2; } ; extern struct _tuple5 Cyc_Tcpat_tcPat( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Pat* p, void** topt, void** region_opt); extern void Cyc_Tcpat_check_pat_regions(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat* p); extern void Cyc_Tcpat_check_switch_exhaustive(
struct Cyc_Position_Segment*, struct Cyc_List_List*); extern int Cyc_Tcpat_check_let_pat_exhaustive(
struct Cyc_Position_Segment*, struct Cyc_Absyn_Pat* p); extern void Cyc_Tcpat_check_catch_overlap(
struct Cyc_Position_Segment*, struct Cyc_List_List*); struct _tuple6{ struct Cyc_List_List*
f1; struct Cyc_Absyn_Pat* f2; } ; static void Cyc_Tcpat_resolve_pat( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Pat* p){ void* _temp0=( void*) p->r; struct _tuple1*
_temp10; struct Cyc_List_List* _temp12; struct Cyc_List_List* _temp14; struct
_tuple1* _temp16; struct Cyc_List_List* _temp18; struct Cyc_List_List* _temp20;
struct _tuple1* _temp22; _LL2: if(( unsigned int) _temp0 > 2u?*(( int*) _temp0)
== Cyc_Absyn_UnknownId_p: 0){ _LL11: _temp10=( struct _tuple1*)(( struct Cyc_Absyn_UnknownId_p_struct*)
_temp0)->f1; goto _LL3;} else{ goto _LL4;} _LL4: if(( unsigned int) _temp0 > 2u?*((
int*) _temp0) == Cyc_Absyn_UnknownCall_p: 0){ _LL17: _temp16=( struct _tuple1*)((
struct Cyc_Absyn_UnknownCall_p_struct*) _temp0)->f1; goto _LL15; _LL15: _temp14=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp0)->f2;
goto _LL13; _LL13: _temp12=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp0)->f3; goto _LL5;} else{ goto _LL6;} _LL6: if(( unsigned int) _temp0 > 2u?*((
int*) _temp0) == Cyc_Absyn_UnknownFields_p: 0){ _LL23: _temp22=( struct _tuple1*)((
struct Cyc_Absyn_UnknownFields_p_struct*) _temp0)->f1; goto _LL21; _LL21:
_temp20=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp0)->f2; goto _LL19; _LL19: _temp18=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp0)->f3; goto _LL7;} else{ goto _LL8;} _LL8: goto _LL9; _LL3:{ struct
_handler_cons _temp24; _push_handler(& _temp24);{ void* _temp25=( void*) setjmp(
_temp24.handler); if( ! _temp25){{ void* _temp26= Cyc_Tcenv_lookup_ordinary( te,
p->loc, _temp10); struct Cyc_Absyn_Structdecl* _temp36; struct Cyc_Absyn_Tunionfield*
_temp38; struct Cyc_Absyn_Tuniondecl* _temp40; struct Cyc_Absyn_Enumfield*
_temp42; struct Cyc_Absyn_Enumdecl* _temp44; void* _temp46; _LL28: if(*(( int*)
_temp26) == Cyc_Tcenv_StructRes){ _LL37: _temp36=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Tcenv_StructRes_struct*) _temp26)->f1; goto _LL29;} else{ goto _LL30;}
_LL30: if(*(( int*) _temp26) == Cyc_Tcenv_TunionRes){ _LL41: _temp40=( struct
Cyc_Absyn_Tuniondecl*)(( struct Cyc_Tcenv_TunionRes_struct*) _temp26)->f1; goto
_LL39; _LL39: _temp38=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Tcenv_TunionRes_struct*)
_temp26)->f2; goto _LL31;} else{ goto _LL32;} _LL32: if(*(( int*) _temp26) ==
Cyc_Tcenv_EnumRes){ _LL45: _temp44=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Tcenv_EnumRes_struct*)
_temp26)->f1; goto _LL43; _LL43: _temp42=( struct Cyc_Absyn_Enumfield*)(( struct
Cyc_Tcenv_EnumRes_struct*) _temp26)->f2; goto _LL33;} else{ goto _LL34;} _LL34:
if(*(( int*) _temp26) == Cyc_Tcenv_VarRes){ _LL47: _temp46=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp26)->f1; goto _LL35;} else{ goto _LL27;} _LL29: Cyc_Tcutil_terr( p->loc,(
struct _tagged_string)({ char* _temp48=( char*)"struct tag used without arguments in pattern";
struct _tagged_string _temp49; _temp49.curr= _temp48; _temp49.base= _temp48;
_temp49.last_plus_one= _temp48 + 45; _temp49;})); _npop_handler( 0u); return;
_LL31:( void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Tunion_p_struct*
_temp50=( struct Cyc_Absyn_Tunion_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_p_struct));
_temp50[ 0]=({ struct Cyc_Absyn_Tunion_p_struct _temp51; _temp51.tag= Cyc_Absyn_Tunion_p;
_temp51.f1= _temp40; _temp51.f2= _temp38; _temp51.f3= 0; _temp51.f4= 0; _temp51;});
_temp50;}))); _npop_handler( 0u); return; _LL33:( void*)( p->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_p_struct* _temp52=( struct Cyc_Absyn_Enum_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_p_struct)); _temp52[ 0]=({ struct Cyc_Absyn_Enum_p_struct
_temp53; _temp53.tag= Cyc_Absyn_Enum_p; _temp53.f1= _temp44; _temp53.f2= _temp42;
_temp53;}); _temp52;}))); _npop_handler( 0u); return; _LL35: goto _LL27; _LL27:;};
_pop_handler();} else{ void* _temp55= _temp25; _LL57: if( _temp55 == Cyc_Dict_Absent){
goto _LL58;} else{ goto _LL59;} _LL59: goto _LL60; _LL58: goto _LL56; _LL60:(
void) _throw( _temp55); _LL56:;}}}{ void* _temp61=(* _temp10).f1; struct Cyc_List_List*
_temp69; _LL63: if( _temp61 ==( void*) Cyc_Absyn_Loc_n){ goto _LL64;} else{ goto
_LL65;} _LL65: if(( unsigned int) _temp61 > 1u?*(( int*) _temp61) == Cyc_Absyn_Rel_n:
0){ _LL70: _temp69=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp61)->f1; if( _temp69 == 0){ goto _LL66;} else{ goto _LL67;}} else{ goto
_LL67;} _LL67: goto _LL68; _LL64: goto _LL66; _LL66:(* _temp10).f1=( void*) Cyc_Absyn_Loc_n;(
void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Var_p_struct* _temp71=( struct
Cyc_Absyn_Var_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct));
_temp71[ 0]=({ struct Cyc_Absyn_Var_p_struct _temp72; _temp72.tag= Cyc_Absyn_Var_p;
_temp72.f1= Cyc_Absyn_new_vardecl( _temp10,( void*) Cyc_Absyn_VoidType, 0);
_temp72;}); _temp71;}))); return; _LL68: Cyc_Tcutil_terr( p->loc,( struct
_tagged_string)({ char* _temp73=( char*)"qualified variable in pattern"; struct
_tagged_string _temp74; _temp74.curr= _temp73; _temp74.base= _temp73; _temp74.last_plus_one=
_temp73 + 30; _temp74;})); return; _LL62:;} _LL5:{ struct _handler_cons _temp75;
_push_handler(& _temp75);{ void* _temp76=( void*) setjmp( _temp75.handler); if(
! _temp76){{ void* _temp77= Cyc_Tcenv_lookup_ordinary( te, p->loc, _temp16);
struct Cyc_Absyn_Structdecl* _temp87; struct Cyc_Absyn_Tunionfield* _temp89;
struct Cyc_Absyn_Tuniondecl* _temp91; struct Cyc_Absyn_Enumfield* _temp93;
struct Cyc_Absyn_Enumdecl* _temp95; void* _temp97; _LL79: if(*(( int*) _temp77)
== Cyc_Tcenv_StructRes){ _LL88: _temp87=( struct Cyc_Absyn_Structdecl*)(( struct
Cyc_Tcenv_StructRes_struct*) _temp77)->f1; goto _LL80;} else{ goto _LL81;} _LL81:
if(*(( int*) _temp77) == Cyc_Tcenv_TunionRes){ _LL92: _temp91=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Tcenv_TunionRes_struct*) _temp77)->f1; goto _LL90; _LL90: _temp89=(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Tcenv_TunionRes_struct*) _temp77)->f2;
goto _LL82;} else{ goto _LL83;} _LL83: if(*(( int*) _temp77) == Cyc_Tcenv_EnumRes){
_LL96: _temp95=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Tcenv_EnumRes_struct*)
_temp77)->f1; goto _LL94; _LL94: _temp93=( struct Cyc_Absyn_Enumfield*)(( struct
Cyc_Tcenv_EnumRes_struct*) _temp77)->f2; goto _LL84;} else{ goto _LL85;} _LL85:
if(*(( int*) _temp77) == Cyc_Tcenv_VarRes){ _LL98: _temp97=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp77)->f1; goto _LL86;} else{ goto _LL78;} _LL80: { struct Cyc_List_List*
new_ps= 0; for( 0; _temp12 != 0; _temp12=({ struct Cyc_List_List* _temp99=
_temp12; if( _temp99 == 0){ _throw( Null_Exception);} _temp99->tl;})){ new_ps=({
struct Cyc_List_List* _temp100=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp100->hd=( void*)({ struct _tuple6* _temp101=(
struct _tuple6*) GC_malloc( sizeof( struct _tuple6)); _temp101->f1= 0; _temp101->f2=(
struct Cyc_Absyn_Pat*)({ struct Cyc_List_List* _temp102= _temp12; if( _temp102
== 0){ _throw( Null_Exception);} _temp102->hd;}); _temp101;}); _temp100->tl=
new_ps; _temp100;});}( void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_p_struct*
_temp103=( struct Cyc_Absyn_Struct_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_p_struct));
_temp103[ 0]=({ struct Cyc_Absyn_Struct_p_struct _temp104; _temp104.tag= Cyc_Absyn_Struct_p;
_temp104.f1= _temp87; _temp104.f2= 0; _temp104.f3= _temp14; _temp104.f4=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( new_ps);
_temp104;}); _temp103;}))); _npop_handler( 0u); return;} _LL82:( void*)( p->r=(
void*)(( void*)({ struct Cyc_Absyn_Tunion_p_struct* _temp105=( struct Cyc_Absyn_Tunion_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunion_p_struct)); _temp105[ 0]=({ struct
Cyc_Absyn_Tunion_p_struct _temp106; _temp106.tag= Cyc_Absyn_Tunion_p; _temp106.f1=
_temp91; _temp106.f2= _temp89; _temp106.f3= _temp14; _temp106.f4= _temp12;
_temp106;}); _temp105;}))); _npop_handler( 0u); return; _LL84: Cyc_Tcutil_terr(
p->loc,({ struct _tagged_string _temp107= Cyc_Absynpp_qvar2string( _temp16);
xprintf("bad enum constructor %.*s in pattern", _temp107.last_plus_one -
_temp107.curr, _temp107.curr);})); _npop_handler( 0u); return; _LL86: goto _LL78;
_LL78:;}; _pop_handler();} else{ void* _temp109= _temp76; _LL111: if( _temp109
== Cyc_Dict_Absent){ goto _LL112;} else{ goto _LL113;} _LL113: goto _LL114;
_LL112: goto _LL110; _LL114:( void) _throw( _temp109); _LL110:;}}} Cyc_Tcutil_terr(
p->loc,({ struct _tagged_string _temp115= Cyc_Absynpp_qvar2string( _temp16);
xprintf("%.*s is not a constructor in pattern", _temp115.last_plus_one -
_temp115.curr, _temp115.curr);})); return; _LL7:{ struct _handler_cons _temp116;
_push_handler(& _temp116);{ void* _temp117=( void*) setjmp( _temp116.handler);
if( ! _temp117){{ void* _temp118= Cyc_Tcenv_lookup_ordinary( te, p->loc, _temp22);
struct Cyc_Absyn_Structdecl* _temp124; _LL120: if(*(( int*) _temp118) == Cyc_Tcenv_StructRes){
_LL125: _temp124=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Tcenv_StructRes_struct*)
_temp118)->f1; goto _LL121;} else{ goto _LL122;} _LL122: goto _LL123; _LL121:(
void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_p_struct* _temp126=(
struct Cyc_Absyn_Struct_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_p_struct));
_temp126[ 0]=({ struct Cyc_Absyn_Struct_p_struct _temp127; _temp127.tag= Cyc_Absyn_Struct_p;
_temp127.f1= _temp124; _temp127.f2= 0; _temp127.f3= _temp20; _temp127.f4=
_temp18; _temp127;}); _temp126;}))); _npop_handler( 0u); return; _LL123: goto
_LL119; _LL119:;}; _pop_handler();} else{ void* _temp129= _temp117; _LL131: if(
_temp129 == Cyc_Dict_Absent){ goto _LL132;} else{ goto _LL133;} _LL133: goto
_LL134; _LL132: goto _LL130; _LL134:( void) _throw( _temp129); _LL130:;}}} Cyc_Tcutil_terr(
p->loc,({ struct _tagged_string _temp135= Cyc_Absynpp_qvar2string( _temp22);
xprintf("%.*s is not a struct name, but pattern has designators", _temp135.last_plus_one
- _temp135.curr, _temp135.curr);})); return; _LL9: return; _LL1:;} static struct
_tagged_string* Cyc_Tcpat_get_name( struct Cyc_Absyn_Vardecl* vd){ return(* vd->name).f2;}
static void* Cyc_Tcpat_any_type( void** topt){ if( topt != 0){ return({ void**
_temp136= topt; if( _temp136 == 0){ _throw( Null_Exception);}* _temp136;});}
return Cyc_Absyn_new_evar(( void*) Cyc_Absyn_MemKind);} static void* Cyc_Tcpat_num_type(
void** topt, void* numt){ if( topt != 0? Cyc_Tcutil_coerceable(({ void**
_temp137= topt; if( _temp137 == 0){ _throw( Null_Exception);}* _temp137;})): 0){
return({ void** _temp138= topt; if( _temp138 == 0){ _throw( Null_Exception);}*
_temp138;});} return numt;} static void Cyc_Tcpat_set_vd( struct Cyc_Absyn_Vardecl*
vd, struct Cyc_List_List** v_result_ptr, void* t){( void*)( vd->type=( void*) t);
vd->tq= Cyc_Absyn_empty_tqual();* v_result_ptr=({ struct Cyc_List_List* _temp139=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp139->hd=(
void*) vd; _temp139->tl=* v_result_ptr; _temp139;});} struct _tuple7{ struct Cyc_Absyn_Tqual
f1; void* f2; } ; struct _tuple8{ struct Cyc_Absyn_Structfield* f1; struct Cyc_Absyn_Pat*
f2; } ; struct _tuple5 Cyc_Tcpat_tcPatRec( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat*
p, void** topt, void** region_opt){ Cyc_Tcpat_resolve_pat( te, p);{ void* t;
struct Cyc_List_List* tv_result= 0; struct Cyc_List_List* v_result= 0;{ void*
_temp140=( void*) p->r; struct Cyc_Absyn_Vardecl* _temp174; struct Cyc_Absyn_Vardecl*
_temp176; int _temp178; void* _temp180; int _temp182; void* _temp184; char
_temp186; struct _tagged_string _temp188; struct Cyc_Absyn_Enumfield* _temp190;
struct Cyc_Absyn_Enumdecl* _temp192; struct Cyc_Absyn_Pat* _temp194; struct Cyc_List_List*
_temp196; struct Cyc_List_List* _temp198; struct Cyc_List_List* _temp200; struct
Cyc_Core_Opt* _temp202; struct Cyc_Core_Opt** _temp204; struct Cyc_Absyn_Structdecl*
_temp205; struct Cyc_List_List* _temp207; struct Cyc_List_List* _temp209; struct
Cyc_Absyn_Tunionfield* _temp211; struct Cyc_Absyn_Tuniondecl* _temp213; struct
_tuple1* _temp215; struct Cyc_List_List* _temp217; struct Cyc_List_List*
_temp219; struct _tuple1* _temp221; struct Cyc_List_List* _temp223; struct Cyc_List_List*
_temp225; struct _tuple1* _temp227; _LL142: if( _temp140 ==( void*) Cyc_Absyn_Wild_p){
goto _LL143;} else{ goto _LL144;} _LL144: if(( unsigned int) _temp140 > 2u?*((
int*) _temp140) == Cyc_Absyn_Var_p: 0){ _LL175: _temp174=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_p_struct*) _temp140)->f1; goto _LL145;} else{ goto _LL146;}
_LL146: if(( unsigned int) _temp140 > 2u?*(( int*) _temp140) == Cyc_Absyn_Reference_p:
0){ _LL177: _temp176=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Reference_p_struct*)
_temp140)->f1; goto _LL147;} else{ goto _LL148;} _LL148: if(( unsigned int)
_temp140 > 2u?*(( int*) _temp140) == Cyc_Absyn_Int_p: 0){ _LL181: _temp180=(
void*)(( struct Cyc_Absyn_Int_p_struct*) _temp140)->f1; if( _temp180 ==( void*)
Cyc_Absyn_Unsigned){ goto _LL179;} else{ goto _LL150;} _LL179: _temp178=( int)((
struct Cyc_Absyn_Int_p_struct*) _temp140)->f2; goto _LL149;} else{ goto _LL150;}
_LL150: if(( unsigned int) _temp140 > 2u?*(( int*) _temp140) == Cyc_Absyn_Int_p:
0){ _LL185: _temp184=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp140)->f1;
if( _temp184 ==( void*) Cyc_Absyn_Signed){ goto _LL183;} else{ goto _LL152;}
_LL183: _temp182=( int)(( struct Cyc_Absyn_Int_p_struct*) _temp140)->f2; goto
_LL151;} else{ goto _LL152;} _LL152: if(( unsigned int) _temp140 > 2u?*(( int*)
_temp140) == Cyc_Absyn_Char_p: 0){ _LL187: _temp186=( char)(( struct Cyc_Absyn_Char_p_struct*)
_temp140)->f1; goto _LL153;} else{ goto _LL154;} _LL154: if(( unsigned int)
_temp140 > 2u?*(( int*) _temp140) == Cyc_Absyn_Float_p: 0){ _LL189: _temp188=(
struct _tagged_string)(( struct Cyc_Absyn_Float_p_struct*) _temp140)->f1; goto
_LL155;} else{ goto _LL156;} _LL156: if(( unsigned int) _temp140 > 2u?*(( int*)
_temp140) == Cyc_Absyn_Enum_p: 0){ _LL193: _temp192=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_p_struct*) _temp140)->f1; goto _LL191; _LL191: _temp190=(
struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_p_struct*) _temp140)->f2;
goto _LL157;} else{ goto _LL158;} _LL158: if( _temp140 ==( void*) Cyc_Absyn_Null_p){
goto _LL159;} else{ goto _LL160;} _LL160: if(( unsigned int) _temp140 > 2u?*((
int*) _temp140) == Cyc_Absyn_Pointer_p: 0){ _LL195: _temp194=( struct Cyc_Absyn_Pat*)((
struct Cyc_Absyn_Pointer_p_struct*) _temp140)->f1; goto _LL161;} else{ goto
_LL162;} _LL162: if(( unsigned int) _temp140 > 2u?*(( int*) _temp140) == Cyc_Absyn_Tuple_p:
0){ _LL197: _temp196=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*)
_temp140)->f1; goto _LL163;} else{ goto _LL164;} _LL164: if(( unsigned int)
_temp140 > 2u?*(( int*) _temp140) == Cyc_Absyn_Struct_p: 0){ _LL206: _temp205=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_p_struct*) _temp140)->f1;
goto _LL203; _LL203: _temp202=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp140)->f2; _temp204=&(( struct Cyc_Absyn_Struct_p_struct*) _temp140)->f2;
goto _LL201; _LL201: _temp200=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp140)->f3; goto _LL199; _LL199: _temp198=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Struct_p_struct*) _temp140)->f4; goto _LL165;} else{ goto _LL166;}
_LL166: if(( unsigned int) _temp140 > 2u?*(( int*) _temp140) == Cyc_Absyn_Tunion_p:
0){ _LL214: _temp213=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp140)->f1; goto _LL212; _LL212: _temp211=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp140)->f2; goto _LL210; _LL210: _temp209=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp140)->f3; goto
_LL208; _LL208: _temp207=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp140)->f4; goto _LL167;} else{ goto _LL168;} _LL168: if(( unsigned int)
_temp140 > 2u?*(( int*) _temp140) == Cyc_Absyn_UnknownId_p: 0){ _LL216: _temp215=(
struct _tuple1*)(( struct Cyc_Absyn_UnknownId_p_struct*) _temp140)->f1; goto
_LL169;} else{ goto _LL170;} _LL170: if(( unsigned int) _temp140 > 2u?*(( int*)
_temp140) == Cyc_Absyn_UnknownCall_p: 0){ _LL222: _temp221=( struct _tuple1*)((
struct Cyc_Absyn_UnknownCall_p_struct*) _temp140)->f1; goto _LL220; _LL220:
_temp219=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp140)->f2; goto _LL218; _LL218: _temp217=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnknownCall_p_struct*) _temp140)->f3; goto _LL171;} else{ goto _LL172;}
_LL172: if(( unsigned int) _temp140 > 2u?*(( int*) _temp140) == Cyc_Absyn_UnknownFields_p:
0){ _LL228: _temp227=( struct _tuple1*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp140)->f1; goto _LL226; _LL226: _temp225=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnknownFields_p_struct*) _temp140)->f2; goto _LL224; _LL224: _temp223=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp140)->f3;
goto _LL173;} else{ goto _LL141;} _LL143: t= Cyc_Tcpat_any_type( topt); goto
_LL141; _LL145: t= Cyc_Tcpat_any_type( topt); Cyc_Tcpat_set_vd( _temp174,&
v_result, t); goto _LL141; _LL147: t= Cyc_Tcpat_any_type( topt); if( region_opt
== 0){ Cyc_Tcutil_terr( p->loc,( struct _tagged_string)({ char* _temp229=( char*)"* pattern would point into an unknown region";
struct _tagged_string _temp230; _temp230.curr= _temp229; _temp230.base= _temp229;
_temp230.last_plus_one= _temp229 + 45; _temp230;})); goto _LL141;} Cyc_Tcpat_set_vd(
_temp176,& v_result,( void*)({ struct Cyc_Absyn_PointerType_struct* _temp231=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp231[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp232; _temp232.tag= Cyc_Absyn_PointerType;
_temp232.f1=({ struct Cyc_Absyn_PtrInfo _temp233; _temp233.elt_typ=( void*) t;
_temp233.rgn_typ=( void*)({ void** _temp234= region_opt; if( _temp234 == 0){
_throw( Null_Exception);}* _temp234;}); _temp233.nullable=(( struct Cyc_Absyn_Conref*(*)(
int x)) Cyc_Absyn_new_conref)( 0); _temp233.tq= Cyc_Absyn_empty_tqual();
_temp233.bounds=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp233;}); _temp232;}); _temp231;})); goto _LL141; _LL149: t= Cyc_Tcpat_num_type(
topt, Cyc_Absyn_uint_t); goto _LL141; _LL151: t= Cyc_Tcpat_num_type( topt, Cyc_Absyn_sint_t);
goto _LL141; _LL153: t= Cyc_Tcpat_num_type( topt, Cyc_Absyn_uchar_t); goto
_LL141; _LL155: t= Cyc_Tcpat_num_type( topt, Cyc_Absyn_float_t); goto _LL141;
_LL157: t=( void*)({ struct Cyc_Absyn_EnumType_struct* _temp235=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp235[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp236; _temp236.tag= Cyc_Absyn_EnumType; _temp236.f1=
_temp192->name; _temp236.f2=( struct Cyc_Absyn_Enumdecl*) _temp192; _temp236;});
_temp235;}); goto _LL141; _LL159: if( topt != 0){ void* _temp238= Cyc_Tcutil_compress(({
void** _temp237= topt; if( _temp237 == 0){ _throw( Null_Exception);}* _temp237;}));
struct Cyc_Absyn_PtrInfo _temp244; _LL240: if(( unsigned int) _temp238 > 4u?*((
int*) _temp238) == Cyc_Absyn_PointerType: 0){ _LL245: _temp244=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp238)->f1; goto _LL241;} else{ goto
_LL242;} _LL242: goto _LL243; _LL241: t=({ void** _temp246= topt; if( _temp246
== 0){ _throw( Null_Exception);}* _temp246;}); goto tcpat_end; _LL243: goto
_LL239; _LL239:;} t=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp247=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp247[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp248; _temp248.tag= Cyc_Absyn_PointerType;
_temp248.f1=({ struct Cyc_Absyn_PtrInfo _temp249; _temp249.elt_typ=( void*) Cyc_Absyn_new_evar((
void*) Cyc_Absyn_AnyKind); _temp249.rgn_typ=( void*) Cyc_Absyn_new_evar(( void*)
Cyc_Absyn_RgnKind); _temp249.nullable=(( struct Cyc_Absyn_Conref*(*)( int x))
Cyc_Absyn_new_conref)( 1); _temp249.tq= Cyc_Absyn_empty_tqual(); _temp249.bounds=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp249;}); _temp248;});
_temp247;}); goto _LL141; _LL161: { void* inner_typ=( void*) Cyc_Absyn_VoidType;
void** inner_topt= 0; if( topt != 0){ void* _temp251= Cyc_Tcutil_compress(({
void** _temp250= topt; if( _temp250 == 0){ _throw( Null_Exception);}* _temp250;}));
struct Cyc_Absyn_PtrInfo _temp257; struct Cyc_Absyn_Conref* _temp259; struct Cyc_Absyn_Tqual
_temp261; struct Cyc_Absyn_Conref* _temp263; void* _temp265; void* _temp267;
_LL253: if(( unsigned int) _temp251 > 4u?*(( int*) _temp251) == Cyc_Absyn_PointerType:
0){ _LL258: _temp257=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp251)->f1; _LL268: _temp267=( void*) _temp257.elt_typ; goto _LL266; _LL266:
_temp265=( void*) _temp257.rgn_typ; goto _LL264; _LL264: _temp263=( struct Cyc_Absyn_Conref*)
_temp257.nullable; goto _LL262; _LL262: _temp261=( struct Cyc_Absyn_Tqual)
_temp257.tq; goto _LL260; _LL260: _temp259=( struct Cyc_Absyn_Conref*) _temp257.bounds;
goto _LL254;} else{ goto _LL255;} _LL255: goto _LL256; _LL254: inner_typ=
_temp267; inner_topt=( void**)& inner_typ; goto _LL252; _LL256: goto _LL252;
_LL252:;}{ void* ptr_rgn= Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind); struct
Cyc_List_List* _temp271; struct Cyc_List_List* _temp273; struct _tuple5 _temp269=
Cyc_Tcpat_tcPatRec( te, _temp194, inner_topt,( void**)& ptr_rgn); _LL274:
_temp273= _temp269.f1; goto _LL272; _LL272: _temp271= _temp269.f2; goto _LL270;
_LL270: tv_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( tv_result, _temp273); v_result=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
v_result, _temp271); if( inner_topt == 0){ void* _temp276= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp275= _temp194->topt; if( _temp275 == 0){
_throw( Null_Exception);} _temp275->v;})); struct Cyc_Absyn_TunionFieldInfo
_temp282; struct Cyc_List_List* _temp284; void* _temp286; struct Cyc_Absyn_Tunionfield*
_temp288; struct Cyc_Absyn_Tuniondecl* _temp290; _LL278: if(( unsigned int)
_temp276 > 4u?*(( int*) _temp276) == Cyc_Absyn_TunionFieldType: 0){ _LL283:
_temp282=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp276)->f1; _LL287: _temp286=( void*) _temp282.field_info; if(*(( int*)
_temp286) == Cyc_Absyn_KnownTunionfield){ _LL291: _temp290=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp286)->f1; goto _LL289; _LL289:
_temp288=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp286)->f2; goto _LL285;} else{ goto _LL280;} _LL285: _temp284=( struct Cyc_List_List*)
_temp282.targs; goto _LL279;} else{ goto _LL280;} _LL280: goto _LL281; _LL279: t=(
void*)({ struct Cyc_Absyn_TunionType_struct* _temp292=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp292[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp293; _temp293.tag= Cyc_Absyn_TunionType;
_temp293.f1=({ struct Cyc_Absyn_TunionInfo _temp294; _temp294.tunion_info=( void*)((
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp295=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp295[ 0]=({ struct
Cyc_Absyn_KnownTunion_struct _temp296; _temp296.tag= Cyc_Absyn_KnownTunion;
_temp296.f1= _temp290; _temp296;}); _temp295;})); _temp294.targs= _temp284;
_temp294.rgn=( void*) ptr_rgn; _temp294;}); _temp293;}); _temp292;}); goto
_LL277; _LL281: t=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp297=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp297[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp298; _temp298.tag= Cyc_Absyn_PointerType;
_temp298.f1=({ struct Cyc_Absyn_PtrInfo _temp299; _temp299.elt_typ=( void*)((
void*)({ struct Cyc_Core_Opt* _temp300= _temp194->topt; if( _temp300 == 0){
_throw( Null_Exception);} _temp300->v;})); _temp299.rgn_typ=( void*) ptr_rgn;
_temp299.nullable=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp299.tq= Cyc_Absyn_empty_tqual(); _temp299.bounds=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp299;}); _temp298;}); _temp297;}); goto _LL277;
_LL277:;} else{ t=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp301=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp301[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp302; _temp302.tag= Cyc_Absyn_PointerType;
_temp302.f1=({ struct Cyc_Absyn_PtrInfo _temp303; _temp303.elt_typ=( void*)((
void*)({ struct Cyc_Core_Opt* _temp304= _temp194->topt; if( _temp304 == 0){
_throw( Null_Exception);} _temp304->v;})); _temp303.rgn_typ=( void*) ptr_rgn;
_temp303.nullable=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp303.tq= Cyc_Absyn_empty_tqual(); _temp303.bounds=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp303;}); _temp302;}); _temp301;});} goto _LL141;}}
_LL163: { struct Cyc_List_List* pat_ts= 0; struct Cyc_List_List* topt_ts= 0; if(
topt != 0){ void* _temp306= Cyc_Tcutil_compress(({ void** _temp305= topt; if(
_temp305 == 0){ _throw( Null_Exception);}* _temp305;})); struct Cyc_List_List*
_temp312; _LL308: if(( unsigned int) _temp306 > 4u?*(( int*) _temp306) == Cyc_Absyn_TupleType:
0){ _LL313: _temp312=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp306)->f1; goto _LL309;} else{ goto _LL310;} _LL310: goto _LL311; _LL309:
topt_ts= _temp312; goto _LL307; _LL311: goto _LL307; _LL307:;} for( 0; _temp196
!= 0; _temp196=({ struct Cyc_List_List* _temp314= _temp196; if( _temp314 == 0){
_throw( Null_Exception);} _temp314->tl;})){ void** inner_topt= 0; if( topt_ts !=
0){ inner_topt=( void**)&(*(( struct _tuple7*)({ struct Cyc_List_List* _temp315=
topt_ts; if( _temp315 == 0){ _throw( Null_Exception);} _temp315->hd;}))).f2;
topt_ts=({ struct Cyc_List_List* _temp316= topt_ts; if( _temp316 == 0){ _throw(
Null_Exception);} _temp316->tl;});}{ struct Cyc_List_List* _temp320; struct Cyc_List_List*
_temp322; struct _tuple5 _temp318= Cyc_Tcpat_tcPatRec( te,( struct Cyc_Absyn_Pat*)({
struct Cyc_List_List* _temp317= _temp196; if( _temp317 == 0){ _throw(
Null_Exception);} _temp317->hd;}), inner_topt, region_opt); _LL323: _temp322=
_temp318.f1; goto _LL321; _LL321: _temp320= _temp318.f2; goto _LL319; _LL319:
tv_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( tv_result, _temp322); v_result=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( v_result,
_temp320); pat_ts=({ struct Cyc_List_List* _temp324=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp324->hd=( void*)({ struct
_tuple7* _temp325=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7));
_temp325->f1= Cyc_Absyn_empty_tqual(); _temp325->f2=( void*)({ struct Cyc_Core_Opt*
_temp327=(( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List* _temp326= _temp196;
if( _temp326 == 0){ _throw( Null_Exception);} _temp326->hd;}))->topt; if(
_temp327 == 0){ _throw( Null_Exception);} _temp327->v;}); _temp325;}); _temp324->tl=
pat_ts; _temp324;});}} t=( void*)({ struct Cyc_Absyn_TupleType_struct* _temp328=(
struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp328[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp329; _temp329.tag= Cyc_Absyn_TupleType;
_temp329.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
pat_ts); _temp329;}); _temp328;}); goto _LL141;} _LL165: if( _temp200 != 0){(
void) _throw((( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ char* _temp330=( char*)"tcPat: struct<...> not implemented";
struct _tagged_string _temp331; _temp331.curr= _temp330; _temp331.base= _temp330;
_temp331.last_plus_one= _temp330 + 35; _temp331;})));}{ struct _RegionHandle
_temp332= _new_region(); struct _RegionHandle* rgn=& _temp332; struct
_handler_cons _temp333; _push_handler(& _temp333);{ void* _temp334=( void*)
setjmp( _temp333.handler); if( ! _temp334){{ struct Cyc_List_List* inst=((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple4*(* f)( struct
_RegionHandle*, struct Cyc_Absyn_Tvar*), struct _RegionHandle* env, struct Cyc_List_List*
x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var, rgn, _temp205->tvs);
struct Cyc_List_List* all_typs=(( struct Cyc_List_List*(*)( void*(* f)( struct
_tuple4*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct _tuple4*))
Cyc_Core_snd, inst); t=( void*)({ struct Cyc_Absyn_StructType_struct* _temp335=(
struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp335[ 0]=({ struct Cyc_Absyn_StructType_struct _temp336; _temp336.tag= Cyc_Absyn_StructType;
_temp336.f1=( struct _tuple1*)(( struct _tuple1*)({ struct Cyc_Core_Opt*
_temp337= _temp205->name; if( _temp337 == 0){ _throw( Null_Exception);} _temp337->v;}));
_temp336.f2= all_typs; _temp336.f3=({ struct Cyc_Absyn_Structdecl** _temp338=(
struct Cyc_Absyn_Structdecl**) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*));
_temp338[ 0]= _temp205; _temp338;}); _temp336;}); _temp335;}); if( topt != 0){
Cyc_Tcutil_unify( t,({ void** _temp339= topt; if( _temp339 == 0){ _throw(
Null_Exception);}* _temp339;}));}* _temp204=({ struct Cyc_Core_Opt* _temp340=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp340->v=(
void*) all_typs; _temp340;});{ struct Cyc_List_List* fields=(( struct Cyc_List_List*(*)(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_Absyn_Structdecl* sd)) Cyc_Tcutil_resolve_struct_designators)(
rgn, p->loc, _temp198, _temp205); for( 0; fields != 0; fields=({ struct Cyc_List_List*
_temp341= fields; if( _temp341 == 0){ _throw( Null_Exception);} _temp341->tl;})){
struct _tuple8 _temp345; struct Cyc_Absyn_Pat* _temp346; struct Cyc_Absyn_Structfield*
_temp348; struct _tuple8* _temp343=( struct _tuple8*)({ struct Cyc_List_List*
_temp342= fields; if( _temp342 == 0){ _throw( Null_Exception);} _temp342->hd;});
_temp345=* _temp343; _LL349: _temp348= _temp345.f1; goto _LL347; _LL347:
_temp346= _temp345.f2; goto _LL344; _LL344: { void* inst_fieldtyp= Cyc_Tcutil_rsubstitute(
rgn, inst,( void*) _temp348->type); struct Cyc_List_List* _temp352; struct Cyc_List_List*
_temp354; struct _tuple5 _temp350= Cyc_Tcpat_tcPatRec( te, _temp346,( void**)&
inst_fieldtyp, region_opt); _LL355: _temp354= _temp350.f1; goto _LL353; _LL353:
_temp352= _temp350.f2; goto _LL351; _LL351: tv_result=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result,
_temp354); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( v_result, _temp352); if( ! Cyc_Tcutil_unify((
void*)({ struct Cyc_Core_Opt* _temp356= _temp346->topt; if( _temp356 == 0){
_throw( Null_Exception);} _temp356->v;}), inst_fieldtyp)){ Cyc_Tcutil_terr( p->loc,({
struct _tagged_string _temp358=* _temp348->name; struct _tagged_string _temp359=
Cyc_Absynpp_typ2string( inst_fieldtyp); struct _tagged_string _temp360= Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Core_Opt* _temp357= _temp346->topt; if( _temp357 == 0){
_throw( Null_Exception);} _temp357->v;})); xprintf("field %.*s of struct pattern expects type %.*s != %.*s",
_temp358.last_plus_one - _temp358.curr, _temp358.curr, _temp359.last_plus_one -
_temp359.curr, _temp359.curr, _temp360.last_plus_one - _temp360.curr, _temp360.curr);}));}}}}}
_free_region(& _temp332); _pop_handler();} else{ _free_region(& _temp332);
_throw( _temp334);}}} goto _LL141; _LL167: if((( int(*)( struct Cyc_List_List* x))
Cyc_List_length)( _temp209) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp211->tvs)){ Cyc_Tcutil_terr( p->loc,( struct _tagged_string)({ char*
_temp361=( char*)"wrong number of existential type variables in pattern"; struct
_tagged_string _temp362; _temp362.curr= _temp361; _temp362.base= _temp361;
_temp362.last_plus_one= _temp361 + 54; _temp362;}));}{ struct _RegionHandle
_temp363= _new_region(); struct _RegionHandle* rgn=& _temp363; struct
_handler_cons _temp364; _push_handler(& _temp364);{ void* _temp365=( void*)
setjmp( _temp364.handler); if( ! _temp365){{ struct Cyc_List_List* var_tvs= 0;{
struct Cyc_List_List* t= _temp209; for( 0; t != 0; t=({ struct Cyc_List_List*
_temp366= t; if( _temp366 == 0){ _throw( Null_Exception);} _temp366->tl;})){
struct Cyc_Absyn_Tvar* tv=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp393= t; if( _temp393 == 0){ _throw( Null_Exception);} _temp393->hd;});{
void* _temp367=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( tv->kind))->v; void* _temp377; void* _temp379;
struct Cyc_Absyn_Conref* _temp381; _LL369: if(( unsigned int) _temp367 > 1u?*((
int*) _temp367) == Cyc_Absyn_Eq_constr: 0){ _LL378: _temp377=( void*)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp367)->f1; if( _temp377 ==( void*) Cyc_Absyn_BoxKind){
goto _LL370;} else{ goto _LL371;}} else{ goto _LL371;} _LL371: if(( unsigned int)
_temp367 > 1u?*(( int*) _temp367) == Cyc_Absyn_Eq_constr: 0){ _LL380: _temp379=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp367)->f1; goto _LL372;} else{
goto _LL373;} _LL373: if( _temp367 ==( void*) Cyc_Absyn_No_constr){ goto _LL374;}
else{ goto _LL375;} _LL375: if(( unsigned int) _temp367 > 1u?*(( int*) _temp367)
== Cyc_Absyn_Forward_constr: 0){ _LL382: _temp381=( struct Cyc_Absyn_Conref*)((
struct Cyc_Absyn_Forward_constr_struct*) _temp367)->f1; goto _LL376;} else{ goto
_LL368;} _LL370: goto _LL368; _LL372: Cyc_Tcutil_terr( p->loc,({ struct
_tagged_string _temp383= Cyc_Absynpp_qvar2string( _temp213->name); struct
_tagged_string _temp384=* tv->name; struct _tagged_string _temp385= Cyc_Absynpp_kind2string(
_temp379); xprintf("tunion member %.*s abstracts type variable %.*s of kind %.*s != B",
_temp383.last_plus_one - _temp383.curr, _temp383.curr, _temp384.last_plus_one -
_temp384.curr, _temp384.curr, _temp385.last_plus_one - _temp385.curr, _temp385.curr);}));
goto _LL368; _LL374:( void*)(( tv->kind)->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp386=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp386[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp387; _temp387.tag= Cyc_Absyn_Eq_constr;
_temp387.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp387;}); _temp386;})));
goto _LL368; _LL376:( void) _throw((( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp388=( char*)"forward after compress in conref";
struct _tagged_string _temp389; _temp389.curr= _temp388; _temp389.base= _temp388;
_temp389.last_plus_one= _temp388 + 33; _temp389;}))); _LL368:;} var_tvs=({
struct Cyc_List_List* _temp390=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp390->hd=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct*
_temp391=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp391[ 0]=({ struct Cyc_Absyn_VarType_struct _temp392; _temp392.tag= Cyc_Absyn_VarType;
_temp392.f1= tv; _temp392;}); _temp391;})); _temp390->tl= var_tvs; _temp390;});}}
var_tvs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
var_tvs); tv_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( tv_result, _temp209);{ struct Cyc_Tcenv_Tenv*
te2= Cyc_Tcenv_add_type_vars( p->loc, te, _temp209); struct Cyc_List_List* tqts=
_temp211->typs; struct Cyc_List_List* all_inst=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple4*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Tvar*),
struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,
rgn, _temp213->tvs); struct Cyc_List_List* exist_inst=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, _temp211->tvs, var_tvs);
struct Cyc_List_List* all_typs=(( struct Cyc_List_List*(*)( void*(* f)( struct
_tuple4*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct _tuple4*))
Cyc_Core_snd, all_inst); struct Cyc_List_List* exist_typs=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple4*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple4*)) Cyc_Core_snd, exist_inst); struct Cyc_List_List* inst=((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_rappend)( rgn, all_inst, exist_inst); if( tqts == 0){
t=( void*)({ struct Cyc_Absyn_TunionType_struct* _temp394=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp394[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp395; _temp395.tag= Cyc_Absyn_TunionType;
_temp395.f1=({ struct Cyc_Absyn_TunionInfo _temp396; _temp396.tunion_info=( void*)((
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp397=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp397[ 0]=({ struct
Cyc_Absyn_KnownTunion_struct _temp398; _temp398.tag= Cyc_Absyn_KnownTunion;
_temp398.f1= _temp213; _temp398;}); _temp397;})); _temp396.targs= all_typs;
_temp396.rgn=( void*) Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind); _temp396;});
_temp395;}); _temp394;});} else{ t=( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp399=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp399[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp400; _temp400.tag= Cyc_Absyn_TunionFieldType; _temp400.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp401; _temp401.field_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp402=( struct Cyc_Absyn_KnownTunionfield_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp402[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp403; _temp403.tag= Cyc_Absyn_KnownTunionfield; _temp403.f1= _temp213;
_temp403.f2= _temp211; _temp403;}); _temp402;})); _temp401.targs= all_typs;
_temp401;}); _temp400;}); _temp399;});} if( topt != 0? tqts == 0: 0){ void*
_temp405= Cyc_Tcutil_compress(({ void** _temp404= topt; if( _temp404 == 0){
_throw( Null_Exception);}* _temp404;})); struct Cyc_Absyn_TunionFieldInfo
_temp413; struct Cyc_Absyn_TunionInfo _temp415; void* _temp417; struct Cyc_List_List*
_temp419; void* _temp421; _LL407: if(( unsigned int) _temp405 > 4u?*(( int*)
_temp405) == Cyc_Absyn_TunionFieldType: 0){ _LL414: _temp413=( struct Cyc_Absyn_TunionFieldInfo)((
struct Cyc_Absyn_TunionFieldType_struct*) _temp405)->f1; goto _LL408;} else{
goto _LL409;} _LL409: if(( unsigned int) _temp405 > 4u?*(( int*) _temp405) ==
Cyc_Absyn_TunionType: 0){ _LL416: _temp415=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp405)->f1; _LL422: _temp421=( void*)
_temp415.tunion_info; goto _LL420; _LL420: _temp419=( struct Cyc_List_List*)
_temp415.targs; goto _LL418; _LL418: _temp417=( void*) _temp415.rgn; goto _LL410;}
else{ goto _LL411;} _LL411: goto _LL412; _LL408: t=( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp423=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp423[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp424; _temp424.tag= Cyc_Absyn_TunionFieldType; _temp424.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp425; _temp425.field_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp426=( struct Cyc_Absyn_KnownTunionfield_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp426[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp427; _temp427.tag= Cyc_Absyn_KnownTunionfield; _temp427.f1= _temp213;
_temp427.f2= _temp211; _temp427;}); _temp426;})); _temp425.targs= all_typs;
_temp425;}); _temp424;}); _temp423;}); goto _LL406; _LL410: { struct Cyc_List_List*
a= all_typs; for( 0; a != 0? _temp419 != 0: 0; a=({ struct Cyc_List_List*
_temp428= a; if( _temp428 == 0){ _throw( Null_Exception);} _temp428->tl;}),
_temp419=({ struct Cyc_List_List* _temp429= _temp419; if( _temp429 == 0){ _throw(
Null_Exception);} _temp429->tl;})){ Cyc_Tcutil_unify(( void*)({ struct Cyc_List_List*
_temp430= a; if( _temp430 == 0){ _throw( Null_Exception);} _temp430->hd;}),(
void*)({ struct Cyc_List_List* _temp431= _temp419; if( _temp431 == 0){ _throw(
Null_Exception);} _temp431->hd;}));} goto _LL406;} _LL412: goto _LL406; _LL406:;}
for( 0; _temp207 != 0? tqts != 0: 0; _temp207=({ struct Cyc_List_List* _temp432=
_temp207; if( _temp432 == 0){ _throw( Null_Exception);} _temp432->tl;}), tqts=({
struct Cyc_List_List* _temp433= tqts; if( _temp433 == 0){ _throw( Null_Exception);}
_temp433->tl;})){ struct Cyc_Absyn_Pat* p2=( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List*
_temp446= _temp207; if( _temp446 == 0){ _throw( Null_Exception);} _temp446->hd;});
void* field_typ= Cyc_Tcutil_rsubstitute( rgn, inst,(*(( struct _tuple7*)({
struct Cyc_List_List* _temp445= tqts; if( _temp445 == 0){ _throw( Null_Exception);}
_temp445->hd;}))).f2); struct Cyc_List_List* _temp436; struct Cyc_List_List*
_temp438; struct _tuple5 _temp434= Cyc_Tcpat_tcPatRec( te2, p2,( void**)&
field_typ, region_opt); _LL439: _temp438= _temp434.f1; goto _LL437; _LL437:
_temp436= _temp434.f2; goto _LL435; _LL435: tv_result=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result,
_temp438); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( v_result, _temp436); if( ! Cyc_Tcutil_unify((
void*)({ struct Cyc_Core_Opt* _temp440= p2->topt; if( _temp440 == 0){ _throw(
Null_Exception);} _temp440->v;}), field_typ)){ Cyc_Tcutil_terr( p2->loc,({
struct _tagged_string _temp442= Cyc_Absynpp_qvar2string( _temp213->name); struct
_tagged_string _temp443= Cyc_Absynpp_typ2string( field_typ); struct
_tagged_string _temp444= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp441= p2->topt; if( _temp441 == 0){ _throw( Null_Exception);} _temp441->v;}));
xprintf("%.*s expects argument type %.*s, not %.*s", _temp442.last_plus_one -
_temp442.curr, _temp442.curr, _temp443.last_plus_one - _temp443.curr, _temp443.curr,
_temp444.last_plus_one - _temp444.curr, _temp444.curr);}));}} if( _temp207 != 0){
Cyc_Tcutil_terr( p->loc,({ struct _tagged_string _temp447= Cyc_Absynpp_qvar2string(
_temp213->name); xprintf("too many arguments for tunion constructor %.*s",
_temp447.last_plus_one - _temp447.curr, _temp447.curr);}));} if( tqts != 0){ Cyc_Tcutil_terr(
p->loc,({ struct _tagged_string _temp448= Cyc_Absynpp_qvar2string( _temp213->name);
xprintf("too few arguments for tunion constructor %.*s", _temp448.last_plus_one
- _temp448.curr, _temp448.curr);}));}}} _free_region(& _temp363); _pop_handler();}
else{ _free_region(& _temp363); _throw( _temp365);}}} goto _LL141; _LL169: goto
_LL171; _LL171: goto _LL173; _LL173: t= Cyc_Absyn_wildtyp(); goto _LL141; _LL141:;}
tcpat_end: p->topt=({ struct Cyc_Core_Opt* _temp449=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp449->v=( void*) t; _temp449;});
return({ struct _tuple5 _temp450; _temp450.f1= tv_result; _temp450.f2= v_result;
_temp450;});}} struct _tuple5 Cyc_Tcpat_tcPat( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Pat* p, void** topt, void** region_opt){ struct _tuple5 ans= Cyc_Tcpat_tcPatRec(
te, p, topt, region_opt);{ struct _RegionHandle _temp451= _new_region(); struct
_RegionHandle* r=& _temp451; struct _handler_cons _temp452; _push_handler(&
_temp452);{ void* _temp453=( void*) setjmp( _temp452.handler); if( ! _temp453){
Cyc_Tcutil_check_unique_vars((( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct _tagged_string*(* f)( struct Cyc_Absyn_Vardecl*), struct Cyc_List_List* x))
Cyc_List_rmap)( r, Cyc_Tcpat_get_name, ans.f2), p->loc,( struct _tagged_string)({
char* _temp454=( char*)"pattern contains a repeated variable"; struct
_tagged_string _temp455; _temp455.curr= _temp454; _temp455.base= _temp454;
_temp455.last_plus_one= _temp454 + 37; _temp455;})); _free_region(& _temp451);
_pop_handler();} else{ _free_region(& _temp451); _throw( _temp453);}}} return
ans;} void Cyc_Tcpat_check_pat_regions( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat*
p){ void* _temp456=( void*) p->r; struct Cyc_Absyn_Pat* _temp468; struct Cyc_List_List*
_temp470; struct Cyc_List_List* _temp472; struct Cyc_Absyn_Tunionfield* _temp474;
struct Cyc_Absyn_Tuniondecl* _temp476; struct Cyc_List_List* _temp478; struct
Cyc_List_List* _temp480; struct Cyc_Core_Opt* _temp482; struct Cyc_Absyn_Structdecl*
_temp484; struct Cyc_List_List* _temp486; _LL458: if(( unsigned int) _temp456 >
2u?*(( int*) _temp456) == Cyc_Absyn_Pointer_p: 0){ _LL469: _temp468=( struct Cyc_Absyn_Pat*)((
struct Cyc_Absyn_Pointer_p_struct*) _temp456)->f1; goto _LL459;} else{ goto
_LL460;} _LL460: if(( unsigned int) _temp456 > 2u?*(( int*) _temp456) == Cyc_Absyn_Tunion_p:
0){ _LL477: _temp476=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp456)->f1; goto _LL475; _LL475: _temp474=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp456)->f2; goto _LL473; _LL473: _temp472=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp456)->f3; goto
_LL471; _LL471: _temp470=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp456)->f4; goto _LL461;} else{ goto _LL462;} _LL462: if(( unsigned int)
_temp456 > 2u?*(( int*) _temp456) == Cyc_Absyn_Struct_p: 0){ _LL485: _temp484=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_p_struct*) _temp456)->f1;
goto _LL483; _LL483: _temp482=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp456)->f2; goto _LL481; _LL481: _temp480=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Struct_p_struct*) _temp456)->f3; goto _LL479; _LL479: _temp478=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*) _temp456)->f4; goto
_LL463;} else{ goto _LL464;} _LL464: if(( unsigned int) _temp456 > 2u?*(( int*)
_temp456) == Cyc_Absyn_Tuple_p: 0){ _LL487: _temp486=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tuple_p_struct*) _temp456)->f1; goto _LL465;} else{ goto _LL466;}
_LL466: goto _LL467; _LL459: Cyc_Tcpat_check_pat_regions( te, _temp468);{ void*
_temp489=( void*)({ struct Cyc_Core_Opt* _temp488= p->topt; if( _temp488 == 0){
_throw( Null_Exception);} _temp488->v;}); struct Cyc_Absyn_PtrInfo _temp497;
struct Cyc_Absyn_Conref* _temp499; struct Cyc_Absyn_Tqual _temp501; struct Cyc_Absyn_Conref*
_temp503; void* _temp505; void* _temp507; struct Cyc_Absyn_TunionInfo _temp509;
void* _temp511; struct Cyc_List_List* _temp513; void* _temp515; _LL491: if((
unsigned int) _temp489 > 4u?*(( int*) _temp489) == Cyc_Absyn_PointerType: 0){
_LL498: _temp497=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp489)->f1; _LL508: _temp507=( void*) _temp497.elt_typ; goto _LL506; _LL506:
_temp505=( void*) _temp497.rgn_typ; goto _LL504; _LL504: _temp503=( struct Cyc_Absyn_Conref*)
_temp497.nullable; goto _LL502; _LL502: _temp501=( struct Cyc_Absyn_Tqual)
_temp497.tq; goto _LL500; _LL500: _temp499=( struct Cyc_Absyn_Conref*) _temp497.bounds;
goto _LL492;} else{ goto _LL493;} _LL493: if(( unsigned int) _temp489 > 4u?*((
int*) _temp489) == Cyc_Absyn_TunionType: 0){ _LL510: _temp509=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp489)->f1; _LL516: _temp515=( void*)
_temp509.tunion_info; goto _LL514; _LL514: _temp513=( struct Cyc_List_List*)
_temp509.targs; goto _LL512; _LL512: _temp511=( void*) _temp509.rgn; goto _LL494;}
else{ goto _LL495;} _LL495: goto _LL496; _LL492: _temp511= _temp505; goto _LL494;
_LL494: Cyc_Tcenv_check_rgn_accessible( te, p->loc, _temp511); return; _LL496:(
void) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp517=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp517[ 0]=({ struct
Cyc_Core_Impossible_struct _temp518; _temp518.tag= Cyc_Core_Impossible; _temp518.f1=(
struct _tagged_string)({ char* _temp519=( char*)"check_pat_regions: bad pointer type";
struct _tagged_string _temp520; _temp520.curr= _temp519; _temp520.base= _temp519;
_temp520.last_plus_one= _temp519 + 36; _temp520;}); _temp518;}); _temp517;}));
_LL490:;} _LL461: for( 0; _temp470 != 0; _temp470=({ struct Cyc_List_List*
_temp521= _temp470; if( _temp521 == 0){ _throw( Null_Exception);} _temp521->tl;})){
Cyc_Tcpat_check_pat_regions( te,( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List*
_temp522= _temp470; if( _temp522 == 0){ _throw( Null_Exception);} _temp522->hd;}));}{
void* _temp524=( void*)({ struct Cyc_Core_Opt* _temp523= p->topt; if( _temp523
== 0){ _throw( Null_Exception);} _temp523->v;}); struct Cyc_Absyn_TunionInfo
_temp532; void* _temp534; struct Cyc_List_List* _temp536; void* _temp538; struct
Cyc_Absyn_TunionFieldInfo _temp540; _LL526: if(( unsigned int) _temp524 > 4u?*((
int*) _temp524) == Cyc_Absyn_TunionType: 0){ _LL533: _temp532=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp524)->f1; _LL539: _temp538=( void*)
_temp532.tunion_info; goto _LL537; _LL537: _temp536=( struct Cyc_List_List*)
_temp532.targs; goto _LL535; _LL535: _temp534=( void*) _temp532.rgn; goto _LL527;}
else{ goto _LL528;} _LL528: if(( unsigned int) _temp524 > 4u?*(( int*) _temp524)
== Cyc_Absyn_TunionFieldType: 0){ _LL541: _temp540=( struct Cyc_Absyn_TunionFieldInfo)((
struct Cyc_Absyn_TunionFieldType_struct*) _temp524)->f1; goto _LL529;} else{
goto _LL530;} _LL530: goto _LL531; _LL527: Cyc_Tcenv_check_rgn_accessible( te, p->loc,
_temp534); return; _LL529: return; _LL531:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp542=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp542[ 0]=({ struct Cyc_Core_Impossible_struct _temp543; _temp543.tag= Cyc_Core_Impossible;
_temp543.f1=( struct _tagged_string)({ char* _temp544=( char*)"check_pat_regions: bad tunion type";
struct _tagged_string _temp545; _temp545.curr= _temp544; _temp545.base= _temp544;
_temp545.last_plus_one= _temp544 + 35; _temp545;}); _temp543;}); _temp542;}));
_LL525:;} _LL463: _temp486=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Pat*(*
f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Absyn_Pat*(*)(
struct _tuple6*)) Cyc_Core_snd, _temp478); goto _LL465; _LL465: for( 0; _temp486
!= 0; _temp486=({ struct Cyc_List_List* _temp546= _temp486; if( _temp546 == 0){
_throw( Null_Exception);} _temp546->tl;})){ Cyc_Tcpat_check_pat_regions( te,(
struct Cyc_Absyn_Pat*)({ struct Cyc_List_List* _temp547= _temp486; if( _temp547
== 0){ _throw( Null_Exception);} _temp547->hd;}));} return; _LL467: return;
_LL457:;} static const int Cyc_Tcpat_Name_v= 0; struct Cyc_Tcpat_Name_v_struct{
int tag; struct _tagged_string f1; } ; static const int Cyc_Tcpat_Int_v= 1;
struct Cyc_Tcpat_Int_v_struct{ int tag; int f1; } ; typedef void* Cyc_Tcpat_name_value_t;
struct Cyc_Tcpat_Con_s{ void* name; int arity; struct Cyc_Core_Opt* span; } ;
typedef struct Cyc_Tcpat_Con_s* Cyc_Tcpat_con_t; static const int Cyc_Tcpat_Any=
0; static const int Cyc_Tcpat_Con= 0; struct Cyc_Tcpat_Con_struct{ int tag;
struct Cyc_Tcpat_Con_s* f1; struct Cyc_List_List* f2; } ; typedef void* Cyc_Tcpat_simple_pat_t;
static int Cyc_Tcpat_compare_con( struct Cyc_Tcpat_Con_s* c1, struct Cyc_Tcpat_Con_s*
c2){ void* _temp548=( void*) c1->name; struct _tagged_string _temp554; int
_temp556; _LL550: if(*(( int*) _temp548) == Cyc_Tcpat_Name_v){ _LL555: _temp554=(
struct _tagged_string)(( struct Cyc_Tcpat_Name_v_struct*) _temp548)->f1; goto
_LL551;} else{ goto _LL552;} _LL552: if(*(( int*) _temp548) == Cyc_Tcpat_Int_v){
_LL557: _temp556=( int)(( struct Cyc_Tcpat_Int_v_struct*) _temp548)->f1; goto
_LL553;} else{ goto _LL549;} _LL551: { void* _temp558=( void*) c2->name; struct
_tagged_string _temp564; int _temp566; _LL560: if(*(( int*) _temp558) == Cyc_Tcpat_Name_v){
_LL565: _temp564=( struct _tagged_string)(( struct Cyc_Tcpat_Name_v_struct*)
_temp558)->f1; goto _LL561;} else{ goto _LL562;} _LL562: if(*(( int*) _temp558)
== Cyc_Tcpat_Int_v){ _LL567: _temp566=( int)(( struct Cyc_Tcpat_Int_v_struct*)
_temp558)->f1; goto _LL563;} else{ goto _LL559;} _LL561: return Cyc_String_zstrcmp(
_temp554, _temp564); _LL563: return - 1; _LL559:;} _LL553: { void* _temp568=(
void*) c2->name; struct _tagged_string _temp574; int _temp576; _LL570: if(*((
int*) _temp568) == Cyc_Tcpat_Name_v){ _LL575: _temp574=( struct _tagged_string)((
struct Cyc_Tcpat_Name_v_struct*) _temp568)->f1; goto _LL571;} else{ goto _LL572;}
_LL572: if(*(( int*) _temp568) == Cyc_Tcpat_Int_v){ _LL577: _temp576=( int)((
struct Cyc_Tcpat_Int_v_struct*) _temp568)->f1; goto _LL573;} else{ goto _LL569;}
_LL571: return 1; _LL573: return _temp556 - _temp576; _LL569:;} _LL549:;} static
struct Cyc_Set_Set* Cyc_Tcpat_empty_con_set(){ return(( struct Cyc_Set_Set*(*)(
int(* comp)( struct Cyc_Tcpat_Con_s*, struct Cyc_Tcpat_Con_s*))) Cyc_Set_empty)(
Cyc_Tcpat_compare_con);} static struct Cyc_Core_Opt Cyc_Tcpat_one_opt=( struct
Cyc_Core_Opt){.v=( void*) 1}; static struct Cyc_Core_Opt Cyc_Tcpat_two_opt=(
struct Cyc_Core_Opt){.v=( void*) 2}; static struct Cyc_Core_Opt* Cyc_Tcpat_one_opt_ptr=(
struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt; static char _temp581[ 5u]="null";
static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_null_name_value={ 0u,( struct
_tagged_string){ _temp581, _temp581, _temp581 + 5u}}; static char _temp585[ 4u]="ptr";
static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_ptr_name_value={ 0u,( struct
_tagged_string){ _temp585, _temp585, _temp585 + 4u}}; static char _temp589[ 6u]="tuple";
static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_tuple_name_value={ 0u,( struct
_tagged_string){ _temp589, _temp589, _temp589 + 6u}}; static struct Cyc_Tcpat_Con_s
Cyc_Tcpat_null_con0=( struct Cyc_Tcpat_Con_s){.name=( void*)(( void*)& Cyc_Tcpat_null_name_value),.arity=
0,.span=( struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt}; static struct Cyc_Tcpat_Con_s
Cyc_Tcpat_null_ptr_con0=( struct Cyc_Tcpat_Con_s){.name=( void*)(( void*)& Cyc_Tcpat_ptr_name_value),.arity=
1,.span=( struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt}; static struct Cyc_Tcpat_Con_s
Cyc_Tcpat_ptr_con0=( struct Cyc_Tcpat_Con_s){.name=( void*)(( void*)& Cyc_Tcpat_ptr_name_value),.arity=
1,.span=( struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt}; static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_null_con=& Cyc_Tcpat_null_con0; static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_null_ptr_con=&
Cyc_Tcpat_null_ptr_con0; static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_ptr_con=& Cyc_Tcpat_ptr_con0;
static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_int_con( int i){ return({ struct Cyc_Tcpat_Con_s*
_temp590=( struct Cyc_Tcpat_Con_s*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_s));
_temp590->name=( void*)(( void*)({ struct Cyc_Tcpat_Int_v_struct* _temp591=(
struct Cyc_Tcpat_Int_v_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Int_v_struct));
_temp591[ 0]=({ struct Cyc_Tcpat_Int_v_struct _temp592; _temp592.tag= Cyc_Tcpat_Int_v;
_temp592.f1= i; _temp592;}); _temp591;})); _temp590->arity= 0; _temp590->span= 0;
_temp590;});} static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_float_con( struct
_tagged_string f){ return({ struct Cyc_Tcpat_Con_s* _temp593=( struct Cyc_Tcpat_Con_s*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp593->name=( void*)(( void*)({
struct Cyc_Tcpat_Name_v_struct* _temp594=( struct Cyc_Tcpat_Name_v_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Name_v_struct)); _temp594[ 0]=({ struct Cyc_Tcpat_Name_v_struct
_temp595; _temp595.tag= Cyc_Tcpat_Name_v; _temp595.f1= f; _temp595;}); _temp594;}));
_temp593->arity= 0; _temp593->span= 0; _temp593;});} static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_char_con( char c){ return({ struct Cyc_Tcpat_Con_s* _temp596=( struct
Cyc_Tcpat_Con_s*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp596->name=(
void*)(( void*)({ struct Cyc_Tcpat_Int_v_struct* _temp598=( struct Cyc_Tcpat_Int_v_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Int_v_struct)); _temp598[ 0]=({ struct Cyc_Tcpat_Int_v_struct
_temp599; _temp599.tag= Cyc_Tcpat_Int_v; _temp599.f1=( int) c; _temp599;});
_temp598;})); _temp596->arity= 0; _temp596->span=({ struct Cyc_Core_Opt*
_temp597=( struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof( struct Cyc_Core_Opt));
_temp597->v=( void*) 256; _temp597;}); _temp596;});} static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_tuple_con( int i){ return({ struct Cyc_Tcpat_Con_s* _temp600=( struct
Cyc_Tcpat_Con_s*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp600->name=(
void*)(( void*)& Cyc_Tcpat_tuple_name_value); _temp600->arity= i; _temp600->span=
Cyc_Tcpat_one_opt_ptr; _temp600;});} static void* Cyc_Tcpat_null_pat(){ return(
void*)({ struct Cyc_Tcpat_Con_struct* _temp601=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp601[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp602; _temp602.tag= Cyc_Tcpat_Con; _temp602.f1= Cyc_Tcpat_null_con; _temp602.f2=
0; _temp602;}); _temp601;});} static void* Cyc_Tcpat_int_pat( int i){ return(
void*)({ struct Cyc_Tcpat_Con_struct* _temp603=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp603[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp604; _temp604.tag= Cyc_Tcpat_Con; _temp604.f1= Cyc_Tcpat_int_con( i);
_temp604.f2= 0; _temp604;}); _temp603;});} static void* Cyc_Tcpat_char_pat( char
c){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp605=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp605[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp606; _temp606.tag= Cyc_Tcpat_Con; _temp606.f1= Cyc_Tcpat_char_con( c);
_temp606.f2= 0; _temp606;}); _temp605;});} static void* Cyc_Tcpat_float_pat(
struct _tagged_string f){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp607=(
struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp607[ 0]=({ struct Cyc_Tcpat_Con_struct _temp608; _temp608.tag= Cyc_Tcpat_Con;
_temp608.f1= Cyc_Tcpat_float_con( f); _temp608.f2= 0; _temp608;}); _temp607;});}
static void* Cyc_Tcpat_null_ptr_pat( void* p){ return( void*)({ struct Cyc_Tcpat_Con_struct*
_temp609=( struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp609[ 0]=({ struct Cyc_Tcpat_Con_struct _temp610; _temp610.tag= Cyc_Tcpat_Con;
_temp610.f1= Cyc_Tcpat_null_ptr_con; _temp610.f2=({ struct Cyc_List_List*
_temp611=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp611->hd=( void*) p; _temp611->tl= 0; _temp611;}); _temp610;}); _temp609;});}
static void* Cyc_Tcpat_ptr_pat( void* p){ return( void*)({ struct Cyc_Tcpat_Con_struct*
_temp612=( struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp612[ 0]=({ struct Cyc_Tcpat_Con_struct _temp613; _temp613.tag= Cyc_Tcpat_Con;
_temp613.f1= Cyc_Tcpat_ptr_con; _temp613.f2=({ struct Cyc_List_List* _temp614=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp614->hd=(
void*) p; _temp614->tl= 0; _temp614;}); _temp613;}); _temp612;});} static void*
Cyc_Tcpat_tuple_pat( struct Cyc_List_List* ss){ return( void*)({ struct Cyc_Tcpat_Con_struct*
_temp615=( struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp615[ 0]=({ struct Cyc_Tcpat_Con_struct _temp616; _temp616.tag= Cyc_Tcpat_Con;
_temp616.f1= Cyc_Tcpat_tuple_con((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
ss)); _temp616.f2= ss; _temp616;}); _temp615;});} static void* Cyc_Tcpat_con_pat(
struct _tagged_string con_name, struct Cyc_Core_Opt* span, struct Cyc_List_List*
ps){ struct Cyc_Tcpat_Con_s* c=({ struct Cyc_Tcpat_Con_s* _temp619=( struct Cyc_Tcpat_Con_s*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp619->name=( void*)(( void*)({
struct Cyc_Tcpat_Name_v_struct* _temp620=( struct Cyc_Tcpat_Name_v_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Name_v_struct)); _temp620[ 0]=({ struct Cyc_Tcpat_Name_v_struct
_temp621; _temp621.tag= Cyc_Tcpat_Name_v; _temp621.f1= con_name; _temp621;});
_temp620;})); _temp619->arity=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
ps); _temp619->span= span; _temp619;}); return( void*)({ struct Cyc_Tcpat_Con_struct*
_temp617=( struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp617[ 0]=({ struct Cyc_Tcpat_Con_struct _temp618; _temp618.tag= Cyc_Tcpat_Con;
_temp618.f1= c; _temp618.f2= ps; _temp618;}); _temp617;});} static void* Cyc_Tcpat_compile_pat(
struct Cyc_Absyn_Pat* p){ void* s;{ void* _temp622=( void*) p->r; struct Cyc_Absyn_Vardecl*
_temp656; int _temp658; void* _temp660; char _temp662; struct _tagged_string
_temp664; struct Cyc_Absyn_Vardecl* _temp666; struct Cyc_Absyn_Pat* _temp668;
struct Cyc_List_List* _temp670; struct Cyc_List_List* _temp672; struct Cyc_Absyn_Tunionfield*
_temp674; struct Cyc_Absyn_Tuniondecl* _temp676; struct Cyc_List_List* _temp678;
struct Cyc_List_List* _temp680; struct Cyc_Absyn_Tunionfield* _temp682; struct
Cyc_Absyn_Tuniondecl* _temp684; struct Cyc_List_List* _temp686; struct Cyc_List_List*
_temp688; struct Cyc_List_List* _temp690; struct Cyc_Core_Opt* _temp692; struct
Cyc_Absyn_Structdecl* _temp694; struct Cyc_Absyn_Enumfield* _temp696; struct Cyc_Absyn_Enumdecl*
_temp698; struct _tuple1* _temp700; struct Cyc_List_List* _temp702; struct Cyc_List_List*
_temp704; struct _tuple1* _temp706; struct Cyc_List_List* _temp708; struct Cyc_List_List*
_temp710; struct _tuple1* _temp712; _LL624: if( _temp622 ==( void*) Cyc_Absyn_Wild_p){
goto _LL625;} else{ goto _LL626;} _LL626: if(( unsigned int) _temp622 > 2u?*((
int*) _temp622) == Cyc_Absyn_Var_p: 0){ _LL657: _temp656=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_p_struct*) _temp622)->f1; goto _LL627;} else{ goto _LL628;}
_LL628: if( _temp622 ==( void*) Cyc_Absyn_Null_p){ goto _LL629;} else{ goto
_LL630;} _LL630: if(( unsigned int) _temp622 > 2u?*(( int*) _temp622) == Cyc_Absyn_Int_p:
0){ _LL661: _temp660=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp622)->f1;
goto _LL659; _LL659: _temp658=( int)(( struct Cyc_Absyn_Int_p_struct*) _temp622)->f2;
goto _LL631;} else{ goto _LL632;} _LL632: if(( unsigned int) _temp622 > 2u?*((
int*) _temp622) == Cyc_Absyn_Char_p: 0){ _LL663: _temp662=( char)(( struct Cyc_Absyn_Char_p_struct*)
_temp622)->f1; goto _LL633;} else{ goto _LL634;} _LL634: if(( unsigned int)
_temp622 > 2u?*(( int*) _temp622) == Cyc_Absyn_Float_p: 0){ _LL665: _temp664=(
struct _tagged_string)(( struct Cyc_Absyn_Float_p_struct*) _temp622)->f1; goto
_LL635;} else{ goto _LL636;} _LL636: if(( unsigned int) _temp622 > 2u?*(( int*)
_temp622) == Cyc_Absyn_Reference_p: 0){ _LL667: _temp666=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Reference_p_struct*) _temp622)->f1; goto _LL637;} else{ goto
_LL638;} _LL638: if(( unsigned int) _temp622 > 2u?*(( int*) _temp622) == Cyc_Absyn_Pointer_p:
0){ _LL669: _temp668=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*)
_temp622)->f1; goto _LL639;} else{ goto _LL640;} _LL640: if(( unsigned int)
_temp622 > 2u?*(( int*) _temp622) == Cyc_Absyn_Tunion_p: 0){ _LL677: _temp676=(
struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp622)->f1;
goto _LL675; _LL675: _temp674=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp622)->f2; goto _LL673; _LL673: _temp672=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Tunion_p_struct*) _temp622)->f3; goto _LL671; _LL671: _temp670=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp622)->f4; if(
_temp670 == 0){ goto _LL641;} else{ goto _LL642;}} else{ goto _LL642;} _LL642:
if(( unsigned int) _temp622 > 2u?*(( int*) _temp622) == Cyc_Absyn_Tunion_p: 0){
_LL685: _temp684=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp622)->f1; goto _LL683; _LL683: _temp682=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp622)->f2; goto _LL681; _LL681: _temp680=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp622)->f3; goto
_LL679; _LL679: _temp678=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp622)->f4; goto _LL643;} else{ goto _LL644;} _LL644: if(( unsigned int)
_temp622 > 2u?*(( int*) _temp622) == Cyc_Absyn_Tuple_p: 0){ _LL687: _temp686=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*) _temp622)->f1; goto
_LL645;} else{ goto _LL646;} _LL646: if(( unsigned int) _temp622 > 2u?*(( int*)
_temp622) == Cyc_Absyn_Struct_p: 0){ _LL695: _temp694=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_p_struct*) _temp622)->f1; goto _LL693; _LL693: _temp692=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_p_struct*) _temp622)->f2; goto
_LL691; _LL691: _temp690=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp622)->f3; goto _LL689; _LL689: _temp688=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Struct_p_struct*) _temp622)->f4; goto _LL647;} else{ goto _LL648;}
_LL648: if(( unsigned int) _temp622 > 2u?*(( int*) _temp622) == Cyc_Absyn_Enum_p:
0){ _LL699: _temp698=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp622)->f1; goto _LL697; _LL697: _temp696=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_p_struct*) _temp622)->f2; goto _LL649;} else{ goto _LL650;}
_LL650: if(( unsigned int) _temp622 > 2u?*(( int*) _temp622) == Cyc_Absyn_UnknownId_p:
0){ _LL701: _temp700=( struct _tuple1*)(( struct Cyc_Absyn_UnknownId_p_struct*)
_temp622)->f1; goto _LL651;} else{ goto _LL652;} _LL652: if(( unsigned int)
_temp622 > 2u?*(( int*) _temp622) == Cyc_Absyn_UnknownCall_p: 0){ _LL707:
_temp706=( struct _tuple1*)(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp622)->f1;
goto _LL705; _LL705: _temp704=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp622)->f2; goto _LL703; _LL703: _temp702=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnknownCall_p_struct*) _temp622)->f3; goto _LL653;} else{ goto _LL654;}
_LL654: if(( unsigned int) _temp622 > 2u?*(( int*) _temp622) == Cyc_Absyn_UnknownFields_p:
0){ _LL713: _temp712=( struct _tuple1*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp622)->f1; goto _LL711; _LL711: _temp710=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnknownFields_p_struct*) _temp622)->f2; goto _LL709; _LL709: _temp708=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp622)->f3;
goto _LL655;} else{ goto _LL623;} _LL625: s=( void*) Cyc_Tcpat_Any; goto _LL623;
_LL627: s=( void*) Cyc_Tcpat_Any; goto _LL623; _LL629: s= Cyc_Tcpat_null_pat();
goto _LL623; _LL631: s= Cyc_Tcpat_int_pat( _temp658); goto _LL623; _LL633: s=
Cyc_Tcpat_char_pat( _temp662); goto _LL623; _LL635: s= Cyc_Tcpat_float_pat(
_temp664); goto _LL623; _LL637: s=( void*) Cyc_Tcpat_Any; goto _LL623; _LL639:{
void* _temp715= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp714= p->topt;
if( _temp714 == 0){ _throw( Null_Exception);} _temp714->v;})); struct Cyc_Absyn_PtrInfo
_temp723; struct Cyc_Absyn_Conref* _temp725; struct Cyc_Absyn_Tqual _temp727;
struct Cyc_Absyn_Conref* _temp729; void* _temp731; void* _temp733; struct Cyc_Absyn_TunionInfo
_temp735; _LL717: if(( unsigned int) _temp715 > 4u?*(( int*) _temp715) == Cyc_Absyn_PointerType:
0){ _LL724: _temp723=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp715)->f1; _LL734: _temp733=( void*) _temp723.elt_typ; goto _LL732; _LL732:
_temp731=( void*) _temp723.rgn_typ; goto _LL730; _LL730: _temp729=( struct Cyc_Absyn_Conref*)
_temp723.nullable; goto _LL728; _LL728: _temp727=( struct Cyc_Absyn_Tqual)
_temp723.tq; goto _LL726; _LL726: _temp725=( struct Cyc_Absyn_Conref*) _temp723.bounds;
goto _LL718;} else{ goto _LL719;} _LL719: if(( unsigned int) _temp715 > 4u?*((
int*) _temp715) == Cyc_Absyn_TunionType: 0){ _LL736: _temp735=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp715)->f1; goto _LL720;} else{ goto
_LL721;} _LL721: goto _LL722; _LL718: { int is_nullable= 0; int still_working= 1;
while( still_working) { void* _temp737=( void*) _temp729->v; struct Cyc_Absyn_Conref*
_temp745; int _temp747; _LL739: if(( unsigned int) _temp737 > 1u?*(( int*)
_temp737) == Cyc_Absyn_Forward_constr: 0){ _LL746: _temp745=( struct Cyc_Absyn_Conref*)((
struct Cyc_Absyn_Forward_constr_struct*) _temp737)->f1; goto _LL740;} else{ goto
_LL741;} _LL741: if( _temp737 ==( void*) Cyc_Absyn_No_constr){ goto _LL742;}
else{ goto _LL743;} _LL743: if(( unsigned int) _temp737 > 1u?*(( int*) _temp737)
== Cyc_Absyn_Eq_constr: 0){ _LL748: _temp747=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp737)->f1; goto _LL744;} else{ goto _LL738;} _LL740:( void*)( _temp729->v=(
void*)(( void*) _temp745->v)); continue; _LL742:( void*)( _temp729->v=( void*)((
void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp749=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp749[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp750; _temp750.tag= Cyc_Absyn_Eq_constr; _temp750.f1=(
void*) 0; _temp750;}); _temp749;}))); is_nullable= 0; still_working= 0; goto
_LL738; _LL744: is_nullable=( int) _temp747; still_working= 0; goto _LL738;
_LL738:;}{ void* ss= Cyc_Tcpat_compile_pat( _temp668); if( is_nullable){ s= Cyc_Tcpat_null_ptr_pat(
ss);} else{ s= Cyc_Tcpat_ptr_pat( ss);} goto _LL716;}} _LL720:{ void* _temp751=(
void*) _temp668->r; struct Cyc_List_List* _temp757; struct Cyc_List_List*
_temp759; struct Cyc_Absyn_Tunionfield* _temp761; struct Cyc_Absyn_Tuniondecl*
_temp763; _LL753: if(( unsigned int) _temp751 > 2u?*(( int*) _temp751) == Cyc_Absyn_Tunion_p:
0){ _LL764: _temp763=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp751)->f1; goto _LL762; _LL762: _temp761=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp751)->f2; goto _LL760; _LL760: _temp759=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp751)->f3; goto
_LL758; _LL758: _temp757=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp751)->f4; goto _LL754;} else{ goto _LL755;} _LL755: goto _LL756; _LL754: {
struct Cyc_Core_Opt* span; if( _temp763->is_xtunion){ span= 0;} else{ span=({
struct Cyc_Core_Opt* _temp765=( struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof(
struct Cyc_Core_Opt)); _temp765->v=( void*)(( int(*)( struct Cyc_List_List* x))
Cyc_List_length)(( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp766=
_temp763->fields; if( _temp766 == 0){ _throw( Null_Exception);} _temp766->v;}));
_temp765;});} s= Cyc_Tcpat_con_pat(*(* _temp761->name).f2, span,(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_compile_pat,
_temp757)); goto _LL752;} _LL756: s=(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp767=( char*)"non-[x]tunion pattern has tunion type";
struct _tagged_string _temp768; _temp768.curr= _temp767; _temp768.base= _temp767;
_temp768.last_plus_one= _temp767 + 38; _temp768;})); goto _LL752; _LL752:;} goto
_LL716; _LL722: s=(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp769=( char*)"pointer pattern does not have pointer type!";
struct _tagged_string _temp770; _temp770.curr= _temp769; _temp770.base= _temp769;
_temp770.last_plus_one= _temp769 + 44; _temp770;})); goto _LL716; _LL716:;} goto
_LL623; _LL641: { struct Cyc_Core_Opt* span;{ void* _temp772= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp771= p->topt; if( _temp771 == 0){ _throw(
Null_Exception);} _temp771->v;})); struct Cyc_Absyn_TunionInfo _temp780; struct
Cyc_Absyn_TunionFieldInfo _temp782; _LL774: if(( unsigned int) _temp772 > 4u?*((
int*) _temp772) == Cyc_Absyn_TunionType: 0){ _LL781: _temp780=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp772)->f1; goto _LL775;} else{ goto
_LL776;} _LL776: if(( unsigned int) _temp772 > 4u?*(( int*) _temp772) == Cyc_Absyn_TunionFieldType:
0){ _LL783: _temp782=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp772)->f1; goto _LL777;} else{ goto _LL778;} _LL778: goto _LL779; _LL775:
if( _temp676->is_xtunion){ span= 0;} else{ span=({ struct Cyc_Core_Opt* _temp784=(
struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp784->v=(
void*)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp785= _temp676->fields; if( _temp785 == 0){ _throw(
Null_Exception);} _temp785->v;})); _temp784;});} goto _LL773; _LL777: span=({
struct Cyc_Core_Opt* _temp786=( struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof(
struct Cyc_Core_Opt)); _temp786->v=( void*) 1; _temp786;}); goto _LL773; _LL779:
span=(( struct Cyc_Core_Opt*(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp787=( char*)"void tunion pattern has bad type";
struct _tagged_string _temp788; _temp788.curr= _temp787; _temp788.base= _temp787;
_temp788.last_plus_one= _temp787 + 33; _temp788;})); goto _LL773; _LL773:;} s=
Cyc_Tcpat_con_pat(*(* _temp674->name).f2, span, 0); goto _LL623;} _LL643:
_temp686= _temp678; goto _LL645; _LL645: s= Cyc_Tcpat_tuple_pat((( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_compile_pat,
_temp686)); goto _LL623; _LL647: { struct Cyc_List_List* ps= 0;{ struct Cyc_List_List*
fields=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp821= _temp694->fields;
if( _temp821 == 0){ _throw( Null_Exception);} _temp821->v;}); for( 0; fields !=
0; fields=({ struct Cyc_List_List* _temp789= fields; if( _temp789 == 0){ _throw(
Null_Exception);} _temp789->tl;})){ int found= Cyc_String_strcmp(*(( struct Cyc_Absyn_Structfield*)({
struct Cyc_List_List* _temp818= fields; if( _temp818 == 0){ _throw(
Null_Exception);} _temp818->hd;}))->name,( struct _tagged_string)({ char*
_temp819=( char*)""; struct _tagged_string _temp820; _temp820.curr= _temp819;
_temp820.base= _temp819; _temp820.last_plus_one= _temp819 + 1; _temp820;})) == 0;{
struct Cyc_List_List* dlps0= _temp688; for( 0; ! found? dlps0 != 0: 0; dlps0=({
struct Cyc_List_List* _temp790= dlps0; if( _temp790 == 0){ _throw(
Null_Exception);} _temp790->tl;})){ struct _tuple6 _temp794; struct Cyc_Absyn_Pat*
_temp795; struct Cyc_List_List* _temp797; struct _tuple6* _temp792=( struct
_tuple6*)({ struct Cyc_List_List* _temp791= dlps0; if( _temp791 == 0){ _throw(
Null_Exception);} _temp791->hd;}); _temp794=* _temp792; _LL798: _temp797=
_temp794.f1; goto _LL796; _LL796: _temp795= _temp794.f2; goto _LL793; _LL793: {
struct Cyc_List_List* _temp799= _temp797; struct Cyc_List_List _temp805; struct
Cyc_List_List* _temp806; void* _temp808; struct _tagged_string* _temp810; _LL801:
if( _temp799 == 0){ goto _LL803;} else{ _temp805=* _temp799; _LL809: _temp808=(
void*) _temp805.hd; if(*(( int*) _temp808) == Cyc_Absyn_FieldName){ _LL811:
_temp810=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp808)->f1; goto _LL807;} else{ goto _LL803;} _LL807: _temp806=( struct Cyc_List_List*)
_temp805.tl; if( _temp806 == 0){ goto _LL802;} else{ goto _LL803;}} _LL803: goto
_LL804; _LL802: if( Cyc_String_zstrptrcmp( _temp810,(( struct Cyc_Absyn_Structfield*)({
struct Cyc_List_List* _temp812= fields; if( _temp812 == 0){ _throw(
Null_Exception);} _temp812->hd;}))->name) == 0){ ps=({ struct Cyc_List_List*
_temp813=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp813->hd=( void*) Cyc_Tcpat_compile_pat( _temp795); _temp813->tl= ps;
_temp813;}); found= 1;} goto _LL800; _LL804:(( void(*)( struct _tagged_string))
Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp814=( char*)"bad designator(s)";
struct _tagged_string _temp815; _temp815.curr= _temp814; _temp815.base= _temp814;
_temp815.last_plus_one= _temp814 + 18; _temp815;})); goto _LL800; _LL800:;}}}
if( ! found){(( void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ char* _temp816=( char*)"bad designator"; struct _tagged_string
_temp817; _temp817.curr= _temp816; _temp817.base= _temp816; _temp817.last_plus_one=
_temp816 + 15; _temp817;}));}}} s= Cyc_Tcpat_tuple_pat( ps); goto _LL623;}
_LL649: { int span=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp698->fields); s= Cyc_Tcpat_con_pat(*(* _temp696->name).f2,({ struct Cyc_Core_Opt*
_temp822=( struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof( struct Cyc_Core_Opt));
_temp822->v=( void*) span; _temp822;}), 0); goto _LL623;} _LL651: s=(( void*(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp823=( char*)"compile_pat: unknownid"; struct _tagged_string _temp824;
_temp824.curr= _temp823; _temp824.base= _temp823; _temp824.last_plus_one=
_temp823 + 23; _temp824;})); goto _LL623; _LL653: s=(( void*(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp825=(
char*)"compile_pat: unknowncall"; struct _tagged_string _temp826; _temp826.curr=
_temp825; _temp826.base= _temp825; _temp826.last_plus_one= _temp825 + 25;
_temp826;})); goto _LL623; _LL655: s=(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp827=( char*)"compile_pat: unknownfields";
struct _tagged_string _temp828; _temp828.curr= _temp827; _temp828.base= _temp827;
_temp828.last_plus_one= _temp827 + 27; _temp828;})); goto _LL623; _LL623:;}
return s;} typedef struct Cyc_List_List* Cyc_Tcpat_match_t; static const int Cyc_Tcpat_Pos=
0; struct Cyc_Tcpat_Pos_struct{ int tag; struct Cyc_Tcpat_Con_s* f1; struct Cyc_List_List*
f2; } ; static const int Cyc_Tcpat_Neg= 1; struct Cyc_Tcpat_Neg_struct{ int tag;
struct Cyc_Set_Set* f1; } ; typedef void* Cyc_Tcpat_term_desc_t; typedef struct
Cyc_List_List* Cyc_Tcpat_context_t; typedef struct Cyc_List_List* Cyc_Tcpat_access_t;
static const int Cyc_Tcpat_Failure= 0; static const int Cyc_Tcpat_Success= 0;
struct Cyc_Tcpat_Success_struct{ int tag; void* f1; } ; static const int Cyc_Tcpat_IfEq=
1; struct Cyc_Tcpat_IfEq_struct{ int tag; struct Cyc_List_List* f1; struct Cyc_Tcpat_Con_s*
f2; void* f3; void* f4; } ; typedef void* Cyc_Tcpat_decision_t; struct _tuple9{
struct Cyc_List_List* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3; }
; typedef struct _tuple9* Cyc_Tcpat_work_stack_frame_t; typedef struct Cyc_List_List*
Cyc_Tcpat_work_stack_t; static const int Cyc_Tcpat_Yes= 0; static const int Cyc_Tcpat_No=
1; static const int Cyc_Tcpat_Maybe= 2; typedef void* Cyc_Tcpat_answer_t; static
void* Cyc_Tcpat_add_neg( void* td, struct Cyc_Tcpat_Con_s* c){ void* _temp829=
td; struct Cyc_Set_Set* _temp835; struct Cyc_List_List* _temp837; struct Cyc_Tcpat_Con_s*
_temp839; _LL831: if(*(( int*) _temp829) == Cyc_Tcpat_Neg){ _LL836: _temp835=(
struct Cyc_Set_Set*)(( struct Cyc_Tcpat_Neg_struct*) _temp829)->f1; goto _LL832;}
else{ goto _LL833;} _LL833: if(*(( int*) _temp829) == Cyc_Tcpat_Pos){ _LL840:
_temp839=( struct Cyc_Tcpat_Con_s*)(( struct Cyc_Tcpat_Pos_struct*) _temp829)->f1;
goto _LL838; _LL838: _temp837=( struct Cyc_List_List*)(( struct Cyc_Tcpat_Pos_struct*)
_temp829)->f2; goto _LL834;} else{ goto _LL830;} _LL832: if((( int(*)( struct
Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_member)( _temp835, c)){((
void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp841=( char*)"add_neg called when constructor already in set"; struct
_tagged_string _temp842; _temp842.curr= _temp841; _temp842.base= _temp841;
_temp842.last_plus_one= _temp841 + 47; _temp842;}));} if( c->span != 0?(( int(*)(
struct Cyc_Set_Set* s)) Cyc_Set_cardinality)( _temp835) + 1 >=( int)({ struct
Cyc_Core_Opt* _temp843= c->span; if( _temp843 == 0){ _throw( Null_Exception);}
_temp843->v;}): 0){(( void(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp844=( char*)"add_neg called when |cs U {c}| >= span(c)";
struct _tagged_string _temp845; _temp845.curr= _temp844; _temp845.base= _temp844;
_temp845.last_plus_one= _temp844 + 42; _temp845;}));} return( void*)({ struct
Cyc_Tcpat_Neg_struct* _temp846=( struct Cyc_Tcpat_Neg_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Neg_struct)); _temp846[ 0]=({ struct Cyc_Tcpat_Neg_struct
_temp847; _temp847.tag= Cyc_Tcpat_Neg; _temp847.f1=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_insert)( _temp835,
c); _temp847;}); _temp846;}); _LL834: return(( void*(*)( struct _tagged_string))
Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp848=( char*)"add_neg called when td is Positive";
struct _tagged_string _temp849; _temp849.curr= _temp848; _temp849.base= _temp848;
_temp849.last_plus_one= _temp848 + 35; _temp849;})); _LL830:;} static void* Cyc_Tcpat_static_match(
struct Cyc_Tcpat_Con_s* c, void* td){ void* _temp850= td; struct Cyc_List_List*
_temp856; struct Cyc_Tcpat_Con_s* _temp858; struct Cyc_Set_Set* _temp860; _LL852:
if(*(( int*) _temp850) == Cyc_Tcpat_Pos){ _LL859: _temp858=( struct Cyc_Tcpat_Con_s*)((
struct Cyc_Tcpat_Pos_struct*) _temp850)->f1; goto _LL857; _LL857: _temp856=(
struct Cyc_List_List*)(( struct Cyc_Tcpat_Pos_struct*) _temp850)->f2; goto
_LL853;} else{ goto _LL854;} _LL854: if(*(( int*) _temp850) == Cyc_Tcpat_Neg){
_LL861: _temp860=( struct Cyc_Set_Set*)(( struct Cyc_Tcpat_Neg_struct*) _temp850)->f1;
goto _LL855;} else{ goto _LL851;} _LL853: if( Cyc_Tcpat_compare_con( c, _temp858)
== 0){ return( void*) Cyc_Tcpat_Yes;} else{ return( void*) Cyc_Tcpat_No;} _LL855:
if((( int(*)( struct Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_member)(
_temp860, c)){ return( void*) Cyc_Tcpat_No;} else{ if( c->span != 0?( int)({
struct Cyc_Core_Opt* _temp862= c->span; if( _temp862 == 0){ _throw(
Null_Exception);} _temp862->v;}) ==(( int(*)( struct Cyc_Set_Set* s)) Cyc_Set_cardinality)(
_temp860) + 1: 0){ return( void*) Cyc_Tcpat_Yes;} else{ return( void*) Cyc_Tcpat_Maybe;}}
_LL851:;} struct _tuple10{ struct Cyc_Tcpat_Con_s* f1; struct Cyc_List_List* f2;
} ; static struct Cyc_List_List* Cyc_Tcpat_augment( struct Cyc_List_List* ctxt,
void* dsc){ struct Cyc_List_List* _temp863= ctxt; struct Cyc_List_List _temp869;
struct Cyc_List_List* _temp870; struct _tuple10* _temp872; struct _tuple10
_temp874; struct Cyc_List_List* _temp875; struct Cyc_Tcpat_Con_s* _temp877;
_LL865: if( _temp863 == 0){ goto _LL866;} else{ goto _LL867;} _LL867: if(
_temp863 == 0){ goto _LL864;} else{ _temp869=* _temp863; _LL873: _temp872=(
struct _tuple10*) _temp869.hd; _temp874=* _temp872; _LL878: _temp877= _temp874.f1;
goto _LL876; _LL876: _temp875= _temp874.f2; goto _LL871; _LL871: _temp870=(
struct Cyc_List_List*) _temp869.tl; goto _LL868;} _LL866: return 0; _LL868:
return({ struct Cyc_List_List* _temp879=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp879->hd=( void*)({ struct _tuple10*
_temp880=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp880->f1=
_temp877; _temp880->f2=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp881=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp881->hd=(
void*) dsc; _temp881->tl= _temp875; _temp881;}); _temp880;}); _temp879->tl=
_temp870; _temp879;}); _LL864:;} static struct Cyc_List_List* Cyc_Tcpat_norm_context(
struct Cyc_List_List* ctxt){ struct Cyc_List_List* _temp882= ctxt; struct Cyc_List_List
_temp888; struct Cyc_List_List* _temp889; struct _tuple10* _temp891; struct
_tuple10 _temp893; struct Cyc_List_List* _temp894; struct Cyc_Tcpat_Con_s*
_temp896; _LL884: if( _temp882 == 0){ goto _LL885;} else{ goto _LL886;} _LL886:
if( _temp882 == 0){ goto _LL883;} else{ _temp888=* _temp882; _LL892: _temp891=(
struct _tuple10*) _temp888.hd; _temp893=* _temp891; _LL897: _temp896= _temp893.f1;
goto _LL895; _LL895: _temp894= _temp893.f2; goto _LL890; _LL890: _temp889=(
struct Cyc_List_List*) _temp888.tl; goto _LL887;} _LL885: return(( struct Cyc_List_List*(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp898=( char*)"norm_context: empty context"; struct _tagged_string _temp899;
_temp899.curr= _temp898; _temp899.base= _temp898; _temp899.last_plus_one=
_temp898 + 28; _temp899;})); _LL887: return Cyc_Tcpat_augment( _temp889,( void*)({
struct Cyc_Tcpat_Pos_struct* _temp900=( struct Cyc_Tcpat_Pos_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Pos_struct)); _temp900[ 0]=({ struct Cyc_Tcpat_Pos_struct
_temp901; _temp901.tag= Cyc_Tcpat_Pos; _temp901.f1= _temp896; _temp901.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( _temp894);
_temp901;}); _temp900;})); _LL883:;} static void* Cyc_Tcpat_build_desc( struct
Cyc_List_List* ctxt, void* dsc, struct Cyc_List_List* work){ struct _tuple5
_temp903=({ struct _tuple5 _temp902; _temp902.f1= ctxt; _temp902.f2= work;
_temp902;}); struct Cyc_List_List* _temp913; struct Cyc_List_List* _temp915;
struct Cyc_List_List* _temp917; struct Cyc_List_List* _temp919; struct Cyc_List_List*
_temp921; struct Cyc_List_List* _temp923; struct Cyc_List_List* _temp925; struct
Cyc_List_List _temp927; struct Cyc_List_List* _temp928; struct _tuple9* _temp930;
struct _tuple9 _temp932; struct Cyc_List_List* _temp933; struct Cyc_List_List*
_temp935; struct Cyc_List_List* _temp937; struct Cyc_List_List* _temp939; struct
Cyc_List_List _temp941; struct Cyc_List_List* _temp942; struct _tuple10*
_temp944; struct _tuple10 _temp946; struct Cyc_List_List* _temp947; struct Cyc_Tcpat_Con_s*
_temp949; _LL905: _LL916: _temp915= _temp903.f1; if( _temp915 == 0){ goto _LL914;}
else{ goto _LL907;} _LL914: _temp913= _temp903.f2; if( _temp913 == 0){ goto
_LL906;} else{ goto _LL907;} _LL907: _LL920: _temp919= _temp903.f1; if( _temp919
== 0){ goto _LL918;} else{ goto _LL909;} _LL918: _temp917= _temp903.f2; goto
_LL908; _LL909: _LL924: _temp923= _temp903.f1; goto _LL922; _LL922: _temp921=
_temp903.f2; if( _temp921 == 0){ goto _LL910;} else{ goto _LL911;} _LL911:
_LL940: _temp939= _temp903.f1; if( _temp939 == 0){ goto _LL904;} else{ _temp941=*
_temp939; _LL945: _temp944=( struct _tuple10*) _temp941.hd; _temp946=* _temp944;
_LL950: _temp949= _temp946.f1; goto _LL948; _LL948: _temp947= _temp946.f2; goto
_LL943; _LL943: _temp942=( struct Cyc_List_List*) _temp941.tl; goto _LL926;}
_LL926: _temp925= _temp903.f2; if( _temp925 == 0){ goto _LL904;} else{ _temp927=*
_temp925; _LL931: _temp930=( struct _tuple9*) _temp927.hd; _temp932=* _temp930;
_LL938: _temp937= _temp932.f1; goto _LL936; _LL936: _temp935= _temp932.f2; goto
_LL934; _LL934: _temp933= _temp932.f3; goto _LL929; _LL929: _temp928=( struct
Cyc_List_List*) _temp927.tl; goto _LL912;} _LL906: return dsc; _LL908: return((
void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp951=( char*)"build_desc: ctxt and work don't match"; struct
_tagged_string _temp952; _temp952.curr= _temp951; _temp952.base= _temp951;
_temp952.last_plus_one= _temp951 + 38; _temp952;})); _LL910: return(( void*(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp953=( char*)"build_desc: ctxt and work don't match"; struct _tagged_string
_temp954; _temp954.curr= _temp953; _temp954.base= _temp953; _temp954.last_plus_one=
_temp953 + 38; _temp954;})); _LL912: { struct Cyc_Tcpat_Pos_struct* td=({ struct
Cyc_Tcpat_Pos_struct* _temp955=( struct Cyc_Tcpat_Pos_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Pos_struct)); _temp955[ 0]=({ struct Cyc_Tcpat_Pos_struct
_temp956; _temp956.tag= Cyc_Tcpat_Pos; _temp956.f1= _temp949; _temp956.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( _temp947),({
struct Cyc_List_List* _temp957=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp957->hd=( void*) dsc; _temp957->tl= _temp933;
_temp957;})); _temp956;}); _temp955;}); return Cyc_Tcpat_build_desc( _temp942,(
void*) td, _temp928);} _LL904:;} static void* Cyc_Tcpat_match( void* p, struct
Cyc_List_List* obj, void* dsc, struct Cyc_List_List* ctx, struct Cyc_List_List*
work, void* right_hand_side, struct Cyc_List_List* rules); static void* Cyc_Tcpat_or_match(
void* dsc, struct Cyc_List_List* allmrules){ struct Cyc_List_List* _temp958=
allmrules; struct Cyc_List_List _temp964; struct Cyc_List_List* _temp965; struct
_tuple0* _temp967; struct _tuple0 _temp969; void* _temp970; void* _temp972;
_LL960: if( _temp958 == 0){ goto _LL961;} else{ goto _LL962;} _LL962: if(
_temp958 == 0){ goto _LL959;} else{ _temp964=* _temp958; _LL968: _temp967=(
struct _tuple0*) _temp964.hd; _temp969=* _temp967; _LL973: _temp972= _temp969.f1;
goto _LL971; _LL971: _temp970= _temp969.f2; goto _LL966; _LL966: _temp965=(
struct Cyc_List_List*) _temp964.tl; goto _LL963;} _LL961: return( void*) Cyc_Tcpat_Failure;
_LL963: return(( void*(*)( void* p, struct Cyc_List_List* obj, void* dsc, struct
Cyc_List_List* ctx, struct Cyc_List_List* work, void* right_hand_side, struct
Cyc_List_List* rules)) Cyc_Tcpat_match)( _temp972, 0, dsc, 0, 0, _temp970,
_temp965); _LL959:;} static void* Cyc_Tcpat_match_compile( struct Cyc_List_List*
allmrules){ return(( void*(*)( void* dsc, struct Cyc_List_List* allmrules)) Cyc_Tcpat_or_match)((
void*)({ struct Cyc_Tcpat_Neg_struct* _temp974=( struct Cyc_Tcpat_Neg_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Neg_struct)); _temp974[ 0]=({ struct Cyc_Tcpat_Neg_struct
_temp975; _temp975.tag= Cyc_Tcpat_Neg; _temp975.f1= Cyc_Tcpat_empty_con_set();
_temp975;}); _temp974;}), allmrules);} static void* Cyc_Tcpat_and_match( struct
Cyc_List_List* ctx, struct Cyc_List_List* work, void* right_hand_side, struct
Cyc_List_List* rules){ struct Cyc_List_List* _temp976= work; struct Cyc_List_List
_temp984; struct Cyc_List_List* _temp985; struct _tuple9* _temp987; struct
_tuple9 _temp989; struct Cyc_List_List* _temp990; struct Cyc_List_List* _temp992;
struct Cyc_List_List* _temp994; struct Cyc_List_List _temp996; struct Cyc_List_List*
_temp997; struct _tuple9* _temp999; struct _tuple9 _temp1001; struct Cyc_List_List*
_temp1002; struct Cyc_List_List* _temp1004; struct Cyc_List_List* _temp1006;
_LL978: if( _temp976 == 0){ goto _LL979;} else{ goto _LL980;} _LL980: if(
_temp976 == 0){ goto _LL982;} else{ _temp984=* _temp976; _LL988: _temp987=(
struct _tuple9*) _temp984.hd; _temp989=* _temp987; _LL995: _temp994= _temp989.f1;
if( _temp994 == 0){ goto _LL993;} else{ goto _LL982;} _LL993: _temp992= _temp989.f2;
if( _temp992 == 0){ goto _LL991;} else{ goto _LL982;} _LL991: _temp990= _temp989.f3;
if( _temp990 == 0){ goto _LL986;} else{ goto _LL982;} _LL986: _temp985=( struct
Cyc_List_List*) _temp984.tl; goto _LL981;} _LL982: if( _temp976 == 0){ goto
_LL977;} else{ _temp996=* _temp976; _LL1000: _temp999=( struct _tuple9*)
_temp996.hd; _temp1001=* _temp999; _LL1007: _temp1006= _temp1001.f1; goto
_LL1005; _LL1005: _temp1004= _temp1001.f2; goto _LL1003; _LL1003: _temp1002=
_temp1001.f3; goto _LL998; _LL998: _temp997=( struct Cyc_List_List*) _temp996.tl;
goto _LL983;} _LL979: return( void*)({ struct Cyc_Tcpat_Success_struct*
_temp1008=( struct Cyc_Tcpat_Success_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Success_struct));
_temp1008[ 0]=({ struct Cyc_Tcpat_Success_struct _temp1009; _temp1009.tag= Cyc_Tcpat_Success;
_temp1009.f1=( void*) right_hand_side; _temp1009;}); _temp1008;}); _LL981:
return(( void*(*)( struct Cyc_List_List* ctx, struct Cyc_List_List* work, void*
right_hand_side, struct Cyc_List_List* rules)) Cyc_Tcpat_and_match)( Cyc_Tcpat_norm_context(
ctx), _temp985, right_hand_side, rules); _LL983: if(( _temp1006 == 0? 1:
_temp1004 == 0)? 1: _temp1002 == 0){ return(( void*(*)( struct _tagged_string))
Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp1010=( char*)"tcpat:and_match: malformed work frame";
struct _tagged_string _temp1011; _temp1011.curr= _temp1010; _temp1011.base=
_temp1010; _temp1011.last_plus_one= _temp1010 + 38; _temp1011;}));}{ struct Cyc_List_List
_temp1015; struct Cyc_List_List* _temp1016; void* _temp1018; struct Cyc_List_List*
_temp1013=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp1012= _temp1006;
if( _temp1012 == 0){ _throw( Null_Exception);} _temp1012;}); _temp1015=*
_temp1013; _LL1019: _temp1018=( void*) _temp1015.hd; goto _LL1017; _LL1017:
_temp1016=( struct Cyc_List_List*) _temp1015.tl; goto _LL1014; _LL1014: { struct
Cyc_List_List _temp1023; struct Cyc_List_List* _temp1024; struct Cyc_List_List*
_temp1026; struct Cyc_List_List* _temp1021=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp1020= _temp1004; if( _temp1020 == 0){ _throw( Null_Exception);} _temp1020;});
_temp1023=* _temp1021; _LL1027: _temp1026=( struct Cyc_List_List*) _temp1023.hd;
goto _LL1025; _LL1025: _temp1024=( struct Cyc_List_List*) _temp1023.tl; goto
_LL1022; _LL1022: { struct Cyc_List_List _temp1031; struct Cyc_List_List*
_temp1032; void* _temp1034; struct Cyc_List_List* _temp1029=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp1028= _temp1002; if( _temp1028 == 0){ _throw(
Null_Exception);} _temp1028;}); _temp1031=* _temp1029; _LL1035: _temp1034=( void*)
_temp1031.hd; goto _LL1033; _LL1033: _temp1032=( struct Cyc_List_List*)
_temp1031.tl; goto _LL1030; _LL1030: { struct _tuple9* wf=({ struct _tuple9*
_temp1037=( struct _tuple9*) GC_malloc( sizeof( struct _tuple9)); _temp1037->f1=
_temp1016; _temp1037->f2= _temp1024; _temp1037->f3= _temp1032; _temp1037;});
return(( void*(*)( void* p, struct Cyc_List_List* obj, void* dsc, struct Cyc_List_List*
ctx, struct Cyc_List_List* work, void* right_hand_side, struct Cyc_List_List*
rules)) Cyc_Tcpat_match)( _temp1018, _temp1026, _temp1034, ctx,({ struct Cyc_List_List*
_temp1036=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1036->hd=( void*) wf; _temp1036->tl= _temp997; _temp1036;}),
right_hand_side, rules);}}}} _LL977:;} static struct Cyc_List_List* Cyc_Tcpat_getdargs(
struct Cyc_Tcpat_Con_s* pcon, void* dsc){ void* _temp1038= dsc; struct Cyc_Set_Set*
_temp1044; struct Cyc_List_List* _temp1046; struct Cyc_Tcpat_Con_s* _temp1048;
_LL1040: if(*(( int*) _temp1038) == Cyc_Tcpat_Neg){ _LL1045: _temp1044=( struct
Cyc_Set_Set*)(( struct Cyc_Tcpat_Neg_struct*) _temp1038)->f1; goto _LL1041;}
else{ goto _LL1042;} _LL1042: if(*(( int*) _temp1038) == Cyc_Tcpat_Pos){ _LL1049:
_temp1048=( struct Cyc_Tcpat_Con_s*)(( struct Cyc_Tcpat_Pos_struct*) _temp1038)->f1;
goto _LL1047; _LL1047: _temp1046=( struct Cyc_List_List*)(( struct Cyc_Tcpat_Pos_struct*)
_temp1038)->f2; goto _LL1043;} else{ goto _LL1039;} _LL1041: { void* any=( void*)({
struct Cyc_Tcpat_Neg_struct* _temp1051=( struct Cyc_Tcpat_Neg_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Neg_struct)); _temp1051[ 0]=({ struct Cyc_Tcpat_Neg_struct
_temp1052; _temp1052.tag= Cyc_Tcpat_Neg; _temp1052.f1= Cyc_Tcpat_empty_con_set();
_temp1052;}); _temp1051;}); struct Cyc_List_List* res= 0;{ int i= 0; for( 0; i <
pcon->arity; ++ i){ res=({ struct Cyc_List_List* _temp1050=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1050->hd=( void*) any; _temp1050->tl=
res; _temp1050;});}} return res;} _LL1043: return _temp1046; _LL1039:;} static
struct Cyc_List_List* Cyc_Tcpat_getoarg( struct Cyc_List_List* obj, int i){
return({ struct Cyc_List_List* _temp1053=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1053->hd=( void*)( i + 1); _temp1053->tl=
obj; _temp1053;});} static struct Cyc_List_List* Cyc_Tcpat_getoargs( struct Cyc_Tcpat_Con_s*
pcon, struct Cyc_List_List* obj){ return(( struct Cyc_List_List*(*)( int n,
struct Cyc_List_List*(* f)( struct Cyc_List_List*, int), struct Cyc_List_List*
env)) Cyc_List_tabulate_c)( pcon->arity, Cyc_Tcpat_getoarg, obj);} static void*
Cyc_Tcpat_match( void* p, struct Cyc_List_List* obj, void* dsc, struct Cyc_List_List*
ctx, struct Cyc_List_List* work, void* right_hand_side, struct Cyc_List_List*
rules){ void* _temp1054= p; struct Cyc_List_List* _temp1060; struct Cyc_Tcpat_Con_s*
_temp1062; _LL1056: if( _temp1054 ==( void*) Cyc_Tcpat_Any){ goto _LL1057;}
else{ goto _LL1058;} _LL1058: if(( unsigned int) _temp1054 > 1u?*(( int*)
_temp1054) == Cyc_Tcpat_Con: 0){ _LL1063: _temp1062=( struct Cyc_Tcpat_Con_s*)((
struct Cyc_Tcpat_Con_struct*) _temp1054)->f1; goto _LL1061; _LL1061: _temp1060=(
struct Cyc_List_List*)(( struct Cyc_Tcpat_Con_struct*) _temp1054)->f2; goto
_LL1059;} else{ goto _LL1055;} _LL1057: return(( void*(*)( struct Cyc_List_List*
ctx, struct Cyc_List_List* work, void* right_hand_side, struct Cyc_List_List*
rules)) Cyc_Tcpat_and_match)( Cyc_Tcpat_augment( ctx, dsc), work,
right_hand_side, rules); _LL1059: { void* _temp1064= Cyc_Tcpat_static_match(
_temp1062, dsc); _LL1066: if( _temp1064 ==( void*) Cyc_Tcpat_Yes){ goto _LL1067;}
else{ goto _LL1068;} _LL1068: if( _temp1064 ==( void*) Cyc_Tcpat_No){ goto
_LL1069;} else{ goto _LL1070;} _LL1070: if( _temp1064 ==( void*) Cyc_Tcpat_Maybe){
goto _LL1071;} else{ goto _LL1065;} _LL1067: { struct Cyc_List_List* ctx2=({
struct Cyc_List_List* _temp1074=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1074->hd=( void*)({ struct _tuple10* _temp1075=(
struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp1075->f1= _temp1062;
_temp1075->f2= 0; _temp1075;}); _temp1074->tl= ctx; _temp1074;}); struct _tuple9*
work_frame=({ struct _tuple9* _temp1073=( struct _tuple9*) GC_malloc( sizeof(
struct _tuple9)); _temp1073->f1= _temp1060; _temp1073->f2= Cyc_Tcpat_getoargs(
_temp1062, obj); _temp1073->f3= Cyc_Tcpat_getdargs( _temp1062, dsc); _temp1073;});
struct Cyc_List_List* work2=({ struct Cyc_List_List* _temp1072=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1072->hd=( void*) work_frame;
_temp1072->tl= work; _temp1072;}); return(( void*(*)( struct Cyc_List_List* ctx,
struct Cyc_List_List* work, void* right_hand_side, struct Cyc_List_List* rules))
Cyc_Tcpat_and_match)( ctx2, work2, right_hand_side, rules);} _LL1069: return((
void*(*)( void* dsc, struct Cyc_List_List* allmrules)) Cyc_Tcpat_or_match)( Cyc_Tcpat_build_desc(
ctx, dsc, work), rules); _LL1071: { struct Cyc_List_List* ctx2=({ struct Cyc_List_List*
_temp1080=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1080->hd=( void*)({ struct _tuple10* _temp1081=( struct _tuple10*)
GC_malloc( sizeof( struct _tuple10)); _temp1081->f1= _temp1062; _temp1081->f2= 0;
_temp1081;}); _temp1080->tl= ctx; _temp1080;}); struct _tuple9* work_frame=({
struct _tuple9* _temp1079=( struct _tuple9*) GC_malloc( sizeof( struct _tuple9));
_temp1079->f1= _temp1060; _temp1079->f2= Cyc_Tcpat_getoargs( _temp1062, obj);
_temp1079->f3= Cyc_Tcpat_getdargs( _temp1062, dsc); _temp1079;}); struct Cyc_List_List*
work2=({ struct Cyc_List_List* _temp1078=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1078->hd=( void*) work_frame; _temp1078->tl=
work; _temp1078;}); void* s=(( void*(*)( struct Cyc_List_List* ctx, struct Cyc_List_List*
work, void* right_hand_side, struct Cyc_List_List* rules)) Cyc_Tcpat_and_match)(
ctx2, work2, right_hand_side, rules); void* f=(( void*(*)( void* dsc, struct Cyc_List_List*
allmrules)) Cyc_Tcpat_or_match)( Cyc_Tcpat_build_desc( ctx, Cyc_Tcpat_add_neg(
dsc, _temp1062), work), rules); return( void*)({ struct Cyc_Tcpat_IfEq_struct*
_temp1076=( struct Cyc_Tcpat_IfEq_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_IfEq_struct));
_temp1076[ 0]=({ struct Cyc_Tcpat_IfEq_struct _temp1077; _temp1077.tag= Cyc_Tcpat_IfEq;
_temp1077.f1= obj; _temp1077.f2= _temp1062; _temp1077.f3=( void*) s; _temp1077.f4=(
void*) f; _temp1077;}); _temp1076;});} _LL1065:;} _LL1055:;} static void Cyc_Tcpat_check_exhaust_overlap(
void* d, void(* not_exhaust)( void*), void* env1, void(* rhs_appears)( void*,
void*), void* env2){ void* _temp1082= d; void* _temp1090; void* _temp1092; void*
_temp1094; struct Cyc_Tcpat_Con_s* _temp1096; struct Cyc_List_List* _temp1098;
_LL1084: if( _temp1082 ==( void*) Cyc_Tcpat_Failure){ goto _LL1085;} else{ goto
_LL1086;} _LL1086: if(( unsigned int) _temp1082 > 1u?*(( int*) _temp1082) == Cyc_Tcpat_Success:
0){ _LL1091: _temp1090=( void*)(( struct Cyc_Tcpat_Success_struct*) _temp1082)->f1;
goto _LL1087;} else{ goto _LL1088;} _LL1088: if(( unsigned int) _temp1082 > 1u?*((
int*) _temp1082) == Cyc_Tcpat_IfEq: 0){ _LL1099: _temp1098=( struct Cyc_List_List*)((
struct Cyc_Tcpat_IfEq_struct*) _temp1082)->f1; goto _LL1097; _LL1097: _temp1096=(
struct Cyc_Tcpat_Con_s*)(( struct Cyc_Tcpat_IfEq_struct*) _temp1082)->f2; goto
_LL1095; _LL1095: _temp1094=( void*)(( struct Cyc_Tcpat_IfEq_struct*) _temp1082)->f3;
goto _LL1093; _LL1093: _temp1092=( void*)(( struct Cyc_Tcpat_IfEq_struct*)
_temp1082)->f4; goto _LL1089;} else{ goto _LL1083;} _LL1085: not_exhaust( env1);
goto _LL1083; _LL1087: rhs_appears( env2, _temp1090); goto _LL1083; _LL1089:((
void(*)( void* d, void(* not_exhaust)( void*), void* env1, void(* rhs_appears)(
void*, void*), void* env2)) Cyc_Tcpat_check_exhaust_overlap)( _temp1094,
not_exhaust, env1, rhs_appears, env2);(( void(*)( void* d, void(* not_exhaust)(
void*), void* env1, void(* rhs_appears)( void*, void*), void* env2)) Cyc_Tcpat_check_exhaust_overlap)(
_temp1092, not_exhaust, env1, rhs_appears, env2); goto _LL1083; _LL1083:;}
struct _tuple11{ int f1; struct Cyc_Position_Segment* f2; } ; struct _tuple12{
void* f1; struct _tuple11* f2; } ; static struct _tuple12* Cyc_Tcpat_get_match(
int* r, struct Cyc_Absyn_Switch_clause* swc){ void* sp0= Cyc_Tcpat_compile_pat(
swc->pattern); struct _tuple11* rhs=({ struct _tuple11* _temp1105=( struct
_tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp1105->f1= 0; _temp1105->f2=(
swc->pattern)->loc; _temp1105;}); void* sp; if( swc->where_clause != 0){ sp= Cyc_Tcpat_tuple_pat(({
struct Cyc_List_List* _temp1100=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1100->hd=( void*) sp0; _temp1100->tl=({ struct Cyc_List_List*
_temp1101=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1101->hd=( void*) Cyc_Tcpat_int_pat(* r); _temp1101->tl= 0; _temp1101;});
_temp1100;}));* r=* r + 1;} else{ sp= Cyc_Tcpat_tuple_pat(({ struct Cyc_List_List*
_temp1102=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1102->hd=( void*) sp0; _temp1102->tl=({ struct Cyc_List_List* _temp1103=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1103->hd=(
void*)(( void*) Cyc_Tcpat_Any); _temp1103->tl= 0; _temp1103;}); _temp1102;}));}
return({ struct _tuple12* _temp1104=( struct _tuple12*) GC_malloc( sizeof(
struct _tuple12)); _temp1104->f1= sp; _temp1104->f2= rhs; _temp1104;});} static
void Cyc_Tcpat_not_exhaust_err( struct Cyc_Position_Segment* loc){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp1106=( char*)"patterns may not be exhaustive";
struct _tagged_string _temp1107; _temp1107.curr= _temp1106; _temp1107.base=
_temp1106; _temp1107.last_plus_one= _temp1106 + 31; _temp1107;}));} static void
Cyc_Tcpat_rule_occurs( int dummy, struct _tuple11* rhs){(* rhs).f1= 1;} void Cyc_Tcpat_check_switch_exhaustive(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* swcs){ int where_ctr= 0;
struct Cyc_List_List* match_rules=(( struct Cyc_List_List*(*)( struct _tuple12*(*
f)( int*, struct Cyc_Absyn_Switch_clause*), int* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Tcpat_get_match,& where_ctr, swcs); void* dec_tree=(( void*(*)(
struct Cyc_List_List* allmrules)) Cyc_Tcpat_match_compile)( match_rules);(( void(*)(
void* d, void(* not_exhaust)( struct Cyc_Position_Segment*), struct Cyc_Position_Segment*
env1, void(* rhs_appears)( int, struct _tuple11*), int env2)) Cyc_Tcpat_check_exhaust_overlap)(
dec_tree, Cyc_Tcpat_not_exhaust_err, loc, Cyc_Tcpat_rule_occurs, 0); for( 0;
match_rules != 0; match_rules=({ struct Cyc_List_List* _temp1108= match_rules;
if( _temp1108 == 0){ _throw( Null_Exception);} _temp1108->tl;})){ struct
_tuple12 _temp1112; struct _tuple11* _temp1113; struct _tuple11 _temp1115;
struct Cyc_Position_Segment* _temp1116; int _temp1118; void* _temp1120; struct
_tuple12* _temp1110=( struct _tuple12*)({ struct Cyc_List_List* _temp1109=
match_rules; if( _temp1109 == 0){ _throw( Null_Exception);} _temp1109->hd;});
_temp1112=* _temp1110; _LL1121: _temp1120= _temp1112.f1; goto _LL1114; _LL1114:
_temp1113= _temp1112.f2; _temp1115=* _temp1113; _LL1119: _temp1118= _temp1115.f1;
goto _LL1117; _LL1117: _temp1116= _temp1115.f2; goto _LL1111; _LL1111: if( !
_temp1118){ Cyc_Tcutil_terr( _temp1116,( struct _tagged_string)({ char*
_temp1122=( char*)"redundant pattern"; struct _tagged_string _temp1123;
_temp1123.curr= _temp1122; _temp1123.base= _temp1122; _temp1123.last_plus_one=
_temp1122 + 18; _temp1123;}));}}} struct _tuple13{ struct Cyc_Position_Segment*
f1; int f2; } ; static void Cyc_Tcpat_not_exhaust_warn( struct _tuple13* pr){(*
pr).f2= 0; Cyc_Tcutil_warn((* pr).f1,( struct _tagged_string)({ char* _temp1124=(
char*)"pattern not exhaustive"; struct _tagged_string _temp1125; _temp1125.curr=
_temp1124; _temp1125.base= _temp1124; _temp1125.last_plus_one= _temp1124 + 23;
_temp1125;}));} static void Cyc_Tcpat_dummy_fn( int i, int j){ return;} struct
_tuple14{ void* f1; int f2; } ; int Cyc_Tcpat_check_let_pat_exhaustive( struct
Cyc_Position_Segment* loc, struct Cyc_Absyn_Pat* p){ struct Cyc_List_List*
match_rules=({ struct Cyc_List_List* _temp1127=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1127->hd=( void*)({ struct
_tuple14* _temp1128=( struct _tuple14*) GC_malloc( sizeof( struct _tuple14));
_temp1128->f1= Cyc_Tcpat_compile_pat( p); _temp1128->f2= 0; _temp1128;});
_temp1127->tl= 0; _temp1127;}); void* dec_tree=(( void*(*)( struct Cyc_List_List*
allmrules)) Cyc_Tcpat_match_compile)( match_rules); struct _tuple13* exhaust_env=({
struct _tuple13* _temp1126=( struct _tuple13*) GC_malloc( sizeof( struct
_tuple13)); _temp1126->f1= loc; _temp1126->f2= 1; _temp1126;});(( void(*)( void*
d, void(* not_exhaust)( struct _tuple13*), struct _tuple13* env1, void(*
rhs_appears)( int, int), int env2)) Cyc_Tcpat_check_exhaust_overlap)( dec_tree,
Cyc_Tcpat_not_exhaust_warn, exhaust_env, Cyc_Tcpat_dummy_fn, 0); return(*
exhaust_env).f2;} static struct _tuple12* Cyc_Tcpat_get_match2( struct Cyc_Absyn_Switch_clause*
swc){ void* sp0= Cyc_Tcpat_compile_pat( swc->pattern); struct _tuple11* rhs=({
struct _tuple11* _temp1130=( struct _tuple11*) GC_malloc( sizeof( struct
_tuple11)); _temp1130->f1= 0; _temp1130->f2=( swc->pattern)->loc; _temp1130;});
return({ struct _tuple12* _temp1129=( struct _tuple12*) GC_malloc( sizeof(
struct _tuple12)); _temp1129->f1= sp0; _temp1129->f2= rhs; _temp1129;});} static
void Cyc_Tcpat_not_exhaust_err2( struct Cyc_Position_Segment* loc){;} void Cyc_Tcpat_check_catch_overlap(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* swcs){ struct Cyc_List_List*
match_rules=(( struct Cyc_List_List*(*)( struct _tuple12*(* f)( struct Cyc_Absyn_Switch_clause*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_get_match2, swcs); void*
dec_tree=(( void*(*)( struct Cyc_List_List* allmrules)) Cyc_Tcpat_match_compile)(
match_rules);(( void(*)( void* d, void(* not_exhaust)( struct Cyc_Position_Segment*),
struct Cyc_Position_Segment* env1, void(* rhs_appears)( int, struct _tuple11*),
int env2)) Cyc_Tcpat_check_exhaust_overlap)( dec_tree, Cyc_Tcpat_not_exhaust_err2,
loc, Cyc_Tcpat_rule_occurs, 0); for( 0; match_rules != 0; match_rules=({ struct
Cyc_List_List* _temp1131= match_rules; if( _temp1131 == 0){ _throw(
Null_Exception);} _temp1131->tl;})){ struct _tuple12 _temp1135; struct _tuple11*
_temp1136; struct _tuple11 _temp1138; struct Cyc_Position_Segment* _temp1139;
int _temp1141; void* _temp1143; struct _tuple12* _temp1133=( struct _tuple12*)({
struct Cyc_List_List* _temp1132= match_rules; if( _temp1132 == 0){ _throw(
Null_Exception);} _temp1132->hd;}); _temp1135=* _temp1133; _LL1144: _temp1143=
_temp1135.f1; goto _LL1137; _LL1137: _temp1136= _temp1135.f2; _temp1138=*
_temp1136; _LL1142: _temp1141= _temp1138.f1; goto _LL1140; _LL1140: _temp1139=
_temp1138.f2; goto _LL1134; _LL1134: if( ! _temp1141){ Cyc_Tcutil_terr(
_temp1139,( struct _tagged_string)({ char* _temp1145=( char*)"redundant pattern";
struct _tagged_string _temp1146; _temp1146.curr= _temp1145; _temp1146.base=
_temp1145; _temp1146.last_plus_one= _temp1145 + 18; _temp1146;}));}}}