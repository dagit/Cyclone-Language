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
extern char Cyc_Stdio_FileCloseError[ 19u]; extern char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1;
} ; extern int Cyc_String_strcmp( struct _tagged_string s1, struct
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
f2; struct Cyc_Absyn_Uniondecl** f3; } ; static const int Cyc_Absyn_AnonStructType=
11; struct Cyc_Absyn_AnonStructType_struct{ int tag; struct Cyc_List_List* f1; }
; static const int Cyc_Absyn_AnonUnionType= 12; struct Cyc_Absyn_AnonUnionType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_EnumType= 13;
struct Cyc_Absyn_EnumType_struct{ int tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl*
f2; } ; static const int Cyc_Absyn_RgnHandleType= 14; struct Cyc_Absyn_RgnHandleType_struct{
int tag; void* f1; } ; static const int Cyc_Absyn_TypedefType= 15; struct Cyc_Absyn_TypedefType_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3;
} ; static const int Cyc_Absyn_HeapRgn= 3; static const int Cyc_Absyn_AccessEff=
16; struct Cyc_Absyn_AccessEff_struct{ int tag; void* f1; } ; static const int
Cyc_Absyn_JoinEff= 17; struct Cyc_Absyn_JoinEff_struct{ int tag; struct Cyc_List_List*
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
struct Cyc_Absyn_Structdecl* f4; } ; static const int Cyc_Absyn_AnonStruct_e= 27;
struct Cyc_Absyn_AnonStruct_e_struct{ int tag; void* f1; struct Cyc_List_List*
f2; } ; static const int Cyc_Absyn_Tunion_e= 28; struct Cyc_Absyn_Tunion_e_struct{
int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List*
f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield* f5; } ;
static const int Cyc_Absyn_Enum_e= 29; struct Cyc_Absyn_Enum_e_struct{ int tag;
struct _tuple1* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
f3; } ; static const int Cyc_Absyn_Malloc_e= 30; struct Cyc_Absyn_Malloc_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; void* f2; } ; static const int Cyc_Absyn_UnresolvedMem_e=
31; struct Cyc_Absyn_UnresolvedMem_e_struct{ int tag; struct Cyc_Core_Opt* f1;
struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_StmtExp_e= 32; struct
Cyc_Absyn_StmtExp_e_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; static const
int Cyc_Absyn_Codegen_e= 33; struct Cyc_Absyn_Codegen_e_struct{ int tag; struct
Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Fill_e= 34; struct Cyc_Absyn_Fill_e_struct{
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
struct Cyc_Position_Segment* loc; void* sc; } ; struct Cyc_Absyn_Tuniondecl{
void* sc; struct _tuple1* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
fields; int is_xtunion; } ; struct Cyc_Absyn_Enumfield{ struct _tuple1* name;
struct Cyc_Absyn_Exp* tag; struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_Enumdecl{
void* sc; struct _tuple1* name; struct Cyc_Core_Opt* fields; } ; struct Cyc_Absyn_Typedefdecl{
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
des, struct Cyc_List_List* fields); struct _tuple5{ struct Cyc_List_List* f1;
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
_handler_cons _temp24; _push_handler(& _temp24);{ int _temp26= 0; if( setjmp(
_temp24.handler)){ _temp26= 1;} if( ! _temp26){{ void* _temp27= Cyc_Tcenv_lookup_ordinary(
te, p->loc, _temp10); struct Cyc_Absyn_Structdecl* _temp37; struct Cyc_Absyn_Tunionfield*
_temp39; struct Cyc_Absyn_Tuniondecl* _temp41; struct Cyc_Absyn_Enumfield*
_temp43; struct Cyc_Absyn_Enumdecl* _temp45; void* _temp47; _LL29: if(*(( int*)
_temp27) == Cyc_Tcenv_StructRes){ _LL38: _temp37=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Tcenv_StructRes_struct*) _temp27)->f1; goto _LL30;} else{ goto _LL31;}
_LL31: if(*(( int*) _temp27) == Cyc_Tcenv_TunionRes){ _LL42: _temp41=( struct
Cyc_Absyn_Tuniondecl*)(( struct Cyc_Tcenv_TunionRes_struct*) _temp27)->f1; goto
_LL40; _LL40: _temp39=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Tcenv_TunionRes_struct*)
_temp27)->f2; goto _LL32;} else{ goto _LL33;} _LL33: if(*(( int*) _temp27) ==
Cyc_Tcenv_EnumRes){ _LL46: _temp45=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Tcenv_EnumRes_struct*)
_temp27)->f1; goto _LL44; _LL44: _temp43=( struct Cyc_Absyn_Enumfield*)(( struct
Cyc_Tcenv_EnumRes_struct*) _temp27)->f2; goto _LL34;} else{ goto _LL35;} _LL35:
if(*(( int*) _temp27) == Cyc_Tcenv_VarRes){ _LL48: _temp47=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp27)->f1; goto _LL36;} else{ goto _LL28;} _LL30: Cyc_Tcutil_terr( p->loc,(
struct _tagged_string)({ char* _temp49=( char*)"struct tag used without arguments in pattern";
struct _tagged_string _temp50; _temp50.curr= _temp49; _temp50.base= _temp49;
_temp50.last_plus_one= _temp49 + 45; _temp50;})); _npop_handler( 0u); return;
_LL32:( void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Tunion_p_struct*
_temp51=( struct Cyc_Absyn_Tunion_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_p_struct));
_temp51[ 0]=({ struct Cyc_Absyn_Tunion_p_struct _temp52; _temp52.tag= Cyc_Absyn_Tunion_p;
_temp52.f1= _temp41; _temp52.f2= _temp39; _temp52.f3= 0; _temp52.f4= 0; _temp52;});
_temp51;}))); _npop_handler( 0u); return; _LL34:( void*)( p->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_p_struct* _temp53=( struct Cyc_Absyn_Enum_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_p_struct)); _temp53[ 0]=({ struct Cyc_Absyn_Enum_p_struct
_temp54; _temp54.tag= Cyc_Absyn_Enum_p; _temp54.f1= _temp45; _temp54.f2= _temp43;
_temp54;}); _temp53;}))); _npop_handler( 0u); return; _LL36: goto _LL28; _LL28:;};
_pop_handler();} else{ void* _temp25=( void*) _exn_thrown; void* _temp56=
_temp25; _LL58: if( _temp56 == Cyc_Dict_Absent){ goto _LL59;} else{ goto _LL60;}
_LL60: goto _LL61; _LL59: goto _LL57; _LL61:( void) _throw( _temp56); _LL57:;}}}{
void* _temp62=(* _temp10).f1; struct Cyc_List_List* _temp70; _LL64: if( _temp62
==( void*) Cyc_Absyn_Loc_n){ goto _LL65;} else{ goto _LL66;} _LL66: if((
unsigned int) _temp62 > 1u?*(( int*) _temp62) == Cyc_Absyn_Rel_n: 0){ _LL71:
_temp70=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp62)->f1;
if( _temp70 == 0){ goto _LL67;} else{ goto _LL68;}} else{ goto _LL68;} _LL68:
goto _LL69; _LL65: goto _LL67; _LL67:(* _temp10).f1=( void*) Cyc_Absyn_Loc_n;(
void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Var_p_struct* _temp72=( struct
Cyc_Absyn_Var_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct));
_temp72[ 0]=({ struct Cyc_Absyn_Var_p_struct _temp73; _temp73.tag= Cyc_Absyn_Var_p;
_temp73.f1= Cyc_Absyn_new_vardecl( _temp10,( void*) Cyc_Absyn_VoidType, 0);
_temp73;}); _temp72;}))); return; _LL69: Cyc_Tcutil_terr( p->loc,( struct
_tagged_string)({ char* _temp74=( char*)"qualified variable in pattern"; struct
_tagged_string _temp75; _temp75.curr= _temp74; _temp75.base= _temp74; _temp75.last_plus_one=
_temp74 + 30; _temp75;})); return; _LL63:;} _LL5:{ struct _handler_cons _temp76;
_push_handler(& _temp76);{ int _temp78= 0; if( setjmp( _temp76.handler)){
_temp78= 1;} if( ! _temp78){{ void* _temp79= Cyc_Tcenv_lookup_ordinary( te, p->loc,
_temp16); struct Cyc_Absyn_Structdecl* _temp89; struct Cyc_Absyn_Tunionfield*
_temp91; struct Cyc_Absyn_Tuniondecl* _temp93; struct Cyc_Absyn_Enumfield*
_temp95; struct Cyc_Absyn_Enumdecl* _temp97; void* _temp99; _LL81: if(*(( int*)
_temp79) == Cyc_Tcenv_StructRes){ _LL90: _temp89=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Tcenv_StructRes_struct*) _temp79)->f1; goto _LL82;} else{ goto _LL83;}
_LL83: if(*(( int*) _temp79) == Cyc_Tcenv_TunionRes){ _LL94: _temp93=( struct
Cyc_Absyn_Tuniondecl*)(( struct Cyc_Tcenv_TunionRes_struct*) _temp79)->f1; goto
_LL92; _LL92: _temp91=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Tcenv_TunionRes_struct*)
_temp79)->f2; goto _LL84;} else{ goto _LL85;} _LL85: if(*(( int*) _temp79) ==
Cyc_Tcenv_EnumRes){ _LL98: _temp97=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Tcenv_EnumRes_struct*)
_temp79)->f1; goto _LL96; _LL96: _temp95=( struct Cyc_Absyn_Enumfield*)(( struct
Cyc_Tcenv_EnumRes_struct*) _temp79)->f2; goto _LL86;} else{ goto _LL87;} _LL87:
if(*(( int*) _temp79) == Cyc_Tcenv_VarRes){ _LL100: _temp99=( void*)(( struct
Cyc_Tcenv_VarRes_struct*) _temp79)->f1; goto _LL88;} else{ goto _LL80;} _LL82: {
struct Cyc_List_List* _temp101= 0; goto _LL102; _LL102: for( 0; _temp12 != 0;
_temp12=({ struct Cyc_List_List* _temp103= _temp12; if( _temp103 == 0){ _throw(
Null_Exception);} _temp103->tl;})){ _temp101=({ struct Cyc_List_List* _temp104=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp104->hd=(
void*)({ struct _tuple6* _temp105=( struct _tuple6*) GC_malloc( sizeof( struct
_tuple6)); _temp105->f1= 0; _temp105->f2=( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List*
_temp106= _temp12; if( _temp106 == 0){ _throw( Null_Exception);} _temp106->hd;});
_temp105;}); _temp104->tl= _temp101; _temp104;});}( void*)( p->r=( void*)(( void*)({
struct Cyc_Absyn_Struct_p_struct* _temp107=( struct Cyc_Absyn_Struct_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_p_struct)); _temp107[ 0]=({ struct
Cyc_Absyn_Struct_p_struct _temp108; _temp108.tag= Cyc_Absyn_Struct_p; _temp108.f1=
_temp89; _temp108.f2= 0; _temp108.f3= _temp14; _temp108.f4=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp101); _temp108;}); _temp107;})));
_npop_handler( 0u); return;} _LL84:( void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Tunion_p_struct*
_temp109=( struct Cyc_Absyn_Tunion_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_p_struct));
_temp109[ 0]=({ struct Cyc_Absyn_Tunion_p_struct _temp110; _temp110.tag= Cyc_Absyn_Tunion_p;
_temp110.f1= _temp93; _temp110.f2= _temp91; _temp110.f3= _temp14; _temp110.f4=
_temp12; _temp110;}); _temp109;}))); _npop_handler( 0u); return; _LL86: Cyc_Tcutil_terr(
p->loc,({ struct _tagged_string _temp111= Cyc_Absynpp_qvar2string( _temp16);
xprintf("bad enum constructor %.*s in pattern", _temp111.last_plus_one -
_temp111.curr, _temp111.curr);})); _npop_handler( 0u); return; _LL88: goto _LL80;
_LL80:;}; _pop_handler();} else{ void* _temp77=( void*) _exn_thrown; void*
_temp113= _temp77; _LL115: if( _temp113 == Cyc_Dict_Absent){ goto _LL116;} else{
goto _LL117;} _LL117: goto _LL118; _LL116: goto _LL114; _LL118:( void) _throw(
_temp113); _LL114:;}}} Cyc_Tcutil_terr( p->loc,({ struct _tagged_string _temp119=
Cyc_Absynpp_qvar2string( _temp16); xprintf("%.*s is not a constructor in pattern",
_temp119.last_plus_one - _temp119.curr, _temp119.curr);})); return; _LL7:{
struct _handler_cons _temp120; _push_handler(& _temp120);{ int _temp122= 0; if(
setjmp( _temp120.handler)){ _temp122= 1;} if( ! _temp122){{ void* _temp123= Cyc_Tcenv_lookup_ordinary(
te, p->loc, _temp22); struct Cyc_Absyn_Structdecl* _temp129; _LL125: if(*(( int*)
_temp123) == Cyc_Tcenv_StructRes){ _LL130: _temp129=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Tcenv_StructRes_struct*) _temp123)->f1; goto _LL126;} else{ goto
_LL127;} _LL127: goto _LL128; _LL126:( void*)( p->r=( void*)(( void*)({ struct
Cyc_Absyn_Struct_p_struct* _temp131=( struct Cyc_Absyn_Struct_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_p_struct)); _temp131[ 0]=({ struct
Cyc_Absyn_Struct_p_struct _temp132; _temp132.tag= Cyc_Absyn_Struct_p; _temp132.f1=
_temp129; _temp132.f2= 0; _temp132.f3= _temp20; _temp132.f4= _temp18; _temp132;});
_temp131;}))); _npop_handler( 0u); return; _LL128: goto _LL124; _LL124:;};
_pop_handler();} else{ void* _temp121=( void*) _exn_thrown; void* _temp134=
_temp121; _LL136: if( _temp134 == Cyc_Dict_Absent){ goto _LL137;} else{ goto
_LL138;} _LL138: goto _LL139; _LL137: goto _LL135; _LL139:( void) _throw(
_temp134); _LL135:;}}} Cyc_Tcutil_terr( p->loc,({ struct _tagged_string _temp140=
Cyc_Absynpp_qvar2string( _temp22); xprintf("%.*s is not a struct name, but pattern has designators",
_temp140.last_plus_one - _temp140.curr, _temp140.curr);})); return; _LL9:
return; _LL1:;} static struct _tagged_string* Cyc_Tcpat_get_name( struct Cyc_Absyn_Vardecl*
vd){ return(* vd->name).f2;} static void* Cyc_Tcpat_any_type( void** topt){ if(
topt != 0){ return({ void** _temp141= topt; if( _temp141 == 0){ _throw(
Null_Exception);}* _temp141;});} return Cyc_Absyn_new_evar(( void*) Cyc_Absyn_MemKind);}
static void* Cyc_Tcpat_num_type( void** topt, void* numt){ if( topt != 0? Cyc_Tcutil_coerceable(({
void** _temp142= topt; if( _temp142 == 0){ _throw( Null_Exception);}* _temp142;})):
0){ return({ void** _temp143= topt; if( _temp143 == 0){ _throw( Null_Exception);}*
_temp143;});} return numt;} static void Cyc_Tcpat_set_vd( struct Cyc_Absyn_Vardecl*
vd, struct Cyc_List_List** v_result_ptr, void* t){( void*)( vd->type=( void*) t);
vd->tq= Cyc_Absyn_empty_tqual();* v_result_ptr=({ struct Cyc_List_List* _temp144=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp144->hd=(
void*) vd; _temp144->tl=* v_result_ptr; _temp144;});} struct _tuple7{ struct Cyc_Absyn_Tqual
f1; void* f2; } ; struct _tuple8{ struct Cyc_Absyn_Structfield* f1; struct Cyc_Absyn_Pat*
f2; } ; struct _tuple5 Cyc_Tcpat_tcPatRec( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat*
p, void** topt, void** region_opt){ Cyc_Tcpat_resolve_pat( te, p);{ void* t;
struct Cyc_List_List* tv_result= 0; struct Cyc_List_List* v_result= 0;{ void*
_temp145=( void*) p->r; struct Cyc_Absyn_Vardecl* _temp179; struct Cyc_Absyn_Vardecl*
_temp181; int _temp183; void* _temp185; int _temp187; void* _temp189; char
_temp191; struct _tagged_string _temp193; struct Cyc_Absyn_Enumfield* _temp195;
struct Cyc_Absyn_Enumdecl* _temp197; struct Cyc_Absyn_Pat* _temp199; struct Cyc_List_List*
_temp201; struct Cyc_List_List* _temp203; struct Cyc_List_List* _temp205; struct
Cyc_Core_Opt* _temp207; struct Cyc_Core_Opt** _temp209; struct Cyc_Absyn_Structdecl*
_temp210; struct Cyc_List_List* _temp212; struct Cyc_List_List* _temp214; struct
Cyc_Absyn_Tunionfield* _temp216; struct Cyc_Absyn_Tuniondecl* _temp218; struct
_tuple1* _temp220; struct Cyc_List_List* _temp222; struct Cyc_List_List*
_temp224; struct _tuple1* _temp226; struct Cyc_List_List* _temp228; struct Cyc_List_List*
_temp230; struct _tuple1* _temp232; _LL147: if( _temp145 ==( void*) Cyc_Absyn_Wild_p){
goto _LL148;} else{ goto _LL149;} _LL149: if(( unsigned int) _temp145 > 2u?*((
int*) _temp145) == Cyc_Absyn_Var_p: 0){ _LL180: _temp179=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_p_struct*) _temp145)->f1; goto _LL150;} else{ goto _LL151;}
_LL151: if(( unsigned int) _temp145 > 2u?*(( int*) _temp145) == Cyc_Absyn_Reference_p:
0){ _LL182: _temp181=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Reference_p_struct*)
_temp145)->f1; goto _LL152;} else{ goto _LL153;} _LL153: if(( unsigned int)
_temp145 > 2u?*(( int*) _temp145) == Cyc_Absyn_Int_p: 0){ _LL186: _temp185=(
void*)(( struct Cyc_Absyn_Int_p_struct*) _temp145)->f1; if( _temp185 ==( void*)
Cyc_Absyn_Unsigned){ goto _LL184;} else{ goto _LL155;} _LL184: _temp183=( int)((
struct Cyc_Absyn_Int_p_struct*) _temp145)->f2; goto _LL154;} else{ goto _LL155;}
_LL155: if(( unsigned int) _temp145 > 2u?*(( int*) _temp145) == Cyc_Absyn_Int_p:
0){ _LL190: _temp189=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp145)->f1;
if( _temp189 ==( void*) Cyc_Absyn_Signed){ goto _LL188;} else{ goto _LL157;}
_LL188: _temp187=( int)(( struct Cyc_Absyn_Int_p_struct*) _temp145)->f2; goto
_LL156;} else{ goto _LL157;} _LL157: if(( unsigned int) _temp145 > 2u?*(( int*)
_temp145) == Cyc_Absyn_Char_p: 0){ _LL192: _temp191=( char)(( struct Cyc_Absyn_Char_p_struct*)
_temp145)->f1; goto _LL158;} else{ goto _LL159;} _LL159: if(( unsigned int)
_temp145 > 2u?*(( int*) _temp145) == Cyc_Absyn_Float_p: 0){ _LL194: _temp193=(
struct _tagged_string)(( struct Cyc_Absyn_Float_p_struct*) _temp145)->f1; goto
_LL160;} else{ goto _LL161;} _LL161: if(( unsigned int) _temp145 > 2u?*(( int*)
_temp145) == Cyc_Absyn_Enum_p: 0){ _LL198: _temp197=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_p_struct*) _temp145)->f1; goto _LL196; _LL196: _temp195=(
struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_p_struct*) _temp145)->f2;
goto _LL162;} else{ goto _LL163;} _LL163: if( _temp145 ==( void*) Cyc_Absyn_Null_p){
goto _LL164;} else{ goto _LL165;} _LL165: if(( unsigned int) _temp145 > 2u?*((
int*) _temp145) == Cyc_Absyn_Pointer_p: 0){ _LL200: _temp199=( struct Cyc_Absyn_Pat*)((
struct Cyc_Absyn_Pointer_p_struct*) _temp145)->f1; goto _LL166;} else{ goto
_LL167;} _LL167: if(( unsigned int) _temp145 > 2u?*(( int*) _temp145) == Cyc_Absyn_Tuple_p:
0){ _LL202: _temp201=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*)
_temp145)->f1; goto _LL168;} else{ goto _LL169;} _LL169: if(( unsigned int)
_temp145 > 2u?*(( int*) _temp145) == Cyc_Absyn_Struct_p: 0){ _LL211: _temp210=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_p_struct*) _temp145)->f1;
goto _LL208; _LL208: _temp207=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp145)->f2; _temp209=&(( struct Cyc_Absyn_Struct_p_struct*) _temp145)->f2;
goto _LL206; _LL206: _temp205=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp145)->f3; goto _LL204; _LL204: _temp203=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Struct_p_struct*) _temp145)->f4; goto _LL170;} else{ goto _LL171;}
_LL171: if(( unsigned int) _temp145 > 2u?*(( int*) _temp145) == Cyc_Absyn_Tunion_p:
0){ _LL219: _temp218=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp145)->f1; goto _LL217; _LL217: _temp216=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp145)->f2; goto _LL215; _LL215: _temp214=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp145)->f3; goto
_LL213; _LL213: _temp212=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp145)->f4; goto _LL172;} else{ goto _LL173;} _LL173: if(( unsigned int)
_temp145 > 2u?*(( int*) _temp145) == Cyc_Absyn_UnknownId_p: 0){ _LL221: _temp220=(
struct _tuple1*)(( struct Cyc_Absyn_UnknownId_p_struct*) _temp145)->f1; goto
_LL174;} else{ goto _LL175;} _LL175: if(( unsigned int) _temp145 > 2u?*(( int*)
_temp145) == Cyc_Absyn_UnknownCall_p: 0){ _LL227: _temp226=( struct _tuple1*)((
struct Cyc_Absyn_UnknownCall_p_struct*) _temp145)->f1; goto _LL225; _LL225:
_temp224=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp145)->f2; goto _LL223; _LL223: _temp222=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnknownCall_p_struct*) _temp145)->f3; goto _LL176;} else{ goto _LL177;}
_LL177: if(( unsigned int) _temp145 > 2u?*(( int*) _temp145) == Cyc_Absyn_UnknownFields_p:
0){ _LL233: _temp232=( struct _tuple1*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp145)->f1; goto _LL231; _LL231: _temp230=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnknownFields_p_struct*) _temp145)->f2; goto _LL229; _LL229: _temp228=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp145)->f3;
goto _LL178;} else{ goto _LL146;} _LL148: t= Cyc_Tcpat_any_type( topt); goto
_LL146; _LL150: t= Cyc_Tcpat_any_type( topt); Cyc_Tcpat_set_vd( _temp179,&
v_result, t); goto _LL146; _LL152: t= Cyc_Tcpat_any_type( topt); if( region_opt
== 0){ Cyc_Tcutil_terr( p->loc,( struct _tagged_string)({ char* _temp234=( char*)"* pattern would point into an unknown region";
struct _tagged_string _temp235; _temp235.curr= _temp234; _temp235.base= _temp234;
_temp235.last_plus_one= _temp234 + 45; _temp235;})); goto _LL146;} Cyc_Tcpat_set_vd(
_temp181,& v_result,( void*)({ struct Cyc_Absyn_PointerType_struct* _temp236=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp236[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp237; _temp237.tag= Cyc_Absyn_PointerType;
_temp237.f1=({ struct Cyc_Absyn_PtrInfo _temp238; _temp238.elt_typ=( void*) t;
_temp238.rgn_typ=( void*)({ void** _temp239= region_opt; if( _temp239 == 0){
_throw( Null_Exception);}* _temp239;}); _temp238.nullable=(( struct Cyc_Absyn_Conref*(*)(
int x)) Cyc_Absyn_new_conref)( 0); _temp238.tq= Cyc_Absyn_empty_tqual();
_temp238.bounds=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp238;}); _temp237;}); _temp236;})); goto _LL146; _LL154: t= Cyc_Tcpat_num_type(
topt, Cyc_Absyn_uint_t); goto _LL146; _LL156: t= Cyc_Tcpat_num_type( topt, Cyc_Absyn_sint_t);
goto _LL146; _LL158: t= Cyc_Tcpat_num_type( topt, Cyc_Absyn_uchar_t); goto
_LL146; _LL160: t= Cyc_Tcpat_num_type( topt, Cyc_Absyn_float_t); goto _LL146;
_LL162: t=( void*)({ struct Cyc_Absyn_EnumType_struct* _temp240=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp240[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp241; _temp241.tag= Cyc_Absyn_EnumType; _temp241.f1=
_temp197->name; _temp241.f2=( struct Cyc_Absyn_Enumdecl*) _temp197; _temp241;});
_temp240;}); goto _LL146; _LL164: if( topt != 0){ void* _temp243= Cyc_Tcutil_compress(({
void** _temp242= topt; if( _temp242 == 0){ _throw( Null_Exception);}* _temp242;}));
struct Cyc_Absyn_PtrInfo _temp249; _LL245: if(( unsigned int) _temp243 > 4u?*((
int*) _temp243) == Cyc_Absyn_PointerType: 0){ _LL250: _temp249=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp243)->f1; goto _LL246;} else{ goto
_LL247;} _LL247: goto _LL248; _LL246: t=({ void** _temp251= topt; if( _temp251
== 0){ _throw( Null_Exception);}* _temp251;}); goto tcpat_end; _LL248: goto
_LL244; _LL244:;} t=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp252=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp252[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp253; _temp253.tag= Cyc_Absyn_PointerType;
_temp253.f1=({ struct Cyc_Absyn_PtrInfo _temp254; _temp254.elt_typ=( void*) Cyc_Absyn_new_evar((
void*) Cyc_Absyn_AnyKind); _temp254.rgn_typ=( void*) Cyc_Absyn_new_evar(( void*)
Cyc_Absyn_RgnKind); _temp254.nullable=(( struct Cyc_Absyn_Conref*(*)( int x))
Cyc_Absyn_new_conref)( 1); _temp254.tq= Cyc_Absyn_empty_tqual(); _temp254.bounds=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp254;}); _temp253;});
_temp252;}); goto _LL146; _LL166: { void* inner_typ=( void*) Cyc_Absyn_VoidType;
void** _temp255= 0; goto _LL256; _LL256: if( topt != 0){ void* _temp258= Cyc_Tcutil_compress(({
void** _temp257= topt; if( _temp257 == 0){ _throw( Null_Exception);}* _temp257;}));
struct Cyc_Absyn_PtrInfo _temp264; struct Cyc_Absyn_Conref* _temp266; struct Cyc_Absyn_Tqual
_temp268; struct Cyc_Absyn_Conref* _temp270; void* _temp272; void* _temp274;
_LL260: if(( unsigned int) _temp258 > 4u?*(( int*) _temp258) == Cyc_Absyn_PointerType:
0){ _LL265: _temp264=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp258)->f1; _LL275: _temp274=( void*) _temp264.elt_typ; goto _LL273; _LL273:
_temp272=( void*) _temp264.rgn_typ; goto _LL271; _LL271: _temp270=( struct Cyc_Absyn_Conref*)
_temp264.nullable; goto _LL269; _LL269: _temp268=( struct Cyc_Absyn_Tqual)
_temp264.tq; goto _LL267; _LL267: _temp266=( struct Cyc_Absyn_Conref*) _temp264.bounds;
goto _LL261;} else{ goto _LL262;} _LL262: goto _LL263; _LL261: inner_typ=
_temp274; _temp255=( void**)& inner_typ; goto _LL259; _LL263: goto _LL259;
_LL259:;}{ void* ptr_rgn= Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind); struct
Cyc_List_List* _temp278; struct Cyc_List_List* _temp280; struct _tuple5 _temp276=
Cyc_Tcpat_tcPatRec( te, _temp199, _temp255,( void**)& ptr_rgn); _LL281: _temp280=
_temp276.f1; goto _LL279; _LL279: _temp278= _temp276.f2; goto _LL277; _LL277:
tv_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( tv_result, _temp280); v_result=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( v_result,
_temp278); if( _temp255 == 0){ void* _temp283= Cyc_Tcutil_compress(( void*)({
struct Cyc_Core_Opt* _temp282= _temp199->topt; if( _temp282 == 0){ _throw(
Null_Exception);} _temp282->v;})); struct Cyc_Absyn_TunionFieldInfo _temp289;
struct Cyc_List_List* _temp291; void* _temp293; struct Cyc_Absyn_Tunionfield*
_temp295; struct Cyc_Absyn_Tuniondecl* _temp297; _LL285: if(( unsigned int)
_temp283 > 4u?*(( int*) _temp283) == Cyc_Absyn_TunionFieldType: 0){ _LL290:
_temp289=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp283)->f1; _LL294: _temp293=( void*) _temp289.field_info; if(*(( int*)
_temp293) == Cyc_Absyn_KnownTunionfield){ _LL298: _temp297=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp293)->f1; goto _LL296; _LL296:
_temp295=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp293)->f2; goto _LL292;} else{ goto _LL287;} _LL292: _temp291=( struct Cyc_List_List*)
_temp289.targs; goto _LL286;} else{ goto _LL287;} _LL287: goto _LL288; _LL286: t=(
void*)({ struct Cyc_Absyn_TunionType_struct* _temp299=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp299[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp300; _temp300.tag= Cyc_Absyn_TunionType;
_temp300.f1=({ struct Cyc_Absyn_TunionInfo _temp301; _temp301.tunion_info=( void*)((
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp302=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp302[ 0]=({ struct
Cyc_Absyn_KnownTunion_struct _temp303; _temp303.tag= Cyc_Absyn_KnownTunion;
_temp303.f1= _temp297; _temp303;}); _temp302;})); _temp301.targs= _temp291;
_temp301.rgn=( void*) ptr_rgn; _temp301;}); _temp300;}); _temp299;}); goto
_LL284; _LL288: t=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp304=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp304[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp305; _temp305.tag= Cyc_Absyn_PointerType;
_temp305.f1=({ struct Cyc_Absyn_PtrInfo _temp306; _temp306.elt_typ=( void*)((
void*)({ struct Cyc_Core_Opt* _temp307= _temp199->topt; if( _temp307 == 0){
_throw( Null_Exception);} _temp307->v;})); _temp306.rgn_typ=( void*) ptr_rgn;
_temp306.nullable=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp306.tq= Cyc_Absyn_empty_tqual(); _temp306.bounds=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp306;}); _temp305;}); _temp304;}); goto _LL284;
_LL284:;} else{ t=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp308=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp308[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp309; _temp309.tag= Cyc_Absyn_PointerType;
_temp309.f1=({ struct Cyc_Absyn_PtrInfo _temp310; _temp310.elt_typ=( void*)((
void*)({ struct Cyc_Core_Opt* _temp311= _temp199->topt; if( _temp311 == 0){
_throw( Null_Exception);} _temp311->v;})); _temp310.rgn_typ=( void*) ptr_rgn;
_temp310.nullable=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp310.tq= Cyc_Absyn_empty_tqual(); _temp310.bounds=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp310;}); _temp309;}); _temp308;});} goto _LL146;}}
_LL168: { struct Cyc_List_List* pat_ts= 0; struct Cyc_List_List* topt_ts= 0; if(
topt != 0){ void* _temp313= Cyc_Tcutil_compress(({ void** _temp312= topt; if(
_temp312 == 0){ _throw( Null_Exception);}* _temp312;})); struct Cyc_List_List*
_temp319; _LL315: if(( unsigned int) _temp313 > 4u?*(( int*) _temp313) == Cyc_Absyn_TupleType:
0){ _LL320: _temp319=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp313)->f1; goto _LL316;} else{ goto _LL317;} _LL317: goto _LL318; _LL316:
topt_ts= _temp319; goto _LL314; _LL318: goto _LL314; _LL314:;} for( 0; _temp201
!= 0; _temp201=({ struct Cyc_List_List* _temp321= _temp201; if( _temp321 == 0){
_throw( Null_Exception);} _temp321->tl;})){ void** _temp322= 0; goto _LL323;
_LL323: if( topt_ts != 0){ _temp322=( void**)&(*(( struct _tuple7*)({ struct Cyc_List_List*
_temp324= topt_ts; if( _temp324 == 0){ _throw( Null_Exception);} _temp324->hd;}))).f2;
topt_ts=({ struct Cyc_List_List* _temp325= topt_ts; if( _temp325 == 0){ _throw(
Null_Exception);} _temp325->tl;});}{ struct Cyc_List_List* _temp329; struct Cyc_List_List*
_temp331; struct _tuple5 _temp327= Cyc_Tcpat_tcPatRec( te,( struct Cyc_Absyn_Pat*)({
struct Cyc_List_List* _temp326= _temp201; if( _temp326 == 0){ _throw(
Null_Exception);} _temp326->hd;}), _temp322, region_opt); _LL332: _temp331=
_temp327.f1; goto _LL330; _LL330: _temp329= _temp327.f2; goto _LL328; _LL328:
tv_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( tv_result, _temp331); v_result=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( v_result,
_temp329); pat_ts=({ struct Cyc_List_List* _temp333=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp333->hd=( void*)({ struct
_tuple7* _temp334=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7));
_temp334->f1= Cyc_Absyn_empty_tqual(); _temp334->f2=( void*)({ struct Cyc_Core_Opt*
_temp336=(( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List* _temp335= _temp201;
if( _temp335 == 0){ _throw( Null_Exception);} _temp335->hd;}))->topt; if(
_temp336 == 0){ _throw( Null_Exception);} _temp336->v;}); _temp334;}); _temp333->tl=
pat_ts; _temp333;});}} t=( void*)({ struct Cyc_Absyn_TupleType_struct* _temp337=(
struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp337[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp338; _temp338.tag= Cyc_Absyn_TupleType;
_temp338.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
pat_ts); _temp338;}); _temp337;}); goto _LL146;} _LL170: if( _temp205 != 0){(
void) _throw((( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ char* _temp339=( char*)"tcPat: struct<...> not implemented";
struct _tagged_string _temp340; _temp340.curr= _temp339; _temp340.base= _temp339;
_temp340.last_plus_one= _temp339 + 35; _temp340;})));}{ struct _RegionHandle
_temp341= _new_region(); struct _RegionHandle* rgn=& _temp341; _push_region( rgn);{
struct Cyc_List_List* _temp342=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct _tuple4*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Tvar*), struct
_RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,
rgn, _temp210->tvs); goto _LL343; _LL343: { struct Cyc_List_List* _temp344=((
struct Cyc_List_List*(*)( void*(* f)( struct _tuple4*), struct Cyc_List_List* x))
Cyc_List_map)(( void*(*)( struct _tuple4*)) Cyc_Core_snd, _temp342); goto _LL345;
_LL345: t=( void*)({ struct Cyc_Absyn_StructType_struct* _temp346=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp346[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp347; _temp347.tag= Cyc_Absyn_StructType;
_temp347.f1=( struct _tuple1*)(( struct _tuple1*)({ struct Cyc_Core_Opt*
_temp348= _temp210->name; if( _temp348 == 0){ _throw( Null_Exception);} _temp348->v;}));
_temp347.f2= _temp344; _temp347.f3=({ struct Cyc_Absyn_Structdecl** _temp349=(
struct Cyc_Absyn_Structdecl**) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*));
_temp349[ 0]= _temp210; _temp349;}); _temp347;}); _temp346;}); if( topt != 0){
Cyc_Tcutil_unify( t,({ void** _temp350= topt; if( _temp350 == 0){ _throw(
Null_Exception);}* _temp350;}));}* _temp209=({ struct Cyc_Core_Opt* _temp351=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp351->v=(
void*) _temp344; _temp351;}); if( _temp210->fields == 0){ Cyc_Tcutil_terr( p->loc,(
struct _tagged_string)({ char* _temp352=( char*)"can't destructure an abstract struct";
struct _tagged_string _temp353; _temp353.curr= _temp352; _temp353.base= _temp352;
_temp353.last_plus_one= _temp352 + 37; _temp353;})); _npop_handler( 0u); goto
_LL146;}{ struct Cyc_List_List* fields=(( struct Cyc_List_List*(*)( struct
_RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List* des,
struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)( rgn, p->loc,
_temp203,( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp376= _temp210->fields;
if( _temp376 == 0){ _throw( Null_Exception);} _temp376->v;})); for( 0; fields !=
0; fields=({ struct Cyc_List_List* _temp354= fields; if( _temp354 == 0){ _throw(
Null_Exception);} _temp354->tl;})){ struct _tuple8 _temp358; struct Cyc_Absyn_Pat*
_temp359; struct Cyc_Absyn_Structfield* _temp361; struct _tuple8* _temp356=(
struct _tuple8*)({ struct Cyc_List_List* _temp355= fields; if( _temp355 == 0){
_throw( Null_Exception);} _temp355->hd;}); _temp358=* _temp356; _LL362: _temp361=
_temp358.f1; goto _LL360; _LL360: _temp359= _temp358.f2; goto _LL357; _LL357: {
void* _temp363= Cyc_Tcutil_rsubstitute( rgn, _temp342,( void*) _temp361->type);
goto _LL364; _LL364: { struct Cyc_List_List* _temp367; struct Cyc_List_List*
_temp369; struct _tuple5 _temp365= Cyc_Tcpat_tcPatRec( te, _temp359,( void**)&
_temp363, region_opt); _LL370: _temp369= _temp365.f1; goto _LL368; _LL368:
_temp367= _temp365.f2; goto _LL366; _LL366: tv_result=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result,
_temp369); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( v_result, _temp367); if( ! Cyc_Tcutil_unify((
void*)({ struct Cyc_Core_Opt* _temp371= _temp359->topt; if( _temp371 == 0){
_throw( Null_Exception);} _temp371->v;}), _temp363)){ Cyc_Tcutil_terr( p->loc,({
struct _tagged_string _temp373=* _temp361->name; struct _tagged_string _temp374=
Cyc_Absynpp_typ2string( _temp363); struct _tagged_string _temp375= Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Core_Opt* _temp372= _temp359->topt; if( _temp372 == 0){
_throw( Null_Exception);} _temp372->v;})); xprintf("field %.*s of struct pattern expects type %.*s != %.*s",
_temp373.last_plus_one - _temp373.curr, _temp373.curr, _temp374.last_plus_one -
_temp374.curr, _temp374.curr, _temp375.last_plus_one - _temp375.curr, _temp375.curr);}));}}}}}}};
_pop_region(& _temp341);} goto _LL146; _LL172: if((( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( _temp214) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp216->tvs)){ Cyc_Tcutil_terr( p->loc,( struct _tagged_string)({ char*
_temp377=( char*)"wrong number of existential type variables in pattern"; struct
_tagged_string _temp378; _temp378.curr= _temp377; _temp378.base= _temp377;
_temp378.last_plus_one= _temp377 + 54; _temp378;}));}{ struct _RegionHandle
_temp379= _new_region(); struct _RegionHandle* rgn=& _temp379; _push_region( rgn);{
struct Cyc_List_List* _temp380= 0; goto _LL381; _LL381:{ struct Cyc_List_List* t=
_temp214; for( 0; t != 0; t=({ struct Cyc_List_List* _temp382= t; if( _temp382
== 0){ _throw( Null_Exception);} _temp382->tl;})){ struct Cyc_Absyn_Tvar* tv=(
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp409= t; if( _temp409 == 0){
_throw( Null_Exception);} _temp409->hd;});{ void* _temp383=( void*)((( struct
Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
tv->kind))->v; void* _temp393; void* _temp395; struct Cyc_Absyn_Conref* _temp397;
_LL385: if(( unsigned int) _temp383 > 1u?*(( int*) _temp383) == Cyc_Absyn_Eq_constr:
0){ _LL394: _temp393=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp383)->f1;
if( _temp393 ==( void*) Cyc_Absyn_BoxKind){ goto _LL386;} else{ goto _LL387;}}
else{ goto _LL387;} _LL387: if(( unsigned int) _temp383 > 1u?*(( int*) _temp383)
== Cyc_Absyn_Eq_constr: 0){ _LL396: _temp395=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp383)->f1; goto _LL388;} else{ goto _LL389;} _LL389: if( _temp383 ==( void*)
Cyc_Absyn_No_constr){ goto _LL390;} else{ goto _LL391;} _LL391: if((
unsigned int) _temp383 > 1u?*(( int*) _temp383) == Cyc_Absyn_Forward_constr: 0){
_LL398: _temp397=( struct Cyc_Absyn_Conref*)(( struct Cyc_Absyn_Forward_constr_struct*)
_temp383)->f1; goto _LL392;} else{ goto _LL384;} _LL386: goto _LL384; _LL388:
Cyc_Tcutil_terr( p->loc,({ struct _tagged_string _temp399= Cyc_Absynpp_qvar2string(
_temp218->name); struct _tagged_string _temp400=* tv->name; struct
_tagged_string _temp401= Cyc_Absynpp_kind2string( _temp395); xprintf("tunion member %.*s abstracts type variable %.*s of kind %.*s != B",
_temp399.last_plus_one - _temp399.curr, _temp399.curr, _temp400.last_plus_one -
_temp400.curr, _temp400.curr, _temp401.last_plus_one - _temp401.curr, _temp401.curr);}));
goto _LL384; _LL390:( void*)(( tv->kind)->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp402=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp402[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp403; _temp403.tag= Cyc_Absyn_Eq_constr;
_temp403.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp403;}); _temp402;})));
goto _LL384; _LL392:( void) _throw((( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp404=( char*)"forward after compress in conref";
struct _tagged_string _temp405; _temp405.curr= _temp404; _temp405.base= _temp404;
_temp405.last_plus_one= _temp404 + 33; _temp405;}))); _LL384:;} _temp380=({
struct Cyc_List_List* _temp406=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp406->hd=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct*
_temp407=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp407[ 0]=({ struct Cyc_Absyn_VarType_struct _temp408; _temp408.tag= Cyc_Absyn_VarType;
_temp408.f1= tv; _temp408;}); _temp407;})); _temp406->tl= _temp380; _temp406;});}}
_temp380=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp380); tv_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( tv_result, _temp214);{ struct Cyc_Tcenv_Tenv*
te2= Cyc_Tcenv_add_type_vars( p->loc, te, _temp214); struct Cyc_List_List* tqts=
_temp216->typs; struct Cyc_List_List* _temp410=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple4*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Tvar*),
struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,
rgn, _temp218->tvs); goto _LL411; _LL411: { struct Cyc_List_List* _temp412=((
struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct _RegionHandle* r4,
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn,
_temp216->tvs, _temp380); goto _LL413; _LL413: { struct Cyc_List_List* _temp414=((
struct Cyc_List_List*(*)( void*(* f)( struct _tuple4*), struct Cyc_List_List* x))
Cyc_List_map)(( void*(*)( struct _tuple4*)) Cyc_Core_snd, _temp410); goto _LL415;
_LL415: { struct Cyc_List_List* _temp416=(( struct Cyc_List_List*(*)( void*(* f)(
struct _tuple4*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct
_tuple4*)) Cyc_Core_snd, _temp412); goto _LL417; _LL417: { struct Cyc_List_List*
_temp418=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_rappend)( rgn, _temp410, _temp412); goto
_LL419; _LL419: if( tqts == 0){ t=( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp420=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp420[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp421; _temp421.tag= Cyc_Absyn_TunionType;
_temp421.f1=({ struct Cyc_Absyn_TunionInfo _temp422; _temp422.tunion_info=( void*)((
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp423=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp423[ 0]=({ struct
Cyc_Absyn_KnownTunion_struct _temp424; _temp424.tag= Cyc_Absyn_KnownTunion;
_temp424.f1= _temp218; _temp424;}); _temp423;})); _temp422.targs= _temp414;
_temp422.rgn=( void*) Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind); _temp422;});
_temp421;}); _temp420;});} else{ t=( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp425=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp425[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp426; _temp426.tag= Cyc_Absyn_TunionFieldType; _temp426.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp427; _temp427.field_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp428=( struct Cyc_Absyn_KnownTunionfield_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp428[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp429; _temp429.tag= Cyc_Absyn_KnownTunionfield; _temp429.f1= _temp218;
_temp429.f2= _temp216; _temp429;}); _temp428;})); _temp427.targs= _temp414;
_temp427;}); _temp426;}); _temp425;});} if( topt != 0? tqts == 0: 0){ void*
_temp431= Cyc_Tcutil_compress(({ void** _temp430= topt; if( _temp430 == 0){
_throw( Null_Exception);}* _temp430;})); struct Cyc_Absyn_TunionFieldInfo
_temp439; struct Cyc_Absyn_TunionInfo _temp441; void* _temp443; struct Cyc_List_List*
_temp445; void* _temp447; _LL433: if(( unsigned int) _temp431 > 4u?*(( int*)
_temp431) == Cyc_Absyn_TunionFieldType: 0){ _LL440: _temp439=( struct Cyc_Absyn_TunionFieldInfo)((
struct Cyc_Absyn_TunionFieldType_struct*) _temp431)->f1; goto _LL434;} else{
goto _LL435;} _LL435: if(( unsigned int) _temp431 > 4u?*(( int*) _temp431) ==
Cyc_Absyn_TunionType: 0){ _LL442: _temp441=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp431)->f1; _LL448: _temp447=( void*)
_temp441.tunion_info; goto _LL446; _LL446: _temp445=( struct Cyc_List_List*)
_temp441.targs; goto _LL444; _LL444: _temp443=( void*) _temp441.rgn; goto _LL436;}
else{ goto _LL437;} _LL437: goto _LL438; _LL434: t=( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp449=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp449[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp450; _temp450.tag= Cyc_Absyn_TunionFieldType; _temp450.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp451; _temp451.field_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp452=( struct Cyc_Absyn_KnownTunionfield_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp452[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp453; _temp453.tag= Cyc_Absyn_KnownTunionfield; _temp453.f1= _temp218;
_temp453.f2= _temp216; _temp453;}); _temp452;})); _temp451.targs= _temp414;
_temp451;}); _temp450;}); _temp449;}); goto _LL432; _LL436: { struct Cyc_List_List*
_temp454= _temp414; goto _LL455; _LL455: for( 0; _temp454 != 0? _temp445 != 0: 0;
_temp454=({ struct Cyc_List_List* _temp456= _temp454; if( _temp456 == 0){ _throw(
Null_Exception);} _temp456->tl;}), _temp445=({ struct Cyc_List_List* _temp457=
_temp445; if( _temp457 == 0){ _throw( Null_Exception);} _temp457->tl;})){ Cyc_Tcutil_unify((
void*)({ struct Cyc_List_List* _temp458= _temp454; if( _temp458 == 0){ _throw(
Null_Exception);} _temp458->hd;}),( void*)({ struct Cyc_List_List* _temp459=
_temp445; if( _temp459 == 0){ _throw( Null_Exception);} _temp459->hd;}));} goto
_LL432;} _LL438: goto _LL432; _LL432:;} for( 0; _temp212 != 0? tqts != 0: 0;
_temp212=({ struct Cyc_List_List* _temp460= _temp212; if( _temp460 == 0){ _throw(
Null_Exception);} _temp460->tl;}), tqts=({ struct Cyc_List_List* _temp461= tqts;
if( _temp461 == 0){ _throw( Null_Exception);} _temp461->tl;})){ struct Cyc_Absyn_Pat*
_temp463=( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List* _temp462= _temp212;
if( _temp462 == 0){ _throw( Null_Exception);} _temp462->hd;}); goto _LL464;
_LL464: { void* _temp466= Cyc_Tcutil_rsubstitute( rgn, _temp418,(*(( struct
_tuple7*)({ struct Cyc_List_List* _temp465= tqts; if( _temp465 == 0){ _throw(
Null_Exception);} _temp465->hd;}))).f2); goto _LL467; _LL467: { struct Cyc_List_List*
_temp470; struct Cyc_List_List* _temp472; struct _tuple5 _temp468= Cyc_Tcpat_tcPatRec(
te2, _temp463,( void**)& _temp466, region_opt); _LL473: _temp472= _temp468.f1;
goto _LL471; _LL471: _temp470= _temp468.f2; goto _LL469; _LL469: tv_result=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
tv_result, _temp472); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( v_result, _temp470); if( ! Cyc_Tcutil_unify((
void*)({ struct Cyc_Core_Opt* _temp474= _temp463->topt; if( _temp474 == 0){
_throw( Null_Exception);} _temp474->v;}), _temp466)){ Cyc_Tcutil_terr( _temp463->loc,({
struct _tagged_string _temp476= Cyc_Absynpp_qvar2string( _temp218->name); struct
_tagged_string _temp477= Cyc_Absynpp_typ2string( _temp466); struct
_tagged_string _temp478= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp475= _temp463->topt; if( _temp475 == 0){ _throw( Null_Exception);} _temp475->v;}));
xprintf("%.*s expects argument type %.*s, not %.*s", _temp476.last_plus_one -
_temp476.curr, _temp476.curr, _temp477.last_plus_one - _temp477.curr, _temp477.curr,
_temp478.last_plus_one - _temp478.curr, _temp478.curr);}));}}}} if( _temp212 !=
0){ Cyc_Tcutil_terr( p->loc,({ struct _tagged_string _temp479= Cyc_Absynpp_qvar2string(
_temp218->name); xprintf("too many arguments for tunion constructor %.*s",
_temp479.last_plus_one - _temp479.curr, _temp479.curr);}));} if( tqts != 0){ Cyc_Tcutil_terr(
p->loc,({ struct _tagged_string _temp480= Cyc_Absynpp_qvar2string( _temp218->name);
xprintf("too few arguments for tunion constructor %.*s", _temp480.last_plus_one
- _temp480.curr, _temp480.curr);}));}}}}}}}; _pop_region(& _temp379);} goto
_LL146; _LL174: goto _LL176; _LL176: goto _LL178; _LL178: t= Cyc_Absyn_wildtyp();
goto _LL146; _LL146:;} tcpat_end: p->topt=({ struct Cyc_Core_Opt* _temp481=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp481->v=(
void*) t; _temp481;}); return({ struct _tuple5 _temp482; _temp482.f1= tv_result;
_temp482.f2= v_result; _temp482;});}} struct _tuple5 Cyc_Tcpat_tcPat( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Pat* p, void** topt, void** region_opt){ struct _tuple5
_temp483= Cyc_Tcpat_tcPatRec( te, p, topt, region_opt); goto _LL484; _LL484:{
struct _RegionHandle _temp485= _new_region(); struct _RegionHandle* r=& _temp485;
_push_region( r); Cyc_Tcutil_check_unique_vars((( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tagged_string*(* f)( struct Cyc_Absyn_Vardecl*),
struct Cyc_List_List* x)) Cyc_List_rmap)( r, Cyc_Tcpat_get_name, _temp483.f2), p->loc,(
struct _tagged_string)({ char* _temp486=( char*)"pattern contains a repeated variable";
struct _tagged_string _temp487; _temp487.curr= _temp486; _temp487.base= _temp486;
_temp487.last_plus_one= _temp486 + 37; _temp487;}));; _pop_region(& _temp485);}
return _temp483;} void Cyc_Tcpat_check_pat_regions( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Pat* p){ void* _temp488=( void*) p->r; struct Cyc_Absyn_Pat*
_temp500; struct Cyc_List_List* _temp502; struct Cyc_List_List* _temp504; struct
Cyc_Absyn_Tunionfield* _temp506; struct Cyc_Absyn_Tuniondecl* _temp508; struct
Cyc_List_List* _temp510; struct Cyc_List_List* _temp512; struct Cyc_Core_Opt*
_temp514; struct Cyc_Absyn_Structdecl* _temp516; struct Cyc_List_List* _temp518;
_LL490: if(( unsigned int) _temp488 > 2u?*(( int*) _temp488) == Cyc_Absyn_Pointer_p:
0){ _LL501: _temp500=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*)
_temp488)->f1; goto _LL491;} else{ goto _LL492;} _LL492: if(( unsigned int)
_temp488 > 2u?*(( int*) _temp488) == Cyc_Absyn_Tunion_p: 0){ _LL509: _temp508=(
struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp488)->f1;
goto _LL507; _LL507: _temp506=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp488)->f2; goto _LL505; _LL505: _temp504=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Tunion_p_struct*) _temp488)->f3; goto _LL503; _LL503: _temp502=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp488)->f4; goto
_LL493;} else{ goto _LL494;} _LL494: if(( unsigned int) _temp488 > 2u?*(( int*)
_temp488) == Cyc_Absyn_Struct_p: 0){ _LL517: _temp516=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_p_struct*) _temp488)->f1; goto _LL515; _LL515: _temp514=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_p_struct*) _temp488)->f2; goto
_LL513; _LL513: _temp512=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp488)->f3; goto _LL511; _LL511: _temp510=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Struct_p_struct*) _temp488)->f4; goto _LL495;} else{ goto _LL496;}
_LL496: if(( unsigned int) _temp488 > 2u?*(( int*) _temp488) == Cyc_Absyn_Tuple_p:
0){ _LL519: _temp518=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*)
_temp488)->f1; goto _LL497;} else{ goto _LL498;} _LL498: goto _LL499; _LL491:
Cyc_Tcpat_check_pat_regions( te, _temp500);{ void* _temp521=( void*)({ struct
Cyc_Core_Opt* _temp520= p->topt; if( _temp520 == 0){ _throw( Null_Exception);}
_temp520->v;}); struct Cyc_Absyn_PtrInfo _temp529; struct Cyc_Absyn_Conref*
_temp531; struct Cyc_Absyn_Tqual _temp533; struct Cyc_Absyn_Conref* _temp535;
void* _temp537; void* _temp539; struct Cyc_Absyn_TunionInfo _temp541; void*
_temp543; struct Cyc_List_List* _temp545; void* _temp547; _LL523: if((
unsigned int) _temp521 > 4u?*(( int*) _temp521) == Cyc_Absyn_PointerType: 0){
_LL530: _temp529=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp521)->f1; _LL540: _temp539=( void*) _temp529.elt_typ; goto _LL538; _LL538:
_temp537=( void*) _temp529.rgn_typ; goto _LL536; _LL536: _temp535=( struct Cyc_Absyn_Conref*)
_temp529.nullable; goto _LL534; _LL534: _temp533=( struct Cyc_Absyn_Tqual)
_temp529.tq; goto _LL532; _LL532: _temp531=( struct Cyc_Absyn_Conref*) _temp529.bounds;
goto _LL524;} else{ goto _LL525;} _LL525: if(( unsigned int) _temp521 > 4u?*((
int*) _temp521) == Cyc_Absyn_TunionType: 0){ _LL542: _temp541=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp521)->f1; _LL548: _temp547=( void*)
_temp541.tunion_info; goto _LL546; _LL546: _temp545=( struct Cyc_List_List*)
_temp541.targs; goto _LL544; _LL544: _temp543=( void*) _temp541.rgn; goto _LL526;}
else{ goto _LL527;} _LL527: goto _LL528; _LL524: _temp543= _temp537; goto _LL526;
_LL526: Cyc_Tcenv_check_rgn_accessible( te, p->loc, _temp543); return; _LL528:(
void) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp549=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp549[ 0]=({ struct
Cyc_Core_Impossible_struct _temp550; _temp550.tag= Cyc_Core_Impossible; _temp550.f1=(
struct _tagged_string)({ char* _temp551=( char*)"check_pat_regions: bad pointer type";
struct _tagged_string _temp552; _temp552.curr= _temp551; _temp552.base= _temp551;
_temp552.last_plus_one= _temp551 + 36; _temp552;}); _temp550;}); _temp549;}));
_LL522:;} _LL493: for( 0; _temp502 != 0; _temp502=({ struct Cyc_List_List*
_temp553= _temp502; if( _temp553 == 0){ _throw( Null_Exception);} _temp553->tl;})){
Cyc_Tcpat_check_pat_regions( te,( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List*
_temp554= _temp502; if( _temp554 == 0){ _throw( Null_Exception);} _temp554->hd;}));}{
void* _temp556=( void*)({ struct Cyc_Core_Opt* _temp555= p->topt; if( _temp555
== 0){ _throw( Null_Exception);} _temp555->v;}); struct Cyc_Absyn_TunionInfo
_temp564; void* _temp566; struct Cyc_List_List* _temp568; void* _temp570; struct
Cyc_Absyn_TunionFieldInfo _temp572; _LL558: if(( unsigned int) _temp556 > 4u?*((
int*) _temp556) == Cyc_Absyn_TunionType: 0){ _LL565: _temp564=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp556)->f1; _LL571: _temp570=( void*)
_temp564.tunion_info; goto _LL569; _LL569: _temp568=( struct Cyc_List_List*)
_temp564.targs; goto _LL567; _LL567: _temp566=( void*) _temp564.rgn; goto _LL559;}
else{ goto _LL560;} _LL560: if(( unsigned int) _temp556 > 4u?*(( int*) _temp556)
== Cyc_Absyn_TunionFieldType: 0){ _LL573: _temp572=( struct Cyc_Absyn_TunionFieldInfo)((
struct Cyc_Absyn_TunionFieldType_struct*) _temp556)->f1; goto _LL561;} else{
goto _LL562;} _LL562: goto _LL563; _LL559: Cyc_Tcenv_check_rgn_accessible( te, p->loc,
_temp566); return; _LL561: return; _LL563:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp574=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp574[ 0]=({ struct Cyc_Core_Impossible_struct _temp575; _temp575.tag= Cyc_Core_Impossible;
_temp575.f1=( struct _tagged_string)({ char* _temp576=( char*)"check_pat_regions: bad tunion type";
struct _tagged_string _temp577; _temp577.curr= _temp576; _temp577.base= _temp576;
_temp577.last_plus_one= _temp576 + 35; _temp577;}); _temp575;}); _temp574;}));
_LL557:;} _LL495: _temp518=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Pat*(*
f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Absyn_Pat*(*)(
struct _tuple6*)) Cyc_Core_snd, _temp510); goto _LL497; _LL497: for( 0; _temp518
!= 0; _temp518=({ struct Cyc_List_List* _temp578= _temp518; if( _temp578 == 0){
_throw( Null_Exception);} _temp578->tl;})){ Cyc_Tcpat_check_pat_regions( te,(
struct Cyc_Absyn_Pat*)({ struct Cyc_List_List* _temp579= _temp518; if( _temp579
== 0){ _throw( Null_Exception);} _temp579->hd;}));} return; _LL499: return;
_LL489:;} static const int Cyc_Tcpat_Name_v= 0; struct Cyc_Tcpat_Name_v_struct{
int tag; struct _tagged_string f1; } ; static const int Cyc_Tcpat_Int_v= 1;
struct Cyc_Tcpat_Int_v_struct{ int tag; int f1; } ; typedef void* Cyc_Tcpat_name_value_t;
struct Cyc_Tcpat_Con_s{ void* name; int arity; struct Cyc_Core_Opt* span; } ;
typedef struct Cyc_Tcpat_Con_s* Cyc_Tcpat_con_t; static const int Cyc_Tcpat_Any=
0; static const int Cyc_Tcpat_Con= 0; struct Cyc_Tcpat_Con_struct{ int tag;
struct Cyc_Tcpat_Con_s* f1; struct Cyc_List_List* f2; } ; typedef void* Cyc_Tcpat_simple_pat_t;
static int Cyc_Tcpat_compare_con( struct Cyc_Tcpat_Con_s* c1, struct Cyc_Tcpat_Con_s*
c2){ void* _temp580=( void*) c1->name; struct _tagged_string _temp586; int
_temp588; _LL582: if(*(( int*) _temp580) == Cyc_Tcpat_Name_v){ _LL587: _temp586=(
struct _tagged_string)(( struct Cyc_Tcpat_Name_v_struct*) _temp580)->f1; goto
_LL583;} else{ goto _LL584;} _LL584: if(*(( int*) _temp580) == Cyc_Tcpat_Int_v){
_LL589: _temp588=( int)(( struct Cyc_Tcpat_Int_v_struct*) _temp580)->f1; goto
_LL585;} else{ goto _LL581;} _LL583: { void* _temp590=( void*) c2->name; struct
_tagged_string _temp596; int _temp598; _LL592: if(*(( int*) _temp590) == Cyc_Tcpat_Name_v){
_LL597: _temp596=( struct _tagged_string)(( struct Cyc_Tcpat_Name_v_struct*)
_temp590)->f1; goto _LL593;} else{ goto _LL594;} _LL594: if(*(( int*) _temp590)
== Cyc_Tcpat_Int_v){ _LL599: _temp598=( int)(( struct Cyc_Tcpat_Int_v_struct*)
_temp590)->f1; goto _LL595;} else{ goto _LL591;} _LL593: return Cyc_String_zstrcmp(
_temp586, _temp596); _LL595: return - 1; _LL591:;} _LL585: { void* _temp600=(
void*) c2->name; struct _tagged_string _temp606; int _temp608; _LL602: if(*((
int*) _temp600) == Cyc_Tcpat_Name_v){ _LL607: _temp606=( struct _tagged_string)((
struct Cyc_Tcpat_Name_v_struct*) _temp600)->f1; goto _LL603;} else{ goto _LL604;}
_LL604: if(*(( int*) _temp600) == Cyc_Tcpat_Int_v){ _LL609: _temp608=( int)((
struct Cyc_Tcpat_Int_v_struct*) _temp600)->f1; goto _LL605;} else{ goto _LL601;}
_LL603: return 1; _LL605: return _temp588 - _temp608; _LL601:;} _LL581:;} static
struct Cyc_Set_Set* Cyc_Tcpat_empty_con_set(){ return(( struct Cyc_Set_Set*(*)(
int(* comp)( struct Cyc_Tcpat_Con_s*, struct Cyc_Tcpat_Con_s*))) Cyc_Set_empty)(
Cyc_Tcpat_compare_con);} static struct Cyc_Core_Opt Cyc_Tcpat_one_opt=( struct
Cyc_Core_Opt){.v=( void*) 1}; static struct Cyc_Core_Opt Cyc_Tcpat_two_opt=(
struct Cyc_Core_Opt){.v=( void*) 2}; static struct Cyc_Core_Opt* Cyc_Tcpat_one_opt_ptr=(
struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt; static char _temp613[ 5u]="null";
static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_null_name_value={ 0u,( struct
_tagged_string){ _temp613, _temp613, _temp613 + 5u}}; static char _temp617[ 4u]="ptr";
static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_ptr_name_value={ 0u,( struct
_tagged_string){ _temp617, _temp617, _temp617 + 4u}}; static char _temp621[ 6u]="tuple";
static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_tuple_name_value={ 0u,( struct
_tagged_string){ _temp621, _temp621, _temp621 + 6u}}; static struct Cyc_Tcpat_Con_s
Cyc_Tcpat_null_con0=( struct Cyc_Tcpat_Con_s){.name=( void*)(( void*)& Cyc_Tcpat_null_name_value),.arity=
0,.span=( struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt}; static struct Cyc_Tcpat_Con_s
Cyc_Tcpat_null_ptr_con0=( struct Cyc_Tcpat_Con_s){.name=( void*)(( void*)& Cyc_Tcpat_ptr_name_value),.arity=
1,.span=( struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt}; static struct Cyc_Tcpat_Con_s
Cyc_Tcpat_ptr_con0=( struct Cyc_Tcpat_Con_s){.name=( void*)(( void*)& Cyc_Tcpat_ptr_name_value),.arity=
1,.span=( struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt}; static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_null_con=& Cyc_Tcpat_null_con0; static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_null_ptr_con=&
Cyc_Tcpat_null_ptr_con0; static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_ptr_con=& Cyc_Tcpat_ptr_con0;
static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_int_con( int i){ return({ struct Cyc_Tcpat_Con_s*
_temp622=( struct Cyc_Tcpat_Con_s*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_s));
_temp622->name=( void*)(( void*)({ struct Cyc_Tcpat_Int_v_struct* _temp623=(
struct Cyc_Tcpat_Int_v_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Int_v_struct));
_temp623[ 0]=({ struct Cyc_Tcpat_Int_v_struct _temp624; _temp624.tag= Cyc_Tcpat_Int_v;
_temp624.f1= i; _temp624;}); _temp623;})); _temp622->arity= 0; _temp622->span= 0;
_temp622;});} static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_float_con( struct
_tagged_string f){ return({ struct Cyc_Tcpat_Con_s* _temp625=( struct Cyc_Tcpat_Con_s*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp625->name=( void*)(( void*)({
struct Cyc_Tcpat_Name_v_struct* _temp626=( struct Cyc_Tcpat_Name_v_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Name_v_struct)); _temp626[ 0]=({ struct Cyc_Tcpat_Name_v_struct
_temp627; _temp627.tag= Cyc_Tcpat_Name_v; _temp627.f1= f; _temp627;}); _temp626;}));
_temp625->arity= 0; _temp625->span= 0; _temp625;});} static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_char_con( char c){ return({ struct Cyc_Tcpat_Con_s* _temp628=( struct
Cyc_Tcpat_Con_s*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp628->name=(
void*)(( void*)({ struct Cyc_Tcpat_Int_v_struct* _temp630=( struct Cyc_Tcpat_Int_v_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Int_v_struct)); _temp630[ 0]=({ struct Cyc_Tcpat_Int_v_struct
_temp631; _temp631.tag= Cyc_Tcpat_Int_v; _temp631.f1=( int) c; _temp631;});
_temp630;})); _temp628->arity= 0; _temp628->span=({ struct Cyc_Core_Opt*
_temp629=( struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof( struct Cyc_Core_Opt));
_temp629->v=( void*) 256; _temp629;}); _temp628;});} static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_tuple_con( int i){ return({ struct Cyc_Tcpat_Con_s* _temp632=( struct
Cyc_Tcpat_Con_s*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp632->name=(
void*)(( void*)& Cyc_Tcpat_tuple_name_value); _temp632->arity= i; _temp632->span=
Cyc_Tcpat_one_opt_ptr; _temp632;});} static void* Cyc_Tcpat_null_pat(){ return(
void*)({ struct Cyc_Tcpat_Con_struct* _temp633=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp633[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp634; _temp634.tag= Cyc_Tcpat_Con; _temp634.f1= Cyc_Tcpat_null_con; _temp634.f2=
0; _temp634;}); _temp633;});} static void* Cyc_Tcpat_int_pat( int i){ return(
void*)({ struct Cyc_Tcpat_Con_struct* _temp635=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp635[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp636; _temp636.tag= Cyc_Tcpat_Con; _temp636.f1= Cyc_Tcpat_int_con( i);
_temp636.f2= 0; _temp636;}); _temp635;});} static void* Cyc_Tcpat_char_pat( char
c){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp637=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp637[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp638; _temp638.tag= Cyc_Tcpat_Con; _temp638.f1= Cyc_Tcpat_char_con( c);
_temp638.f2= 0; _temp638;}); _temp637;});} static void* Cyc_Tcpat_float_pat(
struct _tagged_string f){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp639=(
struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp639[ 0]=({ struct Cyc_Tcpat_Con_struct _temp640; _temp640.tag= Cyc_Tcpat_Con;
_temp640.f1= Cyc_Tcpat_float_con( f); _temp640.f2= 0; _temp640;}); _temp639;});}
static void* Cyc_Tcpat_null_ptr_pat( void* p){ return( void*)({ struct Cyc_Tcpat_Con_struct*
_temp641=( struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp641[ 0]=({ struct Cyc_Tcpat_Con_struct _temp642; _temp642.tag= Cyc_Tcpat_Con;
_temp642.f1= Cyc_Tcpat_null_ptr_con; _temp642.f2=({ struct Cyc_List_List*
_temp643=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp643->hd=( void*) p; _temp643->tl= 0; _temp643;}); _temp642;}); _temp641;});}
static void* Cyc_Tcpat_ptr_pat( void* p){ return( void*)({ struct Cyc_Tcpat_Con_struct*
_temp644=( struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp644[ 0]=({ struct Cyc_Tcpat_Con_struct _temp645; _temp645.tag= Cyc_Tcpat_Con;
_temp645.f1= Cyc_Tcpat_ptr_con; _temp645.f2=({ struct Cyc_List_List* _temp646=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp646->hd=(
void*) p; _temp646->tl= 0; _temp646;}); _temp645;}); _temp644;});} static void*
Cyc_Tcpat_tuple_pat( struct Cyc_List_List* ss){ return( void*)({ struct Cyc_Tcpat_Con_struct*
_temp647=( struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp647[ 0]=({ struct Cyc_Tcpat_Con_struct _temp648; _temp648.tag= Cyc_Tcpat_Con;
_temp648.f1= Cyc_Tcpat_tuple_con((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
ss)); _temp648.f2= ss; _temp648;}); _temp647;});} static void* Cyc_Tcpat_con_pat(
struct _tagged_string con_name, struct Cyc_Core_Opt* span, struct Cyc_List_List*
ps){ struct Cyc_Tcpat_Con_s* c=({ struct Cyc_Tcpat_Con_s* _temp651=( struct Cyc_Tcpat_Con_s*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp651->name=( void*)(( void*)({
struct Cyc_Tcpat_Name_v_struct* _temp652=( struct Cyc_Tcpat_Name_v_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Name_v_struct)); _temp652[ 0]=({ struct Cyc_Tcpat_Name_v_struct
_temp653; _temp653.tag= Cyc_Tcpat_Name_v; _temp653.f1= con_name; _temp653;});
_temp652;})); _temp651->arity=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
ps); _temp651->span= span; _temp651;}); return( void*)({ struct Cyc_Tcpat_Con_struct*
_temp649=( struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp649[ 0]=({ struct Cyc_Tcpat_Con_struct _temp650; _temp650.tag= Cyc_Tcpat_Con;
_temp650.f1= c; _temp650.f2= ps; _temp650;}); _temp649;});} static void* Cyc_Tcpat_compile_pat(
struct Cyc_Absyn_Pat* p){ void* s;{ void* _temp654=( void*) p->r; struct Cyc_Absyn_Vardecl*
_temp688; int _temp690; void* _temp692; char _temp694; struct _tagged_string
_temp696; struct Cyc_Absyn_Vardecl* _temp698; struct Cyc_Absyn_Pat* _temp700;
struct Cyc_List_List* _temp702; struct Cyc_List_List* _temp704; struct Cyc_Absyn_Tunionfield*
_temp706; struct Cyc_Absyn_Tuniondecl* _temp708; struct Cyc_List_List* _temp710;
struct Cyc_List_List* _temp712; struct Cyc_Absyn_Tunionfield* _temp714; struct
Cyc_Absyn_Tuniondecl* _temp716; struct Cyc_List_List* _temp718; struct Cyc_List_List*
_temp720; struct Cyc_List_List* _temp722; struct Cyc_Core_Opt* _temp724; struct
Cyc_Absyn_Structdecl* _temp726; struct Cyc_Absyn_Enumfield* _temp728; struct Cyc_Absyn_Enumdecl*
_temp730; struct _tuple1* _temp732; struct Cyc_List_List* _temp734; struct Cyc_List_List*
_temp736; struct _tuple1* _temp738; struct Cyc_List_List* _temp740; struct Cyc_List_List*
_temp742; struct _tuple1* _temp744; _LL656: if( _temp654 ==( void*) Cyc_Absyn_Wild_p){
goto _LL657;} else{ goto _LL658;} _LL658: if(( unsigned int) _temp654 > 2u?*((
int*) _temp654) == Cyc_Absyn_Var_p: 0){ _LL689: _temp688=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_p_struct*) _temp654)->f1; goto _LL659;} else{ goto _LL660;}
_LL660: if( _temp654 ==( void*) Cyc_Absyn_Null_p){ goto _LL661;} else{ goto
_LL662;} _LL662: if(( unsigned int) _temp654 > 2u?*(( int*) _temp654) == Cyc_Absyn_Int_p:
0){ _LL693: _temp692=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp654)->f1;
goto _LL691; _LL691: _temp690=( int)(( struct Cyc_Absyn_Int_p_struct*) _temp654)->f2;
goto _LL663;} else{ goto _LL664;} _LL664: if(( unsigned int) _temp654 > 2u?*((
int*) _temp654) == Cyc_Absyn_Char_p: 0){ _LL695: _temp694=( char)(( struct Cyc_Absyn_Char_p_struct*)
_temp654)->f1; goto _LL665;} else{ goto _LL666;} _LL666: if(( unsigned int)
_temp654 > 2u?*(( int*) _temp654) == Cyc_Absyn_Float_p: 0){ _LL697: _temp696=(
struct _tagged_string)(( struct Cyc_Absyn_Float_p_struct*) _temp654)->f1; goto
_LL667;} else{ goto _LL668;} _LL668: if(( unsigned int) _temp654 > 2u?*(( int*)
_temp654) == Cyc_Absyn_Reference_p: 0){ _LL699: _temp698=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Reference_p_struct*) _temp654)->f1; goto _LL669;} else{ goto
_LL670;} _LL670: if(( unsigned int) _temp654 > 2u?*(( int*) _temp654) == Cyc_Absyn_Pointer_p:
0){ _LL701: _temp700=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*)
_temp654)->f1; goto _LL671;} else{ goto _LL672;} _LL672: if(( unsigned int)
_temp654 > 2u?*(( int*) _temp654) == Cyc_Absyn_Tunion_p: 0){ _LL709: _temp708=(
struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp654)->f1;
goto _LL707; _LL707: _temp706=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp654)->f2; goto _LL705; _LL705: _temp704=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Tunion_p_struct*) _temp654)->f3; goto _LL703; _LL703: _temp702=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp654)->f4; if(
_temp702 == 0){ goto _LL673;} else{ goto _LL674;}} else{ goto _LL674;} _LL674:
if(( unsigned int) _temp654 > 2u?*(( int*) _temp654) == Cyc_Absyn_Tunion_p: 0){
_LL717: _temp716=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp654)->f1; goto _LL715; _LL715: _temp714=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp654)->f2; goto _LL713; _LL713: _temp712=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp654)->f3; goto
_LL711; _LL711: _temp710=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp654)->f4; goto _LL675;} else{ goto _LL676;} _LL676: if(( unsigned int)
_temp654 > 2u?*(( int*) _temp654) == Cyc_Absyn_Tuple_p: 0){ _LL719: _temp718=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*) _temp654)->f1; goto
_LL677;} else{ goto _LL678;} _LL678: if(( unsigned int) _temp654 > 2u?*(( int*)
_temp654) == Cyc_Absyn_Struct_p: 0){ _LL727: _temp726=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_p_struct*) _temp654)->f1; goto _LL725; _LL725: _temp724=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_p_struct*) _temp654)->f2; goto
_LL723; _LL723: _temp722=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp654)->f3; goto _LL721; _LL721: _temp720=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Struct_p_struct*) _temp654)->f4; goto _LL679;} else{ goto _LL680;}
_LL680: if(( unsigned int) _temp654 > 2u?*(( int*) _temp654) == Cyc_Absyn_Enum_p:
0){ _LL731: _temp730=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp654)->f1; goto _LL729; _LL729: _temp728=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_p_struct*) _temp654)->f2; goto _LL681;} else{ goto _LL682;}
_LL682: if(( unsigned int) _temp654 > 2u?*(( int*) _temp654) == Cyc_Absyn_UnknownId_p:
0){ _LL733: _temp732=( struct _tuple1*)(( struct Cyc_Absyn_UnknownId_p_struct*)
_temp654)->f1; goto _LL683;} else{ goto _LL684;} _LL684: if(( unsigned int)
_temp654 > 2u?*(( int*) _temp654) == Cyc_Absyn_UnknownCall_p: 0){ _LL739:
_temp738=( struct _tuple1*)(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp654)->f1;
goto _LL737; _LL737: _temp736=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp654)->f2; goto _LL735; _LL735: _temp734=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnknownCall_p_struct*) _temp654)->f3; goto _LL685;} else{ goto _LL686;}
_LL686: if(( unsigned int) _temp654 > 2u?*(( int*) _temp654) == Cyc_Absyn_UnknownFields_p:
0){ _LL745: _temp744=( struct _tuple1*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp654)->f1; goto _LL743; _LL743: _temp742=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnknownFields_p_struct*) _temp654)->f2; goto _LL741; _LL741: _temp740=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp654)->f3;
goto _LL687;} else{ goto _LL655;} _LL657: s=( void*) Cyc_Tcpat_Any; goto _LL655;
_LL659: s=( void*) Cyc_Tcpat_Any; goto _LL655; _LL661: s= Cyc_Tcpat_null_pat();
goto _LL655; _LL663: s= Cyc_Tcpat_int_pat( _temp690); goto _LL655; _LL665: s=
Cyc_Tcpat_char_pat( _temp694); goto _LL655; _LL667: s= Cyc_Tcpat_float_pat(
_temp696); goto _LL655; _LL669: s=( void*) Cyc_Tcpat_Any; goto _LL655; _LL671:{
void* _temp747= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp746= p->topt;
if( _temp746 == 0){ _throw( Null_Exception);} _temp746->v;})); struct Cyc_Absyn_PtrInfo
_temp755; struct Cyc_Absyn_Conref* _temp757; struct Cyc_Absyn_Tqual _temp759;
struct Cyc_Absyn_Conref* _temp761; void* _temp763; void* _temp765; struct Cyc_Absyn_TunionInfo
_temp767; _LL749: if(( unsigned int) _temp747 > 4u?*(( int*) _temp747) == Cyc_Absyn_PointerType:
0){ _LL756: _temp755=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp747)->f1; _LL766: _temp765=( void*) _temp755.elt_typ; goto _LL764; _LL764:
_temp763=( void*) _temp755.rgn_typ; goto _LL762; _LL762: _temp761=( struct Cyc_Absyn_Conref*)
_temp755.nullable; goto _LL760; _LL760: _temp759=( struct Cyc_Absyn_Tqual)
_temp755.tq; goto _LL758; _LL758: _temp757=( struct Cyc_Absyn_Conref*) _temp755.bounds;
goto _LL750;} else{ goto _LL751;} _LL751: if(( unsigned int) _temp747 > 4u?*((
int*) _temp747) == Cyc_Absyn_TunionType: 0){ _LL768: _temp767=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp747)->f1; goto _LL752;} else{ goto
_LL753;} _LL753: goto _LL754; _LL750: { int is_nullable= 0; int still_working= 1;
while( still_working) { void* _temp769=( void*) _temp761->v; struct Cyc_Absyn_Conref*
_temp777; int _temp779; _LL771: if(( unsigned int) _temp769 > 1u?*(( int*)
_temp769) == Cyc_Absyn_Forward_constr: 0){ _LL778: _temp777=( struct Cyc_Absyn_Conref*)((
struct Cyc_Absyn_Forward_constr_struct*) _temp769)->f1; goto _LL772;} else{ goto
_LL773;} _LL773: if( _temp769 ==( void*) Cyc_Absyn_No_constr){ goto _LL774;}
else{ goto _LL775;} _LL775: if(( unsigned int) _temp769 > 1u?*(( int*) _temp769)
== Cyc_Absyn_Eq_constr: 0){ _LL780: _temp779=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp769)->f1; goto _LL776;} else{ goto _LL770;} _LL772:( void*)( _temp761->v=(
void*)(( void*) _temp777->v)); continue; _LL774:( void*)( _temp761->v=( void*)((
void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp781=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp781[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp782; _temp782.tag= Cyc_Absyn_Eq_constr; _temp782.f1=(
void*) 0; _temp782;}); _temp781;}))); is_nullable= 0; still_working= 0; goto
_LL770; _LL776: is_nullable=( int) _temp779; still_working= 0; goto _LL770;
_LL770:;}{ void* ss= Cyc_Tcpat_compile_pat( _temp700); if( is_nullable){ s= Cyc_Tcpat_null_ptr_pat(
ss);} else{ s= Cyc_Tcpat_ptr_pat( ss);} goto _LL748;}} _LL752:{ void* _temp783=(
void*) _temp700->r; struct Cyc_List_List* _temp789; struct Cyc_List_List*
_temp791; struct Cyc_Absyn_Tunionfield* _temp793; struct Cyc_Absyn_Tuniondecl*
_temp795; _LL785: if(( unsigned int) _temp783 > 2u?*(( int*) _temp783) == Cyc_Absyn_Tunion_p:
0){ _LL796: _temp795=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp783)->f1; goto _LL794; _LL794: _temp793=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp783)->f2; goto _LL792; _LL792: _temp791=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp783)->f3; goto
_LL790; _LL790: _temp789=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp783)->f4; goto _LL786;} else{ goto _LL787;} _LL787: goto _LL788; _LL786: {
struct Cyc_Core_Opt* span; if( _temp795->is_xtunion){ span= 0;} else{ span=({
struct Cyc_Core_Opt* _temp797=( struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof(
struct Cyc_Core_Opt)); _temp797->v=( void*)(( int(*)( struct Cyc_List_List* x))
Cyc_List_length)(( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp798=
_temp795->fields; if( _temp798 == 0){ _throw( Null_Exception);} _temp798->v;}));
_temp797;});} s= Cyc_Tcpat_con_pat(*(* _temp793->name).f2, span,(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_compile_pat,
_temp789)); goto _LL784;} _LL788: s=(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp799=( char*)"non-[x]tunion pattern has tunion type";
struct _tagged_string _temp800; _temp800.curr= _temp799; _temp800.base= _temp799;
_temp800.last_plus_one= _temp799 + 38; _temp800;})); goto _LL784; _LL784:;} goto
_LL748; _LL754: s=(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp801=( char*)"pointer pattern does not have pointer type!";
struct _tagged_string _temp802; _temp802.curr= _temp801; _temp802.base= _temp801;
_temp802.last_plus_one= _temp801 + 44; _temp802;})); goto _LL748; _LL748:;} goto
_LL655; _LL673: { struct Cyc_Core_Opt* span;{ void* _temp804= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp803= p->topt; if( _temp803 == 0){ _throw(
Null_Exception);} _temp803->v;})); struct Cyc_Absyn_TunionInfo _temp812; struct
Cyc_Absyn_TunionFieldInfo _temp814; _LL806: if(( unsigned int) _temp804 > 4u?*((
int*) _temp804) == Cyc_Absyn_TunionType: 0){ _LL813: _temp812=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp804)->f1; goto _LL807;} else{ goto
_LL808;} _LL808: if(( unsigned int) _temp804 > 4u?*(( int*) _temp804) == Cyc_Absyn_TunionFieldType:
0){ _LL815: _temp814=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp804)->f1; goto _LL809;} else{ goto _LL810;} _LL810: goto _LL811; _LL807:
if( _temp708->is_xtunion){ span= 0;} else{ span=({ struct Cyc_Core_Opt* _temp816=(
struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp816->v=(
void*)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp817= _temp708->fields; if( _temp817 == 0){ _throw(
Null_Exception);} _temp817->v;})); _temp816;});} goto _LL805; _LL809: span=({
struct Cyc_Core_Opt* _temp818=( struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof(
struct Cyc_Core_Opt)); _temp818->v=( void*) 1; _temp818;}); goto _LL805; _LL811:
span=(( struct Cyc_Core_Opt*(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp819=( char*)"void tunion pattern has bad type";
struct _tagged_string _temp820; _temp820.curr= _temp819; _temp820.base= _temp819;
_temp820.last_plus_one= _temp819 + 33; _temp820;})); goto _LL805; _LL805:;} s=
Cyc_Tcpat_con_pat(*(* _temp706->name).f2, span, 0); goto _LL655;} _LL675:
_temp718= _temp710; goto _LL677; _LL677: s= Cyc_Tcpat_tuple_pat((( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_compile_pat,
_temp718)); goto _LL655; _LL679: { struct Cyc_List_List* ps= 0;{ struct Cyc_List_List*
fields=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp853= _temp726->fields;
if( _temp853 == 0){ _throw( Null_Exception);} _temp853->v;}); for( 0; fields !=
0; fields=({ struct Cyc_List_List* _temp821= fields; if( _temp821 == 0){ _throw(
Null_Exception);} _temp821->tl;})){ int found= Cyc_String_strcmp(*(( struct Cyc_Absyn_Structfield*)({
struct Cyc_List_List* _temp850= fields; if( _temp850 == 0){ _throw(
Null_Exception);} _temp850->hd;}))->name,( struct _tagged_string)({ char*
_temp851=( char*)""; struct _tagged_string _temp852; _temp852.curr= _temp851;
_temp852.base= _temp851; _temp852.last_plus_one= _temp851 + 1; _temp852;})) == 0;{
struct Cyc_List_List* dlps0= _temp720; for( 0; ! found? dlps0 != 0: 0; dlps0=({
struct Cyc_List_List* _temp822= dlps0; if( _temp822 == 0){ _throw(
Null_Exception);} _temp822->tl;})){ struct _tuple6 _temp826; struct Cyc_Absyn_Pat*
_temp827; struct Cyc_List_List* _temp829; struct _tuple6* _temp824=( struct
_tuple6*)({ struct Cyc_List_List* _temp823= dlps0; if( _temp823 == 0){ _throw(
Null_Exception);} _temp823->hd;}); _temp826=* _temp824; _LL830: _temp829=
_temp826.f1; goto _LL828; _LL828: _temp827= _temp826.f2; goto _LL825; _LL825: {
struct Cyc_List_List* _temp831= _temp829; struct Cyc_List_List _temp837; struct
Cyc_List_List* _temp838; void* _temp840; struct _tagged_string* _temp842; _LL833:
if( _temp831 == 0){ goto _LL835;} else{ _temp837=* _temp831; _LL841: _temp840=(
void*) _temp837.hd; if(*(( int*) _temp840) == Cyc_Absyn_FieldName){ _LL843:
_temp842=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp840)->f1; goto _LL839;} else{ goto _LL835;} _LL839: _temp838=( struct Cyc_List_List*)
_temp837.tl; if( _temp838 == 0){ goto _LL834;} else{ goto _LL835;}} _LL835: goto
_LL836; _LL834: if( Cyc_String_zstrptrcmp( _temp842,(( struct Cyc_Absyn_Structfield*)({
struct Cyc_List_List* _temp844= fields; if( _temp844 == 0){ _throw(
Null_Exception);} _temp844->hd;}))->name) == 0){ ps=({ struct Cyc_List_List*
_temp845=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp845->hd=( void*) Cyc_Tcpat_compile_pat( _temp827); _temp845->tl= ps;
_temp845;}); found= 1;} goto _LL832; _LL836:(( void(*)( struct _tagged_string))
Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp846=( char*)"bad designator(s)";
struct _tagged_string _temp847; _temp847.curr= _temp846; _temp847.base= _temp846;
_temp847.last_plus_one= _temp846 + 18; _temp847;})); goto _LL832; _LL832:;}}}
if( ! found){(( void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ char* _temp848=( char*)"bad designator"; struct _tagged_string
_temp849; _temp849.curr= _temp848; _temp849.base= _temp848; _temp849.last_plus_one=
_temp848 + 15; _temp849;}));}}} s= Cyc_Tcpat_tuple_pat( ps); goto _LL655;}
_LL681: { int span=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)((
struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp855= _temp730->fields; if(
_temp855 == 0){ _throw( Null_Exception);} _temp855->v;})); s= Cyc_Tcpat_con_pat(*(*
_temp728->name).f2,({ struct Cyc_Core_Opt* _temp854=( struct Cyc_Core_Opt*)
GC_malloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp854->v=( void*) span;
_temp854;}), 0); goto _LL655;} _LL683: s=(( void*(*)( struct _tagged_string))
Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp856=( char*)"compile_pat: unknownid";
struct _tagged_string _temp857; _temp857.curr= _temp856; _temp857.base= _temp856;
_temp857.last_plus_one= _temp856 + 23; _temp857;})); goto _LL655; _LL685: s=((
void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp858=( char*)"compile_pat: unknowncall"; struct _tagged_string
_temp859; _temp859.curr= _temp858; _temp859.base= _temp858; _temp859.last_plus_one=
_temp858 + 25; _temp859;})); goto _LL655; _LL687: s=(( void*(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp860=(
char*)"compile_pat: unknownfields"; struct _tagged_string _temp861; _temp861.curr=
_temp860; _temp861.base= _temp860; _temp861.last_plus_one= _temp860 + 27;
_temp861;})); goto _LL655; _LL655:;} return s;} typedef struct Cyc_List_List*
Cyc_Tcpat_match_t; static const int Cyc_Tcpat_Pos= 0; struct Cyc_Tcpat_Pos_struct{
int tag; struct Cyc_Tcpat_Con_s* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Tcpat_Neg= 1; struct Cyc_Tcpat_Neg_struct{ int tag; struct Cyc_Set_Set*
f1; } ; typedef void* Cyc_Tcpat_term_desc_t; typedef struct Cyc_List_List* Cyc_Tcpat_context_t;
typedef struct Cyc_List_List* Cyc_Tcpat_access_t; static const int Cyc_Tcpat_Failure=
0; static const int Cyc_Tcpat_Success= 0; struct Cyc_Tcpat_Success_struct{ int
tag; void* f1; } ; static const int Cyc_Tcpat_IfEq= 1; struct Cyc_Tcpat_IfEq_struct{
int tag; struct Cyc_List_List* f1; struct Cyc_Tcpat_Con_s* f2; void* f3; void*
f4; } ; typedef void* Cyc_Tcpat_decision_t; struct _tuple9{ struct Cyc_List_List*
f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3; } ; typedef struct
_tuple9* Cyc_Tcpat_work_stack_frame_t; typedef struct Cyc_List_List* Cyc_Tcpat_work_stack_t;
static const int Cyc_Tcpat_Yes= 0; static const int Cyc_Tcpat_No= 1; static
const int Cyc_Tcpat_Maybe= 2; typedef void* Cyc_Tcpat_answer_t; static void* Cyc_Tcpat_add_neg(
void* td, struct Cyc_Tcpat_Con_s* c){ void* _temp862= td; struct Cyc_Set_Set*
_temp868; struct Cyc_List_List* _temp870; struct Cyc_Tcpat_Con_s* _temp872;
_LL864: if(*(( int*) _temp862) == Cyc_Tcpat_Neg){ _LL869: _temp868=( struct Cyc_Set_Set*)((
struct Cyc_Tcpat_Neg_struct*) _temp862)->f1; goto _LL865;} else{ goto _LL866;}
_LL866: if(*(( int*) _temp862) == Cyc_Tcpat_Pos){ _LL873: _temp872=( struct Cyc_Tcpat_Con_s*)((
struct Cyc_Tcpat_Pos_struct*) _temp862)->f1; goto _LL871; _LL871: _temp870=(
struct Cyc_List_List*)(( struct Cyc_Tcpat_Pos_struct*) _temp862)->f2; goto
_LL867;} else{ goto _LL863;} _LL865: if((( int(*)( struct Cyc_Set_Set* s, struct
Cyc_Tcpat_Con_s* elt)) Cyc_Set_member)( _temp868, c)){(( void(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp874=(
char*)"add_neg called when constructor already in set"; struct _tagged_string
_temp875; _temp875.curr= _temp874; _temp875.base= _temp874; _temp875.last_plus_one=
_temp874 + 47; _temp875;}));} if( c->span != 0?(( int(*)( struct Cyc_Set_Set* s))
Cyc_Set_cardinality)( _temp868) + 1 >=( int)({ struct Cyc_Core_Opt* _temp876= c->span;
if( _temp876 == 0){ _throw( Null_Exception);} _temp876->v;}): 0){(( void(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp877=( char*)"add_neg called when |cs U {c}| >= span(c)"; struct
_tagged_string _temp878; _temp878.curr= _temp877; _temp878.base= _temp877;
_temp878.last_plus_one= _temp877 + 42; _temp878;}));} return( void*)({ struct
Cyc_Tcpat_Neg_struct* _temp879=( struct Cyc_Tcpat_Neg_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Neg_struct)); _temp879[ 0]=({ struct Cyc_Tcpat_Neg_struct
_temp880; _temp880.tag= Cyc_Tcpat_Neg; _temp880.f1=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_insert)( _temp868,
c); _temp880;}); _temp879;}); _LL867: return(( void*(*)( struct _tagged_string))
Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp881=( char*)"add_neg called when td is Positive";
struct _tagged_string _temp882; _temp882.curr= _temp881; _temp882.base= _temp881;
_temp882.last_plus_one= _temp881 + 35; _temp882;})); _LL863:;} static void* Cyc_Tcpat_static_match(
struct Cyc_Tcpat_Con_s* c, void* td){ void* _temp883= td; struct Cyc_List_List*
_temp889; struct Cyc_Tcpat_Con_s* _temp891; struct Cyc_Set_Set* _temp893; _LL885:
if(*(( int*) _temp883) == Cyc_Tcpat_Pos){ _LL892: _temp891=( struct Cyc_Tcpat_Con_s*)((
struct Cyc_Tcpat_Pos_struct*) _temp883)->f1; goto _LL890; _LL890: _temp889=(
struct Cyc_List_List*)(( struct Cyc_Tcpat_Pos_struct*) _temp883)->f2; goto
_LL886;} else{ goto _LL887;} _LL887: if(*(( int*) _temp883) == Cyc_Tcpat_Neg){
_LL894: _temp893=( struct Cyc_Set_Set*)(( struct Cyc_Tcpat_Neg_struct*) _temp883)->f1;
goto _LL888;} else{ goto _LL884;} _LL886: if( Cyc_Tcpat_compare_con( c, _temp891)
== 0){ return( void*) Cyc_Tcpat_Yes;} else{ return( void*) Cyc_Tcpat_No;} _LL888:
if((( int(*)( struct Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_member)(
_temp893, c)){ return( void*) Cyc_Tcpat_No;} else{ if( c->span != 0?( int)({
struct Cyc_Core_Opt* _temp895= c->span; if( _temp895 == 0){ _throw(
Null_Exception);} _temp895->v;}) ==(( int(*)( struct Cyc_Set_Set* s)) Cyc_Set_cardinality)(
_temp893) + 1: 0){ return( void*) Cyc_Tcpat_Yes;} else{ return( void*) Cyc_Tcpat_Maybe;}}
_LL884:;} struct _tuple10{ struct Cyc_Tcpat_Con_s* f1; struct Cyc_List_List* f2;
} ; static struct Cyc_List_List* Cyc_Tcpat_augment( struct Cyc_List_List* ctxt,
void* dsc){ struct Cyc_List_List* _temp896= ctxt; struct Cyc_List_List _temp902;
struct Cyc_List_List* _temp903; struct _tuple10* _temp905; struct _tuple10
_temp907; struct Cyc_List_List* _temp908; struct Cyc_Tcpat_Con_s* _temp910;
_LL898: if( _temp896 == 0){ goto _LL899;} else{ goto _LL900;} _LL900: if(
_temp896 == 0){ goto _LL897;} else{ _temp902=* _temp896; _LL906: _temp905=(
struct _tuple10*) _temp902.hd; _temp907=* _temp905; _LL911: _temp910= _temp907.f1;
goto _LL909; _LL909: _temp908= _temp907.f2; goto _LL904; _LL904: _temp903=(
struct Cyc_List_List*) _temp902.tl; goto _LL901;} _LL899: return 0; _LL901:
return({ struct Cyc_List_List* _temp912=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp912->hd=( void*)({ struct _tuple10*
_temp913=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp913->f1=
_temp910; _temp913->f2=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp914=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp914->hd=(
void*) dsc; _temp914->tl= _temp908; _temp914;}); _temp913;}); _temp912->tl=
_temp903; _temp912;}); _LL897:;} static struct Cyc_List_List* Cyc_Tcpat_norm_context(
struct Cyc_List_List* ctxt){ struct Cyc_List_List* _temp915= ctxt; struct Cyc_List_List
_temp921; struct Cyc_List_List* _temp922; struct _tuple10* _temp924; struct
_tuple10 _temp926; struct Cyc_List_List* _temp927; struct Cyc_Tcpat_Con_s*
_temp929; _LL917: if( _temp915 == 0){ goto _LL918;} else{ goto _LL919;} _LL919:
if( _temp915 == 0){ goto _LL916;} else{ _temp921=* _temp915; _LL925: _temp924=(
struct _tuple10*) _temp921.hd; _temp926=* _temp924; _LL930: _temp929= _temp926.f1;
goto _LL928; _LL928: _temp927= _temp926.f2; goto _LL923; _LL923: _temp922=(
struct Cyc_List_List*) _temp921.tl; goto _LL920;} _LL918: return(( struct Cyc_List_List*(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp931=( char*)"norm_context: empty context"; struct _tagged_string _temp932;
_temp932.curr= _temp931; _temp932.base= _temp931; _temp932.last_plus_one=
_temp931 + 28; _temp932;})); _LL920: return Cyc_Tcpat_augment( _temp922,( void*)({
struct Cyc_Tcpat_Pos_struct* _temp933=( struct Cyc_Tcpat_Pos_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Pos_struct)); _temp933[ 0]=({ struct Cyc_Tcpat_Pos_struct
_temp934; _temp934.tag= Cyc_Tcpat_Pos; _temp934.f1= _temp929; _temp934.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( _temp927);
_temp934;}); _temp933;})); _LL916:;} static void* Cyc_Tcpat_build_desc( struct
Cyc_List_List* ctxt, void* dsc, struct Cyc_List_List* work){ struct _tuple5
_temp936=({ struct _tuple5 _temp935; _temp935.f1= ctxt; _temp935.f2= work;
_temp935;}); struct Cyc_List_List* _temp946; struct Cyc_List_List* _temp948;
struct Cyc_List_List* _temp950; struct Cyc_List_List* _temp952; struct Cyc_List_List*
_temp954; struct Cyc_List_List* _temp956; struct Cyc_List_List* _temp958; struct
Cyc_List_List _temp960; struct Cyc_List_List* _temp961; struct _tuple9* _temp963;
struct _tuple9 _temp965; struct Cyc_List_List* _temp966; struct Cyc_List_List*
_temp968; struct Cyc_List_List* _temp970; struct Cyc_List_List* _temp972; struct
Cyc_List_List _temp974; struct Cyc_List_List* _temp975; struct _tuple10*
_temp977; struct _tuple10 _temp979; struct Cyc_List_List* _temp980; struct Cyc_Tcpat_Con_s*
_temp982; _LL938: _LL949: _temp948= _temp936.f1; if( _temp948 == 0){ goto _LL947;}
else{ goto _LL940;} _LL947: _temp946= _temp936.f2; if( _temp946 == 0){ goto
_LL939;} else{ goto _LL940;} _LL940: _LL953: _temp952= _temp936.f1; if( _temp952
== 0){ goto _LL951;} else{ goto _LL942;} _LL951: _temp950= _temp936.f2; goto
_LL941; _LL942: _LL957: _temp956= _temp936.f1; goto _LL955; _LL955: _temp954=
_temp936.f2; if( _temp954 == 0){ goto _LL943;} else{ goto _LL944;} _LL944:
_LL973: _temp972= _temp936.f1; if( _temp972 == 0){ goto _LL937;} else{ _temp974=*
_temp972; _LL978: _temp977=( struct _tuple10*) _temp974.hd; _temp979=* _temp977;
_LL983: _temp982= _temp979.f1; goto _LL981; _LL981: _temp980= _temp979.f2; goto
_LL976; _LL976: _temp975=( struct Cyc_List_List*) _temp974.tl; goto _LL959;}
_LL959: _temp958= _temp936.f2; if( _temp958 == 0){ goto _LL937;} else{ _temp960=*
_temp958; _LL964: _temp963=( struct _tuple9*) _temp960.hd; _temp965=* _temp963;
_LL971: _temp970= _temp965.f1; goto _LL969; _LL969: _temp968= _temp965.f2; goto
_LL967; _LL967: _temp966= _temp965.f3; goto _LL962; _LL962: _temp961=( struct
Cyc_List_List*) _temp960.tl; goto _LL945;} _LL939: return dsc; _LL941: return((
void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp984=( char*)"build_desc: ctxt and work don't match"; struct
_tagged_string _temp985; _temp985.curr= _temp984; _temp985.base= _temp984;
_temp985.last_plus_one= _temp984 + 38; _temp985;})); _LL943: return(( void*(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp986=( char*)"build_desc: ctxt and work don't match"; struct _tagged_string
_temp987; _temp987.curr= _temp986; _temp987.base= _temp986; _temp987.last_plus_one=
_temp986 + 38; _temp987;})); _LL945: { struct Cyc_Tcpat_Pos_struct* _temp991=({
struct Cyc_Tcpat_Pos_struct* _temp988=( struct Cyc_Tcpat_Pos_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Pos_struct)); _temp988[ 0]=({ struct Cyc_Tcpat_Pos_struct
_temp989; _temp989.tag= Cyc_Tcpat_Pos; _temp989.f1= _temp982; _temp989.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( _temp980),({
struct Cyc_List_List* _temp990=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp990->hd=( void*) dsc; _temp990->tl= _temp966;
_temp990;})); _temp989;}); _temp988;}); goto _LL992; _LL992: return Cyc_Tcpat_build_desc(
_temp975,( void*) _temp991, _temp961);} _LL937:;} static void* Cyc_Tcpat_match(
void* p, struct Cyc_List_List* obj, void* dsc, struct Cyc_List_List* ctx, struct
Cyc_List_List* work, void* right_hand_side, struct Cyc_List_List* rules); static
void* Cyc_Tcpat_or_match( void* dsc, struct Cyc_List_List* allmrules){ struct
Cyc_List_List* _temp993= allmrules; struct Cyc_List_List _temp999; struct Cyc_List_List*
_temp1000; struct _tuple0* _temp1002; struct _tuple0 _temp1004; void* _temp1005;
void* _temp1007; _LL995: if( _temp993 == 0){ goto _LL996;} else{ goto _LL997;}
_LL997: if( _temp993 == 0){ goto _LL994;} else{ _temp999=* _temp993; _LL1003:
_temp1002=( struct _tuple0*) _temp999.hd; _temp1004=* _temp1002; _LL1008:
_temp1007= _temp1004.f1; goto _LL1006; _LL1006: _temp1005= _temp1004.f2; goto
_LL1001; _LL1001: _temp1000=( struct Cyc_List_List*) _temp999.tl; goto _LL998;}
_LL996: return( void*) Cyc_Tcpat_Failure; _LL998: return(( void*(*)( void* p,
struct Cyc_List_List* obj, void* dsc, struct Cyc_List_List* ctx, struct Cyc_List_List*
work, void* right_hand_side, struct Cyc_List_List* rules)) Cyc_Tcpat_match)(
_temp1007, 0, dsc, 0, 0, _temp1005, _temp1000); _LL994:;} static void* Cyc_Tcpat_match_compile(
struct Cyc_List_List* allmrules){ return(( void*(*)( void* dsc, struct Cyc_List_List*
allmrules)) Cyc_Tcpat_or_match)(( void*)({ struct Cyc_Tcpat_Neg_struct*
_temp1009=( struct Cyc_Tcpat_Neg_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Neg_struct));
_temp1009[ 0]=({ struct Cyc_Tcpat_Neg_struct _temp1010; _temp1010.tag= Cyc_Tcpat_Neg;
_temp1010.f1= Cyc_Tcpat_empty_con_set(); _temp1010;}); _temp1009;}), allmrules);}
static void* Cyc_Tcpat_and_match( struct Cyc_List_List* ctx, struct Cyc_List_List*
work, void* right_hand_side, struct Cyc_List_List* rules){ struct Cyc_List_List*
_temp1011= work; struct Cyc_List_List _temp1019; struct Cyc_List_List* _temp1020;
struct _tuple9* _temp1022; struct _tuple9 _temp1024; struct Cyc_List_List*
_temp1025; struct Cyc_List_List* _temp1027; struct Cyc_List_List* _temp1029;
struct Cyc_List_List _temp1031; struct Cyc_List_List* _temp1032; struct _tuple9*
_temp1034; struct _tuple9 _temp1036; struct Cyc_List_List* _temp1037; struct Cyc_List_List*
_temp1039; struct Cyc_List_List* _temp1041; _LL1013: if( _temp1011 == 0){ goto
_LL1014;} else{ goto _LL1015;} _LL1015: if( _temp1011 == 0){ goto _LL1017;}
else{ _temp1019=* _temp1011; _LL1023: _temp1022=( struct _tuple9*) _temp1019.hd;
_temp1024=* _temp1022; _LL1030: _temp1029= _temp1024.f1; if( _temp1029 == 0){
goto _LL1028;} else{ goto _LL1017;} _LL1028: _temp1027= _temp1024.f2; if(
_temp1027 == 0){ goto _LL1026;} else{ goto _LL1017;} _LL1026: _temp1025=
_temp1024.f3; if( _temp1025 == 0){ goto _LL1021;} else{ goto _LL1017;} _LL1021:
_temp1020=( struct Cyc_List_List*) _temp1019.tl; goto _LL1016;} _LL1017: if(
_temp1011 == 0){ goto _LL1012;} else{ _temp1031=* _temp1011; _LL1035: _temp1034=(
struct _tuple9*) _temp1031.hd; _temp1036=* _temp1034; _LL1042: _temp1041=
_temp1036.f1; goto _LL1040; _LL1040: _temp1039= _temp1036.f2; goto _LL1038;
_LL1038: _temp1037= _temp1036.f3; goto _LL1033; _LL1033: _temp1032=( struct Cyc_List_List*)
_temp1031.tl; goto _LL1018;} _LL1014: return( void*)({ struct Cyc_Tcpat_Success_struct*
_temp1043=( struct Cyc_Tcpat_Success_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Success_struct));
_temp1043[ 0]=({ struct Cyc_Tcpat_Success_struct _temp1044; _temp1044.tag= Cyc_Tcpat_Success;
_temp1044.f1=( void*) right_hand_side; _temp1044;}); _temp1043;}); _LL1016:
return(( void*(*)( struct Cyc_List_List* ctx, struct Cyc_List_List* work, void*
right_hand_side, struct Cyc_List_List* rules)) Cyc_Tcpat_and_match)( Cyc_Tcpat_norm_context(
ctx), _temp1020, right_hand_side, rules); _LL1018: if(( _temp1041 == 0? 1:
_temp1039 == 0)? 1: _temp1037 == 0){ return(( void*(*)( struct _tagged_string))
Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp1045=( char*)"tcpat:and_match: malformed work frame";
struct _tagged_string _temp1046; _temp1046.curr= _temp1045; _temp1046.base=
_temp1045; _temp1046.last_plus_one= _temp1045 + 38; _temp1046;}));}{ struct Cyc_List_List
_temp1050; struct Cyc_List_List* _temp1051; void* _temp1053; struct Cyc_List_List*
_temp1048=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp1047= _temp1041;
if( _temp1047 == 0){ _throw( Null_Exception);} _temp1047;}); _temp1050=*
_temp1048; _LL1054: _temp1053=( void*) _temp1050.hd; goto _LL1052; _LL1052:
_temp1051=( struct Cyc_List_List*) _temp1050.tl; goto _LL1049; _LL1049: { struct
Cyc_List_List _temp1058; struct Cyc_List_List* _temp1059; struct Cyc_List_List*
_temp1061; struct Cyc_List_List* _temp1056=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp1055= _temp1039; if( _temp1055 == 0){ _throw( Null_Exception);} _temp1055;});
_temp1058=* _temp1056; _LL1062: _temp1061=( struct Cyc_List_List*) _temp1058.hd;
goto _LL1060; _LL1060: _temp1059=( struct Cyc_List_List*) _temp1058.tl; goto
_LL1057; _LL1057: { struct Cyc_List_List _temp1066; struct Cyc_List_List*
_temp1067; void* _temp1069; struct Cyc_List_List* _temp1064=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp1063= _temp1037; if( _temp1063 == 0){ _throw(
Null_Exception);} _temp1063;}); _temp1066=* _temp1064; _LL1070: _temp1069=( void*)
_temp1066.hd; goto _LL1068; _LL1068: _temp1067=( struct Cyc_List_List*)
_temp1066.tl; goto _LL1065; _LL1065: { struct _tuple9* _temp1072=({ struct
_tuple9* _temp1071=( struct _tuple9*) GC_malloc( sizeof( struct _tuple9));
_temp1071->f1= _temp1051; _temp1071->f2= _temp1059; _temp1071->f3= _temp1067;
_temp1071;}); goto _LL1073; _LL1073: return(( void*(*)( void* p, struct Cyc_List_List*
obj, void* dsc, struct Cyc_List_List* ctx, struct Cyc_List_List* work, void*
right_hand_side, struct Cyc_List_List* rules)) Cyc_Tcpat_match)( _temp1053,
_temp1061, _temp1069, ctx,({ struct Cyc_List_List* _temp1074=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1074->hd=( void*) _temp1072;
_temp1074->tl= _temp1032; _temp1074;}), right_hand_side, rules);}}}} _LL1012:;}
static struct Cyc_List_List* Cyc_Tcpat_getdargs( struct Cyc_Tcpat_Con_s* pcon,
void* dsc){ void* _temp1075= dsc; struct Cyc_Set_Set* _temp1081; struct Cyc_List_List*
_temp1083; struct Cyc_Tcpat_Con_s* _temp1085; _LL1077: if(*(( int*) _temp1075)
== Cyc_Tcpat_Neg){ _LL1082: _temp1081=( struct Cyc_Set_Set*)(( struct Cyc_Tcpat_Neg_struct*)
_temp1075)->f1; goto _LL1078;} else{ goto _LL1079;} _LL1079: if(*(( int*)
_temp1075) == Cyc_Tcpat_Pos){ _LL1086: _temp1085=( struct Cyc_Tcpat_Con_s*)((
struct Cyc_Tcpat_Pos_struct*) _temp1075)->f1; goto _LL1084; _LL1084: _temp1083=(
struct Cyc_List_List*)(( struct Cyc_Tcpat_Pos_struct*) _temp1075)->f2; goto
_LL1080;} else{ goto _LL1076;} _LL1078: { void* any=( void*)({ struct Cyc_Tcpat_Neg_struct*
_temp1090=( struct Cyc_Tcpat_Neg_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Neg_struct));
_temp1090[ 0]=({ struct Cyc_Tcpat_Neg_struct _temp1091; _temp1091.tag= Cyc_Tcpat_Neg;
_temp1091.f1= Cyc_Tcpat_empty_con_set(); _temp1091;}); _temp1090;}); struct Cyc_List_List*
_temp1087= 0; goto _LL1088; _LL1088:{ int i= 0; for( 0; i < pcon->arity; ++ i){
_temp1087=({ struct Cyc_List_List* _temp1089=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1089->hd=( void*) any; _temp1089->tl=
_temp1087; _temp1089;});}} return _temp1087;} _LL1080: return _temp1083; _LL1076:;}
static struct Cyc_List_List* Cyc_Tcpat_getoarg( struct Cyc_List_List* obj, int i){
return({ struct Cyc_List_List* _temp1092=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1092->hd=( void*)( i + 1); _temp1092->tl=
obj; _temp1092;});} static struct Cyc_List_List* Cyc_Tcpat_getoargs( struct Cyc_Tcpat_Con_s*
pcon, struct Cyc_List_List* obj){ return(( struct Cyc_List_List*(*)( int n,
struct Cyc_List_List*(* f)( struct Cyc_List_List*, int), struct Cyc_List_List*
env)) Cyc_List_tabulate_c)( pcon->arity, Cyc_Tcpat_getoarg, obj);} static void*
Cyc_Tcpat_match( void* p, struct Cyc_List_List* obj, void* dsc, struct Cyc_List_List*
ctx, struct Cyc_List_List* work, void* right_hand_side, struct Cyc_List_List*
rules){ void* _temp1093= p; struct Cyc_List_List* _temp1099; struct Cyc_Tcpat_Con_s*
_temp1101; _LL1095: if( _temp1093 ==( void*) Cyc_Tcpat_Any){ goto _LL1096;}
else{ goto _LL1097;} _LL1097: if(( unsigned int) _temp1093 > 1u?*(( int*)
_temp1093) == Cyc_Tcpat_Con: 0){ _LL1102: _temp1101=( struct Cyc_Tcpat_Con_s*)((
struct Cyc_Tcpat_Con_struct*) _temp1093)->f1; goto _LL1100; _LL1100: _temp1099=(
struct Cyc_List_List*)(( struct Cyc_Tcpat_Con_struct*) _temp1093)->f2; goto
_LL1098;} else{ goto _LL1094;} _LL1096: return(( void*(*)( struct Cyc_List_List*
ctx, struct Cyc_List_List* work, void* right_hand_side, struct Cyc_List_List*
rules)) Cyc_Tcpat_and_match)( Cyc_Tcpat_augment( ctx, dsc), work,
right_hand_side, rules); _LL1098: { void* _temp1103= Cyc_Tcpat_static_match(
_temp1101, dsc); _LL1105: if( _temp1103 ==( void*) Cyc_Tcpat_Yes){ goto _LL1106;}
else{ goto _LL1107;} _LL1107: if( _temp1103 ==( void*) Cyc_Tcpat_No){ goto
_LL1108;} else{ goto _LL1109;} _LL1109: if( _temp1103 ==( void*) Cyc_Tcpat_Maybe){
goto _LL1110;} else{ goto _LL1104;} _LL1106: { struct Cyc_List_List* _temp1113=({
struct Cyc_List_List* _temp1111=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1111->hd=( void*)({ struct _tuple10* _temp1112=(
struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp1112->f1= _temp1101;
_temp1112->f2= 0; _temp1112;}); _temp1111->tl= ctx; _temp1111;}); goto _LL1114;
_LL1114: { struct _tuple9* _temp1116=({ struct _tuple9* _temp1115=( struct
_tuple9*) GC_malloc( sizeof( struct _tuple9)); _temp1115->f1= _temp1099;
_temp1115->f2= Cyc_Tcpat_getoargs( _temp1101, obj); _temp1115->f3= Cyc_Tcpat_getdargs(
_temp1101, dsc); _temp1115;}); goto _LL1117; _LL1117: { struct Cyc_List_List*
_temp1119=({ struct Cyc_List_List* _temp1118=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1118->hd=( void*) _temp1116; _temp1118->tl=
work; _temp1118;}); goto _LL1120; _LL1120: return(( void*(*)( struct Cyc_List_List*
ctx, struct Cyc_List_List* work, void* right_hand_side, struct Cyc_List_List*
rules)) Cyc_Tcpat_and_match)( _temp1113, _temp1119, right_hand_side, rules);}}}
_LL1108: return(( void*(*)( void* dsc, struct Cyc_List_List* allmrules)) Cyc_Tcpat_or_match)(
Cyc_Tcpat_build_desc( ctx, dsc, work), rules); _LL1110: { struct Cyc_List_List*
_temp1123=({ struct Cyc_List_List* _temp1121=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1121->hd=( void*)({ struct _tuple10*
_temp1122=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp1122->f1=
_temp1101; _temp1122->f2= 0; _temp1122;}); _temp1121->tl= ctx; _temp1121;});
goto _LL1124; _LL1124: { struct _tuple9* _temp1126=({ struct _tuple9* _temp1125=(
struct _tuple9*) GC_malloc( sizeof( struct _tuple9)); _temp1125->f1= _temp1099;
_temp1125->f2= Cyc_Tcpat_getoargs( _temp1101, obj); _temp1125->f3= Cyc_Tcpat_getdargs(
_temp1101, dsc); _temp1125;}); goto _LL1127; _LL1127: { struct Cyc_List_List*
_temp1129=({ struct Cyc_List_List* _temp1128=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1128->hd=( void*) _temp1126; _temp1128->tl=
work; _temp1128;}); goto _LL1130; _LL1130: { void* _temp1131=(( void*(*)( struct
Cyc_List_List* ctx, struct Cyc_List_List* work, void* right_hand_side, struct
Cyc_List_List* rules)) Cyc_Tcpat_and_match)( _temp1123, _temp1129,
right_hand_side, rules); goto _LL1132; _LL1132: { void* _temp1133=(( void*(*)(
void* dsc, struct Cyc_List_List* allmrules)) Cyc_Tcpat_or_match)( Cyc_Tcpat_build_desc(
ctx, Cyc_Tcpat_add_neg( dsc, _temp1101), work), rules); goto _LL1134; _LL1134:
return( void*)({ struct Cyc_Tcpat_IfEq_struct* _temp1135=( struct Cyc_Tcpat_IfEq_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_IfEq_struct)); _temp1135[ 0]=({ struct Cyc_Tcpat_IfEq_struct
_temp1136; _temp1136.tag= Cyc_Tcpat_IfEq; _temp1136.f1= obj; _temp1136.f2=
_temp1101; _temp1136.f3=( void*) _temp1131; _temp1136.f4=( void*) _temp1133;
_temp1136;}); _temp1135;});}}}}} _LL1104:;} _LL1094:;} static void Cyc_Tcpat_check_exhaust_overlap(
void* d, void(* not_exhaust)( void*), void* env1, void(* rhs_appears)( void*,
void*), void* env2){ void* _temp1137= d; void* _temp1145; void* _temp1147; void*
_temp1149; struct Cyc_Tcpat_Con_s* _temp1151; struct Cyc_List_List* _temp1153;
_LL1139: if( _temp1137 ==( void*) Cyc_Tcpat_Failure){ goto _LL1140;} else{ goto
_LL1141;} _LL1141: if(( unsigned int) _temp1137 > 1u?*(( int*) _temp1137) == Cyc_Tcpat_Success:
0){ _LL1146: _temp1145=( void*)(( struct Cyc_Tcpat_Success_struct*) _temp1137)->f1;
goto _LL1142;} else{ goto _LL1143;} _LL1143: if(( unsigned int) _temp1137 > 1u?*((
int*) _temp1137) == Cyc_Tcpat_IfEq: 0){ _LL1154: _temp1153=( struct Cyc_List_List*)((
struct Cyc_Tcpat_IfEq_struct*) _temp1137)->f1; goto _LL1152; _LL1152: _temp1151=(
struct Cyc_Tcpat_Con_s*)(( struct Cyc_Tcpat_IfEq_struct*) _temp1137)->f2; goto
_LL1150; _LL1150: _temp1149=( void*)(( struct Cyc_Tcpat_IfEq_struct*) _temp1137)->f3;
goto _LL1148; _LL1148: _temp1147=( void*)(( struct Cyc_Tcpat_IfEq_struct*)
_temp1137)->f4; goto _LL1144;} else{ goto _LL1138;} _LL1140: not_exhaust( env1);
goto _LL1138; _LL1142: rhs_appears( env2, _temp1145); goto _LL1138; _LL1144:((
void(*)( void* d, void(* not_exhaust)( void*), void* env1, void(* rhs_appears)(
void*, void*), void* env2)) Cyc_Tcpat_check_exhaust_overlap)( _temp1149,
not_exhaust, env1, rhs_appears, env2);(( void(*)( void* d, void(* not_exhaust)(
void*), void* env1, void(* rhs_appears)( void*, void*), void* env2)) Cyc_Tcpat_check_exhaust_overlap)(
_temp1147, not_exhaust, env1, rhs_appears, env2); goto _LL1138; _LL1138:;}
struct _tuple11{ int f1; struct Cyc_Position_Segment* f2; } ; struct _tuple12{
void* f1; struct _tuple11* f2; } ; static struct _tuple12* Cyc_Tcpat_get_match(
int* r, struct Cyc_Absyn_Switch_clause* swc){ void* sp0= Cyc_Tcpat_compile_pat(
swc->pattern); struct _tuple11* rhs=({ struct _tuple11* _temp1160=( struct
_tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp1160->f1= 0; _temp1160->f2=(
swc->pattern)->loc; _temp1160;}); void* sp; if( swc->where_clause != 0){ sp= Cyc_Tcpat_tuple_pat(({
struct Cyc_List_List* _temp1155=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1155->hd=( void*) sp0; _temp1155->tl=({ struct Cyc_List_List*
_temp1156=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1156->hd=( void*) Cyc_Tcpat_int_pat(* r); _temp1156->tl= 0; _temp1156;});
_temp1155;}));* r=* r + 1;} else{ sp= Cyc_Tcpat_tuple_pat(({ struct Cyc_List_List*
_temp1157=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1157->hd=( void*) sp0; _temp1157->tl=({ struct Cyc_List_List* _temp1158=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1158->hd=(
void*)(( void*) Cyc_Tcpat_Any); _temp1158->tl= 0; _temp1158;}); _temp1157;}));}
return({ struct _tuple12* _temp1159=( struct _tuple12*) GC_malloc( sizeof(
struct _tuple12)); _temp1159->f1= sp; _temp1159->f2= rhs; _temp1159;});} static
void Cyc_Tcpat_not_exhaust_err( struct Cyc_Position_Segment* loc){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp1161=( char*)"patterns may not be exhaustive";
struct _tagged_string _temp1162; _temp1162.curr= _temp1161; _temp1162.base=
_temp1161; _temp1162.last_plus_one= _temp1161 + 31; _temp1162;}));} static void
Cyc_Tcpat_rule_occurs( int dummy, struct _tuple11* rhs){(* rhs).f1= 1;} void Cyc_Tcpat_check_switch_exhaustive(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* swcs){ int _temp1163= 0;
goto _LL1164; _LL1164: { struct Cyc_List_List* _temp1165=(( struct Cyc_List_List*(*)(
struct _tuple12*(* f)( int*, struct Cyc_Absyn_Switch_clause*), int* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcpat_get_match,& _temp1163, swcs); goto
_LL1166; _LL1166: { void* _temp1167=(( void*(*)( struct Cyc_List_List* allmrules))
Cyc_Tcpat_match_compile)( _temp1165); goto _LL1168; _LL1168:(( void(*)( void* d,
void(* not_exhaust)( struct Cyc_Position_Segment*), struct Cyc_Position_Segment*
env1, void(* rhs_appears)( int, struct _tuple11*), int env2)) Cyc_Tcpat_check_exhaust_overlap)(
_temp1167, Cyc_Tcpat_not_exhaust_err, loc, Cyc_Tcpat_rule_occurs, 0); for( 0;
_temp1165 != 0; _temp1165=({ struct Cyc_List_List* _temp1169= _temp1165; if(
_temp1169 == 0){ _throw( Null_Exception);} _temp1169->tl;})){ struct _tuple12
_temp1173; struct _tuple11* _temp1174; struct _tuple11 _temp1176; struct Cyc_Position_Segment*
_temp1177; int _temp1179; void* _temp1181; struct _tuple12* _temp1171=( struct
_tuple12*)({ struct Cyc_List_List* _temp1170= _temp1165; if( _temp1170 == 0){
_throw( Null_Exception);} _temp1170->hd;}); _temp1173=* _temp1171; _LL1182:
_temp1181= _temp1173.f1; goto _LL1175; _LL1175: _temp1174= _temp1173.f2;
_temp1176=* _temp1174; _LL1180: _temp1179= _temp1176.f1; goto _LL1178; _LL1178:
_temp1177= _temp1176.f2; goto _LL1172; _LL1172: if( ! _temp1179){ Cyc_Tcutil_terr(
_temp1177,( struct _tagged_string)({ char* _temp1183=( char*)"redundant pattern";
struct _tagged_string _temp1184; _temp1184.curr= _temp1183; _temp1184.base=
_temp1183; _temp1184.last_plus_one= _temp1183 + 18; _temp1184;}));}}}}} struct
_tuple13{ struct Cyc_Position_Segment* f1; int f2; } ; static void Cyc_Tcpat_not_exhaust_warn(
struct _tuple13* pr){(* pr).f2= 0; Cyc_Tcutil_warn((* pr).f1,( struct
_tagged_string)({ char* _temp1185=( char*)"pattern not exhaustive"; struct
_tagged_string _temp1186; _temp1186.curr= _temp1185; _temp1186.base= _temp1185;
_temp1186.last_plus_one= _temp1185 + 23; _temp1186;}));} static void Cyc_Tcpat_dummy_fn(
int i, int j){ return;} struct _tuple14{ void* f1; int f2; } ; int Cyc_Tcpat_check_let_pat_exhaustive(
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Pat* p){ struct Cyc_List_List*
_temp1189=({ struct Cyc_List_List* _temp1187=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1187->hd=( void*)({ struct _tuple14*
_temp1188=( struct _tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp1188->f1=
Cyc_Tcpat_compile_pat( p); _temp1188->f2= 0; _temp1188;}); _temp1187->tl= 0;
_temp1187;}); goto _LL1190; _LL1190: { void* _temp1191=(( void*(*)( struct Cyc_List_List*
allmrules)) Cyc_Tcpat_match_compile)( _temp1189); goto _LL1192; _LL1192: {
struct _tuple13* _temp1194=({ struct _tuple13* _temp1193=( struct _tuple13*)
GC_malloc( sizeof( struct _tuple13)); _temp1193->f1= loc; _temp1193->f2= 1;
_temp1193;}); goto _LL1195; _LL1195:(( void(*)( void* d, void(* not_exhaust)(
struct _tuple13*), struct _tuple13* env1, void(* rhs_appears)( int, int), int
env2)) Cyc_Tcpat_check_exhaust_overlap)( _temp1191, Cyc_Tcpat_not_exhaust_warn,
_temp1194, Cyc_Tcpat_dummy_fn, 0); return(* _temp1194).f2;}}} static struct
_tuple12* Cyc_Tcpat_get_match2( struct Cyc_Absyn_Switch_clause* swc){ void* sp0=
Cyc_Tcpat_compile_pat( swc->pattern); struct _tuple11* rhs=({ struct _tuple11*
_temp1197=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp1197->f1=
0; _temp1197->f2=( swc->pattern)->loc; _temp1197;}); return({ struct _tuple12*
_temp1196=( struct _tuple12*) GC_malloc( sizeof( struct _tuple12)); _temp1196->f1=
sp0; _temp1196->f2= rhs; _temp1196;});} static void Cyc_Tcpat_not_exhaust_err2(
struct Cyc_Position_Segment* loc){;} void Cyc_Tcpat_check_catch_overlap( struct
Cyc_Position_Segment* loc, struct Cyc_List_List* swcs){ struct Cyc_List_List*
_temp1198=(( struct Cyc_List_List*(*)( struct _tuple12*(* f)( struct Cyc_Absyn_Switch_clause*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_get_match2, swcs); goto
_LL1199; _LL1199: { void* _temp1200=(( void*(*)( struct Cyc_List_List* allmrules))
Cyc_Tcpat_match_compile)( _temp1198); goto _LL1201; _LL1201:(( void(*)( void* d,
void(* not_exhaust)( struct Cyc_Position_Segment*), struct Cyc_Position_Segment*
env1, void(* rhs_appears)( int, struct _tuple11*), int env2)) Cyc_Tcpat_check_exhaust_overlap)(
_temp1200, Cyc_Tcpat_not_exhaust_err2, loc, Cyc_Tcpat_rule_occurs, 0); for( 0;
_temp1198 != 0; _temp1198=({ struct Cyc_List_List* _temp1202= _temp1198; if(
_temp1202 == 0){ _throw( Null_Exception);} _temp1202->tl;})){ struct _tuple12
_temp1206; struct _tuple11* _temp1207; struct _tuple11 _temp1209; struct Cyc_Position_Segment*
_temp1210; int _temp1212; void* _temp1214; struct _tuple12* _temp1204=( struct
_tuple12*)({ struct Cyc_List_List* _temp1203= _temp1198; if( _temp1203 == 0){
_throw( Null_Exception);} _temp1203->hd;}); _temp1206=* _temp1204; _LL1215:
_temp1214= _temp1206.f1; goto _LL1208; _LL1208: _temp1207= _temp1206.f2;
_temp1209=* _temp1207; _LL1213: _temp1212= _temp1209.f1; goto _LL1211; _LL1211:
_temp1210= _temp1209.f2; goto _LL1205; _LL1205: if( ! _temp1212){ Cyc_Tcutil_terr(
_temp1210,( struct _tagged_string)({ char* _temp1216=( char*)"redundant pattern";
struct _tagged_string _temp1217; _temp1217.curr= _temp1216; _temp1217.base=
_temp1216; _temp1217.last_plus_one= _temp1216 + 18; _temp1217;}));}}}}