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
_temp1;});{ struct Cyc_Tcenv_Genv* _temp2= Cyc_Tcenv_empty_genv(); goto _LL3;
_LL3: { struct Cyc_List_List* _temp4= 0; goto _LL5; _LL5: _temp2->tuniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Tuniondecl** data)) Cyc_Dict_insert)( _temp2->tuniondecls,(*
Cyc_Absyn_exn_name).f2,& Cyc_Absyn_exn_xed);{ struct Cyc_Dict_Dict* _temp6=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key,
struct Cyc_Tcenv_Genv* data)) Cyc_Dict_insert)((( struct Cyc_Dict_Dict*(*)( int(*
comp)( struct Cyc_List_List*, struct Cyc_List_List*))) Cyc_Dict_empty)( Cyc_Absyn_varlist_cmp),
_temp4, _temp2); goto _LL7; _LL7: return({ struct Cyc_Tcenv_Tenv* _temp8=(
struct Cyc_Tcenv_Tenv*) GC_malloc( sizeof( struct Cyc_Tcenv_Tenv)); _temp8->ns=
_temp4; _temp8->ae= _temp6; _temp8->le= 0; _temp8;});}}}} static struct Cyc_Tcenv_Genv*
Cyc_Tcenv_lookup_namespace( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct _tagged_string* n, struct Cyc_List_List* ns){ return(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
Cyc_Tcenv_resolve_namespace( te, loc, n, ns));} static struct Cyc_List_List* Cyc_Tcenv_outer_namespace(
struct Cyc_List_List* ns){ if( ns == 0){ return(( struct Cyc_List_List*(*)(
struct _tagged_string msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char*
_temp9=( char*)"Tcenv::outer_namespace"; struct _tagged_string _temp10; _temp10.curr=
_temp9; _temp10.base= _temp9; _temp10.last_plus_one= _temp9 + 23; _temp10;}));}
return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)(({
struct Cyc_List_List* _temp11=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( ns); if( _temp11 == 0){ _throw( Null_Exception);} _temp11->tl;}));}
static void Cyc_Tcenv_check_repeat( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void*(* lookup)( struct Cyc_Tcenv_Genv*, struct _tagged_string*), struct
_tagged_string* v, struct Cyc_List_List* nss){ for( 0; nss != 0; nss=({ struct
Cyc_List_List* _temp12= nss; if( _temp12 == 0){ _throw( Null_Exception);}
_temp12->tl;})){ struct Cyc_Tcenv_Genv* ge2=(( struct Cyc_Tcenv_Genv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,( struct
Cyc_List_List*)({ struct Cyc_List_List* _temp24= nss; if( _temp24 == 0){ _throw(
Null_Exception);} _temp24->hd;})); struct _handler_cons _temp13; _push_handler(&
_temp13);{ int _temp15= 0; if( setjmp( _temp13.handler)){ _temp15= 1;} if( !
_temp15){ lookup( ge2, v); Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp16=*
v; xprintf("%.*s is ambiguous", _temp16.last_plus_one - _temp16.curr, _temp16.curr);}));;
_pop_handler();} else{ void* _temp14=( void*) _exn_thrown; void* _temp18=
_temp14; _LL20: if( _temp18 == Cyc_Dict_Absent){ goto _LL21;} else{ goto _LL22;}
_LL22: goto _LL23; _LL21: goto _LL19; _LL23:( void) _throw( _temp18); _LL19:;}}}
return;} static void* Cyc_Tcenv_scoped_lookup( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void*(* lookup)( struct Cyc_Tcenv_Genv*, struct
_tagged_string*), struct _tagged_string* v){ struct Cyc_List_List* cns= te->ns;
void* result; while( 1) { struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
cns);{ struct _handler_cons _temp25; _push_handler(& _temp25);{ int _temp27= 0;
if( setjmp( _temp25.handler)){ _temp27= 1;} if( ! _temp27){ result= lookup( ge,
v);(( void(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void*(*
lookup)( struct Cyc_Tcenv_Genv*, struct _tagged_string*), struct _tagged_string*
v, struct Cyc_List_List* nss)) Cyc_Tcenv_check_repeat)( te, loc, lookup, v, ge->availables);{
void* _temp28= result; _npop_handler( 0u); return _temp28;}; _pop_handler();}
else{ void* _temp26=( void*) _exn_thrown; void* _temp30= _temp26; _LL32: if(
_temp30 == Cyc_Dict_Absent){ goto _LL33;} else{ goto _LL34;} _LL34: goto _LL35;
_LL33: goto _LL31; _LL35:( void) _throw( _temp30); _LL31:;}}}{ struct Cyc_List_List*
_temp36= ge->availables; goto _LL37; _LL37: for( 0; _temp36 != 0; _temp36=({
struct Cyc_List_List* _temp38= _temp36; if( _temp38 == 0){ _throw(
Null_Exception);} _temp38->tl;})){ struct Cyc_Tcenv_Genv* ge2=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp51= _temp36; if( _temp51 ==
0){ _throw( Null_Exception);} _temp51->hd;})); struct _handler_cons _temp39;
_push_handler(& _temp39);{ int _temp41= 0; if( setjmp( _temp39.handler)){
_temp41= 1;} if( ! _temp41){ result= lookup( ge2, v);(( void(*)( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void*(* lookup)( struct Cyc_Tcenv_Genv*,
struct _tagged_string*), struct _tagged_string* v, struct Cyc_List_List* nss))
Cyc_Tcenv_check_repeat)( te, loc, lookup, v,({ struct Cyc_List_List* _temp42=
_temp36; if( _temp42 == 0){ _throw( Null_Exception);} _temp42->tl;}));{ void*
_temp43= result; _npop_handler( 0u); return _temp43;}; _pop_handler();} else{
void* _temp40=( void*) _exn_thrown; void* _temp45= _temp40; _LL47: if( _temp45
== Cyc_Dict_Absent){ goto _LL48;} else{ goto _LL49;} _LL49: goto _LL50; _LL48:
goto _LL46; _LL50:( void) _throw( _temp45); _LL46:;}}}} if( cns == 0){( void)
_throw(( void*) Cyc_Dict_Absent);} cns= Cyc_Tcenv_outer_namespace( cns);}}
struct _tuple4{ void* f1; int f2; } ; static void* Cyc_Tcenv_lookup_ordinary_global_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_string* v){ struct _tuple4* ans=((
struct _tuple4*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)(
ge->ordinaries, v);(* ans).f2= 1; return(* ans).f1;} static void* Cyc_Tcenv_lookup_ordinary_global(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct _tuple0* q){
struct _tuple0 _temp54; struct _tagged_string* _temp55; void* _temp57; struct
_tuple0* _temp52= q; _temp54=* _temp52; _LL58: _temp57= _temp54.f1; goto _LL56;
_LL56: _temp55= _temp54.f2; goto _LL53; _LL53: { void* _temp59= _temp57; struct
Cyc_List_List* _temp69; struct Cyc_List_List* _temp71; struct Cyc_List_List
_temp73; struct Cyc_List_List* _temp74; struct _tagged_string* _temp76; struct
Cyc_List_List* _temp78; _LL61: if( _temp59 ==( void*) Cyc_Absyn_Loc_n){ goto
_LL62;} else{ goto _LL63;} _LL63: if(( unsigned int) _temp59 > 1u?*(( int*)
_temp59) == Cyc_Absyn_Rel_n: 0){ _LL70: _temp69=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp59)->f1; if( _temp69 == 0){ goto _LL64;}
else{ goto _LL65;}} else{ goto _LL65;} _LL65: if(( unsigned int) _temp59 > 1u?*((
int*) _temp59) == Cyc_Absyn_Rel_n: 0){ _LL72: _temp71=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp59)->f1; if( _temp71 == 0){ goto _LL67;}
else{ _temp73=* _temp71; _LL77: _temp76=( struct _tagged_string*) _temp73.hd;
goto _LL75; _LL75: _temp74=( struct Cyc_List_List*) _temp73.tl; goto _LL66;}}
else{ goto _LL67;} _LL67: if(( unsigned int) _temp59 > 1u?*(( int*) _temp59) ==
Cyc_Absyn_Abs_n: 0){ _LL79: _temp78=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp59)->f1; goto _LL68;} else{ goto _LL60;} _LL62: goto _LL64; _LL64: return((
void*(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void*(*
lookup)( struct Cyc_Tcenv_Genv*, struct _tagged_string*), struct _tagged_string*
v)) Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_ordinary_global_f,
_temp55); _LL66: { struct Cyc_Tcenv_Genv* _temp80= Cyc_Tcenv_lookup_namespace(
te, loc, _temp76, _temp74); goto _LL81; _LL81: return Cyc_Tcenv_lookup_ordinary_global_f(
_temp80, _temp55);} _LL68: return Cyc_Tcenv_lookup_ordinary_global_f((( struct
Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)(
te->ae, _temp78), _temp55); _LL60:;}} struct Cyc_List_List* Cyc_Tcenv_resolve_namespace(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct
_tagged_string* n, struct Cyc_List_List* ns){ struct Cyc_List_List* _temp82= te->ns;
goto _LL83; _LL83: { struct Cyc_List_List* _temp84= 0; goto _LL85; _LL85: while(
1) { struct Cyc_Tcenv_Genv* _temp86=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, _temp82); goto _LL87;
_LL87:{ struct Cyc_List_List* _temp88= _temp86->availables; goto _LL89; _LL89:
for( 0; _temp88 != 0; _temp88=({ struct Cyc_List_List* _temp90= _temp88; if(
_temp90 == 0){ _throw( Null_Exception);} _temp90->tl;})){ struct Cyc_Tcenv_Genv*
ge2=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)( te->ae,( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp93= _temp88; if( _temp93 == 0){ _throw( Null_Exception);} _temp93->hd;}));
if((( int(*)( struct Cyc_Set_Set* s, struct _tagged_string* elt)) Cyc_Set_member)(
ge2->namespaces, n)){ _temp84=({ struct Cyc_List_List* _temp91=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp91->hd=( void*)(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp82,({
struct Cyc_List_List* _temp92=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp92->hd=( void*) n; _temp92->tl= ns; _temp92;})); _temp91->tl=
_temp84; _temp91;});}}} if((( int(*)( struct Cyc_Set_Set* s, struct
_tagged_string* elt)) Cyc_Set_member)( _temp86->namespaces, n)){ _temp84=({
struct Cyc_List_List* _temp94=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp94->hd=( void*)(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( _temp82,({ struct Cyc_List_List*
_temp95=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp95->hd=( void*) n; _temp95->tl= ns; _temp95;})); _temp94->tl= _temp84;
_temp94;});} if( _temp84 != 0){ if(({ struct Cyc_List_List* _temp96= _temp84;
if( _temp96 == 0){ _throw( Null_Exception);} _temp96->tl;}) != 0){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp97=* n; xprintf("%.*s is ambiguous", _temp97.last_plus_one
- _temp97.curr, _temp97.curr);}));} return( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp98= _temp84; if( _temp98 == 0){ _throw( Null_Exception);} _temp98->hd;});}
if( _temp82 == 0){( void) _throw(( void*) Cyc_Dict_Absent);} _temp82= Cyc_Tcenv_outer_namespace(
_temp82);}}} static struct Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_string* v){ return(( struct Cyc_Absyn_Structdecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->structdecls,
v);} struct Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp101; struct _tagged_string* _temp102; void* _temp104; struct _tuple0*
_temp99= q; _temp101=* _temp99; _LL105: _temp104= _temp101.f1; goto _LL103;
_LL103: _temp102= _temp101.f2; goto _LL100; _LL100: { void* _temp106= _temp104;
struct Cyc_List_List* _temp116; struct Cyc_List_List* _temp118; struct Cyc_List_List*
_temp120; struct Cyc_List_List _temp122; struct Cyc_List_List* _temp123; struct
_tagged_string* _temp125; _LL108: if( _temp106 ==( void*) Cyc_Absyn_Loc_n){ goto
_LL109;} else{ goto _LL110;} _LL110: if(( unsigned int) _temp106 > 1u?*(( int*)
_temp106) == Cyc_Absyn_Rel_n: 0){ _LL117: _temp116=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp106)->f1; if( _temp116 == 0){ goto _LL111;}
else{ goto _LL112;}} else{ goto _LL112;} _LL112: if(( unsigned int) _temp106 > 1u?*((
int*) _temp106) == Cyc_Absyn_Abs_n: 0){ _LL119: _temp118=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp106)->f1; goto _LL113;} else{ goto _LL114;}
_LL114: if(( unsigned int) _temp106 > 1u?*(( int*) _temp106) == Cyc_Absyn_Rel_n:
0){ _LL121: _temp120=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp106)->f1; if( _temp120 == 0){ goto _LL107;} else{ _temp122=* _temp120;
_LL126: _temp125=( struct _tagged_string*) _temp122.hd; goto _LL124; _LL124:
_temp123=( struct Cyc_List_List*) _temp122.tl; goto _LL115;}} else{ goto _LL107;}
_LL109: goto _LL111; _LL111: return(( struct Cyc_Absyn_Structdecl**(*)( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Structdecl**(*
lookup)( struct Cyc_Tcenv_Genv*, struct _tagged_string*), struct _tagged_string*
v)) Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_structdecl_f, _temp102);
_LL113: { struct Cyc_Tcenv_Genv* _temp127=(( struct Cyc_Tcenv_Genv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, _temp118);
goto _LL128; _LL128: return(( struct Cyc_Absyn_Structdecl**(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup)( _temp127->structdecls,
_temp102);} _LL115: { struct Cyc_Tcenv_Genv* _temp129= Cyc_Tcenv_lookup_namespace(
te, loc, _temp125, _temp123); goto _LL130; _LL130: return(( struct Cyc_Absyn_Structdecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( _temp129->structdecls,
_temp102);} _LL107:;}} static struct Cyc_Absyn_Uniondecl** Cyc_Tcenv_lookup_uniondecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_string* v){ return(( struct Cyc_Absyn_Uniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->uniondecls,
v);} struct Cyc_Absyn_Uniondecl** Cyc_Tcenv_lookup_uniondecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp133; struct _tagged_string* _temp134; void* _temp136; struct _tuple0*
_temp131= q; _temp133=* _temp131; _LL137: _temp136= _temp133.f1; goto _LL135;
_LL135: _temp134= _temp133.f2; goto _LL132; _LL132: { void* _temp138= _temp136;
struct Cyc_List_List* _temp148; struct Cyc_List_List* _temp150; struct Cyc_List_List*
_temp152; struct Cyc_List_List _temp154; struct Cyc_List_List* _temp155; struct
_tagged_string* _temp157; _LL140: if( _temp138 ==( void*) Cyc_Absyn_Loc_n){ goto
_LL141;} else{ goto _LL142;} _LL142: if(( unsigned int) _temp138 > 1u?*(( int*)
_temp138) == Cyc_Absyn_Rel_n: 0){ _LL149: _temp148=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp138)->f1; if( _temp148 == 0){ goto _LL143;}
else{ goto _LL144;}} else{ goto _LL144;} _LL144: if(( unsigned int) _temp138 > 1u?*((
int*) _temp138) == Cyc_Absyn_Abs_n: 0){ _LL151: _temp150=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp138)->f1; goto _LL145;} else{ goto _LL146;}
_LL146: if(( unsigned int) _temp138 > 1u?*(( int*) _temp138) == Cyc_Absyn_Rel_n:
0){ _LL153: _temp152=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp138)->f1; if( _temp152 == 0){ goto _LL139;} else{ _temp154=* _temp152;
_LL158: _temp157=( struct _tagged_string*) _temp154.hd; goto _LL156; _LL156:
_temp155=( struct Cyc_List_List*) _temp154.tl; goto _LL147;}} else{ goto _LL139;}
_LL141: goto _LL143; _LL143: return(( struct Cyc_Absyn_Uniondecl**(*)( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Uniondecl**(*
lookup)( struct Cyc_Tcenv_Genv*, struct _tagged_string*), struct _tagged_string*
v)) Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_uniondecl_f, _temp134);
_LL145: { struct Cyc_Tcenv_Genv* _temp159=(( struct Cyc_Tcenv_Genv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, _temp150);
goto _LL160; _LL160: return(( struct Cyc_Absyn_Uniondecl**(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup)( _temp159->uniondecls, _temp134);}
_LL147: { struct Cyc_Tcenv_Genv* _temp161= Cyc_Tcenv_lookup_namespace( te, loc,
_temp157, _temp155); goto _LL162; _LL162: return(( struct Cyc_Absyn_Uniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( _temp161->uniondecls,
_temp134);} _LL139:;}} static struct Cyc_Absyn_Tuniondecl** Cyc_Tcenv_lookup_tuniondecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_string* v){ return(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->tuniondecls,
v);} struct Cyc_Absyn_Tuniondecl** Cyc_Tcenv_lookup_tuniondecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp165; struct _tagged_string* _temp166; void* _temp168; struct _tuple0*
_temp163= q; _temp165=* _temp163; _LL169: _temp168= _temp165.f1; goto _LL167;
_LL167: _temp166= _temp165.f2; goto _LL164; _LL164: { void* _temp170= _temp168;
struct Cyc_List_List* _temp180; struct Cyc_List_List* _temp182; struct Cyc_List_List
_temp184; struct Cyc_List_List* _temp185; struct _tagged_string* _temp187;
struct Cyc_List_List* _temp189; _LL172: if( _temp170 ==( void*) Cyc_Absyn_Loc_n){
goto _LL173;} else{ goto _LL174;} _LL174: if(( unsigned int) _temp170 > 1u?*((
int*) _temp170) == Cyc_Absyn_Rel_n: 0){ _LL181: _temp180=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp170)->f1; if( _temp180 == 0){ goto _LL175;}
else{ goto _LL176;}} else{ goto _LL176;} _LL176: if(( unsigned int) _temp170 > 1u?*((
int*) _temp170) == Cyc_Absyn_Rel_n: 0){ _LL183: _temp182=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp170)->f1; if( _temp182 == 0){ goto _LL178;}
else{ _temp184=* _temp182; _LL188: _temp187=( struct _tagged_string*) _temp184.hd;
goto _LL186; _LL186: _temp185=( struct Cyc_List_List*) _temp184.tl; goto _LL177;}}
else{ goto _LL178;} _LL178: if(( unsigned int) _temp170 > 1u?*(( int*) _temp170)
== Cyc_Absyn_Abs_n: 0){ _LL190: _temp189=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp170)->f1; goto _LL179;} else{ goto _LL171;} _LL173: goto _LL175; _LL175:
return(( struct Cyc_Absyn_Tuniondecl**(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Tuniondecl**(* lookup)( struct Cyc_Tcenv_Genv*, struct
_tagged_string*), struct _tagged_string* v)) Cyc_Tcenv_scoped_lookup)( te, loc,
Cyc_Tcenv_lookup_tuniondecl_f, _temp166); _LL177: { struct Cyc_Tcenv_Genv*
_temp191= Cyc_Tcenv_lookup_namespace( te, loc, _temp187, _temp185); goto _LL192;
_LL192: return(( struct Cyc_Absyn_Tuniondecl**(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key)) Cyc_Dict_lookup)( _temp191->tuniondecls, _temp166);}
_LL179: { struct Cyc_Tcenv_Genv* _temp193=(( struct Cyc_Tcenv_Genv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, _temp189);
goto _LL194; _LL194: return(( struct Cyc_Absyn_Tuniondecl**(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup)( _temp193->tuniondecls,
_temp166);} _LL171:;}} static struct Cyc_Absyn_Tuniondecl** Cyc_Tcenv_lookup_xtuniondecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_string* v){ return(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->tuniondecls,
v);} struct Cyc_Core_Opt* Cyc_Tcenv_lookup_xtuniondecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp197; struct _tagged_string* _temp198; void* _temp200; struct _tuple0*
_temp195= q; _temp197=* _temp195; _LL201: _temp200= _temp197.f1; goto _LL199;
_LL199: _temp198= _temp197.f2; goto _LL196; _LL196: { void* _temp202= _temp200;
struct Cyc_List_List* _temp212; struct Cyc_List_List* _temp214; struct Cyc_List_List
_temp216; struct Cyc_List_List* _temp217; struct _tagged_string* _temp219;
struct Cyc_List_List* _temp221; _LL204: if(( unsigned int) _temp202 > 1u?*(( int*)
_temp202) == Cyc_Absyn_Rel_n: 0){ _LL213: _temp212=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp202)->f1; if( _temp212 == 0){ goto _LL205;}
else{ goto _LL206;}} else{ goto _LL206;} _LL206: if( _temp202 ==( void*) Cyc_Absyn_Loc_n){
goto _LL207;} else{ goto _LL208;} _LL208: if(( unsigned int) _temp202 > 1u?*((
int*) _temp202) == Cyc_Absyn_Rel_n: 0){ _LL215: _temp214=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp202)->f1; if( _temp214 == 0){ goto _LL210;}
else{ _temp216=* _temp214; _LL220: _temp219=( struct _tagged_string*) _temp216.hd;
goto _LL218; _LL218: _temp217=( struct Cyc_List_List*) _temp216.tl; goto _LL209;}}
else{ goto _LL210;} _LL210: if(( unsigned int) _temp202 > 1u?*(( int*) _temp202)
== Cyc_Absyn_Abs_n: 0){ _LL222: _temp221=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp202)->f1; goto _LL211;} else{ goto _LL203;} _LL205: { struct _handler_cons
_temp223; _push_handler(& _temp223);{ int _temp225= 0; if( setjmp( _temp223.handler)){
_temp225= 1;} if( ! _temp225){{ struct Cyc_Core_Opt* _temp227=({ struct Cyc_Core_Opt*
_temp226=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp226->v=( void*)(( struct Cyc_Absyn_Tuniondecl**(*)( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Tuniondecl**(* lookup)(
struct Cyc_Tcenv_Genv*, struct _tagged_string*), struct _tagged_string* v)) Cyc_Tcenv_scoped_lookup)(
te, loc, Cyc_Tcenv_lookup_xtuniondecl_f, _temp198); _temp226;}); _npop_handler(
0u); return _temp227;}; _pop_handler();} else{ void* _temp224=( void*)
_exn_thrown; void* _temp229= _temp224; _LL231: if( _temp229 == Cyc_Dict_Absent){
goto _LL232;} else{ goto _LL233;} _LL233: goto _LL234; _LL232: return 0; _LL234:(
void) _throw( _temp229); _LL230:;}}} _LL207: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp235=( char*)"lookup_xtuniondecl: impossible";
struct _tagged_string _temp236; _temp236.curr= _temp235; _temp236.base= _temp235;
_temp236.last_plus_one= _temp235 + 31; _temp236;})); return 0; _LL209: { struct
Cyc_Tcenv_Genv* ge;{ struct _handler_cons _temp237; _push_handler(& _temp237);{
int _temp239= 0; if( setjmp( _temp237.handler)){ _temp239= 1;} if( ! _temp239){
ge= Cyc_Tcenv_lookup_namespace( te, loc, _temp219, _temp217);; _pop_handler();}
else{ void* _temp238=( void*) _exn_thrown; void* _temp241= _temp238; _LL243: if(
_temp241 == Cyc_Dict_Absent){ goto _LL244;} else{ goto _LL245;} _LL245: goto
_LL246; _LL244: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp247=(
char*)"bad qualified name for xtunion"; struct _tagged_string _temp248; _temp248.curr=
_temp247; _temp248.base= _temp247; _temp248.last_plus_one= _temp247 + 31;
_temp248;}));( void) _throw(( void*) Cyc_Dict_Absent); _LL246:( void) _throw(
_temp241); _LL242:;}}} return({ struct Cyc_Core_Opt* _temp249=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp249->v=( void*)(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->tuniondecls,
_temp198); _temp249;});} _LL211: { struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
_temp221); return({ struct Cyc_Core_Opt* _temp250=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp250->v=( void*)(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->tuniondecls,
_temp198); _temp250;});} _LL203:;}} static struct Cyc_Absyn_Enumdecl** Cyc_Tcenv_lookup_enumdecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_string* v){ return(( struct Cyc_Absyn_Enumdecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->enumdecls,
v);} struct Cyc_Absyn_Enumdecl** Cyc_Tcenv_lookup_enumdecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp253; struct _tagged_string* _temp254; void* _temp256; struct _tuple0*
_temp251= q; _temp253=* _temp251; _LL257: _temp256= _temp253.f1; goto _LL255;
_LL255: _temp254= _temp253.f2; goto _LL252; _LL252: { void* _temp258= _temp256;
struct Cyc_List_List* _temp268; struct Cyc_List_List* _temp270; struct Cyc_List_List
_temp272; struct Cyc_List_List* _temp273; struct _tagged_string* _temp275;
struct Cyc_List_List* _temp277; _LL260: if( _temp258 ==( void*) Cyc_Absyn_Loc_n){
goto _LL261;} else{ goto _LL262;} _LL262: if(( unsigned int) _temp258 > 1u?*((
int*) _temp258) == Cyc_Absyn_Rel_n: 0){ _LL269: _temp268=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp258)->f1; if( _temp268 == 0){ goto _LL263;}
else{ goto _LL264;}} else{ goto _LL264;} _LL264: if(( unsigned int) _temp258 > 1u?*((
int*) _temp258) == Cyc_Absyn_Rel_n: 0){ _LL271: _temp270=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp258)->f1; if( _temp270 == 0){ goto _LL266;}
else{ _temp272=* _temp270; _LL276: _temp275=( struct _tagged_string*) _temp272.hd;
goto _LL274; _LL274: _temp273=( struct Cyc_List_List*) _temp272.tl; goto _LL265;}}
else{ goto _LL266;} _LL266: if(( unsigned int) _temp258 > 1u?*(( int*) _temp258)
== Cyc_Absyn_Abs_n: 0){ _LL278: _temp277=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp258)->f1; goto _LL267;} else{ goto _LL259;} _LL261: goto _LL263; _LL263:
return(( struct Cyc_Absyn_Enumdecl**(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Enumdecl**(* lookup)( struct Cyc_Tcenv_Genv*, struct
_tagged_string*), struct _tagged_string* v)) Cyc_Tcenv_scoped_lookup)( te, loc,
Cyc_Tcenv_lookup_enumdecl_f, _temp254); _LL265: { struct Cyc_Tcenv_Genv*
_temp279= Cyc_Tcenv_lookup_namespace( te, loc, _temp275, _temp273); goto _LL280;
_LL280: return(( struct Cyc_Absyn_Enumdecl**(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup)( _temp279->enumdecls, _temp254);} _LL267: {
struct Cyc_Tcenv_Genv* _temp281=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, _temp277); goto _LL282;
_LL282: return(( struct Cyc_Absyn_Enumdecl**(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup)( _temp281->enumdecls, _temp254);} _LL259:;}}
static struct Cyc_Absyn_Typedefdecl* Cyc_Tcenv_lookup_typedefdecl_f( struct Cyc_Tcenv_Genv*
ge, struct _tagged_string* v){ return(( struct Cyc_Absyn_Typedefdecl*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->typedefs, v);}
struct Cyc_Absyn_Typedefdecl* Cyc_Tcenv_lookup_typedefdecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp285; struct _tagged_string* _temp286; void* _temp288; struct _tuple0*
_temp283= q; _temp285=* _temp283; _LL289: _temp288= _temp285.f1; goto _LL287;
_LL287: _temp286= _temp285.f2; goto _LL284; _LL284: { void* _temp290= _temp288;
struct Cyc_List_List* _temp300; struct Cyc_List_List* _temp302; struct Cyc_List_List
_temp304; struct Cyc_List_List* _temp305; struct _tagged_string* _temp307;
struct Cyc_List_List* _temp309; _LL292: if( _temp290 ==( void*) Cyc_Absyn_Loc_n){
goto _LL293;} else{ goto _LL294;} _LL294: if(( unsigned int) _temp290 > 1u?*((
int*) _temp290) == Cyc_Absyn_Rel_n: 0){ _LL301: _temp300=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp290)->f1; if( _temp300 == 0){ goto _LL295;}
else{ goto _LL296;}} else{ goto _LL296;} _LL296: if(( unsigned int) _temp290 > 1u?*((
int*) _temp290) == Cyc_Absyn_Rel_n: 0){ _LL303: _temp302=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp290)->f1; if( _temp302 == 0){ goto _LL298;}
else{ _temp304=* _temp302; _LL308: _temp307=( struct _tagged_string*) _temp304.hd;
goto _LL306; _LL306: _temp305=( struct Cyc_List_List*) _temp304.tl; goto _LL297;}}
else{ goto _LL298;} _LL298: if(( unsigned int) _temp290 > 1u?*(( int*) _temp290)
== Cyc_Absyn_Abs_n: 0){ _LL310: _temp309=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp290)->f1; goto _LL299;} else{ goto _LL291;} _LL293: goto _LL295; _LL295:
return(( struct Cyc_Absyn_Typedefdecl*(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Typedefdecl*(* lookup)( struct Cyc_Tcenv_Genv*, struct
_tagged_string*), struct _tagged_string* v)) Cyc_Tcenv_scoped_lookup)( te, loc,
Cyc_Tcenv_lookup_typedefdecl_f, _temp286); _LL297: { struct Cyc_Tcenv_Genv*
_temp311= Cyc_Tcenv_lookup_namespace( te, loc, _temp307, _temp305); goto _LL312;
_LL312: return(( struct Cyc_Absyn_Typedefdecl*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key)) Cyc_Dict_lookup)( _temp311->typedefs, _temp286);}
_LL299: { struct Cyc_Tcenv_Genv* _temp313=(( struct Cyc_Tcenv_Genv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, _temp309);
goto _LL314; _LL314: return(( struct Cyc_Absyn_Typedefdecl*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup)( _temp313->typedefs, _temp286);}
_LL291:;}} struct Cyc_Tcenv_Genv* Cyc_Tcenv_empty_genv(){ return({ struct Cyc_Tcenv_Genv*
_temp315=( struct Cyc_Tcenv_Genv*) GC_malloc( sizeof( struct Cyc_Tcenv_Genv));
_temp315->namespaces=( struct Cyc_Set_Set*)({ struct Cyc_Core_Opt* _temp316= Cyc_Tcutil_empty_var_set;
if( _temp316 == 0){ _throw( Null_Exception);} _temp316->v;}); _temp315->structdecls=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_string*, struct
_tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp315->uniondecls=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_string*, struct
_tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp315->tuniondecls=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_string*, struct
_tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp315->enumdecls=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_string*, struct
_tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp315->typedefs=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_string*, struct
_tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp315->ordinaries=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_string*, struct
_tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp315->availables=
0; _temp315;});} void* Cyc_Tcenv_get_visible( void* f){ while( 1) { void*
_temp317= f; void* _temp325; void* _temp327; void* _temp329; void* _temp331;
void* _temp333; _LL319: if(*(( int*) _temp317) == Cyc_Tcenv_Outermost){ _LL326:
_temp325=( void*)(( struct Cyc_Tcenv_Outermost_struct*) _temp317)->f1; goto
_LL320;} else{ goto _LL321;} _LL321: if(*(( int*) _temp317) == Cyc_Tcenv_Frame){
_LL330: _temp329=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp317)->f1; goto
_LL328; _LL328: _temp327=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp317)->f2;
goto _LL322;} else{ goto _LL323;} _LL323: if(*(( int*) _temp317) == Cyc_Tcenv_Hidden){
_LL334: _temp333=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp317)->f1; goto
_LL332; _LL332: _temp331=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp317)->f2;
goto _LL324;} else{ goto _LL318;} _LL320: return _temp325; _LL322: return
_temp329; _LL324: f= _temp331; goto _LL318; _LL318:;}} void* Cyc_Tcenv_put_visible(
void* f, void* x){ void* _temp335= f; void* _temp343; void* _temp345; void*
_temp347; void* _temp349; void* _temp351; _LL337: if(*(( int*) _temp335) == Cyc_Tcenv_Outermost){
_LL344: _temp343=( void*)(( struct Cyc_Tcenv_Outermost_struct*) _temp335)->f1;
goto _LL338;} else{ goto _LL339;} _LL339: if(*(( int*) _temp335) == Cyc_Tcenv_Frame){
_LL348: _temp347=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp335)->f1; goto
_LL346; _LL346: _temp345=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp335)->f2;
goto _LL340;} else{ goto _LL341;} _LL341: if(*(( int*) _temp335) == Cyc_Tcenv_Hidden){
_LL352: _temp351=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp335)->f1; goto
_LL350; _LL350: _temp349=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp335)->f2;
goto _LL342;} else{ goto _LL336;} _LL338: return( void*)({ struct Cyc_Tcenv_Outermost_struct*
_temp353=( struct Cyc_Tcenv_Outermost_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Outermost_struct));
_temp353[ 0]=({ struct Cyc_Tcenv_Outermost_struct _temp354; _temp354.tag= Cyc_Tcenv_Outermost;
_temp354.f1=( void*) x; _temp354;}); _temp353;}); _LL340: return( void*)({
struct Cyc_Tcenv_Frame_struct* _temp355=( struct Cyc_Tcenv_Frame_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Frame_struct)); _temp355[ 0]=({ struct Cyc_Tcenv_Frame_struct
_temp356; _temp356.tag= Cyc_Tcenv_Frame; _temp356.f1=( void*) x; _temp356.f2=(
void*) _temp345; _temp356;}); _temp355;}); _LL342: return( void*)({ struct Cyc_Tcenv_Hidden_struct*
_temp357=( struct Cyc_Tcenv_Hidden_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Hidden_struct));
_temp357[ 0]=({ struct Cyc_Tcenv_Hidden_struct _temp358; _temp358.tag= Cyc_Tcenv_Hidden;
_temp358.f1=( void*) _temp351; _temp358.f2=( void*)(( void*(*)( void* f, void* x))
Cyc_Tcenv_put_visible)( _temp349, x); _temp358;}); _temp357;}); _LL336:;} void*
Cyc_Tcenv_apply_to_visible( void*(* g)( void*), void* f){ return(( void*(*)(
void* f, void* x)) Cyc_Tcenv_put_visible)( f, g((( void*(*)( void* f)) Cyc_Tcenv_get_visible)(
f)));} void* Cyc_Tcenv_map_frames( void*(* g)( void*), void* f){ void* _temp359=
f; void* _temp367; void* _temp369; void* _temp371; void* _temp373; void*
_temp375; _LL361: if(*(( int*) _temp359) == Cyc_Tcenv_Outermost){ _LL368:
_temp367=( void*)(( struct Cyc_Tcenv_Outermost_struct*) _temp359)->f1; goto
_LL362;} else{ goto _LL363;} _LL363: if(*(( int*) _temp359) == Cyc_Tcenv_Frame){
_LL372: _temp371=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp359)->f1; goto
_LL370; _LL370: _temp369=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp359)->f2;
goto _LL364;} else{ goto _LL365;} _LL365: if(*(( int*) _temp359) == Cyc_Tcenv_Hidden){
_LL376: _temp375=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp359)->f1; goto
_LL374; _LL374: _temp373=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp359)->f2;
goto _LL366;} else{ goto _LL360;} _LL362: return( void*)({ struct Cyc_Tcenv_Outermost_struct*
_temp377=( struct Cyc_Tcenv_Outermost_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Outermost_struct));
_temp377[ 0]=({ struct Cyc_Tcenv_Outermost_struct _temp378; _temp378.tag= Cyc_Tcenv_Outermost;
_temp378.f1=( void*) g( _temp367); _temp378;}); _temp377;}); _LL364: return(
void*)({ struct Cyc_Tcenv_Frame_struct* _temp379=( struct Cyc_Tcenv_Frame_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Frame_struct)); _temp379[ 0]=({ struct Cyc_Tcenv_Frame_struct
_temp380; _temp380.tag= Cyc_Tcenv_Frame; _temp380.f1=( void*) g( _temp371);
_temp380.f2=( void*)(( void*(*)( void*(* g)( void*), void* f)) Cyc_Tcenv_map_frames)(
g, _temp369); _temp380;}); _temp379;}); _LL366: return( void*)({ struct Cyc_Tcenv_Hidden_struct*
_temp381=( struct Cyc_Tcenv_Hidden_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Hidden_struct));
_temp381[ 0]=({ struct Cyc_Tcenv_Hidden_struct _temp382; _temp382.tag= Cyc_Tcenv_Hidden;
_temp382.f1=( void*) g( _temp375); _temp382.f2=( void*)(( void*(*)( void*(* g)(
void*), void* f)) Cyc_Tcenv_map_frames)( g, _temp373); _temp382;}); _temp381;});
_LL360:;} struct Cyc_Tcenv_Fenv* Cyc_Tcenv_get_fenv( struct Cyc_Tcenv_Tenv* te){
if( te->le == 0){( void) _throw(( void*) Cyc_Tcenv_Env_error);} return(( struct
Cyc_Tcenv_Fenv*(*)( void* f)) Cyc_Tcenv_get_visible)(( void*)({ struct Cyc_Core_Opt*
_temp383= te->le; if( _temp383 == 0){ _throw( Null_Exception);} _temp383->v;}));}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_put_fenv( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Fenv*
fe){ if( te->le == 0){( void) _throw(( void*) Cyc_Tcenv_Env_error);} return({
struct Cyc_Tcenv_Tenv* _temp384=( struct Cyc_Tcenv_Tenv*) GC_malloc( sizeof(
struct Cyc_Tcenv_Tenv)); _temp384->ns= te->ns; _temp384->ae= te->ae; _temp384->le=({
struct Cyc_Core_Opt* _temp385=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp385->v=( void*)(( void*(*)( void* f, struct Cyc_Tcenv_Fenv*
x)) Cyc_Tcenv_put_visible)(( void*)({ struct Cyc_Core_Opt* _temp386= te->le; if(
_temp386 == 0){ _throw( Null_Exception);} _temp386->v;}), fe); _temp385;});
_temp384;});} static struct Cyc_Tcenv_Fenv* Cyc_Tcenv_copy_fenv_old_ctrl( struct
Cyc_Tcenv_Fenv* f){ return({ struct Cyc_Tcenv_Fenv* _temp387=( struct Cyc_Tcenv_Fenv*)
GC_malloc( sizeof( struct Cyc_Tcenv_Fenv)); _temp387->shared= f->shared;
_temp387->type_vars= f->type_vars; _temp387->locals= f->locals; _temp387->encloser=
f->encloser; _temp387->ctrl_env= f->ctrl_env; _temp387->capability=( void*)((
void*) f->capability); _temp387->curr_rgn=( void*)(( void*) f->curr_rgn);
_temp387;});} static struct Cyc_Tcenv_Fenv* Cyc_Tcenv_copy_fenv_new_ctrl( struct
Cyc_Tcenv_Fenv* f){ struct Cyc_Tcenv_CtrlEnv* c= f->ctrl_env; return({ struct
Cyc_Tcenv_Fenv* _temp388=( struct Cyc_Tcenv_Fenv*) GC_malloc( sizeof( struct Cyc_Tcenv_Fenv));
_temp388->shared= f->shared; _temp388->type_vars= f->type_vars; _temp388->locals=
f->locals; _temp388->encloser= f->encloser; _temp388->ctrl_env=({ struct Cyc_Tcenv_CtrlEnv*
_temp389=( struct Cyc_Tcenv_CtrlEnv*) GC_malloc( sizeof( struct Cyc_Tcenv_CtrlEnv));
_temp389->continue_stmt=( void*)(( void*) c->continue_stmt); _temp389->break_stmt=(
void*)(( void*) c->break_stmt); _temp389->fallthru_clause= c->fallthru_clause;
_temp389->next_stmt=( void*)(( void*) c->next_stmt); _temp389->try_depth= c->try_depth;
_temp389;}); _temp388->capability=( void*)(( void*) f->capability); _temp388->curr_rgn=(
void*)(( void*) f->curr_rgn); _temp388;});} void* Cyc_Tcenv_return_typ( struct
Cyc_Tcenv_Tenv* te){ if( te->le == 0){(( void(*)( struct _tagged_string msg))
Cyc_Tcenv_env_err)(( struct _tagged_string)({ char* _temp390=( char*)"Tcenv: unexpected return_typ";
struct _tagged_string _temp391; _temp391.curr= _temp390; _temp391.base= _temp390;
_temp391.last_plus_one= _temp390 + 29; _temp391;}));} return( void*)(( Cyc_Tcenv_get_fenv(
te))->shared)->return_typ;} struct Cyc_List_List* Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv* te){ if( te->le == 0){ return 0;} return( Cyc_Tcenv_get_fenv(
te))->type_vars;} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_type_vars( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* tvs){ if( te->le == 0){((
void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({
char* _temp392=( char*)"Tcenv: unexpected add_type_vars"; struct _tagged_string
_temp393; _temp393.curr= _temp392; _temp393.base= _temp392; _temp393.last_plus_one=
_temp392 + 32; _temp393;}));}{ struct Cyc_Tcenv_Fenv* _temp394= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); goto _LL395; _LL395: { struct Cyc_List_List* _temp396=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
tvs, _temp394->type_vars); goto _LL397; _LL397: Cyc_Tcutil_check_unique_tvars(
loc, _temp396); _temp394->type_vars= _temp396; return Cyc_Tcenv_put_fenv( te,
_temp394);}}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_local_var( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Vardecl* vd){ if( te->le == 0){((
void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({
char* _temp398=( char*)"Tcenv: unexpected add_local_var"; struct _tagged_string
_temp399; _temp399.curr= _temp398; _temp399.base= _temp398; _temp399.last_plus_one=
_temp398 + 32; _temp399;}));}{ struct Cyc_Tcenv_Fenv* _temp400= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); goto _LL401; _LL401: { struct _tagged_string* _temp402=(*
vd->name).f2; goto _LL403; _LL403:{ void* _temp404=(* vd->name).f1; _LL406: if(
_temp404 ==( void*) Cyc_Absyn_Loc_n){ goto _LL407;} else{ goto _LL408;} _LL408:
goto _LL409; _LL407: goto _LL405; _LL409:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp410=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp410[ 0]=({ struct Cyc_Core_Impossible_struct _temp411; _temp411.tag= Cyc_Core_Impossible;
_temp411.f1=( struct _tagged_string)({ char* _temp412=( char*)"add_local_var: called with Rel_n";
struct _tagged_string _temp413; _temp413.curr= _temp412; _temp413.base= _temp412;
_temp413.last_plus_one= _temp412 + 33; _temp413;}); _temp411;}); _temp410;}));
goto _LL405; _LL405:;} _temp400->locals=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, void* data)) Cyc_Dict_insert)( _temp400->locals,
_temp402,( void*)({ struct Cyc_Absyn_Local_b_struct* _temp414=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp414[ 0]=({ struct Cyc_Absyn_Local_b_struct
_temp415; _temp415.tag= Cyc_Absyn_Local_b; _temp415.f1= vd; _temp415;});
_temp414;})); return Cyc_Tcenv_put_fenv( te, _temp400);}}} struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_pat_var( struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Vardecl* vd){ if( te->le == 0){(( void(*)( struct
_tagged_string msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char*
_temp416=( char*)"Tcenv: unexpected add_pat_var"; struct _tagged_string _temp417;
_temp417.curr= _temp416; _temp417.base= _temp416; _temp417.last_plus_one=
_temp416 + 30; _temp417;}));}{ struct Cyc_Tcenv_Fenv* _temp418= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); goto _LL419; _LL419: { struct _tagged_string* _temp420=(*
vd->name).f2; goto _LL421; _LL421: _temp418->locals=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, void* data)) Cyc_Dict_insert)(
_temp418->locals, _temp420,( void*)({ struct Cyc_Absyn_Pat_b_struct* _temp422=(
struct Cyc_Absyn_Pat_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pat_b_struct));
_temp422[ 0]=({ struct Cyc_Absyn_Pat_b_struct _temp423; _temp423.tag= Cyc_Absyn_Pat_b;
_temp423.f1= vd; _temp423;}); _temp422;})); return Cyc_Tcenv_put_fenv( te,
_temp418);}}} void* Cyc_Tcenv_lookup_ordinary( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0 _temp426; struct
_tagged_string* _temp427; void* _temp429; struct _tuple0* _temp424= q; _temp426=*
_temp424; _LL430: _temp429= _temp426.f1; goto _LL428; _LL428: _temp427= _temp426.f2;
goto _LL425; _LL425: { void* _temp431= _temp429; struct Cyc_List_List* _temp440;
_LL433: if( _temp431 ==( void*) Cyc_Absyn_Loc_n){ goto _LL434;} else{ goto
_LL435;} _LL435: if(( unsigned int) _temp431 > 1u?*(( int*) _temp431) == Cyc_Absyn_Rel_n:
0){ _LL441: _temp440=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp431)->f1; if( _temp440 == 0){ goto _LL439;} else{ goto _LL437;}} else{ goto
_LL437;} _LL439: if( te->le != 0){ goto _LL436;} else{ goto _LL437;} _LL437:
goto _LL438; _LL434: if( te->le == 0){( void) _throw(( void*) Cyc_Dict_Absent);}
else{ goto _LL436;} _LL436: { struct Cyc_Tcenv_Fenv* _temp442= Cyc_Tcenv_get_fenv(
te); goto _LL443; _LL443: { struct _handler_cons _temp444; _push_handler(&
_temp444);{ int _temp446= 0; if( setjmp( _temp444.handler)){ _temp446= 1;} if( !
_temp446){{ void* _temp449=( void*)({ struct Cyc_Tcenv_VarRes_struct* _temp447=(
struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp447[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp448; _temp448.tag= Cyc_Tcenv_VarRes;
_temp448.f1=( void*)(( void*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string*
key)) Cyc_Dict_lookup)( _temp442->locals, _temp427); _temp448;}); _temp447;});
_npop_handler( 0u); return _temp449;}; _pop_handler();} else{ void* _temp445=(
void*) _exn_thrown; void* _temp451= _temp445; _LL453: if( _temp451 == Cyc_Dict_Absent){
goto _LL454;} else{ goto _LL455;} _LL455: goto _LL456; _LL454: return Cyc_Tcenv_lookup_ordinary_global(
te, loc, q); _LL456:( void) _throw( _temp451); _LL452:;}}}} _LL438: { struct
_handler_cons _temp457; _push_handler(& _temp457);{ int _temp459= 0; if( setjmp(
_temp457.handler)){ _temp459= 1;} if( ! _temp459){{ void* _temp460= Cyc_Tcenv_lookup_ordinary_global(
te, loc, q); _npop_handler( 0u); return _temp460;}; _pop_handler();} else{ void*
_temp458=( void*) _exn_thrown; void* _temp462= _temp458; _LL464: if( _temp462 ==
Cyc_Dict_Absent){ goto _LL465;} else{ goto _LL466;} _LL466: goto _LL467; _LL465:
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp468= Cyc_Absynpp_qvar2string(
q); xprintf("unbound identifer: %.*s", _temp468.last_plus_one - _temp468.curr,
_temp468.curr);})); return( void*)({ struct Cyc_Tcenv_VarRes_struct* _temp469=(
struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp469[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp470; _temp470.tag= Cyc_Tcenv_VarRes;
_temp470.f1=( void*)(( void*) Cyc_Absyn_Unresolved_b); _temp470;}); _temp469;});
_LL467:( void) _throw( _temp462); _LL463:;}}} _LL432:;}} void Cyc_Tcenv_process_continue(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Stmt**
sopt){ if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp471=( char*)"Tcenv: unexpected process_continue";
struct _tagged_string _temp472; _temp472.curr= _temp471; _temp472.base= _temp471;
_temp472.last_plus_one= _temp471 + 35; _temp472;}));}{ void* _temp473=( void*)((
Cyc_Tcenv_get_fenv( te))->ctrl_env)->continue_stmt; struct Cyc_Absyn_Stmt*
_temp483; _LL475: if(( unsigned int) _temp473 > 3u?*(( int*) _temp473) == Cyc_Tcenv_Stmt_j:
0){ _LL484: _temp483=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Tcenv_Stmt_j_struct*)
_temp473)->f1; goto _LL476;} else{ goto _LL477;} _LL477: if( _temp473 ==( void*)
Cyc_Tcenv_NotLoop_j){ goto _LL478;} else{ goto _LL479;} _LL479: if( _temp473 ==(
void*) Cyc_Tcenv_CaseEnd_j){ goto _LL480;} else{ goto _LL481;} _LL481: if(
_temp473 ==( void*) Cyc_Tcenv_FnEnd_j){ goto _LL482;} else{ goto _LL474;} _LL476:
_temp483->non_local_preds=({ struct Cyc_List_List* _temp485=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp485->hd=( void*) s; _temp485->tl=
_temp483->non_local_preds; _temp485;});* sopt=( struct Cyc_Absyn_Stmt*) _temp483;
return; _LL478: Cyc_Tcutil_terr( s->loc,( struct _tagged_string)({ char*
_temp486=( char*)"continue not in a loop"; struct _tagged_string _temp487;
_temp487.curr= _temp486; _temp487.base= _temp486; _temp487.last_plus_one=
_temp486 + 23; _temp487;})); return; _LL480:(( void(*)( struct _tagged_string
msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char* _temp488=( char*)"Tcenv: continue dest is CaseEnd";
struct _tagged_string _temp489; _temp489.curr= _temp488; _temp489.base= _temp488;
_temp489.last_plus_one= _temp488 + 32; _temp489;})); return; _LL482:(( void(*)(
struct _tagged_string msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char*
_temp490=( char*)"Tcenv: continue dest is CaseEnd"; struct _tagged_string
_temp491; _temp491.curr= _temp490; _temp491.base= _temp490; _temp491.last_plus_one=
_temp490 + 32; _temp491;})); return; _LL474:;}} void Cyc_Tcenv_process_break(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Stmt**
sopt){ if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp492=( char*)"Tcenv: unexpected process_break";
struct _tagged_string _temp493; _temp493.curr= _temp492; _temp493.base= _temp492;
_temp493.last_plus_one= _temp492 + 32; _temp493;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_get_fenv( te); void* _temp494=( void*)( fe->ctrl_env)->break_stmt;
struct Cyc_Absyn_Stmt* _temp504; _LL496: if(( unsigned int) _temp494 > 3u?*((
int*) _temp494) == Cyc_Tcenv_Stmt_j: 0){ _LL505: _temp504=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Tcenv_Stmt_j_struct*) _temp494)->f1; goto _LL497;} else{ goto _LL498;}
_LL498: if( _temp494 ==( void*) Cyc_Tcenv_NotLoop_j){ goto _LL499;} else{ goto
_LL500;} _LL500: if( _temp494 ==( void*) Cyc_Tcenv_FnEnd_j){ goto _LL501;} else{
goto _LL502;} _LL502: if( _temp494 ==( void*) Cyc_Tcenv_CaseEnd_j){ goto _LL503;}
else{ goto _LL495;} _LL497: _temp504->non_local_preds=({ struct Cyc_List_List*
_temp506=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp506->hd=( void*) s; _temp506->tl= _temp504->non_local_preds; _temp506;});*
sopt=( struct Cyc_Absyn_Stmt*) _temp504; return; _LL499: Cyc_Tcutil_terr( s->loc,(
struct _tagged_string)({ char* _temp507=( char*)"break not in a loop or switch";
struct _tagged_string _temp508; _temp508.curr= _temp507; _temp508.base= _temp507;
_temp508.last_plus_one= _temp507 + 30; _temp508;})); return; _LL501: if(( void*)(
fe->shared)->return_typ !=( void*) Cyc_Absyn_VoidType){ Cyc_Tcutil_terr( s->loc,(
struct _tagged_string)({ char* _temp509=( char*)"break causes function not to return a value";
struct _tagged_string _temp510; _temp510.curr= _temp509; _temp510.base= _temp509;
_temp510.last_plus_one= _temp509 + 44; _temp510;}));} return; _LL503: Cyc_Tcutil_terr(
s->loc,( struct _tagged_string)({ char* _temp511=( char*)"break causes outer switch clause to implicitly fallthru";
struct _tagged_string _temp512; _temp512.curr= _temp511; _temp512.base= _temp511;
_temp512.last_plus_one= _temp511 + 56; _temp512;})); return; _LL495:;}} void Cyc_Tcenv_process_goto(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s, struct _tagged_string* l,
struct Cyc_Absyn_Stmt** sopt){ if( te->le == 0){(( void(*)( struct
_tagged_string msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char*
_temp513=( char*)"Tcenv: unexpected process_goto"; struct _tagged_string
_temp514; _temp514.curr= _temp513; _temp514.base= _temp513; _temp514.last_plus_one=
_temp513 + 31; _temp514;}));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_get_fenv( te);
struct Cyc_Core_Opt* _temp515=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup_opt)(( fe->shared)->seen_labels,
l); goto _LL516; _LL516: if( _temp515 == 0){ struct Cyc_SlowDict_Dict* _temp517=(
fe->shared)->needed_labels; goto _LL518; _LL518: { struct Cyc_Core_Opt* _temp519=((
struct Cyc_Core_Opt*(*)( struct Cyc_SlowDict_Dict* d, struct _tagged_string* key))
Cyc_SlowDict_lookup_opt)( _temp517, l); goto _LL520; _LL520: if( _temp519 == 0){
_temp519=({ struct Cyc_Core_Opt* _temp521=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp521->v=( void*) 0; _temp521;});}( fe->shared)->needed_labels=((
struct Cyc_SlowDict_Dict*(*)( struct Cyc_SlowDict_Dict* d, struct _tagged_string*
key, struct Cyc_List_List* data)) Cyc_SlowDict_insert)( _temp517, l,({ struct
Cyc_List_List* _temp522=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp522->hd=( void*) s; _temp522->tl=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp523= _temp519; if( _temp523 == 0){ _throw( Null_Exception);} _temp523->v;});
_temp522;}));}} else{(( struct Cyc_Absyn_Stmt*)({ struct Cyc_Core_Opt* _temp524=
_temp515; if( _temp524 == 0){ _throw( Null_Exception);} _temp524->v;}))->non_local_preds=({
struct Cyc_List_List* _temp525=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp525->hd=( void*) s; _temp525->tl=(( struct Cyc_Absyn_Stmt*)({
struct Cyc_Core_Opt* _temp526= _temp515; if( _temp526 == 0){ _throw(
Null_Exception);} _temp526->v;}))->non_local_preds; _temp525;});* sopt=( struct
Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Stmt*)({ struct Cyc_Core_Opt* _temp527=
_temp515; if( _temp527 == 0){ _throw( Null_Exception);} _temp527->v;}));}}}
struct _tuple3* Cyc_Tcenv_process_fallthru( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Switch_clause*** clauseopt){ if( te->le == 0){((
void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({
char* _temp528=( char*)"Tcenv: unexpected process_fallthru"; struct
_tagged_string _temp529; _temp529.curr= _temp528; _temp529.base= _temp528;
_temp529.last_plus_one= _temp528 + 35; _temp529;}));}{ struct Cyc_Tcenv_Fenv*
_temp530= Cyc_Tcenv_get_fenv( te); goto _LL531; _LL531: { struct _tuple3*
_temp532=( _temp530->ctrl_env)->fallthru_clause; goto _LL533; _LL533: if(
_temp532 != 0){((({ struct _tuple3* _temp534= _temp532; if( _temp534 == 0){
_throw( Null_Exception);}* _temp534;}).f1)->body)->non_local_preds=({ struct Cyc_List_List*
_temp535=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp535->hd=( void*) s; _temp535->tl=((({ struct _tuple3* _temp536= _temp532;
if( _temp536 == 0){ _throw( Null_Exception);}* _temp536;}).f1)->body)->non_local_preds;
_temp535;});* clauseopt=({ struct Cyc_Absyn_Switch_clause** _temp537=( struct
Cyc_Absyn_Switch_clause**) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause*) *
1); _temp537[ 0]=({ struct _tuple3* _temp538= _temp532; if( _temp538 == 0){
_throw( Null_Exception);}* _temp538;}).f1; _temp537;});} return _temp532;}}}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_fallthru( struct Cyc_Tcenv_Tenv* te, struct
Cyc_List_List* new_tvs, struct Cyc_List_List* vds, struct Cyc_Absyn_Switch_clause*
clause){ if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp539=( char*)"Tcenv: unexpected set_fallthru";
struct _tagged_string _temp540; _temp540.curr= _temp539; _temp540.base= _temp539;
_temp540.last_plus_one= _temp539 + 31; _temp540;}));}{ struct Cyc_List_List*
_temp541= 0; goto _LL542; _LL542: for( 0; vds != 0; vds=({ struct Cyc_List_List*
_temp543= vds; if( _temp543 == 0){ _throw( Null_Exception);} _temp543->tl;})){
_temp541=({ struct Cyc_List_List* _temp544=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp544->hd=( void*)(( void*)(( struct Cyc_Absyn_Vardecl*)({
struct Cyc_List_List* _temp545= vds; if( _temp545 == 0){ _throw( Null_Exception);}
_temp545->hd;}))->type); _temp544->tl= _temp541; _temp544;});} _temp541=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp541);{
struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_new_ctrl( Cyc_Tcenv_get_fenv( te));(
fe->ctrl_env)->fallthru_clause=({ struct _tuple3* _temp546=( struct _tuple3*)
GC_malloc( sizeof( struct _tuple3)); _temp546->f1= clause; _temp546->f2= new_tvs;
_temp546->f3= _temp541; _temp546;}); return Cyc_Tcenv_put_fenv( te, fe);}}}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_clear_fallthru( struct Cyc_Tcenv_Tenv* te){ if(
te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(( struct
_tagged_string)({ char* _temp547=( char*)"Tcenv: unexpected clear_fallthru";
struct _tagged_string _temp548; _temp548.curr= _temp547; _temp548.base= _temp547;
_temp548.last_plus_one= _temp547 + 33; _temp548;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_new_ctrl( Cyc_Tcenv_get_fenv( te));( fe->ctrl_env)->fallthru_clause=
0; return Cyc_Tcenv_put_fenv( te, fe);}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_in_loop(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* continue_dest){ if( te->le ==
0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(( struct
_tagged_string)({ char* _temp549=( char*)"Tcenv: unexpected set_in_loop"; struct
_tagged_string _temp550; _temp550.curr= _temp549; _temp550.base= _temp549;
_temp550.last_plus_one= _temp549 + 30; _temp550;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_new_ctrl( Cyc_Tcenv_get_fenv( te));( void*)(( fe->ctrl_env)->continue_stmt=(
void*)(( void*)({ struct Cyc_Tcenv_Stmt_j_struct* _temp551=( struct Cyc_Tcenv_Stmt_j_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Stmt_j_struct)); _temp551[ 0]=({ struct Cyc_Tcenv_Stmt_j_struct
_temp552; _temp552.tag= Cyc_Tcenv_Stmt_j; _temp552.f1= continue_dest; _temp552;});
_temp551;})));( void*)(( fe->ctrl_env)->break_stmt=( void*)(( void*)( fe->ctrl_env)->next_stmt));(
void*)(( fe->ctrl_env)->next_stmt=( void*)(( void*)({ struct Cyc_Tcenv_Stmt_j_struct*
_temp553=( struct Cyc_Tcenv_Stmt_j_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Stmt_j_struct));
_temp553[ 0]=({ struct Cyc_Tcenv_Stmt_j_struct _temp554; _temp554.tag= Cyc_Tcenv_Stmt_j;
_temp554.f1= continue_dest; _temp554;}); _temp553;}))); return Cyc_Tcenv_put_fenv(
te, fe);}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_enter_try( struct Cyc_Tcenv_Tenv* te){
if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp555=( char*)"Tcenv: unexpected enter_try";
struct _tagged_string _temp556; _temp556.curr= _temp555; _temp556.base= _temp555;
_temp556.last_plus_one= _temp555 + 28; _temp556;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_new_ctrl( Cyc_Tcenv_get_fenv( te)); ++( fe->ctrl_env)->try_depth;
return Cyc_Tcenv_put_fenv( te, fe);}} int Cyc_Tcenv_get_try_depth( struct Cyc_Tcenv_Tenv*
te){ if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp557=( char*)"Tcenv: unexpected get_try_depth";
struct _tagged_string _temp558; _temp558.curr= _temp557; _temp558.base= _temp557;
_temp558.last_plus_one= _temp557 + 32; _temp558;}));} return(( Cyc_Tcenv_get_fenv(
te))->ctrl_env)->try_depth;} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_in_switch(
struct Cyc_Tcenv_Tenv* te){ if( te->le == 0){(( void(*)( struct _tagged_string
msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char* _temp559=( char*)"Tcenv: unexpected set_in_switch";
struct _tagged_string _temp560; _temp560.curr= _temp559; _temp560.base= _temp559;
_temp560.last_plus_one= _temp559 + 32; _temp560;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_new_ctrl( Cyc_Tcenv_get_fenv( te));( void*)(( fe->ctrl_env)->break_stmt=(
void*)(( void*)( fe->ctrl_env)->next_stmt));( void*)(( fe->ctrl_env)->next_stmt=(
void*)(( void*) Cyc_Tcenv_CaseEnd_j)); return Cyc_Tcenv_put_fenv( te, fe);}}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_next( struct Cyc_Tcenv_Tenv* te, void* j){
if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp561=( char*)"Tcenv: unexpected set_next";
struct _tagged_string _temp562; _temp562.curr= _temp561; _temp562.base= _temp561;
_temp562.last_plus_one= _temp561 + 27; _temp562;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_new_ctrl( Cyc_Tcenv_get_fenv( te));( void*)(( fe->ctrl_env)->next_stmt=(
void*) j); return Cyc_Tcenv_put_fenv( te, fe);}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_label(
struct Cyc_Tcenv_Tenv* te, struct _tagged_string* v, struct Cyc_Absyn_Stmt* s){
if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp563=( char*)"Tcenv: unexpected add_label";
struct _tagged_string _temp564; _temp564.curr= _temp563; _temp564.base= _temp563;
_temp564.last_plus_one= _temp563 + 28; _temp564;}));}{ struct Cyc_Tcenv_Fenv*
_temp565= Cyc_Tcenv_get_fenv( te); goto _LL566; _LL566: { struct Cyc_SlowDict_Dict*
_temp567=( _temp565->shared)->needed_labels; goto _LL568; _LL568: { struct Cyc_Core_Opt*
_temp569=(( struct Cyc_Core_Opt*(*)( struct Cyc_SlowDict_Dict* d, struct
_tagged_string* key)) Cyc_SlowDict_lookup_opt)( _temp567, v); goto _LL570;
_LL570: if( _temp569 != 0){( _temp565->shared)->needed_labels=(( struct Cyc_SlowDict_Dict*(*)(
struct Cyc_SlowDict_Dict* d, struct _tagged_string* key)) Cyc_SlowDict_delete)(
_temp567, v);{ struct Cyc_List_List* _temp572=( struct Cyc_List_List*)({ struct
Cyc_Core_Opt* _temp571= _temp569; if( _temp571 == 0){ _throw( Null_Exception);}
_temp571->v;}); goto _LL573; _LL573: s->non_local_preds= _temp572; for( 0;
_temp572 != 0; _temp572=({ struct Cyc_List_List* _temp574= _temp572; if(
_temp574 == 0){ _throw( Null_Exception);} _temp574->tl;})){ void* _temp576=(
void*)(( struct Cyc_Absyn_Stmt*)({ struct Cyc_List_List* _temp575= _temp572; if(
_temp575 == 0){ _throw( Null_Exception);} _temp575->hd;}))->r; struct Cyc_Absyn_Stmt*
_temp582; struct Cyc_Absyn_Stmt** _temp584; struct _tagged_string* _temp585;
_LL578: if(( unsigned int) _temp576 > 1u?*(( int*) _temp576) == Cyc_Absyn_Goto_s:
0){ _LL586: _temp585=( struct _tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp576)->f1; goto _LL583; _LL583: _temp582=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_Goto_s_struct*) _temp576)->f2; _temp584=&(( struct Cyc_Absyn_Goto_s_struct*)
_temp576)->f2; goto _LL579;} else{ goto _LL580;} _LL580: goto _LL581; _LL579:*
_temp584=( struct Cyc_Absyn_Stmt*) s; goto _LL577; _LL581:(( void(*)( struct
_tagged_string msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char*
_temp587=( char*)"Tcenv: add_label backpatching of non-goto"; struct
_tagged_string _temp588; _temp588.curr= _temp587; _temp588.base= _temp587;
_temp588.last_plus_one= _temp587 + 42; _temp588;})); goto _LL577; _LL577:;}}}
if((( int(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_member)((
_temp565->shared)->seen_labels, v)){ Cyc_Tcutil_terr( s->loc,({ struct
_tagged_string _temp589=* v; xprintf("Repeated label: %.*s", _temp589.last_plus_one
- _temp589.curr, _temp589.curr);}));}( _temp565->shared)->seen_labels=(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct
Cyc_Absyn_Stmt* data)) Cyc_Dict_insert)(( _temp565->shared)->seen_labels, v, s);
return te;}}}} int Cyc_Tcenv_all_labels_resolved( struct Cyc_Tcenv_Tenv* te){
return(( int(*)( struct Cyc_SlowDict_Dict* d)) Cyc_SlowDict_is_empty)((( Cyc_Tcenv_get_fenv(
te))->shared)->needed_labels);} struct Cyc_Absyn_Stmt* Cyc_Tcenv_get_encloser(
struct Cyc_Tcenv_Tenv* te){ if( te->le == 0){(( void(*)( struct _tagged_string
msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char* _temp590=( char*)"Tcenv: unexpected get_encloser";
struct _tagged_string _temp591; _temp591.curr= _temp590; _temp591.base= _temp590;
_temp591.last_plus_one= _temp590 + 31; _temp591;}));} return( Cyc_Tcenv_get_fenv(
te))->encloser;} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_encloser( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Stmt* s){ if( te->le == 0){(( void(*)( struct
_tagged_string msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char*
_temp592=( char*)"Tcenv: unexpected set_encloser"; struct _tagged_string
_temp593; _temp593.curr= _temp592; _temp593.base= _temp592; _temp593.last_plus_one=
_temp592 + 31; _temp593;}));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); fe->encloser= s; return Cyc_Tcenv_put_fenv( te, fe);}}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_region( struct Cyc_Tcenv_Tenv* te, void* r){
if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp594=( char*)"Tcenv::add_region: unexpected add_region";
struct _tagged_string _temp595; _temp595.curr= _temp594; _temp595.base= _temp594;
_temp595.last_plus_one= _temp594 + 41; _temp595;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_old_ctrl( Cyc_Tcenv_get_fenv( te));( void*)( fe->capability=(
void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp596=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp596[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp597; _temp597.tag= Cyc_Absyn_JoinEff; _temp597.f1=({ struct Cyc_List_List*
_temp598=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp598->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp600=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp600[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp601; _temp601.tag= Cyc_Absyn_AccessEff;
_temp601.f1=( void*) r; _temp601;}); _temp600;})); _temp598->tl=({ struct Cyc_List_List*
_temp599=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp599->hd=( void*)(( void*) fe->capability); _temp599->tl= 0; _temp599;});
_temp598;}); _temp597;}); _temp596;}))); return Cyc_Tcenv_put_fenv( te, fe);}}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_new_named_block( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Tvar* block_rgn){ if( te->le ==
0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(( struct
_tagged_string)({ char* _temp602=( char*)"Tcenv: unexpected new_block"; struct
_tagged_string _temp603; _temp603.curr= _temp602; _temp603.base= _temp602;
_temp603.last_plus_one= _temp602 + 28; _temp603;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_old_ctrl( Cyc_Tcenv_get_fenv( te)); void* block_typ=( void*)({
struct Cyc_Absyn_VarType_struct* _temp611=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp611[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp612; _temp612.tag= Cyc_Absyn_VarType; _temp612.f1= block_rgn; _temp612;});
_temp611;}); fe->type_vars=({ struct Cyc_List_List* _temp604=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp604->hd=( void*) block_rgn;
_temp604->tl= fe->type_vars; _temp604;}); Cyc_Tcutil_check_unique_tvars( loc, fe->type_vars);(
void*)( fe->capability=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp605=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp605[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp606; _temp606.tag= Cyc_Absyn_JoinEff;
_temp606.f1=({ struct Cyc_List_List* _temp607=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp607->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp609=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp609[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp610; _temp610.tag= Cyc_Absyn_AccessEff;
_temp610.f1=( void*) block_typ; _temp610;}); _temp609;})); _temp607->tl=({
struct Cyc_List_List* _temp608=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp608->hd=( void*)(( void*) fe->capability); _temp608->tl=
0; _temp608;}); _temp607;}); _temp606;}); _temp605;})));( void*)( fe->curr_rgn=(
void*) block_typ); return Cyc_Tcenv_put_fenv( te, fe);}} struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_new_block( struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te){
return Cyc_Tcenv_new_named_block( loc, te, Cyc_Tcutil_new_tvar(( void*) Cyc_Absyn_RgnKind));}
void* Cyc_Tcenv_curr_rgn( struct Cyc_Tcenv_Tenv* te){ if( te->le == 0){ return(
void*) Cyc_Absyn_HeapRgn;} return( void*)( Cyc_Tcenv_get_fenv( te))->curr_rgn;}
void Cyc_Tcenv_check_rgn_accessible( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void* rgn){ if( te->le != 0){ void* capability=( void*)( Cyc_Tcenv_get_fenv(
te))->capability; if( Cyc_Tcutil_region_in_effect( 0, rgn, capability)? 1: Cyc_Tcutil_region_in_effect(
1, rgn, capability)){ return;}} Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp613= Cyc_Absynpp_typ2string( rgn); xprintf("Expression accesses unavailable region %.*s",
_temp613.last_plus_one - _temp613.curr, _temp613.curr);}));} void Cyc_Tcenv_check_effect_accessible(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* eff){ void*
capability; if( te->le == 0){ capability=( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp614=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp614[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp615; _temp615.tag= Cyc_Absyn_JoinEff;
_temp615.f1= 0; _temp615;}); _temp614;});} else{ struct Cyc_Tcenv_Fenv* _temp616=
Cyc_Tcenv_get_fenv( te); goto _LL617; _LL617: capability=( void*) _temp616->capability;}
if( ! Cyc_Tcutil_subset_effect( 1, eff, capability)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp618= Cyc_Absynpp_typ2string( capability); struct
_tagged_string _temp619= Cyc_Absynpp_typ2string( eff); xprintf("Capability %.*s does not cover function's effect %.*s",
_temp618.last_plus_one - _temp618.curr, _temp618.curr, _temp619.last_plus_one -
_temp619.curr, _temp619.curr);}));}} struct _tuple5{ struct _tagged_string* f1;
struct Cyc_Absyn_Tqual f2; void* f3; } ; struct Cyc_Tcenv_Fenv* Cyc_Tcenv_new_fenv(
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Fndecl* fd){ struct Cyc_Dict_Dict*
locals=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_string*, struct
_tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); struct Cyc_Absyn_Tvar*
rgn0=({ struct Cyc_Absyn_Tvar* _temp658=( struct Cyc_Absyn_Tvar*) GC_malloc(
sizeof( struct Cyc_Absyn_Tvar)); _temp658->name=({ struct _tagged_string*
_temp659=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string));
_temp659[ 0]=({ struct _tagged_string _temp660=*(* fd->name).f2; xprintf("`%.*s",
_temp660.last_plus_one - _temp660.curr, _temp660.curr);}); _temp659;}); _temp658->kind=((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind);
_temp658;}); struct Cyc_List_List* _temp621=({ struct Cyc_List_List* _temp620=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp620->hd=(
void*) rgn0; _temp620->tl= fd->tvs; _temp620;}); goto _LL622; _LL622: Cyc_Tcutil_check_unique_tvars(
loc, _temp621);{ void* param_rgn=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp656=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp656[ 0]=({ struct Cyc_Absyn_VarType_struct _temp657; _temp657.tag= Cyc_Absyn_VarType;
_temp657.f1= rgn0; _temp657;}); _temp656;}); struct Cyc_List_List* _temp623= 0;
goto _LL624; _LL624:{ struct Cyc_List_List* _temp625= fd->args; goto _LL626;
_LL626: for( 0; _temp625 != 0; _temp625=({ struct Cyc_List_List* _temp627=
_temp625; if( _temp627 == 0){ _throw( Null_Exception);} _temp627->tl;})){ struct
Cyc_Absyn_Vardecl* _temp637=({ struct Cyc_Absyn_Vardecl* _temp628=( struct Cyc_Absyn_Vardecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Vardecl) * 1); _temp628[ 0]=({ struct Cyc_Absyn_Vardecl
_temp629; _temp629.sc=( void*)(( void*) Cyc_Absyn_Public); _temp629.name=({
struct _tuple0* _temp634=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0) *
1); _temp634[ 0]=({ struct _tuple0 _temp635; _temp635.f1=( void*) Cyc_Absyn_Loc_n;
_temp635.f2=(*(( struct _tuple5*)({ struct Cyc_List_List* _temp636= _temp625;
if( _temp636 == 0){ _throw( Null_Exception);} _temp636->hd;}))).f1; _temp635;});
_temp634;}); _temp629.tq=(*(( struct _tuple5*)({ struct Cyc_List_List* _temp633=
_temp625; if( _temp633 == 0){ _throw( Null_Exception);} _temp633->hd;}))).f2;
_temp629.type=( void*)(*(( struct _tuple5*)({ struct Cyc_List_List* _temp632=
_temp625; if( _temp632 == 0){ _throw( Null_Exception);} _temp632->hd;}))).f3;
_temp629.initializer= 0; _temp629.rgn=({ struct Cyc_Core_Opt* _temp630=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt) * 1); _temp630[ 0]=({
struct Cyc_Core_Opt _temp631; _temp631.v=( void*) param_rgn; _temp631;});
_temp630;}); _temp629.attributes= 0; _temp629;}); _temp628;}); goto _LL638;
_LL638: _temp623=({ struct Cyc_List_List* _temp639=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp639[ 0]=({ struct Cyc_List_List
_temp640; _temp640.hd=( void*) _temp637; _temp640.tl= _temp623; _temp640;});
_temp639;}); locals=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key, void* data)) Cyc_Dict_insert)( locals,(*(( struct _tuple5*)({
struct Cyc_List_List* _temp641= _temp625; if( _temp641 == 0){ _throw(
Null_Exception);} _temp641->hd;}))).f1,( void*)({ struct Cyc_Absyn_Param_b_struct*
_temp642=( struct Cyc_Absyn_Param_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Param_b_struct));
_temp642[ 0]=({ struct Cyc_Absyn_Param_b_struct _temp643; _temp643.tag= Cyc_Absyn_Param_b;
_temp643.f1= _temp637; _temp643;}); _temp642;}));}} fd->param_vardecls=({ struct
Cyc_Core_Opt* _temp644=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)
* 1); _temp644[ 0]=({ struct Cyc_Core_Opt _temp645; _temp645.v=( void*) _temp623;
_temp645;}); _temp644;}); return({ struct Cyc_Tcenv_Fenv* _temp646=( struct Cyc_Tcenv_Fenv*)
GC_malloc( sizeof( struct Cyc_Tcenv_Fenv)); _temp646->shared=({ struct Cyc_Tcenv_SharedFenv*
_temp655=( struct Cyc_Tcenv_SharedFenv*) GC_malloc( sizeof( struct Cyc_Tcenv_SharedFenv));
_temp655->return_typ=( void*)(( void*) fd->ret_type); _temp655->seen_labels=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_string*, struct
_tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp655->needed_labels=((
struct Cyc_SlowDict_Dict*(*)( int(* comp)( struct _tagged_string*, struct
_tagged_string*))) Cyc_SlowDict_empty)( Cyc_String_zstrptrcmp); _temp655;});
_temp646->type_vars= _temp621; _temp646->locals= locals; _temp646->encloser= fd->body;
_temp646->ctrl_env=({ struct Cyc_Tcenv_CtrlEnv* _temp654=( struct Cyc_Tcenv_CtrlEnv*)
GC_malloc( sizeof( struct Cyc_Tcenv_CtrlEnv)); _temp654->continue_stmt=( void*)((
void*) Cyc_Tcenv_NotLoop_j); _temp654->break_stmt=( void*)(( void*) Cyc_Tcenv_NotLoop_j);
_temp654->fallthru_clause= 0; _temp654->next_stmt=( void*)(( void*) Cyc_Tcenv_FnEnd_j);
_temp654->try_depth= 0; _temp654;}); _temp646->capability=( void*)(( void*)({
struct Cyc_Absyn_JoinEff_struct* _temp647=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp647[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp648; _temp648.tag= Cyc_Absyn_JoinEff; _temp648.f1=({ struct Cyc_List_List*
_temp649=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp649->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp652=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp652[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp653; _temp653.tag= Cyc_Absyn_AccessEff;
_temp653.f1=( void*) param_rgn; _temp653;}); _temp652;})); _temp649->tl=({
struct Cyc_List_List* _temp650=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp650->hd=( void*)(( void*)({ struct Cyc_Core_Opt*
_temp651= fd->effect; if( _temp651 == 0){ _throw( Null_Exception);} _temp651->v;}));
_temp650->tl= 0; _temp650;}); _temp649;}); _temp648;}); _temp647;})); _temp646->curr_rgn=(
void*) param_rgn; _temp646;});}}