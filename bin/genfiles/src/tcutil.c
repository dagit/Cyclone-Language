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
struct _tuple4*); extern void* Cyc_Tcexp_tcExp( struct Cyc_Tcenv_Tenv*, void**,
struct Cyc_Absyn_Exp*); char Cyc_Tcutil_TypeErr[ 12u]; extern void Cyc_Tcutil_unify_it(
void* t1, void* t2); void Cyc_Tcutil_terr( struct Cyc_Position_Segment* loc,
struct _tagged_string s){ Cyc_Position_post_error( Cyc_Position_mk_err_elab( loc,
s));} void* Cyc_Tcutil_impos( struct _tagged_string msg){({ struct
_tagged_string _temp0= msg; fprintf( Cyc_Stdio_stderr,"Error: %.*s\n", _temp0.last_plus_one
- _temp0.curr, _temp0.curr);}); Cyc_Stdio_fflush( Cyc_Stdio_stderr);( void)
_throw(( void*) Cyc_Tcutil_TypeErr);} static struct _tagged_string Cyc_Tcutil_tvar2string(
struct Cyc_Absyn_Tvar* tv){ return* tv->name;} void Cyc_Tcutil_print_tvars(
struct Cyc_List_List* tvs){ for( 0; tvs != 0; tvs=({ struct Cyc_List_List*
_temp1= tvs; if( _temp1 == 0){ _throw( Null_Exception);} _temp1->tl;})){({
struct _tagged_string _temp4= Cyc_Tcutil_tvar2string(( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp2= tvs; if( _temp2 == 0){ _throw( Null_Exception);}
_temp2->hd;})); struct _tagged_string _temp5= Cyc_Absynpp_ckind2string((( struct
Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp3= tvs; if( _temp3 == 0){ _throw(
Null_Exception);} _temp3->hd;}))->kind); fprintf( Cyc_Stdio_stderr,"%.*s::%.*s ",
_temp4.last_plus_one - _temp4.curr, _temp4.curr, _temp5.last_plus_one - _temp5.curr,
_temp5.curr);});} fprintf( Cyc_Stdio_stderr,"\n"); Cyc_Stdio_fflush( Cyc_Stdio_stderr);}
static struct Cyc_List_List* Cyc_Tcutil_warning_segs= 0; static struct Cyc_List_List*
Cyc_Tcutil_warning_msgs= 0; void Cyc_Tcutil_warn( struct Cyc_Position_Segment*
sg, struct _tagged_string msg){ Cyc_Tcutil_warning_segs=({ struct Cyc_List_List*
_temp6=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp6->hd=( void*) sg; _temp6->tl= Cyc_Tcutil_warning_segs; _temp6;}); Cyc_Tcutil_warning_msgs=({
struct Cyc_List_List* _temp7=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp7->hd=( void*)(( struct _tagged_string*)({ struct
_tagged_string* _temp8=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp8[ 0]= msg; _temp8;})); _temp7->tl= Cyc_Tcutil_warning_msgs;
_temp7;});} void Cyc_Tcutil_flush_warnings(){ if( Cyc_Tcutil_warning_segs == 0){
return;} fprintf( Cyc_Stdio_stderr,"***Warnings***\n");{ struct Cyc_List_List*
seg_strs= Cyc_Position_strings_of_segments( Cyc_Tcutil_warning_segs); Cyc_Tcutil_warning_segs=
0; Cyc_Tcutil_warning_msgs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( Cyc_Tcutil_warning_msgs); while( Cyc_Tcutil_warning_msgs != 0) {({
struct _tagged_string _temp11=*(( struct _tagged_string*)({ struct Cyc_List_List*
_temp9= seg_strs; if( _temp9 == 0){ _throw( Null_Exception);} _temp9->hd;}));
struct _tagged_string _temp12=*(( struct _tagged_string*)({ struct Cyc_List_List*
_temp10= Cyc_Tcutil_warning_msgs; if( _temp10 == 0){ _throw( Null_Exception);}
_temp10->hd;})); fprintf( Cyc_Stdio_stderr,"%.*s: %.*s\n", _temp11.last_plus_one
- _temp11.curr, _temp11.curr, _temp12.last_plus_one - _temp12.curr, _temp12.curr);});
seg_strs=({ struct Cyc_List_List* _temp13= seg_strs; if( _temp13 == 0){ _throw(
Null_Exception);} _temp13->tl;}); Cyc_Tcutil_warning_msgs=({ struct Cyc_List_List*
_temp14= Cyc_Tcutil_warning_msgs; if( _temp14 == 0){ _throw( Null_Exception);}
_temp14->tl;});} fprintf( Cyc_Stdio_stderr,"**************\n"); Cyc_Stdio_fflush(
Cyc_Stdio_stderr);}} struct Cyc_Core_Opt* Cyc_Tcutil_empty_var_set= 0; void* Cyc_Tcutil_compress(
void* t){ void* _temp15= t; int _temp27; struct Cyc_Core_Opt* _temp29; void*
_temp31; int _temp33; struct Cyc_Core_Opt* _temp35; struct Cyc_Core_Opt**
_temp37; void* _temp38; struct Cyc_Core_Opt* _temp40; struct Cyc_List_List*
_temp42; struct _tuple1* _temp44; struct Cyc_Core_Opt* _temp46; struct Cyc_Core_Opt**
_temp48; struct Cyc_List_List* _temp49; struct _tuple1* _temp51; _LL17: if((
unsigned int) _temp15 > 4u?*(( int*) _temp15) == Cyc_Absyn_Evar: 0){ _LL32:
_temp31=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp15)->f1; goto _LL30;
_LL30: _temp29=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp15)->f2;
if( _temp29 == 0){ goto _LL28;} else{ goto _LL19;} _LL28: _temp27=( int)((
struct Cyc_Absyn_Evar_struct*) _temp15)->f3; goto _LL18;} else{ goto _LL19;}
_LL19: if(( unsigned int) _temp15 > 4u?*(( int*) _temp15) == Cyc_Absyn_Evar: 0){
_LL39: _temp38=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp15)->f1; goto
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
_handler_cons _temp245; _push_handler(& _temp245);{ void* _temp246=( void*)
setjmp( _temp245.handler); if( ! _temp246){ Cyc_Tcutil_unify_it( t1, t2);{ int
_temp247= 1; _npop_handler( 0u); return _temp247;}; _pop_handler();} else{ void*
_temp249= _temp246; _LL251: if( _temp249 == Cyc_Tcutil_Unify){ goto _LL252;}
else{ goto _LL253;} _LL253: goto _LL254; _LL252: return 0; _LL254:( void) _throw(
_temp249); _LL250:;}}} static void Cyc_Tcutil_occurslist( void* evar, struct Cyc_List_List*
ts); static void Cyc_Tcutil_occurs( void* evar, void* t){ void* _temp255= Cyc_Tcutil_compress(
t); int _temp287; struct Cyc_Core_Opt* _temp289; void* _temp291; struct Cyc_Absyn_PtrInfo
_temp293; struct Cyc_Absyn_Exp* _temp295; struct Cyc_Absyn_Tqual _temp297; void*
_temp299; struct Cyc_Absyn_FnInfo _temp301; struct Cyc_List_List* _temp303; int
_temp305; struct Cyc_List_List* _temp307; void* _temp309; struct Cyc_Core_Opt*
_temp311; struct Cyc_List_List* _temp313; struct Cyc_List_List* _temp315; void*
_temp317; struct Cyc_Absyn_TunionInfo _temp319; void* _temp321; struct Cyc_List_List*
_temp323; void* _temp325; struct Cyc_Core_Opt* _temp327; struct Cyc_List_List*
_temp329; struct _tuple1* _temp331; struct Cyc_Absyn_TunionFieldInfo _temp333;
struct Cyc_List_List* _temp335; void* _temp337; struct Cyc_Absyn_Structdecl**
_temp339; struct Cyc_List_List* _temp341; struct _tuple1* _temp343; struct Cyc_List_List*
_temp345; struct Cyc_List_List* _temp347; void* _temp349; struct Cyc_List_List*
_temp351; _LL257: if(( unsigned int) _temp255 > 4u?*(( int*) _temp255) == Cyc_Absyn_Evar:
0){ _LL292: _temp291=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp255)->f1;
goto _LL290; _LL290: _temp289=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp255)->f2; goto _LL288; _LL288: _temp287=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp255)->f3; goto _LL258;} else{ goto _LL259;} _LL259: if(( unsigned int)
_temp255 > 4u?*(( int*) _temp255) == Cyc_Absyn_PointerType: 0){ _LL294: _temp293=(
struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp255)->f1;
goto _LL260;} else{ goto _LL261;} _LL261: if(( unsigned int) _temp255 > 4u?*((
int*) _temp255) == Cyc_Absyn_ArrayType: 0){ _LL300: _temp299=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp255)->f1; goto _LL298; _LL298: _temp297=(
struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*) _temp255)->f2;
goto _LL296; _LL296: _temp295=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp255)->f3; goto _LL262;} else{ goto _LL263;} _LL263: if(( unsigned int)
_temp255 > 4u?*(( int*) _temp255) == Cyc_Absyn_FnType: 0){ _LL302: _temp301=(
struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*) _temp255)->f1;
_LL314: _temp313=( struct Cyc_List_List*) _temp301.tvars; goto _LL312; _LL312:
_temp311=( struct Cyc_Core_Opt*) _temp301.effect; goto _LL310; _LL310: _temp309=(
void*) _temp301.ret_typ; goto _LL308; _LL308: _temp307=( struct Cyc_List_List*)
_temp301.args; goto _LL306; _LL306: _temp305=( int) _temp301.varargs; goto
_LL304; _LL304: _temp303=( struct Cyc_List_List*) _temp301.attributes; goto
_LL264;} else{ goto _LL265;} _LL265: if(( unsigned int) _temp255 > 4u?*(( int*)
_temp255) == Cyc_Absyn_TupleType: 0){ _LL316: _temp315=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp255)->f1; goto _LL266;} else{ goto
_LL267;} _LL267: if(( unsigned int) _temp255 > 4u?*(( int*) _temp255) == Cyc_Absyn_RgnHandleType:
0){ _LL318: _temp317=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp255)->f1;
goto _LL268;} else{ goto _LL269;} _LL269: if(( unsigned int) _temp255 > 4u?*((
int*) _temp255) == Cyc_Absyn_TunionType: 0){ _LL320: _temp319=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp255)->f1; _LL326: _temp325=( void*)
_temp319.tunion_info; goto _LL324; _LL324: _temp323=( struct Cyc_List_List*)
_temp319.targs; goto _LL322; _LL322: _temp321=( void*) _temp319.rgn; goto _LL270;}
else{ goto _LL271;} _LL271: if(( unsigned int) _temp255 > 4u?*(( int*) _temp255)
== Cyc_Absyn_TypedefType: 0){ _LL332: _temp331=( struct _tuple1*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp255)->f1; goto _LL330; _LL330: _temp329=( struct Cyc_List_List*)(( struct
Cyc_Absyn_TypedefType_struct*) _temp255)->f2; goto _LL328; _LL328: _temp327=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_TypedefType_struct*) _temp255)->f3;
goto _LL272;} else{ goto _LL273;} _LL273: if(( unsigned int) _temp255 > 4u?*((
int*) _temp255) == Cyc_Absyn_TunionFieldType: 0){ _LL334: _temp333=( struct Cyc_Absyn_TunionFieldInfo)((
struct Cyc_Absyn_TunionFieldType_struct*) _temp255)->f1; _LL338: _temp337=( void*)
_temp333.field_info; goto _LL336; _LL336: _temp335=( struct Cyc_List_List*)
_temp333.targs; goto _LL274;} else{ goto _LL275;} _LL275: if(( unsigned int)
_temp255 > 4u?*(( int*) _temp255) == Cyc_Absyn_StructType: 0){ _LL344: _temp343=(
struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp255)->f1; goto
_LL342; _LL342: _temp341=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp255)->f2; goto _LL340; _LL340: _temp339=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp255)->f3; goto _LL276;} else{ goto
_LL277;} _LL277: if(( unsigned int) _temp255 > 4u?*(( int*) _temp255) == Cyc_Absyn_AnonStructType:
0){ _LL346: _temp345=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp255)->f1; goto _LL278;} else{ goto _LL279;} _LL279: if(( unsigned int)
_temp255 > 4u?*(( int*) _temp255) == Cyc_Absyn_AnonUnionType: 0){ _LL348:
_temp347=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp255)->f1; goto _LL280;} else{ goto _LL281;} _LL281: if(( unsigned int)
_temp255 > 4u?*(( int*) _temp255) == Cyc_Absyn_AccessEff: 0){ _LL350: _temp349=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp255)->f1; goto _LL282;} else{
goto _LL283;} _LL283: if(( unsigned int) _temp255 > 4u?*(( int*) _temp255) ==
Cyc_Absyn_JoinEff: 0){ _LL352: _temp351=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*)
_temp255)->f1; goto _LL284;} else{ goto _LL285;} _LL285: goto _LL286; _LL258:
if( t == evar){( void) _throw(( void*) Cyc_Tcutil_Unify);} else{ if( _temp289 !=
0){ Cyc_Tcutil_occurs( evar,( void*)({ struct Cyc_Core_Opt* _temp353= _temp289;
if( _temp353 == 0){ _throw( Null_Exception);} _temp353->v;}));}} goto _LL256;
_LL260: Cyc_Tcutil_occurs( evar,( void*) _temp293.elt_typ); Cyc_Tcutil_occurs(
evar,( void*) _temp293.rgn_typ); goto _LL256; _LL262: Cyc_Tcutil_occurs( evar,
_temp299); goto _LL256; _LL264: if( _temp311 != 0){ Cyc_Tcutil_occurs( evar,(
void*)({ struct Cyc_Core_Opt* _temp354= _temp311; if( _temp354 == 0){ _throw(
Null_Exception);} _temp354->v;}));} Cyc_Tcutil_occurs( evar, _temp309); for( 0;
_temp307 != 0; _temp307=({ struct Cyc_List_List* _temp355= _temp307; if(
_temp355 == 0){ _throw( Null_Exception);} _temp355->tl;})){ Cyc_Tcutil_occurs(
evar,(*(( struct _tuple2*)({ struct Cyc_List_List* _temp356= _temp307; if(
_temp356 == 0){ _throw( Null_Exception);} _temp356->hd;}))).f3);} goto _LL256;
_LL266: for( 0; _temp315 != 0; _temp315=({ struct Cyc_List_List* _temp357=
_temp315; if( _temp357 == 0){ _throw( Null_Exception);} _temp357->tl;})){ Cyc_Tcutil_occurs(
evar,(*(( struct _tuple4*)({ struct Cyc_List_List* _temp358= _temp315; if(
_temp358 == 0){ _throw( Null_Exception);} _temp358->hd;}))).f2);} goto _LL256;
_LL268: Cyc_Tcutil_occurs( evar, _temp317); goto _LL256; _LL270: Cyc_Tcutil_occurs(
evar, _temp321); Cyc_Tcutil_occurslist( evar, _temp323); goto _LL256; _LL272:
_temp335= _temp329; goto _LL274; _LL274: _temp341= _temp335; goto _LL276; _LL276:
Cyc_Tcutil_occurslist( evar, _temp341); goto _LL256; _LL278: _temp347= _temp345;
goto _LL280; _LL280: for( 0; _temp347 != 0; _temp347=({ struct Cyc_List_List*
_temp359= _temp347; if( _temp359 == 0){ _throw( Null_Exception);} _temp359->tl;})){
Cyc_Tcutil_occurs( evar,( void*)(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp360= _temp347; if( _temp360 == 0){ _throw( Null_Exception);} _temp360->hd;}))->type);}
goto _LL256; _LL282: Cyc_Tcutil_occurs( evar, _temp349); goto _LL256; _LL284:
Cyc_Tcutil_occurslist( evar, _temp351); goto _LL256; _LL286: goto _LL256; _LL256:;}
static void Cyc_Tcutil_occurslist( void* evar, struct Cyc_List_List* ts){ for( 0;
ts != 0; ts=({ struct Cyc_List_List* _temp361= ts; if( _temp361 == 0){ _throw(
Null_Exception);} _temp361->tl;})){ Cyc_Tcutil_occurs( evar,( void*)({ struct
Cyc_List_List* _temp362= ts; if( _temp362 == 0){ _throw( Null_Exception);}
_temp362->hd;}));}} static void Cyc_Tcutil_unify_list( struct Cyc_List_List* t1,
struct Cyc_List_List* t2){ struct _handler_cons _temp363; _push_handler(&
_temp363);{ void* _temp364=( void*) setjmp( _temp363.handler); if( ! _temp364){((
void(*)( void(* f)( void*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_iter2)( Cyc_Tcutil_unify_it, t1, t2);; _pop_handler();} else{ void*
_temp366= _temp364; _LL368: if( _temp366 == Cyc_List_List_mismatch){ goto _LL369;}
else{ goto _LL370;} _LL370: goto _LL371; _LL369:( void) _throw(( void*) Cyc_Tcutil_Unify);
_LL371:( void) _throw( _temp366); _LL367:;}}} static void Cyc_Tcutil_unify_tqual(
struct Cyc_Absyn_Tqual tq1, struct Cyc_Absyn_Tqual tq2){ if(( tq1.q_const != tq2.q_const?
1: tq1.q_volatile != tq2.q_volatile)? 1: tq1.q_restrict != tq2.q_restrict){(
void) _throw(( void*) Cyc_Tcutil_Unify);}} int Cyc_Tcutil_equal_tqual( struct
Cyc_Absyn_Tqual tq1, struct Cyc_Absyn_Tqual tq2){ return( tq1.q_const == tq2.q_const?
tq1.q_volatile == tq2.q_volatile: 0)? tq1.q_restrict == tq2.q_restrict: 0;}
static void Cyc_Tcutil_unify_it_conrefs( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref*
x, struct Cyc_Absyn_Conref* y){ x=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( x); y=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( y); if( x == y){ return;}{ void* _temp372=( void*)
x->v; struct Cyc_Absyn_Conref* _temp380; void* _temp382; _LL374: if( _temp372 ==(
void*) Cyc_Absyn_No_constr){ goto _LL375;} else{ goto _LL376;} _LL376: if((
unsigned int) _temp372 > 1u?*(( int*) _temp372) == Cyc_Absyn_Forward_constr: 0){
_LL381: _temp380=( struct Cyc_Absyn_Conref*)(( struct Cyc_Absyn_Forward_constr_struct*)
_temp372)->f1; goto _LL377;} else{ goto _LL378;} _LL378: if(( unsigned int)
_temp372 > 1u?*(( int*) _temp372) == Cyc_Absyn_Eq_constr: 0){ _LL383: _temp382=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp372)->f1; goto _LL379;} else{
goto _LL373;} _LL375:( void*)( x->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp384=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp384[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp385; _temp385.tag= Cyc_Absyn_Forward_constr; _temp385.f1= y; _temp385;});
_temp384;}))); return; _LL377:( void) _throw((( void*(*)( struct _tagged_string
msg)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp386=( char*)"unify_conref: forward after compress";
struct _tagged_string _temp387; _temp387.curr= _temp386; _temp387.base= _temp386;
_temp387.last_plus_one= _temp386 + 37; _temp387;}))); _LL379: { void* _temp388=(
void*) y->v; struct Cyc_Absyn_Conref* _temp396; void* _temp398; _LL390: if(
_temp388 ==( void*) Cyc_Absyn_No_constr){ goto _LL391;} else{ goto _LL392;}
_LL392: if(( unsigned int) _temp388 > 1u?*(( int*) _temp388) == Cyc_Absyn_Forward_constr:
0){ _LL397: _temp396=( struct Cyc_Absyn_Conref*)(( struct Cyc_Absyn_Forward_constr_struct*)
_temp388)->f1; goto _LL393;} else{ goto _LL394;} _LL394: if(( unsigned int)
_temp388 > 1u?*(( int*) _temp388) == Cyc_Absyn_Eq_constr: 0){ _LL399: _temp398=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp388)->f1; goto _LL395;} else{
goto _LL389;} _LL391:( void*)( y->v=( void*)(( void*) x->v)); return; _LL393:(
void) _throw((( void*(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ char* _temp400=( char*)"unify_conref: forward after compress(2)";
struct _tagged_string _temp401; _temp401.curr= _temp400; _temp401.base= _temp400;
_temp401.last_plus_one= _temp400 + 40; _temp401;}))); _LL395: if( cmp( _temp382,
_temp398) != 0){( void) _throw(( void*) Cyc_Tcutil_Unify);} return; _LL389:;}
_LL373:;}} static int Cyc_Tcutil_unify_conrefs( int(* cmp)( void*, void*),
struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y){ struct _handler_cons
_temp402; _push_handler(& _temp402);{ void* _temp403=( void*) setjmp( _temp402.handler);
if( ! _temp403){(( void(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref*
x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_it_conrefs)( cmp, x, y);{ int
_temp404= 1; _npop_handler( 0u); return _temp404;}; _pop_handler();} else{ void*
_temp406= _temp403; _LL408: if( _temp406 == Cyc_Tcutil_Unify){ goto _LL409;}
else{ goto _LL410;} _LL410: goto _LL411; _LL409: return 0; _LL411:( void) _throw(
_temp406); _LL407:;}}} static int Cyc_Tcutil_boundscmp( void* b1, void* b2){
struct _tuple7 _temp413=({ struct _tuple7 _temp412; _temp412.f1= b1; _temp412.f2=
b2; _temp412;}); void* _temp423; void* _temp425; void* _temp427; void* _temp429;
void* _temp431; void* _temp433; void* _temp435; struct Cyc_Absyn_Exp* _temp437;
void* _temp439; struct Cyc_Absyn_Exp* _temp441; _LL415: _LL426: _temp425=
_temp413.f1; if( _temp425 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL424;} else{
goto _LL417;} _LL424: _temp423= _temp413.f2; if( _temp423 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL416;} else{ goto _LL417;} _LL417: _LL430: _temp429= _temp413.f1; if(
_temp429 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL428;} else{ goto _LL419;}
_LL428: _temp427= _temp413.f2; goto _LL418; _LL419: _LL434: _temp433= _temp413.f1;
goto _LL432; _LL432: _temp431= _temp413.f2; if( _temp431 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL420;} else{ goto _LL421;} _LL421: _LL440: _temp439= _temp413.f1; if((
unsigned int) _temp439 > 1u?*(( int*) _temp439) == Cyc_Absyn_Upper_b: 0){ _LL442:
_temp441=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp439)->f1;
goto _LL436;} else{ goto _LL414;} _LL436: _temp435= _temp413.f2; if((
unsigned int) _temp435 > 1u?*(( int*) _temp435) == Cyc_Absyn_Upper_b: 0){ _LL438:
_temp437=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp435)->f1;
goto _LL422;} else{ goto _LL414;} _LL416: return 0; _LL418: return - 1; _LL420:
return 1; _LL422: { int i1=( int) Cyc_Evexp_eval_const_uint_exp( _temp441); int
i2=( int) Cyc_Evexp_eval_const_uint_exp( _temp437); if( i1 == i2){ return 0;}
if( i1 < i2){ return - 1;} return 1;} _LL414:;} static int Cyc_Tcutil_equal_att(
void* a1, void* a2){ if( a1 == a2){ return 1;}{ struct _tuple7 _temp444=({
struct _tuple7 _temp443; _temp443.f1= a1; _temp443.f2= a2; _temp443;}); void*
_temp454; int _temp456; void* _temp458; int _temp460; void* _temp462; int
_temp464; void* _temp466; int _temp468; void* _temp470; struct _tagged_string
_temp472; void* _temp474; struct _tagged_string _temp476; _LL446: _LL459:
_temp458= _temp444.f1; if(( unsigned int) _temp458 > 15u?*(( int*) _temp458) ==
Cyc_Absyn_Regparm_att: 0){ _LL461: _temp460=( int)(( struct Cyc_Absyn_Regparm_att_struct*)
_temp458)->f1; goto _LL455;} else{ goto _LL448;} _LL455: _temp454= _temp444.f2;
if(( unsigned int) _temp454 > 15u?*(( int*) _temp454) == Cyc_Absyn_Regparm_att:
0){ _LL457: _temp456=( int)(( struct Cyc_Absyn_Regparm_att_struct*) _temp454)->f1;
goto _LL447;} else{ goto _LL448;} _LL448: _LL467: _temp466= _temp444.f1; if((
unsigned int) _temp466 > 15u?*(( int*) _temp466) == Cyc_Absyn_Aligned_att: 0){
_LL469: _temp468=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp466)->f1;
goto _LL463;} else{ goto _LL450;} _LL463: _temp462= _temp444.f2; if((
unsigned int) _temp462 > 15u?*(( int*) _temp462) == Cyc_Absyn_Aligned_att: 0){
_LL465: _temp464=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp462)->f1;
goto _LL449;} else{ goto _LL450;} _LL450: _LL475: _temp474= _temp444.f1; if((
unsigned int) _temp474 > 15u?*(( int*) _temp474) == Cyc_Absyn_Section_att: 0){
_LL477: _temp476=( struct _tagged_string)(( struct Cyc_Absyn_Section_att_struct*)
_temp474)->f1; goto _LL471;} else{ goto _LL452;} _LL471: _temp470= _temp444.f2;
if(( unsigned int) _temp470 > 15u?*(( int*) _temp470) == Cyc_Absyn_Section_att:
0){ _LL473: _temp472=( struct _tagged_string)(( struct Cyc_Absyn_Section_att_struct*)
_temp470)->f1; goto _LL451;} else{ goto _LL452;} _LL452: goto _LL453; _LL447:
_temp468= _temp460; _temp464= _temp456; goto _LL449; _LL449: return _temp468 ==
_temp464; _LL451: return Cyc_String_strcmp( _temp476, _temp472) == 0; _LL453:
return 0; _LL445:;}} int Cyc_Tcutil_same_atts( struct Cyc_List_List* a1, struct
Cyc_List_List* a2){{ struct Cyc_List_List* a= a1; for( 0; a != 0; a=({ struct
Cyc_List_List* _temp478= a; if( _temp478 == 0){ _throw( Null_Exception);}
_temp478->tl;})){ if( !(( int(*)( int(* pred)( void*, void*), void* env, struct
Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcutil_equal_att,( void*)({ struct
Cyc_List_List* _temp479= a; if( _temp479 == 0){ _throw( Null_Exception);}
_temp479->hd;}), a2)){ return 0;}}}{ struct Cyc_List_List* a= a2; for( 0; a != 0;
a=({ struct Cyc_List_List* _temp480= a; if( _temp480 == 0){ _throw(
Null_Exception);} _temp480->tl;})){ if( !(( int(*)( int(* pred)( void*, void*),
void* env, struct Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcutil_equal_att,(
void*)({ struct Cyc_List_List* _temp481= a; if( _temp481 == 0){ _throw(
Null_Exception);} _temp481->hd;}), a1)){ return 0;}}} return 1;} static void Cyc_Tcutil_normalize_effect(
void* e){ e= Cyc_Tcutil_compress( e);{ void* _temp482= e; struct Cyc_List_List*
_temp488; struct Cyc_List_List** _temp490; _LL484: if(( unsigned int) _temp482 >
4u?*(( int*) _temp482) == Cyc_Absyn_JoinEff: 0){ _LL489: _temp488=( struct Cyc_List_List*)((
struct Cyc_Absyn_JoinEff_struct*) _temp482)->f1; _temp490=&(( struct Cyc_Absyn_JoinEff_struct*)
_temp482)->f1; goto _LL485;} else{ goto _LL486;} _LL486: goto _LL487; _LL485: {
int nested_join= 0;{ struct Cyc_List_List* effs=* _temp490; for( 0; effs != 0;
effs=({ struct Cyc_List_List* _temp491= effs; if( _temp491 == 0){ _throw(
Null_Exception);} _temp491->tl;})){ void* eff=( void*)({ struct Cyc_List_List*
_temp500= effs; if( _temp500 == 0){ _throw( Null_Exception);} _temp500->hd;});
Cyc_Tcutil_normalize_effect( eff);{ void* _temp492= Cyc_Tcutil_compress( eff);
struct Cyc_List_List* _temp498; _LL494: if(( unsigned int) _temp492 > 4u?*(( int*)
_temp492) == Cyc_Absyn_JoinEff: 0){ _LL499: _temp498=( struct Cyc_List_List*)((
struct Cyc_Absyn_JoinEff_struct*) _temp492)->f1; goto _LL495;} else{ goto _LL496;}
_LL496: goto _LL497; _LL495: nested_join= 1; goto _LL493; _LL497: goto _LL493;
_LL493:;}}} if( ! nested_join){ return;}{ struct Cyc_List_List* effects= 0;{
struct Cyc_List_List* effs=* _temp490; for( 0; effs != 0; effs=({ struct Cyc_List_List*
_temp501= effs; if( _temp501 == 0){ _throw( Null_Exception);} _temp501->tl;})){
void* _temp503= Cyc_Tcutil_compress(( void*)({ struct Cyc_List_List* _temp502=
effs; if( _temp502 == 0){ _throw( Null_Exception);} _temp502->hd;})); struct Cyc_List_List*
_temp509; _LL505: if(( unsigned int) _temp503 > 4u?*(( int*) _temp503) == Cyc_Absyn_JoinEff:
0){ _LL510: _temp509=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*)
_temp503)->f1; goto _LL506;} else{ goto _LL507;} _LL507: goto _LL508; _LL506:
effects=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_revappend)( _temp509, effects); goto _LL504; _LL508: effects=({
struct Cyc_List_List* _temp511=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp511->hd=( void*) _temp503; _temp511->tl= effects;
_temp511;}); goto _LL504; _LL504:;}}* _temp490=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( effects); goto _LL483;}} _LL487:
goto _LL483; _LL483:;}} struct _tuple8{ void* f1; struct Cyc_List_List* f2; } ;
static struct _tuple8* Cyc_Tcutil_get_effect_evar( void* t){ void* _temp512= Cyc_Tcutil_compress(
t); struct Cyc_List_List* _temp520; struct Cyc_List_List _temp522; struct Cyc_List_List*
_temp523; void* _temp525; int _temp527; struct Cyc_Core_Opt* _temp529; struct
Cyc_Core_Opt** _temp531; void* _temp532; void** _temp534; _LL514: if((
unsigned int) _temp512 > 4u?*(( int*) _temp512) == Cyc_Absyn_JoinEff: 0){ _LL521:
_temp520=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp512)->f1;
if( _temp520 == 0){ goto _LL516;} else{ _temp522=* _temp520; _LL526: _temp525=(
void*) _temp522.hd; goto _LL524; _LL524: _temp523=( struct Cyc_List_List*)
_temp522.tl; goto _LL515;}} else{ goto _LL516;} _LL516: if(( unsigned int)
_temp512 > 4u?*(( int*) _temp512) == Cyc_Absyn_Evar: 0){ _LL533: _temp532=( void*)((
struct Cyc_Absyn_Evar_struct*) _temp512)->f1; _temp534=&(( struct Cyc_Absyn_Evar_struct*)
_temp512)->f1; goto _LL530; _LL530: _temp529=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Evar_struct*) _temp512)->f2; _temp531=&(( struct Cyc_Absyn_Evar_struct*)
_temp512)->f2; goto _LL528; _LL528: _temp527=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp512)->f3; goto _LL517;} else{ goto _LL518;} _LL518: goto _LL519; _LL515: {
void* _temp535= Cyc_Tcutil_compress( _temp525); int _temp541; struct Cyc_Core_Opt*
_temp543; void* _temp545; _LL537: if(( unsigned int) _temp535 > 4u?*(( int*)
_temp535) == Cyc_Absyn_Evar: 0){ _LL546: _temp545=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp535)->f1; goto _LL544; _LL544: _temp543=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Evar_struct*) _temp535)->f2; goto _LL542; _LL542: _temp541=( int)((
struct Cyc_Absyn_Evar_struct*) _temp535)->f3; goto _LL538;} else{ goto _LL539;}
_LL539: goto _LL540; _LL538: return({ struct _tuple8* _temp547=( struct _tuple8*)
GC_malloc( sizeof( struct _tuple8)); _temp547->f1= _temp525; _temp547->f2=
_temp523; _temp547;}); _LL540: return 0; _LL536:;} _LL517: if(* _temp534 !=(
void*) Cyc_Absyn_EffKind){(( void(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp548=( char*)"effect evar has wrong kind";
struct _tagged_string _temp549; _temp549.curr= _temp548; _temp549.base= _temp548;
_temp549.last_plus_one= _temp548 + 27; _temp549;}));} return({ struct _tuple8*
_temp550=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8)); _temp550->f1= t;
_temp550->f2= 0; _temp550;}); _LL519: return 0; _LL513:;} int Cyc_Tcutil_region_in_effect(
int constrain, void* r, void* e){ r= Cyc_Tcutil_compress( r); if( r ==( void*)
Cyc_Absyn_HeapRgn){ return 1;}{ void* _temp551= Cyc_Tcutil_compress( e); void*
_temp561; struct Cyc_List_List* _temp563; int _temp565; struct Cyc_Core_Opt*
_temp567; struct Cyc_Core_Opt** _temp569; void* _temp570; void** _temp572;
_LL553: if(( unsigned int) _temp551 > 4u?*(( int*) _temp551) == Cyc_Absyn_AccessEff:
0){ _LL562: _temp561=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp551)->f1;
goto _LL554;} else{ goto _LL555;} _LL555: if(( unsigned int) _temp551 > 4u?*((
int*) _temp551) == Cyc_Absyn_JoinEff: 0){ _LL564: _temp563=( struct Cyc_List_List*)((
struct Cyc_Absyn_JoinEff_struct*) _temp551)->f1; goto _LL556;} else{ goto _LL557;}
_LL557: if(( unsigned int) _temp551 > 4u?*(( int*) _temp551) == Cyc_Absyn_Evar:
0){ _LL571: _temp570=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp551)->f1;
_temp572=&(( struct Cyc_Absyn_Evar_struct*) _temp551)->f1; goto _LL568; _LL568:
_temp567=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp551)->f2;
_temp569=&(( struct Cyc_Absyn_Evar_struct*) _temp551)->f2; goto _LL566; _LL566:
_temp565=( int)(( struct Cyc_Absyn_Evar_struct*) _temp551)->f3; goto _LL558;}
else{ goto _LL559;} _LL559: goto _LL560; _LL554: if( constrain){ return Cyc_Tcutil_unify(
r, _temp561);} Cyc_Tcutil_compress( _temp561); if( r == _temp561){ return 1;}{
struct _tuple7 _temp574=({ struct _tuple7 _temp573; _temp573.f1= r; _temp573.f2=
_temp561; _temp573;}); void* _temp580; struct Cyc_Absyn_Tvar* _temp582; void*
_temp584; struct Cyc_Absyn_Tvar* _temp586; _LL576: _LL585: _temp584= _temp574.f1;
if(( unsigned int) _temp584 > 4u?*(( int*) _temp584) == Cyc_Absyn_VarType: 0){
_LL587: _temp586=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp584)->f1; goto _LL581;} else{ goto _LL578;} _LL581: _temp580= _temp574.f2;
if(( unsigned int) _temp580 > 4u?*(( int*) _temp580) == Cyc_Absyn_VarType: 0){
_LL583: _temp582=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp580)->f1; goto _LL577;} else{ goto _LL578;} _LL578: goto _LL579; _LL577:
return Cyc_Absyn_tvar_cmp( _temp586, _temp582) == 0; _LL579: return 0; _LL575:;}
_LL556: for( 0; _temp563 != 0; _temp563=({ struct Cyc_List_List* _temp588=
_temp563; if( _temp588 == 0){ _throw( Null_Exception);} _temp588->tl;})){ if(
Cyc_Tcutil_region_in_effect( constrain, r,( void*)({ struct Cyc_List_List*
_temp589= _temp563; if( _temp589 == 0){ _throw( Null_Exception);} _temp589->hd;}))){
return 1;}} return 0; _LL558: if(* _temp572 !=( void*) Cyc_Absyn_EffKind){((
void(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp590=( char*)"effect evar has wrong kind"; struct _tagged_string
_temp591; _temp591.curr= _temp590; _temp591.base= _temp590; _temp591.last_plus_one=
_temp590 + 27; _temp591;}));} if( ! constrain){ return 0;}{ void* ev= Cyc_Absyn_new_evar((
void*) Cyc_Absyn_EffKind); struct Cyc_Absyn_JoinEff_struct* new_typ=({ struct
Cyc_Absyn_JoinEff_struct* _temp593=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp593[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp594; _temp594.tag= Cyc_Absyn_JoinEff; _temp594.f1=({ struct Cyc_List_List*
_temp595=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp595->hd=( void*) ev; _temp595->tl=({ struct Cyc_List_List* _temp596=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp596->hd=(
void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp597=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp597[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp598; _temp598.tag= Cyc_Absyn_AccessEff; _temp598.f1=(
void*) r; _temp598;}); _temp597;})); _temp596->tl= 0; _temp596;}); _temp595;});
_temp594;}); _temp593;});* _temp569=({ struct Cyc_Core_Opt* _temp592=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp592->v=( void*)((
void*) new_typ); _temp592;}); return 1;} _LL560: return 0; _LL552:;}} static int
Cyc_Tcutil_variable_in_effect( int constrain, struct Cyc_Absyn_Tvar* v, void* e){
e= Cyc_Tcutil_compress( e);{ void* _temp599= e; struct Cyc_Absyn_Tvar* _temp609;
struct Cyc_List_List* _temp611; int _temp613; struct Cyc_Core_Opt* _temp615;
struct Cyc_Core_Opt** _temp617; void* _temp618; void** _temp620; _LL601: if((
unsigned int) _temp599 > 4u?*(( int*) _temp599) == Cyc_Absyn_VarType: 0){ _LL610:
_temp609=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*) _temp599)->f1;
goto _LL602;} else{ goto _LL603;} _LL603: if(( unsigned int) _temp599 > 4u?*((
int*) _temp599) == Cyc_Absyn_JoinEff: 0){ _LL612: _temp611=( struct Cyc_List_List*)((
struct Cyc_Absyn_JoinEff_struct*) _temp599)->f1; goto _LL604;} else{ goto _LL605;}
_LL605: if(( unsigned int) _temp599 > 4u?*(( int*) _temp599) == Cyc_Absyn_Evar:
0){ _LL619: _temp618=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp599)->f1;
_temp620=&(( struct Cyc_Absyn_Evar_struct*) _temp599)->f1; goto _LL616; _LL616:
_temp615=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp599)->f2;
_temp617=&(( struct Cyc_Absyn_Evar_struct*) _temp599)->f2; goto _LL614; _LL614:
_temp613=( int)(( struct Cyc_Absyn_Evar_struct*) _temp599)->f3; goto _LL606;}
else{ goto _LL607;} _LL607: goto _LL608; _LL602: return Cyc_Absyn_tvar_cmp( v,
_temp609) == 0; _LL604: for( 0; _temp611 != 0; _temp611=({ struct Cyc_List_List*
_temp621= _temp611; if( _temp621 == 0){ _throw( Null_Exception);} _temp621->tl;})){
if( Cyc_Tcutil_variable_in_effect( constrain, v,( void*)({ struct Cyc_List_List*
_temp622= _temp611; if( _temp622 == 0){ _throw( Null_Exception);} _temp622->hd;}))){
return 1;}} return 0; _LL606: if(* _temp620 !=( void*) Cyc_Absyn_EffKind){((
void(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp623=( char*)"effect evar has wrong kind"; struct _tagged_string
_temp624; _temp624.curr= _temp623; _temp624.base= _temp623; _temp624.last_plus_one=
_temp623 + 27; _temp624;}));}{ void* ev= Cyc_Absyn_new_evar(( void*) Cyc_Absyn_EffKind);
struct Cyc_Absyn_JoinEff_struct* new_typ=({ struct Cyc_Absyn_JoinEff_struct*
_temp626=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp626[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp627; _temp627.tag= Cyc_Absyn_JoinEff;
_temp627.f1=({ struct Cyc_List_List* _temp628=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp628->hd=( void*) ev; _temp628->tl=({ struct
Cyc_List_List* _temp629=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp629->hd=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp630=(
struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp630[ 0]=({ struct Cyc_Absyn_VarType_struct _temp631; _temp631.tag= Cyc_Absyn_VarType;
_temp631.f1= v; _temp631;}); _temp630;})); _temp629->tl= 0; _temp629;});
_temp628;}); _temp627;}); _temp626;});* _temp617=({ struct Cyc_Core_Opt*
_temp625=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp625->v=( void*)(( void*) new_typ); _temp625;}); return 1;} _LL608: return 0;
_LL600:;}} static int Cyc_Tcutil_evar_in_effect( void* evar, void* e){ e= Cyc_Tcutil_compress(
e);{ void* _temp632= e; struct Cyc_List_List* _temp640; int _temp642; struct Cyc_Core_Opt*
_temp644; void* _temp646; _LL634: if(( unsigned int) _temp632 > 4u?*(( int*)
_temp632) == Cyc_Absyn_JoinEff: 0){ _LL641: _temp640=( struct Cyc_List_List*)((
struct Cyc_Absyn_JoinEff_struct*) _temp632)->f1; goto _LL635;} else{ goto _LL636;}
_LL636: if(( unsigned int) _temp632 > 4u?*(( int*) _temp632) == Cyc_Absyn_Evar:
0){ _LL647: _temp646=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp632)->f1;
goto _LL645; _LL645: _temp644=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp632)->f2; goto _LL643; _LL643: _temp642=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp632)->f3; goto _LL637;} else{ goto _LL638;} _LL638: goto _LL639; _LL635:
for( 0; _temp640 != 0; _temp640=({ struct Cyc_List_List* _temp648= _temp640; if(
_temp648 == 0){ _throw( Null_Exception);} _temp648->tl;})){ if( Cyc_Tcutil_evar_in_effect(
evar,( void*)({ struct Cyc_List_List* _temp649= _temp640; if( _temp649 == 0){
_throw( Null_Exception);} _temp649->hd;}))){ return 1;}} return 0; _LL637:
return evar == e; _LL639: return 0; _LL633:;}} int Cyc_Tcutil_subset_effect( int
set_to_empty, void* e1, void* e2){{ struct _tuple8* _temp650= Cyc_Tcutil_get_effect_evar(
e2); struct _tuple8 _temp656; struct Cyc_List_List* _temp657; void* _temp659;
_LL652: if( _temp650 == 0){ goto _LL654;} else{ _temp656=* _temp650; _LL660:
_temp659= _temp656.f1; goto _LL658; _LL658: _temp657= _temp656.f2; goto _LL653;}
_LL654: goto _LL655; _LL653: { void* ev= Cyc_Absyn_new_evar(( void*) Cyc_Absyn_EffKind);
struct Cyc_Absyn_JoinEff_struct* new_typ=({ struct Cyc_Absyn_JoinEff_struct*
_temp661=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp661[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp662; _temp662.tag= Cyc_Absyn_JoinEff;
_temp662.f1=({ struct Cyc_List_List* _temp663=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp663->hd=( void*) ev; _temp663->tl=({ struct
Cyc_List_List* _temp664=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp664->hd=( void*) e1; _temp664->tl= 0; _temp664;}); _temp663;}); _temp662;});
_temp661;}); return Cyc_Tcutil_unify( _temp659,( void*) new_typ);} _LL655: goto
_LL651; _LL651:;}{ void* _temp665= Cyc_Tcutil_compress( e1); struct Cyc_List_List*
_temp677; void* _temp679; struct Cyc_Absyn_Tvar* _temp681; int _temp683; struct
Cyc_Core_Opt* _temp685; struct Cyc_Core_Opt** _temp687; void* _temp688; _LL667:
if(( unsigned int) _temp665 > 4u?*(( int*) _temp665) == Cyc_Absyn_JoinEff: 0){
_LL678: _temp677=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*)
_temp665)->f1; goto _LL668;} else{ goto _LL669;} _LL669: if(( unsigned int)
_temp665 > 4u?*(( int*) _temp665) == Cyc_Absyn_AccessEff: 0){ _LL680: _temp679=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp665)->f1; goto _LL670;} else{
goto _LL671;} _LL671: if(( unsigned int) _temp665 > 4u?*(( int*) _temp665) ==
Cyc_Absyn_VarType: 0){ _LL682: _temp681=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp665)->f1; goto _LL672;} else{ goto _LL673;} _LL673: if(( unsigned int)
_temp665 > 4u?*(( int*) _temp665) == Cyc_Absyn_Evar: 0){ _LL689: _temp688=( void*)((
struct Cyc_Absyn_Evar_struct*) _temp665)->f1; goto _LL686; _LL686: _temp685=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp665)->f2; _temp687=&((
struct Cyc_Absyn_Evar_struct*) _temp665)->f2; goto _LL684; _LL684: _temp683=(
int)(( struct Cyc_Absyn_Evar_struct*) _temp665)->f3; goto _LL674;} else{ goto
_LL675;} _LL675: goto _LL676; _LL668: for( 0; _temp677 != 0; _temp677=({ struct
Cyc_List_List* _temp690= _temp677; if( _temp690 == 0){ _throw( Null_Exception);}
_temp690->tl;})){ if( ! Cyc_Tcutil_subset_effect( set_to_empty,( void*)({ struct
Cyc_List_List* _temp691= _temp677; if( _temp691 == 0){ _throw( Null_Exception);}
_temp691->hd;}), e2)){ return 0;}} return 1; _LL670: return( Cyc_Tcutil_region_in_effect(
0, _temp679, e2)? 1: Cyc_Tcutil_region_in_effect( 1, _temp679, e2))? 1: Cyc_Tcutil_unify(
_temp679,( void*) Cyc_Absyn_HeapRgn); _LL672: return Cyc_Tcutil_variable_in_effect(
0, _temp681, e2)? 1: Cyc_Tcutil_variable_in_effect( 1, _temp681, e2); _LL674:
if( ! Cyc_Tcutil_evar_in_effect( e1, e2)){ if( set_to_empty){* _temp687=({
struct Cyc_Core_Opt* _temp692=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp692->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp693=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp693[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp694; _temp694.tag= Cyc_Absyn_JoinEff;
_temp694.f1= 0; _temp694;}); _temp693;})); _temp692;});} else{* _temp687=({
struct Cyc_Core_Opt* _temp695=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp695->v=( void*) e2; _temp695;});}} return 1; _LL676: return((
int(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({ struct _tagged_string
_temp696= Cyc_Absynpp_typ2string( e1); xprintf("subset_effect: bad effect: %.*s",
_temp696.last_plus_one - _temp696.curr, _temp696.curr);})); _LL666:;}} struct
_tuple9{ struct _tuple8* f1; struct _tuple8* f2; } ; static int Cyc_Tcutil_unify_effect(
void* e1, void* e2){ e1= Cyc_Tcutil_compress( e1); e2= Cyc_Tcutil_compress( e2);
Cyc_Tcutil_normalize_effect( e1); Cyc_Tcutil_normalize_effect( e2);{ struct
_tuple9 _temp698=({ struct _tuple9 _temp697; _temp697.f1= Cyc_Tcutil_get_effect_evar(
e1); _temp697.f2= Cyc_Tcutil_get_effect_evar( e2); _temp697;}); struct _tuple8*
_temp704; struct _tuple8 _temp706; struct Cyc_List_List* _temp707; void*
_temp709; struct _tuple8* _temp711; struct _tuple8 _temp713; struct Cyc_List_List*
_temp714; void* _temp716; _LL700: _LL712: _temp711= _temp698.f1; if( _temp711 ==
0){ goto _LL702;} else{ _temp713=* _temp711; _LL717: _temp716= _temp713.f1; goto
_LL715; _LL715: _temp714= _temp713.f2; goto _LL705;} _LL705: _temp704= _temp698.f2;
if( _temp704 == 0){ goto _LL702;} else{ _temp706=* _temp704; _LL710: _temp709=
_temp706.f1; goto _LL708; _LL708: _temp707= _temp706.f2; goto _LL701;} _LL702:
goto _LL703; _LL701: { void* ev= Cyc_Absyn_new_evar(( void*) Cyc_Absyn_EffKind);
struct Cyc_Absyn_JoinEff_struct* new_typ=({ struct Cyc_Absyn_JoinEff_struct*
_temp718=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp718[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp719; _temp719.tag= Cyc_Absyn_JoinEff;
_temp719.f1=({ struct Cyc_List_List* _temp720=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp720->hd=( void*) ev; _temp720->tl=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp714, _temp707); _temp720;}); _temp719;}); _temp718;}); return Cyc_Tcutil_unify(
_temp716,( void*) new_typ)? Cyc_Tcutil_unify( _temp709,( void*) new_typ): 0;}
_LL703: return Cyc_Tcutil_subset_effect( 0, e1, e2)? Cyc_Tcutil_subset_effect( 0,
e2, e1): 0; _LL699:;}} struct _tuple10{ struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt*
f2; } ; void Cyc_Tcutil_unify_it( void* t1, void* t2){ t1= Cyc_Tcutil_compress(
t1); t2= Cyc_Tcutil_compress( t2); if( t1 == t2){ return;}{ void* _temp721= t1;
int _temp727; struct Cyc_Core_Opt* _temp729; struct Cyc_Core_Opt** _temp731;
void* _temp732; _LL723: if(( unsigned int) _temp721 > 4u?*(( int*) _temp721) ==
Cyc_Absyn_Evar: 0){ _LL733: _temp732=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp721)->f1; goto _LL730; _LL730: _temp729=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Evar_struct*) _temp721)->f2; _temp731=&(( struct Cyc_Absyn_Evar_struct*)
_temp721)->f2; goto _LL728; _LL728: _temp727=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp721)->f3; goto _LL724;} else{ goto _LL725;} _LL725: goto _LL726; _LL724:
Cyc_Tcutil_occurs( t1, t2);{ void* kind2= Cyc_Tcutil_typ_kind( t2); if( Cyc_Tcutil_kind_leq(
kind2, _temp732)){* _temp731=({ struct Cyc_Core_Opt* _temp734=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp734->v=( void*) t2; _temp734;});
return;} else{{ void* _temp735= t2; int _temp743; struct Cyc_Core_Opt* _temp745;
struct Cyc_Core_Opt** _temp747; void* _temp748; struct Cyc_Absyn_PtrInfo
_temp751; _LL737: if(( unsigned int) _temp735 > 4u?*(( int*) _temp735) == Cyc_Absyn_Evar:
0){ _LL749: _temp748=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp735)->f1;
goto _LL746; _LL746: _temp745=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp735)->f2; _temp747=&(( struct Cyc_Absyn_Evar_struct*) _temp735)->f2; goto
_LL744; _LL744: _temp743=( int)(( struct Cyc_Absyn_Evar_struct*) _temp735)->f3;
goto _LL738;} else{ goto _LL739;} _LL739: if(( unsigned int) _temp735 > 4u?*((
int*) _temp735) == Cyc_Absyn_PointerType: 0){ _LL752: _temp751=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp735)->f1; goto _LL750;} else{ goto
_LL741;} _LL750: if( _temp732 ==( void*) Cyc_Absyn_BoxKind){ goto _LL740;} else{
goto _LL741;} _LL741: goto _LL742; _LL738: if( Cyc_Tcutil_kind_leq( _temp732,
kind2)){* _temp747=({ struct Cyc_Core_Opt* _temp753=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp753->v=( void*) t1; _temp753;});
return;} goto _LL736; _LL740: { struct Cyc_Absyn_Conref* c=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp751.bounds);{ void*
_temp754=( void*) c->v; _LL756: if( _temp754 ==( void*) Cyc_Absyn_No_constr){
goto _LL757;} else{ goto _LL758;} _LL758: goto _LL759; _LL757:( void*)( c->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp760=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp760[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp761; _temp761.tag= Cyc_Absyn_Eq_constr; _temp761.f1=(
void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp762=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp762[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp763; _temp763.tag= Cyc_Absyn_Upper_b; _temp763.f1= Cyc_Absyn_signed_int_exp(
1, 0); _temp763;}); _temp762;})); _temp761;}); _temp760;})));* _temp731=({
struct Cyc_Core_Opt* _temp764=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp764->v=( void*) t2; _temp764;}); return; _LL759: goto
_LL755; _LL755:;} goto _LL736;} _LL742: goto _LL736; _LL736:;}( void) _throw((
void*) Cyc_Tcutil_Unify);}} _LL726: goto _LL722; _LL722:;}{ struct _tuple7
_temp766=({ struct _tuple7 _temp765; _temp765.f1= t2; _temp765.f2= t1; _temp765;});
void* _temp816; void* _temp818; int _temp820; struct Cyc_Core_Opt* _temp822;
void* _temp824; void* _temp826; void* _temp828; void* _temp830; struct Cyc_Absyn_Tvar*
_temp832; void* _temp834; struct Cyc_Absyn_Tvar* _temp836; void* _temp838;
struct Cyc_Absyn_Structdecl** _temp840; struct Cyc_List_List* _temp842; struct
_tuple1* _temp844; void* _temp846; struct Cyc_Absyn_Structdecl** _temp848;
struct Cyc_List_List* _temp850; struct _tuple1* _temp852; void* _temp854; struct
Cyc_Absyn_Enumdecl* _temp856; struct _tuple1* _temp858; void* _temp860; struct
Cyc_Absyn_Enumdecl* _temp862; struct _tuple1* _temp864; void* _temp866; struct
Cyc_Absyn_Uniondecl** _temp868; struct Cyc_List_List* _temp870; struct _tuple1*
_temp872; void* _temp874; struct Cyc_Absyn_Uniondecl** _temp876; struct Cyc_List_List*
_temp878; struct _tuple1* _temp880; void* _temp882; struct Cyc_Absyn_TunionInfo
_temp884; void* _temp886; struct Cyc_List_List* _temp888; void* _temp890; struct
Cyc_Absyn_Tuniondecl* _temp892; void* _temp894; struct Cyc_Absyn_TunionInfo
_temp896; void* _temp898; struct Cyc_List_List* _temp900; void* _temp902; struct
Cyc_Absyn_Tuniondecl* _temp904; void* _temp906; struct Cyc_Absyn_TunionFieldInfo
_temp908; struct Cyc_List_List* _temp910; void* _temp912; struct Cyc_Absyn_Tunionfield*
_temp914; struct Cyc_Absyn_Tuniondecl* _temp916; void* _temp918; struct Cyc_Absyn_TunionFieldInfo
_temp920; struct Cyc_List_List* _temp922; void* _temp924; struct Cyc_Absyn_Tunionfield*
_temp926; struct Cyc_Absyn_Tuniondecl* _temp928; void* _temp930; struct Cyc_Absyn_PtrInfo
_temp932; struct Cyc_Absyn_Conref* _temp934; struct Cyc_Absyn_Tqual _temp936;
struct Cyc_Absyn_Conref* _temp938; void* _temp940; void* _temp942; void*
_temp944; struct Cyc_Absyn_PtrInfo _temp946; struct Cyc_Absyn_Conref* _temp948;
struct Cyc_Absyn_Tqual _temp950; struct Cyc_Absyn_Conref* _temp952; void*
_temp954; void* _temp956; void* _temp958; void* _temp960; void* _temp962; void*
_temp964; void* _temp966; void* _temp968; void* _temp970; void* _temp972; void*
_temp974; void* _temp976; void* _temp978; struct Cyc_Absyn_Exp* _temp980; struct
Cyc_Absyn_Tqual _temp982; void* _temp984; void* _temp986; struct Cyc_Absyn_Exp*
_temp988; struct Cyc_Absyn_Tqual _temp990; void* _temp992; void* _temp994;
struct Cyc_Absyn_FnInfo _temp996; struct Cyc_List_List* _temp998; int _temp1000;
struct Cyc_List_List* _temp1002; void* _temp1004; struct Cyc_Core_Opt* _temp1006;
struct Cyc_List_List* _temp1008; void* _temp1010; struct Cyc_Absyn_FnInfo
_temp1012; struct Cyc_List_List* _temp1014; int _temp1016; struct Cyc_List_List*
_temp1018; void* _temp1020; struct Cyc_Core_Opt* _temp1022; struct Cyc_List_List*
_temp1024; void* _temp1026; struct Cyc_List_List* _temp1028; void* _temp1030;
struct Cyc_List_List* _temp1032; void* _temp1034; struct Cyc_List_List*
_temp1036; void* _temp1038; struct Cyc_List_List* _temp1040; void* _temp1042;
struct Cyc_List_List* _temp1044; void* _temp1046; struct Cyc_List_List*
_temp1048; void* _temp1050; void* _temp1052; void* _temp1054; void* _temp1056;
void* _temp1058; void* _temp1060; void* _temp1062; void* _temp1064; struct Cyc_List_List*
_temp1066; void* _temp1068; struct Cyc_List_List* _temp1070; void* _temp1072;
void* _temp1074; void* _temp1076; void* _temp1078; void* _temp1080; void*
_temp1082; void* _temp1084; _LL768: _LL819: _temp818= _temp766.f1; if((
unsigned int) _temp818 > 4u?*(( int*) _temp818) == Cyc_Absyn_Evar: 0){ _LL825:
_temp824=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp818)->f1; goto _LL823;
_LL823: _temp822=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp818)->f2; goto _LL821; _LL821: _temp820=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp818)->f3; goto _LL817;} else{ goto _LL770;} _LL817: _temp816= _temp766.f2;
goto _LL769; _LL770: _LL829: _temp828= _temp766.f1; if( _temp828 ==( void*) Cyc_Absyn_VoidType){
goto _LL827;} else{ goto _LL772;} _LL827: _temp826= _temp766.f2; if( _temp826 ==(
void*) Cyc_Absyn_VoidType){ goto _LL771;} else{ goto _LL772;} _LL772: _LL835:
_temp834= _temp766.f1; if(( unsigned int) _temp834 > 4u?*(( int*) _temp834) ==
Cyc_Absyn_VarType: 0){ _LL837: _temp836=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp834)->f1; goto _LL831;} else{ goto _LL774;} _LL831: _temp830= _temp766.f2;
if(( unsigned int) _temp830 > 4u?*(( int*) _temp830) == Cyc_Absyn_VarType: 0){
_LL833: _temp832=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp830)->f1; goto _LL773;} else{ goto _LL774;} _LL774: _LL847: _temp846=
_temp766.f1; if(( unsigned int) _temp846 > 4u?*(( int*) _temp846) == Cyc_Absyn_StructType:
0){ _LL853: _temp852=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp846)->f1; goto _LL851; _LL851: _temp850=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp846)->f2; goto _LL849; _LL849: _temp848=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp846)->f3;
goto _LL839;} else{ goto _LL776;} _LL839: _temp838= _temp766.f2; if((
unsigned int) _temp838 > 4u?*(( int*) _temp838) == Cyc_Absyn_StructType: 0){
_LL845: _temp844=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp838)->f1; goto _LL843; _LL843: _temp842=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp838)->f2; goto _LL841; _LL841: _temp840=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp838)->f3;
goto _LL775;} else{ goto _LL776;} _LL776: _LL861: _temp860= _temp766.f1; if((
unsigned int) _temp860 > 4u?*(( int*) _temp860) == Cyc_Absyn_EnumType: 0){
_LL865: _temp864=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*)
_temp860)->f1; goto _LL863; _LL863: _temp862=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_EnumType_struct*) _temp860)->f2; goto _LL855;} else{ goto
_LL778;} _LL855: _temp854= _temp766.f2; if(( unsigned int) _temp854 > 4u?*(( int*)
_temp854) == Cyc_Absyn_EnumType: 0){ _LL859: _temp858=( struct _tuple1*)((
struct Cyc_Absyn_EnumType_struct*) _temp854)->f1; goto _LL857; _LL857: _temp856=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*) _temp854)->f2;
goto _LL777;} else{ goto _LL778;} _LL778: _LL875: _temp874= _temp766.f1; if((
unsigned int) _temp874 > 4u?*(( int*) _temp874) == Cyc_Absyn_UnionType: 0){
_LL881: _temp880=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp874)->f1; goto _LL879; _LL879: _temp878=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp874)->f2; goto _LL877; _LL877: _temp876=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp874)->f3;
goto _LL867;} else{ goto _LL780;} _LL867: _temp866= _temp766.f2; if((
unsigned int) _temp866 > 4u?*(( int*) _temp866) == Cyc_Absyn_UnionType: 0){
_LL873: _temp872=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp866)->f1; goto _LL871; _LL871: _temp870=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp866)->f2; goto _LL869; _LL869: _temp868=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp866)->f3;
goto _LL779;} else{ goto _LL780;} _LL780: _LL895: _temp894= _temp766.f1; if((
unsigned int) _temp894 > 4u?*(( int*) _temp894) == Cyc_Absyn_TunionType: 0){
_LL897: _temp896=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp894)->f1; _LL903: _temp902=( void*) _temp896.tunion_info; if(*(( int*)
_temp902) == Cyc_Absyn_KnownTunion){ _LL905: _temp904=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunion_struct*) _temp902)->f1; goto _LL901;} else{ goto
_LL782;} _LL901: _temp900=( struct Cyc_List_List*) _temp896.targs; goto _LL899;
_LL899: _temp898=( void*) _temp896.rgn; goto _LL883;} else{ goto _LL782;} _LL883:
_temp882= _temp766.f2; if(( unsigned int) _temp882 > 4u?*(( int*) _temp882) ==
Cyc_Absyn_TunionType: 0){ _LL885: _temp884=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp882)->f1; _LL891: _temp890=( void*)
_temp884.tunion_info; if(*(( int*) _temp890) == Cyc_Absyn_KnownTunion){ _LL893:
_temp892=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_KnownTunion_struct*)
_temp890)->f1; goto _LL889;} else{ goto _LL782;} _LL889: _temp888=( struct Cyc_List_List*)
_temp884.targs; goto _LL887; _LL887: _temp886=( void*) _temp884.rgn; goto _LL781;}
else{ goto _LL782;} _LL782: _LL919: _temp918= _temp766.f1; if(( unsigned int)
_temp918 > 4u?*(( int*) _temp918) == Cyc_Absyn_TunionFieldType: 0){ _LL921:
_temp920=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp918)->f1; _LL925: _temp924=( void*) _temp920.field_info; if(*(( int*)
_temp924) == Cyc_Absyn_KnownTunionfield){ _LL929: _temp928=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp924)->f1; goto _LL927; _LL927:
_temp926=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp924)->f2; goto _LL923;} else{ goto _LL784;} _LL923: _temp922=( struct Cyc_List_List*)
_temp920.targs; goto _LL907;} else{ goto _LL784;} _LL907: _temp906= _temp766.f2;
if(( unsigned int) _temp906 > 4u?*(( int*) _temp906) == Cyc_Absyn_TunionFieldType:
0){ _LL909: _temp908=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp906)->f1; _LL913: _temp912=( void*) _temp908.field_info; if(*(( int*)
_temp912) == Cyc_Absyn_KnownTunionfield){ _LL917: _temp916=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp912)->f1; goto _LL915; _LL915:
_temp914=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp912)->f2; goto _LL911;} else{ goto _LL784;} _LL911: _temp910=( struct Cyc_List_List*)
_temp908.targs; goto _LL783;} else{ goto _LL784;} _LL784: _LL945: _temp944=
_temp766.f1; if(( unsigned int) _temp944 > 4u?*(( int*) _temp944) == Cyc_Absyn_PointerType:
0){ _LL947: _temp946=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp944)->f1; _LL957: _temp956=( void*) _temp946.elt_typ; goto _LL955; _LL955:
_temp954=( void*) _temp946.rgn_typ; goto _LL953; _LL953: _temp952=( struct Cyc_Absyn_Conref*)
_temp946.nullable; goto _LL951; _LL951: _temp950=( struct Cyc_Absyn_Tqual)
_temp946.tq; goto _LL949; _LL949: _temp948=( struct Cyc_Absyn_Conref*) _temp946.bounds;
goto _LL931;} else{ goto _LL786;} _LL931: _temp930= _temp766.f2; if((
unsigned int) _temp930 > 4u?*(( int*) _temp930) == Cyc_Absyn_PointerType: 0){
_LL933: _temp932=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp930)->f1; _LL943: _temp942=( void*) _temp932.elt_typ; goto _LL941; _LL941:
_temp940=( void*) _temp932.rgn_typ; goto _LL939; _LL939: _temp938=( struct Cyc_Absyn_Conref*)
_temp932.nullable; goto _LL937; _LL937: _temp936=( struct Cyc_Absyn_Tqual)
_temp932.tq; goto _LL935; _LL935: _temp934=( struct Cyc_Absyn_Conref*) _temp932.bounds;
goto _LL785;} else{ goto _LL786;} _LL786: _LL965: _temp964= _temp766.f1; if((
unsigned int) _temp964 > 4u?*(( int*) _temp964) == Cyc_Absyn_IntType: 0){ _LL969:
_temp968=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp964)->f1; goto _LL967;
_LL967: _temp966=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp964)->f2;
goto _LL959;} else{ goto _LL788;} _LL959: _temp958= _temp766.f2; if((
unsigned int) _temp958 > 4u?*(( int*) _temp958) == Cyc_Absyn_IntType: 0){ _LL963:
_temp962=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp958)->f1; goto _LL961;
_LL961: _temp960=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp958)->f2;
goto _LL787;} else{ goto _LL788;} _LL788: _LL973: _temp972= _temp766.f1; if(
_temp972 ==( void*) Cyc_Absyn_FloatType){ goto _LL971;} else{ goto _LL790;}
_LL971: _temp970= _temp766.f2; if( _temp970 ==( void*) Cyc_Absyn_FloatType){
goto _LL789;} else{ goto _LL790;} _LL790: _LL977: _temp976= _temp766.f1; if(
_temp976 ==( void*) Cyc_Absyn_DoubleType){ goto _LL975;} else{ goto _LL792;}
_LL975: _temp974= _temp766.f2; if( _temp974 ==( void*) Cyc_Absyn_DoubleType){
goto _LL791;} else{ goto _LL792;} _LL792: _LL987: _temp986= _temp766.f1; if((
unsigned int) _temp986 > 4u?*(( int*) _temp986) == Cyc_Absyn_ArrayType: 0){
_LL993: _temp992=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp986)->f1;
goto _LL991; _LL991: _temp990=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp986)->f2; goto _LL989; _LL989: _temp988=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp986)->f3; goto _LL979;} else{ goto _LL794;}
_LL979: _temp978= _temp766.f2; if(( unsigned int) _temp978 > 4u?*(( int*)
_temp978) == Cyc_Absyn_ArrayType: 0){ _LL985: _temp984=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp978)->f1; goto _LL983; _LL983: _temp982=( struct Cyc_Absyn_Tqual)(( struct
Cyc_Absyn_ArrayType_struct*) _temp978)->f2; goto _LL981; _LL981: _temp980=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*) _temp978)->f3; goto
_LL793;} else{ goto _LL794;} _LL794: _LL1011: _temp1010= _temp766.f1; if((
unsigned int) _temp1010 > 4u?*(( int*) _temp1010) == Cyc_Absyn_FnType: 0){
_LL1013: _temp1012=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp1010)->f1; _LL1025: _temp1024=( struct Cyc_List_List*) _temp1012.tvars;
goto _LL1023; _LL1023: _temp1022=( struct Cyc_Core_Opt*) _temp1012.effect; goto
_LL1021; _LL1021: _temp1020=( void*) _temp1012.ret_typ; goto _LL1019; _LL1019:
_temp1018=( struct Cyc_List_List*) _temp1012.args; goto _LL1017; _LL1017:
_temp1016=( int) _temp1012.varargs; goto _LL1015; _LL1015: _temp1014=( struct
Cyc_List_List*) _temp1012.attributes; goto _LL995;} else{ goto _LL796;} _LL995:
_temp994= _temp766.f2; if(( unsigned int) _temp994 > 4u?*(( int*) _temp994) ==
Cyc_Absyn_FnType: 0){ _LL997: _temp996=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp994)->f1; _LL1009: _temp1008=( struct Cyc_List_List*) _temp996.tvars; goto
_LL1007; _LL1007: _temp1006=( struct Cyc_Core_Opt*) _temp996.effect; goto
_LL1005; _LL1005: _temp1004=( void*) _temp996.ret_typ; goto _LL1003; _LL1003:
_temp1002=( struct Cyc_List_List*) _temp996.args; goto _LL1001; _LL1001:
_temp1000=( int) _temp996.varargs; goto _LL999; _LL999: _temp998=( struct Cyc_List_List*)
_temp996.attributes; goto _LL795;} else{ goto _LL796;} _LL796: _LL1031:
_temp1030= _temp766.f1; if(( unsigned int) _temp1030 > 4u?*(( int*) _temp1030)
== Cyc_Absyn_TupleType: 0){ _LL1033: _temp1032=( struct Cyc_List_List*)(( struct
Cyc_Absyn_TupleType_struct*) _temp1030)->f1; goto _LL1027;} else{ goto _LL798;}
_LL1027: _temp1026= _temp766.f2; if(( unsigned int) _temp1026 > 4u?*(( int*)
_temp1026) == Cyc_Absyn_TupleType: 0){ _LL1029: _temp1028=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp1026)->f1; goto _LL797;} else{ goto
_LL798;} _LL798: _LL1039: _temp1038= _temp766.f1; if(( unsigned int) _temp1038 >
4u?*(( int*) _temp1038) == Cyc_Absyn_AnonStructType: 0){ _LL1041: _temp1040=(
struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*) _temp1038)->f1;
goto _LL1035;} else{ goto _LL800;} _LL1035: _temp1034= _temp766.f2; if((
unsigned int) _temp1034 > 4u?*(( int*) _temp1034) == Cyc_Absyn_AnonStructType: 0){
_LL1037: _temp1036=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp1034)->f1; goto _LL799;} else{ goto _LL800;} _LL800: _LL1047: _temp1046=
_temp766.f1; if(( unsigned int) _temp1046 > 4u?*(( int*) _temp1046) == Cyc_Absyn_AnonUnionType:
0){ _LL1049: _temp1048=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp1046)->f1; goto _LL1043;} else{ goto _LL802;} _LL1043: _temp1042= _temp766.f2;
if(( unsigned int) _temp1042 > 4u?*(( int*) _temp1042) == Cyc_Absyn_AnonUnionType:
0){ _LL1045: _temp1044=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp1042)->f1; goto _LL801;} else{ goto _LL802;} _LL802: _LL1053: _temp1052=
_temp766.f1; if( _temp1052 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL1051;} else{
goto _LL804;} _LL1051: _temp1050= _temp766.f2; if( _temp1050 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL803;} else{ goto _LL804;} _LL804: _LL1059: _temp1058= _temp766.f1; if((
unsigned int) _temp1058 > 4u?*(( int*) _temp1058) == Cyc_Absyn_RgnHandleType: 0){
_LL1061: _temp1060=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp1058)->f1;
goto _LL1055;} else{ goto _LL806;} _LL1055: _temp1054= _temp766.f2; if((
unsigned int) _temp1054 > 4u?*(( int*) _temp1054) == Cyc_Absyn_RgnHandleType: 0){
_LL1057: _temp1056=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp1054)->f1;
goto _LL805;} else{ goto _LL806;} _LL806: _LL1065: _temp1064= _temp766.f1; if((
unsigned int) _temp1064 > 4u?*(( int*) _temp1064) == Cyc_Absyn_JoinEff: 0){
_LL1067: _temp1066=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*)
_temp1064)->f1; goto _LL1063;} else{ goto _LL808;} _LL1063: _temp1062= _temp766.f2;
goto _LL807; _LL808: _LL1073: _temp1072= _temp766.f1; goto _LL1069; _LL1069:
_temp1068= _temp766.f2; if(( unsigned int) _temp1068 > 4u?*(( int*) _temp1068)
== Cyc_Absyn_JoinEff: 0){ _LL1071: _temp1070=( struct Cyc_List_List*)(( struct
Cyc_Absyn_JoinEff_struct*) _temp1068)->f1; goto _LL809;} else{ goto _LL810;}
_LL810: _LL1077: _temp1076= _temp766.f1; if(( unsigned int) _temp1076 > 4u?*((
int*) _temp1076) == Cyc_Absyn_AccessEff: 0){ _LL1079: _temp1078=( void*)((
struct Cyc_Absyn_AccessEff_struct*) _temp1076)->f1; goto _LL1075;} else{ goto
_LL812;} _LL1075: _temp1074= _temp766.f2; goto _LL811; _LL812: _LL1085:
_temp1084= _temp766.f1; goto _LL1081; _LL1081: _temp1080= _temp766.f2; if((
unsigned int) _temp1080 > 4u?*(( int*) _temp1080) == Cyc_Absyn_AccessEff: 0){
_LL1083: _temp1082=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp1080)->f1;
goto _LL813;} else{ goto _LL814;} _LL814: goto _LL815; _LL769: Cyc_Tcutil_unify_it(
t2, t1); return; _LL771: return; _LL773: { struct _tagged_string* x2= _temp836->name;
struct _tagged_string* x1= _temp832->name; void* k2=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp836->kind); void* k1=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp832->kind); if( Cyc_String_zstrptrcmp( x2, x1)
== 0){ if( k2 != k1){(( void(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({
struct _tagged_string _temp1086=* x2; struct _tagged_string _temp1087= Cyc_Absynpp_kind2string(
k2); struct _tagged_string _temp1088= Cyc_Absynpp_kind2string( k1); xprintf("same type variable %.*s has kinds %.*s and %.*s",
_temp1086.last_plus_one - _temp1086.curr, _temp1086.curr, _temp1087.last_plus_one
- _temp1087.curr, _temp1087.curr, _temp1088.last_plus_one - _temp1088.curr,
_temp1088.curr);}));} return;} goto _LL767;} _LL775: if((( _temp844 != 0?
_temp852 != 0: 0)? Cyc_Absyn_qvar_cmp(( struct _tuple1*)({ struct _tuple1*
_temp1089= _temp844; if( _temp1089 == 0){ _throw( Null_Exception);} _temp1089;}),(
struct _tuple1*)({ struct _tuple1* _temp1090= _temp852; if( _temp1090 == 0){
_throw( Null_Exception);} _temp1090;})) == 0: 0)? 1:( _temp844 == 0? _temp852 ==
0: 0)){ Cyc_Tcutil_unify_list( _temp842, _temp850); return;} goto _LL767; _LL777:
if( Cyc_Absyn_qvar_cmp( _temp864, _temp858) == 0){ return;} goto _LL767; _LL779:
if((( _temp872 != 0? _temp880 != 0: 0)? Cyc_Absyn_qvar_cmp(( struct _tuple1*)({
struct _tuple1* _temp1091= _temp872; if( _temp1091 == 0){ _throw( Null_Exception);}
_temp1091;}),( struct _tuple1*)({ struct _tuple1* _temp1092= _temp880; if(
_temp1092 == 0){ _throw( Null_Exception);} _temp1092;})) == 0: 0)? 1:( _temp872
== 0? _temp880 == 0: 0)){ Cyc_Tcutil_unify_list( _temp870, _temp878); return;}
goto _LL767; _LL781: if( _temp904 == _temp892? 1: Cyc_Absyn_qvar_cmp( _temp904->name,
_temp892->name) == 0){ Cyc_Tcutil_unify_it( _temp886, _temp898); Cyc_Tcutil_unify_list(
_temp888, _temp900); return;} goto _LL767; _LL783: if(( _temp928 == _temp916? 1:
Cyc_Absyn_qvar_cmp( _temp928->name, _temp916->name) == 0)? _temp926 == _temp914?
1: Cyc_Absyn_qvar_cmp( _temp926->name, _temp914->name) == 0: 0){ Cyc_Tcutil_unify_list(
_temp910, _temp922); return;} goto _LL767; _LL785: Cyc_Tcutil_unify_it( _temp942,
_temp956); Cyc_Tcutil_unify_it( _temp954, _temp940); Cyc_Tcutil_unify_tqual(
_temp936, _temp950);(( void(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref*
x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_it_conrefs)( Cyc_Tcutil_boundscmp,
_temp934, _temp948);{ void* _temp1093=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp934))->v; void*
_temp1099; _LL1095: if(( unsigned int) _temp1093 > 1u?*(( int*) _temp1093) ==
Cyc_Absyn_Eq_constr: 0){ _LL1100: _temp1099=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp1093)->f1; if( _temp1099 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1096;}
else{ goto _LL1097;}} else{ goto _LL1097;} _LL1097: goto _LL1098; _LL1096:
return; _LL1098: goto _LL1094; _LL1094:;}(( void(*)( int(* cmp)( int, int),
struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_it_conrefs)(
Cyc_Core_intcmp, _temp938, _temp952); return; _LL787: if( _temp962 == _temp968?
_temp960 == _temp966: 0){ return;} goto _LL767; _LL789: return; _LL791: return;
_LL793: Cyc_Tcutil_unify_it( _temp984, _temp992); Cyc_Tcutil_unify_tqual(
_temp982, _temp990); if( _temp988 == _temp980){ return;} if( _temp988 == 0? 1:
_temp980 == 0){ goto _LL767;} if( Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1101= _temp988; if( _temp1101 == 0){ _throw(
Null_Exception);} _temp1101;})) == Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1102= _temp980; if( _temp1102 == 0){ _throw(
Null_Exception);} _temp1102;}))){ return;} goto _LL767; _LL795: { int done= 0;{
struct _RegionHandle _temp1103= _new_region(); struct _RegionHandle* rgn=&
_temp1103; _push_region( rgn);{ struct Cyc_List_List* inst= 0; while( _temp1008
!= 0) { if( _temp1024 == 0){ break;} inst=({ struct Cyc_List_List* _temp1104=(
struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp1104->hd=( void*)({ struct _tuple5* _temp1105=( struct _tuple5*)
_region_malloc( rgn, sizeof( struct _tuple5)); _temp1105->f1=( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp1109= _temp1024; if( _temp1109 == 0){ _throw(
Null_Exception);} _temp1109->hd;}); _temp1105->f2=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp1106=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp1106[ 0]=({ struct Cyc_Absyn_VarType_struct _temp1107; _temp1107.tag= Cyc_Absyn_VarType;
_temp1107.f1=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp1108=
_temp1008; if( _temp1108 == 0){ _throw( Null_Exception);} _temp1108->hd;});
_temp1107;}); _temp1106;}); _temp1105;}); _temp1104->tl= inst; _temp1104;});
_temp1008=({ struct Cyc_List_List* _temp1110= _temp1008; if( _temp1110 == 0){
_throw( Null_Exception);} _temp1110->tl;}); _temp1024=({ struct Cyc_List_List*
_temp1111= _temp1024; if( _temp1111 == 0){ _throw( Null_Exception);} _temp1111->tl;});}
if( _temp1024 != 0){ _npop_handler( 0u); goto _LL767;} if( inst != 0){ Cyc_Tcutil_unify_it((
void*)({ struct Cyc_Absyn_FnType_struct* _temp1112=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1112[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1113; _temp1113.tag= Cyc_Absyn_FnType; _temp1113.f1=({ struct Cyc_Absyn_FnInfo
_temp1114; _temp1114.tvars= 0; _temp1114.effect= _temp1006; _temp1114.ret_typ=(
void*) _temp1004; _temp1114.args= _temp1002; _temp1114.varargs= _temp1000;
_temp1114.attributes= _temp998; _temp1114;}); _temp1113;}); _temp1112;}), Cyc_Tcutil_rsubstitute(
rgn, inst,( void*)({ struct Cyc_Absyn_FnType_struct* _temp1115=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1115[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1116; _temp1116.tag= Cyc_Absyn_FnType; _temp1116.f1=({ struct Cyc_Absyn_FnInfo
_temp1117; _temp1117.tvars= 0; _temp1117.effect= _temp1022; _temp1117.ret_typ=(
void*) _temp1020; _temp1117.args= _temp1018; _temp1117.varargs= _temp1016;
_temp1117.attributes= _temp1014; _temp1117;}); _temp1116;}); _temp1115;})));
done= 1;}}; _pop_region(& _temp1103);} if( done){ return;} Cyc_Tcutil_unify_it(
_temp1004, _temp1020); for( 0; _temp1002 != 0? _temp1018 != 0: 0; _temp1002=({
struct Cyc_List_List* _temp1118= _temp1002; if( _temp1118 == 0){ _throw(
Null_Exception);} _temp1118->tl;}), _temp1018=({ struct Cyc_List_List* _temp1119=
_temp1018; if( _temp1119 == 0){ _throw( Null_Exception);} _temp1119->tl;})){ Cyc_Tcutil_unify_tqual((*((
struct _tuple2*)({ struct Cyc_List_List* _temp1120= _temp1002; if( _temp1120 ==
0){ _throw( Null_Exception);} _temp1120->hd;}))).f2,(*(( struct _tuple2*)({
struct Cyc_List_List* _temp1121= _temp1018; if( _temp1121 == 0){ _throw(
Null_Exception);} _temp1121->hd;}))).f2); Cyc_Tcutil_unify_it((*(( struct
_tuple2*)({ struct Cyc_List_List* _temp1122= _temp1002; if( _temp1122 == 0){
_throw( Null_Exception);} _temp1122->hd;}))).f3,(*(( struct _tuple2*)({ struct
Cyc_List_List* _temp1123= _temp1018; if( _temp1123 == 0){ _throw( Null_Exception);}
_temp1123->hd;}))).f3);} if( _temp1002 != 0? 1: _temp1018 != 0){ goto _LL767;}
if( _temp1000 != _temp1016){ goto _LL767;}{ int bad_effect= 0;{ struct _tuple10
_temp1125=({ struct _tuple10 _temp1124; _temp1124.f1= _temp1006; _temp1124.f2=
_temp1022; _temp1124;}); struct Cyc_Core_Opt* _temp1135; struct Cyc_Core_Opt*
_temp1137; struct Cyc_Core_Opt* _temp1139; struct Cyc_Core_Opt* _temp1141;
struct Cyc_Core_Opt* _temp1143; struct Cyc_Core_Opt* _temp1145; _LL1127: _LL1138:
_temp1137= _temp1125.f1; if( _temp1137 == 0){ goto _LL1136;} else{ goto _LL1129;}
_LL1136: _temp1135= _temp1125.f2; if( _temp1135 == 0){ goto _LL1128;} else{ goto
_LL1129;} _LL1129: _LL1142: _temp1141= _temp1125.f1; if( _temp1141 == 0){ goto
_LL1140;} else{ goto _LL1131;} _LL1140: _temp1139= _temp1125.f2; goto _LL1130;
_LL1131: _LL1146: _temp1145= _temp1125.f1; goto _LL1144; _LL1144: _temp1143=
_temp1125.f2; if( _temp1143 == 0){ goto _LL1132;} else{ goto _LL1133;} _LL1133:
goto _LL1134; _LL1128: goto _LL1126; _LL1130: goto _LL1132; _LL1132: bad_effect=
1; goto _LL1126; _LL1134: bad_effect= ! Cyc_Tcutil_unify_effect(( void*)({
struct Cyc_Core_Opt* _temp1147= _temp1006; if( _temp1147 == 0){ _throw(
Null_Exception);} _temp1147->v;}),( void*)({ struct Cyc_Core_Opt* _temp1148=
_temp1022; if( _temp1148 == 0){ _throw( Null_Exception);} _temp1148->v;})); goto
_LL1126; _LL1126:;} if( bad_effect){ goto _LL767;} if( ! Cyc_Tcutil_same_atts(
_temp1014, _temp998)){ goto _LL767;} return;}} _LL797: for( 0; _temp1028 != 0?
_temp1032 != 0: 0; _temp1028=({ struct Cyc_List_List* _temp1149= _temp1028; if(
_temp1149 == 0){ _throw( Null_Exception);} _temp1149->tl;}), _temp1032=({ struct
Cyc_List_List* _temp1150= _temp1032; if( _temp1150 == 0){ _throw( Null_Exception);}
_temp1150->tl;})){ Cyc_Tcutil_unify_tqual((*(( struct _tuple4*)({ struct Cyc_List_List*
_temp1151= _temp1028; if( _temp1151 == 0){ _throw( Null_Exception);} _temp1151->hd;}))).f1,(*((
struct _tuple4*)({ struct Cyc_List_List* _temp1152= _temp1032; if( _temp1152 ==
0){ _throw( Null_Exception);} _temp1152->hd;}))).f1); Cyc_Tcutil_unify_it((*((
struct _tuple4*)({ struct Cyc_List_List* _temp1153= _temp1028; if( _temp1153 ==
0){ _throw( Null_Exception);} _temp1153->hd;}))).f2,(*(( struct _tuple4*)({
struct Cyc_List_List* _temp1154= _temp1032; if( _temp1154 == 0){ _throw(
Null_Exception);} _temp1154->hd;}))).f2);} if( _temp1028 == 0? _temp1032 == 0: 0){
return;} goto _LL767; _LL799: _temp1048= _temp1040; _temp1044= _temp1036; goto
_LL801; _LL801: for( 0; _temp1044 != 0? _temp1048 != 0: 0; _temp1044=({ struct
Cyc_List_List* _temp1155= _temp1044; if( _temp1155 == 0){ _throw( Null_Exception);}
_temp1155->tl;}), _temp1048=({ struct Cyc_List_List* _temp1156= _temp1048; if(
_temp1156 == 0){ _throw( Null_Exception);} _temp1156->tl;})){ struct Cyc_Absyn_Structfield*
f1=( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp1160= _temp1044;
if( _temp1160 == 0){ _throw( Null_Exception);} _temp1160->hd;}); struct Cyc_Absyn_Structfield*
f2=( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp1159= _temp1048;
if( _temp1159 == 0){ _throw( Null_Exception);} _temp1159->hd;}); if( Cyc_String_zstrptrcmp(
f1->name, f2->name) != 0){( void) _throw(( void*) Cyc_Tcutil_Unify);} Cyc_Tcutil_unify_tqual(
f1->tq, f2->tq); Cyc_Tcutil_unify_it(( void*) f1->type,( void*) f2->type); if(((
! Cyc_Tcutil_same_atts( f1->attributes, f2->attributes)? 1:( f1->width != 0? f2->width
== 0: 0))? 1:( f2->width != 0? f1->width == 0: 0))? 1:(( f1->width != 0? f2->width
!= 0: 0)? Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt*
_temp1157= f1->width; if( _temp1157 == 0){ _throw( Null_Exception);} _temp1157->v;}))
!= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt*
_temp1158= f2->width; if( _temp1158 == 0){ _throw( Null_Exception);} _temp1158->v;})):
0)){( void) _throw(( void*) Cyc_Tcutil_Unify);}} if( _temp1044 == 0? _temp1048
== 0: 0){ return;} goto _LL767; _LL803: return; _LL805: Cyc_Tcutil_unify_it(
_temp1060, _temp1056); return; _LL807: goto _LL809; _LL809: goto _LL811; _LL811:
goto _LL813; _LL813: if( Cyc_Tcutil_unify_effect( t1, t2)){ return;} else{ goto
_LL767;} _LL815: goto _LL767; _LL767:;}( void) _throw(( void*) Cyc_Tcutil_Unify);}
int Cyc_Tcutil_is_arithmetic_type( void* t){ void* _temp1161= Cyc_Tcutil_compress(
t); void* _temp1173; void* _temp1175; struct Cyc_Absyn_Enumdecl* _temp1177;
struct _tuple1* _temp1179; _LL1163: if(( unsigned int) _temp1161 > 4u?*(( int*)
_temp1161) == Cyc_Absyn_IntType: 0){ _LL1176: _temp1175=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1161)->f1; goto _LL1174; _LL1174: _temp1173=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1161)->f2; goto _LL1164;} else{ goto _LL1165;} _LL1165: if( _temp1161 ==(
void*) Cyc_Absyn_FloatType){ goto _LL1166;} else{ goto _LL1167;} _LL1167: if(
_temp1161 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1168;} else{ goto _LL1169;}
_LL1169: if(( unsigned int) _temp1161 > 4u?*(( int*) _temp1161) == Cyc_Absyn_EnumType:
0){ _LL1180: _temp1179=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*)
_temp1161)->f1; goto _LL1178; _LL1178: _temp1177=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_EnumType_struct*) _temp1161)->f2; goto _LL1170;} else{ goto
_LL1171;} _LL1171: goto _LL1172; _LL1164: return 1; _LL1166: return 1; _LL1168:
return 1; _LL1170: return 1; _LL1172: return 0; _LL1162:;} int Cyc_Tcutil_will_lose_precision(
void* t1, void* t2){ t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{
struct _tuple7 _temp1182=({ struct _tuple7 _temp1181; _temp1181.f1= t1;
_temp1181.f2= t2; _temp1181;}); void* _temp1204; void* _temp1206; void*
_temp1208; void* _temp1210; void* _temp1212; void* _temp1214; void* _temp1216;
void* _temp1218; void* _temp1220; void* _temp1222; void* _temp1224; void*
_temp1226; void* _temp1228; void* _temp1230; void* _temp1232; void* _temp1234;
void* _temp1236; void* _temp1238; void* _temp1240; void* _temp1242; void*
_temp1244; void* _temp1246; void* _temp1248; void* _temp1250; void* _temp1252;
void* _temp1254; void* _temp1256; void* _temp1258; void* _temp1260; void*
_temp1262; void* _temp1264; void* _temp1266; void* _temp1268; void* _temp1270;
void* _temp1272; void* _temp1274; void* _temp1276; void* _temp1278; void*
_temp1280; void* _temp1282; void* _temp1284; void* _temp1286; _LL1184: _LL1207:
_temp1206= _temp1182.f1; if( _temp1206 ==( void*) Cyc_Absyn_DoubleType){ goto
_LL1205;} else{ goto _LL1186;} _LL1205: _temp1204= _temp1182.f2; if( _temp1204
==( void*) Cyc_Absyn_FloatType){ goto _LL1185;} else{ goto _LL1186;} _LL1186:
_LL1215: _temp1214= _temp1182.f1; if( _temp1214 ==( void*) Cyc_Absyn_DoubleType){
goto _LL1209;} else{ goto _LL1188;} _LL1209: _temp1208= _temp1182.f2; if((
unsigned int) _temp1208 > 4u?*(( int*) _temp1208) == Cyc_Absyn_IntType: 0){
_LL1213: _temp1212=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1208)->f1;
goto _LL1211; _LL1211: _temp1210=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1208)->f2; goto _LL1187;} else{ goto _LL1188;} _LL1188: _LL1223: _temp1222=
_temp1182.f1; if( _temp1222 ==( void*) Cyc_Absyn_FloatType){ goto _LL1217;}
else{ goto _LL1190;} _LL1217: _temp1216= _temp1182.f2; if(( unsigned int)
_temp1216 > 4u?*(( int*) _temp1216) == Cyc_Absyn_IntType: 0){ _LL1221: _temp1220=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1216)->f1; goto _LL1219; _LL1219:
_temp1218=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1216)->f2; goto
_LL1189;} else{ goto _LL1190;} _LL1190: _LL1231: _temp1230= _temp1182.f1; if((
unsigned int) _temp1230 > 4u?*(( int*) _temp1230) == Cyc_Absyn_IntType: 0){
_LL1235: _temp1234=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1230)->f1;
goto _LL1233; _LL1233: _temp1232=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1230)->f2; if( _temp1232 ==( void*) Cyc_Absyn_B8){ goto _LL1225;} else{
goto _LL1192;}} else{ goto _LL1192;} _LL1225: _temp1224= _temp1182.f2; if((
unsigned int) _temp1224 > 4u?*(( int*) _temp1224) == Cyc_Absyn_IntType: 0){
_LL1229: _temp1228=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1224)->f1;
goto _LL1227; _LL1227: _temp1226=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1224)->f2; if( _temp1226 ==( void*) Cyc_Absyn_B8){ goto _LL1191;} else{
goto _LL1192;}} else{ goto _LL1192;} _LL1192: _LL1239: _temp1238= _temp1182.f1;
if(( unsigned int) _temp1238 > 4u?*(( int*) _temp1238) == Cyc_Absyn_IntType: 0){
_LL1243: _temp1242=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1238)->f1;
goto _LL1241; _LL1241: _temp1240=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1238)->f2; if( _temp1240 ==( void*) Cyc_Absyn_B8){ goto _LL1237;} else{
goto _LL1194;}} else{ goto _LL1194;} _LL1237: _temp1236= _temp1182.f2; goto
_LL1193; _LL1194: _LL1247: _temp1246= _temp1182.f1; if(( unsigned int) _temp1246
> 4u?*(( int*) _temp1246) == Cyc_Absyn_IntType: 0){ _LL1251: _temp1250=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp1246)->f1; goto _LL1249; _LL1249:
_temp1248=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1246)->f2; if(
_temp1248 ==( void*) Cyc_Absyn_B4){ goto _LL1245;} else{ goto _LL1196;}} else{
goto _LL1196;} _LL1245: _temp1244= _temp1182.f2; if( _temp1244 ==( void*) Cyc_Absyn_FloatType){
goto _LL1195;} else{ goto _LL1196;} _LL1196: _LL1259: _temp1258= _temp1182.f1;
if(( unsigned int) _temp1258 > 4u?*(( int*) _temp1258) == Cyc_Absyn_IntType: 0){
_LL1263: _temp1262=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1258)->f1;
goto _LL1261; _LL1261: _temp1260=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1258)->f2; if( _temp1260 ==( void*) Cyc_Absyn_B4){ goto _LL1253;} else{
goto _LL1198;}} else{ goto _LL1198;} _LL1253: _temp1252= _temp1182.f2; if((
unsigned int) _temp1252 > 4u?*(( int*) _temp1252) == Cyc_Absyn_IntType: 0){
_LL1257: _temp1256=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1252)->f1;
goto _LL1255; _LL1255: _temp1254=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1252)->f2; if( _temp1254 ==( void*) Cyc_Absyn_B2){ goto _LL1197;} else{
goto _LL1198;}} else{ goto _LL1198;} _LL1198: _LL1271: _temp1270= _temp1182.f1;
if(( unsigned int) _temp1270 > 4u?*(( int*) _temp1270) == Cyc_Absyn_IntType: 0){
_LL1275: _temp1274=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1270)->f1;
goto _LL1273; _LL1273: _temp1272=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1270)->f2; if( _temp1272 ==( void*) Cyc_Absyn_B4){ goto _LL1265;} else{
goto _LL1200;}} else{ goto _LL1200;} _LL1265: _temp1264= _temp1182.f2; if((
unsigned int) _temp1264 > 4u?*(( int*) _temp1264) == Cyc_Absyn_IntType: 0){
_LL1269: _temp1268=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1264)->f1;
goto _LL1267; _LL1267: _temp1266=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1264)->f2; if( _temp1266 ==( void*) Cyc_Absyn_B1){ goto _LL1199;} else{
goto _LL1200;}} else{ goto _LL1200;} _LL1200: _LL1283: _temp1282= _temp1182.f1;
if(( unsigned int) _temp1282 > 4u?*(( int*) _temp1282) == Cyc_Absyn_IntType: 0){
_LL1287: _temp1286=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1282)->f1;
goto _LL1285; _LL1285: _temp1284=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1282)->f2; if( _temp1284 ==( void*) Cyc_Absyn_B2){ goto _LL1277;} else{
goto _LL1202;}} else{ goto _LL1202;} _LL1277: _temp1276= _temp1182.f2; if((
unsigned int) _temp1276 > 4u?*(( int*) _temp1276) == Cyc_Absyn_IntType: 0){
_LL1281: _temp1280=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1276)->f1;
goto _LL1279; _LL1279: _temp1278=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1276)->f2; if( _temp1278 ==( void*) Cyc_Absyn_B1){ goto _LL1201;} else{
goto _LL1202;}} else{ goto _LL1202;} _LL1202: goto _LL1203; _LL1185: return 1;
_LL1187: return 1; _LL1189: return 1; _LL1191: return 0; _LL1193: return 1;
_LL1195: return 1; _LL1197: return 1; _LL1199: return 1; _LL1201: return 1;
_LL1203: return 0; _LL1183:;}} int Cyc_Tcutil_coerce_list( struct Cyc_Tcenv_Tenv*
te, void* t, struct Cyc_List_List* es){ struct Cyc_Core_Opt* max_arith_type= 0;{
struct Cyc_List_List* el= es; for( 0; el != 0; el=({ struct Cyc_List_List*
_temp1288= el; if( _temp1288 == 0){ _throw( Null_Exception);} _temp1288->tl;})){
void* t1= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp1292=((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1291= el; if( _temp1291 == 0){
_throw( Null_Exception);} _temp1291->hd;}))->topt; if( _temp1292 == 0){ _throw(
Null_Exception);} _temp1292->v;})); if( Cyc_Tcutil_is_arithmetic_type( t1)){ if(
max_arith_type == 0? 1: Cyc_Tcutil_will_lose_precision( t1,( void*)({ struct Cyc_Core_Opt*
_temp1289= max_arith_type; if( _temp1289 == 0){ _throw( Null_Exception);}
_temp1289->v;}))){ max_arith_type=({ struct Cyc_Core_Opt* _temp1290=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1290->v=( void*) t1; _temp1290;});}}}}
if( max_arith_type != 0){ if( ! Cyc_Tcutil_unify( t,( void*)({ struct Cyc_Core_Opt*
_temp1293= max_arith_type; if( _temp1293 == 0){ _throw( Null_Exception);}
_temp1293->v;}))){ return 0;}}{ struct Cyc_List_List* el= es; for( 0; el != 0;
el=({ struct Cyc_List_List* _temp1294= el; if( _temp1294 == 0){ _throw(
Null_Exception);} _temp1294->tl;})){ if( ! Cyc_Tcutil_coerce_assign( te,( struct
Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1295= el; if( _temp1295 == 0){
_throw( Null_Exception);} _temp1295->hd;}), t)){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1296= el; if( _temp1296 == 0){ _throw( Null_Exception);}
_temp1296->hd;}))->loc,({ struct _tagged_string _temp1299= Cyc_Absynpp_typ2string(
t); struct _tagged_string _temp1300= Cyc_Absynpp_typ2string(( void*)({ struct
Cyc_Core_Opt* _temp1298=(( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1297= el; if( _temp1297 == 0){ _throw( Null_Exception);} _temp1297->hd;}))->topt;
if( _temp1298 == 0){ _throw( Null_Exception);} _temp1298->v;})); xprintf("type mismatch: expecting %.*s but found %.*s",
_temp1299.last_plus_one - _temp1299.curr, _temp1299.curr, _temp1300.last_plus_one
- _temp1300.curr, _temp1300.curr);})); return 0;}}} return 1;} int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( ! Cyc_Tcutil_coerce_sint_typ(
te, e)){ void* _temp1302= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp1301= e->topt; if( _temp1301 == 0){ _throw( Null_Exception);} _temp1301->v;}));
struct Cyc_Absyn_PtrInfo _temp1308; _LL1304: if(( unsigned int) _temp1302 > 4u?*((
int*) _temp1302) == Cyc_Absyn_PointerType: 0){ _LL1309: _temp1308=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1302)->f1; goto _LL1305;} else{ goto
_LL1306;} _LL1306: goto _LL1307; _LL1305: Cyc_Tcutil_unchecked_cast( te, e, Cyc_Absyn_uint_t);
goto _LL1303; _LL1307: return 0; _LL1303:;} return 1;} int Cyc_Tcutil_is_integral_type(
void* t){ void* _temp1310= Cyc_Tcutil_compress( t); void* _temp1318; void*
_temp1320; struct Cyc_Absyn_Enumdecl* _temp1322; struct _tuple1* _temp1324;
_LL1312: if(( unsigned int) _temp1310 > 4u?*(( int*) _temp1310) == Cyc_Absyn_IntType:
0){ _LL1321: _temp1320=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1310)->f1;
goto _LL1319; _LL1319: _temp1318=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1310)->f2; goto _LL1313;} else{ goto _LL1314;} _LL1314: if(( unsigned int)
_temp1310 > 4u?*(( int*) _temp1310) == Cyc_Absyn_EnumType: 0){ _LL1325:
_temp1324=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*) _temp1310)->f1;
goto _LL1323; _LL1323: _temp1322=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*)
_temp1310)->f2; goto _LL1315;} else{ goto _LL1316;} _LL1316: goto _LL1317;
_LL1313: return 1; _LL1315: return 1; _LL1317: return 0; _LL1311:;} int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)({
struct Cyc_Core_Opt* _temp1326= e->topt; if( _temp1326 == 0){ _throw(
Null_Exception);} _temp1326->v;}), Cyc_Absyn_uint_t)){ return 1;} if( Cyc_Tcutil_is_integral_type((
void*)({ struct Cyc_Core_Opt* _temp1327= e->topt; if( _temp1327 == 0){ _throw(
Null_Exception);} _temp1327->v;}))){ if( Cyc_Tcutil_will_lose_precision(( void*)({
struct Cyc_Core_Opt* _temp1328= e->topt; if( _temp1328 == 0){ _throw(
Null_Exception);} _temp1328->v;}), Cyc_Absyn_uint_t)){ Cyc_Tcutil_warn( e->loc,(
struct _tagged_string)({ char* _temp1329=( char*)"integral size mismatch; conversion supplied";
struct _tagged_string _temp1330; _temp1330.curr= _temp1329; _temp1330.base=
_temp1329; _temp1330.last_plus_one= _temp1329 + 44; _temp1330;}));} Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_uint_t); return 1;} return 0;} int Cyc_Tcutil_coerce_sint_typ(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)({
struct Cyc_Core_Opt* _temp1331= e->topt; if( _temp1331 == 0){ _throw(
Null_Exception);} _temp1331->v;}), Cyc_Absyn_sint_t)){ return 1;} if( Cyc_Tcutil_is_integral_type((
void*)({ struct Cyc_Core_Opt* _temp1332= e->topt; if( _temp1332 == 0){ _throw(
Null_Exception);} _temp1332->v;}))){ if( Cyc_Tcutil_will_lose_precision(( void*)({
struct Cyc_Core_Opt* _temp1333= e->topt; if( _temp1333 == 0){ _throw(
Null_Exception);} _temp1333->v;}), Cyc_Absyn_sint_t)){ Cyc_Tcutil_warn( e->loc,(
struct _tagged_string)({ char* _temp1334=( char*)"integral size mismatch; conversion supplied";
struct _tagged_string _temp1335; _temp1335.curr= _temp1334; _temp1335.base=
_temp1334; _temp1335.last_plus_one= _temp1334 + 44; _temp1335;}));} Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_sint_t); return 1;} return 0;} int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* t1, void* t2){
t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple7
_temp1337=({ struct _tuple7 _temp1336; _temp1336.f1= t1; _temp1336.f2= t2;
_temp1336;}); void* _temp1349; struct Cyc_Absyn_PtrInfo _temp1351; void*
_temp1353; struct Cyc_Absyn_PtrInfo _temp1355; void* _temp1357; struct Cyc_Absyn_Exp*
_temp1359; struct Cyc_Absyn_Tqual _temp1361; void* _temp1363; void* _temp1365;
struct Cyc_Absyn_Exp* _temp1367; struct Cyc_Absyn_Tqual _temp1369; void*
_temp1371; void* _temp1373; struct Cyc_Absyn_TunionInfo _temp1375; void*
_temp1377; struct Cyc_List_List* _temp1379; void* _temp1381; struct Cyc_Absyn_Tuniondecl*
_temp1383; void* _temp1385; struct Cyc_Absyn_TunionFieldInfo _temp1387; struct
Cyc_List_List* _temp1389; void* _temp1391; struct Cyc_Absyn_Tunionfield*
_temp1393; struct Cyc_Absyn_Tuniondecl* _temp1395; void* _temp1397; struct Cyc_Absyn_TunionInfo
_temp1399; void* _temp1401; struct Cyc_List_List* _temp1403; void* _temp1405;
struct Cyc_Absyn_Tuniondecl* _temp1407; void* _temp1409; struct Cyc_Absyn_PtrInfo
_temp1411; struct Cyc_Absyn_Conref* _temp1413; struct Cyc_Absyn_Tqual _temp1415;
struct Cyc_Absyn_Conref* _temp1417; void* _temp1419; void* _temp1421; _LL1339:
_LL1354: _temp1353= _temp1337.f1; if(( unsigned int) _temp1353 > 4u?*(( int*)
_temp1353) == Cyc_Absyn_PointerType: 0){ _LL1356: _temp1355=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1353)->f1; goto _LL1350;} else{ goto
_LL1341;} _LL1350: _temp1349= _temp1337.f2; if(( unsigned int) _temp1349 > 4u?*((
int*) _temp1349) == Cyc_Absyn_PointerType: 0){ _LL1352: _temp1351=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1349)->f1; goto _LL1340;} else{ goto
_LL1341;} _LL1341: _LL1366: _temp1365= _temp1337.f1; if(( unsigned int)
_temp1365 > 4u?*(( int*) _temp1365) == Cyc_Absyn_ArrayType: 0){ _LL1372:
_temp1371=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1365)->f1; goto
_LL1370; _LL1370: _temp1369=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1365)->f2; goto _LL1368; _LL1368: _temp1367=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1365)->f3; goto _LL1358;} else{ goto
_LL1343;} _LL1358: _temp1357= _temp1337.f2; if(( unsigned int) _temp1357 > 4u?*((
int*) _temp1357) == Cyc_Absyn_ArrayType: 0){ _LL1364: _temp1363=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1357)->f1; goto _LL1362; _LL1362:
_temp1361=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1357)->f2; goto _LL1360; _LL1360: _temp1359=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1357)->f3; goto _LL1342;} else{ goto
_LL1343;} _LL1343: _LL1386: _temp1385= _temp1337.f1; if(( unsigned int)
_temp1385 > 4u?*(( int*) _temp1385) == Cyc_Absyn_TunionFieldType: 0){ _LL1388:
_temp1387=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp1385)->f1; _LL1392: _temp1391=( void*) _temp1387.field_info; if(*(( int*)
_temp1391) == Cyc_Absyn_KnownTunionfield){ _LL1396: _temp1395=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp1391)->f1; goto _LL1394; _LL1394:
_temp1393=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1391)->f2; goto _LL1390;} else{ goto _LL1345;} _LL1390: _temp1389=( struct
Cyc_List_List*) _temp1387.targs; goto _LL1374;} else{ goto _LL1345;} _LL1374:
_temp1373= _temp1337.f2; if(( unsigned int) _temp1373 > 4u?*(( int*) _temp1373)
== Cyc_Absyn_TunionType: 0){ _LL1376: _temp1375=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp1373)->f1; _LL1382: _temp1381=( void*)
_temp1375.tunion_info; if(*(( int*) _temp1381) == Cyc_Absyn_KnownTunion){
_LL1384: _temp1383=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1381)->f1; goto _LL1380;} else{ goto _LL1345;} _LL1380: _temp1379=( struct
Cyc_List_List*) _temp1375.targs; goto _LL1378; _LL1378: _temp1377=( void*)
_temp1375.rgn; goto _LL1344;} else{ goto _LL1345;} _LL1345: _LL1410: _temp1409=
_temp1337.f1; if(( unsigned int) _temp1409 > 4u?*(( int*) _temp1409) == Cyc_Absyn_PointerType:
0){ _LL1412: _temp1411=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1409)->f1; _LL1422: _temp1421=( void*) _temp1411.elt_typ; goto _LL1420;
_LL1420: _temp1419=( void*) _temp1411.rgn_typ; goto _LL1418; _LL1418: _temp1417=(
struct Cyc_Absyn_Conref*) _temp1411.nullable; goto _LL1416; _LL1416: _temp1415=(
struct Cyc_Absyn_Tqual) _temp1411.tq; goto _LL1414; _LL1414: _temp1413=( struct
Cyc_Absyn_Conref*) _temp1411.bounds; goto _LL1398;} else{ goto _LL1347;} _LL1398:
_temp1397= _temp1337.f2; if(( unsigned int) _temp1397 > 4u?*(( int*) _temp1397)
== Cyc_Absyn_TunionType: 0){ _LL1400: _temp1399=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp1397)->f1; _LL1406: _temp1405=( void*)
_temp1399.tunion_info; if(*(( int*) _temp1405) == Cyc_Absyn_KnownTunion){
_LL1408: _temp1407=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1405)->f1; goto _LL1404;} else{ goto _LL1347;} _LL1404: _temp1403=( struct
Cyc_List_List*) _temp1399.targs; goto _LL1402; _LL1402: _temp1401=( void*)
_temp1399.rgn; goto _LL1346;} else{ goto _LL1347;} _LL1347: goto _LL1348;
_LL1340: { int okay= 1; if( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref*
x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp,
_temp1355.nullable, _temp1351.nullable)){ void* _temp1423=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp1355.nullable))->v;
int _temp1429; _LL1425: if(( unsigned int) _temp1423 > 1u?*(( int*) _temp1423)
== Cyc_Absyn_Eq_constr: 0){ _LL1430: _temp1429=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp1423)->f1; goto _LL1426;} else{ goto _LL1427;} _LL1427: goto _LL1428;
_LL1426: okay= ! _temp1429; goto _LL1424; _LL1428:( void) _throw((( void*(*)(
struct _tagged_string msg)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp1431=( char*)"silent_castable conref not eq"; struct _tagged_string
_temp1432; _temp1432.curr= _temp1431; _temp1432.base= _temp1431; _temp1432.last_plus_one=
_temp1431 + 30; _temp1432;}))); _LL1424:;} if( !(( int(*)( int(* cmp)( void*,
void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)(
Cyc_Tcutil_boundscmp, _temp1355.bounds, _temp1351.bounds)){ struct _tuple7
_temp1434=({ struct _tuple7 _temp1433; _temp1433.f1=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp1355.bounds))->v;
_temp1433.f2=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)( _temp1351.bounds))->v; _temp1433;}); void* _temp1442;
void* _temp1444; void* _temp1446; void* _temp1448; struct Cyc_Absyn_Exp*
_temp1450; void* _temp1452; void* _temp1454; void* _temp1456; void* _temp1458;
_LL1436: _LL1447: _temp1446= _temp1434.f1; if(( unsigned int) _temp1446 > 1u?*((
int*) _temp1446) == Cyc_Absyn_Eq_constr: 0){ _LL1449: _temp1448=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp1446)->f1; if(( unsigned int) _temp1448
> 1u?*(( int*) _temp1448) == Cyc_Absyn_Upper_b: 0){ _LL1451: _temp1450=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp1448)->f1; goto _LL1443;}
else{ goto _LL1438;}} else{ goto _LL1438;} _LL1443: _temp1442= _temp1434.f2; if((
unsigned int) _temp1442 > 1u?*(( int*) _temp1442) == Cyc_Absyn_Eq_constr: 0){
_LL1445: _temp1444=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1442)->f1;
if( _temp1444 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1437;} else{ goto _LL1438;}}
else{ goto _LL1438;} _LL1438: _LL1457: _temp1456= _temp1434.f1; if((
unsigned int) _temp1456 > 1u?*(( int*) _temp1456) == Cyc_Absyn_Eq_constr: 0){
_LL1459: _temp1458=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1456)->f1;
if( _temp1458 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1453;} else{ goto _LL1440;}}
else{ goto _LL1440;} _LL1453: _temp1452= _temp1434.f2; if(( unsigned int)
_temp1452 > 1u?*(( int*) _temp1452) == Cyc_Absyn_Eq_constr: 0){ _LL1455:
_temp1454=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1452)->f1; if(
_temp1454 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1439;} else{ goto _LL1440;}}
else{ goto _LL1440;} _LL1440: goto _LL1441; _LL1437: okay= 1; goto _LL1435;
_LL1439: okay= 1; goto _LL1435; _LL1441: okay= 0; goto _LL1435; _LL1435:;}
return(( okay? Cyc_Tcutil_unify(( void*) _temp1355.elt_typ,( void*) _temp1351.elt_typ):
0)? Cyc_Tcutil_unify(( void*) _temp1355.rgn_typ,( void*) _temp1351.rgn_typ): 0)?
!( _temp1355.tq).q_const? 1:( _temp1351.tq).q_const: 0;} _LL1342: { int okay;
okay=( _temp1367 != 0? _temp1359 != 0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1460= _temp1367; if(
_temp1460 == 0){ _throw( Null_Exception);} _temp1460;})) == Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1461= _temp1359; if(
_temp1461 == 0){ _throw( Null_Exception);} _temp1461;})): 0; return( okay? Cyc_Tcutil_unify(
_temp1371, _temp1363): 0)? ! _temp1369.q_const? 1: _temp1361.q_const: 0;}
_LL1344: if(( _temp1395 == _temp1383? 1: Cyc_Absyn_qvar_cmp( _temp1395->name,
_temp1383->name) == 0)? _temp1393->typs == 0: 0){ for( 0; _temp1389 != 0?
_temp1379 != 0: 0; _temp1389=({ struct Cyc_List_List* _temp1462= _temp1389; if(
_temp1462 == 0){ _throw( Null_Exception);} _temp1462->tl;}), _temp1379=({ struct
Cyc_List_List* _temp1463= _temp1379; if( _temp1463 == 0){ _throw( Null_Exception);}
_temp1463->tl;})){ if( ! Cyc_Tcutil_unify(( void*)({ struct Cyc_List_List*
_temp1464= _temp1389; if( _temp1464 == 0){ _throw( Null_Exception);} _temp1464->hd;}),(
void*)({ struct Cyc_List_List* _temp1465= _temp1379; if( _temp1465 == 0){ _throw(
Null_Exception);} _temp1465->hd;}))){ break;}} if( _temp1389 == 0? _temp1379 ==
0: 0){ return 1;}} return 0; _LL1346:{ void* _temp1466= Cyc_Tcutil_compress(
_temp1421); struct Cyc_Absyn_TunionFieldInfo _temp1472; struct Cyc_List_List*
_temp1474; void* _temp1476; struct Cyc_Absyn_Tunionfield* _temp1478; struct Cyc_Absyn_Tuniondecl*
_temp1480; _LL1468: if(( unsigned int) _temp1466 > 4u?*(( int*) _temp1466) ==
Cyc_Absyn_TunionFieldType: 0){ _LL1473: _temp1472=( struct Cyc_Absyn_TunionFieldInfo)((
struct Cyc_Absyn_TunionFieldType_struct*) _temp1466)->f1; _LL1477: _temp1476=(
void*) _temp1472.field_info; if(*(( int*) _temp1476) == Cyc_Absyn_KnownTunionfield){
_LL1481: _temp1480=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1476)->f1; goto _LL1479; _LL1479: _temp1478=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp1476)->f2; goto _LL1475;} else{
goto _LL1470;} _LL1475: _temp1474=( struct Cyc_List_List*) _temp1472.targs; goto
_LL1469;} else{ goto _LL1470;} _LL1470: goto _LL1471; _LL1469: if( ! Cyc_Tcutil_unify(
_temp1419, _temp1401)? _temp1419 !=( void*) Cyc_Absyn_HeapRgn: 0){ return 0;}
if( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp, _temp1417,(( struct Cyc_Absyn_Conref*(*)(
int x)) Cyc_Absyn_new_conref)( 0))){ return 0;} if( !(( int(*)( int(* cmp)( void*,
void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)(
Cyc_Tcutil_boundscmp, _temp1413,(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
Cyc_Absyn_bounds_one))){ return 0;} if( Cyc_Absyn_qvar_cmp( _temp1407->name,
_temp1480->name) == 0? _temp1478->typs != 0: 0){ int okay= 1; for( 0; _temp1474
!= 0? _temp1403 != 0: 0; _temp1474=({ struct Cyc_List_List* _temp1482= _temp1474;
if( _temp1482 == 0){ _throw( Null_Exception);} _temp1482->tl;}), _temp1403=({
struct Cyc_List_List* _temp1483= _temp1403; if( _temp1483 == 0){ _throw(
Null_Exception);} _temp1483->tl;})){ if( ! Cyc_Tcutil_unify(( void*)({ struct
Cyc_List_List* _temp1484= _temp1474; if( _temp1484 == 0){ _throw( Null_Exception);}
_temp1484->hd;}),( void*)({ struct Cyc_List_List* _temp1485= _temp1403; if(
_temp1485 == 0){ _throw( Null_Exception);} _temp1485->hd;}))){ okay= 0; break;}}
if(( ! okay? 1: _temp1474 != 0)? 1: _temp1403 != 0){ return 0;} return 1;} goto
_LL1467; _LL1471: goto _LL1467; _LL1467:;} return 0; _LL1348: return Cyc_Tcutil_unify(
t1, t2); _LL1338:;}} int Cyc_Tcutil_coerce_arg( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e, void* t2){ void* t1= Cyc_Tcutil_compress(( void*)({
struct Cyc_Core_Opt* _temp1490= e->topt; if( _temp1490 == 0){ _throw(
Null_Exception);} _temp1490->v;})); if( Cyc_Tcutil_unify( t1, t2)){ return 1;}
if( Cyc_Tcutil_is_arithmetic_type( t1)? Cyc_Tcutil_is_arithmetic_type( t2): 0){
if( Cyc_Tcutil_will_lose_precision( t1, t2)){ Cyc_Tcutil_warn( e->loc,( struct
_tagged_string)({ char* _temp1486=( char*)"integral size mismatch; conversion supplied";
struct _tagged_string _temp1487; _temp1487.curr= _temp1486; _temp1487.base=
_temp1486; _temp1487.last_plus_one= _temp1486 + 44; _temp1487;}));} Cyc_Tcutil_unchecked_cast(
te, e, t2); return 1;} else{ if( Cyc_Tcutil_silent_castable( te, e->loc, t1, t2)){
Cyc_Tcutil_unchecked_cast( te, e, t2); return 1;} else{ if( Cyc_Tcutil_castable(
te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te, e, t2); Cyc_Tcutil_warn( e->loc,({
struct _tagged_string _temp1488= Cyc_Absynpp_typ2string( t1); struct
_tagged_string _temp1489= Cyc_Absynpp_typ2string( t2); xprintf("implicit cast from %.*s to %.*s",
_temp1488.last_plus_one - _temp1488.curr, _temp1488.curr, _temp1489.last_plus_one
- _temp1489.curr, _temp1489.curr);})); return 1;} else{ return 0;}}}} int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, void* t){ return Cyc_Tcutil_coerce_arg(
te, e, t);} static int Cyc_Tcutil_any_promote( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Exp* e){ void* old_typ= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp1521= e->topt; if( _temp1521 == 0){ _throw( Null_Exception);} _temp1521->v;}));
void* t;{ void* _temp1491= old_typ; struct Cyc_Absyn_Enumdecl* _temp1503; struct
_tuple1* _temp1505; void* _temp1507; void* _temp1509; _LL1493: if(( unsigned int)
_temp1491 > 4u?*(( int*) _temp1491) == Cyc_Absyn_EnumType: 0){ _LL1506:
_temp1505=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*) _temp1491)->f1;
goto _LL1504; _LL1504: _temp1503=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*)
_temp1491)->f2; goto _LL1494;} else{ goto _LL1495;} _LL1495: if(( unsigned int)
_temp1491 > 4u?*(( int*) _temp1491) == Cyc_Absyn_IntType: 0){ _LL1510: _temp1509=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1491)->f1; goto _LL1508; _LL1508:
_temp1507=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1491)->f2; goto
_LL1496;} else{ goto _LL1497;} _LL1497: if( _temp1491 ==( void*) Cyc_Absyn_FloatType){
goto _LL1498;} else{ goto _LL1499;} _LL1499: if( _temp1491 ==( void*) Cyc_Absyn_DoubleType){
goto _LL1500;} else{ goto _LL1501;} _LL1501: goto _LL1502; _LL1494: t= Cyc_Absyn_sint_t;
goto _LL1492; _LL1496:{ void* _temp1511= _temp1507; _LL1513: if( _temp1511 ==(
void*) Cyc_Absyn_B1){ goto _LL1514;} else{ goto _LL1515;} _LL1515: if( _temp1511
==( void*) Cyc_Absyn_B2){ goto _LL1516;} else{ goto _LL1517;} _LL1517: if(
_temp1511 ==( void*) Cyc_Absyn_B4){ goto _LL1518;} else{ goto _LL1519;} _LL1519:
if( _temp1511 ==( void*) Cyc_Absyn_B8){ goto _LL1520;} else{ goto _LL1512;}
_LL1514: t= Cyc_Absyn_sint_t; goto _LL1512; _LL1516: t= Cyc_Absyn_sint_t; goto
_LL1512; _LL1518: t= _temp1509 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t:
Cyc_Absyn_sint_t; goto _LL1512; _LL1520: t= _temp1509 ==( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_ulong_t: Cyc_Absyn_slong_t; goto _LL1512; _LL1512:;} goto _LL1492;
_LL1498: t= Cyc_Absyn_float_t; goto _LL1492; _LL1500: t= Cyc_Absyn_double_t;
goto _LL1492; _LL1502: return 0; _LL1492:;} Cyc_Tcutil_unchecked_cast( te, e, t);
return 1;} int Cyc_Tcutil_coerce_use( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, void* t2){ void* t1= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp1525= e->topt; if( _temp1525 == 0){ _throw( Null_Exception);} _temp1525->v;}));
if( Cyc_Tcutil_unify( t1, t2)){ return 1;} Cyc_Tcutil_any_promote( te, e); if(
Cyc_Tcutil_unify(( void*)({ struct Cyc_Core_Opt* _temp1522= e->topt; if(
_temp1522 == 0){ _throw( Null_Exception);} _temp1522->v;}), t2)){ return 1;}
else{ if( Cyc_Tcutil_silent_castable( te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast(
te, e, t2); return 1;} else{ if( Cyc_Tcutil_castable( te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast(
te, e, t2); Cyc_Tcutil_warn( e->loc,({ struct _tagged_string _temp1523= Cyc_Absynpp_typ2string(
t1); struct _tagged_string _temp1524= Cyc_Absynpp_typ2string( t2); xprintf("implicit cast from %.*s to %.*s",
_temp1523.last_plus_one - _temp1523.curr, _temp1523.curr, _temp1524.last_plus_one
- _temp1524.curr, _temp1524.curr);})); return 1;} else{ return 0;}}}} int Cyc_Tcutil_coerceable(
void* t){ void* _temp1526= Cyc_Tcutil_compress( t); void* _temp1536; void*
_temp1538; _LL1528: if(( unsigned int) _temp1526 > 4u?*(( int*) _temp1526) ==
Cyc_Absyn_IntType: 0){ _LL1539: _temp1538=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1526)->f1; goto _LL1537; _LL1537: _temp1536=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1526)->f2; goto _LL1529;} else{ goto _LL1530;} _LL1530: if( _temp1526 ==(
void*) Cyc_Absyn_FloatType){ goto _LL1531;} else{ goto _LL1532;} _LL1532: if(
_temp1526 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1533;} else{ goto _LL1534;}
_LL1534: goto _LL1535; _LL1529: return 1; _LL1531: return 1; _LL1533: return 1;
_LL1535: return 0; _LL1527:;} static struct _tuple4* Cyc_Tcutil_flatten_typ_f(
struct Cyc_List_List* inst, struct Cyc_Absyn_Structfield* x){ return({ struct
_tuple4* _temp1540=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1540->f1= x->tq; _temp1540->f2= Cyc_Tcutil_substitute( inst,( void*) x->type);
_temp1540;});} static struct Cyc_List_List* Cyc_Tcutil_flatten_typ( struct Cyc_Tcenv_Tenv*
te, void* t1){ t1= Cyc_Tcutil_compress( t1);{ void* _temp1541= t1; struct Cyc_List_List*
_temp1553; struct Cyc_Absyn_Structdecl** _temp1555; struct Cyc_List_List*
_temp1557; struct _tuple1* _temp1559; struct Cyc_List_List* _temp1561; _LL1543:
if( _temp1541 ==( void*) Cyc_Absyn_VoidType){ goto _LL1544;} else{ goto _LL1545;}
_LL1545: if(( unsigned int) _temp1541 > 4u?*(( int*) _temp1541) == Cyc_Absyn_TupleType:
0){ _LL1554: _temp1553=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp1541)->f1; goto _LL1546;} else{ goto _LL1547;} _LL1547: if(( unsigned int)
_temp1541 > 4u?*(( int*) _temp1541) == Cyc_Absyn_StructType: 0){ _LL1560:
_temp1559=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp1541)->f1;
goto _LL1558; _LL1558: _temp1557=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp1541)->f2; goto _LL1556; _LL1556: _temp1555=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp1541)->f3; goto _LL1548;} else{ goto
_LL1549;} _LL1549: if(( unsigned int) _temp1541 > 4u?*(( int*) _temp1541) == Cyc_Absyn_AnonStructType:
0){ _LL1562: _temp1561=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp1541)->f1; goto _LL1550;} else{ goto _LL1551;} _LL1551: goto _LL1552;
_LL1544: return 0; _LL1546: return _temp1553; _LL1548: if( _temp1555 == 0? 1:
_temp1559 == 0){ return({ struct Cyc_List_List* _temp1563=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1563->hd=( void*)({ struct
_tuple4* _temp1564=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1564->f1= Cyc_Absyn_empty_tqual(); _temp1564->f2= t1; _temp1564;});
_temp1563->tl= 0; _temp1563;});}{ struct Cyc_Absyn_Structdecl* sd=({ struct Cyc_Absyn_Structdecl**
_temp1569= _temp1555; if( _temp1569 == 0){ _throw( Null_Exception);}* _temp1569;});
struct _tuple1 n=({ struct _tuple1* _temp1568= _temp1559; if( _temp1568 == 0){
_throw( Null_Exception);}* _temp1568;}); struct Cyc_List_List* inst=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
sd->tvs, _temp1557); if( sd->fields == 0){ return({ struct Cyc_List_List*
_temp1565=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1565->hd=( void*)({ struct _tuple4* _temp1566=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp1566->f1= Cyc_Absyn_empty_tqual(); _temp1566->f2=
t1; _temp1566;}); _temp1565->tl= 0; _temp1565;});} return(( struct Cyc_List_List*(*)(
struct _tuple4*(* f)( struct Cyc_List_List*, struct Cyc_Absyn_Structfield*),
struct Cyc_List_List* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f,
inst,( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp1567= sd->fields; if(
_temp1567 == 0){ _throw( Null_Exception);} _temp1567->v;}));} _LL1550: return((
struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_List_List*, struct
Cyc_Absyn_Structfield*), struct Cyc_List_List* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, 0, _temp1561); _LL1552: return({
struct Cyc_List_List* _temp1570=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1570->hd=( void*)({ struct _tuple4* _temp1571=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1571->f1= Cyc_Absyn_empty_tqual();
_temp1571->f2= t1; _temp1571;}); _temp1570->tl= 0; _temp1570;}); _LL1542:;}}
static int Cyc_Tcutil_ptrsubtype( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
assume, void* t1, void* t2); static int Cyc_Tcutil_subtype( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* assume, void* t1, void* t2){ if( Cyc_Tcutil_unify( t1,
t2)){ return 1;}{ struct Cyc_List_List* a= assume; for( 0; a != 0; a=({ struct
Cyc_List_List* _temp1572= a; if( _temp1572 == 0){ _throw( Null_Exception);}
_temp1572->tl;})){ if( Cyc_Tcutil_unify( t1,(*(( struct _tuple7*)({ struct Cyc_List_List*
_temp1573= a; if( _temp1573 == 0){ _throw( Null_Exception);} _temp1573->hd;}))).f1)?
Cyc_Tcutil_unify( t2,(*(( struct _tuple7*)({ struct Cyc_List_List* _temp1574= a;
if( _temp1574 == 0){ _throw( Null_Exception);} _temp1574->hd;}))).f2): 0){
return 1;}}} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct
_tuple7 _temp1576=({ struct _tuple7 _temp1575; _temp1575.f1= t1; _temp1575.f2=
t2; _temp1575;}); void* _temp1582; struct Cyc_Absyn_PtrInfo _temp1584; struct
Cyc_Absyn_Conref* _temp1586; struct Cyc_Absyn_Tqual _temp1588; struct Cyc_Absyn_Conref*
_temp1590; void* _temp1592; void* _temp1594; void* _temp1596; struct Cyc_Absyn_PtrInfo
_temp1598; struct Cyc_Absyn_Conref* _temp1600; struct Cyc_Absyn_Tqual _temp1602;
struct Cyc_Absyn_Conref* _temp1604; void* _temp1606; void* _temp1608; _LL1578:
_LL1597: _temp1596= _temp1576.f1; if(( unsigned int) _temp1596 > 4u?*(( int*)
_temp1596) == Cyc_Absyn_PointerType: 0){ _LL1599: _temp1598=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1596)->f1; _LL1609: _temp1608=( void*)
_temp1598.elt_typ; goto _LL1607; _LL1607: _temp1606=( void*) _temp1598.rgn_typ;
goto _LL1605; _LL1605: _temp1604=( struct Cyc_Absyn_Conref*) _temp1598.nullable;
goto _LL1603; _LL1603: _temp1602=( struct Cyc_Absyn_Tqual) _temp1598.tq; goto
_LL1601; _LL1601: _temp1600=( struct Cyc_Absyn_Conref*) _temp1598.bounds; goto
_LL1583;} else{ goto _LL1580;} _LL1583: _temp1582= _temp1576.f2; if((
unsigned int) _temp1582 > 4u?*(( int*) _temp1582) == Cyc_Absyn_PointerType: 0){
_LL1585: _temp1584=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1582)->f1; _LL1595: _temp1594=( void*) _temp1584.elt_typ; goto _LL1593;
_LL1593: _temp1592=( void*) _temp1584.rgn_typ; goto _LL1591; _LL1591: _temp1590=(
struct Cyc_Absyn_Conref*) _temp1584.nullable; goto _LL1589; _LL1589: _temp1588=(
struct Cyc_Absyn_Tqual) _temp1584.tq; goto _LL1587; _LL1587: _temp1586=( struct
Cyc_Absyn_Conref*) _temp1584.bounds; goto _LL1579;} else{ goto _LL1580;} _LL1580:
goto _LL1581; _LL1579: if( _temp1602.q_const? ! _temp1588.q_const: 0){ return 0;}
if(( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp, _temp1604, _temp1590)?(( int(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp1604): 0)? !(( int(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp1590): 0){ return 0;}{
void* _temp1610= _temp1606; _LL1612: if( _temp1610 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL1613;} else{ goto _LL1614;} _LL1614: goto _LL1615; _LL1613: goto _LL1611;
_LL1615: Cyc_Tcutil_unify( _temp1606, _temp1592); goto _LL1611; _LL1611:;} if( !((
int(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y)) Cyc_Tcutil_unify_conrefs)( Cyc_Tcutil_boundscmp, _temp1600, _temp1586)){
struct _tuple7 _temp1617=({ struct _tuple7 _temp1616; _temp1616.f1=(( void*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp1600); _temp1616.f2=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp1586);
_temp1616;}); void* _temp1625; void* _temp1627; struct Cyc_Absyn_Exp* _temp1629;
void* _temp1631; struct Cyc_Absyn_Exp* _temp1633; void* _temp1635; struct Cyc_Absyn_Exp*
_temp1637; _LL1619: _LL1628: _temp1627= _temp1617.f1; if(( unsigned int)
_temp1627 > 1u?*(( int*) _temp1627) == Cyc_Absyn_Upper_b: 0){ _LL1630: _temp1629=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp1627)->f1; goto
_LL1626;} else{ goto _LL1621;} _LL1626: _temp1625= _temp1617.f2; if( _temp1625
==( void*) Cyc_Absyn_Unknown_b){ goto _LL1620;} else{ goto _LL1621;} _LL1621:
_LL1636: _temp1635= _temp1617.f1; if(( unsigned int) _temp1635 > 1u?*(( int*)
_temp1635) == Cyc_Absyn_Upper_b: 0){ _LL1638: _temp1637=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Upper_b_struct*) _temp1635)->f1; goto _LL1632;} else{ goto
_LL1623;} _LL1632: _temp1631= _temp1617.f2; if(( unsigned int) _temp1631 > 1u?*((
int*) _temp1631) == Cyc_Absyn_Upper_b: 0){ _LL1634: _temp1633=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Upper_b_struct*) _temp1631)->f1; goto _LL1622;} else{ goto
_LL1623;} _LL1623: goto _LL1624; _LL1620: goto _LL1618; _LL1622: if( Cyc_Evexp_eval_const_uint_exp(
_temp1637) < Cyc_Evexp_eval_const_uint_exp( _temp1633)){ return 0;} goto _LL1618;
_LL1624: return 0; _LL1618:;} return Cyc_Tcutil_ptrsubtype( te,({ struct Cyc_List_List*
_temp1639=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1639->hd=( void*)({ struct _tuple7* _temp1640=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp1640->f1= t1; _temp1640->f2= t2; _temp1640;});
_temp1639->tl= assume; _temp1639;}), _temp1608, _temp1594); _LL1581: return 0;
_LL1577:;}} static int Cyc_Tcutil_ptrsubtype( struct Cyc_Tcenv_Tenv* te, struct
Cyc_List_List* assume, void* t1, void* t2){ struct Cyc_List_List* tqs1= Cyc_Tcutil_flatten_typ(
te, t1); struct Cyc_List_List* tqs2= Cyc_Tcutil_flatten_typ( te, t2); for( 0;
tqs2 != 0; tqs2=({ struct Cyc_List_List* _temp1641= tqs2; if( _temp1641 == 0){
_throw( Null_Exception);} _temp1641->tl;}), tqs1=({ struct Cyc_List_List*
_temp1642= tqs1; if( _temp1642 == 0){ _throw( Null_Exception);} _temp1642->tl;})){
if( tqs1 == 0){ return 0;}{ struct _tuple4 _temp1646; void* _temp1647; struct
Cyc_Absyn_Tqual _temp1649; struct _tuple4* _temp1644=( struct _tuple4*)({ struct
Cyc_List_List* _temp1643= tqs1; if( _temp1643 == 0){ _throw( Null_Exception);}
_temp1643->hd;}); _temp1646=* _temp1644; _LL1650: _temp1649= _temp1646.f1; goto
_LL1648; _LL1648: _temp1647= _temp1646.f2; goto _LL1645; _LL1645: { struct
_tuple4 _temp1654; void* _temp1655; struct Cyc_Absyn_Tqual _temp1657; struct
_tuple4* _temp1652=( struct _tuple4*)({ struct Cyc_List_List* _temp1651= tqs2;
if( _temp1651 == 0){ _throw( Null_Exception);} _temp1651->hd;}); _temp1654=*
_temp1652; _LL1658: _temp1657= _temp1654.f1; goto _LL1656; _LL1656: _temp1655=
_temp1654.f2; goto _LL1653; _LL1653: if( _temp1657.q_const? Cyc_Tcutil_subtype(
te, assume, _temp1647, _temp1655): 0){ continue;} else{ if( Cyc_Tcutil_unify(
_temp1647, _temp1655)){ continue;} else{ return 0;}}}}} return 1;} int Cyc_Tcutil_castable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* t1, void* t2){
if( Cyc_Tcutil_unify( t1, t2)){ return 1;} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress(
t2);{ void* _temp1659= t2; struct Cyc_Absyn_Uniondecl** _temp1667; struct Cyc_List_List*
_temp1669; struct _tuple1* _temp1671; void* _temp1673; void* _temp1675; _LL1661:
if(( unsigned int) _temp1659 > 4u?*(( int*) _temp1659) == Cyc_Absyn_UnionType: 0){
_LL1672: _temp1671=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp1659)->f1; goto _LL1670; _LL1670: _temp1669=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnionType_struct*) _temp1659)->f2; goto _LL1668; _LL1668:
_temp1667=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp1659)->f3; goto _LL1662;} else{ goto _LL1663;} _LL1663: if(( unsigned int)
_temp1659 > 4u?*(( int*) _temp1659) == Cyc_Absyn_IntType: 0){ _LL1676: _temp1675=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1659)->f1; goto _LL1674; _LL1674:
_temp1673=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1659)->f2; if(
_temp1673 ==( void*) Cyc_Absyn_B4){ goto _LL1664;} else{ goto _LL1665;}} else{
goto _LL1665;} _LL1665: goto _LL1666; _LL1662: if( _temp1667 == 0){ goto _LL1660;}{
struct Cyc_Absyn_Uniondecl* ud=({ struct Cyc_Absyn_Uniondecl** _temp1680=
_temp1667; if( _temp1680 == 0){ _throw( Null_Exception);}* _temp1680;}); if( ud->fields
== 0){ goto _LL1660;}{ struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( ud->tvs,
_temp1669);{ struct Cyc_List_List* fs=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp1679= ud->fields; if( _temp1679 == 0){ _throw( Null_Exception);} _temp1679->v;});
for( 0; fs != 0; fs=({ struct Cyc_List_List* _temp1677= fs; if( _temp1677 == 0){
_throw( Null_Exception);} _temp1677->tl;})){ if( Cyc_Tcutil_unify( t1, Cyc_Tcutil_substitute(
inst,( void*)(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp1678=
fs; if( _temp1678 == 0){ _throw( Null_Exception);} _temp1678->hd;}))->type))){
return 1;}}} goto _LL1660;}} _LL1664: if( Cyc_Tcutil_typ_kind( t2) ==( void*)
Cyc_Absyn_BoxKind){ return 1;} goto _LL1660; _LL1666: goto _LL1660; _LL1660:;}{
void* _temp1681= t1; struct Cyc_Absyn_PtrInfo _temp1697; struct Cyc_Absyn_Conref*
_temp1699; struct Cyc_Absyn_Tqual _temp1701; struct Cyc_Absyn_Conref* _temp1703;
void* _temp1705; void* _temp1707; struct Cyc_Absyn_Exp* _temp1709; struct Cyc_Absyn_Tqual
_temp1711; void* _temp1713; struct Cyc_Absyn_Enumdecl* _temp1715; struct _tuple1*
_temp1717; void* _temp1719; void* _temp1721; _LL1683: if(( unsigned int)
_temp1681 > 4u?*(( int*) _temp1681) == Cyc_Absyn_PointerType: 0){ _LL1698:
_temp1697=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1681)->f1; _LL1708: _temp1707=( void*) _temp1697.elt_typ; goto _LL1706;
_LL1706: _temp1705=( void*) _temp1697.rgn_typ; goto _LL1704; _LL1704: _temp1703=(
struct Cyc_Absyn_Conref*) _temp1697.nullable; goto _LL1702; _LL1702: _temp1701=(
struct Cyc_Absyn_Tqual) _temp1697.tq; goto _LL1700; _LL1700: _temp1699=( struct
Cyc_Absyn_Conref*) _temp1697.bounds; goto _LL1684;} else{ goto _LL1685;} _LL1685:
if(( unsigned int) _temp1681 > 4u?*(( int*) _temp1681) == Cyc_Absyn_ArrayType: 0){
_LL1714: _temp1713=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1681)->f1;
goto _LL1712; _LL1712: _temp1711=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1681)->f2; goto _LL1710; _LL1710: _temp1709=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1681)->f3; goto _LL1686;} else{ goto
_LL1687;} _LL1687: if(( unsigned int) _temp1681 > 4u?*(( int*) _temp1681) == Cyc_Absyn_EnumType:
0){ _LL1718: _temp1717=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*)
_temp1681)->f1; goto _LL1716; _LL1716: _temp1715=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_EnumType_struct*) _temp1681)->f2; goto _LL1688;} else{ goto
_LL1689;} _LL1689: if(( unsigned int) _temp1681 > 4u?*(( int*) _temp1681) == Cyc_Absyn_IntType:
0){ _LL1722: _temp1721=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1681)->f1;
goto _LL1720; _LL1720: _temp1719=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1681)->f2; goto _LL1690;} else{ goto _LL1691;} _LL1691: if( _temp1681 ==(
void*) Cyc_Absyn_FloatType){ goto _LL1692;} else{ goto _LL1693;} _LL1693: if(
_temp1681 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1694;} else{ goto _LL1695;}
_LL1695: goto _LL1696; _LL1684:{ void* _temp1723= t2; struct Cyc_Absyn_PtrInfo
_temp1729; struct Cyc_Absyn_Conref* _temp1731; struct Cyc_Absyn_Tqual _temp1733;
struct Cyc_Absyn_Conref* _temp1735; void* _temp1737; void* _temp1739; _LL1725:
if(( unsigned int) _temp1723 > 4u?*(( int*) _temp1723) == Cyc_Absyn_PointerType:
0){ _LL1730: _temp1729=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1723)->f1; _LL1740: _temp1739=( void*) _temp1729.elt_typ; goto _LL1738;
_LL1738: _temp1737=( void*) _temp1729.rgn_typ; goto _LL1736; _LL1736: _temp1735=(
struct Cyc_Absyn_Conref*) _temp1729.nullable; goto _LL1734; _LL1734: _temp1733=(
struct Cyc_Absyn_Tqual) _temp1729.tq; goto _LL1732; _LL1732: _temp1731=( struct
Cyc_Absyn_Conref*) _temp1729.bounds; goto _LL1726;} else{ goto _LL1727;} _LL1727:
goto _LL1728; _LL1726: { struct Cyc_List_List* assump=({ struct Cyc_List_List*
_temp1756=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1756->hd=( void*)({ struct _tuple7* _temp1757=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp1757->f1= t1; _temp1757->f2= t2; _temp1757;});
_temp1756->tl= 0; _temp1756;}); int ptrsub= Cyc_Tcutil_ptrsubtype( te, assump,
_temp1707, _temp1739)? ! _temp1701.q_const? 1: _temp1733.q_const: 0; int
bounds_ok=(( int(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x,
struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Tcutil_boundscmp,
_temp1699, _temp1731); if( ! bounds_ok){ struct _tuple7 _temp1742=({ struct
_tuple7 _temp1741; _temp1741.f1=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1699); _temp1741.f2=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1731); _temp1741;}); void* _temp1748; struct Cyc_Absyn_Exp* _temp1750; void*
_temp1752; struct Cyc_Absyn_Exp* _temp1754; _LL1744: _LL1753: _temp1752=
_temp1742.f1; if(( unsigned int) _temp1752 > 1u?*(( int*) _temp1752) == Cyc_Absyn_Upper_b:
0){ _LL1755: _temp1754=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp1752)->f1; goto _LL1749;} else{ goto _LL1746;} _LL1749: _temp1748=
_temp1742.f2; if(( unsigned int) _temp1748 > 1u?*(( int*) _temp1748) == Cyc_Absyn_Upper_b:
0){ _LL1751: _temp1750=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp1748)->f1; goto _LL1745;} else{ goto _LL1746;} _LL1746: goto _LL1747;
_LL1745: if( Cyc_Evexp_eval_const_uint_exp( _temp1754) >= Cyc_Evexp_eval_const_uint_exp(
_temp1750)){ bounds_ok= 1;} goto _LL1743; _LL1747: bounds_ok= 1; goto _LL1743;
_LL1743:;} return( bounds_ok? ptrsub: 0)? _temp1705 ==( void*) Cyc_Absyn_HeapRgn?
1: Cyc_Tcutil_unify( _temp1705, _temp1737): 0;} _LL1728: goto _LL1724; _LL1724:;}
return 0; _LL1686:{ void* _temp1758= t2; struct Cyc_Absyn_Exp* _temp1764; struct
Cyc_Absyn_Tqual _temp1766; void* _temp1768; _LL1760: if(( unsigned int)
_temp1758 > 4u?*(( int*) _temp1758) == Cyc_Absyn_ArrayType: 0){ _LL1769:
_temp1768=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1758)->f1; goto
_LL1767; _LL1767: _temp1766=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1758)->f2; goto _LL1765; _LL1765: _temp1764=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1758)->f3; goto _LL1761;} else{ goto
_LL1762;} _LL1762: goto _LL1763; _LL1761: { int okay; okay=( _temp1709 != 0?
_temp1764 != 0: 0)? Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1770= _temp1709; if( _temp1770 == 0){ _throw(
Null_Exception);} _temp1770;})) >= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1771= _temp1764; if( _temp1771 == 0){ _throw(
Null_Exception);} _temp1771;})): 0; return( okay? Cyc_Tcutil_unify( _temp1713,
_temp1768): 0)? ! _temp1711.q_const? 1: _temp1766.q_const: 0;} _LL1763: return 0;
_LL1759:;} return 0; _LL1688: goto _LL1690; _LL1690: goto _LL1692; _LL1692: goto
_LL1694; _LL1694: return Cyc_Tcutil_coerceable( t2); _LL1696: return 0; _LL1682:;}}
void Cyc_Tcutil_unchecked_cast( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, void* t){ if( ! Cyc_Tcutil_unify(( void*)({ struct Cyc_Core_Opt* _temp1772= e->topt;
if( _temp1772 == 0){ _throw( Null_Exception);} _temp1772->v;}), t)){ struct Cyc_Absyn_Exp*
inner= Cyc_Absyn_copy_exp( e);( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Cast_e_struct*
_temp1773=( struct Cyc_Absyn_Cast_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct));
_temp1773[ 0]=({ struct Cyc_Absyn_Cast_e_struct _temp1774; _temp1774.tag= Cyc_Absyn_Cast_e;
_temp1774.f1=( void*) t; _temp1774.f2= inner; _temp1774;}); _temp1773;}))); e->topt=({
struct Cyc_Core_Opt* _temp1775=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1775->v=( void*) t; _temp1775;});}} int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp* e){ void* _temp1777= Cyc_Tcutil_compress(( void*)({ struct
Cyc_Core_Opt* _temp1776= e->topt; if( _temp1776 == 0){ _throw( Null_Exception);}
_temp1776->v;})); struct Cyc_Absyn_Enumdecl* _temp1787; struct _tuple1*
_temp1789; void* _temp1791; void* _temp1793; int _temp1795; struct Cyc_Core_Opt*
_temp1797; void* _temp1799; _LL1779: if(( unsigned int) _temp1777 > 4u?*(( int*)
_temp1777) == Cyc_Absyn_EnumType: 0){ _LL1790: _temp1789=( struct _tuple1*)((
struct Cyc_Absyn_EnumType_struct*) _temp1777)->f1; goto _LL1788; _LL1788:
_temp1787=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*)
_temp1777)->f2; goto _LL1780;} else{ goto _LL1781;} _LL1781: if(( unsigned int)
_temp1777 > 4u?*(( int*) _temp1777) == Cyc_Absyn_IntType: 0){ _LL1794: _temp1793=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1777)->f1; goto _LL1792; _LL1792:
_temp1791=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1777)->f2; goto
_LL1782;} else{ goto _LL1783;} _LL1783: if(( unsigned int) _temp1777 > 4u?*((
int*) _temp1777) == Cyc_Absyn_Evar: 0){ _LL1800: _temp1799=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp1777)->f1; goto _LL1798; _LL1798: _temp1797=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Evar_struct*) _temp1777)->f2; goto _LL1796; _LL1796: _temp1795=(
int)(( struct Cyc_Absyn_Evar_struct*) _temp1777)->f3; goto _LL1784;} else{ goto
_LL1785;} _LL1785: goto _LL1786; _LL1780: goto _LL1782; _LL1782: return 1;
_LL1784: if( Cyc_Tcutil_unify(( void*)({ struct Cyc_Core_Opt* _temp1801= e->topt;
if( _temp1801 == 0){ _throw( Null_Exception);} _temp1801->v;}), Cyc_Absyn_sint_t)){
return 1;} return 0; _LL1786: return 0; _LL1778:;} int Cyc_Tcutil_is_numeric(
struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_is_integral( e)){ return 1;}{ void*
_temp1803= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp1802= e->topt;
if( _temp1802 == 0){ _throw( Null_Exception);} _temp1802->v;})); _LL1805: if(
_temp1803 ==( void*) Cyc_Absyn_FloatType){ goto _LL1806;} else{ goto _LL1807;}
_LL1807: if( _temp1803 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1808;} else{
goto _LL1809;} _LL1809: goto _LL1810; _LL1806: goto _LL1808; _LL1808: return 1;
_LL1810: return 0; _LL1804:;}} int Cyc_Tcutil_is_function_type( void* t){ void*
_temp1811= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo _temp1817; _LL1813:
if(( unsigned int) _temp1811 > 4u?*(( int*) _temp1811) == Cyc_Absyn_FnType: 0){
_LL1818: _temp1817=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp1811)->f1; goto _LL1814;} else{ goto _LL1815;} _LL1815: goto _LL1816;
_LL1814: return 1; _LL1816: return 0; _LL1812:;} void* Cyc_Tcutil_max_arithmetic_type(
void* t1, void* t2){ struct _tuple7 _temp1820=({ struct _tuple7 _temp1819;
_temp1819.f1= t1; _temp1819.f2= t2; _temp1819;}); void* _temp1844; void*
_temp1846; void* _temp1848; void* _temp1850; void* _temp1852; void* _temp1854;
void* _temp1856; void* _temp1858; void* _temp1860; void* _temp1862; void*
_temp1864; void* _temp1866; void* _temp1868; void* _temp1870; void* _temp1872;
void* _temp1874; void* _temp1876; void* _temp1878; void* _temp1880; void*
_temp1882; void* _temp1884; void* _temp1886; void* _temp1888; void* _temp1890;
void* _temp1892; void* _temp1894; void* _temp1896; void* _temp1898; void*
_temp1900; void* _temp1902; void* _temp1904; void* _temp1906; _LL1822: _LL1847:
_temp1846= _temp1820.f1; if( _temp1846 ==( void*) Cyc_Absyn_DoubleType){ goto
_LL1845;} else{ goto _LL1824;} _LL1845: _temp1844= _temp1820.f2; goto _LL1823;
_LL1824: _LL1851: _temp1850= _temp1820.f1; goto _LL1849; _LL1849: _temp1848=
_temp1820.f2; if( _temp1848 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1825;}
else{ goto _LL1826;} _LL1826: _LL1855: _temp1854= _temp1820.f1; if( _temp1854 ==(
void*) Cyc_Absyn_FloatType){ goto _LL1853;} else{ goto _LL1828;} _LL1853:
_temp1852= _temp1820.f2; goto _LL1827; _LL1828: _LL1859: _temp1858= _temp1820.f1;
goto _LL1857; _LL1857: _temp1856= _temp1820.f2; if( _temp1856 ==( void*) Cyc_Absyn_FloatType){
goto _LL1829;} else{ goto _LL1830;} _LL1830: _LL1863: _temp1862= _temp1820.f1;
if(( unsigned int) _temp1862 > 4u?*(( int*) _temp1862) == Cyc_Absyn_IntType: 0){
_LL1867: _temp1866=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1862)->f1;
if( _temp1866 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1865;} else{ goto _LL1832;}
_LL1865: _temp1864=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1862)->f2;
if( _temp1864 ==( void*) Cyc_Absyn_B8){ goto _LL1861;} else{ goto _LL1832;}}
else{ goto _LL1832;} _LL1861: _temp1860= _temp1820.f2; goto _LL1831; _LL1832:
_LL1875: _temp1874= _temp1820.f1; goto _LL1869; _LL1869: _temp1868= _temp1820.f2;
if(( unsigned int) _temp1868 > 4u?*(( int*) _temp1868) == Cyc_Absyn_IntType: 0){
_LL1873: _temp1872=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1868)->f1;
if( _temp1872 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1871;} else{ goto _LL1834;}
_LL1871: _temp1870=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1868)->f2;
if( _temp1870 ==( void*) Cyc_Absyn_B8){ goto _LL1833;} else{ goto _LL1834;}}
else{ goto _LL1834;} _LL1834: _LL1879: _temp1878= _temp1820.f1; if((
unsigned int) _temp1878 > 4u?*(( int*) _temp1878) == Cyc_Absyn_IntType: 0){
_LL1883: _temp1882=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1878)->f1;
if( _temp1882 ==( void*) Cyc_Absyn_Signed){ goto _LL1881;} else{ goto _LL1836;}
_LL1881: _temp1880=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1878)->f2;
if( _temp1880 ==( void*) Cyc_Absyn_B8){ goto _LL1877;} else{ goto _LL1836;}}
else{ goto _LL1836;} _LL1877: _temp1876= _temp1820.f2; goto _LL1835; _LL1836:
_LL1891: _temp1890= _temp1820.f1; goto _LL1885; _LL1885: _temp1884= _temp1820.f2;
if(( unsigned int) _temp1884 > 4u?*(( int*) _temp1884) == Cyc_Absyn_IntType: 0){
_LL1889: _temp1888=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1884)->f1;
if( _temp1888 ==( void*) Cyc_Absyn_Signed){ goto _LL1887;} else{ goto _LL1838;}
_LL1887: _temp1886=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1884)->f2;
if( _temp1886 ==( void*) Cyc_Absyn_B8){ goto _LL1837;} else{ goto _LL1838;}}
else{ goto _LL1838;} _LL1838: _LL1895: _temp1894= _temp1820.f1; if((
unsigned int) _temp1894 > 4u?*(( int*) _temp1894) == Cyc_Absyn_IntType: 0){
_LL1899: _temp1898=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1894)->f1;
if( _temp1898 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1897;} else{ goto _LL1840;}
_LL1897: _temp1896=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1894)->f2;
if( _temp1896 ==( void*) Cyc_Absyn_B4){ goto _LL1893;} else{ goto _LL1840;}}
else{ goto _LL1840;} _LL1893: _temp1892= _temp1820.f2; goto _LL1839; _LL1840:
_LL1907: _temp1906= _temp1820.f1; goto _LL1901; _LL1901: _temp1900= _temp1820.f2;
if(( unsigned int) _temp1900 > 4u?*(( int*) _temp1900) == Cyc_Absyn_IntType: 0){
_LL1905: _temp1904=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1900)->f1;
if( _temp1904 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1903;} else{ goto _LL1842;}
_LL1903: _temp1902=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1900)->f2;
if( _temp1902 ==( void*) Cyc_Absyn_B4){ goto _LL1841;} else{ goto _LL1842;}}
else{ goto _LL1842;} _LL1842: goto _LL1843; _LL1823: goto _LL1825; _LL1825:
return( void*) Cyc_Absyn_DoubleType; _LL1827: goto _LL1829; _LL1829: return(
void*) Cyc_Absyn_FloatType; _LL1831: goto _LL1833; _LL1833: return Cyc_Absyn_ulong_t;
_LL1835: goto _LL1837; _LL1837: return Cyc_Absyn_slong_t; _LL1839: goto _LL1841;
_LL1841: return Cyc_Absyn_uint_t; _LL1843: return Cyc_Absyn_sint_t; _LL1821:;}
void Cyc_Tcutil_check_contains_assign( struct Cyc_Absyn_Exp* e){ void* _temp1908=(
void*) e->r; struct Cyc_Absyn_Exp* _temp1914; struct Cyc_Core_Opt* _temp1916;
struct Cyc_Absyn_Exp* _temp1918; _LL1910: if(*(( int*) _temp1908) == Cyc_Absyn_AssignOp_e){
_LL1919: _temp1918=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1908)->f1; goto _LL1917; _LL1917: _temp1916=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1908)->f2; if( _temp1916 == 0){ goto
_LL1915;} else{ goto _LL1912;} _LL1915: _temp1914=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1908)->f3; goto _LL1911;} else{ goto
_LL1912;} _LL1912: goto _LL1913; _LL1911: Cyc_Tcutil_warn( e->loc,( struct
_tagged_string)({ char* _temp1920=( char*)"assignment in test"; struct
_tagged_string _temp1921; _temp1921.curr= _temp1920; _temp1921.base= _temp1920;
_temp1921.last_plus_one= _temp1920 + 19; _temp1921;})); goto _LL1909; _LL1913:
goto _LL1909; _LL1909:;} static int Cyc_Tcutil_constrain_kinds( struct Cyc_Absyn_Conref*
c1, struct Cyc_Absyn_Conref* c2){ c1=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( c1); c2=(( struct Cyc_Absyn_Conref*(*)( struct
Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( c2); if( c1 == c2){ return 1;}
if(( void*) c1->v ==( void*) Cyc_Absyn_No_constr){( void*)( c1->v=( void*)((
void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp1922=( struct Cyc_Absyn_Forward_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp1922[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp1923; _temp1923.tag= Cyc_Absyn_Forward_constr;
_temp1923.f1= c2; _temp1923;}); _temp1922;}))); return 1;} else{ if(( void*) c2->v
==( void*) Cyc_Absyn_No_constr){( void*)( c2->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp1924=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp1924[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp1925; _temp1925.tag= Cyc_Absyn_Forward_constr; _temp1925.f1= c1; _temp1925;});
_temp1924;}))); return 1;} else{ void* k1=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( c1); void* k2=(( void*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_val)( c2); if( Cyc_Tcutil_kind_leq( k1, k2)){( void*)( c2->v=(
void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp1926=( struct Cyc_Absyn_Forward_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp1926[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp1927; _temp1927.tag= Cyc_Absyn_Forward_constr;
_temp1927.f1= c1; _temp1927;}); _temp1926;}))); return 1;} else{ if( Cyc_Tcutil_kind_leq(
k2, k1)){( void*)( c1->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp1928=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp1928[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp1929; _temp1929.tag= Cyc_Absyn_Forward_constr; _temp1929.f1= c2; _temp1929;});
_temp1928;}))); return 1;} else{ return 0;}}}}} static struct Cyc_List_List* Cyc_Tcutil_add_free_tvar(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar*
tv){ if( tvs == 0){ return({ struct Cyc_List_List* _temp1930=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1930->hd=( void*) tv; _temp1930->tl=
0; _temp1930;});} else{{ struct Cyc_List_List* tvs2= tvs; for( 0; tvs2 != 0;
tvs2=({ struct Cyc_List_List* _temp1931= tvs2; if( _temp1931 == 0){ _throw(
Null_Exception);} _temp1931->tl;})){ if( Cyc_String_zstrptrcmp((( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp1932= tvs2; if( _temp1932 == 0){ _throw(
Null_Exception);} _temp1932->hd;}))->name, tv->name) == 0){ struct Cyc_Absyn_Conref*
k1=(( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp1936= tvs2; if(
_temp1936 == 0){ _throw( Null_Exception);} _temp1936->hd;}))->kind; struct Cyc_Absyn_Conref*
k2= tv->kind; if( ! Cyc_Tcutil_constrain_kinds( k1, k2)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp1933=* tv->name; struct _tagged_string _temp1934= Cyc_Absynpp_ckind2string(
k1); struct _tagged_string _temp1935= Cyc_Absynpp_ckind2string( k2); xprintf("type variable %.*s is used with inconsistent kinds %.*s and %.*s",
_temp1933.last_plus_one - _temp1933.curr, _temp1933.curr, _temp1934.last_plus_one
- _temp1934.curr, _temp1934.curr, _temp1935.last_plus_one - _temp1935.curr,
_temp1935.curr);}));} return tvs;} else{ if(({ struct Cyc_List_List* _temp1937=
tvs2; if( _temp1937 == 0){ _throw( Null_Exception);} _temp1937->tl;}) == 0){({
struct Cyc_List_List* _temp1938= tvs2; if( _temp1938 == 0){ _throw(
Null_Exception);} _temp1938->tl=({ struct Cyc_List_List* _temp1939=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1939->hd=( void*) tv; _temp1939->tl=
0; _temp1939;});}); return tvs;}}}} return(( struct Cyc_List_List*(*)( struct
_tagged_string msg)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp1940=( char*)"Tcutil::add_free_tvar:expecting to have returned."; struct
_tagged_string _temp1941; _temp1941.curr= _temp1940; _temp1941.base= _temp1940;
_temp1941.last_plus_one= _temp1940 + 50; _temp1941;}));}} static struct Cyc_List_List*
Cyc_Tcutil_remove_bound_tvars( struct Cyc_List_List* tvs, struct Cyc_List_List*
btvs){ struct Cyc_List_List* r= 0; for( 0; tvs != 0; tvs=({ struct Cyc_List_List*
_temp1942= tvs; if( _temp1942 == 0){ _throw( Null_Exception);} _temp1942->tl;})){
int present= 0;{ struct Cyc_List_List* b= btvs; for( 0; b != 0; b=({ struct Cyc_List_List*
_temp1943= b; if( _temp1943 == 0){ _throw( Null_Exception);} _temp1943->tl;})){
if( Cyc_String_zstrptrcmp((( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp1944= tvs; if( _temp1944 == 0){ _throw( Null_Exception);} _temp1944->hd;}))->name,((
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp1945= b; if( _temp1945 == 0){
_throw( Null_Exception);} _temp1945->hd;}))->name) == 0){ present= 1; break;}}}
if( ! present){ r=({ struct Cyc_List_List* _temp1946=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1946->hd=( void*)(( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp1947= tvs; if( _temp1947 == 0){ _throw(
Null_Exception);} _temp1947->hd;})); _temp1946->tl= r; _temp1946;});}} r=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( r);
return r;} static int Cyc_Tcutil_tvar_counter= 0; struct Cyc_Absyn_Tvar* Cyc_Tcutil_new_tvar(
void* k){ struct _tagged_string s= xprintf("#%d",( Cyc_Tcutil_tvar_counter ++));
return({ struct Cyc_Absyn_Tvar* _temp1948=( struct Cyc_Absyn_Tvar*) GC_malloc(
sizeof( struct Cyc_Absyn_Tvar)); _temp1948->name=({ struct _tagged_string*
_temp1949=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp1949[ 0]= s; _temp1949;}); _temp1948->kind=(( struct Cyc_Absyn_Conref*(*)(
void* x)) Cyc_Absyn_new_conref)( k); _temp1948;});} int Cyc_Tcutil_is_temp_tvar(
struct Cyc_Absyn_Tvar* t){ struct _tagged_string s=* t->name; return({ struct
_tagged_string _temp1950= s; char* _temp1952= _temp1950.curr + 0; if( _temp1950.base
== 0? 1:( _temp1952 < _temp1950.base? 1: _temp1952 >= _temp1950.last_plus_one)){
_throw( Null_Exception);}* _temp1952;}) =='#';} void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar* t){ if( ! Cyc_Tcutil_is_temp_tvar( t)){ return;}{ struct
_tagged_string s= Cyc_String_strconcat(( struct _tagged_string)({ char*
_temp1957=( char*)"`"; struct _tagged_string _temp1958; _temp1958.curr=
_temp1957; _temp1958.base= _temp1957; _temp1958.last_plus_one= _temp1957 + 2;
_temp1958;}),* t->name);({ struct _tagged_string _temp1953= s; char* _temp1955=
_temp1953.curr + 1; if( _temp1953.base == 0? 1:( _temp1955 < _temp1953.base? 1:
_temp1955 >= _temp1953.last_plus_one)){ _throw( Null_Exception);}* _temp1955='t';});
t->name=({ struct _tagged_string* _temp1956=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp1956[ 0]= s; _temp1956;});}} struct
_tuple11{ struct _tagged_string* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ;
static struct _tuple2* Cyc_Tcutil_fndecl2typ_f( struct _tuple11* x){ return({
struct _tuple2* _temp1959=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp1959->f1=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt* _temp1960=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1960->v=( void*)(*
x).f1; _temp1960;}); _temp1959->f2=(* x).f2; _temp1959->f3=(* x).f3; _temp1959;});}
int Cyc_Tcutil_fntype_att( void* a){ void* _temp1961= a; int _temp1975; _LL1963:
if(( unsigned int) _temp1961 > 15u?*(( int*) _temp1961) == Cyc_Absyn_Regparm_att:
0){ _LL1976: _temp1975=( int)(( struct Cyc_Absyn_Regparm_att_struct*) _temp1961)->f1;
goto _LL1964;} else{ goto _LL1965;} _LL1965: if( _temp1961 ==( void*) Cyc_Absyn_Stdcall_att){
goto _LL1966;} else{ goto _LL1967;} _LL1967: if( _temp1961 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL1968;} else{ goto _LL1969;} _LL1969: if( _temp1961 ==( void*) Cyc_Absyn_Noreturn_att){
goto _LL1970;} else{ goto _LL1971;} _LL1971: if( _temp1961 ==( void*) Cyc_Absyn_Const_att){
goto _LL1972;} else{ goto _LL1973;} _LL1973: goto _LL1974; _LL1964: goto _LL1966;
_LL1966: goto _LL1968; _LL1968: goto _LL1970; _LL1970: goto _LL1972; _LL1972:
return 1; _LL1974: return 0; _LL1962:;} void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl*
fd){ if( fd->cached_typ == 0){ struct Cyc_List_List* fn_type_atts= 0;{ struct
Cyc_List_List* atts= fd->attributes; for( 0; atts != 0; atts=({ struct Cyc_List_List*
_temp1977= atts; if( _temp1977 == 0){ _throw( Null_Exception);} _temp1977->tl;})){
if( Cyc_Tcutil_fntype_att(( void*)({ struct Cyc_List_List* _temp1978= atts; if(
_temp1978 == 0){ _throw( Null_Exception);} _temp1978->hd;}))){ fn_type_atts=({
struct Cyc_List_List* _temp1979=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1979->hd=( void*)(( void*)({ struct Cyc_List_List*
_temp1980= atts; if( _temp1980 == 0){ _throw( Null_Exception);} _temp1980->hd;}));
_temp1979->tl= fn_type_atts; _temp1979;});}}} return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp1981=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp1981[ 0]=({ struct Cyc_Absyn_FnType_struct _temp1982; _temp1982.tag= Cyc_Absyn_FnType;
_temp1982.f1=({ struct Cyc_Absyn_FnInfo _temp1983; _temp1983.tvars= fd->tvs;
_temp1983.effect= fd->effect; _temp1983.ret_typ=( void*)(( void*) fd->ret_type);
_temp1983.args=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct
_tuple11*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_fndecl2typ_f, fd->args);
_temp1983.varargs= fd->varargs; _temp1983.attributes= fn_type_atts; _temp1983;});
_temp1982;}); _temp1981;});} return( void*)({ struct Cyc_Core_Opt* _temp1984= fd->cached_typ;
if( _temp1984 == 0){ _throw( Null_Exception);} _temp1984->v;});} struct _tuple12{
void* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; static void* Cyc_Tcutil_fst_fdarg(
struct _tuple12* t){ return(* t).f1;} void* Cyc_Tcutil_snd_tqt( struct _tuple4*
t){ return(* t).f2;} static struct _tuple4* Cyc_Tcutil_map2_tq( struct _tuple4*
pr, void* t){ return({ struct _tuple4* _temp1985=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp1985->f1=(* pr).f1; _temp1985->f2= t; _temp1985;});}
void Cyc_Tcutil_check_fndecl_valid_type( struct Cyc_Position_Segment* loc,
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Fndecl* fd){ void* t= Cyc_Tcutil_fndecl2typ(
fd); Cyc_Tcutil_check_valid_toplevel_type( loc, te, t);{ void* _temp1986= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_FnInfo _temp1992; struct Cyc_List_List* _temp1994; int
_temp1996; struct Cyc_List_List* _temp1998; void* _temp2000; struct Cyc_Core_Opt*
_temp2002; struct Cyc_List_List* _temp2004; _LL1988: if(( unsigned int)
_temp1986 > 4u?*(( int*) _temp1986) == Cyc_Absyn_FnType: 0){ _LL1993: _temp1992=(
struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*) _temp1986)->f1;
_LL2005: _temp2004=( struct Cyc_List_List*) _temp1992.tvars; goto _LL2003;
_LL2003: _temp2002=( struct Cyc_Core_Opt*) _temp1992.effect; goto _LL2001;
_LL2001: _temp2000=( void*) _temp1992.ret_typ; goto _LL1999; _LL1999: _temp1998=(
struct Cyc_List_List*) _temp1992.args; goto _LL1997; _LL1997: _temp1996=( int)
_temp1992.varargs; goto _LL1995; _LL1995: _temp1994=( struct Cyc_List_List*)
_temp1992.attributes; goto _LL1989;} else{ goto _LL1990;} _LL1990: goto _LL1991;
_LL1989: fd->tvs= _temp2004; fd->effect= _temp2002; goto _LL1987; _LL1991:((
void(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp2006=( char*)"check_fndecl_valid_type: not a FnType"; struct
_tagged_string _temp2007; _temp2007.curr= _temp2006; _temp2007.base= _temp2006;
_temp2007.last_plus_one= _temp2006 + 38; _temp2007;})); return; _LL1987:;}{
struct _RegionHandle _temp2008= _new_region(); struct _RegionHandle* r=&
_temp2008; _push_region( r); Cyc_Tcutil_check_unique_vars((( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tagged_string*(* f)( struct _tuple11*), struct
Cyc_List_List* x)) Cyc_List_rmap)( r,( struct _tagged_string*(*)( struct
_tuple11* t)) Cyc_Tcutil_fst_fdarg, fd->args), loc,( struct _tagged_string)({
char* _temp2009=( char*)"function declaration has repeated parameter"; struct
_tagged_string _temp2010; _temp2010.curr= _temp2009; _temp2010.base= _temp2009;
_temp2010.last_plus_one= _temp2009 + 44; _temp2010;}));; _pop_region(& _temp2008);}
fd->cached_typ=({ struct Cyc_Core_Opt* _temp2011=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2011->v=( void*) t; _temp2011;});}
struct _tuple13{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; } ; struct
_tuple14{ struct _tuple13* f1; void* f2; } ; static struct _tuple14* Cyc_Tcutil_substitute_f1(
struct _RegionHandle* rgn, struct _tuple2* y){ return({ struct _tuple14*
_temp2012=( struct _tuple14*) _region_malloc( rgn, sizeof( struct _tuple14));
_temp2012->f1=({ struct _tuple13* _temp2013=( struct _tuple13*) _region_malloc(
rgn, sizeof( struct _tuple13)); _temp2013->f1=(* y).f1; _temp2013->f2=(* y).f2;
_temp2013;}); _temp2012->f2=(* y).f3; _temp2012;});} static struct _tuple2* Cyc_Tcutil_substitute_f2(
struct _tuple14* w){ void* _temp2016; struct _tuple13* _temp2018; struct
_tuple14 _temp2014=* w; _LL2019: _temp2018= _temp2014.f1; goto _LL2017; _LL2017:
_temp2016= _temp2014.f2; goto _LL2015; _LL2015: { struct Cyc_Absyn_Tqual
_temp2022; struct Cyc_Core_Opt* _temp2024; struct _tuple13 _temp2020=* _temp2018;
_LL2025: _temp2024= _temp2020.f1; goto _LL2023; _LL2023: _temp2022= _temp2020.f2;
goto _LL2021; _LL2021: return({ struct _tuple2* _temp2026=( struct _tuple2*)
GC_malloc( sizeof( struct _tuple2)); _temp2026->f1= _temp2024; _temp2026->f2=
_temp2022; _temp2026->f3= _temp2016; _temp2026;});}} static void* Cyc_Tcutil_field_type(
struct Cyc_Absyn_Structfield* f){ return( void*) f->type;} static struct Cyc_Absyn_Structfield*
Cyc_Tcutil_zip_field_type( struct Cyc_Absyn_Structfield* f, void* t){ return({
struct Cyc_Absyn_Structfield* _temp2027=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2027->name= f->name;
_temp2027->tq= f->tq; _temp2027->type=( void*) t; _temp2027->width= f->width;
_temp2027->attributes= f->attributes; _temp2027;});} static struct Cyc_List_List*
Cyc_Tcutil_substs( struct _RegionHandle* rgn, struct Cyc_List_List* inst, struct
Cyc_List_List* ts); void* Cyc_Tcutil_rsubstitute( struct _RegionHandle* rgn,
struct Cyc_List_List* inst, void* t){ void* _temp2028= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Tvar* _temp2074; struct Cyc_Absyn_Structdecl** _temp2076;
struct Cyc_List_List* _temp2078; struct _tuple1* _temp2080; struct Cyc_Absyn_Uniondecl**
_temp2082; struct Cyc_List_List* _temp2084; struct _tuple1* _temp2086; struct
Cyc_Absyn_TunionInfo _temp2088; void* _temp2090; struct Cyc_List_List* _temp2092;
void* _temp2094; struct Cyc_Absyn_TunionFieldInfo _temp2096; struct Cyc_List_List*
_temp2098; void* _temp2100; struct Cyc_Core_Opt* _temp2102; struct Cyc_List_List*
_temp2104; struct _tuple1* _temp2106; struct Cyc_Absyn_Exp* _temp2108; struct
Cyc_Absyn_Tqual _temp2110; void* _temp2112; struct Cyc_Absyn_PtrInfo _temp2114;
struct Cyc_Absyn_Conref* _temp2116; struct Cyc_Absyn_Tqual _temp2118; struct Cyc_Absyn_Conref*
_temp2120; void* _temp2122; void* _temp2124; struct Cyc_Absyn_FnInfo _temp2126;
struct Cyc_List_List* _temp2128; int _temp2130; struct Cyc_List_List* _temp2132;
void* _temp2134; struct Cyc_Core_Opt* _temp2136; struct Cyc_List_List* _temp2138;
struct Cyc_List_List* _temp2140; struct Cyc_List_List* _temp2142; struct Cyc_List_List*
_temp2144; int _temp2146; struct Cyc_Core_Opt* _temp2148; void* _temp2150; void*
_temp2152; struct Cyc_Absyn_Enumdecl* _temp2154; struct _tuple1* _temp2156; void*
_temp2158; void* _temp2160; void* _temp2162; struct Cyc_List_List* _temp2164;
_LL2030: if(( unsigned int) _temp2028 > 4u?*(( int*) _temp2028) == Cyc_Absyn_VarType:
0){ _LL2075: _temp2074=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp2028)->f1; goto _LL2031;} else{ goto _LL2032;} _LL2032: if(( unsigned int)
_temp2028 > 4u?*(( int*) _temp2028) == Cyc_Absyn_StructType: 0){ _LL2081:
_temp2080=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp2028)->f1;
goto _LL2079; _LL2079: _temp2078=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp2028)->f2; goto _LL2077; _LL2077: _temp2076=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp2028)->f3; goto _LL2033;} else{ goto
_LL2034;} _LL2034: if(( unsigned int) _temp2028 > 4u?*(( int*) _temp2028) == Cyc_Absyn_UnionType:
0){ _LL2087: _temp2086=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp2028)->f1; goto _LL2085; _LL2085: _temp2084=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnionType_struct*) _temp2028)->f2; goto _LL2083; _LL2083:
_temp2082=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp2028)->f3; goto _LL2035;} else{ goto _LL2036;} _LL2036: if(( unsigned int)
_temp2028 > 4u?*(( int*) _temp2028) == Cyc_Absyn_TunionType: 0){ _LL2089:
_temp2088=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp2028)->f1; _LL2095: _temp2094=( void*) _temp2088.tunion_info; goto _LL2093;
_LL2093: _temp2092=( struct Cyc_List_List*) _temp2088.targs; goto _LL2091;
_LL2091: _temp2090=( void*) _temp2088.rgn; goto _LL2037;} else{ goto _LL2038;}
_LL2038: if(( unsigned int) _temp2028 > 4u?*(( int*) _temp2028) == Cyc_Absyn_TunionFieldType:
0){ _LL2097: _temp2096=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp2028)->f1; _LL2101: _temp2100=( void*) _temp2096.field_info; goto _LL2099;
_LL2099: _temp2098=( struct Cyc_List_List*) _temp2096.targs; goto _LL2039;}
else{ goto _LL2040;} _LL2040: if(( unsigned int) _temp2028 > 4u?*(( int*)
_temp2028) == Cyc_Absyn_TypedefType: 0){ _LL2107: _temp2106=( struct _tuple1*)((
struct Cyc_Absyn_TypedefType_struct*) _temp2028)->f1; goto _LL2105; _LL2105:
_temp2104=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp2028)->f2; goto _LL2103; _LL2103: _temp2102=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_TypedefType_struct*) _temp2028)->f3; goto _LL2041;} else{ goto
_LL2042;} _LL2042: if(( unsigned int) _temp2028 > 4u?*(( int*) _temp2028) == Cyc_Absyn_ArrayType:
0){ _LL2113: _temp2112=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2028)->f1;
goto _LL2111; _LL2111: _temp2110=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2028)->f2; goto _LL2109; _LL2109: _temp2108=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2028)->f3; goto _LL2043;} else{ goto
_LL2044;} _LL2044: if(( unsigned int) _temp2028 > 4u?*(( int*) _temp2028) == Cyc_Absyn_PointerType:
0){ _LL2115: _temp2114=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp2028)->f1; _LL2125: _temp2124=( void*) _temp2114.elt_typ; goto _LL2123;
_LL2123: _temp2122=( void*) _temp2114.rgn_typ; goto _LL2121; _LL2121: _temp2120=(
struct Cyc_Absyn_Conref*) _temp2114.nullable; goto _LL2119; _LL2119: _temp2118=(
struct Cyc_Absyn_Tqual) _temp2114.tq; goto _LL2117; _LL2117: _temp2116=( struct
Cyc_Absyn_Conref*) _temp2114.bounds; goto _LL2045;} else{ goto _LL2046;} _LL2046:
if(( unsigned int) _temp2028 > 4u?*(( int*) _temp2028) == Cyc_Absyn_FnType: 0){
_LL2127: _temp2126=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp2028)->f1; _LL2139: _temp2138=( struct Cyc_List_List*) _temp2126.tvars;
goto _LL2137; _LL2137: _temp2136=( struct Cyc_Core_Opt*) _temp2126.effect; goto
_LL2135; _LL2135: _temp2134=( void*) _temp2126.ret_typ; goto _LL2133; _LL2133:
_temp2132=( struct Cyc_List_List*) _temp2126.args; goto _LL2131; _LL2131:
_temp2130=( int) _temp2126.varargs; goto _LL2129; _LL2129: _temp2128=( struct
Cyc_List_List*) _temp2126.attributes; goto _LL2047;} else{ goto _LL2048;}
_LL2048: if(( unsigned int) _temp2028 > 4u?*(( int*) _temp2028) == Cyc_Absyn_TupleType:
0){ _LL2141: _temp2140=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp2028)->f1; goto _LL2049;} else{ goto _LL2050;} _LL2050: if(( unsigned int)
_temp2028 > 4u?*(( int*) _temp2028) == Cyc_Absyn_AnonStructType: 0){ _LL2143:
_temp2142=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp2028)->f1; goto _LL2051;} else{ goto _LL2052;} _LL2052: if(( unsigned int)
_temp2028 > 4u?*(( int*) _temp2028) == Cyc_Absyn_AnonUnionType: 0){ _LL2145:
_temp2144=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp2028)->f1; goto _LL2053;} else{ goto _LL2054;} _LL2054: if(( unsigned int)
_temp2028 > 4u?*(( int*) _temp2028) == Cyc_Absyn_Evar: 0){ _LL2151: _temp2150=(
void*)(( struct Cyc_Absyn_Evar_struct*) _temp2028)->f1; goto _LL2149; _LL2149:
_temp2148=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp2028)->f2;
goto _LL2147; _LL2147: _temp2146=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp2028)->f3; goto _LL2055;} else{ goto _LL2056;} _LL2056: if(( unsigned int)
_temp2028 > 4u?*(( int*) _temp2028) == Cyc_Absyn_RgnHandleType: 0){ _LL2153:
_temp2152=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp2028)->f1;
goto _LL2057;} else{ goto _LL2058;} _LL2058: if(( unsigned int) _temp2028 > 4u?*((
int*) _temp2028) == Cyc_Absyn_EnumType: 0){ _LL2157: _temp2156=( struct _tuple1*)((
struct Cyc_Absyn_EnumType_struct*) _temp2028)->f1; goto _LL2155; _LL2155:
_temp2154=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*)
_temp2028)->f2; goto _LL2059;} else{ goto _LL2060;} _LL2060: if( _temp2028 ==(
void*) Cyc_Absyn_VoidType){ goto _LL2061;} else{ goto _LL2062;} _LL2062: if((
unsigned int) _temp2028 > 4u?*(( int*) _temp2028) == Cyc_Absyn_IntType: 0){
_LL2161: _temp2160=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2028)->f1;
goto _LL2159; _LL2159: _temp2158=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2028)->f2; goto _LL2063;} else{ goto _LL2064;} _LL2064: if( _temp2028 ==(
void*) Cyc_Absyn_FloatType){ goto _LL2065;} else{ goto _LL2066;} _LL2066: if(
_temp2028 ==( void*) Cyc_Absyn_DoubleType){ goto _LL2067;} else{ goto _LL2068;}
_LL2068: if( _temp2028 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL2069;} else{ goto
_LL2070;} _LL2070: if(( unsigned int) _temp2028 > 4u?*(( int*) _temp2028) == Cyc_Absyn_AccessEff:
0){ _LL2163: _temp2162=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp2028)->f1;
goto _LL2071;} else{ goto _LL2072;} _LL2072: if(( unsigned int) _temp2028 > 4u?*((
int*) _temp2028) == Cyc_Absyn_JoinEff: 0){ _LL2165: _temp2164=( struct Cyc_List_List*)((
struct Cyc_Absyn_JoinEff_struct*) _temp2028)->f1; goto _LL2073;} else{ goto
_LL2029;} _LL2031: { struct _handler_cons _temp2166; _push_handler(& _temp2166);{
void* _temp2167=( void*) setjmp( _temp2166.handler); if( ! _temp2167){{ void*
_temp2168=(( void*(*)( int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_assoc_cmp)( Cyc_Absyn_tvar_cmp,
inst, _temp2074); _npop_handler( 0u); return _temp2168;}; _pop_handler();} else{
void* _temp2170= _temp2167; _LL2172: if( _temp2170 == Cyc_Core_Not_found){ goto
_LL2173;} else{ goto _LL2174;} _LL2174: goto _LL2175; _LL2173: return t; _LL2175:(
void) _throw( _temp2170); _LL2171:;}}} _LL2033: { struct Cyc_List_List* new_ts=
Cyc_Tcutil_substs( rgn, inst, _temp2078); return new_ts == _temp2078? t:( void*)({
struct Cyc_Absyn_StructType_struct* _temp2176=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp2176[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp2177; _temp2177.tag= Cyc_Absyn_StructType;
_temp2177.f1= _temp2080; _temp2177.f2= new_ts; _temp2177.f3= _temp2076;
_temp2177;}); _temp2176;});} _LL2035: { struct Cyc_List_List* new_ts= Cyc_Tcutil_substs(
rgn, inst, _temp2084); return new_ts == _temp2084? t:( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp2178=( struct Cyc_Absyn_UnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp2178[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp2179; _temp2179.tag= Cyc_Absyn_UnionType;
_temp2179.f1= _temp2086; _temp2179.f2= new_ts; _temp2179.f3= _temp2082;
_temp2179;}); _temp2178;});} _LL2037: { struct Cyc_List_List* new_ts= Cyc_Tcutil_substs(
rgn, inst, _temp2092); void* new_r= Cyc_Tcutil_rsubstitute( rgn, inst, _temp2090);
return( new_ts == _temp2092? new_r == _temp2090: 0)? t:( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp2180=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp2180[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp2181; _temp2181.tag=
Cyc_Absyn_TunionType; _temp2181.f1=({ struct Cyc_Absyn_TunionInfo _temp2182;
_temp2182.tunion_info=( void*) _temp2094; _temp2182.targs= new_ts; _temp2182.rgn=(
void*) new_r; _temp2182;}); _temp2181;}); _temp2180;});} _LL2039: { struct Cyc_List_List*
new_ts= Cyc_Tcutil_substs( rgn, inst, _temp2098); return new_ts == _temp2098? t:(
void*)({ struct Cyc_Absyn_TunionFieldType_struct* _temp2183=( struct Cyc_Absyn_TunionFieldType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct)); _temp2183[ 0]=({
struct Cyc_Absyn_TunionFieldType_struct _temp2184; _temp2184.tag= Cyc_Absyn_TunionFieldType;
_temp2184.f1=({ struct Cyc_Absyn_TunionFieldInfo _temp2185; _temp2185.field_info=(
void*) _temp2100; _temp2185.targs= new_ts; _temp2185;}); _temp2184;}); _temp2183;});}
_LL2041: { struct Cyc_List_List* new_ts= Cyc_Tcutil_substs( rgn, inst, _temp2104);
return new_ts == _temp2104? t:( void*)({ struct Cyc_Absyn_TypedefType_struct*
_temp2186=( struct Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp2186[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp2187; _temp2187.tag=
Cyc_Absyn_TypedefType; _temp2187.f1= _temp2106; _temp2187.f2= new_ts; _temp2187.f3=
_temp2102; _temp2187;}); _temp2186;});} _LL2043: { void* new_t1= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2112); return new_t1 == _temp2112? t:( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp2188=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp2188[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp2189; _temp2189.tag= Cyc_Absyn_ArrayType;
_temp2189.f1=( void*) new_t1; _temp2189.f2= _temp2110; _temp2189.f3= _temp2108;
_temp2189;}); _temp2188;});} _LL2045: { void* new_t1= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2124); void* new_r= Cyc_Tcutil_rsubstitute( rgn, inst, _temp2122);
if( new_t1 == _temp2124? new_r == _temp2122: 0){ return t;} return( void*)({
struct Cyc_Absyn_PointerType_struct* _temp2190=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp2190[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp2191; _temp2191.tag= Cyc_Absyn_PointerType;
_temp2191.f1=({ struct Cyc_Absyn_PtrInfo _temp2192; _temp2192.elt_typ=( void*)
new_t1; _temp2192.rgn_typ=( void*) new_r; _temp2192.nullable= _temp2120;
_temp2192.tq= _temp2118; _temp2192.bounds= _temp2116; _temp2192;}); _temp2191;});
_temp2190;});} _LL2047:{ struct Cyc_List_List* p= _temp2138; for( 0; p != 0; p=({
struct Cyc_List_List* _temp2193= p; if( _temp2193 == 0){ _throw( Null_Exception);}
_temp2193->tl;})){ inst=({ struct Cyc_List_List* _temp2194=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2194->hd=( void*)({
struct _tuple5* _temp2195=( struct _tuple5*) _region_malloc( rgn, sizeof( struct
_tuple5)); _temp2195->f1=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp2199= p; if( _temp2199 == 0){ _throw( Null_Exception);} _temp2199->hd;});
_temp2195->f2=( void*)({ struct Cyc_Absyn_VarType_struct* _temp2196=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp2196[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp2197; _temp2197.tag= Cyc_Absyn_VarType; _temp2197.f1=(
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp2198= p; if( _temp2198 == 0){
_throw( Null_Exception);} _temp2198->hd;}); _temp2197;}); _temp2196;});
_temp2195;}); _temp2194->tl= inst; _temp2194;});}}{ struct Cyc_List_List*
_temp2202; struct Cyc_List_List* _temp2204; struct _tuple0 _temp2200=(( struct
_tuple0(*)( struct _RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List*
x)) Cyc_List_rsplit)( rgn, rgn,(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct _tuple14*(* f)( struct _RegionHandle*, struct _tuple2*), struct
_RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_substitute_f1,
rgn, _temp2132)); _LL2205: _temp2204= _temp2200.f1; goto _LL2203; _LL2203:
_temp2202= _temp2200.f2; goto _LL2201; _LL2201: { struct Cyc_List_List* ts2= Cyc_Tcutil_substs(
rgn, inst, _temp2202); struct Cyc_List_List* args2=(( struct Cyc_List_List*(*)(
struct _tuple2*(* f)( struct _tuple14*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcutil_substitute_f2,(( struct Cyc_List_List*(*)( struct _RegionHandle* r3,
struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, _temp2204, ts2)); struct Cyc_Core_Opt* eff2; if( _temp2136 == 0){ eff2=
0;} else{ void* new_eff= Cyc_Tcutil_rsubstitute( rgn, inst,( void*)({ struct Cyc_Core_Opt*
_temp2208= _temp2136; if( _temp2208 == 0){ _throw( Null_Exception);} _temp2208->v;}));
if( new_eff ==( void*)({ struct Cyc_Core_Opt* _temp2206= _temp2136; if(
_temp2206 == 0){ _throw( Null_Exception);} _temp2206->v;})){ eff2= _temp2136;}
else{ eff2=({ struct Cyc_Core_Opt* _temp2207=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp2207->v=( void*) new_eff; _temp2207;});}}
return( void*)({ struct Cyc_Absyn_FnType_struct* _temp2209=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp2209[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp2210; _temp2210.tag= Cyc_Absyn_FnType; _temp2210.f1=({ struct Cyc_Absyn_FnInfo
_temp2211; _temp2211.tvars= _temp2138; _temp2211.effect= eff2; _temp2211.ret_typ=(
void*) Cyc_Tcutil_rsubstitute( rgn, inst, _temp2134); _temp2211.args= args2;
_temp2211.varargs= _temp2130; _temp2211.attributes= _temp2128; _temp2211;});
_temp2210;}); _temp2209;});}} _LL2049: { struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, void*(* f)( struct _tuple4*), struct Cyc_List_List* x))
Cyc_List_rmap)( rgn, Cyc_Tcutil_snd_tqt, _temp2140); struct Cyc_List_List* ts2=
Cyc_Tcutil_substs( rgn, inst, ts); if( ts2 == ts){ return t;}{ struct Cyc_List_List*
tqts2=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct _tuple4*, void*),
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_map2)( Cyc_Tcutil_map2_tq,
_temp2140, ts2); return( void*)({ struct Cyc_Absyn_TupleType_struct* _temp2212=(
struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp2212[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp2213; _temp2213.tag= Cyc_Absyn_TupleType;
_temp2213.f1= tqts2; _temp2213;}); _temp2212;});}} _LL2051: { struct Cyc_List_List*
ts=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct Cyc_Absyn_Structfield*),
struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type, _temp2142);
struct Cyc_List_List* ts2= Cyc_Tcutil_substs( rgn, inst, ts); if( ts2 == ts){
return t;}{ struct Cyc_List_List* fs2=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type, _temp2142, ts2); return( void*)({
struct Cyc_Absyn_AnonStructType_struct* _temp2214=( struct Cyc_Absyn_AnonStructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AnonStructType_struct)); _temp2214[ 0]=({
struct Cyc_Absyn_AnonStructType_struct _temp2215; _temp2215.tag= Cyc_Absyn_AnonStructType;
_temp2215.f1= fs2; _temp2215;}); _temp2214;});}} _LL2053: { struct Cyc_List_List*
ts=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct Cyc_Absyn_Structfield*),
struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type, _temp2144);
struct Cyc_List_List* ts2= Cyc_Tcutil_substs( rgn, inst, ts); if( ts2 == ts){
return t;}{ struct Cyc_List_List* fs2=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type, _temp2144, ts2); return( void*)({
struct Cyc_Absyn_AnonStructType_struct* _temp2216=( struct Cyc_Absyn_AnonStructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AnonStructType_struct)); _temp2216[ 0]=({
struct Cyc_Absyn_AnonStructType_struct _temp2217; _temp2217.tag= Cyc_Absyn_AnonStructType;
_temp2217.f1= fs2; _temp2217;}); _temp2216;});}} _LL2055: if( _temp2148 != 0){
return Cyc_Tcutil_rsubstitute( rgn, inst,( void*)({ struct Cyc_Core_Opt*
_temp2218= _temp2148; if( _temp2218 == 0){ _throw( Null_Exception);} _temp2218->v;}));}
else{ return t;} _LL2057: { void* new_rt= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2152); return new_rt == _temp2152? t:( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp2219=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp2219[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp2220; _temp2220.tag= Cyc_Absyn_RgnHandleType; _temp2220.f1=( void*) new_rt;
_temp2220;}); _temp2219;});} _LL2059: return t; _LL2061: return t; _LL2063:
return t; _LL2065: return t; _LL2067: return t; _LL2069: return t; _LL2071: {
void* new_r= Cyc_Tcutil_rsubstitute( rgn, inst, _temp2162); return new_r ==
_temp2162? t:( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2221=( struct
Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2221[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2222; _temp2222.tag= Cyc_Absyn_AccessEff;
_temp2222.f1=( void*) new_r; _temp2222;}); _temp2221;});} _LL2073: { struct Cyc_List_List*
new_es= Cyc_Tcutil_substs( rgn, inst, _temp2164); return new_es == _temp2164? t:(
void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2223=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp2223[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp2224; _temp2224.tag= Cyc_Absyn_JoinEff; _temp2224.f1=
new_es; _temp2224;}); _temp2223;});} _LL2029:;} static struct Cyc_List_List* Cyc_Tcutil_substs(
struct _RegionHandle* rgn, struct Cyc_List_List* inst, struct Cyc_List_List* ts){
if( ts == 0){ return 0;}{ void* old_hd=( void*)({ struct Cyc_List_List*
_temp2227= ts; if( _temp2227 == 0){ _throw( Null_Exception);} _temp2227->hd;});
struct Cyc_List_List* old_tl=({ struct Cyc_List_List* _temp2226= ts; if(
_temp2226 == 0){ _throw( Null_Exception);} _temp2226->tl;}); void* new_hd= Cyc_Tcutil_rsubstitute(
rgn, inst, old_hd); struct Cyc_List_List* new_tl= Cyc_Tcutil_substs( rgn, inst,
old_tl); if( old_hd == new_hd? old_tl == new_tl: 0){ return ts;} return( struct
Cyc_List_List*)({ struct Cyc_List_List* _temp2225=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2225->hd=( void*) new_hd;
_temp2225->tl= new_tl; _temp2225;});}} extern void* Cyc_Tcutil_substitute(
struct Cyc_List_List* inst, void* t){ return Cyc_Tcutil_rsubstitute( Cyc_Core_heap_region,
inst, t);} struct Cyc_Absyn_Exp* Cyc_Tcutil_default_initializer( struct Cyc_Tcenv_Tenv*
te, void* t, struct Cyc_Position_Segment* loc){ struct Cyc_Absyn_Exp* e= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp2277=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2277[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2278; _temp2278.tag= Cyc_Absyn_Const_e; _temp2278.f1=(
void*)(( void*) Cyc_Absyn_Null_c); _temp2278;}); _temp2277;}), loc);{ void*
_temp2228= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp2242; void*
_temp2244; void* _temp2246; void* _temp2248; void* _temp2250; _LL2230: if((
unsigned int) _temp2228 > 4u?*(( int*) _temp2228) == Cyc_Absyn_PointerType: 0){
_LL2243: _temp2242=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp2228)->f1; goto _LL2231;} else{ goto _LL2232;} _LL2232: if(( unsigned int)
_temp2228 > 4u?*(( int*) _temp2228) == Cyc_Absyn_IntType: 0){ _LL2247: _temp2246=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp2228)->f1; goto _LL2245; _LL2245:
_temp2244=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2228)->f2; if(
_temp2244 ==( void*) Cyc_Absyn_B1){ goto _LL2233;} else{ goto _LL2234;}} else{
goto _LL2234;} _LL2234: if(( unsigned int) _temp2228 > 4u?*(( int*) _temp2228)
== Cyc_Absyn_IntType: 0){ _LL2251: _temp2250=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2228)->f1; goto _LL2249; _LL2249: _temp2248=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2228)->f2; goto _LL2235;} else{ goto _LL2236;} _LL2236: if( _temp2228 ==(
void*) Cyc_Absyn_FloatType){ goto _LL2237;} else{ goto _LL2238;} _LL2238: if(
_temp2228 ==( void*) Cyc_Absyn_DoubleType){ goto _LL2239;} else{ goto _LL2240;}
_LL2240: goto _LL2241; _LL2231: goto _LL2229; _LL2233:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp2252=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2252[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2253; _temp2253.tag= Cyc_Absyn_Const_e; _temp2253.f1=(
void*)(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp2254=( struct Cyc_Absyn_Char_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct)); _temp2254[ 0]=({ struct Cyc_Absyn_Char_c_struct
_temp2255; _temp2255.tag= Cyc_Absyn_Char_c; _temp2255.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp2255.f2='\000'; _temp2255;}); _temp2254;})); _temp2253;}); _temp2252;})));
goto _LL2229; _LL2235:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2256=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2256[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2257; _temp2257.tag= Cyc_Absyn_Const_e;
_temp2257.f1=( void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp2258=(
struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp2258[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp2259; _temp2259.tag= Cyc_Absyn_Int_c;
_temp2259.f1=( void*) _temp2250; _temp2259.f2= 0; _temp2259;}); _temp2258;}));
_temp2257;}); _temp2256;}))); if( _temp2248 !=( void*) Cyc_Absyn_B4){ e= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Cast_e_struct* _temp2260=( struct Cyc_Absyn_Cast_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp2260[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp2261; _temp2261.tag= Cyc_Absyn_Cast_e; _temp2261.f1=( void*) t; _temp2261.f2=
e; _temp2261;}); _temp2260;}), loc);} goto _LL2229; _LL2237:( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp2262=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2262[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2263; _temp2263.tag= Cyc_Absyn_Const_e; _temp2263.f1=(
void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp2264=( struct Cyc_Absyn_Float_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Float_c_struct)); _temp2264[ 0]=({ struct
Cyc_Absyn_Float_c_struct _temp2265; _temp2265.tag= Cyc_Absyn_Float_c; _temp2265.f1=(
struct _tagged_string)({ char* _temp2266=( char*)"0.0"; struct _tagged_string
_temp2267; _temp2267.curr= _temp2266; _temp2267.base= _temp2266; _temp2267.last_plus_one=
_temp2266 + 4; _temp2267;}); _temp2265;}); _temp2264;})); _temp2263;});
_temp2262;}))); goto _LL2229; _LL2239:( void*)( e->r=( void*)(( void*)({ struct
Cyc_Absyn_Cast_e_struct* _temp2268=( struct Cyc_Absyn_Cast_e_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp2268[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp2269; _temp2269.tag= Cyc_Absyn_Cast_e; _temp2269.f1=( void*) t; _temp2269.f2=
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp2270=( struct
Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2270[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2271; _temp2271.tag= Cyc_Absyn_Const_e;
_temp2271.f1=( void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp2272=(
struct Cyc_Absyn_Float_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Float_c_struct));
_temp2272[ 0]=({ struct Cyc_Absyn_Float_c_struct _temp2273; _temp2273.tag= Cyc_Absyn_Float_c;
_temp2273.f1=( struct _tagged_string)({ char* _temp2274=( char*)"0.0"; struct
_tagged_string _temp2275; _temp2275.curr= _temp2274; _temp2275.base= _temp2274;
_temp2275.last_plus_one= _temp2274 + 4; _temp2275;}); _temp2273;}); _temp2272;}));
_temp2271;}); _temp2270;}), loc); _temp2269;}); _temp2268;}))); goto _LL2229;
_LL2241: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2276= Cyc_Absynpp_typ2string(
t); xprintf("declaration of type %.*s requires initializer", _temp2276.last_plus_one
- _temp2276.curr, _temp2276.curr);})); goto _LL2229; _LL2229:;} return e;}
struct _tuple5* Cyc_Tcutil_make_inst_var( struct Cyc_Absyn_Tvar* tv){ void* k=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( tv->kind); return({
struct _tuple5* _temp2279=( struct _tuple5*) GC_malloc( sizeof( struct _tuple5));
_temp2279->f1= tv; _temp2279->f2= Cyc_Absyn_new_evar( k); _temp2279;});} struct
_tuple5* Cyc_Tcutil_r_make_inst_var( struct _RegionHandle* rgn, struct Cyc_Absyn_Tvar*
tv){ void* k=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( tv->kind);
return({ struct _tuple5* _temp2280=( struct _tuple5*) _region_malloc( rgn,
sizeof( struct _tuple5)); _temp2280->f1= tv; _temp2280->f2= Cyc_Absyn_new_evar(
k); _temp2280;});} struct Cyc_List_List* Cyc_Tcutil_check_valid_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* free_vars, void*
expected_kind, void* t){{ void* _temp2281= Cyc_Tcutil_compress( t); int
_temp2327; struct Cyc_Core_Opt* _temp2329; void* _temp2331; struct Cyc_Absyn_Tvar*
_temp2333; struct Cyc_Absyn_Enumdecl* _temp2335; struct Cyc_Absyn_Enumdecl**
_temp2337; struct _tuple1* _temp2338; struct Cyc_Absyn_TunionInfo _temp2340;
void* _temp2342; struct Cyc_List_List* _temp2344; void* _temp2346; void**
_temp2348; struct Cyc_Absyn_TunionFieldInfo _temp2349; struct Cyc_List_List*
_temp2351; void* _temp2353; void** _temp2355; struct Cyc_Absyn_PtrInfo _temp2356;
struct Cyc_Absyn_Conref* _temp2358; struct Cyc_Absyn_Tqual _temp2360; struct Cyc_Absyn_Conref*
_temp2362; void* _temp2364; void* _temp2366; void* _temp2368; void* _temp2370;
struct Cyc_Absyn_Exp* _temp2372; struct Cyc_Absyn_Tqual _temp2374; void*
_temp2376; struct Cyc_Absyn_FnInfo _temp2378; struct Cyc_List_List* _temp2380;
int _temp2382; struct Cyc_List_List* _temp2384; void* _temp2386; struct Cyc_Core_Opt*
_temp2388; struct Cyc_Core_Opt** _temp2390; struct Cyc_List_List* _temp2391;
struct Cyc_List_List** _temp2393; struct Cyc_List_List* _temp2394; struct Cyc_List_List*
_temp2396; struct Cyc_List_List* _temp2398; struct Cyc_Absyn_Structdecl**
_temp2400; struct Cyc_Absyn_Structdecl*** _temp2402; struct Cyc_List_List*
_temp2403; struct _tuple1* _temp2405; struct Cyc_Absyn_Uniondecl** _temp2407;
struct Cyc_Absyn_Uniondecl*** _temp2409; struct Cyc_List_List* _temp2410; struct
_tuple1* _temp2412; struct Cyc_Core_Opt* _temp2414; struct Cyc_Core_Opt**
_temp2416; struct Cyc_List_List* _temp2417; struct _tuple1* _temp2419; void*
_temp2421; void* _temp2423; struct Cyc_List_List* _temp2425; _LL2283: if(
_temp2281 ==( void*) Cyc_Absyn_VoidType){ goto _LL2284;} else{ goto _LL2285;}
_LL2285: if(( unsigned int) _temp2281 > 4u?*(( int*) _temp2281) == Cyc_Absyn_Evar:
0){ _LL2332: _temp2331=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp2281)->f1;
goto _LL2330; _LL2330: _temp2329=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp2281)->f2; goto _LL2328; _LL2328: _temp2327=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp2281)->f3; goto _LL2286;} else{ goto _LL2287;} _LL2287: if(( unsigned int)
_temp2281 > 4u?*(( int*) _temp2281) == Cyc_Absyn_VarType: 0){ _LL2334: _temp2333=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*) _temp2281)->f1; goto
_LL2288;} else{ goto _LL2289;} _LL2289: if(( unsigned int) _temp2281 > 4u?*((
int*) _temp2281) == Cyc_Absyn_EnumType: 0){ _LL2339: _temp2338=( struct _tuple1*)((
struct Cyc_Absyn_EnumType_struct*) _temp2281)->f1; goto _LL2336; _LL2336:
_temp2335=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*)
_temp2281)->f2; _temp2337=&(( struct Cyc_Absyn_EnumType_struct*) _temp2281)->f2;
goto _LL2290;} else{ goto _LL2291;} _LL2291: if(( unsigned int) _temp2281 > 4u?*((
int*) _temp2281) == Cyc_Absyn_TunionType: 0){ _LL2341: _temp2340=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp2281)->f1; _LL2347: _temp2346=( void*)
_temp2340.tunion_info; _temp2348=&((( struct Cyc_Absyn_TunionType_struct*)
_temp2281)->f1).tunion_info; goto _LL2345; _LL2345: _temp2344=( struct Cyc_List_List*)
_temp2340.targs; goto _LL2343; _LL2343: _temp2342=( void*) _temp2340.rgn; goto
_LL2292;} else{ goto _LL2293;} _LL2293: if(( unsigned int) _temp2281 > 4u?*((
int*) _temp2281) == Cyc_Absyn_TunionFieldType: 0){ _LL2350: _temp2349=( struct
Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*) _temp2281)->f1;
_LL2354: _temp2353=( void*) _temp2349.field_info; _temp2355=&((( struct Cyc_Absyn_TunionFieldType_struct*)
_temp2281)->f1).field_info; goto _LL2352; _LL2352: _temp2351=( struct Cyc_List_List*)
_temp2349.targs; goto _LL2294;} else{ goto _LL2295;} _LL2295: if(( unsigned int)
_temp2281 > 4u?*(( int*) _temp2281) == Cyc_Absyn_PointerType: 0){ _LL2357:
_temp2356=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp2281)->f1; _LL2367: _temp2366=( void*) _temp2356.elt_typ; goto _LL2365;
_LL2365: _temp2364=( void*) _temp2356.rgn_typ; goto _LL2363; _LL2363: _temp2362=(
struct Cyc_Absyn_Conref*) _temp2356.nullable; goto _LL2361; _LL2361: _temp2360=(
struct Cyc_Absyn_Tqual) _temp2356.tq; goto _LL2359; _LL2359: _temp2358=( struct
Cyc_Absyn_Conref*) _temp2356.bounds; goto _LL2296;} else{ goto _LL2297;} _LL2297:
if(( unsigned int) _temp2281 > 4u?*(( int*) _temp2281) == Cyc_Absyn_IntType: 0){
_LL2371: _temp2370=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2281)->f1;
goto _LL2369; _LL2369: _temp2368=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2281)->f2; goto _LL2298;} else{ goto _LL2299;} _LL2299: if( _temp2281 ==(
void*) Cyc_Absyn_FloatType){ goto _LL2300;} else{ goto _LL2301;} _LL2301: if(
_temp2281 ==( void*) Cyc_Absyn_DoubleType){ goto _LL2302;} else{ goto _LL2303;}
_LL2303: if(( unsigned int) _temp2281 > 4u?*(( int*) _temp2281) == Cyc_Absyn_ArrayType:
0){ _LL2377: _temp2376=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2281)->f1;
goto _LL2375; _LL2375: _temp2374=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2281)->f2; goto _LL2373; _LL2373: _temp2372=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2281)->f3; goto _LL2304;} else{ goto
_LL2305;} _LL2305: if(( unsigned int) _temp2281 > 4u?*(( int*) _temp2281) == Cyc_Absyn_FnType:
0){ _LL2379: _temp2378=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp2281)->f1; _LL2392: _temp2391=( struct Cyc_List_List*) _temp2378.tvars;
_temp2393=&((( struct Cyc_Absyn_FnType_struct*) _temp2281)->f1).tvars; goto
_LL2389; _LL2389: _temp2388=( struct Cyc_Core_Opt*) _temp2378.effect; _temp2390=&(((
struct Cyc_Absyn_FnType_struct*) _temp2281)->f1).effect; goto _LL2387; _LL2387:
_temp2386=( void*) _temp2378.ret_typ; goto _LL2385; _LL2385: _temp2384=( struct
Cyc_List_List*) _temp2378.args; goto _LL2383; _LL2383: _temp2382=( int)
_temp2378.varargs; goto _LL2381; _LL2381: _temp2380=( struct Cyc_List_List*)
_temp2378.attributes; goto _LL2306;} else{ goto _LL2307;} _LL2307: if((
unsigned int) _temp2281 > 4u?*(( int*) _temp2281) == Cyc_Absyn_TupleType: 0){
_LL2395: _temp2394=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp2281)->f1; goto _LL2308;} else{ goto _LL2309;} _LL2309: if(( unsigned int)
_temp2281 > 4u?*(( int*) _temp2281) == Cyc_Absyn_AnonStructType: 0){ _LL2397:
_temp2396=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp2281)->f1; goto _LL2310;} else{ goto _LL2311;} _LL2311: if(( unsigned int)
_temp2281 > 4u?*(( int*) _temp2281) == Cyc_Absyn_AnonUnionType: 0){ _LL2399:
_temp2398=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp2281)->f1; goto _LL2312;} else{ goto _LL2313;} _LL2313: if(( unsigned int)
_temp2281 > 4u?*(( int*) _temp2281) == Cyc_Absyn_StructType: 0){ _LL2406:
_temp2405=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp2281)->f1;
goto _LL2404; _LL2404: _temp2403=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp2281)->f2; goto _LL2401; _LL2401: _temp2400=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp2281)->f3; _temp2402=&(( struct Cyc_Absyn_StructType_struct*)
_temp2281)->f3; goto _LL2314;} else{ goto _LL2315;} _LL2315: if(( unsigned int)
_temp2281 > 4u?*(( int*) _temp2281) == Cyc_Absyn_UnionType: 0){ _LL2413:
_temp2412=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*) _temp2281)->f1;
goto _LL2411; _LL2411: _temp2410=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp2281)->f2; goto _LL2408; _LL2408: _temp2407=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp2281)->f3; _temp2409=&(( struct Cyc_Absyn_UnionType_struct*)
_temp2281)->f3; goto _LL2316;} else{ goto _LL2317;} _LL2317: if(( unsigned int)
_temp2281 > 4u?*(( int*) _temp2281) == Cyc_Absyn_TypedefType: 0){ _LL2420:
_temp2419=( struct _tuple1*)(( struct Cyc_Absyn_TypedefType_struct*) _temp2281)->f1;
goto _LL2418; _LL2418: _temp2417=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp2281)->f2; goto _LL2415; _LL2415: _temp2414=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_TypedefType_struct*) _temp2281)->f3; _temp2416=&(( struct Cyc_Absyn_TypedefType_struct*)
_temp2281)->f3; goto _LL2318;} else{ goto _LL2319;} _LL2319: if( _temp2281 ==(
void*) Cyc_Absyn_HeapRgn){ goto _LL2320;} else{ goto _LL2321;} _LL2321: if((
unsigned int) _temp2281 > 4u?*(( int*) _temp2281) == Cyc_Absyn_RgnHandleType: 0){
_LL2422: _temp2421=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp2281)->f1;
goto _LL2322;} else{ goto _LL2323;} _LL2323: if(( unsigned int) _temp2281 > 4u?*((
int*) _temp2281) == Cyc_Absyn_AccessEff: 0){ _LL2424: _temp2423=( void*)((
struct Cyc_Absyn_AccessEff_struct*) _temp2281)->f1; goto _LL2324;} else{ goto
_LL2325;} _LL2325: if(( unsigned int) _temp2281 > 4u?*(( int*) _temp2281) == Cyc_Absyn_JoinEff:
0){ _LL2426: _temp2425=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*)
_temp2281)->f1; goto _LL2326;} else{ goto _LL2282;} _LL2284: goto _LL2282;
_LL2286: goto _LL2282; _LL2288: { struct Cyc_Absyn_Conref* c=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp2333->kind); if((
void*) c->v ==( void*) Cyc_Absyn_No_constr){( void*)( c->v=( void*)(( void*)({
struct Cyc_Absyn_Eq_constr_struct* _temp2427=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp2427[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp2428; _temp2428.tag= Cyc_Absyn_Eq_constr;
_temp2428.f1=( void*) expected_kind; _temp2428;}); _temp2427;})));} free_vars=
Cyc_Tcutil_add_free_tvar( loc, free_vars, _temp2333); goto _LL2282;} _LL2290:
if(* _temp2337 == 0){ struct _handler_cons _temp2429; _push_handler(& _temp2429);{
void* _temp2430=( void*) setjmp( _temp2429.handler); if( ! _temp2430){{ struct
Cyc_Absyn_Enumdecl** ed= Cyc_Tcenv_lookup_enumdecl( te, loc, _temp2338);*
_temp2337=( struct Cyc_Absyn_Enumdecl*)* ed;}; _pop_handler();} else{ void*
_temp2432= _temp2430; _LL2434: if( _temp2432 == Cyc_Dict_Absent){ goto _LL2435;}
else{ goto _LL2436;} _LL2436: goto _LL2437; _LL2435: Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2438= Cyc_Absynpp_qvar2string( _temp2338); xprintf("unbound type enum %.*s",
_temp2438.last_plus_one - _temp2438.curr, _temp2438.curr);})); return free_vars;
_LL2437:( void) _throw( _temp2432); _LL2433:;}}}{ struct Cyc_Absyn_Enumdecl* ed=(
struct Cyc_Absyn_Enumdecl*)({ struct Cyc_Absyn_Enumdecl* _temp2441=* _temp2337;
if( _temp2441 == 0){ _throw( Null_Exception);} _temp2441;});* _temp2338=({
struct _tuple1* _temp2439= ed->name; unsigned int _temp2440= 0; if( _temp2440 >=
1u){ _throw( Null_Exception);} _temp2439[ _temp2440];}); goto _LL2282;} _LL2292:{
void* _temp2442=* _temp2348; struct Cyc_Absyn_UnknownTunionInfo _temp2448; int
_temp2450; struct _tuple1* _temp2452; struct Cyc_Absyn_Tuniondecl* _temp2454;
_LL2444: if(*(( int*) _temp2442) == Cyc_Absyn_UnknownTunion){ _LL2449: _temp2448=(
struct Cyc_Absyn_UnknownTunionInfo)(( struct Cyc_Absyn_UnknownTunion_struct*)
_temp2442)->f1; _LL2453: _temp2452=( struct _tuple1*) _temp2448.name; goto
_LL2451; _LL2451: _temp2450=( int) _temp2448.is_xtunion; goto _LL2445;} else{
goto _LL2446;} _LL2446: if(*(( int*) _temp2442) == Cyc_Absyn_KnownTunion){
_LL2455: _temp2454=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_KnownTunion_struct*)
_temp2442)->f1; goto _LL2447;} else{ goto _LL2443;} _LL2445: { struct Cyc_Absyn_Tuniondecl*
tud;{ struct _handler_cons _temp2456; _push_handler(& _temp2456);{ void*
_temp2457=( void*) setjmp( _temp2456.handler); if( ! _temp2457){ tud=* Cyc_Tcenv_lookup_tuniondecl(
te, loc, _temp2452);; _pop_handler();} else{ void* _temp2459= _temp2457; _LL2461:
if( _temp2459 == Cyc_Dict_Absent){ goto _LL2462;} else{ goto _LL2463;} _LL2463:
goto _LL2464; _LL2462: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2465=
Cyc_Absynpp_qvar2string( _temp2452); xprintf("unbound type tunion %.*s",
_temp2465.last_plus_one - _temp2465.curr, _temp2465.curr);})); return free_vars;
_LL2464:( void) _throw( _temp2459); _LL2460:;}}} if( tud->is_xtunion !=
_temp2450){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2466= Cyc_Absynpp_qvar2string(
_temp2452); xprintf("[x]tunion is different from type declaration %.*s",
_temp2466.last_plus_one - _temp2466.curr, _temp2466.curr);}));}* _temp2348=(
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp2467=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp2467[ 0]=({
struct Cyc_Absyn_KnownTunion_struct _temp2468; _temp2468.tag= Cyc_Absyn_KnownTunion;
_temp2468.f1= tud; _temp2468;}); _temp2467;}); _temp2454= tud; goto _LL2447;}
_LL2447: free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars,( void*) Cyc_Absyn_RgnKind,
_temp2342);{ struct Cyc_List_List* tvs= _temp2454->tvs; for( 0; _temp2344 != 0?
tvs != 0: 0; _temp2344=({ struct Cyc_List_List* _temp2469= _temp2344; if(
_temp2469 == 0){ _throw( Null_Exception);} _temp2469->tl;}), tvs=({ struct Cyc_List_List*
_temp2470= tvs; if( _temp2470 == 0){ _throw( Null_Exception);} _temp2470->tl;})){
void* t1=( void*)({ struct Cyc_List_List* _temp2472= _temp2344; if( _temp2472 ==
0){ _throw( Null_Exception);} _temp2472->hd;}); void* k1=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp2471= tvs; if( _temp2471 == 0){ _throw( Null_Exception);} _temp2471->hd;}))->kind);
free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars, k1, t1);} if(
_temp2344 != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2473= Cyc_Absynpp_qvar2string(
_temp2454->name); xprintf("too many type arguments for tunion %.*s", _temp2473.last_plus_one
- _temp2473.curr, _temp2473.curr);}));} if( tvs != 0){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2474= Cyc_Absynpp_qvar2string( _temp2454->name);
xprintf("too few type arguments for tunion %.*s", _temp2474.last_plus_one -
_temp2474.curr, _temp2474.curr);}));} goto _LL2443;} _LL2443:;} goto _LL2282;
_LL2294:{ void* _temp2475=* _temp2355; struct Cyc_Absyn_UnknownTunionFieldInfo
_temp2481; int _temp2483; struct _tuple1* _temp2485; struct _tuple1* _temp2487;
struct Cyc_Absyn_Tunionfield* _temp2489; struct Cyc_Absyn_Tuniondecl* _temp2491;
_LL2477: if(*(( int*) _temp2475) == Cyc_Absyn_UnknownTunionfield){ _LL2482:
_temp2481=( struct Cyc_Absyn_UnknownTunionFieldInfo)(( struct Cyc_Absyn_UnknownTunionfield_struct*)
_temp2475)->f1; _LL2488: _temp2487=( struct _tuple1*) _temp2481.tunion_name;
goto _LL2486; _LL2486: _temp2485=( struct _tuple1*) _temp2481.field_name; goto
_LL2484; _LL2484: _temp2483=( int) _temp2481.is_xtunion; goto _LL2478;} else{
goto _LL2479;} _LL2479: if(*(( int*) _temp2475) == Cyc_Absyn_KnownTunionfield){
_LL2492: _temp2491=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp2475)->f1; goto _LL2490; _LL2490: _temp2489=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp2475)->f2; goto _LL2480;} else{
goto _LL2476;} _LL2478: { struct Cyc_Absyn_Tuniondecl* tud; struct Cyc_Absyn_Tunionfield*
tuf;{ struct _handler_cons _temp2493; _push_handler(& _temp2493);{ void*
_temp2494=( void*) setjmp( _temp2493.handler); if( ! _temp2494){ tud=* Cyc_Tcenv_lookup_tuniondecl(
te, loc, _temp2487);; _pop_handler();} else{ void* _temp2496= _temp2494; _LL2498:
if( _temp2496 == Cyc_Dict_Absent){ goto _LL2499;} else{ goto _LL2500;} _LL2500:
goto _LL2501; _LL2499: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2502=
Cyc_Absynpp_qvar2string( _temp2487); xprintf("unbound type tunion %.*s",
_temp2502.last_plus_one - _temp2502.curr, _temp2502.curr);})); return free_vars;
_LL2501:( void) _throw( _temp2496); _LL2497:;}}} if( tud->is_xtunion !=
_temp2483){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2503= Cyc_Absynpp_qvar2string(
_temp2487); xprintf("[x]tunion is different from type declaration %.*s",
_temp2503.last_plus_one - _temp2503.curr, _temp2503.curr);}));}{ struct
_handler_cons _temp2504; _push_handler(& _temp2504);{ void* _temp2505=( void*)
setjmp( _temp2504.handler); if( ! _temp2505){{ void* _temp2506= Cyc_Tcenv_lookup_ordinary(
te, loc, _temp2485); struct Cyc_Absyn_Tunionfield* _temp2512; struct Cyc_Absyn_Tuniondecl*
_temp2514; _LL2508: if(*(( int*) _temp2506) == Cyc_Tcenv_TunionRes){ _LL2515:
_temp2514=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Tcenv_TunionRes_struct*)
_temp2506)->f1; goto _LL2513; _LL2513: _temp2512=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Tcenv_TunionRes_struct*) _temp2506)->f2; goto _LL2509;} else{ goto
_LL2510;} _LL2510: goto _LL2511; _LL2509: tuf= _temp2512; if( _temp2514 != tud){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2516= Cyc_Absynpp_qvar2string(
_temp2485); struct _tagged_string _temp2517= Cyc_Absynpp_qvar2string( _temp2487);
xprintf("unbound field %.*s in type tunion %.*s", _temp2516.last_plus_one -
_temp2516.curr, _temp2516.curr, _temp2517.last_plus_one - _temp2517.curr,
_temp2517.curr);}));{ struct Cyc_List_List* _temp2518= free_vars; _npop_handler(
0u); return _temp2518;}} goto _LL2507; _LL2511: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2519= Cyc_Absynpp_qvar2string( _temp2485); struct
_tagged_string _temp2520= Cyc_Absynpp_qvar2string( _temp2487); xprintf("unbound field %.*s in type tunion %.*s",
_temp2519.last_plus_one - _temp2519.curr, _temp2519.curr, _temp2520.last_plus_one
- _temp2520.curr, _temp2520.curr);}));{ struct Cyc_List_List* _temp2521=
free_vars; _npop_handler( 0u); return _temp2521;} _LL2507:;}; _pop_handler();}
else{ void* _temp2523= _temp2505; _LL2525: if( _temp2523 == Cyc_Dict_Absent){
goto _LL2526;} else{ goto _LL2527;} _LL2527: goto _LL2528; _LL2526: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2529= Cyc_Absynpp_qvar2string( _temp2485);
struct _tagged_string _temp2530= Cyc_Absynpp_qvar2string( _temp2487); xprintf("unbound field %.*s in type tunion %.*s",
_temp2529.last_plus_one - _temp2529.curr, _temp2529.curr, _temp2530.last_plus_one
- _temp2530.curr, _temp2530.curr);})); return free_vars; _LL2528:( void) _throw(
_temp2523); _LL2524:;}}}* _temp2355=( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp2531=( struct Cyc_Absyn_KnownTunionfield_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp2531[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp2532; _temp2532.tag= Cyc_Absyn_KnownTunionfield; _temp2532.f1= tud;
_temp2532.f2= tuf; _temp2532;}); _temp2531;}); _temp2491= tud; _temp2489= tuf;
goto _LL2480;} _LL2480: { struct Cyc_List_List* tvs= _temp2491->tvs; for( 0;
_temp2351 != 0? tvs != 0: 0; _temp2351=({ struct Cyc_List_List* _temp2533=
_temp2351; if( _temp2533 == 0){ _throw( Null_Exception);} _temp2533->tl;}), tvs=({
struct Cyc_List_List* _temp2534= tvs; if( _temp2534 == 0){ _throw(
Null_Exception);} _temp2534->tl;})){ void* t1=( void*)({ struct Cyc_List_List*
_temp2536= _temp2351; if( _temp2536 == 0){ _throw( Null_Exception);} _temp2536->hd;});
void* k1=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(((
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp2535= tvs; if( _temp2535 ==
0){ _throw( Null_Exception);} _temp2535->hd;}))->kind); free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars, k1, t1);} if( _temp2351 != 0){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2537= Cyc_Absynpp_qvar2string( _temp2491->name);
struct _tagged_string _temp2538= Cyc_Absynpp_qvar2string( _temp2489->name);
xprintf("too many type arguments for tunion %.*s.%.*s", _temp2537.last_plus_one
- _temp2537.curr, _temp2537.curr, _temp2538.last_plus_one - _temp2538.curr,
_temp2538.curr);}));} if( tvs != 0){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2539= Cyc_Absynpp_qvar2string( _temp2491->name); struct
_tagged_string _temp2540= Cyc_Absynpp_qvar2string( _temp2489->name); xprintf("too few type arguments for tunion %.*s.%.*s",
_temp2539.last_plus_one - _temp2539.curr, _temp2539.curr, _temp2540.last_plus_one
- _temp2540.curr, _temp2540.curr);}));} goto _LL2476;} _LL2476:;} goto _LL2282;
_LL2296: free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars,( void*) Cyc_Absyn_AnyKind,
_temp2366); free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars,( void*)
Cyc_Absyn_RgnKind, _temp2364);{ void* _temp2541=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp2358))->v; void*
_temp2547; struct Cyc_Absyn_Exp* _temp2549; _LL2543: if(( unsigned int)
_temp2541 > 1u?*(( int*) _temp2541) == Cyc_Absyn_Eq_constr: 0){ _LL2548:
_temp2547=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2541)->f1; if((
unsigned int) _temp2547 > 1u?*(( int*) _temp2547) == Cyc_Absyn_Upper_b: 0){
_LL2550: _temp2549=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp2547)->f1; goto _LL2544;} else{ goto _LL2545;}} else{ goto _LL2545;}
_LL2545: goto _LL2546; _LL2544: if( ! Cyc_Tcutil_is_const_exp( te, _temp2549)){
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp2551=( char*)"pointer bounds expression is not a constant";
struct _tagged_string _temp2552; _temp2552.curr= _temp2551; _temp2552.base=
_temp2551; _temp2552.last_plus_one= _temp2551 + 44; _temp2552;}));} Cyc_Tcexp_tcExp(
te, 0, _temp2549); if( ! Cyc_Tcutil_coerce_uint_typ( te, _temp2549)){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp2553=( char*)"pointer bounds expression is not an unsigned int";
struct _tagged_string _temp2554; _temp2554.curr= _temp2553; _temp2554.base=
_temp2553; _temp2554.last_plus_one= _temp2553 + 49; _temp2554;}));} Cyc_Evexp_eval_const_uint_exp(
_temp2549); goto _LL2542; _LL2546: goto _LL2542; _LL2542:;} goto _LL2282;
_LL2298: goto _LL2282; _LL2300: goto _LL2282; _LL2302: goto _LL2282; _LL2304:
free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars,( void*) Cyc_Absyn_MemKind,
_temp2376); if( _temp2372 == 0? 1: ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp2555= _temp2372; if( _temp2555 == 0){ _throw(
Null_Exception);} _temp2555;}))){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({
char* _temp2556=( char*)"array bounds expression is not constant"; struct
_tagged_string _temp2557; _temp2557.curr= _temp2556; _temp2557.base= _temp2556;
_temp2557.last_plus_one= _temp2556 + 40; _temp2557;}));} Cyc_Tcexp_tcExp( te, 0,(
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2558= _temp2372; if(
_temp2558 == 0){ _throw( Null_Exception);} _temp2558;})); if( ! Cyc_Tcutil_coerce_uint_typ(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2559= _temp2372; if(
_temp2559 == 0){ _throw( Null_Exception);} _temp2559;}))){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp2560=( char*)"array bounds expression is not an unsigned int";
struct _tagged_string _temp2561; _temp2561.curr= _temp2560; _temp2561.base=
_temp2560; _temp2561.last_plus_one= _temp2560 + 47; _temp2561;}));} Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2562= _temp2372; if(
_temp2562 == 0){ _throw( Null_Exception);} _temp2562;})); goto _LL2282; _LL2306:
if( _temp2380 != 0){ int seen_cdecl= 0; int seen_stdcall= 0; for( 0; _temp2380
!= 0; _temp2380=({ struct Cyc_List_List* _temp2563= _temp2380; if( _temp2563 ==
0){ _throw( Null_Exception);} _temp2563->tl;})){ if( ! Cyc_Tcutil_fntype_att((
void*)({ struct Cyc_List_List* _temp2564= _temp2380; if( _temp2564 == 0){ _throw(
Null_Exception);} _temp2564->hd;}))){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2566= Cyc_Absyn_attribute2string(( void*)({ struct Cyc_List_List*
_temp2565= _temp2380; if( _temp2565 == 0){ _throw( Null_Exception);} _temp2565->hd;}));
xprintf("bad function type attribute %.*s", _temp2566.last_plus_one - _temp2566.curr,
_temp2566.curr);}));} if(( void*)({ struct Cyc_List_List* _temp2567= _temp2380;
if( _temp2567 == 0){ _throw( Null_Exception);} _temp2567->hd;}) ==( void*) Cyc_Absyn_Stdcall_att){
if( seen_cdecl){ Cyc_Tcutil_terr( loc, xprintf("function can't be both cdecl and stdcall"));}
seen_stdcall= 1;} else{ if(( void*)({ struct Cyc_List_List* _temp2568= _temp2380;
if( _temp2568 == 0){ _throw( Null_Exception);} _temp2568->hd;}) ==( void*) Cyc_Absyn_Cdecl_att){
if( seen_stdcall){ Cyc_Tcutil_terr( loc, xprintf("function can't be both cdecl and stdcall"));}
seen_cdecl= 1;}}}} Cyc_Tcutil_check_unique_tvars( loc,* _temp2393);{ struct Cyc_List_List*
b=* _temp2393; for( 0; b != 0; b=({ struct Cyc_List_List* _temp2569= b; if(
_temp2569 == 0){ _throw( Null_Exception);} _temp2569->tl;})){ void* _temp2571=(
void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp2570= b; if( _temp2570 == 0){
_throw( Null_Exception);} _temp2570->hd;}))->kind))->v; void* _temp2577; _LL2573:
if(( unsigned int) _temp2571 > 1u?*(( int*) _temp2571) == Cyc_Absyn_Eq_constr: 0){
_LL2578: _temp2577=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2571)->f1;
if( _temp2577 ==( void*) Cyc_Absyn_MemKind){ goto _LL2574;} else{ goto _LL2575;}}
else{ goto _LL2575;} _LL2575: goto _LL2576; _LL2574: Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2580=*(( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp2579= b; if( _temp2579 == 0){ _throw( Null_Exception);} _temp2579->hd;}))->name;
xprintf("function attempts to abstract Mem type variable %.*s", _temp2580.last_plus_one
- _temp2580.curr, _temp2580.curr);})); goto _LL2572; _LL2576: goto _LL2572;
_LL2572:;}}{ struct Cyc_List_List* new_free_vars= 0; new_free_vars= Cyc_Tcutil_check_valid_type(
loc, te, new_free_vars,( void*) Cyc_Absyn_MemKind, _temp2386); for( 0; _temp2384
!= 0; _temp2384=({ struct Cyc_List_List* _temp2581= _temp2384; if( _temp2581 ==
0){ _throw( Null_Exception);} _temp2581->tl;})){ new_free_vars= Cyc_Tcutil_check_valid_type(
loc, te, new_free_vars,( void*) Cyc_Absyn_MemKind,(*(( struct _tuple2*)({ struct
Cyc_List_List* _temp2582= _temp2384; if( _temp2582 == 0){ _throw( Null_Exception);}
_temp2582->hd;}))).f3);} if(* _temp2390 != 0){ new_free_vars= Cyc_Tcutil_check_valid_type(
loc, te, new_free_vars,( void*) Cyc_Absyn_EffKind,( void*)({ struct Cyc_Core_Opt*
_temp2583=* _temp2390; if( _temp2583 == 0){ _throw( Null_Exception);} _temp2583->v;}));}
else{ struct Cyc_Absyn_Tvar* e= Cyc_Tcutil_new_tvar(( void*) Cyc_Absyn_EffKind);
new_free_vars=({ struct Cyc_List_List* _temp2584=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2584->hd=( void*) e; _temp2584->tl=
new_free_vars; _temp2584;});{ struct Cyc_List_List* effect= 0;{ struct Cyc_List_List*
tvs= new_free_vars; for( 0; tvs != 0; tvs=({ struct Cyc_List_List* _temp2585=
tvs; if( _temp2585 == 0){ _throw( Null_Exception);} _temp2585->tl;})){ void*
_temp2587=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp2586= tvs; if( _temp2586 == 0){ _throw( Null_Exception);} _temp2586->hd;}))->kind))->v;
void* _temp2595; void* _temp2597; _LL2589: if(( unsigned int) _temp2587 > 1u?*((
int*) _temp2587) == Cyc_Absyn_Eq_constr: 0){ _LL2596: _temp2595=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp2587)->f1; if( _temp2595 ==( void*) Cyc_Absyn_RgnKind){
goto _LL2590;} else{ goto _LL2591;}} else{ goto _LL2591;} _LL2591: if((
unsigned int) _temp2587 > 1u?*(( int*) _temp2587) == Cyc_Absyn_Eq_constr: 0){
_LL2598: _temp2597=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2587)->f1;
if( _temp2597 ==( void*) Cyc_Absyn_EffKind){ goto _LL2592;} else{ goto _LL2593;}}
else{ goto _LL2593;} _LL2593: goto _LL2594; _LL2590: effect=({ struct Cyc_List_List*
_temp2599=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2599->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2600=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2600[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2601; _temp2601.tag= Cyc_Absyn_AccessEff;
_temp2601.f1=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp2602=(
struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2602[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2603; _temp2603.tag= Cyc_Absyn_VarType;
_temp2603.f1=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp2604= tvs;
if( _temp2604 == 0){ _throw( Null_Exception);} _temp2604->hd;}); _temp2603;});
_temp2602;})); _temp2601;}); _temp2600;})); _temp2599->tl= effect; _temp2599;});
goto _LL2588; _LL2592: effect=({ struct Cyc_List_List* _temp2605=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2605->hd=( void*)(( void*)({
struct Cyc_Absyn_VarType_struct* _temp2606=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp2606[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp2607; _temp2607.tag= Cyc_Absyn_VarType; _temp2607.f1=(
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp2608= tvs; if( _temp2608 ==
0){ _throw( Null_Exception);} _temp2608->hd;}); _temp2607;}); _temp2606;}));
_temp2605->tl= effect; _temp2605;}); goto _LL2588; _LL2594: goto _LL2588;
_LL2588:;}}* _temp2390=({ struct Cyc_Core_Opt* _temp2609=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2609->v=( void*)(( void*)({
struct Cyc_Absyn_JoinEff_struct* _temp2610=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp2610[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp2611; _temp2611.tag= Cyc_Absyn_JoinEff; _temp2611.f1=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( effect);
_temp2611;}); _temp2610;})); _temp2609;});}} if(* _temp2393 != 0){ struct Cyc_List_List*
new_btvs= 0;{ struct Cyc_List_List* bs=* _temp2393; for( 0; bs != 0; bs=({
struct Cyc_List_List* _temp2612= bs; if( _temp2612 == 0){ _throw( Null_Exception);}
_temp2612->tl;})){ new_free_vars= Cyc_Tcutil_add_free_tvar( loc, new_free_vars,(
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp2613= bs; if( _temp2613 ==
0){ _throw( Null_Exception);} _temp2613->hd;}));{ struct Cyc_Absyn_Conref* c=((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp2628= bs; if( _temp2628 ==
0){ _throw( Null_Exception);} _temp2628->hd;}))->kind); void* _temp2614=( void*)
c->v; void* _temp2622; _LL2616: if( _temp2614 ==( void*) Cyc_Absyn_No_constr){
goto _LL2617;} else{ goto _LL2618;} _LL2618: if(( unsigned int) _temp2614 > 1u?*((
int*) _temp2614) == Cyc_Absyn_Eq_constr: 0){ _LL2623: _temp2622=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp2614)->f1; if( _temp2622 ==( void*) Cyc_Absyn_MemKind){
goto _LL2619;} else{ goto _LL2620;}} else{ goto _LL2620;} _LL2620: goto _LL2621;
_LL2617: Cyc_Tcutil_warn( loc,({ struct _tagged_string _temp2625=*(( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp2624= bs; if( _temp2624 == 0){ _throw( Null_Exception);}
_temp2624->hd;}))->name; xprintf("Type variable %.*s unconstrained, assuming boxed",
_temp2625.last_plus_one - _temp2625.curr, _temp2625.curr);})); goto _LL2619;
_LL2619:( void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp2626=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2626[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp2627; _temp2627.tag= Cyc_Absyn_Eq_constr;
_temp2627.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp2627;}); _temp2626;})));
goto _LL2615; _LL2621: goto _LL2615; _LL2615:;}}} new_free_vars= Cyc_Tcutil_remove_bound_tvars(
new_free_vars,* _temp2393);} for( 0; new_free_vars != 0; new_free_vars=({ struct
Cyc_List_List* _temp2629= new_free_vars; if( _temp2629 == 0){ _throw(
Null_Exception);} _temp2629->tl;})){ free_vars= Cyc_Tcutil_add_free_tvar( loc,
free_vars,( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp2630=
new_free_vars; if( _temp2630 == 0){ _throw( Null_Exception);} _temp2630->hd;}));}
goto _LL2282;} _LL2308: for( 0; _temp2394 != 0; _temp2394=({ struct Cyc_List_List*
_temp2631= _temp2394; if( _temp2631 == 0){ _throw( Null_Exception);} _temp2631->tl;})){
free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars,( void*) Cyc_Absyn_MemKind,(*((
struct _tuple4*)({ struct Cyc_List_List* _temp2632= _temp2394; if( _temp2632 ==
0){ _throw( Null_Exception);} _temp2632->hd;}))).f2);} goto _LL2282; _LL2310:{
struct _RegionHandle _temp2633= _new_region(); struct _RegionHandle* sprev_rgn=&
_temp2633; _push_region( sprev_rgn);{ struct Cyc_List_List* prev_fields= 0; for(
0; _temp2396 != 0; _temp2396=({ struct Cyc_List_List* _temp2634= _temp2396; if(
_temp2634 == 0){ _throw( Null_Exception);} _temp2634->tl;})){ struct Cyc_Absyn_Structfield
_temp2638; struct Cyc_List_List* _temp2639; struct Cyc_Core_Opt* _temp2641; void*
_temp2643; struct Cyc_Absyn_Tqual _temp2645; struct _tagged_string* _temp2647;
struct Cyc_Absyn_Structfield* _temp2636=( struct Cyc_Absyn_Structfield*)({
struct Cyc_List_List* _temp2635= _temp2396; if( _temp2635 == 0){ _throw(
Null_Exception);} _temp2635->hd;}); _temp2638=* _temp2636; _LL2648: _temp2647=(
struct _tagged_string*) _temp2638.name; goto _LL2646; _LL2646: _temp2645=(
struct Cyc_Absyn_Tqual) _temp2638.tq; goto _LL2644; _LL2644: _temp2643=( void*)
_temp2638.type; goto _LL2642; _LL2642: _temp2641=( struct Cyc_Core_Opt*)
_temp2638.width; goto _LL2640; _LL2640: _temp2639=( struct Cyc_List_List*)
_temp2638.attributes; goto _LL2637; _LL2637: if((( int(*)( int(* compare)(
struct _tagged_string*, struct _tagged_string*), struct Cyc_List_List* l, struct
_tagged_string* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp, prev_fields, _temp2647)){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2649=* _temp2647; xprintf("duplicate field %.*s in struct",
_temp2649.last_plus_one - _temp2649.curr, _temp2649.curr);}));} if( Cyc_String_strcmp(*
_temp2647,( struct _tagged_string)({ char* _temp2650=( char*)""; struct
_tagged_string _temp2651; _temp2651.curr= _temp2650; _temp2651.base= _temp2650;
_temp2651.last_plus_one= _temp2650 + 1; _temp2651;})) != 0){ prev_fields=({
struct Cyc_List_List* _temp2652=( struct Cyc_List_List*) _region_malloc(
sprev_rgn, sizeof( struct Cyc_List_List)); _temp2652->hd=( void*) _temp2647;
_temp2652->tl= prev_fields; _temp2652;});} Cyc_Tcutil_check_valid_type( loc, te,
free_vars,( void*) Cyc_Absyn_MemKind, _temp2643); if( _temp2641 != 0){
unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)({
struct Cyc_Core_Opt* _temp2653= _temp2641; if( _temp2653 == 0){ _throw(
Null_Exception);} _temp2653->v;}))){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2654=* _temp2647; xprintf("bitfield %.*s does not have constant width",
_temp2654.last_plus_one - _temp2654.curr, _temp2654.curr);}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp2655= _temp2641; if(
_temp2655 == 0){ _throw( Null_Exception);} _temp2655->v;}));}{ void* _temp2656=
Cyc_Tcutil_compress( _temp2643); void* _temp2662; void* _temp2664; _LL2658: if((
unsigned int) _temp2656 > 4u?*(( int*) _temp2656) == Cyc_Absyn_IntType: 0){
_LL2665: _temp2664=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2656)->f1;
goto _LL2663; _LL2663: _temp2662=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2656)->f2; goto _LL2659;} else{ goto _LL2660;} _LL2660: goto _LL2661;
_LL2659:{ void* _temp2666= _temp2662; _LL2668: if( _temp2666 ==( void*) Cyc_Absyn_B1){
goto _LL2669;} else{ goto _LL2670;} _LL2670: if( _temp2666 ==( void*) Cyc_Absyn_B2){
goto _LL2671;} else{ goto _LL2672;} _LL2672: if( _temp2666 ==( void*) Cyc_Absyn_B4){
goto _LL2673;} else{ goto _LL2674;} _LL2674: if( _temp2666 ==( void*) Cyc_Absyn_B8){
goto _LL2675;} else{ goto _LL2667;} _LL2669: if( w > 8){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp2676=( char*)"bitfield larger than type";
struct _tagged_string _temp2677; _temp2677.curr= _temp2676; _temp2677.base=
_temp2676; _temp2677.last_plus_one= _temp2676 + 26; _temp2677;}));} goto _LL2667;
_LL2671: if( w > 16){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp2678=( char*)"bitfield larger than type"; struct _tagged_string _temp2679;
_temp2679.curr= _temp2678; _temp2679.base= _temp2678; _temp2679.last_plus_one=
_temp2678 + 26; _temp2679;}));} goto _LL2667; _LL2673: if( w > 32){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp2680=( char*)"bitfield larger than type";
struct _tagged_string _temp2681; _temp2681.curr= _temp2680; _temp2681.base=
_temp2680; _temp2681.last_plus_one= _temp2680 + 26; _temp2681;}));} goto _LL2667;
_LL2675: if( w > 64){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp2682=( char*)"bitfield larger than type"; struct _tagged_string _temp2683;
_temp2683.curr= _temp2682; _temp2683.base= _temp2682; _temp2683.last_plus_one=
_temp2682 + 26; _temp2683;}));} goto _LL2667; _LL2667:;} goto _LL2657; _LL2661:
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2684=* _temp2647; struct
_tagged_string _temp2685= Cyc_Absynpp_typ2string( _temp2643); xprintf("bitfield %.*s must have integral type but has type %.*s",
_temp2684.last_plus_one - _temp2684.curr, _temp2684.curr, _temp2685.last_plus_one
- _temp2685.curr, _temp2685.curr);})); goto _LL2657; _LL2657:;}} for( 0;
_temp2639 != 0; _temp2639=({ struct Cyc_List_List* _temp2686= _temp2639; if(
_temp2686 == 0){ _throw( Null_Exception);} _temp2686->tl;})){ void* _temp2688=(
void*)({ struct Cyc_List_List* _temp2687= _temp2639; if( _temp2687 == 0){ _throw(
Null_Exception);} _temp2687->hd;}); int _temp2696; _LL2690: if( _temp2688 ==(
void*) Cyc_Absyn_Packed_att){ goto _LL2691;} else{ goto _LL2692;} _LL2692: if((
unsigned int) _temp2688 > 15u?*(( int*) _temp2688) == Cyc_Absyn_Aligned_att: 0){
_LL2697: _temp2696=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp2688)->f1;
goto _LL2693;} else{ goto _LL2694;} _LL2694: goto _LL2695; _LL2691: continue;
_LL2693: continue; _LL2695: Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2699= Cyc_Absyn_attribute2string(( void*)({ struct Cyc_List_List* _temp2698=
_temp2639; if( _temp2698 == 0){ _throw( Null_Exception);} _temp2698->hd;}));
struct _tagged_string _temp2700=* _temp2647; xprintf("bad attribute %.*s on struct member %.*s",
_temp2699.last_plus_one - _temp2699.curr, _temp2699.curr, _temp2700.last_plus_one
- _temp2700.curr, _temp2700.curr);})); goto _LL2689; _LL2689:;}}}; _pop_region(&
_temp2633);} goto _LL2282; _LL2312:{ struct _RegionHandle _temp2701= _new_region();
struct _RegionHandle* uprev_rgn=& _temp2701; _push_region( uprev_rgn);{ struct
Cyc_List_List* prev_fields= 0; for( 0; _temp2398 != 0; _temp2398=({ struct Cyc_List_List*
_temp2702= _temp2398; if( _temp2702 == 0){ _throw( Null_Exception);} _temp2702->tl;})){
struct Cyc_Absyn_Structfield _temp2706; struct Cyc_List_List* _temp2707; struct
Cyc_Core_Opt* _temp2709; void* _temp2711; struct Cyc_Absyn_Tqual _temp2713;
struct _tagged_string* _temp2715; struct Cyc_Absyn_Structfield* _temp2704=(
struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp2703= _temp2398; if(
_temp2703 == 0){ _throw( Null_Exception);} _temp2703->hd;}); _temp2706=*
_temp2704; _LL2716: _temp2715=( struct _tagged_string*) _temp2706.name; goto
_LL2714; _LL2714: _temp2713=( struct Cyc_Absyn_Tqual) _temp2706.tq; goto _LL2712;
_LL2712: _temp2711=( void*) _temp2706.type; goto _LL2710; _LL2710: _temp2709=(
struct Cyc_Core_Opt*) _temp2706.width; goto _LL2708; _LL2708: _temp2707=( struct
Cyc_List_List*) _temp2706.attributes; goto _LL2705; _LL2705: if((( int(*)( int(*
compare)( struct _tagged_string*, struct _tagged_string*), struct Cyc_List_List*
l, struct _tagged_string* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp, prev_fields,
_temp2715)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2717=* _temp2715;
xprintf("duplicate field %.*s in struct", _temp2717.last_plus_one - _temp2717.curr,
_temp2717.curr);}));} if( Cyc_String_strcmp(* _temp2715,( struct _tagged_string)({
char* _temp2718=( char*)""; struct _tagged_string _temp2719; _temp2719.curr=
_temp2718; _temp2719.base= _temp2718; _temp2719.last_plus_one= _temp2718 + 1;
_temp2719;})) != 0){ prev_fields=({ struct Cyc_List_List* _temp2720=( struct Cyc_List_List*)
_region_malloc( uprev_rgn, sizeof( struct Cyc_List_List)); _temp2720->hd=( void*)
_temp2715; _temp2720->tl= prev_fields; _temp2720;});} Cyc_Tcutil_check_valid_type(
loc, te, free_vars,( void*) Cyc_Absyn_MemKind, _temp2711); if( ! Cyc_Tcutil_bits_only(
_temp2711)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2721=* _temp2715;
xprintf("%.*s has a type that is possibly incompatible with other members of union",
_temp2721.last_plus_one - _temp2721.curr, _temp2721.curr);}));} if( _temp2709 !=
0){ unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)({
struct Cyc_Core_Opt* _temp2722= _temp2709; if( _temp2722 == 0){ _throw(
Null_Exception);} _temp2722->v;}))){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2723=* _temp2715; xprintf("bitfield %.*s does not have constant width",
_temp2723.last_plus_one - _temp2723.curr, _temp2723.curr);}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp2724= _temp2709; if(
_temp2724 == 0){ _throw( Null_Exception);} _temp2724->v;}));}{ void* _temp2725=
Cyc_Tcutil_compress( _temp2711); void* _temp2731; void* _temp2733; _LL2727: if((
unsigned int) _temp2725 > 4u?*(( int*) _temp2725) == Cyc_Absyn_IntType: 0){
_LL2734: _temp2733=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2725)->f1;
goto _LL2732; _LL2732: _temp2731=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2725)->f2; goto _LL2728;} else{ goto _LL2729;} _LL2729: goto _LL2730;
_LL2728:{ void* _temp2735= _temp2731; _LL2737: if( _temp2735 ==( void*) Cyc_Absyn_B1){
goto _LL2738;} else{ goto _LL2739;} _LL2739: if( _temp2735 ==( void*) Cyc_Absyn_B2){
goto _LL2740;} else{ goto _LL2741;} _LL2741: if( _temp2735 ==( void*) Cyc_Absyn_B4){
goto _LL2742;} else{ goto _LL2743;} _LL2743: if( _temp2735 ==( void*) Cyc_Absyn_B8){
goto _LL2744;} else{ goto _LL2736;} _LL2738: if( w > 8){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp2745=( char*)"bitfield larger than type";
struct _tagged_string _temp2746; _temp2746.curr= _temp2745; _temp2746.base=
_temp2745; _temp2746.last_plus_one= _temp2745 + 26; _temp2746;}));} goto _LL2736;
_LL2740: if( w > 16){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp2747=( char*)"bitfield larger than type"; struct _tagged_string _temp2748;
_temp2748.curr= _temp2747; _temp2748.base= _temp2747; _temp2748.last_plus_one=
_temp2747 + 26; _temp2748;}));} goto _LL2736; _LL2742: if( w > 32){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp2749=( char*)"bitfield larger than type";
struct _tagged_string _temp2750; _temp2750.curr= _temp2749; _temp2750.base=
_temp2749; _temp2750.last_plus_one= _temp2749 + 26; _temp2750;}));} goto _LL2736;
_LL2744: if( w > 64){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp2751=( char*)"bitfield larger than type"; struct _tagged_string _temp2752;
_temp2752.curr= _temp2751; _temp2752.base= _temp2751; _temp2752.last_plus_one=
_temp2751 + 26; _temp2752;}));} goto _LL2736; _LL2736:;} goto _LL2726; _LL2730:
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2753=* _temp2715; struct
_tagged_string _temp2754= Cyc_Absynpp_typ2string( _temp2711); xprintf("bitfield %.*s must have integral type but has type %.*s",
_temp2753.last_plus_one - _temp2753.curr, _temp2753.curr, _temp2754.last_plus_one
- _temp2754.curr, _temp2754.curr);})); goto _LL2726; _LL2726:;}} for( 0;
_temp2707 != 0; _temp2707=({ struct Cyc_List_List* _temp2755= _temp2707; if(
_temp2755 == 0){ _throw( Null_Exception);} _temp2755->tl;})){ void* _temp2757=(
void*)({ struct Cyc_List_List* _temp2756= _temp2707; if( _temp2756 == 0){ _throw(
Null_Exception);} _temp2756->hd;}); int _temp2765; _LL2759: if( _temp2757 ==(
void*) Cyc_Absyn_Packed_att){ goto _LL2760;} else{ goto _LL2761;} _LL2761: if((
unsigned int) _temp2757 > 15u?*(( int*) _temp2757) == Cyc_Absyn_Aligned_att: 0){
_LL2766: _temp2765=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp2757)->f1;
goto _LL2762;} else{ goto _LL2763;} _LL2763: goto _LL2764; _LL2760: continue;
_LL2762: continue; _LL2764: Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2768= Cyc_Absyn_attribute2string(( void*)({ struct Cyc_List_List* _temp2767=
_temp2707; if( _temp2767 == 0){ _throw( Null_Exception);} _temp2767->hd;}));
struct _tagged_string _temp2769=* _temp2715; xprintf("bad attribute %.*s on struct member %.*s",
_temp2768.last_plus_one - _temp2768.curr, _temp2768.curr, _temp2769.last_plus_one
- _temp2769.curr, _temp2769.curr);})); goto _LL2758; _LL2758:;}}}; _pop_region(&
_temp2701);} goto _LL2282; _LL2314: if( _temp2405 == 0){ if(* _temp2402 == 0){
Cyc_Tcutil_terr( loc, xprintf("bad anonymous struct")); return free_vars;}{
struct Cyc_Absyn_Structdecl* sd=({ struct Cyc_Absyn_Structdecl** _temp2770=*
_temp2402; if( _temp2770 == 0){ _throw( Null_Exception);}* _temp2770;});;}} if(*
_temp2402 == 0){ struct _handler_cons _temp2771; _push_handler(& _temp2771);{
void* _temp2772=( void*) setjmp( _temp2771.handler); if( ! _temp2772){*
_temp2402=( struct Cyc_Absyn_Structdecl**) Cyc_Tcenv_lookup_structdecl( te, loc,(
struct _tuple1*)({ struct _tuple1* _temp2773= _temp2405; if( _temp2773 == 0){
_throw( Null_Exception);} _temp2773;}));; _pop_handler();} else{ void* _temp2775=
_temp2772; _LL2777: if( _temp2775 == Cyc_Dict_Absent){ goto _LL2778;} else{ goto
_LL2779;} _LL2779: goto _LL2780; _LL2778: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2782= Cyc_Absynpp_qvar2string(( struct _tuple1*)({ struct
_tuple1* _temp2781= _temp2405; if( _temp2781 == 0){ _throw( Null_Exception);}
_temp2781;})); xprintf("unbound type struct %.*s", _temp2782.last_plus_one -
_temp2782.curr, _temp2782.curr);})); return free_vars; _LL2780:( void) _throw(
_temp2775); _LL2776:;}}}{ struct Cyc_Absyn_Structdecl* sd=({ struct Cyc_Absyn_Structdecl**
_temp2795=* _temp2402; if( _temp2795 == 0){ _throw( Null_Exception);}* _temp2795;});
if( sd->name != 0){({ struct _tuple1* _temp2783= _temp2405; if( _temp2783 == 0){
_throw( Null_Exception);}* _temp2783=({ struct _tuple1* _temp2785=( struct
_tuple1*)({ struct Cyc_Core_Opt* _temp2784= sd->name; if( _temp2784 == 0){
_throw( Null_Exception);} _temp2784->v;}); unsigned int _temp2786= 0; if(
_temp2786 >= 1u){ _throw( Null_Exception);} _temp2785[ _temp2786];});});}{
struct Cyc_List_List* tvs= sd->tvs; int lvs=(( int(*)( struct Cyc_List_List* x))
Cyc_List_length)( tvs); int largs=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp2403); if( lvs != largs){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2788= Cyc_Absynpp_qvar2string(( struct _tuple1*)({ struct _tuple1*
_temp2787= _temp2405; if( _temp2787 == 0){ _throw( Null_Exception);} _temp2787;}));
int _temp2789= lvs; int _temp2790= largs; xprintf("struct %.*s expects %d type arguments but was given %d",
_temp2788.last_plus_one - _temp2788.curr, _temp2788.curr, _temp2789, _temp2790);}));}
for( 0; _temp2403 != 0; _temp2403=({ struct Cyc_List_List* _temp2791= _temp2403;
if( _temp2791 == 0){ _throw( Null_Exception);} _temp2791->tl;}), tvs=({ struct
Cyc_List_List* _temp2792= tvs; if( _temp2792 == 0){ _throw( Null_Exception);}
_temp2792->tl;})){ void* t1=( void*)({ struct Cyc_List_List* _temp2794=
_temp2403; if( _temp2794 == 0){ _throw( Null_Exception);} _temp2794->hd;}); void*
k1=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp2793= tvs; if( _temp2793 == 0){ _throw(
Null_Exception);} _temp2793->hd;}))->kind); free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars, k1, t1);} goto _LL2282;}} _LL2316: if( _temp2412 == 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp2796=( char*)"missing union name";
struct _tagged_string _temp2797; _temp2797.curr= _temp2796; _temp2797.base=
_temp2796; _temp2797.last_plus_one= _temp2796 + 19; _temp2797;})); return
free_vars;} if(* _temp2409 == 0){ struct _handler_cons _temp2798; _push_handler(&
_temp2798);{ void* _temp2799=( void*) setjmp( _temp2798.handler); if( !
_temp2799){* _temp2409=( struct Cyc_Absyn_Uniondecl**) Cyc_Tcenv_lookup_uniondecl(
te, loc,( struct _tuple1*)({ struct _tuple1* _temp2800= _temp2412; if( _temp2800
== 0){ _throw( Null_Exception);} _temp2800;}));; _pop_handler();} else{ void*
_temp2802= _temp2799; _LL2804: if( _temp2802 == Cyc_Dict_Absent){ goto _LL2805;}
else{ goto _LL2806;} _LL2806: goto _LL2807; _LL2805: Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2809= Cyc_Absynpp_qvar2string(( struct _tuple1*)({
struct _tuple1* _temp2808= _temp2412; if( _temp2808 == 0){ _throw(
Null_Exception);} _temp2808;})); xprintf("unbound type union %.*s", _temp2809.last_plus_one
- _temp2809.curr, _temp2809.curr);})); return free_vars; _LL2807:( void) _throw(
_temp2802); _LL2803:;}}}{ struct Cyc_Absyn_Uniondecl* ud=({ struct Cyc_Absyn_Uniondecl**
_temp2822=* _temp2409; if( _temp2822 == 0){ _throw( Null_Exception);}* _temp2822;});
if( ud->name != 0){({ struct _tuple1* _temp2810= _temp2412; if( _temp2810 == 0){
_throw( Null_Exception);}* _temp2810=({ struct _tuple1* _temp2812=( struct
_tuple1*)({ struct Cyc_Core_Opt* _temp2811= ud->name; if( _temp2811 == 0){
_throw( Null_Exception);} _temp2811->v;}); unsigned int _temp2813= 0; if(
_temp2813 >= 1u){ _throw( Null_Exception);} _temp2812[ _temp2813];});});}{
struct Cyc_List_List* tvs= ud->tvs; int lvs=(( int(*)( struct Cyc_List_List* x))
Cyc_List_length)( tvs); int largs=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp2410); if( lvs != largs){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2815= Cyc_Absynpp_qvar2string(( struct _tuple1*)({ struct _tuple1*
_temp2814= _temp2412; if( _temp2814 == 0){ _throw( Null_Exception);} _temp2814;}));
int _temp2816= lvs; int _temp2817= largs; xprintf("union %.*s expects %d type arguments but was given %d",
_temp2815.last_plus_one - _temp2815.curr, _temp2815.curr, _temp2816, _temp2817);}));}
for( 0; _temp2410 != 0; _temp2410=({ struct Cyc_List_List* _temp2818= _temp2410;
if( _temp2818 == 0){ _throw( Null_Exception);} _temp2818->tl;}), tvs=({ struct
Cyc_List_List* _temp2819= tvs; if( _temp2819 == 0){ _throw( Null_Exception);}
_temp2819->tl;})){ void* t1=( void*)({ struct Cyc_List_List* _temp2821=
_temp2410; if( _temp2821 == 0){ _throw( Null_Exception);} _temp2821->hd;}); void*
k1=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp2820= tvs; if( _temp2820 == 0){ _throw(
Null_Exception);} _temp2820->hd;}))->kind); free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars, k1, t1);} goto _LL2282;}} _LL2318: { struct Cyc_Absyn_Typedefdecl*
td;{ struct _handler_cons _temp2823; _push_handler(& _temp2823);{ void*
_temp2824=( void*) setjmp( _temp2823.handler); if( ! _temp2824){ td= Cyc_Tcenv_lookup_typedefdecl(
te, loc, _temp2419);; _pop_handler();} else{ void* _temp2826= _temp2824; _LL2828:
if( _temp2826 == Cyc_Dict_Absent){ goto _LL2829;} else{ goto _LL2830;} _LL2830:
goto _LL2831; _LL2829: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2832=
Cyc_Absynpp_qvar2string( _temp2419); xprintf("unbound typedef name %.*s",
_temp2832.last_plus_one - _temp2832.curr, _temp2832.curr);})); return free_vars;
_LL2831:( void) _throw( _temp2826); _LL2827:;}}}({ struct _tuple1* _temp2833=
_temp2419; unsigned int _temp2834= 0; if( _temp2834 >= 1u){ _throw(
Null_Exception);} _temp2833[ _temp2834]=({ struct _tuple1* _temp2835= td->name;
unsigned int _temp2836= 0; if( _temp2836 >= 1u){ _throw( Null_Exception);}
_temp2835[ _temp2836];});});{ struct Cyc_List_List* tvs= td->tvs; struct Cyc_List_List*
ts= _temp2417; struct Cyc_List_List* inst= 0; for( 0; ts != 0? tvs != 0: 0; ts=({
struct Cyc_List_List* _temp2837= ts; if( _temp2837 == 0){ _throw( Null_Exception);}
_temp2837->tl;}), tvs=({ struct Cyc_List_List* _temp2838= tvs; if( _temp2838 ==
0){ _throw( Null_Exception);} _temp2838->tl;})){ void* k=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp2844= tvs; if( _temp2844 == 0){ _throw( Null_Exception);} _temp2844->hd;}))->kind);
free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars, k,( void*)({ struct
Cyc_List_List* _temp2839= ts; if( _temp2839 == 0){ _throw( Null_Exception);}
_temp2839->hd;})); inst=({ struct Cyc_List_List* _temp2840=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2840->hd=( void*)({ struct
_tuple5* _temp2841=( struct _tuple5*) GC_malloc( sizeof( struct _tuple5));
_temp2841->f1=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp2843= tvs;
if( _temp2843 == 0){ _throw( Null_Exception);} _temp2843->hd;}); _temp2841->f2=(
void*)({ struct Cyc_List_List* _temp2842= ts; if( _temp2842 == 0){ _throw(
Null_Exception);} _temp2842->hd;}); _temp2841;}); _temp2840->tl= inst; _temp2840;});}
if( tvs != 0? 1: ts != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2845= Cyc_Absynpp_qvar2string( _temp2419); xprintf("typedef %.*s expects a different number of arguments",
_temp2845.last_plus_one - _temp2845.curr, _temp2845.curr);})); return free_vars;}{
void* new_typ= Cyc_Tcutil_substitute( inst,( void*) td->defn);* _temp2416=({
struct Cyc_Core_Opt* _temp2846=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2846->v=( void*) new_typ; _temp2846;}); goto _LL2282;}}}
_LL2320: goto _LL2282; _LL2322: _temp2423= _temp2421; goto _LL2324; _LL2324:
free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars,( void*) Cyc_Absyn_RgnKind,
_temp2423); goto _LL2282; _LL2326: for( 0; _temp2425 != 0; _temp2425=({ struct
Cyc_List_List* _temp2847= _temp2425; if( _temp2847 == 0){ _throw( Null_Exception);}
_temp2847->tl;})){ free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars,(
void*) Cyc_Absyn_EffKind,( void*)({ struct Cyc_List_List* _temp2848= _temp2425;
if( _temp2848 == 0){ _throw( Null_Exception);} _temp2848->hd;}));} goto _LL2282;
_LL2282:;} if( ! Cyc_Tcutil_kind_leq( Cyc_Tcutil_typ_kind( t), expected_kind)){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2849= Cyc_Absynpp_typ2string(
t); struct _tagged_string _temp2850= Cyc_Absynpp_kind2string( Cyc_Tcutil_typ_kind(
t)); struct _tagged_string _temp2851= Cyc_Absynpp_kind2string( expected_kind);
xprintf("type %.*s has kind %.*s but as used here needs kind %.*s", _temp2849.last_plus_one
- _temp2849.curr, _temp2849.curr, _temp2850.last_plus_one - _temp2850.curr,
_temp2850.curr, _temp2851.last_plus_one - _temp2851.curr, _temp2851.curr);}));}
return free_vars;} void Cyc_Tcutil_check_valid_toplevel_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, void* t){ struct Cyc_List_List* free_tvars= Cyc_Tcutil_check_valid_type(
loc, te, 0,( void*) Cyc_Absyn_MemKind, t);{ struct Cyc_List_List* x= free_tvars;
for( 0; x != 0; x=({ struct Cyc_List_List* _temp2852= x; if( _temp2852 == 0){
_throw( Null_Exception);} _temp2852->tl;})){ struct Cyc_Absyn_Conref* c=((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp2865= x; if( _temp2865 == 0){
_throw( Null_Exception);} _temp2865->hd;}))->kind); void* _temp2853=( void*) c->v;
void* _temp2861; _LL2855: if( _temp2853 ==( void*) Cyc_Absyn_No_constr){ goto
_LL2856;} else{ goto _LL2857;} _LL2857: if(( unsigned int) _temp2853 > 1u?*((
int*) _temp2853) == Cyc_Absyn_Eq_constr: 0){ _LL2862: _temp2861=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp2853)->f1; if( _temp2861 ==( void*) Cyc_Absyn_MemKind){
goto _LL2858;} else{ goto _LL2859;}} else{ goto _LL2859;} _LL2859: goto _LL2860;
_LL2856: goto _LL2858; _LL2858:( void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp2863=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2863[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp2864; _temp2864.tag= Cyc_Absyn_Eq_constr;
_temp2864.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp2864;}); _temp2863;})));
goto _LL2854; _LL2860: goto _LL2854; _LL2854:;}} if( free_tvars != 0){ void*
_temp2866= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo _temp2872; struct
Cyc_List_List* _temp2874; int _temp2876; struct Cyc_List_List* _temp2878; void*
_temp2880; struct Cyc_Core_Opt* _temp2882; struct Cyc_List_List* _temp2884;
struct Cyc_List_List** _temp2886; _LL2868: if(( unsigned int) _temp2866 > 4u?*((
int*) _temp2866) == Cyc_Absyn_FnType: 0){ _LL2873: _temp2872=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp2866)->f1; _LL2885: _temp2884=( struct Cyc_List_List*)
_temp2872.tvars; _temp2886=&((( struct Cyc_Absyn_FnType_struct*) _temp2866)->f1).tvars;
goto _LL2883; _LL2883: _temp2882=( struct Cyc_Core_Opt*) _temp2872.effect; goto
_LL2881; _LL2881: _temp2880=( void*) _temp2872.ret_typ; goto _LL2879; _LL2879:
_temp2878=( struct Cyc_List_List*) _temp2872.args; goto _LL2877; _LL2877:
_temp2876=( int) _temp2872.varargs; goto _LL2875; _LL2875: _temp2874=( struct
Cyc_List_List*) _temp2872.attributes; goto _LL2869;} else{ goto _LL2870;}
_LL2870: goto _LL2871; _LL2869: if(* _temp2886 == 0){* _temp2886= free_tvars;}
else{ if( free_tvars != 0){ struct _tagged_string s=*(( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp2891= free_tvars; if( _temp2891 == 0){ _throw(
Null_Exception);} _temp2891->hd;}))->name; if(({ struct _tagged_string _temp2887=
s; char* _temp2889= _temp2887.curr + 0; if( _temp2887.base == 0? 1:( _temp2889 <
_temp2887.base? 1: _temp2889 >= _temp2887.last_plus_one)){ _throw(
Null_Exception);}* _temp2889;}) =='?'){ Cyc_Tcutil_terr( loc, xprintf("You failed to abstract over the generated type variables. \nRemove the bound type variables or else make the effects of the type explicit\nand then abstract the free type variables properly."));}
else{ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2890= s; xprintf("unbound type variable %.*s",
_temp2890.last_plus_one - _temp2890.curr, _temp2890.curr);}));}}} goto _LL2867;
_LL2871: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2893=*(( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp2892= free_tvars; if( _temp2892 == 0){ _throw(
Null_Exception);} _temp2892->hd;}))->name; xprintf("unbound type variable %.*s",
_temp2893.last_plus_one - _temp2893.curr, _temp2893.curr);})); return; _LL2867:;}}
void Cyc_Tcutil_check_type( struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* bound_tvars, void* expected_kind, void* t){ struct Cyc_List_List*
free_tvars= Cyc_Tcutil_check_valid_type( loc, te, 0, expected_kind, t); struct
Cyc_List_List* fs= free_tvars; for( 0; fs != 0; fs=({ struct Cyc_List_List*
_temp2894= fs; if( _temp2894 == 0){ _throw( Null_Exception);} _temp2894->tl;})){
struct Cyc_Absyn_Tvar* ftv=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp2902= fs; if( _temp2902 == 0){ _throw( Null_Exception);} _temp2902->hd;});
struct _tagged_string* ftvn= ftv->name; struct Cyc_Absyn_Conref* ftvk= ftv->kind;
int present= 0;{ struct Cyc_List_List* bs= bound_tvars; for( 0; bs != 0; bs=({
struct Cyc_List_List* _temp2895= bs; if( _temp2895 == 0){ _throw( Null_Exception);}
_temp2895->tl;})){ struct Cyc_Absyn_Tvar* btv=( struct Cyc_Absyn_Tvar*)({ struct
Cyc_List_List* _temp2899= bs; if( _temp2899 == 0){ _throw( Null_Exception);}
_temp2899->hd;}); struct _tagged_string* btvn= btv->name; struct Cyc_Absyn_Conref*
btvk= btv->kind; if( Cyc_String_zstrptrcmp( ftvn, btvn) == 0){ present= 1; if( !
Cyc_Tcutil_constrain_kinds( btvk, ftvk)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2896=* ftvn; struct _tagged_string _temp2897= Cyc_Absynpp_ckind2string(
btvk); struct _tagged_string _temp2898= Cyc_Absynpp_ckind2string( ftvk); xprintf("type variable %.*s has kind %.*s but is used here atkind %.*s",
_temp2896.last_plus_one - _temp2896.curr, _temp2896.curr, _temp2897.last_plus_one
- _temp2897.curr, _temp2897.curr, _temp2898.last_plus_one - _temp2898.curr,
_temp2898.curr);}));} break;}}} if( ! present){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2900=* ftvn; struct _tagged_string _temp2901= Cyc_Absynpp_typ2string(
t); xprintf("unbound type variable %.*s in type %.*s", _temp2900.last_plus_one -
_temp2900.curr, _temp2900.curr, _temp2901.last_plus_one - _temp2901.curr,
_temp2901.curr);}));}}} static void Cyc_Tcutil_check_unique_unsorted( int(* cmp)(
void*, void*), struct Cyc_List_List* vs, struct Cyc_Position_Segment* loc,
struct _tagged_string(* a2string)( void*), struct _tagged_string msg){ for( 0;
vs != 0; vs=({ struct Cyc_List_List* _temp2903= vs; if( _temp2903 == 0){ _throw(
Null_Exception);} _temp2903->tl;})){ struct Cyc_List_List* vs2=({ struct Cyc_List_List*
_temp2910= vs; if( _temp2910 == 0){ _throw( Null_Exception);} _temp2910->tl;});
for( 0; vs2 != 0; vs2=({ struct Cyc_List_List* _temp2904= vs2; if( _temp2904 ==
0){ _throw( Null_Exception);} _temp2904->tl;})){ if( cmp(( void*)({ struct Cyc_List_List*
_temp2905= vs; if( _temp2905 == 0){ _throw( Null_Exception);} _temp2905->hd;}),(
void*)({ struct Cyc_List_List* _temp2906= vs2; if( _temp2906 == 0){ _throw(
Null_Exception);} _temp2906->hd;})) == 0){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2908= msg; struct _tagged_string _temp2909= a2string(( void*)({
struct Cyc_List_List* _temp2907= vs; if( _temp2907 == 0){ _throw( Null_Exception);}
_temp2907->hd;})); xprintf("%.*s: %.*s", _temp2908.last_plus_one - _temp2908.curr,
_temp2908.curr, _temp2909.last_plus_one - _temp2909.curr, _temp2909.curr);}));}}}}
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
tvs, loc, Cyc_Tcutil_tvar2string,( struct _tagged_string)({ char* _temp2911=(
char*)"duplicate type variable"; struct _tagged_string _temp2912; _temp2912.curr=
_temp2911; _temp2912.base= _temp2911; _temp2912.last_plus_one= _temp2911 + 24;
_temp2912;}));} struct _tuple15{ struct Cyc_Absyn_Structfield* f1; int f2; } ;
struct _tuple16{ struct Cyc_List_List* f1; void* f2; } ; struct _tuple17{ struct
Cyc_Absyn_Structfield* f1; void* f2; } ; struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* sdfields){ struct Cyc_List_List* fields= 0;{ struct
Cyc_List_List* sd_fields= sdfields; for( 0; sd_fields != 0; sd_fields=({ struct
Cyc_List_List* _temp2913= sd_fields; if( _temp2913 == 0){ _throw( Null_Exception);}
_temp2913->tl;})){ if( Cyc_String_strcmp(*(( struct Cyc_Absyn_Structfield*)({
struct Cyc_List_List* _temp2914= sd_fields; if( _temp2914 == 0){ _throw(
Null_Exception);} _temp2914->hd;}))->name,( struct _tagged_string)({ char*
_temp2915=( char*)""; struct _tagged_string _temp2916; _temp2916.curr= _temp2915;
_temp2916.base= _temp2915; _temp2916.last_plus_one= _temp2915 + 1; _temp2916;}))
!= 0){ fields=({ struct Cyc_List_List* _temp2917=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2917->hd=( void*)({ struct
_tuple15* _temp2918=( struct _tuple15*) GC_malloc( sizeof( struct _tuple15));
_temp2918->f1=( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp2919=
sd_fields; if( _temp2919 == 0){ _throw( Null_Exception);} _temp2919->hd;});
_temp2918->f2= 0; _temp2918;}); _temp2917->tl= fields; _temp2917;});}}} fields=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( fields);{
struct Cyc_List_List* ans= 0; for( 0; des != 0; des=({ struct Cyc_List_List*
_temp2920= des; if( _temp2920 == 0){ _throw( Null_Exception);} _temp2920->tl;})){
struct _tuple16 _temp2924; void* _temp2925; struct Cyc_List_List* _temp2927;
struct _tuple16* _temp2922=( struct _tuple16*)({ struct Cyc_List_List* _temp2921=
des; if( _temp2921 == 0){ _throw( Null_Exception);} _temp2921->hd;}); _temp2924=*
_temp2922; _LL2928: _temp2927= _temp2924.f1; goto _LL2926; _LL2926: _temp2925=
_temp2924.f2; goto _LL2923; _LL2923: if( _temp2927 == 0){ struct Cyc_List_List*
fields2= fields; for( 0; fields2 != 0; fields2=({ struct Cyc_List_List*
_temp2929= fields2; if( _temp2929 == 0){ _throw( Null_Exception);} _temp2929->tl;})){
if( !(*(( struct _tuple15*)({ struct Cyc_List_List* _temp2930= fields2; if(
_temp2930 == 0){ _throw( Null_Exception);} _temp2930->hd;}))).f2){(*(( struct
_tuple15*)({ struct Cyc_List_List* _temp2931= fields2; if( _temp2931 == 0){
_throw( Null_Exception);} _temp2931->hd;}))).f2= 1;(*(( struct _tuple16*)({
struct Cyc_List_List* _temp2932= des; if( _temp2932 == 0){ _throw(
Null_Exception);} _temp2932->hd;}))).f1=({ struct Cyc_List_List* _temp2933=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2933->hd=(
void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp2934=( struct Cyc_Absyn_FieldName_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct)); _temp2934[ 0]=({ struct
Cyc_Absyn_FieldName_struct _temp2935; _temp2935.tag= Cyc_Absyn_FieldName;
_temp2935.f1=((*(( struct _tuple15*)({ struct Cyc_List_List* _temp2936= fields2;
if( _temp2936 == 0){ _throw( Null_Exception);} _temp2936->hd;}))).f1)->name;
_temp2935;}); _temp2934;})); _temp2933->tl= 0; _temp2933;}); ans=({ struct Cyc_List_List*
_temp2937=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp2937->hd=( void*)({ struct _tuple17* _temp2938=( struct _tuple17*)
_region_malloc( rgn, sizeof( struct _tuple17)); _temp2938->f1=(*(( struct
_tuple15*)({ struct Cyc_List_List* _temp2939= fields2; if( _temp2939 == 0){
_throw( Null_Exception);} _temp2939->hd;}))).f1; _temp2938->f2= _temp2925;
_temp2938;}); _temp2937->tl= ans; _temp2937;}); break;}} if( fields2 == 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp2940=( char*)"too many arguments to struct";
struct _tagged_string _temp2941; _temp2941.curr= _temp2940; _temp2941.base=
_temp2940; _temp2941.last_plus_one= _temp2940 + 29; _temp2941;}));}} else{ if(({
struct Cyc_List_List* _temp2942= _temp2927; if( _temp2942 == 0){ _throw(
Null_Exception);} _temp2942->tl;}) != 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp2943=( char*)"multiple designators are not supported";
struct _tagged_string _temp2944; _temp2944.curr= _temp2943; _temp2944.base=
_temp2943; _temp2944.last_plus_one= _temp2943 + 39; _temp2944;}));} else{ void*
_temp2946=( void*)({ struct Cyc_List_List* _temp2945= _temp2927; if( _temp2945
== 0){ _throw( Null_Exception);} _temp2945->hd;}); struct Cyc_Absyn_Exp*
_temp2952; struct _tagged_string* _temp2954; _LL2948: if(*(( int*) _temp2946) ==
Cyc_Absyn_ArrayElement){ _LL2953: _temp2952=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayElement_struct*) _temp2946)->f1; goto _LL2949;} else{ goto
_LL2950;} _LL2950: if(*(( int*) _temp2946) == Cyc_Absyn_FieldName){ _LL2955:
_temp2954=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp2946)->f1; goto _LL2951;} else{ goto _LL2947;} _LL2949: Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp2956=( char*)"array designator used in argument to struct";
struct _tagged_string _temp2957; _temp2957.curr= _temp2956; _temp2957.base=
_temp2956; _temp2957.last_plus_one= _temp2956 + 44; _temp2957;})); goto _LL2947;
_LL2951: { struct Cyc_List_List* fields2= fields; for( 0; fields2 != 0; fields2=({
struct Cyc_List_List* _temp2958= fields2; if( _temp2958 == 0){ _throw(
Null_Exception);} _temp2958->tl;})){ if( Cyc_String_zstrptrcmp( _temp2954,((*((
struct _tuple15*)({ struct Cyc_List_List* _temp2959= fields2; if( _temp2959 == 0){
_throw( Null_Exception);} _temp2959->hd;}))).f1)->name) == 0){ if((*(( struct
_tuple15*)({ struct Cyc_List_List* _temp2960= fields2; if( _temp2960 == 0){
_throw( Null_Exception);} _temp2960->hd;}))).f2){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2961=* _temp2954; xprintf("field %.*s has already been used as an argument",
_temp2961.last_plus_one - _temp2961.curr, _temp2961.curr);}));}(*(( struct
_tuple15*)({ struct Cyc_List_List* _temp2962= fields2; if( _temp2962 == 0){
_throw( Null_Exception);} _temp2962->hd;}))).f2= 1; ans=({ struct Cyc_List_List*
_temp2963=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp2963->hd=( void*)({ struct _tuple17* _temp2964=( struct _tuple17*)
_region_malloc( rgn, sizeof( struct _tuple17)); _temp2964->f1=(*(( struct
_tuple15*)({ struct Cyc_List_List* _temp2965= fields2; if( _temp2965 == 0){
_throw( Null_Exception);} _temp2965->hd;}))).f1; _temp2964->f2= _temp2925;
_temp2964;}); _temp2963->tl= ans; _temp2963;}); break;}} if( fields2 == 0){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2966=* _temp2954; xprintf("bad field designator %.*s",
_temp2966.last_plus_one - _temp2966.curr, _temp2966.curr);}));} goto _LL2947;}
_LL2947:;}}} for( 0; fields != 0; fields=({ struct Cyc_List_List* _temp2967=
fields; if( _temp2967 == 0){ _throw( Null_Exception);} _temp2967->tl;})){ if( !(*((
struct _tuple15*)({ struct Cyc_List_List* _temp2968= fields; if( _temp2968 == 0){
_throw( Null_Exception);} _temp2968->hd;}))).f2){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp2969=( char*)"too few arguments to struct"; struct
_tagged_string _temp2970; _temp2970.curr= _temp2969; _temp2970.base= _temp2969;
_temp2970.last_plus_one= _temp2969 + 28; _temp2970;})); break;}} return(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( ans);}} int Cyc_Tcutil_is_tagged_pointer_typ(
void* t){ void* _temp2971= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp2977; struct Cyc_Absyn_Conref* _temp2979; struct Cyc_Absyn_Tqual _temp2981;
struct Cyc_Absyn_Conref* _temp2983; void* _temp2985; void* _temp2987; _LL2973:
if(( unsigned int) _temp2971 > 4u?*(( int*) _temp2971) == Cyc_Absyn_PointerType:
0){ _LL2978: _temp2977=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp2971)->f1; _LL2988: _temp2987=( void*) _temp2977.elt_typ; goto _LL2986;
_LL2986: _temp2985=( void*) _temp2977.rgn_typ; goto _LL2984; _LL2984: _temp2983=(
struct Cyc_Absyn_Conref*) _temp2977.nullable; goto _LL2982; _LL2982: _temp2981=(
struct Cyc_Absyn_Tqual) _temp2977.tq; goto _LL2980; _LL2980: _temp2979=( struct
Cyc_Absyn_Conref*) _temp2977.bounds; goto _LL2974;} else{ goto _LL2975;} _LL2975:
goto _LL2976; _LL2974: { struct Cyc_Absyn_Conref* b=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp2979); void*
_temp2989=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)( b))->v; void* _temp2997; _LL2991: if(( unsigned int)
_temp2989 > 1u?*(( int*) _temp2989) == Cyc_Absyn_Eq_constr: 0){ _LL2998:
_temp2997=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2989)->f1; if(
_temp2997 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL2992;} else{ goto _LL2993;}}
else{ goto _LL2993;} _LL2993: if( _temp2989 ==( void*) Cyc_Absyn_No_constr){
goto _LL2994;} else{ goto _LL2995;} _LL2995: goto _LL2996; _LL2992: return 1;
_LL2994:( void*)( b->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp2999=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2999[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp3000; _temp3000.tag= Cyc_Absyn_Eq_constr;
_temp3000.f1=( void*)(( void*) Cyc_Absyn_Unknown_b); _temp3000;}); _temp2999;})));
return 1; _LL2996: return 0; _LL2990:;} _LL2976: return 0; _LL2972:;} struct
_tuple6 Cyc_Tcutil_addressof_props( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e){ struct _tuple6 bogus_ans=({ struct _tuple6 _temp3245; _temp3245.f1= 0;
_temp3245.f2=( void*) Cyc_Absyn_HeapRgn; _temp3245;}); void* _temp3001=( void*)
e->r; void* _temp3015; struct _tuple1* _temp3017; struct _tagged_string*
_temp3019; struct Cyc_Absyn_Exp* _temp3021; struct _tagged_string* _temp3023;
struct Cyc_Absyn_Exp* _temp3025; struct Cyc_Absyn_Exp* _temp3027; struct Cyc_Absyn_Exp*
_temp3029; struct Cyc_Absyn_Exp* _temp3031; _LL3003: if(*(( int*) _temp3001) ==
Cyc_Absyn_Var_e){ _LL3018: _temp3017=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*)
_temp3001)->f1; goto _LL3016; _LL3016: _temp3015=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp3001)->f2; goto _LL3004;} else{ goto _LL3005;} _LL3005: if(*(( int*)
_temp3001) == Cyc_Absyn_StructMember_e){ _LL3022: _temp3021=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp3001)->f1; goto _LL3020; _LL3020:
_temp3019=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp3001)->f2; goto _LL3006;} else{ goto _LL3007;} _LL3007: if(*(( int*)
_temp3001) == Cyc_Absyn_StructArrow_e){ _LL3026: _temp3025=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp3001)->f1; goto _LL3024; _LL3024:
_temp3023=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp3001)->f2; goto _LL3008;} else{ goto _LL3009;} _LL3009: if(*(( int*)
_temp3001) == Cyc_Absyn_Deref_e){ _LL3028: _temp3027=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Deref_e_struct*) _temp3001)->f1; goto _LL3010;} else{ goto
_LL3011;} _LL3011: if(*(( int*) _temp3001) == Cyc_Absyn_Subscript_e){ _LL3032:
_temp3031=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp3001)->f1; goto _LL3030; _LL3030: _temp3029=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp3001)->f2; goto _LL3012;} else{ goto
_LL3013;} _LL3013: goto _LL3014; _LL3004: { void* _temp3033= _temp3015; struct
Cyc_Absyn_Fndecl* _temp3047; struct Cyc_Absyn_Vardecl* _temp3049; struct Cyc_Absyn_Vardecl*
_temp3051; struct Cyc_Absyn_Vardecl* _temp3053; struct Cyc_Absyn_Vardecl*
_temp3055; _LL3035: if( _temp3033 ==( void*) Cyc_Absyn_Unresolved_b){ goto
_LL3036;} else{ goto _LL3037;} _LL3037: if(( unsigned int) _temp3033 > 1u?*((
int*) _temp3033) == Cyc_Absyn_Funname_b: 0){ _LL3048: _temp3047=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Funname_b_struct*) _temp3033)->f1; goto _LL3038;} else{ goto
_LL3039;} _LL3039: if(( unsigned int) _temp3033 > 1u?*(( int*) _temp3033) == Cyc_Absyn_Global_b:
0){ _LL3050: _temp3049=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Global_b_struct*)
_temp3033)->f1; goto _LL3040;} else{ goto _LL3041;} _LL3041: if(( unsigned int)
_temp3033 > 1u?*(( int*) _temp3033) == Cyc_Absyn_Local_b: 0){ _LL3052: _temp3051=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Local_b_struct*) _temp3033)->f1;
goto _LL3042;} else{ goto _LL3043;} _LL3043: if(( unsigned int) _temp3033 > 1u?*((
int*) _temp3033) == Cyc_Absyn_Pat_b: 0){ _LL3054: _temp3053=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Pat_b_struct*) _temp3033)->f1; goto _LL3044;} else{ goto
_LL3045;} _LL3045: if(( unsigned int) _temp3033 > 1u?*(( int*) _temp3033) == Cyc_Absyn_Param_b:
0){ _LL3056: _temp3055=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Param_b_struct*)
_temp3033)->f1; goto _LL3046;} else{ goto _LL3034;} _LL3036: return bogus_ans;
_LL3038: return({ struct _tuple6 _temp3057; _temp3057.f1= 1; _temp3057.f2=( void*)
Cyc_Absyn_HeapRgn; _temp3057;}); _LL3040: { void* _temp3059= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp3058= e->topt; if( _temp3058 == 0){ _throw(
Null_Exception);} _temp3058->v;})); struct Cyc_Absyn_Exp* _temp3065; struct Cyc_Absyn_Tqual
_temp3067; void* _temp3069; _LL3061: if(( unsigned int) _temp3059 > 4u?*(( int*)
_temp3059) == Cyc_Absyn_ArrayType: 0){ _LL3070: _temp3069=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp3059)->f1; goto _LL3068; _LL3068: _temp3067=( struct Cyc_Absyn_Tqual)((
struct Cyc_Absyn_ArrayType_struct*) _temp3059)->f2; goto _LL3066; _LL3066:
_temp3065=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp3059)->f3; goto _LL3062;} else{ goto _LL3063;} _LL3063: goto _LL3064;
_LL3062: return({ struct _tuple6 _temp3071; _temp3071.f1= 1; _temp3071.f2=( void*)
Cyc_Absyn_HeapRgn; _temp3071;}); _LL3064: return({ struct _tuple6 _temp3072;
_temp3072.f1=( _temp3049->tq).q_const; _temp3072.f2=( void*) Cyc_Absyn_HeapRgn;
_temp3072;}); _LL3060:;} _LL3042: { void* _temp3074= Cyc_Tcutil_compress(( void*)({
struct Cyc_Core_Opt* _temp3073= e->topt; if( _temp3073 == 0){ _throw(
Null_Exception);} _temp3073->v;})); struct Cyc_Absyn_Exp* _temp3080; struct Cyc_Absyn_Tqual
_temp3082; void* _temp3084; _LL3076: if(( unsigned int) _temp3074 > 4u?*(( int*)
_temp3074) == Cyc_Absyn_ArrayType: 0){ _LL3085: _temp3084=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp3074)->f1; goto _LL3083; _LL3083: _temp3082=( struct Cyc_Absyn_Tqual)((
struct Cyc_Absyn_ArrayType_struct*) _temp3074)->f2; goto _LL3081; _LL3081:
_temp3080=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp3074)->f3; goto _LL3077;} else{ goto _LL3078;} _LL3078: goto _LL3079;
_LL3077: return({ struct _tuple6 _temp3086; _temp3086.f1= 1; _temp3086.f2=( void*)({
struct Cyc_Core_Opt* _temp3087= _temp3051->rgn; if( _temp3087 == 0){ _throw(
Null_Exception);} _temp3087->v;}); _temp3086;}); _LL3079: return({ struct
_tuple6 _temp3088; _temp3088.f1=( _temp3051->tq).q_const; _temp3088.f2=( void*)({
struct Cyc_Core_Opt* _temp3089= _temp3051->rgn; if( _temp3089 == 0){ _throw(
Null_Exception);} _temp3089->v;}); _temp3088;}); _LL3075:;} _LL3044: _temp3055=
_temp3053; goto _LL3046; _LL3046: return({ struct _tuple6 _temp3090; _temp3090.f1=(
_temp3055->tq).q_const; _temp3090.f2=( void*)({ struct Cyc_Core_Opt* _temp3091=
_temp3055->rgn; if( _temp3091 == 0){ _throw( Null_Exception);} _temp3091->v;});
_temp3090;}); _LL3034:;} _LL3006: { void* _temp3093= Cyc_Tcutil_compress(( void*)({
struct Cyc_Core_Opt* _temp3092= _temp3021->topt; if( _temp3092 == 0){ _throw(
Null_Exception);} _temp3092->v;})); struct Cyc_List_List* _temp3105; struct Cyc_List_List*
_temp3107; struct Cyc_Absyn_Structdecl** _temp3109; struct Cyc_Absyn_Structdecl*
_temp3111; struct Cyc_List_List* _temp3112; struct _tuple1* _temp3114; struct
Cyc_Absyn_Uniondecl** _temp3116; struct Cyc_Absyn_Uniondecl* _temp3118; struct
Cyc_List_List* _temp3119; struct _tuple1* _temp3121; _LL3095: if(( unsigned int)
_temp3093 > 4u?*(( int*) _temp3093) == Cyc_Absyn_AnonStructType: 0){ _LL3106:
_temp3105=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp3093)->f1; goto _LL3096;} else{ goto _LL3097;} _LL3097: if(( unsigned int)
_temp3093 > 4u?*(( int*) _temp3093) == Cyc_Absyn_AnonUnionType: 0){ _LL3108:
_temp3107=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp3093)->f1; goto _LL3098;} else{ goto _LL3099;} _LL3099: if(( unsigned int)
_temp3093 > 4u?*(( int*) _temp3093) == Cyc_Absyn_StructType: 0){ _LL3115:
_temp3114=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp3093)->f1;
goto _LL3113; _LL3113: _temp3112=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp3093)->f2; goto _LL3110; _LL3110: _temp3109=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp3093)->f3; if( _temp3109 == 0){ goto
_LL3101;} else{ _temp3111=* _temp3109; goto _LL3100;}} else{ goto _LL3101;}
_LL3101: if(( unsigned int) _temp3093 > 4u?*(( int*) _temp3093) == Cyc_Absyn_UnionType:
0){ _LL3122: _temp3121=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp3093)->f1; goto _LL3120; _LL3120: _temp3119=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnionType_struct*) _temp3093)->f2; goto _LL3117; _LL3117:
_temp3116=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp3093)->f3; if( _temp3116 == 0){ goto _LL3103;} else{ _temp3118=* _temp3116;
goto _LL3102;}} else{ goto _LL3103;} _LL3103: goto _LL3104; _LL3096: _temp3107=
_temp3105; goto _LL3098; _LL3098: { struct Cyc_Absyn_Structfield* finfo= Cyc_Absyn_lookup_field(
_temp3107, _temp3019); if( finfo != 0?({ struct Cyc_Absyn_Structfield* _temp3123=
finfo; if( _temp3123 == 0){ _throw( Null_Exception);} _temp3123->width;}) != 0:
0){ return({ struct _tuple6 _temp3124; _temp3124.f1=({ struct Cyc_Absyn_Structfield*
_temp3125= finfo; if( _temp3125 == 0){ _throw( Null_Exception);} _temp3125->tq;}).q_const;
_temp3124.f2=( Cyc_Tcutil_addressof_props( te, _temp3021)).f2; _temp3124;});}
return bogus_ans;} _LL3100: { struct Cyc_Absyn_Structfield* finfo= Cyc_Absyn_lookup_struct_field(
_temp3111, _temp3019); if( finfo != 0?({ struct Cyc_Absyn_Structfield* _temp3126=
finfo; if( _temp3126 == 0){ _throw( Null_Exception);} _temp3126->width;}) != 0:
0){ return({ struct _tuple6 _temp3127; _temp3127.f1=({ struct Cyc_Absyn_Structfield*
_temp3128= finfo; if( _temp3128 == 0){ _throw( Null_Exception);} _temp3128->tq;}).q_const;
_temp3127.f2=( Cyc_Tcutil_addressof_props( te, _temp3021)).f2; _temp3127;});}
return bogus_ans;} _LL3102: { struct Cyc_Absyn_Structfield* finfo= Cyc_Absyn_lookup_union_field(
_temp3118, _temp3019); if( finfo != 0){ return({ struct _tuple6 _temp3129;
_temp3129.f1=({ struct Cyc_Absyn_Structfield* _temp3130= finfo; if( _temp3130 ==
0){ _throw( Null_Exception);} _temp3130->tq;}).q_const; _temp3129.f2=( Cyc_Tcutil_addressof_props(
te, _temp3021)).f2; _temp3129;});} goto _LL3104;} _LL3104: return bogus_ans;
_LL3094:;} _LL3008: { void* _temp3132= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp3131= _temp3025->topt; if( _temp3131 == 0){ _throw( Null_Exception);}
_temp3131->v;})); struct Cyc_Absyn_PtrInfo _temp3138; struct Cyc_Absyn_Conref*
_temp3140; struct Cyc_Absyn_Tqual _temp3142; struct Cyc_Absyn_Conref* _temp3144;
void* _temp3146; void* _temp3148; _LL3134: if(( unsigned int) _temp3132 > 4u?*((
int*) _temp3132) == Cyc_Absyn_PointerType: 0){ _LL3139: _temp3138=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp3132)->f1; _LL3149: _temp3148=( void*)
_temp3138.elt_typ; goto _LL3147; _LL3147: _temp3146=( void*) _temp3138.rgn_typ;
goto _LL3145; _LL3145: _temp3144=( struct Cyc_Absyn_Conref*) _temp3138.nullable;
goto _LL3143; _LL3143: _temp3142=( struct Cyc_Absyn_Tqual) _temp3138.tq; goto
_LL3141; _LL3141: _temp3140=( struct Cyc_Absyn_Conref*) _temp3138.bounds; goto
_LL3135;} else{ goto _LL3136;} _LL3136: goto _LL3137; _LL3135: { void* _temp3150=
Cyc_Tcutil_compress( _temp3148); struct Cyc_List_List* _temp3162; struct Cyc_List_List*
_temp3164; struct Cyc_Absyn_Structdecl** _temp3166; struct Cyc_Absyn_Structdecl*
_temp3168; struct Cyc_List_List* _temp3169; struct _tuple1* _temp3171; struct
Cyc_Absyn_Uniondecl** _temp3173; struct Cyc_Absyn_Uniondecl* _temp3175; struct
Cyc_List_List* _temp3176; struct _tuple1* _temp3178; _LL3152: if(( unsigned int)
_temp3150 > 4u?*(( int*) _temp3150) == Cyc_Absyn_AnonStructType: 0){ _LL3163:
_temp3162=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp3150)->f1; goto _LL3153;} else{ goto _LL3154;} _LL3154: if(( unsigned int)
_temp3150 > 4u?*(( int*) _temp3150) == Cyc_Absyn_AnonUnionType: 0){ _LL3165:
_temp3164=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp3150)->f1; goto _LL3155;} else{ goto _LL3156;} _LL3156: if(( unsigned int)
_temp3150 > 4u?*(( int*) _temp3150) == Cyc_Absyn_StructType: 0){ _LL3172:
_temp3171=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp3150)->f1;
goto _LL3170; _LL3170: _temp3169=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp3150)->f2; goto _LL3167; _LL3167: _temp3166=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp3150)->f3; if( _temp3166 == 0){ goto
_LL3158;} else{ _temp3168=* _temp3166; goto _LL3157;}} else{ goto _LL3158;}
_LL3158: if(( unsigned int) _temp3150 > 4u?*(( int*) _temp3150) == Cyc_Absyn_UnionType:
0){ _LL3179: _temp3178=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp3150)->f1; goto _LL3177; _LL3177: _temp3176=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnionType_struct*) _temp3150)->f2; goto _LL3174; _LL3174:
_temp3173=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp3150)->f3; if( _temp3173 == 0){ goto _LL3160;} else{ _temp3175=* _temp3173;
goto _LL3159;}} else{ goto _LL3160;} _LL3160: goto _LL3161; _LL3153: _temp3164=
_temp3162; goto _LL3155; _LL3155: { struct Cyc_Absyn_Structfield* finfo= Cyc_Absyn_lookup_field(
_temp3164, _temp3023); if( finfo != 0?({ struct Cyc_Absyn_Structfield* _temp3180=
finfo; if( _temp3180 == 0){ _throw( Null_Exception);} _temp3180->width;}) != 0:
0){ return({ struct _tuple6 _temp3181; _temp3181.f1=({ struct Cyc_Absyn_Structfield*
_temp3182= finfo; if( _temp3182 == 0){ _throw( Null_Exception);} _temp3182->tq;}).q_const;
_temp3181.f2= _temp3146; _temp3181;});} return bogus_ans;} _LL3157: { struct Cyc_Absyn_Structfield*
finfo= Cyc_Absyn_lookup_struct_field( _temp3168, _temp3023); if( finfo != 0?({
struct Cyc_Absyn_Structfield* _temp3183= finfo; if( _temp3183 == 0){ _throw(
Null_Exception);} _temp3183->width;}) != 0: 0){ return({ struct _tuple6
_temp3184; _temp3184.f1=({ struct Cyc_Absyn_Structfield* _temp3185= finfo; if(
_temp3185 == 0){ _throw( Null_Exception);} _temp3185->tq;}).q_const; _temp3184.f2=
_temp3146; _temp3184;});} return bogus_ans;} _LL3159: { struct Cyc_Absyn_Structfield*
finfo= Cyc_Absyn_lookup_union_field( _temp3175, _temp3023); if( finfo != 0){
return({ struct _tuple6 _temp3186; _temp3186.f1=({ struct Cyc_Absyn_Structfield*
_temp3187= finfo; if( _temp3187 == 0){ _throw( Null_Exception);} _temp3187->tq;}).q_const;
_temp3186.f2= _temp3146; _temp3186;});} return bogus_ans;} _LL3161: return
bogus_ans; _LL3151:;} _LL3137: return bogus_ans; _LL3133:;} _LL3010: { void*
_temp3189= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp3188=
_temp3027->topt; if( _temp3188 == 0){ _throw( Null_Exception);} _temp3188->v;}));
struct Cyc_Absyn_PtrInfo _temp3195; struct Cyc_Absyn_Conref* _temp3197; struct
Cyc_Absyn_Tqual _temp3199; struct Cyc_Absyn_Conref* _temp3201; void* _temp3203;
void* _temp3205; _LL3191: if(( unsigned int) _temp3189 > 4u?*(( int*) _temp3189)
== Cyc_Absyn_PointerType: 0){ _LL3196: _temp3195=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp3189)->f1; _LL3206: _temp3205=( void*)
_temp3195.elt_typ; goto _LL3204; _LL3204: _temp3203=( void*) _temp3195.rgn_typ;
goto _LL3202; _LL3202: _temp3201=( struct Cyc_Absyn_Conref*) _temp3195.nullable;
goto _LL3200; _LL3200: _temp3199=( struct Cyc_Absyn_Tqual) _temp3195.tq; goto
_LL3198; _LL3198: _temp3197=( struct Cyc_Absyn_Conref*) _temp3195.bounds; goto
_LL3192;} else{ goto _LL3193;} _LL3193: goto _LL3194; _LL3192: return({ struct
_tuple6 _temp3207; _temp3207.f1= _temp3199.q_const; _temp3207.f2= _temp3203;
_temp3207;}); _LL3194: return bogus_ans; _LL3190:;} _LL3012: { void* t= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp3242= _temp3031->topt; if( _temp3242 == 0){
_throw( Null_Exception);} _temp3242->v;})); void* _temp3208= t; struct Cyc_Absyn_Exp*
_temp3218; struct Cyc_Absyn_Tqual _temp3220; void* _temp3222; struct Cyc_List_List*
_temp3224; struct Cyc_Absyn_PtrInfo _temp3226; struct Cyc_Absyn_Conref*
_temp3228; struct Cyc_Absyn_Tqual _temp3230; struct Cyc_Absyn_Conref* _temp3232;
void* _temp3234; void* _temp3236; _LL3210: if(( unsigned int) _temp3208 > 4u?*((
int*) _temp3208) == Cyc_Absyn_ArrayType: 0){ _LL3223: _temp3222=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3208)->f1; goto _LL3221; _LL3221:
_temp3220=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp3208)->f2; goto _LL3219; _LL3219: _temp3218=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3208)->f3; goto _LL3211;} else{ goto
_LL3212;} _LL3212: if(( unsigned int) _temp3208 > 4u?*(( int*) _temp3208) == Cyc_Absyn_TupleType:
0){ _LL3225: _temp3224=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp3208)->f1; goto _LL3213;} else{ goto _LL3214;} _LL3214: if(( unsigned int)
_temp3208 > 4u?*(( int*) _temp3208) == Cyc_Absyn_PointerType: 0){ _LL3227:
_temp3226=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp3208)->f1; _LL3237: _temp3236=( void*) _temp3226.elt_typ; goto _LL3235;
_LL3235: _temp3234=( void*) _temp3226.rgn_typ; goto _LL3233; _LL3233: _temp3232=(
struct Cyc_Absyn_Conref*) _temp3226.nullable; goto _LL3231; _LL3231: _temp3230=(
struct Cyc_Absyn_Tqual) _temp3226.tq; goto _LL3229; _LL3229: _temp3228=( struct
Cyc_Absyn_Conref*) _temp3226.bounds; goto _LL3215;} else{ goto _LL3216;} _LL3216:
goto _LL3217; _LL3211: return({ struct _tuple6 _temp3238; _temp3238.f1=
_temp3220.q_const; _temp3238.f2=( Cyc_Tcutil_addressof_props( te, _temp3031)).f2;
_temp3238;}); _LL3213: { struct _tuple4* finfo= Cyc_Absyn_lookup_tuple_field(
_temp3224,( int) Cyc_Evexp_eval_const_uint_exp( _temp3029)); if( finfo != 0){
return({ struct _tuple6 _temp3239; _temp3239.f1=(({ struct _tuple4* _temp3240=
finfo; if( _temp3240 == 0){ _throw( Null_Exception);}* _temp3240;}).f1).q_const;
_temp3239.f2=( Cyc_Tcutil_addressof_props( te, _temp3031)).f2; _temp3239;});}
return bogus_ans;} _LL3215: return({ struct _tuple6 _temp3241; _temp3241.f1=
_temp3230.q_const; _temp3241.f2= _temp3234; _temp3241;}); _LL3217: return
bogus_ans; _LL3209:;} _LL3014: Cyc_Tcutil_terr( e->loc,( struct _tagged_string)({
char* _temp3243=( char*)"unary & applied to non-lvalue"; struct _tagged_string
_temp3244; _temp3244.curr= _temp3243; _temp3244.base= _temp3243; _temp3244.last_plus_one=
_temp3243 + 30; _temp3244;})); return bogus_ans; _LL3002:;} void* Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv* te, void* e_typ, struct Cyc_Absyn_Exp* e){ void*
_temp3246= Cyc_Tcutil_compress( e_typ); struct Cyc_Absyn_Exp* _temp3252; struct
Cyc_Absyn_Tqual _temp3254; void* _temp3256; _LL3248: if(( unsigned int)
_temp3246 > 4u?*(( int*) _temp3246) == Cyc_Absyn_ArrayType: 0){ _LL3257:
_temp3256=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3246)->f1; goto
_LL3255; _LL3255: _temp3254=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp3246)->f2; goto _LL3253; _LL3253: _temp3252=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3246)->f3; goto _LL3249;} else{ goto
_LL3250;} _LL3250: goto _LL3251; _LL3249: { void* _temp3260; int _temp3262;
struct _tuple6 _temp3258= Cyc_Tcutil_addressof_props( te, e); _LL3263: _temp3262=
_temp3258.f1; goto _LL3261; _LL3261: _temp3260= _temp3258.f2; goto _LL3259;
_LL3259: return Cyc_Absyn_atb_typ( _temp3256, _temp3260, _temp3254,( void*)({
struct Cyc_Absyn_Upper_b_struct* _temp3264=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp3264[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp3265; _temp3265.tag= Cyc_Absyn_Upper_b; _temp3265.f1=
e; _temp3265;}); _temp3264;}));} _LL3251: return e_typ; _LL3247:;} void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment* loc, unsigned int i, struct Cyc_Absyn_Conref* b){ b=((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
b);{ void* _temp3266=( void*) b->v; void* _temp3274; void* _temp3276; struct Cyc_Absyn_Exp*
_temp3278; _LL3268: if(( unsigned int) _temp3266 > 1u?*(( int*) _temp3266) ==
Cyc_Absyn_Eq_constr: 0){ _LL3275: _temp3274=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3266)->f1; if( _temp3274 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL3269;}
else{ goto _LL3270;}} else{ goto _LL3270;} _LL3270: if(( unsigned int) _temp3266
> 1u?*(( int*) _temp3266) == Cyc_Absyn_Eq_constr: 0){ _LL3277: _temp3276=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp3266)->f1; if(( unsigned int) _temp3276
> 1u?*(( int*) _temp3276) == Cyc_Absyn_Upper_b: 0){ _LL3279: _temp3278=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp3276)->f1; goto _LL3271;}
else{ goto _LL3272;}} else{ goto _LL3272;} _LL3272: goto _LL3273; _LL3269:
return; _LL3271: if( Cyc_Evexp_eval_const_uint_exp( _temp3278) <= i){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp3280=( char*)"dereference is out of bounds";
struct _tagged_string _temp3281; _temp3281.curr= _temp3280; _temp3281.base=
_temp3280; _temp3281.last_plus_one= _temp3280 + 29; _temp3281;}));} return;
_LL3273:( void*)( b->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp3282=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp3282[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp3283; _temp3283.tag= Cyc_Absyn_Eq_constr;
_temp3283.f1=( void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp3284=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp3284[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp3285; _temp3285.tag= Cyc_Absyn_Upper_b;
_temp3285.f1= Cyc_Absyn_uint_exp( i + 1, 0); _temp3285;}); _temp3284;}));
_temp3283;}); _temp3282;}))); return; _LL3267:;}} void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Conref* b){ Cyc_Tcutil_check_bound(
loc, 0, b);} int Cyc_Tcutil_bits_only( void* t){ void* _temp3286= Cyc_Tcutil_compress(
t); void* _temp3312; void* _temp3314; struct Cyc_Absyn_Enumdecl* _temp3316;
struct _tuple1* _temp3318; struct Cyc_Absyn_Exp* _temp3320; struct Cyc_Absyn_Tqual
_temp3322; void* _temp3324; struct Cyc_List_List* _temp3326; struct Cyc_Absyn_Structdecl**
_temp3328; struct Cyc_List_List* _temp3330; struct _tuple1* _temp3332; struct
Cyc_Absyn_Uniondecl** _temp3334; struct Cyc_List_List* _temp3336; struct _tuple1*
_temp3338; struct Cyc_List_List* _temp3340; struct Cyc_List_List* _temp3342;
_LL3288: if( _temp3286 ==( void*) Cyc_Absyn_VoidType){ goto _LL3289;} else{ goto
_LL3290;} _LL3290: if(( unsigned int) _temp3286 > 4u?*(( int*) _temp3286) == Cyc_Absyn_IntType:
0){ _LL3315: _temp3314=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp3286)->f1;
goto _LL3313; _LL3313: _temp3312=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp3286)->f2; goto _LL3291;} else{ goto _LL3292;} _LL3292: if( _temp3286 ==(
void*) Cyc_Absyn_FloatType){ goto _LL3293;} else{ goto _LL3294;} _LL3294: if(
_temp3286 ==( void*) Cyc_Absyn_DoubleType){ goto _LL3295;} else{ goto _LL3296;}
_LL3296: if(( unsigned int) _temp3286 > 4u?*(( int*) _temp3286) == Cyc_Absyn_EnumType:
0){ _LL3319: _temp3318=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*)
_temp3286)->f1; goto _LL3317; _LL3317: _temp3316=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_EnumType_struct*) _temp3286)->f2; goto _LL3297;} else{ goto
_LL3298;} _LL3298: if(( unsigned int) _temp3286 > 4u?*(( int*) _temp3286) == Cyc_Absyn_ArrayType:
0){ _LL3325: _temp3324=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3286)->f1;
goto _LL3323; _LL3323: _temp3322=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp3286)->f2; goto _LL3321; _LL3321: _temp3320=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3286)->f3; goto _LL3299;} else{ goto
_LL3300;} _LL3300: if(( unsigned int) _temp3286 > 4u?*(( int*) _temp3286) == Cyc_Absyn_TupleType:
0){ _LL3327: _temp3326=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp3286)->f1; goto _LL3301;} else{ goto _LL3302;} _LL3302: if(( unsigned int)
_temp3286 > 4u?*(( int*) _temp3286) == Cyc_Absyn_StructType: 0){ _LL3333:
_temp3332=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp3286)->f1;
goto _LL3331; _LL3331: _temp3330=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp3286)->f2; goto _LL3329; _LL3329: _temp3328=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp3286)->f3; goto _LL3303;} else{ goto
_LL3304;} _LL3304: if(( unsigned int) _temp3286 > 4u?*(( int*) _temp3286) == Cyc_Absyn_UnionType:
0){ _LL3339: _temp3338=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp3286)->f1; goto _LL3337; _LL3337: _temp3336=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnionType_struct*) _temp3286)->f2; goto _LL3335; _LL3335:
_temp3334=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp3286)->f3; goto _LL3305;} else{ goto _LL3306;} _LL3306: if(( unsigned int)
_temp3286 > 4u?*(( int*) _temp3286) == Cyc_Absyn_AnonStructType: 0){ _LL3341:
_temp3340=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp3286)->f1; goto _LL3307;} else{ goto _LL3308;} _LL3308: if(( unsigned int)
_temp3286 > 4u?*(( int*) _temp3286) == Cyc_Absyn_AnonUnionType: 0){ _LL3343:
_temp3342=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp3286)->f1; goto _LL3309;} else{ goto _LL3310;} _LL3310: goto _LL3311;
_LL3289: return 1; _LL3291: return 1; _LL3293: return 1; _LL3295: return 1;
_LL3297: return 1; _LL3299: return Cyc_Tcutil_bits_only( _temp3324); _LL3301:
for( 0; _temp3326 != 0; _temp3326=({ struct Cyc_List_List* _temp3344= _temp3326;
if( _temp3344 == 0){ _throw( Null_Exception);} _temp3344->tl;})){ if( ! Cyc_Tcutil_bits_only((*((
struct _tuple4*)({ struct Cyc_List_List* _temp3345= _temp3326; if( _temp3345 ==
0){ _throw( Null_Exception);} _temp3345->hd;}))).f2)){ return 0;}} return 1;
_LL3303: if( _temp3328 == 0){ return 0;}{ struct Cyc_Absyn_Structdecl* sd=({
struct Cyc_Absyn_Structdecl** _temp3352= _temp3328; if( _temp3352 == 0){ _throw(
Null_Exception);}* _temp3352;}); if( sd->fields == 0){ return 0;}{ struct
_RegionHandle _temp3346= _new_region(); struct _RegionHandle* rgn=& _temp3346;
_push_region( rgn);{ struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, sd->tvs, _temp3330);{ struct
Cyc_List_List* fs=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp3350= sd->fields;
if( _temp3350 == 0){ _throw( Null_Exception);} _temp3350->v;}); for( 0; fs != 0;
fs=({ struct Cyc_List_List* _temp3347= fs; if( _temp3347 == 0){ _throw(
Null_Exception);} _temp3347->tl;})){ if( ! Cyc_Tcutil_bits_only( Cyc_Tcutil_rsubstitute(
rgn, inst,( void*)(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp3348= fs; if( _temp3348 == 0){ _throw( Null_Exception);} _temp3348->hd;}))->type))){
int _temp3349= 0; _npop_handler( 0u); return _temp3349;}}}{ int _temp3351= 1;
_npop_handler( 0u); return _temp3351;}}; _pop_region(& _temp3346);}} _LL3305:
if( _temp3334 == 0){ return 0;}{ struct Cyc_Absyn_Uniondecl* ud=({ struct Cyc_Absyn_Uniondecl**
_temp3359= _temp3334; if( _temp3359 == 0){ _throw( Null_Exception);}* _temp3359;});
if( ud->fields == 0){ return 0;}{ struct _RegionHandle _temp3353= _new_region();
struct _RegionHandle* rgn=& _temp3353; _push_region( rgn);{ struct Cyc_List_List*
inst=(( struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct _RegionHandle*
r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn,
ud->tvs, _temp3336);{ struct Cyc_List_List* fs=( struct Cyc_List_List*)({ struct
Cyc_Core_Opt* _temp3357= ud->fields; if( _temp3357 == 0){ _throw( Null_Exception);}
_temp3357->v;}); for( 0; fs != 0; fs=({ struct Cyc_List_List* _temp3354= fs; if(
_temp3354 == 0){ _throw( Null_Exception);} _temp3354->tl;})){ if( ! Cyc_Tcutil_bits_only(
Cyc_Tcutil_rsubstitute( rgn, inst,( void*)(( struct Cyc_Absyn_Structfield*)({
struct Cyc_List_List* _temp3355= fs; if( _temp3355 == 0){ _throw( Null_Exception);}
_temp3355->hd;}))->type))){ int _temp3356= 0; _npop_handler( 0u); return
_temp3356;}}}{ int _temp3358= 1; _npop_handler( 0u); return _temp3358;}};
_pop_region(& _temp3353);}} _LL3307: _temp3342= _temp3340; goto _LL3309; _LL3309:
for( 0; _temp3342 != 0; _temp3342=({ struct Cyc_List_List* _temp3360= _temp3342;
if( _temp3360 == 0){ _throw( Null_Exception);} _temp3360->tl;})){ if( ! Cyc_Tcutil_bits_only((
void*)(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp3361=
_temp3342; if( _temp3361 == 0){ _throw( Null_Exception);} _temp3361->hd;}))->type)){
return 0;}} return 1; _LL3311: return 0; _LL3287:;} struct _tuple18{ struct Cyc_List_List*
f1; struct Cyc_Absyn_Exp* f2; } ; static int Cyc_Tcutil_cnst_exp( struct Cyc_Tcenv_Tenv*
te, int var_okay, struct Cyc_Absyn_Exp* e){ void* _temp3362=( void*) e->r; void*
_temp3400; void* _temp3402; struct Cyc_Absyn_Exp* _temp3404; struct Cyc_Absyn_Enumfield*
_temp3406; struct Cyc_Absyn_Enumdecl* _temp3408; struct _tuple1* _temp3410; void*
_temp3412; struct _tuple1* _temp3414; struct Cyc_Absyn_Exp* _temp3416; struct
Cyc_Absyn_Exp* _temp3418; struct Cyc_Absyn_Exp* _temp3420; struct Cyc_Absyn_Exp*
_temp3422; struct Cyc_Absyn_Exp* _temp3424; struct Cyc_List_List* _temp3426;
struct Cyc_Absyn_Exp* _temp3428; struct Cyc_Absyn_Exp* _temp3430; void*
_temp3432; struct Cyc_Absyn_Exp* _temp3434; struct Cyc_Absyn_Exp* _temp3436;
struct Cyc_Absyn_Exp* _temp3438; struct Cyc_Absyn_Vardecl* _temp3440; struct Cyc_List_List*
_temp3442; struct Cyc_List_List* _temp3444; void* _temp3446; struct Cyc_Absyn_Structdecl*
_temp3448; struct Cyc_List_List* _temp3450; struct Cyc_Core_Opt* _temp3452;
struct _tuple1* _temp3454; struct Cyc_List_List* _temp3456; void* _temp3458;
struct Cyc_List_List* _temp3460; struct Cyc_Absyn_Tunionfield* _temp3462; struct
Cyc_Absyn_Tuniondecl* _temp3464; struct Cyc_List_List* _temp3466; struct Cyc_Core_Opt*
_temp3468; struct Cyc_Core_Opt* _temp3470; _LL3364: if(*(( int*) _temp3362) ==
Cyc_Absyn_Const_e){ _LL3401: _temp3400=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp3362)->f1; goto _LL3365;} else{ goto _LL3366;} _LL3366: if(*(( int*)
_temp3362) == Cyc_Absyn_Sizeoftyp_e){ _LL3403: _temp3402=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp3362)->f1; goto _LL3367;} else{ goto _LL3368;} _LL3368: if(*(( int*)
_temp3362) == Cyc_Absyn_Sizeofexp_e){ _LL3405: _temp3404=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Sizeofexp_e_struct*) _temp3362)->f1; goto _LL3369;} else{ goto
_LL3370;} _LL3370: if(*(( int*) _temp3362) == Cyc_Absyn_Enum_e){ _LL3411:
_temp3410=( struct _tuple1*)(( struct Cyc_Absyn_Enum_e_struct*) _temp3362)->f1;
goto _LL3409; _LL3409: _temp3408=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp3362)->f2; goto _LL3407; _LL3407: _temp3406=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_e_struct*) _temp3362)->f3; goto _LL3371;} else{ goto
_LL3372;} _LL3372: if(*(( int*) _temp3362) == Cyc_Absyn_Var_e){ _LL3415:
_temp3414=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp3362)->f1;
goto _LL3413; _LL3413: _temp3412=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp3362)->f2; goto _LL3373;} else{ goto _LL3374;} _LL3374: if(*(( int*)
_temp3362) == Cyc_Absyn_Conditional_e){ _LL3421: _temp3420=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp3362)->f1; goto _LL3419; _LL3419:
_temp3418=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp3362)->f2; goto _LL3417; _LL3417: _temp3416=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp3362)->f3; goto _LL3375;} else{
goto _LL3376;} _LL3376: if(*(( int*) _temp3362) == Cyc_Absyn_SeqExp_e){ _LL3425:
_temp3424=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp3362)->f1; goto _LL3423; _LL3423: _temp3422=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_SeqExp_e_struct*) _temp3362)->f2; goto _LL3377;} else{ goto
_LL3378;} _LL3378: if(*(( int*) _temp3362) == Cyc_Absyn_Instantiate_e){ _LL3429:
_temp3428=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp3362)->f1; goto _LL3427; _LL3427: _temp3426=( struct Cyc_List_List*)((
struct Cyc_Absyn_Instantiate_e_struct*) _temp3362)->f2; goto _LL3379;} else{
goto _LL3380;} _LL3380: if(*(( int*) _temp3362) == Cyc_Absyn_Cast_e){ _LL3433:
_temp3432=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp3362)->f1; goto
_LL3431; _LL3431: _temp3430=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp3362)->f2; goto _LL3381;} else{ goto _LL3382;} _LL3382: if(*(( int*)
_temp3362) == Cyc_Absyn_Address_e){ _LL3435: _temp3434=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Address_e_struct*) _temp3362)->f1; goto _LL3383;} else{ goto
_LL3384;} _LL3384: if(*(( int*) _temp3362) == Cyc_Absyn_Comprehension_e){
_LL3441: _temp3440=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3362)->f1; goto _LL3439; _LL3439: _temp3438=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp3362)->f2; goto _LL3437; _LL3437:
_temp3436=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3362)->f3; goto _LL3385;} else{ goto _LL3386;} _LL3386: if(*(( int*)
_temp3362) == Cyc_Absyn_Array_e){ _LL3443: _temp3442=( struct Cyc_List_List*)((
struct Cyc_Absyn_Array_e_struct*) _temp3362)->f1; goto _LL3387;} else{ goto
_LL3388;} _LL3388: if(*(( int*) _temp3362) == Cyc_Absyn_AnonStruct_e){ _LL3447:
_temp3446=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp3362)->f1; goto
_LL3445; _LL3445: _temp3444=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp3362)->f2; goto _LL3389;} else{ goto _LL3390;} _LL3390: if(*(( int*)
_temp3362) == Cyc_Absyn_Struct_e){ _LL3455: _temp3454=( struct _tuple1*)((
struct Cyc_Absyn_Struct_e_struct*) _temp3362)->f1; goto _LL3453; _LL3453:
_temp3452=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*) _temp3362)->f2;
goto _LL3451; _LL3451: _temp3450=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp3362)->f3; goto _LL3449; _LL3449: _temp3448=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp3362)->f4; goto _LL3391;} else{ goto
_LL3392;} _LL3392: if(*(( int*) _temp3362) == Cyc_Absyn_Primop_e){ _LL3459:
_temp3458=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp3362)->f1; goto
_LL3457; _LL3457: _temp3456=( struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp3362)->f2; goto _LL3393;} else{ goto _LL3394;} _LL3394: if(*(( int*)
_temp3362) == Cyc_Absyn_Tuple_e){ _LL3461: _temp3460=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tuple_e_struct*) _temp3362)->f1; goto _LL3395;} else{ goto
_LL3396;} _LL3396: if(*(( int*) _temp3362) == Cyc_Absyn_Tunion_e){ _LL3471:
_temp3470=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp3362)->f1;
goto _LL3469; _LL3469: _temp3468=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3362)->f2; goto _LL3467; _LL3467: _temp3466=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp3362)->f3; goto _LL3465; _LL3465:
_temp3464=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3362)->f4; goto _LL3463; _LL3463: _temp3462=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp3362)->f5; goto _LL3397;} else{ goto
_LL3398;} _LL3398: goto _LL3399; _LL3365: return 1; _LL3367: return 1; _LL3369:
return Cyc_Tcutil_cnst_exp( te, 1, _temp3404); _LL3371: return 1; _LL3373: {
struct _handler_cons _temp3472; _push_handler(& _temp3472);{ void* _temp3473=(
void*) setjmp( _temp3472.handler); if( ! _temp3473){{ void* _temp3474= Cyc_Tcenv_lookup_ordinary(
te, e->loc, _temp3414); void* _temp3482; struct Cyc_Absyn_Fndecl* _temp3484;
void* _temp3486; struct Cyc_Absyn_Vardecl* _temp3488; _LL3476: if(*(( int*)
_temp3474) == Cyc_Tcenv_VarRes){ _LL3483: _temp3482=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp3474)->f1; if(( unsigned int) _temp3482 > 1u?*(( int*) _temp3482) == Cyc_Absyn_Funname_b:
0){ _LL3485: _temp3484=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Funname_b_struct*)
_temp3482)->f1; goto _LL3477;} else{ goto _LL3478;}} else{ goto _LL3478;}
_LL3478: if(*(( int*) _temp3474) == Cyc_Tcenv_VarRes){ _LL3487: _temp3486=( void*)((
struct Cyc_Tcenv_VarRes_struct*) _temp3474)->f1; if(( unsigned int) _temp3486 >
1u?*(( int*) _temp3486) == Cyc_Absyn_Global_b: 0){ _LL3489: _temp3488=( struct
Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Global_b_struct*) _temp3486)->f1; goto
_LL3479;} else{ goto _LL3480;}} else{ goto _LL3480;} _LL3480: goto _LL3481;
_LL3477: { int _temp3490= 1; _npop_handler( 0u); return _temp3490;} _LL3479: {
void* _temp3491= Cyc_Tcutil_compress(( void*) _temp3488->type); struct Cyc_Absyn_Exp*
_temp3497; struct Cyc_Absyn_Tqual _temp3499; void* _temp3501; _LL3493: if((
unsigned int) _temp3491 > 4u?*(( int*) _temp3491) == Cyc_Absyn_ArrayType: 0){
_LL3502: _temp3501=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3491)->f1;
goto _LL3500; _LL3500: _temp3499=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp3491)->f2; goto _LL3498; _LL3498: _temp3497=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3491)->f3; goto _LL3494;} else{ goto
_LL3495;} _LL3495: goto _LL3496; _LL3494: { int _temp3503= 1; _npop_handler( 0u);
return _temp3503;} _LL3496: { int _temp3504= var_okay; _npop_handler( 0u);
return _temp3504;} _LL3492:;} _LL3481: { int _temp3505= var_okay; _npop_handler(
0u); return _temp3505;} _LL3475:;}; _pop_handler();} else{ void* _temp3507=
_temp3473; _LL3509: if( _temp3507 == Cyc_Dict_Absent){ goto _LL3510;} else{ goto
_LL3511;} _LL3511: goto _LL3512; _LL3510: return 0; _LL3512:( void) _throw(
_temp3507); _LL3508:;}}} _LL3375: return( Cyc_Tcutil_cnst_exp( te, 0, _temp3420)?
Cyc_Tcutil_cnst_exp( te, 0, _temp3418): 0)? Cyc_Tcutil_cnst_exp( te, 0,
_temp3416): 0; _LL3377: return Cyc_Tcutil_cnst_exp( te, 0, _temp3424)? Cyc_Tcutil_cnst_exp(
te, 0, _temp3422): 0; _LL3379: return Cyc_Tcutil_cnst_exp( te, var_okay,
_temp3428); _LL3381: return Cyc_Tcutil_cnst_exp( te, var_okay, _temp3430);
_LL3383: return Cyc_Tcutil_cnst_exp( te, 1, _temp3434); _LL3385: return Cyc_Tcutil_cnst_exp(
te, 0, _temp3438)? Cyc_Tcutil_cnst_exp( te, 0, _temp3436): 0; _LL3387: _temp3444=
_temp3442; goto _LL3389; _LL3389: _temp3450= _temp3444; goto _LL3391; _LL3391:
for( 0; _temp3450 != 0; _temp3450=({ struct Cyc_List_List* _temp3513= _temp3450;
if( _temp3513 == 0){ _throw( Null_Exception);} _temp3513->tl;})){ if( ! Cyc_Tcutil_cnst_exp(
te, 0,(*(( struct _tuple18*)({ struct Cyc_List_List* _temp3514= _temp3450; if(
_temp3514 == 0){ _throw( Null_Exception);} _temp3514->hd;}))).f2)){ return 0;}}
return 1; _LL3393: _temp3460= _temp3456; goto _LL3395; _LL3395: _temp3466=
_temp3460; goto _LL3397; _LL3397: for( 0; _temp3466 != 0; _temp3466=({ struct
Cyc_List_List* _temp3515= _temp3466; if( _temp3515 == 0){ _throw( Null_Exception);}
_temp3515->tl;})){ if( ! Cyc_Tcutil_cnst_exp( te, 0,( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp3516= _temp3466; if( _temp3516 == 0){ _throw(
Null_Exception);} _temp3516->hd;}))){ return 0;}} return 1; _LL3399: return 0;
_LL3363:;} int Cyc_Tcutil_is_const_exp( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e){ return Cyc_Tcutil_cnst_exp( te, 0, e);}