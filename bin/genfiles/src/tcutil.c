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
typedef struct _tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void
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
0; static const int Cyc_Absyn_Cdecl_att= 1; static const int Cyc_Absyn_Fastcall_att=
2; static const int Cyc_Absyn_Noreturn_att= 3; static const int Cyc_Absyn_Const_att=
4; static const int Cyc_Absyn_Aligned_att= 1; struct Cyc_Absyn_Aligned_att_struct{
int tag; int f1; } ; static const int Cyc_Absyn_Packed_att= 5; static const int
Cyc_Absyn_Section_att= 2; struct Cyc_Absyn_Section_att_struct{ int tag; struct
_tagged_string f1; } ; static const int Cyc_Absyn_Nocommon_att= 6; static const
int Cyc_Absyn_Shared_att= 7; static const int Cyc_Absyn_Unused_att= 8; static
const int Cyc_Absyn_Weak_att= 9; static const int Cyc_Absyn_Dllimport_att= 10;
static const int Cyc_Absyn_Dllexport_att= 11; static const int Cyc_Absyn_No_instrument_function_att=
12; static const int Cyc_Absyn_Constructor_att= 13; static const int Cyc_Absyn_Destructor_att=
14; static const int Cyc_Absyn_No_check_memory_usage_att= 15; static const int
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
int Cyc_Absyn_Letv_d= 3; struct Cyc_Absyn_Letv_d_struct{ int tag; struct Cyc_List_List*
f1; } ; static const int Cyc_Absyn_Struct_d= 4; struct Cyc_Absyn_Struct_d_struct{
int tag; struct Cyc_Absyn_Structdecl* f1; } ; static const int Cyc_Absyn_Union_d=
5; struct Cyc_Absyn_Union_d_struct{ int tag; struct Cyc_Absyn_Uniondecl* f1; } ;
static const int Cyc_Absyn_Tunion_d= 6; struct Cyc_Absyn_Tunion_d_struct{ int
tag; struct Cyc_Absyn_Tuniondecl* f1; } ; static const int Cyc_Absyn_Enum_d= 7;
struct Cyc_Absyn_Enum_d_struct{ int tag; struct Cyc_Absyn_Enumdecl* f1; } ;
static const int Cyc_Absyn_Typedef_d= 8; struct Cyc_Absyn_Typedef_d_struct{ int
tag; struct Cyc_Absyn_Typedefdecl* f1; } ; static const int Cyc_Absyn_Namespace_d=
9; struct Cyc_Absyn_Namespace_d_struct{ int tag; struct _tagged_string* f1;
struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Using_d= 10; struct Cyc_Absyn_Using_d_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d=
11; struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ;
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
_tagged_string Cyc_Absyn_attribute2string( void*); extern int Cyc_Absyn_fntype_att(
void* a); struct Cyc_PP_Ppstate; typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t;
struct Cyc_PP_Out; typedef struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc;
typedef struct Cyc_PP_Doc* Cyc_PP_doc_t; extern struct _tagged_string Cyc_Absynpp_typ2string(
void*); extern struct _tagged_string Cyc_Absynpp_kind2string( void*); extern
struct _tagged_string Cyc_Absynpp_ckind2string( struct Cyc_Absyn_Conref*);
extern struct _tagged_string Cyc_Absynpp_qvar2string( struct _tuple1*); struct
Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct Cyc_Set_Set*
Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t; extern char Cyc_Set_Absent[
11u]; struct Cyc_Dict_Dict; typedef struct Cyc_Dict_Dict* Cyc_Dict_hdict_t;
typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t; extern char Cyc_Dict_Present[ 12u];
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
Cyc_Tcutil_TypeErr[ 12u]="\000\000\000\000TypeErr"; extern void Cyc_Tcutil_unify_it(
void* t1, void* t2); void Cyc_Tcutil_terr( struct Cyc_Position_Segment* loc,
struct _tagged_string s){ Cyc_Position_post_error( Cyc_Position_mk_err_elab( loc,
s));} void* Cyc_Tcutil_impos( struct _tagged_string msg){({ struct
_tagged_string _temp0= msg; fprintf( Cyc_Stdio_stderr,"Error: %.*s\n", _temp0.last_plus_one
- _temp0.curr, _temp0.curr);}); Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*)
Cyc_Stdio_stderr);( void) _throw(( void*) Cyc_Tcutil_TypeErr);} static struct
_tagged_string Cyc_Tcutil_tvar2string( struct Cyc_Absyn_Tvar* tv){ return* tv->name;}
void Cyc_Tcutil_print_tvars( struct Cyc_List_List* tvs){ for( 0; tvs != 0; tvs=((
struct Cyc_List_List*) _check_null( tvs))->tl){({ struct _tagged_string _temp1=
Cyc_Tcutil_tvar2string(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs))->hd); struct _tagged_string _temp2= Cyc_Absynpp_ckind2string(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
fprintf( Cyc_Stdio_stderr,"%.*s::%.*s ", _temp1.last_plus_one - _temp1.curr,
_temp1.curr, _temp2.last_plus_one - _temp2.curr, _temp2.curr);});} fprintf( Cyc_Stdio_stderr,"\n");
Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*) Cyc_Stdio_stderr);} static struct
Cyc_List_List* Cyc_Tcutil_warning_segs= 0; static struct Cyc_List_List* Cyc_Tcutil_warning_msgs=
0; void Cyc_Tcutil_warn( struct Cyc_Position_Segment* sg, struct _tagged_string
msg){ Cyc_Tcutil_warning_segs=({ struct Cyc_List_List* _temp3=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3->hd=( void*) sg; _temp3->tl=
Cyc_Tcutil_warning_segs; _temp3;}); Cyc_Tcutil_warning_msgs=({ struct Cyc_List_List*
_temp4=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4->hd=( void*)(( struct _tagged_string*)({ struct _tagged_string* _temp5=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp5[
0]= msg; _temp5;})); _temp4->tl= Cyc_Tcutil_warning_msgs; _temp4;});} void Cyc_Tcutil_flush_warnings(){
if( Cyc_Tcutil_warning_segs == 0){ return;} fprintf( Cyc_Stdio_stderr,"***Warnings***\n");{
struct Cyc_List_List* seg_strs= Cyc_Position_strings_of_segments( Cyc_Tcutil_warning_segs);
Cyc_Tcutil_warning_segs= 0; Cyc_Tcutil_warning_msgs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_Tcutil_warning_msgs); while(
Cyc_Tcutil_warning_msgs != 0) {({ struct _tagged_string _temp6=*(( struct
_tagged_string*)(( struct Cyc_List_List*) _check_null( seg_strs))->hd); struct
_tagged_string _temp7=*(( struct _tagged_string*)(( struct Cyc_List_List*)
_check_null( Cyc_Tcutil_warning_msgs))->hd); fprintf( Cyc_Stdio_stderr,"%.*s: %.*s\n",
_temp6.last_plus_one - _temp6.curr, _temp6.curr, _temp7.last_plus_one - _temp7.curr,
_temp7.curr);}); seg_strs=(( struct Cyc_List_List*) _check_null( seg_strs))->tl;
Cyc_Tcutil_warning_msgs=(( struct Cyc_List_List*) _check_null( Cyc_Tcutil_warning_msgs))->tl;}
fprintf( Cyc_Stdio_stderr,"**************\n"); Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*)
Cyc_Stdio_stderr);}} struct Cyc_Core_Opt* Cyc_Tcutil_empty_var_set= 0; void* Cyc_Tcutil_compress(
void* t){ void* _temp8= t; int _temp20; struct Cyc_Core_Opt* _temp22; void*
_temp24; int _temp26; struct Cyc_Core_Opt* _temp28; struct Cyc_Core_Opt**
_temp30; void* _temp31; struct Cyc_Core_Opt* _temp33; struct Cyc_List_List*
_temp35; struct _tuple1* _temp37; struct Cyc_Core_Opt* _temp39; struct Cyc_Core_Opt**
_temp41; struct Cyc_List_List* _temp42; struct _tuple1* _temp44; _LL10: if((
unsigned int) _temp8 > 4u?*(( int*) _temp8) == Cyc_Absyn_Evar: 0){ _LL25:
_temp24=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp8)->f1; goto _LL23; _LL23:
_temp22=(( struct Cyc_Absyn_Evar_struct*) _temp8)->f2; if( _temp22 == 0){ goto
_LL21;} else{ goto _LL12;} _LL21: _temp20=(( struct Cyc_Absyn_Evar_struct*)
_temp8)->f3; goto _LL11;} else{ goto _LL12;} _LL12: if(( unsigned int) _temp8 >
4u?*(( int*) _temp8) == Cyc_Absyn_Evar: 0){ _LL32: _temp31=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp8)->f1; goto _LL29; _LL29: _temp28=(( struct Cyc_Absyn_Evar_struct*) _temp8)->f2;
_temp30=&(( struct Cyc_Absyn_Evar_struct*) _temp8)->f2; goto _LL27; _LL27:
_temp26=(( struct Cyc_Absyn_Evar_struct*) _temp8)->f3; goto _LL13;} else{ goto
_LL14;} _LL14: if(( unsigned int) _temp8 > 4u?*(( int*) _temp8) == Cyc_Absyn_TypedefType:
0){ _LL38: _temp37=(( struct Cyc_Absyn_TypedefType_struct*) _temp8)->f1; goto
_LL36; _LL36: _temp35=(( struct Cyc_Absyn_TypedefType_struct*) _temp8)->f2; goto
_LL34; _LL34: _temp33=(( struct Cyc_Absyn_TypedefType_struct*) _temp8)->f3; if(
_temp33 == 0){ goto _LL15;} else{ goto _LL16;}} else{ goto _LL16;} _LL16: if((
unsigned int) _temp8 > 4u?*(( int*) _temp8) == Cyc_Absyn_TypedefType: 0){ _LL45:
_temp44=(( struct Cyc_Absyn_TypedefType_struct*) _temp8)->f1; goto _LL43; _LL43:
_temp42=(( struct Cyc_Absyn_TypedefType_struct*) _temp8)->f2; goto _LL40; _LL40:
_temp39=(( struct Cyc_Absyn_TypedefType_struct*) _temp8)->f3; _temp41=&(( struct
Cyc_Absyn_TypedefType_struct*) _temp8)->f3; goto _LL17;} else{ goto _LL18;}
_LL18: goto _LL19; _LL11: return t; _LL13: { void* t2= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null(* _temp30))->v); if( t2 !=( void*)((
struct Cyc_Core_Opt*) _check_null(* _temp30))->v){* _temp30=({ struct Cyc_Core_Opt*
_temp46=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp46->v=( void*) t2; _temp46;});} return t2;} _LL15: return t; _LL17: { void*
t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(* _temp41))->v);
if( t2 !=( void*)(( struct Cyc_Core_Opt*) _check_null(* _temp41))->v){* _temp41=({
struct Cyc_Core_Opt* _temp47=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp47->v=( void*) t2; _temp47;});} return t2;} _LL19: return t;
_LL9:;} struct _tuple7{ void* f1; void* f2; } ; static int Cyc_Tcutil_kind_leq(
void* k1, void* k2){ if( k1 == k2){ return 1;}{ struct _tuple7 _temp49=({ struct
_tuple7 _temp48; _temp48.f1= k1; _temp48.f2= k2; _temp48;}); void* _temp59; void*
_temp61; void* _temp63; void* _temp65; void* _temp67; void* _temp69; _LL51:
_LL62: _temp61= _temp49.f1; if( _temp61 ==( void*) Cyc_Absyn_BoxKind){ goto
_LL60;} else{ goto _LL53;} _LL60: _temp59= _temp49.f2; if( _temp59 ==( void*)
Cyc_Absyn_MemKind){ goto _LL52;} else{ goto _LL53;} _LL53: _LL66: _temp65=
_temp49.f1; if( _temp65 ==( void*) Cyc_Absyn_BoxKind){ goto _LL64;} else{ goto
_LL55;} _LL64: _temp63= _temp49.f2; if( _temp63 ==( void*) Cyc_Absyn_AnyKind){
goto _LL54;} else{ goto _LL55;} _LL55: _LL70: _temp69= _temp49.f1; if( _temp69
==( void*) Cyc_Absyn_MemKind){ goto _LL68;} else{ goto _LL57;} _LL68: _temp67=
_temp49.f2; if( _temp67 ==( void*) Cyc_Absyn_AnyKind){ goto _LL56;} else{ goto
_LL57;} _LL57: goto _LL58; _LL52: return 1; _LL54: return 1; _LL56: return 1;
_LL58: return 0; _LL50:;}} void* Cyc_Tcutil_typ_kind( void* t){ void* _temp71=
Cyc_Tcutil_compress( t); int _temp123; struct Cyc_Core_Opt* _temp125; void*
_temp127; struct Cyc_Absyn_Tvar* _temp129; void* _temp131; void* _temp133;
struct Cyc_Absyn_FnInfo _temp135; void* _temp137; struct Cyc_Absyn_TunionInfo
_temp139; struct Cyc_Absyn_TunionFieldInfo _temp141; struct Cyc_List_List*
_temp143; void* _temp145; struct Cyc_Absyn_Tunionfield* _temp147; struct Cyc_Absyn_Tuniondecl*
_temp149; struct Cyc_Absyn_TunionFieldInfo _temp151; struct Cyc_List_List*
_temp153; void* _temp155; struct Cyc_Absyn_UnknownTunionFieldInfo _temp157;
struct Cyc_Absyn_Structdecl** _temp159; struct Cyc_List_List* _temp161; struct
_tuple1* _temp163; struct Cyc_Absyn_Uniondecl** _temp165; struct Cyc_List_List*
_temp167; struct _tuple1* _temp169; struct Cyc_Absyn_Structdecl** _temp171;
struct Cyc_List_List* _temp173; struct _tuple1* _temp175; struct Cyc_Absyn_Uniondecl**
_temp177; struct Cyc_List_List* _temp179; struct _tuple1* _temp181; struct Cyc_List_List*
_temp183; struct Cyc_List_List* _temp185; struct Cyc_Absyn_Enumdecl* _temp187;
struct _tuple1* _temp189; struct Cyc_Absyn_PtrInfo _temp191; struct Cyc_Absyn_Exp*
_temp193; struct Cyc_Absyn_Tqual _temp195; void* _temp197; struct Cyc_List_List*
_temp199; struct Cyc_Core_Opt* _temp201; struct Cyc_List_List* _temp203; struct
_tuple1* _temp205; void* _temp207; struct Cyc_List_List* _temp209; _LL73: if((
unsigned int) _temp71 > 4u?*(( int*) _temp71) == Cyc_Absyn_Evar: 0){ _LL128:
_temp127=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp71)->f1; goto _LL126;
_LL126: _temp125=(( struct Cyc_Absyn_Evar_struct*) _temp71)->f2; goto _LL124;
_LL124: _temp123=(( struct Cyc_Absyn_Evar_struct*) _temp71)->f3; goto _LL74;}
else{ goto _LL75;} _LL75: if(( unsigned int) _temp71 > 4u?*(( int*) _temp71) ==
Cyc_Absyn_VarType: 0){ _LL130: _temp129=(( struct Cyc_Absyn_VarType_struct*)
_temp71)->f1; goto _LL76;} else{ goto _LL77;} _LL77: if( _temp71 ==( void*) Cyc_Absyn_VoidType){
goto _LL78;} else{ goto _LL79;} _LL79: if(( unsigned int) _temp71 > 4u?*(( int*)
_temp71) == Cyc_Absyn_IntType: 0){ _LL134: _temp133=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp71)->f1; goto _LL132; _LL132: _temp131=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp71)->f2; goto _LL80;} else{ goto _LL81;} _LL81: if( _temp71 ==( void*) Cyc_Absyn_FloatType){
goto _LL82;} else{ goto _LL83;} _LL83: if( _temp71 ==( void*) Cyc_Absyn_DoubleType){
goto _LL84;} else{ goto _LL85;} _LL85: if(( unsigned int) _temp71 > 4u?*(( int*)
_temp71) == Cyc_Absyn_FnType: 0){ _LL136: _temp135=(( struct Cyc_Absyn_FnType_struct*)
_temp71)->f1; goto _LL86;} else{ goto _LL87;} _LL87: if(( unsigned int) _temp71
> 4u?*(( int*) _temp71) == Cyc_Absyn_RgnHandleType: 0){ _LL138: _temp137=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp71)->f1; goto _LL88;} else{ goto
_LL89;} _LL89: if( _temp71 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL90;} else{
goto _LL91;} _LL91: if(( unsigned int) _temp71 > 4u?*(( int*) _temp71) == Cyc_Absyn_TunionType:
0){ _LL140: _temp139=(( struct Cyc_Absyn_TunionType_struct*) _temp71)->f1; goto
_LL92;} else{ goto _LL93;} _LL93: if(( unsigned int) _temp71 > 4u?*(( int*)
_temp71) == Cyc_Absyn_TunionFieldType: 0){ _LL142: _temp141=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp71)->f1; _LL146: _temp145=( void*) _temp141.field_info; if(*(( int*)
_temp145) == Cyc_Absyn_KnownTunionfield){ _LL150: _temp149=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp145)->f1; goto _LL148; _LL148: _temp147=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp145)->f2; goto _LL144;} else{ goto _LL95;} _LL144: _temp143= _temp141.targs;
goto _LL94;} else{ goto _LL95;} _LL95: if(( unsigned int) _temp71 > 4u?*(( int*)
_temp71) == Cyc_Absyn_TunionFieldType: 0){ _LL152: _temp151=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp71)->f1; _LL156: _temp155=( void*) _temp151.field_info; if(*(( int*)
_temp155) == Cyc_Absyn_UnknownTunionfield){ _LL158: _temp157=(( struct Cyc_Absyn_UnknownTunionfield_struct*)
_temp155)->f1; goto _LL154;} else{ goto _LL97;} _LL154: _temp153= _temp151.targs;
goto _LL96;} else{ goto _LL97;} _LL97: if(( unsigned int) _temp71 > 4u?*(( int*)
_temp71) == Cyc_Absyn_StructType: 0){ _LL164: _temp163=(( struct Cyc_Absyn_StructType_struct*)
_temp71)->f1; goto _LL162; _LL162: _temp161=(( struct Cyc_Absyn_StructType_struct*)
_temp71)->f2; goto _LL160; _LL160: _temp159=(( struct Cyc_Absyn_StructType_struct*)
_temp71)->f3; if( _temp159 == 0){ goto _LL98;} else{ goto _LL99;}} else{ goto
_LL99;} _LL99: if(( unsigned int) _temp71 > 4u?*(( int*) _temp71) == Cyc_Absyn_UnionType:
0){ _LL170: _temp169=(( struct Cyc_Absyn_UnionType_struct*) _temp71)->f1; goto
_LL168; _LL168: _temp167=(( struct Cyc_Absyn_UnionType_struct*) _temp71)->f2;
goto _LL166; _LL166: _temp165=(( struct Cyc_Absyn_UnionType_struct*) _temp71)->f3;
if( _temp165 == 0){ goto _LL100;} else{ goto _LL101;}} else{ goto _LL101;}
_LL101: if(( unsigned int) _temp71 > 4u?*(( int*) _temp71) == Cyc_Absyn_StructType:
0){ _LL176: _temp175=(( struct Cyc_Absyn_StructType_struct*) _temp71)->f1; goto
_LL174; _LL174: _temp173=(( struct Cyc_Absyn_StructType_struct*) _temp71)->f2;
goto _LL172; _LL172: _temp171=(( struct Cyc_Absyn_StructType_struct*) _temp71)->f3;
goto _LL102;} else{ goto _LL103;} _LL103: if(( unsigned int) _temp71 > 4u?*((
int*) _temp71) == Cyc_Absyn_UnionType: 0){ _LL182: _temp181=(( struct Cyc_Absyn_UnionType_struct*)
_temp71)->f1; goto _LL180; _LL180: _temp179=(( struct Cyc_Absyn_UnionType_struct*)
_temp71)->f2; goto _LL178; _LL178: _temp177=(( struct Cyc_Absyn_UnionType_struct*)
_temp71)->f3; goto _LL104;} else{ goto _LL105;} _LL105: if(( unsigned int)
_temp71 > 4u?*(( int*) _temp71) == Cyc_Absyn_AnonStructType: 0){ _LL184:
_temp183=(( struct Cyc_Absyn_AnonStructType_struct*) _temp71)->f1; goto _LL106;}
else{ goto _LL107;} _LL107: if(( unsigned int) _temp71 > 4u?*(( int*) _temp71)
== Cyc_Absyn_AnonUnionType: 0){ _LL186: _temp185=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp71)->f1; goto _LL108;} else{ goto _LL109;} _LL109: if(( unsigned int)
_temp71 > 4u?*(( int*) _temp71) == Cyc_Absyn_EnumType: 0){ _LL190: _temp189=((
struct Cyc_Absyn_EnumType_struct*) _temp71)->f1; goto _LL188; _LL188: _temp187=((
struct Cyc_Absyn_EnumType_struct*) _temp71)->f2; goto _LL110;} else{ goto _LL111;}
_LL111: if(( unsigned int) _temp71 > 4u?*(( int*) _temp71) == Cyc_Absyn_PointerType:
0){ _LL192: _temp191=(( struct Cyc_Absyn_PointerType_struct*) _temp71)->f1; goto
_LL112;} else{ goto _LL113;} _LL113: if(( unsigned int) _temp71 > 4u?*(( int*)
_temp71) == Cyc_Absyn_ArrayType: 0){ _LL198: _temp197=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp71)->f1; goto _LL196; _LL196: _temp195=(( struct Cyc_Absyn_ArrayType_struct*)
_temp71)->f2; goto _LL194; _LL194: _temp193=(( struct Cyc_Absyn_ArrayType_struct*)
_temp71)->f3; goto _LL114;} else{ goto _LL115;} _LL115: if(( unsigned int)
_temp71 > 4u?*(( int*) _temp71) == Cyc_Absyn_TupleType: 0){ _LL200: _temp199=((
struct Cyc_Absyn_TupleType_struct*) _temp71)->f1; goto _LL116;} else{ goto
_LL117;} _LL117: if(( unsigned int) _temp71 > 4u?*(( int*) _temp71) == Cyc_Absyn_TypedefType:
0){ _LL206: _temp205=(( struct Cyc_Absyn_TypedefType_struct*) _temp71)->f1; goto
_LL204; _LL204: _temp203=(( struct Cyc_Absyn_TypedefType_struct*) _temp71)->f2;
goto _LL202; _LL202: _temp201=(( struct Cyc_Absyn_TypedefType_struct*) _temp71)->f3;
goto _LL118;} else{ goto _LL119;} _LL119: if(( unsigned int) _temp71 > 4u?*((
int*) _temp71) == Cyc_Absyn_AccessEff: 0){ _LL208: _temp207=( void*)(( struct
Cyc_Absyn_AccessEff_struct*) _temp71)->f1; goto _LL120;} else{ goto _LL121;}
_LL121: if(( unsigned int) _temp71 > 4u?*(( int*) _temp71) == Cyc_Absyn_JoinEff:
0){ _LL210: _temp209=(( struct Cyc_Absyn_JoinEff_struct*) _temp71)->f1; goto
_LL122;} else{ goto _LL72;} _LL74: return _temp127; _LL76: return(( void*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp129->kind); _LL78:
return( void*) Cyc_Absyn_MemKind; _LL80: return _temp131 ==( void*) Cyc_Absyn_B4?(
void*) Cyc_Absyn_BoxKind:( void*) Cyc_Absyn_MemKind; _LL82: return( void*) Cyc_Absyn_MemKind;
_LL84: return( void*) Cyc_Absyn_MemKind; _LL86: return( void*) Cyc_Absyn_MemKind;
_LL88: return( void*) Cyc_Absyn_BoxKind; _LL90: return( void*) Cyc_Absyn_RgnKind;
_LL92: return( void*) Cyc_Absyn_BoxKind; _LL94: if( _temp147->typs == 0){ return(
void*) Cyc_Absyn_BoxKind;} else{ return( void*) Cyc_Absyn_MemKind;} _LL96:
return(( void*(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({ char*
_temp211="typ_kind: Unresolved TunionFieldType"; struct _tagged_string _temp212;
_temp212.curr= _temp211; _temp212.base= _temp211; _temp212.last_plus_one=
_temp211 + 37; _temp212;})); _LL98: return( void*) Cyc_Absyn_AnyKind; _LL100:
return( void*) Cyc_Absyn_AnyKind; _LL102: return( void*) Cyc_Absyn_MemKind;
_LL104: return( void*) Cyc_Absyn_MemKind; _LL106: return( void*) Cyc_Absyn_MemKind;
_LL108: return( void*) Cyc_Absyn_MemKind; _LL110: return( void*) Cyc_Absyn_BoxKind;
_LL112: { void* _temp213=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp191.bounds))->v; void* _temp223; void*
_temp225; struct Cyc_Absyn_Exp* _temp227; struct Cyc_Absyn_Conref* _temp229;
_LL215: if(( unsigned int) _temp213 > 1u?*(( int*) _temp213) == Cyc_Absyn_Eq_constr:
0){ _LL224: _temp223=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp213)->f1;
if( _temp223 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL216;} else{ goto _LL217;}}
else{ goto _LL217;} _LL217: if(( unsigned int) _temp213 > 1u?*(( int*) _temp213)
== Cyc_Absyn_Eq_constr: 0){ _LL226: _temp225=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp213)->f1; if(( unsigned int) _temp225 > 1u?*(( int*) _temp225) == Cyc_Absyn_Upper_b:
0){ _LL228: _temp227=(( struct Cyc_Absyn_Upper_b_struct*) _temp225)->f1; goto
_LL218;} else{ goto _LL219;}} else{ goto _LL219;} _LL219: if( _temp213 ==( void*)
Cyc_Absyn_No_constr){ goto _LL220;} else{ goto _LL221;} _LL221: if((
unsigned int) _temp213 > 1u?*(( int*) _temp213) == Cyc_Absyn_Forward_constr: 0){
_LL230: _temp229=(( struct Cyc_Absyn_Forward_constr_struct*) _temp213)->f1; goto
_LL222;} else{ goto _LL214;} _LL216: return( void*) Cyc_Absyn_MemKind; _LL218:
return( void*) Cyc_Absyn_BoxKind; _LL220: return( void*) Cyc_Absyn_MemKind;
_LL222: return(( void*(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({ char*
_temp231="typ_kind: forward constraint in ptr bounds"; struct _tagged_string
_temp232; _temp232.curr= _temp231; _temp232.base= _temp231; _temp232.last_plus_one=
_temp231 + 43; _temp232;})); _LL214:;} _LL114: return( void*) Cyc_Absyn_MemKind;
_LL116: return( void*) Cyc_Absyn_MemKind; _LL118: return(( void*(*)( struct
_tagged_string msg)) Cyc_Tcutil_impos)(({ struct _tagged_string _temp233= Cyc_Absynpp_typ2string(
t); xprintf("typ_kind: typedef found: %.*s", _temp233.last_plus_one - _temp233.curr,
_temp233.curr);})); _LL120: return( void*) Cyc_Absyn_EffKind; _LL122: return(
void*) Cyc_Absyn_EffKind; _LL72:;} char Cyc_Tcutil_Unify[ 10u]="\000\000\000\000Unify";
int Cyc_Tcutil_unify( void* t1, void* t2){ struct _handler_cons _temp234;
_push_handler(& _temp234);{ int _temp236= 0; if( setjmp( _temp234.handler)){
_temp236= 1;} if( ! _temp236){ Cyc_Tcutil_unify_it( t1, t2);{ int _temp237= 1;
_npop_handler( 0u); return _temp237;}; _pop_handler();} else{ void* _temp235=(
void*) _exn_thrown; void* _temp239= _temp235; _LL241: if( _temp239 == Cyc_Tcutil_Unify){
goto _LL242;} else{ goto _LL243;} _LL243: goto _LL244; _LL242: return 0; _LL244:(
void) _throw( _temp239); _LL240:;}}} static void Cyc_Tcutil_occurslist( void*
evar, struct Cyc_List_List* ts); static void Cyc_Tcutil_occurs( void* evar, void*
t){ void* _temp245= Cyc_Tcutil_compress( t); int _temp277; struct Cyc_Core_Opt*
_temp279; void* _temp281; struct Cyc_Absyn_PtrInfo _temp283; struct Cyc_Absyn_Exp*
_temp285; struct Cyc_Absyn_Tqual _temp287; void* _temp289; struct Cyc_Absyn_FnInfo
_temp291; struct Cyc_List_List* _temp293; int _temp295; struct Cyc_List_List*
_temp297; void* _temp299; struct Cyc_Core_Opt* _temp301; struct Cyc_List_List*
_temp303; struct Cyc_List_List* _temp305; void* _temp307; struct Cyc_Absyn_TunionInfo
_temp309; void* _temp311; struct Cyc_List_List* _temp313; void* _temp315; struct
Cyc_Core_Opt* _temp317; struct Cyc_List_List* _temp319; struct _tuple1* _temp321;
struct Cyc_Absyn_TunionFieldInfo _temp323; struct Cyc_List_List* _temp325; void*
_temp327; struct Cyc_Absyn_Structdecl** _temp329; struct Cyc_List_List* _temp331;
struct _tuple1* _temp333; struct Cyc_List_List* _temp335; struct Cyc_List_List*
_temp337; void* _temp339; struct Cyc_List_List* _temp341; _LL247: if((
unsigned int) _temp245 > 4u?*(( int*) _temp245) == Cyc_Absyn_Evar: 0){ _LL282:
_temp281=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp245)->f1; goto _LL280;
_LL280: _temp279=(( struct Cyc_Absyn_Evar_struct*) _temp245)->f2; goto _LL278;
_LL278: _temp277=(( struct Cyc_Absyn_Evar_struct*) _temp245)->f3; goto _LL248;}
else{ goto _LL249;} _LL249: if(( unsigned int) _temp245 > 4u?*(( int*) _temp245)
== Cyc_Absyn_PointerType: 0){ _LL284: _temp283=(( struct Cyc_Absyn_PointerType_struct*)
_temp245)->f1; goto _LL250;} else{ goto _LL251;} _LL251: if(( unsigned int)
_temp245 > 4u?*(( int*) _temp245) == Cyc_Absyn_ArrayType: 0){ _LL290: _temp289=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp245)->f1; goto _LL288; _LL288:
_temp287=(( struct Cyc_Absyn_ArrayType_struct*) _temp245)->f2; goto _LL286;
_LL286: _temp285=(( struct Cyc_Absyn_ArrayType_struct*) _temp245)->f3; goto
_LL252;} else{ goto _LL253;} _LL253: if(( unsigned int) _temp245 > 4u?*(( int*)
_temp245) == Cyc_Absyn_FnType: 0){ _LL292: _temp291=(( struct Cyc_Absyn_FnType_struct*)
_temp245)->f1; _LL304: _temp303= _temp291.tvars; goto _LL302; _LL302: _temp301=
_temp291.effect; goto _LL300; _LL300: _temp299=( void*) _temp291.ret_typ; goto
_LL298; _LL298: _temp297= _temp291.args; goto _LL296; _LL296: _temp295= _temp291.varargs;
goto _LL294; _LL294: _temp293= _temp291.attributes; goto _LL254;} else{ goto
_LL255;} _LL255: if(( unsigned int) _temp245 > 4u?*(( int*) _temp245) == Cyc_Absyn_TupleType:
0){ _LL306: _temp305=(( struct Cyc_Absyn_TupleType_struct*) _temp245)->f1; goto
_LL256;} else{ goto _LL257;} _LL257: if(( unsigned int) _temp245 > 4u?*(( int*)
_temp245) == Cyc_Absyn_RgnHandleType: 0){ _LL308: _temp307=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp245)->f1; goto _LL258;} else{ goto _LL259;} _LL259: if(( unsigned int)
_temp245 > 4u?*(( int*) _temp245) == Cyc_Absyn_TunionType: 0){ _LL310: _temp309=((
struct Cyc_Absyn_TunionType_struct*) _temp245)->f1; _LL316: _temp315=( void*)
_temp309.tunion_info; goto _LL314; _LL314: _temp313= _temp309.targs; goto _LL312;
_LL312: _temp311=( void*) _temp309.rgn; goto _LL260;} else{ goto _LL261;} _LL261:
if(( unsigned int) _temp245 > 4u?*(( int*) _temp245) == Cyc_Absyn_TypedefType: 0){
_LL322: _temp321=(( struct Cyc_Absyn_TypedefType_struct*) _temp245)->f1; goto
_LL320; _LL320: _temp319=(( struct Cyc_Absyn_TypedefType_struct*) _temp245)->f2;
goto _LL318; _LL318: _temp317=(( struct Cyc_Absyn_TypedefType_struct*) _temp245)->f3;
goto _LL262;} else{ goto _LL263;} _LL263: if(( unsigned int) _temp245 > 4u?*((
int*) _temp245) == Cyc_Absyn_TunionFieldType: 0){ _LL324: _temp323=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp245)->f1; _LL328: _temp327=( void*) _temp323.field_info; goto _LL326;
_LL326: _temp325= _temp323.targs; goto _LL264;} else{ goto _LL265;} _LL265: if((
unsigned int) _temp245 > 4u?*(( int*) _temp245) == Cyc_Absyn_StructType: 0){
_LL334: _temp333=(( struct Cyc_Absyn_StructType_struct*) _temp245)->f1; goto
_LL332; _LL332: _temp331=(( struct Cyc_Absyn_StructType_struct*) _temp245)->f2;
goto _LL330; _LL330: _temp329=(( struct Cyc_Absyn_StructType_struct*) _temp245)->f3;
goto _LL266;} else{ goto _LL267;} _LL267: if(( unsigned int) _temp245 > 4u?*((
int*) _temp245) == Cyc_Absyn_AnonStructType: 0){ _LL336: _temp335=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp245)->f1; goto _LL268;} else{ goto _LL269;} _LL269: if(( unsigned int)
_temp245 > 4u?*(( int*) _temp245) == Cyc_Absyn_AnonUnionType: 0){ _LL338:
_temp337=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp245)->f1; goto _LL270;}
else{ goto _LL271;} _LL271: if(( unsigned int) _temp245 > 4u?*(( int*) _temp245)
== Cyc_Absyn_AccessEff: 0){ _LL340: _temp339=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp245)->f1; goto _LL272;} else{ goto _LL273;} _LL273: if(( unsigned int)
_temp245 > 4u?*(( int*) _temp245) == Cyc_Absyn_JoinEff: 0){ _LL342: _temp341=((
struct Cyc_Absyn_JoinEff_struct*) _temp245)->f1; goto _LL274;} else{ goto _LL275;}
_LL275: goto _LL276; _LL248: if( t == evar){( void) _throw(( void*) Cyc_Tcutil_Unify);}
else{ if( _temp279 != 0){ Cyc_Tcutil_occurs( evar,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp279))->v);}} goto _LL246; _LL250: Cyc_Tcutil_occurs( evar,(
void*) _temp283.elt_typ); Cyc_Tcutil_occurs( evar,( void*) _temp283.rgn_typ);
goto _LL246; _LL252: Cyc_Tcutil_occurs( evar, _temp289); goto _LL246; _LL254:
if( _temp301 != 0){ Cyc_Tcutil_occurs( evar,( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp301))->v);} Cyc_Tcutil_occurs( evar, _temp299); for( 0;
_temp297 != 0; _temp297=(( struct Cyc_List_List*) _check_null( _temp297))->tl){
Cyc_Tcutil_occurs( evar,(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp297))->hd)).f3);} goto _LL246; _LL256: for( 0; _temp305 != 0;
_temp305=(( struct Cyc_List_List*) _check_null( _temp305))->tl){ Cyc_Tcutil_occurs(
evar,(*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp305))->hd)).f2);}
goto _LL246; _LL258: Cyc_Tcutil_occurs( evar, _temp307); goto _LL246; _LL260:
Cyc_Tcutil_occurs( evar, _temp311); Cyc_Tcutil_occurslist( evar, _temp313); goto
_LL246; _LL262: _temp325= _temp319; goto _LL264; _LL264: _temp331= _temp325;
goto _LL266; _LL266: Cyc_Tcutil_occurslist( evar, _temp331); goto _LL246; _LL268:
_temp337= _temp335; goto _LL270; _LL270: for( 0; _temp337 != 0; _temp337=((
struct Cyc_List_List*) _check_null( _temp337))->tl){ Cyc_Tcutil_occurs( evar,(
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp337))->hd)->type);} goto _LL246; _LL272: Cyc_Tcutil_occurs( evar, _temp339);
goto _LL246; _LL274: Cyc_Tcutil_occurslist( evar, _temp341); goto _LL246; _LL276:
goto _LL246; _LL246:;} static void Cyc_Tcutil_occurslist( void* evar, struct Cyc_List_List*
ts){ for( 0; ts != 0; ts=(( struct Cyc_List_List*) _check_null( ts))->tl){ Cyc_Tcutil_occurs(
evar,( void*)(( struct Cyc_List_List*) _check_null( ts))->hd);}} static void Cyc_Tcutil_unify_list(
struct Cyc_List_List* t1, struct Cyc_List_List* t2){ struct _handler_cons
_temp343; _push_handler(& _temp343);{ int _temp345= 0; if( setjmp( _temp343.handler)){
_temp345= 1;} if( ! _temp345){(( void(*)( void(* f)( void*, void*), struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_iter2)( Cyc_Tcutil_unify_it, t1, t2);;
_pop_handler();} else{ void* _temp344=( void*) _exn_thrown; void* _temp347=
_temp344; _LL349: if( _temp347 == Cyc_List_List_mismatch){ goto _LL350;} else{
goto _LL351;} _LL351: goto _LL352; _LL350:( void) _throw(( void*) Cyc_Tcutil_Unify);
_LL352:( void) _throw( _temp347); _LL348:;}}} static void Cyc_Tcutil_unify_tqual(
struct Cyc_Absyn_Tqual tq1, struct Cyc_Absyn_Tqual tq2){ if(( tq1.q_const != tq2.q_const?
1: tq1.q_volatile != tq2.q_volatile)? 1: tq1.q_restrict != tq2.q_restrict){(
void) _throw(( void*) Cyc_Tcutil_Unify);}} int Cyc_Tcutil_equal_tqual( struct
Cyc_Absyn_Tqual tq1, struct Cyc_Absyn_Tqual tq2){ return( tq1.q_const == tq2.q_const?
tq1.q_volatile == tq2.q_volatile: 0)? tq1.q_restrict == tq2.q_restrict: 0;}
static void Cyc_Tcutil_unify_it_conrefs( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref*
x, struct Cyc_Absyn_Conref* y){ x=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( x); y=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( y); if( x == y){ return;}{ void* _temp353=( void*)
x->v; struct Cyc_Absyn_Conref* _temp361; void* _temp363; _LL355: if( _temp353 ==(
void*) Cyc_Absyn_No_constr){ goto _LL356;} else{ goto _LL357;} _LL357: if((
unsigned int) _temp353 > 1u?*(( int*) _temp353) == Cyc_Absyn_Forward_constr: 0){
_LL362: _temp361=(( struct Cyc_Absyn_Forward_constr_struct*) _temp353)->f1; goto
_LL358;} else{ goto _LL359;} _LL359: if(( unsigned int) _temp353 > 1u?*(( int*)
_temp353) == Cyc_Absyn_Eq_constr: 0){ _LL364: _temp363=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp353)->f1; goto _LL360;} else{ goto _LL354;} _LL356:( void*)( x->v=( void*)((
void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp365=( struct Cyc_Absyn_Forward_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp365[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp366; _temp366.tag= Cyc_Absyn_Forward_constr;
_temp366.f1= y; _temp366;}); _temp365;}))); return; _LL358:( void) _throw(((
void*(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({ char* _temp367="unify_conref: forward after compress";
struct _tagged_string _temp368; _temp368.curr= _temp367; _temp368.base= _temp367;
_temp368.last_plus_one= _temp367 + 37; _temp368;}))); _LL360: { void* _temp369=(
void*) y->v; struct Cyc_Absyn_Conref* _temp377; void* _temp379; _LL371: if(
_temp369 ==( void*) Cyc_Absyn_No_constr){ goto _LL372;} else{ goto _LL373;}
_LL373: if(( unsigned int) _temp369 > 1u?*(( int*) _temp369) == Cyc_Absyn_Forward_constr:
0){ _LL378: _temp377=(( struct Cyc_Absyn_Forward_constr_struct*) _temp369)->f1;
goto _LL374;} else{ goto _LL375;} _LL375: if(( unsigned int) _temp369 > 1u?*((
int*) _temp369) == Cyc_Absyn_Eq_constr: 0){ _LL380: _temp379=( void*)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp369)->f1; goto _LL376;} else{ goto _LL370;}
_LL372:( void*)( y->v=( void*)(( void*) x->v)); return; _LL374:( void) _throw(((
void*(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({ char* _temp381="unify_conref: forward after compress(2)";
struct _tagged_string _temp382; _temp382.curr= _temp381; _temp382.base= _temp381;
_temp382.last_plus_one= _temp381 + 40; _temp382;}))); _LL376: if( cmp( _temp363,
_temp379) != 0){( void) _throw(( void*) Cyc_Tcutil_Unify);} return; _LL370:;}
_LL354:;}} static int Cyc_Tcutil_unify_conrefs( int(* cmp)( void*, void*),
struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y){ struct _handler_cons
_temp383; _push_handler(& _temp383);{ int _temp385= 0; if( setjmp( _temp383.handler)){
_temp385= 1;} if( ! _temp385){(( void(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref*
x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_it_conrefs)( cmp, x, y);{ int
_temp386= 1; _npop_handler( 0u); return _temp386;}; _pop_handler();} else{ void*
_temp384=( void*) _exn_thrown; void* _temp388= _temp384; _LL390: if( _temp388 ==
Cyc_Tcutil_Unify){ goto _LL391;} else{ goto _LL392;} _LL392: goto _LL393; _LL391:
return 0; _LL393:( void) _throw( _temp388); _LL389:;}}} static int Cyc_Tcutil_boundscmp(
void* b1, void* b2){ struct _tuple7 _temp395=({ struct _tuple7 _temp394;
_temp394.f1= b1; _temp394.f2= b2; _temp394;}); void* _temp405; void* _temp407;
void* _temp409; void* _temp411; void* _temp413; void* _temp415; void* _temp417;
struct Cyc_Absyn_Exp* _temp419; void* _temp421; struct Cyc_Absyn_Exp* _temp423;
_LL397: _LL408: _temp407= _temp395.f1; if( _temp407 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL406;} else{ goto _LL399;} _LL406: _temp405= _temp395.f2; if( _temp405 ==(
void*) Cyc_Absyn_Unknown_b){ goto _LL398;} else{ goto _LL399;} _LL399: _LL412:
_temp411= _temp395.f1; if( _temp411 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL410;}
else{ goto _LL401;} _LL410: _temp409= _temp395.f2; goto _LL400; _LL401: _LL416:
_temp415= _temp395.f1; goto _LL414; _LL414: _temp413= _temp395.f2; if( _temp413
==( void*) Cyc_Absyn_Unknown_b){ goto _LL402;} else{ goto _LL403;} _LL403:
_LL422: _temp421= _temp395.f1; if(( unsigned int) _temp421 > 1u?*(( int*)
_temp421) == Cyc_Absyn_Upper_b: 0){ _LL424: _temp423=(( struct Cyc_Absyn_Upper_b_struct*)
_temp421)->f1; goto _LL418;} else{ goto _LL396;} _LL418: _temp417= _temp395.f2;
if(( unsigned int) _temp417 > 1u?*(( int*) _temp417) == Cyc_Absyn_Upper_b: 0){
_LL420: _temp419=(( struct Cyc_Absyn_Upper_b_struct*) _temp417)->f1; goto _LL404;}
else{ goto _LL396;} _LL398: return 0; _LL400: return - 1; _LL402: return 1;
_LL404: { int i1=( int) Cyc_Evexp_eval_const_uint_exp( _temp423); int i2=( int)
Cyc_Evexp_eval_const_uint_exp( _temp419); if( i1 == i2){ return 0;} if( i1 < i2){
return - 1;} return 1;} _LL396:;} static int Cyc_Tcutil_equal_att( void* a1,
void* a2){ if( a1 == a2){ return 1;}{ struct _tuple7 _temp426=({ struct _tuple7
_temp425; _temp425.f1= a1; _temp425.f2= a2; _temp425;}); void* _temp436; int
_temp438; void* _temp440; int _temp442; void* _temp444; int _temp446; void*
_temp448; int _temp450; void* _temp452; struct _tagged_string _temp454; void*
_temp456; struct _tagged_string _temp458; _LL428: _LL441: _temp440= _temp426.f1;
if(( unsigned int) _temp440 > 16u?*(( int*) _temp440) == Cyc_Absyn_Regparm_att:
0){ _LL443: _temp442=(( struct Cyc_Absyn_Regparm_att_struct*) _temp440)->f1;
goto _LL437;} else{ goto _LL430;} _LL437: _temp436= _temp426.f2; if((
unsigned int) _temp436 > 16u?*(( int*) _temp436) == Cyc_Absyn_Regparm_att: 0){
_LL439: _temp438=(( struct Cyc_Absyn_Regparm_att_struct*) _temp436)->f1; goto
_LL429;} else{ goto _LL430;} _LL430: _LL449: _temp448= _temp426.f1; if((
unsigned int) _temp448 > 16u?*(( int*) _temp448) == Cyc_Absyn_Aligned_att: 0){
_LL451: _temp450=(( struct Cyc_Absyn_Aligned_att_struct*) _temp448)->f1; goto
_LL445;} else{ goto _LL432;} _LL445: _temp444= _temp426.f2; if(( unsigned int)
_temp444 > 16u?*(( int*) _temp444) == Cyc_Absyn_Aligned_att: 0){ _LL447:
_temp446=(( struct Cyc_Absyn_Aligned_att_struct*) _temp444)->f1; goto _LL431;}
else{ goto _LL432;} _LL432: _LL457: _temp456= _temp426.f1; if(( unsigned int)
_temp456 > 16u?*(( int*) _temp456) == Cyc_Absyn_Section_att: 0){ _LL459:
_temp458=(( struct Cyc_Absyn_Section_att_struct*) _temp456)->f1; goto _LL453;}
else{ goto _LL434;} _LL453: _temp452= _temp426.f2; if(( unsigned int) _temp452 >
16u?*(( int*) _temp452) == Cyc_Absyn_Section_att: 0){ _LL455: _temp454=(( struct
Cyc_Absyn_Section_att_struct*) _temp452)->f1; goto _LL433;} else{ goto _LL434;}
_LL434: goto _LL435; _LL429: _temp450= _temp442; _temp446= _temp438; goto _LL431;
_LL431: return _temp450 == _temp446; _LL433: return Cyc_String_strcmp( _temp458,
_temp454) == 0; _LL435: return 0; _LL427:;}} int Cyc_Tcutil_same_atts( struct
Cyc_List_List* a1, struct Cyc_List_List* a2){{ struct Cyc_List_List* a= a1; for(
0; a != 0; a=(( struct Cyc_List_List*) _check_null( a))->tl){ if( !(( int(*)(
int(* pred)( void*, void*), void* env, struct Cyc_List_List* x)) Cyc_List_exists_c)(
Cyc_Tcutil_equal_att,( void*)(( struct Cyc_List_List*) _check_null( a))->hd, a2)){
return 0;}}}{ struct Cyc_List_List* a= a2; for( 0; a != 0; a=(( struct Cyc_List_List*)
_check_null( a))->tl){ if( !(( int(*)( int(* pred)( void*, void*), void* env,
struct Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcutil_equal_att,( void*)((
struct Cyc_List_List*) _check_null( a))->hd, a1)){ return 0;}}} return 1;}
static void Cyc_Tcutil_normalize_effect( void* e){ e= Cyc_Tcutil_compress( e);{
void* _temp460= e; struct Cyc_List_List* _temp466; struct Cyc_List_List**
_temp468; _LL462: if(( unsigned int) _temp460 > 4u?*(( int*) _temp460) == Cyc_Absyn_JoinEff:
0){ _LL467: _temp466=(( struct Cyc_Absyn_JoinEff_struct*) _temp460)->f1;
_temp468=&(( struct Cyc_Absyn_JoinEff_struct*) _temp460)->f1; goto _LL463;}
else{ goto _LL464;} _LL464: goto _LL465; _LL463: { int nested_join= 0;{ struct
Cyc_List_List* effs=* _temp468; for( 0; effs != 0; effs=(( struct Cyc_List_List*)
_check_null( effs))->tl){ void* _temp469=( void*)(( struct Cyc_List_List*)
_check_null( effs))->hd; goto _LL470; _LL470: Cyc_Tcutil_normalize_effect(
_temp469);{ void* _temp471= Cyc_Tcutil_compress( _temp469); struct Cyc_List_List*
_temp477; _LL473: if(( unsigned int) _temp471 > 4u?*(( int*) _temp471) == Cyc_Absyn_JoinEff:
0){ _LL478: _temp477=(( struct Cyc_Absyn_JoinEff_struct*) _temp471)->f1; goto
_LL474;} else{ goto _LL475;} _LL475: goto _LL476; _LL474: nested_join= 1; goto
_LL472; _LL476: goto _LL472; _LL472:;}}} if( ! nested_join){ return;}{ struct
Cyc_List_List* effects= 0;{ struct Cyc_List_List* effs=* _temp468; for( 0; effs
!= 0; effs=(( struct Cyc_List_List*) _check_null( effs))->tl){ void* _temp479=
Cyc_Tcutil_compress(( void*)(( struct Cyc_List_List*) _check_null( effs))->hd);
struct Cyc_List_List* _temp485; _LL481: if(( unsigned int) _temp479 > 4u?*(( int*)
_temp479) == Cyc_Absyn_JoinEff: 0){ _LL486: _temp485=(( struct Cyc_Absyn_JoinEff_struct*)
_temp479)->f1; goto _LL482;} else{ goto _LL483;} _LL483: goto _LL484; _LL482:
effects=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_revappend)( _temp485, effects); goto _LL480; _LL484: effects=({
struct Cyc_List_List* _temp487=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp487->hd=( void*) _temp479; _temp487->tl= effects;
_temp487;}); goto _LL480; _LL480:;}}* _temp468=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( effects); goto _LL461;}} _LL465:
goto _LL461; _LL461:;}} struct _tuple8{ void* f1; struct Cyc_List_List* f2; } ;
static struct _tuple8* Cyc_Tcutil_get_effect_evar( void* t){ void* _temp488= Cyc_Tcutil_compress(
t); struct Cyc_List_List* _temp496; struct Cyc_List_List _temp498; struct Cyc_List_List*
_temp499; void* _temp501; int _temp503; struct Cyc_Core_Opt* _temp505; struct
Cyc_Core_Opt** _temp507; void* _temp508; void** _temp510; _LL490: if((
unsigned int) _temp488 > 4u?*(( int*) _temp488) == Cyc_Absyn_JoinEff: 0){ _LL497:
_temp496=(( struct Cyc_Absyn_JoinEff_struct*) _temp488)->f1; if( _temp496 == 0){
goto _LL492;} else{ _temp498=* _temp496; _LL502: _temp501=( void*) _temp498.hd;
goto _LL500; _LL500: _temp499= _temp498.tl; goto _LL491;}} else{ goto _LL492;}
_LL492: if(( unsigned int) _temp488 > 4u?*(( int*) _temp488) == Cyc_Absyn_Evar:
0){ _LL509: _temp508=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp488)->f1;
_temp510=&(( void*)(( struct Cyc_Absyn_Evar_struct*) _temp488)->f1); goto _LL506;
_LL506: _temp505=(( struct Cyc_Absyn_Evar_struct*) _temp488)->f2; _temp507=&((
struct Cyc_Absyn_Evar_struct*) _temp488)->f2; goto _LL504; _LL504: _temp503=((
struct Cyc_Absyn_Evar_struct*) _temp488)->f3; goto _LL493;} else{ goto _LL494;}
_LL494: goto _LL495; _LL491: { void* _temp511= Cyc_Tcutil_compress( _temp501);
int _temp517; struct Cyc_Core_Opt* _temp519; void* _temp521; _LL513: if((
unsigned int) _temp511 > 4u?*(( int*) _temp511) == Cyc_Absyn_Evar: 0){ _LL522:
_temp521=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp511)->f1; goto _LL520;
_LL520: _temp519=(( struct Cyc_Absyn_Evar_struct*) _temp511)->f2; goto _LL518;
_LL518: _temp517=(( struct Cyc_Absyn_Evar_struct*) _temp511)->f3; goto _LL514;}
else{ goto _LL515;} _LL515: goto _LL516; _LL514: return({ struct _tuple8*
_temp523=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8)); _temp523->f1=
_temp501; _temp523->f2= _temp499; _temp523;}); _LL516: return 0; _LL512:;}
_LL493: if(* _temp510 !=( void*) Cyc_Absyn_EffKind){(( void(*)( struct
_tagged_string msg)) Cyc_Tcutil_impos)(({ char* _temp524="effect evar has wrong kind";
struct _tagged_string _temp525; _temp525.curr= _temp524; _temp525.base= _temp524;
_temp525.last_plus_one= _temp524 + 27; _temp525;}));} return({ struct _tuple8*
_temp526=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8)); _temp526->f1= t;
_temp526->f2= 0; _temp526;}); _LL495: return 0; _LL489:;} int Cyc_Tcutil_region_in_effect(
int constrain, void* r, void* e){ r= Cyc_Tcutil_compress( r); if( r ==( void*)
Cyc_Absyn_HeapRgn){ return 1;}{ void* _temp527= Cyc_Tcutil_compress( e); void*
_temp537; struct Cyc_List_List* _temp539; int _temp541; struct Cyc_Core_Opt*
_temp543; struct Cyc_Core_Opt** _temp545; void* _temp546; void** _temp548;
_LL529: if(( unsigned int) _temp527 > 4u?*(( int*) _temp527) == Cyc_Absyn_AccessEff:
0){ _LL538: _temp537=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp527)->f1;
goto _LL530;} else{ goto _LL531;} _LL531: if(( unsigned int) _temp527 > 4u?*((
int*) _temp527) == Cyc_Absyn_JoinEff: 0){ _LL540: _temp539=(( struct Cyc_Absyn_JoinEff_struct*)
_temp527)->f1; goto _LL532;} else{ goto _LL533;} _LL533: if(( unsigned int)
_temp527 > 4u?*(( int*) _temp527) == Cyc_Absyn_Evar: 0){ _LL547: _temp546=( void*)((
struct Cyc_Absyn_Evar_struct*) _temp527)->f1; _temp548=&(( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp527)->f1); goto _LL544; _LL544: _temp543=(( struct Cyc_Absyn_Evar_struct*)
_temp527)->f2; _temp545=&(( struct Cyc_Absyn_Evar_struct*) _temp527)->f2; goto
_LL542; _LL542: _temp541=(( struct Cyc_Absyn_Evar_struct*) _temp527)->f3; goto
_LL534;} else{ goto _LL535;} _LL535: goto _LL536; _LL530: if( constrain){ return
Cyc_Tcutil_unify( r, _temp537);} Cyc_Tcutil_compress( _temp537); if( r ==
_temp537){ return 1;}{ struct _tuple7 _temp550=({ struct _tuple7 _temp549;
_temp549.f1= r; _temp549.f2= _temp537; _temp549;}); void* _temp556; struct Cyc_Absyn_Tvar*
_temp558; void* _temp560; struct Cyc_Absyn_Tvar* _temp562; _LL552: _LL561:
_temp560= _temp550.f1; if(( unsigned int) _temp560 > 4u?*(( int*) _temp560) ==
Cyc_Absyn_VarType: 0){ _LL563: _temp562=(( struct Cyc_Absyn_VarType_struct*)
_temp560)->f1; goto _LL557;} else{ goto _LL554;} _LL557: _temp556= _temp550.f2;
if(( unsigned int) _temp556 > 4u?*(( int*) _temp556) == Cyc_Absyn_VarType: 0){
_LL559: _temp558=(( struct Cyc_Absyn_VarType_struct*) _temp556)->f1; goto _LL553;}
else{ goto _LL554;} _LL554: goto _LL555; _LL553: return Cyc_Absyn_tvar_cmp(
_temp562, _temp558) == 0; _LL555: return 0; _LL551:;} _LL532: for( 0; _temp539
!= 0; _temp539=(( struct Cyc_List_List*) _check_null( _temp539))->tl){ if( Cyc_Tcutil_region_in_effect(
constrain, r,( void*)(( struct Cyc_List_List*) _check_null( _temp539))->hd)){
return 1;}} return 0; _LL534: if(* _temp548 !=( void*) Cyc_Absyn_EffKind){((
void(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({ char* _temp564="effect evar has wrong kind";
struct _tagged_string _temp565; _temp565.curr= _temp564; _temp565.base= _temp564;
_temp565.last_plus_one= _temp564 + 27; _temp565;}));} if( ! constrain){ return 0;}{
void* _temp566= Cyc_Absyn_new_evar(( void*) Cyc_Absyn_EffKind); goto _LL567;
_LL567: { struct Cyc_Absyn_JoinEff_struct* _temp574=({ struct Cyc_Absyn_JoinEff_struct*
_temp568=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp568[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp569; _temp569.tag= Cyc_Absyn_JoinEff;
_temp569.f1=({ struct Cyc_List_List* _temp570=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp570->hd=( void*) _temp566; _temp570->tl=({
struct Cyc_List_List* _temp571=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp571->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp572=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp572[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp573; _temp573.tag= Cyc_Absyn_AccessEff;
_temp573.f1=( void*) r; _temp573;}); _temp572;})); _temp571->tl= 0; _temp571;});
_temp570;}); _temp569;}); _temp568;}); goto _LL575; _LL575:* _temp545=({ struct
Cyc_Core_Opt* _temp576=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp576->v=( void*)(( void*) _temp574); _temp576;}); return 1;}} _LL536: return
0; _LL528:;}} static int Cyc_Tcutil_variable_in_effect( int constrain, struct
Cyc_Absyn_Tvar* v, void* e){ e= Cyc_Tcutil_compress( e);{ void* _temp577= e;
struct Cyc_Absyn_Tvar* _temp587; struct Cyc_List_List* _temp589; int _temp591;
struct Cyc_Core_Opt* _temp593; struct Cyc_Core_Opt** _temp595; void* _temp596;
void** _temp598; _LL579: if(( unsigned int) _temp577 > 4u?*(( int*) _temp577) ==
Cyc_Absyn_VarType: 0){ _LL588: _temp587=(( struct Cyc_Absyn_VarType_struct*)
_temp577)->f1; goto _LL580;} else{ goto _LL581;} _LL581: if(( unsigned int)
_temp577 > 4u?*(( int*) _temp577) == Cyc_Absyn_JoinEff: 0){ _LL590: _temp589=((
struct Cyc_Absyn_JoinEff_struct*) _temp577)->f1; goto _LL582;} else{ goto _LL583;}
_LL583: if(( unsigned int) _temp577 > 4u?*(( int*) _temp577) == Cyc_Absyn_Evar:
0){ _LL597: _temp596=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp577)->f1;
_temp598=&(( void*)(( struct Cyc_Absyn_Evar_struct*) _temp577)->f1); goto _LL594;
_LL594: _temp593=(( struct Cyc_Absyn_Evar_struct*) _temp577)->f2; _temp595=&((
struct Cyc_Absyn_Evar_struct*) _temp577)->f2; goto _LL592; _LL592: _temp591=((
struct Cyc_Absyn_Evar_struct*) _temp577)->f3; goto _LL584;} else{ goto _LL585;}
_LL585: goto _LL586; _LL580: return Cyc_Absyn_tvar_cmp( v, _temp587) == 0;
_LL582: for( 0; _temp589 != 0; _temp589=(( struct Cyc_List_List*) _check_null(
_temp589))->tl){ if( Cyc_Tcutil_variable_in_effect( constrain, v,( void*)((
struct Cyc_List_List*) _check_null( _temp589))->hd)){ return 1;}} return 0;
_LL584: if(* _temp598 !=( void*) Cyc_Absyn_EffKind){(( void(*)( struct
_tagged_string msg)) Cyc_Tcutil_impos)(({ char* _temp599="effect evar has wrong kind";
struct _tagged_string _temp600; _temp600.curr= _temp599; _temp600.base= _temp599;
_temp600.last_plus_one= _temp599 + 27; _temp600;}));}{ void* _temp601= Cyc_Absyn_new_evar((
void*) Cyc_Absyn_EffKind); goto _LL602; _LL602: { struct Cyc_Absyn_JoinEff_struct*
_temp609=({ struct Cyc_Absyn_JoinEff_struct* _temp603=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp603[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp604; _temp604.tag= Cyc_Absyn_JoinEff; _temp604.f1=({ struct Cyc_List_List*
_temp605=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp605->hd=( void*) _temp601; _temp605->tl=({ struct Cyc_List_List* _temp606=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp606->hd=(
void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp607=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp607[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp608; _temp608.tag= Cyc_Absyn_VarType; _temp608.f1= v; _temp608;}); _temp607;}));
_temp606->tl= 0; _temp606;}); _temp605;}); _temp604;}); _temp603;}); goto _LL610;
_LL610:* _temp595=({ struct Cyc_Core_Opt* _temp611=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp611->v=( void*)(( void*) _temp609);
_temp611;}); return 1;}} _LL586: return 0; _LL578:;}} static int Cyc_Tcutil_evar_in_effect(
void* evar, void* e){ e= Cyc_Tcutil_compress( e);{ void* _temp612= e; struct Cyc_List_List*
_temp620; int _temp622; struct Cyc_Core_Opt* _temp624; void* _temp626; _LL614:
if(( unsigned int) _temp612 > 4u?*(( int*) _temp612) == Cyc_Absyn_JoinEff: 0){
_LL621: _temp620=(( struct Cyc_Absyn_JoinEff_struct*) _temp612)->f1; goto _LL615;}
else{ goto _LL616;} _LL616: if(( unsigned int) _temp612 > 4u?*(( int*) _temp612)
== Cyc_Absyn_Evar: 0){ _LL627: _temp626=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp612)->f1; goto _LL625; _LL625: _temp624=(( struct Cyc_Absyn_Evar_struct*)
_temp612)->f2; goto _LL623; _LL623: _temp622=(( struct Cyc_Absyn_Evar_struct*)
_temp612)->f3; goto _LL617;} else{ goto _LL618;} _LL618: goto _LL619; _LL615:
for( 0; _temp620 != 0; _temp620=(( struct Cyc_List_List*) _check_null( _temp620))->tl){
if( Cyc_Tcutil_evar_in_effect( evar,( void*)(( struct Cyc_List_List*)
_check_null( _temp620))->hd)){ return 1;}} return 0; _LL617: return evar == e;
_LL619: return 0; _LL613:;}} int Cyc_Tcutil_subset_effect( int set_to_empty,
void* e1, void* e2){{ struct _tuple8* _temp628= Cyc_Tcutil_get_effect_evar( e2);
struct _tuple8 _temp634; struct Cyc_List_List* _temp635; void* _temp637; _LL630:
if( _temp628 == 0){ goto _LL632;} else{ _temp634=* _temp628; _LL638: _temp637=
_temp634.f1; goto _LL636; _LL636: _temp635= _temp634.f2; goto _LL631;} _LL632:
goto _LL633; _LL631: { void* _temp639= Cyc_Absyn_new_evar(( void*) Cyc_Absyn_EffKind);
goto _LL640; _LL640: { struct Cyc_Absyn_JoinEff_struct* _temp645=({ struct Cyc_Absyn_JoinEff_struct*
_temp641=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp641[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp642; _temp642.tag= Cyc_Absyn_JoinEff;
_temp642.f1=({ struct Cyc_List_List* _temp643=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp643->hd=( void*) _temp639; _temp643->tl=({
struct Cyc_List_List* _temp644=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp644->hd=( void*) e1; _temp644->tl= 0; _temp644;});
_temp643;}); _temp642;}); _temp641;}); goto _LL646; _LL646: return Cyc_Tcutil_unify(
_temp637,( void*) _temp645);}} _LL633: goto _LL629; _LL629:;}{ void* _temp647=
Cyc_Tcutil_compress( e1); struct Cyc_List_List* _temp659; void* _temp661; struct
Cyc_Absyn_Tvar* _temp663; int _temp665; struct Cyc_Core_Opt* _temp667; struct
Cyc_Core_Opt** _temp669; void* _temp670; _LL649: if(( unsigned int) _temp647 > 4u?*((
int*) _temp647) == Cyc_Absyn_JoinEff: 0){ _LL660: _temp659=(( struct Cyc_Absyn_JoinEff_struct*)
_temp647)->f1; goto _LL650;} else{ goto _LL651;} _LL651: if(( unsigned int)
_temp647 > 4u?*(( int*) _temp647) == Cyc_Absyn_AccessEff: 0){ _LL662: _temp661=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp647)->f1; goto _LL652;} else{
goto _LL653;} _LL653: if(( unsigned int) _temp647 > 4u?*(( int*) _temp647) ==
Cyc_Absyn_VarType: 0){ _LL664: _temp663=(( struct Cyc_Absyn_VarType_struct*)
_temp647)->f1; goto _LL654;} else{ goto _LL655;} _LL655: if(( unsigned int)
_temp647 > 4u?*(( int*) _temp647) == Cyc_Absyn_Evar: 0){ _LL671: _temp670=( void*)((
struct Cyc_Absyn_Evar_struct*) _temp647)->f1; goto _LL668; _LL668: _temp667=((
struct Cyc_Absyn_Evar_struct*) _temp647)->f2; _temp669=&(( struct Cyc_Absyn_Evar_struct*)
_temp647)->f2; goto _LL666; _LL666: _temp665=(( struct Cyc_Absyn_Evar_struct*)
_temp647)->f3; goto _LL656;} else{ goto _LL657;} _LL657: goto _LL658; _LL650:
for( 0; _temp659 != 0; _temp659=(( struct Cyc_List_List*) _check_null( _temp659))->tl){
if( ! Cyc_Tcutil_subset_effect( set_to_empty,( void*)(( struct Cyc_List_List*)
_check_null( _temp659))->hd, e2)){ return 0;}} return 1; _LL652: return( Cyc_Tcutil_region_in_effect(
0, _temp661, e2)? 1: Cyc_Tcutil_region_in_effect( 1, _temp661, e2))? 1: Cyc_Tcutil_unify(
_temp661,( void*) Cyc_Absyn_HeapRgn); _LL654: return Cyc_Tcutil_variable_in_effect(
0, _temp663, e2)? 1: Cyc_Tcutil_variable_in_effect( 1, _temp663, e2); _LL656:
if( ! Cyc_Tcutil_evar_in_effect( e1, e2)){ if( set_to_empty){* _temp669=({
struct Cyc_Core_Opt* _temp672=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp672->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp673=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp673[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp674; _temp674.tag= Cyc_Absyn_JoinEff;
_temp674.f1= 0; _temp674;}); _temp673;})); _temp672;});} else{* _temp669=({
struct Cyc_Core_Opt* _temp675=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp675->v=( void*) e2; _temp675;});}} return 1; _LL658: return((
int(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({ struct _tagged_string
_temp676= Cyc_Absynpp_typ2string( e1); xprintf("subset_effect: bad effect: %.*s",
_temp676.last_plus_one - _temp676.curr, _temp676.curr);})); _LL648:;}} struct
_tuple9{ struct _tuple8* f1; struct _tuple8* f2; } ; static int Cyc_Tcutil_unify_effect(
void* e1, void* e2){ e1= Cyc_Tcutil_compress( e1); e2= Cyc_Tcutil_compress( e2);
Cyc_Tcutil_normalize_effect( e1); Cyc_Tcutil_normalize_effect( e2);{ struct
_tuple9 _temp678=({ struct _tuple9 _temp677; _temp677.f1= Cyc_Tcutil_get_effect_evar(
e1); _temp677.f2= Cyc_Tcutil_get_effect_evar( e2); _temp677;}); struct _tuple8*
_temp684; struct _tuple8 _temp686; struct Cyc_List_List* _temp687; void*
_temp689; struct _tuple8* _temp691; struct _tuple8 _temp693; struct Cyc_List_List*
_temp694; void* _temp696; _LL680: _LL692: _temp691= _temp678.f1; if( _temp691 ==
0){ goto _LL682;} else{ _temp693=* _temp691; _LL697: _temp696= _temp693.f1; goto
_LL695; _LL695: _temp694= _temp693.f2; goto _LL685;} _LL685: _temp684= _temp678.f2;
if( _temp684 == 0){ goto _LL682;} else{ _temp686=* _temp684; _LL690: _temp689=
_temp686.f1; goto _LL688; _LL688: _temp687= _temp686.f2; goto _LL681;} _LL682:
goto _LL683; _LL681: { void* _temp698= Cyc_Absyn_new_evar(( void*) Cyc_Absyn_EffKind);
goto _LL699; _LL699: { struct Cyc_Absyn_JoinEff_struct* _temp703=({ struct Cyc_Absyn_JoinEff_struct*
_temp700=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp700[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp701; _temp701.tag= Cyc_Absyn_JoinEff;
_temp701.f1=({ struct Cyc_List_List* _temp702=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp702->hd=( void*) _temp698; _temp702->tl=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp694, _temp687); _temp702;}); _temp701;}); _temp700;}); goto _LL704; _LL704:
return Cyc_Tcutil_unify( _temp696,( void*) _temp703)? Cyc_Tcutil_unify( _temp689,(
void*) _temp703): 0;}} _LL683: return Cyc_Tcutil_subset_effect( 0, e1, e2)? Cyc_Tcutil_subset_effect(
0, e2, e1): 0; _LL679:;}} struct _tuple10{ struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt*
f2; } ; void Cyc_Tcutil_unify_it( void* t1, void* t2){ t1= Cyc_Tcutil_compress(
t1); t2= Cyc_Tcutil_compress( t2); if( t1 == t2){ return;}{ void* _temp705= t1;
int _temp711; struct Cyc_Core_Opt* _temp713; struct Cyc_Core_Opt** _temp715;
void* _temp716; _LL707: if(( unsigned int) _temp705 > 4u?*(( int*) _temp705) ==
Cyc_Absyn_Evar: 0){ _LL717: _temp716=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp705)->f1; goto _LL714; _LL714: _temp713=(( struct Cyc_Absyn_Evar_struct*)
_temp705)->f2; _temp715=&(( struct Cyc_Absyn_Evar_struct*) _temp705)->f2; goto
_LL712; _LL712: _temp711=(( struct Cyc_Absyn_Evar_struct*) _temp705)->f3; goto
_LL708;} else{ goto _LL709;} _LL709: goto _LL710; _LL708: Cyc_Tcutil_occurs( t1,
t2);{ void* _temp718= Cyc_Tcutil_typ_kind( t2); goto _LL719; _LL719: if( Cyc_Tcutil_kind_leq(
_temp718, _temp716)){* _temp715=({ struct Cyc_Core_Opt* _temp720=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp720->v=( void*) t2; _temp720;});
return;} else{{ void* _temp721= t2; int _temp729; struct Cyc_Core_Opt* _temp731;
struct Cyc_Core_Opt** _temp733; void* _temp734; struct Cyc_Absyn_PtrInfo
_temp737; _LL723: if(( unsigned int) _temp721 > 4u?*(( int*) _temp721) == Cyc_Absyn_Evar:
0){ _LL735: _temp734=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp721)->f1;
goto _LL732; _LL732: _temp731=(( struct Cyc_Absyn_Evar_struct*) _temp721)->f2;
_temp733=&(( struct Cyc_Absyn_Evar_struct*) _temp721)->f2; goto _LL730; _LL730:
_temp729=(( struct Cyc_Absyn_Evar_struct*) _temp721)->f3; goto _LL724;} else{
goto _LL725;} _LL725: if(( unsigned int) _temp721 > 4u?*(( int*) _temp721) ==
Cyc_Absyn_PointerType: 0){ _LL738: _temp737=(( struct Cyc_Absyn_PointerType_struct*)
_temp721)->f1; goto _LL736;} else{ goto _LL727;} _LL736: if( _temp716 ==( void*)
Cyc_Absyn_BoxKind){ goto _LL726;} else{ goto _LL727;} _LL727: goto _LL728;
_LL724: if( Cyc_Tcutil_kind_leq( _temp716, _temp718)){* _temp733=({ struct Cyc_Core_Opt*
_temp739=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp739->v=( void*) t1; _temp739;}); return;} goto _LL722; _LL726: { struct Cyc_Absyn_Conref*
_temp740=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp737.bounds); goto _LL741; _LL741:{ void* _temp742=( void*) _temp740->v;
_LL744: if( _temp742 ==( void*) Cyc_Absyn_No_constr){ goto _LL745;} else{ goto
_LL746;} _LL746: goto _LL747; _LL745:( void*)( _temp740->v=( void*)(( void*)({
struct Cyc_Absyn_Eq_constr_struct* _temp748=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp748[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp749; _temp749.tag= Cyc_Absyn_Eq_constr; _temp749.f1=(
void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp750=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp750[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp751; _temp751.tag= Cyc_Absyn_Upper_b; _temp751.f1= Cyc_Absyn_signed_int_exp(
1, 0); _temp751;}); _temp750;})); _temp749;}); _temp748;})));* _temp715=({
struct Cyc_Core_Opt* _temp752=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp752->v=( void*) t2; _temp752;}); return; _LL747: goto
_LL743; _LL743:;} goto _LL722;} _LL728: goto _LL722; _LL722:;}( void) _throw((
void*) Cyc_Tcutil_Unify);}} _LL710: goto _LL706; _LL706:;}{ struct _tuple7
_temp754=({ struct _tuple7 _temp753; _temp753.f1= t2; _temp753.f2= t1; _temp753;});
void* _temp804; void* _temp806; int _temp808; struct Cyc_Core_Opt* _temp810;
void* _temp812; void* _temp814; void* _temp816; void* _temp818; struct Cyc_Absyn_Tvar*
_temp820; void* _temp822; struct Cyc_Absyn_Tvar* _temp824; void* _temp826;
struct Cyc_Absyn_Structdecl** _temp828; struct Cyc_List_List* _temp830; struct
_tuple1* _temp832; void* _temp834; struct Cyc_Absyn_Structdecl** _temp836;
struct Cyc_List_List* _temp838; struct _tuple1* _temp840; void* _temp842; struct
Cyc_Absyn_Enumdecl* _temp844; struct _tuple1* _temp846; void* _temp848; struct
Cyc_Absyn_Enumdecl* _temp850; struct _tuple1* _temp852; void* _temp854; struct
Cyc_Absyn_Uniondecl** _temp856; struct Cyc_List_List* _temp858; struct _tuple1*
_temp860; void* _temp862; struct Cyc_Absyn_Uniondecl** _temp864; struct Cyc_List_List*
_temp866; struct _tuple1* _temp868; void* _temp870; struct Cyc_Absyn_TunionInfo
_temp872; void* _temp874; struct Cyc_List_List* _temp876; void* _temp878; struct
Cyc_Absyn_Tuniondecl* _temp880; void* _temp882; struct Cyc_Absyn_TunionInfo
_temp884; void* _temp886; struct Cyc_List_List* _temp888; void* _temp890; struct
Cyc_Absyn_Tuniondecl* _temp892; void* _temp894; struct Cyc_Absyn_TunionFieldInfo
_temp896; struct Cyc_List_List* _temp898; void* _temp900; struct Cyc_Absyn_Tunionfield*
_temp902; struct Cyc_Absyn_Tuniondecl* _temp904; void* _temp906; struct Cyc_Absyn_TunionFieldInfo
_temp908; struct Cyc_List_List* _temp910; void* _temp912; struct Cyc_Absyn_Tunionfield*
_temp914; struct Cyc_Absyn_Tuniondecl* _temp916; void* _temp918; struct Cyc_Absyn_PtrInfo
_temp920; struct Cyc_Absyn_Conref* _temp922; struct Cyc_Absyn_Tqual _temp924;
struct Cyc_Absyn_Conref* _temp926; void* _temp928; void* _temp930; void*
_temp932; struct Cyc_Absyn_PtrInfo _temp934; struct Cyc_Absyn_Conref* _temp936;
struct Cyc_Absyn_Tqual _temp938; struct Cyc_Absyn_Conref* _temp940; void*
_temp942; void* _temp944; void* _temp946; void* _temp948; void* _temp950; void*
_temp952; void* _temp954; void* _temp956; void* _temp958; void* _temp960; void*
_temp962; void* _temp964; void* _temp966; struct Cyc_Absyn_Exp* _temp968; struct
Cyc_Absyn_Tqual _temp970; void* _temp972; void* _temp974; struct Cyc_Absyn_Exp*
_temp976; struct Cyc_Absyn_Tqual _temp978; void* _temp980; void* _temp982;
struct Cyc_Absyn_FnInfo _temp984; struct Cyc_List_List* _temp986; int _temp988;
struct Cyc_List_List* _temp990; void* _temp992; struct Cyc_Core_Opt* _temp994;
struct Cyc_List_List* _temp996; void* _temp998; struct Cyc_Absyn_FnInfo
_temp1000; struct Cyc_List_List* _temp1002; int _temp1004; struct Cyc_List_List*
_temp1006; void* _temp1008; struct Cyc_Core_Opt* _temp1010; struct Cyc_List_List*
_temp1012; void* _temp1014; struct Cyc_List_List* _temp1016; void* _temp1018;
struct Cyc_List_List* _temp1020; void* _temp1022; struct Cyc_List_List*
_temp1024; void* _temp1026; struct Cyc_List_List* _temp1028; void* _temp1030;
struct Cyc_List_List* _temp1032; void* _temp1034; struct Cyc_List_List*
_temp1036; void* _temp1038; void* _temp1040; void* _temp1042; void* _temp1044;
void* _temp1046; void* _temp1048; void* _temp1050; void* _temp1052; struct Cyc_List_List*
_temp1054; void* _temp1056; struct Cyc_List_List* _temp1058; void* _temp1060;
void* _temp1062; void* _temp1064; void* _temp1066; void* _temp1068; void*
_temp1070; void* _temp1072; _LL756: _LL807: _temp806= _temp754.f1; if((
unsigned int) _temp806 > 4u?*(( int*) _temp806) == Cyc_Absyn_Evar: 0){ _LL813:
_temp812=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp806)->f1; goto _LL811;
_LL811: _temp810=(( struct Cyc_Absyn_Evar_struct*) _temp806)->f2; goto _LL809;
_LL809: _temp808=(( struct Cyc_Absyn_Evar_struct*) _temp806)->f3; goto _LL805;}
else{ goto _LL758;} _LL805: _temp804= _temp754.f2; goto _LL757; _LL758: _LL817:
_temp816= _temp754.f1; if( _temp816 ==( void*) Cyc_Absyn_VoidType){ goto _LL815;}
else{ goto _LL760;} _LL815: _temp814= _temp754.f2; if( _temp814 ==( void*) Cyc_Absyn_VoidType){
goto _LL759;} else{ goto _LL760;} _LL760: _LL823: _temp822= _temp754.f1; if((
unsigned int) _temp822 > 4u?*(( int*) _temp822) == Cyc_Absyn_VarType: 0){ _LL825:
_temp824=(( struct Cyc_Absyn_VarType_struct*) _temp822)->f1; goto _LL819;} else{
goto _LL762;} _LL819: _temp818= _temp754.f2; if(( unsigned int) _temp818 > 4u?*((
int*) _temp818) == Cyc_Absyn_VarType: 0){ _LL821: _temp820=(( struct Cyc_Absyn_VarType_struct*)
_temp818)->f1; goto _LL761;} else{ goto _LL762;} _LL762: _LL835: _temp834=
_temp754.f1; if(( unsigned int) _temp834 > 4u?*(( int*) _temp834) == Cyc_Absyn_StructType:
0){ _LL841: _temp840=(( struct Cyc_Absyn_StructType_struct*) _temp834)->f1; goto
_LL839; _LL839: _temp838=(( struct Cyc_Absyn_StructType_struct*) _temp834)->f2;
goto _LL837; _LL837: _temp836=(( struct Cyc_Absyn_StructType_struct*) _temp834)->f3;
goto _LL827;} else{ goto _LL764;} _LL827: _temp826= _temp754.f2; if((
unsigned int) _temp826 > 4u?*(( int*) _temp826) == Cyc_Absyn_StructType: 0){
_LL833: _temp832=(( struct Cyc_Absyn_StructType_struct*) _temp826)->f1; goto
_LL831; _LL831: _temp830=(( struct Cyc_Absyn_StructType_struct*) _temp826)->f2;
goto _LL829; _LL829: _temp828=(( struct Cyc_Absyn_StructType_struct*) _temp826)->f3;
goto _LL763;} else{ goto _LL764;} _LL764: _LL849: _temp848= _temp754.f1; if((
unsigned int) _temp848 > 4u?*(( int*) _temp848) == Cyc_Absyn_EnumType: 0){
_LL853: _temp852=(( struct Cyc_Absyn_EnumType_struct*) _temp848)->f1; goto
_LL851; _LL851: _temp850=(( struct Cyc_Absyn_EnumType_struct*) _temp848)->f2;
goto _LL843;} else{ goto _LL766;} _LL843: _temp842= _temp754.f2; if((
unsigned int) _temp842 > 4u?*(( int*) _temp842) == Cyc_Absyn_EnumType: 0){
_LL847: _temp846=(( struct Cyc_Absyn_EnumType_struct*) _temp842)->f1; goto
_LL845; _LL845: _temp844=(( struct Cyc_Absyn_EnumType_struct*) _temp842)->f2;
goto _LL765;} else{ goto _LL766;} _LL766: _LL863: _temp862= _temp754.f1; if((
unsigned int) _temp862 > 4u?*(( int*) _temp862) == Cyc_Absyn_UnionType: 0){
_LL869: _temp868=(( struct Cyc_Absyn_UnionType_struct*) _temp862)->f1; goto
_LL867; _LL867: _temp866=(( struct Cyc_Absyn_UnionType_struct*) _temp862)->f2;
goto _LL865; _LL865: _temp864=(( struct Cyc_Absyn_UnionType_struct*) _temp862)->f3;
goto _LL855;} else{ goto _LL768;} _LL855: _temp854= _temp754.f2; if((
unsigned int) _temp854 > 4u?*(( int*) _temp854) == Cyc_Absyn_UnionType: 0){
_LL861: _temp860=(( struct Cyc_Absyn_UnionType_struct*) _temp854)->f1; goto
_LL859; _LL859: _temp858=(( struct Cyc_Absyn_UnionType_struct*) _temp854)->f2;
goto _LL857; _LL857: _temp856=(( struct Cyc_Absyn_UnionType_struct*) _temp854)->f3;
goto _LL767;} else{ goto _LL768;} _LL768: _LL883: _temp882= _temp754.f1; if((
unsigned int) _temp882 > 4u?*(( int*) _temp882) == Cyc_Absyn_TunionType: 0){
_LL885: _temp884=(( struct Cyc_Absyn_TunionType_struct*) _temp882)->f1; _LL891:
_temp890=( void*) _temp884.tunion_info; if(*(( int*) _temp890) == Cyc_Absyn_KnownTunion){
_LL893: _temp892=(( struct Cyc_Absyn_KnownTunion_struct*) _temp890)->f1; goto
_LL889;} else{ goto _LL770;} _LL889: _temp888= _temp884.targs; goto _LL887;
_LL887: _temp886=( void*) _temp884.rgn; goto _LL871;} else{ goto _LL770;} _LL871:
_temp870= _temp754.f2; if(( unsigned int) _temp870 > 4u?*(( int*) _temp870) ==
Cyc_Absyn_TunionType: 0){ _LL873: _temp872=(( struct Cyc_Absyn_TunionType_struct*)
_temp870)->f1; _LL879: _temp878=( void*) _temp872.tunion_info; if(*(( int*)
_temp878) == Cyc_Absyn_KnownTunion){ _LL881: _temp880=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp878)->f1; goto _LL877;} else{ goto _LL770;} _LL877: _temp876= _temp872.targs;
goto _LL875; _LL875: _temp874=( void*) _temp872.rgn; goto _LL769;} else{ goto
_LL770;} _LL770: _LL907: _temp906= _temp754.f1; if(( unsigned int) _temp906 > 4u?*((
int*) _temp906) == Cyc_Absyn_TunionFieldType: 0){ _LL909: _temp908=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp906)->f1; _LL913: _temp912=( void*) _temp908.field_info; if(*(( int*)
_temp912) == Cyc_Absyn_KnownTunionfield){ _LL917: _temp916=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp912)->f1; goto _LL915; _LL915: _temp914=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp912)->f2; goto _LL911;} else{ goto _LL772;} _LL911: _temp910= _temp908.targs;
goto _LL895;} else{ goto _LL772;} _LL895: _temp894= _temp754.f2; if((
unsigned int) _temp894 > 4u?*(( int*) _temp894) == Cyc_Absyn_TunionFieldType: 0){
_LL897: _temp896=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp894)->f1;
_LL901: _temp900=( void*) _temp896.field_info; if(*(( int*) _temp900) == Cyc_Absyn_KnownTunionfield){
_LL905: _temp904=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp900)->f1;
goto _LL903; _LL903: _temp902=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp900)->f2; goto _LL899;} else{ goto _LL772;} _LL899: _temp898= _temp896.targs;
goto _LL771;} else{ goto _LL772;} _LL772: _LL933: _temp932= _temp754.f1; if((
unsigned int) _temp932 > 4u?*(( int*) _temp932) == Cyc_Absyn_PointerType: 0){
_LL935: _temp934=(( struct Cyc_Absyn_PointerType_struct*) _temp932)->f1; _LL945:
_temp944=( void*) _temp934.elt_typ; goto _LL943; _LL943: _temp942=( void*)
_temp934.rgn_typ; goto _LL941; _LL941: _temp940= _temp934.nullable; goto _LL939;
_LL939: _temp938= _temp934.tq; goto _LL937; _LL937: _temp936= _temp934.bounds;
goto _LL919;} else{ goto _LL774;} _LL919: _temp918= _temp754.f2; if((
unsigned int) _temp918 > 4u?*(( int*) _temp918) == Cyc_Absyn_PointerType: 0){
_LL921: _temp920=(( struct Cyc_Absyn_PointerType_struct*) _temp918)->f1; _LL931:
_temp930=( void*) _temp920.elt_typ; goto _LL929; _LL929: _temp928=( void*)
_temp920.rgn_typ; goto _LL927; _LL927: _temp926= _temp920.nullable; goto _LL925;
_LL925: _temp924= _temp920.tq; goto _LL923; _LL923: _temp922= _temp920.bounds;
goto _LL773;} else{ goto _LL774;} _LL774: _LL953: _temp952= _temp754.f1; if((
unsigned int) _temp952 > 4u?*(( int*) _temp952) == Cyc_Absyn_IntType: 0){ _LL957:
_temp956=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp952)->f1; goto _LL955;
_LL955: _temp954=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp952)->f2;
goto _LL947;} else{ goto _LL776;} _LL947: _temp946= _temp754.f2; if((
unsigned int) _temp946 > 4u?*(( int*) _temp946) == Cyc_Absyn_IntType: 0){ _LL951:
_temp950=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp946)->f1; goto _LL949;
_LL949: _temp948=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp946)->f2;
goto _LL775;} else{ goto _LL776;} _LL776: _LL961: _temp960= _temp754.f1; if(
_temp960 ==( void*) Cyc_Absyn_FloatType){ goto _LL959;} else{ goto _LL778;}
_LL959: _temp958= _temp754.f2; if( _temp958 ==( void*) Cyc_Absyn_FloatType){
goto _LL777;} else{ goto _LL778;} _LL778: _LL965: _temp964= _temp754.f1; if(
_temp964 ==( void*) Cyc_Absyn_DoubleType){ goto _LL963;} else{ goto _LL780;}
_LL963: _temp962= _temp754.f2; if( _temp962 ==( void*) Cyc_Absyn_DoubleType){
goto _LL779;} else{ goto _LL780;} _LL780: _LL975: _temp974= _temp754.f1; if((
unsigned int) _temp974 > 4u?*(( int*) _temp974) == Cyc_Absyn_ArrayType: 0){
_LL981: _temp980=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp974)->f1;
goto _LL979; _LL979: _temp978=(( struct Cyc_Absyn_ArrayType_struct*) _temp974)->f2;
goto _LL977; _LL977: _temp976=(( struct Cyc_Absyn_ArrayType_struct*) _temp974)->f3;
goto _LL967;} else{ goto _LL782;} _LL967: _temp966= _temp754.f2; if((
unsigned int) _temp966 > 4u?*(( int*) _temp966) == Cyc_Absyn_ArrayType: 0){
_LL973: _temp972=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp966)->f1;
goto _LL971; _LL971: _temp970=(( struct Cyc_Absyn_ArrayType_struct*) _temp966)->f2;
goto _LL969; _LL969: _temp968=(( struct Cyc_Absyn_ArrayType_struct*) _temp966)->f3;
goto _LL781;} else{ goto _LL782;} _LL782: _LL999: _temp998= _temp754.f1; if((
unsigned int) _temp998 > 4u?*(( int*) _temp998) == Cyc_Absyn_FnType: 0){ _LL1001:
_temp1000=(( struct Cyc_Absyn_FnType_struct*) _temp998)->f1; _LL1013: _temp1012=
_temp1000.tvars; goto _LL1011; _LL1011: _temp1010= _temp1000.effect; goto
_LL1009; _LL1009: _temp1008=( void*) _temp1000.ret_typ; goto _LL1007; _LL1007:
_temp1006= _temp1000.args; goto _LL1005; _LL1005: _temp1004= _temp1000.varargs;
goto _LL1003; _LL1003: _temp1002= _temp1000.attributes; goto _LL983;} else{ goto
_LL784;} _LL983: _temp982= _temp754.f2; if(( unsigned int) _temp982 > 4u?*(( int*)
_temp982) == Cyc_Absyn_FnType: 0){ _LL985: _temp984=(( struct Cyc_Absyn_FnType_struct*)
_temp982)->f1; _LL997: _temp996= _temp984.tvars; goto _LL995; _LL995: _temp994=
_temp984.effect; goto _LL993; _LL993: _temp992=( void*) _temp984.ret_typ; goto
_LL991; _LL991: _temp990= _temp984.args; goto _LL989; _LL989: _temp988= _temp984.varargs;
goto _LL987; _LL987: _temp986= _temp984.attributes; goto _LL783;} else{ goto
_LL784;} _LL784: _LL1019: _temp1018= _temp754.f1; if(( unsigned int) _temp1018 >
4u?*(( int*) _temp1018) == Cyc_Absyn_TupleType: 0){ _LL1021: _temp1020=(( struct
Cyc_Absyn_TupleType_struct*) _temp1018)->f1; goto _LL1015;} else{ goto _LL786;}
_LL1015: _temp1014= _temp754.f2; if(( unsigned int) _temp1014 > 4u?*(( int*)
_temp1014) == Cyc_Absyn_TupleType: 0){ _LL1017: _temp1016=(( struct Cyc_Absyn_TupleType_struct*)
_temp1014)->f1; goto _LL785;} else{ goto _LL786;} _LL786: _LL1027: _temp1026=
_temp754.f1; if(( unsigned int) _temp1026 > 4u?*(( int*) _temp1026) == Cyc_Absyn_AnonStructType:
0){ _LL1029: _temp1028=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1026)->f1;
goto _LL1023;} else{ goto _LL788;} _LL1023: _temp1022= _temp754.f2; if((
unsigned int) _temp1022 > 4u?*(( int*) _temp1022) == Cyc_Absyn_AnonStructType: 0){
_LL1025: _temp1024=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1022)->f1;
goto _LL787;} else{ goto _LL788;} _LL788: _LL1035: _temp1034= _temp754.f1; if((
unsigned int) _temp1034 > 4u?*(( int*) _temp1034) == Cyc_Absyn_AnonUnionType: 0){
_LL1037: _temp1036=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1034)->f1;
goto _LL1031;} else{ goto _LL790;} _LL1031: _temp1030= _temp754.f2; if((
unsigned int) _temp1030 > 4u?*(( int*) _temp1030) == Cyc_Absyn_AnonUnionType: 0){
_LL1033: _temp1032=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1030)->f1;
goto _LL789;} else{ goto _LL790;} _LL790: _LL1041: _temp1040= _temp754.f1; if(
_temp1040 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL1039;} else{ goto _LL792;}
_LL1039: _temp1038= _temp754.f2; if( _temp1038 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL791;} else{ goto _LL792;} _LL792: _LL1047: _temp1046= _temp754.f1; if((
unsigned int) _temp1046 > 4u?*(( int*) _temp1046) == Cyc_Absyn_RgnHandleType: 0){
_LL1049: _temp1048=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp1046)->f1;
goto _LL1043;} else{ goto _LL794;} _LL1043: _temp1042= _temp754.f2; if((
unsigned int) _temp1042 > 4u?*(( int*) _temp1042) == Cyc_Absyn_RgnHandleType: 0){
_LL1045: _temp1044=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp1042)->f1;
goto _LL793;} else{ goto _LL794;} _LL794: _LL1053: _temp1052= _temp754.f1; if((
unsigned int) _temp1052 > 4u?*(( int*) _temp1052) == Cyc_Absyn_JoinEff: 0){
_LL1055: _temp1054=(( struct Cyc_Absyn_JoinEff_struct*) _temp1052)->f1; goto
_LL1051;} else{ goto _LL796;} _LL1051: _temp1050= _temp754.f2; goto _LL795;
_LL796: _LL1061: _temp1060= _temp754.f1; goto _LL1057; _LL1057: _temp1056=
_temp754.f2; if(( unsigned int) _temp1056 > 4u?*(( int*) _temp1056) == Cyc_Absyn_JoinEff:
0){ _LL1059: _temp1058=(( struct Cyc_Absyn_JoinEff_struct*) _temp1056)->f1; goto
_LL797;} else{ goto _LL798;} _LL798: _LL1065: _temp1064= _temp754.f1; if((
unsigned int) _temp1064 > 4u?*(( int*) _temp1064) == Cyc_Absyn_AccessEff: 0){
_LL1067: _temp1066=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp1064)->f1;
goto _LL1063;} else{ goto _LL800;} _LL1063: _temp1062= _temp754.f2; goto _LL799;
_LL800: _LL1073: _temp1072= _temp754.f1; goto _LL1069; _LL1069: _temp1068=
_temp754.f2; if(( unsigned int) _temp1068 > 4u?*(( int*) _temp1068) == Cyc_Absyn_AccessEff:
0){ _LL1071: _temp1070=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp1068)->f1;
goto _LL801;} else{ goto _LL802;} _LL802: goto _LL803; _LL757: Cyc_Tcutil_unify_it(
t2, t1); return; _LL759: return; _LL761: { struct _tagged_string* _temp1074=
_temp824->name; goto _LL1075; _LL1075: { struct _tagged_string* _temp1076=
_temp820->name; goto _LL1077; _LL1077: { void* _temp1078=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp824->kind); goto _LL1079; _LL1079: { void*
_temp1080=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp820->kind); goto _LL1081; _LL1081: if( Cyc_String_zstrptrcmp( _temp1074,
_temp1076) == 0){ if( _temp1078 != _temp1080){(( void(*)( struct _tagged_string
msg)) Cyc_Tcutil_impos)(({ struct _tagged_string _temp1082=* _temp1074; struct
_tagged_string _temp1083= Cyc_Absynpp_kind2string( _temp1078); struct
_tagged_string _temp1084= Cyc_Absynpp_kind2string( _temp1080); xprintf("same type variable %.*s has kinds %.*s and %.*s",
_temp1082.last_plus_one - _temp1082.curr, _temp1082.curr, _temp1083.last_plus_one
- _temp1083.curr, _temp1083.curr, _temp1084.last_plus_one - _temp1084.curr,
_temp1084.curr);}));} return;} goto _LL755;}}}} _LL763: if((( _temp832 != 0?
_temp840 != 0: 0)? Cyc_Absyn_qvar_cmp(( struct _tuple1*) _check_null( _temp832),(
struct _tuple1*) _check_null( _temp840)) == 0: 0)? 1:( _temp832 == 0? _temp840
== 0: 0)){ Cyc_Tcutil_unify_list( _temp830, _temp838); return;} goto _LL755;
_LL765: if( Cyc_Absyn_qvar_cmp( _temp852, _temp846) == 0){ return;} goto _LL755;
_LL767: if((( _temp860 != 0? _temp868 != 0: 0)? Cyc_Absyn_qvar_cmp(( struct
_tuple1*) _check_null( _temp860),( struct _tuple1*) _check_null( _temp868)) == 0:
0)? 1:( _temp860 == 0? _temp868 == 0: 0)){ Cyc_Tcutil_unify_list( _temp858,
_temp866); return;} goto _LL755; _LL769: if( _temp892 == _temp880? 1: Cyc_Absyn_qvar_cmp(
_temp892->name, _temp880->name) == 0){ Cyc_Tcutil_unify_it( _temp874, _temp886);
Cyc_Tcutil_unify_list( _temp876, _temp888); return;} goto _LL755; _LL771: if((
_temp916 == _temp904? 1: Cyc_Absyn_qvar_cmp( _temp916->name, _temp904->name) ==
0)? _temp914 == _temp902? 1: Cyc_Absyn_qvar_cmp( _temp914->name, _temp902->name)
== 0: 0){ Cyc_Tcutil_unify_list( _temp898, _temp910); return;} goto _LL755;
_LL773: Cyc_Tcutil_unify_it( _temp930, _temp944); Cyc_Tcutil_unify_it( _temp942,
_temp928); Cyc_Tcutil_unify_tqual( _temp924, _temp938);(( void(*)( int(* cmp)(
void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_it_conrefs)(
Cyc_Tcutil_boundscmp, _temp922, _temp936);{ void* _temp1085=( void*)((( struct
Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp922))->v; void* _temp1091; _LL1087: if(( unsigned int) _temp1085 > 1u?*((
int*) _temp1085) == Cyc_Absyn_Eq_constr: 0){ _LL1092: _temp1091=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp1085)->f1; if( _temp1091 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL1088;} else{ goto _LL1089;}} else{ goto _LL1089;} _LL1089: goto _LL1090;
_LL1088: return; _LL1090: goto _LL1086; _LL1086:;}(( void(*)( int(* cmp)( int,
int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_it_conrefs)(
Cyc_Core_intcmp, _temp926, _temp940); return; _LL775: if( _temp950 == _temp956?
_temp948 == _temp954: 0){ return;} goto _LL755; _LL777: return; _LL779: return;
_LL781: Cyc_Tcutil_unify_it( _temp972, _temp980); Cyc_Tcutil_unify_tqual(
_temp970, _temp978); if( _temp976 == _temp968){ return;} if( _temp976 == 0? 1:
_temp968 == 0){ goto _LL755;} if( Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp976)) == Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp968))){ return;} goto _LL755; _LL783: { int done= 0;{ struct
_RegionHandle _temp1093= _new_region(); struct _RegionHandle* rgn=& _temp1093;
_push_region( rgn);{ struct Cyc_List_List* inst= 0; while( _temp996 != 0) { if(
_temp1012 == 0){ break;} inst=({ struct Cyc_List_List* _temp1094=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp1094->hd=( void*)({
struct _tuple5* _temp1095=( struct _tuple5*) _region_malloc( rgn, sizeof( struct
_tuple5)); _temp1095->f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp1012))->hd; _temp1095->f2=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp1096=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp1096[ 0]=({ struct Cyc_Absyn_VarType_struct _temp1097; _temp1097.tag= Cyc_Absyn_VarType;
_temp1097.f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
_temp996))->hd; _temp1097;}); _temp1096;}); _temp1095;}); _temp1094->tl= inst;
_temp1094;}); _temp996=(( struct Cyc_List_List*) _check_null( _temp996))->tl;
_temp1012=(( struct Cyc_List_List*) _check_null( _temp1012))->tl;} if( _temp1012
!= 0){ _npop_handler( 0u); goto _LL755;} if( inst != 0){ Cyc_Tcutil_unify_it((
void*)({ struct Cyc_Absyn_FnType_struct* _temp1098=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1098[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1099; _temp1099.tag= Cyc_Absyn_FnType; _temp1099.f1=({ struct Cyc_Absyn_FnInfo
_temp1100; _temp1100.tvars= 0; _temp1100.effect= _temp994; _temp1100.ret_typ=(
void*) _temp992; _temp1100.args= _temp990; _temp1100.varargs= _temp988;
_temp1100.attributes= _temp986; _temp1100;}); _temp1099;}); _temp1098;}), Cyc_Tcutil_rsubstitute(
rgn, inst,( void*)({ struct Cyc_Absyn_FnType_struct* _temp1101=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1101[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1102; _temp1102.tag= Cyc_Absyn_FnType; _temp1102.f1=({ struct Cyc_Absyn_FnInfo
_temp1103; _temp1103.tvars= 0; _temp1103.effect= _temp1010; _temp1103.ret_typ=(
void*) _temp1008; _temp1103.args= _temp1006; _temp1103.varargs= _temp1004;
_temp1103.attributes= _temp1002; _temp1103;}); _temp1102;}); _temp1101;})));
done= 1;}}; _pop_region(& _temp1093);} if( done){ return;} Cyc_Tcutil_unify_it(
_temp992, _temp1008); for( 0; _temp990 != 0? _temp1006 != 0: 0;( _temp990=((
struct Cyc_List_List*) _check_null( _temp990))->tl, _temp1006=(( struct Cyc_List_List*)
_check_null( _temp1006))->tl)){ Cyc_Tcutil_unify_tqual((*(( struct _tuple2*)((
struct Cyc_List_List*) _check_null( _temp990))->hd)).f2,(*(( struct _tuple2*)((
struct Cyc_List_List*) _check_null( _temp1006))->hd)).f2); Cyc_Tcutil_unify_it((*((
struct _tuple2*)(( struct Cyc_List_List*) _check_null( _temp990))->hd)).f3,(*((
struct _tuple2*)(( struct Cyc_List_List*) _check_null( _temp1006))->hd)).f3);}
if( _temp990 != 0? 1: _temp1006 != 0){ goto _LL755;} if( _temp988 != _temp1004){
goto _LL755;}{ int bad_effect= 0;{ struct _tuple10 _temp1105=({ struct _tuple10
_temp1104; _temp1104.f1= _temp994; _temp1104.f2= _temp1010; _temp1104;}); struct
Cyc_Core_Opt* _temp1115; struct Cyc_Core_Opt* _temp1117; struct Cyc_Core_Opt*
_temp1119; struct Cyc_Core_Opt* _temp1121; struct Cyc_Core_Opt* _temp1123;
struct Cyc_Core_Opt* _temp1125; _LL1107: _LL1118: _temp1117= _temp1105.f1; if(
_temp1117 == 0){ goto _LL1116;} else{ goto _LL1109;} _LL1116: _temp1115=
_temp1105.f2; if( _temp1115 == 0){ goto _LL1108;} else{ goto _LL1109;} _LL1109:
_LL1122: _temp1121= _temp1105.f1; if( _temp1121 == 0){ goto _LL1120;} else{ goto
_LL1111;} _LL1120: _temp1119= _temp1105.f2; goto _LL1110; _LL1111: _LL1126:
_temp1125= _temp1105.f1; goto _LL1124; _LL1124: _temp1123= _temp1105.f2; if(
_temp1123 == 0){ goto _LL1112;} else{ goto _LL1113;} _LL1113: goto _LL1114;
_LL1108: goto _LL1106; _LL1110: goto _LL1112; _LL1112: bad_effect= 1; goto
_LL1106; _LL1114: bad_effect= ! Cyc_Tcutil_unify_effect(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp994))->v,( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp1010))->v); goto _LL1106; _LL1106:;} if( bad_effect){ goto _LL755;} if( !
Cyc_Tcutil_same_atts( _temp1002, _temp986)){ goto _LL755;} return;}} _LL785:
for( 0; _temp1016 != 0? _temp1020 != 0: 0;( _temp1016=(( struct Cyc_List_List*)
_check_null( _temp1016))->tl, _temp1020=(( struct Cyc_List_List*) _check_null(
_temp1020))->tl)){ Cyc_Tcutil_unify_tqual((*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1016))->hd)).f1,(*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp1020))->hd)).f1); Cyc_Tcutil_unify_it((*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp1016))->hd)).f2,(*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp1020))->hd)).f2);} if( _temp1016 == 0?
_temp1020 == 0: 0){ return;} goto _LL755; _LL787: _temp1036= _temp1028;
_temp1032= _temp1024; goto _LL789; _LL789: for( 0; _temp1032 != 0? _temp1036 !=
0: 0;( _temp1032=(( struct Cyc_List_List*) _check_null( _temp1032))->tl,
_temp1036=(( struct Cyc_List_List*) _check_null( _temp1036))->tl)){ struct Cyc_Absyn_Structfield*
_temp1127=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp1032))->hd; goto _LL1128; _LL1128: { struct Cyc_Absyn_Structfield*
_temp1129=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp1036))->hd; goto _LL1130; _LL1130: if( Cyc_String_zstrptrcmp( _temp1127->name,
_temp1129->name) != 0){( void) _throw(( void*) Cyc_Tcutil_Unify);} Cyc_Tcutil_unify_tqual(
_temp1127->tq, _temp1129->tq); Cyc_Tcutil_unify_it(( void*) _temp1127->type,(
void*) _temp1129->type); if((( ! Cyc_Tcutil_same_atts( _temp1127->attributes,
_temp1129->attributes)? 1:( _temp1127->width != 0? _temp1129->width == 0: 0))? 1:(
_temp1129->width != 0? _temp1127->width == 0: 0))? 1:(( _temp1127->width != 0?
_temp1129->width != 0: 0)? Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( _temp1127->width))->v) != Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp1129->width))->v):
0)){( void) _throw(( void*) Cyc_Tcutil_Unify);}}} if( _temp1032 == 0? _temp1036
== 0: 0){ return;} goto _LL755; _LL791: return; _LL793: Cyc_Tcutil_unify_it(
_temp1048, _temp1044); return; _LL795: goto _LL797; _LL797: goto _LL799; _LL799:
goto _LL801; _LL801: if( Cyc_Tcutil_unify_effect( t1, t2)){ return;} else{ goto
_LL755;} _LL803: goto _LL755; _LL755:;}( void) _throw(( void*) Cyc_Tcutil_Unify);}
int Cyc_Tcutil_is_arithmetic_type( void* t){ void* _temp1131= Cyc_Tcutil_compress(
t); void* _temp1143; void* _temp1145; struct Cyc_Absyn_Enumdecl* _temp1147;
struct _tuple1* _temp1149; _LL1133: if(( unsigned int) _temp1131 > 4u?*(( int*)
_temp1131) == Cyc_Absyn_IntType: 0){ _LL1146: _temp1145=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1131)->f1; goto _LL1144; _LL1144: _temp1143=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1131)->f2; goto _LL1134;} else{ goto _LL1135;} _LL1135: if( _temp1131 ==(
void*) Cyc_Absyn_FloatType){ goto _LL1136;} else{ goto _LL1137;} _LL1137: if(
_temp1131 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1138;} else{ goto _LL1139;}
_LL1139: if(( unsigned int) _temp1131 > 4u?*(( int*) _temp1131) == Cyc_Absyn_EnumType:
0){ _LL1150: _temp1149=(( struct Cyc_Absyn_EnumType_struct*) _temp1131)->f1;
goto _LL1148; _LL1148: _temp1147=(( struct Cyc_Absyn_EnumType_struct*) _temp1131)->f2;
goto _LL1140;} else{ goto _LL1141;} _LL1141: goto _LL1142; _LL1134: return 1;
_LL1136: return 1; _LL1138: return 1; _LL1140: return 1; _LL1142: return 0;
_LL1132:;} int Cyc_Tcutil_will_lose_precision( void* t1, void* t2){ t1= Cyc_Tcutil_compress(
t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple7 _temp1152=({ struct _tuple7
_temp1151; _temp1151.f1= t1; _temp1151.f2= t2; _temp1151;}); void* _temp1174;
void* _temp1176; void* _temp1178; void* _temp1180; void* _temp1182; void*
_temp1184; void* _temp1186; void* _temp1188; void* _temp1190; void* _temp1192;
void* _temp1194; void* _temp1196; void* _temp1198; void* _temp1200; void*
_temp1202; void* _temp1204; void* _temp1206; void* _temp1208; void* _temp1210;
void* _temp1212; void* _temp1214; void* _temp1216; void* _temp1218; void*
_temp1220; void* _temp1222; void* _temp1224; void* _temp1226; void* _temp1228;
void* _temp1230; void* _temp1232; void* _temp1234; void* _temp1236; void*
_temp1238; void* _temp1240; void* _temp1242; void* _temp1244; void* _temp1246;
void* _temp1248; void* _temp1250; void* _temp1252; void* _temp1254; void*
_temp1256; _LL1154: _LL1177: _temp1176= _temp1152.f1; if( _temp1176 ==( void*)
Cyc_Absyn_DoubleType){ goto _LL1175;} else{ goto _LL1156;} _LL1175: _temp1174=
_temp1152.f2; if( _temp1174 ==( void*) Cyc_Absyn_FloatType){ goto _LL1155;}
else{ goto _LL1156;} _LL1156: _LL1185: _temp1184= _temp1152.f1; if( _temp1184 ==(
void*) Cyc_Absyn_DoubleType){ goto _LL1179;} else{ goto _LL1158;} _LL1179:
_temp1178= _temp1152.f2; if(( unsigned int) _temp1178 > 4u?*(( int*) _temp1178)
== Cyc_Absyn_IntType: 0){ _LL1183: _temp1182=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1178)->f1; goto _LL1181; _LL1181: _temp1180=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1178)->f2; goto _LL1157;} else{ goto _LL1158;} _LL1158: _LL1193: _temp1192=
_temp1152.f1; if( _temp1192 ==( void*) Cyc_Absyn_FloatType){ goto _LL1187;}
else{ goto _LL1160;} _LL1187: _temp1186= _temp1152.f2; if(( unsigned int)
_temp1186 > 4u?*(( int*) _temp1186) == Cyc_Absyn_IntType: 0){ _LL1191: _temp1190=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1186)->f1; goto _LL1189; _LL1189:
_temp1188=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1186)->f2; goto
_LL1159;} else{ goto _LL1160;} _LL1160: _LL1201: _temp1200= _temp1152.f1; if((
unsigned int) _temp1200 > 4u?*(( int*) _temp1200) == Cyc_Absyn_IntType: 0){
_LL1205: _temp1204=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1200)->f1;
goto _LL1203; _LL1203: _temp1202=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1200)->f2; if( _temp1202 ==( void*) Cyc_Absyn_B8){ goto _LL1195;} else{
goto _LL1162;}} else{ goto _LL1162;} _LL1195: _temp1194= _temp1152.f2; if((
unsigned int) _temp1194 > 4u?*(( int*) _temp1194) == Cyc_Absyn_IntType: 0){
_LL1199: _temp1198=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1194)->f1;
goto _LL1197; _LL1197: _temp1196=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1194)->f2; if( _temp1196 ==( void*) Cyc_Absyn_B8){ goto _LL1161;} else{
goto _LL1162;}} else{ goto _LL1162;} _LL1162: _LL1209: _temp1208= _temp1152.f1;
if(( unsigned int) _temp1208 > 4u?*(( int*) _temp1208) == Cyc_Absyn_IntType: 0){
_LL1213: _temp1212=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1208)->f1;
goto _LL1211; _LL1211: _temp1210=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1208)->f2; if( _temp1210 ==( void*) Cyc_Absyn_B8){ goto _LL1207;} else{
goto _LL1164;}} else{ goto _LL1164;} _LL1207: _temp1206= _temp1152.f2; goto
_LL1163; _LL1164: _LL1217: _temp1216= _temp1152.f1; if(( unsigned int) _temp1216
> 4u?*(( int*) _temp1216) == Cyc_Absyn_IntType: 0){ _LL1221: _temp1220=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp1216)->f1; goto _LL1219; _LL1219:
_temp1218=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1216)->f2; if(
_temp1218 ==( void*) Cyc_Absyn_B4){ goto _LL1215;} else{ goto _LL1166;}} else{
goto _LL1166;} _LL1215: _temp1214= _temp1152.f2; if( _temp1214 ==( void*) Cyc_Absyn_FloatType){
goto _LL1165;} else{ goto _LL1166;} _LL1166: _LL1229: _temp1228= _temp1152.f1;
if(( unsigned int) _temp1228 > 4u?*(( int*) _temp1228) == Cyc_Absyn_IntType: 0){
_LL1233: _temp1232=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1228)->f1;
goto _LL1231; _LL1231: _temp1230=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1228)->f2; if( _temp1230 ==( void*) Cyc_Absyn_B4){ goto _LL1223;} else{
goto _LL1168;}} else{ goto _LL1168;} _LL1223: _temp1222= _temp1152.f2; if((
unsigned int) _temp1222 > 4u?*(( int*) _temp1222) == Cyc_Absyn_IntType: 0){
_LL1227: _temp1226=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1222)->f1;
goto _LL1225; _LL1225: _temp1224=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1222)->f2; if( _temp1224 ==( void*) Cyc_Absyn_B2){ goto _LL1167;} else{
goto _LL1168;}} else{ goto _LL1168;} _LL1168: _LL1241: _temp1240= _temp1152.f1;
if(( unsigned int) _temp1240 > 4u?*(( int*) _temp1240) == Cyc_Absyn_IntType: 0){
_LL1245: _temp1244=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1240)->f1;
goto _LL1243; _LL1243: _temp1242=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1240)->f2; if( _temp1242 ==( void*) Cyc_Absyn_B4){ goto _LL1235;} else{
goto _LL1170;}} else{ goto _LL1170;} _LL1235: _temp1234= _temp1152.f2; if((
unsigned int) _temp1234 > 4u?*(( int*) _temp1234) == Cyc_Absyn_IntType: 0){
_LL1239: _temp1238=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1234)->f1;
goto _LL1237; _LL1237: _temp1236=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1234)->f2; if( _temp1236 ==( void*) Cyc_Absyn_B1){ goto _LL1169;} else{
goto _LL1170;}} else{ goto _LL1170;} _LL1170: _LL1253: _temp1252= _temp1152.f1;
if(( unsigned int) _temp1252 > 4u?*(( int*) _temp1252) == Cyc_Absyn_IntType: 0){
_LL1257: _temp1256=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1252)->f1;
goto _LL1255; _LL1255: _temp1254=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1252)->f2; if( _temp1254 ==( void*) Cyc_Absyn_B2){ goto _LL1247;} else{
goto _LL1172;}} else{ goto _LL1172;} _LL1247: _temp1246= _temp1152.f2; if((
unsigned int) _temp1246 > 4u?*(( int*) _temp1246) == Cyc_Absyn_IntType: 0){
_LL1251: _temp1250=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1246)->f1;
goto _LL1249; _LL1249: _temp1248=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1246)->f2; if( _temp1248 ==( void*) Cyc_Absyn_B1){ goto _LL1171;} else{
goto _LL1172;}} else{ goto _LL1172;} _LL1172: goto _LL1173; _LL1155: return 1;
_LL1157: return 1; _LL1159: return 1; _LL1161: return 0; _LL1163: return 1;
_LL1165: return 1; _LL1167: return 1; _LL1169: return 1; _LL1171: return 1;
_LL1173: return 0; _LL1153:;}} int Cyc_Tcutil_coerce_list( struct Cyc_Tcenv_Tenv*
te, void* t, struct Cyc_List_List* es){ struct Cyc_Core_Opt* max_arith_type= 0;{
struct Cyc_List_List* el= es; for( 0; el != 0; el=(( struct Cyc_List_List*)
_check_null( el))->tl){ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( el))->hd)->topt))->v);
if( Cyc_Tcutil_is_arithmetic_type( t1)){ if( max_arith_type == 0? 1: Cyc_Tcutil_will_lose_precision(
t1,( void*)(( struct Cyc_Core_Opt*) _check_null( max_arith_type))->v)){
max_arith_type=({ struct Cyc_Core_Opt* _temp1258=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1258->v=( void*) t1; _temp1258;});}}}}
if( max_arith_type != 0){ if( ! Cyc_Tcutil_unify( t,( void*)(( struct Cyc_Core_Opt*)
_check_null( max_arith_type))->v)){ return 0;}}{ struct Cyc_List_List* el= es;
for( 0; el != 0; el=(( struct Cyc_List_List*) _check_null( el))->tl){ if( ! Cyc_Tcutil_coerce_assign(
te,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( el))->hd, t)){
Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
el))->hd)->loc,({ struct _tagged_string _temp1259= Cyc_Absynpp_typ2string( t);
struct _tagged_string _temp1260= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( el))->hd)->topt))->v);
xprintf("type mismatch: expecting %.*s but found %.*s", _temp1259.last_plus_one
- _temp1259.curr, _temp1259.curr, _temp1260.last_plus_one - _temp1260.curr,
_temp1260.curr);})); return 0;}}} return 1;} int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( ! Cyc_Tcutil_coerce_sint_typ(
te, e)){ void* _temp1261= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); struct Cyc_Absyn_PtrInfo _temp1267; _LL1263: if((
unsigned int) _temp1261 > 4u?*(( int*) _temp1261) == Cyc_Absyn_PointerType: 0){
_LL1268: _temp1267=(( struct Cyc_Absyn_PointerType_struct*) _temp1261)->f1; goto
_LL1264;} else{ goto _LL1265;} _LL1265: goto _LL1266; _LL1264: Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_uint_t); goto _LL1262; _LL1266: return 0; _LL1262:;} return 1;}
int Cyc_Tcutil_is_integral_type( void* t){ void* _temp1269= Cyc_Tcutil_compress(
t); void* _temp1277; void* _temp1279; struct Cyc_Absyn_Enumdecl* _temp1281;
struct _tuple1* _temp1283; _LL1271: if(( unsigned int) _temp1269 > 4u?*(( int*)
_temp1269) == Cyc_Absyn_IntType: 0){ _LL1280: _temp1279=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1269)->f1; goto _LL1278; _LL1278: _temp1277=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1269)->f2; goto _LL1272;} else{ goto _LL1273;} _LL1273: if(( unsigned int)
_temp1269 > 4u?*(( int*) _temp1269) == Cyc_Absyn_EnumType: 0){ _LL1284:
_temp1283=(( struct Cyc_Absyn_EnumType_struct*) _temp1269)->f1; goto _LL1282;
_LL1282: _temp1281=(( struct Cyc_Absyn_EnumType_struct*) _temp1269)->f2; goto
_LL1274;} else{ goto _LL1275;} _LL1275: goto _LL1276; _LL1272: return 1; _LL1274:
return 1; _LL1276: return 0; _LL1270:;} int Cyc_Tcutil_coerce_uint_typ( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_uint_t)){ return 1;}
if( Cyc_Tcutil_is_integral_type(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)){
if( Cyc_Tcutil_will_lose_precision(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v, Cyc_Absyn_uint_t)){ Cyc_Tcutil_warn( e->loc,({ char* _temp1285="integral size mismatch; conversion supplied";
struct _tagged_string _temp1286; _temp1286.curr= _temp1285; _temp1286.base=
_temp1285; _temp1286.last_plus_one= _temp1285 + 44; _temp1286;}));} Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_uint_t); return 1;} return 0;} int Cyc_Tcutil_coerce_sint_typ(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_sint_t)){ return 1;}
if( Cyc_Tcutil_is_integral_type(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)){
if( Cyc_Tcutil_will_lose_precision(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v, Cyc_Absyn_sint_t)){ Cyc_Tcutil_warn( e->loc,({ char* _temp1287="integral size mismatch; conversion supplied";
struct _tagged_string _temp1288; _temp1288.curr= _temp1287; _temp1288.base=
_temp1287; _temp1288.last_plus_one= _temp1287 + 44; _temp1288;}));} Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_sint_t); return 1;} return 0;} int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* t1, void* t2){
t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple7
_temp1290=({ struct _tuple7 _temp1289; _temp1289.f1= t1; _temp1289.f2= t2;
_temp1289;}); void* _temp1302; struct Cyc_Absyn_PtrInfo _temp1304; void*
_temp1306; struct Cyc_Absyn_PtrInfo _temp1308; void* _temp1310; struct Cyc_Absyn_Exp*
_temp1312; struct Cyc_Absyn_Tqual _temp1314; void* _temp1316; void* _temp1318;
struct Cyc_Absyn_Exp* _temp1320; struct Cyc_Absyn_Tqual _temp1322; void*
_temp1324; void* _temp1326; struct Cyc_Absyn_TunionInfo _temp1328; void*
_temp1330; struct Cyc_List_List* _temp1332; void* _temp1334; struct Cyc_Absyn_Tuniondecl*
_temp1336; void* _temp1338; struct Cyc_Absyn_TunionFieldInfo _temp1340; struct
Cyc_List_List* _temp1342; void* _temp1344; struct Cyc_Absyn_Tunionfield*
_temp1346; struct Cyc_Absyn_Tuniondecl* _temp1348; void* _temp1350; struct Cyc_Absyn_TunionInfo
_temp1352; void* _temp1354; struct Cyc_List_List* _temp1356; void* _temp1358;
struct Cyc_Absyn_Tuniondecl* _temp1360; void* _temp1362; struct Cyc_Absyn_PtrInfo
_temp1364; struct Cyc_Absyn_Conref* _temp1366; struct Cyc_Absyn_Tqual _temp1368;
struct Cyc_Absyn_Conref* _temp1370; void* _temp1372; void* _temp1374; _LL1292:
_LL1307: _temp1306= _temp1290.f1; if(( unsigned int) _temp1306 > 4u?*(( int*)
_temp1306) == Cyc_Absyn_PointerType: 0){ _LL1309: _temp1308=(( struct Cyc_Absyn_PointerType_struct*)
_temp1306)->f1; goto _LL1303;} else{ goto _LL1294;} _LL1303: _temp1302=
_temp1290.f2; if(( unsigned int) _temp1302 > 4u?*(( int*) _temp1302) == Cyc_Absyn_PointerType:
0){ _LL1305: _temp1304=(( struct Cyc_Absyn_PointerType_struct*) _temp1302)->f1;
goto _LL1293;} else{ goto _LL1294;} _LL1294: _LL1319: _temp1318= _temp1290.f1;
if(( unsigned int) _temp1318 > 4u?*(( int*) _temp1318) == Cyc_Absyn_ArrayType: 0){
_LL1325: _temp1324=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1318)->f1;
goto _LL1323; _LL1323: _temp1322=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1318)->f2; goto _LL1321; _LL1321: _temp1320=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1318)->f3; goto _LL1311;} else{ goto _LL1296;} _LL1311: _temp1310=
_temp1290.f2; if(( unsigned int) _temp1310 > 4u?*(( int*) _temp1310) == Cyc_Absyn_ArrayType:
0){ _LL1317: _temp1316=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1310)->f1;
goto _LL1315; _LL1315: _temp1314=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1310)->f2; goto _LL1313; _LL1313: _temp1312=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1310)->f3; goto _LL1295;} else{ goto _LL1296;} _LL1296: _LL1339: _temp1338=
_temp1290.f1; if(( unsigned int) _temp1338 > 4u?*(( int*) _temp1338) == Cyc_Absyn_TunionFieldType:
0){ _LL1341: _temp1340=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1338)->f1;
_LL1345: _temp1344=( void*) _temp1340.field_info; if(*(( int*) _temp1344) == Cyc_Absyn_KnownTunionfield){
_LL1349: _temp1348=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp1344)->f1;
goto _LL1347; _LL1347: _temp1346=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1344)->f2; goto _LL1343;} else{ goto _LL1298;} _LL1343: _temp1342=
_temp1340.targs; goto _LL1327;} else{ goto _LL1298;} _LL1327: _temp1326=
_temp1290.f2; if(( unsigned int) _temp1326 > 4u?*(( int*) _temp1326) == Cyc_Absyn_TunionType:
0){ _LL1329: _temp1328=(( struct Cyc_Absyn_TunionType_struct*) _temp1326)->f1;
_LL1335: _temp1334=( void*) _temp1328.tunion_info; if(*(( int*) _temp1334) ==
Cyc_Absyn_KnownTunion){ _LL1337: _temp1336=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1334)->f1; goto _LL1333;} else{ goto _LL1298;} _LL1333: _temp1332=
_temp1328.targs; goto _LL1331; _LL1331: _temp1330=( void*) _temp1328.rgn; goto
_LL1297;} else{ goto _LL1298;} _LL1298: _LL1363: _temp1362= _temp1290.f1; if((
unsigned int) _temp1362 > 4u?*(( int*) _temp1362) == Cyc_Absyn_PointerType: 0){
_LL1365: _temp1364=(( struct Cyc_Absyn_PointerType_struct*) _temp1362)->f1;
_LL1375: _temp1374=( void*) _temp1364.elt_typ; goto _LL1373; _LL1373: _temp1372=(
void*) _temp1364.rgn_typ; goto _LL1371; _LL1371: _temp1370= _temp1364.nullable;
goto _LL1369; _LL1369: _temp1368= _temp1364.tq; goto _LL1367; _LL1367: _temp1366=
_temp1364.bounds; goto _LL1351;} else{ goto _LL1300;} _LL1351: _temp1350=
_temp1290.f2; if(( unsigned int) _temp1350 > 4u?*(( int*) _temp1350) == Cyc_Absyn_TunionType:
0){ _LL1353: _temp1352=(( struct Cyc_Absyn_TunionType_struct*) _temp1350)->f1;
_LL1359: _temp1358=( void*) _temp1352.tunion_info; if(*(( int*) _temp1358) ==
Cyc_Absyn_KnownTunion){ _LL1361: _temp1360=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1358)->f1; goto _LL1357;} else{ goto _LL1300;} _LL1357: _temp1356=
_temp1352.targs; goto _LL1355; _LL1355: _temp1354=( void*) _temp1352.rgn; goto
_LL1299;} else{ goto _LL1300;} _LL1300: goto _LL1301; _LL1293: { int okay= 1;
if( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp, _temp1308.nullable, _temp1304.nullable)){
void* _temp1376=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp1308.nullable))->v; int _temp1382; _LL1378:
if(( unsigned int) _temp1376 > 1u?*(( int*) _temp1376) == Cyc_Absyn_Eq_constr: 0){
_LL1383: _temp1382=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1376)->f1;
goto _LL1379;} else{ goto _LL1380;} _LL1380: goto _LL1381; _LL1379: okay= !
_temp1382; goto _LL1377; _LL1381:( void) _throw((( void*(*)( struct
_tagged_string msg)) Cyc_Tcutil_impos)(({ char* _temp1384="silent_castable conref not eq";
struct _tagged_string _temp1385; _temp1385.curr= _temp1384; _temp1385.base=
_temp1384; _temp1385.last_plus_one= _temp1384 + 30; _temp1385;}))); _LL1377:;}
if( !(( int(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x, struct
Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Tcutil_boundscmp, _temp1308.bounds,
_temp1304.bounds)){ struct _tuple7 _temp1387=({ struct _tuple7 _temp1386;
_temp1386.f1=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)( _temp1308.bounds))->v; _temp1386.f2=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp1304.bounds))->v; _temp1386;}); void* _temp1395; void* _temp1397; void*
_temp1399; void* _temp1401; struct Cyc_Absyn_Exp* _temp1403; void* _temp1405;
void* _temp1407; void* _temp1409; void* _temp1411; _LL1389: _LL1400: _temp1399=
_temp1387.f1; if(( unsigned int) _temp1399 > 1u?*(( int*) _temp1399) == Cyc_Absyn_Eq_constr:
0){ _LL1402: _temp1401=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1399)->f1;
if(( unsigned int) _temp1401 > 1u?*(( int*) _temp1401) == Cyc_Absyn_Upper_b: 0){
_LL1404: _temp1403=(( struct Cyc_Absyn_Upper_b_struct*) _temp1401)->f1; goto
_LL1396;} else{ goto _LL1391;}} else{ goto _LL1391;} _LL1396: _temp1395=
_temp1387.f2; if(( unsigned int) _temp1395 > 1u?*(( int*) _temp1395) == Cyc_Absyn_Eq_constr:
0){ _LL1398: _temp1397=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1395)->f1;
if( _temp1397 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1390;} else{ goto _LL1391;}}
else{ goto _LL1391;} _LL1391: _LL1410: _temp1409= _temp1387.f1; if((
unsigned int) _temp1409 > 1u?*(( int*) _temp1409) == Cyc_Absyn_Eq_constr: 0){
_LL1412: _temp1411=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1409)->f1;
if( _temp1411 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1406;} else{ goto _LL1393;}}
else{ goto _LL1393;} _LL1406: _temp1405= _temp1387.f2; if(( unsigned int)
_temp1405 > 1u?*(( int*) _temp1405) == Cyc_Absyn_Eq_constr: 0){ _LL1408:
_temp1407=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1405)->f1; if(
_temp1407 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1392;} else{ goto _LL1393;}}
else{ goto _LL1393;} _LL1393: goto _LL1394; _LL1390: okay= 1; goto _LL1388;
_LL1392: okay= 1; goto _LL1388; _LL1394: okay= 0; goto _LL1388; _LL1388:;}
return(( okay? Cyc_Tcutil_unify(( void*) _temp1308.elt_typ,( void*) _temp1304.elt_typ):
0)? Cyc_Tcutil_unify(( void*) _temp1308.rgn_typ,( void*) _temp1304.rgn_typ): 0)?
!( _temp1308.tq).q_const? 1:( _temp1304.tq).q_const: 0;} _LL1295: { int okay;
okay=( _temp1320 != 0? _temp1312 != 0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1320)) == Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1312)): 0; return( okay? Cyc_Tcutil_unify(
_temp1324, _temp1316): 0)? ! _temp1322.q_const? 1: _temp1314.q_const: 0;}
_LL1297: if(( _temp1348 == _temp1336? 1: Cyc_Absyn_qvar_cmp( _temp1348->name,
_temp1336->name) == 0)? _temp1346->typs == 0: 0){ for( 0; _temp1342 != 0?
_temp1332 != 0: 0;( _temp1342=(( struct Cyc_List_List*) _check_null( _temp1342))->tl,
_temp1332=(( struct Cyc_List_List*) _check_null( _temp1332))->tl)){ if( ! Cyc_Tcutil_unify((
void*)(( struct Cyc_List_List*) _check_null( _temp1342))->hd,( void*)(( struct
Cyc_List_List*) _check_null( _temp1332))->hd)){ break;}} if( _temp1342 == 0?
_temp1332 == 0: 0){ return 1;}} return 0; _LL1299:{ void* _temp1413= Cyc_Tcutil_compress(
_temp1374); struct Cyc_Absyn_TunionFieldInfo _temp1419; struct Cyc_List_List*
_temp1421; void* _temp1423; struct Cyc_Absyn_Tunionfield* _temp1425; struct Cyc_Absyn_Tuniondecl*
_temp1427; _LL1415: if(( unsigned int) _temp1413 > 4u?*(( int*) _temp1413) ==
Cyc_Absyn_TunionFieldType: 0){ _LL1420: _temp1419=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp1413)->f1; _LL1424: _temp1423=( void*) _temp1419.field_info; if(*(( int*)
_temp1423) == Cyc_Absyn_KnownTunionfield){ _LL1428: _temp1427=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1423)->f1; goto _LL1426; _LL1426: _temp1425=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1423)->f2; goto _LL1422;} else{ goto _LL1417;} _LL1422: _temp1421=
_temp1419.targs; goto _LL1416;} else{ goto _LL1417;} _LL1417: goto _LL1418;
_LL1416: if( ! Cyc_Tcutil_unify( _temp1372, _temp1354)? _temp1372 !=( void*) Cyc_Absyn_HeapRgn:
0){ return 0;} if( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x,
struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp,
_temp1370,(( struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)( 0))){
return 0;} if( !(( int(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x,
struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Tcutil_boundscmp,
_temp1366,(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)( Cyc_Absyn_bounds_one))){
return 0;} if( Cyc_Absyn_qvar_cmp( _temp1360->name, _temp1427->name) == 0?
_temp1425->typs != 0: 0){ int okay= 1; for( 0; _temp1421 != 0? _temp1356 != 0: 0;(
_temp1421=(( struct Cyc_List_List*) _check_null( _temp1421))->tl, _temp1356=((
struct Cyc_List_List*) _check_null( _temp1356))->tl)){ if( ! Cyc_Tcutil_unify((
void*)(( struct Cyc_List_List*) _check_null( _temp1421))->hd,( void*)(( struct
Cyc_List_List*) _check_null( _temp1356))->hd)){ okay= 0; break;}} if(( ! okay? 1:
_temp1421 != 0)? 1: _temp1356 != 0){ return 0;} return 1;} goto _LL1414; _LL1418:
goto _LL1414; _LL1414:;} return 0; _LL1301: return Cyc_Tcutil_unify( t1, t2);
_LL1291:;}} int Cyc_Tcutil_coerce_arg( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, void* t2){ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); if( Cyc_Tcutil_unify( t1, t2)){ return 1;} if( Cyc_Tcutil_is_arithmetic_type(
t1)? Cyc_Tcutil_is_arithmetic_type( t2): 0){ if( Cyc_Tcutil_will_lose_precision(
t1, t2)){ Cyc_Tcutil_warn( e->loc,({ char* _temp1429="integral size mismatch; conversion supplied";
struct _tagged_string _temp1430; _temp1430.curr= _temp1429; _temp1430.base=
_temp1429; _temp1430.last_plus_one= _temp1429 + 44; _temp1430;}));} Cyc_Tcutil_unchecked_cast(
te, e, t2); return 1;} else{ if( Cyc_Tcutil_silent_castable( te, e->loc, t1, t2)){
Cyc_Tcutil_unchecked_cast( te, e, t2); return 1;} else{ if( Cyc_Tcutil_castable(
te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te, e, t2); Cyc_Tcutil_warn( e->loc,({
struct _tagged_string _temp1431= Cyc_Absynpp_typ2string( t1); struct
_tagged_string _temp1432= Cyc_Absynpp_typ2string( t2); xprintf("implicit cast from %.*s to %.*s",
_temp1431.last_plus_one - _temp1431.curr, _temp1431.curr, _temp1432.last_plus_one
- _temp1432.curr, _temp1432.curr);})); return 1;} else{ return 0;}}}} int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, void* t){ return Cyc_Tcutil_coerce_arg(
te, e, t);} static int Cyc_Tcutil_any_promote( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Exp* e){ void* old_typ= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); void* t;{ void* _temp1433= old_typ; struct Cyc_Absyn_Enumdecl*
_temp1445; struct _tuple1* _temp1447; void* _temp1449; void* _temp1451; _LL1435:
if(( unsigned int) _temp1433 > 4u?*(( int*) _temp1433) == Cyc_Absyn_EnumType: 0){
_LL1448: _temp1447=(( struct Cyc_Absyn_EnumType_struct*) _temp1433)->f1; goto
_LL1446; _LL1446: _temp1445=(( struct Cyc_Absyn_EnumType_struct*) _temp1433)->f2;
goto _LL1436;} else{ goto _LL1437;} _LL1437: if(( unsigned int) _temp1433 > 4u?*((
int*) _temp1433) == Cyc_Absyn_IntType: 0){ _LL1452: _temp1451=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp1433)->f1; goto _LL1450; _LL1450: _temp1449=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1433)->f2; goto _LL1438;} else{
goto _LL1439;} _LL1439: if( _temp1433 ==( void*) Cyc_Absyn_FloatType){ goto
_LL1440;} else{ goto _LL1441;} _LL1441: if( _temp1433 ==( void*) Cyc_Absyn_DoubleType){
goto _LL1442;} else{ goto _LL1443;} _LL1443: goto _LL1444; _LL1436: t= Cyc_Absyn_sint_t;
goto _LL1434; _LL1438:{ void* _temp1453= _temp1449; _LL1455: if( _temp1453 ==(
void*) Cyc_Absyn_B1){ goto _LL1456;} else{ goto _LL1457;} _LL1457: if( _temp1453
==( void*) Cyc_Absyn_B2){ goto _LL1458;} else{ goto _LL1459;} _LL1459: if(
_temp1453 ==( void*) Cyc_Absyn_B4){ goto _LL1460;} else{ goto _LL1461;} _LL1461:
if( _temp1453 ==( void*) Cyc_Absyn_B8){ goto _LL1462;} else{ goto _LL1454;}
_LL1456: t= Cyc_Absyn_sint_t; goto _LL1454; _LL1458: t= Cyc_Absyn_sint_t; goto
_LL1454; _LL1460: t= _temp1451 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t:
Cyc_Absyn_sint_t; goto _LL1454; _LL1462: t= _temp1451 ==( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_ulong_t: Cyc_Absyn_slong_t; goto _LL1454; _LL1454:;} goto _LL1434;
_LL1440: t= Cyc_Absyn_float_t; goto _LL1434; _LL1442: t= Cyc_Absyn_double_t;
goto _LL1434; _LL1444: return 0; _LL1434:;} Cyc_Tcutil_unchecked_cast( te, e, t);
return 1;} int Cyc_Tcutil_coerce_use( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, void* t2){ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); if( Cyc_Tcutil_unify( t1, t2)){ return 1;} Cyc_Tcutil_any_promote(
te, e); if( Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v,
t2)){ return 1;} else{ if( Cyc_Tcutil_silent_castable( te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast(
te, e, t2); return 1;} else{ if( Cyc_Tcutil_castable( te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast(
te, e, t2); Cyc_Tcutil_warn( e->loc,({ struct _tagged_string _temp1463= Cyc_Absynpp_typ2string(
t1); struct _tagged_string _temp1464= Cyc_Absynpp_typ2string( t2); xprintf("implicit cast from %.*s to %.*s",
_temp1463.last_plus_one - _temp1463.curr, _temp1463.curr, _temp1464.last_plus_one
- _temp1464.curr, _temp1464.curr);})); return 1;} else{ return 0;}}}} int Cyc_Tcutil_coerceable(
void* t){ void* _temp1465= Cyc_Tcutil_compress( t); void* _temp1475; void*
_temp1477; _LL1467: if(( unsigned int) _temp1465 > 4u?*(( int*) _temp1465) ==
Cyc_Absyn_IntType: 0){ _LL1478: _temp1477=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1465)->f1; goto _LL1476; _LL1476: _temp1475=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1465)->f2; goto _LL1468;} else{ goto _LL1469;} _LL1469: if( _temp1465 ==(
void*) Cyc_Absyn_FloatType){ goto _LL1470;} else{ goto _LL1471;} _LL1471: if(
_temp1465 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1472;} else{ goto _LL1473;}
_LL1473: goto _LL1474; _LL1468: return 1; _LL1470: return 1; _LL1472: return 1;
_LL1474: return 0; _LL1466:;} static struct _tuple4* Cyc_Tcutil_flatten_typ_f(
struct Cyc_List_List* inst, struct Cyc_Absyn_Structfield* x){ return({ struct
_tuple4* _temp1479=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1479->f1= x->tq; _temp1479->f2= Cyc_Tcutil_substitute( inst,( void*) x->type);
_temp1479;});} static struct Cyc_List_List* Cyc_Tcutil_flatten_typ( struct Cyc_Tcenv_Tenv*
te, void* t1){ t1= Cyc_Tcutil_compress( t1);{ void* _temp1480= t1; struct Cyc_List_List*
_temp1492; struct Cyc_Absyn_Structdecl** _temp1494; struct Cyc_List_List*
_temp1496; struct _tuple1* _temp1498; struct Cyc_List_List* _temp1500; _LL1482:
if( _temp1480 ==( void*) Cyc_Absyn_VoidType){ goto _LL1483;} else{ goto _LL1484;}
_LL1484: if(( unsigned int) _temp1480 > 4u?*(( int*) _temp1480) == Cyc_Absyn_TupleType:
0){ _LL1493: _temp1492=(( struct Cyc_Absyn_TupleType_struct*) _temp1480)->f1;
goto _LL1485;} else{ goto _LL1486;} _LL1486: if(( unsigned int) _temp1480 > 4u?*((
int*) _temp1480) == Cyc_Absyn_StructType: 0){ _LL1499: _temp1498=(( struct Cyc_Absyn_StructType_struct*)
_temp1480)->f1; goto _LL1497; _LL1497: _temp1496=(( struct Cyc_Absyn_StructType_struct*)
_temp1480)->f2; goto _LL1495; _LL1495: _temp1494=(( struct Cyc_Absyn_StructType_struct*)
_temp1480)->f3; goto _LL1487;} else{ goto _LL1488;} _LL1488: if(( unsigned int)
_temp1480 > 4u?*(( int*) _temp1480) == Cyc_Absyn_AnonStructType: 0){ _LL1501:
_temp1500=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1480)->f1; goto
_LL1489;} else{ goto _LL1490;} _LL1490: goto _LL1491; _LL1483: return 0; _LL1485:
return _temp1492; _LL1487: if( _temp1494 == 0? 1: _temp1498 == 0){ return({
struct Cyc_List_List* _temp1502=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1502->hd=( void*)({ struct _tuple4* _temp1503=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1503->f1= Cyc_Absyn_empty_tqual();
_temp1503->f2= t1; _temp1503;}); _temp1502->tl= 0; _temp1502;});}{ struct Cyc_Absyn_Structdecl*
_temp1504=*(( struct Cyc_Absyn_Structdecl**) _check_null( _temp1494)); goto
_LL1505; _LL1505: { struct _tuple1 _temp1506=*(( struct _tuple1*) _check_null(
_temp1498)); goto _LL1507; _LL1507: { struct Cyc_List_List* _temp1508=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
_temp1504->tvs, _temp1496); goto _LL1509; _LL1509: if( _temp1504->fields == 0){
return({ struct Cyc_List_List* _temp1510=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1510->hd=( void*)({ struct _tuple4*
_temp1511=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1511->f1=
Cyc_Absyn_empty_tqual(); _temp1511->f2= t1; _temp1511;}); _temp1510->tl= 0;
_temp1510;});} return(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct
Cyc_List_List*, struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, _temp1508,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1504->fields))->v);}}}
_LL1489: return(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_List_List*,
struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f, 0, _temp1500); _LL1491: return({
struct Cyc_List_List* _temp1512=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1512->hd=( void*)({ struct _tuple4* _temp1513=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1513->f1= Cyc_Absyn_empty_tqual();
_temp1513->f2= t1; _temp1513;}); _temp1512->tl= 0; _temp1512;}); _LL1481:;}}
static int Cyc_Tcutil_ptrsubtype( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
assume, void* t1, void* t2); static int Cyc_Tcutil_subtype( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* assume, void* t1, void* t2){ if( Cyc_Tcutil_unify( t1,
t2)){ return 1;}{ struct Cyc_List_List* a= assume; for( 0; a != 0; a=(( struct
Cyc_List_List*) _check_null( a))->tl){ if( Cyc_Tcutil_unify( t1,(*(( struct
_tuple7*)(( struct Cyc_List_List*) _check_null( a))->hd)).f1)? Cyc_Tcutil_unify(
t2,(*(( struct _tuple7*)(( struct Cyc_List_List*) _check_null( a))->hd)).f2): 0){
return 1;}}} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct
_tuple7 _temp1515=({ struct _tuple7 _temp1514; _temp1514.f1= t1; _temp1514.f2=
t2; _temp1514;}); void* _temp1521; struct Cyc_Absyn_PtrInfo _temp1523; struct
Cyc_Absyn_Conref* _temp1525; struct Cyc_Absyn_Tqual _temp1527; struct Cyc_Absyn_Conref*
_temp1529; void* _temp1531; void* _temp1533; void* _temp1535; struct Cyc_Absyn_PtrInfo
_temp1537; struct Cyc_Absyn_Conref* _temp1539; struct Cyc_Absyn_Tqual _temp1541;
struct Cyc_Absyn_Conref* _temp1543; void* _temp1545; void* _temp1547; _LL1517:
_LL1536: _temp1535= _temp1515.f1; if(( unsigned int) _temp1535 > 4u?*(( int*)
_temp1535) == Cyc_Absyn_PointerType: 0){ _LL1538: _temp1537=(( struct Cyc_Absyn_PointerType_struct*)
_temp1535)->f1; _LL1548: _temp1547=( void*) _temp1537.elt_typ; goto _LL1546;
_LL1546: _temp1545=( void*) _temp1537.rgn_typ; goto _LL1544; _LL1544: _temp1543=
_temp1537.nullable; goto _LL1542; _LL1542: _temp1541= _temp1537.tq; goto _LL1540;
_LL1540: _temp1539= _temp1537.bounds; goto _LL1522;} else{ goto _LL1519;}
_LL1522: _temp1521= _temp1515.f2; if(( unsigned int) _temp1521 > 4u?*(( int*)
_temp1521) == Cyc_Absyn_PointerType: 0){ _LL1524: _temp1523=(( struct Cyc_Absyn_PointerType_struct*)
_temp1521)->f1; _LL1534: _temp1533=( void*) _temp1523.elt_typ; goto _LL1532;
_LL1532: _temp1531=( void*) _temp1523.rgn_typ; goto _LL1530; _LL1530: _temp1529=
_temp1523.nullable; goto _LL1528; _LL1528: _temp1527= _temp1523.tq; goto _LL1526;
_LL1526: _temp1525= _temp1523.bounds; goto _LL1518;} else{ goto _LL1519;}
_LL1519: goto _LL1520; _LL1518: if( _temp1541.q_const? ! _temp1527.q_const: 0){
return 0;} if(( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x,
struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp,
_temp1543, _temp1529)?(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1543): 0)? !(( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1529): 0){ return 0;}{ void* _temp1549= _temp1545; _LL1551: if( _temp1549
==( void*) Cyc_Absyn_HeapRgn){ goto _LL1552;} else{ goto _LL1553;} _LL1553: goto
_LL1554; _LL1552: goto _LL1550; _LL1554: Cyc_Tcutil_unify( _temp1545, _temp1531);
goto _LL1550; _LL1550:;} if( !(( int(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref*
x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Tcutil_boundscmp,
_temp1539, _temp1525)){ struct _tuple7 _temp1556=({ struct _tuple7 _temp1555;
_temp1555.f1=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1539); _temp1555.f2=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1525); _temp1555;}); void* _temp1564; void* _temp1566; struct Cyc_Absyn_Exp*
_temp1568; void* _temp1570; struct Cyc_Absyn_Exp* _temp1572; void* _temp1574;
struct Cyc_Absyn_Exp* _temp1576; _LL1558: _LL1567: _temp1566= _temp1556.f1; if((
unsigned int) _temp1566 > 1u?*(( int*) _temp1566) == Cyc_Absyn_Upper_b: 0){
_LL1569: _temp1568=(( struct Cyc_Absyn_Upper_b_struct*) _temp1566)->f1; goto
_LL1565;} else{ goto _LL1560;} _LL1565: _temp1564= _temp1556.f2; if( _temp1564
==( void*) Cyc_Absyn_Unknown_b){ goto _LL1559;} else{ goto _LL1560;} _LL1560:
_LL1575: _temp1574= _temp1556.f1; if(( unsigned int) _temp1574 > 1u?*(( int*)
_temp1574) == Cyc_Absyn_Upper_b: 0){ _LL1577: _temp1576=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1574)->f1; goto _LL1571;} else{ goto _LL1562;} _LL1571: _temp1570=
_temp1556.f2; if(( unsigned int) _temp1570 > 1u?*(( int*) _temp1570) == Cyc_Absyn_Upper_b:
0){ _LL1573: _temp1572=(( struct Cyc_Absyn_Upper_b_struct*) _temp1570)->f1; goto
_LL1561;} else{ goto _LL1562;} _LL1562: goto _LL1563; _LL1559: goto _LL1557;
_LL1561: if( Cyc_Evexp_eval_const_uint_exp( _temp1576) < Cyc_Evexp_eval_const_uint_exp(
_temp1572)){ return 0;} goto _LL1557; _LL1563: return 0; _LL1557:;} return Cyc_Tcutil_ptrsubtype(
te,({ struct Cyc_List_List* _temp1578=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1578->hd=( void*)({ struct _tuple7*
_temp1579=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7)); _temp1579->f1=
t1; _temp1579->f2= t2; _temp1579;}); _temp1578->tl= assume; _temp1578;}),
_temp1547, _temp1533); _LL1520: return 0; _LL1516:;}} static int Cyc_Tcutil_ptrsubtype(
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* assume, void* t1, void* t2){
struct Cyc_List_List* tqs1= Cyc_Tcutil_flatten_typ( te, t1); struct Cyc_List_List*
tqs2= Cyc_Tcutil_flatten_typ( te, t2); for( 0; tqs2 != 0;( tqs2=(( struct Cyc_List_List*)
_check_null( tqs2))->tl, tqs1=(( struct Cyc_List_List*) _check_null( tqs1))->tl)){
if( tqs1 == 0){ return 0;}{ struct _tuple4 _temp1582; void* _temp1583; struct
Cyc_Absyn_Tqual _temp1585; struct _tuple4* _temp1580=( struct _tuple4*)(( struct
Cyc_List_List*) _check_null( tqs1))->hd; _temp1582=* _temp1580; _LL1586:
_temp1585= _temp1582.f1; goto _LL1584; _LL1584: _temp1583= _temp1582.f2; goto
_LL1581; _LL1581: { struct _tuple4 _temp1589; void* _temp1590; struct Cyc_Absyn_Tqual
_temp1592; struct _tuple4* _temp1587=( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( tqs2))->hd; _temp1589=* _temp1587; _LL1593: _temp1592= _temp1589.f1;
goto _LL1591; _LL1591: _temp1590= _temp1589.f2; goto _LL1588; _LL1588: if(
_temp1592.q_const? Cyc_Tcutil_subtype( te, assume, _temp1583, _temp1590): 0){
continue;} else{ if( Cyc_Tcutil_unify( _temp1583, _temp1590)){ continue;} else{
return 0;}}}}} return 1;} int Cyc_Tcutil_castable( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void* t1, void* t2){ if( Cyc_Tcutil_unify( t1,
t2)){ return 1;} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{
void* _temp1594= t2; struct Cyc_Absyn_Uniondecl** _temp1602; struct Cyc_List_List*
_temp1604; struct _tuple1* _temp1606; void* _temp1608; void* _temp1610; _LL1596:
if(( unsigned int) _temp1594 > 4u?*(( int*) _temp1594) == Cyc_Absyn_UnionType: 0){
_LL1607: _temp1606=(( struct Cyc_Absyn_UnionType_struct*) _temp1594)->f1; goto
_LL1605; _LL1605: _temp1604=(( struct Cyc_Absyn_UnionType_struct*) _temp1594)->f2;
goto _LL1603; _LL1603: _temp1602=(( struct Cyc_Absyn_UnionType_struct*)
_temp1594)->f3; goto _LL1597;} else{ goto _LL1598;} _LL1598: if(( unsigned int)
_temp1594 > 4u?*(( int*) _temp1594) == Cyc_Absyn_IntType: 0){ _LL1611: _temp1610=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1594)->f1; goto _LL1609; _LL1609:
_temp1608=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1594)->f2; if(
_temp1608 ==( void*) Cyc_Absyn_B4){ goto _LL1599;} else{ goto _LL1600;}} else{
goto _LL1600;} _LL1600: goto _LL1601; _LL1597: if( _temp1602 == 0){ goto _LL1595;}{
struct Cyc_Absyn_Uniondecl* _temp1612=*(( struct Cyc_Absyn_Uniondecl**)
_check_null( _temp1602)); goto _LL1613; _LL1613: if( _temp1612->fields == 0){
goto _LL1595;}{ struct Cyc_List_List* _temp1614=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp1612->tvs,
_temp1604); goto _LL1615; _LL1615:{ struct Cyc_List_List* fs=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1612->fields))->v; for( 0; fs != 0; fs=((
struct Cyc_List_List*) _check_null( fs))->tl){ if( Cyc_Tcutil_unify( t1, Cyc_Tcutil_substitute(
_temp1614,( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->type))){ return 1;}}} goto _LL1595;}} _LL1599: if( Cyc_Tcutil_typ_kind(
t2) ==( void*) Cyc_Absyn_BoxKind){ return 1;} goto _LL1595; _LL1601: goto
_LL1595; _LL1595:;}{ void* _temp1616= t1; struct Cyc_Absyn_PtrInfo _temp1632;
struct Cyc_Absyn_Conref* _temp1634; struct Cyc_Absyn_Tqual _temp1636; struct Cyc_Absyn_Conref*
_temp1638; void* _temp1640; void* _temp1642; struct Cyc_Absyn_Exp* _temp1644;
struct Cyc_Absyn_Tqual _temp1646; void* _temp1648; struct Cyc_Absyn_Enumdecl*
_temp1650; struct _tuple1* _temp1652; void* _temp1654; void* _temp1656; _LL1618:
if(( unsigned int) _temp1616 > 4u?*(( int*) _temp1616) == Cyc_Absyn_PointerType:
0){ _LL1633: _temp1632=(( struct Cyc_Absyn_PointerType_struct*) _temp1616)->f1;
_LL1643: _temp1642=( void*) _temp1632.elt_typ; goto _LL1641; _LL1641: _temp1640=(
void*) _temp1632.rgn_typ; goto _LL1639; _LL1639: _temp1638= _temp1632.nullable;
goto _LL1637; _LL1637: _temp1636= _temp1632.tq; goto _LL1635; _LL1635: _temp1634=
_temp1632.bounds; goto _LL1619;} else{ goto _LL1620;} _LL1620: if(( unsigned int)
_temp1616 > 4u?*(( int*) _temp1616) == Cyc_Absyn_ArrayType: 0){ _LL1649:
_temp1648=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1616)->f1; goto
_LL1647; _LL1647: _temp1646=(( struct Cyc_Absyn_ArrayType_struct*) _temp1616)->f2;
goto _LL1645; _LL1645: _temp1644=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1616)->f3; goto _LL1621;} else{ goto _LL1622;} _LL1622: if(( unsigned int)
_temp1616 > 4u?*(( int*) _temp1616) == Cyc_Absyn_EnumType: 0){ _LL1653:
_temp1652=(( struct Cyc_Absyn_EnumType_struct*) _temp1616)->f1; goto _LL1651;
_LL1651: _temp1650=(( struct Cyc_Absyn_EnumType_struct*) _temp1616)->f2; goto
_LL1623;} else{ goto _LL1624;} _LL1624: if(( unsigned int) _temp1616 > 4u?*((
int*) _temp1616) == Cyc_Absyn_IntType: 0){ _LL1657: _temp1656=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp1616)->f1; goto _LL1655; _LL1655: _temp1654=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1616)->f2; goto _LL1625;} else{
goto _LL1626;} _LL1626: if( _temp1616 ==( void*) Cyc_Absyn_FloatType){ goto
_LL1627;} else{ goto _LL1628;} _LL1628: if( _temp1616 ==( void*) Cyc_Absyn_DoubleType){
goto _LL1629;} else{ goto _LL1630;} _LL1630: goto _LL1631; _LL1619:{ void*
_temp1658= t2; struct Cyc_Absyn_PtrInfo _temp1664; struct Cyc_Absyn_Conref*
_temp1666; struct Cyc_Absyn_Tqual _temp1668; struct Cyc_Absyn_Conref* _temp1670;
void* _temp1672; void* _temp1674; _LL1660: if(( unsigned int) _temp1658 > 4u?*((
int*) _temp1658) == Cyc_Absyn_PointerType: 0){ _LL1665: _temp1664=(( struct Cyc_Absyn_PointerType_struct*)
_temp1658)->f1; _LL1675: _temp1674=( void*) _temp1664.elt_typ; goto _LL1673;
_LL1673: _temp1672=( void*) _temp1664.rgn_typ; goto _LL1671; _LL1671: _temp1670=
_temp1664.nullable; goto _LL1669; _LL1669: _temp1668= _temp1664.tq; goto _LL1667;
_LL1667: _temp1666= _temp1664.bounds; goto _LL1661;} else{ goto _LL1662;}
_LL1662: goto _LL1663; _LL1661: { struct Cyc_List_List* _temp1678=({ struct Cyc_List_List*
_temp1676=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1676->hd=( void*)({ struct _tuple7* _temp1677=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp1677->f1= t1; _temp1677->f2= t2; _temp1677;});
_temp1676->tl= 0; _temp1676;}); goto _LL1679; _LL1679: { int _temp1680= Cyc_Tcutil_ptrsubtype(
te, _temp1678, _temp1642, _temp1674)? ! _temp1636.q_const? 1: _temp1668.q_const:
0; goto _LL1681; _LL1681: { int bounds_ok=(( int(*)( int(* cmp)( void*, void*),
struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)(
Cyc_Tcutil_boundscmp, _temp1634, _temp1666); if( ! bounds_ok){ struct _tuple7
_temp1683=({ struct _tuple7 _temp1682; _temp1682.f1=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp1634); _temp1682.f2=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp1666); _temp1682;}); void* _temp1689; struct Cyc_Absyn_Exp*
_temp1691; void* _temp1693; struct Cyc_Absyn_Exp* _temp1695; _LL1685: _LL1694:
_temp1693= _temp1683.f1; if(( unsigned int) _temp1693 > 1u?*(( int*) _temp1693)
== Cyc_Absyn_Upper_b: 0){ _LL1696: _temp1695=(( struct Cyc_Absyn_Upper_b_struct*)
_temp1693)->f1; goto _LL1690;} else{ goto _LL1687;} _LL1690: _temp1689=
_temp1683.f2; if(( unsigned int) _temp1689 > 1u?*(( int*) _temp1689) == Cyc_Absyn_Upper_b:
0){ _LL1692: _temp1691=(( struct Cyc_Absyn_Upper_b_struct*) _temp1689)->f1; goto
_LL1686;} else{ goto _LL1687;} _LL1687: goto _LL1688; _LL1686: if( Cyc_Evexp_eval_const_uint_exp(
_temp1695) >= Cyc_Evexp_eval_const_uint_exp( _temp1691)){ bounds_ok= 1;} goto
_LL1684; _LL1688: bounds_ok= 1; goto _LL1684; _LL1684:;} return( bounds_ok?
_temp1680: 0)? _temp1640 ==( void*) Cyc_Absyn_HeapRgn? 1: Cyc_Tcutil_unify(
_temp1640, _temp1672): 0;}}} _LL1663: goto _LL1659; _LL1659:;} return 0; _LL1621:{
void* _temp1697= t2; struct Cyc_Absyn_Exp* _temp1703; struct Cyc_Absyn_Tqual
_temp1705; void* _temp1707; _LL1699: if(( unsigned int) _temp1697 > 4u?*(( int*)
_temp1697) == Cyc_Absyn_ArrayType: 0){ _LL1708: _temp1707=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1697)->f1; goto _LL1706; _LL1706: _temp1705=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1697)->f2; goto _LL1704; _LL1704: _temp1703=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1697)->f3; goto _LL1700;} else{ goto _LL1701;} _LL1701: goto _LL1702;
_LL1700: { int okay; okay=( _temp1644 != 0? _temp1703 != 0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1644)) >= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp1703)): 0; return( okay? Cyc_Tcutil_unify(
_temp1648, _temp1707): 0)? ! _temp1646.q_const? 1: _temp1705.q_const: 0;}
_LL1702: return 0; _LL1698:;} return 0; _LL1623: goto _LL1625; _LL1625: goto
_LL1627; _LL1627: goto _LL1629; _LL1629: return Cyc_Tcutil_coerceable( t2);
_LL1631: return 0; _LL1617:;}} void Cyc_Tcutil_unchecked_cast( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e, void* t){ if( ! Cyc_Tcutil_unify(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v, t)){ struct Cyc_Absyn_Exp* _temp1709=
Cyc_Absyn_copy_exp( e); goto _LL1710; _LL1710:( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_Cast_e_struct* _temp1711=( struct Cyc_Absyn_Cast_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp1711[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp1712; _temp1712.tag= Cyc_Absyn_Cast_e; _temp1712.f1=( void*) t; _temp1712.f2=
_temp1709; _temp1712;}); _temp1711;}))); e->topt=({ struct Cyc_Core_Opt*
_temp1713=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1713->v=( void*) t; _temp1713;});}} int Cyc_Tcutil_is_integral( struct Cyc_Absyn_Exp*
e){ void* _temp1714= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); struct Cyc_Absyn_Enumdecl* _temp1724; struct _tuple1*
_temp1726; void* _temp1728; void* _temp1730; int _temp1732; struct Cyc_Core_Opt*
_temp1734; void* _temp1736; _LL1716: if(( unsigned int) _temp1714 > 4u?*(( int*)
_temp1714) == Cyc_Absyn_EnumType: 0){ _LL1727: _temp1726=(( struct Cyc_Absyn_EnumType_struct*)
_temp1714)->f1; goto _LL1725; _LL1725: _temp1724=(( struct Cyc_Absyn_EnumType_struct*)
_temp1714)->f2; goto _LL1717;} else{ goto _LL1718;} _LL1718: if(( unsigned int)
_temp1714 > 4u?*(( int*) _temp1714) == Cyc_Absyn_IntType: 0){ _LL1731: _temp1730=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1714)->f1; goto _LL1729; _LL1729:
_temp1728=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1714)->f2; goto
_LL1719;} else{ goto _LL1720;} _LL1720: if(( unsigned int) _temp1714 > 4u?*((
int*) _temp1714) == Cyc_Absyn_Evar: 0){ _LL1737: _temp1736=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp1714)->f1; goto _LL1735; _LL1735: _temp1734=(( struct Cyc_Absyn_Evar_struct*)
_temp1714)->f2; goto _LL1733; _LL1733: _temp1732=(( struct Cyc_Absyn_Evar_struct*)
_temp1714)->f3; goto _LL1721;} else{ goto _LL1722;} _LL1722: goto _LL1723;
_LL1717: goto _LL1719; _LL1719: return 1; _LL1721: if( Cyc_Tcutil_unify(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_Absyn_sint_t)){ return 1;}
return 0; _LL1723: return 0; _LL1715:;} int Cyc_Tcutil_is_numeric( struct Cyc_Absyn_Exp*
e){ if( Cyc_Tcutil_is_integral( e)){ return 1;}{ void* _temp1738= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); _LL1740: if(
_temp1738 ==( void*) Cyc_Absyn_FloatType){ goto _LL1741;} else{ goto _LL1742;}
_LL1742: if( _temp1738 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1743;} else{
goto _LL1744;} _LL1744: goto _LL1745; _LL1741: goto _LL1743; _LL1743: return 1;
_LL1745: return 0; _LL1739:;}} int Cyc_Tcutil_is_function_type( void* t){ void*
_temp1746= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo _temp1752; _LL1748:
if(( unsigned int) _temp1746 > 4u?*(( int*) _temp1746) == Cyc_Absyn_FnType: 0){
_LL1753: _temp1752=(( struct Cyc_Absyn_FnType_struct*) _temp1746)->f1; goto
_LL1749;} else{ goto _LL1750;} _LL1750: goto _LL1751; _LL1749: return 1; _LL1751:
return 0; _LL1747:;} void* Cyc_Tcutil_max_arithmetic_type( void* t1, void* t2){
struct _tuple7 _temp1755=({ struct _tuple7 _temp1754; _temp1754.f1= t1;
_temp1754.f2= t2; _temp1754;}); void* _temp1779; void* _temp1781; void*
_temp1783; void* _temp1785; void* _temp1787; void* _temp1789; void* _temp1791;
void* _temp1793; void* _temp1795; void* _temp1797; void* _temp1799; void*
_temp1801; void* _temp1803; void* _temp1805; void* _temp1807; void* _temp1809;
void* _temp1811; void* _temp1813; void* _temp1815; void* _temp1817; void*
_temp1819; void* _temp1821; void* _temp1823; void* _temp1825; void* _temp1827;
void* _temp1829; void* _temp1831; void* _temp1833; void* _temp1835; void*
_temp1837; void* _temp1839; void* _temp1841; _LL1757: _LL1782: _temp1781=
_temp1755.f1; if( _temp1781 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1780;}
else{ goto _LL1759;} _LL1780: _temp1779= _temp1755.f2; goto _LL1758; _LL1759:
_LL1786: _temp1785= _temp1755.f1; goto _LL1784; _LL1784: _temp1783= _temp1755.f2;
if( _temp1783 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1760;} else{ goto
_LL1761;} _LL1761: _LL1790: _temp1789= _temp1755.f1; if( _temp1789 ==( void*)
Cyc_Absyn_FloatType){ goto _LL1788;} else{ goto _LL1763;} _LL1788: _temp1787=
_temp1755.f2; goto _LL1762; _LL1763: _LL1794: _temp1793= _temp1755.f1; goto
_LL1792; _LL1792: _temp1791= _temp1755.f2; if( _temp1791 ==( void*) Cyc_Absyn_FloatType){
goto _LL1764;} else{ goto _LL1765;} _LL1765: _LL1798: _temp1797= _temp1755.f1;
if(( unsigned int) _temp1797 > 4u?*(( int*) _temp1797) == Cyc_Absyn_IntType: 0){
_LL1802: _temp1801=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1797)->f1;
if( _temp1801 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1800;} else{ goto _LL1767;}
_LL1800: _temp1799=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1797)->f2;
if( _temp1799 ==( void*) Cyc_Absyn_B8){ goto _LL1796;} else{ goto _LL1767;}}
else{ goto _LL1767;} _LL1796: _temp1795= _temp1755.f2; goto _LL1766; _LL1767:
_LL1810: _temp1809= _temp1755.f1; goto _LL1804; _LL1804: _temp1803= _temp1755.f2;
if(( unsigned int) _temp1803 > 4u?*(( int*) _temp1803) == Cyc_Absyn_IntType: 0){
_LL1808: _temp1807=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1803)->f1;
if( _temp1807 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1806;} else{ goto _LL1769;}
_LL1806: _temp1805=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1803)->f2;
if( _temp1805 ==( void*) Cyc_Absyn_B8){ goto _LL1768;} else{ goto _LL1769;}}
else{ goto _LL1769;} _LL1769: _LL1814: _temp1813= _temp1755.f1; if((
unsigned int) _temp1813 > 4u?*(( int*) _temp1813) == Cyc_Absyn_IntType: 0){
_LL1818: _temp1817=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1813)->f1;
if( _temp1817 ==( void*) Cyc_Absyn_Signed){ goto _LL1816;} else{ goto _LL1771;}
_LL1816: _temp1815=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1813)->f2;
if( _temp1815 ==( void*) Cyc_Absyn_B8){ goto _LL1812;} else{ goto _LL1771;}}
else{ goto _LL1771;} _LL1812: _temp1811= _temp1755.f2; goto _LL1770; _LL1771:
_LL1826: _temp1825= _temp1755.f1; goto _LL1820; _LL1820: _temp1819= _temp1755.f2;
if(( unsigned int) _temp1819 > 4u?*(( int*) _temp1819) == Cyc_Absyn_IntType: 0){
_LL1824: _temp1823=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1819)->f1;
if( _temp1823 ==( void*) Cyc_Absyn_Signed){ goto _LL1822;} else{ goto _LL1773;}
_LL1822: _temp1821=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1819)->f2;
if( _temp1821 ==( void*) Cyc_Absyn_B8){ goto _LL1772;} else{ goto _LL1773;}}
else{ goto _LL1773;} _LL1773: _LL1830: _temp1829= _temp1755.f1; if((
unsigned int) _temp1829 > 4u?*(( int*) _temp1829) == Cyc_Absyn_IntType: 0){
_LL1834: _temp1833=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1829)->f1;
if( _temp1833 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1832;} else{ goto _LL1775;}
_LL1832: _temp1831=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1829)->f2;
if( _temp1831 ==( void*) Cyc_Absyn_B4){ goto _LL1828;} else{ goto _LL1775;}}
else{ goto _LL1775;} _LL1828: _temp1827= _temp1755.f2; goto _LL1774; _LL1775:
_LL1842: _temp1841= _temp1755.f1; goto _LL1836; _LL1836: _temp1835= _temp1755.f2;
if(( unsigned int) _temp1835 > 4u?*(( int*) _temp1835) == Cyc_Absyn_IntType: 0){
_LL1840: _temp1839=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1835)->f1;
if( _temp1839 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1838;} else{ goto _LL1777;}
_LL1838: _temp1837=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1835)->f2;
if( _temp1837 ==( void*) Cyc_Absyn_B4){ goto _LL1776;} else{ goto _LL1777;}}
else{ goto _LL1777;} _LL1777: goto _LL1778; _LL1758: goto _LL1760; _LL1760:
return( void*) Cyc_Absyn_DoubleType; _LL1762: goto _LL1764; _LL1764: return(
void*) Cyc_Absyn_FloatType; _LL1766: goto _LL1768; _LL1768: return Cyc_Absyn_ulong_t;
_LL1770: goto _LL1772; _LL1772: return Cyc_Absyn_slong_t; _LL1774: goto _LL1776;
_LL1776: return Cyc_Absyn_uint_t; _LL1778: return Cyc_Absyn_sint_t; _LL1756:;}
void Cyc_Tcutil_check_contains_assign( struct Cyc_Absyn_Exp* e){ void* _temp1843=(
void*) e->r; struct Cyc_Absyn_Exp* _temp1849; struct Cyc_Core_Opt* _temp1851;
struct Cyc_Absyn_Exp* _temp1853; _LL1845: if(*(( int*) _temp1843) == Cyc_Absyn_AssignOp_e){
_LL1854: _temp1853=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp1843)->f1; goto
_LL1852; _LL1852: _temp1851=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp1843)->f2;
if( _temp1851 == 0){ goto _LL1850;} else{ goto _LL1847;} _LL1850: _temp1849=((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1843)->f3; goto _LL1846;} else{ goto
_LL1847;} _LL1847: goto _LL1848; _LL1846: Cyc_Tcutil_warn( e->loc,({ char*
_temp1855="assignment in test"; struct _tagged_string _temp1856; _temp1856.curr=
_temp1855; _temp1856.base= _temp1855; _temp1856.last_plus_one= _temp1855 + 19;
_temp1856;})); goto _LL1844; _LL1848: goto _LL1844; _LL1844:;} static int Cyc_Tcutil_constrain_kinds(
struct Cyc_Absyn_Conref* c1, struct Cyc_Absyn_Conref* c2){ c1=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( c1); c2=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( c2); if( c1 == c2){
return 1;} if(( void*) c1->v ==( void*) Cyc_Absyn_No_constr){( void*)( c1->v=(
void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp1857=( struct Cyc_Absyn_Forward_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp1857[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp1858; _temp1858.tag= Cyc_Absyn_Forward_constr;
_temp1858.f1= c2; _temp1858;}); _temp1857;}))); return 1;} else{ if(( void*) c2->v
==( void*) Cyc_Absyn_No_constr){( void*)( c2->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp1859=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp1859[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp1860; _temp1860.tag= Cyc_Absyn_Forward_constr; _temp1860.f1= c1; _temp1860;});
_temp1859;}))); return 1;} else{ void* k1=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( c1); void* k2=(( void*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_val)( c2); if( Cyc_Tcutil_kind_leq( k1, k2)){( void*)( c2->v=(
void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp1861=( struct Cyc_Absyn_Forward_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp1861[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp1862; _temp1862.tag= Cyc_Absyn_Forward_constr;
_temp1862.f1= c1; _temp1862;}); _temp1861;}))); return 1;} else{ if( Cyc_Tcutil_kind_leq(
k2, k1)){( void*)( c1->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp1863=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp1863[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp1864; _temp1864.tag= Cyc_Absyn_Forward_constr; _temp1864.f1= c2; _temp1864;});
_temp1863;}))); return 1;} else{ return 0;}}}}} static struct Cyc_List_List* Cyc_Tcutil_add_free_tvar(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar*
tv){ if( tvs == 0){ return({ struct Cyc_List_List* _temp1865=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1865->hd=( void*) tv; _temp1865->tl=
0; _temp1865;});} else{{ struct Cyc_List_List* tvs2= tvs; for( 0; tvs2 != 0;
tvs2=(( struct Cyc_List_List*) _check_null( tvs2))->tl){ if( Cyc_String_zstrptrcmp(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs2))->hd)->name,
tv->name) == 0){ struct Cyc_Absyn_Conref* k1=(( struct Cyc_Absyn_Tvar*)(( struct
Cyc_List_List*) _check_null( tvs2))->hd)->kind; struct Cyc_Absyn_Conref* k2= tv->kind;
if( ! Cyc_Tcutil_constrain_kinds( k1, k2)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp1866=* tv->name; struct _tagged_string _temp1867= Cyc_Absynpp_ckind2string(
k1); struct _tagged_string _temp1868= Cyc_Absynpp_ckind2string( k2); xprintf("type variable %.*s is used with inconsistent kinds %.*s and %.*s",
_temp1866.last_plus_one - _temp1866.curr, _temp1866.curr, _temp1867.last_plus_one
- _temp1867.curr, _temp1867.curr, _temp1868.last_plus_one - _temp1868.curr,
_temp1868.curr);}));} return tvs;} else{ if((( struct Cyc_List_List*)
_check_null( tvs2))->tl == 0){(( struct Cyc_List_List*) _check_null( tvs2))->tl=({
struct Cyc_List_List* _temp1869=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1869->hd=( void*) tv; _temp1869->tl= 0; _temp1869;});
return tvs;}}}} return(( struct Cyc_List_List*(*)( struct _tagged_string msg))
Cyc_Tcutil_impos)(({ char* _temp1870="Tcutil::add_free_tvar:expecting to have returned.";
struct _tagged_string _temp1871; _temp1871.curr= _temp1870; _temp1871.base=
_temp1870; _temp1871.last_plus_one= _temp1870 + 50; _temp1871;}));}} static
struct Cyc_List_List* Cyc_Tcutil_remove_bound_tvars( struct Cyc_List_List* tvs,
struct Cyc_List_List* btvs){ struct Cyc_List_List* r= 0; for( 0; tvs != 0; tvs=((
struct Cyc_List_List*) _check_null( tvs))->tl){ int present= 0;{ struct Cyc_List_List*
b= btvs; for( 0; b != 0; b=(( struct Cyc_List_List*) _check_null( b))->tl){ if(
Cyc_String_zstrptrcmp((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs))->hd)->name,(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( b))->hd)->name) == 0){ present= 1; break;}}} if( ! present){ r=({
struct Cyc_List_List* _temp1872=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1872->hd=( void*)(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd); _temp1872->tl= r; _temp1872;});}}
r=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( r);
return r;} static int Cyc_Tcutil_tvar_counter= 0; struct Cyc_Absyn_Tvar* Cyc_Tcutil_new_tvar(
void* k){ struct _tagged_string s= xprintf("#%d", Cyc_Tcutil_tvar_counter ++);
return({ struct Cyc_Absyn_Tvar* _temp1873=( struct Cyc_Absyn_Tvar*) GC_malloc(
sizeof( struct Cyc_Absyn_Tvar)); _temp1873->name=({ struct _tagged_string*
_temp1874=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp1874[ 0]= s; _temp1874;}); _temp1873->kind=(( struct Cyc_Absyn_Conref*(*)(
void* x)) Cyc_Absyn_new_conref)( k); _temp1873;});} int Cyc_Tcutil_is_temp_tvar(
struct Cyc_Absyn_Tvar* t){ struct _tagged_string _temp1875=* t->name; goto
_LL1876; _LL1876: return*(( char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( _temp1875, sizeof( char), 0) =='#';}
void Cyc_Tcutil_rewrite_temp_tvar( struct Cyc_Absyn_Tvar* t){ if( ! Cyc_Tcutil_is_temp_tvar(
t)){ return;}{ struct _tagged_string _temp1879= Cyc_String_strconcat(({ char*
_temp1877="`"; struct _tagged_string _temp1878; _temp1878.curr= _temp1877;
_temp1878.base= _temp1877; _temp1878.last_plus_one= _temp1877 + 2; _temp1878;}),*
t->name); goto _LL1880; _LL1880:*(( char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( _temp1879, sizeof( char),
1)='t'; t->name=({ struct _tagged_string* _temp1881=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp1881[ 0]= _temp1879;
_temp1881;});}} struct _tuple11{ struct _tagged_string* f1; struct Cyc_Absyn_Tqual
f2; void* f3; } ; static struct _tuple2* Cyc_Tcutil_fndecl2typ_f( struct
_tuple11* x){ return({ struct _tuple2* _temp1882=( struct _tuple2*) GC_malloc(
sizeof( struct _tuple2)); _temp1882->f1=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt*
_temp1883=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1883->v=( void*)(* x).f1; _temp1883;}); _temp1882->f2=(* x).f2; _temp1882->f3=(*
x).f3; _temp1882;});} void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl* fd){
if( fd->cached_typ == 0){ struct Cyc_List_List* _temp1884= 0; goto _LL1885;
_LL1885:{ struct Cyc_List_List* atts= fd->attributes; for( 0; atts != 0; atts=((
struct Cyc_List_List*) _check_null( atts))->tl){ if( Cyc_Absyn_fntype_att(( void*)((
struct Cyc_List_List*) _check_null( atts))->hd)){ _temp1884=({ struct Cyc_List_List*
_temp1886=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1886->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( atts))->hd);
_temp1886->tl= _temp1884; _temp1886;});}}} return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp1887=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp1887[ 0]=({ struct Cyc_Absyn_FnType_struct _temp1888; _temp1888.tag= Cyc_Absyn_FnType;
_temp1888.f1=({ struct Cyc_Absyn_FnInfo _temp1889; _temp1889.tvars= fd->tvs;
_temp1889.effect= fd->effect; _temp1889.ret_typ=( void*)(( void*) fd->ret_type);
_temp1889.args=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct
_tuple11*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_fndecl2typ_f, fd->args);
_temp1889.varargs= fd->varargs; _temp1889.attributes= _temp1884; _temp1889;});
_temp1888;}); _temp1887;});} return( void*)(( struct Cyc_Core_Opt*) _check_null(
fd->cached_typ))->v;} struct _tuple12{ void* f1; struct Cyc_Absyn_Tqual f2; void*
f3; } ; static void* Cyc_Tcutil_fst_fdarg( struct _tuple12* t){ return(* t).f1;}
void* Cyc_Tcutil_snd_tqt( struct _tuple4* t){ return(* t).f2;} static struct
_tuple4* Cyc_Tcutil_map2_tq( struct _tuple4* pr, void* t){ return({ struct
_tuple4* _temp1890=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1890->f1=(* pr).f1; _temp1890->f2= t; _temp1890;});} void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Fndecl*
fd){ void* t= Cyc_Tcutil_fndecl2typ( fd); Cyc_Tcutil_check_valid_toplevel_type(
loc, te, t);{ void* _temp1891= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo
_temp1897; struct Cyc_List_List* _temp1899; int _temp1901; struct Cyc_List_List*
_temp1903; void* _temp1905; struct Cyc_Core_Opt* _temp1907; struct Cyc_List_List*
_temp1909; _LL1893: if(( unsigned int) _temp1891 > 4u?*(( int*) _temp1891) ==
Cyc_Absyn_FnType: 0){ _LL1898: _temp1897=(( struct Cyc_Absyn_FnType_struct*)
_temp1891)->f1; _LL1910: _temp1909= _temp1897.tvars; goto _LL1908; _LL1908:
_temp1907= _temp1897.effect; goto _LL1906; _LL1906: _temp1905=( void*) _temp1897.ret_typ;
goto _LL1904; _LL1904: _temp1903= _temp1897.args; goto _LL1902; _LL1902:
_temp1901= _temp1897.varargs; goto _LL1900; _LL1900: _temp1899= _temp1897.attributes;
goto _LL1894;} else{ goto _LL1895;} _LL1895: goto _LL1896; _LL1894: fd->tvs=
_temp1909; fd->effect= _temp1907; goto _LL1892; _LL1896:(( void(*)( struct
_tagged_string msg)) Cyc_Tcutil_impos)(({ char* _temp1911="check_fndecl_valid_type: not a FnType";
struct _tagged_string _temp1912; _temp1912.curr= _temp1911; _temp1912.base=
_temp1911; _temp1912.last_plus_one= _temp1911 + 38; _temp1912;})); return;
_LL1892:;}{ struct _RegionHandle _temp1913= _new_region(); struct _RegionHandle*
r=& _temp1913; _push_region( r); Cyc_Tcutil_check_unique_vars((( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tagged_string*(* f)( struct _tuple11*), struct
Cyc_List_List* x)) Cyc_List_rmap)( r,( struct _tagged_string*(*)( struct
_tuple11* t)) Cyc_Tcutil_fst_fdarg, fd->args), loc,({ char* _temp1914="function declaration has repeated parameter";
struct _tagged_string _temp1915; _temp1915.curr= _temp1914; _temp1915.base=
_temp1914; _temp1915.last_plus_one= _temp1914 + 44; _temp1915;}));; _pop_region(&
_temp1913);} fd->cached_typ=({ struct Cyc_Core_Opt* _temp1916=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1916->v=( void*) t; _temp1916;});}
struct _tuple13{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; } ; struct
_tuple14{ struct _tuple13* f1; void* f2; } ; static struct _tuple14* Cyc_Tcutil_substitute_f1(
struct _RegionHandle* rgn, struct _tuple2* y){ return({ struct _tuple14*
_temp1917=( struct _tuple14*) _region_malloc( rgn, sizeof( struct _tuple14));
_temp1917->f1=({ struct _tuple13* _temp1918=( struct _tuple13*) _region_malloc(
rgn, sizeof( struct _tuple13)); _temp1918->f1=(* y).f1; _temp1918->f2=(* y).f2;
_temp1918;}); _temp1917->f2=(* y).f3; _temp1917;});} static struct _tuple2* Cyc_Tcutil_substitute_f2(
struct _tuple14* w){ void* _temp1921; struct _tuple13* _temp1923; struct
_tuple14 _temp1919=* w; _LL1924: _temp1923= _temp1919.f1; goto _LL1922; _LL1922:
_temp1921= _temp1919.f2; goto _LL1920; _LL1920: { struct Cyc_Absyn_Tqual
_temp1927; struct Cyc_Core_Opt* _temp1929; struct _tuple13 _temp1925=* _temp1923;
_LL1930: _temp1929= _temp1925.f1; goto _LL1928; _LL1928: _temp1927= _temp1925.f2;
goto _LL1926; _LL1926: return({ struct _tuple2* _temp1931=( struct _tuple2*)
GC_malloc( sizeof( struct _tuple2)); _temp1931->f1= _temp1929; _temp1931->f2=
_temp1927; _temp1931->f3= _temp1921; _temp1931;});}} static void* Cyc_Tcutil_field_type(
struct Cyc_Absyn_Structfield* f){ return( void*) f->type;} static struct Cyc_Absyn_Structfield*
Cyc_Tcutil_zip_field_type( struct Cyc_Absyn_Structfield* f, void* t){ return({
struct Cyc_Absyn_Structfield* _temp1932=( struct Cyc_Absyn_Structfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Structfield)); _temp1932->name= f->name;
_temp1932->tq= f->tq; _temp1932->type=( void*) t; _temp1932->width= f->width;
_temp1932->attributes= f->attributes; _temp1932;});} static struct Cyc_List_List*
Cyc_Tcutil_substs( struct _RegionHandle* rgn, struct Cyc_List_List* inst, struct
Cyc_List_List* ts); void* Cyc_Tcutil_rsubstitute( struct _RegionHandle* rgn,
struct Cyc_List_List* inst, void* t){ void* _temp1933= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Tvar* _temp1979; struct Cyc_Absyn_Structdecl** _temp1981;
struct Cyc_List_List* _temp1983; struct _tuple1* _temp1985; struct Cyc_Absyn_Uniondecl**
_temp1987; struct Cyc_List_List* _temp1989; struct _tuple1* _temp1991; struct
Cyc_Absyn_TunionInfo _temp1993; void* _temp1995; struct Cyc_List_List* _temp1997;
void* _temp1999; struct Cyc_Absyn_TunionFieldInfo _temp2001; struct Cyc_List_List*
_temp2003; void* _temp2005; struct Cyc_Core_Opt* _temp2007; struct Cyc_List_List*
_temp2009; struct _tuple1* _temp2011; struct Cyc_Absyn_Exp* _temp2013; struct
Cyc_Absyn_Tqual _temp2015; void* _temp2017; struct Cyc_Absyn_PtrInfo _temp2019;
struct Cyc_Absyn_Conref* _temp2021; struct Cyc_Absyn_Tqual _temp2023; struct Cyc_Absyn_Conref*
_temp2025; void* _temp2027; void* _temp2029; struct Cyc_Absyn_FnInfo _temp2031;
struct Cyc_List_List* _temp2033; int _temp2035; struct Cyc_List_List* _temp2037;
void* _temp2039; struct Cyc_Core_Opt* _temp2041; struct Cyc_List_List* _temp2043;
struct Cyc_List_List* _temp2045; struct Cyc_List_List* _temp2047; struct Cyc_List_List*
_temp2049; int _temp2051; struct Cyc_Core_Opt* _temp2053; void* _temp2055; void*
_temp2057; struct Cyc_Absyn_Enumdecl* _temp2059; struct _tuple1* _temp2061; void*
_temp2063; void* _temp2065; void* _temp2067; struct Cyc_List_List* _temp2069;
_LL1935: if(( unsigned int) _temp1933 > 4u?*(( int*) _temp1933) == Cyc_Absyn_VarType:
0){ _LL1980: _temp1979=(( struct Cyc_Absyn_VarType_struct*) _temp1933)->f1; goto
_LL1936;} else{ goto _LL1937;} _LL1937: if(( unsigned int) _temp1933 > 4u?*((
int*) _temp1933) == Cyc_Absyn_StructType: 0){ _LL1986: _temp1985=(( struct Cyc_Absyn_StructType_struct*)
_temp1933)->f1; goto _LL1984; _LL1984: _temp1983=(( struct Cyc_Absyn_StructType_struct*)
_temp1933)->f2; goto _LL1982; _LL1982: _temp1981=(( struct Cyc_Absyn_StructType_struct*)
_temp1933)->f3; goto _LL1938;} else{ goto _LL1939;} _LL1939: if(( unsigned int)
_temp1933 > 4u?*(( int*) _temp1933) == Cyc_Absyn_UnionType: 0){ _LL1992:
_temp1991=(( struct Cyc_Absyn_UnionType_struct*) _temp1933)->f1; goto _LL1990;
_LL1990: _temp1989=(( struct Cyc_Absyn_UnionType_struct*) _temp1933)->f2; goto
_LL1988; _LL1988: _temp1987=(( struct Cyc_Absyn_UnionType_struct*) _temp1933)->f3;
goto _LL1940;} else{ goto _LL1941;} _LL1941: if(( unsigned int) _temp1933 > 4u?*((
int*) _temp1933) == Cyc_Absyn_TunionType: 0){ _LL1994: _temp1993=(( struct Cyc_Absyn_TunionType_struct*)
_temp1933)->f1; _LL2000: _temp1999=( void*) _temp1993.tunion_info; goto _LL1998;
_LL1998: _temp1997= _temp1993.targs; goto _LL1996; _LL1996: _temp1995=( void*)
_temp1993.rgn; goto _LL1942;} else{ goto _LL1943;} _LL1943: if(( unsigned int)
_temp1933 > 4u?*(( int*) _temp1933) == Cyc_Absyn_TunionFieldType: 0){ _LL2002:
_temp2001=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1933)->f1; _LL2006:
_temp2005=( void*) _temp2001.field_info; goto _LL2004; _LL2004: _temp2003=
_temp2001.targs; goto _LL1944;} else{ goto _LL1945;} _LL1945: if(( unsigned int)
_temp1933 > 4u?*(( int*) _temp1933) == Cyc_Absyn_TypedefType: 0){ _LL2012:
_temp2011=(( struct Cyc_Absyn_TypedefType_struct*) _temp1933)->f1; goto _LL2010;
_LL2010: _temp2009=(( struct Cyc_Absyn_TypedefType_struct*) _temp1933)->f2; goto
_LL2008; _LL2008: _temp2007=(( struct Cyc_Absyn_TypedefType_struct*) _temp1933)->f3;
goto _LL1946;} else{ goto _LL1947;} _LL1947: if(( unsigned int) _temp1933 > 4u?*((
int*) _temp1933) == Cyc_Absyn_ArrayType: 0){ _LL2018: _temp2017=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1933)->f1; goto _LL2016; _LL2016:
_temp2015=(( struct Cyc_Absyn_ArrayType_struct*) _temp1933)->f2; goto _LL2014;
_LL2014: _temp2013=(( struct Cyc_Absyn_ArrayType_struct*) _temp1933)->f3; goto
_LL1948;} else{ goto _LL1949;} _LL1949: if(( unsigned int) _temp1933 > 4u?*((
int*) _temp1933) == Cyc_Absyn_PointerType: 0){ _LL2020: _temp2019=(( struct Cyc_Absyn_PointerType_struct*)
_temp1933)->f1; _LL2030: _temp2029=( void*) _temp2019.elt_typ; goto _LL2028;
_LL2028: _temp2027=( void*) _temp2019.rgn_typ; goto _LL2026; _LL2026: _temp2025=
_temp2019.nullable; goto _LL2024; _LL2024: _temp2023= _temp2019.tq; goto _LL2022;
_LL2022: _temp2021= _temp2019.bounds; goto _LL1950;} else{ goto _LL1951;}
_LL1951: if(( unsigned int) _temp1933 > 4u?*(( int*) _temp1933) == Cyc_Absyn_FnType:
0){ _LL2032: _temp2031=(( struct Cyc_Absyn_FnType_struct*) _temp1933)->f1;
_LL2044: _temp2043= _temp2031.tvars; goto _LL2042; _LL2042: _temp2041= _temp2031.effect;
goto _LL2040; _LL2040: _temp2039=( void*) _temp2031.ret_typ; goto _LL2038;
_LL2038: _temp2037= _temp2031.args; goto _LL2036; _LL2036: _temp2035= _temp2031.varargs;
goto _LL2034; _LL2034: _temp2033= _temp2031.attributes; goto _LL1952;} else{
goto _LL1953;} _LL1953: if(( unsigned int) _temp1933 > 4u?*(( int*) _temp1933)
== Cyc_Absyn_TupleType: 0){ _LL2046: _temp2045=(( struct Cyc_Absyn_TupleType_struct*)
_temp1933)->f1; goto _LL1954;} else{ goto _LL1955;} _LL1955: if(( unsigned int)
_temp1933 > 4u?*(( int*) _temp1933) == Cyc_Absyn_AnonStructType: 0){ _LL2048:
_temp2047=(( struct Cyc_Absyn_AnonStructType_struct*) _temp1933)->f1; goto
_LL1956;} else{ goto _LL1957;} _LL1957: if(( unsigned int) _temp1933 > 4u?*((
int*) _temp1933) == Cyc_Absyn_AnonUnionType: 0){ _LL2050: _temp2049=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp1933)->f1; goto _LL1958;} else{ goto
_LL1959;} _LL1959: if(( unsigned int) _temp1933 > 4u?*(( int*) _temp1933) == Cyc_Absyn_Evar:
0){ _LL2056: _temp2055=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp1933)->f1;
goto _LL2054; _LL2054: _temp2053=(( struct Cyc_Absyn_Evar_struct*) _temp1933)->f2;
goto _LL2052; _LL2052: _temp2051=(( struct Cyc_Absyn_Evar_struct*) _temp1933)->f3;
goto _LL1960;} else{ goto _LL1961;} _LL1961: if(( unsigned int) _temp1933 > 4u?*((
int*) _temp1933) == Cyc_Absyn_RgnHandleType: 0){ _LL2058: _temp2057=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp1933)->f1; goto _LL1962;} else{
goto _LL1963;} _LL1963: if(( unsigned int) _temp1933 > 4u?*(( int*) _temp1933)
== Cyc_Absyn_EnumType: 0){ _LL2062: _temp2061=(( struct Cyc_Absyn_EnumType_struct*)
_temp1933)->f1; goto _LL2060; _LL2060: _temp2059=(( struct Cyc_Absyn_EnumType_struct*)
_temp1933)->f2; goto _LL1964;} else{ goto _LL1965;} _LL1965: if( _temp1933 ==(
void*) Cyc_Absyn_VoidType){ goto _LL1966;} else{ goto _LL1967;} _LL1967: if((
unsigned int) _temp1933 > 4u?*(( int*) _temp1933) == Cyc_Absyn_IntType: 0){
_LL2066: _temp2065=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1933)->f1;
goto _LL2064; _LL2064: _temp2063=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1933)->f2; goto _LL1968;} else{ goto _LL1969;} _LL1969: if( _temp1933 ==(
void*) Cyc_Absyn_FloatType){ goto _LL1970;} else{ goto _LL1971;} _LL1971: if(
_temp1933 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1972;} else{ goto _LL1973;}
_LL1973: if( _temp1933 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL1974;} else{ goto
_LL1975;} _LL1975: if(( unsigned int) _temp1933 > 4u?*(( int*) _temp1933) == Cyc_Absyn_AccessEff:
0){ _LL2068: _temp2067=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp1933)->f1;
goto _LL1976;} else{ goto _LL1977;} _LL1977: if(( unsigned int) _temp1933 > 4u?*((
int*) _temp1933) == Cyc_Absyn_JoinEff: 0){ _LL2070: _temp2069=(( struct Cyc_Absyn_JoinEff_struct*)
_temp1933)->f1; goto _LL1978;} else{ goto _LL1934;} _LL1936: { struct
_handler_cons _temp2071; _push_handler(& _temp2071);{ int _temp2073= 0; if(
setjmp( _temp2071.handler)){ _temp2073= 1;} if( ! _temp2073){{ void* _temp2074=((
void*(*)( int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_assoc_cmp)( Cyc_Absyn_tvar_cmp,
inst, _temp1979); _npop_handler( 0u); return _temp2074;}; _pop_handler();} else{
void* _temp2072=( void*) _exn_thrown; void* _temp2076= _temp2072; _LL2078: if(
_temp2076 == Cyc_Core_Not_found){ goto _LL2079;} else{ goto _LL2080;} _LL2080:
goto _LL2081; _LL2079: return t; _LL2081:( void) _throw( _temp2076); _LL2077:;}}}
_LL1938: { struct Cyc_List_List* _temp2082= Cyc_Tcutil_substs( rgn, inst,
_temp1983); goto _LL2083; _LL2083: return _temp2082 == _temp1983? t:( void*)({
struct Cyc_Absyn_StructType_struct* _temp2084=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp2084[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp2085; _temp2085.tag= Cyc_Absyn_StructType;
_temp2085.f1= _temp1985; _temp2085.f2= _temp2082; _temp2085.f3= _temp1981;
_temp2085;}); _temp2084;});} _LL1940: { struct Cyc_List_List* _temp2086= Cyc_Tcutil_substs(
rgn, inst, _temp1989); goto _LL2087; _LL2087: return _temp2086 == _temp1989? t:(
void*)({ struct Cyc_Absyn_UnionType_struct* _temp2088=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp2088[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp2089; _temp2089.tag= Cyc_Absyn_UnionType;
_temp2089.f1= _temp1991; _temp2089.f2= _temp2086; _temp2089.f3= _temp1987;
_temp2089;}); _temp2088;});} _LL1942: { struct Cyc_List_List* _temp2090= Cyc_Tcutil_substs(
rgn, inst, _temp1997); goto _LL2091; _LL2091: { void* _temp2092= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp1995); goto _LL2093; _LL2093: return( _temp2090 == _temp1997?
_temp2092 == _temp1995: 0)? t:( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp2094=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp2094[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp2095; _temp2095.tag=
Cyc_Absyn_TunionType; _temp2095.f1=({ struct Cyc_Absyn_TunionInfo _temp2096;
_temp2096.tunion_info=( void*) _temp1999; _temp2096.targs= _temp2090; _temp2096.rgn=(
void*) _temp2092; _temp2096;}); _temp2095;}); _temp2094;});}} _LL1944: { struct
Cyc_List_List* _temp2097= Cyc_Tcutil_substs( rgn, inst, _temp2003); goto _LL2098;
_LL2098: return _temp2097 == _temp2003? t:( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp2099=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp2099[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp2100; _temp2100.tag= Cyc_Absyn_TunionFieldType; _temp2100.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp2101; _temp2101.field_info=( void*) _temp2005; _temp2101.targs= _temp2097;
_temp2101;}); _temp2100;}); _temp2099;});} _LL1946: { struct Cyc_List_List*
_temp2102= Cyc_Tcutil_substs( rgn, inst, _temp2009); goto _LL2103; _LL2103:
return _temp2102 == _temp2009? t:( void*)({ struct Cyc_Absyn_TypedefType_struct*
_temp2104=( struct Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp2104[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp2105; _temp2105.tag=
Cyc_Absyn_TypedefType; _temp2105.f1= _temp2011; _temp2105.f2= _temp2102;
_temp2105.f3= _temp2007; _temp2105;}); _temp2104;});} _LL1948: { void* _temp2106=
Cyc_Tcutil_rsubstitute( rgn, inst, _temp2017); goto _LL2107; _LL2107: return
_temp2106 == _temp2017? t:( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp2108=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp2108[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp2109; _temp2109.tag= Cyc_Absyn_ArrayType;
_temp2109.f1=( void*) _temp2106; _temp2109.f2= _temp2015; _temp2109.f3=
_temp2013; _temp2109;}); _temp2108;});} _LL1950: { void* _temp2110= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2029); goto _LL2111; _LL2111: { void* _temp2112= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2027); goto _LL2113; _LL2113: if( _temp2110 == _temp2029?
_temp2112 == _temp2027: 0){ return t;} return( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp2114=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp2114[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp2115; _temp2115.tag=
Cyc_Absyn_PointerType; _temp2115.f1=({ struct Cyc_Absyn_PtrInfo _temp2116;
_temp2116.elt_typ=( void*) _temp2110; _temp2116.rgn_typ=( void*) _temp2112;
_temp2116.nullable= _temp2025; _temp2116.tq= _temp2023; _temp2116.bounds=
_temp2021; _temp2116;}); _temp2115;}); _temp2114;});}} _LL1952:{ struct Cyc_List_List*
_temp2117= _temp2043; goto _LL2118; _LL2118: for( 0; _temp2117 != 0; _temp2117=((
struct Cyc_List_List*) _check_null( _temp2117))->tl){ inst=({ struct Cyc_List_List*
_temp2119=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp2119->hd=( void*)({ struct _tuple5* _temp2120=( struct _tuple5*)
_region_malloc( rgn, sizeof( struct _tuple5)); _temp2120->f1=( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( _temp2117))->hd; _temp2120->f2=( void*)({
struct Cyc_Absyn_VarType_struct* _temp2121=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp2121[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp2122; _temp2122.tag= Cyc_Absyn_VarType; _temp2122.f1=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( _temp2117))->hd;
_temp2122;}); _temp2121;}); _temp2120;}); _temp2119->tl= inst; _temp2119;});}}{
struct Cyc_List_List* _temp2125; struct Cyc_List_List* _temp2127; struct _tuple0
_temp2123=(( struct _tuple0(*)( struct _RegionHandle* r3, struct _RegionHandle*
r4, struct Cyc_List_List* x)) Cyc_List_rsplit)( rgn, rgn,(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple14*(* f)( struct _RegionHandle*, struct
_tuple2*), struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)(
rgn, Cyc_Tcutil_substitute_f1, rgn, _temp2037)); _LL2128: _temp2127= _temp2123.f1;
goto _LL2126; _LL2126: _temp2125= _temp2123.f2; goto _LL2124; _LL2124: { struct
Cyc_List_List* _temp2129= Cyc_Tcutil_substs( rgn, inst, _temp2125); goto _LL2130;
_LL2130: { struct Cyc_List_List* _temp2131=(( struct Cyc_List_List*(*)( struct
_tuple2*(* f)( struct _tuple14*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_substitute_f2,((
struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct _RegionHandle* r4,
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn,
_temp2127, _temp2129)); goto _LL2132; _LL2132: { struct Cyc_Core_Opt* eff2; if(
_temp2041 == 0){ eff2= 0;} else{ void* _temp2133= Cyc_Tcutil_rsubstitute( rgn,
inst,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp2041))->v); goto _LL2134;
_LL2134: if( _temp2133 ==( void*)(( struct Cyc_Core_Opt*) _check_null( _temp2041))->v){
eff2= _temp2041;} else{ eff2=({ struct Cyc_Core_Opt* _temp2135=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2135->v=( void*) _temp2133;
_temp2135;});}} return( void*)({ struct Cyc_Absyn_FnType_struct* _temp2136=(
struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp2136[ 0]=({ struct Cyc_Absyn_FnType_struct _temp2137; _temp2137.tag= Cyc_Absyn_FnType;
_temp2137.f1=({ struct Cyc_Absyn_FnInfo _temp2138; _temp2138.tvars= _temp2043;
_temp2138.effect= eff2; _temp2138.ret_typ=( void*) Cyc_Tcutil_rsubstitute( rgn,
inst, _temp2039); _temp2138.args= _temp2131; _temp2138.varargs= _temp2035;
_temp2138.attributes= _temp2033; _temp2138;}); _temp2137;}); _temp2136;});}}}}
_LL1954: { struct Cyc_List_List* _temp2139=(( struct Cyc_List_List*(*)( struct
_RegionHandle*, void*(* f)( struct _tuple4*), struct Cyc_List_List* x)) Cyc_List_rmap)(
rgn, Cyc_Tcutil_snd_tqt, _temp2045); goto _LL2140; _LL2140: { struct Cyc_List_List*
_temp2141= Cyc_Tcutil_substs( rgn, inst, _temp2139); goto _LL2142; _LL2142: if(
_temp2141 == _temp2139){ return t;}{ struct Cyc_List_List* _temp2143=(( struct
Cyc_List_List*(*)( struct _tuple4*(* f)( struct _tuple4*, void*), struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_map2)( Cyc_Tcutil_map2_tq, _temp2045,
_temp2141); goto _LL2144; _LL2144: return( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp2145=( struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp2145[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp2146; _temp2146.tag= Cyc_Absyn_TupleType;
_temp2146.f1= _temp2143; _temp2146;}); _temp2145;});}}} _LL1956: { struct Cyc_List_List*
_temp2147=(( struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct
Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type,
_temp2047); goto _LL2148; _LL2148: { struct Cyc_List_List* _temp2149= Cyc_Tcutil_substs(
rgn, inst, _temp2147); goto _LL2150; _LL2150: if( _temp2149 == _temp2147){
return t;}{ struct Cyc_List_List* _temp2151=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Structfield*(* f)( struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type,
_temp2047, _temp2149); goto _LL2152; _LL2152: return( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp2153=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp2153[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp2154; _temp2154.tag= Cyc_Absyn_AnonStructType; _temp2154.f1= _temp2151;
_temp2154;}); _temp2153;});}}} _LL1958: { struct Cyc_List_List* _temp2155=((
struct Cyc_List_List*(*)( struct _RegionHandle*, void*(* f)( struct Cyc_Absyn_Structfield*),
struct Cyc_List_List* x)) Cyc_List_rmap)( rgn, Cyc_Tcutil_field_type, _temp2049);
goto _LL2156; _LL2156: { struct Cyc_List_List* _temp2157= Cyc_Tcutil_substs( rgn,
inst, _temp2155); goto _LL2158; _LL2158: if( _temp2157 == _temp2155){ return t;}{
struct Cyc_List_List* _temp2159=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Absyn_Structfield*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_map2)( Cyc_Tcutil_zip_field_type, _temp2049, _temp2157); goto
_LL2160; _LL2160: return( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp2161=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp2161[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp2162; _temp2162.tag= Cyc_Absyn_AnonStructType; _temp2162.f1= _temp2159;
_temp2162;}); _temp2161;});}}} _LL1960: if( _temp2053 != 0){ return Cyc_Tcutil_rsubstitute(
rgn, inst,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp2053))->v);} else{
return t;} _LL1962: { void* _temp2163= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2057); goto _LL2164; _LL2164: return _temp2163 == _temp2057? t:( void*)({
struct Cyc_Absyn_RgnHandleType_struct* _temp2165=( struct Cyc_Absyn_RgnHandleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp2165[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp2166; _temp2166.tag= Cyc_Absyn_RgnHandleType;
_temp2166.f1=( void*) _temp2163; _temp2166;}); _temp2165;});} _LL1964: return t;
_LL1966: return t; _LL1968: return t; _LL1970: return t; _LL1972: return t;
_LL1974: return t; _LL1976: { void* _temp2167= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2067); goto _LL2168; _LL2168: return _temp2167 == _temp2067? t:( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp2169=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp2169[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp2170; _temp2170.tag= Cyc_Absyn_AccessEff;
_temp2170.f1=( void*) _temp2167; _temp2170;}); _temp2169;});} _LL1978: { struct
Cyc_List_List* _temp2171= Cyc_Tcutil_substs( rgn, inst, _temp2069); goto _LL2172;
_LL2172: return _temp2171 == _temp2069? t:( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp2173=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2173[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2174; _temp2174.tag= Cyc_Absyn_JoinEff;
_temp2174.f1= _temp2171; _temp2174;}); _temp2173;});} _LL1934:;} static struct
Cyc_List_List* Cyc_Tcutil_substs( struct _RegionHandle* rgn, struct Cyc_List_List*
inst, struct Cyc_List_List* ts){ if( ts == 0){ return 0;}{ void* _temp2175=(
void*)(( struct Cyc_List_List*) _check_null( ts))->hd; goto _LL2176; _LL2176: {
struct Cyc_List_List* _temp2177=(( struct Cyc_List_List*) _check_null( ts))->tl;
goto _LL2178; _LL2178: { void* _temp2179= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2175); goto _LL2180; _LL2180: { struct Cyc_List_List* _temp2181= Cyc_Tcutil_substs(
rgn, inst, _temp2177); goto _LL2182; _LL2182: if( _temp2175 == _temp2179?
_temp2177 == _temp2181: 0){ return ts;} return( struct Cyc_List_List*)({ struct
Cyc_List_List* _temp2183=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2183->hd=( void*) _temp2179; _temp2183->tl= _temp2181; _temp2183;});}}}}}
extern void* Cyc_Tcutil_substitute( struct Cyc_List_List* inst, void* t){ return
Cyc_Tcutil_rsubstitute( Cyc_Core_heap_region, inst, t);} struct Cyc_Absyn_Exp*
Cyc_Tcutil_default_initializer( struct Cyc_Tcenv_Tenv* te, void* t, struct Cyc_Position_Segment*
loc){ struct Cyc_Absyn_Exp* e= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2233=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2233[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2234; _temp2234.tag= Cyc_Absyn_Const_e;
_temp2234.f1=( void*)(( void*) Cyc_Absyn_Null_c); _temp2234;}); _temp2233;}),
loc);{ void* _temp2184= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp2198; void* _temp2200; void* _temp2202; void* _temp2204; void* _temp2206;
_LL2186: if(( unsigned int) _temp2184 > 4u?*(( int*) _temp2184) == Cyc_Absyn_PointerType:
0){ _LL2199: _temp2198=(( struct Cyc_Absyn_PointerType_struct*) _temp2184)->f1;
goto _LL2187;} else{ goto _LL2188;} _LL2188: if(( unsigned int) _temp2184 > 4u?*((
int*) _temp2184) == Cyc_Absyn_IntType: 0){ _LL2203: _temp2202=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp2184)->f1; goto _LL2201; _LL2201: _temp2200=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp2184)->f2; if( _temp2200 ==(
void*) Cyc_Absyn_B1){ goto _LL2189;} else{ goto _LL2190;}} else{ goto _LL2190;}
_LL2190: if(( unsigned int) _temp2184 > 4u?*(( int*) _temp2184) == Cyc_Absyn_IntType:
0){ _LL2207: _temp2206=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2184)->f1;
goto _LL2205; _LL2205: _temp2204=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2184)->f2; goto _LL2191;} else{ goto _LL2192;} _LL2192: if( _temp2184 ==(
void*) Cyc_Absyn_FloatType){ goto _LL2193;} else{ goto _LL2194;} _LL2194: if(
_temp2184 ==( void*) Cyc_Absyn_DoubleType){ goto _LL2195;} else{ goto _LL2196;}
_LL2196: goto _LL2197; _LL2187: goto _LL2185; _LL2189:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp2208=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2208[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2209; _temp2209.tag= Cyc_Absyn_Const_e; _temp2209.f1=(
void*)(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp2210=( struct Cyc_Absyn_Char_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct)); _temp2210[ 0]=({ struct Cyc_Absyn_Char_c_struct
_temp2211; _temp2211.tag= Cyc_Absyn_Char_c; _temp2211.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp2211.f2='\000'; _temp2211;}); _temp2210;})); _temp2209;}); _temp2208;})));
goto _LL2185; _LL2191:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2212=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2212[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2213; _temp2213.tag= Cyc_Absyn_Const_e;
_temp2213.f1=( void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp2214=(
struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp2214[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp2215; _temp2215.tag= Cyc_Absyn_Int_c;
_temp2215.f1=( void*) _temp2206; _temp2215.f2= 0; _temp2215;}); _temp2214;}));
_temp2213;}); _temp2212;}))); if( _temp2204 !=( void*) Cyc_Absyn_B4){ e= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Cast_e_struct* _temp2216=( struct Cyc_Absyn_Cast_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp2216[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp2217; _temp2217.tag= Cyc_Absyn_Cast_e; _temp2217.f1=( void*) t; _temp2217.f2=
e; _temp2217;}); _temp2216;}), loc);} goto _LL2185; _LL2193:( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp2218=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2218[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2219; _temp2219.tag= Cyc_Absyn_Const_e; _temp2219.f1=(
void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp2220=( struct Cyc_Absyn_Float_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Float_c_struct)); _temp2220[ 0]=({ struct
Cyc_Absyn_Float_c_struct _temp2221; _temp2221.tag= Cyc_Absyn_Float_c; _temp2221.f1=({
char* _temp2222="0.0"; struct _tagged_string _temp2223; _temp2223.curr=
_temp2222; _temp2223.base= _temp2222; _temp2223.last_plus_one= _temp2222 + 4;
_temp2223;}); _temp2221;}); _temp2220;})); _temp2219;}); _temp2218;}))); goto
_LL2185; _LL2195:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Cast_e_struct*
_temp2224=( struct Cyc_Absyn_Cast_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct));
_temp2224[ 0]=({ struct Cyc_Absyn_Cast_e_struct _temp2225; _temp2225.tag= Cyc_Absyn_Cast_e;
_temp2225.f1=( void*) t; _temp2225.f2= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2226=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2226[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2227; _temp2227.tag= Cyc_Absyn_Const_e;
_temp2227.f1=( void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp2228=(
struct Cyc_Absyn_Float_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Float_c_struct));
_temp2228[ 0]=({ struct Cyc_Absyn_Float_c_struct _temp2229; _temp2229.tag= Cyc_Absyn_Float_c;
_temp2229.f1=({ char* _temp2230="0.0"; struct _tagged_string _temp2231;
_temp2231.curr= _temp2230; _temp2231.base= _temp2230; _temp2231.last_plus_one=
_temp2230 + 4; _temp2231;}); _temp2229;}); _temp2228;})); _temp2227;});
_temp2226;}), loc); _temp2225;}); _temp2224;}))); goto _LL2185; _LL2197: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2232= Cyc_Absynpp_typ2string( t); xprintf("declaration of type %.*s requires initializer",
_temp2232.last_plus_one - _temp2232.curr, _temp2232.curr);})); goto _LL2185;
_LL2185:;} return e;} struct _tuple5* Cyc_Tcutil_make_inst_var( struct Cyc_Absyn_Tvar*
tv){ void* k=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( tv->kind);
return({ struct _tuple5* _temp2235=( struct _tuple5*) GC_malloc( sizeof( struct
_tuple5)); _temp2235->f1= tv; _temp2235->f2= Cyc_Absyn_new_evar( k); _temp2235;});}
struct _tuple5* Cyc_Tcutil_r_make_inst_var( struct _RegionHandle* rgn, struct
Cyc_Absyn_Tvar* tv){ void* k=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
tv->kind); return({ struct _tuple5* _temp2236=( struct _tuple5*) _region_malloc(
rgn, sizeof( struct _tuple5)); _temp2236->f1= tv; _temp2236->f2= Cyc_Absyn_new_evar(
k); _temp2236;});} struct Cyc_List_List* Cyc_Tcutil_check_valid_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* free_vars, void*
expected_kind, void* t){{ void* _temp2237= Cyc_Tcutil_compress( t); int
_temp2283; struct Cyc_Core_Opt* _temp2285; void* _temp2287; struct Cyc_Absyn_Tvar*
_temp2289; struct Cyc_Absyn_Enumdecl* _temp2291; struct Cyc_Absyn_Enumdecl**
_temp2293; struct _tuple1* _temp2294; struct Cyc_Absyn_TunionInfo _temp2296;
void* _temp2298; struct Cyc_List_List* _temp2300; void* _temp2302; void**
_temp2304; struct Cyc_Absyn_TunionFieldInfo _temp2305; struct Cyc_List_List*
_temp2307; void* _temp2309; void** _temp2311; struct Cyc_Absyn_PtrInfo _temp2312;
struct Cyc_Absyn_Conref* _temp2314; struct Cyc_Absyn_Tqual _temp2316; struct Cyc_Absyn_Conref*
_temp2318; void* _temp2320; void* _temp2322; void* _temp2324; void* _temp2326;
struct Cyc_Absyn_Exp* _temp2328; struct Cyc_Absyn_Tqual _temp2330; void*
_temp2332; struct Cyc_Absyn_FnInfo _temp2334; struct Cyc_List_List* _temp2336;
int _temp2338; struct Cyc_List_List* _temp2340; void* _temp2342; struct Cyc_Core_Opt*
_temp2344; struct Cyc_Core_Opt** _temp2346; struct Cyc_List_List* _temp2347;
struct Cyc_List_List** _temp2349; struct Cyc_List_List* _temp2350; struct Cyc_List_List*
_temp2352; struct Cyc_List_List* _temp2354; struct Cyc_Absyn_Structdecl**
_temp2356; struct Cyc_Absyn_Structdecl*** _temp2358; struct Cyc_List_List*
_temp2359; struct _tuple1* _temp2361; struct Cyc_Absyn_Uniondecl** _temp2363;
struct Cyc_Absyn_Uniondecl*** _temp2365; struct Cyc_List_List* _temp2366; struct
_tuple1* _temp2368; struct Cyc_Core_Opt* _temp2370; struct Cyc_Core_Opt**
_temp2372; struct Cyc_List_List* _temp2373; struct _tuple1* _temp2375; void*
_temp2377; void* _temp2379; struct Cyc_List_List* _temp2381; _LL2239: if(
_temp2237 ==( void*) Cyc_Absyn_VoidType){ goto _LL2240;} else{ goto _LL2241;}
_LL2241: if(( unsigned int) _temp2237 > 4u?*(( int*) _temp2237) == Cyc_Absyn_Evar:
0){ _LL2288: _temp2287=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp2237)->f1;
goto _LL2286; _LL2286: _temp2285=(( struct Cyc_Absyn_Evar_struct*) _temp2237)->f2;
goto _LL2284; _LL2284: _temp2283=(( struct Cyc_Absyn_Evar_struct*) _temp2237)->f3;
goto _LL2242;} else{ goto _LL2243;} _LL2243: if(( unsigned int) _temp2237 > 4u?*((
int*) _temp2237) == Cyc_Absyn_VarType: 0){ _LL2290: _temp2289=(( struct Cyc_Absyn_VarType_struct*)
_temp2237)->f1; goto _LL2244;} else{ goto _LL2245;} _LL2245: if(( unsigned int)
_temp2237 > 4u?*(( int*) _temp2237) == Cyc_Absyn_EnumType: 0){ _LL2295:
_temp2294=(( struct Cyc_Absyn_EnumType_struct*) _temp2237)->f1; goto _LL2292;
_LL2292: _temp2291=(( struct Cyc_Absyn_EnumType_struct*) _temp2237)->f2;
_temp2293=&(( struct Cyc_Absyn_EnumType_struct*) _temp2237)->f2; goto _LL2246;}
else{ goto _LL2247;} _LL2247: if(( unsigned int) _temp2237 > 4u?*(( int*)
_temp2237) == Cyc_Absyn_TunionType: 0){ _LL2297: _temp2296=(( struct Cyc_Absyn_TunionType_struct*)
_temp2237)->f1; _LL2303: _temp2302=( void*) _temp2296.tunion_info; _temp2304=&(((
struct Cyc_Absyn_TunionType_struct*) _temp2237)->f1).tunion_info; goto _LL2301;
_LL2301: _temp2300= _temp2296.targs; goto _LL2299; _LL2299: _temp2298=( void*)
_temp2296.rgn; goto _LL2248;} else{ goto _LL2249;} _LL2249: if(( unsigned int)
_temp2237 > 4u?*(( int*) _temp2237) == Cyc_Absyn_TunionFieldType: 0){ _LL2306:
_temp2305=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp2237)->f1; _LL2310:
_temp2309=( void*) _temp2305.field_info; _temp2311=&((( struct Cyc_Absyn_TunionFieldType_struct*)
_temp2237)->f1).field_info; goto _LL2308; _LL2308: _temp2307= _temp2305.targs;
goto _LL2250;} else{ goto _LL2251;} _LL2251: if(( unsigned int) _temp2237 > 4u?*((
int*) _temp2237) == Cyc_Absyn_PointerType: 0){ _LL2313: _temp2312=(( struct Cyc_Absyn_PointerType_struct*)
_temp2237)->f1; _LL2323: _temp2322=( void*) _temp2312.elt_typ; goto _LL2321;
_LL2321: _temp2320=( void*) _temp2312.rgn_typ; goto _LL2319; _LL2319: _temp2318=
_temp2312.nullable; goto _LL2317; _LL2317: _temp2316= _temp2312.tq; goto _LL2315;
_LL2315: _temp2314= _temp2312.bounds; goto _LL2252;} else{ goto _LL2253;}
_LL2253: if(( unsigned int) _temp2237 > 4u?*(( int*) _temp2237) == Cyc_Absyn_IntType:
0){ _LL2327: _temp2326=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2237)->f1;
goto _LL2325; _LL2325: _temp2324=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2237)->f2; goto _LL2254;} else{ goto _LL2255;} _LL2255: if( _temp2237 ==(
void*) Cyc_Absyn_FloatType){ goto _LL2256;} else{ goto _LL2257;} _LL2257: if(
_temp2237 ==( void*) Cyc_Absyn_DoubleType){ goto _LL2258;} else{ goto _LL2259;}
_LL2259: if(( unsigned int) _temp2237 > 4u?*(( int*) _temp2237) == Cyc_Absyn_ArrayType:
0){ _LL2333: _temp2332=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2237)->f1;
goto _LL2331; _LL2331: _temp2330=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2237)->f2; goto _LL2329; _LL2329: _temp2328=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2237)->f3; goto _LL2260;} else{ goto _LL2261;} _LL2261: if(( unsigned int)
_temp2237 > 4u?*(( int*) _temp2237) == Cyc_Absyn_FnType: 0){ _LL2335: _temp2334=((
struct Cyc_Absyn_FnType_struct*) _temp2237)->f1; _LL2348: _temp2347= _temp2334.tvars;
_temp2349=&((( struct Cyc_Absyn_FnType_struct*) _temp2237)->f1).tvars; goto
_LL2345; _LL2345: _temp2344= _temp2334.effect; _temp2346=&((( struct Cyc_Absyn_FnType_struct*)
_temp2237)->f1).effect; goto _LL2343; _LL2343: _temp2342=( void*) _temp2334.ret_typ;
goto _LL2341; _LL2341: _temp2340= _temp2334.args; goto _LL2339; _LL2339:
_temp2338= _temp2334.varargs; goto _LL2337; _LL2337: _temp2336= _temp2334.attributes;
goto _LL2262;} else{ goto _LL2263;} _LL2263: if(( unsigned int) _temp2237 > 4u?*((
int*) _temp2237) == Cyc_Absyn_TupleType: 0){ _LL2351: _temp2350=(( struct Cyc_Absyn_TupleType_struct*)
_temp2237)->f1; goto _LL2264;} else{ goto _LL2265;} _LL2265: if(( unsigned int)
_temp2237 > 4u?*(( int*) _temp2237) == Cyc_Absyn_AnonStructType: 0){ _LL2353:
_temp2352=(( struct Cyc_Absyn_AnonStructType_struct*) _temp2237)->f1; goto
_LL2266;} else{ goto _LL2267;} _LL2267: if(( unsigned int) _temp2237 > 4u?*((
int*) _temp2237) == Cyc_Absyn_AnonUnionType: 0){ _LL2355: _temp2354=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp2237)->f1; goto _LL2268;} else{ goto
_LL2269;} _LL2269: if(( unsigned int) _temp2237 > 4u?*(( int*) _temp2237) == Cyc_Absyn_StructType:
0){ _LL2362: _temp2361=(( struct Cyc_Absyn_StructType_struct*) _temp2237)->f1;
goto _LL2360; _LL2360: _temp2359=(( struct Cyc_Absyn_StructType_struct*)
_temp2237)->f2; goto _LL2357; _LL2357: _temp2356=(( struct Cyc_Absyn_StructType_struct*)
_temp2237)->f3; _temp2358=&(( struct Cyc_Absyn_StructType_struct*) _temp2237)->f3;
goto _LL2270;} else{ goto _LL2271;} _LL2271: if(( unsigned int) _temp2237 > 4u?*((
int*) _temp2237) == Cyc_Absyn_UnionType: 0){ _LL2369: _temp2368=(( struct Cyc_Absyn_UnionType_struct*)
_temp2237)->f1; goto _LL2367; _LL2367: _temp2366=(( struct Cyc_Absyn_UnionType_struct*)
_temp2237)->f2; goto _LL2364; _LL2364: _temp2363=(( struct Cyc_Absyn_UnionType_struct*)
_temp2237)->f3; _temp2365=&(( struct Cyc_Absyn_UnionType_struct*) _temp2237)->f3;
goto _LL2272;} else{ goto _LL2273;} _LL2273: if(( unsigned int) _temp2237 > 4u?*((
int*) _temp2237) == Cyc_Absyn_TypedefType: 0){ _LL2376: _temp2375=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2237)->f1; goto _LL2374; _LL2374: _temp2373=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2237)->f2; goto _LL2371; _LL2371: _temp2370=(( struct Cyc_Absyn_TypedefType_struct*)
_temp2237)->f3; _temp2372=&(( struct Cyc_Absyn_TypedefType_struct*) _temp2237)->f3;
goto _LL2274;} else{ goto _LL2275;} _LL2275: if( _temp2237 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL2276;} else{ goto _LL2277;} _LL2277: if(( unsigned int) _temp2237 > 4u?*((
int*) _temp2237) == Cyc_Absyn_RgnHandleType: 0){ _LL2378: _temp2377=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp2237)->f1; goto _LL2278;} else{
goto _LL2279;} _LL2279: if(( unsigned int) _temp2237 > 4u?*(( int*) _temp2237)
== Cyc_Absyn_AccessEff: 0){ _LL2380: _temp2379=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp2237)->f1; goto _LL2280;} else{ goto _LL2281;} _LL2281: if(( unsigned int)
_temp2237 > 4u?*(( int*) _temp2237) == Cyc_Absyn_JoinEff: 0){ _LL2382: _temp2381=((
struct Cyc_Absyn_JoinEff_struct*) _temp2237)->f1; goto _LL2282;} else{ goto
_LL2238;} _LL2240: goto _LL2238; _LL2242: goto _LL2238; _LL2244: { struct Cyc_Absyn_Conref*
c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp2289->kind); if(( void*) c->v ==( void*) Cyc_Absyn_No_constr){( void*)( c->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp2383=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp2383[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp2384; _temp2384.tag= Cyc_Absyn_Eq_constr;
_temp2384.f1=( void*) expected_kind; _temp2384;}); _temp2383;})));} free_vars=
Cyc_Tcutil_add_free_tvar( loc, free_vars, _temp2289); goto _LL2238;} _LL2246:
if(* _temp2293 == 0){ struct _handler_cons _temp2385; _push_handler(& _temp2385);{
int _temp2387= 0; if( setjmp( _temp2385.handler)){ _temp2387= 1;} if( !
_temp2387){{ struct Cyc_Absyn_Enumdecl** ed= Cyc_Tcenv_lookup_enumdecl( te, loc,
_temp2294);* _temp2293=( struct Cyc_Absyn_Enumdecl*)* ed;}; _pop_handler();}
else{ void* _temp2386=( void*) _exn_thrown; void* _temp2389= _temp2386; _LL2391:
if( _temp2389 == Cyc_Dict_Absent){ goto _LL2392;} else{ goto _LL2393;} _LL2393:
goto _LL2394; _LL2392: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2395=
Cyc_Absynpp_qvar2string( _temp2294); xprintf("unbound type enum %.*s", _temp2395.last_plus_one
- _temp2395.curr, _temp2395.curr);})); return free_vars; _LL2394:( void) _throw(
_temp2389); _LL2390:;}}}{ struct Cyc_Absyn_Enumdecl* ed=( struct Cyc_Absyn_Enumdecl*)
_check_null(* _temp2293);* _temp2294=( ed->name)[ _check_known_subscript_notnull(
1u, 0)]; goto _LL2238;} _LL2248:{ void* _temp2396=* _temp2304; struct Cyc_Absyn_UnknownTunionInfo
_temp2402; int _temp2404; struct _tuple1* _temp2406; struct Cyc_Absyn_Tuniondecl*
_temp2408; _LL2398: if(*(( int*) _temp2396) == Cyc_Absyn_UnknownTunion){ _LL2403:
_temp2402=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp2396)->f1; _LL2407:
_temp2406= _temp2402.name; goto _LL2405; _LL2405: _temp2404= _temp2402.is_xtunion;
goto _LL2399;} else{ goto _LL2400;} _LL2400: if(*(( int*) _temp2396) == Cyc_Absyn_KnownTunion){
_LL2409: _temp2408=(( struct Cyc_Absyn_KnownTunion_struct*) _temp2396)->f1; goto
_LL2401;} else{ goto _LL2397;} _LL2399: { struct Cyc_Absyn_Tuniondecl* tud;{
struct _handler_cons _temp2410; _push_handler(& _temp2410);{ int _temp2412= 0;
if( setjmp( _temp2410.handler)){ _temp2412= 1;} if( ! _temp2412){ tud=* Cyc_Tcenv_lookup_tuniondecl(
te, loc, _temp2406);; _pop_handler();} else{ void* _temp2411=( void*)
_exn_thrown; void* _temp2414= _temp2411; _LL2416: if( _temp2414 == Cyc_Dict_Absent){
goto _LL2417;} else{ goto _LL2418;} _LL2418: goto _LL2419; _LL2417: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2420= Cyc_Absynpp_qvar2string( _temp2406);
xprintf("unbound type [x]tunion %.*s", _temp2420.last_plus_one - _temp2420.curr,
_temp2420.curr);})); return free_vars; _LL2419:( void) _throw( _temp2414);
_LL2415:;}}} if( tud->is_xtunion != _temp2404){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2421= Cyc_Absynpp_qvar2string( _temp2406); xprintf("[x]tunion is different from type declaration %.*s",
_temp2421.last_plus_one - _temp2421.curr, _temp2421.curr);}));}* _temp2304=(
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp2422=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp2422[ 0]=({
struct Cyc_Absyn_KnownTunion_struct _temp2423; _temp2423.tag= Cyc_Absyn_KnownTunion;
_temp2423.f1= tud; _temp2423;}); _temp2422;}); _temp2408= tud; goto _LL2401;}
_LL2401: free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars,( void*) Cyc_Absyn_RgnKind,
_temp2298);{ struct Cyc_List_List* tvs= _temp2408->tvs; for( 0; _temp2300 != 0?
tvs != 0: 0;( _temp2300=(( struct Cyc_List_List*) _check_null( _temp2300))->tl,
tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void* t1=( void*)((
struct Cyc_List_List*) _check_null( _temp2300))->hd; void* k1=(( void*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind); free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars, k1, t1);} if( _temp2300 != 0){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2424= Cyc_Absynpp_qvar2string( _temp2408->name);
xprintf("too many type arguments for tunion %.*s", _temp2424.last_plus_one -
_temp2424.curr, _temp2424.curr);}));} if( tvs != 0){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2425= Cyc_Absynpp_qvar2string( _temp2408->name);
xprintf("too few type arguments for tunion %.*s", _temp2425.last_plus_one -
_temp2425.curr, _temp2425.curr);}));} goto _LL2397;} _LL2397:;} goto _LL2238;
_LL2250:{ void* _temp2426=* _temp2311; struct Cyc_Absyn_UnknownTunionFieldInfo
_temp2432; int _temp2434; struct _tuple1* _temp2436; struct _tuple1* _temp2438;
struct Cyc_Absyn_Tunionfield* _temp2440; struct Cyc_Absyn_Tuniondecl* _temp2442;
_LL2428: if(*(( int*) _temp2426) == Cyc_Absyn_UnknownTunionfield){ _LL2433:
_temp2432=(( struct Cyc_Absyn_UnknownTunionfield_struct*) _temp2426)->f1;
_LL2439: _temp2438= _temp2432.tunion_name; goto _LL2437; _LL2437: _temp2436=
_temp2432.field_name; goto _LL2435; _LL2435: _temp2434= _temp2432.is_xtunion;
goto _LL2429;} else{ goto _LL2430;} _LL2430: if(*(( int*) _temp2426) == Cyc_Absyn_KnownTunionfield){
_LL2443: _temp2442=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp2426)->f1;
goto _LL2441; _LL2441: _temp2440=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp2426)->f2; goto _LL2431;} else{ goto _LL2427;} _LL2429: { struct Cyc_Absyn_Tuniondecl*
tud; struct Cyc_Absyn_Tunionfield* tuf;{ struct _handler_cons _temp2444;
_push_handler(& _temp2444);{ int _temp2446= 0; if( setjmp( _temp2444.handler)){
_temp2446= 1;} if( ! _temp2446){* Cyc_Tcenv_lookup_tuniondecl( te, loc,
_temp2438);; _pop_handler();} else{ void* _temp2445=( void*) _exn_thrown; void*
_temp2448= _temp2445; _LL2450: if( _temp2448 == Cyc_Dict_Absent){ goto _LL2451;}
else{ goto _LL2452;} _LL2452: goto _LL2453; _LL2451: Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2454= Cyc_Absynpp_qvar2string( _temp2438); xprintf("unbound type tunion %.*s",
_temp2454.last_plus_one - _temp2454.curr, _temp2454.curr);})); return free_vars;
_LL2453:( void) _throw( _temp2448); _LL2449:;}}}{ struct _handler_cons _temp2455;
_push_handler(& _temp2455);{ int _temp2457= 0; if( setjmp( _temp2455.handler)){
_temp2457= 1;} if( ! _temp2457){{ void* _temp2458= Cyc_Tcenv_lookup_ordinary( te,
loc, _temp2436); struct Cyc_Absyn_Tunionfield* _temp2464; struct Cyc_Absyn_Tuniondecl*
_temp2466; _LL2460: if(*(( int*) _temp2458) == Cyc_Tcenv_TunionRes){ _LL2467:
_temp2466=(( struct Cyc_Tcenv_TunionRes_struct*) _temp2458)->f1; goto _LL2465;
_LL2465: _temp2464=(( struct Cyc_Tcenv_TunionRes_struct*) _temp2458)->f2; goto
_LL2461;} else{ goto _LL2462;} _LL2462: goto _LL2463; _LL2461: tuf= _temp2464;
tud= _temp2466; if( tud->is_xtunion != _temp2434){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2468= Cyc_Absynpp_qvar2string( _temp2438); xprintf("[x]tunion is different from type declaration %.*s",
_temp2468.last_plus_one - _temp2468.curr, _temp2468.curr);}));} goto _LL2459;
_LL2463: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2469= Cyc_Absynpp_qvar2string(
_temp2436); struct _tagged_string _temp2470= Cyc_Absynpp_qvar2string( _temp2438);
xprintf("unbound field %.*s in type tunion %.*s", _temp2469.last_plus_one -
_temp2469.curr, _temp2469.curr, _temp2470.last_plus_one - _temp2470.curr,
_temp2470.curr);}));{ struct Cyc_List_List* _temp2471= free_vars; _npop_handler(
0u); return _temp2471;} _LL2459:;}; _pop_handler();} else{ void* _temp2456=(
void*) _exn_thrown; void* _temp2473= _temp2456; _LL2475: if( _temp2473 == Cyc_Dict_Absent){
goto _LL2476;} else{ goto _LL2477;} _LL2477: goto _LL2478; _LL2476: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2479= Cyc_Absynpp_qvar2string( _temp2436);
struct _tagged_string _temp2480= Cyc_Absynpp_qvar2string( _temp2438); xprintf("unbound field %.*s in type tunion %.*s",
_temp2479.last_plus_one - _temp2479.curr, _temp2479.curr, _temp2480.last_plus_one
- _temp2480.curr, _temp2480.curr);})); return free_vars; _LL2478:( void) _throw(
_temp2473); _LL2474:;}}}* _temp2311=( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp2481=( struct Cyc_Absyn_KnownTunionfield_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp2481[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp2482; _temp2482.tag= Cyc_Absyn_KnownTunionfield; _temp2482.f1= tud;
_temp2482.f2= tuf; _temp2482;}); _temp2481;}); _temp2442= tud; _temp2440= tuf;
goto _LL2431;} _LL2431: { struct Cyc_List_List* tvs= _temp2442->tvs; for( 0;
_temp2307 != 0? tvs != 0: 0;( _temp2307=(( struct Cyc_List_List*) _check_null(
_temp2307))->tl, tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void*
t1=( void*)(( struct Cyc_List_List*) _check_null( _temp2307))->hd; void* k1=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind); free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars, k1, t1);} if( _temp2307 != 0){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2483= Cyc_Absynpp_qvar2string( _temp2442->name);
struct _tagged_string _temp2484= Cyc_Absynpp_qvar2string( _temp2440->name);
xprintf("too many type arguments for tunion %.*s.%.*s", _temp2483.last_plus_one
- _temp2483.curr, _temp2483.curr, _temp2484.last_plus_one - _temp2484.curr,
_temp2484.curr);}));} if( tvs != 0){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2485= Cyc_Absynpp_qvar2string( _temp2442->name); struct
_tagged_string _temp2486= Cyc_Absynpp_qvar2string( _temp2440->name); xprintf("too few type arguments for tunion %.*s.%.*s",
_temp2485.last_plus_one - _temp2485.curr, _temp2485.curr, _temp2486.last_plus_one
- _temp2486.curr, _temp2486.curr);}));} goto _LL2427;} _LL2427:;} goto _LL2238;
_LL2252: free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars,( void*) Cyc_Absyn_AnyKind,
_temp2322); free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars,( void*)
Cyc_Absyn_RgnKind, _temp2320);{ void* _temp2487=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp2314))->v; void*
_temp2493; struct Cyc_Absyn_Exp* _temp2495; _LL2489: if(( unsigned int)
_temp2487 > 1u?*(( int*) _temp2487) == Cyc_Absyn_Eq_constr: 0){ _LL2494:
_temp2493=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2487)->f1; if((
unsigned int) _temp2493 > 1u?*(( int*) _temp2493) == Cyc_Absyn_Upper_b: 0){
_LL2496: _temp2495=(( struct Cyc_Absyn_Upper_b_struct*) _temp2493)->f1; goto
_LL2490;} else{ goto _LL2491;}} else{ goto _LL2491;} _LL2491: goto _LL2492;
_LL2490: if( ! Cyc_Tcutil_is_const_exp( te, _temp2495)){ Cyc_Tcutil_terr( loc,({
char* _temp2497="pointer bounds expression is not a constant"; struct
_tagged_string _temp2498; _temp2498.curr= _temp2497; _temp2498.base= _temp2497;
_temp2498.last_plus_one= _temp2497 + 44; _temp2498;}));} Cyc_Tcexp_tcExp( te, 0,
_temp2495); if( ! Cyc_Tcutil_coerce_uint_typ( te, _temp2495)){ Cyc_Tcutil_terr(
loc,({ char* _temp2499="pointer bounds expression is not an unsigned int";
struct _tagged_string _temp2500; _temp2500.curr= _temp2499; _temp2500.base=
_temp2499; _temp2500.last_plus_one= _temp2499 + 49; _temp2500;}));} Cyc_Evexp_eval_const_uint_exp(
_temp2495); goto _LL2488; _LL2492: goto _LL2488; _LL2488:;} goto _LL2238;
_LL2254: goto _LL2238; _LL2256: goto _LL2238; _LL2258: goto _LL2238; _LL2260:
free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars,( void*) Cyc_Absyn_MemKind,
_temp2332); if( _temp2328 == 0? 1: ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)
_check_null( _temp2328))){ Cyc_Tcutil_terr( loc,({ char* _temp2501="array bounds expression is not constant";
struct _tagged_string _temp2502; _temp2502.curr= _temp2501; _temp2502.base=
_temp2501; _temp2502.last_plus_one= _temp2501 + 40; _temp2502;}));} Cyc_Tcexp_tcExp(
te, 0,( struct Cyc_Absyn_Exp*) _check_null( _temp2328)); if( ! Cyc_Tcutil_coerce_uint_typ(
te,( struct Cyc_Absyn_Exp*) _check_null( _temp2328))){ Cyc_Tcutil_terr( loc,({
char* _temp2503="array bounds expression is not an unsigned int"; struct
_tagged_string _temp2504; _temp2504.curr= _temp2503; _temp2504.base= _temp2503;
_temp2504.last_plus_one= _temp2503 + 47; _temp2504;}));} Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp2328)); goto _LL2238; _LL2262: { int
num_convs= 0; int seen_cdecl= 0; int seen_stdcall= 0; int seen_fastcall= 0; for(
0; _temp2336 != 0; _temp2336=(( struct Cyc_List_List*) _check_null( _temp2336))->tl){
if( ! Cyc_Absyn_fntype_att(( void*)(( struct Cyc_List_List*) _check_null(
_temp2336))->hd)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2505= Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( _temp2336))->hd); xprintf("bad function type attribute %.*s",
_temp2505.last_plus_one - _temp2505.curr, _temp2505.curr);}));}{ void* _temp2506=(
void*)(( struct Cyc_List_List*) _check_null( _temp2336))->hd; _LL2508: if(
_temp2506 ==( void*) Cyc_Absyn_Stdcall_att){ goto _LL2509;} else{ goto _LL2510;}
_LL2510: if( _temp2506 ==( void*) Cyc_Absyn_Cdecl_att){ goto _LL2511;} else{
goto _LL2512;} _LL2512: if( _temp2506 ==( void*) Cyc_Absyn_Fastcall_att){ goto
_LL2513;} else{ goto _LL2514;} _LL2514: goto _LL2515; _LL2509: if( !
seen_stdcall){ seen_stdcall= 1; ++ num_convs;} goto _LL2507; _LL2511: if( !
seen_cdecl){ seen_cdecl= 1; ++ num_convs;} goto _LL2507; _LL2513: if( !
seen_fastcall){ seen_fastcall= 1; ++ num_convs;} goto _LL2507; _LL2515: goto
_LL2507; _LL2507:;}} if( num_convs > 1){ Cyc_Tcutil_terr( loc, xprintf("function can't have multiple calling conventions"));}
Cyc_Tcutil_check_unique_tvars( loc,* _temp2349);{ struct Cyc_List_List* b=*
_temp2349; for( 0; b != 0; b=(( struct Cyc_List_List*) _check_null( b))->tl){
void* _temp2516=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( b))->hd)->kind))->v; void* _temp2522; _LL2518: if(( unsigned int)
_temp2516 > 1u?*(( int*) _temp2516) == Cyc_Absyn_Eq_constr: 0){ _LL2523:
_temp2522=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2516)->f1; if(
_temp2522 ==( void*) Cyc_Absyn_MemKind){ goto _LL2519;} else{ goto _LL2520;}}
else{ goto _LL2520;} _LL2520: goto _LL2521; _LL2519: Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2524=*(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( b))->hd)->name; xprintf("function attempts to abstract Mem type variable %.*s",
_temp2524.last_plus_one - _temp2524.curr, _temp2524.curr);})); goto _LL2517;
_LL2521: goto _LL2517; _LL2517:;}}{ struct Cyc_List_List* new_free_vars= 0;
new_free_vars= Cyc_Tcutil_check_valid_type( loc, te, new_free_vars,( void*) Cyc_Absyn_MemKind,
_temp2342); for( 0; _temp2340 != 0; _temp2340=(( struct Cyc_List_List*)
_check_null( _temp2340))->tl){ new_free_vars= Cyc_Tcutil_check_valid_type( loc,
te, new_free_vars,( void*) Cyc_Absyn_MemKind,(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp2340))->hd)).f3);} if(* _temp2346 != 0){ new_free_vars= Cyc_Tcutil_check_valid_type(
loc, te, new_free_vars,( void*) Cyc_Absyn_EffKind,( void*)(( struct Cyc_Core_Opt*)
_check_null(* _temp2346))->v);} else{ struct Cyc_Absyn_Tvar* e= Cyc_Tcutil_new_tvar((
void*) Cyc_Absyn_EffKind); new_free_vars=({ struct Cyc_List_List* _temp2525=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2525->hd=(
void*) e; _temp2525->tl= new_free_vars; _temp2525;});{ struct Cyc_List_List*
effect= 0;{ struct Cyc_List_List* tvs= new_free_vars; for( 0; tvs != 0; tvs=((
struct Cyc_List_List*) _check_null( tvs))->tl){ void* _temp2526=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind))->v;
void* _temp2534; void* _temp2536; _LL2528: if(( unsigned int) _temp2526 > 1u?*((
int*) _temp2526) == Cyc_Absyn_Eq_constr: 0){ _LL2535: _temp2534=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp2526)->f1; if( _temp2534 ==( void*) Cyc_Absyn_RgnKind){
goto _LL2529;} else{ goto _LL2530;}} else{ goto _LL2530;} _LL2530: if((
unsigned int) _temp2526 > 1u?*(( int*) _temp2526) == Cyc_Absyn_Eq_constr: 0){
_LL2537: _temp2536=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2526)->f1;
if( _temp2536 ==( void*) Cyc_Absyn_EffKind){ goto _LL2531;} else{ goto _LL2532;}}
else{ goto _LL2532;} _LL2532: goto _LL2533; _LL2529: effect=({ struct Cyc_List_List*
_temp2538=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2538->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2539=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2539[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2540; _temp2540.tag= Cyc_Absyn_AccessEff;
_temp2540.f1=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp2541=(
struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2541[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2542; _temp2542.tag= Cyc_Absyn_VarType;
_temp2542.f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd;
_temp2542;}); _temp2541;})); _temp2540;}); _temp2539;})); _temp2538->tl= effect;
_temp2538;}); goto _LL2527; _LL2531: effect=({ struct Cyc_List_List* _temp2543=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2543->hd=(
void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp2544=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp2544[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp2545; _temp2545.tag= Cyc_Absyn_VarType; _temp2545.f1=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd;
_temp2545;}); _temp2544;})); _temp2543->tl= effect; _temp2543;}); goto _LL2527;
_LL2533: goto _LL2527; _LL2527:;}}* _temp2346=({ struct Cyc_Core_Opt* _temp2546=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2546->v=(
void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2547=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp2547[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp2548; _temp2548.tag= Cyc_Absyn_JoinEff; _temp2548.f1=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( effect);
_temp2548;}); _temp2547;})); _temp2546;});}} if(* _temp2349 != 0){ struct Cyc_List_List*
new_btvs= 0;{ struct Cyc_List_List* bs=* _temp2349; for( 0; bs != 0; bs=((
struct Cyc_List_List*) _check_null( bs))->tl){ new_free_vars= Cyc_Tcutil_add_free_tvar(
loc, new_free_vars,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( bs))->hd);{ struct Cyc_Absyn_Conref* _temp2549=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( bs))->hd)->kind); goto _LL2550; _LL2550: {
void* _temp2551=( void*) _temp2549->v; void* _temp2559; _LL2553: if( _temp2551
==( void*) Cyc_Absyn_No_constr){ goto _LL2554;} else{ goto _LL2555;} _LL2555:
if(( unsigned int) _temp2551 > 1u?*(( int*) _temp2551) == Cyc_Absyn_Eq_constr: 0){
_LL2560: _temp2559=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2551)->f1;
if( _temp2559 ==( void*) Cyc_Absyn_MemKind){ goto _LL2556;} else{ goto _LL2557;}}
else{ goto _LL2557;} _LL2557: goto _LL2558; _LL2554: Cyc_Tcutil_warn( loc,({
struct _tagged_string _temp2561=*(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( bs))->hd)->name; xprintf("Type variable %.*s unconstrained, assuming boxed",
_temp2561.last_plus_one - _temp2561.curr, _temp2561.curr);})); goto _LL2556;
_LL2556:( void*)( _temp2549->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp2562=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2562[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp2563; _temp2563.tag= Cyc_Absyn_Eq_constr;
_temp2563.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp2563;}); _temp2562;})));
goto _LL2552; _LL2558: goto _LL2552; _LL2552:;}}}} new_free_vars= Cyc_Tcutil_remove_bound_tvars(
new_free_vars,* _temp2349);} for( 0; new_free_vars != 0; new_free_vars=(( struct
Cyc_List_List*) _check_null( new_free_vars))->tl){ free_vars= Cyc_Tcutil_add_free_tvar(
loc, free_vars,( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
new_free_vars))->hd);} goto _LL2238;}} _LL2264: for( 0; _temp2350 != 0;
_temp2350=(( struct Cyc_List_List*) _check_null( _temp2350))->tl){ free_vars=
Cyc_Tcutil_check_valid_type( loc, te, free_vars,( void*) Cyc_Absyn_MemKind,(*((
struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp2350))->hd)).f2);}
goto _LL2238; _LL2266:{ struct _RegionHandle _temp2564= _new_region(); struct
_RegionHandle* sprev_rgn=& _temp2564; _push_region( sprev_rgn);{ struct Cyc_List_List*
prev_fields= 0; for( 0; _temp2352 != 0; _temp2352=(( struct Cyc_List_List*)
_check_null( _temp2352))->tl){ struct Cyc_Absyn_Structfield _temp2567; struct
Cyc_List_List* _temp2568; struct Cyc_Core_Opt* _temp2570; void* _temp2572;
struct Cyc_Absyn_Tqual _temp2574; struct _tagged_string* _temp2576; struct Cyc_Absyn_Structfield*
_temp2565=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp2352))->hd; _temp2567=* _temp2565; _LL2577: _temp2576= _temp2567.name; goto
_LL2575; _LL2575: _temp2574= _temp2567.tq; goto _LL2573; _LL2573: _temp2572=(
void*) _temp2567.type; goto _LL2571; _LL2571: _temp2570= _temp2567.width; goto
_LL2569; _LL2569: _temp2568= _temp2567.attributes; goto _LL2566; _LL2566: if(((
int(*)( int(* compare)( struct _tagged_string*, struct _tagged_string*), struct
Cyc_List_List* l, struct _tagged_string* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp,
prev_fields, _temp2576)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2578=* _temp2576; xprintf("duplicate field %.*s in struct", _temp2578.last_plus_one
- _temp2578.curr, _temp2578.curr);}));} if( Cyc_String_strcmp(* _temp2576,({
char* _temp2579=""; struct _tagged_string _temp2580; _temp2580.curr= _temp2579;
_temp2580.base= _temp2579; _temp2580.last_plus_one= _temp2579 + 1; _temp2580;}))
!= 0){ prev_fields=({ struct Cyc_List_List* _temp2581=( struct Cyc_List_List*)
_region_malloc( sprev_rgn, sizeof( struct Cyc_List_List)); _temp2581->hd=( void*)
_temp2576; _temp2581->tl= prev_fields; _temp2581;});} Cyc_Tcutil_check_valid_type(
loc, te, free_vars,( void*) Cyc_Absyn_MemKind, _temp2572); if( _temp2570 != 0){
unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( _temp2570))->v)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2582=* _temp2576; xprintf("bitfield %.*s does not have constant width",
_temp2582.last_plus_one - _temp2582.curr, _temp2582.curr);}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp2570))->v);}{
void* _temp2583= Cyc_Tcutil_compress( _temp2572); void* _temp2589; void*
_temp2591; _LL2585: if(( unsigned int) _temp2583 > 4u?*(( int*) _temp2583) ==
Cyc_Absyn_IntType: 0){ _LL2592: _temp2591=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2583)->f1; goto _LL2590; _LL2590: _temp2589=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2583)->f2; goto _LL2586;} else{ goto _LL2587;} _LL2587: goto _LL2588;
_LL2586:{ void* _temp2593= _temp2589; _LL2595: if( _temp2593 ==( void*) Cyc_Absyn_B1){
goto _LL2596;} else{ goto _LL2597;} _LL2597: if( _temp2593 ==( void*) Cyc_Absyn_B2){
goto _LL2598;} else{ goto _LL2599;} _LL2599: if( _temp2593 ==( void*) Cyc_Absyn_B4){
goto _LL2600;} else{ goto _LL2601;} _LL2601: if( _temp2593 ==( void*) Cyc_Absyn_B8){
goto _LL2602;} else{ goto _LL2594;} _LL2596: if( w > 8){ Cyc_Tcutil_terr( loc,({
char* _temp2603="bitfield larger than type"; struct _tagged_string _temp2604;
_temp2604.curr= _temp2603; _temp2604.base= _temp2603; _temp2604.last_plus_one=
_temp2603 + 26; _temp2604;}));} goto _LL2594; _LL2598: if( w > 16){ Cyc_Tcutil_terr(
loc,({ char* _temp2605="bitfield larger than type"; struct _tagged_string
_temp2606; _temp2606.curr= _temp2605; _temp2606.base= _temp2605; _temp2606.last_plus_one=
_temp2605 + 26; _temp2606;}));} goto _LL2594; _LL2600: if( w > 32){ Cyc_Tcutil_terr(
loc,({ char* _temp2607="bitfield larger than type"; struct _tagged_string
_temp2608; _temp2608.curr= _temp2607; _temp2608.base= _temp2607; _temp2608.last_plus_one=
_temp2607 + 26; _temp2608;}));} goto _LL2594; _LL2602: if( w > 64){ Cyc_Tcutil_terr(
loc,({ char* _temp2609="bitfield larger than type"; struct _tagged_string
_temp2610; _temp2610.curr= _temp2609; _temp2610.base= _temp2609; _temp2610.last_plus_one=
_temp2609 + 26; _temp2610;}));} goto _LL2594; _LL2594:;} goto _LL2584; _LL2588:
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2611=* _temp2576; struct
_tagged_string _temp2612= Cyc_Absynpp_typ2string( _temp2572); xprintf("bitfield %.*s must have integral type but has type %.*s",
_temp2611.last_plus_one - _temp2611.curr, _temp2611.curr, _temp2612.last_plus_one
- _temp2612.curr, _temp2612.curr);})); goto _LL2584; _LL2584:;}} for( 0;
_temp2568 != 0; _temp2568=(( struct Cyc_List_List*) _check_null( _temp2568))->tl){
void* _temp2613=( void*)(( struct Cyc_List_List*) _check_null( _temp2568))->hd;
int _temp2621; _LL2615: if( _temp2613 ==( void*) Cyc_Absyn_Packed_att){ goto
_LL2616;} else{ goto _LL2617;} _LL2617: if(( unsigned int) _temp2613 > 16u?*((
int*) _temp2613) == Cyc_Absyn_Aligned_att: 0){ _LL2622: _temp2621=(( struct Cyc_Absyn_Aligned_att_struct*)
_temp2613)->f1; goto _LL2618;} else{ goto _LL2619;} _LL2619: goto _LL2620;
_LL2616: continue; _LL2618: continue; _LL2620: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2623= Cyc_Absyn_attribute2string(( void*)(( struct Cyc_List_List*)
_check_null( _temp2568))->hd); struct _tagged_string _temp2624=* _temp2576;
xprintf("bad attribute %.*s on struct member %.*s", _temp2623.last_plus_one -
_temp2623.curr, _temp2623.curr, _temp2624.last_plus_one - _temp2624.curr,
_temp2624.curr);})); goto _LL2614; _LL2614:;}}}; _pop_region(& _temp2564);} goto
_LL2238; _LL2268:{ struct _RegionHandle _temp2625= _new_region(); struct
_RegionHandle* uprev_rgn=& _temp2625; _push_region( uprev_rgn);{ struct Cyc_List_List*
prev_fields= 0; for( 0; _temp2354 != 0; _temp2354=(( struct Cyc_List_List*)
_check_null( _temp2354))->tl){ struct Cyc_Absyn_Structfield _temp2628; struct
Cyc_List_List* _temp2629; struct Cyc_Core_Opt* _temp2631; void* _temp2633;
struct Cyc_Absyn_Tqual _temp2635; struct _tagged_string* _temp2637; struct Cyc_Absyn_Structfield*
_temp2626=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp2354))->hd; _temp2628=* _temp2626; _LL2638: _temp2637= _temp2628.name; goto
_LL2636; _LL2636: _temp2635= _temp2628.tq; goto _LL2634; _LL2634: _temp2633=(
void*) _temp2628.type; goto _LL2632; _LL2632: _temp2631= _temp2628.width; goto
_LL2630; _LL2630: _temp2629= _temp2628.attributes; goto _LL2627; _LL2627: if(((
int(*)( int(* compare)( struct _tagged_string*, struct _tagged_string*), struct
Cyc_List_List* l, struct _tagged_string* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp,
prev_fields, _temp2637)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2639=* _temp2637; xprintf("duplicate field %.*s in struct", _temp2639.last_plus_one
- _temp2639.curr, _temp2639.curr);}));} if( Cyc_String_strcmp(* _temp2637,({
char* _temp2640=""; struct _tagged_string _temp2641; _temp2641.curr= _temp2640;
_temp2641.base= _temp2640; _temp2641.last_plus_one= _temp2640 + 1; _temp2641;}))
!= 0){ prev_fields=({ struct Cyc_List_List* _temp2642=( struct Cyc_List_List*)
_region_malloc( uprev_rgn, sizeof( struct Cyc_List_List)); _temp2642->hd=( void*)
_temp2637; _temp2642->tl= prev_fields; _temp2642;});} Cyc_Tcutil_check_valid_type(
loc, te, free_vars,( void*) Cyc_Absyn_MemKind, _temp2633); if( ! Cyc_Tcutil_bits_only(
_temp2633)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2643=* _temp2637;
xprintf("%.*s has a type that is possibly incompatible with other members of union",
_temp2643.last_plus_one - _temp2643.curr, _temp2643.curr);}));} if( _temp2631 !=
0){ unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( _temp2631))->v)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2644=* _temp2637; xprintf("bitfield %.*s does not have constant width",
_temp2644.last_plus_one - _temp2644.curr, _temp2644.curr);}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp2631))->v);}{
void* _temp2645= Cyc_Tcutil_compress( _temp2633); void* _temp2651; void*
_temp2653; _LL2647: if(( unsigned int) _temp2645 > 4u?*(( int*) _temp2645) ==
Cyc_Absyn_IntType: 0){ _LL2654: _temp2653=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2645)->f1; goto _LL2652; _LL2652: _temp2651=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2645)->f2; goto _LL2648;} else{ goto _LL2649;} _LL2649: goto _LL2650;
_LL2648:{ void* _temp2655= _temp2651; _LL2657: if( _temp2655 ==( void*) Cyc_Absyn_B1){
goto _LL2658;} else{ goto _LL2659;} _LL2659: if( _temp2655 ==( void*) Cyc_Absyn_B2){
goto _LL2660;} else{ goto _LL2661;} _LL2661: if( _temp2655 ==( void*) Cyc_Absyn_B4){
goto _LL2662;} else{ goto _LL2663;} _LL2663: if( _temp2655 ==( void*) Cyc_Absyn_B8){
goto _LL2664;} else{ goto _LL2656;} _LL2658: if( w > 8){ Cyc_Tcutil_terr( loc,({
char* _temp2665="bitfield larger than type"; struct _tagged_string _temp2666;
_temp2666.curr= _temp2665; _temp2666.base= _temp2665; _temp2666.last_plus_one=
_temp2665 + 26; _temp2666;}));} goto _LL2656; _LL2660: if( w > 16){ Cyc_Tcutil_terr(
loc,({ char* _temp2667="bitfield larger than type"; struct _tagged_string
_temp2668; _temp2668.curr= _temp2667; _temp2668.base= _temp2667; _temp2668.last_plus_one=
_temp2667 + 26; _temp2668;}));} goto _LL2656; _LL2662: if( w > 32){ Cyc_Tcutil_terr(
loc,({ char* _temp2669="bitfield larger than type"; struct _tagged_string
_temp2670; _temp2670.curr= _temp2669; _temp2670.base= _temp2669; _temp2670.last_plus_one=
_temp2669 + 26; _temp2670;}));} goto _LL2656; _LL2664: if( w > 64){ Cyc_Tcutil_terr(
loc,({ char* _temp2671="bitfield larger than type"; struct _tagged_string
_temp2672; _temp2672.curr= _temp2671; _temp2672.base= _temp2671; _temp2672.last_plus_one=
_temp2671 + 26; _temp2672;}));} goto _LL2656; _LL2656:;} goto _LL2646; _LL2650:
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2673=* _temp2637; struct
_tagged_string _temp2674= Cyc_Absynpp_typ2string( _temp2633); xprintf("bitfield %.*s must have integral type but has type %.*s",
_temp2673.last_plus_one - _temp2673.curr, _temp2673.curr, _temp2674.last_plus_one
- _temp2674.curr, _temp2674.curr);})); goto _LL2646; _LL2646:;}} for( 0;
_temp2629 != 0; _temp2629=(( struct Cyc_List_List*) _check_null( _temp2629))->tl){
void* _temp2675=( void*)(( struct Cyc_List_List*) _check_null( _temp2629))->hd;
int _temp2683; _LL2677: if( _temp2675 ==( void*) Cyc_Absyn_Packed_att){ goto
_LL2678;} else{ goto _LL2679;} _LL2679: if(( unsigned int) _temp2675 > 16u?*((
int*) _temp2675) == Cyc_Absyn_Aligned_att: 0){ _LL2684: _temp2683=(( struct Cyc_Absyn_Aligned_att_struct*)
_temp2675)->f1; goto _LL2680;} else{ goto _LL2681;} _LL2681: goto _LL2682;
_LL2678: continue; _LL2680: continue; _LL2682: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2685= Cyc_Absyn_attribute2string(( void*)(( struct Cyc_List_List*)
_check_null( _temp2629))->hd); struct _tagged_string _temp2686=* _temp2637;
xprintf("bad attribute %.*s on struct member %.*s", _temp2685.last_plus_one -
_temp2685.curr, _temp2685.curr, _temp2686.last_plus_one - _temp2686.curr,
_temp2686.curr);})); goto _LL2676; _LL2676:;}}}; _pop_region(& _temp2625);} goto
_LL2238; _LL2270: if( _temp2361 == 0){ if(* _temp2358 == 0){ Cyc_Tcutil_terr(
loc, xprintf("bad anonymous struct")); return free_vars;}{ struct Cyc_Absyn_Structdecl*
sd=*(( struct Cyc_Absyn_Structdecl**) _check_null(* _temp2358));;}} if(*
_temp2358 == 0){ struct _handler_cons _temp2687; _push_handler(& _temp2687);{
int _temp2689= 0; if( setjmp( _temp2687.handler)){ _temp2689= 1;} if( !
_temp2689){* _temp2358=( struct Cyc_Absyn_Structdecl**) Cyc_Tcenv_lookup_structdecl(
te, loc,( struct _tuple1*) _check_null( _temp2361));; _pop_handler();} else{
void* _temp2688=( void*) _exn_thrown; void* _temp2691= _temp2688; _LL2693: if(
_temp2691 == Cyc_Dict_Absent){ goto _LL2694;} else{ goto _LL2695;} _LL2695: goto
_LL2696; _LL2694: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2697= Cyc_Absynpp_qvar2string((
struct _tuple1*) _check_null( _temp2361)); xprintf("unbound type struct %.*s",
_temp2697.last_plus_one - _temp2697.curr, _temp2697.curr);})); return free_vars;
_LL2696:( void) _throw( _temp2691); _LL2692:;}}}{ struct Cyc_Absyn_Structdecl*
sd=*(( struct Cyc_Absyn_Structdecl**) _check_null(* _temp2358)); if( sd->name !=
0){*(( struct _tuple1*) _check_null( _temp2361))=(( struct _tuple1*)(( struct
Cyc_Core_Opt*) _check_null( sd->name))->v)[ _check_known_subscript_notnull( 1u,
0)];}{ struct Cyc_List_List* tvs= sd->tvs; int lvs=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( tvs); int largs=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp2359); if( lvs != largs){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2698= Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp2361));
int _temp2699= lvs; int _temp2700= largs; xprintf("struct %.*s expects %d type arguments but was given %d",
_temp2698.last_plus_one - _temp2698.curr, _temp2698.curr, _temp2699, _temp2700);}));}
for( 0; _temp2359 != 0;( _temp2359=(( struct Cyc_List_List*) _check_null(
_temp2359))->tl, tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void*
t1=( void*)(( struct Cyc_List_List*) _check_null( _temp2359))->hd; void* k1=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind); free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars, k1, t1);} goto _LL2238;}} _LL2272: if( _temp2368 == 0){ Cyc_Tcutil_terr(
loc,({ char* _temp2701="missing union name"; struct _tagged_string _temp2702;
_temp2702.curr= _temp2701; _temp2702.base= _temp2701; _temp2702.last_plus_one=
_temp2701 + 19; _temp2702;})); return free_vars;} if(* _temp2365 == 0){ struct
_handler_cons _temp2703; _push_handler(& _temp2703);{ int _temp2705= 0; if(
setjmp( _temp2703.handler)){ _temp2705= 1;} if( ! _temp2705){* _temp2365=(
struct Cyc_Absyn_Uniondecl**) Cyc_Tcenv_lookup_uniondecl( te, loc,( struct
_tuple1*) _check_null( _temp2368));; _pop_handler();} else{ void* _temp2704=(
void*) _exn_thrown; void* _temp2707= _temp2704; _LL2709: if( _temp2707 == Cyc_Dict_Absent){
goto _LL2710;} else{ goto _LL2711;} _LL2711: goto _LL2712; _LL2710: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2713= Cyc_Absynpp_qvar2string(( struct _tuple1*)
_check_null( _temp2368)); xprintf("unbound type union %.*s", _temp2713.last_plus_one
- _temp2713.curr, _temp2713.curr);})); return free_vars; _LL2712:( void) _throw(
_temp2707); _LL2708:;}}}{ struct Cyc_Absyn_Uniondecl* ud=*(( struct Cyc_Absyn_Uniondecl**)
_check_null(* _temp2365)); if( ud->name != 0){*(( struct _tuple1*) _check_null(
_temp2368))=(( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( ud->name))->v)[
_check_known_subscript_notnull( 1u, 0)];}{ struct Cyc_List_List* tvs= ud->tvs;
int lvs=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs); int largs=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp2366); if( lvs != largs){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2714= Cyc_Absynpp_qvar2string((
struct _tuple1*) _check_null( _temp2368)); int _temp2715= lvs; int _temp2716=
largs; xprintf("union %.*s expects %d type arguments but was given %d",
_temp2714.last_plus_one - _temp2714.curr, _temp2714.curr, _temp2715, _temp2716);}));}
for( 0; _temp2366 != 0;( _temp2366=(( struct Cyc_List_List*) _check_null(
_temp2366))->tl, tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl)){ void*
t1=( void*)(( struct Cyc_List_List*) _check_null( _temp2366))->hd; void* k1=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd)->kind); free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars, k1, t1);} goto _LL2238;}} _LL2274: { struct Cyc_Absyn_Typedefdecl*
td;{ struct _handler_cons _temp2717; _push_handler(& _temp2717);{ int _temp2719=
0; if( setjmp( _temp2717.handler)){ _temp2719= 1;} if( ! _temp2719){ td= Cyc_Tcenv_lookup_typedefdecl(
te, loc, _temp2375);; _pop_handler();} else{ void* _temp2718=( void*)
_exn_thrown; void* _temp2721= _temp2718; _LL2723: if( _temp2721 == Cyc_Dict_Absent){
goto _LL2724;} else{ goto _LL2725;} _LL2725: goto _LL2726; _LL2724: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2727= Cyc_Absynpp_qvar2string( _temp2375);
xprintf("unbound typedef name %.*s", _temp2727.last_plus_one - _temp2727.curr,
_temp2727.curr);})); return free_vars; _LL2726:( void) _throw( _temp2721);
_LL2722:;}}} _temp2375[ _check_known_subscript_notnull( 1u, 0)]=( td->name)[
_check_known_subscript_notnull( 1u, 0)];{ struct Cyc_List_List* tvs= td->tvs;
struct Cyc_List_List* ts= _temp2373; struct Cyc_List_List* inst= 0; for( 0; ts
!= 0? tvs != 0: 0;( ts=(( struct Cyc_List_List*) _check_null( ts))->tl, tvs=((
struct Cyc_List_List*) _check_null( tvs))->tl)){ void* k=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs))->hd)->kind); free_vars= Cyc_Tcutil_check_valid_type( loc, te,
free_vars, k,( void*)(( struct Cyc_List_List*) _check_null( ts))->hd); inst=({
struct Cyc_List_List* _temp2728=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2728->hd=( void*)({ struct _tuple5* _temp2729=(
struct _tuple5*) GC_malloc( sizeof( struct _tuple5)); _temp2729->f1=( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs))->hd; _temp2729->f2=( void*)(( struct
Cyc_List_List*) _check_null( ts))->hd; _temp2729;}); _temp2728->tl= inst;
_temp2728;});} if( tvs != 0? 1: ts != 0){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2730= Cyc_Absynpp_qvar2string( _temp2375); xprintf("typedef %.*s expects a different number of arguments",
_temp2730.last_plus_one - _temp2730.curr, _temp2730.curr);})); return free_vars;}{
void* new_typ= Cyc_Tcutil_substitute( inst,( void*) td->defn);* _temp2372=({
struct Cyc_Core_Opt* _temp2731=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2731->v=( void*) new_typ; _temp2731;}); goto _LL2238;}}}
_LL2276: goto _LL2238; _LL2278: _temp2379= _temp2377; goto _LL2280; _LL2280:
free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars,( void*) Cyc_Absyn_RgnKind,
_temp2379); goto _LL2238; _LL2282: for( 0; _temp2381 != 0; _temp2381=(( struct
Cyc_List_List*) _check_null( _temp2381))->tl){ free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars,( void*) Cyc_Absyn_EffKind,( void*)(( struct Cyc_List_List*)
_check_null( _temp2381))->hd);} goto _LL2238; _LL2238:;} if( ! Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind( t), expected_kind)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2732= Cyc_Absynpp_typ2string( t); struct _tagged_string
_temp2733= Cyc_Absynpp_kind2string( Cyc_Tcutil_typ_kind( t)); struct
_tagged_string _temp2734= Cyc_Absynpp_kind2string( expected_kind); xprintf("type %.*s has kind %.*s but as used here needs kind %.*s",
_temp2732.last_plus_one - _temp2732.curr, _temp2732.curr, _temp2733.last_plus_one
- _temp2733.curr, _temp2733.curr, _temp2734.last_plus_one - _temp2734.curr,
_temp2734.curr);}));} return free_vars;} void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, void* t){ struct
Cyc_List_List* free_tvars= Cyc_Tcutil_check_valid_type( loc, te, 0,( void*) Cyc_Absyn_MemKind,
t);{ struct Cyc_List_List* x= free_tvars; for( 0; x != 0; x=(( struct Cyc_List_List*)
_check_null( x))->tl){ struct Cyc_Absyn_Conref* c=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( x))->hd)->kind); void* _temp2735=( void*) c->v;
void* _temp2743; _LL2737: if( _temp2735 ==( void*) Cyc_Absyn_No_constr){ goto
_LL2738;} else{ goto _LL2739;} _LL2739: if(( unsigned int) _temp2735 > 1u?*((
int*) _temp2735) == Cyc_Absyn_Eq_constr: 0){ _LL2744: _temp2743=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp2735)->f1; if( _temp2743 ==( void*) Cyc_Absyn_MemKind){
goto _LL2740;} else{ goto _LL2741;}} else{ goto _LL2741;} _LL2741: goto _LL2742;
_LL2738: goto _LL2740; _LL2740:( void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp2745=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2745[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp2746; _temp2746.tag= Cyc_Absyn_Eq_constr;
_temp2746.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp2746;}); _temp2745;})));
goto _LL2736; _LL2742: goto _LL2736; _LL2736:;}} if( free_tvars != 0){ void*
_temp2747= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo _temp2753; struct
Cyc_List_List* _temp2755; int _temp2757; struct Cyc_List_List* _temp2759; void*
_temp2761; struct Cyc_Core_Opt* _temp2763; struct Cyc_List_List* _temp2765;
struct Cyc_List_List** _temp2767; _LL2749: if(( unsigned int) _temp2747 > 4u?*((
int*) _temp2747) == Cyc_Absyn_FnType: 0){ _LL2754: _temp2753=(( struct Cyc_Absyn_FnType_struct*)
_temp2747)->f1; _LL2766: _temp2765= _temp2753.tvars; _temp2767=&((( struct Cyc_Absyn_FnType_struct*)
_temp2747)->f1).tvars; goto _LL2764; _LL2764: _temp2763= _temp2753.effect; goto
_LL2762; _LL2762: _temp2761=( void*) _temp2753.ret_typ; goto _LL2760; _LL2760:
_temp2759= _temp2753.args; goto _LL2758; _LL2758: _temp2757= _temp2753.varargs;
goto _LL2756; _LL2756: _temp2755= _temp2753.attributes; goto _LL2750;} else{
goto _LL2751;} _LL2751: goto _LL2752; _LL2750: if(* _temp2767 == 0){* _temp2767=
free_tvars;} else{ if( free_tvars != 0){ struct _tagged_string s=*(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( free_tvars))->hd)->name; if(*(( char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( s,
sizeof( char), 0) =='?'){ Cyc_Tcutil_terr( loc, xprintf("You failed to abstract over the generated type variables. \nRemove the bound type variables or else make the effects of the type explicit\nand then abstract the free type variables properly."));}
else{ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2768= s; xprintf("unbound type variable %.*s",
_temp2768.last_plus_one - _temp2768.curr, _temp2768.curr);}));}}} goto _LL2748;
_LL2752: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2769=*(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( free_tvars))->hd)->name; xprintf("unbound type variable %.*s",
_temp2769.last_plus_one - _temp2769.curr, _temp2769.curr);})); return; _LL2748:;}}
void Cyc_Tcutil_check_type( struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* bound_tvars, void* expected_kind, void* t){ struct Cyc_List_List*
free_tvars= Cyc_Tcutil_check_valid_type( loc, te, 0, expected_kind, t); struct
Cyc_List_List* fs= free_tvars; for( 0; fs != 0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ struct Cyc_Absyn_Tvar* ftv=( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( fs))->hd; struct _tagged_string* _temp2770=
ftv->name; goto _LL2771; _LL2771: { struct Cyc_Absyn_Conref* _temp2772= ftv->kind;
goto _LL2773; _LL2773: { int present= 0;{ struct Cyc_List_List* bs= bound_tvars;
for( 0; bs != 0; bs=(( struct Cyc_List_List*) _check_null( bs))->tl){ struct Cyc_Absyn_Tvar*
btv=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( bs))->hd;
struct _tagged_string* _temp2774= btv->name; goto _LL2775; _LL2775: { struct Cyc_Absyn_Conref*
_temp2776= btv->kind; goto _LL2777; _LL2777: if( Cyc_String_zstrptrcmp(
_temp2770, _temp2774) == 0){ present= 1; if( ! Cyc_Tcutil_constrain_kinds(
_temp2776, _temp2772)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2778=*
_temp2770; struct _tagged_string _temp2779= Cyc_Absynpp_ckind2string( _temp2776);
struct _tagged_string _temp2780= Cyc_Absynpp_ckind2string( _temp2772); xprintf("type variable %.*s has kind %.*s but is used here atkind %.*s",
_temp2778.last_plus_one - _temp2778.curr, _temp2778.curr, _temp2779.last_plus_one
- _temp2779.curr, _temp2779.curr, _temp2780.last_plus_one - _temp2780.curr,
_temp2780.curr);}));} break;}}}} if( ! present){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2781=* _temp2770; struct _tagged_string _temp2782= Cyc_Absynpp_typ2string(
t); xprintf("unbound type variable %.*s in type %.*s", _temp2781.last_plus_one -
_temp2781.curr, _temp2781.curr, _temp2782.last_plus_one - _temp2782.curr,
_temp2782.curr);}));}}}}} static void Cyc_Tcutil_check_unique_unsorted( int(*
cmp)( void*, void*), struct Cyc_List_List* vs, struct Cyc_Position_Segment* loc,
struct _tagged_string(* a2string)( void*), struct _tagged_string msg){ for( 0;
vs != 0; vs=(( struct Cyc_List_List*) _check_null( vs))->tl){ struct Cyc_List_List*
vs2=(( struct Cyc_List_List*) _check_null( vs))->tl; for( 0; vs2 != 0; vs2=((
struct Cyc_List_List*) _check_null( vs2))->tl){ if( cmp(( void*)(( struct Cyc_List_List*)
_check_null( vs))->hd,( void*)(( struct Cyc_List_List*) _check_null( vs2))->hd)
== 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2783= msg; struct
_tagged_string _temp2784= a2string(( void*)(( struct Cyc_List_List*) _check_null(
vs))->hd); xprintf("%.*s: %.*s", _temp2783.last_plus_one - _temp2783.curr,
_temp2783.curr, _temp2784.last_plus_one - _temp2784.curr, _temp2784.curr);}));}}}}
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
tvs, loc, Cyc_Tcutil_tvar2string,({ char* _temp2785="duplicate type variable";
struct _tagged_string _temp2786; _temp2786.curr= _temp2785; _temp2786.base=
_temp2785; _temp2786.last_plus_one= _temp2785 + 24; _temp2786;}));} struct
_tuple15{ struct Cyc_Absyn_Structfield* f1; int f2; } ; struct _tuple16{ struct
Cyc_List_List* f1; void* f2; } ; struct _tuple17{ struct Cyc_Absyn_Structfield*
f1; void* f2; } ; struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* sdfields){ struct Cyc_List_List* fields= 0;{ struct
Cyc_List_List* sd_fields= sdfields; for( 0; sd_fields != 0; sd_fields=(( struct
Cyc_List_List*) _check_null( sd_fields))->tl){ if( Cyc_String_strcmp(*(( struct
Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( sd_fields))->hd)->name,({
char* _temp2787=""; struct _tagged_string _temp2788; _temp2788.curr= _temp2787;
_temp2788.base= _temp2787; _temp2788.last_plus_one= _temp2787 + 1; _temp2788;}))
!= 0){ fields=({ struct Cyc_List_List* _temp2789=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2789->hd=( void*)({ struct
_tuple15* _temp2790=( struct _tuple15*) GC_malloc( sizeof( struct _tuple15));
_temp2790->f1=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( sd_fields))->hd; _temp2790->f2= 0; _temp2790;}); _temp2789->tl=
fields; _temp2789;});}}} fields=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( fields);{ struct Cyc_List_List* ans= 0; for( 0; des != 0;
des=(( struct Cyc_List_List*) _check_null( des))->tl){ struct _tuple16 _temp2793;
void* _temp2794; struct Cyc_List_List* _temp2796; struct _tuple16* _temp2791=(
struct _tuple16*)(( struct Cyc_List_List*) _check_null( des))->hd; _temp2793=*
_temp2791; _LL2797: _temp2796= _temp2793.f1; goto _LL2795; _LL2795: _temp2794=
_temp2793.f2; goto _LL2792; _LL2792: if( _temp2796 == 0){ struct Cyc_List_List*
_temp2798= fields; goto _LL2799; _LL2799: for( 0; _temp2798 != 0; _temp2798=((
struct Cyc_List_List*) _check_null( _temp2798))->tl){ if( !(*(( struct _tuple15*)((
struct Cyc_List_List*) _check_null( _temp2798))->hd)).f2){(*(( struct _tuple15*)((
struct Cyc_List_List*) _check_null( _temp2798))->hd)).f2= 1;(*(( struct _tuple16*)((
struct Cyc_List_List*) _check_null( des))->hd)).f1=({ struct Cyc_List_List*
_temp2800=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2800->hd=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp2801=(
struct Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp2801[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp2802; _temp2802.tag= Cyc_Absyn_FieldName;
_temp2802.f1=((*(( struct _tuple15*)(( struct Cyc_List_List*) _check_null(
_temp2798))->hd)).f1)->name; _temp2802;}); _temp2801;})); _temp2800->tl= 0;
_temp2800;}); ans=({ struct Cyc_List_List* _temp2803=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2803->hd=( void*)({
struct _tuple17* _temp2804=( struct _tuple17*) _region_malloc( rgn, sizeof(
struct _tuple17)); _temp2804->f1=(*(( struct _tuple15*)(( struct Cyc_List_List*)
_check_null( _temp2798))->hd)).f1; _temp2804->f2= _temp2794; _temp2804;});
_temp2803->tl= ans; _temp2803;}); break;}} if( _temp2798 == 0){ Cyc_Tcutil_terr(
loc,({ char* _temp2805="too many arguments to struct"; struct _tagged_string
_temp2806; _temp2806.curr= _temp2805; _temp2806.base= _temp2805; _temp2806.last_plus_one=
_temp2805 + 29; _temp2806;}));}} else{ if((( struct Cyc_List_List*) _check_null(
_temp2796))->tl != 0){ Cyc_Tcutil_terr( loc,({ char* _temp2807="multiple designators are not supported";
struct _tagged_string _temp2808; _temp2808.curr= _temp2807; _temp2808.base=
_temp2807; _temp2808.last_plus_one= _temp2807 + 39; _temp2808;}));} else{ void*
_temp2809=( void*)(( struct Cyc_List_List*) _check_null( _temp2796))->hd; struct
Cyc_Absyn_Exp* _temp2815; struct _tagged_string* _temp2817; _LL2811: if(*(( int*)
_temp2809) == Cyc_Absyn_ArrayElement){ _LL2816: _temp2815=(( struct Cyc_Absyn_ArrayElement_struct*)
_temp2809)->f1; goto _LL2812;} else{ goto _LL2813;} _LL2813: if(*(( int*)
_temp2809) == Cyc_Absyn_FieldName){ _LL2818: _temp2817=(( struct Cyc_Absyn_FieldName_struct*)
_temp2809)->f1; goto _LL2814;} else{ goto _LL2810;} _LL2812: Cyc_Tcutil_terr(
loc,({ char* _temp2819="array designator used in argument to struct"; struct
_tagged_string _temp2820; _temp2820.curr= _temp2819; _temp2820.base= _temp2819;
_temp2820.last_plus_one= _temp2819 + 44; _temp2820;})); goto _LL2810; _LL2814: {
struct Cyc_List_List* _temp2821= fields; goto _LL2822; _LL2822: for( 0;
_temp2821 != 0; _temp2821=(( struct Cyc_List_List*) _check_null( _temp2821))->tl){
if( Cyc_String_zstrptrcmp( _temp2817,((*(( struct _tuple15*)(( struct Cyc_List_List*)
_check_null( _temp2821))->hd)).f1)->name) == 0){ if((*(( struct _tuple15*)((
struct Cyc_List_List*) _check_null( _temp2821))->hd)).f2){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2823=* _temp2817; xprintf("field %.*s has already been used as an argument",
_temp2823.last_plus_one - _temp2823.curr, _temp2823.curr);}));}(*(( struct
_tuple15*)(( struct Cyc_List_List*) _check_null( _temp2821))->hd)).f2= 1; ans=({
struct Cyc_List_List* _temp2824=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp2824->hd=( void*)({ struct _tuple17*
_temp2825=( struct _tuple17*) _region_malloc( rgn, sizeof( struct _tuple17));
_temp2825->f1=(*(( struct _tuple15*)(( struct Cyc_List_List*) _check_null(
_temp2821))->hd)).f1; _temp2825->f2= _temp2794; _temp2825;}); _temp2824->tl= ans;
_temp2824;}); break;}} if( _temp2821 == 0){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2826=* _temp2817; xprintf("bad field designator %.*s",
_temp2826.last_plus_one - _temp2826.curr, _temp2826.curr);}));} goto _LL2810;}
_LL2810:;}}} for( 0; fields != 0; fields=(( struct Cyc_List_List*) _check_null(
fields))->tl){ if( !(*(( struct _tuple15*)(( struct Cyc_List_List*) _check_null(
fields))->hd)).f2){ Cyc_Tcutil_terr( loc,({ char* _temp2827="too few arguments to struct";
struct _tagged_string _temp2828; _temp2828.curr= _temp2827; _temp2828.base=
_temp2827; _temp2828.last_plus_one= _temp2827 + 28; _temp2828;})); break;}}
return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
ans);}} int Cyc_Tcutil_is_tagged_pointer_typ( void* t){ void* _temp2829= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_PtrInfo _temp2835; struct Cyc_Absyn_Conref* _temp2837;
struct Cyc_Absyn_Tqual _temp2839; struct Cyc_Absyn_Conref* _temp2841; void*
_temp2843; void* _temp2845; _LL2831: if(( unsigned int) _temp2829 > 4u?*(( int*)
_temp2829) == Cyc_Absyn_PointerType: 0){ _LL2836: _temp2835=(( struct Cyc_Absyn_PointerType_struct*)
_temp2829)->f1; _LL2846: _temp2845=( void*) _temp2835.elt_typ; goto _LL2844;
_LL2844: _temp2843=( void*) _temp2835.rgn_typ; goto _LL2842; _LL2842: _temp2841=
_temp2835.nullable; goto _LL2840; _LL2840: _temp2839= _temp2835.tq; goto _LL2838;
_LL2838: _temp2837= _temp2835.bounds; goto _LL2832;} else{ goto _LL2833;}
_LL2833: goto _LL2834; _LL2832: { struct Cyc_Absyn_Conref* _temp2847=(( struct
Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp2837); goto _LL2848; _LL2848: { void* _temp2849=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp2847))->v; void*
_temp2857; _LL2851: if(( unsigned int) _temp2849 > 1u?*(( int*) _temp2849) ==
Cyc_Absyn_Eq_constr: 0){ _LL2858: _temp2857=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp2849)->f1; if( _temp2857 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL2852;}
else{ goto _LL2853;}} else{ goto _LL2853;} _LL2853: if( _temp2849 ==( void*) Cyc_Absyn_No_constr){
goto _LL2854;} else{ goto _LL2855;} _LL2855: goto _LL2856; _LL2852: return 1;
_LL2854:( void*)( _temp2847->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp2859=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2859[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp2860; _temp2860.tag= Cyc_Absyn_Eq_constr;
_temp2860.f1=( void*)(( void*) Cyc_Absyn_Unknown_b); _temp2860;}); _temp2859;})));
return 1; _LL2856: return 0; _LL2850:;}} _LL2834: return 0; _LL2830:;} struct
_tuple6 Cyc_Tcutil_addressof_props( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e){ struct _tuple6 bogus_ans=({ struct _tuple6 _temp3099; _temp3099.f1= 0;
_temp3099.f2=( void*) Cyc_Absyn_HeapRgn; _temp3099;}); void* _temp2861=( void*)
e->r; void* _temp2875; struct _tuple1* _temp2877; struct _tagged_string*
_temp2879; struct Cyc_Absyn_Exp* _temp2881; struct _tagged_string* _temp2883;
struct Cyc_Absyn_Exp* _temp2885; struct Cyc_Absyn_Exp* _temp2887; struct Cyc_Absyn_Exp*
_temp2889; struct Cyc_Absyn_Exp* _temp2891; _LL2863: if(*(( int*) _temp2861) ==
Cyc_Absyn_Var_e){ _LL2878: _temp2877=(( struct Cyc_Absyn_Var_e_struct*)
_temp2861)->f1; goto _LL2876; _LL2876: _temp2875=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp2861)->f2; goto _LL2864;} else{ goto _LL2865;} _LL2865: if(*(( int*)
_temp2861) == Cyc_Absyn_StructMember_e){ _LL2882: _temp2881=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp2861)->f1; goto _LL2880; _LL2880: _temp2879=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp2861)->f2; goto _LL2866;} else{ goto _LL2867;} _LL2867: if(*(( int*)
_temp2861) == Cyc_Absyn_StructArrow_e){ _LL2886: _temp2885=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp2861)->f1; goto _LL2884; _LL2884: _temp2883=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp2861)->f2; goto _LL2868;} else{ goto _LL2869;} _LL2869: if(*(( int*)
_temp2861) == Cyc_Absyn_Deref_e){ _LL2888: _temp2887=(( struct Cyc_Absyn_Deref_e_struct*)
_temp2861)->f1; goto _LL2870;} else{ goto _LL2871;} _LL2871: if(*(( int*)
_temp2861) == Cyc_Absyn_Subscript_e){ _LL2892: _temp2891=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp2861)->f1; goto _LL2890; _LL2890: _temp2889=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp2861)->f2; goto _LL2872;} else{ goto _LL2873;} _LL2873: goto _LL2874;
_LL2864: { void* _temp2893= _temp2875; struct Cyc_Absyn_Fndecl* _temp2907;
struct Cyc_Absyn_Vardecl* _temp2909; struct Cyc_Absyn_Vardecl* _temp2911; struct
Cyc_Absyn_Vardecl* _temp2913; struct Cyc_Absyn_Vardecl* _temp2915; _LL2895: if(
_temp2893 ==( void*) Cyc_Absyn_Unresolved_b){ goto _LL2896;} else{ goto _LL2897;}
_LL2897: if(( unsigned int) _temp2893 > 1u?*(( int*) _temp2893) == Cyc_Absyn_Funname_b:
0){ _LL2908: _temp2907=(( struct Cyc_Absyn_Funname_b_struct*) _temp2893)->f1;
goto _LL2898;} else{ goto _LL2899;} _LL2899: if(( unsigned int) _temp2893 > 1u?*((
int*) _temp2893) == Cyc_Absyn_Global_b: 0){ _LL2910: _temp2909=(( struct Cyc_Absyn_Global_b_struct*)
_temp2893)->f1; goto _LL2900;} else{ goto _LL2901;} _LL2901: if(( unsigned int)
_temp2893 > 1u?*(( int*) _temp2893) == Cyc_Absyn_Local_b: 0){ _LL2912: _temp2911=((
struct Cyc_Absyn_Local_b_struct*) _temp2893)->f1; goto _LL2902;} else{ goto
_LL2903;} _LL2903: if(( unsigned int) _temp2893 > 1u?*(( int*) _temp2893) == Cyc_Absyn_Pat_b:
0){ _LL2914: _temp2913=(( struct Cyc_Absyn_Pat_b_struct*) _temp2893)->f1; goto
_LL2904;} else{ goto _LL2905;} _LL2905: if(( unsigned int) _temp2893 > 1u?*((
int*) _temp2893) == Cyc_Absyn_Param_b: 0){ _LL2916: _temp2915=(( struct Cyc_Absyn_Param_b_struct*)
_temp2893)->f1; goto _LL2906;} else{ goto _LL2894;} _LL2896: return bogus_ans;
_LL2898: return({ struct _tuple6 _temp2917; _temp2917.f1= 1; _temp2917.f2=( void*)
Cyc_Absyn_HeapRgn; _temp2917;}); _LL2900: { void* _temp2918= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); struct Cyc_Absyn_Exp*
_temp2924; struct Cyc_Absyn_Tqual _temp2926; void* _temp2928; _LL2920: if((
unsigned int) _temp2918 > 4u?*(( int*) _temp2918) == Cyc_Absyn_ArrayType: 0){
_LL2929: _temp2928=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2918)->f1;
goto _LL2927; _LL2927: _temp2926=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2918)->f2; goto _LL2925; _LL2925: _temp2924=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2918)->f3; goto _LL2921;} else{ goto _LL2922;} _LL2922: goto _LL2923;
_LL2921: return({ struct _tuple6 _temp2930; _temp2930.f1= 1; _temp2930.f2=( void*)
Cyc_Absyn_HeapRgn; _temp2930;}); _LL2923: return({ struct _tuple6 _temp2931;
_temp2931.f1=( _temp2909->tq).q_const; _temp2931.f2=( void*) Cyc_Absyn_HeapRgn;
_temp2931;}); _LL2919:;} _LL2902: { void* _temp2932= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v); struct Cyc_Absyn_Exp*
_temp2938; struct Cyc_Absyn_Tqual _temp2940; void* _temp2942; _LL2934: if((
unsigned int) _temp2932 > 4u?*(( int*) _temp2932) == Cyc_Absyn_ArrayType: 0){
_LL2943: _temp2942=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2932)->f1;
goto _LL2941; _LL2941: _temp2940=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2932)->f2; goto _LL2939; _LL2939: _temp2938=(( struct Cyc_Absyn_ArrayType_struct*)
_temp2932)->f3; goto _LL2935;} else{ goto _LL2936;} _LL2936: goto _LL2937;
_LL2935: return({ struct _tuple6 _temp2944; _temp2944.f1= 1; _temp2944.f2=( void*)((
struct Cyc_Core_Opt*) _check_null( _temp2911->rgn))->v; _temp2944;}); _LL2937:
return({ struct _tuple6 _temp2945; _temp2945.f1=( _temp2911->tq).q_const;
_temp2945.f2=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp2911->rgn))->v;
_temp2945;}); _LL2933:;} _LL2904: _temp2915= _temp2913; goto _LL2906; _LL2906:
return({ struct _tuple6 _temp2946; _temp2946.f1=( _temp2915->tq).q_const;
_temp2946.f2=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp2915->rgn))->v;
_temp2946;}); _LL2894:;} _LL2866: { void* _temp2947= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp2881->topt))->v); struct Cyc_List_List*
_temp2959; struct Cyc_List_List* _temp2961; struct Cyc_Absyn_Structdecl**
_temp2963; struct Cyc_Absyn_Structdecl* _temp2965; struct Cyc_List_List*
_temp2966; struct _tuple1* _temp2968; struct Cyc_Absyn_Uniondecl** _temp2970;
struct Cyc_Absyn_Uniondecl* _temp2972; struct Cyc_List_List* _temp2973; struct
_tuple1* _temp2975; _LL2949: if(( unsigned int) _temp2947 > 4u?*(( int*)
_temp2947) == Cyc_Absyn_AnonStructType: 0){ _LL2960: _temp2959=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp2947)->f1; goto _LL2950;} else{ goto _LL2951;} _LL2951: if(( unsigned int)
_temp2947 > 4u?*(( int*) _temp2947) == Cyc_Absyn_AnonUnionType: 0){ _LL2962:
_temp2961=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp2947)->f1; goto
_LL2952;} else{ goto _LL2953;} _LL2953: if(( unsigned int) _temp2947 > 4u?*((
int*) _temp2947) == Cyc_Absyn_StructType: 0){ _LL2969: _temp2968=(( struct Cyc_Absyn_StructType_struct*)
_temp2947)->f1; goto _LL2967; _LL2967: _temp2966=(( struct Cyc_Absyn_StructType_struct*)
_temp2947)->f2; goto _LL2964; _LL2964: _temp2963=(( struct Cyc_Absyn_StructType_struct*)
_temp2947)->f3; if( _temp2963 == 0){ goto _LL2955;} else{ _temp2965=* _temp2963;
goto _LL2954;}} else{ goto _LL2955;} _LL2955: if(( unsigned int) _temp2947 > 4u?*((
int*) _temp2947) == Cyc_Absyn_UnionType: 0){ _LL2976: _temp2975=(( struct Cyc_Absyn_UnionType_struct*)
_temp2947)->f1; goto _LL2974; _LL2974: _temp2973=(( struct Cyc_Absyn_UnionType_struct*)
_temp2947)->f2; goto _LL2971; _LL2971: _temp2970=(( struct Cyc_Absyn_UnionType_struct*)
_temp2947)->f3; if( _temp2970 == 0){ goto _LL2957;} else{ _temp2972=* _temp2970;
goto _LL2956;}} else{ goto _LL2957;} _LL2957: goto _LL2958; _LL2950: _temp2961=
_temp2959; goto _LL2952; _LL2952: { struct Cyc_Absyn_Structfield* _temp2977= Cyc_Absyn_lookup_field(
_temp2961, _temp2879); goto _LL2978; _LL2978: if( _temp2977 != 0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp2977))->width != 0: 0){ return({ struct _tuple6 _temp2979;
_temp2979.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp2977))->tq).q_const;
_temp2979.f2=( Cyc_Tcutil_addressof_props( te, _temp2881)).f2; _temp2979;});}
return bogus_ans;} _LL2954: { struct Cyc_Absyn_Structfield* _temp2980= Cyc_Absyn_lookup_struct_field(
_temp2965, _temp2879); goto _LL2981; _LL2981: if( _temp2980 != 0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp2980))->width != 0: 0){ return({ struct _tuple6 _temp2982;
_temp2982.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp2980))->tq).q_const;
_temp2982.f2=( Cyc_Tcutil_addressof_props( te, _temp2881)).f2; _temp2982;});}
return bogus_ans;} _LL2956: { struct Cyc_Absyn_Structfield* _temp2983= Cyc_Absyn_lookup_union_field(
_temp2972, _temp2879); goto _LL2984; _LL2984: if( _temp2983 != 0){ return({
struct _tuple6 _temp2985; _temp2985.f1=((( struct Cyc_Absyn_Structfield*)
_check_null( _temp2983))->tq).q_const; _temp2985.f2=( Cyc_Tcutil_addressof_props(
te, _temp2881)).f2; _temp2985;});} goto _LL2958;} _LL2958: return bogus_ans;
_LL2948:;} _LL2868: { void* _temp2986= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp2885->topt))->v); struct Cyc_Absyn_PtrInfo _temp2992; struct
Cyc_Absyn_Conref* _temp2994; struct Cyc_Absyn_Tqual _temp2996; struct Cyc_Absyn_Conref*
_temp2998; void* _temp3000; void* _temp3002; _LL2988: if(( unsigned int)
_temp2986 > 4u?*(( int*) _temp2986) == Cyc_Absyn_PointerType: 0){ _LL2993:
_temp2992=(( struct Cyc_Absyn_PointerType_struct*) _temp2986)->f1; _LL3003:
_temp3002=( void*) _temp2992.elt_typ; goto _LL3001; _LL3001: _temp3000=( void*)
_temp2992.rgn_typ; goto _LL2999; _LL2999: _temp2998= _temp2992.nullable; goto
_LL2997; _LL2997: _temp2996= _temp2992.tq; goto _LL2995; _LL2995: _temp2994=
_temp2992.bounds; goto _LL2989;} else{ goto _LL2990;} _LL2990: goto _LL2991;
_LL2989: { void* _temp3004= Cyc_Tcutil_compress( _temp3002); struct Cyc_List_List*
_temp3016; struct Cyc_List_List* _temp3018; struct Cyc_Absyn_Structdecl**
_temp3020; struct Cyc_Absyn_Structdecl* _temp3022; struct Cyc_List_List*
_temp3023; struct _tuple1* _temp3025; struct Cyc_Absyn_Uniondecl** _temp3027;
struct Cyc_Absyn_Uniondecl* _temp3029; struct Cyc_List_List* _temp3030; struct
_tuple1* _temp3032; _LL3006: if(( unsigned int) _temp3004 > 4u?*(( int*)
_temp3004) == Cyc_Absyn_AnonStructType: 0){ _LL3017: _temp3016=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp3004)->f1; goto _LL3007;} else{ goto _LL3008;} _LL3008: if(( unsigned int)
_temp3004 > 4u?*(( int*) _temp3004) == Cyc_Absyn_AnonUnionType: 0){ _LL3019:
_temp3018=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp3004)->f1; goto
_LL3009;} else{ goto _LL3010;} _LL3010: if(( unsigned int) _temp3004 > 4u?*((
int*) _temp3004) == Cyc_Absyn_StructType: 0){ _LL3026: _temp3025=(( struct Cyc_Absyn_StructType_struct*)
_temp3004)->f1; goto _LL3024; _LL3024: _temp3023=(( struct Cyc_Absyn_StructType_struct*)
_temp3004)->f2; goto _LL3021; _LL3021: _temp3020=(( struct Cyc_Absyn_StructType_struct*)
_temp3004)->f3; if( _temp3020 == 0){ goto _LL3012;} else{ _temp3022=* _temp3020;
goto _LL3011;}} else{ goto _LL3012;} _LL3012: if(( unsigned int) _temp3004 > 4u?*((
int*) _temp3004) == Cyc_Absyn_UnionType: 0){ _LL3033: _temp3032=(( struct Cyc_Absyn_UnionType_struct*)
_temp3004)->f1; goto _LL3031; _LL3031: _temp3030=(( struct Cyc_Absyn_UnionType_struct*)
_temp3004)->f2; goto _LL3028; _LL3028: _temp3027=(( struct Cyc_Absyn_UnionType_struct*)
_temp3004)->f3; if( _temp3027 == 0){ goto _LL3014;} else{ _temp3029=* _temp3027;
goto _LL3013;}} else{ goto _LL3014;} _LL3014: goto _LL3015; _LL3007: _temp3018=
_temp3016; goto _LL3009; _LL3009: { struct Cyc_Absyn_Structfield* _temp3034= Cyc_Absyn_lookup_field(
_temp3018, _temp2883); goto _LL3035; _LL3035: if( _temp3034 != 0?(( struct Cyc_Absyn_Structfield*)
_check_null( _temp3034))->width != 0: 0){ return({ struct _tuple6 _temp3036;
_temp3036.f1=((( struct Cyc_Absyn_Structfield*) _check_null( _temp3034))->tq).q_const;
_temp3036.f2= _temp3000; _temp3036;});} return bogus_ans;} _LL3011: { struct Cyc_Absyn_Structfield*
_temp3037= Cyc_Absyn_lookup_struct_field( _temp3022, _temp2883); goto _LL3038;
_LL3038: if( _temp3037 != 0?(( struct Cyc_Absyn_Structfield*) _check_null(
_temp3037))->width != 0: 0){ return({ struct _tuple6 _temp3039; _temp3039.f1=(((
struct Cyc_Absyn_Structfield*) _check_null( _temp3037))->tq).q_const; _temp3039.f2=
_temp3000; _temp3039;});} return bogus_ans;} _LL3013: { struct Cyc_Absyn_Structfield*
_temp3040= Cyc_Absyn_lookup_union_field( _temp3029, _temp2883); goto _LL3041;
_LL3041: if( _temp3040 != 0){ return({ struct _tuple6 _temp3042; _temp3042.f1=(((
struct Cyc_Absyn_Structfield*) _check_null( _temp3040))->tq).q_const; _temp3042.f2=
_temp3000; _temp3042;});} return bogus_ans;} _LL3015: return bogus_ans; _LL3005:;}
_LL2991: return bogus_ans; _LL2987:;} _LL2870: { void* _temp3043= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp2887->topt))->v); struct Cyc_Absyn_PtrInfo
_temp3049; struct Cyc_Absyn_Conref* _temp3051; struct Cyc_Absyn_Tqual _temp3053;
struct Cyc_Absyn_Conref* _temp3055; void* _temp3057; void* _temp3059; _LL3045:
if(( unsigned int) _temp3043 > 4u?*(( int*) _temp3043) == Cyc_Absyn_PointerType:
0){ _LL3050: _temp3049=(( struct Cyc_Absyn_PointerType_struct*) _temp3043)->f1;
_LL3060: _temp3059=( void*) _temp3049.elt_typ; goto _LL3058; _LL3058: _temp3057=(
void*) _temp3049.rgn_typ; goto _LL3056; _LL3056: _temp3055= _temp3049.nullable;
goto _LL3054; _LL3054: _temp3053= _temp3049.tq; goto _LL3052; _LL3052: _temp3051=
_temp3049.bounds; goto _LL3046;} else{ goto _LL3047;} _LL3047: goto _LL3048;
_LL3046: return({ struct _tuple6 _temp3061; _temp3061.f1= _temp3053.q_const;
_temp3061.f2= _temp3057; _temp3061;}); _LL3048: return bogus_ans; _LL3044:;}
_LL2872: { void* t= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp2891->topt))->v); void* _temp3062= t; struct Cyc_Absyn_Exp*
_temp3072; struct Cyc_Absyn_Tqual _temp3074; void* _temp3076; struct Cyc_List_List*
_temp3078; struct Cyc_Absyn_PtrInfo _temp3080; struct Cyc_Absyn_Conref*
_temp3082; struct Cyc_Absyn_Tqual _temp3084; struct Cyc_Absyn_Conref* _temp3086;
void* _temp3088; void* _temp3090; _LL3064: if(( unsigned int) _temp3062 > 4u?*((
int*) _temp3062) == Cyc_Absyn_ArrayType: 0){ _LL3077: _temp3076=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3062)->f1; goto _LL3075; _LL3075:
_temp3074=(( struct Cyc_Absyn_ArrayType_struct*) _temp3062)->f2; goto _LL3073;
_LL3073: _temp3072=(( struct Cyc_Absyn_ArrayType_struct*) _temp3062)->f3; goto
_LL3065;} else{ goto _LL3066;} _LL3066: if(( unsigned int) _temp3062 > 4u?*((
int*) _temp3062) == Cyc_Absyn_TupleType: 0){ _LL3079: _temp3078=(( struct Cyc_Absyn_TupleType_struct*)
_temp3062)->f1; goto _LL3067;} else{ goto _LL3068;} _LL3068: if(( unsigned int)
_temp3062 > 4u?*(( int*) _temp3062) == Cyc_Absyn_PointerType: 0){ _LL3081:
_temp3080=(( struct Cyc_Absyn_PointerType_struct*) _temp3062)->f1; _LL3091:
_temp3090=( void*) _temp3080.elt_typ; goto _LL3089; _LL3089: _temp3088=( void*)
_temp3080.rgn_typ; goto _LL3087; _LL3087: _temp3086= _temp3080.nullable; goto
_LL3085; _LL3085: _temp3084= _temp3080.tq; goto _LL3083; _LL3083: _temp3082=
_temp3080.bounds; goto _LL3069;} else{ goto _LL3070;} _LL3070: goto _LL3071;
_LL3065: return({ struct _tuple6 _temp3092; _temp3092.f1= _temp3074.q_const;
_temp3092.f2=( Cyc_Tcutil_addressof_props( te, _temp2891)).f2; _temp3092;});
_LL3067: { struct _tuple4* _temp3093= Cyc_Absyn_lookup_tuple_field( _temp3078,(
int) Cyc_Evexp_eval_const_uint_exp( _temp2889)); goto _LL3094; _LL3094: if(
_temp3093 != 0){ return({ struct _tuple6 _temp3095; _temp3095.f1=((*(( struct
_tuple4*) _check_null( _temp3093))).f1).q_const; _temp3095.f2=( Cyc_Tcutil_addressof_props(
te, _temp2891)).f2; _temp3095;});} return bogus_ans;} _LL3069: return({ struct
_tuple6 _temp3096; _temp3096.f1= _temp3084.q_const; _temp3096.f2= _temp3088;
_temp3096;}); _LL3071: return bogus_ans; _LL3063:;} _LL2874: Cyc_Tcutil_terr( e->loc,({
char* _temp3097="unary & applied to non-lvalue"; struct _tagged_string _temp3098;
_temp3098.curr= _temp3097; _temp3098.base= _temp3097; _temp3098.last_plus_one=
_temp3097 + 30; _temp3098;})); return bogus_ans; _LL2862:;} void* Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv* te, void* e_typ, struct Cyc_Absyn_Exp* e){ void*
_temp3100= Cyc_Tcutil_compress( e_typ); struct Cyc_Absyn_Exp* _temp3106; struct
Cyc_Absyn_Tqual _temp3108; void* _temp3110; _LL3102: if(( unsigned int)
_temp3100 > 4u?*(( int*) _temp3100) == Cyc_Absyn_ArrayType: 0){ _LL3111:
_temp3110=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3100)->f1; goto
_LL3109; _LL3109: _temp3108=(( struct Cyc_Absyn_ArrayType_struct*) _temp3100)->f2;
goto _LL3107; _LL3107: _temp3106=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3100)->f3; goto _LL3103;} else{ goto _LL3104;} _LL3104: goto _LL3105;
_LL3103: { void* _temp3114; int _temp3116; struct _tuple6 _temp3112= Cyc_Tcutil_addressof_props(
te, e); _LL3117: _temp3116= _temp3112.f1; goto _LL3115; _LL3115: _temp3114=
_temp3112.f2; goto _LL3113; _LL3113: return Cyc_Absyn_atb_typ( _temp3110,
_temp3114, _temp3108,( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp3118=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp3118[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp3119; _temp3119.tag= Cyc_Absyn_Upper_b;
_temp3119.f1= e; _temp3119;}); _temp3118;}));} _LL3105: return e_typ; _LL3101:;}
void Cyc_Tcutil_check_bound( struct Cyc_Position_Segment* loc, unsigned int i,
struct Cyc_Absyn_Conref* b){ b=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( b);{ void* _temp3120=( void*) b->v; void*
_temp3128; void* _temp3130; struct Cyc_Absyn_Exp* _temp3132; _LL3122: if((
unsigned int) _temp3120 > 1u?*(( int*) _temp3120) == Cyc_Absyn_Eq_constr: 0){
_LL3129: _temp3128=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3120)->f1;
if( _temp3128 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL3123;} else{ goto _LL3124;}}
else{ goto _LL3124;} _LL3124: if(( unsigned int) _temp3120 > 1u?*(( int*)
_temp3120) == Cyc_Absyn_Eq_constr: 0){ _LL3131: _temp3130=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3120)->f1; if(( unsigned int) _temp3130 > 1u?*(( int*) _temp3130) == Cyc_Absyn_Upper_b:
0){ _LL3133: _temp3132=(( struct Cyc_Absyn_Upper_b_struct*) _temp3130)->f1; goto
_LL3125;} else{ goto _LL3126;}} else{ goto _LL3126;} _LL3126: goto _LL3127;
_LL3123: return; _LL3125: if( Cyc_Evexp_eval_const_uint_exp( _temp3132) <= i){
Cyc_Tcutil_terr( loc,({ char* _temp3134="dereference is out of bounds"; struct
_tagged_string _temp3135; _temp3135.curr= _temp3134; _temp3135.base= _temp3134;
_temp3135.last_plus_one= _temp3134 + 29; _temp3135;}));} return; _LL3127:( void*)(
b->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp3136=( struct
Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp3136[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp3137; _temp3137.tag= Cyc_Absyn_Eq_constr;
_temp3137.f1=( void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp3138=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp3138[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp3139; _temp3139.tag= Cyc_Absyn_Upper_b;
_temp3139.f1= Cyc_Absyn_uint_exp( i + 1, 0); _temp3139;}); _temp3138;}));
_temp3137;}); _temp3136;}))); return; _LL3121:;}} void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Conref* b){ Cyc_Tcutil_check_bound(
loc, 0, b);} int Cyc_Tcutil_bits_only( void* t){ void* _temp3140= Cyc_Tcutil_compress(
t); void* _temp3166; void* _temp3168; struct Cyc_Absyn_Enumdecl* _temp3170;
struct _tuple1* _temp3172; struct Cyc_Absyn_Exp* _temp3174; struct Cyc_Absyn_Tqual
_temp3176; void* _temp3178; struct Cyc_List_List* _temp3180; struct Cyc_Absyn_Structdecl**
_temp3182; struct Cyc_List_List* _temp3184; struct _tuple1* _temp3186; struct
Cyc_Absyn_Uniondecl** _temp3188; struct Cyc_List_List* _temp3190; struct _tuple1*
_temp3192; struct Cyc_List_List* _temp3194; struct Cyc_List_List* _temp3196;
_LL3142: if( _temp3140 ==( void*) Cyc_Absyn_VoidType){ goto _LL3143;} else{ goto
_LL3144;} _LL3144: if(( unsigned int) _temp3140 > 4u?*(( int*) _temp3140) == Cyc_Absyn_IntType:
0){ _LL3169: _temp3168=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp3140)->f1;
goto _LL3167; _LL3167: _temp3166=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp3140)->f2; goto _LL3145;} else{ goto _LL3146;} _LL3146: if( _temp3140 ==(
void*) Cyc_Absyn_FloatType){ goto _LL3147;} else{ goto _LL3148;} _LL3148: if(
_temp3140 ==( void*) Cyc_Absyn_DoubleType){ goto _LL3149;} else{ goto _LL3150;}
_LL3150: if(( unsigned int) _temp3140 > 4u?*(( int*) _temp3140) == Cyc_Absyn_EnumType:
0){ _LL3173: _temp3172=(( struct Cyc_Absyn_EnumType_struct*) _temp3140)->f1;
goto _LL3171; _LL3171: _temp3170=(( struct Cyc_Absyn_EnumType_struct*) _temp3140)->f2;
goto _LL3151;} else{ goto _LL3152;} _LL3152: if(( unsigned int) _temp3140 > 4u?*((
int*) _temp3140) == Cyc_Absyn_ArrayType: 0){ _LL3179: _temp3178=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3140)->f1; goto _LL3177; _LL3177:
_temp3176=(( struct Cyc_Absyn_ArrayType_struct*) _temp3140)->f2; goto _LL3175;
_LL3175: _temp3174=(( struct Cyc_Absyn_ArrayType_struct*) _temp3140)->f3; goto
_LL3153;} else{ goto _LL3154;} _LL3154: if(( unsigned int) _temp3140 > 4u?*((
int*) _temp3140) == Cyc_Absyn_TupleType: 0){ _LL3181: _temp3180=(( struct Cyc_Absyn_TupleType_struct*)
_temp3140)->f1; goto _LL3155;} else{ goto _LL3156;} _LL3156: if(( unsigned int)
_temp3140 > 4u?*(( int*) _temp3140) == Cyc_Absyn_StructType: 0){ _LL3187:
_temp3186=(( struct Cyc_Absyn_StructType_struct*) _temp3140)->f1; goto _LL3185;
_LL3185: _temp3184=(( struct Cyc_Absyn_StructType_struct*) _temp3140)->f2; goto
_LL3183; _LL3183: _temp3182=(( struct Cyc_Absyn_StructType_struct*) _temp3140)->f3;
goto _LL3157;} else{ goto _LL3158;} _LL3158: if(( unsigned int) _temp3140 > 4u?*((
int*) _temp3140) == Cyc_Absyn_UnionType: 0){ _LL3193: _temp3192=(( struct Cyc_Absyn_UnionType_struct*)
_temp3140)->f1; goto _LL3191; _LL3191: _temp3190=(( struct Cyc_Absyn_UnionType_struct*)
_temp3140)->f2; goto _LL3189; _LL3189: _temp3188=(( struct Cyc_Absyn_UnionType_struct*)
_temp3140)->f3; goto _LL3159;} else{ goto _LL3160;} _LL3160: if(( unsigned int)
_temp3140 > 4u?*(( int*) _temp3140) == Cyc_Absyn_AnonStructType: 0){ _LL3195:
_temp3194=(( struct Cyc_Absyn_AnonStructType_struct*) _temp3140)->f1; goto
_LL3161;} else{ goto _LL3162;} _LL3162: if(( unsigned int) _temp3140 > 4u?*((
int*) _temp3140) == Cyc_Absyn_AnonUnionType: 0){ _LL3197: _temp3196=(( struct
Cyc_Absyn_AnonUnionType_struct*) _temp3140)->f1; goto _LL3163;} else{ goto
_LL3164;} _LL3164: goto _LL3165; _LL3143: return 1; _LL3145: return 1; _LL3147:
return 1; _LL3149: return 1; _LL3151: return 1; _LL3153: return Cyc_Tcutil_bits_only(
_temp3178); _LL3155: for( 0; _temp3180 != 0; _temp3180=(( struct Cyc_List_List*)
_check_null( _temp3180))->tl){ if( ! Cyc_Tcutil_bits_only((*(( struct _tuple4*)((
struct Cyc_List_List*) _check_null( _temp3180))->hd)).f2)){ return 0;}} return 1;
_LL3157: if( _temp3182 == 0){ return 0;}{ struct Cyc_Absyn_Structdecl* _temp3198=*((
struct Cyc_Absyn_Structdecl**) _check_null( _temp3182)); goto _LL3199; _LL3199:
if( _temp3198->fields == 0){ return 0;}{ struct _RegionHandle _temp3200=
_new_region(); struct _RegionHandle* rgn=& _temp3200; _push_region( rgn);{
struct Cyc_List_List* _temp3201=(( struct Cyc_List_List*(*)( struct
_RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_rzip)( rgn, rgn, _temp3198->tvs, _temp3184); goto _LL3202; _LL3202:{
struct Cyc_List_List* fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp3198->fields))->v; for( 0; fs != 0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ if( ! Cyc_Tcutil_bits_only( Cyc_Tcutil_rsubstitute( rgn,
_temp3201,( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->type))){ int _temp3203= 0; _npop_handler( 0u); return
_temp3203;}}}{ int _temp3204= 1; _npop_handler( 0u); return _temp3204;}};
_pop_region(& _temp3200);}} _LL3159: if( _temp3188 == 0){ return 0;}{ struct Cyc_Absyn_Uniondecl*
_temp3205=*(( struct Cyc_Absyn_Uniondecl**) _check_null( _temp3188)); goto
_LL3206; _LL3206: if( _temp3205->fields == 0){ return 0;}{ struct _RegionHandle
_temp3207= _new_region(); struct _RegionHandle* rgn=& _temp3207; _push_region(
rgn);{ struct Cyc_List_List* _temp3208=(( struct Cyc_List_List*(*)( struct
_RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_rzip)( rgn, rgn, _temp3205->tvs, _temp3190); goto _LL3209; _LL3209:{
struct Cyc_List_List* fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp3205->fields))->v; for( 0; fs != 0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ if( ! Cyc_Tcutil_bits_only( Cyc_Tcutil_rsubstitute( rgn,
_temp3208,( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( fs))->hd)->type))){ int _temp3210= 0; _npop_handler( 0u); return
_temp3210;}}}{ int _temp3211= 1; _npop_handler( 0u); return _temp3211;}};
_pop_region(& _temp3207);}} _LL3161: _temp3196= _temp3194; goto _LL3163; _LL3163:
for( 0; _temp3196 != 0; _temp3196=(( struct Cyc_List_List*) _check_null(
_temp3196))->tl){ if( ! Cyc_Tcutil_bits_only(( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp3196))->hd)->type)){ return 0;}} return
1; _LL3165: return 0; _LL3141:;} struct _tuple18{ struct Cyc_List_List* f1;
struct Cyc_Absyn_Exp* f2; } ; static int Cyc_Tcutil_cnst_exp( struct Cyc_Tcenv_Tenv*
te, int var_okay, struct Cyc_Absyn_Exp* e){ void* _temp3212=( void*) e->r; void*
_temp3250; void* _temp3252; struct Cyc_Absyn_Exp* _temp3254; struct Cyc_Absyn_Enumfield*
_temp3256; struct Cyc_Absyn_Enumdecl* _temp3258; struct _tuple1* _temp3260; void*
_temp3262; struct _tuple1* _temp3264; struct Cyc_Absyn_Exp* _temp3266; struct
Cyc_Absyn_Exp* _temp3268; struct Cyc_Absyn_Exp* _temp3270; struct Cyc_Absyn_Exp*
_temp3272; struct Cyc_Absyn_Exp* _temp3274; struct Cyc_List_List* _temp3276;
struct Cyc_Absyn_Exp* _temp3278; struct Cyc_Absyn_Exp* _temp3280; void*
_temp3282; struct Cyc_Absyn_Exp* _temp3284; struct Cyc_Absyn_Exp* _temp3286;
struct Cyc_Absyn_Exp* _temp3288; struct Cyc_Absyn_Vardecl* _temp3290; struct Cyc_List_List*
_temp3292; struct Cyc_List_List* _temp3294; void* _temp3296; struct Cyc_Absyn_Structdecl*
_temp3298; struct Cyc_List_List* _temp3300; struct Cyc_Core_Opt* _temp3302;
struct _tuple1* _temp3304; struct Cyc_List_List* _temp3306; void* _temp3308;
struct Cyc_List_List* _temp3310; struct Cyc_Absyn_Tunionfield* _temp3312; struct
Cyc_Absyn_Tuniondecl* _temp3314; struct Cyc_List_List* _temp3316; struct Cyc_Core_Opt*
_temp3318; struct Cyc_Core_Opt* _temp3320; _LL3214: if(*(( int*) _temp3212) ==
Cyc_Absyn_Const_e){ _LL3251: _temp3250=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp3212)->f1; goto _LL3215;} else{ goto _LL3216;} _LL3216: if(*(( int*)
_temp3212) == Cyc_Absyn_Sizeoftyp_e){ _LL3253: _temp3252=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp3212)->f1; goto _LL3217;} else{ goto _LL3218;} _LL3218: if(*(( int*)
_temp3212) == Cyc_Absyn_Sizeofexp_e){ _LL3255: _temp3254=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp3212)->f1; goto _LL3219;} else{ goto _LL3220;} _LL3220: if(*(( int*)
_temp3212) == Cyc_Absyn_Enum_e){ _LL3261: _temp3260=(( struct Cyc_Absyn_Enum_e_struct*)
_temp3212)->f1; goto _LL3259; _LL3259: _temp3258=(( struct Cyc_Absyn_Enum_e_struct*)
_temp3212)->f2; goto _LL3257; _LL3257: _temp3256=(( struct Cyc_Absyn_Enum_e_struct*)
_temp3212)->f3; goto _LL3221;} else{ goto _LL3222;} _LL3222: if(*(( int*)
_temp3212) == Cyc_Absyn_Var_e){ _LL3265: _temp3264=(( struct Cyc_Absyn_Var_e_struct*)
_temp3212)->f1; goto _LL3263; _LL3263: _temp3262=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp3212)->f2; goto _LL3223;} else{ goto _LL3224;} _LL3224: if(*(( int*)
_temp3212) == Cyc_Absyn_Conditional_e){ _LL3271: _temp3270=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp3212)->f1; goto _LL3269; _LL3269: _temp3268=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp3212)->f2; goto _LL3267; _LL3267: _temp3266=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp3212)->f3; goto _LL3225;} else{ goto _LL3226;} _LL3226: if(*(( int*)
_temp3212) == Cyc_Absyn_SeqExp_e){ _LL3275: _temp3274=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp3212)->f1; goto _LL3273; _LL3273: _temp3272=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp3212)->f2; goto _LL3227;} else{ goto _LL3228;} _LL3228: if(*(( int*)
_temp3212) == Cyc_Absyn_Instantiate_e){ _LL3279: _temp3278=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp3212)->f1; goto _LL3277; _LL3277: _temp3276=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp3212)->f2; goto _LL3229;} else{ goto _LL3230;} _LL3230: if(*(( int*)
_temp3212) == Cyc_Absyn_Cast_e){ _LL3283: _temp3282=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp3212)->f1; goto _LL3281; _LL3281: _temp3280=(( struct Cyc_Absyn_Cast_e_struct*)
_temp3212)->f2; goto _LL3231;} else{ goto _LL3232;} _LL3232: if(*(( int*)
_temp3212) == Cyc_Absyn_Address_e){ _LL3285: _temp3284=(( struct Cyc_Absyn_Address_e_struct*)
_temp3212)->f1; goto _LL3233;} else{ goto _LL3234;} _LL3234: if(*(( int*)
_temp3212) == Cyc_Absyn_Comprehension_e){ _LL3291: _temp3290=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3212)->f1; goto _LL3289; _LL3289: _temp3288=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3212)->f2; goto _LL3287; _LL3287: _temp3286=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3212)->f3; goto _LL3235;} else{ goto _LL3236;} _LL3236: if(*(( int*)
_temp3212) == Cyc_Absyn_Array_e){ _LL3293: _temp3292=(( struct Cyc_Absyn_Array_e_struct*)
_temp3212)->f1; goto _LL3237;} else{ goto _LL3238;} _LL3238: if(*(( int*)
_temp3212) == Cyc_Absyn_AnonStruct_e){ _LL3297: _temp3296=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp3212)->f1; goto _LL3295; _LL3295: _temp3294=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp3212)->f2; goto _LL3239;} else{ goto _LL3240;} _LL3240: if(*(( int*)
_temp3212) == Cyc_Absyn_Struct_e){ _LL3305: _temp3304=(( struct Cyc_Absyn_Struct_e_struct*)
_temp3212)->f1; goto _LL3303; _LL3303: _temp3302=(( struct Cyc_Absyn_Struct_e_struct*)
_temp3212)->f2; goto _LL3301; _LL3301: _temp3300=(( struct Cyc_Absyn_Struct_e_struct*)
_temp3212)->f3; goto _LL3299; _LL3299: _temp3298=(( struct Cyc_Absyn_Struct_e_struct*)
_temp3212)->f4; goto _LL3241;} else{ goto _LL3242;} _LL3242: if(*(( int*)
_temp3212) == Cyc_Absyn_Primop_e){ _LL3309: _temp3308=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp3212)->f1; goto _LL3307; _LL3307: _temp3306=(( struct Cyc_Absyn_Primop_e_struct*)
_temp3212)->f2; goto _LL3243;} else{ goto _LL3244;} _LL3244: if(*(( int*)
_temp3212) == Cyc_Absyn_Tuple_e){ _LL3311: _temp3310=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp3212)->f1; goto _LL3245;} else{ goto _LL3246;} _LL3246: if(*(( int*)
_temp3212) == Cyc_Absyn_Tunion_e){ _LL3321: _temp3320=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3212)->f1; goto _LL3319; _LL3319: _temp3318=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3212)->f2; goto _LL3317; _LL3317: _temp3316=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3212)->f3; goto _LL3315; _LL3315: _temp3314=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3212)->f4; goto _LL3313; _LL3313: _temp3312=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3212)->f5; goto _LL3247;} else{ goto _LL3248;} _LL3248: goto _LL3249;
_LL3215: return 1; _LL3217: return 1; _LL3219: return Cyc_Tcutil_cnst_exp( te, 1,
_temp3254); _LL3221: return 1; _LL3223: { struct _handler_cons _temp3322;
_push_handler(& _temp3322);{ int _temp3324= 0; if( setjmp( _temp3322.handler)){
_temp3324= 1;} if( ! _temp3324){{ void* _temp3325= Cyc_Tcenv_lookup_ordinary( te,
e->loc, _temp3264); void* _temp3333; struct Cyc_Absyn_Fndecl* _temp3335; void*
_temp3337; struct Cyc_Absyn_Vardecl* _temp3339; _LL3327: if(*(( int*) _temp3325)
== Cyc_Tcenv_VarRes){ _LL3334: _temp3333=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp3325)->f1; if(( unsigned int) _temp3333 > 1u?*(( int*) _temp3333) == Cyc_Absyn_Funname_b:
0){ _LL3336: _temp3335=(( struct Cyc_Absyn_Funname_b_struct*) _temp3333)->f1;
goto _LL3328;} else{ goto _LL3329;}} else{ goto _LL3329;} _LL3329: if(*(( int*)
_temp3325) == Cyc_Tcenv_VarRes){ _LL3338: _temp3337=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp3325)->f1; if(( unsigned int) _temp3337 > 1u?*(( int*) _temp3337) == Cyc_Absyn_Global_b:
0){ _LL3340: _temp3339=(( struct Cyc_Absyn_Global_b_struct*) _temp3337)->f1;
goto _LL3330;} else{ goto _LL3331;}} else{ goto _LL3331;} _LL3331: goto _LL3332;
_LL3328: { int _temp3341= 1; _npop_handler( 0u); return _temp3341;} _LL3330: {
void* _temp3342= Cyc_Tcutil_compress(( void*) _temp3339->type); struct Cyc_Absyn_Exp*
_temp3348; struct Cyc_Absyn_Tqual _temp3350; void* _temp3352; _LL3344: if((
unsigned int) _temp3342 > 4u?*(( int*) _temp3342) == Cyc_Absyn_ArrayType: 0){
_LL3353: _temp3352=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3342)->f1;
goto _LL3351; _LL3351: _temp3350=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3342)->f2; goto _LL3349; _LL3349: _temp3348=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3342)->f3; goto _LL3345;} else{ goto _LL3346;} _LL3346: goto _LL3347;
_LL3345: { int _temp3354= 1; _npop_handler( 0u); return _temp3354;} _LL3347: {
int _temp3355= var_okay; _npop_handler( 0u); return _temp3355;} _LL3343:;}
_LL3332: { int _temp3356= var_okay; _npop_handler( 0u); return _temp3356;}
_LL3326:;}; _pop_handler();} else{ void* _temp3323=( void*) _exn_thrown; void*
_temp3358= _temp3323; _LL3360: if( _temp3358 == Cyc_Dict_Absent){ goto _LL3361;}
else{ goto _LL3362;} _LL3362: goto _LL3363; _LL3361: return 0; _LL3363:( void)
_throw( _temp3358); _LL3359:;}}} _LL3225: return( Cyc_Tcutil_cnst_exp( te, 0,
_temp3270)? Cyc_Tcutil_cnst_exp( te, 0, _temp3268): 0)? Cyc_Tcutil_cnst_exp( te,
0, _temp3266): 0; _LL3227: return Cyc_Tcutil_cnst_exp( te, 0, _temp3274)? Cyc_Tcutil_cnst_exp(
te, 0, _temp3272): 0; _LL3229: return Cyc_Tcutil_cnst_exp( te, var_okay,
_temp3278); _LL3231: return Cyc_Tcutil_cnst_exp( te, var_okay, _temp3280);
_LL3233: return Cyc_Tcutil_cnst_exp( te, 1, _temp3284); _LL3235: return Cyc_Tcutil_cnst_exp(
te, 0, _temp3288)? Cyc_Tcutil_cnst_exp( te, 0, _temp3286): 0; _LL3237: _temp3294=
_temp3292; goto _LL3239; _LL3239: _temp3300= _temp3294; goto _LL3241; _LL3241:
for( 0; _temp3300 != 0; _temp3300=(( struct Cyc_List_List*) _check_null(
_temp3300))->tl){ if( ! Cyc_Tcutil_cnst_exp( te, 0,(*(( struct _tuple18*)((
struct Cyc_List_List*) _check_null( _temp3300))->hd)).f2)){ return 0;}} return 1;
_LL3243: _temp3310= _temp3306; goto _LL3245; _LL3245: _temp3316= _temp3310; goto
_LL3247; _LL3247: for( 0; _temp3316 != 0; _temp3316=(( struct Cyc_List_List*)
_check_null( _temp3316))->tl){ if( ! Cyc_Tcutil_cnst_exp( te, 0,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp3316))->hd)){ return 0;}} return 1;
_LL3249: return 0; _LL3213:;} int Cyc_Tcutil_is_const_exp( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e){ return Cyc_Tcutil_cnst_exp( te, 0, e);} static int
Cyc_Tcutil_fields_support_default( struct Cyc_List_List* tvs, struct Cyc_List_List*
ts, struct Cyc_List_List* fs); int Cyc_Tcutil_supports_default( void* t){ void*
_temp3364= Cyc_Tcutil_compress( t); int _temp3402; struct Cyc_Core_Opt*
_temp3404; void* _temp3406; struct Cyc_Absyn_Tvar* _temp3408; struct Cyc_Absyn_TunionInfo
_temp3410; struct Cyc_Absyn_TunionFieldInfo _temp3412; struct Cyc_Absyn_PtrInfo
_temp3414; struct Cyc_Absyn_Conref* _temp3416; struct Cyc_Absyn_Tqual _temp3418;
struct Cyc_Absyn_Conref* _temp3420; void* _temp3422; void* _temp3424; void*
_temp3426; void* _temp3428; struct Cyc_Absyn_Exp* _temp3430; struct Cyc_Absyn_Tqual
_temp3432; void* _temp3434; struct Cyc_Absyn_FnInfo _temp3436; struct Cyc_List_List*
_temp3438; struct Cyc_Absyn_Structdecl** _temp3440; struct Cyc_List_List*
_temp3442; struct _tuple1* _temp3444; struct Cyc_Absyn_Uniondecl** _temp3446;
struct Cyc_List_List* _temp3448; struct _tuple1* _temp3450; struct Cyc_List_List*
_temp3452; struct Cyc_List_List* _temp3454; struct Cyc_Absyn_Enumdecl* _temp3456;
struct _tuple1* _temp3458; _LL3366: if( _temp3364 ==( void*) Cyc_Absyn_VoidType){
goto _LL3367;} else{ goto _LL3368;} _LL3368: if(( unsigned int) _temp3364 > 4u?*((
int*) _temp3364) == Cyc_Absyn_Evar: 0){ _LL3407: _temp3406=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp3364)->f1; goto _LL3405; _LL3405: _temp3404=(( struct Cyc_Absyn_Evar_struct*)
_temp3364)->f2; goto _LL3403; _LL3403: _temp3402=(( struct Cyc_Absyn_Evar_struct*)
_temp3364)->f3; goto _LL3369;} else{ goto _LL3370;} _LL3370: if(( unsigned int)
_temp3364 > 4u?*(( int*) _temp3364) == Cyc_Absyn_VarType: 0){ _LL3409: _temp3408=((
struct Cyc_Absyn_VarType_struct*) _temp3364)->f1; goto _LL3371;} else{ goto
_LL3372;} _LL3372: if(( unsigned int) _temp3364 > 4u?*(( int*) _temp3364) == Cyc_Absyn_TunionType:
0){ _LL3411: _temp3410=(( struct Cyc_Absyn_TunionType_struct*) _temp3364)->f1;
goto _LL3373;} else{ goto _LL3374;} _LL3374: if(( unsigned int) _temp3364 > 4u?*((
int*) _temp3364) == Cyc_Absyn_TunionFieldType: 0){ _LL3413: _temp3412=(( struct
Cyc_Absyn_TunionFieldType_struct*) _temp3364)->f1; goto _LL3375;} else{ goto
_LL3376;} _LL3376: if(( unsigned int) _temp3364 > 4u?*(( int*) _temp3364) == Cyc_Absyn_PointerType:
0){ _LL3415: _temp3414=(( struct Cyc_Absyn_PointerType_struct*) _temp3364)->f1;
_LL3425: _temp3424=( void*) _temp3414.elt_typ; goto _LL3423; _LL3423: _temp3422=(
void*) _temp3414.rgn_typ; goto _LL3421; _LL3421: _temp3420= _temp3414.nullable;
goto _LL3419; _LL3419: _temp3418= _temp3414.tq; goto _LL3417; _LL3417: _temp3416=
_temp3414.bounds; goto _LL3377;} else{ goto _LL3378;} _LL3378: if(( unsigned int)
_temp3364 > 4u?*(( int*) _temp3364) == Cyc_Absyn_IntType: 0){ _LL3429: _temp3428=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp3364)->f1; goto _LL3427; _LL3427:
_temp3426=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp3364)->f2; goto
_LL3379;} else{ goto _LL3380;} _LL3380: if( _temp3364 ==( void*) Cyc_Absyn_FloatType){
goto _LL3381;} else{ goto _LL3382;} _LL3382: if( _temp3364 ==( void*) Cyc_Absyn_DoubleType){
goto _LL3383;} else{ goto _LL3384;} _LL3384: if(( unsigned int) _temp3364 > 4u?*((
int*) _temp3364) == Cyc_Absyn_ArrayType: 0){ _LL3435: _temp3434=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3364)->f1; goto _LL3433; _LL3433:
_temp3432=(( struct Cyc_Absyn_ArrayType_struct*) _temp3364)->f2; goto _LL3431;
_LL3431: _temp3430=(( struct Cyc_Absyn_ArrayType_struct*) _temp3364)->f3; goto
_LL3385;} else{ goto _LL3386;} _LL3386: if(( unsigned int) _temp3364 > 4u?*((
int*) _temp3364) == Cyc_Absyn_FnType: 0){ _LL3437: _temp3436=(( struct Cyc_Absyn_FnType_struct*)
_temp3364)->f1; goto _LL3387;} else{ goto _LL3388;} _LL3388: if(( unsigned int)
_temp3364 > 4u?*(( int*) _temp3364) == Cyc_Absyn_TupleType: 0){ _LL3439:
_temp3438=(( struct Cyc_Absyn_TupleType_struct*) _temp3364)->f1; goto _LL3389;}
else{ goto _LL3390;} _LL3390: if(( unsigned int) _temp3364 > 4u?*(( int*)
_temp3364) == Cyc_Absyn_StructType: 0){ _LL3445: _temp3444=(( struct Cyc_Absyn_StructType_struct*)
_temp3364)->f1; goto _LL3443; _LL3443: _temp3442=(( struct Cyc_Absyn_StructType_struct*)
_temp3364)->f2; goto _LL3441; _LL3441: _temp3440=(( struct Cyc_Absyn_StructType_struct*)
_temp3364)->f3; goto _LL3391;} else{ goto _LL3392;} _LL3392: if(( unsigned int)
_temp3364 > 4u?*(( int*) _temp3364) == Cyc_Absyn_UnionType: 0){ _LL3451:
_temp3450=(( struct Cyc_Absyn_UnionType_struct*) _temp3364)->f1; goto _LL3449;
_LL3449: _temp3448=(( struct Cyc_Absyn_UnionType_struct*) _temp3364)->f2; goto
_LL3447; _LL3447: _temp3446=(( struct Cyc_Absyn_UnionType_struct*) _temp3364)->f3;
goto _LL3393;} else{ goto _LL3394;} _LL3394: if(( unsigned int) _temp3364 > 4u?*((
int*) _temp3364) == Cyc_Absyn_AnonStructType: 0){ _LL3453: _temp3452=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp3364)->f1; goto _LL3395;} else{ goto
_LL3396;} _LL3396: if(( unsigned int) _temp3364 > 4u?*(( int*) _temp3364) == Cyc_Absyn_AnonUnionType:
0){ _LL3455: _temp3454=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp3364)->f1;
goto _LL3397;} else{ goto _LL3398;} _LL3398: if(( unsigned int) _temp3364 > 4u?*((
int*) _temp3364) == Cyc_Absyn_EnumType: 0){ _LL3459: _temp3458=(( struct Cyc_Absyn_EnumType_struct*)
_temp3364)->f1; goto _LL3457; _LL3457: _temp3456=(( struct Cyc_Absyn_EnumType_struct*)
_temp3364)->f2; goto _LL3399;} else{ goto _LL3400;} _LL3400: goto _LL3401;
_LL3367: return 1; _LL3369: return 0; _LL3371: return 0; _LL3373: return 0;
_LL3375: return 0; _LL3377: { void* _temp3460=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp3416))->v; void*
_temp3468; void* _temp3470; struct Cyc_Absyn_Exp* _temp3472; _LL3462: if((
unsigned int) _temp3460 > 1u?*(( int*) _temp3460) == Cyc_Absyn_Eq_constr: 0){
_LL3469: _temp3468=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3460)->f1;
if( _temp3468 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL3463;} else{ goto _LL3464;}}
else{ goto _LL3464;} _LL3464: if(( unsigned int) _temp3460 > 1u?*(( int*)
_temp3460) == Cyc_Absyn_Eq_constr: 0){ _LL3471: _temp3470=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3460)->f1; if(( unsigned int) _temp3470 > 1u?*(( int*) _temp3470) == Cyc_Absyn_Upper_b:
0){ _LL3473: _temp3472=(( struct Cyc_Absyn_Upper_b_struct*) _temp3470)->f1; goto
_LL3465;} else{ goto _LL3466;}} else{ goto _LL3466;} _LL3466: goto _LL3467;
_LL3463: return 1; _LL3465: { void* _temp3474=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp3420))->v; int
_temp3480; _LL3476: if(( unsigned int) _temp3474 > 1u?*(( int*) _temp3474) ==
Cyc_Absyn_Eq_constr: 0){ _LL3481: _temp3480=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3474)->f1; goto _LL3477;} else{ goto _LL3478;} _LL3478: goto _LL3479;
_LL3477: return _temp3480; _LL3479: return 0; _LL3475:;} _LL3467: return 0;
_LL3461:;} _LL3379: return 1; _LL3381: return 1; _LL3383: return 1; _LL3385:
return Cyc_Tcutil_supports_default( _temp3434); _LL3387: return 0; _LL3389: for(
0; _temp3438 != 0; _temp3438=(( struct Cyc_List_List*) _check_null( _temp3438))->tl){
if( ! Cyc_Tcutil_supports_default((*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp3438))->hd)).f2)){ return 0;}} return 1; _LL3391: if(
_temp3440 == 0){ return 0;}{ struct Cyc_Absyn_Structdecl* sd=*(( struct Cyc_Absyn_Structdecl**)
_check_null( _temp3440)); if( sd->fields == 0){ return 0;} return Cyc_Tcutil_fields_support_default(
sd->tvs, _temp3442,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
sd->fields))->v);} _LL3393: if( _temp3446 == 0){ return 0;}{ struct Cyc_Absyn_Uniondecl*
ud=*(( struct Cyc_Absyn_Uniondecl**) _check_null( _temp3446)); if( ud->fields ==
0){ return 0;} return Cyc_Tcutil_fields_support_default( ud->tvs, _temp3448,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( ud->fields))->v);}
_LL3395: _temp3454= _temp3452; goto _LL3397; _LL3397: return Cyc_Tcutil_fields_support_default(
0, 0, _temp3454); _LL3399: return 1; _LL3401: return 0; _LL3365:;} static int
Cyc_Tcutil_fields_support_default( struct Cyc_List_List* tvs, struct Cyc_List_List*
ts, struct Cyc_List_List* fs){{ struct _RegionHandle _temp3482= _new_region();
struct _RegionHandle* rgn=& _temp3482; _push_region( rgn);{ struct Cyc_List_List*
_temp3483=(( struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct
_RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, tvs, ts); goto _LL3484; _LL3484: for( 0; fs != 0; fs=(( struct Cyc_List_List*)
_check_null( fs))->tl){ void* t= Cyc_Tcutil_rsubstitute( rgn, _temp3483,( void*)((
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( fs))->hd)->type);
if( ! Cyc_Tcutil_supports_default( t)){ int _temp3485= 0; _npop_handler( 0u);
return _temp3485;}}}; _pop_region(& _temp3482);} return 1;}