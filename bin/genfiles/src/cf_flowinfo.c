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
struct Cyc_Core_Opt* Cyc_Core_opt_t; extern int Cyc_Core_intcmp( int, int);
extern int Cyc_Core_ptrcmp( void**, void**); extern char Cyc_Core_InvalidArg[ 15u];
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
_temp159= _temp155->typs; goto _LL142; _LL142: { struct Cyc_Dict_Dict* _temp184=((
struct Cyc_Dict_Dict*(*)( int(* comp)( int, int))) Cyc_Dict_empty)( Cyc_Core_intcmp);
goto _LL185; _LL185:{ int i= 0; for( 0; _temp159 != 0; _temp159=({ struct Cyc_List_List*
_temp186= _temp159; if( _temp186 == 0){ _throw( Null_Exception);} _temp186->tl;}),
++ i){ struct _tuple4 _temp190; void* _temp191; struct Cyc_Absyn_Tqual _temp193;
struct _tuple4* _temp188=( struct _tuple4*)({ struct Cyc_List_List* _temp187=
_temp159; if( _temp187 == 0){ _throw( Null_Exception);} _temp187->hd;});
_temp190=* _temp188; _LL194: _temp193= _temp190.f1; goto _LL192; _LL192:
_temp191= _temp190.f2; goto _LL189; _LL189: _temp184=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, int key, void* data)) Cyc_Dict_insert)( _temp184, i,
Cyc_CfFlowInfo_typ_to_unesc_none_pinfo( _temp191));}} return( void*)({ struct
Cyc_CfFlowInfo_TuplePI_struct* _temp195=( struct Cyc_CfFlowInfo_TuplePI_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct)); _temp195[ 0]=({
struct Cyc_CfFlowInfo_TuplePI_struct _temp196; _temp196.tag= Cyc_CfFlowInfo_TuplePI;
_temp196.f1= _temp184; _temp196;}); _temp195;});} _LL144: _temp172= _temp161;
goto _LL146; _LL146: { struct Cyc_Dict_Dict* _temp197=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); goto _LL198; _LL198: for( 0; _temp172 != 0; _temp172=({
struct Cyc_List_List* _temp199= _temp172; if( _temp199 == 0){ _throw(
Null_Exception);} _temp199->tl;})){ struct Cyc_Absyn_Structfield _temp203;
struct Cyc_List_List* _temp204; struct Cyc_Core_Opt* _temp206; void* _temp208;
struct Cyc_Absyn_Tqual _temp210; struct _tagged_string* _temp212; struct Cyc_Absyn_Structfield*
_temp201=( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp200=
_temp172; if( _temp200 == 0){ _throw( Null_Exception);} _temp200->hd;});
_temp203=* _temp201; _LL213: _temp212=( struct _tagged_string*) _temp203.name;
goto _LL211; _LL211: _temp210=( struct Cyc_Absyn_Tqual) _temp203.tq; goto _LL209;
_LL209: _temp208=( void*) _temp203.type; goto _LL207; _LL207: _temp206=( struct
Cyc_Core_Opt*) _temp203.width; goto _LL205; _LL205: _temp204=( struct Cyc_List_List*)
_temp203.attributes; goto _LL202; _LL202: if( Cyc_String_strcmp(* _temp212,(
struct _tagged_string)({ char* _temp214=( char*)""; struct _tagged_string
_temp215; _temp215.curr= _temp214; _temp215.base= _temp214; _temp215.last_plus_one=
_temp214 + 1; _temp215;})) != 0){ _temp197=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key, void* data)) Cyc_Dict_insert)(
_temp197, _temp212, Cyc_CfFlowInfo_typ_to_unesc_none_pinfo( _temp208));}} return(
void*)({ struct Cyc_CfFlowInfo_StructPI_struct* _temp216=( struct Cyc_CfFlowInfo_StructPI_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct)); _temp216[ 0]=({
struct Cyc_CfFlowInfo_StructPI_struct _temp217; _temp217.tag= Cyc_CfFlowInfo_StructPI;
_temp217.f1= _temp197; _temp217;}); _temp216;});} _LL148: return Cyc_CfFlowInfo_leaf_unesc_none;
_LL138:;} void* Cyc_CfFlowInfo_lookup_place( struct Cyc_Dict_Dict* d, struct Cyc_CfFlowInfo_Place*
place){ struct Cyc_CfFlowInfo_Place _temp220; struct Cyc_List_List* _temp221;
void* _temp223; struct Cyc_CfFlowInfo_Place* _temp218= place; _temp220=*
_temp218; _LL224: _temp223=( void*) _temp220.root; goto _LL222; _LL222: _temp221=(
struct Cyc_List_List*) _temp220.fields; goto _LL219; _LL219: { void* _temp225=((
void*(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( d, _temp223);
goto _LL226; _LL226: for( 0; _temp221 != 0; _temp221=({ struct Cyc_List_List*
_temp227= _temp221; if( _temp227 == 0){ _throw( Null_Exception);} _temp227->tl;})){
struct _tuple3 _temp230=({ struct _tuple3 _temp228; _temp228.f1= _temp225;
_temp228.f2=( void*)({ struct Cyc_List_List* _temp229= _temp221; if( _temp229 ==
0){ _throw( Null_Exception);} _temp229->hd;}); _temp228;}); void* _temp238; int
_temp240; void* _temp242; struct Cyc_Dict_Dict* _temp244; void* _temp246; struct
_tagged_string* _temp248; void* _temp250; struct Cyc_Dict_Dict* _temp252; _LL232:
_LL243: _temp242= _temp230.f1; if(*(( int*) _temp242) == Cyc_CfFlowInfo_TuplePI){
_LL245: _temp244=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_TuplePI_struct*)
_temp242)->f1; goto _LL239;} else{ goto _LL234;} _LL239: _temp238= _temp230.f2;
if(*(( int*) _temp238) == Cyc_CfFlowInfo_TupleF){ _LL241: _temp240=( int)((
struct Cyc_CfFlowInfo_TupleF_struct*) _temp238)->f1; goto _LL233;} else{ goto
_LL234;} _LL234: _LL251: _temp250= _temp230.f1; if(*(( int*) _temp250) == Cyc_CfFlowInfo_StructPI){
_LL253: _temp252=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp250)->f1; goto _LL247;} else{ goto _LL236;} _LL247: _temp246= _temp230.f2;
if(*(( int*) _temp246) == Cyc_CfFlowInfo_StructF){ _LL249: _temp248=( struct
_tagged_string*)(( struct Cyc_CfFlowInfo_StructF_struct*) _temp246)->f1; goto
_LL235;} else{ goto _LL236;} _LL236: goto _LL237; _LL233: _temp225=(( void*(*)(
struct Cyc_Dict_Dict* d, int key)) Cyc_Dict_lookup)( _temp244, _temp240); goto
_LL231; _LL235: _temp225=(( void*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup)( _temp252, _temp248); goto _LL231; _LL237:(
void) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp254=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp254[ 0]=({ struct
Cyc_Core_Impossible_struct _temp255; _temp255.tag= Cyc_Core_Impossible; _temp255.f1=(
struct _tagged_string)({ char* _temp256=( char*)"bad lookup_place"; struct
_tagged_string _temp257; _temp257.curr= _temp256; _temp257.base= _temp256;
_temp257.last_plus_one= _temp256 + 17; _temp257;}); _temp255;}); _temp254;}));
_LL231:;} return _temp225;}} struct _tuple5{ void* f1; struct Cyc_List_List* f2;
} ; void* Cyc_CfFlowInfo_insert_place_rec( void* insert_path, struct Cyc_List_List*
fields, void* pinfo){ if( fields == 0){ return pinfo;}{ struct _tuple5 _temp259=({
struct _tuple5 _temp258; _temp258.f1= insert_path; _temp258.f2= fields; _temp258;});
struct Cyc_List_List* _temp267; struct Cyc_List_List _temp269; struct Cyc_List_List*
_temp270; void* _temp272; int _temp274; void* _temp276; struct Cyc_Dict_Dict*
_temp278; struct Cyc_List_List* _temp280; struct Cyc_List_List _temp282; struct
Cyc_List_List* _temp283; void* _temp285; struct _tagged_string* _temp287; void*
_temp289; struct Cyc_Dict_Dict* _temp291; _LL261: _LL277: _temp276= _temp259.f1;
if(*(( int*) _temp276) == Cyc_CfFlowInfo_TuplePI){ _LL279: _temp278=( struct Cyc_Dict_Dict*)((
struct Cyc_CfFlowInfo_TuplePI_struct*) _temp276)->f1; goto _LL268;} else{ goto
_LL263;} _LL268: _temp267= _temp259.f2; if( _temp267 == 0){ goto _LL263;} else{
_temp269=* _temp267; _LL273: _temp272=( void*) _temp269.hd; if(*(( int*)
_temp272) == Cyc_CfFlowInfo_TupleF){ _LL275: _temp274=( int)(( struct Cyc_CfFlowInfo_TupleF_struct*)
_temp272)->f1; goto _LL271;} else{ goto _LL263;} _LL271: _temp270=( struct Cyc_List_List*)
_temp269.tl; goto _LL262;} _LL263: _LL290: _temp289= _temp259.f1; if(*(( int*)
_temp289) == Cyc_CfFlowInfo_StructPI){ _LL292: _temp291=( struct Cyc_Dict_Dict*)((
struct Cyc_CfFlowInfo_StructPI_struct*) _temp289)->f1; goto _LL281;} else{ goto
_LL265;} _LL281: _temp280= _temp259.f2; if( _temp280 == 0){ goto _LL265;} else{
_temp282=* _temp280; _LL286: _temp285=( void*) _temp282.hd; if(*(( int*)
_temp285) == Cyc_CfFlowInfo_StructF){ _LL288: _temp287=( struct _tagged_string*)((
struct Cyc_CfFlowInfo_StructF_struct*) _temp285)->f1; goto _LL284;} else{ goto
_LL265;} _LL284: _temp283=( struct Cyc_List_List*) _temp282.tl; goto _LL264;}
_LL265: goto _LL266; _LL262: { void* _temp293=(( void*(*)( struct Cyc_Dict_Dict*
d, int key)) Cyc_Dict_lookup)( _temp278, _temp274); goto _LL294; _LL294: return(
void*)({ struct Cyc_CfFlowInfo_TuplePI_struct* _temp295=( struct Cyc_CfFlowInfo_TuplePI_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct)); _temp295[ 0]=({
struct Cyc_CfFlowInfo_TuplePI_struct _temp296; _temp296.tag= Cyc_CfFlowInfo_TuplePI;
_temp296.f1=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, int key, void*
data)) Cyc_Dict_insert)( _temp278, _temp274, Cyc_CfFlowInfo_insert_place_rec(
_temp293, _temp270, pinfo)); _temp296;}); _temp295;});} _LL264: { void* _temp297=((
void*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)(
_temp291, _temp287); goto _LL298; _LL298: return( void*)({ struct Cyc_CfFlowInfo_StructPI_struct*
_temp299=( struct Cyc_CfFlowInfo_StructPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct));
_temp299[ 0]=({ struct Cyc_CfFlowInfo_StructPI_struct _temp300; _temp300.tag=
Cyc_CfFlowInfo_StructPI; _temp300.f1=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, void* data)) Cyc_Dict_insert)( _temp291, _temp287,
Cyc_CfFlowInfo_insert_place_rec( _temp297, _temp283, pinfo)); _temp300;});
_temp299;});} _LL266:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp301=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp301[ 0]=({ struct Cyc_Core_Impossible_struct _temp302; _temp302.tag= Cyc_Core_Impossible;
_temp302.f1=( struct _tagged_string)({ char* _temp303=( char*)"bad insert_place";
struct _tagged_string _temp304; _temp304.curr= _temp303; _temp304.base= _temp303;
_temp304.last_plus_one= _temp303 + 17; _temp304;}); _temp302;}); _temp301;}));
_LL260:;}} struct Cyc_Dict_Dict* Cyc_CfFlowInfo_insert_place( struct Cyc_Dict_Dict*
d, struct Cyc_CfFlowInfo_Place* place, void* pinfo){ struct Cyc_CfFlowInfo_Place
_temp307; struct Cyc_List_List* _temp308; void* _temp310; struct Cyc_CfFlowInfo_Place*
_temp305= place; _temp307=* _temp305; _LL311: _temp310=( void*) _temp307.root;
goto _LL309; _LL309: _temp308=( struct Cyc_List_List*) _temp307.fields; goto
_LL306; _LL306: return(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void*
key, void* data)) Cyc_Dict_insert)( d, _temp310, Cyc_CfFlowInfo_insert_place_rec(((
void*(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( d, _temp310),
_temp308, pinfo));} struct _tuple6{ struct Cyc_Dict_Dict* f1; struct Cyc_List_List*
f2; } ; int Cyc_CfFlowInfo_isAllInit_pinfo( struct _tuple6* env, void* path);
int Cyc_CfFlowInfo_isAllInit_is( struct _tuple6* env, void* state){ void*
_temp312= state; void* _temp320; void* _temp322; void* _temp324; void* _temp326;
struct Cyc_CfFlowInfo_Place* _temp328; _LL314: if(*(( int*) _temp312) == Cyc_CfFlowInfo_UnknownIS){
_LL323: _temp322=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp312)->f1;
goto _LL321; _LL321: _temp320=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp312)->f2; if( _temp320 ==( void*) Cyc_CfFlowInfo_AllIL){ goto _LL315;}
else{ goto _LL316;}} else{ goto _LL316;} _LL316: if(*(( int*) _temp312) == Cyc_CfFlowInfo_UnknownIS){
_LL327: _temp326=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp312)->f1;
goto _LL325; _LL325: _temp324=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp312)->f2; goto _LL317;} else{ goto _LL318;} _LL318: if(*(( int*) _temp312)
== Cyc_CfFlowInfo_MustPointTo){ _LL329: _temp328=( struct Cyc_CfFlowInfo_Place*)((
struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp312)->f1; goto _LL319;} else{
goto _LL313;} _LL315: return 1; _LL317: return 0; _LL319: { struct _tuple6
_temp332; struct Cyc_List_List* _temp333; struct Cyc_List_List** _temp335;
struct Cyc_Dict_Dict* _temp336; struct _tuple6* _temp330= env; _temp332=*
_temp330; _LL337: _temp336= _temp332.f1; goto _LL334; _LL334: _temp333= _temp332.f2;
_temp335=&(* _temp330).f2; goto _LL331; _LL331: if((( int(*)( int(* compare)(
struct Cyc_CfFlowInfo_Place*, struct Cyc_CfFlowInfo_Place*), struct Cyc_List_List*
l, struct Cyc_CfFlowInfo_Place* x)) Cyc_List_mem)( Cyc_CfFlowInfo_place_cmp,*
_temp335, _temp328)){ return 1;}* _temp335=({ struct Cyc_List_List* _temp338=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp338[
0]=({ struct Cyc_List_List _temp339; _temp339.hd=( void*) _temp328; _temp339.tl=*
_temp335; _temp339;}); _temp338;}); return Cyc_CfFlowInfo_isAllInit_pinfo( env,
Cyc_CfFlowInfo_lookup_place( _temp336, _temp328));} _LL313:;} int Cyc_CfFlowInfo_isAllInit_f(
struct _tuple6* env, void* a, void* path){ return Cyc_CfFlowInfo_isAllInit_pinfo(
env, path);} int Cyc_CfFlowInfo_isAllInit_pinfo( struct _tuple6* env, void* path){
void* _temp340= path; void* _temp348; struct Cyc_Dict_Dict* _temp350; struct Cyc_Dict_Dict*
_temp352; _LL342: if(*(( int*) _temp340) == Cyc_CfFlowInfo_LeafPI){ _LL349:
_temp348=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp340)->f1; goto
_LL343;} else{ goto _LL344;} _LL344: if(*(( int*) _temp340) == Cyc_CfFlowInfo_TuplePI){
_LL351: _temp350=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_TuplePI_struct*)
_temp340)->f1; goto _LL345;} else{ goto _LL346;} _LL346: if(*(( int*) _temp340)
== Cyc_CfFlowInfo_StructPI){ _LL353: _temp352=( struct Cyc_Dict_Dict*)(( struct
Cyc_CfFlowInfo_StructPI_struct*) _temp340)->f1; goto _LL347;} else{ goto _LL341;}
_LL343: return Cyc_CfFlowInfo_isAllInit_is( env, _temp348); _LL345: return(( int(*)(
int(* f)( struct _tuple6*, int, void*), struct _tuple6* env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_forall_c)(( int(*)( struct _tuple6* env, int a, void* path)) Cyc_CfFlowInfo_isAllInit_f,
env, _temp350); _LL347: return(( int(*)( int(* f)( struct _tuple6*, struct
_tagged_string*, void*), struct _tuple6* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_forall_c)((
int(*)( struct _tuple6* env, struct _tagged_string* a, void* path)) Cyc_CfFlowInfo_isAllInit_f,
env, _temp352); _LL341:;} int Cyc_CfFlowInfo_isAllInit( struct Cyc_Dict_Dict*
pinfo_dict, void* pinfo){ struct _tuple6 _temp355=({ struct _tuple6 _temp354;
_temp354.f1= pinfo_dict; _temp354.f2= 0; _temp354;}); goto _LL356; _LL356:
return Cyc_CfFlowInfo_isAllInit_pinfo(& _temp355, pinfo);} struct _tuple7{
struct Cyc_Dict_Dict* f1; struct Cyc_Set_Set** f2; } ; void* Cyc_CfFlowInfo_pinfo_getplaces_esc(
struct _tuple7* env, void* pinfo){ struct _tuple7 _temp359; struct Cyc_Set_Set**
_temp360; struct Cyc_Dict_Dict* _temp362; struct _tuple7* _temp357= env;
_temp359=* _temp357; _LL363: _temp362= _temp359.f1; goto _LL361; _LL361:
_temp360= _temp359.f2; goto _LL358; _LL358: { void* _temp364= pinfo; void*
_temp374; struct Cyc_CfFlowInfo_Place* _temp376; void* _temp378; void* _temp380;
void* _temp382; struct Cyc_Dict_Dict* _temp384; struct Cyc_Dict_Dict* _temp386;
_LL366: if(*(( int*) _temp364) == Cyc_CfFlowInfo_LeafPI){ _LL375: _temp374=(
void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp364)->f1; if(*(( int*)
_temp374) == Cyc_CfFlowInfo_MustPointTo){ _LL377: _temp376=( struct Cyc_CfFlowInfo_Place*)((
struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp374)->f1; goto _LL367;} else{
goto _LL368;}} else{ goto _LL368;} _LL368: if(*(( int*) _temp364) == Cyc_CfFlowInfo_LeafPI){
_LL379: _temp378=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp364)->f1;
if(*(( int*) _temp378) == Cyc_CfFlowInfo_UnknownIS){ _LL383: _temp382=( void*)((
struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp378)->f1; goto _LL381; _LL381:
_temp380=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp378)->f2; goto
_LL369;} else{ goto _LL370;}} else{ goto _LL370;} _LL370: if(*(( int*) _temp364)
== Cyc_CfFlowInfo_TuplePI){ _LL385: _temp384=( struct Cyc_Dict_Dict*)(( struct
Cyc_CfFlowInfo_TuplePI_struct*) _temp364)->f1; goto _LL371;} else{ goto _LL372;}
_LL372: if(*(( int*) _temp364) == Cyc_CfFlowInfo_StructPI){ _LL387: _temp386=(
struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp364)->f1;
goto _LL373;} else{ goto _LL365;} _LL367:* _temp360=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place* elt)) Cyc_Set_insert)(*
_temp360, _temp376); if( Cyc_CfFlowInfo_isAllInit( _temp362, Cyc_CfFlowInfo_lookup_place(
_temp362, _temp376))){ return Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Esc,(
void*) Cyc_CfFlowInfo_AllIL);} return Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Esc,(
void*) Cyc_CfFlowInfo_ThisIL); _LL369: return Cyc_CfFlowInfo_mkLeafPI(( void*)
Cyc_CfFlowInfo_Esc, _temp380); _LL371: return( void*)({ struct Cyc_CfFlowInfo_TuplePI_struct*
_temp388=( struct Cyc_CfFlowInfo_TuplePI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct));
_temp388[ 0]=({ struct Cyc_CfFlowInfo_TuplePI_struct _temp389; _temp389.tag= Cyc_CfFlowInfo_TuplePI;
_temp389.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)( struct _tuple7*, void*),
struct _tuple7* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_map_c)( Cyc_CfFlowInfo_pinfo_getplaces_esc,
env, _temp384); _temp389;}); _temp388;}); _LL373: return( void*)({ struct Cyc_CfFlowInfo_StructPI_struct*
_temp390=( struct Cyc_CfFlowInfo_StructPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct));
_temp390[ 0]=({ struct Cyc_CfFlowInfo_StructPI_struct _temp391; _temp391.tag=
Cyc_CfFlowInfo_StructPI; _temp391.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)(
struct _tuple7*, void*), struct _tuple7* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_map_c)(
Cyc_CfFlowInfo_pinfo_getplaces_esc, env, _temp386); _temp391;}); _temp390;});
_LL365:;}} void Cyc_CfFlowInfo_pinfo_getplaces( struct Cyc_Set_Set** places_ptr,
void* a, void* pinfo){ void* _temp392= pinfo; void* _temp402; void* _temp404;
void* _temp406; void* _temp408; struct Cyc_CfFlowInfo_Place* _temp410; struct
Cyc_Dict_Dict* _temp412; struct Cyc_Dict_Dict* _temp414; _LL394: if(*(( int*)
_temp392) == Cyc_CfFlowInfo_LeafPI){ _LL403: _temp402=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*)
_temp392)->f1; if(*(( int*) _temp402) == Cyc_CfFlowInfo_UnknownIS){ _LL407:
_temp406=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp402)->f1; goto
_LL405; _LL405: _temp404=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp402)->f2; goto _LL395;} else{ goto _LL396;}} else{ goto _LL396;} _LL396:
if(*(( int*) _temp392) == Cyc_CfFlowInfo_LeafPI){ _LL409: _temp408=( void*)((
struct Cyc_CfFlowInfo_LeafPI_struct*) _temp392)->f1; if(*(( int*) _temp408) ==
Cyc_CfFlowInfo_MustPointTo){ _LL411: _temp410=( struct Cyc_CfFlowInfo_Place*)((
struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp408)->f1; goto _LL397;} else{
goto _LL398;}} else{ goto _LL398;} _LL398: if(*(( int*) _temp392) == Cyc_CfFlowInfo_TuplePI){
_LL413: _temp412=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_TuplePI_struct*)
_temp392)->f1; goto _LL399;} else{ goto _LL400;} _LL400: if(*(( int*) _temp392)
== Cyc_CfFlowInfo_StructPI){ _LL415: _temp414=( struct Cyc_Dict_Dict*)(( struct
Cyc_CfFlowInfo_StructPI_struct*) _temp392)->f1; goto _LL401;} else{ goto _LL393;}
_LL395: return; _LL397:* places_ptr=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set*
s, struct Cyc_CfFlowInfo_Place* elt)) Cyc_Set_insert)(* places_ptr, _temp410);
return; _LL399:(( void(*)( void(* f)( struct Cyc_Set_Set**, int, void*), struct
Cyc_Set_Set** env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(( void(*)( struct
Cyc_Set_Set** places_ptr, int a, void* pinfo)) Cyc_CfFlowInfo_pinfo_getplaces,
places_ptr, _temp412); return; _LL401:(( void(*)( void(* f)( struct Cyc_Set_Set**,
struct _tagged_string*, void*), struct Cyc_Set_Set** env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_iter_c)(( void(*)( struct Cyc_Set_Set** places_ptr, struct
_tagged_string* a, void* pinfo)) Cyc_CfFlowInfo_pinfo_getplaces, places_ptr,
_temp414); return; _LL393:;} struct Cyc_Dict_Dict* Cyc_CfFlowInfo_escape_these(
struct Cyc_Set_Set* places, struct Cyc_Dict_Dict* d){ while( !(( int(*)( struct
Cyc_Set_Set* s)) Cyc_Set_is_empty)( places)) { struct Cyc_CfFlowInfo_Place*
_temp416=(( struct Cyc_CfFlowInfo_Place*(*)( struct Cyc_Set_Set* s)) Cyc_Set_choose)(
places); goto _LL417; _LL417: places=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set*
s, struct Cyc_CfFlowInfo_Place* elt)) Cyc_Set_delete)( places, _temp416);{ void*
place_old_pinfo;{ struct _handler_cons _temp418; _push_handler(& _temp418);{ int
_temp420= 0; if( setjmp( _temp418.handler)){ _temp420= 1;} if( ! _temp420){
place_old_pinfo= Cyc_CfFlowInfo_lookup_place( d, _temp416);; _pop_handler();}
else{ void* _temp419=( void*) _exn_thrown; void* _temp422= _temp419; _LL424: if(
_temp422 == Cyc_Dict_Absent){ goto _LL425;} else{ goto _LL426;} _LL426: goto
_LL427; _LL425: continue; _LL427:( void) _throw( _temp422); _LL423:;}}}{ struct
_tuple7 _temp429=({ struct _tuple7 _temp428; _temp428.f1= d; _temp428.f2=&
places; _temp428;}); goto _LL430; _LL430: { void* _temp431= Cyc_CfFlowInfo_pinfo_getplaces_esc(&
_temp429, place_old_pinfo); goto _LL432; _LL432: if( place_old_pinfo != _temp431){
d= Cyc_CfFlowInfo_insert_place( d, _temp416, _temp431);}}}}} return d;} static
struct Cyc_Set_Set** Cyc_CfFlowInfo_empty_place_set_ptr= 0; struct Cyc_Dict_Dict*
Cyc_CfFlowInfo_escape_pointsto( void* pinfo, struct Cyc_Dict_Dict* d){ if( Cyc_CfFlowInfo_empty_place_set_ptr
== 0){ Cyc_CfFlowInfo_empty_place_set_ptr=({ struct Cyc_Set_Set** _temp433=(
struct Cyc_Set_Set**) GC_malloc( sizeof( struct Cyc_Set_Set*) * 1); _temp433[ 0]=((
struct Cyc_Set_Set*(*)( int(* comp)( struct Cyc_CfFlowInfo_Place*, struct Cyc_CfFlowInfo_Place*)))
Cyc_Set_empty)( Cyc_CfFlowInfo_place_cmp); _temp433;});}{ struct Cyc_Set_Set*
_temp435=({ struct Cyc_Set_Set** _temp434= Cyc_CfFlowInfo_empty_place_set_ptr;
if( _temp434 == 0){ _throw( Null_Exception);}* _temp434;}); goto _LL436; _LL436:((
void(*)( struct Cyc_Set_Set** places_ptr, int a, void* pinfo)) Cyc_CfFlowInfo_pinfo_getplaces)(&
_temp435, 0, pinfo); return Cyc_CfFlowInfo_escape_these( _temp435, d);}} void*
Cyc_CfFlowInfo_assign_unknown_dict( void* level, void* pinfo){ void* _temp437=
pinfo; void* _temp447; void* _temp449; void* _temp451; void* _temp453; struct
Cyc_Dict_Dict* _temp455; struct Cyc_Dict_Dict* _temp457; _LL439: if(*(( int*)
_temp437) == Cyc_CfFlowInfo_LeafPI){ _LL448: _temp447=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*)
_temp437)->f1; if(*(( int*) _temp447) == Cyc_CfFlowInfo_UnknownIS){ _LL452:
_temp451=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp447)->f1; if(
_temp451 ==( void*) Cyc_CfFlowInfo_Esc){ goto _LL450;} else{ goto _LL441;}
_LL450: _temp449=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp447)->f2;
goto _LL440;} else{ goto _LL441;}} else{ goto _LL441;} _LL441: if(*(( int*)
_temp437) == Cyc_CfFlowInfo_LeafPI){ _LL454: _temp453=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*)
_temp437)->f1; goto _LL442;} else{ goto _LL443;} _LL443: if(*(( int*) _temp437)
== Cyc_CfFlowInfo_TuplePI){ _LL456: _temp455=( struct Cyc_Dict_Dict*)(( struct
Cyc_CfFlowInfo_TuplePI_struct*) _temp437)->f1; goto _LL444;} else{ goto _LL445;}
_LL445: if(*(( int*) _temp437) == Cyc_CfFlowInfo_StructPI){ _LL458: _temp457=(
struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp437)->f1;
goto _LL446;} else{ goto _LL438;} _LL440: return Cyc_CfFlowInfo_mkLeafPI(( void*)
Cyc_CfFlowInfo_Esc, level); _LL442: return Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Unesc,
level); _LL444: return( void*)({ struct Cyc_CfFlowInfo_TuplePI_struct* _temp459=(
struct Cyc_CfFlowInfo_TuplePI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct));
_temp459[ 0]=({ struct Cyc_CfFlowInfo_TuplePI_struct _temp460; _temp460.tag= Cyc_CfFlowInfo_TuplePI;
_temp460.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)( void*, void*), void* env,
struct Cyc_Dict_Dict* d)) Cyc_Dict_map_c)( Cyc_CfFlowInfo_assign_unknown_dict,
level, _temp455); _temp460;}); _temp459;}); _LL446: return( void*)({ struct Cyc_CfFlowInfo_StructPI_struct*
_temp461=( struct Cyc_CfFlowInfo_StructPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct));
_temp461[ 0]=({ struct Cyc_CfFlowInfo_StructPI_struct _temp462; _temp462.tag=
Cyc_CfFlowInfo_StructPI; _temp462.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)(
void*, void*), void* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_map_c)( Cyc_CfFlowInfo_assign_unknown_dict,
level, _temp457); _temp462;}); _temp461;}); _LL438:;} struct Cyc_CfFlowInfo_JoinEnv{
struct Cyc_Dict_Dict* d1; struct Cyc_Dict_Dict* d2; struct Cyc_Set_Set** places;
} ; typedef struct Cyc_CfFlowInfo_JoinEnv* Cyc_CfFlowInfo_join_env_t; void* Cyc_CfFlowInfo_join_escape(
void* esc1, void* esc2){ struct _tuple3 _temp464=({ struct _tuple3 _temp463;
_temp463.f1= esc1; _temp463.f2= esc2; _temp463;}); void* _temp472; void*
_temp474; void* _temp476; void* _temp478; _LL466: _LL475: _temp474= _temp464.f1;
if( _temp474 ==( void*) Cyc_CfFlowInfo_Esc){ goto _LL473;} else{ goto _LL468;}
_LL473: _temp472= _temp464.f2; goto _LL467; _LL468: _LL479: _temp478= _temp464.f1;
goto _LL477; _LL477: _temp476= _temp464.f2; if( _temp476 ==( void*) Cyc_CfFlowInfo_Esc){
goto _LL469;} else{ goto _LL470;} _LL470: goto _LL471; _LL467: goto _LL469;
_LL469: return( void*) Cyc_CfFlowInfo_Esc; _LL471: return( void*) Cyc_CfFlowInfo_Unesc;
_LL465:;} void* Cyc_CfFlowInfo_join_initlevel( void* il1, void* il2){ struct
_tuple3 _temp481=({ struct _tuple3 _temp480; _temp480.f1= il1; _temp480.f2= il2;
_temp480;}); void* _temp493; void* _temp495; void* _temp497; void* _temp499;
void* _temp501; void* _temp503; void* _temp505; void* _temp507; _LL483: _LL496:
_temp495= _temp481.f1; if( _temp495 ==( void*) Cyc_CfFlowInfo_NoneIL){ goto
_LL494;} else{ goto _LL485;} _LL494: _temp493= _temp481.f2; goto _LL484; _LL485:
_LL500: _temp499= _temp481.f1; goto _LL498; _LL498: _temp497= _temp481.f2; if(
_temp497 ==( void*) Cyc_CfFlowInfo_NoneIL){ goto _LL486;} else{ goto _LL487;}
_LL487: _LL504: _temp503= _temp481.f1; if( _temp503 ==( void*) Cyc_CfFlowInfo_ThisIL){
goto _LL502;} else{ goto _LL489;} _LL502: _temp501= _temp481.f2; goto _LL488;
_LL489: _LL508: _temp507= _temp481.f1; goto _LL506; _LL506: _temp505= _temp481.f2;
if( _temp505 ==( void*) Cyc_CfFlowInfo_ThisIL){ goto _LL490;} else{ goto _LL491;}
_LL491: goto _LL492; _LL484: goto _LL486; _LL486: return( void*) Cyc_CfFlowInfo_NoneIL;
_LL488: goto _LL490; _LL490: return( void*) Cyc_CfFlowInfo_ThisIL; _LL492:
return( void*) Cyc_CfFlowInfo_AllIL; _LL482:;} void* Cyc_CfFlowInfo_join_initstate(
struct Cyc_CfFlowInfo_JoinEnv* env, void* state1, void* state2){ struct _tuple3
_temp510=({ struct _tuple3 _temp509; _temp509.f1= state1; _temp509.f2= state2;
_temp509;}); void* _temp520; void* _temp522; void* _temp524; void* _temp526;
void* _temp528; void* _temp530; void* _temp532; struct Cyc_CfFlowInfo_Place*
_temp534; void* _temp536; void* _temp538; void* _temp540; void* _temp542; void*
_temp544; void* _temp546; void* _temp548; struct Cyc_CfFlowInfo_Place* _temp550;
void* _temp552; struct Cyc_CfFlowInfo_Place* _temp554; void* _temp556; struct
Cyc_CfFlowInfo_Place* _temp558; _LL512: _LL527: _temp526= _temp510.f1; if(*((
int*) _temp526) == Cyc_CfFlowInfo_UnknownIS){ _LL531: _temp530=( void*)(( struct
Cyc_CfFlowInfo_UnknownIS_struct*) _temp526)->f1; goto _LL529; _LL529: _temp528=(
void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp526)->f2; goto _LL521;}
else{ goto _LL514;} _LL521: _temp520= _temp510.f2; if(*(( int*) _temp520) == Cyc_CfFlowInfo_UnknownIS){
_LL525: _temp524=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp520)->f1;
goto _LL523; _LL523: _temp522=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp520)->f2; goto _LL513;} else{ goto _LL514;} _LL514: _LL537: _temp536=
_temp510.f1; if(*(( int*) _temp536) == Cyc_CfFlowInfo_UnknownIS){ _LL541:
_temp540=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp536)->f1; goto
_LL539; _LL539: _temp538=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp536)->f2; goto _LL533;} else{ goto _LL516;} _LL533: _temp532= _temp510.f2;
if(*(( int*) _temp532) == Cyc_CfFlowInfo_MustPointTo){ _LL535: _temp534=( struct
Cyc_CfFlowInfo_Place*)(( struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp532)->f1;
goto _LL515;} else{ goto _LL516;} _LL516: _LL549: _temp548= _temp510.f1; if(*((
int*) _temp548) == Cyc_CfFlowInfo_MustPointTo){ _LL551: _temp550=( struct Cyc_CfFlowInfo_Place*)((
struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp548)->f1; goto _LL543;} else{
goto _LL518;} _LL543: _temp542= _temp510.f2; if(*(( int*) _temp542) == Cyc_CfFlowInfo_UnknownIS){
_LL547: _temp546=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp542)->f1;
goto _LL545; _LL545: _temp544=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp542)->f2; goto _LL517;} else{ goto _LL518;} _LL518: _LL557: _temp556=
_temp510.f1; if(*(( int*) _temp556) == Cyc_CfFlowInfo_MustPointTo){ _LL559:
_temp558=( struct Cyc_CfFlowInfo_Place*)(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp556)->f1; goto _LL553;} else{ goto _LL511;} _LL553: _temp552= _temp510.f2;
if(*(( int*) _temp552) == Cyc_CfFlowInfo_MustPointTo){ _LL555: _temp554=( struct
Cyc_CfFlowInfo_Place*)(( struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp552)->f1;
goto _LL519;} else{ goto _LL511;} _LL513: return Cyc_CfFlowInfo_mkLeafPI( Cyc_CfFlowInfo_join_escape(
_temp530, _temp524), Cyc_CfFlowInfo_join_initlevel( _temp528, _temp522)); _LL515: {
struct _tuple6 _temp561=({ struct _tuple6 _temp560; _temp560.f1= env->d2;
_temp560.f2= 0; _temp560;}); goto _LL562; _LL562: if( _temp538 ==( void*) Cyc_CfFlowInfo_AllIL?
Cyc_CfFlowInfo_isAllInit_pinfo(& _temp561, Cyc_CfFlowInfo_lookup_place( env->d2,
_temp534)): 0){* env->places=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s,
struct Cyc_CfFlowInfo_Place* elt)) Cyc_Set_insert)(* env->places, _temp534);
return Cyc_CfFlowInfo_mkLeafPI( _temp540, _temp538);} return Cyc_CfFlowInfo_mkLeafPI(
_temp540, Cyc_CfFlowInfo_join_initlevel( _temp538,( void*) Cyc_CfFlowInfo_ThisIL));}
_LL517: { struct _tuple6 _temp564=({ struct _tuple6 _temp563; _temp563.f1= env->d1;
_temp563.f2= 0; _temp563;}); goto _LL565; _LL565: if( _temp544 ==( void*) Cyc_CfFlowInfo_AllIL?
Cyc_CfFlowInfo_isAllInit_pinfo(& _temp564, Cyc_CfFlowInfo_lookup_place( env->d1,
_temp550)): 0){* env->places=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s,
struct Cyc_CfFlowInfo_Place* elt)) Cyc_Set_insert)(* env->places, _temp550);
return Cyc_CfFlowInfo_mkLeafPI( _temp546, _temp544);} return Cyc_CfFlowInfo_mkLeafPI(
_temp546, Cyc_CfFlowInfo_join_initlevel(( void*) Cyc_CfFlowInfo_ThisIL, _temp544));}
_LL519: if( Cyc_CfFlowInfo_place_cmp( _temp558, _temp554) == 0){ return( void*)({
struct Cyc_CfFlowInfo_LeafPI_struct* _temp566=( struct Cyc_CfFlowInfo_LeafPI_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_LeafPI_struct)); _temp566[ 0]=({ struct
Cyc_CfFlowInfo_LeafPI_struct _temp567; _temp567.tag= Cyc_CfFlowInfo_LeafPI;
_temp567.f1=( void*) state1; _temp567;}); _temp566;});}{ struct _tuple6 _temp569=({
struct _tuple6 _temp568; _temp568.f1= env->d1; _temp568.f2= 0; _temp568;}); goto
_LL570; _LL570: { struct _tuple6 _temp572=({ struct _tuple6 _temp571; _temp571.f1=
env->d2; _temp571.f2= 0; _temp571;}); goto _LL573; _LL573: if( Cyc_CfFlowInfo_isAllInit_pinfo(&
_temp569, Cyc_CfFlowInfo_lookup_place( env->d1, _temp558))? Cyc_CfFlowInfo_isAllInit_pinfo(&
_temp572, Cyc_CfFlowInfo_lookup_place( env->d2, _temp554)): 0){* env->places=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place* elt))
Cyc_Set_insert)(* env->places, _temp558);* env->places=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place* elt)) Cyc_Set_insert)(* env->places,
_temp554); return Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Unesc,( void*)
Cyc_CfFlowInfo_AllIL);} return Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Unesc,(
void*) Cyc_CfFlowInfo_ThisIL);}} _LL511:;} void* Cyc_CfFlowInfo_join_pathinfo(
struct Cyc_CfFlowInfo_JoinEnv* env, void* pinfo1, void* pinfo2){ if( pinfo1 ==
pinfo2){ return pinfo1;}{ struct _tuple3 _temp575=({ struct _tuple3 _temp574;
_temp574.f1= pinfo1; _temp574.f2= pinfo2; _temp574;}); void* _temp585; void*
_temp587; void* _temp589; void* _temp591; void* _temp593; struct Cyc_Dict_Dict*
_temp595; void* _temp597; struct Cyc_Dict_Dict* _temp599; void* _temp601; struct
Cyc_Dict_Dict* _temp603; void* _temp605; struct Cyc_Dict_Dict* _temp607; _LL577:
_LL590: _temp589= _temp575.f1; if(*(( int*) _temp589) == Cyc_CfFlowInfo_LeafPI){
_LL592: _temp591=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp589)->f1;
goto _LL586;} else{ goto _LL579;} _LL586: _temp585= _temp575.f2; if(*(( int*)
_temp585) == Cyc_CfFlowInfo_LeafPI){ _LL588: _temp587=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*)
_temp585)->f1; goto _LL578;} else{ goto _LL579;} _LL579: _LL598: _temp597=
_temp575.f1; if(*(( int*) _temp597) == Cyc_CfFlowInfo_TuplePI){ _LL600: _temp599=(
struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_TuplePI_struct*) _temp597)->f1;
goto _LL594;} else{ goto _LL581;} _LL594: _temp593= _temp575.f2; if(*(( int*)
_temp593) == Cyc_CfFlowInfo_TuplePI){ _LL596: _temp595=( struct Cyc_Dict_Dict*)((
struct Cyc_CfFlowInfo_TuplePI_struct*) _temp593)->f1; goto _LL580;} else{ goto
_LL581;} _LL581: _LL606: _temp605= _temp575.f1; if(*(( int*) _temp605) == Cyc_CfFlowInfo_StructPI){
_LL608: _temp607=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp605)->f1; goto _LL602;} else{ goto _LL583;} _LL602: _temp601= _temp575.f2;
if(*(( int*) _temp601) == Cyc_CfFlowInfo_StructPI){ _LL604: _temp603=( struct
Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp601)->f1; goto
_LL582;} else{ goto _LL583;} _LL583: goto _LL584; _LL578: if( _temp591 ==
_temp587){ return pinfo1;} return Cyc_CfFlowInfo_join_initstate( env, _temp591,
_temp587); _LL580: if( _temp599 == _temp595){ return pinfo1;} return( void*)({
struct Cyc_CfFlowInfo_TuplePI_struct* _temp609=( struct Cyc_CfFlowInfo_TuplePI_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct)); _temp609[ 0]=({
struct Cyc_CfFlowInfo_TuplePI_struct _temp610; _temp610.tag= Cyc_CfFlowInfo_TuplePI;
_temp610.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)( struct Cyc_CfFlowInfo_JoinEnv*,
void*, void*), struct Cyc_CfFlowInfo_JoinEnv* env, struct Cyc_Dict_Dict* d1,
struct Cyc_Dict_Dict* d2)) Cyc_Dict_intersect_c)( Cyc_CfFlowInfo_join_pathinfo,
env, _temp599, _temp595); _temp610;}); _temp609;}); _LL582: if( _temp607 ==
_temp603){ return pinfo1;} return( void*)({ struct Cyc_CfFlowInfo_StructPI_struct*
_temp611=( struct Cyc_CfFlowInfo_StructPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructPI_struct));
_temp611[ 0]=({ struct Cyc_CfFlowInfo_StructPI_struct _temp612; _temp612.tag=
Cyc_CfFlowInfo_StructPI; _temp612.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)(
struct Cyc_CfFlowInfo_JoinEnv*, void*, void*), struct Cyc_CfFlowInfo_JoinEnv*
env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_intersect_c)(
Cyc_CfFlowInfo_join_pathinfo, env, _temp607, _temp603); _temp612;}); _temp611;});
_LL584:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp613=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp613[ 0]=({ struct Cyc_Core_Impossible_struct _temp614; _temp614.tag= Cyc_Core_Impossible;
_temp614.f1=( struct _tagged_string)({ char* _temp615=( char*)"join_pathinfo";
struct _tagged_string _temp616; _temp616.curr= _temp615; _temp616.base= _temp615;
_temp616.last_plus_one= _temp615 + 14; _temp616;}); _temp614;}); _temp613;}));
_LL576:;}} void* Cyc_CfFlowInfo_join_flow( void* f1, void* f2){ if( Cyc_CfFlowInfo_empty_place_set_ptr
== 0){ Cyc_CfFlowInfo_empty_place_set_ptr=({ struct Cyc_Set_Set** _temp617=(
struct Cyc_Set_Set**) GC_malloc( sizeof( struct Cyc_Set_Set*) * 1); _temp617[ 0]=((
struct Cyc_Set_Set*(*)( int(* comp)( struct Cyc_CfFlowInfo_Place*, struct Cyc_CfFlowInfo_Place*)))
Cyc_Set_empty)( Cyc_CfFlowInfo_place_cmp); _temp617;});} if( f1 == f2){ return
f1;}{ struct _tuple3 _temp619=({ struct _tuple3 _temp618; _temp618.f1= f1;
_temp618.f2= f2; _temp618;}); void* _temp627; void* _temp629; void* _temp631;
void* _temp633; void* _temp635; struct Cyc_Dict_Dict* _temp637; void* _temp639;
struct Cyc_Dict_Dict* _temp641; _LL621: _LL630: _temp629= _temp619.f1; if(
_temp629 ==( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL628;} else{ goto _LL623;}
_LL628: _temp627= _temp619.f2; goto _LL622; _LL623: _LL634: _temp633= _temp619.f1;
goto _LL632; _LL632: _temp631= _temp619.f2; if( _temp631 ==( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL624;} else{ goto _LL625;} _LL625: _LL640: _temp639= _temp619.f1; if((
unsigned int) _temp639 > 1u?*(( int*) _temp639) == Cyc_CfFlowInfo_InitsFL: 0){
_LL642: _temp641=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp639)->f1; goto _LL636;} else{ goto _LL620;} _LL636: _temp635= _temp619.f2;
if(( unsigned int) _temp635 > 1u?*(( int*) _temp635) == Cyc_CfFlowInfo_InitsFL:
0){ _LL638: _temp637=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp635)->f1; goto _LL626;} else{ goto _LL620;} _LL622: return f2; _LL624:
return f1; _LL626: if( _temp641 == _temp637){ return f1;}{ struct Cyc_Set_Set*
_temp644=({ struct Cyc_Set_Set** _temp643= Cyc_CfFlowInfo_empty_place_set_ptr;
if( _temp643 == 0){ _throw( Null_Exception);}* _temp643;}); goto _LL645; _LL645: {
struct Cyc_CfFlowInfo_JoinEnv _temp647=({ struct Cyc_CfFlowInfo_JoinEnv _temp646;
_temp646.d1= _temp641; _temp646.d2= _temp637; _temp646.places=& _temp644;
_temp646;}); goto _LL648; _LL648: { struct Cyc_Dict_Dict* _temp649=(( struct Cyc_Dict_Dict*(*)(
void*(* f)( struct Cyc_CfFlowInfo_JoinEnv*, void*, void*), struct Cyc_CfFlowInfo_JoinEnv*
env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_intersect_c)(
Cyc_CfFlowInfo_join_pathinfo,& _temp647, _temp641, _temp637); goto _LL650;
_LL650: return( void*)({ struct Cyc_CfFlowInfo_InitsFL_struct* _temp651=( struct
Cyc_CfFlowInfo_InitsFL_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct));
_temp651[ 0]=({ struct Cyc_CfFlowInfo_InitsFL_struct _temp652; _temp652.tag= Cyc_CfFlowInfo_InitsFL;
_temp652.f1= Cyc_CfFlowInfo_escape_these( _temp644, _temp649); _temp652;});
_temp651;});}}} _LL620:;}} int Cyc_CfFlowInfo_initstate_lessthan_approx( void*
state1, void* state2){ struct _tuple3 _temp654=({ struct _tuple3 _temp653;
_temp653.f1= state1; _temp653.f2= state2; _temp653;}); void* _temp662; void*
_temp664; void* _temp666; void* _temp668; void* _temp670; void* _temp672; void*
_temp674; struct Cyc_CfFlowInfo_Place* _temp676; void* _temp678; struct Cyc_CfFlowInfo_Place*
_temp680; _LL656: _LL669: _temp668= _temp654.f1; if(*(( int*) _temp668) == Cyc_CfFlowInfo_UnknownIS){
_LL673: _temp672=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp668)->f1;
goto _LL671; _LL671: _temp670=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp668)->f2; goto _LL663;} else{ goto _LL658;} _LL663: _temp662= _temp654.f2;
if(*(( int*) _temp662) == Cyc_CfFlowInfo_UnknownIS){ _LL667: _temp666=( void*)((
struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp662)->f1; goto _LL665; _LL665:
_temp664=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp662)->f2; goto
_LL657;} else{ goto _LL658;} _LL658: _LL679: _temp678= _temp654.f1; if(*(( int*)
_temp678) == Cyc_CfFlowInfo_MustPointTo){ _LL681: _temp680=( struct Cyc_CfFlowInfo_Place*)((
struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp678)->f1; goto _LL675;} else{
goto _LL660;} _LL675: _temp674= _temp654.f2; if(*(( int*) _temp674) == Cyc_CfFlowInfo_MustPointTo){
_LL677: _temp676=( struct Cyc_CfFlowInfo_Place*)(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp674)->f1; goto _LL659;} else{ goto _LL660;} _LL660: goto _LL661; _LL657:
return Cyc_CfFlowInfo_join_escape( _temp672, _temp666) == _temp666? Cyc_CfFlowInfo_join_initlevel(
_temp670, _temp664) == _temp664: 0; _LL659: return Cyc_CfFlowInfo_place_cmp(
_temp680, _temp676) == 0; _LL661: return 0; _LL655:;} int Cyc_CfFlowInfo_pinfo_lessthan_approx(
void* ignore, void* pinfo1, void* pinfo2){ if( pinfo1 == pinfo2){ return 1;}{
struct _tuple3 _temp683=({ struct _tuple3 _temp682; _temp682.f1= pinfo1;
_temp682.f2= pinfo2; _temp682;}); void* _temp693; void* _temp695; void* _temp697;
void* _temp699; void* _temp701; struct Cyc_Dict_Dict* _temp703; void* _temp705;
struct Cyc_Dict_Dict* _temp707; void* _temp709; struct Cyc_Dict_Dict* _temp711;
void* _temp713; struct Cyc_Dict_Dict* _temp715; _LL685: _LL698: _temp697=
_temp683.f1; if(*(( int*) _temp697) == Cyc_CfFlowInfo_LeafPI){ _LL700: _temp699=(
void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp697)->f1; goto _LL694;}
else{ goto _LL687;} _LL694: _temp693= _temp683.f2; if(*(( int*) _temp693) == Cyc_CfFlowInfo_LeafPI){
_LL696: _temp695=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp693)->f1;
goto _LL686;} else{ goto _LL687;} _LL687: _LL706: _temp705= _temp683.f1; if(*((
int*) _temp705) == Cyc_CfFlowInfo_TuplePI){ _LL708: _temp707=( struct Cyc_Dict_Dict*)((
struct Cyc_CfFlowInfo_TuplePI_struct*) _temp705)->f1; goto _LL702;} else{ goto
_LL689;} _LL702: _temp701= _temp683.f2; if(*(( int*) _temp701) == Cyc_CfFlowInfo_TuplePI){
_LL704: _temp703=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_TuplePI_struct*)
_temp701)->f1; goto _LL688;} else{ goto _LL689;} _LL689: _LL714: _temp713=
_temp683.f1; if(*(( int*) _temp713) == Cyc_CfFlowInfo_StructPI){ _LL716:
_temp715=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp713)->f1; goto _LL710;} else{ goto _LL691;} _LL710: _temp709= _temp683.f2;
if(*(( int*) _temp709) == Cyc_CfFlowInfo_StructPI){ _LL712: _temp711=( struct
Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp709)->f1; goto
_LL690;} else{ goto _LL691;} _LL691: goto _LL692; _LL686: return _temp699 ==
_temp695? 1: Cyc_CfFlowInfo_initstate_lessthan_approx( _temp699, _temp695);
_LL688: return _temp707 == _temp703? 1:(( int(*)( int(* f)( int, void*, void*),
struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_forall_intersect)((
int(*)( int ignore, void* pinfo1, void* pinfo2)) Cyc_CfFlowInfo_pinfo_lessthan_approx,
_temp707, _temp703); _LL690: return _temp715 == _temp711? 1:(( int(*)( int(* f)(
struct _tagged_string*, void*, void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict*
d2)) Cyc_Dict_forall_intersect)(( int(*)( struct _tagged_string* ignore, void*
pinfo1, void* pinfo2)) Cyc_CfFlowInfo_pinfo_lessthan_approx, _temp715, _temp711);
_LL692:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp717=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp717[ 0]=({ struct Cyc_Core_Impossible_struct _temp718; _temp718.tag= Cyc_Core_Impossible;
_temp718.f1=( struct _tagged_string)({ char* _temp719=( char*)"pinfo_lessthan";
struct _tagged_string _temp720; _temp720.curr= _temp719; _temp720.base= _temp719;
_temp720.last_plus_one= _temp719 + 15; _temp720;}); _temp718;}); _temp717;}));
_LL684:;}} int Cyc_CfFlowInfo_flow_lessthan_approx( void* f1, void* f2){ if( f1
== f2){ return 1;}{ struct _tuple3 _temp722=({ struct _tuple3 _temp721; _temp721.f1=
f1; _temp721.f2= f2; _temp721;}); void* _temp730; void* _temp732; void* _temp734;
void* _temp736; void* _temp738; struct Cyc_Dict_Dict* _temp740; void* _temp742;
struct Cyc_Dict_Dict* _temp744; _LL724: _LL733: _temp732= _temp722.f1; if(
_temp732 ==( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL731;} else{ goto _LL726;}
_LL731: _temp730= _temp722.f2; goto _LL725; _LL726: _LL737: _temp736= _temp722.f1;
goto _LL735; _LL735: _temp734= _temp722.f2; if( _temp734 ==( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL727;} else{ goto _LL728;} _LL728: _LL743: _temp742= _temp722.f1; if((
unsigned int) _temp742 > 1u?*(( int*) _temp742) == Cyc_CfFlowInfo_InitsFL: 0){
_LL745: _temp744=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp742)->f1; goto _LL739;} else{ goto _LL723;} _LL739: _temp738= _temp722.f2;
if(( unsigned int) _temp738 > 1u?*(( int*) _temp738) == Cyc_CfFlowInfo_InitsFL:
0){ _LL741: _temp740=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp738)->f1; goto _LL729;} else{ goto _LL723;} _LL725: return 1; _LL727:
return 0; _LL729: if( _temp744 == _temp740){ return 1;} return(( int(*)( int(* f)(
void*, void*, void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_forall_intersect)((
int(*)( void* ignore, void* pinfo1, void* pinfo2)) Cyc_CfFlowInfo_pinfo_lessthan_approx,
_temp744, _temp740); _LL723:;}}