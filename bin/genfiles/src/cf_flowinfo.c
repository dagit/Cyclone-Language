#include "cyc_include.h"

 typedef int Cyc_ptrdiff_t; typedef unsigned int Cyc_size_t; typedef int Cyc_wchar_t;
typedef unsigned int Cyc_wint_t; typedef char Cyc_u_char; typedef unsigned short
Cyc_u_short; typedef unsigned int Cyc_u_int; typedef unsigned int Cyc_u_long;
typedef unsigned short Cyc_ushort; typedef unsigned int Cyc_uint; typedef
unsigned int Cyc_clock_t; typedef int Cyc_time_t; struct Cyc_timespec{ int
tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec it_interval;
struct Cyc_timespec it_value; } ; typedef int Cyc_daddr_t; typedef char* Cyc_caddr_t;
typedef unsigned short Cyc_ino_t; typedef short Cyc_dev_t; typedef int Cyc_off_t;
typedef unsigned short Cyc_uid_t; typedef unsigned short Cyc_gid_t; typedef int
Cyc_pid_t; typedef int Cyc_key_t; typedef int Cyc_ssize_t; typedef unsigned int
Cyc_mode_t; typedef unsigned short Cyc_nlink_t; typedef int Cyc_fd_mask; struct
Cyc__types_fd_set{ int fds_bits[ 8u]; } ; typedef struct Cyc__types_fd_set Cyc__types_fd_set;
typedef char* Cyc_Cstring; typedef struct _tagged_string Cyc_string; typedef
struct _tagged_string Cyc_string_t; typedef struct _tagged_string* Cyc_stringptr;
typedef int Cyc_bool; extern void* exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; typedef struct Cyc_Core_Opt* Cyc_Core_opt_t; extern int Cyc_Core_intcmp(
int, int); extern int Cyc_Core_ptrcmp( void**, void**); extern char Cyc_Core_InvalidArg[
15u]; struct Cyc_Core_InvalidArg_struct{ char* tag; struct _tagged_string f1; }
; extern char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{ char* tag;
struct _tagged_string f1; } ; extern char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Core_Not_found[ 14u];
extern char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{ char*
tag; struct _tagged_string f1; } ; extern char* string_to_Cstring( struct
_tagged_string); extern char* underlying_Cstring( struct _tagged_string); extern
struct _tagged_string Cstring_to_string( char*); extern int system( char*);
struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List*
Cyc_List_glist_t; typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct
Cyc_List_List* Cyc_List_List_t; extern char Cyc_List_List_empty[ 15u]; extern
struct Cyc_List_List* Cyc_List_tl( struct Cyc_List_List* x); extern char Cyc_List_List_mismatch[
18u]; extern char Cyc_List_Nth[ 8u]; extern int Cyc_List_mem( int(* compare)(
void*, void*), struct Cyc_List_List* l, void* x); extern int Cyc_List_list_cmp(
int(* cmp)( void*, void*), struct Cyc_List_List* l1, struct Cyc_List_List* l2);
struct Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct
Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t; extern
struct Cyc_Set_Set* Cyc_Set_empty( int(* comp)( void*, void*)); extern struct
Cyc_Set_Set* Cyc_Set_insert( struct Cyc_Set_Set* s, void* elt); extern struct
Cyc_Set_Set* Cyc_Set_delete( struct Cyc_Set_Set* s, void* elt); extern int Cyc_Set_is_empty(
struct Cyc_Set_Set* s); extern char Cyc_Set_Absent[ 11u]; extern void* Cyc_Set_choose(
struct Cyc_Set_Set* s); struct Cyc_Dict_Dict; typedef struct Cyc_Dict_Dict* Cyc_Dict_hdict_t;
typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t; extern char Cyc_Dict_Present[ 12u];
extern char Cyc_Dict_Absent[ 11u]; extern struct Cyc_Dict_Dict* Cyc_Dict_empty(
int(* comp)( void*, void*)); extern struct Cyc_Dict_Dict* Cyc_Dict_insert(
struct Cyc_Dict_Dict* d, void* key, void* data); extern void* Cyc_Dict_lookup(
struct Cyc_Dict_Dict* d, void* key); extern void Cyc_Dict_iter_c( void(* f)(
void*, void*, void*), void* env, struct Cyc_Dict_Dict* d); extern struct Cyc_Dict_Dict*
Cyc_Dict_map_c( void*(* f)( void*, void*), void* env, struct Cyc_Dict_Dict* d);
extern struct Cyc_Dict_Dict* Cyc_Dict_intersect_c( void*(* f)( void*, void*,
void*), void* env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2); extern
int Cyc_Dict_forall_c( int(* f)( void*, void*, void*), void* env, struct Cyc_Dict_Dict*
d); extern int Cyc_Dict_forall_intersect( int(* f)( void*, void*, void*), struct
Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2); struct Cyc_Stdio___sFILE; typedef
struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; typedef int Cyc_Stdio_fpos_t; extern
char Cyc_Stdio_FileCloseError[ 19u]; extern char Cyc_Stdio_FileOpenError[ 18u];
struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1; } ;
extern int Cyc_String_strcmp( struct _tagged_string s1, struct _tagged_string s2);
extern int Cyc_String_zstrptrcmp( struct _tagged_string*, struct _tagged_string*);
struct Cyc_Lineno_Pos{ struct _tagged_string logical_file; struct _tagged_string
line; int line_no; int col; } ; typedef struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t;
extern char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment; typedef struct
Cyc_Position_Segment* Cyc_Position_seg_t; static const int Cyc_Position_Lex= 0;
static const int Cyc_Position_Parse= 1; static const int Cyc_Position_Elab= 2;
typedef void* Cyc_Position_error_kind_t; struct Cyc_Position_Error{ struct
_tagged_string source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_string desc; } ; typedef struct Cyc_Position_Error* Cyc_Position_error_t;
extern char Cyc_Position_Nocontext[ 14u]; typedef struct _tagged_string* Cyc_Absyn_field_name_t;
typedef struct _tagged_string* Cyc_Absyn_var_t; typedef struct _tagged_string*
Cyc_Absyn_tvarname_t; typedef void* Cyc_Absyn_nmspace_t; struct _tuple0{ void*
f1; struct _tagged_string* f2; } ; typedef struct _tuple0* Cyc_Absyn_qvar_t;
typedef struct _tuple0* Cyc_Absyn_qvar_opt_t; typedef struct _tuple0* Cyc_Absyn_typedef_name_t;
typedef struct _tuple0* Cyc_Absyn_typedef_name_opt_t; struct Cyc_Absyn_Tvar;
struct Cyc_Absyn_Tqual; struct Cyc_Absyn_Conref; struct Cyc_Absyn_PtrInfo;
struct Cyc_Absyn_FnInfo; struct Cyc_Absyn_TunionInfo; struct Cyc_Absyn_TunionFieldInfo;
struct Cyc_Absyn_Exp; struct Cyc_Absyn_Stmt; struct Cyc_Absyn_Pat; struct Cyc_Absyn_Switch_clause;
struct Cyc_Absyn_Fndecl; struct Cyc_Absyn_Structdecl; struct Cyc_Absyn_Uniondecl;
struct Cyc_Absyn_Tuniondecl; struct Cyc_Absyn_Tunionfield; struct Cyc_Absyn_Enumfield;
struct Cyc_Absyn_Enumdecl; struct Cyc_Absyn_Typedefdecl; struct Cyc_Absyn_Vardecl;
struct Cyc_Absyn_Decl; struct Cyc_Absyn_Structfield; typedef void* Cyc_Absyn_scope_t;
typedef struct Cyc_Absyn_Tqual Cyc_Absyn_tqual_t; typedef void* Cyc_Absyn_size_of_t;
typedef void* Cyc_Absyn_kind_t; typedef struct Cyc_Absyn_Tvar* Cyc_Absyn_tvar_t;
typedef void* Cyc_Absyn_sign_t; typedef struct Cyc_Absyn_Conref* Cyc_Absyn_conref_t;
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
attributes; } ; struct Cyc_Absyn_UnknownTunionInfo{ struct _tuple0* name; int
is_xtunion; } ; static const int Cyc_Absyn_UnknownTunion= 0; struct Cyc_Absyn_UnknownTunion_struct{
int tag; struct Cyc_Absyn_UnknownTunionInfo f1; } ; static const int Cyc_Absyn_KnownTunion=
1; struct Cyc_Absyn_KnownTunion_struct{ int tag; struct Cyc_Absyn_Tuniondecl* f1;
} ; struct Cyc_Absyn_TunionInfo{ void* tunion_info; struct Cyc_List_List* targs;
void* rgn; } ; struct Cyc_Absyn_UnknownTunionFieldInfo{ struct _tuple0*
tunion_name; struct _tuple0* field_name; int is_xtunion; } ; static const int
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
struct Cyc_Absyn_StructType_struct{ int tag; struct _tuple0* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Structdecl** f3; } ; static const int Cyc_Absyn_UnionType=
10; struct Cyc_Absyn_UnionType_struct{ int tag; struct _tuple0* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Uniondecl** f3; } ; static const int Cyc_Absyn_AnonStructType=
11; struct Cyc_Absyn_AnonStructType_struct{ int tag; struct Cyc_List_List* f1; }
; static const int Cyc_Absyn_AnonUnionType= 12; struct Cyc_Absyn_AnonUnionType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_EnumType= 13;
struct Cyc_Absyn_EnumType_struct{ int tag; struct _tuple0* f1; struct Cyc_Absyn_Enumdecl*
f2; } ; static const int Cyc_Absyn_RgnHandleType= 14; struct Cyc_Absyn_RgnHandleType_struct{
int tag; void* f1; } ; static const int Cyc_Absyn_TypedefType= 15; struct Cyc_Absyn_TypedefType_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3;
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
int tag; struct _tuple0* f1; void* f2; } ; static const int Cyc_Absyn_UnknownId_e=
2; struct Cyc_Absyn_UnknownId_e_struct{ int tag; struct _tuple0* f1; } ; static
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
f1; } ; static const int Cyc_Absyn_CompoundLit_e= 23; struct _tuple1{ struct Cyc_Core_Opt*
f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; struct Cyc_Absyn_CompoundLit_e_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Array_e=
24; struct Cyc_Absyn_Array_e_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_Comprehension_e= 25; struct Cyc_Absyn_Comprehension_e_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp*
f3; } ; static const int Cyc_Absyn_Struct_e= 26; struct Cyc_Absyn_Struct_e_struct{
int tag; struct _tuple0* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List* f3;
struct Cyc_Absyn_Structdecl* f4; } ; static const int Cyc_Absyn_AnonStruct_e= 27;
struct Cyc_Absyn_AnonStruct_e_struct{ int tag; void* f1; struct Cyc_List_List*
f2; } ; static const int Cyc_Absyn_Tunion_e= 28; struct Cyc_Absyn_Tunion_e_struct{
int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List*
f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield* f5; } ;
static const int Cyc_Absyn_Enum_e= 29; struct Cyc_Absyn_Enum_e_struct{ int tag;
struct _tuple0* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
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
f3; } ; static const int Cyc_Absyn_While_s= 4; struct _tuple2{ struct Cyc_Absyn_Exp*
f1; struct Cyc_Absyn_Stmt* f2; } ; struct Cyc_Absyn_While_s_struct{ int tag;
struct _tuple2 f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Break_s=
5; struct Cyc_Absyn_Break_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ;
static const int Cyc_Absyn_Continue_s= 6; struct Cyc_Absyn_Continue_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Absyn_Goto_s= 7;
struct Cyc_Absyn_Goto_s_struct{ int tag; struct _tagged_string* f1; struct Cyc_Absyn_Stmt*
f2; } ; static const int Cyc_Absyn_For_s= 8; struct Cyc_Absyn_For_s_struct{ int
tag; struct Cyc_Absyn_Exp* f1; struct _tuple2 f2; struct _tuple2 f3; struct Cyc_Absyn_Stmt*
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
f1; struct _tuple2 f2; } ; static const int Cyc_Absyn_TryCatch_s= 16; struct Cyc_Absyn_TryCatch_s_struct{
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
struct Cyc_Absyn_UnknownId_p_struct{ int tag; struct _tuple0* f1; } ; static
const int Cyc_Absyn_UnknownCall_p= 11; struct Cyc_Absyn_UnknownCall_p_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
} ; static const int Cyc_Absyn_UnknownFields_p= 12; struct Cyc_Absyn_UnknownFields_p_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
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
f1; } ; struct Cyc_Absyn_Vardecl{ void* sc; struct _tuple0* name; struct Cyc_Absyn_Tqual
tq; void* type; struct Cyc_Absyn_Exp* initializer; struct Cyc_Core_Opt* rgn;
struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Fndecl{ void* sc; int
is_inline; struct _tuple0* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
effect; void* ret_type; struct Cyc_List_List* args; int varargs; struct Cyc_Absyn_Stmt*
body; struct Cyc_Core_Opt* cached_typ; struct Cyc_Core_Opt* param_vardecls;
struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Structfield{ struct
_tagged_string* name; struct Cyc_Absyn_Tqual tq; void* type; struct Cyc_Core_Opt*
width; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Structdecl{ void*
sc; struct Cyc_Core_Opt* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
fields; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Uniondecl{ void*
sc; struct Cyc_Core_Opt* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
fields; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Tunionfield{
struct _tuple0* name; struct Cyc_List_List* tvs; struct Cyc_List_List* typs;
struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_Tuniondecl{ void* sc;
struct _tuple0* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt* fields;
int is_xtunion; } ; struct Cyc_Absyn_Enumfield{ struct _tuple0* name; struct Cyc_Absyn_Exp*
tag; struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_Enumdecl{ void* sc;
struct _tuple0* name; struct Cyc_Core_Opt* fields; } ; struct Cyc_Absyn_Typedefdecl{
struct _tuple0* name; struct Cyc_List_List* tvs; void* defn; } ; static const
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
_tuple0* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d=
10; struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ;
struct Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc; } ; static
const int Cyc_Absyn_ArrayElement= 0; struct Cyc_Absyn_ArrayElement_struct{ int
tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_FieldName= 1;
struct Cyc_Absyn_FieldName_struct{ int tag; struct _tagged_string* f1; } ;
extern char Cyc_Absyn_EmptyAnnot[ 15u]; static const int Cyc_Tcenv_VarRes= 0;
struct Cyc_Tcenv_VarRes_struct{ int tag; void* f1; } ; static const int Cyc_Tcenv_StructRes=
1; struct Cyc_Tcenv_StructRes_struct{ int tag; struct Cyc_Absyn_Structdecl* f1;
} ; static const int Cyc_Tcenv_TunionRes= 2; struct Cyc_Tcenv_TunionRes_struct{
int tag; struct Cyc_Absyn_Tuniondecl* f1; struct Cyc_Absyn_Tunionfield* f2; } ;
static const int Cyc_Tcenv_EnumRes= 3; struct Cyc_Tcenv_EnumRes_struct{ int tag;
struct Cyc_Absyn_Enumdecl* f1; struct Cyc_Absyn_Enumfield* f2; } ; typedef void*
Cyc_Tcenv_resolved_t; struct Cyc_Tcenv_Genv{ struct Cyc_Set_Set* namespaces;
struct Cyc_Dict_Dict* structdecls; struct Cyc_Dict_Dict* uniondecls; struct Cyc_Dict_Dict*
tuniondecls; struct Cyc_Dict_Dict* enumdecls; struct Cyc_Dict_Dict* typedefs;
struct Cyc_Dict_Dict* ordinaries; struct Cyc_List_List* availables; } ; typedef
struct Cyc_Tcenv_Genv* Cyc_Tcenv_genv_t; struct Cyc_Tcenv_Fenv; typedef struct
Cyc_Tcenv_Fenv* Cyc_Tcenv_fenv_t; static const int Cyc_Tcenv_NotLoop_j= 0;
static const int Cyc_Tcenv_CaseEnd_j= 1; static const int Cyc_Tcenv_FnEnd_j= 2;
static const int Cyc_Tcenv_Stmt_j= 0; struct Cyc_Tcenv_Stmt_j_struct{ int tag;
struct Cyc_Absyn_Stmt* f1; } ; typedef void* Cyc_Tcenv_jumpee_t; static const
int Cyc_Tcenv_Outermost= 0; struct Cyc_Tcenv_Outermost_struct{ int tag; void* f1;
} ; static const int Cyc_Tcenv_Frame= 1; struct Cyc_Tcenv_Frame_struct{ int tag;
void* f1; void* f2; } ; static const int Cyc_Tcenv_Hidden= 2; struct Cyc_Tcenv_Hidden_struct{
int tag; void* f1; void* f2; } ; typedef void* Cyc_Tcenv_frames_t; struct Cyc_Tcenv_Tenv{
struct Cyc_List_List* ns; struct Cyc_Dict_Dict* ae; struct Cyc_Core_Opt* le; } ;
typedef struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tenv_t; extern char Cyc_Tcutil_TypeErr[
12u]; extern void* Cyc_Tcutil_compress( void* t); struct Cyc_CfFlowInfo_Place;
typedef void* Cyc_CfFlowInfo_local_root_t; typedef void* Cyc_CfFlowInfo_field_t;
typedef struct Cyc_CfFlowInfo_Place* Cyc_CfFlowInfo_place_t; typedef void* Cyc_CfFlowInfo_escaped_t;
typedef void* Cyc_CfFlowInfo_init_level_t; typedef void* Cyc_CfFlowInfo_init_state_t;
typedef void* Cyc_CfFlowInfo_path_info_t; typedef struct Cyc_Dict_Dict* Cyc_CfFlowInfo_pinfo_dict_t;
typedef void* Cyc_CfFlowInfo_flow_info_t; static const int Cyc_CfFlowInfo_VarRoot=
0; struct Cyc_CfFlowInfo_VarRoot_struct{ int tag; struct Cyc_Absyn_Vardecl* f1;
} ; static const int Cyc_CfFlowInfo_MallocPt= 1; struct Cyc_CfFlowInfo_MallocPt_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_CfFlowInfo_StructF=
0; struct Cyc_CfFlowInfo_StructF_struct{ int tag; struct _tagged_string* f1; } ;
static const int Cyc_CfFlowInfo_TupleF= 1; struct Cyc_CfFlowInfo_TupleF_struct{
int tag; int f1; } ; struct Cyc_CfFlowInfo_Place{ void* root; struct Cyc_List_List*
fields; } ; static const int Cyc_CfFlowInfo_Esc= 0; static const int Cyc_CfFlowInfo_Unesc=
1; static const int Cyc_CfFlowInfo_NoneIL= 0; static const int Cyc_CfFlowInfo_ThisIL=
1; static const int Cyc_CfFlowInfo_AllIL= 2; static const int Cyc_CfFlowInfo_UnknownIS=
0; struct Cyc_CfFlowInfo_UnknownIS_struct{ int tag; void* f1; void* f2; } ;
static const int Cyc_CfFlowInfo_MustPointTo= 1; struct Cyc_CfFlowInfo_MustPointTo_struct{
int tag; struct Cyc_CfFlowInfo_Place* f1; } ; static const int Cyc_CfFlowInfo_LeafPI=
0; struct Cyc_CfFlowInfo_LeafPI_struct{ int tag; void* f1; } ; static const int
Cyc_CfFlowInfo_TuplePI= 1; struct Cyc_CfFlowInfo_TuplePI_struct{ int tag; struct
Cyc_Dict_Dict* f1; } ; static const int Cyc_CfFlowInfo_StructPI= 2; struct Cyc_CfFlowInfo_StructPI_struct{
int tag; struct Cyc_Dict_Dict* f1; } ; static const int Cyc_CfFlowInfo_BottomFL=
0; static const int Cyc_CfFlowInfo_InitsFL= 0; struct Cyc_CfFlowInfo_InitsFL_struct{
int tag; struct Cyc_Dict_Dict* f1; } ; extern void* Cyc_CfFlowInfo_mkLeafPI(
void* esc, void* il); extern int Cyc_CfFlowInfo_local_root_cmp( void*, void*);
extern int Cyc_CfFlowInfo_field_cmp( void*, void*); extern int Cyc_CfFlowInfo_place_cmp(
struct Cyc_CfFlowInfo_Place*, struct Cyc_CfFlowInfo_Place*); extern void* Cyc_CfFlowInfo_typ_to_unesc_none_pinfo(
void* t); extern void* Cyc_CfFlowInfo_lookup_place( struct Cyc_Dict_Dict* d,
struct Cyc_CfFlowInfo_Place* place); extern struct Cyc_Dict_Dict* Cyc_CfFlowInfo_insert_place(
struct Cyc_Dict_Dict* d, struct Cyc_CfFlowInfo_Place* place, void* pinfo);
extern int Cyc_CfFlowInfo_isAllInit( struct Cyc_Dict_Dict* pinfo_dict, void*
pinfo); extern struct Cyc_Dict_Dict* Cyc_CfFlowInfo_escape_pointsto( void* pinfo,
struct Cyc_Dict_Dict* d); extern void* Cyc_CfFlowInfo_assign_unknown_dict( void*,
void*); extern void* Cyc_CfFlowInfo_join_flow( void*, void*); extern int Cyc_CfFlowInfo_flow_lessthan_approx(
void* f1, void* f2); static struct Cyc_CfFlowInfo_UnknownIS_struct Cyc_CfFlowInfo_unknownis_unes_noneil={
0u,( void*)(( void*) 1u),( void*)(( void*) 0u)}; static struct Cyc_CfFlowInfo_UnknownIS_struct
Cyc_CfFlowInfo_unknownis_unes_thisil={ 0u,( void*)(( void*) 1u),( void*)(( void*)
1u)}; static struct Cyc_CfFlowInfo_UnknownIS_struct Cyc_CfFlowInfo_unknownis_unes_allil={
0u,( void*)(( void*) 1u),( void*)(( void*) 2u)}; static struct Cyc_CfFlowInfo_UnknownIS_struct
Cyc_CfFlowInfo_unknownis_esc_noneil={ 0u,( void*)(( void*) 0u),( void*)(( void*)
0u)}; static struct Cyc_CfFlowInfo_UnknownIS_struct Cyc_CfFlowInfo_unknownis_esc_thisil={
0u,( void*)(( void*) 0u),( void*)(( void*) 1u)}; static struct Cyc_CfFlowInfo_UnknownIS_struct
Cyc_CfFlowInfo_unknownis_esc_allil={ 0u,( void*)(( void*) 0u),( void*)(( void*)
2u)}; static struct Cyc_CfFlowInfo_LeafPI_struct Cyc_CfFlowInfo_leaf_unesc_none_v={
0u,( void*)(( void*)& Cyc_CfFlowInfo_unknownis_unes_noneil)}; static struct Cyc_CfFlowInfo_LeafPI_struct
Cyc_CfFlowInfo_leaf_unesc_this_v={ 0u,( void*)(( void*)& Cyc_CfFlowInfo_unknownis_unes_thisil)};
static struct Cyc_CfFlowInfo_LeafPI_struct Cyc_CfFlowInfo_leaf_unesc_all_v={ 0u,(
void*)(( void*)& Cyc_CfFlowInfo_unknownis_unes_allil)}; static struct Cyc_CfFlowInfo_LeafPI_struct
Cyc_CfFlowInfo_leaf_esc_none_v={ 0u,( void*)(( void*)& Cyc_CfFlowInfo_unknownis_esc_noneil)};
static struct Cyc_CfFlowInfo_LeafPI_struct Cyc_CfFlowInfo_leaf_esc_this_v={ 0u,(
void*)(( void*)& Cyc_CfFlowInfo_unknownis_esc_thisil)}; static struct Cyc_CfFlowInfo_LeafPI_struct
Cyc_CfFlowInfo_leaf_esc_all_v={ 0u,( void*)(( void*)& Cyc_CfFlowInfo_unknownis_esc_allil)};
void* Cyc_CfFlowInfo_leaf_unesc_none=( void*)& Cyc_CfFlowInfo_leaf_unesc_none_v;
void* Cyc_CfFlowInfo_leaf_unesc_this=( void*)& Cyc_CfFlowInfo_leaf_unesc_this_v;
void* Cyc_CfFlowInfo_leaf_unesc_all=( void*)& Cyc_CfFlowInfo_leaf_unesc_all_v;
void* Cyc_CfFlowInfo_leaf_esc_none=( void*)& Cyc_CfFlowInfo_leaf_esc_none_v;
void* Cyc_CfFlowInfo_leaf_esc_this=( void*)& Cyc_CfFlowInfo_leaf_esc_this_v;
void* Cyc_CfFlowInfo_leaf_esc_all=( void*)& Cyc_CfFlowInfo_leaf_esc_all_v;
struct _tuple3{ void* f1; void* f2; } ; void* Cyc_CfFlowInfo_mkLeafPI( void* esc,
void* il){ struct _tuple3 _temp13=({ struct _tuple3 _temp12; _temp12.f1= esc;
_temp12.f2= il; _temp12;}); void* _temp27; void* _temp29; void* _temp31; void*
_temp33; void* _temp35; void* _temp37; void* _temp39; void* _temp41; void*
_temp43; void* _temp45; void* _temp47; void* _temp49; _LL15: _LL30: _temp29=
_temp13.f1; if( _temp29 ==( void*) Cyc_CfFlowInfo_Unesc){ goto _LL28;} else{
goto _LL17;} _LL28: _temp27= _temp13.f2; if( _temp27 ==( void*) Cyc_CfFlowInfo_NoneIL){
goto _LL16;} else{ goto _LL17;} _LL17: _LL34: _temp33= _temp13.f1; if( _temp33
==( void*) Cyc_CfFlowInfo_Unesc){ goto _LL32;} else{ goto _LL19;} _LL32: _temp31=
_temp13.f2; if( _temp31 ==( void*) Cyc_CfFlowInfo_ThisIL){ goto _LL18;} else{
goto _LL19;} _LL19: _LL38: _temp37= _temp13.f1; if( _temp37 ==( void*) Cyc_CfFlowInfo_Unesc){
goto _LL36;} else{ goto _LL21;} _LL36: _temp35= _temp13.f2; if( _temp35 ==( void*)
Cyc_CfFlowInfo_AllIL){ goto _LL20;} else{ goto _LL21;} _LL21: _LL42: _temp41=
_temp13.f1; if( _temp41 ==( void*) Cyc_CfFlowInfo_Esc){ goto _LL40;} else{ goto
_LL23;} _LL40: _temp39= _temp13.f2; if( _temp39 ==( void*) Cyc_CfFlowInfo_NoneIL){
goto _LL22;} else{ goto _LL23;} _LL23: _LL46: _temp45= _temp13.f1; if( _temp45
==( void*) Cyc_CfFlowInfo_Esc){ goto _LL44;} else{ goto _LL25;} _LL44: _temp43=
_temp13.f2; if( _temp43 ==( void*) Cyc_CfFlowInfo_ThisIL){ goto _LL24;} else{
goto _LL25;} _LL25: _LL50: _temp49= _temp13.f1; if( _temp49 ==( void*) Cyc_CfFlowInfo_Esc){
goto _LL48;} else{ goto _LL14;} _LL48: _temp47= _temp13.f2; if( _temp47 ==( void*)
Cyc_CfFlowInfo_AllIL){ goto _LL26;} else{ goto _LL14;} _LL16: return Cyc_CfFlowInfo_leaf_unesc_none;
_LL18: return Cyc_CfFlowInfo_leaf_unesc_this; _LL20: return Cyc_CfFlowInfo_leaf_unesc_all;
_LL22: return Cyc_CfFlowInfo_leaf_esc_none; _LL24: return Cyc_CfFlowInfo_leaf_esc_this;
_LL26: return Cyc_CfFlowInfo_leaf_esc_all; _LL14:;} int Cyc_CfFlowInfo_local_root_cmp(
void* r1, void* r2){ if( r1 == r2){ return 0;}{ struct _tuple3 _temp52=({ struct
_tuple3 _temp51; _temp51.f1= r1; _temp51.f2= r2; _temp51;}); void* _temp62;
struct Cyc_Absyn_Exp* _temp64; void* _temp66; struct Cyc_Absyn_Vardecl* _temp68;
void* _temp70; struct Cyc_Absyn_Vardecl* _temp72; void* _temp74; struct Cyc_Absyn_Exp*
_temp76; void* _temp78; struct Cyc_Absyn_Vardecl* _temp80; void* _temp82; struct
Cyc_Absyn_Vardecl* _temp84; void* _temp86; struct Cyc_Absyn_Exp* _temp88; void*
_temp90; struct Cyc_Absyn_Exp* _temp92; _LL54: _LL67: _temp66= _temp52.f1; if(*((
int*) _temp66) == Cyc_CfFlowInfo_VarRoot){ _LL69: _temp68=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_CfFlowInfo_VarRoot_struct*) _temp66)->f1; goto _LL63;} else{ goto
_LL56;} _LL63: _temp62= _temp52.f2; if(*(( int*) _temp62) == Cyc_CfFlowInfo_MallocPt){
_LL65: _temp64=( struct Cyc_Absyn_Exp*)(( struct Cyc_CfFlowInfo_MallocPt_struct*)
_temp62)->f1; goto _LL55;} else{ goto _LL56;} _LL56: _LL75: _temp74= _temp52.f1;
if(*(( int*) _temp74) == Cyc_CfFlowInfo_MallocPt){ _LL77: _temp76=( struct Cyc_Absyn_Exp*)((
struct Cyc_CfFlowInfo_MallocPt_struct*) _temp74)->f1; goto _LL71;} else{ goto
_LL58;} _LL71: _temp70= _temp52.f2; if(*(( int*) _temp70) == Cyc_CfFlowInfo_VarRoot){
_LL73: _temp72=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_CfFlowInfo_VarRoot_struct*)
_temp70)->f1; goto _LL57;} else{ goto _LL58;} _LL58: _LL83: _temp82= _temp52.f1;
if(*(( int*) _temp82) == Cyc_CfFlowInfo_VarRoot){ _LL85: _temp84=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_CfFlowInfo_VarRoot_struct*) _temp82)->f1; goto _LL79;} else{ goto
_LL60;} _LL79: _temp78= _temp52.f2; if(*(( int*) _temp78) == Cyc_CfFlowInfo_VarRoot){
_LL81: _temp80=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_CfFlowInfo_VarRoot_struct*)
_temp78)->f1; goto _LL59;} else{ goto _LL60;} _LL60: _LL91: _temp90= _temp52.f1;
if(*(( int*) _temp90) == Cyc_CfFlowInfo_MallocPt){ _LL93: _temp92=( struct Cyc_Absyn_Exp*)((
struct Cyc_CfFlowInfo_MallocPt_struct*) _temp90)->f1; goto _LL87;} else{ goto
_LL53;} _LL87: _temp86= _temp52.f2; if(*(( int*) _temp86) == Cyc_CfFlowInfo_MallocPt){
_LL89: _temp88=( struct Cyc_Absyn_Exp*)(( struct Cyc_CfFlowInfo_MallocPt_struct*)
_temp86)->f1; goto _LL61;} else{ goto _LL53;} _LL55: return 1; _LL57: return - 1;
_LL59: return(( int(*)( struct Cyc_Absyn_Vardecl*, struct Cyc_Absyn_Vardecl*))
Cyc_Core_ptrcmp)( _temp84, _temp80); _LL61: return(( int(*)( struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*)) Cyc_Core_ptrcmp)( _temp92, _temp88); _LL53:;}} int Cyc_CfFlowInfo_field_cmp(
void* f1, void* f2){ if( f1 == f2){ return 0;}{ struct _tuple3 _temp95=({ struct
_tuple3 _temp94; _temp94.f1= f1; _temp94.f2= f2; _temp94;}); void* _temp105; int
_temp107; void* _temp109; struct _tagged_string* _temp111; void* _temp113;
struct _tagged_string* _temp115; void* _temp117; int _temp119; void* _temp121;
struct _tagged_string* _temp123; void* _temp125; struct _tagged_string* _temp127;
void* _temp129; int _temp131; void* _temp133; int _temp135; _LL97: _LL110:
_temp109= _temp95.f1; if(*(( int*) _temp109) == Cyc_CfFlowInfo_StructF){ _LL112:
_temp111=( struct _tagged_string*)(( struct Cyc_CfFlowInfo_StructF_struct*)
_temp109)->f1; goto _LL106;} else{ goto _LL99;} _LL106: _temp105= _temp95.f2;
if(*(( int*) _temp105) == Cyc_CfFlowInfo_TupleF){ _LL108: _temp107=( int)((
struct Cyc_CfFlowInfo_TupleF_struct*) _temp105)->f1; goto _LL98;} else{ goto
_LL99;} _LL99: _LL118: _temp117= _temp95.f1; if(*(( int*) _temp117) == Cyc_CfFlowInfo_TupleF){
_LL120: _temp119=( int)(( struct Cyc_CfFlowInfo_TupleF_struct*) _temp117)->f1;
goto _LL114;} else{ goto _LL101;} _LL114: _temp113= _temp95.f2; if(*(( int*)
_temp113) == Cyc_CfFlowInfo_StructF){ _LL116: _temp115=( struct _tagged_string*)((
struct Cyc_CfFlowInfo_StructF_struct*) _temp113)->f1; goto _LL100;} else{ goto
_LL101;} _LL101: _LL126: _temp125= _temp95.f1; if(*(( int*) _temp125) == Cyc_CfFlowInfo_StructF){
_LL128: _temp127=( struct _tagged_string*)(( struct Cyc_CfFlowInfo_StructF_struct*)
_temp125)->f1; goto _LL122;} else{ goto _LL103;} _LL122: _temp121= _temp95.f2;
if(*(( int*) _temp121) == Cyc_CfFlowInfo_StructF){ _LL124: _temp123=( struct
_tagged_string*)(( struct Cyc_CfFlowInfo_StructF_struct*) _temp121)->f1; goto
_LL102;} else{ goto _LL103;} _LL103: _LL134: _temp133= _temp95.f1; if(*(( int*)
_temp133) == Cyc_CfFlowInfo_TupleF){ _LL136: _temp135=( int)(( struct Cyc_CfFlowInfo_TupleF_struct*)
_temp133)->f1; goto _LL130;} else{ goto _LL96;} _LL130: _temp129= _temp95.f2;
if(*(( int*) _temp129) == Cyc_CfFlowInfo_TupleF){ _LL132: _temp131=( int)((
struct Cyc_CfFlowInfo_TupleF_struct*) _temp129)->f1; goto _LL104;} else{ goto
_LL96;} _LL98: return 1; _LL100: return - 1; _LL102: return(( int(*)( struct
_tagged_string*, struct _tagged_string*)) Cyc_Core_ptrcmp)( _temp127, _temp123);
_LL104: return _temp135 - _temp131; _LL96:;}} int Cyc_CfFlowInfo_place_cmp(
struct Cyc_CfFlowInfo_Place* p1, struct Cyc_CfFlowInfo_Place* p2){ if( p1 == p2){
return 0;}{ int i= Cyc_CfFlowInfo_local_root_cmp(( void*) p1->root,( void*) p2->root);
if( i != 0){ return i;} return(( int(*)( int(* cmp)( void*, void*), struct Cyc_List_List*
l1, struct Cyc_List_List* l2)) Cyc_List_list_cmp)( Cyc_CfFlowInfo_field_cmp, p1->fields,
p2->fields);}} struct _tuple4{ struct Cyc_Absyn_Tqual f1; void* f2; } ; void*
Cyc_CfFlowInfo_typ_to_unesc_none_pinfo( void* t){ void* _temp137= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_TunionFieldInfo _temp149; struct Cyc_List_List* _temp151;
void* _temp153; struct Cyc_Absyn_Tunionfield* _temp155; struct Cyc_Absyn_Tuniondecl*
_temp157; struct Cyc_List_List* _temp159; struct Cyc_List_List* _temp161; struct
Cyc_Absyn_Structdecl** _temp163; struct Cyc_Absyn_Structdecl* _temp165; struct
Cyc_Absyn_Structdecl _temp166; struct Cyc_List_List* _temp167; struct Cyc_Core_Opt*
_temp169; struct Cyc_Core_Opt _temp171; struct Cyc_List_List* _temp172; struct
Cyc_List_List* _temp174; struct Cyc_Core_Opt* _temp176; void* _temp178; struct
Cyc_List_List* _temp180; struct _tuple0* _temp182; _LL139: if(( unsigned int)
_temp137 > 4u?*(( int*) _temp137) == Cyc_Absyn_TunionFieldType: 0){ _LL150:
_temp149=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp137)->f1; _LL154: _temp153=( void*) _temp149.field_info; if(*(( int*)
_temp153) == Cyc_Absyn_KnownTunionfield){ _LL158: _temp157=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp153)->f1; goto _LL156; _LL156:
_temp155=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp153)->f2; goto _LL152;} else{ goto _LL141;} _LL152: _temp151=( struct Cyc_List_List*)
_temp149.targs; goto _LL140;} else{ goto _LL141;} _LL141: if(( unsigned int)
_temp137 > 4u?*(( int*) _temp137) == Cyc_Absyn_TupleType: 0){ _LL160: _temp159=(
struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp137)->f1; goto
_LL142;} else{ goto _LL143;} _LL143: if(( unsigned int) _temp137 > 4u?*(( int*)
_temp137) == Cyc_Absyn_AnonStructType: 0){ _LL162: _temp161=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonStructType_struct*) _temp137)->f1; goto _LL144;} else{ goto
_LL145;} _LL145: if(( unsigned int) _temp137 > 4u?*(( int*) _temp137) == Cyc_Absyn_StructType:
0){ _LL183: _temp182=( struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*)
_temp137)->f1; goto _LL181; _LL181: _temp180=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp137)->f2; goto _LL164; _LL164: _temp163=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp137)->f3;
if( _temp163 == 0){ goto _LL147;} else{ _temp165=* _temp163; _temp166=* _temp165;
_LL179: _temp178=( void*) _temp166.sc; goto _LL177; _LL177: _temp176=( struct
Cyc_Core_Opt*) _temp166.name; goto _LL175; _LL175: _temp174=( struct Cyc_List_List*)
_temp166.tvs; goto _LL170; _LL170: _temp169=( struct Cyc_Core_Opt*) _temp166.fields;
if( _temp169 == 0){ goto _LL147;} else{ _temp171=* _temp169; _LL173: _temp172=(
struct Cyc_List_List*) _temp171.v; goto _LL168;} _LL168: _temp167=( struct Cyc_List_List*)
_temp166.attributes; goto _LL146;}} else{ goto _LL147;} _LL147: goto _LL148;
_LL140: if( _temp155->typs == 0){ return Cyc_CfFlowInfo_leaf_unesc_none;}
_temp159= _temp155->typs; goto _LL142; _LL142: { struct Cyc_Dict_Dict* d=((
struct Cyc_Dict_Dict*(*)( int(* comp)( int, int))) Cyc_Dict_empty)( Cyc_Core_intcmp);{
int i= 0; for( 0; _temp159 != 0; _temp159=({ struct Cyc_List_List* _temp184=
_temp159; if( _temp184 == 0){ _throw( Null_Exception);} _temp184->tl;}), ++ i){
struct _tuple4 _temp188; void* _temp189; struct Cyc_Absyn_Tqual _temp191; struct
_tuple4* _temp186=( struct _tuple4*)({ struct Cyc_List_List* _temp185= _temp159;
if( _temp185 == 0){ _throw( Null_Exception);} _temp185->hd;}); _temp188=*
_temp186; _LL192: _temp191= _temp188.f1; goto _LL190; _LL190: _temp189= _temp188.f2;
goto _LL187; _LL187: d=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, int
key, void* data)) Cyc_Dict_insert)( d, i, Cyc_CfFlowInfo_typ_to_unesc_none_pinfo(
_temp189));}} return( void*)({ struct Cyc_CfFlowInfo_TuplePI_struct* _temp193=(
struct Cyc_CfFlowInfo_TuplePI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct));
_temp193[ 0]=({ struct Cyc_CfFlowInfo_TuplePI_struct _temp194; _temp194.tag= Cyc_CfFlowInfo_TuplePI;
_temp194.f1= d; _temp194;}); _temp193;});} _LL144: _temp172= _temp161; goto
_LL146; _LL146: { struct Cyc_Dict_Dict* d=(( struct Cyc_Dict_Dict*(*)( int(*
comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
for( 0; _temp172 != 0; _temp172=({ struct Cyc_List_List* _temp195= _temp172; if(
_temp195 == 0){ _throw( Null_Exception);} _temp195->tl;})){ struct Cyc_Absyn_Structfield
_temp199; struct Cyc_List_List* _temp200; struct Cyc_Core_Opt* _temp202; void*
_temp204; struct Cyc_Absyn_Tqual _temp206; struct _tagged_string* _temp208;
struct Cyc_Absyn_Structfield* _temp197=( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp196= _temp172; if( _temp196 == 0){ _throw( Null_Exception);}
_temp196->hd;}); _temp199=* _temp197; _LL209: _temp208=( struct _tagged_string*)
_temp199.name; goto _LL207; _LL207: _temp206=( struct Cyc_Absyn_Tqual) _temp199.tq;
goto _LL205; _LL205: _temp204=( void*) _temp199.type; goto _LL203; _LL203:
_temp202=( struct Cyc_Core_Opt*) _temp199.width; goto _LL201; _LL201: _temp200=(
struct Cyc_List_List*) _temp199.attributes; goto _LL198; _LL198: if( Cyc_String_strcmp(*
_temp208,( struct _tagged_string)({ char* _temp210=( char*)""; struct
_tagged_string _temp211; _temp211.curr= _temp210; _temp211.base= _temp210;
_temp211.last_plus_one= _temp210 + 1; _temp211;})) != 0){ d=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, void* data)) Cyc_Dict_insert)(
d, _temp208, Cyc_CfFlowInfo_typ_to_unesc_none_pinfo( _temp204));}} return( void*)({
struct Cyc_CfFlowInfo_StructPI_struct* _temp212=( struct Cyc_CfFlowInfo_StructPI_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct)); _temp212[ 0]=({
struct Cyc_CfFlowInfo_StructPI_struct _temp213; _temp213.tag= Cyc_CfFlowInfo_StructPI;
_temp213.f1= d; _temp213;}); _temp212;});} _LL148: return Cyc_CfFlowInfo_leaf_unesc_none;
_LL138:;} void* Cyc_CfFlowInfo_lookup_place( struct Cyc_Dict_Dict* d, struct Cyc_CfFlowInfo_Place*
place){ struct Cyc_CfFlowInfo_Place _temp216; struct Cyc_List_List* _temp217;
void* _temp219; struct Cyc_CfFlowInfo_Place* _temp214= place; _temp216=*
_temp214; _LL220: _temp219=( void*) _temp216.root; goto _LL218; _LL218: _temp217=(
struct Cyc_List_List*) _temp216.fields; goto _LL215; _LL215: { void* ans=(( void*(*)(
struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( d, _temp219); for( 0;
_temp217 != 0; _temp217=({ struct Cyc_List_List* _temp221= _temp217; if(
_temp221 == 0){ _throw( Null_Exception);} _temp221->tl;})){ struct _tuple3
_temp224=({ struct _tuple3 _temp222; _temp222.f1= ans; _temp222.f2=( void*)({
struct Cyc_List_List* _temp223= _temp217; if( _temp223 == 0){ _throw(
Null_Exception);} _temp223->hd;}); _temp222;}); void* _temp232; int _temp234;
void* _temp236; struct Cyc_Dict_Dict* _temp238; void* _temp240; struct
_tagged_string* _temp242; void* _temp244; struct Cyc_Dict_Dict* _temp246; _LL226:
_LL237: _temp236= _temp224.f1; if(*(( int*) _temp236) == Cyc_CfFlowInfo_TuplePI){
_LL239: _temp238=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_TuplePI_struct*)
_temp236)->f1; goto _LL233;} else{ goto _LL228;} _LL233: _temp232= _temp224.f2;
if(*(( int*) _temp232) == Cyc_CfFlowInfo_TupleF){ _LL235: _temp234=( int)((
struct Cyc_CfFlowInfo_TupleF_struct*) _temp232)->f1; goto _LL227;} else{ goto
_LL228;} _LL228: _LL245: _temp244= _temp224.f1; if(*(( int*) _temp244) == Cyc_CfFlowInfo_StructPI){
_LL247: _temp246=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp244)->f1; goto _LL241;} else{ goto _LL230;} _LL241: _temp240= _temp224.f2;
if(*(( int*) _temp240) == Cyc_CfFlowInfo_StructF){ _LL243: _temp242=( struct
_tagged_string*)(( struct Cyc_CfFlowInfo_StructF_struct*) _temp240)->f1; goto
_LL229;} else{ goto _LL230;} _LL230: goto _LL231; _LL227: ans=(( void*(*)(
struct Cyc_Dict_Dict* d, int key)) Cyc_Dict_lookup)( _temp238, _temp234); goto
_LL225; _LL229: ans=(( void*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string*
key)) Cyc_Dict_lookup)( _temp246, _temp242); goto _LL225; _LL231:( void) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp248=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp248[ 0]=({ struct
Cyc_Core_Impossible_struct _temp249; _temp249.tag= Cyc_Core_Impossible; _temp249.f1=(
struct _tagged_string)({ char* _temp250=( char*)"bad lookup_place"; struct
_tagged_string _temp251; _temp251.curr= _temp250; _temp251.base= _temp250;
_temp251.last_plus_one= _temp250 + 17; _temp251;}); _temp249;}); _temp248;}));
_LL225:;} return ans;}} struct _tuple5{ void* f1; struct Cyc_List_List* f2; } ;
void* Cyc_CfFlowInfo_insert_place_rec( void* insert_path, struct Cyc_List_List*
fields, void* pinfo){ if( fields == 0){ return pinfo;}{ struct _tuple5 _temp253=({
struct _tuple5 _temp252; _temp252.f1= insert_path; _temp252.f2= fields; _temp252;});
struct Cyc_List_List* _temp261; struct Cyc_List_List _temp263; struct Cyc_List_List*
_temp264; void* _temp266; int _temp268; void* _temp270; struct Cyc_Dict_Dict*
_temp272; struct Cyc_List_List* _temp274; struct Cyc_List_List _temp276; struct
Cyc_List_List* _temp277; void* _temp279; struct _tagged_string* _temp281; void*
_temp283; struct Cyc_Dict_Dict* _temp285; _LL255: _LL271: _temp270= _temp253.f1;
if(*(( int*) _temp270) == Cyc_CfFlowInfo_TuplePI){ _LL273: _temp272=( struct Cyc_Dict_Dict*)((
struct Cyc_CfFlowInfo_TuplePI_struct*) _temp270)->f1; goto _LL262;} else{ goto
_LL257;} _LL262: _temp261= _temp253.f2; if( _temp261 == 0){ goto _LL257;} else{
_temp263=* _temp261; _LL267: _temp266=( void*) _temp263.hd; if(*(( int*)
_temp266) == Cyc_CfFlowInfo_TupleF){ _LL269: _temp268=( int)(( struct Cyc_CfFlowInfo_TupleF_struct*)
_temp266)->f1; goto _LL265;} else{ goto _LL257;} _LL265: _temp264=( struct Cyc_List_List*)
_temp263.tl; goto _LL256;} _LL257: _LL284: _temp283= _temp253.f1; if(*(( int*)
_temp283) == Cyc_CfFlowInfo_StructPI){ _LL286: _temp285=( struct Cyc_Dict_Dict*)((
struct Cyc_CfFlowInfo_StructPI_struct*) _temp283)->f1; goto _LL275;} else{ goto
_LL259;} _LL275: _temp274= _temp253.f2; if( _temp274 == 0){ goto _LL259;} else{
_temp276=* _temp274; _LL280: _temp279=( void*) _temp276.hd; if(*(( int*)
_temp279) == Cyc_CfFlowInfo_StructF){ _LL282: _temp281=( struct _tagged_string*)((
struct Cyc_CfFlowInfo_StructF_struct*) _temp279)->f1; goto _LL278;} else{ goto
_LL259;} _LL278: _temp277=( struct Cyc_List_List*) _temp276.tl; goto _LL258;}
_LL259: goto _LL260; _LL256: { void* next_path=(( void*(*)( struct Cyc_Dict_Dict*
d, int key)) Cyc_Dict_lookup)( _temp272, _temp268); return( void*)({ struct Cyc_CfFlowInfo_TuplePI_struct*
_temp287=( struct Cyc_CfFlowInfo_TuplePI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct));
_temp287[ 0]=({ struct Cyc_CfFlowInfo_TuplePI_struct _temp288; _temp288.tag= Cyc_CfFlowInfo_TuplePI;
_temp288.f1=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, int key, void*
data)) Cyc_Dict_insert)( _temp272, _temp268, Cyc_CfFlowInfo_insert_place_rec(
next_path, _temp264, pinfo)); _temp288;}); _temp287;});} _LL258: { void*
next_path=(( void*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)(
_temp285, _temp281); return( void*)({ struct Cyc_CfFlowInfo_StructPI_struct*
_temp289=( struct Cyc_CfFlowInfo_StructPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct));
_temp289[ 0]=({ struct Cyc_CfFlowInfo_StructPI_struct _temp290; _temp290.tag=
Cyc_CfFlowInfo_StructPI; _temp290.f1=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, void* data)) Cyc_Dict_insert)( _temp285, _temp281,
Cyc_CfFlowInfo_insert_place_rec( next_path, _temp277, pinfo)); _temp290;});
_temp289;});} _LL260:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp291=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp291[ 0]=({ struct Cyc_Core_Impossible_struct _temp292; _temp292.tag= Cyc_Core_Impossible;
_temp292.f1=( struct _tagged_string)({ char* _temp293=( char*)"bad insert_place";
struct _tagged_string _temp294; _temp294.curr= _temp293; _temp294.base= _temp293;
_temp294.last_plus_one= _temp293 + 17; _temp294;}); _temp292;}); _temp291;}));
_LL254:;}} struct Cyc_Dict_Dict* Cyc_CfFlowInfo_insert_place( struct Cyc_Dict_Dict*
d, struct Cyc_CfFlowInfo_Place* place, void* pinfo){ struct Cyc_CfFlowInfo_Place
_temp297; struct Cyc_List_List* _temp298; void* _temp300; struct Cyc_CfFlowInfo_Place*
_temp295= place; _temp297=* _temp295; _LL301: _temp300=( void*) _temp297.root;
goto _LL299; _LL299: _temp298=( struct Cyc_List_List*) _temp297.fields; goto
_LL296; _LL296: return(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void*
key, void* data)) Cyc_Dict_insert)( d, _temp300, Cyc_CfFlowInfo_insert_place_rec(((
void*(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( d, _temp300),
_temp298, pinfo));} struct _tuple6{ struct Cyc_Dict_Dict* f1; struct Cyc_List_List*
f2; } ; int Cyc_CfFlowInfo_isAllInit_pinfo( struct _tuple6* env, void* path);
int Cyc_CfFlowInfo_isAllInit_is( struct _tuple6* env, void* state){ void*
_temp302= state; void* _temp310; void* _temp312; void* _temp314; void* _temp316;
struct Cyc_CfFlowInfo_Place* _temp318; _LL304: if(*(( int*) _temp302) == Cyc_CfFlowInfo_UnknownIS){
_LL313: _temp312=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp302)->f1;
goto _LL311; _LL311: _temp310=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp302)->f2; if( _temp310 ==( void*) Cyc_CfFlowInfo_AllIL){ goto _LL305;}
else{ goto _LL306;}} else{ goto _LL306;} _LL306: if(*(( int*) _temp302) == Cyc_CfFlowInfo_UnknownIS){
_LL317: _temp316=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp302)->f1;
goto _LL315; _LL315: _temp314=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp302)->f2; goto _LL307;} else{ goto _LL308;} _LL308: if(*(( int*) _temp302)
== Cyc_CfFlowInfo_MustPointTo){ _LL319: _temp318=( struct Cyc_CfFlowInfo_Place*)((
struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp302)->f1; goto _LL309;} else{
goto _LL303;} _LL305: return 1; _LL307: return 0; _LL309: { struct _tuple6
_temp322; struct Cyc_List_List* _temp323; struct Cyc_List_List** _temp325;
struct Cyc_Dict_Dict* _temp326; struct _tuple6* _temp320= env; _temp322=*
_temp320; _LL327: _temp326= _temp322.f1; goto _LL324; _LL324: _temp323= _temp322.f2;
_temp325=&(* _temp320).f2; goto _LL321; _LL321: if((( int(*)( int(* compare)(
struct Cyc_CfFlowInfo_Place*, struct Cyc_CfFlowInfo_Place*), struct Cyc_List_List*
l, struct Cyc_CfFlowInfo_Place* x)) Cyc_List_mem)( Cyc_CfFlowInfo_place_cmp,*
_temp325, _temp318)){ return 1;}* _temp325=({ struct Cyc_List_List* _temp328=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp328[
0]=({ struct Cyc_List_List _temp329; _temp329.hd=( void*) _temp318; _temp329.tl=*
_temp325; _temp329;}); _temp328;}); return Cyc_CfFlowInfo_isAllInit_pinfo( env,
Cyc_CfFlowInfo_lookup_place( _temp326, _temp318));} _LL303:;} int Cyc_CfFlowInfo_isAllInit_f(
struct _tuple6* env, void* a, void* path){ return Cyc_CfFlowInfo_isAllInit_pinfo(
env, path);} int Cyc_CfFlowInfo_isAllInit_pinfo( struct _tuple6* env, void* path){
void* _temp330= path; void* _temp338; struct Cyc_Dict_Dict* _temp340; struct Cyc_Dict_Dict*
_temp342; _LL332: if(*(( int*) _temp330) == Cyc_CfFlowInfo_LeafPI){ _LL339:
_temp338=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp330)->f1; goto
_LL333;} else{ goto _LL334;} _LL334: if(*(( int*) _temp330) == Cyc_CfFlowInfo_TuplePI){
_LL341: _temp340=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_TuplePI_struct*)
_temp330)->f1; goto _LL335;} else{ goto _LL336;} _LL336: if(*(( int*) _temp330)
== Cyc_CfFlowInfo_StructPI){ _LL343: _temp342=( struct Cyc_Dict_Dict*)(( struct
Cyc_CfFlowInfo_StructPI_struct*) _temp330)->f1; goto _LL337;} else{ goto _LL331;}
_LL333: return Cyc_CfFlowInfo_isAllInit_is( env, _temp338); _LL335: return(( int(*)(
int(* f)( struct _tuple6*, int, void*), struct _tuple6* env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_forall_c)(( int(*)( struct _tuple6* env, int a, void* path)) Cyc_CfFlowInfo_isAllInit_f,
env, _temp340); _LL337: return(( int(*)( int(* f)( struct _tuple6*, struct
_tagged_string*, void*), struct _tuple6* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_forall_c)((
int(*)( struct _tuple6* env, struct _tagged_string* a, void* path)) Cyc_CfFlowInfo_isAllInit_f,
env, _temp342); _LL331:;} int Cyc_CfFlowInfo_isAllInit( struct Cyc_Dict_Dict*
pinfo_dict, void* pinfo){ struct _tuple6 isInitEnv=({ struct _tuple6 _temp344;
_temp344.f1= pinfo_dict; _temp344.f2= 0; _temp344;}); return Cyc_CfFlowInfo_isAllInit_pinfo(&
isInitEnv, pinfo);} struct _tuple7{ struct Cyc_Dict_Dict* f1; struct Cyc_Set_Set**
f2; } ; void* Cyc_CfFlowInfo_pinfo_getplaces_esc( struct _tuple7* env, void*
pinfo){ struct _tuple7 _temp347; struct Cyc_Set_Set** _temp348; struct Cyc_Dict_Dict*
_temp350; struct _tuple7* _temp345= env; _temp347=* _temp345; _LL351: _temp350=
_temp347.f1; goto _LL349; _LL349: _temp348= _temp347.f2; goto _LL346; _LL346: {
void* _temp352= pinfo; void* _temp362; struct Cyc_CfFlowInfo_Place* _temp364;
void* _temp366; void* _temp368; void* _temp370; struct Cyc_Dict_Dict* _temp372;
struct Cyc_Dict_Dict* _temp374; _LL354: if(*(( int*) _temp352) == Cyc_CfFlowInfo_LeafPI){
_LL363: _temp362=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp352)->f1;
if(*(( int*) _temp362) == Cyc_CfFlowInfo_MustPointTo){ _LL365: _temp364=( struct
Cyc_CfFlowInfo_Place*)(( struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp362)->f1;
goto _LL355;} else{ goto _LL356;}} else{ goto _LL356;} _LL356: if(*(( int*)
_temp352) == Cyc_CfFlowInfo_LeafPI){ _LL367: _temp366=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*)
_temp352)->f1; if(*(( int*) _temp366) == Cyc_CfFlowInfo_UnknownIS){ _LL371:
_temp370=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp366)->f1; goto
_LL369; _LL369: _temp368=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp366)->f2; goto _LL357;} else{ goto _LL358;}} else{ goto _LL358;} _LL358:
if(*(( int*) _temp352) == Cyc_CfFlowInfo_TuplePI){ _LL373: _temp372=( struct Cyc_Dict_Dict*)((
struct Cyc_CfFlowInfo_TuplePI_struct*) _temp352)->f1; goto _LL359;} else{ goto
_LL360;} _LL360: if(*(( int*) _temp352) == Cyc_CfFlowInfo_StructPI){ _LL375:
_temp374=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp352)->f1; goto _LL361;} else{ goto _LL353;} _LL355:* _temp348=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place* elt)) Cyc_Set_insert)(*
_temp348, _temp364); if( Cyc_CfFlowInfo_isAllInit( _temp350, Cyc_CfFlowInfo_lookup_place(
_temp350, _temp364))){ return Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Esc,(
void*) Cyc_CfFlowInfo_AllIL);} return Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Esc,(
void*) Cyc_CfFlowInfo_ThisIL); _LL357: return Cyc_CfFlowInfo_mkLeafPI(( void*)
Cyc_CfFlowInfo_Esc, _temp368); _LL359: return( void*)({ struct Cyc_CfFlowInfo_TuplePI_struct*
_temp376=( struct Cyc_CfFlowInfo_TuplePI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct));
_temp376[ 0]=({ struct Cyc_CfFlowInfo_TuplePI_struct _temp377; _temp377.tag= Cyc_CfFlowInfo_TuplePI;
_temp377.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)( struct _tuple7*, void*),
struct _tuple7* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_map_c)( Cyc_CfFlowInfo_pinfo_getplaces_esc,
env, _temp372); _temp377;}); _temp376;}); _LL361: return( void*)({ struct Cyc_CfFlowInfo_StructPI_struct*
_temp378=( struct Cyc_CfFlowInfo_StructPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct));
_temp378[ 0]=({ struct Cyc_CfFlowInfo_StructPI_struct _temp379; _temp379.tag=
Cyc_CfFlowInfo_StructPI; _temp379.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)(
struct _tuple7*, void*), struct _tuple7* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_map_c)(
Cyc_CfFlowInfo_pinfo_getplaces_esc, env, _temp374); _temp379;}); _temp378;});
_LL353:;}} void Cyc_CfFlowInfo_pinfo_getplaces( struct Cyc_Set_Set** places_ptr,
void* a, void* pinfo){ void* _temp380= pinfo; void* _temp390; void* _temp392;
void* _temp394; void* _temp396; struct Cyc_CfFlowInfo_Place* _temp398; struct
Cyc_Dict_Dict* _temp400; struct Cyc_Dict_Dict* _temp402; _LL382: if(*(( int*)
_temp380) == Cyc_CfFlowInfo_LeafPI){ _LL391: _temp390=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*)
_temp380)->f1; if(*(( int*) _temp390) == Cyc_CfFlowInfo_UnknownIS){ _LL395:
_temp394=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp390)->f1; goto
_LL393; _LL393: _temp392=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp390)->f2; goto _LL383;} else{ goto _LL384;}} else{ goto _LL384;} _LL384:
if(*(( int*) _temp380) == Cyc_CfFlowInfo_LeafPI){ _LL397: _temp396=( void*)((
struct Cyc_CfFlowInfo_LeafPI_struct*) _temp380)->f1; if(*(( int*) _temp396) ==
Cyc_CfFlowInfo_MustPointTo){ _LL399: _temp398=( struct Cyc_CfFlowInfo_Place*)((
struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp396)->f1; goto _LL385;} else{
goto _LL386;}} else{ goto _LL386;} _LL386: if(*(( int*) _temp380) == Cyc_CfFlowInfo_TuplePI){
_LL401: _temp400=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_TuplePI_struct*)
_temp380)->f1; goto _LL387;} else{ goto _LL388;} _LL388: if(*(( int*) _temp380)
== Cyc_CfFlowInfo_StructPI){ _LL403: _temp402=( struct Cyc_Dict_Dict*)(( struct
Cyc_CfFlowInfo_StructPI_struct*) _temp380)->f1; goto _LL389;} else{ goto _LL381;}
_LL383: return; _LL385:* places_ptr=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set*
s, struct Cyc_CfFlowInfo_Place* elt)) Cyc_Set_insert)(* places_ptr, _temp398);
return; _LL387:(( void(*)( void(* f)( struct Cyc_Set_Set**, int, void*), struct
Cyc_Set_Set** env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(( void(*)( struct
Cyc_Set_Set** places_ptr, int a, void* pinfo)) Cyc_CfFlowInfo_pinfo_getplaces,
places_ptr, _temp400); return; _LL389:(( void(*)( void(* f)( struct Cyc_Set_Set**,
struct _tagged_string*, void*), struct Cyc_Set_Set** env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_iter_c)(( void(*)( struct Cyc_Set_Set** places_ptr, struct
_tagged_string* a, void* pinfo)) Cyc_CfFlowInfo_pinfo_getplaces, places_ptr,
_temp402); return; _LL381:;} struct Cyc_Dict_Dict* Cyc_CfFlowInfo_escape_these(
struct Cyc_Set_Set* places, struct Cyc_Dict_Dict* d){ while( !(( int(*)( struct
Cyc_Set_Set* s)) Cyc_Set_is_empty)( places)) { struct Cyc_CfFlowInfo_Place*
place=(( struct Cyc_CfFlowInfo_Place*(*)( struct Cyc_Set_Set* s)) Cyc_Set_choose)(
places); places=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place*
elt)) Cyc_Set_delete)( places, place);{ void* place_old_pinfo;{ struct
_handler_cons _temp404; _push_handler(& _temp404);{ int _temp406= 0; if( setjmp(
_temp404.handler)){ _temp406= 1;} if( ! _temp406){ place_old_pinfo= Cyc_CfFlowInfo_lookup_place(
d, place);; _pop_handler();} else{ void* _temp405=( void*) _exn_thrown; void*
_temp408= _temp405; _LL410: if( _temp408 == Cyc_Dict_Absent){ goto _LL411;}
else{ goto _LL412;} _LL412: goto _LL413; _LL411: continue; _LL413:( void) _throw(
_temp408); _LL409:;}}}{ struct _tuple7 places_env=({ struct _tuple7 _temp414;
_temp414.f1= d; _temp414.f2=& places; _temp414;}); void* place_new_pinfo= Cyc_CfFlowInfo_pinfo_getplaces_esc(&
places_env, place_old_pinfo); if( place_old_pinfo != place_new_pinfo){ d= Cyc_CfFlowInfo_insert_place(
d, place, place_new_pinfo);}}}} return d;} static struct Cyc_Set_Set** Cyc_CfFlowInfo_empty_place_set_ptr=
0; struct Cyc_Dict_Dict* Cyc_CfFlowInfo_escape_pointsto( void* pinfo, struct Cyc_Dict_Dict*
d){ if( Cyc_CfFlowInfo_empty_place_set_ptr == 0){ Cyc_CfFlowInfo_empty_place_set_ptr=({
struct Cyc_Set_Set** _temp415=( struct Cyc_Set_Set**) GC_malloc( sizeof( struct
Cyc_Set_Set*) * 1); _temp415[ 0]=(( struct Cyc_Set_Set*(*)( int(* comp)( struct
Cyc_CfFlowInfo_Place*, struct Cyc_CfFlowInfo_Place*))) Cyc_Set_empty)( Cyc_CfFlowInfo_place_cmp);
_temp415;});}{ struct Cyc_Set_Set* places=({ struct Cyc_Set_Set** _temp416= Cyc_CfFlowInfo_empty_place_set_ptr;
if( _temp416 == 0){ _throw( Null_Exception);}* _temp416;});(( void(*)( struct
Cyc_Set_Set** places_ptr, int a, void* pinfo)) Cyc_CfFlowInfo_pinfo_getplaces)(&
places, 0, pinfo); return Cyc_CfFlowInfo_escape_these( places, d);}} void* Cyc_CfFlowInfo_assign_unknown_dict(
void* level, void* pinfo){ void* _temp417= pinfo; void* _temp427; void* _temp429;
void* _temp431; void* _temp433; struct Cyc_Dict_Dict* _temp435; struct Cyc_Dict_Dict*
_temp437; _LL419: if(*(( int*) _temp417) == Cyc_CfFlowInfo_LeafPI){ _LL428:
_temp427=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp417)->f1; if(*((
int*) _temp427) == Cyc_CfFlowInfo_UnknownIS){ _LL432: _temp431=( void*)(( struct
Cyc_CfFlowInfo_UnknownIS_struct*) _temp427)->f1; if( _temp431 ==( void*) Cyc_CfFlowInfo_Esc){
goto _LL430;} else{ goto _LL421;} _LL430: _temp429=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp427)->f2; goto _LL420;} else{ goto _LL421;}} else{ goto _LL421;} _LL421:
if(*(( int*) _temp417) == Cyc_CfFlowInfo_LeafPI){ _LL434: _temp433=( void*)((
struct Cyc_CfFlowInfo_LeafPI_struct*) _temp417)->f1; goto _LL422;} else{ goto
_LL423;} _LL423: if(*(( int*) _temp417) == Cyc_CfFlowInfo_TuplePI){ _LL436:
_temp435=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_TuplePI_struct*)
_temp417)->f1; goto _LL424;} else{ goto _LL425;} _LL425: if(*(( int*) _temp417)
== Cyc_CfFlowInfo_StructPI){ _LL438: _temp437=( struct Cyc_Dict_Dict*)(( struct
Cyc_CfFlowInfo_StructPI_struct*) _temp417)->f1; goto _LL426;} else{ goto _LL418;}
_LL420: return Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Esc, level);
_LL422: return Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Unesc, level);
_LL424: return( void*)({ struct Cyc_CfFlowInfo_TuplePI_struct* _temp439=( struct
Cyc_CfFlowInfo_TuplePI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct));
_temp439[ 0]=({ struct Cyc_CfFlowInfo_TuplePI_struct _temp440; _temp440.tag= Cyc_CfFlowInfo_TuplePI;
_temp440.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)( void*, void*), void* env,
struct Cyc_Dict_Dict* d)) Cyc_Dict_map_c)( Cyc_CfFlowInfo_assign_unknown_dict,
level, _temp435); _temp440;}); _temp439;}); _LL426: return( void*)({ struct Cyc_CfFlowInfo_StructPI_struct*
_temp441=( struct Cyc_CfFlowInfo_StructPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct));
_temp441[ 0]=({ struct Cyc_CfFlowInfo_StructPI_struct _temp442; _temp442.tag=
Cyc_CfFlowInfo_StructPI; _temp442.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)(
void*, void*), void* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_map_c)( Cyc_CfFlowInfo_assign_unknown_dict,
level, _temp437); _temp442;}); _temp441;}); _LL418:;} struct Cyc_CfFlowInfo_JoinEnv{
struct Cyc_Dict_Dict* d1; struct Cyc_Dict_Dict* d2; struct Cyc_Set_Set** places;
} ; typedef struct Cyc_CfFlowInfo_JoinEnv* Cyc_CfFlowInfo_join_env_t; void* Cyc_CfFlowInfo_join_escape(
void* esc1, void* esc2){ struct _tuple3 _temp444=({ struct _tuple3 _temp443;
_temp443.f1= esc1; _temp443.f2= esc2; _temp443;}); void* _temp452; void*
_temp454; void* _temp456; void* _temp458; _LL446: _LL455: _temp454= _temp444.f1;
if( _temp454 ==( void*) Cyc_CfFlowInfo_Esc){ goto _LL453;} else{ goto _LL448;}
_LL453: _temp452= _temp444.f2; goto _LL447; _LL448: _LL459: _temp458= _temp444.f1;
goto _LL457; _LL457: _temp456= _temp444.f2; if( _temp456 ==( void*) Cyc_CfFlowInfo_Esc){
goto _LL449;} else{ goto _LL450;} _LL450: goto _LL451; _LL447: goto _LL449;
_LL449: return( void*) Cyc_CfFlowInfo_Esc; _LL451: return( void*) Cyc_CfFlowInfo_Unesc;
_LL445:;} void* Cyc_CfFlowInfo_join_initlevel( void* il1, void* il2){ struct
_tuple3 _temp461=({ struct _tuple3 _temp460; _temp460.f1= il1; _temp460.f2= il2;
_temp460;}); void* _temp473; void* _temp475; void* _temp477; void* _temp479;
void* _temp481; void* _temp483; void* _temp485; void* _temp487; _LL463: _LL476:
_temp475= _temp461.f1; if( _temp475 ==( void*) Cyc_CfFlowInfo_NoneIL){ goto
_LL474;} else{ goto _LL465;} _LL474: _temp473= _temp461.f2; goto _LL464; _LL465:
_LL480: _temp479= _temp461.f1; goto _LL478; _LL478: _temp477= _temp461.f2; if(
_temp477 ==( void*) Cyc_CfFlowInfo_NoneIL){ goto _LL466;} else{ goto _LL467;}
_LL467: _LL484: _temp483= _temp461.f1; if( _temp483 ==( void*) Cyc_CfFlowInfo_ThisIL){
goto _LL482;} else{ goto _LL469;} _LL482: _temp481= _temp461.f2; goto _LL468;
_LL469: _LL488: _temp487= _temp461.f1; goto _LL486; _LL486: _temp485= _temp461.f2;
if( _temp485 ==( void*) Cyc_CfFlowInfo_ThisIL){ goto _LL470;} else{ goto _LL471;}
_LL471: goto _LL472; _LL464: goto _LL466; _LL466: return( void*) Cyc_CfFlowInfo_NoneIL;
_LL468: goto _LL470; _LL470: return( void*) Cyc_CfFlowInfo_ThisIL; _LL472:
return( void*) Cyc_CfFlowInfo_AllIL; _LL462:;} void* Cyc_CfFlowInfo_join_initstate(
struct Cyc_CfFlowInfo_JoinEnv* env, void* state1, void* state2){ struct _tuple3
_temp490=({ struct _tuple3 _temp489; _temp489.f1= state1; _temp489.f2= state2;
_temp489;}); void* _temp500; void* _temp502; void* _temp504; void* _temp506;
void* _temp508; void* _temp510; void* _temp512; struct Cyc_CfFlowInfo_Place*
_temp514; void* _temp516; void* _temp518; void* _temp520; void* _temp522; void*
_temp524; void* _temp526; void* _temp528; struct Cyc_CfFlowInfo_Place* _temp530;
void* _temp532; struct Cyc_CfFlowInfo_Place* _temp534; void* _temp536; struct
Cyc_CfFlowInfo_Place* _temp538; _LL492: _LL507: _temp506= _temp490.f1; if(*((
int*) _temp506) == Cyc_CfFlowInfo_UnknownIS){ _LL511: _temp510=( void*)(( struct
Cyc_CfFlowInfo_UnknownIS_struct*) _temp506)->f1; goto _LL509; _LL509: _temp508=(
void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp506)->f2; goto _LL501;}
else{ goto _LL494;} _LL501: _temp500= _temp490.f2; if(*(( int*) _temp500) == Cyc_CfFlowInfo_UnknownIS){
_LL505: _temp504=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp500)->f1;
goto _LL503; _LL503: _temp502=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp500)->f2; goto _LL493;} else{ goto _LL494;} _LL494: _LL517: _temp516=
_temp490.f1; if(*(( int*) _temp516) == Cyc_CfFlowInfo_UnknownIS){ _LL521:
_temp520=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp516)->f1; goto
_LL519; _LL519: _temp518=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp516)->f2; goto _LL513;} else{ goto _LL496;} _LL513: _temp512= _temp490.f2;
if(*(( int*) _temp512) == Cyc_CfFlowInfo_MustPointTo){ _LL515: _temp514=( struct
Cyc_CfFlowInfo_Place*)(( struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp512)->f1;
goto _LL495;} else{ goto _LL496;} _LL496: _LL529: _temp528= _temp490.f1; if(*((
int*) _temp528) == Cyc_CfFlowInfo_MustPointTo){ _LL531: _temp530=( struct Cyc_CfFlowInfo_Place*)((
struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp528)->f1; goto _LL523;} else{
goto _LL498;} _LL523: _temp522= _temp490.f2; if(*(( int*) _temp522) == Cyc_CfFlowInfo_UnknownIS){
_LL527: _temp526=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp522)->f1;
goto _LL525; _LL525: _temp524=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp522)->f2; goto _LL497;} else{ goto _LL498;} _LL498: _LL537: _temp536=
_temp490.f1; if(*(( int*) _temp536) == Cyc_CfFlowInfo_MustPointTo){ _LL539:
_temp538=( struct Cyc_CfFlowInfo_Place*)(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp536)->f1; goto _LL533;} else{ goto _LL491;} _LL533: _temp532= _temp490.f2;
if(*(( int*) _temp532) == Cyc_CfFlowInfo_MustPointTo){ _LL535: _temp534=( struct
Cyc_CfFlowInfo_Place*)(( struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp532)->f1;
goto _LL499;} else{ goto _LL491;} _LL493: return Cyc_CfFlowInfo_mkLeafPI( Cyc_CfFlowInfo_join_escape(
_temp510, _temp504), Cyc_CfFlowInfo_join_initlevel( _temp508, _temp502)); _LL495: {
struct _tuple6 isInitEnv=({ struct _tuple6 _temp540; _temp540.f1= env->d2;
_temp540.f2= 0; _temp540;}); if( _temp518 ==( void*) Cyc_CfFlowInfo_AllIL? Cyc_CfFlowInfo_isAllInit_pinfo(&
isInitEnv, Cyc_CfFlowInfo_lookup_place( env->d2, _temp514)): 0){* env->places=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place* elt))
Cyc_Set_insert)(* env->places, _temp514); return Cyc_CfFlowInfo_mkLeafPI(
_temp520, _temp518);} return Cyc_CfFlowInfo_mkLeafPI( _temp520, Cyc_CfFlowInfo_join_initlevel(
_temp518,( void*) Cyc_CfFlowInfo_ThisIL));} _LL497: { struct _tuple6 isInitEnv=({
struct _tuple6 _temp541; _temp541.f1= env->d1; _temp541.f2= 0; _temp541;}); if(
_temp524 ==( void*) Cyc_CfFlowInfo_AllIL? Cyc_CfFlowInfo_isAllInit_pinfo(&
isInitEnv, Cyc_CfFlowInfo_lookup_place( env->d1, _temp530)): 0){* env->places=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place* elt))
Cyc_Set_insert)(* env->places, _temp530); return Cyc_CfFlowInfo_mkLeafPI(
_temp526, _temp524);} return Cyc_CfFlowInfo_mkLeafPI( _temp526, Cyc_CfFlowInfo_join_initlevel((
void*) Cyc_CfFlowInfo_ThisIL, _temp524));} _LL499: if( Cyc_CfFlowInfo_place_cmp(
_temp538, _temp534) == 0){ return( void*)({ struct Cyc_CfFlowInfo_LeafPI_struct*
_temp542=( struct Cyc_CfFlowInfo_LeafPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_LeafPI_struct));
_temp542[ 0]=({ struct Cyc_CfFlowInfo_LeafPI_struct _temp543; _temp543.tag= Cyc_CfFlowInfo_LeafPI;
_temp543.f1=( void*) state1; _temp543;}); _temp542;});}{ struct _tuple6
isInitEnv1=({ struct _tuple6 _temp545; _temp545.f1= env->d1; _temp545.f2= 0;
_temp545;}); struct _tuple6 isInitEnv2=({ struct _tuple6 _temp544; _temp544.f1=
env->d2; _temp544.f2= 0; _temp544;}); if( Cyc_CfFlowInfo_isAllInit_pinfo(&
isInitEnv1, Cyc_CfFlowInfo_lookup_place( env->d1, _temp538))? Cyc_CfFlowInfo_isAllInit_pinfo(&
isInitEnv2, Cyc_CfFlowInfo_lookup_place( env->d2, _temp534)): 0){* env->places=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place* elt))
Cyc_Set_insert)(* env->places, _temp538);* env->places=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place* elt)) Cyc_Set_insert)(* env->places,
_temp534); return Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Unesc,( void*)
Cyc_CfFlowInfo_AllIL);} return Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Unesc,(
void*) Cyc_CfFlowInfo_ThisIL);} _LL491:;} void* Cyc_CfFlowInfo_join_pathinfo(
struct Cyc_CfFlowInfo_JoinEnv* env, void* pinfo1, void* pinfo2){ if( pinfo1 ==
pinfo2){ return pinfo1;}{ struct _tuple3 _temp547=({ struct _tuple3 _temp546;
_temp546.f1= pinfo1; _temp546.f2= pinfo2; _temp546;}); void* _temp557; void*
_temp559; void* _temp561; void* _temp563; void* _temp565; struct Cyc_Dict_Dict*
_temp567; void* _temp569; struct Cyc_Dict_Dict* _temp571; void* _temp573; struct
Cyc_Dict_Dict* _temp575; void* _temp577; struct Cyc_Dict_Dict* _temp579; _LL549:
_LL562: _temp561= _temp547.f1; if(*(( int*) _temp561) == Cyc_CfFlowInfo_LeafPI){
_LL564: _temp563=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp561)->f1;
goto _LL558;} else{ goto _LL551;} _LL558: _temp557= _temp547.f2; if(*(( int*)
_temp557) == Cyc_CfFlowInfo_LeafPI){ _LL560: _temp559=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*)
_temp557)->f1; goto _LL550;} else{ goto _LL551;} _LL551: _LL570: _temp569=
_temp547.f1; if(*(( int*) _temp569) == Cyc_CfFlowInfo_TuplePI){ _LL572: _temp571=(
struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_TuplePI_struct*) _temp569)->f1;
goto _LL566;} else{ goto _LL553;} _LL566: _temp565= _temp547.f2; if(*(( int*)
_temp565) == Cyc_CfFlowInfo_TuplePI){ _LL568: _temp567=( struct Cyc_Dict_Dict*)((
struct Cyc_CfFlowInfo_TuplePI_struct*) _temp565)->f1; goto _LL552;} else{ goto
_LL553;} _LL553: _LL578: _temp577= _temp547.f1; if(*(( int*) _temp577) == Cyc_CfFlowInfo_StructPI){
_LL580: _temp579=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp577)->f1; goto _LL574;} else{ goto _LL555;} _LL574: _temp573= _temp547.f2;
if(*(( int*) _temp573) == Cyc_CfFlowInfo_StructPI){ _LL576: _temp575=( struct
Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp573)->f1; goto
_LL554;} else{ goto _LL555;} _LL555: goto _LL556; _LL550: if( _temp563 ==
_temp559){ return pinfo1;} return Cyc_CfFlowInfo_join_initstate( env, _temp563,
_temp559); _LL552: if( _temp571 == _temp567){ return pinfo1;} return( void*)({
struct Cyc_CfFlowInfo_TuplePI_struct* _temp581=( struct Cyc_CfFlowInfo_TuplePI_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct)); _temp581[ 0]=({
struct Cyc_CfFlowInfo_TuplePI_struct _temp582; _temp582.tag= Cyc_CfFlowInfo_TuplePI;
_temp582.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)( struct Cyc_CfFlowInfo_JoinEnv*,
void*, void*), struct Cyc_CfFlowInfo_JoinEnv* env, struct Cyc_Dict_Dict* d1,
struct Cyc_Dict_Dict* d2)) Cyc_Dict_intersect_c)( Cyc_CfFlowInfo_join_pathinfo,
env, _temp571, _temp567); _temp582;}); _temp581;}); _LL554: if( _temp579 ==
_temp575){ return pinfo1;} return( void*)({ struct Cyc_CfFlowInfo_StructPI_struct*
_temp583=( struct Cyc_CfFlowInfo_StructPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct));
_temp583[ 0]=({ struct Cyc_CfFlowInfo_StructPI_struct _temp584; _temp584.tag=
Cyc_CfFlowInfo_StructPI; _temp584.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)(
struct Cyc_CfFlowInfo_JoinEnv*, void*, void*), struct Cyc_CfFlowInfo_JoinEnv*
env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_intersect_c)(
Cyc_CfFlowInfo_join_pathinfo, env, _temp579, _temp575); _temp584;}); _temp583;});
_LL556:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp585=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp585[ 0]=({ struct Cyc_Core_Impossible_struct _temp586; _temp586.tag= Cyc_Core_Impossible;
_temp586.f1=( struct _tagged_string)({ char* _temp587=( char*)"join_pathinfo";
struct _tagged_string _temp588; _temp588.curr= _temp587; _temp588.base= _temp587;
_temp588.last_plus_one= _temp587 + 14; _temp588;}); _temp586;}); _temp585;}));
_LL548:;}} void* Cyc_CfFlowInfo_join_flow( void* f1, void* f2){ if( Cyc_CfFlowInfo_empty_place_set_ptr
== 0){ Cyc_CfFlowInfo_empty_place_set_ptr=({ struct Cyc_Set_Set** _temp589=(
struct Cyc_Set_Set**) GC_malloc( sizeof( struct Cyc_Set_Set*) * 1); _temp589[ 0]=((
struct Cyc_Set_Set*(*)( int(* comp)( struct Cyc_CfFlowInfo_Place*, struct Cyc_CfFlowInfo_Place*)))
Cyc_Set_empty)( Cyc_CfFlowInfo_place_cmp); _temp589;});} if( f1 == f2){ return
f1;}{ struct _tuple3 _temp591=({ struct _tuple3 _temp590; _temp590.f1= f1;
_temp590.f2= f2; _temp590;}); void* _temp599; void* _temp601; void* _temp603;
void* _temp605; void* _temp607; struct Cyc_Dict_Dict* _temp609; void* _temp611;
struct Cyc_Dict_Dict* _temp613; _LL593: _LL602: _temp601= _temp591.f1; if(
_temp601 ==( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL600;} else{ goto _LL595;}
_LL600: _temp599= _temp591.f2; goto _LL594; _LL595: _LL606: _temp605= _temp591.f1;
goto _LL604; _LL604: _temp603= _temp591.f2; if( _temp603 ==( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL596;} else{ goto _LL597;} _LL597: _LL612: _temp611= _temp591.f1; if((
unsigned int) _temp611 > 1u?*(( int*) _temp611) == Cyc_CfFlowInfo_InitsFL: 0){
_LL614: _temp613=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp611)->f1; goto _LL608;} else{ goto _LL592;} _LL608: _temp607= _temp591.f2;
if(( unsigned int) _temp607 > 1u?*(( int*) _temp607) == Cyc_CfFlowInfo_InitsFL:
0){ _LL610: _temp609=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp607)->f1; goto _LL598;} else{ goto _LL592;} _LL594: return f2; _LL596:
return f1; _LL598: if( _temp613 == _temp609){ return f1;}{ struct Cyc_Set_Set*
places=({ struct Cyc_Set_Set** _temp618= Cyc_CfFlowInfo_empty_place_set_ptr; if(
_temp618 == 0){ _throw( Null_Exception);}* _temp618;}); struct Cyc_CfFlowInfo_JoinEnv
joinenv=({ struct Cyc_CfFlowInfo_JoinEnv _temp617; _temp617.d1= _temp613;
_temp617.d2= _temp609; _temp617.places=& places; _temp617;}); struct Cyc_Dict_Dict*
outdict=(( struct Cyc_Dict_Dict*(*)( void*(* f)( struct Cyc_CfFlowInfo_JoinEnv*,
void*, void*), struct Cyc_CfFlowInfo_JoinEnv* env, struct Cyc_Dict_Dict* d1,
struct Cyc_Dict_Dict* d2)) Cyc_Dict_intersect_c)( Cyc_CfFlowInfo_join_pathinfo,&
joinenv, _temp613, _temp609); return( void*)({ struct Cyc_CfFlowInfo_InitsFL_struct*
_temp615=( struct Cyc_CfFlowInfo_InitsFL_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct));
_temp615[ 0]=({ struct Cyc_CfFlowInfo_InitsFL_struct _temp616; _temp616.tag= Cyc_CfFlowInfo_InitsFL;
_temp616.f1= Cyc_CfFlowInfo_escape_these( places, outdict); _temp616;});
_temp615;});} _LL592:;}} int Cyc_CfFlowInfo_initstate_lessthan_approx( void*
state1, void* state2){ struct _tuple3 _temp620=({ struct _tuple3 _temp619;
_temp619.f1= state1; _temp619.f2= state2; _temp619;}); void* _temp628; void*
_temp630; void* _temp632; void* _temp634; void* _temp636; void* _temp638; void*
_temp640; struct Cyc_CfFlowInfo_Place* _temp642; void* _temp644; struct Cyc_CfFlowInfo_Place*
_temp646; _LL622: _LL635: _temp634= _temp620.f1; if(*(( int*) _temp634) == Cyc_CfFlowInfo_UnknownIS){
_LL639: _temp638=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp634)->f1;
goto _LL637; _LL637: _temp636=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp634)->f2; goto _LL629;} else{ goto _LL624;} _LL629: _temp628= _temp620.f2;
if(*(( int*) _temp628) == Cyc_CfFlowInfo_UnknownIS){ _LL633: _temp632=( void*)((
struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp628)->f1; goto _LL631; _LL631:
_temp630=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp628)->f2; goto
_LL623;} else{ goto _LL624;} _LL624: _LL645: _temp644= _temp620.f1; if(*(( int*)
_temp644) == Cyc_CfFlowInfo_MustPointTo){ _LL647: _temp646=( struct Cyc_CfFlowInfo_Place*)((
struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp644)->f1; goto _LL641;} else{
goto _LL626;} _LL641: _temp640= _temp620.f2; if(*(( int*) _temp640) == Cyc_CfFlowInfo_MustPointTo){
_LL643: _temp642=( struct Cyc_CfFlowInfo_Place*)(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp640)->f1; goto _LL625;} else{ goto _LL626;} _LL626: goto _LL627; _LL623:
return Cyc_CfFlowInfo_join_escape( _temp638, _temp632) == _temp632? Cyc_CfFlowInfo_join_initlevel(
_temp636, _temp630) == _temp630: 0; _LL625: return Cyc_CfFlowInfo_place_cmp(
_temp646, _temp642) == 0; _LL627: return 0; _LL621:;} int Cyc_CfFlowInfo_pinfo_lessthan_approx(
void* ignore, void* pinfo1, void* pinfo2){ if( pinfo1 == pinfo2){ return 1;}{
struct _tuple3 _temp649=({ struct _tuple3 _temp648; _temp648.f1= pinfo1;
_temp648.f2= pinfo2; _temp648;}); void* _temp659; void* _temp661; void* _temp663;
void* _temp665; void* _temp667; struct Cyc_Dict_Dict* _temp669; void* _temp671;
struct Cyc_Dict_Dict* _temp673; void* _temp675; struct Cyc_Dict_Dict* _temp677;
void* _temp679; struct Cyc_Dict_Dict* _temp681; _LL651: _LL664: _temp663=
_temp649.f1; if(*(( int*) _temp663) == Cyc_CfFlowInfo_LeafPI){ _LL666: _temp665=(
void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp663)->f1; goto _LL660;}
else{ goto _LL653;} _LL660: _temp659= _temp649.f2; if(*(( int*) _temp659) == Cyc_CfFlowInfo_LeafPI){
_LL662: _temp661=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp659)->f1;
goto _LL652;} else{ goto _LL653;} _LL653: _LL672: _temp671= _temp649.f1; if(*((
int*) _temp671) == Cyc_CfFlowInfo_TuplePI){ _LL674: _temp673=( struct Cyc_Dict_Dict*)((
struct Cyc_CfFlowInfo_TuplePI_struct*) _temp671)->f1; goto _LL668;} else{ goto
_LL655;} _LL668: _temp667= _temp649.f2; if(*(( int*) _temp667) == Cyc_CfFlowInfo_TuplePI){
_LL670: _temp669=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_TuplePI_struct*)
_temp667)->f1; goto _LL654;} else{ goto _LL655;} _LL655: _LL680: _temp679=
_temp649.f1; if(*(( int*) _temp679) == Cyc_CfFlowInfo_StructPI){ _LL682:
_temp681=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp679)->f1; goto _LL676;} else{ goto _LL657;} _LL676: _temp675= _temp649.f2;
if(*(( int*) _temp675) == Cyc_CfFlowInfo_StructPI){ _LL678: _temp677=( struct
Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp675)->f1; goto
_LL656;} else{ goto _LL657;} _LL657: goto _LL658; _LL652: return _temp665 ==
_temp661? 1: Cyc_CfFlowInfo_initstate_lessthan_approx( _temp665, _temp661);
_LL654: return _temp673 == _temp669? 1:(( int(*)( int(* f)( int, void*, void*),
struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_forall_intersect)((
int(*)( int ignore, void* pinfo1, void* pinfo2)) Cyc_CfFlowInfo_pinfo_lessthan_approx,
_temp673, _temp669); _LL656: return _temp681 == _temp677? 1:(( int(*)( int(* f)(
struct _tagged_string*, void*, void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict*
d2)) Cyc_Dict_forall_intersect)(( int(*)( struct _tagged_string* ignore, void*
pinfo1, void* pinfo2)) Cyc_CfFlowInfo_pinfo_lessthan_approx, _temp681, _temp677);
_LL658:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp683=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp683[ 0]=({ struct Cyc_Core_Impossible_struct _temp684; _temp684.tag= Cyc_Core_Impossible;
_temp684.f1=( struct _tagged_string)({ char* _temp685=( char*)"pinfo_lessthan";
struct _tagged_string _temp686; _temp686.curr= _temp685; _temp686.base= _temp685;
_temp686.last_plus_one= _temp685 + 15; _temp686;}); _temp684;}); _temp683;}));
_LL650:;}} int Cyc_CfFlowInfo_flow_lessthan_approx( void* f1, void* f2){ if( f1
== f2){ return 1;}{ struct _tuple3 _temp688=({ struct _tuple3 _temp687; _temp687.f1=
f1; _temp687.f2= f2; _temp687;}); void* _temp696; void* _temp698; void* _temp700;
void* _temp702; void* _temp704; struct Cyc_Dict_Dict* _temp706; void* _temp708;
struct Cyc_Dict_Dict* _temp710; _LL690: _LL699: _temp698= _temp688.f1; if(
_temp698 ==( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL697;} else{ goto _LL692;}
_LL697: _temp696= _temp688.f2; goto _LL691; _LL692: _LL703: _temp702= _temp688.f1;
goto _LL701; _LL701: _temp700= _temp688.f2; if( _temp700 ==( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL693;} else{ goto _LL694;} _LL694: _LL709: _temp708= _temp688.f1; if((
unsigned int) _temp708 > 1u?*(( int*) _temp708) == Cyc_CfFlowInfo_InitsFL: 0){
_LL711: _temp710=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp708)->f1; goto _LL705;} else{ goto _LL689;} _LL705: _temp704= _temp688.f2;
if(( unsigned int) _temp704 > 1u?*(( int*) _temp704) == Cyc_CfFlowInfo_InitsFL:
0){ _LL707: _temp706=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp704)->f1; goto _LL695;} else{ goto _LL689;} _LL691: return 1; _LL693:
return 0; _LL695: if( _temp710 == _temp706){ return 1;} return(( int(*)( int(* f)(
void*, void*, void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_forall_intersect)((
int(*)( void* ignore, void* pinfo1, void* pinfo2)) Cyc_CfFlowInfo_pinfo_lessthan_approx,
_temp710, _temp706); _LL689:;}}