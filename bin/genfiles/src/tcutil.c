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
struct Cyc_List_List* x); extern void* Cyc_List_assoc_cmp( int(* compare)( void*,
void*), struct Cyc_List_List* l, void* x); struct Cyc_Lineno_Pos{ struct
_tagged_string logical_file; struct _tagged_string line; int line_no; int col; }
; typedef struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t; extern char Cyc_Position_Exit[
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
f2; struct Cyc_Absyn_Uniondecl** f3; } ; static const int Cyc_Absyn_EnumType= 11;
struct Cyc_Absyn_EnumType_struct{ int tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl*
f2; } ; static const int Cyc_Absyn_RgnHandleType= 12; struct Cyc_Absyn_RgnHandleType_struct{
int tag; void* f1; } ; static const int Cyc_Absyn_TypedefType= 13; struct Cyc_Absyn_TypedefType_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3;
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
struct Cyc_Absyn_Structdecl* f4; } ; static const int Cyc_Absyn_Tunion_e= 27;
struct Cyc_Absyn_Tunion_e_struct{ int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield*
f5; } ; static const int Cyc_Absyn_Enum_e= 28; struct Cyc_Absyn_Enum_e_struct{
int tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
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
extern struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_struct_field( struct Cyc_Absyn_Structdecl*,
struct _tagged_string*); extern struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_union_field(
struct Cyc_Absyn_Uniondecl*, struct _tagged_string*); struct _tuple4{ struct Cyc_Absyn_Tqual
f1; void* f2; } ; extern struct _tuple4* Cyc_Absyn_lookup_tuple_field( struct
Cyc_List_List*, int); extern struct _tagged_string Cyc_Absyn_attribute2string(
void*); struct Cyc_PP_Ppstate; typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t;
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
loc, struct Cyc_List_List* des, struct Cyc_Absyn_Structdecl* sd); extern int Cyc_Tcutil_is_tagged_pointer_typ(
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
Cyc_Tcutil_compress( t); int _temp130; struct Cyc_Core_Opt* _temp132; void*
_temp134; struct Cyc_Absyn_Tvar* _temp136; void* _temp138; void* _temp140;
struct Cyc_Absyn_FnInfo _temp142; void* _temp144; struct Cyc_Absyn_TunionInfo
_temp146; struct Cyc_Absyn_TunionFieldInfo _temp148; struct Cyc_List_List*
_temp150; void* _temp152; struct Cyc_Absyn_Tunionfield* _temp154; struct Cyc_Absyn_Tuniondecl*
_temp156; struct Cyc_Absyn_TunionFieldInfo _temp158; struct Cyc_List_List*
_temp160; void* _temp162; struct Cyc_Absyn_UnknownTunionFieldInfo _temp164;
struct Cyc_Absyn_Structdecl** _temp166; struct Cyc_List_List* _temp168; struct
_tuple1* _temp170; struct Cyc_Absyn_Uniondecl** _temp172; struct Cyc_List_List*
_temp174; struct _tuple1* _temp176; struct Cyc_Absyn_Structdecl** _temp178;
struct Cyc_List_List* _temp180; struct _tuple1* _temp182; struct Cyc_Absyn_Uniondecl**
_temp184; struct Cyc_List_List* _temp186; struct _tuple1* _temp188; struct Cyc_Absyn_Enumdecl*
_temp190; struct _tuple1* _temp192; struct Cyc_Absyn_PtrInfo _temp194; struct
Cyc_Absyn_Exp* _temp196; struct Cyc_Absyn_Tqual _temp198; void* _temp200; struct
Cyc_List_List* _temp202; struct Cyc_Core_Opt* _temp204; struct Cyc_List_List*
_temp206; struct _tuple1* _temp208; void* _temp210; struct Cyc_List_List*
_temp212; _LL84: if(( unsigned int) _temp82 > 4u?*(( int*) _temp82) == Cyc_Absyn_Evar:
0){ _LL135: _temp134=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp82)->f1;
goto _LL133; _LL133: _temp132=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp82)->f2; goto _LL131; _LL131: _temp130=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp82)->f3; goto _LL85;} else{ goto _LL86;} _LL86: if(( unsigned int) _temp82
> 4u?*(( int*) _temp82) == Cyc_Absyn_VarType: 0){ _LL137: _temp136=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_VarType_struct*) _temp82)->f1; goto _LL87;} else{ goto _LL88;}
_LL88: if( _temp82 ==( void*) Cyc_Absyn_VoidType){ goto _LL89;} else{ goto _LL90;}
_LL90: if(( unsigned int) _temp82 > 4u?*(( int*) _temp82) == Cyc_Absyn_IntType:
0){ _LL141: _temp140=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp82)->f1;
goto _LL139; _LL139: _temp138=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp82)->f2; goto _LL91;} else{ goto _LL92;} _LL92: if( _temp82 ==( void*) Cyc_Absyn_FloatType){
goto _LL93;} else{ goto _LL94;} _LL94: if( _temp82 ==( void*) Cyc_Absyn_DoubleType){
goto _LL95;} else{ goto _LL96;} _LL96: if(( unsigned int) _temp82 > 4u?*(( int*)
_temp82) == Cyc_Absyn_FnType: 0){ _LL143: _temp142=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp82)->f1; goto _LL97;} else{ goto _LL98;}
_LL98: if(( unsigned int) _temp82 > 4u?*(( int*) _temp82) == Cyc_Absyn_RgnHandleType:
0){ _LL145: _temp144=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp82)->f1;
goto _LL99;} else{ goto _LL100;} _LL100: if( _temp82 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL101;} else{ goto _LL102;} _LL102: if(( unsigned int) _temp82 > 4u?*((
int*) _temp82) == Cyc_Absyn_TunionType: 0){ _LL147: _temp146=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp82)->f1; goto _LL103;} else{ goto
_LL104;} _LL104: if(( unsigned int) _temp82 > 4u?*(( int*) _temp82) == Cyc_Absyn_TunionFieldType:
0){ _LL149: _temp148=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp82)->f1; _LL153: _temp152=( void*) _temp148.field_info; if(*(( int*)
_temp152) == Cyc_Absyn_KnownTunionfield){ _LL157: _temp156=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp152)->f1; goto _LL155; _LL155:
_temp154=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp152)->f2; goto _LL151;} else{ goto _LL106;} _LL151: _temp150=( struct Cyc_List_List*)
_temp148.targs; goto _LL105;} else{ goto _LL106;} _LL106: if(( unsigned int)
_temp82 > 4u?*(( int*) _temp82) == Cyc_Absyn_TunionFieldType: 0){ _LL159:
_temp158=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp82)->f1; _LL163: _temp162=( void*) _temp158.field_info; if(*(( int*)
_temp162) == Cyc_Absyn_UnknownTunionfield){ _LL165: _temp164=( struct Cyc_Absyn_UnknownTunionFieldInfo)((
struct Cyc_Absyn_UnknownTunionfield_struct*) _temp162)->f1; goto _LL161;} else{
goto _LL108;} _LL161: _temp160=( struct Cyc_List_List*) _temp158.targs; goto
_LL107;} else{ goto _LL108;} _LL108: if(( unsigned int) _temp82 > 4u?*(( int*)
_temp82) == Cyc_Absyn_StructType: 0){ _LL171: _temp170=( struct _tuple1*)((
struct Cyc_Absyn_StructType_struct*) _temp82)->f1; goto _LL169; _LL169: _temp168=(
struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*) _temp82)->f2; goto
_LL167; _LL167: _temp166=( struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*)
_temp82)->f3; if( _temp166 == 0){ goto _LL109;} else{ goto _LL110;}} else{ goto
_LL110;} _LL110: if(( unsigned int) _temp82 > 4u?*(( int*) _temp82) == Cyc_Absyn_UnionType:
0){ _LL177: _temp176=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp82)->f1; goto _LL175; _LL175: _temp174=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp82)->f2; goto _LL173; _LL173: _temp172=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp82)->f3;
if( _temp172 == 0){ goto _LL111;} else{ goto _LL112;}} else{ goto _LL112;}
_LL112: if(( unsigned int) _temp82 > 4u?*(( int*) _temp82) == Cyc_Absyn_StructType:
0){ _LL183: _temp182=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp82)->f1; goto _LL181; _LL181: _temp180=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp82)->f2; goto _LL179; _LL179: _temp178=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp82)->f3;
goto _LL113;} else{ goto _LL114;} _LL114: if(( unsigned int) _temp82 > 4u?*((
int*) _temp82) == Cyc_Absyn_UnionType: 0){ _LL189: _temp188=( struct _tuple1*)((
struct Cyc_Absyn_UnionType_struct*) _temp82)->f1; goto _LL187; _LL187: _temp186=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*) _temp82)->f2; goto
_LL185; _LL185: _temp184=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp82)->f3; goto _LL115;} else{ goto _LL116;} _LL116: if(( unsigned int)
_temp82 > 4u?*(( int*) _temp82) == Cyc_Absyn_EnumType: 0){ _LL193: _temp192=(
struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*) _temp82)->f1; goto _LL191;
_LL191: _temp190=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*)
_temp82)->f2; goto _LL117;} else{ goto _LL118;} _LL118: if(( unsigned int)
_temp82 > 4u?*(( int*) _temp82) == Cyc_Absyn_PointerType: 0){ _LL195: _temp194=(
struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp82)->f1;
goto _LL119;} else{ goto _LL120;} _LL120: if(( unsigned int) _temp82 > 4u?*((
int*) _temp82) == Cyc_Absyn_ArrayType: 0){ _LL201: _temp200=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp82)->f1; goto _LL199; _LL199: _temp198=(
struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*) _temp82)->f2; goto
_LL197; _LL197: _temp196=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp82)->f3; goto _LL121;} else{ goto _LL122;} _LL122: if(( unsigned int)
_temp82 > 4u?*(( int*) _temp82) == Cyc_Absyn_TupleType: 0){ _LL203: _temp202=(
struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp82)->f1; goto
_LL123;} else{ goto _LL124;} _LL124: if(( unsigned int) _temp82 > 4u?*(( int*)
_temp82) == Cyc_Absyn_TypedefType: 0){ _LL209: _temp208=( struct _tuple1*)((
struct Cyc_Absyn_TypedefType_struct*) _temp82)->f1; goto _LL207; _LL207:
_temp206=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp82)->f2; goto _LL205; _LL205: _temp204=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp82)->f3; goto _LL125;} else{ goto _LL126;} _LL126: if(( unsigned int)
_temp82 > 4u?*(( int*) _temp82) == Cyc_Absyn_AccessEff: 0){ _LL211: _temp210=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp82)->f1; goto _LL127;} else{
goto _LL128;} _LL128: if(( unsigned int) _temp82 > 4u?*(( int*) _temp82) == Cyc_Absyn_JoinEff:
0){ _LL213: _temp212=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*)
_temp82)->f1; goto _LL129;} else{ goto _LL83;} _LL85: return _temp134; _LL87:
return(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp136->kind);
_LL89: return( void*) Cyc_Absyn_MemKind; _LL91: return _temp138 ==( void*) Cyc_Absyn_B4?(
void*) Cyc_Absyn_BoxKind:( void*) Cyc_Absyn_MemKind; _LL93: return( void*) Cyc_Absyn_MemKind;
_LL95: return( void*) Cyc_Absyn_MemKind; _LL97: return( void*) Cyc_Absyn_MemKind;
_LL99: return( void*) Cyc_Absyn_BoxKind; _LL101: return( void*) Cyc_Absyn_RgnKind;
_LL103: return( void*) Cyc_Absyn_BoxKind; _LL105: if( _temp154->typs == 0){
return( void*) Cyc_Absyn_BoxKind;} else{ return( void*) Cyc_Absyn_MemKind;}
_LL107: return(( void*(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp214=( char*)"typ_kind: Unresolved TunionFieldType";
struct _tagged_string _temp215; _temp215.curr= _temp214; _temp215.base= _temp214;
_temp215.last_plus_one= _temp214 + 37; _temp215;})); _LL109: return( void*) Cyc_Absyn_AnyKind;
_LL111: return( void*) Cyc_Absyn_AnyKind; _LL113: return( void*) Cyc_Absyn_MemKind;
_LL115: return( void*) Cyc_Absyn_MemKind; _LL117: return( void*) Cyc_Absyn_BoxKind;
_LL119: { void* _temp216=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp194.bounds))->v; void* _temp226; void*
_temp228; struct Cyc_Absyn_Exp* _temp230; struct Cyc_Absyn_Conref* _temp232;
_LL218: if(( unsigned int) _temp216 > 1u?*(( int*) _temp216) == Cyc_Absyn_Eq_constr:
0){ _LL227: _temp226=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp216)->f1;
if( _temp226 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL219;} else{ goto _LL220;}}
else{ goto _LL220;} _LL220: if(( unsigned int) _temp216 > 1u?*(( int*) _temp216)
== Cyc_Absyn_Eq_constr: 0){ _LL229: _temp228=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp216)->f1; if(( unsigned int) _temp228 > 1u?*(( int*) _temp228) == Cyc_Absyn_Upper_b:
0){ _LL231: _temp230=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp228)->f1; goto _LL221;} else{ goto _LL222;}} else{ goto _LL222;} _LL222:
if( _temp216 ==( void*) Cyc_Absyn_No_constr){ goto _LL223;} else{ goto _LL224;}
_LL224: if(( unsigned int) _temp216 > 1u?*(( int*) _temp216) == Cyc_Absyn_Forward_constr:
0){ _LL233: _temp232=( struct Cyc_Absyn_Conref*)(( struct Cyc_Absyn_Forward_constr_struct*)
_temp216)->f1; goto _LL225;} else{ goto _LL217;} _LL219: return( void*) Cyc_Absyn_MemKind;
_LL221: return( void*) Cyc_Absyn_BoxKind; _LL223: return( void*) Cyc_Absyn_MemKind;
_LL225: return(( void*(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp234=( char*)"typ_kind: forward constraint in ptr bounds";
struct _tagged_string _temp235; _temp235.curr= _temp234; _temp235.base= _temp234;
_temp235.last_plus_one= _temp234 + 43; _temp235;})); _LL217:;} _LL121: return(
void*) Cyc_Absyn_MemKind; _LL123: return( void*) Cyc_Absyn_MemKind; _LL125:
return(( void*(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({ struct
_tagged_string _temp236= Cyc_Absynpp_typ2string( t); xprintf("typ_kind: typedef found: %.*s",
_temp236.last_plus_one - _temp236.curr, _temp236.curr);})); _LL127: return( void*)
Cyc_Absyn_EffKind; _LL129: return( void*) Cyc_Absyn_EffKind; _LL83:;} char Cyc_Tcutil_Unify[
10u]="\000\000\000\000Unify"; int Cyc_Tcutil_unify( void* t1, void* t2){ struct
_handler_cons _temp237; _push_handler(& _temp237);{ void* _temp238=( void*)
setjmp( _temp237.handler); if( ! _temp238){ Cyc_Tcutil_unify_it( t1, t2);{ int
_temp239= 1; _npop_handler( 0u); return _temp239;}; _pop_handler();} else{ void*
_temp241= _temp238; _LL243: if( _temp241 == Cyc_Tcutil_Unify){ goto _LL244;}
else{ goto _LL245;} _LL245: goto _LL246; _LL244: return 0; _LL246:( void) _throw(
_temp241); _LL242:;}}} static void Cyc_Tcutil_occurslist( void* evar, struct Cyc_List_List*
ts); static void Cyc_Tcutil_occurs( void* evar, void* t){ void* _temp247= Cyc_Tcutil_compress(
t); int _temp275; struct Cyc_Core_Opt* _temp277; void* _temp279; struct Cyc_Absyn_PtrInfo
_temp281; struct Cyc_Absyn_Exp* _temp283; struct Cyc_Absyn_Tqual _temp285; void*
_temp287; struct Cyc_Absyn_FnInfo _temp289; struct Cyc_List_List* _temp291; int
_temp293; struct Cyc_List_List* _temp295; void* _temp297; struct Cyc_Core_Opt*
_temp299; struct Cyc_List_List* _temp301; struct Cyc_List_List* _temp303; void*
_temp305; struct Cyc_Absyn_TunionInfo _temp307; void* _temp309; struct Cyc_List_List*
_temp311; void* _temp313; struct Cyc_Core_Opt* _temp315; struct Cyc_List_List*
_temp317; struct _tuple1* _temp319; struct Cyc_Absyn_TunionFieldInfo _temp321;
struct Cyc_List_List* _temp323; void* _temp325; struct Cyc_Absyn_Structdecl**
_temp327; struct Cyc_List_List* _temp329; struct _tuple1* _temp331; void*
_temp333; struct Cyc_List_List* _temp335; _LL249: if(( unsigned int) _temp247 >
4u?*(( int*) _temp247) == Cyc_Absyn_Evar: 0){ _LL280: _temp279=( void*)(( struct
Cyc_Absyn_Evar_struct*) _temp247)->f1; goto _LL278; _LL278: _temp277=( struct
Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp247)->f2; goto _LL276;
_LL276: _temp275=( int)(( struct Cyc_Absyn_Evar_struct*) _temp247)->f3; goto
_LL250;} else{ goto _LL251;} _LL251: if(( unsigned int) _temp247 > 4u?*(( int*)
_temp247) == Cyc_Absyn_PointerType: 0){ _LL282: _temp281=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp247)->f1; goto _LL252;} else{ goto
_LL253;} _LL253: if(( unsigned int) _temp247 > 4u?*(( int*) _temp247) == Cyc_Absyn_ArrayType:
0){ _LL288: _temp287=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp247)->f1;
goto _LL286; _LL286: _temp285=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp247)->f2; goto _LL284; _LL284: _temp283=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp247)->f3; goto _LL254;} else{ goto _LL255;}
_LL255: if(( unsigned int) _temp247 > 4u?*(( int*) _temp247) == Cyc_Absyn_FnType:
0){ _LL290: _temp289=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp247)->f1; _LL302: _temp301=( struct Cyc_List_List*) _temp289.tvars; goto
_LL300; _LL300: _temp299=( struct Cyc_Core_Opt*) _temp289.effect; goto _LL298;
_LL298: _temp297=( void*) _temp289.ret_typ; goto _LL296; _LL296: _temp295=(
struct Cyc_List_List*) _temp289.args; goto _LL294; _LL294: _temp293=( int)
_temp289.varargs; goto _LL292; _LL292: _temp291=( struct Cyc_List_List*)
_temp289.attributes; goto _LL256;} else{ goto _LL257;} _LL257: if(( unsigned int)
_temp247 > 4u?*(( int*) _temp247) == Cyc_Absyn_TupleType: 0){ _LL304: _temp303=(
struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp247)->f1; goto
_LL258;} else{ goto _LL259;} _LL259: if(( unsigned int) _temp247 > 4u?*(( int*)
_temp247) == Cyc_Absyn_RgnHandleType: 0){ _LL306: _temp305=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp247)->f1; goto _LL260;} else{ goto _LL261;} _LL261: if(( unsigned int)
_temp247 > 4u?*(( int*) _temp247) == Cyc_Absyn_TunionType: 0){ _LL308: _temp307=(
struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*) _temp247)->f1;
_LL314: _temp313=( void*) _temp307.tunion_info; goto _LL312; _LL312: _temp311=(
struct Cyc_List_List*) _temp307.targs; goto _LL310; _LL310: _temp309=( void*)
_temp307.rgn; goto _LL262;} else{ goto _LL263;} _LL263: if(( unsigned int)
_temp247 > 4u?*(( int*) _temp247) == Cyc_Absyn_TypedefType: 0){ _LL320: _temp319=(
struct _tuple1*)(( struct Cyc_Absyn_TypedefType_struct*) _temp247)->f1; goto
_LL318; _LL318: _temp317=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp247)->f2; goto _LL316; _LL316: _temp315=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_TypedefType_struct*) _temp247)->f3; goto _LL264;} else{ goto _LL265;}
_LL265: if(( unsigned int) _temp247 > 4u?*(( int*) _temp247) == Cyc_Absyn_TunionFieldType:
0){ _LL322: _temp321=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp247)->f1; _LL326: _temp325=( void*) _temp321.field_info; goto _LL324;
_LL324: _temp323=( struct Cyc_List_List*) _temp321.targs; goto _LL266;} else{
goto _LL267;} _LL267: if(( unsigned int) _temp247 > 4u?*(( int*) _temp247) ==
Cyc_Absyn_StructType: 0){ _LL332: _temp331=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp247)->f1; goto _LL330; _LL330: _temp329=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp247)->f2; goto _LL328; _LL328: _temp327=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp247)->f3;
goto _LL268;} else{ goto _LL269;} _LL269: if(( unsigned int) _temp247 > 4u?*((
int*) _temp247) == Cyc_Absyn_AccessEff: 0){ _LL334: _temp333=( void*)(( struct
Cyc_Absyn_AccessEff_struct*) _temp247)->f1; goto _LL270;} else{ goto _LL271;}
_LL271: if(( unsigned int) _temp247 > 4u?*(( int*) _temp247) == Cyc_Absyn_JoinEff:
0){ _LL336: _temp335=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*)
_temp247)->f1; goto _LL272;} else{ goto _LL273;} _LL273: goto _LL274; _LL250:
if( t == evar){( void) _throw(( void*) Cyc_Tcutil_Unify);} else{ if( _temp277 !=
0){ Cyc_Tcutil_occurs( evar,( void*)({ struct Cyc_Core_Opt* _temp337= _temp277;
if( _temp337 == 0){ _throw( Null_Exception);} _temp337->v;}));}} goto _LL248;
_LL252: Cyc_Tcutil_occurs( evar,( void*) _temp281.elt_typ); Cyc_Tcutil_occurs(
evar,( void*) _temp281.rgn_typ); goto _LL248; _LL254: Cyc_Tcutil_occurs( evar,
_temp287); goto _LL248; _LL256: if( _temp299 != 0){ Cyc_Tcutil_occurs( evar,(
void*)({ struct Cyc_Core_Opt* _temp338= _temp299; if( _temp338 == 0){ _throw(
Null_Exception);} _temp338->v;}));} Cyc_Tcutil_occurs( evar, _temp297); for( 0;
_temp295 != 0; _temp295=({ struct Cyc_List_List* _temp339= _temp295; if(
_temp339 == 0){ _throw( Null_Exception);} _temp339->tl;})){ Cyc_Tcutil_occurs(
evar,(*(( struct _tuple2*)({ struct Cyc_List_List* _temp340= _temp295; if(
_temp340 == 0){ _throw( Null_Exception);} _temp340->hd;}))).f3);} goto _LL248;
_LL258: for( 0; _temp303 != 0; _temp303=({ struct Cyc_List_List* _temp341=
_temp303; if( _temp341 == 0){ _throw( Null_Exception);} _temp341->tl;})){ Cyc_Tcutil_occurs(
evar,(*(( struct _tuple4*)({ struct Cyc_List_List* _temp342= _temp303; if(
_temp342 == 0){ _throw( Null_Exception);} _temp342->hd;}))).f2);} goto _LL248;
_LL260: Cyc_Tcutil_occurs( evar, _temp305); goto _LL248; _LL262: Cyc_Tcutil_occurs(
evar, _temp309); Cyc_Tcutil_occurslist( evar, _temp311); goto _LL248; _LL264:
_temp323= _temp317; goto _LL266; _LL266: _temp329= _temp323; goto _LL268; _LL268:
Cyc_Tcutil_occurslist( evar, _temp329); goto _LL248; _LL270: Cyc_Tcutil_occurs(
evar, _temp333); goto _LL248; _LL272: Cyc_Tcutil_occurslist( evar, _temp335);
goto _LL248; _LL274: goto _LL248; _LL248:;} static void Cyc_Tcutil_occurslist(
void* evar, struct Cyc_List_List* ts){ for( 0; ts != 0; ts=({ struct Cyc_List_List*
_temp343= ts; if( _temp343 == 0){ _throw( Null_Exception);} _temp343->tl;})){
Cyc_Tcutil_occurs( evar,( void*)({ struct Cyc_List_List* _temp344= ts; if(
_temp344 == 0){ _throw( Null_Exception);} _temp344->hd;}));}} static void Cyc_Tcutil_unify_list(
struct Cyc_List_List* t1, struct Cyc_List_List* t2){ struct _handler_cons
_temp345; _push_handler(& _temp345);{ void* _temp346=( void*) setjmp( _temp345.handler);
if( ! _temp346){(( void(*)( void(* f)( void*, void*), struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_iter2)( Cyc_Tcutil_unify_it, t1, t2);;
_pop_handler();} else{ void* _temp348= _temp346; _LL350: if( _temp348 == Cyc_List_List_mismatch){
goto _LL351;} else{ goto _LL352;} _LL352: goto _LL353; _LL351:( void) _throw((
void*) Cyc_Tcutil_Unify); _LL353:( void) _throw( _temp348); _LL349:;}}} static
void Cyc_Tcutil_unify_tqual( struct Cyc_Absyn_Tqual tq1, struct Cyc_Absyn_Tqual
tq2){ if(( tq1.q_const != tq2.q_const? 1: tq1.q_volatile != tq2.q_volatile)? 1:
tq1.q_restrict != tq2.q_restrict){( void) _throw(( void*) Cyc_Tcutil_Unify);}}
int Cyc_Tcutil_equal_tqual( struct Cyc_Absyn_Tqual tq1, struct Cyc_Absyn_Tqual
tq2){ return( tq1.q_const == tq2.q_const? tq1.q_volatile == tq2.q_volatile: 0)?
tq1.q_restrict == tq2.q_restrict: 0;} static void Cyc_Tcutil_unify_it_conrefs(
int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y){ x=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
x); y=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
y); if( x == y){ return;}{ void* _temp354=( void*) x->v; struct Cyc_Absyn_Conref*
_temp362; void* _temp364; _LL356: if( _temp354 ==( void*) Cyc_Absyn_No_constr){
goto _LL357;} else{ goto _LL358;} _LL358: if(( unsigned int) _temp354 > 1u?*((
int*) _temp354) == Cyc_Absyn_Forward_constr: 0){ _LL363: _temp362=( struct Cyc_Absyn_Conref*)((
struct Cyc_Absyn_Forward_constr_struct*) _temp354)->f1; goto _LL359;} else{ goto
_LL360;} _LL360: if(( unsigned int) _temp354 > 1u?*(( int*) _temp354) == Cyc_Absyn_Eq_constr:
0){ _LL365: _temp364=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp354)->f1;
goto _LL361;} else{ goto _LL355;} _LL357:( void*)( x->v=( void*)(( void*)({
struct Cyc_Absyn_Forward_constr_struct* _temp366=( struct Cyc_Absyn_Forward_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp366[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp367; _temp367.tag= Cyc_Absyn_Forward_constr;
_temp367.f1= y; _temp367;}); _temp366;}))); return; _LL359:( void) _throw(((
void*(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp368=( char*)"unify_conref: forward after compress"; struct
_tagged_string _temp369; _temp369.curr= _temp368; _temp369.base= _temp368;
_temp369.last_plus_one= _temp368 + 37; _temp369;}))); _LL361: { void* _temp370=(
void*) y->v; struct Cyc_Absyn_Conref* _temp378; void* _temp380; _LL372: if(
_temp370 ==( void*) Cyc_Absyn_No_constr){ goto _LL373;} else{ goto _LL374;}
_LL374: if(( unsigned int) _temp370 > 1u?*(( int*) _temp370) == Cyc_Absyn_Forward_constr:
0){ _LL379: _temp378=( struct Cyc_Absyn_Conref*)(( struct Cyc_Absyn_Forward_constr_struct*)
_temp370)->f1; goto _LL375;} else{ goto _LL376;} _LL376: if(( unsigned int)
_temp370 > 1u?*(( int*) _temp370) == Cyc_Absyn_Eq_constr: 0){ _LL381: _temp380=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp370)->f1; goto _LL377;} else{
goto _LL371;} _LL373:( void*)( y->v=( void*)(( void*) x->v)); return; _LL375:(
void) _throw((( void*(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ char* _temp382=( char*)"unify_conref: forward after compress(2)";
struct _tagged_string _temp383; _temp383.curr= _temp382; _temp383.base= _temp382;
_temp383.last_plus_one= _temp382 + 40; _temp383;}))); _LL377: if( cmp( _temp364,
_temp380) != 0){( void) _throw(( void*) Cyc_Tcutil_Unify);} return; _LL371:;}
_LL355:;}} static int Cyc_Tcutil_unify_conrefs( int(* cmp)( void*, void*),
struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y){ struct _handler_cons
_temp384; _push_handler(& _temp384);{ void* _temp385=( void*) setjmp( _temp384.handler);
if( ! _temp385){(( void(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref*
x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_it_conrefs)( cmp, x, y);{ int
_temp386= 1; _npop_handler( 0u); return _temp386;}; _pop_handler();} else{ void*
_temp388= _temp385; _LL390: if( _temp388 == Cyc_Tcutil_Unify){ goto _LL391;}
else{ goto _LL392;} _LL392: goto _LL393; _LL391: return 0; _LL393:( void) _throw(
_temp388); _LL389:;}}} static int Cyc_Tcutil_boundscmp( void* b1, void* b2){
struct _tuple7 _temp395=({ struct _tuple7 _temp394; _temp394.f1= b1; _temp394.f2=
b2; _temp394;}); void* _temp405; void* _temp407; void* _temp409; void* _temp411;
void* _temp413; void* _temp415; void* _temp417; struct Cyc_Absyn_Exp* _temp419;
void* _temp421; struct Cyc_Absyn_Exp* _temp423; _LL397: _LL408: _temp407=
_temp395.f1; if( _temp407 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL406;} else{
goto _LL399;} _LL406: _temp405= _temp395.f2; if( _temp405 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL398;} else{ goto _LL399;} _LL399: _LL412: _temp411= _temp395.f1; if(
_temp411 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL410;} else{ goto _LL401;}
_LL410: _temp409= _temp395.f2; goto _LL400; _LL401: _LL416: _temp415= _temp395.f1;
goto _LL414; _LL414: _temp413= _temp395.f2; if( _temp413 ==( void*) Cyc_Absyn_Unknown_b){
goto _LL402;} else{ goto _LL403;} _LL403: _LL422: _temp421= _temp395.f1; if((
unsigned int) _temp421 > 1u?*(( int*) _temp421) == Cyc_Absyn_Upper_b: 0){ _LL424:
_temp423=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp421)->f1;
goto _LL418;} else{ goto _LL396;} _LL418: _temp417= _temp395.f2; if((
unsigned int) _temp417 > 1u?*(( int*) _temp417) == Cyc_Absyn_Upper_b: 0){ _LL420:
_temp419=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp417)->f1;
goto _LL404;} else{ goto _LL396;} _LL398: return 0; _LL400: return - 1; _LL402:
return 1; _LL404: { int i1=( int) Cyc_Evexp_eval_const_uint_exp( _temp423); int
i2=( int) Cyc_Evexp_eval_const_uint_exp( _temp419); if( i1 == i2){ return 0;}
if( i1 < i2){ return - 1;} return 1;} _LL396:;} static int Cyc_Tcutil_equal_att(
void* a1, void* a2){ if( a1 == a2){ return 1;}{ struct _tuple7 _temp426=({
struct _tuple7 _temp425; _temp425.f1= a1; _temp425.f2= a2; _temp425;}); void*
_temp436; int _temp438; void* _temp440; int _temp442; void* _temp444; int
_temp446; void* _temp448; int _temp450; void* _temp452; struct _tagged_string
_temp454; void* _temp456; struct _tagged_string _temp458; _LL428: _LL441:
_temp440= _temp426.f1; if(( unsigned int) _temp440 > 15u?*(( int*) _temp440) ==
Cyc_Absyn_Regparm_att: 0){ _LL443: _temp442=( int)(( struct Cyc_Absyn_Regparm_att_struct*)
_temp440)->f1; goto _LL437;} else{ goto _LL430;} _LL437: _temp436= _temp426.f2;
if(( unsigned int) _temp436 > 15u?*(( int*) _temp436) == Cyc_Absyn_Regparm_att:
0){ _LL439: _temp438=( int)(( struct Cyc_Absyn_Regparm_att_struct*) _temp436)->f1;
goto _LL429;} else{ goto _LL430;} _LL430: _LL449: _temp448= _temp426.f1; if((
unsigned int) _temp448 > 15u?*(( int*) _temp448) == Cyc_Absyn_Aligned_att: 0){
_LL451: _temp450=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp448)->f1;
goto _LL445;} else{ goto _LL432;} _LL445: _temp444= _temp426.f2; if((
unsigned int) _temp444 > 15u?*(( int*) _temp444) == Cyc_Absyn_Aligned_att: 0){
_LL447: _temp446=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp444)->f1;
goto _LL431;} else{ goto _LL432;} _LL432: _LL457: _temp456= _temp426.f1; if((
unsigned int) _temp456 > 15u?*(( int*) _temp456) == Cyc_Absyn_Section_att: 0){
_LL459: _temp458=( struct _tagged_string)(( struct Cyc_Absyn_Section_att_struct*)
_temp456)->f1; goto _LL453;} else{ goto _LL434;} _LL453: _temp452= _temp426.f2;
if(( unsigned int) _temp452 > 15u?*(( int*) _temp452) == Cyc_Absyn_Section_att:
0){ _LL455: _temp454=( struct _tagged_string)(( struct Cyc_Absyn_Section_att_struct*)
_temp452)->f1; goto _LL433;} else{ goto _LL434;} _LL434: goto _LL435; _LL429:
_temp450= _temp442; _temp446= _temp438; goto _LL431; _LL431: return _temp450 ==
_temp446; _LL433: return Cyc_String_strcmp( _temp458, _temp454) == 0; _LL435:
return 0; _LL427:;}} int Cyc_Tcutil_same_atts( struct Cyc_List_List* a1, struct
Cyc_List_List* a2){{ struct Cyc_List_List* a= a1; for( 0; a != 0; a=({ struct
Cyc_List_List* _temp460= a; if( _temp460 == 0){ _throw( Null_Exception);}
_temp460->tl;})){ if( !(( int(*)( int(* pred)( void*, void*), void* env, struct
Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcutil_equal_att,( void*)({ struct
Cyc_List_List* _temp461= a; if( _temp461 == 0){ _throw( Null_Exception);}
_temp461->hd;}), a2)){ return 0;}}}{ struct Cyc_List_List* a= a2; for( 0; a != 0;
a=({ struct Cyc_List_List* _temp462= a; if( _temp462 == 0){ _throw(
Null_Exception);} _temp462->tl;})){ if( !(( int(*)( int(* pred)( void*, void*),
void* env, struct Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcutil_equal_att,(
void*)({ struct Cyc_List_List* _temp463= a; if( _temp463 == 0){ _throw(
Null_Exception);} _temp463->hd;}), a1)){ return 0;}}} return 1;} static void Cyc_Tcutil_normalize_effect(
void* e){ e= Cyc_Tcutil_compress( e);{ void* _temp464= e; struct Cyc_List_List*
_temp470; struct Cyc_List_List** _temp472; _LL466: if(( unsigned int) _temp464 >
4u?*(( int*) _temp464) == Cyc_Absyn_JoinEff: 0){ _LL471: _temp470=( struct Cyc_List_List*)((
struct Cyc_Absyn_JoinEff_struct*) _temp464)->f1; _temp472=&(( struct Cyc_Absyn_JoinEff_struct*)
_temp464)->f1; goto _LL467;} else{ goto _LL468;} _LL468: goto _LL469; _LL467: {
int nested_join= 0;{ struct Cyc_List_List* effs=* _temp472; for( 0; effs != 0;
effs=({ struct Cyc_List_List* _temp473= effs; if( _temp473 == 0){ _throw(
Null_Exception);} _temp473->tl;})){ void* eff=( void*)({ struct Cyc_List_List*
_temp482= effs; if( _temp482 == 0){ _throw( Null_Exception);} _temp482->hd;});
Cyc_Tcutil_normalize_effect( eff);{ void* _temp474= Cyc_Tcutil_compress( eff);
struct Cyc_List_List* _temp480; _LL476: if(( unsigned int) _temp474 > 4u?*(( int*)
_temp474) == Cyc_Absyn_JoinEff: 0){ _LL481: _temp480=( struct Cyc_List_List*)((
struct Cyc_Absyn_JoinEff_struct*) _temp474)->f1; goto _LL477;} else{ goto _LL478;}
_LL478: goto _LL479; _LL477: nested_join= 1; goto _LL475; _LL479: goto _LL475;
_LL475:;}}} if( ! nested_join){ return;}{ struct Cyc_List_List* effects= 0;{
struct Cyc_List_List* effs=* _temp472; for( 0; effs != 0; effs=({ struct Cyc_List_List*
_temp483= effs; if( _temp483 == 0){ _throw( Null_Exception);} _temp483->tl;})){
void* _temp485= Cyc_Tcutil_compress(( void*)({ struct Cyc_List_List* _temp484=
effs; if( _temp484 == 0){ _throw( Null_Exception);} _temp484->hd;})); struct Cyc_List_List*
_temp491; _LL487: if(( unsigned int) _temp485 > 4u?*(( int*) _temp485) == Cyc_Absyn_JoinEff:
0){ _LL492: _temp491=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*)
_temp485)->f1; goto _LL488;} else{ goto _LL489;} _LL489: goto _LL490; _LL488:
effects=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_revappend)( _temp491, effects); goto _LL486; _LL490: effects=({
struct Cyc_List_List* _temp493=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp493->hd=( void*) _temp485; _temp493->tl= effects;
_temp493;}); goto _LL486; _LL486:;}}* _temp472=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( effects); goto _LL465;}} _LL469:
goto _LL465; _LL465:;}} struct _tuple8{ void* f1; struct Cyc_List_List* f2; } ;
static struct _tuple8* Cyc_Tcutil_get_effect_evar( void* t){ void* _temp494= Cyc_Tcutil_compress(
t); struct Cyc_List_List* _temp502; struct Cyc_List_List _temp504; struct Cyc_List_List*
_temp505; void* _temp507; int _temp509; struct Cyc_Core_Opt* _temp511; struct
Cyc_Core_Opt** _temp513; void* _temp514; void** _temp516; _LL496: if((
unsigned int) _temp494 > 4u?*(( int*) _temp494) == Cyc_Absyn_JoinEff: 0){ _LL503:
_temp502=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp494)->f1;
if( _temp502 == 0){ goto _LL498;} else{ _temp504=* _temp502; _LL508: _temp507=(
void*) _temp504.hd; goto _LL506; _LL506: _temp505=( struct Cyc_List_List*)
_temp504.tl; goto _LL497;}} else{ goto _LL498;} _LL498: if(( unsigned int)
_temp494 > 4u?*(( int*) _temp494) == Cyc_Absyn_Evar: 0){ _LL515: _temp514=( void*)((
struct Cyc_Absyn_Evar_struct*) _temp494)->f1; _temp516=&(( struct Cyc_Absyn_Evar_struct*)
_temp494)->f1; goto _LL512; _LL512: _temp511=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Evar_struct*) _temp494)->f2; _temp513=&(( struct Cyc_Absyn_Evar_struct*)
_temp494)->f2; goto _LL510; _LL510: _temp509=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp494)->f3; goto _LL499;} else{ goto _LL500;} _LL500: goto _LL501; _LL497: {
void* _temp517= Cyc_Tcutil_compress( _temp507); int _temp523; struct Cyc_Core_Opt*
_temp525; void* _temp527; _LL519: if(( unsigned int) _temp517 > 4u?*(( int*)
_temp517) == Cyc_Absyn_Evar: 0){ _LL528: _temp527=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp517)->f1; goto _LL526; _LL526: _temp525=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Evar_struct*) _temp517)->f2; goto _LL524; _LL524: _temp523=( int)((
struct Cyc_Absyn_Evar_struct*) _temp517)->f3; goto _LL520;} else{ goto _LL521;}
_LL521: goto _LL522; _LL520: return({ struct _tuple8* _temp529=( struct _tuple8*)
GC_malloc( sizeof( struct _tuple8)); _temp529->f1= _temp507; _temp529->f2=
_temp505; _temp529;}); _LL522: return 0; _LL518:;} _LL499: if(* _temp516 !=(
void*) Cyc_Absyn_EffKind){(( void(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp530=( char*)"effect evar has wrong kind";
struct _tagged_string _temp531; _temp531.curr= _temp530; _temp531.base= _temp530;
_temp531.last_plus_one= _temp530 + 27; _temp531;}));} return({ struct _tuple8*
_temp532=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8)); _temp532->f1= t;
_temp532->f2= 0; _temp532;}); _LL501: return 0; _LL495:;} int Cyc_Tcutil_region_in_effect(
int constrain, void* r, void* e){ r= Cyc_Tcutil_compress( r); if( r ==( void*)
Cyc_Absyn_HeapRgn){ return 1;}{ void* _temp533= Cyc_Tcutil_compress( e); void*
_temp543; struct Cyc_List_List* _temp545; int _temp547; struct Cyc_Core_Opt*
_temp549; struct Cyc_Core_Opt** _temp551; void* _temp552; void** _temp554;
_LL535: if(( unsigned int) _temp533 > 4u?*(( int*) _temp533) == Cyc_Absyn_AccessEff:
0){ _LL544: _temp543=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp533)->f1;
goto _LL536;} else{ goto _LL537;} _LL537: if(( unsigned int) _temp533 > 4u?*((
int*) _temp533) == Cyc_Absyn_JoinEff: 0){ _LL546: _temp545=( struct Cyc_List_List*)((
struct Cyc_Absyn_JoinEff_struct*) _temp533)->f1; goto _LL538;} else{ goto _LL539;}
_LL539: if(( unsigned int) _temp533 > 4u?*(( int*) _temp533) == Cyc_Absyn_Evar:
0){ _LL553: _temp552=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp533)->f1;
_temp554=&(( struct Cyc_Absyn_Evar_struct*) _temp533)->f1; goto _LL550; _LL550:
_temp549=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp533)->f2;
_temp551=&(( struct Cyc_Absyn_Evar_struct*) _temp533)->f2; goto _LL548; _LL548:
_temp547=( int)(( struct Cyc_Absyn_Evar_struct*) _temp533)->f3; goto _LL540;}
else{ goto _LL541;} _LL541: goto _LL542; _LL536: if( constrain){ return Cyc_Tcutil_unify(
r, _temp543);} Cyc_Tcutil_compress( _temp543); if( r == _temp543){ return 1;}{
struct _tuple7 _temp556=({ struct _tuple7 _temp555; _temp555.f1= r; _temp555.f2=
_temp543; _temp555;}); void* _temp562; struct Cyc_Absyn_Tvar* _temp564; void*
_temp566; struct Cyc_Absyn_Tvar* _temp568; _LL558: _LL567: _temp566= _temp556.f1;
if(( unsigned int) _temp566 > 4u?*(( int*) _temp566) == Cyc_Absyn_VarType: 0){
_LL569: _temp568=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp566)->f1; goto _LL563;} else{ goto _LL560;} _LL563: _temp562= _temp556.f2;
if(( unsigned int) _temp562 > 4u?*(( int*) _temp562) == Cyc_Absyn_VarType: 0){
_LL565: _temp564=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp562)->f1; goto _LL559;} else{ goto _LL560;} _LL560: goto _LL561; _LL559:
return Cyc_Absyn_tvar_cmp( _temp568, _temp564) == 0; _LL561: return 0; _LL557:;}
_LL538: for( 0; _temp545 != 0; _temp545=({ struct Cyc_List_List* _temp570=
_temp545; if( _temp570 == 0){ _throw( Null_Exception);} _temp570->tl;})){ if(
Cyc_Tcutil_region_in_effect( constrain, r,( void*)({ struct Cyc_List_List*
_temp571= _temp545; if( _temp571 == 0){ _throw( Null_Exception);} _temp571->hd;}))){
return 1;}} return 0; _LL540: if(* _temp554 !=( void*) Cyc_Absyn_EffKind){((
void(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp572=( char*)"effect evar has wrong kind"; struct _tagged_string
_temp573; _temp573.curr= _temp572; _temp573.base= _temp572; _temp573.last_plus_one=
_temp572 + 27; _temp573;}));} if( ! constrain){ return 0;}{ void* ev= Cyc_Absyn_new_evar((
void*) Cyc_Absyn_EffKind); struct Cyc_Absyn_JoinEff_struct* new_typ=({ struct
Cyc_Absyn_JoinEff_struct* _temp575=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp575[ 0]=({ struct Cyc_Absyn_JoinEff_struct
_temp576; _temp576.tag= Cyc_Absyn_JoinEff; _temp576.f1=({ struct Cyc_List_List*
_temp577=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp577->hd=( void*) ev; _temp577->tl=({ struct Cyc_List_List* _temp578=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp578->hd=(
void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp579=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp579[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp580; _temp580.tag= Cyc_Absyn_AccessEff; _temp580.f1=(
void*) r; _temp580;}); _temp579;})); _temp578->tl= 0; _temp578;}); _temp577;});
_temp576;}); _temp575;});* _temp551=({ struct Cyc_Core_Opt* _temp574=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp574->v=( void*)((
void*) new_typ); _temp574;}); return 1;} _LL542: return 0; _LL534:;}} static int
Cyc_Tcutil_variable_in_effect( int constrain, struct Cyc_Absyn_Tvar* v, void* e){
e= Cyc_Tcutil_compress( e);{ void* _temp581= e; struct Cyc_Absyn_Tvar* _temp591;
struct Cyc_List_List* _temp593; int _temp595; struct Cyc_Core_Opt* _temp597;
struct Cyc_Core_Opt** _temp599; void* _temp600; void** _temp602; _LL583: if((
unsigned int) _temp581 > 4u?*(( int*) _temp581) == Cyc_Absyn_VarType: 0){ _LL592:
_temp591=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*) _temp581)->f1;
goto _LL584;} else{ goto _LL585;} _LL585: if(( unsigned int) _temp581 > 4u?*((
int*) _temp581) == Cyc_Absyn_JoinEff: 0){ _LL594: _temp593=( struct Cyc_List_List*)((
struct Cyc_Absyn_JoinEff_struct*) _temp581)->f1; goto _LL586;} else{ goto _LL587;}
_LL587: if(( unsigned int) _temp581 > 4u?*(( int*) _temp581) == Cyc_Absyn_Evar:
0){ _LL601: _temp600=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp581)->f1;
_temp602=&(( struct Cyc_Absyn_Evar_struct*) _temp581)->f1; goto _LL598; _LL598:
_temp597=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp581)->f2;
_temp599=&(( struct Cyc_Absyn_Evar_struct*) _temp581)->f2; goto _LL596; _LL596:
_temp595=( int)(( struct Cyc_Absyn_Evar_struct*) _temp581)->f3; goto _LL588;}
else{ goto _LL589;} _LL589: goto _LL590; _LL584: return Cyc_Absyn_tvar_cmp( v,
_temp591) == 0; _LL586: for( 0; _temp593 != 0; _temp593=({ struct Cyc_List_List*
_temp603= _temp593; if( _temp603 == 0){ _throw( Null_Exception);} _temp603->tl;})){
if( Cyc_Tcutil_variable_in_effect( constrain, v,( void*)({ struct Cyc_List_List*
_temp604= _temp593; if( _temp604 == 0){ _throw( Null_Exception);} _temp604->hd;}))){
return 1;}} return 0; _LL588: if(* _temp602 !=( void*) Cyc_Absyn_EffKind){((
void(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp605=( char*)"effect evar has wrong kind"; struct _tagged_string
_temp606; _temp606.curr= _temp605; _temp606.base= _temp605; _temp606.last_plus_one=
_temp605 + 27; _temp606;}));}{ void* ev= Cyc_Absyn_new_evar(( void*) Cyc_Absyn_EffKind);
struct Cyc_Absyn_JoinEff_struct* new_typ=({ struct Cyc_Absyn_JoinEff_struct*
_temp608=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp608[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp609; _temp609.tag= Cyc_Absyn_JoinEff;
_temp609.f1=({ struct Cyc_List_List* _temp610=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp610->hd=( void*) ev; _temp610->tl=({ struct
Cyc_List_List* _temp611=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp611->hd=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp612=(
struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp612[ 0]=({ struct Cyc_Absyn_VarType_struct _temp613; _temp613.tag= Cyc_Absyn_VarType;
_temp613.f1= v; _temp613;}); _temp612;})); _temp611->tl= 0; _temp611;});
_temp610;}); _temp609;}); _temp608;});* _temp599=({ struct Cyc_Core_Opt*
_temp607=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp607->v=( void*)(( void*) new_typ); _temp607;}); return 1;} _LL590: return 0;
_LL582:;}} static int Cyc_Tcutil_evar_in_effect( void* evar, void* e){ e= Cyc_Tcutil_compress(
e);{ void* _temp614= e; struct Cyc_List_List* _temp622; int _temp624; struct Cyc_Core_Opt*
_temp626; void* _temp628; _LL616: if(( unsigned int) _temp614 > 4u?*(( int*)
_temp614) == Cyc_Absyn_JoinEff: 0){ _LL623: _temp622=( struct Cyc_List_List*)((
struct Cyc_Absyn_JoinEff_struct*) _temp614)->f1; goto _LL617;} else{ goto _LL618;}
_LL618: if(( unsigned int) _temp614 > 4u?*(( int*) _temp614) == Cyc_Absyn_Evar:
0){ _LL629: _temp628=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp614)->f1;
goto _LL627; _LL627: _temp626=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp614)->f2; goto _LL625; _LL625: _temp624=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp614)->f3; goto _LL619;} else{ goto _LL620;} _LL620: goto _LL621; _LL617:
for( 0; _temp622 != 0; _temp622=({ struct Cyc_List_List* _temp630= _temp622; if(
_temp630 == 0){ _throw( Null_Exception);} _temp630->tl;})){ if( Cyc_Tcutil_evar_in_effect(
evar,( void*)({ struct Cyc_List_List* _temp631= _temp622; if( _temp631 == 0){
_throw( Null_Exception);} _temp631->hd;}))){ return 1;}} return 0; _LL619:
return evar == e; _LL621: return 0; _LL615:;}} int Cyc_Tcutil_subset_effect( int
set_to_empty, void* e1, void* e2){{ struct _tuple8* _temp632= Cyc_Tcutil_get_effect_evar(
e2); struct _tuple8 _temp638; struct Cyc_List_List* _temp639; void* _temp641;
_LL634: if( _temp632 == 0){ goto _LL636;} else{ _temp638=* _temp632; _LL642:
_temp641= _temp638.f1; goto _LL640; _LL640: _temp639= _temp638.f2; goto _LL635;}
_LL636: goto _LL637; _LL635: { void* ev= Cyc_Absyn_new_evar(( void*) Cyc_Absyn_EffKind);
struct Cyc_Absyn_JoinEff_struct* new_typ=({ struct Cyc_Absyn_JoinEff_struct*
_temp643=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp643[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp644; _temp644.tag= Cyc_Absyn_JoinEff;
_temp644.f1=({ struct Cyc_List_List* _temp645=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp645->hd=( void*) ev; _temp645->tl=({ struct
Cyc_List_List* _temp646=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp646->hd=( void*) e1; _temp646->tl= 0; _temp646;}); _temp645;}); _temp644;});
_temp643;}); return Cyc_Tcutil_unify( _temp641,( void*) new_typ);} _LL637: goto
_LL633; _LL633:;}{ void* _temp647= Cyc_Tcutil_compress( e1); struct Cyc_List_List*
_temp659; void* _temp661; struct Cyc_Absyn_Tvar* _temp663; int _temp665; struct
Cyc_Core_Opt* _temp667; struct Cyc_Core_Opt** _temp669; void* _temp670; _LL649:
if(( unsigned int) _temp647 > 4u?*(( int*) _temp647) == Cyc_Absyn_JoinEff: 0){
_LL660: _temp659=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*)
_temp647)->f1; goto _LL650;} else{ goto _LL651;} _LL651: if(( unsigned int)
_temp647 > 4u?*(( int*) _temp647) == Cyc_Absyn_AccessEff: 0){ _LL662: _temp661=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp647)->f1; goto _LL652;} else{
goto _LL653;} _LL653: if(( unsigned int) _temp647 > 4u?*(( int*) _temp647) ==
Cyc_Absyn_VarType: 0){ _LL664: _temp663=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp647)->f1; goto _LL654;} else{ goto _LL655;} _LL655: if(( unsigned int)
_temp647 > 4u?*(( int*) _temp647) == Cyc_Absyn_Evar: 0){ _LL671: _temp670=( void*)((
struct Cyc_Absyn_Evar_struct*) _temp647)->f1; goto _LL668; _LL668: _temp667=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp647)->f2; _temp669=&((
struct Cyc_Absyn_Evar_struct*) _temp647)->f2; goto _LL666; _LL666: _temp665=(
int)(( struct Cyc_Absyn_Evar_struct*) _temp647)->f3; goto _LL656;} else{ goto
_LL657;} _LL657: goto _LL658; _LL650: for( 0; _temp659 != 0; _temp659=({ struct
Cyc_List_List* _temp672= _temp659; if( _temp672 == 0){ _throw( Null_Exception);}
_temp672->tl;})){ if( ! Cyc_Tcutil_subset_effect( set_to_empty,( void*)({ struct
Cyc_List_List* _temp673= _temp659; if( _temp673 == 0){ _throw( Null_Exception);}
_temp673->hd;}), e2)){ return 0;}} return 1; _LL652: return( Cyc_Tcutil_region_in_effect(
0, _temp661, e2)? 1: Cyc_Tcutil_region_in_effect( 1, _temp661, e2))? 1: Cyc_Tcutil_unify(
_temp661,( void*) Cyc_Absyn_HeapRgn); _LL654: return Cyc_Tcutil_variable_in_effect(
0, _temp663, e2)? 1: Cyc_Tcutil_variable_in_effect( 1, _temp663, e2); _LL656:
if( ! Cyc_Tcutil_evar_in_effect( e1, e2)){ if( set_to_empty){* _temp669=({
struct Cyc_Core_Opt* _temp674=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp674->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp675=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp675[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp676; _temp676.tag= Cyc_Absyn_JoinEff;
_temp676.f1= 0; _temp676;}); _temp675;})); _temp674;});} else{* _temp669=({
struct Cyc_Core_Opt* _temp677=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp677->v=( void*) e2; _temp677;});}} return 1; _LL658: return((
int(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({ struct _tagged_string
_temp678= Cyc_Absynpp_typ2string( e1); xprintf("subset_effect: bad effect: %.*s",
_temp678.last_plus_one - _temp678.curr, _temp678.curr);})); _LL648:;}} struct
_tuple9{ struct _tuple8* f1; struct _tuple8* f2; } ; static int Cyc_Tcutil_unify_effect(
void* e1, void* e2){ e1= Cyc_Tcutil_compress( e1); e2= Cyc_Tcutil_compress( e2);
Cyc_Tcutil_normalize_effect( e1); Cyc_Tcutil_normalize_effect( e2);{ struct
_tuple9 _temp680=({ struct _tuple9 _temp679; _temp679.f1= Cyc_Tcutil_get_effect_evar(
e1); _temp679.f2= Cyc_Tcutil_get_effect_evar( e2); _temp679;}); struct _tuple8*
_temp686; struct _tuple8 _temp688; struct Cyc_List_List* _temp689; void*
_temp691; struct _tuple8* _temp693; struct _tuple8 _temp695; struct Cyc_List_List*
_temp696; void* _temp698; _LL682: _LL694: _temp693= _temp680.f1; if( _temp693 ==
0){ goto _LL684;} else{ _temp695=* _temp693; _LL699: _temp698= _temp695.f1; goto
_LL697; _LL697: _temp696= _temp695.f2; goto _LL687;} _LL687: _temp686= _temp680.f2;
if( _temp686 == 0){ goto _LL684;} else{ _temp688=* _temp686; _LL692: _temp691=
_temp688.f1; goto _LL690; _LL690: _temp689= _temp688.f2; goto _LL683;} _LL684:
goto _LL685; _LL683: { void* ev= Cyc_Absyn_new_evar(( void*) Cyc_Absyn_EffKind);
struct Cyc_Absyn_JoinEff_struct* new_typ=({ struct Cyc_Absyn_JoinEff_struct*
_temp700=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp700[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp701; _temp701.tag= Cyc_Absyn_JoinEff;
_temp701.f1=({ struct Cyc_List_List* _temp702=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp702->hd=( void*) ev; _temp702->tl=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp696, _temp689); _temp702;}); _temp701;}); _temp700;}); return Cyc_Tcutil_unify(
_temp698,( void*) new_typ)? Cyc_Tcutil_unify( _temp691,( void*) new_typ): 0;}
_LL685: return Cyc_Tcutil_subset_effect( 0, e1, e2)? Cyc_Tcutil_subset_effect( 0,
e2, e1): 0; _LL681:;}} struct _tuple10{ struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt*
f2; } ; void Cyc_Tcutil_unify_it( void* t1, void* t2){ t1= Cyc_Tcutil_compress(
t1); t2= Cyc_Tcutil_compress( t2); if( t1 == t2){ return;}{ void* _temp703= t1;
int _temp709; struct Cyc_Core_Opt* _temp711; struct Cyc_Core_Opt** _temp713;
void* _temp714; _LL705: if(( unsigned int) _temp703 > 4u?*(( int*) _temp703) ==
Cyc_Absyn_Evar: 0){ _LL715: _temp714=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp703)->f1; goto _LL712; _LL712: _temp711=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Evar_struct*) _temp703)->f2; _temp713=&(( struct Cyc_Absyn_Evar_struct*)
_temp703)->f2; goto _LL710; _LL710: _temp709=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp703)->f3; goto _LL706;} else{ goto _LL707;} _LL707: goto _LL708; _LL706:
Cyc_Tcutil_occurs( t1, t2);{ void* kind2= Cyc_Tcutil_typ_kind( t2); if( Cyc_Tcutil_kind_leq(
kind2, _temp714)){* _temp713=({ struct Cyc_Core_Opt* _temp716=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp716->v=( void*) t2; _temp716;});
return;} else{{ void* _temp717= t2; int _temp725; struct Cyc_Core_Opt* _temp727;
struct Cyc_Core_Opt** _temp729; void* _temp730; struct Cyc_Absyn_PtrInfo
_temp733; _LL719: if(( unsigned int) _temp717 > 4u?*(( int*) _temp717) == Cyc_Absyn_Evar:
0){ _LL731: _temp730=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp717)->f1;
goto _LL728; _LL728: _temp727=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp717)->f2; _temp729=&(( struct Cyc_Absyn_Evar_struct*) _temp717)->f2; goto
_LL726; _LL726: _temp725=( int)(( struct Cyc_Absyn_Evar_struct*) _temp717)->f3;
goto _LL720;} else{ goto _LL721;} _LL721: if(( unsigned int) _temp717 > 4u?*((
int*) _temp717) == Cyc_Absyn_PointerType: 0){ _LL734: _temp733=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp717)->f1; goto _LL732;} else{ goto
_LL723;} _LL732: if( _temp714 ==( void*) Cyc_Absyn_BoxKind){ goto _LL722;} else{
goto _LL723;} _LL723: goto _LL724; _LL720: if( Cyc_Tcutil_kind_leq( _temp714,
kind2)){* _temp729=({ struct Cyc_Core_Opt* _temp735=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp735->v=( void*) t1; _temp735;});
return;} goto _LL718; _LL722: { struct Cyc_Absyn_Conref* c=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp733.bounds);{ void*
_temp736=( void*) c->v; _LL738: if( _temp736 ==( void*) Cyc_Absyn_No_constr){
goto _LL739;} else{ goto _LL740;} _LL740: goto _LL741; _LL739:( void*)( c->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp742=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp742[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp743; _temp743.tag= Cyc_Absyn_Eq_constr; _temp743.f1=(
void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp744=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp744[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp745; _temp745.tag= Cyc_Absyn_Upper_b; _temp745.f1= Cyc_Absyn_signed_int_exp(
1, 0); _temp745;}); _temp744;})); _temp743;}); _temp742;})));* _temp713=({
struct Cyc_Core_Opt* _temp746=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp746->v=( void*) t2; _temp746;}); return; _LL741: goto
_LL737; _LL737:;} goto _LL718;} _LL724: goto _LL718; _LL718:;}( void) _throw((
void*) Cyc_Tcutil_Unify);}} _LL708: goto _LL704; _LL704:;}{ struct _tuple7
_temp748=({ struct _tuple7 _temp747; _temp747.f1= t2; _temp747.f2= t1; _temp747;});
void* _temp794; void* _temp796; int _temp798; struct Cyc_Core_Opt* _temp800;
void* _temp802; void* _temp804; void* _temp806; void* _temp808; struct Cyc_Absyn_Tvar*
_temp810; void* _temp812; struct Cyc_Absyn_Tvar* _temp814; void* _temp816;
struct Cyc_Absyn_Structdecl** _temp818; struct Cyc_List_List* _temp820; struct
_tuple1* _temp822; void* _temp824; struct Cyc_Absyn_Structdecl** _temp826;
struct Cyc_List_List* _temp828; struct _tuple1* _temp830; void* _temp832; struct
Cyc_Absyn_Enumdecl* _temp834; struct _tuple1* _temp836; void* _temp838; struct
Cyc_Absyn_Enumdecl* _temp840; struct _tuple1* _temp842; void* _temp844; struct
Cyc_Absyn_Uniondecl** _temp846; struct Cyc_List_List* _temp848; struct _tuple1*
_temp850; void* _temp852; struct Cyc_Absyn_Uniondecl** _temp854; struct Cyc_List_List*
_temp856; struct _tuple1* _temp858; void* _temp860; struct Cyc_Absyn_TunionInfo
_temp862; void* _temp864; struct Cyc_List_List* _temp866; void* _temp868; struct
Cyc_Absyn_Tuniondecl* _temp870; void* _temp872; struct Cyc_Absyn_TunionInfo
_temp874; void* _temp876; struct Cyc_List_List* _temp878; void* _temp880; struct
Cyc_Absyn_Tuniondecl* _temp882; void* _temp884; struct Cyc_Absyn_TunionFieldInfo
_temp886; struct Cyc_List_List* _temp888; void* _temp890; struct Cyc_Absyn_Tunionfield*
_temp892; struct Cyc_Absyn_Tuniondecl* _temp894; void* _temp896; struct Cyc_Absyn_TunionFieldInfo
_temp898; struct Cyc_List_List* _temp900; void* _temp902; struct Cyc_Absyn_Tunionfield*
_temp904; struct Cyc_Absyn_Tuniondecl* _temp906; void* _temp908; struct Cyc_Absyn_PtrInfo
_temp910; struct Cyc_Absyn_Conref* _temp912; struct Cyc_Absyn_Tqual _temp914;
struct Cyc_Absyn_Conref* _temp916; void* _temp918; void* _temp920; void*
_temp922; struct Cyc_Absyn_PtrInfo _temp924; struct Cyc_Absyn_Conref* _temp926;
struct Cyc_Absyn_Tqual _temp928; struct Cyc_Absyn_Conref* _temp930; void*
_temp932; void* _temp934; void* _temp936; void* _temp938; void* _temp940; void*
_temp942; void* _temp944; void* _temp946; void* _temp948; void* _temp950; void*
_temp952; void* _temp954; void* _temp956; struct Cyc_Absyn_Exp* _temp958; struct
Cyc_Absyn_Tqual _temp960; void* _temp962; void* _temp964; struct Cyc_Absyn_Exp*
_temp966; struct Cyc_Absyn_Tqual _temp968; void* _temp970; void* _temp972;
struct Cyc_Absyn_FnInfo _temp974; struct Cyc_List_List* _temp976; int _temp978;
struct Cyc_List_List* _temp980; void* _temp982; struct Cyc_Core_Opt* _temp984;
struct Cyc_List_List* _temp986; void* _temp988; struct Cyc_Absyn_FnInfo _temp990;
struct Cyc_List_List* _temp992; int _temp994; struct Cyc_List_List* _temp996;
void* _temp998; struct Cyc_Core_Opt* _temp1000; struct Cyc_List_List* _temp1002;
void* _temp1004; struct Cyc_List_List* _temp1006; void* _temp1008; struct Cyc_List_List*
_temp1010; void* _temp1012; void* _temp1014; void* _temp1016; void* _temp1018;
void* _temp1020; void* _temp1022; void* _temp1024; void* _temp1026; struct Cyc_List_List*
_temp1028; void* _temp1030; struct Cyc_List_List* _temp1032; void* _temp1034;
void* _temp1036; void* _temp1038; void* _temp1040; void* _temp1042; void*
_temp1044; void* _temp1046; _LL750: _LL797: _temp796= _temp748.f1; if((
unsigned int) _temp796 > 4u?*(( int*) _temp796) == Cyc_Absyn_Evar: 0){ _LL803:
_temp802=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp796)->f1; goto _LL801;
_LL801: _temp800=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp796)->f2; goto _LL799; _LL799: _temp798=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp796)->f3; goto _LL795;} else{ goto _LL752;} _LL795: _temp794= _temp748.f2;
goto _LL751; _LL752: _LL807: _temp806= _temp748.f1; if( _temp806 ==( void*) Cyc_Absyn_VoidType){
goto _LL805;} else{ goto _LL754;} _LL805: _temp804= _temp748.f2; if( _temp804 ==(
void*) Cyc_Absyn_VoidType){ goto _LL753;} else{ goto _LL754;} _LL754: _LL813:
_temp812= _temp748.f1; if(( unsigned int) _temp812 > 4u?*(( int*) _temp812) ==
Cyc_Absyn_VarType: 0){ _LL815: _temp814=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp812)->f1; goto _LL809;} else{ goto _LL756;} _LL809: _temp808= _temp748.f2;
if(( unsigned int) _temp808 > 4u?*(( int*) _temp808) == Cyc_Absyn_VarType: 0){
_LL811: _temp810=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp808)->f1; goto _LL755;} else{ goto _LL756;} _LL756: _LL825: _temp824=
_temp748.f1; if(( unsigned int) _temp824 > 4u?*(( int*) _temp824) == Cyc_Absyn_StructType:
0){ _LL831: _temp830=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp824)->f1; goto _LL829; _LL829: _temp828=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp824)->f2; goto _LL827; _LL827: _temp826=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp824)->f3;
goto _LL817;} else{ goto _LL758;} _LL817: _temp816= _temp748.f2; if((
unsigned int) _temp816 > 4u?*(( int*) _temp816) == Cyc_Absyn_StructType: 0){
_LL823: _temp822=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp816)->f1; goto _LL821; _LL821: _temp820=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp816)->f2; goto _LL819; _LL819: _temp818=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp816)->f3;
goto _LL757;} else{ goto _LL758;} _LL758: _LL839: _temp838= _temp748.f1; if((
unsigned int) _temp838 > 4u?*(( int*) _temp838) == Cyc_Absyn_EnumType: 0){
_LL843: _temp842=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*)
_temp838)->f1; goto _LL841; _LL841: _temp840=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_EnumType_struct*) _temp838)->f2; goto _LL833;} else{ goto
_LL760;} _LL833: _temp832= _temp748.f2; if(( unsigned int) _temp832 > 4u?*(( int*)
_temp832) == Cyc_Absyn_EnumType: 0){ _LL837: _temp836=( struct _tuple1*)((
struct Cyc_Absyn_EnumType_struct*) _temp832)->f1; goto _LL835; _LL835: _temp834=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*) _temp832)->f2;
goto _LL759;} else{ goto _LL760;} _LL760: _LL853: _temp852= _temp748.f1; if((
unsigned int) _temp852 > 4u?*(( int*) _temp852) == Cyc_Absyn_UnionType: 0){
_LL859: _temp858=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp852)->f1; goto _LL857; _LL857: _temp856=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp852)->f2; goto _LL855; _LL855: _temp854=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp852)->f3;
goto _LL845;} else{ goto _LL762;} _LL845: _temp844= _temp748.f2; if((
unsigned int) _temp844 > 4u?*(( int*) _temp844) == Cyc_Absyn_UnionType: 0){
_LL851: _temp850=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp844)->f1; goto _LL849; _LL849: _temp848=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp844)->f2; goto _LL847; _LL847: _temp846=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp844)->f3;
goto _LL761;} else{ goto _LL762;} _LL762: _LL873: _temp872= _temp748.f1; if((
unsigned int) _temp872 > 4u?*(( int*) _temp872) == Cyc_Absyn_TunionType: 0){
_LL875: _temp874=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp872)->f1; _LL881: _temp880=( void*) _temp874.tunion_info; if(*(( int*)
_temp880) == Cyc_Absyn_KnownTunion){ _LL883: _temp882=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunion_struct*) _temp880)->f1; goto _LL879;} else{ goto
_LL764;} _LL879: _temp878=( struct Cyc_List_List*) _temp874.targs; goto _LL877;
_LL877: _temp876=( void*) _temp874.rgn; goto _LL861;} else{ goto _LL764;} _LL861:
_temp860= _temp748.f2; if(( unsigned int) _temp860 > 4u?*(( int*) _temp860) ==
Cyc_Absyn_TunionType: 0){ _LL863: _temp862=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp860)->f1; _LL869: _temp868=( void*)
_temp862.tunion_info; if(*(( int*) _temp868) == Cyc_Absyn_KnownTunion){ _LL871:
_temp870=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_KnownTunion_struct*)
_temp868)->f1; goto _LL867;} else{ goto _LL764;} _LL867: _temp866=( struct Cyc_List_List*)
_temp862.targs; goto _LL865; _LL865: _temp864=( void*) _temp862.rgn; goto _LL763;}
else{ goto _LL764;} _LL764: _LL897: _temp896= _temp748.f1; if(( unsigned int)
_temp896 > 4u?*(( int*) _temp896) == Cyc_Absyn_TunionFieldType: 0){ _LL899:
_temp898=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp896)->f1; _LL903: _temp902=( void*) _temp898.field_info; if(*(( int*)
_temp902) == Cyc_Absyn_KnownTunionfield){ _LL907: _temp906=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp902)->f1; goto _LL905; _LL905:
_temp904=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp902)->f2; goto _LL901;} else{ goto _LL766;} _LL901: _temp900=( struct Cyc_List_List*)
_temp898.targs; goto _LL885;} else{ goto _LL766;} _LL885: _temp884= _temp748.f2;
if(( unsigned int) _temp884 > 4u?*(( int*) _temp884) == Cyc_Absyn_TunionFieldType:
0){ _LL887: _temp886=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp884)->f1; _LL891: _temp890=( void*) _temp886.field_info; if(*(( int*)
_temp890) == Cyc_Absyn_KnownTunionfield){ _LL895: _temp894=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp890)->f1; goto _LL893; _LL893:
_temp892=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp890)->f2; goto _LL889;} else{ goto _LL766;} _LL889: _temp888=( struct Cyc_List_List*)
_temp886.targs; goto _LL765;} else{ goto _LL766;} _LL766: _LL923: _temp922=
_temp748.f1; if(( unsigned int) _temp922 > 4u?*(( int*) _temp922) == Cyc_Absyn_PointerType:
0){ _LL925: _temp924=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp922)->f1; _LL935: _temp934=( void*) _temp924.elt_typ; goto _LL933; _LL933:
_temp932=( void*) _temp924.rgn_typ; goto _LL931; _LL931: _temp930=( struct Cyc_Absyn_Conref*)
_temp924.nullable; goto _LL929; _LL929: _temp928=( struct Cyc_Absyn_Tqual)
_temp924.tq; goto _LL927; _LL927: _temp926=( struct Cyc_Absyn_Conref*) _temp924.bounds;
goto _LL909;} else{ goto _LL768;} _LL909: _temp908= _temp748.f2; if((
unsigned int) _temp908 > 4u?*(( int*) _temp908) == Cyc_Absyn_PointerType: 0){
_LL911: _temp910=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp908)->f1; _LL921: _temp920=( void*) _temp910.elt_typ; goto _LL919; _LL919:
_temp918=( void*) _temp910.rgn_typ; goto _LL917; _LL917: _temp916=( struct Cyc_Absyn_Conref*)
_temp910.nullable; goto _LL915; _LL915: _temp914=( struct Cyc_Absyn_Tqual)
_temp910.tq; goto _LL913; _LL913: _temp912=( struct Cyc_Absyn_Conref*) _temp910.bounds;
goto _LL767;} else{ goto _LL768;} _LL768: _LL943: _temp942= _temp748.f1; if((
unsigned int) _temp942 > 4u?*(( int*) _temp942) == Cyc_Absyn_IntType: 0){ _LL947:
_temp946=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp942)->f1; goto _LL945;
_LL945: _temp944=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp942)->f2;
goto _LL937;} else{ goto _LL770;} _LL937: _temp936= _temp748.f2; if((
unsigned int) _temp936 > 4u?*(( int*) _temp936) == Cyc_Absyn_IntType: 0){ _LL941:
_temp940=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp936)->f1; goto _LL939;
_LL939: _temp938=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp936)->f2;
goto _LL769;} else{ goto _LL770;} _LL770: _LL951: _temp950= _temp748.f1; if(
_temp950 ==( void*) Cyc_Absyn_FloatType){ goto _LL949;} else{ goto _LL772;}
_LL949: _temp948= _temp748.f2; if( _temp948 ==( void*) Cyc_Absyn_FloatType){
goto _LL771;} else{ goto _LL772;} _LL772: _LL955: _temp954= _temp748.f1; if(
_temp954 ==( void*) Cyc_Absyn_DoubleType){ goto _LL953;} else{ goto _LL774;}
_LL953: _temp952= _temp748.f2; if( _temp952 ==( void*) Cyc_Absyn_DoubleType){
goto _LL773;} else{ goto _LL774;} _LL774: _LL965: _temp964= _temp748.f1; if((
unsigned int) _temp964 > 4u?*(( int*) _temp964) == Cyc_Absyn_ArrayType: 0){
_LL971: _temp970=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp964)->f1;
goto _LL969; _LL969: _temp968=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp964)->f2; goto _LL967; _LL967: _temp966=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp964)->f3; goto _LL957;} else{ goto _LL776;}
_LL957: _temp956= _temp748.f2; if(( unsigned int) _temp956 > 4u?*(( int*)
_temp956) == Cyc_Absyn_ArrayType: 0){ _LL963: _temp962=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp956)->f1; goto _LL961; _LL961: _temp960=( struct Cyc_Absyn_Tqual)(( struct
Cyc_Absyn_ArrayType_struct*) _temp956)->f2; goto _LL959; _LL959: _temp958=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*) _temp956)->f3; goto
_LL775;} else{ goto _LL776;} _LL776: _LL989: _temp988= _temp748.f1; if((
unsigned int) _temp988 > 4u?*(( int*) _temp988) == Cyc_Absyn_FnType: 0){ _LL991:
_temp990=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*) _temp988)->f1;
_LL1003: _temp1002=( struct Cyc_List_List*) _temp990.tvars; goto _LL1001;
_LL1001: _temp1000=( struct Cyc_Core_Opt*) _temp990.effect; goto _LL999; _LL999:
_temp998=( void*) _temp990.ret_typ; goto _LL997; _LL997: _temp996=( struct Cyc_List_List*)
_temp990.args; goto _LL995; _LL995: _temp994=( int) _temp990.varargs; goto
_LL993; _LL993: _temp992=( struct Cyc_List_List*) _temp990.attributes; goto
_LL973;} else{ goto _LL778;} _LL973: _temp972= _temp748.f2; if(( unsigned int)
_temp972 > 4u?*(( int*) _temp972) == Cyc_Absyn_FnType: 0){ _LL975: _temp974=(
struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*) _temp972)->f1;
_LL987: _temp986=( struct Cyc_List_List*) _temp974.tvars; goto _LL985; _LL985:
_temp984=( struct Cyc_Core_Opt*) _temp974.effect; goto _LL983; _LL983: _temp982=(
void*) _temp974.ret_typ; goto _LL981; _LL981: _temp980=( struct Cyc_List_List*)
_temp974.args; goto _LL979; _LL979: _temp978=( int) _temp974.varargs; goto
_LL977; _LL977: _temp976=( struct Cyc_List_List*) _temp974.attributes; goto
_LL777;} else{ goto _LL778;} _LL778: _LL1009: _temp1008= _temp748.f1; if((
unsigned int) _temp1008 > 4u?*(( int*) _temp1008) == Cyc_Absyn_TupleType: 0){
_LL1011: _temp1010=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp1008)->f1; goto _LL1005;} else{ goto _LL780;} _LL1005: _temp1004= _temp748.f2;
if(( unsigned int) _temp1004 > 4u?*(( int*) _temp1004) == Cyc_Absyn_TupleType: 0){
_LL1007: _temp1006=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp1004)->f1; goto _LL779;} else{ goto _LL780;} _LL780: _LL1015: _temp1014=
_temp748.f1; if( _temp1014 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL1013;} else{
goto _LL782;} _LL1013: _temp1012= _temp748.f2; if( _temp1012 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL781;} else{ goto _LL782;} _LL782: _LL1021: _temp1020= _temp748.f1; if((
unsigned int) _temp1020 > 4u?*(( int*) _temp1020) == Cyc_Absyn_RgnHandleType: 0){
_LL1023: _temp1022=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp1020)->f1;
goto _LL1017;} else{ goto _LL784;} _LL1017: _temp1016= _temp748.f2; if((
unsigned int) _temp1016 > 4u?*(( int*) _temp1016) == Cyc_Absyn_RgnHandleType: 0){
_LL1019: _temp1018=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp1016)->f1;
goto _LL783;} else{ goto _LL784;} _LL784: _LL1027: _temp1026= _temp748.f1; if((
unsigned int) _temp1026 > 4u?*(( int*) _temp1026) == Cyc_Absyn_JoinEff: 0){
_LL1029: _temp1028=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*)
_temp1026)->f1; goto _LL1025;} else{ goto _LL786;} _LL1025: _temp1024= _temp748.f2;
goto _LL785; _LL786: _LL1035: _temp1034= _temp748.f1; goto _LL1031; _LL1031:
_temp1030= _temp748.f2; if(( unsigned int) _temp1030 > 4u?*(( int*) _temp1030)
== Cyc_Absyn_JoinEff: 0){ _LL1033: _temp1032=( struct Cyc_List_List*)(( struct
Cyc_Absyn_JoinEff_struct*) _temp1030)->f1; goto _LL787;} else{ goto _LL788;}
_LL788: _LL1039: _temp1038= _temp748.f1; if(( unsigned int) _temp1038 > 4u?*((
int*) _temp1038) == Cyc_Absyn_AccessEff: 0){ _LL1041: _temp1040=( void*)((
struct Cyc_Absyn_AccessEff_struct*) _temp1038)->f1; goto _LL1037;} else{ goto
_LL790;} _LL1037: _temp1036= _temp748.f2; goto _LL789; _LL790: _LL1047:
_temp1046= _temp748.f1; goto _LL1043; _LL1043: _temp1042= _temp748.f2; if((
unsigned int) _temp1042 > 4u?*(( int*) _temp1042) == Cyc_Absyn_AccessEff: 0){
_LL1045: _temp1044=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp1042)->f1;
goto _LL791;} else{ goto _LL792;} _LL792: goto _LL793; _LL751: Cyc_Tcutil_unify_it(
t2, t1); return; _LL753: return; _LL755: { struct _tagged_string* x2= _temp814->name;
struct _tagged_string* x1= _temp810->name; void* k2=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp814->kind); void* k1=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp810->kind); if( Cyc_String_zstrptrcmp( x2, x1)
== 0){ if( k2 != k1){(( void(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(({
struct _tagged_string _temp1048=* x2; struct _tagged_string _temp1049= Cyc_Absynpp_kind2string(
k2); struct _tagged_string _temp1050= Cyc_Absynpp_kind2string( k1); xprintf("same type variable %.*s has kinds %.*s and %.*s",
_temp1048.last_plus_one - _temp1048.curr, _temp1048.curr, _temp1049.last_plus_one
- _temp1049.curr, _temp1049.curr, _temp1050.last_plus_one - _temp1050.curr,
_temp1050.curr);}));} return;} goto _LL749;} _LL757: if((( _temp822 != 0?
_temp830 != 0: 0)? Cyc_Absyn_qvar_cmp(( struct _tuple1*)({ struct _tuple1*
_temp1051= _temp822; if( _temp1051 == 0){ _throw( Null_Exception);} _temp1051;}),(
struct _tuple1*)({ struct _tuple1* _temp1052= _temp830; if( _temp1052 == 0){
_throw( Null_Exception);} _temp1052;})) == 0: 0)? 1:( _temp822 == 0? _temp830 ==
0: 0)){ Cyc_Tcutil_unify_list( _temp820, _temp828); return;} goto _LL749; _LL759:
if( Cyc_Absyn_qvar_cmp( _temp842, _temp836) == 0){ return;} goto _LL749; _LL761:
if((( _temp850 != 0? _temp858 != 0: 0)? Cyc_Absyn_qvar_cmp(( struct _tuple1*)({
struct _tuple1* _temp1053= _temp850; if( _temp1053 == 0){ _throw( Null_Exception);}
_temp1053;}),( struct _tuple1*)({ struct _tuple1* _temp1054= _temp858; if(
_temp1054 == 0){ _throw( Null_Exception);} _temp1054;})) == 0: 0)? 1:( _temp850
== 0? _temp858 == 0: 0)){ Cyc_Tcutil_unify_list( _temp848, _temp856); return;}
goto _LL749; _LL763: if( _temp882 == _temp870? 1: Cyc_Absyn_qvar_cmp( _temp882->name,
_temp870->name) == 0){ Cyc_Tcutil_unify_it( _temp864, _temp876); Cyc_Tcutil_unify_list(
_temp866, _temp878); return;} goto _LL749; _LL765: if(( _temp906 == _temp894? 1:
Cyc_Absyn_qvar_cmp( _temp906->name, _temp894->name) == 0)? _temp904 == _temp892?
1: Cyc_Absyn_qvar_cmp( _temp904->name, _temp892->name) == 0: 0){ Cyc_Tcutil_unify_list(
_temp888, _temp900); return;} goto _LL749; _LL767: Cyc_Tcutil_unify_it( _temp920,
_temp934); Cyc_Tcutil_unify_it( _temp932, _temp918); Cyc_Tcutil_unify_tqual(
_temp914, _temp928);(( void(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref*
x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_it_conrefs)( Cyc_Tcutil_boundscmp,
_temp912, _temp926);{ void* _temp1055=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp912))->v; void*
_temp1061; _LL1057: if(( unsigned int) _temp1055 > 1u?*(( int*) _temp1055) ==
Cyc_Absyn_Eq_constr: 0){ _LL1062: _temp1061=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp1055)->f1; if( _temp1061 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1058;}
else{ goto _LL1059;}} else{ goto _LL1059;} _LL1059: goto _LL1060; _LL1058:
return; _LL1060: goto _LL1056; _LL1056:;}(( void(*)( int(* cmp)( int, int),
struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_it_conrefs)(
Cyc_Core_intcmp, _temp916, _temp930); return; _LL769: if( _temp940 == _temp946?
_temp938 == _temp944: 0){ return;} goto _LL749; _LL771: return; _LL773: return;
_LL775: Cyc_Tcutil_unify_it( _temp962, _temp970); Cyc_Tcutil_unify_tqual(
_temp960, _temp968); if( _temp966 == _temp958){ return;} if( _temp966 == 0? 1:
_temp958 == 0){ goto _LL749;} if( Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1063= _temp966; if( _temp1063 == 0){ _throw(
Null_Exception);} _temp1063;})) == Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1064= _temp958; if( _temp1064 == 0){ _throw(
Null_Exception);} _temp1064;}))){ return;} goto _LL749; _LL777: { int done= 0;{
struct _RegionHandle _temp1065= _new_region(); struct _RegionHandle* rgn=&
_temp1065; struct _handler_cons _temp1066; _push_handler(& _temp1066);{ void*
_temp1067=( void*) setjmp( _temp1066.handler); if( ! _temp1067){{ struct Cyc_List_List*
inst= 0; while( _temp986 != 0) { if( _temp1002 == 0){ break;} inst=({ struct Cyc_List_List*
_temp1068=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp1068->hd=( void*)({ struct _tuple5* _temp1069=( struct _tuple5*)
_region_malloc( rgn, sizeof( struct _tuple5)); _temp1069->f1=( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp1073= _temp1002; if( _temp1073 == 0){ _throw(
Null_Exception);} _temp1073->hd;}); _temp1069->f2=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp1070=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp1070[ 0]=({ struct Cyc_Absyn_VarType_struct _temp1071; _temp1071.tag= Cyc_Absyn_VarType;
_temp1071.f1=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp1072=
_temp986; if( _temp1072 == 0){ _throw( Null_Exception);} _temp1072->hd;});
_temp1071;}); _temp1070;}); _temp1069;}); _temp1068->tl= inst; _temp1068;});
_temp986=({ struct Cyc_List_List* _temp1074= _temp986; if( _temp1074 == 0){
_throw( Null_Exception);} _temp1074->tl;}); _temp1002=({ struct Cyc_List_List*
_temp1075= _temp1002; if( _temp1075 == 0){ _throw( Null_Exception);} _temp1075->tl;});}
if( _temp1002 != 0){ _npop_handler( 0u); goto _LL749;} if( inst != 0){ Cyc_Tcutil_unify_it((
void*)({ struct Cyc_Absyn_FnType_struct* _temp1076=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1076[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1077; _temp1077.tag= Cyc_Absyn_FnType; _temp1077.f1=({ struct Cyc_Absyn_FnInfo
_temp1078; _temp1078.tvars= 0; _temp1078.effect= _temp984; _temp1078.ret_typ=(
void*) _temp982; _temp1078.args= _temp980; _temp1078.varargs= _temp978;
_temp1078.attributes= _temp976; _temp1078;}); _temp1077;}); _temp1076;}), Cyc_Tcutil_rsubstitute(
rgn, inst,( void*)({ struct Cyc_Absyn_FnType_struct* _temp1079=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1079[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1080; _temp1080.tag= Cyc_Absyn_FnType; _temp1080.f1=({ struct Cyc_Absyn_FnInfo
_temp1081; _temp1081.tvars= 0; _temp1081.effect= _temp1000; _temp1081.ret_typ=(
void*) _temp998; _temp1081.args= _temp996; _temp1081.varargs= _temp994;
_temp1081.attributes= _temp992; _temp1081;}); _temp1080;}); _temp1079;}))); done=
1;}} _free_region(& _temp1065); _pop_handler();} else{ _free_region(& _temp1065);
_throw( _temp1067);}}} if( done){ return;} Cyc_Tcutil_unify_it( _temp982,
_temp998); for( 0; _temp980 != 0? _temp996 != 0: 0; _temp980=({ struct Cyc_List_List*
_temp1082= _temp980; if( _temp1082 == 0){ _throw( Null_Exception);} _temp1082->tl;}),
_temp996=({ struct Cyc_List_List* _temp1083= _temp996; if( _temp1083 == 0){
_throw( Null_Exception);} _temp1083->tl;})){ Cyc_Tcutil_unify_tqual((*(( struct
_tuple2*)({ struct Cyc_List_List* _temp1084= _temp980; if( _temp1084 == 0){
_throw( Null_Exception);} _temp1084->hd;}))).f2,(*(( struct _tuple2*)({ struct
Cyc_List_List* _temp1085= _temp996; if( _temp1085 == 0){ _throw( Null_Exception);}
_temp1085->hd;}))).f2); Cyc_Tcutil_unify_it((*(( struct _tuple2*)({ struct Cyc_List_List*
_temp1086= _temp980; if( _temp1086 == 0){ _throw( Null_Exception);} _temp1086->hd;}))).f3,(*((
struct _tuple2*)({ struct Cyc_List_List* _temp1087= _temp996; if( _temp1087 == 0){
_throw( Null_Exception);} _temp1087->hd;}))).f3);} if( _temp980 != 0? 1:
_temp996 != 0){ goto _LL749;} if( _temp978 != _temp994){ goto _LL749;}{ int
bad_effect= 0;{ struct _tuple10 _temp1089=({ struct _tuple10 _temp1088;
_temp1088.f1= _temp984; _temp1088.f2= _temp1000; _temp1088;}); struct Cyc_Core_Opt*
_temp1099; struct Cyc_Core_Opt* _temp1101; struct Cyc_Core_Opt* _temp1103;
struct Cyc_Core_Opt* _temp1105; struct Cyc_Core_Opt* _temp1107; struct Cyc_Core_Opt*
_temp1109; _LL1091: _LL1102: _temp1101= _temp1089.f1; if( _temp1101 == 0){ goto
_LL1100;} else{ goto _LL1093;} _LL1100: _temp1099= _temp1089.f2; if( _temp1099
== 0){ goto _LL1092;} else{ goto _LL1093;} _LL1093: _LL1106: _temp1105=
_temp1089.f1; if( _temp1105 == 0){ goto _LL1104;} else{ goto _LL1095;} _LL1104:
_temp1103= _temp1089.f2; goto _LL1094; _LL1095: _LL1110: _temp1109= _temp1089.f1;
goto _LL1108; _LL1108: _temp1107= _temp1089.f2; if( _temp1107 == 0){ goto
_LL1096;} else{ goto _LL1097;} _LL1097: goto _LL1098; _LL1092: goto _LL1090;
_LL1094: goto _LL1096; _LL1096: bad_effect= 1; goto _LL1090; _LL1098: bad_effect=
! Cyc_Tcutil_unify_effect(( void*)({ struct Cyc_Core_Opt* _temp1111= _temp984;
if( _temp1111 == 0){ _throw( Null_Exception);} _temp1111->v;}),( void*)({ struct
Cyc_Core_Opt* _temp1112= _temp1000; if( _temp1112 == 0){ _throw( Null_Exception);}
_temp1112->v;})); goto _LL1090; _LL1090:;} if( bad_effect){ goto _LL749;} if( !
Cyc_Tcutil_same_atts( _temp992, _temp976)){ goto _LL749;} return;}} _LL779: for(
0; _temp1006 != 0? _temp1010 != 0: 0; _temp1006=({ struct Cyc_List_List*
_temp1113= _temp1006; if( _temp1113 == 0){ _throw( Null_Exception);} _temp1113->tl;}),
_temp1010=({ struct Cyc_List_List* _temp1114= _temp1010; if( _temp1114 == 0){
_throw( Null_Exception);} _temp1114->tl;})){ Cyc_Tcutil_unify_tqual((*(( struct
_tuple4*)({ struct Cyc_List_List* _temp1115= _temp1006; if( _temp1115 == 0){
_throw( Null_Exception);} _temp1115->hd;}))).f1,(*(( struct _tuple4*)({ struct
Cyc_List_List* _temp1116= _temp1010; if( _temp1116 == 0){ _throw( Null_Exception);}
_temp1116->hd;}))).f1); Cyc_Tcutil_unify_it((*(( struct _tuple4*)({ struct Cyc_List_List*
_temp1117= _temp1006; if( _temp1117 == 0){ _throw( Null_Exception);} _temp1117->hd;}))).f2,(*((
struct _tuple4*)({ struct Cyc_List_List* _temp1118= _temp1010; if( _temp1118 ==
0){ _throw( Null_Exception);} _temp1118->hd;}))).f2);} if( _temp1006 == 0?
_temp1010 == 0: 0){ return;} goto _LL749; _LL781: return; _LL783: Cyc_Tcutil_unify_it(
_temp1022, _temp1018); return; _LL785: goto _LL787; _LL787: goto _LL789; _LL789:
goto _LL791; _LL791: if( Cyc_Tcutil_unify_effect( t1, t2)){ return;} else{ goto
_LL749;} _LL793: goto _LL749; _LL749:;}( void) _throw(( void*) Cyc_Tcutil_Unify);}
int Cyc_Tcutil_is_arithmetic_type( void* t){ void* _temp1119= Cyc_Tcutil_compress(
t); void* _temp1131; void* _temp1133; struct Cyc_Absyn_Enumdecl* _temp1135;
struct _tuple1* _temp1137; _LL1121: if(( unsigned int) _temp1119 > 4u?*(( int*)
_temp1119) == Cyc_Absyn_IntType: 0){ _LL1134: _temp1133=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1119)->f1; goto _LL1132; _LL1132: _temp1131=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1119)->f2; goto _LL1122;} else{ goto _LL1123;} _LL1123: if( _temp1119 ==(
void*) Cyc_Absyn_FloatType){ goto _LL1124;} else{ goto _LL1125;} _LL1125: if(
_temp1119 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1126;} else{ goto _LL1127;}
_LL1127: if(( unsigned int) _temp1119 > 4u?*(( int*) _temp1119) == Cyc_Absyn_EnumType:
0){ _LL1138: _temp1137=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*)
_temp1119)->f1; goto _LL1136; _LL1136: _temp1135=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_EnumType_struct*) _temp1119)->f2; goto _LL1128;} else{ goto
_LL1129;} _LL1129: goto _LL1130; _LL1122: return 1; _LL1124: return 1; _LL1126:
return 1; _LL1128: return 1; _LL1130: return 0; _LL1120:;} int Cyc_Tcutil_will_lose_precision(
void* t1, void* t2){ t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{
struct _tuple7 _temp1140=({ struct _tuple7 _temp1139; _temp1139.f1= t1;
_temp1139.f2= t2; _temp1139;}); void* _temp1162; void* _temp1164; void*
_temp1166; void* _temp1168; void* _temp1170; void* _temp1172; void* _temp1174;
void* _temp1176; void* _temp1178; void* _temp1180; void* _temp1182; void*
_temp1184; void* _temp1186; void* _temp1188; void* _temp1190; void* _temp1192;
void* _temp1194; void* _temp1196; void* _temp1198; void* _temp1200; void*
_temp1202; void* _temp1204; void* _temp1206; void* _temp1208; void* _temp1210;
void* _temp1212; void* _temp1214; void* _temp1216; void* _temp1218; void*
_temp1220; void* _temp1222; void* _temp1224; void* _temp1226; void* _temp1228;
void* _temp1230; void* _temp1232; void* _temp1234; void* _temp1236; void*
_temp1238; void* _temp1240; void* _temp1242; void* _temp1244; _LL1142: _LL1165:
_temp1164= _temp1140.f1; if( _temp1164 ==( void*) Cyc_Absyn_DoubleType){ goto
_LL1163;} else{ goto _LL1144;} _LL1163: _temp1162= _temp1140.f2; if( _temp1162
==( void*) Cyc_Absyn_FloatType){ goto _LL1143;} else{ goto _LL1144;} _LL1144:
_LL1173: _temp1172= _temp1140.f1; if( _temp1172 ==( void*) Cyc_Absyn_DoubleType){
goto _LL1167;} else{ goto _LL1146;} _LL1167: _temp1166= _temp1140.f2; if((
unsigned int) _temp1166 > 4u?*(( int*) _temp1166) == Cyc_Absyn_IntType: 0){
_LL1171: _temp1170=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1166)->f1;
goto _LL1169; _LL1169: _temp1168=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1166)->f2; goto _LL1145;} else{ goto _LL1146;} _LL1146: _LL1181: _temp1180=
_temp1140.f1; if( _temp1180 ==( void*) Cyc_Absyn_FloatType){ goto _LL1175;}
else{ goto _LL1148;} _LL1175: _temp1174= _temp1140.f2; if(( unsigned int)
_temp1174 > 4u?*(( int*) _temp1174) == Cyc_Absyn_IntType: 0){ _LL1179: _temp1178=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1174)->f1; goto _LL1177; _LL1177:
_temp1176=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1174)->f2; goto
_LL1147;} else{ goto _LL1148;} _LL1148: _LL1189: _temp1188= _temp1140.f1; if((
unsigned int) _temp1188 > 4u?*(( int*) _temp1188) == Cyc_Absyn_IntType: 0){
_LL1193: _temp1192=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1188)->f1;
goto _LL1191; _LL1191: _temp1190=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1188)->f2; if( _temp1190 ==( void*) Cyc_Absyn_B8){ goto _LL1183;} else{
goto _LL1150;}} else{ goto _LL1150;} _LL1183: _temp1182= _temp1140.f2; if((
unsigned int) _temp1182 > 4u?*(( int*) _temp1182) == Cyc_Absyn_IntType: 0){
_LL1187: _temp1186=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1182)->f1;
goto _LL1185; _LL1185: _temp1184=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1182)->f2; if( _temp1184 ==( void*) Cyc_Absyn_B8){ goto _LL1149;} else{
goto _LL1150;}} else{ goto _LL1150;} _LL1150: _LL1197: _temp1196= _temp1140.f1;
if(( unsigned int) _temp1196 > 4u?*(( int*) _temp1196) == Cyc_Absyn_IntType: 0){
_LL1201: _temp1200=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1196)->f1;
goto _LL1199; _LL1199: _temp1198=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1196)->f2; if( _temp1198 ==( void*) Cyc_Absyn_B8){ goto _LL1195;} else{
goto _LL1152;}} else{ goto _LL1152;} _LL1195: _temp1194= _temp1140.f2; goto
_LL1151; _LL1152: _LL1205: _temp1204= _temp1140.f1; if(( unsigned int) _temp1204
> 4u?*(( int*) _temp1204) == Cyc_Absyn_IntType: 0){ _LL1209: _temp1208=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp1204)->f1; goto _LL1207; _LL1207:
_temp1206=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1204)->f2; if(
_temp1206 ==( void*) Cyc_Absyn_B4){ goto _LL1203;} else{ goto _LL1154;}} else{
goto _LL1154;} _LL1203: _temp1202= _temp1140.f2; if( _temp1202 ==( void*) Cyc_Absyn_FloatType){
goto _LL1153;} else{ goto _LL1154;} _LL1154: _LL1217: _temp1216= _temp1140.f1;
if(( unsigned int) _temp1216 > 4u?*(( int*) _temp1216) == Cyc_Absyn_IntType: 0){
_LL1221: _temp1220=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1216)->f1;
goto _LL1219; _LL1219: _temp1218=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1216)->f2; if( _temp1218 ==( void*) Cyc_Absyn_B4){ goto _LL1211;} else{
goto _LL1156;}} else{ goto _LL1156;} _LL1211: _temp1210= _temp1140.f2; if((
unsigned int) _temp1210 > 4u?*(( int*) _temp1210) == Cyc_Absyn_IntType: 0){
_LL1215: _temp1214=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1210)->f1;
goto _LL1213; _LL1213: _temp1212=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1210)->f2; if( _temp1212 ==( void*) Cyc_Absyn_B2){ goto _LL1155;} else{
goto _LL1156;}} else{ goto _LL1156;} _LL1156: _LL1229: _temp1228= _temp1140.f1;
if(( unsigned int) _temp1228 > 4u?*(( int*) _temp1228) == Cyc_Absyn_IntType: 0){
_LL1233: _temp1232=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1228)->f1;
goto _LL1231; _LL1231: _temp1230=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1228)->f2; if( _temp1230 ==( void*) Cyc_Absyn_B4){ goto _LL1223;} else{
goto _LL1158;}} else{ goto _LL1158;} _LL1223: _temp1222= _temp1140.f2; if((
unsigned int) _temp1222 > 4u?*(( int*) _temp1222) == Cyc_Absyn_IntType: 0){
_LL1227: _temp1226=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1222)->f1;
goto _LL1225; _LL1225: _temp1224=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1222)->f2; if( _temp1224 ==( void*) Cyc_Absyn_B1){ goto _LL1157;} else{
goto _LL1158;}} else{ goto _LL1158;} _LL1158: _LL1241: _temp1240= _temp1140.f1;
if(( unsigned int) _temp1240 > 4u?*(( int*) _temp1240) == Cyc_Absyn_IntType: 0){
_LL1245: _temp1244=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1240)->f1;
goto _LL1243; _LL1243: _temp1242=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1240)->f2; if( _temp1242 ==( void*) Cyc_Absyn_B2){ goto _LL1235;} else{
goto _LL1160;}} else{ goto _LL1160;} _LL1235: _temp1234= _temp1140.f2; if((
unsigned int) _temp1234 > 4u?*(( int*) _temp1234) == Cyc_Absyn_IntType: 0){
_LL1239: _temp1238=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1234)->f1;
goto _LL1237; _LL1237: _temp1236=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1234)->f2; if( _temp1236 ==( void*) Cyc_Absyn_B1){ goto _LL1159;} else{
goto _LL1160;}} else{ goto _LL1160;} _LL1160: goto _LL1161; _LL1143: return 1;
_LL1145: return 1; _LL1147: return 1; _LL1149: return 0; _LL1151: return 1;
_LL1153: return 1; _LL1155: return 1; _LL1157: return 1; _LL1159: return 1;
_LL1161: return 0; _LL1141:;}} int Cyc_Tcutil_coerce_list( struct Cyc_Tcenv_Tenv*
te, void* t, struct Cyc_List_List* es){ struct Cyc_Core_Opt* max_arith_type= 0;{
struct Cyc_List_List* el= es; for( 0; el != 0; el=({ struct Cyc_List_List*
_temp1246= el; if( _temp1246 == 0){ _throw( Null_Exception);} _temp1246->tl;})){
void* t1= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp1250=((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1249= el; if( _temp1249 == 0){
_throw( Null_Exception);} _temp1249->hd;}))->topt; if( _temp1250 == 0){ _throw(
Null_Exception);} _temp1250->v;})); if( Cyc_Tcutil_is_arithmetic_type( t1)){ if(
max_arith_type == 0? 1: Cyc_Tcutil_will_lose_precision( t1,( void*)({ struct Cyc_Core_Opt*
_temp1247= max_arith_type; if( _temp1247 == 0){ _throw( Null_Exception);}
_temp1247->v;}))){ max_arith_type=({ struct Cyc_Core_Opt* _temp1248=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1248->v=( void*) t1; _temp1248;});}}}}
if( max_arith_type != 0){ if( ! Cyc_Tcutil_unify( t,( void*)({ struct Cyc_Core_Opt*
_temp1251= max_arith_type; if( _temp1251 == 0){ _throw( Null_Exception);}
_temp1251->v;}))){ return 0;}}{ struct Cyc_List_List* el= es; for( 0; el != 0;
el=({ struct Cyc_List_List* _temp1252= el; if( _temp1252 == 0){ _throw(
Null_Exception);} _temp1252->tl;})){ if( ! Cyc_Tcutil_coerce_assign( te,( struct
Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1253= el; if( _temp1253 == 0){
_throw( Null_Exception);} _temp1253->hd;}), t)){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1254= el; if( _temp1254 == 0){ _throw( Null_Exception);}
_temp1254->hd;}))->loc,({ struct _tagged_string _temp1257= Cyc_Absynpp_typ2string(
t); struct _tagged_string _temp1258= Cyc_Absynpp_typ2string(( void*)({ struct
Cyc_Core_Opt* _temp1256=(( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1255= el; if( _temp1255 == 0){ _throw( Null_Exception);} _temp1255->hd;}))->topt;
if( _temp1256 == 0){ _throw( Null_Exception);} _temp1256->v;})); xprintf("type mismatch: expecting %.*s but found %.*s",
_temp1257.last_plus_one - _temp1257.curr, _temp1257.curr, _temp1258.last_plus_one
- _temp1258.curr, _temp1258.curr);})); return 0;}}} return 1;} int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( ! Cyc_Tcutil_coerce_sint_typ(
te, e)){ void* _temp1260= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp1259= e->topt; if( _temp1259 == 0){ _throw( Null_Exception);} _temp1259->v;}));
struct Cyc_Absyn_PtrInfo _temp1266; _LL1262: if(( unsigned int) _temp1260 > 4u?*((
int*) _temp1260) == Cyc_Absyn_PointerType: 0){ _LL1267: _temp1266=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1260)->f1; goto _LL1263;} else{ goto
_LL1264;} _LL1264: goto _LL1265; _LL1263: Cyc_Tcutil_unchecked_cast( te, e, Cyc_Absyn_uint_t);
goto _LL1261; _LL1265: return 0; _LL1261:;} return 1;} int Cyc_Tcutil_is_integral_type(
void* t){ void* _temp1268= Cyc_Tcutil_compress( t); void* _temp1276; void*
_temp1278; struct Cyc_Absyn_Enumdecl* _temp1280; struct _tuple1* _temp1282;
_LL1270: if(( unsigned int) _temp1268 > 4u?*(( int*) _temp1268) == Cyc_Absyn_IntType:
0){ _LL1279: _temp1278=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1268)->f1;
goto _LL1277; _LL1277: _temp1276=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1268)->f2; goto _LL1271;} else{ goto _LL1272;} _LL1272: if(( unsigned int)
_temp1268 > 4u?*(( int*) _temp1268) == Cyc_Absyn_EnumType: 0){ _LL1283:
_temp1282=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*) _temp1268)->f1;
goto _LL1281; _LL1281: _temp1280=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*)
_temp1268)->f2; goto _LL1273;} else{ goto _LL1274;} _LL1274: goto _LL1275;
_LL1271: return 1; _LL1273: return 1; _LL1275: return 0; _LL1269:;} int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)({
struct Cyc_Core_Opt* _temp1284= e->topt; if( _temp1284 == 0){ _throw(
Null_Exception);} _temp1284->v;}), Cyc_Absyn_uint_t)){ return 1;} if( Cyc_Tcutil_is_integral_type((
void*)({ struct Cyc_Core_Opt* _temp1285= e->topt; if( _temp1285 == 0){ _throw(
Null_Exception);} _temp1285->v;}))){ if( Cyc_Tcutil_will_lose_precision(( void*)({
struct Cyc_Core_Opt* _temp1286= e->topt; if( _temp1286 == 0){ _throw(
Null_Exception);} _temp1286->v;}), Cyc_Absyn_uint_t)){ Cyc_Tcutil_warn( e->loc,(
struct _tagged_string)({ char* _temp1287=( char*)"integral size mismatch; conversion supplied";
struct _tagged_string _temp1288; _temp1288.curr= _temp1287; _temp1288.base=
_temp1287; _temp1288.last_plus_one= _temp1287 + 44; _temp1288;}));} Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_uint_t); return 1;} return 0;} int Cyc_Tcutil_coerce_sint_typ(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_unify(( void*)({
struct Cyc_Core_Opt* _temp1289= e->topt; if( _temp1289 == 0){ _throw(
Null_Exception);} _temp1289->v;}), Cyc_Absyn_sint_t)){ return 1;} if( Cyc_Tcutil_is_integral_type((
void*)({ struct Cyc_Core_Opt* _temp1290= e->topt; if( _temp1290 == 0){ _throw(
Null_Exception);} _temp1290->v;}))){ if( Cyc_Tcutil_will_lose_precision(( void*)({
struct Cyc_Core_Opt* _temp1291= e->topt; if( _temp1291 == 0){ _throw(
Null_Exception);} _temp1291->v;}), Cyc_Absyn_sint_t)){ Cyc_Tcutil_warn( e->loc,(
struct _tagged_string)({ char* _temp1292=( char*)"integral size mismatch; conversion supplied";
struct _tagged_string _temp1293; _temp1293.curr= _temp1292; _temp1293.base=
_temp1292; _temp1293.last_plus_one= _temp1292 + 44; _temp1293;}));} Cyc_Tcutil_unchecked_cast(
te, e, Cyc_Absyn_sint_t); return 1;} return 0;} int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* t1, void* t2){
t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct _tuple7
_temp1295=({ struct _tuple7 _temp1294; _temp1294.f1= t1; _temp1294.f2= t2;
_temp1294;}); void* _temp1307; struct Cyc_Absyn_PtrInfo _temp1309; void*
_temp1311; struct Cyc_Absyn_PtrInfo _temp1313; void* _temp1315; struct Cyc_Absyn_Exp*
_temp1317; struct Cyc_Absyn_Tqual _temp1319; void* _temp1321; void* _temp1323;
struct Cyc_Absyn_Exp* _temp1325; struct Cyc_Absyn_Tqual _temp1327; void*
_temp1329; void* _temp1331; struct Cyc_Absyn_TunionInfo _temp1333; void*
_temp1335; struct Cyc_List_List* _temp1337; void* _temp1339; struct Cyc_Absyn_Tuniondecl*
_temp1341; void* _temp1343; struct Cyc_Absyn_TunionFieldInfo _temp1345; struct
Cyc_List_List* _temp1347; void* _temp1349; struct Cyc_Absyn_Tunionfield*
_temp1351; struct Cyc_Absyn_Tuniondecl* _temp1353; void* _temp1355; struct Cyc_Absyn_TunionInfo
_temp1357; void* _temp1359; struct Cyc_List_List* _temp1361; void* _temp1363;
struct Cyc_Absyn_Tuniondecl* _temp1365; void* _temp1367; struct Cyc_Absyn_PtrInfo
_temp1369; struct Cyc_Absyn_Conref* _temp1371; struct Cyc_Absyn_Tqual _temp1373;
struct Cyc_Absyn_Conref* _temp1375; void* _temp1377; void* _temp1379; _LL1297:
_LL1312: _temp1311= _temp1295.f1; if(( unsigned int) _temp1311 > 4u?*(( int*)
_temp1311) == Cyc_Absyn_PointerType: 0){ _LL1314: _temp1313=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1311)->f1; goto _LL1308;} else{ goto
_LL1299;} _LL1308: _temp1307= _temp1295.f2; if(( unsigned int) _temp1307 > 4u?*((
int*) _temp1307) == Cyc_Absyn_PointerType: 0){ _LL1310: _temp1309=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1307)->f1; goto _LL1298;} else{ goto
_LL1299;} _LL1299: _LL1324: _temp1323= _temp1295.f1; if(( unsigned int)
_temp1323 > 4u?*(( int*) _temp1323) == Cyc_Absyn_ArrayType: 0){ _LL1330:
_temp1329=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1323)->f1; goto
_LL1328; _LL1328: _temp1327=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1323)->f2; goto _LL1326; _LL1326: _temp1325=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1323)->f3; goto _LL1316;} else{ goto
_LL1301;} _LL1316: _temp1315= _temp1295.f2; if(( unsigned int) _temp1315 > 4u?*((
int*) _temp1315) == Cyc_Absyn_ArrayType: 0){ _LL1322: _temp1321=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1315)->f1; goto _LL1320; _LL1320:
_temp1319=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1315)->f2; goto _LL1318; _LL1318: _temp1317=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1315)->f3; goto _LL1300;} else{ goto
_LL1301;} _LL1301: _LL1344: _temp1343= _temp1295.f1; if(( unsigned int)
_temp1343 > 4u?*(( int*) _temp1343) == Cyc_Absyn_TunionFieldType: 0){ _LL1346:
_temp1345=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp1343)->f1; _LL1350: _temp1349=( void*) _temp1345.field_info; if(*(( int*)
_temp1349) == Cyc_Absyn_KnownTunionfield){ _LL1354: _temp1353=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp1349)->f1; goto _LL1352; _LL1352:
_temp1351=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1349)->f2; goto _LL1348;} else{ goto _LL1303;} _LL1348: _temp1347=( struct
Cyc_List_List*) _temp1345.targs; goto _LL1332;} else{ goto _LL1303;} _LL1332:
_temp1331= _temp1295.f2; if(( unsigned int) _temp1331 > 4u?*(( int*) _temp1331)
== Cyc_Absyn_TunionType: 0){ _LL1334: _temp1333=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp1331)->f1; _LL1340: _temp1339=( void*)
_temp1333.tunion_info; if(*(( int*) _temp1339) == Cyc_Absyn_KnownTunion){
_LL1342: _temp1341=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1339)->f1; goto _LL1338;} else{ goto _LL1303;} _LL1338: _temp1337=( struct
Cyc_List_List*) _temp1333.targs; goto _LL1336; _LL1336: _temp1335=( void*)
_temp1333.rgn; goto _LL1302;} else{ goto _LL1303;} _LL1303: _LL1368: _temp1367=
_temp1295.f1; if(( unsigned int) _temp1367 > 4u?*(( int*) _temp1367) == Cyc_Absyn_PointerType:
0){ _LL1370: _temp1369=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1367)->f1; _LL1380: _temp1379=( void*) _temp1369.elt_typ; goto _LL1378;
_LL1378: _temp1377=( void*) _temp1369.rgn_typ; goto _LL1376; _LL1376: _temp1375=(
struct Cyc_Absyn_Conref*) _temp1369.nullable; goto _LL1374; _LL1374: _temp1373=(
struct Cyc_Absyn_Tqual) _temp1369.tq; goto _LL1372; _LL1372: _temp1371=( struct
Cyc_Absyn_Conref*) _temp1369.bounds; goto _LL1356;} else{ goto _LL1305;} _LL1356:
_temp1355= _temp1295.f2; if(( unsigned int) _temp1355 > 4u?*(( int*) _temp1355)
== Cyc_Absyn_TunionType: 0){ _LL1358: _temp1357=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp1355)->f1; _LL1364: _temp1363=( void*)
_temp1357.tunion_info; if(*(( int*) _temp1363) == Cyc_Absyn_KnownTunion){
_LL1366: _temp1365=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_KnownTunion_struct*)
_temp1363)->f1; goto _LL1362;} else{ goto _LL1305;} _LL1362: _temp1361=( struct
Cyc_List_List*) _temp1357.targs; goto _LL1360; _LL1360: _temp1359=( void*)
_temp1357.rgn; goto _LL1304;} else{ goto _LL1305;} _LL1305: goto _LL1306;
_LL1298: { int okay= 1; if( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref*
x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp,
_temp1313.nullable, _temp1309.nullable)){ void* _temp1381=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp1313.nullable))->v;
int _temp1387; _LL1383: if(( unsigned int) _temp1381 > 1u?*(( int*) _temp1381)
== Cyc_Absyn_Eq_constr: 0){ _LL1388: _temp1387=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp1381)->f1; goto _LL1384;} else{ goto _LL1385;} _LL1385: goto _LL1386;
_LL1384: okay= ! _temp1387; goto _LL1382; _LL1386:( void) _throw((( void*(*)(
struct _tagged_string msg)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp1389=( char*)"silent_castable conref not eq"; struct _tagged_string
_temp1390; _temp1390.curr= _temp1389; _temp1390.base= _temp1389; _temp1390.last_plus_one=
_temp1389 + 30; _temp1390;}))); _LL1382:;} if( !(( int(*)( int(* cmp)( void*,
void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)(
Cyc_Tcutil_boundscmp, _temp1313.bounds, _temp1309.bounds)){ struct _tuple7
_temp1392=({ struct _tuple7 _temp1391; _temp1391.f1=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp1313.bounds))->v;
_temp1391.f2=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)( _temp1309.bounds))->v; _temp1391;}); void* _temp1400;
void* _temp1402; void* _temp1404; void* _temp1406; struct Cyc_Absyn_Exp*
_temp1408; void* _temp1410; void* _temp1412; void* _temp1414; void* _temp1416;
_LL1394: _LL1405: _temp1404= _temp1392.f1; if(( unsigned int) _temp1404 > 1u?*((
int*) _temp1404) == Cyc_Absyn_Eq_constr: 0){ _LL1407: _temp1406=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp1404)->f1; if(( unsigned int) _temp1406
> 1u?*(( int*) _temp1406) == Cyc_Absyn_Upper_b: 0){ _LL1409: _temp1408=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp1406)->f1; goto _LL1401;}
else{ goto _LL1396;}} else{ goto _LL1396;} _LL1401: _temp1400= _temp1392.f2; if((
unsigned int) _temp1400 > 1u?*(( int*) _temp1400) == Cyc_Absyn_Eq_constr: 0){
_LL1403: _temp1402=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1400)->f1;
if( _temp1402 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1395;} else{ goto _LL1396;}}
else{ goto _LL1396;} _LL1396: _LL1415: _temp1414= _temp1392.f1; if((
unsigned int) _temp1414 > 1u?*(( int*) _temp1414) == Cyc_Absyn_Eq_constr: 0){
_LL1417: _temp1416=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1414)->f1;
if( _temp1416 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1411;} else{ goto _LL1398;}}
else{ goto _LL1398;} _LL1411: _temp1410= _temp1392.f2; if(( unsigned int)
_temp1410 > 1u?*(( int*) _temp1410) == Cyc_Absyn_Eq_constr: 0){ _LL1413:
_temp1412=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1410)->f1; if(
_temp1412 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL1397;} else{ goto _LL1398;}}
else{ goto _LL1398;} _LL1398: goto _LL1399; _LL1395: okay= 1; goto _LL1393;
_LL1397: okay= 1; goto _LL1393; _LL1399: okay= 0; goto _LL1393; _LL1393:;}
return(( okay? Cyc_Tcutil_unify(( void*) _temp1313.elt_typ,( void*) _temp1309.elt_typ):
0)? Cyc_Tcutil_unify(( void*) _temp1313.rgn_typ,( void*) _temp1309.rgn_typ): 0)?
!( _temp1313.tq).q_const? 1:( _temp1309.tq).q_const: 0;} _LL1300: { int okay;
okay=( _temp1325 != 0? _temp1317 != 0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1418= _temp1325; if(
_temp1418 == 0){ _throw( Null_Exception);} _temp1418;})) == Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1419= _temp1317; if(
_temp1419 == 0){ _throw( Null_Exception);} _temp1419;})): 0; return( okay? Cyc_Tcutil_unify(
_temp1329, _temp1321): 0)? ! _temp1327.q_const? 1: _temp1319.q_const: 0;}
_LL1302: if(( _temp1353 == _temp1341? 1: Cyc_Absyn_qvar_cmp( _temp1353->name,
_temp1341->name) == 0)? _temp1351->typs == 0: 0){ for( 0; _temp1347 != 0?
_temp1337 != 0: 0; _temp1347=({ struct Cyc_List_List* _temp1420= _temp1347; if(
_temp1420 == 0){ _throw( Null_Exception);} _temp1420->tl;}), _temp1337=({ struct
Cyc_List_List* _temp1421= _temp1337; if( _temp1421 == 0){ _throw( Null_Exception);}
_temp1421->tl;})){ if( ! Cyc_Tcutil_unify(( void*)({ struct Cyc_List_List*
_temp1422= _temp1347; if( _temp1422 == 0){ _throw( Null_Exception);} _temp1422->hd;}),(
void*)({ struct Cyc_List_List* _temp1423= _temp1337; if( _temp1423 == 0){ _throw(
Null_Exception);} _temp1423->hd;}))){ break;}} if( _temp1347 == 0? _temp1337 ==
0: 0){ return 1;}} return 0; _LL1304:{ void* _temp1424= Cyc_Tcutil_compress(
_temp1379); struct Cyc_Absyn_TunionFieldInfo _temp1430; struct Cyc_List_List*
_temp1432; void* _temp1434; struct Cyc_Absyn_Tunionfield* _temp1436; struct Cyc_Absyn_Tuniondecl*
_temp1438; _LL1426: if(( unsigned int) _temp1424 > 4u?*(( int*) _temp1424) ==
Cyc_Absyn_TunionFieldType: 0){ _LL1431: _temp1430=( struct Cyc_Absyn_TunionFieldInfo)((
struct Cyc_Absyn_TunionFieldType_struct*) _temp1424)->f1; _LL1435: _temp1434=(
void*) _temp1430.field_info; if(*(( int*) _temp1434) == Cyc_Absyn_KnownTunionfield){
_LL1439: _temp1438=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1434)->f1; goto _LL1437; _LL1437: _temp1436=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp1434)->f2; goto _LL1433;} else{
goto _LL1428;} _LL1433: _temp1432=( struct Cyc_List_List*) _temp1430.targs; goto
_LL1427;} else{ goto _LL1428;} _LL1428: goto _LL1429; _LL1427: if( ! Cyc_Tcutil_unify(
_temp1377, _temp1359)? _temp1377 !=( void*) Cyc_Absyn_HeapRgn: 0){ return 0;}
if( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp, _temp1375,(( struct Cyc_Absyn_Conref*(*)(
int x)) Cyc_Absyn_new_conref)( 0))){ return 0;} if( !(( int(*)( int(* cmp)( void*,
void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)(
Cyc_Tcutil_boundscmp, _temp1371,(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
Cyc_Absyn_bounds_one))){ return 0;} if( Cyc_Absyn_qvar_cmp( _temp1365->name,
_temp1438->name) == 0? _temp1436->typs != 0: 0){ int okay= 1; for( 0; _temp1432
!= 0? _temp1361 != 0: 0; _temp1432=({ struct Cyc_List_List* _temp1440= _temp1432;
if( _temp1440 == 0){ _throw( Null_Exception);} _temp1440->tl;}), _temp1361=({
struct Cyc_List_List* _temp1441= _temp1361; if( _temp1441 == 0){ _throw(
Null_Exception);} _temp1441->tl;})){ if( ! Cyc_Tcutil_unify(( void*)({ struct
Cyc_List_List* _temp1442= _temp1432; if( _temp1442 == 0){ _throw( Null_Exception);}
_temp1442->hd;}),( void*)({ struct Cyc_List_List* _temp1443= _temp1361; if(
_temp1443 == 0){ _throw( Null_Exception);} _temp1443->hd;}))){ okay= 0; break;}}
if(( ! okay? 1: _temp1432 != 0)? 1: _temp1361 != 0){ return 0;} return 1;} goto
_LL1425; _LL1429: goto _LL1425; _LL1425:;} return 0; _LL1306: return Cyc_Tcutil_unify(
t1, t2); _LL1296:;}} int Cyc_Tcutil_coerce_arg( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Exp* e, void* t2){ void* t1= Cyc_Tcutil_compress(( void*)({
struct Cyc_Core_Opt* _temp1448= e->topt; if( _temp1448 == 0){ _throw(
Null_Exception);} _temp1448->v;})); if( Cyc_Tcutil_unify( t1, t2)){ return 1;}
if( Cyc_Tcutil_is_arithmetic_type( t1)? Cyc_Tcutil_is_arithmetic_type( t2): 0){
if( Cyc_Tcutil_will_lose_precision( t1, t2)){ Cyc_Tcutil_warn( e->loc,( struct
_tagged_string)({ char* _temp1444=( char*)"integral size mismatch; conversion supplied";
struct _tagged_string _temp1445; _temp1445.curr= _temp1444; _temp1445.base=
_temp1444; _temp1445.last_plus_one= _temp1444 + 44; _temp1445;}));} Cyc_Tcutil_unchecked_cast(
te, e, t2); return 1;} else{ if( Cyc_Tcutil_silent_castable( te, e->loc, t1, t2)){
Cyc_Tcutil_unchecked_cast( te, e, t2); return 1;} else{ if( Cyc_Tcutil_castable(
te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te, e, t2); Cyc_Tcutil_warn( e->loc,({
struct _tagged_string _temp1446= Cyc_Absynpp_typ2string( t1); struct
_tagged_string _temp1447= Cyc_Absynpp_typ2string( t2); xprintf("implicit cast from %.*s to %.*s",
_temp1446.last_plus_one - _temp1446.curr, _temp1446.curr, _temp1447.last_plus_one
- _temp1447.curr, _temp1447.curr);})); return 1;} else{ return 0;}}}} int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, void* t){ return Cyc_Tcutil_coerce_arg(
te, e, t);} static int Cyc_Tcutil_any_promote( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Exp* e){ void* old_typ= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp1479= e->topt; if( _temp1479 == 0){ _throw( Null_Exception);} _temp1479->v;}));
void* t;{ void* _temp1449= old_typ; struct Cyc_Absyn_Enumdecl* _temp1461; struct
_tuple1* _temp1463; void* _temp1465; void* _temp1467; _LL1451: if(( unsigned int)
_temp1449 > 4u?*(( int*) _temp1449) == Cyc_Absyn_EnumType: 0){ _LL1464:
_temp1463=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*) _temp1449)->f1;
goto _LL1462; _LL1462: _temp1461=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*)
_temp1449)->f2; goto _LL1452;} else{ goto _LL1453;} _LL1453: if(( unsigned int)
_temp1449 > 4u?*(( int*) _temp1449) == Cyc_Absyn_IntType: 0){ _LL1468: _temp1467=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1449)->f1; goto _LL1466; _LL1466:
_temp1465=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1449)->f2; goto
_LL1454;} else{ goto _LL1455;} _LL1455: if( _temp1449 ==( void*) Cyc_Absyn_FloatType){
goto _LL1456;} else{ goto _LL1457;} _LL1457: if( _temp1449 ==( void*) Cyc_Absyn_DoubleType){
goto _LL1458;} else{ goto _LL1459;} _LL1459: goto _LL1460; _LL1452: t= Cyc_Absyn_sint_t;
goto _LL1450; _LL1454:{ void* _temp1469= _temp1465; _LL1471: if( _temp1469 ==(
void*) Cyc_Absyn_B1){ goto _LL1472;} else{ goto _LL1473;} _LL1473: if( _temp1469
==( void*) Cyc_Absyn_B2){ goto _LL1474;} else{ goto _LL1475;} _LL1475: if(
_temp1469 ==( void*) Cyc_Absyn_B4){ goto _LL1476;} else{ goto _LL1477;} _LL1477:
if( _temp1469 ==( void*) Cyc_Absyn_B8){ goto _LL1478;} else{ goto _LL1470;}
_LL1472: t= Cyc_Absyn_sint_t; goto _LL1470; _LL1474: t= Cyc_Absyn_sint_t; goto
_LL1470; _LL1476: t= _temp1467 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t:
Cyc_Absyn_sint_t; goto _LL1470; _LL1478: t= _temp1467 ==( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_ulong_t: Cyc_Absyn_slong_t; goto _LL1470; _LL1470:;} goto _LL1450;
_LL1456: t= Cyc_Absyn_float_t; goto _LL1450; _LL1458: t= Cyc_Absyn_double_t;
goto _LL1450; _LL1460: return 0; _LL1450:;} Cyc_Tcutil_unchecked_cast( te, e, t);
return 1;} int Cyc_Tcutil_coerce_use( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, void* t2){ void* t1= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp1483= e->topt; if( _temp1483 == 0){ _throw( Null_Exception);} _temp1483->v;}));
if( Cyc_Tcutil_unify( t1, t2)){ return 1;} Cyc_Tcutil_any_promote( te, e); if(
Cyc_Tcutil_unify(( void*)({ struct Cyc_Core_Opt* _temp1480= e->topt; if(
_temp1480 == 0){ _throw( Null_Exception);} _temp1480->v;}), t2)){ return 1;}
else{ if( Cyc_Tcutil_silent_castable( te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast(
te, e, t2); return 1;} else{ if( Cyc_Tcutil_castable( te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast(
te, e, t2); Cyc_Tcutil_warn( e->loc,({ struct _tagged_string _temp1481= Cyc_Absynpp_typ2string(
t1); struct _tagged_string _temp1482= Cyc_Absynpp_typ2string( t2); xprintf("implicit cast from %.*s to %.*s",
_temp1481.last_plus_one - _temp1481.curr, _temp1481.curr, _temp1482.last_plus_one
- _temp1482.curr, _temp1482.curr);})); return 1;} else{ return 0;}}}} int Cyc_Tcutil_coerceable(
void* t){ void* _temp1484= Cyc_Tcutil_compress( t); void* _temp1494; void*
_temp1496; _LL1486: if(( unsigned int) _temp1484 > 4u?*(( int*) _temp1484) ==
Cyc_Absyn_IntType: 0){ _LL1497: _temp1496=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1484)->f1; goto _LL1495; _LL1495: _temp1494=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1484)->f2; goto _LL1487;} else{ goto _LL1488;} _LL1488: if( _temp1484 ==(
void*) Cyc_Absyn_FloatType){ goto _LL1489;} else{ goto _LL1490;} _LL1490: if(
_temp1484 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1491;} else{ goto _LL1492;}
_LL1492: goto _LL1493; _LL1487: return 1; _LL1489: return 1; _LL1491: return 1;
_LL1493: return 0; _LL1485:;} static struct _tuple4* Cyc_Tcutil_flatten_typ_f(
struct Cyc_List_List* inst, struct Cyc_Absyn_Structfield* x){ return({ struct
_tuple4* _temp1498=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1498->f1= x->tq; _temp1498->f2= Cyc_Tcutil_substitute( inst,( void*) x->type);
_temp1498;});} static struct Cyc_List_List* Cyc_Tcutil_flatten_typ( struct Cyc_Tcenv_Tenv*
te, void* t1){ t1= Cyc_Tcutil_compress( t1);{ void* _temp1499= t1; struct Cyc_List_List*
_temp1509; struct Cyc_Absyn_Structdecl** _temp1511; struct Cyc_List_List*
_temp1513; struct _tuple1* _temp1515; _LL1501: if( _temp1499 ==( void*) Cyc_Absyn_VoidType){
goto _LL1502;} else{ goto _LL1503;} _LL1503: if(( unsigned int) _temp1499 > 4u?*((
int*) _temp1499) == Cyc_Absyn_TupleType: 0){ _LL1510: _temp1509=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp1499)->f1; goto _LL1504;} else{ goto
_LL1505;} _LL1505: if(( unsigned int) _temp1499 > 4u?*(( int*) _temp1499) == Cyc_Absyn_StructType:
0){ _LL1516: _temp1515=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp1499)->f1; goto _LL1514; _LL1514: _temp1513=( struct Cyc_List_List*)((
struct Cyc_Absyn_StructType_struct*) _temp1499)->f2; goto _LL1512; _LL1512:
_temp1511=( struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*)
_temp1499)->f3; goto _LL1506;} else{ goto _LL1507;} _LL1507: goto _LL1508;
_LL1502: return 0; _LL1504: return _temp1509; _LL1506: if( _temp1511 == 0? 1:
_temp1515 == 0){ return({ struct Cyc_List_List* _temp1517=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1517->hd=( void*)({ struct
_tuple4* _temp1518=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp1518->f1= Cyc_Absyn_empty_tqual(); _temp1518->f2= t1; _temp1518;});
_temp1517->tl= 0; _temp1517;});}{ struct Cyc_Absyn_Structdecl* sd=({ struct Cyc_Absyn_Structdecl**
_temp1523= _temp1511; if( _temp1523 == 0){ _throw( Null_Exception);}* _temp1523;});
struct _tuple1 n=({ struct _tuple1* _temp1522= _temp1515; if( _temp1522 == 0){
_throw( Null_Exception);}* _temp1522;}); struct Cyc_List_List* inst=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
sd->tvs, _temp1513); if( sd->fields == 0){ return({ struct Cyc_List_List*
_temp1519=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1519->hd=( void*)({ struct _tuple4* _temp1520=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp1520->f1= Cyc_Absyn_empty_tqual(); _temp1520->f2=
t1; _temp1520;}); _temp1519->tl= 0; _temp1519;});} return(( struct Cyc_List_List*(*)(
struct _tuple4*(* f)( struct Cyc_List_List*, struct Cyc_Absyn_Structfield*),
struct Cyc_List_List* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcutil_flatten_typ_f,
inst,( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp1521= sd->fields; if(
_temp1521 == 0){ _throw( Null_Exception);} _temp1521->v;}));} _LL1508: return({
struct Cyc_List_List* _temp1524=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1524->hd=( void*)({ struct _tuple4* _temp1525=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1525->f1= Cyc_Absyn_empty_tqual();
_temp1525->f2= t1; _temp1525;}); _temp1524->tl= 0; _temp1524;}); _LL1500:;}}
static int Cyc_Tcutil_ptrsubtype( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
assume, void* t1, void* t2); static int Cyc_Tcutil_subtype( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* assume, void* t1, void* t2){ if( Cyc_Tcutil_unify( t1,
t2)){ return 1;}{ struct Cyc_List_List* a= assume; for( 0; a != 0; a=({ struct
Cyc_List_List* _temp1526= a; if( _temp1526 == 0){ _throw( Null_Exception);}
_temp1526->tl;})){ if( Cyc_Tcutil_unify( t1,(*(( struct _tuple7*)({ struct Cyc_List_List*
_temp1527= a; if( _temp1527 == 0){ _throw( Null_Exception);} _temp1527->hd;}))).f1)?
Cyc_Tcutil_unify( t2,(*(( struct _tuple7*)({ struct Cyc_List_List* _temp1528= a;
if( _temp1528 == 0){ _throw( Null_Exception);} _temp1528->hd;}))).f2): 0){
return 1;}}} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress( t2);{ struct
_tuple7 _temp1530=({ struct _tuple7 _temp1529; _temp1529.f1= t1; _temp1529.f2=
t2; _temp1529;}); void* _temp1536; struct Cyc_Absyn_PtrInfo _temp1538; struct
Cyc_Absyn_Conref* _temp1540; struct Cyc_Absyn_Tqual _temp1542; struct Cyc_Absyn_Conref*
_temp1544; void* _temp1546; void* _temp1548; void* _temp1550; struct Cyc_Absyn_PtrInfo
_temp1552; struct Cyc_Absyn_Conref* _temp1554; struct Cyc_Absyn_Tqual _temp1556;
struct Cyc_Absyn_Conref* _temp1558; void* _temp1560; void* _temp1562; _LL1532:
_LL1551: _temp1550= _temp1530.f1; if(( unsigned int) _temp1550 > 4u?*(( int*)
_temp1550) == Cyc_Absyn_PointerType: 0){ _LL1553: _temp1552=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1550)->f1; _LL1563: _temp1562=( void*)
_temp1552.elt_typ; goto _LL1561; _LL1561: _temp1560=( void*) _temp1552.rgn_typ;
goto _LL1559; _LL1559: _temp1558=( struct Cyc_Absyn_Conref*) _temp1552.nullable;
goto _LL1557; _LL1557: _temp1556=( struct Cyc_Absyn_Tqual) _temp1552.tq; goto
_LL1555; _LL1555: _temp1554=( struct Cyc_Absyn_Conref*) _temp1552.bounds; goto
_LL1537;} else{ goto _LL1534;} _LL1537: _temp1536= _temp1530.f2; if((
unsigned int) _temp1536 > 4u?*(( int*) _temp1536) == Cyc_Absyn_PointerType: 0){
_LL1539: _temp1538=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1536)->f1; _LL1549: _temp1548=( void*) _temp1538.elt_typ; goto _LL1547;
_LL1547: _temp1546=( void*) _temp1538.rgn_typ; goto _LL1545; _LL1545: _temp1544=(
struct Cyc_Absyn_Conref*) _temp1538.nullable; goto _LL1543; _LL1543: _temp1542=(
struct Cyc_Absyn_Tqual) _temp1538.tq; goto _LL1541; _LL1541: _temp1540=( struct
Cyc_Absyn_Conref*) _temp1538.bounds; goto _LL1533;} else{ goto _LL1534;} _LL1534:
goto _LL1535; _LL1533: if( _temp1556.q_const? ! _temp1542.q_const: 0){ return 0;}
if(( !(( int(*)( int(* cmp)( int, int), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y)) Cyc_Tcutil_unify_conrefs)( Cyc_Core_intcmp, _temp1558, _temp1544)?(( int(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp1558): 0)? !(( int(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp1544): 0){ return 0;}{
void* _temp1564= _temp1560; _LL1566: if( _temp1564 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL1567;} else{ goto _LL1568;} _LL1568: goto _LL1569; _LL1567: goto _LL1565;
_LL1569: Cyc_Tcutil_unify( _temp1560, _temp1546); goto _LL1565; _LL1565:;} if( !((
int(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x, struct Cyc_Absyn_Conref*
y)) Cyc_Tcutil_unify_conrefs)( Cyc_Tcutil_boundscmp, _temp1554, _temp1540)){
struct _tuple7 _temp1571=({ struct _tuple7 _temp1570; _temp1570.f1=(( void*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp1554); _temp1570.f2=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp1540);
_temp1570;}); void* _temp1579; void* _temp1581; struct Cyc_Absyn_Exp* _temp1583;
void* _temp1585; struct Cyc_Absyn_Exp* _temp1587; void* _temp1589; struct Cyc_Absyn_Exp*
_temp1591; _LL1573: _LL1582: _temp1581= _temp1571.f1; if(( unsigned int)
_temp1581 > 1u?*(( int*) _temp1581) == Cyc_Absyn_Upper_b: 0){ _LL1584: _temp1583=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp1581)->f1; goto
_LL1580;} else{ goto _LL1575;} _LL1580: _temp1579= _temp1571.f2; if( _temp1579
==( void*) Cyc_Absyn_Unknown_b){ goto _LL1574;} else{ goto _LL1575;} _LL1575:
_LL1590: _temp1589= _temp1571.f1; if(( unsigned int) _temp1589 > 1u?*(( int*)
_temp1589) == Cyc_Absyn_Upper_b: 0){ _LL1592: _temp1591=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Upper_b_struct*) _temp1589)->f1; goto _LL1586;} else{ goto
_LL1577;} _LL1586: _temp1585= _temp1571.f2; if(( unsigned int) _temp1585 > 1u?*((
int*) _temp1585) == Cyc_Absyn_Upper_b: 0){ _LL1588: _temp1587=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Upper_b_struct*) _temp1585)->f1; goto _LL1576;} else{ goto
_LL1577;} _LL1577: goto _LL1578; _LL1574: goto _LL1572; _LL1576: if( Cyc_Evexp_eval_const_uint_exp(
_temp1591) < Cyc_Evexp_eval_const_uint_exp( _temp1587)){ return 0;} goto _LL1572;
_LL1578: return 0; _LL1572:;} return Cyc_Tcutil_ptrsubtype( te,({ struct Cyc_List_List*
_temp1593=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1593->hd=( void*)({ struct _tuple7* _temp1594=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp1594->f1= t1; _temp1594->f2= t2; _temp1594;});
_temp1593->tl= assume; _temp1593;}), _temp1562, _temp1548); _LL1535: return 0;
_LL1531:;}} static int Cyc_Tcutil_ptrsubtype( struct Cyc_Tcenv_Tenv* te, struct
Cyc_List_List* assume, void* t1, void* t2){ struct Cyc_List_List* tqs1= Cyc_Tcutil_flatten_typ(
te, t1); struct Cyc_List_List* tqs2= Cyc_Tcutil_flatten_typ( te, t2); for( 0;
tqs2 != 0; tqs2=({ struct Cyc_List_List* _temp1595= tqs2; if( _temp1595 == 0){
_throw( Null_Exception);} _temp1595->tl;}), tqs1=({ struct Cyc_List_List*
_temp1596= tqs1; if( _temp1596 == 0){ _throw( Null_Exception);} _temp1596->tl;})){
if( tqs1 == 0){ return 0;}{ struct _tuple4 _temp1600; void* _temp1601; struct
Cyc_Absyn_Tqual _temp1603; struct _tuple4* _temp1598=( struct _tuple4*)({ struct
Cyc_List_List* _temp1597= tqs1; if( _temp1597 == 0){ _throw( Null_Exception);}
_temp1597->hd;}); _temp1600=* _temp1598; _LL1604: _temp1603= _temp1600.f1; goto
_LL1602; _LL1602: _temp1601= _temp1600.f2; goto _LL1599; _LL1599: { struct
_tuple4 _temp1608; void* _temp1609; struct Cyc_Absyn_Tqual _temp1611; struct
_tuple4* _temp1606=( struct _tuple4*)({ struct Cyc_List_List* _temp1605= tqs2;
if( _temp1605 == 0){ _throw( Null_Exception);} _temp1605->hd;}); _temp1608=*
_temp1606; _LL1612: _temp1611= _temp1608.f1; goto _LL1610; _LL1610: _temp1609=
_temp1608.f2; goto _LL1607; _LL1607: if( _temp1611.q_const? Cyc_Tcutil_subtype(
te, assume, _temp1601, _temp1609): 0){ continue;} else{ if( Cyc_Tcutil_unify(
_temp1601, _temp1609)){ continue;} else{ return 0;}}}}} return 1;} int Cyc_Tcutil_castable(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* t1, void* t2){
if( Cyc_Tcutil_unify( t1, t2)){ return 1;} t1= Cyc_Tcutil_compress( t1); t2= Cyc_Tcutil_compress(
t2);{ void* _temp1613= t2; struct Cyc_Absyn_Uniondecl** _temp1621; struct Cyc_List_List*
_temp1623; struct _tuple1* _temp1625; void* _temp1627; void* _temp1629; _LL1615:
if(( unsigned int) _temp1613 > 4u?*(( int*) _temp1613) == Cyc_Absyn_UnionType: 0){
_LL1626: _temp1625=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp1613)->f1; goto _LL1624; _LL1624: _temp1623=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnionType_struct*) _temp1613)->f2; goto _LL1622; _LL1622:
_temp1621=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp1613)->f3; goto _LL1616;} else{ goto _LL1617;} _LL1617: if(( unsigned int)
_temp1613 > 4u?*(( int*) _temp1613) == Cyc_Absyn_IntType: 0){ _LL1630: _temp1629=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1613)->f1; goto _LL1628; _LL1628:
_temp1627=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1613)->f2; if(
_temp1627 ==( void*) Cyc_Absyn_B4){ goto _LL1618;} else{ goto _LL1619;}} else{
goto _LL1619;} _LL1619: goto _LL1620; _LL1616: if( _temp1621 == 0){ goto _LL1614;}{
struct Cyc_Absyn_Uniondecl* ud=({ struct Cyc_Absyn_Uniondecl** _temp1634=
_temp1621; if( _temp1634 == 0){ _throw( Null_Exception);}* _temp1634;}); if( ud->fields
== 0){ goto _LL1614;}{ struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( ud->tvs,
_temp1623);{ struct Cyc_List_List* fs=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp1633= ud->fields; if( _temp1633 == 0){ _throw( Null_Exception);} _temp1633->v;});
for( 0; fs != 0; fs=({ struct Cyc_List_List* _temp1631= fs; if( _temp1631 == 0){
_throw( Null_Exception);} _temp1631->tl;})){ if( Cyc_Tcutil_unify( t1, Cyc_Tcutil_substitute(
inst,( void*)(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp1632=
fs; if( _temp1632 == 0){ _throw( Null_Exception);} _temp1632->hd;}))->type))){
return 1;}}} goto _LL1614;}} _LL1618: if( Cyc_Tcutil_typ_kind( t2) ==( void*)
Cyc_Absyn_BoxKind){ return 1;} goto _LL1614; _LL1620: goto _LL1614; _LL1614:;}{
void* _temp1635= t1; struct Cyc_Absyn_PtrInfo _temp1651; struct Cyc_Absyn_Conref*
_temp1653; struct Cyc_Absyn_Tqual _temp1655; struct Cyc_Absyn_Conref* _temp1657;
void* _temp1659; void* _temp1661; struct Cyc_Absyn_Exp* _temp1663; struct Cyc_Absyn_Tqual
_temp1665; void* _temp1667; struct Cyc_Absyn_Enumdecl* _temp1669; struct _tuple1*
_temp1671; void* _temp1673; void* _temp1675; _LL1637: if(( unsigned int)
_temp1635 > 4u?*(( int*) _temp1635) == Cyc_Absyn_PointerType: 0){ _LL1652:
_temp1651=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1635)->f1; _LL1662: _temp1661=( void*) _temp1651.elt_typ; goto _LL1660;
_LL1660: _temp1659=( void*) _temp1651.rgn_typ; goto _LL1658; _LL1658: _temp1657=(
struct Cyc_Absyn_Conref*) _temp1651.nullable; goto _LL1656; _LL1656: _temp1655=(
struct Cyc_Absyn_Tqual) _temp1651.tq; goto _LL1654; _LL1654: _temp1653=( struct
Cyc_Absyn_Conref*) _temp1651.bounds; goto _LL1638;} else{ goto _LL1639;} _LL1639:
if(( unsigned int) _temp1635 > 4u?*(( int*) _temp1635) == Cyc_Absyn_ArrayType: 0){
_LL1668: _temp1667=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1635)->f1;
goto _LL1666; _LL1666: _temp1665=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1635)->f2; goto _LL1664; _LL1664: _temp1663=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1635)->f3; goto _LL1640;} else{ goto
_LL1641;} _LL1641: if(( unsigned int) _temp1635 > 4u?*(( int*) _temp1635) == Cyc_Absyn_EnumType:
0){ _LL1672: _temp1671=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*)
_temp1635)->f1; goto _LL1670; _LL1670: _temp1669=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_EnumType_struct*) _temp1635)->f2; goto _LL1642;} else{ goto
_LL1643;} _LL1643: if(( unsigned int) _temp1635 > 4u?*(( int*) _temp1635) == Cyc_Absyn_IntType:
0){ _LL1676: _temp1675=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1635)->f1;
goto _LL1674; _LL1674: _temp1673=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1635)->f2; goto _LL1644;} else{ goto _LL1645;} _LL1645: if( _temp1635 ==(
void*) Cyc_Absyn_FloatType){ goto _LL1646;} else{ goto _LL1647;} _LL1647: if(
_temp1635 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1648;} else{ goto _LL1649;}
_LL1649: goto _LL1650; _LL1638:{ void* _temp1677= t2; struct Cyc_Absyn_PtrInfo
_temp1683; struct Cyc_Absyn_Conref* _temp1685; struct Cyc_Absyn_Tqual _temp1687;
struct Cyc_Absyn_Conref* _temp1689; void* _temp1691; void* _temp1693; _LL1679:
if(( unsigned int) _temp1677 > 4u?*(( int*) _temp1677) == Cyc_Absyn_PointerType:
0){ _LL1684: _temp1683=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1677)->f1; _LL1694: _temp1693=( void*) _temp1683.elt_typ; goto _LL1692;
_LL1692: _temp1691=( void*) _temp1683.rgn_typ; goto _LL1690; _LL1690: _temp1689=(
struct Cyc_Absyn_Conref*) _temp1683.nullable; goto _LL1688; _LL1688: _temp1687=(
struct Cyc_Absyn_Tqual) _temp1683.tq; goto _LL1686; _LL1686: _temp1685=( struct
Cyc_Absyn_Conref*) _temp1683.bounds; goto _LL1680;} else{ goto _LL1681;} _LL1681:
goto _LL1682; _LL1680: { struct Cyc_List_List* assump=({ struct Cyc_List_List*
_temp1710=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1710->hd=( void*)({ struct _tuple7* _temp1711=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp1711->f1= t1; _temp1711->f2= t2; _temp1711;});
_temp1710->tl= 0; _temp1710;}); int ptrsub= Cyc_Tcutil_ptrsubtype( te, assump,
_temp1661, _temp1693)? ! _temp1655.q_const? 1: _temp1687.q_const: 0; int
bounds_ok=(( int(*)( int(* cmp)( void*, void*), struct Cyc_Absyn_Conref* x,
struct Cyc_Absyn_Conref* y)) Cyc_Tcutil_unify_conrefs)( Cyc_Tcutil_boundscmp,
_temp1653, _temp1685); if( ! bounds_ok){ struct _tuple7 _temp1696=({ struct
_tuple7 _temp1695; _temp1695.f1=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1653); _temp1695.f2=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1685); _temp1695;}); void* _temp1702; struct Cyc_Absyn_Exp* _temp1704; void*
_temp1706; struct Cyc_Absyn_Exp* _temp1708; _LL1698: _LL1707: _temp1706=
_temp1696.f1; if(( unsigned int) _temp1706 > 1u?*(( int*) _temp1706) == Cyc_Absyn_Upper_b:
0){ _LL1709: _temp1708=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp1706)->f1; goto _LL1703;} else{ goto _LL1700;} _LL1703: _temp1702=
_temp1696.f2; if(( unsigned int) _temp1702 > 1u?*(( int*) _temp1702) == Cyc_Absyn_Upper_b:
0){ _LL1705: _temp1704=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp1702)->f1; goto _LL1699;} else{ goto _LL1700;} _LL1700: goto _LL1701;
_LL1699: if( Cyc_Evexp_eval_const_uint_exp( _temp1708) >= Cyc_Evexp_eval_const_uint_exp(
_temp1704)){ bounds_ok= 1;} goto _LL1697; _LL1701: bounds_ok= 1; goto _LL1697;
_LL1697:;} return( bounds_ok? ptrsub: 0)? _temp1659 ==( void*) Cyc_Absyn_HeapRgn?
1: Cyc_Tcutil_unify( _temp1659, _temp1691): 0;} _LL1682: goto _LL1678; _LL1678:;}
return 0; _LL1640:{ void* _temp1712= t2; struct Cyc_Absyn_Exp* _temp1718; struct
Cyc_Absyn_Tqual _temp1720; void* _temp1722; _LL1714: if(( unsigned int)
_temp1712 > 4u?*(( int*) _temp1712) == Cyc_Absyn_ArrayType: 0){ _LL1723:
_temp1722=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1712)->f1; goto
_LL1721; _LL1721: _temp1720=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1712)->f2; goto _LL1719; _LL1719: _temp1718=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1712)->f3; goto _LL1715;} else{ goto
_LL1716;} _LL1716: goto _LL1717; _LL1715: { int okay; okay=( _temp1663 != 0?
_temp1718 != 0: 0)? Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1724= _temp1663; if( _temp1724 == 0){ _throw(
Null_Exception);} _temp1724;})) >= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1725= _temp1718; if( _temp1725 == 0){ _throw(
Null_Exception);} _temp1725;})): 0; return( okay? Cyc_Tcutil_unify( _temp1667,
_temp1722): 0)? ! _temp1665.q_const? 1: _temp1720.q_const: 0;} _LL1717: return 0;
_LL1713:;} return 0; _LL1642: goto _LL1644; _LL1644: goto _LL1646; _LL1646: goto
_LL1648; _LL1648: return Cyc_Tcutil_coerceable( t2); _LL1650: return 0; _LL1636:;}}
void Cyc_Tcutil_unchecked_cast( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, void* t){ if( ! Cyc_Tcutil_unify(( void*)({ struct Cyc_Core_Opt* _temp1726= e->topt;
if( _temp1726 == 0){ _throw( Null_Exception);} _temp1726->v;}), t)){ struct Cyc_Absyn_Exp*
inner= Cyc_Absyn_copy_exp( e);( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Cast_e_struct*
_temp1727=( struct Cyc_Absyn_Cast_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct));
_temp1727[ 0]=({ struct Cyc_Absyn_Cast_e_struct _temp1728; _temp1728.tag= Cyc_Absyn_Cast_e;
_temp1728.f1=( void*) t; _temp1728.f2= inner; _temp1728;}); _temp1727;}))); e->topt=({
struct Cyc_Core_Opt* _temp1729=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1729->v=( void*) t; _temp1729;});}} int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp* e){ void* _temp1731= Cyc_Tcutil_compress(( void*)({ struct
Cyc_Core_Opt* _temp1730= e->topt; if( _temp1730 == 0){ _throw( Null_Exception);}
_temp1730->v;})); struct Cyc_Absyn_Enumdecl* _temp1741; struct _tuple1*
_temp1743; void* _temp1745; void* _temp1747; int _temp1749; struct Cyc_Core_Opt*
_temp1751; void* _temp1753; _LL1733: if(( unsigned int) _temp1731 > 4u?*(( int*)
_temp1731) == Cyc_Absyn_EnumType: 0){ _LL1744: _temp1743=( struct _tuple1*)((
struct Cyc_Absyn_EnumType_struct*) _temp1731)->f1; goto _LL1742; _LL1742:
_temp1741=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*)
_temp1731)->f2; goto _LL1734;} else{ goto _LL1735;} _LL1735: if(( unsigned int)
_temp1731 > 4u?*(( int*) _temp1731) == Cyc_Absyn_IntType: 0){ _LL1748: _temp1747=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp1731)->f1; goto _LL1746; _LL1746:
_temp1745=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1731)->f2; goto
_LL1736;} else{ goto _LL1737;} _LL1737: if(( unsigned int) _temp1731 > 4u?*((
int*) _temp1731) == Cyc_Absyn_Evar: 0){ _LL1754: _temp1753=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp1731)->f1; goto _LL1752; _LL1752: _temp1751=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Evar_struct*) _temp1731)->f2; goto _LL1750; _LL1750: _temp1749=(
int)(( struct Cyc_Absyn_Evar_struct*) _temp1731)->f3; goto _LL1738;} else{ goto
_LL1739;} _LL1739: goto _LL1740; _LL1734: goto _LL1736; _LL1736: return 1;
_LL1738: if( Cyc_Tcutil_unify(( void*)({ struct Cyc_Core_Opt* _temp1755= e->topt;
if( _temp1755 == 0){ _throw( Null_Exception);} _temp1755->v;}), Cyc_Absyn_sint_t)){
return 1;} return 0; _LL1740: return 0; _LL1732:;} int Cyc_Tcutil_is_numeric(
struct Cyc_Absyn_Exp* e){ if( Cyc_Tcutil_is_integral( e)){ return 1;}{ void*
_temp1757= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp1756= e->topt;
if( _temp1756 == 0){ _throw( Null_Exception);} _temp1756->v;})); _LL1759: if(
_temp1757 ==( void*) Cyc_Absyn_FloatType){ goto _LL1760;} else{ goto _LL1761;}
_LL1761: if( _temp1757 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1762;} else{
goto _LL1763;} _LL1763: goto _LL1764; _LL1760: goto _LL1762; _LL1762: return 1;
_LL1764: return 0; _LL1758:;}} int Cyc_Tcutil_is_function_type( void* t){ void*
_temp1765= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo _temp1771; _LL1767:
if(( unsigned int) _temp1765 > 4u?*(( int*) _temp1765) == Cyc_Absyn_FnType: 0){
_LL1772: _temp1771=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp1765)->f1; goto _LL1768;} else{ goto _LL1769;} _LL1769: goto _LL1770;
_LL1768: return 1; _LL1770: return 0; _LL1766:;} void* Cyc_Tcutil_max_arithmetic_type(
void* t1, void* t2){ struct _tuple7 _temp1774=({ struct _tuple7 _temp1773;
_temp1773.f1= t1; _temp1773.f2= t2; _temp1773;}); void* _temp1798; void*
_temp1800; void* _temp1802; void* _temp1804; void* _temp1806; void* _temp1808;
void* _temp1810; void* _temp1812; void* _temp1814; void* _temp1816; void*
_temp1818; void* _temp1820; void* _temp1822; void* _temp1824; void* _temp1826;
void* _temp1828; void* _temp1830; void* _temp1832; void* _temp1834; void*
_temp1836; void* _temp1838; void* _temp1840; void* _temp1842; void* _temp1844;
void* _temp1846; void* _temp1848; void* _temp1850; void* _temp1852; void*
_temp1854; void* _temp1856; void* _temp1858; void* _temp1860; _LL1776: _LL1801:
_temp1800= _temp1774.f1; if( _temp1800 ==( void*) Cyc_Absyn_DoubleType){ goto
_LL1799;} else{ goto _LL1778;} _LL1799: _temp1798= _temp1774.f2; goto _LL1777;
_LL1778: _LL1805: _temp1804= _temp1774.f1; goto _LL1803; _LL1803: _temp1802=
_temp1774.f2; if( _temp1802 ==( void*) Cyc_Absyn_DoubleType){ goto _LL1779;}
else{ goto _LL1780;} _LL1780: _LL1809: _temp1808= _temp1774.f1; if( _temp1808 ==(
void*) Cyc_Absyn_FloatType){ goto _LL1807;} else{ goto _LL1782;} _LL1807:
_temp1806= _temp1774.f2; goto _LL1781; _LL1782: _LL1813: _temp1812= _temp1774.f1;
goto _LL1811; _LL1811: _temp1810= _temp1774.f2; if( _temp1810 ==( void*) Cyc_Absyn_FloatType){
goto _LL1783;} else{ goto _LL1784;} _LL1784: _LL1817: _temp1816= _temp1774.f1;
if(( unsigned int) _temp1816 > 4u?*(( int*) _temp1816) == Cyc_Absyn_IntType: 0){
_LL1821: _temp1820=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1816)->f1;
if( _temp1820 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1819;} else{ goto _LL1786;}
_LL1819: _temp1818=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1816)->f2;
if( _temp1818 ==( void*) Cyc_Absyn_B8){ goto _LL1815;} else{ goto _LL1786;}}
else{ goto _LL1786;} _LL1815: _temp1814= _temp1774.f2; goto _LL1785; _LL1786:
_LL1829: _temp1828= _temp1774.f1; goto _LL1823; _LL1823: _temp1822= _temp1774.f2;
if(( unsigned int) _temp1822 > 4u?*(( int*) _temp1822) == Cyc_Absyn_IntType: 0){
_LL1827: _temp1826=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1822)->f1;
if( _temp1826 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1825;} else{ goto _LL1788;}
_LL1825: _temp1824=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1822)->f2;
if( _temp1824 ==( void*) Cyc_Absyn_B8){ goto _LL1787;} else{ goto _LL1788;}}
else{ goto _LL1788;} _LL1788: _LL1833: _temp1832= _temp1774.f1; if((
unsigned int) _temp1832 > 4u?*(( int*) _temp1832) == Cyc_Absyn_IntType: 0){
_LL1837: _temp1836=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1832)->f1;
if( _temp1836 ==( void*) Cyc_Absyn_Signed){ goto _LL1835;} else{ goto _LL1790;}
_LL1835: _temp1834=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1832)->f2;
if( _temp1834 ==( void*) Cyc_Absyn_B8){ goto _LL1831;} else{ goto _LL1790;}}
else{ goto _LL1790;} _LL1831: _temp1830= _temp1774.f2; goto _LL1789; _LL1790:
_LL1845: _temp1844= _temp1774.f1; goto _LL1839; _LL1839: _temp1838= _temp1774.f2;
if(( unsigned int) _temp1838 > 4u?*(( int*) _temp1838) == Cyc_Absyn_IntType: 0){
_LL1843: _temp1842=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1838)->f1;
if( _temp1842 ==( void*) Cyc_Absyn_Signed){ goto _LL1841;} else{ goto _LL1792;}
_LL1841: _temp1840=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1838)->f2;
if( _temp1840 ==( void*) Cyc_Absyn_B8){ goto _LL1791;} else{ goto _LL1792;}}
else{ goto _LL1792;} _LL1792: _LL1849: _temp1848= _temp1774.f1; if((
unsigned int) _temp1848 > 4u?*(( int*) _temp1848) == Cyc_Absyn_IntType: 0){
_LL1853: _temp1852=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1848)->f1;
if( _temp1852 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1851;} else{ goto _LL1794;}
_LL1851: _temp1850=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1848)->f2;
if( _temp1850 ==( void*) Cyc_Absyn_B4){ goto _LL1847;} else{ goto _LL1794;}}
else{ goto _LL1794;} _LL1847: _temp1846= _temp1774.f2; goto _LL1793; _LL1794:
_LL1861: _temp1860= _temp1774.f1; goto _LL1855; _LL1855: _temp1854= _temp1774.f2;
if(( unsigned int) _temp1854 > 4u?*(( int*) _temp1854) == Cyc_Absyn_IntType: 0){
_LL1859: _temp1858=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1854)->f1;
if( _temp1858 ==( void*) Cyc_Absyn_Unsigned){ goto _LL1857;} else{ goto _LL1796;}
_LL1857: _temp1856=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1854)->f2;
if( _temp1856 ==( void*) Cyc_Absyn_B4){ goto _LL1795;} else{ goto _LL1796;}}
else{ goto _LL1796;} _LL1796: goto _LL1797; _LL1777: goto _LL1779; _LL1779:
return( void*) Cyc_Absyn_DoubleType; _LL1781: goto _LL1783; _LL1783: return(
void*) Cyc_Absyn_FloatType; _LL1785: goto _LL1787; _LL1787: return Cyc_Absyn_ulong_t;
_LL1789: goto _LL1791; _LL1791: return Cyc_Absyn_slong_t; _LL1793: goto _LL1795;
_LL1795: return Cyc_Absyn_uint_t; _LL1797: return Cyc_Absyn_sint_t; _LL1775:;}
void Cyc_Tcutil_check_contains_assign( struct Cyc_Absyn_Exp* e){ void* _temp1862=(
void*) e->r; struct Cyc_Absyn_Exp* _temp1868; struct Cyc_Core_Opt* _temp1870;
struct Cyc_Absyn_Exp* _temp1872; _LL1864: if(*(( int*) _temp1862) == Cyc_Absyn_AssignOp_e){
_LL1873: _temp1872=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1862)->f1; goto _LL1871; _LL1871: _temp1870=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1862)->f2; if( _temp1870 == 0){ goto
_LL1869;} else{ goto _LL1866;} _LL1869: _temp1868=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1862)->f3; goto _LL1865;} else{ goto
_LL1866;} _LL1866: goto _LL1867; _LL1865: Cyc_Tcutil_warn( e->loc,( struct
_tagged_string)({ char* _temp1874=( char*)"assignment in test"; struct
_tagged_string _temp1875; _temp1875.curr= _temp1874; _temp1875.base= _temp1874;
_temp1875.last_plus_one= _temp1874 + 19; _temp1875;})); goto _LL1863; _LL1867:
goto _LL1863; _LL1863:;} static int Cyc_Tcutil_constrain_kinds( struct Cyc_Absyn_Conref*
c1, struct Cyc_Absyn_Conref* c2){ c1=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( c1); c2=(( struct Cyc_Absyn_Conref*(*)( struct
Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( c2); if( c1 == c2){ return 1;}
if(( void*) c1->v ==( void*) Cyc_Absyn_No_constr){( void*)( c1->v=( void*)((
void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp1876=( struct Cyc_Absyn_Forward_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp1876[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp1877; _temp1877.tag= Cyc_Absyn_Forward_constr;
_temp1877.f1= c2; _temp1877;}); _temp1876;}))); return 1;} else{ if(( void*) c2->v
==( void*) Cyc_Absyn_No_constr){( void*)( c2->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp1878=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp1878[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp1879; _temp1879.tag= Cyc_Absyn_Forward_constr; _temp1879.f1= c1; _temp1879;});
_temp1878;}))); return 1;} else{ void* k1=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( c1); void* k2=(( void*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_val)( c2); if( Cyc_Tcutil_kind_leq( k1, k2)){( void*)( c2->v=(
void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp1880=( struct Cyc_Absyn_Forward_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp1880[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp1881; _temp1881.tag= Cyc_Absyn_Forward_constr;
_temp1881.f1= c1; _temp1881;}); _temp1880;}))); return 1;} else{ if( Cyc_Tcutil_kind_leq(
k2, k1)){( void*)( c1->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp1882=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp1882[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp1883; _temp1883.tag= Cyc_Absyn_Forward_constr; _temp1883.f1= c2; _temp1883;});
_temp1882;}))); return 1;} else{ return 0;}}}}} static struct Cyc_List_List* Cyc_Tcutil_add_free_tvar(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* tvs, struct Cyc_Absyn_Tvar*
tv){ if( tvs == 0){ return({ struct Cyc_List_List* _temp1884=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1884->hd=( void*) tv; _temp1884->tl=
0; _temp1884;});} else{{ struct Cyc_List_List* tvs2= tvs; for( 0; tvs2 != 0;
tvs2=({ struct Cyc_List_List* _temp1885= tvs2; if( _temp1885 == 0){ _throw(
Null_Exception);} _temp1885->tl;})){ if( Cyc_String_zstrptrcmp((( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp1886= tvs2; if( _temp1886 == 0){ _throw(
Null_Exception);} _temp1886->hd;}))->name, tv->name) == 0){ struct Cyc_Absyn_Conref*
k1=(( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp1890= tvs2; if(
_temp1890 == 0){ _throw( Null_Exception);} _temp1890->hd;}))->kind; struct Cyc_Absyn_Conref*
k2= tv->kind; if( ! Cyc_Tcutil_constrain_kinds( k1, k2)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp1887=* tv->name; struct _tagged_string _temp1888= Cyc_Absynpp_ckind2string(
k1); struct _tagged_string _temp1889= Cyc_Absynpp_ckind2string( k2); xprintf("type variable %.*s is used with inconsistent kinds %.*s and %.*s",
_temp1887.last_plus_one - _temp1887.curr, _temp1887.curr, _temp1888.last_plus_one
- _temp1888.curr, _temp1888.curr, _temp1889.last_plus_one - _temp1889.curr,
_temp1889.curr);}));} return tvs;} else{ if(({ struct Cyc_List_List* _temp1891=
tvs2; if( _temp1891 == 0){ _throw( Null_Exception);} _temp1891->tl;}) == 0){({
struct Cyc_List_List* _temp1892= tvs2; if( _temp1892 == 0){ _throw(
Null_Exception);} _temp1892->tl=({ struct Cyc_List_List* _temp1893=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1893->hd=( void*) tv; _temp1893->tl=
0; _temp1893;});}); return tvs;}}}} return(( struct Cyc_List_List*(*)( struct
_tagged_string msg)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp1894=( char*)"Tcutil::add_free_tvar:expecting to have returned."; struct
_tagged_string _temp1895; _temp1895.curr= _temp1894; _temp1895.base= _temp1894;
_temp1895.last_plus_one= _temp1894 + 50; _temp1895;}));}} static struct Cyc_List_List*
Cyc_Tcutil_remove_bound_tvars( struct Cyc_List_List* tvs, struct Cyc_List_List*
btvs){ struct Cyc_List_List* r= 0; for( 0; tvs != 0; tvs=({ struct Cyc_List_List*
_temp1896= tvs; if( _temp1896 == 0){ _throw( Null_Exception);} _temp1896->tl;})){
int present= 0;{ struct Cyc_List_List* b= btvs; for( 0; b != 0; b=({ struct Cyc_List_List*
_temp1897= b; if( _temp1897 == 0){ _throw( Null_Exception);} _temp1897->tl;})){
if( Cyc_String_zstrptrcmp((( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp1898= tvs; if( _temp1898 == 0){ _throw( Null_Exception);} _temp1898->hd;}))->name,((
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp1899= b; if( _temp1899 == 0){
_throw( Null_Exception);} _temp1899->hd;}))->name) == 0){ present= 1; break;}}}
if( ! present){ r=({ struct Cyc_List_List* _temp1900=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1900->hd=( void*)(( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp1901= tvs; if( _temp1901 == 0){ _throw(
Null_Exception);} _temp1901->hd;})); _temp1900->tl= r; _temp1900;});}} r=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( r);
return r;} static int Cyc_Tcutil_tvar_counter= 0; struct Cyc_Absyn_Tvar* Cyc_Tcutil_new_tvar(
void* k){ struct _tagged_string s= xprintf("#%d",( Cyc_Tcutil_tvar_counter ++));
return({ struct Cyc_Absyn_Tvar* _temp1902=( struct Cyc_Absyn_Tvar*) GC_malloc(
sizeof( struct Cyc_Absyn_Tvar)); _temp1902->name=({ struct _tagged_string*
_temp1903=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp1903[ 0]= s; _temp1903;}); _temp1902->kind=(( struct Cyc_Absyn_Conref*(*)(
void* x)) Cyc_Absyn_new_conref)( k); _temp1902;});} int Cyc_Tcutil_is_temp_tvar(
struct Cyc_Absyn_Tvar* t){ struct _tagged_string s=* t->name; return({ struct
_tagged_string _temp1904= s; char* _temp1906= _temp1904.curr + 0; if( _temp1904.base
== 0? 1:( _temp1906 < _temp1904.base? 1: _temp1906 >= _temp1904.last_plus_one)){
_throw( Null_Exception);}* _temp1906;}) =='#';} void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar* t){ if( ! Cyc_Tcutil_is_temp_tvar( t)){ return;}{ struct
_tagged_string s= Cyc_String_strconcat(( struct _tagged_string)({ char*
_temp1911=( char*)"`"; struct _tagged_string _temp1912; _temp1912.curr=
_temp1911; _temp1912.base= _temp1911; _temp1912.last_plus_one= _temp1911 + 2;
_temp1912;}),* t->name);({ struct _tagged_string _temp1907= s; char* _temp1909=
_temp1907.curr + 1; if( _temp1907.base == 0? 1:( _temp1909 < _temp1907.base? 1:
_temp1909 >= _temp1907.last_plus_one)){ _throw( Null_Exception);}* _temp1909='t';});
t->name=({ struct _tagged_string* _temp1910=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp1910[ 0]= s; _temp1910;});}} struct
_tuple11{ struct _tagged_string* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ;
static struct _tuple2* Cyc_Tcutil_fndecl2typ_f( struct _tuple11* x){ return({
struct _tuple2* _temp1913=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp1913->f1=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt* _temp1914=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1914->v=( void*)(*
x).f1; _temp1914;}); _temp1913->f2=(* x).f2; _temp1913->f3=(* x).f3; _temp1913;});}
int Cyc_Tcutil_fntype_att( void* a){ void* _temp1915= a; int _temp1929; _LL1917:
if(( unsigned int) _temp1915 > 15u?*(( int*) _temp1915) == Cyc_Absyn_Regparm_att:
0){ _LL1930: _temp1929=( int)(( struct Cyc_Absyn_Regparm_att_struct*) _temp1915)->f1;
goto _LL1918;} else{ goto _LL1919;} _LL1919: if( _temp1915 ==( void*) Cyc_Absyn_Stdcall_att){
goto _LL1920;} else{ goto _LL1921;} _LL1921: if( _temp1915 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL1922;} else{ goto _LL1923;} _LL1923: if( _temp1915 ==( void*) Cyc_Absyn_Noreturn_att){
goto _LL1924;} else{ goto _LL1925;} _LL1925: if( _temp1915 ==( void*) Cyc_Absyn_Const_att){
goto _LL1926;} else{ goto _LL1927;} _LL1927: goto _LL1928; _LL1918: goto _LL1920;
_LL1920: goto _LL1922; _LL1922: goto _LL1924; _LL1924: goto _LL1926; _LL1926:
return 1; _LL1928: return 0; _LL1916:;} void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl*
fd){ if( fd->cached_typ == 0){ struct Cyc_List_List* fn_type_atts= 0;{ struct
Cyc_List_List* atts= fd->attributes; for( 0; atts != 0; atts=({ struct Cyc_List_List*
_temp1931= atts; if( _temp1931 == 0){ _throw( Null_Exception);} _temp1931->tl;})){
if( Cyc_Tcutil_fntype_att(( void*)({ struct Cyc_List_List* _temp1932= atts; if(
_temp1932 == 0){ _throw( Null_Exception);} _temp1932->hd;}))){ fn_type_atts=({
struct Cyc_List_List* _temp1933=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1933->hd=( void*)(( void*)({ struct Cyc_List_List*
_temp1934= atts; if( _temp1934 == 0){ _throw( Null_Exception);} _temp1934->hd;}));
_temp1933->tl= fn_type_atts; _temp1933;});}}} return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp1935=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp1935[ 0]=({ struct Cyc_Absyn_FnType_struct _temp1936; _temp1936.tag= Cyc_Absyn_FnType;
_temp1936.f1=({ struct Cyc_Absyn_FnInfo _temp1937; _temp1937.tvars= fd->tvs;
_temp1937.effect= fd->effect; _temp1937.ret_typ=( void*)(( void*) fd->ret_type);
_temp1937.args=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct
_tuple11*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_fndecl2typ_f, fd->args);
_temp1937.varargs= fd->varargs; _temp1937.attributes= fn_type_atts; _temp1937;});
_temp1936;}); _temp1935;});} return( void*)({ struct Cyc_Core_Opt* _temp1938= fd->cached_typ;
if( _temp1938 == 0){ _throw( Null_Exception);} _temp1938->v;});} struct _tuple12{
void* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; static void* Cyc_Tcutil_fst_fdarg(
struct _tuple12* t){ return(* t).f1;} void* Cyc_Tcutil_snd_tqt( struct _tuple4*
t){ return(* t).f2;} static struct _tuple4* Cyc_Tcutil_map2_tq( struct _tuple4*
pr, void* t){ return({ struct _tuple4* _temp1939=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp1939->f1=(* pr).f1; _temp1939->f2= t; _temp1939;});}
void Cyc_Tcutil_check_fndecl_valid_type( struct Cyc_Position_Segment* loc,
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Fndecl* fd){ void* t= Cyc_Tcutil_fndecl2typ(
fd); Cyc_Tcutil_check_valid_toplevel_type( loc, te, t);{ void* _temp1940= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_FnInfo _temp1946; struct Cyc_List_List* _temp1948; int
_temp1950; struct Cyc_List_List* _temp1952; void* _temp1954; struct Cyc_Core_Opt*
_temp1956; struct Cyc_List_List* _temp1958; _LL1942: if(( unsigned int)
_temp1940 > 4u?*(( int*) _temp1940) == Cyc_Absyn_FnType: 0){ _LL1947: _temp1946=(
struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*) _temp1940)->f1;
_LL1959: _temp1958=( struct Cyc_List_List*) _temp1946.tvars; goto _LL1957;
_LL1957: _temp1956=( struct Cyc_Core_Opt*) _temp1946.effect; goto _LL1955;
_LL1955: _temp1954=( void*) _temp1946.ret_typ; goto _LL1953; _LL1953: _temp1952=(
struct Cyc_List_List*) _temp1946.args; goto _LL1951; _LL1951: _temp1950=( int)
_temp1946.varargs; goto _LL1949; _LL1949: _temp1948=( struct Cyc_List_List*)
_temp1946.attributes; goto _LL1943;} else{ goto _LL1944;} _LL1944: goto _LL1945;
_LL1943: fd->tvs= _temp1958; fd->effect= _temp1956; goto _LL1941; _LL1945:((
void(*)( struct _tagged_string msg)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp1960=( char*)"check_fndecl_valid_type: not a FnType"; struct
_tagged_string _temp1961; _temp1961.curr= _temp1960; _temp1961.base= _temp1960;
_temp1961.last_plus_one= _temp1960 + 38; _temp1961;})); return; _LL1941:;}{
struct _RegionHandle _temp1962= _new_region(); struct _RegionHandle* r=&
_temp1962; struct _handler_cons _temp1963; _push_handler(& _temp1963);{ void*
_temp1964=( void*) setjmp( _temp1963.handler); if( ! _temp1964){ Cyc_Tcutil_check_unique_vars(((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tagged_string*(* f)(
struct _tuple11*), struct Cyc_List_List* x)) Cyc_List_rmap)( r,( struct
_tagged_string*(*)( struct _tuple11* t)) Cyc_Tcutil_fst_fdarg, fd->args), loc,(
struct _tagged_string)({ char* _temp1965=( char*)"function declaration has repeated parameter";
struct _tagged_string _temp1966; _temp1966.curr= _temp1965; _temp1966.base=
_temp1965; _temp1966.last_plus_one= _temp1965 + 44; _temp1966;})); _free_region(&
_temp1962); _pop_handler();} else{ _free_region(& _temp1962); _throw( _temp1964);}}}
fd->cached_typ=({ struct Cyc_Core_Opt* _temp1967=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1967->v=( void*) t; _temp1967;});}
struct _tuple13{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; } ; struct
_tuple14{ struct _tuple13* f1; void* f2; } ; static struct _tuple14* Cyc_Tcutil_substitute_f1(
struct _RegionHandle* rgn, struct _tuple2* y){ return({ struct _tuple14*
_temp1968=( struct _tuple14*) _region_malloc( rgn, sizeof( struct _tuple14));
_temp1968->f1=({ struct _tuple13* _temp1969=( struct _tuple13*) _region_malloc(
rgn, sizeof( struct _tuple13)); _temp1969->f1=(* y).f1; _temp1969->f2=(* y).f2;
_temp1969;}); _temp1968->f2=(* y).f3; _temp1968;});} static struct _tuple2* Cyc_Tcutil_substitute_f2(
struct _tuple14* w){ void* _temp1972; struct _tuple13* _temp1974; struct
_tuple14 _temp1970=* w; _LL1975: _temp1974= _temp1970.f1; goto _LL1973; _LL1973:
_temp1972= _temp1970.f2; goto _LL1971; _LL1971: { struct Cyc_Absyn_Tqual
_temp1978; struct Cyc_Core_Opt* _temp1980; struct _tuple13 _temp1976=* _temp1974;
_LL1981: _temp1980= _temp1976.f1; goto _LL1979; _LL1979: _temp1978= _temp1976.f2;
goto _LL1977; _LL1977: return({ struct _tuple2* _temp1982=( struct _tuple2*)
GC_malloc( sizeof( struct _tuple2)); _temp1982->f1= _temp1980; _temp1982->f2=
_temp1978; _temp1982->f3= _temp1972; _temp1982;});}} static struct Cyc_List_List*
Cyc_Tcutil_substs( struct _RegionHandle* rgn, struct Cyc_List_List* inst, struct
Cyc_List_List* ts); void* Cyc_Tcutil_rsubstitute( struct _RegionHandle* rgn,
struct Cyc_List_List* inst, void* t){ void* _temp1983= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Tvar* _temp2025; struct Cyc_Absyn_Structdecl** _temp2027;
struct Cyc_List_List* _temp2029; struct _tuple1* _temp2031; struct Cyc_Absyn_Uniondecl**
_temp2033; struct Cyc_List_List* _temp2035; struct _tuple1* _temp2037; struct
Cyc_Absyn_TunionInfo _temp2039; void* _temp2041; struct Cyc_List_List* _temp2043;
void* _temp2045; struct Cyc_Absyn_TunionFieldInfo _temp2047; struct Cyc_List_List*
_temp2049; void* _temp2051; struct Cyc_Core_Opt* _temp2053; struct Cyc_List_List*
_temp2055; struct _tuple1* _temp2057; struct Cyc_Absyn_Exp* _temp2059; struct
Cyc_Absyn_Tqual _temp2061; void* _temp2063; struct Cyc_Absyn_PtrInfo _temp2065;
struct Cyc_Absyn_Conref* _temp2067; struct Cyc_Absyn_Tqual _temp2069; struct Cyc_Absyn_Conref*
_temp2071; void* _temp2073; void* _temp2075; struct Cyc_Absyn_FnInfo _temp2077;
struct Cyc_List_List* _temp2079; int _temp2081; struct Cyc_List_List* _temp2083;
void* _temp2085; struct Cyc_Core_Opt* _temp2087; struct Cyc_List_List* _temp2089;
struct Cyc_List_List* _temp2091; int _temp2093; struct Cyc_Core_Opt* _temp2095;
void* _temp2097; void* _temp2099; struct Cyc_Absyn_Enumdecl* _temp2101; struct
_tuple1* _temp2103; void* _temp2105; void* _temp2107; void* _temp2109; struct
Cyc_List_List* _temp2111; _LL1985: if(( unsigned int) _temp1983 > 4u?*(( int*)
_temp1983) == Cyc_Absyn_VarType: 0){ _LL2026: _temp2025=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_VarType_struct*) _temp1983)->f1; goto _LL1986;} else{ goto
_LL1987;} _LL1987: if(( unsigned int) _temp1983 > 4u?*(( int*) _temp1983) == Cyc_Absyn_StructType:
0){ _LL2032: _temp2031=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp1983)->f1; goto _LL2030; _LL2030: _temp2029=( struct Cyc_List_List*)((
struct Cyc_Absyn_StructType_struct*) _temp1983)->f2; goto _LL2028; _LL2028:
_temp2027=( struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*)
_temp1983)->f3; goto _LL1988;} else{ goto _LL1989;} _LL1989: if(( unsigned int)
_temp1983 > 4u?*(( int*) _temp1983) == Cyc_Absyn_UnionType: 0){ _LL2038:
_temp2037=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*) _temp1983)->f1;
goto _LL2036; _LL2036: _temp2035=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp1983)->f2; goto _LL2034; _LL2034: _temp2033=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp1983)->f3; goto _LL1990;} else{ goto
_LL1991;} _LL1991: if(( unsigned int) _temp1983 > 4u?*(( int*) _temp1983) == Cyc_Absyn_TunionType:
0){ _LL2040: _temp2039=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp1983)->f1; _LL2046: _temp2045=( void*) _temp2039.tunion_info; goto _LL2044;
_LL2044: _temp2043=( struct Cyc_List_List*) _temp2039.targs; goto _LL2042;
_LL2042: _temp2041=( void*) _temp2039.rgn; goto _LL1992;} else{ goto _LL1993;}
_LL1993: if(( unsigned int) _temp1983 > 4u?*(( int*) _temp1983) == Cyc_Absyn_TunionFieldType:
0){ _LL2048: _temp2047=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp1983)->f1; _LL2052: _temp2051=( void*) _temp2047.field_info; goto _LL2050;
_LL2050: _temp2049=( struct Cyc_List_List*) _temp2047.targs; goto _LL1994;}
else{ goto _LL1995;} _LL1995: if(( unsigned int) _temp1983 > 4u?*(( int*)
_temp1983) == Cyc_Absyn_TypedefType: 0){ _LL2058: _temp2057=( struct _tuple1*)((
struct Cyc_Absyn_TypedefType_struct*) _temp1983)->f1; goto _LL2056; _LL2056:
_temp2055=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp1983)->f2; goto _LL2054; _LL2054: _temp2053=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_TypedefType_struct*) _temp1983)->f3; goto _LL1996;} else{ goto
_LL1997;} _LL1997: if(( unsigned int) _temp1983 > 4u?*(( int*) _temp1983) == Cyc_Absyn_ArrayType:
0){ _LL2064: _temp2063=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1983)->f1;
goto _LL2062; _LL2062: _temp2061=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1983)->f2; goto _LL2060; _LL2060: _temp2059=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1983)->f3; goto _LL1998;} else{ goto
_LL1999;} _LL1999: if(( unsigned int) _temp1983 > 4u?*(( int*) _temp1983) == Cyc_Absyn_PointerType:
0){ _LL2066: _temp2065=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1983)->f1; _LL2076: _temp2075=( void*) _temp2065.elt_typ; goto _LL2074;
_LL2074: _temp2073=( void*) _temp2065.rgn_typ; goto _LL2072; _LL2072: _temp2071=(
struct Cyc_Absyn_Conref*) _temp2065.nullable; goto _LL2070; _LL2070: _temp2069=(
struct Cyc_Absyn_Tqual) _temp2065.tq; goto _LL2068; _LL2068: _temp2067=( struct
Cyc_Absyn_Conref*) _temp2065.bounds; goto _LL2000;} else{ goto _LL2001;} _LL2001:
if(( unsigned int) _temp1983 > 4u?*(( int*) _temp1983) == Cyc_Absyn_FnType: 0){
_LL2078: _temp2077=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp1983)->f1; _LL2090: _temp2089=( struct Cyc_List_List*) _temp2077.tvars;
goto _LL2088; _LL2088: _temp2087=( struct Cyc_Core_Opt*) _temp2077.effect; goto
_LL2086; _LL2086: _temp2085=( void*) _temp2077.ret_typ; goto _LL2084; _LL2084:
_temp2083=( struct Cyc_List_List*) _temp2077.args; goto _LL2082; _LL2082:
_temp2081=( int) _temp2077.varargs; goto _LL2080; _LL2080: _temp2079=( struct
Cyc_List_List*) _temp2077.attributes; goto _LL2002;} else{ goto _LL2003;}
_LL2003: if(( unsigned int) _temp1983 > 4u?*(( int*) _temp1983) == Cyc_Absyn_TupleType:
0){ _LL2092: _temp2091=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp1983)->f1; goto _LL2004;} else{ goto _LL2005;} _LL2005: if(( unsigned int)
_temp1983 > 4u?*(( int*) _temp1983) == Cyc_Absyn_Evar: 0){ _LL2098: _temp2097=(
void*)(( struct Cyc_Absyn_Evar_struct*) _temp1983)->f1; goto _LL2096; _LL2096:
_temp2095=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp1983)->f2;
goto _LL2094; _LL2094: _temp2093=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp1983)->f3; goto _LL2006;} else{ goto _LL2007;} _LL2007: if(( unsigned int)
_temp1983 > 4u?*(( int*) _temp1983) == Cyc_Absyn_RgnHandleType: 0){ _LL2100:
_temp2099=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp1983)->f1;
goto _LL2008;} else{ goto _LL2009;} _LL2009: if(( unsigned int) _temp1983 > 4u?*((
int*) _temp1983) == Cyc_Absyn_EnumType: 0){ _LL2104: _temp2103=( struct _tuple1*)((
struct Cyc_Absyn_EnumType_struct*) _temp1983)->f1; goto _LL2102; _LL2102:
_temp2101=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*)
_temp1983)->f2; goto _LL2010;} else{ goto _LL2011;} _LL2011: if( _temp1983 ==(
void*) Cyc_Absyn_VoidType){ goto _LL2012;} else{ goto _LL2013;} _LL2013: if((
unsigned int) _temp1983 > 4u?*(( int*) _temp1983) == Cyc_Absyn_IntType: 0){
_LL2108: _temp2107=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1983)->f1;
goto _LL2106; _LL2106: _temp2105=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp1983)->f2; goto _LL2014;} else{ goto _LL2015;} _LL2015: if( _temp1983 ==(
void*) Cyc_Absyn_FloatType){ goto _LL2016;} else{ goto _LL2017;} _LL2017: if(
_temp1983 ==( void*) Cyc_Absyn_DoubleType){ goto _LL2018;} else{ goto _LL2019;}
_LL2019: if( _temp1983 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL2020;} else{ goto
_LL2021;} _LL2021: if(( unsigned int) _temp1983 > 4u?*(( int*) _temp1983) == Cyc_Absyn_AccessEff:
0){ _LL2110: _temp2109=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp1983)->f1;
goto _LL2022;} else{ goto _LL2023;} _LL2023: if(( unsigned int) _temp1983 > 4u?*((
int*) _temp1983) == Cyc_Absyn_JoinEff: 0){ _LL2112: _temp2111=( struct Cyc_List_List*)((
struct Cyc_Absyn_JoinEff_struct*) _temp1983)->f1; goto _LL2024;} else{ goto
_LL1984;} _LL1986: { struct _handler_cons _temp2113; _push_handler(& _temp2113);{
void* _temp2114=( void*) setjmp( _temp2113.handler); if( ! _temp2114){{ void*
_temp2115=(( void*(*)( int(* compare)( struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* l, struct Cyc_Absyn_Tvar* x)) Cyc_List_assoc_cmp)( Cyc_Absyn_tvar_cmp,
inst, _temp2025); _npop_handler( 0u); return _temp2115;}; _pop_handler();} else{
void* _temp2117= _temp2114; _LL2119: if( _temp2117 == Cyc_Core_Not_found){ goto
_LL2120;} else{ goto _LL2121;} _LL2121: goto _LL2122; _LL2120: return t; _LL2122:(
void) _throw( _temp2117); _LL2118:;}}} _LL1988: { struct Cyc_List_List* new_ts=
Cyc_Tcutil_substs( rgn, inst, _temp2029); return new_ts == _temp2029? t:( void*)({
struct Cyc_Absyn_StructType_struct* _temp2123=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp2123[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp2124; _temp2124.tag= Cyc_Absyn_StructType;
_temp2124.f1= _temp2031; _temp2124.f2= new_ts; _temp2124.f3= _temp2027;
_temp2124;}); _temp2123;});} _LL1990: { struct Cyc_List_List* new_ts= Cyc_Tcutil_substs(
rgn, inst, _temp2035); return new_ts == _temp2035? t:( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp2125=( struct Cyc_Absyn_UnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp2125[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp2126; _temp2126.tag= Cyc_Absyn_UnionType;
_temp2126.f1= _temp2037; _temp2126.f2= new_ts; _temp2126.f3= _temp2033;
_temp2126;}); _temp2125;});} _LL1992: { struct Cyc_List_List* new_ts= Cyc_Tcutil_substs(
rgn, inst, _temp2043); void* new_r= Cyc_Tcutil_rsubstitute( rgn, inst, _temp2041);
return( new_ts == _temp2043? new_r == _temp2041: 0)? t:( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp2127=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp2127[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp2128; _temp2128.tag=
Cyc_Absyn_TunionType; _temp2128.f1=({ struct Cyc_Absyn_TunionInfo _temp2129;
_temp2129.tunion_info=( void*) _temp2045; _temp2129.targs= new_ts; _temp2129.rgn=(
void*) new_r; _temp2129;}); _temp2128;}); _temp2127;});} _LL1994: { struct Cyc_List_List*
new_ts= Cyc_Tcutil_substs( rgn, inst, _temp2049); return new_ts == _temp2049? t:(
void*)({ struct Cyc_Absyn_TunionFieldType_struct* _temp2130=( struct Cyc_Absyn_TunionFieldType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct)); _temp2130[ 0]=({
struct Cyc_Absyn_TunionFieldType_struct _temp2131; _temp2131.tag= Cyc_Absyn_TunionFieldType;
_temp2131.f1=({ struct Cyc_Absyn_TunionFieldInfo _temp2132; _temp2132.field_info=(
void*) _temp2051; _temp2132.targs= new_ts; _temp2132;}); _temp2131;}); _temp2130;});}
_LL1996: { struct Cyc_List_List* new_ts= Cyc_Tcutil_substs( rgn, inst, _temp2055);
return new_ts == _temp2055? t:( void*)({ struct Cyc_Absyn_TypedefType_struct*
_temp2133=( struct Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp2133[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp2134; _temp2134.tag=
Cyc_Absyn_TypedefType; _temp2134.f1= _temp2057; _temp2134.f2= new_ts; _temp2134.f3=
_temp2053; _temp2134;}); _temp2133;});} _LL1998: { void* new_t1= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2063); return new_t1 == _temp2063? t:( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp2135=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp2135[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp2136; _temp2136.tag= Cyc_Absyn_ArrayType;
_temp2136.f1=( void*) new_t1; _temp2136.f2= _temp2061; _temp2136.f3= _temp2059;
_temp2136;}); _temp2135;});} _LL2000: { void* new_t1= Cyc_Tcutil_rsubstitute(
rgn, inst, _temp2075); void* new_r= Cyc_Tcutil_rsubstitute( rgn, inst, _temp2073);
if( new_t1 == _temp2075? new_r == _temp2073: 0){ return t;} return( void*)({
struct Cyc_Absyn_PointerType_struct* _temp2137=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp2137[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp2138; _temp2138.tag= Cyc_Absyn_PointerType;
_temp2138.f1=({ struct Cyc_Absyn_PtrInfo _temp2139; _temp2139.elt_typ=( void*)
new_t1; _temp2139.rgn_typ=( void*) new_r; _temp2139.nullable= _temp2071;
_temp2139.tq= _temp2069; _temp2139.bounds= _temp2067; _temp2139;}); _temp2138;});
_temp2137;});} _LL2002:{ struct Cyc_List_List* p= _temp2089; for( 0; p != 0; p=({
struct Cyc_List_List* _temp2140= p; if( _temp2140 == 0){ _throw( Null_Exception);}
_temp2140->tl;})){ inst=({ struct Cyc_List_List* _temp2141=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp2141->hd=( void*)({
struct _tuple5* _temp2142=( struct _tuple5*) _region_malloc( rgn, sizeof( struct
_tuple5)); _temp2142->f1=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp2146= p; if( _temp2146 == 0){ _throw( Null_Exception);} _temp2146->hd;});
_temp2142->f2=( void*)({ struct Cyc_Absyn_VarType_struct* _temp2143=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp2143[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp2144; _temp2144.tag= Cyc_Absyn_VarType; _temp2144.f1=(
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp2145= p; if( _temp2145 == 0){
_throw( Null_Exception);} _temp2145->hd;}); _temp2144;}); _temp2143;});
_temp2142;}); _temp2141->tl= inst; _temp2141;});}}{ struct Cyc_List_List*
_temp2149; struct Cyc_List_List* _temp2151; struct _tuple0 _temp2147=(( struct
_tuple0(*)( struct _RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List*
x)) Cyc_List_rsplit)( rgn, rgn,(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct _tuple14*(* f)( struct _RegionHandle*, struct _tuple2*), struct
_RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_substitute_f1,
rgn, _temp2083)); _LL2152: _temp2151= _temp2147.f1; goto _LL2150; _LL2150:
_temp2149= _temp2147.f2; goto _LL2148; _LL2148: { struct Cyc_List_List* ts2= Cyc_Tcutil_substs(
rgn, inst, _temp2149); struct Cyc_List_List* args2=(( struct Cyc_List_List*(*)(
struct _tuple2*(* f)( struct _tuple14*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcutil_substitute_f2,(( struct Cyc_List_List*(*)( struct _RegionHandle* r3,
struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, _temp2151, ts2)); struct Cyc_Core_Opt* eff2; if( _temp2087 == 0){ eff2=
0;} else{ void* new_eff= Cyc_Tcutil_rsubstitute( rgn, inst,( void*)({ struct Cyc_Core_Opt*
_temp2155= _temp2087; if( _temp2155 == 0){ _throw( Null_Exception);} _temp2155->v;}));
if( new_eff ==( void*)({ struct Cyc_Core_Opt* _temp2153= _temp2087; if(
_temp2153 == 0){ _throw( Null_Exception);} _temp2153->v;})){ eff2= _temp2087;}
else{ eff2=({ struct Cyc_Core_Opt* _temp2154=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp2154->v=( void*) new_eff; _temp2154;});}}
return( void*)({ struct Cyc_Absyn_FnType_struct* _temp2156=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp2156[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp2157; _temp2157.tag= Cyc_Absyn_FnType; _temp2157.f1=({ struct Cyc_Absyn_FnInfo
_temp2158; _temp2158.tvars= _temp2089; _temp2158.effect= eff2; _temp2158.ret_typ=(
void*) Cyc_Tcutil_rsubstitute( rgn, inst, _temp2085); _temp2158.args= args2;
_temp2158.varargs= _temp2081; _temp2158.attributes= _temp2079; _temp2158;});
_temp2157;}); _temp2156;});}} _LL2004: { struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, void*(* f)( struct _tuple4*), struct Cyc_List_List* x))
Cyc_List_rmap)( rgn, Cyc_Tcutil_snd_tqt, _temp2091); struct Cyc_List_List* ts2=
Cyc_Tcutil_substs( rgn, inst, ts); if( ts2 == ts){ return t;}{ struct Cyc_List_List*
tqts2=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct _tuple4*, void*),
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_map2)( Cyc_Tcutil_map2_tq,
_temp2091, ts2); return( void*)({ struct Cyc_Absyn_TupleType_struct* _temp2159=(
struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp2159[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp2160; _temp2160.tag= Cyc_Absyn_TupleType;
_temp2160.f1= tqts2; _temp2160;}); _temp2159;});}} _LL2006: if( _temp2095 != 0){
return Cyc_Tcutil_rsubstitute( rgn, inst,( void*)({ struct Cyc_Core_Opt*
_temp2161= _temp2095; if( _temp2161 == 0){ _throw( Null_Exception);} _temp2161->v;}));}
else{ return t;} _LL2008: { void* new_rt= Cyc_Tcutil_rsubstitute( rgn, inst,
_temp2099); return new_rt == _temp2099? t:( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp2162=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp2162[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp2163; _temp2163.tag= Cyc_Absyn_RgnHandleType; _temp2163.f1=( void*) new_rt;
_temp2163;}); _temp2162;});} _LL2010: return t; _LL2012: return t; _LL2014:
return t; _LL2016: return t; _LL2018: return t; _LL2020: return t; _LL2022: {
void* new_r= Cyc_Tcutil_rsubstitute( rgn, inst, _temp2109); return new_r ==
_temp2109? t:( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2164=( struct
Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2164[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2165; _temp2165.tag= Cyc_Absyn_AccessEff;
_temp2165.f1=( void*) new_r; _temp2165;}); _temp2164;});} _LL2024: { struct Cyc_List_List*
new_es= Cyc_Tcutil_substs( rgn, inst, _temp2111); return new_es == _temp2111? t:(
void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2166=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp2166[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp2167; _temp2167.tag= Cyc_Absyn_JoinEff; _temp2167.f1=
new_es; _temp2167;}); _temp2166;});} _LL1984:;} static struct Cyc_List_List* Cyc_Tcutil_substs(
struct _RegionHandle* rgn, struct Cyc_List_List* inst, struct Cyc_List_List* ts){
if( ts == 0){ return 0;}{ void* old_hd=( void*)({ struct Cyc_List_List*
_temp2170= ts; if( _temp2170 == 0){ _throw( Null_Exception);} _temp2170->hd;});
struct Cyc_List_List* old_tl=({ struct Cyc_List_List* _temp2169= ts; if(
_temp2169 == 0){ _throw( Null_Exception);} _temp2169->tl;}); void* new_hd= Cyc_Tcutil_rsubstitute(
rgn, inst, old_hd); struct Cyc_List_List* new_tl= Cyc_Tcutil_substs( rgn, inst,
old_tl); if( old_hd == new_hd? old_tl == new_tl: 0){ return ts;} return( struct
Cyc_List_List*)({ struct Cyc_List_List* _temp2168=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2168->hd=( void*) new_hd;
_temp2168->tl= new_tl; _temp2168;});}} extern void* Cyc_Tcutil_substitute(
struct Cyc_List_List* inst, void* t){ return Cyc_Tcutil_rsubstitute( Cyc_Core_heap_region,
inst, t);} struct Cyc_Absyn_Exp* Cyc_Tcutil_default_initializer( struct Cyc_Tcenv_Tenv*
te, void* t, struct Cyc_Position_Segment* loc){ struct Cyc_Absyn_Exp* e= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp2220=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2220[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2221; _temp2221.tag= Cyc_Absyn_Const_e; _temp2221.f1=(
void*)(( void*) Cyc_Absyn_Null_c); _temp2221;}); _temp2220;}), loc);{ void*
_temp2171= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo _temp2185; void*
_temp2187; void* _temp2189; void* _temp2191; void* _temp2193; _LL2173: if((
unsigned int) _temp2171 > 4u?*(( int*) _temp2171) == Cyc_Absyn_PointerType: 0){
_LL2186: _temp2185=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp2171)->f1; goto _LL2174;} else{ goto _LL2175;} _LL2175: if(( unsigned int)
_temp2171 > 4u?*(( int*) _temp2171) == Cyc_Absyn_IntType: 0){ _LL2190: _temp2189=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp2171)->f1; goto _LL2188; _LL2188:
_temp2187=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2171)->f2; if(
_temp2187 ==( void*) Cyc_Absyn_B1){ goto _LL2176;} else{ goto _LL2177;}} else{
goto _LL2177;} _LL2177: if(( unsigned int) _temp2171 > 4u?*(( int*) _temp2171)
== Cyc_Absyn_IntType: 0){ _LL2194: _temp2193=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2171)->f1; goto _LL2192; _LL2192: _temp2191=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2171)->f2; goto _LL2178;} else{ goto _LL2179;} _LL2179: if( _temp2171 ==(
void*) Cyc_Absyn_FloatType){ goto _LL2180;} else{ goto _LL2181;} _LL2181: if(
_temp2171 ==( void*) Cyc_Absyn_DoubleType){ goto _LL2182;} else{ goto _LL2183;}
_LL2183: goto _LL2184; _LL2174: goto _LL2172; _LL2176:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp2195=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2195[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2196; _temp2196.tag= Cyc_Absyn_Const_e; _temp2196.f1=(
void*)(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp2197=( struct Cyc_Absyn_Char_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct)); _temp2197[ 0]=({ struct Cyc_Absyn_Char_c_struct
_temp2198; _temp2198.tag= Cyc_Absyn_Char_c; _temp2198.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp2198.f2='\000'; _temp2198;}); _temp2197;})); _temp2196;}); _temp2195;})));
goto _LL2172; _LL2178:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp2199=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2199[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2200; _temp2200.tag= Cyc_Absyn_Const_e;
_temp2200.f1=( void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp2201=(
struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp2201[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp2202; _temp2202.tag= Cyc_Absyn_Int_c;
_temp2202.f1=( void*) _temp2193; _temp2202.f2= 0; _temp2202;}); _temp2201;}));
_temp2200;}); _temp2199;}))); if( _temp2191 !=( void*) Cyc_Absyn_B4){ e= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Cast_e_struct* _temp2203=( struct Cyc_Absyn_Cast_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp2203[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp2204; _temp2204.tag= Cyc_Absyn_Cast_e; _temp2204.f1=( void*) t; _temp2204.f2=
e; _temp2204;}); _temp2203;}), loc);} goto _LL2172; _LL2180:( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp2205=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp2205[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp2206; _temp2206.tag= Cyc_Absyn_Const_e; _temp2206.f1=(
void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp2207=( struct Cyc_Absyn_Float_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Float_c_struct)); _temp2207[ 0]=({ struct
Cyc_Absyn_Float_c_struct _temp2208; _temp2208.tag= Cyc_Absyn_Float_c; _temp2208.f1=(
struct _tagged_string)({ char* _temp2209=( char*)"0.0"; struct _tagged_string
_temp2210; _temp2210.curr= _temp2209; _temp2210.base= _temp2209; _temp2210.last_plus_one=
_temp2209 + 4; _temp2210;}); _temp2208;}); _temp2207;})); _temp2206;});
_temp2205;}))); goto _LL2172; _LL2182:( void*)( e->r=( void*)(( void*)({ struct
Cyc_Absyn_Cast_e_struct* _temp2211=( struct Cyc_Absyn_Cast_e_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp2211[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp2212; _temp2212.tag= Cyc_Absyn_Cast_e; _temp2212.f1=( void*) t; _temp2212.f2=
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp2213=( struct
Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp2213[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp2214; _temp2214.tag= Cyc_Absyn_Const_e;
_temp2214.f1=( void*)(( void*)({ struct Cyc_Absyn_Float_c_struct* _temp2215=(
struct Cyc_Absyn_Float_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Float_c_struct));
_temp2215[ 0]=({ struct Cyc_Absyn_Float_c_struct _temp2216; _temp2216.tag= Cyc_Absyn_Float_c;
_temp2216.f1=( struct _tagged_string)({ char* _temp2217=( char*)"0.0"; struct
_tagged_string _temp2218; _temp2218.curr= _temp2217; _temp2218.base= _temp2217;
_temp2218.last_plus_one= _temp2217 + 4; _temp2218;}); _temp2216;}); _temp2215;}));
_temp2214;}); _temp2213;}), loc); _temp2212;}); _temp2211;}))); goto _LL2172;
_LL2184: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2219= Cyc_Absynpp_typ2string(
t); xprintf("declaration of type %.*s requires initializer", _temp2219.last_plus_one
- _temp2219.curr, _temp2219.curr);})); goto _LL2172; _LL2172:;} return e;}
struct _tuple5* Cyc_Tcutil_make_inst_var( struct Cyc_Absyn_Tvar* tv){ void* k=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( tv->kind); return({
struct _tuple5* _temp2222=( struct _tuple5*) GC_malloc( sizeof( struct _tuple5));
_temp2222->f1= tv; _temp2222->f2= Cyc_Absyn_new_evar( k); _temp2222;});} struct
_tuple5* Cyc_Tcutil_r_make_inst_var( struct _RegionHandle* rgn, struct Cyc_Absyn_Tvar*
tv){ void* k=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( tv->kind);
return({ struct _tuple5* _temp2223=( struct _tuple5*) _region_malloc( rgn,
sizeof( struct _tuple5)); _temp2223->f1= tv; _temp2223->f2= Cyc_Absyn_new_evar(
k); _temp2223;});} struct Cyc_List_List* Cyc_Tcutil_check_valid_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* free_vars, void*
expected_kind, void* t){{ void* _temp2224= Cyc_Tcutil_compress( t); int
_temp2266; struct Cyc_Core_Opt* _temp2268; void* _temp2270; struct Cyc_Absyn_Tvar*
_temp2272; struct Cyc_Absyn_Enumdecl* _temp2274; struct Cyc_Absyn_Enumdecl**
_temp2276; struct _tuple1* _temp2277; struct Cyc_Absyn_TunionInfo _temp2279;
void* _temp2281; struct Cyc_List_List* _temp2283; void* _temp2285; void**
_temp2287; struct Cyc_Absyn_TunionFieldInfo _temp2288; struct Cyc_List_List*
_temp2290; void* _temp2292; void** _temp2294; struct Cyc_Absyn_PtrInfo _temp2295;
struct Cyc_Absyn_Conref* _temp2297; struct Cyc_Absyn_Tqual _temp2299; struct Cyc_Absyn_Conref*
_temp2301; void* _temp2303; void* _temp2305; void* _temp2307; void* _temp2309;
struct Cyc_Absyn_Exp* _temp2311; struct Cyc_Absyn_Tqual _temp2313; void*
_temp2315; struct Cyc_Absyn_FnInfo _temp2317; struct Cyc_List_List* _temp2319;
int _temp2321; struct Cyc_List_List* _temp2323; void* _temp2325; struct Cyc_Core_Opt*
_temp2327; struct Cyc_Core_Opt** _temp2329; struct Cyc_List_List* _temp2330;
struct Cyc_List_List** _temp2332; struct Cyc_List_List* _temp2333; struct Cyc_Absyn_Structdecl**
_temp2335; struct Cyc_Absyn_Structdecl*** _temp2337; struct Cyc_List_List*
_temp2338; struct _tuple1* _temp2340; struct Cyc_Absyn_Uniondecl** _temp2342;
struct Cyc_Absyn_Uniondecl*** _temp2344; struct Cyc_List_List* _temp2345; struct
_tuple1* _temp2347; struct Cyc_Core_Opt* _temp2349; struct Cyc_Core_Opt**
_temp2351; struct Cyc_List_List* _temp2352; struct _tuple1* _temp2354; void*
_temp2356; void* _temp2358; struct Cyc_List_List* _temp2360; _LL2226: if(
_temp2224 ==( void*) Cyc_Absyn_VoidType){ goto _LL2227;} else{ goto _LL2228;}
_LL2228: if(( unsigned int) _temp2224 > 4u?*(( int*) _temp2224) == Cyc_Absyn_Evar:
0){ _LL2271: _temp2270=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp2224)->f1;
goto _LL2269; _LL2269: _temp2268=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp2224)->f2; goto _LL2267; _LL2267: _temp2266=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp2224)->f3; goto _LL2229;} else{ goto _LL2230;} _LL2230: if(( unsigned int)
_temp2224 > 4u?*(( int*) _temp2224) == Cyc_Absyn_VarType: 0){ _LL2273: _temp2272=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*) _temp2224)->f1; goto
_LL2231;} else{ goto _LL2232;} _LL2232: if(( unsigned int) _temp2224 > 4u?*((
int*) _temp2224) == Cyc_Absyn_EnumType: 0){ _LL2278: _temp2277=( struct _tuple1*)((
struct Cyc_Absyn_EnumType_struct*) _temp2224)->f1; goto _LL2275; _LL2275:
_temp2274=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*)
_temp2224)->f2; _temp2276=&(( struct Cyc_Absyn_EnumType_struct*) _temp2224)->f2;
goto _LL2233;} else{ goto _LL2234;} _LL2234: if(( unsigned int) _temp2224 > 4u?*((
int*) _temp2224) == Cyc_Absyn_TunionType: 0){ _LL2280: _temp2279=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp2224)->f1; _LL2286: _temp2285=( void*)
_temp2279.tunion_info; _temp2287=&((( struct Cyc_Absyn_TunionType_struct*)
_temp2224)->f1).tunion_info; goto _LL2284; _LL2284: _temp2283=( struct Cyc_List_List*)
_temp2279.targs; goto _LL2282; _LL2282: _temp2281=( void*) _temp2279.rgn; goto
_LL2235;} else{ goto _LL2236;} _LL2236: if(( unsigned int) _temp2224 > 4u?*((
int*) _temp2224) == Cyc_Absyn_TunionFieldType: 0){ _LL2289: _temp2288=( struct
Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*) _temp2224)->f1;
_LL2293: _temp2292=( void*) _temp2288.field_info; _temp2294=&((( struct Cyc_Absyn_TunionFieldType_struct*)
_temp2224)->f1).field_info; goto _LL2291; _LL2291: _temp2290=( struct Cyc_List_List*)
_temp2288.targs; goto _LL2237;} else{ goto _LL2238;} _LL2238: if(( unsigned int)
_temp2224 > 4u?*(( int*) _temp2224) == Cyc_Absyn_PointerType: 0){ _LL2296:
_temp2295=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp2224)->f1; _LL2306: _temp2305=( void*) _temp2295.elt_typ; goto _LL2304;
_LL2304: _temp2303=( void*) _temp2295.rgn_typ; goto _LL2302; _LL2302: _temp2301=(
struct Cyc_Absyn_Conref*) _temp2295.nullable; goto _LL2300; _LL2300: _temp2299=(
struct Cyc_Absyn_Tqual) _temp2295.tq; goto _LL2298; _LL2298: _temp2297=( struct
Cyc_Absyn_Conref*) _temp2295.bounds; goto _LL2239;} else{ goto _LL2240;} _LL2240:
if(( unsigned int) _temp2224 > 4u?*(( int*) _temp2224) == Cyc_Absyn_IntType: 0){
_LL2310: _temp2309=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp2224)->f1;
goto _LL2308; _LL2308: _temp2307=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp2224)->f2; goto _LL2241;} else{ goto _LL2242;} _LL2242: if( _temp2224 ==(
void*) Cyc_Absyn_FloatType){ goto _LL2243;} else{ goto _LL2244;} _LL2244: if(
_temp2224 ==( void*) Cyc_Absyn_DoubleType){ goto _LL2245;} else{ goto _LL2246;}
_LL2246: if(( unsigned int) _temp2224 > 4u?*(( int*) _temp2224) == Cyc_Absyn_ArrayType:
0){ _LL2316: _temp2315=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2224)->f1;
goto _LL2314; _LL2314: _temp2313=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2224)->f2; goto _LL2312; _LL2312: _temp2311=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2224)->f3; goto _LL2247;} else{ goto
_LL2248;} _LL2248: if(( unsigned int) _temp2224 > 4u?*(( int*) _temp2224) == Cyc_Absyn_FnType:
0){ _LL2318: _temp2317=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp2224)->f1; _LL2331: _temp2330=( struct Cyc_List_List*) _temp2317.tvars;
_temp2332=&((( struct Cyc_Absyn_FnType_struct*) _temp2224)->f1).tvars; goto
_LL2328; _LL2328: _temp2327=( struct Cyc_Core_Opt*) _temp2317.effect; _temp2329=&(((
struct Cyc_Absyn_FnType_struct*) _temp2224)->f1).effect; goto _LL2326; _LL2326:
_temp2325=( void*) _temp2317.ret_typ; goto _LL2324; _LL2324: _temp2323=( struct
Cyc_List_List*) _temp2317.args; goto _LL2322; _LL2322: _temp2321=( int)
_temp2317.varargs; goto _LL2320; _LL2320: _temp2319=( struct Cyc_List_List*)
_temp2317.attributes; goto _LL2249;} else{ goto _LL2250;} _LL2250: if((
unsigned int) _temp2224 > 4u?*(( int*) _temp2224) == Cyc_Absyn_TupleType: 0){
_LL2334: _temp2333=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp2224)->f1; goto _LL2251;} else{ goto _LL2252;} _LL2252: if(( unsigned int)
_temp2224 > 4u?*(( int*) _temp2224) == Cyc_Absyn_StructType: 0){ _LL2341:
_temp2340=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp2224)->f1;
goto _LL2339; _LL2339: _temp2338=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp2224)->f2; goto _LL2336; _LL2336: _temp2335=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp2224)->f3; _temp2337=&(( struct Cyc_Absyn_StructType_struct*)
_temp2224)->f3; goto _LL2253;} else{ goto _LL2254;} _LL2254: if(( unsigned int)
_temp2224 > 4u?*(( int*) _temp2224) == Cyc_Absyn_UnionType: 0){ _LL2348:
_temp2347=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*) _temp2224)->f1;
goto _LL2346; _LL2346: _temp2345=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp2224)->f2; goto _LL2343; _LL2343: _temp2342=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp2224)->f3; _temp2344=&(( struct Cyc_Absyn_UnionType_struct*)
_temp2224)->f3; goto _LL2255;} else{ goto _LL2256;} _LL2256: if(( unsigned int)
_temp2224 > 4u?*(( int*) _temp2224) == Cyc_Absyn_TypedefType: 0){ _LL2355:
_temp2354=( struct _tuple1*)(( struct Cyc_Absyn_TypedefType_struct*) _temp2224)->f1;
goto _LL2353; _LL2353: _temp2352=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp2224)->f2; goto _LL2350; _LL2350: _temp2349=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_TypedefType_struct*) _temp2224)->f3; _temp2351=&(( struct Cyc_Absyn_TypedefType_struct*)
_temp2224)->f3; goto _LL2257;} else{ goto _LL2258;} _LL2258: if( _temp2224 ==(
void*) Cyc_Absyn_HeapRgn){ goto _LL2259;} else{ goto _LL2260;} _LL2260: if((
unsigned int) _temp2224 > 4u?*(( int*) _temp2224) == Cyc_Absyn_RgnHandleType: 0){
_LL2357: _temp2356=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp2224)->f1;
goto _LL2261;} else{ goto _LL2262;} _LL2262: if(( unsigned int) _temp2224 > 4u?*((
int*) _temp2224) == Cyc_Absyn_AccessEff: 0){ _LL2359: _temp2358=( void*)((
struct Cyc_Absyn_AccessEff_struct*) _temp2224)->f1; goto _LL2263;} else{ goto
_LL2264;} _LL2264: if(( unsigned int) _temp2224 > 4u?*(( int*) _temp2224) == Cyc_Absyn_JoinEff:
0){ _LL2361: _temp2360=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*)
_temp2224)->f1; goto _LL2265;} else{ goto _LL2225;} _LL2227: goto _LL2225;
_LL2229: goto _LL2225; _LL2231: { struct Cyc_Absyn_Conref* c=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp2272->kind); if((
void*) c->v ==( void*) Cyc_Absyn_No_constr){( void*)( c->v=( void*)(( void*)({
struct Cyc_Absyn_Eq_constr_struct* _temp2362=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp2362[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp2363; _temp2363.tag= Cyc_Absyn_Eq_constr;
_temp2363.f1=( void*) expected_kind; _temp2363;}); _temp2362;})));} free_vars=
Cyc_Tcutil_add_free_tvar( loc, free_vars, _temp2272); goto _LL2225;} _LL2233:
if(* _temp2276 == 0){ struct _handler_cons _temp2364; _push_handler(& _temp2364);{
void* _temp2365=( void*) setjmp( _temp2364.handler); if( ! _temp2365){{ struct
Cyc_Absyn_Enumdecl** ed= Cyc_Tcenv_lookup_enumdecl( te, loc, _temp2277);*
_temp2276=( struct Cyc_Absyn_Enumdecl*)* ed;}; _pop_handler();} else{ void*
_temp2367= _temp2365; _LL2369: if( _temp2367 == Cyc_Dict_Absent){ goto _LL2370;}
else{ goto _LL2371;} _LL2371: goto _LL2372; _LL2370: Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2373= Cyc_Absynpp_qvar2string( _temp2277); xprintf("unbound type enum %.*s",
_temp2373.last_plus_one - _temp2373.curr, _temp2373.curr);})); return free_vars;
_LL2372:( void) _throw( _temp2367); _LL2368:;}}}{ struct Cyc_Absyn_Enumdecl* ed=(
struct Cyc_Absyn_Enumdecl*)({ struct Cyc_Absyn_Enumdecl* _temp2376=* _temp2276;
if( _temp2376 == 0){ _throw( Null_Exception);} _temp2376;});* _temp2277=({
struct _tuple1* _temp2374= ed->name; unsigned int _temp2375= 0; if( _temp2375 >=
1u){ _throw( Null_Exception);} _temp2374[ _temp2375];}); goto _LL2225;} _LL2235:{
void* _temp2377=* _temp2287; struct Cyc_Absyn_UnknownTunionInfo _temp2383; int
_temp2385; struct _tuple1* _temp2387; struct Cyc_Absyn_Tuniondecl* _temp2389;
_LL2379: if(*(( int*) _temp2377) == Cyc_Absyn_UnknownTunion){ _LL2384: _temp2383=(
struct Cyc_Absyn_UnknownTunionInfo)(( struct Cyc_Absyn_UnknownTunion_struct*)
_temp2377)->f1; _LL2388: _temp2387=( struct _tuple1*) _temp2383.name; goto
_LL2386; _LL2386: _temp2385=( int) _temp2383.is_xtunion; goto _LL2380;} else{
goto _LL2381;} _LL2381: if(*(( int*) _temp2377) == Cyc_Absyn_KnownTunion){
_LL2390: _temp2389=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_KnownTunion_struct*)
_temp2377)->f1; goto _LL2382;} else{ goto _LL2378;} _LL2380: { struct Cyc_Absyn_Tuniondecl*
tud;{ struct _handler_cons _temp2391; _push_handler(& _temp2391);{ void*
_temp2392=( void*) setjmp( _temp2391.handler); if( ! _temp2392){ tud=* Cyc_Tcenv_lookup_tuniondecl(
te, loc, _temp2387);; _pop_handler();} else{ void* _temp2394= _temp2392; _LL2396:
if( _temp2394 == Cyc_Dict_Absent){ goto _LL2397;} else{ goto _LL2398;} _LL2398:
goto _LL2399; _LL2397: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2400=
Cyc_Absynpp_qvar2string( _temp2387); xprintf("unbound type tunion %.*s",
_temp2400.last_plus_one - _temp2400.curr, _temp2400.curr);})); return free_vars;
_LL2399:( void) _throw( _temp2394); _LL2395:;}}} if( tud->is_xtunion !=
_temp2385){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2401= Cyc_Absynpp_qvar2string(
_temp2387); xprintf("[x]tunion is different from type declaration %.*s",
_temp2401.last_plus_one - _temp2401.curr, _temp2401.curr);}));}* _temp2287=(
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp2402=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp2402[ 0]=({
struct Cyc_Absyn_KnownTunion_struct _temp2403; _temp2403.tag= Cyc_Absyn_KnownTunion;
_temp2403.f1= tud; _temp2403;}); _temp2402;}); _temp2389= tud; goto _LL2382;}
_LL2382: free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars,( void*) Cyc_Absyn_RgnKind,
_temp2281);{ struct Cyc_List_List* tvs= _temp2389->tvs; for( 0; _temp2283 != 0?
tvs != 0: 0; _temp2283=({ struct Cyc_List_List* _temp2404= _temp2283; if(
_temp2404 == 0){ _throw( Null_Exception);} _temp2404->tl;}), tvs=({ struct Cyc_List_List*
_temp2405= tvs; if( _temp2405 == 0){ _throw( Null_Exception);} _temp2405->tl;})){
void* t1=( void*)({ struct Cyc_List_List* _temp2407= _temp2283; if( _temp2407 ==
0){ _throw( Null_Exception);} _temp2407->hd;}); void* k1=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp2406= tvs; if( _temp2406 == 0){ _throw( Null_Exception);} _temp2406->hd;}))->kind);
free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars, k1, t1);} if(
_temp2283 != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2408= Cyc_Absynpp_qvar2string(
_temp2389->name); xprintf("too many type arguments for tunion %.*s", _temp2408.last_plus_one
- _temp2408.curr, _temp2408.curr);}));} if( tvs != 0){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2409= Cyc_Absynpp_qvar2string( _temp2389->name);
xprintf("too few type arguments for tunion %.*s", _temp2409.last_plus_one -
_temp2409.curr, _temp2409.curr);}));} goto _LL2378;} _LL2378:;} goto _LL2225;
_LL2237:{ void* _temp2410=* _temp2294; struct Cyc_Absyn_UnknownTunionFieldInfo
_temp2416; int _temp2418; struct _tuple1* _temp2420; struct _tuple1* _temp2422;
struct Cyc_Absyn_Tunionfield* _temp2424; struct Cyc_Absyn_Tuniondecl* _temp2426;
_LL2412: if(*(( int*) _temp2410) == Cyc_Absyn_UnknownTunionfield){ _LL2417:
_temp2416=( struct Cyc_Absyn_UnknownTunionFieldInfo)(( struct Cyc_Absyn_UnknownTunionfield_struct*)
_temp2410)->f1; _LL2423: _temp2422=( struct _tuple1*) _temp2416.tunion_name;
goto _LL2421; _LL2421: _temp2420=( struct _tuple1*) _temp2416.field_name; goto
_LL2419; _LL2419: _temp2418=( int) _temp2416.is_xtunion; goto _LL2413;} else{
goto _LL2414;} _LL2414: if(*(( int*) _temp2410) == Cyc_Absyn_KnownTunionfield){
_LL2427: _temp2426=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp2410)->f1; goto _LL2425; _LL2425: _temp2424=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp2410)->f2; goto _LL2415;} else{
goto _LL2411;} _LL2413: { struct Cyc_Absyn_Tuniondecl* tud; struct Cyc_Absyn_Tunionfield*
tuf;{ struct _handler_cons _temp2428; _push_handler(& _temp2428);{ void*
_temp2429=( void*) setjmp( _temp2428.handler); if( ! _temp2429){ tud=* Cyc_Tcenv_lookup_tuniondecl(
te, loc, _temp2422);; _pop_handler();} else{ void* _temp2431= _temp2429; _LL2433:
if( _temp2431 == Cyc_Dict_Absent){ goto _LL2434;} else{ goto _LL2435;} _LL2435:
goto _LL2436; _LL2434: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2437=
Cyc_Absynpp_qvar2string( _temp2422); xprintf("unbound type tunion %.*s",
_temp2437.last_plus_one - _temp2437.curr, _temp2437.curr);})); return free_vars;
_LL2436:( void) _throw( _temp2431); _LL2432:;}}} if( tud->is_xtunion !=
_temp2418){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2438= Cyc_Absynpp_qvar2string(
_temp2422); xprintf("[x]tunion is different from type declaration %.*s",
_temp2438.last_plus_one - _temp2438.curr, _temp2438.curr);}));}{ struct
_handler_cons _temp2439; _push_handler(& _temp2439);{ void* _temp2440=( void*)
setjmp( _temp2439.handler); if( ! _temp2440){{ void* _temp2441= Cyc_Tcenv_lookup_ordinary(
te, loc, _temp2420); struct Cyc_Absyn_Tunionfield* _temp2447; struct Cyc_Absyn_Tuniondecl*
_temp2449; _LL2443: if(*(( int*) _temp2441) == Cyc_Tcenv_TunionRes){ _LL2450:
_temp2449=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Tcenv_TunionRes_struct*)
_temp2441)->f1; goto _LL2448; _LL2448: _temp2447=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Tcenv_TunionRes_struct*) _temp2441)->f2; goto _LL2444;} else{ goto
_LL2445;} _LL2445: goto _LL2446; _LL2444: tuf= _temp2447; if( _temp2449 != tud){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2451= Cyc_Absynpp_qvar2string(
_temp2420); struct _tagged_string _temp2452= Cyc_Absynpp_qvar2string( _temp2422);
xprintf("unbound field %.*s in type tunion %.*s", _temp2451.last_plus_one -
_temp2451.curr, _temp2451.curr, _temp2452.last_plus_one - _temp2452.curr,
_temp2452.curr);}));{ struct Cyc_List_List* _temp2453= free_vars; _npop_handler(
0u); return _temp2453;}} goto _LL2442; _LL2446: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2454= Cyc_Absynpp_qvar2string( _temp2420); struct
_tagged_string _temp2455= Cyc_Absynpp_qvar2string( _temp2422); xprintf("unbound field %.*s in type tunion %.*s",
_temp2454.last_plus_one - _temp2454.curr, _temp2454.curr, _temp2455.last_plus_one
- _temp2455.curr, _temp2455.curr);}));{ struct Cyc_List_List* _temp2456=
free_vars; _npop_handler( 0u); return _temp2456;} _LL2442:;}; _pop_handler();}
else{ void* _temp2458= _temp2440; _LL2460: if( _temp2458 == Cyc_Dict_Absent){
goto _LL2461;} else{ goto _LL2462;} _LL2462: goto _LL2463; _LL2461: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2464= Cyc_Absynpp_qvar2string( _temp2420);
struct _tagged_string _temp2465= Cyc_Absynpp_qvar2string( _temp2422); xprintf("unbound field %.*s in type tunion %.*s",
_temp2464.last_plus_one - _temp2464.curr, _temp2464.curr, _temp2465.last_plus_one
- _temp2465.curr, _temp2465.curr);})); return free_vars; _LL2463:( void) _throw(
_temp2458); _LL2459:;}}}* _temp2294=( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp2466=( struct Cyc_Absyn_KnownTunionfield_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp2466[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp2467; _temp2467.tag= Cyc_Absyn_KnownTunionfield; _temp2467.f1= tud;
_temp2467.f2= tuf; _temp2467;}); _temp2466;}); _temp2426= tud; _temp2424= tuf;
goto _LL2415;} _LL2415: { struct Cyc_List_List* tvs= _temp2426->tvs; for( 0;
_temp2290 != 0? tvs != 0: 0; _temp2290=({ struct Cyc_List_List* _temp2468=
_temp2290; if( _temp2468 == 0){ _throw( Null_Exception);} _temp2468->tl;}), tvs=({
struct Cyc_List_List* _temp2469= tvs; if( _temp2469 == 0){ _throw(
Null_Exception);} _temp2469->tl;})){ void* t1=( void*)({ struct Cyc_List_List*
_temp2471= _temp2290; if( _temp2471 == 0){ _throw( Null_Exception);} _temp2471->hd;});
void* k1=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(((
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp2470= tvs; if( _temp2470 ==
0){ _throw( Null_Exception);} _temp2470->hd;}))->kind); free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars, k1, t1);} if( _temp2290 != 0){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2472= Cyc_Absynpp_qvar2string( _temp2426->name);
struct _tagged_string _temp2473= Cyc_Absynpp_qvar2string( _temp2424->name);
xprintf("too many type arguments for tunion %.*s.%.*s", _temp2472.last_plus_one
- _temp2472.curr, _temp2472.curr, _temp2473.last_plus_one - _temp2473.curr,
_temp2473.curr);}));} if( tvs != 0){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2474= Cyc_Absynpp_qvar2string( _temp2426->name); struct
_tagged_string _temp2475= Cyc_Absynpp_qvar2string( _temp2424->name); xprintf("too few type arguments for tunion %.*s.%.*s",
_temp2474.last_plus_one - _temp2474.curr, _temp2474.curr, _temp2475.last_plus_one
- _temp2475.curr, _temp2475.curr);}));} goto _LL2411;} _LL2411:;} goto _LL2225;
_LL2239: free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars,( void*) Cyc_Absyn_AnyKind,
_temp2305); free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars,( void*)
Cyc_Absyn_RgnKind, _temp2303);{ void* _temp2476=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp2297))->v; void*
_temp2482; struct Cyc_Absyn_Exp* _temp2484; _LL2478: if(( unsigned int)
_temp2476 > 1u?*(( int*) _temp2476) == Cyc_Absyn_Eq_constr: 0){ _LL2483:
_temp2482=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2476)->f1; if((
unsigned int) _temp2482 > 1u?*(( int*) _temp2482) == Cyc_Absyn_Upper_b: 0){
_LL2485: _temp2484=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp2482)->f1; goto _LL2479;} else{ goto _LL2480;}} else{ goto _LL2480;}
_LL2480: goto _LL2481; _LL2479: if( ! Cyc_Tcutil_is_const_exp( te, _temp2484)){
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp2486=( char*)"pointer bounds expression is not a constant";
struct _tagged_string _temp2487; _temp2487.curr= _temp2486; _temp2487.base=
_temp2486; _temp2487.last_plus_one= _temp2486 + 44; _temp2487;}));} Cyc_Tcexp_tcExp(
te, 0, _temp2484); if( ! Cyc_Tcutil_coerce_uint_typ( te, _temp2484)){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp2488=( char*)"pointer bounds expression is not an unsigned int";
struct _tagged_string _temp2489; _temp2489.curr= _temp2488; _temp2489.base=
_temp2488; _temp2489.last_plus_one= _temp2488 + 49; _temp2489;}));} Cyc_Evexp_eval_const_uint_exp(
_temp2484); goto _LL2477; _LL2481: goto _LL2477; _LL2477:;} goto _LL2225;
_LL2241: goto _LL2225; _LL2243: goto _LL2225; _LL2245: goto _LL2225; _LL2247:
free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars,( void*) Cyc_Absyn_MemKind,
_temp2315); if( _temp2311 == 0? 1: ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp2490= _temp2311; if( _temp2490 == 0){ _throw(
Null_Exception);} _temp2490;}))){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({
char* _temp2491=( char*)"array bounds expression is not constant"; struct
_tagged_string _temp2492; _temp2492.curr= _temp2491; _temp2492.base= _temp2491;
_temp2492.last_plus_one= _temp2491 + 40; _temp2492;}));} Cyc_Tcexp_tcExp( te, 0,(
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2493= _temp2311; if(
_temp2493 == 0){ _throw( Null_Exception);} _temp2493;})); if( ! Cyc_Tcutil_coerce_uint_typ(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2494= _temp2311; if(
_temp2494 == 0){ _throw( Null_Exception);} _temp2494;}))){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp2495=( char*)"array bounds expression is not an unsigned int";
struct _tagged_string _temp2496; _temp2496.curr= _temp2495; _temp2496.base=
_temp2495; _temp2496.last_plus_one= _temp2495 + 47; _temp2496;}));} Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2497= _temp2311; if(
_temp2497 == 0){ _throw( Null_Exception);} _temp2497;})); goto _LL2225; _LL2249:
if( _temp2319 != 0){ int seen_cdecl= 0; int seen_stdcall= 0; for( 0; _temp2319
!= 0; _temp2319=({ struct Cyc_List_List* _temp2498= _temp2319; if( _temp2498 ==
0){ _throw( Null_Exception);} _temp2498->tl;})){ if( ! Cyc_Tcutil_fntype_att((
void*)({ struct Cyc_List_List* _temp2499= _temp2319; if( _temp2499 == 0){ _throw(
Null_Exception);} _temp2499->hd;}))){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2501= Cyc_Absyn_attribute2string(( void*)({ struct Cyc_List_List*
_temp2500= _temp2319; if( _temp2500 == 0){ _throw( Null_Exception);} _temp2500->hd;}));
xprintf("bad function type attribute %.*s", _temp2501.last_plus_one - _temp2501.curr,
_temp2501.curr);}));} if(( void*)({ struct Cyc_List_List* _temp2502= _temp2319;
if( _temp2502 == 0){ _throw( Null_Exception);} _temp2502->hd;}) ==( void*) Cyc_Absyn_Stdcall_att){
if( seen_cdecl){ Cyc_Tcutil_terr( loc, xprintf("function can't be both cdecl and stdcall"));}
seen_stdcall= 1;} else{ if(( void*)({ struct Cyc_List_List* _temp2503= _temp2319;
if( _temp2503 == 0){ _throw( Null_Exception);} _temp2503->hd;}) ==( void*) Cyc_Absyn_Cdecl_att){
if( seen_stdcall){ Cyc_Tcutil_terr( loc, xprintf("function can't be both cdecl and stdcall"));}
seen_cdecl= 1;}}}} Cyc_Tcutil_check_unique_tvars( loc,* _temp2332);{ struct Cyc_List_List*
b=* _temp2332; for( 0; b != 0; b=({ struct Cyc_List_List* _temp2504= b; if(
_temp2504 == 0){ _throw( Null_Exception);} _temp2504->tl;})){ void* _temp2506=(
void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp2505= b; if( _temp2505 == 0){
_throw( Null_Exception);} _temp2505->hd;}))->kind))->v; void* _temp2512; _LL2508:
if(( unsigned int) _temp2506 > 1u?*(( int*) _temp2506) == Cyc_Absyn_Eq_constr: 0){
_LL2513: _temp2512=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2506)->f1;
if( _temp2512 ==( void*) Cyc_Absyn_MemKind){ goto _LL2509;} else{ goto _LL2510;}}
else{ goto _LL2510;} _LL2510: goto _LL2511; _LL2509: Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2515=*(( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp2514= b; if( _temp2514 == 0){ _throw( Null_Exception);} _temp2514->hd;}))->name;
xprintf("function attempts to abstract Mem type variable %.*s", _temp2515.last_plus_one
- _temp2515.curr, _temp2515.curr);})); goto _LL2507; _LL2511: goto _LL2507;
_LL2507:;}}{ struct Cyc_List_List* new_free_vars= 0; new_free_vars= Cyc_Tcutil_check_valid_type(
loc, te, new_free_vars,( void*) Cyc_Absyn_MemKind, _temp2325); for( 0; _temp2323
!= 0; _temp2323=({ struct Cyc_List_List* _temp2516= _temp2323; if( _temp2516 ==
0){ _throw( Null_Exception);} _temp2516->tl;})){ new_free_vars= Cyc_Tcutil_check_valid_type(
loc, te, new_free_vars,( void*) Cyc_Absyn_MemKind,(*(( struct _tuple2*)({ struct
Cyc_List_List* _temp2517= _temp2323; if( _temp2517 == 0){ _throw( Null_Exception);}
_temp2517->hd;}))).f3);} if(* _temp2329 != 0){ new_free_vars= Cyc_Tcutil_check_valid_type(
loc, te, new_free_vars,( void*) Cyc_Absyn_EffKind,( void*)({ struct Cyc_Core_Opt*
_temp2518=* _temp2329; if( _temp2518 == 0){ _throw( Null_Exception);} _temp2518->v;}));}
else{ struct Cyc_Absyn_Tvar* e= Cyc_Tcutil_new_tvar(( void*) Cyc_Absyn_EffKind);
new_free_vars=({ struct Cyc_List_List* _temp2519=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2519->hd=( void*) e; _temp2519->tl=
new_free_vars; _temp2519;});{ struct Cyc_List_List* effect= 0;{ struct Cyc_List_List*
tvs= new_free_vars; for( 0; tvs != 0; tvs=({ struct Cyc_List_List* _temp2520=
tvs; if( _temp2520 == 0){ _throw( Null_Exception);} _temp2520->tl;})){ void*
_temp2522=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp2521= tvs; if( _temp2521 == 0){ _throw( Null_Exception);} _temp2521->hd;}))->kind))->v;
void* _temp2530; void* _temp2532; _LL2524: if(( unsigned int) _temp2522 > 1u?*((
int*) _temp2522) == Cyc_Absyn_Eq_constr: 0){ _LL2531: _temp2530=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp2522)->f1; if( _temp2530 ==( void*) Cyc_Absyn_RgnKind){
goto _LL2525;} else{ goto _LL2526;}} else{ goto _LL2526;} _LL2526: if((
unsigned int) _temp2522 > 1u?*(( int*) _temp2522) == Cyc_Absyn_Eq_constr: 0){
_LL2533: _temp2532=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2522)->f1;
if( _temp2532 ==( void*) Cyc_Absyn_EffKind){ goto _LL2527;} else{ goto _LL2528;}}
else{ goto _LL2528;} _LL2528: goto _LL2529; _LL2525: effect=({ struct Cyc_List_List*
_temp2534=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2534->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp2535=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp2535[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp2536; _temp2536.tag= Cyc_Absyn_AccessEff;
_temp2536.f1=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp2537=(
struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp2537[ 0]=({ struct Cyc_Absyn_VarType_struct _temp2538; _temp2538.tag= Cyc_Absyn_VarType;
_temp2538.f1=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp2539= tvs;
if( _temp2539 == 0){ _throw( Null_Exception);} _temp2539->hd;}); _temp2538;});
_temp2537;})); _temp2536;}); _temp2535;})); _temp2534->tl= effect; _temp2534;});
goto _LL2523; _LL2527: effect=({ struct Cyc_List_List* _temp2540=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2540->hd=( void*)(( void*)({
struct Cyc_Absyn_VarType_struct* _temp2541=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp2541[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp2542; _temp2542.tag= Cyc_Absyn_VarType; _temp2542.f1=(
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp2543= tvs; if( _temp2543 ==
0){ _throw( Null_Exception);} _temp2543->hd;}); _temp2542;}); _temp2541;}));
_temp2540->tl= effect; _temp2540;}); goto _LL2523; _LL2529: goto _LL2523;
_LL2523:;}}* _temp2329=({ struct Cyc_Core_Opt* _temp2544=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2544->v=( void*)(( void*)({
struct Cyc_Absyn_JoinEff_struct* _temp2545=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp2545[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp2546; _temp2546.tag= Cyc_Absyn_JoinEff; _temp2546.f1=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( effect);
_temp2546;}); _temp2545;})); _temp2544;});}} if(* _temp2332 != 0){ struct Cyc_List_List*
new_btvs= 0;{ struct Cyc_List_List* bs=* _temp2332; for( 0; bs != 0; bs=({
struct Cyc_List_List* _temp2547= bs; if( _temp2547 == 0){ _throw( Null_Exception);}
_temp2547->tl;})){ new_free_vars= Cyc_Tcutil_add_free_tvar( loc, new_free_vars,(
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp2548= bs; if( _temp2548 ==
0){ _throw( Null_Exception);} _temp2548->hd;}));{ struct Cyc_Absyn_Conref* c=((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp2563= bs; if( _temp2563 ==
0){ _throw( Null_Exception);} _temp2563->hd;}))->kind); void* _temp2549=( void*)
c->v; void* _temp2557; _LL2551: if( _temp2549 ==( void*) Cyc_Absyn_No_constr){
goto _LL2552;} else{ goto _LL2553;} _LL2553: if(( unsigned int) _temp2549 > 1u?*((
int*) _temp2549) == Cyc_Absyn_Eq_constr: 0){ _LL2558: _temp2557=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp2549)->f1; if( _temp2557 ==( void*) Cyc_Absyn_MemKind){
goto _LL2554;} else{ goto _LL2555;}} else{ goto _LL2555;} _LL2555: goto _LL2556;
_LL2552: Cyc_Tcutil_warn( loc,({ struct _tagged_string _temp2560=*(( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp2559= bs; if( _temp2559 == 0){ _throw( Null_Exception);}
_temp2559->hd;}))->name; xprintf("Type variable %.*s unconstrained, assuming boxed",
_temp2560.last_plus_one - _temp2560.curr, _temp2560.curr);})); goto _LL2554;
_LL2554:( void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp2561=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2561[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp2562; _temp2562.tag= Cyc_Absyn_Eq_constr;
_temp2562.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp2562;}); _temp2561;})));
goto _LL2550; _LL2556: goto _LL2550; _LL2550:;}}} new_free_vars= Cyc_Tcutil_remove_bound_tvars(
new_free_vars,* _temp2332);} for( 0; new_free_vars != 0; new_free_vars=({ struct
Cyc_List_List* _temp2564= new_free_vars; if( _temp2564 == 0){ _throw(
Null_Exception);} _temp2564->tl;})){ free_vars= Cyc_Tcutil_add_free_tvar( loc,
free_vars,( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp2565=
new_free_vars; if( _temp2565 == 0){ _throw( Null_Exception);} _temp2565->hd;}));}
goto _LL2225;} _LL2251: for( 0; _temp2333 != 0; _temp2333=({ struct Cyc_List_List*
_temp2566= _temp2333; if( _temp2566 == 0){ _throw( Null_Exception);} _temp2566->tl;})){
free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars,( void*) Cyc_Absyn_MemKind,(*((
struct _tuple4*)({ struct Cyc_List_List* _temp2567= _temp2333; if( _temp2567 ==
0){ _throw( Null_Exception);} _temp2567->hd;}))).f2);} goto _LL2225; _LL2253:
if( _temp2340 == 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp2568=( char*)"missing struct name"; struct _tagged_string _temp2569;
_temp2569.curr= _temp2568; _temp2569.base= _temp2568; _temp2569.last_plus_one=
_temp2568 + 20; _temp2569;})); return free_vars;} if(* _temp2337 == 0){ struct
_handler_cons _temp2570; _push_handler(& _temp2570);{ void* _temp2571=( void*)
setjmp( _temp2570.handler); if( ! _temp2571){* _temp2337=( struct Cyc_Absyn_Structdecl**)
Cyc_Tcenv_lookup_structdecl( te, loc,( struct _tuple1*)({ struct _tuple1*
_temp2572= _temp2340; if( _temp2572 == 0){ _throw( Null_Exception);} _temp2572;}));;
_pop_handler();} else{ void* _temp2574= _temp2571; _LL2576: if( _temp2574 == Cyc_Dict_Absent){
goto _LL2577;} else{ goto _LL2578;} _LL2578: goto _LL2579; _LL2577: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2581= Cyc_Absynpp_qvar2string(( struct _tuple1*)({
struct _tuple1* _temp2580= _temp2340; if( _temp2580 == 0){ _throw(
Null_Exception);} _temp2580;})); xprintf("unbound type struct %.*s", _temp2581.last_plus_one
- _temp2581.curr, _temp2581.curr);})); return free_vars; _LL2579:( void) _throw(
_temp2574); _LL2575:;}}}{ struct Cyc_Absyn_Structdecl* sd=({ struct Cyc_Absyn_Structdecl**
_temp2594=* _temp2337; if( _temp2594 == 0){ _throw( Null_Exception);}* _temp2594;});
if( sd->name != 0){({ struct _tuple1* _temp2582= _temp2340; if( _temp2582 == 0){
_throw( Null_Exception);}* _temp2582=({ struct _tuple1* _temp2584=( struct
_tuple1*)({ struct Cyc_Core_Opt* _temp2583= sd->name; if( _temp2583 == 0){
_throw( Null_Exception);} _temp2583->v;}); unsigned int _temp2585= 0; if(
_temp2585 >= 1u){ _throw( Null_Exception);} _temp2584[ _temp2585];});});}{
struct Cyc_List_List* tvs= sd->tvs; int lvs=(( int(*)( struct Cyc_List_List* x))
Cyc_List_length)( tvs); int largs=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp2338); if( lvs != largs){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2587= Cyc_Absynpp_qvar2string(( struct _tuple1*)({ struct _tuple1*
_temp2586= _temp2340; if( _temp2586 == 0){ _throw( Null_Exception);} _temp2586;}));
int _temp2588= lvs; int _temp2589= largs; xprintf("struct %.*s expects %d type arguments but was given %d",
_temp2587.last_plus_one - _temp2587.curr, _temp2587.curr, _temp2588, _temp2589);}));}
for( 0; _temp2338 != 0; _temp2338=({ struct Cyc_List_List* _temp2590= _temp2338;
if( _temp2590 == 0){ _throw( Null_Exception);} _temp2590->tl;}), tvs=({ struct
Cyc_List_List* _temp2591= tvs; if( _temp2591 == 0){ _throw( Null_Exception);}
_temp2591->tl;})){ void* t1=( void*)({ struct Cyc_List_List* _temp2593=
_temp2338; if( _temp2593 == 0){ _throw( Null_Exception);} _temp2593->hd;}); void*
k1=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp2592= tvs; if( _temp2592 == 0){ _throw(
Null_Exception);} _temp2592->hd;}))->kind); free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars, k1, t1);} goto _LL2225;}} _LL2255: if( _temp2347 == 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp2595=( char*)"missing union name";
struct _tagged_string _temp2596; _temp2596.curr= _temp2595; _temp2596.base=
_temp2595; _temp2596.last_plus_one= _temp2595 + 19; _temp2596;})); return
free_vars;} if(* _temp2344 == 0){ struct _handler_cons _temp2597; _push_handler(&
_temp2597);{ void* _temp2598=( void*) setjmp( _temp2597.handler); if( !
_temp2598){* _temp2344=( struct Cyc_Absyn_Uniondecl**) Cyc_Tcenv_lookup_uniondecl(
te, loc,( struct _tuple1*)({ struct _tuple1* _temp2599= _temp2347; if( _temp2599
== 0){ _throw( Null_Exception);} _temp2599;}));; _pop_handler();} else{ void*
_temp2601= _temp2598; _LL2603: if( _temp2601 == Cyc_Dict_Absent){ goto _LL2604;}
else{ goto _LL2605;} _LL2605: goto _LL2606; _LL2604: Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp2608= Cyc_Absynpp_qvar2string(( struct _tuple1*)({
struct _tuple1* _temp2607= _temp2347; if( _temp2607 == 0){ _throw(
Null_Exception);} _temp2607;})); xprintf("unbound type union %.*s", _temp2608.last_plus_one
- _temp2608.curr, _temp2608.curr);})); return free_vars; _LL2606:( void) _throw(
_temp2601); _LL2602:;}}}{ struct Cyc_Absyn_Uniondecl* ud=({ struct Cyc_Absyn_Uniondecl**
_temp2621=* _temp2344; if( _temp2621 == 0){ _throw( Null_Exception);}* _temp2621;});
if( ud->name != 0){({ struct _tuple1* _temp2609= _temp2347; if( _temp2609 == 0){
_throw( Null_Exception);}* _temp2609=({ struct _tuple1* _temp2611=( struct
_tuple1*)({ struct Cyc_Core_Opt* _temp2610= ud->name; if( _temp2610 == 0){
_throw( Null_Exception);} _temp2610->v;}); unsigned int _temp2612= 0; if(
_temp2612 >= 1u){ _throw( Null_Exception);} _temp2611[ _temp2612];});});}{
struct Cyc_List_List* tvs= ud->tvs; int lvs=(( int(*)( struct Cyc_List_List* x))
Cyc_List_length)( tvs); int largs=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp2345); if( lvs != largs){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2614= Cyc_Absynpp_qvar2string(( struct _tuple1*)({ struct _tuple1*
_temp2613= _temp2347; if( _temp2613 == 0){ _throw( Null_Exception);} _temp2613;}));
int _temp2615= lvs; int _temp2616= largs; xprintf("union %.*s expects %d type arguments but was given %d",
_temp2614.last_plus_one - _temp2614.curr, _temp2614.curr, _temp2615, _temp2616);}));}
for( 0; _temp2345 != 0; _temp2345=({ struct Cyc_List_List* _temp2617= _temp2345;
if( _temp2617 == 0){ _throw( Null_Exception);} _temp2617->tl;}), tvs=({ struct
Cyc_List_List* _temp2618= tvs; if( _temp2618 == 0){ _throw( Null_Exception);}
_temp2618->tl;})){ void* t1=( void*)({ struct Cyc_List_List* _temp2620=
_temp2345; if( _temp2620 == 0){ _throw( Null_Exception);} _temp2620->hd;}); void*
k1=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp2619= tvs; if( _temp2619 == 0){ _throw(
Null_Exception);} _temp2619->hd;}))->kind); free_vars= Cyc_Tcutil_check_valid_type(
loc, te, free_vars, k1, t1);} goto _LL2225;}} _LL2257: { struct Cyc_Absyn_Typedefdecl*
td;{ struct _handler_cons _temp2622; _push_handler(& _temp2622);{ void*
_temp2623=( void*) setjmp( _temp2622.handler); if( ! _temp2623){ td= Cyc_Tcenv_lookup_typedefdecl(
te, loc, _temp2354);; _pop_handler();} else{ void* _temp2625= _temp2623; _LL2627:
if( _temp2625 == Cyc_Dict_Absent){ goto _LL2628;} else{ goto _LL2629;} _LL2629:
goto _LL2630; _LL2628: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2631=
Cyc_Absynpp_qvar2string( _temp2354); xprintf("unbound typedef name %.*s",
_temp2631.last_plus_one - _temp2631.curr, _temp2631.curr);})); return free_vars;
_LL2630:( void) _throw( _temp2625); _LL2626:;}}}({ struct _tuple1* _temp2632=
_temp2354; unsigned int _temp2633= 0; if( _temp2633 >= 1u){ _throw(
Null_Exception);} _temp2632[ _temp2633]=({ struct _tuple1* _temp2634= td->name;
unsigned int _temp2635= 0; if( _temp2635 >= 1u){ _throw( Null_Exception);}
_temp2634[ _temp2635];});});{ struct Cyc_List_List* tvs= td->tvs; struct Cyc_List_List*
ts= _temp2352; struct Cyc_List_List* inst= 0; for( 0; ts != 0? tvs != 0: 0; ts=({
struct Cyc_List_List* _temp2636= ts; if( _temp2636 == 0){ _throw( Null_Exception);}
_temp2636->tl;}), tvs=({ struct Cyc_List_List* _temp2637= tvs; if( _temp2637 ==
0){ _throw( Null_Exception);} _temp2637->tl;})){ void* k=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp2643= tvs; if( _temp2643 == 0){ _throw( Null_Exception);} _temp2643->hd;}))->kind);
free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars, k,( void*)({ struct
Cyc_List_List* _temp2638= ts; if( _temp2638 == 0){ _throw( Null_Exception);}
_temp2638->hd;})); inst=({ struct Cyc_List_List* _temp2639=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2639->hd=( void*)({ struct
_tuple5* _temp2640=( struct _tuple5*) GC_malloc( sizeof( struct _tuple5));
_temp2640->f1=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp2642= tvs;
if( _temp2642 == 0){ _throw( Null_Exception);} _temp2642->hd;}); _temp2640->f2=(
void*)({ struct Cyc_List_List* _temp2641= ts; if( _temp2641 == 0){ _throw(
Null_Exception);} _temp2641->hd;}); _temp2640;}); _temp2639->tl= inst; _temp2639;});}
if( tvs != 0? 1: ts != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp2644= Cyc_Absynpp_qvar2string( _temp2354); xprintf("typedef %.*s expects a different number of arguments",
_temp2644.last_plus_one - _temp2644.curr, _temp2644.curr);})); return free_vars;}{
void* new_typ= Cyc_Tcutil_substitute( inst,( void*) td->defn);* _temp2351=({
struct Cyc_Core_Opt* _temp2645=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2645->v=( void*) new_typ; _temp2645;}); goto _LL2225;}}}
_LL2259: goto _LL2225; _LL2261: _temp2358= _temp2356; goto _LL2263; _LL2263:
free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars,( void*) Cyc_Absyn_RgnKind,
_temp2358); goto _LL2225; _LL2265: for( 0; _temp2360 != 0; _temp2360=({ struct
Cyc_List_List* _temp2646= _temp2360; if( _temp2646 == 0){ _throw( Null_Exception);}
_temp2646->tl;})){ free_vars= Cyc_Tcutil_check_valid_type( loc, te, free_vars,(
void*) Cyc_Absyn_EffKind,( void*)({ struct Cyc_List_List* _temp2647= _temp2360;
if( _temp2647 == 0){ _throw( Null_Exception);} _temp2647->hd;}));} goto _LL2225;
_LL2225:;} if( ! Cyc_Tcutil_kind_leq( Cyc_Tcutil_typ_kind( t), expected_kind)){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2648= Cyc_Absynpp_typ2string(
t); struct _tagged_string _temp2649= Cyc_Absynpp_kind2string( Cyc_Tcutil_typ_kind(
t)); struct _tagged_string _temp2650= Cyc_Absynpp_kind2string( expected_kind);
xprintf("type %.*s has kind %.*s but as used here needs kind %.*s", _temp2648.last_plus_one
- _temp2648.curr, _temp2648.curr, _temp2649.last_plus_one - _temp2649.curr,
_temp2649.curr, _temp2650.last_plus_one - _temp2650.curr, _temp2650.curr);}));}
return free_vars;} void Cyc_Tcutil_check_valid_toplevel_type( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, void* t){ struct Cyc_List_List* free_tvars= Cyc_Tcutil_check_valid_type(
loc, te, 0,( void*) Cyc_Absyn_MemKind, t);{ struct Cyc_List_List* x= free_tvars;
for( 0; x != 0; x=({ struct Cyc_List_List* _temp2651= x; if( _temp2651 == 0){
_throw( Null_Exception);} _temp2651->tl;})){ struct Cyc_Absyn_Conref* c=((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp2664= x; if( _temp2664 == 0){
_throw( Null_Exception);} _temp2664->hd;}))->kind); void* _temp2652=( void*) c->v;
void* _temp2660; _LL2654: if( _temp2652 ==( void*) Cyc_Absyn_No_constr){ goto
_LL2655;} else{ goto _LL2656;} _LL2656: if(( unsigned int) _temp2652 > 1u?*((
int*) _temp2652) == Cyc_Absyn_Eq_constr: 0){ _LL2661: _temp2660=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp2652)->f1; if( _temp2660 ==( void*) Cyc_Absyn_MemKind){
goto _LL2657;} else{ goto _LL2658;}} else{ goto _LL2658;} _LL2658: goto _LL2659;
_LL2655: goto _LL2657; _LL2657:( void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp2662=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2662[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp2663; _temp2663.tag= Cyc_Absyn_Eq_constr;
_temp2663.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp2663;}); _temp2662;})));
goto _LL2653; _LL2659: goto _LL2653; _LL2653:;}} if( free_tvars != 0){ void*
_temp2665= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo _temp2671; struct
Cyc_List_List* _temp2673; int _temp2675; struct Cyc_List_List* _temp2677; void*
_temp2679; struct Cyc_Core_Opt* _temp2681; struct Cyc_List_List* _temp2683;
struct Cyc_List_List** _temp2685; _LL2667: if(( unsigned int) _temp2665 > 4u?*((
int*) _temp2665) == Cyc_Absyn_FnType: 0){ _LL2672: _temp2671=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp2665)->f1; _LL2684: _temp2683=( struct Cyc_List_List*)
_temp2671.tvars; _temp2685=&((( struct Cyc_Absyn_FnType_struct*) _temp2665)->f1).tvars;
goto _LL2682; _LL2682: _temp2681=( struct Cyc_Core_Opt*) _temp2671.effect; goto
_LL2680; _LL2680: _temp2679=( void*) _temp2671.ret_typ; goto _LL2678; _LL2678:
_temp2677=( struct Cyc_List_List*) _temp2671.args; goto _LL2676; _LL2676:
_temp2675=( int) _temp2671.varargs; goto _LL2674; _LL2674: _temp2673=( struct
Cyc_List_List*) _temp2671.attributes; goto _LL2668;} else{ goto _LL2669;}
_LL2669: goto _LL2670; _LL2668: if(* _temp2685 == 0){* _temp2685= free_tvars;}
else{ if( free_tvars != 0){ struct _tagged_string s=*(( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp2690= free_tvars; if( _temp2690 == 0){ _throw(
Null_Exception);} _temp2690->hd;}))->name; if(({ struct _tagged_string _temp2686=
s; char* _temp2688= _temp2686.curr + 0; if( _temp2686.base == 0? 1:( _temp2688 <
_temp2686.base? 1: _temp2688 >= _temp2686.last_plus_one)){ _throw(
Null_Exception);}* _temp2688;}) =='?'){ Cyc_Tcutil_terr( loc, xprintf("You failed to abstract over the generated type variables. \nRemove the bound type variables or else make the effects of the type explicit\nand then abstract the free type variables properly."));}
else{ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2689= s; xprintf("unbound type variable %.*s",
_temp2689.last_plus_one - _temp2689.curr, _temp2689.curr);}));}}} goto _LL2666;
_LL2670: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp2692=*(( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp2691= free_tvars; if( _temp2691 == 0){ _throw(
Null_Exception);} _temp2691->hd;}))->name; xprintf("unbound type variable %.*s",
_temp2692.last_plus_one - _temp2692.curr, _temp2692.curr);})); return; _LL2666:;}}
void Cyc_Tcutil_check_type( struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* bound_tvars, void* expected_kind, void* t){ struct Cyc_List_List*
free_tvars= Cyc_Tcutil_check_valid_type( loc, te, 0, expected_kind, t); struct
Cyc_List_List* fs= free_tvars; for( 0; fs != 0; fs=({ struct Cyc_List_List*
_temp2693= fs; if( _temp2693 == 0){ _throw( Null_Exception);} _temp2693->tl;})){
struct Cyc_Absyn_Tvar* ftv=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp2701= fs; if( _temp2701 == 0){ _throw( Null_Exception);} _temp2701->hd;});
struct _tagged_string* ftvn= ftv->name; struct Cyc_Absyn_Conref* ftvk= ftv->kind;
int present= 0;{ struct Cyc_List_List* bs= bound_tvars; for( 0; bs != 0; bs=({
struct Cyc_List_List* _temp2694= bs; if( _temp2694 == 0){ _throw( Null_Exception);}
_temp2694->tl;})){ struct Cyc_Absyn_Tvar* btv=( struct Cyc_Absyn_Tvar*)({ struct
Cyc_List_List* _temp2698= bs; if( _temp2698 == 0){ _throw( Null_Exception);}
_temp2698->hd;}); struct _tagged_string* btvn= btv->name; struct Cyc_Absyn_Conref*
btvk= btv->kind; if( Cyc_String_zstrptrcmp( ftvn, btvn) == 0){ present= 1; if( !
Cyc_Tcutil_constrain_kinds( btvk, ftvk)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2695=* ftvn; struct _tagged_string _temp2696= Cyc_Absynpp_ckind2string(
btvk); struct _tagged_string _temp2697= Cyc_Absynpp_ckind2string( ftvk); xprintf("type variable %.*s has kind %.*s but is used here atkind %.*s",
_temp2695.last_plus_one - _temp2695.curr, _temp2695.curr, _temp2696.last_plus_one
- _temp2696.curr, _temp2696.curr, _temp2697.last_plus_one - _temp2697.curr,
_temp2697.curr);}));} break;}}} if( ! present){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2699=* ftvn; struct _tagged_string _temp2700= Cyc_Absynpp_typ2string(
t); xprintf("unbound type variable %.*s in type %.*s", _temp2699.last_plus_one -
_temp2699.curr, _temp2699.curr, _temp2700.last_plus_one - _temp2700.curr,
_temp2700.curr);}));}}} static void Cyc_Tcutil_check_unique_unsorted( int(* cmp)(
void*, void*), struct Cyc_List_List* vs, struct Cyc_Position_Segment* loc,
struct _tagged_string(* a2string)( void*), struct _tagged_string msg){ for( 0;
vs != 0; vs=({ struct Cyc_List_List* _temp2702= vs; if( _temp2702 == 0){ _throw(
Null_Exception);} _temp2702->tl;})){ struct Cyc_List_List* vs2=({ struct Cyc_List_List*
_temp2709= vs; if( _temp2709 == 0){ _throw( Null_Exception);} _temp2709->tl;});
for( 0; vs2 != 0; vs2=({ struct Cyc_List_List* _temp2703= vs2; if( _temp2703 ==
0){ _throw( Null_Exception);} _temp2703->tl;})){ if( cmp(( void*)({ struct Cyc_List_List*
_temp2704= vs; if( _temp2704 == 0){ _throw( Null_Exception);} _temp2704->hd;}),(
void*)({ struct Cyc_List_List* _temp2705= vs2; if( _temp2705 == 0){ _throw(
Null_Exception);} _temp2705->hd;})) == 0){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2707= msg; struct _tagged_string _temp2708= a2string(( void*)({
struct Cyc_List_List* _temp2706= vs; if( _temp2706 == 0){ _throw( Null_Exception);}
_temp2706->hd;})); xprintf("%.*s: %.*s", _temp2707.last_plus_one - _temp2707.curr,
_temp2707.curr, _temp2708.last_plus_one - _temp2708.curr, _temp2708.curr);}));}}}}
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
tvs, loc, Cyc_Tcutil_tvar2string,( struct _tagged_string)({ char* _temp2710=(
char*)"duplicate type variable"; struct _tagged_string _temp2711; _temp2711.curr=
_temp2710; _temp2711.base= _temp2710; _temp2711.last_plus_one= _temp2710 + 24;
_temp2711;}));} struct _tuple15{ struct Cyc_Absyn_Structfield* f1; int f2; } ;
struct _tuple16{ struct Cyc_List_List* f1; void* f2; } ; struct _tuple17{ struct
Cyc_Absyn_Structfield* f1; void* f2; } ; struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_Absyn_Structdecl* sd){ if( sd->fields == 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp2712=( char*)"inappropriate use of abstract struct";
struct _tagged_string _temp2713; _temp2713.curr= _temp2712; _temp2713.base=
_temp2712; _temp2713.last_plus_one= _temp2712 + 37; _temp2713;}));}{ struct Cyc_List_List*
fields= 0;{ struct Cyc_List_List* sd_fields=( struct Cyc_List_List*)({ struct
Cyc_Core_Opt* _temp2721= sd->fields; if( _temp2721 == 0){ _throw( Null_Exception);}
_temp2721->v;}); for( 0; sd_fields != 0; sd_fields=({ struct Cyc_List_List*
_temp2714= sd_fields; if( _temp2714 == 0){ _throw( Null_Exception);} _temp2714->tl;})){
if( Cyc_String_strcmp(*(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp2715= sd_fields; if( _temp2715 == 0){ _throw( Null_Exception);} _temp2715->hd;}))->name,(
struct _tagged_string)({ char* _temp2716=( char*)""; struct _tagged_string
_temp2717; _temp2717.curr= _temp2716; _temp2717.base= _temp2716; _temp2717.last_plus_one=
_temp2716 + 1; _temp2717;})) != 0){ fields=({ struct Cyc_List_List* _temp2718=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2718->hd=(
void*)({ struct _tuple15* _temp2719=( struct _tuple15*) GC_malloc( sizeof(
struct _tuple15)); _temp2719->f1=( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp2720= sd_fields; if( _temp2720 == 0){ _throw( Null_Exception);} _temp2720->hd;});
_temp2719->f2= 0; _temp2719;}); _temp2718->tl= fields; _temp2718;});}}} fields=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( fields);{
struct Cyc_List_List* ans= 0; for( 0; des != 0; des=({ struct Cyc_List_List*
_temp2722= des; if( _temp2722 == 0){ _throw( Null_Exception);} _temp2722->tl;})){
struct _tuple16 _temp2726; void* _temp2727; struct Cyc_List_List* _temp2729;
struct _tuple16* _temp2724=( struct _tuple16*)({ struct Cyc_List_List* _temp2723=
des; if( _temp2723 == 0){ _throw( Null_Exception);} _temp2723->hd;}); _temp2726=*
_temp2724; _LL2730: _temp2729= _temp2726.f1; goto _LL2728; _LL2728: _temp2727=
_temp2726.f2; goto _LL2725; _LL2725: if( _temp2729 == 0){ struct Cyc_List_List*
fields2= fields; for( 0; fields2 != 0; fields2=({ struct Cyc_List_List*
_temp2731= fields2; if( _temp2731 == 0){ _throw( Null_Exception);} _temp2731->tl;})){
if( !(*(( struct _tuple15*)({ struct Cyc_List_List* _temp2732= fields2; if(
_temp2732 == 0){ _throw( Null_Exception);} _temp2732->hd;}))).f2){(*(( struct
_tuple15*)({ struct Cyc_List_List* _temp2733= fields2; if( _temp2733 == 0){
_throw( Null_Exception);} _temp2733->hd;}))).f2= 1;(*(( struct _tuple16*)({
struct Cyc_List_List* _temp2734= des; if( _temp2734 == 0){ _throw(
Null_Exception);} _temp2734->hd;}))).f1=({ struct Cyc_List_List* _temp2735=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2735->hd=(
void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp2736=( struct Cyc_Absyn_FieldName_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct)); _temp2736[ 0]=({ struct
Cyc_Absyn_FieldName_struct _temp2737; _temp2737.tag= Cyc_Absyn_FieldName;
_temp2737.f1=((*(( struct _tuple15*)({ struct Cyc_List_List* _temp2738= fields2;
if( _temp2738 == 0){ _throw( Null_Exception);} _temp2738->hd;}))).f1)->name;
_temp2737;}); _temp2736;})); _temp2735->tl= 0; _temp2735;}); ans=({ struct Cyc_List_List*
_temp2739=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp2739->hd=( void*)({ struct _tuple17* _temp2740=( struct _tuple17*)
_region_malloc( rgn, sizeof( struct _tuple17)); _temp2740->f1=(*(( struct
_tuple15*)({ struct Cyc_List_List* _temp2741= fields2; if( _temp2741 == 0){
_throw( Null_Exception);} _temp2741->hd;}))).f1; _temp2740->f2= _temp2727;
_temp2740;}); _temp2739->tl= ans; _temp2739;}); break;}} if( fields2 == 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp2742=( char*)"too many arguments to struct";
struct _tagged_string _temp2743; _temp2743.curr= _temp2742; _temp2743.base=
_temp2742; _temp2743.last_plus_one= _temp2742 + 29; _temp2743;}));}} else{ if(({
struct Cyc_List_List* _temp2744= _temp2729; if( _temp2744 == 0){ _throw(
Null_Exception);} _temp2744->tl;}) != 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp2745=( char*)"multiple designators are not supported";
struct _tagged_string _temp2746; _temp2746.curr= _temp2745; _temp2746.base=
_temp2745; _temp2746.last_plus_one= _temp2745 + 39; _temp2746;}));} else{ void*
_temp2748=( void*)({ struct Cyc_List_List* _temp2747= _temp2729; if( _temp2747
== 0){ _throw( Null_Exception);} _temp2747->hd;}); struct Cyc_Absyn_Exp*
_temp2754; struct _tagged_string* _temp2756; _LL2750: if(*(( int*) _temp2748) ==
Cyc_Absyn_ArrayElement){ _LL2755: _temp2754=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayElement_struct*) _temp2748)->f1; goto _LL2751;} else{ goto
_LL2752;} _LL2752: if(*(( int*) _temp2748) == Cyc_Absyn_FieldName){ _LL2757:
_temp2756=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp2748)->f1; goto _LL2753;} else{ goto _LL2749;} _LL2751: Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp2758=( char*)"array designator used in argument to struct";
struct _tagged_string _temp2759; _temp2759.curr= _temp2758; _temp2759.base=
_temp2758; _temp2759.last_plus_one= _temp2758 + 44; _temp2759;})); goto _LL2749;
_LL2753: { struct Cyc_List_List* fields2= fields; for( 0; fields2 != 0; fields2=({
struct Cyc_List_List* _temp2760= fields2; if( _temp2760 == 0){ _throw(
Null_Exception);} _temp2760->tl;})){ if( Cyc_String_zstrptrcmp( _temp2756,((*((
struct _tuple15*)({ struct Cyc_List_List* _temp2761= fields2; if( _temp2761 == 0){
_throw( Null_Exception);} _temp2761->hd;}))).f1)->name) == 0){ if((*(( struct
_tuple15*)({ struct Cyc_List_List* _temp2762= fields2; if( _temp2762 == 0){
_throw( Null_Exception);} _temp2762->hd;}))).f2){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp2763=* _temp2756; xprintf("field %.*s has already been used as an argument",
_temp2763.last_plus_one - _temp2763.curr, _temp2763.curr);}));}(*(( struct
_tuple15*)({ struct Cyc_List_List* _temp2764= fields2; if( _temp2764 == 0){
_throw( Null_Exception);} _temp2764->hd;}))).f2= 1; ans=({ struct Cyc_List_List*
_temp2765=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp2765->hd=( void*)({ struct _tuple17* _temp2766=( struct _tuple17*)
_region_malloc( rgn, sizeof( struct _tuple17)); _temp2766->f1=(*(( struct
_tuple15*)({ struct Cyc_List_List* _temp2767= fields2; if( _temp2767 == 0){
_throw( Null_Exception);} _temp2767->hd;}))).f1; _temp2766->f2= _temp2727;
_temp2766;}); _temp2765->tl= ans; _temp2765;}); break;}} if( fields2 == 0){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp2768=* _temp2756; xprintf("bad field designator %.*s",
_temp2768.last_plus_one - _temp2768.curr, _temp2768.curr);}));} goto _LL2749;}
_LL2749:;}}} for( 0; fields != 0; fields=({ struct Cyc_List_List* _temp2769=
fields; if( _temp2769 == 0){ _throw( Null_Exception);} _temp2769->tl;})){ if( !(*((
struct _tuple15*)({ struct Cyc_List_List* _temp2770= fields; if( _temp2770 == 0){
_throw( Null_Exception);} _temp2770->hd;}))).f2){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp2771=( char*)"too few arguments to struct"; struct
_tagged_string _temp2772; _temp2772.curr= _temp2771; _temp2772.base= _temp2771;
_temp2772.last_plus_one= _temp2771 + 28; _temp2772;})); break;}} return(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( ans);}}} int Cyc_Tcutil_is_tagged_pointer_typ(
void* t){ void* _temp2773= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp2779; struct Cyc_Absyn_Conref* _temp2781; struct Cyc_Absyn_Tqual _temp2783;
struct Cyc_Absyn_Conref* _temp2785; void* _temp2787; void* _temp2789; _LL2775:
if(( unsigned int) _temp2773 > 4u?*(( int*) _temp2773) == Cyc_Absyn_PointerType:
0){ _LL2780: _temp2779=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp2773)->f1; _LL2790: _temp2789=( void*) _temp2779.elt_typ; goto _LL2788;
_LL2788: _temp2787=( void*) _temp2779.rgn_typ; goto _LL2786; _LL2786: _temp2785=(
struct Cyc_Absyn_Conref*) _temp2779.nullable; goto _LL2784; _LL2784: _temp2783=(
struct Cyc_Absyn_Tqual) _temp2779.tq; goto _LL2782; _LL2782: _temp2781=( struct
Cyc_Absyn_Conref*) _temp2779.bounds; goto _LL2776;} else{ goto _LL2777;} _LL2777:
goto _LL2778; _LL2776: { struct Cyc_Absyn_Conref* b=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp2781); void*
_temp2791=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)( b))->v; void* _temp2799; _LL2793: if(( unsigned int)
_temp2791 > 1u?*(( int*) _temp2791) == Cyc_Absyn_Eq_constr: 0){ _LL2800:
_temp2799=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2791)->f1; if(
_temp2799 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL2794;} else{ goto _LL2795;}}
else{ goto _LL2795;} _LL2795: if( _temp2791 ==( void*) Cyc_Absyn_No_constr){
goto _LL2796;} else{ goto _LL2797;} _LL2797: goto _LL2798; _LL2794: return 1;
_LL2796:( void*)( b->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp2801=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp2801[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp2802; _temp2802.tag= Cyc_Absyn_Eq_constr;
_temp2802.f1=( void*)(( void*) Cyc_Absyn_Unknown_b); _temp2802;}); _temp2801;})));
return 1; _LL2798: return 0; _LL2792:;} _LL2778: return 0; _LL2774:;} struct
_tuple6 Cyc_Tcutil_addressof_props( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e){ struct _tuple6 bogus_ans=({ struct _tuple6 _temp3025; _temp3025.f1= 0;
_temp3025.f2=( void*) Cyc_Absyn_HeapRgn; _temp3025;}); void* _temp2803=( void*)
e->r; void* _temp2817; struct _tuple1* _temp2819; struct _tagged_string*
_temp2821; struct Cyc_Absyn_Exp* _temp2823; struct _tagged_string* _temp2825;
struct Cyc_Absyn_Exp* _temp2827; struct Cyc_Absyn_Exp* _temp2829; struct Cyc_Absyn_Exp*
_temp2831; struct Cyc_Absyn_Exp* _temp2833; _LL2805: if(*(( int*) _temp2803) ==
Cyc_Absyn_Var_e){ _LL2820: _temp2819=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*)
_temp2803)->f1; goto _LL2818; _LL2818: _temp2817=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp2803)->f2; goto _LL2806;} else{ goto _LL2807;} _LL2807: if(*(( int*)
_temp2803) == Cyc_Absyn_StructMember_e){ _LL2824: _temp2823=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp2803)->f1; goto _LL2822; _LL2822:
_temp2821=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp2803)->f2; goto _LL2808;} else{ goto _LL2809;} _LL2809: if(*(( int*)
_temp2803) == Cyc_Absyn_StructArrow_e){ _LL2828: _temp2827=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp2803)->f1; goto _LL2826; _LL2826:
_temp2825=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp2803)->f2; goto _LL2810;} else{ goto _LL2811;} _LL2811: if(*(( int*)
_temp2803) == Cyc_Absyn_Deref_e){ _LL2830: _temp2829=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Deref_e_struct*) _temp2803)->f1; goto _LL2812;} else{ goto
_LL2813;} _LL2813: if(*(( int*) _temp2803) == Cyc_Absyn_Subscript_e){ _LL2834:
_temp2833=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp2803)->f1; goto _LL2832; _LL2832: _temp2831=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp2803)->f2; goto _LL2814;} else{ goto
_LL2815;} _LL2815: goto _LL2816; _LL2806: { void* _temp2835= _temp2817; struct
Cyc_Absyn_Fndecl* _temp2849; struct Cyc_Absyn_Vardecl* _temp2851; struct Cyc_Absyn_Vardecl*
_temp2853; struct Cyc_Absyn_Vardecl* _temp2855; struct Cyc_Absyn_Vardecl*
_temp2857; _LL2837: if( _temp2835 ==( void*) Cyc_Absyn_Unresolved_b){ goto
_LL2838;} else{ goto _LL2839;} _LL2839: if(( unsigned int) _temp2835 > 1u?*((
int*) _temp2835) == Cyc_Absyn_Funname_b: 0){ _LL2850: _temp2849=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Funname_b_struct*) _temp2835)->f1; goto _LL2840;} else{ goto
_LL2841;} _LL2841: if(( unsigned int) _temp2835 > 1u?*(( int*) _temp2835) == Cyc_Absyn_Global_b:
0){ _LL2852: _temp2851=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Global_b_struct*)
_temp2835)->f1; goto _LL2842;} else{ goto _LL2843;} _LL2843: if(( unsigned int)
_temp2835 > 1u?*(( int*) _temp2835) == Cyc_Absyn_Local_b: 0){ _LL2854: _temp2853=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Local_b_struct*) _temp2835)->f1;
goto _LL2844;} else{ goto _LL2845;} _LL2845: if(( unsigned int) _temp2835 > 1u?*((
int*) _temp2835) == Cyc_Absyn_Pat_b: 0){ _LL2856: _temp2855=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Pat_b_struct*) _temp2835)->f1; goto _LL2846;} else{ goto
_LL2847;} _LL2847: if(( unsigned int) _temp2835 > 1u?*(( int*) _temp2835) == Cyc_Absyn_Param_b:
0){ _LL2858: _temp2857=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Param_b_struct*)
_temp2835)->f1; goto _LL2848;} else{ goto _LL2836;} _LL2838: return bogus_ans;
_LL2840: return({ struct _tuple6 _temp2859; _temp2859.f1= 1; _temp2859.f2=( void*)
Cyc_Absyn_HeapRgn; _temp2859;}); _LL2842: { void* _temp2861= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp2860= e->topt; if( _temp2860 == 0){ _throw(
Null_Exception);} _temp2860->v;})); struct Cyc_Absyn_Exp* _temp2867; struct Cyc_Absyn_Tqual
_temp2869; void* _temp2871; _LL2863: if(( unsigned int) _temp2861 > 4u?*(( int*)
_temp2861) == Cyc_Absyn_ArrayType: 0){ _LL2872: _temp2871=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2861)->f1; goto _LL2870; _LL2870: _temp2869=( struct Cyc_Absyn_Tqual)((
struct Cyc_Absyn_ArrayType_struct*) _temp2861)->f2; goto _LL2868; _LL2868:
_temp2867=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2861)->f3; goto _LL2864;} else{ goto _LL2865;} _LL2865: goto _LL2866;
_LL2864: return({ struct _tuple6 _temp2873; _temp2873.f1= 1; _temp2873.f2=( void*)
Cyc_Absyn_HeapRgn; _temp2873;}); _LL2866: return({ struct _tuple6 _temp2874;
_temp2874.f1=( _temp2851->tq).q_const; _temp2874.f2=( void*) Cyc_Absyn_HeapRgn;
_temp2874;}); _LL2862:;} _LL2844: { void* _temp2876= Cyc_Tcutil_compress(( void*)({
struct Cyc_Core_Opt* _temp2875= e->topt; if( _temp2875 == 0){ _throw(
Null_Exception);} _temp2875->v;})); struct Cyc_Absyn_Exp* _temp2882; struct Cyc_Absyn_Tqual
_temp2884; void* _temp2886; _LL2878: if(( unsigned int) _temp2876 > 4u?*(( int*)
_temp2876) == Cyc_Absyn_ArrayType: 0){ _LL2887: _temp2886=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2876)->f1; goto _LL2885; _LL2885: _temp2884=( struct Cyc_Absyn_Tqual)((
struct Cyc_Absyn_ArrayType_struct*) _temp2876)->f2; goto _LL2883; _LL2883:
_temp2882=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2876)->f3; goto _LL2879;} else{ goto _LL2880;} _LL2880: goto _LL2881;
_LL2879: return({ struct _tuple6 _temp2888; _temp2888.f1= 1; _temp2888.f2=( void*)({
struct Cyc_Core_Opt* _temp2889= _temp2853->rgn; if( _temp2889 == 0){ _throw(
Null_Exception);} _temp2889->v;}); _temp2888;}); _LL2881: return({ struct
_tuple6 _temp2890; _temp2890.f1=( _temp2853->tq).q_const; _temp2890.f2=( void*)({
struct Cyc_Core_Opt* _temp2891= _temp2853->rgn; if( _temp2891 == 0){ _throw(
Null_Exception);} _temp2891->v;}); _temp2890;}); _LL2877:;} _LL2846: _temp2857=
_temp2855; goto _LL2848; _LL2848: return({ struct _tuple6 _temp2892; _temp2892.f1=(
_temp2857->tq).q_const; _temp2892.f2=( void*)({ struct Cyc_Core_Opt* _temp2893=
_temp2857->rgn; if( _temp2893 == 0){ _throw( Null_Exception);} _temp2893->v;});
_temp2892;}); _LL2836:;} _LL2808: { void* _temp2895= Cyc_Tcutil_compress(( void*)({
struct Cyc_Core_Opt* _temp2894= _temp2823->topt; if( _temp2894 == 0){ _throw(
Null_Exception);} _temp2894->v;})); struct Cyc_Absyn_Structdecl** _temp2903;
struct Cyc_Absyn_Structdecl* _temp2905; struct Cyc_List_List* _temp2906; struct
_tuple1* _temp2908; struct Cyc_Absyn_Uniondecl** _temp2910; struct Cyc_Absyn_Uniondecl*
_temp2912; struct Cyc_List_List* _temp2913; struct _tuple1* _temp2915; _LL2897:
if(( unsigned int) _temp2895 > 4u?*(( int*) _temp2895) == Cyc_Absyn_StructType:
0){ _LL2909: _temp2908=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp2895)->f1; goto _LL2907; _LL2907: _temp2906=( struct Cyc_List_List*)((
struct Cyc_Absyn_StructType_struct*) _temp2895)->f2; goto _LL2904; _LL2904:
_temp2903=( struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*)
_temp2895)->f3; if( _temp2903 == 0){ goto _LL2899;} else{ _temp2905=* _temp2903;
goto _LL2898;}} else{ goto _LL2899;} _LL2899: if(( unsigned int) _temp2895 > 4u?*((
int*) _temp2895) == Cyc_Absyn_UnionType: 0){ _LL2916: _temp2915=( struct _tuple1*)((
struct Cyc_Absyn_UnionType_struct*) _temp2895)->f1; goto _LL2914; _LL2914:
_temp2913=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp2895)->f2; goto _LL2911; _LL2911: _temp2910=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp2895)->f3; if( _temp2910 == 0){ goto
_LL2901;} else{ _temp2912=* _temp2910; goto _LL2900;}} else{ goto _LL2901;}
_LL2901: goto _LL2902; _LL2898: { struct Cyc_Absyn_Structfield* finfo= Cyc_Absyn_lookup_struct_field(
_temp2905, _temp2821); if( finfo != 0?({ struct Cyc_Absyn_Structfield* _temp2917=
finfo; if( _temp2917 == 0){ _throw( Null_Exception);} _temp2917->width;}) != 0:
0){ return({ struct _tuple6 _temp2918; _temp2918.f1=({ struct Cyc_Absyn_Structfield*
_temp2919= finfo; if( _temp2919 == 0){ _throw( Null_Exception);} _temp2919->tq;}).q_const;
_temp2918.f2=( Cyc_Tcutil_addressof_props( te, _temp2823)).f2; _temp2918;});}
return bogus_ans;} _LL2900: { struct Cyc_Absyn_Structfield* finfo= Cyc_Absyn_lookup_union_field(
_temp2912, _temp2821); if( finfo != 0){ return({ struct _tuple6 _temp2920;
_temp2920.f1=({ struct Cyc_Absyn_Structfield* _temp2921= finfo; if( _temp2921 ==
0){ _throw( Null_Exception);} _temp2921->tq;}).q_const; _temp2920.f2=( Cyc_Tcutil_addressof_props(
te, _temp2823)).f2; _temp2920;});} goto _LL2902;} _LL2902: return bogus_ans;
_LL2896:;} _LL2810: { void* _temp2923= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp2922= _temp2827->topt; if( _temp2922 == 0){ _throw( Null_Exception);}
_temp2922->v;})); struct Cyc_Absyn_PtrInfo _temp2929; struct Cyc_Absyn_Conref*
_temp2931; struct Cyc_Absyn_Tqual _temp2933; struct Cyc_Absyn_Conref* _temp2935;
void* _temp2937; void* _temp2939; _LL2925: if(( unsigned int) _temp2923 > 4u?*((
int*) _temp2923) == Cyc_Absyn_PointerType: 0){ _LL2930: _temp2929=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp2923)->f1; _LL2940: _temp2939=( void*)
_temp2929.elt_typ; goto _LL2938; _LL2938: _temp2937=( void*) _temp2929.rgn_typ;
goto _LL2936; _LL2936: _temp2935=( struct Cyc_Absyn_Conref*) _temp2929.nullable;
goto _LL2934; _LL2934: _temp2933=( struct Cyc_Absyn_Tqual) _temp2929.tq; goto
_LL2932; _LL2932: _temp2931=( struct Cyc_Absyn_Conref*) _temp2929.bounds; goto
_LL2926;} else{ goto _LL2927;} _LL2927: goto _LL2928; _LL2926: { void* _temp2941=
Cyc_Tcutil_compress( _temp2939); struct Cyc_Absyn_Structdecl** _temp2949; struct
Cyc_Absyn_Structdecl* _temp2951; struct Cyc_List_List* _temp2952; struct _tuple1*
_temp2954; struct Cyc_Absyn_Uniondecl** _temp2956; struct Cyc_Absyn_Uniondecl*
_temp2958; struct Cyc_List_List* _temp2959; struct _tuple1* _temp2961; _LL2943:
if(( unsigned int) _temp2941 > 4u?*(( int*) _temp2941) == Cyc_Absyn_StructType:
0){ _LL2955: _temp2954=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp2941)->f1; goto _LL2953; _LL2953: _temp2952=( struct Cyc_List_List*)((
struct Cyc_Absyn_StructType_struct*) _temp2941)->f2; goto _LL2950; _LL2950:
_temp2949=( struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*)
_temp2941)->f3; if( _temp2949 == 0){ goto _LL2945;} else{ _temp2951=* _temp2949;
goto _LL2944;}} else{ goto _LL2945;} _LL2945: if(( unsigned int) _temp2941 > 4u?*((
int*) _temp2941) == Cyc_Absyn_UnionType: 0){ _LL2962: _temp2961=( struct _tuple1*)((
struct Cyc_Absyn_UnionType_struct*) _temp2941)->f1; goto _LL2960; _LL2960:
_temp2959=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp2941)->f2; goto _LL2957; _LL2957: _temp2956=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp2941)->f3; if( _temp2956 == 0){ goto
_LL2947;} else{ _temp2958=* _temp2956; goto _LL2946;}} else{ goto _LL2947;}
_LL2947: goto _LL2948; _LL2944: { struct Cyc_Absyn_Structfield* finfo= Cyc_Absyn_lookup_struct_field(
_temp2951, _temp2825); if( finfo != 0?({ struct Cyc_Absyn_Structfield* _temp2963=
finfo; if( _temp2963 == 0){ _throw( Null_Exception);} _temp2963->width;}) != 0:
0){ return({ struct _tuple6 _temp2964; _temp2964.f1=({ struct Cyc_Absyn_Structfield*
_temp2965= finfo; if( _temp2965 == 0){ _throw( Null_Exception);} _temp2965->tq;}).q_const;
_temp2964.f2= _temp2937; _temp2964;});} return bogus_ans;} _LL2946: { struct Cyc_Absyn_Structfield*
finfo= Cyc_Absyn_lookup_union_field( _temp2958, _temp2825); if( finfo != 0){
return({ struct _tuple6 _temp2966; _temp2966.f1=({ struct Cyc_Absyn_Structfield*
_temp2967= finfo; if( _temp2967 == 0){ _throw( Null_Exception);} _temp2967->tq;}).q_const;
_temp2966.f2= _temp2937; _temp2966;});} return bogus_ans;} _LL2948: return
bogus_ans; _LL2942:;} _LL2928: return bogus_ans; _LL2924:;} _LL2812: { void*
_temp2969= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp2968=
_temp2829->topt; if( _temp2968 == 0){ _throw( Null_Exception);} _temp2968->v;}));
struct Cyc_Absyn_PtrInfo _temp2975; struct Cyc_Absyn_Conref* _temp2977; struct
Cyc_Absyn_Tqual _temp2979; struct Cyc_Absyn_Conref* _temp2981; void* _temp2983;
void* _temp2985; _LL2971: if(( unsigned int) _temp2969 > 4u?*(( int*) _temp2969)
== Cyc_Absyn_PointerType: 0){ _LL2976: _temp2975=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp2969)->f1; _LL2986: _temp2985=( void*)
_temp2975.elt_typ; goto _LL2984; _LL2984: _temp2983=( void*) _temp2975.rgn_typ;
goto _LL2982; _LL2982: _temp2981=( struct Cyc_Absyn_Conref*) _temp2975.nullable;
goto _LL2980; _LL2980: _temp2979=( struct Cyc_Absyn_Tqual) _temp2975.tq; goto
_LL2978; _LL2978: _temp2977=( struct Cyc_Absyn_Conref*) _temp2975.bounds; goto
_LL2972;} else{ goto _LL2973;} _LL2973: goto _LL2974; _LL2972: return({ struct
_tuple6 _temp2987; _temp2987.f1= _temp2979.q_const; _temp2987.f2= _temp2983;
_temp2987;}); _LL2974: return bogus_ans; _LL2970:;} _LL2814: { void* t= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp3022= _temp2833->topt; if( _temp3022 == 0){
_throw( Null_Exception);} _temp3022->v;})); void* _temp2988= t; struct Cyc_Absyn_Exp*
_temp2998; struct Cyc_Absyn_Tqual _temp3000; void* _temp3002; struct Cyc_List_List*
_temp3004; struct Cyc_Absyn_PtrInfo _temp3006; struct Cyc_Absyn_Conref*
_temp3008; struct Cyc_Absyn_Tqual _temp3010; struct Cyc_Absyn_Conref* _temp3012;
void* _temp3014; void* _temp3016; _LL2990: if(( unsigned int) _temp2988 > 4u?*((
int*) _temp2988) == Cyc_Absyn_ArrayType: 0){ _LL3003: _temp3002=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2988)->f1; goto _LL3001; _LL3001:
_temp3000=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2988)->f2; goto _LL2999; _LL2999: _temp2998=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2988)->f3; goto _LL2991;} else{ goto
_LL2992;} _LL2992: if(( unsigned int) _temp2988 > 4u?*(( int*) _temp2988) == Cyc_Absyn_TupleType:
0){ _LL3005: _temp3004=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp2988)->f1; goto _LL2993;} else{ goto _LL2994;} _LL2994: if(( unsigned int)
_temp2988 > 4u?*(( int*) _temp2988) == Cyc_Absyn_PointerType: 0){ _LL3007:
_temp3006=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp2988)->f1; _LL3017: _temp3016=( void*) _temp3006.elt_typ; goto _LL3015;
_LL3015: _temp3014=( void*) _temp3006.rgn_typ; goto _LL3013; _LL3013: _temp3012=(
struct Cyc_Absyn_Conref*) _temp3006.nullable; goto _LL3011; _LL3011: _temp3010=(
struct Cyc_Absyn_Tqual) _temp3006.tq; goto _LL3009; _LL3009: _temp3008=( struct
Cyc_Absyn_Conref*) _temp3006.bounds; goto _LL2995;} else{ goto _LL2996;} _LL2996:
goto _LL2997; _LL2991: return({ struct _tuple6 _temp3018; _temp3018.f1=
_temp3000.q_const; _temp3018.f2=( Cyc_Tcutil_addressof_props( te, _temp2833)).f2;
_temp3018;}); _LL2993: { struct _tuple4* finfo= Cyc_Absyn_lookup_tuple_field(
_temp3004,( int) Cyc_Evexp_eval_const_uint_exp( _temp2831)); if( finfo != 0){
return({ struct _tuple6 _temp3019; _temp3019.f1=(({ struct _tuple4* _temp3020=
finfo; if( _temp3020 == 0){ _throw( Null_Exception);}* _temp3020;}).f1).q_const;
_temp3019.f2=( Cyc_Tcutil_addressof_props( te, _temp2833)).f2; _temp3019;});}
return bogus_ans;} _LL2995: return({ struct _tuple6 _temp3021; _temp3021.f1=
_temp3010.q_const; _temp3021.f2= _temp3014; _temp3021;}); _LL2997: return
bogus_ans; _LL2989:;} _LL2816: Cyc_Tcutil_terr( e->loc,( struct _tagged_string)({
char* _temp3023=( char*)"unary & applied to non-lvalue"; struct _tagged_string
_temp3024; _temp3024.curr= _temp3023; _temp3024.base= _temp3023; _temp3024.last_plus_one=
_temp3023 + 30; _temp3024;})); return bogus_ans; _LL2804:;} void* Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv* te, void* e_typ, struct Cyc_Absyn_Exp* e){ void*
_temp3026= Cyc_Tcutil_compress( e_typ); struct Cyc_Absyn_Exp* _temp3032; struct
Cyc_Absyn_Tqual _temp3034; void* _temp3036; _LL3028: if(( unsigned int)
_temp3026 > 4u?*(( int*) _temp3026) == Cyc_Absyn_ArrayType: 0){ _LL3037:
_temp3036=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3026)->f1; goto
_LL3035; _LL3035: _temp3034=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp3026)->f2; goto _LL3033; _LL3033: _temp3032=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3026)->f3; goto _LL3029;} else{ goto
_LL3030;} _LL3030: goto _LL3031; _LL3029: { void* _temp3040; int _temp3042;
struct _tuple6 _temp3038= Cyc_Tcutil_addressof_props( te, e); _LL3043: _temp3042=
_temp3038.f1; goto _LL3041; _LL3041: _temp3040= _temp3038.f2; goto _LL3039;
_LL3039: return Cyc_Absyn_atb_typ( _temp3036, _temp3040, _temp3034,( void*)({
struct Cyc_Absyn_Upper_b_struct* _temp3044=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp3044[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp3045; _temp3045.tag= Cyc_Absyn_Upper_b; _temp3045.f1=
e; _temp3045;}); _temp3044;}));} _LL3031: return e_typ; _LL3027:;} void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment* loc, unsigned int i, struct Cyc_Absyn_Conref* b){ b=((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
b);{ void* _temp3046=( void*) b->v; void* _temp3054; void* _temp3056; struct Cyc_Absyn_Exp*
_temp3058; _LL3048: if(( unsigned int) _temp3046 > 1u?*(( int*) _temp3046) ==
Cyc_Absyn_Eq_constr: 0){ _LL3055: _temp3054=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3046)->f1; if( _temp3054 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL3049;}
else{ goto _LL3050;}} else{ goto _LL3050;} _LL3050: if(( unsigned int) _temp3046
> 1u?*(( int*) _temp3046) == Cyc_Absyn_Eq_constr: 0){ _LL3057: _temp3056=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp3046)->f1; if(( unsigned int) _temp3056
> 1u?*(( int*) _temp3056) == Cyc_Absyn_Upper_b: 0){ _LL3059: _temp3058=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp3056)->f1; goto _LL3051;}
else{ goto _LL3052;}} else{ goto _LL3052;} _LL3052: goto _LL3053; _LL3049:
return; _LL3051: if( Cyc_Evexp_eval_const_uint_exp( _temp3058) <= i){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp3060=( char*)"dereference is out of bounds";
struct _tagged_string _temp3061; _temp3061.curr= _temp3060; _temp3061.base=
_temp3060; _temp3061.last_plus_one= _temp3060 + 29; _temp3061;}));} return;
_LL3053:( void*)( b->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp3062=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp3062[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp3063; _temp3063.tag= Cyc_Absyn_Eq_constr;
_temp3063.f1=( void*)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp3064=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp3064[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp3065; _temp3065.tag= Cyc_Absyn_Upper_b;
_temp3065.f1= Cyc_Absyn_uint_exp( i + 1, 0); _temp3065;}); _temp3064;}));
_temp3063;}); _temp3062;}))); return; _LL3047:;}} void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Conref* b){ Cyc_Tcutil_check_bound(
loc, 0, b);} int Cyc_Tcutil_bits_only( void* t){ void* _temp3066= Cyc_Tcutil_compress(
t); void* _temp3088; void* _temp3090; struct Cyc_Absyn_Enumdecl* _temp3092;
struct _tuple1* _temp3094; struct Cyc_Absyn_Exp* _temp3096; struct Cyc_Absyn_Tqual
_temp3098; void* _temp3100; struct Cyc_List_List* _temp3102; struct Cyc_Absyn_Structdecl**
_temp3104; struct Cyc_List_List* _temp3106; struct _tuple1* _temp3108; struct
Cyc_Absyn_Uniondecl** _temp3110; struct Cyc_List_List* _temp3112; struct _tuple1*
_temp3114; _LL3068: if( _temp3066 ==( void*) Cyc_Absyn_VoidType){ goto _LL3069;}
else{ goto _LL3070;} _LL3070: if(( unsigned int) _temp3066 > 4u?*(( int*)
_temp3066) == Cyc_Absyn_IntType: 0){ _LL3091: _temp3090=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp3066)->f1; goto _LL3089; _LL3089: _temp3088=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp3066)->f2; goto _LL3071;} else{ goto _LL3072;} _LL3072: if( _temp3066 ==(
void*) Cyc_Absyn_FloatType){ goto _LL3073;} else{ goto _LL3074;} _LL3074: if(
_temp3066 ==( void*) Cyc_Absyn_DoubleType){ goto _LL3075;} else{ goto _LL3076;}
_LL3076: if(( unsigned int) _temp3066 > 4u?*(( int*) _temp3066) == Cyc_Absyn_EnumType:
0){ _LL3095: _temp3094=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*)
_temp3066)->f1; goto _LL3093; _LL3093: _temp3092=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_EnumType_struct*) _temp3066)->f2; goto _LL3077;} else{ goto
_LL3078;} _LL3078: if(( unsigned int) _temp3066 > 4u?*(( int*) _temp3066) == Cyc_Absyn_ArrayType:
0){ _LL3101: _temp3100=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3066)->f1;
goto _LL3099; _LL3099: _temp3098=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp3066)->f2; goto _LL3097; _LL3097: _temp3096=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3066)->f3; goto _LL3079;} else{ goto
_LL3080;} _LL3080: if(( unsigned int) _temp3066 > 4u?*(( int*) _temp3066) == Cyc_Absyn_TupleType:
0){ _LL3103: _temp3102=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp3066)->f1; goto _LL3081;} else{ goto _LL3082;} _LL3082: if(( unsigned int)
_temp3066 > 4u?*(( int*) _temp3066) == Cyc_Absyn_StructType: 0){ _LL3109:
_temp3108=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp3066)->f1;
goto _LL3107; _LL3107: _temp3106=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp3066)->f2; goto _LL3105; _LL3105: _temp3104=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp3066)->f3; goto _LL3083;} else{ goto
_LL3084;} _LL3084: if(( unsigned int) _temp3066 > 4u?*(( int*) _temp3066) == Cyc_Absyn_UnionType:
0){ _LL3115: _temp3114=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp3066)->f1; goto _LL3113; _LL3113: _temp3112=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnionType_struct*) _temp3066)->f2; goto _LL3111; _LL3111:
_temp3110=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp3066)->f3; goto _LL3085;} else{ goto _LL3086;} _LL3086: goto _LL3087;
_LL3069: return 1; _LL3071: return 1; _LL3073: return 1; _LL3075: return 1;
_LL3077: return 1; _LL3079: return Cyc_Tcutil_bits_only( _temp3100); _LL3081:
for( 0; _temp3102 != 0; _temp3102=({ struct Cyc_List_List* _temp3116= _temp3102;
if( _temp3116 == 0){ _throw( Null_Exception);} _temp3116->tl;})){ if( ! Cyc_Tcutil_bits_only((*((
struct _tuple4*)({ struct Cyc_List_List* _temp3117= _temp3102; if( _temp3117 ==
0){ _throw( Null_Exception);} _temp3117->hd;}))).f2)){ return 0;}} return 1;
_LL3083: if( _temp3104 == 0){ return 0;}{ struct Cyc_Absyn_Structdecl* sd=({
struct Cyc_Absyn_Structdecl** _temp3126= _temp3104; if( _temp3126 == 0){ _throw(
Null_Exception);}* _temp3126;}); if( sd->fields == 0){ return 0;}{ struct
_RegionHandle _temp3118= _new_region(); struct _RegionHandle* rgn=& _temp3118;
struct _handler_cons _temp3119; _push_handler(& _temp3119);{ void* _temp3120=(
void*) setjmp( _temp3119.handler); if( ! _temp3120){{ struct Cyc_List_List* inst=((
struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct _RegionHandle* r4,
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, sd->tvs,
_temp3106);{ struct Cyc_List_List* fs=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp3124= sd->fields; if( _temp3124 == 0){ _throw( Null_Exception);} _temp3124->v;});
for( 0; fs != 0; fs=({ struct Cyc_List_List* _temp3121= fs; if( _temp3121 == 0){
_throw( Null_Exception);} _temp3121->tl;})){ if( ! Cyc_Tcutil_bits_only( Cyc_Tcutil_rsubstitute(
rgn, inst,( void*)(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp3122= fs; if( _temp3122 == 0){ _throw( Null_Exception);} _temp3122->hd;}))->type))){
int _temp3123= 0; _npop_handler( 0u); return _temp3123;}}}{ int _temp3125= 1;
_npop_handler( 0u); return _temp3125;}} _free_region(& _temp3118); _pop_handler();}
else{ _free_region(& _temp3118); _throw( _temp3120);}}}} _LL3085: if( _temp3110
== 0){ return 0;}{ struct Cyc_Absyn_Uniondecl* ud=({ struct Cyc_Absyn_Uniondecl**
_temp3135= _temp3110; if( _temp3135 == 0){ _throw( Null_Exception);}* _temp3135;});
if( ud->fields == 0){ return 0;}{ struct _RegionHandle _temp3127= _new_region();
struct _RegionHandle* rgn=& _temp3127; struct _handler_cons _temp3128;
_push_handler(& _temp3128);{ void* _temp3129=( void*) setjmp( _temp3128.handler);
if( ! _temp3129){{ struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, ud->tvs, _temp3112);{ struct
Cyc_List_List* fs=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp3133= ud->fields;
if( _temp3133 == 0){ _throw( Null_Exception);} _temp3133->v;}); for( 0; fs != 0;
fs=({ struct Cyc_List_List* _temp3130= fs; if( _temp3130 == 0){ _throw(
Null_Exception);} _temp3130->tl;})){ if( ! Cyc_Tcutil_bits_only( Cyc_Tcutil_rsubstitute(
rgn, inst,( void*)(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp3131= fs; if( _temp3131 == 0){ _throw( Null_Exception);} _temp3131->hd;}))->type))){
int _temp3132= 0; _npop_handler( 0u); return _temp3132;}}}{ int _temp3134= 1;
_npop_handler( 0u); return _temp3134;}} _free_region(& _temp3127); _pop_handler();}
else{ _free_region(& _temp3127); _throw( _temp3129);}}}} _LL3087: return 0;
_LL3067:;} struct _tuple18{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2;
} ; static int Cyc_Tcutil_cnst_exp( struct Cyc_Tcenv_Tenv* te, int var_okay,
struct Cyc_Absyn_Exp* e){ void* _temp3136=( void*) e->r; void* _temp3172; void*
_temp3174; struct Cyc_Absyn_Exp* _temp3176; struct Cyc_Absyn_Enumfield*
_temp3178; struct Cyc_Absyn_Enumdecl* _temp3180; struct _tuple1* _temp3182; void*
_temp3184; struct _tuple1* _temp3186; struct Cyc_Absyn_Exp* _temp3188; struct
Cyc_Absyn_Exp* _temp3190; struct Cyc_Absyn_Exp* _temp3192; struct Cyc_Absyn_Exp*
_temp3194; struct Cyc_Absyn_Exp* _temp3196; struct Cyc_List_List* _temp3198;
struct Cyc_Absyn_Exp* _temp3200; struct Cyc_Absyn_Exp* _temp3202; void*
_temp3204; struct Cyc_Absyn_Exp* _temp3206; struct Cyc_Absyn_Exp* _temp3208;
struct Cyc_Absyn_Exp* _temp3210; struct Cyc_Absyn_Vardecl* _temp3212; struct Cyc_List_List*
_temp3214; struct Cyc_Absyn_Structdecl* _temp3216; struct Cyc_List_List*
_temp3218; struct Cyc_Core_Opt* _temp3220; struct _tuple1* _temp3222; struct Cyc_List_List*
_temp3224; void* _temp3226; struct Cyc_List_List* _temp3228; struct Cyc_Absyn_Tunionfield*
_temp3230; struct Cyc_Absyn_Tuniondecl* _temp3232; struct Cyc_List_List*
_temp3234; struct Cyc_Core_Opt* _temp3236; struct Cyc_Core_Opt* _temp3238;
_LL3138: if(*(( int*) _temp3136) == Cyc_Absyn_Const_e){ _LL3173: _temp3172=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp3136)->f1; goto _LL3139;} else{
goto _LL3140;} _LL3140: if(*(( int*) _temp3136) == Cyc_Absyn_Sizeoftyp_e){
_LL3175: _temp3174=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp3136)->f1;
goto _LL3141;} else{ goto _LL3142;} _LL3142: if(*(( int*) _temp3136) == Cyc_Absyn_Sizeofexp_e){
_LL3177: _temp3176=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp3136)->f1; goto _LL3143;} else{ goto _LL3144;} _LL3144: if(*(( int*)
_temp3136) == Cyc_Absyn_Enum_e){ _LL3183: _temp3182=( struct _tuple1*)(( struct
Cyc_Absyn_Enum_e_struct*) _temp3136)->f1; goto _LL3181; _LL3181: _temp3180=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*) _temp3136)->f2;
goto _LL3179; _LL3179: _temp3178=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp3136)->f3; goto _LL3145;} else{ goto _LL3146;} _LL3146: if(*(( int*)
_temp3136) == Cyc_Absyn_Var_e){ _LL3187: _temp3186=( struct _tuple1*)(( struct
Cyc_Absyn_Var_e_struct*) _temp3136)->f1; goto _LL3185; _LL3185: _temp3184=( void*)((
struct Cyc_Absyn_Var_e_struct*) _temp3136)->f2; goto _LL3147;} else{ goto
_LL3148;} _LL3148: if(*(( int*) _temp3136) == Cyc_Absyn_Conditional_e){ _LL3193:
_temp3192=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp3136)->f1; goto _LL3191; _LL3191: _temp3190=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp3136)->f2; goto _LL3189; _LL3189:
_temp3188=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp3136)->f3; goto _LL3149;} else{ goto _LL3150;} _LL3150: if(*(( int*)
_temp3136) == Cyc_Absyn_SeqExp_e){ _LL3197: _temp3196=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_SeqExp_e_struct*) _temp3136)->f1; goto _LL3195; _LL3195:
_temp3194=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp3136)->f2; goto _LL3151;} else{ goto _LL3152;} _LL3152: if(*(( int*)
_temp3136) == Cyc_Absyn_Instantiate_e){ _LL3201: _temp3200=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Instantiate_e_struct*) _temp3136)->f1; goto _LL3199; _LL3199:
_temp3198=( struct Cyc_List_List*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp3136)->f2; goto _LL3153;} else{ goto _LL3154;} _LL3154: if(*(( int*)
_temp3136) == Cyc_Absyn_Cast_e){ _LL3205: _temp3204=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp3136)->f1; goto _LL3203; _LL3203: _temp3202=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Cast_e_struct*) _temp3136)->f2; goto _LL3155;} else{ goto
_LL3156;} _LL3156: if(*(( int*) _temp3136) == Cyc_Absyn_Address_e){ _LL3207:
_temp3206=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*)
_temp3136)->f1; goto _LL3157;} else{ goto _LL3158;} _LL3158: if(*(( int*)
_temp3136) == Cyc_Absyn_Comprehension_e){ _LL3213: _temp3212=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp3136)->f1; goto _LL3211; _LL3211:
_temp3210=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp3136)->f2; goto _LL3209; _LL3209: _temp3208=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp3136)->f3; goto _LL3159;} else{
goto _LL3160;} _LL3160: if(*(( int*) _temp3136) == Cyc_Absyn_Array_e){ _LL3215:
_temp3214=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp3136)->f1;
goto _LL3161;} else{ goto _LL3162;} _LL3162: if(*(( int*) _temp3136) == Cyc_Absyn_Struct_e){
_LL3223: _temp3222=( struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp3136)->f1; goto _LL3221; _LL3221: _temp3220=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Struct_e_struct*) _temp3136)->f2; goto _LL3219; _LL3219:
_temp3218=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp3136)->f3; goto _LL3217; _LL3217: _temp3216=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp3136)->f4; goto _LL3163;} else{ goto
_LL3164;} _LL3164: if(*(( int*) _temp3136) == Cyc_Absyn_Primop_e){ _LL3227:
_temp3226=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp3136)->f1; goto
_LL3225; _LL3225: _temp3224=( struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp3136)->f2; goto _LL3165;} else{ goto _LL3166;} _LL3166: if(*(( int*)
_temp3136) == Cyc_Absyn_Tuple_e){ _LL3229: _temp3228=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tuple_e_struct*) _temp3136)->f1; goto _LL3167;} else{ goto
_LL3168;} _LL3168: if(*(( int*) _temp3136) == Cyc_Absyn_Tunion_e){ _LL3239:
_temp3238=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp3136)->f1;
goto _LL3237; _LL3237: _temp3236=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3136)->f2; goto _LL3235; _LL3235: _temp3234=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp3136)->f3; goto _LL3233; _LL3233:
_temp3232=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp3136)->f4; goto _LL3231; _LL3231: _temp3230=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp3136)->f5; goto _LL3169;} else{ goto
_LL3170;} _LL3170: goto _LL3171; _LL3139: return 1; _LL3141: return 1; _LL3143:
return Cyc_Tcutil_cnst_exp( te, 1, _temp3176); _LL3145: return 1; _LL3147: {
struct _handler_cons _temp3240; _push_handler(& _temp3240);{ void* _temp3241=(
void*) setjmp( _temp3240.handler); if( ! _temp3241){{ void* _temp3242= Cyc_Tcenv_lookup_ordinary(
te, e->loc, _temp3186); void* _temp3250; struct Cyc_Absyn_Fndecl* _temp3252;
void* _temp3254; struct Cyc_Absyn_Vardecl* _temp3256; _LL3244: if(*(( int*)
_temp3242) == Cyc_Tcenv_VarRes){ _LL3251: _temp3250=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp3242)->f1; if(( unsigned int) _temp3250 > 1u?*(( int*) _temp3250) == Cyc_Absyn_Funname_b:
0){ _LL3253: _temp3252=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Funname_b_struct*)
_temp3250)->f1; goto _LL3245;} else{ goto _LL3246;}} else{ goto _LL3246;}
_LL3246: if(*(( int*) _temp3242) == Cyc_Tcenv_VarRes){ _LL3255: _temp3254=( void*)((
struct Cyc_Tcenv_VarRes_struct*) _temp3242)->f1; if(( unsigned int) _temp3254 >
1u?*(( int*) _temp3254) == Cyc_Absyn_Global_b: 0){ _LL3257: _temp3256=( struct
Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Global_b_struct*) _temp3254)->f1; goto
_LL3247;} else{ goto _LL3248;}} else{ goto _LL3248;} _LL3248: goto _LL3249;
_LL3245: { int _temp3258= 1; _npop_handler( 0u); return _temp3258;} _LL3247: {
void* _temp3259= Cyc_Tcutil_compress(( void*) _temp3256->type); struct Cyc_Absyn_Exp*
_temp3265; struct Cyc_Absyn_Tqual _temp3267; void* _temp3269; _LL3261: if((
unsigned int) _temp3259 > 4u?*(( int*) _temp3259) == Cyc_Absyn_ArrayType: 0){
_LL3270: _temp3269=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp3259)->f1;
goto _LL3268; _LL3268: _temp3267=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp3259)->f2; goto _LL3266; _LL3266: _temp3265=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp3259)->f3; goto _LL3262;} else{ goto
_LL3263;} _LL3263: goto _LL3264; _LL3262: { int _temp3271= 1; _npop_handler( 0u);
return _temp3271;} _LL3264: { int _temp3272= var_okay; _npop_handler( 0u);
return _temp3272;} _LL3260:;} _LL3249: { int _temp3273= var_okay; _npop_handler(
0u); return _temp3273;} _LL3243:;}; _pop_handler();} else{ void* _temp3275=
_temp3241; _LL3277: if( _temp3275 == Cyc_Dict_Absent){ goto _LL3278;} else{ goto
_LL3279;} _LL3279: goto _LL3280; _LL3278: return 0; _LL3280:( void) _throw(
_temp3275); _LL3276:;}}} _LL3149: return( Cyc_Tcutil_cnst_exp( te, 0, _temp3192)?
Cyc_Tcutil_cnst_exp( te, 0, _temp3190): 0)? Cyc_Tcutil_cnst_exp( te, 0,
_temp3188): 0; _LL3151: return Cyc_Tcutil_cnst_exp( te, 0, _temp3196)? Cyc_Tcutil_cnst_exp(
te, 0, _temp3194): 0; _LL3153: return Cyc_Tcutil_cnst_exp( te, var_okay,
_temp3200); _LL3155: return Cyc_Tcutil_cnst_exp( te, var_okay, _temp3202);
_LL3157: return Cyc_Tcutil_cnst_exp( te, 1, _temp3206); _LL3159: return Cyc_Tcutil_cnst_exp(
te, 0, _temp3210)? Cyc_Tcutil_cnst_exp( te, 0, _temp3208): 0; _LL3161: _temp3218=
_temp3214; goto _LL3163; _LL3163: for( 0; _temp3218 != 0; _temp3218=({ struct
Cyc_List_List* _temp3281= _temp3218; if( _temp3281 == 0){ _throw( Null_Exception);}
_temp3281->tl;})){ if( ! Cyc_Tcutil_cnst_exp( te, 0,(*(( struct _tuple18*)({
struct Cyc_List_List* _temp3282= _temp3218; if( _temp3282 == 0){ _throw(
Null_Exception);} _temp3282->hd;}))).f2)){ return 0;}} return 1; _LL3165:
_temp3228= _temp3224; goto _LL3167; _LL3167: _temp3234= _temp3228; goto _LL3169;
_LL3169: for( 0; _temp3234 != 0; _temp3234=({ struct Cyc_List_List* _temp3283=
_temp3234; if( _temp3283 == 0){ _throw( Null_Exception);} _temp3283->tl;})){ if(
! Cyc_Tcutil_cnst_exp( te, 0,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp3284= _temp3234; if( _temp3284 == 0){ _throw( Null_Exception);} _temp3284->hd;}))){
return 0;}} return 1; _LL3171: return 0; _LL3137:;} int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ return Cyc_Tcutil_cnst_exp(
te, 0, e);}