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
extern char Cyc_Stdio_FileCloseError[ 19u]; extern char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1;
} ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; typedef struct
Cyc_List_List* Cyc_List_glist_t; typedef struct Cyc_List_List* Cyc_List_list_t;
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
Null_Exception);} _temp494->tl;})){ void* _temp496=( void*)({ struct Cyc_List_List*
_temp495= effs; if( _temp495 == 0){ _throw( Null_Exception);} _temp495->hd;});
goto _LL497; _LL497: Cyc_Tcutil_normalize_effect( _temp496);{ void* _temp498=
Cyc_Tcutil_compress( _temp496); struct Cyc_List_List* _temp504; _LL500: if((
unsigned int) _temp498 > 4u?*(( int*) _temp498) == Cyc_Absyn_JoinEff: 0){ _LL505:
_temp504=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp498)->f1;
goto _LL501;} else{ goto _LL502;} _LL502: goto _LL503; _LL501: nested_join= 1;
goto _LL499; _LL503: goto _LL499; _LL499:;}}} if( ! nested_join){ return;}{
struct Cyc_List_List* effects= 0;{ struct Cyc_List_List* effs=* _temp493; for( 0;
effs != 0; effs=({ struct Cyc_List_List* _temp506= effs; if( _temp506 == 0){
_throw( Null_Exception);} _temp506->tl;})){ void* _temp508= Cyc_Tcutil_compress((
void*)({ struct Cyc_List_List* _temp507= effs; if( _temp507 == 0){ _throw(
Null_Exception);} _temp507->hd;})); struct Cyc_List_List* _temp514; _LL510: if((
unsigned int) _temp508 > 4u?*(( int*) _temp508) == Cyc_Absyn_JoinEff: 0){ _LL515:
_temp514=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp508)->f1;
goto _LL511;} else{ goto _LL512;} _LL512: goto _LL513; _LL511: effects=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_revappend)(
_temp514, effects); goto _LL509; _LL513: effects=({ struct Cyc_List_List*
_temp516=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp516->hd=( void*) _temp508; _temp516->tl= effects; _temp516;}); goto _LL509;
_LL509:;}}* _temp493=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
effects); goto _LL486;}} _LL490: goto _LL486; _LL486:;}} struct _tuple8{ void*
f1; struct Cyc_List_List* f2; } ; static struct _tuple8* Cyc_Tcutil_get_effect_evar(
void* t){ void* _temp517= Cyc_Tcutil_compress( t); struct Cyc_List_List*
_temp525; struct Cyc_List_List _temp527; struct Cyc_List_List* _temp528; void*
_temp530; int _temp532; struct Cyc_Core_Opt* _temp534; struct Cyc_Core_Opt**
_temp536; void* _temp537; void** _temp539; _LL519: if(( unsigned int) _temp517 >
4u?*(( int*) _temp517) == Cyc_Absyn_JoinEff: 0){ _LL526: _temp525=( struct Cyc_List_List*)((
struct Cyc_Absyn_JoinEff_struct*) _temp517)->f1; if( _temp525 == 0){ goto _LL521;}
else{ _temp527=* _temp525; _LL531: _temp530=( void*) _temp527.hd; goto _LL529;
_LL529: _temp528=( struct Cyc_List_List*) _temp527.tl; goto _LL520;}} else{ goto
_LL521;} _LL521: if(( unsigned int) _temp517 > 4u?*(( int*) _temp517) == Cyc_Absyn_Evar:
0){ _LL538: _temp537=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp517)->f1;
_temp539=&(( struct Cyc_Absyn_Evar_struct*) _temp517)->f1; goto _LL535; _LL535:
_temp534=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp517)->f2;
_temp536=&(( struct Cyc_Absyn_Evar_struct*) _temp517)->f2; goto _LL533; _LL533:
_temp532=( int)(( struct Cyc_Absyn_Evar_struct*) _temp517)->f3; goto _LL522;}
else{ goto _LL523;} _LL523: goto _LL524; _LL520: { void* _temp540= Cyc_Tcutil_compress(
_temp530); int _temp546; struct Cyc_Core_Opt* _temp548; void* _temp550; _LL542:
if(( unsigned int) _temp540 > 4u?*(( int*) _temp540) == Cyc_Absyn_Evar: 0){
_LL551: _temp550=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp540)->f1; goto
_LL549; _LL549: _temp548=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp540)->f2; goto _LL547; _LL547: _temp546=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp540)->f3; goto _LL543;} else{ goto _LL544;} _LL544: goto _LL545; _LL543:
return({ struct _tuple8* _temp552=( struct _tuple8*) GC_malloc( sizeof( struct
_tuple8)); _temp552->f1= _temp530; _temp552->f2= _temp528; _temp552;}); _LL545:
return 0; _LL541:;} _LL522: if(* _temp539 !=( void*) Cyc_Absyn_EffKind){(( void(*)(
struct _tagged_string msg)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp553=( char*)"effect evar has wrong kind"; struct _tagged_string _temp554;
_temp554.curr= _temp553; _temp554.base= _temp553; _temp554.last_plus_one=
_temp553 + 27; _temp554;}));} return({ struct _tuple8* _temp555=( struct _tuple8*)
GC_malloc( sizeof( struct _tuple8)); _temp555->f1= t; _temp555->f2= 0; _temp555;});
_LL524: return 0; _LL518:;} int Cyc_Tcutil_region_in_effect( int constrain, void*
r, void* e){ r= Cyc_Tcutil_compress( r); if( r ==( void*) Cyc_Absyn_HeapRgn){
return 1;}{ void* _temp556= Cyc_Tcutil_compress( e); void* _temp566; struct Cyc_List_List*
_temp568; int _temp570; struct Cyc_Core_Opt* _temp572; struct Cyc_Core_Opt**
_temp574; void* _temp575; void** _temp577; _LL558: if(( unsigned int) _temp556 >
4u?*(( int*) _temp556) == Cyc_Absyn_AccessEff: 0){ _LL567: _temp566=( void*)((
struct Cyc_Absyn_AccessEff_struct*) _temp556)->f1; goto _LL559;} else{ goto
_LL560;} _LL560: if(( unsigned int) _temp556 > 4u?*(( int*) _temp556) == Cyc_Absyn_JoinEff:
0){ _LL569: _temp568=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*)
_temp556)->f1; goto _LL561;} else{ goto _LL562;} _LL562: if(( unsigned int)
_temp556 > 4u?*(( int*) _temp556) == Cyc_Absyn_Evar: 0){ _LL576: _temp575=( void*)((
struct Cyc_Absyn_Evar_struct*) _temp556)->f1; _temp577=&(( struct Cyc_Absyn_Evar_struct*)
_temp556)->f1; goto _LL573; _LL573: _temp572=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Evar_struct*) _temp556)->f2; _temp574=&(( struct Cyc_Absyn_Evar_struct*)
_temp556)->f2; goto _LL571; _LL571: _temp570=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp556)->f3; goto _LL563;} else{ goto _LL564;} _LL564: goto _LL565; _LL559:
if( constrain){ return Cyc_Tcutil_unify( r, _temp566);} Cyc_Tcutil_compress(
_temp566); if( r == _temp566){ return 1;}{ struct _tuple7 _temp579=({ struct
_tuple7 _temp578; _temp578.f1= r; _temp578.f2= _temp566; _temp578;}); void*
_temp585; struct Cyc_Absyn_Tvar* _temp587; void* _temp589; struct Cyc_Absyn_Tvar*
_temp591; _LL581: _LL590: _temp589= _temp579.f1; if(( unsigned int) _temp589 > 4u?*((
int*) _temp589) == Cyc_Absyn_VarType: 0){ _LL592: _temp591=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_VarType_struct*) _temp589)->f1; goto _LL586;} else{ goto _LL583;}
_LL586: _temp585= _temp579.f2; if(( unsigned int) _temp585 > 4u?*(( int*)
_temp585) == Cyc_Absyn_VarType: 0){ _LL588: _temp587=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_VarType_struct*) _temp585)->f1; goto _LL582;} else{ goto _LL583;}
_LL583: goto _LL584; _LL582: return Cyc_Absyn_tvar_cmp( _temp591, _temp587) == 0;
_LL584: return 0; _LL580:;} _LL561: for( 0; _temp568 != 0; _temp568=({ struct
Cyc_List_List* _temp593= _temp568; if( _temp593 == 0){ _throw( Null_Exception);}
_temp593->tl;})){ if( Cyc_Tcutil_region_in_effect( constrain, r,( void*)({
struct Cyc_List_List* _temp594= _temp568; if( _temp594 == 0){ _throw(
Null_Exception);} _temp594->hd;}))){ return 1;}} return 0; _LL563: if(* _temp577
!=( void*) Cyc_Absyn_EffKind){(( void(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp595=( char*)"effect evar has wrong kind";
struct _tagged_string _temp596; _temp596.curr= _temp595; _temp596.base= _temp595;
_temp596.last_plus_one= _temp595 + 27; _temp596;}));} if( ! constrain){ return 0;}{
void* _temp597= Cyc_Absyn_new_evar(( void*) Cyc_Absyn_EffKind); goto _LL598;
_LL598: { struct Cyc_Absyn_JoinEff_struct* _temp605=({ struct Cyc_Absyn_JoinEff_struct*
_temp599=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp599[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp600; _temp600.tag= Cyc_Absyn_JoinEff;
_temp600.f1=({ struct Cyc_List_List* _temp601=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp601->hd=( void*) _temp597; _temp601->tl=({
struct Cyc_List_List* _temp602=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp602->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp603=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp603[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp604; _temp604.tag= Cyc_Absyn_AccessEff;
_temp604.f1=( void*) r; _temp604;}); _temp603;})); _temp602->tl= 0; _temp602;});
_temp601;}); _temp600;}); _temp599;}); goto _LL606; _LL606:* _temp574=({ struct
Cyc_Core_Opt* _temp607=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp607->v=( void*)(( void*) _temp605); _temp607;}); return 1;}} _LL565: return
0; _LL557:;}} static int Cyc_Tcutil_variable_in_effect( int constrain, struct
Cyc_Absyn_Tvar* v, void* e){ e= Cyc_Tcutil_compress( e);{ void* _temp608= e;
struct Cyc_Absyn_Tvar* _temp618; struct Cyc_List_List* _temp620; int _temp622;
struct Cyc_Core_Opt* _temp624; struct Cyc_Core_Opt** _temp626; void* _temp627;
void** _temp629; _LL610: if(( unsigned int) _temp608 > 4u?*(( int*) _temp608) ==
Cyc_Absyn_VarType: 0){ _LL619: _temp618=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp608)->f1; goto _LL611;} else{ goto _LL612;} _LL612: if(( unsigned int)
_temp608 > 4u?*(( int*) _temp608) == Cyc_Absyn_JoinEff: 0){ _LL621: _temp620=(
struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp608)->f1; goto
_LL613;} else{ goto _LL614;} _LL614: if(( unsigned int) _temp608 > 4u?*(( int*)
_temp608) == Cyc_Absyn_Evar: 0){ _LL628: _temp627=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp608)->f1; _temp629=&(( struct Cyc_Absyn_Evar_struct*) _temp608)->f1; goto
_LL625; _LL625: _temp624=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp608)->f2; _temp626=&(( struct Cyc_Absyn_Evar_struct*) _temp608)->f2; goto
_LL623; _LL623: _temp622=( int)(( struct Cyc_Absyn_Evar_struct*) _temp608)->f3;
goto _LL615;} else{ goto _LL616;} _LL616: goto _LL617; _LL611: return Cyc_Absyn_tvar_cmp(
v, _temp618) == 0; _LL613: for( 0; _temp620 != 0; _temp620=({ struct Cyc_List_List*
_temp630= _temp620; if( _temp630 == 0){ _throw( Null_Exception);} _temp630->tl;})){
if( Cyc_Tcutil_variable_in_effect( constrain, v,( void*)({ struct Cyc_List_List*
_temp631= _temp620; if( _temp631 == 0){ _throw( Null_Exception);} _temp631->hd;}))){
return 1;}} return 0; _LL615: if(* _temp629 !=( void*) Cyc_Absyn_EffKind){((
void(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp632=( char*)"effect evar has wrong kind"; struct _tagged_string
_temp633; _temp633.curr= _temp632; _temp633.base= _temp632; _temp633.last_plus_one=
_temp632 + 27; _temp633;}));}{ void* _temp634= Cyc_Absyn_new_evar(( void*) Cyc_Absyn_EffKind);
goto _LL635; _LL635: { struct Cyc_Absyn_JoinEff_struct* _temp642=({ struct Cyc_Absyn_JoinEff_struct*
_temp636=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp636[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp637; _temp637.tag= Cyc_Absyn_JoinEff;
_temp637.f1=({ struct Cyc_List_List* _temp638=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp638->hd=( void*) _temp634; _temp638->tl=({
struct Cyc_List_List* _temp639=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp639->hd=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct*
_temp640=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp640[ 0]=({ struct Cyc_Absyn_VarType_struct _temp641; _temp641.tag= Cyc_Absyn_VarType;
_temp641.f1= v; _temp641;}); _temp640;})); _temp639->tl= 0; _temp639;});
_temp638;}); _temp637;}); _temp636;}); goto _LL643; _LL643:* _temp626=({ struct
Cyc_Core_Opt* _temp644=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp644->v=( void*)(( void*) _temp642); _temp644;}); return 1;}} _LL617: return
0; _LL609:;}} static int Cyc_Tcutil_evar_in_effect( void* evar, void* e){ e= Cyc_Tcutil_compress(
e);{ void* _temp645= e; struct Cyc_List_List* _temp653; int _temp655; struct Cyc_Core_Opt*
_temp657; void* _temp659; _LL647: if(( unsigned int) _temp645 > 4u?*(( int*)
_temp645) == Cyc_Absyn_JoinEff: 0){ _LL654: _temp653=( struct Cyc_List_List*)((
struct Cyc_Absyn_JoinEff_struct*) _temp645)->f1; goto _LL648;} else{ goto _LL649;}
_LL649: if(( unsigned int) _temp645 > 4u?*(( int*) _temp645) == Cyc_Absyn_Evar:
0){ _LL660: _temp659=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp645)->f1;
goto _LL658; _LL658: _temp657=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp645)->f2; goto _LL656; _LL656: _temp655=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp645)->f3; goto _LL650;} else{ goto _LL651;} _LL651: goto _LL652; _LL648:
for( 0; _temp653 != 0; _temp653=({ struct Cyc_List_List* _temp661= _temp653; if(
_temp661 == 0){ _throw( Null_Exception);} _temp661->tl;})){ if( Cyc_Tcutil_evar_in_effect(
evar,( void*)({ struct Cyc_List_List* _temp662= _temp653; if( _temp662 == 0){
_throw( Null_Exception);} _temp662->hd;}))){ return 1;}} return 0; _LL650:
return evar == e; _LL652: return 0; _LL646:;}} int Cyc_Tcutil_subset_effect( int
set_to_empty, void* e1, void* e2){{ struct _tuple8* _temp663= Cyc_Tcutil_get_effect_evar(
e2); struct _tuple8 _temp669; struct Cyc_List_List* _temp670; void* _temp672;
_LL665: if( _temp663 == 0){ goto _LL667;} else{ _temp669=* _temp663; _LL673:
_temp672= _temp669.f1; goto _LL671; _LL671: _temp670= _temp669.f2; goto _LL666;}
_LL667: goto _LL668; _LL666: { void* _temp674= Cyc_Absyn_new_evar(( void*) Cyc_Absyn_EffKind);
goto _LL675; _LL675: { struct Cyc_Absyn_JoinEff_struct* _temp680=({ struct Cyc_Absyn_JoinEff_struct*
_temp676=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp676[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp677; _temp677.tag= Cyc_Absyn_JoinEff;
_temp677.f1=({ struct Cyc_List_List* _temp678=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp678->hd=( void*) _temp674; _temp678->tl=({
struct Cyc_List_List* _temp679=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp679->hd=( void*) e1; _temp679->tl= 0; _temp679;});
_temp678;}); _temp677;}); _temp676;}); goto _LL681; _LL681: return Cyc_Tcutil_unify(
_temp672,( void*) _temp680);}} _LL668: goto _LL664; _LL664:;}{ void* _temp682=
Cyc_Tcutil_compress( e1); struct Cyc_List_List* _temp694; void* _temp696; struct
Cyc_Absyn_Tvar* _temp698; int _temp700; struct Cyc_Core_Opt* _temp702; struct
Cyc_Core_Opt** _temp704; void* _temp705; _LL684: if(( unsigned int) _temp682 > 4u?*((
int*) _temp682) == Cyc_Absyn_JoinEff: 0){ _LL695: _temp694=( struct Cyc_List_List*)((
struct Cyc_Absyn_JoinEff_struct*) _temp682)->f1; goto _LL685;} else{ goto _LL686;}
_LL686: if(( unsigned int) _temp682 > 4u?*(( int*) _temp682) == Cyc_Absyn_AccessEff:
0){ _LL697: _temp696=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp682)->f1;
goto _LL687;} else{ goto _LL688;} _LL688: if(( unsigned int) _temp682 > 4u?*((
int*) _temp682) == Cyc_Absyn_VarType: 0){ _LL699: _temp698=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_VarType_struct*) _temp682)->f1; goto _LL689;} else{ goto _LL690;}
_LL690: if(( unsigned int) _temp682 > 4u?*(( int*) _temp682) == Cyc_Absyn_Evar:
0){ _LL706: _temp705=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp682)->f1;
goto _LL703; _LL703: _temp702=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp682)->f2; _temp704=&(( struct Cyc_Absyn_Evar_struct*) _temp682)->f2; goto
_LL701; _LL701: _temp700=( int)(( struct Cyc_Absyn_Evar_struct*) _temp682)->f3;
goto _LL691;} else{ goto _LL692;} _LL692: goto _LL693; _LL685: for( 0; _temp694
!= 0; _temp694=({ struct Cyc_List_List* _temp707= _temp694; if( _temp707 == 0){
_throw( Null_Exception);} _temp707->tl;})){ if( ! Cyc_Tcutil_subset_effect(
set_to_empty,( void*)({ struct Cyc_List_List* _temp708= _temp694; if( _temp708
== 0){ _throw( Null_Exception);} _temp708->hd;}), e2)){ return 0;}} return 1;
_LL687: return( Cyc_Tcutil_region_in_effect( 0, _temp696, e2)? 1: Cyc_Tcutil_region_in_effect(
1, _temp696, e2))? 1: Cyc_Tcutil_unify( _temp696,( void*) Cyc_Absyn_HeapRgn);
_LL689: return Cyc_Tcutil_variable_in_effect( 0, _temp698, e2)? 1: Cyc_Tcutil_variable_in_effect(
1, _temp698, e2); _LL691: if( ! Cyc_Tcutil_evar_in_effect( e1, e2)){ if(
set_to_empty){* _temp704=({ struct Cyc_Core_Opt* _temp709=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp709->v=( void*)(( void*)({ struct
Cyc_Absyn_JoinEff_struct* _temp710=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp710[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp711; _temp711.tag= Cyc_Absyn_JoinEff; _temp711.f1= 0; _temp711;}); _temp710;}));
_temp709;});} else{* _temp704=({ struct Cyc_Core_Opt* _temp712=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp712->v=( void*) e2; _temp712;});}}
return 1; _LL693: return(( int(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({
struct _tagged_string _temp713= Cyc_Absynpp_typ2string( e1); xprintf("subset_effect: bad effect: %.*s",
_temp713.last_plus_one - _temp713.curr, _temp713.curr);})); _LL683:;}} struct
_tuple9{ struct _tuple8* f1; struct _tuple8* f2; } ; static int Cyc_Tcutil_unify_effect(
void* e1, void* e2){ e1= Cyc_Tcutil_compress( e1); e2= Cyc_Tcutil_compress( e2);
Cyc_Tcutil_normalize_effect( e1); Cyc_Tcutil_normalize_effect( e2);{ struct
_tuple9 _temp715=({ struct _tuple9 _temp714; _temp714.f1= Cyc_Tcutil_get_effect_evar(
e1); _temp714.f2= Cyc_Tcutil_get_effect_evar( e2); _temp714;}); struct _tuple8*
_temp721; struct _tuple8 _temp723; struct Cyc_List_List* _temp724; void*
_temp726; struct _tuple8* _temp728; struct _tuple8 _temp730; struct Cyc_List_List*
_temp731; void* _temp733; _LL717: _LL729: _temp728= _temp715.f1; if( _temp728 ==
0){ goto _LL719;} else{ _temp730=* _temp728; _LL734: _temp733= _temp730.f1; goto
_LL732; _LL732: _temp731= _temp730.f2; goto _LL722;} _LL722: _temp721= _temp715.f2;
if( _temp721 == 0){ goto _LL719;} else{ _temp723=* _temp721; _LL727: _temp726=
_temp723.f1; goto _LL725; _LL725: _temp724= _temp723.f2; goto _LL718;} _LL719:
goto _LL720; _LL718: { void* _temp735= Cyc_Absyn_new_evar(( void*) Cyc_Absyn_EffKind);
goto _LL736; _LL736: { struct Cyc_Absyn_JoinEff_struct* _temp740=({ struct Cyc_Absyn_JoinEff_struct*
_temp737=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp737[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp738; _temp738.tag= Cyc_Absyn_JoinEff;
_temp738.f1=({ struct Cyc_List_List* _temp739=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp739->hd=( void*) _temp735; _temp739->tl=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp731, _temp724); _temp739;}); _temp738;}); _temp737;}); goto _LL741; _LL741:
return Cyc_Tcutil_unify( _temp733,( void*) _temp740)? Cyc_Tcutil_unify( _temp726,(
void*) _temp740): 0;}} _LL720: return Cyc_Tcutil_subset_effect( 0, e1, e2)? Cyc_Tcutil_subset_effect(
0, e2, e1): 0; _LL716:;}} struct _tuple10{ struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt*
f2; } ; void Cyc_Tcutil_unify_it( void* t1, void* t2){ t1= Cyc_Tcutil_compress(
t1); t2= Cyc_Tcutil_compress( t2); if( t1 == t2){ return;}{ void* _temp742= t1;
int _temp748; struct Cyc_Core_Opt* _temp750; struct Cyc_Core_Opt** _temp752;
void* _temp753; _LL744: if(( unsigned int) _temp742 > 4u?*(( int*) _temp742) ==
Cyc_Absyn_Evar: 0){ _LL754: _temp753=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp742)->f1; goto _LL751; _LL751: _temp750=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Evar_struct*) _temp742)->f2; _temp752=&(( struct Cyc_Absyn_Evar_struct*)
_temp742)->f2; goto _LL749; _LL749: _temp748=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp742)->f3; goto _LL745;} else{ goto _LL746;} _LL746: goto _LL747; _LL745:
Cyc_Tcutil_occurs( t1, t2);{ void* _temp755= Cyc_Tcutil_typ_kind( t2); goto
_LL756; _LL756: if( Cyc_Tcutil_kind_leq( _temp755, _temp753)){* _temp752=({
struct Cyc_Core_Opt* _temp757=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp757->v=( void*) t2; _temp757;}); return;} else{{ void*
_temp758= t2; int _temp766; struct Cyc_Core_Opt* _temp768; struct Cyc_Core_Opt**
_temp770; void* _temp771; struct Cyc_Absyn_PtrInfo _temp774; _LL760: if((
unsigned int) _temp758 > 4u?*(( int*) _temp758) == Cyc_Absyn_Evar: 0){ _LL772:
_temp771=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp758)->f1; goto _LL769;
_LL769: _temp768=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp758)->f2; _temp770=&(( struct Cyc_Absyn_Evar_struct*) _temp758)->f2; goto
_LL767; _LL767: _temp766=( int)(( struct Cyc_Absyn_Evar_struct*) _temp758)->f3;
goto _LL761;} else{ goto _LL762;} _LL762: if(( unsigned int) _temp758 > 4u?*((
int*) _temp758) == Cyc_Absyn_PointerType: 0){ _LL775: _temp774=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp758)->f1; goto _LL773;} else{ goto
_LL764;} _LL773: if( _temp753 ==( void*) Cyc_Absyn_BoxKind){ goto _LL763;} else{
goto _LL764;} _LL764: goto _LL765; _LL761: if( Cyc_Tcutil_kind_leq( _temp753,
_temp755)){* _temp770=({ struct Cyc_Core_Opt* _temp776=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp776->v=( void*) t1; _temp776;});
return;} goto _LL759; _LL763: { struct Cyc_Absyn_Conref* _temp777=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp774.bounds); goto
_LL778; _LL778:{ void* _temp779=( void*) _temp777->v; _LL781: if( _temp779 ==(
void*) Cyc_Absyn_No_constr){ goto _LL782;} else{ goto _LL783;} _LL783: goto
_LL784; _LL782:( void*)( _temp777->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp785=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp785[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp786; _temp786.tag= Cyc_Absyn_Eq_constr;
_temp786.f1=( void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp787=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp787[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp788; _temp788.tag= Cyc_Absyn_Upper_b;
_temp788.f1= Cyc_Absyn_signed_int_exp( 1, 0); _temp788;}); _temp787;}));
_temp786;}); _temp785;})));* _temp752=({ struct Cyc_Core_Opt* _temp789=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp789->v=( void*) t2;
_temp789;}); return; _LL784: goto _LL780; _LL780:;} goto _LL759;} _LL765: goto
_LL759; _LL759:;}( void) _throw(( void*) Cyc_Tcutil_Unify);}} _LL747: goto
_LL743; _LL743:;}{ struct _tuple7 _temp791=({ struct _tuple7 _temp790; _temp790.f1=
t2; _temp790.f2= t1; _temp790;}); void* _temp841; void* _temp843; int _temp845;
struct Cyc_Core_Opt* _temp847; void* _temp849; void* _temp851; void* _temp853;
void* _temp855; struct Cyc_Absyn_Tvar* _temp857; void* _temp859; struct Cyc_Absyn_Tvar*
_temp861; void* _temp863; struct Cyc_Absyn_Structdecl** _temp865; struct Cyc_List_List*
_temp867; struct _tuple1* _temp869; void* _temp871; struct Cyc_Absyn_Structdecl**
_temp873; struct Cyc_List_List* _temp875; struct _tuple1* _temp877; void*
_temp879; struct Cyc_Absyn_Enumdecl* _temp881; struct _tuple1* _temp883; void*
_temp885; struct Cyc_Absyn_Enumdecl* _temp887; struct _tuple1* _temp889; void*
_temp891; struct Cyc_Absyn_Uniondecl** _temp893; struct Cyc_List_List* _temp895;
struct _tuple1* _temp897; void* _temp899; struct Cyc_Absyn_Uniondecl** _temp901;
struct Cyc_List_List* _temp903; struct _tuple1* _temp905; void* _temp907; struct
Cyc_Absyn_TunionInfo _temp909; void* _temp911; struct Cyc_List_List* _temp913;
void* _temp915; struct Cyc_Absyn_Tuniondecl* _temp917; void* _temp919; struct
Cyc_Absyn_TunionInfo _temp921; void* _temp923; struct Cyc_List_List* _temp925;
void* _temp927; struct Cyc_Absyn_Tuniondecl* _temp929; void* _temp931; struct
Cyc_Absyn_TunionFieldInfo _temp933; struct Cyc_List_List* _temp935; void*
_temp937; struct Cyc_Absyn_Tunionfield* _temp939; struct Cyc_Absyn_Tuniondecl*
_temp941; void* _temp943; struct Cyc_Absyn_TunionFieldInfo _temp945; struct Cyc_List_List*
_temp947; void* _temp949; struct Cyc_Absyn_Tunionfield* _temp951; struct Cyc_Absyn_Tuniondecl*
_temp953; void* _temp955; struct Cyc_Absyn_PtrInfo _temp957; struct Cyc_Absyn_Conref*
_temp959; struct Cyc_Absyn_Tqual _temp961; struct Cyc_Absyn_Conref* _temp963;
void* _temp965; void* _temp967; void* _temp969; struct Cyc_Absyn_PtrInfo
_temp971; struct Cyc_Absyn_Conref* _temp973; struct Cyc_Absyn_Tqual _temp975;
struct Cyc_Absyn_Conref* _temp977; void* _temp979; void* _temp981; void*
_temp983; void* _temp985; void* _temp987; void* _temp989; void* _temp991; void*
_temp993; void* _temp995; void* _temp997; void* _temp999; void* _temp1001; void*
_temp1003; struct Cyc_Absyn_Exp* _temp1005; struct Cyc_Absyn_Tqual _temp1007;
void* _temp1009; void* _temp1011; struct Cyc_Absyn_Exp* _temp1013; struct Cyc_Absyn_Tqual
_temp1015; void* _temp1017; void* _temp1019; struct Cyc_Absyn_FnInfo _temp1021;
struct Cyc_List_List* _temp1023; int _temp1025; struct Cyc_List_List* _temp1027;
void* _temp1029; struct Cyc_Core_Opt* _temp1031; struct Cyc_List_List* _temp1033;
void* _temp1035; struct Cyc_Absyn_FnInfo _temp1037; struct Cyc_List_List*
_temp1039; int _temp1041; struct Cyc_List_List* _temp1043; void* _temp1045;
struct Cyc_Core_Opt* _temp1047; struct Cyc_List_List* _temp1049; void* _temp1051;
struct Cyc_List_List* _temp1053; void* _temp1055; struct Cyc_List_List*
_temp1057; void* _temp1059; struct Cyc_List_List* _temp1061; void* _temp1063;
struct Cyc_List_List* _temp1065; void* _temp1067; struct Cyc_List_List*
_temp1069; void* _temp1071; struct Cyc_List_List* _temp1073; void* _temp1075;
void* _temp1077; void* _temp1079; void* _temp1081; void* _temp1083; void*
_temp1085; void* _temp1087; void* _temp1089; struct Cyc_List_List* _temp1091;
void* _temp1093; struct Cyc_List_List* _temp1095; void* _temp1097; void*
_temp1099; void* _temp1101; void* _temp1103; void* _temp1105; void* _temp1107;
void* _temp1109; _LL793: _LL844: _temp843= _temp791.f1; if(( unsigned int)
_temp843 > 4u?*(( int*) _temp843) == Cyc_Absyn_Evar: 0){ _LL850: _temp849=( void*)((
struct Cyc_Absyn_Evar_struct*) _temp843)->f1; goto _LL848; _LL848: _temp847=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp843)->f2; goto
_LL846; _LL846: _temp845=( int)(( struct Cyc_Absyn_Evar_struct*) _temp843)->f3;
goto _LL842;} else{ goto _LL795;} _LL842: _temp841= _temp791.f2; goto _LL794;
_LL795: _LL854: _temp853= _temp791.f1; if( _temp853 ==( void*) Cyc_Absyn_VoidType){
goto _LL852;} else{ goto _LL797;} _LL852: _temp851= _temp791.f2; if( _temp851 ==(
void*) Cyc_Absyn_VoidType){ goto _LL796;} else{ goto _LL797;} _LL797: _LL860:
_temp859= _temp791.f1; if(( unsigned int) _temp859 > 4u?*(( int*) _temp859) ==
Cyc_Absyn_VarType: 0){ _LL862: _temp861=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp859)->f1; goto _LL856;} else{ goto _LL799;} _LL856: _temp855= _temp791.f2;
if(( unsigned int) _temp855 > 4u?*(( int*) _temp855) == Cyc_Absyn_VarType: 0){
_LL858: _temp857=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp855)->f1; goto _LL798;} else{ goto _LL799;} _LL799: _LL872: _temp871=
_temp791.f1; if(( unsigned int) _temp871 > 4u?*(( int*) _temp871) == Cyc_Absyn_StructType:
0){ _LL878: _temp877=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp871)->f1; goto _LL876; _LL876: _temp875=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp871)->f2; goto _LL874; _LL874: _temp873=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp871)->f3;
goto _LL864;} else{ goto _LL801;} _LL864: _temp863= _temp791.f2; if((
unsigned int) _temp863 > 4u?*(( int*) _temp863) == Cyc_Absyn_StructType: 0){
_LL870: _temp869=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp863)->f1; goto _LL868; _LL868: _temp867=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp863)->f2; goto _LL866; _LL866: _temp865=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp863)->f3;
goto _LL800;} else{ goto _LL801;} _LL801: _LL886: _temp885= _temp791.f1; if((
unsigned int) _temp885 > 4u?*(( int*) _temp885) == Cyc_Absyn_EnumType: 0){
_LL890: _temp889=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*)
_temp885)->f1; goto _LL888; _LL888: _temp887=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_EnumType_struct*) _temp885)->f2; goto _LL880;} else{ goto
_LL803;} _LL880: _temp879= _temp791.f2; if(( unsigned int) _temp879 > 4u?*(( int*)
_temp879) == Cyc_Absyn_EnumType: 0){ _LL884: _temp883=( struct _tuple1*)((
struct Cyc_Absyn_EnumType_struct*) _temp879)->f1; goto _LL882; _LL882: _temp881=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*) _temp879)->f2;
goto _LL802;} else{ goto _LL803;} _LL803: _LL900: _temp899= _temp791.f1; if((
unsigned int) _temp899 > 4u?*(( int*) _temp899) == Cyc_Absyn_UnionType: 0){
_LL906: _temp905=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp899)->f1; goto _LL904; _LL904: _temp903=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp899)->f2; goto _LL902; _LL902: _temp901=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp899)->f3;
goto _LL892;} else{ goto _LL805;} _LL892: _temp891= _temp791.f2; if((
unsigned int) _temp891 > 4u?*(( int*) _temp891) == Cyc_Absyn_UnionType: 0){
_LL898: _temp897=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp891)->f1; goto _LL896; _LL896: _temp895=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp891)->f2; goto _LL894; _LL894: _temp893=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp891)->f3;
goto _LL804;} else{ goto _LL805;} _LL805: _LL920: _temp919= _temp791.f1; if((
unsigned int) _temp919 > 4u?*(( int*) _temp919) == Cyc_Absyn_TunionType: 0){
_LL922: _temp921=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp919)->f1; _LL928: _temp927=( void*) _temp921.tunion_info; if(*(( int*)
_temp927) == Cyc_Absyn_KnownTunion){ _LL930: _temp929=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunion_struct*) _temp927)->f1; goto _LL926;} else{ goto
_LL807;} _LL926: _temp925=( struct Cyc_List_List*) _temp921.targs; goto _LL924;
_LL924: _temp923=( void*) _temp921.rgn; goto _LL908;} else{ goto _LL807;} _LL908:
_temp907= _temp791.f2; if(( unsigned int) _temp907 > 4u?*(( int*) _temp907) ==
Cyc_Absyn_TunionType: 0){ _LL910: _temp909=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp907)->f1; _LL916: _temp915=( void*)
_temp909.tunion_info; if(*(( int*) _temp915) == Cyc_Absyn_KnownTunion){ _LL918:
_temp917=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_KnownTunion_struct*)
_temp915)->f1; goto _LL914;} else{ goto _LL807;} _LL914: _temp913=( struct Cyc_List_List*)
_temp909.targs; goto _LL912; _LL912: _temp911=( void*) _temp909.rgn; goto _LL806;}
else{ goto _LL807;} _LL807: _LL944: _temp943= _temp791.f1; if(( unsigned int)
_temp943 > 4u?*(( int*) _temp943) == Cyc_Absyn_TunionFieldType: 0){ _LL946:
_temp945=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp943)->f1; _LL950: _temp949=( void*) _temp945.field_info; if(*(( int*)
_temp949) == Cyc_Absyn_KnownTunionfield){ _LL954: _temp953=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp949)->f1; goto _LL952; _LL952:
_temp951=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp949)->f2; goto _LL948;} else{ goto _LL809;} _LL948: _temp947=( struct Cyc_List_List*)
_temp945.targs; goto _LL932;} else{ goto _LL809;} _LL932: _temp931= _temp791.f2;
if(( unsigned int) _temp931 > 4u?*(( int*) _temp931) == Cyc_Absyn_TunionFieldType:
0){ _LL934: _temp933=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp931)->f1; _LL938: _temp937=( void*) _temp933.field_info; if(*(( int*)
_temp937) == Cyc_Absyn_KnownTunionfield){ _LL942: _temp941=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp937)->f1; goto _LL940; _LL940:
_temp939=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp937)->f2; goto _LL936;} else{ goto _LL809;} _LL936: _temp935=( struct Cyc_List_List*)
_temp933.targs; goto _LL808;} else{ goto _LL809;} _LL809: _LL970: _temp969=
_temp791.f1; if(( unsigned int) _temp969 > 4u?*(( int*) _temp969) == Cyc_Absyn_PointerType:
0){ _LL972: _temp971=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp969)->f1; _LL982: _temp981=( void*) _temp971.elt_typ; goto _LL980; _LL980:
_temp979=( void*) _temp971.rgn_typ; goto _LL978; _LL978: _temp977=( struct Cyc_Absyn_Conref*)
_temp971.nullable; goto _LL976; _LL976: _temp975=( struct Cyc_Absyn_Tqual)
_temp971.tq; goto _LL974; _LL974: _temp973=( struct Cyc_Absyn_Conref*) _temp971.bounds;
goto _LL956;} else{ goto _LL811;} _LL956: _temp955= _temp791.f2; if((
unsigned int) _temp955 > 4u?*(( int*) _temp955) == Cyc_Absyn_PointerType: 0){
_LL958: _temp957=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp955)->f1; _LL968: _temp967=( void*) _temp957.elt_typ; goto _LL966; _LL966:
_temp965=( void*) _temp957.rgn_typ; goto _LL964; _LL964: _temp963=( struct Cyc_Absyn_Conref*)
_temp957.nullable; goto _LL962; _LL962: _temp961=( struct Cyc_Absyn_Tqual)
_temp957.tq; goto _LL960; _LL960: _temp959=( struct Cyc_Absyn_Conref*) _temp957.bounds;
goto _LL810;} else{ goto _LL811;} _LL811: _LL990: _temp989= _temp791.f1; if((
unsigned int) _temp989 > 4u?*(( int*) _temp989) == Cyc_Absyn_IntType: 0){ _LL994:
_temp993=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp989)->f1; goto _LL992;
_LL992: _temp991=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp989)->f2;
goto _LL984;} else{ goto _LL813;} _LL984: _temp983= _temp791.f2; if((
unsigned int) _temp983 > 4u?*(( int*) _temp983) == Cyc_Absyn_IntType: 0){ _LL988:
_temp987=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp983)->f1; goto _LL986;
_LL986: _temp985=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp983)->f2;
goto _LL812;} else{ goto _LL813;} _LL813: _LL998: _temp997= _temp791.f1; if(
_temp997 ==( void*) Cyc_Absyn_FloatType){ goto _LL996;} else{ goto _LL815;}
_LL996: _temp995= _temp791.f2; if( _temp995 ==( void*) Cyc_Absyn_FloatType){
goto _LL814;} else{ goto _LL815;} _LL815: _LL1002: _temp1001= _temp791.f1; if(
_temp1001 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1000;} else{ goto _LL817;}
_LL1000: _temp999= _temp791.f2; if( _temp999 ==( void*) Cyc_Absyn_DoubleType){
goto _LL816;} else{ goto _LL817;} _LL817: _LL1012: _temp1011= _temp791.f1; if((
unsigned int) _temp1011 > 4u?*(( int*) _temp1011) == Cyc_Absyn_ArrayType: 0){
_LL1018: _temp1017=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1011)->f1;
goto _LL1016; _LL1016: _temp1015=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1011)->f2; goto _LL1014; _LL1014: _temp1013=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1011)->f3; goto _LL1004;} else{ goto
_LL819;} _LL1004: _temp1003= _temp791.f2; if(( unsigned int) _temp1003 > 4u?*((
int*) _temp1003) == Cyc_Absyn_ArrayType: 0){ _LL1010: _temp1009=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1003)->f1; goto _LL1008; _LL1008:
_temp1007=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1003)->f2; goto _LL1006; _LL1006: _temp1005=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1003)->f3; goto _LL818;} else{ goto
_LL819;} _LL819: _LL1036: _temp1035= _temp791.f1; if(( unsigned int) _temp1035 >
4u?*(( int*) _temp1035) == Cyc_Absyn_FnType: 0){ _LL1038: _temp1037=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp1035)->f1; _LL1050: _temp1049=( struct Cyc_List_List*)
_temp1037.tvars; goto _LL1048; _LL1048: _temp1047=( struct Cyc_Core_Opt*)
_temp1037.effect; goto _LL1046; _LL1046: _temp1045=( void*) _temp1037.ret_typ;
goto _LL1044; _LL1044: _temp1043=( struct Cyc_List_List*) _temp1037.args; goto
_LL1042; _LL1042: _temp1041=( int) _temp1037.varargs; goto _LL1040; _LL1040:
_temp1039=( struct Cyc_List_List*) _temp1037.attributes; goto _LL1020;} else{
goto _LL821;} _LL1020: _temp1019= _temp791.f2; if(( unsigned int) _temp1019 > 4u?*((
int*) _temp1019) == Cyc_Absyn_FnType: 0){ _LL1022: _temp1021=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp1019)->f1; _LL1034: _temp1033=( struct Cyc_List_List*)
_temp1021.tvars; goto _LL1032; _LL1032: _temp1031=( struct Cyc_Core_Opt*)
_temp1021.effect; goto _LL1030; _LL1030: _temp1029=( void*) _temp1021.ret_typ;
goto _LL1028; _LL1028: _temp1027=( struct Cyc_List_List*) _temp1021.args; goto
_LL1026; _LL1026: _temp1025=( int) _temp1021.varargs; goto _LL1024; _LL1024:
_temp1023=( struct Cyc_List_List*) _temp1021.attributes; goto _LL820;} else{
goto _LL821;} _LL821: _LL1056: _temp1055= _temp791.f1; if(( unsigned int)
_temp1055 > 4u?*(( int*) _temp1055) == Cyc_Absyn_TupleType: 0){ _LL1058:
_temp1057=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp1055)->f1; goto _LL1052;} else{ goto _LL823;} _LL1052: _temp1051= _temp791.f2;
if(( unsigned int) _temp1051 > 4u?*(( int*) _temp1051) == Cyc_Absyn_TupleType: 0){
_LL1054: _temp1053=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp1051)->f1; goto _LL822;} else{ goto _LL823;} _LL823: _LL1064: _temp1063=
_temp791.f1; if(( unsigned int) _temp1063 > 4u?*(( int*) _temp1063) == Cyc_Absyn_AnonStructType:
0){ _LL1066: _temp1065=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp1063)->f1; goto _LL1060;} else{ goto _LL825;} _LL1060: _temp1059= _temp791.f2;
if(( unsigned int) _temp1059 > 4u?*(( int*) _temp1059) == Cyc_Absyn_AnonStructType:
0){ _LL1062: _temp1061=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp1059)->f1; goto _LL824;} else{ goto _LL825;} _LL825: _LL1072: _temp1071=
_temp791.f1; if(( unsigned int) _temp1071 > 4u?*(( int*) _temp1071) == Cyc_Absyn_AnonUnionType:
0){ _LL1074: _temp1073=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp1071)->f1; goto _LL1068;} else{ goto _LL827;} _LL1068: _temp1067= _temp791.f2;
if(( unsigned int) _temp1067 > 4u?*(( int*) _temp1067) == Cyc_Absyn_AnonUnionType:
0){ _LL1070: _temp1069=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp1067)->f1; goto _LL826;} else{ goto _LL827;} _LL827: _LL1078: _temp1077=
_temp791.f1; if( _temp1077 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL1076;} else{
goto _LL829;} _LL1076: _temp1075= _temp791.f2; if( _temp1075 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL828;} else{ goto _LL829;} _LL829: _LL1084: _temp1083= _temp791.f1; if((
unsigned int) _temp1083 > 4u?*(( int*) _temp1083) == Cyc_Absyn_RgnHandleType: 0){
_LL1086: _temp1085=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp1083)->f1;
goto _LL1080;} else{ goto _LL831;} _LL1080: _temp1079= _temp791.f2; if((
unsigned int) _temp1079 > 4u?*(( int*) _temp1079) == Cyc_Absyn_RgnHandleType: 0){
_LL1082: _temp1081=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp1079)->f1;
goto _LL830;} else{ goto _LL831;} _LL831: _LL1090: _temp1089= _temp791.f1; if((
unsigned int) _temp1089 > 4u?*(( int*) _temp1089) == Cyc_Absyn_JoinEff: 0){
_LL1092: _temp1091=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*)
_temp1089)->f1; goto _LL1088;} else{ goto _LL833;} _LL1088: _temp1087= _temp791.f2;
goto _LL832; _LL833: _LL1098: _temp1097= _temp791.f1; goto _LL1094; _LL1094:
_temp1093= _temp791.f2; if(( unsigned int) _temp1093 > 4u?*(( int*) _temp1093)
== Cyc_Absyn_JoinEff: 0){ _LL1096: _temp1095=( struct Cyc_List_List*)(( struct
Cyc_Absyn_JoinEff_struct*) _temp1093)->f1; goto _LL834;} else{ goto _LL835;}
_LL835: _LL1102: _temp1101= _temp791.f1; if(( unsigned int) _temp1101 > 4u?*((
int*) _temp1101) == Cyc_Absyn_AccessEff: 0){ _LL1104: _temp1103=( void*)((
struct Cyc_Absyn_AccessEff_struct*) _temp1101)->f1; goto _LL1100;} else{ goto
_LL837;} _LL1100: _temp1099= _temp791.f2; goto _LL836; _LL837: _LL1110:
_temp1109= _temp791.f1; goto _LL1106; _LL1106: _temp1105= _temp791.f2; if((
unsigned int) _temp1105 > 4u?*(( int*) _temp1105) == Cyc_Absyn_AccessEff: 0){
_LL1108: _temp1107=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp1105)->f1;
goto _LL838;} else{ goto _LL839;} _LL839: goto _LL840; _LL794: Cyc_Tcutil_unify_it(
t2, t1); return; _LL796: return; _LL798: { struct _tagged_string* _temp1111=
_temp861->name; goto _LL1112; _LL1112: { struct _tagged_string* _temp1113=
_temp857->name; goto _LL1114; _LL1114: { void* _temp1115=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp861->kind); goto _LL1116; _LL1116: { void*
_temp1117=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp857->kind); goto _LL1118; _LL1118: if( Cyc_String_zstrptrcmp( _temp1111,
_temp1113) == 0){ if( _temp1115 != _temp1117){(( void(*)( struct _tagged_string
msg)) Cyc_Tcutil_impos)(({ struct _tagged_string _temp1119=* _temp1111; struct
_tagged_string _temp1120= Cyc_Absynpp_kind2string( _temp1115); struct
_tagged_string _temp1121= Cyc_Absynpp_kind2string( _temp1117); xprintf("same type variable %.*s has kinds %.*s and %.*s",
_temp1119.last_plus_one - _temp1119.curr, _temp1119.curr, _temp1120.last_plus_one
- _temp1120.curr, _temp1120.curr, _temp1121.last_plus_one - _temp1121.curr,
_temp1121.curr);}));} return;} goto _LL792;}}}} _LL800: if((( _temp869 != 0?
_temp877 != 0: 0)? Cyc_Absyn_qvar_cmp(( struct _tuple1*)({ struct _tuple1*
_temp1122= _temp869; if( _temp1122 == 0){ _throw( Null_Exception);} _temp1122;}),(
struct _tuple1*)({ struct _tuple1* _temp1123= _temp877; if( _temp1123 == 0){
_throw( Null_Exception);} _temp1123;})) == 0: 0)? 1:( _temp869 == 0? _temp877 ==
0: 0)){ Cyc_Tcutil_unify_list( _temp867, _temp875); return;} goto _LL792; _LL802:
if( Cyc_Absyn_qvar_cmp( _temp889, _temp883) == 0){ return;} goto _LL792; _LL804:
if((( _temp897 != 0? _temp905 != 0: 0)? Cyc_Absyn_qvar_cmp(( struct _tuple1*)({
struct _tuple1* _temp1124= _temp897; if( _temp1124 == 0){ _throw( Null_Exception);}
_temp1124;}),( struct _tuple1*)({ struct _tuple1* _temp1125= _temp905; if(
_temp1125 == 0){ _throw( Null_Exception);} _temp1125;})) == 0: 0)? 1:( _temp897
== 0? _temp905 == 0: 0)){ Cyc_Tcutil_unify_list( _temp895, _temp903); return;}
goto _LL792; _LL806: if( _temp929 == _temp917? 1: Cyc_Absyn_qvar_cmp( _temp929->name,
_temp917->name) == 0){ Cyc_Tcutil_unify_it( _temp911, _temp923); Cyc_Tcutil_unify_list(
_temp913, _temp925); return;} goto _LL792; _LL808: if(( _temp953 == _temp941? 1:
Cyc_Absyn_qvar_cmp( _temp953->name, _temp941->name) == 0)? _temp951 == _temp939?
1: Cyc_Absyn_qvar_cmp( _temp951->name, _temp939->name) == 0: 0){ Cyc_Tcutil_unify_list(
_temp935, _temp947); return;} goto _LL792; _LL810: Cyc_Tcutil_unify_it( _temp967,
_temp981); Cyc_Tcutil_unify_it( _temp979, _temp965); Cyc_Tcutil_unify_tqual(
_temp961, _temp975);(( void(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref*
x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_it_conrefs)( Cyc_Tcutil_boundscmp,
_temp959, _temp973);{ void* _temp1126=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp959))->v; void*
_temp1132; _LL1128: if(( unsigned int) _temp1126 > 1u?*(( int*) _temp1126) ==
Cyc_Absyn_Eq_constr: 0){ _LL1133: _temp1132=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp1126)->f1; if( _temp1132 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1129;}
else{ goto _LL1130;}} else{ goto _LL1130;} _LL1130: goto _LL1131; _LL1129:
return; _LL1131: goto _LL1127; _LL1127:;}(( void(*)( int(* cmp)( int, int),
struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_it_conrefs)(
Cyc_Core_intcmp, _temp963, _temp977); return; _LL812: if( _temp987 == _temp993?
_temp985 == _temp991: 0){ return;} goto _LL792; _LL814: return; _LL816: return;
_LL818: Cyc_Tcutil_unify_it( _temp1009, _temp1017); Cyc_Tcutil_unify_tqual(
_temp1007, _temp1015); if( _temp1013 == _temp1005){ return;} if( _temp1013 == 0?
1: _temp1005 == 0){ goto _LL792;} if( Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1134= _temp1013; if( _temp1134 == 0){ _throw(
Null_Exception);} _temp1134;})) == Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1135= _temp1005; if( _temp1135 == 0){ _throw(
Null_Exception);} _temp1135;}))){ return;} goto _LL792; _LL820: { int done= 0;{
struct _RegionHandle _temp1136= _new_region(); struct _RegionHandle* rgn=&
_temp1136; _push_region( rgn);{ struct Cyc_List_List* inst= 0; while( _temp1033
!= 0) { if( _temp1049 == 0){ break;} inst=({ struct Cyc_List_List* _temp1137=(
struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp1137->hd=( void*)({ struct _tuple5* _temp1138=( struct _tuple5*)
_region_malloc( rgn, sizeof( struct _tuple5)); _temp1138->f1=( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp1142= _temp1049; if( _temp1142 == 0){ _throw(
Null_Exception);} _temp1142->hd;}); _temp1138->f2=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp1139=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp1139[ 0]=({ struct Cyc_Absyn_VarType_struct _temp1140; _temp1140.tag= Cyc_Absyn_VarType;
_temp1140.f1=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp1141=
_temp1033; if( _temp1141 == 0){ _throw( Null_Exception);} _temp1141->hd;});
_temp1140;}); _temp1139;}); _temp1138;}); _temp1137->tl= inst; _temp1137;});
_temp1033=({ struct Cyc_List_List* _temp1143= _temp1033; if( _temp1143 == 0){
_throw( Null_Exception);} _temp1143->tl;}); _temp1049=({ struct Cyc_List_List*
_temp1144= _temp1049; if( _temp1144 == 0){ _throw( Null_Exception);} _temp1144->tl;});}
if( _temp1049 != 0){ _npop_handler( 0u); goto _LL792;} if( inst != 0){ Cyc_Tcutil_unify_it((
void*)({ struct Cyc_Absyn_FnType_struct* _temp1145=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1145[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1146; _temp1146.tag= Cyc_Absyn_FnType; _temp1146.f1=({ struct Cyc_Absyn_FnInfo
_temp1147; _temp1147.tvars= 0; _temp1147.effect= _temp1031; _temp1147.ret_typ=(
void*) _temp1029; _temp1147.args= _temp1027; _temp1147.varargs= _temp1025;
_temp1147.attributes= _temp1023; _temp1147;}); _temp1146;}); _temp1145;}), Cyc_Tcutil_rsubstitute(
rgn, inst,( void*)({ struct Cyc_Absyn_FnType_struct* _temp1148=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1148[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1149; _temp1149.tag= Cyc_Absyn_FnType; _temp1149.f1=({ struct Cyc_Absyn_FnInfo
_temp1150; _temp1150.tvars= 0; _temp1150.effect= _temp1047; _temp1150.ret_typ=(
void*) _temp1045; _temp1150.args= _temp1043; _temp1150.varargs= _temp1041;
_temp1150.attributes= _temp1039; _temp1150;}); _temp1149;}); _temp1148;})));
done= 1;}}; _pop_region(& _temp1136);} if( done){ return;} Cyc_Tcutil_unify_it(
_temp1029, _temp1045); for( 0; _temp1027 != 0? _temp1043 != 0: 0; _temp1027=({
struct Cyc_List_List* _temp1151= _temp1027; if( _temp1151 == 0){ _throw(
Null_Exception);} _temp1151->tl;}), _temp1043=({ struct Cyc_List_List* _temp1152=
_temp1043; if( _temp1152 == 0){ _throw( Null_Exception);} _temp1152->tl;})){ Cyc_Tcutil_unify_tqual((*((
struct _tuple2*)({ struct Cyc_List_List* _temp1153= _temp1027; if( _temp1153 ==
0){ _throw( Null_Exception);} _temp1153->hd;}))).f2,(*(( struct _tuple2*)({
struct Cyc_List_List* _temp1154= _temp1043; if( _temp1154 == 0){ _throw(
Null_Exception);} _temp1154->hd;}))).f2); Cyc_Tcutil_unify_it((*(( struct
_tuple2*)({ struct Cyc_List_List* _temp1155= _temp1027; if( _temp1155 == 0){
_throw( Null_Exception);} _temp1155->hd;}))).f3,(*(( struct _tuple2*)({ struct
Cyc_List_List* _temp1156= _temp1043; if( _temp1156 == 0){ _throw( Null_Exception);}
_temp1156->hd;}))).f3);} if( _temp1027 != 0? 1: _temp1043 != 0){ goto _LL792;}
if( _temp1025 != _temp1041){ goto _LL792;}{ int bad_effect= 0;{ struct _tuple10
_temp1158=({ struct _tuple10 _temp1157; _temp1157.f1= _temp1031; _temp1157.f2=
_temp1047; _temp1157;}); struct Cyc_Core_Opt* _temp1168; struct Cyc_Core_Opt*
_temp1170; struct Cyc_Core_Opt* _temp1172; struct Cyc_Core_Opt* _temp1174;
struct Cyc_Core_Opt* _temp1176; struct Cyc_Core_Opt* _temp1178; _LL1160: _LL1171:
_temp1170= _temp1158.f1; if( _temp1170 == 0){ goto _LL1169;} else{ goto _LL1162;}
_LL1169: _temp1168= _temp1158.f2; if( _temp1168 == 0){ goto _LL1161;} else{ goto
_LL1162;} _LL1162: _LL1175: _temp1174= _temp1158.f1; if( _temp1174 == 0){ goto
_LL1173;} else{ goto _LL1164;} _LL1173: _temp1172= _temp1158.f2; goto _LL1163;
_LL1164: _LL1179: _temp1178= _temp1158.f1; goto _LL1177; _LL1177: _temp1176=
_temp1158.f2; if( _temp1176 == 0){ goto _LL1165;} else{ goto _LL1166;} _LL1166:
goto _LL1167; _LL1161: goto _LL1159; _LL1163: goto _LL1165; _LL1165: bad_effect=
1; goto _LL1159; _LL1167: bad_effect= ! Cyc_Tcutil_unify_effect(( void*)({
struct Cyc_Core_Opt* _temp1180= _temp1031; if( _temp1180 == 0){ _throw(
Null_Exception);} _temp1180->v;}),( void*)({ struct Cyc_Core_Opt* _temp1181=
_temp1047; if( _temp1181 == 0){ _throw( Null_Exception);} _temp1181->v;})); goto
_LL1159; _LL1159:;} if( bad_effect){ goto _LL792;} if( ! Cyc_Tcutil_same_atts(
_temp1039, _temp1023)){ goto _LL792;} return;}} _LL822: for( 0; _temp1053 != 0?
_temp1057 != 0: 0; _temp1053=({ struct Cyc_List_List* _temp1182= _temp1053; if(
_temp1182 == 0){ _throw( Null_Exception);} _temp1182->tl;}), _temp1057=({ struct
Cyc_List_List* _temp1183= _temp1057; if( _temp1183 == 0){ _throw( Null_Exception);}
_temp1183->tl;})){ Cyc_Tcutil_unify_tqual((*(( struct _tuple4*)({ struct Cyc_List_List*
_temp1184= _temp1053; if( _temp1184 == 0){ _throw( Null_Exception);} _temp1184->hd;}))).f1,(*((
struct _tuple4*)({ struct Cyc_List_List* _temp1185= _temp1057; if( _temp1185 ==
0){ _throw( Null_Exception);} _temp1185->hd;}))).f1); Cyc_Tcutil_unify_it((*((
struct _tuple4*)({ struct Cyc_List_List* _temp1186= _temp1053; if( _temp1186 ==
0){ _throw( Null_Exception);} _temp1186->hd;}))).f2,(*(( struct _tuple4*)({
struct Cyc_List_List* _temp1187= _temp1057; if( _temp1187 == 0){ _throw(
Null_Exception);} _temp1187->hd;}))).f2);} if( _temp1053 == 0? _temp1057 == 0: 0){
return;} goto _LL792; _LL824: _temp1073= _temp1065; _temp1069= _temp1061; goto
_LL826; _LL826: for( 0; _temp1069 != 0? _temp1073 != 0: 0; _temp1069=({ struct
Cyc_List_List* _temp1188= _temp1069; if( _temp1188 == 0){ _throw( Null_Exception);}
_temp1188->tl;}), _temp1073=({ struct Cyc_List_List* _temp1189= _temp1073; if(
_temp1189 == 0){ _throw( Null_Exception);} _temp1189->tl;})){ struct Cyc_Absyn_Structfield*
_temp1191=( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp1190=
_temp1069; if( _temp1190 == 0){ _throw( Null_Exception);} _temp1190->hd;}); goto
_LL1192; _LL1192: { struct Cyc_Absyn_Structfield* _temp1194=( struct Cyc_Absyn_Structfield*)({
struct Cyc_List_List* _temp1193= _temp1073; if( _temp1193 == 0){ _throw(
Null_Exception);} _temp1193->hd;}); goto _LL1195; _LL1195: if( Cyc_String_zstrptrcmp(
_temp1191->name, _temp1194->name) != 0){( void) _throw(( void*) Cyc_Tcutil_Unify);}
Cyc_Tcutil_unify_tqual( _temp1191->tq, _temp1194->tq); Cyc_Tcutil_unify_it((
void*) _temp1191->type,( void*) _temp1194->type); if((( ! Cyc_Tcutil_same_atts(
_temp1191->attributes, _temp1194->attributes)? 1:( _temp1191->width != 0?
_temp1194->width == 0: 0))? 1:( _temp1194->width != 0? _temp1191->width == 0: 0))?
1:(( _temp1191->width != 0? _temp1194->width != 0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp1196= _temp1191->width; if(
_temp1196 == 0){ _throw( Null_Exception);} _temp1196->v;})) != Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp1197= _temp1194->width; if(
_temp1197 == 0){ _throw( Null_Exception);} _temp1197->v;})): 0)){( void) _throw((
void*) Cyc_Tcutil_Unify);}}} if( _temp1069 == 0? _temp1073 == 0: 0){ return;}
goto _LL792; _LL828: return; _LL830: Cyc_Tcutil_unify_it( _temp1085, _temp1081);
return; _LL832: goto _LL834; _LL834: goto _LL836; _LL836: goto _LL838; _LL838:
if( Cyc_Tcutil_unify_effect( t1, t2)){ return;} else{ goto _LL792;} _LL840: goto
_LL792; _LL792:;}( void) _throw(( void*) Cyc_Tcutil_Unify);} int Cyc_Tcutil_is_arithmetic_type(
void* t){ void* _temp1198= Cyc_Tcutil_compress( t); void* _temp1210; void*
_temp1212; struct Cyc_Absyn_Enumdecl* _temp1214; struct _tuple1* _temp1216;
_LL1200: if(( unsigned int) _temp1198 > 4u?*(( int*) _temp1198) == Cyc_Absyn_IntType:
0){ _LL1213: _temp1212=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1198)->f1;
goto _LL1211; _LL1211: _temp1210=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1198)->f2; goto _LL1201;} else{ goto _LL1202;} _LL1202: if( _temp1198 ==(
void*) Cyc_Absyn_FloatType){ goto _LL1203;} else{ goto _LL1204;} _LL1204: if(
_temp1198 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1205;} else{ goto _LL1206;}
_LL1206: if(( unsigned int) _temp1198 > 4u?*(( int*) _temp1198) == Cyc_Absyn_EnumType:
0){ _LL1217: _temp1216=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*)
_temp1198)->f1; goto _LL1215; _LL1215: _temp1214=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_EnumType_struct*) _temp1198)->f2; goto _LL1207;} else{ goto
_LL1208;} _LL1208: goto _LL1209; _LL1201: return 1; _LL1203: return 1; _LL1205:
return 1; _LL1207: return 1; _LL1209: return 0; _LL1199:;} int Cyc_Tcutil_will_lose_precision(
void* t1, void* t2){ t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{
struct _tuple7 _temp1219=({ struct _tuple7 _temp1218; _temp1218.f1= t1;
_temp1218.f2= t2; _temp1218;}); void* _temp1241; void* _temp1243; void*
_temp1245; void* _temp1247; void* _temp1249; void* _temp1251; void* _temp1253;
void* _temp1255; void* _temp1257; void* _temp1259; void* _temp1261; void*
_temp1263; void* _temp1265; void* _temp1267; void* _temp1269; void* _temp1271;
void* _temp1273; void* _temp1275; void* _temp1277; void* _temp1279; void*
_temp1281; void* _temp1283; void* _temp1285; void* _temp1287; void* _temp1289;
void* _temp1291; void* _temp1293; void* _temp1295; void* _temp1297; void*
_temp1299; void* _temp1301; void* _temp1303; void* _temp1305; void* _temp1307;
void* _temp1309; void* _temp1311; void* _temp1313; void* _temp1315; void*
_temp1317; void* _temp1319; void* _temp1321; void* _temp1323; _LL1221: _LL1244:
_temp1243= _temp1219.f1; if( _temp1243 ==( void*) Cyc_Absyn_DoubleType){ goto
_LL1242;} else{ goto _LL1223;} _LL1242: _temp1241= _temp1219.f2; if( _temp1241
==( void*) Cyc_Absyn_FloatType){ goto _LL1222;} else{ goto _LL1223;} _LL1223:
_LL1252: _temp1251= _temp1219.f1; if( _temp1251 ==( void*) Cyc_Absyn_DoubleType){
goto _LL1246;} else{ goto _LL1225;} _LL1246: _temp1245= _temp1219.f2; if((
unsigned int) _temp1245 > 4u?*(( int*) _temp1245) == Cyc_Absyn_IntType: 0){
_LL1250: _temp1249=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1245)->f1;
goto _LL1248; _LL1248: _temp1247=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1245)->f2; goto _LL1224;} else{ goto _LL1225;} _LL1225: _LL1260: _temp1259=
_temp1219.f1; if( _temp1259 ==( void*) Cyc_Absyn_FloatType){ goto _LL1254;}
else{ goto _LL1227;} _LL1254: _temp1253= _temp1219.f2; if(( unsigned int)
_temp1253 > 4u?*(( int*) _temp1253) == Cyc_Absyn_IntType: 0){ _LL1258: _temp1257=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1253)->f1; goto _LL1256; _LL1256:
_temp1255=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1253)->f2; goto
_LL1226;} else{ goto _LL1227;} _LL1227: _LL1268: _temp1267= _temp1219.f1; if((
unsigned int) _temp1267 > 4u?*(( int*) _temp1267) == Cyc_Absyn_IntType: 0){
_LL1272: _temp1271=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1267)->f1;
goto _LL1270; _LL1270: _temp1269=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1267)->f2; if( _temp1269 ==( void*) Cyc_Absyn_B8){ goto _LL1262;} else{
goto _LL1229;}} else{ goto _LL1229;} _LL1262: _temp1261= _temp1219.f2; if((
unsigned int) _temp1261 > 4u?*(( int*) _temp1261) == Cyc_Absyn_IntType: 0){
_LL1266: _temp1265=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1261)->f1;
goto _LL1264; _LL1264: _temp1263=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1261)->f2; if( _temp1263 ==( void*) Cyc_Absyn_B8){ goto _LL1228;} else{
goto _LL1229;}} else{ goto _LL1229;} _LL1229: _LL1276: _temp1275= _temp1219.f1;
if(( unsigned int) _temp1275 > 4u?*(( int*) _temp1275) == Cyc_Absyn_IntType: 0){
_LL1280: _temp1279=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1275)->f1;
goto _LL1278; _LL1278: _temp1277=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1275)->f2; if( _temp1277 ==( void*) Cyc_Absyn_B8){ goto _LL1274;} else{
goto _LL1231;}} else{ goto _LL1231;} _LL1274: _temp1273= _temp1219.f2; goto
_LL1230; _LL1231: _LL1284: _temp1283= _temp1219.f1; if(( unsigned int) _temp1283
> 4u?*(( int*) _temp1283) == Cyc_Absyn_IntType: 0){ _LL1288: _temp1287=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp1283)->f1; goto _LL1286; _LL1286:
_temp1285=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1283)->f2; if(
_temp1285 ==( void*) Cyc_Absyn_B4){ goto _LL1282;} else{ goto _LL1233;}} else{
goto _LL1233;} _LL1282: _temp1281= _temp1219.f2; if( _temp1281 ==( void*) Cyc_Absyn_FloatType){
goto _LL1232;} else{ goto _LL1233;} _LL1233: _LL1296: _temp1295= _temp1219.f1;
if(( unsigned int) _temp1295 > 4u?*(( int*) _temp1295) == Cyc_Absyn_IntType: 0){
_LL1300: _temp1299=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1295)->f1;
goto _LL1298; _LL1298: _temp1297=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1295)->f2; if( _temp1297 ==( void*) Cyc_Absyn_B4){ goto _LL1290;} else{
goto _LL1235;}} else{ goto _LL1235;} _LL1290: _temp1289= _temp1219.f2; if((
unsigned int) _temp1289 > 4u?*(( int*) _temp1289) == Cyc_Absyn_IntType: 0){
_LL1294: _temp1293=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1289)->f1;
goto _LL1292; _LL1292: _temp1291=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1289)->f2; if( _temp1291 ==( void*) Cyc_Absyn_B2){ goto _LL1234;} else{
goto _LL1235;}} else{ goto _LL1235;} _LL1235: _LL1308: _temp1307= _temp1219.f1;
if(( unsigned int) _temp1307 > 4u?*(( int*) _temp1307) == Cyc_Absyn_IntType: 0){
_LL1312: _temp1311=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1307)->f1;
goto _LL1310; _LL1310: _temp1309=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1307)->f2; if( _temp1309 ==( void*) Cyc_Absyn_B4){ goto _LL1302;} else{
goto _LL1237;}} else{ goto _LL1237;} _LL1302: _temp1301= _temp1219.f2; if((
unsigned int) _temp1301 > 4u?*(( int*) _temp1301) == Cyc_Absyn_IntType: 0){
_LL1306: _temp1305=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1301)->f1;
goto _LL1304; _LL1304: _temp1303=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1301)->f2; if( _temp1303 ==( void*) Cyc_Absyn_B1){ goto _LL1236;} else{
goto _LL1237;}} else{ goto _LL1237;} _LL1237: _LL1320: _temp1319= _temp1219.f1;
if(( unsigned int) _temp1319 > 4u?*(( int*) _temp1319) == Cyc_Absyn_IntType: 0){
_LL1324: _temp1323=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1319)->f1;
goto _LL1322; _LL1322: _temp1321=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1319)->f2; if( _temp1321 ==( void*) Cyc_Absyn_B2){ goto _LL1314;} else{
goto _LL1239;}} else{ goto _LL1239;} _LL1314: _temp1313= _temp1219.f2; if((
unsigned int) _temp1313 > 4u?*(( int*) _temp1313) == Cyc_Absyn_IntType: 0){
_LL1318: _temp1317=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1313)->f1;
goto _LL1316; _LL1316: _temp1315=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1313)->f2; if( _temp1315 ==( void*) Cyc_Absyn_B1){ goto _LL1238;} else{
goto _LL1239;}} else{ goto _LL1239;} _LL1239: goto _LL1240; _LL1222: return 1;
_LL1224: return 1; _LL1226: return 1; _LL1228: return 0; _LL1230: return 1;
_LL1232: return 1; _LL1234: return 1; _LL1236: return 1; _LL1238: return 1;
_LL1240: return 0; _LL1220:;}} int Cyc_Tcutil_coerce_list( struct Cyc_Tcenv_Tenv*
te, void* t, struct Cyc_List_List* es){ struct Cyc_Core_Opt* max_arith_type= 0;{
struct Cyc_List_List* el= es; for( 0; el != 0; el=({ struct Cyc_List_List*
_temp1325= el; if( _temp1325 == 0){ _throw( Null_Exception);} _temp1325->tl;})){
void* t1= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp1329=((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1328= el; if( _temp1328 == 0){
_throw( Null_Exception);} _temp1328->hd;}))->topt; if( _temp1329 == 0){ _throw(
Null_Exception);} _temp1329->v;})); if( Cyc_Tcutil_is_arithmetic_type( t1)){ if(
max_arith_type == 0? 1: Cyc_Tcutil_will_lose_precision( t1,( void*)({ struct Cyc_Core_Opt*
_temp1326= max_arith_type; if( _temp1326 == 0){ _throw( Null_Exception);}
_temp1326->v;}))){ max_arith_type=({ struct Cyc_Core_Opt* _temp1327=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1327->v=( void*) t1; _temp1327;});}}}}
if( max_arith_type != 0){ if( ! Cyc_Tcutil_unify( t,( void*)({ struct Cyc_Core_Opt*
_temp1330= max_arith_type; if( _temp1330 == 0){ _throw( Null_Exception);}
_temp1330->v;}))){ return 0;}}{ struct Cyc_List_List* el= es; for( 0; el != 0;
el=({ struct Cyc_List_List* _temp1331= el; if( _temp1331 == 0){ _throw(
Null_Exception);} _temp1331->tl;})){ if( ! Cyc_Tcutil_coerce_assign( te,( struct
Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1332= el; if( _temp1332 == 0){
_throw( Null_Exception);} _temp1332->hd;}), t)){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1333= el; if( _temp1333 == 0){ _throw( Null_Exception);}
_temp1333->hd;}))->loc,({ struct _tagged_string _temp1336= Cyc_Absynpp_typ2string(
t); struct _tagged_string _temp1337= Cyc_Absynpp_typ2string(( void*)({ struct
Cyc_Core_Opt* _temp1335=(( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1334= el; if( _temp1334 == 0){ _throw( Null_Exception);} _temp1334->hd;}))->topt;
if( _temp1335 == 0){ _throw( Null_Exception);} _temp1335->v;})); xprintf("type mismatch: expecting %.*s but found %.*s",
_temp1336.last_plus_one - _temp1336.curr, _temp1336.curr, _temp1337.last_plus_one
- _temp1337.curr, _temp1337.curr);})); return 0;}}} return 1;} int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( ! Cyc_Tcutil_coerce_sint_typ(
te, e)){ void* _temp1339= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp1338= e->topt; if( _temp1338 == 0){ _throw( Null_Exception);} _temp1338->v;}));
struct Cyc_Absyn_PtrInfo _temp1345; _LL1341: if(( unsigned int) _temp1339 > 4u?*((
int*) _temp1339) == Cyc_Absyn_PointerType: 0){ _LL1346: _temp1345=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1339)->f1; goto _LL1342;} else{ goto
_LL1343;} _LL1343: goto _LL1344; _LL1342: Cyc_Tcutil_unchecked_cast( te, e, Cyc_Absyn_uint_t);
goto _LL1340; _LL1344: return 0; _LL1340:;} return 1;} int Cyc_Tcutil_is_integral_type(
void* t){ void* _temp1347= Cyc_Tcutil_compress( t); void* _temp1355; void*
_temp1357; struct Cyc_Absyn_Enumdecl* _temp1359; struct _tuple1* _temp1361;
_LL1349: if(( unsigned int) _temp1347 > 4u?*(( int*) _temp1347) == Cyc_Absyn_IntType:
0){ _LL1358: _temp1357=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1347)->f1;
goto _LL1356; _LL1356: _temp1355=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1347)->f2; goto _LL1350;} else{ goto _LL1351;} _LL1351: if(( unsigned int)
_temp1347 > 4u?*(( int*) _temp1347) == Cyc_Absyn_EnumType: 0){ _LL1362:
_temp1361=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*) _temp1347)->f1;
goto _LL1360; _LL1360: _temp1359=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*)
_temp1347)->f2; goto _LL1352;} else{ goto _LL1353;} _LL1353: goto _LL1354;
_LL1350: return 1; _LL1352: return 1; _LL1354: return 0; _LL1348:;} int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)({
struct Cyc_Core_Opt* _temp1363= e->topt; if( _temp1363 == 0){ _throw(
Null_Exception);} _temp1363->v;}), Cyc_Absyn_uint_t)){ return 1;} if( Cyc_Tcutil_is_integral_type((
void*)({ struct Cyc_Core_Opt* _temp1364= e->topt; if( _temp1364 == 0){ _throw(
Null_Exception);} _temp1364->v;}))){ if( Cyc_Tcutil_will_lose_precision(( void*)({
struct Cyc_Core_Opt* _temp1365= e->topt; if( _temp1365 == 0){ _throw(
Null_Exception);} _temp1365->v;}), Cyc_Absyn_uint_t)){ Cyc_Tcutil_warn( e->loc,(
struct _tagged_string)({ char* _temp1366=( char*)"integral size mismatch; conversion supplied";
struct _tagged_string _temp1367; _temp1367.curr= _temp1366; _temp1367.base=
_temp1366; _temp1367.last_plus_one= _temp1366 + 44; _temp1367;}));} Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_uint_t); return 1;} return 0;} int Cyc_Tcutil_coerce_sint_typ(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)({
struct Cyc_Core_Opt* _temp1368= e->topt; if( _temp1368 == 0){ _throw(
Null_Exception);} _temp1368->v;}), Cyc_Absyn_sint_t)){ return 1;} if( Cyc_Tcutil_is_integral_type((
void*)({ struct Cyc_Core_Opt* _temp1369= e->topt; if( _temp1369 == 0){ _throw(
Null_Exception);} _temp1369->v;}))){ if( Cyc_Tcutil_will_lose_precision(( void*)({
struct Cyc_Core_Opt* _temp1370= e->topt; if( _temp1370 == 0){ _throw(
Null_Exception);} _temp1370->v;}), Cyc_Absyn_sint_t)){ Cyc_Tcutil_warn( e->loc,(
struct _tagged_string)({ char* _temp1371=( char*)"integral size mismatch; conversion supplied";
struct _tagged_string _temp1372; _temp1372.curr= _temp1371; _temp1372.base=
_temp1371; _temp1372.last_plus_one= _temp1371 + 44; _temp1372;}));} Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_sint_t); return 1;} return 0;} int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* t1, void* t2){
t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple7
_temp1374=({ struct _tuple7 _temp1373; _temp1373.f1= t1; _temp1373.f2= t2;
_temp1373;}); void* _temp1386; struct Cyc_Absyn_PtrInfo _temp1388; void*
_temp1390; struct Cyc_Absyn_PtrInfo _temp1392; void* _temp1394; struct Cyc_Absyn_Exp*
_temp1396; struct Cyc_Absyn_Tqual _temp1398; void* _temp1400; void* _temp1402;
struct Cyc_Absyn_Exp* _temp1404; struct Cyc_Absyn_Tqual _temp1406; void*
_temp1408; void* _temp1410; struct Cyc_Absyn_TunionInfo _temp1412; void*
_temp1414; struct Cyc_List_List* _temp1416; void* _temp1418; struct Cyc_Absyn_Tuniondecl*
_temp1420; void* _temp1422; struct Cyc_Absyn_TunionFieldInfo _temp1424; struct
Cyc_List_List* _temp1426; void* _temp1428; struct Cyc_Absyn_Tunionfield*
_temp1430; struct Cyc_Absyn_Tuniondecl* _temp1432; void* _temp1434; struct Cyc_Absyn_TunionInfo
_temp1436; void* _temp1438; struct Cyc_List_List* _temp1440; void* _temp1442;
struct Cyc_Absyn_Tuniondecl* _temp1444; void* _temp1446; struct Cyc_Absyn_PtrInfo
_temp1448; struct Cyc_Absyn_Conref* _temp1450; struct Cyc_Absyn_Tqual _temp1452;
struct Cyc_Absyn_Conref* _temp1454; void* _temp1456; void* _temp1458; _LL1376:
_LL1391: _temp1390= _temp1374.f1; if(( unsigned int) _temp1390 > 4u?*(( int*)
_temp1390) == Cyc_Absyn_PointerType: 0){ _LL1393: _temp1392=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1390)->f1; goto _LL1387;} else{ goto
_LL1378;} _LL1387: _temp1386= _temp1374.f2; if(( unsigned int) _temp1386 > 4u?*((
int*) _temp1386) == Cyc_Absyn_PointerType: 0){ _LL1389: _temp1388=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1386)->f1; goto _LL1377;} else{ goto
_LL1378;} _LL1378: _LL1403: _temp1402= _temp1374.f1; if(( unsigned int)
_temp1402 > 4u?*(( int*) _temp1402) == Cyc_Absyn_ArrayType: 0){ _LL1409:
_temp1408=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1402)->f1; goto
_LL1407; _LL1407: _temp1406=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1402)->f2; goto _LL1405; _LL1405: _temp1404=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1402)->f3; goto _LL1395;} else{ goto
_LL1380;} _LL1395: _temp1394= _temp1374.f2; if(( unsigned int) _temp1394 > 4u?*((
int*) _temp1394) == Cyc_Absyn_ArrayType: 0){ _LL1401: _temp1400=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1394)->f1; goto _LL1399; _LL1399:
_temp1398=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1394)->f2; goto _LL1397; _LL1397: _temp1396=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1394)->f3; goto _LL1379;} else{ goto
_LL1380;} _LL1380: _LL1423: _temp1422= _temp1374.f1; if(( unsigned int)
_temp1422 > 4u?*(( int*) _temp1422) == Cyc_Absyn_TunionFieldType: 0){ _LL1425:
_temp1424=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp1422)->f1; _LL1429: _temp1428=( void*) _temp1424.field_info; if(*(( int*)
_temp1428) == Cyc_Absyn_KnownTunionfield){ _LL1433: _temp1432=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp1428)->f1; goto _LL1431; _LL1431:
_temp1430=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1428)->f2; goto _LL1427;} else{ goto _LL1382;} _LL1427: _temp1426=( struct
Cyc_List_List*) _temp1424.targs; goto _LL1411;} else{ goto _LL1382;} _LL1411:
_temp1410= _temp1374.f2; if(( unsigned int) _temp1410 > 4u?*(( int*) _temp1410)
== Cyc_Absyn_TunionType: 0){ _LL1413: _temp1412=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp1410)->f1; _LL1419: _temp1418=( void*)
_temp1412.tunion_info; if(*(( int*) _temp1418) == Cyc_Absyn_KnownTunion){
_LL1421: _temp1420=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1418)->f1; goto _LL1417;} else{ goto _LL1382;} _LL1417: _temp1416=( struct
Cyc_List_List*) _temp1412.targs; goto _LL1415; _LL1415: _temp1414=( void*)
_temp1412.rgn; goto _LL1381;} else{ goto _LL1382;} _LL1382: _LL1447: _temp1446=
_temp1374.f1; if(( unsigned int) _temp1446 > 4u?*(( int*) _temp1446) == Cyc_Absyn_PointerType:
0){ _LL1449: _temp1448=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1446)->f1; _LL1459: _temp1458=( void*) _temp1448.elt_typ; goto _LL1457;
_LL1457: _temp1456=( void*) _temp1448.rgn_typ; goto _LL1455; _LL1455: _temp1454=(
struct Cyc_Absyn_Conref*) _temp1448.nullable; goto _LL1453; _LL1453: _temp1452=(
struct Cyc_Absyn_Tqual) _temp1448.tq; goto _LL1451; _LL1451: _temp1450=( struct
Cyc_Absyn_Conref*) _temp1448.bounds; goto _LL1435;} else{ goto _LL1384;} _LL1435:
_temp1434= _temp1374.f2; if(( unsigned int) _temp1434 > 4u?*(( int*) _temp1434)
== Cyc_Absyn_TunionType: 0){ _LL1437: _temp1436=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp1434)->f1; _LL1443: _temp1442=( void*)
_temp1436.tunion_info; if(*(( int*) _temp1442) == Cyc_Absyn_KnownTunion){
_LL1445: _temp1444=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1442)->f1; goto _LL1441;} else{ goto _LL1384;} _LL1441: _temp1440=( struct
Cyc_List_List*) _temp1436.targs; goto _LL1439; _LL1439: _temp1438=( void*)
_temp1436.rgn; goto _LL1383;} else{ goto _LL1384;} _LL1384: goto _LL1385;
_LL1377: { int okay= 1; if( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref*
x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp,
_temp1392.nullable, _temp1388.nullable)){ void* _temp1460=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp1392.nullable))->v;
int _temp1466; _LL1462: if(( unsigned int) _temp1460 > 1u?*(( int*) _temp1460)
== Cyc_Absyn_Eq_constr: 0){ _LL1467: _temp1466=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp1460)->f1; goto _LL1463;} else{ goto _LL1464;} _LL1464: goto _LL1465;
_LL1463: okay= ! _temp1466; goto _LL1461; _LL1465:( void) _throw((( void*(*)(
struct _tagged_string msg)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp1468=( char*)"silent_castable conref not eq"; struct _tagged_string
_temp1469; _temp1469.curr= _temp1468; _temp1469.base= _temp1468; _temp1469.last_plus_one=
_temp1468 + 30; _temp1469;}))); _LL1461:;} if( !(( int(*)( int(* cmp)( void*,
void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)(
Cyc_Tcutil_boundscmp, _temp1392.bounds, _temp1388.bounds)){ struct _tuple7
_temp1471=({ struct _tuple7 _temp1470; _temp1470.f1=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp1392.bounds))->v;
_temp1470.f2=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)( _temp1388.bounds))->v; _temp1470;}); void* _temp1479;
void* _temp1481; void* _temp1483; void* _temp1485; struct Cyc_Absyn_Exp*
_temp1487; void* _temp1489; void* _temp1491; void* _temp1493; void* _temp1495;
_LL1473: _LL1484: _temp1483= _temp1471.f1; if(( unsigned int) _temp1483 > 1u?*((
int*) _temp1483) == Cyc_Absyn_Eq_constr: 0){ _LL1486: _temp1485=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp1483)->f1; if(( unsigned int) _temp1485
> 1u?*(( int*) _temp1485) == Cyc_Absyn_Upper_b: 0){ _LL1488: _temp1487=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp1485)->f1; goto _LL1480;}
else{ goto _LL1475;}} else{ goto _LL1475;} _LL1480: _temp1479= _temp1471.f2; if((
unsigned int) _temp1479 > 1u?*(( int*) _temp1479) == Cyc_Absyn_Eq_constr: 0){
_LL1482: _temp1481=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1479)->f1;
if( _temp1481 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1474;} else{ goto _LL1475;}}
else{ goto _LL1475;} _LL1475: _LL1494: _temp1493= _temp1471.f1; if((
unsigned int) _temp1493 > 1u?*(( int*) _temp1493) == Cyc_Absyn_Eq_constr: 0){
_LL1496: _temp1495=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1493)->f1;
if( _temp1495 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1490;} else{ goto _LL1477;}}
else{ goto _LL1477;} _LL1490: _temp1489= _temp1471.f2; if(( unsigned int)
_temp1489 > 1u?*(( int*) _temp1489) == Cyc_Absyn_Eq_constr: 0){ _LL1492:
_temp1491=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1489)->f1; if(
_temp1491 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1476;} else{ goto _LL1477;}}
else{ goto _LL1477;} _LL1477: goto _LL1478; _LL1474: okay= 1; goto _LL1472;
_LL1476: okay= 1; goto _LL1472; _LL1478: okay= 0; goto _LL1472; _LL1472:;}
return(( okay? Cyc_Tcutil_unify(( void*) _temp1392.elt_typ,( void*) _temp1388.elt_typ):
0)? Cyc_Tcutil_unify(( void*) _temp1392.rgn_typ,( void*) _temp1388.rgn_typ): 0)?
!( _temp1392.tq).q_const? 1:( _temp1388.tq).q_const: 0;} _LL1379: { int okay;
okay=( _temp1404 != 0? _temp1396 != 0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1497= _temp1404; if(
_temp1497 == 0){ _throw( Null_Exception);} _temp1497;})) == Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1498= _temp1396; if(
_temp1498 == 0){ _throw( Null_Exception);} _temp1498;})): 0; return( okay? Cyc_Tcutil_unify(
_temp1408, _temp1400): 0)? ! _temp1406.q_const? 1: _temp1398.q_const: 0;}
_LL1381: if(( _temp1432 == _temp1420? 1: Cyc_Absyn_qvar_cmp( _temp1432->name,
_temp1420->name) == 0)? _temp1430->typs == 0: 0){ for( 0; _temp1426 != 0?
_temp1416 != 0: 0; _temp1426=({ struct Cyc_List_List* _temp1499= _temp1426; if(
_temp1499 == 0){ _throw( Null_Exception);} _temp1499->tl;}), _temp1416=({ struct
Cyc_List_List* _temp1500= _temp1416; if( _temp1500 == 0){ _throw( Null_Exception);}
_temp1500->tl;})){ if( ! Cyc_Tcutil_unify(( void*)({ struct Cyc_List_List*
_temp1501= _temp1426; if( _temp1501 == 0){ _throw( Null_Exception);} _temp1501->hd;}),(
void*)({ struct Cyc_List_List* _temp1502= _temp1416; if( _temp1502 == 0){ _throw(
Null_Exception);} _temp1502->hd;}))){ break;}} if( _temp1426 == 0? _temp1416 ==
0: 0){ return 1;}} return 0; _LL1383:{ void* _temp1503= Cyc_Tcutil_compress(
_temp1458); struct Cyc_Absyn_TunionFieldInfo _temp1509; struct Cyc_List_List*
_temp1511; void* _temp1513; struct Cyc_Absyn_Tunionfield* _temp1515; struct Cyc_Absyn_Tuniondecl*
_temp1517; _LL1505: if(( unsigned int) _temp1503 > 4u?*(( int*) _temp1503) ==
Cyc_Absyn_TunionFieldType: 0){ _LL1510: _temp1509=( struct Cyc_Absyn_TunionFieldInfo)((
struct Cyc_Absyn_TunionFieldType_struct*) _temp1503)->f1; _LL1514: _temp1513=(
void*) _temp1509.field_info; if(*(( int*) _temp1513) == Cyc_Absyn_KnownTunionfield){
_LL1518: _temp1517=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1513)->f1; goto _LL1516; _LL1516: _temp1515=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp1513)->f2; goto _LL1512;} else{
goto _LL1507;} _LL1512: _temp1511=( struct Cyc_List_List*) _temp1509.targs; goto
_LL1506;} else{ goto _LL1507;} _LL1507: goto _LL1508; _LL1506: if( ! Cyc_Tcutil_unify(
_temp1456, _temp1438)? _temp1456 !=( void*) Cyc_Absyn_HeapRgn: 0){ return 0;}
if( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp, _temp1454,(( struct Cyc_Absyn_Conref*(*)(
int x)) Cyc_Absyn_new_conref)( 0))){ return 0;} if( !(( int(*)( int(* cmp)( void*,
void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)(
Cyc_Tcutil_boundscmp, _temp1450,(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
Cyc_Absyn_bounds_one))){ return 0;} if( Cyc_Absyn_qvar_cmp( _temp1444->name,
_temp1517->name) == 0? _temp1515->typs != 0: 0){ int okay= 1; for( 0; _temp1511
!= 0? _temp1440 != 0: 0; _temp1511=({ struct Cyc_List_List* _temp1519= _temp1511;
if( _temp1519 == 0){ _throw( Null_Exception);} _temp1519->tl;}), _temp1440=({
struct Cyc_List_List* _temp1520= _temp1440; if( _temp1520 == 0){ _throw(
Null_Exception);} _temp1520->tl;})){ if( ! Cyc_Tcutil_unify(( void*)({ struct
Cyc_List_List* _temp1521= _temp1511; if( _temp1521 == 0){ _throw( Null_Exception);}
_temp1521->hd;}),( void*)({ struct Cyc_List_List* _temp1522= _temp1440; if(
_temp1522 == 0){ _throw( Null_Exception);} _temp1522->hd;}))){ okay= 0; break;}}
if(( ! okay? 1: _temp1511 != 0)? 1: _temp1440 != 0){ return 0;} return 1;} goto
_LL1504; _LL1508: goto _LL1504; _LL1504:;} return 0; _LL1385: return Cyc_Tcutil_unify(
t1, t2); _LL1375:;}} int Cyc_Tcutil_coerce_arg( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e, void* t2){ void* t1= Cyc_Tcutil_compress(( void*)({
struct Cyc_Core_Opt* _temp1527= e->topt; if( _temp1527 == 0){ _throw(
Null_Exception);} _temp1527->v;})); if( Cyc_Tcutil_unify( t1, t2)){ return 1;}
if( Cyc_Tcutil_is_arithmetic_type( t1)? Cyc_Tcutil_is_arithmetic_type( t2): 0){
if( Cyc_Tcutil_will_lose_precision( t1, t2)){ Cyc_Tcutil_warn( e->loc,( struct
_tagged_string)({ char* _temp1523=( char*)"integral size mismatch; conversion supplied";
struct _tagged_string _temp1524; _temp1524.curr= _temp1523; _temp1524.base=
_temp1523; _temp1524.last_plus_one= _temp1523 + 44; _temp1524;}));} Cyc_Tcutil_unchecked_cast(
te, e, t2); return 1;} else{ if( Cyc_Tcutil_silent_castable( te, e->loc, t1, t2)){
Cyc_Tcutil_unchecked_cast( te, e, t2); return 1;} else{ if( Cyc_Tcutil_castable(
te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te, e, t2); Cyc_Tcutil_warn( e->loc,({
struct _tagged_string _temp1525= Cyc_Absynpp_typ2string( t1); struct
_tagged_string _temp1526= Cyc_Absynpp_typ2string( t2); xprintf("implicit cast from %.*s to %.*s",
_temp1525.last_plus_one - _temp1525.curr, _temp1525.curr, _temp1526.last_plus_one
- _temp1526.curr, _temp1526.curr);})); return 1;} else{ return 0;}}}} int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, void* t){ return Cyc_Tcutil_coerce_arg(
te, e, t);} static int Cyc_Tcutil_any_promote( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Exp* e){ void* old_typ= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp1558= e->topt; if( _temp1558 == 0){ _throw( Null_Exception);} _temp1558->v;}));
void* t;{ void* _temp1528= old_typ; struct Cyc_Absyn_Enumdecl* _temp1540; struct
_tuple1* _temp1542; void* _temp1544; void* _temp1546; _LL1530: if(( unsigned int)
_temp1528 > 4u?*(( int*) _temp1528) == Cyc_Absyn_EnumType: 0){ _LL1543:
_temp1542=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*) _temp1528)->f1;
goto _LL1541; _LL1541: _temp1540=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*)
_temp1528)->f2; goto _LL1531;} else{ goto _LL1532;} _LL1532: if(( unsigned int)
_temp1528 > 4u?*(( int*) _temp1528) == Cyc_Absyn_IntType: 0){ _LL1547: _temp1546=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1528)->f1; goto _LL1545; _LL1545:
_temp1544=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1528)->f2; goto
_LL1533;} else{ goto _LL1534;} _LL1534: if( _temp1528 ==( void*) Cyc_Absyn_FloatType){
goto _LL1535;} else{ goto _LL1536;} _LL1536: if( _temp1528 ==( void*) Cyc_Absyn_DoubleType){
goto _LL1537;} else{ goto _LL1538;} _LL1538: goto _LL1539; _LL1531: t= Cyc_Absyn_sint_t;
goto _LL1529; _LL1533:{ void* _temp1548= _temp1544; _LL1550: if( _temp1548 ==(
void*) Cyc_Absyn_B1){ goto _LL1551;} else{ goto _LL1552;} _LL1552: if( _temp1548
==( void*) Cyc_Absyn_B2){ goto _LL1553;} else{ goto _LL1554;} _LL1554: if(
_temp1548 ==( void*) Cyc_Absyn_B4){ goto _LL1555;} else{ goto _LL1556;} _LL1556:
if( _temp1548 ==( void*) Cyc_Absyn_B8){ goto _LL1557;} else{ goto _LL1549;}
_LL1551: t= Cyc_Absyn_sint_t; goto _LL1549; _LL1553: t= Cyc_Absyn_sint_t; goto
_LL1549; _LL1555: t= _temp1546 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t:
Cyc_Absyn_sint_t; goto _LL1549; _LL1557: t= _temp1546 ==( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_ulong_t: Cyc_Absyn_slong_t; goto _LL1549; _LL1549:;} goto _LL1529;
_LL1535: t= Cyc_Absyn_float_t; goto _LL1529; _LL1537: t= Cyc_Absyn_double_t;
goto _LL1529; _LL1539: return 0; _LL1529:;} Cyc_Tcutil_unchecked_cast( te, e, t);
return 1;} int Cyc_Tcutil_coerce_use( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, void* t2){ void* t1= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp1562= e->topt; if( _temp1562 == 0){ _throw( Null_Exception);} _temp1562->v;}));
if( Cyc_Tcutil_unify( t1, t2)){ return 1;} Cyc_Tcutil_any_promote( te, e); if(
Cyc_Tcutil_unify(( void*)({ struct Cyc_Core_Opt* _temp1559= e->topt; if(
_temp1559 == 0){ _throw( Null_Exception);} _temp1559->v;}), t2)){ return 1;}
else{ if( Cyc_Tcutil_silent_castable( te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast(
te, e, t2); return 1;} else{ if( Cyc_Tcutil_castable( te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast(
te, e, t2); Cyc_Tcutil_warn( e->loc,({ struct _tagged_string _temp1560= Cyc_Absynpp_typ2string(
t1); struct _tagged_string _temp1561= Cyc_Absynpp_typ2string( t2); xprintf("implicit cast from %.*s to %.*s",
_temp1560.last_plus_one - _temp1560.curr, _temp1560.curr, _temp1561.last_plus_one
- _temp1561.curr, _temp1561.curr);})); return 1;} else{ return 0;}}}} int Cyc_Tcutil_coerceable(
void* t){ void* _temp1563= Cyc_Tcutil_compress( t); void* _temp1573; void*
_temp1575; _LL1565: if(( unsigned int) _temp1563 > 4u?*(( int*) _temp1563) ==
Cyc_Absyn_IntType: 0){ _LL1576: _temp1575=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1563)->f1; goto _LL1574; _LL1574: _temp1573=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1563)->f2; goto _LL1566;} else{ goto _LL1567;} _LL1567: if( _temp1563 ==(
void*) Cyc_Absyn_FloatType){ goto _LL1568;} else{ goto _LL1569;} _LL1569: if(
_temp1563 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1570;} else{ goto _LL1571;}
_LL1571: goto _LL1572; _LL1566: return 1; _LL1568: return 1; _LL1570: return 1;
_LL1572: return 0; _LL1564:;} static struct _tuple4* Cyc_Tcutil_flatten_typ_f(
struct Cyc_List_List* inst, struct Cyc_Absyn_Structfield* x){ return({ struct
_tuple4* _temp1577=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1577->f1= x->tq; _temp1577->f2= Cyc_Tcutil_substitute( inst,( void*) x->type);
_temp1577;});} static struct Cyc_List_List* Cyc_Tcutil_flatten_typ( struct Cyc_Tcenv_Tenv*
te, void* t1){ t1= Cyc_Tcutil_compress( t1);{ void* _temp1578= t1; struct Cyc_List_List*
_temp1590; struct Cyc_Absyn_Structdecl** _temp1592; struct Cyc_List_List*
_temp1594; struct _tuple1* _temp1596; struct Cyc_List_List* _temp1598; _LL1580:
if( _temp1578 ==( void*) Cyc_Absyn_VoidType){ goto _LL1581;} else{ goto _LL1582;}
_LL1582: if(( unsigned int) _temp1578 > 4u?*(( int*) _temp1578) == Cyc_Absyn_TupleType:
0){ _LL1591: _temp1590=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp1578)->f1; goto _LL1583;} else{ goto _LL1584;} _LL1584: if(( unsigned int)
_temp1578 > 4u?*(( int*) _temp1578) == Cyc_Absyn_StructType: 0){ _LL1597:
_temp1596=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp1578)->f1;
goto _LL1595; _LL1595: _temp1594=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp1578)->f2; goto _LL1593; _LL1593: _temp1592=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp1578)->f3; goto _LL1585;} else{ goto
_LL1586;} _LL1586: if(( unsigned int) _temp1578 > 4u?*(( int*) _temp1578) == Cyc_Absyn_AnonStructType:
0){ _LL1599: _temp1598=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp1578)->f1; goto _LL1587;} else{ goto _LL1588;} _LL1588: goto _LL1589;
_LL1581: return 0; _LL1583: return _temp1590; _LL1585: if( _temp1592 == 0? 1:
_temp1596 == 0){ return({ struct Cyc_List_List* _temp1600=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1600->hd=( void*)({ struct
_tuple4* _temp1601=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1601->f1= Cyc_Absyn_empty_tqual(); _temp1601->f2= t1; _temp1601;});
_temp1600->tl= 0; _temp1600;});}{ struct Cyc_Absyn_Structdecl* _temp1603=({
struct Cyc_Absyn_Structdecl** _temp1602= _temp1592; if( _temp1602 == 0){ _throw(
Null_Exception);}* _temp1602;}); goto _LL1604; _LL1604: { struct _tuple1
_temp1606=({ struct _tuple1* _temp1605= _temp1596; if( _temp1605 == 0){ _throw(
Null_Exception);}* _temp1605;}); goto _LL1607; _LL1607: { struct Cyc_List_List*
_temp1608=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_zip)( _temp1603->tvs, _temp1594); goto _LL1609; _LL1609: if(
_temp1603->fields == 0){ return({ struct Cyc_List_List* _temp1610=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1610->hd=( void*)({ struct
_tuple4* _temp1611=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1611->f1= Cyc_Absyn_empty_tqual(); _temp1611->f2= t1; _temp1611;});
_temp1610->tl= 0; _temp1610;});} return(( struct Cyc_List_List*(*)( struct
_tuple4*(* f)( struct Cyc_List_List*, struct Cyc_Absyn_Structfield*), struct Cyc_List_List*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f,
_temp1608,( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp1612= _temp1603->fields;
if( _temp1612 == 0){ _throw( Null_Exception);} _temp1612->v;}));}}} _LL1587:
return(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_List_List*,
struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, 0, _temp1598); _LL1589: return({
struct Cyc_List_List* _temp1613=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1613->hd=( void*)({ struct _tuple4* _temp1614=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1614->f1= Cyc_Absyn_empty_tqual();
_temp1614->f2= t1; _temp1614;}); _temp1613->tl= 0; _temp1613;}); _LL1579:;}}
static int Cyc_Tcutil_ptrsubtype( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
assume, void* t1, void* t2); static int Cyc_Tcutil_subtype( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* assume, void* t1, void* t2){ if( Cyc_Tcutil_unify( t1,
t2)){ return 1;}{ struct Cyc_List_List* a= assume; for( 0; a != 0; a=({ struct
Cyc_List_List* _temp1615= a; if( _temp1615 == 0){ _throw( Null_Exception);}
_temp1615->tl;})){ if( Cyc_Tcutil_unify( t1,(*(( struct _tuple7*)({ struct Cyc_List_List*
_temp1616= a; if( _temp1616 == 0){ _throw( Null_Exception);} _temp1616->hd;}))).f1)?
Cyc_Tcutil_unify( t2,(*(( struct _tuple7*)({ struct Cyc_List_List* _temp1617= a;
if( _temp1617 == 0){ _throw( Null_Exception);} _temp1617->hd;}))).f2): 0){
return 1;}}} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct
_tuple7 _temp1619=({ struct _tuple7 _temp1618; _temp1618.f1= t1; _temp1618.f2=
t2; _temp1618;}); void* _temp1625; struct Cyc_Absyn_PtrInfo _temp1627; struct
Cyc_Absyn_Conref* _temp1629; struct Cyc_Absyn_Tqual _temp1631; struct Cyc_Absyn_Conref*
_temp1633; void* _temp1635; void* _temp1637; void* _temp1639; struct Cyc_Absyn_PtrInfo
_temp1641; struct Cyc_Absyn_Conref* _temp1643; struct Cyc_Absyn_Tqual _temp1645;
struct Cyc_Absyn_Conref* _temp1647; void* _temp1649; void* _temp1651; _LL1621:
_LL1640: _temp1639= _temp1619.f1; if(( unsigned int) _temp1639 > 4u?*(( int*)
_temp1639) == Cyc_Absyn_PointerType: 0){ _LL1642: _temp1641=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1639)->f1; _LL1652: _temp1651=( void*)
_temp1641.elt_typ; goto _LL1650; _LL1650: _temp1649=( void*) _temp1641.rgn_typ;
goto _LL1648; _LL1648: _temp1647=( struct Cyc_Absyn_Conref*) _temp1641.nullable;
goto _LL1646; _LL1646: _temp1645=( struct Cyc_Absyn_Tqual) _temp1641.tq; goto
_LL1644; _LL1644: _temp1643=( struct Cyc_Absyn_Conref*) _temp1641.bounds; goto
_LL1626;} else{ goto _LL1623;} _LL1626: _temp1625= _temp1619.f2; if((
unsigned int) _temp1625 > 4u?*(( int*) _temp1625) == Cyc_Absyn_PointerType: 0){
_LL1628: _temp1627=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1625)->f1; _LL1638: _temp1637=( void*) _temp1627.elt_typ; goto _LL1636;
_LL1636: _temp1635=( void*) _temp1627.rgn_typ; goto _LL1634; _LL1634: _temp1633=(
struct Cyc_Absyn_Conref*) _temp1627.nullable; goto _LL1632; _LL1632: _temp1631=(
struct Cyc_Absyn_Tqual) _temp1627.tq; goto _LL1630; _LL1630: _temp1629=( struct
Cyc_Absyn_Conref*) _temp1627.bounds; goto _LL1622;} else{ goto _LL1623;} _LL1623:
goto _LL1624; _LL1622: if( _temp1645.q_const? ! _temp1631.q_const: 0){ return 0;}
if(( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp, _temp1647, _temp1633)?(( int(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp1647): 0)? !(( int(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp1633): 0){ return 0;}{
void* _temp1653= _temp1649; _LL1655: if( _temp1653 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL1656;} else{ goto _LL1657;} _LL1657: goto _LL1658; _LL1656: goto _LL1654;
_LL1658: Cyc_Tcutil_unify( _temp1649, _temp1635); goto _LL1654; _LL1654:;} if( !((
int(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y)) Cyc_Tcutil_unify_conrefs)( Cyc_Tcutil_boundscmp, _temp1643, _temp1629)){
struct _tuple7 _temp1660=({ struct _tuple7 _temp1659; _temp1659.f1=(( void*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp1643); _temp1659.f2=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp1629);
_temp1659;}); void* _temp1668; void* _temp1670; struct Cyc_Absyn_Exp* _temp1672;
void* _temp1674; struct Cyc_Absyn_Exp* _temp1676; void* _temp1678; struct Cyc_Absyn_Exp*
_temp1680; _LL1662: _LL1671: _temp1670= _temp1660.f1; if(( unsigned int)
_temp1670 > 1u?*(( int*) _temp1670) == Cyc_Absyn_Upper_b: 0){ _LL1673: _temp1672=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp1670)->f1; goto
_LL1669;} else{ goto _LL1664;} _LL1669: _temp1668= _temp1660.f2; if( _temp1668
==( void*) Cyc_Absyn_Unknown_b){ goto _LL1663;} else{ goto _LL1664;} _LL1664:
_LL1679: _temp1678= _temp1660.f1; if(( unsigned int) _temp1678 > 1u?*(( int*)
_temp1678) == Cyc_Absyn_Upper_b: 0){ _LL1681: _temp1680=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Upper_b_struct*) _temp1678)->f1; goto _LL1675;} else{ goto
_LL1666;} _LL1675: _temp1674= _temp1660.f2; if(( unsigned int) _temp1674 > 1u?*((
int*) _temp1674) == Cyc_Absyn_Upper_b: 0){ _LL1677: _temp1676=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Upper_b_struct*) _temp1674)->f1; goto _LL1665;} else{ goto
_LL1666;} _LL1666: goto _LL1667; _LL1663: goto _LL1661; _LL1665: if( Cyc_Evexp_eval_const_uint_exp(
_temp1680) < Cyc_Evexp_eval_const_uint_exp( _temp1676)){ return 0;} goto _LL1661;
_LL1667: return 0; _LL1661:;} return Cyc_Tcutil_ptrsubtype( te,({ struct Cyc_List_List*
_temp1682=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1682->hd=( void*)({ struct _tuple7* _temp1683=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp1683->f1= t1; _temp1683->f2= t2; _temp1683;});
_temp1682->tl= assume; _temp1682;}), _temp1651, _temp1637); _LL1624: return 0;
_LL1620:;}} static int Cyc_Tcutil_ptrsubtype( struct Cyc_Tcenv_Tenv* te, struct
Cyc_List_List* assume, void* t1, void* t2){ struct Cyc_List_List* tqs1= Cyc_Tcutil_flatten_typ(
te, t1); struct Cyc_List_List* tqs2= Cyc_Tcutil_flatten_typ( te, t2); for( 0;
tqs2 != 0; tqs2=({ struct Cyc_List_List* _temp1684= tqs2; if( _temp1684 == 0){
_throw( Null_Exception);} _temp1684->tl;}), tqs1=({ struct Cyc_List_List*
_temp1685= tqs1; if( _temp1685 == 0){ _throw( Null_Exception);} _temp1685->tl;})){
if( tqs1 == 0){ return 0;}{ struct _tuple4 _temp1689; void* _temp1690; struct
Cyc_Absyn_Tqual _temp1692; struct _tuple4* _temp1687=( struct _tuple4*)({ struct
Cyc_List_List* _temp1686= tqs1; if( _temp1686 == 0){ _throw( Null_Exception);}
_temp1686->hd;}); _temp1689=* _temp1687; _LL1693: _temp1692= _temp1689.f1; goto
_LL1691; _LL1691: _temp1690= _temp1689.f2; goto _LL1688; _LL1688: { struct
_tuple4 _temp1697; void* _temp1698; struct Cyc_Absyn_Tqual _temp1700; struct
_tuple4* _temp1695=( struct _tuple4*)({ struct Cyc_List_List* _temp1694= tqs2;
if( _temp1694 == 0){ _throw( Null_Exception);} _temp1694->hd;}); _temp1697=*
_temp1695; _LL1701: _temp1700= _temp1697.f1; goto _LL1699; _LL1699: _temp1698=
_temp1697.f2; goto _LL1696; _LL1696: if( _temp1700.q_const? Cyc_Tcutil_subtype(
te, assume, _temp1690, _temp1698): 0){ continue;} else{ if( Cyc_Tcutil_unify(
_temp1690, _temp1698)){ continue;} else{ return 0;}}}}} return 1;} int Cyc_Tcutil_castable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* t1, void* t2){
if( Cyc_Tcutil_unify( t1, t2)){ return 1;} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress(
t2);{ void* _temp1702= t2; struct Cyc_Absyn_Uniondecl** _temp1710; struct Cyc_List_List*
_temp1712; struct _tuple1* _temp1714; void* _temp1716; void* _temp1718; _LL1704:
if(( unsigned int) _temp1702 > 4u?*(( int*) _temp1702) == Cyc_Absyn_UnionType: 0){
_LL1715: _temp1714=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp1702)->f1; goto _LL1713; _LL1713: _temp1712=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnionType_struct*) _temp1702)->f2; goto _LL1711; _LL1711:
_temp1710=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp1702)->f3; goto _LL1705;} else{ goto _LL1706;} _LL1706: if(( unsigned int)
_temp1702 > 4u?*(( int*) _temp1702) == Cyc_Absyn_IntType: 0){ _LL1719: _temp1718=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1702)->f1; goto _LL1717; _LL1717:
_temp1716=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1702)->f2; if(
_temp1716 ==( void*) Cyc_Absyn_B4){ goto _LL1707;} else{ goto _LL1708;}} else{
goto _LL1708;} _LL1708: goto _LL1709; _LL1705: if( _temp1710 == 0){ goto _LL1703;}{
struct Cyc_Absyn_Uniondecl* _temp1721=({ struct Cyc_Absyn_Uniondecl** _temp1720=
_temp1710; if( _temp1720 == 0){ _throw( Null_Exception);}* _temp1720;}); goto
_LL1722; _LL1722: if( _temp1721->fields == 0){ goto _LL1703;}{ struct Cyc_List_List*
_temp1723=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_zip)( _temp1721->tvs, _temp1712); goto _LL1724; _LL1724:{ struct
Cyc_List_List* fs=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp1727=
_temp1721->fields; if( _temp1727 == 0){ _throw( Null_Exception);} _temp1727->v;});
for( 0; fs != 0; fs=({ struct Cyc_List_List* _temp1725= fs; if( _temp1725 == 0){
_throw( Null_Exception);} _temp1725->tl;})){ if( Cyc_Tcutil_unify( t1, Cyc_Tcutil_substitute(
_temp1723,( void*)(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp1726= fs; if( _temp1726 == 0){ _throw( Null_Exception);} _temp1726->hd;}))->type))){
return 1;}}} goto _LL1703;}} _LL1707: if( Cyc_Tcutil_typ_kind( t2) ==( void*)
Cyc_Absyn_BoxKind){ return 1;} goto _LL1703; _LL1709: goto _LL1703; _LL1703:;}{
void* _temp1728= t1; struct Cyc_Absyn_PtrInfo _temp1744; struct Cyc_Absyn_Conref*
_temp1746; struct Cyc_Absyn_Tqual _temp1748; struct Cyc_Absyn_Conref* _temp1750;
void* _temp1752; void* _temp1754; struct Cyc_Absyn_Exp* _temp1756; struct Cyc_Absyn_Tqual
_temp1758; void* _temp1760; struct Cyc_Absyn_Enumdecl* _temp1762; struct _tuple1*
_temp1764; void* _temp1766; void* _temp1768; _LL1730: if(( unsigned int)
_temp1728 > 4u?*(( int*) _temp1728) == Cyc_Absyn_PointerType: 0){ _LL1745:
_temp1744=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1728)->f1; _LL1755: _temp1754=( void*) _temp1744.elt_typ; goto _LL1753;
_LL1753: _temp1752=( void*) _temp1744.rgn_typ; goto _LL1751; _LL1751: _temp1750=(
struct Cyc_Absyn_Conref*) _temp1744.nullable; goto _LL1749; _LL1749: _temp1748=(
struct Cyc_Absyn_Tqual) _temp1744.tq; goto _LL1747; _LL1747: _temp1746=( struct
Cyc_Absyn_Conref*) _temp1744.bounds; goto _LL1731;} else{ goto _LL1732;} _LL1732:
if(( unsigned int) _temp1728 > 4u?*(( int*) _temp1728) == Cyc_Absyn_ArrayType: 0){
_LL1761: _temp1760=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1728)->f1;
goto _LL1759; _LL1759: _temp1758=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1728)->f2; goto _LL1757; _LL1757: _temp1756=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1728)->f3; goto _LL1733;} else{ goto
_LL1734;} _LL1734: if(( unsigned int) _temp1728 > 4u?*(( int*) _temp1728) == Cyc_Absyn_EnumType:
0){ _LL1765: _temp1764=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*)
_temp1728)->f1; goto _LL1763; _LL1763: _temp1762=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_EnumType_struct*) _temp1728)->f2; goto _LL1735;} else{ goto
_LL1736;} _LL1736: if(( unsigned int) _temp1728 > 4u?*(( int*) _temp1728) == Cyc_Absyn_IntType:
0){ _LL1769: _temp1768=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1728)->f1;
goto _LL1767; _LL1767: _temp1766=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1728)->f2; goto _LL1737;} else{ goto _LL1738;} _LL1738: if( _temp1728 ==(
void*) Cyc_Absyn_FloatType){ goto _LL1739;} else{ goto _LL1740;} _LL1740: if(
_temp1728 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1741;} else{ goto _LL1742;}
_LL1742: goto _LL1743; _LL1731:{ void* _temp1770= t2; struct Cyc_Absyn_PtrInfo
_temp1776; struct Cyc_Absyn_Conref* _temp1778; struct Cyc_Absyn_Tqual _temp1780;
struct Cyc_Absyn_Conref* _temp1782; void* _temp1784; void* _temp1786; _LL1772:
if(( unsigned int) _temp1770 > 4u?*(( int*) _temp1770) == Cyc_Absyn_PointerType:
0){ _LL1777: _temp1776=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1770)->f1; _LL1787: _temp1786=( void*) _temp1776.elt_typ; goto _LL1785;
_LL1785: _temp1784=( void*) _temp1776.rgn_typ; goto _LL1783; _LL1783: _temp1782=(
struct Cyc_Absyn_Conref*) _temp1776.nullable; goto _LL1781; _LL1781: _temp1780=(
struct Cyc_Absyn_Tqual) _temp1776.tq; goto _LL1779; _LL1779: _temp1778=( struct
Cyc_Absyn_Conref*) _temp1776.bounds; goto _LL1773;} else{ goto _LL1774;} _LL1774:
goto _LL1775; _LL1773: { struct Cyc_List_List* _temp1790=({ struct Cyc_List_List*
_temp1788=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1788->hd=( void*)({ struct _tuple7* _temp1789=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp1789->f1= t1; _temp1789->f2= t2; _temp1789;});
_temp1788->tl= 0; _temp1788;}); goto _LL1791; _LL1791: { int _temp1792= Cyc_Tcutil_ptrsubtype(
te, _temp1790, _temp1754, _temp1786)? ! _temp1748.q_const? 1: _temp1780.q_const:
0; goto _LL1793; _LL1793: { int bounds_ok=(( int(*)( int(* cmp)( void*, void*),
struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)(
Cyc_Tcutil_boundscmp, _temp1746, _temp1778); if( ! bounds_ok){ struct _tuple7
_temp1795=({ struct _tuple7 _temp1794; _temp1794.f1=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp1746); _temp1794.f2=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp1778); _temp1794;}); void* _temp1801; struct Cyc_Absyn_Exp*
_temp1803; void* _temp1805; struct Cyc_Absyn_Exp* _temp1807; _LL1797: _LL1806:
_temp1805= _temp1795.f1; if(( unsigned int) _temp1805 > 1u?*(( int*) _temp1805)
== Cyc_Absyn_Upper_b: 0){ _LL1808: _temp1807=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Upper_b_struct*) _temp1805)->f1; goto _LL1802;} else{ goto _LL1799;}
_LL1802: _temp1801= _temp1795.f2; if(( unsigned int) _temp1801 > 1u?*(( int*)
_temp1801) == Cyc_Absyn_Upper_b: 0){ _LL1804: _temp1803=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Upper_b_struct*) _temp1801)->f1; goto _LL1798;} else{ goto
_LL1799;} _LL1799: goto _LL1800; _LL1798: if( Cyc_Evexp_eval_const_uint_exp(
_temp1807) >= Cyc_Evexp_eval_const_uint_exp( _temp1803)){ bounds_ok= 1;} goto
_LL1796; _LL1800: bounds_ok= 1; goto _LL1796; _LL1796:;} return( bounds_ok?
_temp1792: 0)? _temp1752 ==( void*) Cyc_Absyn_HeapRgn? 1: Cyc_Tcutil_unify(
_temp1752, _temp1784): 0;}}} _LL1775: goto _LL1771; _LL1771:;} return 0; _LL1733:{
void* _temp1809= t2; struct Cyc_Absyn_Exp* _temp1815; struct Cyc_Absyn_Tqual
_temp1817; void* _temp1819; _LL1811: if(( unsigned int) _temp1809 > 4u?*(( int*)
_temp1809) == Cyc_Absyn_ArrayType: 0){ _LL1820: _temp1819=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1809)->f1; goto _LL1818; _LL1818: _temp1817=( struct Cyc_Absyn_Tqual)((
struct Cyc_Absyn_ArrayType_struct*) _temp1809)->f2; goto _LL1816; _LL1816:
_temp1815=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1809)->f3; goto _LL1812;} else{ goto _LL1813;} _LL1813: goto _LL1814;
_LL1812: { int okay; okay=( _temp1756 != 0? _temp1815 != 0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1821= _temp1756; if(
_temp1821 == 0){ _throw( Null_Exception);} _temp1821;})) >= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1822= _temp1815; if(
_temp1822 == 0){ _throw( Null_Exception);} _temp1822;})): 0; return( okay? Cyc_Tcutil_unify(
_temp1760, _temp1819): 0)? ! _temp1758.q_const? 1: _temp1817.q_const: 0;}
_LL1814: return 0; _LL1810:;} return 0; _LL1735: goto _LL1737; _LL1737: goto
_LL1739; _LL1739: goto _LL1741; _LL1741: return Cyc_Tcutil_coerceable( t2);
_LL1743: return 0; _LL1729:;}} void Cyc_Tcutil_unchecked_cast( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e, void* t){ if( ! Cyc_Tcutil_unify(( void*)({ struct
Cyc_Core_Opt* _temp1823= e->topt; if( _temp1823 == 0){ _throw( Null_Exception);}
_temp1823->v;}), t)){ struct Cyc_Absyn_Exp* _temp1824= Cyc_Absyn_copy_exp( e);
goto _LL1825; _LL1825:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Cast_e_struct*
_temp1826=( struct Cyc_Absyn_Cast_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct));
_temp1826[ 0]=({ struct Cyc_Absyn_Cast_e_struct _temp1827; _temp1827.tag= Cyc_Absyn_Cast_e;
_temp1827.f1=( void*) t; _temp1827.f2= _temp1824; _temp1827;}); _temp1826;})));
e->topt=({ struct Cyc_Core_Opt* _temp1828=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1828->v=( void*) t; _temp1828;});}} int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp* e){ void* _temp1830= Cyc_Tcutil_compress(( void*)({ struct
Cyc_Core_Opt* _temp1829= e->topt; if( _temp1829 == 0){ _throw( Null_Exception);}
_temp1829->v;})); struct Cyc_Absyn_Enumdecl* _temp1840; struct _tuple1*
_temp1842; void* _temp1844; void* _temp1846; int _temp1848; struct Cyc_Core_Opt*
_temp1850; void* _temp1852; _LL1832: if(( unsigned int) _temp1830 > 4u?*(( int*)
_temp1830) == Cyc_Absyn_EnumType: 0){ _LL1843: _temp1842=( struct _tuple1*)((
struct Cyc_Absyn_EnumType_struct*) _temp1830)->f1; goto _LL1841; _LL1841:
_temp1840=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*)
_temp1830)->f2; goto _LL1833;} else{ goto _LL1834;} _LL1834: if(( unsigned int)
_temp1830 > 4u?*(( int*) _temp1830) == Cyc_Absyn_IntType: 0){ _LL1847: _temp1846=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1830)->f1; goto _LL1845; _LL1845:
_temp1844=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1830)->f2; goto
_LL1835;} else{ goto _LL1836;} _LL1836: if(( unsigned int) _temp1830 > 4u?*((
int*) _temp1830) == Cyc_Absyn_Evar: 0){ _LL1853: _temp1852=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp1830)->f1; goto _LL1851; _LL1851: _temp1850=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Evar_struct*) _temp1830)->f2; goto _LL1849; _LL1849: _temp1848=(
int)(( struct Cyc_Absyn_Evar_struct*) _temp1830)->f3; goto _LL1837;} else{ goto
_LL1838;} _LL1838: goto _LL1839; _LL1833: goto _LL1835; _LL1835: return 1;
_LL1837: if( Cyc_Tcutil_unify(( void*)({ struct Cyc_Core_Opt* _temp1854= e->topt;
if( _temp1854 == 0){ _throw( Null_Exception);} _temp1854->v;}), Cyc_Absyn_sint_t)){
return 1;} return 0; _LL1839: return 0; _LL1831:;} int Cyc_Tcutil_is_numeric(
struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_is_integral( e)){ return 1;}{ void*
_temp1856= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp1855= e->topt;
if( _temp1855 == 0){ _throw( Null_Exception);} _temp1855->v;})); _LL1858: if(
_temp1856 ==( void*) Cyc_Absyn_FloatType){ goto _LL1859;} else{ goto _LL1860;}
_LL1860: if( _temp1856 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1861;} else{
goto _LL1862;} _LL1862: goto _LL1863; _LL1859: goto _LL1861; _LL1861: return 1;
_LL1863: return 0; _LL1857:;}} int Cyc_Tcutil_is_function_type( void* t){ void*
_temp1864= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo _temp1870; _LL1866:
if(( unsigned int) _temp1864 > 4u?*(( int*) _temp1864) == Cyc_Absyn_FnType: 0){
_LL1871: _temp1870=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp1864)->f1; goto _LL1867;} else{ goto _LL1868;} _LL1868: goto _LL1869;
_LL1867: return 1; _LL1869: return 0; _LL1865:;} void* Cyc_Tcutil_max_arithmetic_type(
void* t1, void* t2){ struct _tuple7 _temp1873=({ struct _tuple7 _temp1872;
_temp1872.f1= t1; _temp1872.f2= t2; _temp1872;}); void* _temp1897; void*
_temp1899; void* _temp1901; void* _temp1903; void* _temp1905; void* _temp1907;
void* _temp1909; void* _temp1911; void* _temp1913; void* _temp1915; void*
_temp1917; void* _temp1919; void* _temp1921; void* _temp1923; void* _temp1925;
void* _temp1927; void* _temp1929; void* _temp1931; void* _temp1933; void*
_temp1935; void* _temp1937; void* _temp1939; void* _temp1941; void* _temp1943;
void* _temp1945; void* _temp1947; void* _temp1949; void* _temp1951; void*
_temp1953; void* _temp1955; void* _temp1957; void* _temp1959; _LL1875: _LL1900:
_temp1899= _temp1873.f1; if( _temp1899 ==( void*) Cyc_Absyn_DoubleType){ goto
_LL1898;} else{ goto _LL1877;} _LL1898: _temp1897= _temp1873.f2; goto _LL1876;
_LL1877: _LL1904: _temp1903= _temp1873.f1; goto _LL1902; _LL1902: _temp1901=
_temp1873.f2; if( _temp1901 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1878;}
else{ goto _LL1879;} _LL1879: _LL1908: _temp1907= _temp1873.f1; if( _temp1907 ==(
void*) Cyc_Absyn_FloatType){ goto _LL1906;} else{ goto _LL1881;} _LL1906:
_temp1905= _temp1873.f2; goto _LL1880; _LL1881: _LL1912: _temp1911= _temp1873.f1;
goto _LL1910; _LL1910: _temp1909= _temp1873.f2; if( _temp1909 ==( void*) Cyc_Absyn_FloatType){
goto _LL1882;} else{ goto _LL1883;} _LL1883: _LL1916: _temp1915= _temp1873.f1;
if(( unsigned int) _temp1915 > 4u?*(( int*) _temp1915) == Cyc_Absyn_IntType: 0){
_LL1920: _temp1919=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1915)->f1;
if( _temp1919 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1918;} else{ goto _LL1885;}
_LL1918: _temp1917=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1915)->f2;
if( _temp1917 ==( void*) Cyc_Absyn_B8){ goto _LL1914;} else{ goto _LL1885;}}
else{ goto _LL1885;} _LL1914: _temp1913= _temp1873.f2; goto _LL1884; _LL1885:
_LL1928: _temp1927= _temp1873.f1; goto _LL1922; _LL1922: _temp1921= _temp1873.f2;
if(( unsigned int) _temp1921 > 4u?*(( int*) _temp1921) == Cyc_Absyn_IntType: 0){
_LL1926: _temp1925=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1921)->f1;
if( _temp1925 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1924;} else{ goto _LL1887;}
_LL1924: _temp1923=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1921)->f2;
if( _temp1923 ==( void*) Cyc_Absyn_B8){ goto _LL1886;} else{ goto _LL1887;}}
else{ goto _LL1887;} _LL1887: _LL1932: _temp1931= _temp1873.f1; if((
unsigned int) _temp1931 > 4u?*(( int*) _temp1931) == Cyc_Absyn_IntType: 0){
_LL1936: _temp1935=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1931)->f1;
if( _temp1935 ==( void*) Cyc_Absyn_Signed){ goto _LL1934;} else{ goto _LL1889;}
_LL1934: _temp1933=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1931)->f2;
if( _temp1933 ==( void*) Cyc_Absyn_B8){ goto _LL1930;} else{ goto _LL1889;}}
else{ goto _LL1889;} _LL1930: _temp1929= _temp1873.f2; goto _LL1888; _LL1889:
_LL1944: _temp1943= _temp1873.f1; goto _LL1938; _LL1938: _temp1937= _temp1873.f2;
if(( unsigned int) _temp1937 > 4u?*(( int*) _temp1937) == Cyc_Absyn_IntType: 0){
_LL1942: _temp1941=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1937)->f1;
if( _temp1941 ==( void*) Cyc_Absyn_Signed){ goto _LL1940;} else{ goto _LL1891;}
_LL1940: _temp1939=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1937)->f2;
if( _temp1939 ==( void*) Cyc_Absyn_B8){ goto _LL1890;} else{ goto _LL1891;}}
else{ goto _LL1891;} _LL1891: _LL1948: _temp1947= _temp1873.f1; if((
unsigned int) _temp1947 > 4u?*(( int*) _temp1947) == Cyc_Absyn_IntType: 0){
_LL1952: _temp1951=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1947)->f1;
if( _temp1951 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1950;} else{ goto _LL1893;}
_LL1950: _temp1949=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1947)->f2;
if( _temp1949 ==( void*) Cyc_Absyn_B4){ goto _LL1946;} else{ goto _LL1893;}}
else{ goto _LL1893;} _LL1946: _temp1945= _temp1873.f2; goto _LL1892; _LL1893:
_LL1960: _temp1959= _temp1873.f1; goto _LL1954; _LL1954: _temp1953= _temp1873.f2;
if(( unsigned int) _temp1953 > 4u?*(( int*) _temp1953) == Cyc_Absyn_IntType: 0){
_LL1958: _temp1957=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1953)->f1;
if( _temp1957 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1956;} else{ goto _LL1895;}
_LL1956: _temp1955=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1953)->f2;
if( _temp1955 ==( void*) Cyc_Absyn_B4){ goto _LL1894;} else{ goto _LL1895;}}
else{ goto _LL1895;} _LL1895: goto _LL1896; _LL1876: goto _LL1878; _LL1878:
return( void*) Cyc_Absyn_DoubleType; _LL1880: goto _LL1882; _LL1882: return(
void*) Cyc_Absyn_FloatType; _LL1884: goto _LL1886; _LL1886: return Cyc_Absyn_ulong_t;
_LL1888: goto _LL1890; _LL1890: return Cyc_Absyn_slong_t; _LL1892: goto _LL1894;
_LL1894: return Cyc_Absyn_uint_t; _LL1896: return Cyc_Absyn_sint_t; _LL1874:;}
void Cyc_Tcutil_check_contains_assign( struct Cyc_Absyn_Exp* e){ void* _temp1961=(
void*) e->r; struct Cyc_Absyn_Exp* _temp1967; struct Cyc_Core_Opt* _temp1969;
struct Cyc_Absyn_Exp* _temp1971; _LL1963: if(*(( int*) _temp1961) == Cyc_Absyn_AssignOp_e){
_LL1972: _temp1971=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1961)->f1; goto _LL1970; _LL1970: _temp1969=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1961)->f2; if( _temp1969 == 0){ goto
_LL1968;} else{ goto _LL1965;} _LL1968: _temp1967=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1961)->f3; goto _LL1964;} else{ goto
_LL1965;} _LL1965: goto _LL1966; _LL1964: Cyc_Tcutil_warn( e->loc,( struct
_tagged_string)({ char* _temp1973=( char*)"assignment in test"; struct
_tagged_string _temp1974; _temp1974.curr= _temp1973; _temp1974.base= _temp1973;
_temp1974.last_plus_one= _temp1973 + 19; _temp1974;})); goto _LL1962; _LL1966:
goto _LL1962; _LL1962:;} static int Cyc_Tcutil_constrain_kinds( struct Cyc_Absyn_Conref*
c1, struct Cyc_Absyn_Conref* c2){ c1=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( c1); c2=(( struct Cyc_Absyn_Conref*(*)( struct
Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( c2); if( c1 == c2){ return 1;}
if(( void*) c1->v ==( void*) Cyc_Absyn_No_constr){( void*)( c1->v=( void*)((
void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp1975=( struct Cyc_Absyn_Forward_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp1975[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp1976; _temp1976.tag= Cyc_Absyn_Forward_constr;
_temp1976.f1= c2; _temp1976;}); _temp1975;}))); return 1;} else{ if(( void*) c2->v
==( void*) Cyc_Absyn_No_constr){( void*)( c2->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp1977=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp1977[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp1978; _temp1978.tag= Cyc_Absyn_Forward_constr; _temp1978.f1= c1; _temp1978;});
_temp1977;}))); return 1;} else{ void* k1=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( c1); void* k2=(( void*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_val)( c2); if( Cyc_Tcutil_kind_leq( k1, k2)){( void*)( c2->v=(
void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp1979=( struct Cyc_Absyn_Forward_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp1979[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp1980; _temp1980.tag= Cyc_Absyn_Forward_constr;
_temp1980.f1= c1; _temp1980;}); _temp1979;}))); return 1;} else{ if( Cyc_Tcutil_kind_leq(
k2, k1)){( void*)( c1->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp1981=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp1981[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp1982; _temp1982.tag= Cyc_Absyn_Forward_constr; _temp1982.f1= c2; _temp1982;});
_temp1981;}))); return 1;} else{ return 0;}}}}} static struct Cyc_List_List* Cyc_Tcutil_add_free_tvar(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar*
tv){ if( tvs == 0){ return({ struct Cyc_List_List* _temp1983=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1983->hd=( void*) tv; _temp1983->tl=
0; _temp1983;});} else{{ struct Cyc_List_List* tvs2= tvs; for( 0; tvs2 != 0;
tvs2=({ struct Cyc_List_List* _temp1984= tvs2; if( _temp1984 == 0){ _throw(
Null_Exception);} _temp1984->tl;})){ if( Cyc_String_zstrptrcmp((( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp1985= tvs2; if( _temp1985 == 0){ _throw(
Null_Exception);} _temp1985->hd;}))->name, tv->name) == 0){ struct Cyc_Absyn_Conref*
k1=(( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp1989= tvs2; if(
_temp1989 == 0){ _throw( Null_Exception);} _temp1989->hd;}))->kind; struct Cyc_Absyn_Conref*
k2= tv->kind; if( ! Cyc_Tcutil_constrain_kinds( k1, k2)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp1986=* tv->name; struct _tagged_string _temp1987= Cyc_Absynpp_ckind2string(
k1); struct _tagged_string _temp1988= Cyc_Absynpp_ckind2string( k2); xprintf("type variable %.*s is used with inconsistent kinds %.*s and %.*s",
_temp1986.last_plus_one - _temp1986.curr, _temp1986.curr, _temp1987.last_plus_one
- _temp1987.curr, _temp1987.curr, _temp1988.last_plus_one - _temp1988.curr,
_temp1988.curr);}));} return tvs;} else{ if(({ struct Cyc_List_List* _temp1990=
tvs2; if( _temp1990 == 0){ _throw( Null_Exception);} _temp1990->tl;}) == 0){({
struct Cyc_List_List* _temp1991= tvs2; if( _temp1991 == 0){ _throw(
Null_Exception);} _temp1991->tl=({ struct Cyc_List_List* _temp1992=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1992->hd=( void*) tv; _temp1992->tl=
0; _temp1992;});}); return tvs;}}}} return(( struct Cyc_List_List*(*)( struct
_tagged_string msg)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp1993=( char*)"Tcutil::add_free_tvar:expecting to have returned."; struct
_tagged_string _temp1994; _temp1994.curr= _temp1993; _temp1994.base= _temp1993;
_temp1994.last_plus_one= _temp1993 + 50; _temp1994;}));}} static struct Cyc_List_List*
Cyc_Tcutil_remove_bound_tvars( struct Cyc_List_List* tvs, struct Cyc_List_List*
btvs){ struct Cyc_List_List* r= 0; for( 0; tvs != 0; tvs=({ struct Cyc_List_List*
_temp1995= tvs; if( _temp1995 == 0){ _throw( Null_Exception);} _temp1995->tl;})){
int present= 0;{ struct Cyc_List_List* b= btvs; for( 0; b != 0; b=({ struct Cyc_List_List*
_temp1996= b; if( _temp1996 == 0){ _throw( Null_Exception);} _temp1996->tl;})){
if( Cyc_String_zstrptrcmp((( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp1997= tvs; if( _temp1997 == 0){ _throw( Null_Exception);} _temp1997->hd;}))->name,((
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp1998= b; if( _temp1998 == 0){
_throw( Null_Exception);} _temp1998->hd;}))->name) == 0){ present= 1; break;}}}
if( ! present){ r=({ struct Cyc_List_List* _temp1999=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1999->hd=( void*)(( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp2000= tvs; if( _temp2000 == 0){ _throw(
Null_Exception);} _temp2000->hd;})); _temp1999->tl= r; _temp1999;});}} r=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( r);
return r;} static int Cyc_Tcutil_tvar_counter= 0; struct Cyc_Absyn_Tvar* Cyc_Tcutil_new_tvar(
void* k){ struct _tagged_string s= xprintf("#%d",( Cyc_Tcutil_tvar_counter ++));
return({ struct Cyc_Absyn_Tvar* _temp2001=( struct Cyc_Absyn_Tvar*) GC_malloc(
sizeof( struct Cyc_Absyn_Tvar)); _temp2001->name=({ struct _tagged_string*
_temp2002=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp2002[ 0]= s; _temp2002;}); _temp2001->kind=(( struct Cyc_Absyn_Conref*(*)(
void* x)) Cyc_Absyn_new_conref)( k); _temp2001;});} int Cyc_Tcutil_is_temp_tvar(
struct Cyc_Absyn_Tvar* t){ struct _tagged_string _temp2003=* t->name; goto
_LL2004; _LL2004: return({ struct _tagged_string _temp2005= _temp2003; char*
_temp2007= _temp2005.curr + 0; if( _temp2005.base == 0? 1:( _temp2007 <
_temp2005.base? 1: _temp2007 >= _temp2005.last_plus_one)){ _throw(
Null_Exception);}* _temp2007;}) =='#';} void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar* t){ if( ! Cyc_Tcutil_is_temp_tvar( t)){ return;}{ struct
_tagged_string _temp2010= Cyc_String_strconcat(( struct _tagged_string)({ char*
_temp2008=( char*)"`"; struct _tagged_string _temp2009; _temp2009.curr=
_temp2008; _temp2009.base= _temp2008; _temp2009.last_plus_one= _temp2008 + 2;
_temp2009;}),* t->name); goto _LL2011; _LL2011:({ struct _tagged_string
_temp2012= _temp2010; char* _temp2014= _temp2012.curr + 1; if( _temp2012.base ==
0? 1:( _temp2014 < _temp2012.base? 1: _temp2014 >= _temp2012.last_plus_one)){
_throw( Null_Exception);}* _temp2014='t';}); t->name=({ struct _tagged_string*
_temp2015=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp2015[ 0]= _temp2010; _temp2015;});}} struct _tuple11{ struct
_tagged_string* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; static struct
_tuple2* Cyc_Tcutil_fndecl2typ_f( struct _tuple11* x){ return({ struct _tuple2*
_temp2016=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2)); _temp2016->f1=(
struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt* _temp2017=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2017->v=( void*)(* x).f1;
_temp2017;}); _temp2016->f2=(* x).f2; _temp2016->f3=(* x).f3; _temp2016;});} int
Cyc_Tcutil_fntype_att( void* a){ void* _temp2018= a; int _temp2032; _LL2020: if((
unsigned int) _temp2018 > 15u?*(( int*) _temp2018) == Cyc_Absyn_Regparm_att: 0){
_LL2033: _temp2032=( int)(( struct Cyc_Absyn_Regparm_att_struct*) _temp2018)->f1;
goto _LL2021;} else{ goto _LL2022;} _LL2022: if( _temp2018 ==( void*) Cyc_Absyn_Stdcall_att){
goto _LL2023;} else{ goto _LL2024;} _LL2024: if( _temp2018 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL2025;} else{ goto _LL2026;} _LL2026: if( _temp2018 ==( void*) Cyc_Absyn_Noreturn_att){
goto _LL2027;} else{ goto _LL2028;} _LL2028: if( _temp2018 ==( void*) Cyc_Absyn_Const_att){
goto _LL2029;} else{ goto _LL2030;} _LL2030: goto _LL2031; _LL2021: goto _LL2023;
_LL2023: goto _LL2025; _LL2025: goto _LL2027; _LL2027: goto _LL2029; _LL2029:
return 1; _LL2031: return 0; _LL2019:;} void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl*
fd){ if( fd->cached_typ == 0){ struct Cyc_List_List* _temp2034= 0; goto _LL2035;
_LL2035:{ struct Cyc_List_List* atts= fd->attributes; for( 0; atts != 0; atts=({
struct Cyc_List_List* _temp2036= atts; if( _temp2036 == 0){ _throw(
Null_Exception);} _temp2036->tl;})){ if( Cyc_Tcutil_fntype_att(( void*)({ struct
Cyc_List_List* _temp2037= atts; if( _temp2037 == 0){ _throw( Null_Exception);}
_temp2037->hd;}))){ _temp2034=({ struct Cyc_List_List* _temp2038=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2038->hd=( void*)(( void*)({
struct Cyc_List_List* _temp2039= atts; if( _temp2039 == 0){ _throw(
Null_Exception);} _temp2039->hd;})); _temp2038->tl= _temp2034; _temp2038;});}}}
return( void*)({ struct Cyc_Absyn_FnType_struct* _temp2040=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp2040[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp2041; _temp2041.tag= Cyc_Absyn_FnType; _temp2041.f1=({ struct Cyc_Absyn_FnInfo
_temp2042; _temp2042.tvars= fd->tvs; _temp2042.effect= fd->effect; _temp2042.ret_typ=(
void*)(( void*) fd->ret_type); _temp2042.args=(( struct Cyc_List_List*(*)(
struct _tuple2*(* f)( struct _tuple11*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcutil_fndecl2typ_f, fd->args); _temp2042.varargs= fd->varargs; _temp2042.attributes=
_temp2034; _temp2042;}); _temp2041;}); _temp2040;});} return( void*)({ struct
Cyc_Core_Opt* _temp2043= fd->cached_typ; if( _temp2043 == 0){ _throw(
Null_Exception);} _temp2043->v;});} struct _tuple12{ void* f1; struct Cyc_Absyn_Tqual
f2; void* f3; } ; static void* Cyc_Tcutil_fst_fdarg( struct _tuple12* t){ return(*
t).f1;} void* Cyc_Tcutil_snd_tqt( struct _tuple4* t){ return(* t).f2;} static
struct _tuple4* Cyc_Tcutil_map2_tq( struct _tuple4* pr, void* t){ return({
struct _tuple4* _temp2044=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp2044->f1=(* pr).f1; _temp2044->f2= t; _temp2044;});} void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Fndecl*
fd){ void* t= Cyc_Tcutil_fndecl2typ( fd); Cyc_Tcutil_check_valid_toplevel_type(
loc, te, t);{ void* _temp2045= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo
_temp2051; struct Cyc_List_List* _temp2053; int _temp2055; struct Cyc_List_List*
_temp2057; void* _temp2059; struct Cyc_Core_Opt* _temp2061; struct Cyc_List_List*
_temp2063; _LL2047: if(( unsigned int) _temp2045 > 4u?*(( int*) _temp2045) ==
Cyc_Absyn_FnType: 0){ _LL2052: _temp2051=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp2045)->f1; _LL2064: _temp2063=( struct Cyc_List_List*) _temp2051.tvars;
goto _LL2062; _LL2062: _temp2061=( struct Cyc_Core_Opt*) _temp2051.effect; goto
_LL2060; _LL2060: _temp2059=( void*) _temp2051.ret_typ; goto _LL2058; _LL2058:
_temp2057=( struct Cyc_List_List*) _temp2051.args; goto _LL2056; _LL2056:
_temp2055=( int) _temp2051.varargs; goto _LL2054; _LL2054: _temp2053=( struct
Cyc_List_List*) _temp2051.attributes; goto _LL2048;} else{ goto _LL2049;}
_LL2049: goto _LL2050; _LL2048: fd->tvs= _temp2063; fd->effect= _temp2061; goto
_LL2046; _LL2050:(( void(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp2065=( char*)"check_fndecl_valid_type: not a FnType";
struct _tagged_string _temp2066; _temp2066.curr= _temp2065; _temp2066.base=
_temp2065; _temp2066.last_plus_one= _temp2065 + 38; _temp2066;})); return;
_LL2046:;}{ struct _RegionHandle _temp2067= _new_region(); struct _RegionHandle*
r=& _temp2067; _push_region( r); Cyc_Tcutil_check_unique_vars((( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tagged_string*(* f)( struct _tuple11*), struct
Cyc_List_List* x)) Cyc_List_rmap)( r,( struct _tagged_string*(*)( struct
_tuple11* t)) Cyc_Tcutil_fst_fdarg, fd->args), loc,( struct _tagged_string)({
char* _temp2068=( char*)"function declaration has repeated parameter"; struct
_tagged_string _temp2069; _temp2069.curr= _temp2068; _temp2069.base= _temp2068;
_temp2069.last_plus_one= _temp2068 + 44; _temp2069;}));; _pop_region(& _temp2067);}
fd->cached_typ=({ struct Cyc_Core_Opt* _temp2070=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2070->v=( void*) t; _temp2070;});}
struct _tuple13{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; } ; struct
_tuple14{ struct _tuple13* f1; void* f2; } ; static struct _tuple14* Cyc_Tcutil_substitute_f1(
struct _RegionHandle* rgn, struct _tuple2* y){ return({ struct _tuple14*
_temp2071=( struct _tuple14*) _region_malloc( rgn, sizeof( struct _tuple14));
_temp2071->f1=({ struct _tuple13* _temp2072=( struct _tuple13*) _region_malloc(
rgn, sizeof( struct _tuple13)); _temp2072->f1=(* y).f1; _temp2072->f2=(* y).f2;
_temp2072;}); _temp2071->f2=(* y).f3; _temp2071;});} static struct _tuple2* Cyc_Tcutil_substitute_f2(
struct _tuple14* w){ void* _temp2075; struct _tuple13* _temp2077; struct
_tuple14 _temp2073=* w; _LL2078: _temp2077= _temp2073.f1; goto _LL2076; _LL2076:
_temp2075= _temp2073.f2; goto _LL2074; _LL2074: { struct Cyc_Absyn_Tqual
_temp2081; struct Cyc_Core_Opt* _temp2083; struct _tuple13 _temp2079=* _temp2077;
_LL2084: _temp2083= _temp2079.f1; goto _LL2082; _LL2082: _temp2081= _temp2079.f2;
goto _LL2080; _LL2080: return({ struct _tuple2* _temp2085=( struct _tuple2*)
GC_malloc( sizeof( struct _tuple2)); _temp2085->f1= _temp2083; _temp2085->f2=
_temp2081; _temp2085->f3= _temp2075; _temp2085;});}} static void* Cyc_Tcutil_field_type(
struct Cyc_Absyn_Structfield* f){ return( void*) f->type;} static struct Cyc_Absyn_Structfield*
Cyc_Tcutil_zip_field_type( struct Cyc_Absyn_Structfield* f, void* t){ return({
struct Cyc_Absyn_Structfield* _temp2086=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp2086->name= f->name;
_temp2086->tq= f->tq; _temp2086->type=( void*) t; _temp2086->width= f->width;
_temp2086->attributes= f->attributes; _temp2086;});} static struct Cyc_List_List*
Cyc_Tcutil_substs( struct _RegionHandle* rgn, struct Cyc_List_List* inst, struct
Cyc_List_List* ts); void* Cyc_Tcutil_rsubstitute( struct _RegionHandle* rgn,
struct Cyc_List_List* inst, void* t){ void* _temp2087= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Tvar* _temp2133; struct Cyc_Absyn_Structdecl** _temp2135;
struct Cyc_List_List* _temp2137; struct _tuple1* _temp2139; struct Cyc_Absyn_Uniondecl**
_temp2141; struct Cyc_List_List* _temp2143; struct _tuple1* _temp2145; struct
Cyc_Absyn_TunionInfo _temp2147; void* _temp2149; struct Cyc_List_List* _temp2151;
void* _temp2153; struct Cyc_Absyn_TunionFieldInfo _temp2155; struct Cyc_List_List*
_temp2157; void* _temp2159; struct Cyc_Core_Opt* _temp2161; struct Cyc_List_List*
_temp2163; struct _tuple1* _temp2165; struct Cyc_Absyn_Exp* _temp2167; struct
Cyc_Absyn_Tqual _temp2169; void* _temp2171; struct Cyc_Absyn_PtrInfo _temp2173;
struct Cyc_Absyn_Conref* _temp2175; struct Cyc_Absyn_Tqual _temp2177; struct Cyc_Absyn_Conref*
_temp2179; void* _temp2181; void* _temp2183; struct Cyc_Absyn_FnInfo _temp2185;
struct Cyc_List_List* _temp2187; int _temp2189; struct Cyc_List_List* _temp2191;
void* _temp2193; struct Cyc_Core_Opt* _temp2195; struct Cyc_List_List* _temp2197;
struct Cyc_List_List* _temp2199; struct Cyc_List_List* _temp2201; struct Cyc_List_List*
_temp2203; int _temp2205; struct Cyc_Core_Opt* _temp2207; void* _temp2209; void*
_temp2211; struct Cyc_Absyn_Enumdecl* _temp2213; struct _tuple1* _temp2215; void*
_temp2217; void* _temp2219; void* _temp2221; struct Cyc_List_List* _temp2223;
_LL2089: if(( unsigned int) _temp2087 > 4u?*(( int*) _temp2087) == Cyc_Absyn_VarType:
0){ _LL2134: _temp2133=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp2087)->f1; goto _LL2090;} else{ goto _LL2091;} _LL2091: if(( unsigned int)
_temp2087 > 4u?*(( int*) _temp2087) == Cyc_Absyn_StructType: 0){ _LL2140:
_temp2139=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp2087)->f1;
goto _LL2138; _LL2138: _temp2137=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp2087)->f2; goto _LL2136; _LL2136: _temp2135=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp2087)->f3; goto _LL2092;} else{ goto
_LL2093;} _LL2093: if(( unsigned int) _temp2087 > 4u?*(( int*) _temp2087) == Cyc_Absyn_UnionType:
0){ _LL2146: _temp2145=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp2087)->f1; goto _LL2144; _LL2144: _temp2143=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnionType_struct*) _temp2087)->f2; goto _LL2142; _LL2142:
_temp2141=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp2087)->f3; goto _LL2094;} else{ goto _LL2095;} _LL2095: if(( unsigned int)
_temp2087 > 4u?*(( int*) _temp2087) == Cyc_Absyn_TunionType: 0){ _LL2148:
_temp2147=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp2087)->f1; _LL2154: _temp2153=( void*) _temp2147.tunion_info; goto _LL2152;
_LL2152: _temp2151=( struct Cyc_List_List*) _temp2147.targs; goto _LL2150;
_LL2150: _temp2149=( void*) _temp2147.rgn; goto _LL2096;} else{ goto _LL2097;}
_LL2097: if(( unsigned int) _temp2087 > 4u?*(( int*) _temp2087) == Cyc_Absyn_TunionFieldType:
0){ _LL2156: _temp2155=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp2087)->f1; _LL2160: _temp2159=( void*) _temp2155.field_info; goto _LL2158;
_LL2158: _temp2157=( struct Cyc_List_List*) _temp2155.targs; goto _LL2098;}
else{ goto _LL2099;} _LL2099: if(( unsigned int) _temp2087 > 4u?*(( int*)
_temp2087) == Cyc_Absyn_TypedefType: 0){ _LL2166: _temp2165=( struct _tuple1*)((
struct Cyc_Absyn_TypedefType_struct*) _temp2087)->f1; goto _LL2164; _LL2164:
_temp2163=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp2087)->f2; goto _LL2162; _LL2162: _temp2161=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_TypedefType_struct*) _temp2087)->f3; goto _LL2100;} else{ goto
_LL2101;} _LL2101: if(( unsigned int) _temp2087 > 4u?*(( int*) _temp2087) == Cyc_Absyn_ArrayType:
0){ _LL2172: _temp2171=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2087)->f1;
goto _LL2170; _LL2170: _temp2169=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2087)->f2; goto _LL2168; _LL2168: _temp2167=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2087)->f3; goto _LL2102;} else{ goto
_LL2103;} _LL2103: if(( unsigned int) _temp2087 > 4u?*(( int*) _temp2087) == Cyc_Absyn_PointerType:
0){ _LL2174: _temp2173=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp2087)->f1; _LL2184: _temp2183=( void*) _temp2173.elt_typ; goto _LL2182;
_LL2182: _temp2181=( void*) _temp2173.rgn_typ; goto _LL2180; _LL2180: _temp2179=(
struct Cyc_Absyn_Conref*) _temp2173.nullable; goto _LL2178; _LL2178: _temp2177=(
struct Cyc_Absyn_Tqual) _temp2173.tq; goto _LL2176; _LL2176: _temp2175=( struct
Cyc_Absyn_Conref*) _temp2173.bounds; goto _LL2104;} else{ goto _LL2105;} _LL2105:
if(( unsigned int) _temp2087 > 4u?*(( int*) _temp2087) == Cyc_Absyn_FnType: 0){
_LL2186: _temp2185=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp2087)->f1; _LL2198: _temp2197=( struct Cyc_List_List*) _temp2185.tvars;
goto _LL2196; _LL2196: _temp2195=( struct Cyc_Core_Opt*) _temp2185.effect; goto
_LL2194; _LL2194: _temp2193=( void*) _temp2185.ret_typ; goto _LL2192; _LL2192:
_temp2191=( struct Cyc_List_List*) _temp2185.args; goto _LL2190; _LL2190:
_temp2189=( int) _temp2185.varargs; goto _LL2188; _LL2188: _temp2187=( struct
Cyc_List_List*) _temp2185.attributes; goto _LL2106;} else{ goto _LL2107;}
_LL2107: if(( unsigned int) _temp2087 > 4u?*(( int*) _temp2087) == Cyc_Absyn_TupleType:
0){ _LL2200: _temp2199=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp2087)->f1; goto _LL2108;} else{ goto _LL2109;} _LL2109: if(( unsigned int)
_temp2087 > 4u?*(( int*) _temp2087) == Cyc_Absyn_AnonStructType: 0){ _LL2202:
_temp2201=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp2087)->f1; goto _LL2110;} else{ goto _LL2111;} _LL2111: if(( unsigned int)
_temp2087 > 4u?*(( int*) _temp2087) == Cyc_Absyn_AnonUnionType: 0){ _LL2204:
_temp2203=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp2087)->f1; goto _LL2112;} else{ goto _LL2113;} _LL2113: if(( unsigned int)
_temp2087 > 4u?*(( int*) _temp2087) == Cyc_Absyn_Evar: 0){ _LL2210: _temp2209=(
void*)(( struct Cyc_Absyn_Evar_struct*) _temp2087)->f1; goto _LL2208; _LL2208:
_temp2207=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp2087)->f2;
goto _LL2206; _LL2206: _temp2205=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp2087)->f3; goto _LL2114;} else{ goto _LL2115;} _LL2115: if(( unsigned int)
_temp2087 > 4u?*(( int*) _temp2087) == Cyc_Absyn_RgnHandleType: 0){ _LL2212:
_temp2211=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp2087)->f1;
goto _LL2116;} else{ goto _LL2117;} _LL2117: if(( unsigned int) _temp2087 > 4u?*((
int*) _temp2087) == Cyc_Absyn_EnumType: 0){ _LL2216: _temp2215=( struct _tuple1*)((
struct Cyc_Absyn_EnumType_struct*) _temp2087)->f1; goto _LL2214; _LL2214:
_temp2213=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*)
_temp2087)->f2; goto _LL2118;} else{ goto _LL2119;} _LL2119: if( _temp2087 ==(
void*) Cyc_Absyn_VoidType){ goto _LL2120;} else{ goto _LL2121;} _LL2121: if((
unsigned int) _temp2087 > 4u?*(( int*) _temp2087) == Cyc_Absyn_IntType: 0){
_LL2220: _temp2219=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2087)->f1;
goto _LL2218; _LL2218: _temp2217=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2087)->f2; goto _LL2122;} else{ goto _LL2123;} _LL2123: if( _temp2087 ==(
void*) Cyc_Absyn_FloatType){ goto _LL2124;} else{ goto _LL2125;} _LL2125: if(
_temp2087 ==( void*) Cyc_Absyn_DoubleType){ goto _LL2126;} else{ goto _LL2127;}
_LL2127: if( _temp2087 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL2128;} else{ goto
_LL2129;} _LL2129: if(( unsigned int) _temp2087 > 4u?*(( int*) _temp2087) == Cyc_Absyn_AccessEff:
0){ _LL2222: _temp2221=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp2087)->f1;
goto _LL2130;} else{ goto _LL2131;} _LL2131: if(( unsigned int) _temp2087 > 4u?*((
int*) _temp2087) == Cyc_Absyn_JoinEff: 0){ _LL2224: _temp2223=( struct Cyc_List_List*)((
struct Cyc_Absyn_JoinEff_struct*) _temp2087)->f1; goto _LL2132;} else{ goto
_LL2088;} _LL2090: { struct _handler_cons _temp2225; _push_handler(& _temp2225);{
int _temp2227= 0; if( setjmp( _temp2225.handler)){ _temp2227= 1;} if( !
_temp2227){{ void* _temp2228=(( void*(*)( int(* compare)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_assoc_cmp)(
Cyc_Absyn_tvar_cmp, inst, _temp2133); _npop_handler( 0u); return _temp2228;};
_pop_handler();} else{ void* _temp2226=( void*) _exn_thrown; void* _temp2230=
_temp2226; _LL2232: if( _temp2230 == Cyc_Core_Not_found){ goto _LL2233;} else{
goto _LL2234;} _LL2234: goto _LL2235; _LL2233: return t; _LL2235:( void) _throw(
_temp2230); _LL2231:;}}} _LL2092: { struct Cyc_List_List* _temp2236= Cyc_Tcutil_substs(
rgn, inst, _temp2137); goto _LL2237; _LL2237: return _temp2236 == _temp2137? t:(
void*)({ struct Cyc_Absyn_StructType_struct* _temp2238=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp2238[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp2239; _temp2239.tag= Cyc_Absyn_StructType;
_temp2239.f1= _temp2139; _temp2239.f2= _temp2236; _temp2239.f3= _temp2135;
_temp2239;}); _temp2238;});} _LL2094: { struct Cyc_List_List* _temp2240= Cyc_Tcutil_substs(
rgn, inst, _temp2143); goto _LL2241; _LL2241: return _temp2240 == _temp2143? t:(
void*)({ struct Cyc_Absyn_UnionType_struct* _temp2242=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp2242[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp2243; _temp2243.tag= Cyc_Absyn_UnionType;
_temp2243.f1= _temp2145; _temp2243.f2= _temp2240; _temp2243.f3= _temp2141;
_temp2243;}); _temp2242;});} _LL2096: { struct Cyc_List_List* _temp2244= Cyc_Tcutil_substs(
rgn, inst, _temp2151); goto _LL2245; _LL2245: { void* _temp2246= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2149); goto _LL2247; _LL2247: return( _temp2244 == _temp2151?
_temp2246 == _temp2149: 0)? t:( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp2248=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp2248[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp2249; _temp2249.tag=
Cyc_Absyn_TunionType; _temp2249.f1=({ struct Cyc_Absyn_TunionInfo _temp2250;
_temp2250.tunion_info=( void*) _temp2153; _temp2250.targs= _temp2244; _temp2250.rgn=(
void*) _temp2246; _temp2250;}); _temp2249;}); _temp2248;});}} _LL2098: { struct
Cyc_List_List* _temp2251= Cyc_Tcutil_substs( rgn, inst, _temp2157); goto _LL2252;
_LL2252: return _temp2251 == _temp2157? t:( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp2253=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp2253[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp2254; _temp2254.tag= Cyc_Absyn_TunionFieldType; _temp2254.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp2255; _temp2255.field_info=( void*) _temp2159; _temp2255.targs= _temp2251;
_temp2255;}); _temp2254;}); _temp2253;});} _LL2100: { struct Cyc_List_List*
_temp2256= Cyc_Tcutil_substs( rgn, inst, _temp2163); goto _LL2257; _LL2257:
return _temp2256 == _temp2163? t:( void*)({ struct Cyc_Absyn_TypedefType_struct*
_temp2258=( struct Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp2258[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp2259; _temp2259.tag=
Cyc_Absyn_TypedefType; _temp2259.f1= _temp2165; _temp2259.f2= _temp2256;
_temp2259.f3= _temp2161; _temp2259;}); _temp2258;});} _LL2102: { void* _temp2260=
Cyc_Tcutil_rsubstitute( rgn, inst, _temp2171); goto _LL2261; _LL2261: return
_temp2260 == _temp2171? t:( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp2262=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp2262[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp2263; _temp2263.tag= Cyc_Absyn_ArrayType;
_temp2263.f1=( void*) _temp2260; _temp2263.f2= _temp2169; _temp2263.f3=
_temp2167; _temp2263;}); _temp2262;});} _LL2104: { void* _temp2264= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2183); goto _LL2265; _LL2265: { void* _temp2266= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2181); goto _LL2267; _LL2267: if( _temp2264 == _temp2183?
_temp2266 == _temp2181: 0){ return t;} return( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp2268=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp2268[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp2269; _temp2269.tag=
Cyc_Absyn_PointerType; _temp2269.f1=({ struct Cyc_Absyn_PtrInfo _temp2270;
_temp2270.elt_typ=( void*) _temp2264; _temp2270.rgn_typ=( void*) _temp2266;
_temp2270.nullable= _temp2179; _temp2270.tq= _temp2177; _temp2270.bounds=
_temp2175; _temp2270;}); _temp2269;}); _temp2268;});}} _LL2106:{ struct Cyc_List_List*
_temp2271= _temp2197; goto _LL2272; _LL2272: for( 0; _temp2271 != 0; _temp2271=({
struct Cyc_List_List* _temp2273= _temp2271; if( _temp2273 == 0){ _throw(
Null_Exception);} _temp2273->tl;})){ inst=({ struct Cyc_List_List* _temp2274=(
struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp2274->hd=( void*)({ struct _tuple5* _temp2275=( struct _tuple5*)
_region_malloc( rgn, sizeof( struct _tuple5)); _temp2275->f1=( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp2279= _temp2271; if( _temp2279 == 0){ _throw(
Null_Exception);} _temp2279->hd;}); _temp2275->f2=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp2276=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2276[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2277; _temp2277.tag= Cyc_Absyn_VarType;
_temp2277.f1=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp2278=
_temp2271; if( _temp2278 == 0){ _throw( Null_Exception);} _temp2278->hd;});
_temp2277;}); _temp2276;}); _temp2275;}); _temp2274->tl= inst; _temp2274;});}}{
struct Cyc_List_List* _temp2282; struct Cyc_List_List* _temp2284; struct _tuple0
_temp2280=(( struct _tuple0(*)( struct _RegionHandle* r3, struct _RegionHandle*
r4, struct Cyc_List_List* x)) Cyc_List_rsplit)( rgn, rgn,(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple14*(* f)( struct _RegionHandle*, struct
_tuple2*), struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)(
rgn, Cyc_Tcutil_substitute_f1, rgn, _temp2191)); _LL2285: _temp2284= _temp2280.f1;
goto _LL2283; _LL2283: _temp2282= _temp2280.f2; goto _LL2281; _LL2281: { struct
Cyc_List_List* _temp2286= Cyc_Tcutil_substs( rgn, inst, _temp2282); goto _LL2287;
_LL2287: { struct Cyc_List_List* _temp2288=(( struct Cyc_List_List*(*)( struct
_tuple2*(* f)( struct _tuple14*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_substitute_f2,((
struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct _RegionHandle* r4,
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn,
_temp2284, _temp2286)); goto _LL2289; _LL2289: { struct Cyc_Core_Opt* eff2; if(
_temp2195 == 0){ eff2= 0;} else{ void* _temp2291= Cyc_Tcutil_rsubstitute( rgn,
inst,( void*)({ struct Cyc_Core_Opt* _temp2290= _temp2195; if( _temp2290 == 0){
_throw( Null_Exception);} _temp2290->v;})); goto _LL2292; _LL2292: if( _temp2291
==( void*)({ struct Cyc_Core_Opt* _temp2293= _temp2195; if( _temp2293 == 0){
_throw( Null_Exception);} _temp2293->v;})){ eff2= _temp2195;} else{ eff2=({
struct Cyc_Core_Opt* _temp2294=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2294->v=( void*) _temp2291; _temp2294;});}} return( void*)({
struct Cyc_Absyn_FnType_struct* _temp2295=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp2295[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp2296; _temp2296.tag= Cyc_Absyn_FnType; _temp2296.f1=({ struct Cyc_Absyn_FnInfo
_temp2297; _temp2297.tvars= _temp2197; _temp2297.effect= eff2; _temp2297.ret_typ=(
void*) Cyc_Tcutil_rsubstitute( rgn, inst, _temp2193); _temp2297.args= _temp2288;
_temp2297.varargs= _temp2189; _temp2297.attributes= _temp2187; _temp2297;});
_temp2296;}); _temp2295;});}}}} _LL2108: { struct Cyc_List_List* _temp2298=((
struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct _tuple4*),
struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_snd_tqt, _temp2199);
goto _LL2299; _LL2299: { struct Cyc_List_List* _temp2300= Cyc_Tcutil_substs( rgn,
inst, _temp2298); goto _LL2301; _LL2301: if( _temp2300 == _temp2298){ return t;}{
struct Cyc_List_List* _temp2302=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)(
struct _tuple4*, void*), struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_map2)(
Cyc_Tcutil_map2_tq, _temp2199, _temp2300); goto _LL2303; _LL2303: return( void*)({
struct Cyc_Absyn_TupleType_struct* _temp2304=( struct Cyc_Absyn_TupleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp2304[ 0]=({ struct
Cyc_Absyn_TupleType_struct _temp2305; _temp2305.tag= Cyc_Absyn_TupleType;
_temp2305.f1= _temp2302; _temp2305;}); _temp2304;});}}} _LL2110: { struct Cyc_List_List*
_temp2306=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type,
_temp2201); goto _LL2307; _LL2307: { struct Cyc_List_List* _temp2308= Cyc_Tcutil_substs(
rgn, inst, _temp2306); goto _LL2309; _LL2309: if( _temp2308 == _temp2306){
return t;}{ struct Cyc_List_List* _temp2310=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Structfield*(* f)( struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type,
_temp2201, _temp2308); goto _LL2311; _LL2311: return( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp2312=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp2312[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp2313; _temp2313.tag= Cyc_Absyn_AnonStructType; _temp2313.f1= _temp2310;
_temp2313;}); _temp2312;});}}} _LL2112: { struct Cyc_List_List* _temp2314=((
struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct Cyc_Absyn_Structfield*),
struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type, _temp2203);
goto _LL2315; _LL2315: { struct Cyc_List_List* _temp2316= Cyc_Tcutil_substs( rgn,
inst, _temp2314); goto _LL2317; _LL2317: if( _temp2316 == _temp2314){ return t;}{
struct Cyc_List_List* _temp2318=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type, _temp2203, _temp2316); goto
_LL2319; _LL2319: return( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp2320=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp2320[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp2321; _temp2321.tag= Cyc_Absyn_AnonStructType; _temp2321.f1= _temp2318;
_temp2321;}); _temp2320;});}}} _LL2114: if( _temp2207 != 0){ return Cyc_Tcutil_rsubstitute(
rgn, inst,( void*)({ struct Cyc_Core_Opt* _temp2322= _temp2207; if( _temp2322 ==
0){ _throw( Null_Exception);} _temp2322->v;}));} else{ return t;} _LL2116: {
void* _temp2323= Cyc_Tcutil_rsubstitute( rgn, inst, _temp2211); goto _LL2324;
_LL2324: return _temp2323 == _temp2211? t:( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp2325=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp2325[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp2326; _temp2326.tag= Cyc_Absyn_RgnHandleType; _temp2326.f1=( void*)
_temp2323; _temp2326;}); _temp2325;});} _LL2118: return t; _LL2120: return t;
_LL2122: return t; _LL2124: return t; _LL2126: return t; _LL2128: return t;
_LL2130: { void* _temp2327= Cyc_Tcutil_rsubstitute( rgn, inst, _temp2221); goto
_LL2328; _LL2328: return _temp2327 == _temp2221? t:( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp2329=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2329[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2330; _temp2330.tag= Cyc_Absyn_AccessEff;
_temp2330.f1=( void*) _temp2327; _temp2330;}); _temp2329;});} _LL2132: { struct
Cyc_List_List* _temp2331= Cyc_Tcutil_substs( rgn, inst, _temp2223); goto _LL2332;
_LL2332: return _temp2331 == _temp2223? t:( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp2333=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2333[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2334; _temp2334.tag= Cyc_Absyn_JoinEff;
_temp2334.f1= _temp2331; _temp2334;}); _temp2333;});} _LL2088:;} static struct
Cyc_List_List* Cyc_Tcutil_substs( struct _RegionHandle* rgn, struct Cyc_List_List*
inst, struct Cyc_List_List* ts){ if( ts == 0){ return 0;}{ void* _temp2336=(
void*)({ struct Cyc_List_List* _temp2335= ts; if( _temp2335 == 0){ _throw(
Null_Exception);} _temp2335->hd;}); goto _LL2337; _LL2337: { struct Cyc_List_List*
_temp2339=({ struct Cyc_List_List* _temp2338= ts; if( _temp2338 == 0){ _throw(
Null_Exception);} _temp2338->tl;}); goto _LL2340; _LL2340: { void* _temp2341=
Cyc_Tcutil_rsubstitute( rgn, inst, _temp2336); goto _LL2342; _LL2342: { struct
Cyc_List_List* _temp2343= Cyc_Tcutil_substs( rgn, inst, _temp2339); goto _LL2344;
_LL2344: if( _temp2336 == _temp2341? _temp2339 == _temp2343: 0){ return ts;}
return( struct Cyc_List_List*)({ struct Cyc_List_List* _temp2345=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2345->hd=( void*) _temp2341;
_temp2345->tl= _temp2343; _temp2345;});}}}}} extern void* Cyc_Tcutil_substitute(
struct Cyc_List_List* inst, void* t){ return Cyc_Tcutil_rsubstitute( Cyc_Core_heap_region,
inst, t);} struct Cyc_Absyn_Exp* Cyc_Tcutil_default_initializer( struct Cyc_Tcenv_Tenv*
te, void* t, struct Cyc_Position_Segment* loc){ struct Cyc_Absyn_Exp* e= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp2395=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2395[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2396; _temp2396.tag= Cyc_Absyn_Const_e; _temp2396.f1=(
void*)(( void*) Cyc_Absyn_Null_c); _temp2396;}); _temp2395;}), loc);{ void*
_temp2346= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp2360; void*
_temp2362; void* _temp2364; void* _temp2366; void* _temp2368; _LL2348: if((
unsigned int) _temp2346 > 4u?*(( int*) _temp2346) == Cyc_Absyn_PointerType: 0){
_LL2361: _temp2360=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp2346)->f1; goto _LL2349;} else{ goto _LL2350;} _LL2350: if(( unsigned int)
_temp2346 > 4u?*(( int*) _temp2346) == Cyc_Absyn_IntType: 0){ _LL2365: _temp2364=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp2346)->f1; goto _LL2363; _LL2363:
_temp2362=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2346)->f2; if(
_temp2362 ==( void*) Cyc_Absyn_B1){ goto _LL2351;} else{ goto _LL2352;}} else{
goto _LL2352;} _LL2352: if(( unsigned int) _temp2346 > 4u?*(( int*) _temp2346)
== Cyc_Absyn_IntType: 0){ _LL2369: _temp2368=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2346)->f1; goto _LL2367; _LL2367: _temp2366=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2346)->f2; goto _LL2353;} else{ goto _LL2354;} _LL2354: if( _temp2346 ==(
void*) Cyc_Absyn_FloatType){ goto _LL2355;} else{ goto _LL2356;} _LL2356: if(
_temp2346 ==( void*) Cyc_Absyn_DoubleType){ goto _LL2357;} else{ goto _LL2358;}
_LL2358: goto _LL2359; _LL2349: goto _LL2347; _LL2351:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp2370=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2370[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2371; _temp2371.tag= Cyc_Absyn_Const_e; _temp2371.f1=(
void*)(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp2372=( struct Cyc_Absyn_Char_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct)); _temp2372[ 0]=({ struct Cyc_Absyn_Char_c_struct
_temp2373; _temp2373.tag= Cyc_Absyn_Char_c; _temp2373.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp2373.f2='\000'; _temp2373;}); _temp2372;})); _temp2371;}); _temp2370;})));
goto _LL2347; _LL2353:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2374=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2374[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2375; _temp2375.tag= Cyc_Absyn_Const_e;
_temp2375.f1=( void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp2376=(
struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp2376[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp2377; _temp2377.tag= Cyc_Absyn_Int_c;
_temp2377.f1=( void*) _temp2368; _temp2377.f2= 0; _temp2377;}); _temp2376;}));
_temp2375;}); _temp2374;}))); if( _temp2366 !=( void*) Cyc_Absyn_B4){ e= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Cast_e_struct* _temp2378=( struct Cyc_Absyn_Cast_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp2378[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp2379; _temp2379.tag= Cyc_Absyn_Cast_e; _temp2379.f1=( void*) t; _temp2379.f2=
e; _temp2379;}); _temp2378;}), loc);} goto _LL2347; _LL2355:( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp2380=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2380[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2381; _temp2381.tag= Cyc_Absyn_Const_e; _temp2381.f1=(
void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp2382=( struct Cyc_Absyn_Float_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Float_c_struct)); _temp2382[ 0]=({ struct
Cyc_Absyn_Float_c_struct _temp2383; _temp2383.tag= Cyc_Absyn_Float_c; _temp2383.f1=(
struct _tagged_string)({ char* _temp2384=( char*)"0.0"; struct _tagged_string
_temp2385; _temp2385.curr= _temp2384; _temp2385.base= _temp2384; _temp2385.last_plus_one=
_temp2384 + 4; _temp2385;}); _temp2383;}); _temp2382;})); _temp2381;});
_temp2380;}))); goto _LL2347; _LL2357:( void*)( e->r=( void*)(( void*)({ struct
Cyc_Absyn_Cast_e_struct* _temp2386=( struct Cyc_Absyn_Cast_e_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp2386[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp2387; _temp2387.tag= Cyc_Absyn_Cast_e; _temp2387.f1=( void*) t; _temp2387.f2=
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp2388=( struct
Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2388[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2389; _temp2389.tag= Cyc_Absyn_Const_e;
_temp2389.f1=( void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp2390=(
struct Cyc_Absyn_Float_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Float_c_struct));
_temp2390[ 0]=({ struct Cyc_Absyn_Float_c_struct _temp2391; _temp2391.tag= Cyc_Absyn_Float_c;
_temp2391.f1=( struct _tagged_string)({ char* _temp2392=( char*)"0.0"; struct
_tagged_string _temp2393; _temp2393.curr= _temp2392; _temp2393.base= _temp2392;
_temp2393.last_plus_one= _temp2392 + 4; _temp2393;}); _temp2391;}); _temp2390;}));
_temp2389;}); _temp2388;}), loc); _temp2387;}); _temp2386;}))); goto _LL2347;
_LL2359: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2394= Cyc_Absynpp_typ2string(
t); xprintf("declaration of type %.*s requires initializer", _temp2394.last_plus_one
- _temp2394.curr, _temp2394.curr);})); goto _LL2347; _LL2347:;} return e;}
struct _tuple5* Cyc_Tcutil_make_inst_var( struct Cyc_Absyn_Tvar* tv){ void* k=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( tv->kind); return({
struct _tuple5* _temp2397=( struct _tuple5*) GC_malloc( sizeof( struct _tuple5));
_temp2397->f1= tv; _temp2397->f2= Cyc_Absyn_new_evar( k); _temp2397;});} struct
_tuple5* Cyc_Tcutil_r_make_inst_var( struct _RegionHandle* rgn, struct Cyc_Absyn_Tvar*
tv){ void* k=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( tv->kind);
return({ struct _tuple5* _temp2398=( struct _tuple5*) _region_malloc( rgn,
sizeof( struct _tuple5)); _temp2398->f1= tv; _temp2398->f2= Cyc_Absyn_new_evar(
k); _temp2398;});} struct Cyc_List_List* Cyc_Tcutil_check_valid_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* free_vars, void*
expected_kind, void* t){{ void* _temp2399= Cyc_Tcutil_compress( t); int
_temp2445; struct Cyc_Core_Opt* _temp2447; void* _temp2449; struct Cyc_Absyn_Tvar*
_temp2451; struct Cyc_Absyn_Enumdecl* _temp2453; struct Cyc_Absyn_Enumdecl**
_temp2455; struct _tuple1* _temp2456; struct Cyc_Absyn_TunionInfo _temp2458;
void* _temp2460; struct Cyc_List_List* _temp2462; void* _temp2464; void**
_temp2466; struct Cyc_Absyn_TunionFieldInfo _temp2467; struct Cyc_List_List*
_temp2469; void* _temp2471; void** _temp2473; struct Cyc_Absyn_PtrInfo _temp2474;
struct Cyc_Absyn_Conref* _temp2476; struct Cyc_Absyn_Tqual _temp2478; struct Cyc_Absyn_Conref*
_temp2480; void* _temp2482; void* _temp2484; void* _temp2486; void* _temp2488;
struct Cyc_Absyn_Exp* _temp2490; struct Cyc_Absyn_Tqual _temp2492; void*
_temp2494; struct Cyc_Absyn_FnInfo _temp2496; struct Cyc_List_List* _temp2498;
int _temp2500; struct Cyc_List_List* _temp2502; void* _temp2504; struct Cyc_Core_Opt*
_temp2506; struct Cyc_Core_Opt** _temp2508; struct Cyc_List_List* _temp2509;
struct Cyc_List_List** _temp2511; struct Cyc_List_List* _temp2512; struct Cyc_List_List*
_temp2514; struct Cyc_List_List* _temp2516; struct Cyc_Absyn_Structdecl**
_temp2518; struct Cyc_Absyn_Structdecl*** _temp2520; struct Cyc_List_List*
_temp2521; struct _tuple1* _temp2523; struct Cyc_Absyn_Uniondecl** _temp2525;
struct Cyc_Absyn_Uniondecl*** _temp2527; struct Cyc_List_List* _temp2528; struct
_tuple1* _temp2530; struct Cyc_Core_Opt* _temp2532; struct Cyc_Core_Opt**
_temp2534; struct Cyc_List_List* _temp2535; struct _tuple1* _temp2537; void*
_temp2539; void* _temp2541; struct Cyc_List_List* _temp2543; _LL2401: if(
_temp2399 ==( void*) Cyc_Absyn_VoidType){ goto _LL2402;} else{ goto _LL2403;}
_LL2403: if(( unsigned int) _temp2399 > 4u?*(( int*) _temp2399) == Cyc_Absyn_Evar:
0){ _LL2450: _temp2449=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp2399)->f1;
goto _LL2448; _LL2448: _temp2447=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp2399)->f2; goto _LL2446; _LL2446: _temp2445=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp2399)->f3; goto _LL2404;} else{ goto _LL2405;} _LL2405: if(( unsigned int)
_temp2399 > 4u?*(( int*) _temp2399) == Cyc_Absyn_VarType: 0){ _LL2452: _temp2451=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*) _temp2399)->f1; goto
_LL2406;} else{ goto _LL2407;} _LL2407: if(( unsigned int) _temp2399 > 4u?*((
int*) _temp2399) == Cyc_Absyn_EnumType: 0){ _LL2457: _temp2456=( struct _tuple1*)((
struct Cyc_Absyn_EnumType_struct*) _temp2399)->f1; goto _LL2454; _LL2454:
_temp2453=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*)
_temp2399)->f2; _temp2455=&(( struct Cyc_Absyn_EnumType_struct*) _temp2399)->f2;
goto _LL2408;} else{ goto _LL2409;} _LL2409: if(( unsigned int) _temp2399 > 4u?*((
int*) _temp2399) == Cyc_Absyn_TunionType: 0){ _LL2459: _temp2458=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp2399)->f1; _LL2465: _temp2464=( void*)
_temp2458.tunion_info; _temp2466=&((( struct Cyc_Absyn_TunionType_struct*)
_temp2399)->f1).tunion_info; goto _LL2463; _LL2463: _temp2462=( struct Cyc_List_List*)
_temp2458.targs; goto _LL2461; _LL2461: _temp2460=( void*) _temp2458.rgn; goto
_LL2410;} else{ goto _LL2411;} _LL2411: if(( unsigned int) _temp2399 > 4u?*((
int*) _temp2399) == Cyc_Absyn_TunionFieldType: 0){ _LL2468: _temp2467=( struct
Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*) _temp2399)->f1;
_LL2472: _temp2471=( void*) _temp2467.field_info; _temp2473=&((( struct Cyc_Absyn_TunionFieldType_struct*)
_temp2399)->f1).field_info; goto _LL2470; _LL2470: _temp2469=( struct Cyc_List_List*)
_temp2467.targs; goto _LL2412;} else{ goto _LL2413;} _LL2413: if(( unsigned int)
_temp2399 > 4u?*(( int*) _temp2399) == Cyc_Absyn_PointerType: 0){ _LL2475:
_temp2474=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp2399)->f1; _LL2485: _temp2484=( void*) _temp2474.elt_typ; goto _LL2483;
_LL2483: _temp2482=( void*) _temp2474.rgn_typ; goto _LL2481; _LL2481: _temp2480=(
struct Cyc_Absyn_Conref*) _temp2474.nullable; goto _LL2479; _LL2479: _temp2478=(
struct Cyc_Absyn_Tqual) _temp2474.tq; goto _LL2477; _LL2477: _temp2476=( struct
Cyc_Absyn_Conref*) _temp2474.bounds; goto _LL2414;} else{ goto _LL2415;} _LL2415:
if(( unsigned int) _temp2399 > 4u?*(( int*) _temp2399) == Cyc_Absyn_IntType: 0){
_LL2489: _temp2488=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2399)->f1;
goto _LL2487; _LL2487: _temp2486=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2399)->f2; goto _LL2416;} else{ goto _LL2417;} _LL2417: if( _temp2399 ==(
void*) Cyc_Absyn_FloatType){ goto _LL2418;} else{ goto _LL2419;} _LL2419: if(
_temp2399 ==( void*) Cyc_Absyn_DoubleType){ goto _LL2420;} else{ goto _LL2421;}
_LL2421: if(( unsigned int) _temp2399 > 4u?*(( int*) _temp2399) == Cyc_Absyn_ArrayType:
0){ _LL2495: _temp2494=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2399)->f1;
goto _LL2493; _LL2493: _temp2492=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2399)->f2; goto _LL2491; _LL2491: _temp2490=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2399)->f3; goto _LL2422;} else{ goto
_LL2423;} _LL2423: if(( unsigned int) _temp2399 > 4u?*(( int*) _temp2399) == Cyc_Absyn_FnType:
0){ _LL2497: _temp2496=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp2399)->f1; _LL2510: _temp2509=( struct Cyc_List_List*) _temp2496.tvars;
_temp2511=&((( struct Cyc_Absyn_FnType_struct*) _temp2399)->f1).tvars; goto
_LL2507; _LL2507: _temp2506=( struct Cyc_Core_Opt*) _temp2496.effect; _temp2508=&(((
struct Cyc_Absyn_FnType_struct*) _temp2399)->f1).effect; goto _LL2505; _LL2505:
_temp2504=( void*) _temp2496.ret_typ; goto _LL2503; _LL2503: _temp2502=( struct
Cyc_List_List*) _temp2496.args; goto _LL2501; _LL2501: _temp2500=( int)
_temp2496.varargs; goto _LL2499; _LL2499: _temp2498=( struct Cyc_List_List*)
_temp2496.attributes; goto _LL2424;} else{ goto _LL2425;} _LL2425: if((
unsigned int) _temp2399 > 4u?*(( int*) _temp2399) == Cyc_Absyn_TupleType: 0){
_LL2513: _temp2512=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp2399)->f1; goto _LL2426;} else{ goto _LL2427;} _LL2427: if(( unsigned int)
_temp2399 > 4u?*(( int*) _temp2399) == Cyc_Absyn_AnonStructType: 0){ _LL2515:
_temp2514=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp2399)->f1; goto _LL2428;} else{ goto _LL2429;} _LL2429: if(( unsigned int)
_temp2399 > 4u?*(( int*) _temp2399) == Cyc_Absyn_AnonUnionType: 0){ _LL2517:
_temp2516=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp2399)->f1; goto _LL2430;} else{ goto _LL2431;} _LL2431: if(( unsigned int)
_temp2399 > 4u?*(( int*) _temp2399) == Cyc_Absyn_StructType: 0){ _LL2524:
_temp2523=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp2399)->f1;
goto _LL2522; _LL2522: _temp2521=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp2399)->f2; goto _LL2519; _LL2519: _temp2518=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp2399)->f3; _temp2520=&(( struct Cyc_Absyn_StructType_struct*)
_temp2399)->f3; goto _LL2432;} else{ goto _LL2433;} _LL2433: if(( unsigned int)
_temp2399 > 4u?*(( int*) _temp2399) == Cyc_Absyn_UnionType: 0){ _LL2531:
_temp2530=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*) _temp2399)->f1;
goto _LL2529; _LL2529: _temp2528=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp2399)->f2; goto _LL2526; _LL2526: _temp2525=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp2399)->f3; _temp2527=&(( struct Cyc_Absyn_UnionType_struct*)
_temp2399)->f3; goto _LL2434;} else{ goto _LL2435;} _LL2435: if(( unsigned int)
_temp2399 > 4u?*(( int*) _temp2399) == Cyc_Absyn_TypedefType: 0){ _LL2538:
_temp2537=( struct _tuple1*)(( struct Cyc_Absyn_TypedefType_struct*) _temp2399)->f1;
goto _LL2536; _LL2536: _temp2535=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp2399)->f2; goto _LL2533; _LL2533: _temp2532=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_TypedefType_struct*) _temp2399)->f3; _temp2534=&(( struct Cyc_Absyn_TypedefType_struct*)
_temp2399)->f3; goto _LL2436;} else{ goto _LL2437;} _LL2437: if( _temp2399 ==(
void*) Cyc_Absyn_HeapRgn){ goto _LL2438;} else{ goto _LL2439;} _LL2439: if((
unsigned int) _temp2399 > 4u?*(( int*) _temp2399) == Cyc_Absyn_RgnHandleType: 0){
_LL2540: _temp2539=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp2399)->f1;
goto _LL2440;} else{ goto _LL2441;} _LL2441: if(( unsigned int) _temp2399 > 4u?*((
int*) _temp2399) == Cyc_Absyn_AccessEff: 0){ _LL2542: _temp2541=( void*)((
struct Cyc_Absyn_AccessEff_struct*) _temp2399)->f1; goto _LL2442;} else{ goto
_LL2443;} _LL2443: if(( unsigned int) _temp2399 > 4u?*(( int*) _temp2399) == Cyc_Absyn_JoinEff:
0){ _LL2544: _temp2543=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*)
_temp2399)->f1; goto _LL2444;} else{ goto _LL2400;} _LL2402: goto _LL2400;
_LL2404: goto _LL2400; _LL2406: { struct Cyc_Absyn_Conref* c=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp2451->kind); if((
void*) c->v ==( void*) Cyc_Absyn_No_constr){( void*)( c->v=( void*)(( void*)({
struct Cyc_Absyn_Eq_constr_struct* _temp2545=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp2545[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp2546; _temp2546.tag= Cyc_Absyn_Eq_constr;
_temp2546.f1=( void*) expected_kind; _temp2546;}); _temp2545;})));} free_vars=
Cyc_Tcutil_add_free_tvar( loc, free_vars, _temp2451); goto _LL2400;} _LL2408:
if(* _temp2455 == 0){ struct _handler_cons _temp2547; _push_handler(& _temp2547);{
int _temp2549= 0; if( setjmp( _temp2547.handler)){ _temp2549= 1;} if( !
_temp2549){{ struct Cyc_Absyn_Enumdecl** ed= Cyc_Tcenv_lookup_enumdecl( te, loc,
_temp2456);* _temp2455=( struct Cyc_Absyn_Enumdecl*)* ed;}; _pop_handler();}
else{ void* _temp2548=( void*) _exn_thrown; void* _temp2551= _temp2548; _LL2553:
if( _temp2551 == Cyc_Dict_Absent){ goto _LL2554;} else{ goto _LL2555;} _LL2555:
goto _LL2556; _LL2554: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2557=
Cyc_Absynpp_qvar2string( _temp2456); xprintf("unbound type enum %.*s", _temp2557.last_plus_one
- _temp2557.curr, _temp2557.curr);})); return free_vars; _LL2556:( void) _throw(
_temp2551); _LL2552:;}}}{ struct Cyc_Absyn_Enumdecl* ed=( struct Cyc_Absyn_Enumdecl*)({
struct Cyc_Absyn_Enumdecl* _temp2560=* _temp2455; if( _temp2560 == 0){ _throw(
Null_Exception);} _temp2560;});* _temp2456=({ struct _tuple1* _temp2558= ed->name;
unsigned int _temp2559= 0; if( _temp2559 >= 1u){ _throw( Null_Exception);}
_temp2558[ _temp2559];}); goto _LL2400;} _LL2410:{ void* _temp2561=* _temp2466;
struct Cyc_Absyn_UnknownTunionInfo _temp2567; int _temp2569; struct _tuple1*
_temp2571; struct Cyc_Absyn_Tuniondecl* _temp2573; _LL2563: if(*(( int*)
_temp2561) == Cyc_Absyn_UnknownTunion){ _LL2568: _temp2567=( struct Cyc_Absyn_UnknownTunionInfo)((
struct Cyc_Absyn_UnknownTunion_struct*) _temp2561)->f1; _LL2572: _temp2571=(
struct _tuple1*) _temp2567.name; goto _LL2570; _LL2570: _temp2569=( int)
_temp2567.is_xtunion; goto _LL2564;} else{ goto _LL2565;} _LL2565: if(*(( int*)
_temp2561) == Cyc_Absyn_KnownTunion){ _LL2574: _temp2573=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunion_struct*) _temp2561)->f1; goto _LL2566;} else{ goto
_LL2562;} _LL2564: { struct Cyc_Absyn_Tuniondecl* tud;{ struct _handler_cons
_temp2575; _push_handler(& _temp2575);{ int _temp2577= 0; if( setjmp( _temp2575.handler)){
_temp2577= 1;} if( ! _temp2577){ tud=* Cyc_Tcenv_lookup_tuniondecl( te, loc,
_temp2571);; _pop_handler();} else{ void* _temp2576=( void*) _exn_thrown; void*
_temp2579= _temp2576; _LL2581: if( _temp2579 == Cyc_Dict_Absent){ goto _LL2582;}
else{ goto _LL2583;} _LL2583: goto _LL2584; _LL2582: Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2585= Cyc_Absynpp_qvar2string( _temp2571); xprintf("unbound type [x]tunion %.*s",
_temp2585.last_plus_one - _temp2585.curr, _temp2585.curr);})); return free_vars;
_LL2584:( void) _throw( _temp2579); _LL2580:;}}} if( tud->is_xtunion !=
_temp2569){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2586= Cyc_Absynpp_qvar2string(
_temp2571); xprintf("[x]tunion is different from type declaration %.*s",
_temp2586.last_plus_one - _temp2586.curr, _temp2586.curr);}));}* _temp2466=(
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp2587=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp2587[ 0]=({
struct Cyc_Absyn_KnownTunion_struct _temp2588; _temp2588.tag= Cyc_Absyn_KnownTunion;
_temp2588.f1= tud; _temp2588;}); _temp2587;}); _temp2573= tud; goto _LL2566;}
_LL2566: free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars,( void*) Cyc_Absyn_RgnKind,
_temp2460);{ struct Cyc_List_List* tvs= _temp2573->tvs; for( 0; _temp2462 != 0?
tvs != 0: 0; _temp2462=({ struct Cyc_List_List* _temp2589= _temp2462; if(
_temp2589 == 0){ _throw( Null_Exception);} _temp2589->tl;}), tvs=({ struct Cyc_List_List*
_temp2590= tvs; if( _temp2590 == 0){ _throw( Null_Exception);} _temp2590->tl;})){
void* t1=( void*)({ struct Cyc_List_List* _temp2592= _temp2462; if( _temp2592 ==
0){ _throw( Null_Exception);} _temp2592->hd;}); void* k1=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp2591= tvs; if( _temp2591 == 0){ _throw( Null_Exception);} _temp2591->hd;}))->kind);
free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars, k1, t1);} if(
_temp2462 != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2593= Cyc_Absynpp_qvar2string(
_temp2573->name); xprintf("too many type arguments for tunion %.*s", _temp2593.last_plus_one
- _temp2593.curr, _temp2593.curr);}));} if( tvs != 0){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2594= Cyc_Absynpp_qvar2string( _temp2573->name);
xprintf("too few type arguments for tunion %.*s", _temp2594.last_plus_one -
_temp2594.curr, _temp2594.curr);}));} goto _LL2562;} _LL2562:;} goto _LL2400;
_LL2412:{ void* _temp2595=* _temp2473; struct Cyc_Absyn_UnknownTunionFieldInfo
_temp2601; int _temp2603; struct _tuple1* _temp2605; struct _tuple1* _temp2607;
struct Cyc_Absyn_Tunionfield* _temp2609; struct Cyc_Absyn_Tuniondecl* _temp2611;
_LL2597: if(*(( int*) _temp2595) == Cyc_Absyn_UnknownTunionfield){ _LL2602:
_temp2601=( struct Cyc_Absyn_UnknownTunionFieldInfo)(( struct Cyc_Absyn_UnknownTunionfield_struct*)
_temp2595)->f1; _LL2608: _temp2607=( struct _tuple1*) _temp2601.tunion_name;
goto _LL2606; _LL2606: _temp2605=( struct _tuple1*) _temp2601.field_name; goto
_LL2604; _LL2604: _temp2603=( int) _temp2601.is_xtunion; goto _LL2598;} else{
goto _LL2599;} _LL2599: if(*(( int*) _temp2595) == Cyc_Absyn_KnownTunionfield){
_LL2612: _temp2611=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp2595)->f1; goto _LL2610; _LL2610: _temp2609=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp2595)->f2; goto _LL2600;} else{
goto _LL2596;} _LL2598: { struct Cyc_Absyn_Tuniondecl* tud; struct Cyc_Absyn_Tunionfield*
tuf;{ struct _handler_cons _temp2613; _push_handler(& _temp2613);{ int _temp2615=
0; if( setjmp( _temp2613.handler)){ _temp2615= 1;} if( ! _temp2615){* Cyc_Tcenv_lookup_tuniondecl(
te, loc, _temp2607);; _pop_handler();} else{ void* _temp2614=( void*)
_exn_thrown; void* _temp2617= _temp2614; _LL2619: if( _temp2617 == Cyc_Dict_Absent){
goto _LL2620;} else{ goto _LL2621;} _LL2621: goto _LL2622; _LL2620: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2623= Cyc_Absynpp_qvar2string( _temp2607);
xprintf("unbound type tunion %.*s", _temp2623.last_plus_one - _temp2623.curr,
_temp2623.curr);})); return free_vars; _LL2622:( void) _throw( _temp2617);
_LL2618:;}}}{ struct _handler_cons _temp2624; _push_handler(& _temp2624);{ int
_temp2626= 0; if( setjmp( _temp2624.handler)){ _temp2626= 1;} if( ! _temp2626){{
void* _temp2627= Cyc_Tcenv_lookup_ordinary( te, loc, _temp2605); struct Cyc_Absyn_Tunionfield*
_temp2633; struct Cyc_Absyn_Tuniondecl* _temp2635; _LL2629: if(*(( int*)
_temp2627) == Cyc_Tcenv_TunionRes){ _LL2636: _temp2635=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Tcenv_TunionRes_struct*) _temp2627)->f1; goto _LL2634; _LL2634:
_temp2633=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Tcenv_TunionRes_struct*)
_temp2627)->f2; goto _LL2630;} else{ goto _LL2631;} _LL2631: goto _LL2632;
_LL2630: tuf= _temp2633; tud= _temp2635; if( tud->is_xtunion != _temp2603){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2637= Cyc_Absynpp_qvar2string( _temp2607);
xprintf("[x]tunion is different from type declaration %.*s", _temp2637.last_plus_one
- _temp2637.curr, _temp2637.curr);}));} goto _LL2628; _LL2632: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2638= Cyc_Absynpp_qvar2string( _temp2605);
struct _tagged_string _temp2639= Cyc_Absynpp_qvar2string( _temp2607); xprintf("unbound field %.*s in type tunion %.*s",
_temp2638.last_plus_one - _temp2638.curr, _temp2638.curr, _temp2639.last_plus_one
- _temp2639.curr, _temp2639.curr);}));{ struct Cyc_List_List* _temp2640=
free_vars; _npop_handler( 0u); return _temp2640;} _LL2628:;}; _pop_handler();}
else{ void* _temp2625=( void*) _exn_thrown; void* _temp2642= _temp2625; _LL2644:
if( _temp2642 == Cyc_Dict_Absent){ goto _LL2645;} else{ goto _LL2646;} _LL2646:
goto _LL2647; _LL2645: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2648=
Cyc_Absynpp_qvar2string( _temp2605); struct _tagged_string _temp2649= Cyc_Absynpp_qvar2string(
_temp2607); xprintf("unbound field %.*s in type tunion %.*s", _temp2648.last_plus_one
- _temp2648.curr, _temp2648.curr, _temp2649.last_plus_one - _temp2649.curr,
_temp2649.curr);})); return free_vars; _LL2647:( void) _throw( _temp2642);
_LL2643:;}}}* _temp2473=( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp2650=( struct Cyc_Absyn_KnownTunionfield_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp2650[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp2651; _temp2651.tag= Cyc_Absyn_KnownTunionfield; _temp2651.f1= tud;
_temp2651.f2= tuf; _temp2651;}); _temp2650;}); _temp2611= tud; _temp2609= tuf;
goto _LL2600;} _LL2600: { struct Cyc_List_List* tvs= _temp2611->tvs; for( 0;
_temp2469 != 0? tvs != 0: 0; _temp2469=({ struct Cyc_List_List* _temp2652=
_temp2469; if( _temp2652 == 0){ _throw( Null_Exception);} _temp2652->tl;}), tvs=({
struct Cyc_List_List* _temp2653= tvs; if( _temp2653 == 0){ _throw(
Null_Exception);} _temp2653->tl;})){ void* t1=( void*)({ struct Cyc_List_List*
_temp2655= _temp2469; if( _temp2655 == 0){ _throw( Null_Exception);} _temp2655->hd;});
void* k1=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(((
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp2654= tvs; if( _temp2654 ==
0){ _throw( Null_Exception);} _temp2654->hd;}))->kind); free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars, k1, t1);} if( _temp2469 != 0){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2656= Cyc_Absynpp_qvar2string( _temp2611->name);
struct _tagged_string _temp2657= Cyc_Absynpp_qvar2string( _temp2609->name);
xprintf("too many type arguments for tunion %.*s.%.*s", _temp2656.last_plus_one
- _temp2656.curr, _temp2656.curr, _temp2657.last_plus_one - _temp2657.curr,
_temp2657.curr);}));} if( tvs != 0){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2658= Cyc_Absynpp_qvar2string( _temp2611->name); struct
_tagged_string _temp2659= Cyc_Absynpp_qvar2string( _temp2609->name); xprintf("too few type arguments for tunion %.*s.%.*s",
_temp2658.last_plus_one - _temp2658.curr, _temp2658.curr, _temp2659.last_plus_one
- _temp2659.curr, _temp2659.curr);}));} goto _LL2596;} _LL2596:;} goto _LL2400;
_LL2414: free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars,( void*) Cyc_Absyn_AnyKind,
_temp2484); free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars,( void*)
Cyc_Absyn_RgnKind, _temp2482);{ void* _temp2660=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp2476))->v; void*
_temp2666; struct Cyc_Absyn_Exp* _temp2668; _LL2662: if(( unsigned int)
_temp2660 > 1u?*(( int*) _temp2660) == Cyc_Absyn_Eq_constr: 0){ _LL2667:
_temp2666=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2660)->f1; if((
unsigned int) _temp2666 > 1u?*(( int*) _temp2666) == Cyc_Absyn_Upper_b: 0){
_LL2669: _temp2668=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp2666)->f1; goto _LL2663;} else{ goto _LL2664;}} else{ goto _LL2664;}
_LL2664: goto _LL2665; _LL2663: if( ! Cyc_Tcutil_is_const_exp( te, _temp2668)){
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp2670=( char*)"pointer bounds expression is not a constant";
struct _tagged_string _temp2671; _temp2671.curr= _temp2670; _temp2671.base=
_temp2670; _temp2671.last_plus_one= _temp2670 + 44; _temp2671;}));} Cyc_Tcexp_tcExp(
te, 0, _temp2668); if( ! Cyc_Tcutil_coerce_uint_typ( te, _temp2668)){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp2672=( char*)"pointer bounds expression is not an unsigned int";
struct _tagged_string _temp2673; _temp2673.curr= _temp2672; _temp2673.base=
_temp2672; _temp2673.last_plus_one= _temp2672 + 49; _temp2673;}));} Cyc_Evexp_eval_const_uint_exp(
_temp2668); goto _LL2661; _LL2665: goto _LL2661; _LL2661:;} goto _LL2400;
_LL2416: goto _LL2400; _LL2418: goto _LL2400; _LL2420: goto _LL2400; _LL2422:
free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars,( void*) Cyc_Absyn_MemKind,
_temp2494); if( _temp2490 == 0? 1: ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp2674= _temp2490; if( _temp2674 == 0){ _throw(
Null_Exception);} _temp2674;}))){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({
char* _temp2675=( char*)"array bounds expression is not constant"; struct
_tagged_string _temp2676; _temp2676.curr= _temp2675; _temp2676.base= _temp2675;
_temp2676.last_plus_one= _temp2675 + 40; _temp2676;}));} Cyc_Tcexp_tcExp( te, 0,(
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2677= _temp2490; if(
_temp2677 == 0){ _throw( Null_Exception);} _temp2677;})); if( ! Cyc_Tcutil_coerce_uint_typ(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2678= _temp2490; if(
_temp2678 == 0){ _throw( Null_Exception);} _temp2678;}))){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp2679=( char*)"array bounds expression is not an unsigned int";
struct _tagged_string _temp2680; _temp2680.curr= _temp2679; _temp2680.base=
_temp2679; _temp2680.last_plus_one= _temp2679 + 47; _temp2680;}));} Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2681= _temp2490; if(
_temp2681 == 0){ _throw( Null_Exception);} _temp2681;})); goto _LL2400; _LL2424:
if( _temp2498 != 0){ int seen_cdecl= 0; int seen_stdcall= 0; for( 0; _temp2498
!= 0; _temp2498=({ struct Cyc_List_List* _temp2682= _temp2498; if( _temp2682 ==
0){ _throw( Null_Exception);} _temp2682->tl;})){ if( ! Cyc_Tcutil_fntype_att((
void*)({ struct Cyc_List_List* _temp2683= _temp2498; if( _temp2683 == 0){ _throw(
Null_Exception);} _temp2683->hd;}))){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2685= Cyc_Absyn_attribute2string(( void*)({ struct Cyc_List_List*
_temp2684= _temp2498; if( _temp2684 == 0){ _throw( Null_Exception);} _temp2684->hd;}));
xprintf("bad function type attribute %.*s", _temp2685.last_plus_one - _temp2685.curr,
_temp2685.curr);}));} if(( void*)({ struct Cyc_List_List* _temp2686= _temp2498;
if( _temp2686 == 0){ _throw( Null_Exception);} _temp2686->hd;}) ==( void*) Cyc_Absyn_Stdcall_att){
if( seen_cdecl){ Cyc_Tcutil_terr( loc, xprintf("function can't be both cdecl and stdcall"));}
seen_stdcall= 1;} else{ if(( void*)({ struct Cyc_List_List* _temp2687= _temp2498;
if( _temp2687 == 0){ _throw( Null_Exception);} _temp2687->hd;}) ==( void*) Cyc_Absyn_Cdecl_att){
if( seen_stdcall){ Cyc_Tcutil_terr( loc, xprintf("function can't be both cdecl and stdcall"));}
seen_cdecl= 1;}}}} Cyc_Tcutil_check_unique_tvars( loc,* _temp2511);{ struct Cyc_List_List*
b=* _temp2511; for( 0; b != 0; b=({ struct Cyc_List_List* _temp2688= b; if(
_temp2688 == 0){ _throw( Null_Exception);} _temp2688->tl;})){ void* _temp2690=(
void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp2689= b; if( _temp2689 == 0){
_throw( Null_Exception);} _temp2689->hd;}))->kind))->v; void* _temp2696; _LL2692:
if(( unsigned int) _temp2690 > 1u?*(( int*) _temp2690) == Cyc_Absyn_Eq_constr: 0){
_LL2697: _temp2696=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2690)->f1;
if( _temp2696 ==( void*) Cyc_Absyn_MemKind){ goto _LL2693;} else{ goto _LL2694;}}
else{ goto _LL2694;} _LL2694: goto _LL2695; _LL2693: Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2699=*(( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp2698= b; if( _temp2698 == 0){ _throw( Null_Exception);} _temp2698->hd;}))->name;
xprintf("function attempts to abstract Mem type variable %.*s", _temp2699.last_plus_one
- _temp2699.curr, _temp2699.curr);})); goto _LL2691; _LL2695: goto _LL2691;
_LL2691:;}}{ struct Cyc_List_List* new_free_vars= 0; new_free_vars= Cyc_Tcutil_check_valid_type(
loc, te, new_free_vars,( void*) Cyc_Absyn_MemKind, _temp2504); for( 0; _temp2502
!= 0; _temp2502=({ struct Cyc_List_List* _temp2700= _temp2502; if( _temp2700 ==
0){ _throw( Null_Exception);} _temp2700->tl;})){ new_free_vars= Cyc_Tcutil_check_valid_type(
loc, te, new_free_vars,( void*) Cyc_Absyn_MemKind,(*(( struct _tuple2*)({ struct
Cyc_List_List* _temp2701= _temp2502; if( _temp2701 == 0){ _throw( Null_Exception);}
_temp2701->hd;}))).f3);} if(* _temp2508 != 0){ new_free_vars= Cyc_Tcutil_check_valid_type(
loc, te, new_free_vars,( void*) Cyc_Absyn_EffKind,( void*)({ struct Cyc_Core_Opt*
_temp2702=* _temp2508; if( _temp2702 == 0){ _throw( Null_Exception);} _temp2702->v;}));}
else{ struct Cyc_Absyn_Tvar* e= Cyc_Tcutil_new_tvar(( void*) Cyc_Absyn_EffKind);
new_free_vars=({ struct Cyc_List_List* _temp2703=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2703->hd=( void*) e; _temp2703->tl=
new_free_vars; _temp2703;});{ struct Cyc_List_List* effect= 0;{ struct Cyc_List_List*
tvs= new_free_vars; for( 0; tvs != 0; tvs=({ struct Cyc_List_List* _temp2704=
tvs; if( _temp2704 == 0){ _throw( Null_Exception);} _temp2704->tl;})){ void*
_temp2706=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp2705= tvs; if( _temp2705 == 0){ _throw( Null_Exception);} _temp2705->hd;}))->kind))->v;
void* _temp2714; void* _temp2716; _LL2708: if(( unsigned int) _temp2706 > 1u?*((
int*) _temp2706) == Cyc_Absyn_Eq_constr: 0){ _LL2715: _temp2714=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp2706)->f1; if( _temp2714 ==( void*) Cyc_Absyn_RgnKind){
goto _LL2709;} else{ goto _LL2710;}} else{ goto _LL2710;} _LL2710: if((
unsigned int) _temp2706 > 1u?*(( int*) _temp2706) == Cyc_Absyn_Eq_constr: 0){
_LL2717: _temp2716=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2706)->f1;
if( _temp2716 ==( void*) Cyc_Absyn_EffKind){ goto _LL2711;} else{ goto _LL2712;}}
else{ goto _LL2712;} _LL2712: goto _LL2713; _LL2709: effect=({ struct Cyc_List_List*
_temp2718=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2718->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2719=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2719[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2720; _temp2720.tag= Cyc_Absyn_AccessEff;
_temp2720.f1=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp2721=(
struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2721[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2722; _temp2722.tag= Cyc_Absyn_VarType;
_temp2722.f1=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp2723= tvs;
if( _temp2723 == 0){ _throw( Null_Exception);} _temp2723->hd;}); _temp2722;});
_temp2721;})); _temp2720;}); _temp2719;})); _temp2718->tl= effect; _temp2718;});
goto _LL2707; _LL2711: effect=({ struct Cyc_List_List* _temp2724=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2724->hd=( void*)(( void*)({
struct Cyc_Absyn_VarType_struct* _temp2725=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp2725[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp2726; _temp2726.tag= Cyc_Absyn_VarType; _temp2726.f1=(
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp2727= tvs; if( _temp2727 ==
0){ _throw( Null_Exception);} _temp2727->hd;}); _temp2726;}); _temp2725;}));
_temp2724->tl= effect; _temp2724;}); goto _LL2707; _LL2713: goto _LL2707;
_LL2707:;}}* _temp2508=({ struct Cyc_Core_Opt* _temp2728=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2728->v=( void*)(( void*)({
struct Cyc_Absyn_JoinEff_struct* _temp2729=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp2729[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp2730; _temp2730.tag= Cyc_Absyn_JoinEff; _temp2730.f1=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( effect);
_temp2730;}); _temp2729;})); _temp2728;});}} if(* _temp2511 != 0){ struct Cyc_List_List*
new_btvs= 0;{ struct Cyc_List_List* bs=* _temp2511; for( 0; bs != 0; bs=({
struct Cyc_List_List* _temp2731= bs; if( _temp2731 == 0){ _throw( Null_Exception);}
_temp2731->tl;})){ new_free_vars= Cyc_Tcutil_add_free_tvar( loc, new_free_vars,(
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp2732= bs; if( _temp2732 ==
0){ _throw( Null_Exception);} _temp2732->hd;}));{ struct Cyc_Absyn_Conref*
_temp2734=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp2733= bs; if( _temp2733 ==
0){ _throw( Null_Exception);} _temp2733->hd;}))->kind); goto _LL2735; _LL2735: {
void* _temp2736=( void*) _temp2734->v; void* _temp2744; _LL2738: if( _temp2736
==( void*) Cyc_Absyn_No_constr){ goto _LL2739;} else{ goto _LL2740;} _LL2740:
if(( unsigned int) _temp2736 > 1u?*(( int*) _temp2736) == Cyc_Absyn_Eq_constr: 0){
_LL2745: _temp2744=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2736)->f1;
if( _temp2744 ==( void*) Cyc_Absyn_MemKind){ goto _LL2741;} else{ goto _LL2742;}}
else{ goto _LL2742;} _LL2742: goto _LL2743; _LL2739: Cyc_Tcutil_warn( loc,({
struct _tagged_string _temp2747=*(( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp2746= bs; if( _temp2746 == 0){ _throw( Null_Exception);} _temp2746->hd;}))->name;
xprintf("Type variable %.*s unconstrained, assuming boxed", _temp2747.last_plus_one
- _temp2747.curr, _temp2747.curr);})); goto _LL2741; _LL2741:( void*)( _temp2734->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp2748=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp2748[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp2749; _temp2749.tag= Cyc_Absyn_Eq_constr;
_temp2749.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp2749;}); _temp2748;})));
goto _LL2737; _LL2743: goto _LL2737; _LL2737:;}}}} new_free_vars= Cyc_Tcutil_remove_bound_tvars(
new_free_vars,* _temp2511);} for( 0; new_free_vars != 0; new_free_vars=({ struct
Cyc_List_List* _temp2750= new_free_vars; if( _temp2750 == 0){ _throw(
Null_Exception);} _temp2750->tl;})){ free_vars= Cyc_Tcutil_add_free_tvar( loc,
free_vars,( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp2751=
new_free_vars; if( _temp2751 == 0){ _throw( Null_Exception);} _temp2751->hd;}));}
goto _LL2400;} _LL2426: for( 0; _temp2512 != 0; _temp2512=({ struct Cyc_List_List*
_temp2752= _temp2512; if( _temp2752 == 0){ _throw( Null_Exception);} _temp2752->tl;})){
free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars,( void*) Cyc_Absyn_MemKind,(*((
struct _tuple4*)({ struct Cyc_List_List* _temp2753= _temp2512; if( _temp2753 ==
0){ _throw( Null_Exception);} _temp2753->hd;}))).f2);} goto _LL2400; _LL2428:{
struct _RegionHandle _temp2754= _new_region(); struct _RegionHandle* sprev_rgn=&
_temp2754; _push_region( sprev_rgn);{ struct Cyc_List_List* prev_fields= 0; for(
0; _temp2514 != 0; _temp2514=({ struct Cyc_List_List* _temp2755= _temp2514; if(
_temp2755 == 0){ _throw( Null_Exception);} _temp2755->tl;})){ struct Cyc_Absyn_Structfield
_temp2759; struct Cyc_List_List* _temp2760; struct Cyc_Core_Opt* _temp2762; void*
_temp2764; struct Cyc_Absyn_Tqual _temp2766; struct _tagged_string* _temp2768;
struct Cyc_Absyn_Structfield* _temp2757=( struct Cyc_Absyn_Structfield*)({
struct Cyc_List_List* _temp2756= _temp2514; if( _temp2756 == 0){ _throw(
Null_Exception);} _temp2756->hd;}); _temp2759=* _temp2757; _LL2769: _temp2768=(
struct _tagged_string*) _temp2759.name; goto _LL2767; _LL2767: _temp2766=(
struct Cyc_Absyn_Tqual) _temp2759.tq; goto _LL2765; _LL2765: _temp2764=( void*)
_temp2759.type; goto _LL2763; _LL2763: _temp2762=( struct Cyc_Core_Opt*)
_temp2759.width; goto _LL2761; _LL2761: _temp2760=( struct Cyc_List_List*)
_temp2759.attributes; goto _LL2758; _LL2758: if((( int(*)( int(* compare)(
struct _tagged_string*, struct _tagged_string*), struct Cyc_List_List* l, struct
_tagged_string* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp, prev_fields, _temp2768)){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2770=* _temp2768; xprintf("duplicate field %.*s in struct",
_temp2770.last_plus_one - _temp2770.curr, _temp2770.curr);}));} if( Cyc_String_strcmp(*
_temp2768,( struct _tagged_string)({ char* _temp2771=( char*)""; struct
_tagged_string _temp2772; _temp2772.curr= _temp2771; _temp2772.base= _temp2771;
_temp2772.last_plus_one= _temp2771 + 1; _temp2772;})) != 0){ prev_fields=({
struct Cyc_List_List* _temp2773=( struct Cyc_List_List*) _region_malloc(
sprev_rgn, sizeof( struct Cyc_List_List)); _temp2773->hd=( void*) _temp2768;
_temp2773->tl= prev_fields; _temp2773;});} Cyc_Tcutil_check_valid_type( loc, te,
free_vars,( void*) Cyc_Absyn_MemKind, _temp2764); if( _temp2762 != 0){
unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)({
struct Cyc_Core_Opt* _temp2774= _temp2762; if( _temp2774 == 0){ _throw(
Null_Exception);} _temp2774->v;}))){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2775=* _temp2768; xprintf("bitfield %.*s does not have constant width",
_temp2775.last_plus_one - _temp2775.curr, _temp2775.curr);}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp2776= _temp2762; if(
_temp2776 == 0){ _throw( Null_Exception);} _temp2776->v;}));}{ void* _temp2777=
Cyc_Tcutil_compress( _temp2764); void* _temp2783; void* _temp2785; _LL2779: if((
unsigned int) _temp2777 > 4u?*(( int*) _temp2777) == Cyc_Absyn_IntType: 0){
_LL2786: _temp2785=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2777)->f1;
goto _LL2784; _LL2784: _temp2783=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2777)->f2; goto _LL2780;} else{ goto _LL2781;} _LL2781: goto _LL2782;
_LL2780:{ void* _temp2787= _temp2783; _LL2789: if( _temp2787 ==( void*) Cyc_Absyn_B1){
goto _LL2790;} else{ goto _LL2791;} _LL2791: if( _temp2787 ==( void*) Cyc_Absyn_B2){
goto _LL2792;} else{ goto _LL2793;} _LL2793: if( _temp2787 ==( void*) Cyc_Absyn_B4){
goto _LL2794;} else{ goto _LL2795;} _LL2795: if( _temp2787 ==( void*) Cyc_Absyn_B8){
goto _LL2796;} else{ goto _LL2788;} _LL2790: if( w > 8){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp2797=( char*)"bitfield larger than type";
struct _tagged_string _temp2798; _temp2798.curr= _temp2797; _temp2798.base=
_temp2797; _temp2798.last_plus_one= _temp2797 + 26; _temp2798;}));} goto _LL2788;
_LL2792: if( w > 16){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp2799=( char*)"bitfield larger than type"; struct _tagged_string _temp2800;
_temp2800.curr= _temp2799; _temp2800.base= _temp2799; _temp2800.last_plus_one=
_temp2799 + 26; _temp2800;}));} goto _LL2788; _LL2794: if( w > 32){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp2801=( char*)"bitfield larger than type";
struct _tagged_string _temp2802; _temp2802.curr= _temp2801; _temp2802.base=
_temp2801; _temp2802.last_plus_one= _temp2801 + 26; _temp2802;}));} goto _LL2788;
_LL2796: if( w > 64){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp2803=( char*)"bitfield larger than type"; struct _tagged_string _temp2804;
_temp2804.curr= _temp2803; _temp2804.base= _temp2803; _temp2804.last_plus_one=
_temp2803 + 26; _temp2804;}));} goto _LL2788; _LL2788:;} goto _LL2778; _LL2782:
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2805=* _temp2768; struct
_tagged_string _temp2806= Cyc_Absynpp_typ2string( _temp2764); xprintf("bitfield %.*s must have integral type but has type %.*s",
_temp2805.last_plus_one - _temp2805.curr, _temp2805.curr, _temp2806.last_plus_one
- _temp2806.curr, _temp2806.curr);})); goto _LL2778; _LL2778:;}} for( 0;
_temp2760 != 0; _temp2760=({ struct Cyc_List_List* _temp2807= _temp2760; if(
_temp2807 == 0){ _throw( Null_Exception);} _temp2807->tl;})){ void* _temp2809=(
void*)({ struct Cyc_List_List* _temp2808= _temp2760; if( _temp2808 == 0){ _throw(
Null_Exception);} _temp2808->hd;}); int _temp2817; _LL2811: if( _temp2809 ==(
void*) Cyc_Absyn_Packed_att){ goto _LL2812;} else{ goto _LL2813;} _LL2813: if((
unsigned int) _temp2809 > 15u?*(( int*) _temp2809) == Cyc_Absyn_Aligned_att: 0){
_LL2818: _temp2817=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp2809)->f1;
goto _LL2814;} else{ goto _LL2815;} _LL2815: goto _LL2816; _LL2812: continue;
_LL2814: continue; _LL2816: Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2820= Cyc_Absyn_attribute2string(( void*)({ struct Cyc_List_List* _temp2819=
_temp2760; if( _temp2819 == 0){ _throw( Null_Exception);} _temp2819->hd;}));
struct _tagged_string _temp2821=* _temp2768; xprintf("bad attribute %.*s on struct member %.*s",
_temp2820.last_plus_one - _temp2820.curr, _temp2820.curr, _temp2821.last_plus_one
- _temp2821.curr, _temp2821.curr);})); goto _LL2810; _LL2810:;}}}; _pop_region(&
_temp2754);} goto _LL2400; _LL2430:{ struct _RegionHandle _temp2822= _new_region();
struct _RegionHandle* uprev_rgn=& _temp2822; _push_region( uprev_rgn);{ struct
Cyc_List_List* prev_fields= 0; for( 0; _temp2516 != 0; _temp2516=({ struct Cyc_List_List*
_temp2823= _temp2516; if( _temp2823 == 0){ _throw( Null_Exception);} _temp2823->tl;})){
struct Cyc_Absyn_Structfield _temp2827; struct Cyc_List_List* _temp2828; struct
Cyc_Core_Opt* _temp2830; void* _temp2832; struct Cyc_Absyn_Tqual _temp2834;
struct _tagged_string* _temp2836; struct Cyc_Absyn_Structfield* _temp2825=(
struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp2824= _temp2516; if(
_temp2824 == 0){ _throw( Null_Exception);} _temp2824->hd;}); _temp2827=*
_temp2825; _LL2837: _temp2836=( struct _tagged_string*) _temp2827.name; goto
_LL2835; _LL2835: _temp2834=( struct Cyc_Absyn_Tqual) _temp2827.tq; goto _LL2833;
_LL2833: _temp2832=( void*) _temp2827.type; goto _LL2831; _LL2831: _temp2830=(
struct Cyc_Core_Opt*) _temp2827.width; goto _LL2829; _LL2829: _temp2828=( struct
Cyc_List_List*) _temp2827.attributes; goto _LL2826; _LL2826: if((( int(*)( int(*
compare)( struct _tagged_string*, struct _tagged_string*), struct Cyc_List_List*
l, struct _tagged_string* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp, prev_fields,
_temp2836)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2838=* _temp2836;
xprintf("duplicate field %.*s in struct", _temp2838.last_plus_one - _temp2838.curr,
_temp2838.curr);}));} if( Cyc_String_strcmp(* _temp2836,( struct _tagged_string)({
char* _temp2839=( char*)""; struct _tagged_string _temp2840; _temp2840.curr=
_temp2839; _temp2840.base= _temp2839; _temp2840.last_plus_one= _temp2839 + 1;
_temp2840;})) != 0){ prev_fields=({ struct Cyc_List_List* _temp2841=( struct Cyc_List_List*)
_region_malloc( uprev_rgn, sizeof( struct Cyc_List_List)); _temp2841->hd=( void*)
_temp2836; _temp2841->tl= prev_fields; _temp2841;});} Cyc_Tcutil_check_valid_type(
loc, te, free_vars,( void*) Cyc_Absyn_MemKind, _temp2832); if( ! Cyc_Tcutil_bits_only(
_temp2832)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2842=* _temp2836;
xprintf("%.*s has a type that is possibly incompatible with other members of union",
_temp2842.last_plus_one - _temp2842.curr, _temp2842.curr);}));} if( _temp2830 !=
0){ unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)({
struct Cyc_Core_Opt* _temp2843= _temp2830; if( _temp2843 == 0){ _throw(
Null_Exception);} _temp2843->v;}))){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2844=* _temp2836; xprintf("bitfield %.*s does not have constant width",
_temp2844.last_plus_one - _temp2844.curr, _temp2844.curr);}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp2845= _temp2830; if(
_temp2845 == 0){ _throw( Null_Exception);} _temp2845->v;}));}{ void* _temp2846=
Cyc_Tcutil_compress( _temp2832); void* _temp2852; void* _temp2854; _LL2848: if((
unsigned int) _temp2846 > 4u?*(( int*) _temp2846) == Cyc_Absyn_IntType: 0){
_LL2855: _temp2854=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2846)->f1;
goto _LL2853; _LL2853: _temp2852=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2846)->f2; goto _LL2849;} else{ goto _LL2850;} _LL2850: goto _LL2851;
_LL2849:{ void* _temp2856= _temp2852; _LL2858: if( _temp2856 ==( void*) Cyc_Absyn_B1){
goto _LL2859;} else{ goto _LL2860;} _LL2860: if( _temp2856 ==( void*) Cyc_Absyn_B2){
goto _LL2861;} else{ goto _LL2862;} _LL2862: if( _temp2856 ==( void*) Cyc_Absyn_B4){
goto _LL2863;} else{ goto _LL2864;} _LL2864: if( _temp2856 ==( void*) Cyc_Absyn_B8){
goto _LL2865;} else{ goto _LL2857;} _LL2859: if( w > 8){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp2866=( char*)"bitfield larger than type";
struct _tagged_string _temp2867; _temp2867.curr= _temp2866; _temp2867.base=
_temp2866; _temp2867.last_plus_one= _temp2866 + 26; _temp2867;}));} goto _LL2857;
_LL2861: if( w > 16){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp2868=( char*)"bitfield larger than type"; struct _tagged_string _temp2869;
_temp2869.curr= _temp2868; _temp2869.base= _temp2868; _temp2869.last_plus_one=
_temp2868 + 26; _temp2869;}));} goto _LL2857; _LL2863: if( w > 32){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp2870=( char*)"bitfield larger than type";
struct _tagged_string _temp2871; _temp2871.curr= _temp2870; _temp2871.base=
_temp2870; _temp2871.last_plus_one= _temp2870 + 26; _temp2871;}));} goto _LL2857;
_LL2865: if( w > 64){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp2872=( char*)"bitfield larger than type"; struct _tagged_string _temp2873;
_temp2873.curr= _temp2872; _temp2873.base= _temp2872; _temp2873.last_plus_one=
_temp2872 + 26; _temp2873;}));} goto _LL2857; _LL2857:;} goto _LL2847; _LL2851:
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2874=* _temp2836; struct
_tagged_string _temp2875= Cyc_Absynpp_typ2string( _temp2832); xprintf("bitfield %.*s must have integral type but has type %.*s",
_temp2874.last_plus_one - _temp2874.curr, _temp2874.curr, _temp2875.last_plus_one
- _temp2875.curr, _temp2875.curr);})); goto _LL2847; _LL2847:;}} for( 0;
_temp2828 != 0; _temp2828=({ struct Cyc_List_List* _temp2876= _temp2828; if(
_temp2876 == 0){ _throw( Null_Exception);} _temp2876->tl;})){ void* _temp2878=(
void*)({ struct Cyc_List_List* _temp2877= _temp2828; if( _temp2877 == 0){ _throw(
Null_Exception);} _temp2877->hd;}); int _temp2886; _LL2880: if( _temp2878 ==(
void*) Cyc_Absyn_Packed_att){ goto _LL2881;} else{ goto _LL2882;} _LL2882: if((
unsigned int) _temp2878 > 15u?*(( int*) _temp2878) == Cyc_Absyn_Aligned_att: 0){
_LL2887: _temp2886=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp2878)->f1;
goto _LL2883;} else{ goto _LL2884;} _LL2884: goto _LL2885; _LL2881: continue;
_LL2883: continue; _LL2885: Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2889= Cyc_Absyn_attribute2string(( void*)({ struct Cyc_List_List* _temp2888=
_temp2828; if( _temp2888 == 0){ _throw( Null_Exception);} _temp2888->hd;}));
struct _tagged_string _temp2890=* _temp2836; xprintf("bad attribute %.*s on struct member %.*s",
_temp2889.last_plus_one - _temp2889.curr, _temp2889.curr, _temp2890.last_plus_one
- _temp2890.curr, _temp2890.curr);})); goto _LL2879; _LL2879:;}}}; _pop_region(&
_temp2822);} goto _LL2400; _LL2432: if( _temp2523 == 0){ if(* _temp2520 == 0){
Cyc_Tcutil_terr( loc, xprintf("bad anonymous struct")); return free_vars;}{
struct Cyc_Absyn_Structdecl* sd=({ struct Cyc_Absyn_Structdecl** _temp2891=*
_temp2520; if( _temp2891 == 0){ _throw( Null_Exception);}* _temp2891;});;}} if(*
_temp2520 == 0){ struct _handler_cons _temp2892; _push_handler(& _temp2892);{
int _temp2894= 0; if( setjmp( _temp2892.handler)){ _temp2894= 1;} if( !
_temp2894){* _temp2520=( struct Cyc_Absyn_Structdecl**) Cyc_Tcenv_lookup_structdecl(
te, loc,( struct _tuple1*)({ struct _tuple1* _temp2895= _temp2523; if( _temp2895
== 0){ _throw( Null_Exception);} _temp2895;}));; _pop_handler();} else{ void*
_temp2893=( void*) _exn_thrown; void* _temp2897= _temp2893; _LL2899: if(
_temp2897 == Cyc_Dict_Absent){ goto _LL2900;} else{ goto _LL2901;} _LL2901: goto
_LL2902; _LL2900: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2904= Cyc_Absynpp_qvar2string((
struct _tuple1*)({ struct _tuple1* _temp2903= _temp2523; if( _temp2903 == 0){
_throw( Null_Exception);} _temp2903;})); xprintf("unbound type struct %.*s",
_temp2904.last_plus_one - _temp2904.curr, _temp2904.curr);})); return free_vars;
_LL2902:( void) _throw( _temp2897); _LL2898:;}}}{ struct Cyc_Absyn_Structdecl*
sd=({ struct Cyc_Absyn_Structdecl** _temp2917=* _temp2520; if( _temp2917 == 0){
_throw( Null_Exception);}* _temp2917;}); if( sd->name != 0){({ struct _tuple1*
_temp2905= _temp2523; if( _temp2905 == 0){ _throw( Null_Exception);}* _temp2905=({
struct _tuple1* _temp2907=( struct _tuple1*)({ struct Cyc_Core_Opt* _temp2906=
sd->name; if( _temp2906 == 0){ _throw( Null_Exception);} _temp2906->v;});
unsigned int _temp2908= 0; if( _temp2908 >= 1u){ _throw( Null_Exception);}
_temp2907[ _temp2908];});});}{ struct Cyc_List_List* tvs= sd->tvs; int lvs=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs); int largs=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp2521); if( lvs != largs){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2910= Cyc_Absynpp_qvar2string(( struct _tuple1*)({
struct _tuple1* _temp2909= _temp2523; if( _temp2909 == 0){ _throw(
Null_Exception);} _temp2909;})); int _temp2911= lvs; int _temp2912= largs;
xprintf("struct %.*s expects %d type arguments but was given %d", _temp2910.last_plus_one
- _temp2910.curr, _temp2910.curr, _temp2911, _temp2912);}));} for( 0; _temp2521
!= 0; _temp2521=({ struct Cyc_List_List* _temp2913= _temp2521; if( _temp2913 ==
0){ _throw( Null_Exception);} _temp2913->tl;}), tvs=({ struct Cyc_List_List*
_temp2914= tvs; if( _temp2914 == 0){ _throw( Null_Exception);} _temp2914->tl;})){
void* t1=( void*)({ struct Cyc_List_List* _temp2916= _temp2521; if( _temp2916 ==
0){ _throw( Null_Exception);} _temp2916->hd;}); void* k1=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp2915= tvs; if( _temp2915 == 0){ _throw( Null_Exception);} _temp2915->hd;}))->kind);
free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars, k1, t1);} goto
_LL2400;}} _LL2434: if( _temp2530 == 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp2918=( char*)"missing union name"; struct
_tagged_string _temp2919; _temp2919.curr= _temp2918; _temp2919.base= _temp2918;
_temp2919.last_plus_one= _temp2918 + 19; _temp2919;})); return free_vars;} if(*
_temp2527 == 0){ struct _handler_cons _temp2920; _push_handler(& _temp2920);{
int _temp2922= 0; if( setjmp( _temp2920.handler)){ _temp2922= 1;} if( !
_temp2922){* _temp2527=( struct Cyc_Absyn_Uniondecl**) Cyc_Tcenv_lookup_uniondecl(
te, loc,( struct _tuple1*)({ struct _tuple1* _temp2923= _temp2530; if( _temp2923
== 0){ _throw( Null_Exception);} _temp2923;}));; _pop_handler();} else{ void*
_temp2921=( void*) _exn_thrown; void* _temp2925= _temp2921; _LL2927: if(
_temp2925 == Cyc_Dict_Absent){ goto _LL2928;} else{ goto _LL2929;} _LL2929: goto
_LL2930; _LL2928: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2932= Cyc_Absynpp_qvar2string((
struct _tuple1*)({ struct _tuple1* _temp2931= _temp2530; if( _temp2931 == 0){
_throw( Null_Exception);} _temp2931;})); xprintf("unbound type union %.*s",
_temp2932.last_plus_one - _temp2932.curr, _temp2932.curr);})); return free_vars;
_LL2930:( void) _throw( _temp2925); _LL2926:;}}}{ struct Cyc_Absyn_Uniondecl* ud=({
struct Cyc_Absyn_Uniondecl** _temp2945=* _temp2527; if( _temp2945 == 0){ _throw(
Null_Exception);}* _temp2945;}); if( ud->name != 0){({ struct _tuple1* _temp2933=
_temp2530; if( _temp2933 == 0){ _throw( Null_Exception);}* _temp2933=({ struct
_tuple1* _temp2935=( struct _tuple1*)({ struct Cyc_Core_Opt* _temp2934= ud->name;
if( _temp2934 == 0){ _throw( Null_Exception);} _temp2934->v;}); unsigned int
_temp2936= 0; if( _temp2936 >= 1u){ _throw( Null_Exception);} _temp2935[
_temp2936];});});}{ struct Cyc_List_List* tvs= ud->tvs; int lvs=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( tvs); int largs=(( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)( _temp2528); if( lvs != largs){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2938= Cyc_Absynpp_qvar2string(( struct _tuple1*)({
struct _tuple1* _temp2937= _temp2530; if( _temp2937 == 0){ _throw(
Null_Exception);} _temp2937;})); int _temp2939= lvs; int _temp2940= largs;
xprintf("union %.*s expects %d type arguments but was given %d", _temp2938.last_plus_one
- _temp2938.curr, _temp2938.curr, _temp2939, _temp2940);}));} for( 0; _temp2528
!= 0; _temp2528=({ struct Cyc_List_List* _temp2941= _temp2528; if( _temp2941 ==
0){ _throw( Null_Exception);} _temp2941->tl;}), tvs=({ struct Cyc_List_List*
_temp2942= tvs; if( _temp2942 == 0){ _throw( Null_Exception);} _temp2942->tl;})){
void* t1=( void*)({ struct Cyc_List_List* _temp2944= _temp2528; if( _temp2944 ==
0){ _throw( Null_Exception);} _temp2944->hd;}); void* k1=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp2943= tvs; if( _temp2943 == 0){ _throw( Null_Exception);} _temp2943->hd;}))->kind);
free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars, k1, t1);} goto
_LL2400;}} _LL2436: { struct Cyc_Absyn_Typedefdecl* td;{ struct _handler_cons
_temp2946; _push_handler(& _temp2946);{ int _temp2948= 0; if( setjmp( _temp2946.handler)){
_temp2948= 1;} if( ! _temp2948){ td= Cyc_Tcenv_lookup_typedefdecl( te, loc,
_temp2537);; _pop_handler();} else{ void* _temp2947=( void*) _exn_thrown; void*
_temp2950= _temp2947; _LL2952: if( _temp2950 == Cyc_Dict_Absent){ goto _LL2953;}
else{ goto _LL2954;} _LL2954: goto _LL2955; _LL2953: Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2956= Cyc_Absynpp_qvar2string( _temp2537); xprintf("unbound typedef name %.*s",
_temp2956.last_plus_one - _temp2956.curr, _temp2956.curr);})); return free_vars;
_LL2955:( void) _throw( _temp2950); _LL2951:;}}}({ struct _tuple1* _temp2957=
_temp2537; unsigned int _temp2958= 0; if( _temp2958 >= 1u){ _throw(
Null_Exception);} _temp2957[ _temp2958]=({ struct _tuple1* _temp2959= td->name;
unsigned int _temp2960= 0; if( _temp2960 >= 1u){ _throw( Null_Exception);}
_temp2959[ _temp2960];});});{ struct Cyc_List_List* tvs= td->tvs; struct Cyc_List_List*
ts= _temp2535; struct Cyc_List_List* inst= 0; for( 0; ts != 0? tvs != 0: 0; ts=({
struct Cyc_List_List* _temp2961= ts; if( _temp2961 == 0){ _throw( Null_Exception);}
_temp2961->tl;}), tvs=({ struct Cyc_List_List* _temp2962= tvs; if( _temp2962 ==
0){ _throw( Null_Exception);} _temp2962->tl;})){ void* k=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp2968= tvs; if( _temp2968 == 0){ _throw( Null_Exception);} _temp2968->hd;}))->kind);
free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars, k,( void*)({ struct
Cyc_List_List* _temp2963= ts; if( _temp2963 == 0){ _throw( Null_Exception);}
_temp2963->hd;})); inst=({ struct Cyc_List_List* _temp2964=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2964->hd=( void*)({ struct
_tuple5* _temp2965=( struct _tuple5*) GC_malloc( sizeof( struct _tuple5));
_temp2965->f1=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp2967= tvs;
if( _temp2967 == 0){ _throw( Null_Exception);} _temp2967->hd;}); _temp2965->f2=(
void*)({ struct Cyc_List_List* _temp2966= ts; if( _temp2966 == 0){ _throw(
Null_Exception);} _temp2966->hd;}); _temp2965;}); _temp2964->tl= inst; _temp2964;});}
if( tvs != 0? 1: ts != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2969= Cyc_Absynpp_qvar2string( _temp2537); xprintf("typedef %.*s expects a different number of arguments",
_temp2969.last_plus_one - _temp2969.curr, _temp2969.curr);})); return free_vars;}{
void* new_typ= Cyc_Tcutil_substitute( inst,( void*) td->defn);* _temp2534=({
struct Cyc_Core_Opt* _temp2970=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2970->v=( void*) new_typ; _temp2970;}); goto _LL2400;}}}
_LL2438: goto _LL2400; _LL2440: _temp2541= _temp2539; goto _LL2442; _LL2442:
free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars,( void*) Cyc_Absyn_RgnKind,
_temp2541); goto _LL2400; _LL2444: for( 0; _temp2543 != 0; _temp2543=({ struct
Cyc_List_List* _temp2971= _temp2543; if( _temp2971 == 0){ _throw( Null_Exception);}
_temp2971->tl;})){ free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars,(
void*) Cyc_Absyn_EffKind,( void*)({ struct Cyc_List_List* _temp2972= _temp2543;
if( _temp2972 == 0){ _throw( Null_Exception);} _temp2972->hd;}));} goto _LL2400;
_LL2400:;} if( ! Cyc_Tcutil_kind_leq( Cyc_Tcutil_typ_kind( t), expected_kind)){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2973= Cyc_Absynpp_typ2string(
t); struct _tagged_string _temp2974= Cyc_Absynpp_kind2string( Cyc_Tcutil_typ_kind(
t)); struct _tagged_string _temp2975= Cyc_Absynpp_kind2string( expected_kind);
xprintf("type %.*s has kind %.*s but as used here needs kind %.*s", _temp2973.last_plus_one
- _temp2973.curr, _temp2973.curr, _temp2974.last_plus_one - _temp2974.curr,
_temp2974.curr, _temp2975.last_plus_one - _temp2975.curr, _temp2975.curr);}));}
return free_vars;} void Cyc_Tcutil_check_valid_toplevel_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, void* t){ struct Cyc_List_List* free_tvars= Cyc_Tcutil_check_valid_type(
loc, te, 0,( void*) Cyc_Absyn_MemKind, t);{ struct Cyc_List_List* x= free_tvars;
for( 0; x != 0; x=({ struct Cyc_List_List* _temp2976= x; if( _temp2976 == 0){
_throw( Null_Exception);} _temp2976->tl;})){ struct Cyc_Absyn_Conref* c=((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp2989= x; if( _temp2989 == 0){
_throw( Null_Exception);} _temp2989->hd;}))->kind); void* _temp2977=( void*) c->v;
void* _temp2985; _LL2979: if( _temp2977 ==( void*) Cyc_Absyn_No_constr){ goto
_LL2980;} else{ goto _LL2981;} _LL2981: if(( unsigned int) _temp2977 > 1u?*((
int*) _temp2977) == Cyc_Absyn_Eq_constr: 0){ _LL2986: _temp2985=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp2977)->f1; if( _temp2985 ==( void*) Cyc_Absyn_MemKind){
goto _LL2982;} else{ goto _LL2983;}} else{ goto _LL2983;} _LL2983: goto _LL2984;
_LL2980: goto _LL2982; _LL2982:( void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp2987=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2987[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp2988; _temp2988.tag= Cyc_Absyn_Eq_constr;
_temp2988.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp2988;}); _temp2987;})));
goto _LL2978; _LL2984: goto _LL2978; _LL2978:;}} if( free_tvars != 0){ void*
_temp2990= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo _temp2996; struct
Cyc_List_List* _temp2998; int _temp3000; struct Cyc_List_List* _temp3002; void*
_temp3004; struct Cyc_Core_Opt* _temp3006; struct Cyc_List_List* _temp3008;
struct Cyc_List_List** _temp3010; _LL2992: if(( unsigned int) _temp2990 > 4u?*((
int*) _temp2990) == Cyc_Absyn_FnType: 0){ _LL2997: _temp2996=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp2990)->f1; _LL3009: _temp3008=( struct Cyc_List_List*)
_temp2996.tvars; _temp3010=&((( struct Cyc_Absyn_FnType_struct*) _temp2990)->f1).tvars;
goto _LL3007; _LL3007: _temp3006=( struct Cyc_Core_Opt*) _temp2996.effect; goto
_LL3005; _LL3005: _temp3004=( void*) _temp2996.ret_typ; goto _LL3003; _LL3003:
_temp3002=( struct Cyc_List_List*) _temp2996.args; goto _LL3001; _LL3001:
_temp3000=( int) _temp2996.varargs; goto _LL2999; _LL2999: _temp2998=( struct
Cyc_List_List*) _temp2996.attributes; goto _LL2993;} else{ goto _LL2994;}
_LL2994: goto _LL2995; _LL2993: if(* _temp3010 == 0){* _temp3010= free_tvars;}
else{ if( free_tvars != 0){ struct _tagged_string s=*(( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp3015= free_tvars; if( _temp3015 == 0){ _throw(
Null_Exception);} _temp3015->hd;}))->name; if(({ struct _tagged_string _temp3011=
s; char* _temp3013= _temp3011.curr + 0; if( _temp3011.base == 0? 1:( _temp3013 <
_temp3011.base? 1: _temp3013 >= _temp3011.last_plus_one)){ _throw(
Null_Exception);}* _temp3013;}) =='?'){ Cyc_Tcutil_terr( loc, xprintf("You failed to abstract over the generated type variables. \nRemove the bound type variables or else make the effects of the type explicit\nand then abstract the free type variables properly."));}
else{ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp3014= s; xprintf("unbound type variable %.*s",
_temp3014.last_plus_one - _temp3014.curr, _temp3014.curr);}));}}} goto _LL2991;
_LL2995: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp3017=*(( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp3016= free_tvars; if( _temp3016 == 0){ _throw(
Null_Exception);} _temp3016->hd;}))->name; xprintf("unbound type variable %.*s",
_temp3017.last_plus_one - _temp3017.curr, _temp3017.curr);})); return; _LL2991:;}}
void Cyc_Tcutil_check_type( struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* bound_tvars, void* expected_kind, void* t){ struct Cyc_List_List*
free_tvars= Cyc_Tcutil_check_valid_type( loc, te, 0, expected_kind, t); struct
Cyc_List_List* fs= free_tvars; for( 0; fs != 0; fs=({ struct Cyc_List_List*
_temp3018= fs; if( _temp3018 == 0){ _throw( Null_Exception);} _temp3018->tl;})){
struct Cyc_Absyn_Tvar* ftv=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp3034= fs; if( _temp3034 == 0){ _throw( Null_Exception);} _temp3034->hd;});
struct _tagged_string* _temp3019= ftv->name; goto _LL3020; _LL3020: { struct Cyc_Absyn_Conref*
_temp3021= ftv->kind; goto _LL3022; _LL3022: { int present= 0;{ struct Cyc_List_List*
bs= bound_tvars; for( 0; bs != 0; bs=({ struct Cyc_List_List* _temp3023= bs; if(
_temp3023 == 0){ _throw( Null_Exception);} _temp3023->tl;})){ struct Cyc_Absyn_Tvar*
btv=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp3031= bs; if(
_temp3031 == 0){ _throw( Null_Exception);} _temp3031->hd;}); struct
_tagged_string* _temp3024= btv->name; goto _LL3025; _LL3025: { struct Cyc_Absyn_Conref*
_temp3026= btv->kind; goto _LL3027; _LL3027: if( Cyc_String_zstrptrcmp(
_temp3019, _temp3024) == 0){ present= 1; if( ! Cyc_Tcutil_constrain_kinds(
_temp3026, _temp3021)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp3028=*
_temp3019; struct _tagged_string _temp3029= Cyc_Absynpp_ckind2string( _temp3026);
struct _tagged_string _temp3030= Cyc_Absynpp_ckind2string( _temp3021); xprintf("type variable %.*s has kind %.*s but is used here atkind %.*s",
_temp3028.last_plus_one - _temp3028.curr, _temp3028.curr, _temp3029.last_plus_one
- _temp3029.curr, _temp3029.curr, _temp3030.last_plus_one - _temp3030.curr,
_temp3030.curr);}));} break;}}}} if( ! present){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp3032=* _temp3019; struct _tagged_string _temp3033= Cyc_Absynpp_typ2string(
t); xprintf("unbound type variable %.*s in type %.*s", _temp3032.last_plus_one -
_temp3032.curr, _temp3032.curr, _temp3033.last_plus_one - _temp3033.curr,
_temp3033.curr);}));}}}}} static void Cyc_Tcutil_check_unique_unsorted( int(*
cmp)( void*, void*), struct Cyc_List_List* vs, struct Cyc_Position_Segment* loc,
struct _tagged_string(* a2string)( void*), struct _tagged_string msg){ for( 0;
vs != 0; vs=({ struct Cyc_List_List* _temp3035= vs; if( _temp3035 == 0){ _throw(
Null_Exception);} _temp3035->tl;})){ struct Cyc_List_List* vs2=({ struct Cyc_List_List*
_temp3042= vs; if( _temp3042 == 0){ _throw( Null_Exception);} _temp3042->tl;});
for( 0; vs2 != 0; vs2=({ struct Cyc_List_List* _temp3036= vs2; if( _temp3036 ==
0){ _throw( Null_Exception);} _temp3036->tl;})){ if( cmp(( void*)({ struct Cyc_List_List*
_temp3037= vs; if( _temp3037 == 0){ _throw( Null_Exception);} _temp3037->hd;}),(
void*)({ struct Cyc_List_List* _temp3038= vs2; if( _temp3038 == 0){ _throw(
Null_Exception);} _temp3038->hd;})) == 0){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp3040= msg; struct _tagged_string _temp3041= a2string(( void*)({
struct Cyc_List_List* _temp3039= vs; if( _temp3039 == 0){ _throw( Null_Exception);}
_temp3039->hd;})); xprintf("%.*s: %.*s", _temp3040.last_plus_one - _temp3040.curr,
_temp3040.curr, _temp3041.last_plus_one - _temp3041.curr, _temp3041.curr);}));}}}}
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
tvs, loc, Cyc_Tcutil_tvar2string,( struct _tagged_string)({ char* _temp3043=(
char*)"duplicate type variable"; struct _tagged_string _temp3044; _temp3044.curr=
_temp3043; _temp3044.base= _temp3043; _temp3044.last_plus_one= _temp3043 + 24;
_temp3044;}));} struct _tuple15{ struct Cyc_Absyn_Structfield* f1; int f2; } ;
struct _tuple16{ struct Cyc_List_List* f1; void* f2; } ; struct _tuple17{ struct
Cyc_Absyn_Structfield* f1; void* f2; } ; struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* sdfields){ struct Cyc_List_List* fields= 0;{ struct
Cyc_List_List* sd_fields= sdfields; for( 0; sd_fields != 0; sd_fields=({ struct
Cyc_List_List* _temp3045= sd_fields; if( _temp3045 == 0){ _throw( Null_Exception);}
_temp3045->tl;})){ if( Cyc_String_strcmp(*(( struct Cyc_Absyn_Structfield*)({
struct Cyc_List_List* _temp3046= sd_fields; if( _temp3046 == 0){ _throw(
Null_Exception);} _temp3046->hd;}))->name,( struct _tagged_string)({ char*
_temp3047=( char*)""; struct _tagged_string _temp3048; _temp3048.curr= _temp3047;
_temp3048.base= _temp3047; _temp3048.last_plus_one= _temp3047 + 1; _temp3048;}))
!= 0){ fields=({ struct Cyc_List_List* _temp3049=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3049->hd=( void*)({ struct
_tuple15* _temp3050=( struct _tuple15*) GC_malloc( sizeof( struct _tuple15));
_temp3050->f1=( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp3051=
sd_fields; if( _temp3051 == 0){ _throw( Null_Exception);} _temp3051->hd;});
_temp3050->f2= 0; _temp3050;}); _temp3049->tl= fields; _temp3049;});}}} fields=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( fields);{
struct Cyc_List_List* ans= 0; for( 0; des != 0; des=({ struct Cyc_List_List*
_temp3052= des; if( _temp3052 == 0){ _throw( Null_Exception);} _temp3052->tl;})){
struct _tuple16 _temp3056; void* _temp3057; struct Cyc_List_List* _temp3059;
struct _tuple16* _temp3054=( struct _tuple16*)({ struct Cyc_List_List* _temp3053=
des; if( _temp3053 == 0){ _throw( Null_Exception);} _temp3053->hd;}); _temp3056=*
_temp3054; _LL3060: _temp3059= _temp3056.f1; goto _LL3058; _LL3058: _temp3057=
_temp3056.f2; goto _LL3055; _LL3055: if( _temp3059 == 0){ struct Cyc_List_List*
_temp3061= fields; goto _LL3062; _LL3062: for( 0; _temp3061 != 0; _temp3061=({
struct Cyc_List_List* _temp3063= _temp3061; if( _temp3063 == 0){ _throw(
Null_Exception);} _temp3063->tl;})){ if( !(*(( struct _tuple15*)({ struct Cyc_List_List*
_temp3064= _temp3061; if( _temp3064 == 0){ _throw( Null_Exception);} _temp3064->hd;}))).f2){(*((
struct _tuple15*)({ struct Cyc_List_List* _temp3065= _temp3061; if( _temp3065 ==
0){ _throw( Null_Exception);} _temp3065->hd;}))).f2= 1;(*(( struct _tuple16*)({
struct Cyc_List_List* _temp3066= des; if( _temp3066 == 0){ _throw(
Null_Exception);} _temp3066->hd;}))).f1=({ struct Cyc_List_List* _temp3067=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3067->hd=(
void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp3068=( struct Cyc_Absyn_FieldName_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct)); _temp3068[ 0]=({ struct
Cyc_Absyn_FieldName_struct _temp3069; _temp3069.tag= Cyc_Absyn_FieldName;
_temp3069.f1=((*(( struct _tuple15*)({ struct Cyc_List_List* _temp3070=
_temp3061; if( _temp3070 == 0){ _throw( Null_Exception);} _temp3070->hd;}))).f1)->name;
_temp3069;}); _temp3068;})); _temp3067->tl= 0; _temp3067;}); ans=({ struct Cyc_List_List*
_temp3071=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp3071->hd=( void*)({ struct _tuple17* _temp3072=( struct _tuple17*)
_region_malloc( rgn, sizeof( struct _tuple17)); _temp3072->f1=(*(( struct
_tuple15*)({ struct Cyc_List_List* _temp3073= _temp3061; if( _temp3073 == 0){
_throw( Null_Exception);} _temp3073->hd;}))).f1; _temp3072->f2= _temp3057;
_temp3072;}); _temp3071->tl= ans; _temp3071;}); break;}} if( _temp3061 == 0){
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp3074=( char*)"too many arguments to struct";
struct _tagged_string _temp3075; _temp3075.curr= _temp3074; _temp3075.base=
_temp3074; _temp3075.last_plus_one= _temp3074 + 29; _temp3075;}));}} else{ if(({
struct Cyc_List_List* _temp3076= _temp3059; if( _temp3076 == 0){ _throw(
Null_Exception);} _temp3076->tl;}) != 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp3077=( char*)"multiple designators are not supported";
struct _tagged_string _temp3078; _temp3078.curr= _temp3077; _temp3078.base=
_temp3077; _temp3078.last_plus_one= _temp3077 + 39; _temp3078;}));} else{ void*
_temp3080=( void*)({ struct Cyc_List_List* _temp3079= _temp3059; if( _temp3079
== 0){ _throw( Null_Exception);} _temp3079->hd;}); struct Cyc_Absyn_Exp*
_temp3086; struct _tagged_string* _temp3088; _LL3082: if(*(( int*) _temp3080) ==
Cyc_Absyn_ArrayElement){ _LL3087: _temp3086=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayElement_struct*) _temp3080)->f1; goto _LL3083;} else{ goto
_LL3084;} _LL3084: if(*(( int*) _temp3080) == Cyc_Absyn_FieldName){ _LL3089:
_temp3088=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp3080)->f1; goto _LL3085;} else{ goto _LL3081;} _LL3083: Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp3090=( char*)"array designator used in argument to struct";
struct _tagged_string _temp3091; _temp3091.curr= _temp3090; _temp3091.base=
_temp3090; _temp3091.last_plus_one= _temp3090 + 44; _temp3091;})); goto _LL3081;
_LL3085: { struct Cyc_List_List* _temp3092= fields; goto _LL3093; _LL3093: for(
0; _temp3092 != 0; _temp3092=({ struct Cyc_List_List* _temp3094= _temp3092; if(
_temp3094 == 0){ _throw( Null_Exception);} _temp3094->tl;})){ if( Cyc_String_zstrptrcmp(
_temp3088,((*(( struct _tuple15*)({ struct Cyc_List_List* _temp3095= _temp3092;
if( _temp3095 == 0){ _throw( Null_Exception);} _temp3095->hd;}))).f1)->name) ==
0){ if((*(( struct _tuple15*)({ struct Cyc_List_List* _temp3096= _temp3092; if(
_temp3096 == 0){ _throw( Null_Exception);} _temp3096->hd;}))).f2){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp3097=* _temp3088; xprintf("field %.*s has already been used as an argument",
_temp3097.last_plus_one - _temp3097.curr, _temp3097.curr);}));}(*(( struct
_tuple15*)({ struct Cyc_List_List* _temp3098= _temp3092; if( _temp3098 == 0){
_throw( Null_Exception);} _temp3098->hd;}))).f2= 1; ans=({ struct Cyc_List_List*
_temp3099=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp3099->hd=( void*)({ struct _tuple17* _temp3100=( struct _tuple17*)
_region_malloc( rgn, sizeof( struct _tuple17)); _temp3100->f1=(*(( struct
_tuple15*)({ struct Cyc_List_List* _temp3101= _temp3092; if( _temp3101 == 0){
_throw( Null_Exception);} _temp3101->hd;}))).f1; _temp3100->f2= _temp3057;
_temp3100;}); _temp3099->tl= ans; _temp3099;}); break;}} if( _temp3092 == 0){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp3102=* _temp3088; xprintf("bad field designator %.*s",
_temp3102.last_plus_one - _temp3102.curr, _temp3102.curr);}));} goto _LL3081;}
_LL3081:;}}} for( 0; fields != 0; fields=({ struct Cyc_List_List* _temp3103=
fields; if( _temp3103 == 0){ _throw( Null_Exception);} _temp3103->tl;})){ if( !(*((
struct _tuple15*)({ struct Cyc_List_List* _temp3104= fields; if( _temp3104 == 0){
_throw( Null_Exception);} _temp3104->hd;}))).f2){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp3105=( char*)"too few arguments to struct"; struct
_tagged_string _temp3106; _temp3106.curr= _temp3105; _temp3106.base= _temp3105;
_temp3106.last_plus_one= _temp3105 + 28; _temp3106;})); break;}} return(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( ans);}} int Cyc_Tcutil_is_tagged_pointer_typ(
void* t){ void* _temp3107= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp3113; struct Cyc_Absyn_Conref* _temp3115; struct Cyc_Absyn_Tqual _temp3117;
struct Cyc_Absyn_Conref* _temp3119; void* _temp3121; void* _temp3123; _LL3109:
if(( unsigned int) _temp3107 > 4u?*(( int*) _temp3107) == Cyc_Absyn_PointerType:
0){ _LL3114: _temp3113=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp3107)->f1; _LL3124: _temp3123=( void*) _temp3113.elt_typ; goto _LL3122;
_LL3122: _temp3121=( void*) _temp3113.rgn_typ; goto _LL3120; _LL3120: _temp3119=(
struct Cyc_Absyn_Conref*) _temp3113.nullable; goto _LL3118; _LL3118: _temp3117=(
struct Cyc_Absyn_Tqual) _temp3113.tq; goto _LL3116; _LL3116: _temp3115=( struct
Cyc_Absyn_Conref*) _temp3113.bounds; goto _LL3110;} else{ goto _LL3111;} _LL3111:
goto _LL3112; _LL3110: { struct Cyc_Absyn_Conref* _temp3125=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp3115); goto
_LL3126; _LL3126: { void* _temp3127=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp3125))->v; void*
_temp3135; _LL3129: if(( unsigned int) _temp3127 > 1u?*(( int*) _temp3127) ==
Cyc_Absyn_Eq_constr: 0){ _LL3136: _temp3135=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3127)->f1; if( _temp3135 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL3130;}
else{ goto _LL3131;}} else{ goto _LL3131;} _LL3131: if( _temp3127 ==( void*) Cyc_Absyn_No_constr){
goto _LL3132;} else{ goto _LL3133;} _LL3133: goto _LL3134; _LL3130: return 1;
_LL3132:( void*)( _temp3125->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp3137=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp3137[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp3138; _temp3138.tag= Cyc_Absyn_Eq_constr;
_temp3138.f1=( void*)(( void*) Cyc_Absyn_Unknown_b); _temp3138;}); _temp3137;})));
return 1; _LL3134: return 0; _LL3128:;}} _LL3112: return 0; _LL3108:;} struct
_tuple6 Cyc_Tcutil_addressof_props( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e){ struct _tuple6 bogus_ans=({ struct _tuple6 _temp3397; _temp3397.f1= 0;
_temp3397.f2=( void*) Cyc_Absyn_HeapRgn; _temp3397;}); void* _temp3139=( void*)
e->r; void* _temp3153; struct _tuple1* _temp3155; struct _tagged_string*
_temp3157; struct Cyc_Absyn_Exp* _temp3159; struct _tagged_string* _temp3161;
struct Cyc_Absyn_Exp* _temp3163; struct Cyc_Absyn_Exp* _temp3165; struct Cyc_Absyn_Exp*
_temp3167; struct Cyc_Absyn_Exp* _temp3169; _LL3141: if(*(( int*) _temp3139) ==
Cyc_Absyn_Var_e){ _LL3156: _temp3155=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*)
_temp3139)->f1; goto _LL3154; _LL3154: _temp3153=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp3139)->f2; goto _LL3142;} else{ goto _LL3143;} _LL3143: if(*(( int*)
_temp3139) == Cyc_Absyn_StructMember_e){ _LL3160: _temp3159=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp3139)->f1; goto _LL3158; _LL3158:
_temp3157=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp3139)->f2; goto _LL3144;} else{ goto _LL3145;} _LL3145: if(*(( int*)
_temp3139) == Cyc_Absyn_StructArrow_e){ _LL3164: _temp3163=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp3139)->f1; goto _LL3162; _LL3162:
_temp3161=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp3139)->f2; goto _LL3146;} else{ goto _LL3147;} _LL3147: if(*(( int*)
_temp3139) == Cyc_Absyn_Deref_e){ _LL3166: _temp3165=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Deref_e_struct*) _temp3139)->f1; goto _LL3148;} else{ goto
_LL3149;} _LL3149: if(*(( int*) _temp3139) == Cyc_Absyn_Subscript_e){ _LL3170:
_temp3169=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp3139)->f1; goto _LL3168; _LL3168: _temp3167=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp3139)->f2; goto _LL3150;} else{ goto
_LL3151;} _LL3151: goto _LL3152; _LL3142: { void* _temp3171= _temp3153; struct
Cyc_Absyn_Fndecl* _temp3185; struct Cyc_Absyn_Vardecl* _temp3187; struct Cyc_Absyn_Vardecl*
_temp3189; struct Cyc_Absyn_Vardecl* _temp3191; struct Cyc_Absyn_Vardecl*
_temp3193; _LL3173: if( _temp3171 ==( void*) Cyc_Absyn_Unresolved_b){ goto
_LL3174;} else{ goto _LL3175;} _LL3175: if(( unsigned int) _temp3171 > 1u?*((
int*) _temp3171) == Cyc_Absyn_Funname_b: 0){ _LL3186: _temp3185=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Funname_b_struct*) _temp3171)->f1; goto _LL3176;} else{ goto
_LL3177;} _LL3177: if(( unsigned int) _temp3171 > 1u?*(( int*) _temp3171) == Cyc_Absyn_Global_b:
0){ _LL3188: _temp3187=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Global_b_struct*)
_temp3171)->f1; goto _LL3178;} else{ goto _LL3179;} _LL3179: if(( unsigned int)
_temp3171 > 1u?*(( int*) _temp3171) == Cyc_Absyn_Local_b: 0){ _LL3190: _temp3189=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Local_b_struct*) _temp3171)->f1;
goto _LL3180;} else{ goto _LL3181;} _LL3181: if(( unsigned int) _temp3171 > 1u?*((
int*) _temp3171) == Cyc_Absyn_Pat_b: 0){ _LL3192: _temp3191=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Pat_b_struct*) _temp3171)->f1; goto _LL3182;} else{ goto
_LL3183;} _LL3183: if(( unsigned int) _temp3171 > 1u?*(( int*) _temp3171) == Cyc_Absyn_Param_b:
0){ _LL3194: _temp3193=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Param_b_struct*)
_temp3171)->f1; goto _LL3184;} else{ goto _LL3172;} _LL3174: return bogus_ans;
_LL3176: return({ struct _tuple6 _temp3195; _temp3195.f1= 1; _temp3195.f2=( void*)
Cyc_Absyn_HeapRgn; _temp3195;}); _LL3178: { void* _temp3197= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp3196= e->topt; if( _temp3196 == 0){ _throw(
Null_Exception);} _temp3196->v;})); struct Cyc_Absyn_Exp* _temp3203; struct Cyc_Absyn_Tqual
_temp3205; void* _temp3207; _LL3199: if(( unsigned int) _temp3197 > 4u?*(( int*)
_temp3197) == Cyc_Absyn_ArrayType: 0){ _LL3208: _temp3207=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp3197)->f1; goto _LL3206; _LL3206: _temp3205=( struct Cyc_Absyn_Tqual)((
struct Cyc_Absyn_ArrayType_struct*) _temp3197)->f2; goto _LL3204; _LL3204:
_temp3203=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp3197)->f3; goto _LL3200;} else{ goto _LL3201;} _LL3201: goto _LL3202;
_LL3200: return({ struct _tuple6 _temp3209; _temp3209.f1= 1; _temp3209.f2=( void*)
Cyc_Absyn_HeapRgn; _temp3209;}); _LL3202: return({ struct _tuple6 _temp3210;
_temp3210.f1=( _temp3187->tq).q_const; _temp3210.f2=( void*) Cyc_Absyn_HeapRgn;
_temp3210;}); _LL3198:;} _LL3180: { void* _temp3212= Cyc_Tcutil_compress(( void*)({
struct Cyc_Core_Opt* _temp3211= e->topt; if( _temp3211 == 0){ _throw(
Null_Exception);} _temp3211->v;})); struct Cyc_Absyn_Exp* _temp3218; struct Cyc_Absyn_Tqual
_temp3220; void* _temp3222; _LL3214: if(( unsigned int) _temp3212 > 4u?*(( int*)
_temp3212) == Cyc_Absyn_ArrayType: 0){ _LL3223: _temp3222=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp3212)->f1; goto _LL3221; _LL3221: _temp3220=( struct Cyc_Absyn_Tqual)((
struct Cyc_Absyn_ArrayType_struct*) _temp3212)->f2; goto _LL3219; _LL3219:
_temp3218=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp3212)->f3; goto _LL3215;} else{ goto _LL3216;} _LL3216: goto _LL3217;
_LL3215: return({ struct _tuple6 _temp3224; _temp3224.f1= 1; _temp3224.f2=( void*)({
struct Cyc_Core_Opt* _temp3225= _temp3189->rgn; if( _temp3225 == 0){ _throw(
Null_Exception);} _temp3225->v;}); _temp3224;}); _LL3217: return({ struct
_tuple6 _temp3226; _temp3226.f1=( _temp3189->tq).q_const; _temp3226.f2=( void*)({
struct Cyc_Core_Opt* _temp3227= _temp3189->rgn; if( _temp3227 == 0){ _throw(
Null_Exception);} _temp3227->v;}); _temp3226;}); _LL3213:;} _LL3182: _temp3193=
_temp3191; goto _LL3184; _LL3184: return({ struct _tuple6 _temp3228; _temp3228.f1=(
_temp3193->tq).q_const; _temp3228.f2=( void*)({ struct Cyc_Core_Opt* _temp3229=
_temp3193->rgn; if( _temp3229 == 0){ _throw( Null_Exception);} _temp3229->v;});
_temp3228;}); _LL3172:;} _LL3144: { void* _temp3231= Cyc_Tcutil_compress(( void*)({
struct Cyc_Core_Opt* _temp3230= _temp3159->topt; if( _temp3230 == 0){ _throw(
Null_Exception);} _temp3230->v;})); struct Cyc_List_List* _temp3243; struct Cyc_List_List*
_temp3245; struct Cyc_Absyn_Structdecl** _temp3247; struct Cyc_Absyn_Structdecl*
_temp3249; struct Cyc_List_List* _temp3250; struct _tuple1* _temp3252; struct
Cyc_Absyn_Uniondecl** _temp3254; struct Cyc_Absyn_Uniondecl* _temp3256; struct
Cyc_List_List* _temp3257; struct _tuple1* _temp3259; _LL3233: if(( unsigned int)
_temp3231 > 4u?*(( int*) _temp3231) == Cyc_Absyn_AnonStructType: 0){ _LL3244:
_temp3243=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp3231)->f1; goto _LL3234;} else{ goto _LL3235;} _LL3235: if(( unsigned int)
_temp3231 > 4u?*(( int*) _temp3231) == Cyc_Absyn_AnonUnionType: 0){ _LL3246:
_temp3245=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp3231)->f1; goto _LL3236;} else{ goto _LL3237;} _LL3237: if(( unsigned int)
_temp3231 > 4u?*(( int*) _temp3231) == Cyc_Absyn_StructType: 0){ _LL3253:
_temp3252=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp3231)->f1;
goto _LL3251; _LL3251: _temp3250=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp3231)->f2; goto _LL3248; _LL3248: _temp3247=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp3231)->f3; if( _temp3247 == 0){ goto
_LL3239;} else{ _temp3249=* _temp3247; goto _LL3238;}} else{ goto _LL3239;}
_LL3239: if(( unsigned int) _temp3231 > 4u?*(( int*) _temp3231) == Cyc_Absyn_UnionType:
0){ _LL3260: _temp3259=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp3231)->f1; goto _LL3258; _LL3258: _temp3257=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnionType_struct*) _temp3231)->f2; goto _LL3255; _LL3255:
_temp3254=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp3231)->f3; if( _temp3254 == 0){ goto _LL3241;} else{ _temp3256=* _temp3254;
goto _LL3240;}} else{ goto _LL3241;} _LL3241: goto _LL3242; _LL3234: _temp3245=
_temp3243; goto _LL3236; _LL3236: { struct Cyc_Absyn_Structfield* _temp3261= Cyc_Absyn_lookup_field(
_temp3245, _temp3157); goto _LL3262; _LL3262: if( _temp3261 != 0?({ struct Cyc_Absyn_Structfield*
_temp3263= _temp3261; if( _temp3263 == 0){ _throw( Null_Exception);} _temp3263->width;})
!= 0: 0){ return({ struct _tuple6 _temp3264; _temp3264.f1=({ struct Cyc_Absyn_Structfield*
_temp3265= _temp3261; if( _temp3265 == 0){ _throw( Null_Exception);} _temp3265->tq;}).q_const;
_temp3264.f2=( Cyc_Tcutil_addressof_props( te, _temp3159)).f2; _temp3264;});}
return bogus_ans;} _LL3238: { struct Cyc_Absyn_Structfield* _temp3266= Cyc_Absyn_lookup_struct_field(
_temp3249, _temp3157); goto _LL3267; _LL3267: if( _temp3266 != 0?({ struct Cyc_Absyn_Structfield*
_temp3268= _temp3266; if( _temp3268 == 0){ _throw( Null_Exception);} _temp3268->width;})
!= 0: 0){ return({ struct _tuple6 _temp3269; _temp3269.f1=({ struct Cyc_Absyn_Structfield*
_temp3270= _temp3266; if( _temp3270 == 0){ _throw( Null_Exception);} _temp3270->tq;}).q_const;
_temp3269.f2=( Cyc_Tcutil_addressof_props( te, _temp3159)).f2; _temp3269;});}
return bogus_ans;} _LL3240: { struct Cyc_Absyn_Structfield* _temp3271= Cyc_Absyn_lookup_union_field(
_temp3256, _temp3157); goto _LL3272; _LL3272: if( _temp3271 != 0){ return({
struct _tuple6 _temp3273; _temp3273.f1=({ struct Cyc_Absyn_Structfield*
_temp3274= _temp3271; if( _temp3274 == 0){ _throw( Null_Exception);} _temp3274->tq;}).q_const;
_temp3273.f2=( Cyc_Tcutil_addressof_props( te, _temp3159)).f2; _temp3273;});}
goto _LL3242;} _LL3242: return bogus_ans; _LL3232:;} _LL3146: { void* _temp3276=
Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp3275= _temp3163->topt;
if( _temp3275 == 0){ _throw( Null_Exception);} _temp3275->v;})); struct Cyc_Absyn_PtrInfo
_temp3282; struct Cyc_Absyn_Conref* _temp3284; struct Cyc_Absyn_Tqual _temp3286;
struct Cyc_Absyn_Conref* _temp3288; void* _temp3290; void* _temp3292; _LL3278:
if(( unsigned int) _temp3276 > 4u?*(( int*) _temp3276) == Cyc_Absyn_PointerType:
0){ _LL3283: _temp3282=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp3276)->f1; _LL3293: _temp3292=( void*) _temp3282.elt_typ; goto _LL3291;
_LL3291: _temp3290=( void*) _temp3282.rgn_typ; goto _LL3289; _LL3289: _temp3288=(
struct Cyc_Absyn_Conref*) _temp3282.nullable; goto _LL3287; _LL3287: _temp3286=(
struct Cyc_Absyn_Tqual) _temp3282.tq; goto _LL3285; _LL3285: _temp3284=( struct
Cyc_Absyn_Conref*) _temp3282.bounds; goto _LL3279;} else{ goto _LL3280;} _LL3280:
goto _LL3281; _LL3279: { void* _temp3294= Cyc_Tcutil_compress( _temp3292);
struct Cyc_List_List* _temp3306; struct Cyc_List_List* _temp3308; struct Cyc_Absyn_Structdecl**
_temp3310; struct Cyc_Absyn_Structdecl* _temp3312; struct Cyc_List_List*
_temp3313; struct _tuple1* _temp3315; struct Cyc_Absyn_Uniondecl** _temp3317;
struct Cyc_Absyn_Uniondecl* _temp3319; struct Cyc_List_List* _temp3320; struct
_tuple1* _temp3322; _LL3296: if(( unsigned int) _temp3294 > 4u?*(( int*)
_temp3294) == Cyc_Absyn_AnonStructType: 0){ _LL3307: _temp3306=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonStructType_struct*) _temp3294)->f1; goto _LL3297;} else{
goto _LL3298;} _LL3298: if(( unsigned int) _temp3294 > 4u?*(( int*) _temp3294)
== Cyc_Absyn_AnonUnionType: 0){ _LL3309: _temp3308=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonUnionType_struct*) _temp3294)->f1; goto _LL3299;} else{
goto _LL3300;} _LL3300: if(( unsigned int) _temp3294 > 4u?*(( int*) _temp3294)
== Cyc_Absyn_StructType: 0){ _LL3316: _temp3315=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp3294)->f1; goto _LL3314; _LL3314: _temp3313=( struct Cyc_List_List*)((
struct Cyc_Absyn_StructType_struct*) _temp3294)->f2; goto _LL3311; _LL3311:
_temp3310=( struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*)
_temp3294)->f3; if( _temp3310 == 0){ goto _LL3302;} else{ _temp3312=* _temp3310;
goto _LL3301;}} else{ goto _LL3302;} _LL3302: if(( unsigned int) _temp3294 > 4u?*((
int*) _temp3294) == Cyc_Absyn_UnionType: 0){ _LL3323: _temp3322=( struct _tuple1*)((
struct Cyc_Absyn_UnionType_struct*) _temp3294)->f1; goto _LL3321; _LL3321:
_temp3320=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp3294)->f2; goto _LL3318; _LL3318: _temp3317=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp3294)->f3; if( _temp3317 == 0){ goto
_LL3304;} else{ _temp3319=* _temp3317; goto _LL3303;}} else{ goto _LL3304;}
_LL3304: goto _LL3305; _LL3297: _temp3308= _temp3306; goto _LL3299; _LL3299: {
struct Cyc_Absyn_Structfield* _temp3324= Cyc_Absyn_lookup_field( _temp3308,
_temp3161); goto _LL3325; _LL3325: if( _temp3324 != 0?({ struct Cyc_Absyn_Structfield*
_temp3326= _temp3324; if( _temp3326 == 0){ _throw( Null_Exception);} _temp3326->width;})
!= 0: 0){ return({ struct _tuple6 _temp3327; _temp3327.f1=({ struct Cyc_Absyn_Structfield*
_temp3328= _temp3324; if( _temp3328 == 0){ _throw( Null_Exception);} _temp3328->tq;}).q_const;
_temp3327.f2= _temp3290; _temp3327;});} return bogus_ans;} _LL3301: { struct Cyc_Absyn_Structfield*
_temp3329= Cyc_Absyn_lookup_struct_field( _temp3312, _temp3161); goto _LL3330;
_LL3330: if( _temp3329 != 0?({ struct Cyc_Absyn_Structfield* _temp3331=
_temp3329; if( _temp3331 == 0){ _throw( Null_Exception);} _temp3331->width;}) !=
0: 0){ return({ struct _tuple6 _temp3332; _temp3332.f1=({ struct Cyc_Absyn_Structfield*
_temp3333= _temp3329; if( _temp3333 == 0){ _throw( Null_Exception);} _temp3333->tq;}).q_const;
_temp3332.f2= _temp3290; _temp3332;});} return bogus_ans;} _LL3303: { struct Cyc_Absyn_Structfield*
_temp3334= Cyc_Absyn_lookup_union_field( _temp3319, _temp3161); goto _LL3335;
_LL3335: if( _temp3334 != 0){ return({ struct _tuple6 _temp3336; _temp3336.f1=({
struct Cyc_Absyn_Structfield* _temp3337= _temp3334; if( _temp3337 == 0){ _throw(
Null_Exception);} _temp3337->tq;}).q_const; _temp3336.f2= _temp3290; _temp3336;});}
return bogus_ans;} _LL3305: return bogus_ans; _LL3295:;} _LL3281: return
bogus_ans; _LL3277:;} _LL3148: { void* _temp3339= Cyc_Tcutil_compress(( void*)({
struct Cyc_Core_Opt* _temp3338= _temp3165->topt; if( _temp3338 == 0){ _throw(
Null_Exception);} _temp3338->v;})); struct Cyc_Absyn_PtrInfo _temp3345; struct
Cyc_Absyn_Conref* _temp3347; struct Cyc_Absyn_Tqual _temp3349; struct Cyc_Absyn_Conref*
_temp3351; void* _temp3353; void* _temp3355; _LL3341: if(( unsigned int)
_temp3339 > 4u?*(( int*) _temp3339) == Cyc_Absyn_PointerType: 0){ _LL3346:
_temp3345=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp3339)->f1; _LL3356: _temp3355=( void*) _temp3345.elt_typ; goto _LL3354;
_LL3354: _temp3353=( void*) _temp3345.rgn_typ; goto _LL3352; _LL3352: _temp3351=(
struct Cyc_Absyn_Conref*) _temp3345.nullable; goto _LL3350; _LL3350: _temp3349=(
struct Cyc_Absyn_Tqual) _temp3345.tq; goto _LL3348; _LL3348: _temp3347=( struct
Cyc_Absyn_Conref*) _temp3345.bounds; goto _LL3342;} else{ goto _LL3343;} _LL3343:
goto _LL3344; _LL3342: return({ struct _tuple6 _temp3357; _temp3357.f1=
_temp3349.q_const; _temp3357.f2= _temp3353; _temp3357;}); _LL3344: return
bogus_ans; _LL3340:;} _LL3150: { void* t= Cyc_Tcutil_compress(( void*)({ struct
Cyc_Core_Opt* _temp3394= _temp3169->topt; if( _temp3394 == 0){ _throw(
Null_Exception);} _temp3394->v;})); void* _temp3358= t; struct Cyc_Absyn_Exp*
_temp3368; struct Cyc_Absyn_Tqual _temp3370; void* _temp3372; struct Cyc_List_List*
_temp3374; struct Cyc_Absyn_PtrInfo _temp3376; struct Cyc_Absyn_Conref*
_temp3378; struct Cyc_Absyn_Tqual _temp3380; struct Cyc_Absyn_Conref* _temp3382;
void* _temp3384; void* _temp3386; _LL3360: if(( unsigned int) _temp3358 > 4u?*((
int*) _temp3358) == Cyc_Absyn_ArrayType: 0){ _LL3373: _temp3372=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3358)->f1; goto _LL3371; _LL3371:
_temp3370=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp3358)->f2; goto _LL3369; _LL3369: _temp3368=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3358)->f3; goto _LL3361;} else{ goto
_LL3362;} _LL3362: if(( unsigned int) _temp3358 > 4u?*(( int*) _temp3358) == Cyc_Absyn_TupleType:
0){ _LL3375: _temp3374=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp3358)->f1; goto _LL3363;} else{ goto _LL3364;} _LL3364: if(( unsigned int)
_temp3358 > 4u?*(( int*) _temp3358) == Cyc_Absyn_PointerType: 0){ _LL3377:
_temp3376=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp3358)->f1; _LL3387: _temp3386=( void*) _temp3376.elt_typ; goto _LL3385;
_LL3385: _temp3384=( void*) _temp3376.rgn_typ; goto _LL3383; _LL3383: _temp3382=(
struct Cyc_Absyn_Conref*) _temp3376.nullable; goto _LL3381; _LL3381: _temp3380=(
struct Cyc_Absyn_Tqual) _temp3376.tq; goto _LL3379; _LL3379: _temp3378=( struct
Cyc_Absyn_Conref*) _temp3376.bounds; goto _LL3365;} else{ goto _LL3366;} _LL3366:
goto _LL3367; _LL3361: return({ struct _tuple6 _temp3388; _temp3388.f1=
_temp3370.q_const; _temp3388.f2=( Cyc_Tcutil_addressof_props( te, _temp3169)).f2;
_temp3388;}); _LL3363: { struct _tuple4* _temp3389= Cyc_Absyn_lookup_tuple_field(
_temp3374,( int) Cyc_Evexp_eval_const_uint_exp( _temp3167)); goto _LL3390;
_LL3390: if( _temp3389 != 0){ return({ struct _tuple6 _temp3391; _temp3391.f1=(({
struct _tuple4* _temp3392= _temp3389; if( _temp3392 == 0){ _throw(
Null_Exception);}* _temp3392;}).f1).q_const; _temp3391.f2=( Cyc_Tcutil_addressof_props(
te, _temp3169)).f2; _temp3391;});} return bogus_ans;} _LL3365: return({ struct
_tuple6 _temp3393; _temp3393.f1= _temp3380.q_const; _temp3393.f2= _temp3384;
_temp3393;}); _LL3367: return bogus_ans; _LL3359:;} _LL3152: Cyc_Tcutil_terr( e->loc,(
struct _tagged_string)({ char* _temp3395=( char*)"unary & applied to non-lvalue";
struct _tagged_string _temp3396; _temp3396.curr= _temp3395; _temp3396.base=
_temp3395; _temp3396.last_plus_one= _temp3395 + 30; _temp3396;})); return
bogus_ans; _LL3140:;} void* Cyc_Tcutil_array_to_ptr( struct Cyc_Tcenv_Tenv* te,
void* e_typ, struct Cyc_Absyn_Exp* e){ void* _temp3398= Cyc_Tcutil_compress(
e_typ); struct Cyc_Absyn_Exp* _temp3404; struct Cyc_Absyn_Tqual _temp3406; void*
_temp3408; _LL3400: if(( unsigned int) _temp3398 > 4u?*(( int*) _temp3398) ==
Cyc_Absyn_ArrayType: 0){ _LL3409: _temp3408=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp3398)->f1; goto _LL3407; _LL3407: _temp3406=( struct Cyc_Absyn_Tqual)((
struct Cyc_Absyn_ArrayType_struct*) _temp3398)->f2; goto _LL3405; _LL3405:
_temp3404=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp3398)->f3; goto _LL3401;} else{ goto _LL3402;} _LL3402: goto _LL3403;
_LL3401: { void* _temp3412; int _temp3414; struct _tuple6 _temp3410= Cyc_Tcutil_addressof_props(
te, e); _LL3415: _temp3414= _temp3410.f1; goto _LL3413; _LL3413: _temp3412=
_temp3410.f2; goto _LL3411; _LL3411: return Cyc_Absyn_atb_typ( _temp3408,
_temp3412, _temp3406,( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp3416=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp3416[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp3417; _temp3417.tag= Cyc_Absyn_Upper_b;
_temp3417.f1= e; _temp3417;}); _temp3416;}));} _LL3403: return e_typ; _LL3399:;}
void Cyc_Tcutil_check_bound( struct Cyc_Position_Segment* loc, unsigned int i,
struct Cyc_Absyn_Conref* b){ b=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( b);{ void* _temp3418=( void*) b->v; void*
_temp3426; void* _temp3428; struct Cyc_Absyn_Exp* _temp3430; _LL3420: if((
unsigned int) _temp3418 > 1u?*(( int*) _temp3418) == Cyc_Absyn_Eq_constr: 0){
_LL3427: _temp3426=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3418)->f1;
if( _temp3426 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL3421;} else{ goto _LL3422;}}
else{ goto _LL3422;} _LL3422: if(( unsigned int) _temp3418 > 1u?*(( int*)
_temp3418) == Cyc_Absyn_Eq_constr: 0){ _LL3429: _temp3428=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3418)->f1; if(( unsigned int) _temp3428 > 1u?*(( int*) _temp3428) == Cyc_Absyn_Upper_b:
0){ _LL3431: _temp3430=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp3428)->f1; goto _LL3423;} else{ goto _LL3424;}} else{ goto _LL3424;}
_LL3424: goto _LL3425; _LL3421: return; _LL3423: if( Cyc_Evexp_eval_const_uint_exp(
_temp3430) <= i){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp3432=( char*)"dereference is out of bounds"; struct _tagged_string
_temp3433; _temp3433.curr= _temp3432; _temp3433.base= _temp3432; _temp3433.last_plus_one=
_temp3432 + 29; _temp3433;}));} return; _LL3425:( void*)( b->v=( void*)(( void*)({
struct Cyc_Absyn_Eq_constr_struct* _temp3434=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp3434[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp3435; _temp3435.tag= Cyc_Absyn_Eq_constr;
_temp3435.f1=( void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp3436=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp3436[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp3437; _temp3437.tag= Cyc_Absyn_Upper_b;
_temp3437.f1= Cyc_Absyn_uint_exp( i + 1, 0); _temp3437;}); _temp3436;}));
_temp3435;}); _temp3434;}))); return; _LL3419:;}} void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Conref* b){ Cyc_Tcutil_check_bound(
loc, 0, b);} int Cyc_Tcutil_bits_only( void* t){ void* _temp3438= Cyc_Tcutil_compress(
t); void* _temp3464; void* _temp3466; struct Cyc_Absyn_Enumdecl* _temp3468;
struct _tuple1* _temp3470; struct Cyc_Absyn_Exp* _temp3472; struct Cyc_Absyn_Tqual
_temp3474; void* _temp3476; struct Cyc_List_List* _temp3478; struct Cyc_Absyn_Structdecl**
_temp3480; struct Cyc_List_List* _temp3482; struct _tuple1* _temp3484; struct
Cyc_Absyn_Uniondecl** _temp3486; struct Cyc_List_List* _temp3488; struct _tuple1*
_temp3490; struct Cyc_List_List* _temp3492; struct Cyc_List_List* _temp3494;
_LL3440: if( _temp3438 ==( void*) Cyc_Absyn_VoidType){ goto _LL3441;} else{ goto
_LL3442;} _LL3442: if(( unsigned int) _temp3438 > 4u?*(( int*) _temp3438) == Cyc_Absyn_IntType:
0){ _LL3467: _temp3466=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp3438)->f1;
goto _LL3465; _LL3465: _temp3464=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp3438)->f2; goto _LL3443;} else{ goto _LL3444;} _LL3444: if( _temp3438 ==(
void*) Cyc_Absyn_FloatType){ goto _LL3445;} else{ goto _LL3446;} _LL3446: if(
_temp3438 ==( void*) Cyc_Absyn_DoubleType){ goto _LL3447;} else{ goto _LL3448;}
_LL3448: if(( unsigned int) _temp3438 > 4u?*(( int*) _temp3438) == Cyc_Absyn_EnumType:
0){ _LL3471: _temp3470=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*)
_temp3438)->f1; goto _LL3469; _LL3469: _temp3468=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_EnumType_struct*) _temp3438)->f2; goto _LL3449;} else{ goto
_LL3450;} _LL3450: if(( unsigned int) _temp3438 > 4u?*(( int*) _temp3438) == Cyc_Absyn_ArrayType:
0){ _LL3477: _temp3476=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3438)->f1;
goto _LL3475; _LL3475: _temp3474=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp3438)->f2; goto _LL3473; _LL3473: _temp3472=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3438)->f3; goto _LL3451;} else{ goto
_LL3452;} _LL3452: if(( unsigned int) _temp3438 > 4u?*(( int*) _temp3438) == Cyc_Absyn_TupleType:
0){ _LL3479: _temp3478=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp3438)->f1; goto _LL3453;} else{ goto _LL3454;} _LL3454: if(( unsigned int)
_temp3438 > 4u?*(( int*) _temp3438) == Cyc_Absyn_StructType: 0){ _LL3485:
_temp3484=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp3438)->f1;
goto _LL3483; _LL3483: _temp3482=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp3438)->f2; goto _LL3481; _LL3481: _temp3480=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp3438)->f3; goto _LL3455;} else{ goto
_LL3456;} _LL3456: if(( unsigned int) _temp3438 > 4u?*(( int*) _temp3438) == Cyc_Absyn_UnionType:
0){ _LL3491: _temp3490=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp3438)->f1; goto _LL3489; _LL3489: _temp3488=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnionType_struct*) _temp3438)->f2; goto _LL3487; _LL3487:
_temp3486=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp3438)->f3; goto _LL3457;} else{ goto _LL3458;} _LL3458: if(( unsigned int)
_temp3438 > 4u?*(( int*) _temp3438) == Cyc_Absyn_AnonStructType: 0){ _LL3493:
_temp3492=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp3438)->f1; goto _LL3459;} else{ goto _LL3460;} _LL3460: if(( unsigned int)
_temp3438 > 4u?*(( int*) _temp3438) == Cyc_Absyn_AnonUnionType: 0){ _LL3495:
_temp3494=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp3438)->f1; goto _LL3461;} else{ goto _LL3462;} _LL3462: goto _LL3463;
_LL3441: return 1; _LL3443: return 1; _LL3445: return 1; _LL3447: return 1;
_LL3449: return 1; _LL3451: return Cyc_Tcutil_bits_only( _temp3476); _LL3453:
for( 0; _temp3478 != 0; _temp3478=({ struct Cyc_List_List* _temp3496= _temp3478;
if( _temp3496 == 0){ _throw( Null_Exception);} _temp3496->tl;})){ if( ! Cyc_Tcutil_bits_only((*((
struct _tuple4*)({ struct Cyc_List_List* _temp3497= _temp3478; if( _temp3497 ==
0){ _throw( Null_Exception);} _temp3497->hd;}))).f2)){ return 0;}} return 1;
_LL3455: if( _temp3480 == 0){ return 0;}{ struct Cyc_Absyn_Structdecl* _temp3499=({
struct Cyc_Absyn_Structdecl** _temp3498= _temp3480; if( _temp3498 == 0){ _throw(
Null_Exception);}* _temp3498;}); goto _LL3500; _LL3500: if( _temp3499->fields ==
0){ return 0;}{ struct _RegionHandle _temp3501= _new_region(); struct
_RegionHandle* rgn=& _temp3501; _push_region( rgn);{ struct Cyc_List_List*
_temp3502=(( struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct
_RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, _temp3499->tvs, _temp3482); goto _LL3503; _LL3503:{ struct Cyc_List_List*
fs=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp3507= _temp3499->fields;
if( _temp3507 == 0){ _throw( Null_Exception);} _temp3507->v;}); for( 0; fs != 0;
fs=({ struct Cyc_List_List* _temp3504= fs; if( _temp3504 == 0){ _throw(
Null_Exception);} _temp3504->tl;})){ if( ! Cyc_Tcutil_bits_only( Cyc_Tcutil_rsubstitute(
rgn, _temp3502,( void*)(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp3505= fs; if( _temp3505 == 0){ _throw( Null_Exception);} _temp3505->hd;}))->type))){
int _temp3506= 0; _npop_handler( 0u); return _temp3506;}}}{ int _temp3508= 1;
_npop_handler( 0u); return _temp3508;}}; _pop_region(& _temp3501);}} _LL3457:
if( _temp3486 == 0){ return 0;}{ struct Cyc_Absyn_Uniondecl* _temp3510=({ struct
Cyc_Absyn_Uniondecl** _temp3509= _temp3486; if( _temp3509 == 0){ _throw(
Null_Exception);}* _temp3509;}); goto _LL3511; _LL3511: if( _temp3510->fields ==
0){ return 0;}{ struct _RegionHandle _temp3512= _new_region(); struct
_RegionHandle* rgn=& _temp3512; _push_region( rgn);{ struct Cyc_List_List*
_temp3513=(( struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct
_RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, _temp3510->tvs, _temp3488); goto _LL3514; _LL3514:{ struct Cyc_List_List*
fs=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp3518= _temp3510->fields;
if( _temp3518 == 0){ _throw( Null_Exception);} _temp3518->v;}); for( 0; fs != 0;
fs=({ struct Cyc_List_List* _temp3515= fs; if( _temp3515 == 0){ _throw(
Null_Exception);} _temp3515->tl;})){ if( ! Cyc_Tcutil_bits_only( Cyc_Tcutil_rsubstitute(
rgn, _temp3513,( void*)(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp3516= fs; if( _temp3516 == 0){ _throw( Null_Exception);} _temp3516->hd;}))->type))){
int _temp3517= 0; _npop_handler( 0u); return _temp3517;}}}{ int _temp3519= 1;
_npop_handler( 0u); return _temp3519;}}; _pop_region(& _temp3512);}} _LL3459:
_temp3494= _temp3492; goto _LL3461; _LL3461: for( 0; _temp3494 != 0; _temp3494=({
struct Cyc_List_List* _temp3520= _temp3494; if( _temp3520 == 0){ _throw(
Null_Exception);} _temp3520->tl;})){ if( ! Cyc_Tcutil_bits_only(( void*)((
struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp3521= _temp3494; if(
_temp3521 == 0){ _throw( Null_Exception);} _temp3521->hd;}))->type)){ return 0;}}
return 1; _LL3463: return 0; _LL3439:;} struct _tuple18{ struct Cyc_List_List*
f1; struct Cyc_Absyn_Exp* f2; } ; static int Cyc_Tcutil_cnst_exp( struct Cyc_Tcenv_Tenv*
te, int var_okay, struct Cyc_Absyn_Exp* e){ void* _temp3522=( void*) e->r; void*
_temp3560; void* _temp3562; struct Cyc_Absyn_Exp* _temp3564; struct Cyc_Absyn_Enumfield*
_temp3566; struct Cyc_Absyn_Enumdecl* _temp3568; struct _tuple1* _temp3570; void*
_temp3572; struct _tuple1* _temp3574; struct Cyc_Absyn_Exp* _temp3576; struct
Cyc_Absyn_Exp* _temp3578; struct Cyc_Absyn_Exp* _temp3580; struct Cyc_Absyn_Exp*
_temp3582; struct Cyc_Absyn_Exp* _temp3584; struct Cyc_List_List* _temp3586;
struct Cyc_Absyn_Exp* _temp3588; struct Cyc_Absyn_Exp* _temp3590; void*
_temp3592; struct Cyc_Absyn_Exp* _temp3594; struct Cyc_Absyn_Exp* _temp3596;
struct Cyc_Absyn_Exp* _temp3598; struct Cyc_Absyn_Vardecl* _temp3600; struct Cyc_List_List*
_temp3602; struct Cyc_List_List* _temp3604; void* _temp3606; struct Cyc_Absyn_Structdecl*
_temp3608; struct Cyc_List_List* _temp3610; struct Cyc_Core_Opt* _temp3612;
struct _tuple1* _temp3614; struct Cyc_List_List* _temp3616; void* _temp3618;
struct Cyc_List_List* _temp3620; struct Cyc_Absyn_Tunionfield* _temp3622; struct
Cyc_Absyn_Tuniondecl* _temp3624; struct Cyc_List_List* _temp3626; struct Cyc_Core_Opt*
_temp3628; struct Cyc_Core_Opt* _temp3630; _LL3524: if(*(( int*) _temp3522) ==
Cyc_Absyn_Const_e){ _LL3561: _temp3560=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp3522)->f1; goto _LL3525;} else{ goto _LL3526;} _LL3526: if(*(( int*)
_temp3522) == Cyc_Absyn_Sizeoftyp_e){ _LL3563: _temp3562=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp3522)->f1; goto _LL3527;} else{ goto _LL3528;} _LL3528: if(*(( int*)
_temp3522) == Cyc_Absyn_Sizeofexp_e){ _LL3565: _temp3564=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Sizeofexp_e_struct*) _temp3522)->f1; goto _LL3529;} else{ goto
_LL3530;} _LL3530: if(*(( int*) _temp3522) == Cyc_Absyn_Enum_e){ _LL3571:
_temp3570=( struct _tuple1*)(( struct Cyc_Absyn_Enum_e_struct*) _temp3522)->f1;
goto _LL3569; _LL3569: _temp3568=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp3522)->f2; goto _LL3567; _LL3567: _temp3566=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_e_struct*) _temp3522)->f3; goto _LL3531;} else{ goto
_LL3532;} _LL3532: if(*(( int*) _temp3522) == Cyc_Absyn_Var_e){ _LL3575:
_temp3574=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp3522)->f1;
goto _LL3573; _LL3573: _temp3572=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp3522)->f2; goto _LL3533;} else{ goto _LL3534;} _LL3534: if(*(( int*)
_temp3522) == Cyc_Absyn_Conditional_e){ _LL3581: _temp3580=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp3522)->f1; goto _LL3579; _LL3579:
_temp3578=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp3522)->f2; goto _LL3577; _LL3577: _temp3576=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp3522)->f3; goto _LL3535;} else{
goto _LL3536;} _LL3536: if(*(( int*) _temp3522) == Cyc_Absyn_SeqExp_e){ _LL3585:
_temp3584=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp3522)->f1; goto _LL3583; _LL3583: _temp3582=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_SeqExp_e_struct*) _temp3522)->f2; goto _LL3537;} else{ goto
_LL3538;} _LL3538: if(*(( int*) _temp3522) == Cyc_Absyn_Instantiate_e){ _LL3589:
_temp3588=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp3522)->f1; goto _LL3587; _LL3587: _temp3586=( struct Cyc_List_List*)((
struct Cyc_Absyn_Instantiate_e_struct*) _temp3522)->f2; goto _LL3539;} else{
goto _LL3540;} _LL3540: if(*(( int*) _temp3522) == Cyc_Absyn_Cast_e){ _LL3593:
_temp3592=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp3522)->f1; goto
_LL3591; _LL3591: _temp3590=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp3522)->f2; goto _LL3541;} else{ goto _LL3542;} _LL3542: if(*(( int*)
_temp3522) == Cyc_Absyn_Address_e){ _LL3595: _temp3594=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Address_e_struct*) _temp3522)->f1; goto _LL3543;} else{ goto
_LL3544;} _LL3544: if(*(( int*) _temp3522) == Cyc_Absyn_Comprehension_e){
_LL3601: _temp3600=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3522)->f1; goto _LL3599; _LL3599: _temp3598=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp3522)->f2; goto _LL3597; _LL3597:
_temp3596=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3522)->f3; goto _LL3545;} else{ goto _LL3546;} _LL3546: if(*(( int*)
_temp3522) == Cyc_Absyn_Array_e){ _LL3603: _temp3602=( struct Cyc_List_List*)((
struct Cyc_Absyn_Array_e_struct*) _temp3522)->f1; goto _LL3547;} else{ goto
_LL3548;} _LL3548: if(*(( int*) _temp3522) == Cyc_Absyn_AnonStruct_e){ _LL3607:
_temp3606=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp3522)->f1; goto
_LL3605; _LL3605: _temp3604=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp3522)->f2; goto _LL3549;} else{ goto _LL3550;} _LL3550: if(*(( int*)
_temp3522) == Cyc_Absyn_Struct_e){ _LL3615: _temp3614=( struct _tuple1*)((
struct Cyc_Absyn_Struct_e_struct*) _temp3522)->f1; goto _LL3613; _LL3613:
_temp3612=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*) _temp3522)->f2;
goto _LL3611; _LL3611: _temp3610=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp3522)->f3; goto _LL3609; _LL3609: _temp3608=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp3522)->f4; goto _LL3551;} else{ goto
_LL3552;} _LL3552: if(*(( int*) _temp3522) == Cyc_Absyn_Primop_e){ _LL3619:
_temp3618=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp3522)->f1; goto
_LL3617; _LL3617: _temp3616=( struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp3522)->f2; goto _LL3553;} else{ goto _LL3554;} _LL3554: if(*(( int*)
_temp3522) == Cyc_Absyn_Tuple_e){ _LL3621: _temp3620=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tuple_e_struct*) _temp3522)->f1; goto _LL3555;} else{ goto
_LL3556;} _LL3556: if(*(( int*) _temp3522) == Cyc_Absyn_Tunion_e){ _LL3631:
_temp3630=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp3522)->f1;
goto _LL3629; _LL3629: _temp3628=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3522)->f2; goto _LL3627; _LL3627: _temp3626=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp3522)->f3; goto _LL3625; _LL3625:
_temp3624=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3522)->f4; goto _LL3623; _LL3623: _temp3622=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp3522)->f5; goto _LL3557;} else{ goto
_LL3558;} _LL3558: goto _LL3559; _LL3525: return 1; _LL3527: return 1; _LL3529:
return Cyc_Tcutil_cnst_exp( te, 1, _temp3564); _LL3531: return 1; _LL3533: {
struct _handler_cons _temp3632; _push_handler(& _temp3632);{ int _temp3634= 0;
if( setjmp( _temp3632.handler)){ _temp3634= 1;} if( ! _temp3634){{ void*
_temp3635= Cyc_Tcenv_lookup_ordinary( te, e->loc, _temp3574); void* _temp3643;
struct Cyc_Absyn_Fndecl* _temp3645; void* _temp3647; struct Cyc_Absyn_Vardecl*
_temp3649; _LL3637: if(*(( int*) _temp3635) == Cyc_Tcenv_VarRes){ _LL3644:
_temp3643=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp3635)->f1; if((
unsigned int) _temp3643 > 1u?*(( int*) _temp3643) == Cyc_Absyn_Funname_b: 0){
_LL3646: _temp3645=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Funname_b_struct*)
_temp3643)->f1; goto _LL3638;} else{ goto _LL3639;}} else{ goto _LL3639;}
_LL3639: if(*(( int*) _temp3635) == Cyc_Tcenv_VarRes){ _LL3648: _temp3647=( void*)((
struct Cyc_Tcenv_VarRes_struct*) _temp3635)->f1; if(( unsigned int) _temp3647 >
1u?*(( int*) _temp3647) == Cyc_Absyn_Global_b: 0){ _LL3650: _temp3649=( struct
Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Global_b_struct*) _temp3647)->f1; goto
_LL3640;} else{ goto _LL3641;}} else{ goto _LL3641;} _LL3641: goto _LL3642;
_LL3638: { int _temp3651= 1; _npop_handler( 0u); return _temp3651;} _LL3640: {
void* _temp3652= Cyc_Tcutil_compress(( void*) _temp3649->type); struct Cyc_Absyn_Exp*
_temp3658; struct Cyc_Absyn_Tqual _temp3660; void* _temp3662; _LL3654: if((
unsigned int) _temp3652 > 4u?*(( int*) _temp3652) == Cyc_Absyn_ArrayType: 0){
_LL3663: _temp3662=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3652)->f1;
goto _LL3661; _LL3661: _temp3660=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp3652)->f2; goto _LL3659; _LL3659: _temp3658=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3652)->f3; goto _LL3655;} else{ goto
_LL3656;} _LL3656: goto _LL3657; _LL3655: { int _temp3664= 1; _npop_handler( 0u);
return _temp3664;} _LL3657: { int _temp3665= var_okay; _npop_handler( 0u);
return _temp3665;} _LL3653:;} _LL3642: { int _temp3666= var_okay; _npop_handler(
0u); return _temp3666;} _LL3636:;}; _pop_handler();} else{ void* _temp3633=(
void*) _exn_thrown; void* _temp3668= _temp3633; _LL3670: if( _temp3668 == Cyc_Dict_Absent){
goto _LL3671;} else{ goto _LL3672;} _LL3672: goto _LL3673; _LL3671: return 0;
_LL3673:( void) _throw( _temp3668); _LL3669:;}}} _LL3535: return( Cyc_Tcutil_cnst_exp(
te, 0, _temp3580)? Cyc_Tcutil_cnst_exp( te, 0, _temp3578): 0)? Cyc_Tcutil_cnst_exp(
te, 0, _temp3576): 0; _LL3537: return Cyc_Tcutil_cnst_exp( te, 0, _temp3584)?
Cyc_Tcutil_cnst_exp( te, 0, _temp3582): 0; _LL3539: return Cyc_Tcutil_cnst_exp(
te, var_okay, _temp3588); _LL3541: return Cyc_Tcutil_cnst_exp( te, var_okay,
_temp3590); _LL3543: return Cyc_Tcutil_cnst_exp( te, 1, _temp3594); _LL3545:
return Cyc_Tcutil_cnst_exp( te, 0, _temp3598)? Cyc_Tcutil_cnst_exp( te, 0,
_temp3596): 0; _LL3547: _temp3604= _temp3602; goto _LL3549; _LL3549: _temp3610=
_temp3604; goto _LL3551; _LL3551: for( 0; _temp3610 != 0; _temp3610=({ struct
Cyc_List_List* _temp3674= _temp3610; if( _temp3674 == 0){ _throw( Null_Exception);}
_temp3674->tl;})){ if( ! Cyc_Tcutil_cnst_exp( te, 0,(*(( struct _tuple18*)({
struct Cyc_List_List* _temp3675= _temp3610; if( _temp3675 == 0){ _throw(
Null_Exception);} _temp3675->hd;}))).f2)){ return 0;}} return 1; _LL3553:
_temp3620= _temp3616; goto _LL3555; _LL3555: _temp3626= _temp3620; goto _LL3557;
_LL3557: for( 0; _temp3626 != 0; _temp3626=({ struct Cyc_List_List* _temp3676=
_temp3626; if( _temp3676 == 0){ _throw( Null_Exception);} _temp3676->tl;})){ if(
! Cyc_Tcutil_cnst_exp( te, 0,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp3677= _temp3626; if( _temp3677 == 0){ _throw( Null_Exception);} _temp3677->hd;}))){
return 0;}} return 1; _LL3559: return 0; _LL3523:;} int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ return Cyc_Tcutil_cnst_exp(
te, 0, e);} static int Cyc_Tcutil_fields_support_default( struct Cyc_List_List*
tvs, struct Cyc_List_List* ts, struct Cyc_List_List* fs); int Cyc_Tcutil_supports_default(
void* t){ void* _temp3678= Cyc_Tcutil_compress( t); int _temp3716; struct Cyc_Core_Opt*
_temp3718; void* _temp3720; struct Cyc_Absyn_Tvar* _temp3722; struct Cyc_Absyn_TunionInfo
_temp3724; struct Cyc_Absyn_TunionFieldInfo _temp3726; struct Cyc_Absyn_PtrInfo
_temp3728; struct Cyc_Absyn_Conref* _temp3730; struct Cyc_Absyn_Tqual _temp3732;
struct Cyc_Absyn_Conref* _temp3734; void* _temp3736; void* _temp3738; void*
_temp3740; void* _temp3742; struct Cyc_Absyn_Exp* _temp3744; struct Cyc_Absyn_Tqual
_temp3746; void* _temp3748; struct Cyc_Absyn_FnInfo _temp3750; struct Cyc_List_List*
_temp3752; struct Cyc_Absyn_Structdecl** _temp3754; struct Cyc_List_List*
_temp3756; struct _tuple1* _temp3758; struct Cyc_Absyn_Uniondecl** _temp3760;
struct Cyc_List_List* _temp3762; struct _tuple1* _temp3764; struct Cyc_List_List*
_temp3766; struct Cyc_List_List* _temp3768; struct Cyc_Absyn_Enumdecl* _temp3770;
struct _tuple1* _temp3772; _LL3680: if( _temp3678 ==( void*) Cyc_Absyn_VoidType){
goto _LL3681;} else{ goto _LL3682;} _LL3682: if(( unsigned int) _temp3678 > 4u?*((
int*) _temp3678) == Cyc_Absyn_Evar: 0){ _LL3721: _temp3720=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp3678)->f1; goto _LL3719; _LL3719: _temp3718=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Evar_struct*) _temp3678)->f2; goto _LL3717; _LL3717: _temp3716=(
int)(( struct Cyc_Absyn_Evar_struct*) _temp3678)->f3; goto _LL3683;} else{ goto
_LL3684;} _LL3684: if(( unsigned int) _temp3678 > 4u?*(( int*) _temp3678) == Cyc_Absyn_VarType:
0){ _LL3723: _temp3722=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp3678)->f1; goto _LL3685;} else{ goto _LL3686;} _LL3686: if(( unsigned int)
_temp3678 > 4u?*(( int*) _temp3678) == Cyc_Absyn_TunionType: 0){ _LL3725:
_temp3724=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp3678)->f1; goto _LL3687;} else{ goto _LL3688;} _LL3688: if(( unsigned int)
_temp3678 > 4u?*(( int*) _temp3678) == Cyc_Absyn_TunionFieldType: 0){ _LL3727:
_temp3726=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp3678)->f1; goto _LL3689;} else{ goto _LL3690;} _LL3690: if(( unsigned int)
_temp3678 > 4u?*(( int*) _temp3678) == Cyc_Absyn_PointerType: 0){ _LL3729:
_temp3728=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp3678)->f1; _LL3739: _temp3738=( void*) _temp3728.elt_typ; goto _LL3737;
_LL3737: _temp3736=( void*) _temp3728.rgn_typ; goto _LL3735; _LL3735: _temp3734=(
struct Cyc_Absyn_Conref*) _temp3728.nullable; goto _LL3733; _LL3733: _temp3732=(
struct Cyc_Absyn_Tqual) _temp3728.tq; goto _LL3731; _LL3731: _temp3730=( struct
Cyc_Absyn_Conref*) _temp3728.bounds; goto _LL3691;} else{ goto _LL3692;} _LL3692:
if(( unsigned int) _temp3678 > 4u?*(( int*) _temp3678) == Cyc_Absyn_IntType: 0){
_LL3743: _temp3742=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp3678)->f1;
goto _LL3741; _LL3741: _temp3740=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp3678)->f2; goto _LL3693;} else{ goto _LL3694;} _LL3694: if( _temp3678 ==(
void*) Cyc_Absyn_FloatType){ goto _LL3695;} else{ goto _LL3696;} _LL3696: if(
_temp3678 ==( void*) Cyc_Absyn_DoubleType){ goto _LL3697;} else{ goto _LL3698;}
_LL3698: if(( unsigned int) _temp3678 > 4u?*(( int*) _temp3678) == Cyc_Absyn_ArrayType:
0){ _LL3749: _temp3748=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3678)->f1;
goto _LL3747; _LL3747: _temp3746=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp3678)->f2; goto _LL3745; _LL3745: _temp3744=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3678)->f3; goto _LL3699;} else{ goto
_LL3700;} _LL3700: if(( unsigned int) _temp3678 > 4u?*(( int*) _temp3678) == Cyc_Absyn_FnType:
0){ _LL3751: _temp3750=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp3678)->f1; goto _LL3701;} else{ goto _LL3702;} _LL3702: if(( unsigned int)
_temp3678 > 4u?*(( int*) _temp3678) == Cyc_Absyn_TupleType: 0){ _LL3753:
_temp3752=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp3678)->f1; goto _LL3703;} else{ goto _LL3704;} _LL3704: if(( unsigned int)
_temp3678 > 4u?*(( int*) _temp3678) == Cyc_Absyn_StructType: 0){ _LL3759:
_temp3758=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp3678)->f1;
goto _LL3757; _LL3757: _temp3756=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp3678)->f2; goto _LL3755; _LL3755: _temp3754=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp3678)->f3; goto _LL3705;} else{ goto
_LL3706;} _LL3706: if(( unsigned int) _temp3678 > 4u?*(( int*) _temp3678) == Cyc_Absyn_UnionType:
0){ _LL3765: _temp3764=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp3678)->f1; goto _LL3763; _LL3763: _temp3762=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnionType_struct*) _temp3678)->f2; goto _LL3761; _LL3761:
_temp3760=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp3678)->f3; goto _LL3707;} else{ goto _LL3708;} _LL3708: if(( unsigned int)
_temp3678 > 4u?*(( int*) _temp3678) == Cyc_Absyn_AnonStructType: 0){ _LL3767:
_temp3766=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp3678)->f1; goto _LL3709;} else{ goto _LL3710;} _LL3710: if(( unsigned int)
_temp3678 > 4u?*(( int*) _temp3678) == Cyc_Absyn_AnonUnionType: 0){ _LL3769:
_temp3768=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp3678)->f1; goto _LL3711;} else{ goto _LL3712;} _LL3712: if(( unsigned int)
_temp3678 > 4u?*(( int*) _temp3678) == Cyc_Absyn_EnumType: 0){ _LL3773:
_temp3772=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*) _temp3678)->f1;
goto _LL3771; _LL3771: _temp3770=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*)
_temp3678)->f2; goto _LL3713;} else{ goto _LL3714;} _LL3714: goto _LL3715;
_LL3681: return 1; _LL3683: return 0; _LL3685: return 0; _LL3687: return 0;
_LL3689: return 0; _LL3691: { void* _temp3774=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp3730))->v; void*
_temp3782; void* _temp3784; struct Cyc_Absyn_Exp* _temp3786; _LL3776: if((
unsigned int) _temp3774 > 1u?*(( int*) _temp3774) == Cyc_Absyn_Eq_constr: 0){
_LL3783: _temp3782=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3774)->f1;
if( _temp3782 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL3777;} else{ goto _LL3778;}}
else{ goto _LL3778;} _LL3778: if(( unsigned int) _temp3774 > 1u?*(( int*)
_temp3774) == Cyc_Absyn_Eq_constr: 0){ _LL3785: _temp3784=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3774)->f1; if(( unsigned int) _temp3784 > 1u?*(( int*) _temp3784) == Cyc_Absyn_Upper_b:
0){ _LL3787: _temp3786=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp3784)->f1; goto _LL3779;} else{ goto _LL3780;}} else{ goto _LL3780;}
_LL3780: goto _LL3781; _LL3777: return 1; _LL3779: { void* _temp3788=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp3734))->v; int _temp3794; _LL3790: if(( unsigned int) _temp3788 > 1u?*((
int*) _temp3788) == Cyc_Absyn_Eq_constr: 0){ _LL3795: _temp3794=( int)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp3788)->f1; goto _LL3791;} else{ goto _LL3792;}
_LL3792: goto _LL3793; _LL3791: return _temp3794; _LL3793: return 0; _LL3789:;}
_LL3781: return 0; _LL3775:;} _LL3693: return 1; _LL3695: return 1; _LL3697:
return 1; _LL3699: return Cyc_Tcutil_supports_default( _temp3748); _LL3701:
return 0; _LL3703: for( 0; _temp3752 != 0; _temp3752=({ struct Cyc_List_List*
_temp3796= _temp3752; if( _temp3796 == 0){ _throw( Null_Exception);} _temp3796->tl;})){
if( ! Cyc_Tcutil_supports_default((*(( struct _tuple4*)({ struct Cyc_List_List*
_temp3797= _temp3752; if( _temp3797 == 0){ _throw( Null_Exception);} _temp3797->hd;}))).f2)){
return 0;}} return 1; _LL3705: if( _temp3754 == 0){ return 0;}{ struct Cyc_Absyn_Structdecl*
sd=({ struct Cyc_Absyn_Structdecl** _temp3799= _temp3754; if( _temp3799 == 0){
_throw( Null_Exception);}* _temp3799;}); if( sd->fields == 0){ return 0;} return
Cyc_Tcutil_fields_support_default( sd->tvs, _temp3756,( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp3798= sd->fields; if( _temp3798 == 0){ _throw(
Null_Exception);} _temp3798->v;}));} _LL3707: if( _temp3760 == 0){ return 0;}{
struct Cyc_Absyn_Uniondecl* ud=({ struct Cyc_Absyn_Uniondecl** _temp3801=
_temp3760; if( _temp3801 == 0){ _throw( Null_Exception);}* _temp3801;}); if( ud->fields
== 0){ return 0;} return Cyc_Tcutil_fields_support_default( ud->tvs, _temp3762,(
struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp3800= ud->fields; if(
_temp3800 == 0){ _throw( Null_Exception);} _temp3800->v;}));} _LL3709: _temp3768=
_temp3766; goto _LL3711; _LL3711: return Cyc_Tcutil_fields_support_default( 0, 0,
_temp3768); _LL3713: return 1; _LL3715: return 0; _LL3679:;} static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List* tvs, struct Cyc_List_List* ts, struct Cyc_List_List* fs){{
struct _RegionHandle _temp3802= _new_region(); struct _RegionHandle* rgn=&
_temp3802; _push_region( rgn);{ struct Cyc_List_List* _temp3803=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, tvs, ts); goto _LL3804;
_LL3804: for( 0; fs != 0; fs=({ struct Cyc_List_List* _temp3805= fs; if(
_temp3805 == 0){ _throw( Null_Exception);} _temp3805->tl;})){ void* t= Cyc_Tcutil_rsubstitute(
rgn, _temp3803,( void*)(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp3807= fs; if( _temp3807 == 0){ _throw( Null_Exception);} _temp3807->hd;}))->type);
if( ! Cyc_Tcutil_supports_default( t)){ int _temp3806= 0; _npop_handler( 0u);
return _temp3806;}}}; _pop_region(& _temp3802);} return 1;}