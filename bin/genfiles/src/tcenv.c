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
void* v; } ; typedef struct Cyc_Core_Opt* Cyc_Core_opt_t; extern char Cyc_Core_InvalidArg[
15u]; struct Cyc_Core_InvalidArg_struct{ char* tag; struct _tagged_string f1; }
; extern char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{ char* tag;
struct _tagged_string f1; } ; extern char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Core_Not_found[ 14u];
extern char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{ char*
tag; struct _tagged_string f1; } ; extern char* string_to_Cstring( struct
_tagged_string); extern char* underlying_Cstring( struct _tagged_string); extern
struct _tagged_string Cstring_to_string( char*); extern int system( char*);
struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE;
extern struct Cyc_Stdio___sFILE* Cyc_Stdio_stderr; typedef int Cyc_Stdio_fpos_t;
extern int Cyc_Stdio_fflush( struct Cyc_Stdio___sFILE*); extern char Cyc_Stdio_FileCloseError[
19u]; extern char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
char* tag; struct _tagged_string f1; } ; struct Cyc_List_List{ void* hd; struct
Cyc_List_List* tl; } ; typedef struct Cyc_List_List* Cyc_List_glist_t; typedef
struct Cyc_List_List* Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t;
extern char Cyc_List_List_empty[ 15u]; extern char Cyc_List_List_mismatch[ 18u];
extern struct Cyc_List_List* Cyc_List_rev( struct Cyc_List_List* x); extern
struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x); extern struct
Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x, struct Cyc_List_List* y);
extern char Cyc_List_Nth[ 8u]; struct Cyc_Dict_Dict; typedef struct Cyc_Dict_Dict*
Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t; extern char Cyc_Dict_Present[
12u]; extern char Cyc_Dict_Absent[ 11u]; extern struct Cyc_Dict_Dict* Cyc_Dict_empty(
int(* comp)( void*, void*)); extern int Cyc_Dict_member( struct Cyc_Dict_Dict* d,
void* key); extern struct Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict*
d, void* key, void* data); extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d,
void* key); extern struct Cyc_Core_Opt* Cyc_Dict_lookup_opt( struct Cyc_Dict_Dict*
d, void* key); struct Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t;
typedef struct Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t;
extern struct Cyc_Set_Set* Cyc_Set_empty( int(* comp)( void*, void*)); extern
int Cyc_Set_member( struct Cyc_Set_Set* s, void* elt); extern char Cyc_Set_Absent[
11u]; struct Cyc_SlowDict_Dict; typedef struct Cyc_SlowDict_Dict* Cyc_SlowDict_hdict_t;
typedef struct Cyc_SlowDict_Dict* Cyc_SlowDict_dict_t; extern char Cyc_SlowDict_Present[
12u]; extern char Cyc_SlowDict_Absent[ 11u]; extern struct Cyc_SlowDict_Dict*
Cyc_SlowDict_empty( int(* comp)( void*, void*)); extern int Cyc_SlowDict_is_empty(
struct Cyc_SlowDict_Dict* d); extern struct Cyc_SlowDict_Dict* Cyc_SlowDict_insert(
struct Cyc_SlowDict_Dict* d, void* key, void* data); extern struct Cyc_Core_Opt*
Cyc_SlowDict_lookup_opt( struct Cyc_SlowDict_Dict* d, void* key); extern struct
Cyc_SlowDict_Dict* Cyc_SlowDict_delete( struct Cyc_SlowDict_Dict* d, void* key);
struct Cyc_Lineno_Pos{ struct _tagged_string logical_file; struct _tagged_string
line; int line_no; int col; } ; typedef struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t;
extern char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment; typedef struct
Cyc_Position_Segment* Cyc_Position_seg_t; static const int Cyc_Position_Lex= 0;
static const int Cyc_Position_Parse= 1; static const int Cyc_Position_Elab= 2;
typedef void* Cyc_Position_error_kind_t; struct Cyc_Position_Error{ struct
_tagged_string source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_string desc; } ; typedef struct Cyc_Position_Error* Cyc_Position_error_t;
extern char Cyc_Position_Nocontext[ 14u]; extern int Cyc_String_zstrptrcmp(
struct _tagged_string*, struct _tagged_string*); struct Cyc_PP_Ppstate; typedef
struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t; struct Cyc_PP_Out; typedef struct Cyc_PP_Out*
Cyc_PP_out_t; struct Cyc_PP_Doc; typedef struct Cyc_PP_Doc* Cyc_PP_doc_t;
typedef struct _tagged_string* Cyc_Absyn_field_name_t; typedef struct
_tagged_string* Cyc_Absyn_var_t; typedef struct _tagged_string* Cyc_Absyn_tvarname_t;
typedef void* Cyc_Absyn_nmspace_t; struct _tuple0{ void* f1; struct
_tagged_string* f2; } ; typedef struct _tuple0* Cyc_Absyn_qvar_t; typedef struct
_tuple0* Cyc_Absyn_qvar_opt_t; typedef struct _tuple0* Cyc_Absyn_typedef_name_t;
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
struct Cyc_Position_Segment* loc; void* sc; } ; struct Cyc_Absyn_Tuniondecl{
void* sc; struct _tuple0* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
fields; int is_xtunion; } ; struct Cyc_Absyn_Enumfield{ struct _tuple0* name;
struct Cyc_Absyn_Exp* tag; struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_Enumdecl{
void* sc; struct _tuple0* name; struct Cyc_Core_Opt* fields; } ; struct Cyc_Absyn_Typedefdecl{
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
extern char Cyc_Absyn_EmptyAnnot[ 15u]; extern int Cyc_Absyn_varlist_cmp( struct
Cyc_List_List*, struct Cyc_List_List*); extern struct Cyc_Absyn_Conref* Cyc_Absyn_new_conref(
void* x); extern struct _tuple0* Cyc_Absyn_exn_name; extern struct Cyc_Absyn_Tuniondecl*
Cyc_Absyn_exn_xed; extern struct _tagged_string Cyc_Absynpp_typ2string( void*);
extern struct _tagged_string Cyc_Absynpp_qvar2string( struct _tuple0*); static
const int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{ int tag; void* f1;
} ; static const int Cyc_Tcenv_StructRes= 1; struct Cyc_Tcenv_StructRes_struct{
int tag; struct Cyc_Absyn_Structdecl* f1; } ; static const int Cyc_Tcenv_TunionRes=
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
typedef struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tenv_t; extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_tc_init(); extern struct Cyc_Tcenv_Genv* Cyc_Tcenv_empty_genv();
extern struct Cyc_Tcenv_Fenv* Cyc_Tcenv_new_fenv( struct Cyc_Position_Segment*,
struct Cyc_Absyn_Fndecl*); extern struct Cyc_List_List* Cyc_Tcenv_resolve_namespace(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tagged_string*,
struct Cyc_List_List*); extern void* Cyc_Tcenv_lookup_ordinary( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tuple0*); extern struct Cyc_Absyn_Structdecl**
Cyc_Tcenv_lookup_structdecl( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tuple0*); extern struct Cyc_Absyn_Uniondecl** Cyc_Tcenv_lookup_uniondecl(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple0*); extern
struct Cyc_Absyn_Tuniondecl** Cyc_Tcenv_lookup_tuniondecl( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tuple0*); extern struct Cyc_Core_Opt* Cyc_Tcenv_lookup_xtuniondecl(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple0*); extern
struct Cyc_Absyn_Enumdecl** Cyc_Tcenv_lookup_enumdecl( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tuple0*); extern struct Cyc_Absyn_Typedefdecl*
Cyc_Tcenv_lookup_typedefdecl( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tuple0*); extern void* Cyc_Tcenv_return_typ( struct Cyc_Tcenv_Tenv*);
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_local_var( struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Vardecl*); extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_pat_var( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Vardecl*); extern struct Cyc_List_List* Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_type_vars(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_List_List*);
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_in_loop( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Stmt* continue_dest); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_in_switch(
struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_fallthru(
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* new_tvs, struct Cyc_List_List*
vds, struct Cyc_Absyn_Switch_clause* clause); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_clear_fallthru(
struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_next(
struct Cyc_Tcenv_Tenv*, void*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_enter_try(
struct Cyc_Tcenv_Tenv* te); extern int Cyc_Tcenv_get_try_depth( struct Cyc_Tcenv_Tenv*
te); extern void Cyc_Tcenv_process_continue( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*,
struct Cyc_Absyn_Stmt**); extern void Cyc_Tcenv_process_break( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*, struct Cyc_Absyn_Stmt**); extern void Cyc_Tcenv_process_goto(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*, struct _tagged_string*, struct
Cyc_Absyn_Stmt**); struct _tuple3{ struct Cyc_Absyn_Switch_clause* f1; struct
Cyc_List_List* f2; struct Cyc_List_List* f3; } ; extern struct _tuple3* Cyc_Tcenv_process_fallthru(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*, struct Cyc_Absyn_Switch_clause***);
extern struct Cyc_Absyn_Stmt* Cyc_Tcenv_get_encloser( struct Cyc_Tcenv_Tenv*);
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_encloser( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_label(
struct Cyc_Tcenv_Tenv*, struct _tagged_string*, struct Cyc_Absyn_Stmt*); extern
int Cyc_Tcenv_all_labels_resolved( struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_new_block( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*);
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_new_named_block( struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Tvar* name); extern void* Cyc_Tcenv_curr_rgn(
struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_region(
struct Cyc_Tcenv_Tenv* te, void* r); extern void Cyc_Tcenv_check_rgn_accessible(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, void* rgn); extern void
Cyc_Tcenv_check_effect_accessible( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void* eff); extern char Cyc_Tcutil_TypeErr[ 12u]; extern void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*, struct _tagged_string); extern struct Cyc_Core_Opt*
Cyc_Tcutil_empty_var_set; extern int Cyc_Tcutil_subset_effect( int set_to_empty,
void* e1, void* e2); extern int Cyc_Tcutil_region_in_effect( int constrain, void*
r, void* e); extern void Cyc_Tcutil_check_unique_tvars( struct Cyc_Position_Segment*,
struct Cyc_List_List*); extern struct Cyc_Absyn_Tvar* Cyc_Tcutil_new_tvar( void*
k); char Cyc_Tcenv_Env_error[ 14u]="\000\000\000\000Env_error"; void* Cyc_Tcenv_env_err(
struct _tagged_string msg){({ struct _tagged_string _temp0= msg; fprintf( Cyc_Stdio_stderr,"Error: %.*s\n",
_temp0.last_plus_one - _temp0.curr, _temp0.curr);}); Cyc_Stdio_fflush(( struct
Cyc_Stdio___sFILE*) Cyc_Stdio_stderr);( void) _throw(( void*) Cyc_Tcenv_Env_error);}
struct Cyc_Tcenv_Tenv; struct Cyc_Tcenv_Genv; struct Cyc_Tcenv_CtrlEnv{ void*
continue_stmt; void* break_stmt; struct _tuple3* fallthru_clause; void*
next_stmt; int try_depth; } ; typedef struct Cyc_Tcenv_CtrlEnv* Cyc_Tcenv_ctrl_env_t;
struct Cyc_Tcenv_SharedFenv{ void* return_typ; struct Cyc_Dict_Dict* seen_labels;
struct Cyc_SlowDict_Dict* needed_labels; } ; struct Cyc_Tcenv_Fenv{ struct Cyc_Tcenv_SharedFenv*
shared; struct Cyc_List_List* type_vars; struct Cyc_Dict_Dict* locals; struct
Cyc_Absyn_Stmt* encloser; struct Cyc_Tcenv_CtrlEnv* ctrl_env; void* capability;
void* curr_rgn; } ; struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tc_init(){ Cyc_Tcutil_empty_var_set=({
struct Cyc_Core_Opt* _temp1=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1->v=( void*)(( struct Cyc_Set_Set*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Set_empty)( Cyc_String_zstrptrcmp);
_temp1;});{ struct Cyc_Tcenv_Genv* ge= Cyc_Tcenv_empty_genv(); struct Cyc_List_List*
top_ns= 0; ge->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key, struct Cyc_Absyn_Tuniondecl** data)) Cyc_Dict_insert)(
ge->tuniondecls,(* Cyc_Absyn_exn_name).f2,& Cyc_Absyn_exn_xed);{ struct Cyc_Dict_Dict*
ae=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key, struct Cyc_Tcenv_Genv* data)) Cyc_Dict_insert)((( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct Cyc_List_List*, struct Cyc_List_List*))) Cyc_Dict_empty)(
Cyc_Absyn_varlist_cmp), top_ns, ge); return({ struct Cyc_Tcenv_Tenv* _temp2=(
struct Cyc_Tcenv_Tenv*) GC_malloc( sizeof( struct Cyc_Tcenv_Tenv)); _temp2->ns=
top_ns; _temp2->ae= ae; _temp2->le= 0; _temp2;});}}} static struct Cyc_Tcenv_Genv*
Cyc_Tcenv_lookup_namespace( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct _tagged_string* n, struct Cyc_List_List* ns){ return(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
Cyc_Tcenv_resolve_namespace( te, loc, n, ns));} static struct Cyc_List_List* Cyc_Tcenv_outer_namespace(
struct Cyc_List_List* ns){ if( ns == 0){ return(( struct Cyc_List_List*(*)(
struct _tagged_string msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char*
_temp3=( char*)"Tcenv::outer_namespace"; struct _tagged_string _temp4; _temp4.curr=
_temp3; _temp4.base= _temp3; _temp4.last_plus_one= _temp3 + 23; _temp4;}));}
return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(({
struct Cyc_List_List* _temp5=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( ns); if( _temp5 == 0){ _throw( Null_Exception);} _temp5->tl;}));}
static void Cyc_Tcenv_check_repeat( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void*(* lookup)( struct Cyc_Tcenv_Genv*, struct _tagged_string*), struct
_tagged_string* v, struct Cyc_List_List* nss){ for( 0; nss != 0; nss=({ struct
Cyc_List_List* _temp6= nss; if( _temp6 == 0){ _throw( Null_Exception);} _temp6->tl;})){
struct Cyc_Tcenv_Genv* ge2=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,( struct Cyc_List_List*)({
struct Cyc_List_List* _temp18= nss; if( _temp18 == 0){ _throw( Null_Exception);}
_temp18->hd;})); struct _handler_cons _temp7; _push_handler(& _temp7);{ int
_temp9= 0; if( setjmp( _temp7.handler)){ _temp9= 1;} if( ! _temp9){ lookup( ge2,
v); Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp10=* v; xprintf("%.*s is ambiguous",
_temp10.last_plus_one - _temp10.curr, _temp10.curr);}));; _pop_handler();} else{
void* _temp8=( void*) _exn_thrown; void* _temp12= _temp8; _LL14: if( _temp12 ==
Cyc_Dict_Absent){ goto _LL15;} else{ goto _LL16;} _LL16: goto _LL17; _LL15: goto
_LL13; _LL17:( void) _throw( _temp12); _LL13:;}}} return;} static void* Cyc_Tcenv_scoped_lookup(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void*(* lookup)(
struct Cyc_Tcenv_Genv*, struct _tagged_string*), struct _tagged_string* v){
struct Cyc_List_List* cns= te->ns; void* result; while( 1) { struct Cyc_Tcenv_Genv*
ge=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)( te->ae, cns);{ struct _handler_cons _temp19;
_push_handler(& _temp19);{ int _temp21= 0; if( setjmp( _temp19.handler)){
_temp21= 1;} if( ! _temp21){ result= lookup( ge, v);(( void(*)( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void*(* lookup)( struct Cyc_Tcenv_Genv*,
struct _tagged_string*), struct _tagged_string* v, struct Cyc_List_List* nss))
Cyc_Tcenv_check_repeat)( te, loc, lookup, v, ge->availables);{ void* _temp22=
result; _npop_handler( 0u); return _temp22;}; _pop_handler();} else{ void*
_temp20=( void*) _exn_thrown; void* _temp24= _temp20; _LL26: if( _temp24 == Cyc_Dict_Absent){
goto _LL27;} else{ goto _LL28;} _LL28: goto _LL29; _LL27: goto _LL25; _LL29:(
void) _throw( _temp24); _LL25:;}}}{ struct Cyc_List_List* nss= ge->availables;
for( 0; nss != 0; nss=({ struct Cyc_List_List* _temp30= nss; if( _temp30 == 0){
_throw( Null_Exception);} _temp30->tl;})){ struct Cyc_Tcenv_Genv* ge2=(( struct
Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)(
te->ae,( struct Cyc_List_List*)({ struct Cyc_List_List* _temp43= nss; if(
_temp43 == 0){ _throw( Null_Exception);} _temp43->hd;})); struct _handler_cons
_temp31; _push_handler(& _temp31);{ int _temp33= 0; if( setjmp( _temp31.handler)){
_temp33= 1;} if( ! _temp33){ result= lookup( ge2, v);(( void(*)( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void*(* lookup)( struct Cyc_Tcenv_Genv*,
struct _tagged_string*), struct _tagged_string* v, struct Cyc_List_List* nss))
Cyc_Tcenv_check_repeat)( te, loc, lookup, v,({ struct Cyc_List_List* _temp34=
nss; if( _temp34 == 0){ _throw( Null_Exception);} _temp34->tl;}));{ void*
_temp35= result; _npop_handler( 0u); return _temp35;}; _pop_handler();} else{
void* _temp32=( void*) _exn_thrown; void* _temp37= _temp32; _LL39: if( _temp37
== Cyc_Dict_Absent){ goto _LL40;} else{ goto _LL41;} _LL41: goto _LL42; _LL40:
goto _LL38; _LL42:( void) _throw( _temp37); _LL38:;}}}} if( cns == 0){( void)
_throw(( void*) Cyc_Dict_Absent);} cns= Cyc_Tcenv_outer_namespace( cns);}}
struct _tuple4{ void* f1; int f2; } ; static void* Cyc_Tcenv_lookup_ordinary_global_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_string* v){ struct _tuple4* ans=((
struct _tuple4*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)(
ge->ordinaries, v);(* ans).f2= 1; return(* ans).f1;} static void* Cyc_Tcenv_lookup_ordinary_global(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct _tuple0* q){
struct _tuple0 _temp46; struct _tagged_string* _temp47; void* _temp49; struct
_tuple0* _temp44= q; _temp46=* _temp44; _LL50: _temp49= _temp46.f1; goto _LL48;
_LL48: _temp47= _temp46.f2; goto _LL45; _LL45: { void* _temp51= _temp49; struct
Cyc_List_List* _temp61; struct Cyc_List_List* _temp63; struct Cyc_List_List
_temp65; struct Cyc_List_List* _temp66; struct _tagged_string* _temp68; struct
Cyc_List_List* _temp70; _LL53: if( _temp51 ==( void*) Cyc_Absyn_Loc_n){ goto
_LL54;} else{ goto _LL55;} _LL55: if(( unsigned int) _temp51 > 1u?*(( int*)
_temp51) == Cyc_Absyn_Rel_n: 0){ _LL62: _temp61=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp51)->f1; if( _temp61 == 0){ goto _LL56;}
else{ goto _LL57;}} else{ goto _LL57;} _LL57: if(( unsigned int) _temp51 > 1u?*((
int*) _temp51) == Cyc_Absyn_Rel_n: 0){ _LL64: _temp63=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp51)->f1; if( _temp63 == 0){ goto _LL59;}
else{ _temp65=* _temp63; _LL69: _temp68=( struct _tagged_string*) _temp65.hd;
goto _LL67; _LL67: _temp66=( struct Cyc_List_List*) _temp65.tl; goto _LL58;}}
else{ goto _LL59;} _LL59: if(( unsigned int) _temp51 > 1u?*(( int*) _temp51) ==
Cyc_Absyn_Abs_n: 0){ _LL71: _temp70=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp51)->f1; goto _LL60;} else{ goto _LL52;} _LL54: goto _LL56; _LL56: return((
void*(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void*(*
lookup)( struct Cyc_Tcenv_Genv*, struct _tagged_string*), struct _tagged_string*
v)) Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_ordinary_global_f,
_temp47); _LL58: { struct Cyc_Tcenv_Genv* ge= Cyc_Tcenv_lookup_namespace( te,
loc, _temp68, _temp66); return Cyc_Tcenv_lookup_ordinary_global_f( ge, _temp47);}
_LL60: return Cyc_Tcenv_lookup_ordinary_global_f((( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
_temp70), _temp47); _LL52:;}} struct Cyc_List_List* Cyc_Tcenv_resolve_namespace(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct
_tagged_string* n, struct Cyc_List_List* ns){ struct Cyc_List_List* cns= te->ns;
struct Cyc_List_List* possible_results= 0; while( 1) { struct Cyc_Tcenv_Genv* ge=((
struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key))
Cyc_Dict_lookup)( te->ae, cns);{ struct Cyc_List_List* nss= ge->availables; for(
0; nss != 0; nss=({ struct Cyc_List_List* _temp72= nss; if( _temp72 == 0){
_throw( Null_Exception);} _temp72->tl;})){ struct Cyc_Tcenv_Genv* ge2=(( struct
Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)(
te->ae,( struct Cyc_List_List*)({ struct Cyc_List_List* _temp75= nss; if(
_temp75 == 0){ _throw( Null_Exception);} _temp75->hd;})); if((( int(*)( struct
Cyc_Set_Set* s, struct _tagged_string* elt)) Cyc_Set_member)( ge2->namespaces, n)){
possible_results=({ struct Cyc_List_List* _temp73=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp73->hd=( void*)(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( cns,({
struct Cyc_List_List* _temp74=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp74->hd=( void*) n; _temp74->tl= ns; _temp74;})); _temp73->tl=
possible_results; _temp73;});}}} if((( int(*)( struct Cyc_Set_Set* s, struct
_tagged_string* elt)) Cyc_Set_member)( ge->namespaces, n)){ possible_results=({
struct Cyc_List_List* _temp76=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp76->hd=( void*)(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( cns,({ struct Cyc_List_List*
_temp77=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp77->hd=( void*) n; _temp77->tl= ns; _temp77;})); _temp76->tl=
possible_results; _temp76;});} if( possible_results != 0){ if(({ struct Cyc_List_List*
_temp78= possible_results; if( _temp78 == 0){ _throw( Null_Exception);} _temp78->tl;})
!= 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp79=* n; xprintf("%.*s is ambiguous",
_temp79.last_plus_one - _temp79.curr, _temp79.curr);}));} return( struct Cyc_List_List*)({
struct Cyc_List_List* _temp80= possible_results; if( _temp80 == 0){ _throw(
Null_Exception);} _temp80->hd;});} if( cns == 0){( void) _throw(( void*) Cyc_Dict_Absent);}
cns= Cyc_Tcenv_outer_namespace( cns);}} static struct Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_string* v){ return(( struct Cyc_Absyn_Structdecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->structdecls,
v);} struct Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0 _temp83;
struct _tagged_string* _temp84; void* _temp86; struct _tuple0* _temp81= q;
_temp83=* _temp81; _LL87: _temp86= _temp83.f1; goto _LL85; _LL85: _temp84=
_temp83.f2; goto _LL82; _LL82: { void* _temp88= _temp86; struct Cyc_List_List*
_temp98; struct Cyc_List_List* _temp100; struct Cyc_List_List* _temp102; struct
Cyc_List_List _temp104; struct Cyc_List_List* _temp105; struct _tagged_string*
_temp107; _LL90: if( _temp88 ==( void*) Cyc_Absyn_Loc_n){ goto _LL91;} else{
goto _LL92;} _LL92: if(( unsigned int) _temp88 > 1u?*(( int*) _temp88) == Cyc_Absyn_Rel_n:
0){ _LL99: _temp98=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp88)->f1; if( _temp98 == 0){ goto _LL93;} else{ goto _LL94;}} else{ goto
_LL94;} _LL94: if(( unsigned int) _temp88 > 1u?*(( int*) _temp88) == Cyc_Absyn_Abs_n:
0){ _LL101: _temp100=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp88)->f1; goto _LL95;} else{ goto _LL96;} _LL96: if(( unsigned int) _temp88
> 1u?*(( int*) _temp88) == Cyc_Absyn_Rel_n: 0){ _LL103: _temp102=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp88)->f1; if( _temp102 == 0){ goto _LL89;}
else{ _temp104=* _temp102; _LL108: _temp107=( struct _tagged_string*) _temp104.hd;
goto _LL106; _LL106: _temp105=( struct Cyc_List_List*) _temp104.tl; goto _LL97;}}
else{ goto _LL89;} _LL91: goto _LL93; _LL93: return(( struct Cyc_Absyn_Structdecl**(*)(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Structdecl**(*
lookup)( struct Cyc_Tcenv_Genv*, struct _tagged_string*), struct _tagged_string*
v)) Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_structdecl_f, _temp84);
_LL95: { struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, _temp100); return((
struct Cyc_Absyn_Structdecl**(*)( struct Cyc_Dict_Dict* d, struct _tagged_string*
key)) Cyc_Dict_lookup)( ge->structdecls, _temp84);} _LL97: { struct Cyc_Tcenv_Genv*
ge= Cyc_Tcenv_lookup_namespace( te, loc, _temp107, _temp105); return(( struct
Cyc_Absyn_Structdecl**(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key))
Cyc_Dict_lookup)( ge->structdecls, _temp84);} _LL89:;}} static struct Cyc_Absyn_Uniondecl**
Cyc_Tcenv_lookup_uniondecl_f( struct Cyc_Tcenv_Genv* ge, struct _tagged_string*
v){ return(( struct Cyc_Absyn_Uniondecl**(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup)( ge->uniondecls, v);} struct Cyc_Absyn_Uniondecl**
Cyc_Tcenv_lookup_uniondecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct _tuple0* q){ struct _tuple0 _temp111; struct _tagged_string*
_temp112; void* _temp114; struct _tuple0* _temp109= q; _temp111=* _temp109;
_LL115: _temp114= _temp111.f1; goto _LL113; _LL113: _temp112= _temp111.f2; goto
_LL110; _LL110: { void* _temp116= _temp114; struct Cyc_List_List* _temp126;
struct Cyc_List_List* _temp128; struct Cyc_List_List* _temp130; struct Cyc_List_List
_temp132; struct Cyc_List_List* _temp133; struct _tagged_string* _temp135;
_LL118: if( _temp116 ==( void*) Cyc_Absyn_Loc_n){ goto _LL119;} else{ goto
_LL120;} _LL120: if(( unsigned int) _temp116 > 1u?*(( int*) _temp116) == Cyc_Absyn_Rel_n:
0){ _LL127: _temp126=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp116)->f1; if( _temp126 == 0){ goto _LL121;} else{ goto _LL122;}} else{ goto
_LL122;} _LL122: if(( unsigned int) _temp116 > 1u?*(( int*) _temp116) == Cyc_Absyn_Abs_n:
0){ _LL129: _temp128=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp116)->f1; goto _LL123;} else{ goto _LL124;} _LL124: if(( unsigned int)
_temp116 > 1u?*(( int*) _temp116) == Cyc_Absyn_Rel_n: 0){ _LL131: _temp130=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp116)->f1; if(
_temp130 == 0){ goto _LL117;} else{ _temp132=* _temp130; _LL136: _temp135=(
struct _tagged_string*) _temp132.hd; goto _LL134; _LL134: _temp133=( struct Cyc_List_List*)
_temp132.tl; goto _LL125;}} else{ goto _LL117;} _LL119: goto _LL121; _LL121:
return(( struct Cyc_Absyn_Uniondecl**(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Uniondecl**(* lookup)( struct Cyc_Tcenv_Genv*, struct
_tagged_string*), struct _tagged_string* v)) Cyc_Tcenv_scoped_lookup)( te, loc,
Cyc_Tcenv_lookup_uniondecl_f, _temp112); _LL123: { struct Cyc_Tcenv_Genv* ge=((
struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key))
Cyc_Dict_lookup)( te->ae, _temp128); return(( struct Cyc_Absyn_Uniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->uniondecls,
_temp112);} _LL125: { struct Cyc_Tcenv_Genv* ge= Cyc_Tcenv_lookup_namespace( te,
loc, _temp135, _temp133); return(( struct Cyc_Absyn_Uniondecl**(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->uniondecls, _temp112);}
_LL117:;}} static struct Cyc_Absyn_Tuniondecl** Cyc_Tcenv_lookup_tuniondecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_string* v){ return(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->tuniondecls,
v);} struct Cyc_Absyn_Tuniondecl** Cyc_Tcenv_lookup_tuniondecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp139; struct _tagged_string* _temp140; void* _temp142; struct _tuple0*
_temp137= q; _temp139=* _temp137; _LL143: _temp142= _temp139.f1; goto _LL141;
_LL141: _temp140= _temp139.f2; goto _LL138; _LL138: { void* _temp144= _temp142;
struct Cyc_List_List* _temp154; struct Cyc_List_List* _temp156; struct Cyc_List_List
_temp158; struct Cyc_List_List* _temp159; struct _tagged_string* _temp161;
struct Cyc_List_List* _temp163; _LL146: if( _temp144 ==( void*) Cyc_Absyn_Loc_n){
goto _LL147;} else{ goto _LL148;} _LL148: if(( unsigned int) _temp144 > 1u?*((
int*) _temp144) == Cyc_Absyn_Rel_n: 0){ _LL155: _temp154=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp144)->f1; if( _temp154 == 0){ goto _LL149;}
else{ goto _LL150;}} else{ goto _LL150;} _LL150: if(( unsigned int) _temp144 > 1u?*((
int*) _temp144) == Cyc_Absyn_Rel_n: 0){ _LL157: _temp156=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp144)->f1; if( _temp156 == 0){ goto _LL152;}
else{ _temp158=* _temp156; _LL162: _temp161=( struct _tagged_string*) _temp158.hd;
goto _LL160; _LL160: _temp159=( struct Cyc_List_List*) _temp158.tl; goto _LL151;}}
else{ goto _LL152;} _LL152: if(( unsigned int) _temp144 > 1u?*(( int*) _temp144)
== Cyc_Absyn_Abs_n: 0){ _LL164: _temp163=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp144)->f1; goto _LL153;} else{ goto _LL145;} _LL147: goto _LL149; _LL149:
return(( struct Cyc_Absyn_Tuniondecl**(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Tuniondecl**(* lookup)( struct Cyc_Tcenv_Genv*, struct
_tagged_string*), struct _tagged_string* v)) Cyc_Tcenv_scoped_lookup)( te, loc,
Cyc_Tcenv_lookup_tuniondecl_f, _temp140); _LL151: { struct Cyc_Tcenv_Genv* ge=
Cyc_Tcenv_lookup_namespace( te, loc, _temp161, _temp159); return(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->tuniondecls,
_temp140);} _LL153: { struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
_temp163); return(( struct Cyc_Absyn_Tuniondecl**(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key)) Cyc_Dict_lookup)( ge->tuniondecls, _temp140);}
_LL145:;}} static struct Cyc_Absyn_Tuniondecl** Cyc_Tcenv_lookup_xtuniondecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_string* v){ return(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->tuniondecls,
v);} struct Cyc_Core_Opt* Cyc_Tcenv_lookup_xtuniondecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp167; struct _tagged_string* _temp168; void* _temp170; struct _tuple0*
_temp165= q; _temp167=* _temp165; _LL171: _temp170= _temp167.f1; goto _LL169;
_LL169: _temp168= _temp167.f2; goto _LL166; _LL166: { void* _temp172= _temp170;
struct Cyc_List_List* _temp182; struct Cyc_List_List* _temp184; struct Cyc_List_List
_temp186; struct Cyc_List_List* _temp187; struct _tagged_string* _temp189;
struct Cyc_List_List* _temp191; _LL174: if(( unsigned int) _temp172 > 1u?*(( int*)
_temp172) == Cyc_Absyn_Rel_n: 0){ _LL183: _temp182=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp172)->f1; if( _temp182 == 0){ goto _LL175;}
else{ goto _LL176;}} else{ goto _LL176;} _LL176: if( _temp172 ==( void*) Cyc_Absyn_Loc_n){
goto _LL177;} else{ goto _LL178;} _LL178: if(( unsigned int) _temp172 > 1u?*((
int*) _temp172) == Cyc_Absyn_Rel_n: 0){ _LL185: _temp184=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp172)->f1; if( _temp184 == 0){ goto _LL180;}
else{ _temp186=* _temp184; _LL190: _temp189=( struct _tagged_string*) _temp186.hd;
goto _LL188; _LL188: _temp187=( struct Cyc_List_List*) _temp186.tl; goto _LL179;}}
else{ goto _LL180;} _LL180: if(( unsigned int) _temp172 > 1u?*(( int*) _temp172)
== Cyc_Absyn_Abs_n: 0){ _LL192: _temp191=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp172)->f1; goto _LL181;} else{ goto _LL173;} _LL175: { struct _handler_cons
_temp193; _push_handler(& _temp193);{ int _temp195= 0; if( setjmp( _temp193.handler)){
_temp195= 1;} if( ! _temp195){{ struct Cyc_Core_Opt* _temp197=({ struct Cyc_Core_Opt*
_temp196=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp196->v=( void*)(( struct Cyc_Absyn_Tuniondecl**(*)( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Tuniondecl**(* lookup)(
struct Cyc_Tcenv_Genv*, struct _tagged_string*), struct _tagged_string* v)) Cyc_Tcenv_scoped_lookup)(
te, loc, Cyc_Tcenv_lookup_xtuniondecl_f, _temp168); _temp196;}); _npop_handler(
0u); return _temp197;}; _pop_handler();} else{ void* _temp194=( void*)
_exn_thrown; void* _temp199= _temp194; _LL201: if( _temp199 == Cyc_Dict_Absent){
goto _LL202;} else{ goto _LL203;} _LL203: goto _LL204; _LL202: return 0; _LL204:(
void) _throw( _temp199); _LL200:;}}} _LL177: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp205=( char*)"lookup_xtuniondecl: impossible";
struct _tagged_string _temp206; _temp206.curr= _temp205; _temp206.base= _temp205;
_temp206.last_plus_one= _temp205 + 31; _temp206;})); return 0; _LL179: { struct
Cyc_Tcenv_Genv* ge;{ struct _handler_cons _temp207; _push_handler(& _temp207);{
int _temp209= 0; if( setjmp( _temp207.handler)){ _temp209= 1;} if( ! _temp209){
ge= Cyc_Tcenv_lookup_namespace( te, loc, _temp189, _temp187);; _pop_handler();}
else{ void* _temp208=( void*) _exn_thrown; void* _temp211= _temp208; _LL213: if(
_temp211 == Cyc_Dict_Absent){ goto _LL214;} else{ goto _LL215;} _LL215: goto
_LL216; _LL214: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp217=(
char*)"bad qualified name for xtunion"; struct _tagged_string _temp218; _temp218.curr=
_temp217; _temp218.base= _temp217; _temp218.last_plus_one= _temp217 + 31;
_temp218;}));( void) _throw(( void*) Cyc_Dict_Absent); _LL216:( void) _throw(
_temp211); _LL212:;}}} return({ struct Cyc_Core_Opt* _temp219=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp219->v=( void*)(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->tuniondecls,
_temp168); _temp219;});} _LL181: { struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
_temp191); return({ struct Cyc_Core_Opt* _temp220=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp220->v=( void*)(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->tuniondecls,
_temp168); _temp220;});} _LL173:;}} static struct Cyc_Absyn_Enumdecl** Cyc_Tcenv_lookup_enumdecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_string* v){ return(( struct Cyc_Absyn_Enumdecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->enumdecls,
v);} struct Cyc_Absyn_Enumdecl** Cyc_Tcenv_lookup_enumdecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp223; struct _tagged_string* _temp224; void* _temp226; struct _tuple0*
_temp221= q; _temp223=* _temp221; _LL227: _temp226= _temp223.f1; goto _LL225;
_LL225: _temp224= _temp223.f2; goto _LL222; _LL222: { void* _temp228= _temp226;
struct Cyc_List_List* _temp238; struct Cyc_List_List* _temp240; struct Cyc_List_List
_temp242; struct Cyc_List_List* _temp243; struct _tagged_string* _temp245;
struct Cyc_List_List* _temp247; _LL230: if( _temp228 ==( void*) Cyc_Absyn_Loc_n){
goto _LL231;} else{ goto _LL232;} _LL232: if(( unsigned int) _temp228 > 1u?*((
int*) _temp228) == Cyc_Absyn_Rel_n: 0){ _LL239: _temp238=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp228)->f1; if( _temp238 == 0){ goto _LL233;}
else{ goto _LL234;}} else{ goto _LL234;} _LL234: if(( unsigned int) _temp228 > 1u?*((
int*) _temp228) == Cyc_Absyn_Rel_n: 0){ _LL241: _temp240=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp228)->f1; if( _temp240 == 0){ goto _LL236;}
else{ _temp242=* _temp240; _LL246: _temp245=( struct _tagged_string*) _temp242.hd;
goto _LL244; _LL244: _temp243=( struct Cyc_List_List*) _temp242.tl; goto _LL235;}}
else{ goto _LL236;} _LL236: if(( unsigned int) _temp228 > 1u?*(( int*) _temp228)
== Cyc_Absyn_Abs_n: 0){ _LL248: _temp247=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp228)->f1; goto _LL237;} else{ goto _LL229;} _LL231: goto _LL233; _LL233:
return(( struct Cyc_Absyn_Enumdecl**(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Enumdecl**(* lookup)( struct Cyc_Tcenv_Genv*, struct
_tagged_string*), struct _tagged_string* v)) Cyc_Tcenv_scoped_lookup)( te, loc,
Cyc_Tcenv_lookup_enumdecl_f, _temp224); _LL235: { struct Cyc_Tcenv_Genv* ge= Cyc_Tcenv_lookup_namespace(
te, loc, _temp245, _temp243); return(( struct Cyc_Absyn_Enumdecl**(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->enumdecls,
_temp224);} _LL237: { struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
_temp247); return(( struct Cyc_Absyn_Enumdecl**(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key)) Cyc_Dict_lookup)( ge->enumdecls, _temp224);} _LL229:;}}
static struct Cyc_Absyn_Typedefdecl* Cyc_Tcenv_lookup_typedefdecl_f( struct Cyc_Tcenv_Genv*
ge, struct _tagged_string* v){ return(( struct Cyc_Absyn_Typedefdecl*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->typedefs, v);}
struct Cyc_Absyn_Typedefdecl* Cyc_Tcenv_lookup_typedefdecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp251; struct _tagged_string* _temp252; void* _temp254; struct _tuple0*
_temp249= q; _temp251=* _temp249; _LL255: _temp254= _temp251.f1; goto _LL253;
_LL253: _temp252= _temp251.f2; goto _LL250; _LL250: { void* _temp256= _temp254;
struct Cyc_List_List* _temp266; struct Cyc_List_List* _temp268; struct Cyc_List_List
_temp270; struct Cyc_List_List* _temp271; struct _tagged_string* _temp273;
struct Cyc_List_List* _temp275; _LL258: if( _temp256 ==( void*) Cyc_Absyn_Loc_n){
goto _LL259;} else{ goto _LL260;} _LL260: if(( unsigned int) _temp256 > 1u?*((
int*) _temp256) == Cyc_Absyn_Rel_n: 0){ _LL267: _temp266=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp256)->f1; if( _temp266 == 0){ goto _LL261;}
else{ goto _LL262;}} else{ goto _LL262;} _LL262: if(( unsigned int) _temp256 > 1u?*((
int*) _temp256) == Cyc_Absyn_Rel_n: 0){ _LL269: _temp268=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp256)->f1; if( _temp268 == 0){ goto _LL264;}
else{ _temp270=* _temp268; _LL274: _temp273=( struct _tagged_string*) _temp270.hd;
goto _LL272; _LL272: _temp271=( struct Cyc_List_List*) _temp270.tl; goto _LL263;}}
else{ goto _LL264;} _LL264: if(( unsigned int) _temp256 > 1u?*(( int*) _temp256)
== Cyc_Absyn_Abs_n: 0){ _LL276: _temp275=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp256)->f1; goto _LL265;} else{ goto _LL257;} _LL259: goto _LL261; _LL261:
return(( struct Cyc_Absyn_Typedefdecl*(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Typedefdecl*(* lookup)( struct Cyc_Tcenv_Genv*, struct
_tagged_string*), struct _tagged_string* v)) Cyc_Tcenv_scoped_lookup)( te, loc,
Cyc_Tcenv_lookup_typedefdecl_f, _temp252); _LL263: { struct Cyc_Tcenv_Genv* ge=
Cyc_Tcenv_lookup_namespace( te, loc, _temp273, _temp271); return(( struct Cyc_Absyn_Typedefdecl*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->typedefs,
_temp252);} _LL265: { struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
_temp275); return(( struct Cyc_Absyn_Typedefdecl*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key)) Cyc_Dict_lookup)( ge->typedefs, _temp252);} _LL257:;}}
struct Cyc_Tcenv_Genv* Cyc_Tcenv_empty_genv(){ return({ struct Cyc_Tcenv_Genv*
_temp277=( struct Cyc_Tcenv_Genv*) GC_malloc( sizeof( struct Cyc_Tcenv_Genv));
_temp277->namespaces=( struct Cyc_Set_Set*)({ struct Cyc_Core_Opt* _temp278= Cyc_Tcutil_empty_var_set;
if( _temp278 == 0){ _throw( Null_Exception);} _temp278->v;}); _temp277->structdecls=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_string*, struct
_tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp277->uniondecls=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_string*, struct
_tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp277->tuniondecls=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_string*, struct
_tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp277->enumdecls=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_string*, struct
_tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp277->typedefs=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_string*, struct
_tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp277->ordinaries=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_string*, struct
_tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp277->availables=
0; _temp277;});} void* Cyc_Tcenv_get_visible( void* f){ while( 1) { void*
_temp279= f; void* _temp287; void* _temp289; void* _temp291; void* _temp293;
void* _temp295; _LL281: if(*(( int*) _temp279) == Cyc_Tcenv_Outermost){ _LL288:
_temp287=( void*)(( struct Cyc_Tcenv_Outermost_struct*) _temp279)->f1; goto
_LL282;} else{ goto _LL283;} _LL283: if(*(( int*) _temp279) == Cyc_Tcenv_Frame){
_LL292: _temp291=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp279)->f1; goto
_LL290; _LL290: _temp289=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp279)->f2;
goto _LL284;} else{ goto _LL285;} _LL285: if(*(( int*) _temp279) == Cyc_Tcenv_Hidden){
_LL296: _temp295=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp279)->f1; goto
_LL294; _LL294: _temp293=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp279)->f2;
goto _LL286;} else{ goto _LL280;} _LL282: return _temp287; _LL284: return
_temp291; _LL286: f= _temp293; goto _LL280; _LL280:;}} void* Cyc_Tcenv_put_visible(
void* f, void* x){ void* _temp297= f; void* _temp305; void* _temp307; void*
_temp309; void* _temp311; void* _temp313; _LL299: if(*(( int*) _temp297) == Cyc_Tcenv_Outermost){
_LL306: _temp305=( void*)(( struct Cyc_Tcenv_Outermost_struct*) _temp297)->f1;
goto _LL300;} else{ goto _LL301;} _LL301: if(*(( int*) _temp297) == Cyc_Tcenv_Frame){
_LL310: _temp309=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp297)->f1; goto
_LL308; _LL308: _temp307=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp297)->f2;
goto _LL302;} else{ goto _LL303;} _LL303: if(*(( int*) _temp297) == Cyc_Tcenv_Hidden){
_LL314: _temp313=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp297)->f1; goto
_LL312; _LL312: _temp311=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp297)->f2;
goto _LL304;} else{ goto _LL298;} _LL300: return( void*)({ struct Cyc_Tcenv_Outermost_struct*
_temp315=( struct Cyc_Tcenv_Outermost_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Outermost_struct));
_temp315[ 0]=({ struct Cyc_Tcenv_Outermost_struct _temp316; _temp316.tag= Cyc_Tcenv_Outermost;
_temp316.f1=( void*) x; _temp316;}); _temp315;}); _LL302: return( void*)({
struct Cyc_Tcenv_Frame_struct* _temp317=( struct Cyc_Tcenv_Frame_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Frame_struct)); _temp317[ 0]=({ struct Cyc_Tcenv_Frame_struct
_temp318; _temp318.tag= Cyc_Tcenv_Frame; _temp318.f1=( void*) x; _temp318.f2=(
void*) _temp307; _temp318;}); _temp317;}); _LL304: return( void*)({ struct Cyc_Tcenv_Hidden_struct*
_temp319=( struct Cyc_Tcenv_Hidden_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Hidden_struct));
_temp319[ 0]=({ struct Cyc_Tcenv_Hidden_struct _temp320; _temp320.tag= Cyc_Tcenv_Hidden;
_temp320.f1=( void*) _temp313; _temp320.f2=( void*)(( void*(*)( void* f, void* x))
Cyc_Tcenv_put_visible)( _temp311, x); _temp320;}); _temp319;}); _LL298:;} void*
Cyc_Tcenv_apply_to_visible( void*(* g)( void*), void* f){ return(( void*(*)(
void* f, void* x)) Cyc_Tcenv_put_visible)( f, g((( void*(*)( void* f)) Cyc_Tcenv_get_visible)(
f)));} void* Cyc_Tcenv_map_frames( void*(* g)( void*), void* f){ void* _temp321=
f; void* _temp329; void* _temp331; void* _temp333; void* _temp335; void*
_temp337; _LL323: if(*(( int*) _temp321) == Cyc_Tcenv_Outermost){ _LL330:
_temp329=( void*)(( struct Cyc_Tcenv_Outermost_struct*) _temp321)->f1; goto
_LL324;} else{ goto _LL325;} _LL325: if(*(( int*) _temp321) == Cyc_Tcenv_Frame){
_LL334: _temp333=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp321)->f1; goto
_LL332; _LL332: _temp331=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp321)->f2;
goto _LL326;} else{ goto _LL327;} _LL327: if(*(( int*) _temp321) == Cyc_Tcenv_Hidden){
_LL338: _temp337=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp321)->f1; goto
_LL336; _LL336: _temp335=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp321)->f2;
goto _LL328;} else{ goto _LL322;} _LL324: return( void*)({ struct Cyc_Tcenv_Outermost_struct*
_temp339=( struct Cyc_Tcenv_Outermost_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Outermost_struct));
_temp339[ 0]=({ struct Cyc_Tcenv_Outermost_struct _temp340; _temp340.tag= Cyc_Tcenv_Outermost;
_temp340.f1=( void*) g( _temp329); _temp340;}); _temp339;}); _LL326: return(
void*)({ struct Cyc_Tcenv_Frame_struct* _temp341=( struct Cyc_Tcenv_Frame_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Frame_struct)); _temp341[ 0]=({ struct Cyc_Tcenv_Frame_struct
_temp342; _temp342.tag= Cyc_Tcenv_Frame; _temp342.f1=( void*) g( _temp333);
_temp342.f2=( void*)(( void*(*)( void*(* g)( void*), void* f)) Cyc_Tcenv_map_frames)(
g, _temp331); _temp342;}); _temp341;}); _LL328: return( void*)({ struct Cyc_Tcenv_Hidden_struct*
_temp343=( struct Cyc_Tcenv_Hidden_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Hidden_struct));
_temp343[ 0]=({ struct Cyc_Tcenv_Hidden_struct _temp344; _temp344.tag= Cyc_Tcenv_Hidden;
_temp344.f1=( void*) g( _temp337); _temp344.f2=( void*)(( void*(*)( void*(* g)(
void*), void* f)) Cyc_Tcenv_map_frames)( g, _temp335); _temp344;}); _temp343;});
_LL322:;} struct Cyc_Tcenv_Fenv* Cyc_Tcenv_get_fenv( struct Cyc_Tcenv_Tenv* te){
if( te->le == 0){( void) _throw(( void*) Cyc_Tcenv_Env_error);} return(( struct
Cyc_Tcenv_Fenv*(*)( void* f)) Cyc_Tcenv_get_visible)(( void*)({ struct Cyc_Core_Opt*
_temp345= te->le; if( _temp345 == 0){ _throw( Null_Exception);} _temp345->v;}));}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_put_fenv( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Fenv*
fe){ if( te->le == 0){( void) _throw(( void*) Cyc_Tcenv_Env_error);} return({
struct Cyc_Tcenv_Tenv* _temp346=( struct Cyc_Tcenv_Tenv*) GC_malloc( sizeof(
struct Cyc_Tcenv_Tenv)); _temp346->ns= te->ns; _temp346->ae= te->ae; _temp346->le=({
struct Cyc_Core_Opt* _temp347=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp347->v=( void*)(( void*(*)( void* f, struct Cyc_Tcenv_Fenv*
x)) Cyc_Tcenv_put_visible)(( void*)({ struct Cyc_Core_Opt* _temp348= te->le; if(
_temp348 == 0){ _throw( Null_Exception);} _temp348->v;}), fe); _temp347;});
_temp346;});} static struct Cyc_Tcenv_Fenv* Cyc_Tcenv_copy_fenv_old_ctrl( struct
Cyc_Tcenv_Fenv* f){ return({ struct Cyc_Tcenv_Fenv* _temp349=( struct Cyc_Tcenv_Fenv*)
GC_malloc( sizeof( struct Cyc_Tcenv_Fenv)); _temp349->shared= f->shared;
_temp349->type_vars= f->type_vars; _temp349->locals= f->locals; _temp349->encloser=
f->encloser; _temp349->ctrl_env= f->ctrl_env; _temp349->capability=( void*)((
void*) f->capability); _temp349->curr_rgn=( void*)(( void*) f->curr_rgn);
_temp349;});} static struct Cyc_Tcenv_Fenv* Cyc_Tcenv_copy_fenv_new_ctrl( struct
Cyc_Tcenv_Fenv* f){ struct Cyc_Tcenv_CtrlEnv* c= f->ctrl_env; return({ struct
Cyc_Tcenv_Fenv* _temp350=( struct Cyc_Tcenv_Fenv*) GC_malloc( sizeof( struct Cyc_Tcenv_Fenv));
_temp350->shared= f->shared; _temp350->type_vars= f->type_vars; _temp350->locals=
f->locals; _temp350->encloser= f->encloser; _temp350->ctrl_env=({ struct Cyc_Tcenv_CtrlEnv*
_temp351=( struct Cyc_Tcenv_CtrlEnv*) GC_malloc( sizeof( struct Cyc_Tcenv_CtrlEnv));
_temp351->continue_stmt=( void*)(( void*) c->continue_stmt); _temp351->break_stmt=(
void*)(( void*) c->break_stmt); _temp351->fallthru_clause= c->fallthru_clause;
_temp351->next_stmt=( void*)(( void*) c->next_stmt); _temp351->try_depth= c->try_depth;
_temp351;}); _temp350->capability=( void*)(( void*) f->capability); _temp350->curr_rgn=(
void*)(( void*) f->curr_rgn); _temp350;});} void* Cyc_Tcenv_return_typ( struct
Cyc_Tcenv_Tenv* te){ if( te->le == 0){(( void(*)( struct _tagged_string msg))
Cyc_Tcenv_env_err)(( struct _tagged_string)({ char* _temp352=( char*)"Tcenv: unexpected return_typ";
struct _tagged_string _temp353; _temp353.curr= _temp352; _temp353.base= _temp352;
_temp353.last_plus_one= _temp352 + 29; _temp353;}));} return( void*)(( Cyc_Tcenv_get_fenv(
te))->shared)->return_typ;} struct Cyc_List_List* Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv* te){ if( te->le == 0){ return 0;} return( Cyc_Tcenv_get_fenv(
te))->type_vars;} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_type_vars( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* tvs){ if( te->le == 0){((
void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({
char* _temp354=( char*)"Tcenv: unexpected add_type_vars"; struct _tagged_string
_temp355; _temp355.curr= _temp354; _temp355.base= _temp354; _temp355.last_plus_one=
_temp354 + 32; _temp355;}));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); struct Cyc_List_List* new_tvs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tvs, fe->type_vars);
Cyc_Tcutil_check_unique_tvars( loc, new_tvs); fe->type_vars= new_tvs; return Cyc_Tcenv_put_fenv(
te, fe);}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_local_var( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Vardecl* vd){ if( te->le == 0){((
void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({
char* _temp356=( char*)"Tcenv: unexpected add_local_var"; struct _tagged_string
_temp357; _temp357.curr= _temp356; _temp357.base= _temp356; _temp357.last_plus_one=
_temp356 + 32; _temp357;}));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); struct _tagged_string* v=(* vd->name).f2;{ void*
_temp358=(* vd->name).f1; _LL360: if( _temp358 ==( void*) Cyc_Absyn_Loc_n){ goto
_LL361;} else{ goto _LL362;} _LL362: goto _LL363; _LL361: goto _LL359; _LL363:(
void) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp364=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp364[ 0]=({ struct
Cyc_Core_Impossible_struct _temp365; _temp365.tag= Cyc_Core_Impossible; _temp365.f1=(
struct _tagged_string)({ char* _temp366=( char*)"add_local_var: called with Rel_n";
struct _tagged_string _temp367; _temp367.curr= _temp366; _temp367.base= _temp366;
_temp367.last_plus_one= _temp366 + 33; _temp367;}); _temp365;}); _temp364;}));
goto _LL359; _LL359:;} fe->locals=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, void* data)) Cyc_Dict_insert)( fe->locals, v,(
void*)({ struct Cyc_Absyn_Local_b_struct* _temp368=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp368[ 0]=({ struct Cyc_Absyn_Local_b_struct
_temp369; _temp369.tag= Cyc_Absyn_Local_b; _temp369.f1= vd; _temp369;});
_temp368;})); return Cyc_Tcenv_put_fenv( te, fe);}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_pat_var(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Vardecl*
vd){ if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp370=( char*)"Tcenv: unexpected add_pat_var";
struct _tagged_string _temp371; _temp371.curr= _temp370; _temp371.base= _temp370;
_temp371.last_plus_one= _temp370 + 30; _temp371;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_old_ctrl( Cyc_Tcenv_get_fenv( te)); struct _tagged_string* v=(*
vd->name).f2; fe->locals=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key, void* data)) Cyc_Dict_insert)( fe->locals, v,( void*)({
struct Cyc_Absyn_Pat_b_struct* _temp372=( struct Cyc_Absyn_Pat_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pat_b_struct)); _temp372[ 0]=({ struct Cyc_Absyn_Pat_b_struct
_temp373; _temp373.tag= Cyc_Absyn_Pat_b; _temp373.f1= vd; _temp373;}); _temp372;}));
return Cyc_Tcenv_put_fenv( te, fe);}} void* Cyc_Tcenv_lookup_ordinary( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct
_tuple0 _temp376; struct _tagged_string* _temp377; void* _temp379; struct
_tuple0* _temp374= q; _temp376=* _temp374; _LL380: _temp379= _temp376.f1; goto
_LL378; _LL378: _temp377= _temp376.f2; goto _LL375; _LL375: { void* _temp381=
_temp379; struct Cyc_List_List* _temp390; _LL383: if( _temp381 ==( void*) Cyc_Absyn_Loc_n){
goto _LL384;} else{ goto _LL385;} _LL385: if(( unsigned int) _temp381 > 1u?*((
int*) _temp381) == Cyc_Absyn_Rel_n: 0){ _LL391: _temp390=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp381)->f1; if( _temp390 == 0){ goto _LL389;}
else{ goto _LL387;}} else{ goto _LL387;} _LL389: if( te->le != 0){ goto _LL386;}
else{ goto _LL387;} _LL387: goto _LL388; _LL384: if( te->le == 0){( void) _throw((
void*) Cyc_Dict_Absent);} else{ goto _LL386;} _LL386: { struct Cyc_Tcenv_Fenv*
fe= Cyc_Tcenv_get_fenv( te); struct _handler_cons _temp392; _push_handler(&
_temp392);{ int _temp394= 0; if( setjmp( _temp392.handler)){ _temp394= 1;} if( !
_temp394){{ void* _temp397=( void*)({ struct Cyc_Tcenv_VarRes_struct* _temp395=(
struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp395[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp396; _temp396.tag= Cyc_Tcenv_VarRes;
_temp396.f1=( void*)(( void*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string*
key)) Cyc_Dict_lookup)( fe->locals, _temp377); _temp396;}); _temp395;});
_npop_handler( 0u); return _temp397;}; _pop_handler();} else{ void* _temp393=(
void*) _exn_thrown; void* _temp399= _temp393; _LL401: if( _temp399 == Cyc_Dict_Absent){
goto _LL402;} else{ goto _LL403;} _LL403: goto _LL404; _LL402: return Cyc_Tcenv_lookup_ordinary_global(
te, loc, q); _LL404:( void) _throw( _temp399); _LL400:;}}} _LL388: { struct
_handler_cons _temp405; _push_handler(& _temp405);{ int _temp407= 0; if( setjmp(
_temp405.handler)){ _temp407= 1;} if( ! _temp407){{ void* _temp408= Cyc_Tcenv_lookup_ordinary_global(
te, loc, q); _npop_handler( 0u); return _temp408;}; _pop_handler();} else{ void*
_temp406=( void*) _exn_thrown; void* _temp410= _temp406; _LL412: if( _temp410 ==
Cyc_Dict_Absent){ goto _LL413;} else{ goto _LL414;} _LL414: goto _LL415; _LL413:
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp416= Cyc_Absynpp_qvar2string(
q); xprintf("unbound identifer: %.*s", _temp416.last_plus_one - _temp416.curr,
_temp416.curr);})); return( void*)({ struct Cyc_Tcenv_VarRes_struct* _temp417=(
struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp417[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp418; _temp418.tag= Cyc_Tcenv_VarRes;
_temp418.f1=( void*)(( void*) Cyc_Absyn_Unresolved_b); _temp418;}); _temp417;});
_LL415:( void) _throw( _temp410); _LL411:;}}} _LL382:;}} void Cyc_Tcenv_process_continue(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Stmt**
sopt){ if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp419=( char*)"Tcenv: unexpected process_continue";
struct _tagged_string _temp420; _temp420.curr= _temp419; _temp420.base= _temp419;
_temp420.last_plus_one= _temp419 + 35; _temp420;}));}{ void* _temp421=( void*)((
Cyc_Tcenv_get_fenv( te))->ctrl_env)->continue_stmt; struct Cyc_Absyn_Stmt*
_temp431; _LL423: if(( unsigned int) _temp421 > 3u?*(( int*) _temp421) == Cyc_Tcenv_Stmt_j:
0){ _LL432: _temp431=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Tcenv_Stmt_j_struct*)
_temp421)->f1; goto _LL424;} else{ goto _LL425;} _LL425: if( _temp421 ==( void*)
Cyc_Tcenv_NotLoop_j){ goto _LL426;} else{ goto _LL427;} _LL427: if( _temp421 ==(
void*) Cyc_Tcenv_CaseEnd_j){ goto _LL428;} else{ goto _LL429;} _LL429: if(
_temp421 ==( void*) Cyc_Tcenv_FnEnd_j){ goto _LL430;} else{ goto _LL422;} _LL424:
_temp431->non_local_preds=({ struct Cyc_List_List* _temp433=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp433->hd=( void*) s; _temp433->tl=
_temp431->non_local_preds; _temp433;});* sopt=( struct Cyc_Absyn_Stmt*) _temp431;
return; _LL426: Cyc_Tcutil_terr( s->loc,( struct _tagged_string)({ char*
_temp434=( char*)"continue not in a loop"; struct _tagged_string _temp435;
_temp435.curr= _temp434; _temp435.base= _temp434; _temp435.last_plus_one=
_temp434 + 23; _temp435;})); return; _LL428:(( void(*)( struct _tagged_string
msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char* _temp436=( char*)"Tcenv: continue dest is CaseEnd";
struct _tagged_string _temp437; _temp437.curr= _temp436; _temp437.base= _temp436;
_temp437.last_plus_one= _temp436 + 32; _temp437;})); return; _LL430:(( void(*)(
struct _tagged_string msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char*
_temp438=( char*)"Tcenv: continue dest is CaseEnd"; struct _tagged_string
_temp439; _temp439.curr= _temp438; _temp439.base= _temp438; _temp439.last_plus_one=
_temp438 + 32; _temp439;})); return; _LL422:;}} void Cyc_Tcenv_process_break(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Stmt**
sopt){ if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp440=( char*)"Tcenv: unexpected process_break";
struct _tagged_string _temp441; _temp441.curr= _temp440; _temp441.base= _temp440;
_temp441.last_plus_one= _temp440 + 32; _temp441;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_get_fenv( te); void* _temp442=( void*)( fe->ctrl_env)->break_stmt;
struct Cyc_Absyn_Stmt* _temp452; _LL444: if(( unsigned int) _temp442 > 3u?*((
int*) _temp442) == Cyc_Tcenv_Stmt_j: 0){ _LL453: _temp452=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Tcenv_Stmt_j_struct*) _temp442)->f1; goto _LL445;} else{ goto _LL446;}
_LL446: if( _temp442 ==( void*) Cyc_Tcenv_NotLoop_j){ goto _LL447;} else{ goto
_LL448;} _LL448: if( _temp442 ==( void*) Cyc_Tcenv_FnEnd_j){ goto _LL449;} else{
goto _LL450;} _LL450: if( _temp442 ==( void*) Cyc_Tcenv_CaseEnd_j){ goto _LL451;}
else{ goto _LL443;} _LL445: _temp452->non_local_preds=({ struct Cyc_List_List*
_temp454=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp454->hd=( void*) s; _temp454->tl= _temp452->non_local_preds; _temp454;});*
sopt=( struct Cyc_Absyn_Stmt*) _temp452; return; _LL447: Cyc_Tcutil_terr( s->loc,(
struct _tagged_string)({ char* _temp455=( char*)"break not in a loop or switch";
struct _tagged_string _temp456; _temp456.curr= _temp455; _temp456.base= _temp455;
_temp456.last_plus_one= _temp455 + 30; _temp456;})); return; _LL449: if(( void*)(
fe->shared)->return_typ !=( void*) Cyc_Absyn_VoidType){ Cyc_Tcutil_terr( s->loc,(
struct _tagged_string)({ char* _temp457=( char*)"break causes function not to return a value";
struct _tagged_string _temp458; _temp458.curr= _temp457; _temp458.base= _temp457;
_temp458.last_plus_one= _temp457 + 44; _temp458;}));} return; _LL451: Cyc_Tcutil_terr(
s->loc,( struct _tagged_string)({ char* _temp459=( char*)"break causes outer switch clause to implicitly fallthru";
struct _tagged_string _temp460; _temp460.curr= _temp459; _temp460.base= _temp459;
_temp460.last_plus_one= _temp459 + 56; _temp460;})); return; _LL443:;}} void Cyc_Tcenv_process_goto(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s, struct _tagged_string* l,
struct Cyc_Absyn_Stmt** sopt){ if( te->le == 0){(( void(*)( struct
_tagged_string msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char*
_temp461=( char*)"Tcenv: unexpected process_goto"; struct _tagged_string
_temp462; _temp462.curr= _temp461; _temp462.base= _temp461; _temp462.last_plus_one=
_temp461 + 31; _temp462;}));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_get_fenv( te);
struct Cyc_Core_Opt* sopt2=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key)) Cyc_Dict_lookup_opt)(( fe->shared)->seen_labels, l);
if( sopt2 == 0){ struct Cyc_SlowDict_Dict* needed=( fe->shared)->needed_labels;
struct Cyc_Core_Opt* slopt=(( struct Cyc_Core_Opt*(*)( struct Cyc_SlowDict_Dict*
d, struct _tagged_string* key)) Cyc_SlowDict_lookup_opt)( needed, l); if( slopt
== 0){ slopt=({ struct Cyc_Core_Opt* _temp463=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp463->v=( void*) 0; _temp463;});}( fe->shared)->needed_labels=((
struct Cyc_SlowDict_Dict*(*)( struct Cyc_SlowDict_Dict* d, struct _tagged_string*
key, struct Cyc_List_List* data)) Cyc_SlowDict_insert)( needed, l,({ struct Cyc_List_List*
_temp464=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp464->hd=( void*) s; _temp464->tl=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp465= slopt; if( _temp465 == 0){ _throw( Null_Exception);} _temp465->v;});
_temp464;}));} else{(( struct Cyc_Absyn_Stmt*)({ struct Cyc_Core_Opt* _temp466=
sopt2; if( _temp466 == 0){ _throw( Null_Exception);} _temp466->v;}))->non_local_preds=({
struct Cyc_List_List* _temp467=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp467->hd=( void*) s; _temp467->tl=(( struct Cyc_Absyn_Stmt*)({
struct Cyc_Core_Opt* _temp468= sopt2; if( _temp468 == 0){ _throw( Null_Exception);}
_temp468->v;}))->non_local_preds; _temp467;});* sopt=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Stmt*)({ struct Cyc_Core_Opt* _temp469= sopt2; if( _temp469 ==
0){ _throw( Null_Exception);} _temp469->v;}));}}} struct _tuple3* Cyc_Tcenv_process_fallthru(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Switch_clause***
clauseopt){ if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp470=( char*)"Tcenv: unexpected process_fallthru";
struct _tagged_string _temp471; _temp471.curr= _temp470; _temp471.base= _temp470;
_temp471.last_plus_one= _temp470 + 35; _temp471;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_get_fenv( te); struct _tuple3* ans=( fe->ctrl_env)->fallthru_clause;
if( ans != 0){((({ struct _tuple3* _temp472= ans; if( _temp472 == 0){ _throw(
Null_Exception);}* _temp472;}).f1)->body)->non_local_preds=({ struct Cyc_List_List*
_temp473=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp473->hd=( void*) s; _temp473->tl=((({ struct _tuple3* _temp474= ans; if(
_temp474 == 0){ _throw( Null_Exception);}* _temp474;}).f1)->body)->non_local_preds;
_temp473;});* clauseopt=({ struct Cyc_Absyn_Switch_clause** _temp475=( struct
Cyc_Absyn_Switch_clause**) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause*) *
1); _temp475[ 0]=({ struct _tuple3* _temp476= ans; if( _temp476 == 0){ _throw(
Null_Exception);}* _temp476;}).f1; _temp475;});} return ans;}} struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_set_fallthru( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* new_tvs,
struct Cyc_List_List* vds, struct Cyc_Absyn_Switch_clause* clause){ if( te->le
== 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(( struct
_tagged_string)({ char* _temp477=( char*)"Tcenv: unexpected set_fallthru";
struct _tagged_string _temp478; _temp478.curr= _temp477; _temp478.base= _temp477;
_temp478.last_plus_one= _temp477 + 31; _temp478;}));}{ struct Cyc_List_List*
ft_typ= 0; for( 0; vds != 0; vds=({ struct Cyc_List_List* _temp479= vds; if(
_temp479 == 0){ _throw( Null_Exception);} _temp479->tl;})){ ft_typ=({ struct Cyc_List_List*
_temp480=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp480->hd=( void*)(( void*)(( struct Cyc_Absyn_Vardecl*)({ struct Cyc_List_List*
_temp481= vds; if( _temp481 == 0){ _throw( Null_Exception);} _temp481->hd;}))->type);
_temp480->tl= ft_typ; _temp480;});} ft_typ=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( ft_typ);{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_new_ctrl(
Cyc_Tcenv_get_fenv( te));( fe->ctrl_env)->fallthru_clause=({ struct _tuple3*
_temp482=( struct _tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp482->f1=
clause; _temp482->f2= new_tvs; _temp482->f3= ft_typ; _temp482;}); return Cyc_Tcenv_put_fenv(
te, fe);}}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_clear_fallthru( struct Cyc_Tcenv_Tenv*
te){ if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp483=( char*)"Tcenv: unexpected clear_fallthru";
struct _tagged_string _temp484; _temp484.curr= _temp483; _temp484.base= _temp483;
_temp484.last_plus_one= _temp483 + 33; _temp484;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_new_ctrl( Cyc_Tcenv_get_fenv( te));( fe->ctrl_env)->fallthru_clause=
0; return Cyc_Tcenv_put_fenv( te, fe);}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_in_loop(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* continue_dest){ if( te->le ==
0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(( struct
_tagged_string)({ char* _temp485=( char*)"Tcenv: unexpected set_in_loop"; struct
_tagged_string _temp486; _temp486.curr= _temp485; _temp486.base= _temp485;
_temp486.last_plus_one= _temp485 + 30; _temp486;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_new_ctrl( Cyc_Tcenv_get_fenv( te));( void*)(( fe->ctrl_env)->continue_stmt=(
void*)(( void*)({ struct Cyc_Tcenv_Stmt_j_struct* _temp487=( struct Cyc_Tcenv_Stmt_j_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Stmt_j_struct)); _temp487[ 0]=({ struct Cyc_Tcenv_Stmt_j_struct
_temp488; _temp488.tag= Cyc_Tcenv_Stmt_j; _temp488.f1= continue_dest; _temp488;});
_temp487;})));( void*)(( fe->ctrl_env)->break_stmt=( void*)(( void*)( fe->ctrl_env)->next_stmt));(
void*)(( fe->ctrl_env)->next_stmt=( void*)(( void*)({ struct Cyc_Tcenv_Stmt_j_struct*
_temp489=( struct Cyc_Tcenv_Stmt_j_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Stmt_j_struct));
_temp489[ 0]=({ struct Cyc_Tcenv_Stmt_j_struct _temp490; _temp490.tag= Cyc_Tcenv_Stmt_j;
_temp490.f1= continue_dest; _temp490;}); _temp489;}))); return Cyc_Tcenv_put_fenv(
te, fe);}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_enter_try( struct Cyc_Tcenv_Tenv* te){
if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp491=( char*)"Tcenv: unexpected enter_try";
struct _tagged_string _temp492; _temp492.curr= _temp491; _temp492.base= _temp491;
_temp492.last_plus_one= _temp491 + 28; _temp492;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_new_ctrl( Cyc_Tcenv_get_fenv( te)); ++( fe->ctrl_env)->try_depth;
return Cyc_Tcenv_put_fenv( te, fe);}} int Cyc_Tcenv_get_try_depth( struct Cyc_Tcenv_Tenv*
te){ if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp493=( char*)"Tcenv: unexpected get_try_depth";
struct _tagged_string _temp494; _temp494.curr= _temp493; _temp494.base= _temp493;
_temp494.last_plus_one= _temp493 + 32; _temp494;}));} return(( Cyc_Tcenv_get_fenv(
te))->ctrl_env)->try_depth;} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_in_switch(
struct Cyc_Tcenv_Tenv* te){ if( te->le == 0){(( void(*)( struct _tagged_string
msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char* _temp495=( char*)"Tcenv: unexpected set_in_switch";
struct _tagged_string _temp496; _temp496.curr= _temp495; _temp496.base= _temp495;
_temp496.last_plus_one= _temp495 + 32; _temp496;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_new_ctrl( Cyc_Tcenv_get_fenv( te));( void*)(( fe->ctrl_env)->break_stmt=(
void*)(( void*)( fe->ctrl_env)->next_stmt));( void*)(( fe->ctrl_env)->next_stmt=(
void*)(( void*) Cyc_Tcenv_CaseEnd_j)); return Cyc_Tcenv_put_fenv( te, fe);}}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_next( struct Cyc_Tcenv_Tenv* te, void* j){
if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp497=( char*)"Tcenv: unexpected set_next";
struct _tagged_string _temp498; _temp498.curr= _temp497; _temp498.base= _temp497;
_temp498.last_plus_one= _temp497 + 27; _temp498;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_new_ctrl( Cyc_Tcenv_get_fenv( te));( void*)(( fe->ctrl_env)->next_stmt=(
void*) j); return Cyc_Tcenv_put_fenv( te, fe);}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_label(
struct Cyc_Tcenv_Tenv* te, struct _tagged_string* v, struct Cyc_Absyn_Stmt* s){
if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp499=( char*)"Tcenv: unexpected add_label";
struct _tagged_string _temp500; _temp500.curr= _temp499; _temp500.base= _temp499;
_temp500.last_plus_one= _temp499 + 28; _temp500;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_get_fenv( te); struct Cyc_SlowDict_Dict* needed=( fe->shared)->needed_labels;
struct Cyc_Core_Opt* sl_opt=(( struct Cyc_Core_Opt*(*)( struct Cyc_SlowDict_Dict*
d, struct _tagged_string* key)) Cyc_SlowDict_lookup_opt)( needed, v); if( sl_opt
!= 0){( fe->shared)->needed_labels=(( struct Cyc_SlowDict_Dict*(*)( struct Cyc_SlowDict_Dict*
d, struct _tagged_string* key)) Cyc_SlowDict_delete)( needed, v);{ struct Cyc_List_List*
sl=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp516= sl_opt; if(
_temp516 == 0){ _throw( Null_Exception);} _temp516->v;}); s->non_local_preds= sl;
for( 0; sl != 0; sl=({ struct Cyc_List_List* _temp501= sl; if( _temp501 == 0){
_throw( Null_Exception);} _temp501->tl;})){ void* _temp503=( void*)(( struct Cyc_Absyn_Stmt*)({
struct Cyc_List_List* _temp502= sl; if( _temp502 == 0){ _throw( Null_Exception);}
_temp502->hd;}))->r; struct Cyc_Absyn_Stmt* _temp509; struct Cyc_Absyn_Stmt**
_temp511; struct _tagged_string* _temp512; _LL505: if(( unsigned int) _temp503 >
1u?*(( int*) _temp503) == Cyc_Absyn_Goto_s: 0){ _LL513: _temp512=( struct
_tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp503)->f1; goto _LL510;
_LL510: _temp509=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp503)->f2; _temp511=&(( struct Cyc_Absyn_Goto_s_struct*) _temp503)->f2; goto
_LL506;} else{ goto _LL507;} _LL507: goto _LL508; _LL506:* _temp511=( struct Cyc_Absyn_Stmt*)
s; goto _LL504; _LL508:(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp514=( char*)"Tcenv: add_label backpatching of non-goto";
struct _tagged_string _temp515; _temp515.curr= _temp514; _temp515.base= _temp514;
_temp515.last_plus_one= _temp514 + 42; _temp515;})); goto _LL504; _LL504:;}}}
if((( int(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_member)((
fe->shared)->seen_labels, v)){ Cyc_Tcutil_terr( s->loc,({ struct _tagged_string
_temp517=* v; xprintf("Repeated label: %.*s", _temp517.last_plus_one - _temp517.curr,
_temp517.curr);}));}( fe->shared)->seen_labels=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Stmt* data))
Cyc_Dict_insert)(( fe->shared)->seen_labels, v, s); return te;}} int Cyc_Tcenv_all_labels_resolved(
struct Cyc_Tcenv_Tenv* te){ return(( int(*)( struct Cyc_SlowDict_Dict* d)) Cyc_SlowDict_is_empty)(((
Cyc_Tcenv_get_fenv( te))->shared)->needed_labels);} struct Cyc_Absyn_Stmt* Cyc_Tcenv_get_encloser(
struct Cyc_Tcenv_Tenv* te){ if( te->le == 0){(( void(*)( struct _tagged_string
msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char* _temp518=( char*)"Tcenv: unexpected get_encloser";
struct _tagged_string _temp519; _temp519.curr= _temp518; _temp519.base= _temp518;
_temp519.last_plus_one= _temp518 + 31; _temp519;}));} return( Cyc_Tcenv_get_fenv(
te))->encloser;} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_encloser( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Stmt* s){ if( te->le == 0){(( void(*)( struct
_tagged_string msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char*
_temp520=( char*)"Tcenv: unexpected set_encloser"; struct _tagged_string
_temp521; _temp521.curr= _temp520; _temp521.base= _temp520; _temp521.last_plus_one=
_temp520 + 31; _temp521;}));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); fe->encloser= s; return Cyc_Tcenv_put_fenv( te, fe);}}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_region( struct Cyc_Tcenv_Tenv* te, void* r){
if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp522=( char*)"Tcenv::add_region: unexpected add_region";
struct _tagged_string _temp523; _temp523.curr= _temp522; _temp523.base= _temp522;
_temp523.last_plus_one= _temp522 + 41; _temp523;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_old_ctrl( Cyc_Tcenv_get_fenv( te));( void*)( fe->capability=(
void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp524=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp524[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp525; _temp525.tag= Cyc_Absyn_JoinEff; _temp525.f1=({ struct Cyc_List_List*
_temp526=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp526->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp528=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp528[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp529; _temp529.tag= Cyc_Absyn_AccessEff;
_temp529.f1=( void*) r; _temp529;}); _temp528;})); _temp526->tl=({ struct Cyc_List_List*
_temp527=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp527->hd=( void*)(( void*) fe->capability); _temp527->tl= 0; _temp527;});
_temp526;}); _temp525;}); _temp524;}))); return Cyc_Tcenv_put_fenv( te, fe);}}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_new_named_block( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Tvar* block_rgn){ if( te->le ==
0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(( struct
_tagged_string)({ char* _temp530=( char*)"Tcenv: unexpected new_block"; struct
_tagged_string _temp531; _temp531.curr= _temp530; _temp531.base= _temp530;
_temp531.last_plus_one= _temp530 + 28; _temp531;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_old_ctrl( Cyc_Tcenv_get_fenv( te)); void* block_typ=( void*)({
struct Cyc_Absyn_VarType_struct* _temp539=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp539[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp540; _temp540.tag= Cyc_Absyn_VarType; _temp540.f1= block_rgn; _temp540;});
_temp539;}); fe->type_vars=({ struct Cyc_List_List* _temp532=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp532->hd=( void*) block_rgn;
_temp532->tl= fe->type_vars; _temp532;}); Cyc_Tcutil_check_unique_tvars( loc, fe->type_vars);(
void*)( fe->capability=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp533=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp533[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp534; _temp534.tag= Cyc_Absyn_JoinEff;
_temp534.f1=({ struct Cyc_List_List* _temp535=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp535->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp537=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp537[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp538; _temp538.tag= Cyc_Absyn_AccessEff;
_temp538.f1=( void*) block_typ; _temp538;}); _temp537;})); _temp535->tl=({
struct Cyc_List_List* _temp536=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp536->hd=( void*)(( void*) fe->capability); _temp536->tl=
0; _temp536;}); _temp535;}); _temp534;}); _temp533;})));( void*)( fe->curr_rgn=(
void*) block_typ); return Cyc_Tcenv_put_fenv( te, fe);}} struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_new_block( struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te){
return Cyc_Tcenv_new_named_block( loc, te, Cyc_Tcutil_new_tvar(( void*) Cyc_Absyn_RgnKind));}
void* Cyc_Tcenv_curr_rgn( struct Cyc_Tcenv_Tenv* te){ if( te->le == 0){ return(
void*) Cyc_Absyn_HeapRgn;} return( void*)( Cyc_Tcenv_get_fenv( te))->curr_rgn;}
void Cyc_Tcenv_check_rgn_accessible( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void* rgn){ if( te->le != 0){ void* capability=( void*)( Cyc_Tcenv_get_fenv(
te))->capability; if( Cyc_Tcutil_region_in_effect( 0, rgn, capability)? 1: Cyc_Tcutil_region_in_effect(
1, rgn, capability)){ return;}} Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp541= Cyc_Absynpp_typ2string( rgn); xprintf("Expression accesses unavailable region %.*s",
_temp541.last_plus_one - _temp541.curr, _temp541.curr);}));} void Cyc_Tcenv_check_effect_accessible(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* eff){ void*
capability; if( te->le == 0){ capability=( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp542=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp542[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp543; _temp543.tag= Cyc_Absyn_JoinEff;
_temp543.f1= 0; _temp543;}); _temp542;});} else{ struct Cyc_Tcenv_Fenv* fenv=
Cyc_Tcenv_get_fenv( te); capability=( void*) fenv->capability;} if( ! Cyc_Tcutil_subset_effect(
1, eff, capability)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp544=
Cyc_Absynpp_typ2string( capability); struct _tagged_string _temp545= Cyc_Absynpp_typ2string(
eff); xprintf("Capability %.*s does not cover function's effect %.*s", _temp544.last_plus_one
- _temp544.curr, _temp544.curr, _temp545.last_plus_one - _temp545.curr, _temp545.curr);}));}}
struct _tuple5{ struct _tagged_string* f1; struct Cyc_Absyn_Tqual f2; void* f3;
} ; struct Cyc_Tcenv_Fenv* Cyc_Tcenv_new_fenv( struct Cyc_Position_Segment* loc,
struct Cyc_Absyn_Fndecl* fd){ struct Cyc_Dict_Dict* locals=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); struct Cyc_Absyn_Tvar* rgn0=({ struct Cyc_Absyn_Tvar*
_temp576=( struct Cyc_Absyn_Tvar*) GC_malloc( sizeof( struct Cyc_Absyn_Tvar));
_temp576->name=({ struct _tagged_string* _temp577=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp577[ 0]=({ struct
_tagged_string _temp578=*(* fd->name).f2; xprintf("`%.*s", _temp578.last_plus_one
- _temp578.curr, _temp578.curr);}); _temp577;}); _temp576->kind=(( struct Cyc_Absyn_Conref*(*)(
void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind); _temp576;}); struct
Cyc_List_List* tvs=({ struct Cyc_List_List* _temp575=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp575->hd=( void*) rgn0; _temp575->tl=
fd->tvs; _temp575;}); Cyc_Tcutil_check_unique_tvars( loc, tvs);{ void* param_rgn=(
void*)({ struct Cyc_Absyn_VarType_struct* _temp573=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp573[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp574; _temp574.tag= Cyc_Absyn_VarType; _temp574.f1= rgn0; _temp574;});
_temp573;}); struct Cyc_List_List* vds= 0;{ struct Cyc_List_List* args= fd->args;
for( 0; args != 0; args=({ struct Cyc_List_List* _temp546= args; if( _temp546 ==
0){ _throw( Null_Exception);} _temp546->tl;})){ struct Cyc_Absyn_Vardecl* vd=({
struct Cyc_Absyn_Vardecl* _temp552=( struct Cyc_Absyn_Vardecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Vardecl) * 1); _temp552[ 0]=({ struct Cyc_Absyn_Vardecl
_temp553; _temp553.sc=( void*)(( void*) Cyc_Absyn_Public); _temp553.name=({
struct _tuple0* _temp558=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0) *
1); _temp558[ 0]=({ struct _tuple0 _temp559; _temp559.f1=( void*) Cyc_Absyn_Loc_n;
_temp559.f2=(*(( struct _tuple5*)({ struct Cyc_List_List* _temp560= args; if(
_temp560 == 0){ _throw( Null_Exception);} _temp560->hd;}))).f1; _temp559;});
_temp558;}); _temp553.tq=(*(( struct _tuple5*)({ struct Cyc_List_List* _temp557=
args; if( _temp557 == 0){ _throw( Null_Exception);} _temp557->hd;}))).f2;
_temp553.type=( void*)(*(( struct _tuple5*)({ struct Cyc_List_List* _temp556=
args; if( _temp556 == 0){ _throw( Null_Exception);} _temp556->hd;}))).f3;
_temp553.initializer= 0; _temp553.rgn=({ struct Cyc_Core_Opt* _temp554=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt) * 1); _temp554[ 0]=({
struct Cyc_Core_Opt _temp555; _temp555.v=( void*) param_rgn; _temp555;});
_temp554;}); _temp553.attributes= 0; _temp553;}); _temp552;}); vds=({ struct Cyc_List_List*
_temp547=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp547[ 0]=({ struct Cyc_List_List _temp548; _temp548.hd=( void*) vd; _temp548.tl=
vds; _temp548;}); _temp547;}); locals=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, void* data)) Cyc_Dict_insert)( locals,(*(( struct
_tuple5*)({ struct Cyc_List_List* _temp549= args; if( _temp549 == 0){ _throw(
Null_Exception);} _temp549->hd;}))).f1,( void*)({ struct Cyc_Absyn_Param_b_struct*
_temp550=( struct Cyc_Absyn_Param_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Param_b_struct));
_temp550[ 0]=({ struct Cyc_Absyn_Param_b_struct _temp551; _temp551.tag= Cyc_Absyn_Param_b;
_temp551.f1= vd; _temp551;}); _temp550;}));}} fd->param_vardecls=({ struct Cyc_Core_Opt*
_temp561=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt) * 1);
_temp561[ 0]=({ struct Cyc_Core_Opt _temp562; _temp562.v=( void*) vds; _temp562;});
_temp561;}); return({ struct Cyc_Tcenv_Fenv* _temp563=( struct Cyc_Tcenv_Fenv*)
GC_malloc( sizeof( struct Cyc_Tcenv_Fenv)); _temp563->shared=({ struct Cyc_Tcenv_SharedFenv*
_temp572=( struct Cyc_Tcenv_SharedFenv*) GC_malloc( sizeof( struct Cyc_Tcenv_SharedFenv));
_temp572->return_typ=( void*)(( void*) fd->ret_type); _temp572->seen_labels=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_string*, struct
_tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp572->needed_labels=((
struct Cyc_SlowDict_Dict*(*)( int(* comp)( struct _tagged_string*, struct
_tagged_string*))) Cyc_SlowDict_empty)( Cyc_String_zstrptrcmp); _temp572;});
_temp563->type_vars= tvs; _temp563->locals= locals; _temp563->encloser= fd->body;
_temp563->ctrl_env=({ struct Cyc_Tcenv_CtrlEnv* _temp571=( struct Cyc_Tcenv_CtrlEnv*)
GC_malloc( sizeof( struct Cyc_Tcenv_CtrlEnv)); _temp571->continue_stmt=( void*)((
void*) Cyc_Tcenv_NotLoop_j); _temp571->break_stmt=( void*)(( void*) Cyc_Tcenv_NotLoop_j);
_temp571->fallthru_clause= 0; _temp571->next_stmt=( void*)(( void*) Cyc_Tcenv_FnEnd_j);
_temp571->try_depth= 0; _temp571;}); _temp563->capability=( void*)(( void*)({
struct Cyc_Absyn_JoinEff_struct* _temp564=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp564[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp565; _temp565.tag= Cyc_Absyn_JoinEff; _temp565.f1=({ struct Cyc_List_List*
_temp566=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp566->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp569=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp569[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp570; _temp570.tag= Cyc_Absyn_AccessEff;
_temp570.f1=( void*) param_rgn; _temp570;}); _temp569;})); _temp566->tl=({
struct Cyc_List_List* _temp567=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp567->hd=( void*)(( void*)({ struct Cyc_Core_Opt*
_temp568= fd->effect; if( _temp568 == 0){ _throw( Null_Exception);} _temp568->v;}));
_temp567->tl= 0; _temp567;}); _temp566;}); _temp565;}); _temp564;})); _temp563->curr_rgn=(
void*) param_rgn; _temp563;});}}