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
struct Cyc_Core_Opt* Cyc_Core_opt_t; extern char Cyc_Core_InvalidArg[ 15u];
struct Cyc_Core_InvalidArg_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{ char* tag;
struct _tagged_string f1; } ; extern char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Core_Not_found[ 14u];
extern char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{ char*
tag; struct _tagged_string f1; } ; extern char* string_to_Cstring( struct
_tagged_string); extern char* underlying_Cstring( struct _tagged_string); extern
struct _tagged_string Cstring_to_string( char*); extern int system( char*);
struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE;
extern struct Cyc_Stdio___sFILE* Cyc_Stdio_stderr; typedef int Cyc_Stdio_fpos_t;
extern int Cyc_Stdio_fflush( struct Cyc_Stdio___sFILE*); extern char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1;
} ; extern char Cyc_Stdio_FileCloseError[ 19u]; struct Cyc_List_List{ void* hd;
struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List* Cyc_List_glist_t;
typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t;
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
f2; struct Cyc_Absyn_Uniondecl** f3; } ; static const int Cyc_Absyn_EnumType= 11;
struct Cyc_Absyn_EnumType_struct{ int tag; struct _tuple0* f1; struct Cyc_Absyn_Enumdecl*
f2; } ; static const int Cyc_Absyn_RgnHandleType= 12; struct Cyc_Absyn_RgnHandleType_struct{
int tag; void* f1; } ; static const int Cyc_Absyn_TypedefType= 13; struct Cyc_Absyn_TypedefType_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3;
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
struct Cyc_Absyn_Structdecl* f4; } ; static const int Cyc_Absyn_Tunion_e= 27;
struct Cyc_Absyn_Tunion_e_struct{ int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield*
f5; } ; static const int Cyc_Absyn_Enum_e= 28; struct Cyc_Absyn_Enum_e_struct{
int tag; struct _tuple0* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
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
struct _tuple0* name; struct Cyc_List_List* fields; } ; struct Cyc_Absyn_Typedefdecl{
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
_temp0.last_plus_one - _temp0.curr, _temp0.curr);}); Cyc_Stdio_fflush( Cyc_Stdio_stderr);(
void) _throw(( void*) Cyc_Tcenv_Env_error);} struct Cyc_Tcenv_Tenv; struct Cyc_Tcenv_Genv;
struct Cyc_Tcenv_CtrlEnv{ void* continue_stmt; void* break_stmt; struct _tuple3*
fallthru_clause; void* next_stmt; int try_depth; } ; typedef struct Cyc_Tcenv_CtrlEnv*
Cyc_Tcenv_ctrl_env_t; struct Cyc_Tcenv_SharedFenv{ void* return_typ; struct Cyc_Dict_Dict*
seen_labels; struct Cyc_SlowDict_Dict* needed_labels; } ; struct Cyc_Tcenv_Fenv{
struct Cyc_Tcenv_SharedFenv* shared; struct Cyc_List_List* type_vars; struct Cyc_Dict_Dict*
locals; struct Cyc_Absyn_Stmt* encloser; struct Cyc_Tcenv_CtrlEnv* ctrl_env;
void* capability; void* curr_rgn; } ; struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tc_init(){
Cyc_Tcutil_empty_var_set=({ struct Cyc_Core_Opt* _temp1=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1->v=( void*)(( struct Cyc_Set_Set*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Set_empty)(
Cyc_String_zstrptrcmp); _temp1;});{ struct Cyc_Tcenv_Genv* ge= Cyc_Tcenv_empty_genv();
struct Cyc_List_List* top_ns= 0; ge->tuniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Tuniondecl**
data)) Cyc_Dict_insert)( ge->tuniondecls,(* Cyc_Absyn_exn_name).f2,& Cyc_Absyn_exn_xed);{
struct Cyc_Dict_Dict* ae=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key, struct Cyc_Tcenv_Genv* data)) Cyc_Dict_insert)(((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct Cyc_List_List*, struct Cyc_List_List*)))
Cyc_Dict_empty)( Cyc_Absyn_varlist_cmp), top_ns, ge); return({ struct Cyc_Tcenv_Tenv*
_temp2=( struct Cyc_Tcenv_Tenv*) GC_malloc( sizeof( struct Cyc_Tcenv_Tenv));
_temp2->ns= top_ns; _temp2->ae= ae; _temp2->le= 0; _temp2;});}}} static struct
Cyc_Tcenv_Genv* Cyc_Tcenv_lookup_namespace( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, struct _tagged_string* n, struct Cyc_List_List* ns){
return(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)( te->ae, Cyc_Tcenv_resolve_namespace( te, loc, n, ns));}
static struct Cyc_List_List* Cyc_Tcenv_outer_namespace( struct Cyc_List_List* ns){
if( ns == 0){ return(( struct Cyc_List_List*(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp3=( char*)"Tcenv::outer_namespace"; struct
_tagged_string _temp4; _temp4.curr= _temp3; _temp4.base= _temp3; _temp4.last_plus_one=
_temp3 + 23; _temp4;}));} return(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)(({ struct Cyc_List_List* _temp5=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( ns); if( _temp5 == 0){ _throw(
Null_Exception);} _temp5->tl;}));} static void Cyc_Tcenv_check_repeat( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void*(* lookup)( struct
Cyc_Tcenv_Genv*, struct _tagged_string*), struct _tagged_string* v, struct Cyc_List_List*
nss){ for( 0; nss != 0; nss=({ struct Cyc_List_List* _temp6= nss; if( _temp6 ==
0){ _throw( Null_Exception);} _temp6->tl;})){ struct Cyc_Tcenv_Genv* ge2=((
struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key))
Cyc_Dict_lookup)( te->ae,( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp17= nss; if( _temp17 == 0){ _throw( Null_Exception);} _temp17->hd;}));
struct _handler_cons _temp7; _push_handler(& _temp7);{ void* _temp8=( void*)
setjmp( _temp7.handler); if( ! _temp8){ lookup( ge2, v); Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp9=* v; xprintf("%.*s is ambiguous", _temp9.last_plus_one
- _temp9.curr, _temp9.curr);}));; _pop_handler();} else{ void* _temp11= _temp8;
_LL13: if( _temp11 == Cyc_Dict_Absent){ goto _LL14;} else{ goto _LL15;} _LL15:
goto _LL16; _LL14: goto _LL12; _LL16:( void) _throw( _temp11); _LL12:;}}}
return;} static void* Cyc_Tcenv_scoped_lookup( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void*(* lookup)( struct Cyc_Tcenv_Genv*, struct
_tagged_string*), struct _tagged_string* v){ struct Cyc_List_List* cns= te->ns;
void* result; while( 1) { struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
cns);{ struct _handler_cons _temp18; _push_handler(& _temp18);{ void* _temp19=(
void*) setjmp( _temp18.handler); if( ! _temp19){ result= lookup( ge, v);(( void(*)(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void*(* lookup)(
struct Cyc_Tcenv_Genv*, struct _tagged_string*), struct _tagged_string* v,
struct Cyc_List_List* nss)) Cyc_Tcenv_check_repeat)( te, loc, lookup, v, ge->availables);{
void* _temp20= result; _npop_handler( 0u); return _temp20;}; _pop_handler();}
else{ void* _temp22= _temp19; _LL24: if( _temp22 == Cyc_Dict_Absent){ goto _LL25;}
else{ goto _LL26;} _LL26: goto _LL27; _LL25: goto _LL23; _LL27:( void) _throw(
_temp22); _LL23:;}}}{ struct Cyc_List_List* nss= ge->availables; for( 0; nss !=
0; nss=({ struct Cyc_List_List* _temp28= nss; if( _temp28 == 0){ _throw(
Null_Exception);} _temp28->tl;})){ struct Cyc_Tcenv_Genv* ge2=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp40= nss; if( _temp40 == 0){
_throw( Null_Exception);} _temp40->hd;})); struct _handler_cons _temp29;
_push_handler(& _temp29);{ void* _temp30=( void*) setjmp( _temp29.handler); if(
! _temp30){ result= lookup( ge2, v);(( void(*)( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void*(* lookup)( struct Cyc_Tcenv_Genv*,
struct _tagged_string*), struct _tagged_string* v, struct Cyc_List_List* nss))
Cyc_Tcenv_check_repeat)( te, loc, lookup, v,({ struct Cyc_List_List* _temp31=
nss; if( _temp31 == 0){ _throw( Null_Exception);} _temp31->tl;}));{ void*
_temp32= result; _npop_handler( 0u); return _temp32;}; _pop_handler();} else{
void* _temp34= _temp30; _LL36: if( _temp34 == Cyc_Dict_Absent){ goto _LL37;}
else{ goto _LL38;} _LL38: goto _LL39; _LL37: goto _LL35; _LL39:( void) _throw(
_temp34); _LL35:;}}}} if( cns == 0){( void) _throw(( void*) Cyc_Dict_Absent);}
cns= Cyc_Tcenv_outer_namespace( cns);}} struct _tuple4{ void* f1; int f2; } ;
static void* Cyc_Tcenv_lookup_ordinary_global_f( struct Cyc_Tcenv_Genv* ge,
struct _tagged_string* v){ struct _tuple4* ans=(( struct _tuple4*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->ordinaries, v);(* ans).f2=
1; return(* ans).f1;} static void* Cyc_Tcenv_lookup_ordinary_global( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0 _temp43;
struct _tagged_string* _temp44; void* _temp46; struct _tuple0* _temp41= q;
_temp43=* _temp41; _LL47: _temp46= _temp43.f1; goto _LL45; _LL45: _temp44=
_temp43.f2; goto _LL42; _LL42: { void* _temp48= _temp46; struct Cyc_List_List*
_temp58; struct Cyc_List_List* _temp60; struct Cyc_List_List _temp62; struct Cyc_List_List*
_temp63; struct _tagged_string* _temp65; struct Cyc_List_List* _temp67; _LL50:
if( _temp48 ==( void*) Cyc_Absyn_Loc_n){ goto _LL51;} else{ goto _LL52;} _LL52:
if(( unsigned int) _temp48 > 1u?*(( int*) _temp48) == Cyc_Absyn_Rel_n: 0){ _LL59:
_temp58=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp48)->f1;
if( _temp58 == 0){ goto _LL53;} else{ goto _LL54;}} else{ goto _LL54;} _LL54:
if(( unsigned int) _temp48 > 1u?*(( int*) _temp48) == Cyc_Absyn_Rel_n: 0){ _LL61:
_temp60=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp48)->f1;
if( _temp60 == 0){ goto _LL56;} else{ _temp62=* _temp60; _LL66: _temp65=( struct
_tagged_string*) _temp62.hd; goto _LL64; _LL64: _temp63=( struct Cyc_List_List*)
_temp62.tl; goto _LL55;}} else{ goto _LL56;} _LL56: if(( unsigned int) _temp48 >
1u?*(( int*) _temp48) == Cyc_Absyn_Abs_n: 0){ _LL68: _temp67=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp48)->f1; goto _LL57;} else{ goto _LL49;}
_LL51: goto _LL53; _LL53: return(( void*(*)( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void*(* lookup)( struct Cyc_Tcenv_Genv*, struct
_tagged_string*), struct _tagged_string* v)) Cyc_Tcenv_scoped_lookup)( te, loc,
Cyc_Tcenv_lookup_ordinary_global_f, _temp44); _LL55: { struct Cyc_Tcenv_Genv* ge=
Cyc_Tcenv_lookup_namespace( te, loc, _temp65, _temp63); return Cyc_Tcenv_lookup_ordinary_global_f(
ge, _temp44);} _LL57: return Cyc_Tcenv_lookup_ordinary_global_f((( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
_temp67), _temp44); _LL49:;}} struct Cyc_List_List* Cyc_Tcenv_resolve_namespace(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct
_tagged_string* n, struct Cyc_List_List* ns){ struct Cyc_List_List* cns= te->ns;
struct Cyc_List_List* possible_results= 0; while( 1) { struct Cyc_Tcenv_Genv* ge=((
struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key))
Cyc_Dict_lookup)( te->ae, cns);{ struct Cyc_List_List* nss= ge->availables; for(
0; nss != 0; nss=({ struct Cyc_List_List* _temp69= nss; if( _temp69 == 0){
_throw( Null_Exception);} _temp69->tl;})){ struct Cyc_Tcenv_Genv* ge2=(( struct
Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)(
te->ae,( struct Cyc_List_List*)({ struct Cyc_List_List* _temp72= nss; if(
_temp72 == 0){ _throw( Null_Exception);} _temp72->hd;})); if((( int(*)( struct
Cyc_Set_Set* s, struct _tagged_string* elt)) Cyc_Set_member)( ge2->namespaces, n)){
possible_results=({ struct Cyc_List_List* _temp70=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp70->hd=( void*)(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( cns,({
struct Cyc_List_List* _temp71=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp71->hd=( void*) n; _temp71->tl= ns; _temp71;})); _temp70->tl=
possible_results; _temp70;});}}} if((( int(*)( struct Cyc_Set_Set* s, struct
_tagged_string* elt)) Cyc_Set_member)( ge->namespaces, n)){ possible_results=({
struct Cyc_List_List* _temp73=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp73->hd=( void*)(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( cns,({ struct Cyc_List_List*
_temp74=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp74->hd=( void*) n; _temp74->tl= ns; _temp74;})); _temp73->tl=
possible_results; _temp73;});} if( possible_results != 0){ if(({ struct Cyc_List_List*
_temp75= possible_results; if( _temp75 == 0){ _throw( Null_Exception);} _temp75->tl;})
!= 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp76=* n; xprintf("%.*s is ambiguous",
_temp76.last_plus_one - _temp76.curr, _temp76.curr);}));} return( struct Cyc_List_List*)({
struct Cyc_List_List* _temp77= possible_results; if( _temp77 == 0){ _throw(
Null_Exception);} _temp77->hd;});} if( cns == 0){( void) _throw(( void*) Cyc_Dict_Absent);}
cns= Cyc_Tcenv_outer_namespace( cns);}} static struct Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_string* v){ return(( struct Cyc_Absyn_Structdecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->structdecls,
v);} struct Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0 _temp80;
struct _tagged_string* _temp81; void* _temp83; struct _tuple0* _temp78= q;
_temp80=* _temp78; _LL84: _temp83= _temp80.f1; goto _LL82; _LL82: _temp81=
_temp80.f2; goto _LL79; _LL79: { void* _temp85= _temp83; struct Cyc_List_List*
_temp95; struct Cyc_List_List* _temp97; struct Cyc_List_List* _temp99; struct
Cyc_List_List _temp101; struct Cyc_List_List* _temp102; struct _tagged_string*
_temp104; _LL87: if( _temp85 ==( void*) Cyc_Absyn_Loc_n){ goto _LL88;} else{
goto _LL89;} _LL89: if(( unsigned int) _temp85 > 1u?*(( int*) _temp85) == Cyc_Absyn_Rel_n:
0){ _LL96: _temp95=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp85)->f1; if( _temp95 == 0){ goto _LL90;} else{ goto _LL91;}} else{ goto
_LL91;} _LL91: if(( unsigned int) _temp85 > 1u?*(( int*) _temp85) == Cyc_Absyn_Abs_n:
0){ _LL98: _temp97=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp85)->f1; goto _LL92;} else{ goto _LL93;} _LL93: if(( unsigned int) _temp85
> 1u?*(( int*) _temp85) == Cyc_Absyn_Rel_n: 0){ _LL100: _temp99=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp85)->f1; if( _temp99 == 0){ goto _LL86;}
else{ _temp101=* _temp99; _LL105: _temp104=( struct _tagged_string*) _temp101.hd;
goto _LL103; _LL103: _temp102=( struct Cyc_List_List*) _temp101.tl; goto _LL94;}}
else{ goto _LL86;} _LL88: goto _LL90; _LL90: return(( struct Cyc_Absyn_Structdecl**(*)(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Structdecl**(*
lookup)( struct Cyc_Tcenv_Genv*, struct _tagged_string*), struct _tagged_string*
v)) Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_structdecl_f, _temp81);
_LL92: { struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, _temp97); return((
struct Cyc_Absyn_Structdecl**(*)( struct Cyc_Dict_Dict* d, struct _tagged_string*
key)) Cyc_Dict_lookup)( ge->structdecls, _temp81);} _LL94: { struct Cyc_Tcenv_Genv*
ge= Cyc_Tcenv_lookup_namespace( te, loc, _temp104, _temp102); return(( struct
Cyc_Absyn_Structdecl**(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key))
Cyc_Dict_lookup)( ge->structdecls, _temp81);} _LL86:;}} static struct Cyc_Absyn_Uniondecl**
Cyc_Tcenv_lookup_uniondecl_f( struct Cyc_Tcenv_Genv* ge, struct _tagged_string*
v){ return(( struct Cyc_Absyn_Uniondecl**(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup)( ge->uniondecls, v);} struct Cyc_Absyn_Uniondecl**
Cyc_Tcenv_lookup_uniondecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct _tuple0* q){ struct _tuple0 _temp108; struct _tagged_string*
_temp109; void* _temp111; struct _tuple0* _temp106= q; _temp108=* _temp106;
_LL112: _temp111= _temp108.f1; goto _LL110; _LL110: _temp109= _temp108.f2; goto
_LL107; _LL107: { void* _temp113= _temp111; struct Cyc_List_List* _temp123;
struct Cyc_List_List* _temp125; struct Cyc_List_List* _temp127; struct Cyc_List_List
_temp129; struct Cyc_List_List* _temp130; struct _tagged_string* _temp132;
_LL115: if( _temp113 ==( void*) Cyc_Absyn_Loc_n){ goto _LL116;} else{ goto
_LL117;} _LL117: if(( unsigned int) _temp113 > 1u?*(( int*) _temp113) == Cyc_Absyn_Rel_n:
0){ _LL124: _temp123=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp113)->f1; if( _temp123 == 0){ goto _LL118;} else{ goto _LL119;}} else{ goto
_LL119;} _LL119: if(( unsigned int) _temp113 > 1u?*(( int*) _temp113) == Cyc_Absyn_Abs_n:
0){ _LL126: _temp125=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp113)->f1; goto _LL120;} else{ goto _LL121;} _LL121: if(( unsigned int)
_temp113 > 1u?*(( int*) _temp113) == Cyc_Absyn_Rel_n: 0){ _LL128: _temp127=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp113)->f1; if(
_temp127 == 0){ goto _LL114;} else{ _temp129=* _temp127; _LL133: _temp132=(
struct _tagged_string*) _temp129.hd; goto _LL131; _LL131: _temp130=( struct Cyc_List_List*)
_temp129.tl; goto _LL122;}} else{ goto _LL114;} _LL116: goto _LL118; _LL118:
return(( struct Cyc_Absyn_Uniondecl**(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Uniondecl**(* lookup)( struct Cyc_Tcenv_Genv*, struct
_tagged_string*), struct _tagged_string* v)) Cyc_Tcenv_scoped_lookup)( te, loc,
Cyc_Tcenv_lookup_uniondecl_f, _temp109); _LL120: { struct Cyc_Tcenv_Genv* ge=((
struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key))
Cyc_Dict_lookup)( te->ae, _temp125); return(( struct Cyc_Absyn_Uniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->uniondecls,
_temp109);} _LL122: { struct Cyc_Tcenv_Genv* ge= Cyc_Tcenv_lookup_namespace( te,
loc, _temp132, _temp130); return(( struct Cyc_Absyn_Uniondecl**(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->uniondecls, _temp109);}
_LL114:;}} static struct Cyc_Absyn_Tuniondecl** Cyc_Tcenv_lookup_tuniondecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_string* v){ return(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->tuniondecls,
v);} struct Cyc_Absyn_Tuniondecl** Cyc_Tcenv_lookup_tuniondecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp136; struct _tagged_string* _temp137; void* _temp139; struct _tuple0*
_temp134= q; _temp136=* _temp134; _LL140: _temp139= _temp136.f1; goto _LL138;
_LL138: _temp137= _temp136.f2; goto _LL135; _LL135: { void* _temp141= _temp139;
struct Cyc_List_List* _temp151; struct Cyc_List_List* _temp153; struct Cyc_List_List
_temp155; struct Cyc_List_List* _temp156; struct _tagged_string* _temp158;
struct Cyc_List_List* _temp160; _LL143: if( _temp141 ==( void*) Cyc_Absyn_Loc_n){
goto _LL144;} else{ goto _LL145;} _LL145: if(( unsigned int) _temp141 > 1u?*((
int*) _temp141) == Cyc_Absyn_Rel_n: 0){ _LL152: _temp151=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp141)->f1; if( _temp151 == 0){ goto _LL146;}
else{ goto _LL147;}} else{ goto _LL147;} _LL147: if(( unsigned int) _temp141 > 1u?*((
int*) _temp141) == Cyc_Absyn_Rel_n: 0){ _LL154: _temp153=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp141)->f1; if( _temp153 == 0){ goto _LL149;}
else{ _temp155=* _temp153; _LL159: _temp158=( struct _tagged_string*) _temp155.hd;
goto _LL157; _LL157: _temp156=( struct Cyc_List_List*) _temp155.tl; goto _LL148;}}
else{ goto _LL149;} _LL149: if(( unsigned int) _temp141 > 1u?*(( int*) _temp141)
== Cyc_Absyn_Abs_n: 0){ _LL161: _temp160=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp141)->f1; goto _LL150;} else{ goto _LL142;} _LL144: goto _LL146; _LL146:
return(( struct Cyc_Absyn_Tuniondecl**(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Tuniondecl**(* lookup)( struct Cyc_Tcenv_Genv*, struct
_tagged_string*), struct _tagged_string* v)) Cyc_Tcenv_scoped_lookup)( te, loc,
Cyc_Tcenv_lookup_tuniondecl_f, _temp137); _LL148: { struct Cyc_Tcenv_Genv* ge=
Cyc_Tcenv_lookup_namespace( te, loc, _temp158, _temp156); return(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->tuniondecls,
_temp137);} _LL150: { struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
_temp160); return(( struct Cyc_Absyn_Tuniondecl**(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key)) Cyc_Dict_lookup)( ge->tuniondecls, _temp137);}
_LL142:;}} static struct Cyc_Absyn_Tuniondecl** Cyc_Tcenv_lookup_xtuniondecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_string* v){ return(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->tuniondecls,
v);} struct Cyc_Core_Opt* Cyc_Tcenv_lookup_xtuniondecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp164; struct _tagged_string* _temp165; void* _temp167; struct _tuple0*
_temp162= q; _temp164=* _temp162; _LL168: _temp167= _temp164.f1; goto _LL166;
_LL166: _temp165= _temp164.f2; goto _LL163; _LL163: { void* _temp169= _temp167;
struct Cyc_List_List* _temp179; struct Cyc_List_List* _temp181; struct Cyc_List_List
_temp183; struct Cyc_List_List* _temp184; struct _tagged_string* _temp186;
struct Cyc_List_List* _temp188; _LL171: if(( unsigned int) _temp169 > 1u?*(( int*)
_temp169) == Cyc_Absyn_Rel_n: 0){ _LL180: _temp179=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp169)->f1; if( _temp179 == 0){ goto _LL172;}
else{ goto _LL173;}} else{ goto _LL173;} _LL173: if( _temp169 ==( void*) Cyc_Absyn_Loc_n){
goto _LL174;} else{ goto _LL175;} _LL175: if(( unsigned int) _temp169 > 1u?*((
int*) _temp169) == Cyc_Absyn_Rel_n: 0){ _LL182: _temp181=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp169)->f1; if( _temp181 == 0){ goto _LL177;}
else{ _temp183=* _temp181; _LL187: _temp186=( struct _tagged_string*) _temp183.hd;
goto _LL185; _LL185: _temp184=( struct Cyc_List_List*) _temp183.tl; goto _LL176;}}
else{ goto _LL177;} _LL177: if(( unsigned int) _temp169 > 1u?*(( int*) _temp169)
== Cyc_Absyn_Abs_n: 0){ _LL189: _temp188=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp169)->f1; goto _LL178;} else{ goto _LL170;} _LL172: { struct _handler_cons
_temp190; _push_handler(& _temp190);{ void* _temp191=( void*) setjmp( _temp190.handler);
if( ! _temp191){{ struct Cyc_Core_Opt* _temp193=({ struct Cyc_Core_Opt* _temp192=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp192->v=(
void*)(( struct Cyc_Absyn_Tuniondecl**(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Tuniondecl**(* lookup)( struct Cyc_Tcenv_Genv*, struct
_tagged_string*), struct _tagged_string* v)) Cyc_Tcenv_scoped_lookup)( te, loc,
Cyc_Tcenv_lookup_xtuniondecl_f, _temp165); _temp192;}); _npop_handler( 0u);
return _temp193;}; _pop_handler();} else{ void* _temp195= _temp191; _LL197: if(
_temp195 == Cyc_Dict_Absent){ goto _LL198;} else{ goto _LL199;} _LL199: goto
_LL200; _LL198: return 0; _LL200:( void) _throw( _temp195); _LL196:;}}} _LL174:
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp201=( char*)"lookup_xtuniondecl: impossible";
struct _tagged_string _temp202; _temp202.curr= _temp201; _temp202.base= _temp201;
_temp202.last_plus_one= _temp201 + 31; _temp202;})); return 0; _LL176: { struct
Cyc_Tcenv_Genv* ge;{ struct _handler_cons _temp203; _push_handler(& _temp203);{
void* _temp204=( void*) setjmp( _temp203.handler); if( ! _temp204){ ge= Cyc_Tcenv_lookup_namespace(
te, loc, _temp186, _temp184);; _pop_handler();} else{ void* _temp206= _temp204;
_LL208: if( _temp206 == Cyc_Dict_Absent){ goto _LL209;} else{ goto _LL210;}
_LL210: goto _LL211; _LL209: Cyc_Tcutil_terr( loc,( struct _tagged_string)({
char* _temp212=( char*)"bad qualified name for xtunion"; struct _tagged_string
_temp213; _temp213.curr= _temp212; _temp213.base= _temp212; _temp213.last_plus_one=
_temp212 + 31; _temp213;}));( void) _throw(( void*) Cyc_Dict_Absent); _LL211:(
void) _throw( _temp206); _LL207:;}}} return({ struct Cyc_Core_Opt* _temp214=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp214->v=(
void*)(( struct Cyc_Absyn_Tuniondecl**(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup)( ge->tuniondecls, _temp165); _temp214;});}
_LL178: { struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, _temp188); return({
struct Cyc_Core_Opt* _temp215=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp215->v=( void*)(( struct Cyc_Absyn_Tuniondecl**(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->tuniondecls,
_temp165); _temp215;});} _LL170:;}} static struct Cyc_Absyn_Enumdecl** Cyc_Tcenv_lookup_enumdecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_string* v){ return(( struct Cyc_Absyn_Enumdecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->enumdecls,
v);} struct Cyc_Absyn_Enumdecl** Cyc_Tcenv_lookup_enumdecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp218; struct _tagged_string* _temp219; void* _temp221; struct _tuple0*
_temp216= q; _temp218=* _temp216; _LL222: _temp221= _temp218.f1; goto _LL220;
_LL220: _temp219= _temp218.f2; goto _LL217; _LL217: { void* _temp223= _temp221;
struct Cyc_List_List* _temp233; struct Cyc_List_List* _temp235; struct Cyc_List_List
_temp237; struct Cyc_List_List* _temp238; struct _tagged_string* _temp240;
struct Cyc_List_List* _temp242; _LL225: if( _temp223 ==( void*) Cyc_Absyn_Loc_n){
goto _LL226;} else{ goto _LL227;} _LL227: if(( unsigned int) _temp223 > 1u?*((
int*) _temp223) == Cyc_Absyn_Rel_n: 0){ _LL234: _temp233=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp223)->f1; if( _temp233 == 0){ goto _LL228;}
else{ goto _LL229;}} else{ goto _LL229;} _LL229: if(( unsigned int) _temp223 > 1u?*((
int*) _temp223) == Cyc_Absyn_Rel_n: 0){ _LL236: _temp235=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp223)->f1; if( _temp235 == 0){ goto _LL231;}
else{ _temp237=* _temp235; _LL241: _temp240=( struct _tagged_string*) _temp237.hd;
goto _LL239; _LL239: _temp238=( struct Cyc_List_List*) _temp237.tl; goto _LL230;}}
else{ goto _LL231;} _LL231: if(( unsigned int) _temp223 > 1u?*(( int*) _temp223)
== Cyc_Absyn_Abs_n: 0){ _LL243: _temp242=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp223)->f1; goto _LL232;} else{ goto _LL224;} _LL226: goto _LL228; _LL228:
return(( struct Cyc_Absyn_Enumdecl**(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Enumdecl**(* lookup)( struct Cyc_Tcenv_Genv*, struct
_tagged_string*), struct _tagged_string* v)) Cyc_Tcenv_scoped_lookup)( te, loc,
Cyc_Tcenv_lookup_enumdecl_f, _temp219); _LL230: { struct Cyc_Tcenv_Genv* ge= Cyc_Tcenv_lookup_namespace(
te, loc, _temp240, _temp238); return(( struct Cyc_Absyn_Enumdecl**(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->enumdecls,
_temp219);} _LL232: { struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
_temp242); return(( struct Cyc_Absyn_Enumdecl**(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key)) Cyc_Dict_lookup)( ge->enumdecls, _temp219);} _LL224:;}}
static struct Cyc_Absyn_Typedefdecl* Cyc_Tcenv_lookup_typedefdecl_f( struct Cyc_Tcenv_Genv*
ge, struct _tagged_string* v){ return(( struct Cyc_Absyn_Typedefdecl*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->typedefs, v);}
struct Cyc_Absyn_Typedefdecl* Cyc_Tcenv_lookup_typedefdecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp246; struct _tagged_string* _temp247; void* _temp249; struct _tuple0*
_temp244= q; _temp246=* _temp244; _LL250: _temp249= _temp246.f1; goto _LL248;
_LL248: _temp247= _temp246.f2; goto _LL245; _LL245: { void* _temp251= _temp249;
struct Cyc_List_List* _temp261; struct Cyc_List_List* _temp263; struct Cyc_List_List
_temp265; struct Cyc_List_List* _temp266; struct _tagged_string* _temp268;
struct Cyc_List_List* _temp270; _LL253: if( _temp251 ==( void*) Cyc_Absyn_Loc_n){
goto _LL254;} else{ goto _LL255;} _LL255: if(( unsigned int) _temp251 > 1u?*((
int*) _temp251) == Cyc_Absyn_Rel_n: 0){ _LL262: _temp261=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp251)->f1; if( _temp261 == 0){ goto _LL256;}
else{ goto _LL257;}} else{ goto _LL257;} _LL257: if(( unsigned int) _temp251 > 1u?*((
int*) _temp251) == Cyc_Absyn_Rel_n: 0){ _LL264: _temp263=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp251)->f1; if( _temp263 == 0){ goto _LL259;}
else{ _temp265=* _temp263; _LL269: _temp268=( struct _tagged_string*) _temp265.hd;
goto _LL267; _LL267: _temp266=( struct Cyc_List_List*) _temp265.tl; goto _LL258;}}
else{ goto _LL259;} _LL259: if(( unsigned int) _temp251 > 1u?*(( int*) _temp251)
== Cyc_Absyn_Abs_n: 0){ _LL271: _temp270=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp251)->f1; goto _LL260;} else{ goto _LL252;} _LL254: goto _LL256; _LL256:
return(( struct Cyc_Absyn_Typedefdecl*(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Typedefdecl*(* lookup)( struct Cyc_Tcenv_Genv*, struct
_tagged_string*), struct _tagged_string* v)) Cyc_Tcenv_scoped_lookup)( te, loc,
Cyc_Tcenv_lookup_typedefdecl_f, _temp247); _LL258: { struct Cyc_Tcenv_Genv* ge=
Cyc_Tcenv_lookup_namespace( te, loc, _temp268, _temp266); return(( struct Cyc_Absyn_Typedefdecl*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->typedefs,
_temp247);} _LL260: { struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
_temp270); return(( struct Cyc_Absyn_Typedefdecl*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key)) Cyc_Dict_lookup)( ge->typedefs, _temp247);} _LL252:;}}
struct Cyc_Tcenv_Genv* Cyc_Tcenv_empty_genv(){ return({ struct Cyc_Tcenv_Genv*
_temp272=( struct Cyc_Tcenv_Genv*) GC_malloc( sizeof( struct Cyc_Tcenv_Genv));
_temp272->namespaces=( struct Cyc_Set_Set*)({ struct Cyc_Core_Opt* _temp273= Cyc_Tcutil_empty_var_set;
if( _temp273 == 0){ _throw( Null_Exception);} _temp273->v;}); _temp272->structdecls=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_string*, struct
_tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp272->uniondecls=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_string*, struct
_tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp272->tuniondecls=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_string*, struct
_tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp272->enumdecls=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_string*, struct
_tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp272->typedefs=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_string*, struct
_tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp272->ordinaries=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_string*, struct
_tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp272->availables=
0; _temp272;});} void* Cyc_Tcenv_get_visible( void* f){ while( 1) { void*
_temp274= f; void* _temp282; void* _temp284; void* _temp286; void* _temp288;
void* _temp290; _LL276: if(*(( int*) _temp274) == Cyc_Tcenv_Outermost){ _LL283:
_temp282=( void*)(( struct Cyc_Tcenv_Outermost_struct*) _temp274)->f1; goto
_LL277;} else{ goto _LL278;} _LL278: if(*(( int*) _temp274) == Cyc_Tcenv_Frame){
_LL287: _temp286=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp274)->f1; goto
_LL285; _LL285: _temp284=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp274)->f2;
goto _LL279;} else{ goto _LL280;} _LL280: if(*(( int*) _temp274) == Cyc_Tcenv_Hidden){
_LL291: _temp290=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp274)->f1; goto
_LL289; _LL289: _temp288=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp274)->f2;
goto _LL281;} else{ goto _LL275;} _LL277: return _temp282; _LL279: return
_temp286; _LL281: f= _temp288; goto _LL275; _LL275:;}} void* Cyc_Tcenv_put_visible(
void* f, void* x){ void* _temp292= f; void* _temp300; void* _temp302; void*
_temp304; void* _temp306; void* _temp308; _LL294: if(*(( int*) _temp292) == Cyc_Tcenv_Outermost){
_LL301: _temp300=( void*)(( struct Cyc_Tcenv_Outermost_struct*) _temp292)->f1;
goto _LL295;} else{ goto _LL296;} _LL296: if(*(( int*) _temp292) == Cyc_Tcenv_Frame){
_LL305: _temp304=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp292)->f1; goto
_LL303; _LL303: _temp302=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp292)->f2;
goto _LL297;} else{ goto _LL298;} _LL298: if(*(( int*) _temp292) == Cyc_Tcenv_Hidden){
_LL309: _temp308=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp292)->f1; goto
_LL307; _LL307: _temp306=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp292)->f2;
goto _LL299;} else{ goto _LL293;} _LL295: return( void*)({ struct Cyc_Tcenv_Outermost_struct*
_temp310=( struct Cyc_Tcenv_Outermost_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Outermost_struct));
_temp310[ 0]=({ struct Cyc_Tcenv_Outermost_struct _temp311; _temp311.tag= Cyc_Tcenv_Outermost;
_temp311.f1=( void*) x; _temp311;}); _temp310;}); _LL297: return( void*)({
struct Cyc_Tcenv_Frame_struct* _temp312=( struct Cyc_Tcenv_Frame_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Frame_struct)); _temp312[ 0]=({ struct Cyc_Tcenv_Frame_struct
_temp313; _temp313.tag= Cyc_Tcenv_Frame; _temp313.f1=( void*) x; _temp313.f2=(
void*) _temp302; _temp313;}); _temp312;}); _LL299: return( void*)({ struct Cyc_Tcenv_Hidden_struct*
_temp314=( struct Cyc_Tcenv_Hidden_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Hidden_struct));
_temp314[ 0]=({ struct Cyc_Tcenv_Hidden_struct _temp315; _temp315.tag= Cyc_Tcenv_Hidden;
_temp315.f1=( void*) _temp308; _temp315.f2=( void*)(( void*(*)( void* f, void* x))
Cyc_Tcenv_put_visible)( _temp306, x); _temp315;}); _temp314;}); _LL293:;} void*
Cyc_Tcenv_apply_to_visible( void*(* g)( void*), void* f){ return(( void*(*)(
void* f, void* x)) Cyc_Tcenv_put_visible)( f, g((( void*(*)( void* f)) Cyc_Tcenv_get_visible)(
f)));} void* Cyc_Tcenv_map_frames( void*(* g)( void*), void* f){ void* _temp316=
f; void* _temp324; void* _temp326; void* _temp328; void* _temp330; void*
_temp332; _LL318: if(*(( int*) _temp316) == Cyc_Tcenv_Outermost){ _LL325:
_temp324=( void*)(( struct Cyc_Tcenv_Outermost_struct*) _temp316)->f1; goto
_LL319;} else{ goto _LL320;} _LL320: if(*(( int*) _temp316) == Cyc_Tcenv_Frame){
_LL329: _temp328=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp316)->f1; goto
_LL327; _LL327: _temp326=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp316)->f2;
goto _LL321;} else{ goto _LL322;} _LL322: if(*(( int*) _temp316) == Cyc_Tcenv_Hidden){
_LL333: _temp332=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp316)->f1; goto
_LL331; _LL331: _temp330=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp316)->f2;
goto _LL323;} else{ goto _LL317;} _LL319: return( void*)({ struct Cyc_Tcenv_Outermost_struct*
_temp334=( struct Cyc_Tcenv_Outermost_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Outermost_struct));
_temp334[ 0]=({ struct Cyc_Tcenv_Outermost_struct _temp335; _temp335.tag= Cyc_Tcenv_Outermost;
_temp335.f1=( void*) g( _temp324); _temp335;}); _temp334;}); _LL321: return(
void*)({ struct Cyc_Tcenv_Frame_struct* _temp336=( struct Cyc_Tcenv_Frame_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Frame_struct)); _temp336[ 0]=({ struct Cyc_Tcenv_Frame_struct
_temp337; _temp337.tag= Cyc_Tcenv_Frame; _temp337.f1=( void*) g( _temp328);
_temp337.f2=( void*)(( void*(*)( void*(* g)( void*), void* f)) Cyc_Tcenv_map_frames)(
g, _temp326); _temp337;}); _temp336;}); _LL323: return( void*)({ struct Cyc_Tcenv_Hidden_struct*
_temp338=( struct Cyc_Tcenv_Hidden_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Hidden_struct));
_temp338[ 0]=({ struct Cyc_Tcenv_Hidden_struct _temp339; _temp339.tag= Cyc_Tcenv_Hidden;
_temp339.f1=( void*) g( _temp332); _temp339.f2=( void*)(( void*(*)( void*(* g)(
void*), void* f)) Cyc_Tcenv_map_frames)( g, _temp330); _temp339;}); _temp338;});
_LL317:;} struct Cyc_Tcenv_Fenv* Cyc_Tcenv_get_fenv( struct Cyc_Tcenv_Tenv* te){
if( te->le == 0){( void) _throw(( void*) Cyc_Tcenv_Env_error);} return(( struct
Cyc_Tcenv_Fenv*(*)( void* f)) Cyc_Tcenv_get_visible)(( void*)({ struct Cyc_Core_Opt*
_temp340= te->le; if( _temp340 == 0){ _throw( Null_Exception);} _temp340->v;}));}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_put_fenv( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Fenv*
fe){ if( te->le == 0){( void) _throw(( void*) Cyc_Tcenv_Env_error);} return({
struct Cyc_Tcenv_Tenv* _temp341=( struct Cyc_Tcenv_Tenv*) GC_malloc( sizeof(
struct Cyc_Tcenv_Tenv)); _temp341->ns= te->ns; _temp341->ae= te->ae; _temp341->le=({
struct Cyc_Core_Opt* _temp342=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp342->v=( void*)(( void*(*)( void* f, struct Cyc_Tcenv_Fenv*
x)) Cyc_Tcenv_put_visible)(( void*)({ struct Cyc_Core_Opt* _temp343= te->le; if(
_temp343 == 0){ _throw( Null_Exception);} _temp343->v;}), fe); _temp342;});
_temp341;});} static struct Cyc_Tcenv_Fenv* Cyc_Tcenv_copy_fenv_old_ctrl( struct
Cyc_Tcenv_Fenv* f){ return({ struct Cyc_Tcenv_Fenv* _temp344=( struct Cyc_Tcenv_Fenv*)
GC_malloc( sizeof( struct Cyc_Tcenv_Fenv)); _temp344->shared= f->shared;
_temp344->type_vars= f->type_vars; _temp344->locals= f->locals; _temp344->encloser=
f->encloser; _temp344->ctrl_env= f->ctrl_env; _temp344->capability=( void*)((
void*) f->capability); _temp344->curr_rgn=( void*)(( void*) f->curr_rgn);
_temp344;});} static struct Cyc_Tcenv_Fenv* Cyc_Tcenv_copy_fenv_new_ctrl( struct
Cyc_Tcenv_Fenv* f){ struct Cyc_Tcenv_CtrlEnv* c= f->ctrl_env; return({ struct
Cyc_Tcenv_Fenv* _temp345=( struct Cyc_Tcenv_Fenv*) GC_malloc( sizeof( struct Cyc_Tcenv_Fenv));
_temp345->shared= f->shared; _temp345->type_vars= f->type_vars; _temp345->locals=
f->locals; _temp345->encloser= f->encloser; _temp345->ctrl_env=({ struct Cyc_Tcenv_CtrlEnv*
_temp346=( struct Cyc_Tcenv_CtrlEnv*) GC_malloc( sizeof( struct Cyc_Tcenv_CtrlEnv));
_temp346->continue_stmt=( void*)(( void*) c->continue_stmt); _temp346->break_stmt=(
void*)(( void*) c->break_stmt); _temp346->fallthru_clause= c->fallthru_clause;
_temp346->next_stmt=( void*)(( void*) c->next_stmt); _temp346->try_depth= c->try_depth;
_temp346;}); _temp345->capability=( void*)(( void*) f->capability); _temp345->curr_rgn=(
void*)(( void*) f->curr_rgn); _temp345;});} void* Cyc_Tcenv_return_typ( struct
Cyc_Tcenv_Tenv* te){ if( te->le == 0){(( void(*)( struct _tagged_string msg))
Cyc_Tcenv_env_err)(( struct _tagged_string)({ char* _temp347=( char*)"Tcenv: unexpected return_typ";
struct _tagged_string _temp348; _temp348.curr= _temp347; _temp348.base= _temp347;
_temp348.last_plus_one= _temp347 + 29; _temp348;}));} return( void*)(( Cyc_Tcenv_get_fenv(
te))->shared)->return_typ;} struct Cyc_List_List* Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv* te){ if( te->le == 0){ return 0;} return( Cyc_Tcenv_get_fenv(
te))->type_vars;} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_type_vars( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* tvs){ if( te->le == 0){((
void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({
char* _temp349=( char*)"Tcenv: unexpected add_type_vars"; struct _tagged_string
_temp350; _temp350.curr= _temp349; _temp350.base= _temp349; _temp350.last_plus_one=
_temp349 + 32; _temp350;}));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); struct Cyc_List_List* new_tvs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tvs, fe->type_vars);
Cyc_Tcutil_check_unique_tvars( loc, new_tvs); fe->type_vars= new_tvs; return Cyc_Tcenv_put_fenv(
te, fe);}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_local_var( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Vardecl* vd){ if( te->le == 0){((
void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({
char* _temp351=( char*)"Tcenv: unexpected add_local_var"; struct _tagged_string
_temp352; _temp352.curr= _temp351; _temp352.base= _temp351; _temp352.last_plus_one=
_temp351 + 32; _temp352;}));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); struct _tagged_string* v=(* vd->name).f2;{ void*
_temp353=(* vd->name).f1; _LL355: if( _temp353 ==( void*) Cyc_Absyn_Loc_n){ goto
_LL356;} else{ goto _LL357;} _LL357: goto _LL358; _LL356: goto _LL354; _LL358:(
void) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp359=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp359[ 0]=({ struct
Cyc_Core_Impossible_struct _temp360; _temp360.tag= Cyc_Core_Impossible; _temp360.f1=(
struct _tagged_string)({ char* _temp361=( char*)"add_local_var: called with Rel_n";
struct _tagged_string _temp362; _temp362.curr= _temp361; _temp362.base= _temp361;
_temp362.last_plus_one= _temp361 + 33; _temp362;}); _temp360;}); _temp359;}));
goto _LL354; _LL354:;} fe->locals=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, void* data)) Cyc_Dict_insert)( fe->locals, v,(
void*)({ struct Cyc_Absyn_Local_b_struct* _temp363=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp363[ 0]=({ struct Cyc_Absyn_Local_b_struct
_temp364; _temp364.tag= Cyc_Absyn_Local_b; _temp364.f1= vd; _temp364;});
_temp363;})); return Cyc_Tcenv_put_fenv( te, fe);}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_pat_var(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Vardecl*
vd){ if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp365=( char*)"Tcenv: unexpected add_pat_var";
struct _tagged_string _temp366; _temp366.curr= _temp365; _temp366.base= _temp365;
_temp366.last_plus_one= _temp365 + 30; _temp366;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_old_ctrl( Cyc_Tcenv_get_fenv( te)); struct _tagged_string* v=(*
vd->name).f2; fe->locals=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key, void* data)) Cyc_Dict_insert)( fe->locals, v,( void*)({
struct Cyc_Absyn_Pat_b_struct* _temp367=( struct Cyc_Absyn_Pat_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pat_b_struct)); _temp367[ 0]=({ struct Cyc_Absyn_Pat_b_struct
_temp368; _temp368.tag= Cyc_Absyn_Pat_b; _temp368.f1= vd; _temp368;}); _temp367;}));
return Cyc_Tcenv_put_fenv( te, fe);}} void* Cyc_Tcenv_lookup_ordinary( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct
_tuple0 _temp371; struct _tagged_string* _temp372; void* _temp374; struct
_tuple0* _temp369= q; _temp371=* _temp369; _LL375: _temp374= _temp371.f1; goto
_LL373; _LL373: _temp372= _temp371.f2; goto _LL370; _LL370: { void* _temp376=
_temp374; struct Cyc_List_List* _temp385; _LL378: if( _temp376 ==( void*) Cyc_Absyn_Loc_n){
goto _LL379;} else{ goto _LL380;} _LL380: if(( unsigned int) _temp376 > 1u?*((
int*) _temp376) == Cyc_Absyn_Rel_n: 0){ _LL386: _temp385=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp376)->f1; if( _temp385 == 0){ goto _LL384;}
else{ goto _LL382;}} else{ goto _LL382;} _LL384: if( te->le != 0){ goto _LL381;}
else{ goto _LL382;} _LL382: goto _LL383; _LL379: if( te->le == 0){( void) _throw((
void*) Cyc_Dict_Absent);} else{ goto _LL381;} _LL381: { struct Cyc_Tcenv_Fenv*
fe= Cyc_Tcenv_get_fenv( te); struct _handler_cons _temp387; _push_handler(&
_temp387);{ void* _temp388=( void*) setjmp( _temp387.handler); if( ! _temp388){{
void* _temp391=( void*)({ struct Cyc_Tcenv_VarRes_struct* _temp389=( struct Cyc_Tcenv_VarRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp389[ 0]=({ struct Cyc_Tcenv_VarRes_struct
_temp390; _temp390.tag= Cyc_Tcenv_VarRes; _temp390.f1=( void*)(( void*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( fe->locals,
_temp372); _temp390;}); _temp389;}); _npop_handler( 0u); return _temp391;};
_pop_handler();} else{ void* _temp393= _temp388; _LL395: if( _temp393 == Cyc_Dict_Absent){
goto _LL396;} else{ goto _LL397;} _LL397: goto _LL398; _LL396: return Cyc_Tcenv_lookup_ordinary_global(
te, loc, q); _LL398:( void) _throw( _temp393); _LL394:;}}} _LL383: { struct
_handler_cons _temp399; _push_handler(& _temp399);{ void* _temp400=( void*)
setjmp( _temp399.handler); if( ! _temp400){{ void* _temp401= Cyc_Tcenv_lookup_ordinary_global(
te, loc, q); _npop_handler( 0u); return _temp401;}; _pop_handler();} else{ void*
_temp403= _temp400; _LL405: if( _temp403 == Cyc_Dict_Absent){ goto _LL406;}
else{ goto _LL407;} _LL407: goto _LL408; _LL406: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp409= Cyc_Absynpp_qvar2string( q); xprintf("unbound identifer: %.*s",
_temp409.last_plus_one - _temp409.curr, _temp409.curr);})); return( void*)({
struct Cyc_Tcenv_VarRes_struct* _temp410=( struct Cyc_Tcenv_VarRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp410[ 0]=({ struct Cyc_Tcenv_VarRes_struct
_temp411; _temp411.tag= Cyc_Tcenv_VarRes; _temp411.f1=( void*)(( void*) Cyc_Absyn_Unresolved_b);
_temp411;}); _temp410;}); _LL408:( void) _throw( _temp403); _LL404:;}}} _LL377:;}}
void Cyc_Tcenv_process_continue( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt*
s, struct Cyc_Absyn_Stmt** sopt){ if( te->le == 0){(( void(*)( struct
_tagged_string msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char*
_temp412=( char*)"Tcenv: unexpected process_continue"; struct _tagged_string
_temp413; _temp413.curr= _temp412; _temp413.base= _temp412; _temp413.last_plus_one=
_temp412 + 35; _temp413;}));}{ void* _temp414=( void*)(( Cyc_Tcenv_get_fenv( te))->ctrl_env)->continue_stmt;
struct Cyc_Absyn_Stmt* _temp424; _LL416: if(( unsigned int) _temp414 > 3u?*((
int*) _temp414) == Cyc_Tcenv_Stmt_j: 0){ _LL425: _temp424=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Tcenv_Stmt_j_struct*) _temp414)->f1; goto _LL417;} else{ goto _LL418;}
_LL418: if( _temp414 ==( void*) Cyc_Tcenv_NotLoop_j){ goto _LL419;} else{ goto
_LL420;} _LL420: if( _temp414 ==( void*) Cyc_Tcenv_CaseEnd_j){ goto _LL421;}
else{ goto _LL422;} _LL422: if( _temp414 ==( void*) Cyc_Tcenv_FnEnd_j){ goto
_LL423;} else{ goto _LL415;} _LL417: _temp424->non_local_preds=({ struct Cyc_List_List*
_temp426=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp426->hd=( void*) s; _temp426->tl= _temp424->non_local_preds; _temp426;});*
sopt=( struct Cyc_Absyn_Stmt*) _temp424; return; _LL419: Cyc_Tcutil_terr( s->loc,(
struct _tagged_string)({ char* _temp427=( char*)"continue not in a loop"; struct
_tagged_string _temp428; _temp428.curr= _temp427; _temp428.base= _temp427;
_temp428.last_plus_one= _temp427 + 23; _temp428;})); return; _LL421:(( void(*)(
struct _tagged_string msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char*
_temp429=( char*)"Tcenv: continue dest is CaseEnd"; struct _tagged_string
_temp430; _temp430.curr= _temp429; _temp430.base= _temp429; _temp430.last_plus_one=
_temp429 + 32; _temp430;})); return; _LL423:(( void(*)( struct _tagged_string
msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char* _temp431=( char*)"Tcenv: continue dest is CaseEnd";
struct _tagged_string _temp432; _temp432.curr= _temp431; _temp432.base= _temp431;
_temp432.last_plus_one= _temp431 + 32; _temp432;})); return; _LL415:;}} void Cyc_Tcenv_process_break(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Stmt**
sopt){ if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp433=( char*)"Tcenv: unexpected process_break";
struct _tagged_string _temp434; _temp434.curr= _temp433; _temp434.base= _temp433;
_temp434.last_plus_one= _temp433 + 32; _temp434;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_get_fenv( te); void* _temp435=( void*)( fe->ctrl_env)->break_stmt;
struct Cyc_Absyn_Stmt* _temp445; _LL437: if(( unsigned int) _temp435 > 3u?*((
int*) _temp435) == Cyc_Tcenv_Stmt_j: 0){ _LL446: _temp445=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Tcenv_Stmt_j_struct*) _temp435)->f1; goto _LL438;} else{ goto _LL439;}
_LL439: if( _temp435 ==( void*) Cyc_Tcenv_NotLoop_j){ goto _LL440;} else{ goto
_LL441;} _LL441: if( _temp435 ==( void*) Cyc_Tcenv_FnEnd_j){ goto _LL442;} else{
goto _LL443;} _LL443: if( _temp435 ==( void*) Cyc_Tcenv_CaseEnd_j){ goto _LL444;}
else{ goto _LL436;} _LL438: _temp445->non_local_preds=({ struct Cyc_List_List*
_temp447=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp447->hd=( void*) s; _temp447->tl= _temp445->non_local_preds; _temp447;});*
sopt=( struct Cyc_Absyn_Stmt*) _temp445; return; _LL440: Cyc_Tcutil_terr( s->loc,(
struct _tagged_string)({ char* _temp448=( char*)"break not in a loop or switch";
struct _tagged_string _temp449; _temp449.curr= _temp448; _temp449.base= _temp448;
_temp449.last_plus_one= _temp448 + 30; _temp449;})); return; _LL442: if(( void*)(
fe->shared)->return_typ !=( void*) Cyc_Absyn_VoidType){ Cyc_Tcutil_terr( s->loc,(
struct _tagged_string)({ char* _temp450=( char*)"break causes function not to return a value";
struct _tagged_string _temp451; _temp451.curr= _temp450; _temp451.base= _temp450;
_temp451.last_plus_one= _temp450 + 44; _temp451;}));} return; _LL444: Cyc_Tcutil_terr(
s->loc,( struct _tagged_string)({ char* _temp452=( char*)"break causes outer switch clause to implicitly fallthru";
struct _tagged_string _temp453; _temp453.curr= _temp452; _temp453.base= _temp452;
_temp453.last_plus_one= _temp452 + 56; _temp453;})); return; _LL436:;}} void Cyc_Tcenv_process_goto(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s, struct _tagged_string* l,
struct Cyc_Absyn_Stmt** sopt){ if( te->le == 0){(( void(*)( struct
_tagged_string msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char*
_temp454=( char*)"Tcenv: unexpected process_goto"; struct _tagged_string
_temp455; _temp455.curr= _temp454; _temp455.base= _temp454; _temp455.last_plus_one=
_temp454 + 31; _temp455;}));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_get_fenv( te);
struct Cyc_Core_Opt* sopt2=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key)) Cyc_Dict_lookup_opt)(( fe->shared)->seen_labels, l);
if( sopt2 == 0){ struct Cyc_SlowDict_Dict* needed=( fe->shared)->needed_labels;
struct Cyc_Core_Opt* slopt=(( struct Cyc_Core_Opt*(*)( struct Cyc_SlowDict_Dict*
d, struct _tagged_string* key)) Cyc_SlowDict_lookup_opt)( needed, l); if( slopt
== 0){ slopt=({ struct Cyc_Core_Opt* _temp456=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp456->v=( void*) 0; _temp456;});}( fe->shared)->needed_labels=((
struct Cyc_SlowDict_Dict*(*)( struct Cyc_SlowDict_Dict* d, struct _tagged_string*
key, struct Cyc_List_List* data)) Cyc_SlowDict_insert)( needed, l,({ struct Cyc_List_List*
_temp457=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp457->hd=( void*) s; _temp457->tl=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp458= slopt; if( _temp458 == 0){ _throw( Null_Exception);} _temp458->v;});
_temp457;}));} else{(( struct Cyc_Absyn_Stmt*)({ struct Cyc_Core_Opt* _temp459=
sopt2; if( _temp459 == 0){ _throw( Null_Exception);} _temp459->v;}))->non_local_preds=({
struct Cyc_List_List* _temp460=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp460->hd=( void*) s; _temp460->tl=(( struct Cyc_Absyn_Stmt*)({
struct Cyc_Core_Opt* _temp461= sopt2; if( _temp461 == 0){ _throw( Null_Exception);}
_temp461->v;}))->non_local_preds; _temp460;});* sopt=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Stmt*)({ struct Cyc_Core_Opt* _temp462= sopt2; if( _temp462 ==
0){ _throw( Null_Exception);} _temp462->v;}));}}} struct _tuple3* Cyc_Tcenv_process_fallthru(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Switch_clause***
clauseopt){ if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp463=( char*)"Tcenv: unexpected process_fallthru";
struct _tagged_string _temp464; _temp464.curr= _temp463; _temp464.base= _temp463;
_temp464.last_plus_one= _temp463 + 35; _temp464;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_get_fenv( te); struct _tuple3* ans=( fe->ctrl_env)->fallthru_clause;
if( ans != 0){((({ struct _tuple3* _temp465= ans; if( _temp465 == 0){ _throw(
Null_Exception);}* _temp465;}).f1)->body)->non_local_preds=({ struct Cyc_List_List*
_temp466=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp466->hd=( void*) s; _temp466->tl=((({ struct _tuple3* _temp467= ans; if(
_temp467 == 0){ _throw( Null_Exception);}* _temp467;}).f1)->body)->non_local_preds;
_temp466;});* clauseopt=({ struct Cyc_Absyn_Switch_clause** _temp468=( struct
Cyc_Absyn_Switch_clause**) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause*) *
1); _temp468[ 0]=({ struct _tuple3* _temp469= ans; if( _temp469 == 0){ _throw(
Null_Exception);}* _temp469;}).f1; _temp468;});} return ans;}} struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_set_fallthru( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* new_tvs,
struct Cyc_List_List* vds, struct Cyc_Absyn_Switch_clause* clause){ if( te->le
== 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(( struct
_tagged_string)({ char* _temp470=( char*)"Tcenv: unexpected set_fallthru";
struct _tagged_string _temp471; _temp471.curr= _temp470; _temp471.base= _temp470;
_temp471.last_plus_one= _temp470 + 31; _temp471;}));}{ struct Cyc_List_List*
ft_typ= 0; for( 0; vds != 0; vds=({ struct Cyc_List_List* _temp472= vds; if(
_temp472 == 0){ _throw( Null_Exception);} _temp472->tl;})){ ft_typ=({ struct Cyc_List_List*
_temp473=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp473->hd=( void*)(( void*)(( struct Cyc_Absyn_Vardecl*)({ struct Cyc_List_List*
_temp474= vds; if( _temp474 == 0){ _throw( Null_Exception);} _temp474->hd;}))->type);
_temp473->tl= ft_typ; _temp473;});} ft_typ=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( ft_typ);{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_new_ctrl(
Cyc_Tcenv_get_fenv( te));( fe->ctrl_env)->fallthru_clause=({ struct _tuple3*
_temp475=( struct _tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp475->f1=
clause; _temp475->f2= new_tvs; _temp475->f3= ft_typ; _temp475;}); return Cyc_Tcenv_put_fenv(
te, fe);}}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_clear_fallthru( struct Cyc_Tcenv_Tenv*
te){ if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp476=( char*)"Tcenv: unexpected clear_fallthru";
struct _tagged_string _temp477; _temp477.curr= _temp476; _temp477.base= _temp476;
_temp477.last_plus_one= _temp476 + 33; _temp477;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_new_ctrl( Cyc_Tcenv_get_fenv( te));( fe->ctrl_env)->fallthru_clause=
0; return Cyc_Tcenv_put_fenv( te, fe);}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_in_loop(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* continue_dest){ if( te->le ==
0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(( struct
_tagged_string)({ char* _temp478=( char*)"Tcenv: unexpected set_in_loop"; struct
_tagged_string _temp479; _temp479.curr= _temp478; _temp479.base= _temp478;
_temp479.last_plus_one= _temp478 + 30; _temp479;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_new_ctrl( Cyc_Tcenv_get_fenv( te));( void*)(( fe->ctrl_env)->continue_stmt=(
void*)(( void*)({ struct Cyc_Tcenv_Stmt_j_struct* _temp480=( struct Cyc_Tcenv_Stmt_j_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Stmt_j_struct)); _temp480[ 0]=({ struct Cyc_Tcenv_Stmt_j_struct
_temp481; _temp481.tag= Cyc_Tcenv_Stmt_j; _temp481.f1= continue_dest; _temp481;});
_temp480;})));( void*)(( fe->ctrl_env)->break_stmt=( void*)(( void*)( fe->ctrl_env)->next_stmt));(
void*)(( fe->ctrl_env)->next_stmt=( void*)(( void*)({ struct Cyc_Tcenv_Stmt_j_struct*
_temp482=( struct Cyc_Tcenv_Stmt_j_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Stmt_j_struct));
_temp482[ 0]=({ struct Cyc_Tcenv_Stmt_j_struct _temp483; _temp483.tag= Cyc_Tcenv_Stmt_j;
_temp483.f1= continue_dest; _temp483;}); _temp482;}))); return Cyc_Tcenv_put_fenv(
te, fe);}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_enter_try( struct Cyc_Tcenv_Tenv* te){
if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp484=( char*)"Tcenv: unexpected enter_try";
struct _tagged_string _temp485; _temp485.curr= _temp484; _temp485.base= _temp484;
_temp485.last_plus_one= _temp484 + 28; _temp485;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_new_ctrl( Cyc_Tcenv_get_fenv( te)); ++( fe->ctrl_env)->try_depth;
return Cyc_Tcenv_put_fenv( te, fe);}} int Cyc_Tcenv_get_try_depth( struct Cyc_Tcenv_Tenv*
te){ if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp486=( char*)"Tcenv: unexpected get_try_depth";
struct _tagged_string _temp487; _temp487.curr= _temp486; _temp487.base= _temp486;
_temp487.last_plus_one= _temp486 + 32; _temp487;}));} return(( Cyc_Tcenv_get_fenv(
te))->ctrl_env)->try_depth;} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_in_switch(
struct Cyc_Tcenv_Tenv* te){ if( te->le == 0){(( void(*)( struct _tagged_string
msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char* _temp488=( char*)"Tcenv: unexpected set_in_switch";
struct _tagged_string _temp489; _temp489.curr= _temp488; _temp489.base= _temp488;
_temp489.last_plus_one= _temp488 + 32; _temp489;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_new_ctrl( Cyc_Tcenv_get_fenv( te));( void*)(( fe->ctrl_env)->break_stmt=(
void*)(( void*)( fe->ctrl_env)->next_stmt));( void*)(( fe->ctrl_env)->next_stmt=(
void*)(( void*) Cyc_Tcenv_CaseEnd_j)); return Cyc_Tcenv_put_fenv( te, fe);}}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_next( struct Cyc_Tcenv_Tenv* te, void* j){
if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp490=( char*)"Tcenv: unexpected set_next";
struct _tagged_string _temp491; _temp491.curr= _temp490; _temp491.base= _temp490;
_temp491.last_plus_one= _temp490 + 27; _temp491;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_new_ctrl( Cyc_Tcenv_get_fenv( te));( void*)(( fe->ctrl_env)->next_stmt=(
void*) j); return Cyc_Tcenv_put_fenv( te, fe);}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_label(
struct Cyc_Tcenv_Tenv* te, struct _tagged_string* v, struct Cyc_Absyn_Stmt* s){
if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp492=( char*)"Tcenv: unexpected add_label";
struct _tagged_string _temp493; _temp493.curr= _temp492; _temp493.base= _temp492;
_temp493.last_plus_one= _temp492 + 28; _temp493;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_get_fenv( te); struct Cyc_SlowDict_Dict* needed=( fe->shared)->needed_labels;
struct Cyc_Core_Opt* sl_opt=(( struct Cyc_Core_Opt*(*)( struct Cyc_SlowDict_Dict*
d, struct _tagged_string* key)) Cyc_SlowDict_lookup_opt)( needed, v); if( sl_opt
!= 0){( fe->shared)->needed_labels=(( struct Cyc_SlowDict_Dict*(*)( struct Cyc_SlowDict_Dict*
d, struct _tagged_string* key)) Cyc_SlowDict_delete)( needed, v);{ struct Cyc_List_List*
sl=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp509= sl_opt; if(
_temp509 == 0){ _throw( Null_Exception);} _temp509->v;}); s->non_local_preds= sl;
for( 0; sl != 0; sl=({ struct Cyc_List_List* _temp494= sl; if( _temp494 == 0){
_throw( Null_Exception);} _temp494->tl;})){ void* _temp496=( void*)(( struct Cyc_Absyn_Stmt*)({
struct Cyc_List_List* _temp495= sl; if( _temp495 == 0){ _throw( Null_Exception);}
_temp495->hd;}))->r; struct Cyc_Absyn_Stmt* _temp502; struct Cyc_Absyn_Stmt**
_temp504; struct _tagged_string* _temp505; _LL498: if(( unsigned int) _temp496 >
1u?*(( int*) _temp496) == Cyc_Absyn_Goto_s: 0){ _LL506: _temp505=( struct
_tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp496)->f1; goto _LL503;
_LL503: _temp502=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp496)->f2; _temp504=&(( struct Cyc_Absyn_Goto_s_struct*) _temp496)->f2; goto
_LL499;} else{ goto _LL500;} _LL500: goto _LL501; _LL499:* _temp504=( struct Cyc_Absyn_Stmt*)
s; goto _LL497; _LL501:(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp507=( char*)"Tcenv: add_label backpatching of non-goto";
struct _tagged_string _temp508; _temp508.curr= _temp507; _temp508.base= _temp507;
_temp508.last_plus_one= _temp507 + 42; _temp508;})); goto _LL497; _LL497:;}}}
if((( int(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_member)((
fe->shared)->seen_labels, v)){ Cyc_Tcutil_terr( s->loc,({ struct _tagged_string
_temp510=* v; xprintf("Repeated label: %.*s", _temp510.last_plus_one - _temp510.curr,
_temp510.curr);}));}( fe->shared)->seen_labels=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Stmt* data))
Cyc_Dict_insert)(( fe->shared)->seen_labels, v, s); return te;}} int Cyc_Tcenv_all_labels_resolved(
struct Cyc_Tcenv_Tenv* te){ return(( int(*)( struct Cyc_SlowDict_Dict* d)) Cyc_SlowDict_is_empty)(((
Cyc_Tcenv_get_fenv( te))->shared)->needed_labels);} struct Cyc_Absyn_Stmt* Cyc_Tcenv_get_encloser(
struct Cyc_Tcenv_Tenv* te){ if( te->le == 0){(( void(*)( struct _tagged_string
msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char* _temp511=( char*)"Tcenv: unexpected get_encloser";
struct _tagged_string _temp512; _temp512.curr= _temp511; _temp512.base= _temp511;
_temp512.last_plus_one= _temp511 + 31; _temp512;}));} return( Cyc_Tcenv_get_fenv(
te))->encloser;} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_encloser( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Stmt* s){ if( te->le == 0){(( void(*)( struct
_tagged_string msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char*
_temp513=( char*)"Tcenv: unexpected set_encloser"; struct _tagged_string
_temp514; _temp514.curr= _temp513; _temp514.base= _temp513; _temp514.last_plus_one=
_temp513 + 31; _temp514;}));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); fe->encloser= s; return Cyc_Tcenv_put_fenv( te, fe);}}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_region( struct Cyc_Tcenv_Tenv* te, void* r){
if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp515=( char*)"Tcenv::add_region: unexpected add_region";
struct _tagged_string _temp516; _temp516.curr= _temp515; _temp516.base= _temp515;
_temp516.last_plus_one= _temp515 + 41; _temp516;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_old_ctrl( Cyc_Tcenv_get_fenv( te));( void*)( fe->capability=(
void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp517=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp517[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp518; _temp518.tag= Cyc_Absyn_JoinEff; _temp518.f1=({ struct Cyc_List_List*
_temp519=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp519->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp521=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp521[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp522; _temp522.tag= Cyc_Absyn_AccessEff;
_temp522.f1=( void*) r; _temp522;}); _temp521;})); _temp519->tl=({ struct Cyc_List_List*
_temp520=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp520->hd=( void*)(( void*) fe->capability); _temp520->tl= 0; _temp520;});
_temp519;}); _temp518;}); _temp517;}))); return Cyc_Tcenv_put_fenv( te, fe);}}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_new_named_block( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Tvar* block_rgn){ if( te->le ==
0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(( struct
_tagged_string)({ char* _temp523=( char*)"Tcenv: unexpected new_block"; struct
_tagged_string _temp524; _temp524.curr= _temp523; _temp524.base= _temp523;
_temp524.last_plus_one= _temp523 + 28; _temp524;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_old_ctrl( Cyc_Tcenv_get_fenv( te)); void* block_typ=( void*)({
struct Cyc_Absyn_VarType_struct* _temp532=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp532[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp533; _temp533.tag= Cyc_Absyn_VarType; _temp533.f1= block_rgn; _temp533;});
_temp532;}); fe->type_vars=({ struct Cyc_List_List* _temp525=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp525->hd=( void*) block_rgn;
_temp525->tl= fe->type_vars; _temp525;}); Cyc_Tcutil_check_unique_tvars( loc, fe->type_vars);(
void*)( fe->capability=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp526=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp526[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp527; _temp527.tag= Cyc_Absyn_JoinEff;
_temp527.f1=({ struct Cyc_List_List* _temp528=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp528->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp530=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp530[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp531; _temp531.tag= Cyc_Absyn_AccessEff;
_temp531.f1=( void*) block_typ; _temp531;}); _temp530;})); _temp528->tl=({
struct Cyc_List_List* _temp529=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp529->hd=( void*)(( void*) fe->capability); _temp529->tl=
0; _temp529;}); _temp528;}); _temp527;}); _temp526;})));( void*)( fe->curr_rgn=(
void*) block_typ); return Cyc_Tcenv_put_fenv( te, fe);}} struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_new_block( struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te){
return Cyc_Tcenv_new_named_block( loc, te, Cyc_Tcutil_new_tvar(( void*) Cyc_Absyn_RgnKind));}
void* Cyc_Tcenv_curr_rgn( struct Cyc_Tcenv_Tenv* te){ if( te->le == 0){ return(
void*) Cyc_Absyn_HeapRgn;} return( void*)( Cyc_Tcenv_get_fenv( te))->curr_rgn;}
void Cyc_Tcenv_check_rgn_accessible( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void* rgn){ if( te->le != 0){ void* capability=( void*)( Cyc_Tcenv_get_fenv(
te))->capability; if( Cyc_Tcutil_region_in_effect( 0, rgn, capability)? 1: Cyc_Tcutil_region_in_effect(
1, rgn, capability)){ return;}} Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp534= Cyc_Absynpp_typ2string( rgn); xprintf("Expression accesses unavailable region %.*s",
_temp534.last_plus_one - _temp534.curr, _temp534.curr);}));} void Cyc_Tcenv_check_effect_accessible(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* eff){ void*
capability; if( te->le == 0){ capability=( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp535=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp535[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp536; _temp536.tag= Cyc_Absyn_JoinEff;
_temp536.f1= 0; _temp536;}); _temp535;});} else{ struct Cyc_Tcenv_Fenv* fenv=
Cyc_Tcenv_get_fenv( te); capability=( void*) fenv->capability;} if( ! Cyc_Tcutil_subset_effect(
1, eff, capability)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp537=
Cyc_Absynpp_typ2string( capability); struct _tagged_string _temp538= Cyc_Absynpp_typ2string(
eff); xprintf("Capability %.*s does not cover function's effect %.*s", _temp537.last_plus_one
- _temp537.curr, _temp537.curr, _temp538.last_plus_one - _temp538.curr, _temp538.curr);}));}}
struct _tuple5{ struct _tagged_string* f1; struct Cyc_Absyn_Tqual f2; void* f3;
} ; struct Cyc_Tcenv_Fenv* Cyc_Tcenv_new_fenv( struct Cyc_Position_Segment* loc,
struct Cyc_Absyn_Fndecl* fd){ struct Cyc_Dict_Dict* locals=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); struct Cyc_Absyn_Tvar* rgn0=({ struct Cyc_Absyn_Tvar*
_temp569=( struct Cyc_Absyn_Tvar*) GC_malloc( sizeof( struct Cyc_Absyn_Tvar));
_temp569->name=({ struct _tagged_string* _temp570=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp570[ 0]=({ struct
_tagged_string _temp571=*(* fd->name).f2; xprintf("`%.*s", _temp571.last_plus_one
- _temp571.curr, _temp571.curr);}); _temp570;}); _temp569->kind=(( struct Cyc_Absyn_Conref*(*)(
void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind); _temp569;}); struct
Cyc_List_List* tvs=({ struct Cyc_List_List* _temp568=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp568->hd=( void*) rgn0; _temp568->tl=
fd->tvs; _temp568;}); Cyc_Tcutil_check_unique_tvars( loc, tvs);{ void* param_rgn=(
void*)({ struct Cyc_Absyn_VarType_struct* _temp566=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp566[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp567; _temp567.tag= Cyc_Absyn_VarType; _temp567.f1= rgn0; _temp567;});
_temp566;}); struct Cyc_List_List* vds= 0;{ struct Cyc_List_List* args= fd->args;
for( 0; args != 0; args=({ struct Cyc_List_List* _temp539= args; if( _temp539 ==
0){ _throw( Null_Exception);} _temp539->tl;})){ struct Cyc_Absyn_Vardecl* vd=({
struct Cyc_Absyn_Vardecl* _temp545=( struct Cyc_Absyn_Vardecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Vardecl) * 1); _temp545[ 0]=({ struct Cyc_Absyn_Vardecl
_temp546; _temp546.sc=( void*)(( void*) Cyc_Absyn_Public); _temp546.name=({
struct _tuple0* _temp551=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0) *
1); _temp551[ 0]=({ struct _tuple0 _temp552; _temp552.f1=( void*) Cyc_Absyn_Loc_n;
_temp552.f2=(*(( struct _tuple5*)({ struct Cyc_List_List* _temp553= args; if(
_temp553 == 0){ _throw( Null_Exception);} _temp553->hd;}))).f1; _temp552;});
_temp551;}); _temp546.tq=(*(( struct _tuple5*)({ struct Cyc_List_List* _temp550=
args; if( _temp550 == 0){ _throw( Null_Exception);} _temp550->hd;}))).f2;
_temp546.type=( void*)(*(( struct _tuple5*)({ struct Cyc_List_List* _temp549=
args; if( _temp549 == 0){ _throw( Null_Exception);} _temp549->hd;}))).f3;
_temp546.initializer= 0; _temp546.rgn=({ struct Cyc_Core_Opt* _temp547=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt) * 1); _temp547[ 0]=({
struct Cyc_Core_Opt _temp548; _temp548.v=( void*) param_rgn; _temp548;});
_temp547;}); _temp546.attributes= 0; _temp546;}); _temp545;}); vds=({ struct Cyc_List_List*
_temp540=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp540[ 0]=({ struct Cyc_List_List _temp541; _temp541.hd=( void*) vd; _temp541.tl=
vds; _temp541;}); _temp540;}); locals=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, void* data)) Cyc_Dict_insert)( locals,(*(( struct
_tuple5*)({ struct Cyc_List_List* _temp542= args; if( _temp542 == 0){ _throw(
Null_Exception);} _temp542->hd;}))).f1,( void*)({ struct Cyc_Absyn_Param_b_struct*
_temp543=( struct Cyc_Absyn_Param_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Param_b_struct));
_temp543[ 0]=({ struct Cyc_Absyn_Param_b_struct _temp544; _temp544.tag= Cyc_Absyn_Param_b;
_temp544.f1= vd; _temp544;}); _temp543;}));}} fd->param_vardecls=({ struct Cyc_Core_Opt*
_temp554=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt) * 1);
_temp554[ 0]=({ struct Cyc_Core_Opt _temp555; _temp555.v=( void*) vds; _temp555;});
_temp554;}); return({ struct Cyc_Tcenv_Fenv* _temp556=( struct Cyc_Tcenv_Fenv*)
GC_malloc( sizeof( struct Cyc_Tcenv_Fenv)); _temp556->shared=({ struct Cyc_Tcenv_SharedFenv*
_temp565=( struct Cyc_Tcenv_SharedFenv*) GC_malloc( sizeof( struct Cyc_Tcenv_SharedFenv));
_temp565->return_typ=( void*)(( void*) fd->ret_type); _temp565->seen_labels=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_string*, struct
_tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp565->needed_labels=((
struct Cyc_SlowDict_Dict*(*)( int(* comp)( struct _tagged_string*, struct
_tagged_string*))) Cyc_SlowDict_empty)( Cyc_String_zstrptrcmp); _temp565;});
_temp556->type_vars= tvs; _temp556->locals= locals; _temp556->encloser= fd->body;
_temp556->ctrl_env=({ struct Cyc_Tcenv_CtrlEnv* _temp564=( struct Cyc_Tcenv_CtrlEnv*)
GC_malloc( sizeof( struct Cyc_Tcenv_CtrlEnv)); _temp564->continue_stmt=( void*)((
void*) Cyc_Tcenv_NotLoop_j); _temp564->break_stmt=( void*)(( void*) Cyc_Tcenv_NotLoop_j);
_temp564->fallthru_clause= 0; _temp564->next_stmt=( void*)(( void*) Cyc_Tcenv_FnEnd_j);
_temp564->try_depth= 0; _temp564;}); _temp556->capability=( void*)(( void*)({
struct Cyc_Absyn_JoinEff_struct* _temp557=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp557[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp558; _temp558.tag= Cyc_Absyn_JoinEff; _temp558.f1=({ struct Cyc_List_List*
_temp559=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp559->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp562=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp562[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp563; _temp563.tag= Cyc_Absyn_AccessEff;
_temp563.f1=( void*) param_rgn; _temp563;}); _temp562;})); _temp559->tl=({
struct Cyc_List_List* _temp560=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp560->hd=( void*)(( void*)({ struct Cyc_Core_Opt*
_temp561= fd->effect; if( _temp561 == 0){ _throw( Null_Exception);} _temp561->v;}));
_temp560->tl= 0; _temp560;}); _temp559;}); _temp558;}); _temp557;})); _temp556->curr_rgn=(
void*) param_rgn; _temp556;});}}