 typedef int Cyc_ptrdiff_t; typedef unsigned int Cyc_size_t; typedef
unsigned short Cyc_wchar_t; typedef unsigned int Cyc_wint_t; typedef
unsigned char Cyc_u_char; typedef unsigned short Cyc_u_short; typedef
unsigned int Cyc_u_int; typedef unsigned int Cyc_u_long; typedef unsigned short
Cyc_ushort; typedef unsigned int Cyc_uint; typedef unsigned int Cyc_clock_t;
typedef int Cyc_time_t; struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct
Cyc_itimerspec{ struct Cyc_timespec it_interval; struct Cyc_timespec it_value; }
; typedef int Cyc_daddr_t; typedef unsigned char* Cyc_caddr_t; typedef
unsigned int Cyc_ino_t; typedef unsigned int Cyc_vm_offset_t; typedef
unsigned int Cyc_vm_size_t; typedef unsigned char Cyc_int8_t; typedef
unsigned char Cyc_u_int8_t; typedef short Cyc_int16_t; typedef unsigned short
Cyc_u_int16_t; typedef int Cyc_int32_t; typedef unsigned int Cyc_u_int32_t;
typedef long long Cyc_int64_t; typedef unsigned long long Cyc_u_int64_t; typedef
int Cyc_register_t; typedef short Cyc_dev_t; typedef int Cyc_off_t; typedef
unsigned short Cyc_uid_t; typedef unsigned short Cyc_gid_t; typedef int Cyc_pid_t;
typedef int Cyc_key_t; typedef int Cyc_ssize_t; typedef unsigned char* Cyc_addr_t;
typedef int Cyc_mode_t; typedef unsigned short Cyc_nlink_t; typedef int Cyc_fd_mask;
struct Cyc__types_fd_set{ int fds_bits[ 8u]; } ; typedef struct Cyc__types_fd_set
Cyc__types_fd_set; typedef unsigned char* Cyc_Cstring; typedef struct
_tagged_string Cyc_string; typedef struct _tagged_string Cyc_string_t; typedef
struct _tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void exit(
int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; typedef struct Cyc_Core_Opt*
Cyc_Core_opt_t; extern struct _tagged_string Cyc_Core_new_string( int); extern
unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_string
f1; } ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern struct _tagged_string
Cyc_Core_string_of_int( int); extern unsigned char* string_to_Cstring( struct
_tagged_string); extern unsigned char* underlying_Cstring( struct _tagged_string);
extern struct _tagged_string Cstring_to_string( unsigned char*); extern int
system( unsigned char*); struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE
Cyc_Stdio_FILE; typedef int Cyc_Stdio_fpos_t; extern unsigned char Cyc_Stdio_FileCloseError[
19u]; extern unsigned char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
unsigned char* tag; struct _tagged_string f1; } ; struct Cyc_List_List{ void* hd;
struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List* Cyc_List_glist_t;
typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t;
extern unsigned char Cyc_List_List_empty[ 15u]; extern struct Cyc_List_List* Cyc_List_map(
void*(* f)( void*), struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_map_c(
void*(* f)( void*, void*), void* env, struct Cyc_List_List* x); extern
unsigned char Cyc_List_List_mismatch[ 18u]; extern void Cyc_List_iter( void(* f)(
void*), struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_imp_rev(
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_append( struct
Cyc_List_List* x, struct Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_imp_append(
struct Cyc_List_List* x, struct Cyc_List_List* y); extern unsigned char Cyc_List_Nth[
8u]; extern int Cyc_List_exists( int(* pred)( void*), struct Cyc_List_List* x);
extern int Cyc_List_list_cmp( int(* cmp)( void*, void*), struct Cyc_List_List*
l1, struct Cyc_List_List* l2); extern int Cyc_List_list_prefix( int(* cmp)( void*,
void*), struct Cyc_List_List* l1, struct Cyc_List_List* l2); struct Cyc_PP_Ppstate;
typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t; struct Cyc_PP_Out; typedef
struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc; typedef struct Cyc_PP_Doc*
Cyc_PP_doc_t; extern void Cyc_PP_file_of_doc( struct Cyc_PP_Doc* d, int w,
struct Cyc_Stdio___sFILE* f); extern struct _tagged_string Cyc_PP_string_of_doc(
struct Cyc_PP_Doc* d, int w); extern struct Cyc_PP_Doc* Cyc_PP_nil_doc(); extern
struct Cyc_PP_Doc* Cyc_PP_line_doc(); extern struct Cyc_PP_Doc* Cyc_PP_text(
struct _tagged_string s); extern struct Cyc_PP_Doc* Cyc_PP_textptr( struct
_tagged_string* s); extern struct Cyc_PP_Doc* Cyc_PP_nest( int k, struct Cyc_PP_Doc*
d); struct _tagged_ptr0{ struct Cyc_PP_Doc** curr; struct Cyc_PP_Doc** base;
struct Cyc_PP_Doc** last_plus_one; } ; extern struct Cyc_PP_Doc* Cyc_PP_cat(
struct _tagged_ptr0); extern struct Cyc_PP_Doc* Cyc_PP_ppseq( struct Cyc_PP_Doc*(*
pp)( void*), struct _tagged_string sep, struct Cyc_List_List* l0); extern struct
Cyc_PP_Doc* Cyc_PP_ppseql( struct Cyc_PP_Doc*(* pp)( void*), struct
_tagged_string sep, struct Cyc_List_List* l0); extern struct Cyc_PP_Doc* Cyc_PP_seql(
struct _tagged_string sep, struct Cyc_List_List* l0); extern struct Cyc_PP_Doc*
Cyc_PP_group( struct _tagged_string start, struct _tagged_string stop, struct
_tagged_string sep, struct Cyc_List_List* ss); extern struct Cyc_PP_Doc* Cyc_PP_egroup(
struct _tagged_string start, struct _tagged_string stop, struct _tagged_string
sep, struct Cyc_List_List* ss); struct Cyc_Lineno_Pos{ struct _tagged_string
logical_file; struct _tagged_string line; int line_no; int col; } ; typedef
struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t; extern unsigned char Cyc_Position_Exit[
9u]; struct Cyc_Position_Segment; typedef struct Cyc_Position_Segment* Cyc_Position_seg_t;
static const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1;
static const int Cyc_Position_Elab= 2; typedef void* Cyc_Position_error_kind_t;
struct Cyc_Position_Error{ struct _tagged_string source; struct Cyc_Position_Segment*
seg; void* kind; struct _tagged_string desc; } ; typedef struct Cyc_Position_Error*
Cyc_Position_error_t; extern unsigned char Cyc_Position_Nocontext[ 14u]; typedef
struct _tagged_string* Cyc_Absyn_field_name_t; typedef struct _tagged_string*
Cyc_Absyn_var_t; typedef struct _tagged_string* Cyc_Absyn_tvarname_t; typedef
void* Cyc_Absyn_nmspace_t; struct _tuple0{ void* f1; struct _tagged_string* f2;
} ; typedef struct _tuple0* Cyc_Absyn_qvar_t; typedef struct _tuple0* Cyc_Absyn_qvar_opt_t;
typedef struct _tuple0* Cyc_Absyn_typedef_name_t; typedef struct _tuple0* Cyc_Absyn_typedef_name_opt_t;
struct Cyc_Absyn_Tvar; struct Cyc_Absyn_Tqual; struct Cyc_Absyn_Conref; struct
Cyc_Absyn_PtrInfo; struct Cyc_Absyn_VarargInfo; struct Cyc_Absyn_FnInfo; struct
Cyc_Absyn_TunionInfo; struct Cyc_Absyn_TunionFieldInfo; struct Cyc_Absyn_VarargCallInfo;
struct Cyc_Absyn_Exp; struct Cyc_Absyn_Stmt; struct Cyc_Absyn_Pat; struct Cyc_Absyn_Switch_clause;
struct Cyc_Absyn_Fndecl; struct Cyc_Absyn_Structdecl; struct Cyc_Absyn_Uniondecl;
struct Cyc_Absyn_Tuniondecl; struct Cyc_Absyn_Tunionfield; struct Cyc_Absyn_Enumfield;
struct Cyc_Absyn_Enumdecl; struct Cyc_Absyn_Typedefdecl; struct Cyc_Absyn_Vardecl;
struct Cyc_Absyn_Decl; struct Cyc_Absyn_Structfield; typedef void* Cyc_Absyn_scope_t;
typedef struct Cyc_Absyn_Tqual Cyc_Absyn_tqual_t; typedef void* Cyc_Absyn_size_of_t;
typedef void* Cyc_Absyn_kind_t; typedef struct Cyc_Absyn_Tvar* Cyc_Absyn_tvar_t;
typedef void* Cyc_Absyn_sign_t; typedef struct Cyc_Absyn_Conref* Cyc_Absyn_conref_t;
typedef void* Cyc_Absyn_constraint_t; typedef void* Cyc_Absyn_bounds_t; typedef
struct Cyc_Absyn_PtrInfo Cyc_Absyn_ptr_info_t; typedef struct Cyc_Absyn_VarargInfo
Cyc_Absyn_vararg_info_t; typedef struct Cyc_Absyn_FnInfo Cyc_Absyn_fn_info_t;
typedef struct Cyc_Absyn_TunionInfo Cyc_Absyn_tunion_info_t; typedef struct Cyc_Absyn_TunionFieldInfo
Cyc_Absyn_tunion_field_info_t; typedef void* Cyc_Absyn_type_t; typedef void* Cyc_Absyn_rgntype_t;
typedef void* Cyc_Absyn_funcparams_t; typedef void* Cyc_Absyn_type_modifier_t;
typedef void* Cyc_Absyn_cnst_t; typedef void* Cyc_Absyn_primop_t; typedef void*
Cyc_Absyn_incrementor_t; typedef struct Cyc_Absyn_VarargCallInfo Cyc_Absyn_vararg_call_info_t;
typedef void* Cyc_Absyn_raw_exp_t; typedef struct Cyc_Absyn_Exp* Cyc_Absyn_exp_t;
typedef struct Cyc_Absyn_Exp* Cyc_Absyn_exp_opt_t; typedef void* Cyc_Absyn_raw_stmt_t;
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
Cyc_Absyn_VarargInfo{ struct Cyc_Core_Opt* name; struct Cyc_Absyn_Tqual tq; void*
type; void* rgn; int inject; } ; struct Cyc_Absyn_FnInfo{ struct Cyc_List_List*
tvars; struct Cyc_Core_Opt* effect; void* ret_typ; struct Cyc_List_List* args;
int c_varargs; struct Cyc_Absyn_VarargInfo* cyc_varargs; struct Cyc_List_List*
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
struct Cyc_List_List* f1; int f2; struct Cyc_Absyn_VarargInfo* f3; struct Cyc_Core_Opt*
f4; } ; static const int Cyc_Absyn_NonNullable_ps= 0; struct Cyc_Absyn_NonNullable_ps_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_Nullable_ps= 1;
struct Cyc_Absyn_Nullable_ps_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_TaggedArray_ps= 0; static const int Cyc_Absyn_Regparm_att=
0; struct Cyc_Absyn_Regparm_att_struct{ int tag; int f1; } ; static const int
Cyc_Absyn_Stdcall_att= 0; static const int Cyc_Absyn_Cdecl_att= 1; static const
int Cyc_Absyn_Fastcall_att= 2; static const int Cyc_Absyn_Noreturn_att= 3;
static const int Cyc_Absyn_Const_att= 4; static const int Cyc_Absyn_Aligned_att=
1; struct Cyc_Absyn_Aligned_att_struct{ int tag; int f1; } ; static const int
Cyc_Absyn_Packed_att= 5; static const int Cyc_Absyn_Section_att= 2; struct Cyc_Absyn_Section_att_struct{
int tag; struct _tagged_string f1; } ; static const int Cyc_Absyn_Nocommon_att=
6; static const int Cyc_Absyn_Shared_att= 7; static const int Cyc_Absyn_Unused_att=
8; static const int Cyc_Absyn_Weak_att= 9; static const int Cyc_Absyn_Dllimport_att=
10; static const int Cyc_Absyn_Dllexport_att= 11; static const int Cyc_Absyn_No_instrument_function_att=
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
unsigned char f2; } ; static const int Cyc_Absyn_Short_c= 1; struct Cyc_Absyn_Short_c_struct{
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
struct Cyc_Absyn_VarargCallInfo{ int num_varargs; struct Cyc_List_List*
injectors; struct Cyc_Absyn_VarargInfo* vai; } ; static const int Cyc_Absyn_Const_e=
0; struct Cyc_Absyn_Const_e_struct{ int tag; void* f1; } ; static const int Cyc_Absyn_Var_e=
1; struct Cyc_Absyn_Var_e_struct{ int tag; struct _tuple0* f1; void* f2; } ;
static const int Cyc_Absyn_UnknownId_e= 2; struct Cyc_Absyn_UnknownId_e_struct{
int tag; struct _tuple0* f1; } ; static const int Cyc_Absyn_Primop_e= 3; struct
Cyc_Absyn_Primop_e_struct{ int tag; void* f1; struct Cyc_List_List* f2; } ;
static const int Cyc_Absyn_AssignOp_e= 4; struct Cyc_Absyn_AssignOp_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Core_Opt* f2; struct Cyc_Absyn_Exp*
f3; } ; static const int Cyc_Absyn_Increment_e= 5; struct Cyc_Absyn_Increment_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; void* f2; } ; static const int Cyc_Absyn_Conditional_e=
6; struct Cyc_Absyn_Conditional_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp* f3; } ; static const int Cyc_Absyn_SeqExp_e=
7; struct Cyc_Absyn_SeqExp_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
Cyc_Absyn_Exp* f2; } ; static const int Cyc_Absyn_UnknownCall_e= 8; struct Cyc_Absyn_UnknownCall_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_FnCall_e= 9; struct Cyc_Absyn_FnCall_e_struct{ int tag; struct Cyc_Absyn_Exp*
f1; struct Cyc_List_List* f2; struct Cyc_Absyn_VarargCallInfo* f3; } ; static
const int Cyc_Absyn_Throw_e= 10; struct Cyc_Absyn_Throw_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_NoInstantiate_e= 11;
struct Cyc_Absyn_NoInstantiate_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_Instantiate_e= 12; struct Cyc_Absyn_Instantiate_e_struct{
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
int tag; unsigned char f1; } ; static const int Cyc_Absyn_Float_p= 3; struct Cyc_Absyn_Float_p_struct{
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
effect; void* ret_type; struct Cyc_List_List* args; int c_varargs; struct Cyc_Absyn_VarargInfo*
cyc_varargs; struct Cyc_Absyn_Stmt* body; struct Cyc_Core_Opt* cached_typ;
struct Cyc_Core_Opt* param_vardecls; struct Cyc_List_List* attributes; } ;
struct Cyc_Absyn_Structfield{ struct _tagged_string* name; struct Cyc_Absyn_Tqual
tq; void* type; struct Cyc_Core_Opt* width; struct Cyc_List_List* attributes; }
; struct Cyc_Absyn_Structdecl{ void* sc; struct Cyc_Core_Opt* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List* attributes; } ; struct
Cyc_Absyn_Uniondecl{ void* sc; struct Cyc_Core_Opt* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List* attributes; } ; struct
Cyc_Absyn_Tunionfield{ struct _tuple0* name; struct Cyc_List_List* tvs; struct
Cyc_List_List* typs; struct Cyc_Position_Segment* loc; void* sc; } ; struct Cyc_Absyn_Tuniondecl{
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
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d=
11; struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ;
struct Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc; } ; static
const int Cyc_Absyn_ArrayElement= 0; struct Cyc_Absyn_ArrayElement_struct{ int
tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_FieldName= 1;
struct Cyc_Absyn_FieldName_struct{ int tag; struct _tagged_string* f1; } ;
extern unsigned char Cyc_Absyn_EmptyAnnot[ 15u]; extern struct Cyc_Absyn_Tqual
Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref* x); extern int Cyc_Absyn_is_format_prim( void* p);
extern struct _tagged_string Cyc_Absyn_attribute2string( void*); struct Cyc_Buffer_t;
typedef struct Cyc_Buffer_t* Cyc_Buffer_T; extern int Cyc_String_strptrcmp(
struct _tagged_string* s1, struct _tagged_string* s2); extern struct
_tagged_string Cyc_String_str_sepstr( struct Cyc_List_List*, struct
_tagged_string); extern unsigned int Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp*
e); struct Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef
struct Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t;
extern unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict; typedef struct
Cyc_Dict_Dict* Cyc_Dict_gdict_t; typedef struct Cyc_Dict_Dict* Cyc_Dict_hdict_t;
typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t; extern unsigned char Cyc_Dict_Present[
12u]; extern unsigned char Cyc_Dict_Absent[ 11u]; static const int Cyc_Tcenv_VarRes=
0; struct Cyc_Tcenv_VarRes_struct{ int tag; void* f1; } ; static const int Cyc_Tcenv_StructRes=
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
typedef struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tenv_t; extern unsigned char Cyc_Tcutil_TypeErr[
12u]; extern void* Cyc_Tcutil_compress( void* t); extern int Cyc_Tcutil_is_temp_tvar(
struct Cyc_Absyn_Tvar*); extern void Cyc_Tcutil_rewrite_temp_tvar( struct Cyc_Absyn_Tvar*);
extern int Cyc_Absynpp_expand_typedefs; extern int Cyc_Absynpp_qvar_to_Cids;
extern int Cyc_Absynpp_to_VC; extern int Cyc_Absynpp_decls_first; extern int Cyc_Absynpp_print_all_tvars;
extern int Cyc_Absynpp_print_all_kinds; extern int Cyc_Absynpp_print_using_stmts;
extern int Cyc_Absynpp_print_externC_stmts; extern void Cyc_Absynpp_decllist2file(
struct Cyc_List_List* tdl, struct Cyc_Stdio___sFILE* f); extern struct Cyc_PP_Doc*
Cyc_Absynpp_decl2doc( struct Cyc_Absyn_Decl* d); extern struct _tagged_string
Cyc_Absynpp_typ2string( void*); extern struct _tagged_string Cyc_Absynpp_kind2string(
void*); extern struct _tagged_string Cyc_Absynpp_ckind2string( struct Cyc_Absyn_Conref*);
extern struct _tagged_string Cyc_Absynpp_exp2string( struct Cyc_Absyn_Exp*);
extern struct _tagged_string Cyc_Absynpp_stmt2string( struct Cyc_Absyn_Stmt*);
extern struct _tagged_string Cyc_Absynpp_qvar2string( struct _tuple0*); extern
struct _tagged_string Cyc_Absynpp_decllist2string( struct Cyc_List_List* tdl);
extern struct _tagged_string Cyc_Absynpp_prim2string( void* p); extern struct
_tagged_string Cyc_Absynpp_pat2string( struct Cyc_Absyn_Pat* p); extern struct
_tagged_string Cyc_Absynpp_scope2string( void* sc); extern int Cyc_Absynpp_exp_prec(
struct Cyc_Absyn_Exp*); extern struct _tagged_string Cyc_Absynpp_char_escape(
unsigned char); extern struct _tagged_string Cyc_Absynpp_string_escape( struct
_tagged_string); extern struct _tagged_string Cyc_Absynpp_prim2str( void* p);
extern int Cyc_Absynpp_is_declaration( struct Cyc_Absyn_Stmt* s); struct _tuple3{
struct _tagged_string* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; extern
struct _tuple1* Cyc_Absynpp_arg_mk_opt( struct _tuple3* arg); struct _tuple4{
struct Cyc_Absyn_Tqual f1; void* f2; struct Cyc_List_List* f3; } ; extern struct
_tuple4 Cyc_Absynpp_to_tms( struct Cyc_Absyn_Tqual tq, void* t); struct _tuple5{
struct Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ; extern struct Cyc_PP_Doc*
Cyc_Absynpp_dp2doc( struct _tuple5* dp); extern struct Cyc_PP_Doc* Cyc_Absynpp_switchclauses2doc(
struct Cyc_List_List* cs); extern struct Cyc_PP_Doc* Cyc_Absynpp_typ2doc( void*);
extern struct Cyc_PP_Doc* Cyc_Absynpp_structfields2doc( struct Cyc_List_List*
fields); extern struct Cyc_PP_Doc* Cyc_Absynpp_scope2doc( void*); extern struct
Cyc_PP_Doc* Cyc_Absynpp_stmt2doc( struct Cyc_Absyn_Stmt*); extern struct Cyc_PP_Doc*
Cyc_Absynpp_exp2doc( struct Cyc_Absyn_Exp*); extern struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc_prec(
int inprec, struct Cyc_Absyn_Exp* e); extern struct Cyc_PP_Doc* Cyc_Absynpp_exps2doc_prec(
int inprec, struct Cyc_List_List* es); extern struct Cyc_PP_Doc* Cyc_Absynpp_qvar2doc(
struct _tuple0*); extern struct Cyc_PP_Doc* Cyc_Absynpp_typedef_name2doc( struct
_tuple0*); extern struct Cyc_PP_Doc* Cyc_Absynpp_cnst2doc( void*); extern struct
Cyc_PP_Doc* Cyc_Absynpp_prim2doc( void*); extern struct Cyc_PP_Doc* Cyc_Absynpp_primapp2doc(
int inprec, void* p, struct Cyc_List_List* es); struct _tuple6{ struct Cyc_List_List*
f1; struct Cyc_Absyn_Exp* f2; } ; extern struct Cyc_PP_Doc* Cyc_Absynpp_de2doc(
struct _tuple6* de); extern struct Cyc_PP_Doc* Cyc_Absynpp_tqtd2doc( struct Cyc_Absyn_Tqual
tq, void* t, struct Cyc_Core_Opt* dopt); extern struct Cyc_PP_Doc* Cyc_Absynpp_funargs2doc(
struct Cyc_List_List* args, int c_varargs, struct Cyc_Absyn_VarargInfo*
cyc_varargs, struct Cyc_Core_Opt* effopt); extern struct Cyc_PP_Doc* Cyc_Absynpp_tunionfields2doc(
struct Cyc_List_List* fields); int Cyc_Absynpp_expand_typedefs= 0; int Cyc_Absynpp_qvar_to_Cids=
0; int Cyc_Absynpp_to_VC= 0; int Cyc_Absynpp_decls_first= 0; int Cyc_Absynpp_dont_rewrite_temp_tvars=
0; int Cyc_Absynpp_print_all_tvars= 0; int Cyc_Absynpp_print_all_kinds= 0; int
Cyc_Absynpp_print_using_stmts= 0; int Cyc_Absynpp_print_externC_stmts= 0; static
struct Cyc_List_List* Cyc_Absynpp_curr_namespace= 0; static void Cyc_Absynpp_curr_namespace_add(
struct _tagged_string* v){ Cyc_Absynpp_curr_namespace=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_Absynpp_curr_namespace,({
struct Cyc_List_List* _temp0=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp0->hd=( void*) v; _temp0->tl= 0; _temp0;}));} static void
Cyc_Absynpp_suppr_last( struct Cyc_List_List** l){ if((( struct Cyc_List_List*)
_check_null(* l))->tl == 0){* l= 0;} else{(( void(*)( struct Cyc_List_List** l))
Cyc_Absynpp_suppr_last)(&(( struct Cyc_List_List*) _check_null(* l))->tl);}}
static void Cyc_Absynpp_curr_namespace_drop(){(( void(*)( struct Cyc_List_List**
l)) Cyc_Absynpp_suppr_last)(& Cyc_Absynpp_curr_namespace);} struct
_tagged_string Cyc_Absynpp_char_escape( unsigned char c){ switch( c){ case '\a':
_LL1: return({ unsigned char* _temp3="\\a"; struct _tagged_string _temp4; _temp4.curr=
_temp3; _temp4.base= _temp3; _temp4.last_plus_one= _temp3 + 3; _temp4;}); case '\b':
_LL2: return({ unsigned char* _temp6="\\b"; struct _tagged_string _temp7; _temp7.curr=
_temp6; _temp7.base= _temp6; _temp7.last_plus_one= _temp6 + 3; _temp7;}); case '\f':
_LL5: return({ unsigned char* _temp9="\\f"; struct _tagged_string _temp10;
_temp10.curr= _temp9; _temp10.base= _temp9; _temp10.last_plus_one= _temp9 + 3;
_temp10;}); case '\n': _LL8: return({ unsigned char* _temp12="\\n"; struct
_tagged_string _temp13; _temp13.curr= _temp12; _temp13.base= _temp12; _temp13.last_plus_one=
_temp12 + 3; _temp13;}); case '\r': _LL11: return({ unsigned char* _temp15="\\r";
struct _tagged_string _temp16; _temp16.curr= _temp15; _temp16.base= _temp15;
_temp16.last_plus_one= _temp15 + 3; _temp16;}); case '\t': _LL14: return({
unsigned char* _temp18="\\t"; struct _tagged_string _temp19; _temp19.curr=
_temp18; _temp19.base= _temp18; _temp19.last_plus_one= _temp18 + 3; _temp19;});
case '\v': _LL17: return({ unsigned char* _temp21="\\v"; struct _tagged_string
_temp22; _temp22.curr= _temp21; _temp22.base= _temp21; _temp22.last_plus_one=
_temp21 + 3; _temp22;}); case '\\': _LL20: return({ unsigned char* _temp24="\\\\";
struct _tagged_string _temp25; _temp25.curr= _temp24; _temp25.base= _temp24;
_temp25.last_plus_one= _temp24 + 3; _temp25;}); case '"': _LL23: return({
unsigned char* _temp27="\""; struct _tagged_string _temp28; _temp28.curr=
_temp27; _temp28.base= _temp27; _temp28.last_plus_one= _temp27 + 2; _temp28;});
case '\'': _LL26: return({ unsigned char* _temp30="\\'"; struct _tagged_string
_temp31; _temp31.curr= _temp30; _temp31.base= _temp30; _temp31.last_plus_one=
_temp30 + 3; _temp31;}); default: _LL29: if( c >=' '? c <='~': 0){ struct
_tagged_string _temp33= Cyc_Core_new_string( 1); goto _LL34; _LL34:*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( _temp33, sizeof( unsigned char), 0)= c; return
_temp33;} else{ struct _tagged_string _temp35= Cyc_Core_new_string( 4); goto
_LL36; _LL36: { int j= 0;*(( unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( _temp35, sizeof(
unsigned char), j ++)='\\';*(( unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( _temp35, sizeof(
unsigned char), j ++)=( unsigned char)('0' +(( unsigned char) c >> 6 & 7));*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( _temp35, sizeof( unsigned char), j ++)=(
unsigned char)('0' +( c >> 3 & 7));*(( unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( _temp35, sizeof(
unsigned char), j ++)=( unsigned char)('0' +( c & 7)); return _temp35;}}}}
static int Cyc_Absynpp_special( struct _tagged_string s){ int sz=( int)(({
struct _tagged_string _temp37= s;( unsigned int)( _temp37.last_plus_one -
_temp37.curr);}) - 1);{ int i= 0; for( 0; i < sz; i ++){ unsigned char c=*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( s, sizeof( unsigned char), i); if((( c <=' '? 1: c >='~')?
1: c =='"')? 1: c =='\\'){ return 1;}}} return 0;} struct _tagged_string Cyc_Absynpp_string_escape(
struct _tagged_string s){ if( ! Cyc_Absynpp_special( s)){ return s;}{ int n=(
int)(({ struct _tagged_string _temp82= s;( unsigned int)( _temp82.last_plus_one
- _temp82.curr);}) - 1); if( n > 0?*(( unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( s, sizeof( unsigned char),
n) =='\000': 0){ n --;}{ int len= 0;{ int i= 0; for( 0; i <= n; i ++){
unsigned char _temp38=*(( unsigned char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( s, sizeof( unsigned char), i); _LL40:
if( _temp38 =='\a'){ goto _LL41;} else{ goto _LL42;} _LL42: if( _temp38 =='\b'){
goto _LL43;} else{ goto _LL44;} _LL44: if( _temp38 =='\f'){ goto _LL45;} else{
goto _LL46;} _LL46: if( _temp38 =='\n'){ goto _LL47;} else{ goto _LL48;} _LL48:
if( _temp38 =='\r'){ goto _LL49;} else{ goto _LL50;} _LL50: if( _temp38 =='\t'){
goto _LL51;} else{ goto _LL52;} _LL52: if( _temp38 =='\v'){ goto _LL53;} else{
goto _LL54;} _LL54: if( _temp38 =='\\'){ goto _LL55;} else{ goto _LL56;} _LL56:
if( _temp38 =='"'){ goto _LL57;} else{ goto _LL58;} _LL58: goto _LL59; _LL41:
goto _LL43; _LL43: goto _LL45; _LL45: goto _LL47; _LL47: goto _LL49; _LL49: goto
_LL51; _LL51: goto _LL53; _LL53: goto _LL55; _LL55: goto _LL57; _LL57: len += 2;
goto _LL39; _LL59: if( _temp38 >=' '? _temp38 <='~': 0){ len ++;} else{ len += 4;}
goto _LL39; _LL39:;}}{ struct _tagged_string t= Cyc_Core_new_string( len); int j=
0;{ int i= 0; for( 0; i <= n; i ++){ unsigned char _temp60=*(( unsigned char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( s,
sizeof( unsigned char), i); _LL62: if( _temp60 =='\a'){ goto _LL63;} else{ goto
_LL64;} _LL64: if( _temp60 =='\b'){ goto _LL65;} else{ goto _LL66;} _LL66: if(
_temp60 =='\f'){ goto _LL67;} else{ goto _LL68;} _LL68: if( _temp60 =='\n'){
goto _LL69;} else{ goto _LL70;} _LL70: if( _temp60 =='\r'){ goto _LL71;} else{
goto _LL72;} _LL72: if( _temp60 =='\t'){ goto _LL73;} else{ goto _LL74;} _LL74:
if( _temp60 =='\v'){ goto _LL75;} else{ goto _LL76;} _LL76: if( _temp60 =='\\'){
goto _LL77;} else{ goto _LL78;} _LL78: if( _temp60 =='"'){ goto _LL79;} else{
goto _LL80;} _LL80: goto _LL81; _LL63:*(( unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( t,
sizeof( unsigned char), j ++)='\\';*(( unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( t, sizeof( unsigned char),
j ++)='a'; goto _LL61; _LL65:*(( unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( t, sizeof( unsigned char),
j ++)='\\';*(( unsigned char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( t, sizeof( unsigned char), j ++)='b';
goto _LL61; _LL67:*(( unsigned char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( t, sizeof( unsigned char), j ++)='\\';*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( unsigned char), j ++)='f'; goto _LL61;
_LL69:*(( unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( unsigned char), j ++)='\\';*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( unsigned char), j ++)='n'; goto _LL61;
_LL71:*(( unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( unsigned char), j ++)='\\';*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( unsigned char), j ++)='r'; goto _LL61;
_LL73:*(( unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( unsigned char), j ++)='\\';*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( unsigned char), j ++)='t'; goto _LL61;
_LL75:*(( unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( unsigned char), j ++)='\\';*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( unsigned char), j ++)='v'; goto _LL61;
_LL77:*(( unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( unsigned char), j ++)='\\';*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( unsigned char), j ++)='\\'; goto _LL61;
_LL79:*(( unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( unsigned char), j ++)='\\';*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( unsigned char), j ++)='"'; goto _LL61;
_LL81: if( _temp60 >=' '? _temp60 <='~': 0){*(( unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( t,
sizeof( unsigned char), j ++)= _temp60;} else{*(( unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( t,
sizeof( unsigned char), j ++)='\\';*(( unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( t, sizeof( unsigned char),
j ++)=( unsigned char)('0' +( _temp60 >> 6 & 7));*(( unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( t,
sizeof( unsigned char), j ++)=( unsigned char)('0' +( _temp60 >> 3 & 7));*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( t, sizeof( unsigned char), j ++)=( unsigned char)('0'
+( _temp60 & 7));} goto _LL61; _LL61:;}} return t;}}}} static unsigned char
_temp85[ 9u]="restrict"; static struct _tagged_string Cyc_Absynpp_restrict_string={
_temp85, _temp85, _temp85 + 9u}; static unsigned char _temp88[ 9u]="volatile";
static struct _tagged_string Cyc_Absynpp_volatile_string={ _temp88, _temp88,
_temp88 + 9u}; static unsigned char _temp91[ 6u]="const"; static struct
_tagged_string Cyc_Absynpp_const_string={ _temp91, _temp91, _temp91 + 6u};
static struct _tagged_string* Cyc_Absynpp_restrict_sp=& Cyc_Absynpp_restrict_string;
static struct _tagged_string* Cyc_Absynpp_volatile_sp=& Cyc_Absynpp_volatile_string;
static struct _tagged_string* Cyc_Absynpp_const_sp=& Cyc_Absynpp_const_string;
struct Cyc_PP_Doc* Cyc_Absynpp_tqual2doc( struct Cyc_Absyn_Tqual tq){ struct Cyc_List_List*
l= 0; if( tq.q_restrict){ l=({ struct Cyc_List_List* _temp92=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp92->hd=( void*) Cyc_Absynpp_restrict_sp;
_temp92->tl= l; _temp92;});} if( tq.q_volatile){ l=({ struct Cyc_List_List*
_temp93=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp93->hd=( void*) Cyc_Absynpp_volatile_sp; _temp93->tl= l; _temp93;});} if(
tq.q_const){ l=({ struct Cyc_List_List* _temp94=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp94->hd=( void*) Cyc_Absynpp_const_sp;
_temp94->tl= l; _temp94;});} return Cyc_PP_egroup(({ unsigned char* _temp95="";
struct _tagged_string _temp96; _temp96.curr= _temp95; _temp96.base= _temp95;
_temp96.last_plus_one= _temp95 + 1; _temp96;}),({ unsigned char* _temp97=" ";
struct _tagged_string _temp98; _temp98.curr= _temp97; _temp98.base= _temp97;
_temp98.last_plus_one= _temp97 + 2; _temp98;}),({ unsigned char* _temp99=" ";
struct _tagged_string _temp100; _temp100.curr= _temp99; _temp100.base= _temp99;
_temp100.last_plus_one= _temp99 + 2; _temp100;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tagged_string*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_PP_textptr, l));} struct _tagged_string Cyc_Absynpp_kind2string( void* k){
void* _temp101= k; _LL103: if( _temp101 ==( void*) Cyc_Absyn_AnyKind){ goto
_LL104;} else{ goto _LL105;} _LL105: if( _temp101 ==( void*) Cyc_Absyn_MemKind){
goto _LL106;} else{ goto _LL107;} _LL107: if( _temp101 ==( void*) Cyc_Absyn_BoxKind){
goto _LL108;} else{ goto _LL109;} _LL109: if( _temp101 ==( void*) Cyc_Absyn_RgnKind){
goto _LL110;} else{ goto _LL111;} _LL111: if( _temp101 ==( void*) Cyc_Absyn_EffKind){
goto _LL112;} else{ goto _LL102;} _LL104: return({ unsigned char* _temp113="A";
struct _tagged_string _temp114; _temp114.curr= _temp113; _temp114.base= _temp113;
_temp114.last_plus_one= _temp113 + 2; _temp114;}); _LL106: return({
unsigned char* _temp115="M"; struct _tagged_string _temp116; _temp116.curr=
_temp115; _temp116.base= _temp115; _temp116.last_plus_one= _temp115 + 2;
_temp116;}); _LL108: return({ unsigned char* _temp117="B"; struct _tagged_string
_temp118; _temp118.curr= _temp117; _temp118.base= _temp117; _temp118.last_plus_one=
_temp117 + 2; _temp118;}); _LL110: return({ unsigned char* _temp119="R"; struct
_tagged_string _temp120; _temp120.curr= _temp119; _temp120.base= _temp119;
_temp120.last_plus_one= _temp119 + 2; _temp120;}); _LL112: return({
unsigned char* _temp121="E"; struct _tagged_string _temp122; _temp122.curr=
_temp121; _temp122.base= _temp121; _temp122.last_plus_one= _temp121 + 2;
_temp122;}); _LL102:;} struct _tagged_string Cyc_Absynpp_ckind2string( struct
Cyc_Absyn_Conref* c){ void* _temp123=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( c))->v; void* _temp129;
_LL125: if(( unsigned int) _temp123 > 1u?*(( int*) _temp123) == Cyc_Absyn_Eq_constr:
0){ _LL130: _temp129=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp123)->f1;
goto _LL126;} else{ goto _LL127;} _LL127: goto _LL128; _LL126: return Cyc_Absynpp_kind2string(
_temp129); _LL128: return({ unsigned char* _temp131="?"; struct _tagged_string
_temp132; _temp132.curr= _temp131; _temp132.base= _temp131; _temp132.last_plus_one=
_temp131 + 2; _temp132;}); _LL124:;} struct Cyc_PP_Doc* Cyc_Absynpp_kind2doc(
void* k){ return Cyc_PP_text( Cyc_Absynpp_kind2string( k));} struct Cyc_PP_Doc*
Cyc_Absynpp_ckind2doc( struct Cyc_Absyn_Conref* c){ return Cyc_PP_text( Cyc_Absynpp_ckind2string(
c));} struct Cyc_PP_Doc* Cyc_Absynpp_tps2doc( struct Cyc_List_List* ts){ return
Cyc_PP_egroup(({ unsigned char* _temp133="<"; struct _tagged_string _temp134;
_temp134.curr= _temp133; _temp134.base= _temp133; _temp134.last_plus_one=
_temp133 + 2; _temp134;}),({ unsigned char* _temp135=">"; struct _tagged_string
_temp136; _temp136.curr= _temp135; _temp136.base= _temp135; _temp136.last_plus_one=
_temp135 + 2; _temp136;}),({ unsigned char* _temp137=","; struct _tagged_string
_temp138; _temp138.curr= _temp137; _temp138.base= _temp137; _temp138.last_plus_one=
_temp137 + 2; _temp138;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_typ2doc, ts));}
struct Cyc_PP_Doc* Cyc_Absynpp_ktvar2doc( struct Cyc_Absyn_Tvar* tv){ void*
_temp139=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)( tv->kind))->v; void* _temp147; void* _temp149;
_LL141: if(( unsigned int) _temp139 > 1u?*(( int*) _temp139) == Cyc_Absyn_Eq_constr:
0){ _LL148: _temp147=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp139)->f1;
if( _temp147 ==( void*) Cyc_Absyn_BoxKind){ goto _LL142;} else{ goto _LL143;}}
else{ goto _LL143;} _LL143: if(( unsigned int) _temp139 > 1u?*(( int*) _temp139)
== Cyc_Absyn_Eq_constr: 0){ _LL150: _temp149=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp139)->f1; goto _LL144;} else{ goto _LL145;} _LL145: goto _LL146; _LL142:
return Cyc_PP_textptr( tv->name); _LL144: return({ struct Cyc_PP_Doc*(* _temp151)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp154= Cyc_PP_textptr(
tv->name); struct Cyc_PP_Doc* _temp155= Cyc_PP_text(({ unsigned char* _temp157="::";
struct _tagged_string _temp158; _temp158.curr= _temp157; _temp158.base= _temp157;
_temp158.last_plus_one= _temp157 + 3; _temp158;})); struct Cyc_PP_Doc* _temp156=
Cyc_Absynpp_kind2doc( _temp149); struct Cyc_PP_Doc* _temp152[ 3u]={ _temp154,
_temp155, _temp156}; struct _tagged_ptr0 _temp153={ _temp152, _temp152, _temp152
+ 3u}; _temp151( _temp153);}); _LL146: return({ struct Cyc_PP_Doc*(* _temp159)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp162= Cyc_PP_textptr(
tv->name); struct Cyc_PP_Doc* _temp163= Cyc_PP_text(({ unsigned char* _temp164="/*::?*/";
struct _tagged_string _temp165; _temp165.curr= _temp164; _temp165.base= _temp164;
_temp165.last_plus_one= _temp164 + 8; _temp165;})); struct Cyc_PP_Doc* _temp160[
2u]={ _temp162, _temp163}; struct _tagged_ptr0 _temp161={ _temp160, _temp160,
_temp160 + 2u}; _temp159( _temp161);}); _LL140:;} struct Cyc_PP_Doc* Cyc_Absynpp_ktvars2doc(
struct Cyc_List_List* tvs){ return Cyc_PP_egroup(({ unsigned char* _temp166="<";
struct _tagged_string _temp167; _temp167.curr= _temp166; _temp167.base= _temp166;
_temp167.last_plus_one= _temp166 + 2; _temp167;}),({ unsigned char* _temp168=">";
struct _tagged_string _temp169; _temp169.curr= _temp168; _temp169.base= _temp168;
_temp169.last_plus_one= _temp168 + 2; _temp169;}),({ unsigned char* _temp170=",";
struct _tagged_string _temp171; _temp171.curr= _temp170; _temp171.base= _temp170;
_temp171.last_plus_one= _temp170 + 2; _temp171;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_ktvar2doc, tvs));} static struct _tagged_string* Cyc_Absynpp_get_name(
struct Cyc_Absyn_Tvar* tv){ return tv->name;} struct Cyc_PP_Doc* Cyc_Absynpp_tvars2doc(
struct Cyc_List_List* tvs){ if( Cyc_Absynpp_print_all_kinds){ return Cyc_Absynpp_ktvars2doc(
tvs);} return Cyc_PP_egroup(({ unsigned char* _temp172="<"; struct
_tagged_string _temp173; _temp173.curr= _temp172; _temp173.base= _temp172;
_temp173.last_plus_one= _temp172 + 2; _temp173;}),({ unsigned char* _temp174=">";
struct _tagged_string _temp175; _temp175.curr= _temp174; _temp175.base= _temp174;
_temp175.last_plus_one= _temp174 + 2; _temp175;}),({ unsigned char* _temp176=",";
struct _tagged_string _temp177; _temp177.curr= _temp176; _temp177.base= _temp176;
_temp177.last_plus_one= _temp176 + 2; _temp177;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tagged_string*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_PP_textptr,(( struct Cyc_List_List*(*)( struct _tagged_string*(* f)( struct
Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_get_name,
tvs)));} struct _tuple7{ struct Cyc_Absyn_Tqual f1; void* f2; } ; struct Cyc_PP_Doc*
Cyc_Absynpp_arg2doc( struct _tuple7* t){ return Cyc_Absynpp_tqtd2doc((* t).f1,(*
t).f2, 0);} struct Cyc_PP_Doc* Cyc_Absynpp_args2doc( struct Cyc_List_List* ts){
return Cyc_PP_group(({ unsigned char* _temp178="("; struct _tagged_string
_temp179; _temp179.curr= _temp178; _temp179.base= _temp178; _temp179.last_plus_one=
_temp178 + 2; _temp179;}),({ unsigned char* _temp180=")"; struct _tagged_string
_temp181; _temp181.curr= _temp180; _temp181.base= _temp180; _temp181.last_plus_one=
_temp180 + 2; _temp181;}),({ unsigned char* _temp182=","; struct _tagged_string
_temp183; _temp183.curr= _temp182; _temp183.base= _temp182; _temp183.last_plus_one=
_temp182 + 2; _temp183;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple7*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_arg2doc,
ts));} struct Cyc_PP_Doc* Cyc_Absynpp_noncallatt2doc( void* att){ void* _temp184=
att; _LL186: if( _temp184 ==( void*) Cyc_Absyn_Stdcall_att){ goto _LL187;} else{
goto _LL188;} _LL188: if( _temp184 ==( void*) Cyc_Absyn_Cdecl_att){ goto _LL189;}
else{ goto _LL190;} _LL190: if( _temp184 ==( void*) Cyc_Absyn_Fastcall_att){
goto _LL191;} else{ goto _LL192;} _LL192: goto _LL193; _LL187: return Cyc_PP_nil_doc();
_LL189: return Cyc_PP_nil_doc(); _LL191: return Cyc_PP_nil_doc(); _LL193: return
Cyc_PP_text( Cyc_Absyn_attribute2string( att)); _LL185:;} struct Cyc_PP_Doc* Cyc_Absynpp_callconv2doc(
struct Cyc_List_List* atts){ for( 0; atts != 0; atts=(( struct Cyc_List_List*)
_check_null( atts))->tl){ void* _temp194=( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd; _LL196: if( _temp194 ==( void*) Cyc_Absyn_Stdcall_att){
goto _LL197;} else{ goto _LL198;} _LL198: if( _temp194 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL199;} else{ goto _LL200;} _LL200: if( _temp194 ==( void*) Cyc_Absyn_Fastcall_att){
goto _LL201;} else{ goto _LL202;} _LL202: goto _LL203; _LL197: return Cyc_PP_text(({
unsigned char* _temp204="_stdcall"; struct _tagged_string _temp205; _temp205.curr=
_temp204; _temp205.base= _temp204; _temp205.last_plus_one= _temp204 + 9;
_temp205;})); _LL199: return Cyc_PP_text(({ unsigned char* _temp206="_cdecl";
struct _tagged_string _temp207; _temp207.curr= _temp206; _temp207.base= _temp206;
_temp207.last_plus_one= _temp206 + 7; _temp207;})); _LL201: return Cyc_PP_text(({
unsigned char* _temp208="_fastcall"; struct _tagged_string _temp209; _temp209.curr=
_temp208; _temp209.base= _temp208; _temp209.last_plus_one= _temp208 + 10;
_temp209;})); _LL203: goto _LL195; _LL195:;} return Cyc_PP_nil_doc();} struct
Cyc_PP_Doc* Cyc_Absynpp_noncallconv2doc( struct Cyc_List_List* atts){ int hasatt=
0;{ struct Cyc_List_List* atts2= atts; for( 0; atts2 != 0; atts2=(( struct Cyc_List_List*)
_check_null( atts2))->tl){ void* _temp210=( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd; _LL212: if( _temp210 ==( void*) Cyc_Absyn_Stdcall_att){
goto _LL213;} else{ goto _LL214;} _LL214: if( _temp210 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL215;} else{ goto _LL216;} _LL216: if( _temp210 ==( void*) Cyc_Absyn_Fastcall_att){
goto _LL217;} else{ goto _LL218;} _LL218: goto _LL219; _LL213: goto _LL211;
_LL215: goto _LL211; _LL217: goto _LL211; _LL219: hasatt= 1; goto _LL211; _LL211:;}}
if( ! hasatt){ return Cyc_PP_nil_doc();} return({ struct Cyc_PP_Doc*(* _temp220)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp223= Cyc_PP_text(({
unsigned char* _temp234="__declspec("; struct _tagged_string _temp235; _temp235.curr=
_temp234; _temp235.base= _temp234; _temp235.last_plus_one= _temp234 + 12;
_temp235;})); struct Cyc_PP_Doc* _temp224= Cyc_PP_group(({ unsigned char*
_temp228=""; struct _tagged_string _temp229; _temp229.curr= _temp228; _temp229.base=
_temp228; _temp229.last_plus_one= _temp228 + 1; _temp229;}),({ unsigned char*
_temp230=""; struct _tagged_string _temp231; _temp231.curr= _temp230; _temp231.base=
_temp230; _temp231.last_plus_one= _temp230 + 1; _temp231;}),({ unsigned char*
_temp232=" "; struct _tagged_string _temp233; _temp233.curr= _temp232; _temp233.base=
_temp232; _temp233.last_plus_one= _temp232 + 2; _temp233;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_noncallatt2doc,
atts)); struct Cyc_PP_Doc* _temp225= Cyc_PP_text(({ unsigned char* _temp226=")";
struct _tagged_string _temp227; _temp227.curr= _temp226; _temp227.base= _temp226;
_temp227.last_plus_one= _temp226 + 2; _temp227;})); struct Cyc_PP_Doc* _temp221[
3u]={ _temp223, _temp224, _temp225}; struct _tagged_ptr0 _temp222={ _temp221,
_temp221, _temp221 + 3u}; _temp220( _temp222);});} struct Cyc_PP_Doc* Cyc_Absynpp_att2doc(
void* a){ return Cyc_PP_text( Cyc_Absyn_attribute2string( a));} struct Cyc_PP_Doc*
Cyc_Absynpp_atts2doc( struct Cyc_List_List* atts){ if( atts == 0){ return Cyc_PP_nil_doc();}
if( Cyc_Absynpp_to_VC){ return Cyc_Absynpp_noncallconv2doc( atts);} return({
struct Cyc_PP_Doc*(* _temp236)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp239= Cyc_PP_text(({ unsigned char* _temp247=" __attribute__"; struct
_tagged_string _temp248; _temp248.curr= _temp247; _temp248.base= _temp247;
_temp248.last_plus_one= _temp247 + 15; _temp248;})); struct Cyc_PP_Doc* _temp240=
Cyc_PP_group(({ unsigned char* _temp241="(("; struct _tagged_string _temp242;
_temp242.curr= _temp241; _temp242.base= _temp241; _temp242.last_plus_one=
_temp241 + 3; _temp242;}),({ unsigned char* _temp243="))"; struct _tagged_string
_temp244; _temp244.curr= _temp243; _temp244.base= _temp243; _temp244.last_plus_one=
_temp243 + 3; _temp244;}),({ unsigned char* _temp245=","; struct _tagged_string
_temp246; _temp246.curr= _temp245; _temp246.base= _temp245; _temp246.last_plus_one=
_temp245 + 2; _temp246;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_att2doc, atts));
struct Cyc_PP_Doc* _temp237[ 2u]={ _temp239, _temp240}; struct _tagged_ptr0
_temp238={ _temp237, _temp237, _temp237 + 2u}; _temp236( _temp238);});} int Cyc_Absynpp_next_is_pointer(
struct Cyc_List_List* tms){ if( tms == 0){ return 0;}{ void* _temp249=( void*)((
struct Cyc_List_List*) _check_null( tms))->hd; struct Cyc_Absyn_Tqual _temp255;
void* _temp257; void* _temp259; _LL251: if(( unsigned int) _temp249 > 1u?*(( int*)
_temp249) == Cyc_Absyn_Pointer_mod: 0){ _LL260: _temp259=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp249)->f1; goto _LL258; _LL258: _temp257=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp249)->f2; goto _LL256; _LL256: _temp255=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp249)->f3; goto _LL252;} else{ goto _LL253;} _LL253: goto _LL254; _LL252:
return 1; _LL254: return 0; _LL250:;}} extern struct Cyc_PP_Doc* Cyc_Absynpp_ntyp2doc(
void* t); struct Cyc_PP_Doc* Cyc_Absynpp_dtms2doc( struct Cyc_PP_Doc* d, struct
Cyc_List_List* tms){ if( tms == 0){ return d;}{ struct Cyc_PP_Doc* rest= Cyc_Absynpp_dtms2doc(
d,(( struct Cyc_List_List*) _check_null( tms))->tl); struct Cyc_PP_Doc* p_rest=({
struct Cyc_PP_Doc*(* _temp426)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp429= Cyc_PP_text(({ unsigned char* _temp434="("; struct _tagged_string
_temp435; _temp435.curr= _temp434; _temp435.base= _temp434; _temp435.last_plus_one=
_temp434 + 2; _temp435;})); struct Cyc_PP_Doc* _temp430= rest; struct Cyc_PP_Doc*
_temp431= Cyc_PP_text(({ unsigned char* _temp432=")"; struct _tagged_string
_temp433; _temp433.curr= _temp432; _temp433.base= _temp432; _temp433.last_plus_one=
_temp432 + 2; _temp433;})); struct Cyc_PP_Doc* _temp427[ 3u]={ _temp429,
_temp430, _temp431}; struct _tagged_ptr0 _temp428={ _temp427, _temp427, _temp427
+ 3u}; _temp426( _temp428);}); void* _temp261=( void*)(( struct Cyc_List_List*)
_check_null( tms))->hd; struct Cyc_Absyn_Exp* _temp275; void* _temp277; struct
Cyc_List_List* _temp279; struct Cyc_Position_Segment* _temp281; int _temp283;
struct Cyc_Position_Segment* _temp285; struct Cyc_List_List* _temp287; struct
Cyc_Absyn_Tqual _temp289; void* _temp291; void* _temp293; _LL263: if( _temp261
==( void*) Cyc_Absyn_Carray_mod){ goto _LL264;} else{ goto _LL265;} _LL265: if((
unsigned int) _temp261 > 1u?*(( int*) _temp261) == Cyc_Absyn_ConstArray_mod: 0){
_LL276: _temp275=(( struct Cyc_Absyn_ConstArray_mod_struct*) _temp261)->f1; goto
_LL266;} else{ goto _LL267;} _LL267: if(( unsigned int) _temp261 > 1u?*(( int*)
_temp261) == Cyc_Absyn_Function_mod: 0){ _LL278: _temp277=( void*)(( struct Cyc_Absyn_Function_mod_struct*)
_temp261)->f1; goto _LL268;} else{ goto _LL269;} _LL269: if(( unsigned int)
_temp261 > 1u?*(( int*) _temp261) == Cyc_Absyn_Attributes_mod: 0){ _LL282:
_temp281=(( struct Cyc_Absyn_Attributes_mod_struct*) _temp261)->f1; goto _LL280;
_LL280: _temp279=(( struct Cyc_Absyn_Attributes_mod_struct*) _temp261)->f2; goto
_LL270;} else{ goto _LL271;} _LL271: if(( unsigned int) _temp261 > 1u?*(( int*)
_temp261) == Cyc_Absyn_TypeParams_mod: 0){ _LL288: _temp287=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp261)->f1; goto _LL286; _LL286: _temp285=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp261)->f2; goto _LL284; _LL284: _temp283=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp261)->f3; goto _LL272;} else{ goto _LL273;} _LL273: if(( unsigned int)
_temp261 > 1u?*(( int*) _temp261) == Cyc_Absyn_Pointer_mod: 0){ _LL294: _temp293=(
void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp261)->f1; goto _LL292;
_LL292: _temp291=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp261)->f2;
goto _LL290; _LL290: _temp289=(( struct Cyc_Absyn_Pointer_mod_struct*) _temp261)->f3;
goto _LL274;} else{ goto _LL262;} _LL264: if( Cyc_Absynpp_next_is_pointer(((
struct Cyc_List_List*) _check_null( tms))->tl)){ rest= p_rest;} return({ struct
Cyc_PP_Doc*(* _temp295)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp298= rest; struct Cyc_PP_Doc* _temp299= Cyc_PP_text(({ unsigned char*
_temp300="[]"; struct _tagged_string _temp301; _temp301.curr= _temp300; _temp301.base=
_temp300; _temp301.last_plus_one= _temp300 + 3; _temp301;})); struct Cyc_PP_Doc*
_temp296[ 2u]={ _temp298, _temp299}; struct _tagged_ptr0 _temp297={ _temp296,
_temp296, _temp296 + 2u}; _temp295( _temp297);}); _LL266: if( Cyc_Absynpp_next_is_pointer(((
struct Cyc_List_List*) _check_null( tms))->tl)){ rest= p_rest;} return({ struct
Cyc_PP_Doc*(* _temp302)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp305= rest; struct Cyc_PP_Doc* _temp306= Cyc_PP_text(({ unsigned char*
_temp311="["; struct _tagged_string _temp312; _temp312.curr= _temp311; _temp312.base=
_temp311; _temp312.last_plus_one= _temp311 + 2; _temp312;})); struct Cyc_PP_Doc*
_temp307= Cyc_Absynpp_exp2doc( _temp275); struct Cyc_PP_Doc* _temp308= Cyc_PP_text(({
unsigned char* _temp309="]"; struct _tagged_string _temp310; _temp310.curr=
_temp309; _temp310.base= _temp309; _temp310.last_plus_one= _temp309 + 2;
_temp310;})); struct Cyc_PP_Doc* _temp303[ 4u]={ _temp305, _temp306, _temp307,
_temp308}; struct _tagged_ptr0 _temp304={ _temp303, _temp303, _temp303 + 4u};
_temp302( _temp304);}); _LL268: if( Cyc_Absynpp_next_is_pointer((( struct Cyc_List_List*)
_check_null( tms))->tl)){ rest= p_rest;}{ void* _temp313= _temp277; struct Cyc_Core_Opt*
_temp319; struct Cyc_Absyn_VarargInfo* _temp321; int _temp323; struct Cyc_List_List*
_temp325; struct Cyc_Position_Segment* _temp327; struct Cyc_List_List* _temp329;
_LL315: if(*(( int*) _temp313) == Cyc_Absyn_WithTypes){ _LL326: _temp325=((
struct Cyc_Absyn_WithTypes_struct*) _temp313)->f1; goto _LL324; _LL324: _temp323=((
struct Cyc_Absyn_WithTypes_struct*) _temp313)->f2; goto _LL322; _LL322: _temp321=((
struct Cyc_Absyn_WithTypes_struct*) _temp313)->f3; goto _LL320; _LL320: _temp319=((
struct Cyc_Absyn_WithTypes_struct*) _temp313)->f4; goto _LL316;} else{ goto
_LL317;} _LL317: if(*(( int*) _temp313) == Cyc_Absyn_NoTypes){ _LL330: _temp329=((
struct Cyc_Absyn_NoTypes_struct*) _temp313)->f1; goto _LL328; _LL328: _temp327=((
struct Cyc_Absyn_NoTypes_struct*) _temp313)->f2; goto _LL318;} else{ goto _LL314;}
_LL316: return({ struct Cyc_PP_Doc*(* _temp331)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp334= rest; struct Cyc_PP_Doc* _temp335= Cyc_Absynpp_funargs2doc(
_temp325, _temp323, _temp321, _temp319); struct Cyc_PP_Doc* _temp332[ 2u]={
_temp334, _temp335}; struct _tagged_ptr0 _temp333={ _temp332, _temp332, _temp332
+ 2u}; _temp331( _temp333);}); _LL318: return({ struct Cyc_PP_Doc*(* _temp336)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp339= rest; struct Cyc_PP_Doc*
_temp340= Cyc_PP_group(({ unsigned char* _temp341="("; struct _tagged_string
_temp342; _temp342.curr= _temp341; _temp342.base= _temp341; _temp342.last_plus_one=
_temp341 + 2; _temp342;}),({ unsigned char* _temp343=")"; struct _tagged_string
_temp344; _temp344.curr= _temp343; _temp344.base= _temp343; _temp344.last_plus_one=
_temp343 + 2; _temp344;}),({ unsigned char* _temp345=","; struct _tagged_string
_temp346; _temp346.curr= _temp345; _temp346.base= _temp345; _temp346.last_plus_one=
_temp345 + 2; _temp346;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tagged_string*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_PP_textptr,
_temp329)); struct Cyc_PP_Doc* _temp337[ 2u]={ _temp339, _temp340}; struct
_tagged_ptr0 _temp338={ _temp337, _temp337, _temp337 + 2u}; _temp336( _temp338);});
_LL314:;} _LL270: if( Cyc_Absynpp_next_is_pointer((( struct Cyc_List_List*)
_check_null( tms))->tl)){ rest= p_rest;} return({ struct Cyc_PP_Doc*(* _temp347)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp350= rest; struct Cyc_PP_Doc*
_temp351= Cyc_Absynpp_atts2doc( _temp279); struct Cyc_PP_Doc* _temp348[ 2u]={
_temp350, _temp351}; struct _tagged_ptr0 _temp349={ _temp348, _temp348, _temp348
+ 2u}; _temp347( _temp349);}); _LL272: if( Cyc_Absynpp_next_is_pointer((( struct
Cyc_List_List*) _check_null( tms))->tl)){ rest= p_rest;} if( _temp283){ return({
struct Cyc_PP_Doc*(* _temp352)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp355= rest; struct Cyc_PP_Doc* _temp356= Cyc_Absynpp_ktvars2doc( _temp287);
struct Cyc_PP_Doc* _temp353[ 2u]={ _temp355, _temp356}; struct _tagged_ptr0
_temp354={ _temp353, _temp353, _temp353 + 2u}; _temp352( _temp354);});} else{
return({ struct Cyc_PP_Doc*(* _temp357)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp360= rest; struct Cyc_PP_Doc* _temp361= Cyc_Absynpp_tvars2doc(
_temp287); struct Cyc_PP_Doc* _temp358[ 2u]={ _temp360, _temp361}; struct
_tagged_ptr0 _temp359={ _temp358, _temp358, _temp358 + 2u}; _temp357( _temp359);});}
_LL274: { struct Cyc_PP_Doc* ptr;{ void* _temp362= _temp293; struct Cyc_Absyn_Exp*
_temp370; struct Cyc_Absyn_Exp* _temp372; _LL364: if(( unsigned int) _temp362 >
1u?*(( int*) _temp362) == Cyc_Absyn_Nullable_ps: 0){ _LL371: _temp370=(( struct
Cyc_Absyn_Nullable_ps_struct*) _temp362)->f1; goto _LL365;} else{ goto _LL366;}
_LL366: if(( unsigned int) _temp362 > 1u?*(( int*) _temp362) == Cyc_Absyn_NonNullable_ps:
0){ _LL373: _temp372=(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp362)->f1;
goto _LL367;} else{ goto _LL368;} _LL368: if( _temp362 ==( void*) Cyc_Absyn_TaggedArray_ps){
goto _LL369;} else{ goto _LL363;} _LL365: if( Cyc_Evexp_eval_const_uint_exp(
_temp370) == 1){ ptr= Cyc_PP_text(({ unsigned char* _temp374="*"; struct
_tagged_string _temp375; _temp375.curr= _temp374; _temp375.base= _temp374;
_temp375.last_plus_one= _temp374 + 2; _temp375;}));} else{ ptr=({ struct Cyc_PP_Doc*(*
_temp376)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp379= Cyc_PP_text(({
unsigned char* _temp387="*"; struct _tagged_string _temp388; _temp388.curr=
_temp387; _temp388.base= _temp387; _temp388.last_plus_one= _temp387 + 2;
_temp388;})); struct Cyc_PP_Doc* _temp380= Cyc_PP_text(({ unsigned char*
_temp385="{"; struct _tagged_string _temp386; _temp386.curr= _temp385; _temp386.base=
_temp385; _temp386.last_plus_one= _temp385 + 2; _temp386;})); struct Cyc_PP_Doc*
_temp381= Cyc_Absynpp_exp2doc( _temp370); struct Cyc_PP_Doc* _temp382= Cyc_PP_text(({
unsigned char* _temp383="}"; struct _tagged_string _temp384; _temp384.curr=
_temp383; _temp384.base= _temp383; _temp384.last_plus_one= _temp383 + 2;
_temp384;})); struct Cyc_PP_Doc* _temp377[ 4u]={ _temp379, _temp380, _temp381,
_temp382}; struct _tagged_ptr0 _temp378={ _temp377, _temp377, _temp377 + 4u};
_temp376( _temp378);});} goto _LL363; _LL367: if( Cyc_Evexp_eval_const_uint_exp(
_temp372) == 1){ ptr= Cyc_PP_text(({ unsigned char* _temp389="@"; struct
_tagged_string _temp390; _temp390.curr= _temp389; _temp390.base= _temp389;
_temp390.last_plus_one= _temp389 + 2; _temp390;}));} else{ ptr=({ struct Cyc_PP_Doc*(*
_temp391)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp394= Cyc_PP_text(({
unsigned char* _temp402="@"; struct _tagged_string _temp403; _temp403.curr=
_temp402; _temp403.base= _temp402; _temp403.last_plus_one= _temp402 + 2;
_temp403;})); struct Cyc_PP_Doc* _temp395= Cyc_PP_text(({ unsigned char*
_temp400="{"; struct _tagged_string _temp401; _temp401.curr= _temp400; _temp401.base=
_temp400; _temp401.last_plus_one= _temp400 + 2; _temp401;})); struct Cyc_PP_Doc*
_temp396= Cyc_Absynpp_exp2doc( _temp372); struct Cyc_PP_Doc* _temp397= Cyc_PP_text(({
unsigned char* _temp398="}"; struct _tagged_string _temp399; _temp399.curr=
_temp398; _temp399.base= _temp398; _temp399.last_plus_one= _temp398 + 2;
_temp399;})); struct Cyc_PP_Doc* _temp392[ 4u]={ _temp394, _temp395, _temp396,
_temp397}; struct _tagged_ptr0 _temp393={ _temp392, _temp392, _temp392 + 4u};
_temp391( _temp393);});} goto _LL363; _LL369: ptr= Cyc_PP_text(({ unsigned char*
_temp404="?"; struct _tagged_string _temp405; _temp405.curr= _temp404; _temp405.base=
_temp404; _temp405.last_plus_one= _temp404 + 2; _temp405;})); goto _LL363;
_LL363:;}{ void* _temp406= Cyc_Tcutil_compress( _temp291); _LL408: if( _temp406
==( void*) Cyc_Absyn_HeapRgn){ goto _LL409;} else{ goto _LL410;} _LL410: goto
_LL411; _LL409: return({ struct Cyc_PP_Doc*(* _temp412)( struct _tagged_ptr0)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp415= ptr; struct Cyc_PP_Doc* _temp416= rest;
struct Cyc_PP_Doc* _temp413[ 2u]={ _temp415, _temp416}; struct _tagged_ptr0
_temp414={ _temp413, _temp413, _temp413 + 2u}; _temp412( _temp414);}); _LL411:
return({ struct Cyc_PP_Doc*(* _temp417)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp420= ptr; struct Cyc_PP_Doc* _temp421= Cyc_Absynpp_ntyp2doc(
_temp291); struct Cyc_PP_Doc* _temp422= Cyc_PP_text(({ unsigned char* _temp424=" ";
struct _tagged_string _temp425; _temp425.curr= _temp424; _temp425.base= _temp424;
_temp425.last_plus_one= _temp424 + 2; _temp425;})); struct Cyc_PP_Doc* _temp423=
rest; struct Cyc_PP_Doc* _temp418[ 4u]={ _temp420, _temp421, _temp422, _temp423};
struct _tagged_ptr0 _temp419={ _temp418, _temp418, _temp418 + 4u}; _temp417(
_temp419);}); _LL407:;}} _LL262:;}} struct Cyc_PP_Doc* Cyc_Absynpp_rgn2doc( void*
t){ void* _temp436= Cyc_Tcutil_compress( t); _LL438: if( _temp436 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL439;} else{ goto _LL440;} _LL440: goto _LL441; _LL439: return Cyc_PP_text(({
unsigned char* _temp442="`H"; struct _tagged_string _temp443; _temp443.curr=
_temp442; _temp443.base= _temp442; _temp443.last_plus_one= _temp442 + 3;
_temp443;})); _LL441: return Cyc_Absynpp_ntyp2doc( t); _LL437:;} static void Cyc_Absynpp_effects2docs(
struct Cyc_List_List** regions, struct Cyc_List_List** effects, void* t){ void*
_temp444= Cyc_Tcutil_compress( t); void* _temp452; struct Cyc_List_List*
_temp454; _LL446: if(( unsigned int) _temp444 > 4u?*(( int*) _temp444) == Cyc_Absyn_AccessEff:
0){ _LL453: _temp452=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp444)->f1;
goto _LL447;} else{ goto _LL448;} _LL448: if(( unsigned int) _temp444 > 4u?*((
int*) _temp444) == Cyc_Absyn_JoinEff: 0){ _LL455: _temp454=(( struct Cyc_Absyn_JoinEff_struct*)
_temp444)->f1; goto _LL449;} else{ goto _LL450;} _LL450: goto _LL451; _LL447:*
regions=({ struct Cyc_List_List* _temp456=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp456->hd=( void*) Cyc_Absynpp_rgn2doc(
_temp452); _temp456->tl=* regions; _temp456;}); goto _LL445; _LL449: for( 0;
_temp454 != 0; _temp454=(( struct Cyc_List_List*) _check_null( _temp454))->tl){
Cyc_Absynpp_effects2docs( regions, effects,( void*)(( struct Cyc_List_List*)
_check_null( _temp454))->hd);} goto _LL445; _LL451:* effects=({ struct Cyc_List_List*
_temp457=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp457->hd=( void*) Cyc_Absynpp_ntyp2doc( t); _temp457->tl=* effects; _temp457;});
goto _LL445; _LL445:;} struct Cyc_PP_Doc* Cyc_Absynpp_eff2doc( void* t){ struct
Cyc_List_List* regions= 0; struct Cyc_List_List* effects= 0; Cyc_Absynpp_effects2docs(&
regions,& effects, t); regions=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( regions); effects=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( effects); if( regions == 0? effects != 0: 0){ return Cyc_PP_group(({
unsigned char* _temp458=""; struct _tagged_string _temp459; _temp459.curr=
_temp458; _temp459.base= _temp458; _temp459.last_plus_one= _temp458 + 1;
_temp459;}),({ unsigned char* _temp460=""; struct _tagged_string _temp461;
_temp461.curr= _temp460; _temp461.base= _temp460; _temp461.last_plus_one=
_temp460 + 1; _temp461;}),({ unsigned char* _temp462="+"; struct _tagged_string
_temp463; _temp463.curr= _temp462; _temp463.base= _temp462; _temp463.last_plus_one=
_temp462 + 2; _temp463;}), effects);} else{ struct Cyc_PP_Doc* _temp470= Cyc_PP_group(({
unsigned char* _temp464="{"; struct _tagged_string _temp465; _temp465.curr=
_temp464; _temp465.base= _temp464; _temp465.last_plus_one= _temp464 + 2;
_temp465;}),({ unsigned char* _temp466="}"; struct _tagged_string _temp467;
_temp467.curr= _temp466; _temp467.base= _temp466; _temp467.last_plus_one=
_temp466 + 2; _temp467;}),({ unsigned char* _temp468=","; struct _tagged_string
_temp469; _temp469.curr= _temp468; _temp469.base= _temp468; _temp469.last_plus_one=
_temp468 + 2; _temp469;}), regions); goto _LL471; _LL471: return Cyc_PP_group(({
unsigned char* _temp472=""; struct _tagged_string _temp473; _temp473.curr=
_temp472; _temp473.base= _temp472; _temp473.last_plus_one= _temp472 + 1;
_temp473;}),({ unsigned char* _temp474=""; struct _tagged_string _temp475;
_temp475.curr= _temp474; _temp475.base= _temp474; _temp475.last_plus_one=
_temp474 + 1; _temp475;}),({ unsigned char* _temp476="+"; struct _tagged_string
_temp477; _temp477.curr= _temp476; _temp477.base= _temp476; _temp477.last_plus_one=
_temp476 + 2; _temp477;}),(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_imp_append)( effects,({ struct Cyc_List_List*
_temp478=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp478->hd=( void*) _temp470; _temp478->tl= 0; _temp478;})));}} struct Cyc_PP_Doc*
Cyc_Absynpp_ntyp2doc( void* t){ struct Cyc_PP_Doc* s;{ void* _temp479= t; struct
Cyc_Absyn_Exp* _temp525; struct Cyc_Absyn_Tqual _temp527; void* _temp529; struct
Cyc_Absyn_FnInfo _temp531; struct Cyc_Absyn_PtrInfo _temp533; int _temp535;
struct Cyc_Core_Opt* _temp537; void* _temp539; struct Cyc_Absyn_Tvar* _temp541;
struct Cyc_Absyn_TunionInfo _temp543; void* _temp545; struct Cyc_List_List*
_temp547; void* _temp549; struct Cyc_Absyn_TunionFieldInfo _temp551; struct Cyc_List_List*
_temp553; void* _temp555; void* _temp557; void* _temp559; struct Cyc_List_List*
_temp561; struct Cyc_Absyn_Structdecl** _temp563; struct Cyc_List_List* _temp565;
struct _tuple0* _temp567; struct Cyc_Absyn_Uniondecl** _temp569; struct Cyc_List_List*
_temp571; struct _tuple0* _temp573; struct Cyc_List_List* _temp575; struct Cyc_List_List*
_temp577; struct Cyc_Absyn_Enumdecl* _temp579; struct _tuple0* _temp581; struct
Cyc_Core_Opt* _temp583; struct Cyc_List_List* _temp585; struct _tuple0* _temp587;
void* _temp589; void* _temp591; struct Cyc_List_List* _temp593; _LL481: if((
unsigned int) _temp479 > 4u?*(( int*) _temp479) == Cyc_Absyn_ArrayType: 0){
_LL530: _temp529=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp479)->f1;
goto _LL528; _LL528: _temp527=(( struct Cyc_Absyn_ArrayType_struct*) _temp479)->f2;
goto _LL526; _LL526: _temp525=(( struct Cyc_Absyn_ArrayType_struct*) _temp479)->f3;
goto _LL482;} else{ goto _LL483;} _LL483: if(( unsigned int) _temp479 > 4u?*((
int*) _temp479) == Cyc_Absyn_FnType: 0){ _LL532: _temp531=(( struct Cyc_Absyn_FnType_struct*)
_temp479)->f1; goto _LL484;} else{ goto _LL485;} _LL485: if(( unsigned int)
_temp479 > 4u?*(( int*) _temp479) == Cyc_Absyn_PointerType: 0){ _LL534: _temp533=((
struct Cyc_Absyn_PointerType_struct*) _temp479)->f1; goto _LL486;} else{ goto
_LL487;} _LL487: if( _temp479 ==( void*) Cyc_Absyn_VoidType){ goto _LL488;}
else{ goto _LL489;} _LL489: if(( unsigned int) _temp479 > 4u?*(( int*) _temp479)
== Cyc_Absyn_Evar: 0){ _LL540: _temp539=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp479)->f1; goto _LL538; _LL538: _temp537=(( struct Cyc_Absyn_Evar_struct*)
_temp479)->f2; goto _LL536; _LL536: _temp535=(( struct Cyc_Absyn_Evar_struct*)
_temp479)->f3; goto _LL490;} else{ goto _LL491;} _LL491: if(( unsigned int)
_temp479 > 4u?*(( int*) _temp479) == Cyc_Absyn_VarType: 0){ _LL542: _temp541=((
struct Cyc_Absyn_VarType_struct*) _temp479)->f1; goto _LL492;} else{ goto _LL493;}
_LL493: if(( unsigned int) _temp479 > 4u?*(( int*) _temp479) == Cyc_Absyn_TunionType:
0){ _LL544: _temp543=(( struct Cyc_Absyn_TunionType_struct*) _temp479)->f1;
_LL550: _temp549=( void*) _temp543.tunion_info; goto _LL548; _LL548: _temp547=
_temp543.targs; goto _LL546; _LL546: _temp545=( void*) _temp543.rgn; goto _LL494;}
else{ goto _LL495;} _LL495: if(( unsigned int) _temp479 > 4u?*(( int*) _temp479)
== Cyc_Absyn_TunionFieldType: 0){ _LL552: _temp551=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp479)->f1; _LL556: _temp555=( void*) _temp551.field_info; goto _LL554;
_LL554: _temp553= _temp551.targs; goto _LL496;} else{ goto _LL497;} _LL497: if((
unsigned int) _temp479 > 4u?*(( int*) _temp479) == Cyc_Absyn_IntType: 0){ _LL560:
_temp559=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp479)->f1; goto _LL558;
_LL558: _temp557=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp479)->f2;
goto _LL498;} else{ goto _LL499;} _LL499: if( _temp479 ==( void*) Cyc_Absyn_FloatType){
goto _LL500;} else{ goto _LL501;} _LL501: if( _temp479 ==( void*) Cyc_Absyn_DoubleType){
goto _LL502;} else{ goto _LL503;} _LL503: if(( unsigned int) _temp479 > 4u?*((
int*) _temp479) == Cyc_Absyn_TupleType: 0){ _LL562: _temp561=(( struct Cyc_Absyn_TupleType_struct*)
_temp479)->f1; goto _LL504;} else{ goto _LL505;} _LL505: if(( unsigned int)
_temp479 > 4u?*(( int*) _temp479) == Cyc_Absyn_StructType: 0){ _LL568: _temp567=((
struct Cyc_Absyn_StructType_struct*) _temp479)->f1; goto _LL566; _LL566:
_temp565=(( struct Cyc_Absyn_StructType_struct*) _temp479)->f2; goto _LL564;
_LL564: _temp563=(( struct Cyc_Absyn_StructType_struct*) _temp479)->f3; goto
_LL506;} else{ goto _LL507;} _LL507: if(( unsigned int) _temp479 > 4u?*(( int*)
_temp479) == Cyc_Absyn_UnionType: 0){ _LL574: _temp573=(( struct Cyc_Absyn_UnionType_struct*)
_temp479)->f1; goto _LL572; _LL572: _temp571=(( struct Cyc_Absyn_UnionType_struct*)
_temp479)->f2; goto _LL570; _LL570: _temp569=(( struct Cyc_Absyn_UnionType_struct*)
_temp479)->f3; goto _LL508;} else{ goto _LL509;} _LL509: if(( unsigned int)
_temp479 > 4u?*(( int*) _temp479) == Cyc_Absyn_AnonStructType: 0){ _LL576:
_temp575=(( struct Cyc_Absyn_AnonStructType_struct*) _temp479)->f1; goto _LL510;}
else{ goto _LL511;} _LL511: if(( unsigned int) _temp479 > 4u?*(( int*) _temp479)
== Cyc_Absyn_AnonUnionType: 0){ _LL578: _temp577=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp479)->f1; goto _LL512;} else{ goto _LL513;} _LL513: if(( unsigned int)
_temp479 > 4u?*(( int*) _temp479) == Cyc_Absyn_EnumType: 0){ _LL582: _temp581=((
struct Cyc_Absyn_EnumType_struct*) _temp479)->f1; goto _LL580; _LL580: _temp579=((
struct Cyc_Absyn_EnumType_struct*) _temp479)->f2; goto _LL514;} else{ goto
_LL515;} _LL515: if(( unsigned int) _temp479 > 4u?*(( int*) _temp479) == Cyc_Absyn_TypedefType:
0){ _LL588: _temp587=(( struct Cyc_Absyn_TypedefType_struct*) _temp479)->f1;
goto _LL586; _LL586: _temp585=(( struct Cyc_Absyn_TypedefType_struct*) _temp479)->f2;
goto _LL584; _LL584: _temp583=(( struct Cyc_Absyn_TypedefType_struct*) _temp479)->f3;
goto _LL516;} else{ goto _LL517;} _LL517: if(( unsigned int) _temp479 > 4u?*((
int*) _temp479) == Cyc_Absyn_RgnHandleType: 0){ _LL590: _temp589=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp479)->f1; goto _LL518;} else{ goto
_LL519;} _LL519: if( _temp479 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL520;} else{
goto _LL521;} _LL521: if(( unsigned int) _temp479 > 4u?*(( int*) _temp479) ==
Cyc_Absyn_AccessEff: 0){ _LL592: _temp591=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp479)->f1; goto _LL522;} else{ goto _LL523;} _LL523: if(( unsigned int)
_temp479 > 4u?*(( int*) _temp479) == Cyc_Absyn_JoinEff: 0){ _LL594: _temp593=((
struct Cyc_Absyn_JoinEff_struct*) _temp479)->f1; goto _LL524;} else{ goto _LL480;}
_LL482: return Cyc_PP_text(({ unsigned char* _temp595="[[[array]]]"; struct
_tagged_string _temp596; _temp596.curr= _temp595; _temp596.base= _temp595;
_temp596.last_plus_one= _temp595 + 12; _temp596;})); _LL484: return Cyc_PP_nil_doc();
_LL486: return Cyc_PP_nil_doc(); _LL488: s= Cyc_PP_text(({ unsigned char*
_temp597="void"; struct _tagged_string _temp598; _temp598.curr= _temp597;
_temp598.base= _temp597; _temp598.last_plus_one= _temp597 + 5; _temp598;}));
goto _LL480; _LL490: if( _temp537 != 0){ return Cyc_Absynpp_ntyp2doc(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp537))->v);} else{ s=({ struct Cyc_PP_Doc*(*
_temp599)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp602= Cyc_PP_text(({
unsigned char* _temp611="%"; struct _tagged_string _temp612; _temp612.curr=
_temp611; _temp612.base= _temp611; _temp612.last_plus_one= _temp611 + 2;
_temp612;})); struct Cyc_PP_Doc* _temp603= Cyc_PP_text(({ unsigned char*
_temp609="("; struct _tagged_string _temp610; _temp610.curr= _temp609; _temp610.base=
_temp609; _temp610.last_plus_one= _temp609 + 2; _temp610;})); struct Cyc_PP_Doc*
_temp604= Cyc_PP_text( xprintf("%d", _temp535)); struct Cyc_PP_Doc* _temp605=
Cyc_PP_text(({ unsigned char* _temp607=")::"; struct _tagged_string _temp608;
_temp608.curr= _temp607; _temp608.base= _temp607; _temp608.last_plus_one=
_temp607 + 4; _temp608;})); struct Cyc_PP_Doc* _temp606= Cyc_Absynpp_kind2doc(
_temp539); struct Cyc_PP_Doc* _temp600[ 5u]={ _temp602, _temp603, _temp604,
_temp605, _temp606}; struct _tagged_ptr0 _temp601={ _temp600, _temp600, _temp600
+ 5u}; _temp599( _temp601);});} goto _LL480; _LL492: s= Cyc_PP_textptr( _temp541->name);
if( Cyc_Absynpp_print_all_kinds){ s=({ struct Cyc_PP_Doc*(* _temp613)( struct
_tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp616= s; struct Cyc_PP_Doc*
_temp617= Cyc_PP_text(({ unsigned char* _temp619="::"; struct _tagged_string
_temp620; _temp620.curr= _temp619; _temp620.base= _temp619; _temp620.last_plus_one=
_temp619 + 3; _temp620;})); struct Cyc_PP_Doc* _temp618= Cyc_Absynpp_ckind2doc(
_temp541->kind); struct Cyc_PP_Doc* _temp614[ 3u]={ _temp616, _temp617, _temp618};
struct _tagged_ptr0 _temp615={ _temp614, _temp614, _temp614 + 3u}; _temp613(
_temp615);});} if( ! Cyc_Absynpp_dont_rewrite_temp_tvars? Cyc_Tcutil_is_temp_tvar(
_temp541): 0){ s=({ struct Cyc_PP_Doc*(* _temp621)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp624= Cyc_PP_text(({ unsigned char* _temp629="_ /* ";
struct _tagged_string _temp630; _temp630.curr= _temp629; _temp630.base= _temp629;
_temp630.last_plus_one= _temp629 + 6; _temp630;})); struct Cyc_PP_Doc* _temp625=
s; struct Cyc_PP_Doc* _temp626= Cyc_PP_text(({ unsigned char* _temp627=" */";
struct _tagged_string _temp628; _temp628.curr= _temp627; _temp628.base= _temp627;
_temp628.last_plus_one= _temp627 + 4; _temp628;})); struct Cyc_PP_Doc* _temp622[
3u]={ _temp624, _temp625, _temp626}; struct _tagged_ptr0 _temp623={ _temp622,
_temp622, _temp622 + 3u}; _temp621( _temp623);});} goto _LL480; _LL494:{ void*
_temp631= _temp549; struct Cyc_Absyn_UnknownTunionInfo _temp637; int _temp639;
struct _tuple0* _temp641; struct Cyc_Absyn_Tuniondecl* _temp643; struct Cyc_Absyn_Tuniondecl
_temp645; int _temp646; struct Cyc_Core_Opt* _temp648; struct Cyc_List_List*
_temp650; struct _tuple0* _temp652; void* _temp654; _LL633: if(*(( int*)
_temp631) == Cyc_Absyn_UnknownTunion){ _LL638: _temp637=(( struct Cyc_Absyn_UnknownTunion_struct*)
_temp631)->f1; _LL642: _temp641= _temp637.name; goto _LL640; _LL640: _temp639=
_temp637.is_xtunion; goto _LL634;} else{ goto _LL635;} _LL635: if(*(( int*)
_temp631) == Cyc_Absyn_KnownTunion){ _LL644: _temp643=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp631)->f1; _temp645=* _temp643; _LL655: _temp654=( void*) _temp645.sc; goto
_LL653; _LL653: _temp652= _temp645.name; goto _LL651; _LL651: _temp650= _temp645.tvs;
goto _LL649; _LL649: _temp648= _temp645.fields; goto _LL647; _LL647: _temp646=
_temp645.is_xtunion; goto _LL636;} else{ goto _LL632;} _LL634: _temp652=
_temp641; _temp646= _temp639; goto _LL636; _LL636: { struct Cyc_PP_Doc* _temp660=
Cyc_PP_text( _temp646?({ unsigned char* _temp656="xtunion "; struct
_tagged_string _temp657; _temp657.curr= _temp656; _temp657.base= _temp656;
_temp657.last_plus_one= _temp656 + 9; _temp657;}):({ unsigned char* _temp658="tunion ";
struct _tagged_string _temp659; _temp659.curr= _temp658; _temp659.base= _temp658;
_temp659.last_plus_one= _temp658 + 8; _temp659;})); goto _LL661; _LL661:{ void*
_temp662= Cyc_Tcutil_compress( _temp545); _LL664: if( _temp662 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL665;} else{ goto _LL666;} _LL666: goto _LL667; _LL665: s=({ struct Cyc_PP_Doc*(*
_temp668)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp671=
_temp660; struct Cyc_PP_Doc* _temp672= Cyc_Absynpp_qvar2doc( _temp652); struct
Cyc_PP_Doc* _temp673= Cyc_Absynpp_tps2doc( _temp547); struct Cyc_PP_Doc*
_temp669[ 3u]={ _temp671, _temp672, _temp673}; struct _tagged_ptr0 _temp670={
_temp669, _temp669, _temp669 + 3u}; _temp668( _temp670);}); goto _LL663; _LL667:
s=({ struct Cyc_PP_Doc*(* _temp674)( struct _tagged_ptr0)= Cyc_PP_cat; struct
Cyc_PP_Doc* _temp677= _temp660; struct Cyc_PP_Doc* _temp678= Cyc_Absynpp_typ2doc(
_temp545); struct Cyc_PP_Doc* _temp679= Cyc_PP_text(({ unsigned char* _temp682=" ";
struct _tagged_string _temp683; _temp683.curr= _temp682; _temp683.base= _temp682;
_temp683.last_plus_one= _temp682 + 2; _temp683;})); struct Cyc_PP_Doc* _temp680=
Cyc_Absynpp_qvar2doc( _temp652); struct Cyc_PP_Doc* _temp681= Cyc_Absynpp_tps2doc(
_temp547); struct Cyc_PP_Doc* _temp675[ 5u]={ _temp677, _temp678, _temp679,
_temp680, _temp681}; struct _tagged_ptr0 _temp676={ _temp675, _temp675, _temp675
+ 5u}; _temp674( _temp676);}); goto _LL663; _LL663:;} goto _LL632;} _LL632:;}
goto _LL480; _LL496:{ void* _temp684= _temp555; struct Cyc_Absyn_UnknownTunionFieldInfo
_temp690; int _temp692; struct _tuple0* _temp694; struct _tuple0* _temp696;
struct Cyc_Absyn_Tunionfield* _temp698; struct Cyc_Absyn_Tunionfield _temp700;
void* _temp701; struct Cyc_Position_Segment* _temp703; struct Cyc_List_List*
_temp705; struct Cyc_List_List* _temp707; struct _tuple0* _temp709; struct Cyc_Absyn_Tuniondecl*
_temp711; struct Cyc_Absyn_Tuniondecl _temp713; int _temp714; struct Cyc_Core_Opt*
_temp716; struct Cyc_List_List* _temp718; struct _tuple0* _temp720; void*
_temp722; _LL686: if(*(( int*) _temp684) == Cyc_Absyn_UnknownTunionfield){
_LL691: _temp690=(( struct Cyc_Absyn_UnknownTunionfield_struct*) _temp684)->f1;
_LL697: _temp696= _temp690.tunion_name; goto _LL695; _LL695: _temp694= _temp690.field_name;
goto _LL693; _LL693: _temp692= _temp690.is_xtunion; goto _LL687;} else{ goto
_LL688;} _LL688: if(*(( int*) _temp684) == Cyc_Absyn_KnownTunionfield){ _LL712:
_temp711=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp684)->f1; _temp713=*
_temp711; _LL723: _temp722=( void*) _temp713.sc; goto _LL721; _LL721: _temp720=
_temp713.name; goto _LL719; _LL719: _temp718= _temp713.tvs; goto _LL717; _LL717:
_temp716= _temp713.fields; goto _LL715; _LL715: _temp714= _temp713.is_xtunion;
goto _LL699; _LL699: _temp698=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp684)->f2; _temp700=* _temp698; _LL710: _temp709= _temp700.name; goto _LL708;
_LL708: _temp707= _temp700.tvs; goto _LL706; _LL706: _temp705= _temp700.typs;
goto _LL704; _LL704: _temp703= _temp700.loc; goto _LL702; _LL702: _temp701=(
void*) _temp700.sc; goto _LL689;} else{ goto _LL685;} _LL687: _temp720= _temp696;
_temp714= _temp692; _temp709= _temp694; goto _LL689; _LL689: { struct Cyc_PP_Doc*
_temp728= Cyc_PP_text( _temp714?({ unsigned char* _temp724="xtunion "; struct
_tagged_string _temp725; _temp725.curr= _temp724; _temp725.base= _temp724;
_temp725.last_plus_one= _temp724 + 9; _temp725;}):({ unsigned char* _temp726="tunion ";
struct _tagged_string _temp727; _temp727.curr= _temp726; _temp727.base= _temp726;
_temp727.last_plus_one= _temp726 + 8; _temp727;})); goto _LL729; _LL729: s=({
struct Cyc_PP_Doc*(* _temp730)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp733= _temp728; struct Cyc_PP_Doc* _temp734= Cyc_Absynpp_qvar2doc( _temp720);
struct Cyc_PP_Doc* _temp735= Cyc_PP_text(({ unsigned char* _temp737="."; struct
_tagged_string _temp738; _temp738.curr= _temp737; _temp738.base= _temp737;
_temp738.last_plus_one= _temp737 + 2; _temp738;})); struct Cyc_PP_Doc* _temp736=
Cyc_PP_textptr((* _temp709).f2); struct Cyc_PP_Doc* _temp731[ 4u]={ _temp733,
_temp734, _temp735, _temp736}; struct _tagged_ptr0 _temp732={ _temp731, _temp731,
_temp731 + 4u}; _temp730( _temp732);}); goto _LL685;} _LL685:;} goto _LL480;
_LL498: { struct _tagged_string sns; struct _tagged_string ts;{ void* _temp739=
_temp559; _LL741: if( _temp739 ==( void*) Cyc_Absyn_Signed){ goto _LL742;} else{
goto _LL743;} _LL743: if( _temp739 ==( void*) Cyc_Absyn_Unsigned){ goto _LL744;}
else{ goto _LL740;} _LL742: sns=({ unsigned char* _temp745=""; struct
_tagged_string _temp746; _temp746.curr= _temp745; _temp746.base= _temp745;
_temp746.last_plus_one= _temp745 + 1; _temp746;}); goto _LL740; _LL744: sns=({
unsigned char* _temp747="unsigned "; struct _tagged_string _temp748; _temp748.curr=
_temp747; _temp748.base= _temp747; _temp748.last_plus_one= _temp747 + 10;
_temp748;}); goto _LL740; _LL740:;}{ void* _temp749= _temp557; _LL751: if(
_temp749 ==( void*) Cyc_Absyn_B1){ goto _LL752;} else{ goto _LL753;} _LL753: if(
_temp749 ==( void*) Cyc_Absyn_B2){ goto _LL754;} else{ goto _LL755;} _LL755: if(
_temp749 ==( void*) Cyc_Absyn_B4){ goto _LL756;} else{ goto _LL757;} _LL757: if(
_temp749 ==( void*) Cyc_Absyn_B8){ goto _LL758;} else{ goto _LL750;} _LL752:{
void* _temp759= _temp559; _LL761: if( _temp759 ==( void*) Cyc_Absyn_Signed){
goto _LL762;} else{ goto _LL763;} _LL763: if( _temp759 ==( void*) Cyc_Absyn_Unsigned){
goto _LL764;} else{ goto _LL760;} _LL762: sns=({ unsigned char* _temp765="signed ";
struct _tagged_string _temp766; _temp766.curr= _temp765; _temp766.base= _temp765;
_temp766.last_plus_one= _temp765 + 8; _temp766;}); goto _LL760; _LL764: goto
_LL760; _LL760:;} ts=({ unsigned char* _temp767="char"; struct _tagged_string
_temp768; _temp768.curr= _temp767; _temp768.base= _temp767; _temp768.last_plus_one=
_temp767 + 5; _temp768;}); goto _LL750; _LL754: ts=({ unsigned char* _temp769="short";
struct _tagged_string _temp770; _temp770.curr= _temp769; _temp770.base= _temp769;
_temp770.last_plus_one= _temp769 + 6; _temp770;}); goto _LL750; _LL756: ts=({
unsigned char* _temp771="int"; struct _tagged_string _temp772; _temp772.curr=
_temp771; _temp772.base= _temp771; _temp772.last_plus_one= _temp771 + 4;
_temp772;}); goto _LL750; _LL758: ts= Cyc_Absynpp_to_VC?({ unsigned char*
_temp773="__int64"; struct _tagged_string _temp774; _temp774.curr= _temp773;
_temp774.base= _temp773; _temp774.last_plus_one= _temp773 + 8; _temp774;}):({
unsigned char* _temp775="long long"; struct _tagged_string _temp776; _temp776.curr=
_temp775; _temp776.base= _temp775; _temp776.last_plus_one= _temp775 + 10;
_temp776;}); goto _LL750; _LL750:;} s= Cyc_PP_text(({ struct _tagged_string
_temp777= sns; struct _tagged_string _temp778= ts; xprintf("%.*s%.*s", _temp777.last_plus_one
- _temp777.curr, _temp777.curr, _temp778.last_plus_one - _temp778.curr, _temp778.curr);}));
goto _LL480;} _LL500: s= Cyc_PP_text(({ unsigned char* _temp779="float"; struct
_tagged_string _temp780; _temp780.curr= _temp779; _temp780.base= _temp779;
_temp780.last_plus_one= _temp779 + 6; _temp780;})); goto _LL480; _LL502: s= Cyc_PP_text(({
unsigned char* _temp781="double"; struct _tagged_string _temp782; _temp782.curr=
_temp781; _temp782.base= _temp781; _temp782.last_plus_one= _temp781 + 7;
_temp782;})); goto _LL480; _LL504: s=({ struct Cyc_PP_Doc*(* _temp783)( struct
_tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp786= Cyc_PP_text(({
unsigned char* _temp788="$"; struct _tagged_string _temp789; _temp789.curr=
_temp788; _temp789.base= _temp788; _temp789.last_plus_one= _temp788 + 2;
_temp789;})); struct Cyc_PP_Doc* _temp787= Cyc_Absynpp_args2doc( _temp561);
struct Cyc_PP_Doc* _temp784[ 2u]={ _temp786, _temp787}; struct _tagged_ptr0
_temp785={ _temp784, _temp784, _temp784 + 2u}; _temp783( _temp785);}); goto
_LL480; _LL506: if( _temp567 == 0){ s=({ struct Cyc_PP_Doc*(* _temp790)( struct
_tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp793= Cyc_PP_text(({
unsigned char* _temp795="struct "; struct _tagged_string _temp796; _temp796.curr=
_temp795; _temp796.base= _temp795; _temp796.last_plus_one= _temp795 + 8;
_temp796;})); struct Cyc_PP_Doc* _temp794= Cyc_Absynpp_tps2doc( _temp565);
struct Cyc_PP_Doc* _temp791[ 2u]={ _temp793, _temp794}; struct _tagged_ptr0
_temp792={ _temp791, _temp791, _temp791 + 2u}; _temp790( _temp792);});} else{ s=({
struct Cyc_PP_Doc*(* _temp797)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp800= Cyc_PP_text(({ unsigned char* _temp803="struct "; struct
_tagged_string _temp804; _temp804.curr= _temp803; _temp804.base= _temp803;
_temp804.last_plus_one= _temp803 + 8; _temp804;})); struct Cyc_PP_Doc* _temp801=
_temp567 == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc(( struct _tuple0*)
_check_null( _temp567)); struct Cyc_PP_Doc* _temp802= Cyc_Absynpp_tps2doc(
_temp565); struct Cyc_PP_Doc* _temp798[ 3u]={ _temp800, _temp801, _temp802};
struct _tagged_ptr0 _temp799={ _temp798, _temp798, _temp798 + 3u}; _temp797(
_temp799);});} goto _LL480; _LL508: if( _temp573 == 0){ s=({ struct Cyc_PP_Doc*(*
_temp805)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp808= Cyc_PP_text(({
unsigned char* _temp810="union "; struct _tagged_string _temp811; _temp811.curr=
_temp810; _temp811.base= _temp810; _temp811.last_plus_one= _temp810 + 7;
_temp811;})); struct Cyc_PP_Doc* _temp809= Cyc_Absynpp_tps2doc( _temp571);
struct Cyc_PP_Doc* _temp806[ 2u]={ _temp808, _temp809}; struct _tagged_ptr0
_temp807={ _temp806, _temp806, _temp806 + 2u}; _temp805( _temp807);});} else{ s=({
struct Cyc_PP_Doc*(* _temp812)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp815= Cyc_PP_text(({ unsigned char* _temp818="union "; struct _tagged_string
_temp819; _temp819.curr= _temp818; _temp819.base= _temp818; _temp819.last_plus_one=
_temp818 + 7; _temp819;})); struct Cyc_PP_Doc* _temp816= _temp573 == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_qvar2doc(( struct _tuple0*) _check_null( _temp573)); struct Cyc_PP_Doc*
_temp817= Cyc_Absynpp_tps2doc( _temp571); struct Cyc_PP_Doc* _temp813[ 3u]={
_temp815, _temp816, _temp817}; struct _tagged_ptr0 _temp814={ _temp813, _temp813,
_temp813 + 3u}; _temp812( _temp814);});} goto _LL480; _LL510: s=({ struct Cyc_PP_Doc*(*
_temp820)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp823= Cyc_PP_text(({
unsigned char* _temp828="struct {"; struct _tagged_string _temp829; _temp829.curr=
_temp828; _temp829.base= _temp828; _temp829.last_plus_one= _temp828 + 9;
_temp829;})); struct Cyc_PP_Doc* _temp824= Cyc_PP_nest( 2, Cyc_Absynpp_structfields2doc(
_temp575)); struct Cyc_PP_Doc* _temp825= Cyc_PP_text(({ unsigned char* _temp826="}";
struct _tagged_string _temp827; _temp827.curr= _temp826; _temp827.base= _temp826;
_temp827.last_plus_one= _temp826 + 2; _temp827;})); struct Cyc_PP_Doc* _temp821[
3u]={ _temp823, _temp824, _temp825}; struct _tagged_ptr0 _temp822={ _temp821,
_temp821, _temp821 + 3u}; _temp820( _temp822);}); goto _LL480; _LL512: s=({
struct Cyc_PP_Doc*(* _temp830)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp833= Cyc_PP_text(({ unsigned char* _temp838="union {"; struct
_tagged_string _temp839; _temp839.curr= _temp838; _temp839.base= _temp838;
_temp839.last_plus_one= _temp838 + 8; _temp839;})); struct Cyc_PP_Doc* _temp834=
Cyc_PP_nest( 2, Cyc_Absynpp_structfields2doc( _temp577)); struct Cyc_PP_Doc*
_temp835= Cyc_PP_text(({ unsigned char* _temp836="}"; struct _tagged_string
_temp837; _temp837.curr= _temp836; _temp837.base= _temp836; _temp837.last_plus_one=
_temp836 + 2; _temp837;})); struct Cyc_PP_Doc* _temp831[ 3u]={ _temp833,
_temp834, _temp835}; struct _tagged_ptr0 _temp832={ _temp831, _temp831, _temp831
+ 3u}; _temp830( _temp832);}); goto _LL480; _LL514: s=({ struct Cyc_PP_Doc*(*
_temp840)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp843= Cyc_PP_text(({
unsigned char* _temp845="enum "; struct _tagged_string _temp846; _temp846.curr=
_temp845; _temp846.base= _temp845; _temp846.last_plus_one= _temp845 + 6;
_temp846;})); struct Cyc_PP_Doc* _temp844= Cyc_Absynpp_qvar2doc( _temp581);
struct Cyc_PP_Doc* _temp841[ 2u]={ _temp843, _temp844}; struct _tagged_ptr0
_temp842={ _temp841, _temp841, _temp841 + 2u}; _temp840( _temp842);}); goto
_LL480; _LL516: s=({ struct Cyc_PP_Doc*(* _temp847)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp850= Cyc_Absynpp_qvar2doc( _temp587); struct Cyc_PP_Doc*
_temp851= Cyc_Absynpp_tps2doc( _temp585); struct Cyc_PP_Doc* _temp848[ 2u]={
_temp850, _temp851}; struct _tagged_ptr0 _temp849={ _temp848, _temp848, _temp848
+ 2u}; _temp847( _temp849);}); goto _LL480; _LL518: s=({ struct Cyc_PP_Doc*(*
_temp852)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp855= Cyc_PP_text(({
unsigned char* _temp860="region_t<"; struct _tagged_string _temp861; _temp861.curr=
_temp860; _temp861.base= _temp860; _temp861.last_plus_one= _temp860 + 10;
_temp861;})); struct Cyc_PP_Doc* _temp856= Cyc_Absynpp_rgn2doc( _temp589);
struct Cyc_PP_Doc* _temp857= Cyc_PP_text(({ unsigned char* _temp858=">"; struct
_tagged_string _temp859; _temp859.curr= _temp858; _temp859.base= _temp858;
_temp859.last_plus_one= _temp858 + 2; _temp859;})); struct Cyc_PP_Doc* _temp853[
3u]={ _temp855, _temp856, _temp857}; struct _tagged_ptr0 _temp854={ _temp853,
_temp853, _temp853 + 3u}; _temp852( _temp854);}); goto _LL480; _LL520: s= Cyc_Absynpp_rgn2doc(
t); goto _LL480; _LL522: s= Cyc_Absynpp_eff2doc( t); goto _LL480; _LL524: s= Cyc_Absynpp_eff2doc(
t); goto _LL480; _LL480:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_vo2doc(
struct Cyc_Core_Opt* vo){ return vo == 0? Cyc_PP_nil_doc(): Cyc_PP_text(*((
struct _tagged_string*)(( struct Cyc_Core_Opt*) _check_null( vo))->v));} struct
Cyc_PP_Doc* Cyc_Absynpp_funarg2doc( struct _tuple1* t){ struct Cyc_Core_Opt*
dopt=(* t).f1 == 0? 0:({ struct Cyc_Core_Opt* _temp862=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp862->v=( void*) Cyc_PP_text(*((
struct _tagged_string*)(( struct Cyc_Core_Opt*) _check_null((* t).f1))->v));
_temp862;}); return Cyc_Absynpp_tqtd2doc((* t).f2,(* t).f3, dopt);} struct Cyc_PP_Doc*
Cyc_Absynpp_funargs2doc( struct Cyc_List_List* args, int c_varargs, struct Cyc_Absyn_VarargInfo*
cyc_varargs, struct Cyc_Core_Opt* effopt){ struct Cyc_List_List* _temp863=((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple1*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_funarg2doc, args); goto _LL864; _LL864: if(
effopt == 0){ if( c_varargs){ return Cyc_PP_group(({ unsigned char* _temp865="(";
struct _tagged_string _temp866; _temp866.curr= _temp865; _temp866.base= _temp865;
_temp866.last_plus_one= _temp865 + 2; _temp866;}),({ unsigned char* _temp867=")";
struct _tagged_string _temp868; _temp868.curr= _temp867; _temp868.base= _temp867;
_temp868.last_plus_one= _temp867 + 2; _temp868;}),({ unsigned char* _temp869=",";
struct _tagged_string _temp870; _temp870.curr= _temp869; _temp870.base= _temp869;
_temp870.last_plus_one= _temp869 + 2; _temp870;}),(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp863,({
struct Cyc_List_List* _temp871=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp871->hd=( void*) Cyc_PP_text(({ unsigned char*
_temp872="..."; struct _tagged_string _temp873; _temp873.curr= _temp872;
_temp873.base= _temp872; _temp873.last_plus_one= _temp872 + 4; _temp873;}));
_temp871->tl= 0; _temp871;})));} else{ if( cyc_varargs != 0){ struct Cyc_PP_Doc*
_temp875= Cyc_Absynpp_funarg2doc(({ struct _tuple1* _temp874=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp874->f1=(( struct Cyc_Absyn_VarargInfo*)
_check_null( cyc_varargs))->name; _temp874->f2=(( struct Cyc_Absyn_VarargInfo*)
_check_null( cyc_varargs))->tq; _temp874->f3=( void*)(( struct Cyc_Absyn_VarargInfo*)
_check_null( cyc_varargs))->type; _temp874;})); goto _LL876; _LL876: { struct
Cyc_PP_Doc* _temp890=({ struct Cyc_PP_Doc*(* _temp877)( struct _tagged_ptr0)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp880= Cyc_PP_text(({ unsigned char* _temp888="...";
struct _tagged_string _temp889; _temp889.curr= _temp888; _temp889.base= _temp888;
_temp889.last_plus_one= _temp888 + 4; _temp889;})); struct Cyc_PP_Doc* _temp881=
Cyc_Absynpp_typ2doc(( void*)(( struct Cyc_Absyn_VarargInfo*) _check_null(
cyc_varargs))->rgn); struct Cyc_PP_Doc* _temp882=(( struct Cyc_Absyn_VarargInfo*)
_check_null( cyc_varargs))->inject? Cyc_PP_text(({ unsigned char* _temp884=" inject ";
struct _tagged_string _temp885; _temp885.curr= _temp884; _temp885.base= _temp884;
_temp885.last_plus_one= _temp884 + 9; _temp885;})): Cyc_PP_text(({ unsigned char*
_temp886=""; struct _tagged_string _temp887; _temp887.curr= _temp886; _temp887.base=
_temp886; _temp887.last_plus_one= _temp886 + 1; _temp887;})); struct Cyc_PP_Doc*
_temp883= _temp875; struct Cyc_PP_Doc* _temp878[ 4u]={ _temp880, _temp881,
_temp882, _temp883}; struct _tagged_ptr0 _temp879={ _temp878, _temp878, _temp878
+ 4u}; _temp877( _temp879);}); goto _LL891; _LL891: return Cyc_PP_group(({
unsigned char* _temp892="("; struct _tagged_string _temp893; _temp893.curr=
_temp892; _temp893.base= _temp892; _temp893.last_plus_one= _temp892 + 2;
_temp893;}),({ unsigned char* _temp894=")"; struct _tagged_string _temp895;
_temp895.curr= _temp894; _temp895.base= _temp894; _temp895.last_plus_one=
_temp894 + 2; _temp895;}),({ unsigned char* _temp896=","; struct _tagged_string
_temp897; _temp897.curr= _temp896; _temp897.base= _temp896; _temp897.last_plus_one=
_temp896 + 2; _temp897;}),(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( _temp863,({ struct Cyc_List_List*
_temp898=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp898->hd=( void*) _temp890; _temp898->tl= 0; _temp898;})));}} else{ return
Cyc_PP_group(({ unsigned char* _temp899="("; struct _tagged_string _temp900;
_temp900.curr= _temp899; _temp900.base= _temp899; _temp900.last_plus_one=
_temp899 + 2; _temp900;}),({ unsigned char* _temp901=")"; struct _tagged_string
_temp902; _temp902.curr= _temp901; _temp902.base= _temp901; _temp902.last_plus_one=
_temp901 + 2; _temp902;}),({ unsigned char* _temp903=","; struct _tagged_string
_temp904; _temp904.curr= _temp903; _temp904.base= _temp903; _temp904.last_plus_one=
_temp903 + 2; _temp904;}), _temp863);}}} else{ if( c_varargs){ return Cyc_PP_group(({
unsigned char* _temp905="("; struct _tagged_string _temp906; _temp906.curr=
_temp905; _temp906.base= _temp905; _temp906.last_plus_one= _temp905 + 2;
_temp906;}),({ unsigned char* _temp907=")"; struct _tagged_string _temp908;
_temp908.curr= _temp907; _temp908.base= _temp907; _temp908.last_plus_one=
_temp907 + 2; _temp908;}),({ unsigned char* _temp909=";"; struct _tagged_string
_temp910; _temp910.curr= _temp909; _temp910.base= _temp909; _temp910.last_plus_one=
_temp909 + 2; _temp910;}),({ struct Cyc_List_List* _temp911=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp911->hd=( void*) Cyc_PP_group(({
unsigned char* _temp913=""; struct _tagged_string _temp914; _temp914.curr=
_temp913; _temp914.base= _temp913; _temp914.last_plus_one= _temp913 + 1;
_temp914;}),({ unsigned char* _temp915=""; struct _tagged_string _temp916;
_temp916.curr= _temp915; _temp916.base= _temp915; _temp916.last_plus_one=
_temp915 + 1; _temp916;}),({ unsigned char* _temp917=","; struct _tagged_string
_temp918; _temp918.curr= _temp917; _temp918.base= _temp917; _temp918.last_plus_one=
_temp917 + 2; _temp918;}),(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( _temp863,({ struct Cyc_List_List*
_temp919=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp919->hd=( void*) Cyc_PP_text(({ unsigned char* _temp920="..."; struct
_tagged_string _temp921; _temp921.curr= _temp920; _temp921.base= _temp920;
_temp921.last_plus_one= _temp920 + 4; _temp921;})); _temp919->tl= 0; _temp919;})));
_temp911->tl=({ struct Cyc_List_List* _temp912=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp912->hd=( void*) Cyc_Absynpp_eff2doc((
void*)(( struct Cyc_Core_Opt*) _check_null( effopt))->v); _temp912->tl= 0;
_temp912;}); _temp911;}));} else{ if( cyc_varargs != 0){ struct Cyc_PP_Doc*
_temp923= Cyc_Absynpp_funarg2doc(({ struct _tuple1* _temp922=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp922->f1=(( struct Cyc_Absyn_VarargInfo*)
_check_null( cyc_varargs))->name; _temp922->f2=(( struct Cyc_Absyn_VarargInfo*)
_check_null( cyc_varargs))->tq; _temp922->f3=( void*)(( struct Cyc_Absyn_VarargInfo*)
_check_null( cyc_varargs))->type; _temp922;})); goto _LL924; _LL924: { struct
Cyc_PP_Doc* _temp938=({ struct Cyc_PP_Doc*(* _temp925)( struct _tagged_ptr0)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp928= Cyc_PP_text(({ unsigned char* _temp936="...";
struct _tagged_string _temp937; _temp937.curr= _temp936; _temp937.base= _temp936;
_temp937.last_plus_one= _temp936 + 4; _temp937;})); struct Cyc_PP_Doc* _temp929=
Cyc_Absynpp_typ2doc(( void*)(( struct Cyc_Absyn_VarargInfo*) _check_null(
cyc_varargs))->rgn); struct Cyc_PP_Doc* _temp930=(( struct Cyc_Absyn_VarargInfo*)
_check_null( cyc_varargs))->inject? Cyc_PP_text(({ unsigned char* _temp932=" inject ";
struct _tagged_string _temp933; _temp933.curr= _temp932; _temp933.base= _temp932;
_temp933.last_plus_one= _temp932 + 9; _temp933;})): Cyc_PP_text(({ unsigned char*
_temp934=""; struct _tagged_string _temp935; _temp935.curr= _temp934; _temp935.base=
_temp934; _temp935.last_plus_one= _temp934 + 1; _temp935;})); struct Cyc_PP_Doc*
_temp931= _temp923; struct Cyc_PP_Doc* _temp926[ 4u]={ _temp928, _temp929,
_temp930, _temp931}; struct _tagged_ptr0 _temp927={ _temp926, _temp926, _temp926
+ 4u}; _temp925( _temp927);}); goto _LL939; _LL939: return Cyc_PP_group(({
unsigned char* _temp940="("; struct _tagged_string _temp941; _temp941.curr=
_temp940; _temp941.base= _temp940; _temp941.last_plus_one= _temp940 + 2;
_temp941;}),({ unsigned char* _temp942=")"; struct _tagged_string _temp943;
_temp943.curr= _temp942; _temp943.base= _temp942; _temp943.last_plus_one=
_temp942 + 2; _temp943;}),({ unsigned char* _temp944=";"; struct _tagged_string
_temp945; _temp945.curr= _temp944; _temp945.base= _temp944; _temp945.last_plus_one=
_temp944 + 2; _temp945;}),({ struct Cyc_List_List* _temp946=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp946->hd=( void*) Cyc_PP_group(({
unsigned char* _temp948=""; struct _tagged_string _temp949; _temp949.curr=
_temp948; _temp949.base= _temp948; _temp949.last_plus_one= _temp948 + 1;
_temp949;}),({ unsigned char* _temp950=""; struct _tagged_string _temp951;
_temp951.curr= _temp950; _temp951.base= _temp950; _temp951.last_plus_one=
_temp950 + 1; _temp951;}),({ unsigned char* _temp952=","; struct _tagged_string
_temp953; _temp953.curr= _temp952; _temp953.base= _temp952; _temp953.last_plus_one=
_temp952 + 2; _temp953;}),(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( _temp863,({ struct Cyc_List_List*
_temp954=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp954->hd=( void*) _temp938; _temp954->tl= 0; _temp954;}))); _temp946->tl=({
struct Cyc_List_List* _temp947=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp947->hd=( void*) Cyc_Absynpp_eff2doc(( void*)((
struct Cyc_Core_Opt*) _check_null( effopt))->v); _temp947->tl= 0; _temp947;});
_temp946;}));}} else{ return Cyc_PP_group(({ unsigned char* _temp955="("; struct
_tagged_string _temp956; _temp956.curr= _temp955; _temp956.base= _temp955;
_temp956.last_plus_one= _temp955 + 2; _temp956;}),({ unsigned char* _temp957=")";
struct _tagged_string _temp958; _temp958.curr= _temp957; _temp958.base= _temp957;
_temp958.last_plus_one= _temp957 + 2; _temp958;}),({ unsigned char* _temp959=";";
struct _tagged_string _temp960; _temp960.curr= _temp959; _temp960.base= _temp959;
_temp960.last_plus_one= _temp959 + 2; _temp960;}),({ struct Cyc_List_List*
_temp961=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp961->hd=( void*) Cyc_PP_group(({ unsigned char* _temp963=""; struct
_tagged_string _temp964; _temp964.curr= _temp963; _temp964.base= _temp963;
_temp964.last_plus_one= _temp963 + 1; _temp964;}),({ unsigned char* _temp965="";
struct _tagged_string _temp966; _temp966.curr= _temp965; _temp966.base= _temp965;
_temp966.last_plus_one= _temp965 + 1; _temp966;}),({ unsigned char* _temp967=",";
struct _tagged_string _temp968; _temp968.curr= _temp967; _temp968.base= _temp967;
_temp968.last_plus_one= _temp967 + 2; _temp968;}), _temp863); _temp961->tl=({
struct Cyc_List_List* _temp962=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp962->hd=( void*) Cyc_Absynpp_eff2doc(( void*)((
struct Cyc_Core_Opt*) _check_null( effopt))->v); _temp962->tl= 0; _temp962;});
_temp961;}));}}}} struct _tuple1* Cyc_Absynpp_arg_mk_opt( struct _tuple3* arg){
return({ struct _tuple1* _temp969=( struct _tuple1*) GC_malloc( sizeof( struct
_tuple1)); _temp969->f1=({ struct Cyc_Core_Opt* _temp970=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp970->v=( void*)(* arg).f1;
_temp970;}); _temp969->f2=(* arg).f2; _temp969->f3=(* arg).f3; _temp969;});}
struct Cyc_PP_Doc* Cyc_Absynpp_var2doc( struct _tagged_string* v){ return Cyc_PP_text(*
v);} struct Cyc_PP_Doc* Cyc_Absynpp_qvar2doc( struct _tuple0* q){ struct Cyc_List_List*
_temp971= 0; goto _LL972; _LL972:{ void* _temp973=(* q).f1; struct Cyc_List_List*
_temp985; struct Cyc_List_List* _temp987; struct Cyc_List_List* _temp989; struct
Cyc_List_List* _temp991; _LL975: if( _temp973 ==( void*) Cyc_Absyn_Loc_n){ goto
_LL976;} else{ goto _LL977;} _LL977: if(( unsigned int) _temp973 > 1u?*(( int*)
_temp973) == Cyc_Absyn_Rel_n: 0){ _LL986: _temp985=(( struct Cyc_Absyn_Rel_n_struct*)
_temp973)->f1; if( _temp985 == 0){ goto _LL978;} else{ goto _LL979;}} else{ goto
_LL979;} _LL979: if(( unsigned int) _temp973 > 1u?*(( int*) _temp973) == Cyc_Absyn_Abs_n:
0){ _LL988: _temp987=(( struct Cyc_Absyn_Abs_n_struct*) _temp973)->f1; if(
_temp987 == 0){ goto _LL980;} else{ goto _LL981;}} else{ goto _LL981;} _LL981:
if(( unsigned int) _temp973 > 1u?*(( int*) _temp973) == Cyc_Absyn_Rel_n: 0){
_LL990: _temp989=(( struct Cyc_Absyn_Rel_n_struct*) _temp973)->f1; goto _LL982;}
else{ goto _LL983;} _LL983: if(( unsigned int) _temp973 > 1u?*(( int*) _temp973)
== Cyc_Absyn_Abs_n: 0){ _LL992: _temp991=(( struct Cyc_Absyn_Abs_n_struct*)
_temp973)->f1; goto _LL984;} else{ goto _LL974;} _LL976: goto _LL978; _LL978:
goto _LL980; _LL980: return Cyc_Absynpp_var2doc((* q).f2); _LL982: _temp991=
_temp989; goto _LL984; _LL984: _temp971= _temp991; goto _LL974; _LL974:;} if(
Cyc_Absynpp_qvar_to_Cids){ return Cyc_PP_text( Cyc_String_str_sepstr((( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp971,({ struct Cyc_List_List* _temp993=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp993->hd=( void*)(* q).f2; _temp993->tl= 0;
_temp993;})),({ unsigned char* _temp994="_"; struct _tagged_string _temp995;
_temp995.curr= _temp994; _temp995.base= _temp994; _temp995.last_plus_one=
_temp994 + 2; _temp995;})));} else{ if((( int(*)( int(* cmp)( struct
_tagged_string*, struct _tagged_string*), struct Cyc_List_List* l1, struct Cyc_List_List*
l2)) Cyc_List_list_prefix)( Cyc_String_strptrcmp, _temp971, Cyc_Absynpp_curr_namespace)){
return Cyc_Absynpp_var2doc((* q).f2);} else{ return Cyc_PP_text( Cyc_String_str_sepstr(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp971,({ struct Cyc_List_List* _temp996=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp996->hd=( void*)(* q).f2; _temp996->tl= 0;
_temp996;})),({ unsigned char* _temp997="::"; struct _tagged_string _temp998;
_temp998.curr= _temp997; _temp998.base= _temp997; _temp998.last_plus_one=
_temp997 + 3; _temp998;})));}}} struct Cyc_PP_Doc* Cyc_Absynpp_typedef_name2doc(
struct _tuple0* v){ if( Cyc_Absynpp_qvar_to_Cids){ return Cyc_Absynpp_qvar2doc(
v);}{ void* _temp999=(* v).f1; struct Cyc_List_List* _temp1005; _LL1001: if((
unsigned int) _temp999 > 1u?*(( int*) _temp999) == Cyc_Absyn_Abs_n: 0){ _LL1006:
_temp1005=(( struct Cyc_Absyn_Abs_n_struct*) _temp999)->f1; goto _LL1002;} else{
goto _LL1003;} _LL1003: goto _LL1004; _LL1002: if((( int(*)( int(* cmp)( struct
_tagged_string*, struct _tagged_string*), struct Cyc_List_List* l1, struct Cyc_List_List*
l2)) Cyc_List_list_cmp)( Cyc_String_strptrcmp, _temp1005, Cyc_Absynpp_curr_namespace)
== 0){ return Cyc_Absynpp_var2doc((* v).f2);} else{ return({ struct Cyc_PP_Doc*(*
_temp1007)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1010= Cyc_PP_text(({
unsigned char* _temp1012="/* wrong namespace : */ "; struct _tagged_string
_temp1013; _temp1013.curr= _temp1012; _temp1013.base= _temp1012; _temp1013.last_plus_one=
_temp1012 + 25; _temp1013;})); struct Cyc_PP_Doc* _temp1011= Cyc_Absynpp_qvar2doc(
v); struct Cyc_PP_Doc* _temp1008[ 2u]={ _temp1010, _temp1011}; struct
_tagged_ptr0 _temp1009={ _temp1008, _temp1008, _temp1008 + 2u}; _temp1007(
_temp1009);});} _LL1004: return Cyc_Absynpp_qvar2doc( v); _LL1000:;}} struct Cyc_PP_Doc*
Cyc_Absynpp_typ2doc( void* t){ return Cyc_Absynpp_tqtd2doc( Cyc_Absyn_empty_tqual(),
t, 0);} int Cyc_Absynpp_exp_prec( struct Cyc_Absyn_Exp* e){ void* _temp1014=(
void*) e->r; void* _temp1086; void* _temp1088; struct _tuple0* _temp1090; struct
_tuple0* _temp1092; struct Cyc_List_List* _temp1094; void* _temp1096; struct Cyc_Absyn_Exp*
_temp1098; struct Cyc_Core_Opt* _temp1100; struct Cyc_Absyn_Exp* _temp1102; void*
_temp1104; struct Cyc_Absyn_Exp* _temp1106; struct Cyc_Absyn_Exp* _temp1108;
struct Cyc_Absyn_Exp* _temp1110; struct Cyc_Absyn_Exp* _temp1112; struct Cyc_Absyn_Exp*
_temp1114; struct Cyc_Absyn_Exp* _temp1116; struct Cyc_List_List* _temp1118;
struct Cyc_Absyn_Exp* _temp1120; struct Cyc_Absyn_VarargCallInfo* _temp1122;
struct Cyc_List_List* _temp1124; struct Cyc_Absyn_Exp* _temp1126; struct Cyc_Absyn_Exp*
_temp1128; struct Cyc_Absyn_Exp* _temp1130; struct Cyc_List_List* _temp1132;
struct Cyc_Absyn_Exp* _temp1134; struct Cyc_Absyn_Exp* _temp1136; void*
_temp1138; struct Cyc_Absyn_Exp* _temp1140; struct Cyc_Absyn_Exp* _temp1142;
struct Cyc_Absyn_Exp* _temp1144; void* _temp1146; struct Cyc_Absyn_Exp*
_temp1148; struct Cyc_Absyn_Exp* _temp1150; struct _tagged_string* _temp1152;
struct Cyc_Absyn_Exp* _temp1154; struct _tagged_string* _temp1156; struct Cyc_Absyn_Exp*
_temp1158; struct Cyc_Absyn_Exp* _temp1160; struct Cyc_Absyn_Exp* _temp1162;
struct Cyc_List_List* _temp1164; struct Cyc_List_List* _temp1166; struct _tuple1*
_temp1168; struct Cyc_List_List* _temp1170; struct Cyc_Absyn_Exp* _temp1172;
struct Cyc_Absyn_Exp* _temp1174; struct Cyc_Absyn_Vardecl* _temp1176; struct Cyc_Absyn_Structdecl*
_temp1178; struct Cyc_List_List* _temp1180; struct Cyc_Core_Opt* _temp1182;
struct _tuple0* _temp1184; struct Cyc_List_List* _temp1186; void* _temp1188;
struct Cyc_Absyn_Tunionfield* _temp1190; struct Cyc_Absyn_Tuniondecl* _temp1192;
struct Cyc_List_List* _temp1194; struct Cyc_Core_Opt* _temp1196; struct Cyc_Core_Opt*
_temp1198; struct Cyc_Absyn_Enumfield* _temp1200; struct Cyc_Absyn_Enumdecl*
_temp1202; struct _tuple0* _temp1204; void* _temp1206; struct Cyc_Absyn_Exp*
_temp1208; struct Cyc_List_List* _temp1210; struct Cyc_Core_Opt* _temp1212;
struct Cyc_Absyn_Stmt* _temp1214; struct Cyc_Absyn_Fndecl* _temp1216; struct Cyc_Absyn_Exp*
_temp1218; _LL1016: if(*(( int*) _temp1014) == Cyc_Absyn_Const_e){ _LL1087:
_temp1086=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1014)->f1; goto
_LL1017;} else{ goto _LL1018;} _LL1018: if(*(( int*) _temp1014) == Cyc_Absyn_Var_e){
_LL1091: _temp1090=(( struct Cyc_Absyn_Var_e_struct*) _temp1014)->f1; goto
_LL1089; _LL1089: _temp1088=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1014)->f2;
goto _LL1019;} else{ goto _LL1020;} _LL1020: if(*(( int*) _temp1014) == Cyc_Absyn_UnknownId_e){
_LL1093: _temp1092=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp1014)->f1; goto
_LL1021;} else{ goto _LL1022;} _LL1022: if(*(( int*) _temp1014) == Cyc_Absyn_Primop_e){
_LL1097: _temp1096=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1014)->f1;
goto _LL1095; _LL1095: _temp1094=(( struct Cyc_Absyn_Primop_e_struct*) _temp1014)->f2;
goto _LL1023;} else{ goto _LL1024;} _LL1024: if(*(( int*) _temp1014) == Cyc_Absyn_AssignOp_e){
_LL1103: _temp1102=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp1014)->f1; goto
_LL1101; _LL1101: _temp1100=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp1014)->f2;
goto _LL1099; _LL1099: _temp1098=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1014)->f3; goto _LL1025;} else{ goto _LL1026;} _LL1026: if(*(( int*)
_temp1014) == Cyc_Absyn_Increment_e){ _LL1107: _temp1106=(( struct Cyc_Absyn_Increment_e_struct*)
_temp1014)->f1; goto _LL1105; _LL1105: _temp1104=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1014)->f2; goto _LL1027;} else{ goto _LL1028;} _LL1028: if(*(( int*)
_temp1014) == Cyc_Absyn_Conditional_e){ _LL1113: _temp1112=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1014)->f1; goto _LL1111; _LL1111: _temp1110=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1014)->f2; goto _LL1109; _LL1109: _temp1108=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1014)->f3; goto _LL1029;} else{ goto _LL1030;} _LL1030: if(*(( int*)
_temp1014) == Cyc_Absyn_SeqExp_e){ _LL1117: _temp1116=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1014)->f1; goto _LL1115; _LL1115: _temp1114=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1014)->f2; goto _LL1031;} else{ goto _LL1032;} _LL1032: if(*(( int*)
_temp1014) == Cyc_Absyn_UnknownCall_e){ _LL1121: _temp1120=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1014)->f1; goto _LL1119; _LL1119: _temp1118=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1014)->f2; goto _LL1033;} else{ goto _LL1034;} _LL1034: if(*(( int*)
_temp1014) == Cyc_Absyn_FnCall_e){ _LL1127: _temp1126=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1014)->f1; goto _LL1125; _LL1125: _temp1124=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1014)->f2; goto _LL1123; _LL1123: _temp1122=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1014)->f3; goto _LL1035;} else{ goto _LL1036;} _LL1036: if(*(( int*)
_temp1014) == Cyc_Absyn_Throw_e){ _LL1129: _temp1128=(( struct Cyc_Absyn_Throw_e_struct*)
_temp1014)->f1; goto _LL1037;} else{ goto _LL1038;} _LL1038: if(*(( int*)
_temp1014) == Cyc_Absyn_NoInstantiate_e){ _LL1131: _temp1130=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1014)->f1; goto _LL1039;} else{ goto _LL1040;} _LL1040: if(*(( int*)
_temp1014) == Cyc_Absyn_Instantiate_e){ _LL1135: _temp1134=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1014)->f1; goto _LL1133; _LL1133: _temp1132=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1014)->f2; goto _LL1041;} else{ goto _LL1042;} _LL1042: if(*(( int*)
_temp1014) == Cyc_Absyn_Cast_e){ _LL1139: _temp1138=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1014)->f1; goto _LL1137; _LL1137: _temp1136=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1014)->f2; goto _LL1043;} else{ goto _LL1044;} _LL1044: if(*(( int*)
_temp1014) == Cyc_Absyn_New_e){ _LL1143: _temp1142=(( struct Cyc_Absyn_New_e_struct*)
_temp1014)->f1; goto _LL1141; _LL1141: _temp1140=(( struct Cyc_Absyn_New_e_struct*)
_temp1014)->f2; goto _LL1045;} else{ goto _LL1046;} _LL1046: if(*(( int*)
_temp1014) == Cyc_Absyn_Address_e){ _LL1145: _temp1144=(( struct Cyc_Absyn_Address_e_struct*)
_temp1014)->f1; goto _LL1047;} else{ goto _LL1048;} _LL1048: if(*(( int*)
_temp1014) == Cyc_Absyn_Sizeoftyp_e){ _LL1147: _temp1146=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp1014)->f1; goto _LL1049;} else{ goto _LL1050;} _LL1050: if(*(( int*)
_temp1014) == Cyc_Absyn_Sizeofexp_e){ _LL1149: _temp1148=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1014)->f1; goto _LL1051;} else{ goto _LL1052;} _LL1052: if(*(( int*)
_temp1014) == Cyc_Absyn_Deref_e){ _LL1151: _temp1150=(( struct Cyc_Absyn_Deref_e_struct*)
_temp1014)->f1; goto _LL1053;} else{ goto _LL1054;} _LL1054: if(*(( int*)
_temp1014) == Cyc_Absyn_StructMember_e){ _LL1155: _temp1154=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1014)->f1; goto _LL1153; _LL1153: _temp1152=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1014)->f2; goto _LL1055;} else{ goto _LL1056;} _LL1056: if(*(( int*)
_temp1014) == Cyc_Absyn_StructArrow_e){ _LL1159: _temp1158=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1014)->f1; goto _LL1157; _LL1157: _temp1156=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1014)->f2; goto _LL1057;} else{ goto _LL1058;} _LL1058: if(*(( int*)
_temp1014) == Cyc_Absyn_Subscript_e){ _LL1163: _temp1162=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1014)->f1; goto _LL1161; _LL1161: _temp1160=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1014)->f2; goto _LL1059;} else{ goto _LL1060;} _LL1060: if(*(( int*)
_temp1014) == Cyc_Absyn_Tuple_e){ _LL1165: _temp1164=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1014)->f1; goto _LL1061;} else{ goto _LL1062;} _LL1062: if(*(( int*)
_temp1014) == Cyc_Absyn_CompoundLit_e){ _LL1169: _temp1168=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1014)->f1; goto _LL1167; _LL1167: _temp1166=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1014)->f2; goto _LL1063;} else{ goto _LL1064;} _LL1064: if(*(( int*)
_temp1014) == Cyc_Absyn_Array_e){ _LL1171: _temp1170=(( struct Cyc_Absyn_Array_e_struct*)
_temp1014)->f1; goto _LL1065;} else{ goto _LL1066;} _LL1066: if(*(( int*)
_temp1014) == Cyc_Absyn_Comprehension_e){ _LL1177: _temp1176=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1014)->f1; goto _LL1175; _LL1175: _temp1174=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1014)->f2; goto _LL1173; _LL1173: _temp1172=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1014)->f3; goto _LL1067;} else{ goto _LL1068;} _LL1068: if(*(( int*)
_temp1014) == Cyc_Absyn_Struct_e){ _LL1185: _temp1184=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1014)->f1; goto _LL1183; _LL1183: _temp1182=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1014)->f2; goto _LL1181; _LL1181: _temp1180=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1014)->f3; goto _LL1179; _LL1179: _temp1178=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1014)->f4; goto _LL1069;} else{ goto _LL1070;} _LL1070: if(*(( int*)
_temp1014) == Cyc_Absyn_AnonStruct_e){ _LL1189: _temp1188=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1014)->f1; goto _LL1187; _LL1187: _temp1186=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1014)->f2; goto _LL1071;} else{ goto _LL1072;} _LL1072: if(*(( int*)
_temp1014) == Cyc_Absyn_Tunion_e){ _LL1199: _temp1198=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1014)->f1; goto _LL1197; _LL1197: _temp1196=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1014)->f2; goto _LL1195; _LL1195: _temp1194=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1014)->f3; goto _LL1193; _LL1193: _temp1192=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1014)->f4; goto _LL1191; _LL1191: _temp1190=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1014)->f5; goto _LL1073;} else{ goto _LL1074;} _LL1074: if(*(( int*)
_temp1014) == Cyc_Absyn_Enum_e){ _LL1205: _temp1204=(( struct Cyc_Absyn_Enum_e_struct*)
_temp1014)->f1; goto _LL1203; _LL1203: _temp1202=(( struct Cyc_Absyn_Enum_e_struct*)
_temp1014)->f2; goto _LL1201; _LL1201: _temp1200=(( struct Cyc_Absyn_Enum_e_struct*)
_temp1014)->f3; goto _LL1075;} else{ goto _LL1076;} _LL1076: if(*(( int*)
_temp1014) == Cyc_Absyn_Malloc_e){ _LL1209: _temp1208=(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1014)->f1; goto _LL1207; _LL1207: _temp1206=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1014)->f2; goto _LL1077;} else{ goto _LL1078;} _LL1078: if(*(( int*)
_temp1014) == Cyc_Absyn_UnresolvedMem_e){ _LL1213: _temp1212=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1014)->f1; goto _LL1211; _LL1211: _temp1210=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1014)->f2; goto _LL1079;} else{ goto _LL1080;} _LL1080: if(*(( int*)
_temp1014) == Cyc_Absyn_StmtExp_e){ _LL1215: _temp1214=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1014)->f1; goto _LL1081;} else{ goto _LL1082;} _LL1082: if(*(( int*)
_temp1014) == Cyc_Absyn_Codegen_e){ _LL1217: _temp1216=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1014)->f1; goto _LL1083;} else{ goto _LL1084;} _LL1084: if(*(( int*)
_temp1014) == Cyc_Absyn_Fill_e){ _LL1219: _temp1218=(( struct Cyc_Absyn_Fill_e_struct*)
_temp1014)->f1; goto _LL1085;} else{ goto _LL1015;} _LL1017: return 10000;
_LL1019: return 10000; _LL1021: return 10000; _LL1023: { void* _temp1220=
_temp1096; _LL1222: if( _temp1220 ==( void*) Cyc_Absyn_Plus){ goto _LL1223;}
else{ goto _LL1224;} _LL1224: if( _temp1220 ==( void*) Cyc_Absyn_Times){ goto
_LL1225;} else{ goto _LL1226;} _LL1226: if( _temp1220 ==( void*) Cyc_Absyn_Minus){
goto _LL1227;} else{ goto _LL1228;} _LL1228: if( _temp1220 ==( void*) Cyc_Absyn_Div){
goto _LL1229;} else{ goto _LL1230;} _LL1230: if( _temp1220 ==( void*) Cyc_Absyn_Mod){
goto _LL1231;} else{ goto _LL1232;} _LL1232: if( _temp1220 ==( void*) Cyc_Absyn_Eq){
goto _LL1233;} else{ goto _LL1234;} _LL1234: if( _temp1220 ==( void*) Cyc_Absyn_Neq){
goto _LL1235;} else{ goto _LL1236;} _LL1236: if( _temp1220 ==( void*) Cyc_Absyn_Gt){
goto _LL1237;} else{ goto _LL1238;} _LL1238: if( _temp1220 ==( void*) Cyc_Absyn_Lt){
goto _LL1239;} else{ goto _LL1240;} _LL1240: if( _temp1220 ==( void*) Cyc_Absyn_Gte){
goto _LL1241;} else{ goto _LL1242;} _LL1242: if( _temp1220 ==( void*) Cyc_Absyn_Lte){
goto _LL1243;} else{ goto _LL1244;} _LL1244: if( _temp1220 ==( void*) Cyc_Absyn_Not){
goto _LL1245;} else{ goto _LL1246;} _LL1246: if( _temp1220 ==( void*) Cyc_Absyn_Bitnot){
goto _LL1247;} else{ goto _LL1248;} _LL1248: if( _temp1220 ==( void*) Cyc_Absyn_Bitand){
goto _LL1249;} else{ goto _LL1250;} _LL1250: if( _temp1220 ==( void*) Cyc_Absyn_Bitor){
goto _LL1251;} else{ goto _LL1252;} _LL1252: if( _temp1220 ==( void*) Cyc_Absyn_Bitxor){
goto _LL1253;} else{ goto _LL1254;} _LL1254: if( _temp1220 ==( void*) Cyc_Absyn_Bitlshift){
goto _LL1255;} else{ goto _LL1256;} _LL1256: if( _temp1220 ==( void*) Cyc_Absyn_Bitlrshift){
goto _LL1257;} else{ goto _LL1258;} _LL1258: if( _temp1220 ==( void*) Cyc_Absyn_Bitarshift){
goto _LL1259;} else{ goto _LL1260;} _LL1260: if( _temp1220 ==( void*) Cyc_Absyn_Size){
goto _LL1261;} else{ goto _LL1262;} _LL1262: if( _temp1220 ==( void*) Cyc_Absyn_Printf){
goto _LL1263;} else{ goto _LL1264;} _LL1264: if( _temp1220 ==( void*) Cyc_Absyn_Fprintf){
goto _LL1265;} else{ goto _LL1266;} _LL1266: if( _temp1220 ==( void*) Cyc_Absyn_Xprintf){
goto _LL1267;} else{ goto _LL1268;} _LL1268: if( _temp1220 ==( void*) Cyc_Absyn_Scanf){
goto _LL1269;} else{ goto _LL1270;} _LL1270: if( _temp1220 ==( void*) Cyc_Absyn_Fscanf){
goto _LL1271;} else{ goto _LL1272;} _LL1272: if( _temp1220 ==( void*) Cyc_Absyn_Sscanf){
goto _LL1273;} else{ goto _LL1221;} _LL1223: return 100; _LL1225: return 110;
_LL1227: return 100; _LL1229: return 110; _LL1231: return 110; _LL1233: return
70; _LL1235: return 70; _LL1237: return 80; _LL1239: return 80; _LL1241: return
80; _LL1243: return 80; _LL1245: return 130; _LL1247: return 130; _LL1249:
return 60; _LL1251: return 40; _LL1253: return 50; _LL1255: return 90; _LL1257:
return 80; _LL1259: return 80; _LL1261: return 140; _LL1263: return 130; _LL1265:
return 130; _LL1267: return 130; _LL1269: return 130; _LL1271: return 130;
_LL1273: return 130; _LL1221:;} _LL1025: return 20; _LL1027: return 130; _LL1029:
return 30; _LL1031: return 10; _LL1033: return 140; _LL1035: return 140; _LL1037:
return 130; _LL1039: return Cyc_Absynpp_exp_prec( _temp1130); _LL1041: return
Cyc_Absynpp_exp_prec( _temp1134); _LL1043: return 120; _LL1045: return 15;
_LL1047: return 130; _LL1049: return 130; _LL1051: return 130; _LL1053: return
130; _LL1055: return 140; _LL1057: return 140; _LL1059: return 140; _LL1061:
return 150; _LL1063: goto _LL1065; _LL1065: goto _LL1067; _LL1067: goto _LL1069;
_LL1069: goto _LL1071; _LL1071: goto _LL1073; _LL1073: goto _LL1075; _LL1075:
goto _LL1077; _LL1077: return 140; _LL1079: return 140; _LL1081: return 10000;
_LL1083: return 140; _LL1085: return 140; _LL1015:;} struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc(
struct Cyc_Absyn_Exp* e){ return Cyc_Absynpp_exp2doc_prec( 0, e);} struct
_tuple8{ void* f1; void* f2; } ; struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc_prec(
int inprec, struct Cyc_Absyn_Exp* e){ int myprec= Cyc_Absynpp_exp_prec( e);
struct Cyc_PP_Doc* s;{ void* _temp1274=( void*) e->r; void* _temp1346; void*
_temp1348; struct _tuple0* _temp1350; struct _tuple0* _temp1352; struct Cyc_List_List*
_temp1354; void* _temp1356; struct Cyc_Absyn_Exp* _temp1358; struct Cyc_Core_Opt*
_temp1360; struct Cyc_Absyn_Exp* _temp1362; void* _temp1364; struct Cyc_Absyn_Exp*
_temp1366; struct Cyc_Absyn_Exp* _temp1368; struct Cyc_Absyn_Exp* _temp1370;
struct Cyc_Absyn_Exp* _temp1372; struct Cyc_Absyn_Exp* _temp1374; struct Cyc_Absyn_Exp*
_temp1376; struct Cyc_List_List* _temp1378; struct Cyc_Absyn_Exp* _temp1380;
struct Cyc_Absyn_VarargCallInfo* _temp1382; struct Cyc_List_List* _temp1384;
struct Cyc_Absyn_Exp* _temp1386; struct Cyc_Absyn_Exp* _temp1388; struct Cyc_Absyn_Exp*
_temp1390; struct Cyc_List_List* _temp1392; struct Cyc_Absyn_Exp* _temp1394;
struct Cyc_Absyn_Exp* _temp1396; void* _temp1398; struct Cyc_Absyn_Exp*
_temp1400; struct Cyc_Absyn_Exp* _temp1402; struct Cyc_Absyn_Exp* _temp1404;
void* _temp1406; struct Cyc_Absyn_Exp* _temp1408; struct Cyc_Absyn_Exp*
_temp1410; struct _tagged_string* _temp1412; struct Cyc_Absyn_Exp* _temp1414;
struct _tagged_string* _temp1416; struct Cyc_Absyn_Exp* _temp1418; struct Cyc_Absyn_Exp*
_temp1420; struct Cyc_Absyn_Exp* _temp1422; struct Cyc_List_List* _temp1424;
struct Cyc_List_List* _temp1426; struct _tuple1* _temp1428; struct Cyc_List_List*
_temp1430; struct Cyc_Absyn_Exp* _temp1432; struct Cyc_Absyn_Exp* _temp1434;
struct Cyc_Absyn_Vardecl* _temp1436; struct Cyc_Absyn_Structdecl* _temp1438;
struct Cyc_List_List* _temp1440; struct Cyc_Core_Opt* _temp1442; struct _tuple0*
_temp1444; struct Cyc_List_List* _temp1446; void* _temp1448; struct Cyc_Absyn_Tunionfield*
_temp1450; struct Cyc_Absyn_Tuniondecl* _temp1452; struct Cyc_List_List*
_temp1454; struct Cyc_Core_Opt* _temp1456; struct Cyc_Core_Opt* _temp1458;
struct Cyc_Absyn_Enumfield* _temp1460; struct Cyc_Absyn_Enumdecl* _temp1462;
struct _tuple0* _temp1464; void* _temp1466; struct Cyc_Absyn_Exp* _temp1468;
struct Cyc_List_List* _temp1470; struct Cyc_Core_Opt* _temp1472; struct Cyc_Absyn_Stmt*
_temp1474; struct Cyc_Absyn_Fndecl* _temp1476; struct Cyc_Absyn_Exp* _temp1478;
_LL1276: if(*(( int*) _temp1274) == Cyc_Absyn_Const_e){ _LL1347: _temp1346=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1274)->f1; goto _LL1277;} else{
goto _LL1278;} _LL1278: if(*(( int*) _temp1274) == Cyc_Absyn_Var_e){ _LL1351:
_temp1350=(( struct Cyc_Absyn_Var_e_struct*) _temp1274)->f1; goto _LL1349;
_LL1349: _temp1348=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1274)->f2;
goto _LL1279;} else{ goto _LL1280;} _LL1280: if(*(( int*) _temp1274) == Cyc_Absyn_UnknownId_e){
_LL1353: _temp1352=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp1274)->f1; goto
_LL1281;} else{ goto _LL1282;} _LL1282: if(*(( int*) _temp1274) == Cyc_Absyn_Primop_e){
_LL1357: _temp1356=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1274)->f1;
goto _LL1355; _LL1355: _temp1354=(( struct Cyc_Absyn_Primop_e_struct*) _temp1274)->f2;
goto _LL1283;} else{ goto _LL1284;} _LL1284: if(*(( int*) _temp1274) == Cyc_Absyn_AssignOp_e){
_LL1363: _temp1362=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp1274)->f1; goto
_LL1361; _LL1361: _temp1360=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp1274)->f2;
goto _LL1359; _LL1359: _temp1358=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1274)->f3; goto _LL1285;} else{ goto _LL1286;} _LL1286: if(*(( int*)
_temp1274) == Cyc_Absyn_Increment_e){ _LL1367: _temp1366=(( struct Cyc_Absyn_Increment_e_struct*)
_temp1274)->f1; goto _LL1365; _LL1365: _temp1364=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1274)->f2; goto _LL1287;} else{ goto _LL1288;} _LL1288: if(*(( int*)
_temp1274) == Cyc_Absyn_Conditional_e){ _LL1373: _temp1372=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1274)->f1; goto _LL1371; _LL1371: _temp1370=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1274)->f2; goto _LL1369; _LL1369: _temp1368=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1274)->f3; goto _LL1289;} else{ goto _LL1290;} _LL1290: if(*(( int*)
_temp1274) == Cyc_Absyn_SeqExp_e){ _LL1377: _temp1376=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1274)->f1; goto _LL1375; _LL1375: _temp1374=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1274)->f2; goto _LL1291;} else{ goto _LL1292;} _LL1292: if(*(( int*)
_temp1274) == Cyc_Absyn_UnknownCall_e){ _LL1381: _temp1380=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1274)->f1; goto _LL1379; _LL1379: _temp1378=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1274)->f2; goto _LL1293;} else{ goto _LL1294;} _LL1294: if(*(( int*)
_temp1274) == Cyc_Absyn_FnCall_e){ _LL1387: _temp1386=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1274)->f1; goto _LL1385; _LL1385: _temp1384=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1274)->f2; goto _LL1383; _LL1383: _temp1382=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1274)->f3; goto _LL1295;} else{ goto _LL1296;} _LL1296: if(*(( int*)
_temp1274) == Cyc_Absyn_Throw_e){ _LL1389: _temp1388=(( struct Cyc_Absyn_Throw_e_struct*)
_temp1274)->f1; goto _LL1297;} else{ goto _LL1298;} _LL1298: if(*(( int*)
_temp1274) == Cyc_Absyn_NoInstantiate_e){ _LL1391: _temp1390=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1274)->f1; goto _LL1299;} else{ goto _LL1300;} _LL1300: if(*(( int*)
_temp1274) == Cyc_Absyn_Instantiate_e){ _LL1395: _temp1394=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1274)->f1; goto _LL1393; _LL1393: _temp1392=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1274)->f2; goto _LL1301;} else{ goto _LL1302;} _LL1302: if(*(( int*)
_temp1274) == Cyc_Absyn_Cast_e){ _LL1399: _temp1398=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1274)->f1; goto _LL1397; _LL1397: _temp1396=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1274)->f2; goto _LL1303;} else{ goto _LL1304;} _LL1304: if(*(( int*)
_temp1274) == Cyc_Absyn_Address_e){ _LL1401: _temp1400=(( struct Cyc_Absyn_Address_e_struct*)
_temp1274)->f1; goto _LL1305;} else{ goto _LL1306;} _LL1306: if(*(( int*)
_temp1274) == Cyc_Absyn_New_e){ _LL1405: _temp1404=(( struct Cyc_Absyn_New_e_struct*)
_temp1274)->f1; goto _LL1403; _LL1403: _temp1402=(( struct Cyc_Absyn_New_e_struct*)
_temp1274)->f2; goto _LL1307;} else{ goto _LL1308;} _LL1308: if(*(( int*)
_temp1274) == Cyc_Absyn_Sizeoftyp_e){ _LL1407: _temp1406=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp1274)->f1; goto _LL1309;} else{ goto _LL1310;} _LL1310: if(*(( int*)
_temp1274) == Cyc_Absyn_Sizeofexp_e){ _LL1409: _temp1408=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1274)->f1; goto _LL1311;} else{ goto _LL1312;} _LL1312: if(*(( int*)
_temp1274) == Cyc_Absyn_Deref_e){ _LL1411: _temp1410=(( struct Cyc_Absyn_Deref_e_struct*)
_temp1274)->f1; goto _LL1313;} else{ goto _LL1314;} _LL1314: if(*(( int*)
_temp1274) == Cyc_Absyn_StructMember_e){ _LL1415: _temp1414=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1274)->f1; goto _LL1413; _LL1413: _temp1412=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1274)->f2; goto _LL1315;} else{ goto _LL1316;} _LL1316: if(*(( int*)
_temp1274) == Cyc_Absyn_StructArrow_e){ _LL1419: _temp1418=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1274)->f1; goto _LL1417; _LL1417: _temp1416=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1274)->f2; goto _LL1317;} else{ goto _LL1318;} _LL1318: if(*(( int*)
_temp1274) == Cyc_Absyn_Subscript_e){ _LL1423: _temp1422=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1274)->f1; goto _LL1421; _LL1421: _temp1420=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1274)->f2; goto _LL1319;} else{ goto _LL1320;} _LL1320: if(*(( int*)
_temp1274) == Cyc_Absyn_Tuple_e){ _LL1425: _temp1424=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1274)->f1; goto _LL1321;} else{ goto _LL1322;} _LL1322: if(*(( int*)
_temp1274) == Cyc_Absyn_CompoundLit_e){ _LL1429: _temp1428=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1274)->f1; goto _LL1427; _LL1427: _temp1426=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1274)->f2; goto _LL1323;} else{ goto _LL1324;} _LL1324: if(*(( int*)
_temp1274) == Cyc_Absyn_Array_e){ _LL1431: _temp1430=(( struct Cyc_Absyn_Array_e_struct*)
_temp1274)->f1; goto _LL1325;} else{ goto _LL1326;} _LL1326: if(*(( int*)
_temp1274) == Cyc_Absyn_Comprehension_e){ _LL1437: _temp1436=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1274)->f1; goto _LL1435; _LL1435: _temp1434=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1274)->f2; goto _LL1433; _LL1433: _temp1432=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1274)->f3; goto _LL1327;} else{ goto _LL1328;} _LL1328: if(*(( int*)
_temp1274) == Cyc_Absyn_Struct_e){ _LL1445: _temp1444=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1274)->f1; goto _LL1443; _LL1443: _temp1442=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1274)->f2; goto _LL1441; _LL1441: _temp1440=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1274)->f3; goto _LL1439; _LL1439: _temp1438=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1274)->f4; goto _LL1329;} else{ goto _LL1330;} _LL1330: if(*(( int*)
_temp1274) == Cyc_Absyn_AnonStruct_e){ _LL1449: _temp1448=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1274)->f1; goto _LL1447; _LL1447: _temp1446=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1274)->f2; goto _LL1331;} else{ goto _LL1332;} _LL1332: if(*(( int*)
_temp1274) == Cyc_Absyn_Tunion_e){ _LL1459: _temp1458=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1274)->f1; goto _LL1457; _LL1457: _temp1456=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1274)->f2; goto _LL1455; _LL1455: _temp1454=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1274)->f3; goto _LL1453; _LL1453: _temp1452=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1274)->f4; goto _LL1451; _LL1451: _temp1450=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1274)->f5; goto _LL1333;} else{ goto _LL1334;} _LL1334: if(*(( int*)
_temp1274) == Cyc_Absyn_Enum_e){ _LL1465: _temp1464=(( struct Cyc_Absyn_Enum_e_struct*)
_temp1274)->f1; goto _LL1463; _LL1463: _temp1462=(( struct Cyc_Absyn_Enum_e_struct*)
_temp1274)->f2; goto _LL1461; _LL1461: _temp1460=(( struct Cyc_Absyn_Enum_e_struct*)
_temp1274)->f3; goto _LL1335;} else{ goto _LL1336;} _LL1336: if(*(( int*)
_temp1274) == Cyc_Absyn_Malloc_e){ _LL1469: _temp1468=(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1274)->f1; goto _LL1467; _LL1467: _temp1466=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1274)->f2; goto _LL1337;} else{ goto _LL1338;} _LL1338: if(*(( int*)
_temp1274) == Cyc_Absyn_UnresolvedMem_e){ _LL1473: _temp1472=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1274)->f1; goto _LL1471; _LL1471: _temp1470=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1274)->f2; goto _LL1339;} else{ goto _LL1340;} _LL1340: if(*(( int*)
_temp1274) == Cyc_Absyn_StmtExp_e){ _LL1475: _temp1474=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1274)->f1; goto _LL1341;} else{ goto _LL1342;} _LL1342: if(*(( int*)
_temp1274) == Cyc_Absyn_Codegen_e){ _LL1477: _temp1476=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1274)->f1; goto _LL1343;} else{ goto _LL1344;} _LL1344: if(*(( int*)
_temp1274) == Cyc_Absyn_Fill_e){ _LL1479: _temp1478=(( struct Cyc_Absyn_Fill_e_struct*)
_temp1274)->f1; goto _LL1345;} else{ goto _LL1275;} _LL1277: s= Cyc_Absynpp_cnst2doc(
_temp1346); goto _LL1275; _LL1279: _temp1352= _temp1350; goto _LL1281; _LL1281:
s= Cyc_Absynpp_qvar2doc( _temp1352); goto _LL1275; _LL1283: s= Cyc_Absynpp_primapp2doc(
myprec, _temp1356, _temp1354); goto _LL1275; _LL1285: s=({ struct Cyc_PP_Doc*(*
_temp1480)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1483= Cyc_Absynpp_exp2doc_prec(
myprec, _temp1362); struct Cyc_PP_Doc* _temp1484= Cyc_PP_text(({ unsigned char*
_temp1492=" "; struct _tagged_string _temp1493; _temp1493.curr= _temp1492;
_temp1493.base= _temp1492; _temp1493.last_plus_one= _temp1492 + 2; _temp1493;}));
struct Cyc_PP_Doc* _temp1485= _temp1360 == 0? Cyc_PP_text(({ unsigned char*
_temp1490=""; struct _tagged_string _temp1491; _temp1491.curr= _temp1490;
_temp1491.base= _temp1490; _temp1491.last_plus_one= _temp1490 + 1; _temp1491;})):
Cyc_Absynpp_prim2doc(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1360))->v);
struct Cyc_PP_Doc* _temp1486= Cyc_PP_text(({ unsigned char* _temp1488="= ";
struct _tagged_string _temp1489; _temp1489.curr= _temp1488; _temp1489.base=
_temp1488; _temp1489.last_plus_one= _temp1488 + 3; _temp1489;})); struct Cyc_PP_Doc*
_temp1487= Cyc_Absynpp_exp2doc_prec( myprec, _temp1358); struct Cyc_PP_Doc*
_temp1481[ 5u]={ _temp1483, _temp1484, _temp1485, _temp1486, _temp1487}; struct
_tagged_ptr0 _temp1482={ _temp1481, _temp1481, _temp1481 + 5u}; _temp1480(
_temp1482);}); goto _LL1275; _LL1287: { struct Cyc_PP_Doc* _temp1494= Cyc_Absynpp_exp2doc_prec(
myprec, _temp1366); goto _LL1495; _LL1495:{ void* _temp1496= _temp1364; _LL1498:
if( _temp1496 ==( void*) Cyc_Absyn_PreInc){ goto _LL1499;} else{ goto _LL1500;}
_LL1500: if( _temp1496 ==( void*) Cyc_Absyn_PreDec){ goto _LL1501;} else{ goto
_LL1502;} _LL1502: if( _temp1496 ==( void*) Cyc_Absyn_PostInc){ goto _LL1503;}
else{ goto _LL1504;} _LL1504: if( _temp1496 ==( void*) Cyc_Absyn_PostDec){ goto
_LL1505;} else{ goto _LL1497;} _LL1499: s=({ struct Cyc_PP_Doc*(* _temp1506)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1509= Cyc_PP_text(({
unsigned char* _temp1511="++"; struct _tagged_string _temp1512; _temp1512.curr=
_temp1511; _temp1512.base= _temp1511; _temp1512.last_plus_one= _temp1511 + 3;
_temp1512;})); struct Cyc_PP_Doc* _temp1510= _temp1494; struct Cyc_PP_Doc*
_temp1507[ 2u]={ _temp1509, _temp1510}; struct _tagged_ptr0 _temp1508={
_temp1507, _temp1507, _temp1507 + 2u}; _temp1506( _temp1508);}); goto _LL1497;
_LL1501: s=({ struct Cyc_PP_Doc*(* _temp1513)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1516= Cyc_PP_text(({ unsigned char* _temp1518="--";
struct _tagged_string _temp1519; _temp1519.curr= _temp1518; _temp1519.base=
_temp1518; _temp1519.last_plus_one= _temp1518 + 3; _temp1519;})); struct Cyc_PP_Doc*
_temp1517= _temp1494; struct Cyc_PP_Doc* _temp1514[ 2u]={ _temp1516, _temp1517};
struct _tagged_ptr0 _temp1515={ _temp1514, _temp1514, _temp1514 + 2u}; _temp1513(
_temp1515);}); goto _LL1497; _LL1503: s=({ struct Cyc_PP_Doc*(* _temp1520)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1523= _temp1494;
struct Cyc_PP_Doc* _temp1524= Cyc_PP_text(({ unsigned char* _temp1525="++";
struct _tagged_string _temp1526; _temp1526.curr= _temp1525; _temp1526.base=
_temp1525; _temp1526.last_plus_one= _temp1525 + 3; _temp1526;})); struct Cyc_PP_Doc*
_temp1521[ 2u]={ _temp1523, _temp1524}; struct _tagged_ptr0 _temp1522={
_temp1521, _temp1521, _temp1521 + 2u}; _temp1520( _temp1522);}); goto _LL1497;
_LL1505: s=({ struct Cyc_PP_Doc*(* _temp1527)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1530= _temp1494; struct Cyc_PP_Doc* _temp1531= Cyc_PP_text(({
unsigned char* _temp1532="--"; struct _tagged_string _temp1533; _temp1533.curr=
_temp1532; _temp1533.base= _temp1532; _temp1533.last_plus_one= _temp1532 + 3;
_temp1533;})); struct Cyc_PP_Doc* _temp1528[ 2u]={ _temp1530, _temp1531}; struct
_tagged_ptr0 _temp1529={ _temp1528, _temp1528, _temp1528 + 2u}; _temp1527(
_temp1529);}); goto _LL1497; _LL1497:;} goto _LL1275;} _LL1289:{ struct _tuple8
_temp1535=({ struct _tuple8 _temp1534; _temp1534.f1=( void*) _temp1370->r;
_temp1534.f2=( void*) _temp1368->r; _temp1534;}); _LL1537: goto _LL1538; _LL1538:
s=({ struct Cyc_PP_Doc*(* _temp1539)( struct _tagged_ptr0)= Cyc_PP_cat; struct
Cyc_PP_Doc* _temp1542= Cyc_Absynpp_exp2doc_prec( myprec, _temp1372); struct Cyc_PP_Doc*
_temp1543= Cyc_PP_text(({ unsigned char* _temp1549=" ? "; struct _tagged_string
_temp1550; _temp1550.curr= _temp1549; _temp1550.base= _temp1549; _temp1550.last_plus_one=
_temp1549 + 4; _temp1550;})); struct Cyc_PP_Doc* _temp1544= Cyc_Absynpp_exp2doc_prec(
0, _temp1370); struct Cyc_PP_Doc* _temp1545= Cyc_PP_text(({ unsigned char*
_temp1547=" : "; struct _tagged_string _temp1548; _temp1548.curr= _temp1547;
_temp1548.base= _temp1547; _temp1548.last_plus_one= _temp1547 + 4; _temp1548;}));
struct Cyc_PP_Doc* _temp1546= Cyc_Absynpp_exp2doc_prec( myprec, _temp1368);
struct Cyc_PP_Doc* _temp1540[ 5u]={ _temp1542, _temp1543, _temp1544, _temp1545,
_temp1546}; struct _tagged_ptr0 _temp1541={ _temp1540, _temp1540, _temp1540 + 5u};
_temp1539( _temp1541);}); goto _LL1536; _LL1536:;} goto _LL1275; _LL1291: s=({
struct Cyc_PP_Doc*(* _temp1551)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1554= Cyc_PP_text(({ unsigned char* _temp1563="("; struct _tagged_string
_temp1564; _temp1564.curr= _temp1563; _temp1564.base= _temp1563; _temp1564.last_plus_one=
_temp1563 + 2; _temp1564;})); struct Cyc_PP_Doc* _temp1555= Cyc_Absynpp_exp2doc(
_temp1376); struct Cyc_PP_Doc* _temp1556= Cyc_PP_text(({ unsigned char*
_temp1561=", "; struct _tagged_string _temp1562; _temp1562.curr= _temp1561;
_temp1562.base= _temp1561; _temp1562.last_plus_one= _temp1561 + 3; _temp1562;}));
struct Cyc_PP_Doc* _temp1557= Cyc_Absynpp_exp2doc( _temp1374); struct Cyc_PP_Doc*
_temp1558= Cyc_PP_text(({ unsigned char* _temp1559=")"; struct _tagged_string
_temp1560; _temp1560.curr= _temp1559; _temp1560.base= _temp1559; _temp1560.last_plus_one=
_temp1559 + 2; _temp1560;})); struct Cyc_PP_Doc* _temp1552[ 5u]={ _temp1554,
_temp1555, _temp1556, _temp1557, _temp1558}; struct _tagged_ptr0 _temp1553={
_temp1552, _temp1552, _temp1552 + 5u}; _temp1551( _temp1553);}); goto _LL1275;
_LL1293: s=({ struct Cyc_PP_Doc*(* _temp1565)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1568= Cyc_Absynpp_exp2doc_prec( myprec, _temp1380);
struct Cyc_PP_Doc* _temp1569= Cyc_PP_text(({ unsigned char* _temp1574="(";
struct _tagged_string _temp1575; _temp1575.curr= _temp1574; _temp1575.base=
_temp1574; _temp1575.last_plus_one= _temp1574 + 2; _temp1575;})); struct Cyc_PP_Doc*
_temp1570= Cyc_Absynpp_exps2doc_prec( 20, _temp1378); struct Cyc_PP_Doc*
_temp1571= Cyc_PP_text(({ unsigned char* _temp1572=")"; struct _tagged_string
_temp1573; _temp1573.curr= _temp1572; _temp1573.base= _temp1572; _temp1573.last_plus_one=
_temp1572 + 2; _temp1573;})); struct Cyc_PP_Doc* _temp1566[ 4u]={ _temp1568,
_temp1569, _temp1570, _temp1571}; struct _tagged_ptr0 _temp1567={ _temp1566,
_temp1566, _temp1566 + 4u}; _temp1565( _temp1567);}); goto _LL1275; _LL1295: s=({
struct Cyc_PP_Doc*(* _temp1576)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1579= Cyc_Absynpp_exp2doc_prec( myprec, _temp1386); struct Cyc_PP_Doc*
_temp1580= Cyc_PP_text(({ unsigned char* _temp1585="("; struct _tagged_string
_temp1586; _temp1586.curr= _temp1585; _temp1586.base= _temp1585; _temp1586.last_plus_one=
_temp1585 + 2; _temp1586;})); struct Cyc_PP_Doc* _temp1581= Cyc_Absynpp_exps2doc_prec(
20, _temp1384); struct Cyc_PP_Doc* _temp1582= Cyc_PP_text(({ unsigned char*
_temp1583=")"; struct _tagged_string _temp1584; _temp1584.curr= _temp1583;
_temp1584.base= _temp1583; _temp1584.last_plus_one= _temp1583 + 2; _temp1584;}));
struct Cyc_PP_Doc* _temp1577[ 4u]={ _temp1579, _temp1580, _temp1581, _temp1582};
struct _tagged_ptr0 _temp1578={ _temp1577, _temp1577, _temp1577 + 4u}; _temp1576(
_temp1578);}); goto _LL1275; _LL1297: s=({ struct Cyc_PP_Doc*(* _temp1587)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1590= Cyc_PP_text(({
unsigned char* _temp1592="throw "; struct _tagged_string _temp1593; _temp1593.curr=
_temp1592; _temp1593.base= _temp1592; _temp1593.last_plus_one= _temp1592 + 7;
_temp1593;})); struct Cyc_PP_Doc* _temp1591= Cyc_Absynpp_exp2doc_prec( myprec,
_temp1388); struct Cyc_PP_Doc* _temp1588[ 2u]={ _temp1590, _temp1591}; struct
_tagged_ptr0 _temp1589={ _temp1588, _temp1588, _temp1588 + 2u}; _temp1587(
_temp1589);}); goto _LL1275; _LL1299: s= Cyc_Absynpp_exp2doc_prec( inprec,
_temp1390); goto _LL1275; _LL1301: s= Cyc_Absynpp_exp2doc_prec( inprec,
_temp1394); goto _LL1275; _LL1303: s=({ struct Cyc_PP_Doc*(* _temp1594)( struct
_tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1597= Cyc_PP_text(({
unsigned char* _temp1603="("; struct _tagged_string _temp1604; _temp1604.curr=
_temp1603; _temp1604.base= _temp1603; _temp1604.last_plus_one= _temp1603 + 2;
_temp1604;})); struct Cyc_PP_Doc* _temp1598= Cyc_Absynpp_typ2doc( _temp1398);
struct Cyc_PP_Doc* _temp1599= Cyc_PP_text(({ unsigned char* _temp1601=")";
struct _tagged_string _temp1602; _temp1602.curr= _temp1601; _temp1602.base=
_temp1601; _temp1602.last_plus_one= _temp1601 + 2; _temp1602;})); struct Cyc_PP_Doc*
_temp1600= Cyc_Absynpp_exp2doc_prec( myprec, _temp1396); struct Cyc_PP_Doc*
_temp1595[ 4u]={ _temp1597, _temp1598, _temp1599, _temp1600}; struct
_tagged_ptr0 _temp1596={ _temp1595, _temp1595, _temp1595 + 4u}; _temp1594(
_temp1596);}); goto _LL1275; _LL1305: s=({ struct Cyc_PP_Doc*(* _temp1605)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1608= Cyc_PP_text(({
unsigned char* _temp1610="&"; struct _tagged_string _temp1611; _temp1611.curr=
_temp1610; _temp1611.base= _temp1610; _temp1611.last_plus_one= _temp1610 + 2;
_temp1611;})); struct Cyc_PP_Doc* _temp1609= Cyc_Absynpp_exp2doc_prec( myprec,
_temp1400); struct Cyc_PP_Doc* _temp1606[ 2u]={ _temp1608, _temp1609}; struct
_tagged_ptr0 _temp1607={ _temp1606, _temp1606, _temp1606 + 2u}; _temp1605(
_temp1607);}); goto _LL1275; _LL1307: if( _temp1404 == 0){ s=({ struct Cyc_PP_Doc*(*
_temp1612)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1615= Cyc_PP_text(({
unsigned char* _temp1617="new "; struct _tagged_string _temp1618; _temp1618.curr=
_temp1617; _temp1618.base= _temp1617; _temp1618.last_plus_one= _temp1617 + 5;
_temp1618;})); struct Cyc_PP_Doc* _temp1616= Cyc_Absynpp_exp2doc_prec( myprec,
_temp1402); struct Cyc_PP_Doc* _temp1613[ 2u]={ _temp1615, _temp1616}; struct
_tagged_ptr0 _temp1614={ _temp1613, _temp1613, _temp1613 + 2u}; _temp1612(
_temp1614);});} else{ s=({ struct Cyc_PP_Doc*(* _temp1619)( struct _tagged_ptr0)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp1622= Cyc_PP_text(({ unsigned char*
_temp1628="rnew("; struct _tagged_string _temp1629; _temp1629.curr= _temp1628;
_temp1629.base= _temp1628; _temp1629.last_plus_one= _temp1628 + 6; _temp1629;}));
struct Cyc_PP_Doc* _temp1623= Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*)
_check_null( _temp1404)); struct Cyc_PP_Doc* _temp1624= Cyc_PP_text(({
unsigned char* _temp1626=") "; struct _tagged_string _temp1627; _temp1627.curr=
_temp1626; _temp1627.base= _temp1626; _temp1627.last_plus_one= _temp1626 + 3;
_temp1627;})); struct Cyc_PP_Doc* _temp1625= Cyc_Absynpp_exp2doc_prec( myprec,
_temp1402); struct Cyc_PP_Doc* _temp1620[ 4u]={ _temp1622, _temp1623, _temp1624,
_temp1625}; struct _tagged_ptr0 _temp1621={ _temp1620, _temp1620, _temp1620 + 4u};
_temp1619( _temp1621);});} goto _LL1275; _LL1309: s=({ struct Cyc_PP_Doc*(*
_temp1630)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1633= Cyc_PP_text(({
unsigned char* _temp1638="sizeof("; struct _tagged_string _temp1639; _temp1639.curr=
_temp1638; _temp1639.base= _temp1638; _temp1639.last_plus_one= _temp1638 + 8;
_temp1639;})); struct Cyc_PP_Doc* _temp1634= Cyc_Absynpp_typ2doc( _temp1406);
struct Cyc_PP_Doc* _temp1635= Cyc_PP_text(({ unsigned char* _temp1636=")";
struct _tagged_string _temp1637; _temp1637.curr= _temp1636; _temp1637.base=
_temp1636; _temp1637.last_plus_one= _temp1636 + 2; _temp1637;})); struct Cyc_PP_Doc*
_temp1631[ 3u]={ _temp1633, _temp1634, _temp1635}; struct _tagged_ptr0 _temp1632={
_temp1631, _temp1631, _temp1631 + 3u}; _temp1630( _temp1632);}); goto _LL1275;
_LL1311: s=({ struct Cyc_PP_Doc*(* _temp1640)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1643= Cyc_PP_text(({ unsigned char* _temp1648="sizeof(";
struct _tagged_string _temp1649; _temp1649.curr= _temp1648; _temp1649.base=
_temp1648; _temp1649.last_plus_one= _temp1648 + 8; _temp1649;})); struct Cyc_PP_Doc*
_temp1644= Cyc_Absynpp_exp2doc( _temp1408); struct Cyc_PP_Doc* _temp1645= Cyc_PP_text(({
unsigned char* _temp1646=")"; struct _tagged_string _temp1647; _temp1647.curr=
_temp1646; _temp1647.base= _temp1646; _temp1647.last_plus_one= _temp1646 + 2;
_temp1647;})); struct Cyc_PP_Doc* _temp1641[ 3u]={ _temp1643, _temp1644,
_temp1645}; struct _tagged_ptr0 _temp1642={ _temp1641, _temp1641, _temp1641 + 3u};
_temp1640( _temp1642);}); goto _LL1275; _LL1313: s=({ struct Cyc_PP_Doc*(*
_temp1650)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1653= Cyc_PP_text(({
unsigned char* _temp1655="*"; struct _tagged_string _temp1656; _temp1656.curr=
_temp1655; _temp1656.base= _temp1655; _temp1656.last_plus_one= _temp1655 + 2;
_temp1656;})); struct Cyc_PP_Doc* _temp1654= Cyc_Absynpp_exp2doc_prec( myprec,
_temp1410); struct Cyc_PP_Doc* _temp1651[ 2u]={ _temp1653, _temp1654}; struct
_tagged_ptr0 _temp1652={ _temp1651, _temp1651, _temp1651 + 2u}; _temp1650(
_temp1652);}); goto _LL1275; _LL1315: s=({ struct Cyc_PP_Doc*(* _temp1657)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1660= Cyc_Absynpp_exp2doc_prec(
myprec, _temp1414); struct Cyc_PP_Doc* _temp1661= Cyc_PP_text(({ unsigned char*
_temp1663="."; struct _tagged_string _temp1664; _temp1664.curr= _temp1663;
_temp1664.base= _temp1663; _temp1664.last_plus_one= _temp1663 + 2; _temp1664;}));
struct Cyc_PP_Doc* _temp1662= Cyc_PP_textptr( _temp1412); struct Cyc_PP_Doc*
_temp1658[ 3u]={ _temp1660, _temp1661, _temp1662}; struct _tagged_ptr0 _temp1659={
_temp1658, _temp1658, _temp1658 + 3u}; _temp1657( _temp1659);}); goto _LL1275;
_LL1317: s=({ struct Cyc_PP_Doc*(* _temp1665)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1668= Cyc_Absynpp_exp2doc_prec( myprec, _temp1418);
struct Cyc_PP_Doc* _temp1669= Cyc_PP_text(({ unsigned char* _temp1671="->";
struct _tagged_string _temp1672; _temp1672.curr= _temp1671; _temp1672.base=
_temp1671; _temp1672.last_plus_one= _temp1671 + 3; _temp1672;})); struct Cyc_PP_Doc*
_temp1670= Cyc_PP_textptr( _temp1416); struct Cyc_PP_Doc* _temp1666[ 3u]={
_temp1668, _temp1669, _temp1670}; struct _tagged_ptr0 _temp1667={ _temp1666,
_temp1666, _temp1666 + 3u}; _temp1665( _temp1667);}); goto _LL1275; _LL1319: s=({
struct Cyc_PP_Doc*(* _temp1673)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1676= Cyc_Absynpp_exp2doc_prec( myprec, _temp1422); struct Cyc_PP_Doc*
_temp1677= Cyc_PP_text(({ unsigned char* _temp1682="["; struct _tagged_string
_temp1683; _temp1683.curr= _temp1682; _temp1683.base= _temp1682; _temp1683.last_plus_one=
_temp1682 + 2; _temp1683;})); struct Cyc_PP_Doc* _temp1678= Cyc_Absynpp_exp2doc(
_temp1420); struct Cyc_PP_Doc* _temp1679= Cyc_PP_text(({ unsigned char*
_temp1680="]"; struct _tagged_string _temp1681; _temp1681.curr= _temp1680;
_temp1681.base= _temp1680; _temp1681.last_plus_one= _temp1680 + 2; _temp1681;}));
struct Cyc_PP_Doc* _temp1674[ 4u]={ _temp1676, _temp1677, _temp1678, _temp1679};
struct _tagged_ptr0 _temp1675={ _temp1674, _temp1674, _temp1674 + 4u}; _temp1673(
_temp1675);}); goto _LL1275; _LL1321: s=({ struct Cyc_PP_Doc*(* _temp1684)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1687= Cyc_PP_text(({
unsigned char* _temp1692="$("; struct _tagged_string _temp1693; _temp1693.curr=
_temp1692; _temp1693.base= _temp1692; _temp1693.last_plus_one= _temp1692 + 3;
_temp1693;})); struct Cyc_PP_Doc* _temp1688= Cyc_Absynpp_exps2doc_prec( 20,
_temp1424); struct Cyc_PP_Doc* _temp1689= Cyc_PP_text(({ unsigned char*
_temp1690=")"; struct _tagged_string _temp1691; _temp1691.curr= _temp1690;
_temp1691.base= _temp1690; _temp1691.last_plus_one= _temp1690 + 2; _temp1691;}));
struct Cyc_PP_Doc* _temp1685[ 3u]={ _temp1687, _temp1688, _temp1689}; struct
_tagged_ptr0 _temp1686={ _temp1685, _temp1685, _temp1685 + 3u}; _temp1684(
_temp1686);}); goto _LL1275; _LL1323: s=({ struct Cyc_PP_Doc*(* _temp1694)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1697= Cyc_PP_text(({
unsigned char* _temp1709="("; struct _tagged_string _temp1710; _temp1710.curr=
_temp1709; _temp1710.base= _temp1709; _temp1710.last_plus_one= _temp1709 + 2;
_temp1710;})); struct Cyc_PP_Doc* _temp1698= Cyc_Absynpp_typ2doc((* _temp1428).f3);
struct Cyc_PP_Doc* _temp1699= Cyc_PP_text(({ unsigned char* _temp1707=")";
struct _tagged_string _temp1708; _temp1708.curr= _temp1707; _temp1708.base=
_temp1707; _temp1708.last_plus_one= _temp1707 + 2; _temp1708;})); struct Cyc_PP_Doc*
_temp1700= Cyc_PP_group(({ unsigned char* _temp1701="{"; struct _tagged_string
_temp1702; _temp1702.curr= _temp1701; _temp1702.base= _temp1701; _temp1702.last_plus_one=
_temp1701 + 2; _temp1702;}),({ unsigned char* _temp1703="}"; struct
_tagged_string _temp1704; _temp1704.curr= _temp1703; _temp1704.base= _temp1703;
_temp1704.last_plus_one= _temp1703 + 2; _temp1704;}),({ unsigned char* _temp1705=",";
struct _tagged_string _temp1706; _temp1706.curr= _temp1705; _temp1706.base=
_temp1705; _temp1706.last_plus_one= _temp1705 + 2; _temp1706;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_de2doc, _temp1426)); struct Cyc_PP_Doc* _temp1695[ 4u]={ _temp1697,
_temp1698, _temp1699, _temp1700}; struct _tagged_ptr0 _temp1696={ _temp1695,
_temp1695, _temp1695 + 4u}; _temp1694( _temp1696);}); goto _LL1275; _LL1325: s=
Cyc_PP_group(({ unsigned char* _temp1711="{"; struct _tagged_string _temp1712;
_temp1712.curr= _temp1711; _temp1712.base= _temp1711; _temp1712.last_plus_one=
_temp1711 + 2; _temp1712;}),({ unsigned char* _temp1713="}"; struct
_tagged_string _temp1714; _temp1714.curr= _temp1713; _temp1714.base= _temp1713;
_temp1714.last_plus_one= _temp1713 + 2; _temp1714;}),({ unsigned char* _temp1715=",";
struct _tagged_string _temp1716; _temp1716.curr= _temp1715; _temp1716.base=
_temp1715; _temp1716.last_plus_one= _temp1715 + 2; _temp1716;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_de2doc, _temp1430)); goto _LL1275; _LL1327: s=({ struct Cyc_PP_Doc*(*
_temp1717)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1720= Cyc_PP_text(({
unsigned char* _temp1733="{for "; struct _tagged_string _temp1734; _temp1734.curr=
_temp1733; _temp1734.base= _temp1733; _temp1734.last_plus_one= _temp1733 + 6;
_temp1734;})); struct Cyc_PP_Doc* _temp1721= Cyc_PP_text(*(* _temp1436->name).f2);
struct Cyc_PP_Doc* _temp1722= Cyc_PP_text(({ unsigned char* _temp1731=" < ";
struct _tagged_string _temp1732; _temp1732.curr= _temp1731; _temp1732.base=
_temp1731; _temp1732.last_plus_one= _temp1731 + 4; _temp1732;})); struct Cyc_PP_Doc*
_temp1723= Cyc_Absynpp_exp2doc( _temp1434); struct Cyc_PP_Doc* _temp1724= Cyc_PP_text(({
unsigned char* _temp1729=" : "; struct _tagged_string _temp1730; _temp1730.curr=
_temp1729; _temp1730.base= _temp1729; _temp1730.last_plus_one= _temp1729 + 4;
_temp1730;})); struct Cyc_PP_Doc* _temp1725= Cyc_Absynpp_exp2doc( _temp1432);
struct Cyc_PP_Doc* _temp1726= Cyc_PP_text(({ unsigned char* _temp1727="}";
struct _tagged_string _temp1728; _temp1728.curr= _temp1727; _temp1728.base=
_temp1727; _temp1728.last_plus_one= _temp1727 + 2; _temp1728;})); struct Cyc_PP_Doc*
_temp1718[ 7u]={ _temp1720, _temp1721, _temp1722, _temp1723, _temp1724,
_temp1725, _temp1726}; struct _tagged_ptr0 _temp1719={ _temp1718, _temp1718,
_temp1718 + 7u}; _temp1717( _temp1719);}); goto _LL1275; _LL1329: s=({ struct
Cyc_PP_Doc*(* _temp1735)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1738= Cyc_Absynpp_qvar2doc( _temp1444); struct Cyc_PP_Doc* _temp1739= Cyc_PP_group(({
unsigned char* _temp1740="{"; struct _tagged_string _temp1741; _temp1741.curr=
_temp1740; _temp1741.base= _temp1740; _temp1741.last_plus_one= _temp1740 + 2;
_temp1741;}),({ unsigned char* _temp1742="}"; struct _tagged_string _temp1743;
_temp1743.curr= _temp1742; _temp1743.base= _temp1742; _temp1743.last_plus_one=
_temp1742 + 2; _temp1743;}),({ unsigned char* _temp1744=","; struct
_tagged_string _temp1745; _temp1745.curr= _temp1744; _temp1745.base= _temp1744;
_temp1745.last_plus_one= _temp1744 + 2; _temp1745;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_de2doc, _temp1440)); struct Cyc_PP_Doc* _temp1736[ 2u]={ _temp1738,
_temp1739}; struct _tagged_ptr0 _temp1737={ _temp1736, _temp1736, _temp1736 + 2u};
_temp1735( _temp1737);}); goto _LL1275; _LL1331: s= Cyc_PP_group(({
unsigned char* _temp1746="{"; struct _tagged_string _temp1747; _temp1747.curr=
_temp1746; _temp1747.base= _temp1746; _temp1747.last_plus_one= _temp1746 + 2;
_temp1747;}),({ unsigned char* _temp1748="}"; struct _tagged_string _temp1749;
_temp1749.curr= _temp1748; _temp1749.base= _temp1748; _temp1749.last_plus_one=
_temp1748 + 2; _temp1749;}),({ unsigned char* _temp1750=","; struct
_tagged_string _temp1751; _temp1751.curr= _temp1750; _temp1751.base= _temp1750;
_temp1751.last_plus_one= _temp1750 + 2; _temp1751;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_de2doc, _temp1446)); goto _LL1275; _LL1333: if( _temp1454 == 0){ s=
Cyc_Absynpp_qvar2doc( _temp1450->name);} else{ s=({ struct Cyc_PP_Doc*(*
_temp1752)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1755= Cyc_Absynpp_qvar2doc(
_temp1450->name); struct Cyc_PP_Doc* _temp1756= Cyc_PP_egroup(({ unsigned char*
_temp1757="("; struct _tagged_string _temp1758; _temp1758.curr= _temp1757;
_temp1758.base= _temp1757; _temp1758.last_plus_one= _temp1757 + 2; _temp1758;}),({
unsigned char* _temp1759=")"; struct _tagged_string _temp1760; _temp1760.curr=
_temp1759; _temp1760.base= _temp1759; _temp1760.last_plus_one= _temp1759 + 2;
_temp1760;}),({ unsigned char* _temp1761=","; struct _tagged_string _temp1762;
_temp1762.curr= _temp1761; _temp1762.base= _temp1761; _temp1762.last_plus_one=
_temp1761 + 2; _temp1762;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_exp2doc,
_temp1454)); struct Cyc_PP_Doc* _temp1753[ 2u]={ _temp1755, _temp1756}; struct
_tagged_ptr0 _temp1754={ _temp1753, _temp1753, _temp1753 + 2u}; _temp1752(
_temp1754);});} goto _LL1275; _LL1335: s= Cyc_Absynpp_qvar2doc( _temp1464); goto
_LL1275; _LL1337: if( _temp1468 == 0){ s=({ struct Cyc_PP_Doc*(* _temp1763)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1766= Cyc_PP_text(({
unsigned char* _temp1771="malloc(sizeof("; struct _tagged_string _temp1772;
_temp1772.curr= _temp1771; _temp1772.base= _temp1771; _temp1772.last_plus_one=
_temp1771 + 15; _temp1772;})); struct Cyc_PP_Doc* _temp1767= Cyc_Absynpp_typ2doc(
_temp1466); struct Cyc_PP_Doc* _temp1768= Cyc_PP_text(({ unsigned char*
_temp1769="))"; struct _tagged_string _temp1770; _temp1770.curr= _temp1769;
_temp1770.base= _temp1769; _temp1770.last_plus_one= _temp1769 + 3; _temp1770;}));
struct Cyc_PP_Doc* _temp1764[ 3u]={ _temp1766, _temp1767, _temp1768}; struct
_tagged_ptr0 _temp1765={ _temp1764, _temp1764, _temp1764 + 3u}; _temp1763(
_temp1765);});} else{ s=({ struct Cyc_PP_Doc*(* _temp1773)( struct _tagged_ptr0)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp1776= Cyc_PP_text(({ unsigned char*
_temp1788="rmalloc("; struct _tagged_string _temp1789; _temp1789.curr= _temp1788;
_temp1789.base= _temp1788; _temp1789.last_plus_one= _temp1788 + 9; _temp1789;}));
struct Cyc_PP_Doc* _temp1777= Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*)
_check_null( _temp1468)); struct Cyc_PP_Doc* _temp1778= Cyc_PP_text(({
unsigned char* _temp1786=","; struct _tagged_string _temp1787; _temp1787.curr=
_temp1786; _temp1787.base= _temp1786; _temp1787.last_plus_one= _temp1786 + 2;
_temp1787;})); struct Cyc_PP_Doc* _temp1779= Cyc_PP_text(({ unsigned char*
_temp1784="sizeof("; struct _tagged_string _temp1785; _temp1785.curr= _temp1784;
_temp1785.base= _temp1784; _temp1785.last_plus_one= _temp1784 + 8; _temp1785;}));
struct Cyc_PP_Doc* _temp1780= Cyc_Absynpp_typ2doc( _temp1466); struct Cyc_PP_Doc*
_temp1781= Cyc_PP_text(({ unsigned char* _temp1782="))"; struct _tagged_string
_temp1783; _temp1783.curr= _temp1782; _temp1783.base= _temp1782; _temp1783.last_plus_one=
_temp1782 + 3; _temp1783;})); struct Cyc_PP_Doc* _temp1774[ 6u]={ _temp1776,
_temp1777, _temp1778, _temp1779, _temp1780, _temp1781}; struct _tagged_ptr0
_temp1775={ _temp1774, _temp1774, _temp1774 + 6u}; _temp1773( _temp1775);});}
goto _LL1275; _LL1339: s= Cyc_PP_group(({ unsigned char* _temp1790="{"; struct
_tagged_string _temp1791; _temp1791.curr= _temp1790; _temp1791.base= _temp1790;
_temp1791.last_plus_one= _temp1790 + 2; _temp1791;}),({ unsigned char* _temp1792="}";
struct _tagged_string _temp1793; _temp1793.curr= _temp1792; _temp1793.base=
_temp1792; _temp1793.last_plus_one= _temp1792 + 2; _temp1793;}),({ unsigned char*
_temp1794=","; struct _tagged_string _temp1795; _temp1795.curr= _temp1794;
_temp1795.base= _temp1794; _temp1795.last_plus_one= _temp1794 + 2; _temp1795;}),((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_de2doc, _temp1470)); goto _LL1275; _LL1341: s=({
struct Cyc_PP_Doc*(* _temp1796)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1799= Cyc_PP_text(({ unsigned char* _temp1804="({ "; struct _tagged_string
_temp1805; _temp1805.curr= _temp1804; _temp1805.base= _temp1804; _temp1805.last_plus_one=
_temp1804 + 4; _temp1805;})); struct Cyc_PP_Doc* _temp1800= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1474)); struct Cyc_PP_Doc* _temp1801= Cyc_PP_text(({ unsigned char*
_temp1802=" })"; struct _tagged_string _temp1803; _temp1803.curr= _temp1802;
_temp1803.base= _temp1802; _temp1803.last_plus_one= _temp1802 + 4; _temp1803;}));
struct Cyc_PP_Doc* _temp1797[ 3u]={ _temp1799, _temp1800, _temp1801}; struct
_tagged_ptr0 _temp1798={ _temp1797, _temp1797, _temp1797 + 3u}; _temp1796(
_temp1798);}); goto _LL1275; _LL1343: s=({ struct Cyc_PP_Doc*(* _temp1806)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1809= Cyc_PP_text(({
unsigned char* _temp1817="codegen("; struct _tagged_string _temp1818; _temp1818.curr=
_temp1817; _temp1818.base= _temp1817; _temp1818.last_plus_one= _temp1817 + 9;
_temp1818;})); struct Cyc_PP_Doc* _temp1810= Cyc_PP_nest( 2, Cyc_Absynpp_decl2doc(({
struct Cyc_Absyn_Decl* _temp1814=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof(
struct Cyc_Absyn_Decl)); _temp1814->r=( void*)(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp1815=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp1815[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp1816; _temp1816.tag= Cyc_Absyn_Fn_d;
_temp1816.f1= _temp1476; _temp1816;}); _temp1815;})); _temp1814->loc= e->loc;
_temp1814;}))); struct Cyc_PP_Doc* _temp1811= Cyc_PP_text(({ unsigned char*
_temp1812=")"; struct _tagged_string _temp1813; _temp1813.curr= _temp1812;
_temp1813.base= _temp1812; _temp1813.last_plus_one= _temp1812 + 2; _temp1813;}));
struct Cyc_PP_Doc* _temp1807[ 3u]={ _temp1809, _temp1810, _temp1811}; struct
_tagged_ptr0 _temp1808={ _temp1807, _temp1807, _temp1807 + 3u}; _temp1806(
_temp1808);}); goto _LL1275; _LL1345: s=({ struct Cyc_PP_Doc*(* _temp1819)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1822= Cyc_PP_text(({
unsigned char* _temp1827="fill("; struct _tagged_string _temp1828; _temp1828.curr=
_temp1827; _temp1828.base= _temp1827; _temp1828.last_plus_one= _temp1827 + 6;
_temp1828;})); struct Cyc_PP_Doc* _temp1823= Cyc_PP_nest( 2, Cyc_Absynpp_exp2doc(
_temp1478)); struct Cyc_PP_Doc* _temp1824= Cyc_PP_text(({ unsigned char*
_temp1825=")"; struct _tagged_string _temp1826; _temp1826.curr= _temp1825;
_temp1826.base= _temp1825; _temp1826.last_plus_one= _temp1825 + 2; _temp1826;}));
struct Cyc_PP_Doc* _temp1820[ 3u]={ _temp1822, _temp1823, _temp1824}; struct
_tagged_ptr0 _temp1821={ _temp1820, _temp1820, _temp1820 + 3u}; _temp1819(
_temp1821);}); goto _LL1275; _LL1275:;} if( inprec >= myprec){ s=({ struct Cyc_PP_Doc*(*
_temp1829)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1832= Cyc_PP_text(({
unsigned char* _temp1837="("; struct _tagged_string _temp1838; _temp1838.curr=
_temp1837; _temp1838.base= _temp1837; _temp1838.last_plus_one= _temp1837 + 2;
_temp1838;})); struct Cyc_PP_Doc* _temp1833= s; struct Cyc_PP_Doc* _temp1834=
Cyc_PP_text(({ unsigned char* _temp1835=")"; struct _tagged_string _temp1836;
_temp1836.curr= _temp1835; _temp1836.base= _temp1835; _temp1836.last_plus_one=
_temp1835 + 2; _temp1836;})); struct Cyc_PP_Doc* _temp1830[ 3u]={ _temp1832,
_temp1833, _temp1834}; struct _tagged_ptr0 _temp1831={ _temp1830, _temp1830,
_temp1830 + 3u}; _temp1829( _temp1831);});} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_designator2doc(
void* d){ void* _temp1839= d; struct Cyc_Absyn_Exp* _temp1845; struct
_tagged_string* _temp1847; _LL1841: if(*(( int*) _temp1839) == Cyc_Absyn_ArrayElement){
_LL1846: _temp1845=(( struct Cyc_Absyn_ArrayElement_struct*) _temp1839)->f1;
goto _LL1842;} else{ goto _LL1843;} _LL1843: if(*(( int*) _temp1839) == Cyc_Absyn_FieldName){
_LL1848: _temp1847=(( struct Cyc_Absyn_FieldName_struct*) _temp1839)->f1; goto
_LL1844;} else{ goto _LL1840;} _LL1842: return({ struct Cyc_PP_Doc*(* _temp1849)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1852= Cyc_PP_text(({
unsigned char* _temp1857=".["; struct _tagged_string _temp1858; _temp1858.curr=
_temp1857; _temp1858.base= _temp1857; _temp1858.last_plus_one= _temp1857 + 3;
_temp1858;})); struct Cyc_PP_Doc* _temp1853= Cyc_Absynpp_exp2doc( _temp1845);
struct Cyc_PP_Doc* _temp1854= Cyc_PP_text(({ unsigned char* _temp1855="]";
struct _tagged_string _temp1856; _temp1856.curr= _temp1855; _temp1856.base=
_temp1855; _temp1856.last_plus_one= _temp1855 + 2; _temp1856;})); struct Cyc_PP_Doc*
_temp1850[ 3u]={ _temp1852, _temp1853, _temp1854}; struct _tagged_ptr0 _temp1851={
_temp1850, _temp1850, _temp1850 + 3u}; _temp1849( _temp1851);}); _LL1844: return({
struct Cyc_PP_Doc*(* _temp1859)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1862= Cyc_PP_text(({ unsigned char* _temp1864="."; struct _tagged_string
_temp1865; _temp1865.curr= _temp1864; _temp1865.base= _temp1864; _temp1865.last_plus_one=
_temp1864 + 2; _temp1865;})); struct Cyc_PP_Doc* _temp1863= Cyc_PP_textptr(
_temp1847); struct Cyc_PP_Doc* _temp1860[ 2u]={ _temp1862, _temp1863}; struct
_tagged_ptr0 _temp1861={ _temp1860, _temp1860, _temp1860 + 2u}; _temp1859(
_temp1861);}); _LL1840:;} struct Cyc_PP_Doc* Cyc_Absynpp_de2doc( struct _tuple6*
de){ if((* de).f1 == 0){ return Cyc_Absynpp_exp2doc((* de).f2);} else{ return({
struct Cyc_PP_Doc*(* _temp1866)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1869= Cyc_PP_egroup(({ unsigned char* _temp1871=""; struct _tagged_string
_temp1872; _temp1872.curr= _temp1871; _temp1872.base= _temp1871; _temp1872.last_plus_one=
_temp1871 + 1; _temp1872;}),({ unsigned char* _temp1873="="; struct
_tagged_string _temp1874; _temp1874.curr= _temp1873; _temp1874.base= _temp1873;
_temp1874.last_plus_one= _temp1873 + 2; _temp1874;}),({ unsigned char* _temp1875="=";
struct _tagged_string _temp1876; _temp1876.curr= _temp1875; _temp1876.base=
_temp1875; _temp1876.last_plus_one= _temp1875 + 2; _temp1876;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(*
de).f1)); struct Cyc_PP_Doc* _temp1870= Cyc_Absynpp_exp2doc((* de).f2); struct
Cyc_PP_Doc* _temp1867[ 2u]={ _temp1869, _temp1870}; struct _tagged_ptr0
_temp1868={ _temp1867, _temp1867, _temp1867 + 2u}; _temp1866( _temp1868);});}}
struct Cyc_PP_Doc* Cyc_Absynpp_exps2doc_prec( int inprec, struct Cyc_List_List*
es){ return Cyc_PP_group(({ unsigned char* _temp1877=""; struct _tagged_string
_temp1878; _temp1878.curr= _temp1877; _temp1878.base= _temp1877; _temp1878.last_plus_one=
_temp1877 + 1; _temp1878;}),({ unsigned char* _temp1879=""; struct
_tagged_string _temp1880; _temp1880.curr= _temp1879; _temp1880.base= _temp1879;
_temp1880.last_plus_one= _temp1879 + 1; _temp1880;}),({ unsigned char* _temp1881=",";
struct _tagged_string _temp1882; _temp1882.curr= _temp1881; _temp1882.base=
_temp1881; _temp1882.last_plus_one= _temp1881 + 2; _temp1882;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( int, struct Cyc_Absyn_Exp*), int env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Absynpp_exp2doc_prec, inprec, es));} struct Cyc_PP_Doc*
Cyc_Absynpp_cnst2doc( void* c){ void* _temp1883= c; unsigned char _temp1901;
void* _temp1903; short _temp1905; void* _temp1907; int _temp1909; void*
_temp1911; int _temp1913; void* _temp1915; long long _temp1917; void* _temp1919;
struct _tagged_string _temp1921; struct _tagged_string _temp1923; _LL1885: if((
unsigned int) _temp1883 > 1u?*(( int*) _temp1883) == Cyc_Absyn_Char_c: 0){
_LL1904: _temp1903=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp1883)->f1;
goto _LL1902; _LL1902: _temp1901=(( struct Cyc_Absyn_Char_c_struct*) _temp1883)->f2;
goto _LL1886;} else{ goto _LL1887;} _LL1887: if(( unsigned int) _temp1883 > 1u?*((
int*) _temp1883) == Cyc_Absyn_Short_c: 0){ _LL1908: _temp1907=( void*)(( struct
Cyc_Absyn_Short_c_struct*) _temp1883)->f1; goto _LL1906; _LL1906: _temp1905=((
struct Cyc_Absyn_Short_c_struct*) _temp1883)->f2; goto _LL1888;} else{ goto
_LL1889;} _LL1889: if(( unsigned int) _temp1883 > 1u?*(( int*) _temp1883) == Cyc_Absyn_Int_c:
0){ _LL1912: _temp1911=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp1883)->f1;
if( _temp1911 ==( void*) Cyc_Absyn_Signed){ goto _LL1910;} else{ goto _LL1891;}
_LL1910: _temp1909=(( struct Cyc_Absyn_Int_c_struct*) _temp1883)->f2; goto
_LL1890;} else{ goto _LL1891;} _LL1891: if(( unsigned int) _temp1883 > 1u?*((
int*) _temp1883) == Cyc_Absyn_Int_c: 0){ _LL1916: _temp1915=( void*)(( struct
Cyc_Absyn_Int_c_struct*) _temp1883)->f1; if( _temp1915 ==( void*) Cyc_Absyn_Unsigned){
goto _LL1914;} else{ goto _LL1893;} _LL1914: _temp1913=(( struct Cyc_Absyn_Int_c_struct*)
_temp1883)->f2; goto _LL1892;} else{ goto _LL1893;} _LL1893: if(( unsigned int)
_temp1883 > 1u?*(( int*) _temp1883) == Cyc_Absyn_LongLong_c: 0){ _LL1920:
_temp1919=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp1883)->f1; goto
_LL1918; _LL1918: _temp1917=(( struct Cyc_Absyn_LongLong_c_struct*) _temp1883)->f2;
goto _LL1894;} else{ goto _LL1895;} _LL1895: if(( unsigned int) _temp1883 > 1u?*((
int*) _temp1883) == Cyc_Absyn_Float_c: 0){ _LL1922: _temp1921=(( struct Cyc_Absyn_Float_c_struct*)
_temp1883)->f1; goto _LL1896;} else{ goto _LL1897;} _LL1897: if( _temp1883 ==(
void*) Cyc_Absyn_Null_c){ goto _LL1898;} else{ goto _LL1899;} _LL1899: if((
unsigned int) _temp1883 > 1u?*(( int*) _temp1883) == Cyc_Absyn_String_c: 0){
_LL1924: _temp1923=(( struct Cyc_Absyn_String_c_struct*) _temp1883)->f1; goto
_LL1900;} else{ goto _LL1884;} _LL1886: return Cyc_PP_text(({ struct
_tagged_string _temp1925= Cyc_Absynpp_char_escape( _temp1901); xprintf("'%.*s'",
_temp1925.last_plus_one - _temp1925.curr, _temp1925.curr);})); _LL1888: return
Cyc_PP_text( xprintf("%d",( int) _temp1905)); _LL1890: return Cyc_PP_text( Cyc_Core_string_of_int(
_temp1909)); _LL1892: return Cyc_PP_text( xprintf("%u",( unsigned int) _temp1913));
_LL1894: return Cyc_PP_text(({ unsigned char* _temp1926="<<FIX LONG LONG CONSTANT>>";
struct _tagged_string _temp1927; _temp1927.curr= _temp1926; _temp1927.base=
_temp1926; _temp1927.last_plus_one= _temp1926 + 27; _temp1927;})); _LL1896:
return Cyc_PP_text( _temp1921); _LL1898: return Cyc_PP_text(({ unsigned char*
_temp1928="null"; struct _tagged_string _temp1929; _temp1929.curr= _temp1928;
_temp1929.base= _temp1928; _temp1929.last_plus_one= _temp1928 + 5; _temp1929;}));
_LL1900: return({ struct Cyc_PP_Doc*(* _temp1930)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp1933= Cyc_PP_text(({ unsigned char* _temp1938="\"";
struct _tagged_string _temp1939; _temp1939.curr= _temp1938; _temp1939.base=
_temp1938; _temp1939.last_plus_one= _temp1938 + 2; _temp1939;})); struct Cyc_PP_Doc*
_temp1934= Cyc_PP_text( Cyc_Absynpp_string_escape( _temp1923)); struct Cyc_PP_Doc*
_temp1935= Cyc_PP_text(({ unsigned char* _temp1936="\""; struct _tagged_string
_temp1937; _temp1937.curr= _temp1936; _temp1937.base= _temp1936; _temp1937.last_plus_one=
_temp1936 + 2; _temp1937;})); struct Cyc_PP_Doc* _temp1931[ 3u]={ _temp1933,
_temp1934, _temp1935}; struct _tagged_ptr0 _temp1932={ _temp1931, _temp1931,
_temp1931 + 3u}; _temp1930( _temp1932);}); _LL1884:;} struct Cyc_PP_Doc* Cyc_Absynpp_primapp2doc(
int inprec, void* p, struct Cyc_List_List* es){ struct Cyc_PP_Doc* ps= Cyc_Absynpp_prim2doc(
p); if( p ==( void*) Cyc_Absyn_Size){ if( es == 0? 1:(( struct Cyc_List_List*)
_check_null( es))->tl != 0){( void) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp1940=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1940[ 0]=({ struct Cyc_Core_Failure_struct _temp1941; _temp1941.tag= Cyc_Core_Failure;
_temp1941.f1=({ struct _tagged_string _temp1942= Cyc_PP_string_of_doc( ps, 72);
xprintf("Absynpp::primapp2doc Size: %.*s with bad args", _temp1942.last_plus_one
- _temp1942.curr, _temp1942.curr);}); _temp1941;}); _temp1940;}));}{ struct Cyc_PP_Doc*
_temp1943= Cyc_Absynpp_exp2doc_prec( inprec,( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( es))->hd); goto _LL1944; _LL1944: return({ struct
Cyc_PP_Doc*(* _temp1945)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1948= _temp1943; struct Cyc_PP_Doc* _temp1949= Cyc_PP_text(({ unsigned char*
_temp1950=".size"; struct _tagged_string _temp1951; _temp1951.curr= _temp1950;
_temp1951.base= _temp1950; _temp1951.last_plus_one= _temp1950 + 6; _temp1951;}));
struct Cyc_PP_Doc* _temp1946[ 2u]={ _temp1948, _temp1949}; struct _tagged_ptr0
_temp1947={ _temp1946, _temp1946, _temp1946 + 2u}; _temp1945( _temp1947);});}}
else{ if( ! Cyc_Absyn_is_format_prim( p)){ struct Cyc_List_List* ds=(( struct
Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( int, struct Cyc_Absyn_Exp*), int env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Absynpp_exp2doc_prec, inprec, es);
if( ds == 0){( void) _throw(( void*)({ struct Cyc_Core_Failure_struct* _temp1952=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1952[ 0]=({ struct Cyc_Core_Failure_struct _temp1953; _temp1953.tag= Cyc_Core_Failure;
_temp1953.f1=({ struct _tagged_string _temp1954= Cyc_PP_string_of_doc( ps, 72);
xprintf("Absynpp::primapp2doc: %.*s with no args", _temp1954.last_plus_one -
_temp1954.curr, _temp1954.curr);}); _temp1953;}); _temp1952;}));} else{ if(((
struct Cyc_List_List*) _check_null( ds))->tl == 0){ return({ struct Cyc_PP_Doc*(*
_temp1955)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1958= ps;
struct Cyc_PP_Doc* _temp1959= Cyc_PP_text(({ unsigned char* _temp1961=" ";
struct _tagged_string _temp1962; _temp1962.curr= _temp1961; _temp1962.base=
_temp1961; _temp1962.last_plus_one= _temp1961 + 2; _temp1962;})); struct Cyc_PP_Doc*
_temp1960=( struct Cyc_PP_Doc*)(( struct Cyc_List_List*) _check_null( ds))->hd;
struct Cyc_PP_Doc* _temp1956[ 3u]={ _temp1958, _temp1959, _temp1960}; struct
_tagged_ptr0 _temp1957={ _temp1956, _temp1956, _temp1956 + 3u}; _temp1955(
_temp1957);});} else{ if((( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( ds))->tl))->tl != 0){( void) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp1963=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1963[ 0]=({ struct Cyc_Core_Failure_struct _temp1964; _temp1964.tag= Cyc_Core_Failure;
_temp1964.f1=({ struct _tagged_string _temp1965= Cyc_PP_string_of_doc( ps, 72);
xprintf("Absynpp::primapp2doc: %.*s with more than 2 args", _temp1965.last_plus_one
- _temp1965.curr, _temp1965.curr);}); _temp1964;}); _temp1963;}));} else{ return({
struct Cyc_PP_Doc*(* _temp1966)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp1969=( struct Cyc_PP_Doc*)(( struct Cyc_List_List*) _check_null( ds))->hd;
struct Cyc_PP_Doc* _temp1970= Cyc_PP_text(({ unsigned char* _temp1976=" ";
struct _tagged_string _temp1977; _temp1977.curr= _temp1976; _temp1977.base=
_temp1976; _temp1977.last_plus_one= _temp1976 + 2; _temp1977;})); struct Cyc_PP_Doc*
_temp1971= ps; struct Cyc_PP_Doc* _temp1972= Cyc_PP_text(({ unsigned char*
_temp1974=" "; struct _tagged_string _temp1975; _temp1975.curr= _temp1974;
_temp1975.base= _temp1974; _temp1975.last_plus_one= _temp1974 + 2; _temp1975;}));
struct Cyc_PP_Doc* _temp1973=( struct Cyc_PP_Doc*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( ds))->tl))->hd; struct Cyc_PP_Doc*
_temp1967[ 5u]={ _temp1969, _temp1970, _temp1971, _temp1972, _temp1973}; struct
_tagged_ptr0 _temp1968={ _temp1967, _temp1967, _temp1967 + 5u}; _temp1966(
_temp1968);});}}}} else{ return({ struct Cyc_PP_Doc*(* _temp1978)( struct
_tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp1981= ps; struct Cyc_PP_Doc*
_temp1982= Cyc_PP_text(({ unsigned char* _temp1987="("; struct _tagged_string
_temp1988; _temp1988.curr= _temp1987; _temp1988.base= _temp1987; _temp1988.last_plus_one=
_temp1987 + 2; _temp1988;})); struct Cyc_PP_Doc* _temp1983= Cyc_Absynpp_exps2doc_prec(
20, es); struct Cyc_PP_Doc* _temp1984= Cyc_PP_text(({ unsigned char* _temp1985=")";
struct _tagged_string _temp1986; _temp1986.curr= _temp1985; _temp1986.base=
_temp1985; _temp1986.last_plus_one= _temp1985 + 2; _temp1986;})); struct Cyc_PP_Doc*
_temp1979[ 4u]={ _temp1981, _temp1982, _temp1983, _temp1984}; struct
_tagged_ptr0 _temp1980={ _temp1979, _temp1979, _temp1979 + 4u}; _temp1978(
_temp1980);});}}} struct _tagged_string Cyc_Absynpp_prim2str( void* p){ void*
_temp1989= p; _LL1991: if( _temp1989 ==( void*) Cyc_Absyn_Plus){ goto _LL1992;}
else{ goto _LL1993;} _LL1993: if( _temp1989 ==( void*) Cyc_Absyn_Times){ goto
_LL1994;} else{ goto _LL1995;} _LL1995: if( _temp1989 ==( void*) Cyc_Absyn_Minus){
goto _LL1996;} else{ goto _LL1997;} _LL1997: if( _temp1989 ==( void*) Cyc_Absyn_Div){
goto _LL1998;} else{ goto _LL1999;} _LL1999: if( _temp1989 ==( void*) Cyc_Absyn_Mod){
goto _LL2000;} else{ goto _LL2001;} _LL2001: if( _temp1989 ==( void*) Cyc_Absyn_Eq){
goto _LL2002;} else{ goto _LL2003;} _LL2003: if( _temp1989 ==( void*) Cyc_Absyn_Neq){
goto _LL2004;} else{ goto _LL2005;} _LL2005: if( _temp1989 ==( void*) Cyc_Absyn_Gt){
goto _LL2006;} else{ goto _LL2007;} _LL2007: if( _temp1989 ==( void*) Cyc_Absyn_Lt){
goto _LL2008;} else{ goto _LL2009;} _LL2009: if( _temp1989 ==( void*) Cyc_Absyn_Gte){
goto _LL2010;} else{ goto _LL2011;} _LL2011: if( _temp1989 ==( void*) Cyc_Absyn_Lte){
goto _LL2012;} else{ goto _LL2013;} _LL2013: if( _temp1989 ==( void*) Cyc_Absyn_Not){
goto _LL2014;} else{ goto _LL2015;} _LL2015: if( _temp1989 ==( void*) Cyc_Absyn_Bitnot){
goto _LL2016;} else{ goto _LL2017;} _LL2017: if( _temp1989 ==( void*) Cyc_Absyn_Bitand){
goto _LL2018;} else{ goto _LL2019;} _LL2019: if( _temp1989 ==( void*) Cyc_Absyn_Bitor){
goto _LL2020;} else{ goto _LL2021;} _LL2021: if( _temp1989 ==( void*) Cyc_Absyn_Bitxor){
goto _LL2022;} else{ goto _LL2023;} _LL2023: if( _temp1989 ==( void*) Cyc_Absyn_Bitlshift){
goto _LL2024;} else{ goto _LL2025;} _LL2025: if( _temp1989 ==( void*) Cyc_Absyn_Bitlrshift){
goto _LL2026;} else{ goto _LL2027;} _LL2027: if( _temp1989 ==( void*) Cyc_Absyn_Bitarshift){
goto _LL2028;} else{ goto _LL2029;} _LL2029: if( _temp1989 ==( void*) Cyc_Absyn_Size){
goto _LL2030;} else{ goto _LL2031;} _LL2031: if( _temp1989 ==( void*) Cyc_Absyn_Printf){
goto _LL2032;} else{ goto _LL2033;} _LL2033: if( _temp1989 ==( void*) Cyc_Absyn_Fprintf){
goto _LL2034;} else{ goto _LL2035;} _LL2035: if( _temp1989 ==( void*) Cyc_Absyn_Xprintf){
goto _LL2036;} else{ goto _LL2037;} _LL2037: if( _temp1989 ==( void*) Cyc_Absyn_Scanf){
goto _LL2038;} else{ goto _LL2039;} _LL2039: if( _temp1989 ==( void*) Cyc_Absyn_Fscanf){
goto _LL2040;} else{ goto _LL2041;} _LL2041: if( _temp1989 ==( void*) Cyc_Absyn_Sscanf){
goto _LL2042;} else{ goto _LL1990;} _LL1992: return({ unsigned char* _temp2043="+";
struct _tagged_string _temp2044; _temp2044.curr= _temp2043; _temp2044.base=
_temp2043; _temp2044.last_plus_one= _temp2043 + 2; _temp2044;}); _LL1994: return({
unsigned char* _temp2045="*"; struct _tagged_string _temp2046; _temp2046.curr=
_temp2045; _temp2046.base= _temp2045; _temp2046.last_plus_one= _temp2045 + 2;
_temp2046;}); _LL1996: return({ unsigned char* _temp2047="-"; struct
_tagged_string _temp2048; _temp2048.curr= _temp2047; _temp2048.base= _temp2047;
_temp2048.last_plus_one= _temp2047 + 2; _temp2048;}); _LL1998: return({
unsigned char* _temp2049="/"; struct _tagged_string _temp2050; _temp2050.curr=
_temp2049; _temp2050.base= _temp2049; _temp2050.last_plus_one= _temp2049 + 2;
_temp2050;}); _LL2000: return({ unsigned char* _temp2051="%"; struct
_tagged_string _temp2052; _temp2052.curr= _temp2051; _temp2052.base= _temp2051;
_temp2052.last_plus_one= _temp2051 + 2; _temp2052;}); _LL2002: return({
unsigned char* _temp2053="=="; struct _tagged_string _temp2054; _temp2054.curr=
_temp2053; _temp2054.base= _temp2053; _temp2054.last_plus_one= _temp2053 + 3;
_temp2054;}); _LL2004: return({ unsigned char* _temp2055="!="; struct
_tagged_string _temp2056; _temp2056.curr= _temp2055; _temp2056.base= _temp2055;
_temp2056.last_plus_one= _temp2055 + 3; _temp2056;}); _LL2006: return({
unsigned char* _temp2057=">"; struct _tagged_string _temp2058; _temp2058.curr=
_temp2057; _temp2058.base= _temp2057; _temp2058.last_plus_one= _temp2057 + 2;
_temp2058;}); _LL2008: return({ unsigned char* _temp2059="<"; struct
_tagged_string _temp2060; _temp2060.curr= _temp2059; _temp2060.base= _temp2059;
_temp2060.last_plus_one= _temp2059 + 2; _temp2060;}); _LL2010: return({
unsigned char* _temp2061=">="; struct _tagged_string _temp2062; _temp2062.curr=
_temp2061; _temp2062.base= _temp2061; _temp2062.last_plus_one= _temp2061 + 3;
_temp2062;}); _LL2012: return({ unsigned char* _temp2063="<="; struct
_tagged_string _temp2064; _temp2064.curr= _temp2063; _temp2064.base= _temp2063;
_temp2064.last_plus_one= _temp2063 + 3; _temp2064;}); _LL2014: return({
unsigned char* _temp2065="!"; struct _tagged_string _temp2066; _temp2066.curr=
_temp2065; _temp2066.base= _temp2065; _temp2066.last_plus_one= _temp2065 + 2;
_temp2066;}); _LL2016: return({ unsigned char* _temp2067="~"; struct
_tagged_string _temp2068; _temp2068.curr= _temp2067; _temp2068.base= _temp2067;
_temp2068.last_plus_one= _temp2067 + 2; _temp2068;}); _LL2018: return({
unsigned char* _temp2069="&"; struct _tagged_string _temp2070; _temp2070.curr=
_temp2069; _temp2070.base= _temp2069; _temp2070.last_plus_one= _temp2069 + 2;
_temp2070;}); _LL2020: return({ unsigned char* _temp2071="|"; struct
_tagged_string _temp2072; _temp2072.curr= _temp2071; _temp2072.base= _temp2071;
_temp2072.last_plus_one= _temp2071 + 2; _temp2072;}); _LL2022: return({
unsigned char* _temp2073="^"; struct _tagged_string _temp2074; _temp2074.curr=
_temp2073; _temp2074.base= _temp2073; _temp2074.last_plus_one= _temp2073 + 2;
_temp2074;}); _LL2024: return({ unsigned char* _temp2075="<<"; struct
_tagged_string _temp2076; _temp2076.curr= _temp2075; _temp2076.base= _temp2075;
_temp2076.last_plus_one= _temp2075 + 3; _temp2076;}); _LL2026: return({
unsigned char* _temp2077=">>"; struct _tagged_string _temp2078; _temp2078.curr=
_temp2077; _temp2078.base= _temp2077; _temp2078.last_plus_one= _temp2077 + 3;
_temp2078;}); _LL2028: return({ unsigned char* _temp2079=">>>"; struct
_tagged_string _temp2080; _temp2080.curr= _temp2079; _temp2080.base= _temp2079;
_temp2080.last_plus_one= _temp2079 + 4; _temp2080;}); _LL2030: return({
unsigned char* _temp2081="size"; struct _tagged_string _temp2082; _temp2082.curr=
_temp2081; _temp2082.base= _temp2081; _temp2082.last_plus_one= _temp2081 + 5;
_temp2082;}); _LL2032: return({ unsigned char* _temp2083="printf"; struct
_tagged_string _temp2084; _temp2084.curr= _temp2083; _temp2084.base= _temp2083;
_temp2084.last_plus_one= _temp2083 + 7; _temp2084;}); _LL2034: return({
unsigned char* _temp2085="fprintf"; struct _tagged_string _temp2086; _temp2086.curr=
_temp2085; _temp2086.base= _temp2085; _temp2086.last_plus_one= _temp2085 + 8;
_temp2086;}); _LL2036: return({ unsigned char* _temp2087="xprintf"; struct
_tagged_string _temp2088; _temp2088.curr= _temp2087; _temp2088.base= _temp2087;
_temp2088.last_plus_one= _temp2087 + 8; _temp2088;}); _LL2038: return({
unsigned char* _temp2089="scanf"; struct _tagged_string _temp2090; _temp2090.curr=
_temp2089; _temp2090.base= _temp2089; _temp2090.last_plus_one= _temp2089 + 6;
_temp2090;}); _LL2040: return({ unsigned char* _temp2091="fscanf"; struct
_tagged_string _temp2092; _temp2092.curr= _temp2091; _temp2092.base= _temp2091;
_temp2092.last_plus_one= _temp2091 + 7; _temp2092;}); _LL2042: return({
unsigned char* _temp2093="sscanf"; struct _tagged_string _temp2094; _temp2094.curr=
_temp2093; _temp2094.base= _temp2093; _temp2094.last_plus_one= _temp2093 + 7;
_temp2094;}); _LL1990:;} struct Cyc_PP_Doc* Cyc_Absynpp_prim2doc( void* p){
return Cyc_PP_text( Cyc_Absynpp_prim2str( p));} struct _tuple9{ struct Cyc_Core_Opt*
f1; struct Cyc_Absyn_Exp* f2; } ; struct Cyc_PP_Doc* Cyc_Absynpp_structarg2doc(
struct _tuple9* t){ if((* t).f1 == 0){ return Cyc_Absynpp_exp2doc((* t).f2);}
else{ return({ struct Cyc_PP_Doc*(* _temp2095)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2098= Cyc_PP_textptr(( struct _tagged_string*)(( struct
Cyc_Core_Opt*) _check_null((* t).f1))->v); struct Cyc_PP_Doc* _temp2099= Cyc_PP_text(({
unsigned char* _temp2101=" = "; struct _tagged_string _temp2102; _temp2102.curr=
_temp2101; _temp2102.base= _temp2101; _temp2102.last_plus_one= _temp2101 + 4;
_temp2102;})); struct Cyc_PP_Doc* _temp2100= Cyc_Absynpp_exp2doc((* t).f2);
struct Cyc_PP_Doc* _temp2096[ 3u]={ _temp2098, _temp2099, _temp2100}; struct
_tagged_ptr0 _temp2097={ _temp2096, _temp2096, _temp2096 + 3u}; _temp2095(
_temp2097);});}} int Cyc_Absynpp_is_declaration( struct Cyc_Absyn_Stmt* s){ void*
_temp2103=( void*) s->r; struct Cyc_Absyn_Stmt* _temp2109; struct Cyc_Absyn_Decl*
_temp2111; _LL2105: if(( unsigned int) _temp2103 > 1u?*(( int*) _temp2103) ==
Cyc_Absyn_Decl_s: 0){ _LL2112: _temp2111=(( struct Cyc_Absyn_Decl_s_struct*)
_temp2103)->f1; goto _LL2110; _LL2110: _temp2109=(( struct Cyc_Absyn_Decl_s_struct*)
_temp2103)->f2; goto _LL2106;} else{ goto _LL2107;} _LL2107: goto _LL2108;
_LL2106: return 1; _LL2108: return 0; _LL2104:;} struct Cyc_PP_Doc* Cyc_Absynpp_stmt2doc(
struct Cyc_Absyn_Stmt* st){ struct Cyc_PP_Doc* s;{ void* _temp2113=( void*) st->r;
struct Cyc_Absyn_Exp* _temp2155; struct Cyc_Absyn_Stmt* _temp2157; struct Cyc_Absyn_Stmt*
_temp2159; struct Cyc_Absyn_Exp* _temp2161; struct Cyc_Absyn_Stmt* _temp2163;
struct Cyc_Absyn_Stmt* _temp2165; struct Cyc_Absyn_Exp* _temp2167; struct Cyc_Absyn_Stmt*
_temp2169; struct _tuple2 _temp2171; struct Cyc_Absyn_Stmt* _temp2173; struct
Cyc_Absyn_Exp* _temp2175; struct Cyc_Absyn_Stmt* _temp2177; struct Cyc_Absyn_Stmt*
_temp2179; struct Cyc_Absyn_Stmt* _temp2181; struct _tagged_string* _temp2183;
struct Cyc_Absyn_Stmt* _temp2185; struct _tuple2 _temp2187; struct Cyc_Absyn_Stmt*
_temp2189; struct Cyc_Absyn_Exp* _temp2191; struct _tuple2 _temp2193; struct Cyc_Absyn_Stmt*
_temp2195; struct Cyc_Absyn_Exp* _temp2197; struct Cyc_Absyn_Exp* _temp2199;
struct Cyc_List_List* _temp2201; struct Cyc_Absyn_Exp* _temp2203; struct Cyc_Absyn_Switch_clause**
_temp2205; struct Cyc_List_List* _temp2207; struct Cyc_Absyn_Switch_clause**
_temp2209; struct Cyc_List_List* _temp2211; struct Cyc_Absyn_Stmt* _temp2213;
struct Cyc_Absyn_Decl* _temp2215; struct Cyc_Absyn_Stmt* _temp2217; struct Cyc_Absyn_Stmt*
_temp2219; struct Cyc_Absyn_Stmt* _temp2221; struct _tagged_string* _temp2223;
struct _tuple2 _temp2225; struct Cyc_Absyn_Stmt* _temp2227; struct Cyc_Absyn_Exp*
_temp2229; struct Cyc_Absyn_Stmt* _temp2231; struct Cyc_List_List* _temp2233;
struct Cyc_Absyn_Stmt* _temp2235; struct Cyc_Absyn_Stmt* _temp2237; struct Cyc_Absyn_Vardecl*
_temp2239; struct Cyc_Absyn_Tvar* _temp2241; _LL2115: if( _temp2113 ==( void*)
Cyc_Absyn_Skip_s){ goto _LL2116;} else{ goto _LL2117;} _LL2117: if((
unsigned int) _temp2113 > 1u?*(( int*) _temp2113) == Cyc_Absyn_Exp_s: 0){
_LL2156: _temp2155=(( struct Cyc_Absyn_Exp_s_struct*) _temp2113)->f1; goto
_LL2118;} else{ goto _LL2119;} _LL2119: if(( unsigned int) _temp2113 > 1u?*((
int*) _temp2113) == Cyc_Absyn_Seq_s: 0){ _LL2160: _temp2159=(( struct Cyc_Absyn_Seq_s_struct*)
_temp2113)->f1; goto _LL2158; _LL2158: _temp2157=(( struct Cyc_Absyn_Seq_s_struct*)
_temp2113)->f2; goto _LL2120;} else{ goto _LL2121;} _LL2121: if(( unsigned int)
_temp2113 > 1u?*(( int*) _temp2113) == Cyc_Absyn_Return_s: 0){ _LL2162:
_temp2161=(( struct Cyc_Absyn_Return_s_struct*) _temp2113)->f1; goto _LL2122;}
else{ goto _LL2123;} _LL2123: if(( unsigned int) _temp2113 > 1u?*(( int*)
_temp2113) == Cyc_Absyn_IfThenElse_s: 0){ _LL2168: _temp2167=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2113)->f1; goto _LL2166; _LL2166: _temp2165=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2113)->f2; goto _LL2164; _LL2164: _temp2163=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp2113)->f3; goto _LL2124;} else{ goto _LL2125;} _LL2125: if(( unsigned int)
_temp2113 > 1u?*(( int*) _temp2113) == Cyc_Absyn_While_s: 0){ _LL2172: _temp2171=((
struct Cyc_Absyn_While_s_struct*) _temp2113)->f1; _LL2176: _temp2175= _temp2171.f1;
goto _LL2174; _LL2174: _temp2173= _temp2171.f2; goto _LL2170; _LL2170: _temp2169=((
struct Cyc_Absyn_While_s_struct*) _temp2113)->f2; goto _LL2126;} else{ goto
_LL2127;} _LL2127: if(( unsigned int) _temp2113 > 1u?*(( int*) _temp2113) == Cyc_Absyn_Break_s:
0){ _LL2178: _temp2177=(( struct Cyc_Absyn_Break_s_struct*) _temp2113)->f1; goto
_LL2128;} else{ goto _LL2129;} _LL2129: if(( unsigned int) _temp2113 > 1u?*((
int*) _temp2113) == Cyc_Absyn_Continue_s: 0){ _LL2180: _temp2179=(( struct Cyc_Absyn_Continue_s_struct*)
_temp2113)->f1; goto _LL2130;} else{ goto _LL2131;} _LL2131: if(( unsigned int)
_temp2113 > 1u?*(( int*) _temp2113) == Cyc_Absyn_Goto_s: 0){ _LL2184: _temp2183=((
struct Cyc_Absyn_Goto_s_struct*) _temp2113)->f1; goto _LL2182; _LL2182:
_temp2181=(( struct Cyc_Absyn_Goto_s_struct*) _temp2113)->f2; goto _LL2132;}
else{ goto _LL2133;} _LL2133: if(( unsigned int) _temp2113 > 1u?*(( int*)
_temp2113) == Cyc_Absyn_For_s: 0){ _LL2200: _temp2199=(( struct Cyc_Absyn_For_s_struct*)
_temp2113)->f1; goto _LL2194; _LL2194: _temp2193=(( struct Cyc_Absyn_For_s_struct*)
_temp2113)->f2; _LL2198: _temp2197= _temp2193.f1; goto _LL2196; _LL2196:
_temp2195= _temp2193.f2; goto _LL2188; _LL2188: _temp2187=(( struct Cyc_Absyn_For_s_struct*)
_temp2113)->f3; _LL2192: _temp2191= _temp2187.f1; goto _LL2190; _LL2190:
_temp2189= _temp2187.f2; goto _LL2186; _LL2186: _temp2185=(( struct Cyc_Absyn_For_s_struct*)
_temp2113)->f4; goto _LL2134;} else{ goto _LL2135;} _LL2135: if(( unsigned int)
_temp2113 > 1u?*(( int*) _temp2113) == Cyc_Absyn_Switch_s: 0){ _LL2204:
_temp2203=(( struct Cyc_Absyn_Switch_s_struct*) _temp2113)->f1; goto _LL2202;
_LL2202: _temp2201=(( struct Cyc_Absyn_Switch_s_struct*) _temp2113)->f2; goto
_LL2136;} else{ goto _LL2137;} _LL2137: if(( unsigned int) _temp2113 > 1u?*((
int*) _temp2113) == Cyc_Absyn_Fallthru_s: 0){ _LL2208: _temp2207=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp2113)->f1; if( _temp2207 == 0){ goto _LL2206;} else{ goto _LL2139;} _LL2206:
_temp2205=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp2113)->f2; goto _LL2138;}
else{ goto _LL2139;} _LL2139: if(( unsigned int) _temp2113 > 1u?*(( int*)
_temp2113) == Cyc_Absyn_Fallthru_s: 0){ _LL2212: _temp2211=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp2113)->f1; goto _LL2210; _LL2210: _temp2209=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp2113)->f2; goto _LL2140;} else{ goto _LL2141;} _LL2141: if(( unsigned int)
_temp2113 > 1u?*(( int*) _temp2113) == Cyc_Absyn_Decl_s: 0){ _LL2216: _temp2215=((
struct Cyc_Absyn_Decl_s_struct*) _temp2113)->f1; goto _LL2214; _LL2214:
_temp2213=(( struct Cyc_Absyn_Decl_s_struct*) _temp2113)->f2; goto _LL2142;}
else{ goto _LL2143;} _LL2143: if(( unsigned int) _temp2113 > 1u?*(( int*)
_temp2113) == Cyc_Absyn_Cut_s: 0){ _LL2218: _temp2217=(( struct Cyc_Absyn_Cut_s_struct*)
_temp2113)->f1; goto _LL2144;} else{ goto _LL2145;} _LL2145: if(( unsigned int)
_temp2113 > 1u?*(( int*) _temp2113) == Cyc_Absyn_Splice_s: 0){ _LL2220:
_temp2219=(( struct Cyc_Absyn_Splice_s_struct*) _temp2113)->f1; goto _LL2146;}
else{ goto _LL2147;} _LL2147: if(( unsigned int) _temp2113 > 1u?*(( int*)
_temp2113) == Cyc_Absyn_Label_s: 0){ _LL2224: _temp2223=(( struct Cyc_Absyn_Label_s_struct*)
_temp2113)->f1; goto _LL2222; _LL2222: _temp2221=(( struct Cyc_Absyn_Label_s_struct*)
_temp2113)->f2; goto _LL2148;} else{ goto _LL2149;} _LL2149: if(( unsigned int)
_temp2113 > 1u?*(( int*) _temp2113) == Cyc_Absyn_Do_s: 0){ _LL2232: _temp2231=((
struct Cyc_Absyn_Do_s_struct*) _temp2113)->f1; goto _LL2226; _LL2226: _temp2225=((
struct Cyc_Absyn_Do_s_struct*) _temp2113)->f2; _LL2230: _temp2229= _temp2225.f1;
goto _LL2228; _LL2228: _temp2227= _temp2225.f2; goto _LL2150;} else{ goto
_LL2151;} _LL2151: if(( unsigned int) _temp2113 > 1u?*(( int*) _temp2113) == Cyc_Absyn_TryCatch_s:
0){ _LL2236: _temp2235=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp2113)->f1;
goto _LL2234; _LL2234: _temp2233=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp2113)->f2; goto _LL2152;} else{ goto _LL2153;} _LL2153: if(( unsigned int)
_temp2113 > 1u?*(( int*) _temp2113) == Cyc_Absyn_Region_s: 0){ _LL2242:
_temp2241=(( struct Cyc_Absyn_Region_s_struct*) _temp2113)->f1; goto _LL2240;
_LL2240: _temp2239=(( struct Cyc_Absyn_Region_s_struct*) _temp2113)->f2; goto
_LL2238; _LL2238: _temp2237=(( struct Cyc_Absyn_Region_s_struct*) _temp2113)->f3;
goto _LL2154;} else{ goto _LL2114;} _LL2116: s= Cyc_PP_text(({ unsigned char*
_temp2243=";"; struct _tagged_string _temp2244; _temp2244.curr= _temp2243;
_temp2244.base= _temp2243; _temp2244.last_plus_one= _temp2243 + 2; _temp2244;}));
goto _LL2114; _LL2118: s=({ struct Cyc_PP_Doc*(* _temp2245)( struct _tagged_ptr0)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp2248= Cyc_Absynpp_exp2doc( _temp2155);
struct Cyc_PP_Doc* _temp2249= Cyc_PP_text(({ unsigned char* _temp2250=";";
struct _tagged_string _temp2251; _temp2251.curr= _temp2250; _temp2251.base=
_temp2250; _temp2251.last_plus_one= _temp2250 + 2; _temp2251;})); struct Cyc_PP_Doc*
_temp2246[ 2u]={ _temp2248, _temp2249}; struct _tagged_ptr0 _temp2247={
_temp2246, _temp2246, _temp2246 + 2u}; _temp2245( _temp2247);}); goto _LL2114;
_LL2120: if( Cyc_Absynpp_decls_first){ if( Cyc_Absynpp_is_declaration( _temp2159)){
s=({ struct Cyc_PP_Doc*(* _temp2252)( struct _tagged_ptr0)= Cyc_PP_cat; struct
Cyc_PP_Doc* _temp2255= Cyc_PP_text(({ unsigned char* _temp2274="{ "; struct
_tagged_string _temp2275; _temp2275.curr= _temp2274; _temp2275.base= _temp2274;
_temp2275.last_plus_one= _temp2274 + 3; _temp2275;})); struct Cyc_PP_Doc*
_temp2256= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp2159)); struct Cyc_PP_Doc*
_temp2257= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2258= Cyc_PP_text(({
unsigned char* _temp2272="}"; struct _tagged_string _temp2273; _temp2273.curr=
_temp2272; _temp2273.base= _temp2272; _temp2273.last_plus_one= _temp2272 + 2;
_temp2273;})); struct Cyc_PP_Doc* _temp2259= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp2260= Cyc_Absynpp_is_declaration( _temp2157)?({ struct Cyc_PP_Doc*(*
_temp2261)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2264= Cyc_PP_text(({
unsigned char* _temp2270="{ "; struct _tagged_string _temp2271; _temp2271.curr=
_temp2270; _temp2271.base= _temp2270; _temp2271.last_plus_one= _temp2270 + 3;
_temp2271;})); struct Cyc_PP_Doc* _temp2265= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp2157)); struct Cyc_PP_Doc* _temp2266= Cyc_PP_text(({ unsigned char*
_temp2268="}"; struct _tagged_string _temp2269; _temp2269.curr= _temp2268;
_temp2269.base= _temp2268; _temp2269.last_plus_one= _temp2268 + 2; _temp2269;}));
struct Cyc_PP_Doc* _temp2267= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2262[ 4u]={
_temp2264, _temp2265, _temp2266, _temp2267}; struct _tagged_ptr0 _temp2263={
_temp2262, _temp2262, _temp2262 + 4u}; _temp2261( _temp2263);}): Cyc_Absynpp_stmt2doc(
_temp2157); struct Cyc_PP_Doc* _temp2253[ 6u]={ _temp2255, _temp2256, _temp2257,
_temp2258, _temp2259, _temp2260}; struct _tagged_ptr0 _temp2254={ _temp2253,
_temp2253, _temp2253 + 6u}; _temp2252( _temp2254);});} else{ if( Cyc_Absynpp_is_declaration(
_temp2157)){ s=({ struct Cyc_PP_Doc*(* _temp2276)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2279= Cyc_Absynpp_stmt2doc( _temp2159); struct Cyc_PP_Doc*
_temp2280= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2281= Cyc_PP_text(({
unsigned char* _temp2287="{ "; struct _tagged_string _temp2288; _temp2288.curr=
_temp2287; _temp2288.base= _temp2287; _temp2288.last_plus_one= _temp2287 + 3;
_temp2288;})); struct Cyc_PP_Doc* _temp2282= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp2157)); struct Cyc_PP_Doc* _temp2283= Cyc_PP_text(({ unsigned char*
_temp2285="}"; struct _tagged_string _temp2286; _temp2286.curr= _temp2285;
_temp2286.base= _temp2285; _temp2286.last_plus_one= _temp2285 + 2; _temp2286;}));
struct Cyc_PP_Doc* _temp2284= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2277[ 6u]={
_temp2279, _temp2280, _temp2281, _temp2282, _temp2283, _temp2284}; struct
_tagged_ptr0 _temp2278={ _temp2277, _temp2277, _temp2277 + 6u}; _temp2276(
_temp2278);});} else{ s=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)(
struct Cyc_Absyn_Stmt*), struct _tagged_string sep, struct Cyc_List_List* l0))
Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc,({ unsigned char* _temp2289=""; struct
_tagged_string _temp2290; _temp2290.curr= _temp2289; _temp2290.base= _temp2289;
_temp2290.last_plus_one= _temp2289 + 1; _temp2290;}),({ struct Cyc_List_List*
_temp2291=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2291->hd=( void*) _temp2159; _temp2291->tl=({ struct Cyc_List_List*
_temp2292=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2292->hd=( void*) _temp2157; _temp2292->tl= 0; _temp2292;}); _temp2291;}));}}}
else{ s=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Stmt*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc,({
unsigned char* _temp2293=""; struct _tagged_string _temp2294; _temp2294.curr=
_temp2293; _temp2294.base= _temp2293; _temp2294.last_plus_one= _temp2293 + 1;
_temp2294;}),({ struct Cyc_List_List* _temp2295=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2295->hd=( void*) _temp2159;
_temp2295->tl=({ struct Cyc_List_List* _temp2296=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2296->hd=( void*) _temp2157;
_temp2296->tl= 0; _temp2296;}); _temp2295;}));} goto _LL2114; _LL2122: if(
_temp2161 == 0){ s= Cyc_PP_text(({ unsigned char* _temp2297="return;"; struct
_tagged_string _temp2298; _temp2298.curr= _temp2297; _temp2298.base= _temp2297;
_temp2298.last_plus_one= _temp2297 + 8; _temp2298;}));} else{ s=({ struct Cyc_PP_Doc*(*
_temp2299)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2302= Cyc_PP_text(({
unsigned char* _temp2307="return "; struct _tagged_string _temp2308; _temp2308.curr=
_temp2307; _temp2308.base= _temp2307; _temp2308.last_plus_one= _temp2307 + 8;
_temp2308;})); struct Cyc_PP_Doc* _temp2303= _temp2161 == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null( _temp2161)); struct
Cyc_PP_Doc* _temp2304= Cyc_PP_text(({ unsigned char* _temp2305=";"; struct
_tagged_string _temp2306; _temp2306.curr= _temp2305; _temp2306.base= _temp2305;
_temp2306.last_plus_one= _temp2305 + 2; _temp2306;})); struct Cyc_PP_Doc*
_temp2300[ 3u]={ _temp2302, _temp2303, _temp2304}; struct _tagged_ptr0 _temp2301={
_temp2300, _temp2300, _temp2300 + 3u}; _temp2299( _temp2301);});} goto _LL2114;
_LL2124: { int print_else;{ void* _temp2309=( void*) _temp2163->r; _LL2311: if(
_temp2309 ==( void*) Cyc_Absyn_Skip_s){ goto _LL2312;} else{ goto _LL2313;}
_LL2313: goto _LL2314; _LL2312: print_else= 0; goto _LL2310; _LL2314: print_else=
1; goto _LL2310; _LL2310:;} s=({ struct Cyc_PP_Doc*(* _temp2315)( struct
_tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2318= Cyc_PP_text(({
unsigned char* _temp2351="if ("; struct _tagged_string _temp2352; _temp2352.curr=
_temp2351; _temp2352.base= _temp2351; _temp2352.last_plus_one= _temp2351 + 5;
_temp2352;})); struct Cyc_PP_Doc* _temp2319= Cyc_Absynpp_exp2doc( _temp2167);
struct Cyc_PP_Doc* _temp2320= Cyc_PP_text(({ unsigned char* _temp2349=") {";
struct _tagged_string _temp2350; _temp2350.curr= _temp2349; _temp2350.base=
_temp2349; _temp2350.last_plus_one= _temp2349 + 4; _temp2350;})); struct Cyc_PP_Doc*
_temp2321= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp2344)( struct
_tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2347= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2348= Cyc_Absynpp_stmt2doc( _temp2165); struct Cyc_PP_Doc*
_temp2345[ 2u]={ _temp2347, _temp2348}; struct _tagged_ptr0 _temp2346={
_temp2345, _temp2345, _temp2345 + 2u}; _temp2344( _temp2346);})); struct Cyc_PP_Doc*
_temp2322= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2323= Cyc_PP_text(({
unsigned char* _temp2342="}"; struct _tagged_string _temp2343; _temp2343.curr=
_temp2342; _temp2343.base= _temp2342; _temp2343.last_plus_one= _temp2342 + 2;
_temp2343;})); struct Cyc_PP_Doc* _temp2324= print_else?({ struct Cyc_PP_Doc*(*
_temp2325)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2328= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2329= Cyc_PP_text(({ unsigned char* _temp2340="else {";
struct _tagged_string _temp2341; _temp2341.curr= _temp2340; _temp2341.base=
_temp2340; _temp2341.last_plus_one= _temp2340 + 7; _temp2341;})); struct Cyc_PP_Doc*
_temp2330= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp2335)( struct
_tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2338= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2339= Cyc_Absynpp_stmt2doc( _temp2163); struct Cyc_PP_Doc*
_temp2336[ 2u]={ _temp2338, _temp2339}; struct _tagged_ptr0 _temp2337={
_temp2336, _temp2336, _temp2336 + 2u}; _temp2335( _temp2337);})); struct Cyc_PP_Doc*
_temp2331= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2332= Cyc_PP_text(({
unsigned char* _temp2333="}"; struct _tagged_string _temp2334; _temp2334.curr=
_temp2333; _temp2334.base= _temp2333; _temp2334.last_plus_one= _temp2333 + 2;
_temp2334;})); struct Cyc_PP_Doc* _temp2326[ 5u]={ _temp2328, _temp2329,
_temp2330, _temp2331, _temp2332}; struct _tagged_ptr0 _temp2327={ _temp2326,
_temp2326, _temp2326 + 5u}; _temp2325( _temp2327);}): Cyc_PP_nil_doc(); struct
Cyc_PP_Doc* _temp2316[ 7u]={ _temp2318, _temp2319, _temp2320, _temp2321,
_temp2322, _temp2323, _temp2324}; struct _tagged_ptr0 _temp2317={ _temp2316,
_temp2316, _temp2316 + 7u}; _temp2315( _temp2317);}); goto _LL2114;} _LL2126: s=({
struct Cyc_PP_Doc*(* _temp2353)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2356= Cyc_PP_text(({ unsigned char* _temp2371="while ("; struct
_tagged_string _temp2372; _temp2372.curr= _temp2371; _temp2372.base= _temp2371;
_temp2372.last_plus_one= _temp2371 + 8; _temp2372;})); struct Cyc_PP_Doc*
_temp2357= Cyc_Absynpp_exp2doc( _temp2175); struct Cyc_PP_Doc* _temp2358= Cyc_PP_text(({
unsigned char* _temp2369=") {"; struct _tagged_string _temp2370; _temp2370.curr=
_temp2369; _temp2370.base= _temp2369; _temp2370.last_plus_one= _temp2369 + 4;
_temp2370;})); struct Cyc_PP_Doc* _temp2359= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(*
_temp2364)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2367= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2368= Cyc_Absynpp_stmt2doc( _temp2169); struct Cyc_PP_Doc*
_temp2365[ 2u]={ _temp2367, _temp2368}; struct _tagged_ptr0 _temp2366={
_temp2365, _temp2365, _temp2365 + 2u}; _temp2364( _temp2366);})); struct Cyc_PP_Doc*
_temp2360= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2361= Cyc_PP_text(({
unsigned char* _temp2362="}"; struct _tagged_string _temp2363; _temp2363.curr=
_temp2362; _temp2363.base= _temp2362; _temp2363.last_plus_one= _temp2362 + 2;
_temp2363;})); struct Cyc_PP_Doc* _temp2354[ 6u]={ _temp2356, _temp2357,
_temp2358, _temp2359, _temp2360, _temp2361}; struct _tagged_ptr0 _temp2355={
_temp2354, _temp2354, _temp2354 + 6u}; _temp2353( _temp2355);}); goto _LL2114;
_LL2128: s= Cyc_PP_text(({ unsigned char* _temp2373="break;"; struct
_tagged_string _temp2374; _temp2374.curr= _temp2373; _temp2374.base= _temp2373;
_temp2374.last_plus_one= _temp2373 + 7; _temp2374;})); goto _LL2114; _LL2130: s=
Cyc_PP_text(({ unsigned char* _temp2375="continue;"; struct _tagged_string
_temp2376; _temp2376.curr= _temp2375; _temp2376.base= _temp2375; _temp2376.last_plus_one=
_temp2375 + 10; _temp2376;})); goto _LL2114; _LL2132: s= Cyc_PP_text(({ struct
_tagged_string _temp2377=* _temp2183; xprintf("goto %.*s;", _temp2377.last_plus_one
- _temp2377.curr, _temp2377.curr);})); goto _LL2114; _LL2134: s=({ struct Cyc_PP_Doc*(*
_temp2378)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2381= Cyc_PP_text(({
unsigned char* _temp2404="for("; struct _tagged_string _temp2405; _temp2405.curr=
_temp2404; _temp2405.base= _temp2404; _temp2405.last_plus_one= _temp2404 + 5;
_temp2405;})); struct Cyc_PP_Doc* _temp2382= Cyc_Absynpp_exp2doc( _temp2199);
struct Cyc_PP_Doc* _temp2383= Cyc_PP_text(({ unsigned char* _temp2402="; ";
struct _tagged_string _temp2403; _temp2403.curr= _temp2402; _temp2403.base=
_temp2402; _temp2403.last_plus_one= _temp2402 + 3; _temp2403;})); struct Cyc_PP_Doc*
_temp2384= Cyc_Absynpp_exp2doc( _temp2197); struct Cyc_PP_Doc* _temp2385= Cyc_PP_text(({
unsigned char* _temp2400="; "; struct _tagged_string _temp2401; _temp2401.curr=
_temp2400; _temp2401.base= _temp2400; _temp2401.last_plus_one= _temp2400 + 3;
_temp2401;})); struct Cyc_PP_Doc* _temp2386= Cyc_Absynpp_exp2doc( _temp2191);
struct Cyc_PP_Doc* _temp2387= Cyc_PP_text(({ unsigned char* _temp2398=") {";
struct _tagged_string _temp2399; _temp2399.curr= _temp2398; _temp2399.base=
_temp2398; _temp2399.last_plus_one= _temp2398 + 4; _temp2399;})); struct Cyc_PP_Doc*
_temp2388= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp2393)( struct
_tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2396= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2397= Cyc_Absynpp_stmt2doc( _temp2185); struct Cyc_PP_Doc*
_temp2394[ 2u]={ _temp2396, _temp2397}; struct _tagged_ptr0 _temp2395={
_temp2394, _temp2394, _temp2394 + 2u}; _temp2393( _temp2395);})); struct Cyc_PP_Doc*
_temp2389= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2390= Cyc_PP_text(({
unsigned char* _temp2391="}"; struct _tagged_string _temp2392; _temp2392.curr=
_temp2391; _temp2392.base= _temp2391; _temp2392.last_plus_one= _temp2391 + 2;
_temp2392;})); struct Cyc_PP_Doc* _temp2379[ 10u]={ _temp2381, _temp2382,
_temp2383, _temp2384, _temp2385, _temp2386, _temp2387, _temp2388, _temp2389,
_temp2390}; struct _tagged_ptr0 _temp2380={ _temp2379, _temp2379, _temp2379 + 10u};
_temp2378( _temp2380);}); goto _LL2114; _LL2136: s=({ struct Cyc_PP_Doc*(*
_temp2406)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2409= Cyc_PP_text(({
unsigned char* _temp2420="switch ("; struct _tagged_string _temp2421; _temp2421.curr=
_temp2420; _temp2421.base= _temp2420; _temp2421.last_plus_one= _temp2420 + 9;
_temp2421;})); struct Cyc_PP_Doc* _temp2410= Cyc_Absynpp_exp2doc( _temp2203);
struct Cyc_PP_Doc* _temp2411= Cyc_PP_text(({ unsigned char* _temp2418=") {";
struct _tagged_string _temp2419; _temp2419.curr= _temp2418; _temp2419.base=
_temp2418; _temp2419.last_plus_one= _temp2418 + 4; _temp2419;})); struct Cyc_PP_Doc*
_temp2412= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2413= Cyc_Absynpp_switchclauses2doc(
_temp2201); struct Cyc_PP_Doc* _temp2414= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp2415= Cyc_PP_text(({ unsigned char* _temp2416="}"; struct _tagged_string
_temp2417; _temp2417.curr= _temp2416; _temp2417.base= _temp2416; _temp2417.last_plus_one=
_temp2416 + 2; _temp2417;})); struct Cyc_PP_Doc* _temp2407[ 7u]={ _temp2409,
_temp2410, _temp2411, _temp2412, _temp2413, _temp2414, _temp2415}; struct
_tagged_ptr0 _temp2408={ _temp2407, _temp2407, _temp2407 + 7u}; _temp2406(
_temp2408);}); goto _LL2114; _LL2138: s= Cyc_PP_text(({ unsigned char* _temp2422="fallthru;";
struct _tagged_string _temp2423; _temp2423.curr= _temp2422; _temp2423.base=
_temp2422; _temp2423.last_plus_one= _temp2422 + 10; _temp2423;})); goto _LL2114;
_LL2140: s=({ struct Cyc_PP_Doc*(* _temp2424)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2427= Cyc_PP_text(({ unsigned char* _temp2432="fallthru(";
struct _tagged_string _temp2433; _temp2433.curr= _temp2432; _temp2433.base=
_temp2432; _temp2433.last_plus_one= _temp2432 + 10; _temp2433;})); struct Cyc_PP_Doc*
_temp2428= Cyc_Absynpp_exps2doc_prec( 20, _temp2211); struct Cyc_PP_Doc*
_temp2429= Cyc_PP_text(({ unsigned char* _temp2430=");"; struct _tagged_string
_temp2431; _temp2431.curr= _temp2430; _temp2431.base= _temp2430; _temp2431.last_plus_one=
_temp2430 + 3; _temp2431;})); struct Cyc_PP_Doc* _temp2425[ 3u]={ _temp2427,
_temp2428, _temp2429}; struct _tagged_ptr0 _temp2426={ _temp2425, _temp2425,
_temp2425 + 3u}; _temp2424( _temp2426);}); goto _LL2114; _LL2142: s=({ struct
Cyc_PP_Doc*(* _temp2434)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2437= Cyc_Absynpp_decl2doc( _temp2215); struct Cyc_PP_Doc* _temp2438= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2439= Cyc_Absynpp_stmt2doc( _temp2213); struct Cyc_PP_Doc*
_temp2435[ 3u]={ _temp2437, _temp2438, _temp2439}; struct _tagged_ptr0 _temp2436={
_temp2435, _temp2435, _temp2435 + 3u}; _temp2434( _temp2436);}); goto _LL2114;
_LL2144: s=({ struct Cyc_PP_Doc*(* _temp2440)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2443= Cyc_PP_text(({ unsigned char* _temp2445="cut ";
struct _tagged_string _temp2446; _temp2446.curr= _temp2445; _temp2446.base=
_temp2445; _temp2446.last_plus_one= _temp2445 + 5; _temp2446;})); struct Cyc_PP_Doc*
_temp2444= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp2217)); struct Cyc_PP_Doc*
_temp2441[ 2u]={ _temp2443, _temp2444}; struct _tagged_ptr0 _temp2442={
_temp2441, _temp2441, _temp2441 + 2u}; _temp2440( _temp2442);}); goto _LL2114;
_LL2146: s=({ struct Cyc_PP_Doc*(* _temp2447)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2450= Cyc_PP_text(({ unsigned char* _temp2452="splice ";
struct _tagged_string _temp2453; _temp2453.curr= _temp2452; _temp2453.base=
_temp2452; _temp2453.last_plus_one= _temp2452 + 8; _temp2453;})); struct Cyc_PP_Doc*
_temp2451= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp2219)); struct Cyc_PP_Doc*
_temp2448[ 2u]={ _temp2450, _temp2451}; struct _tagged_ptr0 _temp2449={
_temp2448, _temp2448, _temp2448 + 2u}; _temp2447( _temp2449);}); goto _LL2114;
_LL2148: if( Cyc_Absynpp_decls_first? Cyc_Absynpp_is_declaration( _temp2221): 0){
s=({ struct Cyc_PP_Doc*(* _temp2454)( struct _tagged_ptr0)= Cyc_PP_cat; struct
Cyc_PP_Doc* _temp2457= Cyc_PP_textptr( _temp2223); struct Cyc_PP_Doc* _temp2458=
Cyc_PP_text(({ unsigned char* _temp2465=": {"; struct _tagged_string _temp2466;
_temp2466.curr= _temp2465; _temp2466.base= _temp2465; _temp2466.last_plus_one=
_temp2465 + 4; _temp2466;})); struct Cyc_PP_Doc* _temp2459= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2460= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp2221));
struct Cyc_PP_Doc* _temp2461= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2462=
Cyc_PP_text(({ unsigned char* _temp2463="}"; struct _tagged_string _temp2464;
_temp2464.curr= _temp2463; _temp2464.base= _temp2463; _temp2464.last_plus_one=
_temp2463 + 2; _temp2464;})); struct Cyc_PP_Doc* _temp2455[ 6u]={ _temp2457,
_temp2458, _temp2459, _temp2460, _temp2461, _temp2462}; struct _tagged_ptr0
_temp2456={ _temp2455, _temp2455, _temp2455 + 6u}; _temp2454( _temp2456);});}
else{ s=({ struct Cyc_PP_Doc*(* _temp2467)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2470= Cyc_PP_textptr( _temp2223); struct Cyc_PP_Doc*
_temp2471= Cyc_PP_text(({ unsigned char* _temp2473=": "; struct _tagged_string
_temp2474; _temp2474.curr= _temp2473; _temp2474.base= _temp2473; _temp2474.last_plus_one=
_temp2473 + 3; _temp2474;})); struct Cyc_PP_Doc* _temp2472= Cyc_Absynpp_stmt2doc(
_temp2221); struct Cyc_PP_Doc* _temp2468[ 3u]={ _temp2470, _temp2471, _temp2472};
struct _tagged_ptr0 _temp2469={ _temp2468, _temp2468, _temp2468 + 3u}; _temp2467(
_temp2469);});} goto _LL2114; _LL2150: s=({ struct Cyc_PP_Doc*(* _temp2475)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2478= Cyc_PP_text(({
unsigned char* _temp2489="do {"; struct _tagged_string _temp2490; _temp2490.curr=
_temp2489; _temp2490.base= _temp2489; _temp2490.last_plus_one= _temp2489 + 5;
_temp2490;})); struct Cyc_PP_Doc* _temp2479= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp2480= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp2231)); struct Cyc_PP_Doc*
_temp2481= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2482= Cyc_PP_text(({
unsigned char* _temp2487="} while ("; struct _tagged_string _temp2488; _temp2488.curr=
_temp2487; _temp2488.base= _temp2487; _temp2488.last_plus_one= _temp2487 + 10;
_temp2488;})); struct Cyc_PP_Doc* _temp2483= Cyc_Absynpp_exp2doc( _temp2229);
struct Cyc_PP_Doc* _temp2484= Cyc_PP_text(({ unsigned char* _temp2485=");";
struct _tagged_string _temp2486; _temp2486.curr= _temp2485; _temp2486.base=
_temp2485; _temp2486.last_plus_one= _temp2485 + 3; _temp2486;})); struct Cyc_PP_Doc*
_temp2476[ 7u]={ _temp2478, _temp2479, _temp2480, _temp2481, _temp2482,
_temp2483, _temp2484}; struct _tagged_ptr0 _temp2477={ _temp2476, _temp2476,
_temp2476 + 7u}; _temp2475( _temp2477);}); goto _LL2114; _LL2152: s=({ struct
Cyc_PP_Doc*(* _temp2491)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2494= Cyc_PP_text(({ unsigned char* _temp2507="try {"; struct
_tagged_string _temp2508; _temp2508.curr= _temp2507; _temp2508.base= _temp2507;
_temp2508.last_plus_one= _temp2507 + 6; _temp2508;})); struct Cyc_PP_Doc*
_temp2495= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2496= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp2235)); struct Cyc_PP_Doc* _temp2497= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp2498= Cyc_PP_text(({ unsigned char* _temp2505="} catch {"; struct
_tagged_string _temp2506; _temp2506.curr= _temp2505; _temp2506.base= _temp2505;
_temp2506.last_plus_one= _temp2505 + 10; _temp2506;})); struct Cyc_PP_Doc*
_temp2499= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2500= Cyc_PP_nest( 2, Cyc_Absynpp_switchclauses2doc(
_temp2233)); struct Cyc_PP_Doc* _temp2501= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp2502= Cyc_PP_text(({ unsigned char* _temp2503="}"; struct _tagged_string
_temp2504; _temp2504.curr= _temp2503; _temp2504.base= _temp2503; _temp2504.last_plus_one=
_temp2503 + 2; _temp2504;})); struct Cyc_PP_Doc* _temp2492[ 9u]={ _temp2494,
_temp2495, _temp2496, _temp2497, _temp2498, _temp2499, _temp2500, _temp2501,
_temp2502}; struct _tagged_ptr0 _temp2493={ _temp2492, _temp2492, _temp2492 + 9u};
_temp2491( _temp2493);}); goto _LL2114; _LL2154: s=({ struct Cyc_PP_Doc*(*
_temp2509)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2512= Cyc_PP_text(({
unsigned char* _temp2527="region<"; struct _tagged_string _temp2528; _temp2528.curr=
_temp2527; _temp2528.base= _temp2527; _temp2528.last_plus_one= _temp2527 + 8;
_temp2528;})); struct Cyc_PP_Doc* _temp2513= Cyc_PP_textptr( Cyc_Absynpp_get_name(
_temp2241)); struct Cyc_PP_Doc* _temp2514= Cyc_PP_text(({ unsigned char*
_temp2525=">"; struct _tagged_string _temp2526; _temp2526.curr= _temp2525;
_temp2526.base= _temp2525; _temp2526.last_plus_one= _temp2525 + 2; _temp2526;}));
struct Cyc_PP_Doc* _temp2515= Cyc_Absynpp_qvar2doc( _temp2239->name); struct Cyc_PP_Doc*
_temp2516= Cyc_PP_text(({ unsigned char* _temp2523="{"; struct _tagged_string
_temp2524; _temp2524.curr= _temp2523; _temp2524.base= _temp2523; _temp2524.last_plus_one=
_temp2523 + 2; _temp2524;})); struct Cyc_PP_Doc* _temp2517= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2518= Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp2237));
struct Cyc_PP_Doc* _temp2519= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2520=
Cyc_PP_text(({ unsigned char* _temp2521="}"; struct _tagged_string _temp2522;
_temp2522.curr= _temp2521; _temp2522.base= _temp2521; _temp2522.last_plus_one=
_temp2521 + 2; _temp2522;})); struct Cyc_PP_Doc* _temp2510[ 9u]={ _temp2512,
_temp2513, _temp2514, _temp2515, _temp2516, _temp2517, _temp2518, _temp2519,
_temp2520}; struct _tagged_ptr0 _temp2511={ _temp2510, _temp2510, _temp2510 + 9u};
_temp2509( _temp2511);}); goto _LL2114; _LL2114:;} return s;} struct Cyc_PP_Doc*
Cyc_Absynpp_pat2doc( struct Cyc_Absyn_Pat* p){ struct Cyc_PP_Doc* s;{ void*
_temp2529=( void*) p->r; int _temp2561; void* _temp2563; unsigned char _temp2565;
struct _tagged_string _temp2567; struct Cyc_Absyn_Vardecl* _temp2569; struct Cyc_List_List*
_temp2571; struct Cyc_Absyn_Pat* _temp2573; struct Cyc_Absyn_Vardecl* _temp2575;
struct _tuple0* _temp2577; struct Cyc_List_List* _temp2579; struct Cyc_List_List*
_temp2581; struct _tuple0* _temp2583; struct Cyc_List_List* _temp2585; struct
Cyc_List_List* _temp2587; struct _tuple0* _temp2589; struct Cyc_List_List*
_temp2591; struct Cyc_List_List* _temp2593; struct Cyc_Core_Opt* _temp2595;
struct Cyc_Absyn_Structdecl* _temp2597; struct Cyc_Absyn_Enumfield* _temp2599;
struct Cyc_Absyn_Enumdecl* _temp2601; struct Cyc_List_List* _temp2603; struct
Cyc_List_List* _temp2605; struct Cyc_Absyn_Tunionfield* _temp2607; struct Cyc_Absyn_Tuniondecl*
_temp2609; _LL2531: if( _temp2529 ==( void*) Cyc_Absyn_Wild_p){ goto _LL2532;}
else{ goto _LL2533;} _LL2533: if( _temp2529 ==( void*) Cyc_Absyn_Null_p){ goto
_LL2534;} else{ goto _LL2535;} _LL2535: if(( unsigned int) _temp2529 > 2u?*((
int*) _temp2529) == Cyc_Absyn_Int_p: 0){ _LL2564: _temp2563=( void*)(( struct
Cyc_Absyn_Int_p_struct*) _temp2529)->f1; goto _LL2562; _LL2562: _temp2561=((
struct Cyc_Absyn_Int_p_struct*) _temp2529)->f2; goto _LL2536;} else{ goto
_LL2537;} _LL2537: if(( unsigned int) _temp2529 > 2u?*(( int*) _temp2529) == Cyc_Absyn_Char_p:
0){ _LL2566: _temp2565=(( struct Cyc_Absyn_Char_p_struct*) _temp2529)->f1; goto
_LL2538;} else{ goto _LL2539;} _LL2539: if(( unsigned int) _temp2529 > 2u?*((
int*) _temp2529) == Cyc_Absyn_Float_p: 0){ _LL2568: _temp2567=(( struct Cyc_Absyn_Float_p_struct*)
_temp2529)->f1; goto _LL2540;} else{ goto _LL2541;} _LL2541: if(( unsigned int)
_temp2529 > 2u?*(( int*) _temp2529) == Cyc_Absyn_Var_p: 0){ _LL2570: _temp2569=((
struct Cyc_Absyn_Var_p_struct*) _temp2529)->f1; goto _LL2542;} else{ goto
_LL2543;} _LL2543: if(( unsigned int) _temp2529 > 2u?*(( int*) _temp2529) == Cyc_Absyn_Tuple_p:
0){ _LL2572: _temp2571=(( struct Cyc_Absyn_Tuple_p_struct*) _temp2529)->f1; goto
_LL2544;} else{ goto _LL2545;} _LL2545: if(( unsigned int) _temp2529 > 2u?*((
int*) _temp2529) == Cyc_Absyn_Pointer_p: 0){ _LL2574: _temp2573=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp2529)->f1; goto _LL2546;} else{ goto _LL2547;} _LL2547: if(( unsigned int)
_temp2529 > 2u?*(( int*) _temp2529) == Cyc_Absyn_Reference_p: 0){ _LL2576:
_temp2575=(( struct Cyc_Absyn_Reference_p_struct*) _temp2529)->f1; goto _LL2548;}
else{ goto _LL2549;} _LL2549: if(( unsigned int) _temp2529 > 2u?*(( int*)
_temp2529) == Cyc_Absyn_UnknownId_p: 0){ _LL2578: _temp2577=(( struct Cyc_Absyn_UnknownId_p_struct*)
_temp2529)->f1; goto _LL2550;} else{ goto _LL2551;} _LL2551: if(( unsigned int)
_temp2529 > 2u?*(( int*) _temp2529) == Cyc_Absyn_UnknownCall_p: 0){ _LL2584:
_temp2583=(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp2529)->f1; goto
_LL2582; _LL2582: _temp2581=(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp2529)->f2;
goto _LL2580; _LL2580: _temp2579=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp2529)->f3; goto _LL2552;} else{ goto _LL2553;} _LL2553: if(( unsigned int)
_temp2529 > 2u?*(( int*) _temp2529) == Cyc_Absyn_UnknownFields_p: 0){ _LL2590:
_temp2589=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp2529)->f1; goto
_LL2588; _LL2588: _temp2587=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp2529)->f2; goto _LL2586; _LL2586: _temp2585=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp2529)->f3; goto _LL2554;} else{ goto _LL2555;} _LL2555: if(( unsigned int)
_temp2529 > 2u?*(( int*) _temp2529) == Cyc_Absyn_Struct_p: 0){ _LL2598:
_temp2597=(( struct Cyc_Absyn_Struct_p_struct*) _temp2529)->f1; goto _LL2596;
_LL2596: _temp2595=(( struct Cyc_Absyn_Struct_p_struct*) _temp2529)->f2; goto
_LL2594; _LL2594: _temp2593=(( struct Cyc_Absyn_Struct_p_struct*) _temp2529)->f3;
goto _LL2592; _LL2592: _temp2591=(( struct Cyc_Absyn_Struct_p_struct*) _temp2529)->f4;
goto _LL2556;} else{ goto _LL2557;} _LL2557: if(( unsigned int) _temp2529 > 2u?*((
int*) _temp2529) == Cyc_Absyn_Enum_p: 0){ _LL2602: _temp2601=(( struct Cyc_Absyn_Enum_p_struct*)
_temp2529)->f1; goto _LL2600; _LL2600: _temp2599=(( struct Cyc_Absyn_Enum_p_struct*)
_temp2529)->f2; goto _LL2558;} else{ goto _LL2559;} _LL2559: if(( unsigned int)
_temp2529 > 2u?*(( int*) _temp2529) == Cyc_Absyn_Tunion_p: 0){ _LL2610:
_temp2609=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2529)->f1; goto _LL2608;
_LL2608: _temp2607=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2529)->f2; goto
_LL2606; _LL2606: _temp2605=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2529)->f3;
goto _LL2604; _LL2604: _temp2603=(( struct Cyc_Absyn_Tunion_p_struct*) _temp2529)->f4;
goto _LL2560;} else{ goto _LL2530;} _LL2532: s= Cyc_PP_text(({ unsigned char*
_temp2611="_"; struct _tagged_string _temp2612; _temp2612.curr= _temp2611;
_temp2612.base= _temp2611; _temp2612.last_plus_one= _temp2611 + 2; _temp2612;}));
goto _LL2530; _LL2534: s= Cyc_PP_text(({ unsigned char* _temp2613="null"; struct
_tagged_string _temp2614; _temp2614.curr= _temp2613; _temp2614.base= _temp2613;
_temp2614.last_plus_one= _temp2613 + 5; _temp2614;})); goto _LL2530; _LL2536:
if( _temp2563 ==( void*) Cyc_Absyn_Signed){ s= Cyc_PP_text( xprintf("%d",
_temp2561));} else{ s= Cyc_PP_text( xprintf("%u",( unsigned int) _temp2561));}
goto _LL2530; _LL2538: s= Cyc_PP_text(({ struct _tagged_string _temp2615= Cyc_Absynpp_char_escape(
_temp2565); xprintf("'%.*s'", _temp2615.last_plus_one - _temp2615.curr,
_temp2615.curr);})); goto _LL2530; _LL2540: s= Cyc_PP_text( _temp2567); goto
_LL2530; _LL2542: s= Cyc_Absynpp_qvar2doc( _temp2569->name); goto _LL2530;
_LL2544: s=({ struct Cyc_PP_Doc*(* _temp2616)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2619= Cyc_PP_text(({ unsigned char* _temp2626="$(";
struct _tagged_string _temp2627; _temp2627.curr= _temp2626; _temp2627.base=
_temp2626; _temp2627.last_plus_one= _temp2626 + 3; _temp2627;})); struct Cyc_PP_Doc*
_temp2620=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Pat*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseq)( Cyc_Absynpp_pat2doc,({
unsigned char* _temp2624=","; struct _tagged_string _temp2625; _temp2625.curr=
_temp2624; _temp2625.base= _temp2624; _temp2625.last_plus_one= _temp2624 + 2;
_temp2625;}), _temp2571); struct Cyc_PP_Doc* _temp2621= Cyc_PP_text(({
unsigned char* _temp2622=")"; struct _tagged_string _temp2623; _temp2623.curr=
_temp2622; _temp2623.base= _temp2622; _temp2623.last_plus_one= _temp2622 + 2;
_temp2623;})); struct Cyc_PP_Doc* _temp2617[ 3u]={ _temp2619, _temp2620,
_temp2621}; struct _tagged_ptr0 _temp2618={ _temp2617, _temp2617, _temp2617 + 3u};
_temp2616( _temp2618);}); goto _LL2530; _LL2546: s=({ struct Cyc_PP_Doc*(*
_temp2628)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2631= Cyc_PP_text(({
unsigned char* _temp2633="&"; struct _tagged_string _temp2634; _temp2634.curr=
_temp2633; _temp2634.base= _temp2633; _temp2634.last_plus_one= _temp2633 + 2;
_temp2634;})); struct Cyc_PP_Doc* _temp2632= Cyc_Absynpp_pat2doc( _temp2573);
struct Cyc_PP_Doc* _temp2629[ 2u]={ _temp2631, _temp2632}; struct _tagged_ptr0
_temp2630={ _temp2629, _temp2629, _temp2629 + 2u}; _temp2628( _temp2630);});
goto _LL2530; _LL2548: s=({ struct Cyc_PP_Doc*(* _temp2635)( struct _tagged_ptr0)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp2638= Cyc_PP_text(({ unsigned char*
_temp2640="*"; struct _tagged_string _temp2641; _temp2641.curr= _temp2640;
_temp2641.base= _temp2640; _temp2641.last_plus_one= _temp2640 + 2; _temp2641;}));
struct Cyc_PP_Doc* _temp2639= Cyc_Absynpp_qvar2doc( _temp2575->name); struct Cyc_PP_Doc*
_temp2636[ 2u]={ _temp2638, _temp2639}; struct _tagged_ptr0 _temp2637={
_temp2636, _temp2636, _temp2636 + 2u}; _temp2635( _temp2637);}); goto _LL2530;
_LL2550: s= Cyc_Absynpp_qvar2doc( _temp2577); goto _LL2530; _LL2552: s=({ struct
Cyc_PP_Doc*(* _temp2642)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp2645= Cyc_Absynpp_qvar2doc( _temp2583); struct Cyc_PP_Doc* _temp2646= Cyc_Absynpp_tvars2doc(
_temp2581); struct Cyc_PP_Doc* _temp2647= Cyc_PP_group(({ unsigned char*
_temp2648="("; struct _tagged_string _temp2649; _temp2649.curr= _temp2648;
_temp2649.base= _temp2648; _temp2649.last_plus_one= _temp2648 + 2; _temp2649;}),({
unsigned char* _temp2650=")"; struct _tagged_string _temp2651; _temp2651.curr=
_temp2650; _temp2651.base= _temp2650; _temp2651.last_plus_one= _temp2650 + 2;
_temp2651;}),({ unsigned char* _temp2652=","; struct _tagged_string _temp2653;
_temp2653.curr= _temp2652; _temp2653.base= _temp2652; _temp2653.last_plus_one=
_temp2652 + 2; _temp2653;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_pat2doc,
_temp2579)); struct Cyc_PP_Doc* _temp2643[ 3u]={ _temp2645, _temp2646, _temp2647};
struct _tagged_ptr0 _temp2644={ _temp2643, _temp2643, _temp2643 + 3u}; _temp2642(
_temp2644);}); goto _LL2530; _LL2554: s=({ struct Cyc_PP_Doc*(* _temp2654)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2657= Cyc_Absynpp_qvar2doc(
_temp2589); struct Cyc_PP_Doc* _temp2658= Cyc_Absynpp_tvars2doc( _temp2587);
struct Cyc_PP_Doc* _temp2659= Cyc_PP_group(({ unsigned char* _temp2660="{";
struct _tagged_string _temp2661; _temp2661.curr= _temp2660; _temp2661.base=
_temp2660; _temp2661.last_plus_one= _temp2660 + 2; _temp2661;}),({ unsigned char*
_temp2662="}"; struct _tagged_string _temp2663; _temp2663.curr= _temp2662;
_temp2663.base= _temp2662; _temp2663.last_plus_one= _temp2662 + 2; _temp2663;}),({
unsigned char* _temp2664=","; struct _tagged_string _temp2665; _temp2665.curr=
_temp2664; _temp2665.base= _temp2664; _temp2665.last_plus_one= _temp2664 + 2;
_temp2665;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tuple5*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_dp2doc,
_temp2585)); struct Cyc_PP_Doc* _temp2655[ 3u]={ _temp2657, _temp2658, _temp2659};
struct _tagged_ptr0 _temp2656={ _temp2655, _temp2655, _temp2655 + 3u}; _temp2654(
_temp2656);}); goto _LL2530; _LL2556: s=({ struct Cyc_PP_Doc*(* _temp2666)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2669= _temp2597->name
== 0? Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( _temp2597->name))->v); struct Cyc_PP_Doc* _temp2670= Cyc_Absynpp_tvars2doc(
_temp2593); struct Cyc_PP_Doc* _temp2671= Cyc_PP_group(({ unsigned char*
_temp2672="{"; struct _tagged_string _temp2673; _temp2673.curr= _temp2672;
_temp2673.base= _temp2672; _temp2673.last_plus_one= _temp2672 + 2; _temp2673;}),({
unsigned char* _temp2674="}"; struct _tagged_string _temp2675; _temp2675.curr=
_temp2674; _temp2675.base= _temp2674; _temp2675.last_plus_one= _temp2674 + 2;
_temp2675;}),({ unsigned char* _temp2676=","; struct _tagged_string _temp2677;
_temp2677.curr= _temp2676; _temp2677.base= _temp2676; _temp2677.last_plus_one=
_temp2676 + 2; _temp2677;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_dp2doc,
_temp2591)); struct Cyc_PP_Doc* _temp2667[ 3u]={ _temp2669, _temp2670, _temp2671};
struct _tagged_ptr0 _temp2668={ _temp2667, _temp2667, _temp2667 + 3u}; _temp2666(
_temp2668);}); goto _LL2530; _LL2558: s= Cyc_Absynpp_qvar2doc( _temp2599->name);
goto _LL2530; _LL2560: if( _temp2603 == 0){ s= Cyc_Absynpp_qvar2doc( _temp2607->name);}
else{ s=({ struct Cyc_PP_Doc*(* _temp2678)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2681= Cyc_Absynpp_qvar2doc( _temp2607->name); struct Cyc_PP_Doc*
_temp2682= Cyc_Absynpp_tvars2doc( _temp2605); struct Cyc_PP_Doc* _temp2683= Cyc_PP_egroup(({
unsigned char* _temp2684="("; struct _tagged_string _temp2685; _temp2685.curr=
_temp2684; _temp2685.base= _temp2684; _temp2685.last_plus_one= _temp2684 + 2;
_temp2685;}),({ unsigned char* _temp2686=")"; struct _tagged_string _temp2687;
_temp2687.curr= _temp2686; _temp2687.base= _temp2686; _temp2687.last_plus_one=
_temp2686 + 2; _temp2687;}),({ unsigned char* _temp2688=","; struct
_tagged_string _temp2689; _temp2689.curr= _temp2688; _temp2689.base= _temp2688;
_temp2689.last_plus_one= _temp2688 + 2; _temp2689;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_pat2doc, _temp2603)); struct Cyc_PP_Doc* _temp2679[ 3u]={ _temp2681,
_temp2682, _temp2683}; struct _tagged_ptr0 _temp2680={ _temp2679, _temp2679,
_temp2679 + 3u}; _temp2678( _temp2680);});} goto _LL2530; _LL2530:;} return s;}
struct Cyc_PP_Doc* Cyc_Absynpp_dp2doc( struct _tuple5* dp){ return({ struct Cyc_PP_Doc*(*
_temp2690)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2693= Cyc_PP_egroup(({
unsigned char* _temp2695=""; struct _tagged_string _temp2696; _temp2696.curr=
_temp2695; _temp2696.base= _temp2695; _temp2696.last_plus_one= _temp2695 + 1;
_temp2696;}),({ unsigned char* _temp2697="="; struct _tagged_string _temp2698;
_temp2698.curr= _temp2697; _temp2698.base= _temp2697; _temp2698.last_plus_one=
_temp2697 + 2; _temp2698;}),({ unsigned char* _temp2699="="; struct
_tagged_string _temp2700; _temp2700.curr= _temp2699; _temp2700.base= _temp2699;
_temp2700.last_plus_one= _temp2699 + 2; _temp2700;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(*
dp).f1)); struct Cyc_PP_Doc* _temp2694= Cyc_Absynpp_pat2doc((* dp).f2); struct
Cyc_PP_Doc* _temp2691[ 2u]={ _temp2693, _temp2694}; struct _tagged_ptr0
_temp2692={ _temp2691, _temp2691, _temp2691 + 2u}; _temp2690( _temp2692);});}
struct Cyc_PP_Doc* Cyc_Absynpp_switchclause2doc( struct Cyc_Absyn_Switch_clause*
c){ if( c->where_clause == 0?( void*)( c->pattern)->r ==( void*) Cyc_Absyn_Wild_p:
0){ return({ struct Cyc_PP_Doc*(* _temp2701)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2704= Cyc_PP_text(({ unsigned char* _temp2711="default: ";
struct _tagged_string _temp2712; _temp2712.curr= _temp2711; _temp2712.base=
_temp2711; _temp2712.last_plus_one= _temp2711 + 10; _temp2712;})); struct Cyc_PP_Doc*
_temp2705= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp2706)( struct
_tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2709= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2710= Cyc_Absynpp_stmt2doc( c->body); struct Cyc_PP_Doc*
_temp2707[ 2u]={ _temp2709, _temp2710}; struct _tagged_ptr0 _temp2708={
_temp2707, _temp2707, _temp2707 + 2u}; _temp2706( _temp2708);})); struct Cyc_PP_Doc*
_temp2702[ 2u]={ _temp2704, _temp2705}; struct _tagged_ptr0 _temp2703={
_temp2702, _temp2702, _temp2702 + 2u}; _temp2701( _temp2703);});} else{ if( c->where_clause
== 0){ return({ struct Cyc_PP_Doc*(* _temp2713)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2716= Cyc_PP_text(({ unsigned char* _temp2727="case ";
struct _tagged_string _temp2728; _temp2728.curr= _temp2727; _temp2728.base=
_temp2727; _temp2728.last_plus_one= _temp2727 + 6; _temp2728;})); struct Cyc_PP_Doc*
_temp2717= Cyc_Absynpp_pat2doc( c->pattern); struct Cyc_PP_Doc* _temp2718= Cyc_PP_text(({
unsigned char* _temp2725=": "; struct _tagged_string _temp2726; _temp2726.curr=
_temp2725; _temp2726.base= _temp2725; _temp2726.last_plus_one= _temp2725 + 3;
_temp2726;})); struct Cyc_PP_Doc* _temp2719= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(*
_temp2720)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2723= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2724= Cyc_Absynpp_stmt2doc( c->body); struct Cyc_PP_Doc*
_temp2721[ 2u]={ _temp2723, _temp2724}; struct _tagged_ptr0 _temp2722={
_temp2721, _temp2721, _temp2721 + 2u}; _temp2720( _temp2722);})); struct Cyc_PP_Doc*
_temp2714[ 4u]={ _temp2716, _temp2717, _temp2718, _temp2719}; struct
_tagged_ptr0 _temp2715={ _temp2714, _temp2714, _temp2714 + 4u}; _temp2713(
_temp2715);});} else{ return({ struct Cyc_PP_Doc*(* _temp2729)( struct
_tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2732= Cyc_PP_text(({
unsigned char* _temp2747="case "; struct _tagged_string _temp2748; _temp2748.curr=
_temp2747; _temp2748.base= _temp2747; _temp2748.last_plus_one= _temp2747 + 6;
_temp2748;})); struct Cyc_PP_Doc* _temp2733= Cyc_Absynpp_pat2doc( c->pattern);
struct Cyc_PP_Doc* _temp2734= Cyc_PP_text(({ unsigned char* _temp2745=" && ";
struct _tagged_string _temp2746; _temp2746.curr= _temp2745; _temp2746.base=
_temp2745; _temp2746.last_plus_one= _temp2745 + 5; _temp2746;})); struct Cyc_PP_Doc*
_temp2735= Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null( c->where_clause));
struct Cyc_PP_Doc* _temp2736= Cyc_PP_text(({ unsigned char* _temp2743=": ";
struct _tagged_string _temp2744; _temp2744.curr= _temp2743; _temp2744.base=
_temp2743; _temp2744.last_plus_one= _temp2743 + 3; _temp2744;})); struct Cyc_PP_Doc*
_temp2737= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp2738)( struct
_tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2741= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2742= Cyc_Absynpp_stmt2doc( c->body); struct Cyc_PP_Doc*
_temp2739[ 2u]={ _temp2741, _temp2742}; struct _tagged_ptr0 _temp2740={
_temp2739, _temp2739, _temp2739 + 2u}; _temp2738( _temp2740);})); struct Cyc_PP_Doc*
_temp2730[ 6u]={ _temp2732, _temp2733, _temp2734, _temp2735, _temp2736,
_temp2737}; struct _tagged_ptr0 _temp2731={ _temp2730, _temp2730, _temp2730 + 6u};
_temp2729( _temp2731);});}}} struct Cyc_PP_Doc* Cyc_Absynpp_switchclauses2doc(
struct Cyc_List_List* cs){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Switch_clause*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_switchclause2doc,({ unsigned char* _temp2749="";
struct _tagged_string _temp2750; _temp2750.curr= _temp2749; _temp2750.base=
_temp2749; _temp2750.last_plus_one= _temp2749 + 1; _temp2750;}), cs);} struct
Cyc_PP_Doc* Cyc_Absynpp_enumfield2doc( struct Cyc_Absyn_Enumfield* f){ if( f->tag
== 0){ return Cyc_Absynpp_qvar2doc( f->name);} else{ return({ struct Cyc_PP_Doc*(*
_temp2751)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2754= Cyc_Absynpp_qvar2doc(
f->name); struct Cyc_PP_Doc* _temp2755= Cyc_PP_text(({ unsigned char* _temp2757=" = ";
struct _tagged_string _temp2758; _temp2758.curr= _temp2757; _temp2758.base=
_temp2757; _temp2758.last_plus_one= _temp2757 + 4; _temp2758;})); struct Cyc_PP_Doc*
_temp2756= Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*) _check_null( f->tag));
struct Cyc_PP_Doc* _temp2752[ 3u]={ _temp2754, _temp2755, _temp2756}; struct
_tagged_ptr0 _temp2753={ _temp2752, _temp2752, _temp2752 + 3u}; _temp2751(
_temp2753);});}} struct Cyc_PP_Doc* Cyc_Absynpp_enumfields2doc( struct Cyc_List_List*
fs){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Enumfield*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_enumfield2doc,({
unsigned char* _temp2759=","; struct _tagged_string _temp2760; _temp2760.curr=
_temp2759; _temp2760.base= _temp2759; _temp2760.last_plus_one= _temp2759 + 2;
_temp2760;}), fs);} static struct Cyc_PP_Doc* Cyc_Absynpp_id2doc( struct Cyc_Absyn_Vardecl*
v){ return Cyc_Absynpp_qvar2doc( v->name);} static struct Cyc_PP_Doc* Cyc_Absynpp_ids2doc(
struct Cyc_List_List* vds){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Vardecl*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseq)( Cyc_Absynpp_id2doc,({ unsigned char* _temp2761=","; struct
_tagged_string _temp2762; _temp2762.curr= _temp2761; _temp2762.base= _temp2761;
_temp2762.last_plus_one= _temp2761 + 2; _temp2762;}), vds);} struct Cyc_PP_Doc*
Cyc_Absynpp_decl2doc( struct Cyc_Absyn_Decl* d){ struct Cyc_PP_Doc* s;{ void*
_temp2763=( void*) d->r; struct Cyc_Absyn_Fndecl* _temp2789; struct Cyc_Absyn_Structdecl*
_temp2791; struct Cyc_Absyn_Uniondecl* _temp2793; struct Cyc_Absyn_Vardecl*
_temp2795; struct Cyc_Absyn_Vardecl _temp2797; struct Cyc_List_List* _temp2798;
struct Cyc_Core_Opt* _temp2800; struct Cyc_Absyn_Exp* _temp2802; void* _temp2804;
struct Cyc_Absyn_Tqual _temp2806; struct _tuple0* _temp2808; void* _temp2810;
struct Cyc_Absyn_Tuniondecl* _temp2812; struct Cyc_Absyn_Tuniondecl _temp2814;
int _temp2815; struct Cyc_Core_Opt* _temp2817; struct Cyc_List_List* _temp2819;
struct _tuple0* _temp2821; void* _temp2823; int _temp2825; struct Cyc_Absyn_Exp*
_temp2827; struct Cyc_Core_Opt* _temp2829; struct Cyc_Core_Opt* _temp2831;
struct Cyc_Absyn_Pat* _temp2833; struct Cyc_List_List* _temp2835; struct Cyc_Absyn_Enumdecl*
_temp2837; struct Cyc_Absyn_Enumdecl _temp2839; struct Cyc_Core_Opt* _temp2840;
struct _tuple0* _temp2842; void* _temp2844; struct Cyc_Absyn_Typedefdecl*
_temp2846; struct Cyc_List_List* _temp2848; struct _tagged_string* _temp2850;
struct Cyc_List_List* _temp2852; struct _tuple0* _temp2854; struct Cyc_List_List*
_temp2856; _LL2765: if(*(( int*) _temp2763) == Cyc_Absyn_Fn_d){ _LL2790:
_temp2789=(( struct Cyc_Absyn_Fn_d_struct*) _temp2763)->f1; goto _LL2766;} else{
goto _LL2767;} _LL2767: if(*(( int*) _temp2763) == Cyc_Absyn_Struct_d){ _LL2792:
_temp2791=(( struct Cyc_Absyn_Struct_d_struct*) _temp2763)->f1; goto _LL2768;}
else{ goto _LL2769;} _LL2769: if(*(( int*) _temp2763) == Cyc_Absyn_Union_d){
_LL2794: _temp2793=(( struct Cyc_Absyn_Union_d_struct*) _temp2763)->f1; goto
_LL2770;} else{ goto _LL2771;} _LL2771: if(*(( int*) _temp2763) == Cyc_Absyn_Var_d){
_LL2796: _temp2795=(( struct Cyc_Absyn_Var_d_struct*) _temp2763)->f1; _temp2797=*
_temp2795; _LL2811: _temp2810=( void*) _temp2797.sc; goto _LL2809; _LL2809:
_temp2808= _temp2797.name; goto _LL2807; _LL2807: _temp2806= _temp2797.tq; goto
_LL2805; _LL2805: _temp2804=( void*) _temp2797.type; goto _LL2803; _LL2803:
_temp2802= _temp2797.initializer; goto _LL2801; _LL2801: _temp2800= _temp2797.rgn;
goto _LL2799; _LL2799: _temp2798= _temp2797.attributes; goto _LL2772;} else{
goto _LL2773;} _LL2773: if(*(( int*) _temp2763) == Cyc_Absyn_Tunion_d){ _LL2813:
_temp2812=(( struct Cyc_Absyn_Tunion_d_struct*) _temp2763)->f1; _temp2814=*
_temp2812; _LL2824: _temp2823=( void*) _temp2814.sc; goto _LL2822; _LL2822:
_temp2821= _temp2814.name; goto _LL2820; _LL2820: _temp2819= _temp2814.tvs; goto
_LL2818; _LL2818: _temp2817= _temp2814.fields; goto _LL2816; _LL2816: _temp2815=
_temp2814.is_xtunion; goto _LL2774;} else{ goto _LL2775;} _LL2775: if(*(( int*)
_temp2763) == Cyc_Absyn_Let_d){ _LL2834: _temp2833=(( struct Cyc_Absyn_Let_d_struct*)
_temp2763)->f1; goto _LL2832; _LL2832: _temp2831=(( struct Cyc_Absyn_Let_d_struct*)
_temp2763)->f2; goto _LL2830; _LL2830: _temp2829=(( struct Cyc_Absyn_Let_d_struct*)
_temp2763)->f3; goto _LL2828; _LL2828: _temp2827=(( struct Cyc_Absyn_Let_d_struct*)
_temp2763)->f4; goto _LL2826; _LL2826: _temp2825=(( struct Cyc_Absyn_Let_d_struct*)
_temp2763)->f5; goto _LL2776;} else{ goto _LL2777;} _LL2777: if(*(( int*)
_temp2763) == Cyc_Absyn_Letv_d){ _LL2836: _temp2835=(( struct Cyc_Absyn_Letv_d_struct*)
_temp2763)->f1; goto _LL2778;} else{ goto _LL2779;} _LL2779: if(*(( int*)
_temp2763) == Cyc_Absyn_Enum_d){ _LL2838: _temp2837=(( struct Cyc_Absyn_Enum_d_struct*)
_temp2763)->f1; _temp2839=* _temp2837; _LL2845: _temp2844=( void*) _temp2839.sc;
goto _LL2843; _LL2843: _temp2842= _temp2839.name; goto _LL2841; _LL2841:
_temp2840= _temp2839.fields; goto _LL2780;} else{ goto _LL2781;} _LL2781: if(*((
int*) _temp2763) == Cyc_Absyn_Typedef_d){ _LL2847: _temp2846=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp2763)->f1; goto _LL2782;} else{ goto _LL2783;} _LL2783: if(*(( int*)
_temp2763) == Cyc_Absyn_Namespace_d){ _LL2851: _temp2850=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2763)->f1; goto _LL2849; _LL2849: _temp2848=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp2763)->f2; goto _LL2784;} else{ goto _LL2785;} _LL2785: if(*(( int*)
_temp2763) == Cyc_Absyn_Using_d){ _LL2855: _temp2854=(( struct Cyc_Absyn_Using_d_struct*)
_temp2763)->f1; goto _LL2853; _LL2853: _temp2852=(( struct Cyc_Absyn_Using_d_struct*)
_temp2763)->f2; goto _LL2786;} else{ goto _LL2787;} _LL2787: if(*(( int*)
_temp2763) == Cyc_Absyn_ExternC_d){ _LL2857: _temp2856=(( struct Cyc_Absyn_ExternC_d_struct*)
_temp2763)->f1; goto _LL2788;} else{ goto _LL2764;} _LL2766: { void* t=( void*)({
struct Cyc_Absyn_FnType_struct* _temp2897=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp2897[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp2898; _temp2898.tag= Cyc_Absyn_FnType; _temp2898.f1=({ struct Cyc_Absyn_FnInfo
_temp2899; _temp2899.tvars= _temp2789->tvs; _temp2899.effect= _temp2789->effect;
_temp2899.ret_typ=( void*)(( void*) _temp2789->ret_type); _temp2899.args=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple3*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_arg_mk_opt, _temp2789->args); _temp2899.c_varargs=
_temp2789->c_varargs; _temp2899.cyc_varargs= _temp2789->cyc_varargs; _temp2899.attributes=
0; _temp2899;}); _temp2898;}); _temp2897;}); struct Cyc_PP_Doc* attsdoc= Cyc_Absynpp_atts2doc(
_temp2789->attributes); struct Cyc_PP_Doc* inlinedoc; if( _temp2789->is_inline){
if( Cyc_Absynpp_to_VC){ inlinedoc= Cyc_PP_text(({ unsigned char* _temp2858="__inline ";
struct _tagged_string _temp2859; _temp2859.curr= _temp2858; _temp2859.base=
_temp2858; _temp2859.last_plus_one= _temp2858 + 10; _temp2859;}));} else{
inlinedoc= Cyc_PP_text(({ unsigned char* _temp2860="inline "; struct
_tagged_string _temp2861; _temp2861.curr= _temp2860; _temp2861.base= _temp2860;
_temp2861.last_plus_one= _temp2860 + 8; _temp2861;}));}} else{ inlinedoc= Cyc_PP_nil_doc();}{
struct Cyc_PP_Doc* scopedoc= Cyc_Absynpp_scope2doc(( void*) _temp2789->sc);
struct Cyc_PP_Doc* beforenamedoc= Cyc_Absynpp_to_VC? Cyc_Absynpp_callconv2doc(
_temp2789->attributes): attsdoc; struct Cyc_PP_Doc* namedoc= Cyc_Absynpp_typedef_name2doc(
_temp2789->name); struct Cyc_PP_Doc* tqtddoc= Cyc_Absynpp_tqtd2doc(({ struct Cyc_Absyn_Tqual
_temp2890; _temp2890.q_const= 0; _temp2890.q_volatile= 0; _temp2890.q_restrict=
0; _temp2890;}), t,({ struct Cyc_Core_Opt* _temp2891=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2891->v=( void*)({ struct Cyc_PP_Doc*(*
_temp2892)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2895=
beforenamedoc; struct Cyc_PP_Doc* _temp2896= namedoc; struct Cyc_PP_Doc*
_temp2893[ 2u]={ _temp2895, _temp2896}; struct _tagged_ptr0 _temp2894={
_temp2893, _temp2893, _temp2893 + 2u}; _temp2892( _temp2894);}); _temp2891;}));
struct Cyc_PP_Doc* bodydoc=({ struct Cyc_PP_Doc*(* _temp2874)( struct
_tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2877= Cyc_PP_text(({
unsigned char* _temp2888=" {"; struct _tagged_string _temp2889; _temp2889.curr=
_temp2888; _temp2889.base= _temp2888; _temp2889.last_plus_one= _temp2888 + 3;
_temp2889;})); struct Cyc_PP_Doc* _temp2878= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(*
_temp2883)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2886= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2887= Cyc_Absynpp_stmt2doc( _temp2789->body); struct Cyc_PP_Doc*
_temp2884[ 2u]={ _temp2886, _temp2887}; struct _tagged_ptr0 _temp2885={
_temp2884, _temp2884, _temp2884 + 2u}; _temp2883( _temp2885);})); struct Cyc_PP_Doc*
_temp2879= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2880= Cyc_PP_text(({
unsigned char* _temp2881="}"; struct _tagged_string _temp2882; _temp2882.curr=
_temp2881; _temp2882.base= _temp2881; _temp2882.last_plus_one= _temp2881 + 2;
_temp2882;})); struct Cyc_PP_Doc* _temp2875[ 4u]={ _temp2877, _temp2878,
_temp2879, _temp2880}; struct _tagged_ptr0 _temp2876={ _temp2875, _temp2875,
_temp2875 + 4u}; _temp2874( _temp2876);}); s=({ struct Cyc_PP_Doc*(* _temp2862)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2865= inlinedoc;
struct Cyc_PP_Doc* _temp2866= scopedoc; struct Cyc_PP_Doc* _temp2867= tqtddoc;
struct Cyc_PP_Doc* _temp2868= bodydoc; struct Cyc_PP_Doc* _temp2863[ 4u]={
_temp2865, _temp2866, _temp2867, _temp2868}; struct _tagged_ptr0 _temp2864={
_temp2863, _temp2863, _temp2863 + 4u}; _temp2862( _temp2864);}); if( Cyc_Absynpp_to_VC){
s=({ struct Cyc_PP_Doc*(* _temp2869)( struct _tagged_ptr0)= Cyc_PP_cat; struct
Cyc_PP_Doc* _temp2872= attsdoc; struct Cyc_PP_Doc* _temp2873= s; struct Cyc_PP_Doc*
_temp2870[ 2u]={ _temp2872, _temp2873}; struct _tagged_ptr0 _temp2871={
_temp2870, _temp2870, _temp2870 + 2u}; _temp2869( _temp2871);});} goto _LL2764;}}
_LL2768: if( _temp2791->fields == 0){ s=({ struct Cyc_PP_Doc*(* _temp2900)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2903= Cyc_Absynpp_scope2doc((
void*) _temp2791->sc); struct Cyc_PP_Doc* _temp2904= Cyc_PP_text(({
unsigned char* _temp2912="struct "; struct _tagged_string _temp2913; _temp2913.curr=
_temp2912; _temp2913.base= _temp2912; _temp2913.last_plus_one= _temp2912 + 8;
_temp2913;})); struct Cyc_PP_Doc* _temp2905= _temp2791->name == 0? Cyc_PP_text(({
unsigned char* _temp2910=""; struct _tagged_string _temp2911; _temp2911.curr=
_temp2910; _temp2911.base= _temp2910; _temp2911.last_plus_one= _temp2910 + 1;
_temp2911;})): Cyc_Absynpp_typedef_name2doc(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( _temp2791->name))->v); struct Cyc_PP_Doc* _temp2906= Cyc_Absynpp_ktvars2doc(
_temp2791->tvs); struct Cyc_PP_Doc* _temp2907= Cyc_PP_text(({ unsigned char*
_temp2908=";"; struct _tagged_string _temp2909; _temp2909.curr= _temp2908;
_temp2909.base= _temp2908; _temp2909.last_plus_one= _temp2908 + 2; _temp2909;}));
struct Cyc_PP_Doc* _temp2901[ 5u]={ _temp2903, _temp2904, _temp2905, _temp2906,
_temp2907}; struct _tagged_ptr0 _temp2902={ _temp2901, _temp2901, _temp2901 + 5u};
_temp2900( _temp2902);});} else{ s=({ struct Cyc_PP_Doc*(* _temp2914)( struct
_tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2917= Cyc_Absynpp_scope2doc((
void*) _temp2791->sc); struct Cyc_PP_Doc* _temp2918= Cyc_PP_text(({
unsigned char* _temp2940="struct "; struct _tagged_string _temp2941; _temp2941.curr=
_temp2940; _temp2941.base= _temp2940; _temp2941.last_plus_one= _temp2940 + 8;
_temp2941;})); struct Cyc_PP_Doc* _temp2919= _temp2791->name == 0? Cyc_PP_text(({
unsigned char* _temp2938=""; struct _tagged_string _temp2939; _temp2939.curr=
_temp2938; _temp2939.base= _temp2938; _temp2939.last_plus_one= _temp2938 + 1;
_temp2939;})): Cyc_Absynpp_typedef_name2doc(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( _temp2791->name))->v); struct Cyc_PP_Doc* _temp2920= Cyc_Absynpp_ktvars2doc(
_temp2791->tvs); struct Cyc_PP_Doc* _temp2921= Cyc_PP_text(({ unsigned char*
_temp2936=" {"; struct _tagged_string _temp2937; _temp2937.curr= _temp2936;
_temp2937.base= _temp2936; _temp2937.last_plus_one= _temp2936 + 3; _temp2937;}));
struct Cyc_PP_Doc* _temp2922= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp2931)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2934= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2935= Cyc_Absynpp_structfields2doc(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp2791->fields))->v); struct Cyc_PP_Doc*
_temp2932[ 2u]={ _temp2934, _temp2935}; struct _tagged_ptr0 _temp2933={
_temp2932, _temp2932, _temp2932 + 2u}; _temp2931( _temp2933);})); struct Cyc_PP_Doc*
_temp2923= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2924= Cyc_PP_text(({
unsigned char* _temp2929="}"; struct _tagged_string _temp2930; _temp2930.curr=
_temp2929; _temp2930.base= _temp2929; _temp2930.last_plus_one= _temp2929 + 2;
_temp2930;})); struct Cyc_PP_Doc* _temp2925= Cyc_Absynpp_atts2doc( _temp2791->attributes);
struct Cyc_PP_Doc* _temp2926= Cyc_PP_text(({ unsigned char* _temp2927=";";
struct _tagged_string _temp2928; _temp2928.curr= _temp2927; _temp2928.base=
_temp2927; _temp2928.last_plus_one= _temp2927 + 2; _temp2928;})); struct Cyc_PP_Doc*
_temp2915[ 10u]={ _temp2917, _temp2918, _temp2919, _temp2920, _temp2921,
_temp2922, _temp2923, _temp2924, _temp2925, _temp2926}; struct _tagged_ptr0
_temp2916={ _temp2915, _temp2915, _temp2915 + 10u}; _temp2914( _temp2916);});}
goto _LL2764; _LL2770: if( _temp2793->fields == 0){ s=({ struct Cyc_PP_Doc*(*
_temp2942)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2945= Cyc_Absynpp_scope2doc((
void*) _temp2793->sc); struct Cyc_PP_Doc* _temp2946= Cyc_PP_text(({
unsigned char* _temp2954="union "; struct _tagged_string _temp2955; _temp2955.curr=
_temp2954; _temp2955.base= _temp2954; _temp2955.last_plus_one= _temp2954 + 7;
_temp2955;})); struct Cyc_PP_Doc* _temp2947= _temp2793->name == 0? Cyc_PP_text(({
unsigned char* _temp2952=""; struct _tagged_string _temp2953; _temp2953.curr=
_temp2952; _temp2953.base= _temp2952; _temp2953.last_plus_one= _temp2952 + 1;
_temp2953;})): Cyc_Absynpp_qvar2doc(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( _temp2793->name))->v); struct Cyc_PP_Doc* _temp2948= Cyc_Absynpp_tvars2doc(
_temp2793->tvs); struct Cyc_PP_Doc* _temp2949= Cyc_PP_text(({ unsigned char*
_temp2950=";"; struct _tagged_string _temp2951; _temp2951.curr= _temp2950;
_temp2951.base= _temp2950; _temp2951.last_plus_one= _temp2950 + 2; _temp2951;}));
struct Cyc_PP_Doc* _temp2943[ 5u]={ _temp2945, _temp2946, _temp2947, _temp2948,
_temp2949}; struct _tagged_ptr0 _temp2944={ _temp2943, _temp2943, _temp2943 + 5u};
_temp2942( _temp2944);});} else{ s=({ struct Cyc_PP_Doc*(* _temp2956)( struct
_tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2959= Cyc_Absynpp_scope2doc((
void*) _temp2793->sc); struct Cyc_PP_Doc* _temp2960= Cyc_PP_text(({
unsigned char* _temp2982="union "; struct _tagged_string _temp2983; _temp2983.curr=
_temp2982; _temp2983.base= _temp2982; _temp2983.last_plus_one= _temp2982 + 7;
_temp2983;})); struct Cyc_PP_Doc* _temp2961= _temp2793->name == 0? Cyc_PP_text(({
unsigned char* _temp2980=""; struct _tagged_string _temp2981; _temp2981.curr=
_temp2980; _temp2981.base= _temp2980; _temp2981.last_plus_one= _temp2980 + 1;
_temp2981;})): Cyc_Absynpp_qvar2doc(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( _temp2793->name))->v); struct Cyc_PP_Doc* _temp2962= Cyc_Absynpp_tvars2doc(
_temp2793->tvs); struct Cyc_PP_Doc* _temp2963= Cyc_PP_text(({ unsigned char*
_temp2978=" {"; struct _tagged_string _temp2979; _temp2979.curr= _temp2978;
_temp2979.base= _temp2978; _temp2979.last_plus_one= _temp2978 + 3; _temp2979;}));
struct Cyc_PP_Doc* _temp2964= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(* _temp2973)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp2976= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp2977= Cyc_Absynpp_structfields2doc(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp2793->fields))->v); struct Cyc_PP_Doc*
_temp2974[ 2u]={ _temp2976, _temp2977}; struct _tagged_ptr0 _temp2975={
_temp2974, _temp2974, _temp2974 + 2u}; _temp2973( _temp2975);})); struct Cyc_PP_Doc*
_temp2965= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp2966= Cyc_PP_text(({
unsigned char* _temp2971="}"; struct _tagged_string _temp2972; _temp2972.curr=
_temp2971; _temp2972.base= _temp2971; _temp2972.last_plus_one= _temp2971 + 2;
_temp2972;})); struct Cyc_PP_Doc* _temp2967= Cyc_Absynpp_atts2doc( _temp2793->attributes);
struct Cyc_PP_Doc* _temp2968= Cyc_PP_text(({ unsigned char* _temp2969=";";
struct _tagged_string _temp2970; _temp2970.curr= _temp2969; _temp2970.base=
_temp2969; _temp2970.last_plus_one= _temp2969 + 2; _temp2970;})); struct Cyc_PP_Doc*
_temp2957[ 10u]={ _temp2959, _temp2960, _temp2961, _temp2962, _temp2963,
_temp2964, _temp2965, _temp2966, _temp2967, _temp2968}; struct _tagged_ptr0
_temp2958={ _temp2957, _temp2957, _temp2957 + 10u}; _temp2956( _temp2958);});}
goto _LL2764; _LL2772: { struct Cyc_PP_Doc* sn= Cyc_Absynpp_typedef_name2doc(
_temp2808); s=({ struct Cyc_PP_Doc*(* _temp2984)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp2987= Cyc_Absynpp_to_VC? Cyc_Absynpp_atts2doc( _temp2798):
Cyc_PP_nil_doc(); struct Cyc_PP_Doc* _temp2988= Cyc_Absynpp_scope2doc( _temp2810);
struct Cyc_PP_Doc* _temp2989= Cyc_Absynpp_tqtd2doc( _temp2806, _temp2804,({
struct Cyc_Core_Opt* _temp3004=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3004->v=( void*) sn; _temp3004;})); struct Cyc_PP_Doc*
_temp2990= ! Cyc_Absynpp_to_VC? Cyc_Absynpp_atts2doc( _temp2798): Cyc_PP_nil_doc();
struct Cyc_PP_Doc* _temp2991= _temp2802 == 0? Cyc_PP_text(({ unsigned char*
_temp2995=""; struct _tagged_string _temp2996; _temp2996.curr= _temp2995;
_temp2996.base= _temp2995; _temp2996.last_plus_one= _temp2995 + 1; _temp2996;})):({
struct Cyc_PP_Doc*(* _temp2997)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp3000= Cyc_PP_text(({ unsigned char* _temp3002=" = "; struct _tagged_string
_temp3003; _temp3003.curr= _temp3002; _temp3003.base= _temp3002; _temp3003.last_plus_one=
_temp3002 + 4; _temp3003;})); struct Cyc_PP_Doc* _temp3001= Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*) _check_null( _temp2802)); struct Cyc_PP_Doc* _temp2998[ 2u]={
_temp3000, _temp3001}; struct _tagged_ptr0 _temp2999={ _temp2998, _temp2998,
_temp2998 + 2u}; _temp2997( _temp2999);}); struct Cyc_PP_Doc* _temp2992= Cyc_PP_text(({
unsigned char* _temp2993=";"; struct _tagged_string _temp2994; _temp2994.curr=
_temp2993; _temp2994.base= _temp2993; _temp2994.last_plus_one= _temp2993 + 2;
_temp2994;})); struct Cyc_PP_Doc* _temp2985[ 6u]={ _temp2987, _temp2988,
_temp2989, _temp2990, _temp2991, _temp2992}; struct _tagged_ptr0 _temp2986={
_temp2985, _temp2985, _temp2985 + 6u}; _temp2984( _temp2986);}); goto _LL2764;}
_LL2774: if( _temp2817 == 0){ s=({ struct Cyc_PP_Doc*(* _temp3005)( struct
_tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp3008= Cyc_Absynpp_scope2doc(
_temp2823); struct Cyc_PP_Doc* _temp3009= _temp2815? Cyc_PP_text(({
unsigned char* _temp3015="xtunion "; struct _tagged_string _temp3016; _temp3016.curr=
_temp3015; _temp3016.base= _temp3015; _temp3016.last_plus_one= _temp3015 + 9;
_temp3016;})): Cyc_PP_text(({ unsigned char* _temp3017="tunion "; struct
_tagged_string _temp3018; _temp3018.curr= _temp3017; _temp3018.base= _temp3017;
_temp3018.last_plus_one= _temp3017 + 8; _temp3018;})); struct Cyc_PP_Doc*
_temp3010= _temp2815? Cyc_Absynpp_qvar2doc( _temp2821): Cyc_Absynpp_typedef_name2doc(
_temp2821); struct Cyc_PP_Doc* _temp3011= Cyc_Absynpp_ktvars2doc( _temp2819);
struct Cyc_PP_Doc* _temp3012= Cyc_PP_text(({ unsigned char* _temp3013=";";
struct _tagged_string _temp3014; _temp3014.curr= _temp3013; _temp3014.base=
_temp3013; _temp3014.last_plus_one= _temp3013 + 2; _temp3014;})); struct Cyc_PP_Doc*
_temp3006[ 5u]={ _temp3008, _temp3009, _temp3010, _temp3011, _temp3012}; struct
_tagged_ptr0 _temp3007={ _temp3006, _temp3006, _temp3006 + 5u}; _temp3005(
_temp3007);});} else{ s=({ struct Cyc_PP_Doc*(* _temp3019)( struct _tagged_ptr0)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp3022= Cyc_Absynpp_scope2doc( _temp2823);
struct Cyc_PP_Doc* _temp3023= _temp2815? Cyc_PP_text(({ unsigned char* _temp3039="xtunion ";
struct _tagged_string _temp3040; _temp3040.curr= _temp3039; _temp3040.base=
_temp3039; _temp3040.last_plus_one= _temp3039 + 9; _temp3040;})): Cyc_PP_text(({
unsigned char* _temp3041="tunion "; struct _tagged_string _temp3042; _temp3042.curr=
_temp3041; _temp3042.base= _temp3041; _temp3042.last_plus_one= _temp3041 + 8;
_temp3042;})); struct Cyc_PP_Doc* _temp3024= _temp2815? Cyc_Absynpp_qvar2doc(
_temp2821): Cyc_Absynpp_typedef_name2doc( _temp2821); struct Cyc_PP_Doc*
_temp3025= Cyc_Absynpp_ktvars2doc( _temp2819); struct Cyc_PP_Doc* _temp3026= Cyc_PP_text(({
unsigned char* _temp3037=" {"; struct _tagged_string _temp3038; _temp3038.curr=
_temp3037; _temp3038.base= _temp3037; _temp3038.last_plus_one= _temp3037 + 3;
_temp3038;})); struct Cyc_PP_Doc* _temp3027= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(*
_temp3032)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp3035= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp3036= Cyc_Absynpp_tunionfields2doc(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp2817))->v); struct Cyc_PP_Doc* _temp3033[
2u]={ _temp3035, _temp3036}; struct _tagged_ptr0 _temp3034={ _temp3033,
_temp3033, _temp3033 + 2u}; _temp3032( _temp3034);})); struct Cyc_PP_Doc*
_temp3028= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp3029= Cyc_PP_text(({
unsigned char* _temp3030="};"; struct _tagged_string _temp3031; _temp3031.curr=
_temp3030; _temp3031.base= _temp3030; _temp3031.last_plus_one= _temp3030 + 3;
_temp3031;})); struct Cyc_PP_Doc* _temp3020[ 8u]={ _temp3022, _temp3023,
_temp3024, _temp3025, _temp3026, _temp3027, _temp3028, _temp3029}; struct
_tagged_ptr0 _temp3021={ _temp3020, _temp3020, _temp3020 + 8u}; _temp3019(
_temp3021);});} goto _LL2764; _LL2776: s=({ struct Cyc_PP_Doc*(* _temp3043)(
struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp3046= Cyc_PP_text(({
unsigned char* _temp3055="let "; struct _tagged_string _temp3056; _temp3056.curr=
_temp3055; _temp3056.base= _temp3055; _temp3056.last_plus_one= _temp3055 + 5;
_temp3056;})); struct Cyc_PP_Doc* _temp3047= Cyc_Absynpp_pat2doc( _temp2833);
struct Cyc_PP_Doc* _temp3048= Cyc_PP_text(({ unsigned char* _temp3053=" = ";
struct _tagged_string _temp3054; _temp3054.curr= _temp3053; _temp3054.base=
_temp3053; _temp3054.last_plus_one= _temp3053 + 4; _temp3054;})); struct Cyc_PP_Doc*
_temp3049= Cyc_Absynpp_exp2doc( _temp2827); struct Cyc_PP_Doc* _temp3050= Cyc_PP_text(({
unsigned char* _temp3051=";"; struct _tagged_string _temp3052; _temp3052.curr=
_temp3051; _temp3052.base= _temp3051; _temp3052.last_plus_one= _temp3051 + 2;
_temp3052;})); struct Cyc_PP_Doc* _temp3044[ 5u]={ _temp3046, _temp3047,
_temp3048, _temp3049, _temp3050}; struct _tagged_ptr0 _temp3045={ _temp3044,
_temp3044, _temp3044 + 5u}; _temp3043( _temp3045);}); goto _LL2764; _LL2778: s=({
struct Cyc_PP_Doc*(* _temp3057)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp3060= Cyc_PP_text(({ unsigned char* _temp3065="let "; struct _tagged_string
_temp3066; _temp3066.curr= _temp3065; _temp3066.base= _temp3065; _temp3066.last_plus_one=
_temp3065 + 5; _temp3066;})); struct Cyc_PP_Doc* _temp3061= Cyc_Absynpp_ids2doc(
_temp2835); struct Cyc_PP_Doc* _temp3062= Cyc_PP_text(({ unsigned char*
_temp3063=";"; struct _tagged_string _temp3064; _temp3064.curr= _temp3063;
_temp3064.base= _temp3063; _temp3064.last_plus_one= _temp3063 + 2; _temp3064;}));
struct Cyc_PP_Doc* _temp3058[ 3u]={ _temp3060, _temp3061, _temp3062}; struct
_tagged_ptr0 _temp3059={ _temp3058, _temp3058, _temp3058 + 3u}; _temp3057(
_temp3059);}); goto _LL2764; _LL2780: if( _temp2840 == 0){ s=({ struct Cyc_PP_Doc*(*
_temp3067)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp3070= Cyc_Absynpp_scope2doc(
_temp2844); struct Cyc_PP_Doc* _temp3071= Cyc_PP_text(({ unsigned char*
_temp3076="enum "; struct _tagged_string _temp3077; _temp3077.curr= _temp3076;
_temp3077.base= _temp3076; _temp3077.last_plus_one= _temp3076 + 6; _temp3077;}));
struct Cyc_PP_Doc* _temp3072= Cyc_Absynpp_typedef_name2doc( _temp2842); struct
Cyc_PP_Doc* _temp3073= Cyc_PP_text(({ unsigned char* _temp3074=";"; struct
_tagged_string _temp3075; _temp3075.curr= _temp3074; _temp3075.base= _temp3074;
_temp3075.last_plus_one= _temp3074 + 2; _temp3075;})); struct Cyc_PP_Doc*
_temp3068[ 4u]={ _temp3070, _temp3071, _temp3072, _temp3073}; struct
_tagged_ptr0 _temp3069={ _temp3068, _temp3068, _temp3068 + 4u}; _temp3067(
_temp3069);});} else{ s=({ struct Cyc_PP_Doc*(* _temp3078)( struct _tagged_ptr0)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp3081= Cyc_Absynpp_scope2doc( _temp2844);
struct Cyc_PP_Doc* _temp3082= Cyc_PP_text(({ unsigned char* _temp3097="enum ";
struct _tagged_string _temp3098; _temp3098.curr= _temp3097; _temp3098.base=
_temp3097; _temp3098.last_plus_one= _temp3097 + 6; _temp3098;})); struct Cyc_PP_Doc*
_temp3083= Cyc_Absynpp_qvar2doc( _temp2842); struct Cyc_PP_Doc* _temp3084= Cyc_PP_text(({
unsigned char* _temp3095=" {"; struct _tagged_string _temp3096; _temp3096.curr=
_temp3095; _temp3096.base= _temp3095; _temp3096.last_plus_one= _temp3095 + 3;
_temp3096;})); struct Cyc_PP_Doc* _temp3085= Cyc_PP_nest( 2,({ struct Cyc_PP_Doc*(*
_temp3090)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp3093= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp3094= Cyc_Absynpp_enumfields2doc(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp2840))->v); struct Cyc_PP_Doc* _temp3091[
2u]={ _temp3093, _temp3094}; struct _tagged_ptr0 _temp3092={ _temp3091,
_temp3091, _temp3091 + 2u}; _temp3090( _temp3092);})); struct Cyc_PP_Doc*
_temp3086= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp3087= Cyc_PP_text(({
unsigned char* _temp3088="};"; struct _tagged_string _temp3089; _temp3089.curr=
_temp3088; _temp3089.base= _temp3088; _temp3089.last_plus_one= _temp3088 + 3;
_temp3089;})); struct Cyc_PP_Doc* _temp3079[ 7u]={ _temp3081, _temp3082,
_temp3083, _temp3084, _temp3085, _temp3086, _temp3087}; struct _tagged_ptr0
_temp3080={ _temp3079, _temp3079, _temp3079 + 7u}; _temp3078( _temp3080);});}
goto _LL2764; _LL2782: s=({ struct Cyc_PP_Doc*(* _temp3099)( struct _tagged_ptr0)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp3102= Cyc_PP_text(({ unsigned char*
_temp3114="typedef "; struct _tagged_string _temp3115; _temp3115.curr= _temp3114;
_temp3115.base= _temp3114; _temp3115.last_plus_one= _temp3114 + 9; _temp3115;}));
struct Cyc_PP_Doc* _temp3103= Cyc_Absynpp_tqtd2doc(({ struct Cyc_Absyn_Tqual
_temp3107; _temp3107.q_const= 0; _temp3107.q_volatile= 0; _temp3107.q_restrict=
0; _temp3107;}),( void*) _temp2846->defn,({ struct Cyc_Core_Opt* _temp3108=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3108->v=(
void*)({ struct Cyc_PP_Doc*(* _temp3109)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp3112= Cyc_Absynpp_typedef_name2doc( _temp2846->name);
struct Cyc_PP_Doc* _temp3113= Cyc_Absynpp_tvars2doc( _temp2846->tvs); struct Cyc_PP_Doc*
_temp3110[ 2u]={ _temp3112, _temp3113}; struct _tagged_ptr0 _temp3111={
_temp3110, _temp3110, _temp3110 + 2u}; _temp3109( _temp3111);}); _temp3108;}));
struct Cyc_PP_Doc* _temp3104= Cyc_PP_text(({ unsigned char* _temp3105=";";
struct _tagged_string _temp3106; _temp3106.curr= _temp3105; _temp3106.base=
_temp3105; _temp3106.last_plus_one= _temp3105 + 2; _temp3106;})); struct Cyc_PP_Doc*
_temp3100[ 3u]={ _temp3102, _temp3103, _temp3104}; struct _tagged_ptr0 _temp3101={
_temp3100, _temp3100, _temp3100 + 3u}; _temp3099( _temp3101);}); goto _LL2764;
_LL2784: if( ! Cyc_Absynpp_qvar_to_Cids){ Cyc_Absynpp_curr_namespace_add(
_temp2850);} s=({ struct Cyc_PP_Doc*(* _temp3116)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp3119= Cyc_PP_text(({ unsigned char* _temp3132="namespace ";
struct _tagged_string _temp3133; _temp3133.curr= _temp3132; _temp3133.base=
_temp3132; _temp3133.last_plus_one= _temp3132 + 11; _temp3133;})); struct Cyc_PP_Doc*
_temp3120= Cyc_PP_textptr( _temp2850); struct Cyc_PP_Doc* _temp3121= Cyc_PP_text(({
unsigned char* _temp3130=" {"; struct _tagged_string _temp3131; _temp3131.curr=
_temp3130; _temp3131.base= _temp3130; _temp3131.last_plus_one= _temp3130 + 3;
_temp3131;})); struct Cyc_PP_Doc* _temp3122= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp3123=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,({
unsigned char* _temp3128=""; struct _tagged_string _temp3129; _temp3129.curr=
_temp3128; _temp3129.base= _temp3128; _temp3129.last_plus_one= _temp3128 + 1;
_temp3129;}), _temp2848); struct Cyc_PP_Doc* _temp3124= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp3125= Cyc_PP_text(({ unsigned char* _temp3126="}";
struct _tagged_string _temp3127; _temp3127.curr= _temp3126; _temp3127.base=
_temp3126; _temp3127.last_plus_one= _temp3126 + 2; _temp3127;})); struct Cyc_PP_Doc*
_temp3117[ 7u]={ _temp3119, _temp3120, _temp3121, _temp3122, _temp3123,
_temp3124, _temp3125}; struct _tagged_ptr0 _temp3118={ _temp3117, _temp3117,
_temp3117 + 7u}; _temp3116( _temp3118);}); if( ! Cyc_Absynpp_qvar_to_Cids){ Cyc_Absynpp_curr_namespace_drop();}
goto _LL2764; _LL2786: if( Cyc_Absynpp_print_using_stmts){ s=({ struct Cyc_PP_Doc*(*
_temp3134)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp3137= Cyc_PP_text(({
unsigned char* _temp3150="using "; struct _tagged_string _temp3151; _temp3151.curr=
_temp3150; _temp3151.base= _temp3150; _temp3151.last_plus_one= _temp3150 + 7;
_temp3151;})); struct Cyc_PP_Doc* _temp3138= Cyc_Absynpp_qvar2doc( _temp2854);
struct Cyc_PP_Doc* _temp3139= Cyc_PP_text(({ unsigned char* _temp3148=" {";
struct _tagged_string _temp3149; _temp3149.curr= _temp3148; _temp3149.base=
_temp3148; _temp3149.last_plus_one= _temp3148 + 3; _temp3149;})); struct Cyc_PP_Doc*
_temp3140= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp3141=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct _tagged_string sep,
struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,({ unsigned char*
_temp3146=""; struct _tagged_string _temp3147; _temp3147.curr= _temp3146;
_temp3147.base= _temp3146; _temp3147.last_plus_one= _temp3146 + 1; _temp3147;}),
_temp2852); struct Cyc_PP_Doc* _temp3142= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp3143= Cyc_PP_text(({ unsigned char* _temp3144="}"; struct _tagged_string
_temp3145; _temp3145.curr= _temp3144; _temp3145.base= _temp3144; _temp3145.last_plus_one=
_temp3144 + 2; _temp3145;})); struct Cyc_PP_Doc* _temp3135[ 7u]={ _temp3137,
_temp3138, _temp3139, _temp3140, _temp3141, _temp3142, _temp3143}; struct
_tagged_ptr0 _temp3136={ _temp3135, _temp3135, _temp3135 + 7u}; _temp3134(
_temp3136);});} else{ s=({ struct Cyc_PP_Doc*(* _temp3152)( struct _tagged_ptr0)=
Cyc_PP_cat; struct Cyc_PP_Doc* _temp3155= Cyc_PP_text(({ unsigned char*
_temp3168="/* using "; struct _tagged_string _temp3169; _temp3169.curr=
_temp3168; _temp3169.base= _temp3168; _temp3169.last_plus_one= _temp3168 + 10;
_temp3169;})); struct Cyc_PP_Doc* _temp3156= Cyc_Absynpp_qvar2doc( _temp2854);
struct Cyc_PP_Doc* _temp3157= Cyc_PP_text(({ unsigned char* _temp3166=" { */";
struct _tagged_string _temp3167; _temp3167.curr= _temp3166; _temp3167.base=
_temp3166; _temp3167.last_plus_one= _temp3166 + 6; _temp3167;})); struct Cyc_PP_Doc*
_temp3158= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp3159=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct _tagged_string sep,
struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,({ unsigned char*
_temp3164=""; struct _tagged_string _temp3165; _temp3165.curr= _temp3164;
_temp3165.base= _temp3164; _temp3165.last_plus_one= _temp3164 + 1; _temp3165;}),
_temp2852); struct Cyc_PP_Doc* _temp3160= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp3161= Cyc_PP_text(({ unsigned char* _temp3162="/* } */"; struct
_tagged_string _temp3163; _temp3163.curr= _temp3162; _temp3163.base= _temp3162;
_temp3163.last_plus_one= _temp3162 + 8; _temp3163;})); struct Cyc_PP_Doc*
_temp3153[ 7u]={ _temp3155, _temp3156, _temp3157, _temp3158, _temp3159,
_temp3160, _temp3161}; struct _tagged_ptr0 _temp3154={ _temp3153, _temp3153,
_temp3153 + 7u}; _temp3152( _temp3154);});} goto _LL2764; _LL2788: if( Cyc_Absynpp_print_externC_stmts){
s=({ struct Cyc_PP_Doc*(* _temp3170)( struct _tagged_ptr0)= Cyc_PP_cat; struct
Cyc_PP_Doc* _temp3173= Cyc_PP_text(({ unsigned char* _temp3182="extern \"C\" {";
struct _tagged_string _temp3183; _temp3183.curr= _temp3182; _temp3183.base=
_temp3182; _temp3183.last_plus_one= _temp3182 + 13; _temp3183;})); struct Cyc_PP_Doc*
_temp3174= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp3175=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct _tagged_string sep,
struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,({ unsigned char*
_temp3180=""; struct _tagged_string _temp3181; _temp3181.curr= _temp3180;
_temp3181.base= _temp3180; _temp3181.last_plus_one= _temp3180 + 1; _temp3181;}),
_temp2856); struct Cyc_PP_Doc* _temp3176= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp3177= Cyc_PP_text(({ unsigned char* _temp3178="}"; struct _tagged_string
_temp3179; _temp3179.curr= _temp3178; _temp3179.base= _temp3178; _temp3179.last_plus_one=
_temp3178 + 2; _temp3179;})); struct Cyc_PP_Doc* _temp3171[ 5u]={ _temp3173,
_temp3174, _temp3175, _temp3176, _temp3177}; struct _tagged_ptr0 _temp3172={
_temp3171, _temp3171, _temp3171 + 5u}; _temp3170( _temp3172);});} else{ s=({
struct Cyc_PP_Doc*(* _temp3184)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp3187= Cyc_PP_text(({ unsigned char* _temp3196="/* extern \"C\" { */";
struct _tagged_string _temp3197; _temp3197.curr= _temp3196; _temp3197.base=
_temp3196; _temp3197.last_plus_one= _temp3196 + 19; _temp3197;})); struct Cyc_PP_Doc*
_temp3188= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp3189=(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct _tagged_string sep,
struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,({ unsigned char*
_temp3194=""; struct _tagged_string _temp3195; _temp3195.curr= _temp3194;
_temp3195.base= _temp3194; _temp3195.last_plus_one= _temp3194 + 1; _temp3195;}),
_temp2856); struct Cyc_PP_Doc* _temp3190= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp3191= Cyc_PP_text(({ unsigned char* _temp3192="/* } */"; struct
_tagged_string _temp3193; _temp3193.curr= _temp3192; _temp3193.base= _temp3192;
_temp3193.last_plus_one= _temp3192 + 8; _temp3193;})); struct Cyc_PP_Doc*
_temp3185[ 5u]={ _temp3187, _temp3188, _temp3189, _temp3190, _temp3191}; struct
_tagged_ptr0 _temp3186={ _temp3185, _temp3185, _temp3185 + 5u}; _temp3184(
_temp3186);});} goto _LL2764; _LL2764:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_scope2doc(
void* sc){ void* _temp3198= sc; _LL3200: if( _temp3198 ==( void*) Cyc_Absyn_Static){
goto _LL3201;} else{ goto _LL3202;} _LL3202: if( _temp3198 ==( void*) Cyc_Absyn_Public){
goto _LL3203;} else{ goto _LL3204;} _LL3204: if( _temp3198 ==( void*) Cyc_Absyn_Extern){
goto _LL3205;} else{ goto _LL3206;} _LL3206: if( _temp3198 ==( void*) Cyc_Absyn_ExternC){
goto _LL3207;} else{ goto _LL3208;} _LL3208: if( _temp3198 ==( void*) Cyc_Absyn_Abstract){
goto _LL3209;} else{ goto _LL3199;} _LL3201: return Cyc_PP_text(({ unsigned char*
_temp3210="static "; struct _tagged_string _temp3211; _temp3211.curr= _temp3210;
_temp3211.base= _temp3210; _temp3211.last_plus_one= _temp3210 + 8; _temp3211;}));
_LL3203: return Cyc_PP_nil_doc(); _LL3205: return Cyc_PP_text(({ unsigned char*
_temp3212="extern "; struct _tagged_string _temp3213; _temp3213.curr= _temp3212;
_temp3213.base= _temp3212; _temp3213.last_plus_one= _temp3212 + 8; _temp3213;}));
_LL3207: return Cyc_PP_text(({ unsigned char* _temp3214="extern \"C\" "; struct
_tagged_string _temp3215; _temp3215.curr= _temp3214; _temp3215.base= _temp3214;
_temp3215.last_plus_one= _temp3214 + 12; _temp3215;})); _LL3209: return Cyc_PP_text(({
unsigned char* _temp3216="abstract "; struct _tagged_string _temp3217; _temp3217.curr=
_temp3216; _temp3217.base= _temp3216; _temp3217.last_plus_one= _temp3216 + 10;
_temp3217;})); _LL3199:;} int Cyc_Absynpp_exists_temp_tvar_in_effect( void* t){
void* _temp3218= t; struct Cyc_Absyn_Tvar* _temp3226; struct Cyc_List_List*
_temp3228; _LL3220: if(( unsigned int) _temp3218 > 4u?*(( int*) _temp3218) ==
Cyc_Absyn_VarType: 0){ _LL3227: _temp3226=(( struct Cyc_Absyn_VarType_struct*)
_temp3218)->f1; goto _LL3221;} else{ goto _LL3222;} _LL3222: if(( unsigned int)
_temp3218 > 4u?*(( int*) _temp3218) == Cyc_Absyn_JoinEff: 0){ _LL3229: _temp3228=((
struct Cyc_Absyn_JoinEff_struct*) _temp3218)->f1; goto _LL3223;} else{ goto
_LL3224;} _LL3224: goto _LL3225; _LL3221: return Cyc_Tcutil_is_temp_tvar(
_temp3226); _LL3223: return(( int(*)( int(* pred)( void*), struct Cyc_List_List*
x)) Cyc_List_exists)( Cyc_Absynpp_exists_temp_tvar_in_effect, _temp3228);
_LL3225: return 0; _LL3219:;} struct _tuple4 Cyc_Absynpp_to_tms( struct Cyc_Absyn_Tqual
tq, void* t){ void* _temp3230= t; struct Cyc_Absyn_Exp* _temp3244; struct Cyc_Absyn_Tqual
_temp3246; void* _temp3248; struct Cyc_Absyn_PtrInfo _temp3250; struct Cyc_Absyn_Conref*
_temp3252; struct Cyc_Absyn_Tqual _temp3254; struct Cyc_Absyn_Conref* _temp3256;
void* _temp3258; void* _temp3260; struct Cyc_Absyn_FnInfo _temp3262; struct Cyc_List_List*
_temp3264; struct Cyc_Absyn_VarargInfo* _temp3266; int _temp3268; struct Cyc_List_List*
_temp3270; void* _temp3272; struct Cyc_Core_Opt* _temp3274; struct Cyc_List_List*
_temp3276; int _temp3278; struct Cyc_Core_Opt* _temp3280; void* _temp3282;
struct Cyc_Core_Opt* _temp3284; struct Cyc_List_List* _temp3286; struct _tuple0*
_temp3288; _LL3232: if(( unsigned int) _temp3230 > 4u?*(( int*) _temp3230) ==
Cyc_Absyn_ArrayType: 0){ _LL3249: _temp3248=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp3230)->f1; goto _LL3247; _LL3247: _temp3246=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3230)->f2; goto _LL3245; _LL3245: _temp3244=(( struct Cyc_Absyn_ArrayType_struct*)
_temp3230)->f3; goto _LL3233;} else{ goto _LL3234;} _LL3234: if(( unsigned int)
_temp3230 > 4u?*(( int*) _temp3230) == Cyc_Absyn_PointerType: 0){ _LL3251:
_temp3250=(( struct Cyc_Absyn_PointerType_struct*) _temp3230)->f1; _LL3261:
_temp3260=( void*) _temp3250.elt_typ; goto _LL3259; _LL3259: _temp3258=( void*)
_temp3250.rgn_typ; goto _LL3257; _LL3257: _temp3256= _temp3250.nullable; goto
_LL3255; _LL3255: _temp3254= _temp3250.tq; goto _LL3253; _LL3253: _temp3252=
_temp3250.bounds; goto _LL3235;} else{ goto _LL3236;} _LL3236: if(( unsigned int)
_temp3230 > 4u?*(( int*) _temp3230) == Cyc_Absyn_FnType: 0){ _LL3263: _temp3262=((
struct Cyc_Absyn_FnType_struct*) _temp3230)->f1; _LL3277: _temp3276= _temp3262.tvars;
goto _LL3275; _LL3275: _temp3274= _temp3262.effect; goto _LL3273; _LL3273:
_temp3272=( void*) _temp3262.ret_typ; goto _LL3271; _LL3271: _temp3270=
_temp3262.args; goto _LL3269; _LL3269: _temp3268= _temp3262.c_varargs; goto
_LL3267; _LL3267: _temp3266= _temp3262.cyc_varargs; goto _LL3265; _LL3265:
_temp3264= _temp3262.attributes; goto _LL3237;} else{ goto _LL3238;} _LL3238:
if(( unsigned int) _temp3230 > 4u?*(( int*) _temp3230) == Cyc_Absyn_Evar: 0){
_LL3283: _temp3282=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp3230)->f1;
goto _LL3281; _LL3281: _temp3280=(( struct Cyc_Absyn_Evar_struct*) _temp3230)->f2;
goto _LL3279; _LL3279: _temp3278=(( struct Cyc_Absyn_Evar_struct*) _temp3230)->f3;
goto _LL3239;} else{ goto _LL3240;} _LL3240: if(( unsigned int) _temp3230 > 4u?*((
int*) _temp3230) == Cyc_Absyn_TypedefType: 0){ _LL3289: _temp3288=(( struct Cyc_Absyn_TypedefType_struct*)
_temp3230)->f1; goto _LL3287; _LL3287: _temp3286=(( struct Cyc_Absyn_TypedefType_struct*)
_temp3230)->f2; goto _LL3285; _LL3285: _temp3284=(( struct Cyc_Absyn_TypedefType_struct*)
_temp3230)->f3; goto _LL3241;} else{ goto _LL3242;} _LL3242: goto _LL3243;
_LL3233: { struct Cyc_List_List* _temp3292; void* _temp3294; struct Cyc_Absyn_Tqual
_temp3296; struct _tuple4 _temp3290= Cyc_Absynpp_to_tms( _temp3246, _temp3248);
_LL3297: _temp3296= _temp3290.f1; goto _LL3295; _LL3295: _temp3294= _temp3290.f2;
goto _LL3293; _LL3293: _temp3292= _temp3290.f3; goto _LL3291; _LL3291: { void*
tm; if( _temp3244 == 0){ tm=( void*) Cyc_Absyn_Carray_mod;} else{ tm=( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp3298=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp3298[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp3299; _temp3299.tag= Cyc_Absyn_ConstArray_mod;
_temp3299.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp3244); _temp3299;});
_temp3298;});} return({ struct _tuple4 _temp3300; _temp3300.f1= _temp3296;
_temp3300.f2= _temp3294; _temp3300.f3=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp3301=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3301->hd=( void*) tm; _temp3301->tl= _temp3292; _temp3301;}); _temp3300;});}}
_LL3235: { struct Cyc_List_List* _temp3304; void* _temp3306; struct Cyc_Absyn_Tqual
_temp3308; struct _tuple4 _temp3302= Cyc_Absynpp_to_tms( _temp3254, _temp3260);
_LL3309: _temp3308= _temp3302.f1; goto _LL3307; _LL3307: _temp3306= _temp3302.f2;
goto _LL3305; _LL3305: _temp3304= _temp3302.f3; goto _LL3303; _LL3303: { void*
ps;{ struct _tuple8 _temp3311=({ struct _tuple8 _temp3310; _temp3310.f1=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp3256))->v; _temp3310.f2=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp3252))->v; _temp3310;}); void* _temp3321;
void* _temp3323; struct Cyc_Absyn_Exp* _temp3325; void* _temp3327; int _temp3329;
void* _temp3331; void* _temp3333; struct Cyc_Absyn_Exp* _temp3335; void*
_temp3337; int _temp3339; void* _temp3341; void* _temp3343; void* _temp3345;
_LL3313: _LL3328: _temp3327= _temp3311.f1; if(( unsigned int) _temp3327 > 1u?*((
int*) _temp3327) == Cyc_Absyn_Eq_constr: 0){ _LL3330: _temp3329=( int)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp3327)->f1; if( _temp3329 == 1){ goto _LL3322;}
else{ goto _LL3315;}} else{ goto _LL3315;} _LL3322: _temp3321= _temp3311.f2; if((
unsigned int) _temp3321 > 1u?*(( int*) _temp3321) == Cyc_Absyn_Eq_constr: 0){
_LL3324: _temp3323=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3321)->f1;
if(( unsigned int) _temp3323 > 1u?*(( int*) _temp3323) == Cyc_Absyn_Upper_b: 0){
_LL3326: _temp3325=(( struct Cyc_Absyn_Upper_b_struct*) _temp3323)->f1; goto
_LL3314;} else{ goto _LL3315;}} else{ goto _LL3315;} _LL3315: _LL3338: _temp3337=
_temp3311.f1; if(( unsigned int) _temp3337 > 1u?*(( int*) _temp3337) == Cyc_Absyn_Eq_constr:
0){ _LL3340: _temp3339=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3337)->f1;
if( _temp3339 == 0){ goto _LL3332;} else{ goto _LL3317;}} else{ goto _LL3317;}
_LL3332: _temp3331= _temp3311.f2; if(( unsigned int) _temp3331 > 1u?*(( int*)
_temp3331) == Cyc_Absyn_Eq_constr: 0){ _LL3334: _temp3333=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp3331)->f1; if(( unsigned int) _temp3333 > 1u?*(( int*) _temp3333) == Cyc_Absyn_Upper_b:
0){ _LL3336: _temp3335=(( struct Cyc_Absyn_Upper_b_struct*) _temp3333)->f1; goto
_LL3316;} else{ goto _LL3317;}} else{ goto _LL3317;} _LL3317: _LL3346: _temp3345=
_temp3311.f1; goto _LL3342; _LL3342: _temp3341= _temp3311.f2; if(( unsigned int)
_temp3341 > 1u?*(( int*) _temp3341) == Cyc_Absyn_Eq_constr: 0){ _LL3344:
_temp3343=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp3341)->f1; if(
_temp3343 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL3318;} else{ goto _LL3319;}}
else{ goto _LL3319;} _LL3319: goto _LL3320; _LL3314: ps=( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp3347=( struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp3347[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp3348; _temp3348.tag=
Cyc_Absyn_Nullable_ps; _temp3348.f1= _temp3325; _temp3348;}); _temp3347;}); goto
_LL3312; _LL3316: ps=( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp3349=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp3349[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp3350; _temp3350.tag= Cyc_Absyn_NonNullable_ps; _temp3350.f1= _temp3335;
_temp3350;}); _temp3349;}); goto _LL3312; _LL3318: ps=( void*) Cyc_Absyn_TaggedArray_ps;
goto _LL3312; _LL3320: ps=( void*) Cyc_Absyn_TaggedArray_ps; goto _LL3312;
_LL3312:;}{ void* tm=( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp3353=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp3353[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp3354; _temp3354.tag=
Cyc_Absyn_Pointer_mod; _temp3354.f1=( void*) ps; _temp3354.f2=( void*) _temp3258;
_temp3354.f3= tq; _temp3354;}); _temp3353;}); return({ struct _tuple4 _temp3351;
_temp3351.f1= _temp3308; _temp3351.f2= _temp3306; _temp3351.f3=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp3352=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3352->hd=( void*) tm; _temp3352->tl= _temp3304;
_temp3352;}); _temp3351;});}}} _LL3237: if( ! Cyc_Absynpp_print_all_tvars){ if(
_temp3274 == 0? 1: Cyc_Absynpp_exists_temp_tvar_in_effect(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp3274))->v)){ _temp3274= 0; _temp3276= 0;}} else{ if( ! Cyc_Absynpp_dont_rewrite_temp_tvars){((
void(*)( void(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_iter)(
Cyc_Tcutil_rewrite_temp_tvar, _temp3276);}}{ struct Cyc_List_List* _temp3357;
void* _temp3359; struct Cyc_Absyn_Tqual _temp3361; struct _tuple4 _temp3355= Cyc_Absynpp_to_tms(
Cyc_Absyn_empty_tqual(), _temp3272); _LL3362: _temp3361= _temp3355.f1; goto
_LL3360; _LL3360: _temp3359= _temp3355.f2; goto _LL3358; _LL3358: _temp3357=
_temp3355.f3; goto _LL3356; _LL3356: { struct Cyc_List_List* tms= _temp3357; if(
_temp3264 != 0){ tms=({ struct Cyc_List_List* _temp3363=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3363->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp3364=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp3364[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp3365; _temp3365.tag= Cyc_Absyn_Attributes_mod;
_temp3365.f1= 0; _temp3365.f2= _temp3264; _temp3365;}); _temp3364;})); _temp3363->tl=
tms; _temp3363;});} tms=({ struct Cyc_List_List* _temp3366=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3366->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp3367=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp3367[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp3368; _temp3368.tag= Cyc_Absyn_Function_mod;
_temp3368.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp3369=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3369[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp3370; _temp3370.tag= Cyc_Absyn_WithTypes;
_temp3370.f1= _temp3270; _temp3370.f2= _temp3268; _temp3370.f3= _temp3266;
_temp3370.f4= _temp3274; _temp3370;}); _temp3369;})); _temp3368;}); _temp3367;}));
_temp3366->tl= tms; _temp3366;}); if( _temp3276 != 0){ tms=({ struct Cyc_List_List*
_temp3371=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3371->hd=( void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp3372=( struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp3372[ 0]=({ struct Cyc_Absyn_TypeParams_mod_struct
_temp3373; _temp3373.tag= Cyc_Absyn_TypeParams_mod; _temp3373.f1= _temp3276;
_temp3373.f2= 0; _temp3373.f3= 1; _temp3373;}); _temp3372;})); _temp3371->tl=
tms; _temp3371;});} return({ struct _tuple4 _temp3374; _temp3374.f1= _temp3361;
_temp3374.f2= _temp3359; _temp3374.f3= tms; _temp3374;});}} _LL3239: if(
_temp3280 == 0){ return({ struct _tuple4 _temp3375; _temp3375.f1= tq; _temp3375.f2=
t; _temp3375.f3= 0; _temp3375;});} else{ return Cyc_Absynpp_to_tms( tq,( void*)((
struct Cyc_Core_Opt*) _check_null( _temp3280))->v);} _LL3241: if( _temp3284 == 0?
1: ! Cyc_Absynpp_expand_typedefs){ return({ struct _tuple4 _temp3376; _temp3376.f1=
tq; _temp3376.f2= t; _temp3376.f3= 0; _temp3376;});} else{ return Cyc_Absynpp_to_tms(
tq,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp3284))->v);} _LL3243:
return({ struct _tuple4 _temp3377; _temp3377.f1= tq; _temp3377.f2= t; _temp3377.f3=
0; _temp3377;}); _LL3231:;} struct Cyc_PP_Doc* Cyc_Absynpp_tqtd2doc( struct Cyc_Absyn_Tqual
tq, void* t, struct Cyc_Core_Opt* dopt){ struct Cyc_List_List* _temp3380; void*
_temp3382; struct Cyc_Absyn_Tqual _temp3384; struct _tuple4 _temp3378= Cyc_Absynpp_to_tms(
tq, t); _LL3385: _temp3384= _temp3378.f1; goto _LL3383; _LL3383: _temp3382=
_temp3378.f2; goto _LL3381; _LL3381: _temp3380= _temp3378.f3; goto _LL3379;
_LL3379: _temp3380=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp3380); if( _temp3380 == 0? dopt == 0: 0){ return({ struct Cyc_PP_Doc*(*
_temp3386)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp3389= Cyc_Absynpp_tqual2doc(
_temp3384); struct Cyc_PP_Doc* _temp3390= Cyc_Absynpp_ntyp2doc( _temp3382);
struct Cyc_PP_Doc* _temp3387[ 2u]={ _temp3389, _temp3390}; struct _tagged_ptr0
_temp3388={ _temp3387, _temp3387, _temp3387 + 2u}; _temp3386( _temp3388);});}
else{ return({ struct Cyc_PP_Doc*(* _temp3391)( struct _tagged_ptr0)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp3394= Cyc_Absynpp_tqual2doc( _temp3384); struct Cyc_PP_Doc*
_temp3395= Cyc_Absynpp_ntyp2doc( _temp3382); struct Cyc_PP_Doc* _temp3396= Cyc_PP_text(({
unsigned char* _temp3398=" "; struct _tagged_string _temp3399; _temp3399.curr=
_temp3398; _temp3399.base= _temp3398; _temp3399.last_plus_one= _temp3398 + 2;
_temp3399;})); struct Cyc_PP_Doc* _temp3397= Cyc_Absynpp_dtms2doc( dopt == 0?
Cyc_PP_nil_doc():( struct Cyc_PP_Doc*)(( struct Cyc_Core_Opt*) _check_null( dopt))->v,
_temp3380); struct Cyc_PP_Doc* _temp3392[ 4u]={ _temp3394, _temp3395, _temp3396,
_temp3397}; struct _tagged_ptr0 _temp3393={ _temp3392, _temp3392, _temp3392 + 4u};
_temp3391( _temp3393);});}} struct Cyc_PP_Doc* Cyc_Absynpp_structfield2doc(
struct Cyc_Absyn_Structfield* f){ if( f->width != 0){ return({ struct Cyc_PP_Doc*(*
_temp3400)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp3403= Cyc_Absynpp_tqtd2doc(
f->tq,( void*) f->type,({ struct Cyc_Core_Opt* _temp3412=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3412->v=( void*) Cyc_PP_textptr(
f->name); _temp3412;})); struct Cyc_PP_Doc* _temp3404= Cyc_PP_text(({
unsigned char* _temp3410=":"; struct _tagged_string _temp3411; _temp3411.curr=
_temp3410; _temp3411.base= _temp3410; _temp3411.last_plus_one= _temp3410 + 2;
_temp3411;})); struct Cyc_PP_Doc* _temp3405= Cyc_Absynpp_exp2doc(( struct Cyc_Absyn_Exp*)((
struct Cyc_Core_Opt*) _check_null( f->width))->v); struct Cyc_PP_Doc* _temp3406=
Cyc_Absynpp_atts2doc( f->attributes); struct Cyc_PP_Doc* _temp3407= Cyc_PP_text(({
unsigned char* _temp3408=";"; struct _tagged_string _temp3409; _temp3409.curr=
_temp3408; _temp3409.base= _temp3408; _temp3409.last_plus_one= _temp3408 + 2;
_temp3409;})); struct Cyc_PP_Doc* _temp3401[ 5u]={ _temp3403, _temp3404,
_temp3405, _temp3406, _temp3407}; struct _tagged_ptr0 _temp3402={ _temp3401,
_temp3401, _temp3401 + 5u}; _temp3400( _temp3402);});} else{ return({ struct Cyc_PP_Doc*(*
_temp3413)( struct _tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp3416= Cyc_Absynpp_tqtd2doc(
f->tq,( void*) f->type,({ struct Cyc_Core_Opt* _temp3421=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3421->v=( void*) Cyc_PP_textptr(
f->name); _temp3421;})); struct Cyc_PP_Doc* _temp3417= Cyc_Absynpp_atts2doc( f->attributes);
struct Cyc_PP_Doc* _temp3418= Cyc_PP_text(({ unsigned char* _temp3419=";";
struct _tagged_string _temp3420; _temp3420.curr= _temp3419; _temp3420.base=
_temp3419; _temp3420.last_plus_one= _temp3419 + 2; _temp3420;})); struct Cyc_PP_Doc*
_temp3414[ 3u]={ _temp3416, _temp3417, _temp3418}; struct _tagged_ptr0 _temp3415={
_temp3414, _temp3414, _temp3414 + 3u}; _temp3413( _temp3415);});}} struct Cyc_PP_Doc*
Cyc_Absynpp_structfields2doc( struct Cyc_List_List* fields){ return(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Structfield*), struct _tagged_string
sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_structfield2doc,({
unsigned char* _temp3422=""; struct _tagged_string _temp3423; _temp3423.curr=
_temp3422; _temp3423.base= _temp3422; _temp3423.last_plus_one= _temp3422 + 1;
_temp3423;}), fields);} struct Cyc_PP_Doc* Cyc_Absynpp_tunionfield2doc( struct
Cyc_Absyn_Tunionfield* f){ return({ struct Cyc_PP_Doc*(* _temp3424)( struct
_tagged_ptr0)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp3427= Cyc_Absynpp_scope2doc((
void*) f->sc); struct Cyc_PP_Doc* _temp3428= Cyc_Absynpp_typedef_name2doc( f->name);
struct Cyc_PP_Doc* _temp3429= f->tvs == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_ktvars2doc(
f->tvs); struct Cyc_PP_Doc* _temp3430= f->typs == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc(
f->typs); struct Cyc_PP_Doc* _temp3425[ 4u]={ _temp3427, _temp3428, _temp3429,
_temp3430}; struct _tagged_ptr0 _temp3426={ _temp3425, _temp3425, _temp3425 + 4u};
_temp3424( _temp3426);});} struct Cyc_PP_Doc* Cyc_Absynpp_tunionfields2doc(
struct Cyc_List_List* fields){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Tunionfield*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_tunionfield2doc,({ unsigned char* _temp3431=",";
struct _tagged_string _temp3432; _temp3432.curr= _temp3431; _temp3432.base=
_temp3431; _temp3432.last_plus_one= _temp3431 + 2; _temp3432;}), fields);} void
Cyc_Absynpp_decllist2file( struct Cyc_List_List* tdl, struct Cyc_Stdio___sFILE*
f){ for( 0; tdl != 0; tdl=(( struct Cyc_List_List*) _check_null( tdl))->tl){ Cyc_PP_file_of_doc(
Cyc_Absynpp_decl2doc(( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*)
_check_null( tdl))->hd), 72, f); fprintf( sfile_to_file( f),"\n");}} struct
_tagged_string Cyc_Absynpp_decllist2string( struct Cyc_List_List* tdl){ return
Cyc_PP_string_of_doc( Cyc_PP_seql(({ unsigned char* _temp3433=""; struct
_tagged_string _temp3434; _temp3434.curr= _temp3433; _temp3434.base= _temp3433;
_temp3434.last_plus_one= _temp3433 + 1; _temp3434;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Decl*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_decl2doc, tdl)), 72);} struct _tagged_string Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp* e){ return Cyc_PP_string_of_doc( Cyc_Absynpp_exp2doc( e),
72);} struct _tagged_string Cyc_Absynpp_stmt2string( struct Cyc_Absyn_Stmt* s){
return Cyc_PP_string_of_doc( Cyc_Absynpp_stmt2doc( s), 72);} struct
_tagged_string Cyc_Absynpp_typ2string( void* t){ Cyc_Absynpp_dont_rewrite_temp_tvars=
1;{ struct _tagged_string _temp3435= Cyc_PP_string_of_doc( Cyc_Absynpp_typ2doc(
t), 72); goto _LL3436; _LL3436: Cyc_Absynpp_dont_rewrite_temp_tvars= 0; return
_temp3435;}} struct _tagged_string Cyc_Absynpp_qvar2string( struct _tuple0* v){
return Cyc_PP_string_of_doc( Cyc_Absynpp_qvar2doc( v), 72);} struct
_tagged_string Cyc_Absynpp_prim2string( void* p){ return Cyc_PP_string_of_doc(
Cyc_Absynpp_prim2doc( p), 72);} struct _tagged_string Cyc_Absynpp_pat2string(
struct Cyc_Absyn_Pat* p){ return Cyc_PP_string_of_doc( Cyc_Absynpp_pat2doc( p),
72);} struct _tagged_string Cyc_Absynpp_scope2string( void* sc){ void* _temp3437=
sc; _LL3439: if( _temp3437 ==( void*) Cyc_Absyn_Static){ goto _LL3440;} else{
goto _LL3441;} _LL3441: if( _temp3437 ==( void*) Cyc_Absyn_Public){ goto _LL3442;}
else{ goto _LL3443;} _LL3443: if( _temp3437 ==( void*) Cyc_Absyn_Extern){ goto
_LL3444;} else{ goto _LL3445;} _LL3445: if( _temp3437 ==( void*) Cyc_Absyn_ExternC){
goto _LL3446;} else{ goto _LL3447;} _LL3447: if( _temp3437 ==( void*) Cyc_Absyn_Abstract){
goto _LL3448;} else{ goto _LL3438;} _LL3440: return({ unsigned char* _temp3449="static";
struct _tagged_string _temp3450; _temp3450.curr= _temp3449; _temp3450.base=
_temp3449; _temp3450.last_plus_one= _temp3449 + 7; _temp3450;}); _LL3442: return({
unsigned char* _temp3451="public"; struct _tagged_string _temp3452; _temp3452.curr=
_temp3451; _temp3452.base= _temp3451; _temp3452.last_plus_one= _temp3451 + 7;
_temp3452;}); _LL3444: return({ unsigned char* _temp3453="extern"; struct
_tagged_string _temp3454; _temp3454.curr= _temp3453; _temp3454.base= _temp3453;
_temp3454.last_plus_one= _temp3453 + 7; _temp3454;}); _LL3446: return({
unsigned char* _temp3455="extern \"C\""; struct _tagged_string _temp3456;
_temp3456.curr= _temp3455; _temp3456.base= _temp3455; _temp3456.last_plus_one=
_temp3455 + 11; _temp3456;}); _LL3448: return({ unsigned char* _temp3457="abstract";
struct _tagged_string _temp3458; _temp3458.curr= _temp3457; _temp3458.base=
_temp3457; _temp3458.last_plus_one= _temp3457 + 9; _temp3458;}); _LL3438:;}