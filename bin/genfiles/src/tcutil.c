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
extern char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{ char*
tag; struct _tagged_string f1; } ; extern char Cyc_Core_Failure[ 12u]; struct
Cyc_Core_Failure_struct{ char* tag; struct _tagged_string f1; } ; extern char
Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{ char* tag; struct
_tagged_string f1; } ; extern char Cyc_Core_Not_found[ 14u]; extern char Cyc_Core_Unreachable[
16u]; struct Cyc_Core_Unreachable_struct{ char* tag; struct _tagged_string f1; }
; extern struct _RegionHandle* Cyc_Core_heap_region; extern char*
string_to_Cstring( struct _tagged_string); extern char* underlying_Cstring(
struct _tagged_string); extern struct _tagged_string Cstring_to_string( char*);
extern int system( char*); struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE
Cyc_Stdio_FILE; extern struct Cyc_Stdio___sFILE* Cyc_Stdio_stderr; typedef int
Cyc_Stdio_fpos_t; extern int Cyc_Stdio_fflush( struct Cyc_Stdio___sFILE*);
extern char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Stdio_FileCloseError[
19u]; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; typedef
struct Cyc_List_List* Cyc_List_glist_t; typedef struct Cyc_List_List* Cyc_List_list_t;
typedef struct Cyc_List_List* Cyc_List_List_t; extern int Cyc_List_length(
struct Cyc_List_List* x); extern char Cyc_List_List_empty[ 15u]; extern struct
Cyc_List_List* Cyc_List_map( void*(* f)( void*), struct Cyc_List_List* x);
extern struct Cyc_List_List* Cyc_List_rmap( struct _RegionHandle*, void*(* f)(
void*), struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_map_c(
void*(* f)( void*, void*), void* env, struct Cyc_List_List* x); extern struct
Cyc_List_List* Cyc_List_rmap_c( struct _RegionHandle*, void*(* f)( void*, void*),
void* env, struct Cyc_List_List* x); extern char Cyc_List_List_mismatch[ 18u];
extern struct Cyc_List_List* Cyc_List_map2( void*(* f)( void*, void*), struct
Cyc_List_List* x, struct Cyc_List_List* y); extern void Cyc_List_iter2( void(* f)(
void*, void*), struct Cyc_List_List* x, struct Cyc_List_List* y); extern struct
Cyc_List_List* Cyc_List_revappend( struct Cyc_List_List* x, struct Cyc_List_List*
y); extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x);
extern struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x, struct
Cyc_List_List* y); extern char Cyc_List_Nth[ 8u]; extern int Cyc_List_exists_c(
int(* pred)( void*, void*), void* env, struct Cyc_List_List* x); extern struct
Cyc_List_List* Cyc_List_zip( struct Cyc_List_List* x, struct Cyc_List_List* y);
extern struct Cyc_List_List* Cyc_List_rzip( struct _RegionHandle* r3, struct
_RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y); struct
_tuple0{ struct Cyc_List_List* f1; struct Cyc_List_List* f2; } ; extern struct
_tuple0 Cyc_List_rsplit( struct _RegionHandle* r3, struct _RegionHandle* r4,
struct Cyc_List_List* x); extern int Cyc_List_mem( int(* compare)( void*, void*),
struct Cyc_List_List* l, void* x); extern void* Cyc_List_assoc_cmp( int(*
compare)( void*, void*), struct Cyc_List_List* l, void* x); struct Cyc_Lineno_Pos{
struct _tagged_string logical_file; struct _tagged_string line; int line_no; int
col; } ; typedef struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t; extern char Cyc_Position_Exit[
9u]; struct Cyc_Position_Segment; typedef struct Cyc_Position_Segment* Cyc_Position_seg_t;
extern struct Cyc_List_List* Cyc_Position_strings_of_segments( struct Cyc_List_List*);
static const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1;
static const int Cyc_Position_Elab= 2; typedef void* Cyc_Position_error_kind_t;
struct Cyc_Position_Error{ struct _tagged_string source; struct Cyc_Position_Segment*
seg; void* kind; struct _tagged_string desc; } ; typedef struct Cyc_Position_Error*
Cyc_Position_error_t; extern struct Cyc_Position_Error* Cyc_Position_mk_err_elab(
struct Cyc_Position_Segment*, struct _tagged_string); extern char Cyc_Position_Nocontext[
14u]; extern void Cyc_Position_post_error( struct Cyc_Position_Error*); typedef
struct _tagged_string* Cyc_Absyn_field_name_t; typedef struct _tagged_string*
Cyc_Absyn_var_t; typedef struct _tagged_string* Cyc_Absyn_tvarname_t; typedef
void* Cyc_Absyn_nmspace_t; struct _tuple1{ void* f1; struct _tagged_string* f2;
} ; typedef struct _tuple1* Cyc_Absyn_qvar_t; typedef struct _tuple1* Cyc_Absyn_qvar_opt_t;
typedef struct _tuple1* Cyc_Absyn_typedef_name_t; typedef struct _tuple1* Cyc_Absyn_typedef_name_opt_t;
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
extern char Cyc_Absyn_EmptyAnnot[ 15u]; extern int Cyc_Absyn_qvar_cmp( struct
_tuple1*, struct _tuple1*); extern int Cyc_Absyn_tvar_cmp( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*); extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();
extern struct Cyc_Absyn_Conref* Cyc_Absyn_new_conref( void* x); extern struct
Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref* x); extern
void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_new_evar(
void*); extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_ulong_t; extern
void* Cyc_Absyn_sint_t; extern void* Cyc_Absyn_slong_t; extern void* Cyc_Absyn_float_t;
extern void* Cyc_Absyn_double_t; extern void* Cyc_Absyn_bounds_one; extern void*
Cyc_Absyn_atb_typ( void* t, void* rgn, struct Cyc_Absyn_Tqual tq, void* b);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp( void*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_copy_exp( struct Cyc_Absyn_Exp*); extern
struct Cyc_Absyn_Exp* Cyc_Absyn_signed_int_exp( int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp( unsigned int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_field( struct Cyc_List_List*,
struct _tagged_string*); extern struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_struct_field(
struct Cyc_Absyn_Structdecl*, struct _tagged_string*); extern struct Cyc_Absyn_Structfield*
Cyc_Absyn_lookup_union_field( struct Cyc_Absyn_Uniondecl*, struct _tagged_string*);
struct _tuple4{ struct Cyc_Absyn_Tqual f1; void* f2; } ; extern struct _tuple4*
Cyc_Absyn_lookup_tuple_field( struct Cyc_List_List*, int); extern struct
_tagged_string Cyc_Absyn_attribute2string( void*); struct Cyc_PP_Ppstate;
typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t; struct Cyc_PP_Out; typedef
struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc; typedef struct Cyc_PP_Doc*
Cyc_PP_doc_t; extern struct _tagged_string Cyc_Absynpp_typ2string( void*);
extern struct _tagged_string Cyc_Absynpp_kind2string( void*); extern struct
_tagged_string Cyc_Absynpp_ckind2string( struct Cyc_Absyn_Conref*); extern
struct _tagged_string Cyc_Absynpp_qvar2string( struct _tuple1*); struct Cyc_Set_Set;
typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct Cyc_Set_Set* Cyc_Set_hset_t;
typedef struct Cyc_Set_Set* Cyc_Set_set_t; extern char Cyc_Set_Absent[ 11u];
struct Cyc_Dict_Dict; typedef struct Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef
struct Cyc_Dict_Dict* Cyc_Dict_dict_t; extern char Cyc_Dict_Present[ 12u];
extern char Cyc_Dict_Absent[ 11u]; static const int Cyc_Tcenv_VarRes= 0; struct
Cyc_Tcenv_VarRes_struct{ int tag; void* f1; } ; static const int Cyc_Tcenv_StructRes=
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
typedef struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tenv_t; extern void* Cyc_Tcenv_lookup_ordinary(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple1*); extern
struct Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tuple1*); extern struct Cyc_Absyn_Uniondecl**
Cyc_Tcenv_lookup_uniondecl( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tuple1*); extern struct Cyc_Absyn_Tuniondecl** Cyc_Tcenv_lookup_tuniondecl(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple1*); extern
struct Cyc_Absyn_Enumdecl** Cyc_Tcenv_lookup_enumdecl( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tuple1*); extern struct Cyc_Absyn_Typedefdecl*
Cyc_Tcenv_lookup_typedefdecl( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tuple1*); extern int Cyc_String_strcmp( struct _tagged_string s1, struct
_tagged_string s2); extern int Cyc_String_zstrptrcmp( struct _tagged_string*,
struct _tagged_string*); extern struct _tagged_string Cyc_String_strconcat(
struct _tagged_string, struct _tagged_string); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); extern char Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_impos(
struct _tagged_string); extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*,
struct _tagged_string); extern void Cyc_Tcutil_warn( struct Cyc_Position_Segment*,
struct _tagged_string); extern void Cyc_Tcutil_flush_warnings(); extern struct
Cyc_Core_Opt* Cyc_Tcutil_empty_var_set; extern void* Cyc_Tcutil_typ_kind( void*
t); extern void* Cyc_Tcutil_compress( void* t); extern void Cyc_Tcutil_unchecked_cast(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerce_list(
struct Cyc_Tcenv_Tenv*, void*, struct Cyc_List_List*); extern int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerce_sint_typ(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerce_use(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerceable(
void*); extern int Cyc_Tcutil_silent_castable( struct Cyc_Tcenv_Tenv*, struct
Cyc_Position_Segment*, void*, void*); extern int Cyc_Tcutil_castable( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, void*, void*); extern int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_is_numeric( struct Cyc_Absyn_Exp*);
extern int Cyc_Tcutil_is_function_type( void* t); extern void* Cyc_Tcutil_max_arithmetic_type(
void*, void*); extern int Cyc_Tcutil_unify( void*, void*); extern void* Cyc_Tcutil_substitute(
struct Cyc_List_List*, void*); extern void* Cyc_Tcutil_rsubstitute( struct
_RegionHandle*, struct Cyc_List_List*, void*); extern int Cyc_Tcutil_subset_effect(
int set_to_empty, void* e1, void* e2); extern int Cyc_Tcutil_region_in_effect(
int constrain, void* r, void* e); extern void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl*);
extern struct Cyc_Absyn_Exp* Cyc_Tcutil_default_initializer( struct Cyc_Tcenv_Tenv*,
void*, struct Cyc_Position_Segment*); struct _tuple5{ struct Cyc_Absyn_Tvar* f1;
void* f2; } ; extern struct _tuple5* Cyc_Tcutil_make_inst_var( struct Cyc_Absyn_Tvar*);
extern struct _tuple5* Cyc_Tcutil_r_make_inst_var( struct _RegionHandle*, struct
Cyc_Absyn_Tvar*); extern void Cyc_Tcutil_check_contains_assign( struct Cyc_Absyn_Exp*);
extern struct Cyc_List_List* Cyc_Tcutil_check_valid_type( struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*, struct Cyc_List_List*, void* k, void*); extern void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, void*); extern void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Fndecl*);
extern void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List* bound_tvars, void* k, void*); extern void Cyc_Tcutil_check_unique_vars(
struct Cyc_List_List* vs, struct Cyc_Position_Segment* loc, struct
_tagged_string err_msg); extern void Cyc_Tcutil_check_unique_tvars( struct Cyc_Position_Segment*,
struct Cyc_List_List*); extern void Cyc_Tcutil_check_nonzero_bound( struct Cyc_Position_Segment*,
struct Cyc_Absyn_Conref*); extern void Cyc_Tcutil_check_bound( struct Cyc_Position_Segment*,
unsigned int i, struct Cyc_Absyn_Conref*); extern int Cyc_Tcutil_equal_tqual(
struct Cyc_Absyn_Tqual tq1, struct Cyc_Absyn_Tqual tq2); extern struct Cyc_List_List*
Cyc_Tcutil_resolve_struct_designators( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_List_List* fields); extern int Cyc_Tcutil_is_tagged_pointer_typ(
void*); extern void* Cyc_Tcutil_array_to_ptr( struct Cyc_Tcenv_Tenv*, void* t,
struct Cyc_Absyn_Exp* e); struct _tuple6{ int f1; void* f2; } ; extern struct
_tuple6 Cyc_Tcutil_addressof_props( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e); extern struct Cyc_Absyn_Tvar* Cyc_Tcutil_new_tvar( void* k); extern int Cyc_Tcutil_is_temp_tvar(
struct Cyc_Absyn_Tvar*); extern void Cyc_Tcutil_rewrite_temp_tvar( struct Cyc_Absyn_Tvar*);
extern int Cyc_Tcutil_same_atts( struct Cyc_List_List*, struct Cyc_List_List*);
extern int Cyc_Tcutil_bits_only( void* t); extern int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e); extern void* Cyc_Tcutil_snd_tqt(
struct _tuple4*); extern int Cyc_Tcutil_supports_default( void*); extern void*
Cyc_Tcexp_tcExp( struct Cyc_Tcenv_Tenv*, void**, struct Cyc_Absyn_Exp*); char
Cyc_Tcutil_TypeErr[ 12u]; extern void Cyc_Tcutil_unify_it( void* t1, void* t2);
void Cyc_Tcutil_terr( struct Cyc_Position_Segment* loc, struct _tagged_string s){
Cyc_Position_post_error( Cyc_Position_mk_err_elab( loc, s));} void* Cyc_Tcutil_impos(
struct _tagged_string msg){({ struct _tagged_string _temp0= msg; fprintf( Cyc_Stdio_stderr,"Error: %.*s\n",
_temp0.last_plus_one - _temp0.curr, _temp0.curr);}); Cyc_Stdio_fflush(( struct
Cyc_Stdio___sFILE*) Cyc_Stdio_stderr);( void) _throw(( void*) Cyc_Tcutil_TypeErr);}
static struct _tagged_string Cyc_Tcutil_tvar2string( struct Cyc_Absyn_Tvar* tv){
return* tv->name;} void Cyc_Tcutil_print_tvars( struct Cyc_List_List* tvs){ for(
0; tvs != 0; tvs=({ struct Cyc_List_List* _temp1= tvs; if( _temp1 == 0){ _throw(
Null_Exception);} _temp1->tl;})){({ struct _tagged_string _temp4= Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp2= tvs; if( _temp2 == 0){
_throw( Null_Exception);} _temp2->hd;})); struct _tagged_string _temp5= Cyc_Absynpp_ckind2string(((
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp3= tvs; if( _temp3 == 0){
_throw( Null_Exception);} _temp3->hd;}))->kind); fprintf( Cyc_Stdio_stderr,"%.*s::%.*s ",
_temp4.last_plus_one - _temp4.curr, _temp4.curr, _temp5.last_plus_one - _temp5.curr,
_temp5.curr);});} fprintf( Cyc_Stdio_stderr,"\n"); Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*)
Cyc_Stdio_stderr);} static struct Cyc_List_List* Cyc_Tcutil_warning_segs= 0;
static struct Cyc_List_List* Cyc_Tcutil_warning_msgs= 0; void Cyc_Tcutil_warn(
struct Cyc_Position_Segment* sg, struct _tagged_string msg){ Cyc_Tcutil_warning_segs=({
struct Cyc_List_List* _temp6=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp6->hd=( void*) sg; _temp6->tl= Cyc_Tcutil_warning_segs;
_temp6;}); Cyc_Tcutil_warning_msgs=({ struct Cyc_List_List* _temp7=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp7->hd=( void*)(( struct
_tagged_string*)({ struct _tagged_string* _temp8=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp8[ 0]= msg; _temp8;}));
_temp7->tl= Cyc_Tcutil_warning_msgs; _temp7;});} void Cyc_Tcutil_flush_warnings(){
if( Cyc_Tcutil_warning_segs == 0){ return;} fprintf( Cyc_Stdio_stderr,"***Warnings***\n");{
struct Cyc_List_List* seg_strs= Cyc_Position_strings_of_segments( Cyc_Tcutil_warning_segs);
Cyc_Tcutil_warning_segs= 0; Cyc_Tcutil_warning_msgs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_Tcutil_warning_msgs); while(
Cyc_Tcutil_warning_msgs != 0) {({ struct _tagged_string _temp11=*(( struct
_tagged_string*)({ struct Cyc_List_List* _temp9= seg_strs; if( _temp9 == 0){
_throw( Null_Exception);} _temp9->hd;})); struct _tagged_string _temp12=*((
struct _tagged_string*)({ struct Cyc_List_List* _temp10= Cyc_Tcutil_warning_msgs;
if( _temp10 == 0){ _throw( Null_Exception);} _temp10->hd;})); fprintf( Cyc_Stdio_stderr,"%.*s: %.*s\n",
_temp11.last_plus_one - _temp11.curr, _temp11.curr, _temp12.last_plus_one -
_temp12.curr, _temp12.curr);}); seg_strs=({ struct Cyc_List_List* _temp13=
seg_strs; if( _temp13 == 0){ _throw( Null_Exception);} _temp13->tl;}); Cyc_Tcutil_warning_msgs=({
struct Cyc_List_List* _temp14= Cyc_Tcutil_warning_msgs; if( _temp14 == 0){
_throw( Null_Exception);} _temp14->tl;});} fprintf( Cyc_Stdio_stderr,"**************\n");
Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*) Cyc_Stdio_stderr);}} struct Cyc_Core_Opt*
Cyc_Tcutil_empty_var_set= 0; void* Cyc_Tcutil_compress( void* t){ void* _temp15=
t; int _temp27; struct Cyc_Core_Opt* _temp29; void* _temp31; int _temp33; struct
Cyc_Core_Opt* _temp35; struct Cyc_Core_Opt** _temp37; void* _temp38; struct Cyc_Core_Opt*
_temp40; struct Cyc_List_List* _temp42; struct _tuple1* _temp44; struct Cyc_Core_Opt*
_temp46; struct Cyc_Core_Opt** _temp48; struct Cyc_List_List* _temp49; struct
_tuple1* _temp51; _LL17: if(( unsigned int) _temp15 > 4u?*(( int*) _temp15) ==
Cyc_Absyn_Evar: 0){ _LL32: _temp31=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp15)->f1; goto _LL30; _LL30: _temp29=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp15)->f2; if( _temp29 == 0){ goto _LL28;} else{ goto _LL19;} _LL28: _temp27=(
int)(( struct Cyc_Absyn_Evar_struct*) _temp15)->f3; goto _LL18;} else{ goto
_LL19;} _LL19: if(( unsigned int) _temp15 > 4u?*(( int*) _temp15) == Cyc_Absyn_Evar:
0){ _LL39: _temp38=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp15)->f1; goto
_LL36; _LL36: _temp35=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp15)->f2; _temp37=&(( struct Cyc_Absyn_Evar_struct*) _temp15)->f2; goto
_LL34; _LL34: _temp33=( int)(( struct Cyc_Absyn_Evar_struct*) _temp15)->f3; goto
_LL20;} else{ goto _LL21;} _LL21: if(( unsigned int) _temp15 > 4u?*(( int*)
_temp15) == Cyc_Absyn_TypedefType: 0){ _LL45: _temp44=( struct _tuple1*)((
struct Cyc_Absyn_TypedefType_struct*) _temp15)->f1; goto _LL43; _LL43: _temp42=(
struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*) _temp15)->f2;
goto _LL41; _LL41: _temp40=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp15)->f3; if( _temp40 == 0){ goto _LL22;} else{ goto _LL23;}} else{ goto
_LL23;} _LL23: if(( unsigned int) _temp15 > 4u?*(( int*) _temp15) == Cyc_Absyn_TypedefType:
0){ _LL52: _temp51=( struct _tuple1*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp15)->f1; goto _LL50; _LL50: _temp49=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp15)->f2; goto _LL47; _LL47: _temp46=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp15)->f3; _temp48=&(( struct Cyc_Absyn_TypedefType_struct*) _temp15)->f3;
goto _LL24;} else{ goto _LL25;} _LL25: goto _LL26; _LL18: return t; _LL20: {
void* t2= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp55=* _temp37;
if( _temp55 == 0){ _throw( Null_Exception);} _temp55->v;})); if( t2 !=( void*)({
struct Cyc_Core_Opt* _temp53=* _temp37; if( _temp53 == 0){ _throw(
Null_Exception);} _temp53->v;})){* _temp37=({ struct Cyc_Core_Opt* _temp54=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp54->v=(
void*) t2; _temp54;});} return t2;} _LL22: return t; _LL24: { void* t2= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp58=* _temp48; if( _temp58 == 0){ _throw(
Null_Exception);} _temp58->v;})); if( t2 !=( void*)({ struct Cyc_Core_Opt*
_temp56=* _temp48; if( _temp56 == 0){ _throw( Null_Exception);} _temp56->v;})){*
_temp48=({ struct Cyc_Core_Opt* _temp57=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp57->v=( void*) t2; _temp57;});} return t2;}
_LL26: return t; _LL16:;} struct _tuple7{ void* f1; void* f2; } ; static int Cyc_Tcutil_kind_leq(
void* k1, void* k2){ if( k1 == k2){ return 1;}{ struct _tuple7 _temp60=({ struct
_tuple7 _temp59; _temp59.f1= k1; _temp59.f2= k2; _temp59;}); void* _temp70; void*
_temp72; void* _temp74; void* _temp76; void* _temp78; void* _temp80; _LL62:
_LL73: _temp72= _temp60.f1; if( _temp72 ==( void*) Cyc_Absyn_BoxKind){ goto
_LL71;} else{ goto _LL64;} _LL71: _temp70= _temp60.f2; if( _temp70 ==( void*)
Cyc_Absyn_MemKind){ goto _LL63;} else{ goto _LL64;} _LL64: _LL77: _temp76=
_temp60.f1; if( _temp76 ==( void*) Cyc_Absyn_BoxKind){ goto _LL75;} else{ goto
_LL66;} _LL75: _temp74= _temp60.f2; if( _temp74 ==( void*) Cyc_Absyn_AnyKind){
goto _LL65;} else{ goto _LL66;} _LL66: _LL81: _temp80= _temp60.f1; if( _temp80
==( void*) Cyc_Absyn_MemKind){ goto _LL79;} else{ goto _LL68;} _LL79: _temp78=
_temp60.f2; if( _temp78 ==( void*) Cyc_Absyn_AnyKind){ goto _LL67;} else{ goto
_LL68;} _LL68: goto _LL69; _LL63: return 1; _LL65: return 1; _LL67: return 1;
_LL69: return 0; _LL61:;}} void* Cyc_Tcutil_typ_kind( void* t){ void* _temp82=
Cyc_Tcutil_compress( t); int _temp134; struct Cyc_Core_Opt* _temp136; void*
_temp138; struct Cyc_Absyn_Tvar* _temp140; void* _temp142; void* _temp144;
struct Cyc_Absyn_FnInfo _temp146; void* _temp148; struct Cyc_Absyn_TunionInfo
_temp150; struct Cyc_Absyn_TunionFieldInfo _temp152; struct Cyc_List_List*
_temp154; void* _temp156; struct Cyc_Absyn_Tunionfield* _temp158; struct Cyc_Absyn_Tuniondecl*
_temp160; struct Cyc_Absyn_TunionFieldInfo _temp162; struct Cyc_List_List*
_temp164; void* _temp166; struct Cyc_Absyn_UnknownTunionFieldInfo _temp168;
struct Cyc_Absyn_Structdecl** _temp170; struct Cyc_List_List* _temp172; struct
_tuple1* _temp174; struct Cyc_Absyn_Uniondecl** _temp176; struct Cyc_List_List*
_temp178; struct _tuple1* _temp180; struct Cyc_Absyn_Structdecl** _temp182;
struct Cyc_List_List* _temp184; struct _tuple1* _temp186; struct Cyc_Absyn_Uniondecl**
_temp188; struct Cyc_List_List* _temp190; struct _tuple1* _temp192; struct Cyc_List_List*
_temp194; struct Cyc_List_List* _temp196; struct Cyc_Absyn_Enumdecl* _temp198;
struct _tuple1* _temp200; struct Cyc_Absyn_PtrInfo _temp202; struct Cyc_Absyn_Exp*
_temp204; struct Cyc_Absyn_Tqual _temp206; void* _temp208; struct Cyc_List_List*
_temp210; struct Cyc_Core_Opt* _temp212; struct Cyc_List_List* _temp214; struct
_tuple1* _temp216; void* _temp218; struct Cyc_List_List* _temp220; _LL84: if((
unsigned int) _temp82 > 4u?*(( int*) _temp82) == Cyc_Absyn_Evar: 0){ _LL139:
_temp138=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp82)->f1; goto _LL137;
_LL137: _temp136=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp82)->f2; goto _LL135; _LL135: _temp134=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp82)->f3; goto _LL85;} else{ goto _LL86;} _LL86: if(( unsigned int) _temp82
> 4u?*(( int*) _temp82) == Cyc_Absyn_VarType: 0){ _LL141: _temp140=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_VarType_struct*) _temp82)->f1; goto _LL87;} else{ goto _LL88;}
_LL88: if( _temp82 ==( void*) Cyc_Absyn_VoidType){ goto _LL89;} else{ goto _LL90;}
_LL90: if(( unsigned int) _temp82 > 4u?*(( int*) _temp82) == Cyc_Absyn_IntType:
0){ _LL145: _temp144=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp82)->f1;
goto _LL143; _LL143: _temp142=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp82)->f2; goto _LL91;} else{ goto _LL92;} _LL92: if( _temp82 ==( void*) Cyc_Absyn_FloatType){
goto _LL93;} else{ goto _LL94;} _LL94: if( _temp82 ==( void*) Cyc_Absyn_DoubleType){
goto _LL95;} else{ goto _LL96;} _LL96: if(( unsigned int) _temp82 > 4u?*(( int*)
_temp82) == Cyc_Absyn_FnType: 0){ _LL147: _temp146=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp82)->f1; goto _LL97;} else{ goto _LL98;}
_LL98: if(( unsigned int) _temp82 > 4u?*(( int*) _temp82) == Cyc_Absyn_RgnHandleType:
0){ _LL149: _temp148=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp82)->f1;
goto _LL99;} else{ goto _LL100;} _LL100: if( _temp82 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL101;} else{ goto _LL102;} _LL102: if(( unsigned int) _temp82 > 4u?*((
int*) _temp82) == Cyc_Absyn_TunionType: 0){ _LL151: _temp150=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp82)->f1; goto _LL103;} else{ goto
_LL104;} _LL104: if(( unsigned int) _temp82 > 4u?*(( int*) _temp82) == Cyc_Absyn_TunionFieldType:
0){ _LL153: _temp152=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp82)->f1; _LL157: _temp156=( void*) _temp152.field_info; if(*(( int*)
_temp156) == Cyc_Absyn_KnownTunionfield){ _LL161: _temp160=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp156)->f1; goto _LL159; _LL159:
_temp158=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp156)->f2; goto _LL155;} else{ goto _LL106;} _LL155: _temp154=( struct Cyc_List_List*)
_temp152.targs; goto _LL105;} else{ goto _LL106;} _LL106: if(( unsigned int)
_temp82 > 4u?*(( int*) _temp82) == Cyc_Absyn_TunionFieldType: 0){ _LL163:
_temp162=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp82)->f1; _LL167: _temp166=( void*) _temp162.field_info; if(*(( int*)
_temp166) == Cyc_Absyn_UnknownTunionfield){ _LL169: _temp168=( struct Cyc_Absyn_UnknownTunionFieldInfo)((
struct Cyc_Absyn_UnknownTunionfield_struct*) _temp166)->f1; goto _LL165;} else{
goto _LL108;} _LL165: _temp164=( struct Cyc_List_List*) _temp162.targs; goto
_LL107;} else{ goto _LL108;} _LL108: if(( unsigned int) _temp82 > 4u?*(( int*)
_temp82) == Cyc_Absyn_StructType: 0){ _LL175: _temp174=( struct _tuple1*)((
struct Cyc_Absyn_StructType_struct*) _temp82)->f1; goto _LL173; _LL173: _temp172=(
struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*) _temp82)->f2; goto
_LL171; _LL171: _temp170=( struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*)
_temp82)->f3; if( _temp170 == 0){ goto _LL109;} else{ goto _LL110;}} else{ goto
_LL110;} _LL110: if(( unsigned int) _temp82 > 4u?*(( int*) _temp82) == Cyc_Absyn_UnionType:
0){ _LL181: _temp180=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp82)->f1; goto _LL179; _LL179: _temp178=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp82)->f2; goto _LL177; _LL177: _temp176=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp82)->f3;
if( _temp176 == 0){ goto _LL111;} else{ goto _LL112;}} else{ goto _LL112;}
_LL112: if(( unsigned int) _temp82 > 4u?*(( int*) _temp82) == Cyc_Absyn_StructType:
0){ _LL187: _temp186=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp82)->f1; goto _LL185; _LL185: _temp184=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp82)->f2; goto _LL183; _LL183: _temp182=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp82)->f3;
goto _LL113;} else{ goto _LL114;} _LL114: if(( unsigned int) _temp82 > 4u?*((
int*) _temp82) == Cyc_Absyn_UnionType: 0){ _LL193: _temp192=( struct _tuple1*)((
struct Cyc_Absyn_UnionType_struct*) _temp82)->f1; goto _LL191; _LL191: _temp190=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*) _temp82)->f2; goto
_LL189; _LL189: _temp188=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp82)->f3; goto _LL115;} else{ goto _LL116;} _LL116: if(( unsigned int)
_temp82 > 4u?*(( int*) _temp82) == Cyc_Absyn_AnonStructType: 0){ _LL195:
_temp194=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp82)->f1; goto _LL117;} else{ goto _LL118;} _LL118: if(( unsigned int)
_temp82 > 4u?*(( int*) _temp82) == Cyc_Absyn_AnonUnionType: 0){ _LL197: _temp196=(
struct Cyc_List_List*)(( struct Cyc_Absyn_AnonUnionType_struct*) _temp82)->f1;
goto _LL119;} else{ goto _LL120;} _LL120: if(( unsigned int) _temp82 > 4u?*((
int*) _temp82) == Cyc_Absyn_EnumType: 0){ _LL201: _temp200=( struct _tuple1*)((
struct Cyc_Absyn_EnumType_struct*) _temp82)->f1; goto _LL199; _LL199: _temp198=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*) _temp82)->f2;
goto _LL121;} else{ goto _LL122;} _LL122: if(( unsigned int) _temp82 > 4u?*((
int*) _temp82) == Cyc_Absyn_PointerType: 0){ _LL203: _temp202=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp82)->f1; goto _LL123;} else{ goto
_LL124;} _LL124: if(( unsigned int) _temp82 > 4u?*(( int*) _temp82) == Cyc_Absyn_ArrayType:
0){ _LL209: _temp208=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp82)->f1;
goto _LL207; _LL207: _temp206=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp82)->f2; goto _LL205; _LL205: _temp204=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp82)->f3; goto _LL125;} else{ goto _LL126;}
_LL126: if(( unsigned int) _temp82 > 4u?*(( int*) _temp82) == Cyc_Absyn_TupleType:
0){ _LL211: _temp210=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp82)->f1; goto _LL127;} else{ goto _LL128;} _LL128: if(( unsigned int)
_temp82 > 4u?*(( int*) _temp82) == Cyc_Absyn_TypedefType: 0){ _LL217: _temp216=(
struct _tuple1*)(( struct Cyc_Absyn_TypedefType_struct*) _temp82)->f1; goto
_LL215; _LL215: _temp214=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp82)->f2; goto _LL213; _LL213: _temp212=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp82)->f3; goto _LL129;} else{ goto _LL130;} _LL130: if(( unsigned int)
_temp82 > 4u?*(( int*) _temp82) == Cyc_Absyn_AccessEff: 0){ _LL219: _temp218=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp82)->f1; goto _LL131;} else{
goto _LL132;} _LL132: if(( unsigned int) _temp82 > 4u?*(( int*) _temp82) == Cyc_Absyn_JoinEff:
0){ _LL221: _temp220=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*)
_temp82)->f1; goto _LL133;} else{ goto _LL83;} _LL85: return _temp138; _LL87:
return(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp140->kind);
_LL89: return( void*) Cyc_Absyn_MemKind; _LL91: return _temp142 ==( void*) Cyc_Absyn_B4?(
void*) Cyc_Absyn_BoxKind:( void*) Cyc_Absyn_MemKind; _LL93: return( void*) Cyc_Absyn_MemKind;
_LL95: return( void*) Cyc_Absyn_MemKind; _LL97: return( void*) Cyc_Absyn_MemKind;
_LL99: return( void*) Cyc_Absyn_BoxKind; _LL101: return( void*) Cyc_Absyn_RgnKind;
_LL103: return( void*) Cyc_Absyn_BoxKind; _LL105: if( _temp158->typs == 0){
return( void*) Cyc_Absyn_BoxKind;} else{ return( void*) Cyc_Absyn_MemKind;}
_LL107: return(( void*(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp222=( char*)"typ_kind: Unresolved TunionFieldType";
struct _tagged_string _temp223; _temp223.curr= _temp222; _temp223.base= _temp222;
_temp223.last_plus_one= _temp222 + 37; _temp223;})); _LL109: return( void*) Cyc_Absyn_AnyKind;
_LL111: return( void*) Cyc_Absyn_AnyKind; _LL113: return( void*) Cyc_Absyn_MemKind;
_LL115: return( void*) Cyc_Absyn_MemKind; _LL117: return( void*) Cyc_Absyn_MemKind;
_LL119: return( void*) Cyc_Absyn_MemKind; _LL121: return( void*) Cyc_Absyn_BoxKind;
_LL123: { void* _temp224=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp202.bounds))->v; void* _temp234; void*
_temp236; struct Cyc_Absyn_Exp* _temp238; struct Cyc_Absyn_Conref* _temp240;
_LL226: if(( unsigned int) _temp224 > 1u?*(( int*) _temp224) == Cyc_Absyn_Eq_constr:
0){ _LL235: _temp234=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp224)->f1;
if( _temp234 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL227;} else{ goto _LL228;}}
else{ goto _LL228;} _LL228: if(( unsigned int) _temp224 > 1u?*(( int*) _temp224)
== Cyc_Absyn_Eq_constr: 0){ _LL237: _temp236=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp224)->f1; if(( unsigned int) _temp236 > 1u?*(( int*) _temp236) == Cyc_Absyn_Upper_b:
0){ _LL239: _temp238=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp236)->f1; goto _LL229;} else{ goto _LL230;}} else{ goto _LL230;} _LL230:
if( _temp224 ==( void*) Cyc_Absyn_No_constr){ goto _LL231;} else{ goto _LL232;}
_LL232: if(( unsigned int) _temp224 > 1u?*(( int*) _temp224) == Cyc_Absyn_Forward_constr:
0){ _LL241: _temp240=( struct Cyc_Absyn_Conref*)(( struct Cyc_Absyn_Forward_constr_struct*)
_temp224)->f1; goto _LL233;} else{ goto _LL225;} _LL227: return( void*) Cyc_Absyn_MemKind;
_LL229: return( void*) Cyc_Absyn_BoxKind; _LL231: return( void*) Cyc_Absyn_MemKind;
_LL233: return(( void*(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp242=( char*)"typ_kind: forward constraint in ptr bounds";
struct _tagged_string _temp243; _temp243.curr= _temp242; _temp243.base= _temp242;
_temp243.last_plus_one= _temp242 + 43; _temp243;})); _LL225:;} _LL125: return(
void*) Cyc_Absyn_MemKind; _LL127: return( void*) Cyc_Absyn_MemKind; _LL129:
return(( void*(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({ struct
_tagged_string _temp244= Cyc_Absynpp_typ2string( t); xprintf("typ_kind: typedef found: %.*s",
_temp244.last_plus_one - _temp244.curr, _temp244.curr);})); _LL131: return( void*)
Cyc_Absyn_EffKind; _LL133: return( void*) Cyc_Absyn_EffKind; _LL83:;} char Cyc_Tcutil_Unify[
10u]="\000\000\000\000Unify"; int Cyc_Tcutil_unify( void* t1, void* t2){ struct
_handler_cons _temp245; _push_handler(& _temp245);{ int _temp247= 0; if( setjmp(
_temp245.handler)){ _temp247= 1;} if( ! _temp247){ Cyc_Tcutil_unify_it( t1, t2);{
int _temp248= 1; _npop_handler( 0u); return _temp248;}; _pop_handler();} else{
void* _temp246=( void*) _exn_thrown; void* _temp250= _temp246; _LL252: if(
_temp250 == Cyc_Tcutil_Unify){ goto _LL253;} else{ goto _LL254;} _LL254: goto
_LL255; _LL253: return 0; _LL255:( void) _throw( _temp250); _LL251:;}}} static
void Cyc_Tcutil_occurslist( void* evar, struct Cyc_List_List* ts); static void
Cyc_Tcutil_occurs( void* evar, void* t){ void* _temp256= Cyc_Tcutil_compress( t);
int _temp288; struct Cyc_Core_Opt* _temp290; void* _temp292; struct Cyc_Absyn_PtrInfo
_temp294; struct Cyc_Absyn_Exp* _temp296; struct Cyc_Absyn_Tqual _temp298; void*
_temp300; struct Cyc_Absyn_FnInfo _temp302; struct Cyc_List_List* _temp304; int
_temp306; struct Cyc_List_List* _temp308; void* _temp310; struct Cyc_Core_Opt*
_temp312; struct Cyc_List_List* _temp314; struct Cyc_List_List* _temp316; void*
_temp318; struct Cyc_Absyn_TunionInfo _temp320; void* _temp322; struct Cyc_List_List*
_temp324; void* _temp326; struct Cyc_Core_Opt* _temp328; struct Cyc_List_List*
_temp330; struct _tuple1* _temp332; struct Cyc_Absyn_TunionFieldInfo _temp334;
struct Cyc_List_List* _temp336; void* _temp338; struct Cyc_Absyn_Structdecl**
_temp340; struct Cyc_List_List* _temp342; struct _tuple1* _temp344; struct Cyc_List_List*
_temp346; struct Cyc_List_List* _temp348; void* _temp350; struct Cyc_List_List*
_temp352; _LL258: if(( unsigned int) _temp256 > 4u?*(( int*) _temp256) == Cyc_Absyn_Evar:
0){ _LL293: _temp292=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp256)->f1;
goto _LL291; _LL291: _temp290=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp256)->f2; goto _LL289; _LL289: _temp288=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp256)->f3; goto _LL259;} else{ goto _LL260;} _LL260: if(( unsigned int)
_temp256 > 4u?*(( int*) _temp256) == Cyc_Absyn_PointerType: 0){ _LL295: _temp294=(
struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp256)->f1;
goto _LL261;} else{ goto _LL262;} _LL262: if(( unsigned int) _temp256 > 4u?*((
int*) _temp256) == Cyc_Absyn_ArrayType: 0){ _LL301: _temp300=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp256)->f1; goto _LL299; _LL299: _temp298=(
struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*) _temp256)->f2;
goto _LL297; _LL297: _temp296=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp256)->f3; goto _LL263;} else{ goto _LL264;} _LL264: if(( unsigned int)
_temp256 > 4u?*(( int*) _temp256) == Cyc_Absyn_FnType: 0){ _LL303: _temp302=(
struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*) _temp256)->f1;
_LL315: _temp314=( struct Cyc_List_List*) _temp302.tvars; goto _LL313; _LL313:
_temp312=( struct Cyc_Core_Opt*) _temp302.effect; goto _LL311; _LL311: _temp310=(
void*) _temp302.ret_typ; goto _LL309; _LL309: _temp308=( struct Cyc_List_List*)
_temp302.args; goto _LL307; _LL307: _temp306=( int) _temp302.varargs; goto
_LL305; _LL305: _temp304=( struct Cyc_List_List*) _temp302.attributes; goto
_LL265;} else{ goto _LL266;} _LL266: if(( unsigned int) _temp256 > 4u?*(( int*)
_temp256) == Cyc_Absyn_TupleType: 0){ _LL317: _temp316=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp256)->f1; goto _LL267;} else{ goto
_LL268;} _LL268: if(( unsigned int) _temp256 > 4u?*(( int*) _temp256) == Cyc_Absyn_RgnHandleType:
0){ _LL319: _temp318=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp256)->f1;
goto _LL269;} else{ goto _LL270;} _LL270: if(( unsigned int) _temp256 > 4u?*((
int*) _temp256) == Cyc_Absyn_TunionType: 0){ _LL321: _temp320=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp256)->f1; _LL327: _temp326=( void*)
_temp320.tunion_info; goto _LL325; _LL325: _temp324=( struct Cyc_List_List*)
_temp320.targs; goto _LL323; _LL323: _temp322=( void*) _temp320.rgn; goto _LL271;}
else{ goto _LL272;} _LL272: if(( unsigned int) _temp256 > 4u?*(( int*) _temp256)
== Cyc_Absyn_TypedefType: 0){ _LL333: _temp332=( struct _tuple1*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp256)->f1; goto _LL331; _LL331: _temp330=( struct Cyc_List_List*)(( struct
Cyc_Absyn_TypedefType_struct*) _temp256)->f2; goto _LL329; _LL329: _temp328=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_TypedefType_struct*) _temp256)->f3;
goto _LL273;} else{ goto _LL274;} _LL274: if(( unsigned int) _temp256 > 4u?*((
int*) _temp256) == Cyc_Absyn_TunionFieldType: 0){ _LL335: _temp334=( struct Cyc_Absyn_TunionFieldInfo)((
struct Cyc_Absyn_TunionFieldType_struct*) _temp256)->f1; _LL339: _temp338=( void*)
_temp334.field_info; goto _LL337; _LL337: _temp336=( struct Cyc_List_List*)
_temp334.targs; goto _LL275;} else{ goto _LL276;} _LL276: if(( unsigned int)
_temp256 > 4u?*(( int*) _temp256) == Cyc_Absyn_StructType: 0){ _LL345: _temp344=(
struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp256)->f1; goto
_LL343; _LL343: _temp342=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp256)->f2; goto _LL341; _LL341: _temp340=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp256)->f3; goto _LL277;} else{ goto
_LL278;} _LL278: if(( unsigned int) _temp256 > 4u?*(( int*) _temp256) == Cyc_Absyn_AnonStructType:
0){ _LL347: _temp346=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp256)->f1; goto _LL279;} else{ goto _LL280;} _LL280: if(( unsigned int)
_temp256 > 4u?*(( int*) _temp256) == Cyc_Absyn_AnonUnionType: 0){ _LL349:
_temp348=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp256)->f1; goto _LL281;} else{ goto _LL282;} _LL282: if(( unsigned int)
_temp256 > 4u?*(( int*) _temp256) == Cyc_Absyn_AccessEff: 0){ _LL351: _temp350=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp256)->f1; goto _LL283;} else{
goto _LL284;} _LL284: if(( unsigned int) _temp256 > 4u?*(( int*) _temp256) ==
Cyc_Absyn_JoinEff: 0){ _LL353: _temp352=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*)
_temp256)->f1; goto _LL285;} else{ goto _LL286;} _LL286: goto _LL287; _LL259:
if( t == evar){( void) _throw(( void*) Cyc_Tcutil_Unify);} else{ if( _temp290 !=
0){ Cyc_Tcutil_occurs( evar,( void*)({ struct Cyc_Core_Opt* _temp354= _temp290;
if( _temp354 == 0){ _throw( Null_Exception);} _temp354->v;}));}} goto _LL257;
_LL261: Cyc_Tcutil_occurs( evar,( void*) _temp294.elt_typ); Cyc_Tcutil_occurs(
evar,( void*) _temp294.rgn_typ); goto _LL257; _LL263: Cyc_Tcutil_occurs( evar,
_temp300); goto _LL257; _LL265: if( _temp312 != 0){ Cyc_Tcutil_occurs( evar,(
void*)({ struct Cyc_Core_Opt* _temp355= _temp312; if( _temp355 == 0){ _throw(
Null_Exception);} _temp355->v;}));} Cyc_Tcutil_occurs( evar, _temp310); for( 0;
_temp308 != 0; _temp308=({ struct Cyc_List_List* _temp356= _temp308; if(
_temp356 == 0){ _throw( Null_Exception);} _temp356->tl;})){ Cyc_Tcutil_occurs(
evar,(*(( struct _tuple2*)({ struct Cyc_List_List* _temp357= _temp308; if(
_temp357 == 0){ _throw( Null_Exception);} _temp357->hd;}))).f3);} goto _LL257;
_LL267: for( 0; _temp316 != 0; _temp316=({ struct Cyc_List_List* _temp358=
_temp316; if( _temp358 == 0){ _throw( Null_Exception);} _temp358->tl;})){ Cyc_Tcutil_occurs(
evar,(*(( struct _tuple4*)({ struct Cyc_List_List* _temp359= _temp316; if(
_temp359 == 0){ _throw( Null_Exception);} _temp359->hd;}))).f2);} goto _LL257;
_LL269: Cyc_Tcutil_occurs( evar, _temp318); goto _LL257; _LL271: Cyc_Tcutil_occurs(
evar, _temp322); Cyc_Tcutil_occurslist( evar, _temp324); goto _LL257; _LL273:
_temp336= _temp330; goto _LL275; _LL275: _temp342= _temp336; goto _LL277; _LL277:
Cyc_Tcutil_occurslist( evar, _temp342); goto _LL257; _LL279: _temp348= _temp346;
goto _LL281; _LL281: for( 0; _temp348 != 0; _temp348=({ struct Cyc_List_List*
_temp360= _temp348; if( _temp360 == 0){ _throw( Null_Exception);} _temp360->tl;})){
Cyc_Tcutil_occurs( evar,( void*)(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp361= _temp348; if( _temp361 == 0){ _throw( Null_Exception);} _temp361->hd;}))->type);}
goto _LL257; _LL283: Cyc_Tcutil_occurs( evar, _temp350); goto _LL257; _LL285:
Cyc_Tcutil_occurslist( evar, _temp352); goto _LL257; _LL287: goto _LL257; _LL257:;}
static void Cyc_Tcutil_occurslist( void* evar, struct Cyc_List_List* ts){ for( 0;
ts != 0; ts=({ struct Cyc_List_List* _temp362= ts; if( _temp362 == 0){ _throw(
Null_Exception);} _temp362->tl;})){ Cyc_Tcutil_occurs( evar,( void*)({ struct
Cyc_List_List* _temp363= ts; if( _temp363 == 0){ _throw( Null_Exception);}
_temp363->hd;}));}} static void Cyc_Tcutil_unify_list( struct Cyc_List_List* t1,
struct Cyc_List_List* t2){ struct _handler_cons _temp364; _push_handler(&
_temp364);{ int _temp366= 0; if( setjmp( _temp364.handler)){ _temp366= 1;} if( !
_temp366){(( void(*)( void(* f)( void*, void*), struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_iter2)( Cyc_Tcutil_unify_it, t1, t2);; _pop_handler();}
else{ void* _temp365=( void*) _exn_thrown; void* _temp368= _temp365; _LL370: if(
_temp368 == Cyc_List_List_mismatch){ goto _LL371;} else{ goto _LL372;} _LL372:
goto _LL373; _LL371:( void) _throw(( void*) Cyc_Tcutil_Unify); _LL373:( void)
_throw( _temp368); _LL369:;}}} static void Cyc_Tcutil_unify_tqual( struct Cyc_Absyn_Tqual
tq1, struct Cyc_Absyn_Tqual tq2){ if(( tq1.q_const != tq2.q_const? 1: tq1.q_volatile
!= tq2.q_volatile)? 1: tq1.q_restrict != tq2.q_restrict){( void) _throw(( void*)
Cyc_Tcutil_Unify);}} int Cyc_Tcutil_equal_tqual( struct Cyc_Absyn_Tqual tq1,
struct Cyc_Absyn_Tqual tq2){ return( tq1.q_const == tq2.q_const? tq1.q_volatile
== tq2.q_volatile: 0)? tq1.q_restrict == tq2.q_restrict: 0;} static void Cyc_Tcutil_unify_it_conrefs(
int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y){ x=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
x); y=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
y); if( x == y){ return;}{ void* _temp374=( void*) x->v; struct Cyc_Absyn_Conref*
_temp382; void* _temp384; _LL376: if( _temp374 ==( void*) Cyc_Absyn_No_constr){
goto _LL377;} else{ goto _LL378;} _LL378: if(( unsigned int) _temp374 > 1u?*((
int*) _temp374) == Cyc_Absyn_Forward_constr: 0){ _LL383: _temp382=( struct Cyc_Absyn_Conref*)((
struct Cyc_Absyn_Forward_constr_struct*) _temp374)->f1; goto _LL379;} else{ goto
_LL380;} _LL380: if(( unsigned int) _temp374 > 1u?*(( int*) _temp374) == Cyc_Absyn_Eq_constr:
0){ _LL385: _temp384=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp374)->f1;
goto _LL381;} else{ goto _LL375;} _LL377:( void*)( x->v=( void*)(( void*)({
struct Cyc_Absyn_Forward_constr_struct* _temp386=( struct Cyc_Absyn_Forward_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp386[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp387; _temp387.tag= Cyc_Absyn_Forward_constr;
_temp387.f1= y; _temp387;}); _temp386;}))); return; _LL379:( void) _throw(((
void*(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp388=( char*)"unify_conref: forward after compress"; struct
_tagged_string _temp389; _temp389.curr= _temp388; _temp389.base= _temp388;
_temp389.last_plus_one= _temp388 + 37; _temp389;}))); _LL381: { void* _temp390=(
void*) y->v; struct Cyc_Absyn_Conref* _temp398; void* _temp400; _LL392: if(
_temp390 ==( void*) Cyc_Absyn_No_constr){ goto _LL393;} else{ goto _LL394;}
_LL394: if(( unsigned int) _temp390 > 1u?*(( int*) _temp390) == Cyc_Absyn_Forward_constr:
0){ _LL399: _temp398=( struct Cyc_Absyn_Conref*)(( struct Cyc_Absyn_Forward_constr_struct*)
_temp390)->f1; goto _LL395;} else{ goto _LL396;} _LL396: if(( unsigned int)
_temp390 > 1u?*(( int*) _temp390) == Cyc_Absyn_Eq_constr: 0){ _LL401: _temp400=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp390)->f1; goto _LL397;} else{
goto _LL391;} _LL393:( void*)( y->v=( void*)(( void*) x->v)); return; _LL395:(
void) _throw((( void*(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ char* _temp402=( char*)"unify_conref: forward after compress(2)";
struct _tagged_string _temp403; _temp403.curr= _temp402; _temp403.base= _temp402;
_temp403.last_plus_one= _temp402 + 40; _temp403;}))); _LL397: if( cmp( _temp384,
_temp400) != 0){( void) _throw(( void*) Cyc_Tcutil_Unify);} return; _LL391:;}
_LL375:;}} static int Cyc_Tcutil_unify_conrefs( int(* cmp)( void*, void*),
struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y){ struct _handler_cons
_temp404; _push_handler(& _temp404);{ int _temp406= 0; if( setjmp( _temp404.handler)){
_temp406= 1;} if( ! _temp406){(( void(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref*
x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_it_conrefs)( cmp, x, y);{ int
_temp407= 1; _npop_handler( 0u); return _temp407;}; _pop_handler();} else{ void*
_temp405=( void*) _exn_thrown; void* _temp409= _temp405; _LL411: if( _temp409 ==
Cyc_Tcutil_Unify){ goto _LL412;} else{ goto _LL413;} _LL413: goto _LL414; _LL412:
return 0; _LL414:( void) _throw( _temp409); _LL410:;}}} static int Cyc_Tcutil_boundscmp(
void* b1, void* b2){ struct _tuple7 _temp416=({ struct _tuple7 _temp415;
_temp415.f1= b1; _temp415.f2= b2; _temp415;}); void* _temp426; void* _temp428;
void* _temp430; void* _temp432; void* _temp434; void* _temp436; void* _temp438;
struct Cyc_Absyn_Exp* _temp440; void* _temp442; struct Cyc_Absyn_Exp* _temp444;
_LL418: _LL429: _temp428= _temp416.f1; if( _temp428 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL427;} else{ goto _LL420;} _LL427: _temp426= _temp416.f2; if( _temp426 ==(
void*) Cyc_Absyn_Unknown_b){ goto _LL419;} else{ goto _LL420;} _LL420: _LL433:
_temp432= _temp416.f1; if( _temp432 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL431;}
else{ goto _LL422;} _LL431: _temp430= _temp416.f2; goto _LL421; _LL422: _LL437:
_temp436= _temp416.f1; goto _LL435; _LL435: _temp434= _temp416.f2; if( _temp434
==( void*) Cyc_Absyn_Unknown_b){ goto _LL423;} else{ goto _LL424;} _LL424:
_LL443: _temp442= _temp416.f1; if(( unsigned int) _temp442 > 1u?*(( int*)
_temp442) == Cyc_Absyn_Upper_b: 0){ _LL445: _temp444=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Upper_b_struct*) _temp442)->f1; goto _LL439;} else{ goto _LL417;}
_LL439: _temp438= _temp416.f2; if(( unsigned int) _temp438 > 1u?*(( int*)
_temp438) == Cyc_Absyn_Upper_b: 0){ _LL441: _temp440=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Upper_b_struct*) _temp438)->f1; goto _LL425;} else{ goto _LL417;}
_LL419: return 0; _LL421: return - 1; _LL423: return 1; _LL425: { int i1=( int)
Cyc_Evexp_eval_const_uint_exp( _temp444); int i2=( int) Cyc_Evexp_eval_const_uint_exp(
_temp440); if( i1 == i2){ return 0;} if( i1 < i2){ return - 1;} return 1;}
_LL417:;} static int Cyc_Tcutil_equal_att( void* a1, void* a2){ if( a1 == a2){
return 1;}{ struct _tuple7 _temp447=({ struct _tuple7 _temp446; _temp446.f1= a1;
_temp446.f2= a2; _temp446;}); void* _temp457; int _temp459; void* _temp461; int
_temp463; void* _temp465; int _temp467; void* _temp469; int _temp471; void*
_temp473; struct _tagged_string _temp475; void* _temp477; struct _tagged_string
_temp479; _LL449: _LL462: _temp461= _temp447.f1; if(( unsigned int) _temp461 >
15u?*(( int*) _temp461) == Cyc_Absyn_Regparm_att: 0){ _LL464: _temp463=( int)((
struct Cyc_Absyn_Regparm_att_struct*) _temp461)->f1; goto _LL458;} else{ goto
_LL451;} _LL458: _temp457= _temp447.f2; if(( unsigned int) _temp457 > 15u?*((
int*) _temp457) == Cyc_Absyn_Regparm_att: 0){ _LL460: _temp459=( int)(( struct
Cyc_Absyn_Regparm_att_struct*) _temp457)->f1; goto _LL450;} else{ goto _LL451;}
_LL451: _LL470: _temp469= _temp447.f1; if(( unsigned int) _temp469 > 15u?*(( int*)
_temp469) == Cyc_Absyn_Aligned_att: 0){ _LL472: _temp471=( int)(( struct Cyc_Absyn_Aligned_att_struct*)
_temp469)->f1; goto _LL466;} else{ goto _LL453;} _LL466: _temp465= _temp447.f2;
if(( unsigned int) _temp465 > 15u?*(( int*) _temp465) == Cyc_Absyn_Aligned_att:
0){ _LL468: _temp467=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp465)->f1;
goto _LL452;} else{ goto _LL453;} _LL453: _LL478: _temp477= _temp447.f1; if((
unsigned int) _temp477 > 15u?*(( int*) _temp477) == Cyc_Absyn_Section_att: 0){
_LL480: _temp479=( struct _tagged_string)(( struct Cyc_Absyn_Section_att_struct*)
_temp477)->f1; goto _LL474;} else{ goto _LL455;} _LL474: _temp473= _temp447.f2;
if(( unsigned int) _temp473 > 15u?*(( int*) _temp473) == Cyc_Absyn_Section_att:
0){ _LL476: _temp475=( struct _tagged_string)(( struct Cyc_Absyn_Section_att_struct*)
_temp473)->f1; goto _LL454;} else{ goto _LL455;} _LL455: goto _LL456; _LL450:
_temp471= _temp463; _temp467= _temp459; goto _LL452; _LL452: return _temp471 ==
_temp467; _LL454: return Cyc_String_strcmp( _temp479, _temp475) == 0; _LL456:
return 0; _LL448:;}} int Cyc_Tcutil_same_atts( struct Cyc_List_List* a1, struct
Cyc_List_List* a2){{ struct Cyc_List_List* a= a1; for( 0; a != 0; a=({ struct
Cyc_List_List* _temp481= a; if( _temp481 == 0){ _throw( Null_Exception);}
_temp481->tl;})){ if( !(( int(*)( int(* pred)( void*, void*), void* env, struct
Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcutil_equal_att,( void*)({ struct
Cyc_List_List* _temp482= a; if( _temp482 == 0){ _throw( Null_Exception);}
_temp482->hd;}), a2)){ return 0;}}}{ struct Cyc_List_List* a= a2; for( 0; a != 0;
a=({ struct Cyc_List_List* _temp483= a; if( _temp483 == 0){ _throw(
Null_Exception);} _temp483->tl;})){ if( !(( int(*)( int(* pred)( void*, void*),
void* env, struct Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcutil_equal_att,(
void*)({ struct Cyc_List_List* _temp484= a; if( _temp484 == 0){ _throw(
Null_Exception);} _temp484->hd;}), a1)){ return 0;}}} return 1;} static void Cyc_Tcutil_normalize_effect(
void* e){ e= Cyc_Tcutil_compress( e);{ void* _temp485= e; struct Cyc_List_List*
_temp491; struct Cyc_List_List** _temp493; _LL487: if(( unsigned int) _temp485 >
4u?*(( int*) _temp485) == Cyc_Absyn_JoinEff: 0){ _LL492: _temp491=( struct Cyc_List_List*)((
struct Cyc_Absyn_JoinEff_struct*) _temp485)->f1; _temp493=&(( struct Cyc_Absyn_JoinEff_struct*)
_temp485)->f1; goto _LL488;} else{ goto _LL489;} _LL489: goto _LL490; _LL488: {
int nested_join= 0;{ struct Cyc_List_List* effs=* _temp493; for( 0; effs != 0;
effs=({ struct Cyc_List_List* _temp494= effs; if( _temp494 == 0){ _throw(
Null_Exception);} _temp494->tl;})){ void* eff=( void*)({ struct Cyc_List_List*
_temp503= effs; if( _temp503 == 0){ _throw( Null_Exception);} _temp503->hd;});
Cyc_Tcutil_normalize_effect( eff);{ void* _temp495= Cyc_Tcutil_compress( eff);
struct Cyc_List_List* _temp501; _LL497: if(( unsigned int) _temp495 > 4u?*(( int*)
_temp495) == Cyc_Absyn_JoinEff: 0){ _LL502: _temp501=( struct Cyc_List_List*)((
struct Cyc_Absyn_JoinEff_struct*) _temp495)->f1; goto _LL498;} else{ goto _LL499;}
_LL499: goto _LL500; _LL498: nested_join= 1; goto _LL496; _LL500: goto _LL496;
_LL496:;}}} if( ! nested_join){ return;}{ struct Cyc_List_List* effects= 0;{
struct Cyc_List_List* effs=* _temp493; for( 0; effs != 0; effs=({ struct Cyc_List_List*
_temp504= effs; if( _temp504 == 0){ _throw( Null_Exception);} _temp504->tl;})){
void* _temp506= Cyc_Tcutil_compress(( void*)({ struct Cyc_List_List* _temp505=
effs; if( _temp505 == 0){ _throw( Null_Exception);} _temp505->hd;})); struct Cyc_List_List*
_temp512; _LL508: if(( unsigned int) _temp506 > 4u?*(( int*) _temp506) == Cyc_Absyn_JoinEff:
0){ _LL513: _temp512=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*)
_temp506)->f1; goto _LL509;} else{ goto _LL510;} _LL510: goto _LL511; _LL509:
effects=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_revappend)( _temp512, effects); goto _LL507; _LL511: effects=({
struct Cyc_List_List* _temp514=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp514->hd=( void*) _temp506; _temp514->tl= effects;
_temp514;}); goto _LL507; _LL507:;}}* _temp493=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( effects); goto _LL486;}} _LL490:
goto _LL486; _LL486:;}} struct _tuple8{ void* f1; struct Cyc_List_List* f2; } ;
static struct _tuple8* Cyc_Tcutil_get_effect_evar( void* t){ void* _temp515= Cyc_Tcutil_compress(
t); struct Cyc_List_List* _temp523; struct Cyc_List_List _temp525; struct Cyc_List_List*
_temp526; void* _temp528; int _temp530; struct Cyc_Core_Opt* _temp532; struct
Cyc_Core_Opt** _temp534; void* _temp535; void** _temp537; _LL517: if((
unsigned int) _temp515 > 4u?*(( int*) _temp515) == Cyc_Absyn_JoinEff: 0){ _LL524:
_temp523=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp515)->f1;
if( _temp523 == 0){ goto _LL519;} else{ _temp525=* _temp523; _LL529: _temp528=(
void*) _temp525.hd; goto _LL527; _LL527: _temp526=( struct Cyc_List_List*)
_temp525.tl; goto _LL518;}} else{ goto _LL519;} _LL519: if(( unsigned int)
_temp515 > 4u?*(( int*) _temp515) == Cyc_Absyn_Evar: 0){ _LL536: _temp535=( void*)((
struct Cyc_Absyn_Evar_struct*) _temp515)->f1; _temp537=&(( struct Cyc_Absyn_Evar_struct*)
_temp515)->f1; goto _LL533; _LL533: _temp532=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Evar_struct*) _temp515)->f2; _temp534=&(( struct Cyc_Absyn_Evar_struct*)
_temp515)->f2; goto _LL531; _LL531: _temp530=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp515)->f3; goto _LL520;} else{ goto _LL521;} _LL521: goto _LL522; _LL518: {
void* _temp538= Cyc_Tcutil_compress( _temp528); int _temp544; struct Cyc_Core_Opt*
_temp546; void* _temp548; _LL540: if(( unsigned int) _temp538 > 4u?*(( int*)
_temp538) == Cyc_Absyn_Evar: 0){ _LL549: _temp548=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp538)->f1; goto _LL547; _LL547: _temp546=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Evar_struct*) _temp538)->f2; goto _LL545; _LL545: _temp544=( int)((
struct Cyc_Absyn_Evar_struct*) _temp538)->f3; goto _LL541;} else{ goto _LL542;}
_LL542: goto _LL543; _LL541: return({ struct _tuple8* _temp550=( struct _tuple8*)
GC_malloc( sizeof( struct _tuple8)); _temp550->f1= _temp528; _temp550->f2=
_temp526; _temp550;}); _LL543: return 0; _LL539:;} _LL520: if(* _temp537 !=(
void*) Cyc_Absyn_EffKind){(( void(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp551=( char*)"effect evar has wrong kind";
struct _tagged_string _temp552; _temp552.curr= _temp551; _temp552.base= _temp551;
_temp552.last_plus_one= _temp551 + 27; _temp552;}));} return({ struct _tuple8*
_temp553=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8)); _temp553->f1= t;
_temp553->f2= 0; _temp553;}); _LL522: return 0; _LL516:;} int Cyc_Tcutil_region_in_effect(
int constrain, void* r, void* e){ r= Cyc_Tcutil_compress( r); if( r ==( void*)
Cyc_Absyn_HeapRgn){ return 1;}{ void* _temp554= Cyc_Tcutil_compress( e); void*
_temp564; struct Cyc_List_List* _temp566; int _temp568; struct Cyc_Core_Opt*
_temp570; struct Cyc_Core_Opt** _temp572; void* _temp573; void** _temp575;
_LL556: if(( unsigned int) _temp554 > 4u?*(( int*) _temp554) == Cyc_Absyn_AccessEff:
0){ _LL565: _temp564=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp554)->f1;
goto _LL557;} else{ goto _LL558;} _LL558: if(( unsigned int) _temp554 > 4u?*((
int*) _temp554) == Cyc_Absyn_JoinEff: 0){ _LL567: _temp566=( struct Cyc_List_List*)((
struct Cyc_Absyn_JoinEff_struct*) _temp554)->f1; goto _LL559;} else{ goto _LL560;}
_LL560: if(( unsigned int) _temp554 > 4u?*(( int*) _temp554) == Cyc_Absyn_Evar:
0){ _LL574: _temp573=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp554)->f1;
_temp575=&(( struct Cyc_Absyn_Evar_struct*) _temp554)->f1; goto _LL571; _LL571:
_temp570=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp554)->f2;
_temp572=&(( struct Cyc_Absyn_Evar_struct*) _temp554)->f2; goto _LL569; _LL569:
_temp568=( int)(( struct Cyc_Absyn_Evar_struct*) _temp554)->f3; goto _LL561;}
else{ goto _LL562;} _LL562: goto _LL563; _LL557: if( constrain){ return Cyc_Tcutil_unify(
r, _temp564);} Cyc_Tcutil_compress( _temp564); if( r == _temp564){ return 1;}{
struct _tuple7 _temp577=({ struct _tuple7 _temp576; _temp576.f1= r; _temp576.f2=
_temp564; _temp576;}); void* _temp583; struct Cyc_Absyn_Tvar* _temp585; void*
_temp587; struct Cyc_Absyn_Tvar* _temp589; _LL579: _LL588: _temp587= _temp577.f1;
if(( unsigned int) _temp587 > 4u?*(( int*) _temp587) == Cyc_Absyn_VarType: 0){
_LL590: _temp589=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp587)->f1; goto _LL584;} else{ goto _LL581;} _LL584: _temp583= _temp577.f2;
if(( unsigned int) _temp583 > 4u?*(( int*) _temp583) == Cyc_Absyn_VarType: 0){
_LL586: _temp585=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp583)->f1; goto _LL580;} else{ goto _LL581;} _LL581: goto _LL582; _LL580:
return Cyc_Absyn_tvar_cmp( _temp589, _temp585) == 0; _LL582: return 0; _LL578:;}
_LL559: for( 0; _temp566 != 0; _temp566=({ struct Cyc_List_List* _temp591=
_temp566; if( _temp591 == 0){ _throw( Null_Exception);} _temp591->tl;})){ if(
Cyc_Tcutil_region_in_effect( constrain, r,( void*)({ struct Cyc_List_List*
_temp592= _temp566; if( _temp592 == 0){ _throw( Null_Exception);} _temp592->hd;}))){
return 1;}} return 0; _LL561: if(* _temp575 !=( void*) Cyc_Absyn_EffKind){((
void(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp593=( char*)"effect evar has wrong kind"; struct _tagged_string
_temp594; _temp594.curr= _temp593; _temp594.base= _temp593; _temp594.last_plus_one=
_temp593 + 27; _temp594;}));} if( ! constrain){ return 0;}{ void* ev= Cyc_Absyn_new_evar((
void*) Cyc_Absyn_EffKind); struct Cyc_Absyn_JoinEff_struct* new_typ=({ struct
Cyc_Absyn_JoinEff_struct* _temp596=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp596[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp597; _temp597.tag= Cyc_Absyn_JoinEff; _temp597.f1=({ struct Cyc_List_List*
_temp598=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp598->hd=( void*) ev; _temp598->tl=({ struct Cyc_List_List* _temp599=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp599->hd=(
void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp600=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp600[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp601; _temp601.tag= Cyc_Absyn_AccessEff; _temp601.f1=(
void*) r; _temp601;}); _temp600;})); _temp599->tl= 0; _temp599;}); _temp598;});
_temp597;}); _temp596;});* _temp572=({ struct Cyc_Core_Opt* _temp595=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp595->v=( void*)((
void*) new_typ); _temp595;}); return 1;} _LL563: return 0; _LL555:;}} static int
Cyc_Tcutil_variable_in_effect( int constrain, struct Cyc_Absyn_Tvar* v, void* e){
e= Cyc_Tcutil_compress( e);{ void* _temp602= e; struct Cyc_Absyn_Tvar* _temp612;
struct Cyc_List_List* _temp614; int _temp616; struct Cyc_Core_Opt* _temp618;
struct Cyc_Core_Opt** _temp620; void* _temp621; void** _temp623; _LL604: if((
unsigned int) _temp602 > 4u?*(( int*) _temp602) == Cyc_Absyn_VarType: 0){ _LL613:
_temp612=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*) _temp602)->f1;
goto _LL605;} else{ goto _LL606;} _LL606: if(( unsigned int) _temp602 > 4u?*((
int*) _temp602) == Cyc_Absyn_JoinEff: 0){ _LL615: _temp614=( struct Cyc_List_List*)((
struct Cyc_Absyn_JoinEff_struct*) _temp602)->f1; goto _LL607;} else{ goto _LL608;}
_LL608: if(( unsigned int) _temp602 > 4u?*(( int*) _temp602) == Cyc_Absyn_Evar:
0){ _LL622: _temp621=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp602)->f1;
_temp623=&(( struct Cyc_Absyn_Evar_struct*) _temp602)->f1; goto _LL619; _LL619:
_temp618=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp602)->f2;
_temp620=&(( struct Cyc_Absyn_Evar_struct*) _temp602)->f2; goto _LL617; _LL617:
_temp616=( int)(( struct Cyc_Absyn_Evar_struct*) _temp602)->f3; goto _LL609;}
else{ goto _LL610;} _LL610: goto _LL611; _LL605: return Cyc_Absyn_tvar_cmp( v,
_temp612) == 0; _LL607: for( 0; _temp614 != 0; _temp614=({ struct Cyc_List_List*
_temp624= _temp614; if( _temp624 == 0){ _throw( Null_Exception);} _temp624->tl;})){
if( Cyc_Tcutil_variable_in_effect( constrain, v,( void*)({ struct Cyc_List_List*
_temp625= _temp614; if( _temp625 == 0){ _throw( Null_Exception);} _temp625->hd;}))){
return 1;}} return 0; _LL609: if(* _temp623 !=( void*) Cyc_Absyn_EffKind){((
void(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp626=( char*)"effect evar has wrong kind"; struct _tagged_string
_temp627; _temp627.curr= _temp626; _temp627.base= _temp626; _temp627.last_plus_one=
_temp626 + 27; _temp627;}));}{ void* ev= Cyc_Absyn_new_evar(( void*) Cyc_Absyn_EffKind);
struct Cyc_Absyn_JoinEff_struct* new_typ=({ struct Cyc_Absyn_JoinEff_struct*
_temp629=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp629[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp630; _temp630.tag= Cyc_Absyn_JoinEff;
_temp630.f1=({ struct Cyc_List_List* _temp631=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp631->hd=( void*) ev; _temp631->tl=({ struct
Cyc_List_List* _temp632=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp632->hd=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp633=(
struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp633[ 0]=({ struct Cyc_Absyn_VarType_struct _temp634; _temp634.tag= Cyc_Absyn_VarType;
_temp634.f1= v; _temp634;}); _temp633;})); _temp632->tl= 0; _temp632;});
_temp631;}); _temp630;}); _temp629;});* _temp620=({ struct Cyc_Core_Opt*
_temp628=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp628->v=( void*)(( void*) new_typ); _temp628;}); return 1;} _LL611: return 0;
_LL603:;}} static int Cyc_Tcutil_evar_in_effect( void* evar, void* e){ e= Cyc_Tcutil_compress(
e);{ void* _temp635= e; struct Cyc_List_List* _temp643; int _temp645; struct Cyc_Core_Opt*
_temp647; void* _temp649; _LL637: if(( unsigned int) _temp635 > 4u?*(( int*)
_temp635) == Cyc_Absyn_JoinEff: 0){ _LL644: _temp643=( struct Cyc_List_List*)((
struct Cyc_Absyn_JoinEff_struct*) _temp635)->f1; goto _LL638;} else{ goto _LL639;}
_LL639: if(( unsigned int) _temp635 > 4u?*(( int*) _temp635) == Cyc_Absyn_Evar:
0){ _LL650: _temp649=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp635)->f1;
goto _LL648; _LL648: _temp647=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp635)->f2; goto _LL646; _LL646: _temp645=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp635)->f3; goto _LL640;} else{ goto _LL641;} _LL641: goto _LL642; _LL638:
for( 0; _temp643 != 0; _temp643=({ struct Cyc_List_List* _temp651= _temp643; if(
_temp651 == 0){ _throw( Null_Exception);} _temp651->tl;})){ if( Cyc_Tcutil_evar_in_effect(
evar,( void*)({ struct Cyc_List_List* _temp652= _temp643; if( _temp652 == 0){
_throw( Null_Exception);} _temp652->hd;}))){ return 1;}} return 0; _LL640:
return evar == e; _LL642: return 0; _LL636:;}} int Cyc_Tcutil_subset_effect( int
set_to_empty, void* e1, void* e2){{ struct _tuple8* _temp653= Cyc_Tcutil_get_effect_evar(
e2); struct _tuple8 _temp659; struct Cyc_List_List* _temp660; void* _temp662;
_LL655: if( _temp653 == 0){ goto _LL657;} else{ _temp659=* _temp653; _LL663:
_temp662= _temp659.f1; goto _LL661; _LL661: _temp660= _temp659.f2; goto _LL656;}
_LL657: goto _LL658; _LL656: { void* ev= Cyc_Absyn_new_evar(( void*) Cyc_Absyn_EffKind);
struct Cyc_Absyn_JoinEff_struct* new_typ=({ struct Cyc_Absyn_JoinEff_struct*
_temp664=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp664[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp665; _temp665.tag= Cyc_Absyn_JoinEff;
_temp665.f1=({ struct Cyc_List_List* _temp666=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp666->hd=( void*) ev; _temp666->tl=({ struct
Cyc_List_List* _temp667=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp667->hd=( void*) e1; _temp667->tl= 0; _temp667;}); _temp666;}); _temp665;});
_temp664;}); return Cyc_Tcutil_unify( _temp662,( void*) new_typ);} _LL658: goto
_LL654; _LL654:;}{ void* _temp668= Cyc_Tcutil_compress( e1); struct Cyc_List_List*
_temp680; void* _temp682; struct Cyc_Absyn_Tvar* _temp684; int _temp686; struct
Cyc_Core_Opt* _temp688; struct Cyc_Core_Opt** _temp690; void* _temp691; _LL670:
if(( unsigned int) _temp668 > 4u?*(( int*) _temp668) == Cyc_Absyn_JoinEff: 0){
_LL681: _temp680=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*)
_temp668)->f1; goto _LL671;} else{ goto _LL672;} _LL672: if(( unsigned int)
_temp668 > 4u?*(( int*) _temp668) == Cyc_Absyn_AccessEff: 0){ _LL683: _temp682=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp668)->f1; goto _LL673;} else{
goto _LL674;} _LL674: if(( unsigned int) _temp668 > 4u?*(( int*) _temp668) ==
Cyc_Absyn_VarType: 0){ _LL685: _temp684=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp668)->f1; goto _LL675;} else{ goto _LL676;} _LL676: if(( unsigned int)
_temp668 > 4u?*(( int*) _temp668) == Cyc_Absyn_Evar: 0){ _LL692: _temp691=( void*)((
struct Cyc_Absyn_Evar_struct*) _temp668)->f1; goto _LL689; _LL689: _temp688=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp668)->f2; _temp690=&((
struct Cyc_Absyn_Evar_struct*) _temp668)->f2; goto _LL687; _LL687: _temp686=(
int)(( struct Cyc_Absyn_Evar_struct*) _temp668)->f3; goto _LL677;} else{ goto
_LL678;} _LL678: goto _LL679; _LL671: for( 0; _temp680 != 0; _temp680=({ struct
Cyc_List_List* _temp693= _temp680; if( _temp693 == 0){ _throw( Null_Exception);}
_temp693->tl;})){ if( ! Cyc_Tcutil_subset_effect( set_to_empty,( void*)({ struct
Cyc_List_List* _temp694= _temp680; if( _temp694 == 0){ _throw( Null_Exception);}
_temp694->hd;}), e2)){ return 0;}} return 1; _LL673: return( Cyc_Tcutil_region_in_effect(
0, _temp682, e2)? 1: Cyc_Tcutil_region_in_effect( 1, _temp682, e2))? 1: Cyc_Tcutil_unify(
_temp682,( void*) Cyc_Absyn_HeapRgn); _LL675: return Cyc_Tcutil_variable_in_effect(
0, _temp684, e2)? 1: Cyc_Tcutil_variable_in_effect( 1, _temp684, e2); _LL677:
if( ! Cyc_Tcutil_evar_in_effect( e1, e2)){ if( set_to_empty){* _temp690=({
struct Cyc_Core_Opt* _temp695=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp695->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp696=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp696[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp697; _temp697.tag= Cyc_Absyn_JoinEff;
_temp697.f1= 0; _temp697;}); _temp696;})); _temp695;});} else{* _temp690=({
struct Cyc_Core_Opt* _temp698=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp698->v=( void*) e2; _temp698;});}} return 1; _LL679: return((
int(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({ struct _tagged_string
_temp699= Cyc_Absynpp_typ2string( e1); xprintf("subset_effect: bad effect: %.*s",
_temp699.last_plus_one - _temp699.curr, _temp699.curr);})); _LL669:;}} struct
_tuple9{ struct _tuple8* f1; struct _tuple8* f2; } ; static int Cyc_Tcutil_unify_effect(
void* e1, void* e2){ e1= Cyc_Tcutil_compress( e1); e2= Cyc_Tcutil_compress( e2);
Cyc_Tcutil_normalize_effect( e1); Cyc_Tcutil_normalize_effect( e2);{ struct
_tuple9 _temp701=({ struct _tuple9 _temp700; _temp700.f1= Cyc_Tcutil_get_effect_evar(
e1); _temp700.f2= Cyc_Tcutil_get_effect_evar( e2); _temp700;}); struct _tuple8*
_temp707; struct _tuple8 _temp709; struct Cyc_List_List* _temp710; void*
_temp712; struct _tuple8* _temp714; struct _tuple8 _temp716; struct Cyc_List_List*
_temp717; void* _temp719; _LL703: _LL715: _temp714= _temp701.f1; if( _temp714 ==
0){ goto _LL705;} else{ _temp716=* _temp714; _LL720: _temp719= _temp716.f1; goto
_LL718; _LL718: _temp717= _temp716.f2; goto _LL708;} _LL708: _temp707= _temp701.f2;
if( _temp707 == 0){ goto _LL705;} else{ _temp709=* _temp707; _LL713: _temp712=
_temp709.f1; goto _LL711; _LL711: _temp710= _temp709.f2; goto _LL704;} _LL705:
goto _LL706; _LL704: { void* ev= Cyc_Absyn_new_evar(( void*) Cyc_Absyn_EffKind);
struct Cyc_Absyn_JoinEff_struct* new_typ=({ struct Cyc_Absyn_JoinEff_struct*
_temp721=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp721[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp722; _temp722.tag= Cyc_Absyn_JoinEff;
_temp722.f1=({ struct Cyc_List_List* _temp723=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp723->hd=( void*) ev; _temp723->tl=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp717, _temp710); _temp723;}); _temp722;}); _temp721;}); return Cyc_Tcutil_unify(
_temp719,( void*) new_typ)? Cyc_Tcutil_unify( _temp712,( void*) new_typ): 0;}
_LL706: return Cyc_Tcutil_subset_effect( 0, e1, e2)? Cyc_Tcutil_subset_effect( 0,
e2, e1): 0; _LL702:;}} struct _tuple10{ struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt*
f2; } ; void Cyc_Tcutil_unify_it( void* t1, void* t2){ t1= Cyc_Tcutil_compress(
t1); t2= Cyc_Tcutil_compress( t2); if( t1 == t2){ return;}{ void* _temp724= t1;
int _temp730; struct Cyc_Core_Opt* _temp732; struct Cyc_Core_Opt** _temp734;
void* _temp735; _LL726: if(( unsigned int) _temp724 > 4u?*(( int*) _temp724) ==
Cyc_Absyn_Evar: 0){ _LL736: _temp735=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp724)->f1; goto _LL733; _LL733: _temp732=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Evar_struct*) _temp724)->f2; _temp734=&(( struct Cyc_Absyn_Evar_struct*)
_temp724)->f2; goto _LL731; _LL731: _temp730=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp724)->f3; goto _LL727;} else{ goto _LL728;} _LL728: goto _LL729; _LL727:
Cyc_Tcutil_occurs( t1, t2);{ void* kind2= Cyc_Tcutil_typ_kind( t2); if( Cyc_Tcutil_kind_leq(
kind2, _temp735)){* _temp734=({ struct Cyc_Core_Opt* _temp737=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp737->v=( void*) t2; _temp737;});
return;} else{{ void* _temp738= t2; int _temp746; struct Cyc_Core_Opt* _temp748;
struct Cyc_Core_Opt** _temp750; void* _temp751; struct Cyc_Absyn_PtrInfo
_temp754; _LL740: if(( unsigned int) _temp738 > 4u?*(( int*) _temp738) == Cyc_Absyn_Evar:
0){ _LL752: _temp751=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp738)->f1;
goto _LL749; _LL749: _temp748=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp738)->f2; _temp750=&(( struct Cyc_Absyn_Evar_struct*) _temp738)->f2; goto
_LL747; _LL747: _temp746=( int)(( struct Cyc_Absyn_Evar_struct*) _temp738)->f3;
goto _LL741;} else{ goto _LL742;} _LL742: if(( unsigned int) _temp738 > 4u?*((
int*) _temp738) == Cyc_Absyn_PointerType: 0){ _LL755: _temp754=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp738)->f1; goto _LL753;} else{ goto
_LL744;} _LL753: if( _temp735 ==( void*) Cyc_Absyn_BoxKind){ goto _LL743;} else{
goto _LL744;} _LL744: goto _LL745; _LL741: if( Cyc_Tcutil_kind_leq( _temp735,
kind2)){* _temp750=({ struct Cyc_Core_Opt* _temp756=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp756->v=( void*) t1; _temp756;});
return;} goto _LL739; _LL743: { struct Cyc_Absyn_Conref* c=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp754.bounds);{ void*
_temp757=( void*) c->v; _LL759: if( _temp757 ==( void*) Cyc_Absyn_No_constr){
goto _LL760;} else{ goto _LL761;} _LL761: goto _LL762; _LL760:( void*)( c->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp763=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp763[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp764; _temp764.tag= Cyc_Absyn_Eq_constr; _temp764.f1=(
void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp765=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp765[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp766; _temp766.tag= Cyc_Absyn_Upper_b; _temp766.f1= Cyc_Absyn_signed_int_exp(
1, 0); _temp766;}); _temp765;})); _temp764;}); _temp763;})));* _temp734=({
struct Cyc_Core_Opt* _temp767=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp767->v=( void*) t2; _temp767;}); return; _LL762: goto
_LL758; _LL758:;} goto _LL739;} _LL745: goto _LL739; _LL739:;}( void) _throw((
void*) Cyc_Tcutil_Unify);}} _LL729: goto _LL725; _LL725:;}{ struct _tuple7
_temp769=({ struct _tuple7 _temp768; _temp768.f1= t2; _temp768.f2= t1; _temp768;});
void* _temp819; void* _temp821; int _temp823; struct Cyc_Core_Opt* _temp825;
void* _temp827; void* _temp829; void* _temp831; void* _temp833; struct Cyc_Absyn_Tvar*
_temp835; void* _temp837; struct Cyc_Absyn_Tvar* _temp839; void* _temp841;
struct Cyc_Absyn_Structdecl** _temp843; struct Cyc_List_List* _temp845; struct
_tuple1* _temp847; void* _temp849; struct Cyc_Absyn_Structdecl** _temp851;
struct Cyc_List_List* _temp853; struct _tuple1* _temp855; void* _temp857; struct
Cyc_Absyn_Enumdecl* _temp859; struct _tuple1* _temp861; void* _temp863; struct
Cyc_Absyn_Enumdecl* _temp865; struct _tuple1* _temp867; void* _temp869; struct
Cyc_Absyn_Uniondecl** _temp871; struct Cyc_List_List* _temp873; struct _tuple1*
_temp875; void* _temp877; struct Cyc_Absyn_Uniondecl** _temp879; struct Cyc_List_List*
_temp881; struct _tuple1* _temp883; void* _temp885; struct Cyc_Absyn_TunionInfo
_temp887; void* _temp889; struct Cyc_List_List* _temp891; void* _temp893; struct
Cyc_Absyn_Tuniondecl* _temp895; void* _temp897; struct Cyc_Absyn_TunionInfo
_temp899; void* _temp901; struct Cyc_List_List* _temp903; void* _temp905; struct
Cyc_Absyn_Tuniondecl* _temp907; void* _temp909; struct Cyc_Absyn_TunionFieldInfo
_temp911; struct Cyc_List_List* _temp913; void* _temp915; struct Cyc_Absyn_Tunionfield*
_temp917; struct Cyc_Absyn_Tuniondecl* _temp919; void* _temp921; struct Cyc_Absyn_TunionFieldInfo
_temp923; struct Cyc_List_List* _temp925; void* _temp927; struct Cyc_Absyn_Tunionfield*
_temp929; struct Cyc_Absyn_Tuniondecl* _temp931; void* _temp933; struct Cyc_Absyn_PtrInfo
_temp935; struct Cyc_Absyn_Conref* _temp937; struct Cyc_Absyn_Tqual _temp939;
struct Cyc_Absyn_Conref* _temp941; void* _temp943; void* _temp945; void*
_temp947; struct Cyc_Absyn_PtrInfo _temp949; struct Cyc_Absyn_Conref* _temp951;
struct Cyc_Absyn_Tqual _temp953; struct Cyc_Absyn_Conref* _temp955; void*
_temp957; void* _temp959; void* _temp961; void* _temp963; void* _temp965; void*
_temp967; void* _temp969; void* _temp971; void* _temp973; void* _temp975; void*
_temp977; void* _temp979; void* _temp981; struct Cyc_Absyn_Exp* _temp983; struct
Cyc_Absyn_Tqual _temp985; void* _temp987; void* _temp989; struct Cyc_Absyn_Exp*
_temp991; struct Cyc_Absyn_Tqual _temp993; void* _temp995; void* _temp997;
struct Cyc_Absyn_FnInfo _temp999; struct Cyc_List_List* _temp1001; int _temp1003;
struct Cyc_List_List* _temp1005; void* _temp1007; struct Cyc_Core_Opt* _temp1009;
struct Cyc_List_List* _temp1011; void* _temp1013; struct Cyc_Absyn_FnInfo
_temp1015; struct Cyc_List_List* _temp1017; int _temp1019; struct Cyc_List_List*
_temp1021; void* _temp1023; struct Cyc_Core_Opt* _temp1025; struct Cyc_List_List*
_temp1027; void* _temp1029; struct Cyc_List_List* _temp1031; void* _temp1033;
struct Cyc_List_List* _temp1035; void* _temp1037; struct Cyc_List_List*
_temp1039; void* _temp1041; struct Cyc_List_List* _temp1043; void* _temp1045;
struct Cyc_List_List* _temp1047; void* _temp1049; struct Cyc_List_List*
_temp1051; void* _temp1053; void* _temp1055; void* _temp1057; void* _temp1059;
void* _temp1061; void* _temp1063; void* _temp1065; void* _temp1067; struct Cyc_List_List*
_temp1069; void* _temp1071; struct Cyc_List_List* _temp1073; void* _temp1075;
void* _temp1077; void* _temp1079; void* _temp1081; void* _temp1083; void*
_temp1085; void* _temp1087; _LL771: _LL822: _temp821= _temp769.f1; if((
unsigned int) _temp821 > 4u?*(( int*) _temp821) == Cyc_Absyn_Evar: 0){ _LL828:
_temp827=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp821)->f1; goto _LL826;
_LL826: _temp825=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp821)->f2; goto _LL824; _LL824: _temp823=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp821)->f3; goto _LL820;} else{ goto _LL773;} _LL820: _temp819= _temp769.f2;
goto _LL772; _LL773: _LL832: _temp831= _temp769.f1; if( _temp831 ==( void*) Cyc_Absyn_VoidType){
goto _LL830;} else{ goto _LL775;} _LL830: _temp829= _temp769.f2; if( _temp829 ==(
void*) Cyc_Absyn_VoidType){ goto _LL774;} else{ goto _LL775;} _LL775: _LL838:
_temp837= _temp769.f1; if(( unsigned int) _temp837 > 4u?*(( int*) _temp837) ==
Cyc_Absyn_VarType: 0){ _LL840: _temp839=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp837)->f1; goto _LL834;} else{ goto _LL777;} _LL834: _temp833= _temp769.f2;
if(( unsigned int) _temp833 > 4u?*(( int*) _temp833) == Cyc_Absyn_VarType: 0){
_LL836: _temp835=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp833)->f1; goto _LL776;} else{ goto _LL777;} _LL777: _LL850: _temp849=
_temp769.f1; if(( unsigned int) _temp849 > 4u?*(( int*) _temp849) == Cyc_Absyn_StructType:
0){ _LL856: _temp855=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp849)->f1; goto _LL854; _LL854: _temp853=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp849)->f2; goto _LL852; _LL852: _temp851=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp849)->f3;
goto _LL842;} else{ goto _LL779;} _LL842: _temp841= _temp769.f2; if((
unsigned int) _temp841 > 4u?*(( int*) _temp841) == Cyc_Absyn_StructType: 0){
_LL848: _temp847=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp841)->f1; goto _LL846; _LL846: _temp845=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp841)->f2; goto _LL844; _LL844: _temp843=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp841)->f3;
goto _LL778;} else{ goto _LL779;} _LL779: _LL864: _temp863= _temp769.f1; if((
unsigned int) _temp863 > 4u?*(( int*) _temp863) == Cyc_Absyn_EnumType: 0){
_LL868: _temp867=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*)
_temp863)->f1; goto _LL866; _LL866: _temp865=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_EnumType_struct*) _temp863)->f2; goto _LL858;} else{ goto
_LL781;} _LL858: _temp857= _temp769.f2; if(( unsigned int) _temp857 > 4u?*(( int*)
_temp857) == Cyc_Absyn_EnumType: 0){ _LL862: _temp861=( struct _tuple1*)((
struct Cyc_Absyn_EnumType_struct*) _temp857)->f1; goto _LL860; _LL860: _temp859=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*) _temp857)->f2;
goto _LL780;} else{ goto _LL781;} _LL781: _LL878: _temp877= _temp769.f1; if((
unsigned int) _temp877 > 4u?*(( int*) _temp877) == Cyc_Absyn_UnionType: 0){
_LL884: _temp883=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp877)->f1; goto _LL882; _LL882: _temp881=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp877)->f2; goto _LL880; _LL880: _temp879=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp877)->f3;
goto _LL870;} else{ goto _LL783;} _LL870: _temp869= _temp769.f2; if((
unsigned int) _temp869 > 4u?*(( int*) _temp869) == Cyc_Absyn_UnionType: 0){
_LL876: _temp875=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp869)->f1; goto _LL874; _LL874: _temp873=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp869)->f2; goto _LL872; _LL872: _temp871=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp869)->f3;
goto _LL782;} else{ goto _LL783;} _LL783: _LL898: _temp897= _temp769.f1; if((
unsigned int) _temp897 > 4u?*(( int*) _temp897) == Cyc_Absyn_TunionType: 0){
_LL900: _temp899=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp897)->f1; _LL906: _temp905=( void*) _temp899.tunion_info; if(*(( int*)
_temp905) == Cyc_Absyn_KnownTunion){ _LL908: _temp907=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunion_struct*) _temp905)->f1; goto _LL904;} else{ goto
_LL785;} _LL904: _temp903=( struct Cyc_List_List*) _temp899.targs; goto _LL902;
_LL902: _temp901=( void*) _temp899.rgn; goto _LL886;} else{ goto _LL785;} _LL886:
_temp885= _temp769.f2; if(( unsigned int) _temp885 > 4u?*(( int*) _temp885) ==
Cyc_Absyn_TunionType: 0){ _LL888: _temp887=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp885)->f1; _LL894: _temp893=( void*)
_temp887.tunion_info; if(*(( int*) _temp893) == Cyc_Absyn_KnownTunion){ _LL896:
_temp895=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_KnownTunion_struct*)
_temp893)->f1; goto _LL892;} else{ goto _LL785;} _LL892: _temp891=( struct Cyc_List_List*)
_temp887.targs; goto _LL890; _LL890: _temp889=( void*) _temp887.rgn; goto _LL784;}
else{ goto _LL785;} _LL785: _LL922: _temp921= _temp769.f1; if(( unsigned int)
_temp921 > 4u?*(( int*) _temp921) == Cyc_Absyn_TunionFieldType: 0){ _LL924:
_temp923=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp921)->f1; _LL928: _temp927=( void*) _temp923.field_info; if(*(( int*)
_temp927) == Cyc_Absyn_KnownTunionfield){ _LL932: _temp931=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp927)->f1; goto _LL930; _LL930:
_temp929=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp927)->f2; goto _LL926;} else{ goto _LL787;} _LL926: _temp925=( struct Cyc_List_List*)
_temp923.targs; goto _LL910;} else{ goto _LL787;} _LL910: _temp909= _temp769.f2;
if(( unsigned int) _temp909 > 4u?*(( int*) _temp909) == Cyc_Absyn_TunionFieldType:
0){ _LL912: _temp911=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp909)->f1; _LL916: _temp915=( void*) _temp911.field_info; if(*(( int*)
_temp915) == Cyc_Absyn_KnownTunionfield){ _LL920: _temp919=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp915)->f1; goto _LL918; _LL918:
_temp917=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp915)->f2; goto _LL914;} else{ goto _LL787;} _LL914: _temp913=( struct Cyc_List_List*)
_temp911.targs; goto _LL786;} else{ goto _LL787;} _LL787: _LL948: _temp947=
_temp769.f1; if(( unsigned int) _temp947 > 4u?*(( int*) _temp947) == Cyc_Absyn_PointerType:
0){ _LL950: _temp949=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp947)->f1; _LL960: _temp959=( void*) _temp949.elt_typ; goto _LL958; _LL958:
_temp957=( void*) _temp949.rgn_typ; goto _LL956; _LL956: _temp955=( struct Cyc_Absyn_Conref*)
_temp949.nullable; goto _LL954; _LL954: _temp953=( struct Cyc_Absyn_Tqual)
_temp949.tq; goto _LL952; _LL952: _temp951=( struct Cyc_Absyn_Conref*) _temp949.bounds;
goto _LL934;} else{ goto _LL789;} _LL934: _temp933= _temp769.f2; if((
unsigned int) _temp933 > 4u?*(( int*) _temp933) == Cyc_Absyn_PointerType: 0){
_LL936: _temp935=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp933)->f1; _LL946: _temp945=( void*) _temp935.elt_typ; goto _LL944; _LL944:
_temp943=( void*) _temp935.rgn_typ; goto _LL942; _LL942: _temp941=( struct Cyc_Absyn_Conref*)
_temp935.nullable; goto _LL940; _LL940: _temp939=( struct Cyc_Absyn_Tqual)
_temp935.tq; goto _LL938; _LL938: _temp937=( struct Cyc_Absyn_Conref*) _temp935.bounds;
goto _LL788;} else{ goto _LL789;} _LL789: _LL968: _temp967= _temp769.f1; if((
unsigned int) _temp967 > 4u?*(( int*) _temp967) == Cyc_Absyn_IntType: 0){ _LL972:
_temp971=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp967)->f1; goto _LL970;
_LL970: _temp969=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp967)->f2;
goto _LL962;} else{ goto _LL791;} _LL962: _temp961= _temp769.f2; if((
unsigned int) _temp961 > 4u?*(( int*) _temp961) == Cyc_Absyn_IntType: 0){ _LL966:
_temp965=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp961)->f1; goto _LL964;
_LL964: _temp963=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp961)->f2;
goto _LL790;} else{ goto _LL791;} _LL791: _LL976: _temp975= _temp769.f1; if(
_temp975 ==( void*) Cyc_Absyn_FloatType){ goto _LL974;} else{ goto _LL793;}
_LL974: _temp973= _temp769.f2; if( _temp973 ==( void*) Cyc_Absyn_FloatType){
goto _LL792;} else{ goto _LL793;} _LL793: _LL980: _temp979= _temp769.f1; if(
_temp979 ==( void*) Cyc_Absyn_DoubleType){ goto _LL978;} else{ goto _LL795;}
_LL978: _temp977= _temp769.f2; if( _temp977 ==( void*) Cyc_Absyn_DoubleType){
goto _LL794;} else{ goto _LL795;} _LL795: _LL990: _temp989= _temp769.f1; if((
unsigned int) _temp989 > 4u?*(( int*) _temp989) == Cyc_Absyn_ArrayType: 0){
_LL996: _temp995=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp989)->f1;
goto _LL994; _LL994: _temp993=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp989)->f2; goto _LL992; _LL992: _temp991=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp989)->f3; goto _LL982;} else{ goto _LL797;}
_LL982: _temp981= _temp769.f2; if(( unsigned int) _temp981 > 4u?*(( int*)
_temp981) == Cyc_Absyn_ArrayType: 0){ _LL988: _temp987=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp981)->f1; goto _LL986; _LL986: _temp985=( struct Cyc_Absyn_Tqual)(( struct
Cyc_Absyn_ArrayType_struct*) _temp981)->f2; goto _LL984; _LL984: _temp983=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*) _temp981)->f3; goto
_LL796;} else{ goto _LL797;} _LL797: _LL1014: _temp1013= _temp769.f1; if((
unsigned int) _temp1013 > 4u?*(( int*) _temp1013) == Cyc_Absyn_FnType: 0){
_LL1016: _temp1015=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp1013)->f1; _LL1028: _temp1027=( struct Cyc_List_List*) _temp1015.tvars;
goto _LL1026; _LL1026: _temp1025=( struct Cyc_Core_Opt*) _temp1015.effect; goto
_LL1024; _LL1024: _temp1023=( void*) _temp1015.ret_typ; goto _LL1022; _LL1022:
_temp1021=( struct Cyc_List_List*) _temp1015.args; goto _LL1020; _LL1020:
_temp1019=( int) _temp1015.varargs; goto _LL1018; _LL1018: _temp1017=( struct
Cyc_List_List*) _temp1015.attributes; goto _LL998;} else{ goto _LL799;} _LL998:
_temp997= _temp769.f2; if(( unsigned int) _temp997 > 4u?*(( int*) _temp997) ==
Cyc_Absyn_FnType: 0){ _LL1000: _temp999=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp997)->f1; _LL1012: _temp1011=( struct Cyc_List_List*) _temp999.tvars; goto
_LL1010; _LL1010: _temp1009=( struct Cyc_Core_Opt*) _temp999.effect; goto
_LL1008; _LL1008: _temp1007=( void*) _temp999.ret_typ; goto _LL1006; _LL1006:
_temp1005=( struct Cyc_List_List*) _temp999.args; goto _LL1004; _LL1004:
_temp1003=( int) _temp999.varargs; goto _LL1002; _LL1002: _temp1001=( struct Cyc_List_List*)
_temp999.attributes; goto _LL798;} else{ goto _LL799;} _LL799: _LL1034:
_temp1033= _temp769.f1; if(( unsigned int) _temp1033 > 4u?*(( int*) _temp1033)
== Cyc_Absyn_TupleType: 0){ _LL1036: _temp1035=( struct Cyc_List_List*)(( struct
Cyc_Absyn_TupleType_struct*) _temp1033)->f1; goto _LL1030;} else{ goto _LL801;}
_LL1030: _temp1029= _temp769.f2; if(( unsigned int) _temp1029 > 4u?*(( int*)
_temp1029) == Cyc_Absyn_TupleType: 0){ _LL1032: _temp1031=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp1029)->f1; goto _LL800;} else{ goto
_LL801;} _LL801: _LL1042: _temp1041= _temp769.f1; if(( unsigned int) _temp1041 >
4u?*(( int*) _temp1041) == Cyc_Absyn_AnonStructType: 0){ _LL1044: _temp1043=(
struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*) _temp1041)->f1;
goto _LL1038;} else{ goto _LL803;} _LL1038: _temp1037= _temp769.f2; if((
unsigned int) _temp1037 > 4u?*(( int*) _temp1037) == Cyc_Absyn_AnonStructType: 0){
_LL1040: _temp1039=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp1037)->f1; goto _LL802;} else{ goto _LL803;} _LL803: _LL1050: _temp1049=
_temp769.f1; if(( unsigned int) _temp1049 > 4u?*(( int*) _temp1049) == Cyc_Absyn_AnonUnionType:
0){ _LL1052: _temp1051=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp1049)->f1; goto _LL1046;} else{ goto _LL805;} _LL1046: _temp1045= _temp769.f2;
if(( unsigned int) _temp1045 > 4u?*(( int*) _temp1045) == Cyc_Absyn_AnonUnionType:
0){ _LL1048: _temp1047=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp1045)->f1; goto _LL804;} else{ goto _LL805;} _LL805: _LL1056: _temp1055=
_temp769.f1; if( _temp1055 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL1054;} else{
goto _LL807;} _LL1054: _temp1053= _temp769.f2; if( _temp1053 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL806;} else{ goto _LL807;} _LL807: _LL1062: _temp1061= _temp769.f1; if((
unsigned int) _temp1061 > 4u?*(( int*) _temp1061) == Cyc_Absyn_RgnHandleType: 0){
_LL1064: _temp1063=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp1061)->f1;
goto _LL1058;} else{ goto _LL809;} _LL1058: _temp1057= _temp769.f2; if((
unsigned int) _temp1057 > 4u?*(( int*) _temp1057) == Cyc_Absyn_RgnHandleType: 0){
_LL1060: _temp1059=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp1057)->f1;
goto _LL808;} else{ goto _LL809;} _LL809: _LL1068: _temp1067= _temp769.f1; if((
unsigned int) _temp1067 > 4u?*(( int*) _temp1067) == Cyc_Absyn_JoinEff: 0){
_LL1070: _temp1069=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*)
_temp1067)->f1; goto _LL1066;} else{ goto _LL811;} _LL1066: _temp1065= _temp769.f2;
goto _LL810; _LL811: _LL1076: _temp1075= _temp769.f1; goto _LL1072; _LL1072:
_temp1071= _temp769.f2; if(( unsigned int) _temp1071 > 4u?*(( int*) _temp1071)
== Cyc_Absyn_JoinEff: 0){ _LL1074: _temp1073=( struct Cyc_List_List*)(( struct
Cyc_Absyn_JoinEff_struct*) _temp1071)->f1; goto _LL812;} else{ goto _LL813;}
_LL813: _LL1080: _temp1079= _temp769.f1; if(( unsigned int) _temp1079 > 4u?*((
int*) _temp1079) == Cyc_Absyn_AccessEff: 0){ _LL1082: _temp1081=( void*)((
struct Cyc_Absyn_AccessEff_struct*) _temp1079)->f1; goto _LL1078;} else{ goto
_LL815;} _LL1078: _temp1077= _temp769.f2; goto _LL814; _LL815: _LL1088:
_temp1087= _temp769.f1; goto _LL1084; _LL1084: _temp1083= _temp769.f2; if((
unsigned int) _temp1083 > 4u?*(( int*) _temp1083) == Cyc_Absyn_AccessEff: 0){
_LL1086: _temp1085=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp1083)->f1;
goto _LL816;} else{ goto _LL817;} _LL817: goto _LL818; _LL772: Cyc_Tcutil_unify_it(
t2, t1); return; _LL774: return; _LL776: { struct _tagged_string* x2= _temp839->name;
struct _tagged_string* x1= _temp835->name; void* k2=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp839->kind); void* k1=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp835->kind); if( Cyc_String_zstrptrcmp( x2, x1)
== 0){ if( k2 != k1){(( void(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({
struct _tagged_string _temp1089=* x2; struct _tagged_string _temp1090= Cyc_Absynpp_kind2string(
k2); struct _tagged_string _temp1091= Cyc_Absynpp_kind2string( k1); xprintf("same type variable %.*s has kinds %.*s and %.*s",
_temp1089.last_plus_one - _temp1089.curr, _temp1089.curr, _temp1090.last_plus_one
- _temp1090.curr, _temp1090.curr, _temp1091.last_plus_one - _temp1091.curr,
_temp1091.curr);}));} return;} goto _LL770;} _LL778: if((( _temp847 != 0?
_temp855 != 0: 0)? Cyc_Absyn_qvar_cmp(( struct _tuple1*)({ struct _tuple1*
_temp1092= _temp847; if( _temp1092 == 0){ _throw( Null_Exception);} _temp1092;}),(
struct _tuple1*)({ struct _tuple1* _temp1093= _temp855; if( _temp1093 == 0){
_throw( Null_Exception);} _temp1093;})) == 0: 0)? 1:( _temp847 == 0? _temp855 ==
0: 0)){ Cyc_Tcutil_unify_list( _temp845, _temp853); return;} goto _LL770; _LL780:
if( Cyc_Absyn_qvar_cmp( _temp867, _temp861) == 0){ return;} goto _LL770; _LL782:
if((( _temp875 != 0? _temp883 != 0: 0)? Cyc_Absyn_qvar_cmp(( struct _tuple1*)({
struct _tuple1* _temp1094= _temp875; if( _temp1094 == 0){ _throw( Null_Exception);}
_temp1094;}),( struct _tuple1*)({ struct _tuple1* _temp1095= _temp883; if(
_temp1095 == 0){ _throw( Null_Exception);} _temp1095;})) == 0: 0)? 1:( _temp875
== 0? _temp883 == 0: 0)){ Cyc_Tcutil_unify_list( _temp873, _temp881); return;}
goto _LL770; _LL784: if( _temp907 == _temp895? 1: Cyc_Absyn_qvar_cmp( _temp907->name,
_temp895->name) == 0){ Cyc_Tcutil_unify_it( _temp889, _temp901); Cyc_Tcutil_unify_list(
_temp891, _temp903); return;} goto _LL770; _LL786: if(( _temp931 == _temp919? 1:
Cyc_Absyn_qvar_cmp( _temp931->name, _temp919->name) == 0)? _temp929 == _temp917?
1: Cyc_Absyn_qvar_cmp( _temp929->name, _temp917->name) == 0: 0){ Cyc_Tcutil_unify_list(
_temp913, _temp925); return;} goto _LL770; _LL788: Cyc_Tcutil_unify_it( _temp945,
_temp959); Cyc_Tcutil_unify_it( _temp957, _temp943); Cyc_Tcutil_unify_tqual(
_temp939, _temp953);(( void(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref*
x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_it_conrefs)( Cyc_Tcutil_boundscmp,
_temp937, _temp951);{ void* _temp1096=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp937))->v; void*
_temp1102; _LL1098: if(( unsigned int) _temp1096 > 1u?*(( int*) _temp1096) ==
Cyc_Absyn_Eq_constr: 0){ _LL1103: _temp1102=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp1096)->f1; if( _temp1102 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1099;}
else{ goto _LL1100;}} else{ goto _LL1100;} _LL1100: goto _LL1101; _LL1099:
return; _LL1101: goto _LL1097; _LL1097:;}(( void(*)( int(* cmp)( int, int),
struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_it_conrefs)(
Cyc_Core_intcmp, _temp941, _temp955); return; _LL790: if( _temp965 == _temp971?
_temp963 == _temp969: 0){ return;} goto _LL770; _LL792: return; _LL794: return;
_LL796: Cyc_Tcutil_unify_it( _temp987, _temp995); Cyc_Tcutil_unify_tqual(
_temp985, _temp993); if( _temp991 == _temp983){ return;} if( _temp991 == 0? 1:
_temp983 == 0){ goto _LL770;} if( Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1104= _temp991; if( _temp1104 == 0){ _throw(
Null_Exception);} _temp1104;})) == Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1105= _temp983; if( _temp1105 == 0){ _throw(
Null_Exception);} _temp1105;}))){ return;} goto _LL770; _LL798: { int done= 0;{
struct _RegionHandle _temp1106= _new_region(); struct _RegionHandle* rgn=&
_temp1106; _push_region( rgn);{ struct Cyc_List_List* inst= 0; while( _temp1011
!= 0) { if( _temp1027 == 0){ break;} inst=({ struct Cyc_List_List* _temp1107=(
struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp1107->hd=( void*)({ struct _tuple5* _temp1108=( struct _tuple5*)
_region_malloc( rgn, sizeof( struct _tuple5)); _temp1108->f1=( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp1112= _temp1027; if( _temp1112 == 0){ _throw(
Null_Exception);} _temp1112->hd;}); _temp1108->f2=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp1109=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp1109[ 0]=({ struct Cyc_Absyn_VarType_struct _temp1110; _temp1110.tag= Cyc_Absyn_VarType;
_temp1110.f1=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp1111=
_temp1011; if( _temp1111 == 0){ _throw( Null_Exception);} _temp1111->hd;});
_temp1110;}); _temp1109;}); _temp1108;}); _temp1107->tl= inst; _temp1107;});
_temp1011=({ struct Cyc_List_List* _temp1113= _temp1011; if( _temp1113 == 0){
_throw( Null_Exception);} _temp1113->tl;}); _temp1027=({ struct Cyc_List_List*
_temp1114= _temp1027; if( _temp1114 == 0){ _throw( Null_Exception);} _temp1114->tl;});}
if( _temp1027 != 0){ _npop_handler( 0u); goto _LL770;} if( inst != 0){ Cyc_Tcutil_unify_it((
void*)({ struct Cyc_Absyn_FnType_struct* _temp1115=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1115[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1116; _temp1116.tag= Cyc_Absyn_FnType; _temp1116.f1=({ struct Cyc_Absyn_FnInfo
_temp1117; _temp1117.tvars= 0; _temp1117.effect= _temp1009; _temp1117.ret_typ=(
void*) _temp1007; _temp1117.args= _temp1005; _temp1117.varargs= _temp1003;
_temp1117.attributes= _temp1001; _temp1117;}); _temp1116;}); _temp1115;}), Cyc_Tcutil_rsubstitute(
rgn, inst,( void*)({ struct Cyc_Absyn_FnType_struct* _temp1118=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1118[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1119; _temp1119.tag= Cyc_Absyn_FnType; _temp1119.f1=({ struct Cyc_Absyn_FnInfo
_temp1120; _temp1120.tvars= 0; _temp1120.effect= _temp1025; _temp1120.ret_typ=(
void*) _temp1023; _temp1120.args= _temp1021; _temp1120.varargs= _temp1019;
_temp1120.attributes= _temp1017; _temp1120;}); _temp1119;}); _temp1118;})));
done= 1;}}; _pop_region(& _temp1106);} if( done){ return;} Cyc_Tcutil_unify_it(
_temp1007, _temp1023); for( 0; _temp1005 != 0? _temp1021 != 0: 0; _temp1005=({
struct Cyc_List_List* _temp1121= _temp1005; if( _temp1121 == 0){ _throw(
Null_Exception);} _temp1121->tl;}), _temp1021=({ struct Cyc_List_List* _temp1122=
_temp1021; if( _temp1122 == 0){ _throw( Null_Exception);} _temp1122->tl;})){ Cyc_Tcutil_unify_tqual((*((
struct _tuple2*)({ struct Cyc_List_List* _temp1123= _temp1005; if( _temp1123 ==
0){ _throw( Null_Exception);} _temp1123->hd;}))).f2,(*(( struct _tuple2*)({
struct Cyc_List_List* _temp1124= _temp1021; if( _temp1124 == 0){ _throw(
Null_Exception);} _temp1124->hd;}))).f2); Cyc_Tcutil_unify_it((*(( struct
_tuple2*)({ struct Cyc_List_List* _temp1125= _temp1005; if( _temp1125 == 0){
_throw( Null_Exception);} _temp1125->hd;}))).f3,(*(( struct _tuple2*)({ struct
Cyc_List_List* _temp1126= _temp1021; if( _temp1126 == 0){ _throw( Null_Exception);}
_temp1126->hd;}))).f3);} if( _temp1005 != 0? 1: _temp1021 != 0){ goto _LL770;}
if( _temp1003 != _temp1019){ goto _LL770;}{ int bad_effect= 0;{ struct _tuple10
_temp1128=({ struct _tuple10 _temp1127; _temp1127.f1= _temp1009; _temp1127.f2=
_temp1025; _temp1127;}); struct Cyc_Core_Opt* _temp1138; struct Cyc_Core_Opt*
_temp1140; struct Cyc_Core_Opt* _temp1142; struct Cyc_Core_Opt* _temp1144;
struct Cyc_Core_Opt* _temp1146; struct Cyc_Core_Opt* _temp1148; _LL1130: _LL1141:
_temp1140= _temp1128.f1; if( _temp1140 == 0){ goto _LL1139;} else{ goto _LL1132;}
_LL1139: _temp1138= _temp1128.f2; if( _temp1138 == 0){ goto _LL1131;} else{ goto
_LL1132;} _LL1132: _LL1145: _temp1144= _temp1128.f1; if( _temp1144 == 0){ goto
_LL1143;} else{ goto _LL1134;} _LL1143: _temp1142= _temp1128.f2; goto _LL1133;
_LL1134: _LL1149: _temp1148= _temp1128.f1; goto _LL1147; _LL1147: _temp1146=
_temp1128.f2; if( _temp1146 == 0){ goto _LL1135;} else{ goto _LL1136;} _LL1136:
goto _LL1137; _LL1131: goto _LL1129; _LL1133: goto _LL1135; _LL1135: bad_effect=
1; goto _LL1129; _LL1137: bad_effect= ! Cyc_Tcutil_unify_effect(( void*)({
struct Cyc_Core_Opt* _temp1150= _temp1009; if( _temp1150 == 0){ _throw(
Null_Exception);} _temp1150->v;}),( void*)({ struct Cyc_Core_Opt* _temp1151=
_temp1025; if( _temp1151 == 0){ _throw( Null_Exception);} _temp1151->v;})); goto
_LL1129; _LL1129:;} if( bad_effect){ goto _LL770;} if( ! Cyc_Tcutil_same_atts(
_temp1017, _temp1001)){ goto _LL770;} return;}} _LL800: for( 0; _temp1031 != 0?
_temp1035 != 0: 0; _temp1031=({ struct Cyc_List_List* _temp1152= _temp1031; if(
_temp1152 == 0){ _throw( Null_Exception);} _temp1152->tl;}), _temp1035=({ struct
Cyc_List_List* _temp1153= _temp1035; if( _temp1153 == 0){ _throw( Null_Exception);}
_temp1153->tl;})){ Cyc_Tcutil_unify_tqual((*(( struct _tuple4*)({ struct Cyc_List_List*
_temp1154= _temp1031; if( _temp1154 == 0){ _throw( Null_Exception);} _temp1154->hd;}))).f1,(*((
struct _tuple4*)({ struct Cyc_List_List* _temp1155= _temp1035; if( _temp1155 ==
0){ _throw( Null_Exception);} _temp1155->hd;}))).f1); Cyc_Tcutil_unify_it((*((
struct _tuple4*)({ struct Cyc_List_List* _temp1156= _temp1031; if( _temp1156 ==
0){ _throw( Null_Exception);} _temp1156->hd;}))).f2,(*(( struct _tuple4*)({
struct Cyc_List_List* _temp1157= _temp1035; if( _temp1157 == 0){ _throw(
Null_Exception);} _temp1157->hd;}))).f2);} if( _temp1031 == 0? _temp1035 == 0: 0){
return;} goto _LL770; _LL802: _temp1051= _temp1043; _temp1047= _temp1039; goto
_LL804; _LL804: for( 0; _temp1047 != 0? _temp1051 != 0: 0; _temp1047=({ struct
Cyc_List_List* _temp1158= _temp1047; if( _temp1158 == 0){ _throw( Null_Exception);}
_temp1158->tl;}), _temp1051=({ struct Cyc_List_List* _temp1159= _temp1051; if(
_temp1159 == 0){ _throw( Null_Exception);} _temp1159->tl;})){ struct Cyc_Absyn_Structfield*
f1=( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp1163= _temp1047;
if( _temp1163 == 0){ _throw( Null_Exception);} _temp1163->hd;}); struct Cyc_Absyn_Structfield*
f2=( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp1162= _temp1051;
if( _temp1162 == 0){ _throw( Null_Exception);} _temp1162->hd;}); if( Cyc_String_zstrptrcmp(
f1->name, f2->name) != 0){( void) _throw(( void*) Cyc_Tcutil_Unify);} Cyc_Tcutil_unify_tqual(
f1->tq, f2->tq); Cyc_Tcutil_unify_it(( void*) f1->type,( void*) f2->type); if(((
! Cyc_Tcutil_same_atts( f1->attributes, f2->attributes)? 1:( f1->width != 0? f2->width
== 0: 0))? 1:( f2->width != 0? f1->width == 0: 0))? 1:(( f1->width != 0? f2->width
!= 0: 0)? Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt*
_temp1160= f1->width; if( _temp1160 == 0){ _throw( Null_Exception);} _temp1160->v;}))
!= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt*
_temp1161= f2->width; if( _temp1161 == 0){ _throw( Null_Exception);} _temp1161->v;})):
0)){( void) _throw(( void*) Cyc_Tcutil_Unify);}} if( _temp1047 == 0? _temp1051
== 0: 0){ return;} goto _LL770; _LL806: return; _LL808: Cyc_Tcutil_unify_it(
_temp1063, _temp1059); return; _LL810: goto _LL812; _LL812: goto _LL814; _LL814:
goto _LL816; _LL816: if( Cyc_Tcutil_unify_effect( t1, t2)){ return;} else{ goto
_LL770;} _LL818: goto _LL770; _LL770:;}( void) _throw(( void*) Cyc_Tcutil_Unify);}
int Cyc_Tcutil_is_arithmetic_type( void* t){ void* _temp1164= Cyc_Tcutil_compress(
t); void* _temp1176; void* _temp1178; struct Cyc_Absyn_Enumdecl* _temp1180;
struct _tuple1* _temp1182; _LL1166: if(( unsigned int) _temp1164 > 4u?*(( int*)
_temp1164) == Cyc_Absyn_IntType: 0){ _LL1179: _temp1178=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1164)->f1; goto _LL1177; _LL1177: _temp1176=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1164)->f2; goto _LL1167;} else{ goto _LL1168;} _LL1168: if( _temp1164 ==(
void*) Cyc_Absyn_FloatType){ goto _LL1169;} else{ goto _LL1170;} _LL1170: if(
_temp1164 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1171;} else{ goto _LL1172;}
_LL1172: if(( unsigned int) _temp1164 > 4u?*(( int*) _temp1164) == Cyc_Absyn_EnumType:
0){ _LL1183: _temp1182=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*)
_temp1164)->f1; goto _LL1181; _LL1181: _temp1180=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_EnumType_struct*) _temp1164)->f2; goto _LL1173;} else{ goto
_LL1174;} _LL1174: goto _LL1175; _LL1167: return 1; _LL1169: return 1; _LL1171:
return 1; _LL1173: return 1; _LL1175: return 0; _LL1165:;} int Cyc_Tcutil_will_lose_precision(
void* t1, void* t2){ t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{
struct _tuple7 _temp1185=({ struct _tuple7 _temp1184; _temp1184.f1= t1;
_temp1184.f2= t2; _temp1184;}); void* _temp1207; void* _temp1209; void*
_temp1211; void* _temp1213; void* _temp1215; void* _temp1217; void* _temp1219;
void* _temp1221; void* _temp1223; void* _temp1225; void* _temp1227; void*
_temp1229; void* _temp1231; void* _temp1233; void* _temp1235; void* _temp1237;
void* _temp1239; void* _temp1241; void* _temp1243; void* _temp1245; void*
_temp1247; void* _temp1249; void* _temp1251; void* _temp1253; void* _temp1255;
void* _temp1257; void* _temp1259; void* _temp1261; void* _temp1263; void*
_temp1265; void* _temp1267; void* _temp1269; void* _temp1271; void* _temp1273;
void* _temp1275; void* _temp1277; void* _temp1279; void* _temp1281; void*
_temp1283; void* _temp1285; void* _temp1287; void* _temp1289; _LL1187: _LL1210:
_temp1209= _temp1185.f1; if( _temp1209 ==( void*) Cyc_Absyn_DoubleType){ goto
_LL1208;} else{ goto _LL1189;} _LL1208: _temp1207= _temp1185.f2; if( _temp1207
==( void*) Cyc_Absyn_FloatType){ goto _LL1188;} else{ goto _LL1189;} _LL1189:
_LL1218: _temp1217= _temp1185.f1; if( _temp1217 ==( void*) Cyc_Absyn_DoubleType){
goto _LL1212;} else{ goto _LL1191;} _LL1212: _temp1211= _temp1185.f2; if((
unsigned int) _temp1211 > 4u?*(( int*) _temp1211) == Cyc_Absyn_IntType: 0){
_LL1216: _temp1215=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1211)->f1;
goto _LL1214; _LL1214: _temp1213=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1211)->f2; goto _LL1190;} else{ goto _LL1191;} _LL1191: _LL1226: _temp1225=
_temp1185.f1; if( _temp1225 ==( void*) Cyc_Absyn_FloatType){ goto _LL1220;}
else{ goto _LL1193;} _LL1220: _temp1219= _temp1185.f2; if(( unsigned int)
_temp1219 > 4u?*(( int*) _temp1219) == Cyc_Absyn_IntType: 0){ _LL1224: _temp1223=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1219)->f1; goto _LL1222; _LL1222:
_temp1221=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1219)->f2; goto
_LL1192;} else{ goto _LL1193;} _LL1193: _LL1234: _temp1233= _temp1185.f1; if((
unsigned int) _temp1233 > 4u?*(( int*) _temp1233) == Cyc_Absyn_IntType: 0){
_LL1238: _temp1237=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1233)->f1;
goto _LL1236; _LL1236: _temp1235=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1233)->f2; if( _temp1235 ==( void*) Cyc_Absyn_B8){ goto _LL1228;} else{
goto _LL1195;}} else{ goto _LL1195;} _LL1228: _temp1227= _temp1185.f2; if((
unsigned int) _temp1227 > 4u?*(( int*) _temp1227) == Cyc_Absyn_IntType: 0){
_LL1232: _temp1231=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1227)->f1;
goto _LL1230; _LL1230: _temp1229=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1227)->f2; if( _temp1229 ==( void*) Cyc_Absyn_B8){ goto _LL1194;} else{
goto _LL1195;}} else{ goto _LL1195;} _LL1195: _LL1242: _temp1241= _temp1185.f1;
if(( unsigned int) _temp1241 > 4u?*(( int*) _temp1241) == Cyc_Absyn_IntType: 0){
_LL1246: _temp1245=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1241)->f1;
goto _LL1244; _LL1244: _temp1243=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1241)->f2; if( _temp1243 ==( void*) Cyc_Absyn_B8){ goto _LL1240;} else{
goto _LL1197;}} else{ goto _LL1197;} _LL1240: _temp1239= _temp1185.f2; goto
_LL1196; _LL1197: _LL1250: _temp1249= _temp1185.f1; if(( unsigned int) _temp1249
> 4u?*(( int*) _temp1249) == Cyc_Absyn_IntType: 0){ _LL1254: _temp1253=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp1249)->f1; goto _LL1252; _LL1252:
_temp1251=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1249)->f2; if(
_temp1251 ==( void*) Cyc_Absyn_B4){ goto _LL1248;} else{ goto _LL1199;}} else{
goto _LL1199;} _LL1248: _temp1247= _temp1185.f2; if( _temp1247 ==( void*) Cyc_Absyn_FloatType){
goto _LL1198;} else{ goto _LL1199;} _LL1199: _LL1262: _temp1261= _temp1185.f1;
if(( unsigned int) _temp1261 > 4u?*(( int*) _temp1261) == Cyc_Absyn_IntType: 0){
_LL1266: _temp1265=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1261)->f1;
goto _LL1264; _LL1264: _temp1263=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1261)->f2; if( _temp1263 ==( void*) Cyc_Absyn_B4){ goto _LL1256;} else{
goto _LL1201;}} else{ goto _LL1201;} _LL1256: _temp1255= _temp1185.f2; if((
unsigned int) _temp1255 > 4u?*(( int*) _temp1255) == Cyc_Absyn_IntType: 0){
_LL1260: _temp1259=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1255)->f1;
goto _LL1258; _LL1258: _temp1257=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1255)->f2; if( _temp1257 ==( void*) Cyc_Absyn_B2){ goto _LL1200;} else{
goto _LL1201;}} else{ goto _LL1201;} _LL1201: _LL1274: _temp1273= _temp1185.f1;
if(( unsigned int) _temp1273 > 4u?*(( int*) _temp1273) == Cyc_Absyn_IntType: 0){
_LL1278: _temp1277=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1273)->f1;
goto _LL1276; _LL1276: _temp1275=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1273)->f2; if( _temp1275 ==( void*) Cyc_Absyn_B4){ goto _LL1268;} else{
goto _LL1203;}} else{ goto _LL1203;} _LL1268: _temp1267= _temp1185.f2; if((
unsigned int) _temp1267 > 4u?*(( int*) _temp1267) == Cyc_Absyn_IntType: 0){
_LL1272: _temp1271=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1267)->f1;
goto _LL1270; _LL1270: _temp1269=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1267)->f2; if( _temp1269 ==( void*) Cyc_Absyn_B1){ goto _LL1202;} else{
goto _LL1203;}} else{ goto _LL1203;} _LL1203: _LL1286: _temp1285= _temp1185.f1;
if(( unsigned int) _temp1285 > 4u?*(( int*) _temp1285) == Cyc_Absyn_IntType: 0){
_LL1290: _temp1289=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1285)->f1;
goto _LL1288; _LL1288: _temp1287=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1285)->f2; if( _temp1287 ==( void*) Cyc_Absyn_B2){ goto _LL1280;} else{
goto _LL1205;}} else{ goto _LL1205;} _LL1280: _temp1279= _temp1185.f2; if((
unsigned int) _temp1279 > 4u?*(( int*) _temp1279) == Cyc_Absyn_IntType: 0){
_LL1284: _temp1283=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1279)->f1;
goto _LL1282; _LL1282: _temp1281=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1279)->f2; if( _temp1281 ==( void*) Cyc_Absyn_B1){ goto _LL1204;} else{
goto _LL1205;}} else{ goto _LL1205;} _LL1205: goto _LL1206; _LL1188: return 1;
_LL1190: return 1; _LL1192: return 1; _LL1194: return 0; _LL1196: return 1;
_LL1198: return 1; _LL1200: return 1; _LL1202: return 1; _LL1204: return 1;
_LL1206: return 0; _LL1186:;}} int Cyc_Tcutil_coerce_list( struct Cyc_Tcenv_Tenv*
te, void* t, struct Cyc_List_List* es){ struct Cyc_Core_Opt* max_arith_type= 0;{
struct Cyc_List_List* el= es; for( 0; el != 0; el=({ struct Cyc_List_List*
_temp1291= el; if( _temp1291 == 0){ _throw( Null_Exception);} _temp1291->tl;})){
void* t1= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp1295=((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1294= el; if( _temp1294 == 0){
_throw( Null_Exception);} _temp1294->hd;}))->topt; if( _temp1295 == 0){ _throw(
Null_Exception);} _temp1295->v;})); if( Cyc_Tcutil_is_arithmetic_type( t1)){ if(
max_arith_type == 0? 1: Cyc_Tcutil_will_lose_precision( t1,( void*)({ struct Cyc_Core_Opt*
_temp1292= max_arith_type; if( _temp1292 == 0){ _throw( Null_Exception);}
_temp1292->v;}))){ max_arith_type=({ struct Cyc_Core_Opt* _temp1293=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1293->v=( void*) t1; _temp1293;});}}}}
if( max_arith_type != 0){ if( ! Cyc_Tcutil_unify( t,( void*)({ struct Cyc_Core_Opt*
_temp1296= max_arith_type; if( _temp1296 == 0){ _throw( Null_Exception);}
_temp1296->v;}))){ return 0;}}{ struct Cyc_List_List* el= es; for( 0; el != 0;
el=({ struct Cyc_List_List* _temp1297= el; if( _temp1297 == 0){ _throw(
Null_Exception);} _temp1297->tl;})){ if( ! Cyc_Tcutil_coerce_assign( te,( struct
Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1298= el; if( _temp1298 == 0){
_throw( Null_Exception);} _temp1298->hd;}), t)){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1299= el; if( _temp1299 == 0){ _throw( Null_Exception);}
_temp1299->hd;}))->loc,({ struct _tagged_string _temp1302= Cyc_Absynpp_typ2string(
t); struct _tagged_string _temp1303= Cyc_Absynpp_typ2string(( void*)({ struct
Cyc_Core_Opt* _temp1301=(( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1300= el; if( _temp1300 == 0){ _throw( Null_Exception);} _temp1300->hd;}))->topt;
if( _temp1301 == 0){ _throw( Null_Exception);} _temp1301->v;})); xprintf("type mismatch: expecting %.*s but found %.*s",
_temp1302.last_plus_one - _temp1302.curr, _temp1302.curr, _temp1303.last_plus_one
- _temp1303.curr, _temp1303.curr);})); return 0;}}} return 1;} int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( ! Cyc_Tcutil_coerce_sint_typ(
te, e)){ void* _temp1305= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp1304= e->topt; if( _temp1304 == 0){ _throw( Null_Exception);} _temp1304->v;}));
struct Cyc_Absyn_PtrInfo _temp1311; _LL1307: if(( unsigned int) _temp1305 > 4u?*((
int*) _temp1305) == Cyc_Absyn_PointerType: 0){ _LL1312: _temp1311=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1305)->f1; goto _LL1308;} else{ goto
_LL1309;} _LL1309: goto _LL1310; _LL1308: Cyc_Tcutil_unchecked_cast( te, e, Cyc_Absyn_uint_t);
goto _LL1306; _LL1310: return 0; _LL1306:;} return 1;} int Cyc_Tcutil_is_integral_type(
void* t){ void* _temp1313= Cyc_Tcutil_compress( t); void* _temp1321; void*
_temp1323; struct Cyc_Absyn_Enumdecl* _temp1325; struct _tuple1* _temp1327;
_LL1315: if(( unsigned int) _temp1313 > 4u?*(( int*) _temp1313) == Cyc_Absyn_IntType:
0){ _LL1324: _temp1323=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1313)->f1;
goto _LL1322; _LL1322: _temp1321=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1313)->f2; goto _LL1316;} else{ goto _LL1317;} _LL1317: if(( unsigned int)
_temp1313 > 4u?*(( int*) _temp1313) == Cyc_Absyn_EnumType: 0){ _LL1328:
_temp1327=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*) _temp1313)->f1;
goto _LL1326; _LL1326: _temp1325=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*)
_temp1313)->f2; goto _LL1318;} else{ goto _LL1319;} _LL1319: goto _LL1320;
_LL1316: return 1; _LL1318: return 1; _LL1320: return 0; _LL1314:;} int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)({
struct Cyc_Core_Opt* _temp1329= e->topt; if( _temp1329 == 0){ _throw(
Null_Exception);} _temp1329->v;}), Cyc_Absyn_uint_t)){ return 1;} if( Cyc_Tcutil_is_integral_type((
void*)({ struct Cyc_Core_Opt* _temp1330= e->topt; if( _temp1330 == 0){ _throw(
Null_Exception);} _temp1330->v;}))){ if( Cyc_Tcutil_will_lose_precision(( void*)({
struct Cyc_Core_Opt* _temp1331= e->topt; if( _temp1331 == 0){ _throw(
Null_Exception);} _temp1331->v;}), Cyc_Absyn_uint_t)){ Cyc_Tcutil_warn( e->loc,(
struct _tagged_string)({ char* _temp1332=( char*)"integral size mismatch; conversion supplied";
struct _tagged_string _temp1333; _temp1333.curr= _temp1332; _temp1333.base=
_temp1332; _temp1333.last_plus_one= _temp1332 + 44; _temp1333;}));} Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_uint_t); return 1;} return 0;} int Cyc_Tcutil_coerce_sint_typ(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)({
struct Cyc_Core_Opt* _temp1334= e->topt; if( _temp1334 == 0){ _throw(
Null_Exception);} _temp1334->v;}), Cyc_Absyn_sint_t)){ return 1;} if( Cyc_Tcutil_is_integral_type((
void*)({ struct Cyc_Core_Opt* _temp1335= e->topt; if( _temp1335 == 0){ _throw(
Null_Exception);} _temp1335->v;}))){ if( Cyc_Tcutil_will_lose_precision(( void*)({
struct Cyc_Core_Opt* _temp1336= e->topt; if( _temp1336 == 0){ _throw(
Null_Exception);} _temp1336->v;}), Cyc_Absyn_sint_t)){ Cyc_Tcutil_warn( e->loc,(
struct _tagged_string)({ char* _temp1337=( char*)"integral size mismatch; conversion supplied";
struct _tagged_string _temp1338; _temp1338.curr= _temp1337; _temp1338.base=
_temp1337; _temp1338.last_plus_one= _temp1337 + 44; _temp1338;}));} Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_sint_t); return 1;} return 0;} int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* t1, void* t2){
t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple7
_temp1340=({ struct _tuple7 _temp1339; _temp1339.f1= t1; _temp1339.f2= t2;
_temp1339;}); void* _temp1352; struct Cyc_Absyn_PtrInfo _temp1354; void*
_temp1356; struct Cyc_Absyn_PtrInfo _temp1358; void* _temp1360; struct Cyc_Absyn_Exp*
_temp1362; struct Cyc_Absyn_Tqual _temp1364; void* _temp1366; void* _temp1368;
struct Cyc_Absyn_Exp* _temp1370; struct Cyc_Absyn_Tqual _temp1372; void*
_temp1374; void* _temp1376; struct Cyc_Absyn_TunionInfo _temp1378; void*
_temp1380; struct Cyc_List_List* _temp1382; void* _temp1384; struct Cyc_Absyn_Tuniondecl*
_temp1386; void* _temp1388; struct Cyc_Absyn_TunionFieldInfo _temp1390; struct
Cyc_List_List* _temp1392; void* _temp1394; struct Cyc_Absyn_Tunionfield*
_temp1396; struct Cyc_Absyn_Tuniondecl* _temp1398; void* _temp1400; struct Cyc_Absyn_TunionInfo
_temp1402; void* _temp1404; struct Cyc_List_List* _temp1406; void* _temp1408;
struct Cyc_Absyn_Tuniondecl* _temp1410; void* _temp1412; struct Cyc_Absyn_PtrInfo
_temp1414; struct Cyc_Absyn_Conref* _temp1416; struct Cyc_Absyn_Tqual _temp1418;
struct Cyc_Absyn_Conref* _temp1420; void* _temp1422; void* _temp1424; _LL1342:
_LL1357: _temp1356= _temp1340.f1; if(( unsigned int) _temp1356 > 4u?*(( int*)
_temp1356) == Cyc_Absyn_PointerType: 0){ _LL1359: _temp1358=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1356)->f1; goto _LL1353;} else{ goto
_LL1344;} _LL1353: _temp1352= _temp1340.f2; if(( unsigned int) _temp1352 > 4u?*((
int*) _temp1352) == Cyc_Absyn_PointerType: 0){ _LL1355: _temp1354=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1352)->f1; goto _LL1343;} else{ goto
_LL1344;} _LL1344: _LL1369: _temp1368= _temp1340.f1; if(( unsigned int)
_temp1368 > 4u?*(( int*) _temp1368) == Cyc_Absyn_ArrayType: 0){ _LL1375:
_temp1374=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1368)->f1; goto
_LL1373; _LL1373: _temp1372=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1368)->f2; goto _LL1371; _LL1371: _temp1370=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1368)->f3; goto _LL1361;} else{ goto
_LL1346;} _LL1361: _temp1360= _temp1340.f2; if(( unsigned int) _temp1360 > 4u?*((
int*) _temp1360) == Cyc_Absyn_ArrayType: 0){ _LL1367: _temp1366=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1360)->f1; goto _LL1365; _LL1365:
_temp1364=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1360)->f2; goto _LL1363; _LL1363: _temp1362=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1360)->f3; goto _LL1345;} else{ goto
_LL1346;} _LL1346: _LL1389: _temp1388= _temp1340.f1; if(( unsigned int)
_temp1388 > 4u?*(( int*) _temp1388) == Cyc_Absyn_TunionFieldType: 0){ _LL1391:
_temp1390=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp1388)->f1; _LL1395: _temp1394=( void*) _temp1390.field_info; if(*(( int*)
_temp1394) == Cyc_Absyn_KnownTunionfield){ _LL1399: _temp1398=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp1394)->f1; goto _LL1397; _LL1397:
_temp1396=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1394)->f2; goto _LL1393;} else{ goto _LL1348;} _LL1393: _temp1392=( struct
Cyc_List_List*) _temp1390.targs; goto _LL1377;} else{ goto _LL1348;} _LL1377:
_temp1376= _temp1340.f2; if(( unsigned int) _temp1376 > 4u?*(( int*) _temp1376)
== Cyc_Absyn_TunionType: 0){ _LL1379: _temp1378=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp1376)->f1; _LL1385: _temp1384=( void*)
_temp1378.tunion_info; if(*(( int*) _temp1384) == Cyc_Absyn_KnownTunion){
_LL1387: _temp1386=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1384)->f1; goto _LL1383;} else{ goto _LL1348;} _LL1383: _temp1382=( struct
Cyc_List_List*) _temp1378.targs; goto _LL1381; _LL1381: _temp1380=( void*)
_temp1378.rgn; goto _LL1347;} else{ goto _LL1348;} _LL1348: _LL1413: _temp1412=
_temp1340.f1; if(( unsigned int) _temp1412 > 4u?*(( int*) _temp1412) == Cyc_Absyn_PointerType:
0){ _LL1415: _temp1414=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1412)->f1; _LL1425: _temp1424=( void*) _temp1414.elt_typ; goto _LL1423;
_LL1423: _temp1422=( void*) _temp1414.rgn_typ; goto _LL1421; _LL1421: _temp1420=(
struct Cyc_Absyn_Conref*) _temp1414.nullable; goto _LL1419; _LL1419: _temp1418=(
struct Cyc_Absyn_Tqual) _temp1414.tq; goto _LL1417; _LL1417: _temp1416=( struct
Cyc_Absyn_Conref*) _temp1414.bounds; goto _LL1401;} else{ goto _LL1350;} _LL1401:
_temp1400= _temp1340.f2; if(( unsigned int) _temp1400 > 4u?*(( int*) _temp1400)
== Cyc_Absyn_TunionType: 0){ _LL1403: _temp1402=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp1400)->f1; _LL1409: _temp1408=( void*)
_temp1402.tunion_info; if(*(( int*) _temp1408) == Cyc_Absyn_KnownTunion){
_LL1411: _temp1410=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1408)->f1; goto _LL1407;} else{ goto _LL1350;} _LL1407: _temp1406=( struct
Cyc_List_List*) _temp1402.targs; goto _LL1405; _LL1405: _temp1404=( void*)
_temp1402.rgn; goto _LL1349;} else{ goto _LL1350;} _LL1350: goto _LL1351;
_LL1343: { int okay= 1; if( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref*
x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp,
_temp1358.nullable, _temp1354.nullable)){ void* _temp1426=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp1358.nullable))->v;
int _temp1432; _LL1428: if(( unsigned int) _temp1426 > 1u?*(( int*) _temp1426)
== Cyc_Absyn_Eq_constr: 0){ _LL1433: _temp1432=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp1426)->f1; goto _LL1429;} else{ goto _LL1430;} _LL1430: goto _LL1431;
_LL1429: okay= ! _temp1432; goto _LL1427; _LL1431:( void) _throw((( void*(*)(
struct _tagged_string msg)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp1434=( char*)"silent_castable conref not eq"; struct _tagged_string
_temp1435; _temp1435.curr= _temp1434; _temp1435.base= _temp1434; _temp1435.last_plus_one=
_temp1434 + 30; _temp1435;}))); _LL1427:;} if( !(( int(*)( int(* cmp)( void*,
void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)(
Cyc_Tcutil_boundscmp, _temp1358.bounds, _temp1354.bounds)){ struct _tuple7
_temp1437=({ struct _tuple7 _temp1436; _temp1436.f1=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp1358.bounds))->v;
_temp1436.f2=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)( _temp1354.bounds))->v; _temp1436;}); void* _temp1445;
void* _temp1447; void* _temp1449; void* _temp1451; struct Cyc_Absyn_Exp*
_temp1453; void* _temp1455; void* _temp1457; void* _temp1459; void* _temp1461;
_LL1439: _LL1450: _temp1449= _temp1437.f1; if(( unsigned int) _temp1449 > 1u?*((
int*) _temp1449) == Cyc_Absyn_Eq_constr: 0){ _LL1452: _temp1451=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp1449)->f1; if(( unsigned int) _temp1451
> 1u?*(( int*) _temp1451) == Cyc_Absyn_Upper_b: 0){ _LL1454: _temp1453=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp1451)->f1; goto _LL1446;}
else{ goto _LL1441;}} else{ goto _LL1441;} _LL1446: _temp1445= _temp1437.f2; if((
unsigned int) _temp1445 > 1u?*(( int*) _temp1445) == Cyc_Absyn_Eq_constr: 0){
_LL1448: _temp1447=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1445)->f1;
if( _temp1447 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1440;} else{ goto _LL1441;}}
else{ goto _LL1441;} _LL1441: _LL1460: _temp1459= _temp1437.f1; if((
unsigned int) _temp1459 > 1u?*(( int*) _temp1459) == Cyc_Absyn_Eq_constr: 0){
_LL1462: _temp1461=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1459)->f1;
if( _temp1461 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1456;} else{ goto _LL1443;}}
else{ goto _LL1443;} _LL1456: _temp1455= _temp1437.f2; if(( unsigned int)
_temp1455 > 1u?*(( int*) _temp1455) == Cyc_Absyn_Eq_constr: 0){ _LL1458:
_temp1457=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1455)->f1; if(
_temp1457 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1442;} else{ goto _LL1443;}}
else{ goto _LL1443;} _LL1443: goto _LL1444; _LL1440: okay= 1; goto _LL1438;
_LL1442: okay= 1; goto _LL1438; _LL1444: okay= 0; goto _LL1438; _LL1438:;}
return(( okay? Cyc_Tcutil_unify(( void*) _temp1358.elt_typ,( void*) _temp1354.elt_typ):
0)? Cyc_Tcutil_unify(( void*) _temp1358.rgn_typ,( void*) _temp1354.rgn_typ): 0)?
!( _temp1358.tq).q_const? 1:( _temp1354.tq).q_const: 0;} _LL1345: { int okay;
okay=( _temp1370 != 0? _temp1362 != 0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1463= _temp1370; if(
_temp1463 == 0){ _throw( Null_Exception);} _temp1463;})) == Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1464= _temp1362; if(
_temp1464 == 0){ _throw( Null_Exception);} _temp1464;})): 0; return( okay? Cyc_Tcutil_unify(
_temp1374, _temp1366): 0)? ! _temp1372.q_const? 1: _temp1364.q_const: 0;}
_LL1347: if(( _temp1398 == _temp1386? 1: Cyc_Absyn_qvar_cmp( _temp1398->name,
_temp1386->name) == 0)? _temp1396->typs == 0: 0){ for( 0; _temp1392 != 0?
_temp1382 != 0: 0; _temp1392=({ struct Cyc_List_List* _temp1465= _temp1392; if(
_temp1465 == 0){ _throw( Null_Exception);} _temp1465->tl;}), _temp1382=({ struct
Cyc_List_List* _temp1466= _temp1382; if( _temp1466 == 0){ _throw( Null_Exception);}
_temp1466->tl;})){ if( ! Cyc_Tcutil_unify(( void*)({ struct Cyc_List_List*
_temp1467= _temp1392; if( _temp1467 == 0){ _throw( Null_Exception);} _temp1467->hd;}),(
void*)({ struct Cyc_List_List* _temp1468= _temp1382; if( _temp1468 == 0){ _throw(
Null_Exception);} _temp1468->hd;}))){ break;}} if( _temp1392 == 0? _temp1382 ==
0: 0){ return 1;}} return 0; _LL1349:{ void* _temp1469= Cyc_Tcutil_compress(
_temp1424); struct Cyc_Absyn_TunionFieldInfo _temp1475; struct Cyc_List_List*
_temp1477; void* _temp1479; struct Cyc_Absyn_Tunionfield* _temp1481; struct Cyc_Absyn_Tuniondecl*
_temp1483; _LL1471: if(( unsigned int) _temp1469 > 4u?*(( int*) _temp1469) ==
Cyc_Absyn_TunionFieldType: 0){ _LL1476: _temp1475=( struct Cyc_Absyn_TunionFieldInfo)((
struct Cyc_Absyn_TunionFieldType_struct*) _temp1469)->f1; _LL1480: _temp1479=(
void*) _temp1475.field_info; if(*(( int*) _temp1479) == Cyc_Absyn_KnownTunionfield){
_LL1484: _temp1483=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1479)->f1; goto _LL1482; _LL1482: _temp1481=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp1479)->f2; goto _LL1478;} else{
goto _LL1473;} _LL1478: _temp1477=( struct Cyc_List_List*) _temp1475.targs; goto
_LL1472;} else{ goto _LL1473;} _LL1473: goto _LL1474; _LL1472: if( ! Cyc_Tcutil_unify(
_temp1422, _temp1404)? _temp1422 !=( void*) Cyc_Absyn_HeapRgn: 0){ return 0;}
if( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp, _temp1420,(( struct Cyc_Absyn_Conref*(*)(
int x)) Cyc_Absyn_new_conref)( 0))){ return 0;} if( !(( int(*)( int(* cmp)( void*,
void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)(
Cyc_Tcutil_boundscmp, _temp1416,(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
Cyc_Absyn_bounds_one))){ return 0;} if( Cyc_Absyn_qvar_cmp( _temp1410->name,
_temp1483->name) == 0? _temp1481->typs != 0: 0){ int okay= 1; for( 0; _temp1477
!= 0? _temp1406 != 0: 0; _temp1477=({ struct Cyc_List_List* _temp1485= _temp1477;
if( _temp1485 == 0){ _throw( Null_Exception);} _temp1485->tl;}), _temp1406=({
struct Cyc_List_List* _temp1486= _temp1406; if( _temp1486 == 0){ _throw(
Null_Exception);} _temp1486->tl;})){ if( ! Cyc_Tcutil_unify(( void*)({ struct
Cyc_List_List* _temp1487= _temp1477; if( _temp1487 == 0){ _throw( Null_Exception);}
_temp1487->hd;}),( void*)({ struct Cyc_List_List* _temp1488= _temp1406; if(
_temp1488 == 0){ _throw( Null_Exception);} _temp1488->hd;}))){ okay= 0; break;}}
if(( ! okay? 1: _temp1477 != 0)? 1: _temp1406 != 0){ return 0;} return 1;} goto
_LL1470; _LL1474: goto _LL1470; _LL1470:;} return 0; _LL1351: return Cyc_Tcutil_unify(
t1, t2); _LL1341:;}} int Cyc_Tcutil_coerce_arg( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e, void* t2){ void* t1= Cyc_Tcutil_compress(( void*)({
struct Cyc_Core_Opt* _temp1493= e->topt; if( _temp1493 == 0){ _throw(
Null_Exception);} _temp1493->v;})); if( Cyc_Tcutil_unify( t1, t2)){ return 1;}
if( Cyc_Tcutil_is_arithmetic_type( t1)? Cyc_Tcutil_is_arithmetic_type( t2): 0){
if( Cyc_Tcutil_will_lose_precision( t1, t2)){ Cyc_Tcutil_warn( e->loc,( struct
_tagged_string)({ char* _temp1489=( char*)"integral size mismatch; conversion supplied";
struct _tagged_string _temp1490; _temp1490.curr= _temp1489; _temp1490.base=
_temp1489; _temp1490.last_plus_one= _temp1489 + 44; _temp1490;}));} Cyc_Tcutil_unchecked_cast(
te, e, t2); return 1;} else{ if( Cyc_Tcutil_silent_castable( te, e->loc, t1, t2)){
Cyc_Tcutil_unchecked_cast( te, e, t2); return 1;} else{ if( Cyc_Tcutil_castable(
te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te, e, t2); Cyc_Tcutil_warn( e->loc,({
struct _tagged_string _temp1491= Cyc_Absynpp_typ2string( t1); struct
_tagged_string _temp1492= Cyc_Absynpp_typ2string( t2); xprintf("implicit cast from %.*s to %.*s",
_temp1491.last_plus_one - _temp1491.curr, _temp1491.curr, _temp1492.last_plus_one
- _temp1492.curr, _temp1492.curr);})); return 1;} else{ return 0;}}}} int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, void* t){ return Cyc_Tcutil_coerce_arg(
te, e, t);} static int Cyc_Tcutil_any_promote( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Exp* e){ void* old_typ= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp1524= e->topt; if( _temp1524 == 0){ _throw( Null_Exception);} _temp1524->v;}));
void* t;{ void* _temp1494= old_typ; struct Cyc_Absyn_Enumdecl* _temp1506; struct
_tuple1* _temp1508; void* _temp1510; void* _temp1512; _LL1496: if(( unsigned int)
_temp1494 > 4u?*(( int*) _temp1494) == Cyc_Absyn_EnumType: 0){ _LL1509:
_temp1508=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*) _temp1494)->f1;
goto _LL1507; _LL1507: _temp1506=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*)
_temp1494)->f2; goto _LL1497;} else{ goto _LL1498;} _LL1498: if(( unsigned int)
_temp1494 > 4u?*(( int*) _temp1494) == Cyc_Absyn_IntType: 0){ _LL1513: _temp1512=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1494)->f1; goto _LL1511; _LL1511:
_temp1510=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1494)->f2; goto
_LL1499;} else{ goto _LL1500;} _LL1500: if( _temp1494 ==( void*) Cyc_Absyn_FloatType){
goto _LL1501;} else{ goto _LL1502;} _LL1502: if( _temp1494 ==( void*) Cyc_Absyn_DoubleType){
goto _LL1503;} else{ goto _LL1504;} _LL1504: goto _LL1505; _LL1497: t= Cyc_Absyn_sint_t;
goto _LL1495; _LL1499:{ void* _temp1514= _temp1510; _LL1516: if( _temp1514 ==(
void*) Cyc_Absyn_B1){ goto _LL1517;} else{ goto _LL1518;} _LL1518: if( _temp1514
==( void*) Cyc_Absyn_B2){ goto _LL1519;} else{ goto _LL1520;} _LL1520: if(
_temp1514 ==( void*) Cyc_Absyn_B4){ goto _LL1521;} else{ goto _LL1522;} _LL1522:
if( _temp1514 ==( void*) Cyc_Absyn_B8){ goto _LL1523;} else{ goto _LL1515;}
_LL1517: t= Cyc_Absyn_sint_t; goto _LL1515; _LL1519: t= Cyc_Absyn_sint_t; goto
_LL1515; _LL1521: t= _temp1512 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t:
Cyc_Absyn_sint_t; goto _LL1515; _LL1523: t= _temp1512 ==( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_ulong_t: Cyc_Absyn_slong_t; goto _LL1515; _LL1515:;} goto _LL1495;
_LL1501: t= Cyc_Absyn_float_t; goto _LL1495; _LL1503: t= Cyc_Absyn_double_t;
goto _LL1495; _LL1505: return 0; _LL1495:;} Cyc_Tcutil_unchecked_cast( te, e, t);
return 1;} int Cyc_Tcutil_coerce_use( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, void* t2){ void* t1= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp1528= e->topt; if( _temp1528 == 0){ _throw( Null_Exception);} _temp1528->v;}));
if( Cyc_Tcutil_unify( t1, t2)){ return 1;} Cyc_Tcutil_any_promote( te, e); if(
Cyc_Tcutil_unify(( void*)({ struct Cyc_Core_Opt* _temp1525= e->topt; if(
_temp1525 == 0){ _throw( Null_Exception);} _temp1525->v;}), t2)){ return 1;}
else{ if( Cyc_Tcutil_silent_castable( te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast(
te, e, t2); return 1;} else{ if( Cyc_Tcutil_castable( te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast(
te, e, t2); Cyc_Tcutil_warn( e->loc,({ struct _tagged_string _temp1526= Cyc_Absynpp_typ2string(
t1); struct _tagged_string _temp1527= Cyc_Absynpp_typ2string( t2); xprintf("implicit cast from %.*s to %.*s",
_temp1526.last_plus_one - _temp1526.curr, _temp1526.curr, _temp1527.last_plus_one
- _temp1527.curr, _temp1527.curr);})); return 1;} else{ return 0;}}}} int Cyc_Tcutil_coerceable(
void* t){ void* _temp1529= Cyc_Tcutil_compress( t); void* _temp1539; void*
_temp1541; _LL1531: if(( unsigned int) _temp1529 > 4u?*(( int*) _temp1529) ==
Cyc_Absyn_IntType: 0){ _LL1542: _temp1541=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1529)->f1; goto _LL1540; _LL1540: _temp1539=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1529)->f2; goto _LL1532;} else{ goto _LL1533;} _LL1533: if( _temp1529 ==(
void*) Cyc_Absyn_FloatType){ goto _LL1534;} else{ goto _LL1535;} _LL1535: if(
_temp1529 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1536;} else{ goto _LL1537;}
_LL1537: goto _LL1538; _LL1532: return 1; _LL1534: return 1; _LL1536: return 1;
_LL1538: return 0; _LL1530:;} static struct _tuple4* Cyc_Tcutil_flatten_typ_f(
struct Cyc_List_List* inst, struct Cyc_Absyn_Structfield* x){ return({ struct
_tuple4* _temp1543=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1543->f1= x->tq; _temp1543->f2= Cyc_Tcutil_substitute( inst,( void*) x->type);
_temp1543;});} static struct Cyc_List_List* Cyc_Tcutil_flatten_typ( struct Cyc_Tcenv_Tenv*
te, void* t1){ t1= Cyc_Tcutil_compress( t1);{ void* _temp1544= t1; struct Cyc_List_List*
_temp1556; struct Cyc_Absyn_Structdecl** _temp1558; struct Cyc_List_List*
_temp1560; struct _tuple1* _temp1562; struct Cyc_List_List* _temp1564; _LL1546:
if( _temp1544 ==( void*) Cyc_Absyn_VoidType){ goto _LL1547;} else{ goto _LL1548;}
_LL1548: if(( unsigned int) _temp1544 > 4u?*(( int*) _temp1544) == Cyc_Absyn_TupleType:
0){ _LL1557: _temp1556=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp1544)->f1; goto _LL1549;} else{ goto _LL1550;} _LL1550: if(( unsigned int)
_temp1544 > 4u?*(( int*) _temp1544) == Cyc_Absyn_StructType: 0){ _LL1563:
_temp1562=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp1544)->f1;
goto _LL1561; _LL1561: _temp1560=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp1544)->f2; goto _LL1559; _LL1559: _temp1558=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp1544)->f3; goto _LL1551;} else{ goto
_LL1552;} _LL1552: if(( unsigned int) _temp1544 > 4u?*(( int*) _temp1544) == Cyc_Absyn_AnonStructType:
0){ _LL1565: _temp1564=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp1544)->f1; goto _LL1553;} else{ goto _LL1554;} _LL1554: goto _LL1555;
_LL1547: return 0; _LL1549: return _temp1556; _LL1551: if( _temp1558 == 0? 1:
_temp1562 == 0){ return({ struct Cyc_List_List* _temp1566=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1566->hd=( void*)({ struct
_tuple4* _temp1567=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1567->f1= Cyc_Absyn_empty_tqual(); _temp1567->f2= t1; _temp1567;});
_temp1566->tl= 0; _temp1566;});}{ struct Cyc_Absyn_Structdecl* sd=({ struct Cyc_Absyn_Structdecl**
_temp1572= _temp1558; if( _temp1572 == 0){ _throw( Null_Exception);}* _temp1572;});
struct _tuple1 n=({ struct _tuple1* _temp1571= _temp1562; if( _temp1571 == 0){
_throw( Null_Exception);}* _temp1571;}); struct Cyc_List_List* inst=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
sd->tvs, _temp1560); if( sd->fields == 0){ return({ struct Cyc_List_List*
_temp1568=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1568->hd=( void*)({ struct _tuple4* _temp1569=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp1569->f1= Cyc_Absyn_empty_tqual(); _temp1569->f2=
t1; _temp1569;}); _temp1568->tl= 0; _temp1568;});} return(( struct Cyc_List_List*(*)(
struct _tuple4*(* f)( struct Cyc_List_List*, struct Cyc_Absyn_Structfield*),
struct Cyc_List_List* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f,
inst,( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp1570= sd->fields; if(
_temp1570 == 0){ _throw( Null_Exception);} _temp1570->v;}));} _LL1553: return((
struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_List_List*, struct
Cyc_Absyn_Structfield*), struct Cyc_List_List* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, 0, _temp1564); _LL1555: return({
struct Cyc_List_List* _temp1573=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1573->hd=( void*)({ struct _tuple4* _temp1574=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1574->f1= Cyc_Absyn_empty_tqual();
_temp1574->f2= t1; _temp1574;}); _temp1573->tl= 0; _temp1573;}); _LL1545:;}}
static int Cyc_Tcutil_ptrsubtype( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
assume, void* t1, void* t2); static int Cyc_Tcutil_subtype( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* assume, void* t1, void* t2){ if( Cyc_Tcutil_unify( t1,
t2)){ return 1;}{ struct Cyc_List_List* a= assume; for( 0; a != 0; a=({ struct
Cyc_List_List* _temp1575= a; if( _temp1575 == 0){ _throw( Null_Exception);}
_temp1575->tl;})){ if( Cyc_Tcutil_unify( t1,(*(( struct _tuple7*)({ struct Cyc_List_List*
_temp1576= a; if( _temp1576 == 0){ _throw( Null_Exception);} _temp1576->hd;}))).f1)?
Cyc_Tcutil_unify( t2,(*(( struct _tuple7*)({ struct Cyc_List_List* _temp1577= a;
if( _temp1577 == 0){ _throw( Null_Exception);} _temp1577->hd;}))).f2): 0){
return 1;}}} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct
_tuple7 _temp1579=({ struct _tuple7 _temp1578; _temp1578.f1= t1; _temp1578.f2=
t2; _temp1578;}); void* _temp1585; struct Cyc_Absyn_PtrInfo _temp1587; struct
Cyc_Absyn_Conref* _temp1589; struct Cyc_Absyn_Tqual _temp1591; struct Cyc_Absyn_Conref*
_temp1593; void* _temp1595; void* _temp1597; void* _temp1599; struct Cyc_Absyn_PtrInfo
_temp1601; struct Cyc_Absyn_Conref* _temp1603; struct Cyc_Absyn_Tqual _temp1605;
struct Cyc_Absyn_Conref* _temp1607; void* _temp1609; void* _temp1611; _LL1581:
_LL1600: _temp1599= _temp1579.f1; if(( unsigned int) _temp1599 > 4u?*(( int*)
_temp1599) == Cyc_Absyn_PointerType: 0){ _LL1602: _temp1601=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1599)->f1; _LL1612: _temp1611=( void*)
_temp1601.elt_typ; goto _LL1610; _LL1610: _temp1609=( void*) _temp1601.rgn_typ;
goto _LL1608; _LL1608: _temp1607=( struct Cyc_Absyn_Conref*) _temp1601.nullable;
goto _LL1606; _LL1606: _temp1605=( struct Cyc_Absyn_Tqual) _temp1601.tq; goto
_LL1604; _LL1604: _temp1603=( struct Cyc_Absyn_Conref*) _temp1601.bounds; goto
_LL1586;} else{ goto _LL1583;} _LL1586: _temp1585= _temp1579.f2; if((
unsigned int) _temp1585 > 4u?*(( int*) _temp1585) == Cyc_Absyn_PointerType: 0){
_LL1588: _temp1587=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1585)->f1; _LL1598: _temp1597=( void*) _temp1587.elt_typ; goto _LL1596;
_LL1596: _temp1595=( void*) _temp1587.rgn_typ; goto _LL1594; _LL1594: _temp1593=(
struct Cyc_Absyn_Conref*) _temp1587.nullable; goto _LL1592; _LL1592: _temp1591=(
struct Cyc_Absyn_Tqual) _temp1587.tq; goto _LL1590; _LL1590: _temp1589=( struct
Cyc_Absyn_Conref*) _temp1587.bounds; goto _LL1582;} else{ goto _LL1583;} _LL1583:
goto _LL1584; _LL1582: if( _temp1605.q_const? ! _temp1591.q_const: 0){ return 0;}
if(( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp, _temp1607, _temp1593)?(( int(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp1607): 0)? !(( int(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp1593): 0){ return 0;}{
void* _temp1613= _temp1609; _LL1615: if( _temp1613 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL1616;} else{ goto _LL1617;} _LL1617: goto _LL1618; _LL1616: goto _LL1614;
_LL1618: Cyc_Tcutil_unify( _temp1609, _temp1595); goto _LL1614; _LL1614:;} if( !((
int(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y)) Cyc_Tcutil_unify_conrefs)( Cyc_Tcutil_boundscmp, _temp1603, _temp1589)){
struct _tuple7 _temp1620=({ struct _tuple7 _temp1619; _temp1619.f1=(( void*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp1603); _temp1619.f2=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp1589);
_temp1619;}); void* _temp1628; void* _temp1630; struct Cyc_Absyn_Exp* _temp1632;
void* _temp1634; struct Cyc_Absyn_Exp* _temp1636; void* _temp1638; struct Cyc_Absyn_Exp*
_temp1640; _LL1622: _LL1631: _temp1630= _temp1620.f1; if(( unsigned int)
_temp1630 > 1u?*(( int*) _temp1630) == Cyc_Absyn_Upper_b: 0){ _LL1633: _temp1632=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp1630)->f1; goto
_LL1629;} else{ goto _LL1624;} _LL1629: _temp1628= _temp1620.f2; if( _temp1628
==( void*) Cyc_Absyn_Unknown_b){ goto _LL1623;} else{ goto _LL1624;} _LL1624:
_LL1639: _temp1638= _temp1620.f1; if(( unsigned int) _temp1638 > 1u?*(( int*)
_temp1638) == Cyc_Absyn_Upper_b: 0){ _LL1641: _temp1640=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Upper_b_struct*) _temp1638)->f1; goto _LL1635;} else{ goto
_LL1626;} _LL1635: _temp1634= _temp1620.f2; if(( unsigned int) _temp1634 > 1u?*((
int*) _temp1634) == Cyc_Absyn_Upper_b: 0){ _LL1637: _temp1636=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Upper_b_struct*) _temp1634)->f1; goto _LL1625;} else{ goto
_LL1626;} _LL1626: goto _LL1627; _LL1623: goto _LL1621; _LL1625: if( Cyc_Evexp_eval_const_uint_exp(
_temp1640) < Cyc_Evexp_eval_const_uint_exp( _temp1636)){ return 0;} goto _LL1621;
_LL1627: return 0; _LL1621:;} return Cyc_Tcutil_ptrsubtype( te,({ struct Cyc_List_List*
_temp1642=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1642->hd=( void*)({ struct _tuple7* _temp1643=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp1643->f1= t1; _temp1643->f2= t2; _temp1643;});
_temp1642->tl= assume; _temp1642;}), _temp1611, _temp1597); _LL1584: return 0;
_LL1580:;}} static int Cyc_Tcutil_ptrsubtype( struct Cyc_Tcenv_Tenv* te, struct
Cyc_List_List* assume, void* t1, void* t2){ struct Cyc_List_List* tqs1= Cyc_Tcutil_flatten_typ(
te, t1); struct Cyc_List_List* tqs2= Cyc_Tcutil_flatten_typ( te, t2); for( 0;
tqs2 != 0; tqs2=({ struct Cyc_List_List* _temp1644= tqs2; if( _temp1644 == 0){
_throw( Null_Exception);} _temp1644->tl;}), tqs1=({ struct Cyc_List_List*
_temp1645= tqs1; if( _temp1645 == 0){ _throw( Null_Exception);} _temp1645->tl;})){
if( tqs1 == 0){ return 0;}{ struct _tuple4 _temp1649; void* _temp1650; struct
Cyc_Absyn_Tqual _temp1652; struct _tuple4* _temp1647=( struct _tuple4*)({ struct
Cyc_List_List* _temp1646= tqs1; if( _temp1646 == 0){ _throw( Null_Exception);}
_temp1646->hd;}); _temp1649=* _temp1647; _LL1653: _temp1652= _temp1649.f1; goto
_LL1651; _LL1651: _temp1650= _temp1649.f2; goto _LL1648; _LL1648: { struct
_tuple4 _temp1657; void* _temp1658; struct Cyc_Absyn_Tqual _temp1660; struct
_tuple4* _temp1655=( struct _tuple4*)({ struct Cyc_List_List* _temp1654= tqs2;
if( _temp1654 == 0){ _throw( Null_Exception);} _temp1654->hd;}); _temp1657=*
_temp1655; _LL1661: _temp1660= _temp1657.f1; goto _LL1659; _LL1659: _temp1658=
_temp1657.f2; goto _LL1656; _LL1656: if( _temp1660.q_const? Cyc_Tcutil_subtype(
te, assume, _temp1650, _temp1658): 0){ continue;} else{ if( Cyc_Tcutil_unify(
_temp1650, _temp1658)){ continue;} else{ return 0;}}}}} return 1;} int Cyc_Tcutil_castable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* t1, void* t2){
if( Cyc_Tcutil_unify( t1, t2)){ return 1;} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress(
t2);{ void* _temp1662= t2; struct Cyc_Absyn_Uniondecl** _temp1670; struct Cyc_List_List*
_temp1672; struct _tuple1* _temp1674; void* _temp1676; void* _temp1678; _LL1664:
if(( unsigned int) _temp1662 > 4u?*(( int*) _temp1662) == Cyc_Absyn_UnionType: 0){
_LL1675: _temp1674=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp1662)->f1; goto _LL1673; _LL1673: _temp1672=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnionType_struct*) _temp1662)->f2; goto _LL1671; _LL1671:
_temp1670=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp1662)->f3; goto _LL1665;} else{ goto _LL1666;} _LL1666: if(( unsigned int)
_temp1662 > 4u?*(( int*) _temp1662) == Cyc_Absyn_IntType: 0){ _LL1679: _temp1678=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1662)->f1; goto _LL1677; _LL1677:
_temp1676=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1662)->f2; if(
_temp1676 ==( void*) Cyc_Absyn_B4){ goto _LL1667;} else{ goto _LL1668;}} else{
goto _LL1668;} _LL1668: goto _LL1669; _LL1665: if( _temp1670 == 0){ goto _LL1663;}{
struct Cyc_Absyn_Uniondecl* ud=({ struct Cyc_Absyn_Uniondecl** _temp1683=
_temp1670; if( _temp1683 == 0){ _throw( Null_Exception);}* _temp1683;}); if( ud->fields
== 0){ goto _LL1663;}{ struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( ud->tvs,
_temp1672);{ struct Cyc_List_List* fs=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp1682= ud->fields; if( _temp1682 == 0){ _throw( Null_Exception);} _temp1682->v;});
for( 0; fs != 0; fs=({ struct Cyc_List_List* _temp1680= fs; if( _temp1680 == 0){
_throw( Null_Exception);} _temp1680->tl;})){ if( Cyc_Tcutil_unify( t1, Cyc_Tcutil_substitute(
inst,( void*)(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp1681=
fs; if( _temp1681 == 0){ _throw( Null_Exception);} _temp1681->hd;}))->type))){
return 1;}}} goto _LL1663;}} _LL1667: if( Cyc_Tcutil_typ_kind( t2) ==( void*)
Cyc_Absyn_BoxKind){ return 1;} goto _LL1663; _LL1669: goto _LL1663; _LL1663:;}{
void* _temp1684= t1; struct Cyc_Absyn_PtrInfo _temp1700; struct Cyc_Absyn_Conref*
_temp1702; struct Cyc_Absyn_Tqual _temp1704; struct Cyc_Absyn_Conref* _temp1706;
void* _temp1708; void* _temp1710; struct Cyc_Absyn_Exp* _temp1712; struct Cyc_Absyn_Tqual
_temp1714; void* _temp1716; struct Cyc_Absyn_Enumdecl* _temp1718; struct _tuple1*
_temp1720; void* _temp1722; void* _temp1724; _LL1686: if(( unsigned int)
_temp1684 > 4u?*(( int*) _temp1684) == Cyc_Absyn_PointerType: 0){ _LL1701:
_temp1700=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1684)->f1; _LL1711: _temp1710=( void*) _temp1700.elt_typ; goto _LL1709;
_LL1709: _temp1708=( void*) _temp1700.rgn_typ; goto _LL1707; _LL1707: _temp1706=(
struct Cyc_Absyn_Conref*) _temp1700.nullable; goto _LL1705; _LL1705: _temp1704=(
struct Cyc_Absyn_Tqual) _temp1700.tq; goto _LL1703; _LL1703: _temp1702=( struct
Cyc_Absyn_Conref*) _temp1700.bounds; goto _LL1687;} else{ goto _LL1688;} _LL1688:
if(( unsigned int) _temp1684 > 4u?*(( int*) _temp1684) == Cyc_Absyn_ArrayType: 0){
_LL1717: _temp1716=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1684)->f1;
goto _LL1715; _LL1715: _temp1714=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1684)->f2; goto _LL1713; _LL1713: _temp1712=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1684)->f3; goto _LL1689;} else{ goto
_LL1690;} _LL1690: if(( unsigned int) _temp1684 > 4u?*(( int*) _temp1684) == Cyc_Absyn_EnumType:
0){ _LL1721: _temp1720=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*)
_temp1684)->f1; goto _LL1719; _LL1719: _temp1718=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_EnumType_struct*) _temp1684)->f2; goto _LL1691;} else{ goto
_LL1692;} _LL1692: if(( unsigned int) _temp1684 > 4u?*(( int*) _temp1684) == Cyc_Absyn_IntType:
0){ _LL1725: _temp1724=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1684)->f1;
goto _LL1723; _LL1723: _temp1722=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1684)->f2; goto _LL1693;} else{ goto _LL1694;} _LL1694: if( _temp1684 ==(
void*) Cyc_Absyn_FloatType){ goto _LL1695;} else{ goto _LL1696;} _LL1696: if(
_temp1684 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1697;} else{ goto _LL1698;}
_LL1698: goto _LL1699; _LL1687:{ void* _temp1726= t2; struct Cyc_Absyn_PtrInfo
_temp1732; struct Cyc_Absyn_Conref* _temp1734; struct Cyc_Absyn_Tqual _temp1736;
struct Cyc_Absyn_Conref* _temp1738; void* _temp1740; void* _temp1742; _LL1728:
if(( unsigned int) _temp1726 > 4u?*(( int*) _temp1726) == Cyc_Absyn_PointerType:
0){ _LL1733: _temp1732=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1726)->f1; _LL1743: _temp1742=( void*) _temp1732.elt_typ; goto _LL1741;
_LL1741: _temp1740=( void*) _temp1732.rgn_typ; goto _LL1739; _LL1739: _temp1738=(
struct Cyc_Absyn_Conref*) _temp1732.nullable; goto _LL1737; _LL1737: _temp1736=(
struct Cyc_Absyn_Tqual) _temp1732.tq; goto _LL1735; _LL1735: _temp1734=( struct
Cyc_Absyn_Conref*) _temp1732.bounds; goto _LL1729;} else{ goto _LL1730;} _LL1730:
goto _LL1731; _LL1729: { struct Cyc_List_List* assump=({ struct Cyc_List_List*
_temp1759=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1759->hd=( void*)({ struct _tuple7* _temp1760=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp1760->f1= t1; _temp1760->f2= t2; _temp1760;});
_temp1759->tl= 0; _temp1759;}); int ptrsub= Cyc_Tcutil_ptrsubtype( te, assump,
_temp1710, _temp1742)? ! _temp1704.q_const? 1: _temp1736.q_const: 0; int
bounds_ok=(( int(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x,
struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Tcutil_boundscmp,
_temp1702, _temp1734); if( ! bounds_ok){ struct _tuple7 _temp1745=({ struct
_tuple7 _temp1744; _temp1744.f1=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1702); _temp1744.f2=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1734); _temp1744;}); void* _temp1751; struct Cyc_Absyn_Exp* _temp1753; void*
_temp1755; struct Cyc_Absyn_Exp* _temp1757; _LL1747: _LL1756: _temp1755=
_temp1745.f1; if(( unsigned int) _temp1755 > 1u?*(( int*) _temp1755) == Cyc_Absyn_Upper_b:
0){ _LL1758: _temp1757=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp1755)->f1; goto _LL1752;} else{ goto _LL1749;} _LL1752: _temp1751=
_temp1745.f2; if(( unsigned int) _temp1751 > 1u?*(( int*) _temp1751) == Cyc_Absyn_Upper_b:
0){ _LL1754: _temp1753=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp1751)->f1; goto _LL1748;} else{ goto _LL1749;} _LL1749: goto _LL1750;
_LL1748: if( Cyc_Evexp_eval_const_uint_exp( _temp1757) >= Cyc_Evexp_eval_const_uint_exp(
_temp1753)){ bounds_ok= 1;} goto _LL1746; _LL1750: bounds_ok= 1; goto _LL1746;
_LL1746:;} return( bounds_ok? ptrsub: 0)? _temp1708 ==( void*) Cyc_Absyn_HeapRgn?
1: Cyc_Tcutil_unify( _temp1708, _temp1740): 0;} _LL1731: goto _LL1727; _LL1727:;}
return 0; _LL1689:{ void* _temp1761= t2; struct Cyc_Absyn_Exp* _temp1767; struct
Cyc_Absyn_Tqual _temp1769; void* _temp1771; _LL1763: if(( unsigned int)
_temp1761 > 4u?*(( int*) _temp1761) == Cyc_Absyn_ArrayType: 0){ _LL1772:
_temp1771=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1761)->f1; goto
_LL1770; _LL1770: _temp1769=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1761)->f2; goto _LL1768; _LL1768: _temp1767=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1761)->f3; goto _LL1764;} else{ goto
_LL1765;} _LL1765: goto _LL1766; _LL1764: { int okay; okay=( _temp1712 != 0?
_temp1767 != 0: 0)? Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1773= _temp1712; if( _temp1773 == 0){ _throw(
Null_Exception);} _temp1773;})) >= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1774= _temp1767; if( _temp1774 == 0){ _throw(
Null_Exception);} _temp1774;})): 0; return( okay? Cyc_Tcutil_unify( _temp1716,
_temp1771): 0)? ! _temp1714.q_const? 1: _temp1769.q_const: 0;} _LL1766: return 0;
_LL1762:;} return 0; _LL1691: goto _LL1693; _LL1693: goto _LL1695; _LL1695: goto
_LL1697; _LL1697: return Cyc_Tcutil_coerceable( t2); _LL1699: return 0; _LL1685:;}}
void Cyc_Tcutil_unchecked_cast( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, void* t){ if( ! Cyc_Tcutil_unify(( void*)({ struct Cyc_Core_Opt* _temp1775= e->topt;
if( _temp1775 == 0){ _throw( Null_Exception);} _temp1775->v;}), t)){ struct Cyc_Absyn_Exp*
inner= Cyc_Absyn_copy_exp( e);( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Cast_e_struct*
_temp1776=( struct Cyc_Absyn_Cast_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct));
_temp1776[ 0]=({ struct Cyc_Absyn_Cast_e_struct _temp1777; _temp1777.tag= Cyc_Absyn_Cast_e;
_temp1777.f1=( void*) t; _temp1777.f2= inner; _temp1777;}); _temp1776;}))); e->topt=({
struct Cyc_Core_Opt* _temp1778=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1778->v=( void*) t; _temp1778;});}} int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp* e){ void* _temp1780= Cyc_Tcutil_compress(( void*)({ struct
Cyc_Core_Opt* _temp1779= e->topt; if( _temp1779 == 0){ _throw( Null_Exception);}
_temp1779->v;})); struct Cyc_Absyn_Enumdecl* _temp1790; struct _tuple1*
_temp1792; void* _temp1794; void* _temp1796; int _temp1798; struct Cyc_Core_Opt*
_temp1800; void* _temp1802; _LL1782: if(( unsigned int) _temp1780 > 4u?*(( int*)
_temp1780) == Cyc_Absyn_EnumType: 0){ _LL1793: _temp1792=( struct _tuple1*)((
struct Cyc_Absyn_EnumType_struct*) _temp1780)->f1; goto _LL1791; _LL1791:
_temp1790=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*)
_temp1780)->f2; goto _LL1783;} else{ goto _LL1784;} _LL1784: if(( unsigned int)
_temp1780 > 4u?*(( int*) _temp1780) == Cyc_Absyn_IntType: 0){ _LL1797: _temp1796=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1780)->f1; goto _LL1795; _LL1795:
_temp1794=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1780)->f2; goto
_LL1785;} else{ goto _LL1786;} _LL1786: if(( unsigned int) _temp1780 > 4u?*((
int*) _temp1780) == Cyc_Absyn_Evar: 0){ _LL1803: _temp1802=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp1780)->f1; goto _LL1801; _LL1801: _temp1800=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Evar_struct*) _temp1780)->f2; goto _LL1799; _LL1799: _temp1798=(
int)(( struct Cyc_Absyn_Evar_struct*) _temp1780)->f3; goto _LL1787;} else{ goto
_LL1788;} _LL1788: goto _LL1789; _LL1783: goto _LL1785; _LL1785: return 1;
_LL1787: if( Cyc_Tcutil_unify(( void*)({ struct Cyc_Core_Opt* _temp1804= e->topt;
if( _temp1804 == 0){ _throw( Null_Exception);} _temp1804->v;}), Cyc_Absyn_sint_t)){
return 1;} return 0; _LL1789: return 0; _LL1781:;} int Cyc_Tcutil_is_numeric(
struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_is_integral( e)){ return 1;}{ void*
_temp1806= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp1805= e->topt;
if( _temp1805 == 0){ _throw( Null_Exception);} _temp1805->v;})); _LL1808: if(
_temp1806 ==( void*) Cyc_Absyn_FloatType){ goto _LL1809;} else{ goto _LL1810;}
_LL1810: if( _temp1806 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1811;} else{
goto _LL1812;} _LL1812: goto _LL1813; _LL1809: goto _LL1811; _LL1811: return 1;
_LL1813: return 0; _LL1807:;}} int Cyc_Tcutil_is_function_type( void* t){ void*
_temp1814= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo _temp1820; _LL1816:
if(( unsigned int) _temp1814 > 4u?*(( int*) _temp1814) == Cyc_Absyn_FnType: 0){
_LL1821: _temp1820=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp1814)->f1; goto _LL1817;} else{ goto _LL1818;} _LL1818: goto _LL1819;
_LL1817: return 1; _LL1819: return 0; _LL1815:;} void* Cyc_Tcutil_max_arithmetic_type(
void* t1, void* t2){ struct _tuple7 _temp1823=({ struct _tuple7 _temp1822;
_temp1822.f1= t1; _temp1822.f2= t2; _temp1822;}); void* _temp1847; void*
_temp1849; void* _temp1851; void* _temp1853; void* _temp1855; void* _temp1857;
void* _temp1859; void* _temp1861; void* _temp1863; void* _temp1865; void*
_temp1867; void* _temp1869; void* _temp1871; void* _temp1873; void* _temp1875;
void* _temp1877; void* _temp1879; void* _temp1881; void* _temp1883; void*
_temp1885; void* _temp1887; void* _temp1889; void* _temp1891; void* _temp1893;
void* _temp1895; void* _temp1897; void* _temp1899; void* _temp1901; void*
_temp1903; void* _temp1905; void* _temp1907; void* _temp1909; _LL1825: _LL1850:
_temp1849= _temp1823.f1; if( _temp1849 ==( void*) Cyc_Absyn_DoubleType){ goto
_LL1848;} else{ goto _LL1827;} _LL1848: _temp1847= _temp1823.f2; goto _LL1826;
_LL1827: _LL1854: _temp1853= _temp1823.f1; goto _LL1852; _LL1852: _temp1851=
_temp1823.f2; if( _temp1851 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1828;}
else{ goto _LL1829;} _LL1829: _LL1858: _temp1857= _temp1823.f1; if( _temp1857 ==(
void*) Cyc_Absyn_FloatType){ goto _LL1856;} else{ goto _LL1831;} _LL1856:
_temp1855= _temp1823.f2; goto _LL1830; _LL1831: _LL1862: _temp1861= _temp1823.f1;
goto _LL1860; _LL1860: _temp1859= _temp1823.f2; if( _temp1859 ==( void*) Cyc_Absyn_FloatType){
goto _LL1832;} else{ goto _LL1833;} _LL1833: _LL1866: _temp1865= _temp1823.f1;
if(( unsigned int) _temp1865 > 4u?*(( int*) _temp1865) == Cyc_Absyn_IntType: 0){
_LL1870: _temp1869=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1865)->f1;
if( _temp1869 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1868;} else{ goto _LL1835;}
_LL1868: _temp1867=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1865)->f2;
if( _temp1867 ==( void*) Cyc_Absyn_B8){ goto _LL1864;} else{ goto _LL1835;}}
else{ goto _LL1835;} _LL1864: _temp1863= _temp1823.f2; goto _LL1834; _LL1835:
_LL1878: _temp1877= _temp1823.f1; goto _LL1872; _LL1872: _temp1871= _temp1823.f2;
if(( unsigned int) _temp1871 > 4u?*(( int*) _temp1871) == Cyc_Absyn_IntType: 0){
_LL1876: _temp1875=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1871)->f1;
if( _temp1875 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1874;} else{ goto _LL1837;}
_LL1874: _temp1873=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1871)->f2;
if( _temp1873 ==( void*) Cyc_Absyn_B8){ goto _LL1836;} else{ goto _LL1837;}}
else{ goto _LL1837;} _LL1837: _LL1882: _temp1881= _temp1823.f1; if((
unsigned int) _temp1881 > 4u?*(( int*) _temp1881) == Cyc_Absyn_IntType: 0){
_LL1886: _temp1885=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1881)->f1;
if( _temp1885 ==( void*) Cyc_Absyn_Signed){ goto _LL1884;} else{ goto _LL1839;}
_LL1884: _temp1883=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1881)->f2;
if( _temp1883 ==( void*) Cyc_Absyn_B8){ goto _LL1880;} else{ goto _LL1839;}}
else{ goto _LL1839;} _LL1880: _temp1879= _temp1823.f2; goto _LL1838; _LL1839:
_LL1894: _temp1893= _temp1823.f1; goto _LL1888; _LL1888: _temp1887= _temp1823.f2;
if(( unsigned int) _temp1887 > 4u?*(( int*) _temp1887) == Cyc_Absyn_IntType: 0){
_LL1892: _temp1891=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1887)->f1;
if( _temp1891 ==( void*) Cyc_Absyn_Signed){ goto _LL1890;} else{ goto _LL1841;}
_LL1890: _temp1889=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1887)->f2;
if( _temp1889 ==( void*) Cyc_Absyn_B8){ goto _LL1840;} else{ goto _LL1841;}}
else{ goto _LL1841;} _LL1841: _LL1898: _temp1897= _temp1823.f1; if((
unsigned int) _temp1897 > 4u?*(( int*) _temp1897) == Cyc_Absyn_IntType: 0){
_LL1902: _temp1901=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1897)->f1;
if( _temp1901 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1900;} else{ goto _LL1843;}
_LL1900: _temp1899=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1897)->f2;
if( _temp1899 ==( void*) Cyc_Absyn_B4){ goto _LL1896;} else{ goto _LL1843;}}
else{ goto _LL1843;} _LL1896: _temp1895= _temp1823.f2; goto _LL1842; _LL1843:
_LL1910: _temp1909= _temp1823.f1; goto _LL1904; _LL1904: _temp1903= _temp1823.f2;
if(( unsigned int) _temp1903 > 4u?*(( int*) _temp1903) == Cyc_Absyn_IntType: 0){
_LL1908: _temp1907=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1903)->f1;
if( _temp1907 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1906;} else{ goto _LL1845;}
_LL1906: _temp1905=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1903)->f2;
if( _temp1905 ==( void*) Cyc_Absyn_B4){ goto _LL1844;} else{ goto _LL1845;}}
else{ goto _LL1845;} _LL1845: goto _LL1846; _LL1826: goto _LL1828; _LL1828:
return( void*) Cyc_Absyn_DoubleType; _LL1830: goto _LL1832; _LL1832: return(
void*) Cyc_Absyn_FloatType; _LL1834: goto _LL1836; _LL1836: return Cyc_Absyn_ulong_t;
_LL1838: goto _LL1840; _LL1840: return Cyc_Absyn_slong_t; _LL1842: goto _LL1844;
_LL1844: return Cyc_Absyn_uint_t; _LL1846: return Cyc_Absyn_sint_t; _LL1824:;}
void Cyc_Tcutil_check_contains_assign( struct Cyc_Absyn_Exp* e){ void* _temp1911=(
void*) e->r; struct Cyc_Absyn_Exp* _temp1917; struct Cyc_Core_Opt* _temp1919;
struct Cyc_Absyn_Exp* _temp1921; _LL1913: if(*(( int*) _temp1911) == Cyc_Absyn_AssignOp_e){
_LL1922: _temp1921=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1911)->f1; goto _LL1920; _LL1920: _temp1919=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1911)->f2; if( _temp1919 == 0){ goto
_LL1918;} else{ goto _LL1915;} _LL1918: _temp1917=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1911)->f3; goto _LL1914;} else{ goto
_LL1915;} _LL1915: goto _LL1916; _LL1914: Cyc_Tcutil_warn( e->loc,( struct
_tagged_string)({ char* _temp1923=( char*)"assignment in test"; struct
_tagged_string _temp1924; _temp1924.curr= _temp1923; _temp1924.base= _temp1923;
_temp1924.last_plus_one= _temp1923 + 19; _temp1924;})); goto _LL1912; _LL1916:
goto _LL1912; _LL1912:;} static int Cyc_Tcutil_constrain_kinds( struct Cyc_Absyn_Conref*
c1, struct Cyc_Absyn_Conref* c2){ c1=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( c1); c2=(( struct Cyc_Absyn_Conref*(*)( struct
Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( c2); if( c1 == c2){ return 1;}
if(( void*) c1->v ==( void*) Cyc_Absyn_No_constr){( void*)( c1->v=( void*)((
void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp1925=( struct Cyc_Absyn_Forward_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp1925[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp1926; _temp1926.tag= Cyc_Absyn_Forward_constr;
_temp1926.f1= c2; _temp1926;}); _temp1925;}))); return 1;} else{ if(( void*) c2->v
==( void*) Cyc_Absyn_No_constr){( void*)( c2->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp1927=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp1927[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp1928; _temp1928.tag= Cyc_Absyn_Forward_constr; _temp1928.f1= c1; _temp1928;});
_temp1927;}))); return 1;} else{ void* k1=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( c1); void* k2=(( void*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_val)( c2); if( Cyc_Tcutil_kind_leq( k1, k2)){( void*)( c2->v=(
void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp1929=( struct Cyc_Absyn_Forward_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp1929[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp1930; _temp1930.tag= Cyc_Absyn_Forward_constr;
_temp1930.f1= c1; _temp1930;}); _temp1929;}))); return 1;} else{ if( Cyc_Tcutil_kind_leq(
k2, k1)){( void*)( c1->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp1931=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp1931[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp1932; _temp1932.tag= Cyc_Absyn_Forward_constr; _temp1932.f1= c2; _temp1932;});
_temp1931;}))); return 1;} else{ return 0;}}}}} static struct Cyc_List_List* Cyc_Tcutil_add_free_tvar(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar*
tv){ if( tvs == 0){ return({ struct Cyc_List_List* _temp1933=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1933->hd=( void*) tv; _temp1933->tl=
0; _temp1933;});} else{{ struct Cyc_List_List* tvs2= tvs; for( 0; tvs2 != 0;
tvs2=({ struct Cyc_List_List* _temp1934= tvs2; if( _temp1934 == 0){ _throw(
Null_Exception);} _temp1934->tl;})){ if( Cyc_String_zstrptrcmp((( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp1935= tvs2; if( _temp1935 == 0){ _throw(
Null_Exception);} _temp1935->hd;}))->name, tv->name) == 0){ struct Cyc_Absyn_Conref*
k1=(( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp1939= tvs2; if(
_temp1939 == 0){ _throw( Null_Exception);} _temp1939->hd;}))->kind; struct Cyc_Absyn_Conref*
k2= tv->kind; if( ! Cyc_Tcutil_constrain_kinds( k1, k2)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp1936=* tv->name; struct _tagged_string _temp1937= Cyc_Absynpp_ckind2string(
k1); struct _tagged_string _temp1938= Cyc_Absynpp_ckind2string( k2); xprintf("type variable %.*s is used with inconsistent kinds %.*s and %.*s",
_temp1936.last_plus_one - _temp1936.curr, _temp1936.curr, _temp1937.last_plus_one
- _temp1937.curr, _temp1937.curr, _temp1938.last_plus_one - _temp1938.curr,
_temp1938.curr);}));} return tvs;} else{ if(({ struct Cyc_List_List* _temp1940=
tvs2; if( _temp1940 == 0){ _throw( Null_Exception);} _temp1940->tl;}) == 0){({
struct Cyc_List_List* _temp1941= tvs2; if( _temp1941 == 0){ _throw(
Null_Exception);} _temp1941->tl=({ struct Cyc_List_List* _temp1942=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1942->hd=( void*) tv; _temp1942->tl=
0; _temp1942;});}); return tvs;}}}} return(( struct Cyc_List_List*(*)( struct
_tagged_string msg)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp1943=( char*)"Tcutil::add_free_tvar:expecting to have returned."; struct
_tagged_string _temp1944; _temp1944.curr= _temp1943; _temp1944.base= _temp1943;
_temp1944.last_plus_one= _temp1943 + 50; _temp1944;}));}} static struct Cyc_List_List*
Cyc_Tcutil_remove_bound_tvars( struct Cyc_List_List* tvs, struct Cyc_List_List*
btvs){ struct Cyc_List_List* r= 0; for( 0; tvs != 0; tvs=({ struct Cyc_List_List*
_temp1945= tvs; if( _temp1945 == 0){ _throw( Null_Exception);} _temp1945->tl;})){
int present= 0;{ struct Cyc_List_List* b= btvs; for( 0; b != 0; b=({ struct Cyc_List_List*
_temp1946= b; if( _temp1946 == 0){ _throw( Null_Exception);} _temp1946->tl;})){
if( Cyc_String_zstrptrcmp((( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp1947= tvs; if( _temp1947 == 0){ _throw( Null_Exception);} _temp1947->hd;}))->name,((
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp1948= b; if( _temp1948 == 0){
_throw( Null_Exception);} _temp1948->hd;}))->name) == 0){ present= 1; break;}}}
if( ! present){ r=({ struct Cyc_List_List* _temp1949=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1949->hd=( void*)(( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp1950= tvs; if( _temp1950 == 0){ _throw(
Null_Exception);} _temp1950->hd;})); _temp1949->tl= r; _temp1949;});}} r=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( r);
return r;} static int Cyc_Tcutil_tvar_counter= 0; struct Cyc_Absyn_Tvar* Cyc_Tcutil_new_tvar(
void* k){ struct _tagged_string s= xprintf("#%d",( Cyc_Tcutil_tvar_counter ++));
return({ struct Cyc_Absyn_Tvar* _temp1951=( struct Cyc_Absyn_Tvar*) GC_malloc(
sizeof( struct Cyc_Absyn_Tvar)); _temp1951->name=({ struct _tagged_string*
_temp1952=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp1952[ 0]= s; _temp1952;}); _temp1951->kind=(( struct Cyc_Absyn_Conref*(*)(
void* x)) Cyc_Absyn_new_conref)( k); _temp1951;});} int Cyc_Tcutil_is_temp_tvar(
struct Cyc_Absyn_Tvar* t){ struct _tagged_string s=* t->name; return({ struct
_tagged_string _temp1953= s; char* _temp1955= _temp1953.curr + 0; if( _temp1953.base
== 0? 1:( _temp1955 < _temp1953.base? 1: _temp1955 >= _temp1953.last_plus_one)){
_throw( Null_Exception);}* _temp1955;}) =='#';} void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar* t){ if( ! Cyc_Tcutil_is_temp_tvar( t)){ return;}{ struct
_tagged_string s= Cyc_String_strconcat(( struct _tagged_string)({ char*
_temp1960=( char*)"`"; struct _tagged_string _temp1961; _temp1961.curr=
_temp1960; _temp1961.base= _temp1960; _temp1961.last_plus_one= _temp1960 + 2;
_temp1961;}),* t->name);({ struct _tagged_string _temp1956= s; char* _temp1958=
_temp1956.curr + 1; if( _temp1956.base == 0? 1:( _temp1958 < _temp1956.base? 1:
_temp1958 >= _temp1956.last_plus_one)){ _throw( Null_Exception);}* _temp1958='t';});
t->name=({ struct _tagged_string* _temp1959=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp1959[ 0]= s; _temp1959;});}} struct
_tuple11{ struct _tagged_string* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ;
static struct _tuple2* Cyc_Tcutil_fndecl2typ_f( struct _tuple11* x){ return({
struct _tuple2* _temp1962=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp1962->f1=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt* _temp1963=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1963->v=( void*)(*
x).f1; _temp1963;}); _temp1962->f2=(* x).f2; _temp1962->f3=(* x).f3; _temp1962;});}
int Cyc_Tcutil_fntype_att( void* a){ void* _temp1964= a; int _temp1978; _LL1966:
if(( unsigned int) _temp1964 > 15u?*(( int*) _temp1964) == Cyc_Absyn_Regparm_att:
0){ _LL1979: _temp1978=( int)(( struct Cyc_Absyn_Regparm_att_struct*) _temp1964)->f1;
goto _LL1967;} else{ goto _LL1968;} _LL1968: if( _temp1964 ==( void*) Cyc_Absyn_Stdcall_att){
goto _LL1969;} else{ goto _LL1970;} _LL1970: if( _temp1964 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL1971;} else{ goto _LL1972;} _LL1972: if( _temp1964 ==( void*) Cyc_Absyn_Noreturn_att){
goto _LL1973;} else{ goto _LL1974;} _LL1974: if( _temp1964 ==( void*) Cyc_Absyn_Const_att){
goto _LL1975;} else{ goto _LL1976;} _LL1976: goto _LL1977; _LL1967: goto _LL1969;
_LL1969: goto _LL1971; _LL1971: goto _LL1973; _LL1973: goto _LL1975; _LL1975:
return 1; _LL1977: return 0; _LL1965:;} void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl*
fd){ if( fd->cached_typ == 0){ struct Cyc_List_List* fn_type_atts= 0;{ struct
Cyc_List_List* atts= fd->attributes; for( 0; atts != 0; atts=({ struct Cyc_List_List*
_temp1980= atts; if( _temp1980 == 0){ _throw( Null_Exception);} _temp1980->tl;})){
if( Cyc_Tcutil_fntype_att(( void*)({ struct Cyc_List_List* _temp1981= atts; if(
_temp1981 == 0){ _throw( Null_Exception);} _temp1981->hd;}))){ fn_type_atts=({
struct Cyc_List_List* _temp1982=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1982->hd=( void*)(( void*)({ struct Cyc_List_List*
_temp1983= atts; if( _temp1983 == 0){ _throw( Null_Exception);} _temp1983->hd;}));
_temp1982->tl= fn_type_atts; _temp1982;});}}} return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp1984=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp1984[ 0]=({ struct Cyc_Absyn_FnType_struct _temp1985; _temp1985.tag= Cyc_Absyn_FnType;
_temp1985.f1=({ struct Cyc_Absyn_FnInfo _temp1986; _temp1986.tvars= fd->tvs;
_temp1986.effect= fd->effect; _temp1986.ret_typ=( void*)(( void*) fd->ret_type);
_temp1986.args=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct
_tuple11*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_fndecl2typ_f, fd->args);
_temp1986.varargs= fd->varargs; _temp1986.attributes= fn_type_atts; _temp1986;});
_temp1985;}); _temp1984;});} return( void*)({ struct Cyc_Core_Opt* _temp1987= fd->cached_typ;
if( _temp1987 == 0){ _throw( Null_Exception);} _temp1987->v;});} struct _tuple12{
void* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; static void* Cyc_Tcutil_fst_fdarg(
struct _tuple12* t){ return(* t).f1;} void* Cyc_Tcutil_snd_tqt( struct _tuple4*
t){ return(* t).f2;} static struct _tuple4* Cyc_Tcutil_map2_tq( struct _tuple4*
pr, void* t){ return({ struct _tuple4* _temp1988=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp1988->f1=(* pr).f1; _temp1988->f2= t; _temp1988;});}
void Cyc_Tcutil_check_fndecl_valid_type( struct Cyc_Position_Segment* loc,
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Fndecl* fd){ void* t= Cyc_Tcutil_fndecl2typ(
fd); Cyc_Tcutil_check_valid_toplevel_type( loc, te, t);{ void* _temp1989= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_FnInfo _temp1995; struct Cyc_List_List* _temp1997; int
_temp1999; struct Cyc_List_List* _temp2001; void* _temp2003; struct Cyc_Core_Opt*
_temp2005; struct Cyc_List_List* _temp2007; _LL1991: if(( unsigned int)
_temp1989 > 4u?*(( int*) _temp1989) == Cyc_Absyn_FnType: 0){ _LL1996: _temp1995=(
struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*) _temp1989)->f1;
_LL2008: _temp2007=( struct Cyc_List_List*) _temp1995.tvars; goto _LL2006;
_LL2006: _temp2005=( struct Cyc_Core_Opt*) _temp1995.effect; goto _LL2004;
_LL2004: _temp2003=( void*) _temp1995.ret_typ; goto _LL2002; _LL2002: _temp2001=(
struct Cyc_List_List*) _temp1995.args; goto _LL2000; _LL2000: _temp1999=( int)
_temp1995.varargs; goto _LL1998; _LL1998: _temp1997=( struct Cyc_List_List*)
_temp1995.attributes; goto _LL1992;} else{ goto _LL1993;} _LL1993: goto _LL1994;
_LL1992: fd->tvs= _temp2007; fd->effect= _temp2005; goto _LL1990; _LL1994:((
void(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp2009=( char*)"check_fndecl_valid_type: not a FnType"; struct
_tagged_string _temp2010; _temp2010.curr= _temp2009; _temp2010.base= _temp2009;
_temp2010.last_plus_one= _temp2009 + 38; _temp2010;})); return; _LL1990:;}{
struct _RegionHandle _temp2011= _new_region(); struct _RegionHandle* r=&
_temp2011; _push_region( r); Cyc_Tcutil_check_unique_vars((( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tagged_string*(* f)( struct _tuple11*), struct
Cyc_List_List* x)) Cyc_List_rmap)( r,( struct _tagged_string*(*)( struct
_tuple11* t)) Cyc_Tcutil_fst_fdarg, fd->args), loc,( struct _tagged_string)({
char* _temp2012=( char*)"function declaration has repeated parameter"; struct
_tagged_string _temp2013; _temp2013.curr= _temp2012; _temp2013.base= _temp2012;
_temp2013.last_plus_one= _temp2012 + 44; _temp2013;}));; _pop_region(& _temp2011);}
fd->cached_typ=({ struct Cyc_Core_Opt* _temp2014=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2014->v=( void*) t; _temp2014;});}
struct _tuple13{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; } ; struct
_tuple14{ struct _tuple13* f1; void* f2; } ; static struct _tuple14* Cyc_Tcutil_substitute_f1(
struct _RegionHandle* rgn, struct _tuple2* y){ return({ struct _tuple14*
_temp2015=( struct _tuple14*) _region_malloc( rgn, sizeof( struct _tuple14));
_temp2015->f1=({ struct _tuple13* _temp2016=( struct _tuple13*) _region_malloc(
rgn, sizeof( struct _tuple13)); _temp2016->f1=(* y).f1; _temp2016->f2=(* y).f2;
_temp2016;}); _temp2015->f2=(* y).f3; _temp2015;});} static struct _tuple2* Cyc_Tcutil_substitute_f2(
struct _tuple14* w){ void* _temp2019; struct _tuple13* _temp2021; struct
_tuple14 _temp2017=* w; _LL2022: _temp2021= _temp2017.f1; goto _LL2020; _LL2020:
_temp2019= _temp2017.f2; goto _LL2018; _LL2018: { struct Cyc_Absyn_Tqual
_temp2025; struct Cyc_Core_Opt* _temp2027; struct _tuple13 _temp2023=* _temp2021;
_LL2028: _temp2027= _temp2023.f1; goto _LL2026; _LL2026: _temp2025= _temp2023.f2;
goto _LL2024; _LL2024: return({ struct _tuple2* _temp2029=( struct _tuple2*)
GC_malloc( sizeof( struct _tuple2)); _temp2029->f1= _temp2027; _temp2029->f2=
_temp2025; _temp2029->f3= _temp2019; _temp2029;});}} static void* Cyc_Tcutil_field_type(
struct Cyc_Absyn_Structfield* f){ return( void*) f->type;} static struct Cyc_Absyn_Structfield*
Cyc_Tcutil_zip_field_type( struct Cyc_Absyn_Structfield* f, void* t){ return({
struct Cyc_Absyn_Structfield* _temp2030=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2030->name= f->name;
_temp2030->tq= f->tq; _temp2030->type=( void*) t; _temp2030->width= f->width;
_temp2030->attributes= f->attributes; _temp2030;});} static struct Cyc_List_List*
Cyc_Tcutil_substs( struct _RegionHandle* rgn, struct Cyc_List_List* inst, struct
Cyc_List_List* ts); void* Cyc_Tcutil_rsubstitute( struct _RegionHandle* rgn,
struct Cyc_List_List* inst, void* t){ void* _temp2031= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Tvar* _temp2077; struct Cyc_Absyn_Structdecl** _temp2079;
struct Cyc_List_List* _temp2081; struct _tuple1* _temp2083; struct Cyc_Absyn_Uniondecl**
_temp2085; struct Cyc_List_List* _temp2087; struct _tuple1* _temp2089; struct
Cyc_Absyn_TunionInfo _temp2091; void* _temp2093; struct Cyc_List_List* _temp2095;
void* _temp2097; struct Cyc_Absyn_TunionFieldInfo _temp2099; struct Cyc_List_List*
_temp2101; void* _temp2103; struct Cyc_Core_Opt* _temp2105; struct Cyc_List_List*
_temp2107; struct _tuple1* _temp2109; struct Cyc_Absyn_Exp* _temp2111; struct
Cyc_Absyn_Tqual _temp2113; void* _temp2115; struct Cyc_Absyn_PtrInfo _temp2117;
struct Cyc_Absyn_Conref* _temp2119; struct Cyc_Absyn_Tqual _temp2121; struct Cyc_Absyn_Conref*
_temp2123; void* _temp2125; void* _temp2127; struct Cyc_Absyn_FnInfo _temp2129;
struct Cyc_List_List* _temp2131; int _temp2133; struct Cyc_List_List* _temp2135;
void* _temp2137; struct Cyc_Core_Opt* _temp2139; struct Cyc_List_List* _temp2141;
struct Cyc_List_List* _temp2143; struct Cyc_List_List* _temp2145; struct Cyc_List_List*
_temp2147; int _temp2149; struct Cyc_Core_Opt* _temp2151; void* _temp2153; void*
_temp2155; struct Cyc_Absyn_Enumdecl* _temp2157; struct _tuple1* _temp2159; void*
_temp2161; void* _temp2163; void* _temp2165; struct Cyc_List_List* _temp2167;
_LL2033: if(( unsigned int) _temp2031 > 4u?*(( int*) _temp2031) == Cyc_Absyn_VarType:
0){ _LL2078: _temp2077=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp2031)->f1; goto _LL2034;} else{ goto _LL2035;} _LL2035: if(( unsigned int)
_temp2031 > 4u?*(( int*) _temp2031) == Cyc_Absyn_StructType: 0){ _LL2084:
_temp2083=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp2031)->f1;
goto _LL2082; _LL2082: _temp2081=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp2031)->f2; goto _LL2080; _LL2080: _temp2079=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp2031)->f3; goto _LL2036;} else{ goto
_LL2037;} _LL2037: if(( unsigned int) _temp2031 > 4u?*(( int*) _temp2031) == Cyc_Absyn_UnionType:
0){ _LL2090: _temp2089=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp2031)->f1; goto _LL2088; _LL2088: _temp2087=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnionType_struct*) _temp2031)->f2; goto _LL2086; _LL2086:
_temp2085=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp2031)->f3; goto _LL2038;} else{ goto _LL2039;} _LL2039: if(( unsigned int)
_temp2031 > 4u?*(( int*) _temp2031) == Cyc_Absyn_TunionType: 0){ _LL2092:
_temp2091=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp2031)->f1; _LL2098: _temp2097=( void*) _temp2091.tunion_info; goto _LL2096;
_LL2096: _temp2095=( struct Cyc_List_List*) _temp2091.targs; goto _LL2094;
_LL2094: _temp2093=( void*) _temp2091.rgn; goto _LL2040;} else{ goto _LL2041;}
_LL2041: if(( unsigned int) _temp2031 > 4u?*(( int*) _temp2031) == Cyc_Absyn_TunionFieldType:
0){ _LL2100: _temp2099=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp2031)->f1; _LL2104: _temp2103=( void*) _temp2099.field_info; goto _LL2102;
_LL2102: _temp2101=( struct Cyc_List_List*) _temp2099.targs; goto _LL2042;}
else{ goto _LL2043;} _LL2043: if(( unsigned int) _temp2031 > 4u?*(( int*)
_temp2031) == Cyc_Absyn_TypedefType: 0){ _LL2110: _temp2109=( struct _tuple1*)((
struct Cyc_Absyn_TypedefType_struct*) _temp2031)->f1; goto _LL2108; _LL2108:
_temp2107=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp2031)->f2; goto _LL2106; _LL2106: _temp2105=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_TypedefType_struct*) _temp2031)->f3; goto _LL2044;} else{ goto
_LL2045;} _LL2045: if(( unsigned int) _temp2031 > 4u?*(( int*) _temp2031) == Cyc_Absyn_ArrayType:
0){ _LL2116: _temp2115=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2031)->f1;
goto _LL2114; _LL2114: _temp2113=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2031)->f2; goto _LL2112; _LL2112: _temp2111=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2031)->f3; goto _LL2046;} else{ goto
_LL2047;} _LL2047: if(( unsigned int) _temp2031 > 4u?*(( int*) _temp2031) == Cyc_Absyn_PointerType:
0){ _LL2118: _temp2117=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp2031)->f1; _LL2128: _temp2127=( void*) _temp2117.elt_typ; goto _LL2126;
_LL2126: _temp2125=( void*) _temp2117.rgn_typ; goto _LL2124; _LL2124: _temp2123=(
struct Cyc_Absyn_Conref*) _temp2117.nullable; goto _LL2122; _LL2122: _temp2121=(
struct Cyc_Absyn_Tqual) _temp2117.tq; goto _LL2120; _LL2120: _temp2119=( struct
Cyc_Absyn_Conref*) _temp2117.bounds; goto _LL2048;} else{ goto _LL2049;} _LL2049:
if(( unsigned int) _temp2031 > 4u?*(( int*) _temp2031) == Cyc_Absyn_FnType: 0){
_LL2130: _temp2129=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp2031)->f1; _LL2142: _temp2141=( struct Cyc_List_List*) _temp2129.tvars;
goto _LL2140; _LL2140: _temp2139=( struct Cyc_Core_Opt*) _temp2129.effect; goto
_LL2138; _LL2138: _temp2137=( void*) _temp2129.ret_typ; goto _LL2136; _LL2136:
_temp2135=( struct Cyc_List_List*) _temp2129.args; goto _LL2134; _LL2134:
_temp2133=( int) _temp2129.varargs; goto _LL2132; _LL2132: _temp2131=( struct
Cyc_List_List*) _temp2129.attributes; goto _LL2050;} else{ goto _LL2051;}
_LL2051: if(( unsigned int) _temp2031 > 4u?*(( int*) _temp2031) == Cyc_Absyn_TupleType:
0){ _LL2144: _temp2143=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp2031)->f1; goto _LL2052;} else{ goto _LL2053;} _LL2053: if(( unsigned int)
_temp2031 > 4u?*(( int*) _temp2031) == Cyc_Absyn_AnonStructType: 0){ _LL2146:
_temp2145=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp2031)->f1; goto _LL2054;} else{ goto _LL2055;} _LL2055: if(( unsigned int)
_temp2031 > 4u?*(( int*) _temp2031) == Cyc_Absyn_AnonUnionType: 0){ _LL2148:
_temp2147=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp2031)->f1; goto _LL2056;} else{ goto _LL2057;} _LL2057: if(( unsigned int)
_temp2031 > 4u?*(( int*) _temp2031) == Cyc_Absyn_Evar: 0){ _LL2154: _temp2153=(
void*)(( struct Cyc_Absyn_Evar_struct*) _temp2031)->f1; goto _LL2152; _LL2152:
_temp2151=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp2031)->f2;
goto _LL2150; _LL2150: _temp2149=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp2031)->f3; goto _LL2058;} else{ goto _LL2059;} _LL2059: if(( unsigned int)
_temp2031 > 4u?*(( int*) _temp2031) == Cyc_Absyn_RgnHandleType: 0){ _LL2156:
_temp2155=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp2031)->f1;
goto _LL2060;} else{ goto _LL2061;} _LL2061: if(( unsigned int) _temp2031 > 4u?*((
int*) _temp2031) == Cyc_Absyn_EnumType: 0){ _LL2160: _temp2159=( struct _tuple1*)((
struct Cyc_Absyn_EnumType_struct*) _temp2031)->f1; goto _LL2158; _LL2158:
_temp2157=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*)
_temp2031)->f2; goto _LL2062;} else{ goto _LL2063;} _LL2063: if( _temp2031 ==(
void*) Cyc_Absyn_VoidType){ goto _LL2064;} else{ goto _LL2065;} _LL2065: if((
unsigned int) _temp2031 > 4u?*(( int*) _temp2031) == Cyc_Absyn_IntType: 0){
_LL2164: _temp2163=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2031)->f1;
goto _LL2162; _LL2162: _temp2161=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2031)->f2; goto _LL2066;} else{ goto _LL2067;} _LL2067: if( _temp2031 ==(
void*) Cyc_Absyn_FloatType){ goto _LL2068;} else{ goto _LL2069;} _LL2069: if(
_temp2031 ==( void*) Cyc_Absyn_DoubleType){ goto _LL2070;} else{ goto _LL2071;}
_LL2071: if( _temp2031 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL2072;} else{ goto
_LL2073;} _LL2073: if(( unsigned int) _temp2031 > 4u?*(( int*) _temp2031) == Cyc_Absyn_AccessEff:
0){ _LL2166: _temp2165=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp2031)->f1;
goto _LL2074;} else{ goto _LL2075;} _LL2075: if(( unsigned int) _temp2031 > 4u?*((
int*) _temp2031) == Cyc_Absyn_JoinEff: 0){ _LL2168: _temp2167=( struct Cyc_List_List*)((
struct Cyc_Absyn_JoinEff_struct*) _temp2031)->f1; goto _LL2076;} else{ goto
_LL2032;} _LL2034: { struct _handler_cons _temp2169; _push_handler(& _temp2169);{
int _temp2171= 0; if( setjmp( _temp2169.handler)){ _temp2171= 1;} if( !
_temp2171){{ void* _temp2172=(( void*(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_assoc_cmp)(
Cyc_Absyn_tvar_cmp, inst, _temp2077); _npop_handler( 0u); return _temp2172;};
_pop_handler();} else{ void* _temp2170=( void*) _exn_thrown; void* _temp2174=
_temp2170; _LL2176: if( _temp2174 == Cyc_Core_Not_found){ goto _LL2177;} else{
goto _LL2178;} _LL2178: goto _LL2179; _LL2177: return t; _LL2179:( void) _throw(
_temp2174); _LL2175:;}}} _LL2036: { struct Cyc_List_List* new_ts= Cyc_Tcutil_substs(
rgn, inst, _temp2081); return new_ts == _temp2081? t:( void*)({ struct Cyc_Absyn_StructType_struct*
_temp2180=( struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp2180[ 0]=({ struct Cyc_Absyn_StructType_struct _temp2181; _temp2181.tag=
Cyc_Absyn_StructType; _temp2181.f1= _temp2083; _temp2181.f2= new_ts; _temp2181.f3=
_temp2079; _temp2181;}); _temp2180;});} _LL2038: { struct Cyc_List_List* new_ts=
Cyc_Tcutil_substs( rgn, inst, _temp2087); return new_ts == _temp2087? t:( void*)({
struct Cyc_Absyn_UnionType_struct* _temp2182=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp2182[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp2183; _temp2183.tag= Cyc_Absyn_UnionType;
_temp2183.f1= _temp2089; _temp2183.f2= new_ts; _temp2183.f3= _temp2085;
_temp2183;}); _temp2182;});} _LL2040: { struct Cyc_List_List* new_ts= Cyc_Tcutil_substs(
rgn, inst, _temp2095); void* new_r= Cyc_Tcutil_rsubstitute( rgn, inst, _temp2093);
return( new_ts == _temp2095? new_r == _temp2093: 0)? t:( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp2184=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp2184[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp2185; _temp2185.tag=
Cyc_Absyn_TunionType; _temp2185.f1=({ struct Cyc_Absyn_TunionInfo _temp2186;
_temp2186.tunion_info=( void*) _temp2097; _temp2186.targs= new_ts; _temp2186.rgn=(
void*) new_r; _temp2186;}); _temp2185;}); _temp2184;});} _LL2042: { struct Cyc_List_List*
new_ts= Cyc_Tcutil_substs( rgn, inst, _temp2101); return new_ts == _temp2101? t:(
void*)({ struct Cyc_Absyn_TunionFieldType_struct* _temp2187=( struct Cyc_Absyn_TunionFieldType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct)); _temp2187[ 0]=({
struct Cyc_Absyn_TunionFieldType_struct _temp2188; _temp2188.tag= Cyc_Absyn_TunionFieldType;
_temp2188.f1=({ struct Cyc_Absyn_TunionFieldInfo _temp2189; _temp2189.field_info=(
void*) _temp2103; _temp2189.targs= new_ts; _temp2189;}); _temp2188;}); _temp2187;});}
_LL2044: { struct Cyc_List_List* new_ts= Cyc_Tcutil_substs( rgn, inst, _temp2107);
return new_ts == _temp2107? t:( void*)({ struct Cyc_Absyn_TypedefType_struct*
_temp2190=( struct Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp2190[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp2191; _temp2191.tag=
Cyc_Absyn_TypedefType; _temp2191.f1= _temp2109; _temp2191.f2= new_ts; _temp2191.f3=
_temp2105; _temp2191;}); _temp2190;});} _LL2046: { void* new_t1= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2115); return new_t1 == _temp2115? t:( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp2192=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp2192[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp2193; _temp2193.tag= Cyc_Absyn_ArrayType;
_temp2193.f1=( void*) new_t1; _temp2193.f2= _temp2113; _temp2193.f3= _temp2111;
_temp2193;}); _temp2192;});} _LL2048: { void* new_t1= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2127); void* new_r= Cyc_Tcutil_rsubstitute( rgn, inst, _temp2125);
if( new_t1 == _temp2127? new_r == _temp2125: 0){ return t;} return( void*)({
struct Cyc_Absyn_PointerType_struct* _temp2194=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp2194[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp2195; _temp2195.tag= Cyc_Absyn_PointerType;
_temp2195.f1=({ struct Cyc_Absyn_PtrInfo _temp2196; _temp2196.elt_typ=( void*)
new_t1; _temp2196.rgn_typ=( void*) new_r; _temp2196.nullable= _temp2123;
_temp2196.tq= _temp2121; _temp2196.bounds= _temp2119; _temp2196;}); _temp2195;});
_temp2194;});} _LL2050:{ struct Cyc_List_List* p= _temp2141; for( 0; p != 0; p=({
struct Cyc_List_List* _temp2197= p; if( _temp2197 == 0){ _throw( Null_Exception);}
_temp2197->tl;})){ inst=({ struct Cyc_List_List* _temp2198=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2198->hd=( void*)({
struct _tuple5* _temp2199=( struct _tuple5*) _region_malloc( rgn, sizeof( struct
_tuple5)); _temp2199->f1=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp2203= p; if( _temp2203 == 0){ _throw( Null_Exception);} _temp2203->hd;});
_temp2199->f2=( void*)({ struct Cyc_Absyn_VarType_struct* _temp2200=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp2200[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp2201; _temp2201.tag= Cyc_Absyn_VarType; _temp2201.f1=(
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp2202= p; if( _temp2202 == 0){
_throw( Null_Exception);} _temp2202->hd;}); _temp2201;}); _temp2200;});
_temp2199;}); _temp2198->tl= inst; _temp2198;});}}{ struct Cyc_List_List*
_temp2206; struct Cyc_List_List* _temp2208; struct _tuple0 _temp2204=(( struct
_tuple0(*)( struct _RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List*
x)) Cyc_List_rsplit)( rgn, rgn,(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct _tuple14*(* f)( struct _RegionHandle*, struct _tuple2*), struct
_RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_substitute_f1,
rgn, _temp2135)); _LL2209: _temp2208= _temp2204.f1; goto _LL2207; _LL2207:
_temp2206= _temp2204.f2; goto _LL2205; _LL2205: { struct Cyc_List_List* ts2= Cyc_Tcutil_substs(
rgn, inst, _temp2206); struct Cyc_List_List* args2=(( struct Cyc_List_List*(*)(
struct _tuple2*(* f)( struct _tuple14*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcutil_substitute_f2,(( struct Cyc_List_List*(*)( struct _RegionHandle* r3,
struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, _temp2208, ts2)); struct Cyc_Core_Opt* eff2; if( _temp2139 == 0){ eff2=
0;} else{ void* new_eff= Cyc_Tcutil_rsubstitute( rgn, inst,( void*)({ struct Cyc_Core_Opt*
_temp2212= _temp2139; if( _temp2212 == 0){ _throw( Null_Exception);} _temp2212->v;}));
if( new_eff ==( void*)({ struct Cyc_Core_Opt* _temp2210= _temp2139; if(
_temp2210 == 0){ _throw( Null_Exception);} _temp2210->v;})){ eff2= _temp2139;}
else{ eff2=({ struct Cyc_Core_Opt* _temp2211=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp2211->v=( void*) new_eff; _temp2211;});}}
return( void*)({ struct Cyc_Absyn_FnType_struct* _temp2213=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp2213[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp2214; _temp2214.tag= Cyc_Absyn_FnType; _temp2214.f1=({ struct Cyc_Absyn_FnInfo
_temp2215; _temp2215.tvars= _temp2141; _temp2215.effect= eff2; _temp2215.ret_typ=(
void*) Cyc_Tcutil_rsubstitute( rgn, inst, _temp2137); _temp2215.args= args2;
_temp2215.varargs= _temp2133; _temp2215.attributes= _temp2131; _temp2215;});
_temp2214;}); _temp2213;});}} _LL2052: { struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, void*(* f)( struct _tuple4*), struct Cyc_List_List* x))
Cyc_List_rmap)( rgn, Cyc_Tcutil_snd_tqt, _temp2143); struct Cyc_List_List* ts2=
Cyc_Tcutil_substs( rgn, inst, ts); if( ts2 == ts){ return t;}{ struct Cyc_List_List*
tqts2=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct _tuple4*, void*),
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_map2)( Cyc_Tcutil_map2_tq,
_temp2143, ts2); return( void*)({ struct Cyc_Absyn_TupleType_struct* _temp2216=(
struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp2216[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp2217; _temp2217.tag= Cyc_Absyn_TupleType;
_temp2217.f1= tqts2; _temp2217;}); _temp2216;});}} _LL2054: { struct Cyc_List_List*
ts=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct Cyc_Absyn_Structfield*),
struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type, _temp2145);
struct Cyc_List_List* ts2= Cyc_Tcutil_substs( rgn, inst, ts); if( ts2 == ts){
return t;}{ struct Cyc_List_List* fs2=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type, _temp2145, ts2); return( void*)({
struct Cyc_Absyn_AnonStructType_struct* _temp2218=( struct Cyc_Absyn_AnonStructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AnonStructType_struct)); _temp2218[ 0]=({
struct Cyc_Absyn_AnonStructType_struct _temp2219; _temp2219.tag= Cyc_Absyn_AnonStructType;
_temp2219.f1= fs2; _temp2219;}); _temp2218;});}} _LL2056: { struct Cyc_List_List*
ts=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct Cyc_Absyn_Structfield*),
struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type, _temp2147);
struct Cyc_List_List* ts2= Cyc_Tcutil_substs( rgn, inst, ts); if( ts2 == ts){
return t;}{ struct Cyc_List_List* fs2=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type, _temp2147, ts2); return( void*)({
struct Cyc_Absyn_AnonStructType_struct* _temp2220=( struct Cyc_Absyn_AnonStructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AnonStructType_struct)); _temp2220[ 0]=({
struct Cyc_Absyn_AnonStructType_struct _temp2221; _temp2221.tag= Cyc_Absyn_AnonStructType;
_temp2221.f1= fs2; _temp2221;}); _temp2220;});}} _LL2058: if( _temp2151 != 0){
return Cyc_Tcutil_rsubstitute( rgn, inst,( void*)({ struct Cyc_Core_Opt*
_temp2222= _temp2151; if( _temp2222 == 0){ _throw( Null_Exception);} _temp2222->v;}));}
else{ return t;} _LL2060: { void* new_rt= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2155); return new_rt == _temp2155? t:( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp2223=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp2223[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp2224; _temp2224.tag= Cyc_Absyn_RgnHandleType; _temp2224.f1=( void*) new_rt;
_temp2224;}); _temp2223;});} _LL2062: return t; _LL2064: return t; _LL2066:
return t; _LL2068: return t; _LL2070: return t; _LL2072: return t; _LL2074: {
void* new_r= Cyc_Tcutil_rsubstitute( rgn, inst, _temp2165); return new_r ==
_temp2165? t:( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2225=( struct
Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2225[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2226; _temp2226.tag= Cyc_Absyn_AccessEff;
_temp2226.f1=( void*) new_r; _temp2226;}); _temp2225;});} _LL2076: { struct Cyc_List_List*
new_es= Cyc_Tcutil_substs( rgn, inst, _temp2167); return new_es == _temp2167? t:(
void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2227=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp2227[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp2228; _temp2228.tag= Cyc_Absyn_JoinEff; _temp2228.f1=
new_es; _temp2228;}); _temp2227;});} _LL2032:;} static struct Cyc_List_List* Cyc_Tcutil_substs(
struct _RegionHandle* rgn, struct Cyc_List_List* inst, struct Cyc_List_List* ts){
if( ts == 0){ return 0;}{ void* old_hd=( void*)({ struct Cyc_List_List*
_temp2231= ts; if( _temp2231 == 0){ _throw( Null_Exception);} _temp2231->hd;});
struct Cyc_List_List* old_tl=({ struct Cyc_List_List* _temp2230= ts; if(
_temp2230 == 0){ _throw( Null_Exception);} _temp2230->tl;}); void* new_hd= Cyc_Tcutil_rsubstitute(
rgn, inst, old_hd); struct Cyc_List_List* new_tl= Cyc_Tcutil_substs( rgn, inst,
old_tl); if( old_hd == new_hd? old_tl == new_tl: 0){ return ts;} return( struct
Cyc_List_List*)({ struct Cyc_List_List* _temp2229=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2229->hd=( void*) new_hd;
_temp2229->tl= new_tl; _temp2229;});}} extern void* Cyc_Tcutil_substitute(
struct Cyc_List_List* inst, void* t){ return Cyc_Tcutil_rsubstitute( Cyc_Core_heap_region,
inst, t);} struct Cyc_Absyn_Exp* Cyc_Tcutil_default_initializer( struct Cyc_Tcenv_Tenv*
te, void* t, struct Cyc_Position_Segment* loc){ struct Cyc_Absyn_Exp* e= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp2281=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2281[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2282; _temp2282.tag= Cyc_Absyn_Const_e; _temp2282.f1=(
void*)(( void*) Cyc_Absyn_Null_c); _temp2282;}); _temp2281;}), loc);{ void*
_temp2232= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp2246; void*
_temp2248; void* _temp2250; void* _temp2252; void* _temp2254; _LL2234: if((
unsigned int) _temp2232 > 4u?*(( int*) _temp2232) == Cyc_Absyn_PointerType: 0){
_LL2247: _temp2246=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp2232)->f1; goto _LL2235;} else{ goto _LL2236;} _LL2236: if(( unsigned int)
_temp2232 > 4u?*(( int*) _temp2232) == Cyc_Absyn_IntType: 0){ _LL2251: _temp2250=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp2232)->f1; goto _LL2249; _LL2249:
_temp2248=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2232)->f2; if(
_temp2248 ==( void*) Cyc_Absyn_B1){ goto _LL2237;} else{ goto _LL2238;}} else{
goto _LL2238;} _LL2238: if(( unsigned int) _temp2232 > 4u?*(( int*) _temp2232)
== Cyc_Absyn_IntType: 0){ _LL2255: _temp2254=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2232)->f1; goto _LL2253; _LL2253: _temp2252=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2232)->f2; goto _LL2239;} else{ goto _LL2240;} _LL2240: if( _temp2232 ==(
void*) Cyc_Absyn_FloatType){ goto _LL2241;} else{ goto _LL2242;} _LL2242: if(
_temp2232 ==( void*) Cyc_Absyn_DoubleType){ goto _LL2243;} else{ goto _LL2244;}
_LL2244: goto _LL2245; _LL2235: goto _LL2233; _LL2237:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp2256=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2256[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2257; _temp2257.tag= Cyc_Absyn_Const_e; _temp2257.f1=(
void*)(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp2258=( struct Cyc_Absyn_Char_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct)); _temp2258[ 0]=({ struct Cyc_Absyn_Char_c_struct
_temp2259; _temp2259.tag= Cyc_Absyn_Char_c; _temp2259.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp2259.f2='\000'; _temp2259;}); _temp2258;})); _temp2257;}); _temp2256;})));
goto _LL2233; _LL2239:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2260=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2260[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2261; _temp2261.tag= Cyc_Absyn_Const_e;
_temp2261.f1=( void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp2262=(
struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp2262[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp2263; _temp2263.tag= Cyc_Absyn_Int_c;
_temp2263.f1=( void*) _temp2254; _temp2263.f2= 0; _temp2263;}); _temp2262;}));
_temp2261;}); _temp2260;}))); if( _temp2252 !=( void*) Cyc_Absyn_B4){ e= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Cast_e_struct* _temp2264=( struct Cyc_Absyn_Cast_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp2264[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp2265; _temp2265.tag= Cyc_Absyn_Cast_e; _temp2265.f1=( void*) t; _temp2265.f2=
e; _temp2265;}); _temp2264;}), loc);} goto _LL2233; _LL2241:( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp2266=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2266[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2267; _temp2267.tag= Cyc_Absyn_Const_e; _temp2267.f1=(
void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp2268=( struct Cyc_Absyn_Float_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Float_c_struct)); _temp2268[ 0]=({ struct
Cyc_Absyn_Float_c_struct _temp2269; _temp2269.tag= Cyc_Absyn_Float_c; _temp2269.f1=(
struct _tagged_string)({ char* _temp2270=( char*)"0.0"; struct _tagged_string
_temp2271; _temp2271.curr= _temp2270; _temp2271.base= _temp2270; _temp2271.last_plus_one=
_temp2270 + 4; _temp2271;}); _temp2269;}); _temp2268;})); _temp2267;});
_temp2266;}))); goto _LL2233; _LL2243:( void*)( e->r=( void*)(( void*)({ struct
Cyc_Absyn_Cast_e_struct* _temp2272=( struct Cyc_Absyn_Cast_e_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp2272[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp2273; _temp2273.tag= Cyc_Absyn_Cast_e; _temp2273.f1=( void*) t; _temp2273.f2=
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp2274=( struct
Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2274[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2275; _temp2275.tag= Cyc_Absyn_Const_e;
_temp2275.f1=( void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp2276=(
struct Cyc_Absyn_Float_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Float_c_struct));
_temp2276[ 0]=({ struct Cyc_Absyn_Float_c_struct _temp2277; _temp2277.tag= Cyc_Absyn_Float_c;
_temp2277.f1=( struct _tagged_string)({ char* _temp2278=( char*)"0.0"; struct
_tagged_string _temp2279; _temp2279.curr= _temp2278; _temp2279.base= _temp2278;
_temp2279.last_plus_one= _temp2278 + 4; _temp2279;}); _temp2277;}); _temp2276;}));
_temp2275;}); _temp2274;}), loc); _temp2273;}); _temp2272;}))); goto _LL2233;
_LL2245: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2280= Cyc_Absynpp_typ2string(
t); xprintf("declaration of type %.*s requires initializer", _temp2280.last_plus_one
- _temp2280.curr, _temp2280.curr);})); goto _LL2233; _LL2233:;} return e;}
struct _tuple5* Cyc_Tcutil_make_inst_var( struct Cyc_Absyn_Tvar* tv){ void* k=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( tv->kind); return({
struct _tuple5* _temp2283=( struct _tuple5*) GC_malloc( sizeof( struct _tuple5));
_temp2283->f1= tv; _temp2283->f2= Cyc_Absyn_new_evar( k); _temp2283;});} struct
_tuple5* Cyc_Tcutil_r_make_inst_var( struct _RegionHandle* rgn, struct Cyc_Absyn_Tvar*
tv){ void* k=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( tv->kind);
return({ struct _tuple5* _temp2284=( struct _tuple5*) _region_malloc( rgn,
sizeof( struct _tuple5)); _temp2284->f1= tv; _temp2284->f2= Cyc_Absyn_new_evar(
k); _temp2284;});} struct Cyc_List_List* Cyc_Tcutil_check_valid_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* free_vars, void*
expected_kind, void* t){{ void* _temp2285= Cyc_Tcutil_compress( t); int
_temp2331; struct Cyc_Core_Opt* _temp2333; void* _temp2335; struct Cyc_Absyn_Tvar*
_temp2337; struct Cyc_Absyn_Enumdecl* _temp2339; struct Cyc_Absyn_Enumdecl**
_temp2341; struct _tuple1* _temp2342; struct Cyc_Absyn_TunionInfo _temp2344;
void* _temp2346; struct Cyc_List_List* _temp2348; void* _temp2350; void**
_temp2352; struct Cyc_Absyn_TunionFieldInfo _temp2353; struct Cyc_List_List*
_temp2355; void* _temp2357; void** _temp2359; struct Cyc_Absyn_PtrInfo _temp2360;
struct Cyc_Absyn_Conref* _temp2362; struct Cyc_Absyn_Tqual _temp2364; struct Cyc_Absyn_Conref*
_temp2366; void* _temp2368; void* _temp2370; void* _temp2372; void* _temp2374;
struct Cyc_Absyn_Exp* _temp2376; struct Cyc_Absyn_Tqual _temp2378; void*
_temp2380; struct Cyc_Absyn_FnInfo _temp2382; struct Cyc_List_List* _temp2384;
int _temp2386; struct Cyc_List_List* _temp2388; void* _temp2390; struct Cyc_Core_Opt*
_temp2392; struct Cyc_Core_Opt** _temp2394; struct Cyc_List_List* _temp2395;
struct Cyc_List_List** _temp2397; struct Cyc_List_List* _temp2398; struct Cyc_List_List*
_temp2400; struct Cyc_List_List* _temp2402; struct Cyc_Absyn_Structdecl**
_temp2404; struct Cyc_Absyn_Structdecl*** _temp2406; struct Cyc_List_List*
_temp2407; struct _tuple1* _temp2409; struct Cyc_Absyn_Uniondecl** _temp2411;
struct Cyc_Absyn_Uniondecl*** _temp2413; struct Cyc_List_List* _temp2414; struct
_tuple1* _temp2416; struct Cyc_Core_Opt* _temp2418; struct Cyc_Core_Opt**
_temp2420; struct Cyc_List_List* _temp2421; struct _tuple1* _temp2423; void*
_temp2425; void* _temp2427; struct Cyc_List_List* _temp2429; _LL2287: if(
_temp2285 ==( void*) Cyc_Absyn_VoidType){ goto _LL2288;} else{ goto _LL2289;}
_LL2289: if(( unsigned int) _temp2285 > 4u?*(( int*) _temp2285) == Cyc_Absyn_Evar:
0){ _LL2336: _temp2335=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp2285)->f1;
goto _LL2334; _LL2334: _temp2333=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp2285)->f2; goto _LL2332; _LL2332: _temp2331=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp2285)->f3; goto _LL2290;} else{ goto _LL2291;} _LL2291: if(( unsigned int)
_temp2285 > 4u?*(( int*) _temp2285) == Cyc_Absyn_VarType: 0){ _LL2338: _temp2337=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*) _temp2285)->f1; goto
_LL2292;} else{ goto _LL2293;} _LL2293: if(( unsigned int) _temp2285 > 4u?*((
int*) _temp2285) == Cyc_Absyn_EnumType: 0){ _LL2343: _temp2342=( struct _tuple1*)((
struct Cyc_Absyn_EnumType_struct*) _temp2285)->f1; goto _LL2340; _LL2340:
_temp2339=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*)
_temp2285)->f2; _temp2341=&(( struct Cyc_Absyn_EnumType_struct*) _temp2285)->f2;
goto _LL2294;} else{ goto _LL2295;} _LL2295: if(( unsigned int) _temp2285 > 4u?*((
int*) _temp2285) == Cyc_Absyn_TunionType: 0){ _LL2345: _temp2344=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp2285)->f1; _LL2351: _temp2350=( void*)
_temp2344.tunion_info; _temp2352=&((( struct Cyc_Absyn_TunionType_struct*)
_temp2285)->f1).tunion_info; goto _LL2349; _LL2349: _temp2348=( struct Cyc_List_List*)
_temp2344.targs; goto _LL2347; _LL2347: _temp2346=( void*) _temp2344.rgn; goto
_LL2296;} else{ goto _LL2297;} _LL2297: if(( unsigned int) _temp2285 > 4u?*((
int*) _temp2285) == Cyc_Absyn_TunionFieldType: 0){ _LL2354: _temp2353=( struct
Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*) _temp2285)->f1;
_LL2358: _temp2357=( void*) _temp2353.field_info; _temp2359=&((( struct Cyc_Absyn_TunionFieldType_struct*)
_temp2285)->f1).field_info; goto _LL2356; _LL2356: _temp2355=( struct Cyc_List_List*)
_temp2353.targs; goto _LL2298;} else{ goto _LL2299;} _LL2299: if(( unsigned int)
_temp2285 > 4u?*(( int*) _temp2285) == Cyc_Absyn_PointerType: 0){ _LL2361:
_temp2360=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp2285)->f1; _LL2371: _temp2370=( void*) _temp2360.elt_typ; goto _LL2369;
_LL2369: _temp2368=( void*) _temp2360.rgn_typ; goto _LL2367; _LL2367: _temp2366=(
struct Cyc_Absyn_Conref*) _temp2360.nullable; goto _LL2365; _LL2365: _temp2364=(
struct Cyc_Absyn_Tqual) _temp2360.tq; goto _LL2363; _LL2363: _temp2362=( struct
Cyc_Absyn_Conref*) _temp2360.bounds; goto _LL2300;} else{ goto _LL2301;} _LL2301:
if(( unsigned int) _temp2285 > 4u?*(( int*) _temp2285) == Cyc_Absyn_IntType: 0){
_LL2375: _temp2374=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2285)->f1;
goto _LL2373; _LL2373: _temp2372=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2285)->f2; goto _LL2302;} else{ goto _LL2303;} _LL2303: if( _temp2285 ==(
void*) Cyc_Absyn_FloatType){ goto _LL2304;} else{ goto _LL2305;} _LL2305: if(
_temp2285 ==( void*) Cyc_Absyn_DoubleType){ goto _LL2306;} else{ goto _LL2307;}
_LL2307: if(( unsigned int) _temp2285 > 4u?*(( int*) _temp2285) == Cyc_Absyn_ArrayType:
0){ _LL2381: _temp2380=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2285)->f1;
goto _LL2379; _LL2379: _temp2378=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2285)->f2; goto _LL2377; _LL2377: _temp2376=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2285)->f3; goto _LL2308;} else{ goto
_LL2309;} _LL2309: if(( unsigned int) _temp2285 > 4u?*(( int*) _temp2285) == Cyc_Absyn_FnType:
0){ _LL2383: _temp2382=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp2285)->f1; _LL2396: _temp2395=( struct Cyc_List_List*) _temp2382.tvars;
_temp2397=&((( struct Cyc_Absyn_FnType_struct*) _temp2285)->f1).tvars; goto
_LL2393; _LL2393: _temp2392=( struct Cyc_Core_Opt*) _temp2382.effect; _temp2394=&(((
struct Cyc_Absyn_FnType_struct*) _temp2285)->f1).effect; goto _LL2391; _LL2391:
_temp2390=( void*) _temp2382.ret_typ; goto _LL2389; _LL2389: _temp2388=( struct
Cyc_List_List*) _temp2382.args; goto _LL2387; _LL2387: _temp2386=( int)
_temp2382.varargs; goto _LL2385; _LL2385: _temp2384=( struct Cyc_List_List*)
_temp2382.attributes; goto _LL2310;} else{ goto _LL2311;} _LL2311: if((
unsigned int) _temp2285 > 4u?*(( int*) _temp2285) == Cyc_Absyn_TupleType: 0){
_LL2399: _temp2398=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp2285)->f1; goto _LL2312;} else{ goto _LL2313;} _LL2313: if(( unsigned int)
_temp2285 > 4u?*(( int*) _temp2285) == Cyc_Absyn_AnonStructType: 0){ _LL2401:
_temp2400=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp2285)->f1; goto _LL2314;} else{ goto _LL2315;} _LL2315: if(( unsigned int)
_temp2285 > 4u?*(( int*) _temp2285) == Cyc_Absyn_AnonUnionType: 0){ _LL2403:
_temp2402=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp2285)->f1; goto _LL2316;} else{ goto _LL2317;} _LL2317: if(( unsigned int)
_temp2285 > 4u?*(( int*) _temp2285) == Cyc_Absyn_StructType: 0){ _LL2410:
_temp2409=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp2285)->f1;
goto _LL2408; _LL2408: _temp2407=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp2285)->f2; goto _LL2405; _LL2405: _temp2404=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp2285)->f3; _temp2406=&(( struct Cyc_Absyn_StructType_struct*)
_temp2285)->f3; goto _LL2318;} else{ goto _LL2319;} _LL2319: if(( unsigned int)
_temp2285 > 4u?*(( int*) _temp2285) == Cyc_Absyn_UnionType: 0){ _LL2417:
_temp2416=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*) _temp2285)->f1;
goto _LL2415; _LL2415: _temp2414=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp2285)->f2; goto _LL2412; _LL2412: _temp2411=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp2285)->f3; _temp2413=&(( struct Cyc_Absyn_UnionType_struct*)
_temp2285)->f3; goto _LL2320;} else{ goto _LL2321;} _LL2321: if(( unsigned int)
_temp2285 > 4u?*(( int*) _temp2285) == Cyc_Absyn_TypedefType: 0){ _LL2424:
_temp2423=( struct _tuple1*)(( struct Cyc_Absyn_TypedefType_struct*) _temp2285)->f1;
goto _LL2422; _LL2422: _temp2421=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp2285)->f2; goto _LL2419; _LL2419: _temp2418=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_TypedefType_struct*) _temp2285)->f3; _temp2420=&(( struct Cyc_Absyn_TypedefType_struct*)
_temp2285)->f3; goto _LL2322;} else{ goto _LL2323;} _LL2323: if( _temp2285 ==(
void*) Cyc_Absyn_HeapRgn){ goto _LL2324;} else{ goto _LL2325;} _LL2325: if((
unsigned int) _temp2285 > 4u?*(( int*) _temp2285) == Cyc_Absyn_RgnHandleType: 0){
_LL2426: _temp2425=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp2285)->f1;
goto _LL2326;} else{ goto _LL2327;} _LL2327: if(( unsigned int) _temp2285 > 4u?*((
int*) _temp2285) == Cyc_Absyn_AccessEff: 0){ _LL2428: _temp2427=( void*)((
struct Cyc_Absyn_AccessEff_struct*) _temp2285)->f1; goto _LL2328;} else{ goto
_LL2329;} _LL2329: if(( unsigned int) _temp2285 > 4u?*(( int*) _temp2285) == Cyc_Absyn_JoinEff:
0){ _LL2430: _temp2429=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*)
_temp2285)->f1; goto _LL2330;} else{ goto _LL2286;} _LL2288: goto _LL2286;
_LL2290: goto _LL2286; _LL2292: { struct Cyc_Absyn_Conref* c=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp2337->kind); if((
void*) c->v ==( void*) Cyc_Absyn_No_constr){( void*)( c->v=( void*)(( void*)({
struct Cyc_Absyn_Eq_constr_struct* _temp2431=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp2431[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp2432; _temp2432.tag= Cyc_Absyn_Eq_constr;
_temp2432.f1=( void*) expected_kind; _temp2432;}); _temp2431;})));} free_vars=
Cyc_Tcutil_add_free_tvar( loc, free_vars, _temp2337); goto _LL2286;} _LL2294:
if(* _temp2341 == 0){ struct _handler_cons _temp2433; _push_handler(& _temp2433);{
int _temp2435= 0; if( setjmp( _temp2433.handler)){ _temp2435= 1;} if( !
_temp2435){{ struct Cyc_Absyn_Enumdecl** ed= Cyc_Tcenv_lookup_enumdecl( te, loc,
_temp2342);* _temp2341=( struct Cyc_Absyn_Enumdecl*)* ed;}; _pop_handler();}
else{ void* _temp2434=( void*) _exn_thrown; void* _temp2437= _temp2434; _LL2439:
if( _temp2437 == Cyc_Dict_Absent){ goto _LL2440;} else{ goto _LL2441;} _LL2441:
goto _LL2442; _LL2440: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2443=
Cyc_Absynpp_qvar2string( _temp2342); xprintf("unbound type enum %.*s", _temp2443.last_plus_one
- _temp2443.curr, _temp2443.curr);})); return free_vars; _LL2442:( void) _throw(
_temp2437); _LL2438:;}}}{ struct Cyc_Absyn_Enumdecl* ed=( struct Cyc_Absyn_Enumdecl*)({
struct Cyc_Absyn_Enumdecl* _temp2446=* _temp2341; if( _temp2446 == 0){ _throw(
Null_Exception);} _temp2446;});* _temp2342=({ struct _tuple1* _temp2444= ed->name;
unsigned int _temp2445= 0; if( _temp2445 >= 1u){ _throw( Null_Exception);}
_temp2444[ _temp2445];}); goto _LL2286;} _LL2296:{ void* _temp2447=* _temp2352;
struct Cyc_Absyn_UnknownTunionInfo _temp2453; int _temp2455; struct _tuple1*
_temp2457; struct Cyc_Absyn_Tuniondecl* _temp2459; _LL2449: if(*(( int*)
_temp2447) == Cyc_Absyn_UnknownTunion){ _LL2454: _temp2453=( struct Cyc_Absyn_UnknownTunionInfo)((
struct Cyc_Absyn_UnknownTunion_struct*) _temp2447)->f1; _LL2458: _temp2457=(
struct _tuple1*) _temp2453.name; goto _LL2456; _LL2456: _temp2455=( int)
_temp2453.is_xtunion; goto _LL2450;} else{ goto _LL2451;} _LL2451: if(*(( int*)
_temp2447) == Cyc_Absyn_KnownTunion){ _LL2460: _temp2459=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunion_struct*) _temp2447)->f1; goto _LL2452;} else{ goto
_LL2448;} _LL2450: { struct Cyc_Absyn_Tuniondecl* tud;{ struct _handler_cons
_temp2461; _push_handler(& _temp2461);{ int _temp2463= 0; if( setjmp( _temp2461.handler)){
_temp2463= 1;} if( ! _temp2463){ tud=* Cyc_Tcenv_lookup_tuniondecl( te, loc,
_temp2457);; _pop_handler();} else{ void* _temp2462=( void*) _exn_thrown; void*
_temp2465= _temp2462; _LL2467: if( _temp2465 == Cyc_Dict_Absent){ goto _LL2468;}
else{ goto _LL2469;} _LL2469: goto _LL2470; _LL2468: Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2471= Cyc_Absynpp_qvar2string( _temp2457); xprintf("unbound type tunion %.*s",
_temp2471.last_plus_one - _temp2471.curr, _temp2471.curr);})); return free_vars;
_LL2470:( void) _throw( _temp2465); _LL2466:;}}} if( tud->is_xtunion !=
_temp2455){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2472= Cyc_Absynpp_qvar2string(
_temp2457); xprintf("[x]tunion is different from type declaration %.*s",
_temp2472.last_plus_one - _temp2472.curr, _temp2472.curr);}));}* _temp2352=(
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp2473=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp2473[ 0]=({
struct Cyc_Absyn_KnownTunion_struct _temp2474; _temp2474.tag= Cyc_Absyn_KnownTunion;
_temp2474.f1= tud; _temp2474;}); _temp2473;}); _temp2459= tud; goto _LL2452;}
_LL2452: free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars,( void*) Cyc_Absyn_RgnKind,
_temp2346);{ struct Cyc_List_List* tvs= _temp2459->tvs; for( 0; _temp2348 != 0?
tvs != 0: 0; _temp2348=({ struct Cyc_List_List* _temp2475= _temp2348; if(
_temp2475 == 0){ _throw( Null_Exception);} _temp2475->tl;}), tvs=({ struct Cyc_List_List*
_temp2476= tvs; if( _temp2476 == 0){ _throw( Null_Exception);} _temp2476->tl;})){
void* t1=( void*)({ struct Cyc_List_List* _temp2478= _temp2348; if( _temp2478 ==
0){ _throw( Null_Exception);} _temp2478->hd;}); void* k1=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp2477= tvs; if( _temp2477 == 0){ _throw( Null_Exception);} _temp2477->hd;}))->kind);
free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars, k1, t1);} if(
_temp2348 != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2479= Cyc_Absynpp_qvar2string(
_temp2459->name); xprintf("too many type arguments for tunion %.*s", _temp2479.last_plus_one
- _temp2479.curr, _temp2479.curr);}));} if( tvs != 0){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2480= Cyc_Absynpp_qvar2string( _temp2459->name);
xprintf("too few type arguments for tunion %.*s", _temp2480.last_plus_one -
_temp2480.curr, _temp2480.curr);}));} goto _LL2448;} _LL2448:;} goto _LL2286;
_LL2298:{ void* _temp2481=* _temp2359; struct Cyc_Absyn_UnknownTunionFieldInfo
_temp2487; int _temp2489; struct _tuple1* _temp2491; struct _tuple1* _temp2493;
struct Cyc_Absyn_Tunionfield* _temp2495; struct Cyc_Absyn_Tuniondecl* _temp2497;
_LL2483: if(*(( int*) _temp2481) == Cyc_Absyn_UnknownTunionfield){ _LL2488:
_temp2487=( struct Cyc_Absyn_UnknownTunionFieldInfo)(( struct Cyc_Absyn_UnknownTunionfield_struct*)
_temp2481)->f1; _LL2494: _temp2493=( struct _tuple1*) _temp2487.tunion_name;
goto _LL2492; _LL2492: _temp2491=( struct _tuple1*) _temp2487.field_name; goto
_LL2490; _LL2490: _temp2489=( int) _temp2487.is_xtunion; goto _LL2484;} else{
goto _LL2485;} _LL2485: if(*(( int*) _temp2481) == Cyc_Absyn_KnownTunionfield){
_LL2498: _temp2497=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp2481)->f1; goto _LL2496; _LL2496: _temp2495=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp2481)->f2; goto _LL2486;} else{
goto _LL2482;} _LL2484: { struct Cyc_Absyn_Tuniondecl* tud; struct Cyc_Absyn_Tunionfield*
tuf;{ struct _handler_cons _temp2499; _push_handler(& _temp2499);{ int _temp2501=
0; if( setjmp( _temp2499.handler)){ _temp2501= 1;} if( ! _temp2501){ tud=* Cyc_Tcenv_lookup_tuniondecl(
te, loc, _temp2493);; _pop_handler();} else{ void* _temp2500=( void*)
_exn_thrown; void* _temp2503= _temp2500; _LL2505: if( _temp2503 == Cyc_Dict_Absent){
goto _LL2506;} else{ goto _LL2507;} _LL2507: goto _LL2508; _LL2506: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2509= Cyc_Absynpp_qvar2string( _temp2493);
xprintf("unbound type tunion %.*s", _temp2509.last_plus_one - _temp2509.curr,
_temp2509.curr);})); return free_vars; _LL2508:( void) _throw( _temp2503);
_LL2504:;}}} if( tud->is_xtunion != _temp2489){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2510= Cyc_Absynpp_qvar2string( _temp2493); xprintf("[x]tunion is different from type declaration %.*s",
_temp2510.last_plus_one - _temp2510.curr, _temp2510.curr);}));}{ struct
_handler_cons _temp2511; _push_handler(& _temp2511);{ int _temp2513= 0; if(
setjmp( _temp2511.handler)){ _temp2513= 1;} if( ! _temp2513){{ void* _temp2514=
Cyc_Tcenv_lookup_ordinary( te, loc, _temp2491); struct Cyc_Absyn_Tunionfield*
_temp2520; struct Cyc_Absyn_Tuniondecl* _temp2522; _LL2516: if(*(( int*)
_temp2514) == Cyc_Tcenv_TunionRes){ _LL2523: _temp2522=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Tcenv_TunionRes_struct*) _temp2514)->f1; goto _LL2521; _LL2521:
_temp2520=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Tcenv_TunionRes_struct*)
_temp2514)->f2; goto _LL2517;} else{ goto _LL2518;} _LL2518: goto _LL2519;
_LL2517: tuf= _temp2520; if( _temp2522 != tud){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2524= Cyc_Absynpp_qvar2string( _temp2491); struct
_tagged_string _temp2525= Cyc_Absynpp_qvar2string( _temp2493); xprintf("unbound field %.*s in type tunion %.*s",
_temp2524.last_plus_one - _temp2524.curr, _temp2524.curr, _temp2525.last_plus_one
- _temp2525.curr, _temp2525.curr);}));{ struct Cyc_List_List* _temp2526=
free_vars; _npop_handler( 0u); return _temp2526;}} goto _LL2515; _LL2519: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2527= Cyc_Absynpp_qvar2string( _temp2491);
struct _tagged_string _temp2528= Cyc_Absynpp_qvar2string( _temp2493); xprintf("unbound field %.*s in type tunion %.*s",
_temp2527.last_plus_one - _temp2527.curr, _temp2527.curr, _temp2528.last_plus_one
- _temp2528.curr, _temp2528.curr);}));{ struct Cyc_List_List* _temp2529=
free_vars; _npop_handler( 0u); return _temp2529;} _LL2515:;}; _pop_handler();}
else{ void* _temp2512=( void*) _exn_thrown; void* _temp2531= _temp2512; _LL2533:
if( _temp2531 == Cyc_Dict_Absent){ goto _LL2534;} else{ goto _LL2535;} _LL2535:
goto _LL2536; _LL2534: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2537=
Cyc_Absynpp_qvar2string( _temp2491); struct _tagged_string _temp2538= Cyc_Absynpp_qvar2string(
_temp2493); xprintf("unbound field %.*s in type tunion %.*s", _temp2537.last_plus_one
- _temp2537.curr, _temp2537.curr, _temp2538.last_plus_one - _temp2538.curr,
_temp2538.curr);})); return free_vars; _LL2536:( void) _throw( _temp2531);
_LL2532:;}}}* _temp2359=( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp2539=( struct Cyc_Absyn_KnownTunionfield_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp2539[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp2540; _temp2540.tag= Cyc_Absyn_KnownTunionfield; _temp2540.f1= tud;
_temp2540.f2= tuf; _temp2540;}); _temp2539;}); _temp2497= tud; _temp2495= tuf;
goto _LL2486;} _LL2486: { struct Cyc_List_List* tvs= _temp2497->tvs; for( 0;
_temp2355 != 0? tvs != 0: 0; _temp2355=({ struct Cyc_List_List* _temp2541=
_temp2355; if( _temp2541 == 0){ _throw( Null_Exception);} _temp2541->tl;}), tvs=({
struct Cyc_List_List* _temp2542= tvs; if( _temp2542 == 0){ _throw(
Null_Exception);} _temp2542->tl;})){ void* t1=( void*)({ struct Cyc_List_List*
_temp2544= _temp2355; if( _temp2544 == 0){ _throw( Null_Exception);} _temp2544->hd;});
void* k1=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(((
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp2543= tvs; if( _temp2543 ==
0){ _throw( Null_Exception);} _temp2543->hd;}))->kind); free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars, k1, t1);} if( _temp2355 != 0){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2545= Cyc_Absynpp_qvar2string( _temp2497->name);
struct _tagged_string _temp2546= Cyc_Absynpp_qvar2string( _temp2495->name);
xprintf("too many type arguments for tunion %.*s.%.*s", _temp2545.last_plus_one
- _temp2545.curr, _temp2545.curr, _temp2546.last_plus_one - _temp2546.curr,
_temp2546.curr);}));} if( tvs != 0){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2547= Cyc_Absynpp_qvar2string( _temp2497->name); struct
_tagged_string _temp2548= Cyc_Absynpp_qvar2string( _temp2495->name); xprintf("too few type arguments for tunion %.*s.%.*s",
_temp2547.last_plus_one - _temp2547.curr, _temp2547.curr, _temp2548.last_plus_one
- _temp2548.curr, _temp2548.curr);}));} goto _LL2482;} _LL2482:;} goto _LL2286;
_LL2300: free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars,( void*) Cyc_Absyn_AnyKind,
_temp2370); free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars,( void*)
Cyc_Absyn_RgnKind, _temp2368);{ void* _temp2549=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp2362))->v; void*
_temp2555; struct Cyc_Absyn_Exp* _temp2557; _LL2551: if(( unsigned int)
_temp2549 > 1u?*(( int*) _temp2549) == Cyc_Absyn_Eq_constr: 0){ _LL2556:
_temp2555=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2549)->f1; if((
unsigned int) _temp2555 > 1u?*(( int*) _temp2555) == Cyc_Absyn_Upper_b: 0){
_LL2558: _temp2557=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp2555)->f1; goto _LL2552;} else{ goto _LL2553;}} else{ goto _LL2553;}
_LL2553: goto _LL2554; _LL2552: if( ! Cyc_Tcutil_is_const_exp( te, _temp2557)){
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp2559=( char*)"pointer bounds expression is not a constant";
struct _tagged_string _temp2560; _temp2560.curr= _temp2559; _temp2560.base=
_temp2559; _temp2560.last_plus_one= _temp2559 + 44; _temp2560;}));} Cyc_Tcexp_tcExp(
te, 0, _temp2557); if( ! Cyc_Tcutil_coerce_uint_typ( te, _temp2557)){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp2561=( char*)"pointer bounds expression is not an unsigned int";
struct _tagged_string _temp2562; _temp2562.curr= _temp2561; _temp2562.base=
_temp2561; _temp2562.last_plus_one= _temp2561 + 49; _temp2562;}));} Cyc_Evexp_eval_const_uint_exp(
_temp2557); goto _LL2550; _LL2554: goto _LL2550; _LL2550:;} goto _LL2286;
_LL2302: goto _LL2286; _LL2304: goto _LL2286; _LL2306: goto _LL2286; _LL2308:
free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars,( void*) Cyc_Absyn_MemKind,
_temp2380); if( _temp2376 == 0? 1: ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp2563= _temp2376; if( _temp2563 == 0){ _throw(
Null_Exception);} _temp2563;}))){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({
char* _temp2564=( char*)"array bounds expression is not constant"; struct
_tagged_string _temp2565; _temp2565.curr= _temp2564; _temp2565.base= _temp2564;
_temp2565.last_plus_one= _temp2564 + 40; _temp2565;}));} Cyc_Tcexp_tcExp( te, 0,(
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2566= _temp2376; if(
_temp2566 == 0){ _throw( Null_Exception);} _temp2566;})); if( ! Cyc_Tcutil_coerce_uint_typ(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2567= _temp2376; if(
_temp2567 == 0){ _throw( Null_Exception);} _temp2567;}))){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp2568=( char*)"array bounds expression is not an unsigned int";
struct _tagged_string _temp2569; _temp2569.curr= _temp2568; _temp2569.base=
_temp2568; _temp2569.last_plus_one= _temp2568 + 47; _temp2569;}));} Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2570= _temp2376; if(
_temp2570 == 0){ _throw( Null_Exception);} _temp2570;})); goto _LL2286; _LL2310:
if( _temp2384 != 0){ int seen_cdecl= 0; int seen_stdcall= 0; for( 0; _temp2384
!= 0; _temp2384=({ struct Cyc_List_List* _temp2571= _temp2384; if( _temp2571 ==
0){ _throw( Null_Exception);} _temp2571->tl;})){ if( ! Cyc_Tcutil_fntype_att((
void*)({ struct Cyc_List_List* _temp2572= _temp2384; if( _temp2572 == 0){ _throw(
Null_Exception);} _temp2572->hd;}))){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2574= Cyc_Absyn_attribute2string(( void*)({ struct Cyc_List_List*
_temp2573= _temp2384; if( _temp2573 == 0){ _throw( Null_Exception);} _temp2573->hd;}));
xprintf("bad function type attribute %.*s", _temp2574.last_plus_one - _temp2574.curr,
_temp2574.curr);}));} if(( void*)({ struct Cyc_List_List* _temp2575= _temp2384;
if( _temp2575 == 0){ _throw( Null_Exception);} _temp2575->hd;}) ==( void*) Cyc_Absyn_Stdcall_att){
if( seen_cdecl){ Cyc_Tcutil_terr( loc, xprintf("function can't be both cdecl and stdcall"));}
seen_stdcall= 1;} else{ if(( void*)({ struct Cyc_List_List* _temp2576= _temp2384;
if( _temp2576 == 0){ _throw( Null_Exception);} _temp2576->hd;}) ==( void*) Cyc_Absyn_Cdecl_att){
if( seen_stdcall){ Cyc_Tcutil_terr( loc, xprintf("function can't be both cdecl and stdcall"));}
seen_cdecl= 1;}}}} Cyc_Tcutil_check_unique_tvars( loc,* _temp2397);{ struct Cyc_List_List*
b=* _temp2397; for( 0; b != 0; b=({ struct Cyc_List_List* _temp2577= b; if(
_temp2577 == 0){ _throw( Null_Exception);} _temp2577->tl;})){ void* _temp2579=(
void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp2578= b; if( _temp2578 == 0){
_throw( Null_Exception);} _temp2578->hd;}))->kind))->v; void* _temp2585; _LL2581:
if(( unsigned int) _temp2579 > 1u?*(( int*) _temp2579) == Cyc_Absyn_Eq_constr: 0){
_LL2586: _temp2585=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2579)->f1;
if( _temp2585 ==( void*) Cyc_Absyn_MemKind){ goto _LL2582;} else{ goto _LL2583;}}
else{ goto _LL2583;} _LL2583: goto _LL2584; _LL2582: Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2588=*(( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp2587= b; if( _temp2587 == 0){ _throw( Null_Exception);} _temp2587->hd;}))->name;
xprintf("function attempts to abstract Mem type variable %.*s", _temp2588.last_plus_one
- _temp2588.curr, _temp2588.curr);})); goto _LL2580; _LL2584: goto _LL2580;
_LL2580:;}}{ struct Cyc_List_List* new_free_vars= 0; new_free_vars= Cyc_Tcutil_check_valid_type(
loc, te, new_free_vars,( void*) Cyc_Absyn_MemKind, _temp2390); for( 0; _temp2388
!= 0; _temp2388=({ struct Cyc_List_List* _temp2589= _temp2388; if( _temp2589 ==
0){ _throw( Null_Exception);} _temp2589->tl;})){ new_free_vars= Cyc_Tcutil_check_valid_type(
loc, te, new_free_vars,( void*) Cyc_Absyn_MemKind,(*(( struct _tuple2*)({ struct
Cyc_List_List* _temp2590= _temp2388; if( _temp2590 == 0){ _throw( Null_Exception);}
_temp2590->hd;}))).f3);} if(* _temp2394 != 0){ new_free_vars= Cyc_Tcutil_check_valid_type(
loc, te, new_free_vars,( void*) Cyc_Absyn_EffKind,( void*)({ struct Cyc_Core_Opt*
_temp2591=* _temp2394; if( _temp2591 == 0){ _throw( Null_Exception);} _temp2591->v;}));}
else{ struct Cyc_Absyn_Tvar* e= Cyc_Tcutil_new_tvar(( void*) Cyc_Absyn_EffKind);
new_free_vars=({ struct Cyc_List_List* _temp2592=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2592->hd=( void*) e; _temp2592->tl=
new_free_vars; _temp2592;});{ struct Cyc_List_List* effect= 0;{ struct Cyc_List_List*
tvs= new_free_vars; for( 0; tvs != 0; tvs=({ struct Cyc_List_List* _temp2593=
tvs; if( _temp2593 == 0){ _throw( Null_Exception);} _temp2593->tl;})){ void*
_temp2595=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp2594= tvs; if( _temp2594 == 0){ _throw( Null_Exception);} _temp2594->hd;}))->kind))->v;
void* _temp2603; void* _temp2605; _LL2597: if(( unsigned int) _temp2595 > 1u?*((
int*) _temp2595) == Cyc_Absyn_Eq_constr: 0){ _LL2604: _temp2603=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp2595)->f1; if( _temp2603 ==( void*) Cyc_Absyn_RgnKind){
goto _LL2598;} else{ goto _LL2599;}} else{ goto _LL2599;} _LL2599: if((
unsigned int) _temp2595 > 1u?*(( int*) _temp2595) == Cyc_Absyn_Eq_constr: 0){
_LL2606: _temp2605=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2595)->f1;
if( _temp2605 ==( void*) Cyc_Absyn_EffKind){ goto _LL2600;} else{ goto _LL2601;}}
else{ goto _LL2601;} _LL2601: goto _LL2602; _LL2598: effect=({ struct Cyc_List_List*
_temp2607=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2607->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2608=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2608[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2609; _temp2609.tag= Cyc_Absyn_AccessEff;
_temp2609.f1=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp2610=(
struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2610[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2611; _temp2611.tag= Cyc_Absyn_VarType;
_temp2611.f1=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp2612= tvs;
if( _temp2612 == 0){ _throw( Null_Exception);} _temp2612->hd;}); _temp2611;});
_temp2610;})); _temp2609;}); _temp2608;})); _temp2607->tl= effect; _temp2607;});
goto _LL2596; _LL2600: effect=({ struct Cyc_List_List* _temp2613=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2613->hd=( void*)(( void*)({
struct Cyc_Absyn_VarType_struct* _temp2614=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp2614[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp2615; _temp2615.tag= Cyc_Absyn_VarType; _temp2615.f1=(
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp2616= tvs; if( _temp2616 ==
0){ _throw( Null_Exception);} _temp2616->hd;}); _temp2615;}); _temp2614;}));
_temp2613->tl= effect; _temp2613;}); goto _LL2596; _LL2602: goto _LL2596;
_LL2596:;}}* _temp2394=({ struct Cyc_Core_Opt* _temp2617=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2617->v=( void*)(( void*)({
struct Cyc_Absyn_JoinEff_struct* _temp2618=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp2618[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp2619; _temp2619.tag= Cyc_Absyn_JoinEff; _temp2619.f1=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( effect);
_temp2619;}); _temp2618;})); _temp2617;});}} if(* _temp2397 != 0){ struct Cyc_List_List*
new_btvs= 0;{ struct Cyc_List_List* bs=* _temp2397; for( 0; bs != 0; bs=({
struct Cyc_List_List* _temp2620= bs; if( _temp2620 == 0){ _throw( Null_Exception);}
_temp2620->tl;})){ new_free_vars= Cyc_Tcutil_add_free_tvar( loc, new_free_vars,(
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp2621= bs; if( _temp2621 ==
0){ _throw( Null_Exception);} _temp2621->hd;}));{ struct Cyc_Absyn_Conref* c=((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp2636= bs; if( _temp2636 ==
0){ _throw( Null_Exception);} _temp2636->hd;}))->kind); void* _temp2622=( void*)
c->v; void* _temp2630; _LL2624: if( _temp2622 ==( void*) Cyc_Absyn_No_constr){
goto _LL2625;} else{ goto _LL2626;} _LL2626: if(( unsigned int) _temp2622 > 1u?*((
int*) _temp2622) == Cyc_Absyn_Eq_constr: 0){ _LL2631: _temp2630=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp2622)->f1; if( _temp2630 ==( void*) Cyc_Absyn_MemKind){
goto _LL2627;} else{ goto _LL2628;}} else{ goto _LL2628;} _LL2628: goto _LL2629;
_LL2625: Cyc_Tcutil_warn( loc,({ struct _tagged_string _temp2633=*(( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp2632= bs; if( _temp2632 == 0){ _throw( Null_Exception);}
_temp2632->hd;}))->name; xprintf("Type variable %.*s unconstrained, assuming boxed",
_temp2633.last_plus_one - _temp2633.curr, _temp2633.curr);})); goto _LL2627;
_LL2627:( void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp2634=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2634[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp2635; _temp2635.tag= Cyc_Absyn_Eq_constr;
_temp2635.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp2635;}); _temp2634;})));
goto _LL2623; _LL2629: goto _LL2623; _LL2623:;}}} new_free_vars= Cyc_Tcutil_remove_bound_tvars(
new_free_vars,* _temp2397);} for( 0; new_free_vars != 0; new_free_vars=({ struct
Cyc_List_List* _temp2637= new_free_vars; if( _temp2637 == 0){ _throw(
Null_Exception);} _temp2637->tl;})){ free_vars= Cyc_Tcutil_add_free_tvar( loc,
free_vars,( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp2638=
new_free_vars; if( _temp2638 == 0){ _throw( Null_Exception);} _temp2638->hd;}));}
goto _LL2286;} _LL2312: for( 0; _temp2398 != 0; _temp2398=({ struct Cyc_List_List*
_temp2639= _temp2398; if( _temp2639 == 0){ _throw( Null_Exception);} _temp2639->tl;})){
free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars,( void*) Cyc_Absyn_MemKind,(*((
struct _tuple4*)({ struct Cyc_List_List* _temp2640= _temp2398; if( _temp2640 ==
0){ _throw( Null_Exception);} _temp2640->hd;}))).f2);} goto _LL2286; _LL2314:{
struct _RegionHandle _temp2641= _new_region(); struct _RegionHandle* sprev_rgn=&
_temp2641; _push_region( sprev_rgn);{ struct Cyc_List_List* prev_fields= 0; for(
0; _temp2400 != 0; _temp2400=({ struct Cyc_List_List* _temp2642= _temp2400; if(
_temp2642 == 0){ _throw( Null_Exception);} _temp2642->tl;})){ struct Cyc_Absyn_Structfield
_temp2646; struct Cyc_List_List* _temp2647; struct Cyc_Core_Opt* _temp2649; void*
_temp2651; struct Cyc_Absyn_Tqual _temp2653; struct _tagged_string* _temp2655;
struct Cyc_Absyn_Structfield* _temp2644=( struct Cyc_Absyn_Structfield*)({
struct Cyc_List_List* _temp2643= _temp2400; if( _temp2643 == 0){ _throw(
Null_Exception);} _temp2643->hd;}); _temp2646=* _temp2644; _LL2656: _temp2655=(
struct _tagged_string*) _temp2646.name; goto _LL2654; _LL2654: _temp2653=(
struct Cyc_Absyn_Tqual) _temp2646.tq; goto _LL2652; _LL2652: _temp2651=( void*)
_temp2646.type; goto _LL2650; _LL2650: _temp2649=( struct Cyc_Core_Opt*)
_temp2646.width; goto _LL2648; _LL2648: _temp2647=( struct Cyc_List_List*)
_temp2646.attributes; goto _LL2645; _LL2645: if((( int(*)( int(* compare)(
struct _tagged_string*, struct _tagged_string*), struct Cyc_List_List* l, struct
_tagged_string* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp, prev_fields, _temp2655)){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2657=* _temp2655; xprintf("duplicate field %.*s in struct",
_temp2657.last_plus_one - _temp2657.curr, _temp2657.curr);}));} if( Cyc_String_strcmp(*
_temp2655,( struct _tagged_string)({ char* _temp2658=( char*)""; struct
_tagged_string _temp2659; _temp2659.curr= _temp2658; _temp2659.base= _temp2658;
_temp2659.last_plus_one= _temp2658 + 1; _temp2659;})) != 0){ prev_fields=({
struct Cyc_List_List* _temp2660=( struct Cyc_List_List*) _region_malloc(
sprev_rgn, sizeof( struct Cyc_List_List)); _temp2660->hd=( void*) _temp2655;
_temp2660->tl= prev_fields; _temp2660;});} Cyc_Tcutil_check_valid_type( loc, te,
free_vars,( void*) Cyc_Absyn_MemKind, _temp2651); if( _temp2649 != 0){
unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)({
struct Cyc_Core_Opt* _temp2661= _temp2649; if( _temp2661 == 0){ _throw(
Null_Exception);} _temp2661->v;}))){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2662=* _temp2655; xprintf("bitfield %.*s does not have constant width",
_temp2662.last_plus_one - _temp2662.curr, _temp2662.curr);}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp2663= _temp2649; if(
_temp2663 == 0){ _throw( Null_Exception);} _temp2663->v;}));}{ void* _temp2664=
Cyc_Tcutil_compress( _temp2651); void* _temp2670; void* _temp2672; _LL2666: if((
unsigned int) _temp2664 > 4u?*(( int*) _temp2664) == Cyc_Absyn_IntType: 0){
_LL2673: _temp2672=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2664)->f1;
goto _LL2671; _LL2671: _temp2670=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2664)->f2; goto _LL2667;} else{ goto _LL2668;} _LL2668: goto _LL2669;
_LL2667:{ void* _temp2674= _temp2670; _LL2676: if( _temp2674 ==( void*) Cyc_Absyn_B1){
goto _LL2677;} else{ goto _LL2678;} _LL2678: if( _temp2674 ==( void*) Cyc_Absyn_B2){
goto _LL2679;} else{ goto _LL2680;} _LL2680: if( _temp2674 ==( void*) Cyc_Absyn_B4){
goto _LL2681;} else{ goto _LL2682;} _LL2682: if( _temp2674 ==( void*) Cyc_Absyn_B8){
goto _LL2683;} else{ goto _LL2675;} _LL2677: if( w > 8){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp2684=( char*)"bitfield larger than type";
struct _tagged_string _temp2685; _temp2685.curr= _temp2684; _temp2685.base=
_temp2684; _temp2685.last_plus_one= _temp2684 + 26; _temp2685;}));} goto _LL2675;
_LL2679: if( w > 16){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp2686=( char*)"bitfield larger than type"; struct _tagged_string _temp2687;
_temp2687.curr= _temp2686; _temp2687.base= _temp2686; _temp2687.last_plus_one=
_temp2686 + 26; _temp2687;}));} goto _LL2675; _LL2681: if( w > 32){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp2688=( char*)"bitfield larger than type";
struct _tagged_string _temp2689; _temp2689.curr= _temp2688; _temp2689.base=
_temp2688; _temp2689.last_plus_one= _temp2688 + 26; _temp2689;}));} goto _LL2675;
_LL2683: if( w > 64){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp2690=( char*)"bitfield larger than type"; struct _tagged_string _temp2691;
_temp2691.curr= _temp2690; _temp2691.base= _temp2690; _temp2691.last_plus_one=
_temp2690 + 26; _temp2691;}));} goto _LL2675; _LL2675:;} goto _LL2665; _LL2669:
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2692=* _temp2655; struct
_tagged_string _temp2693= Cyc_Absynpp_typ2string( _temp2651); xprintf("bitfield %.*s must have integral type but has type %.*s",
_temp2692.last_plus_one - _temp2692.curr, _temp2692.curr, _temp2693.last_plus_one
- _temp2693.curr, _temp2693.curr);})); goto _LL2665; _LL2665:;}} for( 0;
_temp2647 != 0; _temp2647=({ struct Cyc_List_List* _temp2694= _temp2647; if(
_temp2694 == 0){ _throw( Null_Exception);} _temp2694->tl;})){ void* _temp2696=(
void*)({ struct Cyc_List_List* _temp2695= _temp2647; if( _temp2695 == 0){ _throw(
Null_Exception);} _temp2695->hd;}); int _temp2704; _LL2698: if( _temp2696 ==(
void*) Cyc_Absyn_Packed_att){ goto _LL2699;} else{ goto _LL2700;} _LL2700: if((
unsigned int) _temp2696 > 15u?*(( int*) _temp2696) == Cyc_Absyn_Aligned_att: 0){
_LL2705: _temp2704=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp2696)->f1;
goto _LL2701;} else{ goto _LL2702;} _LL2702: goto _LL2703; _LL2699: continue;
_LL2701: continue; _LL2703: Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2707= Cyc_Absyn_attribute2string(( void*)({ struct Cyc_List_List* _temp2706=
_temp2647; if( _temp2706 == 0){ _throw( Null_Exception);} _temp2706->hd;}));
struct _tagged_string _temp2708=* _temp2655; xprintf("bad attribute %.*s on struct member %.*s",
_temp2707.last_plus_one - _temp2707.curr, _temp2707.curr, _temp2708.last_plus_one
- _temp2708.curr, _temp2708.curr);})); goto _LL2697; _LL2697:;}}}; _pop_region(&
_temp2641);} goto _LL2286; _LL2316:{ struct _RegionHandle _temp2709= _new_region();
struct _RegionHandle* uprev_rgn=& _temp2709; _push_region( uprev_rgn);{ struct
Cyc_List_List* prev_fields= 0; for( 0; _temp2402 != 0; _temp2402=({ struct Cyc_List_List*
_temp2710= _temp2402; if( _temp2710 == 0){ _throw( Null_Exception);} _temp2710->tl;})){
struct Cyc_Absyn_Structfield _temp2714; struct Cyc_List_List* _temp2715; struct
Cyc_Core_Opt* _temp2717; void* _temp2719; struct Cyc_Absyn_Tqual _temp2721;
struct _tagged_string* _temp2723; struct Cyc_Absyn_Structfield* _temp2712=(
struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp2711= _temp2402; if(
_temp2711 == 0){ _throw( Null_Exception);} _temp2711->hd;}); _temp2714=*
_temp2712; _LL2724: _temp2723=( struct _tagged_string*) _temp2714.name; goto
_LL2722; _LL2722: _temp2721=( struct Cyc_Absyn_Tqual) _temp2714.tq; goto _LL2720;
_LL2720: _temp2719=( void*) _temp2714.type; goto _LL2718; _LL2718: _temp2717=(
struct Cyc_Core_Opt*) _temp2714.width; goto _LL2716; _LL2716: _temp2715=( struct
Cyc_List_List*) _temp2714.attributes; goto _LL2713; _LL2713: if((( int(*)( int(*
compare)( struct _tagged_string*, struct _tagged_string*), struct Cyc_List_List*
l, struct _tagged_string* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp, prev_fields,
_temp2723)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2725=* _temp2723;
xprintf("duplicate field %.*s in struct", _temp2725.last_plus_one - _temp2725.curr,
_temp2725.curr);}));} if( Cyc_String_strcmp(* _temp2723,( struct _tagged_string)({
char* _temp2726=( char*)""; struct _tagged_string _temp2727; _temp2727.curr=
_temp2726; _temp2727.base= _temp2726; _temp2727.last_plus_one= _temp2726 + 1;
_temp2727;})) != 0){ prev_fields=({ struct Cyc_List_List* _temp2728=( struct Cyc_List_List*)
_region_malloc( uprev_rgn, sizeof( struct Cyc_List_List)); _temp2728->hd=( void*)
_temp2723; _temp2728->tl= prev_fields; _temp2728;});} Cyc_Tcutil_check_valid_type(
loc, te, free_vars,( void*) Cyc_Absyn_MemKind, _temp2719); if( ! Cyc_Tcutil_bits_only(
_temp2719)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2729=* _temp2723;
xprintf("%.*s has a type that is possibly incompatible with other members of union",
_temp2729.last_plus_one - _temp2729.curr, _temp2729.curr);}));} if( _temp2717 !=
0){ unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)({
struct Cyc_Core_Opt* _temp2730= _temp2717; if( _temp2730 == 0){ _throw(
Null_Exception);} _temp2730->v;}))){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2731=* _temp2723; xprintf("bitfield %.*s does not have constant width",
_temp2731.last_plus_one - _temp2731.curr, _temp2731.curr);}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp2732= _temp2717; if(
_temp2732 == 0){ _throw( Null_Exception);} _temp2732->v;}));}{ void* _temp2733=
Cyc_Tcutil_compress( _temp2719); void* _temp2739; void* _temp2741; _LL2735: if((
unsigned int) _temp2733 > 4u?*(( int*) _temp2733) == Cyc_Absyn_IntType: 0){
_LL2742: _temp2741=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2733)->f1;
goto _LL2740; _LL2740: _temp2739=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2733)->f2; goto _LL2736;} else{ goto _LL2737;} _LL2737: goto _LL2738;
_LL2736:{ void* _temp2743= _temp2739; _LL2745: if( _temp2743 ==( void*) Cyc_Absyn_B1){
goto _LL2746;} else{ goto _LL2747;} _LL2747: if( _temp2743 ==( void*) Cyc_Absyn_B2){
goto _LL2748;} else{ goto _LL2749;} _LL2749: if( _temp2743 ==( void*) Cyc_Absyn_B4){
goto _LL2750;} else{ goto _LL2751;} _LL2751: if( _temp2743 ==( void*) Cyc_Absyn_B8){
goto _LL2752;} else{ goto _LL2744;} _LL2746: if( w > 8){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp2753=( char*)"bitfield larger than type";
struct _tagged_string _temp2754; _temp2754.curr= _temp2753; _temp2754.base=
_temp2753; _temp2754.last_plus_one= _temp2753 + 26; _temp2754;}));} goto _LL2744;
_LL2748: if( w > 16){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp2755=( char*)"bitfield larger than type"; struct _tagged_string _temp2756;
_temp2756.curr= _temp2755; _temp2756.base= _temp2755; _temp2756.last_plus_one=
_temp2755 + 26; _temp2756;}));} goto _LL2744; _LL2750: if( w > 32){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp2757=( char*)"bitfield larger than type";
struct _tagged_string _temp2758; _temp2758.curr= _temp2757; _temp2758.base=
_temp2757; _temp2758.last_plus_one= _temp2757 + 26; _temp2758;}));} goto _LL2744;
_LL2752: if( w > 64){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp2759=( char*)"bitfield larger than type"; struct _tagged_string _temp2760;
_temp2760.curr= _temp2759; _temp2760.base= _temp2759; _temp2760.last_plus_one=
_temp2759 + 26; _temp2760;}));} goto _LL2744; _LL2744:;} goto _LL2734; _LL2738:
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2761=* _temp2723; struct
_tagged_string _temp2762= Cyc_Absynpp_typ2string( _temp2719); xprintf("bitfield %.*s must have integral type but has type %.*s",
_temp2761.last_plus_one - _temp2761.curr, _temp2761.curr, _temp2762.last_plus_one
- _temp2762.curr, _temp2762.curr);})); goto _LL2734; _LL2734:;}} for( 0;
_temp2715 != 0; _temp2715=({ struct Cyc_List_List* _temp2763= _temp2715; if(
_temp2763 == 0){ _throw( Null_Exception);} _temp2763->tl;})){ void* _temp2765=(
void*)({ struct Cyc_List_List* _temp2764= _temp2715; if( _temp2764 == 0){ _throw(
Null_Exception);} _temp2764->hd;}); int _temp2773; _LL2767: if( _temp2765 ==(
void*) Cyc_Absyn_Packed_att){ goto _LL2768;} else{ goto _LL2769;} _LL2769: if((
unsigned int) _temp2765 > 15u?*(( int*) _temp2765) == Cyc_Absyn_Aligned_att: 0){
_LL2774: _temp2773=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp2765)->f1;
goto _LL2770;} else{ goto _LL2771;} _LL2771: goto _LL2772; _LL2768: continue;
_LL2770: continue; _LL2772: Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2776= Cyc_Absyn_attribute2string(( void*)({ struct Cyc_List_List* _temp2775=
_temp2715; if( _temp2775 == 0){ _throw( Null_Exception);} _temp2775->hd;}));
struct _tagged_string _temp2777=* _temp2723; xprintf("bad attribute %.*s on struct member %.*s",
_temp2776.last_plus_one - _temp2776.curr, _temp2776.curr, _temp2777.last_plus_one
- _temp2777.curr, _temp2777.curr);})); goto _LL2766; _LL2766:;}}}; _pop_region(&
_temp2709);} goto _LL2286; _LL2318: if( _temp2409 == 0){ if(* _temp2406 == 0){
Cyc_Tcutil_terr( loc, xprintf("bad anonymous struct")); return free_vars;}{
struct Cyc_Absyn_Structdecl* sd=({ struct Cyc_Absyn_Structdecl** _temp2778=*
_temp2406; if( _temp2778 == 0){ _throw( Null_Exception);}* _temp2778;});;}} if(*
_temp2406 == 0){ struct _handler_cons _temp2779; _push_handler(& _temp2779);{
int _temp2781= 0; if( setjmp( _temp2779.handler)){ _temp2781= 1;} if( !
_temp2781){* _temp2406=( struct Cyc_Absyn_Structdecl**) Cyc_Tcenv_lookup_structdecl(
te, loc,( struct _tuple1*)({ struct _tuple1* _temp2782= _temp2409; if( _temp2782
== 0){ _throw( Null_Exception);} _temp2782;}));; _pop_handler();} else{ void*
_temp2780=( void*) _exn_thrown; void* _temp2784= _temp2780; _LL2786: if(
_temp2784 == Cyc_Dict_Absent){ goto _LL2787;} else{ goto _LL2788;} _LL2788: goto
_LL2789; _LL2787: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2791= Cyc_Absynpp_qvar2string((
struct _tuple1*)({ struct _tuple1* _temp2790= _temp2409; if( _temp2790 == 0){
_throw( Null_Exception);} _temp2790;})); xprintf("unbound type struct %.*s",
_temp2791.last_plus_one - _temp2791.curr, _temp2791.curr);})); return free_vars;
_LL2789:( void) _throw( _temp2784); _LL2785:;}}}{ struct Cyc_Absyn_Structdecl*
sd=({ struct Cyc_Absyn_Structdecl** _temp2804=* _temp2406; if( _temp2804 == 0){
_throw( Null_Exception);}* _temp2804;}); if( sd->name != 0){({ struct _tuple1*
_temp2792= _temp2409; if( _temp2792 == 0){ _throw( Null_Exception);}* _temp2792=({
struct _tuple1* _temp2794=( struct _tuple1*)({ struct Cyc_Core_Opt* _temp2793=
sd->name; if( _temp2793 == 0){ _throw( Null_Exception);} _temp2793->v;});
unsigned int _temp2795= 0; if( _temp2795 >= 1u){ _throw( Null_Exception);}
_temp2794[ _temp2795];});});}{ struct Cyc_List_List* tvs= sd->tvs; int lvs=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs); int largs=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp2407); if( lvs != largs){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2797= Cyc_Absynpp_qvar2string(( struct _tuple1*)({
struct _tuple1* _temp2796= _temp2409; if( _temp2796 == 0){ _throw(
Null_Exception);} _temp2796;})); int _temp2798= lvs; int _temp2799= largs;
xprintf("struct %.*s expects %d type arguments but was given %d", _temp2797.last_plus_one
- _temp2797.curr, _temp2797.curr, _temp2798, _temp2799);}));} for( 0; _temp2407
!= 0; _temp2407=({ struct Cyc_List_List* _temp2800= _temp2407; if( _temp2800 ==
0){ _throw( Null_Exception);} _temp2800->tl;}), tvs=({ struct Cyc_List_List*
_temp2801= tvs; if( _temp2801 == 0){ _throw( Null_Exception);} _temp2801->tl;})){
void* t1=( void*)({ struct Cyc_List_List* _temp2803= _temp2407; if( _temp2803 ==
0){ _throw( Null_Exception);} _temp2803->hd;}); void* k1=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp2802= tvs; if( _temp2802 == 0){ _throw( Null_Exception);} _temp2802->hd;}))->kind);
free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars, k1, t1);} goto
_LL2286;}} _LL2320: if( _temp2416 == 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp2805=( char*)"missing union name"; struct
_tagged_string _temp2806; _temp2806.curr= _temp2805; _temp2806.base= _temp2805;
_temp2806.last_plus_one= _temp2805 + 19; _temp2806;})); return free_vars;} if(*
_temp2413 == 0){ struct _handler_cons _temp2807; _push_handler(& _temp2807);{
int _temp2809= 0; if( setjmp( _temp2807.handler)){ _temp2809= 1;} if( !
_temp2809){* _temp2413=( struct Cyc_Absyn_Uniondecl**) Cyc_Tcenv_lookup_uniondecl(
te, loc,( struct _tuple1*)({ struct _tuple1* _temp2810= _temp2416; if( _temp2810
== 0){ _throw( Null_Exception);} _temp2810;}));; _pop_handler();} else{ void*
_temp2808=( void*) _exn_thrown; void* _temp2812= _temp2808; _LL2814: if(
_temp2812 == Cyc_Dict_Absent){ goto _LL2815;} else{ goto _LL2816;} _LL2816: goto
_LL2817; _LL2815: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2819= Cyc_Absynpp_qvar2string((
struct _tuple1*)({ struct _tuple1* _temp2818= _temp2416; if( _temp2818 == 0){
_throw( Null_Exception);} _temp2818;})); xprintf("unbound type union %.*s",
_temp2819.last_plus_one - _temp2819.curr, _temp2819.curr);})); return free_vars;
_LL2817:( void) _throw( _temp2812); _LL2813:;}}}{ struct Cyc_Absyn_Uniondecl* ud=({
struct Cyc_Absyn_Uniondecl** _temp2832=* _temp2413; if( _temp2832 == 0){ _throw(
Null_Exception);}* _temp2832;}); if( ud->name != 0){({ struct _tuple1* _temp2820=
_temp2416; if( _temp2820 == 0){ _throw( Null_Exception);}* _temp2820=({ struct
_tuple1* _temp2822=( struct _tuple1*)({ struct Cyc_Core_Opt* _temp2821= ud->name;
if( _temp2821 == 0){ _throw( Null_Exception);} _temp2821->v;}); unsigned int
_temp2823= 0; if( _temp2823 >= 1u){ _throw( Null_Exception);} _temp2822[
_temp2823];});});}{ struct Cyc_List_List* tvs= ud->tvs; int lvs=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( tvs); int largs=(( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)( _temp2414); if( lvs != largs){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2825= Cyc_Absynpp_qvar2string(( struct _tuple1*)({
struct _tuple1* _temp2824= _temp2416; if( _temp2824 == 0){ _throw(
Null_Exception);} _temp2824;})); int _temp2826= lvs; int _temp2827= largs;
xprintf("union %.*s expects %d type arguments but was given %d", _temp2825.last_plus_one
- _temp2825.curr, _temp2825.curr, _temp2826, _temp2827);}));} for( 0; _temp2414
!= 0; _temp2414=({ struct Cyc_List_List* _temp2828= _temp2414; if( _temp2828 ==
0){ _throw( Null_Exception);} _temp2828->tl;}), tvs=({ struct Cyc_List_List*
_temp2829= tvs; if( _temp2829 == 0){ _throw( Null_Exception);} _temp2829->tl;})){
void* t1=( void*)({ struct Cyc_List_List* _temp2831= _temp2414; if( _temp2831 ==
0){ _throw( Null_Exception);} _temp2831->hd;}); void* k1=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp2830= tvs; if( _temp2830 == 0){ _throw( Null_Exception);} _temp2830->hd;}))->kind);
free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars, k1, t1);} goto
_LL2286;}} _LL2322: { struct Cyc_Absyn_Typedefdecl* td;{ struct _handler_cons
_temp2833; _push_handler(& _temp2833);{ int _temp2835= 0; if( setjmp( _temp2833.handler)){
_temp2835= 1;} if( ! _temp2835){ td= Cyc_Tcenv_lookup_typedefdecl( te, loc,
_temp2423);; _pop_handler();} else{ void* _temp2834=( void*) _exn_thrown; void*
_temp2837= _temp2834; _LL2839: if( _temp2837 == Cyc_Dict_Absent){ goto _LL2840;}
else{ goto _LL2841;} _LL2841: goto _LL2842; _LL2840: Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2843= Cyc_Absynpp_qvar2string( _temp2423); xprintf("unbound typedef name %.*s",
_temp2843.last_plus_one - _temp2843.curr, _temp2843.curr);})); return free_vars;
_LL2842:( void) _throw( _temp2837); _LL2838:;}}}({ struct _tuple1* _temp2844=
_temp2423; unsigned int _temp2845= 0; if( _temp2845 >= 1u){ _throw(
Null_Exception);} _temp2844[ _temp2845]=({ struct _tuple1* _temp2846= td->name;
unsigned int _temp2847= 0; if( _temp2847 >= 1u){ _throw( Null_Exception);}
_temp2846[ _temp2847];});});{ struct Cyc_List_List* tvs= td->tvs; struct Cyc_List_List*
ts= _temp2421; struct Cyc_List_List* inst= 0; for( 0; ts != 0? tvs != 0: 0; ts=({
struct Cyc_List_List* _temp2848= ts; if( _temp2848 == 0){ _throw( Null_Exception);}
_temp2848->tl;}), tvs=({ struct Cyc_List_List* _temp2849= tvs; if( _temp2849 ==
0){ _throw( Null_Exception);} _temp2849->tl;})){ void* k=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp2855= tvs; if( _temp2855 == 0){ _throw( Null_Exception);} _temp2855->hd;}))->kind);
free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars, k,( void*)({ struct
Cyc_List_List* _temp2850= ts; if( _temp2850 == 0){ _throw( Null_Exception);}
_temp2850->hd;})); inst=({ struct Cyc_List_List* _temp2851=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2851->hd=( void*)({ struct
_tuple5* _temp2852=( struct _tuple5*) GC_malloc( sizeof( struct _tuple5));
_temp2852->f1=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp2854= tvs;
if( _temp2854 == 0){ _throw( Null_Exception);} _temp2854->hd;}); _temp2852->f2=(
void*)({ struct Cyc_List_List* _temp2853= ts; if( _temp2853 == 0){ _throw(
Null_Exception);} _temp2853->hd;}); _temp2852;}); _temp2851->tl= inst; _temp2851;});}
if( tvs != 0? 1: ts != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2856= Cyc_Absynpp_qvar2string( _temp2423); xprintf("typedef %.*s expects a different number of arguments",
_temp2856.last_plus_one - _temp2856.curr, _temp2856.curr);})); return free_vars;}{
void* new_typ= Cyc_Tcutil_substitute( inst,( void*) td->defn);* _temp2420=({
struct Cyc_Core_Opt* _temp2857=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2857->v=( void*) new_typ; _temp2857;}); goto _LL2286;}}}
_LL2324: goto _LL2286; _LL2326: _temp2427= _temp2425; goto _LL2328; _LL2328:
free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars,( void*) Cyc_Absyn_RgnKind,
_temp2427); goto _LL2286; _LL2330: for( 0; _temp2429 != 0; _temp2429=({ struct
Cyc_List_List* _temp2858= _temp2429; if( _temp2858 == 0){ _throw( Null_Exception);}
_temp2858->tl;})){ free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars,(
void*) Cyc_Absyn_EffKind,( void*)({ struct Cyc_List_List* _temp2859= _temp2429;
if( _temp2859 == 0){ _throw( Null_Exception);} _temp2859->hd;}));} goto _LL2286;
_LL2286:;} if( ! Cyc_Tcutil_kind_leq( Cyc_Tcutil_typ_kind( t), expected_kind)){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2860= Cyc_Absynpp_typ2string(
t); struct _tagged_string _temp2861= Cyc_Absynpp_kind2string( Cyc_Tcutil_typ_kind(
t)); struct _tagged_string _temp2862= Cyc_Absynpp_kind2string( expected_kind);
xprintf("type %.*s has kind %.*s but as used here needs kind %.*s", _temp2860.last_plus_one
- _temp2860.curr, _temp2860.curr, _temp2861.last_plus_one - _temp2861.curr,
_temp2861.curr, _temp2862.last_plus_one - _temp2862.curr, _temp2862.curr);}));}
return free_vars;} void Cyc_Tcutil_check_valid_toplevel_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, void* t){ struct Cyc_List_List* free_tvars= Cyc_Tcutil_check_valid_type(
loc, te, 0,( void*) Cyc_Absyn_MemKind, t);{ struct Cyc_List_List* x= free_tvars;
for( 0; x != 0; x=({ struct Cyc_List_List* _temp2863= x; if( _temp2863 == 0){
_throw( Null_Exception);} _temp2863->tl;})){ struct Cyc_Absyn_Conref* c=((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp2876= x; if( _temp2876 == 0){
_throw( Null_Exception);} _temp2876->hd;}))->kind); void* _temp2864=( void*) c->v;
void* _temp2872; _LL2866: if( _temp2864 ==( void*) Cyc_Absyn_No_constr){ goto
_LL2867;} else{ goto _LL2868;} _LL2868: if(( unsigned int) _temp2864 > 1u?*((
int*) _temp2864) == Cyc_Absyn_Eq_constr: 0){ _LL2873: _temp2872=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp2864)->f1; if( _temp2872 ==( void*) Cyc_Absyn_MemKind){
goto _LL2869;} else{ goto _LL2870;}} else{ goto _LL2870;} _LL2870: goto _LL2871;
_LL2867: goto _LL2869; _LL2869:( void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp2874=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2874[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp2875; _temp2875.tag= Cyc_Absyn_Eq_constr;
_temp2875.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp2875;}); _temp2874;})));
goto _LL2865; _LL2871: goto _LL2865; _LL2865:;}} if( free_tvars != 0){ void*
_temp2877= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo _temp2883; struct
Cyc_List_List* _temp2885; int _temp2887; struct Cyc_List_List* _temp2889; void*
_temp2891; struct Cyc_Core_Opt* _temp2893; struct Cyc_List_List* _temp2895;
struct Cyc_List_List** _temp2897; _LL2879: if(( unsigned int) _temp2877 > 4u?*((
int*) _temp2877) == Cyc_Absyn_FnType: 0){ _LL2884: _temp2883=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp2877)->f1; _LL2896: _temp2895=( struct Cyc_List_List*)
_temp2883.tvars; _temp2897=&((( struct Cyc_Absyn_FnType_struct*) _temp2877)->f1).tvars;
goto _LL2894; _LL2894: _temp2893=( struct Cyc_Core_Opt*) _temp2883.effect; goto
_LL2892; _LL2892: _temp2891=( void*) _temp2883.ret_typ; goto _LL2890; _LL2890:
_temp2889=( struct Cyc_List_List*) _temp2883.args; goto _LL2888; _LL2888:
_temp2887=( int) _temp2883.varargs; goto _LL2886; _LL2886: _temp2885=( struct
Cyc_List_List*) _temp2883.attributes; goto _LL2880;} else{ goto _LL2881;}
_LL2881: goto _LL2882; _LL2880: if(* _temp2897 == 0){* _temp2897= free_tvars;}
else{ if( free_tvars != 0){ struct _tagged_string s=*(( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp2902= free_tvars; if( _temp2902 == 0){ _throw(
Null_Exception);} _temp2902->hd;}))->name; if(({ struct _tagged_string _temp2898=
s; char* _temp2900= _temp2898.curr + 0; if( _temp2898.base == 0? 1:( _temp2900 <
_temp2898.base? 1: _temp2900 >= _temp2898.last_plus_one)){ _throw(
Null_Exception);}* _temp2900;}) =='?'){ Cyc_Tcutil_terr( loc, xprintf("You failed to abstract over the generated type variables. \nRemove the bound type variables or else make the effects of the type explicit\nand then abstract the free type variables properly."));}
else{ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2901= s; xprintf("unbound type variable %.*s",
_temp2901.last_plus_one - _temp2901.curr, _temp2901.curr);}));}}} goto _LL2878;
_LL2882: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2904=*(( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp2903= free_tvars; if( _temp2903 == 0){ _throw(
Null_Exception);} _temp2903->hd;}))->name; xprintf("unbound type variable %.*s",
_temp2904.last_plus_one - _temp2904.curr, _temp2904.curr);})); return; _LL2878:;}}
void Cyc_Tcutil_check_type( struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* bound_tvars, void* expected_kind, void* t){ struct Cyc_List_List*
free_tvars= Cyc_Tcutil_check_valid_type( loc, te, 0, expected_kind, t); struct
Cyc_List_List* fs= free_tvars; for( 0; fs != 0; fs=({ struct Cyc_List_List*
_temp2905= fs; if( _temp2905 == 0){ _throw( Null_Exception);} _temp2905->tl;})){
struct Cyc_Absyn_Tvar* ftv=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp2913= fs; if( _temp2913 == 0){ _throw( Null_Exception);} _temp2913->hd;});
struct _tagged_string* ftvn= ftv->name; struct Cyc_Absyn_Conref* ftvk= ftv->kind;
int present= 0;{ struct Cyc_List_List* bs= bound_tvars; for( 0; bs != 0; bs=({
struct Cyc_List_List* _temp2906= bs; if( _temp2906 == 0){ _throw( Null_Exception);}
_temp2906->tl;})){ struct Cyc_Absyn_Tvar* btv=( struct Cyc_Absyn_Tvar*)({ struct
Cyc_List_List* _temp2910= bs; if( _temp2910 == 0){ _throw( Null_Exception);}
_temp2910->hd;}); struct _tagged_string* btvn= btv->name; struct Cyc_Absyn_Conref*
btvk= btv->kind; if( Cyc_String_zstrptrcmp( ftvn, btvn) == 0){ present= 1; if( !
Cyc_Tcutil_constrain_kinds( btvk, ftvk)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2907=* ftvn; struct _tagged_string _temp2908= Cyc_Absynpp_ckind2string(
btvk); struct _tagged_string _temp2909= Cyc_Absynpp_ckind2string( ftvk); xprintf("type variable %.*s has kind %.*s but is used here atkind %.*s",
_temp2907.last_plus_one - _temp2907.curr, _temp2907.curr, _temp2908.last_plus_one
- _temp2908.curr, _temp2908.curr, _temp2909.last_plus_one - _temp2909.curr,
_temp2909.curr);}));} break;}}} if( ! present){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2911=* ftvn; struct _tagged_string _temp2912= Cyc_Absynpp_typ2string(
t); xprintf("unbound type variable %.*s in type %.*s", _temp2911.last_plus_one -
_temp2911.curr, _temp2911.curr, _temp2912.last_plus_one - _temp2912.curr,
_temp2912.curr);}));}}} static void Cyc_Tcutil_check_unique_unsorted( int(* cmp)(
void*, void*), struct Cyc_List_List* vs, struct Cyc_Position_Segment* loc,
struct _tagged_string(* a2string)( void*), struct _tagged_string msg){ for( 0;
vs != 0; vs=({ struct Cyc_List_List* _temp2914= vs; if( _temp2914 == 0){ _throw(
Null_Exception);} _temp2914->tl;})){ struct Cyc_List_List* vs2=({ struct Cyc_List_List*
_temp2921= vs; if( _temp2921 == 0){ _throw( Null_Exception);} _temp2921->tl;});
for( 0; vs2 != 0; vs2=({ struct Cyc_List_List* _temp2915= vs2; if( _temp2915 ==
0){ _throw( Null_Exception);} _temp2915->tl;})){ if( cmp(( void*)({ struct Cyc_List_List*
_temp2916= vs; if( _temp2916 == 0){ _throw( Null_Exception);} _temp2916->hd;}),(
void*)({ struct Cyc_List_List* _temp2917= vs2; if( _temp2917 == 0){ _throw(
Null_Exception);} _temp2917->hd;})) == 0){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2919= msg; struct _tagged_string _temp2920= a2string(( void*)({
struct Cyc_List_List* _temp2918= vs; if( _temp2918 == 0){ _throw( Null_Exception);}
_temp2918->hd;})); xprintf("%.*s: %.*s", _temp2919.last_plus_one - _temp2919.curr,
_temp2919.curr, _temp2920.last_plus_one - _temp2920.curr, _temp2920.curr);}));}}}}
static struct _tagged_string Cyc_Tcutil_strptr2string( struct _tagged_string* s){
return* s;} void Cyc_Tcutil_check_unique_vars( struct Cyc_List_List* vs, struct
Cyc_Position_Segment* loc, struct _tagged_string msg){(( void(*)( int(* cmp)(
struct _tagged_string*, struct _tagged_string*), struct Cyc_List_List* vs,
struct Cyc_Position_Segment* loc, struct _tagged_string(* a2string)( struct
_tagged_string*), struct _tagged_string msg)) Cyc_Tcutil_check_unique_unsorted)(
Cyc_String_zstrptrcmp, vs, loc, Cyc_Tcutil_strptr2string, msg);} void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* tvs){(( void(*)( int(*
cmp)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List* vs,
struct Cyc_Position_Segment* loc, struct _tagged_string(* a2string)( struct Cyc_Absyn_Tvar*),
struct _tagged_string msg)) Cyc_Tcutil_check_unique_unsorted)( Cyc_Absyn_tvar_cmp,
tvs, loc, Cyc_Tcutil_tvar2string,( struct _tagged_string)({ char* _temp2922=(
char*)"duplicate type variable"; struct _tagged_string _temp2923; _temp2923.curr=
_temp2922; _temp2923.base= _temp2922; _temp2923.last_plus_one= _temp2922 + 24;
_temp2923;}));} struct _tuple15{ struct Cyc_Absyn_Structfield* f1; int f2; } ;
struct _tuple16{ struct Cyc_List_List* f1; void* f2; } ; struct _tuple17{ struct
Cyc_Absyn_Structfield* f1; void* f2; } ; struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* sdfields){ struct Cyc_List_List* fields= 0;{ struct
Cyc_List_List* sd_fields= sdfields; for( 0; sd_fields != 0; sd_fields=({ struct
Cyc_List_List* _temp2924= sd_fields; if( _temp2924 == 0){ _throw( Null_Exception);}
_temp2924->tl;})){ if( Cyc_String_strcmp(*(( struct Cyc_Absyn_Structfield*)({
struct Cyc_List_List* _temp2925= sd_fields; if( _temp2925 == 0){ _throw(
Null_Exception);} _temp2925->hd;}))->name,( struct _tagged_string)({ char*
_temp2926=( char*)""; struct _tagged_string _temp2927; _temp2927.curr= _temp2926;
_temp2927.base= _temp2926; _temp2927.last_plus_one= _temp2926 + 1; _temp2927;}))
!= 0){ fields=({ struct Cyc_List_List* _temp2928=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2928->hd=( void*)({ struct
_tuple15* _temp2929=( struct _tuple15*) GC_malloc( sizeof( struct _tuple15));
_temp2929->f1=( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp2930=
sd_fields; if( _temp2930 == 0){ _throw( Null_Exception);} _temp2930->hd;});
_temp2929->f2= 0; _temp2929;}); _temp2928->tl= fields; _temp2928;});}}} fields=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( fields);{
struct Cyc_List_List* ans= 0; for( 0; des != 0; des=({ struct Cyc_List_List*
_temp2931= des; if( _temp2931 == 0){ _throw( Null_Exception);} _temp2931->tl;})){
struct _tuple16 _temp2935; void* _temp2936; struct Cyc_List_List* _temp2938;
struct _tuple16* _temp2933=( struct _tuple16*)({ struct Cyc_List_List* _temp2932=
des; if( _temp2932 == 0){ _throw( Null_Exception);} _temp2932->hd;}); _temp2935=*
_temp2933; _LL2939: _temp2938= _temp2935.f1; goto _LL2937; _LL2937: _temp2936=
_temp2935.f2; goto _LL2934; _LL2934: if( _temp2938 == 0){ struct Cyc_List_List*
fields2= fields; for( 0; fields2 != 0; fields2=({ struct Cyc_List_List*
_temp2940= fields2; if( _temp2940 == 0){ _throw( Null_Exception);} _temp2940->tl;})){
if( !(*(( struct _tuple15*)({ struct Cyc_List_List* _temp2941= fields2; if(
_temp2941 == 0){ _throw( Null_Exception);} _temp2941->hd;}))).f2){(*(( struct
_tuple15*)({ struct Cyc_List_List* _temp2942= fields2; if( _temp2942 == 0){
_throw( Null_Exception);} _temp2942->hd;}))).f2= 1;(*(( struct _tuple16*)({
struct Cyc_List_List* _temp2943= des; if( _temp2943 == 0){ _throw(
Null_Exception);} _temp2943->hd;}))).f1=({ struct Cyc_List_List* _temp2944=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2944->hd=(
void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp2945=( struct Cyc_Absyn_FieldName_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct)); _temp2945[ 0]=({ struct
Cyc_Absyn_FieldName_struct _temp2946; _temp2946.tag= Cyc_Absyn_FieldName;
_temp2946.f1=((*(( struct _tuple15*)({ struct Cyc_List_List* _temp2947= fields2;
if( _temp2947 == 0){ _throw( Null_Exception);} _temp2947->hd;}))).f1)->name;
_temp2946;}); _temp2945;})); _temp2944->tl= 0; _temp2944;}); ans=({ struct Cyc_List_List*
_temp2948=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp2948->hd=( void*)({ struct _tuple17* _temp2949=( struct _tuple17*)
_region_malloc( rgn, sizeof( struct _tuple17)); _temp2949->f1=(*(( struct
_tuple15*)({ struct Cyc_List_List* _temp2950= fields2; if( _temp2950 == 0){
_throw( Null_Exception);} _temp2950->hd;}))).f1; _temp2949->f2= _temp2936;
_temp2949;}); _temp2948->tl= ans; _temp2948;}); break;}} if( fields2 == 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp2951=( char*)"too many arguments to struct";
struct _tagged_string _temp2952; _temp2952.curr= _temp2951; _temp2952.base=
_temp2951; _temp2952.last_plus_one= _temp2951 + 29; _temp2952;}));}} else{ if(({
struct Cyc_List_List* _temp2953= _temp2938; if( _temp2953 == 0){ _throw(
Null_Exception);} _temp2953->tl;}) != 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp2954=( char*)"multiple designators are not supported";
struct _tagged_string _temp2955; _temp2955.curr= _temp2954; _temp2955.base=
_temp2954; _temp2955.last_plus_one= _temp2954 + 39; _temp2955;}));} else{ void*
_temp2957=( void*)({ struct Cyc_List_List* _temp2956= _temp2938; if( _temp2956
== 0){ _throw( Null_Exception);} _temp2956->hd;}); struct Cyc_Absyn_Exp*
_temp2963; struct _tagged_string* _temp2965; _LL2959: if(*(( int*) _temp2957) ==
Cyc_Absyn_ArrayElement){ _LL2964: _temp2963=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayElement_struct*) _temp2957)->f1; goto _LL2960;} else{ goto
_LL2961;} _LL2961: if(*(( int*) _temp2957) == Cyc_Absyn_FieldName){ _LL2966:
_temp2965=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp2957)->f1; goto _LL2962;} else{ goto _LL2958;} _LL2960: Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp2967=( char*)"array designator used in argument to struct";
struct _tagged_string _temp2968; _temp2968.curr= _temp2967; _temp2968.base=
_temp2967; _temp2968.last_plus_one= _temp2967 + 44; _temp2968;})); goto _LL2958;
_LL2962: { struct Cyc_List_List* fields2= fields; for( 0; fields2 != 0; fields2=({
struct Cyc_List_List* _temp2969= fields2; if( _temp2969 == 0){ _throw(
Null_Exception);} _temp2969->tl;})){ if( Cyc_String_zstrptrcmp( _temp2965,((*((
struct _tuple15*)({ struct Cyc_List_List* _temp2970= fields2; if( _temp2970 == 0){
_throw( Null_Exception);} _temp2970->hd;}))).f1)->name) == 0){ if((*(( struct
_tuple15*)({ struct Cyc_List_List* _temp2971= fields2; if( _temp2971 == 0){
_throw( Null_Exception);} _temp2971->hd;}))).f2){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2972=* _temp2965; xprintf("field %.*s has already been used as an argument",
_temp2972.last_plus_one - _temp2972.curr, _temp2972.curr);}));}(*(( struct
_tuple15*)({ struct Cyc_List_List* _temp2973= fields2; if( _temp2973 == 0){
_throw( Null_Exception);} _temp2973->hd;}))).f2= 1; ans=({ struct Cyc_List_List*
_temp2974=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp2974->hd=( void*)({ struct _tuple17* _temp2975=( struct _tuple17*)
_region_malloc( rgn, sizeof( struct _tuple17)); _temp2975->f1=(*(( struct
_tuple15*)({ struct Cyc_List_List* _temp2976= fields2; if( _temp2976 == 0){
_throw( Null_Exception);} _temp2976->hd;}))).f1; _temp2975->f2= _temp2936;
_temp2975;}); _temp2974->tl= ans; _temp2974;}); break;}} if( fields2 == 0){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2977=* _temp2965; xprintf("bad field designator %.*s",
_temp2977.last_plus_one - _temp2977.curr, _temp2977.curr);}));} goto _LL2958;}
_LL2958:;}}} for( 0; fields != 0; fields=({ struct Cyc_List_List* _temp2978=
fields; if( _temp2978 == 0){ _throw( Null_Exception);} _temp2978->tl;})){ if( !(*((
struct _tuple15*)({ struct Cyc_List_List* _temp2979= fields; if( _temp2979 == 0){
_throw( Null_Exception);} _temp2979->hd;}))).f2){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp2980=( char*)"too few arguments to struct"; struct
_tagged_string _temp2981; _temp2981.curr= _temp2980; _temp2981.base= _temp2980;
_temp2981.last_plus_one= _temp2980 + 28; _temp2981;})); break;}} return(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( ans);}} int Cyc_Tcutil_is_tagged_pointer_typ(
void* t){ void* _temp2982= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp2988; struct Cyc_Absyn_Conref* _temp2990; struct Cyc_Absyn_Tqual _temp2992;
struct Cyc_Absyn_Conref* _temp2994; void* _temp2996; void* _temp2998; _LL2984:
if(( unsigned int) _temp2982 > 4u?*(( int*) _temp2982) == Cyc_Absyn_PointerType:
0){ _LL2989: _temp2988=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp2982)->f1; _LL2999: _temp2998=( void*) _temp2988.elt_typ; goto _LL2997;
_LL2997: _temp2996=( void*) _temp2988.rgn_typ; goto _LL2995; _LL2995: _temp2994=(
struct Cyc_Absyn_Conref*) _temp2988.nullable; goto _LL2993; _LL2993: _temp2992=(
struct Cyc_Absyn_Tqual) _temp2988.tq; goto _LL2991; _LL2991: _temp2990=( struct
Cyc_Absyn_Conref*) _temp2988.bounds; goto _LL2985;} else{ goto _LL2986;} _LL2986:
goto _LL2987; _LL2985: { struct Cyc_Absyn_Conref* b=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp2990); void*
_temp3000=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)( b))->v; void* _temp3008; _LL3002: if(( unsigned int)
_temp3000 > 1u?*(( int*) _temp3000) == Cyc_Absyn_Eq_constr: 0){ _LL3009:
_temp3008=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3000)->f1; if(
_temp3008 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL3003;} else{ goto _LL3004;}}
else{ goto _LL3004;} _LL3004: if( _temp3000 ==( void*) Cyc_Absyn_No_constr){
goto _LL3005;} else{ goto _LL3006;} _LL3006: goto _LL3007; _LL3003: return 1;
_LL3005:( void*)( b->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp3010=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp3010[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp3011; _temp3011.tag= Cyc_Absyn_Eq_constr;
_temp3011.f1=( void*)(( void*) Cyc_Absyn_Unknown_b); _temp3011;}); _temp3010;})));
return 1; _LL3007: return 0; _LL3001:;} _LL2987: return 0; _LL2983:;} struct
_tuple6 Cyc_Tcutil_addressof_props( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e){ struct _tuple6 bogus_ans=({ struct _tuple6 _temp3256; _temp3256.f1= 0;
_temp3256.f2=( void*) Cyc_Absyn_HeapRgn; _temp3256;}); void* _temp3012=( void*)
e->r; void* _temp3026; struct _tuple1* _temp3028; struct _tagged_string*
_temp3030; struct Cyc_Absyn_Exp* _temp3032; struct _tagged_string* _temp3034;
struct Cyc_Absyn_Exp* _temp3036; struct Cyc_Absyn_Exp* _temp3038; struct Cyc_Absyn_Exp*
_temp3040; struct Cyc_Absyn_Exp* _temp3042; _LL3014: if(*(( int*) _temp3012) ==
Cyc_Absyn_Var_e){ _LL3029: _temp3028=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*)
_temp3012)->f1; goto _LL3027; _LL3027: _temp3026=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp3012)->f2; goto _LL3015;} else{ goto _LL3016;} _LL3016: if(*(( int*)
_temp3012) == Cyc_Absyn_StructMember_e){ _LL3033: _temp3032=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp3012)->f1; goto _LL3031; _LL3031:
_temp3030=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp3012)->f2; goto _LL3017;} else{ goto _LL3018;} _LL3018: if(*(( int*)
_temp3012) == Cyc_Absyn_StructArrow_e){ _LL3037: _temp3036=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp3012)->f1; goto _LL3035; _LL3035:
_temp3034=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp3012)->f2; goto _LL3019;} else{ goto _LL3020;} _LL3020: if(*(( int*)
_temp3012) == Cyc_Absyn_Deref_e){ _LL3039: _temp3038=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Deref_e_struct*) _temp3012)->f1; goto _LL3021;} else{ goto
_LL3022;} _LL3022: if(*(( int*) _temp3012) == Cyc_Absyn_Subscript_e){ _LL3043:
_temp3042=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp3012)->f1; goto _LL3041; _LL3041: _temp3040=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp3012)->f2; goto _LL3023;} else{ goto
_LL3024;} _LL3024: goto _LL3025; _LL3015: { void* _temp3044= _temp3026; struct
Cyc_Absyn_Fndecl* _temp3058; struct Cyc_Absyn_Vardecl* _temp3060; struct Cyc_Absyn_Vardecl*
_temp3062; struct Cyc_Absyn_Vardecl* _temp3064; struct Cyc_Absyn_Vardecl*
_temp3066; _LL3046: if( _temp3044 ==( void*) Cyc_Absyn_Unresolved_b){ goto
_LL3047;} else{ goto _LL3048;} _LL3048: if(( unsigned int) _temp3044 > 1u?*((
int*) _temp3044) == Cyc_Absyn_Funname_b: 0){ _LL3059: _temp3058=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Funname_b_struct*) _temp3044)->f1; goto _LL3049;} else{ goto
_LL3050;} _LL3050: if(( unsigned int) _temp3044 > 1u?*(( int*) _temp3044) == Cyc_Absyn_Global_b:
0){ _LL3061: _temp3060=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Global_b_struct*)
_temp3044)->f1; goto _LL3051;} else{ goto _LL3052;} _LL3052: if(( unsigned int)
_temp3044 > 1u?*(( int*) _temp3044) == Cyc_Absyn_Local_b: 0){ _LL3063: _temp3062=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Local_b_struct*) _temp3044)->f1;
goto _LL3053;} else{ goto _LL3054;} _LL3054: if(( unsigned int) _temp3044 > 1u?*((
int*) _temp3044) == Cyc_Absyn_Pat_b: 0){ _LL3065: _temp3064=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Pat_b_struct*) _temp3044)->f1; goto _LL3055;} else{ goto
_LL3056;} _LL3056: if(( unsigned int) _temp3044 > 1u?*(( int*) _temp3044) == Cyc_Absyn_Param_b:
0){ _LL3067: _temp3066=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Param_b_struct*)
_temp3044)->f1; goto _LL3057;} else{ goto _LL3045;} _LL3047: return bogus_ans;
_LL3049: return({ struct _tuple6 _temp3068; _temp3068.f1= 1; _temp3068.f2=( void*)
Cyc_Absyn_HeapRgn; _temp3068;}); _LL3051: { void* _temp3070= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp3069= e->topt; if( _temp3069 == 0){ _throw(
Null_Exception);} _temp3069->v;})); struct Cyc_Absyn_Exp* _temp3076; struct Cyc_Absyn_Tqual
_temp3078; void* _temp3080; _LL3072: if(( unsigned int) _temp3070 > 4u?*(( int*)
_temp3070) == Cyc_Absyn_ArrayType: 0){ _LL3081: _temp3080=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp3070)->f1; goto _LL3079; _LL3079: _temp3078=( struct Cyc_Absyn_Tqual)((
struct Cyc_Absyn_ArrayType_struct*) _temp3070)->f2; goto _LL3077; _LL3077:
_temp3076=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp3070)->f3; goto _LL3073;} else{ goto _LL3074;} _LL3074: goto _LL3075;
_LL3073: return({ struct _tuple6 _temp3082; _temp3082.f1= 1; _temp3082.f2=( void*)
Cyc_Absyn_HeapRgn; _temp3082;}); _LL3075: return({ struct _tuple6 _temp3083;
_temp3083.f1=( _temp3060->tq).q_const; _temp3083.f2=( void*) Cyc_Absyn_HeapRgn;
_temp3083;}); _LL3071:;} _LL3053: { void* _temp3085= Cyc_Tcutil_compress(( void*)({
struct Cyc_Core_Opt* _temp3084= e->topt; if( _temp3084 == 0){ _throw(
Null_Exception);} _temp3084->v;})); struct Cyc_Absyn_Exp* _temp3091; struct Cyc_Absyn_Tqual
_temp3093; void* _temp3095; _LL3087: if(( unsigned int) _temp3085 > 4u?*(( int*)
_temp3085) == Cyc_Absyn_ArrayType: 0){ _LL3096: _temp3095=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp3085)->f1; goto _LL3094; _LL3094: _temp3093=( struct Cyc_Absyn_Tqual)((
struct Cyc_Absyn_ArrayType_struct*) _temp3085)->f2; goto _LL3092; _LL3092:
_temp3091=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp3085)->f3; goto _LL3088;} else{ goto _LL3089;} _LL3089: goto _LL3090;
_LL3088: return({ struct _tuple6 _temp3097; _temp3097.f1= 1; _temp3097.f2=( void*)({
struct Cyc_Core_Opt* _temp3098= _temp3062->rgn; if( _temp3098 == 0){ _throw(
Null_Exception);} _temp3098->v;}); _temp3097;}); _LL3090: return({ struct
_tuple6 _temp3099; _temp3099.f1=( _temp3062->tq).q_const; _temp3099.f2=( void*)({
struct Cyc_Core_Opt* _temp3100= _temp3062->rgn; if( _temp3100 == 0){ _throw(
Null_Exception);} _temp3100->v;}); _temp3099;}); _LL3086:;} _LL3055: _temp3066=
_temp3064; goto _LL3057; _LL3057: return({ struct _tuple6 _temp3101; _temp3101.f1=(
_temp3066->tq).q_const; _temp3101.f2=( void*)({ struct Cyc_Core_Opt* _temp3102=
_temp3066->rgn; if( _temp3102 == 0){ _throw( Null_Exception);} _temp3102->v;});
_temp3101;}); _LL3045:;} _LL3017: { void* _temp3104= Cyc_Tcutil_compress(( void*)({
struct Cyc_Core_Opt* _temp3103= _temp3032->topt; if( _temp3103 == 0){ _throw(
Null_Exception);} _temp3103->v;})); struct Cyc_List_List* _temp3116; struct Cyc_List_List*
_temp3118; struct Cyc_Absyn_Structdecl** _temp3120; struct Cyc_Absyn_Structdecl*
_temp3122; struct Cyc_List_List* _temp3123; struct _tuple1* _temp3125; struct
Cyc_Absyn_Uniondecl** _temp3127; struct Cyc_Absyn_Uniondecl* _temp3129; struct
Cyc_List_List* _temp3130; struct _tuple1* _temp3132; _LL3106: if(( unsigned int)
_temp3104 > 4u?*(( int*) _temp3104) == Cyc_Absyn_AnonStructType: 0){ _LL3117:
_temp3116=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp3104)->f1; goto _LL3107;} else{ goto _LL3108;} _LL3108: if(( unsigned int)
_temp3104 > 4u?*(( int*) _temp3104) == Cyc_Absyn_AnonUnionType: 0){ _LL3119:
_temp3118=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp3104)->f1; goto _LL3109;} else{ goto _LL3110;} _LL3110: if(( unsigned int)
_temp3104 > 4u?*(( int*) _temp3104) == Cyc_Absyn_StructType: 0){ _LL3126:
_temp3125=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp3104)->f1;
goto _LL3124; _LL3124: _temp3123=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp3104)->f2; goto _LL3121; _LL3121: _temp3120=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp3104)->f3; if( _temp3120 == 0){ goto
_LL3112;} else{ _temp3122=* _temp3120; goto _LL3111;}} else{ goto _LL3112;}
_LL3112: if(( unsigned int) _temp3104 > 4u?*(( int*) _temp3104) == Cyc_Absyn_UnionType:
0){ _LL3133: _temp3132=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp3104)->f1; goto _LL3131; _LL3131: _temp3130=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnionType_struct*) _temp3104)->f2; goto _LL3128; _LL3128:
_temp3127=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp3104)->f3; if( _temp3127 == 0){ goto _LL3114;} else{ _temp3129=* _temp3127;
goto _LL3113;}} else{ goto _LL3114;} _LL3114: goto _LL3115; _LL3107: _temp3118=
_temp3116; goto _LL3109; _LL3109: { struct Cyc_Absyn_Structfield* finfo= Cyc_Absyn_lookup_field(
_temp3118, _temp3030); if( finfo != 0?({ struct Cyc_Absyn_Structfield* _temp3134=
finfo; if( _temp3134 == 0){ _throw( Null_Exception);} _temp3134->width;}) != 0:
0){ return({ struct _tuple6 _temp3135; _temp3135.f1=({ struct Cyc_Absyn_Structfield*
_temp3136= finfo; if( _temp3136 == 0){ _throw( Null_Exception);} _temp3136->tq;}).q_const;
_temp3135.f2=( Cyc_Tcutil_addressof_props( te, _temp3032)).f2; _temp3135;});}
return bogus_ans;} _LL3111: { struct Cyc_Absyn_Structfield* finfo= Cyc_Absyn_lookup_struct_field(
_temp3122, _temp3030); if( finfo != 0?({ struct Cyc_Absyn_Structfield* _temp3137=
finfo; if( _temp3137 == 0){ _throw( Null_Exception);} _temp3137->width;}) != 0:
0){ return({ struct _tuple6 _temp3138; _temp3138.f1=({ struct Cyc_Absyn_Structfield*
_temp3139= finfo; if( _temp3139 == 0){ _throw( Null_Exception);} _temp3139->tq;}).q_const;
_temp3138.f2=( Cyc_Tcutil_addressof_props( te, _temp3032)).f2; _temp3138;});}
return bogus_ans;} _LL3113: { struct Cyc_Absyn_Structfield* finfo= Cyc_Absyn_lookup_union_field(
_temp3129, _temp3030); if( finfo != 0){ return({ struct _tuple6 _temp3140;
_temp3140.f1=({ struct Cyc_Absyn_Structfield* _temp3141= finfo; if( _temp3141 ==
0){ _throw( Null_Exception);} _temp3141->tq;}).q_const; _temp3140.f2=( Cyc_Tcutil_addressof_props(
te, _temp3032)).f2; _temp3140;});} goto _LL3115;} _LL3115: return bogus_ans;
_LL3105:;} _LL3019: { void* _temp3143= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp3142= _temp3036->topt; if( _temp3142 == 0){ _throw( Null_Exception);}
_temp3142->v;})); struct Cyc_Absyn_PtrInfo _temp3149; struct Cyc_Absyn_Conref*
_temp3151; struct Cyc_Absyn_Tqual _temp3153; struct Cyc_Absyn_Conref* _temp3155;
void* _temp3157; void* _temp3159; _LL3145: if(( unsigned int) _temp3143 > 4u?*((
int*) _temp3143) == Cyc_Absyn_PointerType: 0){ _LL3150: _temp3149=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp3143)->f1; _LL3160: _temp3159=( void*)
_temp3149.elt_typ; goto _LL3158; _LL3158: _temp3157=( void*) _temp3149.rgn_typ;
goto _LL3156; _LL3156: _temp3155=( struct Cyc_Absyn_Conref*) _temp3149.nullable;
goto _LL3154; _LL3154: _temp3153=( struct Cyc_Absyn_Tqual) _temp3149.tq; goto
_LL3152; _LL3152: _temp3151=( struct Cyc_Absyn_Conref*) _temp3149.bounds; goto
_LL3146;} else{ goto _LL3147;} _LL3147: goto _LL3148; _LL3146: { void* _temp3161=
Cyc_Tcutil_compress( _temp3159); struct Cyc_List_List* _temp3173; struct Cyc_List_List*
_temp3175; struct Cyc_Absyn_Structdecl** _temp3177; struct Cyc_Absyn_Structdecl*
_temp3179; struct Cyc_List_List* _temp3180; struct _tuple1* _temp3182; struct
Cyc_Absyn_Uniondecl** _temp3184; struct Cyc_Absyn_Uniondecl* _temp3186; struct
Cyc_List_List* _temp3187; struct _tuple1* _temp3189; _LL3163: if(( unsigned int)
_temp3161 > 4u?*(( int*) _temp3161) == Cyc_Absyn_AnonStructType: 0){ _LL3174:
_temp3173=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp3161)->f1; goto _LL3164;} else{ goto _LL3165;} _LL3165: if(( unsigned int)
_temp3161 > 4u?*(( int*) _temp3161) == Cyc_Absyn_AnonUnionType: 0){ _LL3176:
_temp3175=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp3161)->f1; goto _LL3166;} else{ goto _LL3167;} _LL3167: if(( unsigned int)
_temp3161 > 4u?*(( int*) _temp3161) == Cyc_Absyn_StructType: 0){ _LL3183:
_temp3182=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp3161)->f1;
goto _LL3181; _LL3181: _temp3180=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp3161)->f2; goto _LL3178; _LL3178: _temp3177=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp3161)->f3; if( _temp3177 == 0){ goto
_LL3169;} else{ _temp3179=* _temp3177; goto _LL3168;}} else{ goto _LL3169;}
_LL3169: if(( unsigned int) _temp3161 > 4u?*(( int*) _temp3161) == Cyc_Absyn_UnionType:
0){ _LL3190: _temp3189=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp3161)->f1; goto _LL3188; _LL3188: _temp3187=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnionType_struct*) _temp3161)->f2; goto _LL3185; _LL3185:
_temp3184=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp3161)->f3; if( _temp3184 == 0){ goto _LL3171;} else{ _temp3186=* _temp3184;
goto _LL3170;}} else{ goto _LL3171;} _LL3171: goto _LL3172; _LL3164: _temp3175=
_temp3173; goto _LL3166; _LL3166: { struct Cyc_Absyn_Structfield* finfo= Cyc_Absyn_lookup_field(
_temp3175, _temp3034); if( finfo != 0?({ struct Cyc_Absyn_Structfield* _temp3191=
finfo; if( _temp3191 == 0){ _throw( Null_Exception);} _temp3191->width;}) != 0:
0){ return({ struct _tuple6 _temp3192; _temp3192.f1=({ struct Cyc_Absyn_Structfield*
_temp3193= finfo; if( _temp3193 == 0){ _throw( Null_Exception);} _temp3193->tq;}).q_const;
_temp3192.f2= _temp3157; _temp3192;});} return bogus_ans;} _LL3168: { struct Cyc_Absyn_Structfield*
finfo= Cyc_Absyn_lookup_struct_field( _temp3179, _temp3034); if( finfo != 0?({
struct Cyc_Absyn_Structfield* _temp3194= finfo; if( _temp3194 == 0){ _throw(
Null_Exception);} _temp3194->width;}) != 0: 0){ return({ struct _tuple6
_temp3195; _temp3195.f1=({ struct Cyc_Absyn_Structfield* _temp3196= finfo; if(
_temp3196 == 0){ _throw( Null_Exception);} _temp3196->tq;}).q_const; _temp3195.f2=
_temp3157; _temp3195;});} return bogus_ans;} _LL3170: { struct Cyc_Absyn_Structfield*
finfo= Cyc_Absyn_lookup_union_field( _temp3186, _temp3034); if( finfo != 0){
return({ struct _tuple6 _temp3197; _temp3197.f1=({ struct Cyc_Absyn_Structfield*
_temp3198= finfo; if( _temp3198 == 0){ _throw( Null_Exception);} _temp3198->tq;}).q_const;
_temp3197.f2= _temp3157; _temp3197;});} return bogus_ans;} _LL3172: return
bogus_ans; _LL3162:;} _LL3148: return bogus_ans; _LL3144:;} _LL3021: { void*
_temp3200= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp3199=
_temp3038->topt; if( _temp3199 == 0){ _throw( Null_Exception);} _temp3199->v;}));
struct Cyc_Absyn_PtrInfo _temp3206; struct Cyc_Absyn_Conref* _temp3208; struct
Cyc_Absyn_Tqual _temp3210; struct Cyc_Absyn_Conref* _temp3212; void* _temp3214;
void* _temp3216; _LL3202: if(( unsigned int) _temp3200 > 4u?*(( int*) _temp3200)
== Cyc_Absyn_PointerType: 0){ _LL3207: _temp3206=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp3200)->f1; _LL3217: _temp3216=( void*)
_temp3206.elt_typ; goto _LL3215; _LL3215: _temp3214=( void*) _temp3206.rgn_typ;
goto _LL3213; _LL3213: _temp3212=( struct Cyc_Absyn_Conref*) _temp3206.nullable;
goto _LL3211; _LL3211: _temp3210=( struct Cyc_Absyn_Tqual) _temp3206.tq; goto
_LL3209; _LL3209: _temp3208=( struct Cyc_Absyn_Conref*) _temp3206.bounds; goto
_LL3203;} else{ goto _LL3204;} _LL3204: goto _LL3205; _LL3203: return({ struct
_tuple6 _temp3218; _temp3218.f1= _temp3210.q_const; _temp3218.f2= _temp3214;
_temp3218;}); _LL3205: return bogus_ans; _LL3201:;} _LL3023: { void* t= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp3253= _temp3042->topt; if( _temp3253 == 0){
_throw( Null_Exception);} _temp3253->v;})); void* _temp3219= t; struct Cyc_Absyn_Exp*
_temp3229; struct Cyc_Absyn_Tqual _temp3231; void* _temp3233; struct Cyc_List_List*
_temp3235; struct Cyc_Absyn_PtrInfo _temp3237; struct Cyc_Absyn_Conref*
_temp3239; struct Cyc_Absyn_Tqual _temp3241; struct Cyc_Absyn_Conref* _temp3243;
void* _temp3245; void* _temp3247; _LL3221: if(( unsigned int) _temp3219 > 4u?*((
int*) _temp3219) == Cyc_Absyn_ArrayType: 0){ _LL3234: _temp3233=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3219)->f1; goto _LL3232; _LL3232:
_temp3231=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp3219)->f2; goto _LL3230; _LL3230: _temp3229=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3219)->f3; goto _LL3222;} else{ goto
_LL3223;} _LL3223: if(( unsigned int) _temp3219 > 4u?*(( int*) _temp3219) == Cyc_Absyn_TupleType:
0){ _LL3236: _temp3235=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp3219)->f1; goto _LL3224;} else{ goto _LL3225;} _LL3225: if(( unsigned int)
_temp3219 > 4u?*(( int*) _temp3219) == Cyc_Absyn_PointerType: 0){ _LL3238:
_temp3237=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp3219)->f1; _LL3248: _temp3247=( void*) _temp3237.elt_typ; goto _LL3246;
_LL3246: _temp3245=( void*) _temp3237.rgn_typ; goto _LL3244; _LL3244: _temp3243=(
struct Cyc_Absyn_Conref*) _temp3237.nullable; goto _LL3242; _LL3242: _temp3241=(
struct Cyc_Absyn_Tqual) _temp3237.tq; goto _LL3240; _LL3240: _temp3239=( struct
Cyc_Absyn_Conref*) _temp3237.bounds; goto _LL3226;} else{ goto _LL3227;} _LL3227:
goto _LL3228; _LL3222: return({ struct _tuple6 _temp3249; _temp3249.f1=
_temp3231.q_const; _temp3249.f2=( Cyc_Tcutil_addressof_props( te, _temp3042)).f2;
_temp3249;}); _LL3224: { struct _tuple4* finfo= Cyc_Absyn_lookup_tuple_field(
_temp3235,( int) Cyc_Evexp_eval_const_uint_exp( _temp3040)); if( finfo != 0){
return({ struct _tuple6 _temp3250; _temp3250.f1=(({ struct _tuple4* _temp3251=
finfo; if( _temp3251 == 0){ _throw( Null_Exception);}* _temp3251;}).f1).q_const;
_temp3250.f2=( Cyc_Tcutil_addressof_props( te, _temp3042)).f2; _temp3250;});}
return bogus_ans;} _LL3226: return({ struct _tuple6 _temp3252; _temp3252.f1=
_temp3241.q_const; _temp3252.f2= _temp3245; _temp3252;}); _LL3228: return
bogus_ans; _LL3220:;} _LL3025: Cyc_Tcutil_terr( e->loc,( struct _tagged_string)({
char* _temp3254=( char*)"unary & applied to non-lvalue"; struct _tagged_string
_temp3255; _temp3255.curr= _temp3254; _temp3255.base= _temp3254; _temp3255.last_plus_one=
_temp3254 + 30; _temp3255;})); return bogus_ans; _LL3013:;} void* Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv* te, void* e_typ, struct Cyc_Absyn_Exp* e){ void*
_temp3257= Cyc_Tcutil_compress( e_typ); struct Cyc_Absyn_Exp* _temp3263; struct
Cyc_Absyn_Tqual _temp3265; void* _temp3267; _LL3259: if(( unsigned int)
_temp3257 > 4u?*(( int*) _temp3257) == Cyc_Absyn_ArrayType: 0){ _LL3268:
_temp3267=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3257)->f1; goto
_LL3266; _LL3266: _temp3265=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp3257)->f2; goto _LL3264; _LL3264: _temp3263=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3257)->f3; goto _LL3260;} else{ goto
_LL3261;} _LL3261: goto _LL3262; _LL3260: { void* _temp3271; int _temp3273;
struct _tuple6 _temp3269= Cyc_Tcutil_addressof_props( te, e); _LL3274: _temp3273=
_temp3269.f1; goto _LL3272; _LL3272: _temp3271= _temp3269.f2; goto _LL3270;
_LL3270: return Cyc_Absyn_atb_typ( _temp3267, _temp3271, _temp3265,( void*)({
struct Cyc_Absyn_Upper_b_struct* _temp3275=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp3275[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp3276; _temp3276.tag= Cyc_Absyn_Upper_b; _temp3276.f1=
e; _temp3276;}); _temp3275;}));} _LL3262: return e_typ; _LL3258:;} void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment* loc, unsigned int i, struct Cyc_Absyn_Conref* b){ b=((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
b);{ void* _temp3277=( void*) b->v; void* _temp3285; void* _temp3287; struct Cyc_Absyn_Exp*
_temp3289; _LL3279: if(( unsigned int) _temp3277 > 1u?*(( int*) _temp3277) ==
Cyc_Absyn_Eq_constr: 0){ _LL3286: _temp3285=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3277)->f1; if( _temp3285 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL3280;}
else{ goto _LL3281;}} else{ goto _LL3281;} _LL3281: if(( unsigned int) _temp3277
> 1u?*(( int*) _temp3277) == Cyc_Absyn_Eq_constr: 0){ _LL3288: _temp3287=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp3277)->f1; if(( unsigned int) _temp3287
> 1u?*(( int*) _temp3287) == Cyc_Absyn_Upper_b: 0){ _LL3290: _temp3289=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp3287)->f1; goto _LL3282;}
else{ goto _LL3283;}} else{ goto _LL3283;} _LL3283: goto _LL3284; _LL3280:
return; _LL3282: if( Cyc_Evexp_eval_const_uint_exp( _temp3289) <= i){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp3291=( char*)"dereference is out of bounds";
struct _tagged_string _temp3292; _temp3292.curr= _temp3291; _temp3292.base=
_temp3291; _temp3292.last_plus_one= _temp3291 + 29; _temp3292;}));} return;
_LL3284:( void*)( b->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp3293=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp3293[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp3294; _temp3294.tag= Cyc_Absyn_Eq_constr;
_temp3294.f1=( void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp3295=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp3295[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp3296; _temp3296.tag= Cyc_Absyn_Upper_b;
_temp3296.f1= Cyc_Absyn_uint_exp( i + 1, 0); _temp3296;}); _temp3295;}));
_temp3294;}); _temp3293;}))); return; _LL3278:;}} void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Conref* b){ Cyc_Tcutil_check_bound(
loc, 0, b);} int Cyc_Tcutil_bits_only( void* t){ void* _temp3297= Cyc_Tcutil_compress(
t); void* _temp3323; void* _temp3325; struct Cyc_Absyn_Enumdecl* _temp3327;
struct _tuple1* _temp3329; struct Cyc_Absyn_Exp* _temp3331; struct Cyc_Absyn_Tqual
_temp3333; void* _temp3335; struct Cyc_List_List* _temp3337; struct Cyc_Absyn_Structdecl**
_temp3339; struct Cyc_List_List* _temp3341; struct _tuple1* _temp3343; struct
Cyc_Absyn_Uniondecl** _temp3345; struct Cyc_List_List* _temp3347; struct _tuple1*
_temp3349; struct Cyc_List_List* _temp3351; struct Cyc_List_List* _temp3353;
_LL3299: if( _temp3297 ==( void*) Cyc_Absyn_VoidType){ goto _LL3300;} else{ goto
_LL3301;} _LL3301: if(( unsigned int) _temp3297 > 4u?*(( int*) _temp3297) == Cyc_Absyn_IntType:
0){ _LL3326: _temp3325=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp3297)->f1;
goto _LL3324; _LL3324: _temp3323=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp3297)->f2; goto _LL3302;} else{ goto _LL3303;} _LL3303: if( _temp3297 ==(
void*) Cyc_Absyn_FloatType){ goto _LL3304;} else{ goto _LL3305;} _LL3305: if(
_temp3297 ==( void*) Cyc_Absyn_DoubleType){ goto _LL3306;} else{ goto _LL3307;}
_LL3307: if(( unsigned int) _temp3297 > 4u?*(( int*) _temp3297) == Cyc_Absyn_EnumType:
0){ _LL3330: _temp3329=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*)
_temp3297)->f1; goto _LL3328; _LL3328: _temp3327=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_EnumType_struct*) _temp3297)->f2; goto _LL3308;} else{ goto
_LL3309;} _LL3309: if(( unsigned int) _temp3297 > 4u?*(( int*) _temp3297) == Cyc_Absyn_ArrayType:
0){ _LL3336: _temp3335=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3297)->f1;
goto _LL3334; _LL3334: _temp3333=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp3297)->f2; goto _LL3332; _LL3332: _temp3331=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3297)->f3; goto _LL3310;} else{ goto
_LL3311;} _LL3311: if(( unsigned int) _temp3297 > 4u?*(( int*) _temp3297) == Cyc_Absyn_TupleType:
0){ _LL3338: _temp3337=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp3297)->f1; goto _LL3312;} else{ goto _LL3313;} _LL3313: if(( unsigned int)
_temp3297 > 4u?*(( int*) _temp3297) == Cyc_Absyn_StructType: 0){ _LL3344:
_temp3343=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp3297)->f1;
goto _LL3342; _LL3342: _temp3341=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp3297)->f2; goto _LL3340; _LL3340: _temp3339=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp3297)->f3; goto _LL3314;} else{ goto
_LL3315;} _LL3315: if(( unsigned int) _temp3297 > 4u?*(( int*) _temp3297) == Cyc_Absyn_UnionType:
0){ _LL3350: _temp3349=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp3297)->f1; goto _LL3348; _LL3348: _temp3347=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnionType_struct*) _temp3297)->f2; goto _LL3346; _LL3346:
_temp3345=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp3297)->f3; goto _LL3316;} else{ goto _LL3317;} _LL3317: if(( unsigned int)
_temp3297 > 4u?*(( int*) _temp3297) == Cyc_Absyn_AnonStructType: 0){ _LL3352:
_temp3351=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp3297)->f1; goto _LL3318;} else{ goto _LL3319;} _LL3319: if(( unsigned int)
_temp3297 > 4u?*(( int*) _temp3297) == Cyc_Absyn_AnonUnionType: 0){ _LL3354:
_temp3353=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp3297)->f1; goto _LL3320;} else{ goto _LL3321;} _LL3321: goto _LL3322;
_LL3300: return 1; _LL3302: return 1; _LL3304: return 1; _LL3306: return 1;
_LL3308: return 1; _LL3310: return Cyc_Tcutil_bits_only( _temp3335); _LL3312:
for( 0; _temp3337 != 0; _temp3337=({ struct Cyc_List_List* _temp3355= _temp3337;
if( _temp3355 == 0){ _throw( Null_Exception);} _temp3355->tl;})){ if( ! Cyc_Tcutil_bits_only((*((
struct _tuple4*)({ struct Cyc_List_List* _temp3356= _temp3337; if( _temp3356 ==
0){ _throw( Null_Exception);} _temp3356->hd;}))).f2)){ return 0;}} return 1;
_LL3314: if( _temp3339 == 0){ return 0;}{ struct Cyc_Absyn_Structdecl* sd=({
struct Cyc_Absyn_Structdecl** _temp3363= _temp3339; if( _temp3363 == 0){ _throw(
Null_Exception);}* _temp3363;}); if( sd->fields == 0){ return 0;}{ struct
_RegionHandle _temp3357= _new_region(); struct _RegionHandle* rgn=& _temp3357;
_push_region( rgn);{ struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, sd->tvs, _temp3341);{ struct
Cyc_List_List* fs=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp3361= sd->fields;
if( _temp3361 == 0){ _throw( Null_Exception);} _temp3361->v;}); for( 0; fs != 0;
fs=({ struct Cyc_List_List* _temp3358= fs; if( _temp3358 == 0){ _throw(
Null_Exception);} _temp3358->tl;})){ if( ! Cyc_Tcutil_bits_only( Cyc_Tcutil_rsubstitute(
rgn, inst,( void*)(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp3359= fs; if( _temp3359 == 0){ _throw( Null_Exception);} _temp3359->hd;}))->type))){
int _temp3360= 0; _npop_handler( 0u); return _temp3360;}}}{ int _temp3362= 1;
_npop_handler( 0u); return _temp3362;}}; _pop_region(& _temp3357);}} _LL3316:
if( _temp3345 == 0){ return 0;}{ struct Cyc_Absyn_Uniondecl* ud=({ struct Cyc_Absyn_Uniondecl**
_temp3370= _temp3345; if( _temp3370 == 0){ _throw( Null_Exception);}* _temp3370;});
if( ud->fields == 0){ return 0;}{ struct _RegionHandle _temp3364= _new_region();
struct _RegionHandle* rgn=& _temp3364; _push_region( rgn);{ struct Cyc_List_List*
inst=(( struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct _RegionHandle*
r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn,
ud->tvs, _temp3347);{ struct Cyc_List_List* fs=( struct Cyc_List_List*)({ struct
Cyc_Core_Opt* _temp3368= ud->fields; if( _temp3368 == 0){ _throw( Null_Exception);}
_temp3368->v;}); for( 0; fs != 0; fs=({ struct Cyc_List_List* _temp3365= fs; if(
_temp3365 == 0){ _throw( Null_Exception);} _temp3365->tl;})){ if( ! Cyc_Tcutil_bits_only(
Cyc_Tcutil_rsubstitute( rgn, inst,( void*)(( struct Cyc_Absyn_Structfield*)({
struct Cyc_List_List* _temp3366= fs; if( _temp3366 == 0){ _throw( Null_Exception);}
_temp3366->hd;}))->type))){ int _temp3367= 0; _npop_handler( 0u); return
_temp3367;}}}{ int _temp3369= 1; _npop_handler( 0u); return _temp3369;}};
_pop_region(& _temp3364);}} _LL3318: _temp3353= _temp3351; goto _LL3320; _LL3320:
for( 0; _temp3353 != 0; _temp3353=({ struct Cyc_List_List* _temp3371= _temp3353;
if( _temp3371 == 0){ _throw( Null_Exception);} _temp3371->tl;})){ if( ! Cyc_Tcutil_bits_only((
void*)(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp3372=
_temp3353; if( _temp3372 == 0){ _throw( Null_Exception);} _temp3372->hd;}))->type)){
return 0;}} return 1; _LL3322: return 0; _LL3298:;} struct _tuple18{ struct Cyc_List_List*
f1; struct Cyc_Absyn_Exp* f2; } ; static int Cyc_Tcutil_cnst_exp( struct Cyc_Tcenv_Tenv*
te, int var_okay, struct Cyc_Absyn_Exp* e){ void* _temp3373=( void*) e->r; void*
_temp3411; void* _temp3413; struct Cyc_Absyn_Exp* _temp3415; struct Cyc_Absyn_Enumfield*
_temp3417; struct Cyc_Absyn_Enumdecl* _temp3419; struct _tuple1* _temp3421; void*
_temp3423; struct _tuple1* _temp3425; struct Cyc_Absyn_Exp* _temp3427; struct
Cyc_Absyn_Exp* _temp3429; struct Cyc_Absyn_Exp* _temp3431; struct Cyc_Absyn_Exp*
_temp3433; struct Cyc_Absyn_Exp* _temp3435; struct Cyc_List_List* _temp3437;
struct Cyc_Absyn_Exp* _temp3439; struct Cyc_Absyn_Exp* _temp3441; void*
_temp3443; struct Cyc_Absyn_Exp* _temp3445; struct Cyc_Absyn_Exp* _temp3447;
struct Cyc_Absyn_Exp* _temp3449; struct Cyc_Absyn_Vardecl* _temp3451; struct Cyc_List_List*
_temp3453; struct Cyc_List_List* _temp3455; void* _temp3457; struct Cyc_Absyn_Structdecl*
_temp3459; struct Cyc_List_List* _temp3461; struct Cyc_Core_Opt* _temp3463;
struct _tuple1* _temp3465; struct Cyc_List_List* _temp3467; void* _temp3469;
struct Cyc_List_List* _temp3471; struct Cyc_Absyn_Tunionfield* _temp3473; struct
Cyc_Absyn_Tuniondecl* _temp3475; struct Cyc_List_List* _temp3477; struct Cyc_Core_Opt*
_temp3479; struct Cyc_Core_Opt* _temp3481; _LL3375: if(*(( int*) _temp3373) ==
Cyc_Absyn_Const_e){ _LL3412: _temp3411=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp3373)->f1; goto _LL3376;} else{ goto _LL3377;} _LL3377: if(*(( int*)
_temp3373) == Cyc_Absyn_Sizeoftyp_e){ _LL3414: _temp3413=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp3373)->f1; goto _LL3378;} else{ goto _LL3379;} _LL3379: if(*(( int*)
_temp3373) == Cyc_Absyn_Sizeofexp_e){ _LL3416: _temp3415=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Sizeofexp_e_struct*) _temp3373)->f1; goto _LL3380;} else{ goto
_LL3381;} _LL3381: if(*(( int*) _temp3373) == Cyc_Absyn_Enum_e){ _LL3422:
_temp3421=( struct _tuple1*)(( struct Cyc_Absyn_Enum_e_struct*) _temp3373)->f1;
goto _LL3420; _LL3420: _temp3419=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp3373)->f2; goto _LL3418; _LL3418: _temp3417=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_e_struct*) _temp3373)->f3; goto _LL3382;} else{ goto
_LL3383;} _LL3383: if(*(( int*) _temp3373) == Cyc_Absyn_Var_e){ _LL3426:
_temp3425=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp3373)->f1;
goto _LL3424; _LL3424: _temp3423=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp3373)->f2; goto _LL3384;} else{ goto _LL3385;} _LL3385: if(*(( int*)
_temp3373) == Cyc_Absyn_Conditional_e){ _LL3432: _temp3431=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp3373)->f1; goto _LL3430; _LL3430:
_temp3429=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp3373)->f2; goto _LL3428; _LL3428: _temp3427=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp3373)->f3; goto _LL3386;} else{
goto _LL3387;} _LL3387: if(*(( int*) _temp3373) == Cyc_Absyn_SeqExp_e){ _LL3436:
_temp3435=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp3373)->f1; goto _LL3434; _LL3434: _temp3433=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_SeqExp_e_struct*) _temp3373)->f2; goto _LL3388;} else{ goto
_LL3389;} _LL3389: if(*(( int*) _temp3373) == Cyc_Absyn_Instantiate_e){ _LL3440:
_temp3439=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp3373)->f1; goto _LL3438; _LL3438: _temp3437=( struct Cyc_List_List*)((
struct Cyc_Absyn_Instantiate_e_struct*) _temp3373)->f2; goto _LL3390;} else{
goto _LL3391;} _LL3391: if(*(( int*) _temp3373) == Cyc_Absyn_Cast_e){ _LL3444:
_temp3443=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp3373)->f1; goto
_LL3442; _LL3442: _temp3441=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp3373)->f2; goto _LL3392;} else{ goto _LL3393;} _LL3393: if(*(( int*)
_temp3373) == Cyc_Absyn_Address_e){ _LL3446: _temp3445=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Address_e_struct*) _temp3373)->f1; goto _LL3394;} else{ goto
_LL3395;} _LL3395: if(*(( int*) _temp3373) == Cyc_Absyn_Comprehension_e){
_LL3452: _temp3451=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3373)->f1; goto _LL3450; _LL3450: _temp3449=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp3373)->f2; goto _LL3448; _LL3448:
_temp3447=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3373)->f3; goto _LL3396;} else{ goto _LL3397;} _LL3397: if(*(( int*)
_temp3373) == Cyc_Absyn_Array_e){ _LL3454: _temp3453=( struct Cyc_List_List*)((
struct Cyc_Absyn_Array_e_struct*) _temp3373)->f1; goto _LL3398;} else{ goto
_LL3399;} _LL3399: if(*(( int*) _temp3373) == Cyc_Absyn_AnonStruct_e){ _LL3458:
_temp3457=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp3373)->f1; goto
_LL3456; _LL3456: _temp3455=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp3373)->f2; goto _LL3400;} else{ goto _LL3401;} _LL3401: if(*(( int*)
_temp3373) == Cyc_Absyn_Struct_e){ _LL3466: _temp3465=( struct _tuple1*)((
struct Cyc_Absyn_Struct_e_struct*) _temp3373)->f1; goto _LL3464; _LL3464:
_temp3463=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*) _temp3373)->f2;
goto _LL3462; _LL3462: _temp3461=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp3373)->f3; goto _LL3460; _LL3460: _temp3459=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp3373)->f4; goto _LL3402;} else{ goto
_LL3403;} _LL3403: if(*(( int*) _temp3373) == Cyc_Absyn_Primop_e){ _LL3470:
_temp3469=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp3373)->f1; goto
_LL3468; _LL3468: _temp3467=( struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp3373)->f2; goto _LL3404;} else{ goto _LL3405;} _LL3405: if(*(( int*)
_temp3373) == Cyc_Absyn_Tuple_e){ _LL3472: _temp3471=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tuple_e_struct*) _temp3373)->f1; goto _LL3406;} else{ goto
_LL3407;} _LL3407: if(*(( int*) _temp3373) == Cyc_Absyn_Tunion_e){ _LL3482:
_temp3481=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp3373)->f1;
goto _LL3480; _LL3480: _temp3479=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3373)->f2; goto _LL3478; _LL3478: _temp3477=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp3373)->f3; goto _LL3476; _LL3476:
_temp3475=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3373)->f4; goto _LL3474; _LL3474: _temp3473=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp3373)->f5; goto _LL3408;} else{ goto
_LL3409;} _LL3409: goto _LL3410; _LL3376: return 1; _LL3378: return 1; _LL3380:
return Cyc_Tcutil_cnst_exp( te, 1, _temp3415); _LL3382: return 1; _LL3384: {
struct _handler_cons _temp3483; _push_handler(& _temp3483);{ int _temp3485= 0;
if( setjmp( _temp3483.handler)){ _temp3485= 1;} if( ! _temp3485){{ void*
_temp3486= Cyc_Tcenv_lookup_ordinary( te, e->loc, _temp3425); void* _temp3494;
struct Cyc_Absyn_Fndecl* _temp3496; void* _temp3498; struct Cyc_Absyn_Vardecl*
_temp3500; _LL3488: if(*(( int*) _temp3486) == Cyc_Tcenv_VarRes){ _LL3495:
_temp3494=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp3486)->f1; if((
unsigned int) _temp3494 > 1u?*(( int*) _temp3494) == Cyc_Absyn_Funname_b: 0){
_LL3497: _temp3496=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Funname_b_struct*)
_temp3494)->f1; goto _LL3489;} else{ goto _LL3490;}} else{ goto _LL3490;}
_LL3490: if(*(( int*) _temp3486) == Cyc_Tcenv_VarRes){ _LL3499: _temp3498=( void*)((
struct Cyc_Tcenv_VarRes_struct*) _temp3486)->f1; if(( unsigned int) _temp3498 >
1u?*(( int*) _temp3498) == Cyc_Absyn_Global_b: 0){ _LL3501: _temp3500=( struct
Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Global_b_struct*) _temp3498)->f1; goto
_LL3491;} else{ goto _LL3492;}} else{ goto _LL3492;} _LL3492: goto _LL3493;
_LL3489: { int _temp3502= 1; _npop_handler( 0u); return _temp3502;} _LL3491: {
void* _temp3503= Cyc_Tcutil_compress(( void*) _temp3500->type); struct Cyc_Absyn_Exp*
_temp3509; struct Cyc_Absyn_Tqual _temp3511; void* _temp3513; _LL3505: if((
unsigned int) _temp3503 > 4u?*(( int*) _temp3503) == Cyc_Absyn_ArrayType: 0){
_LL3514: _temp3513=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3503)->f1;
goto _LL3512; _LL3512: _temp3511=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp3503)->f2; goto _LL3510; _LL3510: _temp3509=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3503)->f3; goto _LL3506;} else{ goto
_LL3507;} _LL3507: goto _LL3508; _LL3506: { int _temp3515= 1; _npop_handler( 0u);
return _temp3515;} _LL3508: { int _temp3516= var_okay; _npop_handler( 0u);
return _temp3516;} _LL3504:;} _LL3493: { int _temp3517= var_okay; _npop_handler(
0u); return _temp3517;} _LL3487:;}; _pop_handler();} else{ void* _temp3484=(
void*) _exn_thrown; void* _temp3519= _temp3484; _LL3521: if( _temp3519 == Cyc_Dict_Absent){
goto _LL3522;} else{ goto _LL3523;} _LL3523: goto _LL3524; _LL3522: return 0;
_LL3524:( void) _throw( _temp3519); _LL3520:;}}} _LL3386: return( Cyc_Tcutil_cnst_exp(
te, 0, _temp3431)? Cyc_Tcutil_cnst_exp( te, 0, _temp3429): 0)? Cyc_Tcutil_cnst_exp(
te, 0, _temp3427): 0; _LL3388: return Cyc_Tcutil_cnst_exp( te, 0, _temp3435)?
Cyc_Tcutil_cnst_exp( te, 0, _temp3433): 0; _LL3390: return Cyc_Tcutil_cnst_exp(
te, var_okay, _temp3439); _LL3392: return Cyc_Tcutil_cnst_exp( te, var_okay,
_temp3441); _LL3394: return Cyc_Tcutil_cnst_exp( te, 1, _temp3445); _LL3396:
return Cyc_Tcutil_cnst_exp( te, 0, _temp3449)? Cyc_Tcutil_cnst_exp( te, 0,
_temp3447): 0; _LL3398: _temp3455= _temp3453; goto _LL3400; _LL3400: _temp3461=
_temp3455; goto _LL3402; _LL3402: for( 0; _temp3461 != 0; _temp3461=({ struct
Cyc_List_List* _temp3525= _temp3461; if( _temp3525 == 0){ _throw( Null_Exception);}
_temp3525->tl;})){ if( ! Cyc_Tcutil_cnst_exp( te, 0,(*(( struct _tuple18*)({
struct Cyc_List_List* _temp3526= _temp3461; if( _temp3526 == 0){ _throw(
Null_Exception);} _temp3526->hd;}))).f2)){ return 0;}} return 1; _LL3404:
_temp3471= _temp3467; goto _LL3406; _LL3406: _temp3477= _temp3471; goto _LL3408;
_LL3408: for( 0; _temp3477 != 0; _temp3477=({ struct Cyc_List_List* _temp3527=
_temp3477; if( _temp3527 == 0){ _throw( Null_Exception);} _temp3527->tl;})){ if(
! Cyc_Tcutil_cnst_exp( te, 0,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp3528= _temp3477; if( _temp3528 == 0){ _throw( Null_Exception);} _temp3528->hd;}))){
return 0;}} return 1; _LL3410: return 0; _LL3374:;} int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ return Cyc_Tcutil_cnst_exp(
te, 0, e);} static int Cyc_Tcutil_fields_support_default( struct Cyc_List_List*
tvs, struct Cyc_List_List* ts, struct Cyc_List_List* fs); int Cyc_Tcutil_supports_default(
void* t){ void* _temp3529= Cyc_Tcutil_compress( t); int _temp3567; struct Cyc_Core_Opt*
_temp3569; void* _temp3571; struct Cyc_Absyn_Tvar* _temp3573; struct Cyc_Absyn_TunionInfo
_temp3575; struct Cyc_Absyn_TunionFieldInfo _temp3577; struct Cyc_Absyn_PtrInfo
_temp3579; struct Cyc_Absyn_Conref* _temp3581; struct Cyc_Absyn_Tqual _temp3583;
struct Cyc_Absyn_Conref* _temp3585; void* _temp3587; void* _temp3589; void*
_temp3591; void* _temp3593; struct Cyc_Absyn_Exp* _temp3595; struct Cyc_Absyn_Tqual
_temp3597; void* _temp3599; struct Cyc_Absyn_FnInfo _temp3601; struct Cyc_List_List*
_temp3603; struct Cyc_Absyn_Structdecl** _temp3605; struct Cyc_List_List*
_temp3607; struct _tuple1* _temp3609; struct Cyc_Absyn_Uniondecl** _temp3611;
struct Cyc_List_List* _temp3613; struct _tuple1* _temp3615; struct Cyc_List_List*
_temp3617; struct Cyc_List_List* _temp3619; struct Cyc_Absyn_Enumdecl* _temp3621;
struct _tuple1* _temp3623; _LL3531: if( _temp3529 ==( void*) Cyc_Absyn_VoidType){
goto _LL3532;} else{ goto _LL3533;} _LL3533: if(( unsigned int) _temp3529 > 4u?*((
int*) _temp3529) == Cyc_Absyn_Evar: 0){ _LL3572: _temp3571=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp3529)->f1; goto _LL3570; _LL3570: _temp3569=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Evar_struct*) _temp3529)->f2; goto _LL3568; _LL3568: _temp3567=(
int)(( struct Cyc_Absyn_Evar_struct*) _temp3529)->f3; goto _LL3534;} else{ goto
_LL3535;} _LL3535: if(( unsigned int) _temp3529 > 4u?*(( int*) _temp3529) == Cyc_Absyn_VarType:
0){ _LL3574: _temp3573=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp3529)->f1; goto _LL3536;} else{ goto _LL3537;} _LL3537: if(( unsigned int)
_temp3529 > 4u?*(( int*) _temp3529) == Cyc_Absyn_TunionType: 0){ _LL3576:
_temp3575=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp3529)->f1; goto _LL3538;} else{ goto _LL3539;} _LL3539: if(( unsigned int)
_temp3529 > 4u?*(( int*) _temp3529) == Cyc_Absyn_TunionFieldType: 0){ _LL3578:
_temp3577=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp3529)->f1; goto _LL3540;} else{ goto _LL3541;} _LL3541: if(( unsigned int)
_temp3529 > 4u?*(( int*) _temp3529) == Cyc_Absyn_PointerType: 0){ _LL3580:
_temp3579=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp3529)->f1; _LL3590: _temp3589=( void*) _temp3579.elt_typ; goto _LL3588;
_LL3588: _temp3587=( void*) _temp3579.rgn_typ; goto _LL3586; _LL3586: _temp3585=(
struct Cyc_Absyn_Conref*) _temp3579.nullable; goto _LL3584; _LL3584: _temp3583=(
struct Cyc_Absyn_Tqual) _temp3579.tq; goto _LL3582; _LL3582: _temp3581=( struct
Cyc_Absyn_Conref*) _temp3579.bounds; goto _LL3542;} else{ goto _LL3543;} _LL3543:
if(( unsigned int) _temp3529 > 4u?*(( int*) _temp3529) == Cyc_Absyn_IntType: 0){
_LL3594: _temp3593=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp3529)->f1;
goto _LL3592; _LL3592: _temp3591=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp3529)->f2; goto _LL3544;} else{ goto _LL3545;} _LL3545: if( _temp3529 ==(
void*) Cyc_Absyn_FloatType){ goto _LL3546;} else{ goto _LL3547;} _LL3547: if(
_temp3529 ==( void*) Cyc_Absyn_DoubleType){ goto _LL3548;} else{ goto _LL3549;}
_LL3549: if(( unsigned int) _temp3529 > 4u?*(( int*) _temp3529) == Cyc_Absyn_ArrayType:
0){ _LL3600: _temp3599=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3529)->f1;
goto _LL3598; _LL3598: _temp3597=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp3529)->f2; goto _LL3596; _LL3596: _temp3595=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3529)->f3; goto _LL3550;} else{ goto
_LL3551;} _LL3551: if(( unsigned int) _temp3529 > 4u?*(( int*) _temp3529) == Cyc_Absyn_FnType:
0){ _LL3602: _temp3601=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp3529)->f1; goto _LL3552;} else{ goto _LL3553;} _LL3553: if(( unsigned int)
_temp3529 > 4u?*(( int*) _temp3529) == Cyc_Absyn_TupleType: 0){ _LL3604:
_temp3603=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp3529)->f1; goto _LL3554;} else{ goto _LL3555;} _LL3555: if(( unsigned int)
_temp3529 > 4u?*(( int*) _temp3529) == Cyc_Absyn_StructType: 0){ _LL3610:
_temp3609=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp3529)->f1;
goto _LL3608; _LL3608: _temp3607=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp3529)->f2; goto _LL3606; _LL3606: _temp3605=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp3529)->f3; goto _LL3556;} else{ goto
_LL3557;} _LL3557: if(( unsigned int) _temp3529 > 4u?*(( int*) _temp3529) == Cyc_Absyn_UnionType:
0){ _LL3616: _temp3615=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp3529)->f1; goto _LL3614; _LL3614: _temp3613=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnionType_struct*) _temp3529)->f2; goto _LL3612; _LL3612:
_temp3611=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp3529)->f3; goto _LL3558;} else{ goto _LL3559;} _LL3559: if(( unsigned int)
_temp3529 > 4u?*(( int*) _temp3529) == Cyc_Absyn_AnonStructType: 0){ _LL3618:
_temp3617=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp3529)->f1; goto _LL3560;} else{ goto _LL3561;} _LL3561: if(( unsigned int)
_temp3529 > 4u?*(( int*) _temp3529) == Cyc_Absyn_AnonUnionType: 0){ _LL3620:
_temp3619=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp3529)->f1; goto _LL3562;} else{ goto _LL3563;} _LL3563: if(( unsigned int)
_temp3529 > 4u?*(( int*) _temp3529) == Cyc_Absyn_EnumType: 0){ _LL3624:
_temp3623=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*) _temp3529)->f1;
goto _LL3622; _LL3622: _temp3621=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*)
_temp3529)->f2; goto _LL3564;} else{ goto _LL3565;} _LL3565: goto _LL3566;
_LL3532: return 1; _LL3534: return 0; _LL3536: return 0; _LL3538: return 0;
_LL3540: return 0; _LL3542: { void* _temp3625=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp3581))->v; void*
_temp3633; void* _temp3635; struct Cyc_Absyn_Exp* _temp3637; _LL3627: if((
unsigned int) _temp3625 > 1u?*(( int*) _temp3625) == Cyc_Absyn_Eq_constr: 0){
_LL3634: _temp3633=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3625)->f1;
if( _temp3633 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL3628;} else{ goto _LL3629;}}
else{ goto _LL3629;} _LL3629: if(( unsigned int) _temp3625 > 1u?*(( int*)
_temp3625) == Cyc_Absyn_Eq_constr: 0){ _LL3636: _temp3635=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3625)->f1; if(( unsigned int) _temp3635 > 1u?*(( int*) _temp3635) == Cyc_Absyn_Upper_b:
0){ _LL3638: _temp3637=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp3635)->f1; goto _LL3630;} else{ goto _LL3631;}} else{ goto _LL3631;}
_LL3631: goto _LL3632; _LL3628: return 1; _LL3630: { void* _temp3639=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp3585))->v; int _temp3645; _LL3641: if(( unsigned int) _temp3639 > 1u?*((
int*) _temp3639) == Cyc_Absyn_Eq_constr: 0){ _LL3646: _temp3645=( int)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp3639)->f1; goto _LL3642;} else{ goto _LL3643;}
_LL3643: goto _LL3644; _LL3642: return _temp3645; _LL3644: return 0; _LL3640:;}
_LL3632: return 0; _LL3626:;} _LL3544: return 1; _LL3546: return 1; _LL3548:
return 1; _LL3550: return Cyc_Tcutil_supports_default( _temp3599); _LL3552:
return 0; _LL3554: for( 0; _temp3603 != 0; _temp3603=({ struct Cyc_List_List*
_temp3647= _temp3603; if( _temp3647 == 0){ _throw( Null_Exception);} _temp3647->tl;})){
if( ! Cyc_Tcutil_supports_default((*(( struct _tuple4*)({ struct Cyc_List_List*
_temp3648= _temp3603; if( _temp3648 == 0){ _throw( Null_Exception);} _temp3648->hd;}))).f2)){
return 0;}} return 1; _LL3556: if( _temp3605 == 0){ return 0;}{ struct Cyc_Absyn_Structdecl*
sd=({ struct Cyc_Absyn_Structdecl** _temp3650= _temp3605; if( _temp3650 == 0){
_throw( Null_Exception);}* _temp3650;}); if( sd->fields == 0){ return 0;} return
Cyc_Tcutil_fields_support_default( sd->tvs, _temp3607,( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp3649= sd->fields; if( _temp3649 == 0){ _throw(
Null_Exception);} _temp3649->v;}));} _LL3558: if( _temp3611 == 0){ return 0;}{
struct Cyc_Absyn_Uniondecl* ud=({ struct Cyc_Absyn_Uniondecl** _temp3652=
_temp3611; if( _temp3652 == 0){ _throw( Null_Exception);}* _temp3652;}); if( ud->fields
== 0){ return 0;} return Cyc_Tcutil_fields_support_default( ud->tvs, _temp3613,(
struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp3651= ud->fields; if(
_temp3651 == 0){ _throw( Null_Exception);} _temp3651->v;}));} _LL3560: _temp3619=
_temp3617; goto _LL3562; _LL3562: return Cyc_Tcutil_fields_support_default( 0, 0,
_temp3619); _LL3564: return 1; _LL3566: return 0; _LL3530:;} static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List* tvs, struct Cyc_List_List* ts, struct Cyc_List_List* fs){{
struct _RegionHandle _temp3653= _new_region(); struct _RegionHandle* rgn=&
_temp3653; _push_region( rgn);{ struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, tvs, ts); for( 0; fs != 0;
fs=({ struct Cyc_List_List* _temp3654= fs; if( _temp3654 == 0){ _throw(
Null_Exception);} _temp3654->tl;})){ void* t= Cyc_Tcutil_rsubstitute( rgn, inst,(
void*)(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp3656= fs;
if( _temp3656 == 0){ _throw( Null_Exception);} _temp3656->hd;}))->type); if( !
Cyc_Tcutil_supports_default( t)){ int _temp3655= 0; _npop_handler( 0u); return
_temp3655;}}}; _pop_region(& _temp3653);} return 1;}